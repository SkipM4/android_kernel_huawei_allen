// SPDX-License-Identifier: GPL-2.0-only
/*
 * perf_ctrl.c
 *
 * interfaces defined for perf_ctrl
 *
 * Copyright (c) 2016-2020 Huawei Technologies Co., Ltd.
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

#define pr_fmt(fmt) "perf_ctrl: " fmt

#include <linux/compat.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <platform_include/cee/linux/perf_ctrl.h>

static unsigned long get_dev_cap(void)
{
	unsigned long cap = 0;

	cap |= BIT(CAP_AI_SCHED_COMM_CMD);
#ifdef CONFIG_SCHED_RTG
	cap |= BIT(CAP_RTG_CMD);
#endif
#ifdef CONFIG_RENDER_RT
	cap |= BIT(CAP_RENDER_RT_CMD);
#endif

	return cap;
}

static int perf_ctrl_get_dev_cap(void __user *uarg)
{
	unsigned long cap = get_dev_cap();

	if (uarg == NULL)
		return -EINVAL;

	if (copy_to_user(uarg, &cap, sizeof(unsigned long))) {
		pr_err("%s: copy_to_user fail\n", __func__);
		return -EFAULT;
	}

	return 0;
}

typedef int (*perf_ctrl_func)(void __user *arg);

/* Bind the released command IDs to Allen's existing handler implementations. */
static const perf_ctrl_func g_func_array[PERF_CTRL_MAX_NR] = {
	[GET_SCHED_STAT] = perf_ctrl_get_sched_stat,
	[GET_IPA_STAT] = perf_ctrl_get_ipa_stat,
	[GET_DDR_FLUX] = perf_ctrl_get_ddr_flux,
	[GET_RELATED_TID] = perf_ctrl_get_related_tid,
	[GET_DEV_FREQ] = perf_ctrl_get_drg_dev_freq,
	[GET_THERMAL_CDEV_POWER] = perf_ctrl_get_thermal_cdev_power,
	[SET_FRAME_RATE] = perf_ctrl_set_frame_rate,
	[SET_FRAME_MARGIN] = perf_ctrl_set_frame_margin,
	[SET_FRAME_STATUS] = perf_ctrl_set_frame_status,
	[SET_TASK_RTG] = perf_ctrl_set_task_rtg,
	[SET_RTG_CPUS] = perf_ctrl_set_rtg_cpus,
	[SET_RTG_FREQ] = perf_ctrl_set_rtg_freq,
	[SET_RTG_FREQ_UPDATE_INTERVAL] = perf_ctrl_set_rtg_freq_update_interval,
	[SET_RTG_UTIL_INVALID_INTERVAL] = perf_ctrl_set_rtg_util_invalid_interval,
	[GET_GPU_FENCE] = perf_ctrl_get_gpu_fence,
	[INIT_RENDER_PID] = init_render_rthread,
	[GET_RENDER_RT] = get_render_rthread,
	[STOP_RENDER_RT] = stop_render_rthread,
	[GET_RENDER_HT] = get_render_hrthread,
	[DESTROY_RENDER_RT] = destroy_render_rthread,
	[SET_RTG_LOAD_MODE] = perf_ctrl_set_rtg_load_mode,
	[SET_RTG_ED_PARAMS] = perf_ctrl_set_rtg_ed_params,
	[GET_DEV_CAP] = perf_ctrl_get_dev_cap,
	[SET_VIP_PRIO] = perf_ctrl_set_vip_prio,
	[SET_FAVOR_SMALL_CAP] = perf_ctrl_set_favor_small_cap,
	[SET_TASK_RTG_MIN_FREQ] = perf_ctrl_set_task_rtg_min_freq,
	[SET_TASK_MIN_UTIL] = perf_ctrl_set_task_min_util,
	[GET_GPU_BUFFER_SIZE] = perf_ctrl_get_gpu_buffer_size,
	[SET_LB_POLICY] = perf_ctrl_lb_set_policy,
	[RESET_LB_POLICY] = perf_ctrl_lb_reset_policy,
	[GET_LB_LITE_INFO] = perf_ctrl_lb_lite_info,
	[ENABLE_GPU_LB] = perf_ctrl_enable_gpu_lb,
	[GET_CPU_BUSY_TIME] = perf_ctrl_get_cpu_busy_time,
	[SET_TASK_MAX_UTIL] = perf_ctrl_set_task_max_util,
	[GET_TASK_YIELD_TIME] = perf_ctrl_get_task_yield_time,
	[SET_TASK_L3C_PART] = perf_ctrl_set_task_l3c_part,
	[GET_HVGR_GPU_FENCE] = perf_ctrl_get_hvgr_gpu_fence,
	[GET_HVGR_GPU_BUFFER_SIZE] = perf_ctrl_get_hvgr_gpu_buffer_size,
#ifdef CONFIG_CPU_HWP_CTRL
	[SET_TASK_CPU_HWP] = perf_ctrl_set_task_cpu_hwp,
#endif
	[SET_TASK_NO_SMT] = perf_ctrl_set_task_no_smt,
	[GET_TID_NAME] = perf_ctrl_get_tid_name,
	[GET_CGRP_NAME] = perf_ctrl_get_cgroup_name,
	[INIT_RENDER_TID_GLO] = init_render_rthread_global,
	[GET_RENDER_RT_GLO] = get_render_rthread_glo,
	[DESTROY_RENDER_RT_GLO] = destroy_render_rthread_glo,
	[SET_FRAME_VIP_PRIO] = perf_ctrl_set_frame_vip_prio,
	[GET_TASK_PID] = perf_ctrl_get_task_pid,
};

static long perf_ctrl_ioctl(struct file *file, unsigned int cmd,
			   unsigned long arg)
{
	void __user *uarg = (void __user *)(uintptr_t)arg;
	unsigned int func_id = _IOC_NR(cmd);

	if (uarg == NULL) {
		pr_err("%s: invalid user uarg\n", __func__);
		return -EINVAL;
	}

	if (_IOC_TYPE(cmd) != PERF_CTRL_MAGIC) {
		pr_err("%s: PERF_CTRL_MAGIC fail, TYPE=%d\n",
		       __func__, _IOC_TYPE(cmd));
		return -EINVAL;
	}

	if (func_id >= PERF_CTRL_MAX_NR) {
		pr_err("%s: PERF_CTRL_MAX_NR fail, _IOC_NR(cmd)=%d, MAX_NR=%d\n",
		       __func__, _IOC_NR(cmd), PERF_CTRL_MAX_NR);
		return -EINVAL;
	}

	if (g_func_array[func_id] == NULL) {
		pr_err("cmd error, here is default, cmd=%u\n", cmd);
		return -EINVAL;
	}

	return g_func_array[func_id](uarg);
}

#ifdef CONFIG_COMPAT
static long perf_ctrl_compat_ioctl(struct file *file, unsigned int cmd,
				  unsigned long arg)
{
	/* The standard compat capability word is four bytes, not eight. */
	if (cmd == _IOR(PERF_CTRL_MAGIC, GET_DEV_CAP, compat_ulong_t))
		return put_user((compat_ulong_t)get_dev_cap(),
				(compat_ulong_t __user *)compat_ptr(arg));

	return perf_ctrl_ioctl(file, cmd, (unsigned long)compat_ptr(arg));
}
#endif

static const struct file_operations perf_ctrl_fops = {
	.owner = THIS_MODULE,
#ifdef CONFIG_COMPAT
	.compat_ioctl = perf_ctrl_compat_ioctl,
#endif
	.unlocked_ioctl = perf_ctrl_ioctl,
};

static struct miscdevice perf_ctrl_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hisi_perf_ctrl",
	.fops = &perf_ctrl_fops,
};

static int __init perf_ctrl_dev_init(void)
{
	return misc_register(&perf_ctrl_device);
}

static void __exit perf_ctrl_dev_exit(void)
{
	misc_deregister(&perf_ctrl_device);
}

module_init(perf_ctrl_dev_init);
module_exit(perf_ctrl_dev_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Huawei performance control interfaces");
