// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   "p2p_scan.c"
 *    \brief  This file defines the p2p scan profile and the processing function
 * of scan result for SCAN Module.
 *
 *    The SCAN Profile selection is part of SCAN MODULE and responsible for
 * defining SCAN Parameters - e.g. MIN_CHANNEL_TIME, number of scan channels. In
 * this file we also define the process of SCAN Result including adding,
 * searching and removing SCAN record from the list.
 */

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

#include "precomp.h"

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

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
 *                   F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */
void scanP2pProcessBeaconAndProbeResp(
	IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb,
	IN P_WLAN_STATUS prStatus, IN P_BSS_DESC_T prBssDesc,
	IN P_WLAN_BEACON_FRAME_T prWlanBeaconFrame)
{
	u8 fgIsSkipThisBeacon = false;
	u8 fgIsP2pNetRegistered = false;

	/* Sanity check for p2p net device state */
	GLUE_SPIN_LOCK_DECLARATION();
	GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_NET_DEV);
	if (prAdapter->fgIsP2PRegistered &&
	    prAdapter->rP2PNetRegState == ENUM_NET_REG_STATE_REGISTERED)
		fgIsP2pNetRegistered = true;
	GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_NET_DEV);

	if (!fgIsP2pNetRegistered)
		return;

	/* Indicate network to kernel for P2P interface when:
	 *     1. This is P2P network
	 *     2. Driver is configured to report all networks
	 */
	if (prBssDesc->fgIsP2PPresent || prAdapter->p2p_scan_report_all_bss) {
		if ((prBssDesc->fgIsConnected) && /* P2P GC connected. */
		    ((prWlanBeaconFrame->u2FrameCtrl & MASK_FRAME_TYPE) ==
		     MAC_FRAME_BEACON) /* TX Beacon */
		    ) {
			u32 u4Idx = 0;
			P_BSS_INFO_T prP2pBssInfo = (P_BSS_INFO_T)NULL;

			for (u4Idx = 0; u4Idx < BSS_INFO_NUM; u4Idx++) {
				/* Check BSS for P2P. */
				/* Check BSSID. */
				prP2pBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter,
								     (u8)u4Idx);

				if (!IS_BSS_ACTIVE(prP2pBssInfo))
					continue;

				if ((prP2pBssInfo->eNetworkType !=
				     NETWORK_TYPE_P2P) ||
				    (UNEQUAL_MAC_ADDR(prP2pBssInfo->aucBSSID,
						      prBssDesc->aucBSSID) ||
				     (!EQUAL_SSID(prP2pBssInfo->aucSSID,
						  prP2pBssInfo->ucSSIDLen,
						  prBssDesc->aucSSID,
						  prBssDesc->ucSSIDLen)))) {
					continue;
				}
				if ((prP2pBssInfo->eCurrentOPMode ==
				     OP_MODE_INFRASTRUCTURE) && /* P2P GC */
				    (prP2pBssInfo->eConnectionState ==
				     PARAM_MEDIA_STATE_CONNECTED)) { /* Connected
					                              */
					fgIsSkipThisBeacon = true;
					if ((!prP2pBssInfo
					     ->ucDTIMPeriod)) {          /* First
						                          * Time.
						                          */
						prP2pBssInfo->ucDTIMPeriod =
							prBssDesc->ucDTIMPeriod;
						nicPmIndicateBssConnected(
							prAdapter,
							prP2pBssInfo
							->ucBssIndex);
					}
				}
			}
		}

		do {
			RF_CHANNEL_INFO_T rChannelInfo;

			ASSERT_BREAK((prSwRfb != NULL) && (prBssDesc != NULL));

			if (((prWlanBeaconFrame->u2FrameCtrl &
			      MASK_FRAME_TYPE) != MAC_FRAME_PROBE_RSP)) {
				/* Only report Probe Response frame to
				 * supplicant except passive scan. */
				/* Probe response collect much more information.
				 */
				if (fgIsSkipThisBeacon)
					break;
			}

			rChannelInfo.ucChannelNum = prBssDesc->ucChannelNum;
			rChannelInfo.eBand = prBssDesc->eBand;
			prBssDesc->fgIsP2PReport = true;

			DBGLOG(P2P, INFO, "indicate %s [%d]\n",
			       prBssDesc->aucSSID, prBssDesc->ucChannelNum);

			kalP2PIndicateBssInfo(prAdapter->prGlueInfo,
					      (u8 *)prSwRfb->pvHeader,
					      (u32)prSwRfb->u2PacketLen,
					      &rChannelInfo,
					      RCPI_TO_dBm(prBssDesc->ucRCPI));
		} while (false);
	}
}

void scnEventReturnChannel(IN P_ADAPTER_T prAdapter, IN u8 ucScnSeqNum)
{
	CMD_SCAN_CANCEL rCmdScanCancel;

	kalMemZero(&rCmdScanCancel, sizeof(rCmdScanCancel));

	/* send cancel message to firmware domain */
	rCmdScanCancel.ucSeqNum = ucScnSeqNum;
	rCmdScanCancel.ucIsExtChannel = (u8) false;

	wlanSendSetQueryCmd(prAdapter, CMD_ID_SCAN_CANCEL, true, false, false,
			    NULL, NULL, sizeof(CMD_SCAN_CANCEL),
			    (u8 *)&rCmdScanCancel, NULL, 0);
}

void scanRemoveAllP2pBssDesc(IN P_ADAPTER_T prAdapter)
{
	P_LINK_T prBSSDescList;
	P_BSS_DESC_T prBssDesc;
	P_BSS_DESC_T prBSSDescNext;

	ASSERT(prAdapter);

	prBSSDescList = &(prAdapter->rWifiVar.rScanInfo.rBSSDescList);

	/* Search BSS Desc from current SCAN result list. */
	LINK_FOR_EACH_ENTRY_SAFE(prBssDesc, prBSSDescNext, prBSSDescList,
				 rLinkEntry, BSS_DESC_T) {
	}
}

P_BSS_DESC_T scanP2pSearchDesc(IN P_ADAPTER_T prAdapter,
			       IN P_P2P_CONNECTION_REQ_INFO_T prConnReqInfo)
{
	P_BSS_DESC_T prCandidateBssDesc = (P_BSS_DESC_T)NULL,
		     prBssDesc = (P_BSS_DESC_T)NULL;
	P_LINK_T prBssDescList = (P_LINK_T)NULL;

	do {
		if ((prAdapter == NULL) || (prConnReqInfo == NULL))
			break;

		prBssDescList = &(prAdapter->rWifiVar.rScanInfo.rBSSDescList);

		DBGLOG(P2P, LOUD, "Connecting to BSSID: " MACSTR "\n",
		       MAC2STR(prConnReqInfo->aucBssid));
		DBGLOG(P2P, LOUD, "Connecting to SSID:%s, length:%d\n",
		       prConnReqInfo->rSsidStruct.aucSsid,
		       prConnReqInfo->rSsidStruct.ucSsidLen);

		LINK_FOR_EACH_ENTRY(prBssDesc, prBssDescList, rLinkEntry,
				    BSS_DESC_T) {
			DBGLOG(P2P, LOUD, "Checking BSS: " MACSTR "\n",
			       MAC2STR(prBssDesc->aucBSSID));

			if (prBssDesc->eBSSType != BSS_TYPE_INFRASTRUCTURE) {
				DBGLOG(P2P, LOUD,
				       "Ignore mismatch BSS type.\n");
				continue;
			}

			if (UNEQUAL_MAC_ADDR(prBssDesc->aucBSSID,
					     prConnReqInfo->aucBssid)) {
				DBGLOG(P2P, LOUD, "Ignore mismatch BSSID.\n");
				continue;
			}

			/* SSID should be the same? SSID is vary for each
			 * connection. so... */
			if (UNEQUAL_SSID(prConnReqInfo->rSsidStruct.aucSsid,
					 prConnReqInfo->rSsidStruct.ucSsidLen,
					 prBssDesc->aucSSID,
					 prBssDesc->ucSSIDLen)) {
				DBGLOG(P2P, TRACE,
				       "Connecting to BSSID: " MACSTR "\n",
				       MAC2STR(prConnReqInfo->aucBssid));
				DBGLOG(P2P, TRACE,
				       "Connecting to SSID:%s, length:%d\n",
				       prConnReqInfo->rSsidStruct.aucSsid,
				       prConnReqInfo->rSsidStruct.ucSsidLen);
				DBGLOG(P2P, TRACE,
				       "Checking SSID:%s, length:%d\n",
				       prBssDesc->aucSSID,
				       prBssDesc->ucSSIDLen);
				DBGLOG(P2P,
				       TRACE,
				       "Ignore mismatch SSID, (But BSSID match).\n");
				/* ASSERT(false); */ /*let p2p re-scan again */
				continue;
			}

			if (!prBssDesc->fgIsP2PPresent) {
				DBGLOG(P2P,
				       ERROR,
				       "SSID, BSSID, BSSTYPE match, but no P2P IE present.\n");
				continue;
			}

			/* Final decision. */
			prCandidateBssDesc = prBssDesc;
			break;
		}
	} while (false);

	return prCandidateBssDesc;
}
