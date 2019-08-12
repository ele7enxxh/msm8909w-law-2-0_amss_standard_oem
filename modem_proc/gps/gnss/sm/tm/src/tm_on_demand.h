#ifndef TM_ON_DEMAND_H
#define TM_ON_DEMAND_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

    TM_PRTL_IFACE Header File

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Position Determination module.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_on_demand.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/09   gk     Support ODP 2.0
07/23/09   gk     Support ODP 2.0
06/12/09   gk      ODP 1.1 support
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
08/02/07    LT     Added variables for SVs non-existence message.
07/30/07    ank    Changed type for accuracy_threshold from uint8 to uint32.
07/17/07    lt     L1 interface.
06/12/07    cl     Only allow PD API access when TM task is active
05/27/07    rw     Add support for JGPS (KDDI) module
02/15/07    cl     Added support to inject pre-fix ppm
01/31/07    cl     Modified event callback function for PD API events
11/29/06    cl     Change event_payload to pointer in prtl_event_cb_f_type
06/13/06    cl     Initial Release
===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "aries_os_globals.h"
#include "aries_os_api.h"
#include "sm_api.h"
#include "tm_lm_iface.h"
#include "tm_common.h"
#include "pdapi.h"
#include "tm_api.h"
#include "tm_qwip_iface.h"
#include "tm_standalone.h"

#define TM_ON_DEMAND_READY_FIX_NUMBER           0xFFFFFFF 
#define TM_ON_DEMAND_READY_1HZ_FIX_TBF_MS           1000
#define TM_ON_DEMAND_READY_1HZ_ACCURACY_THRESHOLD   1000
#define TM_ON_DEMAND_READY_1HZ_SESS_TIMEOUT         1 

#define TM_ON_DEMAND_LPM_FIX_NUMBER                 1 

#define TM_ON_DEMAND_LPM_MODE1_FIX_TBF_MS           600000
#define TM_ON_DEMAND_LPM_MODE1_ACCURACY_THRESHOLD   1000
#ifdef FEATURE_CGPS_ODP_2_0
#define TM_ON_DEMAND_LPM_MODE1_SESS_TIMEOUT         2 
#else
#define TM_ON_DEMAND_LPM_MODE1_SESS_TIMEOUT         3 
#endif
#define TM_ON_DEMAND_LPM_MODE2_FIX_TBF_MS           1200000
#define TM_ON_DEMAND_LPM_MODE2_ACCURACY_THRESHOLD   1000
#define TM_ON_DEMAND_LPM_MODE2_SESS_TIMEOUT         15 


#define TM_ON_DEMAND_LPM_MODE3_FIX_TBF_MS           7200000
#define TM_ON_DEMAND_LPM_MODE3_ACCURACY_THRESHOLD   1000
#define TM_ON_DEMAND_LPM_MODE3_SESS_TIMEOUT         30 

#define TM_ON_DEMAND_LPM_WARMUP_SESS_TIMEOUT         75
#define TM_ON_DEMAND_LPM_WARMUP_ACCURACY_THRESHOLD   1000
#define TM_ON_DEMAND_LPM_WARMUP_FIX_TBF_MS           100000


/**
TM_ON_DEMAND_CHARGER_POLL_FIRST_TIMER_COUNT  is the duration the on-demand feature
waits when first initialized to ensure that the rest of the phone has booted up completely
*/
#define TM_ON_DEMAND_CHARGER_POLL_FIRST_TIMER_COUNT  ((uint32)30 * SEC_MSECS) /* 30 seconds */
/**
 * TM_ON_DEMAND_CHARGER_POLL_TIMER_COUNT is the period with which the on-demand feature polls
 the charger to determine if it is connected or if the battery is completely charged
 */
#define TM_ON_DEMAND_CHARGER_POLL_TIMER_COUNT  ((uint32)60 * SEC_MSECS) /* 60 seconds */
#define TM_ON_DEMAND_READY_MODE_EXTENDED_ON_TIMER_COUNT ((uint32)60 * SEC_MSECS) /* 30 seconds */
#define TM_ON_DEMAND_LPM_MODE_1_TIMER_COUNT ((uint32)600 * SEC_MSECS) /* 300 seconds */
#define TM_ON_DEMAND_LPM_MODE_2_TIMER_COUNT ((uint32)3600 * SEC_MSECS) /* 1800 seconds */
#define TM_ON_DEMAND_LPM_MODE_3_TIMER_COUNT ((uint32)7200 * SEC_MSECS) /* 7200 seconds */


#define TM_ON_DEMAND_LOG_PUNC_INVALID ((float)(0xbaadbeef))
#define TM_ON_DEMAND_LOG_TUNC_INVALID ((float)(0xbaadbabe))

typedef enum
{
   TM_ON_DEMAND_CHARGER_STATUS_UNKNOWN = 0, 
   TM_ON_DEMAND_CHARGER_STATUS_NOT_CONNECTED,
   TM_ON_DEMAND_CHARGER_STATUS_CONNECTED,

   /* add other states here */
   TM_ON_DEMAND_CHARGER_STATUS_LAST
} tm_on_demand_charger_status;

typedef enum
{
   TM_ON_DEMAND_BATTERY_STATE_UNKNOWN = 0,
   TM_ON_DEMAND_BATTERY_STATE_NOT_COMPLETELY_CHARGED,
   TM_ON_DEMAND_BATTERY_STATE_COMPLETELY_CHARGED,

   /* add other states here */
   TM_ON_DEMAND_BATTERY_STATE_LAST
} tm_on_demand_battery_status;


typedef struct
{
   tm_on_demand_charger_status   charger_status;
   tm_on_demand_battery_status   battery_status;
} tm_on_demand_power_state;

typedef enum {
  TM_ON_DEMAND_EVENT_ERROR_NONE = 0,
   TM_ON_DEMAND_EVENT_TIMER_EXPIRY, 
   TM_ON_DEMAND_EVENT_CHARGER_DISCONNECTED,
   TM_ON_DEMAND_EVENT_CHARGER_CONNECTED,
   TM_ON_DEMAND_EVENT_BATTERY_COMPLETELY_CHARGED,
   TM_ON_DEMAND_EVENT_PARAMETER_CHANGED,
   TM_ON_DEMAND_EVENT_NO_FIX_COUNT_REACHED,
   TM_ON_DEMAND_EVENT_FIX_OBTAINED,
   TM_ON_DEMAND_EVENT_ERROR_XTRA_DATA_NOT_VALID,
   TM_ON_DEMAND_EVENT_ERROR_PHONE_OUT_OF_SERVICE,
   TM_ON_DEMAND_EVENT_ERROR_GPS_SESSION_ACTIVE,

   TM_ON_DEMAND_EVENT_OVERRIDE,
   TM_ON_DEMAND_EVENT_DISABLED,
   TM_ON_DEMAND_EVENT_ENABLED,
   TM_ON_DEMAND_EVENT_PUNC_TUNC_INVALID,
   TM_ON_DEMAND_EVENT_PUNC_TUNC_WITHIN_RANGE,
  
   /* add other reasons that might affect ready mode above this line */
   TM_ON_DEMAND_EVENT_UNKNOWN,
   TM_ON_DEMAND_EVENT_LAST
}tm_on_demand_event_type;

typedef enum
{
   TM_ON_DEMAND_MODE_1 = 1,
   TM_ON_DEMAND_MODE_2,
   TM_ON_DEMAND_MODE_3,
   TM_ON_DEMAND_MODE_WARM_UP,
   TM_ON_DEMAND_MODE_READY,
   TM_ON_DEMAND_MODE_BATTERY_CHARGING,
   TM_ON_DEMAND_MODE_BATTERY_CHARGED
} tm_on_demand_mode_type;

#define TM_ON_DEMAND_MODE_1_NO_FIX_COUNT 4 
#define TM_ON_DEMAND_MODE_2_NO_FIX_COUNT 2 


/*===========================================================================

FUNCTION tm_on_demand_get_lpm_mode

DESCRIPTION   get the next mode that on-demand should go into
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_get_lpm_mode(void);
  

/*===========================================================================

FUNCTION tm_on_demand_ready_enable_handler

DESCRIPTION handle on demand ready mode enable or disable
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_ready_enable_handler(boolean on_demand_ready_enabled, 
                                      tm_on_demand_event_type reason);

/*===========================================================================

FUNCTION tm_on_demand_lpm_enable_handler

DESCRIPTION handle on demand low power mode enable or disable
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_lpm_enable_handler(boolean on_demand_enabled);


/*===========================================================================
FUNCTION tm_on_demand_start_sess_req

DESCRIPTION  Start On-Demand LPM positioning. this will get the mode and then
start the positoning in the LPM mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_on_demand_start_sess_req(sm_OnDemandPosPuncTuncStruct *pos_data_ptr);


/*===========================================================================

FUNCTION tm_trigger_on_demand_lpm

DESCRIPTION Start On-Demand LPM positioning. this will get the mode and then
start the positoning in the LPM mode
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_trigger_on_demand_lpm (sm_OnDemandPosPuncTuncStruct *pos_data_ptr);



/*===========================================================================

FUNCTION tm_on_demand_timer_stop

DESCRIPTION   Stop the on-demand timer. this is usually done with MGP is turned
            ON
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_on_demand_timer_stop(void);

/*===========================================================================

FUNCTION tm_on_demand_timer_start

DESCRIPTION   Start the on-demand timer. this is usually done with MGP is turned
            OFF
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_timer_start(void);


/*===========================================================================
FUNCTION tm_handle_forever_on_demand_lpm_timer

DESCRIPTION this fucntion handles the timer expiry of the LPM timer. Upon timer
expiry it starts the next session

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_lpm_timer(void);


/*===========================================================================
FUNCTION tm_on_demand_wait_for_next_timeout

DESCRIPTION this fucntion waits for next timeout to continue on-demand processing

DEPENDENCIES
  None.

RETURN VALUE
  None.

INPUT
   fix_fail is used to indicate if the num of failed fixes need to be incremented

SIDE EFFECTS
  None.
===========================================================================*/
void tm_on_demand_wait_for_next_timeout(boolean fix_fail);




/*===========================================================================

FUNCTION tm_on_demand_ready_timer_stop

DESCRIPTION   Stop the on-demand ready timer. this is usually done with MGP is turned
            ON
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/


void tm_on_demand_ready_timer_stop(void);

/*===========================================================================

FUNCTION tm_on_demand_ready_timer_start

DESCRIPTION   Start the on-demand ready timer. this is usually done with MGP is turned
            OFF
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_ready_timer_start(void);
/*===========================================================================
FUNCTION tm_handle_on_demand_ready_timer

DESCRIPTION this fucntion handles the timer expiry of the ready timer. Upon timer
expiry it starts the next session

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_ready_timer(void);

/*
 ******************************************************************************
 *
 * tm_on_demand_charger_poll
 *
 * Function description:
 *  This function polls battery charger and sends a message to SM_TM if state
 *  changes
 *
 * Parameters: 
 *  
 * Return value:
 *  
 *
 ******************************************************************************
*/

void tm_on_demand_charger_poll(void);

/*===========================================================================
FUNCTION tm_on_demand_charger_poll_timer_handler

DESCRIPTION handles time-outs related on-demand charger-poll module

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_on_demand_charger_poll_timer_handler(void* timer_data);

/*===========================================================================
FUNCTION tm_on_demand_charger_poll_init

DESCRIPTION initializes on-demand charger-poll module. Sets up a timer to wait
            for the rest of the phone to initialize. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_on_demand_charger_poll_init(void);


/*===========================================================================
FUNCTION tm_handle_on_demand_charger_connected


DESCRIPTION this fucntion handles the battery charging event. In this case
 on demand positioning is started in LPM mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_charger_connected(void);

/*===========================================================================
FUNCTION tm_handle_on_demand_battery_charged


DESCRIPTION this fucntion handles the battery charged event. In this case
 on demand positioning is started in ready mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_battery_charged(void);


/*===========================================================================
FUNCTION tm_handle_on_demand_charger_disconnected


DESCRIPTION this fucntion handles the battery notcharging event. In this case
 on demand positioning is stopped.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_handle_on_demand_charger_disconnected(void);

 /******************************************************************************
 *
 * tm_on_demand_abort_handler
 *
 * Function description:
 *  This function polls battery charger and sends a message to SM_TM if state
 *  changes
 *
 * Parameters: 
 *  
 * Return value:
 *  
 *
 ******************************************************************************
*/

void tm_on_demand_abort_handler(tm_sess_stop_reason_e_type reason);


/*
 ******************************************************************************
 *
 * tm_on_demand_log_done_event
 *
 * Function description:
 *  Perform cleanups when on_demand is stopped
 *  
 *
 * Parameters: 
 *  
 * Return value:
 *  
 *
 ******************************************************************************
*/
void tm_on_demand_log_done_event(tm_sess_stop_reason_e_type reason);


/*===========================================================================

FUNCTION tm_on_demand_start_on_demand_processing

DESCRIPTION start on demand positioning 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_start_on_demand_processing(void);

#ifdef __cplusplus
}
#endif

#endif /* TM_ON_DEMAND_H */

