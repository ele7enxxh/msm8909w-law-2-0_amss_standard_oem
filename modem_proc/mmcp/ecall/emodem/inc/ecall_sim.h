#ifndef ECALL_SIM_H_
#define ECALL_SIM_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               E C A L L _ S I M  
                            H E A D E R   F I L E

GENERAL DESCRIPTION 
This file contains the API to query the SIM 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_sim.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_IVS

#include "ecall_modem_apis.h"

#include "rex.h"

#include "reg_cm.h"
#include "reg_mm.h"
#include "reg_sim.h"
#include "reg_ecall.h"

#ifndef FEATURE_ECALL_LCU45
#include "reg_task_v.h"
#else
#include "reg_task.h"
#endif /* FEATURE_ECALL_LCU45 */

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib.h" 
#endif /* FEATURE_MMGSDI_SESSION_LIB */

typedef struct
{
  /* eCall State */
  ecall_operating_mode_type      ecall_operating_mode;
  boolean                        is_pending_send_operating_mode_to_nas;

  /* MMGSDI State */
  mmgsdi_session_id_type         session_id;            // Unique session id provided by MMGSDI
  boolean                        session_activated;     // MMSGDI session is mapped to a provisioning app
  mmgsdi_slot_id_enum_type       slot_id;               // Subscription slot id from SUBS_READY
  mmgsdi_app_enum_type           app_type;              // Subscription APP type from SUBS_READY (SIM, USIM, CSIM, etc)
  mmgsdi_session_type_enum_type  session_type;          // Subscription session type (1X_PRI, GW_SEC, etc)
  boolean                        is_sim_read_success;   // SIM initialization is completed
  boolean                        is_est_fdn_enabled;
  uint8                          sbit89;
  uint8                          sbit2;
  uint8                          sbit4;

  /* CM State */
  sys_modem_as_id_e_type         as_id;                 // Modem AS id from CM SUBS_PREF event (maps session to AS)
  boolean                        is_available;          // CM subs is available for use
  
}ecall_prov_app_info_s_type;

/*=========================================================================
STRUCT: ecall_sim_cm_subs_pref_info_s_type
 
 Holds the state of the current subscription pref info.
 Each time a SUBSCRIPTION_PREF_INFO event arrives, the previous state
 is needed to compare with in order to detect transitions between the
 available and unavailable states.
=========================================================================*/
typedef struct
{
  struct
  {
    mmgsdi_session_type_enum_type          session_type_1x;
    boolean                                is_available_1x;
    mmgsdi_session_type_enum_type          session_type_gw;
    boolean                                is_available_gw;
    boolean                                is_operational;
  } sub[MAX_AS_IDS];
} ecall_sim_cm_subs_pref_info_s_type;

typedef struct
{
  mmgsdi_client_id_type  client_id;

  ecall_sim_cm_subs_pref_info_s_type  ecall_cm_subs_info; /* CM Subs Info  */
  ecall_prov_app_info_s_type          ecall_app1_info;    /* GW_PRI, subs1 */
  ecall_prov_app_info_s_type          ecall_app2_info;    /* GW_SEC, subs2 */
  ecall_prov_app_info_s_type          ecall_app3_info;    /* GW_TER, subs3 */
} ecall_sim_cfg_s_type;

/*===========================================================================

                  F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/

/*==============================================================================

FUNCTION NAME

    ECALL_SIM_MMGSDI_INIT

DESCRIPTION

  Function to get client ID for MMGSDI API calls 

RETURN VALUE
   NONE

==============================================================================*/

void ecall_sim_mmgsdi_init
(
  void 
);

/*============================================================================
   FUNCTION:      ECALL_SEND_MODEM_CONFIG_TO_NAS

   DESCRIPTION:
     This function provides indication to NAS about the ecall modem 
     configuration - ecall_only or ecall_normal

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==============================================================================*/
void ecall_send_modem_config_to_nas 
(
  sys_modem_as_id_e_type as_id
);

/*===========================================================================
   FUNCTION ECALL_SIM_CFG_S_PTR

   DESCRIPTION
     Return the pointer to the ecall SIM configuration data.

   DEPENDENCIES
     none

   RETURN VALUE
     Pointer to the internal configuration data.

   SIDE EFFECTS
     None.
===========================================================================*/
ecall_sim_cfg_s_type * ecall_sim_cfg_s_ptr(
  void
);

/*============================================================================
   FUNCTION:      ECALL_PROCESS_SIM_CONFIG_UPDATE

   DESCRIPTION:
     This function will update the eCall operating mode to clients by
     callback functions.

   DEPENDENCIES:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==============================================================================*/
void ecall_process_sim_config_update
(
  ecall_operating_mode_type ecall_oper_mode
);

/*===========================================================================
FUNCTION ECALL_PROCESS_SIM_CLIENT_ID_REG_SUCCESS

DESCRIPTION
    This function is the callback function used for a register for client ID.

DEPENDENCIES
    None

INPUT
    mmgsdi_client_id_type gsdi_client_id

RETURN VALUE
    None

OUTPUT
   
===========================================================================*/
void ecall_process_sim_client_id_reg_success 
( 
  mmgsdi_client_id_type gsdi_client_id
);

/*==============================================================================
    FUNCTION NAME  ECALL_PROCESS_SIM_EVENTS
     
    DESCRIPTION
        This function is the event handling function for different events from MMGSDI.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_event_data_type *event_ptr
    
    RETURN VALUE
        None
    
    OUTPUT
        None
==============================================================================*/
void ecall_process_sim_events
(
  const mmgsdi_event_data_type event
);

     
/*===========================================================================
   FUNCTION:      ECALL_PROCESS_SIM_READ_SUCCESS

   DESCRIPTION:
     This function will update ecall_operating_mode by the inputs:
	 Service bit 89, FDN service bit 2 and SDN service bit 4

   DEPENDENCIES:
     None

   LIMITATIONS:
     Node

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_process_sim_read_success
(
  const uint8 sbit89,
  const uint8 sbit2,
  const uint8 sbit4,
  ecall_prov_app_info_s_type *appInfo_ptr
);

/*===========================================================================
   FUNCTION:      ECALL_PROCESS_SIM_READ_FAILED

   DESCRIPTION:
     This function will update the is_sim_read_success variable and queue
     a command to IVS to process with operating mode.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_process_sim_read_failed
(
  ecall_prov_app_info_s_type *appInfo_ptr
);

/*===========================================================================
   FUNCTION:      ECALL_SIM_PROCESS_SESSION_OPEN_SUCCESS

   DESCRIPTION:
     This function will set ecall_sim global ecall_session_id to the
     passed in data.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_process_session_open_success
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        slot_id,
  mmgsdi_app_enum_type            app_type
);

/*===========================================================================

FUNCTION ECALL_SIM_CFG_INIT

DESCRIPTION 
   This API will initialize the eCall SIM configuration data to default values.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ecall_sim_cfg_init
(
  void
);

/*===========================================================================
   FUNCTION:      ECALL_SIM_SET_EST_FDN_ENABLED

   DESCRIPTION:
     This function will set ecall_sim global ecall_est_fdn_enabled to the
     passed in data.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_set_est_fdn_enabled
(
  const uint8                 est_fdn,
  ecall_prov_app_info_s_type *appInfo_ptr
);

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_GET_APPINFO_BY_SESSION_TYPE
     
    DESCRIPTION
        This API will return the eCall Provisioning App pointer by
        the input session_type.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_session_type_enum_type
    
    RETURN VALUE
        ecall_prov_app_info_s_type
    
    OUTPUT
        None
==============================================================================*/
ecall_prov_app_info_s_type* ecall_sim_get_appinfo_by_session_type
(
  mmgsdi_session_type_enum_type session_type
);

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_GET_APPINFO_BY_AS_ID
     
    DESCRIPTION
        This API will return the eCall Provisioning App pointer by
        the input as_id.
    
    DEPENDENCIES
        None
    
    INPUT
        sys_modem_as_id_e_type
    
    RETURN VALUE
        ecall_prov_app_info_s_type
    
    OUTPUT
        None
==============================================================================*/
ecall_prov_app_info_s_type* ecall_sim_get_appinfo_by_as_id
(
  sys_modem_as_id_e_type  as_id
);

/*==============================================================================
    FUNCTION NAME  ECALL_SIM_GET_APPINFO_BY_SLOT_ID
     
    DESCRIPTION
        This API will return the eCall Provisioning App pointer by
        the input slot_id.
    
    DEPENDENCIES
        None
    
    INPUT
        mmgsdi_slot_id_enum_type
    
    RETURN VALUE
        ecall_prov_app_info_s_type
    
    OUTPUT
        None
==============================================================================*/
ecall_prov_app_info_s_type* ecall_sim_get_appinfo_by_slot_id
(
  mmgsdi_slot_id_enum_type slot_id
);

/*===========================================================================
   FUNCTION:      ECALL_SIM_PROCESS_SET_AND_SEND_OPERATING_MODE

   DESCRIPTION:
     This function set and send ecall_operating_mode by the parameters
     in the appInfo, and send it to NAS.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_process_set_and_send_operating_mode
(
  sys_modem_as_id_e_type as_id
);

/*===========================================================================
   FUNCTION:      ECALL_SIM_PROCESS_UPDATE_CM_SUBS_INFO

   DESCRIPTION:
     This function will process the updated CM SUBS info and find out is there
     any pending ecall_operating_mode needs to be reported to NAS.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void ecall_sim_process_update_cm_subs_info
(
  void
);

#else

void ecall_sim_dummy_func(void);

#endif //End Featurisation

#endif /* ECALL_DIAG_H_ */

