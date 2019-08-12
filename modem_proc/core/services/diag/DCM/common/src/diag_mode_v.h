#ifndef DIAG_MODE_V_H
#define DIAG_MODE_V_H
/*==========================================================================

              Diag APIs for buffering and streaming modes

General Description
  
Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diag_mode_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/14   ph      Fixed inconsistent drain behavior in circular and threshold buffering.
08/31/13   is      Support for peripheral buffering mode

===========================================================================*/

/* Tx Modes */
#define DIAG_TX_MODE_STREAMING         0    /* Streaming mode (default) */
#define DIAG_TX_MODE_BUFFERED_THRESH   1    /* Threshold buffering mode */
#define DIAG_TX_MODE_BUFFERED_CIR      2    /* Circular buffering mode */
#define DIAG_TX_MODE_INVALID           0xFF /* Invalid mode */
#define DIAG_TX_MODE_LAST              2


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
boolean diag_buffered_mode_stream( uint8 stream_id );

/*===========================================================================

FUNCTION DIAG_TX_MODE_INIT (internal)

DESCRIPTION
  Initializes global variable for keeping track of streaming mode vs buffering mode.

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_tx_mode_init( void );

/*===========================================================================
FUNCTION DIAG_DRAIN_DSM_BUFFER (internal)

DESCRIPTION
  This function drains the buffering DSM pools per stream. 
  
PARAMETERS
  stream_id - stream_id for buffering DSM pool to drain.
              Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_drain_dsm_buffer( uint8 stream_id );

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
uint32 diag_buffering_pool_used_cnt( uint8 stream_id );

/*===========================================================================

FUNCTION DIAG_DRAIN_POOL_CHECK

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
boolean diag_drain_pool_check( uint8 stream_id );

/*===========================================================================
FUNCTION DIAG_SET_BUFFERING_BUFFER

DESCRIPTION
  Changes the buffer used for peripheral buffering.  New DSM pool will be
  allocated from the memory location passed in through this function.
  
PARAMETERS
  buf_ptr - Pointer to buffer to be used for peripheral buffering mode.
  length - Length, in bytes, of buf_ptr.
  stream_id - Stream id to set buffer for
  
RETURN VALUE
  TRUE - If buffer used for peripheral buffering was successfully changed.
  FALSE - Otherwise.
  
===========================================================================*/
  boolean diag_set_buffering_buffer( uint8 * buf_ptr, uint32 length, uint8 stream_id ); 
  
/*===========================================================================
FUNCTION DIAGCOMM_DEQUEUE_BUFFERING_TX_WMQ

DESCRIPTION
  Dequeues a DSM from the SIO Buffering TX watermark queue
  
PARAMETERS
  wm_queue - Pointer to dsm_watermark_type  

RETURN VALUE
  Pointer to DSM dequeued  

===========================================================================*/  
  dsm_item_type * diagcomm_dequeue_buffering_tx_wmq( uint8 port_index );
  
#endif /*DIAG_MODE_V_H*/