#ifndef HDRSTREAMI_H
#define HDRSTREAMI_H

/*===========================================================================

                S T R E A M   C O N F I G U R AT I O N 
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the different files of STREAM.

Copyright (c) 2000,2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrstreami.h_v   1.3   05 Sep 2001 09:14:50   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrstreami.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
04/12/01   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrstream.h"
#include "bit.h"
#include "dsmbit.h"

/*---------------------------------------------------------------------------
                               STREAM TIMERS

                     Configuration Response Timer

---------------------------------------------------------------------------*/


/* <EJECT> */ 
/*===========================================================================

                                STREAM COMMANDS

===========================================================================*/

/* Command type */
typedef enum
{
  HDRSTREAM_CONFIGURE_CMD  /* Configure Command */
} hdrstream_cmd_name_enum_type;

typedef struct
{
  hdrstream_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, for debug */
//  hdrstream_cmd_params_union_type params; /* Common command fields */
} hdrstream_cmd_type;

/* <EJECT> */ 
/*===========================================================================

                                STREAM PROCESS FUNCTIONS

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_CMD

DESCRIPTION
  This function processes commands given to STREAM.

DEPENDENCIES
  None

PARAMETERS
  stream_cmd_ptr - Pointer to the command for STREAM to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrstream_process_cmd
(
  hdrstream_cmd_type * stream_cmd_ptr
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_IND

DESCRIPTION
  This function processes indications given to STREAM.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for STREAM to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrstream_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_PROCESS_MSG

DESCRIPTION
  This function processes messages for STREAM.

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
void hdrstream_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSTREAM_PROCESS_SIG

DESCRIPTION
  This function processes signals for STREAM.
  
DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input 
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrstream_process_sig
( 
  rex_sigs_type sig 
);

#endif               /* HDRSTREAMI_H */
