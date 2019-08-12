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

Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlp_log.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------
01/17/05   atp   Added include of msg.h and err.h.
07/22/04   atp   After lint.
06/08/04   atp   Support for upper bound on DDW and REXMIT_TIMER.
05/07/04   atp   Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "data_msg.h"

#if defined(FEATURE_RLP_LOGGING)
#include "diagbuf.h"

#include "ds707.h"
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsdvrlp.h"
#include "dsdvrlp_log.h"

#include "time_svc.h"
#include "time_tod.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Buffers for EVDV RLP logging
---------------------------------------------------------------------------*/
log_rlp_pdch_pdu5_frames_type dsdvrlp_log_rx_pdch_pdu5_frm_buf;
log_rlp_pdch_pdu5_frames_type dsdvrlp_log_tx_pdch_pdu5_frm_buf;
log_rlp_rx_int_details_type   dsdvrlp_log_rx_int_details_buf;
log_rlp_tx_int_details_type   dsdvrlp_log_tx_int_details_buf;



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
void dsdvrlp_log_rx_int_details
( 
  byte sr_id_index 
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type        *rrcb_ptr;  /* rlp-specific control block */
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  log_rlp_rx_int_det_entry_type *entry_ptr=NULL; /* Pointer to entry in log*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  entry_ptr = dsdvrlp_log_rx_init_int_details_entry();
  if ( entry_ptr == NULL )
  {
    return;
  }
  
  rscb_ptr  = &( dsrlpi_rscb_array [ sr_id_index ]);

  // 1x has only one route
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr  = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  state_ptr = &rrcb_ptr->state;

  entry_ptr->rsvd         = 0;
  entry_ptr->sr_id        = sr_id_index;

  /*lint -save -e734 */
  entry_ptr->ddw          =
     ( state_ptr->fwd_ddw <= DSDVRLP_MAX_DDW_SUPPORTED )
     ? (state_ptr->fwd_ddw)/20  : (DSDVRLP_MAX_DDW_SUPPORTED)/20;

  entry_ptr->rexmit       =
     ( state_ptr->fwd_rexmit_time <= DSDVRLP_MAX_REXMIT_SUPPORTED )
     ? (state_ptr->fwd_rexmit_time)/20  : (DSDVRLP_MAX_REXMIT_SUPPORTED)/20;

  entry_ptr->vn           = state_ptr->needed_seq_num;
  entry_ptr->vr           = state_ptr->expected_seq_num;
  entry_ptr->q_len_fwd_in = q_cnt ( &( state_ptr->fwd_input_q ) );
  entry_ptr->q_len_pre_q  = q_cnt ( &( state_ptr->fwd_pre_q   ) );
  entry_ptr->q_len_rsq    = q_cnt ( &( state_ptr->reseq_q     ) );
  entry_ptr->misc         = 0;
  /*lint -restore */
  
  /*-------------------------------------------------------------------------
    Update length and count fields.
  -------------------------------------------------------------------------*/
  dsdvrlp_log_rx_int_details_buf.hdr.len +=
          sizeof ( log_rlp_rx_int_det_entry_type );
  dsdvrlp_log_rx_int_details_buf.count++;
  
} /* dsdvrlp_log_rx_int_details() */


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
void dsdvrlp_log_tx_int_details
(
  byte sr_id_index
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type        *rrcb_ptr;  /* rlp-specific control block */
  dsrlpi_state_info_type  *state_ptr=NULL;   /* local state ptr            */
  log_rlp_tx_int_det_entry_type *entry_ptr=NULL; /* Pointer to entry in log*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  entry_ptr = dsdvrlp_log_tx_init_int_details_entry();
  if ( entry_ptr == NULL )
  {
    return;
  }

  rscb_ptr  = &( dsrlpi_rscb_array [ sr_id_index ]);

  // 1x has only one route
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr  = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  state_ptr = &rrcb_ptr->state;

  entry_ptr->rsvd             = 0;
  entry_ptr->sr_id            = sr_id_index;
/*lint -save -e734*/
  entry_ptr->q_len_txq        = q_cnt ( & ( state_ptr->tx_q ) );
/*lint -restore */
  entry_ptr->num_bytes_tx_wmk = rrcb_ptr->cfg_ptr->tx_wm_ptr->current_cnt;
  entry_ptr->vs               = state_ptr->next_seq_num;
  entry_ptr->pvn              = state_ptr->peer_vn;

  /*-------------------------------------------------------------------------
    Update length and count fields.
  -------------------------------------------------------------------------*/
  dsdvrlp_log_tx_int_details_buf.hdr.len +=
          sizeof ( log_rlp_tx_int_det_entry_type );
  dsdvrlp_log_tx_int_details_buf.count++;

} /* dsdvrlp_log_tx_int_details() */





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
void dsdvrlp_log_rx_init_pdch_pdu5_frm_buf (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsdvrlp_log_rx_pdch_pdu5_frm_buf.hdr.code =
          LOG_RLP_RX_PDCH_PARTIAL_MUXPDU5_FRAMES_C;
/*lint -save -e507, -e734 */
  dsdvrlp_log_rx_pdch_pdu5_frm_buf.hdr.len  =
          FPOS ( log_rlp_pdch_pdu5_frames_type, entry );
/*lint -restore */
  dsdvrlp_log_rx_pdch_pdu5_frm_buf.count = 0;
} /* dsdvrlp_log_rx_init_pdch_pdu5_frm_buf() */


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
void dsdvrlp_log_tx_init_pdch_pdu5_frm_buf (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsdvrlp_log_tx_pdch_pdu5_frm_buf.hdr.code =
          LOG_RLP_TX_PDCH_PARTIAL_MUXPDU5_FRAMES_C; 
/*lint -save -e507, -e734 */
  dsdvrlp_log_tx_pdch_pdu5_frm_buf.hdr.len  = 
          FPOS ( log_rlp_pdch_pdu5_frames_type, entry );
/*lint -restore */
  dsdvrlp_log_tx_pdch_pdu5_frm_buf.count = 0;
} /* dsdvrlp_log_tx_init_pdch_pdu5_frm_buf() */


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
void dsdvrlp_log_rx_init_int_details_buf (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsdvrlp_log_rx_int_details_buf.hdr.code =
          LOG_RLP_RX_INTERNAL_DETAILS_C;
/*lint -save -e507, -e734 */
  dsdvrlp_log_rx_int_details_buf.hdr.len  = 
          FPOS ( log_rlp_rx_int_details_type, entry );
/*lint -restore */
  dsdvrlp_log_rx_int_details_buf.count = 0;
} /* dsdvrlp_log_rx_init_int_details_buf() */


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
void dsdvrlp_log_tx_init_int_details_buf (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsdvrlp_log_tx_int_details_buf.hdr.code =
          LOG_RLP_TX_INTERNAL_DETAILS_C;
/*lint -save -e507, -e734 */
  dsdvrlp_log_tx_int_details_buf.hdr.len  = 
          FPOS ( log_rlp_tx_int_details_type, entry );
/*lint -restore */
  dsdvrlp_log_tx_int_details_buf.count = 0;
} /* dsdvrlp_log_tx_init_int_details_buf() */







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
log_rlp_pdch_pdu5_entry_type * dsdvrlp_log_rx_init_pdch_pdu5_entry
                                      (void)
{
  log_rlp_pdch_pdu5_entry_type *frm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If logging is enabled.
  -------------------------------------------------------------------------*/
  if (log_status (dsdvrlp_log_rx_pdch_pdu5_frm_buf.hdr.code))
  {
    if ( dsdvrlp_log_rx_pdch_pdu5_frm_buf.count >= 
         LOG_MAX_NUM_RLP_PDCH_PDU5_ENTRIES ) 
    {
      dsdvrlp_log_rx_send_pdch_pdu5_log();
      (void)time_get((unsigned long *) dsdvrlp_log_rx_pdch_pdu5_frm_buf.hdr.ts);
    }

    frm_ptr = (log_rlp_pdch_pdu5_entry_type *)
    &dsdvrlp_log_rx_pdch_pdu5_frm_buf.entry [dsdvrlp_log_rx_pdch_pdu5_frm_buf.count];

  }
  
  return frm_ptr;
} /* dsdvrlp_log_rx_init_pdch_pdu5_entry() */


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
log_rlp_pdch_pdu5_entry_type * dsdvrlp_log_tx_init_pdch_pdu5_entry
                                      (void)
{
  log_rlp_pdch_pdu5_entry_type *frm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If logging is enabled.
  -------------------------------------------------------------------------*/
  if (log_status (dsdvrlp_log_tx_pdch_pdu5_frm_buf.hdr.code))
  {
    if ( dsdvrlp_log_tx_pdch_pdu5_frm_buf.count >=
         LOG_MAX_NUM_RLP_PDCH_PDU5_ENTRIES )
    {
      dsdvrlp_log_tx_send_pdch_pdu5_log();
      (void)time_get((unsigned long *) dsdvrlp_log_tx_pdch_pdu5_frm_buf.hdr.ts);
    }

    frm_ptr = (log_rlp_pdch_pdu5_entry_type *)
    &dsdvrlp_log_tx_pdch_pdu5_frm_buf.entry [dsdvrlp_log_tx_pdch_pdu5_frm_buf.count];

  }

  return frm_ptr;
} /* dsdvrlp_log_tx_init_pdch_pdu5_entry() */


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
log_rlp_rx_int_det_entry_type * dsdvrlp_log_rx_init_int_details_entry
                                      (void)
{
  log_rlp_rx_int_det_entry_type *frm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If logging is enabled.
  -------------------------------------------------------------------------*/
  if (log_status (dsdvrlp_log_rx_int_details_buf.hdr.code))
  {
    if ( dsdvrlp_log_rx_int_details_buf.count >=
         LOG_MAX_NUM_RLP_RX_INT_DET_ENTRIES )
    {
      dsdvrlp_log_rx_send_int_details_log();
      (void)time_get((unsigned long *) dsdvrlp_log_rx_int_details_buf.hdr.ts);
    }

    frm_ptr = (log_rlp_rx_int_det_entry_type *)
    &dsdvrlp_log_rx_int_details_buf.entry [dsdvrlp_log_rx_int_details_buf.count];

  }

  return frm_ptr;
} /* dsdvrlp_log_rx_init_int_details_entry() */


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
log_rlp_tx_int_det_entry_type * dsdvrlp_log_tx_init_int_details_entry
                                       (void)
{
  log_rlp_tx_int_det_entry_type *frm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If logging is enabled.
  -------------------------------------------------------------------------*/
  if (log_status (dsdvrlp_log_tx_int_details_buf.hdr.code))
  {
    if ( dsdvrlp_log_tx_int_details_buf.count >=
         LOG_MAX_NUM_RLP_TX_INT_DET_ENTRIES )
    {
      dsdvrlp_log_tx_send_int_details_log();
      (void)time_get((unsigned long *) dsdvrlp_log_tx_int_details_buf.hdr.ts);
    }

    frm_ptr = (log_rlp_tx_int_det_entry_type *)
    &dsdvrlp_log_tx_int_details_buf.entry [dsdvrlp_log_tx_int_details_buf.count];
  }

  return frm_ptr;
} /* dsdvrlp_log_tx_init_int_details_entry() */


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
void dsdvrlp_log_rx_send_pdch_pdu5_log (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Only send log if there is data in the buffer
  -------------------------------------------------------------------------*/
/*lint -save -e507 */
  if ( dsdvrlp_log_rx_pdch_pdu5_frm_buf.hdr.len > 
       FPOS ( log_rlp_pdch_pdu5_frames_type, entry ) )
/*lint -restore */
  {
    if (( log_submit((log_type *) &dsdvrlp_log_rx_pdch_pdu5_frm_buf ))
                == FALSE )
    {
      
    }

    /*-----------------------------------------------------------------------
      Re-initialize 
    -----------------------------------------------------------------------*/
    dsdvrlp_log_rx_init_pdch_pdu5_frm_buf();
  }
} /* dsdvrlp_log_rx_send_pdch_pdu5_log() */


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
void dsdvrlp_log_tx_send_pdch_pdu5_log (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Only send log if there is data in the buffer
  -------------------------------------------------------------------------*/
/*lint -save -e507 */
  if ( dsdvrlp_log_tx_pdch_pdu5_frm_buf.hdr.len >
       FPOS ( log_rlp_pdch_pdu5_frames_type, entry ) )
/*lint -restore */
  {
    if (( log_submit((log_type *) &dsdvrlp_log_tx_pdch_pdu5_frm_buf ))
                == FALSE )
    {
      
    }

    /*-----------------------------------------------------------------------
      Re-initialize 
    -----------------------------------------------------------------------*/
    dsdvrlp_log_tx_init_pdch_pdu5_frm_buf();
  }
} /* dsdvrlp_log_tx_send_pdch_pdu5_log() */


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
void dsdvrlp_log_rx_send_int_details_log (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Only send log if there is data in the buffer
  -------------------------------------------------------------------------*/
/*lint -save -e507 */
  if ( dsdvrlp_log_rx_int_details_buf.hdr.len >
       FPOS ( log_rlp_rx_int_details_type, entry ) )
/*lint -restore */
  {
    if (( log_submit((log_type *) &dsdvrlp_log_rx_int_details_buf ))
                == FALSE )
    {
      
    }

    /*-----------------------------------------------------------------------
      Re-initialize 
    -----------------------------------------------------------------------*/
    dsdvrlp_log_rx_init_int_details_buf(); 
  }

} /* dsdvrlp_log_rx_send_int_details_log() */


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
void dsdvrlp_log_tx_send_int_details_log (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Only send log if there is data in the buffer
  -------------------------------------------------------------------------*/
/*lint -save -e507 */
  if ( dsdvrlp_log_tx_int_details_buf.hdr.len >
       FPOS ( log_rlp_tx_int_details_type, entry ) )
/*lint -restore */
  {
    if (( log_submit((log_type *) &dsdvrlp_log_tx_int_details_buf ))
                == FALSE )
    {
    
    }

    /*-----------------------------------------------------------------------
      Re-initialize 
    -----------------------------------------------------------------------*/
    dsdvrlp_log_tx_init_int_details_buf(); 
  }
} /* dsdvrlp_log_tx_send_int_details_log() */

#endif /* FEATURE_RLP_LOGGING */
