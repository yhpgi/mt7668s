/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
/*
** Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/wnm.c#1
*/

/*! \file   "wnm.c"
*    \brief  This file includes the 802.11v default vale and functions.
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

#define WNM_MAX_TOD_ERROR  0
#define WNM_MAX_TOA_ERROR  0
#define MICRO_TO_10NANO(x) ((x)*100)
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
#if CFG_SUPPORT_802_11V_TIMING_MEASUREMENT
static UINT_8 ucTimingMeasToken;
#endif
#if CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT
static UINT_8 ucBtmMgtToken = 1;
#endif

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

/*----------------------------------------------------------------------------*/
/*!
*
* \brief This routine is called to process the 802.11v wnm category action
* frame.
*
*
* \note
*      Called by: Handle Rx mgmt request
*/
/*----------------------------------------------------------------------------*/
#if CFG_SUPPORT_802_11V
VOID wnmWNMAction(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_WLAN_ACTION_FRAME prRxFrame;

	ASSERT(prAdapter);
	ASSERT(prSwRfb);

	prRxFrame = (P_WLAN_ACTION_FRAME)prSwRfb->pvHeader;

	DBGLOG(WNM, TRACE, "WNM action frame: %d from " MACSTR "\n",
	       prRxFrame->ucAction, MAC2STR(prRxFrame->aucSrcAddr));

	switch (prRxFrame->ucAction) {
#if CFG_SUPPORT_802_11V_TIMING_MEASUREMENT
	case ACTION_WNM_TIMING_MEASUREMENT_REQUEST:
		break;
#endif
#if CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT
	case ACTION_WNM_BSS_TRANSITION_MANAGEMENT_REQ:
#endif
	default:
		DBGLOG(WNM, INFO,
		       "WNM: action frame %d, try to send to supplicant\n",
		       prRxFrame->ucAction);
		aisFuncValidateRxActionFrame(prAdapter, prSwRfb);
		break;
	}
}

UINT_8 wnmGetBtmToken(void)
{
	return ucBtmMgtToken++;
}

static WLAN_STATUS wnmBTMQueryTxDone(IN P_ADAPTER_T prAdapter,
			IN P_MSDU_INFO_T prMsduInfo,
			IN ENUM_TX_RESULT_CODE_T rTxDoneStatus)
{
	DBGLOG(WNM, INFO, "BTM: Query Frame Tx Done, Status %d\n",
	       rTxDoneStatus);
	return WLAN_STATUS_SUCCESS;
}

static WLAN_STATUS wnmBTMResponseTxDone(IN P_ADAPTER_T prAdapter,
			IN P_MSDU_INFO_T prMsduInfo,
			IN ENUM_TX_RESULT_CODE_T rTxDoneStatus)
{
	P_BSS_TRANSITION_MGT_PARAM_T prBtm =
		&prAdapter->rWifiVar.rAisSpecificBssInfo.rBTMParam;
	P_AIS_FSM_INFO_T prAisFsmInfo = &prAdapter->rWifiVar.rAisFsmInfo;

	DBGLOG(WNM, INFO, "BTM: Response Frame Tx Done Status %d\n",
	       rTxDoneStatus);
	if (prBtm->fgPendingResponse &&
	    prAisFsmInfo->eCurrentState == AIS_STATE_SEARCH) {
		prBtm->fgPendingResponse = FALSE;
		aisFsmSteps(prAdapter, AIS_STATE_REQ_CHANNEL_JOIN);
	}
	return WLAN_STATUS_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will compose the BTM Response frame.
*
* @param[in] prAdapter              Pointer to the Adapter structure.
* @param[in] prStaRec               Pointer to the STA_RECORD_T.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID wnmSendBTMResponseFrame(IN P_ADAPTER_T prAdapter,
			IN P_STA_RECORD_T prStaRec)
{
	P_MSDU_INFO_T prMsduInfo = NULL;
	P_BSS_INFO_T prBssInfo = NULL;
	P_ACTION_BTM_RSP_FRAME_T prTxFrame = NULL;
	UINT_16 u2PayloadLen = 0;
	P_BSS_TRANSITION_MGT_PARAM_T prBtmParam =
	    &prAdapter->rWifiVar.rAisSpecificBssInfo.rBTMParam;
	PUINT_8 pucOptInfo = NULL;

	if (!prStaRec) {
		DBGLOG(WNM, ERROR, "BTM: No station record found\n");
		return;
	}

	prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prStaRec->ucBssIndex);
	ASSERT(prBssInfo);

	/* 1 Allocate MSDU Info */
	prMsduInfo = (P_MSDU_INFO_T)cnmMgtPktAlloc(
		prAdapter, MAC_TX_RESERVED_FIELD + PUBLIC_ACTION_MAX_LEN);
	if (!prMsduInfo)
		return;
	prTxFrame = (P_ACTION_BTM_RSP_FRAME_T)((unsigned long)(prMsduInfo->prPacket) +
				MAC_TX_RESERVED_FIELD);

	/* 2 Compose The Mac Header. */
	prTxFrame->u2FrameCtrl = MAC_FRAME_ACTION;

	COPY_MAC_ADDR(prTxFrame->aucDestAddr, prStaRec->aucMacAddr);
	COPY_MAC_ADDR(prTxFrame->aucSrcAddr, prBssInfo->aucOwnMacAddr);
	COPY_MAC_ADDR(prTxFrame->aucBSSID, prBssInfo->aucBSSID);

	prTxFrame->ucCategory = CATEGORY_WNM_ACTION;
	prTxFrame->ucAction = ACTION_WNM_BSS_TRANSITION_MANAGEMENT_RSP;

	/* 3 Compose the frame body's frame. */
	prTxFrame->ucDialogToken = prBtmParam->ucDialogToken;
	prBtmParam->ucDialogToken = 0;	/* reset dialog token */
	prTxFrame->ucStatusCode = prBtmParam->ucStatusCode;
	prTxFrame->ucBssTermDelay = prBtmParam->ucTermDelay;
	pucOptInfo = &prTxFrame->aucOptInfo[0];
	if (prBtmParam->ucStatusCode == BSS_TRANSITION_MGT_STATUS_ACCEPT) {
		COPY_MAC_ADDR(pucOptInfo, prBtmParam->aucTargetBssid);
		pucOptInfo += MAC_ADDR_LEN;
		u2PayloadLen += MAC_ADDR_LEN;
	}
	if (prBtmParam->u2OurNeighborBssLen > 0) {
		kalMemCopy(pucOptInfo, prBtmParam->pucOurNeighborBss,
			   prBtmParam->u2OurNeighborBssLen);
		kalMemFree(prBtmParam->pucOurNeighborBss, VIR_MEM_TYPE,
			   prBtmParam->u2OurNeighborBssLen);
		prBtmParam->u2OurNeighborBssLen = 0;
		u2PayloadLen += prBtmParam->u2OurNeighborBssLen;
	}

	/* 4 Update information of MSDU_INFO_T */
	TX_SET_MMPDU(prAdapter, prMsduInfo, prStaRec->ucBssIndex,
		     prStaRec->ucIndex, WLAN_MAC_MGMT_HEADER_LEN,
			OFFSET_OF(ACTION_BTM_RSP_FRAME_T, aucOptInfo) +
				u2PayloadLen,
			wnmBTMResponseTxDone, MSDU_RATE_MODE_AUTO);

	/* 5 Enqueue the frame to send this action frame. */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);
}				/* end of wnmComposeBTMResponseFrame() */

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will compose the BTM Query frame.
*
* @param[in] prAdapter              Pointer to the Adapter structure.
* @param[in] prStaRec               Pointer to the STA_RECORD_T.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID wnmSendBTMQueryFrame(IN P_ADAPTER_T prAdapter,
			IN P_STA_RECORD_T prStaRec)
{
	P_MSDU_INFO_T prMsduInfo = NULL;
	P_BSS_INFO_T prBssInfo = NULL;
	P_ACTION_BTM_QUERY_FRAME_T prTxFrame = NULL;
	P_BSS_TRANSITION_MGT_PARAM_T prBtmParam =
	    &prAdapter->rWifiVar.rAisSpecificBssInfo.rBTMParam;
	UINT_16 u2FrameLen = 0;

	prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prStaRec->ucBssIndex);
	ASSERT(prBssInfo);

	/* 1 Allocate MSDU Info */
	prMsduInfo = cnmMgtPktAlloc(prAdapter, MAC_TX_RESERVED_FIELD + PUBLIC_ACTION_MAX_LEN);
	if (!prMsduInfo)
		return;
	prTxFrame = (P_ACTION_BTM_QUERY_FRAME_T)((unsigned long)(prMsduInfo->prPacket) +
				MAC_TX_RESERVED_FIELD);

	/* 2 Compose The Mac Header. */
	prTxFrame->u2FrameCtrl = MAC_FRAME_ACTION;
	COPY_MAC_ADDR(prTxFrame->aucDestAddr, prStaRec->aucMacAddr);
	COPY_MAC_ADDR(prTxFrame->aucSrcAddr, prBssInfo->aucOwnMacAddr);
	COPY_MAC_ADDR(prTxFrame->aucBSSID, prBssInfo->aucBSSID);
	prTxFrame->ucCategory = CATEGORY_WNM_ACTION;
	prTxFrame->ucAction = ACTION_WNM_BSS_TRANSITION_MANAGEMENT_QUERY;

	u2FrameLen = OFFSET_OF(ACTION_NEIGHBOR_REPORT_FRAME_T, aucInfoElem);

	/* 3 Compose the frame body's frame. */
	prTxFrame->ucDialogToken = prBtmParam->ucDialogToken;
	prTxFrame->ucQueryReason = prBtmParam->ucQueryReason;

	if (prBtmParam->u2OurNeighborBssLen > 0) {
		kalMemCopy(prTxFrame->pucNeighborBss,
			   prBtmParam->pucOurNeighborBss,
			   prBtmParam->u2OurNeighborBssLen);
		kalMemFree(prBtmParam->pucOurNeighborBss, VIR_MEM_TYPE,
			   prBtmParam->u2OurNeighborBssLen);
		prBtmParam->u2OurNeighborBssLen = 0;
	}

	/* 4 Update information of MSDU_INFO_T */
	TX_SET_MMPDU(prAdapter, prMsduInfo, prStaRec->ucBssIndex,
		     prStaRec->ucIndex, WLAN_MAC_MGMT_HEADER_LEN,
		     WLAN_MAC_MGMT_HEADER_LEN + 4 +
			     prBtmParam->u2OurNeighborBssLen,
		wnmBTMQueryTxDone, MSDU_RATE_MODE_AUTO);

	/* 5 Enqueue the frame to send this action frame. */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);
}				/* end of wnmComposeBTMQueryFrame() */
/*----------------------------------------------------------------------------*/
/*!
*
* \brief This routine is called to process the 802.11v BTM request.
*
*
* \note
*      Handle Rx mgmt request
*/
/*----------------------------------------------------------------------------*/
#if CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT
VOID wnmRecvBTMRequest(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_ACTION_BTM_REQ_FRAME_T prRxFrame = NULL;
	P_BSS_TRANSITION_MGT_PARAM_T prBtmParam =
	    &prAdapter->rWifiVar.rAisSpecificBssInfo.rBTMParam;
	PUINT_8 pucOptInfo = NULL;
	UINT_8 ucRequestMode = 0;
	UINT_16 u2TmpLen = 0;
	P_MSG_AIS_BSS_TRANSITION_T prMsg = NULL;
	enum WNM_AIS_BSS_TRANSITION eTransType = BSS_TRANSITION_NO_MORE_ACTION;

	prRxFrame = (P_ACTION_BTM_REQ_FRAME_T) prSwRfb->pvHeader;
	if (!prRxFrame)
		return;
	if (prSwRfb->u2PacketLen <
		OFFSET_OF(ACTION_BTM_REQ_FRAME_T, aucOptInfo)) {
		DBGLOG(WNM, WARN,
		       "BTM: Request frame length is less than a standard BTM frame\n");
		return;
	}
	prMsg = (P_MSG_AIS_BSS_TRANSITION_T)cnmMemAlloc(
		prAdapter, RAM_TYPE_MSG,
				sizeof(MSG_AIS_BSS_TRANSITION_T));
	if (!prMsg) {
		DBGLOG(WNM, WARN, "BTM: Msg Hdr is NULL\n");
		return;
	}
	kalMemZero(prMsg, sizeof(*prMsg));
	prBtmParam->ucRequestMode = prRxFrame->ucRequestMode;
	prMsg->ucToken = prRxFrame->ucDialogToken;
	prBtmParam->u2DisassocTimer = prRxFrame->u2DisassocTimer;
	prBtmParam->ucDialogToken = prRxFrame->ucDialogToken;
	pucOptInfo = &prRxFrame->aucOptInfo[0];
	ucRequestMode = prBtmParam->ucRequestMode;
	u2TmpLen = OFFSET_OF(ACTION_BTM_REQ_FRAME_T, aucOptInfo);
	if (ucRequestMode & BTM_REQ_MODE_DISC_IMM)
		eTransType = BSS_TRANSITION_REQ_ROAMING;
	if (ucRequestMode & BTM_REQ_MODE_BSS_TERM_INCLUDE) {
		P_SUB_IE_BSS_TERM_DURATION_T prBssTermDuration =
			(P_SUB_IE_BSS_TERM_DURATION_T)pucOptInfo;

		prBtmParam->u2TermDuration = prBssTermDuration->u2Duration;
		kalMemCopy(prBtmParam->aucTermTsf,
			   prBssTermDuration->aucTermTsf, 8);
		pucOptInfo += sizeof(*prBssTermDuration);
		u2TmpLen += sizeof(*prBssTermDuration);
		eTransType = BSS_TRANSITION_REQ_ROAMING;
	}
	if (ucRequestMode & BTM_REQ_MODE_ESS_DISC_IMM) {
		kalMemCopy(prBtmParam->aucSessionURL, &pucOptInfo[1],
			   pucOptInfo[0]);
		prBtmParam->ucSessionURLLen = pucOptInfo[0];
		u2TmpLen += pucOptInfo[0];
		pucOptInfo += pucOptInfo[0] + 1;
		eTransType = BSS_TRANSITION_DISASSOC;
	}
	if (ucRequestMode & BTM_REQ_MODE_CAND_INCLUDED_BIT) {
		if (!(ucRequestMode & BTM_REQ_MODE_ESS_DISC_IMM))
			eTransType = BSS_TRANSITION_REQ_ROAMING;
		if (prSwRfb->u2PacketLen > u2TmpLen) {
			prMsg->u2CandListLen = prSwRfb->u2PacketLen - u2TmpLen;
			prMsg->pucCandList = pucOptInfo;
			prMsg->ucValidityInterval =
				prRxFrame->ucValidityInterval;
		} else
			DBGLOG(WNM, WARN,
			       "BTM: Candidate Include bit is set, but no candidate list\n");
	}

	DBGLOG(WNM, INFO,
	       "BTM: Req %d, VInt %d, DiscTimer %d, Token %d, TransType %d\n",
	       prBtmParam->ucRequestMode, prRxFrame->ucValidityInterval,
	       prBtmParam->u2DisassocTimer, prMsg->ucToken, eTransType);

	prMsg->eTransitionType = eTransType;
	prMsg->rMsgHdr.eMsgId = MID_WNM_AIS_BSS_TRANSITION;
	/* if BTM Request is dest for broadcast, don't send BTM Response */
	if (kalMemCmp(prRxFrame->aucDestAddr, "\xff\xff\xff\xff\xff\xff",
		      MAC_ADDR_LEN))
		prMsg->fgNeedResponse = TRUE;
	else
		prMsg->fgNeedResponse = FALSE;
	mboxSendMsg(prAdapter, MBOX_ID_0, (P_MSG_HDR_T)prMsg,
		    MSG_SEND_METHOD_BUF);
}
#endif /* #if CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT */
#endif /* #if CFG_SUPPORT_802_11V */