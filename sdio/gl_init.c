// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   gl_init.c
 *    \brief  Main routines of Linux driver
 *
 *    This file contains the main routines of Linux driver for MediaTek Inc. 802.11
 *    Wireless LAN Adapters.
 */

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 ********************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 ********************************************************************************
 */
#include <linux/platform_device.h>
#ifdef CONFIG_PM_SLEEP
#include <linux/suspend.h>
#endif
#include "gl_os.h"
#include "debug.h"
#include "wlan_lib.h"
#include "gl_wext.h"
#include "gl_cfg80211.h"
#include "precomp.h"
#if CFG_SUPPORT_AGPS_ASSIST
#include "gl_kal.h"
#endif
#include "gl_vendor.h"

#include <linux/reboot.h>

/*******************************************************************************
 *                              C O N S T A N T S
 ********************************************************************************
 */
/* #define MAX_IOREQ_NUM   10 */
struct semaphore g_halt_sem;
int				 g_u4HaltFlag;

#ifdef CFG_SUPPORT_PRIVACY_INFO
uint8_t empty_mac[6] = { 0 };
#endif

struct wireless_dev *gprWdev;

struct completion  rWaitForResetComp;
struct completion *prWaitForResetComp = NULL;

/*******************************************************************************
 *                             D A T A   T Y P E S
 ********************************************************************************
 */
/* Tasklet mechanism is like buttom-half in Linux. We just want to
 * send a signal to OS for interrupt defer processing. All resources
 * are NOT allowed reentry, so txPacket, ISR-DPC and ioctl must avoid preempty.
 */
typedef struct _WLANDEV_INFO_T {
	struct net_device *prDev;
} WLANDEV_INFO_T, *P_WLANDEV_INFO_T;

#if CFG_SUPPORT_CFG80211_AUTH
#if CFG_WDEV_LOCK_THREAD_SUPPORT
struct delayed_work wdev_lock_workq;
#endif
#endif
/*******************************************************************************
 *                            P U B L I C   D A T A
 ********************************************************************************
 */

#define DRIVER_NAME "mt76x8_wifi_sdio"

MODULE_AUTHOR("Mediatek Ltd.");
MODULE_AUTHOR("Yogi Hermawan");
MODULE_DESCRIPTION("Mediatek MT7668S (SDIO) Wireleses Driver");
MODULE_VERSION(NIC_DRIVER_VERSION_STRING);
MODULE_LICENSE("Dual BSD/GPL");

#ifdef CFG_DRIVER_INF_NAME_CHANGE
char *gprifnamesta = "";
char *gprifnamep2p = "";
char *gprifnameap  = "";
module_param_named(sta, gprifnamesta, charp, 0);
module_param_named(p2p, gprifnamep2p, charp, 0);
module_param_named(ap, gprifnameap, charp, 0);
#endif /* CFG_DRIVER_INF_NAME_CHANGE */

/* NIC interface name */
#define NIC_INF_NAME "wlan%d"

#ifdef CFG_DRIVER_INF_NAME_CHANGE
/* Kernel IFNAMESIZ is 16, we use 5 in case some protocol might auto gen interface name, */
/* in that case, the interface name might have risk of over kernel's IFNAMESIZ */
#define CUSTOM_IFNAMESIZ 5
#endif /* CFG_DRIVER_INF_NAME_CHANGE */

#if CFG_SUPPORT_SNIFFER
#define NIC_MONITOR_INF_NAME "radiotap%d"
#endif

UINT_8 aucDebugModule[DBG_MODULE_NUM];

/* 4 2007/06/26, mikewu, now we don't use this, we just fix the number of wlan device to 1 */
static WLANDEV_INFO_T arWlanDevInfo[CFG_MAX_WLAN_DEVICES] = { { 0 } };

static UINT_32 u4WlanDevNum; /* How many NICs coexist now */

/**20150205 added work queue for sched_scan to avoid cfg80211 stop schedule scan dead loack**/
struct delayed_work sched_workq;

#define CFG_WIFI_FILENAME "wifi.cfg"

#define CFG_EEPRM_FILENAME "EEPROM"
#define FILE_NAME_MAX 64

#if (CFG_EFUSE_BUFFER_MODE_DELAY_CAL == 1)
static PUINT_8 apucEepromName[] = { (PUINT_8)CFG_EEPRM_FILENAME "_MT", NULL };
#endif

int CFG80211_Suspend(struct wiphy *wiphy, struct cfg80211_wowlan *wow)
{
	DBGLOG(INIT, INFO, "CFG80211 suspend CB\n");

	return 0;
}

int CFG80211_Resume(struct wiphy *wiphy)
{
	DBGLOG(INIT, INFO, "CFG80211 resume CB\n");

	return 0;
}

/*******************************************************************************
 *                           P R I V A T E   D A T A
 ********************************************************************************
 */
#define MT76X8_WIFI_RESET_DEFAULT_DELAY_MS 5

#define CHAN2G(_channel, _freq, _flags) \
	{ \
		.band = KAL_BAND_2GHZ, .center_freq = (_freq), .hw_value = (_channel), .flags = (_flags), \
		.max_antenna_gain = 0, .max_power = 30, \
	}
static struct ieee80211_channel mtk_2ghz_channels[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

#define CHAN5G(_channel, _flags) \
	{ \
		.band = KAL_BAND_5GHZ, .center_freq = 5000 + (5 * (_channel)), .hw_value = (_channel), .flags = (_flags), \
		.max_antenna_gain = 0, .max_power = 30, \
	}
static struct ieee80211_channel mtk_5ghz_channels[] = {
	CHAN5G(36, 0),
	CHAN5G(40, 0),
	CHAN5G(44, 0),
	CHAN5G(48, 0),
	CHAN5G(52, 0),
	CHAN5G(56, 0),
	CHAN5G(60, 0),
	CHAN5G(64, 0),
	CHAN5G(100, 0),
	CHAN5G(104, 0),
	CHAN5G(108, 0),
	CHAN5G(112, 0),
	CHAN5G(116, 0),
	CHAN5G(120, 0),
	CHAN5G(124, 0),
	CHAN5G(128, 0),
	CHAN5G(132, 0),
	CHAN5G(136, 0),
	CHAN5G(140, 0),
	CHAN5G(144, 0),
	CHAN5G(149, 0),
	CHAN5G(153, 0),
	CHAN5G(157, 0),
	CHAN5G(161, 0),
	CHAN5G(165, 0),
};

#define RATETAB_ENT(_rate, _rateid, _flags) \
	{ \
		.bitrate = (_rate), .hw_value = (_rateid), .flags = (_flags), \
	}

/* for cfg80211 - rate table */
static struct ieee80211_rate mtk_rates[] = {
	RATETAB_ENT(10, 0x1000, 0), RATETAB_ENT(20, 0x1001, 0), RATETAB_ENT(55, 0x1002, 0),
	RATETAB_ENT(110, 0x1003, 0), /* 802.11b */
	RATETAB_ENT(60, 0x2000, 0), RATETAB_ENT(90, 0x2001, 0), RATETAB_ENT(120, 0x2002, 0), RATETAB_ENT(180, 0x2003, 0),
	RATETAB_ENT(240, 0x2004, 0), RATETAB_ENT(360, 0x2005, 0), RATETAB_ENT(480, 0x2006, 0),
	RATETAB_ENT(540, 0x2007, 0), /* 802.11a/g */
};

#define mtk_a_rates (mtk_rates + 4)
#define mtk_a_rates_size (ARRAY_SIZE(mtk_rates) - 4)
#define mtk_g_rates (mtk_rates + 0)
#define mtk_g_rates_size (ARRAY_SIZE(mtk_rates) - 0)

#define WLAN_MCS_INFO \
	{ \
		.rx_mask = { 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .rx_highest = 0, .tx_params = IEEE80211_HT_MCS_TX_DEFINED, \
	}

#define WLAN_VHT_MCS_INFO \
	{ \
		.rx_mcs_map = 0xFFFA, .rx_highest = cpu_to_le16(867), .tx_mcs_map = 0xFFFA, .tx_highest = cpu_to_le16(867), \
	}

#define WLAN_HT_CAP \
	{ \
		.ht_supported = true, \
		.cap		  = IEEE80211_HT_CAP_SUP_WIDTH_20_40 | IEEE80211_HT_CAP_SM_PS | IEEE80211_HT_CAP_GRN_FLD | \
			   IEEE80211_HT_CAP_SGI_20 | IEEE80211_HT_CAP_SGI_40, \
		.ampdu_factor = IEEE80211_HT_MAX_AMPDU_64K, .ampdu_density = IEEE80211_HT_MPDU_DENSITY_NONE, \
		.mcs = WLAN_MCS_INFO, \
	}

#define WLAN_VHT_CAP \
	{ \
		.vht_supported = true, \
		.cap		   = IEEE80211_VHT_CAP_RXLDPC | IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK | \
			   IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_11454 | IEEE80211_VHT_CAP_RXLDPC | IEEE80211_VHT_CAP_SHORT_GI_80 | \
			   IEEE80211_VHT_CAP_TXSTBC | IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE | \
			   IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE, \
		.vht_mcs = WLAN_VHT_MCS_INFO, \
	}

/* public for both Legacy Wi-Fi / P2P access */
struct ieee80211_supported_band mtk_band_2ghz = {
	.band		= KAL_BAND_2GHZ,
	.channels	= mtk_2ghz_channels,
	.n_channels = ARRAY_SIZE(mtk_2ghz_channels),
	.bitrates	= mtk_g_rates,
	.n_bitrates = mtk_g_rates_size,
	.ht_cap		= WLAN_HT_CAP,
};

/* public for both Legacy Wi-Fi / P2P access */
struct ieee80211_supported_band mtk_band_5ghz = {
	.band		= KAL_BAND_5GHZ,
	.channels	= mtk_5ghz_channels,
	.n_channels = ARRAY_SIZE(mtk_5ghz_channels),
	.bitrates	= mtk_a_rates,
	.n_bitrates = mtk_a_rates_size,
	.ht_cap		= WLAN_HT_CAP,
	.vht_cap	= WLAN_VHT_CAP,
};

const UINT_32 mtk_cipher_suites[5] = {
	/* keep WEP first, it may be removed below */
	WLAN_CIPHER_SUITE_WEP40, WLAN_CIPHER_SUITE_WEP104, WLAN_CIPHER_SUITE_TKIP, WLAN_CIPHER_SUITE_CCMP,

	/* keep last -- depends on hw flags! */
	WLAN_CIPHER_SUITE_AES_CMAC
};

static struct cfg80211_ops mtk_wlan_ops = {
	.change_virtual_intf = mtk_cfg80211_change_iface,
	.add_key			 = mtk_cfg80211_add_key,
	.get_key			 = mtk_cfg80211_get_key,
	.del_key			 = mtk_cfg80211_del_key,
	.set_default_key	 = mtk_cfg80211_set_default_key,
	.get_station		 = mtk_cfg80211_get_station,
#if CFG_SUPPORT_TDLS
	.change_station = mtk_cfg80211_change_station,
	.add_station	= mtk_cfg80211_add_station,
	.del_station	= mtk_cfg80211_del_station,
#endif
	.scan = mtk_cfg80211_scan,
#if KERNEL_VERSION(4, 5, 0) <= CFG80211_VERSION_CODE
	.abort_scan = mtk_cfg80211_abort_scan,
#endif
	.connect = mtk_cfg80211_connect,
#if CFG_SUPPORT_CFG80211_AUTH
	.deauth	  = mtk_cfg80211_deauth,
	.disassoc = mtk_cfg80211_disassoc,
#endif
	.disconnect		= mtk_cfg80211_disconnect,
	.join_ibss		= mtk_cfg80211_join_ibss,
	.leave_ibss		= mtk_cfg80211_leave_ibss,
	.set_power_mgmt = mtk_cfg80211_set_power_mgmt,
	.set_pmksa		= mtk_cfg80211_set_pmksa,
	.del_pmksa		= mtk_cfg80211_del_pmksa,
	.flush_pmksa	= mtk_cfg80211_flush_pmksa,
#if CONFIG_SUPPORT_GTK_REKEY
	.set_rekey_data = mtk_cfg80211_set_rekey_data,
#endif
	.set_wakeup = mtk_cfg80211_set_wakeup,
	.suspend	= mtk_cfg80211_suspend,
	.resume		= CFG80211_Resume,
#if CFG_SUPPORT_CFG80211_AUTH
	.auth = mtk_cfg80211_auth,
#endif
	.assoc = mtk_cfg80211_assoc,

	/* Action Frame TX/RX */
	.remain_on_channel		  = mtk_cfg80211_remain_on_channel,
	.cancel_remain_on_channel = mtk_cfg80211_cancel_remain_on_channel,
	.mgmt_tx				  = mtk_cfg80211_mgmt_tx,
	/* .mgmt_tx_cancel_wait        = mtk_cfg80211_mgmt_tx_cancel_wait, */
	.mgmt_frame_register = mtk_cfg80211_mgmt_frame_register,

#ifdef CONFIG_NL80211_TESTMODE
	.testmode_cmd = mtk_cfg80211_testmode_cmd,
#endif

#if CFG_SUPPORT_TDLS
	.tdls_oper = mtk_cfg80211_tdls_oper,
	.tdls_mgmt = mtk_cfg80211_tdls_mgmt,
#endif
};

#if KERNEL_VERSION(3, 18, 0) <= CFG80211_VERSION_CODE

static const struct wiphy_vendor_command mtk_wlan_vendor_ops[] = {
	{ { .vendor_id = GOOGLE_OUI, .subcmd = WIFI_SUBCMD_GET_CHANNEL_LIST },
			.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
			.doit  = mtk_cfg80211_vendor_get_channel_list, VENDOR_OPS_SET_POLICY(VENDOR_CMD_RAW_DATA) },
	{ { .vendor_id = GOOGLE_OUI, .subcmd = WIFI_SUBCMD_SET_COUNTRY_CODE },
			.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
			.doit  = mtk_cfg80211_vendor_set_country_code, VENDOR_OPS_SET_POLICY(VENDOR_CMD_RAW_DATA) },

	/* Get Driver Version or Firmware Version */
	{ { .vendor_id = GOOGLE_OUI, .subcmd = LOGGER_GET_VER },
			.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV, .doit = mtk_cfg80211_vendor_get_version,
			VENDOR_OPS_SET_POLICY(VENDOR_CMD_RAW_DATA) },
	/* Get Supported Feature Set */
	{ { .vendor_id = GOOGLE_OUI, .subcmd = WIFI_SUBCMD_GET_FEATURE_SET },
			.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
			.doit  = mtk_cfg80211_vendor_get_supported_feature_set, VENDOR_OPS_SET_POLICY(VENDOR_CMD_RAW_DATA) },
};

static const struct nl80211_vendor_cmd_info mtk_wlan_vendor_events[] = {
	{ .vendor_id = GOOGLE_OUI, .subcmd = GSCAN_EVENT_SIGNIFICANT_CHANGE_RESULTS },
	{ .vendor_id = GOOGLE_OUI, .subcmd = GSCAN_EVENT_HOTLIST_RESULTS_FOUND },
	{ .vendor_id = GOOGLE_OUI, .subcmd = GSCAN_EVENT_SCAN_RESULTS_AVAILABLE },
	{ .vendor_id = GOOGLE_OUI, .subcmd = GSCAN_EVENT_FULL_SCAN_RESULTS },
	{ .vendor_id = GOOGLE_OUI, .subcmd = RTT_EVENT_COMPLETE },
	{ .vendor_id = GOOGLE_OUI, .subcmd = GSCAN_EVENT_COMPLETE_SCAN },
	{ .vendor_id = GOOGLE_OUI, .subcmd = GSCAN_EVENT_HOTLIST_RESULTS_LOST },
};
#endif

/* There isn't a lot of sense in it, but you can transmit anything you like */
static const struct ieee80211_txrx_stypes
		mtk_cfg80211_ais_default_mgmt_stypes[NUM_NL80211_IFTYPES] = {
			[NL80211_IFTYPE_ADHOC] = {
					.tx = 0xffff,
					.rx = BIT(IEEE80211_STYPE_ACTION >> 4) },
			[NL80211_IFTYPE_STATION]	= { .tx = 0xffff, .rx = BIT(IEEE80211_STYPE_ACTION >> 4) | BIT(IEEE80211_STYPE_PROBE_REQ >> 4) | BIT(IEEE80211_STYPE_BEACON >> 4) },
			[NL80211_IFTYPE_AP]			= { .tx = 0xffff, .rx = BIT(IEEE80211_STYPE_PROBE_REQ >> 4) | BIT(IEEE80211_STYPE_ACTION >> 4) },
			[NL80211_IFTYPE_AP_VLAN]	= { /* copy AP */
						.tx = 0xffff,
						.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) | BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) | BIT(IEEE80211_STYPE_PROBE_REQ >> 4) | BIT(IEEE80211_STYPE_DISASSOC >> 4) | BIT(IEEE80211_STYPE_AUTH >> 4) | BIT(IEEE80211_STYPE_DEAUTH >> 4) | BIT(IEEE80211_STYPE_ACTION >> 4) },
			[NL80211_IFTYPE_P2P_CLIENT] = { .tx = 0xffff, .rx = BIT(IEEE80211_STYPE_ACTION >> 4) | BIT(IEEE80211_STYPE_PROBE_REQ >> 4) },
			[NL80211_IFTYPE_P2P_GO]		= { .tx = 0xffff, .rx = BIT(IEEE80211_STYPE_PROBE_REQ >> 4) | BIT(IEEE80211_STYPE_ACTION >> 4) }
		};

#ifdef CONFIG_PM
static const struct wiphy_wowlan_support mtk_wlan_wowlan_support = {
	.flags = WIPHY_WOWLAN_MAGIC_PKT | WIPHY_WOWLAN_DISCONNECT | WIPHY_WOWLAN_ANY,
};
#endif

#ifdef STA_P2P_MCC
static const struct ieee80211_iface_limit iface_limits_mcc[] = { { .max = 2,
		.types = (BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_ADHOC) | BIT(NL80211_IFTYPE_P2P_CLIENT)) } };

static const struct ieee80211_iface_combination iface_comb_mcc[] = { {
		.limits					= iface_limits_mcc,
		.n_limits				= ARRAY_SIZE(iface_limits_mcc),
		.max_interfaces			= 2,
		.num_different_channels = 2,
		.beacon_int_infra_match = false,
} };

#endif

#ifdef CONFIG_PM_SLEEP
struct notifier_block pm_resume_notifier_func = {
	.notifier_call = kalPmResumeHandler,
	.priority	   = 0,
};
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

#if KERNEL_VERSION(5, 2, 0) <= LINUX_VERSION_CODE
u16 wlanSelectQueue(struct net_device *dev, struct sk_buff *skb, struct net_device *sb_dev)
#elif KERNEL_VERSION(4, 19, 0) <= LINUX_VERSION_CODE
u16 wlanSelectQueue(
		struct net_device *dev, struct sk_buff *skb, struct net_device *sb_dev, select_queue_fallback_t fallback)
#elif KERNEL_VERSION(3, 14, 0) <= LINUX_VERSION_CODE
u16 wlanSelectQueue(struct net_device *dev, struct sk_buff *skb, void *accel_priv, select_queue_fallback_t fallback)
#elif KERNEL_VERSION(3, 13, 0) <= LINUX_VERSION_CODE
u16 wlanSelectQueue(struct net_device *dev, struct sk_buff *skb, void *accel_priv)
#else
u16 wlanSelectQueue(struct net_device *dev, struct sk_buff *skb)
#endif
{
	return mtk_wlan_ndev_select_queue(skb);
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief Load NVRAM data and translate it into REG_INFO_T
 *
 * \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
 * \param[out] prRegInfo  Pointer to struct REG_INFO_T
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
static void glLoadNvram(IN P_GLUE_INFO_T prGlueInfo, OUT P_REG_INFO_T prRegInfo)
{
	UINT_32 i, j;
	UINT_8	aucTmp[2];
	PUINT_8 pucDest;

	ASSERT(prGlueInfo);
	ASSERT(prRegInfo);

	if ((!prGlueInfo) || (!prRegInfo))
		return;

	if (kalCfgDataRead16(prGlueInfo, sizeof(WIFI_CFG_PARAM_STRUCT) - sizeof(UINT_16), (PUINT_16)aucTmp) == TRUE) {
		prGlueInfo->fgNvramAvailable = TRUE;

		/* load MAC Address */
		for (i = 0; i < PARAM_MAC_ADDR_LEN; i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucMacAddress) + i,
					(PUINT_16)(((PUINT_8)prRegInfo->aucMacAddr) + i));
		}

		/* load country code */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucCountryCode[0]), (PUINT_16)aucTmp);

		/* cast to wide characters */
		prRegInfo->au2CountryCode[0] = (UINT_16)aucTmp[0];
		prRegInfo->au2CountryCode[1] = (UINT_16)aucTmp[1];

		/* load default normal TX power */
		for (i = 0; i < sizeof(TX_PWR_PARAM_T); i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, rTxPwr) + i,
					(PUINT_16)(((PUINT_8) & (prRegInfo->rTxPwr)) + i));
		}

		/* load feature flags */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, ucTxPwrValid), (PUINT_16)aucTmp);
		prRegInfo->ucTxPwrValid	   = aucTmp[0];
		prRegInfo->ucSupport5GBand = aucTmp[1];

		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, uc2G4BwFixed20M), (PUINT_16)aucTmp);
		prRegInfo->uc2G4BwFixed20M = aucTmp[0];
		prRegInfo->uc5GBwFixed20M  = aucTmp[1];

		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, ucEnable5GBand), (PUINT_16)aucTmp);
		prRegInfo->ucEnable5GBand = aucTmp[0];
		prRegInfo->ucRxDiversity  = aucTmp[1];

		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, fgRssiCompensationVaildbit), (PUINT_16)aucTmp);
		prRegInfo->ucRssiPathCompasationUsed = aucTmp[0];
		prRegInfo->ucGpsDesense				 = aucTmp[1];

#if CFG_SUPPORT_NVRAM_5G
		/* load EFUSE overriding part */
		for (i = 0; i < sizeof(prRegInfo->aucEFUSE); i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, EfuseMapping) + i,
					(PUINT_16)(((PUINT_8) & (prRegInfo->aucEFUSE)) + i));
		}

		prRegInfo->prOldEfuseMapping = (P_NEW_EFUSE_MAPPING2NVRAM_T)&prRegInfo->aucEFUSE;
#else

		/* load EFUSE overriding part */
		for (i = 0; i < sizeof(prRegInfo->aucEFUSE); i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucEFUSE) + i,
					(PUINT_16)(((PUINT_8) & (prRegInfo->aucEFUSE)) + i));
		}
#endif

		/* load band edge tx power control */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, fg2G4BandEdgePwrUsed), (PUINT_16)aucTmp);
		prRegInfo->fg2G4BandEdgePwrUsed = (BOOLEAN)aucTmp[0];
		if (aucTmp[0]) {
			prRegInfo->cBandEdgeMaxPwrCCK = (INT_8)aucTmp[1];

			kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, cBandEdgeMaxPwrOFDM20), (PUINT_16)aucTmp);
			prRegInfo->cBandEdgeMaxPwrOFDM20 = (INT_8)aucTmp[0];
			prRegInfo->cBandEdgeMaxPwrOFDM40 = (INT_8)aucTmp[1];
		}

		/* load regulation subbands */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, ucRegChannelListMap), (PUINT_16)aucTmp);
		prRegInfo->eRegChannelListMap	 = (ENUM_REG_CH_MAP_T)aucTmp[0];
		prRegInfo->ucRegChannelListIndex = aucTmp[1];

		if (prRegInfo->eRegChannelListMap == REG_CH_MAP_CUSTOMIZED) {
			for (i = 0; i < MAX_SUBBAND_NUM; i++) {
				pucDest = (PUINT_8)&prRegInfo->rDomainInfo.rSubBand[i];
				for (j = 0; j < 6; j += sizeof(UINT_16)) {
					kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucRegSubbandInfo) + (i * 6 + j),
							(PUINT_16)aucTmp);

					*pucDest++ = aucTmp[0];
					*pucDest++ = aucTmp[1];
				}
			}
		}

		/* load rssiPathCompensation */
		for (i = 0; i < sizeof(RSSI_PATH_COMPASATION_T); i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, rRssiPathCompensation) + i,
					(PUINT_16)(((PUINT_8) & (prRegInfo->rRssiPathCompasation)) + i));
		}

		/* load full NVRAM */
		for (i = 0; i < sizeof(WIFI_CFG_PARAM_STRUCT); i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, u2Part1OwnVersion) + i,
					(PUINT_16)(((PUINT_8) & (prRegInfo->aucNvram)) + i));
		}
		prRegInfo->prNvramSettings = (P_WIFI_CFG_PARAM_STRUCT)&prRegInfo->aucNvram;

	} else {
		DBGLOG(INIT, INFO, "glLoadNvram fail\n");
		prGlueInfo->fgNvramAvailable = FALSE;
	}
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief Release prDev from wlandev_array and free tasklet object related to it.
 *
 * \param[in] prDev  Pointer to struct net_device
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
static void wlanClearDevIdx(struct net_device *prDev)
{
	int i;

	ASSERT(prDev);

	for (i = 0; i < CFG_MAX_WLAN_DEVICES; i++) {
		if (arWlanDevInfo[i].prDev == prDev) {
			arWlanDevInfo[i].prDev = NULL;
			u4WlanDevNum--;
		}
	}

} /* end of wlanClearDevIdx() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Allocate an unique interface index, net_device::ifindex member for this
 *        wlan device. Store the net_device in wlandev_array, and initialize
 *        tasklet object related to it.
 *
 * \param[in] prDev  Pointer to struct net_device
 *
 * \retval >= 0      The device number.
 * \retval -1        Fail to get index.
 */
/*----------------------------------------------------------------------------*/
static int wlanGetDevIdx(struct net_device *prDev)
{
	int i;

	ASSERT(prDev);

	for (i = 0; i < CFG_MAX_WLAN_DEVICES; i++) {
		if (arWlanDevInfo[i].prDev == (struct net_device *)NULL) {
			/* Reserve 2 bytes space to store one digit of
			 * device number and NULL terminator.
			 */
			arWlanDevInfo[i].prDev = prDev;
			u4WlanDevNum++;
			return i;
		}
	}

	return -1;
} /* end of wlanGetDevIdx() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief A method of struct net_device, a primary SOCKET interface to configure
 *        the interface lively. Handle an ioctl call on one of our devices.
 *        Everything Linux ioctl specific is done here. Then we pass the contents
 *        of the ifr->data to the request message handler.
 *
 * \param[in] prDev      Linux kernel netdevice
 *
 * \param[in] prIFReq    Our private ioctl request structure, typed for the generic
 *                       struct ifreq so we can use ptr to function
 *
 * \param[in] cmd        Command ID
 *
 * \retval WLAN_STATUS_SUCCESS The IOCTL command is executed successfully.
 * \retval OTHER The execution of IOCTL command is failed.
 */
/*----------------------------------------------------------------------------*/
int wlanDoIOCTL(struct net_device *prDev, struct ifreq *prIfReq, int i4Cmd)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	int			  ret		 = 0;

	/* Verify input parameters for the following functions */
	ASSERT(prDev && prIfReq);
	if (!prDev || !prIfReq) {
		DBGLOG(INIT, ERROR, "Invalid input data\n");
		return -EINVAL;
	}

	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(prDev));
	if (!prGlueInfo) {
		DBGLOG(INIT, ERROR, "prGlueInfo is NULL\n");
		return -EFAULT;
	}

	if (prGlueInfo->u4ReadyFlag == 0) {
		DBGLOG(INIT, ERROR, "Adapter is not ready\n");
		return -EINVAL;
	}

	if ((i4Cmd >= SIOCIWFIRST) && (i4Cmd < SIOCIWFIRSTPRIV)) {
		/* 0x8B00 ~ 0x8BDF, wireless extension region */
		ret = wext_support_ioctl(prDev, prIfReq, i4Cmd);
	} else if ((i4Cmd >= SIOCIWFIRSTPRIV) && (i4Cmd < SIOCIWLASTPRIV)) {
		/* 0x8BE0 ~ 0x8BFF, private ioctl region */
		ret = priv_support_ioctl(prDev, prIfReq, i4Cmd);
	} else if (i4Cmd == SIOCDEVPRIVATE + 1) {
#ifdef CFG_ANDROID_AOSP_PRIV_CMD
		ret = android_private_support_driver_cmd(prDev, prIfReq, i4Cmd);
#else
		ret = priv_support_driver_cmd(prDev, prIfReq, i4Cmd);
#endif /* CFG_ANDROID_AOSP_PRIV_CMD */
	} else {
		DBGLOG(INIT, WARN, "Unexpected ioctl command: 0x%04x\n", i4Cmd);
		ret = -EOPNOTSUPP;
	}

	return ret;
} /* end of wlanDoIOCTL() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Export wlan GLUE_INFO_T pointer to p2p module
 *
 * \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
 *
 * \return TRUE: get GlueInfo pointer successfully
 *            FALSE: wlan is not started yet
 */
/*---------------------------------------------------------------------------*/
P_GLUE_INFO_T wlanGetGlueInfo(VOID)
{
	struct net_device *prDev	  = NULL;
	P_GLUE_INFO_T	   prGlueInfo = NULL;

	if (u4WlanDevNum == 0)
		return NULL;

	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;
	if (prDev == NULL)
		return NULL;

	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(prDev));

	return prGlueInfo;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief This function is to set multicast list and set rx mode.
 *
 * \param[in] prDev  Pointer to struct net_device
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/

static struct delayed_work workq;
struct net_device		  *gPrDev;

static void wlanSetMulticastList(struct net_device *prDev)
{
	/* Allow to receive all multicast for WOW */
	DBGLOG(INIT, INFO, "wlanSetMulticastList\n");
	prDev->flags |= (IFF_MULTICAST | IFF_ALLMULTI);
	gPrDev = prDev;
	schedule_delayed_work(&workq, 0);
}

/* FIXME: Since we cannot sleep in the wlanSetMulticastList, we arrange
 * another workqueue for sleeping. We don't want to block
 * main_thread, so we can't let tx_thread to do this
 */

static void wlanSetMulticastListWorkQueue(struct work_struct *work)
{
	P_GLUE_INFO_T	   prGlueInfo	  = NULL;
	UINT_32			   u4PacketFilter = 0;
	UINT_32			   u4SetInfoLen;
	struct net_device *prDev = gPrDev;

	down(&g_halt_sem);
	if (g_u4HaltFlag) {
		up(&g_halt_sem);
		return;
	}

	prGlueInfo = (prDev != NULL) ? *((P_GLUE_INFO_T *)netdev_priv(prDev)) : NULL;
	ASSERT(prDev);
	ASSERT(prGlueInfo);
	if (!prDev || !prGlueInfo) {
		DBGLOG(INIT, WARN, "abnormal dev or skb: prDev(0x%p), prGlueInfo(0x%p)\n", prDev, prGlueInfo);
		up(&g_halt_sem);
		return;
	}

	DBGLOG(INIT, INFO, "wlanSetMulticastListWorkQueue prDev->flags:0x%x\n", prDev->flags);

	if (prDev->flags & IFF_PROMISC)
		u4PacketFilter |= PARAM_PACKET_FILTER_PROMISCUOUS;

	if (prDev->flags & IFF_BROADCAST)
		u4PacketFilter |= PARAM_PACKET_FILTER_BROADCAST;

	if (prDev->flags & IFF_MULTICAST) {
		if ((prDev->flags & IFF_ALLMULTI) || (netdev_mc_count(prDev) > MAX_NUM_GROUP_ADDR))
			u4PacketFilter |= PARAM_PACKET_FILTER_ALL_MULTICAST;
		else
			u4PacketFilter |= PARAM_PACKET_FILTER_MULTICAST;
	}

	up(&g_halt_sem);

	if (kalIoctl(prGlueInfo, wlanoidSetCurrentPacketFilter, &u4PacketFilter, sizeof(u4PacketFilter), FALSE, FALSE, TRUE,
				&u4SetInfoLen) != WLAN_STATUS_SUCCESS) {
		return;
	}

	if (u4PacketFilter & PARAM_PACKET_FILTER_MULTICAST) {
		/* Prepare multicast address list */
		struct netdev_hw_addr *ha;
		PUINT_8				   prMCAddrList = NULL;
		UINT_32				   i			= 0;

		down(&g_halt_sem);
		if (g_u4HaltFlag) {
			up(&g_halt_sem);
			return;
		}

		prMCAddrList = kalMemAlloc(MAX_NUM_GROUP_ADDR * ETH_ALEN, VIR_MEM_TYPE);

		netdev_for_each_mc_addr(ha, prDev) {
			if (i < MAX_NUM_GROUP_ADDR) {
				kalMemCopy((prMCAddrList + i * ETH_ALEN), GET_ADDR(ha), ETH_ALEN);
				i++;
			}
		}

		up(&g_halt_sem);

		kalIoctl(prGlueInfo, wlanoidSetMulticastList, prMCAddrList, (i * ETH_ALEN), FALSE, FALSE, TRUE, &u4SetInfoLen);

		kalMemFree(prMCAddrList, VIR_MEM_TYPE, MAX_NUM_GROUP_ADDR * ETH_ALEN);
	}

} /* end of wlanSetMulticastList() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief    To indicate scheduled scan has been stopped
 *
 * \param[in]
 *           prGlueInfo
 *
 * \return
 *           None
 */
/*----------------------------------------------------------------------------*/
VOID wlanSchedScanStoppedWorkQueue(struct work_struct *work)
{
	P_GLUE_INFO_T	   prGlueInfo = NULL;
	struct net_device *prDev	  = gPrDev;

	DBGLOG(SCN, INFO, "wlanSchedScanStoppedWorkQueue\n");
	prGlueInfo = (prDev != NULL) ? *((P_GLUE_INFO_T *)netdev_priv(prDev)) : NULL;
	if (!prGlueInfo) {
		DBGLOG(SCN, INFO, "prGlueInfo == NULL unexpected\n");
		return;
	}

	/* 2. indication to cfg80211 */
	/* 20150205 change cfg80211_sched_scan_stopped to work queue due to sched_scan_mtx dead lock issue */
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
	cfg80211_sched_scan_stopped(priv_to_wiphy(prGlueInfo), prGlueInfo->prSchedScanRequest->reqid);
#else
	cfg80211_sched_scan_stopped(priv_to_wiphy(prGlueInfo));
#endif
	DBGLOG(SCN, INFO,
			"cfg80211_sched_scan_stopped event send done WorkQueue thread return from wlanSchedScanStoppedWorkQueue\n");
	return;
}

#if CFG_SUPPORT_CFG80211_AUTH
#if CFG_WDEV_LOCK_THREAD_SUPPORT
VOID wlanSchedWDevLockWorkQueue(struct work_struct *work)
{
	P_GLUE_INFO_T			   prGlueInfo	   = NULL;
	struct net_device		  *prDev		   = gPrDev;
	P_PARAM_WDEV_LOCK_THREAD_T prParamWDevLock = NULL;
	QUE_T					   rTempQue;
	P_QUE_T					   prTempQue = &rTempQue;

	GLUE_SPIN_LOCK_DECLARATION();

	QUEUE_INITIALIZE(prTempQue);

	DBGLOG(REQ, INFO, "wlanSchedWDevLockWorkQueue\n");
	prGlueInfo = (prDev != NULL) ? *((P_GLUE_INFO_T *)netdev_priv(prDev)) : NULL;
	if (!prGlueInfo) {
		DBGLOG(REQ, ERROR, "prGlueInfo == NULL unexpected\n");
		return;
	}

	if (prGlueInfo->u4ReadyFlag == 0) {
		DBGLOG(REQ, ERROR, "Adapter is not ready\n");
		return;
	}

	while (QUEUE_IS_NOT_EMPTY(&prGlueInfo->prAdapter->rWDevLockQueue)) {
		GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_WDEV_LOCK);
		QUEUE_MOVE_ALL(prTempQue, &prGlueInfo->prAdapter->rWDevLockQueue);
		GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_WDEV_LOCK);

		while (QUEUE_IS_NOT_EMPTY(prTempQue)) {
			QUEUE_REMOVE_HEAD(prTempQue, prParamWDevLock, P_PARAM_WDEV_LOCK_THREAD_T);

			if (prParamWDevLock == NULL)
				break;

			kalAcquireWDevMutex(prParamWDevLock->pDev);
			switch (prParamWDevLock->fn) {
			case CFG80211_RX_ASSOC_RESP:
#if (KERNEL_VERSION(5, 1, 0) <= CFG80211_VERSION_CODE)
				cfg80211_rx_assoc_resp(prParamWDevLock->pDev, prParamWDevLock->pBss, prParamWDevLock->pFrameBuf,
						prParamWDevLock->frameLen, prParamWDevLock->uapsd_queues, NULL, 0);
				/*!FIXME
				 * prParamWDevLock->req_ies,
				 * prParamWDevLock->req_ies_len);
				 */
#elif (KERNEL_VERSION(3, 18, 0) <= CFG80211_VERSION_CODE)
				cfg80211_rx_assoc_resp(prParamWDevLock->pDev, prParamWDevLock->pBss, prParamWDevLock->pFrameBuf,
						prParamWDevLock->frameLen, prParamWDevLock->uapsd_queues);
#elif (KERNEL_VERSION(3, 11, 0) <= CFG80211_VERSION_CODE)
				cfg80211_rx_assoc_resp(prParamWDevLock->pDev, prParamWDevLock->pBss, prParamWDevLock->pFrameBuf,
						prParamWDevLock->frameLen);
#else
				cfg80211_send_rx_assoc(prParamWDevLock->pDev, prParamWDevLock->pBss, prParamWDevLock->pFrameBuf,
						prParamWDevLock->frameLen);
#endif
				break;

			case CFG80211_RX_MLME_MGMT:
				cfg80211_rx_mlme_mgmt(prParamWDevLock->pDev, prParamWDevLock->pFrameBuf, prParamWDevLock->frameLen);
				break;

			case CFG80211_TX_MLME_MGMT:
				cfg80211_tx_mlme_mgmt(prParamWDevLock->pDev, prParamWDevLock->pFrameBuf, prParamWDevLock->frameLen);
				break;
			case CFG80211_ABANDON_ASSOC:
#if (KERNEL_VERSION(4, 4, 41) <= CFG80211_VERSION_CODE)
				cfg80211_abandon_assoc(prParamWDevLock->pDev, prParamWDevLock->pBss);
				break;
#endif
			/* For kernel < 4.4.41,
			 * fall through to use assoc timeout
			 */
			case CFG80211_ASSOC_TIMEOUT:
#if (KERNEL_VERSION(3, 11, 0) <= CFG80211_VERSION_CODE)
				cfg80211_assoc_timeout(prParamWDevLock->pDev, prParamWDevLock->pBss);
#else
				cfg80211_send_assoc_timeout(prParamWDevLock->pDev, prParamWDevLock->pBss->bssid);
#endif

				break;
			}
			kalReleaseWDevMutex(prParamWDevLock->pDev);

			if (prParamWDevLock->pFrameBuf) {
				DBGLOG(REQ, TRACE, "Free pFrameBuf 0x%x\n", prParamWDevLock->pFrameBuf);
				if (prParamWDevLock->fgIsInterruptContext) {
					kalMemFree(prParamWDevLock->pFrameBuf, PHY_MEM_TYPE, prParamWDevLock->u4InfoBufLen);
				} else {
					kalMemFree(prParamWDevLock->pFrameBuf, VIR_MEM_TYPE, prParamWDevLock->u4InfoBufLen);
				}

				prParamWDevLock->pFrameBuf = NULL;
			}

			DBGLOG(REQ, TRACE, "Release cfg80211_bss\n");
			if (prParamWDevLock->pBss) {
				cfg80211_put_bss(priv_to_wiphy(prGlueInfo), prParamWDevLock->pBss);
			}

			DBGLOG(REQ, TRACE, "Free prParamWDevLock- 0x%x\n", prParamWDevLock);

			if (prParamWDevLock->fgIsInterruptContext) {
				kalMemFree(prParamWDevLock, PHY_MEM_TYPE, sizeof(PARAM_WDEV_LOCK_THREAD));
			} else {
				kalMemFree(prParamWDevLock, VIR_MEM_TYPE, sizeof(PARAM_WDEV_LOCK_THREAD));
			}
		}
	}

	return;
}
#endif
#endif

/* FIXME: Since we cannot sleep in the wlanSetMulticastList, we arrange
 * another workqueue for sleeping. We don't want to block
 * main_thread, so we can't let tx_thread to do this
 */

void p2pSetMulticastListWorkQueueWrapper(P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);

	if (!prGlueInfo) {
		DBGLOG(INIT, WARN, "abnormal dev or skb: prGlueInfo(0x%p)\n", prGlueInfo);
		return;
	}
#if CFG_ENABLE_WIFI_DIRECT
	if (prGlueInfo->prAdapter->fgIsP2PRegistered)
		mtk_p2p_wext_set_Multicastlist(prGlueInfo);
#endif

} /* end of p2pSetMulticastListWorkQueueWrapper() */

/*----------------------------------------------------------------------------*/
/*
 * \brief This function is TX entry point of NET DEVICE.
 *
 * \param[in] prSkb  Pointer of the sk_buff to be sent
 * \param[in] prDev  Pointer to struct net_device
 *
 * \retval NETDEV_TX_OK - on success.
 * \retval NETDEV_TX_BUSY - on failure, packet will be discarded by upper layer.
 */
/*----------------------------------------------------------------------------*/
int wlanHardStartXmit(struct sk_buff *prSkb, struct net_device *prDev)
{
	P_NETDEV_PRIVATE_GLUE_INFO prNetDevPrivate = (P_NETDEV_PRIVATE_GLUE_INFO)NULL;
	P_GLUE_INFO_T			   prGlueInfo	   = *((P_GLUE_INFO_T *)netdev_priv(prDev));
	UINT_8					   ucBssIndex;

	ASSERT(prSkb);
	ASSERT(prDev);
	ASSERT(prGlueInfo);

	prNetDevPrivate = (P_NETDEV_PRIVATE_GLUE_INFO)netdev_priv(prDev);
	ASSERT(prNetDevPrivate->prGlueInfo == prGlueInfo);
	ucBssIndex = prNetDevPrivate->ucBssIdx;

	kalResetPacket(prGlueInfo, (P_NATIVE_PACKET)prSkb);

	if (kalHardStartXmit(prSkb, prDev, prGlueInfo, ucBssIndex) == WLAN_STATUS_SUCCESS) {
		/* Successfully enqueue to Tx queue */
		/* Successfully enqueue to Tx queue */
	}

	/* For Linux, we'll always return OK FLAG, because we'll free this skb by ourself */
	return NETDEV_TX_OK;
} /* end of wlanHardStartXmit() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief A method of struct net_device, to get the network interface statistical
 *        information.
 *
 * Whenever an application needs to get statistics for the interface, this method
 * is called. This happens, for example, when ifconfig or netstat -i is run.
 *
 * \param[in] prDev      Pointer to struct net_device.
 *
 * \return net_device_stats buffer pointer.
 */
/*----------------------------------------------------------------------------*/
struct net_device_stats *wlanGetStats(IN struct net_device *prDev)
{
	return (struct net_device_stats *)kalGetStats(prDev);
} /* end of wlanGetStats() */

VOID wlanDebugInit(VOID)
{
	UINT_8 i;

	/* Set the initial debug level of each module */
#if DBG
	for (i = 0; i < DBG_MODULE_NUM; i++)
		aucDebugModule[i] = DBG_CLASS_MASK; /* enable all */
#else
#ifdef CFG_DEFAULT_DBG_LEVEL
	for (i = 0; i < DBG_MODULE_NUM; i++)
		aucDebugModule[i] = CFG_DEFAULT_DBG_LEVEL;

	aucDebugModule[DBG_QM_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_INFO | DBG_CLASS_WARN);
	aucDebugModule[DBG_RSN_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_INFO | DBG_CLASS_WARN);
#else

	for (i = 0; i < DBG_MODULE_NUM; i++) {
		aucDebugModule[i] = DBG_CLASS_ERROR | DBG_CLASS_WARN | DBG_CLASS_STATE | DBG_CLASS_EVENT | DBG_CLASS_INFO;
	}
	aucDebugModule[DBG_TX_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_INFO);
	aucDebugModule[DBG_RX_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_INFO);
	aucDebugModule[DBG_REQ_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_INFO);
	aucDebugModule[DBG_INTR_IDX] = 0;
	aucDebugModule[DBG_MEM_IDX]	 = DBG_CLASS_ERROR | DBG_CLASS_WARN;
	aucDebugModule[DBG_REQ_IDX]	 = DBG_CLASS_MASK;

#endif
#endif /* DBG */

	DBGLOG(INIT, INFO, "Reset ALL DBG module log level to DEFAULT!\n");
}

WLAN_STATUS wlanSetDebugLevel(IN UINT_32 u4DbgIdx, IN UINT_32 u4DbgMask)
{
	UINT_32		u4Idx;
	WLAN_STATUS fgStatus = WLAN_STATUS_SUCCESS;

	if (u4DbgIdx == DBG_ALL_MODULE_IDX) {
		for (u4Idx = 0; u4Idx < DBG_MODULE_NUM; u4Idx++)
			aucDebugModule[u4Idx] = (UINT_8)u4DbgMask;
		DBGLOG(INIT, INFO, "Set ALL DBG module log level to [0x%02x]\n", u4DbgMask);
	} else if (u4DbgIdx < DBG_MODULE_NUM) {
		aucDebugModule[u4DbgIdx] = (UINT_8)u4DbgMask;
		DBGLOG(INIT, INFO, "Set DBG module[%u] log level to [0x%02x]\n", u4DbgIdx, u4DbgMask);
	} else {
		fgStatus = WLAN_STATUS_FAILURE;
	}

	return fgStatus;
}

WLAN_STATUS wlanGetDebugLevel(IN UINT_32 u4DbgIdx, OUT PUINT_32 pu4DbgMask)
{
	if (u4DbgIdx < DBG_MODULE_NUM) {
		*pu4DbgMask = aucDebugModule[u4DbgIdx];
		return WLAN_STATUS_SUCCESS;
	}

	return WLAN_STATUS_FAILURE;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief A function for prDev->init
 *
 * \param[in] prDev      Pointer to struct net_device.
 *
 * \retval 0         The execution of wlanInit succeeds.
 * \retval -ENXIO    No such device.
 */
/*----------------------------------------------------------------------------*/
static int wlanInit(struct net_device *prDev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (!prDev)
		return -ENXIO;

	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(prDev));
	INIT_DELAYED_WORK(&workq, wlanSetMulticastListWorkQueue);
#if CFG_SUPPORT_CFG80211_AUTH
#if CFG_WDEV_LOCK_THREAD_SUPPORT
	INIT_DELAYED_WORK(&wdev_lock_workq, wlanSchedWDevLockWorkQueue);
#endif
#endif

	/* 20150205 work queue for sched_scan */
	INIT_DELAYED_WORK(&sched_workq, wlanSchedScanStoppedWorkQueue);

/* 20161024 init wow port setting */
#if CFG_WOW_SUPPORT
	kalWowInit(prGlueInfo);
#endif

	return 0; /* success */
} /* end of wlanInit() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief A function for prDev->uninit
 *
 * \param[in] prDev      Pointer to struct net_device.
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
static void wlanUninit(struct net_device *prDev)
{
} /* end of wlanUninit() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief A method of struct net_device, to set the randomized mac address
 *
 * This method is called before Wifi Framework requests a new conenction with
 * enabled feature "Connected Random Mac".
 *
 * \param[in] ndev  Pointer to struct net_device.
 * \param[in] addr  Randomized Mac address passed from WIFI framework.
 *
 * \return int.
 */
/*----------------------------------------------------------------------------*/
static int wlanSetMacAddress(struct net_device *ndev, void *addr)
{
	P_ADAPTER_T			 prAdapter	= NULL;
	P_GLUE_INFO_T		 prGlueInfo = NULL;
	struct sockaddr		*sa			= NULL;
	struct wireless_dev *wdev		= NULL;

	DBGLOG(INIT, STATE, "[%s]\n", __func__);
	/**********************************************************************
	 * Check if kernel passes valid data to us                            *
	 **********************************************************************
	 */
	if (!ndev || !addr) {
		DBGLOG(INIT, ERROR, "Set macaddr with ndev(%d) and addr(%d)\n", (ndev == NULL) ? 0 : 1, (addr == NULL) ? 0 : 1);
		return WLAN_STATUS_INVALID_DATA;
	}

	/**********************************************************************
	 * Block mac address changing if this setting is not for connection   *
	 **********************************************************************
	 */
	wdev = ndev->ieee80211_ptr;
	if (wdev->ssid_len > 0 || (wdev->current_bss)) {
		DBGLOG(INIT, ERROR, "Reject macaddr change due to ssid_len(%d) & bss(%d)\n", wdev->ssid_len,
				(wdev->current_bss == NULL) ? 0 : 1);
		return WLAN_STATUS_NOT_ACCEPTED;
	}

	/**********************************************************************
	 * 1. Change OwnMacAddr which will be updated to FW through           *
	 *    rlmActivateNetwork later.                                       *
	 * 2. Change dev_addr stored in kernel to notify framework that the   *
	 *    mac addr has been changed and what the new value is.            *
	 **********************************************************************
	 */
	sa		   = (struct sockaddr *)addr;
	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(ndev));
	prAdapter  = prGlueInfo->prAdapter;

	COPY_MAC_ADDR(prAdapter->prAisBssInfo->aucOwnMacAddr, sa->sa_data);
	COPY_MAC_ADDR(prGlueInfo->prDevHandler->dev_addr, sa->sa_data);
#if CFG_MESON_G12A_PATCH
	prGlueInfo->prDevHandler->mtu = 1408;
#endif
	DBGLOG(INIT, STATE, "Set connect random macaddr to " MACSTR ".\n", MAC2STR(prAdapter->prAisBssInfo->aucOwnMacAddr));

	return WLAN_STATUS_SUCCESS;
} /* end of wlanSetMacAddr() */

/*n---------------------------------------------------------------------------*/
/*!
 * \brief A function for prDev->open
 *
 * \param[in] prDev      Pointer to struct net_device.
 *
 * \retval 0     The execution of wlanOpen succeeds.
 * \retval < 0   The execution of wlanOpen failed.
 */
/*----------------------------------------------------------------------------*/
static int wlanOpen(struct net_device *prDev)
{
	ASSERT(prDev);

	netif_tx_start_all_queues(prDev);

	return 0; /* success */
} /* end of wlanOpen() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief A function for prDev->stop
 *
 * \param[in] prDev      Pointer to struct net_device.
 *
 * \retval 0     The execution of wlanStop succeeds.
 * \retval < 0   The execution of wlanStop failed.
 */
/*----------------------------------------------------------------------------*/
static int wlanStop(struct net_device *prDev)
{
	P_GLUE_INFO_T				  prGlueInfo	= NULL;
	struct cfg80211_scan_request *prScanRequest = NULL;

	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prDev);

	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(prDev));

	if ((!prGlueInfo) || (prGlueInfo->u4ReadyFlag == 0)) {
		DBGLOG(INIT, WARN, "driver is not ready\n");
	} else {
		/* CFG80211 down, report to kernel directly and run normal
		 * scan abort procedure
		 */
		GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV);
		if (prGlueInfo->prScanRequest) {
			DBGLOG(INIT, INFO, "wlanStop abort scan!\n");
			kalCfg80211ScanDone(prGlueInfo->prScanRequest, TRUE);
			aisFsmStateAbort_SCAN(prGlueInfo->prAdapter);
			prScanRequest			  = prGlueInfo->prScanRequest;
			prGlueInfo->prScanRequest = NULL;
		}
		GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV);
	}

#if CFG_AUTO_CHANNEL_SEL_SUPPORT
	/* zero clear old acs information */
	kalMemZero(&(prGlueInfo->prAdapter->rWifiVar.rChnLoadInfo), sizeof(prGlueInfo->prAdapter->rWifiVar.rChnLoadInfo));
#endif

	netif_tx_stop_all_queues(prDev);

	return 0; /* success */
} /* end of wlanStop() */

#if CFG_SUPPORT_SNIFFER
static int wlanMonOpen(struct net_device *prDev)
{
	ASSERT(prDev);

	netif_tx_start_all_queues(prDev);

	return 0; /* success */
}

static int wlanMonStop(struct net_device *prDev)
{
	ASSERT(prDev);

	netif_tx_stop_all_queues(prDev);

	return 0; /* success */
}

static const struct net_device_ops wlan_mon_netdev_ops = {
	.ndo_open = wlanMonOpen,
	.ndo_stop = wlanMonStop,
};

void wlanMonWorkHandler(struct work_struct *work)
{
	P_GLUE_INFO_T prGlueInfo;

	prGlueInfo = container_of(work, GLUE_INFO_T, monWork);

	if (prGlueInfo->fgIsEnableMon) {
		if (prGlueInfo->prMonDevHandler)
			return;
#if KERNEL_VERSION(3, 17, 0) <= CFG80211_VERSION_CODE
		prGlueInfo->prMonDevHandler = alloc_netdev_mq(sizeof(NETDEV_PRIVATE_GLUE_INFO), NIC_MONITOR_INF_NAME,
				NET_NAME_PREDICTABLE, ether_setup, CFG_MAX_TXQ_NUM);
#else
		prGlueInfo->prMonDevHandler =
				alloc_netdev_mq(sizeof(NETDEV_PRIVATE_GLUE_INFO), NIC_MONITOR_INF_NAME, ether_setup, CFG_MAX_TXQ_NUM);
#endif
		if (prGlueInfo->prMonDevHandler == NULL) {
			DBGLOG(INIT, ERROR, "wlanMonWorkHandler: Allocated prMonDevHandler context FAIL.\n");
			return;
		}

		((P_NETDEV_PRIVATE_GLUE_INFO)netdev_priv(prGlueInfo->prMonDevHandler))->prGlueInfo = prGlueInfo;
		prGlueInfo->prMonDevHandler->type												   = ARPHRD_IEEE80211_RADIOTAP;
		prGlueInfo->prMonDevHandler->netdev_ops											   = &wlan_mon_netdev_ops;
		netif_carrier_off(prGlueInfo->prMonDevHandler);
		netif_tx_stop_all_queues(prGlueInfo->prMonDevHandler);
		kalResetStats(prGlueInfo->prMonDevHandler);

		if (register_netdev(prGlueInfo->prMonDevHandler) < 0) {
			DBGLOG(INIT, ERROR, "wlanMonWorkHandler: Registered prMonDevHandler context FAIL.\n");
			free_netdev(prGlueInfo->prMonDevHandler);
			prGlueInfo->prMonDevHandler = NULL;
		}
		DBGLOG(INIT, INFO, "wlanMonWorkHandler: Registered prMonDevHandler context DONE.\n");
	} else {
		if (prGlueInfo->prMonDevHandler) {
			unregister_netdev(prGlueInfo->prMonDevHandler);
			prGlueInfo->prMonDevHandler = NULL;
			DBGLOG(INIT, INFO, "wlanMonWorkHandler: unRegistered prMonDevHandler context DONE.\n");
		}
	}
}
#endif

/*----------------------------------------------------------------------------*/
/*!
 * \brief Update Channel table for cfg80211 for Wi-Fi Direct based on current country code
 *
 * \param[in] prGlueInfo      Pointer to glue info
 *
 * \return   none
 */
/*----------------------------------------------------------------------------*/
VOID wlanUpdateChannelTable(P_GLUE_INFO_T prGlueInfo)
{
	UINT_8			  i, j;
	UINT_8			  ucNumOfChannel;
	RF_CHANNEL_INFO_T aucChannelList[ARRAY_SIZE(mtk_2ghz_channels) + ARRAY_SIZE(mtk_5ghz_channels)];

	/* 1. Disable all channels */
	for (i = 0; i < ARRAY_SIZE(mtk_2ghz_channels); i++) {
		mtk_2ghz_channels[i].flags |= IEEE80211_CHAN_DISABLED;
		mtk_2ghz_channels[i].orig_flags |= IEEE80211_CHAN_DISABLED;
	}

	for (i = 0; i < ARRAY_SIZE(mtk_5ghz_channels); i++) {
		mtk_5ghz_channels[i].flags |= IEEE80211_CHAN_DISABLED;
		mtk_5ghz_channels[i].orig_flags |= IEEE80211_CHAN_DISABLED;
	}

	/* 2. Get current domain channel list */
	rlmDomainGetChnlList(prGlueInfo->prAdapter, BAND_NULL, FALSE,
			ARRAY_SIZE(mtk_2ghz_channels) + ARRAY_SIZE(mtk_5ghz_channels), &ucNumOfChannel, aucChannelList);

	/* 3. Enable specific channel based on domain channel list */
	for (i = 0; i < ucNumOfChannel; i++) {
		switch (aucChannelList[i].eBand) {
		case BAND_2G4:
			for (j = 0; j < ARRAY_SIZE(mtk_2ghz_channels); j++) {
				if (mtk_2ghz_channels[j].hw_value == aucChannelList[i].ucChannelNum) {
					mtk_2ghz_channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
					mtk_2ghz_channels[j].orig_flags &= ~IEEE80211_CHAN_DISABLED;
					break;
				}
			}
			break;

		case BAND_5G:
			for (j = 0; j < ARRAY_SIZE(mtk_5ghz_channels); j++) {
				if (mtk_5ghz_channels[j].hw_value == aucChannelList[i].ucChannelNum) {
					mtk_5ghz_channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
					mtk_5ghz_channels[j].orig_flags &= ~IEEE80211_CHAN_DISABLED;
					break;
				}
			}
			break;

		default:
			DBGLOG(INIT, WARN, "Unknown band %d\n", aucChannelList[i].eBand);
			break;
		}
	}
}
#if CFG_SUPPORT_SAP_DFS_CHANNEL
void wlanUpdateDfsChannelTable(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucChannel)
{
	UINT_8			  i, j;
	UINT_8			  ucNumOfChannel;
	RF_CHANNEL_INFO_T aucChannelList[ARRAY_SIZE(mtk_5ghz_channels)];

	DBGLOG(INIT, TRACE, "ucChannel %u.\n", ucChannel);

	/* 1. Get current domain DFS channel list */
	rlmDomainGetDfsChnls(prGlueInfo->prAdapter, ARRAY_SIZE(mtk_5ghz_channels), &ucNumOfChannel, aucChannelList);

	/* 2. Enable specific channel based on domain channel list */
	for (i = 0; i < ucNumOfChannel; i++) {
		for (j = 0; j < ARRAY_SIZE(mtk_5ghz_channels); j++) {
			if (aucChannelList[i].ucChannelNum != mtk_5ghz_channels[j].hw_value)
				continue;

			if (aucChannelList[i].ucChannelNum == ucChannel) {
				mtk_5ghz_channels[j].dfs_state = NL80211_DFS_AVAILABLE;
				mtk_5ghz_channels[j].flags &= ~IEEE80211_CHAN_RADAR;
				mtk_5ghz_channels[j].orig_flags &= ~IEEE80211_CHAN_RADAR;
				DBGLOG(INIT, STATE, "ch (%d), force NL80211_DFS_AVAILABLE.\n", ucChannel);
			} else {
				mtk_5ghz_channels[j].dfs_state = NL80211_DFS_USABLE;
				mtk_5ghz_channels[j].flags |= IEEE80211_CHAN_RADAR;
				mtk_5ghz_channels[j].orig_flags |= IEEE80211_CHAN_RADAR;
				DBGLOG(INIT, TRACE, "ch (%d), force NL80211_DFS_USABLE.\n", aucChannelList[i].ucChannelNum);
			}
		}
	}
}
#endif
/*----------------------------------------------------------------------------*/
/*!
 * \brief Register the device to the kernel and return the index.
 *
 * \param[in] prDev      Pointer to struct net_device.
 *
 * \retval 0     The execution of wlanNetRegister succeeds.
 * \retval < 0   The execution of wlanNetRegister failed.
 */
/*----------------------------------------------------------------------------*/
static INT_32 wlanNetRegister(struct wireless_dev *prWdev)
{
	P_GLUE_INFO_T			   prGlueInfo;
	INT_32					   i4DevIdx		   = -1;
	P_NETDEV_PRIVATE_GLUE_INFO prNetDevPrivate = (P_NETDEV_PRIVATE_GLUE_INFO)NULL;

	ASSERT(prWdev);

	do {
		if (!prWdev)
			break;

		prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(prWdev->wiphy);
		i4DevIdx   = wlanGetDevIdx(prWdev->netdev);
		if (i4DevIdx < 0) {
			DBGLOG(INIT, ERROR, "net_device number exceeds!\n");
			break;
		}

		if (register_netdev(prWdev->netdev) < 0) {
			DBGLOG(INIT, ERROR, "Register net_device failed\n");
			wlanClearDevIdx(prWdev->netdev);
			i4DevIdx = -1;
		}
		prNetDevPrivate = (P_NETDEV_PRIVATE_GLUE_INFO)netdev_priv(prGlueInfo->prDevHandler);
		ASSERT(prNetDevPrivate->prGlueInfo == prGlueInfo);
		prNetDevPrivate->ucBssIdx = prGlueInfo->prAdapter->prAisBssInfo->ucBssIndex;
		wlanBindBssIdxToNetInterface(
				prGlueInfo, prGlueInfo->prAdapter->prAisBssInfo->ucBssIndex, (PVOID)prWdev->netdev);

		if (i4DevIdx != -1)
			prGlueInfo->fgIsRegistered = TRUE;
	} while (FALSE);

	return i4DevIdx; /* success */
} /* end of wlanNetRegister() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Unregister the device from the kernel
 *
 * \param[in] prWdev      Pointer to struct net_device.
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
static VOID wlanNetUnregister(struct wireless_dev *prWdev)
{
	P_GLUE_INFO_T prGlueInfo;

	if (!prWdev) {
		DBGLOG(INIT, ERROR, "wlanNetUnregister: The device context is NULL\n");
		return;
	}

	prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(prWdev->wiphy);

	wlanClearDevIdx(prWdev->netdev);
	unregister_netdev(prWdev->netdev);

	prGlueInfo->fgIsRegistered = FALSE;

#if CFG_SUPPORT_SNIFFER
	if (prGlueInfo->prMonDevHandler) {
		unregister_netdev(prGlueInfo->prMonDevHandler);
		prGlueInfo->prMonDevHandler = NULL;
	}
	prGlueInfo->fgIsEnableMon = FALSE;
#endif

} /* end of wlanNetUnregister() */

static const struct net_device_ops wlan_netdev_ops = { .ndo_open = wlanOpen,
	.ndo_stop													 = wlanStop,
	.ndo_set_rx_mode											 = wlanSetMulticastList,
	.ndo_get_stats												 = wlanGetStats,
	.ndo_do_ioctl												 = wlanDoIOCTL,
	.ndo_start_xmit												 = wlanHardStartXmit,
	.ndo_init													 = wlanInit,
	.ndo_uninit													 = wlanUninit,
	.ndo_select_queue											 = wlanSelectQueue,
	.ndo_set_mac_address										 = wlanSetMacAddress };

static void wlanCreateWirelessDevice(void)
{
	struct wiphy		 *prWiphy = NULL;
	struct wireless_dev *prWdev	 = NULL;

	/* 4 <1.1> Create wireless_dev */
	prWdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (!prWdev) {
		DBGLOG(INIT, ERROR, "Allocating memory to wireless_dev context failed\n");
		return;
	}
	/* 4 <1.2> Create wiphy */
	prWiphy = wiphy_new(&mtk_wlan_ops, sizeof(GLUE_INFO_T));
	if (!prWiphy) {
		DBGLOG(INIT, ERROR, "Allocating memory to wiphy device failed\n");
		goto free_wdev;
	}
	/* 4 <1.3> configure wireless_dev & wiphy */
	prWdev->iftype				  = NL80211_IFTYPE_STATION;
	prWiphy->iface_combinations	  = p_mtk_iface_combinations_sta;
	prWiphy->n_iface_combinations = mtk_iface_combinations_sta_num;
	prWiphy->max_scan_ssids		  = CFG_SCAN_SSID_MAX_NUM;
	prWiphy->max_scan_ie_len	  = 512;
	prWiphy->interface_modes	  = BIT(NL80211_IFTYPE_STATION) |
#ifdef STA_P2P_MCC
							   BIT(NL80211_IFTYPE_P2P_CLIENT) |
#endif
							   BIT(NL80211_IFTYPE_ADHOC);
	prWiphy->bands[KAL_BAND_2GHZ] = &mtk_band_2ghz;
	/* always assign 5Ghz bands here, if the chip is not support 5Ghz,
	 *  bands[KAL_BAND_5GHZ] will be assign to NULL
	 */
	prWiphy->bands[KAL_BAND_5GHZ] = &mtk_band_5ghz;
	prWiphy->signal_type		  = CFG80211_SIGNAL_TYPE_MBM;
	prWiphy->cipher_suites		  = (const u32 *)mtk_cipher_suites;
	prWiphy->n_cipher_suites	  = ARRAY_SIZE(mtk_cipher_suites);

	/* CFG80211_VERSION_CODE >= 3.3 */
	prWiphy->flags = WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;

	/* CFG80211_VERSION_CODE >= 3.2 */
#if (CFG_SUPPORT_ROAMING == 1)
	prWiphy->flags |= WIPHY_FLAG_SUPPORTS_FW_ROAM;
#endif /* CFG_SUPPORT_ROAMING */

#if KERNEL_VERSION(3, 14, 0) > CFG80211_VERSION_CODE
	prWiphy->flags |= WIPHY_FLAG_CUSTOM_REGULATORY;
#else
	prWiphy->regulatory_flags = REGULATORY_CUSTOM_REG;
#if (CFG_SUPPORT_DFS_MASTER == 1)
	prWiphy->flags |= WIPHY_FLAG_HAS_CHANNEL_SWITCH;
#endif /* CFG_SUPPORT_DFS_MASTER */
#endif
#if (CFG_SUPPORT_SAE == 1)
	prWiphy->features |= NL80211_FEATURE_SAE;
#endif /* CFG_SUPPORT_DFS_MASTER */

	cfg80211_regd_set_wiphy(prWiphy);

#if (CFG_SUPPORT_TDLS == 1)
	TDLSEX_WIPHY_FLAGS_INIT(prWiphy->flags);
#endif /* CFG_SUPPORT_TDLS */
	prWiphy->max_remain_on_channel_duration = 5000;
	prWiphy->mgmt_stypes					= mtk_cfg80211_ais_default_mgmt_stypes;

#if KERNEL_VERSION(3, 18, 0) <= CFG80211_VERSION_CODE
	prWiphy->vendor_commands   = mtk_wlan_vendor_ops;
	prWiphy->n_vendor_commands = sizeof(mtk_wlan_vendor_ops) / sizeof(struct wiphy_vendor_command);
	prWiphy->vendor_events	   = mtk_wlan_vendor_events;
	prWiphy->n_vendor_events   = ARRAY_SIZE(mtk_wlan_vendor_events);
#endif
	/* 4 <1.4> wowlan support */
#ifdef CONFIG_PM
#if KERNEL_VERSION(3, 11, 0) <= CFG80211_VERSION_CODE
	prWiphy->wowlan = &mtk_wlan_wowlan_support;
#else
	kalMemCopy(&prWiphy->wowlan, &mtk_wlan_wowlan_support, sizeof(struct wiphy_wowlan_support));
#endif
#endif

#ifdef CONFIG_CFG80211_WEXT
	/* 4 <1.5> Use wireless extension to replace IOCTL */
	prWiphy->wext = &wext_handler_def;
#endif

#ifdef STA_P2P_MCC
	prWiphy->iface_combinations	  = iface_comb_mcc;
	prWiphy->n_iface_combinations = ARRAY_SIZE(iface_comb_mcc);
	DBGLOG(INIT, INFO, "The num_different_channels is %d\n", iface_comb_mcc[0].num_different_channels);
#endif
	if (wiphy_register(prWiphy) < 0) {
		DBGLOG(INIT, ERROR, "wiphy_register error\n");
		goto free_wiphy;
	}
	prWdev->wiphy = prWiphy;
	gprWdev		  = prWdev;
	DBGLOG(INIT, INFO, "create wireless device success\n");
	return;

free_wiphy:
	wiphy_free(prWiphy);
free_wdev:
	kfree(prWdev);
}

static void wlanDestroyWirelessDevice(void)
{
	set_wiphy_dev(gprWdev->wiphy, NULL);
	wiphy_unregister(gprWdev->wiphy);
	wiphy_free(gprWdev->wiphy);
	kfree(gprWdev);
	gprWdev = NULL;
}

struct wireless_dev *wlanGetWirelessDevice(void)
{
	return gprWdev;
}

VOID wlanWakeLockInit(P_GLUE_INFO_T prGlueInfo)
{
}

VOID wlanWakeLockUninit(P_GLUE_INFO_T prGlueInfo)
{
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief A method for creating Linux NET4 struct net_device object and the
 *        private data(prGlueInfo and prAdapter). Setup the IO address to the HIF.
 *        Assign the function pointer to the net_device object
 *
 * \param[in] pvData     Memory address for the device
 *
 * \retval Not null      The wireless_dev object.
 * \retval NULL          Fail to create wireless_dev object
 */
/*----------------------------------------------------------------------------*/
static struct lock_class_key rSpinKey[SPIN_LOCK_NUM];
static struct wireless_dev  *wlanNetCreate(PVOID pvData, PVOID pvDriverData)
{
	struct wireless_dev		*prWdev	  = gprWdev;
	P_GLUE_INFO_T			   prGlueInfo = NULL;
	P_ADAPTER_T				   prAdapter  = NULL;
	UINT_32					   i;
	struct device			  *prDev;
	P_NETDEV_PRIVATE_GLUE_INFO prNetDevPrivate = (P_NETDEV_PRIVATE_GLUE_INFO)NULL;

	PUCHAR prInfName = NULL;

	if (prWdev == NULL) {
		DBGLOG(INIT, ERROR, "No wireless dev exist, abort power on\n");
		return NULL;
	}

	/* 4 <1.3> co-relate wiphy & prDev */
	glGetDev(pvData, &prDev);
	if (!prDev)
		DBGLOG(INIT, ERROR, "unable to get struct dev for wlan\n");
	set_wiphy_dev(prWdev->wiphy, prDev);

	/* 4 <2> Create Glue structure */
	prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(prWdev->wiphy);
	kalMemZero(prGlueInfo, sizeof(GLUE_INFO_T));

	/* 4 <2.1> Create Adapter structure */
	prAdapter = (P_ADAPTER_T)wlanAdapterCreate(prGlueInfo);

	if (!prAdapter) {
		DBGLOG(INIT, ERROR, "Allocating memory to adapter failed\n");
		goto netcreate_err;
	}

	prAdapter->chip_info  = ((struct mt66xx_hif_driver_data *)pvDriverData)->chip_info;
	prGlueInfo->prAdapter = prAdapter;

	/* 4 <3> Initialize Glue structure */
	/* 4 <3.1> Create net device */

#ifdef CFG_DRIVER_INF_NAME_CHANGE

	if (kalStrLen(gprifnamesta) > 0) {
		prInfName = kalStrCat(gprifnamesta, "%d");
		DBGLOG(INIT, WARN, "Station ifname customized, use %s\n", prInfName);
	} else
#endif /* CFG_DRIVER_INF_NAME_CHANGE */
		prInfName = NIC_INF_NAME;

#if KERNEL_VERSION(3, 17, 0) <= CFG80211_VERSION_CODE
	prGlueInfo->prDevHandler = alloc_netdev_mq(
			sizeof(NETDEV_PRIVATE_GLUE_INFO), prInfName, NET_NAME_PREDICTABLE, ether_setup, CFG_MAX_TXQ_NUM);
#else
	prGlueInfo->prDevHandler =
			alloc_netdev_mq(sizeof(NETDEV_PRIVATE_GLUE_INFO), prInfName, ether_setup, CFG_MAX_TXQ_NUM);
#endif
	DBGLOG(INIT, INFO, "net_device prDev(0x%p) allocated\n", prGlueInfo->prDevHandler);

	if (!prGlueInfo->prDevHandler) {
		DBGLOG(INIT, ERROR, "Allocating memory to net_device context failed\n");
		goto netcreate_err;
	}
	DBGLOG(INIT, INFO, "net_device prDev(0x%p) allocated\n", prGlueInfo->prDevHandler);

	/* 4 <3.1.1> Initialize net device varaiables */

	prNetDevPrivate				= (P_NETDEV_PRIVATE_GLUE_INFO)netdev_priv(prGlueInfo->prDevHandler);
	prNetDevPrivate->prGlueInfo = prGlueInfo;

	prGlueInfo->prDevHandler->needed_headroom += NIC_TX_HEAD_ROOM;
	prGlueInfo->prDevHandler->netdev_ops = &wlan_netdev_ops;
#ifdef CONFIG_WIRELESS_EXT
	prGlueInfo->prDevHandler->wireless_handlers = &wext_handler_def;
#endif
	netif_carrier_off(prGlueInfo->prDevHandler);
	netif_tx_stop_all_queues(prGlueInfo->prDevHandler);
	kalResetStats(prGlueInfo->prDevHandler);

#if CFG_SUPPORT_SNIFFER
	INIT_WORK(&(prGlueInfo->monWork), wlanMonWorkHandler);
#endif

	/* 4 <3.1.2> co-relate with wiphy bi-directionally */
	prGlueInfo->prDevHandler->ieee80211_ptr = prWdev;

	prWdev->netdev = prGlueInfo->prDevHandler;

	/* 4 <3.1.3> co-relate net device & prDev */
	SET_NETDEV_DEV(prGlueInfo->prDevHandler, wiphy_dev(prWdev->wiphy));

	/* 4 <3.1.4> set device to glue */
	prGlueInfo->prDev = prDev;

	/* 4 <3.2> Initialize glue variables */
	prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
	prGlueInfo->ePowerState				  = ParamDeviceStateD0;
	prGlueInfo->fgIsRegistered			  = FALSE;
	prGlueInfo->prScanRequest			  = NULL;
	prGlueInfo->prSchedScanRequest		  = NULL;

	init_completion(&prGlueInfo->rScanComp);
	init_completion(&prGlueInfo->rHaltComp);
	init_completion(&prGlueInfo->rPendComp);

#if CFG_SUPPORT_MULTITHREAD
	init_completion(&prGlueInfo->rHifHaltComp);
	init_completion(&prGlueInfo->rRxHaltComp);
#endif

	/* initialize timer for OID timeout checker */
	kalOsTimerInitialize(prGlueInfo, kalTimeoutHandler);

	for (i = 0; i < SPIN_LOCK_NUM; i++) {
		spin_lock_init(&prGlueInfo->rSpinLock[i]);
		lockdep_set_class(&prGlueInfo->rSpinLock[i], &rSpinKey[i]);
	}

	for (i = 0; i < MUTEX_NUM; i++)
		mutex_init(&prGlueInfo->arMutex[i]);

	/* initialize semaphore for ioctl */
	sema_init(&prGlueInfo->ioctl_sem, 1);

#if CFG_SUPPORT_SDIO_READ_WRITE_PATTERN
	/* initialize SDIO read-write pattern control */
	prGlueInfo->fgEnSdioTestPattern		= FALSE;
	prGlueInfo->fgIsSdioTestInitialized = FALSE;
#endif

	/* initialize semaphore for halt control */
	sema_init(&g_halt_sem, 1);

	/* 4 <8> Init Queues */
	init_waitqueue_head(&prGlueInfo->waitq);
	QUEUE_INITIALIZE(&prGlueInfo->rCmdQueue);
	prGlueInfo->i4TxPendingCmdNum = 0;
	QUEUE_INITIALIZE(&prGlueInfo->rTxQueue);
	glSetHifInfo(prGlueInfo, (ULONG)pvData);

	/* Init wakelock */
	wlanWakeLockInit(prGlueInfo);

	/* main thread is created in this function */
#if CFG_SUPPORT_MULTITHREAD
	init_waitqueue_head(&prGlueInfo->waitq_rx);
	init_waitqueue_head(&prGlueInfo->waitq_hif);

	prGlueInfo->u4TxThreadPid  = 0xffffffff;
	prGlueInfo->u4RxThreadPid  = 0xffffffff;
	prGlueInfo->u4HifThreadPid = 0xffffffff;
#endif

	return prWdev;

netcreate_err:
	if (prAdapter != NULL) {
		wlanAdapterDestroy(prAdapter);
		prAdapter = NULL;
	}

	if (prGlueInfo->prDevHandler != NULL) {
		free_netdev(prGlueInfo->prDevHandler);
		prGlueInfo->prDevHandler = NULL;
	}

	return prWdev;
} /* end of wlanNetCreate() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Destroying the struct net_device object and the private data.
 *
 * \param[in] prWdev      Pointer to struct wireless_dev.
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
static VOID wlanNetDestroy(struct wireless_dev *prWdev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	ASSERT(prWdev);

	if (!prWdev) {
		DBGLOG(INIT, ERROR, "The device context is NULL\n");
		return;
	}

	/* prGlueInfo is allocated with net_device */
	prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(prWdev->wiphy);
	ASSERT(prGlueInfo);

	/* destroy kal OS timer */
	kalCancelTimer(prGlueInfo);

	glClearHifInfo(prGlueInfo);

	wlanAdapterDestroy(prGlueInfo->prAdapter);
	prGlueInfo->prAdapter = NULL;

	/* Free net_device and private data, which are allocated by alloc_netdev().
	 */
	free_netdev(prWdev->netdev);

} /* end of wlanNetDestroy() */

VOID wlanSetSuspendMode(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgEnable)
{
	struct net_device *prDev = NULL;

	if (!prGlueInfo)
		return;

	prDev = prGlueInfo->prDevHandler;
	if (!prDev)
		return;

#if CFG_STR_DHCP_RENEW_OFFLOAD
	wlanSetDhcpOffloadInfo(prGlueInfo, prDev, fgEnable);
#endif

	kalSetNetAddressFromInterface(prGlueInfo, prDev, fgEnable);
	wlanNotifyFwSuspend(prGlueInfo, prDev, fgEnable);
}

#if CFG_ENABLE_EARLY_SUSPEND
static struct early_suspend wlan_early_suspend_desc = {
	.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN,
};

static void wlan_early_suspend(struct early_suspend *h)
{
	struct net_device *prDev	  = NULL;
	P_GLUE_INFO_T	   prGlueInfo = NULL;

	/* 4 <1> Sanity Check */
	if ((u4WlanDevNum == 0) && (u4WlanDevNum > CFG_MAX_WLAN_DEVICES)) {
		DBGLOG(INIT, ERROR, "wlanLateResume u4WlanDevNum==0 invalid!!\n");
		return;
	}

	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;
	if (!prDev)
		return;

	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(prDev));
	if (!prGlueInfo)
		return;

	DBGLOG(INIT, INFO, "********<%s>********\n", __func__);

	if (prGlueInfo->fgIsInSuspendMode == TRUE) {
		DBGLOG(INIT, INFO, "%s: Already in suspend mode, SKIP!\n", __func__);
		return;
	}

	prGlueInfo->fgIsInSuspendMode = TRUE;

	wlanSetSuspendMode(prGlueInfo, TRUE);
	p2pSetSuspendMode(prGlueInfo, TRUE);
}

static void wlan_late_resume(struct early_suspend *h)
{
	struct net_device *prDev	  = NULL;
	P_GLUE_INFO_T	   prGlueInfo = NULL;

	/* 4 <1> Sanity Check */
	if ((u4WlanDevNum == 0) && (u4WlanDevNum > CFG_MAX_WLAN_DEVICES)) {
		DBGLOG(INIT, ERROR, "wlanLateResume u4WlanDevNum==0 invalid!!\n");
		return;
	}

	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;
	if (!prDev)
		return;

	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(prDev));
	if (!prGlueInfo)
		return;

	DBGLOG(INIT, INFO, "********<%s>********\n", __func__);

	if (prGlueInfo->fgIsInSuspendMode == FALSE) {
		DBGLOG(INIT, INFO, "%s: Not in suspend mode, SKIP!\n", __func__);
		return;
	}

	prGlueInfo->fgIsInSuspendMode = FALSE;

	/* 4 <2> Set suspend mode for each network */
	wlanSetSuspendMode(prGlueInfo, FALSE);
	p2pSetSuspendMode(prGlueInfo, FALSE);
}
#endif

int set_p2p_mode_handler(struct net_device *netdev, PARAM_CUSTOM_P2P_SET_STRUCT_T p2pmode)
{
	P_GLUE_INFO_T				  prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(netdev));
	PARAM_CUSTOM_P2P_SET_STRUCT_T rSetP2P;
	WLAN_STATUS					  rWlanStatus = WLAN_STATUS_SUCCESS;
	UINT_32						  u4BufLen	  = 0;
	BOOLEAN						  ret		  = FALSE;

	rSetP2P.u4Enable = p2pmode.u4Enable;
	rSetP2P.u4Mode	 = p2pmode.u4Mode;

	if (!rSetP2P.u4Enable) {
		p2pNetUnregister(prGlueInfo, FALSE);
	}

	if (((rSetP2P.u4Mode == RUNNING_DUAL_AP_MODE) || (rSetP2P.u4Mode == RUNNING_P2P_AP_MODE)) &&
			(gprP2pRoleWdev[1] == NULL) && (rSetP2P.u4Enable)) {
		glP2pCreateWirelessDevice(prGlueInfo);
		ASSERT(gprP2pRoleWdev[1] != NULL);
	}

	rWlanStatus = kalIoctl(prGlueInfo, wlanoidSetP2pMode, (PVOID)&rSetP2P, sizeof(PARAM_CUSTOM_P2P_SET_STRUCT_T), FALSE,
			FALSE, TRUE, &u4BufLen);

	DBGLOG(INIT, INFO, "set_p2p_mode_handler ret = 0x%08lx\n", (UINT_32)rWlanStatus);

	if ((rSetP2P.u4Enable) && (prGlueInfo->prAdapter->fgIsP2PRegistered)) {
		ret = p2pNetRegister(prGlueInfo, FALSE);
	}

#if CFG_RESET_DUE_TO_REG_NETDEV_FAIL
	if (ret == TRUE)
		return 0;
	else
		return -1;
#else
	return 0;
#endif
}

#if CFG_SUPPORT_EASY_DEBUG
/*----------------------------------------------------------------------------*/
/*!
 * \brief parse config from wifi.cfg
 *
 * \param[in] prAdapter
 *
 * \retval VOID
 */
/*----------------------------------------------------------------------------*/
VOID wlanGetParseConfig(P_ADAPTER_T prAdapter)
{
	PUINT_8 pucConfigBuf;
	UINT_32 u4ConfigReadLen;

	wlanCfgInit(prAdapter, NULL, 0, 0);
	pucConfigBuf = (PUINT_8)kalMemAlloc(WLAN_CFG_FILE_BUF_SIZE, VIR_MEM_TYPE);
	kalMemZero(pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE);
	u4ConfigReadLen = 0;
	if (pucConfigBuf) {
#ifdef CFG_SUPPORT_DUAL_CARD_DUAL_DRIVER
		if (kalRequestFirmware("wifi_sdio.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen,
					prAdapter->prGlueInfo->prDev) == 0) {
		} else
#endif
				if (kalRequestFirmware("wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen,
							prAdapter->prGlueInfo->prDev) == 0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/storage/sdcard0/wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen) ==
				   0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/data/misc/wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen) == 0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/data/misc/wifi/wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen) ==
				   0) {
			/* ToDo:: Nothing */
		}

		if (pucConfigBuf[0] != '\0' && u4ConfigReadLen > 0)
			wlanCfgParse(prAdapter, pucConfigBuf, u4ConfigReadLen, TRUE);

		kalMemFree(pucConfigBuf, VIR_MEM_TYPE, WLAN_CFG_FILE_BUF_SIZE);
	} /* pucConfigBuf */
}

#endif

/*----------------------------------------------------------------------------*/
/*!
 * \brief get config from wifi.cfg
 *
 * \param[in] prAdapter
 *
 * \retval VOID
 */
/*----------------------------------------------------------------------------*/
VOID wlanGetConfig(P_ADAPTER_T prAdapter)
{
	PUINT_8 pucConfigBuf;
	UINT_32 u4ConfigReadLen;

	wlanCfgInit(prAdapter, NULL, 0, 0);
	pucConfigBuf = (PUINT_8)kalMemAlloc(WLAN_CFG_FILE_BUF_SIZE, VIR_MEM_TYPE);
	kalMemZero(pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE);
	u4ConfigReadLen = 0;
	if (pucConfigBuf) {
#ifdef CFG_SUPPORT_DUAL_CARD_DUAL_DRIVER
		if (kalRequestFirmware(CFG_WIFI_FILENAME, pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen,
					prAdapter->prGlueInfo->prDev) == 0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/storage/sdcard0/" CFG_WIFI_FILENAME, pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE,
						   &u4ConfigReadLen) == 0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/data/misc/" CFG_WIFI_FILENAME, pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE,
						   &u4ConfigReadLen) == 0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/data/misc/wifi/" CFG_WIFI_FILENAME, pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE,
						   &u4ConfigReadLen) == 0) {
			/* ToDo:: Nothing */
		}

		if (pucConfigBuf[0] != '\0' && u4ConfigReadLen > 0) {
			wlanCfgParse(prAdapter, pucConfigBuf, u4ConfigReadLen, TRUE);
		}

		kalMemFree(pucConfigBuf, VIR_MEM_TYPE, WLAN_CFG_FILE_BUF_SIZE);
#else
		if (kalRequestFirmware("wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen,
					prAdapter->prGlueInfo->prDev) == 0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/storage/sdcard0/wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen) ==
				   0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/data/misc/wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen) == 0) {
			/* ToDo:: Nothing */
		} else if (kalReadToFile("/data/misc/wifi/wifi.cfg", pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen) ==
				   0) {
			/* ToDo:: Nothing */
		}

		if (pucConfigBuf[0] != '\0' && u4ConfigReadLen > 0)
			wlanCfgInit(prAdapter, pucConfigBuf, u4ConfigReadLen, 0);

		kalMemFree(pucConfigBuf, VIR_MEM_TYPE, WLAN_CFG_FILE_BUF_SIZE);
#endif
	} /* pucConfigBuf */
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief this extract buffer bin EEPROB_MTxxxx.bin to temp buffer
 *
 * \param[in] prAdapter
 *
 * \retval WLAN_STATUS_SUCCESS Success
 * \retval WLAN_STATUS_FAILURE Failed
 */
/*----------------------------------------------------------------------------*/
WLAN_STATUS wlanExtractBufferBin(P_ADAPTER_T prAdapter)
{
	P_GLUE_INFO_T			 prGlueInfo = NULL;
	UINT_32					 u4ContentLen;
	PUINT_8					 pucConfigBuf = NULL;
	struct mt66xx_chip_info *prChipInfo;
	UINT_32					 chip_id;
	UINT_8					 aucEeprom[32];
	WLAN_STATUS				 retWlanStat = WLAN_STATUS_FAILURE;

	if (prAdapter->fgIsSupportPowerOnSendBufferModeCMD == TRUE) {
		DBGLOG(INIT, INFO, "Start Efuse Buffer Mode step0\n");
		DBGLOG(INIT, INFO, "ucEfuseBUfferModeCal is %x\n", prAdapter->rWifiVar.ucEfuseBufferModeCal);

		prChipInfo = prAdapter->chip_info;
		chip_id	   = prChipInfo->chip_id;
		prGlueInfo = prAdapter->prGlueInfo;

		if (prGlueInfo == NULL || prGlueInfo->prDev == NULL)
			goto label_exit;

		DBGLOG(INIT, INFO, "Start Efuse Buffer Mode step1\n");

		/* allocate memory for buffer mode info */
		if ((prAdapter->rWifiVar.ucEfuseBufferModeCal == LOAD_EEPROM_BIN) &&
				(prAdapter->fgIsBufferBinExtract == FALSE)) {
			DBGLOG(INIT, INFO, "Start Efuse Buffer Mode step2\n");
			/* Only in buffer mode need to access bin file */
			/* 1 <1> Load bin file*/
			pucConfigBuf = (PUINT_8)kalMemAlloc(MAX_EEPROM_BUFFER_SIZE, VIR_MEM_TYPE);
			if (pucConfigBuf == NULL)
				goto label_exit;

			kalMemZero(pucConfigBuf, MAX_EEPROM_BUFFER_SIZE);

			/* 1 <2> Construct EEPROM binary name */
			kalMemZero(aucEeprom, sizeof(aucEeprom));

			snprintf(aucEeprom, 32, "%s%x.bin", apucEepromName[0], chip_id);

			/* 1 <3> Request buffer bin */
			if (kalRequestFirmware(aucEeprom, pucConfigBuf, MAX_EEPROM_BUFFER_SIZE, &u4ContentLen, prGlueInfo->prDev) ==
					0) {
				DBGLOG(INIT, INFO, "request file done\n");
			} else {
				DBGLOG(INIT, INFO, "can't find file\n");
				kalMemFree(pucConfigBuf, VIR_MEM_TYPE, MAX_EEPROM_BUFFER_SIZE);
				goto label_exit;
			}

			/* Update contents in local table */
			kalMemCopy(uacEEPROMImage, pucConfigBuf, MAX_EEPROM_BUFFER_SIZE);

			/* Free buffer */
			kalMemFree(pucConfigBuf, VIR_MEM_TYPE, MAX_EEPROM_BUFFER_SIZE);

			DBGLOG(INIT, INFO, "Start Efuse Buffer Mode step3\n");
			prAdapter->fgIsBufferBinExtract = TRUE;
		}
	}

	retWlanStat = WLAN_STATUS_SUCCESS;

label_exit:

	return retWlanStat;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief this function send buffer bin EEPROB_MTxxxx.bin to FW.
 *
 * \param[in] prAdapter
 *
 * \retval WLAN_STATUS_SUCCESS Success
 * \retval WLAN_STATUS_FAILURE Failed
 */
/*----------------------------------------------------------------------------*/
WLAN_STATUS wlanDownloadBufferBin(P_ADAPTER_T prAdapter)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
#if (CFG_FW_Report_Efuse_Address)
	UINT_16 u2InitAddr = prAdapter->u4EfuseStartAddress;
#else
	UINT_16 u2InitAddr = EFUSE_CONTENT_BUFFER_START;
#endif
	UINT_32							  u4BufLen				= 0;
	WLAN_STATUS						  rStatus				= WLAN_STATUS_SUCCESS;
	PARAM_CUSTOM_EFUSE_BUFFER_MODE_T *prSetEfuseBufModeInfo = NULL;
	UINT_32							  u4ContentLen;
	struct mt66xx_chip_info			*prChipInfo;
	UINT_32							  chip_id;
	WLAN_STATUS						  retWlanStat  = WLAN_STATUS_FAILURE;
	INT_32							  i4OidTimeout = -1;

#if CFG_EFUSE_AUTO_MODE_SUPPORT
	UINT_32						u4Efuse_addr = 0;
	UINT_8						u4Index		 = 0;
	PARAM_CUSTOM_ACCESS_EFUSE_T rAccessEfuseInfo;
#endif

	if (prAdapter->fgIsSupportPowerOnSendBufferModeCMD == TRUE) {
		DBGLOG(INIT, INFO, "Start Efuse Buffer Mode ..\n");
		DBGLOG(INIT, INFO, "ucEfuseBUfferModeCal is %x\n", prAdapter->rWifiVar.ucEfuseBufferModeCal);

		prChipInfo = prAdapter->chip_info;
		chip_id	   = prChipInfo->chip_id;
		prGlueInfo = prAdapter->prGlueInfo;

		if (prGlueInfo == NULL || prGlueInfo->prDev == NULL)
			goto label_exit;

		/* allocate memory for buffer mode info */
		prSetEfuseBufModeInfo =
				(PARAM_CUSTOM_EFUSE_BUFFER_MODE_T *)kalMemAlloc(sizeof(PARAM_CUSTOM_EFUSE_BUFFER_MODE_T), VIR_MEM_TYPE);
		if (prSetEfuseBufModeInfo == NULL)
			goto label_exit;
		kalMemZero(prSetEfuseBufModeInfo, sizeof(PARAM_CUSTOM_EFUSE_BUFFER_MODE_T));

#if CFG_EFUSE_AUTO_MODE_SUPPORT
		if (prAdapter->rWifiVar.ucEfuseBufferModeCal == LOAD_AUTO) {
			kalMemSet(&rAccessEfuseInfo, 0, sizeof(PARAM_CUSTOM_ACCESS_EFUSE_T));
			rAccessEfuseInfo.u4Address = (u4Efuse_addr / EFUSE_BLOCK_SIZE) * EFUSE_BLOCK_SIZE;
			u4Index					   = u4Efuse_addr % EFUSE_BLOCK_SIZE;
			rStatus					   = kalIoctl(prGlueInfo, wlanoidQueryProcessAccessEfuseRead, &rAccessEfuseInfo,
									   sizeof(PARAM_CUSTOM_ACCESS_EFUSE_T), TRUE, TRUE, TRUE, &u4BufLen);
			if (prGlueInfo->prAdapter->aucEepromVaule[1] == EFUSE_AUTO_CHEK) {
				prAdapter->rWifiVar.ucEfuseBufferModeCal = LOAD_EFUSE;
				DBGLOG(INIT, STATE, "[EFUSE AUTO] EFUSE Mode\n");
			} else {
				prAdapter->rWifiVar.ucEfuseBufferModeCal = LOAD_EEPROM_BIN;
				DBGLOG(INIT, STATE, "[EFUSE AUTO] Buffer Mode\n");
			}
		}
#endif

		if (prAdapter->rWifiVar.ucEfuseBufferModeCal == LOAD_EEPROM_BIN) {
			/* Buffer mode */
			/* Only in buffer mode need to access bin file */
			if (wlanExtractBufferBin(prAdapter) != WLAN_STATUS_SUCCESS)
				goto label_exit;

				/* copy to the command buffer */
#if (CFG_FW_Report_Efuse_Address)
			u4ContentLen = (prAdapter->u4EfuseEndAddress) - (prAdapter->u4EfuseStartAddress) + 1;
#else
			u4ContentLen = EFUSE_CONTENT_BUFFER_SIZE;
#endif
			if (u4ContentLen > MAX_EEPROM_BUFFER_SIZE)
				goto label_exit;
			kalMemCopy(prSetEfuseBufModeInfo->aBinContent, &uacEEPROMImage[u2InitAddr], u4ContentLen);

			prSetEfuseBufModeInfo->ucSourceMode = 1;
		} else {
			/* eFuse mode */
			/* Only need to tell FW the content from, contents are directly from efuse */
			prSetEfuseBufModeInfo->ucSourceMode = 0;
		}
		prSetEfuseBufModeInfo->ucCmdType = 0x1 | (prAdapter->rWifiVar.ucCalTimingCtrl << 4);
		prSetEfuseBufModeInfo->ucCount	 = 0xFF; /* ucCmdType 1 don't care the ucCount */

		if (!prAdapter->rWifiVar.ucCalTimingCtrl) {
			/* Full channel RF-cal mode. Need 6000ms */
			i4OidTimeout = 6000;
		}

		rStatus = kalIoctlTimeout(prGlueInfo, wlanoidSetEfusBufferMode, (PVOID)prSetEfuseBufModeInfo,
				sizeof(PARAM_CUSTOM_EFUSE_BUFFER_MODE_T), FALSE, TRUE, TRUE, i4OidTimeout, &u4BufLen);
	}

	retWlanStat = WLAN_STATUS_SUCCESS;

label_exit:

	/* free memory */
	if (prSetEfuseBufModeInfo != NULL)
		kalMemFree(prSetEfuseBufModeInfo, VIR_MEM_TYPE, sizeof(PARAM_CUSTOM_EFUSE_BUFFER_MODE_T));

	return retWlanStat;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief Wlan probe function. This function probes and initializes the device.
 *
 * \param[in] pvData     data passed by bus driver init function
 *                       sdio bus driver handle
 *
 * \retval 0 Success
 * \retval negative value Failed
 */
/*----------------------------------------------------------------------------*/
INT_32 wlanProbe(PVOID pvData, PVOID pvDriverData)
{
	struct wireless_dev *prWdev		   = NULL;
	P_WLANDEV_INFO_T	 prWlandevInfo = NULL;
	INT_32				 i4DevIdx	   = 0;
	P_GLUE_INFO_T		 prGlueInfo	   = NULL;
	P_ADAPTER_T			 prAdapter	   = NULL;
	INT_32				 i4Status	   = 0;
	BOOL				 bRet		   = FALSE;
	P_REG_INFO_T		 prRegInfo;

#if CFG_SUPPORT_REPLAY_DETECTION
	UINT_8 ucRpyDetectOffload;
#endif

	do {
		/* 4 <1> Initialize the IO port of the interface */
		/* GeorgeKuo: pData has different meaning for _HIF_XXX:
		 * bus driver handle
		 */

		DBGLOG(INIT, STATE, "enter wlanProbe\n");

		bRet = glBusInit(pvData);

		/* Cannot get IO address from interface */
		if (bRet == FALSE) {
			DBGLOG(INIT, ERROR, "wlanProbe: glBusInit() fail\n");
			i4Status = -EIO;
			break;
		}
		/* 4 <2> Create network device, Adapter, KalInfo, prDevHandler(netdev) */
		prWdev = wlanNetCreate(pvData, pvDriverData);
		if (prWdev == NULL) {
			DBGLOG(INIT, ERROR, "wlanProbe: No memory for dev and its private\n");
			i4Status = -ENOMEM;
			break;
		}
		DBGLOG(INIT, STATE, "wlanNetCreate done\n");
		/* 4 <2.5> Set the ioaddr to HIF Info */
		prGlueInfo = (P_GLUE_INFO_T)wiphy_priv(prWdev->wiphy);
		if (prGlueInfo == NULL) {
			DBGLOG(INIT, ERROR, "wlanProbe: get wiphy_priv() fail\n");
			return -1;
		}

		gPrDev = prGlueInfo->prDevHandler;

		/* 4 <4> Setup IRQ */
		prWlandevInfo = &arWlanDevInfo[i4DevIdx];

		i4Status = glBusSetIrq(prWdev->netdev, NULL, prGlueInfo);

		if (i4Status != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, "wlanProbe: Set IRQ error\n");
			break;
		}

		prGlueInfo->i4DevIdx = i4DevIdx;

		prAdapter = prGlueInfo->prAdapter;

		prGlueInfo->u4ReadyFlag = 0;

#if CFG_TCP_IP_CHKSUM_OFFLOAD
		prAdapter->fgIsSupportCsumOffload = FALSE;
		prAdapter->u4CSUMFlags			  = CSUM_OFFLOAD_EN_ALL;
#endif

#if CFG_SUPPORT_CFG_FILE
		wlanGetConfig(prAdapter);
#endif
		/* Default support 2.4/5G MIMO */
		prAdapter->rWifiFemCfg.u2WifiPath =
				(WLAN_FLAG_2G4_WF0 | WLAN_FLAG_5G_WF0 | WLAN_FLAG_2G4_WF1 | WLAN_FLAG_5G_WF1);

		DBGLOG(INIT, INFO, "WifiPath Init=0x%x\n", prAdapter->rWifiFemCfg.u2WifiPath);

		/* 4 <5> Start Device */
		prRegInfo = &prGlueInfo->rRegInfo;

		/* P_REG_INFO_T prRegInfo = (P_REG_INFO_T) kmalloc(sizeof(REG_INFO_T), GFP_KERNEL); */
		kalMemSet(prRegInfo, 0, sizeof(REG_INFO_T));

		/* Trigger the action of switching Pwr state to drv_own */
		prAdapter->fgIsFwOwn = TRUE;

		nicpmWakeUpWiFi(prAdapter);

		/* Load NVRAM content to REG_INFO_T */
		glLoadNvram(prGlueInfo, prRegInfo);

		/* kalMemCopy(&prGlueInfo->rRegInfo, prRegInfo, sizeof(REG_INFO_T)); */

		prRegInfo->u4PowerMode = CFG_INIT_POWER_SAVE_PROF;

		/* The Init value of u4WpaVersion/u4AuthAlg shall be DISABLE/OPEN, not zero! */
		/* The Init value of u4CipherGroup/u4CipherPairwise shall be NONE, not zero! */
		prGlueInfo->rWpaInfo.u4WpaVersion	  = IW_AUTH_WPA_VERSION_DISABLED;
		prGlueInfo->rWpaInfo.u4AuthAlg		  = IW_AUTH_ALG_OPEN_SYSTEM;
		prGlueInfo->rWpaInfo.u4CipherGroup	  = IW_AUTH_CIPHER_NONE;
		prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_NONE;

		tasklet_init(&prGlueInfo->rRxTask, halRxTasklet, (unsigned long)prGlueInfo);
		tasklet_init(&prGlueInfo->rTxCompleteTask, halTxCompleteTasklet, (unsigned long)prGlueInfo);

		DBGLOG(INIT, STATE, "wlanAdapterStart\n");
		if (wlanAdapterStart(prAdapter, prRegInfo) != WLAN_STATUS_SUCCESS)
			i4Status = -EIO;

		if (i4Status < 0)
			break;

		if (HAL_IS_TX_DIRECT(prAdapter)) {
			if (!prAdapter->fgTxDirectInited) {
				skb_queue_head_init(&prAdapter->rTxDirectSkbQueue);
#if KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE
				timer_setup(&prAdapter->rTxDirectSkbTimer, nicTxDirectTimerCheckSkbQ, 0);
				timer_setup(&prAdapter->rTxDirectHifTimer, nicTxDirectTimerCheckHifQ, 0);
#else
				init_timer(&prAdapter->rTxDirectSkbTimer);
				prAdapter->rTxDirectSkbTimer.data	  = (unsigned long)prGlueInfo;
				prAdapter->rTxDirectSkbTimer.function = nicTxDirectTimerCheckSkbQ;

				init_timer(&prAdapter->rTxDirectHifTimer);
				prAdapter->rTxDirectHifTimer.data	  = (unsigned long)prGlueInfo;
				prAdapter->rTxDirectHifTimer.function = nicTxDirectTimerCheckHifQ;
#endif

				prAdapter->fgTxDirectInited = TRUE;
			}
		}

		/* kfree(prRegInfo); */

		DBGLOG(INIT, STATE, "starting wlan threads ...\n");
		INIT_WORK(&prGlueInfo->rTxMsduFreeWork, kalFreeTxMsduWorker);
		INIT_DELAYED_WORK(&prGlueInfo->rRxPktDeAggWork, halDeAggRxPktWorker);

		prGlueInfo->main_thread = kthread_run(main_thread, prGlueInfo->prDevHandler, "main_thread");
#if CFG_SUPPORT_MULTITHREAD
		prGlueInfo->hif_thread = kthread_run(hif_thread, prGlueInfo->prDevHandler, "hif_thread");
		prGlueInfo->rx_thread  = kthread_run(rx_thread, prGlueInfo->prDevHandler, "rx_thread");
#endif

		/* TODO the change schedule API shall be provided by OS glue layer */
		/* Switch the Wi-Fi task priority to higher priority and change the scheduling method */
		if (prGlueInfo->prAdapter->rWifiVar.ucThreadPriority > 0) {
			const struct sched_param param = { .sched_priority = prGlueInfo->prAdapter->rWifiVar.ucThreadPriority };
			kal_sched_set(prGlueInfo->main_thread, prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling, &param,
					prGlueInfo->prAdapter->rWifiVar.cThreadNice);
#if CFG_SUPPORT_MULTITHREAD
			kal_sched_set(prGlueInfo->hif_thread, prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling, &param,
					prGlueInfo->prAdapter->rWifiVar.cThreadNice);
			kal_sched_set(prGlueInfo->rx_thread, prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling, &param,
					prGlueInfo->prAdapter->rWifiVar.cThreadNice);
#endif
			DBGLOG(INIT, INFO, "Set pri = %d, sched = %d\n", prGlueInfo->prAdapter->rWifiVar.ucThreadPriority,
					prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling);
		}

		/* Disable 5G band for AIS */
		if (prAdapter->fgEnable5GBand == FALSE)
			prWdev->wiphy->bands[KAL_BAND_5GHZ] = NULL;

		g_u4HaltFlag = 0;

#if CFG_SUPPORT_BUFFER_MODE
#if (CFG_EFUSE_BUFFER_MODE_DELAY_CAL == 1)
		if (wlanDownloadBufferBin(prAdapter) != WLAN_STATUS_SUCCESS) {
			i4Status = -EIO;
			DBGLOG(INIT, ERROR, "wlanProbe: wlanDownloadBufferBin() failed\n");
			break;
		}

#endif
#endif
		/* send regulatory information to firmware */
		rlmDomainSendInfoToFirmware(prAdapter);

		/* set MAC address */
		{
			WLAN_STATUS		rStatus = WLAN_STATUS_FAILURE;
			struct sockaddr MacAddr;
			UINT_32			u4SetInfoLen = 0;

			rStatus = kalIoctl(prGlueInfo, wlanoidQueryCurrentAddr, &MacAddr.sa_data, PARAM_MAC_ADDR_LEN, TRUE, TRUE,
					TRUE, &u4SetInfoLen);

			if (rStatus != WLAN_STATUS_SUCCESS) {
				DBGLOG(INIT, WARN, "set MAC addr fail 0x%lx go to probe fail\n", rStatus);
				prGlueInfo->u4ReadyFlag = 0;
				i4Status				= -ENXIO;
				break;
			} else {
				kalMemCopy(prGlueInfo->prDevHandler->dev_addr, &MacAddr.sa_data, ETH_ALEN);
				kalMemCopy(prGlueInfo->prDevHandler->perm_addr, prGlueInfo->prDevHandler->dev_addr, ETH_ALEN);
#if CFG_MESON_G12A_PATCH
				prGlueInfo->prDevHandler->mtu = 1408;
#endif

				/* card is ready */
				prGlueInfo->u4ReadyFlag = 1;
#if CFG_SHOW_MACADDR_SOURCE
				DBGLOG(INIT, INFO, "MAC address: " MACSTR, MAC2STR(&MacAddr.sa_data));
#endif
			}
		}

#if CFG_TCP_IP_CHKSUM_OFFLOAD
		/* set HW checksum offload */
		if (prAdapter->fgIsSupportCsumOffload) {
			WLAN_STATUS rStatus		 = WLAN_STATUS_FAILURE;
			UINT_32		u4CSUMFlags	 = CSUM_OFFLOAD_EN_ALL;
			UINT_32		u4SetInfoLen = 0;

			rStatus = kalIoctl(prGlueInfo, wlanoidSetCSUMOffload, (PVOID)&u4CSUMFlags, sizeof(UINT_32), FALSE, FALSE,
					TRUE, &u4SetInfoLen);

			if (rStatus == WLAN_STATUS_SUCCESS) {
				prGlueInfo->prDevHandler->features = NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_RXCSUM;
			} else {
				DBGLOG(INIT, WARN, "set HW checksum offload fail 0x%lx go to probe fail\n", rStatus);
				prAdapter->fgIsSupportCsumOffload = FALSE;
				i4Status						  = -ENXIO;
				break;
			}
		}
#endif

		DBGLOG(INIT, STATE, "wlanNetRegister\n");
		/* 4 <3> Register the card */
		i4DevIdx = wlanNetRegister(prWdev);
		if (i4DevIdx < 0) {
			i4Status = -ENXIO;
			DBGLOG(INIT, ERROR, "wlanProbe: Cannot register the net_device context to the kernel\n");
			break;
		}
		/* 4 <4> Register early suspend callback */
#if CFG_ENABLE_EARLY_SUSPEND
		glRegisterEarlySuspend(&wlan_early_suspend_desc, wlan_early_suspend, wlan_late_resume);
#endif

		/* 4 <5> Register Notifier callback */
		wlanRegisterNotifier();

		/* 4 <6> Initialize /proc filesystem */
#if WLAN_INCLUDE_PROC
		DBGLOG(INIT, STATE, "procCreateFsEntry\n");
		i4Status = procCreateFsEntry(prGlueInfo);
		if (i4Status < 0) {
			DBGLOG(INIT, ERROR, "wlanProbe: init procfs failed\n");
			break;
		}
#endif

#if CFG_MET_PACKET_TRACE_SUPPORT
		kalMetInit(prGlueInfo);
#endif

#if (CFG_ENABLE_WIFI_DIRECT)
		if (prAdapter->rWifiVar.u4RegP2pIfAtProbe) {
			PARAM_CUSTOM_P2P_SET_STRUCT_T rSetP2P;

			rSetP2P.u4Enable = 1;

#ifdef CFG_DRIVER_INITIAL_RUNNING_MODE
			rSetP2P.u4Mode = CFG_DRIVER_INITIAL_RUNNING_MODE;
#else
			rSetP2P.u4Mode = RUNNING_P2P_MODE;
#endif /* CFG_DRIVER_RUNNING_MODE */
			if (set_p2p_mode_handler(prWdev->netdev, rSetP2P) == 0)
				DBGLOG(INIT, STATE, "%s: p2p device registered\n", __func__);
			else {
				DBGLOG(INIT, ERROR, "%s: Failed to register p2p device\n", __func__);
#if CFG_RESET_DUE_TO_REG_NETDEV_FAIL
				i4Status = -ENXIO;
				break;
#endif
			}
		}
#endif
	} while (FALSE);

	if (i4Status == 0) {
#if CFG_SUPPORT_AGPS_ASSIST
		kalIndicateAgpsNotify(prAdapter, AGPS_EVENT_WLAN_ON, NULL, 0);
#endif

#if CFG_SUPPORT_EASY_DEBUG

		/* move before reading file
		 *wlanLoadDefaultCustomerSetting(prAdapter);
		 */

		DBGLOG(INIT, STATE, "wlanFeatureToFw\n");
		wlanFeatureToFw(prGlueInfo->prAdapter);

#endif
		wlanCfgSetSwCtrl(prGlueInfo->prAdapter);

		wlanCfgSetChip(prGlueInfo->prAdapter);

		wlanCfgSetCountryCode(prGlueInfo->prAdapter);

#if CFG_SUPPORT_ANT_SELECT
		/* update some info needed before connected */
		wlanUpdateExtInfo(prGlueInfo->prAdapter);
#endif

#if CFG_SUPPORT_RSSI_COMP
		wlanUpdateRssiComp(prGlueInfo->prAdapter);
#endif

#if (CFG_MET_PACKET_TRACE_SUPPORT == 1)
		DBGLOG(INIT, TRACE, "init MET procfs...\n");
		i4Status = kalMetInitProcfs(prGlueInfo);
		if (i4Status < 0)
			DBGLOG(INIT, ERROR, "wlanProbe: init MET procfs failed\n");
#endif

#if CFG_SUPPORT_REPLAY_DETECTION
		ucRpyDetectOffload = prAdapter->rWifiVar.ucRpyDetectOffload;

		if (ucRpyDetectOffload == FEATURE_ENABLED) {
			DBGLOG(INIT, STATE, "CMD - Enable Replay Detection offload\n");
			wlanSuspendRekeyOffload(prAdapter->prGlueInfo, GTK_REKEY_CMD_MODE_RPY_OFFLOAD_ON);
		} else {
			DBGLOG(INIT, STATE, "CMD - Disable Replay Detection offload\n");
			wlanSuspendRekeyOffload(prAdapter->prGlueInfo, GTK_REKEY_CMD_MODE_RPY_OFFLOAD_OFF);
		}
#endif

		DBGLOG(INIT, STATE, "wlanProbe: probe success\n");
		set_bit(GLUE_FLAG_ADAPT_RDY_BIT, &prGlueInfo->ulFlag);

#ifdef CONFIG_PM_SLEEP
		register_pm_notifier(&pm_resume_notifier_func);
#endif
	} else {
		if (prGlueInfo == NULL)
			return -1;

		glBusFreeIrq(prGlueInfo->prDevHandler, prGlueInfo);
		DBGLOG(INIT, ERROR, "wlanProbe: probe failed\n");
	}

	if (prWaitForResetComp) {
		complete(prWaitForResetComp);
		prWaitForResetComp = NULL;
	}

	wlanRegisterRebootNotifier();
	return i4Status;
} /* end of wlanProbe() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief A method to stop driver operation and release all resources. Following
 *        this call, no frame should go up or down through this interface.
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
VOID wlanRemove(VOID)
{
	struct net_device *prDev		 = NULL;
	P_WLANDEV_INFO_T   prWlandevInfo = NULL;
	P_GLUE_INFO_T	   prGlueInfo	 = NULL;
	P_ADAPTER_T		   prAdapter	 = NULL;
	BOOLEAN			   fgResult		 = FALSE;
	/*  waitForResetCompInit is set to 0 during kernel module initialization and
		change to 1 only once at the 1st time wlanRemove() call in the module
		lifetime. This ensures prWaitForResetComp is initialized with
		init_completion() and all the subsequent calls will use
		reinit_completion() to reinit
	*/
	static u_int8_t waitForResetCompInit = 0;

	DBGLOG(INIT, STATE, "Remove wlan!\n");

	prWaitForResetComp = &rWaitForResetComp;
	if (waitForResetCompInit) {
		reinit_completion(prWaitForResetComp);
	} else {
		init_completion(prWaitForResetComp);
		waitForResetCompInit = 1;
	}

	/* 4 <0> Sanity check */
	ASSERT(u4WlanDevNum <= CFG_MAX_WLAN_DEVICES);
	if (u4WlanDevNum == 0) {
		DBGLOG(INIT, ERROR, "0 == u4WlanDevNum\n");
		return;
	}

	if (u4WlanDevNum > 0 && u4WlanDevNum <= CFG_MAX_WLAN_DEVICES) {
		prDev		  = arWlanDevInfo[u4WlanDevNum - 1].prDev;
		prWlandevInfo = &arWlanDevInfo[u4WlanDevNum - 1];
	}

	ASSERT(prDev);
	if (prDev == NULL) {
		DBGLOG(INIT, ERROR, "NULL == prDev\n");
		return;
	}

	prGlueInfo = *((P_GLUE_INFO_T *)netdev_priv(prDev));
	ASSERT(prGlueInfo);
	if (prGlueInfo == NULL) {
		DBGLOG(INIT, ERROR, "NULL == prGlueInfo\n");
		free_netdev(prDev);
		return;
	}

	prAdapter = prGlueInfo->prAdapter;

#ifdef CONFIG_PM_SLEEP
	unregister_pm_notifier(&pm_resume_notifier_func);
#endif

	if (prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED) {
		cfg80211_disconnected(prGlueInfo->prDevHandler, 0, NULL, 0, TRUE, GFP_KERNEL);
		kalMsleep(500);
	}

	DBGLOG(INIT, STATE, "flush workq\n");
	flush_delayed_work(&workq);
#if CFG_SUPPORT_CFG80211_AUTH
#if CFG_WDEV_LOCK_THREAD_SUPPORT
	flush_delayed_work(&wdev_lock_workq);
#endif
#endif

	/* 20150205 work queue for sched_scan */

	flush_delayed_work(&sched_workq);

	rtnl_lock();
	clear_bit(GLUE_FLAG_ADAPT_RDY_BIT, &prGlueInfo->ulFlag);
	rtnl_unlock();

	down(&g_halt_sem);
	g_u4HaltFlag = 1;
	up(&g_halt_sem);

	/* 4 <2> Mark HALT, notify main thread to stop, and clean up queued requests */
	set_bit(GLUE_FLAG_HALT_BIT, &prGlueInfo->ulFlag);

	DBGLOG(INIT, STATE, "wlan thread stopping\n");

#if CFG_SUPPORT_MULTITHREAD
	wake_up_interruptible(&prGlueInfo->waitq_hif);
	wait_for_completion_interruptible_timeout(&prGlueInfo->rHifHaltComp, SEC_TO_SYSTIME(15));
	wake_up_interruptible(&prGlueInfo->waitq_rx);
	wait_for_completion_interruptible_timeout(&prGlueInfo->rRxHaltComp, SEC_TO_SYSTIME(15));
#endif

	/* wake up main thread */
	wake_up_interruptible(&prGlueInfo->waitq);
	/* wait main thread stops */
	wait_for_completion_interruptible_timeout(&prGlueInfo->rHaltComp, SEC_TO_SYSTIME(15));

	/* Stop works */
	flush_work(&prGlueInfo->rTxMsduFreeWork);
	cancel_delayed_work_sync(&prGlueInfo->rRxPktDeAggWork);

	DBGLOG(INIT, STATE, "wlan thread stopped\n");

	/* prGlueInfo->rHifInfo.main_thread = NULL; */
	prGlueInfo->main_thread = NULL;
#if CFG_SUPPORT_MULTITHREAD
	prGlueInfo->hif_thread = NULL;
	prGlueInfo->rx_thread  = NULL;

	prGlueInfo->u4TxThreadPid  = 0xffffffff;
	prGlueInfo->u4HifThreadPid = 0xffffffff;
#endif

	if (HAL_IS_TX_DIRECT(prAdapter)) {
		if (prAdapter->fgTxDirectInited) {
			del_timer_sync(&prAdapter->rTxDirectSkbTimer);
			del_timer_sync(&prAdapter->rTxDirectHifTimer);
		}
	}

	/* Destroy wakelock */
	wlanWakeLockUninit(prGlueInfo);

	kalMemSet(&(prGlueInfo->prAdapter->rWlanInfo), 0, sizeof(WLAN_INFO_T));

#if CFG_ENABLE_WIFI_DIRECT
	if (prGlueInfo->prAdapter->fgIsP2PRegistered) {
		DBGLOG(INIT, STATE, "p2pNetUnregister...\n");
		p2pNetUnregister(prGlueInfo, FALSE);
		DBGLOG(INIT, STATE, "p2pRemove...\n");
		/*p2pRemove must before wlanAdapterStop */
		p2pRemove(prGlueInfo);
	}
#endif

	/* 4 <3> Remove /proc filesystem. */
#if WLAN_INCLUDE_PROC
	procRemoveProcfs();
#endif /* WLAN_INCLUDE_PROC */

#if (CFG_MET_PACKET_TRACE_SUPPORT == 1)
	kalMetRemoveProcfs(prGlueInfo);
#endif

	/* 4 <4> wlanAdapterStop */
#if CFG_SUPPORT_AGPS_ASSIST
	kalIndicateAgpsNotify(prAdapter, AGPS_EVENT_WLAN_OFF, NULL, 0);
#endif

	DBGLOG(INIT, STATE, "call wlanAdapterStop\n");
	wlanAdapterStop(prAdapter);
	DBGLOG(INIT, INFO, "Number of Stalled Packets = %d\n", GLUE_GET_REF_CNT(prGlueInfo->i4TxPendingFrameNum));

	HAL_LP_OWN_SET(prAdapter, &fgResult);
	DBGLOG(INIT, STATE, "HAL_LP_OWN_SET(%d)\n", fgResult);

	/* 4 <x> Stopping handling interrupt and free IRQ */
	glBusFreeIrq(prDev, prGlueInfo);

	/* 4 <5> Release the Bus */
	glBusRelease(prDev);

	DBGLOG(INIT, STATE, "call wlanNetUnregister\n");
	/* 4 <6> Unregister the card */
	wlanNetUnregister(prDev->ieee80211_ptr);

	DBGLOG(INIT, STATE, "call wlanNetDestroy\n");
	/* 4 <7> Destroy the device */
	wlanNetDestroy(prDev->ieee80211_ptr);
	prDev = NULL;

	tasklet_kill(&prGlueInfo->rTxCompleteTask);
	tasklet_kill(&prGlueInfo->rRxTask);

	/* 4 <8> Unregister early suspend callback */
#if CFG_ENABLE_EARLY_SUSPEND
	DBGLOG(INIT, STATE, "call glUnregisterEarlySuspend\n");
	glUnregisterEarlySuspend(&wlan_early_suspend_desc);
#endif

	gprWdev->netdev = NULL;

	/* 4 <9> Unregister notifier callback */
	wlanUnregisterNotifier();

	DBGLOG(INIT, STATE, "Remove wlan done\n");
} /* end of wlanRemove() */

static int mt76x8_wireless_init(void)
{
	int ret = 0;

	DBGLOG(INIT, STATE, "Initialize wireless Device\n");

	/* memory pre-allocation */
#if CFG_PRE_ALLOCATION_IO_BUFFER
	kalInitIOBuffer(TRUE);
#else
	kalInitIOBuffer(FALSE);
#endif

#if WLAN_INCLUDE_PROC
	procInitFs();
#endif

	wlanCreateWirelessDevice();
	if (gprWdev)
		glP2pCreateWirelessDevice((P_GLUE_INFO_T)wiphy_priv(gprWdev->wiphy));
	gprP2pWdev = gprP2pRoleWdev[0]; /* P2PDev and P2PRole[0] share the same Wdev */

	ret = ((glRegisterBus(wlanProbe, wlanRemove) == WLAN_STATUS_SUCCESS) ? 0 : -EIO);

	if (ret == -EIO) {
		kalUninitIOBuffer();
		return ret;
	}

	return ret;
}

static VOID mt76x8_wireless_exit(void)
{
	DBGLOG(INIT, STATE, "wireless Device exit\n");

	glUnregisterBus(wlanRemove);
	/* free pre-allocated memory */
	kalUninitIOBuffer();
	wlanDestroyWirelessDevice();
	glP2pDestroyWirelessDevice();
#if WLAN_INCLUDE_PROC
	procUninitProcFs();
#endif
	wlanUnregisterRebootNotifier();
}

#ifdef CFG_SUPPORT_MT76X8_WIFI_PLATFORM_DRIVER

struct mt76x8_wifi_priv {
	int		 reset_gpio;
	uint32_t reset_delay_ms;
};

static int mt76x8_reset_chip(struct mt76x8_wifi_priv *wifi)
{
	if (!gpio_is_valid(wifi->reset_gpio))
		return -EINVAL;

	DBGLOG(INIT, STATE, "Resetting wifi chip\n");

	gpio_direction_output(wifi->reset_gpio, 0);
	mdelay(wifi->reset_delay_ms);
	gpio_direction_output(wifi->reset_gpio, 1);

	return 0;
}

static int mt76x8_wifi_probe(struct platform_device *pdev)
{
	struct mt76x8_wifi_priv *wifi;
	struct device			  *dev = &pdev->dev;
	struct device_node	   *np	 = pdev->dev.of_node;
	int						 gpio, ret = 0;
	const char			   *pwr_limit_file;

	wifi = devm_kzalloc(dev, sizeof(struct mt76x8_wifi_priv), GFP_KERNEL);
	if (!wifi)
		return -ENOMEM;

	platform_set_drvdata(pdev, wifi);

	if (np) {
		gpio = of_get_named_gpio(np, "reset-gpio", 0);

		if (gpio_is_valid(gpio)) {
			wifi->reset_gpio = gpio;

			ret = of_property_read_u32(np, "reset-delay-ms", &wifi->reset_delay_ms);
			if (ret) {
				DBGLOG(INIT, WARN, "No defined reset delay value in dt, use default value\n");
				wifi->reset_delay_ms = MT76X8_WIFI_RESET_DEFAULT_DELAY_MS;
			}

			ret = mt76x8_reset_chip(wifi);
		} else {
			DBGLOG(INIT, WARN, "no reset gpio provided in dt, will not HW reset device\n");
		}

		/* overriding default power limit file if specified */
		if (!of_property_read_string(np, "tx_pwr_limit_file_override", &pwr_limit_file)) {
			rlmDomainOverridePwrLimitFileName(pwr_limit_file);
		}

		ret = mt76x8_wireless_init();
	}

	return ret;
}

static int mt76x8_wifi_remove(struct platform_device *pdev)
{
	DBGLOG(INIT, INFO, "remove wifi driver\n");

	mt76x8_wireless_exit();

	return 0;
}

static const struct of_device_id mt76x8_wifi_ids[] = {
	{
			.compatible = "mediatek,mt76x8_wifi_sdio",
	},
	{ /* end of table */ },
};

MODULE_DEVICE_TABLE(of, mt76x8_wifi_ids);

static struct platform_driver mt76x8_wifi_driver = {
	.driver = {
			.name			= DRIVER_NAME,
			.owner			= THIS_MODULE,
			.of_match_table = mt76x8_wifi_ids,
	},
	.probe	= mt76x8_wifi_probe,
	.remove = mt76x8_wifi_remove,
};

static int registerDriver(void)
{
	int ret = 0;

	ret = platform_driver_register(&mt76x8_wifi_driver);
	if (ret)
		DBGLOG(INIT, ERROR, "faild to register mt76x8_wifi_driver(%d)\n", ret);

	return ret;
}

static VOID unregisterDriver(void)
{
	platform_driver_unregister(&mt76x8_wifi_driver);
}

#else
static int registerDriver(void)
{
	return mt76x8_wireless_init();
}

static VOID unregisterDriver(void)
{
	mt76x8_wireless_exit();
}

#endif

/*----------------------------------------------------------------------------*/
/*!
 * \brief Driver entry point when the driver is configured as a Linux Module, and
 *        is called once at module load time, by the user-level modutils
 *        application: insmod or modprobe.
 *
 * \retval 0     Success
 */
/*----------------------------------------------------------------------------*/
/* 1 Module Entry Point */
static int initWlan(void)
{
	int ret = 0;

#ifdef CFG_DRIVER_INF_NAME_CHANGE

	if (kalStrLen(gprifnamesta) > CUSTOM_IFNAMESIZ || kalStrLen(gprifnamep2p) > CUSTOM_IFNAMESIZ ||
			kalStrLen(gprifnameap) > CUSTOM_IFNAMESIZ) {
		DBGLOG(INIT, ERROR, "custom infname len illegal > %d\n", CUSTOM_IFNAMESIZ);
		return -EINVAL;
	}

#endif /*  CFG_DRIVER_INF_NAME_CHANGE */

	wlanDebugInit();

	DBGLOG(INIT, STATE, "initWlan..\n");

	registerDriver();

	DBGLOG(INIT, STATE, "initWlan end\n");

	return ret;
} /* end of initWlan() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Driver exit point when the driver as a Linux Module is removed. Called
 *        at module unload time, by the user level modutils application: rmmod.
 *        This is our last chance to clean up after ourselves.
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
/* 1 Module Leave Point */
static VOID exitWlan(void)
{
	/* printk("remove %p\n", wlanRemove); */
	DBGLOG(INIT, STATE, "exitWlan\n");

	if (prWaitForResetComp)
		wait_for_completion_interruptible_timeout(prWaitForResetComp, SEC_TO_SYSTIME(5));

	unregisterDriver();
	DBGLOG(INIT, STATE, "exitWlan end\n");
} /* end of exitWlan() */

static int mt7668s_reboot_notify(struct notifier_block *nb, unsigned long event, void *unused)
{
	if (event == SYS_DOWN || event == SYS_RESTART || event == SYS_POWER_OFF) {
		DBGLOG(HAL, STATE, "Power down is detected. Cleaning MT7668S WiFi driver...\n");

		glUnregisterBus(wlanRemove);
		kalUninitIOBuffer();
		wlanDestroyWirelessDevice();
		glP2pDestroyWirelessDevice();
#if WLAN_INCLUDE_PROC
		procUninitProcFs();
#endif

		DBGLOG(HAL, STATE, "Cleaning MT7668S WiFi driver Finish!\n");
	}
	return 0;
}

static struct notifier_block mt7668s_reboot_notifier = {
	.notifier_call = mt7668s_reboot_notify,
	.next		   = NULL,
	.priority	   = __INT_MAX__,
};

void wlanRegisterRebootNotifier(void)
{
	register_reboot_notifier(&mt7668s_reboot_notifier);
}

void wlanUnregisterRebootNotifier(void)
{
	unregister_reboot_notifier(&mt7668s_reboot_notifier);
}

module_init(initWlan);
module_exit(exitWlan);
