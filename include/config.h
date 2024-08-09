/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   "config.h"
 *   \brief  This file includes the various configurable parameters for customers
 *
 *    This file ncludes the configurable parameters except the parameters indicate the turning-on/off of some features
 */

#ifndef _CONFIG_H
#define _CONFIG_H

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

#ifndef DBG
#define DBG     0
#endif
#define BUILD_DBG_MSG			0
#define BUILD_QA_DBG 			1
#define CFG_DEFAULT_DBG_LEVEL 	0xF

#ifndef CFG_MESON_G12A_PATCH
#define CFG_MESON_G12A_PATCH    1
#endif

#ifndef CFG_GARP_KEEPALIVE
#define CFG_GARP_KEEPALIVE      1
#endif

#define CFG_FW_FILENAME 		"WIFI_RAM_CODE"
#define CFG_CR4_FW_FILENAME 	"WIFI_RAM_CODE2"


#if CFG_MESON_G12A_PATCH
#define CFG_TX_MAX_PKT_SIZE 	1408
#define CFG_SDIO_RX_ENHANCE 	0
#define CFG_SDIO_TX_AGG 		0
#else
#define CFG_TX_MAX_PKT_SIZE 	1600
#define CFG_SDIO_RX_ENHANCE 	1
#define CFG_SDIO_TX_AGG 		1
#endif

#define CFG_SDIO_INTR_ENHANCE 	1
#define CFG_SDIO_RX_AGG 		1
#define CFG_SDIO_RX_AGG_TASKLET 0

#if (CFG_SDIO_RX_AGG == 1) && (CFG_SDIO_INTR_ENHANCE == 0)
#error "CFG_SDIO_INTR_ENHANCE must 1 when CFG_SDIO_RX_AGG =1"
#elif (CFG_SDIO_INTR_ENHANCE == 1 \
    || CFG_SDIO_RX_ENHANCE == 1) && (CFG_SDIO_RX_AGG == 0)
#error "CFG_SDIO_RX_AGG must 1 when CFG_SDIO_INTR_ENHANCE nor CFG_SDIO_RX_ENHANCE =1"
#endif

#define CFG_UMAC_GENERATION 	0x20

#define HW_BSSID_NUM 			4
#define HW_WMM_NUM 				4
#define MAX_BSSID_NUM 			4

#define CFG_DRV_OWN_VERSION 	((UINT_16)((NIC_DRIVER_MAJOR_VERSION << 8) \
    | (NIC_DRIVER_MINOR_VERSION)))
#define CFG_DRV_PEER_VERSION 	((UINT_16)0x0000)


#define CFG_TX_MAX_PKT_NUM 		1024
#define CFG_TX_MAX_CMD_PKT_NUM 	32


#define CFG_NUM_OF_RX0_HIF_DESC 16
#define CFG_NUM_OF_RX1_HIF_DESC 2

#define CFG_RX_MAX_PKT_NUM 		((CFG_NUM_OF_RX0_HIF_DESC + \
    CFG_NUM_OF_RX1_HIF_DESC) * 3 + HIF_NUM_OF_QM_RX_PKT_NUM)

#define CFG_RX_REORDER_Q_THRESHOLD 	    8
#define CFG_RX_RETAINED_PKT_THRESHOLD 	0

#define CFG_RX_MAX_PKT_SIZE 		(28 + 2312 + 12 /*HIF_RX_HEADER_T*/)
#define CFG_RX_MIN_PKT_SIZE 		10 /*!< 802.11 Control Frame is 10 bytes */

#define CFG_NUM_OF_RX_BA_AGREEMENTS 8
#define CFG_RX_BA_INC_SIZE 			64
#define CFG_RX_MAX_BA_TID_NUM 		8

#define CFG_PF_ARP_NS_MAX_NUM 		3

#define CFG_RESPONSE_POLLING_TIMEOUT 1000
#define CFG_RESPONSE_POLLING_DELAY 	 5

#define CFG_MAX_NUM_BSS_LIST 		192
#define CFG_MAX_COMMON_IE_BUF_LEN 	((1500 * CFG_MAX_NUM_BSS_LIST) / 3)
#define CFG_RAW_BUFFER_SIZE 		1024
#define CFG_IE_BUFFER_SIZE 			512

#define CFG_MAX_PMKID_CACHE 		16

/* ARRAY_SIZE(mtk_5ghz_channels) + ARRAY_SIZE(mtk_2ghz_channels) */
#define MAX_CHN_NUM 				39
#define MAX_2G_BAND_CHN_NUM 		14
#define MAX_5G_BAND_CHN_NUM 		(MAX_CHN_NUM - MAX_2G_BAND_CHN_NUM)

#define CFG_INIT_ADHOC_FREQ 				(2462000)
#define CFG_INIT_ADHOC_MODE 				AD_HOC_MODE_MIXED_11BG
#define CFG_INIT_ADHOC_BEACON_INTERVAL 		(100)
#define CFG_INIT_ADHOC_ATIM_WINDOW 			(0)

#define CFG_SCAN_SSID_MAX_NUM 				(4)
#define CFG_SCAN_SSID_MATCH_MAX_NUM 		(16)
#define CFG_LINK_QUALITY_VALID_PERIOD 		500
#define CFG_SUPPORT_BEACON_CHANGE_DETECTION 0

#define CFG_MAX_NUM_ROAM_BSS_LIST 	64
#define WNM_UNIT_TEST 				0
#define CFG_MTK_STAGE_SCAN          1
#define CFG_SUPPORT_MTK_SYNERGY     1

#define CFG_FIX_2_TX_PORT           0

#define CFG_SUPPORT_AIS_PASSIVE_SCAN    0
#define CFG_ENABLE_READ_EXTRA_4_BYTES   1 /* !!!!!!!!!!!!!!!!!! */


#define COMPILE_FLAG0_GET_STA_LINK_STATUS 		(1 << 0)
#define COMPILE_FLAG0_WFD_ENHANCEMENT_PROTECT 	(1 << 1)

#define FEATURE_FLAG0_NIC_CAPABILITY_V2 		(1 << 0)

#define CFG_DUAL_P2PLIKE_INTERFACE 	0

#define RUNNING_P2P_MODE            0
#define RUNNING_AP_MODE             1
#define RUNNING_DUAL_AP_MODE        2
#define RUNNING_P2P_AP_MODE         3
#define RUNNING_P2P_MODE_NUM        4

/* TODO: Clean below codes */
#define CFG_SUPPORT_REPLAY_DETECTION 	1

#define CFG_SUPPORT_LAST_SEC_MCS_INFO 	1
#define MCS_INFO_SAMPLE_CNT 			10 /* inherit */


#define CFG_SUPPORT_EASY_DEBUG 			1
#define CFG_SUPPORT_FW_DBG_LEVEL_CTRL 	1

#define CFG_EFUSE_BUFFER_MODE_DELAY_CAL 1
#define CFG_EEPROM_PAGE_ACCESS 			1
#define CFG_SUPPORT_WIFI_HOST_OFFLOAD 	1
#define CFG_SUPPORT_DBDC 				1

#define CFG_FW_NAME_MAX_LEN 			(64)

#define CFG_SUPPORT_LOW_POWER_DEBUG 	1
#define CFG_SISO_SW_DEVELOP 			1
#define CFG_SUPPORT_ANT_SELECT 			1

#define CFG_SUPPORT_SINGLE_SKU 			1
#define CFG_SUPPORT_SINGLE_SKU_LOCAL_DB 1

#define CFG_SDIO_ASYNC_IRQ_AUTO_ENABLE 	1
#define CFG_SUPPORT_ADVANCE_CONTROL 	1
#define CFG_PRE_ALLOCATION_IO_BUFFER 	0
#define CFG_SUPPORT_RSSI_COMP 			1
#define CFG_SUPPORT_H2E 				1

#define CFG_SUPPORT_AAA_CHECK_NO_SSID 	1 /* >0?*/

/*******************************************************************************
 *                             D A T A   T Y P E S
 ********************************************************************************
 */

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

/*******************************************************************************
 *                              F U N C T I O N S
 ********************************************************************************
 */
#endif /* _CONFIG_H */
