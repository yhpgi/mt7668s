/******************************************************************************
 *
 * GPLv2 License
 *
 * Copyright(C) 2024 Yogi Hermawan <yogist.xda@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 *****************************************************************************/

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yogi Hermawan");
MODULE_DESCRIPTION("MT7668S rescan module");

// extern void wifi_setup_dt(void);
extern void extern_wifi_set_enable(int);
extern void sdio_reinit(void);

static int __init mt7668s_reinit(void)
{
	pr_info("Triggered SDIO WiFi power on and bus rescan.\n");
	// wifi_setup_dt();
	// msleep(300);
	extern_wifi_set_enable(0);
	msleep(300);
	extern_wifi_set_enable(1);
	sdio_reinit();
	return 0;
}

static void __exit mt7668s_cleanup(void)
{
    pr_info("Cleaning up module.\n");
}

module_init(mt7668s_reinit);
module_exit(mt7668s_cleanup);
