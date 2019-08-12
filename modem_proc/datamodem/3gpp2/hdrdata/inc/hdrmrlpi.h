#ifndef HDRMRLPI_H
#define HDRMRLPI_H

/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Multiflow Radio Link Protocol.

Copyright (c) 2004 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
08/16/11   vpk     Move hdr log packet definitions to hdrdata
02/27/11   ls      Global variable removal
09/01/06   spn     Updated the MRLP Log Packet, to reflect the sequence
                   length, and the RLP ID Length.
10/11/05   ksu/sy  Eliminate hdrmrlpi_rx_process_nak()
10/10/05   ksu/sy  Eliminate hdrmrlpi_reset_proc()
07/29/05   sy      Moved HDRMRLPI_HEADER_SIZE to hdrmrlp.h
01/31/05   sy      Removed HDRMRLP_NAK_ABORT_TIMER_VALUE as we already have
                   HDRRLP_NAK_ABORT_TIMER_VALUE for the same purpose.
11/06/03   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_HDR_QOS
#include "errno.h"
#include "hdrmrlp.h"
#include "dsrlpi.h"
#include "hdrrlplog.h"


/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*---------------------------------------------------------------------------
  MESSAGE: NAK
---------------------------------------------------------------------------*/
#define HDRMRLPI_RLP_FLOW_SIZE 8
#define HDRMRLPI_NAK_REQUESTS_SIZE 8
#define HDRMRLPI_NAK_WINDOW_LEN_SIZE 16

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================
FUNCTION      GET_HDRMRLP_CONSTANTS_PTR

DESCRIPTION   Accessor function for hdrmrlp_constants

DEPENDENCIES  None

RETURN VALUE  Address of hdrmrlp_constants

SIDE EFFECTS  None
===========================================================================*/
dsrlpi_ver_const_type* get_hdrmrlp_constants_ptr
(
  void
);

/*===========================================================================
FUNCTION      GET_HDRMRLP_SN_ACB_PTR

DESCRIPTION   Accessor function for hdrmrlp_sn_acb

DEPENDENCIES  None

RETURN VALUE  Address of hdrmrlp_sn_acb

SIDE EFFECTS  None
===========================================================================*/
hdrmrlp_app_ctl_blk_type* get_hdrmrlp_sn_acb_ptr
(
  void
);

/*===========================================================================

FUNCTION HDRMRLPI_GET_RSCB_INDEX_FOR_RX_PKT

DESCRIPTION
  This function determines which RLP a received data packet belongs to. It
  does this by extracting the prefix free RLP ID bytes in the data packet
  and determining the RLP ID that they represent.

PARAMETERS

DEPENDENCIES

RETURN VALUE
  The rscb index of the RLP that the packets are meant for.
  DSRLPI_INVALID_RSCB_INDEX - If no matching RLP is found.

SIDE EFFECTS
===========================================================================*/
uint8 hdrmrlpi_get_rscb_index_for_rx_pkt
(
  dsm_item_type *fwd_pkt_ptr
);

/*===========================================================================
FUNCTION HDRMRLPI_EXTRACT_RLP_HEADER

DESCRIPTION
  This function extracts the RLP header out of the received forward link pkt
  for multiflow packet apps

DEPENDENCIES
  None.

RETURN VALUE
  hdrcom_errno_enum_type
  E_SUCCESS - If header parsing is successful. Other errors if not.
  It also fills up the rrcb_ptr_ptr and rlp_header_ptr parameters.

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpi_extract_rlp_header
(
  dsm_item_type           **fwd_pkt_ptr_ptr,    /* ptr to received RLP data packet */
  dsrlpi_rlp_ctl_blk_type **rrcb_ptr_ptr,  /* returns RLP-specific ctl block  */
  dsrlpi_header_type      *rlp_header_ptr  /* rlp_header to be filled up      */
);

/*===========================================================================

FUNCTION HDRMRLPI_UPCONVERT_SAR_SEQ_TO_LONG

DESCRIPTION
  This function converts the SAR Sequence number from SARSequenceLengthShort
  bits to SARSequenceLength bits based on the following interpretation
  interval around V(r).

  [V(r) - 2^(SARSequenceLengthShort-1) + 1, ... , V(r) + 2^(SARSequenceLengthShort-1)]

  The SARSequence is set to the SARSequenceLength-bit value in the interpretation
  interval around V(r), whose SARSequenceLengthShort least significant bits match
  the value of sar_seq_short received in the SAR packet header.

DEPENDENCIES
  None.

RETURN VALUE
  uint32
  SARSequence number after unconverting to SARSequenceLength bits

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrmrlpi_upconvert_sar_seq_to_long
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,       /* RLP-specific ctl block         */
  uint32                   sar_seq_short   /* SAR seq number rcvd in fwd_pkt */
);

/*===========================================================================

FUNCTION HDRMRLPI_GET_NEXT_AVAIL_QN_INDEX

DESCRIPTION
  This function returns the next avaialable QN instance that is not activated
  and returns the index into the rrcb_ptr->qn_instance_table for the same.

DEPENDENCIES
  None.

RETURN VALUE
  uint8
  Index of the available QN instance for use

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrmrlpi_get_next_avail_qn_index
(
  void
);

/*===========================================================================

FUNCTION HDRMRLP_LOG_INIT_TIMER

DESCRIPTION
    This function called to initialize the timer function

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrmrlp_log_init_timer(void );

/*===========================================================================
FUNCTION      HDRMRLP_LOG_INIT

DESCRIPTION   This function initializes a rlp flow

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrmrlp_log_init
(
  uint8 rlp_flow,
  dsrlp_ver_enum_type ver
);

/*===========================================================================
FUNCTION      HDRMRLP_ENHANCED_LOG_INIT

DESCRIPTION   This function initializes the log buffer for an EMPA rlp flow

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrmrlp_enhanced_log_init
(
  uint8 rlp_flow,
  dsrlp_ver_enum_type ver
);
#endif /* FEATURE_HDR_QOS */
#endif /* HDRMRLPI_H */
