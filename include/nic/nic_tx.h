/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   nic_tx.h
 *    \brief  Functions that provide TX operation in NIC's point of view.
 *
 *    This file provides TX functions which are responsible for both Hardware
 * and Software Resource Management and keep their Synchronization.
 *
 */

#ifndef _NIC_TX_H
#define _NIC_TX_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */

#define UNIFIED_MAC_TX_FORMAT		      1

#define MAC_TX_RESERVED_FIELD		      0

#define NIC_TX_RESOURCE_POLLING_TIMEOUT	      256
#define NIC_TX_RESOURCE_POLLING_DELAY_MSEC    5

#define NIC_TX_CMD_INFO_RESERVED_COUNT	      4

/* Maximum buffer count for individual HIF TCQ */
#define NIC_TX_PAGE_COUNT_TC0 \
	(NIC_TX_BUFF_COUNT_TC0 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_PAGE_COUNT_TC1 \
	(NIC_TX_BUFF_COUNT_TC1 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_PAGE_COUNT_TC2 \
	(NIC_TX_BUFF_COUNT_TC2 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_PAGE_COUNT_TC3 \
	(NIC_TX_BUFF_COUNT_TC3 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_PAGE_COUNT_TC4 \
	(NIC_TX_BUFF_COUNT_TC4 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_PAGE_COUNT_TC5 \
	(NIC_TX_BUFF_COUNT_TC5 * NIC_TX_MAX_PAGE_PER_FRAME)

#define NIC_TX_BUFF_COUNT_TC0	      HIF_TX_BUFF_COUNT_TC0
#define NIC_TX_BUFF_COUNT_TC1	      HIF_TX_BUFF_COUNT_TC1
#define NIC_TX_BUFF_COUNT_TC2	      HIF_TX_BUFF_COUNT_TC2
#define NIC_TX_BUFF_COUNT_TC3	      HIF_TX_BUFF_COUNT_TC3
#define NIC_TX_BUFF_COUNT_TC4	      HIF_TX_BUFF_COUNT_TC4
#define NIC_TX_BUFF_COUNT_TC5	      HIF_TX_BUFF_COUNT_TC5

#define NIC_TX_RESOURCE_CTRL \
	HIF_TX_RESOURCE_CTRL /* to enable/disable TX resource control */

#if CFG_ENABLE_FW_DOWNLOAD

#define NIC_TX_INIT_BUFF_COUNT_TC0    8
#define NIC_TX_INIT_BUFF_COUNT_TC1    0
#define NIC_TX_INIT_BUFF_COUNT_TC2    0
#define NIC_TX_INIT_BUFF_COUNT_TC3    0
#define NIC_TX_INIT_BUFF_COUNT_TC4    8
#define NIC_TX_INIT_BUFF_COUNT_TC5    0

#define NIC_TX_INIT_BUFF_SUM					   \
	(NIC_TX_INIT_BUFF_COUNT_TC0 + NIC_TX_INIT_BUFF_COUNT_TC1 + \
	 NIC_TX_INIT_BUFF_COUNT_TC2 + NIC_TX_INIT_BUFF_COUNT_TC3 + \
	 NIC_TX_INIT_BUFF_COUNT_TC4 + NIC_TX_INIT_BUFF_COUNT_TC5)

#define NIC_TX_INIT_PAGE_COUNT_TC0 \
	(NIC_TX_INIT_BUFF_COUNT_TC0 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_INIT_PAGE_COUNT_TC1 \
	(NIC_TX_INIT_BUFF_COUNT_TC1 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_INIT_PAGE_COUNT_TC2 \
	(NIC_TX_INIT_BUFF_COUNT_TC2 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_INIT_PAGE_COUNT_TC3 \
	(NIC_TX_INIT_BUFF_COUNT_TC3 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_INIT_PAGE_COUNT_TC4 \
	(NIC_TX_INIT_BUFF_COUNT_TC4 * NIC_TX_MAX_PAGE_PER_FRAME)
#define NIC_TX_INIT_PAGE_COUNT_TC5 \
	(NIC_TX_INIT_BUFF_COUNT_TC5 * NIC_TX_MAX_PAGE_PER_FRAME)

#endif

#define NIC_TX_ENABLE_SECOND_HW_QUEUE	  0

/* 4 TODO: The following values shall be got from FW by query CMD */
/*------------------------------------------------------------------------*/
/* Resource Management related information                                */
/*------------------------------------------------------------------------*/
#define NIC_TX_PAGE_SIZE_IS_POWER_OF_2	  true
#define NIC_TX_PAGE_SIZE_IN_POWER_OF_2	  HIF_TX_PAGE_SIZE_IN_POWER_OF_2
#define NIC_TX_PAGE_SIZE		  HIF_TX_PAGE_SIZE

/* For development only */
#define NIC_TX_MAX_SIZE_PER_FRAME \
	1532 /* calculated by MS native 802.11 format */
#define NIC_TX_MAX_PAGE_PER_FRAME					       \
	((NIC_TX_DESC_AND_PADDING_LENGTH + NIC_TX_DESC_HEADER_PADDING_LENGTH + \
	  NIC_TX_MAX_SIZE_PER_FRAME + NIC_TX_PAGE_SIZE - 1) /		       \
	 NIC_TX_PAGE_SIZE)

/*------------------------------------------------------------------------*/
/* Tx descriptor related information                                      */
/*------------------------------------------------------------------------*/

/* Frame Buffer
 *  |<--Tx Descriptor-->|<--Tx descriptor padding-->|<--802.3/802.11
 * Header-->|<--Header padding-->|<--Payload-->|
 */

/* Tx descriptor length by format (TXD.FT) */
#define NIC_TX_DESC_LONG_FORMAT_LENGTH_DW	   8 /* in unit of double word */
#define NIC_TX_DESC_LONG_FORMAT_LENGTH \
	DWORD_TO_BYTE(NIC_TX_DESC_LONG_FORMAT_LENGTH_DW)
#define NIC_TX_DESC_SHORT_FORMAT_LENGTH_DW	   3 /* in unit of double word */
#define NIC_TX_DESC_SHORT_FORMAT_LENGTH	\
	DWORD_TO_BYTE(NIC_TX_DESC_SHORT_FORMAT_LENGTH_DW)

/* Tx descriptor padding length (DMA.MICR.TXDSCR_PAD) */
#define NIC_TX_DESC_PADDING_LENGTH_DW		   0 /* in unit of double word */
#define NIC_TX_DESC_PADDING_LENGTH		   DWORD_TO_BYTE( \
		NIC_TX_DESC_PADDING_LENGTH_DW)

#define NIC_TX_PSE_HEADER_LENGTH		   4

#define NIC_TX_DESC_AND_PADDING_LENGTH \
	(NIC_TX_DESC_LONG_FORMAT_LENGTH + NIC_TX_DESC_PADDING_LENGTH)

/* Tx header padding (TXD.HeaderPadding)  */
/* Warning!! To use MAC header padding, every Tx packet must be decomposed */
#define NIC_TX_DESC_HEADER_PADDING_LENGTH	   0 /* in unit of bytes */

#define NIC_TX_DEFAULT_WLAN_INDEX \
	31 /* For Tx packets to peer who has no WLAN table index. */

#define NIC_TX_DESC_PID_RESERVED		   0
#define NIC_TX_DESC_DRIVER_PID_MIN		   1
#define NIC_TX_DESC_DRIVER_PID_MAX		   127

#define NIC_TX_DATA_DEFAULT_RETRY_COUNT_LIMIT	   30
#define NIC_TX_MGMT_DEFAULT_RETRY_COUNT_LIMIT	   30

#define NIC_TX_AC_BE_REMAINING_TX_TIME \
	TX_DESC_TX_TIME_NO_LIMIT /* in unit of ms */
#define NIC_TX_AC_BK_REMAINING_TX_TIME \
	TX_DESC_TX_TIME_NO_LIMIT /* in unit of ms */
#define NIC_TX_AC_VO_REMAINING_TX_TIME \
	TX_DESC_TX_TIME_NO_LIMIT /* in unit of ms */
#define NIC_TX_AC_VI_REMAINING_TX_TIME \
	TX_DESC_TX_TIME_NO_LIMIT /* in unit of ms */
#define NIC_TX_MGMT_REMAINING_TX_TIME		   2000 /* in unit of ms */

#define NIC_TX_CRITICAL_DATA_TID		   7
#define NIC_TX_AC_VO_TID			   6
#define NIC_TX_AC_VI_TID			   5
#define NIC_TX_AC_BE_TID			   0
#define NIC_TX_AC_BK_TID			   1

#define HW_MAC_TX_DESC_APPEND_T_LENGTH		   44
#define NIC_TX_HEAD_ROOM					       \
	(NIC_TX_DESC_LONG_FORMAT_LENGTH + NIC_TX_DESC_PADDING_LENGTH + \
	 HW_MAC_TX_DESC_APPEND_T_LENGTH)

/*------------------------------------------------------------------------*/
/* Tx status related information                                          */
/*------------------------------------------------------------------------*/

/* Tx status header & content length */
#define NIC_TX_STATUS_HEADER_LENGTH_DW		   1 /* in unit of double word */
#define NIC_TX_STATUS_HEADER_LENGTH \
	DWORD_TO_BYTE(NIC_TX_STATUS_HEADER_LENGTH_DW)
#define NIC_TX_STATUS_LENGTH_DW			   7 /* in unit of double word */
#define NIC_TX_STATUS_LENGTH			   DWORD_TO_BYTE( \
		NIC_TX_STATUS_LENGTH_DW)

/*------------------------------------------------------------------------*/
/* Tx descriptor field related information */
/*------------------------------------------------------------------------*/
/* DW 0 */
#define TX_DESC_TX_BYTE_COUNT_MASK		   BITS(0, 15)
#define TX_DESC_TX_BYTE_COUNT_OFFSET		   0

#define TX_DESC_ETHER_TYPE_OFFSET_MASK		   BITS(0, 6)
#define TX_DESC_ETHER_TYPE_OFFSET_OFFSET	   0
#define TX_DESC_IP_CHKSUM_OFFLOAD		   BIT(7)
#define TX_DESC_TCP_UDP_CHKSUM_OFFLOAD		   BIT(0)
#define TX_DESC_USB_NEXT_VLD			   BIT(1)
#define TX_DESC_USB_TX_BURST			   BIT(2)
#define TX_DESC_QUEUE_INDEX_MASK		   BITS(2, 6)
#define TX_DESC_QUEUE_INDEX_OFFSET		   2
#define TX_DESC_PORT_INDEX			   BIT(7)
#define TX_DESC_PORT_INDEX_OFFSET		   7

#define PORT_INDEX_LMAC				   0
#define PORT_INDEX_MCU				   1

/* DW 1 */
#define TX_DESC_WLAN_INDEX_MASK			   BITS(0, 7)
#define TX_DESC_WLAN_INDEX_OFFSET		   0
#define TX_DESC_HEADER_FORMAT_MASK		   BITS(5, 6)
#define TX_DESC_HEADER_FORMAT_OFFSET		   5

#define HEADER_FORMAT_NON_802_11		   0 /* Non-802.11 */
#define HEADER_FORMAT_COMMAND			   1 /* Command */
#define HEADER_FORMAT_802_11_NORMAL_MODE	   2 /* 802.11 (normal mode) */
#define HEADER_FORMAT_802_11_ENHANCE_MODE	   3 /* 802.11 (Enhancement mode) */
#define HEADER_FORMAT_802_11_MASK		   BIT(1)

#define TX_DESC_NON_802_11_MORE_DATA		   BIT(0)
#define TX_DESC_NON_802_11_EOSP			   BIT(1)
#define TX_DESC_NON_802_11_REMOVE_VLAN		   BIT(2)
#define TX_DESC_NON_802_11_VLAN_FIELD		   BIT(3)
#define TX_DESC_NON_802_11_ETHERNET_II		   BIT(4)
#define TX_DESC_NOR_802_11_HEADER_LENGTH_MASK	   BITS(0, 4)
#define TX_DESC_NOR_802_11_HEADER_LENGTH_OFFSET	   0
#define TX_DESC_ENH_802_11_EOSP			   BIT(1)
#define TX_DESC_ENH_802_11_AMSDU		   BIT(2)

#define TX_DESC_FORMAT				   BIT(7)
#define TX_DESC_SHORT_FORMAT			   0
#define TX_DESC_LONG_FORMAT			   1

#define TX_DESC_TXD_LENGTH_MASK			   BIT(0)
#define TX_DESC_TXD_LENGTH_OFFSET		   0

#define TX_DESC_HEADER_PADDING_LENGTH_MASK	   BIT(1)
#define TX_DESC_HEADER_PADDING_LENGTH_OFFSET	   1
#define TX_DESC_HEADER_PADDING_MODE		   BIT(2)

#define TX_DESC_TXD_EXTEND_LENGTH_MASK		   BIT(3)
#define TX_DESC_TXD_EXTEND_LENGTH_OFFSET	   3

#define TX_DESC_TXD_UTXB_AMSDU_MASK		   BIT(4)
#define TX_DESC_TXD_UTXB_AMSDU_OFFSET		   4

#define TX_DESC_TID_MASK			   BITS(5, 7)
#define TX_DESC_TID_OFFSET			   5
#define TX_DESC_TID_NUM				   8

#define TX_DESC_PACKET_FORMAT_MASK		   BITS(0, 1) /* SW Field */
#define TX_DESC_PACKET_FORMAT_OFFSET		   0
#define TX_DESC_OWN_MAC_MASK			   BITS(2, 7)
#define TX_DESC_OWN_MAC_OFFSET			   2

/* DW 2 */
#define TX_DESC_SUB_TYPE_MASK			   BITS(0, 3)
#define TX_DESC_SUB_TYPE_OFFSET			   0
#define TX_DESC_TYPE_MASK			   BITS(4, 5)
#define TX_DESC_TYPE_OFFSET			   4
#define TX_DESC_NDP				   BIT(6)
#define TX_DESC_NDPA				   BIT(7)

#define TX_DESC_SOUNDING			   BIT(0)
#define TX_DESC_FORCE_RTS_CTS			   BIT(1)
#define TX_DESC_BROADCAST_MULTICAST		   BIT(2)
#define TX_DESC_BIP_PROTECTED			   BIT(3)
#define TX_DESC_DURATION_FIELD_CONTROL		   BIT(4)
#define TX_DESC_HTC_EXISTS			   BIT(5)
#define TX_DESC_FRAGMENT_MASK			   BITS(6, 7)
#define TX_DESC_FRAGMENT_OFFSET			   6
#define FRAGMENT_FISRT_PACKET			   1
#define FRAGMENT_MIDDLE_PACKET			   2
#define FRAGMENT_LAST_PACKET			   3

#define TX_DESC_REMAINING_MAX_TX_TIME		   BITS(0, 7)
#define TX_DESC_TX_TIME_NO_LIMIT		   0
/* Unit of life time calculation of Tx descriptor */
#define TX_DESC_LIFE_TIME_UNIT_IN_POWER_OF_2	   5
#define TX_DESC_LIFE_TIME_UNIT			   POWER_OF_2( \
		TX_DESC_LIFE_TIME_UNIT_IN_POWER_OF_2)
#define TX_DESC_POWER_OFFSET_MASK		   BITS(0, 4)
#define TX_DESC_BA_DISABLE			   BIT(5)
#define TX_DESC_TIMING_MEASUREMENT		   BIT(6)
#define TX_DESC_FIXED_RATE			   BIT(7)

/* DW 3 */
#define TX_DESC_NO_ACK				   BIT(0)
#define TX_DESC_PROTECTED_FRAME			   BIT(1)
#define TX_DESC_EXTEND_MORE_DATA		   BIT(2)
#define TX_DESC_EXTEND_EOSP			   BIT(3)

#define TX_DESC_SW_RESERVED_MASK		   BITS(4, 5)
#define TX_DESC_SW_RESERVED_OFFSET		   4

#define TX_DESC_TX_COUNT_MASK			   BITS(6, 10)
#define TX_DESC_TX_COUNT_OFFSET			   6
#define TX_DESC_TX_COUNT_NO_ATTEMPT		   0
#define TX_DESC_TX_COUNT_NO_LIMIT		   31
#define TX_DESC_REMAINING_TX_COUNT_MASK		   BITS(11, 15)
#define TX_DESC_REMAINING_TX_COUNT_OFFSET	   11
#define TX_DESC_SEQUENCE_NUMBER			   BITS(0, 11)
#define TX_DESC_HW_RESERVED_MASK		   BITS(12, 13)
#define TX_DESC_HW_RESERVED_OFFSET		   12
#define TX_DESC_PN_IS_VALID			   BIT(14)
#define TX_DESC_SN_IS_VALID			   BIT(15)

/* DW 4 */
#define TX_DESC_PN_PART1			   BITS(0, 31)

/* DW 5 */
#define TX_DESC_PACKET_ID			   BIT(0, 7)
#define TX_DESC_TX_STATUS_FORMAT		   BIT(0)
#define TX_DESC_TX_STATUS_FORMAT_OFFSET		   0
#define TX_DESC_TX_STATUS_TO_MCU		   BIT(1)
#define TX_DESC_TX_STATUS_TO_HOST		   BIT(2)
#define TX_DESC_DA_SOURCE			   BIT(3)
#define TX_DESC_POWER_MANAGEMENT_CONTROL	   BIT(5)
#define TX_DESC_PN_PART2			   BITS(0, 15)

/* DW 6 */ /* FR = 1 */
#define TX_DESC_BANDWIDTH_MASK			   BITS(0, 2)
#define TX_DESC_BANDWIDTH_OFFSET		   0
#define TX_DESC_DYNAMIC_BANDWIDTH		   BIT(3)
#define TX_DESC_ANTENNA_INDEX_MASK		   BITS(4, 15)
#define TX_DESC_ANTENNA_INDEX_OFFSET		   4

#define TX_DESC_FIXDE_RATE_MASK			   BITS(0, 11)
#define TX_DESC_FIXDE_RATE_OFFSET		   0
#define TX_DESC_TX_RATE				   BITS(0, 5)
#define TX_DESC_TX_RATE_OFFSET			   0
#define TX_DESC_TX_MODE				   BITS(6, 8)
#define TX_DESC_TX_MODE_OFFSET			   6
#define TX_DESC_NSTS_MASK			   BITS(9, 10)
#define TX_DESC_NSTS_OFFSET			   9
#define TX_DESC_STBC				   BIT(11)
#define TX_DESC_BF				   BIT(12)
#define TX_DESC_LDPC				   BIT(13)
#define TX_DESC_GUARD_INTERVAL			   BIT(14)
#define TX_DESC_FIXED_RATE_MODE			   BIT(15)

/* DW 7 */
#define TX_DESC_SPE_EXT_IDX_MASK		   BITS(11, 15)
#define TX_DESC_SPE_EXT_IDX_OFFSET		   11

#if CFG_ENABLE_PKT_LIFETIME_PROFILE
#define NIC_TX_TIME_THRESHOLD			   100 /* in unit of ms */
#endif

#define NIC_TX_INIT_CMD_PORT			   HIF_TX_INIT_CMD_PORT

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

/* 3 */ /* Session for TX QUEUES */
/* The definition in this ENUM is used to categorize packet's Traffic Class
 * according to the their TID(User Priority). In order to achieve QoS goal, a
 * particular TC should not block the process of another packet with different
 * TC. In current design we will have 5 categories(TCs) of SW resource.
 */
/* TXD_PKT_FORMAT options*/
typedef enum _ENUM_TXD_PKT_FORMAT_OPTION_T {
	TXD_PKT_FORMAT_TXD = 0, /* TXD only */
	TXD_PKT_FORMAT_TXD_PAYLOAD, /* TXD and paload */
	TXD_PKT_FORMAT_COMMAND, /* Command */
	TXD_PKT_FORMAT_FWDL, /* Firmware download */
	TXD_PKT_FORMAT_NUM,
} ENUM_TXD_PKT_FORMAT_OPTION_T;

/* HIF Tx interrupt status queue index*/
typedef enum _ENUM_HIF_TX_INDEX_T {
	HIF_TX_AC0_INDEX = 0,
	HIF_TX_AC1_INDEX,
	HIF_TX_AC2_INDEX,
	HIF_TX_AC3_INDEX,

	HIF_TX_AC10_INDEX,
	HIF_TX_AC11_INDEX,
	HIF_TX_AC12_INDEX,
	HIF_TX_AC13_INDEX,

	HIF_TX_AC20_INDEX,
	HIF_TX_AC21_INDEX,
	HIF_TX_AC22_INDEX,
	HIF_TX_AC23_INDEX,

	HIF_TX_RSV0_INDEX,
	HIF_TX_RSV1_INDEX,
	HIF_TX_FFA_INDEX,
	HIF_TX_CPU_INDEX,

	HIF_TX_NUM
} ENUM_HIF_TX_INDEX_T;

#if (HIF_TX_RSRC_WMM_ENHANCE == 1)
#define HIF_TX_AC3X_INDEX    HIF_TX_RSV0_INDEX
#endif

/* LMAC Tx queue index */
typedef enum _ENUM_MAC_TXQ_INDEX_T {
	MAC_TXQ_AC0_INDEX = 0,
	MAC_TXQ_AC1_INDEX,
	MAC_TXQ_AC2_INDEX,
	MAC_TXQ_AC3_INDEX,

	MAC_TXQ_AC10_INDEX,
	MAC_TXQ_AC11_INDEX,
	MAC_TXQ_AC12_INDEX,
	MAC_TXQ_AC13_INDEX,

	MAC_TXQ_AC20_INDEX,
	MAC_TXQ_AC21_INDEX,
	MAC_TXQ_AC22_INDEX,
	MAC_TXQ_AC23_INDEX,

	MAC_TXQ_AC30_INDEX,
	MAC_TXQ_AC31_INDEX,
	MAC_TXQ_AC32_INDEX,
	MAC_TXQ_AC33_INDEX,

	MAC_TXQ_ALTX_0_INDEX,
	MAC_TXQ_BMC_0_INDEX,
	MAC_TXQ_BCN_0_INDEX,
	MAC_TXQ_PSMP_0_INDEX,

	MAC_TXQ_ALTX_1_INDEX,
	MAC_TXQ_BMC_1_INDEX,
	MAC_TXQ_BCN_1_INDEX,
	MAC_TXQ_PSMP_1_INDEX,

	MAC_TXQ_NAF_INDEX,
	MAC_TXQ_NBCN_INDEX,

	MAC_TXQ_NUM
} ENUM_MAC_TXQ_INDEX_T;

/* MCU quque index */
typedef enum _ENUM_MCU_Q_INDEX_T {
	MCU_Q0_INDEX = 0,
	MCU_Q1_INDEX,
	MCU_Q2_INDEX,
	MCU_Q3_INDEX,
	MCU_Q_NUM
} ENUM_MCU_Q_INDEX_T;

/* Tc Resource index */
typedef enum _ENUM_TRAFFIC_CLASS_INDEX_T {
	/*First HW queue */
	TC0_INDEX = 0, /* HIF TX: AC0 packets */
	TC1_INDEX, /* HIF TX: AC1 packets */
	TC2_INDEX, /* HIF TX: AC2 packets */
	TC3_INDEX, /* HIF TX: AC3 packets */
	TC4_INDEX, /* HIF TX: CPU packets */
	TC5_INDEX, /* HIF TX: AC4 packets */

#if (HIF_TX_RSRC_WMM_ENHANCE == 1)
	TC6_INDEX, /* HIF TX: AC10 packets */
	TC7_INDEX, /* HIF TX: AC11 packets */
	TC8_INDEX, /* HIF TX: AC12 packets */
	TC9_INDEX, /* HIF TX: AC13 packets */
	TC10_INDEX, /* HIF TX: AC14 packets */

	TC11_INDEX, /* HIF TX: AC20 packets */
	TC12_INDEX, /* HIF TX: AC21 packets */
	TC13_INDEX, /* HIF TX: AC22 packets */
	TC14_INDEX, /* HIF TX: AC23 packets */
	TC15_INDEX, /* HIF TX: AC24 packets */

	TC16_INDEX, /* HIF TX: AC3x packets */
#endif

/* Second HW queue */
#if NIC_TX_ENABLE_SECOND_HW_QUEUE
	TC6_INDEX, /* HIF TX: AC10 packets */
	TC7_INDEX, /* HIF TX: AC11 packets */
	TC8_INDEX, /* HIF TX: AC12 packets */
	TC9_INDEX, /* HIF TX: AC13 packets */
	TC10_INDEX, /* HIF TX: AC14 packets */
#endif

	TC_NUM /* Maximum number of Traffic Classes. */
} ENUM_TRAFFIC_CLASS_INDEX_T;
#if (HIF_TX_RSRC_WMM_ENHANCE == 1)
#define HIF_WMM_SET_NUM	      HW_WMM_NUM
#endif

/* +1 for DBDC */
#define TX_PORT_NUM	      (TC_NUM + 1)

#define TX_2G_WMM_PORT_NUM    (TC_NUM)

#define BMC_TC_INDEX	      TC1_INDEX

/* per-Network Tc Resource index */
typedef enum _ENUM_NETWORK_TC_RESOURCE_INDEX_T {
	/* QoS Data frame, WMM AC index */
	NET_TC_WMM_AC_BE_INDEX = 0,
	NET_TC_WMM_AC_BK_INDEX,
	NET_TC_WMM_AC_VI_INDEX,
	NET_TC_WMM_AC_VO_INDEX,
	/* Mgmt frame */
	NET_TC_MGMT_INDEX,
	/* nonQoS / non StaRec frame (BMC/non-associated frame) */
	NET_TC_BMC_INDEX,

	NET_TC_NUM
} ENUM_NETWORK_TC_RESOURCE_INDEX_T;

typedef enum _ENUM_TX_STATISTIC_COUNTER_T {
	TX_MPDU_TOTAL_COUNT = 0,
	TX_INACTIVE_BSS_DROP,
	TX_INACTIVE_STA_DROP,
	TX_FORWARD_OVERFLOW_DROP,
	TX_AP_BORADCAST_DROP,
	TX_INVALID_MSDUINFO_COUNT,
	TX_DROP_PID_COUNT,
	TX_STATISTIC_COUNTER_NUM
} ENUM_TX_STATISTIC_COUNTER_T;

typedef enum _ENUM_FIX_BW_T {
	FIX_BW_NO_FIXED = 0,
	FIX_BW_20 = 4,
	FIX_BW_40,
	FIX_BW_80,
	FIX_BW_160,
	FIX_BW_NUM
} ENUM_FIX_BW_T;

typedef enum _ENUM_MSDU_OPTION_T {
	MSDU_OPT_NO_ACK = BIT(0),
	MSDU_OPT_NO_AGGREGATE = BIT(1),
	MSDU_OPT_TIMING_MEASURE = BIT(2),
	MSDU_OPT_RCPI_NOISE_STATUS = BIT(3),

	/* Option by Frame Format */
	/* Non-80211 */
	MSDU_OPT_MORE_DATA = BIT(4),
	MSDU_OPT_REMOVE_VLAN = BIT(5), /* Remove VLAN tag if exists */

	/* 80211-enhanced */
	MSDU_OPT_AMSDU = BIT(6),

	/* 80211-enhanced & Non-80211 */
	MSDU_OPT_EOSP = BIT(7),

	/* Beamform */
	MSDU_OPT_NDP = BIT(8),
	MSDU_OPT_NDPA = BIT(9),
	MSDU_OPT_SOUNDING = BIT(10),

	/* Protection */
	MSDU_OPT_FORCE_RTS = BIT(11),

	/* Security */
	MSDU_OPT_BIP = BIT(12),
	MSDU_OPT_PROTECTED_FRAME = BIT(13),

	/* SW Field */
	MSDU_OPT_SW_DURATION = BIT(14),
	MSDU_OPT_SW_PS_BIT = BIT(15),
	MSDU_OPT_SW_HTC = BIT(16),
	MSDU_OPT_SW_BAR_SN = BIT(17),

	/* Manual Mode */
	MSDU_OPT_MANUAL_FIRST_BIT = BIT(18),

	MSDU_OPT_MANUAL_LIFE_TIME = MSDU_OPT_MANUAL_FIRST_BIT,
	MSDU_OPT_MANUAL_RETRY_LIMIT = BIT(19),
	MSDU_OPT_MANUAL_POWER_OFFSET = BIT(20),
	MSDU_OPT_MANUAL_TX_QUE = BIT(21),
	MSDU_OPT_MANUAL_SN = BIT(22),

	MSDU_OPT_MANUAL_LAST_BIT = MSDU_OPT_MANUAL_SN
} ENUM_MSDU_OPTION_T;

typedef enum _ENUM_MSDU_CONTROL_FLAG_T {
	MSDU_CONTROL_FLAG_FORCE_TX = BIT(0)
} ENUM_MSDU_CONTROL_FLAG_T;

typedef enum _ENUM_MSDU_RATE_MODE_T {
	MSDU_RATE_MODE_AUTO = 0,
	MSDU_RATE_MODE_MANUAL_DESC,
	/* The following rate mode is not implemented yet */
	/* DON'T use!!! */
	MSDU_RATE_MODE_MANUAL_CR,
	MSDU_RATE_MODE_LOWEST_RATE
} ENUM_MSDU_RATE_MODE_T;

typedef enum _ENUM_DATA_RATE_MODE_T {
	DATA_RATE_MODE_AUTO = 0,
	DATA_RATE_MODE_MANUAL,
	DATA_RATE_MODE_BSS_LOWEST
} ENUM_DATA_RATE_MODE_T;

typedef struct _TX_TCQ_STATUS_T {
	/* HIF reported page count delta */
	u32 au4TxDonePageCount[TC_NUM]; /* other TC */
	u32 au4PreUsedPageCount[TC_NUM];
	u32 u4AvaliablePageCount; /* FFA */
	u8 ucNextTcIdx; /* For round-robin distribute free page count */

	/* distributed page count */
	u32 au4FreePageCount[TC_NUM];
	u32 au4MaxNumOfPage[TC_NUM];

	/* buffer count */
	u32 au4FreeBufferCount[TC_NUM];
	u32 au4MaxNumOfBuffer[TC_NUM];

#if (HIF_TX_RSRC_WMM_ENHANCE == 1)
	u8 ucNextHifWmmIdx;
#endif
} TX_TCQ_STATUS_T, *P_TX_TCQ_STATUS_T;

typedef struct _TX_TCQ_ADJUST_T {
	s32 ai4Variation[TC_NUM];
} TX_TCQ_ADJUST_T, *P_TX_TCQ_ADJUST_T;

typedef struct _TX_CTRL_T {
	u32 u4TxCachedSize;
	u8 *pucTxCached;

	u32 u4PageSize;

	u32 u4TotalPageNum;

	u32 u4TotalTxRsvPageNum;

	/* Elements below is classified according to TC (Traffic Class) value.
	 */

	TX_TCQ_STATUS_T rTc;

	u8 *pucTxCoalescingBufPtr;

	u32 u4WrIdx;

	QUE_T rFreeMsduInfoList;

	/* Management Frame Tracking */
	/* number of management frames to be sent */
	s32 i4TxMgmtPendingNum;

	/* to tracking management frames need TX done callback */
	QUE_T rTxMgmtTxingQueue;

#if CFG_HIF_STATISTICS
	u32 u4TotalTxAccessNum;
	u32 u4TotalTxPacketNum;
#endif
	u32 au4Statistics[TX_STATISTIC_COUNTER_NUM];

	/* Number to track forwarding frames */
	s32 i4PendingFwdFrameCount;

	/* enable/disable TX resource control */
	u8 fgIsTxResourceCtrl;
} TX_CTRL_T, *P_TX_CTRL_T;

typedef enum _ENUM_TX_PACKET_TYPE_T {
	TX_PACKET_TYPE_DATA = 0,
	TX_PACKET_TYPE_MGMT,
	/* TX_PACKET_TYPE_1X, */
	X_PACKET_TYPE_NUM
} ENUM_TX_PACKET_TYPE_T,
*P_ENUM_TX_PACKET_TYPE_T;

typedef enum _ENUM_TX_PACKET_SRC_T {
	TX_PACKET_OS,
	TX_PACKET_OS_OID,
	TX_PACKET_FORWARDING,
	TX_PACKET_MGMT,
	TX_PACKET_NUM
} ENUM_TX_PACKET_SRC_T;

/* TX Call Back Function  */
typedef WLAN_STATUS (*PFN_TX_DONE_HANDLER)(IN P_ADAPTER_T prAdapter,
					   IN P_MSDU_INFO_T prMsduInfo,
					   IN ENUM_TX_RESULT_CODE_T
					   rTxDoneStatus);

#if CFG_ENABLE_PKT_LIFETIME_PROFILE
typedef struct _PKT_PROFILE_T {
	u8 fgIsValid;
#if CFG_PRINT_PKT_LIFETIME_PROFILE
	u8 fgIsPrinted;
	u16 u2IpSn;
	u16 u2RtpSn;
	u8 ucTcxFreeCount;
#endif
	u32 rHardXmitArrivalTimestamp;
	u32 rEnqueueTimestamp;
	u32 rDequeueTimestamp;
	u32 rHifTxDoneTimestamp;
} PKT_PROFILE_T, *P_PKT_PROFILE_T;
#endif
/* TX transactions could be divided into 4 kinds:
 *
 * 1) 802.1X / Bluetooth-over-Wi-Fi Security Frames
 *    [CMD_INFO_T] - [prPacket] - in skb or NDIS_PACKET form
 *
 * 2) MMPDU
 *    [CMD_INFO_T] - [prPacket] - [MSDU_INFO_T] - [prPacket] - direct buffer for
 * frame body
 *
 * 3) Command Packets
 *    [CMD_INFO_T] - [pucInfoBuffer] - direct buffer for content of command
 * packet
 *
 * 4) Normal data frame
 *    [MSDU_INFO_T] - [prPacket] - in skb or NDIS_PACKET form
 */

/* PS_FORWARDING_TYPE_NON_PS means that the receiving STA is in Active Mode
 *   from the perspective of host driver (maybe not synchronized with FW --> SN
 * is needed)
 */

struct _MSDU_INFO_T {
	QUE_ENTRY_T rQueEntry;
	P_NATIVE_PACKET prPacket; /* Pointer to packet buffer */

	ENUM_TX_PACKET_SRC_T eSrc; /* specify OS/FORWARD packet */
	u8 ucUserPriority; /* QoS parameter, convert to TID */

	/* For composing TX descriptor header */
	u8 ucTC; /* Traffic Class: 0~4 (HIF TX0), 5 (HIF TX1) */
	u8 ucPacketType; /* 0: Data, 1: Management Frame */
	u8 ucStaRecIndex; /* STA_REC index */
	u8 ucBssIndex; /* BSS_INFO_T index */
	u8 ucWlanIndex; /* Wlan entry index */
	u8 ucPacketFormat; /* TXD.DW1[25:24] Packet Format */

	u8 fgIs802_1x; /* true: 802.1x frame */
	u8 fgIs802_1x_NonProtected; /* true: 802.1x frame - Non-Protected */
	u8 fgIs802_11; /* true: 802.11 header is present */
	u8 fgIs802_3; /* true: 802.3 frame */
	u8 fgIsVlanExists; /* true: VLAN tag is exists */

	/* u8                     fgIsBIP;                */ /* Management Frame
	 *                                                      Protection
	 */
	/* u8                     fgIsBasicRate;      */ /* Force Basic Rate
	 *                                                  Transmission
	 */
	/* u8                     fgIsMoreData;      */ /* More data */
	/* u8                     fgIsEOSP;            */ /* End of service
	 *                                                   period
	 */

	/* Special Option */
	u32 u4Option; /* Special option in bitmask, no ACK, etc... */
	s8 cPowerOffset; /* Per-packet power offset, in 2's complement */
	u16 u2SwSN; /* SW assigned sequence number */
	u8 ucRetryLimit; /* The retry limit */
	u32 u4RemainingLifetime; /* Remaining lifetime, unit:ms */

	/* Control flag */
	u8 ucControlFlag; /* Control flag in bitmask */

	/* Fixed Rate Option */
	u8 ucRateMode; /* Rate mode: AUTO, MANUAL_DESC, MANUAL_CR */
	u32 u4FixedRateOption; /* The rate option, rate code, GI, etc... */

	u8 fgIsTXDTemplateValid; /* There is a valid Tx descriptor for this
	                          * packet */

	/* flattened from PACKET_INFO_T */
	u8 ucMacHeaderLength; /* MAC header legth */
	u8 ucLlcLength; /* w/o EtherType */
	u16 u2FrameLength; /* Total frame length */
	u8 aucEthDestAddr[MAC_ADDR_LEN]; /* Ethernet Destination Address */
	u32 u4PageCount; /* Required page count for this MSDU */

	/* for TX done tracking */
	u8 ucTxSeqNum; /* MGMT frame serial number */
	u8 ucPID; /* PID */
	u8 ucWmmQueSet; /* WMM Set */
	PFN_TX_DONE_HANDLER pfTxDoneHandler; /* Tx done handler */
	u32 u4TxDoneTag; /* Tag for data frame Tx done log */

#if CFG_ENABLE_PKT_LIFETIME_PROFILE
	PKT_PROFILE_T rPktProfile;
#endif

	/* To be removed  */
	u8 ucFormatID; /* 0: MAUI, Linux, Windows NDIS 5.1 */
	u8 ucPsForwardingType; /* See ENUM_PS_FORWARDING_TYPE_T */

	/* Compose TxDesc in main_thread and place here */
	u8 aucTxDescBuffer[NIC_TX_DESC_AND_PADDING_LENGTH];

	/* sanity drop flag */
	u8 fgDrop;
};

#define HIT_PKT_FLAGS_CT_WITH_TXD	   BIT(0)
#define HIF_PKT_FLAGS_COPY_HOST_TXD_ALL	   BIT(1)

#define MAX_BUF_NUM_PER_PKT		   6

typedef struct _HW_MAC_TX_DESC_APPEND_T {
	u16 u2PktFlags;
	u16 u2MsduToken;
	u8 ucBssIndex;
	u8 aucReserved[2];
	u8 ucBufNum;

	u32 au4BufPtr[MAX_BUF_NUM_PER_PKT];
	u16 au2BufLen[MAX_BUF_NUM_PER_PKT];
	u8 aucPktContent[0];
} HW_MAC_TX_DESC_APPEND_T, *P_HW_MAC_TX_DESC_APPEND_T;

/*!A data structure which is identical with HW MAC TX DMA Descriptor */
typedef struct _HW_MAC_TX_DESC_T {
	/* DW 0 */
	u16 u2TxByteCount;
	u8 ucEtherOffset; /* Ether-Type Offset,  IP checksum offload */
	u8 ucPortIdx_QueueIdx; /* UDP/TCP checksum offload,  USB
	                        * NextVLD/TxBURST, Queue index, Port index */
	/* DW 1 */
	u8 ucWlanIdx;
	u8 ucHeaderFormat; /* Header format, TX descriptor format */
	u8 ucHeaderPadding; /* Header padding, no ACK, TID, Protect frame */
	u8 ucOwnMAC;

	/* Long Format, the following structure is for long format ONLY */
	/* DW 2 */
	u8 ucType_SubType; /* Type, Sub-type, NDP, NDPA */
	u8 ucFrag; /* Sounding, force RTS/CTS, BMC, BIP, Duration, HTC exist,
	            * Fragment */
	u8 ucRemainingMaxTxTime;
	u8 ucPowerOffset; /* Power offset, Disable BA, Timing measurement, Fixed
	                   * rate */
	/* DW 3 */
	u16 u2TxCountLimit; /* TX count limit */
	u16 u2SN; /* SN, HW own, PN valid, SN valid */
	/* DW 4 */
	u32 u4PN1;
	/* DW 5 */
	u8 ucPID;
	u8 ucTxStatus; /* TXS format, TXS to mcu, TXS to host, DA source, BAR
	                * SSN, Power management */
	u16 u2PN2;
	/* DW 6 */
	u16 u2AntID; /* Fixed rate, Antenna ID */
	u16 u2FixedRate; /* Explicit/implicit beamforming, Fixed rate table,
	                  * LDPC, GI */
	/* DW 7 */
	u16 u2SwTxTime; /* Sw Tx time[9:0], SPE_IDX[15:11] */
	u16 u2PseFid; /* indicate frame ID in PSE for this TXD */
} HW_MAC_TX_DESC_T, *P_HW_MAC_TX_DESC_T, **PP_HW_MAC_TX_DESC_T;

typedef struct _TX_RESOURCE_CONTROL_T {
	/* HW TX queue definition */
	u8 ucDestPortIndex;
	u8 ucDestQueueIndex;
	/* HIF Interrupt status index */
	u8 ucHifTxQIndex;
} TX_RESOURCE_CONTROL_T, *PTX_RESOURCE_CONTROL_T;

typedef struct _TX_TC_TRAFFIC_SETTING_T {
	u32 u4TxDescLength;
	u32 u4RemainingTxTime;
	u8 ucTxCountLimit;
} TX_TC_TRAFFIC_SETTING_T, P_TX_TC_TRAFFIC_SETTING_T;

typedef void (*PFN_TX_DATA_DONE_CB)(IN P_GLUE_INFO_T prGlueInfo,
				    IN P_QUE_T prQue);

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */

extern PFN_TX_DATA_DONE_CB g_pfTxDataDoneCb;

/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */

#define TX_INC_CNT(prTxCtrl, eCounter)				    \
	{							    \
		((P_TX_CTRL_T)prTxCtrl)->au4Statistics[eCounter]++; \
	}

#define TX_ADD_CNT(prTxCtrl, eCounter, u8Amount)		    \
	{							    \
		((P_TX_CTRL_T)prTxCtrl)->au4Statistics[eCounter] += \
			(u32)u8Amount;				    \
	}

#define TX_GET_CNT(prTxCtrl, eCounter) \
	(((P_TX_CTRL_T)prTxCtrl)->au4Statistics[eCounter])

#define TX_RESET_ALL_CNTS(prTxCtrl)			     \
	{						     \
		kalMemZero(&prTxCtrl->au4Statistics[0],	     \
			   sizeof(prTxCtrl->au4Statistics)); \
	}
#if CFG_ENABLE_PKT_LIFETIME_PROFILE

#if CFG_PRINT_PKT_LIFETIME_PROFILE
#define PRINT_PKT_PROFILE(_pkt_profile, _note)					  \
	do {									  \
		if (!(_pkt_profile)->fgIsPrinted) {				  \
			DBGLOG(TX,						  \
			       TRACE,						  \
			       "X[%lu] E[%lu] D[%lu] HD[%lu] B[%d] RTP[%d] %s\n", \
			       (u32)((_pkt_profile)->				  \
				     rHardXmitArrivalTimestamp),		  \
			       (u32)((_pkt_profile)->rEnqueueTimestamp),	  \
			       (u32)((_pkt_profile)->rDequeueTimestamp),	  \
			       (u32)((_pkt_profile)->rHifTxDoneTimestamp),	  \
			       (u8)((_pkt_profile)->ucTcxFreeCount),		  \
			       (u16)((_pkt_profile)->u2RtpSn),			  \
			       (_note)));					  \
			(_pkt_profile)->fgIsPrinted = true;			  \
		}								  \
	} while (0)
#else
#define PRINT_PKT_PROFILE(_pkt_profile, _note)
#endif

#define CHK_PROFILES_DELTA(_pkt1, _pkt2, _delta)			    \
	(CHECK_FOR_TIMEOUT((_pkt1)->rHardXmitArrivalTimestamp,		    \
			   (_pkt2)->rHardXmitArrivalTimestamp, (_delta)) || \
	 CHECK_FOR_TIMEOUT((_pkt1)->rEnqueueTimestamp,			    \
			   (_pkt2)->rEnqueueTimestamp, (_delta)) ||	    \
	 CHECK_FOR_TIMEOUT((_pkt1)->rDequeueTimestamp,			    \
			   (_pkt2)->rDequeueTimestamp, (_delta)) ||	    \
	 CHECK_FOR_TIMEOUT((_pkt1)->rHifTxDoneTimestamp,		    \
			   (_pkt2)->rHifTxDoneTimestamp, (_delta)))

#define CHK_PROFILE_DELTA(_pkt, _delta)					   \
	(CHECK_FOR_TIMEOUT((_pkt)->rEnqueueTimestamp,			   \
			   (_pkt)->rHardXmitArrivalTimestamp, (_delta)) || \
	 CHECK_FOR_TIMEOUT((_pkt)->rDequeueTimestamp,			   \
			   (_pkt)->rEnqueueTimestamp, (_delta)) ||	   \
	 CHECK_FOR_TIMEOUT((_pkt)->rHifTxDoneTimestamp,			   \
			   (_pkt)->rDequeueTimestamp, (_delta)))
#endif

/*------------------------------------------------------------------------------
 * MACRO for MSDU_INFO
 *------------------------------------------------------------------------------
 */
#define TX_SET_MMPDU	      nicTxSetMngPacket
#define TX_SET_DATA_PACKET    nicTxSetDataPacket

/*------------------------------------------------------------------------------
 * MACRO for HW_MAC_TX_DESC_T
 *------------------------------------------------------------------------------
 */
#define TX_DESC_GET_FIELD(_rHwMacTxDescField, _mask, _offset) \
	(((_rHwMacTxDescField) & (_mask)) >> (_offset))
#define TX_DESC_SET_FIELD(_rHwMacTxDescField, _value, _mask, _offset)	     \
	{								     \
		(_rHwMacTxDescField) &= ~(_mask);			     \
		(_rHwMacTxDescField) |= (((_value) << (_offset)) & (_mask)); \
	}

#define HAL_MAC_TX_DESC_SET_DW(_prHwMacTxDesc, _ucOffsetInDw, _ucLengthInDw, \
			       _pucValueAddr)				     \
	kalMemCopy((u32 *)(_prHwMacTxDesc) + (_ucOffsetInDw),		     \
		   (u8 *)(_pucValueAddr), DWORD_TO_BYTE(_ucLengthInDw))
#define HAL_MAC_TX_DESC_GET_DW(_prHwMacTxDesc, _ucOffsetInDw, _ucLengthInDw, \
			       _pucValueAddr)				     \
	kalMemCopy((u8 *)(_pucValueAddr),				     \
		   (u32 *)(_prHwMacTxDesc) + (_ucOffsetInDw),		     \
		   DWORD_TO_BYTE(_ucLengthInDw))

/* DW 0 */
#define HAL_MAC_TX_DESC_GET_TX_BYTE_COUNT(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxByteCount)
#define HAL_MAC_TX_DESC_SET_TX_BYTE_COUNT(_prHwMacTxDesc, _u2TxByteCount) \
	(((_prHwMacTxDesc)->u2TxByteCount) = ((u16)_u2TxByteCount))

#define HAL_MAC_TX_DESC_GET_ETHER_TYPE_OFFSET(_prHwMacTxDesc) \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucEtherOffset,    \
			  TX_DESC_ETHER_TYPE_OFFSET_MASK,     \
			  TX_DESC_ETHER_TYPE_OFFSET_OFFSET)
#define HAL_MAC_TX_DESC_SET_ETHER_TYPE_OFFSET(_prHwMacTxDesc,	  \
					      _ucEtherTypeOffset) \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucEtherOffset),	  \
			  ((u8)_ucEtherTypeOffset),		  \
			  TX_DESC_ETHER_TYPE_OFFSET_MASK,	  \
			  TX_DESC_ETHER_TYPE_OFFSET_OFFSET)

#define HAL_MAC_TX_DESC_IS_IP_CHKSUM_ENABLED(_prHwMacTxDesc)		 \
	(((_prHwMacTxDesc)->ucEtherOffset & TX_DESC_IP_CHKSUM_OFFLOAD) ? \
	 false :							 \
	 true)
#define HAL_MAC_TX_DESC_SET_IP_CHKSUM(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucEtherOffset |= TX_DESC_IP_CHKSUM_OFFLOAD)
#define HAL_MAC_TX_DESC_UNSET_IP_CHKSUM(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucEtherOffset &= ~TX_DESC_IP_CHKSUM_OFFLOAD)

#define HAL_MAC_TX_DESC_IS_TCP_UDP_CHKSUM_ENABLED(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucPortIdx_QueueIdx &		  \
	  TX_DESC_TCP_UDP_CHKSUM_OFFLOAD) ?			  \
	 false :						  \
	 true)
#define HAL_MAC_TX_DESC_SET_TCP_UDP_CHKSUM(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPortIdx_QueueIdx |= TX_DESC_TCP_UDP_CHKSUM_OFFLOAD)
#define HAL_MAC_TX_DESC_UNSET_TCP_UDP_CHKSUM(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPortIdx_QueueIdx &=	     \
		 ~TX_DESC_TCP_UDP_CHKSUM_OFFLOAD)

#define HAL_MAC_TX_DESC_IS_USB_NEXT_VLD_ENABLED(_prHwMacTxDesc)		 \
	(((_prHwMacTxDesc)->ucPortIdx_QueueIdx & TX_DESC_USB_NEXT_VLD) ? \
	 true :								 \
	 false)
#define HAL_MAC_TX_DESC_SET_USB_NEXT_VLD(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPortIdx_QueueIdx |= TX_DESC_USB_NEXT_VLD)
#define HAL_MAC_TX_DESC_UNSET_USB_NEXT_VLD(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPortIdx_QueueIdx &= ~TX_DESC_USB_NEXT_VLD)

#define HAL_MAC_TX_DESC_GET_QUEUE_INDEX(_prHwMacTxDesc)		\
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucPortIdx_QueueIdx,	\
			  TX_DESC_QUEUE_INDEX_MASK,		\
			  TX_DESC_QUEUE_INDEX_OFFSET)
#define HAL_MAC_TX_DESC_SET_QUEUE_INDEX(_prHwMacTxDesc, _ucQueueIndex)	 \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucPortIdx_QueueIdx),	 \
			  ((u8)_ucQueueIndex), TX_DESC_QUEUE_INDEX_MASK, \
			  TX_DESC_QUEUE_INDEX_OFFSET)

#define HAL_MAC_TX_DESC_GET_PORT_INDEX(_prHwMacTxDesc)		\
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucPortIdx_QueueIdx,	\
			  TX_DESC_PORT_INDEX, TX_DESC_PORT_INDEX_OFFSET)
#define HAL_MAC_TX_DESC_SET_PORT_INDEX(_prHwMacTxDesc, _ucPortIndex) \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucPortIdx_QueueIdx),    \
			  ((u8)_ucPortIndex), TX_DESC_PORT_INDEX,    \
			  TX_DESC_PORT_INDEX_OFFSET)

/* DW 1 */
#define HAL_MAC_TX_DESC_GET_WLAN_INDEX(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucWlanIdx)
#define HAL_MAC_TX_DESC_SET_WLAN_INDEX(_prHwMacTxDesc, _ucWlanIdx) \
	(((_prHwMacTxDesc)->ucWlanIdx) = (_ucWlanIdx))

#define HAL_MAC_TX_DESC_IS_LONG_FORMAT(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_FORMAT) ? true : false)
#define HAL_MAC_TX_DESC_SET_LONG_FORMAT(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_FORMAT)
#define HAL_MAC_TX_DESC_SET_SHORT_FORMAT(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_FORMAT)

#define HAL_MAC_TX_DESC_GET_HEADER_FORMAT(_prHwMacTxDesc)   \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderFormat, \
			  TX_DESC_HEADER_FORMAT_MASK,	    \
			  TX_DESC_HEADER_FORMAT_OFFSET)
#define HAL_MAC_TX_DESC_SET_HEADER_FORMAT(_prHwMacTxDesc, _ucHdrFormat)	  \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderFormat),		  \
			  ((u8)_ucHdrFormat), TX_DESC_HEADER_FORMAT_MASK, \
			  TX_DESC_HEADER_FORMAT_OFFSET)

/* HF = 0x00, 802.11 normal mode */
#define HAL_MAC_TX_DESC_IS_MORE_DATA(_prHwMacTxDesc)			     \
	(((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_MORE_DATA) ? \
	 true :								     \
	 false)
#define HAL_MAC_TX_DESC_SET_MORE_DATA(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_MORE_DATA)
#define HAL_MAC_TX_DESC_UNSET_MORE_DATA(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_MORE_DATA)

#define HAL_MAC_TX_DESC_IS_REMOVE_VLAN(_prHwMacTxDesc)			       \
	(((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_REMOVE_VLAN) ? \
	 true :								       \
	 false)
#define HAL_MAC_TX_DESC_SET_REMOVE_VLAN(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_REMOVE_VLAN)
#define HAL_MAC_TX_DESC_UNSET_REMOVE_VLAN(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_REMOVE_VLAN)

#define HAL_MAC_TX_DESC_IS_VLAN(_prHwMacTxDesc)				      \
	(((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_VLAN_FIELD) ? \
	 true :								      \
	 false)
#define HAL_MAC_TX_DESC_SET_VLAN(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_VLAN_FIELD)
#define HAL_MAC_TX_DESC_UNSET_VLAN(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_VLAN_FIELD)

#define HAL_MAC_TX_DESC_IS_ETHERNET_II(_prHwMacTxDesc)			       \
	(((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_ETHERNET_II) ? \
	 true :								       \
	 false)
#define HAL_MAC_TX_DESC_SET_ETHERNET_II(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_ETHERNET_II)
#define HAL_MAC_TX_DESC_UNSET_ETHERNET_II(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_ETHERNET_II)

/* HF = 0x00/0x11, 802.11 normal/enhancement mode */
#define HAL_MAC_TX_DESC_IS_EOSP(_prHwMacTxDesc)				       \
	(((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_EOSP) ? true : \
	 false)
#define HAL_MAC_TX_DESC_SET_EOSP(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_EOSP)
#define HAL_MAC_TX_DESC_UNSET_EOSP(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_EOSP)

/* HF = 0x11, 802.11 enhancement mode */
#define HAL_MAC_TX_DESC_IS_AMSDU(_prHwMacTxDesc)			 \
	(((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_ENH_802_11_AMSDU) ? \
	 true :								 \
	 false)
#define HAL_MAC_TX_DESC_SET_AMSDU(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_ENH_802_11_AMSDU)
#define HAL_MAC_TX_DESC_UNSET_AMSDU(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_ENH_802_11_AMSDU)

/* HF = 0x10, non-802.11 */
#define HAL_MAC_TX_DESC_GET_802_11_HEADER_LENGTH(_prHwMacTxDesc) \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderFormat,	 \
			  TX_DESC_NOR_802_11_HEADER_LENGTH_MASK, \
			  TX_DESC_NOR_802_11_HEADER_LENGTH_OFFSET)
#define HAL_MAC_TX_DESC_SET_802_11_HEADER_LENGTH(_prHwMacTxDesc, _ucHdrLength) \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderFormat),		       \
			  ((u8)_ucHdrLength),				       \
			  TX_DESC_NOR_802_11_HEADER_LENGTH_MASK,	       \
			  TX_DESC_NOR_802_11_HEADER_LENGTH_OFFSET)

#define HAL_MAC_TX_DESC_GET_TXD_LENGTH(_prHwMacTxDesc)	     \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderPadding, \
			  TX_DESC_TXD_LENGTH_MASK, TX_DESC_TXD_LENGTH_OFFSET)
#define HAL_MAC_TX_DESC_SET_TXD_LENGTH(_prHwMacTxDesc, _ucHdrPadding)	\
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderPadding),		\
			  ((u8)_ucHdrPadding), TX_DESC_TXD_LENGTH_MASK,	\
			  TX_DESC_TXD_LENGTH_OFFSET)

#define HAL_MAC_TX_DESC_GET_TXD_EXTEND_LENGTH(_prHwMacTxDesc) \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderPadding,  \
			  TX_DESC_TXD_EXTEND_LENGTH_MASK,     \
			  TX_DESC_TXD_EXTEND_LENGTH_OFFSET)
#define HAL_MAC_TX_DESC_SET_TXD_EXTEND_LENGTH(_prHwMacTxDesc, _ucHdrPadding)   \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderPadding),		       \
			  ((u8)_ucHdrPadding), TX_DESC_TXD_EXTEND_LENGTH_MASK, \
			  TX_DESC_TXD_EXTEND_LENGTH_OFFSET)

#define HAL_MAC_TX_DESC_GET_HEADER_PADDING(_prHwMacTxDesc)    \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderPadding,  \
			  TX_DESC_HEADER_PADDING_LENGTH_MASK, \
			  TX_DESC_HEADER_PADDING_LENGTH_OFFSET)
#define HAL_MAC_TX_DESC_SET_HEADER_PADDING(_prHwMacTxDesc, _ucHdrPadding) \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderPadding),		  \
			  ((u8)_ucHdrPadding),				  \
			  TX_DESC_HEADER_PADDING_LENGTH_MASK,		  \
			  TX_DESC_HEADER_PADDING_LENGTH_OFFSET)

#define HAL_MAC_TX_DESC_GET_UTXB_AMSDU(_prHwMacTxDesc)	     \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderPadding, \
			  TX_DESC_TXD_UTXB_AMSDU_MASK,	     \
			  TX_DESC_TXD_UTXB_AMSDU_OFFSET)
#define HAL_MAC_TX_DESC_SET_UTXB_AMSDU(_prHwMacTxDesc, _ucHdrPadding)	    \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderPadding),		    \
			  ((u8)_ucHdrPadding), TX_DESC_TXD_UTXB_AMSDU_MASK, \
			  TX_DESC_TXD_UTXB_AMSDU_OFFSET)

#define HAL_MAC_TX_DESC_IS_HEADER_PADDING_IN_THE_HEAD(_prHwMacTxDesc)	     \
	(((_prHwMacTxDesc)->ucHeaderPadding & TX_DESC_HEADER_PADDING_MODE) ? \
	 true :								     \
	 false)
#define HAL_MAC_TX_DESC_SET_HEADER_PADDING_IN_THE_HEAD(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderPadding |= TX_DESC_HEADER_PADDING_MODE)
#define HAL_MAC_TX_DESC_SET_HEADER_PADDING_IN_THE_TAIL(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucHeaderPadding &= ~TX_DESC_HEADER_PADDING_MODE)

#define HAL_MAC_TX_DESC_GET_TID(_prHwMacTxDesc)				       \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderPadding, TX_DESC_TID_MASK, \
			  TX_DESC_TID_OFFSET)
#define HAL_MAC_TX_DESC_SET_TID(_prHwMacTxDesc, _ucTID)			     \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderPadding), ((u8)_ucTID), \
			  TX_DESC_TID_MASK, TX_DESC_TID_OFFSET)

#define HAL_MAC_TX_DESC_GET_PKT_FORMAT(_prHwMacTxDesc) \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucOwnMAC,  \
			  TX_DESC_PACKET_FORMAT_MASK,  \
			  TX_DESC_PACKET_FORMAT_OFFSET)
#define HAL_MAC_TX_DESC_SET_PKT_FORMAT(_prHwMacTxDesc, _ucPktFormat)	    \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucOwnMAC), ((u8)_ucPktFormat), \
			  TX_DESC_PACKET_FORMAT_MASK,			    \
			  TX_DESC_PACKET_FORMAT_OFFSET)

#define HAL_MAC_TX_DESC_GET_OWN_MAC_INDEX(_prHwMacTxDesc)		    \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucOwnMAC, TX_DESC_OWN_MAC_MASK, \
			  TX_DESC_OWN_MAC_OFFSET)
#define HAL_MAC_TX_DESC_SET_OWN_MAC_INDEX(_prHwMacTxDesc, _ucOwnMacIdx)	    \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucOwnMAC), ((u8)_ucOwnMacIdx), \
			  TX_DESC_OWN_MAC_MASK, TX_DESC_OWN_MAC_OFFSET)

/* DW 2 */
#define HAL_MAC_TX_DESC_GET_SUB_TYPE(_prHwMacTxDesc)	    \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucType_SubType, \
			  TX_DESC_SUB_TYPE_MASK, TX_DESC_SUB_TYPE_OFFSET)
#define HAL_MAC_TX_DESC_SET_SUB_TYPE(_prHwMacTxDesc, _ucSubType)   \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucType_SubType),	   \
			  ((u8)_ucSubType), TX_DESC_SUB_TYPE_MASK, \
			  TX_DESC_SUB_TYPE_OFFSET)

#define HAL_MAC_TX_DESC_GET_TYPE(_prHwMacTxDesc)			       \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucType_SubType, TX_DESC_TYPE_MASK, \
			  TX_DESC_TYPE_OFFSET)
#define HAL_MAC_TX_DESC_SET_TYPE(_prHwMacTxDesc, _ucType)		     \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucType_SubType), ((u8)_ucType), \
			  TX_DESC_TYPE_MASK, TX_DESC_TYPE_OFFSET)

#define HAL_MAC_TX_DESC_IS_NDP(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucType_SubType & TX_DESC_NDP) ? true : false)
#define HAL_MAC_TX_DESC_SET_NDP(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucType_SubType |= TX_DESC_NDP)
#define HAL_MAC_TX_DESC_UNSET_NDP(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucType_SubType &= ~TX_DESC_NDP)

#define HAL_MAC_TX_DESC_IS_NDPA(_prHwMacTxDesc)	\
	(((_prHwMacTxDesc)->ucType_SubType & TX_DESC_NDPA) ? true : false)
#define HAL_MAC_TX_DESC_SET_NDPA(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucType_SubType |= TX_DESC_NDPA)
#define HAL_MAC_TX_DESC_UNSET_NDPA(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucType_SubType &= ~TX_DESC_NDPA)

#define HAL_MAC_TX_DESC_IS_SOUNDING_FRAME(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucFrag & TX_DESC_SOUNDING) ? true : false)
#define HAL_MAC_TX_DESC_SET_SOUNDING_FRAME(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag |= TX_DESC_SOUNDING)
#define HAL_MAC_TX_DESC_UNSET_SOUNDING_FRAME(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_SOUNDING)

#define HAL_MAC_TX_DESC_IS_FORCE_RTS_CTS_EN(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucFrag & TX_DESC_FORCE_RTS_CTS) ? true : false)
#define HAL_MAC_TX_DESC_SET_FORCE_RTS_CTS(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag |= TX_DESC_FORCE_RTS_CTS)
#define HAL_MAC_TX_DESC_UNSET_FORCE_RTS_CTS(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_FORCE_RTS_CTS)

#define HAL_MAC_TX_DESC_IS_BMC(_prHwMacTxDesc)				   \
	(((_prHwMacTxDesc)->ucFrag & TX_DESC_BROADCAST_MULTICAST) ? true : \
	 false)
#define HAL_MAC_TX_DESC_SET_BMC(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucFrag |= TX_DESC_BROADCAST_MULTICAST)
#define HAL_MAC_TX_DESC_UNSET_BMC(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_BROADCAST_MULTICAST)

#define HAL_MAC_TX_DESC_IS_BIP(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucFrag & TX_DESC_BIP_PROTECTED) ? true : false)
#define HAL_MAC_TX_DESC_SET_BIP(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucFrag |= TX_DESC_BIP_PROTECTED)
#define HAL_MAC_TX_DESC_UNSET_BIP(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_BIP_PROTECTED)

#define HAL_MAC_TX_DESC_IS_DURATION_CONTROL_BY_SW(_prHwMacTxDesc)	      \
	(((_prHwMacTxDesc)->ucFrag & TX_DESC_DURATION_FIELD_CONTROL) ? true : \
	 false)
#define HAL_MAC_TX_DESC_SET_DURATION_CONTROL_BY_SW(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag |= TX_DESC_DURATION_FIELD_CONTROL)
#define HAL_MAC_TX_DESC_SET_DURATION_CONTROL_BY_HW(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_DURATION_FIELD_CONTROL)

#define HAL_MAC_TX_DESC_IS_HTC_EXIST(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucFrag & TX_DESC_HTC_EXISTS) ? true : false)
#define HAL_MAC_TX_DESC_SET_HTC_EXIST(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucFrag |= TX_DESC_HTC_EXISTS)
#define HAL_MAC_TX_DESC_UNSET_HTC_EXIST(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_HTC_EXISTS)

#define HAL_MAC_TX_DESC_IS_FRAG_PACKET(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucFrag & TX_DESC_FRAGMENT_MASK) ? true : false)
#define HAL_MAC_TX_DESC_GET_FRAG_PACKET_POS(_prHwMacTxDesc)		   \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucFrag, TX_DESC_FRAGMENT_MASK, \
			  TX_DESC_FRAGMENT_OFFSET)
#define HAL_MAC_TX_DESC_SET_FRAG_PACKET_POS(_prHwMacTxDesc, _ucFragPos)	\
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucFrag), ((u8)_ucFragPos),	\
			  TX_DESC_FRAGMENT_MASK, TX_DESC_FRAGMENT_OFFSET)

/* For driver */
/* in unit of 32TU */
#define HAL_MAC_TX_DESC_GET_REMAINING_LIFE_TIME(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucRemainingMaxTxTime)
#define HAL_MAC_TX_DESC_SET_REMAINING_LIFE_TIME(_prHwMacTxDesc, _ucLifeTime) \
	((_prHwMacTxDesc)->ucRemainingMaxTxTime = (_ucLifeTime))
/* in unit of ms (minimal value is about 40ms) */
#define HAL_MAC_TX_DESC_GET_REMAINING_LIFE_TIME_IN_MS(_prHwMacTxDesc)	    \
	(TU_TO_MSEC(HAL_MAC_TX_DESC_GET_REMAINING_LIFE_TIME(_prHwMacTxDesc) \
		    << TX_DESC_LIFE_TIME_UNIT_IN_POWER_OF_2))
#define HAL_MAC_TX_DESC_SET_REMAINING_LIFE_TIME_IN_MS(_prHwMacTxDesc,	       \
						      _u4LifeTimeMs)	       \
	do {								       \
		u32 u4LifeTimeInUnit =					       \
			((MSEC_TO_USEC(_u4LifeTimeMs) / USEC_PER_TU) >>	       \
			 TX_DESC_LIFE_TIME_UNIT_IN_POWER_OF_2);		       \
		if (u4LifeTimeInUnit >= BIT(8))				       \
		u4LifeTimeInUnit = BITS(0, 7);				       \
		else if ((_u4LifeTimeMs != TX_DESC_TX_TIME_NO_LIMIT) &&	       \
			 (u4LifeTimeInUnit == TX_DESC_TX_TIME_NO_LIMIT))       \
		u4LifeTimeInUnit = 1;					       \
		HAL_MAC_TX_DESC_SET_REMAINING_LIFE_TIME(_prHwMacTxDesc,	       \
							(u8)u4LifeTimeInUnit); \
	} while (0)

#define HAL_MAC_TX_DESC_GET_POWER_OFFSET(_prHwMacTxDesc)   \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucPowerOffset, \
			  TX_DESC_POWER_OFFSET_MASK, 0)
#define HAL_MAC_TX_DESC_SET_POWER_OFFSET(_prHwMacTxDesc, _ucPowerOffset) \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucPowerOffset),		 \
			  ((u8)_ucPowerOffset), TX_DESC_POWER_OFFSET_MASK, 0)

#define HAL_MAC_TX_DESC_IS_BA_DISABLE(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucPowerOffset & TX_DESC_BA_DISABLE) ? true : false)
#define HAL_MAC_TX_DESC_SET_BA_DISABLE(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_BA_DISABLE)
#define HAL_MAC_TX_DESC_SET_BA_ENABLE(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_BA_DISABLE)

#define HAL_MAC_TX_DESC_IS_TIMING_MEASUREMENT(_prHwMacTxDesc)		  \
	(((_prHwMacTxDesc)->ucPowerOffset & TX_DESC_TIMING_MEASUREMENT) ? \
	 true :								  \
	 false)
#define HAL_MAC_TX_DESC_SET_TIMING_MEASUREMENT(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_TIMING_MEASUREMENT)
#define HAL_MAC_TX_DESC_UNSET_TIMING_MEASUREMENT(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_TIMING_MEASUREMENT)

#define HAL_MAC_TX_DESC_IS_FIXED_RATE_ENABLE(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucPowerOffset & TX_DESC_FIXED_RATE) ? true : false)
#define HAL_MAC_TX_DESC_SET_FIXED_RATE_ENABLE(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_FIXED_RATE)
#define HAL_MAC_TX_DESC_SET_FIXED_RATE_DISABLE(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_FIXED_RATE)

/* DW 3 */
#define HAL_MAC_TX_DESC_IS_NO_ACK(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->u2TxCountLimit & TX_DESC_NO_ACK) ? true : false)
#define HAL_MAC_TX_DESC_SET_NO_ACK(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxCountLimit |= TX_DESC_NO_ACK)
#define HAL_MAC_TX_DESC_UNSET_NO_ACK(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxCountLimit &= ~TX_DESC_NO_ACK)

#define HAL_MAC_TX_DESC_IS_PROTECTION(_prHwMacTxDesc)			       \
	(((_prHwMacTxDesc)->u2TxCountLimit & TX_DESC_PROTECTED_FRAME) ? true : \
	 false)
#define HAL_MAC_TX_DESC_SET_PROTECTION(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxCountLimit |= TX_DESC_PROTECTED_FRAME)
#define HAL_MAC_TX_DESC_UNSET_PROTECTION(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxCountLimit &= ~TX_DESC_PROTECTED_FRAME)

#define HAL_MAC_TX_DESC_IS_EXTEND_MORE_DATA(_prHwMacTxDesc)		 \
	(((_prHwMacTxDesc)->u2TxCountLimit & TX_DESC_EXTEND_MORE_DATA) ? \
	 true :								 \
	 false)
#define HAL_MAC_TX_DESC_SET_EXTEND_MORE_DATA(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxCountLimit |= TX_DESC_EXTEND_MORE_DATA)
#define HAL_MAC_TX_DESC_UNSET_EXTEND_MORE_DATA(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxCountLimit &= ~TX_DESC_EXTEND_MORE_DATA)

#define HAL_MAC_TX_DESC_IS_EXTEND_EOSP(_prHwMacTxDesc)			   \
	(((_prHwMacTxDesc)->u2TxCountLimit & TX_DESC_EXTEND_EOSP) ? true : \
	 false)
#define HAL_MAC_TX_DESC_SET_EXTEND_EOSP(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->u2TxCountLimit |= TX_DESC_EXTEND_EOSP)
#define HAL_MAC_TX_DESC_UNSET_EXTEND_EOSP(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2TxCountLimit &= ~TX_DESC_EXTEND_EOSP)

#define HAL_MAC_TX_DESC_GET_SW_RESERVED(_prHwMacTxDesc)	    \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->u2TxCountLimit, \
			  TX_DESC_SW_RESERVED_MASK,	    \
			  TX_DESC_SW_RESERVED_OFFSET)
#define HAL_MAC_TX_DESC_SET_SW_RESERVED(_prHwMacTxDesc, _ucSwReserved)	 \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2TxCountLimit),		 \
			  ((u8)_ucSwReserved), TX_DESC_SW_RESERVED_MASK, \
			  TX_DESC_SW_RESERVED_OFFSET)
#define HAL_MAC_TX_DESC_GET_TX_COUNT(_prHwMacTxDesc)	    \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->u2TxCountLimit, \
			  TX_DESC_TX_COUNT_MASK, TX_DESC_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_SET_TX_COUNT(_prHwMacTxDesc, _ucTxCountLimit)	\
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2TxCountLimit),		\
			  ((u8)_ucTxCountLimit), TX_DESC_TX_COUNT_MASK,	\
			  TX_DESC_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_GET_REMAINING_TX_COUNT(_prHwMacTxDesc) \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->u2TxCountLimit,    \
			  TX_DESC_REMAINING_TX_COUNT_MASK,     \
			  TX_DESC_REMAINING_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_SET_REMAINING_TX_COUNT(_prHwMacTxDesc,	\
					       _ucTxCountLimit)	\
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2TxCountLimit),	\
			  ((u8)_ucTxCountLimit),		\
			  TX_DESC_REMAINING_TX_COUNT_MASK,	\
			  TX_DESC_REMAINING_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_GET_SEQUENCE_NUMBER(_prHwMacTxDesc) \
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->u2SN, TX_DESC_SEQUENCE_NUMBER, 0)
#define HAL_MAC_TX_DESC_SET_SEQUENCE_NUMBER(_prHwMacTxDesc, _u2SN) \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2SN), ((u16)_u2SN),  \
			  TX_DESC_SEQUENCE_NUMBER, 0)
#define HAL_MAC_TX_DESC_SET_HW_RESERVED(_prHwMacTxDesc, _ucHwReserved)	 \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2SN), ((u8)_ucHwReserved), \
			  TX_DESC_HW_RESERVED_MASK,			 \
			  TX_DESC_HW_RESERVED_OFFSET)
#define HAL_MAC_TX_DESC_IS_TXD_SN_VALID(_prHwMacTxDesc)	\
	(((_prHwMacTxDesc)->u2SN & TX_DESC_SN_IS_VALID) ? true : false)
#define HAL_MAC_TX_DESC_SET_TXD_SN_VALID(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2SN |= TX_DESC_SN_IS_VALID)
#define HAL_MAC_TX_DESC_SET_TXD_SN_INVALID(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2SN &= ~TX_DESC_SN_IS_VALID)

#define HAL_MAC_TX_DESC_IS_TXD_PN_VALID(_prHwMacTxDesc)	\
	(((_prHwMacTxDesc)->u2SN & TX_DESC_PN_IS_VALID) ? true : false)
#define HAL_MAC_TX_DESC_SET_TXD_PN_VALID(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2SN |= TX_DESC_PN_IS_VALID)
#define HAL_MAC_TX_DESC_SET_TXD_PN_INVALID(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2SN &= ~TX_DESC_PN_IS_VALID)

#define HAL_MAC_TX_DESC_ASSIGN_SN_BY_SW(_prHwMacTxDesc, _u2SN)		    \
	{								    \
		HAL_MAC_TX_DESC_SET_SEQUENCE_NUMBER(_prHwMacTxDesc, _u2SN); \
		HAL_MAC_TX_DESC_SET_TXD_SN_VALID(_prHwMacTxDesc);	    \
	}
#define HAL_MAC_TX_DESC_ASSIGN_SN_BY_HW(_prHwMacTxDesc)			\
	{								\
		HAL_MAC_TX_DESC_SET_SEQUENCE_NUMBER(_prHwMacTxDesc, 0);	\
		HAL_MAC_TX_DESC_SET_TXD_SN_INVALID(_prHwMacTxDesc);	\
	}

/* DW 4 */
#define HAL_MAC_TX_DESC_GET_PN(_prHwMacTxDesc, _u4PN_0_31, _u2PN_32_47)	\
	{								\
		((u32)_u4PN_0_31) = (_prHwMacTxDesc)->u4PN1;		\
		((u16)_u2PN_32_47) = (_prHwMacTxDesc)->u2PN2;		\
	}
#define HAL_MAC_TX_DESC_SET_PN(_prHwMacTxDesc, _u4PN_0_31, _u2PN_32_47)	\
	{								\
		(_prHwMacTxDesc)->u4PN1 = ((u32)_u4PN_0_31);		\
		(_prHwMacTxDesc)->u2PN2 = ((u16)_u2PN_32_47);		\
	}

#define HAL_MAC_TX_DESC_ASSIGN_PN_BY_SW(_prHwMacTxDesc, _u4PN_0_31, \
					_u2PN_32_47)		    \
	{							    \
		HAL_MAC_TX_DESC_SET_PN(_prHwMacTxDesc, _u4PN_0_31,  \
				       _u2PN_32_47);		    \
		HAL_MAC_TX_DESC_SET_TXD_PN_VALID(_prHwMacTxDesc);   \
	}
#define HAL_MAC_TX_DESC_ASSIGN_PSN_BY_HW(_prHwMacTxDesc)	    \
	{							    \
		HAL_MAC_TX_DESC_SET_PN(_prHwMacTxDesc, 0, 0);	    \
		HAL_MAC_TX_DESC_SET_TXD_PN_INVALID(_prHwMacTxDesc); \
	}

/* DW 5 */
#define HAL_MAC_TX_DESC_GET_PID(_prHwMacTxDesc)    ((_prHwMacTxDesc)->ucPID)
#define HAL_MAC_TX_DESC_SET_PID(_prHwMacTxDesc, _ucPID)	\
	(((_prHwMacTxDesc)->ucPID) = (_ucPID))

#define HAL_MAC_TX_DESC_GET_TXS_FORMAT(_prHwMacTxDesc)	\
	TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucTxStatus,	\
			  TX_DESC_TX_STATUS_FORMAT,	\
			  TX_DESC_TX_STATUS_FORMAT_OFFSET)
#define HAL_MAC_TX_DESC_SET_TXS_FORMAT(_prHwMacTxDesc, _ucTXSFormat)	      \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucTxStatus), ((u8)_ucTXSFormat), \
			  TX_DESC_TX_STATUS_FORMAT,			      \
			  TX_DESC_TX_STATUS_FORMAT_OFFSET)

#define HAL_MAC_TX_DESC_IS_TXS_TO_MCU(_prHwMacTxDesc)			    \
	(((_prHwMacTxDesc)->ucTxStatus & TX_DESC_TX_STATUS_TO_MCU) ? true : \
	 false)
#define HAL_MAC_TX_DESC_SET_TXS_TO_MCU(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucTxStatus |= TX_DESC_TX_STATUS_TO_MCU)
#define HAL_MAC_TX_DESC_UNSET_TXS_TO_MCU(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucTxStatus &= ~TX_DESC_TX_STATUS_TO_MCU)

#define HAL_MAC_TX_DESC_IS_TXS_TO_HOST(_prHwMacTxDesc)			     \
	(((_prHwMacTxDesc)->ucTxStatus & TX_DESC_TX_STATUS_TO_HOST) ? true : \
	 false)
#define HAL_MAC_TX_DESC_SET_TXS_TO_HOST(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->ucTxStatus |= TX_DESC_TX_STATUS_TO_HOST)
#define HAL_MAC_TX_DESC_UNSET_TXS_TO_HOST(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucTxStatus &= ~TX_DESC_TX_STATUS_TO_HOST)

#define HAL_MAC_TX_DESC_IS_DA_FROM_WTBL(_prHwMacTxDesc)	\
	(((_prHwMacTxDesc)->ucPowerOffset & TX_DESC_DA_SOURCE) ? true : false)
#define HAL_MAC_TX_DESC_SET_DA_FROM_WTBL(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_DA_SOURCE)
#define HAL_MAC_TX_DESC_SET_DA_FROM_MSDU(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_DA_SOURCE)

#define HAL_MAC_TX_DESC_IS_SW_PM_CONTROL(_prHwMacTxDesc) \
	(((_prHwMacTxDesc)->ucPowerOffset &		 \
	  TX_DESC_POWER_MANAGEMENT_CONTROL) ?		 \
	 true :						 \
	 false)
#define HAL_MAC_TX_DESC_SET_SW_PM_CONTROL(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_POWER_MANAGEMENT_CONTROL)
#define HAL_MAC_TX_DESC_SET_HW_PM_CONTROL(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_POWER_MANAGEMENT_CONTROL)

/* DW 6 */
#define HAL_MAC_TX_DESC_SET_FR_BW(_prHwMacTxDesc, ucBw)		   \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2AntID), ((u8)ucBw), \
			  TX_DESC_BANDWIDTH_MASK, TX_DESC_BANDWIDTH_OFFSET)

#define HAL_MAC_TX_DESC_SET_FR_DYNAMIC_BW_RTS(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2AntID |= TX_DESC_DYNAMIC_BANDWIDTH)

#define HAL_MAC_TX_DESC_SET_FR_ANTENNA_ID(_prHwMacTxDesc, _ucAntId)    \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2AntID), ((u8)_ucAntId), \
			  TX_DESC_ANTENNA_INDEX_MASK,		       \
			  TX_DESC_ANTENNA_INDEX_OFFSET)

#define HAL_MAC_TX_DESC_SET_FR_RATE(_prHwMacTxDesc, _u2RatetoFixed)	 \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2FixedRate),		 \
			  ((u8)_u2RatetoFixed), TX_DESC_FIXDE_RATE_MASK, \
			  TX_DESC_FIXDE_RATE_OFFSET)

#define HAL_MAC_TX_DESC_SET_FR_BF(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_BF)

#define HAL_MAC_TX_DESC_SET_FR_LDPC(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_LDPC)

#define HAL_MAC_TX_DESC_SET_FR_SHORT_GI(_prHwMacTxDesc)	\
	((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_GUARD_INTERVAL)

#define HAL_MAC_TX_DESC_SET_FR_NORMAL_GI(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2FixedRate &= ~TX_DESC_GUARD_INTERVAL)

#define HAL_MAC_TX_DESC_IS_CR_FIXED_RATE_MODE(_prHwMacTxDesc)		    \
	(((_prHwMacTxDesc)->u2FixedRate & TX_DESC_FIXED_RATE_MODE) ? true : \
	 false)

#define HAL_MAC_TX_DESC_SET_FIXED_RATE_MODE_TO_DESC(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2FixedRate &= ~TX_DESC_FIXED_RATE_MODE)
#define HAL_MAC_TX_DESC_SET_FIXED_RATE_MODE_TO_CR(_prHwMacTxDesc) \
	((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_FIXED_RATE_MODE)

/* DW 7 */
#define HAL_MAC_TX_DESC_SET_SPE_IDX(_prHwMacTxDesc, _ucSpeIdx)		    \
	TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2SwTxTime), ((u16)_ucSpeIdx), \
			  TX_DESC_SPE_EXT_IDX_MASK,			    \
			  TX_DESC_SPE_EXT_IDX_OFFSET)

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

void nicTxInitialize(IN P_ADAPTER_T prAdapter);

WLAN_STATUS nicTxAcquireResource(IN P_ADAPTER_T prAdapter,
				 IN u8 ucTC,
				 IN u8 fgReqLock);

WLAN_STATUS nicTxPollingResource(IN P_ADAPTER_T prAdapter, IN u8 ucTC);

u8 nicTxReleaseResource(IN P_ADAPTER_T prAdapter,
			IN u8 ucTc,
			IN u32 u4PageCount,
			IN u8 fgReqLock);

void nicTxDropInvalidMsduInfo(IN P_ADAPTER_T prAdapter,
			      IN P_MSDU_INFO_T prMsduInfo);

void nicTxReleaseMsduResource(IN P_ADAPTER_T prAdapter,
			      IN P_MSDU_INFO_T prMsduInfoListHead);

WLAN_STATUS nicTxResetResource(IN P_ADAPTER_T prAdapter);

u32 nicTxGetAdjustableResourceCnt(IN P_ADAPTER_T prAdapter);

u16 nicTxGetResource(IN P_ADAPTER_T prAdapter, IN u8 ucTC);

u8 nicTxGetFrameResourceType(IN u8 eFrameType, IN P_MSDU_INFO_T prMsduInfo);

u8 nicTxGetCmdResourceType(IN P_CMD_INFO_T prCmdInfo);

u8 nicTxSanityCheckResource(IN P_ADAPTER_T prAdapter);

void nicTxFillDesc(IN P_ADAPTER_T prAdapter,
		   IN P_MSDU_INFO_T prMsduInfo,
		   OUT u8 *prTxDescBuffer,
		   OUT u32 *pu4TxDescLength);

void nicTxFillDataDesc(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

void nicTxComposeSecurityFrameDesc(IN P_ADAPTER_T prAdapter,
				   IN P_CMD_INFO_T prCmdInfo,
				   OUT u8 *prTxDescBuffer,
				   OUT u8 *pucTxDescLength);

WLAN_STATUS nicTxMsduInfoList(IN P_ADAPTER_T prAdapter,
			      IN P_MSDU_INFO_T prMsduInfoListHead);

u8 nicTxGetTxQByTc(IN P_ADAPTER_T prAdapter, IN u8 ucTc);

WLAN_STATUS nicTxMsduInfoListMthread(IN P_ADAPTER_T prAdapter,
				     IN P_MSDU_INFO_T prMsduInfoListHead);

u32 nicTxMsduQueueMthread(IN P_ADAPTER_T prAdapter);

u32 nicTxGetMsduPendingCnt(IN P_ADAPTER_T prAdapter);

WLAN_STATUS nicTxMsduQueue(IN P_ADAPTER_T prAdapter, u8 ucPortIdx,
			   P_QUE_T prQue);

WLAN_STATUS nicTxCmd(IN P_ADAPTER_T prAdapter,
		     IN P_CMD_INFO_T prCmdInfo,
		     IN u8 ucTC);

void nicTxRelease(IN P_ADAPTER_T prAdapter, IN u8 fgProcTxDoneHandler);

void nicProcessTxInterrupt(IN P_ADAPTER_T prAdapter);

void nicTxFreeMsduInfoPacket(IN P_ADAPTER_T prAdapter,
			     IN P_MSDU_INFO_T prMsduInfoListHead);

void nicTxReturnMsduInfo(IN P_ADAPTER_T prAdapter,
			 IN P_MSDU_INFO_T prMsduInfoListHead);

u8 nicTxFillMsduInfo(IN P_ADAPTER_T prAdapter,
		     IN P_MSDU_INFO_T prMsduInfo,
		     IN P_NATIVE_PACKET prNdisPacket);

WLAN_STATUS nicTxAdjustTcq(IN P_ADAPTER_T prAdapter);

WLAN_STATUS nicTxFlush(IN P_ADAPTER_T prAdapter);

#if CFG_ENABLE_FW_DOWNLOAD
WLAN_STATUS nicTxInitCmd(IN P_ADAPTER_T prAdapter, IN P_CMD_INFO_T prCmdInfo);

WLAN_STATUS nicTxInitResetResource(IN P_ADAPTER_T prAdapter);
#endif

WLAN_STATUS nicTxEnqueueMsdu(IN P_ADAPTER_T prAdapter,
			     IN P_MSDU_INFO_T prMsduInfo);

u8 nicTxGetWlanIdx(IN P_ADAPTER_T prAdapter, IN u8 ucBssIdx, IN u8 ucStaRecIdx);

u8 nicTxIsMgmtResourceEnough(IN P_ADAPTER_T prAdapter);

u32 nicTxGetFreeCmdCount(IN P_ADAPTER_T prAdapter);

WLAN_STATUS nicTxGenerateDescTemplate(IN P_ADAPTER_T prAdapter,
				      IN P_STA_RECORD_T prStaRec);

void nicTxFreeDescTemplate(IN P_ADAPTER_T prAdapter,IN P_STA_RECORD_T prStaRec);

void nicTxFreePacket(IN P_ADAPTER_T prAdapter,
		     IN P_MSDU_INFO_T prMsduInfo,
		     IN u8 fgDrop);

void nicTxSetMngPacket(IN P_ADAPTER_T prAdapter,
		       IN P_MSDU_INFO_T prMsduInfo,
		       IN u8 ucBssIndex,
		       IN u8 ucStaRecIndex,
		       IN u8 ucMacHeaderLength,
		       IN u16 u2FrameLength,
		       IN PFN_TX_DONE_HANDLER pfTxDoneHandler,
		       IN u8 ucRateMode);

void nicTxSetDataPacket(IN P_ADAPTER_T prAdapter,
			IN P_MSDU_INFO_T prMsduInfo,
			IN u8 ucBssIndex,
			IN u8 ucStaRecIndex,
			IN u8 ucMacHeaderLength,
			IN u16 u2FrameLength,
			IN PFN_TX_DONE_HANDLER pfTxDoneHandler,
			IN u8 ucRateMode,
			IN ENUM_TX_PACKET_SRC_T eSrc,
			IN u8 ucTID,
			IN u8 fgIs802_11Frame,
			IN u8 fgIs1xFrame);

void nicTxFillDescByPktOption(IN P_MSDU_INFO_T prMsduInfo,
			      IN P_HW_MAC_TX_DESC_T prTxDesc);

void nicTxConfigPktOption(IN P_MSDU_INFO_T prMsduInfo,
			  IN u32 u4OptionMask,
			  IN u8 fgSetOption);

void nicTxFillDescByPktControl(P_MSDU_INFO_T prMsduInfo,
			       P_HW_MAC_TX_DESC_T prTxDesc);

void nicTxConfigPktControlFlag(IN P_MSDU_INFO_T prMsduInfo,
			       IN u8 ucControlFlagMask,
			       IN u8 fgSetFlag);

void nicTxSetPktLifeTime(IN P_MSDU_INFO_T prMsduInfo, IN u32 u4TxLifeTimeInMs);

void nicTxSetPktRetryLimit(IN P_MSDU_INFO_T prMsduInfo, IN u8 ucRetryLimit);

void nicTxSetPktPowerOffset(IN P_MSDU_INFO_T prMsduInfo, IN s8 cPowerOffset);

void nicTxSetPktSequenceNumber(IN P_MSDU_INFO_T prMsduInfo, IN u16 u2SN);

void nicTxSetPktMacTxQue(IN P_MSDU_INFO_T prMsduInfo, IN u8 ucMacTxQue);

void nicTxSetPktFixedRateOptionFull(P_MSDU_INFO_T prMsduInfo,
				    u16 u2RateCode,
				    u8 ucBandwidth,
				    u8 fgShortGI,
				    u8 fgLDPC,
				    u8 fgDynamicBwRts,
				    u8 fgBeamforming,
				    u8 ucAntennaIndex);

void nicTxSetPktFixedRateOption(IN P_MSDU_INFO_T prMsduInfo,
				IN u16 u2RateCode,
				IN u8 ucBandwidth,
				IN u8 fgShortGI,
				IN u8 fgDynamicBwRts);

void nicTxSetPktLowestFixedRate(IN P_ADAPTER_T prAdapter,
				IN P_MSDU_INFO_T prMsduInfo);

void nicTxSetPktMoreData(IN P_MSDU_INFO_T prCurrentMsduInfo,
			 IN u8 fgSetMoreDataBit);

void nicTxSetPktEOSP(IN P_MSDU_INFO_T prCurrentMsduInfo, IN u8 fgSetEOSPBit);

u8 nicTxAssignPID(IN P_ADAPTER_T prAdapter, IN u8 ucWlanIndex);

WLAN_STATUS
nicTxDummyTxDone(IN P_ADAPTER_T prAdapter,
		 IN P_MSDU_INFO_T prMsduInfo,
		 IN ENUM_TX_RESULT_CODE_T rTxDoneStatus);

void nicTxUpdateBssDefaultRate(IN P_BSS_INFO_T prBssInfo);

void nicTxUpdateStaRecDefaultRate(IN P_STA_RECORD_T prStaRec);

void nicTxPrintMetRTP(IN P_ADAPTER_T prAdapter,
		      IN P_MSDU_INFO_T prMsduInfo,
		      IN P_NATIVE_PACKET prPacket,
		      IN u32 u4PacketLen,
		      IN u8 bFreeSkb);

void nicTxProcessTxDoneEvent(IN P_ADAPTER_T prAdapter,
			     IN P_WIFI_EVENT_T prEvent,
			     IN u32 u4EventBufLen);

void nicTxMsduDoneCb(IN P_GLUE_INFO_T prGlueInfo, IN P_QUE_T prQue);

/* TX Direct functions : BEGIN */
void nicTxDirectStartCheckQTimer(IN P_ADAPTER_T prAdapter);
void nicTxDirectClearSkbQ(IN P_ADAPTER_T prAdapter);
void nicTxDirectClearHifQ(IN P_ADAPTER_T prAdapter);
void nicTxDirectClearStaPsQ(IN P_ADAPTER_T prAdapter, u8 ucStaRecIndex);
void nicTxDirectClearBssAbsentQ(IN P_ADAPTER_T prAdapter, u8 ucBssIndex);
void nicTxDirectClearAllStaPsQ(IN P_ADAPTER_T prAdapter);
#if KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE
void nicTxDirectTimerCheckSkbQ(struct timer_list *timer);
void nicTxDirectTimerCheckHifQ(struct timer_list *timer);
#else
void nicTxDirectTimerCheckSkbQ(unsigned long data);
void nicTxDirectTimerCheckHifQ(unsigned long data);
#endif
WLAN_STATUS nicTxDirectStartXmit(struct sk_buff *prSkb,
				 P_GLUE_INFO_T prGlueInfo);
/* TX Direct functions : END */

#if (HIF_TX_RSRC_WMM_ENHANCE == 1)
u8 nicTxGetWmmIdxByTc(u8 ucTC);
extern const u8 arTcRemapTable[HW_WMM_NUM][TC3_INDEX + 1];
#endif

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */

#endif
