/*===========================================================================

                            Q M I _ C H A R G E R . C

DESCRIPTION

  The OEM's Charger interface source file for QMI.

EXTERNALIZED FUNCTIONS

  qmi_charger_init()
    Initializes the QMI charger module
  qmi_charger_is_ext_pwr_src_used()
    Determines whether external power source is used or not
  qmi_charger_is_battery_connected()
    Determines whether the battery is connected or not
  qmi_charger_is_charging()
    Determines whether the charger is charging or not
  qmi_charger_get_battery_level()
    gives the current battery level in terms of percentage
  qmi_charger_set_battery_level_limits()
    used to set the battery level upper and lower limits

Copyright (c) 2005-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/qmi_charger.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
06/07/06    ks     Fixed Power state and Battery level reporting
09/08/05    ks     Added QMI_CMD_SEND_POWER_STATE.
09/06/05    ks     Eliminated dependency on db.h. Using vbatt.h instead.
06/27/05    ks     Change to use rex timer instead of ps_timer.
06/27/05    jd     Cleanup
06/07/05    jd     Fixed assert, report all changed fields in state mask
04/05/05    ks     Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common definitions and customization headers */
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_QMI
#include "amssassert.h"
#ifdef FEATURE_BATTERY_CHARGER
#include "charger.h"
#include "vbatt.h"
#endif /* FEATURE_BATTERY_CHARGER */
#include "timer.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "ds_qmi_svc.h"
#include "qmi_charger.h"
#include "ds_qmi_dms_ext.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_system_heap.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
#define BAT_LVL_IND_PERIOD        120
#define QMI_CMD_SEND_POWER_STATE  0


/*===========================================================================

                                DATA TYPES

===========================================================================*/
#ifdef FEATURE_BATTERY_CHARGER
struct
{
  uint8 prev_power_state_mask;
  uint8 prev_bat_lvl;
  uint8 bat_lvl_lower_limit;
  uint8 bat_lvl_upper_limit;
} qmi_charger_info;
#endif /* FEATURE_BATTERY_CHARGER */

struct
{
  timer_type  battery_timer;
} qmi_charger_global;

/* This is the default QMI_Charger command buffer. Make sure that it does not grow
 * beyond 512 bytes. In case you need a command buffer larger than 512 bytes,
 * declare a separate structure. */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
} qmi_chargeri_cmd_buf_type;

/*===========================================================================

                   INTERNAL FUNCTION FORWARD DECLARATIONS

===========================================================================*/
#ifdef FEATURE_BATTERY_CHARGER
static void qmi_chargeri_get_power_state(uint8*);
#endif /* FEATURE_BATTERY_CHARGER */

static void qmi_chargeri_power_state_cback
(
  timer_cb_data_type  user_data
);
static void qmi_chargeri_send_power_state_info(void);

static void *qmi_chargeri_get_cmd_data_buf(void);

#define qmi_chargeri_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)


/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
                                                          
/*===========================================================================
  FUNCTION QMI_CHARGER_INIT()

  DESCRIPTION
    Initializes the QMI charger module
    
  PARAMETERS
    none

  RETURN VALUE
    None
      
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_charger_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    initialize internal global data
  -------------------------------------------------------------------------*/
  timer_def( &qmi_charger_global.battery_timer,
              NULL,
              rex_self(),
              0,
              qmi_chargeri_power_state_cback,
              0 );

#ifdef FEATURE_BATTERY_CHARGER
  /*-------------------------------------------------------------------------
    initialize QMI-Charger interface settings
  -------------------------------------------------------------------------*/
  qmi_charger_info.prev_power_state_mask = 0;
  qmi_charger_info.prev_bat_lvl = QMI_BAT_LVL_INACTIVE;
  qmi_charger_info.bat_lvl_lower_limit = QMI_BAT_LVL_INACTIVE;
  qmi_charger_info.bat_lvl_upper_limit = QMI_BAT_LVL_INACTIVE;
#endif /* FEATURE_BATTERY_CHARGER */
} /* qmi_charger_init() */


                                                          
/*===========================================================================
  FUNCTION QMI_CHARGER_IS_EXT_PWR_SRC_USED()

  DESCRIPTION
    Determines whether external power source is used or not
    
  PARAMETERS
    none

  RETURN VALUE
    TRUE  - External power source is being used to power the phone
    FALSE - External power source is not used

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_charger_is_ext_pwr_src_used
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_BATTERY_CHARGER
  /*-------------------------------------------------------------------------
    Return FALSE if the charger is not connected
  -------------------------------------------------------------------------*/
  if(chg_is_charger_valid() == FALSE) 
  {
    return FALSE;
  }             
#endif /* FEATURE_BATTERY_CHARGER */

  /*-------------------------------------------------------------------------
    For SURFs always return TRUE as external power source is always used
  -------------------------------------------------------------------------*/
  return TRUE;
} /* qmi_charger_is_ext_pwr_src_used() */



/*===========================================================================
  FUNCTION QMI_CHARGER_IS_BATTERY_CONNECTED()

  DESCRIPTION
    Determines whether the battery is connected or not
    
  PARAMETERS
    none

  RETURN VALUE
    TRUE  - Battery is connected
    FALSE - Battery is not connected

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_charger_is_battery_connected
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_BATTERY_CHARGER
  if(chg_is_battery_valid() == TRUE)
  {
    return TRUE;
  }
#endif /* FEATURE_BATTERY_CHARGER */

  return FALSE;
} /* qmi_charger_is_battery_connected() */



/*===========================================================================
  FUNCTION QMI_CHARGER_IS_CHARGING()

  DESCRIPTION
    Determines whether the charger is charging or not
        
  PARAMETERS
    none

  RETURN VALUE
    TRUE  - Charger is charging
    FALSE - Charger is not charging or no charger connected

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_charger_is_charging
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_BATTERY_CHARGER
  return chg_is_charging();
#else
  return FALSE;
#endif /* FEATURE_BATTERY_CHARGER */
} /* qmi_charger_is_charging() */



/*===========================================================================
  FUNCTION QMI_CHARGER_GET_BATTERY_LEVEL()

  DESCRIPTION
    It gives the current battery level in terms of percentage
    
  PARAMETERS
    none

  RETURN VALUE
    uint8 - current battery level
            0 means NO battery connected or battery is exhausted
    
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_charger_get_battery_level
(
  void
)
{
#ifdef FEATURE_BATTERY_CHARGER
  uint16 batt_lvl;
#endif /* FEATURE_BATTERY_CHARGER */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_BATTERY_CHARGER

  #ifdef FEATURE_PM_CHARGING
    batt_lvl = chg_battery_volt(); 
  #else
    batt_lvl = vbatt_read();
  #endif

  return (uint8) (batt_lvl * 100/255);
#else
  return 0;
#endif /* FEATURE_BATTERY_CHARGER */
} /* qmi_charger_get_battery_level() */ 



/*===========================================================================
  FUNCTION QMI_CHARGER_SET_BATTERY_LEVEL_LIMITS()

  DESCRIPTION
    It is used to set the battery level upper and lower limits
    
  PARAMETERS
    battery_lvl_upper_limit - value of battery charge as a percentage, which
                              will trigger a power state indication when
                              risen to or exceeded.
    battery_lvl_lower_limit - value of battery charge as a percentage, which
                              will trigger a power state indication when
                              fallen to or exceeded.

  RETURN VALUE
    None
    
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_charger_set_battery_level_limits
(
  uint8 battery_lvl_lower_limit,
  uint8 battery_lvl_upper_limit
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_BATTERY_CHARGER
  qmi_charger_info.bat_lvl_lower_limit = battery_lvl_lower_limit;
  qmi_charger_info.bat_lvl_upper_limit = battery_lvl_upper_limit;
#endif /* FEATURE_BATTERY_CHARGER */
} /* qmi_charger_set_battery_level_limits() */



/*===========================================================================
  FUNCTION QMI_CHARGER_RESET_PREV_POWER_STATE_MASK()

  DESCRIPTION
    It is used to reset the previous power state reported state variable in
    charger_info.
    
  PARAMETERS
    None

  RETURN VALUE
    None
    
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_charger_reset_prev_power_state_mask
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_BATTERY_CHARGER
  qmi_charger_info.prev_power_state_mask = 0;
#endif /* FEATURE_BATTERY_CHARGER */
} /* qmi_charger_reset_prev_power_state_mask() */



/*===========================================================================
  FUNCTION QMI_CHARGER_SET_POWER_STATE_REPORTING()

  DESCRIPTION
    It is used to turn on power state reporting, to report any change in 
    power state. Current implementation starts a timer to periodically 
    monitor power state and report if changed.
    
  PARAMETERS
    report : TRUE  - Enable power state reporting
             FALSE - Disable power state reporting

  RETURN VALUE
    None
    
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_charger_set_power_state_reporting
(
  boolean report
)
{
     LOG_MSG_INFO2_0 ("In qmi_charger_set_power_state_reporting(TRUE) before");
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_BATTERY_CHARGER
  
  if(report == TRUE)
  {
    /*-----------------------------------------------------------------------
      First client registered, start the timer.
      Note - charger state variables are already intialized in init
    -----------------------------------------------------------------------*/
    timer_set( &qmi_charger_global.battery_timer,
               BAT_LVL_IND_PERIOD,
               BAT_LVL_IND_PERIOD,
               T_SEC );
     LOG_MSG_INFO2_0 ("In qmi_charger_set_power_state_reporting(TRUE)");
  }
  else
  {
    /*-----------------------------------------------------------------------
      Last client de-registered, no more clients need reporting
      clear the timer and reset the charger state variables
    -----------------------------------------------------------------------*/
    (void) timer_clr( &qmi_charger_global.battery_timer, T_NONE );
    qmi_charger_info.prev_power_state_mask = 0;
    qmi_charger_info.prev_bat_lvl = QMI_BAT_LVL_INACTIVE;
    qmi_charger_info.bat_lvl_lower_limit = QMI_BAT_LVL_INACTIVE;
    qmi_charger_info.bat_lvl_upper_limit = QMI_BAT_LVL_INACTIVE;
  }
#endif /* FEATURE_BATTERY_CHARGER */

} /* qmi_charger_set_power_state_reporting() */



/*===========================================================================
  FUNCTION QMI_CHARGER_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI Charger cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          : QMI command ID to be processed
    user_data_ptr: private data buffer containing the QMI command 
                   information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_charger_process_cmd
(
  dcc_cmd_enum_type        cmd,
  void                   * user_data_ptr
)
{
  qmi_chargeri_cmd_buf_type * cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  cmd_ptr = (qmi_chargeri_cmd_buf_type * )user_data_ptr;

  ASSERT (cmd_ptr);
  ASSERT (cmd == DCC_QMI_CHARGER_CMD);
  
  switch (cmd_ptr->cmd_id)
  {
    case QMI_CMD_SEND_POWER_STATE:
      qmi_chargeri_send_power_state_info();
      break;
    default:
      LOG_MSG_INFO2_1 ("Ignoring QMI cmd %d", cmd_ptr->cmd_id);
      break;
  }

  LOG_MSG_INFO1_0("qmi_charger_process_cmd(): "
                  "QMI Charger cmd processed");

  qmi_chargeri_free_cmd_buf(cmd_ptr);

} /* qmi_charger_process_cmd() */
/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CHARGERI_POWER_STATE_CBACK()

  DESCRIPTION
    This is the callback function which is called when the battery timer 
    expires. It checks the current power state and battery level and
    if the conditions are met then generates an charger event for DMS.    
  PARAMETERS
    none

  RETURN VALUE
    None
        
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_chargeri_power_state_cback
(
  timer_cb_data_type  user_data
)
{
  qmi_chargeri_cmd_buf_type *  qmi_charger_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qmi_charger_cmd = (qmi_chargeri_cmd_buf_type *)qmi_chargeri_get_cmd_data_buf();
  ASSERT(qmi_charger_cmd);

  qmi_charger_cmd->cmd_id = QMI_CMD_SEND_POWER_STATE;    

  LOG_MSG_INFO1_0("qmi_chargeri_power_state_cback(): "
                  "QMI Charger cmd send_cmd_ex");

  dcc_send_cmd_ex (DCC_QMI_CHARGER_CMD, qmi_charger_cmd);

} /* qmi_chargeri_power_state_cback() */


/*===========================================================================
  FUNCTION qmi_chargeri_get_cmd_data_buf

  DESCRIPTION
    Allocate and assign a QMI Charger command buffer from the PS MEM heap
    based on the QMI CMD type
    
  PARAMETERS
    NONE
   
  RETURN VALUE
    cmd_buf_ptr - Pointer to the allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *qmi_chargeri_get_cmd_data_buf
(
  void
)
{

  qmi_chargeri_cmd_buf_type *cmd_buf_ptr = NULL;
  PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(qmi_chargeri_cmd_buf_type),
                           qmi_chargeri_cmd_buf_type*);

  return ((void*)cmd_buf_ptr);
   
} /* qmi_chargeri_get_cmd_data_buf */

/*===========================================================================
  FUNCTION QMI_CHARGERI_SEND_POWER_STATE_INFO()

  DESCRIPTION
    This is the callback function which is called when the battery timer
    expires. It checks the current power state and battery level and
    if the conditions are met then generates an charger event for DMS.

  PARAMETERS
    none

  RETURN VALUE
    None
        
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_chargeri_send_power_state_info
(
  void
)
{
#ifdef FEATURE_BATTERY_CHARGER
  uint16  batt;
  uint8   curr_bat_lvl;
  uint8   power_state_mask;
  uint8   charger_event_mask;         /* mask of events that have occurred */

  /* battery level drops from above lower limit to the lower limit or less */
  boolean level_goes_south;

  /* battery level rises from below upper limit to the upper limit or more */
  boolean level_goes_north;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  #ifdef FEATURE_PM_CHARGING
    batt = chg_battery_volt();
  #else
    batt = vbatt_read();
  #endif

  charger_event_mask = 0;

  /*-------------------------------------------------------------------------
    get the current power state
  -------------------------------------------------------------------------*/
  qmi_chargeri_get_power_state(&power_state_mask);

  /*-------------------------------------------------------------------------
    get the current battery level
  -------------------------------------------------------------------------*/
  curr_bat_lvl = (uint8) (batt * 100/255);

  /*-------------------------------------------------------------------------
    if any change in power state, immediately report the event to QMI_WDS
    along with current power state and battery level
  -------------------------------------------------------------------------*/
  if(power_state_mask != qmi_charger_info.prev_power_state_mask)
  {
    charger_event_mask |= QMI_CHARGER_EVENT_MASK_POWER_STATE;
    qmi_charger_info.prev_power_state_mask = power_state_mask;
  }

  /*-------------------------------------------------------------------------
    if no change in power state, check if the current battery level meets 
    the set lower and upper limits if the battery level falls from > lower 
    limit to <= lower limit OR the battery level rises from < upper limit 
    to >= upper limit, report the battery level event to QMI_DMS
    along with current power state and battery level
  -------------------------------------------------------------------------*/
  level_goes_south = FALSE;
  level_goes_north = FALSE;
  if (qmi_charger_info.bat_lvl_lower_limit != QMI_BAT_LVL_INACTIVE)
  {
    level_goes_south = 
      qmi_charger_info.prev_bat_lvl == QMI_BAT_LVL_INACTIVE ||
      ( (curr_bat_lvl <= qmi_charger_info.bat_lvl_lower_limit) &&
        (qmi_charger_info.prev_bat_lvl > 
         qmi_charger_info.bat_lvl_lower_limit) );
  }

  if (qmi_charger_info.bat_lvl_upper_limit != QMI_BAT_LVL_INACTIVE)
  {
    level_goes_north =
      qmi_charger_info.prev_bat_lvl == QMI_BAT_LVL_INACTIVE ||
      ( (curr_bat_lvl >= qmi_charger_info.bat_lvl_upper_limit) &&
        (qmi_charger_info.prev_bat_lvl <
         qmi_charger_info.bat_lvl_upper_limit) );
  }

  if( level_goes_south || level_goes_north )
  {
    charger_event_mask |= QMI_CHARGER_EVENT_MASK_BAT_LVL;
  }

  /*-------------------------------------------------------------------------
    If something of note has changed, let the DMS service know
  -------------------------------------------------------------------------*/
  if( charger_event_mask )
  {
    qmi_dms_charger_event_occured( charger_event_mask,
                                   power_state_mask,
                                   curr_bat_lvl );
  }

  /*-------------------------------------------------------------------------
    Save the last queried power level and start a timer for the next 
    time to query power state.
  -------------------------------------------------------------------------*/
  qmi_charger_info.prev_bat_lvl = curr_bat_lvl;

#endif /* FEATURE_BATTERY_CHARGER */

} /* qmi_chargeri_send_power_state_info */


/*===========================================================================
  FUNCTION QMI_CHARGERI_GET_POWER_STATE()

  DESCRIPTION
    It is used to get the power state bitmask to be returned to DMS.
    (lsb) bit 0 - external power source connected or not
          bit 1 - battery connected or not
          bit 2 - charging or not
    
  PARAMETERS
    power_state -  current power state will be written to this address

  RETURN VALUE
    None
    
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
#ifdef FEATURE_BATTERY_CHARGER
static void qmi_chargeri_get_power_state
(
  uint8 * power_state
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(power_state);
  *power_state = 0;

  if(TRUE == qmi_charger_is_ext_pwr_src_used())
  {
    *power_state |= QMI_PWR_ST_EXT_POWER_SRC_MASK;
  }

  if(TRUE == qmi_charger_is_battery_connected())
  {
    *power_state |= QMI_PWR_ST_BAT_CONNECTED_MASK;
  }

  if(TRUE == qmi_charger_is_charging())
  {
    *power_state |= QMI_PWR_ST_CHARGING_MASK;
  }

} /* qmi_chargeri_get_power_state() */

#endif /* FEATURE_DATA_QMI */
#endif /* FEATURE_BATTERY_CHARGER */
