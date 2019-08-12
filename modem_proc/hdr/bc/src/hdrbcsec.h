#ifndef HDRBCSEC_H
#define HDRBCSEC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 H D R   B R O A D C A S T   S E C U R I T Y 
       
                             P R O T O C O L
       
                  E X T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the external interface of the HDR Broadcast Security
  Protocol.
  
EXTERNALIZED FUNCTIONS
  hdrbcsec_put_pkt()
    Process a broadcast security packet.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  

  Copyright (c) 2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2003 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcsec.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/16/04   kss     Minor change to API (always use flow_id)
07/14/04   kss     Added support for using dsm items.
04/26/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdrlogi.h"
#include "dsm.h"



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
void hdrbcsec_init( void );

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
  dsm_item_type* data, 
  uint16 length,
  int chan_idx 
);

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
  uint32 flowID 
);


/*===========================================================================

FUNCTION HDRBCSEC_CHANNEL_DISABLE

DESCRIPTION
  The function disables a channel in the HDR BCMCS security protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow to disable.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcsec_channel_disable
( 
  uint32 flow_id 
);


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
void hdrbcsec_clear_channel_metrics
(
  uint8 chan_idx 
);


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
void hdrbcsec_get_channel_metrics
(
  uint8 chan_idx,
  hdrlogbc_sec_metrics_type *chan_metrics
);

#endif /* HDRBCSEC_H */


