#ifndef HDRAUTHI_H
#define HDRAUTHI_H
/*===========================================================================

        S H A - 1   A U T H E N T I C A T I O N   P R O T O C O L   
                   I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This module contains the internal declarations for the SHA-1 
  Authentication Protocol as defined in the security layer of IS-856.

Copyright (c) 2001 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrauthi.h_v   1.1   19 Mar 2002 12:57:36   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrauthi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
10/19/01   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "task.h"
#include "hdrmc_v.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRAUTH_MESSAGE_PRIORITY 40

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRAUTH_TASK_PTR hdrmc_tcb_ptr

/*--------------------------------------------------------------------------
                               AUTH COMMANDS
--------------------------------------------------------------------------*/

/********** Command types **********/

typedef enum
{
  /* No commands defined for AUTH at this time */
  HDRAUTH_DUMMY_CMD
}
hdrauth_cmd_name_enum_type;

typedef struct
{
  hdrauth_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, for debug */
}
hdrauth_cmd_type;


/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRAUTH_PROCESS_CMD

DESCRIPTION
  This function processes commands given to AUTH.

DEPENDENCIES
  None

PARAMETERS
  auth_cmd_ptr - Pointer to the command for AUTH to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrauth_process_cmd (hdrauth_cmd_type * auth_cmd_ptr);

/*===========================================================================

FUNCTION HDRAUTH_PROCESS_IND

DESCRIPTION
  This function processes indications given to AUTH.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for AUTH to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrauth_process_ind (hdrind_ind_name_enum_type ind_name,
            hdrind_ind_data_union_type * ind_data_ptr);

/*===========================================================================

FUNCTION HDRAUTH_PROCESS_MSG

DESCRIPTION
  This function processes messages for AUTH.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrauth_process_msg 
( 
  dsm_item_type * msg_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*===========================================================================

FUNCTION HDRAUTH_PROCESS_SIG

DESCRIPTION
  This function processes signals for AUTH.  

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input 
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrauth_process_sig (rex_sigs_type sig);

/*===========================================================================

FUNCTION HDRAUTH_CREATE_ACPAC

DESCRIPTION
  This function generates the access channel authentication
  code for given message.

DEPENDENCIES
  None

PARAMETERS
  pkt_ptr        - Pointer to DSM item w/ the message to be authenticated
  cryptosync     - Pointer to byte array containing the cryptosync
  cryptosync_len - Length of cryptosync in bytes
  acpac_ptr      - Pointer to empty DSM item or NULL to put ACPAC into
  use_previous_session_context - Use keys from previous session context
  
RETURN VALUE
  HDR error code to indicate the outcome.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrauth_create_acpac
(
  dsm_item_type*   pkt_ptr,
  uint8*           cryptosync,
  uint16           cryptosync_len,
  dsm_item_type**  acpac_ptr,
  boolean          use_previous_session_context
);

#endif /* HDRAUTHI_H */
