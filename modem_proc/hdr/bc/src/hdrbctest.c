/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R   B R O A D C A S T   L O G I C A L 
                
                        T E S T   M O D E   A P P
       
                  E X T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the external interface of the HDR Broadcast 
  Test Mode app functionality.
  
EXTERNALIZED FUNCTIONS
  hdrbctest_init
    Initialize the test mode application.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The application must be initialized after system startup.
  
  Copyright (c) 2003,2004,2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbctest.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/08   smd     Replaced assert.h with amssassert.h
02/12/08   kss     Fixed lint error.
02/23/07   rmg     Fixed lint errors.
02/13/06   mpa     Clean up test flow list when enable/disable cmd fails.
08/28/05   mpa     Use test flow only interface for enabling/disabling flows.
02/10/05   etv     Fixed lint errors.
02/02/05   etv     Fixed coding style to match convention.
11/10/04   kss     Use unsigned format in F3.
11/01/04   kss     Increased supported number of test flow instances.
08/04/04   kss     Use hdrbccp interface.
07/14/04   kss     Fixed potential dsm leak.
06/14/04   ss      (mpa for) Use bctask flow enable/disable instead of bccp
06/07/04   kss     Changed interface to use dsm_items.
04/25/04   kss     Ported from QC Corp.

===========================================================================*/


/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "amssassert.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrbctest.h"
#include "hdrdebug.h"  
#include "hdrbccp.h"

/* Metrics maintained for each flow */  
typedef struct
{  
   /* Packets processed */
   uint32 packets_received;
   
   /* Packet received in a row */
   uint32 packets_in_sequence;

   /* Count of missing expected packets */
   uint32 packets_missing;

} hdrbctest_metrics;

/* Maximum concurrent test app instances */
#define HDRBCTEST_MAX_INSTANCES 10

/* Minimum size of test packet */
#define HDRBCTEST_PKT_MIN_SIZE  8


/* Status of each instance of the test mode app */
struct
{

  /* Active flag for each instance */
  boolean active;

  /* flow ID */
  uint32 flow_id;

  /* 1st frame indicator, helps with initial state */
  uint8 got_first_frame;

  /* current sequence number */
  uint32 last_sequence;

  /* Metrics */
  hdrbctest_metrics metrics;

} hdrbctest[HDRBCTEST_MAX_INSTANCES];
  


 /* EJECT */
/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION HDRBCTEST_INIT

DESCRIPTION
  This function initializes the test mode app prior to use.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_init( void )
{
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<HDRBCTEST_MAX_INSTANCES;i++)
  {
    hdrbctest[i].flow_id = 0;
    hdrbctest[i].active = FALSE;
  }

} /* hdrbctest_init */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCTEST_PROCESS_PACKET

DESCRIPTION
  The function processes BSC test mode packets.  
  
DEPENDENCIES
  None.

PARAMETERS
  data     - A pointer to the packet data
  length   - length of the data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_process_packet
( 
  dsm_item_type *data_pkt
)
{

  uint32 packed_channel_id;
  uint32 current_sequence;
  static uint32 debug_counter = 0;
  static uint32 debug_counter_mod = 0;
  boolean flow_found = FALSE;
  int i;
  int length;
  byte data[HDRBCTEST_PKT_MIN_SIZE];
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  length = dsm_length_packet(data_pkt);

  if (length >= HDRBCTEST_PKT_MIN_SIZE) 
  {
    
    /* Copy out flow ID and sequence number */
    (void) dsm_extract(data_pkt,0,data,8);
  
    debug_counter++;
    debug_counter_mod = debug_counter % 100;

    /* Get flow id */
    packed_channel_id  = data[0] << 24;
    packed_channel_id |= data[1] << 16;
    packed_channel_id |= data[2] << 8;
    packed_channel_id |= data[3] ;

  
    /* If this the channel we want? */
    for (i=0; i<HDRBCTEST_MAX_INSTANCES;i++)
    {
      if ((hdrbctest[i].flow_id == packed_channel_id) &&
         (hdrbctest[i].active == TRUE))
      {
        flow_found = TRUE;
        break;
      }
    }

    /* Redundant check on i makes LINT happy */
    if ((flow_found == FALSE) || (i >= HDRBCTEST_MAX_INSTANCES))
    {
      /* got a frame for the wrong (or inactive) channel */
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                   "HDR BC test received unexpected packet (flow %u).",
                    packed_channel_id);
    }  
    else
    {
      
      /* update metrics */
      hdrbctest[i].metrics.packets_received++;

      /* Get sequence number */
      current_sequence = data[4] << 24;
      current_sequence |= data[5] << 16;
      current_sequence |= data[6] <<  8;
      current_sequence |= data[7];

      if ( hdrbctest[i].got_first_frame == 0 )
      {
        /* init last sequence, don't process sequence numbers. */
        hdrbctest[i].got_first_frame = 1;
      }
      else
      {
        uint32 exp_sequence = hdrbctest[i].last_sequence + 1;
        if ( exp_sequence != current_sequence )
        {
          HDR_MSG_PROT_4(MSG_LEGACY_HIGH,
             "HDR MCS Test flow %u: Missed pkt (Last[%u] Exp[%u] Rxd[%u])",
                       hdrbctest[i].flow_id,
                       hdrbctest[i].last_sequence,
                       exp_sequence, current_sequence );

          /* didn't get the next sequence - compute number of missing packets */
          hdrbctest[i].metrics.packets_missing += 
            (current_sequence - exp_sequence);
        }
        else
        {
          hdrbctest[i].metrics.packets_in_sequence++;

          if (debug_counter_mod == 0)
          {
            HDR_MSG_PROT_4(MSG_LEGACY_HIGH,
              "HDR MCS Test flow %u stats: Rx'd %u, Missed %u (Length %u)", 
                 packed_channel_id, 
                 hdrbctest[i].metrics.packets_received,
                 hdrbctest[i].metrics.packets_missing, 
                 length);
          }

        }
      }

      hdrbctest[i].last_sequence = current_sequence;
    }

  }

  /* Free dsm item */
  dsm_free_packet(&data_pkt);

} /* hdrbctest_process_packet */ 


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCTEST_ENABLE

DESCRIPTION
  The function enables a test mode flow over the HDR BCMCS air interface.
  Test mode packets must be transmitted on the requested flow.
  
DEPENDENCIES
  - Test Flows are only allowed when no real app. flows are desired.
  - Test Flows may only be enabled when HDR is active.

PARAMETERS
  flow_id - The desired flow that carries test mode packets.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_enable
( 
  uint32 flow_id
)
{
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<HDRBCTEST_MAX_INSTANCES; i++)
  {
    if ( (hdrbctest[i].flow_id == flow_id) &&
         (hdrbctest[i].active == TRUE) )
    {
      /* Already active */
      HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH,
                       "HDR MCS test mode already active on flow %u",
                       flow_id );

      return;
    }
  }

  for (i=0; i<HDRBCTEST_MAX_INSTANCES;i++)
  {
    if (hdrbctest[i].active == FALSE)
    {
      /* Activate */
      hdrbctest[i].active = TRUE;
      hdrbctest[i].flow_id = flow_id;
      hdrbctest[i].got_first_frame =  0;

      memset( &hdrbctest[i].metrics, 0, sizeof(hdrbctest_metrics) );

      HDR_MSG_PROT_2 ( MSG_LEGACY_HIGH,
                       "HDR MCS activating test mode (flow %u, instance %u)",
                       flow_id, 
                       i );

      /* Send Enable Test Flow Cmd */
      if ( hdrbccp_test_flow_enable(flow_id, 
                                    hdrbctest_process_packet,
                                    HDRHAI_BROADCAST_PROTOCOL) != E_SUCCESS )
      {
        /* Could not enable test flow, free up entry from the test flow list */
        hdrbctest[i].active = FALSE;
        hdrbctest[i].flow_id = 0;
      }
      break;
    }
  }

  if (i==HDRBCTEST_MAX_INSTANCES)
  {
    HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH,
                     "No more test mode instances available (%u active)",
                     i );
               
  }
  
} /* hdrbctest_enable */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCTEST_DISABLE

DESCRIPTION
  The function disables interest in a test mode flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow to disable.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_disable
( 
  uint32 flow_id
)
{
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<HDRBCTEST_MAX_INSTANCES;i++)
  {
    if ((hdrbctest[i].flow_id == flow_id) &&
       (hdrbctest[i].active == TRUE))
    {

      /* Send Disable Test flow Command */
      if ( hdrbccp_test_flow_disable(flow_id,
                                     HDRHAI_BROADCAST_PROTOCOL) == E_SUCCESS)
      {
        hdrbctest[i].active = FALSE;

        /* Already active */
        HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                       "HDR MCS test mode disable on flow %u",
                       flow_id);
      }
      break;
    }
  }  

} /* hdrbctest_disable */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCTEST_RESET_METRICS

DESCRIPTION
  This function clears metrics for the specified flow. 
    
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow whose metrics we are going to clear.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctest_reset_metrics
( 
  uint32 flow_id
)
{

  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<HDRBCTEST_MAX_INSTANCES;i++)
  {
    if (hdrbctest[i].flow_id == flow_id)
    {
      memset( &hdrbctest[i].metrics, 0, sizeof(hdrbctest_metrics) );
      break;
    }
  } 

} /* hdrbctest_reset_metrics */

#endif /* FEATURE_HDR_BCMCS */

