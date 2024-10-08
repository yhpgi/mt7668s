// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   "mib.c"
 *    \brief  This file includes the mib default vale and functions.
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

NON_HT_PHY_ATTRIBUTE_T rNonHTPhyAttributes[] = {
	{ RATE_SET_HR_DSSS, true, false }, /* For PHY_TYPE_HR_DSSS_INDEX(0) */
	{ RATE_SET_ERP, true, true }, /* For PHY_TYPE_ERP_INDEX(1) */
	{ RATE_SET_ERP_P2P, true, true }, /* For PHY_TYPE_ERP_P2P_INDEX(2) */
	{ RATE_SET_OFDM, false, false }, /* For PHY_TYPE_OFDM_INDEX(3) */
};

NON_HT_ADHOC_MODE_ATTRIBUTE_T rNonHTAdHocModeAttributes[AD_HOC_MODE_NUM] = {
	{ PHY_TYPE_HR_DSSS_INDEX, BASIC_RATE_SET_HR_DSSS }, /* For
	                                                     * AD_HOC_MODE_11B(0)
	                                                     */
	{ PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_HR_DSSS_ERP }, /* For
	                                                     * AD_HOC_MODE_MIXED_11BG(1)
	                                                     */
	{ PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_ERP }, /* For AD_HOC_MODE_11G(2) */
	{ PHY_TYPE_OFDM_INDEX, BASIC_RATE_SET_OFDM }, /* For AD_HOC_MODE_11A(3)
	                                               */
};

NON_HT_AP_MODE_ATTRIBUTE_T rNonHTApModeAttributes[AP_MODE_NUM] = {
	{ PHY_TYPE_HR_DSSS_INDEX, BASIC_RATE_SET_HR_DSSS }, /* For
	                                                     * AP_MODE_11B(0) */
	{ PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_HR_DSSS_ERP }, /* For
	                                                     * AP_MODE_MIXED_11BG(1)
	                                                     */
	{ PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_ERP }, /* For AP_MODE_11G(2) */
	{ PHY_TYPE_ERP_P2P_INDEX, BASIC_RATE_SET_ERP_P2P }, /* For
	                                                     * AP_MODE_11G_P2P(3)
	                                                     */
	{ PHY_TYPE_OFDM_INDEX, BASIC_RATE_SET_OFDM }, /* For AP_MODE_11A(4) */
};

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
