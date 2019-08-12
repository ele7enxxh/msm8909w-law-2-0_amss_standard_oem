#ifndef _QMI_CHARGER_H
#define _QMI_CHARGER_H
/*===========================================================================

                            Q M I _ C H A R G E R . H        

DESCRIPTION

  The OEM's Charger interface header file for QMI.

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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/qmi_charger.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
06/07/06    ks     Fixed Power state and Battery level reporting
06/27/05    ks     Change to use rex timer instead of ps_timer.
06/27/05    jd     Cleanup
06/07/05    jd     Removed qmi_charger_power_state_cback and added
                   qmi_bat_lvl_inactive
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
#include "dcc_task_defs.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

#define QMI_PWR_ST_EXT_POWER_SRC_MASK  (0x01)
#define QMI_PWR_ST_BAT_CONNECTED_MASK  (0x02)
#define QMI_PWR_ST_CHARGING_MASK       (0x04)

#define QMI_CHARGER_EVENT_MASK_POWER_STATE  (0x01)                                                  
#define QMI_CHARGER_EVENT_MASK_BAT_LVL      (0x02)

#define QMI_BAT_LVL_INACTIVE (255)
#define QMI_BAT_LVL_MIN      (0)
#define QMI_BAT_LVL_MAX      (100)

/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_CHARGER_INIT()

  DESCRIPTION
    Initializes the charger state
    
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
);
                                                    

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
);


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
);

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
);


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
);
 
/*===========================================================================
  FUNCTION QMI_CHARGER_SET_BATTERY_LEVEL_LIMITS()

  DESCRIPTION
    It sets the lower and upper batter level reports. An event is generated
    when the current battery level cross the lower or upper limit.
    
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
  uint8 batteryl_lvl_upper_limit
);

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
);


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
);
  

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
);

#endif /* FEATURE_DATA_QMI */
#endif /* _QMI_CHARGER_H */
