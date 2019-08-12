#ifndef ONEX_LOCAL_DEFINES_H
#define ONEX_LOCAL_DEFINES_H

/*===========================================================================

DESCRIPTION
  Local configuration for CDMA Operation


  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <1x> */
#define FEATURE_GPSONE_FAST_RTC_SYNC
#define FEATURE_IS2000_REL_A_CC
#define FEATURE_IS2000_REL_A_CC_BCSMS_PROC_UNSCHED
#define FEATURE_IS2000_REL_A_CC_QPM
#define FEATURE_IS2000_REL_A_CC_SMMC
#define FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
#define FEATURE_IS2000_REL_A_REACH_10MS
#define FEATURE_IS2000_REL_A_REACH_5MS
#define FEATURE_MSM6500_DEC_DYNAMIC_CLK_OFF
#define FEATURE_PLCM_ENHANCEMENTS


/* includes for local cust files
 * Note - these files are only visible to 1x.  Any new FEATURE used by multiple SUs must be
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h
 * file generated for each SUs that uses the FEATURE. */
#include "custcdma.h"

#endif /* ONEX_LOCAL_DEFINES_H */
