#ifndef HDRBCTASK_H
#define HDRBCTASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   B R O A D C A S T   P R O T O C O L    T A S K

GENERAL DESCRIPTION
  The HDR Broadcast Protocol task runs the following protocols:
  - Broadcast Content Mac Protocol
  - Broadcast Security Protocol
  - Broadcast Framing Protocol
      
EXTERNALIZED FUNCTIONS
  hdrbc_task
    Entry point to the HDR Broadcast Protocol task.
    

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/inc/hdrbctask.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
09/30/05   mpa     Added hdrbcmac_queue_cmd().
                   Moved flow enable/disable commands to hdrbcframe.
08/28/05   dna     (mpa for) Added hdrbctask_queue_ind().
02/02/05   etv     Fixed coding style to match convention.
08/16/04   kss     Moved flow tracking functions to hdrbcmac.
07/06/04   kss     Added access functions for logging info.
06/30/04   kss     Use dsm_items in data_ready function.
06/07/04   kss     Added enable/disable flow command processing.
04/24/04   kss     Ported from QC Corp.

===========================================================================*/

/* <EJECT> */

#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "rcinit.h"
#include "hdrlogi.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrbcframe.h"
#include "hdrbcmac.h"

#ifndef FEATURE_CMI
#include "hdrtask.h"
#endif
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Address of the TCB for the HDRBC task */
extern rex_tcb_type *hdrbc_tcb_ptr;

/* Pointer to HDRBC task TCB */
#define HDRBC_TASK_PTR hdrbc_tcb_ptr

/*---------------------------------------------------------------------------
                             Signal Masks
---------------------------------------------------------------------------*/
#define  HDRBC_RPT_TIMER_SIG       0x0001    /* Sig for watchdog timer   */
#define  HDRBC_EVENT_Q_SIG         0x0002    /* Sig for event queue      */
#define  HDRBC_MAC_PKTS_SIG        0x0004    /* Sig for pkts ready       */
#define  HDRBC_SCHEDULE_SLOTS_SIG  0x0008    /* Sig for scheduling slots */
#define  HDRBC_OVHD_UPDATE_SIG     0x0010    /* Sig for scheduling slots */
#define  HDRBC_ENTER_HDR_SIG       0x0040    /* Sig for HDR entry        */
#define  HDRBC_EXIT_HDR_SIG        0x0080    /* Sig for HDR exit         */

typedef union
{
  hdrbcframe_cmd_union_type bcframe;
  hdrbcmac_cmd_union_type bcmac;
} hdrbctask_cmd_union_type;

/*===========================================================================

FUNCTION HDRBCTASK_QUEUE_MAC_PKT

DESCRIPTION
  This function places a dsm_item (containing a Broadcast MAC packet) on 
  the broadcast queue for handling by the HDRBCMAC in HDRBC task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbctask_queue_mac_pkt
( 
   dsm_item_type* item_ptr
);

/*===========================================================================

FUNCTION HDRBCTASK_QUEUE_IND

DESCRIPTION
  This function queues an indication on the hdrbctask_event_q and sets the
  signal.  If there is any data associated with the indication, it is copied
  into the buffer before it is queued.  This function is called in the task
  context of whoever is giving the indication.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  an indication to protocols in the BC task at the same time.

  ind_data_size must not be larger than hdrind_ind_data_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrbctask_queue_ind
( 
  hdrhai_protocol_name_enum_type protocol_name,
  hdrind_ind_name_enum_type ind_name,
  void *input_ind_data_ptr
);

/*===========================================================================

FUNCTION HDRBCTASK_QUEUE_CMD

DESCRIPTION
  This function queues a command on the hdrbctask_event_q and sets the signal.
  The command data is copied into the command buffer before it is queued.  
  This function is called in the task context of whoever is giving the command.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  commands to protocols in the RX task at the same time.

  cmd_size must not be larger than hdrbctask_protocol_cmd_type, because 
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the command is for
  input_cmd_ptr - Pointer to a union of all protocols in HDRRX subtask
  cmd_size - Size of the command being given

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbctask_queue_cmd
( 
  hdrhai_protocol_name_enum_type protocol_name,
  void                        *input_cmd_ptr, 
  uint32                       cmd_size 
);

#endif /* HDRBCTASK_H */
