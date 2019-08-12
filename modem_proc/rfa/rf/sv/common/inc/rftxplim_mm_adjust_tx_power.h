#ifndef RFTXPLIM_MM_ADJUST_TX_POWER_H
#define RFTXPLIM_MM_ADJUST_TX_POWER_H
/*!
   @file rftxplim_mm_adjust_tx_power.h

   @brief
   This file contains structure and function prototype for acquring TX power
   status from remote RTR

   @details

*/

/*===========================================================================
Copyright (c) 2010,2011  by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/inc/rftxplim_mm_adjust_tx_power.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/12   ndb     Implementation of IM2 backoff for LTE B4 and 1xBC0
02/21/12   sar     Removed unused/obsolete code.
11/14/11   Saul    SV Limiting. Added support for 1x-HDR Half-Duplex and 
                   DSI backoffs.
11/11/11   Saul    SV Limiting. LTE backoff support.
11/10/11   Saul    SV Limiting. L1 interface update.
06/16/11   sar     Included internal rf hdr interface.
05/04/11   pl     SV-LTE SAR non-RPC implementation
04/20/11   kma     Added DSI SAR implementation
04/05/11   kma     Compilation fix
03/31/11    ra     Corrected bug in integrating SAR limit with HDET loop
12/13/10   kma     Added API to set the flag of current SAR back off status
12/02/10   kma     Added global variable to track remote TX power
11/16/10   kma     Added bypass parameter in RPC API
10/13/10   kma     Fixed compilation issue
10/11/10   kma     Cleaned up the code
09/13/10   kma     Fixed the boot up race condition on Fusion
07/14/10   kma     Updated interface code with LTE RF driver
07/13/10   kma     Fixed TX power resolution bug
06/30/10   kma     Added API to convert HDR tx power from agc to dbm
06/30/10   kma     Added macro for device max power
06/08/10   kma     Initial version
============================================================================*/

#include "rfcom.h"
#include "rfm_hdr.h"
#include "rfnv_sv.h"

#define HDR_P_MAX 420 //HDR device max power
#define LTE_P_MAX 300 //LTE device max power

#define MAX_NUM_CHANNELS_SUPPORTED 3

typedef struct{
  int16 sar_limit;
  int16 half_duplex_desense_limit;
}hdr_power_limit;  //move this to tech specific files

typedef struct{
  int16 sar_limit;
  int16 im3_limit;
  int16 im2_limit;
}lte_power_limit;  //move this to tech specific files

typedef struct {
  boolean                   tx_on;
  rfm_mode_enum_type        rf_mode;
  rf_card_band_type         rf_bands;
  uint32                    num_band_chan; 
  uint32                    band_chan_array[MAX_NUM_CHANNELS_SUPPORTED]; 
} rf_local_tx_status;

extern hdr_power_limit old_hdr_power_limit;
extern lte_power_limit old_lte_power_limit;

extern boolean hdr_normal_power_limit_active;

/*===========================================================================

FUNCTION RFSV_GET_HDR_SAR_PWR_LIMITS_STATUS                 INTERNAL FUNCTION

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
boolean rfsv_get_hdr_sar_pwr_limits_status( void );

boolean rfsv_get_lte_sar_pwr_limits_status( void );

#endif /* RFTXPLIM_MM_ADJUST_TX_POWER_H */
