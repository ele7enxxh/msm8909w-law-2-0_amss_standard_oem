#ifndef HDRBUF_H
#define HDRBUF_H

/*===========================================================================

            H D R    B U F F E R    S E R V I C E S   H E A D E R  

DESCRIPTION
  This file contains common data types and external declarations for common
  functions used by tasks when queueing messages, commands, and indications 
  for the protocols that run in their context.

Copyright (c) 2000 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/hdrbuf.h_v   1.8   17 Jul 2002 19:55:48   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrbuf.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     -------------------------------------------------------
10/24/11   smd     Moved more to SU API header files.
10/27/09   wsh     Fixed compiler warning for ARM compiler
09/17/09   etv     Force hdrbuf_item_enum_type to uint32 type.
07/17/02   dna     Queue timer expiration events on hdrmc.event_q because timer
                   callback runs as a separate thread and causes problems.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
02/02/00   dna     Created Module

===========================================================================*/

#include "hdrhai.h"
#include "queue.h"
#include "dsm.h"
#include "hdrind.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  HDRBUF_COMMAND, /* The item is a command */
  HDRBUF_INDICATION, /* The item is an indication */
  HDRBUF_MESSAGE, /* The item is a message (DSM item) */
  HDRBUF_TIMER, /* The item is a timer expiration event */
  HDRBUF_SIGNAL, /* For debug purposes only */
  HDRBUF_FUNCTION,/* For debug purposes only */
  HDRBUF_RESERVED = 0x7FFFFFFF /* To force type to int32 */
} hdrbuf_item_enum_type;
/* Command header type.
*/

typedef struct
  {
    q_link_type link;          /* Queue link */
    q_type *done_q_ptr;          /* Queue to place buffer on when done */
    hdrhai_protocol_name_enum_type protocol_name; /* Who the command is for */
    hdrhai_protocol_instance_enum_type protocol_instance;/* in-use:in-config */
    /* What type of item is this header for */
    hdrbuf_item_enum_type type_of_item; 
  }
hdrbuf_hdr_type;


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBUF_DONE

DESCRIPTION
  This function returns the buffer to its free queue, if necessary.

DEPENDENCIES
  None
  
PARAMETERS
  hdr_ptr - pointer to the header of the buffer to be returned

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrbuf_done( hdrbuf_hdr_type * hdr_ptr );

/*===========================================================================

FUNCTION HDRBUF_COPY_IND_DATA

DESCRIPTION
  This function copies the data passed along with an indication into the
  general union for indication data.

DEPENDENCIES
  None
  
PARAMETERS
  ind_data_dest_ptr - Global union for indication data 
  ind_name - Global enumeration of indications 
  ind_data_src_ptr - Pointer to data given with indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrbuf_copy_ind_data
(
  hdrind_ind_data_union_type * ind_data_dest_ptr,
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_src_ptr
);

#endif               /* HDRBUF_H */
