#ifndef DS_EPC_SYS_H
#define DS_EPC_SYS_H
/*===========================================================================

                      DS_EPC_SYS.H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_sys.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/01/11   fjia    Create the module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_DATA_WLAN_MAPCON
#include "stm2.h"
#include "ds_epc_sm_defs.h"
/*===========================================================================
FUNCTION      DS_EPC_SYS_SM_INIT()

DESCRIPTION   Initialize EPC SYS SM machine.Initial state of SYS SM is 
              EPC_SSM_STATE_INIT.
	 
PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_sys_sm_init(void);

/*===========================================================================
FUNCTION      DS_EPC_SYS_SM_INPUT_HANDLER()

DESCRIPTION   Set the user-defined data to EPC SYS State Machine.
	 
PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_sys_sm_input_handler
( 
  epc_ssm_input_event_type input, 
  void *info 
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_SM_SET_USER_DATA()

DESCRIPTION   Set the user-defined data to EPC SYS State Machine.
	 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_sys_sm_set_user_data( void *user_data);

/*===========================================================================
FUNCTION      DS_EPC_SYS_SM_GET_USER_DATA()

DESCRIPTION   
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void *ds_epc_sys_sm_get_user_data( void );

/*===========================================================================
FUNCTION      DS_EPC_SYS_SM_GET_STATE()

DESCRIPTION   Get current state of EPC SYS SM

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
stm_state_t ds_epc_sys_sm_get_state( void );

#endif /*FEATURE_DATA_WLAN_MAPCON*/

#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_SYS_H */
