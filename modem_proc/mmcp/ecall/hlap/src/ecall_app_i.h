#ifndef ECALL_APP_I_H_
#define ECALL_APP_I_H_
/*===========================================================================

                                E C A L L _ A P P _ I . H

DESCRIPTION
  Internal eCall functions header file.

REFERENCES
  3GPP TS 26.267

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright(c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_app_i.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/06/12   DL      Added free-floating 
10/20/11   SAR     Support for PBM session API's for Dual sim, registering for 
                   Notifications and storing the FDN/SDN entries globally  
10/11/11   DL      Added input to ecall_process_session_get_call_info_cmd()
10/06/11   DL      Added ecall_process_session_call_info_cmd() and 
                   ecall_reset_type_of_call()
09/20/09   PA      Initial version.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_APP

#include "ecall_app.h"
#include "ecall_efs.h"
#include "ecall_modem_apis.h"
#include "ecall_app.h"
#include "event.h"
#include "ecall_app_task.h"

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

typedef struct
{
  ecall_type_of_call type_of_call;
  ecall_activation_type activation_type;
  ecall_efs_config_type efs_params;
}ecall_config_params_type;

typedef struct 
{
  uint8              ecall_msd_msg_id;
  cm_client_id_type  ecall_app_cm_client_id;
  ecall_config_params_type ecall_config_params;
}ecall_app_struct_type;

/*===========================================================================

                  F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/
/*==========================================================================
  FUNCTION   ECALL_APP_PBM_INIT

  DESCRIPTION
    Registers with pbm interface for notificaiton or events.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_app_pbm_init
(
  void
);

/*==========================================================================
  FUNCTION   ECALL_APP_CALL_STATUS_INIT

  DESCRIPTION
    This API will initialize the eCall App call status table.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_app_call_status_init
(
  void
);

/*==========================================================================
  FUNCTION      ECALL_APP_PROCESS_FDN_SIM_FILE_INFO

  DESCRIPTION
  This function reads the test and reconfiguration numbers
  programmeed in the FDN of the USIM and stores in app global structure

  DEPENDENCIES
    None.

  RETURN VALUE
    None    

  SIDE EFFECTS
    None
============================================================================*/
void ecall_app_process_fdn_sim_file_info
(
  pbm_phonebook_type pb_id,
  pbm_extended_fileinfo_s_type info
);

/*==========================================================================
  FUNCTION      ECALL_APP_PROCESS_SDN_SIM_FILE_INFO

  DESCRIPTION
  This function reads the test and reconfiguration numbers in 
  programmeed in the SDN of the USIM and stores in app global structure

  DEPENDENCIES
    None.

  RETURN VALUE
    None    

  SIDE EFFECTS
    None
============================================================================*/
void ecall_app_process_sdn_sim_file_info
(
  pbm_phonebook_type pb_id,
  pbm_extended_fileinfo_s_type info
);

/*==========================================================================
  FUNCTION   ECALL_APP_PROCESS_PBM_NOTIFY_UPDATE

  DESCRIPTION
    When PB is ready or if entries are added/deleted/updated/refresh,
    ecall app would update it's global variable for FDN/SDN device

  DEPENDENCIES
    None.

  RETURN VALUE
    
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_app_process_pbm_notify_update
(
  pbm_phonebook_type pb_id
);

/*==========================================================================

  FUNCTION ECALL_CONSTRUCT_MSD

  DESCRIPTION
    This function Contrcuts the MSD (with/without GPS) and then originates an ecall
    It gets the ecall_cm_client_id that the ecall_app_task recevied thru cm_client_init()

  DEPENDENCIES
    None.

  RETURN VALUE
    
  SIDE EFFECTS
    None

===========================================================================*/

void ecall_construct_msd
(
  ecall_msd_array_type ecall_app_msd_byte_array
);

/*==========================================================================

  FUNCTION ECALL_APP_IS_EMERGENCY_CALL_ON_GOING

  DESCRIPTION
    This API will check is there any on-going emergency call.

  DEPENDENCIES
    None.

  RETURN VALUE
    TRUE  - One or more on-going emergency call(s)
    FALSE - No on-going emergency call

  SIDE EFFECTS
===========================================================================*/
boolean ecall_app_is_emergency_call_on_going
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_DUMP_NON_EMERGENCY_PS_CALL

  DESCRIPTION
    This API will check is there any on-going non-emergency and non-PS calls.
    If yes, eCall App will call cm_mm_call_cmd_end() to dump them.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
===========================================================================*/
void ecall_app_dump_non_emergency_ps_call
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_PROCESS_SETUP_CALL

  DESCRIPTION
    This function will call CM API to setup the call.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_process_setup_call
(
  cm_call_id_type call_id
);

/*==========================================================================

  FUNCTION ECALL_APP_PROCESS_UPDATE_CALL_STATUS

  DESCRIPTION
    This API will update the eCall App call status table.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
===========================================================================*/
void ecall_app_process_update_call_status
(
  cm_call_event_e_type           call_event,
  cm_call_id_type                call_id,
  cm_call_type_e_type            call_type
);

/*==========================================================================

  FUNCTION ECALL_ORIGINATE_CALL

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean - 1 = Call Originated Successfully
              0 = Call Origination Failed

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_originate_call
(
  void
);

/*==========================================================================

  FUNCTION ECALL_END_CALL

  DESCRIPTION
    This function ends an ecall, triggered by the DIAG ?

  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean - 1 = Call Originated Successfully
              0 = Call Origination Failed

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_end_call
(
  void
);

/*===========================================================================

FUNCTION REPORT_ECALL_EVENT

DESCRIPTION
 
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void report_ecall_event
(
  event_id_enum_type event_id
);

/*==========================================================================

  FUNCTION ECALL_APP_SET_ECALL_START_CONFIG

  DESCRIPTION
    This function will set type_of_call and activation_type to ecall_app_struct.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_set_ecall_start_config
(
  ecall_start_config_type *start_config_ptr
);
 
/*==========================================================================

  FUNCTION ECALL_APP_GET_EFS_PARAMS

  DESCRIPTION
    This function will be called by ecall_app_task when ecall_start_session
    is triggered. Will copy EFS data from ecall_app_task_struct to ecall_app_struct

  DEPENDENCIES
    None.

  RETURN VALUE
    
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_get_efs_params
(
  ecall_efs_config_type *efs_config
); 

/*==========================================================================

  FUNCTION ECALL_PROCESS_SESSION_GET_CALL_INFO_CMD
    This function will call the passed in registered callback with current
    call info, such as type_of_call and activation_type

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE
    
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_process_session_get_call_info_cmd
(
  ecall_session_call_info_cb_type call_info_cb
);

/*==========================================================================

  FUNCTION ECALL_RESET_TYPE_OF_CALL
    This function will reset type_of_call to default value ECALL_NO_ACTIVE_CALL

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE
    
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_reset_type_of_call
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_INIT

  DESCRIPTION
    This function will initialize ecall_app global structure variables

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_init
(
  void
);

#else

void ecall_app_i_dummy_func(void);

#endif //End Featurisation


#endif /* ECALL_APP_H_ */
