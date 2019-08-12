#ifndef HDRIS890_V_H
#define HDRIS890_V_H

/*===========================================================================

       H D R  E N H A N C E D  T E S T   A P P L I C A T I O N

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  Top level module of the IS890A.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdris890a_init() has to be called before 

Copyright (c) 2005, 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/tap/api/hdris890_v.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/21/14   rmv     Added an API to check if TAP is activated
09/23/11   smd     SU API cleanup.
10/01/09   wsh     Moved redirection defines to API header 
08/27/09   wsh     CMI:Split HDRIS890 due to external reference in OEMCM.c 
06/12/08   etv     Added constant for invalid channel frequency.
08/07/07   cws     Added MC connected stats.
08/01/07   grl     Fixed RCVT 2.2 compiler warnings.
10/28/05   jyw     Added function hdris890a_update_num_tx_bytes.
09/27/05   jyw     Fixed bug in the macro definitions.
09/09/05   jyw     General clean up/more comments.
09/08/05   jyw     Replaced TAP0 with ETAP.
08/02/05   jyw     General Clean up.
07/26/05   jyw     Added the revision header.
05/19/05   jyw     Created this file

===========================================================================*/

/*========================================================================== 
                    INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "target.h"

#if defined ( FEATURE_HDR_IS890 ) || defined ( FEATURE_HDR_IS890A )

#include "dsm.h"
#include "hdrhai.h" 
#include "hdrcp_api.h"

/*===========================================================================

                 DEFINITIONS AND CONSTANTS for is890A 
                 maybe they sould be sperated into hdrretap.h and hdrfetap.h

===========================================================================*/

/* === forward link rate and payload size definition === */
#define HDR_FWD_RATE_NULL        0x00
#define HDR_FWD_RATE_38_4        0x01
#define HDR_FWD_RATE_76_8        0x02
#define HDR_FWD_RATE_153_6       0x03
#define HDR_FWD_RATE_307_2_S     0x04
#define HDR_FWD_RATE_307_2_L     0x05
#define HDR_FWD_RATE_614_4_S     0x06
#define HDR_FWD_RATE_614_4_L     0x07
#define HDR_FWD_RATE_921_6       0x08
#define HDR_FWD_RATE_1228_8_S    0x09
#define HDR_FWD_RATE_1228_8_L    0x0a
#define HDR_FWD_RATE_1843_2      0x0b
#define HDR_FWD_RATE_2457_6      0x0c 
/* ---- REL A fwd like rate ---- */
#define HDR_FWD_RATE_1536        0x0d 
#define HDR_FWD_RATE_3072        0x0e 


/* -REL A PHY payload size index- */
#define HDR_FWD_PL2_PLSIZE_128   0x0
#define HDR_FWD_PL2_PLSIZE_256   0x1
#define HDR_FWD_PL2_PLSIZE_512   0x2
#define HDR_FWD_PL2_PLSIZE_1024  0x3
#define HDR_FWD_PL2_PLSIZE_2048  0x4
#define HDR_FWD_PL2_PLSIZE_3072  0x5
#define HDR_FWD_PL2_PLSIZE_4096  0x6
#define HDR_FWD_PL2_PLSIZE_5120  0x7

/* === reverse link rate and payload size definition === */
#define HDR_REV_PL01_RATE_NULL   0x00
#define HDR_REV_PL01_RATE_9_6    0x01
#define HDR_REV_PL01_RATE_19_2   0x02
#define HDR_REV_PL01_RATE_38_4   0x03
#define HDR_REV_PL01_RATE_76_8   0x04
#define HDR_REV_PL01_RATE_153_6  0x05

/* -REL A PHY payload size index- */
#define HDR_REV_PL2_PLSIZE_0     0x00
#define HDR_REV_PL2_PLSIZE_96    0x01
#define HDR_REV_PL2_PLSIZE_224   0x02
#define HDR_REV_PL2_PLSIZE_480   0x03
#define HDR_REV_PL2_PLSIZE_736   0x04
#define HDR_REV_PL2_PLSIZE_992   0x05
#define HDR_REV_PL2_PLSIZE_1504  0x06
#define HDR_REV_PL2_PLSIZE_2016  0x07
#define HDR_REV_PL2_PLSIZE_3040  0x08
#define HDR_REV_PL2_PLSIZE_4064  0x09
#define HDR_REV_PL2_PLSIZE_6112  0x0a
#define HDR_REV_PL2_PLSIZE_8160  0x0b
#define HDR_REV_PL2_PLSIZE_12256 0x0c

/* -REL A EACH rate index- */
#define HDR_REACH_RATE_96        0x0
#define HDR_REACH_RATE_192       0x1
#define HDR_REACH_RATE_384       0x2

/*===========================================================================

                  REDIRECTION FOR THE IS890 

===========================================================================*/

/*===========================================================================

                   COMMON DEFINITIONS for is890a

===========================================================================*/
/* Maximum packet buffer size (IS-890, section 2.9.2 gives   */
/* a minimum of 8, typically we need up to 20 to fill holes  */
/* on the forward link.)                                     */
#define HDRIS890A_PKT_BUFFER_SIZE              16 

/* ((27 + (16*33)) / 8) + 1 for FETAP lo pkts and 
   (94 / 8) + 1             for RETAP pkts */
#define HDRIS890A_PKT_MAX_SIZE                 70

#ifdef FEATURE_HDR_REVB_7500
#error code not present
#else 
#define HDRIS890B_NUM_CARRIERS                  3
#endif /* FEATURE_HDR_REVB_7500 */

/* Invalid frequency for channel ranges of all band classes */
#define HDRIS890_INVALID_CHAN_FREQUENCY         0xFFFFFFFF
/*===========================================================================

                FETAP MSG/ATTRIBUTE DEFINITIONS

===========================================================================*/
#define HDRFETAP_PROTOCOL_ID                        0x00

#define HDRFETAP_MSG_PARAMETER_ASSIGNMENT           0x00
#define HDRFETAP_MSG_PARAMETER_COMPLETE             0x01
#define HDRFETAP_MSG_STATS_CLEAR_REQUEST            0x02
#define HDRFETAP_MSG_STATS_CLEAR_RESPONSE           0x03
#define HDRFETAP_MSG_STATS_GET_REQUEST              0x04
#define HDRFETAP_MSG_STATS_GET_RESPONSE             0x05
#define HDRFETAP_MSG_MAX_ID                         0x7f

/* Attributes associated with F/FE/FMCTAPParameterAssignment */
#define HDRFETAP_ATTR_DRC_VALUE_FIXED_MODE          0x00
#define HDRFETAP_ATTR_DRC_COVER_FIXED_MODE          0x01
#define HDRFETAP_ATTR_ACK_CHN_BIT_FIXED_MODE        0x02
#define HDRFETAP_ATTR_LOOPBACK_MODE                 0x03
#define HDRFETAP_ATTR_ACK_CHN_MODTYPE_FIXED_MODE    0x04
#ifdef FEATURE_HDR_REVB
#define HDRFETAP_ATTR_DRC_VALUE_FIXED_MODE_MC       0x05
#define HDRFETAP_ATTR_DRC_COVER_FIXED_MODE_MC       0x06
#define HDRFETAP_ATTR_ACK_CHN_BIT_FIXED_MODE_MC     0x07
#define HDRFETAP_ATTR_LOOPBACK_MODE_MC              0x08
#define HDRFETAP_ATTR_ACK_CHN_MODTYPE_FIXED_MODE_MC 0x09
#endif /* FEATURE_HDR_REVB */

/* Attributes associated with StatsClear and StatsGet messages */
#define HDRFETAP_ATTR_IDLE_ASP_STATS                0x04
#define HDRFETAP_ATTR_CONNECTED_SS_STATS            0x05
#define HDRFETAP_ATTR_FIRST_SYNC_CC_PKT_STATS       0x06
#ifdef FEATURE_HDR_REVB
#define HDRFETAP_ATTR_CONNECTED_SS_STATS_MC         0x07
#endif /* FEATURE_HDR_REVB */

/*===========================================================================

                RETAP MSG DEFINITIONS

===========================================================================*/
#define HDRRETAP_PROTOCOL_ID                   0x01

#define HDRRETAP_MSG_PARAMETER_ASSIGNMENT      0x80
#define HDRRETAP_MSG_PARAMETER_COMPLETE        0x81

/* -- new in is890a  -- */
#define HDRRETAP_MSG_STATS_CLEAR_REQUEST       0x82
#define HDRRETAP_MSG_STATS_CLEAR_RESPONSE      0x83
#define HDRRETAP_MSG_STATS_GET_REQUEST         0x84
#define HDRRETAP_MSG_STATS_GET_RESPONSE        0x85

/* -- ATTRIBUTES -- */

#define HDRRETAP_ATTR_TEST_PKT_MODE            0x00
#define HDRRETAP_ATTR_PKT_RATE_MODE            0x01
#define HDRRETAP_ATTR_PKT_PAYLOAD_SIZE_MODE    0x02
#define HDRRETAP_ATTR_EACH_RATE_MODE           0x03
#define HDRRETAP_ATTR_BURST_PERIOD_MODE        0x04
#define HDRRETAP_ATTR_BURST_SIZE_MODE          0x05
#define HDRRETAP_ATTR_PKT_PAYLOAD_SIZE_MODE_MC 0x06
  /* assume 6 here though spec says 7 */

#define HDRRETAP_TEST_PKT_ID                   0x00
#define HDRRETAP_PL_01_TEST_PKT_SIZE           94

#define HDRRETAP_FILL_PKT_ID                   0x01
#define HDRRETAP_FILL_PKT_SIZE                 4096

/* Attributes associated with StatsClear and StatsGet messages */
#define HDRRETAP_ATTR_PL_2_STATS_RECORD        0x00
#define HDRRETAP_ATTR_PL_3_STATS_RECORD        0x01


/*===========================================================================

                    DATA DECLARATIONS FROM IS-890A

===========================================================================*/
/* the following is defined in the hdris890.h as well
   maybe in the later clean up, this part can be merged 
   with hdris890.h */

/* Validity check for 8-bit sequence numbers */
#define HDRIS890A_SEQNO_IS_VALID( rx_seqno, ref_seqno )      \
  ((rx_seqno + 384 - ref_seqno) % 256 > 128)

/* Test app packet buffer */
#define HDRIS890A_PKT_BUFFER_SET_IN_PROGRESS( di_ptr )       \
  (di_ptr)->app_field &= 0xfffeffff;

#define HDRIS890A_PKT_BUFFER_SET_READY( di_ptr )             \
  (di_ptr)->app_field |= 0x00010000;

#define HDRIS890A_PKT_BUFFER_IS_READY( di_ptr )              \
  (((di_ptr)->app_field & 0x00010000) == 0x00010000)
#define HDRIS890A_PKT_BUFFER_SET_FRAMETIME( di_ptr, ftime )  \
  (di_ptr)->app_field &= 0xffff0000;                        \
  (di_ptr)->app_field |= (ftime & 0x0000ffff);

#define HDRIS890A_PKT_BUFFER_GET_FRAMETIME( di_ptr )         \
  ((di_ptr)->app_field &= 0x0000ffff)


/* External variables */
extern uint32                               hdris890a_inactivity_frame_cnt;
extern int                                  hdris890a_num_rx_slots ;
extern const hdrhai_protocol_type_enum_type hdris890a_prot_id[4];

/*===========================================================================

               HDRIS890A EXTERNAL FUNCTION DECLARATIONS 

===========================================================================*/

/*===========================================================================

FUNCTION HDRIS890A_ACTIVATE

DESCRIPTION
  This function does all one-time initialization.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdris890a_activate( uint8 is890_stream_id );

/*===========================================================================

FUNCTION HDRIS890A_DEACTIVATE

DESCRIPTION
  This function is called when the session is closed.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdris890a_deactivate( uint8 stream_id );

/*===========================================================================

FUNCTION HDRIS890A_MSG_CB

DESCRIPTION
  This function is registered to be called whenever HMP receives a
  message for is890a.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdris890a_msg_cb ( dsm_item_type* msg_ptr );


/*===========================================================================
FUNCTION HDRIS890A_PUT_PKT

DESCRIPTION
  This function is registered with PCP to handle received packets
  for is890a. 

DEPENDENCIES
  None
 
PARAMETERS
  rx_pkt_ptr     - Pointer to DSM chain containing received packet
  user_data_ptr  - User data pointer (not used for IS-890)
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdris890a_put_pkt
(
  dsm_item_type*   rx_pkt_ptr,
  void*            user_data_ptr
);

/*===========================================================================

FUNCTION HDRIS890_UPDATE_NUM_TX_BYTES

DESCRIPTION
  This function update the total number of bytes in the hdris890a stats. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Number of RX bytes

SIDE EFFECTS
  None
===========================================================================*/
void hdris890a_update_num_tx_bytes
( 
  uint32 num_bytes 
    /* the number of bytes transmitted including 
     * FETAP loop back packet and RETAP test packet */
);

/*===========================================================================

FUNCTION HDRIS890_IS_ACTIVATED

DESCRIPTION
  This function returns if IS890A is activated or not. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  true - IS890A activated
  false - IS890A not activated

SIDE EFFECTS
  None
===========================================================================*/
boolean hdris890a_is_activated( void );

#endif   /* FEATURE_HDR_IS890 || FEATURE_HDR_IS890A */
#endif   /* HDRIS890_V_H */

