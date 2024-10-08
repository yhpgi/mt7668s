/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   gl_cfg80211.h
 *    \brief  This file is for Portable Driver linux cfg80211 support.
 */

#ifndef _GL_CFG80211_H
#define _GL_CFG80211_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <linux/ieee80211.h>
#include <net/cfg80211.h>

#include "gl_os.h"

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */

#ifdef CONFIG_NL80211_TESTMODE
#define NL80211_DRIVER_TESTMODE_VERSION    2
#endif

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

#ifdef CONFIG_NL80211_TESTMODE
#if CFG_SUPPORT_NFC_BEAM_PLUS

typedef struct _NL80211_DRIVER_SET_NFC_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	u32 NFC_Enable;
} NL80211_DRIVER_SET_NFC_PARAMS, *P_NL80211_DRIVER_SET_NFC_PARAMS;

#endif

typedef struct _NL80211_DRIVER_GET_STA_STATISTICS_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	u32 u4Version;
	u32 u4Flag;
	u8 aucMacAddr[MAC_ADDR_LEN];
} NL80211_DRIVER_GET_STA_STATISTICS_PARAMS,
*P_NL80211_DRIVER_GET_STA_STATISTICS_PARAMS;

typedef enum _ENUM_TESTMODE_STA_STATISTICS_ATTR {
	NL80211_TESTMODE_STA_STATISTICS_INVALID = 0,
	NL80211_TESTMODE_STA_STATISTICS_VERSION,
	NL80211_TESTMODE_STA_STATISTICS_MAC,
	NL80211_TESTMODE_STA_STATISTICS_LINK_SCORE,
	NL80211_TESTMODE_STA_STATISTICS_FLAG,

	NL80211_TESTMODE_STA_STATISTICS_PER,
	NL80211_TESTMODE_STA_STATISTICS_RSSI,
	NL80211_TESTMODE_STA_STATISTICS_PHY_MODE,
	NL80211_TESTMODE_STA_STATISTICS_TX_RATE,

	NL80211_TESTMODE_STA_STATISTICS_TOTAL_CNT,
	NL80211_TESTMODE_STA_STATISTICS_THRESHOLD_CNT,
	NL80211_TESTMODE_STA_STATISTICS_AVG_PROCESS_TIME,

	NL80211_TESTMODE_STA_STATISTICS_FAIL_CNT,
	NL80211_TESTMODE_STA_STATISTICS_TIMEOUT_CNT,
	NL80211_TESTMODE_STA_STATISTICS_AVG_AIR_TIME,

	NL80211_TESTMODE_STA_STATISTICS_TC_EMPTY_CNT_ARRAY,
	NL80211_TESTMODE_STA_STATISTICS_TC_QUE_LEN_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_TC_AVG_QUE_LEN_ARRAY,
	NL80211_TESTMODE_STA_STATISTICS_TC_CUR_QUE_LEN_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_RESERVED_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_NUM
} ENUM_TESTMODE_STA_STATISTICS_ATTR;
#endif
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

/* cfg80211 hooks */
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
int mtk_cfg80211_change_iface(struct wiphy *wiphy,
			      struct net_device *ndev,
			      enum nl80211_iftype type,
			      struct vif_params *params);
#else
int mtk_cfg80211_change_iface(struct wiphy *wiphy,
			      struct net_device *ndev,
			      enum nl80211_iftype type,
			      u32 *flags,
			      struct vif_params *params);
#endif

int mtk_cfg80211_add_key(struct wiphy *wiphy,
			 struct net_device *ndev,
			 u8 key_index,
			 bool pairwise,
			 const u8 *mac_addr,
			 struct key_params *params);

int mtk_cfg80211_get_key(struct wiphy *wiphy,
			 struct net_device *ndev,
			 u8 key_index,
			 bool pairwise,
			 const u8 *mac_addr,
			 void *cookie,
			 void (*callback)(void *cookie, struct key_params *));

int mtk_cfg80211_del_key(struct wiphy *wiphy,
			 struct net_device *ndev,
			 u8 key_index,
			 bool pairwise,
			 const u8 *mac_addr);

int mtk_cfg80211_set_default_key(struct wiphy *wiphy,
				 struct net_device *ndev,
				 u8 key_index,
				 bool unicast,
				 bool multicast);

int mtk_cfg80211_get_station(struct wiphy *wiphy,
			     struct net_device *ndev,
			     const u8 *mac,
			     struct station_info *sinfo);

int mtk_cfg80211_get_link_statistics(struct wiphy *wiphy,
				     struct net_device *ndev,
				     u8 *mac,
				     struct station_info *sinfo);

int mtk_cfg80211_scan(struct wiphy *wiphy,
		      struct cfg80211_scan_request *request);
void mtk_cfg80211_abort_scan(struct wiphy *wiphy, struct wireless_dev *wdev);

int mtk_cfg80211_auth(struct wiphy *wiphy,
		      struct net_device *ndev,
		      struct cfg80211_auth_request *req);
int mtk_cfg80211_connect(struct wiphy *wiphy,
			 struct net_device *ndev,
			 struct cfg80211_connect_params *sme);
int mtk_cfg80211_disconnect(struct wiphy *wiphy,
			    struct net_device *ndev,
			    u16 reason_code);
int mtk_cfg80211_deauth(struct wiphy *wiphy,
			struct net_device *ndev,
			struct cfg80211_deauth_request *req);
int mtk_cfg80211_disassoc(struct wiphy *wiphy,
			  struct net_device *ndev,
			  struct cfg80211_disassoc_request *req);

int mtk_cfg80211_join_ibss(struct wiphy *wiphy,
			   struct net_device *ndev,
			   struct cfg80211_ibss_params *params);

int mtk_cfg80211_leave_ibss(struct wiphy *wiphy, struct net_device *ndev);

int mtk_cfg80211_set_power_mgmt(struct wiphy *wiphy,
				struct net_device *ndev,
				bool enabled,
				int timeout);

int mtk_cfg80211_set_pmksa(struct wiphy *wiphy,
			   struct net_device *ndev,
			   struct cfg80211_pmksa *pmksa);

int mtk_cfg80211_del_pmksa(struct wiphy *wiphy,
			   struct net_device *ndev,
			   struct cfg80211_pmksa *pmksa);

int mtk_cfg80211_flush_pmksa(struct wiphy *wiphy, struct net_device *ndev);

int mtk_cfg80211_set_rekey_data(struct wiphy *wiphy,
				struct net_device *dev,
				struct cfg80211_gtk_rekey_data *data);

int mtk_cfg80211_remain_on_channel(struct wiphy *wiphy,
				   struct wireless_dev *wdev,
				   struct ieee80211_channel *chan,
				   unsigned int duration,
				   u64 *cookie);

int mtk_cfg80211_cancel_remain_on_channel(struct wiphy *wiphy,
					  struct wireless_dev *wdev,
					  u64 cookie);

int mtk_cfg80211_mgmt_tx(struct wiphy *wiphy,
			 struct wireless_dev *wdev,
			 struct cfg80211_mgmt_tx_params *params,
			 u64 *cookie);

void mtk_cfg80211_mgmt_frame_register(IN struct wiphy *wiphy,
				      IN struct wireless_dev *wdev,
				      IN u16 frame_type,
				      IN bool reg);

int mtk_cfg80211_mgmt_tx_cancel_wait(struct wiphy *wiphy,
				     struct wireless_dev *wdev,
				     u64 cookie);

#ifdef CONFIG_NL80211_TESTMODE
int mtk_cfg80211_testmode_get_sta_statistics(IN struct wiphy *wiphy,
					     IN void *data,
					     IN int len,
					     IN P_GLUE_INFO_T prGlueInfo);

int mtk_cfg80211_testmode_get_scan_done(IN struct wiphy *wiphy,
					IN void *data,
					IN int len,
					IN P_GLUE_INFO_T prGlueInfo);

int mtk_cfg80211_testmode_cmd(struct wiphy *wiphy,
			      struct wireless_dev *wdev,
			      void *data,
			      int len);

int mtk_cfg80211_testmode_sw_cmd(IN struct wiphy *wiphy,
				 IN void *data,
				 IN int len);

#if CFG_SUPPORT_NFC_BEAM_PLUS
int mtk_cfg80211_testmode_get_scan_done(IN struct wiphy *wiphy,
					IN void *data,
					IN int len,
					IN P_GLUE_INFO_T prGlueInfo);
#endif
#endif

int mtk_cfg80211_sched_scan_start(IN struct wiphy *wiphy,
				  IN struct net_device *ndev,
				  IN struct cfg80211_sched_scan_request *request);

int mtk_cfg80211_sched_scan_stop(IN struct wiphy *wiphy,
				 IN struct net_device *ndev);

int mtk_cfg80211_assoc(struct wiphy *wiphy,
		       struct net_device *ndev,
		       struct cfg80211_assoc_request *req);

int mtk_cfg80211_change_station(struct wiphy *wiphy,
				struct net_device *ndev,
				const u8 *mac,
				struct station_parameters *params);

int mtk_cfg80211_add_station(struct wiphy *wiphy,
			     struct net_device *ndev,
			     const u8 *mac,
			     struct station_parameters *params);

int mtk_cfg80211_del_station(struct wiphy *wiphy,
			     struct net_device *ndev,
			     struct station_del_parameters *params);

int mtk_cfg80211_tdls_mgmt(struct wiphy *wiphy,
			   struct net_device *dev,
			   const u8 *peer,
			   u8 action_code,
			   u8 dialog_token,
			   u16 status_code,
			   u32 peer_capability,
			   bool initiator,
			   const u8 *buf,
			   size_t len);

int mtk_cfg80211_tdls_oper(struct wiphy *wiphy,
			   struct net_device *dev,
			   const u8 *peer,
			   enum nl80211_tdls_operation oper);

s32 mtk_cfg80211_process_str_cmd(P_GLUE_INFO_T prGlueInfo, u8 *cmd, s32 len);

void mtk_reg_notify(IN struct wiphy *pWiphy,
		    IN struct regulatory_request *pRequest);
void cfg80211_regd_set_wiphy(IN struct wiphy *pWiphy);

void mtk_cfg80211_set_wakeup(struct wiphy *wiphy, bool enabled);

int mtk_cfg80211_suspend(struct wiphy *wiphy, struct cfg80211_wowlan *wow);

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */

#endif
