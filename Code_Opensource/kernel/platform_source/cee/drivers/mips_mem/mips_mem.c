/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * mips.c
 *
 * for hisilicon efficinecy control algorithm mips load tracking.
 *
 * Copyright (c) 2023-2024 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#include <platform_include/cee/linux/mips_mem.h>
#include <platform_include/cee/linux/mips.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/topology.h>
#include <linux/cpu.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <trace/events/power.h>
#include <linux/sched/cpufreq.h>
#include <linux/cpu_pm.h>
#include <../../../kernel/sched/sched.h>

#define CREATE_TRACE_POINTS
#include <trace/events/mips_mem.h>

static struct kobject *g_mips_mem_global_kobject;

#define KHZ_PER_MHZ 1000
#define HZ_PER_MHZ 1000000

static DEFINE_SPINLOCK(state_lock);
struct core_dev_map {
	unsigned int core_mhz;
	unsigned long *target_freq;
};

struct target_ratio {
	u32 *ratios;
	int nratios;
};

struct vote_reg {
	void __iomem *reg; /* dev frequency vote register */
	u32 mask;
	u8 bit;
	u32 max_freq;
};

struct dev_info {
	u32 nr_levels;
	struct target_ratio *target_ratios;
	unsigned int *ipc_min_sample_time_ns;
	int nipc_min_sample_time;
	u64 min_sample_time;
	int next_freq;
	u64 update_time;
	unsigned int mem_load_window_size_ns;
	struct vote_reg *hw_vote;
	struct core_dev_map *freq_map;
	unsigned int map_len;
	bool enable;
	struct kobject kobj;
};

struct mipsmem_cluster_policy {
	cpumask_t cpu_mask;
	struct dev_info *ddr_dev;
	struct dev_info *l3c_dev;
	struct kobject kobj;
};

struct mipsmem_cpu_info {
	int cpu;
	bool enable;
	struct mipsmem_cluster_policy *policy;
	struct notifier_block mips_cpu_idle_nb;
	u32 next_l3_freq;
	u32 next_ddr_freq;
	u64 l3_min_sample_time;
	u64 ddr_min_sample_time;
};

static DEFINE_PER_CPU(struct mipsmem_cpu_info, cpu_info);

#define DEFAULT_TARGET_RATIO 600
#define DEFAULT_IPC_MIN_SAMPLE_TIME 4000000
#define DEFAULT_MEM_WINDOW_SIZE	4000000
static unsigned int default_target_ratios[] = {DEFAULT_TARGET_RATIO};
static unsigned int none_target_ratios[] = {0};
static unsigned int default_ipc_min_sample_time[] = {DEFAULT_IPC_MIN_SAMPLE_TIME};

static u32 core_to_dev_freq(struct dev_info *info, unsigned int level, u16 coref)
{
	struct core_dev_map *map = info->freq_map;
	unsigned long freq = 0;
	unsigned int i;

	if (unlikely(map == NULL))
		goto out;

	for (i = 0; i < info->map_len - 1 && map[i].core_mhz < coref; i++)
		;
	freq = map[i].target_freq[level] / HZ_PER_MHZ;

out:
	pr_debug("freq: %lu -> dev: %lu\n", coref, freq);
	return freq;
}

static bool mem_enable(struct mipsmem_cpu_info *mem_cpu)
{
	struct mipsmem_cluster_policy *mem_policy = mem_cpu->policy;

	if (!mem_cpu->enable)
		return false;
	if (!mips_enable)
		return false;
	if (!mem_policy)
		return false;
	if (!mem_policy->l3c_dev || !mem_policy->ddr_dev)
		return false;

	return mem_policy->l3c_dev->enable && mem_policy->ddr_dev->enable;
}
static void print_mem_freq(void)
{
	int cpu;
	struct mipsmem_cpu_info *mem_cpu = NULL;
	int max_ddrfreq = 0;
	int max_l3cfreq = 0;
	int l3_id = 0;
	int ddr_id = 0;

	for_each_online_cpu(cpu) {
		mem_cpu = &per_cpu(cpu_info, cpu);
		if (mem_cpu->next_ddr_freq > max_ddrfreq) {
			max_ddrfreq = mem_cpu->next_ddr_freq;
			ddr_id = cpu;
		}
		if (mem_cpu->next_l3_freq > max_l3cfreq) {
			max_l3cfreq = mem_cpu->next_l3_freq;
			l3_id = cpu;
		}
	}
	trace_clock_set_rate("mipsddrfreq", max_ddrfreq, ddr_id);
	trace_clock_set_rate("mipsl3cfreq", max_l3cfreq, l3_id);
}

static void set_hw_vote(struct dev_info *info, int max_freq, u64 min_sample_time)
{
	u32 value;
	u64 now;

	now = walt_ktime_clock();
	if (max_freq != info->next_freq && now - info->update_time > min_sample_time) {
		info->next_freq = max_freq;
		value = (max_freq << info->hw_vote->bit) | info->hw_vote->mask;
		info->update_time = now;
		writel(value, info->hw_vote->reg);
		print_mem_freq();
	}
}

static void change_mem_freq(struct mipsmem_cpu_info *cpum)
{
	int max_l3_freq = 0;
	int max_ddr_freq = 0;
	int cpu;
	struct mipsmem_cluster_policy *mem_policy = cpum->policy;
	struct mipsmem_cpu_info *mem_cpu = NULL;
	unsigned long flags;

	for_each_cpu(cpu, &mem_policy->cpu_mask) {
		mem_cpu = &per_cpu(cpu_info, cpu);
		max_l3_freq = max_t(u32, max_l3_freq, mem_cpu->next_l3_freq);
		max_ddr_freq = max_t(u32, max_ddr_freq, mem_cpu->next_ddr_freq);
	}
	set_hw_vote(mem_policy->l3c_dev, max_l3_freq, cpum->l3_min_sample_time);
	set_hw_vote(mem_policy->ddr_dev, max_ddr_freq, cpum->ddr_min_sample_time);
}

static u32 freq_to_targetratio(u32 freq, u32 *ratios, int len)
{
	int i;

	for (i = 0; i < len - 1 && freq >= ratios[i + 1]; i += 2)
		;
	return ratios[i];
}

static u32 get_max_possible_freq(struct dev_info *dev, unsigned int level,
				u32 ratio, u32 freq)
{
	int i;
	unsigned int row;
	u32 next_freq;
	u32 max_possible_freq = 0;
	int index = -1;
	struct core_dev_map *map = dev->freq_map;
	struct target_ratio *target = &dev->target_ratios[level];

	for (i = target->nratios - 1; i >= 0; i -= 2) {
		if (ratio <= target->ratios[i]) {
			index = i;
			break;
		}
	}

	if (index != -1) {
		index++;
		if (index >= target->nratios)
			return 0;
		next_freq = target->ratios[index];
		if (next_freq <= freq) {
			for (row = 0; row < dev->map_len &&
			     map[row].core_mhz < next_freq; row++)
				max_possible_freq = map[row].core_mhz;
			return max_possible_freq;
		}
	}
	return 0;
}

static u64 ipc_to_min_sample_time(struct dev_info *info, unsigned int ipc)
{
	unsigned int ret;
	int i;

	for (i = 0; i < info->nipc_min_sample_time - 1 &&
	     ipc >= info->ipc_min_sample_time_ns[i + 1]; i += 2)
		;

	ret = info->ipc_min_sample_time_ns[i];

	return ret;
}

static u32 mips_mem_get_freq(struct dev_info *info, u32 ratio, u32 core_freq)
{
	u32 target_ratio_ceil;
	u32 freq, vote = 0;
	unsigned int level;
	struct target_ratio *target;

	if (info == NULL || !info->enable)
		return 0;

	/* Each level selects independently; hardware receives one maximum MHz vote. */
	for (level = 0; level < info->nr_levels; level++) {
		target = &info->target_ratios[level];
		target_ratio_ceil = freq_to_targetratio(core_freq, target->ratios,
						      target->nratios);
		if (ratio <= target_ratio_ceil)
			freq = core_freq;
		else
			freq = get_max_possible_freq(info, level, ratio, core_freq);
		vote = max(vote, core_to_dev_freq(info, level, freq));
	}
	return vote;
}

void check_l3_ddr_freq_change(int cpu)
{
	int freq;
	struct mipsmem_cpu_info *mem_cpu = &per_cpu(cpu_info, cpu);
	struct mipsmem_cluster_policy *mem_policy = mem_cpu->policy;
	u32 l3_ratio = UINT_MAX;
	u32 ddr_ratio = UINT_MAX;
	int ipc;
	struct pcounts pcount;
	u64 now = walt_ktime_clock();
	u64 mem_window_size;
	int nr_window, nr_mem_load_window;
	unsigned long flags;

	spin_lock_irqsave(&state_lock, flags);
	if (!mem_enable(mem_cpu))
		goto unlock;

	mem_window_size = mem_policy->ddr_dev->mem_load_window_size_ns;
	nr_window = update_mips_window_start(&cpu_rq(cpu)->mips_mem_window_start,
					   mem_window_size, now);
	if (!nr_window)
		goto unlock;

	nr_mem_load_window = mem_window_size / mips_window_size;
	if (!nr_mem_load_window)
		goto unlock;

	get_mips_mem_info(cpu, &pcount, nr_mem_load_window);
	if (pcount.l2refill != 0)
		l3_ratio = pcount.inst / pcount.l2refill;
	if (pcount.l3refill != 0)
		ddr_ratio = pcount.inst / pcount.l3refill;
	freq = pcount.freq / KHZ_PER_MHZ;

	ipc = pcount.cycle ? pcount.inst * 100 / pcount.cycle : 0;
	mem_cpu->next_l3_freq = mips_mem_get_freq(mem_policy->l3c_dev, l3_ratio, freq);
	mem_cpu->next_ddr_freq = mips_mem_get_freq(mem_policy->ddr_dev, ddr_ratio, freq);
	mem_cpu->l3_min_sample_time = ipc_to_min_sample_time(mem_policy->l3c_dev, ipc);
	mem_cpu->ddr_min_sample_time = ipc_to_min_sample_time(mem_policy->ddr_dev, ipc);
	trace_check_l3_ddr_freq_change(cpu, &pcount, l3_ratio, ddr_ratio,
				       mem_cpu->next_l3_freq, mem_cpu->next_ddr_freq);
	change_mem_freq(mem_cpu);
unlock:
	spin_unlock_irqrestore(&state_lock, flags);
}

static int get_mask_from_dev_handle(struct platform_device *pdev,
				    cpumask_t *mask)
{
	struct device_node *np = pdev->dev.of_node;
	struct device_node *cpu_np = NULL;
	struct device_node *mon_np = NULL;
	int i, cpu;

	cpumask_clear(mask);

	for_each_possible_cpu(cpu) {
		cpu_np = of_get_cpu_node(cpu, NULL);
		if (!cpu_np) {
			pr_err("%s: failed to get cpu%d node\n",
			       __func__, cpu);
			return -ENOENT;
		}

		/* Get mipsmem monitor descriptor node */
		for (i = 0; ; i++) {
			mon_np = of_parse_phandle(cpu_np, "mips-mem-monitors", i);
			if (!mon_np)
				break;

			/* CPUs are sharing mipsmem monitor node */
			if (np == mon_np) {
				cpumask_set_cpu(cpu, mask);
				of_node_put(mon_np);
				break;
			}
			of_node_put(mon_np);
		}

		of_node_put(cpu_np);
	}

	if (cpumask_empty(mask))
		return -ENOENT;

	return 0;
}

static ssize_t store_freq_map(struct dev_info *info, const char *buf, size_t count)
{
	char *input, *cursor, *token;
	unsigned long *targets, flags;
	u32 core_freq = 0, idx = 0;
	unsigned int arg = 0;
	ssize_t ret = -EINVAL;
	struct core_dev_map *map;

	input = kstrndup(buf, count, GFP_KERNEL);
	if (!input)
		return -ENOMEM;
	targets = kcalloc(info->nr_levels, sizeof(*targets), GFP_KERNEL);
	if (!targets) {
		kfree(input);
		return -ENOMEM;
	}
	cursor = input;
	while ((token = strsep(&cursor, " \t\n"))) {
		if (!*token)
			continue;
		if (arg == 0) {
			if (kstrtouint(token, 10, &idx) || idx >= info->map_len)
				goto out;
		} else if (arg == 1) {
			if (kstrtouint(token, 10, &core_freq) || !core_freq ||
			    core_freq > USHRT_MAX)
				goto out;
		} else {
			if (arg >= info->nr_levels + 2 ||
			    kstrtoul(token, 10, &targets[arg - 2]) ||
			    targets[arg - 2] / HZ_PER_MHZ > info->hw_vote->max_freq)
				goto out;
		}
		arg++;
	}
	if (arg != info->nr_levels + 2)
		goto out;

	spin_lock_irqsave(&state_lock, flags);
	map = info->freq_map;
	if ((!idx || core_freq > map[idx - 1].core_mhz) &&
	    (idx == info->map_len - 1 || core_freq < map[idx + 1].core_mhz)) {
		map[idx].core_mhz = core_freq;
		memcpy(map[idx].target_freq, targets,
		       info->nr_levels * sizeof(*targets));
		ret = count;
	}
	spin_unlock_irqrestore(&state_lock, flags);
out:
	kfree(targets);
	kfree(input);
	return ret;
}

static ssize_t show_freq_map(const struct dev_info *info, char *buf)
{
	struct core_dev_map *map = info->freq_map;
	unsigned int cnt = 0;
	unsigned int row, level;

	cnt += scnprintf(buf, PAGE_SIZE, "Core freq (MHz)\tDevice BW\n");

	for (row = 0; row < info->map_len && cnt < PAGE_SIZE; row++) {
		cnt += scnprintf(buf + cnt, PAGE_SIZE - cnt, "%15u", map[row].core_mhz);
		for (level = 0; level < info->nr_levels; level++)
			cnt += scnprintf(buf + cnt, PAGE_SIZE - cnt, "\t%9lu",
					 map[row].target_freq[level]);
		cnt += scnprintf(buf + cnt, PAGE_SIZE - cnt, "\n");
	}
	if (cnt < PAGE_SIZE)
		cnt += scnprintf(buf + cnt, PAGE_SIZE - cnt, "\n");

	return cnt;
}

static unsigned int *get_tokenized_data(const char *buf, int *num_tokens)
{
	const char *cp;
	unsigned int i;
	unsigned int ntokens = 1;
	unsigned int *tokenized_data = NULL;
	char *input, *cursor, *token;
	int err = -EINVAL;

	cp = buf;
	while ((cp = strpbrk(cp + 1, " :")))
		ntokens++;

	if (!(ntokens & 0x1))
		goto err;

	tokenized_data = kmalloc(ntokens * sizeof(unsigned int), GFP_KERNEL);
	if (tokenized_data == NULL) {
		err = -ENOMEM;
		goto err;
	}

	input = kstrdup(buf, GFP_KERNEL);
	if (!input) {
		err = -ENOMEM;
		goto err_kfree;
	}
	cursor = input;
	for (i = 0; i < ntokens; i++) {
		token = strsep(&cursor, " :");
		if (kstrtouint(token, 10, &tokenized_data[i])) {
			kfree(input);
			goto err_kfree;
		}
	}
	kfree(input);

	*num_tokens = ntokens;
	return tokenized_data;

err_kfree:
	kfree(tokenized_data);
err:
	return ERR_PTR(err);
}

static void free_target_ratios(struct target_ratio *targets, unsigned int levels)
{
	unsigned int level;

	for (level = 0; level < levels; level++)
		if (targets[level].ratios != default_target_ratios &&
		    targets[level].ratios != none_target_ratios)
			kfree(targets[level].ratios);
	kfree(targets);
}

static ssize_t store_target_ratio(struct dev_info *info, const char *buf, size_t count)
{
	struct target_ratio *targets, *old_targets;
	unsigned int level = 0;
	unsigned long flags;
	char *input, *cursor, *token;
	ssize_t ret = -EINVAL;
	int i;

	input = kstrndup(buf, count, GFP_KERNEL);
	if (!input)
		return -ENOMEM;
	targets = kcalloc(info->nr_levels, sizeof(*targets), GFP_KERNEL);
	if (!targets) {
		kfree(input);
		return -ENOMEM;
	}
	cursor = input;
	while ((token = strsep(&cursor, ","))) {
		token = strim(token);
		if (level == info->nr_levels || !*token)
			goto out;
		targets[level].ratios = get_tokenized_data(token, &targets[level].nratios);
		if (IS_ERR(targets[level].ratios)) {
			ret = PTR_ERR(targets[level].ratios);
			targets[level].ratios = NULL;
			goto out;
		}
		for (i = 1; i < targets[level].nratios; i += 2) {
			targets[level].ratios[i] /= KHZ_PER_MHZ;
			if (!targets[level].ratios[i] ||
			    (i > 1 && targets[level].ratios[i] <= targets[level].ratios[i - 2]))
				goto out;
		}
		level++;
	}
	/* A factory scalar write configures level zero, not every level. */
	while (level < info->nr_levels) {
		targets[level].ratios = none_target_ratios;
		targets[level++].nratios = ARRAY_SIZE(none_target_ratios);
	}
	spin_lock_irqsave(&state_lock, flags);
	old_targets = info->target_ratios;
	info->target_ratios = targets;
	spin_unlock_irqrestore(&state_lock, flags);
	free_target_ratios(old_targets, info->nr_levels);
	kfree(input);
	return count;
out:
	free_target_ratios(targets, info->nr_levels);
	kfree(input);
	return ret;
}

static ssize_t show_target_ratio(const struct dev_info *info, char *buf)
{
	unsigned int i, level;
	ssize_t ret = 0;
	const struct target_ratio *target;

	for (level = 0; level < info->nr_levels && ret < PAGE_SIZE - 1; level++) {
		target = &info->target_ratios[level];
		for (i = 0; i < target->nratios && ret < PAGE_SIZE - 1; i++)
			ret += scnprintf(buf + ret, PAGE_SIZE - ret, "%u%s",
					 (i & 0x1) ? target->ratios[i] * KHZ_PER_MHZ : target->ratios[i],
					 (i & 0x1) ? ":" : " ");
		buf[ret - 1] = level + 1 == info->nr_levels ? '\n' : ',';
	}
	buf[ret - 1] = '\n';
	return ret;
}

static ssize_t show_mipsmem_enable(const struct dev_info *info, char *buf)
{
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%u\n", info->enable);
}

#define TRANS_BASE	10
static ssize_t store_mipsmem_enable(struct dev_info *info, const char *buf, size_t count)
{
	int  val;
	unsigned long flags;

	if (kstrtouint(buf, TRANS_BASE, &val) != 0)
		return -EINVAL;
	spin_lock_irqsave(&state_lock, flags);
	info->enable = !!val;
	if (!info->enable)
		set_hw_vote(info, 0, 0);
	spin_unlock_irqrestore(&state_lock, flags);
	return count;
}

static ssize_t show_ipc_min_sample_time(const struct dev_info *info, char *buf)
{
	ssize_t ret = 0;
	int i;

	for (i = 0; i < info->nipc_min_sample_time; i++)
		ret += scnprintf(buf + ret, PAGE_SIZE - ret, "%u%s", info->ipc_min_sample_time_ns[i],
				 i & 0x1 ? ":" : " ");

	scnprintf(buf + ret - 1, PAGE_SIZE - ret + 1, "\n");
	return ret;
}

static ssize_t store_ipc_min_sample_time(struct dev_info *info, const char *buf, size_t count)
{
	unsigned int *new_ipc_min_sample_time, *old_ipc_min_sample_time;
	unsigned long flags;
	int ntokens;
	int i;

	new_ipc_min_sample_time = cpufreq_get_tokenized_data(buf, &ntokens);
	if (IS_ERR(new_ipc_min_sample_time))
		return PTR_ERR(new_ipc_min_sample_time);

	for (i = 0; i < ntokens; i++) {
		if (new_ipc_min_sample_time[i] == 0) {
			kfree(new_ipc_min_sample_time);
			return -EINVAL;
		}
	}

	spin_lock_irqsave(&state_lock, flags);
	old_ipc_min_sample_time = info->ipc_min_sample_time_ns;
	info->ipc_min_sample_time_ns = new_ipc_min_sample_time;
	info->nipc_min_sample_time = ntokens;
	spin_unlock_irqrestore(&state_lock, flags);
	if (old_ipc_min_sample_time != default_ipc_min_sample_time)
		kfree(old_ipc_min_sample_time);

	return count;
}

static ssize_t show_mem_load_window_size_ns(const struct dev_info *info, char *buf)
{
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%u\n", info->mem_load_window_size_ns);
}

static ssize_t store_mem_load_window_size_ns(struct dev_info *info, const char *buf, size_t count)
{
	unsigned int val;
	unsigned long flags;

	if (kstrtouint(buf, TRANS_BASE, &val) != 0)
		return -EINVAL;
	if (val < MIN_MIPS_WINDOW_SIZE || val > MAX_MIPS_WINDOW_SIZE)
		return -EINVAL;
	spin_lock_irqsave(&state_lock, flags);
	info->mem_load_window_size_ns = val;
	spin_unlock_irqrestore(&state_lock, flags);
	return count;
}

#define to_cluster_data(k) container_of(k, struct dev_info, kobj)
#define to_attr(a) container_of(a, struct mipsmem_attr, attr)
struct mipsmem_attr {
	struct attribute attr;
	ssize_t (*show)(const struct dev_info *, char *);
	ssize_t (*store)(struct dev_info *, const char *, size_t count);
};

#define mipsmem_attr_ro(_name)		\
	static struct mipsmem_attr _name = 	\
	__ATTR(_name, 0440, show_##_name, NULL)

#define mipsmem_attr_rw(_name)		\
	static struct mipsmem_attr _name = 		\
	__ATTR(_name, 0640, show_##_name, store_##_name)

mipsmem_attr_rw(freq_map);
mipsmem_attr_rw(target_ratio);
mipsmem_attr_rw(mipsmem_enable);
mipsmem_attr_rw(ipc_min_sample_time);
mipsmem_attr_rw(mem_load_window_size_ns);

static ssize_t show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct dev_info *data = to_cluster_data(kobj);
	struct mipsmem_attr *mattr = to_attr(attr);
	ssize_t ret = -EIO;
	unsigned long flags;

	spin_lock_irqsave(&state_lock, flags);
	if (mattr->show != NULL)
		ret = mattr->show(data, buf);
	spin_unlock_irqrestore(&state_lock, flags);

	return ret;
}

static ssize_t store(struct kobject *kobj, struct attribute *attr,
		     const char *buf, size_t count)
{
	struct dev_info *data = to_cluster_data(kobj);
	struct mipsmem_attr *mattr = to_attr(attr);
	ssize_t ret = -EIO;

	if (mattr->store != NULL)
		ret = mattr->store(data, buf, count);

	return ret;
}

static struct attribute *mips_attrs[] = {
	&freq_map.attr,
	&target_ratio.attr,
	&mipsmem_enable.attr,
	&ipc_min_sample_time.attr,
	&mem_load_window_size_ns.attr,
	NULL,
};

static const struct sysfs_ops sysfs_ops = {
	.show   = show,
	.store  = store,
};

static void mipsmem_dev_release(struct kobject *kobj)
{
	struct dev_info *info = container_of(kobj, struct dev_info, kobj);

	free_target_ratios(info->target_ratios, info->nr_levels);
	if (info->ipc_min_sample_time_ns != default_ipc_min_sample_time)
		kfree(info->ipc_min_sample_time_ns);
	kfree(info);
}

static struct kobj_type ktype_mips = {
	.release = mipsmem_dev_release,
	.sysfs_ops	  = &sysfs_ops,
	.default_attrs  = mips_attrs,
};

static struct attribute *default_attrs[] = {
	NULL,
};

static void mipsmem_cluster_release(struct kobject *kobj)
{
	kfree(container_of(kobj, struct mipsmem_cluster_policy, kobj));
}

static struct kobj_type ktype_default = {
	.release = mipsmem_cluster_release,
	.default_attrs  = default_attrs,
};

static void clear_mem_freq(struct mipsmem_cpu_info *mem_cpu)
{
	mem_cpu->next_ddr_freq = 0;
	mem_cpu->next_l3_freq = 0;
	mem_cpu->l3_min_sample_time = 0;
	mem_cpu->ddr_min_sample_time = 0;
}

static void mips_mem_idle_clear_freq(struct mipsmem_cpu_info *mem_cpu)
{
	unsigned long flags;

	spin_lock_irqsave(&state_lock, flags);
	if (!mem_enable(mem_cpu))
		goto unlock;

	clear_mem_freq(mem_cpu);
	change_mem_freq(mem_cpu);
unlock:
	spin_unlock_irqrestore(&state_lock, flags);
}
void set_mips_mem_state(bool enabled)
{
	int cpu;
	unsigned long flags;
	struct dev_info *ddr_info;
	struct dev_info *l3_info;

	spin_lock_irqsave(&state_lock, flags);
	for_each_online_cpu(cpu) {
		struct mipsmem_cpu_info *mem_cpu = &per_cpu(cpu_info, cpu);

		if (!mem_cpu->policy || !mem_cpu->policy->ddr_dev ||
		    !mem_cpu->policy->l3c_dev)
			continue;
		if (cpu == cpumask_first(&mem_cpu->policy->cpu_mask)) {
			ddr_info = mem_cpu->policy->ddr_dev;
			l3_info = mem_cpu->policy->l3c_dev;
			ddr_info->enable = enabled;
			l3_info->enable = enabled;
			if (!enabled) {
				set_hw_vote(l3_info, 0, 0);
				set_hw_vote(ddr_info, 0, 0);
			}
		}
	}
	spin_unlock_irqrestore(&state_lock, flags);
}

static int mips_mem_idle_notif(struct notifier_block *nb, unsigned long action,
			       void *data __maybe_unused)
{
	int cpu = smp_processor_id();
	struct mipsmem_cpu_info *mem_cpu = &per_cpu(cpu_info, cpu);

	switch (action) {
	case CPU_PM_ENTER:
		mips_mem_idle_clear_freq(mem_cpu);
		break;

	case CPU_PM_ENTER_FAILED:
		break;
	case CPU_PM_EXIT:
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct mipsmem_cluster_policy *mipsmem_cluster_alloc_and_init(struct platform_device *pdev,
								     cpumask_t *cpus)
{
	struct mipsmem_cluster_policy *policy = NULL;
	int ret = 0;
	int cpu, linked_cpu;
	unsigned long flags;
	struct device *dev = &pdev->dev;
	struct device *cpu_dev;
	struct mipsmem_cpu_info *mem_cpu = &per_cpu(cpu_info, cpumask_first(cpus));

	policy = mem_cpu->policy;
	if (policy)
		return policy;
	/* Per-CPU users share this policy across monitor probe failures. */
	policy = kzalloc(sizeof(*policy), GFP_KERNEL);
	if (!policy) {
		dev_err(dev, "mips mem alloc cluster err\n");
		return NULL;
	}
	cpumask_copy(&policy->cpu_mask, cpus);

	ret = kobject_init_and_add(&policy->kobj, &ktype_default,
				   g_mips_mem_global_kobject, "cluster%u",
				   topology_physical_package_id(cpumask_first(cpus)));
	if (ret) {
		dev_err(dev, "mips: failed to init cluster->kobj: %d\n", ret);
		kobject_put(&policy->kobj);
		return NULL;
	}

	for_each_cpu(cpu, cpus) {
		cpu_dev = get_cpu_device(cpu);
		ret = sysfs_create_link(&cpu_dev->kobj, &policy->kobj, "mips_mem");
		if (ret) {
			dev_err(dev, "mips: symlink creation cpu%d failed\n", cpu);
			goto error;
		}
	}

	for_each_cpu(cpu, cpus) {
		mem_cpu = &per_cpu(cpu_info, cpu);
		mem_cpu->mips_cpu_idle_nb.notifier_call = mips_mem_idle_notif;
		cpu_pm_register_notifier(&mem_cpu->mips_cpu_idle_nb);
	}
	spin_lock_irqsave(&state_lock, flags);
	for_each_cpu(cpu, cpus) {
		mem_cpu = &per_cpu(cpu_info, cpu);
		mem_cpu->policy = policy;
		mem_cpu->cpu = cpu;
		mem_cpu->enable = true;
	}
	spin_unlock_irqrestore(&state_lock, flags);
	return policy;
error:
	for_each_cpu(linked_cpu, cpus) {
		if (linked_cpu == cpu)
			break;
		cpu_dev = get_cpu_device(linked_cpu);
		sysfs_remove_link(&cpu_dev->kobj, "mips_mem");
	}
	kobject_put(&policy->kobj);
	return NULL;
}

static struct core_dev_map *init_core_dev_map(struct device *dev,
					      const char *prop_name, struct dev_info *info)
{
	int len, nf, i, j;
	unsigned int level, cols;
	u32 data;
	struct core_dev_map *tbl = NULL;
	int ret;

	if (of_find_property(dev->of_node, prop_name, &len) == NULL)
		return NULL;

	len /= sizeof(data);

	/* One CPU-frequency column followed by nr-level independent targets. */
	if (!info->nr_levels || info->nr_levels >= len)
		return NULL;
	cols = info->nr_levels + 1;
	if ((len % cols) != 0 || len == 0)
		return NULL;

	nf = len / cols;

	tbl = devm_kzalloc(dev, (nf + 1) * sizeof(struct core_dev_map),
			   GFP_KERNEL);
	if (tbl == NULL)
		return NULL;

	for (i = 0, j = 0; i < nf; i++, j += cols) {
		ret = of_property_read_u32_index(dev->of_node, prop_name, j,
						 &data);
		if (ret != 0)
			return NULL;

		tbl[i].core_mhz = data / KHZ_PER_MHZ;
		if (!tbl[i].core_mhz || tbl[i].core_mhz > USHRT_MAX ||
		    (i && tbl[i].core_mhz <= tbl[i - 1].core_mhz))
			return NULL;

		tbl[i].target_freq = devm_kcalloc(dev, info->nr_levels,
						 sizeof(*tbl[i].target_freq), GFP_KERNEL);
		if (!tbl[i].target_freq)
			return NULL;
		for (level = 0; level < info->nr_levels; level++) {
			ret = of_property_read_u32_index(dev->of_node, prop_name,
							 j + level + 1, &data);
			if (ret || data > info->hw_vote->max_freq)
				return NULL;
			tbl[i].target_freq[level] = (unsigned long)data * HZ_PER_MHZ;
			pr_debug("Entry%d CPU:%u, Level:%u Dev:%lu\n", i,
				 tbl[i].core_mhz, level, tbl[i].target_freq[level]);
		}
	}
	tbl[i].core_mhz = 0;
	info->map_len = nf;

	return tbl;
}

#define VOTE_REG_NUM	2
#define MBITS_LEFT_SHIFT	16
static struct vote_reg *init_hw_vote(struct device *dev)
{
	void __iomem *reg_base = NULL;
	struct vote_reg *hw_vote = NULL;
	struct resource resource;
	u32 temp[VOTE_REG_NUM] = {0};
	u32 reg_mask = 0;
	u32 reg_bit;
	int ret;

	ret = of_address_to_resource(dev->of_node, 0, &resource);
	if (ret)
		return NULL;
	reg_base = devm_ioremap(dev, resource.start, resource_size(&resource));
	if (reg_base == NULL) {
		dev_err(dev, "fail to map io!\n");
		return NULL;
	}
	hw_vote = devm_kzalloc(dev, sizeof(*hw_vote), GFP_KERNEL);
	if (hw_vote == NULL) {
		dev_err(dev, "[%s] node %s doesn't have hw_vote alloc fail!\n",
			__func__, dev->of_node->name);
		return NULL;
	}
	if (of_property_read_u32_array(dev->of_node, "vote-reg",
				       &temp[0], VOTE_REG_NUM) != 0) {
		dev_err(dev, "[%s] node %s doesn't have vote-reg property!\n",
			__func__, dev->of_node->name);
		goto free_hw;
	}
	hw_vote->reg = reg_base + temp[0];

	if (of_property_read_u32(dev->of_node, "vote-reg-mask",
				 &reg_mask) != 0) {
		dev_err(dev, "[%s] node %s doesn't have vote-reg mask property!\n",
			__func__, dev->of_node->name);
		goto free_hw;
	}
	hw_vote->mask = reg_mask;

	if (of_property_read_u32(dev->of_node, "vote-reg-bit", &reg_bit) != 0) {
		dev_err(dev, "[%s] node %s doesn't have vote-reg bit property!\n",
			__func__, dev->of_node->name);
		goto free_hw;
	}
	if (reg_bit >= 32 || resource_size(&resource) < sizeof(u32) ||
	    temp[0] > resource_size(&resource) - sizeof(u32))
		goto free_hw;
	hw_vote->bit = reg_bit;
	hw_vote->max_freq = temp[1] >> reg_bit;
	return hw_vote;
free_hw:
	devm_kfree(dev, hw_vote);
	return NULL;
}

static void init_vote_max_freq(struct dev_info *dev_info)
{
	unsigned int level;
	u32 init_freq = 0;

	for (level = 0; level < dev_info->nr_levels; level++)
		init_freq = max(init_freq, core_to_dev_freq(dev_info, level, USHRT_MAX));
	set_hw_vote(dev_info, init_freq, 0);
}

static int mipsmem_dev_alloc_and_init(struct platform_device *pdev, struct mipsmem_cluster_policy *policy)
{
	struct dev_info *dev_info = NULL;
	struct device *dev = &pdev->dev;
	int ret;
	unsigned long flags;
	unsigned int level;

	dev_info = kzalloc(sizeof(*dev_info), GFP_KERNEL);
	if (dev_info == NULL) {
		dev_err(dev, "[%s] node %s doesn't have dev_info alloc fail!\n",
			__func__, dev->of_node->name);
		return -ENOMEM;
	}
	dev_info->nr_levels = 1;
	if (of_find_property(dev->of_node, "nr-level", NULL)) {
		ret = of_property_read_u32(dev->of_node, "nr-level", &dev_info->nr_levels);
		if (ret)
			goto error;
	}

	dev_info->hw_vote = init_hw_vote(dev);
	if (dev_info->hw_vote == NULL) {
		dev_err(dev, "Couldn't find the hw_vote!\n");
		ret = -EINVAL;
		goto error;
	}

	dev_info->freq_map = init_core_dev_map(dev, "core-dev-table", dev_info);
	if (dev_info->freq_map == NULL) {
		dev_err(dev, "Couldn't find the freq map table!\n");
		ret = -EINVAL;
		goto error;
	}
	dev_info->target_ratios = kcalloc(dev_info->nr_levels,
					 sizeof(*dev_info->target_ratios), GFP_KERNEL);
	if (!dev_info->target_ratios) {
		ret = -ENOMEM;
		goto error;
	}
	for (level = 0; level < dev_info->nr_levels; level++) {
		dev_info->target_ratios[level].nratios = ARRAY_SIZE(default_target_ratios);
		dev_info->target_ratios[level].ratios = default_target_ratios;
	}
	dev_info->ipc_min_sample_time_ns = default_ipc_min_sample_time;
	dev_info->nipc_min_sample_time = ARRAY_SIZE(default_ipc_min_sample_time);
	dev_info->mem_load_window_size_ns = DEFAULT_MEM_WINDOW_SIZE;
	ret = kobject_init_and_add(&dev_info->kobj, &ktype_mips,
				   &policy->kobj,
				   strstr(pdev->name, "l3") ? "mips_l3" : "mips_ddr");
	if (ret) {
		dev_err(dev, "mips: failed to init dev_info->kobj: %d\n", ret);
		kobject_put(&dev_info->kobj);
		return ret;
	}
	/* Scheduler callbacks must never observe a partially initialized monitor. */
	spin_lock_irqsave(&state_lock, flags);
	init_vote_max_freq(dev_info);
	if (strstr(pdev->name, "l3"))
		policy->l3c_dev = dev_info;
	else
		policy->ddr_dev = dev_info;
	spin_unlock_irqrestore(&state_lock, flags);
	return 0;
error:
	kfree(dev_info);
	return ret;
}

static int mips_driver_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	cpumask_t cpus;
	int ret;
	struct mipsmem_cluster_policy *policy = NULL;

	if (get_mask_from_dev_handle(pdev, &cpus) != 0) {
		dev_err(dev, "No CPU use this mon\n");
		return -ENODEV;
	}

	policy = mipsmem_cluster_alloc_and_init(pdev, &cpus);
	if (!policy) {
		dev_err(dev, "init cluster fail\n");
		return -ENOMEM;
	}

	ret = mipsmem_dev_alloc_and_init(pdev, policy);
	if (ret)
		dev_err(dev, "init dev fail\n");
	return ret;
}

static int __ref mips_mem_online(unsigned int cpu)
{
	unsigned long flags;
	struct mipsmem_cpu_info *mem_cpu = &per_cpu(cpu_info, cpu);

	spin_lock_irqsave(&state_lock, flags);
	mem_cpu->enable = true;
	spin_unlock_irqrestore(&state_lock, flags);
	return 0;
}

static int __ref mips_mem_offline(unsigned int cpu)
{
	unsigned long flags;
	struct dev_info *ddr_info;
	struct dev_info *l3_info;
	struct mipsmem_cpu_info *mem_cpu = &per_cpu(cpu_info, cpu);

	spin_lock_irqsave(&state_lock, flags);
	if (!mem_enable(mem_cpu))
		goto unlock;
	clear_mem_freq(mem_cpu);
	change_mem_freq(mem_cpu);
unlock:
	mem_cpu->enable = false;
	spin_unlock_irqrestore(&state_lock, flags);
	return 0;
}

static const struct of_device_id match_table[] = {
	{ .compatible = "mips-mem" },
	{}
};

static struct platform_driver mips_mem_mon_driver = {
	.probe = mips_driver_probe,
	.driver = {
		.name = "mips-mem",
		.of_match_table = match_table,
		.owner = THIS_MODULE,
	},
};

static int __init mips_mem_init(void)
{
	int ret;
	g_mips_mem_global_kobject = kobject_create_and_add("mips_mem", &cpu_subsys.dev_root->kobj);
	if (!g_mips_mem_global_kobject)
		return -ENOMEM;
	ret = cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN, "mips-mem:online",
					mips_mem_online, mips_mem_offline);
	if (ret < 0) {
		pr_err("mips mem set hotplug state failed\n");
		return ret;
	}
	return platform_driver_register(&mips_mem_mon_driver);
}
module_init(mips_mem_init);
