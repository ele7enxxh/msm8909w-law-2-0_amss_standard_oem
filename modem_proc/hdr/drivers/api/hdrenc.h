#ifndef HDRENC_H
#define HDRENC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    H D R    E N C O D E R    D R I V E R
                    
                           H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains definitions and declarations for using the HDR 
  encoder driver.
  
  Copyright (c) 2000 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/drivers/api/hdrenc.h#1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/27/11   ssu	   Added hdrenc_get_rri_sizes and hdrenc_get_rri_reva_sizes. 
08/12/09   wsh     Reverted CMI related change due to restructuring
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
06/16/08   etv     Added timestamp to RL phy layer log packets.
02/04/08   etv     Added support for logging RL physical layer packet dump.
02/12/08   rmg     Created hdrenc_load_frame() for RevA/B.  Removed _rev_a 
                   and _rev_b versions.
09/25/07   rmg     Added hdrenc_load_rev_b_frame().
07/25/05   kss     Moved subframe_id calc and DSP config to hdrrmac3.
05/02/05   kss     Added hdrenc_is_rev_a_ready() function.
03/29/05   kss     Use FEATURE_HDR_REVA_L1.
02/10/05   kss     Add initial Rev A support.
11/11/02   kss     Removed unused code for ROM savings.
05/05/00   kss     Created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrlogi.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRENC_INIT

DESCRIPTION
  Initializes the encoder by loading the frame type table.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrenc_init( void );


/*===========================================================================
FUNCTION HDRENC_IS_READY

DESCRIPTION
  Returns true if the encoder buffer is ready for data.
  
DEPENDENCIES
  None.
  
PARAMETERS
  None.  

RETURN VALUE
  TRUE  - encoder buffer ready for data.
  FALSE - encoder buffer not ready, still working on previous data.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrenc_is_ready( void );


/*===========================================================================
FUNCTION HDRENC_SEND_FRAME

DESCRIPTION
  Writes packet data to the encoder input buffer, and send the frame at the
  RRI indicated. Padding is added if needed. 
  
DEPENDENCIES
  The encoder must have been initialized via hdrenc_init().

PARAMETERS
  data_ptr - pointer to the data to load into the encoder
  last_byte- the last byte (containing the last 2 bits) of the frame
  length   - the byte length of the data to load from *data_ptr
  rri      - RRI at which the packet should be sent.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrenc_send_frame(
  uint8         *data_ptr,
  uint16         length,
  uint8          last_byte,
  uint8          rri
);


#ifdef FEATURE_HDR_REVA_L1
/*===========================================================================
FUNCTION HDRENC_LOAD_FRAME

DESCRIPTION
  Writes packet data to the encoder input buffer, and send the frame at the
  RRI indicated. Padding is added if needed. 
  
DEPENDENCIES
  The encoder must have been initialized via hdrenc_init().  Must call
  hdrenc_is_ready previous to this function to verify encoder is ready.

PARAMETERS
  phy_chan_id - Physical channel id
  data_ptr    - pointer to the data to load into the encoder
  last_byte   - the last byte (containing the last 2 bits) of the frame
  length      - the byte length of the data to load from *data_ptr

RETURN VALUE
  For FEATURE_HDR_REVA_L1 hw this function returns a handle for the sent packet.
  The value of the handle is 0 - 127. Any number over 127 is invalid.  This
  is so the caller can keep track of packets in case they are not accepted or
  nacked in the future.

SIDE EFFECTS
  Note that the dsm item sent is NOT freed!
  
===========================================================================*/
void hdrenc_load_frame
(
  /* Physical Channel id */
  uint8          phy_chan_id,

  /* Frame payload */
  dsm_item_type *item_ptr,

  /* Length of data in data_ptr (bytes) */
  uint16         data_length,
  
  /* Last byte of frame. Only 2 bits used. */
  uint8          last_byte,  

  /* RRI of frame. */
  uint8          rri

);

 /* EJECT */
/*===========================================================================
FUNCTION HDRENC_LOG_RTC_PHY_LAYER_PKT                          INTERNAL

DESCRIPTION
  This function ships out one log packet for every Subtype-3 RTC MAC packet 
  sent to the encoder. 

DEPENDENCIES
  None.

PARAMETERS
  pkt_ptr - chain of DSM items containing the subtype-3 RTC packet.
  mac_trailer - MAC trailer. 
  data_length - Size of the subtype-3 RTC physical layer packet. 
  pkt_size_index - index to get the physical layer frame size.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrenc_log_rtc_phy_layer_pkt
(
  /* Frame payload */  
  dsm_item_type *pkt_ptr,

  /* Last byte of frame. Only 2 bits used. */
  uint8 mac_trailer,

  /* Length of data in data_ptr (bytes) */
  uint16 data_length,

  /* Packet size index (RRI) of frame. */
  uint16 pkt_size_index,

  /* Physical layer revision (0/A/B)*/
  uint8 phy_rev,

  /* Carrier ID - Applicable only for Phy revision B */
  hdrlog_mc_carrier_id_type *carrier_id,

  uint8 subframe_id
    /* Subframe Id corresonding to this packet transmission */
);

#endif /* FEATURE_HDR_REVA_L1 */

#ifdef FEATURE_FACTORY_TESTMODE

/*===========================================================================
FUNCTION HDRENC_GET_RRI_SIZES

DESCRIPTION
  This function returns the value stored in the
  "index"th index of hdrenc_rri_sizes array. 

DEPENDENCIES
  None.

PARAMETERS
  index - the value of this index in the array is returned.

RETURN VALUE
  The value stored in the "index"th index of hdrenc_rri_sizes array.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdrenc_get_rri_sizes
(
	int index
); /* hdrenc_get_rri_sizes */


/*===========================================================================
FUNCTION HDRENC_GET_RRI_REVA_SIZES

DESCRIPTION
  This function returns the value stored in the
  "index"th index of hdrenc_rri_reva_sizes array. 

DEPENDENCIES
  None.

PARAMETERS
  index - the value of this index in the array is returned.

RETURN VALUE
  The value stored in the "index"th index of hdrenc_rri_reva_sizes array.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdrenc_get_rri_reva_sizes
(
	int index
); /* hdrenc_get_rri_sizes */

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* HDRENC_H */
