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

#ifndef _AMAZON_WIFI_TEMP_SENSOR_H
#define _AMAZON_WIFI_TEMP_SENSOR_H

int wifi_temp_sensor_init(void);
int wifi_temp_sensor_exit(void);

int wifi_temp_sensor_register(P_GLUE_INFO_T prGlueInfo);
int wifi_temp_sensor_deregister(void);

#endif
