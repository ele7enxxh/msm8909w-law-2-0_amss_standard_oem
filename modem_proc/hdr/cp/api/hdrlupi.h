#ifndef HDRLUPI_H
#define HDRLUPI_H
/*===========================================================================

              L O C A T I O N   U P D A T E   P R O T O C O L
                   I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for the Location Update Protocol.

Copyright (c) 2001 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrlupi.h_v   1.7   19 Nov 2001 18:42:48   shuang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrlupi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/17/01   vas     Moved msg id defines to external header file so they can be
                   accessed by PAC
07/10/01   dna     Change LUP to not do AT initiated negotiation
03/25/01   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdrmc_v.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRLUP_MESSAGE_PRIORITY 40


/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRLUP_TASK_PTR hdrmc_tcb_ptr

/*--------------------------------------------------------------------------
                               LUP COMMANDS
--------------------------------------------------------------------------*/

/********** Command types **********/

typedef enum
{
  HDRLUP_SEND_LOC_CMD, /* Send an unsolicited LocationNotification message */
  HDRLUP_UPDATE_LOC_CMD /* Updade current location */
}
hdrlup_cmd_name_enum_type;

typedef struct
{
  hdrlup_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, for debug */
}
hdrlup_cmd_type;


/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRLUP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to LUP.

DEPENDENCIES
  None

PARAMETERS
  lup_cmd_ptr - Pointer to the command for LUP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlup_process_cmd (hdrlup_cmd_type * lup_cmd_ptr);

/*===========================================================================

FUNCTION HDRLUP_PROCESS_IND

DESCRIPTION
  This function processes indications given to LUP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for LUP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlup_process_ind (hdrind_ind_name_enum_type ind_name,
            hdrind_ind_data_union_type * ind_data_ptr);

/*===========================================================================

FUNCTION HDRLUP_PROCESS_MSG

DESCRIPTION
  This function processes messages for LUP.

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
extern void hdrlup_process_msg 
( 
  dsm_item_type * msg_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*===========================================================================

FUNCTION HDRLUP_PROCESS_SIG

DESCRIPTION
  This function processes signals for LUP.  

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
extern void hdrlup_process_sig (rex_sigs_type sig);

#endif /* HDRLUPI_H */
