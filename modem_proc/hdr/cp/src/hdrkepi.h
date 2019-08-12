#ifndef HDRKEPI_H
#define HDRKEPI_H
/*===========================================================================

              D H   K E Y   E X C H A N G E   P R O T O C O L   
                   I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for the DH Key Exchange Protocol.

Copyright (c) 2001 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrkepi.h_v   1.1   17 Jul 2002 19:56:08   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrkepi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
07/17/02   dna     Queue timer expiration events on hdrmc.event_q because timer
                   callback runs as a separate thread and causes problems.
10/19/01   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdrmc_v.h"


/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRKEP_MESSAGE_PRIORITY 40


/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRKEP_TASK_PTR hdrmc_tcb_ptr

/*--------------------------------------------------------------------------
                               KEP COMMANDS
--------------------------------------------------------------------------*/

/********** Command types **********/

typedef enum
{
  /* No commands defined for KEP at this time */
  HDRKEP_DUMMY_CMD
}
hdrkep_cmd_name_enum_type;

typedef struct
{
  hdrkep_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, for debug */
}
hdrkep_cmd_type;


/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRKEP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to KEP.

DEPENDENCIES
  None

PARAMETERS
  kep_cmd_ptr - Pointer to the command for KEP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrkep_process_cmd (hdrkep_cmd_type * kep_cmd_ptr);

/*===========================================================================

FUNCTION HDRKEP_PROCESS_IND

DESCRIPTION
  This function processes indications given to KEP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for KEP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrkep_process_ind (hdrind_ind_name_enum_type ind_name,
            hdrind_ind_data_union_type * ind_data_ptr);

/*===========================================================================

FUNCTION HDRKEP_PROCESS_MSG

DESCRIPTION
  This function processes messages for KEP.

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
extern void hdrkep_process_msg 
( 
  dsm_item_type * msg_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*===========================================================================

FUNCTION HDRKEP_PROCESS_SIG

DESCRIPTION
  This function processes signals for KEP.  

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
extern void hdrkep_process_sig (rex_sigs_type sig);

/*===========================================================================

FUNCTION HDRKEP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the key exchange
  protocol.

DEPENDENCIES
  None

PARAMETERS
  timer_id - The ID of the timer that expired.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrkep_process_timer
(
  uint32 timer_id
);

#endif /* HDRKEPI_H */
