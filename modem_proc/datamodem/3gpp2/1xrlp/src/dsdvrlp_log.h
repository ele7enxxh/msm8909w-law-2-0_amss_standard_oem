
#ifndef _DSDVRLP_LOG_H
#define _DSDVRLP_LOG_H
/*===========================================================================

       D S R L P    E V D V    L O G G I N G     H E A D E R    F I L E

DESCRIPTION
  
  This header file contains all the extern definitions related to RLP
  logging for EVDV. Any callers of these functions need to include this 
  header file.
 
EXTERNALIZED FUNCTIONS
 
  dsdvrlp_log_rx_int_details()
    Logs the current values of Rx RLP internal details to the off-target 
    diagnostic tool.

  dsdvrlp_log_tx_int_details()
    Logs the current values of Tx RLP internal details to the off-target
    diagnostic tool.

  dsdvrlp_log_rx_init_pdch_pdu5_frm_buf()
    Initializes the buffer for logging Rx PDCH PDU5 frames.

  dsdvrlp_log_tx_init_pdch_pdu5_frm_buf()
    Initializes the buffer for logging Tx PDCH PDU5 frames.

  dsdvrlp_log_rx_init_int_details_buf()
    Initializes the buffer for logging Rx internal details.

  dsdvrlp_log_tx_init_int_details_buf()
    Initializes the buffer for logging Tx internal details.

  dsdvrlp_log_rx_init_pdch_pdu5_entry()
    Initializes the next PDCH PDU5 entry, and the pointer to it,
    in the RLP Rx PDCH PDU5 frames log buffer.

  dsdvrlp_log_tx_init_pdch_pdu5_entry()
    Initializes the next PDCH PDU5 entry, and the pointer to it,
    in the RLP Tx PDCH PDU5 frames log buffer.

  dsdvrlp_log_rx_init_int_details_entry()
    Initializes the next Rx internal details entry, and the pointer to it,
    in the RLP Rx internal details log buffer.

  dsdvrlp_log_tx_init_int_details_entry()
    Initializes the next Tx internal details entry, and the pointer to it,
    in the RLP Tx internal details log buffer.

  dsdvrlp_log_rx_send_pdch_pdu5_log()
    Terminate and send the accumulated RLP Rx PDCH PDU5 partial frames log.

  dsdvrlp_log_rx_send_pdch_pdu5_log()
    Terminate and send the accumulated RLP Tx PDCH PDU5 partial frames log.

  dsdvrlp_log_rx_send_int_details_log()
    Terminate and send the accumulated RLP Rx internal details log.

  dsdvrlp_log_tx_send_int_details_log()
    Terminate and send the accumulated RLP Tx internal details log.
  
Copyright (c) 2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlp_log.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------
02/04/11   op    Data SU API cleanup
07/22/04   atp   After lint.
06/08/04   atp   Support for upper bound on DDW and REXMIT_TIMER.
05/07/04   atp   Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#if defined( FEATURE_RLP_LOGGING )
#include "customer.h"
#include "diagpkt.h"

#include "dsrlp_v.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                       RLP PDCH PARTIAL MUXPDU5 FRAMES LOG TYPE
---------------------------------------------------------------------------*/
#define LOG_RLP_PDCH_PDU5_NUM_BYTES_PAYLOAD 7

typedef PACKED struct PACKED_POST
{
  byte payload[LOG_RLP_PDCH_PDU5_NUM_BYTES_PAYLOAD];
                             /* First 7 bytes of MuxPDU5 (hdr)             */

  byte time_delta;           /* 0 if same frame time as previous frame     */
                             /* 1 if 1.25ms after previous frame           */
                             /* 2 if 2.5 ms after previous frame           */
                             /* ...
                                16 if 20 ms after previous frame           */


} log_rlp_pdch_pdu5_entry_type;

/* LOG_RLP_RX_PDCH_PARTIAL_MUXPDU5_FRAMES_C,
   LOG_RLP_TX_PDCH_PARTIAL_MUXPDU5_FRAMES_C */
#define LOG_MAX_NUM_RLP_PDCH_PDU5_ENTRIES  40
typedef PACKED struct PACKED_POST
{
  log_hdr_type                 hdr;

  uint32                       count; /* Count of entries in foll array    */

  log_rlp_pdch_pdu5_entry_type entry[LOG_MAX_NUM_RLP_PDCH_PDU5_ENTRIES];

} log_rlp_pdch_pdu5_frames_type;

typedef log_rlp_pdch_pdu5_frames_type
               LOG_RLP_RX_PDCH_PARTIAL_MUXPDU5_FRAMES_C_type;
typedef log_rlp_pdch_pdu5_frames_type
               LOG_RLP_TX_PDCH_PARTIAL_MUXPDU5_FRAMES_C_type;

/*---------------------------------------------------------------------------
                       RLP RX INTERNAL DETAILS LOG TYPE
---------------------------------------------------------------------------*/
#define LOG_MAX_NUM_RLP_RX_INT_DET_ENTRIES 5

typedef PACKED struct PACKED_POST
{
  byte   rsvd;               /* Reserved for future use                    */
  byte   sr_id;              /* Service Id                                 */
  byte   ddw;                /* Delay Detection Window                     */
  byte   rexmit;             /* Rexmit time betw Naks                      */
  uint32 vn;                 /* Needed sequence number                     */
  uint32 vr;                 /* Expected sequence number                   */
  uint16 q_len_fwd_in;       /* Length of forward input queue              */
  uint16 q_len_pre_q;        /* Length of forward pre queue                */
  uint16 q_len_rsq;          /* Length of resequencing queue               */
  uint16 misc;               /* Misc info                                  */

} log_rlp_rx_int_det_entry_type;

/* LOG_RLP_RX_INTERNAL_DETAILS_C  */
typedef PACKED struct PACKED_POST
{
  log_hdr_type                  hdr;

  uint32                        count; /* Count of entries in foll array   */

  log_rlp_rx_int_det_entry_type entry[LOG_MAX_NUM_RLP_RX_INT_DET_ENTRIES];

} log_rlp_rx_int_details_type;

typedef log_rlp_rx_int_details_type LOG_RLP_RX_INTERNAL_DETAILS_C_type;

/*---------------------------------------------------------------------------
                       RLP TX INTERNAL DETAILS LOG TYPE
---------------------------------------------------------------------------*/
#define LOG_MAX_NUM_RLP_TX_INT_DET_ENTRIES 5

typedef PACKED struct PACKED_POST
{
  byte rsvd;                 /* Reserved for future use                    */
  byte sr_id;                /* Service Id                                 */
  uint16 q_len_txq;          /* Length of txq                              */
  uint32 num_bytes_tx_wmk;   /* Number of bytes in tx watermark            */
  uint32 vs;                 /* Next sequence number                       */
  uint32 pvn;                /* Peer's expected sequence number            */

} log_rlp_tx_int_det_entry_type;

/* LOG_RLP_TX_INTERNAL_DETAILS_C  */
typedef PACKED struct PACKED_POST
{
  log_hdr_type                  hdr;

  uint32                        count; /* Count of entries in foll array   */

  log_rlp_tx_int_det_entry_type entry[LOG_MAX_NUM_RLP_TX_INT_DET_ENTRIES];

} log_rlp_tx_int_details_type;

typedef log_rlp_tx_int_details_type LOG_RLP_TX_INTERNAL_DETAILS_C_type;

/*---------------------------------------------------------------------------
  Buffers for EVDV RLP logging
---------------------------------------------------------------------------*/
extern log_rlp_pdch_pdu5_frames_type dsdvrlp_log_rx_pdch_pdu5_frm_buf;
extern log_rlp_pdch_pdu5_frames_type dsdvrlp_log_tx_pdch_pdu5_frm_buf;
extern log_rlp_rx_int_details_type   dsdvrlp_log_rx_int_details_buf;
extern log_rlp_tx_int_details_type   dsdvrlp_log_tx_int_details_buf;


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSDVRLP_LOG_RX_INT_DETAILS

DESCRIPTION
  Logs the current values of Rx RLP internal details to the off-target
  diagnostic tool.  

  Input Parameters:
    sr_id: The Service ID for the particular service. 

DEPENDENCIES
  Logging mask

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsdvrlp_log_rx_int_details
( 
  byte sr_id_index 
);

/*===========================================================================

FUNCTION DSDVRLP_LOG_TX_INT_DETAILS

DESCRIPTION
  Logs the current values of Rx RLP internal details to the off-target
  diagnostic tool.  

  Input Parameters:
    sr_id: The Service ID for the particular service. 

DEPENDENCIES
  Logging mask 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsdvrlp_log_tx_int_details
(
  byte sr_id_index
);

/*===========================================================================

FUNCTION DSDVRLP_LOG_RX_INIT_PDCH_PDU5_FRM_BUF

DESCRIPTION
  Initializes the buffer for logging Rx PDCH PDU5 frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsdvrlp_log_rx_init_pdch_pdu5_frm_buf (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_TX_INIT_PDCH_PDU5_FRM_BUF

DESCRIPTION
  Initializes the buffer for logging Tx PDCH PDU5 frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsdvrlp_log_tx_init_pdch_pdu5_frm_buf (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_RX_INIT_INT_DETAILS_BUF

DESCRIPTION
  Initializes the buffer for logging Rx internal details.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsdvrlp_log_rx_init_int_details_buf (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_TX_INIT_INT_DETAILS_BUF

DESCRIPTION
  Initializes the buffer for logging Tx internal details.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsdvrlp_log_tx_init_int_details_buf (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_RX_INIT_PDCH_PDU5_ENTRY

DESCRIPTION
  Initializes the next PDCH PDU5 frame, and the pointer to it,
  in the RLP Rx PDCH PDU5 frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to log entry.

SIDE EFFECTS
  An RLP Rx PDCH PDU5 log may be sent.

===========================================================================*/
extern log_rlp_pdch_pdu5_entry_type * dsdvrlp_log_rx_init_pdch_pdu5_entry
                                      (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_TX_INIT_PDCH_PDU5_ENTRY

DESCRIPTION
  Initializes the next PDCH PDU5 frame, and the pointer to it,
  in the RLP Tx PDCH PDU5 frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to log entry.

SIDE EFFECTS
  An RLP Tx PDCH PDU5 log may be sent.

===========================================================================*/
extern log_rlp_pdch_pdu5_entry_type * dsdvrlp_log_tx_init_pdch_pdu5_entry
                                      (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_RX_INIT_INT_DETAILS_ENTRY

DESCRIPTION
  Initializes the next Rx internal details entry, and the pointer to it,
  in the RLP Rx internal details log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to log entry.

SIDE EFFECTS
  An RLP Rx internal details log may be sent.

===========================================================================*/
extern log_rlp_rx_int_det_entry_type * dsdvrlp_log_rx_init_int_details_entry
                                      (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_TX_INIT_INT_DETAILS_ENTRY

DESCRIPTION
  Initializes the next Tx internal details entry, and the pointer to it,
  in the RLP Tx internal details log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to log entry.

SIDE EFFECTS
  An RLP Tx internal details log may be sent.

===========================================================================*/
extern log_rlp_tx_int_det_entry_type * dsdvrlp_log_tx_init_int_details_entry
                                       (void);


/*===========================================================================

FUNCTION DSDVRLP_LOG_RX_SEND_PDCH_PDU5_LOG

DESCRIPTION
  Terminate and send the accumulated RLP Rx PDCH PDU5 partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A log may be sent.

===========================================================================*/
extern void dsdvrlp_log_rx_send_pdch_pdu5_log(void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_TX_SEND_PDCH_PDU5_LOG

DESCRIPTION
  Terminate and send the accumulated RLP Tx PDCH PDU5 partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A log may be sent.

===========================================================================*/
extern void dsdvrlp_log_tx_send_pdch_pdu5_log (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_RX_SEND_INT_DETAILS_LOG

DESCRIPTION
  Terminate and send the accumulated RLP Rx internal details log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A log may be sent.

===========================================================================*/
extern void dsdvrlp_log_rx_send_int_details_log (void);

/*===========================================================================

FUNCTION DSDVRLP_LOG_TX_SEND_INT_DETAILS_LOG

DESCRIPTION
  Terminate and send the accumulated RLP Tx internal details log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A log may be sent.

===========================================================================*/
extern void dsdvrlp_log_tx_send_int_details_log (void);

#endif /* FEATURE_RLP_LOGGING */
#endif /* _DSDVRLP_LOG_H */
