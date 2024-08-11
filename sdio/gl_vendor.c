// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2016 MediaTek Inc.
 */
/*
** gl_vendor.c
**
**
*/

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

#include <linux/can/netlink.h>
#include <net/netlink.h>
#include <net/cfg80211.h>

#include "gl_os.h"
#include "debug.h"
#include "wlan_lib.h"
#include "gl_wext.h"
#include "precomp.h"
#include "gl_cfg80211.h"
#include "gl_vendor.h"
#include "wlan_oid.h"

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

P_SW_RFB_T g_arGscnResultsTempBuffer[MAX_BUFFERED_GSCN_RESULTS];
u8 g_GscanResultsTempBufferIndex;
u8 g_arGscanResultsIndicateNumber[MAX_BUFFERED_GSCN_RESULTS] = { 0, 0, 0, 0,
								 0 };

extern struct wireless_dev *gprWdev;

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
int mtk_cfg80211_NLA_PUT(struct sk_buff *skb, int attrtype, int attrlen,
			 const void *data)
{
	if (unlikely(nla_put(skb, attrtype, attrlen, data) < 0))
		return 0;

	return 1;
}

int mtk_cfg80211_nla_put_type(struct sk_buff *skb, ENUM_NLA_PUT_DATE_TYPE type,
			      int attrtype, const void *value)
{
	u8 u8data = 0;
	u16 u16data = 0;
	u32 u32data = 0;
	u64 u64data = 0;

	switch (type) {
	case NLA_PUT_DATE_U8:
		u8data = *(u8 *)value;
		return mtk_cfg80211_NLA_PUT(skb, attrtype, sizeof(u8), &u8data);

	case NLA_PUT_DATE_U16:
		u16data = *(u16 *)value;
		return mtk_cfg80211_NLA_PUT(skb, attrtype, sizeof(u16),
					    &u16data);

	case NLA_PUT_DATE_U32:
		u32data = *(u32 *)value;
		return mtk_cfg80211_NLA_PUT(skb, attrtype, sizeof(u32),
					    &u32data);

	case NLA_PUT_DATE_U64:
		u64data = *(u64 *)value;
		return mtk_cfg80211_NLA_PUT(skb, attrtype, sizeof(u64),
					    &u64data);

	default:
		break;
	}

	return 0;
}

int mtk_cfg80211_vendor_get_channel_list(struct wiphy *wiphy,
					 struct wireless_dev *wdev,
					 const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo;
	struct nlattr *attr;
	u32 band = 0;
	u8 ucNumOfChannel, i, j;
	RF_CHANNEL_INFO_T aucChannelList[MAX_CHN_NUM];
	u32 num_channels;
	wifi_channel channels[MAX_CHN_NUM];
	struct sk_buff *skb;

	ASSERT(wiphy && wdev);
	if ((data == NULL) || !data_len)
		return -EINVAL;

	DBGLOG(REQ, INFO, "vendor command: data_len=%d\n", data_len);

	attr = (struct nlattr *)data;
	if (attr->nla_type == WIFI_ATTRIBUTE_BAND)
		band = nla_get_u32(attr);

	DBGLOG(REQ, INFO, "Get channel list for band: %d\n", band);

	if (wlanGetWirelessDevice() == wdev) {
		prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(wiphy);
	} else {
		prGlueInfo = *((P_GLUE_INFO_T *)wiphy_priv(wiphy));
	}

	if (!prGlueInfo)
		return -EFAULT;

	if (prGlueInfo->u4ReadyFlag == 0) {
		DBGLOG(REQ, WARN, "driver is not ready\n");
		return -EFAULT;
	}

	if (band == 0) { /* 2.4G band */
		rlmDomainGetChnlList(prGlueInfo->prAdapter, BAND_2G4, true,
				     MAX_CHN_NUM, &ucNumOfChannel,
				     aucChannelList);
	} else { /* 5G band */
		rlmDomainGetChnlList(prGlueInfo->prAdapter, BAND_5G, true,
				     MAX_CHN_NUM, &ucNumOfChannel,
				     aucChannelList);
	}

	kalMemZero(channels, sizeof(channels));
	for (i = 0, j = 0; i < ucNumOfChannel; i++) {
		/* We need to report frequency list to HAL */
		channels[j] =
			nicChannelNum2Freq(aucChannelList[i].ucChannelNum) /
			1000;
		if (channels[j] == 0) {
			continue;
		} else if ((prGlueInfo->prAdapter->rWifiVar.rConnSettings
			    .u2CountryCode == COUNTRY_CODE_TW) &&
			   (channels[j] >= 5180 && channels[j] <= 5260)) {
			/* Taiwan NCC has resolution to follow FCC spec to
			 * support 5G Band 1/2/3/4 (CH36~CH48, CH52~CH64,
			 * CH100~CH140, CH149~CH165) Filter CH36~CH52 for
			 * compatible with some old devices.
			 */
			continue;
		} else {
			DBGLOG(REQ, INFO, "channels[%d] = %d\n", j,
			       channels[j]);
			j++;
		}
	}
	num_channels = j;

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(channels));
	if (!skb) {
		DBGLOG(REQ, ERROR, "Allocate skb failed\n");
		return -ENOMEM;
	}

	if (unlikely(nla_put_u32(skb, WIFI_ATTRIBUTE_NUM_CHANNELS,
				 num_channels) < 0))
		goto nla_put_failure;

	if (unlikely(nla_put(skb, WIFI_ATTRIBUTE_CHANNEL_LIST,
			     (sizeof(wifi_channel) * num_channels),
			     channels) < 0))
		goto nla_put_failure;

	return cfg80211_vendor_cmd_reply(skb);

nla_put_failure:
	kfree_skb(skb);
	return -EFAULT;
}

int mtk_cfg80211_vendor_set_country_code(struct wiphy *wiphy,
					 struct wireless_dev *wdev,
					 const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo;
	WLAN_STATUS rStatus;
	u32 u4BufLen;
	struct nlattr *attr;
	u8 country[2];

	ASSERT(wiphy && wdev);
	if ((data == NULL) || (data_len == 0))
		return -EINVAL;

	DBGLOG(REQ, INFO, "vendor command: data_len=%d\n", data_len);

	attr = (struct nlattr *)data;

	if (attr->nla_type != WIFI_ATTRIBUTE_COUNTRY_CODE)
		return -EINVAL;

	if (attr->nla_type == WIFI_ATTRIBUTE_COUNTRY_CODE &&
	    nla_len(attr) >= 2) {
		country[0] = *((u8 *)nla_data(attr));
		country[1] = *((u8 *)nla_data(attr) + 1);
	}

	DBGLOG(REQ, INFO, "Set country code: %c%c\n", country[0], country[1]);

	if (wlanGetWirelessDevice() == wdev) {
		prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(wiphy);
	} else {
		prGlueInfo = *((P_GLUE_INFO_T *)wiphy_priv(wiphy));
	}

	if (!prGlueInfo)
		return -EFAULT;

	if (prGlueInfo->u4ReadyFlag == 0) {
		DBGLOG(REQ, WARN, "driver is not ready\n");
		return -EFAULT;
	}

	rStatus = kalIoctl(prGlueInfo, wlanoidSetCountryCode, country, 2, false,
			   false, true, &u4BufLen);
	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, ERROR, "Set country code error: %x\n", rStatus);
		return -EFAULT;
	}

	return 0;
}

int mtk_cfg80211_vendor_get_version(struct wiphy *wiphy,
				    struct wireless_dev *wdev, const void *data,
				    int data_len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	struct sk_buff *skb = NULL;
	struct nlattr *attrlist = NULL;
	char aucVersionBuf[256];
	uint16_t u2CopySize = 0;
	uint16_t u2Len = 0;
	P_WIFI_VER_INFO_T prVerInfo;

	ASSERT(wiphy);
	ASSERT(wdev);

	if ((data == NULL) || !data_len)
		return -ENOMEM;

	kalMemZero(aucVersionBuf, 256);
	attrlist = (struct nlattr *)((uint8_t *)data);
	if (attrlist->nla_type == LOGGER_ATTRIBUTE_DRIVER_VER) {
		char aucDriverVersionStr[] = NIC_DRIVER_VERSION_STRING
					     "_2024062603000003";

		u2Len = kalStrLen(aucDriverVersionStr);
		DBGLOG(REQ, INFO, "Get driver version len: %d\n", u2Len);
		u2CopySize = (u2Len >= 256) ? 255 : u2Len;

		if (u2CopySize > 0) {
			kalMemCopy(aucVersionBuf, &aucDriverVersionStr[0],
				   u2CopySize);
		}
	} else if (attrlist->nla_type == LOGGER_ATTRIBUTE_FW_VER) {
		P_ADAPTER_T prAdapter;

		prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(wiphy);
		ASSERT(prGlueInfo);
		prAdapter = prGlueInfo->prAdapter;
		if (prAdapter) {
			u2CopySize = 16;
			prVerInfo = &prAdapter->rVerInfo;
			kalStrnCpy(aucVersionBuf,
				   prVerInfo->rPatchHeader.aucBuildDate,
				   u2CopySize);
			aucVersionBuf[u2CopySize] = '\0';
		}
	}

	if (u2CopySize <= 0)
		return -EFAULT;

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, u2CopySize);
	if (!skb) {
		DBGLOG(REQ, ERROR, "Allocate skb failed\n");
		return -ENOMEM;
	}

	DBGLOG(REQ, INFO, "Get version(%d)=[%s]\n", u2CopySize, aucVersionBuf);
	if (unlikely(nla_put_nohdr(skb, u2CopySize, &aucVersionBuf[0]) < 0))
		goto nla_put_failure;

	return cfg80211_vendor_cmd_reply(skb);

nla_put_failure:
	kfree_skb(skb);
	return -EFAULT;
}

int mtk_cfg80211_vendor_get_supported_feature_set(struct wiphy *wiphy,
						  struct wireless_dev *wdev,
						  const void *data,
						  int data_len)
{
	uint32_t u4FeatureSet;
	P_GLUE_INFO_T prGlueInfo;
	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(wdev);

	if (wdev == gprWdev) /* wlan0 */
		prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(wiphy);
	else
		prGlueInfo = *((P_GLUE_INFO_T *)wiphy_priv(wiphy));

	if (!prGlueInfo)
		return -EFAULT;

	if (prGlueInfo->u4ReadyFlag == 0) {
		DBGLOG(REQ, WARN, "driver is not ready\n");
		return -EFAULT;
	}

	u4FeatureSet = wlanGetSupportedFeatureSet(prGlueInfo);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(u4FeatureSet));
	if (!skb) {
		DBGLOG(REQ, ERROR, "Allocate skb failed\n");
		return -ENOMEM;
	}

	if (unlikely(nla_put_nohdr(skb, sizeof(u4FeatureSet), &u4FeatureSet) <
		     0)) {
		DBGLOG(REQ, ERROR, "nla_put_nohdr failed\n");
		goto nla_put_failure;
	}

	DBGLOG(REQ, TRACE, "supported feature set=0x%x\n", u4FeatureSet);

	return cfg80211_vendor_cmd_reply(skb);

nla_put_failure:
	kfree_skb(skb);
	return -EFAULT;
}
