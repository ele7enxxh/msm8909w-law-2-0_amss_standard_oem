#ifndef DS_DSD_COMMON_H
#define DS_DSD_COMMON_H
/*===========================================================================

                      D S _ D S D _ C O M M O N . H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_common.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds_dsd_event_dp.h"
#include "ds_dsd_ext_i.h"
#include "ds3gsubsmgr.h"
#include "cm.h"
#include "sys.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------
  MAXIMUM number of Radio systems supported in DSD module
-------------------------------------------------------------------------*/
#define DS_DSD_MAX_RADIO  7 /* MAX num of radios supported by DSD */

/*-------------------------------------------------------------------------
  Bit Masks to define throttle. Each client is a bit
-------------------------------------------------------------------------*/
#define DS_DSD_HAND_DOWN_THROTTLE                 0x00
#define DS_DSD_CALL_CONNECTED_THROTTLE            0x01
#define DS_DSD_NW_DETACHED_THROTTLE               0x02
#define DS_DSD_DCTM_THROTTLE                      0x03
#define DS_DSD_AVOID_HAND_DOWN_THROTTLE           0x04
#define DS_DSD_NW_DETACHED_HYST_THROTTLE          0x05
#define DS_DSD_EMERGENCY_CALL_THROTTLE            0x06

/*-------------------------------------------------------------------------
  Macro to convert DSD RADIO SM to Mask value
-------------------------------------------------------------------------*/
#define CONVERT_RADIO_SM_TO_MASK( val )   ((uint32)1<<(int)(val))

/*-------------------------------------------------------------------------
  Macro to validate DSD RADIO SM
-------------------------------------------------------------------------*/
#define VALIDATE_DSD_RADIO_SM(radio_sm)             \
((radio_sm >= DS_DSD_RADIO_SM_MIN) ||              \
 (radio_sm <  DS_DSD_RADIO_SM_COUNT))

/*-------------------------------------------------------------------------
  Enum of DSD state machines
-------------------------------------------------------------------------*/
typedef enum
{
  DS_DSD_RADIO_SM_MIN = 0,
  DS_DSD_RADIO_SM_LTE = DS_DSD_RADIO_SM_MIN,
  DS_DSD_RADIO_SM_HDR,
  DS_DSD_RADIO_SM_1X,
  DS_DSD_RADIO_SM_GSM,
  DS_DSD_RADIO_SM_TDSCDMA,
  DS_DSD_RADIO_SM_WCDMA,
  DS_DSD_RADIO_SM_WLAN,
  DS_DSD_RADIO_SM_COUNT
} ds_dsd_radio_sm_enum_type;

/*-------------------------------------------------------------------------
  States of State machine
-------------------------------------------------------------------------*/
typedef enum
{
  DS_DSD_SM_STATE_INVALID = 0,
  DS_DSD_SM_STATE_OOS,
  DS_DSD_SM_STATE_ACQ_HYST,
  DS_DSD_SM_STATE_IN_SVC,
  DS_DSD_SM_STATE_OOS_HYST,
  DS_DSD_SM_STATE_MAX = DS_DSD_SM_STATE_OOS_HYST
}ds_dsd_sm_state_enum_type;

/*-------------------------------------------------------------------------
  States of Sub-state machine
-------------------------------------------------------------------------*/
typedef enum
{
  DS_DSD_SM_SUB_STATE_INVALID = 0,
  DS_DSD_SM_SUB_STATE_UNTHROTTLE,
  DS_DSD_SM_SUB_STATE_THROTTLE,
  DS_DSD_SM_SUB_STATE_MAX = DS_DSD_SM_SUB_STATE_THROTTLE
}ds_dsd_sub_sm_state_enum_type;

/*-------------------------------------------------------------------------
  Radio information enum
-------------------------------------------------------------------------*/
typedef enum
{
  DS_DSD_RADIO_INFO_SRV_STATUS = 0,
  DS_DSD_RADIO_INFO_MIN = DS_DSD_RADIO_INFO_SRV_STATUS,
  DS_DSD_RADIO_INFO_SRV_DOMAIN,
  DS_DSD_RADIO_INFO_RAT_MASK,
  DS_DSD_RADIO_INFO_SO_MASK,
  DS_DSD_RADIO_INFO_CELL_SRV_IND,
  DS_DSD_RADIO_INFO_SM_THROTTLE_IND,
  DS_DSD_RADIO_INFO_ROAM_STATUS,
  DS_DSD_RADIO_INFO_MAX = DS_DSD_RADIO_INFO_ROAM_STATUS
}ds_dsd_radio_info_enum_type;

/*-------------------------------------------------------------------------
  Radio information structure
-------------------------------------------------------------------------*/
typedef struct
{
  sys_srv_status_e_type         srv_status;
  sys_srv_domain_e_type         srv_domain;
  uint32                        rat_mask;
  int32                         so_mask;
  cm_cell_srv_ind_s_type        cell_srv_ind;
  boolean                       roaming_status;

#ifdef FEATURE_DATA_WLAN_MAPCON
  boolean                       acq_wqe_support;
  boolean                       lost_wqe_support;
  ds_dsd_ext_wlan_addr_type     wlan_addr;
#endif /* FEATURE_DATA_WLAN_MAPCON */

}ds_dsd_radio_info_type;

/*-------------------------------------------------------------------------
  Sub-state machine structure
-------------------------------------------------------------------------*/
typedef struct
{
  ds_dsd_sub_sm_state_enum_type state; 
  ds_dsd_sub_sm_state_enum_type state_prev;
  uint16                        throttle_register;
}ds_dsd_sub_sm_type;

/*-------------------------------------------------------------------------
  State machine structure
-------------------------------------------------------------------------*/
typedef struct
{
  dsd_radio_type                radio;
  ds_dsd_sm_state_enum_type     state;
  ds_dsd_sm_state_enum_type     state_prev;
  ds_dsd_sub_sm_type           *sub_sm_ptr;
  boolean                       is_remote;
  ds_dsd_radio_info_type        radio_info;
  ds_dsd_radio_info_type        radio_info_prev;
}ds_dsd_sm_type;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_SM_STATE

DESCRIPTION   This function returns the state of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  State of a State Machine.

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_sm_state_enum_type ds_dsd_common_get_sm_state
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_IS_SM_THROTTLED

DESCRIPTION   Utility to find if the state machine is throttled
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if throttled.
              FALSE otherwise.

SIDE EFFECTS  None
===========================================================================*/
boolean ds_dsd_common_is_sm_throttled
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_REMOTE_FLAG

DESCRIPTION   This function returns the is_remote of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  Returns SO MASK.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_get_remote_flag
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_SRV_STATUS

DESCRIPTION   This function returns the srv status of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  Service status.

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_status_e_type ds_dsd_common_get_radio_srv_status
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_SRV_DOMAIN

DESCRIPTION   This function returns the service domain of a state machine 
              belonging to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  service domain.

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_domain_e_type ds_dsd_common_get_radio_srv_domain
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_RAT_MASK

DESCRIPTION   This function returns the rat mask of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  RAT mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_common_get_radio_rat_mask
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_SO_MASK

DESCRIPTION   This function returns the so mask of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  SO mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_common_get_radio_so_mask
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_CELL_SRV_IND

DESCRIPTION   This function returns the cell serv info of a state machine 
              belonging to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  Cell service info.

SIDE EFFECTS  None.
===========================================================================*/
cm_cell_srv_ind_s_type ds_dsd_common_get_radio_cell_srv_ind
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_ROAM_STATUS

DESCRIPTION   This function returns the roam status of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  roaming status.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_get_radio_roam_status
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_CMP_RADIO_INFO

DESCRIPTION   This function compares the previous and current information 
              values of the state machine belonging to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:   value changed.
              FALSE:  value unchanged.

SIDE EFFECTS  Once compared current value will be copied to previous value.
===========================================================================*/
boolean ds_dsd_common_cmp_radio_info
(
  dsd_radio_type               radio,
  ds_dsd_radio_info_enum_type  param,
  ds3gsubsmgr_subs_id_e_type   subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_CMP_WLAN_INFO

DESCRIPTION   This function compares the previous and current information 
              values of the WLAN state machine.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:   value changed.
              FALSE:  value unchanged.

SIDE EFFECTS  Once compared current value will be copied to previous value.
===========================================================================*/
boolean ds_dsd_common_cmp_wlan_info
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_RESET_SM_INSTANCE

DESCRIPTION   This function resets the state-machine's structure to its 
              default value.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_common_reset_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              radio
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_CREATE_SM_INSTANCE

DESCRIPTION   This function creates an instance of DSD statemachine.
 
DEPENDENCIES  None.

RETURN VALUE  If success, return pointer to the created SM instance 
              If failure, return NULL 

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_common_create_sm_instance
(
  boolean  create_sub_sm
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_CONVERT_DSD_RADIO_TO_RADIO_SM

DESCRIPTION   This function converts DSD radio to radio SM type
 
DEPENDENCIES  None.

RETURN VALUE  RADIO SM enum

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_radio_sm_enum_type ds_dsd_common_convert_dsd_radio_to_radio_sm
(
  dsd_radio_type  dsd_radio
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_VALIDATE_EVENT

DESCRIPTION   This function validates DSD event from event dispatcher
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:  Success
              FALSE: Failure

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_validate_event
(
  dsd_event_type              notified_event,
  dsd_event_client_type       client,
  dsd_event_client_type       client_to_compare,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  void                       *aux_info_ptr
);

/*===========================================================================
FUNCTION      DS_DSD_COMMON_STATE_TRANSITION

DESCRIPTION   This is the transition function for the generic state machine.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  The state is moved to new state.
===========================================================================*/
void ds_dsd_common_state_transition
(
  dsd_event_type              new_event,
  ds_dsd_sm_type             *ds_dsd_sm,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              radio,
  dsd_radio_type              aux_radio
);

#endif /* DS_DSD_COMMON_H */
