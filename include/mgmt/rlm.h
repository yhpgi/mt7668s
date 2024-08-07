/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   "rlm.h"
 *    \brief
 */

#ifndef _RLM_H
#define _RLM_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 ********************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 ********************************************************************************
 */

extern BOOLEAN g_bIcapEnable;
extern BOOLEAN g_bCaptureDone;
extern UINT_16 g_u2DumpIndex;
#if CFG_SUPPORT_QA_TOOL
extern UINT_32 g_au4Offset[2][2];
extern UINT_32 g_au4IQData[256];
#endif

/*******************************************************************************
 *                              C O N S T A N T S
 ********************************************************************************
 */
#define ELEM_EXT_CAP_DEFAULT_VAL (ELEM_EXT_CAP_20_40_COEXIST_SUPPORT /*| ELEM_EXT_CAP_PSMP_CAP*/)

#if CFG_SUPPORT_RX_STBC
#define FIELD_HT_CAP_INFO_RX_STBC HT_CAP_INFO_RX_STBC_1_SS
#else
#define FIELD_HT_CAP_INFO_RX_STBC HT_CAP_INFO_RX_STBC_NO_SUPPORTED
#endif

#if CFG_SUPPORT_RX_SGI
#define FIELD_HT_CAP_INFO_SGI_20M HT_CAP_INFO_SHORT_GI_20M
#define FIELD_HT_CAP_INFO_SGI_40M HT_CAP_INFO_SHORT_GI_40M
#else
#define FIELD_HT_CAP_INFO_SGI_20M 0
#define FIELD_HT_CAP_INFO_SGI_40M 0
#endif

#if CFG_SUPPORT_RX_HT_GF
#define FIELD_HT_CAP_INFO_HT_GF HT_CAP_INFO_HT_GF
#else
#define FIELD_HT_CAP_INFO_HT_GF 0
#endif

#define HT_CAP_INFO_DEFAULT_VAL (HT_CAP_INFO_SUP_CHNL_WIDTH | HT_CAP_INFO_DSSS_CCK_IN_40M | HT_CAP_INFO_SM_POWER_SAVE)

#define AMPDU_PARAM_DEFAULT_VAL (AMPDU_PARAM_MAX_AMPDU_LEN_64K | AMPDU_PARAM_MSS_NO_RESTRICIT)

#define SUP_MCS_TX_DEFAULT_VAL SUP_MCS_TX_SET_DEFINED /* TX defined and TX/RX equal (TBD) */

#if CFG_SUPPORT_MFB
#define FIELD_HT_EXT_CAP_MFB HT_EXT_CAP_MCS_FEEDBACK_BOTH
#else
#define FIELD_HT_EXT_CAP_MFB HT_EXT_CAP_MCS_FEEDBACK_NO_FB
#endif

#if CFG_SUPPORT_RX_RDG
#define FIELD_HT_EXT_CAP_RDR HT_EXT_CAP_RD_RESPONDER
#else
#define FIELD_HT_EXT_CAP_RDR 0
#endif

#if CFG_SUPPORT_MFB || CFG_SUPPORT_RX_RDG
#define FIELD_HT_EXT_CAP_HTC HT_EXT_CAP_HTC_SUPPORT
#else
#define FIELD_HT_EXT_CAP_HTC 0
#endif

#define HT_EXT_CAP_DEFAULT_VAL \
	(HT_EXT_CAP_PCO | HT_EXT_CAP_PCO_TRANS_TIME_NONE | FIELD_HT_EXT_CAP_MFB | FIELD_HT_EXT_CAP_HTC | \
			FIELD_HT_EXT_CAP_RDR)

#define TX_BEAMFORMING_CAP_DEFAULT_VAL 0

#if CFG_SUPPORT_BFEE
#define TX_BEAMFORMING_CAP_BFEE \
	(TXBF_RX_NDP_CAPABLE | TXBF_EXPLICIT_COMPRESSED_FEEDBACK_IMMEDIATE_CAPABLE | TXBF_MINIMAL_GROUPING_1_2_3_CAPABLE | \
			TXBF_COMPRESSED_TX_ANTENNANUM_4_SUPPORTED | TXBF_CHANNEL_ESTIMATION_4STS_CAPABILITY)
#else
#define TX_BEAMFORMING_CAP_BFEE 0
#endif

#if CFG_SUPPORT_BFER
#define TX_BEAMFORMING_CAP_BFER (TXBF_TX_NDP_CAPABLE | TXBF_EXPLICIT_COMPRESSED_TX_CAPAB)
#else
#define TX_BEAMFORMING_CAP_BFER 0
#endif

#define ASEL_CAP_DEFAULT_VAL 0

/* Define bandwidth from user setting */
#define CONFIG_BW_20_40M 0
#define CONFIG_BW_20M 1 /* 20MHz only */

#if CFG_SUPPORT_BFER
#define MODE_LEGACY 0
#define MODE_HT 1
#define MODE_VHT 2
#endif

#if CFG_SUPPORT_802_11AC
#if CFG_SUPPORT_BFEE
#define FIELD_VHT_CAP_INFO_BFEE (VHT_CAP_INFO_SU_BEAMFORMEE_CAPABLE)
#define VHT_CAP_INFO_BEAMFORMEE_STS_CAP_MAX 3
#else
#define FIELD_VHT_CAP_INFO_BFEE 0
#endif

#if CFG_SUPPORT_BFER
#define FIELD_VHT_CAP_INFO_BFER \
	(VHT_CAP_INFO_SU_BEAMFORMER_CAPABLE | VHT_CAP_INFO_NUMBER_OF_SOUNDING_DIMENSIONS_2_SUPPORTED)
#else
#define FIELD_VHT_CAP_INFO_BFER 0
#endif

#define VHT_CAP_INFO_DEFAULT_VAL \
	(VHT_CAP_INFO_MAX_MPDU_LEN_3K | (AMPDU_PARAM_MAX_AMPDU_LEN_1024K << VHT_CAP_INFO_MAX_AMPDU_LENGTH_OFFSET))

#define VHT_CAP_INFO_DEFAULT_HIGHEST_DATA_RATE 0
#endif

/*******************************************************************************
 *                             D A T A   T Y P E S
 ********************************************************************************
 */

#if CFG_SUPPORT_DFS
typedef struct _SWITCH_CH_AND_BAND_PARAMS_T {
	BOOLEAN			fgBeaconNewChannelIsDFS;
	BOOLEAN			fgActionNewChannelIsDFS;
	BOOLEAN			fgNewChannelIsDisabled;
	UINT_8			ucCsaNewCh;
	UINT_8			ucCsaCount;
	UINT_8			ucVhtS1;
	UINT_8			ucVhtS2;
	UINT_8			ucVhtBw;
	ENUM_CHNL_EXT_T eSco;
	UINT_8			ucBssIndex;
} SWITCH_CH_AND_BAND_PARAMS_T, *P_SWITCH_CH_AND_BAND_PARAMS_T;
#endif

struct SUB_ELEMENT_LIST {
	struct SUB_ELEMENT_LIST *prNext;
	struct SUB_ELEMENT		 rSubIE;
};

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

/* It is used for RLM module to judge if specific network is valid
 * Note: Ad-hoc mode of AIS is not included now. (TBD)
 */
#define RLM_NET_PARAM_VALID(_prBssInfo) \
	(IS_BSS_ACTIVE(_prBssInfo) && ((_prBssInfo)->eConnectionState == PARAM_MEDIA_STATE_CONNECTED || \
										  (_prBssInfo)->eCurrentOPMode == OP_MODE_ACCESS_POINT || \
										  (_prBssInfo)->eCurrentOPMode == OP_MODE_IBSS || IS_BSS_BOW(_prBssInfo)))

#define RLM_NET_IS_11N(_prBssInfo) ((_prBssInfo)->ucPhyTypeSet & PHY_TYPE_SET_802_11N)
#define RLM_NET_IS_11GN(_prBssInfo) ((_prBssInfo)->ucPhyTypeSet & PHY_TYPE_SET_802_11GN)

#if CFG_SUPPORT_802_11AC
#define RLM_NET_IS_11AC(_prBssInfo) ((_prBssInfo)->ucPhyTypeSet & PHY_TYPE_SET_802_11AC)
#endif

/* The bandwidth modes are not used anymore. They represent if AP
 * can use 20/40 bandwidth, not all modes. (20110411)
 */
#define RLM_AP_IS_BW_40_ALLOWED(_prAdapter, _prBssInfo) \
	(((_prBssInfo)->eBand == BAND_2G4 && \
			 (_prAdapter)->rWifiVar.rConnSettings.uc2G4BandwidthMode == CONFIG_BW_20_40M) || \
			((_prBssInfo)->eBand == BAND_5G && \
					(_prAdapter)->rWifiVar.rConnSettings.uc5GBandwidthMode == CONFIG_BW_20_40M))

#if CFG_SUPPORT_DFS
#define MAX_CSA_COUNT 255
#define HAS_CH_SWITCH_PARAMS(prCSAParams, prBssDesc) \
	(prCSAParams->ucCsaNewCh > 0 && prCSAParams->ucCsaNewCh != prBssDesc->ucChannelNum)
#define HAS_SCO_PARAMS(prCSAParams) (prCSAParams->eSco > 0)
#define HAS_WIDE_BAND_PARAMS(prCSAParams) \
	(prCSAParams->ucVhtBw > 0 || prCSAParams->ucVhtS1 > 0 || prCSAParams->ucVhtS2 > 0)
#define SHOULD_CH_SWITCH(current, prCSAParams, prBssDesc) \
	(HAS_CH_SWITCH_PARAMS(prCSAParams, prBssDesc) && (current < prCSAParams->ucCsaCount))
#endif

/*******************************************************************************
 *                   F U N C T I O N   D E C L A R A T I O N S
 ********************************************************************************
 */
VOID rlmFsmEventInit(P_ADAPTER_T prAdapter);

VOID rlmFsmEventUninit(P_ADAPTER_T prAdapter);

VOID rlmReqGeneratePowerCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmReqGenerateSupportedChIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmReqGenerateHtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmReqGenerateExtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmRspGenerateHtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmRspGenerateExtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmRspGenerateHtOpIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmRspGenerateErpIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmGenerateMTKOuiIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

BOOLEAN rlmParseCheckMTKOuiIE(IN P_ADAPTER_T prAdapter, IN PUINT_8 pucBuf, IN PUINT_32 pu4Cap);

VOID rlmGenerateCsaIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmProcessBcn(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength);

VOID rlmProcessAssocRsp(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength);

VOID rlmProcessHtAction(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb);

#if CFG_SUPPORT_802_11AC
VOID rlmProcessVhtAction(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb);
#endif

VOID rlmFillSyncCmdParam(P_CMD_SET_BSS_RLM_PARAM_T prCmdBody, P_BSS_INFO_T prBssInfo);

VOID rlmSyncOperationParams(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

VOID rlmBssInitForAPandIbss(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

VOID rlmProcessAssocReq(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength);

VOID rlmBssAborted(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

#if CFG_SUPPORT_TDLS
UINT_32
rlmFillHtCapIEByParams(BOOLEAN fg40mAllowed, BOOLEAN fgShortGIDisabled, UINT_8 u8SupportRxSgi20,
		UINT_8 u8SupportRxSgi40, UINT_8 u8SupportRxGf, ENUM_OP_MODE_T eCurrentOPMode, UINT_8 *pOutBuf);

UINT_32 rlmFillHtCapIEByAdapter(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, UINT_8 *pOutBuf);

UINT_32 rlmFillVhtCapIEByAdapter(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, UINT_8 *pOutBuf);

#endif

#if CFG_SUPPORT_802_11AC
VOID rlmReqGenerateVhtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmRspGenerateVhtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmRspGenerateVhtOpIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

VOID rlmFillVhtOpIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_MSDU_INFO_T prMsduInfo);

VOID rlmRspGenerateVhtOpNotificationIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);
VOID rlmReqGenerateVhtOpNotificationIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

#endif

#if CFG_SUPPORT_DFS
VOID rlmProcessSpecMgtAction(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb);

VOID rlmResetCSAParams(P_BSS_INFO_T prBssInfo);

VOID rlmCsaTimeout(IN P_ADAPTER_T prAdapter, ULONG ulParamPtr);
#endif

VOID rlmSendOpModeNotificationFrame(
		P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec, UINT_8 ucChannelWidth, UINT_8 ucNss);

VOID rlmSendSmPowerSaveFrame(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec, UINT_8 ucNss);

VOID rlmChangeVhtOpBwPara(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex, UINT_8 ucChannelWidth);

BOOLEAN
rlmChangeOperationMode(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex, UINT_8 ucChannelWidth, UINT_8 ucNss);

#if CFG_SUPPORT_BFER
VOID rlmBfStaRecPfmuUpdate(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec);

VOID rlmETxBfTriggerPeriodicSounding(P_ADAPTER_T prAdapter);

BOOLEAN
rlmClientSupportsVhtETxBF(P_STA_RECORD_T prStaRec);

UINT_8
rlmClientSupportsVhtBfeeStsCap(P_STA_RECORD_T prStaRec);

BOOLEAN
rlmClientSupportsHtETxBF(P_STA_RECORD_T prStaRec);
#endif

#if CFG_SUPPORT_DBDC_TC6
void rlmSendChannelSwitchFrame(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex);

UINT_8
rlmGetBssOpBwByVhtAndHtOpInfo(P_BSS_INFO_T prBssInfo);

UINT_8
rlmGetVhtOpBwByBssOpBw(UINT_8 ucBssOpBw);
#endif

VOID rlmModifyVhtBwPara(PUINT_8 pucVhtChannelFrequencyS1, PUINT_8 pucVhtChannelFrequencyS2, PUINT_8 pucVhtChannelWidth);

VOID rlmReviseMaxBw(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex, P_ENUM_CHNL_EXT_T peExtend, PUINT_8 peChannelWidth,
		PUINT_8 pucS1, PUINT_8 pucPrimaryCh);

VOID rlmRevisePreferBandwidthNss(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex, P_STA_RECORD_T prStaRec);

#if CFG_SUPPORT_802_11K
void rlmReqGenerateRRMEnabledCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);

void rlmFillRrmCapa(PUINT_8 pucCapa);

void rlmTxNeighborReportRequest(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec, struct SUB_ELEMENT_LIST *prSubIEs);

void rlmProcessNeighborReportResponse(P_ADAPTER_T prAdapter, P_WLAN_ACTION_FRAME prAction, UINT_16 u2PacketLen);
#endif

#if CFG_SUPPORT_QUIET
VOID rrmQuietIeNotExist(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

VOID rrmQuietHandleQuietIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_IE_QUIET_T pucQUIE);

VOID rrmTxQuietTimeout(P_ADAPTER_T prAdapter, ULONG ulParamPtr);

#endif

/*******************************************************************************
 *                              F U N C T I O N S
 ********************************************************************************
 */

#endif /* _RLM_H */
