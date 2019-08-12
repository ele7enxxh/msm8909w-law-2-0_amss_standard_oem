/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  H D R   B R O A D C A S T   M A C   
                  
                          P R O T O C O L 
                  
                E R R O C   C O N T R O L   B L O C K
                
                           M A N A G E R 
                            

GENERAL DESCRIPTION
   Error Control Block manager for the HDR MCS MAC protocol. 
   
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcecb.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/04/13   sat     Removed unused parameters in F3s.
03/17/10   wsh     Included amssassert.h 
03/15/10   rmg     Klockwork fix: out-of-bounds access of decoder_packet[].
02/10/05   etv     Fixed lint errors. 
02/02/05   etv     Fixed the coding style to match the convention.
08/19/04   kss     Minor cleanup; changed uint64 to uint16.
07/14/04   kss     Removed bogus error message.
06/07/04   kss     Added support for ECB logging.
04/04/04   kss     Initial port.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrbcecb.h"
#include "hdrbcrsd.h"
#include "hdrdebug.h"
#include "amssassert.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* For debug/test, mask for droping packets before doing RS decoding */
LOCAL uint32 hdrbcecb_drop_mask = 0;

/*=============================================================================
       Static Variable Definitions
=============================================================================*/

/* K, N, R  lookup tables, index using ATBROADCAST_OUTERCODES */
LOCAL uint8 hdrbcecb_get_n_rows_table[] = { 1, 16, 16, 16, 32, 32, 32};
LOCAL uint8 hdrbcecb_get_k_rows_table[] = { 1, 12, 13, 14, 24, 26, 28};
LOCAL uint8 hdrbcecb_get_r_rows_table[] = { 0,  4,  3,  2,  8,  6,  4};


/*=============================================================================
      Static Function Prototypes
=============================================================================*/

LOCAL uint8 hdrbcecb_rsdecode( hdrbcecb* ecb );

LOCAL dsm_item_type* hdrbcecb_get_packet_at_index( hdrbcecb* ecb, 
                                                    uint16 row, 
                                                    uint16 col );

LOCAL void hdrbcecb_set_packet_at_index( hdrbcecb* ecb, dsm_item_type* packet, 
                                          uint16 row, uint16 col );

LOCAL uint16 hdrbcecb_get_packet_row( hdrbcecb* ecb, uint16 number );

LOCAL uint16 hdrbcecb_get_packet_column( hdrbcecb* ecb, uint16 number );

LOCAL void hdrbcecb_induce_dropped_packets( hdrbcecb* ecb );

LOCAL void hdrbcecb_packet_array_initialize ( hdrbcecb_packet_array* stream, 
                                              dsm_item_type* array[], 
                                              uint16 count );

LOCAL uint8 hdrbcecb_get_k_rows( uint8 outer_code );

LOCAL uint8 hdrbcecb_get_n_rows( uint8 outer_code );

LOCAL uint8 hdrbcecb_get_r_rows( uint8 outer_code );

/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
 
FUNCTION HDRBCECB_INITIALIZE

DESCRIPTION
  Initializes the error control block.  Should be called prior to calling
  any other function in the class.  Sets the ECB state to ERROR.

DEPENDENCIES
  None.

PARAMETERS
  ecb - error control block object
  flow_id - flow id
  chan_id - logical channel id
  metrics - pointer to hdrlogbc_mac_metrics_type structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcecb_initialize
( 
  hdrbcecb* ecb, 
    /* Error control block object */

  uint32 flow_id,
    /* flow id */

  uint8  chan_id,
    /* logical channel id */ 

  hdrlogbc_mac_metrics_type* metrics 
    /* pointer to metrics structure */
)
{
  /* Initialize block. */ 
  ecb->flow_id = flow_id;
  ecb->chan_id = chan_id;
  ecb->state = HDRBCECB_STATE_ERROR;
  ecb->metrics = metrics;
  ecb->last_column_received = -1;
  ecb->last_row_received = -1;

  /* This block now needs to be configured to be used. */
} /* hdrbcecb_initialize */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCECB_CONFIGURE

DESCRIPTION
  Configures the ECB.  Set all fields required to reassemble and decode 
  the Error control block.  If a pre-configured ecb is passed to this function
  all packets in the ecb will be freeded (_Reset) during configuration.

DEPENDENCIES
  None.

PARAMETERS
  ecb - Error control block object
  outer_code - Reed solomon outer code associated with this ECB.
  mac_packets_per_ecb_row - Number of MAC packets per ECB row.

RETURN VALUE
  - 1 entries are valid
  - 0 entries are not valid, ecb init failure, do not use ECB
  failure occurs when the maximum number of elements (row*col) in the 
  ecb is larger than what the ECB can handle in memory
  row * col <= hdrbcecb_MAX_ELEMENTS

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrbcecb_configure
( 
  hdrbcecb* ecb, 
    /* Error Control Block object */

  uint8 outer_code, 
    /* Reed-Solomon Outer Code associated with this ECB */

  uint16 mac_packets_per_ecb_row 
    /* number of MAC packets per ECB row */
)
{
  uint8 rc = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_4(MSG_LEGACY_HIGH," _Configure ecb(state:%d) chId:%d oc:%d mppr:%d ", 
                 ecb->state, ecb->flow_id, outer_code,mac_packets_per_ecb_row );

  if ( ecb->state != HDRBCECB_STATE_ERROR )
  {
    /* this ecb is configured properly and is in use, we need to free it's 
       packets */
    hdrbcecb_reset( ecb );
  }

  if ( hdrbcecb_get_n_rows( outer_code ) * mac_packets_per_ecb_row > 
       HDRBCECB_MAX_PACKETS_PER_ECB )
  {
    /* row * cols larger than packet array structure, return error */
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH," INVALID config too many pktsPerRow for outer_code(%d)",
                   ( outer_code ));
    ecb->state = HDRBCECB_STATE_ERROR;
  }
  else
  {
    /* configure the ecb */
    ecb->outer_code = outer_code;
    ecb->mac_packets_per_ecb_row = mac_packets_per_ecb_row;
    ecb->block_size = (uint16) ( hdrbcecb_get_n_rows( outer_code ) * 
                                 mac_packets_per_ecb_row );

    if ( ecb->state == HDRBCECB_STATE_ERROR )
    {
      /* reset the ill configured ECB */
      hdrbcecb_reset( ecb );
    }

    rc = 1;
  }

  return rc;

} /* hdrbcecb_configure */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_RESET

DESCRIPTION
  Resets the error control block to the just initialized state.
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - Error control block.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcecb_reset
(
  hdrbcecb* ecb 
    /* Error Control Block */
)
{
  dsm_item_type* packet = NULL;
  uint16 row;
  uint16 col;
  uint16 total_rows = hdrbcecb_get_n_rows( ecb->outer_code );
  uint16 total_cols = ecb->mac_packets_per_ecb_row;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_2(MSG_LEGACY_HIGH," ecb_reset rows:%d cols:%d",total_rows, total_cols);

  ecb->state = HDRBCECB_STATE_COLLECTING;
  ecb->last_column_received = -1;
  ecb->last_row_received = -1;

  /* free all packets in this object */
  for ( row=0; row<total_rows; row++ )
  {
    for ( col=0; col<total_cols; col++ )
    {
      packet = hdrbcecb_get_packet_at_index( ecb, row, col );

      if ( packet )
      {
        dsm_free_packet(&packet);
        hdrbcecb_set_packet_at_index( ecb, 0, row, col );
      }
    }
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH," ResetComplete");

} /* hdrbcecb_reset */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCECB_ADD_PACKET

DESCRIPTION
  Adds a packet/row to the Error Control Block. When the last packet required
  to complete the error control block is received, the Reed-Solomon decoder 
  will automatically start decoding the contents of the packet.
  
DEPENDENCIES
  None.

PARAMETERS 
  ecb - Error Control Block.
  packet - Physical layer packet(row) to add to this ECB.
  
RETURN VALUE
  COLLECTING  - ECB still waiting for more data.
  DONE        - ECB is ready with valid data.
  OUT_OF_SYNC - ECB out of sync, the ecb should be flushed.
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrbcecb_state hdrbcecb_add_packet
( 
  hdrbcecb* ecb,
  dsm_item_type* packet,
  uint16 number 
)
{
  int16 row, col;
  int curr_index, prev_index;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ecb->state == HDRBCECB_STATE_COLLECTING )
  {
    
    /* determine row and column in ECB for packet based on timestamp */
    row = (int16) hdrbcecb_get_packet_row( ecb, number );
    col = (int16) hdrbcecb_get_packet_column( ecb, number );
    HDR_MSG_PROT_4(MSG_LEGACY_MED,"row(%d) col(%d) lastr(%d) lastc(%d)",
                   row,col,ecb->last_row_received, ecb->last_column_received );

    curr_index = (row * ecb->mac_packets_per_ecb_row) + col;
    prev_index = (ecb->last_row_received * ecb->mac_packets_per_ecb_row) + ecb->last_column_received;

    if ( curr_index <= prev_index )
    {

      /* the new packet is out of sync with this error control block
         inform the user to flush this packet and try again
         we don't use timestamps to determine this because it's very 
         computationaly intensive to determine the start time of an 
         ECB.
         NOTE: the packet is not processed */
      ecb->state = HDRBCECB_STATE_OUT_OF_SYNC;
      HDR_MSG_PROT(MSG_LEGACY_ERROR,"ECB Out of Sync");
    }
    else
    {

      /* set the packet at the appropriate row and column index in the ECB. */
      hdrbcecb_set_packet_at_index( ecb, packet, (uint16) row, (uint16) col );
      ecb->last_row_received = row;
      ecb->last_column_received = col;

      /* did we get the last packet for this ECB? */
      /* is it in the max row? */
      if ( row == (hdrbcecb_get_n_rows( ecb->outer_code )-1) &&   

           /* max column? */
           col == (ecb->mac_packets_per_ecb_row - 1) )                 
      {
        /* we are done with this ecb, start reed-solomon decode */
        ecb->state = HDRBCECB_STATE_DONE;  /* done waiting for data */
        HDR_MSG_PROT(MSG_LEGACY_HIGH,"ECB Done");
      }
    }
  }
  else
  {
    /* free packet for unknown ecb state */
    dsm_free_packet( &packet );
  }
  
  return ecb->state;

} /* hdrbcecb_add_packet */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCECB_FLUSH

DESCRIPTION
  Runs the Reed-Solomon decoding on the packets contained within this ECB.
  This function can be used to early decode the ECB (not all packet 
  received).
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - Error Control Block

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcecb_flush( hdrbcecb* ecb )
{
  if ( hdrbcecb_drop_mask )
  {
    /* simulate dropped packets */
    hdrbcecb_induce_dropped_packets( ecb );
  }

  /* not all CRCs for this ECB are good, try reed solomon decoding it */
  (void) hdrbcecb_rsdecode( ecb );


  HDR_MSG_PROT_1(MSG_LEGACY_HIGH," HDRBCECB_STATE_DONE channel(%u)",
                 ecb->flow_id);
  ecb->state = HDRBCECB_STATE_DONE;

} /* hdrbcecb_flush */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCECB_GET_PACKET_ARRAY 

DESCRIPTION
  Returns the packet stream associated with this ECB.  The ECB is no
  longer valid and should not be used until Reset.  Note: The ECB
  should be reset after the user of the stream is done with the content.
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - Error Contro Object
  parray - packet array structure to initialize

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcecb_get_packet_array
( 
  hdrbcecb* ecb, 
  hdrbcecb_packet_array* parray 
)
{

  hdrbcecb_packet_array_initialize( parray, 
                                    ecb->packets,
                                    (uint16)( hdrbcecb_get_k_rows(ecb->outer_code) * 
                                      ecb->mac_packets_per_ecb_row ) );

} /* hdrbcecb_get_packet_array */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_RSDECODE

DESCRIPTION
  Does the Reed Solomon decoding.
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - error control block of interest

RETURN VALUE
  < fill out >
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint8 hdrbcecb_rsdecode( hdrbcecb* ecb )
{
  uint16 packet_per_row = ecb->mac_packets_per_ecb_row;
  uint8 rc = 1;
  uint8 row;
  uint8 packet_failures = 0;           /* CRC failures and missing packets */
  uint8 data_failures = 0;             /* CRC failures and missing packets */
  uint8 n = hdrbcecb_get_n_rows( ecb->outer_code );
  uint8 k = hdrbcecb_get_k_rows( ecb->outer_code );
  uint16 col;

  LOG_HDR_MCS_ECB_STATUS_C_type* log_ptr = NULL;  /* Logging */
  int i;                              /* Loop for logging */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* since we only process one MAC column at a time, we copy the packet
     pointers to an array and pass the array to the rs decoder. */
  dsm_item_type* decoder_packet[32]; /* max rows can only be 32.. */

  /* Initialize the decoder_packet* Array to all NULL */
  memset(decoder_packet,0,sizeof(dsm_item_type*)*32);

  /* Error check to prevent out-of-bounds access of decoder_packet[]. */
  ASSERT( n <= 32 );

  if ( ecb->outer_code == 0 )
  {
    /* no decoding required for this outer code */
  }
  else
  {
    /* we need to treat ONLY one 125-byte/symbol wide column at a time */
    for ( col=0; col<packet_per_row; col++ )
    {

      packet_failures = 0;
      data_failures = 0;

      /* count the missing/corrupted packets in each row */
      for ( row=0; row<n; row++ )
      {
        /* copy packet pointers into our ONE column array */
        decoder_packet[row] = hdrbcecb_get_packet_at_index( ecb, row, col );

        if ( decoder_packet[row] == 0 )
        {
          packet_failures++;

          if (row<k) {
            data_failures++;
          }

        }

      }

      ecb->column_info[col].total_erasures = packet_failures;
      ecb->column_info[col].data_erasures = data_failures;

      {
        if (  packet_failures == 0 )
        {
          /* everything is fine, no decoding required */
          ecb->metrics->ecb_no_erasures++;
          rc = 1;
        }
        else if ( packet_failures > n-k )
        {
          /* too many corrupted packets for reed-solomon to correct */
          ecb->metrics->ecb_decode_abort++;
          HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                       "RSD - too many errors (%d)", packet_failures);
          rc = 0;
        }
        else if ( packet_failures <= n-k )
        {
          HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "RSD, outer %d, flow %d",
                           ecb->outer_code, ecb->flow_id);

          /* packets errors are within limits, start decoding
             but 1st, we must fill in all missing packets in the array with 
             packets so the decoder has a packet buffer to write into */
          rc = hdrbcrsd_decode( decoder_packet, ecb->outer_code );

          /* Copy pointers back to main ECB block. */
          for ( row=0; row<n; row++ )
          {
            ecb->packets[ (row * ecb->mac_packets_per_ecb_row) + col ] 
                = decoder_packet[row];
          }

          if (rc == 0)
          {
            HDR_MSG_PROT(MSG_LEGACY_MED,"R-S Decode: No packets decoded."); 
          }
          else
          {
            HDR_MSG_PROT(MSG_LEGACY_MED,"R-S Decode complete" );
          }

          /* For the time being consider this as Decode pass and set rc = 1
             No other metric to capture this condition, it should be very rare
             any way. */
          ecb->metrics->ecb_decode_pass++;
          rc = 1;

        } /* can do rs decoding */
      }

    }

    /* --- ECB STATUS LOG --- */

    /* Don't log without checking for log_mask... */
    if ( log_status( LOG_HDR_MCS_ECB_STATUS_C ) )
    { 

      log_ptr = (LOG_HDR_MCS_ECB_STATUS_C_type *)
            log_alloc( (log_code_type) LOG_HDR_MCS_ECB_STATUS_C,
                        sizeof(LOG_HDR_MCS_ECB_STATUS_C_type)
                        + (sizeof(uint8) * (packet_per_row-1)*2)
                        );

      if (log_ptr)
      {
        log_ptr->channel_id = ecb->chan_id; 
        log_ptr->flow_id = ecb->flow_id;
        log_ptr->outer_code = ecb->outer_code;
        log_ptr->columns_per_ecb = (uint8) packet_per_row; 
        log_ptr->sec_pkt_included = FALSE; //temp

        for (i=0; i < packet_per_row; i++) 
        {
          log_ptr->column[i].data_erasures = 
            ecb->column_info[i].data_erasures;
          log_ptr->column[i].total_erasures = 
            ecb->column_info[i].total_erasures;
        
        }

        log_commit((log_type*) log_ptr);
      }
    }

  }

  return rc;

} /* hdrbcecb_rsdecode */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_SET_PACKET_AT_INDEX

DESCRIPTION
  Sets the packet at the specified row and column in the ecb packet array.
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - error control block object 
  packet - packet of interest
  row - row of interest
  column - column of interest.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrbcecb_set_packet_at_index
( 
  hdrbcecb* ecb, 
  dsm_item_type* packet, 
  uint16 row, 
  uint16 col 
)
{
  if ( ( packet!=0 ) && 
       ( ecb->packets[ (row * ecb->mac_packets_per_ecb_row) + col ] ) )
  {
    HDR_MSG_PROT(MSG_LEGACY_FATAL,"Hdrtwist...hdrbcecb_set_packet_at_index Memory leak???");
  }

  /* we need to convert a 2 dimensional cooridinate to 1-d space */
  ecb->packets[ (row * ecb->mac_packets_per_ecb_row) + col ] = packet;

} /* hdrbcecb_set_packet_at_index */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_GET_PACKET_AT_INDEX

DESCRIPTION
  Returns the packet at a specified row and column in the ECB packet array.
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - error control block object
  row - row of interest
  column - column of interest.

RETURN VALUE
  dsm item pointer containing the specified packet.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL dsm_item_type* hdrbcecb_get_packet_at_index
( 
  hdrbcecb* ecb, 
  uint16 row, 
  uint16 col 
)
{
  /* we need to convert a 2 dimensional cooridinate to 1-d space */
  return ecb->packets[ (row * ecb->mac_packets_per_ecb_row) + col ];

} /* hdrbcecb_get_packet_at_index */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_GET_K_ROWS

DESCRIPTION
  Returns the number of data rows used by the Reed Solomon Decoder. 
  
DEPENDENCIES
  None.

PARAMETERS
  outer_code - outer code associated with this ECB.

RETURN VALUE
  number of rows.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint8 hdrbcecb_get_k_rows( uint8 outer_code )
{
  return hdrbcecb_get_k_rows_table[ outer_code ];

} /* hdrbcecb_get_k_rows */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_GET_R_ROWS

DESCRIPTION
  Returns the number of Parity rows used by the Reed Solomon Decoder. 
  
DEPENDENCIES
  None.

PARAMETERS
  outer-code - outer code associated with this ECB.

RETURN VALUE
  number of parity rows.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint8 hdrbcecb_get_r_rows( uint8 outer_code )
{
  return hdrbcecb_get_r_rows_table[ outer_code ];

} /* hdrbcecb_get_r_rows */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_GET_N_ROWS

DESCRIPTION
  Returns the total number of rows used by the Reed Solomon Decoder.
  
DEPENDENCIES
  None.

PARAMETERS
  outer-code - outer code associated with this ECB.

RETURN VALUE
  number of rows.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint8 hdrbcecb_get_n_rows( uint8 outer_code )
{
  return hdrbcecb_get_n_rows_table[ outer_code ];

} /* hdrbcecb_get_n_rows */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_GET_PACKET_ROW

DESCRIPTION
  < fill out>
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - error control block.
  number - 

RETURN VALUE
  row 
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint16 hdrbcecb_get_packet_row( hdrbcecb* ecb, uint16 number )
{
  uint16 row = 0;
  uint16 col;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ecb->outer_code != 0 )
  {
    /* determine row and set to number of rows in ECB */
    row = hdrbcecb_get_n_rows( ecb->outer_code ); 
    col = ecb->mac_packets_per_ecb_row;
    row = (uint16)( ( number % (  row * col ) ) / col );
  }

  return row; 

} /* hdrbcecb_get_packet_row */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_GET_PACKET_COLUMN

DESCRIPTION
  < fill out>
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - error control  block
  number - 

RETURN VALUE
  column 
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint16 hdrbcecb_get_packet_column( hdrbcecb* ecb, uint16 number )
{
  uint16 col = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ecb->mac_packets_per_ecb_row == 1 )
  {
    col = 0;
  }
  
  else
  {
    /* 0 or 1 depending on time */
    col = ecb->mac_packets_per_ecb_row;  /* stick in temp var */
    col = (uint16) (( number % 
                   ( hdrbcecb_get_n_rows( ecb->outer_code ) * col ) ) % col );
  }

  return col;

} /* hdrbcecb_get_packet_column */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_INDUCE_DROPPED_PACKETS 

DESCRIPTION
  < fill out>
  
DEPENDENCIES
  None.

PARAMETERS
  ecb - error control block of interest.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrbcecb_induce_dropped_packets( hdrbcecb* ecb )
{
  uint8 col, row;
  uint16 max_cols = ecb->mac_packets_per_ecb_row;
  uint8 n = hdrbcecb_get_n_rows( ecb->outer_code );
  uint16 index;
  uint32 index_mask = 0x1;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* for each column... */
  for ( col=0; col<max_cols; col++ )
  {
    index_mask = 1;

    for ( row=0; row<n; row++ )
    {
      if ( index_mask & hdrbcecb_drop_mask )
      {
        /* packets need to be dropped at this index */
        index = (uint8) (row * ecb->mac_packets_per_ecb_row) + col;
        if ( ecb->packets[ index ] )
        {
          /* drop this packet */
          (void) dsm_free_buffer(ecb->packets[index]);
          hdrbcecb_set_packet_at_index( ecb, 0, row, col );
        }
      }

      /* shift mask to next bit position */
      index_mask <<=1;
    }
  }

} /* hdrbcecb_induce_dropped_packets */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_SET_ECB_DROP_MASK

DESCRIPTION
  This sets the ECB drop mask.
  
DEPENDENCIES
  None.

PARAMETERS
  mask - uint32 mask  to be set.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcecb_set_ecb_drop_mask(uint32 mask)
{
  hdrbcecb_drop_mask = mask;

} /* hdrbcecb_set_ecb_drop_mask */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_PACKET_ARRAY_INITIALIZE

DESCRIPTION
  Initializes the packet array( sets the size and content ).
  A object used to read packets out of an array of packets. 
  
DEPENDENCIES
  None.

PARAMETERS
  stream - object to initialize.
  array - source array of packets.
  count - number of packets in array.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrbcecb_packet_array_initialize
( 
  hdrbcecb_packet_array* stream, 
  dsm_item_type* array[], 
  uint16 count 
)
{
  stream->packets = array;
  stream->count = count;
  stream->total_packets = count;

} /* hdrbcecb_packet_array_initialize */


 /* EJECT */
/*===========================================================================
 
FUNCTION HDRBCECB_PACKET_ARRAY_GET_NEXT_PACKET

DESCRIPTION
  Returns the next packet in the array.
  
DEPENDENCIES
  None.

PARAMETERS
  array - BCMAC packet array.

RETURN VALUE
  Next packet in array or 0 when packet is empty.
  
SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* hdrbcecb_packet_array_get_next_packet
(
  hdrbcecb_packet_array* array
)
{

  dsm_item_type* packet = 0;

  /* find stream end. make sure we didn't leave the bounds the array */
  if ( array->count > 0 )
  {
    packet = array->packets[ array->total_packets - array->count ];

    /* kss new */
    array->packets[ array->total_packets - array->count ] = NULL;
    array->count -= 1;
  }

  return packet;

} /* hdrbcecb_packet_array_get_next_packet */

#endif /* FEATURE_HDR_BCMCS */
