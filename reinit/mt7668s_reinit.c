// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2024 Yogi Hermawan <yogist.xda@gmail.com>.
 */

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
