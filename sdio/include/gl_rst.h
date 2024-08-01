/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   gl_rst.h
 *    \brief  Declaration of functions and finite state machine for
 *	    MT6620 Whole-Chip Reset Mechanism
 */

#ifndef _GL_RST_H
#define _GL_RST_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 ********************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 ********************************************************************************
 */
#include "gl_typedef.h"
#include <linux/of_gpio.h>
#include <linux/mmc/host.h>
/*******************************************************************************
 *                              C O N S T A N T S
 ********************************************************************************
 */

/*******************************************************************************
 *                             D A T A   T Y P E S
 ********************************************************************************
 */

struct rst_struct {
	struct sdio_func	 *func;
	struct work_struct rst_work;

	struct mutex rst_mutex;
	int			 entry_conut;
};

/*******************************************************************************
 *                    E X T E R N A L   F U N C T I O N S
 ********************************************************************************
 */

/*******************************************************************************
 *                            P U B L I C   D A T A
 ********************************************************************************
 */
extern struct rst_struct rst_data;

extern const struct sdio_device_id mtk_sdio_ids[];

enum _ENUM_CHIP_RESET_REASON_TYPE_T {
	RST_PROCESS_ABNORMAL_INT = 1,
	RST_DRV_OWN_FAIL,
	RST_FW_ASSERT,
	RST_BT_TRIGGER,
	RST_OID_TIMEOUT,
	RST_CMD_TRIGGER,
	RST_CR_ACCESS_FAIL,
	RST_HIF_FAIL,
	RST_PROBE_FAIL,
	RST_REASON_MAX
};

/*******************************************************************************
 *                                 M A C R O S
 ********************************************************************************
 */

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 ********************************************************************************
 */

extern int sdio_reset_comm(struct mmc_card *card);

/*******************************************************************************
 *                              F U N C T I O N S
 ********************************************************************************
 */
#if CFG_CHIP_RESET_SUPPORT
BOOLEAN kalIsResetting(VOID);

BOOLEAN checkResetState(void);
VOID	glResetTrigger(P_ADAPTER_T prAdapter, const UINT_8 *pucFile, UINT_32 u4Line);
VOID	glGetRstReason(enum _ENUM_CHIP_RESET_REASON_TYPE_T eReason);

extern atomic_t g_fgBlockBTTriggerReset;

extern enum _ENUM_CHIP_RESET_REASON_TYPE_T eResetReason;
extern uint64_t							   u8ResetTime;

#define GL_RESET_TRIGGER(_prAdapter, _eReason) \
	{ \
		glGetRstReason(_eReason); \
		glResetTrigger(_prAdapter, (const uint8_t *)__FILE__, __LINE__); \
	}

#else
#define glResetTrigger(A, B)
#endif

#endif /* _GL_RST_H */
