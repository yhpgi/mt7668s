/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   p2p_typedef.h
 *    \brief  Declaration of data type and return values of internal protocol
 * stack.
 *
 *    In this file we declare the data type and return values which will be
 * exported to all MGMT Protocol Stack.
 */

#ifndef _P2P_TYPEDEF_H
#define _P2P_TYPEDEF_H

#if CFG_ENABLE_WIFI_DIRECT

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

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

/*
 * type definition of pointer to p2p structure
 */
/* typedef struct _GL_P2P_INFO_T   GL_P2P_INFO_T, *P_GL_P2P_INFO_T; */
typedef struct _P2P_INFO_T P2P_INFO_T, *P_P2P_INFO_T;

typedef struct _P2P_FSM_INFO_T P2P_FSM_INFO_T, *P_P2P_FSM_INFO_T;

typedef struct _P2P_DEV_FSM_INFO_T P2P_DEV_FSM_INFO_T, *P_P2P_DEV_FSM_INFO_T;

typedef struct _P2P_ROLE_FSM_INFO_T P2P_ROLE_FSM_INFO_T, *P_P2P_ROLE_FSM_INFO_T;

typedef struct _P2P_CONNECTION_SETTINGS_T P2P_CONNECTION_SETTINGS_T,
	*P_P2P_CONNECTION_SETTINGS_T;

/* Type definition for function pointer to p2p function*/
typedef u8 (*P2P_LAUNCH)(P_GLUE_INFO_T prGlueInfo);

typedef u8 (*P2P_REMOVE)(P_GLUE_INFO_T prGlueInfo, u8 fgIsWlanLaunched);

typedef u8 (*KAL_P2P_GET_CIPHER)(IN P_GLUE_INFO_T prGlueInfo);

typedef u8 (*KAL_P2P_GET_TKIP_CIPHER)(IN P_GLUE_INFO_T prGlueInfo);

typedef u8 (*KAL_P2P_GET_CCMP_CIPHER)(IN P_GLUE_INFO_T prGlueInfo);

typedef u8 (*KAL_P2P_GET_WSC_MODE)(IN P_GLUE_INFO_T prGlueInfo);

typedef struct net_device *(*KAL_P2P_GET_DEV_HDLR)(P_GLUE_INFO_T prGlueInfo);

typedef void (*KAL_P2P_SET_MULTICAST_WORK_ITEM)(P_GLUE_INFO_T prGlueInfo);

typedef void (*P2P_NET_REGISTER)(P_GLUE_INFO_T prGlueInfo);

typedef void (*P2P_NET_UNREGISTER)(P_GLUE_INFO_T prGlueInfo);

typedef void (*KAL_P2P_UPDATE_ASSOC_INFO)(IN P_GLUE_INFO_T prGlueInfo,
					  IN u8 *pucFrameBody,
					  IN u32 u4FrameBodyLen,
					  IN u8 fgReassocRequest);

typedef u8 (*P2P_VALIDATE_AUTH)(IN P_ADAPTER_T prAdapter,
				IN P_SW_RFB_T prSwRfb,
				IN PP_STA_RECORD_T pprStaRec,
				OUT u16 *pu2StatusCode);

typedef u8 (*P2P_VALIDATE_ASSOC_REQ)(IN P_ADAPTER_T prAdapter,
				     IN P_SW_RFB_T prSwRfb,
				     OUT u16 *pu4ControlFlags);

typedef void (*P2P_RUN_EVENT_AAA_TX_FAIL)(IN P_ADAPTER_T prAdapter,
					  IN P_STA_RECORD_T prStaRec);

typedef u8 (*P2P_PARSE_CHECK_FOR_P2P_INFO_ELEM)(IN P_ADAPTER_T prAdapter,
						IN u8 *pucBuf,
						OUT u8 *pucOuiType);

typedef WLAN_STATUS (*P2P_RUN_EVENT_AAA_COMPLETE)(IN P_ADAPTER_T prAdapter,
						  IN P_STA_RECORD_T prStaRec);

typedef void (*P2P_PROCESS_EVENT_UPDATE_NOA_PARAM)(IN P_ADAPTER_T prAdapter,
						   u8 ucNetTypeIndex,
						   P_EVENT_UPDATE_NOA_PARAMS_T
						   prEventUpdateNoaParam);

typedef void (*SCAN_P2P_PROCESS_BEACON_AND_PROBE_RESP)(IN P_ADAPTER_T prAdapter,
						       IN P_SW_RFB_T prSwRfb,
						       IN P_WLAN_STATUS prStatus,
						       IN P_BSS_DESC_T prBssDesc,
						       IN P_WLAN_BEACON_FRAME_T
						       prWlanBeaconFrame);

typedef void (*P2P_RX_PUBLIC_ACTION_FRAME)(P_ADAPTER_T prAdapter,
					   IN P_SW_RFB_T prSwRfb);

typedef void (*RLM_RSP_GENERATE_OBSS_SCAN_IE)(P_ADAPTER_T prAdapter,
					      P_MSDU_INFO_T prMsduInfo);

typedef void (*RLM_UPDATE_BW_BY_CH_LIST_FOR_AP)(P_ADAPTER_T prAdapter,
						P_BSS_INFO_T prBssInfo);

typedef void (*RLM_PROCESS_PUBLIC_ACTION)(P_ADAPTER_T prAdapter,
					  P_SW_RFB_T prSwRfb);

typedef void (*RLM_PROCESS_HT_ACTION)(P_ADAPTER_T prAdapter,P_SW_RFB_T prSwRfb);

typedef void (*RLM_UPDATE_PARAMS_FOR_AP)(P_ADAPTER_T prAdapter,
					 P_BSS_INFO_T prBssInfo,
					 u8 fgUpdateBeacon);

typedef void (*RLM_HANDLE_OBSS_STATUS_EVENT_PKT)(P_ADAPTER_T prAdapter,
						 P_EVENT_AP_OBSS_STATUS_T
						 prObssStatus);

typedef u8 (*P2P_FUNC_VALIDATE_PROBE_REQ)(IN P_ADAPTER_T prAdapter,
					  IN P_SW_RFB_T prSwRfb,
					  OUT u32 *pu4ControlFlags);

typedef void (*RLM_BSS_INIT_FOR_AP)(P_ADAPTER_T prAdapter,
				    P_BSS_INFO_T prBssInfo);

typedef u32 (*P2P_GET_PROB_RSP_IE_TABLE_SIZE)(void);

typedef u8 *(*P2P_BUILD_REASSOC_REQ_FRAME_COMMON_IES)(IN P_ADAPTER_T prAdapter,
						      IN P_MSDU_INFO_T
						      prMsduInfo,
						      IN u8 *pucBuffer);

typedef void (*P2P_FUNC_DISCONNECT)(IN P_ADAPTER_T prAdapter,
				    IN P_STA_RECORD_T prStaRec,
				    IN u8 fgSendDeauth,
				    IN u16 u2ReasonCode);

typedef void (*P2P_FSM_RUN_EVENT_RX_DEAUTH)(IN P_ADAPTER_T prAdapter,
					    IN P_STA_RECORD_T prStaRec,
					    IN P_SW_RFB_T prSwRfb);

typedef void (*P2P_FSM_RUN_EVENT_RX_DISASSOC)(IN P_ADAPTER_T prAdapter,
					      IN P_STA_RECORD_T prStaRec,
					      IN P_SW_RFB_T prSwRfb);

typedef u8 (*P2P_FUN_IS_AP_MODE)(IN P_P2P_FSM_INFO_T prP2pFsmInfo);

typedef void (*P2P_FSM_RUN_EVENT_BEACON_TIMEOUT)(IN P_ADAPTER_T prAdapter);

typedef void (*P2P_FUNC_STORE_ASSOC_RSP_IE_BUFFER)(IN P_ADAPTER_T prAdapter,
						   IN P_SW_RFB_T prSwRfb);

typedef void (*P2P_GENERATE_P2P_IE)(IN P_ADAPTER_T prAdapter,
				    IN P_MSDU_INFO_T prMsduInfo);

typedef u32 (*P2P_CALCULATE_P2P_IE_LEN)(IN P_ADAPTER_T prAdapter,
					IN u8 ucBssIndex,
					IN P_STA_RECORD_T prStaRec);

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

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */

#endif

#endif
