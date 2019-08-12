/*===========================================================================

                D S R L P  S T A T S  M O D U L E    

DESCRIPTION
  
  This file contains all the functions  related to RLP logging.  Any
  callers of these functions need to include the corresponding header file. 
  
EXTERNALIZED FUNCTIONS

  dsrlp_clear_rlp_statistics()
    Clears the current values of the RLP statistics on command
    from the off-target diagnostic tool.

  dsrlp_log_rlp_statistics()
    Logs the current values of the RLP statistics to the off-target
    diagnostic tool.

  dsrlp_log_rx_init_frm_buf()
    Inits the dsrlp_log_rx_frm_buf data structure.

  dsrlp_log_tx_init_frm_buf()
    Inits the dsrlp_log_tx_frm_buf data structure.

  dsrlp_log_rx_init_frame()
    Initializes the next frame packet, as well as the pointer to it,
    in the RLP Rx frames log buffer.

  dsrlp_log_tx_init_frame()
    Initializes the next frame packet, as well as the pointer to it,
    in the RLP Tx frames log buffer.

  dsrlp_log_rx_send_frames()
    Send the accumulated RLP Rx partial log.

  dsrlp_log_tx_send_frames()
    Send the accumulated RLP Tx partial log.

  ds_log_rlp_frames_type *dsrlp_get_log_rx_rlp_frm_buf()
    Accessor method for dsrlp_log_rx_rlp_frm_buf

  ds_log_rlp_frames_type *dsrlp_get_log_tx_rlp_frm_buf()
    Accessor method for dsrlp_log_tx_rlp_frm_buf


Copyright (c) 2003 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlp_stats.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/12   ash     Aligned RLP stats global variables to 4 byte boundary.
02/04/11   op      Data SU API cleanup
01/18/11   op      Migrated to MSG 2.0 Macros
06/17/10   ms      Rename log_rlp_* to ds_log_rlp_*
02/27/09   spn     Fixed Lint Error
05/10/07   ac      Lint fix
03/20/07   ac      Fix the compiling error when HDR is turned off
07/25/06   ksu     EMPA: 1x RLP code review comments
05/30/06   ksu     EMPA: RLP init code review comments
01/17/05   atp     Added include of msg.h and err.h.
05/07/04   atp     Initial version.

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
#include "dsrlp_stats.h"
#include "data_msg.h"

#if defined(FEATURE_RLP_LOGGING)
#include "diagbuf.h"

#include "ds707.h"
#include "dsrlp_v.h"
#include "dsrsp.h"

#include "time_svc.h"
#include "time_tod.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Buffer for RLP frames logging 
---------------------------------------------------------------------------*/
static ds_log_rlp_frames_type dsrlp_log_rx_rlp_frm_buf ALIGN(4); //__attribute__ ((aligned(4)));
static ds_log_rlp_frames_type dsrlp_log_tx_rlp_frm_buf ALIGN(4); //__attribute__ ((aligned(4)));

/*---------------------------------------------------------------------------
  How often do we log RLP statistics? (in # of frames)  Setting this to 
  zero disables periodic logging--stats will only be logged at the end  
  of a call.
---------------------------------------------------------------------------*/
word  dsrlp_log_stats_interval = 25;
static word  dsrlp_log_stats_frm_cnt    = 0;

#endif

/*===========================================================================
                     SU Level API's
                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSRLP_CLEAR_RLP_STATISTICS

DESCRIPTION
  Clears the current values of the RLP statistics on command from the
  off-target diagnostic tool.

  Input Parameters:
    *req_ptr: Pointer to DIAG request packet
    pkt_len: Length of DIAG request packet

DEPENDENCIES
  Logging mask, dsrlp_log_stats_interval 

RETURN VALUE
  Pointer to DIAG response packet

SIDE EFFECTS
  None
===========================================================================*/
diag_rsp_type * dsrlp_clear_rlp_statistics(
  DIAG_RLP_STAT_RESET_F_req_type *req_ptr,
  word                            pkt_len
)
{
#ifdef FEATURE_RLP_LOGGING
  dsrlp_stats_info_type          *rlp_stats_ptr = NULL;
  DIAG_RLP_STAT_RESET_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof( DIAG_RLP_STAT_RESET_F_rsp_type );
  word                            i; /* loop invariant */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dsrlp_log_stats_frm_cnt = 0;

  /*-------------------------------------------------------------------------
    Get a pointer to the stats block for this sr_id 
    NOTE: Although, it says req_ptr->sr_id, what is actually expected
    from QXDM is the index in the rscb_array, ie: RLP instance        
    -------------------------------------------------------------------------*/
  (void) dsrlp_get_stats_ex( req_ptr->sr_id, DSRSP_ROUTE_A, &rlp_stats_ptr );

  if( rlp_stats_ptr != NULL ) 
  {
    /*-----------------------------------------------------------------------
      Erase it all         
      -----------------------------------------------------------------------*/
    (void)time_get(( rlp_stats_ptr->reset_timestamp ));
    rlp_stats_ptr->rx_rexmit_fr_cnt = 0;
    rlp_stats_ptr->rx_idle_fr_cnt = 0;
    rlp_stats_ptr->rx_fill_fr_cnt = 0;
    rlp_stats_ptr->rx_blank_fr_cnt = 0;
    rlp_stats_ptr->rx_null_fr_cnt = 0;
    rlp_stats_ptr->rx_new_data_fr_cnt = 0;
    rlp_stats_ptr->rx_20ms_fr_cnt = 0;
    rlp_stats_ptr->rx_total_bytes = 0;
    rlp_stats_ptr->rx_rlp_erasures = 0;
    rlp_stats_ptr->rx_mux_erasures = 0;

    rlp_stats_ptr->tx_rexmit_fr_cnt = 0;
    rlp_stats_ptr->tx_idle_fr_cnt = 0;
    rlp_stats_ptr->tx_new_data_fr_cnt = 0;
    rlp_stats_ptr->tx_20ms_fr_cnt = 0;
    rlp_stats_ptr->tx_total_bytes = 0;
    rlp_stats_ptr->curr_tx_nak_rounds = DSRLP_MAX_NAK_ROUNDS;
    for( i = 0; i < DSRLP_MAX_NAK_ROUNDS; i++ ) 
    {
      rlp_stats_ptr->tx_naks[i] = 0;
    }

    rlp_stats_ptr->naks_received = 0;
    rlp_stats_ptr->re_xmits_not_found = 0;
    rlp_stats_ptr->largest_contig_erasure = 0;
    rlp_stats_ptr->resets = 0;
    rlp_stats_ptr->aborts = 0;
    rlp_stats_ptr->last_rtt = 0;
    rlp_stats_ptr->RLP_BLOB_used = DSRLP_BLOB_NOT_USED;
  }

  if( rlp_stats_ptr != NULL ) 
  {
    rsp_ptr = (DIAG_RLP_STAT_RESET_F_rsp_type *) diagbuf_pkt_alloc( rsp_len );
    if( rsp_ptr != NULL ) 
    {
      rsp_ptr->cmd_code = DIAG_RLP_STAT_RESET_F;
    }
  } 
  else 
  {
    rsp_ptr = (DIAG_RLP_STAT_RESET_F_rsp_type *) ERR_PKT( DIAG_BAD_PARM_F );
  }
  return((diag_rsp_type *) rsp_ptr );

#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                "dsrlp_clear_rlp_statistics() not supported");
  return NULL;
#endif

}/* dsrlp_clear_rlp_statistics() */


/*===========================================================================

FUNCTION DSRLP_LOG_RLP_STATISTICS

DESCRIPTION
  Logs the current values of the RLP statistics to the off-target diagnostic
  tool.  If we can't allocate space for the log packet on the DIAG ring
  buffer, we'll abort and try again next time.

  Input Parameters:
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.
    
    callTerminated: True if we're logging at call termination, False if
           we're logging periodically during the call.

DEPENDENCIES
  Logging mask, dsrlp_log_stats_interval 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_log_rlp_statistics( byte sr_id_index, boolean callTerminated ) 
{
#ifdef FEATURE_RLP_LOGGING
  ds_log_rlp_stats_type   *rlp_stats_log_pkt = NULL;
  dsrlp_stats_info_type   *rlp_stats_ptr = NULL;
  word                     i; /* loop invariant */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Should we log RLP statistics yet?     
  -------------------------------------------------------------------------*/
  if( log_status( LOG_RLP_STAT_C ) && ( dsrlp_log_stats_interval > 0 ) &&
      ( ++dsrlp_log_stats_frm_cnt >= dsrlp_log_stats_interval )) {
    
    /*-----------------------------------------------------------------------
      We'll try...   
    -----------------------------------------------------------------------*/
    rlp_stats_log_pkt = (ds_log_rlp_stats_type*) log_alloc( LOG_RLP_STAT_C,
                                                 sizeof( ds_log_rlp_stats_type ));
    if( rlp_stats_log_pkt != NULL ) {

       (void) dsrlp_get_stats_ex( sr_id_index,
                       DSRSP_ROUTE_A,
                       &rlp_stats_ptr );

      rlp_stats_log_pkt->service_ID = dsrlp_get_sr_id(sr_id_index);
      qw_equ(rlp_stats_log_pkt->reset_time, rlp_stats_ptr->reset_timestamp);
      rlp_stats_log_pkt->naks_received = rlp_stats_ptr->naks_received;
      rlp_stats_log_pkt->re_xmits_not_found = rlp_stats_ptr->re_xmits_not_found;
      rlp_stats_log_pkt->largest_contig_erasure = rlp_stats_ptr->largest_contig_erasure;
      rlp_stats_log_pkt->resets = rlp_stats_ptr->resets;
      rlp_stats_log_pkt->aborts = rlp_stats_ptr->aborts;
      rlp_stats_log_pkt->last_rtt = rlp_stats_ptr->last_rtt;
      rlp_stats_log_pkt->rlp_blob_used = rlp_stats_ptr->RLP_BLOB_used;

      rlp_stats_log_pkt->rx_rexmit_fr_cnt   = rlp_stats_ptr->rx_rexmit_fr_cnt;
      rlp_stats_log_pkt->rx_idle_fr_cnt     = rlp_stats_ptr->rx_idle_fr_cnt;
      rlp_stats_log_pkt->rx_fill_fr_cnt     = rlp_stats_ptr->rx_fill_fr_cnt;
      rlp_stats_log_pkt->rx_blank_fr_cnt    = rlp_stats_ptr->rx_blank_fr_cnt;
      rlp_stats_log_pkt->rx_null_fr_cnt     = rlp_stats_ptr->rx_null_fr_cnt;
      rlp_stats_log_pkt->rx_new_data_fr_cnt = rlp_stats_ptr->rx_new_data_fr_cnt;
      rlp_stats_log_pkt->rx_20ms_fr_cnt     = rlp_stats_ptr->rx_20ms_fr_cnt;
      rlp_stats_log_pkt->rx_total_bytes     = rlp_stats_ptr->rx_total_bytes;
      rlp_stats_log_pkt->rx_rlp_erasures    = rlp_stats_ptr->rx_rlp_erasures;
      rlp_stats_log_pkt->rx_mux_erasures    = rlp_stats_ptr->rx_mux_erasures;

      rlp_stats_log_pkt->tx_rexmit_fr_cnt   = rlp_stats_ptr->tx_rexmit_fr_cnt;
      rlp_stats_log_pkt->tx_idle_fr_cnt     = rlp_stats_ptr->tx_idle_fr_cnt;
      rlp_stats_log_pkt->tx_new_data_fr_cnt = rlp_stats_ptr->tx_new_data_fr_cnt;
      rlp_stats_log_pkt->tx_20ms_fr_cnt     = rlp_stats_ptr->tx_20ms_fr_cnt;
      rlp_stats_log_pkt->tx_total_bytes     = rlp_stats_ptr->tx_total_bytes;
      rlp_stats_log_pkt->tx_naks_cnt        = rlp_stats_ptr->curr_tx_nak_rounds;
      for( i = 0; i < rlp_stats_ptr->curr_tx_nak_rounds; i++ ) {
        rlp_stats_log_pkt->tx_naks[i]       = rlp_stats_ptr->tx_naks[i];
      }

      if( callTerminated ) 
      {
        /*-------------------------------------------------------------------
          TODO: Add a flag to indicate if a call is in progress 
        -------------------------------------------------------------------*/
      }

      log_commit( (log_type*) rlp_stats_log_pkt );
      dsrlp_log_stats_frm_cnt = 0;
    }
  }
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_log_rlp_statistics() not supported");
#endif
  return;
}/* dsrlp_log_rlp_statistics() */


/*===========================================================================

FUNCTION DSRLP_LOG_RX_INIT_FRM_BUF

DESCRIPTION
  Initializes the dsrlp_log_rx_rlp_frm_buf structure 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_log_rx_init_frm_buf (void)
{
#ifdef FEATURE_RLP_LOGGING
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Initialize RLP frames logging buffer
  -------------------------------------------------------------------------*/
  dsrlp_log_rx_rlp_frm_buf.hdr.code = LOG_RLP_RX_FRAMES_C;
  dsrlp_log_rx_rlp_frm_buf.hdr.len  = FPOS(ds_log_rlp_frames_type, var_len_buf);
  dsrlp_log_rx_rlp_frm_buf.count    = 0;
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_log_rx_init_frm_buf() not supported");
#endif
  return;
}/* dsrlp_log_rx_init_frm_buf() */


/*===========================================================================

FUNCTION DSRLP_LOG_TX_INIT_FRM_BUF

DESCRIPTION
  Initializes the dsrlp_log_tx_rlp_frm_buf structure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_log_tx_init_frm_buf (void)
{
#ifdef FEATURE_RLP_LOGGING
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Initialize RLP frames logging buffer
  -------------------------------------------------------------------------*/
  dsrlp_log_tx_rlp_frm_buf.hdr.code = LOG_RLP_TX_FRAMES_C;
  dsrlp_log_tx_rlp_frm_buf.hdr.len  = FPOS(ds_log_rlp_frames_type, var_len_buf);
  dsrlp_log_tx_rlp_frm_buf.count    = 0;
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_log_tx_init_frm_buf() not supported");
#endif
  return;
}/* dsrlp_log_tx_init_frm_buf() */


/*===========================================================================

FUNCTION DSRLP_LOG_RX_INIT_FRAME

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP Rx frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  An RLP Rx partial frames log may be sent.

===========================================================================*/
ds_log_rlp_pkt_type * dsrlp_log_rx_init_frame(void)
{
#ifdef FEATURE_RLP_LOGGING
  ds_log_rlp_pkt_type *frm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If logging is enabled.   
  -------------------------------------------------------------------------*/
  if (log_status(dsrlp_log_rx_rlp_frm_buf.hdr.code))
  {
    /*-----------------------------------------------------------------------
      If already exceeding threshold for RLP bytes, log it.   
    -----------------------------------------------------------------------*/
    if (dsrlp_log_rx_rlp_frm_buf.hdr.len >= DS_LOG_RLP_FRM_BUF_THRESHOLD)
    {
      dsrlp_log_rx_send_frames();
    }

    /*-----------------------------------------------------------------------
      Set up log entry for RLP frames    
    -----------------------------------------------------------------------*/
    frm_ptr = (ds_log_rlp_pkt_type *)
    &dsrlp_log_rx_rlp_frm_buf.var_len_buf.buf_ref[DSRLP_LOG_RX_FRAMES_OFFSET];

    if (frm_ptr == &dsrlp_log_rx_rlp_frm_buf.var_len_buf.pkt[0])
    {
      /*---------------------------------------------------------------------
        First frame, log time stamp. 
      ---------------------------------------------------------------------*/
      (void)time_get((unsigned long *) dsrlp_log_rx_rlp_frm_buf.hdr.ts);
    }

    /*-----------------------------------------------------------------------
      Update packet length 
    -----------------------------------------------------------------------*/
    dsrlp_log_rx_rlp_frm_buf.hdr.len += FPOS(ds_log_rlp_pkt_type, payload);

    /*-----------------------------------------------------------------------
      # of frames in packet   
    -----------------------------------------------------------------------*/
    dsrlp_log_rx_rlp_frm_buf.count++;

    /*-----------------------------------------------------------------------
      Set reserved fields to 0    
    -----------------------------------------------------------------------*/
    frm_ptr->mux_chan.rsvd  = 0;
    frm_ptr->time_rate.rsvd = 0;
  }

  return frm_ptr;

#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_log_rx_init_frame() not supported");
  return NULL;
#endif

} /* dsrlp_log_rx_init_frame() */


/*===========================================================================

FUNCTION DSRLP_LOG_TX_INIT_FRAME

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP Tx frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  An RLP Tx partial frames log may be sent.

===========================================================================*/
ds_log_rlp_pkt_type * dsrlp_log_tx_init_frame(void)
{
#ifdef FEATURE_RLP_LOGGING
  ds_log_rlp_pkt_type *frm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If logging is enabled.   
  -------------------------------------------------------------------------*/
  if (log_status(dsrlp_log_tx_rlp_frm_buf.hdr.code))
  {
    /*-----------------------------------------------------------------------
      If already exceeding threshold for RLP bytes, log it.   
    -----------------------------------------------------------------------*/
    if (dsrlp_log_tx_rlp_frm_buf.hdr.len >= DS_LOG_RLP_FRM_BUF_THRESHOLD)
    {
      dsrlp_log_tx_send_frames();
    }

    /*-----------------------------------------------------------------------
      Set up log entry for RLP frames    
    -----------------------------------------------------------------------*/
    frm_ptr = (ds_log_rlp_pkt_type *)
    &dsrlp_log_tx_rlp_frm_buf.var_len_buf.buf_ref[DSRLP_LOG_TX_FRAMES_OFFSET];

    if (frm_ptr == &dsrlp_log_tx_rlp_frm_buf.var_len_buf.pkt[0])
    {
      /*---------------------------------------------------------------------
        First frame, log time stamp. 
      ---------------------------------------------------------------------*/
      (void)time_get((unsigned long *) dsrlp_log_tx_rlp_frm_buf.hdr.ts);
    }

    /*-----------------------------------------------------------------------
      Update packet length 
    -----------------------------------------------------------------------*/
    dsrlp_log_tx_rlp_frm_buf.hdr.len += FPOS(ds_log_rlp_pkt_type, payload);

    /*-----------------------------------------------------------------------
      # of frames in packet   
    -----------------------------------------------------------------------*/
    dsrlp_log_tx_rlp_frm_buf.count++;

    /*-----------------------------------------------------------------------
      Set reserved fields to 0    
    -----------------------------------------------------------------------*/
    frm_ptr->mux_chan.rsvd  = 0;
    frm_ptr->time_rate.rsvd = 0;
  }

  return frm_ptr;
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_log_tx_init_frame() not supported");
  return NULL;
#endif

} /* dsrlp_log_tx_init_frame() */


/*===========================================================================

FUNCTION DSRLP_LOG_RX_SEND_FRAMES

DESCRIPTION
  Terminate and send the accumulated RLP Rx partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  An RLP Rx partial frames log may be sent.

===========================================================================*/
void dsrlp_log_rx_send_frames (void)
{
#ifdef FEATURE_RLP_LOGGING
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Only send log if there is data in the buffer 
  -------------------------------------------------------------------------*/
  if (dsrlp_log_rx_rlp_frm_buf.hdr.len > FPOS(ds_log_rlp_frames_type, var_len_buf))
  {
    (void) log_submit((log_type *) &dsrlp_log_rx_rlp_frm_buf);

    /*-----------------------------------------------------------------------
      Re-initialize 
    -----------------------------------------------------------------------*/
    dsrlp_log_rx_init_frm_buf();
  }
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_log_rx_send_frames() not supported");
#endif
  return;

} /* dsrlp_log_rx_send_frames() */


/*===========================================================================

FUNCTION DSRLP_LOG_TX_SEND_FRAMES

DESCRIPTION
  Terminate and send the accumulated RLP Tx partial frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  An RLP Tx partial frames log may be sent.

===========================================================================*/
void dsrlp_log_tx_send_frames (void)
{
#ifdef FEATURE_RLP_LOGGING
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Only send log if there is data in the buffer
  -------------------------------------------------------------------------*/
  if (dsrlp_log_tx_rlp_frm_buf.hdr.len > FPOS(ds_log_rlp_frames_type, var_len_buf))
  {
    (void) log_submit((log_type *) &dsrlp_log_tx_rlp_frm_buf);

    /*-----------------------------------------------------------------------
      Re-initialize 
    -----------------------------------------------------------------------*/
    dsrlp_log_tx_init_frm_buf();
  }

#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_log_tx_send_frames() not supported");
#endif
  return;

} /* dsrlp_log_tx_send_frames() */

/*===========================================================================

FUNCTION DSRLP_GET_LOG_RX_RLP_FRM_BUF

DESCRIPTION
  Accessor method for dsrlp_log_rx_rlp_frm_buf

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to dsrlp_log_rx_rlp_frm_buf structure

SIDE EFFECTS
  None

===========================================================================*/
ds_log_rlp_frames_type *dsrlp_get_log_rx_rlp_frm_buf()
{
  return &dsrlp_log_rx_rlp_frm_buf;
}

/*===========================================================================

FUNCTION DSRLP_GET_LOG_TX_RLP_FRM_BUF

DESCRIPTION
  Accessor method for dsrlp_log_tx_rlp_frm_buf

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to dsrlp_log_tx_rlp_frm_buf structure

SIDE EFFECTS
  None

===========================================================================*/
ds_log_rlp_frames_type *dsrlp_get_log_tx_rlp_frm_buf()
{
  return &dsrlp_log_tx_rlp_frm_buf;
}

