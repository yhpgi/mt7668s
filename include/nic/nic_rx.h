/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   "nic_rx.h"
 *    \brief  The declaration of the nic rx functions
 *
 */

#ifndef _NIC_RX_H
#define _NIC_RX_H

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

#define UNIFIED_MAC_RX_FORMAT			     1

#define MAX_SEQ_NO				     4095
#define MAX_SEQ_NO_COUNT			     4096
#define HALF_SEQ_NO_CNOUT			     2048

#define HALF_SEQ_NO_COUNT			     2048

#define MT6620_FIXED_WIN_SIZE			     64
#define CFG_RX_MAX_BA_ENTRY			     4
#define CFG_RX_MAX_BA_TID_NUM			     8

#define RX_STATUS_FLAG_MORE_PACKET		     BIT(30)
#define RX_STATUS_CHKSUM_MASK			     BITS(0, 10)

#define RX_RFB_LEN_FIELD_LEN			     4
#define RX_HEADER_OFFSET			     2

#define RX_RETURN_INDICATED_RFB_TIMEOUT_SEC	     1

#define RX_BEHIND_CONTINUOUS_THRESHOLD		     256

#define SDIO_MAXIMUM_RX_LEN_NUM			     0 /*!< 0~15 (0: un-limited) */

/* RXM Definitions */
/* The payload format of a RX packet */
#define RX_PAYLOAD_FORMAT_MSDU			     0
#define RX_PAYLOAD_FORMAT_FIRST_SUB_AMSDU	     3
#define RX_PAYLOAD_FORMAT_MIDDLE_SUB_AMSDU	     2
#define RX_PAYLOAD_FORMAT_LAST_SUB_AMSDU	     1

/* HAL RX from hal_hw_def_rom.h */
/*------------------------------------------------------------------------------
 * Cipher define
 *------------------------------------------------------------------------------
 */
#define CIPHER_SUITE_NONE			     0
#define CIPHER_SUITE_WEP40			     1
#define CIPHER_SUITE_TKIP			     2
#define CIPHER_SUITE_TKIP_WO_MIC		     3
#define CIPHER_SUITE_CCMP			     4
#define CIPHER_SUITE_WEP104			     5
#define CIPHER_SUITE_BIP			     6
#define CIPHER_SUITE_WEP128			     7
#define CIPHER_SUITE_WPI			     8
#define CIPHER_SUITE_CCMP_W_CCX			     9
#define CIPHER_SUITE_GCMP			     10

/*------------------------------------------------------------------------------
 * Bit fields for HW_MAC_RX_DESC_T
 *------------------------------------------------------------------------------
 */

/*! MAC RX DMA Descriptor */
/* DW 0*/
/* Word 0 */
#define RX_STATUS_RX_BYTE_COUNT_MASK		     BITS(0, 16)
/* Word 1 */
#define RX_STATUS_ETH_TYPE_OFFSET_MASK		     BITS(0, 6)
#define RX_STATUS_ETH_TYPE_OFFSET		     0
#define RX_STATUS_IP_CHKSUM			     BIT(7)
#define RX_STATUS_UDP_TCP_CHKSUM		     BIT(8)
#define RX_STATUS_GROUP_VLD_MASK		     BITS(9, 12)
#define RX_STATUS_GROUP_VLD_OFFSET		     9
#define RX_STATUS_PKT_TYPE_MASK			     BITS(13, 15)
#define RX_STATUS_PKT_TYPE_OFFSET		     13

/* DW 1 */
/* Byte 0 */
#define RX_STATUS_HTC				     BIT(0)
#define RX_STATUS_UC2ME				     1
#define RX_STATUS_MC_FRAME			     2
#define RX_STATUS_BC_FRAME			     3
#define RX_STATUS_A1_TYPE_MASK			     BITS(1, 2)
#define RX_STATUS_A1_TYPE_OFFSET		     1
#define RX_STATUS_BCN_WITH_BMC			     BIT(4)
#define RX_STATUS_BCN_WITH_UC			     BIT(5)
#define RX_STATUS_KEYID_MASK			     BITS(6, 7)
#define RX_STATUS_KEYID_OFFSET			     6

/* Byte 1 */
#define RX_STATUS_CHAN_FREQ_MASK		     BITS(0, 7)
/* Byte 2 */
#define RX_STATUS_HEADER_LEN_MASK		     BITS(0, 5)
#define RX_STATUS_HEADER_OFFSET			     BIT(6)
#define RX_STATUS_HEADER_TRAN			     BIT(7)
/* Byte 3 */
#define RX_STATUS_PAYLOAD_FORMAT_MASK		     BITS(0, 1)
#define RX_STATUS_PAYLOAD_FORMAT_OFFSET		     0
#define RX_STATUS_BSSID_MASK			     BITS(2, 7)
#define RX_STATUS_BSSID_OFFSET			     2

/* DW 2 */
/* Byte 1 */
#define RX_STATUS_TID_MASK			     BITS(0, 3)
#define RX_STATUS_SEC_MASK			     BITS(4, 7)
#define RX_STATUS_SEC_OFFSET			     4
/* Byte 2-3 */
#define RX_STATUS_SW_BIT			     BIT(0)
#define RX_STATUS_FLAG_FCS_ERROR		     BIT(1)
#define RX_STATUS_FLAG_CIPHER_MISMATCH		     BIT(2)
#define RX_STATUS_FLAG_CIPHER_LENGTH_MISMATCH	     BIT(3)
#define RX_STATUS_FLAG_ICV_ERROR		     BIT(4)
#define RX_STATUS_FLAG_TKIPMIC_ERROR		     BIT(5)
#define RX_STATUS_FLAG_LEN_MISMATCH		     BIT(6)
#define RX_STATUS_FLAG_DE_AMSDU_FAIL		     BIT(7)
#define RX_STATUS_FLAG_EXCEED_LEN		     BIT(8)
#define RX_STATUS_LLC_MIS			     BIT(9)
#define RX_STATUS_UDF_VLT			     BIT(10)
#define RX_STATUS_FRAG				     BIT(11)
#define RX_STATUS_NULL				     BIT(12)
#define RX_STATUS_DATA				     BIT(13)
#define RX_STATUS_AMPDU_SUB_FRAME		     BIT(14)
#define RX_STATUS_AMPDU_FORMAT			     BIT(15)
#define PAYLOAD_FORMAT_IS_MSDU_FRAME		     0
#define RX_STATUS_FLAG_ERROR_MASK			       \
	(RX_STATUS_FLAG_FCS_ERROR | RX_STATUS_FLAG_ICV_ERROR | \
	 RX_STATUS_FLAG_CIPHER_LENGTH_MISMATCH) /* No TKIP MIC error \
	                                         */

/* DW 3 */
#define RX_STATUS_RXV_SEQ_NO_MASK		     BITS(0, 7)
#define RX_STATUS_TCL				     BIT(8)
#define RX_STATUS_CLS				     BIT(11)
#define RX_STATUS_OFLD_MASK			     BITS(12, 13)
#define RX_STATUS_OFLD_OFFSET			     12
#define RX_STATUS_EAPOL_PACKET			     BIT(12)
#define RX_STATUS_ARP_NS_PACKET			     BIT(13)
#define RX_STATUS_TDLS_PACKET			     BITS(12, 13)
#define RX_STATUS_MGC				     BIT(14)
#define RX_STATUS_WOL_MASK			     BITS(15, 19)
#define RX_STATUS_WOL_OFFSET			     15
#define RX_STATUS_CLS_BITMAP_MASK		     BITS(20, 29)
#define RX_STATUS_CLS_BITMAP_OFFSET		     20
#define RX_STATUS_PF_MODE_BLACK_LIST		     BIT(30)
#define RX_STATUS_PF_STS_CHECKED		     BIT(31)

/* DW 12 */
#define RX_STATUS_FRAG_NUM_MASK			     BITS(0, 3)
#define RX_STATUS_SEQ_NUM_MASK			     BITS(4, 15)
#define RX_STATUS_SEQ_NUM_OFFSET		     4

#define RX_STATUS_GROUP1_VALID			     BIT(0)
#define RX_STATUS_GROUP2_VALID			     BIT(1)
#define RX_STATUS_GROUP3_VALID			     BIT(2)
#define RX_STATUS_GROUP4_VALID			     BIT(3)

#define RX_STATUS_FIXED_LEN			     16

#define RX_STATUS_CHAN_FREQ_MASK_FOR_BY_PASS_MPDE    BITS(0, 7)
#define RX_STATUS_FLAG_FCS_ERROR_FOR_BY_PASS_MODE    BIT(16)

/* Timing Measurement Report */
/* DW0 Word 1 */
#define RX_TMR_TOA_VALID			     BIT(11)
#define RX_TMR_TOD_VALID			     BIT(10)
#define RX_TMR_TYPE_MASK			     BITS(8, 9)
#define RX_TMR_TYPE_OFFSET			     8
#define RX_TMR_SUBTYPE_MASK			     BITS(4, 7)
#define RX_TMR_SUBTYPE_OFFSET			     4

/* DW0 Byte 1*/
#define RX_TMR_TM_FAILED			     BIT(2)
#define RX_TMR_NOISY_CHAN			     BIT(1)
#define RX_TMR_RESPONDER			     BIT(0)

/* TBD */
#define DMA_OWN_TO_HW				     BIT(0)
#define DMA_OWN_TO_FW_PENDING			     BIT(1)
#define STATUS_IS_OWN_TO_FW(flag)    (((flag)&DMA_OWN_TO_HW) ? false : true)
#define STATUS_IS_FW_PENDING(flag) \
	(((flag)&DMA_OWN_TO_FW_PENDING) ? true : false)

/* DW 2 */
#define RX_STATUS_PACKET_LENGTH_MASK		     BITS(0, 16)

#define RX_STATUS_HEADER_TRAN_MASK		     BIT(7)
#define RX_STATUS_HEADER_TRAN_OFFSET		     7
#define RX_STATUS_HEADER_TRAN_BSS0_MASK		     BIT(6)
#define RX_STATUS_HEADER_TRAN_BSS0_OFFSET	     6
#define RX_STATUS_HEADER_TRAN_BSS1_MASK		     BIT(7)
#define RX_STATUS_HEADER_TRAN_BSS1_OFFSET	     7

/* DW 4 */
#define RX_STATUS_MATCH_PACKET			     BIT(4)

#define RX_STATUS_HEADER_OFFSET_MASK		     0xC0
#define RX_STATUS_HEADER_OFFSET_OFFSET		     6

/*------------------------------------------------------------------------------
 * Bit fields for HW_RX_VECTOR_DESC_T
 *------------------------------------------------------------------------------
 */
/* DW 2 */
#define RX_VECTOR_FOR_BA_ACK			     BIT(7)

/*! HIF RX DMA Descriptor */
/* DW 2 */
#define HIF_RX_DESC_BUFFER_LEN			     BITS(0, 15)
#define HIF_RX_DESC_ETHER_TYPE_OFFSET_MASK	     BITS(16, 23)
#define HIF_RX_DESC_ETHER_TYPE_OFFSET_OFFSET	     16
#define HIF_RX_DESC_IP_CHKSUM_CHECK		     BIT(24)
#define HIF_RX_DESC_TCP_UDP_CHKSUM_CHECK	     BIT(25)

#define HIF_RX_DATA_QUEUE			     0
#define HIF_RX_EVENT_QUEUE			     1

/*------------------------------------------------------------------------------
 * Bit fields for PHY Vector
 *------------------------------------------------------------------------------
 */

/* RX Vector, 1st Cycle */
#define RX_VT_RX_RATE_AC_MASK			     BITS(0, 3)
#define RX_VT_RX_RATE_MASK			     BITS(0, 6)
#define RX_VT_RX_RATE_OFFSET			     0
#define RX_VT_STBC_MASK				     BITS(7, 8)
#define RX_VT_STBC_OFFSET			     7
#define RX_VT_LDPC				     BIT(9)
#define RX_VT_NESS_MASK				     BITS(10, 11)
#define RX_VT_NESS_OFFSET			     10
#define RX_VT_RX_MODE_MASK			     BITS(12, 14)
#define RX_VT_RX_MODE_OFFSET			     12
#define RX_VT_RX_MODE_VHT			     BIT(14)
#define RX_VT_FR_MODE_MASK			     BITS(15, 16)
#define RX_VT_FR_MODE_OFFSET			     15
#define RX_VT_TXOP_PS_NOT_ALLOWED		     BIT(17)
#define RX_VT_AGGREGATION			     BIT(18)
#define RX_VT_SHORT_GI				     BIT(19)
#define RX_VT_SMOOTH				     BIT(20)
#define RX_VT_NO_SOUNDING			     BIT(21)
#define RX_VT_SOUNDING				     BIT(21)
#define RX_VT_NUM_RX_MASK			     BITS(22, 23)
#define RX_VT_NUM_RX_OFFSET			     22
#define RX_VT_LDPC_EXTRA_OFDM_SYM		     BIT(24) /* VHT_SIG_A2[B3] */
#define RX_VT_SU_VHT_MU1_3_CODING		     BITS(25, 28) /* VHT_SIG_A2[B4:B7] */
#define RX_VT_SU_VHT_MU1_3_CODING_OFFSET	     25
#define RX_VT_BEAMFORMED			     BIT(29) /* VHT_SIG_A2[B8] */
#define RX_VT_ACID_DET_LOW			     BIT(30)
#define RX_VT_ACID_DET_HIGH			     BIT(31)

#define RX_VT_RX_RATE_1M			     0x0
#define RX_VT_RX_RATE_2M			     0x1
#define RX_VT_RX_RATE_5M			     0x2
#define RX_VT_RX_RATE_11M			     0x3
#define RX_VT_RX_RATE_6M			     0xB
#define RX_VT_RX_RATE_9M			     0xF
#define RX_VT_RX_RATE_12M			     0xA
#define RX_VT_RX_RATE_18M			     0xE
#define RX_VT_RX_RATE_24M			     0x9
#define RX_VT_RX_RATE_36M			     0xD
#define RX_VT_RX_RATE_48M			     0x8
#define RX_VT_RX_RATE_54M			     0xC

#define RX_VT_RX_RATE_MCS0			     0
#define RX_VT_RX_RATE_MCS1			     1
#define RX_VT_RX_RATE_MCS2			     2
#define RX_VT_RX_RATE_MCS3			     3
#define RX_VT_RX_RATE_MCS4			     4
#define RX_VT_RX_RATE_MCS5			     5
#define RX_VT_RX_RATE_MCS6			     6
#define RX_VT_RX_RATE_MCS7			     7
#define RX_VT_RX_RATE_MCS32			     32

#define RX_VT_LEGACY_CCK			     0
#define RX_VT_LEGACY_OFDM			     1
#define RX_VT_MIXED_MODE			     2
#define RX_VT_GREEN_MODE			     3
#define RX_VT_VHT_MODE				     4

#define RX_VT_LG20_HT20				     0
#define RX_VT_DL40_HT40				     1
#define RX_VT_U20				     2
#define RX_VT_L20				     3

#define RX_VT_FR_MODE_20			     0
#define RX_VT_FR_MODE_40			     1
#define RX_VT_FR_MODE_80			     2
#define RX_VT_FR_MODE_160			     3 /*BW160 or BW80+80*/

#define RX_VT_CCK_SHORT_PREAMBLE		     BIT(2)

/* RX Vector, 2nd Cycle */
#define RX_VT_RX_LEN_HT_MASK			     BITS(0, 15)
#define RX_VT_RX_LEN_LEACY_MASK			     BITS(0, 11)
#define RX_VT_RX_LEN_VHT_MASK			     BITS(0, 20)
#define RX_VT_GROUP_ID_MASK			     BITS(21, 26)
#define RX_VT_GROUP_ID_OFFSET			     21
#define RX_VT_GROUPID_0_MASK			     BITS(21, 22) /* VHT_SIG_A1[B4:B5] */
#define RX_VT_GROUPID_0_OFFSET			     21
#define RX_VT_GROUPID_1_MASK			     BITS(23, 26) /* VHT_SIG_A1[B6:B9] */
#define RX_VT_GROUPID_1_OFFSET			     23

#define RX_VT_NSTS_MASK				     BITS(27, 29)
#define RX_VT_NSTS_OFFSET			     27
#define RX_VT_RX_INDICATOR			     BIT(30)
#define RX_VT_SEL_ANT				     BIT(31) /* Not use in MT7615 and MT6632 */

/* RX Vector, 3rd Cycle */
#define RX_VT_PART_AID_MASK			     BITS(3, 11)
#define RX_VT_PART_AID_OFFSET			     3
#define RX_VT_AID_0_MASK			     BITS(3, 6) /* VHT_SIG_A1[B13:B16] */
#define RX_VT_AID_0_OFFSET			     3
#define RX_VT_AID_1_MASK			     BITS(7, 11) /* VHT_SIG_A1[B17:B21] */
#define RX_VT_AID_1_OFFSET			     7

#define RX_VT_NSTS_PART_AID_MASK		     BITS(0, 11)
#define RX_VT_NSTS_PART_AID_OFFSET		     0
#define RX_VT_POP_EVER_TRIG			     BIT(12)
#define RX_VT_FAGC_LNA_RX_MASK			     BITS(13, 15)
#define RX_VT_FAGC_LNA_RX_OFFSET		     13
#define RX_VT_IB_RSSI_MASK			     BITS(16, 23)
#define RX_VT_IB_RSSI_OFFSET			     16
#define RX_VT_WB_RSSI_MASK			     BITS(24, 31)
#define RX_VT_WB_RSSI_OFFSET			     24

/* RX Vector, 4th Cycle */
#define RX_VT_RCPI0_MASK			     BITS(0, 7)
#define RX_VT_RCPI0_OFFSET			     0
#define RX_VT_RCPI1_MASK			     BITS(8, 15)
#define RX_VT_RCPI1_OFFSET			     8
#define RX_VT_RCPI2_MASK			     BITS(16, 23)
#define RX_VT_RCPI2_OFFSET			     16
#define RX_VT_RCPI3_MASK			     BITS(24, 31)
#define RX_VT_RCPI3_OFFSET			     24

/* RX Vector, 5th Cycle */
#define RX_VT_FAGC_LNA_GAIN_MASK		     BITS(0, 2)
#define RX_VT_FAGC_LNA_GAIN_OFFSET		     0
#define RX_VT_FAGC_LPF_GAIN_MASK		     BITS(3, 6)
#define RX_VT_FAGC_LPF_GAIN_OFFSET		     3
#define RX_VT_OFDM_FOE_MASK			     BITS(7, 18)
#define RX_VT_OFDM_FOE_OFFSET			     7
#define RX_VT_LTF_PROC_TIME_MASK		     BITS(19, 25)
#define RX_VT_LTF_PROC_TIME_OFFSET		     19
#define RX_VT_LTF_SNR_MASK			     BITS(26, 31)
#define RX_VT_LTF_SNR_OFFSET			     26

/*RX Vector, 6th Cycle*/
#define RX_VT_NF0_MASK				     BITS(0, 7)
#define RX_VT_NF0_OFFSET			     0
#define RX_VT_NF1_MASK				     BITS(8, 15)
#define RX_VT_NF1_OFFSET			     8
#define RX_VT_NF2_MASK				     BITS(16, 23)
#define RX_VT_NF2_OFFSET			     16
#define RX_VT_NF3_MASK				     BITS(24, 31)
#define RX_VT_NF3_OFFSET			     24

/* RX Vector Group 2, the 1st cycle */
#define RX_VT_PRIM_ITFR_ENV			     BIT(0)
#define RX_VT_SEC_ITFR_ENV			     BIT(1)
#define RX_VT_SEC40_ITFR_ENV			     BIT(2)
#define RX_VT_SEC80_ITFR_ENV			     BIT(3)
#define RX_VT_OFDM_LQ_BPSK_MASK			     BITS(4, 10)
#define RX_VT_OFDM_LQ_BPSK_OFFSET		     4
#define RX_VT_OFDM_CAPACITY_LQ_MASK		     BITS(11, 17)
#define RX_VT_OFDM_CAPACITY_LQ_OFFSET		     11
#define RX_VT_CCK_LQ_MASK			     BITS(4, 13)
#define RX_VT_CCK_LQ_OFFSET			     4

/* RX Vector Group 2, the 2nd cycle */
#define RX_VT_DYNA_BW_IN_NON_HT_DYNA		     BIT(19)
#define RX_VT_CH_BW_IN_NON_HT_MASK		     BITS(20, 21)
#define RX_VT_CH_BW_IN_NON_HT_OFFSET		     20

#define RX_VT_CH_BW_IN_NON_HT_CBW40		     BIT(20)
#define RX_VT_CH_BW_IN_NON_HT_CBW80		     BIT(21)
#define RX_VT_CH_BW_IN_NON_HT_CBW160		     BITS(20, 21)

/* RX Data Type */
#define RX_DATA_TYPE_RX_VECTOR			     0
#define RX_DATA_TYPE_RX_DATA			     1
#define RX_DATA_TYPE_RX_EVM			     2
#define RX_DATA_TYPE_RX_AMBI			     3
#define RX_DATA_TYPE_RX_BT			     4

/*------------------------------------------------------------------------------
 * Radiotap define
 *------------------------------------------------------------------------------
 */

/*Radiotap VHT*/
#define RADIOTAP_VHT_ALL_KNOWN			     BITS(0, 8)
#define RADIOTAP_VHT_STBC_KNOWN			     BIT(0)
#define RADIOTAP_VHT_TXOP_PS_NOT_ALLOWED_KNOWN	     BIT(1)
#define RADIOTAP_VHT_GI_KNOWN			     BIT(2)
#define RADIOTAP_VHT_SHORT_GI_NSYM_KNOWN	     BIT(3)
#define RADIOTAP_VHT_LDPC_EXTRA_OFDM_SYM_KNOWN	     BIT(4)
#define RADIOTAP_VHT_BEAMFORMED_KNOWN		     BIT(5)
#define RADIOTAP_VHT_BAND_WIDTH_KNOWN		     BIT(6)
#define RADIOTAP_VHT_BAND_GROUP_ID_KNOWN	     BIT(7)
#define RADIOTAP_VHT_BAND_PARTIAL_AID_KNOWN	     BIT(8)

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

typedef enum _ENUM_RX_STATISTIC_COUNTER_T {
	RX_MPDU_TOTAL_COUNT = 0,
	RX_SIZE_ERR_DROP_COUNT,

	RX_DATA_INDICATION_COUNT,
	RX_DATA_RETURNED_COUNT,
	RX_DATA_RETAINED_COUNT,

	RX_DATA_REORDER_TOTAL_COUINT,
	RX_DATA_REORDER_MISS_COUNT,
	RX_DATA_REORDER_WITHIN_COUNT,
	RX_DATA_REORDER_AHEAD_COUNT,
	RX_DATA_REORDER_BEHIND_COUNT,
	RX_DATA_REORDER_BEHIND_CONTINUOUS_COUNT,

	RX_DATA_MSDU_IN_AMSDU_COUNT,
	RX_DATA_AMSDU_MISS_COUNT,
	RX_DATA_AMSDU_COUNT,

	RX_DROP_TOTAL_COUNT,

	RX_NO_STA_DROP_COUNT,
	RX_INACTIVE_BSS_DROP_COUNT,
	RX_HS20_DROP_COUNT,
	RX_LESS_SW_RFB_DROP_COUNT,
	RX_DUPICATE_DROP_COUNT,
	RX_MIC_ERROR_DROP_COUNT,
	RX_BAR_DROP_COUNT,
	RX_NO_INTEREST_DROP_COUNT,
	RX_TYPE_ERR_DROP_COUNT,
	RX_CLASS_ERR_DROP_COUNT,
	RX_DST_NULL_DROP_COUNT,

#if CFG_TCP_IP_CHKSUM_OFFLOAD || CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60
	RX_CSUM_TCP_FAILED_COUNT,
	RX_CSUM_UDP_FAILED_COUNT,
	RX_CSUM_IP_FAILED_COUNT,
	RX_CSUM_TCP_SUCCESS_COUNT,
	RX_CSUM_UDP_SUCCESS_COUNT,
	RX_CSUM_IP_SUCCESS_COUNT,
	RX_CSUM_UNKNOWN_L4_PKT_COUNT,
	RX_CSUM_UNKNOWN_L3_PKT_COUNT,
	RX_IP_V6_PKT_CCOUNT,
#endif
#if CFG_KEY_ERROR_STATISTIC_RECOVERY
	RX_BMC_NO_KEY_COUNT,
	RX_BMC_KEY_ERROR_COUNT,
	RX_BMC_PKT_COUNT,
#endif
	RX_STATISTIC_COUNTER_NUM
} ENUM_RX_STATISTIC_COUNTER_T;

typedef enum _ENUM_RX_PKT_DESTINATION_T {
	RX_PKT_DESTINATION_HOST, /* to OS */
	RX_PKT_DESTINATION_FORWARD, /* to TX queue for forward, AP mode */
	RX_PKT_DESTINATION_HOST_WITH_FORWARD, /* to both TX and OS, AP mode
	                                       * broadcast packet */
	RX_PKT_DESTINATION_NULL, /* packet to be freed */
	RX_PKT_DESTINATION_NUM
} ENUM_RX_PKT_DESTINATION_T;

/* Used for MAC RX */
typedef enum _ENUM_MAC_RX_PKT_TYPE_T {
	RX_PKT_TYPE_TX_STATUS = 0,
	RX_PKT_TYPE_RX_VECTOR,
	RX_PKT_TYPE_RX_DATA,
	RX_PKT_TYPE_DUP_RFB,
	RX_PKT_TYPE_TM_REPORT,
	RX_PKT_TYPE_MSDU_REPORT = 6,
	RX_PKT_TYPE_SW_DEFINED = 7
} ENUM_MAC_RX_PKT_TYPE_T;

typedef enum _ENUM_MAC_RX_GROUP_VLD_T {
	RX_GROUP_VLD_1 = 0,
	RX_GROUP_VLD_2,
	RX_GROUP_VLD_3,
	RX_GROUP_VLD_4,
	RX_GROUP_VLD_NUM
} ENUM_MAC_RX_GROUP_VLD_T;

typedef enum _ENUM_MAC_GI_INFO_T {
	MAC_GI_NORMAL = 0,
	MAC_GI_SHORT
} ENUM_MAC_GI_INFO_T,
*P_ENUM_MAC_GI_INFO_T;

typedef enum _ENUM_RXPI_MODE_T {
	RCPI_MODE_WF0 = 0,
	RCPI_MODE_WF1,
	RCPI_MODE_WF2,
	RCPI_MODE_WF3,
	RCPI_MODE_AVG,
	RCPI_MODE_MAX,
	RCPI_MODE_MIN,
	RCPI_MODE_NUM
} ENUM_RXPI_MODE_T;

#define RXM_RXD_PKT_TYPE_SW_BITMAP    0xE00F
#define RXM_RXD_PKT_TYPE_SW_EVENT     0xE000
#define RXM_RXD_PKT_TYPE_SW_FRAME     0xE001

/* AMPDU data frame with no errors including FC/FM/I/T/LM/DAF/EL/LLC-MIS/ UDFVLT
 * and Class 3 error */
#define RXS_DW2_AMPDU_nERR_BITMAP     0xFFFB /* ignore CM bit (2) 0xFFFF */
#define RXS_DW2_AMPDU_nERR_VALUE      0x0000
/* no error including FC/CLM/I/T/LM/DAF/EL/HTF */
#define RXS_DW2_RX_nERR_BITMAP	      0x03FA /* ignore CM bit (2) 0x03FE */
#define RXS_DW2_RX_nERR_VALUE	      0x0000
/* Non-Data frames */
#define RXS_DW2_RX_nDATA_BITMAP	      0x3000
#define RXS_DW2_RX_nDATA_VALUE	      0x2000
/* Claas Error */
#define RXS_DW2_RX_CLASSERR_BITMAP    0x0001
#define RXS_DW2_RX_CLASSERR_VALUE     0x0001
/* Fragmentation */
#define RXS_DW2_RX_FRAG_BITMAP	      0x3800
#define RXS_DW2_RX_FRAG_VALUE	      0x0800

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */

/*! A data structure which is identical with MAC RX DMA Descriptor */
typedef struct _HW_MAC_RX_DESC_T {
	u16 u2RxByteCount; /* DW 0 */
	u16 u2PktTYpe;
	u8 ucMatchPacket; /* DW 1 */
	u8 ucChanFreq;
	u8 ucHeaderLen;
	u8 ucBssid;
	u8 ucWlanIdx; /* DW 2 */
	u8 ucTidSecMode;
	u16 u2StatusFlag;
	u32 u4PatternFilterInfo; /* DW 3 */
} HW_MAC_RX_DESC_T, *P_HW_MAC_RX_DESC_T;

typedef struct _HW_MAC_RX_STS_GROUP_1_T {
	u8 aucPN[16];
} HW_MAC_RX_STS_GROUP_1_T, *P_HW_MAC_RX_STS_GROUP_1_T;

typedef struct _HW_MAC_RX_STS_GROUP_2_T {
	u32 u4Timestamp; /* DW 12 */
	u32 u4CRC; /* DW 13 */
} HW_MAC_RX_STS_GROUP_2_T, *P_HW_MAC_RX_STS_GROUP_2_T;

typedef struct _HW_MAC_RX_STS_GROUP_4_T {
	/* For HDR_TRAN */
	u16 u2FrameCtl; /* DW 4 */
	u8 aucTA[6]; /* DW 4~5 */
	u16 u2SeqFrag; /* DW 6 */
	u16 u2Qos; /* DW 6 */
	u32 u4HTC; /* DW 7 */
} HW_MAC_RX_STS_GROUP_4_T, *P_HW_MAC_RX_STS_GROUP_4_T;

typedef struct _HW_MAC_RX_STS_GROUP_3_T {
	/*!  RX Vector Info */
	u32 u4RxVector[6]; /* DW 14~19 */
} HW_MAC_RX_STS_GROUP_3_T, *P_HW_MAC_RX_STS_GROUP_3_T;

typedef struct _HW_MAC_RX_TMRI_PKT_FORMAT_T {
	u8 ucPID;
	u8 ucStatus;
	u16 u2PktTYpe;
	u32 u4Reserved[2];
	u32 u4ToA;
	u32 u4ToD;
} HW_MAC_RX_TMRI_PKT_FORMAT_T, *P_HW_MAC_RX_TMRI_PKT_FORMAT_T;

typedef struct _HW_MAC_RX_TMRR_PKT_FORMAT_T {
	u8 ucVtSeq;
	u8 ucStatus;
	u16 u2PktTYpe;
	u8 aucTALow[2];
	u16 u2SnField;
	u8 aucTAHigh[4];
	u32 u4ToA;
	u32 u4ToD;
} HW_MAC_RX_TMRR_PKT_FORMAT_T, *P_HW_MAC_RX_TMRR_PKT_FORMAT_T;

/*! A data structure which is identical with MAC RX Vector DMA Descriptor */
typedef struct _HW_RX_VECTOR_DESC_T {
	u8 aucTA[6]; /* DW 0~1 */
	u8 ucRxVtSeqNo;
	/*!  RX Vector Info */
	u32 u4RxVector[9]; /* DW 2~10 */
} HW_RX_VECTOR_DESC_T, *P_HW_RX_VECTOR_DESC_T;

typedef struct _HW_MAC_MSDU_REPORT_T {
	/* 1st DW */
	u16 u2BufByteCount;
	u16 u2MsduCount : 7;
	u16 u2DoneEventType : 6;
	u16 u2PktType : 3;
	/* 2nd DW */
	u32 u4TxdCount : 8;
	u32 u4RvS2 : 24;
	/* MSDU token array */
	u16 au2MsduToken[0];
} HW_MAC_MSDU_REPORT_T, *P_HW_MAC_MSDU_REPORT_T;

struct _SW_RFB_T {
	QUE_ENTRY_T rQueEntry;
	void *pvPacket; /*!< ptr to rx Packet Descriptor */
	u8 *pucRecvBuff; /*!< ptr to receive data buffer */

	/* add fot mt6630 */
	u8 ucGroupVLD;
	u16 u2RxStatusOffst;
	P_HW_MAC_RX_DESC_T prRxStatus;
	P_HW_MAC_RX_STS_GROUP_1_T prRxStatusGroup1;
	P_HW_MAC_RX_STS_GROUP_2_T prRxStatusGroup2;
	P_HW_MAC_RX_STS_GROUP_3_T prRxStatusGroup3;
	P_HW_MAC_RX_STS_GROUP_4_T prRxStatusGroup4;

	/* rx data information */
	void *pvHeader;
	u16 u2PacketLen;
	u16 u2HeaderLen;
	u8 ucHeaderOffset;

	u8 *pucPayload;
	u16 u2PayloadLength;

	P_STA_RECORD_T prStaRec;

	u8 ucPacketType;
	u8 ucPayloadFormat;
	u8 ucSecMode;

	/* rx sta record */
	u8 ucWlanIdx;
	u8 ucStaRecIdx;

	u8 fgReorderBuffer;
	u8 fgDataFrame;
	u8 fgFragFrame;
	u8 fgHdrTran;
	u8 fgIsBC;
	u8 fgIsMC;
	/* duplicate detection */
	u16 u2FrameCtrl;
	u16 u2SequenceControl;
	u16 u2SSN;
	u8 ucTid;

	ENUM_CSUM_RESULT_T aeCSUM[CSUM_TYPE_NUM];
	ENUM_RX_PKT_DESTINATION_T eDst;
	ENUM_TRAFFIC_CLASS_INDEX_T eTC; /* only valid when eDst == FORWARD */
};

/*! RX configuration type structure */
typedef struct _RX_CTRL_T {
	u32 u4RxCachedSize;
	u8 *pucRxCached;
	QUE_T rFreeSwRfbList;
	QUE_T rReceivedRfbList;
	QUE_T rIndicatedRfbList;

#if CFG_SDIO_RX_AGG
	u8 *pucRxCoalescingBufPtr;
#endif

	void *apvIndPacket[CFG_RX_MAX_PKT_NUM];
	void *apvRetainedPacket[CFG_RX_MAX_PKT_NUM];

	u8 ucNumIndPacket;
	u8 ucNumRetainedPacket;
	u64 au8Statistics[RX_STATISTIC_COUNTER_NUM]; /*!< RX Counters */

#if CFG_HIF_STATISTICS
	u32 u4TotalRxAccessNum;
	u32 u4TotalRxPacketNum;
#endif

#if CFG_HIF_RX_STARVATION_WARNING
	u32 u4QueuedCnt;
	u32 u4DequeuedCnt;
#endif

#if CFG_RX_PKTS_DUMP
	u32 u4RxPktsDumpTypeMask;
#endif

#if CFG_SUPPORT_SNIFFER
	u32 u4AmpduRefNum;
#endif
} RX_CTRL_T, *P_RX_CTRL_T;

typedef struct _RX_MAILBOX_T {
	u32 u4RxMailbox[2]; /* for Device-to-Host Mailbox */
} RX_MAILBOX_T, *P_RX_MAILBOX_T;

typedef WLAN_STATUS (*PROCESS_RX_MGT_FUNCTION)(P_ADAPTER_T, P_SW_RFB_T);

typedef void (*PROCESS_RX_EVENT_FUNCTION)(P_ADAPTER_T, P_WIFI_EVENT_T, u32);

typedef struct _RX_EVENT_HANDLER_T {
	ENUM_EVENT_ID_T eEID;
	PROCESS_RX_EVENT_FUNCTION pfnHandler;
} RX_EVENT_HANDLER_T, *P_RX_EVENT_HANDLER_T;

typedef struct _EMU_MAC_RATE_INFO_T {
	u8 ucPhyRateCode;
	u32 u4PhyRate[4][2];
} EMU_MAC_RATE_INFO_T, *P_EMU_MAC_RATE_INFO_T;

/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */

#define RATE_INFO(_RateCode, _Bw20, _Bw20SGI, _Bw40, _BW40SGI, _Bw80,	   \
		  _Bw80SGI, _Bw160, _Bw160SGI)				   \
	{								   \
		.ucPhyRateCode = (_RateCode),				   \
		.u4PhyRate[RX_VT_FR_MODE_20][MAC_GI_NORMAL] = (_Bw20),	   \
		.u4PhyRate[RX_VT_FR_MODE_20][MAC_GI_SHORT] = (_Bw20SGI),   \
		.u4PhyRate[RX_VT_FR_MODE_40][MAC_GI_NORMAL] = (_Bw40),	   \
		.u4PhyRate[RX_VT_FR_MODE_40][MAC_GI_SHORT] = (_BW40SGI),   \
		.u4PhyRate[RX_VT_FR_MODE_80][MAC_GI_NORMAL] = (_Bw80),	   \
		.u4PhyRate[RX_VT_FR_MODE_80][MAC_GI_SHORT] = (_Bw80SGI),   \
		.u4PhyRate[RX_VT_FR_MODE_160][MAC_GI_NORMAL] = (_Bw160),   \
		.u4PhyRate[RX_VT_FR_MODE_160][MAC_GI_SHORT] = (_Bw160SGI), \
	}

#define RX_INC_CNT(prRxCtrl, eCounter)				    \
	{							    \
		((P_RX_CTRL_T)prRxCtrl)->au8Statistics[eCounter]++; \
	}

#define RX_ADD_CNT(prRxCtrl, eCounter, u8Amount)		    \
	{							    \
		((P_RX_CTRL_T)prRxCtrl)->au8Statistics[eCounter] += \
			(u64)u8Amount;				    \
	}

#define RX_GET_CNT(prRxCtrl, eCounter) \
	(((P_RX_CTRL_T)prRxCtrl)->au8Statistics[eCounter])

#define RX_RESET_CNT(prRxCtrl, eCounter)			      \
	{							      \
		((P_RX_CTRL_T)prRxCtrl)->au8Statistics[eCounter] = 0; \
	}

#define RX_RESET_ALL_CNTS(prRxCtrl)			     \
	{						     \
		kalMemZero(&prRxCtrl->au8Statistics[0],	     \
			   sizeof(prRxCtrl->au8Statistics)); \
	}

#define RX_STATUS_TEST_MORE_FLAG(flag) \
	((u8)((flag & RX_STATUS_FLAG_MORE_PACKET) ? true : false))

/*------------------------------------------------------------------------------
 * MACRO for HW_MAC_RX_DESC_T
 *------------------------------------------------------------------------------
 */
/* DW 0 */
#define HAL_RX_STATUS_GET_RX_BYTE_CNT(_prHwMacRxDesc) \
	((_prHwMacRxDesc)->u2RxByteCount)
#define HAL_RX_STATUS_GET_ETH_TYPE_OFFSET(_prHwMacRxDesc)		   \
	(((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_ETH_TYPE_OFFSET_MASK) >> \
	 RX_STATUS_ETH_TYPE_OFFSET)
#define HAL_RX_STATUS_GET_GROUP_VLD(_prHwMacRxDesc)		     \
	(((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_GROUP_VLD_MASK) >> \
	 RX_STATUS_GROUP_VLD_OFFSET)
#define HAL_RX_STATUS_GET_PKT_TYPE(_prHwMacRxDesc)		    \
	(((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_PKT_TYPE_MASK) >> \
	 RX_STATUS_PKT_TYPE_OFFSET)

/* DW 1 */
#define HAL_RX_STATUS_IS_HTC_EXIST(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_HTC) ? true : false)
#define HAL_RX_STATUS_IS_U2ME(_prHwMacRxDesc)				\
	((((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_A1_TYPE_MASK) >>	\
	  RX_STATUS_A1_TYPE_OFFSET ==					\
	  RX_STATUS_UC2ME) ?						\
	 true :								\
	 false)
#define HAL_RX_STATUS_IS_MC(_prHwMacRxDesc)				\
	((((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_A1_TYPE_MASK) >>	\
	  RX_STATUS_A1_TYPE_OFFSET ==					\
	  RX_STATUS_MC_FRAME) ?						\
	 true :								\
	 false)
#define HAL_RX_STATUS_IS_BC(_prHwMacRxDesc)				\
	((((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_A1_TYPE_MASK) >>	\
	  RX_STATUS_A1_TYPE_OFFSET ==					\
	  RX_STATUS_BC_FRAME) ?						\
	 true :								\
	 false)
#define HAL_RX_STATUS_IS_BCN_WITH_BMC(_prHwMacRxDesc)			     \
	(((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_BCN_WITH_BMC) ? true : \
	 false)
#define HAL_RX_STATUS_IS_BCN_WITH_UC(_prHwMacRxDesc)			    \
	(((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_BCN_WITH_UC) ? true : \
	 false)
#define HAL_RX_STATUS_GET_KEY_ID(_prHwMacRxDesc)		     \
	(((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_KEYID_MASK) >> \
	 RX_STATUS_KEYID_OFFSET)
#define HAL_RX_STATUS_GET_CHAN_FREQ(_prHwMacRxDesc) \
	((_prHwMacRxDesc)->ucChanFreq)
#define HAL_RX_STATUS_GET_HEADER_LEN(_prHwMacRxDesc) \
	((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_LEN_MASK)
#define HAL_RX_STATUS_IS_HEADER_OFFSET(_prHwMacRxDesc)			    \
	(((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET) ? true : \
	 false)
#define HAL_RX_STATUS_GET_HEADER_OFFSET(_prHwMacRxDesc)	\
	(((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET) ? 2 : 0)
#define HAL_RX_STATUS_IS_HEADER_TRAN(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_TRAN) ? true : false)
#define HAL_RX_STATUS_GET_HEADER_TRAN(_prHwMacRxDesc) \
	HAL_RX_STATUS_IS_HEADER_TRAN(_prHwMacRxDesc)
#define HAL_RX_STATUS_GET_PAYLOAD_FORMAT(_prHwMacRxDesc)		\
	(((_prHwMacRxDesc)->ucBssid & RX_STATUS_PAYLOAD_FORMAT_MASK) >>	\
	 RX_STATUS_PAYLOAD_FORMAT_OFFSET)
#define HAL_RX_STATUS_GET_BSSID(_prHwMacRxDesc)		       \
	(((_prHwMacRxDesc)->ucBssid & RX_STATUS_BSSID_MASK) >> \
	 RX_STATUS_BSSID_OFFSET)

/* DW 2 */
#define HAL_RX_STATUS_GET_WLAN_IDX( \
		_prHwMacRxDesc)    ((_prHwMacRxDesc)->ucWlanIdx)
#define HAL_RX_STATUS_GET_TID(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->ucTidSecMode & RX_STATUS_TID_MASK))
#define HAL_RX_STATUS_GET_SEC_MODE(_prHwMacRxDesc)		  \
	(((_prHwMacRxDesc)->ucTidSecMode & RX_STATUS_SEC_MASK) >> \
	 RX_STATUS_SEC_OFFSET)
#define HAL_RX_STATUS_GET_SW_BIT(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_SW_BIT) ? true : false)
#define HAL_RX_STATUS_IS_FCS_ERROR(_prHwMacRxDesc)			      \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_FCS_ERROR) ? true : \
	 false)
#define HAL_RX_STATUS_IS_CIPHER_MISMATCH(_prHwMacRxDesc)		     \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_CIPHER_MISMATCH) ? \
	 true :								     \
	 false)
#define HAL_RX_STATUS_IS_CLM_ERROR(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u2StatusFlag &	   \
	  RX_STATUS_FLAG_CIPHER_LENGTH_MISMATCH) ? \
	 true :					   \
	 false)
#define HAL_RX_STATUS_IS_ICV_ERROR(_prHwMacRxDesc)			      \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_ICV_ERROR) ? true : \
	 false)
#define HAL_RX_STATUS_IS_TKIP_MIC_ERROR(_prHwMacRxDesc)			       \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_TKIPMIC_ERROR) > 0 ? \
	 true :								       \
	 false)
#define HAL_RX_STATUS_IS_ERROR(_prHwMacRxDesc)				       \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_ERROR_MASK) ? true : \
	 false)
#define HAL_RX_STATUS_IS_LEN_MISMATCH(_prHwMacRxDesc)			  \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_LEN_MISMATCH) ? \
	 true :								  \
	 false)
#define HAL_RX_STATUS_IS_DE_AMSDU_FAIL(_prHwMacRxDesc)			   \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_DE_AMSDU_FAIL) ? \
	 true :								   \
	 false)
#define HAL_RX_STATUS_IS_EXCEED_LEN(_prHwMacRxDesc)			       \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_EXCEED_LEN) ? true : \
	 false)
#define HAL_RX_STATUS_IS_LLC_MIS(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_LLC_MIS) ? true : false)
#define HAL_RX_STATUS_IS_UDF_VLT(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_UDF_VLT) ? true : false)
#define HAL_RX_STATUS_IS_FRAG(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FRAG) ? true : false)
#define HAL_RX_STATUS_IS_NULL(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_NULL) ? true : false)
#define HAL_RX_STATUS_IS_DATA(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_DATA) ? false : true)
#define HAL_RX_STATUS_IS_AMPDU_SUB_FRAME(_prHwMacRxDesc)		\
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_AMPDU_SUB_FRAME) ?	\
	 false :							\
	 true)
#define HAL_RX_STATUS_IS_AMPDU_FORMAT(_prHwMacRxDesc)			     \
	(((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_AMPDU_FORMAT) ? false : \
	 true)

/* DW 3 */
#define HAL_RX_STATUS_IS_RV_VALID(_prHwMacRxDesc)			       \
	(((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_RXV_SEQ_NO_MASK) ? \
	 true :								       \
	 false)
#define HAL_RX_STATUS_GET_RXV_SEQ_NO(_prHwMacRxDesc) \
	((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_RXV_SEQ_NO_MASK)
#define HAL_RX_STATUS_GET_TCL(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_TCL) ? true : false)
#define HAL_RX_STATUS_IS_CLS(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_CLS) ? true : false)
#define HAL_RX_STATUS_GET_OFLD(_prHwMacRxDesc)				  \
	(((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_OFLD_MASK) >> \
	 RX_STATUS_OFLD_OFFSET)
#define HAL_RX_STATUS_IS_MGC(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_MGC) ? true : false)
#define HAL_RX_STATUS_GET_WOL(_prHwMacRxDesc)				 \
	(((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_WOL_MASK) >> \
	 RX_STATUS_WOL_OFFSET)
#define HAL_RX_STATUS_GET_CLS_BITMAP(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u4PatternFilterInfo &    \
	  RX_STATUS_CLS_BITMAP_MASK) >>		     \
	 RX_STATUS_CLS_BITMAP_OFFSET)
#define HAL_RX_STATUS_IS_PF_BLACK_LIST(_prHwMacRxDesc) \
	(((_prHwMacRxDesc)->u4PatternFilterInfo &      \
	  RX_STATUS_PF_MODE_BLACK_LIST) ?	       \
	 true :					       \
	 false)
#define HAL_RX_STATUS_IS_PF_CHECKED(_prHwMacRxDesc)			      \
	(((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_PF_STS_CHECKED) ? \
	 true :								      \
	 false)

/* DW 4~7 */
#define HAL_RX_STATUS_GET_FRAME_CTL_FIELD(_prHwMacRxStsGroup4) \
	((_prHwMacRxStsGroup4)->u2FrameCtl)
#define HAL_RX_STATUS_GET_TA(_prHwMacRxStsGroup4, pucTA)		\
	{								\
		kalMemCopy(pucTA, &(_prHwMacRxStsGroup4)->aucTA[0], 6);	\
	}
#define HAL_RX_STATUS_GET_SEQ_FRAG_NUM(_prHwMacRxStsGroup4) \
	((_prHwMacRxStsGroup4)->u2SeqFrag)
#define HAL_RX_STATUS_GET_QOS_CTL_FIELD(_prHwMacRxStsGroup4) \
	((_prHwMacRxStsGroup4)->u2Qos)

#define HAL_RX_STATUS_GET_SEQFrag_NUM(_prHwMacRxStsGroup4) \
	((_prHwMacRxStsGroup4)->u2SeqFrag)
#define HAL_RX_STATUS_GET_HTC(_prHwMacRxStsGroup4) \
	((_prHwMacRxStsGroup4)->u4HTC)

/* DW 8~11 */
#define HAL_RX_STATUS_GET_RSC(_prHwMacRxStsGroup1, pucRSC)		 \
	{								 \
		kalMemCopy(pucRSC, &(_prHwMacRxStsGroup1)->aucPN[0], 6); \
	}

#define HAL_RX_STATUS_GET_PN(_prHwMacRxStsGroup1, pucPN)		 \
	{								 \
		kalMemCopy(pucPN, &(_prHwMacRxStsGroup1)->aucPN[0], 16); \
	}

/* DW 12~13 */
#define HAL_RX_STATUS_GET_TIMESTAMP(_prHwMacRxStsGroup2, _ucIdx) \
	((_prHwMacRxStsGroup2)->u4Timestamp)
#define HAL_RX_STATUS_GET_FCS32(_prHwMacRxStsGroup2) \
	((_prHwMacRxStsGroup2)->u4CRC)

/* DW 14~19 */
#define HAL_RX_STATUS_GET_RX_VECTOR(_prHwMacRxStsGroup3, _ucIdx) \
	((_prHwMacRxStsGroup3)->u4RxVector[_ucIdx])

#define HAL_RX_STATUS_GET_RX_NUM(_prHwMacRxStsGroup3)		      \
	(((_prHwMacRxStsGroup3)->u4RxVector[0] &RX_VT_NUM_RX_MASK) >> \
	 RX_VT_NUM_RX_OFFSET)

#define HAL_RX_STATUS_GET_RCPI0(_prHwMacRxStsGroup3)		     \
	(((_prHwMacRxStsGroup3)->u4RxVector[3] &RX_VT_RCPI0_MASK) >> \
	 RX_VT_RCPI0_OFFSET)
#define HAL_RX_STATUS_GET_RCPI1(_prHwMacRxStsGroup3)		     \
	(((_prHwMacRxStsGroup3)->u4RxVector[3] &RX_VT_RCPI1_MASK) >> \
	 RX_VT_RCPI1_OFFSET)

/* TBD */
#define HAL_RX_STATUS_GET_RX_PACKET_LEN(_prHwMacRxDesc)
#define HAL_RX_STATUS_IS_MATCH_PACKET(_prHwMacRxDesc)

#define HAL_RX_STATUS_GET_CHNL_NUM(_prHwMacRxDesc)		    \
	((((_prHwMacRxDesc)->ucChanFreq) > HW_CHNL_NUM_MAX_4G_5G) ? \
	 (((_prHwMacRxDesc)->ucChanFreq) - HW_CHNL_NUM_MAX_4G_5G) : \
	 ((_prHwMacRxDesc)->ucChanFreq))

/* To do: support more bands other than 2.4G and 5G */
#define HAL_RX_STATUS_GET_RF_BAND(_prHwMacRxDesc)			      \
	((((_prHwMacRxDesc)->ucChanFreq) <= HW_CHNL_NUM_MAX_2G4) ? BAND_2G4 : \
	 BAND_5G)

/*------------------------------------------------------------------------------
 * MACRO for HW_RX_VECTOR_DESC_T
 *------------------------------------------------------------------------------
 */
#define HAL_RX_VECTOR_GET_TA(_prHwRxVector, pucTA)		  \
	{							  \
		kalMemCopy(pucTA, &(_prHwRxVector)->aucTA[0], 6); \
	}

#define HAL_RX_VECTOR_GET_SEQ_NO(_prHwRxVector)	\
	((_prHwRxVector)->ucRxVtSeqNo & RX_STATUS_RXV_SEQ_NO_MASK)
#define HAL_RX_VECTOR_IS_FOR_BA_ACK(_prHwRxVector) \
	(((_prHwRxVector)->ucRxVtSeqNo & RX_VECTOR_FOR_BA_ACK) ? true : false)
#define HAL_RX_VECTOR_GET_RX_VECTOR(_prHwRxVector, _ucIdx) \
	((_prHwRxVector)->u4RxVector[_ucIdx])

#define RXM_IS_QOS_DATA_FRAME(_u2FrameCtrl)				   \
	(((_u2FrameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_QOS_DATA) ? true : \
	 false)
#define RXM_IS_TO_DS(_u2FrameCtrl) \
	(((_u2FrameCtrl & MASK_TO_DS_FROM_DS) == MASK_FC_TO_DS) ? true : false)
#define RXM_IS_FROM_DS(_u2FrameCtrl)					   \
	(((_u2FrameCtrl & MASK_TO_DS_FROM_DS) == MASK_FC_FROM_DS) ? true : \
	 false)
#define RXM_IS_MORE_DATA(_u2FrameCtrl)					    \
	(((_u2FrameCtrl & MASK_FC_MORE_DATA) == MASK_FC_MORE_DATA) ? true : \
	 false)

/*******************************************************************************
 *                   F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

void nicRxInitialize(IN P_ADAPTER_T prAdapter);

void nicRxUninitialize(IN P_ADAPTER_T prAdapter);

void nicRxProcessRFBs(IN P_ADAPTER_T prAdapter);

void nicRxProcessMsduReport(IN P_ADAPTER_T prAdapter,IN OUT P_SW_RFB_T prSwRfb);

WLAN_STATUS nicRxSetupRFB(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prRfb);

void nicRxReturnRFB(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prRfb);

void nicProcessRxInterrupt(IN P_ADAPTER_T prAdapter);

void nicRxProcessPktWithoutReorder(IN P_ADAPTER_T prAdapter,
				   IN P_SW_RFB_T prSwRfb);

void nicRxProcessForwardPkt(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

void nicRxProcessGOBroadcastPkt(IN P_ADAPTER_T prAdapter,IN P_SW_RFB_T prSwRfb);

u8 nicRxFillRFB(IN P_ADAPTER_T prAdapter, IN OUT P_SW_RFB_T prSwRfb);

void nicRxClearFrag(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

P_SW_RFB_T incRxDefragMPDU(IN P_ADAPTER_T prAdapter,
			   IN P_SW_RFB_T prSWRfb,
			   OUT P_QUE_T prReturnedQue);

u8 nicRxIsDuplicateFrame(IN OUT P_SW_RFB_T prSwRfb);

#if CFG_SUPPORT_SNIFFER
void nicRxProcessMonitorPacket(IN P_ADAPTER_T prAdapter,
			       IN OUT P_SW_RFB_T prSwRfb);
#endif

void nicRxProcessDataPacket(IN P_ADAPTER_T prAdapter,IN OUT P_SW_RFB_T prSwRfb);

void nicRxProcessEventPacket(IN P_ADAPTER_T prAdapter,
			     IN OUT P_SW_RFB_T prSwRfb);

void nicRxProcessMgmtPacket(IN P_ADAPTER_T prAdapter,IN OUT P_SW_RFB_T prSwRfb);

#if CFG_TCP_IP_CHKSUM_OFFLOAD
void nicRxFillChksumStatus(IN P_ADAPTER_T prAdapter,
			   IN OUT P_SW_RFB_T prSwRfb,
			   IN u32 u4TcpUdpIpCksStatus);

void nicRxUpdateCSUMStatistics(IN P_ADAPTER_T prAdapter,
			       IN const ENUM_CSUM_RESULT_T aeCSUM[]);
#endif

void nicRxQueryStatus(IN P_ADAPTER_T prAdapter,
		      IN u8 *pucBuffer,
		      OUT u32 *pu4Count);

void nicRxClearStatistics(IN P_ADAPTER_T prAdapter);

void nicRxQueryStatistics(IN P_ADAPTER_T prAdapter,
			  IN u8 *pucBuffer,
			  OUT u32 *pu4Count);

WLAN_STATUS
nicRxWaitResponse(IN P_ADAPTER_T prAdapter,
		  IN u8 ucPortIdx,
		  OUT u8 *pucRspBuffer,
		  IN u32 u4MaxRespBufferLen,
		  OUT u32 *pu4Length);

void nicRxEnablePromiscuousMode(IN P_ADAPTER_T prAdapter);

void nicRxDisablePromiscuousMode(IN P_ADAPTER_T prAdapter);

WLAN_STATUS nicRxFlush(IN P_ADAPTER_T prAdapter);

WLAN_STATUS nicRxProcessActionFrame(IN P_ADAPTER_T prAdapter,
				    IN P_SW_RFB_T prSwRfb);

u8 nicRxGetRcpiValueFromRxv(IN u8 ucRcpiMode, IN P_SW_RFB_T prSwRfb);

#endif
