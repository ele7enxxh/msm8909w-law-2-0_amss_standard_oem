#ifndef DS_DSD_TIMER_H
#define DS_DSD_TIMER_H
/*===========================================================================

                      D S _ D S D _ T I M E R . H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_timer.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/07/12    var   Added NV items for DSD timers
11/11/11    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds_dsd_ext_i.h"
#include "ds3gsubsmgr.h"
#include "timer.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  DSD timer IDs
    For Multi-Sim, timer ids will be calc as follows
      timer_id per subs_id = (subs_id * 100)+timer_id
---------------------------------------------------------------------------*/
#define DS_InvalidTimerID_T0            0
#define DS_1xAcqHystTimerID_T1          1
#define DS_1xOOSHystTimerID_T2          2
#define DS_HDRAcqHystTimerID_T3         3
#define DS_HDROOSHystTimerID_T4         4
#define DS_LTEAcqHystTimerID_T5         5
#define DS_LTEOOSHystTimerID_T6         6
#define DS_WLANAcqHystTimerID_T7        7
#define DS_EPCDataCntxTimerID_T8        8
#define DS_HDRHandDownTimerID_T9        9
#define DS_HDRHandUpAvoidTimerID_T10    10
#define DS_HDRNwDetachHystTimerID_T11   11
#define DS_LTENwDetachHystTimerID_T12   12
#define DS_MaxTimerID                   DS_LTENwDetachHystTimerID_T12

/*---------------------------------------------------------------------------
  Timer cmd info structure
---------------------------------------------------------------------------*/
typedef struct
{
  unsigned long               timer_id;
  ds3gsubsmgr_subs_id_e_type  subs_id;
}ds_dsd_timer_cmd_info_type;

/*---------------------------------------------------------------------------
  DSD Timer structure
---------------------------------------------------------------------------*/
typedef struct
{
  timer_type  timer;
  dword       value;
}ds_dsd_timer_info_type;

/*---------------------------------------------------------------------------
  DSD Timer list structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds_dsd_timer_info_type    cdma_acq_hyst_timer;          /* T01 */
  ds_dsd_timer_info_type    cdma_oos_hyst_timer;          /* T02 */
  ds_dsd_timer_info_type    hdr_acq_hyst_timer;           /* T03 */
  ds_dsd_timer_info_type    hdr_oos_hyst_timer;           /* T04 */
  ds_dsd_timer_info_type    lte_acq_hyst_timer;           /* T05 */
  ds_dsd_timer_info_type    lte_oos_hyst_timer;           /* T06 */
  ds_dsd_timer_info_type    epc_data_cntx_timer;          /* T07 */
  ds_dsd_timer_info_type    hdr_handdown_timer;           /* T08 */
  ds_dsd_timer_info_type    hdr_handup_avoid_timer;       /* T09 */
  ds_dsd_timer_info_type    wlan_acq_hyst_timer;          /* T10 */
  ds_dsd_timer_info_type    hdr_nw_detach_hyst_timer;     /* T11 */
  ds_dsd_timer_info_type    lte_nw_detach_hyst_timer;     /* T12 */
}ds_dsd_timer_list_type;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_TIMER_RUNNING

DESCRIPTION   Utility to find if timer is running.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     Timer running 
              FALSE:    Timer not running or Timer id not valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_timer_running
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_TIMER_START_TIMER

DESCRIPTION   Starts the DSD timer for specified duration.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     Timer started or Timer already running.
              FALSE:    Timer id not valid.

SIDE EFFECTS  Starts Timer.
===========================================================================*/
boolean ds_dsd_timer_start_timer
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_TIMER_STOP_TIMER

DESCRIPTION   Stop the dsd Timer.

DEPENDENCIES  None.

RETURN VALUE  TRUE: Timer stopped or Timer already stopped.
              FALSE: Timer ID invalid.

SIDE EFFECTS  Stops timer.
===========================================================================*/
boolean  ds_dsd_timer_stop_timer
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_RESET_TIMER

DESCRIPTION   Reset the DSD timer. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Set the timer to 0ms. Timer expires immediately.
===========================================================================*/
void ds_dsd_reset_timer
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_TIMER_RESET

DESCRIPTION   Stop all the DSD timers

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_reset
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_GET_OOS_TIMER_ID

DESCRIPTION   Utility function to get OOS timer id from dsd radio 

DEPENDENCIES  None.

RETURN VALUE  Timer id.

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds_dsd_get_oos_timer_id
(
  dsd_radio_type dsd_radio
);

/*===========================================================================
FUNCTION      DS_DSD_GET_DETACH_TIMER_ID

DESCRIPTION   Utility function to get detach timer id from dsd radio 

DEPENDENCIES  None.

RETURN VALUE  Timer id.

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds_dsd_get_detach_timer_id
(
  dsd_radio_type dsd_radio
);

/*===========================================================================
FUNCTION      DS_DSD_TIMER_CMD_PROCESSING

DESCRIPTION   This is timer callback command processing function. Depending on the
              the timer id, the appropriate state machine's handler is
              invoked for processing the timer callback.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_cmd_processing
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS_DSD_TIMER_INIT_TIMERS

DESCRIPTION   Initialize timers for specific subscription

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_init_timers
(
  ds3gsubsmgr_subs_id_e_type    subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_TIMER_INIT_TIMER_VAL_PER_SUBS_ID

DESCRIPTION   Initialize timer values for specific subscription by either 
              reading from NV item or setting it to default values.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_init_timer_val_per_subs_id
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_TIMER_EPC_NOTIFY

DESCRIPTION   Called by EPC module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_epc_notify
(
  boolean                     epc_present,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

#endif /* DS_DSD_TIMER_H */
