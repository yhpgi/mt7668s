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
** Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/mgmt/wnm.h#1
*/

/*! \file  wnm.h
*    \brief This file contains the IEEE 802.11 family related 802.11v network management
*	   for MediaTek 802.11 Wireless LAN Adapters.
*/


#ifndef _WNM_H
#define _WNM_H

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
#define BTM_REQ_MODE_CAND_INCLUDED_BIT                  BIT(0)
#define BTM_REQ_MODE_ABRIDGED                           BIT(1)
#define BTM_REQ_MODE_DISC_IMM                           BIT(2)
#define BTM_REQ_MODE_BSS_TERM_INCLUDE                   BIT(3)
#define BTM_REQ_MODE_ESS_DISC_IMM                       BIT(4)

#define BSS_TRANSITION_MGT_STATUS_ACCEPT                0
#define BSS_TRANSITION_MGT_STATUS_UNSPECIFIED           1
#define BSS_TRANSITION_MGT_STATUS_NEED_SCAN             2
#define BSS_TRANSITION_MGT_STATUS_CAND_NO_CAPACITY      3
#define BSS_TRANSITION_MGT_STATUS_TERM_UNDESIRED        4
#define BSS_TRANSITION_MGT_STATUS_TERM_DELAY_REQUESTED  5
#define BSS_TRANSITION_MGT_STATUS_CAND_LIST_PROVIDED    6
#define BSS_TRANSITION_MGT_STATUS_CAND_NO_CANDIDATES    7
#define BSS_TRANSITION_MGT_STATUS_LEAVING_ESS           8

/* 802.11v: define Transtion and Transition Query reasons */
#define BSS_TRANSITION_BETTER_AP_FOUND                  6
#define BSS_TRANSITION_LOW_RSSI                         16
#define BSS_TRANSITION_INCLUDE_PREFER_CAND_LIST         19
#define BSS_TRANSITION_LEAVING_ESS                      20

/*******************************************************************************
*                         D A T A   T Y P E S
********************************************************************************
*/

typedef struct _TIMINGMSMT_PARAM_T {
	BOOLEAN fgInitiator;
	UINT_8 ucTrigger;
	UINT_8 ucDialogToken;	/* Dialog Token */
	UINT_8 ucFollowUpDialogToken;	/* Follow Up Dialog Token */
	UINT_32 u4ToD;		/* Timestamp of Departure [10ns] */
	UINT_32 u4ToA;		/* Timestamp of Arrival [10ns] */
} TIMINGMSMT_PARAM_T, *P_TIMINGMSMT_PARAM_T;

typedef struct _BSS_TRANSITION_MGT_PARAM_T {
	/* for Query */
	UINT_8 ucDialogToken;
	UINT_8 ucQueryReason;
	/* for Request */
	UINT_8 ucRequestMode;
	UINT_16 u2DisassocTimer;
	UINT_16 u2TermDuration;
	UINT_8 aucTermTsf[8];
	UINT_8 ucSessionURLLen;
	UINT_8 aucSessionURL[255];
	/* for Respone */
	UINT_8 fgPendingResponse:1;
	UINT_8 fgUnsolicitedReq:1;
	UINT_8 fgReserved:6;
	UINT_8 ucStatusCode;
	UINT_8 ucTermDelay;
	UINT_8 aucTargetBssid[MAC_ADDR_LEN];
	UINT_8 *pucOurNeighborBss;
	UINT_16 u2OurNeighborBssLen;
} BSS_TRANSITION_MGT_PARAM_T, *P_BSS_TRANSITION_MGT_PARAM_T;

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
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

VOID wnmWNMAction(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

VOID wnmReportTimingMeas(IN P_ADAPTER_T prAdapter, IN UINT_8 ucStaRecIndex, IN UINT_32 u4ToD, IN UINT_32 u4ToA);

#define WNM_UNIT_TEST 1

#if WNM_UNIT_TEST
VOID wnmTimingMeasUnitTest1(P_ADAPTER_T prAdapter, UINT_8 ucStaRecIndex);
#endif

UINT_8 wnmGetBtmToken(void);

VOID wnmSendBTMQueryFrame(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

VOID wnmSendBTMResponseFrame(IN P_ADAPTER_T prAdapter,
			IN P_STA_RECORD_T prStaRec);

VOID wnmRecvBTMRequest(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _WNM_H */
