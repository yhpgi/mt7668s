/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   "rlm_obss.h"
 *    \brief
 */

#ifndef _RLM_OBSS_H
#define _RLM_OBSS_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 ********************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 ********************************************************************************
 */

/*******************************************************************************
 *                              C O N S T A N T S
 ********************************************************************************
 */
#define CHNL_LIST_SZ_2G 14
#define CHNL_LIST_SZ_5G 14

#define CHNL_LEVEL0 0
#define CHNL_LEVEL1 1
#define CHNL_LEVEL2 2

#define AFFECTED_CHNL_OFFSET 5

#define OBSS_SCAN_MIN_INTERVAL 10 /* In unit of sec */

#define PUBLIC_ACTION_MAX_LEN 200 /* In unit of byte */

/* P2P GO only */
/* Define default OBSS Scan parameters (from MIB in spec.) */
#define dot11OBSSScanPassiveDwell 20
#define dot11OBSSScanActiveDwell 10
#define dot11OBSSScanPassiveTotalPerChannel 200
#define dot11OBSSScanActiveTotalPerChannel 20
#define dot11BSSWidthTriggerScanInterval 300 /* Unit: sec */
#define dot11BSSWidthChannelTransitionDelayFactor 5
#define dot11OBSSScanActivityThreshold 25

#define OBSS_20_40M_TIMEOUT (dot11BSSWidthTriggerScanInterval + 10)

/*******************************************************************************
 *                             D A T A   T Y P E S
 ********************************************************************************
 */

/* Control MAC PCO function */
typedef enum _ENUM_SYS_PCO_PHASE_T {
	SYS_PCO_PHASE_DISABLED = 0,
	SYS_PCO_PHASE_20M,
	SYS_PCO_PHASE_40M
} ENUM_SYS_PCO_PHASE_T,
		*P_ENUM_SYS_PCO_PHASE_T;

/*******************************************************************************
 *                            P U B L I C   D A T A
 ********************************************************************************
 */

/*******************************************************************************
 *                           P R I V A T E   D A T A
 ********************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 ********************************************************************************
 */

/*******************************************************************************
 *                   F U N C T I O N   D E C L A R A T I O N S
 ********************************************************************************
 */
VOID rlmObssInit(P_ADAPTER_T prAdapter);

VOID rlmObssScanDone(P_ADAPTER_T prAdapter, P_MSG_HDR_T prMsgHdr);

VOID rlmObssTriggerScan(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

/*******************************************************************************
 *                              F U N C T I O N S
 ********************************************************************************
 */

#endif /* _RLM_OBSS_H */
