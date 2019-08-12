/*==========================================================================

              Diag support for buffering and streaming modes

General Description
  
Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                            Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diag_mode.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/15/15   ph      Corrected the exit condition to dequeue the DSM items from 
                   buffering queue.
08/20/14   is      Buffering mode drain notification
01/01/14   ph      Fixed inconsistent drain behavior in circular and threshold buffering.
11/01/13   is      Resolve buffering mode issues
08/31/13   is      Support for peripheral buffering mode

===========================================================================*/

#include "diagdsm.h"      /* For DSM_DIAG_BUFFERING_ITEM_POOL */
#include "diagdiag_v.h"
#include "diagi_v.h"      /* For diag_tx_mode_type */
#include "diag_mode_v.h"
#include "diag_cfg.h"     /* For DIAGBUF_COMMIT_THRESHOLD */
#include "diagtarget.h"   /* For diag_kick_watchdog() */

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
PACK(void *) diagcomm_tx_mode_config( PACK(void *) req_pkt_ptr, uint16 pkt_len );
static void diag_ctrl_send_drain_status( void );

#if defined(DIAG_MP_MASTER) || !defined(DIAG_MP)
static void diag_update_mode_info( uint8 mode, uint8 stream_id );
#endif

/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern dsm_watermark_type diagcomm_sio_buffering_tx_wmq[NUM_SIO_PORTS]; /*From diagcomm_sio.c */

extern osal_timer_cnt_t diagbuf_tx_sleep_time; /* From diagbuf.c */

volatile diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID];

static const diagpkt_user_table_entry_type diagdiag_subsys_tbl_tx_mode[] =
{
  {DIAGDIAG_TX_MODE_CONFIG, DIAGDIAG_TX_MODE_CONFIG, diagcomm_tx_mode_config}
};

extern uint32 diagcomm_sio_get_buf_tx_wmq_cnt( diagcomm_enum_port_type port_num );

/*===========================================================================
FUNCTION DIAG_SET_BUFFERING_BUFFER (internal)

DESCRIPTION
  Changes the buffer used for peripheral buffering.  New DSM pool will be
  allocated from the memory location passed in through this function.
  
PARAMETERS
  buf_ptr - Pointer to buffer to be used for peripheral buffering mode.
  length - Length, in bytes, of buf_ptr.
  stream_id - Stream id to set buffer for
  
RETURN VALUE
  TRUE if buffer used for peripheral buffering was successfuly changed.
  FALSE otherwise.
  
===========================================================================*/
boolean diag_set_buffering_buffer( uint8 * buf_ptr, uint32 length, uint8 stream_id )
{
  static boolean dynamic_pool_initialized = FALSE;
  uint32 dsm_item_cnt = 0;
  uint8 stream_index = 0;
  uint8 port_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID))
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_buffering_buffer - Invalid stream_id %d", stream_id);
    return FALSE;
  }
  
  if( buf_ptr == NULL )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "buf_ptr is NULL");
    return FALSE;
  }
  else if( length <= DIAG_BUFFERED_DSM_CNT_TO_BYTES(DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_buffering_buffer - buf_ptr of %d bytes is less the default buffer of %d bytes", 
          length, DIAG_BUFFERED_DSM_CNT_TO_BYTES(DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT) );
    return FALSE;
  }
  else
  {
    stream_index = DIAG_ID_TO_INDEX( stream_id );
    port_index = DIAG_ID_TO_PORT( stream_id );
    
    dsm_item_cnt = length / (DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE + DSM_ITEM_HEADER_SIZE);
    
    if( dynamic_pool_initialized )
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_buffering_buffer - Already initialized.");
      return FALSE;
    }
        
    dsm_init_pool( DSM_DIAG_BUFFERING_ITEM_POOL, 
                   (uint8*)buf_ptr, 
                   dsm_item_cnt * (DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE + DSM_ITEM_HEADER_SIZE), 
                   DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE );
    dynamic_pool_initialized = TRUE;
    
    /* Set new pool_id */
    diag_tx_mode[stream_index].pool_info.pool_id = DSM_DIAG_BUFFERING_ITEM_POOL;
    diag_tx_mode[stream_index].pool_info.item_size = DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE;
    /* # of DSMs actually created is less than requested, so query the actual DSM count. */
    diag_tx_mode[stream_index].pool_info.item_cnt = DSM_POOL_ITEM_CNT(DSM_DIAG_BUFFERING_ITEM_POOL);
    
    /* Update flow control and buffering threshold counts. */
    diag_tx_mode[stream_index].buffered_many_per_mark = DIAG_BUFFERED_MANY_DRAIN_PER_MARK;
    diag_tx_mode[stream_index].buffered_many_bytes_mark = 
           ((float)diag_tx_mode[stream_index].buffered_many_per_mark / 100) * 
           (diag_tx_mode[stream_index].pool_info.item_size * diag_tx_mode[stream_index].pool_info.item_cnt);
    
    diag_tx_mode[stream_index].buffered_few_per_mark = DIAG_BUFFERED_FEW_DRAIN_PER_MARK;
    diag_tx_mode[stream_index].buffered_few_bytes_mark = 
            ((float)diag_tx_mode[stream_index].buffered_few_per_mark / 100) * 
           (diag_tx_mode[stream_index].pool_info.item_size * diag_tx_mode[stream_index].pool_info.item_cnt);

    diag_tx_mode[stream_index].many_flow_ctrl_dsm_mark = 
            ((float)DIAG_BUFFERED_MANY_FLOW_PER_MARK / 100) * diag_tx_mode[stream_index].pool_info.item_cnt;
    diag_tx_mode[stream_index].few_flow_ctrl_dsm_mark = 
        ((float)DIAG_BUFFERED_FEW_FLOW_PER_MARK / 100) * diag_tx_mode[stream_index].pool_info.item_cnt;
    
    /* Re-register flow control callbacks with new counts. */
    diagcomm_sio_tx_dsm_level_reg( port_index, TRUE );

    return TRUE;
  }
  
} /* diag_set_buffering_buffer */


/*===========================================================================

FUNCTION DIAG_TX_MODE_INIT (internal)

DESCRIPTION
  Initializes global variable for keeping track of streaming mode vs buffering mode.

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_tx_mode_init( void )
{
  uint8 stream_index = 0;
  
  for( stream_index=0; stream_index<DIAG_MAX_STREAM_ID; stream_index++)
  {
    diag_tx_mode[stream_index].prev_mode = (uint8)DIAG_TX_MODE_INVALID;
    diag_tx_mode[stream_index].mode = (uint8)DIAG_TX_MODE_STREAMING;
    diag_tx_mode[stream_index].drain = TRUE;
  
    diag_tx_mode[stream_index].buffered_many_bytes_mark = 
            (uint32)DIAG_BUFFERED_DSM_CNT_TO_BYTES(DSMI_DIAG_BUFFERED_MANY_DRAIN_MARK);
    diag_tx_mode[stream_index].buffered_many_per_mark = 
            (uint8)(((float)DSMI_DIAG_BUFFERED_MANY_DRAIN_MARK/(float)DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT)*100);
        
    diag_tx_mode[stream_index].buffered_few_bytes_mark = 
            (uint32)DIAG_BUFFERED_DSM_CNT_TO_BYTES(DSMI_DIAG_BUFFERED_FEW_DRAIN_MARK);
    diag_tx_mode[stream_index].buffered_few_per_mark = 
            (uint8)(((float)DSMI_DIAG_BUFFERED_FEW_DRAIN_MARK/(float)DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT)*100);
              
    diag_tx_mode[stream_index].many_flow_ctrl_dsm_mark = DSMI_DIAG_BUFFERED_MANY_FLOW_MARK;
    diag_tx_mode[stream_index].few_flow_ctrl_dsm_mark = DSMI_DIAG_BUFFERED_FEW_FLOW_MARK;
    
    diag_health_reset_dropped_cnt_stream( DIAG_INDEX_TO_ID(stream_index) );
  }
      
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC( DIAG_MY_PROC_ID, 
                                        DIAG_SUBSYS_DIAG_SERV, 
                                        diagdiag_subsys_tbl_tx_mode );

} /* diag_tx_mode_init */


/*===========================================================================
FUNCTION DIAG_BUFFERED_MODE_STREAM (internal)

DESCRIPTION
  Checks if we're currently in one of the buffering modes.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  
RETURN VALUE
  TRUE - If we're currently in one of the buffering modes.
  FALSE - Otherwise.
  
===========================================================================*/
boolean diag_buffered_mode_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return FALSE;
  }
    
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_THRESH) ||
     (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {
    return TRUE;
  }  
  else
  {
    return FALSE;
  }

} /* diag_buffered_mode_stream */


/*===========================================================================
FUNCTION DIAG_UPDATE_MODE_INFO (internal)

DESCRIPTION
  This function toggles the local processor's sleep voting and NRT parameters
  based on Streaming vs Buffering modes. When we enter buffering mode, we
  disable sleep voting and disable drain timers.  Drain thresholds are also
  increased.  These changes are reverted when we go back to streaming mode.
  
  If Apps goes into buffering mode, these sleep and NRT parameters are
  propagated to peripherals via a CTRL packet.  If a peripheral goes
  into buffering mode, these parameters are only toggled for the
  local processor.
  
  Since stream 1 and stream 2 (DCI) can be buffered indepedently, we will
  only switch to NRT mode if all streams are in NRT mode.  If any stream 
  is in streaming mode, we will remain in real-time mode.
  
PARAMETERS
  mode - DIAG_TX_MODE_STREAMING         0
       - DIAG_TX_MODE_BUFFERED_THRESH   1
       - DIAG_TX_MODE_BUFFERED_CIR      2
       
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.

DEPENDENCIES 
  diag_set_tx_mode_stream()
  We should ensure that this procedure is called from diag_set_tx_mode_stream()
    only -- that sets the Diag Transmission Mode
  
RETURN VALUE
  None 
  
===========================================================================*/
#if defined(DIAG_MP_MASTER) || !defined(DIAG_MP)
static void diag_update_mode_info( uint8 mode, uint8 stream_id )
{
    uint8 sleep_voting_flag = 0;

    #if defined(DIAG_MP_MASTER)
    diag_ctrl_msg_type ctrl_msg;
    int port_num = 0;
    boolean is_sent = FALSE;
    #endif /* DIAG_MP_MASTER */
    
    boolean is_streaming = FALSE;
    
    if( mode > DIAG_TX_MODE_LAST )
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_update_mode_info: Invalid mode (%d)", mode);
      return;
    }
    
    #if defined(DIAG_MP_MASTER)
    /* Fill the control message header */
    ctrl_msg.cmd_type = DIAG_CTRL_MSG_DIAGMODE;
    ctrl_msg.data_len = sizeof(diag_ctrl_msg_diagmode_type);
    ctrl_msg.data.ctrl_msg_diagmode.version = DIAG_CTRL_MSG_DIAGMODE_VER;
    #endif /* DIAG_MP_MASTER */
    
    if( mode == DIAG_TX_MODE_STREAMING )
    {      
      /* Enable sleep voting for Streaming Mode */
      sleep_voting_flag = 1;

      #if defined(DIAG_MP_MASTER)
      /* Setup CTRL packet to update peripherals of Apps buffering. */
      ctrl_msg.data.ctrl_msg_diagmode.sleep_vote = sleep_voting_flag;  /* Setting the sleep vote flag */
      ctrl_msg.data.ctrl_msg_diagmode.real_time = 1;
      ctrl_msg.data.ctrl_msg_diagmode.use_nrt_values = 0;
      ctrl_msg.data.ctrl_msg_diagmode.commit_threshold = 0;
      ctrl_msg.data.ctrl_msg_diagmode.sleep_threshold = 0;
      ctrl_msg.data.ctrl_msg_diagmode.sleep_time = 0;
      ctrl_msg.data.ctrl_msg_diagmode.drain_timer_val = 0;
      ctrl_msg.data.ctrl_msg_diagmode.event_stale_timer_val = 0;
      #endif /* DIAG_MP_MASTER */
      
      /* Update locally. */
      diagbuf_ctrl_tx_sleep_parameters( DIAG_TX_SLEEP_THRESHOLD_DEFAULT, DIAG_TX_SLEEP_TIME_DEFAULT );
      diagbuf_ctrl_commit_threshold( DIAGBUF_COMMIT_THRESHOLD );
      diag_ctrl_drain_timer_len( DIAG_DRAIN_TIMER_LEN );
      event_ctrl_stale_timer( EVENT_TIMER_LEN );
      event_ctrl_report_size( EVENT_RPT_PKT_SIZE, EVENT_SEND_MAX, EVENT_RPT_PKT_LEN_SIZE );
    }
    else if( (mode == DIAG_TX_MODE_BUFFERED_THRESH) || (mode == DIAG_TX_MODE_BUFFERED_CIR) )
    {
      /* Check if any steam is in streaming mode. */
      if( diag_tx_mode[DIAG_ID_TO_INDEX(DIAG_STREAM_1)].mode == DIAG_TX_MODE_STREAMING )
      {
        is_streaming = TRUE;
      }

      /* Consider DCI mode only if there are DCI clients. */
      if( diag_tx_mode[DIAG_ID_TO_INDEX(DIAG_STREAM_2)].mode == DIAG_TX_MODE_STREAMING &&
          DIAG_CHECK_MASK(diag_cx_state, DIAG_CX_DCI_S) )
      {
        is_streaming = TRUE;
      }

      /* Move to NRT mode only if all streams are not in streaming mode. */
      if( is_streaming == FALSE )
      {
        /* Disable Sleep Voting for Buffering Modes */
        sleep_voting_flag = 0;

        #if defined(DIAG_MP_MASTER)
        /* Setup CTRL packet to update peripherals of Apps buffering. */
        ctrl_msg.data.ctrl_msg_diagmode.sleep_vote = sleep_voting_flag;  /* Setting the sleep vote flag */
        ctrl_msg.data.ctrl_msg_diagmode.real_time = 0;                   /* real_time = 0 means we use default NRT values */
        ctrl_msg.data.ctrl_msg_diagmode.use_nrt_values = 0;
        ctrl_msg.data.ctrl_msg_diagmode.commit_threshold = 0;
        ctrl_msg.data.ctrl_msg_diagmode.sleep_threshold = 0;
        ctrl_msg.data.ctrl_msg_diagmode.sleep_time = 0;
        ctrl_msg.data.ctrl_msg_diagmode.drain_timer_val = 0;
        ctrl_msg.data.ctrl_msg_diagmode.event_stale_timer_val = 0;
        #endif /* DIAG_MP_MASTER */
  
        /* Update locally. */
        diagbuf_ctrl_tx_sleep_parameters( DIAG_TX_SLEEP_THRESHOLD_NRT, DIAG_TX_SLEEP_TIME_NRT );
        diagbuf_ctrl_commit_threshold( DIAGBUF_COMMIT_THRESHOLD_NRT );
        diag_ctrl_drain_timer_len( 0 );
        event_ctrl_stale_timer( 0 );
        event_ctrl_report_size(EVENT_RPT_PKT_SIZE_NRT, 1024, DIAGBUF_SIO_TX_ITEM_SIZ - DIAG_HDLC_PAD_LEN);
      }
    }

    #if defined(DIAG_MP_MASTER)
    /* Send the control message to all the peripherals */
    for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
    {
      (void)diagcomm_ctrl_send_mult_pkt((PACK(void *)) &ctrl_msg, 
                                         DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_diagmode_type),
                                         NULL, 0, DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, TRUE, &is_sent);
    }
    #endif /* DIAG_MP_MASTER */

    /* Change the local processor's sleep voting mechanism */
    diag_control_sleep_voting(sleep_voting_flag);

    if( sleep_voting_flag == 0 )
    {
      diag_allow_sleep();
    }
    
} /* diag_update_mode_info */
#endif /* DIAG_MP_MASTER || !DIAG_MP */

/*===========================================================================
FUNCTION DIAG_SET_TX_MODE_STREAM (internal)

DESCRIPTION
  Sets the Tx mode. 
  
PARAMETERS
  mode - DIAG_TX_MODE_STREAMING         0
       - DIAG_TX_MODE_BUFFERED_THRESH   1
       - DIAG_TX_MODE_BUFFERED_CIR      2
  
  stream_id - stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and 
                          DIAG_MAX_STREAM_ID.
   
RETURN VALUE
  The current Tx mode.
  
===========================================================================*/
uint8 diag_set_tx_mode_stream( uint8 mode, uint8 stream_id )
{
  uint8 stream_index = 0;
  uint8 prev_mode = 0;
  
  /* ToDo: Only support switching modes for STREAM 1 now. Add support for STREAM 2 later. */
  if( stream_id != DIAG_STREAM_1 )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode_stream - Invalid stream_id (%d)", stream_id);
    return DIAG_TX_MODE_INVALID;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  if( !DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode_stream - Buffering mode not allowed if F_DIAG_REQ_RSP_CHANNEL not supported");
    return diag_tx_mode[stream_index].mode;
  }
    
  prev_mode = diag_tx_mode[stream_index].mode;
  
  if ( mode <= DIAG_TX_MODE_LAST )
  {
    /* Reset health counters if mode has changed. Also saves prev mode statistics. */
    if ( prev_mode != mode )
      diag_health_reset_dropped_cnt_stream( stream_id );

    diag_tx_mode[stream_index].mode = mode;
  }
  else
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode_stream - Cannot set invalid mode (%d) for stream_id (%d)", mode, stream_id);
    return diag_tx_mode[stream_index].mode;
  }

  if ( diag_tx_mode[stream_index].mode == DIAG_TX_MODE_STREAMING )
  {
    diag_tx_mode[stream_index].drain = TRUE;
        
    #if defined(DIAG_MP_MASTER)
    /* Restore lower levels for flow control in streaming mode. Lower flow control
       levels prevents Diag from unable to catch up with draining data if SIO outbound 
       is slow. */
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SMD_TX_ITEM_POOL,                                
      DSM_MEM_LEVEL_MANY,                                       
      DSMI_DIAG_SMD_TX_ITEM_CNT-DSMI_DIAG_SMD_TX_ITEM_MANY_MARK
    );
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SMD_TX_ITEM_POOL,                               
      DSM_MEM_LEVEL_FEW,                                       
      DSMI_DIAG_SMD_TX_ITEM_CNT-DSMI_DIAG_SMD_TX_ITEM_FEW_MARK 
    );

    /* Update sleep voting and NRT params. */
    diag_update_mode_info( diag_tx_mode[stream_index].mode, stream_id );
    
    /* Initiate draining. */
    diag_fwd_notify();
    #else

      #if !defined(DIAG_MP)
      /* Update NRT params locally only if Master or Standalone.  If Slave, Master
         will send the NRT control packets. */
      diag_update_mode_info( diag_tx_mode[stream_index].mode, stream_id );
      #endif
       
    /* Set signal to drain buffering pool */
    if( stream_id == DIAG_STREAM_1 )
      diag_set_internal_sigs( DIAG_INT_DRAIN_DSM_BUFFER_SIG );
    else if( stream_id == DIAG_STREAM_2 )
      diag_set_internal_sigs( DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG );
    #endif
  }
  else if ( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_THRESH) ||
            (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {
    diag_tx_mode[stream_index].drain = FALSE;
    
    #if defined(DIAG_MP_MASTER)
    /* Register max levels for flow control in buffering modes. */
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SMD_TX_ITEM_POOL,                               
      DSM_MEM_LEVEL_MANY,                                     
      DSMI_DIAG_SMD_TX_ITEM_CNT-DSMI_DIAG_SMD_TX_ITEM_BUFFERED_MANY_MARK 
    );
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SMD_TX_ITEM_POOL,                              
      DSM_MEM_LEVEL_FEW,                                      
      DSMI_DIAG_SMD_TX_ITEM_CNT-DSMI_DIAG_SMD_TX_ITEM_BUFFERED_FEW_MARK 
    );
    #endif

    #if defined(DIAG_MP_MASTER) || !defined(DIAG_MP)
      /* Update NRT params locally only if Master or Standalone.  If Slave, Master
         will send the NRT control packets. */
      diag_update_mode_info( diag_tx_mode[stream_index].mode, stream_id );
    #endif
  }
  
  return diag_tx_mode[stream_index].mode;

} /* diag_set_tx_mode_stream */


/*===========================================================================
FUNCTION DIAG_GET_TX_MODE_STREAM (internal)

DESCRIPTION
  Gets the current Tx mode. 
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  
RETURN VALUE
  The current Tx mode.
  
===========================================================================*/
uint8 diag_get_tx_mode_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_get_tx_mode_stream - Invalid stream_id (%d)", stream_id);
    return DIAG_TX_MODE_INVALID;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  return diag_tx_mode[stream_index].mode;
  
} /* diag_get_tx_mode_stream */


/*===========================================================================
FUNCTION DIAG_SET_TX_PARAM_WM_PER_STREAM (internal)

DESCRIPTION
  Sets the Tx high and low watermark parameter. In buffered threshold mode, 
  when the number of buffered bytes has reached the high wm level, we start
  to drain until we reach the low wm level.
  
  High wm level cannot be less then low wm level. High wm level should
  also not be greater than the level when we enable flow control to 
  stop the traffic from the peripherals.
  
  In buffered threshold and buffered circular modes, we stop draining when 
  the number of bytes in the buffered watermark queue has reached the low
  watermark level.
  
PARAMETERS
  high_per_value - High watermark value (as a percentage of total buffering pool size).
  low_per_value - Low watermark value (as a percentage of total buffering pool size).
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  
RETURN VALUE
  TRUE - The high/low wm values were updated successfully.
  FALSE - The high/low watermark values were NOT updated successfully.
  
===========================================================================*/
boolean diag_set_tx_param_wm_stream( uint8 high_per_value, uint8 low_per_value, uint8 stream_id )
{
  /* Don't allow drain thresholds where Diag would have to be flow-controlled before we drain */
  uint32 many_mark_bytes = 0;
  uint32 few_mark_bytes = 0;
  uint8 stream_index = 0;
  uint32 high_byte_value = 0;
  uint32 low_byte_value = 0;
  uint32 buffer_size = 0;
  boolean ret_value = TRUE;
  uint8 new_high_value;
  uint8 new_low_value;
              
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_param_wm_stream - Invalid stream_id (%d)", stream_id);
    return 0;
  }
  
  stream_index = DIAG_ID_TO_INDEX(stream_id);
    
  /* If '0' is passed in, use current value. */ 
  if( high_per_value > 0 )
    new_high_value = high_per_value;
  else
    new_high_value = diag_tx_mode[stream_index].buffered_many_per_mark;
    
  if( low_per_value > 0 )
    new_low_value = low_per_value;
  else
    new_low_value = diag_tx_mode[stream_index].buffered_few_per_mark;
    
  /* Sanity check of new high wm value. */  
  if( (new_high_value <= new_low_value) || (new_high_value > 100) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_param_wm_stream - Invalid high percentage value %d for stream %d", 
          high_per_value, stream_id);
    ret_value = FALSE;
  }
  
  if( (new_low_value >= new_high_value) || (new_low_value > 100) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_param_wm_stream - Invalid low percentage value %d for stream %d", 
          low_per_value, stream_id);
    ret_value = FALSE;
  }
  
  if( ret_value == FALSE )
    return FALSE;
    
  buffer_size = diag_tx_mode[stream_index].pool_info.item_cnt * diag_tx_mode[stream_index].pool_info.item_size;
  
  high_byte_value = ((float)new_high_value/100)*buffer_size;
  many_mark_bytes = diag_tx_mode[stream_index].many_flow_ctrl_dsm_mark * diag_tx_mode[stream_index].pool_info.item_size;
  
  low_byte_value = ((float)new_low_value/100)*buffer_size;
  few_mark_bytes = diag_tx_mode[stream_index].few_flow_ctrl_dsm_mark * diag_tx_mode[stream_index].pool_info.item_size;
          
  /* Check against flow control marks. */
  if( (high_byte_value <= diag_tx_mode[stream_index].buffered_few_bytes_mark) || (high_byte_value >= many_mark_bytes) )
  {
    /* If high_byte_value >= many_mark_bytes, then flow control would kick in before we are allowed to drain in 
       threshold buffering mode or wrap in circular buffering mode. */
    MSG_4(MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
          "High byte value %d for stream %d cannot be less than few bytes mark %d or greater than many flow control mark %d", 
          high_byte_value, stream_id, diag_tx_mode[stream_index].buffered_few_bytes_mark, many_mark_bytes);
    ret_value = FALSE;
  }
  
  if( (low_byte_value >= diag_tx_mode[stream_index].buffered_many_bytes_mark) || (low_byte_value >= few_mark_bytes) )
  {
    /* If low_byte_value >= few_mark_bytes and we happen to be flow controlled, then we would stop draining before
       hitting the low threshold which resumes flow. */
    MSG_4(MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
          "LOW byte value %d for stream %d cannot be greater than many bytes mark %d or greater than few flow control mark %d", 
          low_byte_value, stream_id, diag_tx_mode[stream_index].buffered_many_bytes_mark, few_mark_bytes);
    ret_value = FALSE;
  }
  
  if( ret_value == FALSE )
    return FALSE;
    
  /* Passed all sanity checks. Set values now. */
  diag_tx_mode[stream_index].buffered_many_bytes_mark = high_byte_value;
  diag_tx_mode[stream_index].buffered_many_per_mark = high_per_value;
    
  diag_tx_mode[stream_index].buffered_few_bytes_mark = low_byte_value;
  diag_tx_mode[stream_index].buffered_few_per_mark = low_per_value;
          
  return ret_value;
  
} /* diag_set_tx_param_wm_stream */


/*===========================================================================
FUNCTION DIAG_DRAIN_TX_BUFFER_STREAM (internal)

DESCRIPTION
  Drains the Tx buffer immediately. If in buffered mode, it will drain until
  Tx low watermark threshold is reached.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and 
              DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_drain_tx_buffer_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_drain_tx_buffer_stream - Invalid stream_id (%d)", stream_id);
    return;
  }
  
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  diag_tx_mode[stream_index].drain = TRUE;
  
#if defined(DIAG_MP_MASTER)
  /* Notify forwarding task to drain DSM pool. */
  diag_fwd_notify();
#else
  /* Set signal to transmit DSMs from buffering pool */
  if( stream_id == DIAG_STREAM_1 )
    diag_set_internal_sigs( DIAG_INT_DRAIN_DSM_BUFFER_SIG );
  else if( stream_id == DIAG_STREAM_2 )
    diag_set_internal_sigs( DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG );
#endif
  
} /* diag_drain_tx_buffer_stream */


/*===========================================================================
FUNCTION DIAG_HEALTH_GET_DROPPED_DSM_CNT_STREAM (internal)

DESCRIPTION
  This function returns the number of DSMs dropped while in buffered circular mode.
  In buffered circular mode, when buffer is full, older DSMs are dropped to make 
  room for newwer DSMs.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  Number of DSMs dropped in buffered circular mode.
  
===========================================================================*/
uint32 diag_health_get_dropped_dsm_cnt_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
          "diag_health_get_dropped_dsm_cnt_stream - Invalid stream_id (%d)", stream_id);
    return 0;
  }
  
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  return diag_tx_mode[stream_index].dsm_dropped_cnt;
  
} /* diag_health_get_dropped_dsm_cnt_stream */


/*===========================================================================
FUNCTION DIAG_HEALTH_GET_DROPPED_BYTES_CNT_STREAM (internal)

DESCRIPTION
  This function returns the number of bytes dropped while in buffered circular mode.
  In buffered circular mode, when buffer is full, older DSMs are dropped to make 
  room for newwer DSMs.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  Number of bytes dropped in buffered circular mode.
  
===========================================================================*/
uint32 diag_health_get_dropped_bytes_cnt_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
          "diag_health_get_dropped_bytes_cnt_stream - Invalid stream_id (%d)", stream_id);
    return 0;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  return diag_tx_mode[stream_index].byte_dropped_cnt;
  
} /* diag_health_get_dropped_bytes_cnt_stream */


/*===========================================================================
FUNCTION DIAG_HEALTH_GET_ALLOC_DSM_CNT_STREAM (internal)

DESCRIPTION
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  
RETURN VALUE
  None
  
===========================================================================*/
uint32 diag_health_get_alloc_dsm_cnt_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return 0;
  }
    
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  return diag_tx_mode[stream_index].dsm_alloc_cnt;
  
} /* diag_health_get_alloc_dsm_cnt_stream */


/*===========================================================================
FUNCTION DIAG_HEALTH_GET_ALLOC_BYTES_CNT_STREAM (internal)

DESCRIPTION
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  
RETURN VALUE
  None
  
===========================================================================*/
uint32 diag_health_get_alloc_bytes_cnt_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return 0;
  }
    
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  return diag_tx_mode[stream_index].byte_alloc_cnt;
  
} /* diag_health_get_alloc_bytes_cnt_stream */


/*===========================================================================
FUNCTION DIAG_HEALTH_RESET_DROPPED_CNT_STREAM (internal)

DESCRIPTION
  Resets buffering mode health counter statistics per stream id.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
                  
RETURN VALUE
  None
  
===========================================================================*/
void diag_health_reset_dropped_cnt_stream( uint8 stream_id )
{  
  uint8 num_ports = 0;
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return;
  }
    
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  diag_tx_mode[stream_index].prev_mode = diag_tx_mode[stream_index].mode;
  diag_tx_mode[stream_index].prev_dsm_dropped_cnt = diag_tx_mode[stream_index].dsm_dropped_cnt;
  diag_tx_mode[stream_index].prev_dsm_alloc_cnt = diag_tx_mode[stream_index].dsm_alloc_cnt;
  diag_tx_mode[stream_index].prev_byte_dropped_cnt = diag_tx_mode[stream_index].byte_dropped_cnt;
  diag_tx_mode[stream_index].prev_byte_alloc_cnt = diag_tx_mode[stream_index].byte_alloc_cnt;
  
  for( num_ports=0; num_ports < NUM_SMD_PORT_TYPES; num_ports++ )
  {
    diag_tx_mode[stream_index].cur_dsm_cnt[num_ports] = 0;
  }
  
#if defined(DIAG_MP_MASTER)
  diag_tx_mode.fwd_cur_dsm_cnt = 0;
#endif

  diag_tx_mode[stream_index].dsm_dropped_cnt = 0;
  diag_tx_mode[stream_index].dsm_alloc_cnt = 0;
  diag_tx_mode[stream_index].byte_dropped_cnt = 0;
  diag_tx_mode[stream_index].byte_alloc_cnt = 0;
    
} /* diag_health_reset_dropped_cnt_stream */


/*===========================================================================

FUNCTION DIAG_DRAIN_POOL_CHECK (internal)

DESCRIPTION
  Checks whether Diag needs to set signal to drain the DSM pool. Diag should
  signal draining if we're passed the threshold in threshold buffering mode
  or if we're in streaming mode.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  TRUE - Diag can drain the pool now.
  FALSE - Buffering pools are not ready to be drained yet.
  
===========================================================================*/
boolean diag_drain_pool_check( uint8 stream_id )
{
  uint32 dsm_cnt = 0;
  uint8 stream_index = 0;
  diagcomm_enum_port_type port_num;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return FALSE;
  }

  port_num = DIAG_ID_TO_PORT(stream_id);
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  dsm_cnt = diagcomm_sio_get_buf_tx_wmq_cnt( port_num );
  
  if( diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
    if( diag_tx_mode[stream_index].drain && 
       (DIAG_BUFFERED_DSM_CNT_TO_BYTES(dsm_cnt) <= diag_tx_mode[stream_index].buffered_few_bytes_mark) )
    {
       diag_tx_mode[stream_index].drain = FALSE;
       /* Circular buffering mode has just stopped draining. Send drain status notification. */
       diag_ctrl_send_drain_status();
    }
  }

  if( diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_THRESH )
  {
    if( DIAG_BUFFERED_DSM_CNT_TO_BYTES(dsm_cnt) >= diag_tx_mode[stream_index].buffered_many_bytes_mark )
    {
      diag_tx_mode[stream_index].drain = TRUE;
    }
    else if( diag_tx_mode[stream_index].drain && 
            (DIAG_BUFFERED_DSM_CNT_TO_BYTES(dsm_cnt) <= diag_tx_mode[stream_index].buffered_few_bytes_mark) )
    {    
      diag_tx_mode[stream_index].drain = FALSE;
      /* Threshold buffering mode has just stopped draining. Send drain status notification. */
      diag_ctrl_send_drain_status();
    }
  }
  
  if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_STREAMING) || diag_tx_mode[stream_index].drain )
  {
    if( diagcomm_buffering_tx_wmq_flow_enabled(port_num) == TRUE )
    {
      return TRUE;
    }
  }

  return FALSE;

} /* diag_drain_pool_check */


/*===========================================================================
FUNCTION diag_buffering_pool_used_cnt (internal)
  
DESCRIPTION
  This function returns the # of DSMs used in the buffering pool for
  each stream.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  The # of DSMs used in the buffering pool for each stream.
  
===========================================================================*/
uint32 diag_buffering_pool_used_cnt( uint8 stream_id ) 
{
  uint8 stream_index = 0;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return 0;
  }
    
  stream_index = DIAG_ID_TO_INDEX(stream_id);
   
  return (DSM_POOL_ITEM_CNT(diag_tx_mode[stream_index].pool_info.pool_id) - 
          DSM_POOL_FREE_CNT(diag_tx_mode[stream_index].pool_info.pool_id));

} /* diag_buffering_pool_used_cnt */


/*===========================================================================
FUNCTION DIAGCOMM_TX_MODE_CONFIG (internal)

DESCRIPTION
  Command handler function for buffered mode commands.
  
PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet
  
RETURN VALUE
  Pointer to response packet.
  
===========================================================================*/
PACK(void *) diagcomm_tx_mode_config( PACK(void *) req_pkt_ptr, uint16 pkt_len )
{
  diagcomm_tx_mode_config_stream_req_type * req_ptr = (diagcomm_tx_mode_config_stream_req_type *)req_pkt_ptr;
  diagcomm_tx_mode_config_stream_rsp_type * rsp_ptr = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;
  uint8 stream_id = 0;
  uint8 stream_index = 0;
  
  if( req_ptr == NULL )
    return rsp_ptr;

  cmd_code = diagpkt_subsys_get_cmd_code( req_ptr );
  stream_id = req_ptr->stream_id;
  
  if( stream_id > DIAG_MAX_STREAM_ID )
  {
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
  }
  
  switch( req_ptr->op_code )
  {
    int id = 0;

    case DIAG_TX_MODE_CONFIG_SET_MODE:
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_SET_MODE;
        rsp_ptr->stream_id = stream_id;
        
        if( stream_id == DIAG_STREAM_ALL )
        {
          for( id=DIAG_MIN_STREAM_ID; id<=DIAG_MAX_STREAM_ID; id++ )
            rsp_ptr->data.mode = diag_set_tx_mode_stream( req_ptr->data.mode, id );
        }
        else
        {
          rsp_ptr->data.mode = diag_set_tx_mode_stream( req_ptr->data.mode, stream_id );
        }
      }
      break;

    case DIAG_TX_MODE_CONFIG_GET_MODE:
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_MODE;
        rsp_ptr->stream_id = stream_id;
        rsp_ptr->data.mode = diag_get_tx_mode_stream( stream_id );
      }
      break;

    case DIAG_TX_MODE_CONFIG_SET_PARAM_WM_PER:
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                              FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) 
                                                              + sizeof(boolean));
        if( rsp_ptr )
        {     
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_SET_PARAM_WM_PER;
          rsp_ptr->stream_id = stream_id;
          rsp_ptr->data.ret_value = diag_set_tx_param_wm_stream( req_ptr->data.wm_data.high_wm_per_value, 
                                                                 req_ptr->data.wm_data.low_wm_per_value, 
                                                                 stream_id );
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
      
    case DIAG_TX_MODE_CONFIG_GET_PARAM_WM:
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + 
                                                       sizeof(diagcomm_tx_mode_config_wm_rsp_type) );
        if( rsp_ptr )
        {     
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_PARAM_WM;
          rsp_ptr->stream_id = stream_id;
          stream_index = DIAG_ID_TO_INDEX(stream_id);
          
          rsp_ptr->data.wm_data.high_wm_per_value = diag_tx_mode[stream_index].buffered_many_per_mark;
          rsp_ptr->data.wm_data.high_wm_value = diag_tx_mode[stream_index].buffered_many_bytes_mark;
          rsp_ptr->data.wm_data.low_wm_per_value = diag_tx_mode[stream_index].buffered_few_per_mark;
          rsp_ptr->data.wm_data.low_wm_value = diag_tx_mode[stream_index].buffered_few_bytes_mark;
          rsp_ptr->data.wm_data.buffer_size = (diag_tx_mode[stream_index].pool_info.item_cnt * 
                                               diag_tx_mode[stream_index].pool_info.item_size);
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
              
    case DIAG_TX_MODE_CONFIG_DRAIN:
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) );
      if( rsp_ptr )
      {
        if( stream_id == DIAG_STREAM_ALL )
        {
          for( id=DIAG_MIN_STREAM_ID; id<=DIAG_MAX_STREAM_ID; id++ )
            diag_drain_tx_buffer_stream( id );
        }
        else
        {
          diag_drain_tx_buffer_stream( stream_id );
        }
        
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_DRAIN;
        rsp_ptr->stream_id = stream_id;
      }
      break;
      
    case DIAG_TX_MODE_CONFIG_GET_CUR_DSM_CNT: 
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint16) );
        if( rsp_ptr )
        {
          diagcomm_enum_port_type port_num= DIAG_ID_TO_PORT(stream_id);
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_CUR_DSM_CNT;
          rsp_ptr->stream_id = stream_id;
          rsp_ptr->data.buffered_dsm_cnt =  diagcomm_sio_get_buf_tx_wmq_cnt( port_num );
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
      
    case DIAG_TX_MODE_CONFIG_GET_CUR_BYTES_CNT: 
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
        if( rsp_ptr )
        {
          diagcomm_enum_port_type port_num= DIAG_ID_TO_PORT(stream_id);
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_CUR_BYTES_CNT;
          rsp_ptr->stream_id = stream_id;
          rsp_ptr->data.bytes_buffered_cnt = DIAG_BUFFERED_DSM_CNT_TO_BYTES(diagcomm_sio_get_buf_tx_wmq_cnt( port_num ));
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
      
    case DIAG_TX_MODE_CONFIG_GET_DSM_DROPPED_CNT:
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
        if( rsp_ptr )
        {
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_DSM_DROPPED_CNT;
          rsp_ptr->stream_id = stream_id;
        
          stream_index = DIAG_ID_TO_INDEX(stream_id);
          rsp_ptr->data.dsm_dropped_cnt = diag_tx_mode[stream_index].dsm_dropped_cnt;
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
      
    case DIAG_TX_MODE_CONFIG_GET_DSM_ALLOC_CNT:
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
        if( rsp_ptr )
        {
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_DSM_ALLOC_CNT;
          rsp_ptr->stream_id = stream_id;
        
          stream_index = DIAG_ID_TO_INDEX(stream_id);
          rsp_ptr->data.dsm_alloc_cnt = diag_tx_mode[stream_index].dsm_alloc_cnt;
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
       
    case DIAG_TX_MODE_CONFIG_GET_BYTE_DROPPED_CNT:
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
        if( rsp_ptr )
        {
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_BYTE_DROPPED_CNT;
          rsp_ptr->stream_id = stream_id;
        
          stream_index = DIAG_ID_TO_INDEX(stream_id);
          rsp_ptr->data.byte_dropped_cnt = diag_tx_mode[stream_index].byte_dropped_cnt;
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
      
    case DIAG_TX_MODE_CONFIG_GET_BYTE_ALLOC_CNT:
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
        if( rsp_ptr )
        {
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_BYTE_ALLOC_CNT;
          rsp_ptr->stream_id = stream_id;
        
          stream_index = DIAG_ID_TO_INDEX(stream_id);
          rsp_ptr->data.byte_alloc_cnt = diag_tx_mode[stream_index].byte_alloc_cnt;
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
       
    case DIAG_TX_MODE_CONFIG_RESET_DROPPED_CNT:
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_RESET_DROPPED_CNT;
        rsp_ptr->stream_id = stream_id;
        
        if( stream_id == DIAG_STREAM_ALL )
        {
          for( id=DIAG_MIN_STREAM_ID; id<=DIAG_MAX_STREAM_ID; id++ )
            diag_health_reset_dropped_cnt_stream( id );
        }
        else
        {
          diag_health_reset_dropped_cnt_stream( stream_id );
        }
      }
      break;
      
    case DIAG_TX_MODE_CONFIG_GET_BUFFER_SIZE:
      if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code, 
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
        if( rsp_ptr )
        {
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_BUFFER_SIZE;
          rsp_ptr->stream_id = stream_id;
        
          stream_index = DIAG_ID_TO_INDEX(stream_id);
          rsp_ptr->data.buffer_size = DSM_POOL_ITEM_CNT(diag_tx_mode[stream_index].pool_info.pool_id) * 
                                      DSM_POOL_ITEM_SIZE(diag_tx_mode[stream_index].pool_info.pool_id);
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;
      
    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
  }
  
  return rsp_ptr;
  
} /* diagcomm_tx_mode_config */


/*===========================================================================
FUNCTION DIAG_DRAIN_DSM_BUFFER (internal)

DESCRIPTION
  This function drains the buffering DSM pools per stream. 
  
PARAMETERS
  stream_id - stream_id for buffering DSM pool to drain
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_drain_dsm_buffer( uint8 stream_id )
{
  static dsm_item_type * diag_item_ptr = NULL;
  diagcomm_io_tx_params_type tx_params;
  uint8 port_index;
  uint8 stream_index;
 
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
    return;
    
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  port_index = DIAG_ID_TO_PORT(stream_id);
  
   /* Start drain buffering if buffering checks pass. */
  if( diag_drain_pool_check(stream_id) )
  {
  if( diagcomm_status(port_index ) )
  {
      diag_item_ptr = diagcomm_dequeue_buffering_tx_wmq( port_index );

    if( diag_item_ptr != NULL )
    {
      tx_params.dsm_ptr = diag_item_ptr;
      diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_index], &tx_params );

      //is: test
      if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_THRESH) || 
          (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR) )
      {
        diag_suspend( diagbuf_tx_sleep_time );
      }
            
      /* Set drain buffering signal if buffering checks pass. */
      if( diag_drain_pool_check(stream_id) )
      {
        if( stream_id == DIAG_STREAM_1 )
          diag_set_internal_sigs( DIAG_INT_DRAIN_DSM_BUFFER_SIG );
        else if( stream_id == DIAG_STREAM_2 )
          diag_set_internal_sigs( DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG );
      }
    }
  }
  }
} /* diag_drain_dsm_buffer */

/*===========================================================================
FUNCTION DIAGCOMM_DEQUEUE_BUFFERING_TX_WMQ

DESCRIPTION
  Dequeues a DSM from the SIO Buffering TX watermark queue
  
PARAMETERS
  wm_queue - Pointer to dsm_watermark_type

RETURN VALUE
  Pointer to DSM dequeued

===========================================================================*/
dsm_item_type * diagcomm_dequeue_buffering_tx_wmq( uint8 port_index )
{
    uint32 q_count=0;

    if( port_index > DIAGCOMM_PORT_NUM_LAST )
    {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_dequeue_buffering_tx_wmq - invalid port_index %d", port_index);
        return NULL;
    }
    if(diagcomm_sio_buffering_tx_wmq[port_index].q_ptr)
    {
        q_count = q_cnt( diagcomm_sio_buffering_tx_wmq[port_index].q_ptr );
    }
    if( q_count> 0 )
    {
        return dsm_dequeue( &diagcomm_sio_buffering_tx_wmq[port_index] );
    }
    return NULL;
    
} /* diagcomm_dequeue_buffering_tx_wmq */


/*===========================================================================
FUNCTION DIAG_CTRL_SEND_DRAIN_STATUS

DESCRIPTION
  Sends a buffering mode drain status to apps. This should be called to send 
  drain notification when a drain has completed for buffering mode.  This 
  should not be sent for streaming mode.
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
static void diag_ctrl_send_drain_status( void )
{
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  uint32 bytes_written = 0;
  
  /* Compose DIAG_CTRL_MSG_DRAIN_STATUS packet */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_DRAIN_STATUS;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_drain_status_type);
  ctrl_msg.data.ctrl_drain_status.version = DIAG_CTRL_MSG_DRAIN_STATUS_VER;
  ctrl_msg.data.ctrl_drain_status.peripheral_id = DIAG_CTRL_MY_PROC_ID;
  
  /* Send status to apps */
  bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_drain_status_type), 
                                                NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );
                                                        
  if( (is_sent == FALSE) || (bytes_written == 0) )
  {
    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Send drain status failed %d %d %d", DIAG_CTRL_MY_PROC_ID, bytes_written, is_sent);
  }
  
} /* diag_ctrl_send_drain_status */



