/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */
#ifndef _P2P_ROLE_STATE_H
#define _P2P_ROLE_STATE_H

void p2pRoleStateInit_IDLE(IN P_ADAPTER_T prAdapter,
			   IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo,
			   IN P_BSS_INFO_T prP2pBssInfo);

void p2pRoleStateAbort_IDLE(IN P_ADAPTER_T prAdapter,
			    IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo,
			    IN P_P2P_CHNL_REQ_INFO_T prP2pChnlReqInfo);

void p2pRoleStateInit_SCAN(IN P_ADAPTER_T prAdapter,
			   IN u8 ucBssIndex,
			   IN P_P2P_SCAN_REQ_INFO_T prScanReqInfo);

void p2pRoleStateAbort_SCAN(IN P_ADAPTER_T prAdapter,
			    IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo);

void p2pRoleStateInit_REQING_CHANNEL(IN P_ADAPTER_T prAdapter,
				     IN u8 ucBssIdx,
				     IN P_P2P_CHNL_REQ_INFO_T prChnlReqInfo);

void p2pRoleStateAbort_REQING_CHANNEL(IN P_ADAPTER_T prAdapter,
				      IN P_BSS_INFO_T prP2pRoleBssInfo,
				      IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo,
				      IN ENUM_P2P_ROLE_STATE_T eNextState);

void p2pRoleStateInit_AP_CHNL_DETECTION(IN P_ADAPTER_T prAdapter,
					IN u8 ucBssIndex,
					IN P_P2P_SCAN_REQ_INFO_T prScanReqInfo,
					IN P_P2P_CONNECTION_REQ_INFO_T
					prConnReqInfo);

void p2pRoleStateAbort_AP_CHNL_DETECTION(IN P_ADAPTER_T prAdapter,
					 IN u8 ucBssIndex,
					 IN P_P2P_CONNECTION_REQ_INFO_T
					 prP2pConnReqInfo,
					 IN P_P2P_CHNL_REQ_INFO_T prChnlReqInfo,
					 IN P_P2P_SCAN_REQ_INFO_T
					 prP2pScanReqInfo,
					 IN ENUM_P2P_ROLE_STATE_T eNextState);

void p2pRoleStateInit_GC_JOIN(IN P_ADAPTER_T prAdapter,
			      IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo,
			      IN P_P2P_CHNL_REQ_INFO_T prChnlReqInfo);

void p2pRoleStateAbort_GC_JOIN(IN P_ADAPTER_T prAdapter,
			       IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo,
			       IN P_P2P_JOIN_INFO_T prJoinInfo,
			       IN ENUM_P2P_ROLE_STATE_T eNextState);

#if (CFG_SUPPORT_DFS_MASTER == 1)
void p2pRoleStateInit_DFS_CAC(IN P_ADAPTER_T prAdapter,
			      IN u8 ucBssIdx,
			      IN P_P2P_CHNL_REQ_INFO_T prChnlReqInfo);

void p2pRoleStateAbort_DFS_CAC(IN P_ADAPTER_T prAdapter,
			       IN P_BSS_INFO_T prP2pRoleBssInfo,
			       IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo,
			       IN ENUM_P2P_ROLE_STATE_T eNextState);

void p2pRoleStateInit_SWITCH_CHANNEL(IN P_ADAPTER_T prAdapter,
				     IN u8 ucBssIdx,
				     IN P_P2P_CHNL_REQ_INFO_T prChnlReqInfo);

void p2pRoleStateAbort_SWITCH_CHANNEL(IN P_ADAPTER_T prAdapter,
				      IN P_BSS_INFO_T prP2pRoleBssInfo,
				      IN P_P2P_ROLE_FSM_INFO_T prP2pRoleFsmInfo,
				      IN ENUM_P2P_ROLE_STATE_T eNextState);

void p2pRoleStatePrepare_To_DFS_CAC_STATE(IN P_ADAPTER_T prAdapter,
					  IN P_BSS_INFO_T prBssInfo,
					  IN ENUM_CHANNEL_WIDTH_T rChannelWidth,
					  IN P_P2P_CONNECTION_REQ_INFO_T
					  prConnReqInfo,
					  OUT P_P2P_CHNL_REQ_INFO_T
					  prChnlReqInfo);

#endif

void p2pRoleStatePrepare_To_REQING_CHANNEL_STATE(IN P_ADAPTER_T prAdapter,
						 IN P_BSS_INFO_T prBssInfo,
						 IN P_P2P_CONNECTION_REQ_INFO_T
						 prConnReqInfo,
						 OUT P_P2P_CHNL_REQ_INFO_T
						 prChnlReqInfo);

#endif
