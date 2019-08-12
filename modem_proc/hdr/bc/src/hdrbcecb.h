#ifndef HDRBCECB_H
#define HDRBCECB_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            H D R B C E C B
                            
              E X T E R N A L   H E A D E R   F I L E                                           
                                         

GENERAL DESCRIPTION
   Error Control Block manager for the HDR MCS MAC protocol. 
   
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004, 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcecb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/05   etv     Fixed lint errors. 
02/02/05   etv     Fixed the coding style to match the convention.
08/19/04   kss     Minor cleanup; changed uint64 to uint16.
06/07/04   kss     Added support for ECB logging.
04/04/04   kss     Initial port.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdrlogi.h"
#include "dsm.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
   Defines states of the ECB object.  
      - ERROR          ECB is not configured correctly
      - COLLECTING     ECB still collecting data.
      - DONE           ECB is ready with data.
      - OUT_OF_SYNC    ECB out of sync, the ecb should be flushed and reset
---------------------------------------------------------------------------*/
typedef enum
{
   HDRBCECB_STATE_ERROR       = 0x00,
   HDRBCECB_STATE_COLLECTING  = 0x01,
   HDRBCECB_STATE_DONE        = 0x02,
   HDRBCECB_STATE_OUT_OF_SYNC = 0x03

} hdrbcecb_state;


#define HDRBCECB_MAX_PACKETS_PER_ECB (16 * 32)


/* This structure defines the Error Control Block used by the Reed-Solomon 
   decoder.  The structure is also used to group hdrbcmac_Packets. */
typedef struct
{
   
  uint32          flow_id;

  /* logical channel ID */
  uint8           chan_id;

  /* technically a 2-D array burried in 1-D array */
  dsm_item_type*  packets[ HDRBCECB_MAX_PACKETS_PER_ECB ];

  /* outer code for this ecb */
  uint8           outer_code;

  /* ecb state */
  hdrbcecb_state  state;

  /* number of 1000-bit payload packets per row */
  uint16          mac_packets_per_ecb_row;  

  /* last packet row received by ecb (int used so a -1 value can be 
     evaluated) */
  int16          last_row_received;

  /* last packet column received by ecb */
  int16          last_column_received;

  /* Test flags used to drop packets at a given ECB index.
     This is used to test reed-solomon recovery */
  uint32         drop_packet_at_ecb_index_mask;

  int16            last_pkt_received;
  uint16            block_size;

  /* For logging ECB status log packet */
  struct
  {
    uint8 data_erasures;
    uint8 total_erasures;

  } column_info[32];  /* max columns */

   /* class metrics storage */
  hdrlogbc_mac_metrics_type* metrics;

} hdrbcecb;


typedef struct
{

   dsm_item_type** packets;  /* array of packet pointers */

   uint16 count;

   uint16 total_packets;     /* total packets in stream at init */

} hdrbcecb_packet_array;


/*=============================================================================
                           FUNCTION PROTOTYPES
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
extern void hdrbcecb_initialize
( 
  hdrbcecb* ecb, 
    /* Error control block object */

  uint32 flow_id,
    /* flow id */

  uint8  chan_id,
    /* logical channel id */ 

  hdrlogbc_mac_metrics_type* metrics 
    /* pointer to metrics structure */
);


/*===========================================================================

FUNCTION HDRBCECB_CONFIGURE

DESCRIPTION
  Configures the ECB.  Set all fields required to reassemble and decode 
  the Error control block.  If a pre-configured ecb is passed to this function
  all packets in the ecb will be freeded (_Reset) during configuration.

DEPENDENCIES
  None.

RETURN VALUE
  - 1 entries are valid
  - 0 entries are not valid, ecb init failure, do not use ECB
  failure occurs when the maximum number of elements (row*col) in the 
  ecb is larger than what the ECB can handle in memory
  row * col <= hdrbcecb_MAX_ELEMENTS

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrbcecb_configure
( 
  hdrbcecb* ecb, 
    /* Error Control Block object */

  uint8 outerCode, 
    /* Reed-Solomon Outer Code associated with this ECB */

  uint16 macPacketsPerECBRow 
    /* number of MAC packets per ECB row */
);


/*===========================================================================

FUNCTION HDRBCECB_RESET

DESCRIPTION
  Resets the error control block to the just initialized state.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcecb_reset
(
  hdrbcecb* ecb 
    /* Error Control Block */
);

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
hdrbcecb_state hdrbcecb_add_packet( hdrbcecb* ecb,
   dsm_item_type* packet, uint16 number );

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
void hdrbcecb_flush( hdrbcecb* ecb );

/*===========================================================================

FUNCTION  HDRBCECB_PACKET_ARRAY_GET_NEXT_PACKET

DESCRIPTION
  Returns the next packet in the array
  
DEPENDENCIES
  None.

PARAMETERS
  array - packet array object. 

RETURN VALUE
  next packet in array or 0 when stream is empty
  
SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* hdrbcecb_packet_array_get_next_packet( 
   hdrbcecb_packet_array* stream );

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
void hdrbcecb_get_packet_array( hdrbcecb* ecb, 
   hdrbcecb_packet_array* parray );

/*===========================================================================

FUNCTION  HDRBCECB_SET_ECB_DROP_MASK

DESCRIPTION
  This sets the ECB Drop Mask.
  
DEPENDENCIES
  None.

PARAMETERS
  mask - uint32 the mask to be set.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcecb_set_ecb_drop_mask( uint32 mask );

#endif /* HDRBCECB_H */
