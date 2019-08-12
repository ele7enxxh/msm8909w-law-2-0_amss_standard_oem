#ifndef HWIO_CAP_H
#define HWIO_CAP_H

/*!
  @file   hwio_cap.h

  @brief  HWIO capability header file

  @detail
  Defines HWIO capability related types and interfaces.

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/hwio_cap.h#1 $

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>      /* Definition of basic types and macros */
#include <IxErrno.h>

/*===========================================================================

                    EXTERNAL DEFINES

===========================================================================*/

/*----------------------------------------------------------------------------
  Modem Capability
----------------------------------------------------------------------------*/
typedef enum {

  /* GSM Feature Capability */
  MCS_MODEM_CAPABILITY_FEATURE_GSM,
  
   /* 1X Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_1X,
  
   /* HDR Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_DO,
  
   /* WCDMA Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_WCDMA,
  
   /* HSPA Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_HSPA,
  
   /* LTE Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE,
  
   /* TDSCDMA Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_TDSCDMA,
  
   /* Modem Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_MODEM,
  
  /* HSPA_ABOVE_CAT14 Feature Capability.. Also covers HSPA_DC */ 
  MCS_MODEM_CAPABILITY_FEATURE_HSPA_ABOVE_CAT14,
  
   /* NAV Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_NAV,

  /* HSPA_MIMO Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_HSPA_MIMO,

  /* LTE_ABOVE_CAT1 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT1,

  /* LTE_ABOVE_CAT2 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT2,

  /* LTE_BBRX2 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX2,

  /* LTE_BBRX3 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_BBRX3,

  /* UMTS_DL_ABOVE_CAT8 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT8,

  /* UMTS_DL_ABOVE_CAT10 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_UMTS_DL_ABOVE_CAT10,

  /* MCDO Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_MCDO,

  /* UMTS_UL_ABOVE_CAT6 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_UMTS_UL_ABOVE_CAT6,

  /* RXD Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_RXD,

  /* DSDA Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_DSDA,

  /* HSDPA_DC Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_HSDPA_DC,

  /* LTE_40MHZ Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_40MHZ,

  /* LTE_60MHZ Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_60MHZ,

  /* LTE_UL_CA Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_CA,

  /* ADC2 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_ADC2,

  /* ADC3 Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_ADC3,

  /* HSUPA_DC Feature Capability */
  MCS_MODEM_CAPABILITY_FEATURE_HSUPA_DC,

  /* LTE_ABOVE_CAT4 Feature Capability */
  MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4,

  /* LTE_ABOVE_CAT4_SW Feature Capability */
  MCS_MODEM_CAPABILITY_FEATURE_LTE_ABOVE_CAT4_SW,

  /* LTE_UL_64QAM Feature Capability */ 
  MCS_MODEM_CAPABILITY_FEATURE_LTE_UL_64QAM,

  MCS_MODEM_NUM_CAPABILITY
} 
mcs_modem_capability_enum;

/*----------------------------------------------------------------------------
  Modem Capability Return Values
----------------------------------------------------------------------------*/
typedef enum
{
  /* modem is not capable */
  MCS_MODEM_CAP_UNAVAILABLE,

    /* modem is capable */
  MCS_MODEM_CAP_AVAILABLE,

    /* we could not deduce the capability due to some failures */
  MCS_MODEM_CAP_UNKNOWN = 0xFF,
}
mcs_modem_cap_return_enum;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*============================================================================

FUNCTION HWIO_CAP_INIT

DESCRIPTION
  Reads the modem capability information and caches it.
  
DEPENDENCIES
  None

RETURN VALUE
  mcs_modem_cap_return_enum

SIDE EFFECTS
  None

============================================================================*/
void hwio_cap_init(void);

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
  mcs_modem_capability_enum capability /* capability to check for */
);

#endif /* HWIO_CAP_H */
