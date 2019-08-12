#ifndef RFCOMMON_CORE_TYPES_H
#define RFCOMMON_CORE_TYPES_H
/*!
   @file
   rfcommon_core_types.h

   @brief

   @details

*/

/*===========================================================================
Copyright (c) 2008, 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfcommon_core_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/13   sar     Updates for APQ, GNSS only target.
04/12/11   sty     Deleted rf_has_five_gainranges from this file
04/07/11   sar     Relocated for CMI and removed conditional #def's. 
10/13/09   dyc     Provide target specific RF_DEF_HW definition
09/20/09   lp      Changed RF_HW_DEF to RF_HW_UNDEFINED
03/18/09   dyc     rename rf_state_type to rfcommon_state_type
03/13/09   sr      renamed the rf_state enums which were conflicting with tech enums
03/12/09   sar     Added changes needed for 1x
10/11/08   sr      Initial Revision.
===========================================================================*/

#include "rfcom.h"
#ifdef FEATURE_GNSS_ONLY_NO_WWAN
#include "sys.h"
#endif
typedef struct
{
  int32 slave_id;
  boolean use_ssbi;
  int32 bus_num;
} rfcommon_sbi_info_type;

typedef enum
{
  /* RF client that votes for VREG control */
  RF_PM_VOTE_APP_RF,
  /* GPS client that votes for VREG control*/
  RF_PM_VOTE_APP_GPS,

  RF_PM_VOTE_APP_MAX
} rf_pm_vote_client_type;

typedef enum {

  RFCOMMON_STATE_POWERUP_INIT,
    /* WCDMA Init State. This state is entered by rf_init() and is exited by
    rf_enable_wcdma_rx_pwr(). The RF NV items are read. The PDM controls are
    initialized. The LNA and PA control are initialized.
    */


  /* The following RF state descriptions apply when rf_chipx8_needed is FALSE.
     When rf_codec_on(TRUE) occurs, the phone exits sleep (if sleeping), CODEC
     power is turned on (if off), and chipx8 is enabled into the MSM2P (if
     disabled).  When rf_codec_on(FALSE) occurs, it is responsible for
     restoring the hardware according to the following state descriptions. */

  RFCOMMON_STATE_SLEEP_1,
    /* CDMA sleep state 1 (first catnap).  This state is entered by rf_sleep()
    and exited by either rf_stay_asleep() or rf_enable_digital_rx_pwr().
    The database item DB_RF_MODE == DB_RF_SLEEP, but the RF Rx hardware is
    still powered up.  chipx8 is available.  CODEC power is off.  The Rx
    hardware is set to CDMA mode. */

  RFCOMMON_STATE_SLEEP_2,
    /* CDMA sleep state 2 (after first catnap).  This state is entered by
    rf_stay_asleep() and exited by rf_enable_digital_rx_pwr().  The database
    item DB_RF_MODE == DB_RF_SLEEP and the RF Rx hardware has been powered
    down.  chipx8 is not available.  CODEC power is off.  The Rx hardware is
    set to CDMA mode. */


  RFCOMMON_STATE_DIVERSITY
  /* Diversity State. Indicates the second chain is in the diversity state. */

} rfcommon_state_type;


#define RF_DEF_HW RF_HW_UNDEFINED

#define RF_DEF_BC (SYS_BAND_MASK_BC0|SYS_BAND_MASK_BC1|SYS_BAND_MASK_WCDMA_I_IMT_2000)

#endif /* RFCOMMON_CORE_TYPES_H */

