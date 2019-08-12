/*!
   @file rftxplim_mm_adjust_tx_power.c

   @brief
   This file contains structure and function prototype for acquring TX power
   status from remote RTR

   @details

*/

/*===========================================================================
Copyright (c) 2010-2011     by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/src/rftxplim_mm_adjust_tx_power.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/12   ndb     Implementation of IM2 backoff for LTE B4 and 1xBC0
02/21/12   sar     Removed unused/obsolete code.
11/14/11   Saul    SV Limiting. Added support for 1x-HDR Half-Duplex and 
                   DSI backoffs.
11/10/11   Saul    SV Limiting. L1 interface update.
05/13/11   aca    CW Fix: Compiler Warnings Report (05/12/11)
05/04/11   pl     SV-LTE SAR non-RPC implementation
05/04/11   pl     remove use of rfm_mode
04/20/11   kma     Added DSI SAR implementation
03/31/11    ra     Corrected bug in integrating SAR limit with HDET loop
12/13/10   kma     Added API to set the flag of current SAR back off status
12/02/10   kma     Optimized F3 message for SAR
11/08/10   kma     Updated F3 message to reflect SAR back off reason
10/11/10   kma     Cleaned up the code
09/23/10   kma     Fixed bug in F3 message
09/13/10   kma     Fixed the boot up race condition on Fusion
07/22/10   kma     Fixed agc to dbm conversion resolution bug
07/14/10   kma     Updated interface code with LTE RF driver
07/13/10   kma     Fixed TX power resolution bug
07/07/10   kma     Fixed a logic bug in the searching algorithm
06/30/10   kma     Added API to convert HDR tx power from agc to dbm
06/30/10   kma     Used macro to track device max power
06/08/10   kma     Initial version
============================================================================*/

#include "rftxplim_mm_adjust_tx_power.h"
#include "rfm_common_sar.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#endif

/* min TX AGC value */
#define RFLTE_CORE_TXPL_TX_AGC_MIN               0

/* max Tx AGC value (with offset) */
#define RFLTE_CORE_TXPL_TX_AGC_MAX               1023

#define RFLTE_CORE_TXPL_MIN_TX_POWER_DBM        -70

#define RFLTE_CORE_TXPL_MAX_TX_POWER_DBM        32.3

/*Global variable storing the last HDR power limit computed from each algorithm*/
hdr_power_limit old_hdr_power_limit =
{
  HDR_P_MAX, //default SAR limit to 23dbm
  HDR_P_MAX, //default Half Duplex Desense limit to 23dbm
}; //move this to tech specific files

/*Global variable storing the last LTE power limit computed from each algorithm*/
lte_power_limit old_lte_power_limit =
{
  LTE_P_MAX, //default SAR limit to 23dbm
  LTE_P_MAX, //default IM3 limit to 23dbm
  LTE_P_MAX, //default IM2 limit to 23dbm
}; //move this to tech specific files

/*Local variable holding the current desired max power limit*/
LOCAL int16 current_sar_desired_limit = 0;

#ifdef FEATURE_CDMA1X
extern rf_cdma_data_status_type rf_chain_status[];
#endif

/*===========================================================================

FUNCTION RFSV_GET_SAR_PWR_LIMITS_STATUS                      INTERNAL FUNCTION

DESCRIPTION
  This function returns TRUE if regular pwr limit is
  used and FALSE if SAR/IM3 pwr limit is applied. This 
  API is designed specifically for targets with remote 
  RF driver.


DEPENDENCIES
  None

RETURN VALUE
  Status of power limit

SIDE EFFECTS
  None

===========================================================================*/
boolean rfsv_get_hdr_sar_pwr_limits_status(void)
{
  return hdr_normal_power_limit_active;
}

