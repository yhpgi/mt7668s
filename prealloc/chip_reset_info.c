// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2017 MediaTek Inc.
 */

/*! \file   chip_reset_cnt.c
 *   \brief  Chip reset statistics
 *
 *    This file contains all implementations of chip reset statistics
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

/*******************************************************************************
 *                              C O N S T A N T S
 ********************************************************************************
 */

/*******************************************************************************
 *                             D A T A   T Y P E S
 ********************************************************************************
 */

/*******************************************************************************
 *                            P U B L I C   D A T A
 ********************************************************************************
 */

MODULE_LICENSE("Dual BSD/GPL");

/*******************************************************************************
 *                           P R I V A T E   D A T A
 ********************************************************************************
 */
#define RST_REASON_MAX 10 // sync with RST_REASON_MAX in gl_rst.h

uint32_t u4ChipResetReasonCnt[RST_REASON_MAX];

/*******************************************************************************
 *                                 M A C R O S
 ********************************************************************************
 */

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 ********************************************************************************
 */

/*******************************************************************************
 *                              F U N C T I O N S
 ********************************************************************************
 */

uint32_t getChipResetReasonCnt(uint32_t idx)
{
	if (idx >= RST_REASON_MAX) {
		return -1;
	}

	return u4ChipResetReasonCnt[idx];
}

EXPORT_SYMBOL(getChipResetReasonCnt);

uint32_t incChipResetReasonCnt(uint32_t idx)
{
	if (idx >= RST_REASON_MAX) {
		return 0;
	}

	u4ChipResetReasonCnt[idx]++;

	return 1;
}

EXPORT_SYMBOL(incChipResetReasonCnt);

void rstChipResetReasonCnt(void)
{
	memset(u4ChipResetReasonCnt, 0, sizeof(uint32_t) * RST_REASON_MAX);
}

EXPORT_SYMBOL(rstChipResetReasonCnt);
