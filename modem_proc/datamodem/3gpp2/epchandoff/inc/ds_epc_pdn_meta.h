#ifndef DS_EPC_PDN_META_H
#define DS_EPC_PDN_META_H
/*===========================================================================

                      DS_EPC_PDN_META. H
                   
DESCRIPTION
  DS EPC PDN Meta module.
 
  This module handles DS EPC Handoff related functionalities.
 
                 ds_epc_pdn_ctl -- ds_epc_pdn_meta
                        \              /      \
                         \            /        \
                          \          /   ds_epc_pdn_meta_sm
                         ds_epc_pdn_ho    /
                               |         /
                               |        /
                        ds_epc_pdn_ho_sm
 
  This module contains EPC meta sm related processing.
 
EXTERNALIZED FUNCTIONS

 Copyright (c) 2011-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_pdn_meta.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/23/11    fjia    Created Module
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_sm_defs.h"
#include "ds_epc_pdn_ctl.h"
#include "stm2.h"

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_SM_INIT()

DESCRIPTION   
	 
PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_meta_sm_instance_init
(
  uint32 sm_id
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_SM_SET_USER_DATA()

DESCRIPTION   
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_meta_sm_set_user_data
( 
  uint32 sm_id, 
  void *user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_PROC_INPUT

DESCRIPTION   Handles external input to PDN META SM

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_meta_proc_input
( 
  epc_pdn_meta_sm_input_event_type input, 
  ds_epc_cb_type *epc_pdn_instance,
  void *info
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_SM_GET_STATE()

DESCRIPTION   
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
stm_state_t ds_epc_pdn_meta_sm_get_state( uint32 sm_id );

/*===========================================================================
FUNCTION      ds_epc_pdn_meta_teardown_pdn

DESCRIPTION   Teardown the PDN for failure cases.

DEPENDENCIES  None.

PARAMETERS    instance_ptr: EPC instance ptr 
              local_abort: If tear down the PDN locally or not. 
 
RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_meta_teardown_pdn
(
  ds_epc_cb_type                         *instance_ptr,
  boolean                                 local_abort                       
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_PROCESS_EPC_IFACE_UP_EV

DESCRIPTION   process EPC UP event for ho sm.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
int ds_epc_pdn_meta_process_epc_iface_up_ev
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_PROCESS_EPC_IFACE_DOWN_EV

DESCRIPTION   process EPC DOWN event for ho sm.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
int ds_epc_pdn_meta_process_epc_iface_down_ev
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_EPC_IFACE_UP_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when EPC is UP.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_meta_epc_iface_up_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_EPC_IFACE_DOWN_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when EPC is down.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_meta_epc_iface_down_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_META_SM_GET_PDN_INSTANCE_PTR

DESCRIPTION   
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_cb_type* ds_epc_pdn_meta_sm_get_pdn_instance_ptr
(
  stm_state_machine_t*  sm
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION       DS_EPC_PDN_META_IS_EPC_CALL

DESCRIPTION    Check if this is EPC call by check the rat of a PDN. May need
               to call the iotl to get the accurate info. Since the prefer
               system change saved for non-EPC call may not accurate.

DEPENDENCIES   NONE 

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_epc_pdn_meta_is_epc_call
( 
  ds_epc_cb_type *instance_ptr
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

#endif /*FEATURE_EPC_HANDOFF*/
#endif /*DS_EPC_PDN_META_H*/

