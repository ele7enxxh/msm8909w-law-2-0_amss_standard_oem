/*!
  @file
  hwio_cap.c

  @brief
  HWIO capability file.
*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/hwio/src/hwio_cap.c#1 $
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "mcs_variation.h"
#include <customer.h>
#include "msmhwioreg.h"
#include <DDIChipInfo.h>
#include "hwio_cap.h"
#include <msg_diag_service.h>

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  #if defined(FEATURE_MCS_BOLT) || defined (FEATURE_MCS_THOR)
    #define MCSHWIO_READ_CAP() HWIO_IN(MSS_SW_FEATURE_FUSES)
  #else
    #if defined(FEATURE_MCS_JOLOKIA) || defined(FEATURE_MCS_TABASCO)
      #define MCSHWIO_READ_CAP() HWIO_IN(MODEM_FEATURE_EN)
    #else
      #define MCSHWIO_READ_CAP() HWIO_IN(MODEM_OPTION_EN)
    #endif
  #endif
#endif /* TEST_FRAMEWORK */

 
  
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*----------------------------------------------------------------------------
  Define special values to denote capability supported and unsupported. These 
  allow us to define capability (as needed), when either the corresponding bit
  is missing in hardware or the respective feature is not applicable for a 
  particular modem.
----------------------------------------------------------------------------*/
#define CAP_SUPPORTED    0xFE
#define CAP_UNSUPPORTED  0xFF

typedef enum
{
  TORINO_1X = 0,
  TORINO_DO = 2,
  TORINO_WCDMA = 4,
  TORINO_HSDPA = 6,
  TORINO_HSDPA_MIMO = 8,
  TORINO_HSDPA_DC = 10,
  TORINO_LTE = 12,
  TORINO_LTE_ABOVE_CAT2 = 14,
  TORINO_LTE_ABOVE_CAT1 = 16,
  TORINO_TDSCDMA = 18,
  TORINO_GERAN = 20,
  TORINO_LTE_40MHZ = 22,
  TORINO_LTE_60MHZ = 23,
  TORINO_LTE_UL_CA = 24,
  TORINO_ADC2 = 25,
  TORINO_ADC3 = 26,
  TORINO_MODEM = 27,
  TORINO_DSDA = 28,
  TORINO_SUPPORTED = CAP_SUPPORTED,
  TORINO_UNSUPPORTED = CAP_UNSUPPORTED
}modem_cap_torino;

typedef enum
{
  TESLA_1X = 0,
  TESLA_DO = 2,
  TESLA_WCDMA = 4,
  TESLA_HSDPA = 6,
  TESLA_HSDPA_MIMO = 8,
  TESLA_HSDPA_DC = 10,
  TESLA_LTE = 12,
  TESLA_LTE_ABOVE_CAT2 = 14,
  TESLA_LTE_ABOVE_CAT1 = 16,
  TESLA_TDSCDMA = 18,
  TESLA_GERAN = 20,
  TESLA_MODEM = 27,
  TESLA_SUPPORTED = CAP_SUPPORTED,
  TESLA_UNSUPPORTED = CAP_UNSUPPORTED
}modem_cap_tesla;

typedef enum
{
  CHEEL_GERAN = 0,
  CHEEL_1X = 1,
  CHEEL_DO = 2,
  CHEEL_WCDMA = 3,
  CHEEL_HSDPA = 4,
  CHEEL_LTE = 5,
  CHEEL_MODEM = 6,
  CHEEL_TDSCDMA = 7,
  CHEEL_NAV = 8,
  CHEEL_HSDPA_MIMO = 9,
  CHEEL_HSDPA_DC = 10,
  CHEEL_LTE_ABOVE_CAT1 = 11,
  CHEEL_LTE_ABOVE_CAT2 = 12,
  CHEEL_DSDA = 13,
  CHEEL_SUPPORTED = CAP_SUPPORTED,
  CHEEL_UNSUPPORTED = CAP_UNSUPPORTED
}modem_cap_cheel;

/* NOTE- Feero bit mappings are same as cheel, except for a new bit addition for LTE_UL_64QAM */
typedef enum
{
  FEERO_GERAN = 0,
  FEERO_1X = 1,
  FEERO_DO = 2,
  FEERO_WCDMA = 3,
  FEERO_HSDPA = 4,
  FEERO_LTE = 5,
  FEERO_MODEM = 6,
  FEERO_TDSCDMA = 7,
  FEERO_NAV = 8,
  FEERO_HSDPA_MIMO = 9,
  FEERO_HSDPA_DC = 10,
  FEERO_LTE_ABOVE_CAT1 = 11,
  FEERO_LTE_ABOVE_CAT2 = 12,
  FEERO_DSDA = 13,
  FEERO_LTE_UL_64QAM = 14,
  FEERO_SUPPORTED = CAP_SUPPORTED,
  FEERO_UNSUPPORTED = CAP_UNSUPPORTED
}modem_cap_feero;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
modem_cap_torino torino_bitpositions[MCS_MODEM_NUM_CAPABILITY]=
{
  /* MCS_MODEM_CAPABILITY_FEATURE_GSM */                  TORINO_GERAN,
  /* MCS_MODEM_CAPABILITY_FEATURE_1X */                   TORINO_1X,
  /* MCS_MODEM_CAPABILITY_FEATURE_DO */                   TORINO_DO,
  /* MCS_MODEM_CAPABILITY_FEATURE_WCDMA*/                 TORINO_WCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA */                 TORINO_HSDPA,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE */                  TORINO_LTE,
  /* MCS_MODEM_CAPABILITY_FEATURE_TDSCDMA */              TORINO_TDSCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_MODEM */                TORINO_MODEM,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_ABOVE_CAT14 */     TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_NAV */                  TORINO_SUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_MIMO */            TORINO_HSDPA_MIMO,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT1 */       TORINO_LTE_ABOVE_CAT1,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT2 */       TORINO_LTE_ABOVE_CAT2,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX2 */            TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX3 */            TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT8 */   TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT10 */  TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_MCDO */                 TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_UL_ABOVE_CAT6 */   TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_RXD */                  TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_DSDA */                 TORINO_DSDA,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSDPA_DC */             TORINO_HSDPA_DC,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_40MHZ */            TORINO_LTE_40MHZ,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_60MHZ */            TORINO_LTE_60MHZ,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_CA */            TORINO_LTE_UL_CA,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC2 */                 TORINO_ADC2,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC3 */                 TORINO_ADC3,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSUPA_DC */             TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4 */       TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4_SW */    TORINO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_64QAM */         TORINO_UNSUPPORTED
};

modem_cap_tesla tesla_bitpositions[MCS_MODEM_NUM_CAPABILITY]=
{
  /* MCS_MODEM_CAPABILITY_FEATURE_GSM */                  TESLA_GERAN,
  /* MCS_MODEM_CAPABILITY_FEATURE_1X */                   TESLA_1X,
  /* MCS_MODEM_CAPABILITY_FEATURE_DO */                   TESLA_DO,
  /* MCS_MODEM_CAPABILITY_FEATURE_WCDMA*/                 TESLA_WCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA */                 TESLA_HSDPA,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE */                  TESLA_LTE,
  /* MCS_MODEM_CAPABILITY_FEATURE_TDSCDMA */              TESLA_TDSCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_MODEM */                TESLA_MODEM,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_ABOVE_CAT14 */     TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_NAV */                  TESLA_SUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_MIMO */            TESLA_HSDPA_MIMO,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT1 */       TESLA_LTE_ABOVE_CAT1,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT2 */       TESLA_LTE_ABOVE_CAT2,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX2 */            TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX3 */            TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT8 */   TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT10 */  TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_MCDO */                 TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_UL_ABOVE_CAT6 */   TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_RXD */                  TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_DSDA */                 TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSDPA_DC */             TESLA_HSDPA_DC,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_40MHZ */            TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_60MHZ */            TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_CA */            TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC2 */                 TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC3 */                 TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSUPA_DC */             TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4 */       TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4_SW */    TESLA_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_64QAM */         TESLA_UNSUPPORTED
};

modem_cap_cheel cheel_bitpositions[MCS_MODEM_NUM_CAPABILITY]=
{
  /* MCS_MODEM_CAPABILITY_FEATURE_GSM */                  CHEEL_GERAN,
  /* MCS_MODEM_CAPABILITY_FEATURE_1X */                   CHEEL_1X,
  /* MCS_MODEM_CAPABILITY_FEATURE_DO */                   CHEEL_DO,
  /* MCS_MODEM_CAPABILITY_FEATURE_WCDMA*/                 CHEEL_WCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA */                 CHEEL_HSDPA,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE */                  CHEEL_LTE,
  /* MCS_MODEM_CAPABILITY_FEATURE_TDSCDMA */              CHEEL_TDSCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_MODEM */                CHEEL_MODEM,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_ABOVE_CAT14 */     CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_NAV */                  CHEEL_NAV,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_MIMO */            CHEEL_HSDPA_MIMO,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT1 */       CHEEL_LTE_ABOVE_CAT1,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT2 */       CHEEL_LTE_ABOVE_CAT2,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX2 */            CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX3 */            CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT8 */   CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT10 */  CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_MCDO */                 CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_UL_ABOVE_CAT6 */   CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_RXD */                  CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_DSDA */                 CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSDPA_DC */             CHEEL_HSDPA_DC,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_40MHZ */            CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_60MHZ */            CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_CA */            CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC2 */                 CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC3 */                 CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSUPA_DC */             CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4 */       CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4_SW */    CHEEL_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_64QAM */         CHEEL_UNSUPPORTED
};

modem_cap_feero feero_bitpositions[MCS_MODEM_NUM_CAPABILITY]=
{
  /* MCS_MODEM_CAPABILITY_FEATURE_GSM */                  FEERO_GERAN,
  /* MCS_MODEM_CAPABILITY_FEATURE_1X */                   FEERO_1X,
  /* MCS_MODEM_CAPABILITY_FEATURE_DO */                   FEERO_DO,
  /* MCS_MODEM_CAPABILITY_FEATURE_WCDMA*/                 FEERO_WCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA */                 FEERO_HSDPA,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE */                  FEERO_LTE,
  /* MCS_MODEM_CAPABILITY_FEATURE_TDSCDMA */              FEERO_TDSCDMA,
  /* MCS_MODEM_CAPABILITY_FEATURE_MODEM */                FEERO_MODEM,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_ABOVE_CAT14 */     FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_NAV */                  FEERO_NAV,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSPA_MIMO */            FEERO_HSDPA_MIMO,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT1 */       FEERO_LTE_ABOVE_CAT1,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT2 */       FEERO_LTE_ABOVE_CAT2,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX2 */            FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX3 */            FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT8 */   FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT10 */  FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_MCDO */                 FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_UMTS_UL_ABOVE_CAT6 */   FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_RXD */                  FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_DSDA */                 FEERO_DSDA,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSDPA_DC */             FEERO_HSDPA_DC,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_40MHZ */            FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_60MHZ */            FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_CA */            FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC2 */                 FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_ADC3 */                 FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_HSUPA_DC */             FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4 */       FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4_SW */    FEERO_UNSUPPORTED,
  /* MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_64QAM */         FEERO_LTE_UL_64QAM
};

typedef struct
{
  uint32 data;
  DalChipInfoFamilyType chipType;
}modem_capability_type;

/*! Global state for modem capability */
static modem_capability_type modem_cap;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*============================================================================

FUNCTION HWIO_CAP_INIT

DESCRIPTION
  Reads the modem capability information and caches it.
  
DEPENDENCIES
  Must be called by RC INIT before any other functions in this
  file are called.

RETURN VALUE
  mcs_modem_cap_return_enum

SIDE EFFECTS
  None

============================================================================*/
void hwio_cap_init(void)
{

  /* Get chip type */
  modem_cap.chipType = DalChipInfo_ChipFamily();

  #ifdef FEATURE_GNSS_SA
  /* Not applicable for GPS-only builds */
  modem_cap.data = 0;
  #else
  /* Get all hw capabilities */
  modem_cap.data = MCSHWIO_READ_CAP();
  #endif
}


/*============================================================================

FUNCTION MCS_MODEM_HAS_CAPABILITY

DESCRIPTION
  Returns if modem is capable of supporting queried modem technology.
  
DEPENDENCIES
  None

RETURN VALUE
  mcs_modem_cap_return_enum

SIDE EFFECTS
  None

============================================================================*/
mcs_modem_cap_return_enum mcs_modem_has_capability
(
  mcs_modem_capability_enum capability /* capability to check */
)
{
  mcs_modem_cap_return_enum return_value = MCS_MODEM_CAP_UNKNOWN;
  uint8 bitposition;

  /* Get bit position based on the chip type */
  switch (modem_cap.chipType)
  {
    case DALCHIPINFO_FAMILY_MSM8909:
    case DALCHIPINFO_FAMILY_MSM8952:
    case DALCHIPINFO_FAMILY_MDM9x07:
    {
      bitposition = (uint8)cheel_bitpositions[(uint8)capability];
    }
    break;
    case DALCHIPINFO_FAMILY_MDM9x45:
    {
      bitposition = (uint8)tesla_bitpositions[(uint8)capability];
    }
    break;

    case DALCHIPINFO_FAMILY_MDM9x35:
    case DALCHIPINFO_FAMILY_MSM8994:
    {
      bitposition = (uint8)torino_bitpositions[(uint8)capability];
    }
    break;
    case DALCHIPINFO_FAMILY_MSM8937:
    case DALCHIPINFO_FAMILY_MSM8917:
    {
      bitposition = (uint8)feero_bitpositions[(uint8)capability];
    }
    break;
    default:
      bitposition = CAP_UNSUPPORTED;
      break;
  }

  /* Not all features exist in every chip. If capability is not supported, return unavailable. */
  if (bitposition == CAP_UNSUPPORTED)
  {
    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "Returning Unsupported capability %d for requested bit %d on chip %d",
          (uint8)MCS_MODEM_CAP_UNKNOWN, 
          (uint8)capability,
          modem_cap.chipType);
    return_value = MCS_MODEM_CAP_UNKNOWN;
  }
  /* Check if capability is supported by default (usually due to a missing hardware bit or fuse).
     If yes, return available. */
  else if (bitposition == CAP_SUPPORTED)
  {
    return_value = MCS_MODEM_CAP_AVAILABLE;
  }
  else
  {
    /* Check if the required bit is set */
    if (modem_cap.data & (1<<bitposition))
    {
      return_value = MCS_MODEM_CAP_AVAILABLE;
    }
    else
    {
      return_value = MCS_MODEM_CAP_UNAVAILABLE;
      MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "Modem Option Register 0x%x; Capability Requested %d, Chip %d", 
            modem_cap.data, capability, modem_cap.chipType);
    }
  }

  return return_value;
}
