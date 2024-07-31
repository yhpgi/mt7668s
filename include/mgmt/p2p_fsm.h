/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   p2p_fsm.h
 *  \brief  Declaration of functions and finite state machine for P2P Module.
 *
 *  Declaration of functions and finite state machine for P2P Module.
 */

#ifndef _P2P_FSM_H
#define _P2P_FSM_H

VOID p2pFsmRunEventScanRequest(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventChGrant(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventNetDeviceRegister(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventUpdateMgmtFrame(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

#if CFG_SUPPORT_WFD
VOID p2pFsmRunEventWfdSettingUpdate(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);
#endif

VOID p2pFsmRunEventScanDone(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

#endif /* _P2P_FSM_H */
