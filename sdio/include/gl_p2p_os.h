/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   gl_p2p_os.h
 *    \brief  List the external reference to OS for p2p GLUE Layer.
 *
 *    In this file we define the data structure - GLUE_INFO_T to store those objects
 *    we acquired from OS - e.g. TIMER, SPINLOCK, NET DEVICE ... . And all the
 *    external reference (header file, extern func() ..) to OS for GLUE Layer should
 *    also list down here.
 */

#ifndef _GL_P2P_OS_H
#define _GL_P2P_OS_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 ********************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 ********************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   V A R I A B L E
 ********************************************************************************
 */

extern const struct net_device_ops p2p_netdev_ops;

/*******************************************************************************
 *                              C O N S T A N T S
 ********************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 ********************************************************************************
 */

/*******************************************************************************
 *                             D A T A   T Y P E S
 ********************************************************************************
 */

/*******************************************************************************
 *                            P U B L I C   D A T A
 ********************************************************************************
 */
extern struct net_device	 *g_P2pPrDev;
extern struct wireless_dev *gprP2pWdev;
extern struct wireless_dev *gprP2pRoleWdev[KAL_P2P_NUM];

/*******************************************************************************
 *                           P R I V A T E   D A T A
 ********************************************************************************
 */

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 ********************************************************************************
 */

struct _GL_P2P_INFO_T {
	struct net_device *prDevHandler;
	struct net_device *aprRoleHandler;
	/* cfg80211 */
	struct wireless_dev *prWdev;
	INT_32 i4Generation;
	UINT_8 ucRole; /* 0: P2P Device, 1: Group Client, 2: Group Owner */
	UINT_32 u4CipherPairwise;
	UINT_8	aucWSCIE[4][400]; /* 0 for beacon, 1 for probe req, 2 for probe response, 3 for assoc response */
	UINT_16 u2WSCIELen[4];
	UINT_8	aucWFDIE[400]; /* 0 for beacon, 1 for probe req, 2 for probe response */
	UINT_16 u2WFDIELen;
	struct cfg80211_chan_def *chandef;
	UINT_32					  cac_time_ms;
	PARAM_MAC_ADDRESS aucblackMACList[10];
	UINT_8			  ucMaxClients;
};

struct _GL_P2P_DEV_INFO_T {
	struct cfg80211_scan_request *prScanRequest;
	struct cfg80211_scan_request  rBackupScanRequest;
	UINT_64						  u8Cookie;
	UINT_32						  u4OsMgmtFrameFilter;
	UINT_32			  u4PacketFilter;
	PARAM_MAC_ADDRESS aucMCAddrList[MAX_NUM_GROUP_ADDR];
	UINT_8			  ucWSCRunning;
};

#ifdef CONFIG_NL80211_TESTMODE
typedef struct _NL80211_DRIVER_TEST_PRE_PARAMS {
	UINT_16 idx_mode;
	UINT_16 idx;
	UINT_32 value;
} NL80211_DRIVER_TEST_PRE_PARAMS, *P_NL80211_DRIVER_TEST_PRE_PARAMS;

typedef struct _NL80211_DRIVER_TEST_PARAMS {
	UINT_32 index;
	UINT_32 buflen;
} NL80211_DRIVER_TEST_PARAMS, *P_NL80211_DRIVER_TEST_PARAMS;

/* P2P Sigma*/
typedef struct _NL80211_DRIVER_P2P_SIGMA_PARAMS {
	NL80211_DRIVER_TEST_PARAMS hdr;
	UINT_32					   idx;
	UINT_32					   value;
} NL80211_DRIVER_P2P_SIGMA_PARAMS, *P_NL80211_DRIVER_P2P_SIGMA_PARAMS;

/* Hotspot Client Management */
typedef struct _NL80211_DRIVER_hotspot_block_PARAMS {
	NL80211_DRIVER_TEST_PARAMS hdr;
	UINT_8					   ucblocked;
	UINT_8					   aucBssid[MAC_ADDR_LEN];
} NL80211_DRIVER_hotspot_block_PARAMS, *P_NL80211_DRIVER_hotspot_block_PARAMS;

typedef struct _NL80211_DRIVER_WFD_PARAMS {
	NL80211_DRIVER_TEST_PARAMS hdr;
	UINT_32					   WfdCmdType;
	UINT_8					   WfdEnable;
	UINT_8					   WfdCoupleSinkStatus;
	UINT_8					   WfdSessionAvailable;
	UINT_8					   WfdSigmaMode;
	UINT_16					   WfdDevInfo;
	UINT_16					   WfdControlPort;
	UINT_16					   WfdMaximumTp;
	UINT_16					   WfdExtendCap;
	UINT_8					   WfdCoupleSinkAddress[MAC_ADDR_LEN];
	UINT_8					   WfdAssociatedBssid[MAC_ADDR_LEN];
	UINT_8					   WfdVideoIp[4];
	UINT_8					   WfdAudioIp[4];
	UINT_16					   WfdVideoPort;
	UINT_16					   WfdAudioPort;
	UINT_32					   WfdFlag;
	UINT_32					   WfdPolicy;
	UINT_32					   WfdState;
	UINT_8					   WfdSessionInformationIE[24 * 8]; /* Include Subelement ID, length */
	UINT_16					   WfdSessionInformationIELen;
	UINT_8					   aucReserved1[2];
	UINT_8					   aucWfdPrimarySinkMac[MAC_ADDR_LEN];
	UINT_8					   aucWfdSecondarySinkMac[MAC_ADDR_LEN];
	UINT_32					   WfdAdvanceFlag;
	/* Group 1 64 bytes */
	UINT_8	aucWfdLocalIp[4];
	UINT_16 WfdLifetimeAc2;		 /* Unit is 2 TU */
	UINT_16 WfdLifetimeAc3;		 /* Unit is 2 TU */
	UINT_16 WfdCounterThreshold; /* Unit is ms */
	UINT_8	aucReserved2[54];
	/* Group 3 64 bytes */
	UINT_8 aucReserved3[64];
	/* Group 3 64 bytes */
	UINT_8 aucReserved4[64];
} NL80211_DRIVER_WFD_PARAMS, *P_NL80211_DRIVER_WFD_PARAMS;
#endif

/*******************************************************************************
 *                            P U B L I C   D A T A
 ********************************************************************************
 */

/*******************************************************************************
 *                           P R I V A T E   D A T A
 ********************************************************************************
 */

BOOLEAN p2pRegisterToWlan(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pUnregisterToWlan(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pLaunch(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pRemove(P_GLUE_INFO_T prGlueInfo);

VOID p2pSetMode(IN UINT_8 ucAPMode);

BOOLEAN glRegisterP2P(P_GLUE_INFO_T prGlueInfo, const char *prDevName, const char *prDevName2, UINT_8 ucApMode);

BOOLEAN glUnregisterP2P(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pNetRegister(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgIsRtnlLockAcquired);

BOOLEAN p2pNetUnregister(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgIsRtnlLockAcquired);

BOOLEAN p2PFreeInfo(P_GLUE_INFO_T prGlueInfo);

VOID	p2pSetSuspendMode(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgEnable);
BOOLEAN glP2pCreateWirelessDevice(P_GLUE_INFO_T prGlueInfo);
VOID	glP2pDestroyWirelessDevice(VOID);
VOID	p2pUpdateChannelTableByDomain(P_GLUE_INFO_T prGlueInfo);
VOID	p2pFreeMemSafe(P_GLUE_INFO_T prGlueInfo, VOID **pprMemInfo, UINT_32 size);
#endif
