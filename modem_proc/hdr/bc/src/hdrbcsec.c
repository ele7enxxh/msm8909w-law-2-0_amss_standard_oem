/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R    B R O A D C A S T    S E C U R I T Y
       
                             P R O T O C O L       

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Broadcast Security
  Protocol defined in C.S0054.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol. 
  
  Copyright (c) 2002 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcsec.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/02/09   wsh     Include amssassert.h when ASSERT is used
09/30/09   wsh     Fixed KW warnings
02/10/05   etv     Fixed Lint errors.
02/02/05   etv     Fixed coding style to match convention.
08/16/04   kss     Use flow_id in hdrbcmac APIs.
07/14/04   kss     Support for using dsm items.
07/08/04   kss     Added include of hdrbcmaci.h.
06/07/04   kss     Added FEATURE_HDR_BCMCS.
04/26/04   kss     Ported from corporate.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "amssassert.h"
#include "customer.h"
#include "dsm.h"
#ifdef FEATURE_HDR_BCMCS 
#include "hdrbcsec.h"
#include "hdrbcmac.h"
#include "hdrbcmaci.h"
#include "hdrbcframe.h"
#include "hdrdebug.h"


/*===========================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

struct
{
  uint32  flow_id;  
    /* First flow ID associated with this channel -- for reference only */

  int     ref_count;
    /* Reference count. Number of flows using this channel (?) */

  hdrlogbc_sec_metrics_type chan_metrics;
    /* Metrics for this protocol */

} hdrbcsec[HDRBCMAC_MAX_LOGICAL_CHANNELS];



/* <EJECT> */ 
/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION HDRBCSEC_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcsec_init( void )
{
  int i;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<HDRBCMAC_MAX_LOGICAL_CHANNELS; i++)
  {
    /* Clear reference count to indicate entry is inactive */
    hdrbcsec[i].ref_count=0;
  }

} /* hdrbcsec_init */


 /* EJECT */            
/*===========================================================================

FUNCTION HDRBCSEC_PUT_PKT

DESCRIPTION
  The function receives HDR BCMCS security layer packets and passes them up 
  to the framing layer.
  
DEPENDENCIES
  None.

PARAMETERS
  data     - A pointer to the packet data
  length   - length of the data
  chan_idx - the channel the data arrived on.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcsec_put_pkt
( 
  dsm_item_type* data_pkt, /* Pointer to the data pkt       */
  uint16 length,           /* Packet length                 */
  int chan_idx             /* The channel this packet arrived on */
)
{ 

  /* Do some accounting... */
  hdrbcsec[chan_idx].chan_metrics.byte_count += length;
  hdrbcsec[chan_idx].chan_metrics.packet_count++;

  /* ...and give the packet to the framing layer */
  hdrbcframe_put_pkt( data_pkt, length, chan_idx );

} /* hdrbcsec_put_pkt */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCSEC_CHANNEL_ENABLE

DESCRIPTION
  The function enables a new channel in the HDR BCMCS security protocol.
  A flow_id is taken as a parameter to open a new channel.  The return value
  is a handle (or index) for the assigned logical channel.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The desired flow.
  
RETURN VALUE
  logical channel index to use to refer to this channel.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcsec_channel_enable
( 
  uint32 flow_id
)
{

  int chan_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enable flow in MAC, and get the channel index that is to be 
     assigned to this flow id */
  chan_idx = hdrbcmac_channel_enable( flow_id );

  HDR_MSG_PROT_2 ( MSG_LEGACY_HIGH,
                   "HDR MCS Security enable: flow ID %d -> chan %d",
                   flow_id, chan_idx );

  /*  Make sure channel is valid */
  if (chan_idx != -1) 
  {
    /* It's not an error to be already active when we support multiple
       flows on one logical channel */
    if (hdrbcsec[chan_idx].ref_count == 0)
    {
      hdrbcsec[chan_idx].flow_id = flow_id;
      hdrbcsec_clear_channel_metrics( (uint8) chan_idx);
    }

    hdrbcsec[chan_idx].ref_count++;

  }
  
  /* Enable the content MAC logical channel */
  return chan_idx;
                               
} /* hdrbcsec_channel_enable */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCSEC_CHANNEL_DISABLE

DESCRIPTION
  The function disables a channel in the HDR BCMCS security protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow ID to disable.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcsec_channel_disable
(
  uint32 flow_id
)
{
  int chan_idx;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Disable MAC layer, and the channel number assigned to this flow id */ 
  chan_idx = hdrbcmac_channel_disable( flow_id );

  HDR_MSG_PROT_2 ( MSG_LEGACY_HIGH,
                   "HDR MCS Security disable: flow %d (chan %d)",
                   flow_id, 
                   chan_idx );

  ASSERT(chan_idx < HDRBCMAC_MAX_LOGICAL_CHANNELS);

  /* Make sure the channel index is valid */
  if (chan_idx != -1 ) 
  {    
    /* Check that it was active */
    if (hdrbcsec[chan_idx].ref_count != 0)
    {
      hdrbcsec[chan_idx].ref_count = 0;  
    }
    else
    {
      /* Error! */
      HDR_MSG_PROT_1 ( MSG_LEGACY_ERROR,
                       "Disable fail; inactive chan %d", 
                       chan_idx );
    }
  }

  return chan_idx;

} /* hdrbcsec_channel_disable */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCSEC_GET_CHANNEL_METRICS

DESCRIPTION
  This function copies security protocol metrics into a provided structure
  (for logging, etc.)
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The channel to get metrics from.
  chan_metrics - A structure to copy the data into.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcsec_get_channel_metrics(
  uint8 chan_idx,
  hdrlogbc_sec_metrics_type *chan_metrics
)
{
  memcpy( (void*)chan_metrics,
          (void*)&hdrbcsec[chan_idx].chan_metrics,  
          sizeof(hdrlogbc_sec_metrics_type));

} /* hdrbcsec_get_channel_metrics */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCSEC_CLEAR_CHANNEL_METRICS

DESCRIPTION
  This function clears the channel metrics for the specified channel. The
  channel should be active.
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The channel metrics to clear.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcsec_clear_channel_metrics( uint8 chan_idx )
{
  /* Clear metric structure */
  memset( (void*)&hdrbcsec[chan_idx].chan_metrics, 
          0, 
          sizeof(hdrlogbc_sec_metrics_type) );

} /* hdrbcsec_clear_channel_metrics */

#endif /* FEATURE_HDR_BCMCS */
