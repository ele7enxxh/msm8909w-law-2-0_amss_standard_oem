/*===========================================================================

       H D R  E N H A N C E D  T E S T   A P P L I C A T I O N

                          D E F I N I T I O N S

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

  hdris890a_activate() - One time initialization of all common test 
                         app data structures; called from PAC

  hdris890a_deactivate() - deactivate the RelA test app; called from PAC


INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdris890a_activate() has to be called before either test app can be 
  activated.

Copyright (c) 2005-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/tap/src/hdris890a.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
02/21/14    rmv    Added an API to check if TAP is activated 
06/21/10    cnx    Fixed compiler warnings.
02/09/10   rmg/spn Added extra argument in hdrpcp_register_receive_callbacks().
10/02/09    wsh    CMI API functions must always be defined and return 
                   appropriate values regardless of FEATURE flag
08/27/09    wsh    CMI:Split HDRIS890 due to external reference in OEMCM.c  
11/17/08    kss    Don't deactivate if not activated.
08/01/07    grl    Fixed RCVT 2.2 compiler warnings.
07/25/07    cc     Used new hdrpcp_register_receive_callbacks interface.
04/26/06    jyw    Replaced the ERR macro with the IS890 ERROR massage.
02/16/06    jyw    Added the dip switch cbs to disable the TC OFS in TAP call
02/09/06    jyw    Removed the function predefintions.
11/17/05    jyw    Fixed the lint errors with new config files.
11/17/05    jyw    Added some editorial changes.
10/28/05    jyw    Fixed the lint errors.
09/27/05    jyw    Added comments.
09/09/05    jyw    General interface clean up to be compatible with TAP0.
09/08/05    jyw    Replaced TAP0 with ETAP.
07/26/05    jyw    Added the revision header.
07/20/05    jyw    Cleaned up
05/19/05    jyw    Created this file

===========================================================================*/

/*==========================================================================   
                  
                  INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "target.h"

#if defined ( FEATURE_HDR_IS890 ) || defined ( FEATURE_HDR_IS890A )

#include "hdrdebug.h"
#include "err.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrstream.h"
#include "hdris890.h"
#include "hdris890_v.h"
#include "hdrftap.h"
#include "hdrrtap.h"
#include "hdrpcp.h"
#include "hdrlog.h"
/*===========================================================================

                      TYPE DEFINITION 

===========================================================================*/

/* Not sure about the use of this statistics;
   Current, the stats are not used; they are updated in the ETAP */

typedef struct 
{
  /* Transmission statistics */
  uint32   tx_total_bytes;

  /* Receiving statistics */
  uint32   rx_total_bytes;

} hdris890a_statistics_type;

/*===========================================================================

                        CONSTANTS DEFINITIONS

===========================================================================*/

/* Translation between stream ID's and protocol ID's */

const hdrhai_protocol_type_enum_type hdris890a_prot_id[4] = 
                                                          {  
                                                            HDRHAI_N_APP0_TYPE,
                                                            HDRHAI_N_APP1_TYPE,
                                                            HDRHAI_N_APP2_TYPE,
                                                            HDRHAI_N_APP3_TYPE 
                                                          };

/*===========================================================================

                        VARIABLE DEFINITIONS

===========================================================================*/

/* In traffic we still need to differentiate a test being     */
/* run or not for display purposes. This countdown is reset   */
/* to the number of frames allowed w/o traffic during a run.  */
/* This is used for the to indicate if FTAP is receiving any  */
/* FTAP packets, if no packets come in for more then 20 frames*/
/* the FTAP is not consider as activated; In the IS890, the   */
/* feature is used in FMAC to forward packet;          */
#define  HDRIS890A_INACTIVITY_FRAME_CNT   20 
uint32   hdris890a_inactivity_frame_cnt = HDRIS890A_INACTIVITY_FRAME_CNT; 

int hdris890a_num_rx_slots = 4;
  /* Global var used by hdrrx to indicate the number of rx slots */

struct 
{
  boolean is_activated;
    /* Keep track if we re-enter HDR and are still activated */

  hdris890a_statistics_type  statistics;
    /* HDRIS890A statistics */
} hdris890a = { FALSE, {0, 0}};

/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION HDRIS890_TC_OFS_DIPSW_SET

DESCRIPTION
  This function is a call back when the TAP_TC_OFS dip switch is set.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdris890a_tc_ofs_dipsw_set( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrfetap_disable_tc_ofs( FALSE );
    /* set the FETAP TC OFS flag */

  hdrretap_disable_tc_ofs( FALSE );
    /* set the RETAP TC OFS flag */

} /* static void hdris890_tc_ofs_dipsw_set( void ) */

/*===========================================================================

FUNCTION HDRIS890_TC_OFS_DIPSW_RESET

DESCRIPTION
  This function is a call back when the TAP_TC_OFS dip switch is set.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdris890a_tc_ofs_dipsw_reset( void ) 
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrfetap_disable_tc_ofs( TRUE );
    /* set the FETAP TC OFS flag */

  hdrretap_disable_tc_ofs( TRUE );
   /* set the RETAP TC OFS flag */

} /* static void hdris890_tc_ofs_dipsw_reset( void )  */

/*===========================================================================

FUNCTION HDRIS890A_ACTIVATE

DESCRIPTION
  This function is called by PAC when the enhanced test application is
  negotiated on any stream.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id  - Stream ID on which IS890A is negotiated 

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdris890a_activate
( 
  uint8 stream_id 
    /* the stream id at which the ETAP is negotiated */
)
{

  static uint32 dummy;
  uint16 app_subtype;
  boolean reg_result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if not activated */

  if ( ! hdris890a.is_activated )
  {

    /* Initialize the IS890A statistics the stats update is in the 
     * put_data/get_data back */

    memset( &( hdris890a.statistics ), 0, sizeof( hdris890a_statistics_type ) );

    /* Retrieve the subtype from the stream layer; try to maitain same PAC
     * interface here to avoid compiling issue b/w different targets */

    if ( hdrstream_stream_is_assigned ( HDRHAI_ENH_TEST_APP_IS890A ) )
    {
      app_subtype = HDRHAI_ENH_TEST_APP_IS890A ; 
    }
    else if ( hdrstream_stream_is_assigned ( HDRHAI_DEF_PACKET_APP_IS890 ) )
    {
      app_subtype = HDRHAI_DEF_PACKET_APP_IS890 ; 
    }
#ifdef FEATURE_HDR_REVB
    else if ( hdrstream_stream_is_assigned ( HDRHAI_MC_TEST_APP_IS890B ) )
    {
      app_subtype = HDRHAI_MC_TEST_APP_IS890B ; 
    }
#endif /* FEATURE_HDR_REVB */
    else
    {
      HDR_MSG_IS890( MSG_LEGACY_ERROR, "IS890A: incorrect subtype ! return " );
      return;
    }
    
    /* register the transmission cb functions with PCP */

    reg_result = hdrpcp_register_receive_callbacks ( 
                                        (hdrhai_stream_enum_type) stream_id,
                                        hdris890a_put_pkt,
                                        (hdrpcp_eos_notification_func_type) NULL,
                                        (hdrpcp_rx_buf_empty_notification_func_type) NULL,
                                        &dummy );
    if ( reg_result ) 
    {
      HDR_MSG_IS890_1 ( MSG_LEGACY_HIGH,  
                        "IS890A: Registerred rx pcp clbks for Stream %d",
                        (hdrhai_stream_enum_type) stream_id);

      /* Activate the FETAP and RETAP; this is once per session config */

      hdrfetap_activate( stream_id, app_subtype );
      hdrretap_activate( stream_id, app_subtype );
    
      hdris890a.is_activated = TRUE;
    
      HDR_MSG_IS890_2 ( MSG_LEGACY_HIGH, 
                        "IS890A: Subtype%02x is activated on stream %d", 
                        app_subtype, stream_id );
    }
    else  /* !reg_result */
    {
      HDR_MSG_IS890_2 ( MSG_LEGACY_ERROR,  
                        "IS890A: Subtype%02x PCP rx reg failed on stream %d !", 
                        app_subtype, stream_id );
    }

    /* Set the dip switch callbacks for the TC OFS control */

    hdrlog_set_dipsw_cb( HDRLOG_DIPSW_TAP_TCOFS,
                         hdris890a_tc_ofs_dipsw_set,
                         hdris890a_tc_ofs_dipsw_reset );

  }  /* hdris890a.is_activate */

} /* hdris890a_activate */

/*===========================================================================

FUNCTION HDRIS890A_DEACTIVATE

DESCRIPTION
  This function is called when the session is closed.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id  - Stream ID on which is890a is assigned

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdris890a_deactivate
( 
  uint8 stream_id 
    /* The stream ID at which the is890a was negotiated */
)
{

  boolean unreg_result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Don't deactivate if not active. */
  if (hdris890a.is_activated == TRUE)
  {
    /* De-register is890a receive call back functions to the PCP */

    unreg_result = hdrpcp_unregister_receive_callbacks(
                     ( hdrhai_stream_enum_type ) stream_id );

    if ( ! unreg_result ) 
    {
      /* this should not happen here */

      HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR,  
                        "IS890A: failed to Un-Reg rx clbks for Stream %d",
                        ( hdrhai_stream_enum_type ) stream_id );
    }

    /* De-register is890a transmit call back functions to the PCP use the 
     * substream id 0xff to indicate all substreams   */

    unreg_result = hdrpcp_unregister_transmit_callbacks(
                     ( hdrhai_stream_enum_type ) stream_id, 
                     HDRPCP_UNREGISTER_ALL_SUBSTREAMS  );

    if ( ! unreg_result ) 
    {

      /* this should not happen if using the right stream id */
    
      HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR,  
                        " IS890A: failed to Un-Reg tx clbks for Stream %d",
                       ( hdrhai_stream_enum_type ) stream_id );
    }

    HDR_MSG_IS890_1 ( MSG_LEGACY_HIGH,  
                      " IS890A: deactivated from stream %d",
                     ( hdrhai_stream_enum_type ) stream_id );

    hdrfetap_deactivate();
    hdrretap_deactivate();

    hdris890a.is_activated = FALSE;
  }
  else
  {
    HDR_MSG_IS890 ( MSG_LEGACY_ERROR,  
                    " IS890A: Deactivate called when not activated. Skipping.");
  }

} /* hdris890a_deactivate */

/*===========================================================================

FUNCTION HDRIS890A_IS_IN_TRAFFIC

DESCRIPTION
  This function checks wether is890a is currently negotiated on any stream 
  and in the traffic state.
  hdris890_inactivity is restored every time when a packet is RXed or TXed
  to or from the is890a; and it is decrmented by 1 every PN roll interrupt; 

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdris890a_is_in_traffic( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( (hdrfetap_is_in_traffic() || hdrretap_is_in_traffic()) &&
           hdris890a_inactivity_frame_cnt );
    /* hdris890a_inactivity_frame_cnt needs above zero */

} /* hdris890a_is_in_traffic */

/*===========================================================================

FUNCTION HDRIS890A_MSG_CB

DESCRIPTION
  This function is staticly registered to be called whenever HMP receives a
  message for is890a .

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdris890a_msg_cb
(
  dsm_item_type* msg_ptr
    /* the ptr to the DSM item containing the incoming msg */
)
{

  uint8 msg_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( msg_ptr == NULL ) 
  {
    /* sanity check the message pointor */

    HDR_MSG_IS890( MSG_LEGACY_ERROR,  
                   "IS890A: unexpected NULL msg ptr!" );
    return;
  }
  else
  {
    /* Get the message ID */

    msg_id = *(msg_ptr->data_ptr);
  }

  /* Check wether it belongs to F- or RTAP */

  if ( msg_id < HDRFETAP_MSG_MAX_ID )
  {
    hdrfetap_msg_cb( msg_ptr );
  }
  else
  {
    hdrretap_msg_cb( msg_ptr );
  }

} /* void hdris890a_msg_cb */

/*===========================================================================

FUNCTION HDRIS890A_PUT_PKT

DESCRIPTION
  This function is registered with PCP to handle received packets
  for is890a. Currently IS-890A does not define any forward link
  packets for RETAP.

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
    /* The ptr to the DSM item containing the incoming pkt */

  void*            user_data_ptr
    /* The generic PCP interface, not used */
)
{

  uint8   protocol_id;
  uint32  pkt_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED( user_data_ptr );
    /* mark the unused symbol to avoid lint error */

  if ( rx_pkt_ptr == NULL )
  {

    /* This is a valid case - RLP uses NULL pkt pointers 
     * to indicate the end of frame.       */

    return;
  }

  /* Get the protocol ID (bits ..||.... of first byte) */

  protocol_id = (*(rx_pkt_ptr->data_ptr) & 0x30) >> 4;
  pkt_len = dsm_length_packet( rx_pkt_ptr );
  hdris890a.statistics.rx_total_bytes += pkt_len;

  switch( protocol_id )
  {
    case HDRFETAP_PROTOCOL_ID:

      hdrfetap_put_pkt( rx_pkt_ptr );
      hdris890a_inactivity_frame_cnt = HDRIS890A_INACTIVITY_FRAME_CNT;

      break;

    case HDRRETAP_PROTOCOL_ID:

      hdrretap_put_pkt( rx_pkt_ptr );
      hdris890a_inactivity_frame_cnt = HDRIS890A_INACTIVITY_FRAME_CNT;

      break;

    default:

      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                       "IS890A: Rcvd invalid prot. ID %d", protocol_id );
      dsm_free_packet( &rx_pkt_ptr );

      break;
  }

} /* hdris890a_put_pkt */

/*===========================================================================

FUNCTION HDRIS890_GET_NUM_TX_BYTES

DESCRIPTION
  This function returns the total number of bytes transmitted
  by RTAP and FTAP since the last activation.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Number of TX bytes

SIDE EFFECTS
  None
===========================================================================*/
uint32 hdris890a_get_num_tx_bytes( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdris890a.statistics.tx_total_bytes;

} /* hdris890a_get_num_tx_bytes */

/*===========================================================================

FUNCTION HDRIS890_GET_NUM_RX_BYTES

DESCRIPTION
  This function returns the total number of bytes received
  by RTAP and FTAP since the last activation.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Number of RX bytes

SIDE EFFECTS
  None
===========================================================================*/
uint32 hdris890a_get_num_rx_bytes( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdris890a.statistics.rx_total_bytes;

} /* hdris890a_get_num_rx_bytes */

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
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdris890a.statistics.tx_total_bytes += num_bytes;

} /* hdris890a_get_num_rx_bytes */

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
boolean hdris890a_is_activated( void )
{
   return hdris890a.is_activated;
} /* hdris890a_is_activated */
#else     /* FEATURE_HDR_IS890 || FEATURE_HDR_IS890A */

/*===========================================================================

FUNCTION HDRIS890A_IS_IN_TRAFFIC

DESCRIPTION
  This function checks wether is890a is currently negotiated on any stream 
  and in the traffic state.
  hdris890_inactivity is restored every time when a packet is RXed or TXed
  to or from the is890a; and it is decrmented by 1 every PN roll interrupt; 

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdris890a_is_in_traffic( void )
{
  return FALSE;
} /* hdris890a_is_in_traffic */

/*===========================================================================

FUNCTION HDRIS890_GET_NUM_TX_BYTES

DESCRIPTION
  This function returns the total number of bytes transmitted
  by RTAP and FTAP since the last activation.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Number of TX bytes

SIDE EFFECTS
  None
===========================================================================*/
uint32 hdris890a_get_num_tx_bytes( void )
{
  return 0;
} /* hdris890a_get_num_tx_bytes */

/*===========================================================================

FUNCTION HDRIS890_GET_NUM_RX_BYTES

DESCRIPTION
  This function returns the total number of bytes received
  by RTAP and FTAP since the last activation.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Number of RX bytes

SIDE EFFECTS
  None
===========================================================================*/
uint32 hdris890a_get_num_rx_bytes( void )
{
  return 0;
} /* hdris890a_get_num_rx_bytes */
#endif    /* FEATURE_HDR_IS890 || FEATURE_HDR_IS890A */
