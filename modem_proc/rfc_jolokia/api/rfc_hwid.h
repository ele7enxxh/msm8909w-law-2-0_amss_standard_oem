#ifndef RFC_HWID_H
#define RFC_HWID_H

/*===========================================================================


      R F  D r i v e r  C o m m o n  H a r d w a r e  I d e n t i f i e r

                            H e a d e r  F i l e

DESCRIPTION
  This file contains the Hardware IDs for RF Cards used across multiple
  layers of the RF driver.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1999 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved .
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/api/rfc_hwid.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/16   ds      Added support for rfc_wtr2965_dlca_ulca_4360_88xx
04/09/15   fhuo    Added support for RFC_WTR2955_CHINA_CMCC_5M_DLCA
03/19/15   rp      Added Dummy HW IDs to resolve compile time dependency on ULCA changes.
04/23/14   sd      Initial revision for DimePM 2.0 RFC HW IDs.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* -------------------------------------------------------
** The RF Card Id used in the target
** Note: The Id needs to be sequential
** ------------------------------------------------------- */
typedef enum {
  RF_HW_UNDEFINED                         = (uint8)0,
  RF_TARGET_NONE                          = RF_HW_UNDEFINED,
  RF_HW_WTR2965_V2_CHILE_CA_4320          = (uint8)18,
  RF_HW_WTR2965_V2_NA_CA_4320             = (uint8)19,
  RF_HW_WTR2965_V2_JP_CA_4320             = (uint8)20,
  RF_HW_WTR4905_NON_CA                    = (uint8)31,
  RF_HW_WTR4905_CHILE_SRLTE_V2       	  = (uint8)32,
  RF_HW_WTR4905_JAPAN_V2                  = (uint8)37,
  RF_HW_WTR4905_JAPAN_4320                = (uint8)38,
  RF_HW_WTR4905_CHILE_3G                  = (uint8)43,
  RF_HW_WTR4905_CHINA_CMCC_3M             = (uint8)56,
  RF_HW_WTR4905_CHINA_CU_4M               = (uint8)57,
  RF_HW_WTR4905_CHINA_CT_4M               = (uint8)58,
  RF_HW_WTR4905_OM                        = (uint8)60,
  RF_HW_WTR4905_AMX_ANTENNA               = (uint8)61,
  RF_HW_WTR2965_DLCA_ULCA_4360_88XX       = (uint8)157,
  RF_HW_WTR4905_CHINA_4320                = (uint8)169,
  RF_HW_WTR4905_NA_DLCA                   = (uint8)190,
  RF_HW_WTR4905_CHINA_TDS_4320            = (uint8)192,
  RF_HW_WTR4905_TMO                       = (uint8)199,
  RF_HW_WTR2965_SWOC_NA                   = (uint8)224,
  RF_HW_WTR2955_NON_CA_SAWLESS_4320       = (uint8)227,
  RF_HW_WTR2955_NON_CA_SAWLESS_V2         = (uint8)215,
  RF_HW_WTR2965_DUAL_WTR_4320       	  = (uint8)221,
  RF_HW_WTR2965_NON_CA2_4320              = (uint8)220,
  RF_HW_WTR2965_NON_CA2_4320_SRX          = (uint8)230,
  RF_HW_WTR2965_DUAL_WTR_4320_SRX         = (uint8)232,
  RF_HW_WTR2965_DUAL_WTR_4320_SRX_W       = (uint8)233,
  RF_HW_WTR2965_NON_CA2_4320_3G           = (uint8)240,
  RF_HW_WTR2965_DUAL_WTR_4320_D           = (uint8)231,
  RF_HW_WTR2965_QRD_NON_CA_4373_1         = (uint8)202,
  RF_HW_WTR2965_QRD_NON_CA_4373_2         = (uint8)204,
  RF_HW_WTR2965_QRD_NON_CA_4373_3         = (uint8)205,
  RF_HW_WTR2965_TMO                       = (uint8)206,
  RF_HW_WTR2965_NON_CA2_4320_SAW          = (uint8)219,
  RF_HW_WTR2965_DUAL_WTR_4320_GPS         = (uint8)223,
  RF_HW_WTR2965_NON_CA_4373_4351          = (uint8)203,
  RF_HW_WTR2965_NON_CA_4373_V2            = (uint8)164,
  RF_HW_WTR2965_NON_CA_4373_V2_SAW        = (uint8)165,
  RF_HW_WTR2965_DLCA_ULCA_4340_V2         = (uint8)156,
  RF_HW_WTR2965_V2_NA_CA_4320_B66         = (uint8)250,

  /* Add any new HW ID before this line */
  RF_HW_MAX,
  RF_HW_DEFAULT = RF_HW_MAX, /* Default card for Off-Target Builds */
  RF_HW_EFS_CARD                          = (uint8)191, /* EFS Card */

  /* The following definitions are not needed and will need to be removed  */
  RF_HW_WTR1605_SGLTE                     = RF_HW_MAX,
  RF_HW_WTR1605_SGLTE_DSDA                = RF_HW_MAX,
  RF_HW_WTR3925_TP130                     = RF_HW_MAX,
  RF_HW_WTR3925_TP130_2                   = RF_HW_MAX,
  RF_HW_WTR3925_TP130_2_CA                = RF_HW_MAX,  
  RF_HW_WTR3925_TP130_CA                  = RF_HW_MAX,
  RF_HW_WTR3925_TP160_0                   = RF_HW_MAX,
  RF_HW_WTR3925_FDD_ULCA_4K               = RF_HW_MAX,
  RF_HW_WTR3925_TDD_ULCA_DSDA_QFE4X       = RF_HW_MAX,
  RF_HW_WTR3925_TDD_ULCA_DSDA             = RF_HW_MAX, 
} rf_hw_type;

#ifdef __cplusplus
}
#endif

#endif  /* RFC_HWID_H */
