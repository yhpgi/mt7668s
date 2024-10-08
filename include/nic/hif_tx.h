/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

#ifndef _HIF_TX_H
#define _HIF_TX_H

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

/* Maximum buffer size for individual HIF TCQ Buffer */
#define HIF_TX_BUFF_MAX_SIZE		     1552 /* Reserved field was not included */

#define TX_HDR_SIZE			     sizeof(HIF_TX_HEADER_T)

#define CMD_HDR_SIZE			     sizeof(WIFI_CMD_T)

#if CFG_MESON_G12A_PATCH
#define CMD_PKT_SIZE_FOR_IMAGE		     1408
#else
#define CMD_PKT_SIZE_FOR_IMAGE		     2048 /* !< 2048 Bytes CMD payload buffer */
#endif

/*! NIC_HIF_TX_HEADER_T (for short-header format) */
/* DW 0, Byte 0,1 */
#define HIF_TX_HDR_TX_BYTE_COUNT_MASK	     BITS(0, 15)

/* DW 0, Byte 2 */
#define HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK    BITS(0, 6)
#define HIF_TX_HDR_IP_CSUM		     BIT(7)

/* DW 0, Byte 3 */
#define HIF_TX_HDR_TCP_CSUM		     BIT(0)
#define HIF_TX_HDR_QUEUE_IDX_MASK	     BITS(3, 6)
#define HIF_TX_HDR_QUEUE_IDX_OFFSET	     3
#define HIF_TX_HDR_PORT_IDX_MASK	     BIT(7)
#define HIF_TX_HDR_PORT_IDX_OFFSET	     7

/*******************************************************************************
 *                         D A T A   T Y P E S
 *******************************************************************************
 */

typedef struct _HIF_HW_TX_HEADER_T {
	u16 u2TxByteCount;
	u8 ucEtherTypeOffset;
	u8 ucCSflags;
	u8 aucBuffer[0];
} HIF_HW_TX_HEADER_T, *P_HIF_HW_TX_HEADER_T;

typedef struct _HIF_TX_HEADER_T {
	u16 u2TxByteCount_UserPriority;
	u8 ucEtherTypeOffset;
	u8 ucResource_PktType_CSflags;
	u8 ucWlanHeaderLength;
	u8 ucPktFormtId_Flags;
	u16 u2LLH; /* for BOW */
	u16 u2SeqNo; /* for BOW */
	u8 ucStaRecIdx;
	u8 ucForwardingType_SessionID_Reserved;
	u8 ucPacketSeqNo;
	u8 ucAck_BIP_BasicRate;
	u8 aucReserved[2];
} HIF_TX_HEADER_T, *P_HIF_TX_HEADER_T;

typedef enum _ENUM_HIF_OOB_CTRL_PKT_TYPE_T {
	HIF_OOB_CTRL_PKT_TYPE_LOOPBACK = 1,
	HIF_OOB_CTRL_PKT_TYP_NUM
} ENUM_HIF_OOB_CTRL_PKT_TYPE_T,
*P_ENUM_HIF_OOB_CTRL_PKT_TYPE_T;

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */

#define TFCB_FRAME_PAD_TO_DW(u2Length)    ALIGN_4(u2Length)

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

/* Kevin: we don't have to call following function to inspect the data
 * structure. It will check automatically while at compile time.
 */
static __KAL_INLINE__ void hif_txDataTypeCheck(void);

static __KAL_INLINE__ void hif_txDataTypeCheck(void)
{
	DATA_STRUCT_INSPECTING_ASSERT(sizeof(HIF_TX_HEADER_T) == 16);
}

#endif
