#ifndef _HDRRLPI_H
#define _HDRRLPI_H
/*===========================================================================

             H D R   R L P   I N T E R N A L   H E A D E R  F I L E
                   
DESCRIPTION
   This file contains internal variable and function declarations for the 
   High Data Rate (HDR) Radio Link Protocol.

Copyright (c) 2000-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/hdrrlpi.h_v   1.4   07 Aug 2002 11:38:04   vramaswa  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlpi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
02/08/11   ls      Global variable cleanup
05/14/08   spn     MMPA: Fixed Logging in case of OoO Transmissions
03/30/07   ac      EMPA: Implemented RLP inactivity timer feature. Let every 
				   RLP instance maintain its own inactivity timer.
01/09/07   ac      EMPA: Added the hdrrlp_flush_giveup_timer_cb()
12/04/05   ksu     EMPA segment sequencing: don't fragment for Format B
11/03/05   ksu     EMPA: packet stream, octet sequencing (reverse link only)
10/11/05   ksu     Eliminate hdrmrlpi_rx_process_nak()
09/30/05   ksu     Eliminate hdrmrlp_get_new_or_flush_data() and hdrmrlp_get_retx_data()
09/28/05   ksu     Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb()
07/29/05   sy      Added support for Rev A with FEATURE_HDR_QOS.
07/11/05   tc      Added DO Rev A RTCMAC support for RLP.
09/09/04   vas     Added dsrlptxq.h include
09/12/03   vas     Changed prototype of hdrrlpi_rx_process_nak() 
08/07/02   vr      Added define HDRRLPI_MAX_NAK_LEN   
02/15/02   hcg     Added RLP failsafe timer value in msec.
02/29/00   hcg     Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "dsrlpi.h"
#include "hdrrlptxq.h"
#include "hdrrlprtx.h"
#include "dsrlprtx.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define HDRRLPI_MSG_ID_SIZE   8
#define HDRRLPI_UNPACK_MSG_ID( ptr, val_ptr )  \
           HDRBIT_UNPACK8( ptr, val_ptr, 0,  HDRRLPI_MSG_ID_SIZE )

/*---------------------------------------------------------------------------
  Maximum number of NAK records allowed.  This is based on the maximum HMP
  message size of 255 octets, limited by SLP header Len field.  The NAK 
  message has 1 byte of message ID, 1 byte of number of NAK requests, and 5 
  bytes of NAK record.  Given this, a maximum number of 50 records can be
  supported in one single NAK message.
---------------------------------------------------------------------------*/
#define HDRRLPI_MAX_NAK_RECORDS 50

/*---------------------------------------------------------------------------
  Maximum number of MLNak records allowed. This is based on the maximum
  number of Nak records that can fit into a SMALL DSM item (128 bytes)
---------------------------------------------------------------------------*/
#define HDRRLPI_MAX_MULTILINK_NAK_RECORDS  10

/*---------------------------------------------------------------------------
  bits in a byte.  Used in figuring out first set of padding in a control
  frame.
---------------------------------------------------------------------------*/
#define   BITS_IN_OCTET        (sizeof(byte) * 8)


/*---------------------------------------------------------------------------
  HDR RLP Failsafe Reset Timer

    Used to protect against potential RLP deadlock situation in which the
    reliable RST/RST ACK signaling messages get dropped.  Reset procedures
    should not take longer than 1.5 seconds.
---------------------------------------------------------------------------*/
#define HDRRLPI_RESET_TIMER_VAL_MSEC 1500                    /* time in ms */

/*---------------------------------------------------------------------------
  Struct used in retrieve rexmit information.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32       nak_first_seq;     /* first seq number nak'd                */
  uint32       nak_last_seq;      /* last seq number nak'd                 */
  word         data_payload_len;  /* length of payload data in dsm chain   */
} hdrrlpi_rexmit_info_type;

/*---------------------------------------------------------------------------
  Struct used as handling while sending new data to PCP
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_item_type           *head_item_ptr;
  uint16                   payload_size;
  uint16                   ID;
  uint32                   set_id; /*This is used to hold info about the combo, carrier,
                           marq_id and marq_index*/
} hdrrlpi_new_data_pkt_handle_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION      GET_HDRRLP_PKT_INFO_FREE_Q_PTR

DESCRIPTION   Accessor function for hdrrlp_pkt_info_free_q
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_pkt_info_free_q

SIDE EFFECTS  None
===========================================================================*/
q_type* get_hdrrlp_pkt_info_free_q_ptr 
(
  void
);
/*===========================================================================

FUNCTION HDRRLPI_BUILD_NAK_MSG

DESCRIPTION
  This function is called as part of queuing signaling messages for transmit
  from RLP to SLP.  This function builds up NAK signaling messages based
  on entries in the NAK list.  The function also combines contiguous NAK 
  list entries to form a single NAK request record.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value that indicates if more NAK messages are to be generated.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrlpi_build_nak_msg
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,  /* rlp-specific control block        */
  dsm_item_type            *item_ptr   /* ptr to sig msg to be built        */
);

/*===========================================================================

FUNCTION HDRRLPI_BUILD_MULTILINK_NAK_MSG

DESCRIPTION
  This function is called as part of queuing signaling messages for transmit
  from RLP to SLP.  This function builds up MultiLink NAK signaling messages 
  based on entries in the Multilink NAK list.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value that indicates if more NAK messages are to be generated.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrlpi_build_multilink_nak_msg
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,  /* rlp-specific control block       */
  dsm_item_type            *item_ptr,  /* ptr to sig msg to be built       */
  void                    **quick_repeat_ptr_ptr /* ptr-ptr to quick_repeat_entry */
);

/*===========================================================================

FUNCTION HDRRLPI_TX_REXMIT

DESCRIPTION
  This function is called as part of building RLP data packets for transmit.
  It is used to build up a packet for retransmission by looking into the
  retransmit list.

  The size of the RLP packet is dependent on the  maximum number of bytes 
  that is passed in as a parameter.  If the packet to retransmit is larger
  than the allowed size, then the packet is split.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet that is built, including headers.  A zero-length value
  indicates that the packet could not be found.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlpi_tx_rexmit
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,      /* ptr to RLP specific ctl blk  */
  dsm_item_type            **rev_pkt_ptr,  /* ptr to pkt to build          */
  hdrrlprtx_pkt_handle_type   *handle_ptr,    /* ptr to handle that shd accompany data */
  uint16                   max_bytes,        /* max size allowed pk to build */         
  hdrrlprtx_reason_enum_type* reason,
  uint8                    combo_index
);

/*===========================================================================

FUNCTION HDRRLPI_TX_NEW_DATA

DESCRIPTION
  This function is called as part of building RLP data packets for transmit.
  It is used to build up a new data packet for transmission from the 
  available data in the transmit watermark.

  The size of the RLP packet is dependent on the  maximum number of bytes 
  that is passed in as a parameter.  

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet that is built, including headers.  A zero-length value
  indicates that no data was available for transmit.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlpi_tx_new_data
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,      /* ptr to RLP specific ctl blk  */
  dsm_item_type            **rev_pkt_ptr,  /* ptr to pkt to build          */
  hdrrlpi_new_data_pkt_handle_type *handle_ptr,    /* ptr to handle that   *
                                                    * shd accompany data   */
  uint16                   max_bytes,      /* max size allowed pk to build */         
  uint8                    combo_index
);

/*===========================================================================

FUNCTION HDRRLPI_TX_LAST_DATA_UNIT

DESCRIPTION
  This function is called as part of building RLP data packets for transmit.
  If the RLP flush timer has expired, indicating that no data has been sent
  for a specified amount of time, RLP will send a packet containing the
  last data unit of the last packet that was sent.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet that is built, including headers.  A zero-length value
  indicates that the packet could not be found.

SIDE EFFECTS
  The **rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlpi_tx_last_data_unit
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,      /* ptr to RLP specific ctl blk  */
  dsm_item_type            **rev_pkt_ptr,  /* ptr to pkt to build          */
  uint16                   max_bytes       /* max size allowed pk to build */         
);

/*===========================================================================

FUNCTION HDRRLP_RX_PROCESS_SIG_MSG

DESCRIPTION
  Processes the messages on the RLP receive signaling queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlpi_rx_process_sig_msg
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* RLP-specific ctl block    */
  dsrlpi_state_info_type   *state_ptr,        /* local state ptr           */
  dsm_item_type            *msg_ptr           /* ptr to the message        */
);

/*===========================================================================

FUNCTION HDRRLPI_RX_PROCESS_NAK

DESCRIPTION
  This function is called as part of processing received RLP signalling 
  messages and processes NAK messages.
  
  The NAK signalling messages parses the number of NAK requests, and then
  for each NAK request, parses the FirstErased and WindowLen fields.

  Each NAK request will generate entries into the retransmit list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Items will be queued on to the retransmit list.
===========================================================================*/
void hdrrlpi_rx_process_nak
(  
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* RLP-specific ctl block    */
  dsm_item_type            **nak_msg_ptr   /* ptr to received NAK message  */
);

/*===========================================================================

FUNCTION       HDRRLPI_GET_REXMIT_DATA

DESCRIPTION    This function gets the information on the packet to be
               re-transmitted.
               
DEPENDENCIES   This function assumes RLP is in DSRLPI_ESTABLISHED_STATE.
               This function should not be called if RLP cannot transmit new
               data packets.

RETURN VALUE   TRUE if a packet to rexmit has been found.  FALSE otherwise.

SIDE EFFECTS   NONE
===========================================================================*/
boolean hdrrlpi_get_rexmit_data
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  hdrrlprtx_type               **rexmit_info, /* holds rexmit info          */
  uint16                       *data_len,
  uint8                        combo_index
);

/*===========================================================================

FUNCTION HDRRLPI_RESET_PROC

DESCRIPTION
  Invokes HDR RLP Reset Procedures.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Invokes mobile-initated reset procedures.

===========================================================================*/
void hdrrlpi_reset_proc
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr          /* RLP-specific ctl block    */
);
/*===========================================================================

FUNCTION HDRRLP_FLUSH_TIMER_CB

DESCRIPTION
    This callback function called whenever the flush timer expires.
    The function sets the RLP session's boolean flush_timer_expired variable
    to TRUE, indicating that the next Tx interrupt will generate an RLP
    flush packet.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A flush packet may be generated at the next Tx build-frame interrupt.

===========================================================================*/
void hdrrlp_flush_timer_cb
(
  uint32 param                         /*  RLP control block               */
);

/*===========================================================================

FUNCTION HDRRLP_ENHANCED_FLUSH_TIMER_CB

DESCRIPTION
    This callback function called whenever the flush timer expires.
    The function sets the RLP session's boolean flush_timer_expired variable
    to TRUE, indicating that the next Tx interrupt will generate an RLP
    flush packet.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A flush packet may be generated at the next Tx build-frame interrupt.

===========================================================================*/
void hdrrlp_enhanced_flush_timer_cb
(
  uint32 param                         /*  RLP control block               */
);

/*===========================================================================

FUNCTION HDRRLP_FLUSH_GIVEUP_TIMER_CB

DESCRIPTION
    This callback function called whenever the flush giveup timer expires.
    The function sets the RLP session's boolean flush_giveup_timer_expired variable
    to TRUE, indicating that the next Tx interrupt will not generate an RLP
    flush packet.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A flush packet may be generated at the next Tx build-frame interrupt.

===========================================================================*/
void hdrrlp_flush_giveup_timer_cb
(
  uint32 param                         /*  RLP control block               */
);

/*===========================================================================

FUNCTION HDRRLP_INACTIVITY_TIMER_CB

DESCRIPTION
    This callback function called when the inactivity timer expires.The function 
	call the API to set the corresponding bit in the 64-bit global variable, 
	hdrrlp_enhanced_dormant_bitmap to 0. 

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The HDR data call could go to dormant.

===========================================================================*/
void hdrrlp_inactivity_timer_cb
(
  uint32 param                         /*  RLP control block               */
);
/*===========================================================================

FUNCTION HDRRLP_RESET_TIMER_CB

DESCRIPTION
    This callback function called whenever the reset timer expires.
    If the timer expired, it indicates that the RLP instance has been in the
    RESET state for a duration greater than HDRRLP_MAX_RESET_TIME.  At this
    time, the connection is torn down as a fail-safe mechanism to prevent a
    potential RLP deadlock situation.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The connection is released.

===========================================================================*/
void hdrrlp_reset_timer_cb
(
  uint32 param                                       /* RLP control block  */
);

/*===========================================================================

                      DO_REV_A_RLP PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION       HDRRLPI_NEW_DATA_PACKET_STATUS

DESCRIPTION    Update packet status of the new data packet transmission.
               TRUE if the packet was accepted.
               FALSE if the packet was cancelled.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
hdrrlptxq_type* hdrrlpi_new_data_packet_status
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,       /* current RLP ctl blk         */
  hdrrlpi_new_data_pkt_handle_type *handle, /* handle to determine packet  */
  boolean                  status,           /* packet status               */
  uint8                    combo_index,
  boolean                  retry_again      /* To denote if its a FALSE 
                                                RETRANSMISSION */
);

/*===========================================================================

FUNCTION       HDRRLPI_FLUSH_DATA_PACKET_STATUS

DESCRIPTION    Update packet status of the flush data packet transmission.
               TRUE if the packet was accepted.
               FALSE if the packet was cancelled.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpi_flush_data_packet_status
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,       /* current RLP ctl blk         */
  hdrrlpi_new_data_pkt_handle_type *handle,  
  boolean                  status,           /* packet status               */
  uint8                    combo_index
);


/*===========================================================================
FUNCTION HDRRLPI_GET_NEW_DATA_COUNT

DESCRIPTION
  Returns the size of the new data based on RLP version
 
PARAMETERS
  rrcb_ptr:   RLP Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The new data count

SIDE EFFECTS
  None.
===========================================================================*/
int hdrrlpi_get_new_data_count
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
);

/*===========================================================================
FUNCTION HDRRLPI_MOBILE_INITIATED_RESET

DESCRIPTION
  Mobile initiated reset based on RLP version
 
PARAMETERS
  rrcb_ptr:   RLP Control Block pointer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlpi_mobile_initiated_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
);

/*===========================================================================
FUNCTION HDRMRLPI_EXTRACT_RLP_HEADER

DESCRIPTION
  This function extracts the RLP header out of the received forward link pkt
  
DEPENDENCIES
  None.

RETURN VALUE
  hdrcom_errno_enum_type
  E_SUCCESS - If header parsing is successful. Other errors if not.

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrrlpi_extract_rlp_header
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,       /* RLP-specific ctl block          */
  dsm_item_type           **fwd_pkt_ptr,    /* ptr to received RLP data packet */
  dsrlpi_header_type      *rlp_header_ptr
);

/*===========================================================================
FUNCTION HDRRLPI_NUM_DATA_UNITS

DESCRIPTION
  Computes the number of data units based on RLP version and config
PARAMETERS
  rscb_ptr:  RLP Session Control Block pointer
  num_bytes: number of bytes of payload

DEPENDENCIES
  None.

RETURN VALUE
  The computed number of data units

SIDE EFFECTS
  None.
===========================================================================*/
uint16 HDRRLPI_NUM_DATA_UNITS
(
 const dsrlpi_cfg_info_type *cfg_ptr,
 uint16 num_bytes
);

/*===========================================================================
FUNCTION HDRRLPI_UPDATE_COMBO_INDEX

DESCRIPTION
  Updates the combo index for which current packet building is taking place
  
PARAMETERS
  combo_id: The current combo id for which packet building is being
            requested

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates the 
===========================================================================*/
void hdrrlpi_update_combo_index
( 
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  uint8 combo_id 
);

/*===========================================================================

FUNCTION HDRRLPI_RESET_RXBUFFER

DESCRIPTION
  This function resets the buffered_rx_q and returns all elements to the
  hdrrlp_pkt_info_free_q.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlpi_reset_rxbuffer
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr       /* RLP-specific ctl block         */
);
#endif  /* _HDRRLPI_H */
