#ifndef SRCH_DEP_H
#define SRCH_DEP_H
/*===========================================================================

DESCRIPTION
  This file contains the internal feature dependencies for searcher.  The
  purpose of this file is to aid in development by providing a method of
  documenting and validating feature dependencies.  This file should be
  updated every time a new feature is added or searcher internal feature
  inter-dependencies change.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2008 - 2013 Qualcomm Technologies, Inc. 
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


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/protected/srch_dep.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/19/13   cjb     Mainline FEATURE_1X_SRCH_ROT_ONLY_AFC and
                   FEATURE_1X_SRCH_AFC_SRL ( Removed VCTCXO support )
01/31/12   srk     Feature Clean Up.
01/23/12   srk     Feature Cleanup
01/13/12   srk     Feature Cleanup: Mainlined Always ON features
10/31/11   jtm     Feature clean up.
09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                   FEATURE_MODEM_1X_NIKEL.
08/18/11   vks     Clean up references to sample server which does not exist
                   anymore (since genesis).
02/17/11   cjb     Making the following features mutually exclusive:
                    FEATURE_SRCH_DRX_SUPPORTS_ONE_BAND
                    FEATURE_SRCH_IS_FTS_CAPABLE
02/16/11   vks     Removed non-1X feature checks.
                   Removed checks for mainlined features.
08/11/10   vks     Clean up 9K target revisit feature for srch-gps feature.
11/02/09   sst     Update NO_GPS checks
10/13/09   sst     For MDM9K, temporarily adjust GPS features
                   Match GPS checks to custsrch defs
04/01/09   adw     Categorized included header files and standardized format.
03/27/09   adw     Remove T_MSM7800 featurization, replace T_MSM8650B with
                   FEATURE_MODEM_1X_ARTEMIS feature.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
12/15/08   aps     Added deps for FEATURE_SRCH_QLIC_OLD_PC_TIMELINE
09/30/08   adw     FEATURE_SRCH_REDUCED_MEMORY must only be defined for ULC.
09/09/08   adw     Modified feature checks to support 8650 dependencies.
09/08/08   adw     Modified FEATURE_SRCHZZ_TLC_SLEEPCTL_DEBUG dependency check.
08/28/08   adw     Initial revision.
=============================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "customer.h"

/* 1X Common */
#include "modem_1x_defs.h"


/*===========================================================================

                        SEARCHER FEATURE REQUIREMENTS

===========================================================================*/

/* ------------------------------------------------------------------------
** General SRCH Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check Search Message Debug Mask */
/* MSG_BUILD_MASK_MSG_SSID_1X_SRCH     No feature related dependencies */

/* Check allow masking of searcher instead of slewing to absolute position */
/* FEATURE_RELATIVE_DMOD_POS           No feature related dependencies */

/* ------------------------------------------------------------------------
** Pilot Filter Gain and Trk_lo Control Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check Control the polarity of the TRK_LO_ADJUST pin */
/* FEATURE_INVERTED_TRK_LO_POLARITY    No feature related dependencies */

/* Check Configuration for XO Only Operation */
#ifndef FEATURE_XO
  #error FEATURE_XO not defined. 1x supports XO targets only
#endif /* FEATURE_XO */

/* Check control over pilot filter gain control register */
/* SRCH_HAS_PILOT_GAIN_CNTL            No feature related dependencies */

/* ------------------------------------------------------------------------
** Standby Enhancement Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check Page match enabling */
/* FEATURE_PAGE_MATCH_REQUEST          No feature related dependencies */

/* ------------------------------------------------------------------------
** Sleep Related Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check use dynamic QPCH thresholding */
/* FEATURE_QPCH_DYNAMIC_THRESHOLD      No feature related dependencies */

/* Check eliminate the dependency of the RX front on RTC */
/* FEATURE_SRCH_RF_HAS_NO_RTC_DEP      No feature related dependencies */

/* Check enable new Inactive state for MSM6500 5.0 and onwards */
/* FEATURE_SRCH_INACTIVE_STATE         No feature related dependencies */

/* Check monitor SRCH idle state interactions with sleepctl */
/* FEATURE_SRCHZZ_TLC_SLEEPCTL_DEBUG   No feature related dependencies */


/* ------------------------------------------------------------------------
** RF and AFC Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check multiplier settings for VCO Gain and Slew Rate Limits */
/* AFC_VCO_GAIN_SRL_NORMAL         No feature related dependencies */
/* AFC_VCO_GAIN_X5_SRL_X2          No feature related dependencies */
/* AFC_VCO_GAIN_X10_SRL_X3         No feature related dependencies */


/* ------------------------------------------------------------------------
** Single and Multi Antenna Related Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check 1X demod chain unlock */
/* FEATURE_SRCH_UNLOCK_ONLY_IMMEDIATE  No feature related dependencies */

/* ------------------------------------------------------------------------
** Wakeup, Warmup, and Overhead Constant Dependencies
** ------------------------------------------------------------------------ */

/* Check wakeup, warmup, and overhead constants */
/* RF_WARMUP_BASE_TIME_MS              No feature related dependencies */
/* RF_WARMUP_CX8_ON_TIME_MS            No feature related dependencies */
/* WARMUP_SW_LATENCY_SAFEWIN_MS        No feature related dependencies */
/* WARMUP_RTR6500_OVERHEAD_MS          No feature related dependencies */
/* MIN_SLEEP_SETUP_TIME_SAFEWIN_MS     No feature related dependencies */
/* CGPS_SW_OVERHEAD_MS                 No feature related dependencies */

/* Check times in micro seconds */
/* WARMUP_RTR6500_OVERHEAD_US          No feature related dependencies */
/* WARMUP_SW_LATENCY_SAFEWIN_US        No feature related dependencies */
/* RF_WARMUP_BASE_TIME_US              No feature related dependencies */
/* RF_WARMUP_CX8_ON_TIME_US            No feature related dependencies */
/* CGPS_SW_OVERHEAD_US                 No feature related dependencies */

#endif /* SRCH_DEP_H */
