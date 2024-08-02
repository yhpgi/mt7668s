// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   gl_rst.c
 *    \brief  Main routines for supporintg MT6620 whole-chip reset mechanism
 *
 *    This file contains the support routines of Linux driver for MediaTek Inc. 802.11
 *    Wireless LAN Adapters.
 */

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 ********************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 ********************************************************************************
 */
#include "precomp.h"
#include "gl_os.h"
#include <linux/timer.h>
#include <linux/mmc/card.h>

#if CFG_CHIP_RESET_SUPPORT
static void							glResetTriggerUpdateCnt(void);
enum _ENUM_CHIP_RESET_REASON_TYPE_T eResetReason;
uint64_t							u8ResetTime;

/*******************************************************************************
 *                              C O N S T A N T S
 ********************************************************************************
 */

#define WAIT_CD_STAR_TIME 10
#define WAIT_CD_END_TIME 30

#define MT76x8_PMU_EN_PIN_NAME "mt76x8_pmu_en_gpio"
#define MT76x8_PMU_EN_DELAY_NAME "mt76x8_pmu_en_delay"
#define MT76x8_PMU_EN_DEFAULT_DELAY (5) /* Default delay 5ms */

#define BT_NOTIFY 1
#define TIMEOUT_NOTIFY 2

#define NOTIFY_SUCCESS 0
#define RESETTING 1
#define NOTIFY_REPEAT 2

// update chip_reset_info.c if apcChipResetReason / apcChipResetAction have
// changed
const char *const apcChipResetReason[RST_REASON_MAX] = {
	"RST_UNKNOWN",
	"RST_PROCESS_ABNORMAL_INT",
	"RST_DRV_OWN_FAIL",
	"RST_FW_ASSERT",
	"RST_BT_TRIGGER",
	"RST_OID_TIMEOUT",
	"RST_CMD_TRIGGER",
	"RST_CR_ACCESS_FAIL",
	"RST_HIF_FAIL",
	"RST_PROBE_FAIL",
};

/*******************************************************************************
 *                            P U B L I C   D A T A
 ********************************************************************************
 */
struct rst_struct rst_data;

BOOLEAN g_fgIsCoreDumpStart	   = FALSE;
BOOLEAN g_fgIsCoreDumpEnd	   = FALSE;
BOOLEAN g_fgIsNotifyWlanRemove = FALSE;
BOOLEAN gl_sdio_fail		   = FALSE;

struct work_struct remove_work;

wait_queue_head_t wait_core_dump_start;
wait_queue_head_t wait_core_dump_end;
struct timer_list wait_CD_start_timer;
struct timer_list wait_CD_end_timer;

/*******************************************************************************
 *                           P R I V A T E   D A T A
 ********************************************************************************
 */

BOOLEAN		   g_fgIsWifiTrig = FALSE;
static BOOLEAN g_fgIsBTTrig	  = FALSE;
atomic_t	   g_fgBlockBTTriggerReset;

static BOOLEAN g_fgIsBTExist		   = TRUE;
static BOOLEAN g_fgIsCDEndTimeout	   = FALSE;
static BOOLEAN g_fgIsNotifyBTRemoveEnd = FALSE;

/*******************************************************************************
 *                                 M A C R O S
 ********************************************************************************
 */

#define IS_CORE_DUMP_START() (g_fgIsCoreDumpStart)
#define IS_CORE_DUMP_END() (g_fgIsCoreDumpEnd)

/*******************************************************************************
 *                   F U N C T I O N   D E C L A R A T I O N S
 ********************************************************************************
 */

static void resetInit(void);

static INT_32 pmu_toggle(struct rst_struct *data);

static void init_wait_CD_start_timer(void);
static void start_wait_CD_start_timer(void);

static void init_wait_CD_end_timer(void);
static void start_wait_CD_end_timer(void);
#if KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE
static void wait_CD_start_timeout(struct timer_list *timer);
static void wait_CD_end_timeout(struct timer_list *timer);
#else
static void wait_CD_start_timeout(unsigned long data);
static void wait_CD_end_timeout(unsigned long data);
#endif

static void removeWlanSelf(struct work_struct *work);
static void probeWlanSelf(struct work_struct *work);

static void RSTClearResource(void);
static void RSTClearState(void);
static VOID set_core_dump_start(BOOLEAN fgVal);

static INT_32 notify_wlan_toggle_rst_end(INT_32 reserved);
static void	  RSTP2pDestroyWirelessDevice(void);

/*******************************************************************************
 *                              F U N C T I O N S
 ********************************************************************************
 */
static void glResetTriggerUpdateCnt(void)
{
	typedef uint32_t (*p_inc_func_type)(uint32_t);
	p_inc_func_type inc_func;
	char		   *reason_func_name = "incChipResetReasonCnt";
	void		   *pvAddrReason	 = NULL;

	pvAddrReason = (void *)kallsyms_lookup_name(reason_func_name);

	if (eResetReason >= 0 && eResetReason < RST_REASON_MAX) {
		DBGLOG(INIT, ERROR, "reset reason %s\n", apcChipResetReason[eResetReason]);
		if (pvAddrReason) {
			inc_func = (p_inc_func_type)pvAddrReason;
			inc_func(eResetReason);
		} else {
			DBGLOG(INIT, ERROR, "%s does not exist\n", reason_func_name);
		}
	} else
		DBGLOG(INIT, ERROR, "unsupported reason %d\n", eResetReason);
}

VOID glResetTrigger(P_ADAPTER_T prAdapter, const UINT_8 *pucFile, UINT_32 u4Line)
{
	int bet = 0;
	typedef int (*p_bt_fun_type)(int);
	p_bt_fun_type bt_func;

	/* int btmtk_sdio_bt_trigger_core_dump(int reserved); */
	char *bt_func_name = "btmtk_sdio_bt_trigger_core_dump";

	DBGLOG(HAL, INFO, "[RST] rst_mutex lock\n");
	mutex_lock(&(rst_data.rst_mutex));

	if (checkResetState()) {
		DBGLOG(HAL, STATE, "[RST] resetting...\n");
		mutex_unlock(&(rst_data.rst_mutex));
		DBGLOG(INIT, INFO, "[RST] rst_mutex unlock\n");
		return;
	}

	dump_stack();
	DBGLOG(HAL, STATE, "[RST] glResetTrigger start\n");
	g_fgIsWifiTrig = TRUE;

	mutex_unlock(&(rst_data.rst_mutex));
	DBGLOG(INIT, INFO, "[RST] rst_mutex unlock\n");

	resetInit();

	/* check bt module */
	bt_func = (p_bt_fun_type)kallsyms_lookup_name(bt_func_name);
	if (bt_func) {
		BOOLEAN is_coredump = (~(prAdapter->fgIsChipNoAck)) & 0x1;

		DBGLOG(INIT, STATE, "[RST] wifi driver trigger rst in %s line %u\n", pucFile, u4Line);
		DBGLOG(INIT, STATE, "[RST] is_coredump = %d\n", is_coredump);
#if CFG_ASSERT_DUMP
		bet = bt_func(is_coredump);
#else
		bet = bt_func(0);
#endif

		if (bet) {
			g_fgIsBTExist = FALSE;
			DBGLOG(INIT, ERROR, "[RST] bt driver is not ready\n");
			/* error handle not yet */
			goto RESET_START;
		} else {
			g_fgIsBTExist = TRUE;
			start_wait_CD_start_timer();
			DBGLOG(INIT, STATE, "[RST] wait bt core dump start...\n");

			if (!IS_CORE_DUMP_START()) {
				/* wait bt notify */
				wait_event(wait_core_dump_start, g_fgIsCoreDumpStart);
				DBGLOG(INIT, STATE, "[RST] wait_core_dump_start end\n");
			}
			bet = del_timer_sync(&wait_CD_start_timer);
			DBGLOG(INIT, STATE, "[RST] cancel wait_CD_start_timer=%d\n", bet);

			goto RESET_START;
		}
	} else {
		g_fgIsBTExist = FALSE;
		DBGLOG(INIT, ERROR, "[RST] %s: do not get %s\n", __func__, bt_func_name);
		goto RESET_START;
	}

RESET_START:

	/* checkResetState(); // for debug */
	schedule_work(&remove_work);
	DBGLOG(INIT, STATE, "[RST] creat remove_work\n");
}

void glGetRstReason(enum _ENUM_CHIP_RESET_REASON_TYPE_T eReason)
{
	if (kalIsResetting())
		return;

	u8ResetTime	 = sched_clock();
	eResetReason = eReason;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief .
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
void resetInit(void)
{
	DBGLOG(INIT, STATE, "[RST] resetInit start\n");

	g_fgIsCoreDumpStart		= FALSE;
	g_fgIsCoreDumpEnd		= FALSE;
	g_fgIsCDEndTimeout		= FALSE;
	g_fgIsNotifyWlanRemove	= FALSE;
	g_fgIsNotifyBTRemoveEnd = FALSE;

	INIT_WORK(&remove_work, removeWlanSelf);
	INIT_WORK(&(rst_data.rst_work), probeWlanSelf);

	init_waitqueue_head(&wait_core_dump_start);
	/* init_waitqueue_head(&wait_core_dump_end); */
	init_wait_CD_start_timer();
	init_wait_CD_end_timer();

	DBGLOG(INIT, STATE, "[RST] resetInit end\n");
}
static void RSTClearResource(void)
{
	DBGLOG(INIT, STATE, "[RST] resetClear start\n");

	del_timer_sync(&wait_CD_start_timer);
	DBGLOG(INIT, STATE, "[RST] del CD_end_timer, pending %d\n", timer_pending(&wait_CD_end_timer));
	del_timer_sync(&wait_CD_end_timer);

	/* cancel_work_sync(&rst_data.rst_work); */
	cancel_work_sync(&remove_work);

	gl_sdio_fail = FALSE;
	DBGLOG(INIT, STATE, "[RST] resetClear end\n");
}

static void RSTClearState(void)
{
	DBGLOG(INIT, STATE, "[RST] RSTClearState\n");
	g_fgIsWifiTrig		   = FALSE;
	g_fgIsBTTrig		   = FALSE;
	g_fgIsNotifyWlanRemove = FALSE;
	atomic_set(&g_fgBlockBTTriggerReset, 0);
}

void removeWlanSelf(struct work_struct *work)
{
	int bet = 0;
	typedef int (*p_bt_fun_type)(void);
	p_bt_fun_type bt_func;
	/* int btmtk_sdio_notify_wlan_remove_end(void) */
	char *bt_func_name = "btmtk_sdio_notify_wlan_remove_end";

	DBGLOG(INIT, STATE, "[RST] check entry_conut = %d\n", rst_data.entry_conut);
	while (rst_data.entry_conut != 0)
		kalMsleep(100);

	DBGLOG(INIT, STATE, "[RST] start mtk_sdio_remove\n");
	glResetTriggerUpdateCnt();

#if MTK_WCN_HIF_SDIO
	bet = mtk_sdio_remove();
#else
	mtk_sdio_remove(rst_data.func);
#endif
	DBGLOG(INIT, STATE, "[RST] mtk_sdio_remove end\n");

	DBGLOG(INIT, STATE, "[RST] RSTP2pDestroyWirelessDevice start\n");
	RSTP2pDestroyWirelessDevice();
	DBGLOG(INIT, STATE, "[RST] RSTP2pDestroyWirelessDevice end\n");

	/* notify bt wifi remove end */
	bt_func = (p_bt_fun_type)kallsyms_lookup_name(bt_func_name);
	if (bt_func) {
		DBGLOG(INIT, STATE, "[RST] notify bt remove...\n");
		bet = bt_func();
		if (bet) {
			g_fgIsBTExist = FALSE;
			DBGLOG(INIT, ERROR, "[RST] notify bt fail\n");
			notify_wlan_toggle_rst_end(0);
		} else {
			g_fgIsNotifyBTRemoveEnd = TRUE;
			DBGLOG(INIT, STATE, "[RST] notify bt remove end\n");
			start_wait_CD_end_timer();
		}
	} else {
		g_fgIsBTExist = FALSE;
		DBGLOG(INIT, ERROR, "[RST] do not get %s\n", bt_func_name);
		notify_wlan_toggle_rst_end(0);
	}
}

void probeWlanSelf(struct work_struct *work)
{
	INT_32 i4Status = 0;

	DBGLOG(INIT, INFO, "[RST] g_fgIsBTExist=%d\n", g_fgIsBTExist);
	DBGLOG(INIT, INFO, "[RST] g_fgIsCDEndTimeout=%d\n", g_fgIsCDEndTimeout);
	DBGLOG(INIT, INFO, "[RST] g_fgIsNotifyBTRemoveEnd=%d\n", g_fgIsNotifyBTRemoveEnd);

	while (g_fgIsNotifyBTRemoveEnd == FALSE && g_fgIsBTExist == TRUE)
		kalMsleep(10);

	DBGLOG(INIT, STATE, "[RST] probeWlanSelf start\n");

	if (g_fgIsBTExist == FALSE || g_fgIsCDEndTimeout == TRUE) {
		DBGLOG(INIT, WARN, "[RST] sdio_claim_host\n");
		sdio_claim_host(rst_data.func);

		i4Status = pmu_toggle(&rst_data);
		if (i4Status) {
			DBGLOG(INIT, WARN, "[RST] pmu_toggle fail num=%d!\n", i4Status);
			return;
		}
		kalMsleep(500);

		DBGLOG(INIT, WARN, "[RST] sdio_reset_comm\n");
		i4Status = sdio_reset_comm(rst_data.func->card);
		if (i4Status)
			DBGLOG(INIT, ERROR, "[RST] sdio_reset_comm, err=%d\n", i4Status);
		kalMsleep(1000);

		sdio_release_host(rst_data.func);
		DBGLOG(INIT, WARN, "[RST] sdio_release_host\n");
	}

	RSTClearResource();

	DBGLOG(INIT, STATE, "[RST] mtk_sdio_probe start\n");
#if MTK_WCN_HIF_SDIO
	i4Status = mtk_sdio_probe(rst_data.func, &mtk_sdio_ids[1]);
#else
	i4Status = mtk_sdio_probe(rst_data.func, &mtk_sdio_ids[1]);
#endif
	if (i4Status) {
		DBGLOG(INIT, ERROR, "[RST] mtk_sdio_probe fail num=%d!\n", i4Status);
	}

	RSTClearState();

	DBGLOG(INIT, STATE, "[RST] resetWlanSelf end\n");
}

void init_wait_CD_start_timer(void)
{
	DBGLOG(INIT, INFO, "[RST] init CD start timer\n");
#if KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE
	timer_setup(&wait_CD_start_timer, wait_CD_start_timeout, 0);
#else
	init_timer(&wait_CD_start_timer);
	wait_CD_start_timer.function = wait_CD_start_timeout;
#endif
}

void start_wait_CD_start_timer(void)
{
	DBGLOG(INIT, LOUD, "[RST] create CD start timer\n");

	wait_CD_start_timer.expires = jiffies + (WAIT_CD_STAR_TIME * HZ);
#if KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE
	wait_CD_start_timer.data = ((unsigned long)0);
#endif
	add_timer(&wait_CD_start_timer);
}

#if KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE
void wait_CD_start_timeout(struct timer_list *timer)
#else
void wait_CD_start_timeout(unsigned long data)
#endif
{
	DBGLOG(INIT, ERROR, "[RST] timeout=%d\n", WAIT_CD_STAR_TIME);
	DBGLOG(INIT, ERROR, "[RST] g_fgIsCoreDumpStart = %d\n", g_fgIsCoreDumpStart);
	DBGLOG(INIT, ERROR, "[RST] wake up glResetTrigger\n");
	wake_up(&wait_core_dump_start);
	set_core_dump_start(TRUE);
	g_fgIsBTExist = FALSE;
}

void init_wait_CD_end_timer(void)
{
	DBGLOG(INIT, INFO, "[RST] init CD end timer\n");
#if KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE
	timer_setup(&wait_CD_end_timer, wait_CD_end_timeout, 0);
#else
	init_timer(&wait_CD_end_timer);
	wait_CD_end_timer.function = wait_CD_end_timeout;
#endif
}

void start_wait_CD_end_timer(void)
{
	DBGLOG(INIT, STATE, "[RST] create CD end timer\n");
	wait_CD_end_timer.expires = jiffies + (WAIT_CD_END_TIME * HZ);
#if KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE
	wait_CD_end_timer.data = ((unsigned long)0);
#endif
	add_timer(&wait_CD_end_timer);
}

#if KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE
void wait_CD_end_timeout(struct timer_list *timer)
#else
void wait_CD_end_timeout(unsigned long data)
#endif
{
	DBGLOG(INIT, ERROR, "[RST] timeout=%ld\n", WAIT_CD_END_TIME);
	DBGLOG(INIT, ERROR, "[RST] g_fgIsCoreDumpEnd = %d\n", g_fgIsCoreDumpEnd);

	if (g_fgIsCoreDumpEnd == FALSE) {
		g_fgIsCDEndTimeout = TRUE;
		notify_wlan_toggle_rst_end(0);
	}
}

BOOLEAN checkResetState(void)
{
#if 0
	pr_warn("[RST] g_fgIsWifiTrig=%d\n", g_fgIsWifiTrig);
	pr_warn("[RST] g_fgIsBTTrig=%d\n", g_fgIsBTTrig);
#endif
	return (g_fgIsWifiTrig | g_fgIsBTTrig);
}

BOOLEAN check_wlan_reset_state(void)
{
	DBGLOG(INIT, ERROR, "[RST] g_fgBlockBTTriggerReset = %d\n", g_fgBlockBTTriggerReset);
	return atomic_read(&g_fgBlockBTTriggerReset);
}

EXPORT_SYMBOL(check_wlan_reset_state);
/*----------------------------------------------------------------------------*/
/*!
 * \brief .
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
VOID set_core_dump_start(BOOLEAN fgVal)
{
	DBGLOG(INIT, ERROR, "[RST] set_core_dump_start = %d\n", fgVal);
	g_fgIsCoreDumpStart = fgVal;
}
INT_32 bt_notify_wlan_remove(INT_32 reserved)
{
	if (g_fgIsNotifyWlanRemove) {
		DBGLOG(INIT, ERROR, "[RST] g_fgIsNotifyWlanRemove=%d\n", g_fgIsNotifyWlanRemove);
		return NOTIFY_REPEAT;
	}
	g_fgIsNotifyWlanRemove = TRUE;
	atomic_set(&g_fgBlockBTTriggerReset, 1);
	DBGLOG(INIT, ERROR, "[RST] bt notify cd start...\n");
	if (g_fgIsWifiTrig) {
		DBGLOG(INIT, ERROR, "[RST] wake up glResetTrigger\n");
		set_core_dump_start(TRUE);
		wake_up(&wait_core_dump_start);
	} else {
		g_fgIsBTTrig  = TRUE;
		g_fgIsBTExist = TRUE;
		resetInit();
		set_core_dump_start(TRUE);
		glGetRstReason(RST_BT_TRIGGER);
		schedule_work(&remove_work);
		DBGLOG(INIT, ERROR, "[RST] creat remove_work\n");
	}
	DBGLOG(INIT, ERROR, "[RST] bt notify cd start end\n");
	return NOTIFY_SUCCESS;
}

INT_32 notify_wlan_remove_start(INT_32 reserved)
{
	return bt_notify_wlan_remove(reserved);
}
EXPORT_SYMBOL(notify_wlan_remove_start);
/*----------------------------------------------------------------------------*/
/*!
 * \brief .
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
VOID set_core_dump_end(BOOLEAN fgVal)
{
	DBGLOG(INIT, INFO, "[RST] g_fgIsCoreDumpEnd = %d\n", fgVal);
	g_fgIsCoreDumpEnd = fgVal;
}

INT_32 notify_wlan_toggle_rst_end(INT_32 reserved)
{
	/* create work queue */
	DBGLOG(INIT, STATE, "[RST] bt notify CD end\n");
	set_core_dump_end(TRUE);
	schedule_work(&rst_data.rst_work);
	DBGLOG(INIT, STATE, "[RST] create probe work\n");

	/* cencel timer */
	if (g_fgIsCDEndTimeout != TRUE) {
		DBGLOG(INIT, STATE, "[RST] del CD_end_timer, pending %d\n", timer_pending(&wait_CD_end_timer));
		del_timer_sync(&wait_CD_end_timer);
	}

	return NOTIFY_SUCCESS;
}
EXPORT_SYMBOL(notify_wlan_toggle_rst_end);

INT_32 pmu_toggle(struct rst_struct *data)
{
	UINT_32		   pmu_en_delay = MT76x8_PMU_EN_DEFAULT_DELAY;
	int			   pmu_en;
	struct device *prDev;
	INT_32		   i4Status = 0;

	typedef void (*p_sdio_fun_type)(int);
	p_sdio_fun_type sdio_func;
	/* int btmtk_sdio_notify_wlan_remove_end(void) */
	char *sdio_func_name = "sdio_set_card_clkpd";

	ASSERT(data);

	/* stop mtk sdio clk */
	sdio_func = (p_sdio_fun_type)kallsyms_lookup_name(sdio_func_name);
	if (sdio_func) {
		sdio_func(0);
		DBGLOG(INIT, STATE, "[RST] stop sdio clk\n");
	} else {
		DBGLOG(INIT, ERROR, "[RST] do not get %s\n", sdio_func);
	}

	DBGLOG(INIT, STATE, "[RST] get device\n");
	prDev = mmc_dev(data->func->card->host);
	if (!prDev) {
		DBGLOG(INIT, ERROR, "unable to get struct dev for wlan\n");
		return WLAN_STATUS_FAILURE;
	}

	DBGLOG(INIT, STATE, "[RST] pmu reset\n");
	pmu_en = of_get_named_gpio(prDev->of_node, MT76x8_PMU_EN_PIN_NAME, 0);

	if (gpio_is_valid(pmu_en)) {
		i4Status = of_property_read_u32(prDev->of_node, MT76x8_PMU_EN_DELAY_NAME, &pmu_en_delay);
		if (i4Status) {
			DBGLOG(INIT, ERROR, "[RST] undefined pmu_en delay, use default %ums\n", i4Status);
		}
		gpio_direction_output(pmu_en, 0);
		mdelay(pmu_en_delay);
		gpio_direction_output(pmu_en, 1);
	} else {
		DBGLOG(INIT, ERROR, "[RST] invalid gpio %s\n", MT76x8_PMU_EN_PIN_NAME);
		return WLAN_STATUS_FAILURE;
	}

	return WLAN_STATUS_SUCCESS;
}

void RSTP2pDestroyWirelessDevice(void)
{
	int i = 0;

	for (i = 1; i < KAL_P2P_NUM; i++) {
		if (gprP2pRoleWdev[i] == NULL)
			continue;

		DBGLOG(INIT, STATE, "glP2pDestroyWirelessDevice[%d] (0x%p)\n", i, gprP2pRoleWdev[i]->wiphy);
		set_wiphy_dev(gprP2pRoleWdev[i]->wiphy, NULL);
		wiphy_unregister(gprP2pRoleWdev[i]->wiphy);
		wiphy_free(gprP2pRoleWdev[i]->wiphy);
		kfree(gprP2pRoleWdev[i]);

		gprP2pRoleWdev[i] = NULL;
	}
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is called for checking if connectivity chip is resetting
 *
 * @param   None
 *
 * @retval  TRUE
 *          FALSE
 */
/*----------------------------------------------------------------------------*/
extern int IS_WLAN_REMOVING(void);

BOOLEAN kalIsResetting(VOID)
{
	return IS_WLAN_REMOVING(); // FALSE;
}

#endif /* CFG_CHIP_RESET_SUPPORT */