// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2016 MediaTek Inc.
 */

/*! \file   "rate.c"
 *    \brief  This file contains the transmission rate handling routines.
 *
 *    This file contains the transmission rate handling routines for setting up
 *    ACK/CTS Rate, Highest Tx Rate, Lowest Tx Rate, Initial Tx Rate and do
 *    conversion between Rate Set and Data Rates.
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
/* The list of valid data rates. */
const UINT_8 aucDataRate[] = {
	RATE_1M,	  /* RATE_1M_INDEX = 0 */
	RATE_2M,	  /* RATE_2M_INDEX */
	RATE_5_5M,	  /* RATE_5_5M_INDEX */
	RATE_11M,	  /* RATE_11M_INDEX */
	RATE_22M,	  /* RATE_22M_INDEX */
	RATE_33M,	  /* RATE_33M_INDEX */
	RATE_6M,	  /* RATE_6M_INDEX */
	RATE_9M,	  /* RATE_9M_INDEX */
	RATE_12M,	  /* RATE_12M_INDEX */
	RATE_18M,	  /* RATE_18M_INDEX */
	RATE_24M,	  /* RATE_24M_INDEX */
	RATE_36M,	  /* RATE_36M_INDEX */
	RATE_48M,	  /* RATE_48M_INDEX */
	RATE_54M,	  /* RATE_54M_INDEX */
	RATE_VHT_PHY, /* RATE_VHT_PHY_INDEX */
	RATE_HT_PHY,  /* RATE_HT_PHY_INDEX */
#if CFG_SUPPORT_H2E
	RATE_H2E_ONLY /* RATE_H2E_ONLY_INDEX */
#endif
};

static const UINT_8 aucDefaultAckCtsRateIndex[RATE_NUM_SW] = {
	RATE_1M_SW_INDEX,	/* RATE_1M_SW_INDEX = 0 */
	RATE_2M_SW_INDEX,	/* RATE_2M_SW_INDEX */
	RATE_5_5M_SW_INDEX, /* RATE_5_5M_SW_INDEX */
	RATE_11M_SW_INDEX,	/* RATE_11M_SW_INDEX */
	RATE_1M_SW_INDEX,	/* RATE_22M_SW_INDEX - Not supported */
	RATE_1M_SW_INDEX,	/* RATE_33M_SW_INDEX - Not supported */
	RATE_6M_SW_INDEX,	/* RATE_6M_SW_INDEX */
	RATE_6M_SW_INDEX,	/* RATE_9M_SW_INDEX */
	RATE_12M_SW_INDEX,	/* RATE_12M_SW_INDEX */
	RATE_12M_SW_INDEX,	/* RATE_18M_SW_INDEX */
	RATE_24M_SW_INDEX,	/* RATE_24M_SW_INDEX */
	RATE_24M_SW_INDEX,	/* RATE_36M_SW_INDEX */
	RATE_24M_SW_INDEX,	/* RATE_48M_SW_INDEX */
	RATE_24M_SW_INDEX	/* RATE_54M_SW_INDEX */
};

const BOOLEAN afgIsOFDMRate[RATE_NUM_SW] = {
	FALSE, /* RATE_1M_INDEX = 0 */
	FALSE, /* RATE_2M_INDEX */
	FALSE, /* RATE_5_5M_INDEX */
	FALSE, /* RATE_11M_INDEX */
	FALSE, /* RATE_22M_INDEX - Not supported */
	FALSE, /* RATE_33M_INDEX - Not supported */
	TRUE,  /* RATE_6M_INDEX */
	TRUE,  /* RATE_9M_INDEX */
	TRUE,  /* RATE_12M_INDEX */
	TRUE,  /* RATE_18M_INDEX */
	TRUE,  /* RATE_24M_INDEX */
	TRUE,  /* RATE_36M_INDEX */
	TRUE,  /* RATE_48M_INDEX */
	TRUE   /* RATE_54M_INDEX */
};

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
 * @brief Convert the given Supported Rate & Extended Supported Rate IE to the
 *        Operational Rate Set and Basic Rate Set, and also check if any Basic
 *        Rate Code is unknown by driver.
 *
 * @param[in] prIeSupportedRate          Pointer to the Supported Rate IE
 * @param[in] prIeExtSupportedRate       Pointer to the Ext Supported Rate IE
 * @param[out] pu2OperationalRateSet     Pointer to the Operational Rate Set
 * @param[out] pu2BSSBasicRateSet        Pointer to the Basic Rate Set
 * @param[out] pfgIsUnknownBSSBasicRate  Pointer to a Flag to indicate that Basic
 *                                       Rate Set has unknown Rate Code
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
VOID rateGetRateSetFromIEs(IN P_IE_SUPPORTED_RATE_IOT_T prIeSupportedRate,
		IN P_IE_EXT_SUPPORTED_RATE_T prIeExtSupportedRate, OUT PUINT_16 pu2OperationalRateSet,
		OUT PUINT_16 pu2BSSBasicRateSet, OUT PBOOLEAN pfgIsUnknownBSSBasicRate)
{
	UINT_16 u2OperationalRateSet	= 0;
	UINT_16 u2BSSBasicRateSet		= 0;
	BOOLEAN fgIsUnknownBSSBasicRate = FALSE;
	UINT_8	ucRate;
	UINT_32 i, j;

	ASSERT(pu2OperationalRateSet);
	ASSERT(pu2BSSBasicRateSet);
	ASSERT(pfgIsUnknownBSSBasicRate);

	if (prIeSupportedRate) {
		/* NOTE(Kevin): Buffalo WHR-G54S's supported rate set IE exceed 8.
		 * IE_LEN(pucIE) == 12, "1(B), 2(B), 5.5(B), 6(B), 9(B), 11(B),
		 * 12(B), 18(B), 24(B), 36(B), 48(B), 54(B)"
		 */
		/* ASSERT(prIeSupportedRate->ucLength <= ELEM_MAX_LEN_SUP_RATES); */
		if (prIeSupportedRate->ucLength > ELEM_MAX_LEN_SUP_RATES_IOT) {
			*pu2OperationalRateSet	  = 0;
			*pu2BSSBasicRateSet		  = 0;
			*pfgIsUnknownBSSBasicRate = TRUE;
			return;
		}

		for (i = 0; i < prIeSupportedRate->ucLength; i++) {
			ucRate = prIeSupportedRate->aucSupportedRates[i] & RATE_MASK;

			/* Search all valid data rates */
			for (j = 0; j < sizeof(aucDataRate) / sizeof(UINT_8); j++) {
				if (ucRate == aucDataRate[j]) {
					u2OperationalRateSet |= BIT(j);

					if (prIeSupportedRate->aucSupportedRates[i] & RATE_BASIC_BIT)
						u2BSSBasicRateSet |= BIT(j);

					break;
				}
			}

			if ((j == sizeof(aucDataRate) / sizeof(UINT_8)) &&
					(prIeSupportedRate->aucSupportedRates[i] & RATE_BASIC_BIT)) {
				fgIsUnknownBSSBasicRate = TRUE; /* A data rate not list in the aucDataRate[] */
			}
		}
	}

	if (prIeExtSupportedRate) {
		/* ASSERT(prIeExtSupportedRate->ucLength <= ELEM_MAX_LEN_EXTENDED_SUP_RATES); */
		for (i = 0; i < prIeExtSupportedRate->ucLength; i++) {
			ucRate = prIeExtSupportedRate->aucExtSupportedRates[i] & RATE_MASK;

			/* Search all valid data rates */
			for (j = 0; j < sizeof(aucDataRate) / sizeof(UINT_8); j++) {
				if (ucRate == aucDataRate[j]) {
					u2OperationalRateSet |= BIT(j);

					if (prIeExtSupportedRate->aucExtSupportedRates[i] & RATE_BASIC_BIT)
						u2BSSBasicRateSet |= BIT(j);

					break;
				}
			}

			if ((j == sizeof(aucDataRate) / sizeof(UINT_8)) &&
					(prIeExtSupportedRate->aucExtSupportedRates[i] & RATE_BASIC_BIT)) {
				fgIsUnknownBSSBasicRate = TRUE; /* A data rate not list in the aucDataRate[] */
			}
		}
	}

	*pu2OperationalRateSet	  = u2OperationalRateSet;
	*pu2BSSBasicRateSet		  = u2BSSBasicRateSet;
	*pfgIsUnknownBSSBasicRate = fgIsUnknownBSSBasicRate;

	return;

} /* end of rateGetRateSetFromIEs() */

/*----------------------------------------------------------------------------*/
/*!
 * @brief Convert the given Operational Rate Set & Basic Rate Set to the Rate Code
 *        Format for used in (Ext)Supportec Rate IE.
 *
 * @param[in] u2OperationalRateSet   Operational Rate Set
 * @param[in] u2BSSBasicRateSet      Basic Rate Set
 * @param[out] pucDataRates          Pointer to the Data Rate Buffer
 * @param[out] pucDataRatesLen       Pointer to the Data Rate Buffer Length
 *
 * @return (none)
 */
/*----------------------------------------------------------------------------*/
VOID rateGetDataRatesFromRateSet(IN UINT_16 u2OperationalRateSet, IN UINT_16 u2BSSBasicRateSet,
		OUT PUINT_8 pucDataRates, OUT PUINT_8 pucDataRatesLen)
{
	UINT_32 i, j;

	ASSERT(pucDataRates);
	ASSERT(pucDataRatesLen);

	ASSERT(u2BSSBasicRateSet == (u2OperationalRateSet & u2BSSBasicRateSet));

	for (i = RATE_1M_SW_INDEX, j = 0; i < RATE_NUM_SW; i++) {
		if (u2OperationalRateSet & BIT(i)) {
			*(pucDataRates + j) = aucDataRate[i];

			if (u2BSSBasicRateSet & BIT(i))
				*(pucDataRates + j) |= RATE_BASIC_BIT;

			j++;
		}
	}

	*pucDataRatesLen = (UINT_8)j;

	return;

} /* end of rateGetDataRatesFromRateSet() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Get the highest rate from given Rate Set.
 *
 * \param[in] u2RateSet              Rate Set
 * \param[out] pucHighestRateIndex   Pointer to buffer of the Highest Rate Index
 *
 * \retval TRUE  Highest Rate Index was found
 * \retval FALSE Highest Rate Index was not found
 */
/*----------------------------------------------------------------------------*/
BOOLEAN rateGetHighestRateIndexFromRateSet(IN UINT_16 u2RateSet, OUT PUINT_8 pucHighestRateIndex)
{
	INT_32 i;

	ASSERT(pucHighestRateIndex);

	for (i = RATE_54M_SW_INDEX; i >= RATE_1M_SW_INDEX; i--) {
		if (u2RateSet & BIT(i)) {
			*pucHighestRateIndex = (UINT_8)i;
			return TRUE;
		}
	}

	return FALSE;

} /* end of rateGetHighestRateIndexFromRateSet() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Get the lowest rate from given Rate Set.
 *
 * \param[in] u2RateSet              Rate Set
 * \param[out] pucLowestRateIndex    Pointer to buffer of the Lowest Rate Index
 *
 * \retval TRUE  Lowest Rate Index was found
 * \retval FALSE Lowest Rate Index was not found
 */
/*----------------------------------------------------------------------------*/
BOOLEAN rateGetLowestRateIndexFromRateSet(IN UINT_16 u2RateSet, OUT PUINT_8 pucLowestRateIndex)
{
	UINT_32 i;

	ASSERT(pucLowestRateIndex);

	for (i = RATE_1M_SW_INDEX; i <= RATE_54M_SW_INDEX; i++) {
		if (u2RateSet & BIT(i)) {
			*pucLowestRateIndex = (UINT_8)i;
			return TRUE;
		}
	}

	return FALSE;

} /* end of rateGetLowestRateIndexFromRateSet() */
