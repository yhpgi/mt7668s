/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */
/* porting layer */
/* Android */

#ifndef _MTK_PORTING_H_
#define _MTK_PORTING_H_

#include <linux/kernel.h> /* include stddef.h for NULL */

/* typedef void void, *void *; */

typedef int MTK_WCN_BOOL;
#ifndef MTK_WCN_BOOL_TRUE
#define MTK_WCN_BOOL_FALSE    ((MTK_WCN_BOOL)0)
#define MTK_WCN_BOOL_TRUE     ((MTK_WCN_BOOL)1)
#endif

typedef int MTK_WCN_MUTEX;

typedef int MTK_WCN_TIMER;

/* system APIs */
/* mutex */
typedef MTK_WCN_MUTEX (*MUTEX_CREATE)(const char *const name);
typedef s32 (*MUTEX_DESTROY)(MTK_WCN_MUTEX mtx);
typedef s32 (*MUTEX_LOCK)(MTK_WCN_MUTEX mtx);
typedef s32 (*MUTEX_UNLOCK)(MTK_WCN_MUTEX mtx, unsigned long flags);
/* debug */
typedef s32 (*DBG_PRINT)(const char *str, ...);
typedef s32 (*DBG_ASSERT)(s32 expr, const char *file, s32 line);
/* timer */
typedef void (*MTK_WCN_TIMER_CB)(void);
typedef MTK_WCN_TIMER (*TIMER_CREATE)(const char *const name);
typedef s32 (*TIMER_DESTROY)(MTK_WCN_TIMER tmr);
typedef s32 (*TIMER_START)(MTK_WCN_TIMER tmr,
			   u32 timeout,
			   MTK_WCN_TIMER_CB tmr_cb,
			   void *param);
typedef s32 (*TIMER_STOP)(MTK_WCN_TIMER tmr);
/* kernel lib */
typedef void *(*SYS_MEMCPY)(void *dest, const void *src, u32 n);
typedef void *(*SYS_MEMSET)(void *s, s32 c, u32 n);
typedef s32 (*SYS_SPRINTF)(char *str, const char *format, ...);

#endif
