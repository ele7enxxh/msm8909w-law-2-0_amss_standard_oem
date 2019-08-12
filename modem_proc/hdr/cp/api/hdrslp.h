#ifndef HDRSLP_H
#define HDRSLP_H
/*===========================================================================

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L
                          D E F I N I T I O N S

GENERAL DESCRIPTION
  The Signaling Layer Protocol (SLP) is used as transport layer protocol 
  for HDR signaling messages. It provides reliable and best-effort
  transport as well as fragmentation.
  When queuing a message for transmission, the user can supply a callback
  function to be notified about the outcome of the transmission.

EXTERNALIZED FUNCTIONS

  hdrslp_init()       - One time initialization of all SLP data structures
  hdrslp_msg_cb()     - Callback registered with HMP to deliver messages
  hdrslp_ind_cb()     - Callback registered for delivery of indications
  hdrslp_send_msg()   - Used to enqueue msgs for transmission
  hdrslp_abort_msg()  - Used to abort rexmits of reliable msgs
  hdrslp_get_common_ac_pkt() - Used by lower layers to get AC spec. sign.
  hdrslp_get_pkt()    - Used by lower layers to get sign. data
  hdrslp_put_pkt()    - Used by lower layers to deliver recvd. sign. data

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrslp_init() has to be called before any messages are enqueued for
  transmission.

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM4500/vcs/hdrslp.h_v   1.5   10 Nov 2000 09:24:20   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrslp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/13   wsh     Added crit_sect to guard SLP data structures
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
09/23/11   smd     SU API cleanup.
04/17/08   sju     Added support to bundle AC messages 
11/08/07   sju     Added support to allow AHO before the first access attempt
                   when there is no DO session
07/11/05   dna     Add time_since_last_req to hdrslp_get_tx_queue_size
07/11/05   pba     Restrict route update message size in case it is too large
                   and does not allow sending other messages.
07/09/05   hal     Added hdrslp_get_tx_queue_size().
06/22/05   pba     No fragmentation of DOS message on access channel.
02/22/00    om     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "rex.h"

#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"
#include "hdrrup.h"
#include "hdrcp_api.h"

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM HAI Vol. III

===========================================================================*/


/*--------------------------------------------------------------------------
                               SLP COMMANDS
--------------------------------------------------------------------------*/

/*** This protocol does not define any commands or indications ***/


/********** Route update function type **********/
typedef dsm_item_type* (*hdrslp_route_update_gen_type)
                           ( hdrrup_rup_msg_build_type rup_msg_build_type, 
                             uint16 max_rup_msg_size );

/********** Route update function pointer **********/
/* This needs to be set to the appropr. func in rup_init() */
extern hdrslp_route_update_gen_type hdrslp_route_update_gen_func;

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSLP_INIT

DESCRIPTION
  This function does all one-time initialization.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_init(void);

/*===========================================================================

FUNCTION HDRSLP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to SLP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRSLP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever HMP receives a
  message for SLP.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_msg_cb
(
  dsm_item_type* msg_ptr
);

/*===========================================================================

FUNCTION HDRSLP_PROCESS_SIG

DESCRIPTION
  This function is the signal handler for SLP. The only signal for
  SLP is the callback processing signal HDRTX_SLP_CB_SIG.

DEPENDENCIES
  None
 
PARAMETERS
  signal - REX signal
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_sig
(
  rex_sigs_type signal
);

/*=============================================================================

FUNCTION HDRSLP_TX_QUEUE_IS_EMPTY

DESCRIPTION
   This function returns TRUE if the slp tx queue is empty, otherwise 
   it returns FALSE.

DEPENDENCIES
  None.
  
PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - If the slp tx queue is empty
  FALSE - otherwise.
  
SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrslp_tx_queue_is_empty (hdrhai_protocol_name_enum_type caller);

/*=============================================================================

FUNCTION HDRSLP_GET_TX_QUEUE_SIZE

DESCRIPTION
   This function returns the total size of the messages in the SLP tx queue,
   including an estimate of the size the SLP headers will be.

DEPENDENCIES
  None.

PARAMETERS
  msg_count_ptr - Output parameter; Number of messages in the queue
  time_since_prev_req - PN roll interrupts since last call

RETURN VALUE
  Number of bytes queued in SLP for transmit, including SLP headers.

SIDE EFFECTS
  None.
=============================================================================*/
extern uint32 hdrslp_get_tx_queue_size
( 
  uint32 *msg_count_ptr,
  uint16 time_since_prev_req
);

/* <EJECT> */ 
/*--------------------------------------------------------------------------
------------------         Transmit Functions        -----------------------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_SEND_MESSAGE

DESCRIPTION
  This function enqueues a signaling message onto SLP's TX queue
  by its priority.

  NOTE: This function runs in the context of the protocol sending
        the message. 

DEPENDENCIES
  None

PARAMETERS
  channel         - Channel SDU has to be transmitted on
  reliable        - Set to true if reliable delivery transport is required
  priority        - Priority of the message (0 -> 255, 0 highest)
  msg_ptr         - dsm item chain containing the signaling message
  slp_outcome_cb  - Function to be called after message has been
                    transmitted successfully or transmission was aborted
  outcome_tag_ptr - User data for callback function

RETURN VALUE
  Enqueueing status (E_SUCCESS, E_PROT_Q_FULL)

SIDE EFFECTS

===========================================================================*/

extern hdrerrno_enum_type hdrslp_send_msg
( 
  hdrhai_channel_enum_type      channel,
  boolean                    reliable,
  uint8                      priority,
  dsm_item_type*             msg_ptr,
  hdrslp_outcome_cb_type     slp_outcome_cb,
  void*                      outcome_tag_ptr 
);

/*===========================================================================

FUNCTION HDRSLP_ABORT_MESSAGE

DESCRIPTION
  This function is used to prevent a reliable delivery message to be
  retransmitted if the upper layers know that the message was already
  received, but the layer 2 acknowledgement got lost.

  NOTE: This function runs in the context of the protocol sending
        the message. 

DEPENDENCIES
  None

PARAMETERS
  slp_outcome_cb  - Function to be called after message has been
                    transmitted successfully or transmission was aborted
  outcome_tag_ptr - User data for callback function

RETURN VALUE
  Aborting status (E_SUCCESS, E_NOT_AVAILABLE)

SIDE EFFECTS

===========================================================================*/

extern hdrerrno_enum_type hdrslp_abort_msg
( 
  hdrslp_outcome_cb_type     slp_outcome_cb,
  void*                      outcome_tag_ptr 
);

/*===========================================================================

FUNCTION HDRSLP_GET_COMMON_AC_PKT

DESCRIPTION
  This function causes SLP to create an SDU of up to max_bytes length
  from registered common access channel data (data to be transmitted
  with every access channel probe).

DEPENDENCIES
  None

PARAMETERS
  max_bytes            - Maximum number of bytes current TX frame can 
                         accommodate  
  time_since_prev_req  - Number of TX build interrupts since last call
  msg_ptr              - Will be set to pointer to dsm_item chain 
                         containing the common AC SDU
  channel_ptr          - Will be set to AC channel

RETURN VALUE
  Number of bytes in SDU

SIDE EFFECTS

===========================================================================*/

extern uint16 hdrslp_get_common_ac_pkt
(
  uint16                  max_bytes,
  uint16                  time_since_prev_req,
  dsm_item_type**         msg_ptr,
  hdrhai_channel_enum_type*  channel_ptr
);

/*===========================================================================

FUNCTION HDRSLP_GET_PKT

DESCRIPTION
  This function causes SLP to create an SDU of up to max_bytes length
  from pending TX data.

DEPENDENCIES
  None

PARAMETERS
  max_bytes            - Maximum number of bytes current TX frame can 
                         accommodate
  time_since_prev_req  - Number of TX build interrupts since last call
  msg_ptr              - Will be set to pointer to dsm_item chain 
                         containing the SDU
  channel_ptr          - Will be set to channel SDU is destined for

RETURN VALUE
  Number of bytes in SDU

SIDE EFFECTS

===========================================================================*/

extern uint16 hdrslp_get_pkt
(
  boolean                first_callfor_slpdata,
  uint16                 max_bytes,
  uint16                 time_since_prev_req,
  dsm_item_type**        msg_ptr,
  hdrhai_channel_enum_type* channel_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSLP_FIRST_ACCESS_ATTEMPTED

DESCRIPTION
  This function checks if the AT has started an access attempt since entering 
  idle state when there is no DO session. 
  
DEPENDENCIES
  To call this function, user must also check DO session is not open

PARAMETERS
  None

RETURN VALUE
  TRUE - If AT has started access attempt since entering idle state 

SIDE EFFECTS
===========================================================================*/

extern boolean hdrslp_first_access_attempted(void);

/* <EJECT> */
/*--------------------------------------------------------------------------
-----------------          Receive Functions          ----------------------
--------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSLP_PUT_PKT

DESCRIPTION
  This function is used by PCP to deliver received SDUs to SLP
  for further processing.

DEPENDENCIES
  None

PARAMETERS
  received_pkt_ptr - dsm item chain containing the received SDU

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrslp_put_pkt
(
  dsm_item_type* received_pkt_ptr
);

/*===========================================================================

FUNCTION HDRSLP_SET_WAITING_FOR_BUNDLING_FLAG

DESCRIPTION
  This function is used to set hdrslp_waiting_for_bundling flag.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern void hdrslp_set_waiting_for_bundling_flag
( 
  boolean waiting_for_bundling
);
/*===========================================================================

FUNCTION HDRSLP_IS_WAITING_FOR_BUNDLING

DESCRIPTION
  This function is called to check if we are still waiting for bundling.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern boolean hdrslp_is_waiting_for_bundling(void);

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
/*===========================================================================

FUNCTION HDRSLP_STOP_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  This function is called when AMP no longer expects a UATIAssignment message.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern void hdrslp_stop_waiting_for_uati_assignment(void);

/*===========================================================================

FUNCTION HDRSLP_IS_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  This function is called when AMP no longer expects a UATIAssignment message.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern boolean hdrslp_is_waiting_for_uati_assignment(void);

#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

/*===========================================================================
FUNCTION HDRSLP_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the SLP verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrslp_set_verbose_debug 
( 
  uint8 verbose_debug_setting 
);

/*===========================================================================

FUNCTION HDRSLP_ENTER_CRIT_SECT

DESCRIPTION
  This enters SLP critcal section to protect SLP data struct
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void hdrslp_enter_crit_sect( void );

/*===========================================================================

FUNCTION HDRSLP_LEAVE_CRIT_SECT

DESCRIPTION
  This leaves SLP critcal section
  
DEPENDENCIES
  Must be called after enter_crit_sect and must be matched in pairs.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void hdrslp_leave_crit_sect( void );
#endif /* HDRSLP_H */

