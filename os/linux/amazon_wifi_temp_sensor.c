/*
 * Copyright (C) 2018 Amazon.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/atomic.h>
#include <linux/uaccess.h>
#include <linux/thermal.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kobject.h>
#include <linux/thermal_framework.h>
#include <linux/cpufreq.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/stddef.h>
#include <linux/sysfs.h>
#include <linux/err.h>
#include <linux/types.h>
#include "gl_os.h"
#include "wlan_oid.h"

#define WIFI_TEMP_SENSOR_NAME "amazon_wifi_sensor"

#ifdef CFG_SUPPORT_DUAL_CARD_DUAL_DRIVER
#if defined(_HIF_USB)
#undef WIFI_TEMP_SENSOR_NAME
#define WIFI_TEMP_SENSOR_NAME "amazon_wifi_sensor_usb"
#endif
#endif

#define WIFI_TEMP_SENSOR_NUM 1
#define WIFI_TEMP_VENDOR_DIV 1000

P_GLUE_INFO_T g_prGlueInfo;

int wifi_temp_sensor_register(P_GLUE_INFO_T prGlueInfo) {
	g_prGlueInfo = prGlueInfo;
	return 0;
}

int wifi_temp_sensor_deregister(void) {
	g_prGlueInfo = NULL;
	return 0;
}

static int wlan_get_temperature(void)
{
	int temperature = 0;
	unsigned int oid_len;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;

	if (!g_prGlueInfo)
		return 0;

	rStatus = kalIoctl(g_prGlueInfo,
			   wlanoidGetTemperature, &temperature,
			   sizeof(temperature), TRUE, TRUE, TRUE, &oid_len);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		return 0;
	}

	return temperature;
}

/*
 * struct wifi_temp_sensor_info  - Structure for wifi temp sensor info
 * @pdev: Platform device ptr
 * @tdev: Thermal zone device ptr
 */
struct wifi_temp_sensor_info {
	struct platform_device *pdev;
	struct thermal_dev *tdev;
	struct thermal_zone_device *tzd;
};
static struct wifi_temp_sensor_info *g_info;

static int wifi_temp_sensor_read_temp(struct thermal_dev *tdev)
{
	return wlan_get_temperature();
}

static int wifi_temp_sensor_read_temp_tz(void *data, int *temp)
{
	/* Convert temperture to mC */
	*temp = wlan_get_temperature() * 1000;

	return 0;
}

static ssize_t wifi_temp_show_temp(struct device *dev,
				 struct device_attribute *devattr,
				 char *buf)
{
	return sprintf(buf, "%d\n", wlan_get_temperature());
}

static DEVICE_ATTR(temp, 0444, wifi_temp_show_temp, NULL);

static struct thermal_dev_ops wifi_temp_sensor_ops = {
	.get_temp = wifi_temp_sensor_read_temp,
};

static struct thermal_zone_of_device_ops wifi_temp_sensor_tz_ops = {
	.get_temp = wifi_temp_sensor_read_temp_tz,
};

static int wifi_temp_sensor_probe(struct platform_device *pdev)
{
	struct wifi_temp_sensor_info *info;
	struct thermal_zone_device *tzd;
	int ret;

	if (!pdev->dev.of_node) {
		dev_err(&pdev->dev, "%s: Error no of_node\n", __func__);
		return -EINVAL;
	}

	info = devm_kzalloc(&pdev->dev, sizeof(struct wifi_temp_sensor_info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "%s:%d Could not allocate space for wifi temp sensor info\n",
		       __func__, __LINE__);
		return -ENOMEM;
	}

	g_info = info;
	info->pdev = pdev;

	info->tdev = devm_kzalloc(&pdev->dev, sizeof(struct thermal_dev), GFP_KERNEL);
	if (!info->tdev) {
		dev_err(&pdev->dev, "%s:%d Could not allocate space for wifi thermal dev\n",
		       __func__, __LINE__);
		return -ENOMEM;
	}

	info->tdev->name = pdev->dev.of_node->name;
	info->tdev->dev = &pdev->dev;
	info->tdev->vs = WIFI_TEMP_SENSOR_NUM;
	info->tdev->dev_ops = &wifi_temp_sensor_ops;

/*
 * Set CFG_TEMP_SENSOR_AS_PHYSICAL_ONLY=y in .config file to disable this section
 * and not register it with the virtual sensor framework.
 */
#ifndef CFG_TEMP_SENSOR_AS_PHYSICAL_ONLY
	ret = thermal_dev_register(info->tdev);
	if (ret)
		dev_err(&pdev->dev, "%s error registering thermal device\n", __func__);
#endif
	tzd = thermal_zone_of_sensor_register(&pdev->dev, 0,
						NULL, &wifi_temp_sensor_tz_ops);
	if (IS_ERR(tzd))
		pr_err("%s Failed to register sensor\n", __func__);
	else
		info->tzd = tzd;

	ret = device_create_file(&pdev->dev, &dev_attr_temp);
	if (ret)
		pr_err("%s Failed to create temp attr\n", __func__);

	dev_set_drvdata(&pdev->dev, info);

	return 0;
}

static int wifi_temp_sensor_remove(struct platform_device *pdev)
{
	struct wifi_temp_sensor_info *info;
#ifndef CFG_TEMP_SENSOR_AS_PHYSICAL_ONLY
	int ret;
#endif
	device_remove_file(&pdev->dev, &dev_attr_temp);

	info = dev_get_drvdata(&pdev->dev);

	if (info == NULL) {
		pr_err("%s No driver data available for device\n", __func__);
	} else {
#ifndef CFG_TEMP_SENSOR_AS_PHYSICAL_ONLY
		ret = thermal_dev_deregister(info->tdev);
		if (ret)
			dev_err(&pdev->dev,
				"%s error deregistering thermal device\n",
				__func__);
#endif
		thermal_zone_of_sensor_unregister(&pdev->dev, info->tzd);
	}

	return 0;
}

static struct of_device_id sensor_of_match[] = {
	{.compatible = "amazon,wifi_temp_sensor", },
	{ },
};

static struct platform_driver wifi_temp_sensor_driver = {
	.probe = wifi_temp_sensor_probe,
	.remove = wifi_temp_sensor_remove,
	.driver = {
		.name  = WIFI_TEMP_SENSOR_NAME,
		.owner = THIS_MODULE,
		.of_match_table = sensor_of_match,
	},
};

int wifi_temp_sensor_init(void)
{
	int ret = platform_driver_register(&wifi_temp_sensor_driver);
	if (ret) {
		pr_err("Unable to register wifi temp sensor driver (%d)\n", ret);
		return ret;
	}

	return 0;
}

int wifi_temp_sensor_exit(void)
{
	g_prGlueInfo = NULL;
	platform_driver_unregister(&wifi_temp_sensor_driver);

	return 0;
}
