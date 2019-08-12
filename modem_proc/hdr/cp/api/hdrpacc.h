#ifndef HDRPACC_H
#define HDRPACC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R   P A C K E T  A P P L I C A T I O N 
       
                            C O N T R O L L E R
       
            C O N F I G U R A T I O N   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the configuration interface of the HDR Packet Application
  Controller.
  
EXTERNALIZED FUNCTIONS
  hdrpacc_powerup_init
    Powerup init of protocol configuration.
    
  hdrpacc_process_ind
    Process indications given to HDRPACC.
  
  hdrpacc_process_msg
    Process messages given to HDRPACC.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrpacc_powerup_init must be called at startup.  
  
  Copyright (c) 2004,2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrpacc.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/06   mpa     Removed HDRPACC_UPDATE_FCP_STATE_IN_DB_CMD.
06/05/06   pba     Added new command HDRPACC_UPDATE_FCP_STATE_IN_DB_CMD.
05/17/06   etv     Added hdrpacc_update_flow_mapping.
03/01/05   pba     Moved hdrpacc_cmd_type definition from hdrpacmci.h 
                   (hdrpacmci.h is obsoleted)
03/01/05   pba     Separated handling of In-Use and In-Config messages for PAC.
09/30/04   mpa     Added GAUP messages.
06/11/04   mpa     Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrscmrmac3.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* For hdrpacc - arbitrarily start at 20 
 * We don't want to overlap with PAC DS cmds unless we create a new ID 
 * (Events) 
 */

typedef enum
{
  HDRPACC_CONFIGURE_CMD = 20,
  HDRPACC_UPDATE_FLOW_MAPPING_IN_SCMDB_CMD = 21,
  /* HDRPACC_UPDATE_FCP_STATE_IN_DB_CMD = 22 - obsolete 
     (don't reuse ID because of QXDM) */
} hdrpacc_cmd_name_enum_type;

typedef struct
{
  hdrhai_app_subtype_enum_type app;  /* Application to Configure */
  hdrhai_stream_enum_type stream;
} hdrpacc_configure_params_type;

typedef struct
{
  uint16 attrib_id;
  hdrscmrmac3_associated_flows_struct_type flow_mapping;
} hdrpacc_update_flow_mapping_params_type;

typedef union
{
  hdrpacc_configure_params_type config;
  hdrpacc_update_flow_mapping_params_type update;
} hdrpacc_cmd_params_union_type;

typedef struct
{
   hdrpacc_cmd_name_enum_type name;       /* Name of the command */
   hdrhai_protocol_name_enum_type sender; /* protocol giving cmd, for debug */
   hdrpacc_cmd_params_union_type params;  /* Common command field */
} hdrpacc_cmd_type;

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION HDRPACC_POWERUP_INIT

DESCRIPTION
  This function should be called once upon system startup.  The protocol 
  configuration is initialized. 
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Returns a pointer to the current config data (always valid)

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrpacc_powerup_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_PROCESS_IND

DESCRIPTION
  This function processes indications given to PACC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrpacc_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/* EJECT */
/*===========================================================================
FUNCTION HDRPACC_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAPP1, HDRAPP2, HDRAPP3. Indications given 
  to PACC are processed in HDRMC task context.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrpacc_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_PROCESS_MSG

DESCRIPTION
  This function processes messages for PACC.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void hdrpacc_process_msg
(
  dsm_item_type *msg_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-config instance. All Config 
  messages are processed in the HDRMC task. 

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrpacc_config_msg_cb
(
  dsm_item_type *msg_ptr
);


/*===========================================================================
FUNCTION HDRPACC_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-use instance. 
  
  All InUse Config messages are processed in the HDRMC task. 

  
DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrpacc_msg_cb
(
  dsm_item_type *msg_ptr
);


/* <EJECT> */
/*===========================================================================
FUNCTION HDRPAC_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Packet 
  Application Controller.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrpacc_process_timer
(
  uint32 timer_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_PROCESS_CMD

DESCRIPTION
  This function processes commands for the PAC

DEPENDENCIES
  None
  
PARAMETERS
  pac_cmd_ptr - Pointer to the stucture containing the command

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void hdrpacc_process_cmd
(
  hdrpacc_cmd_type *pac_cmd_ptr 
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_CONFIGURE

DESCRIPTION
  This function conditionally commands HDRPAC to configure 
  (will configure LUP, RLP, FCP and RLP as needed)

DEPENDENCIES
  Runs in HDRMC task context

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the configure command is accepted
  FALSE - the configure command is rejected

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrpacc_configure
(
  hdrhai_protocol_name_enum_type sender 
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_UPDATE_FLOW_MAPPING_IN_SCMDB

DESCRIPTION
  If RLP flow x is bound to MAC flow 1, and a GAUP update binds the same RLP 
  flow x to a different MAC flow (say 2), RMAC unbinds the RLP flow x from 
  MAC flow 1 before binding it to MAC flow 2. We need to update the SCM DB
  in order to keep it consistent for this implicit unbind action.

DEPENDENCIES
  None.

PARAMETERS
  sender       - protocol calling this function.
  flow_mapping - Pointer to Asso flows. struct containing the updated 
                 mapping.
  attrib_id    - Id of the RMAC3 attribute that needs to be updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrpacc_update_flow_mapping_in_scmdb
(
  hdrhai_protocol_name_enum_type sender,
  hdrscmrmac3_associated_flows_struct_type *flow_mapping,
  uint16 attrib_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPACC_UPDATE_FCP_STATE_IN_SCMDB

DESCRIPTION
 This functions posts a command to PACC to update FCP's state in SCMDB.

DEPENDENCIES
  None.

PARAMETERS
  sender - protocol calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrpacc_update_fcp_state_in_scmdb
(
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */
#endif /* HDRPACC_H */


