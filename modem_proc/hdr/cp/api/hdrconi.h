#ifndef HDRCONI_H
#define HDRCONI_H

/*===========================================================================

                C O N N E C T E D   S T A T E   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the different files of CON.

Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrconi.h_v   1.13   05 Sep 2001 09:14:28   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrconi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/18/01   sh      Added the data field to hdrcon_cmd_type
01/13/01   dna     Removed CLOSE_MSG_SENT command and updated unpack macros.
07/28/00   nmn     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrcon.h"
#include "bit.h"
#include "hdrbit.h"

/* <EJECT> */ 
/*===========================================================================

                                CON COMMANDS

===========================================================================*/

/* Command type */
/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRCON_ACTIVATE_CMD=0,            /* Connection is Active */
  HDRCON_DEACTIVATE_CMD=1,          /* Deactivate the Protocol */
  HDRCON_CLOSE_CONNECTION_CMD=2,    /* Close the Connection */
  HDRCON_INIT_CMD=3,                /* Initialize */
  HDRCON_RESEND_CON_CLOSE_CMD=4     /* Resend Connection Close */
} hdrcon_cmd_name_enum_type;

typedef struct
{
  hdrcon_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, for debug */

  union {
    hdrhai_abort_reason_enum_type    abort_reason;
  } data;

} hdrcon_cmd_type;

/* <EJECT> */ 
/*===========================================================================

                                CON PROCESS FUNCTIONS

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_CMD

DESCRIPTION
  This function processes commands given to CON.

DEPENDENCIES
  None

PARAMETERS
  con_cmd_ptr - Pointer to the command for CON to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcon_process_cmd
(
  hdrcon_cmd_type * con_cmd_ptr
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_IND

DESCRIPTION
  This function processes indications given to CON.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for CON to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcon_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_MSG

DESCRIPTION
  This function processes messages for CON.

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
void hdrcon_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

#endif               /* HDRCONI_H */
