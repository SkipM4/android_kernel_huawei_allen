// SPDX-License-Identifier: GPL-2.0-only
/*
 * Thermal performance-control queries.
 *
 * Copyright (c) 2020 Huawei Technologies Co., Ltd.
 */

#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <platform_include/cee/linux/thermal_perf_ctrl.h>
#include <platform_include/cee/linux/lpm_thermal.h>

#ifdef CONFIG_IPA_THERMAL

static const char * const thermal_cdev_names[THERMAL_CDEV_MAX] = {
	[CDEV_GPU] = CDEV_GPU_NAME,
	[CDEV_CPU_CLUSTER0] = CDEV_CPU_CLUSTER0_NAME,
	[CDEV_CPU_CLUSTER1] = CDEV_CPU_CLUSTER1_NAME,
	[CDEV_CPU_CLUSTER2] = CDEV_CPU_CLUSTER2_NAME,
};

static const char * const thermal_zone_names[THERMAL_ZONE_MAX] = {
	[SOC_THERMAL_ZONE] = SOC_THERMAL_NAME,
	[BOARD_THERMAL_ZONE] = BOARD_THERMAL_NAME,
};

static int get_max_soc_temp(int *temp)
{
	int value, ret;
	int max_temp = INT_MIN;
	u32 id;

	/* Use the same DT-configured sensor range as the existing IPA policy. */
	for (id = 0; id < g_ipa_sensor_num; id++) {
		ret = ipa_get_sensor_value(id, &value);
		if (ret)
			return ret;
		max_temp = max(max_temp, value);
	}

	*temp = max_temp;
	return 0;
}

int perf_ctrl_get_ipa_stat(void __user *uarg)
{
	struct ipa_stat status;
	int sensor_id, ret;

	ret = get_ipa_status(&status);
	if (ret)
		return ret;

	ret = get_max_soc_temp(&status.soc_temp);
	if (ret)
		return ret;

	sensor_id = ipa_get_periph_id("system_h");
	if (sensor_id < 0)
		return sensor_id;

	ret = ipa_get_periph_value(sensor_id, &status.board_temp);
	if (ret)
		return ret;

	if (copy_to_user(uarg, &status, sizeof(status)))
		return -EFAULT;

	return 0;
}

int perf_ctrl_get_thermal_cdev_power(void __user *uarg)
{
	struct thermal_cdev_power power;
	int i, ret;

	if (copy_from_user(&power, uarg, sizeof(power)))
		return -EFAULT;

	if (power.thermal_zone_type < 0 ||
	    power.thermal_zone_type >= THERMAL_ZONE_MAX)
		return -EINVAL;

	for (i = 0; i < THERMAL_CDEV_MAX; i++) {
		ret = thermal_zone_cdev_get_power(
			thermal_zone_names[power.thermal_zone_type],
			thermal_cdev_names[i], &power.cdev_power[i]);
		if (ret)
			return ret;
	}

	if (copy_to_user(uarg, &power, sizeof(power)))
		return -EFAULT;

	return 0;
}

#endif /* CONFIG_IPA_THERMAL */
