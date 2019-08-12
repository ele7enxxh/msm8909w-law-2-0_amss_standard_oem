/*===========================================================================

       H D R SIGNALING ADAPTATION P R O T O C O L RECEIVE MODULE


DESCRIPTION
  This file implements the Signaling Adaptation Protocol Receive module

EXTERNALIZED FUNCTIONS (Global)

EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2011 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsaprx.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/20/12   arm     Resolve TCB errors for Dime. 
03/19/12   cnx     Fixed compiler errors.
02/02/12   cnx     Don't drop DL msg if its SAPState does't match current state.
01/05/12   sju     Added support to enable verbose debug f3 at runtime. 
11/08/11   cnx     Support Optimized Handoff phase 3.
10/28/11   pba     Optimized handoff phase 2 support
04/11/11   cnx     Fixed compiler errors.
03/14/11   sju     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"

#include "hdrhai.h"
#include "task.h"
#include "err.h"
#include "hdrbit.h"
#include "amssassert.h"
#include "hdrtrace.h"
#include "IxErrno.h"
#include "hdrdebug.h"
#include "hdrutil.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsaprx.h"
#include "hdrsap.h"
#include "lte_irat_types.h"
#include "hdrsaps.h"
#include "hdrrup.h"
#include "hdrampati.h"
#include "hdrpcp.h"
#include "hdrrx.h"
#include "hdrhai.h"
#include "hdrcp_msg.h"
#include "hdrmc_v.h"
#include "hdridle.h"
#include "hdralmp.h"
#include "hdrstream.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* ----------------- SAP Layer packet header macros ----------------- */

#define HDRSAPRX_SAP_HEADER_SIZE  5

/* A non-zero value indicate that the SAP is in OPEN state */
#define HDRSAPRX_PKT_SAP_IS_OPEN( ptr )\
         ( ( ( *(byte*)(ptr) & 0x80  ) != 0 ) ? TRUE : FALSE )

/* A non-zero value indicates that the Connection Layer packet is format B.*/ 
#define HDRSAPRX_PKT_IS_FMT_B( ptr )\
          ( ( ( *(byte*)(ptr) & 0x40 ) != 0 ) ? TRUE : FALSE )

/* ATI type. */
#define HDRSAPRX_GET_PKT_ATI_TYPE( ptr )\
          ( ( *(byte*)(ptr) & 0x30 ) >> 4 )

/* ATI Address */
#define HDRSAPRX_GET_PKT_ATI( ptr )\
          ( ((*((byte*)(ptr) + 0)& 0x0F ) << 28) |\
            (*((byte*)(ptr) + 1) << 20)          |\
            (*((byte*)(ptr) + 2) << 12)          |\
            (*((byte*)(ptr) + 3) << 4)           |\
            ((*((byte*)(ptr) + 4) & 0xF0) >> 4) ) 

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/



/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
static struct
{
  uint8 verbose_debug;
} hdrsaprx;

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRSAPRX_TIMER_CB                                 

DESCRIPTION
  This function is the timer callback function. 

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsaprx_timer_cb
(
  uint32 timer_id
)
{
  /* queue the timer to RMAC protocol */
  hdrrx_queue_timer(HDRHAI_SAP_RX_MODULE, timer_id);

  return;

} /* hdrsaprx_timer_cb */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPRX_PROCESS_TIMER                                       EXTERNAL

DESCRIPTION
  This function processes timer expiry event for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsaprx_process_timer(uint32 timer_id)
{

  switch ( timer_id )
  {

    default :
      ERR("Invalid SAPRX timer id !",0,0,0);
      break;
  }

} /* hdrsaprx_process_timer */

/* EJECT */
/*===========================================================================
FUNCTION HDRSAPRX_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsaprx_set_verbose_debug
(
  uint8 verbose_debug_setting
)
{
  hdrsaprx.verbose_debug = verbose_debug_setting; 
} /* hdrsaprx_set_verbose_debug() */

#ifdef FEATURE_IRAT_DSM_SUPPORT
/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_DL_info_transfer

DESCRIPTION
  This function processes LTE DownLink Info Transfer indication 

DEPENDENCIES
  None.

PARAMETERS
  dl_msg_ind - pointer to DownLink Info Transfer indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrsaprx_process_dl_info_transfer
(
  emm_irat_3gpp2_dl_msg_ind_type *dl_msg_ind
)
{
  /* Connection Layer packet pointer. */
  dsm_item_type      *pkt_ptr = NULL;

  msgr_attach_struct_type   *att_ptr;

  /* Access Terminal Identifier (ATI) record. */
  hdrhai_ati_record_type  received_ati;

  /* SAP is in OPEN state */
  boolean sap_is_open = FALSE;

  /* Connection Layer Packet is Format B*/
  boolean is_fmt_b = TRUE;

  /* CUrrent SAP state */
  hdrsaps_state_enum_type current_sap_state;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  att_ptr = msgr_get_attach(&(dl_msg_ind->msg_hdr), 0);
  ASSERT(att_ptr != NULL);

  msgr_get_dsm_attach(att_ptr, &pkt_ptr);
  ASSERT(pkt_ptr != NULL);

  /* Parsing SAP Header */
  sap_is_open = HDRSAPRX_PKT_SAP_IS_OPEN(pkt_ptr->data_ptr);

  current_sap_state = hdrsaps_get_state();

  /* Check sap_is_open against actual SAP Sate */
  if ( (sap_is_open && (current_sap_state != HDRSAPS_OPEN) ) ||
       (!sap_is_open && (current_sap_state == HDRSAPS_OPEN) ) )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                    "SAP state does not match. curr: %d, rcvd pkt: %d",
                    current_sap_state, sap_is_open );
  }

  is_fmt_b = HDRSAPRX_PKT_IS_FMT_B(pkt_ptr->data_ptr);
  received_ati.ati_type = HDRSAPRX_GET_PKT_ATI_TYPE(pkt_ptr->data_ptr);
  received_ati.ati_value = HDRSAPRX_GET_PKT_ATI(pkt_ptr->data_ptr);

  HDR_MSG_PROT_4(MSG_LEGACY_HIGH, 
                 "=RX= Received packet: state %d, fmt %d, ati_type %d, ati 0x%x",
                 sap_is_open, 
                 is_fmt_b, 
                 received_ati.ati_type, 
                 received_ati.ati_value);

  if (hdrampati_match_receive_ati_address (&received_ati,
                                            HDRHAI_SAP_RX_MODULE))
  {
    /* Remove the SAP header */
    if ( dsm_pullup(&pkt_ptr, NULL, HDRSAPRX_SAP_HEADER_SIZE) 
		 != HDRSAPRX_SAP_HEADER_SIZE )
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Cannot remove SAP header");
      dsm_free_packet( &pkt_ptr );
      return;
    }

    /* Put packet to PCP */
    hdrpcp_put_pkt( pkt_ptr, is_fmt_b );  
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "Received ATI does not match");

    dsm_free_packet( &pkt_ptr );
    return;
  }

} /* hdrsaprx_process_dl_info_transfer */

/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_DL_HO_FROM_EUTRA_PREP_REQ

DESCRIPTION
  This function processes LTE downLink handoff from EUTRA prep request.

DEPENDENCIES
  None.

PARAMETERS
  dl_msg_ind - pointer to DownLink Info Transfer indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrsaprx_process_dl_ho_from_eutra_prep_req
(
  emm_irat_3gpp2_dl_msg_ind_type *dl_msg_ind
)
{
  if ( !hdrsap_tunnel_mode_is_enabled( HDRHAI_SAP_RX_MODULE ) )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
        "Active handoff request ignored tunnel mode not enabled");
    return;
  }

  if (!hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ))
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
        "Active handoff request ignored session is not open");
    return;
  }

  if (hdrsaprx.verbose_debug)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "=RX= EUTRA HO Prep Request");
  }
  hdrsap_active_handoff( HDRHAI_SAP_RX_MODULE );

} /* hdrsaprx_process_dl_ho_from_eutra_prep_req */

#else

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_DL_info_transfer

DESCRIPTION
  This function processes LTE DownLink Info Transfer indication 

DEPENDENCIES
  None.

PARAMETERS
  container_ptr - pointer to IRAT message container

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrsaprx_process_dl_info_transfer
(
  lte_irat_tunnel_msg_container_s *container_ptr
)
{
  /* Connection Layer packet pointer. */
  dsm_item_type      *pkt_ptr;

  /* Access Terminal Identifier (ATI) record. */
  hdrhai_ati_record_type  received_ati;

  /* SAP is in OPEN state */
  boolean sap_is_open = FALSE;

  /* Connection Layer Packet is Format B*/
  boolean is_fmt_b = TRUE;

  /* CUrrent SAP state */
  hdrsaps_state_enum_type current_sap_state;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert byte stream received from LTE to DSM item */
  pkt_ptr = hdrutil_byte_to_dsm(container_ptr->msg_payload,
                                container_ptr->msg_size);

  /* Parsing SAP Header */
  sap_is_open = HDRSAPRX_PKT_SAP_IS_OPEN(pkt_ptr->data_ptr);

  current_sap_state = hdrsaps_get_state();

  /* Check sap_is_open against actual SAP Sate */
  if ( (sap_is_open && (current_sap_state != HDRSAPS_OPEN) ) ||
       (!sap_is_open && (current_sap_state == HDRSAPS_OPEN) ) )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                    "SAP state does not match. curr: %d, rcvd pkt: %d",
                    current_sap_state, sap_is_open );
  }

  is_fmt_b = HDRSAPRX_PKT_IS_FMT_B(pkt_ptr->data_ptr);
  received_ati.ati_type = HDRSAPRX_GET_PKT_ATI_TYPE(pkt_ptr->data_ptr);
  received_ati.ati_value = HDRSAPRX_GET_PKT_ATI(pkt_ptr->data_ptr);

  HDR_MSG_PROT_4(MSG_LEGACY_HIGH, 
                 "=RX= Received packet: state %d, fmt %d, ati_type %d, ati 0x%x",
                 sap_is_open, 
                 is_fmt_b, 
                 received_ati.ati_type, 
                 received_ati.ati_value);

  if (hdrampati_match_receive_ati_address (&received_ati,
                                            HDRHAI_SAP_RX_MODULE))
  {
    /* Remove the SAP header */
    if ( dsm_pullup(&pkt_ptr, NULL, HDRSAPRX_SAP_HEADER_SIZE) 
		 != HDRSAPRX_SAP_HEADER_SIZE )
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Cannot remove SAP header");
      dsm_free_packet( &pkt_ptr );
      return;
    }

    /* Put packet to PCP */
    hdrpcp_put_pkt( pkt_ptr, is_fmt_b );  
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "Received ATI does not match");

    dsm_free_packet( &pkt_ptr );
    return;
  }

} /* hdrsaprx_process_dl_info_transfer */
#endif /* FEATURE_IRAT_DSM_SUPPORT */

#ifdef FEATURE_IRAT_DSM_SUPPORT
/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_LTE_DL_DSM_MSG

DESCRIPTION
  This function processes LTE DownLink message received event.

DEPENDENCIES
  None.

PARAMETERS
  dl_msg_ind - pointer to the DownLink Info Transfer indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaprx_process_lte_dl_msg
(
  emm_irat_3gpp2_dl_msg_ind_type *dl_msg_ind
)
{
  switch (dl_msg_ind->dl_msg_info.dl_msg_type)
  {
    case LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER:
      HDR_MSG_PROT(MSG_LEGACY_MED,"=RX= Process dl info transfer");
      hdrsaprx_process_dl_info_transfer(dl_msg_ind);
      hdrpcp_rx_buf_empty();
      break;

    case LTE_IRAT_TUNNEL_DL_MSG_HO_FROM_EUTRA_PREP_REQ:
      HDR_MSG_PROT(MSG_LEGACY_MED,"=RX= Process HO prep request");
      hdrsaprx_process_dl_ho_from_eutra_prep_req(dl_msg_ind);
      break;

    default: 
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Received unsupported msg %d",
                      dl_msg_ind->dl_msg_info.dl_msg_type );
      break;
  }

} /* hdrsaprx_process_lte_dl_msg */

#else
/* EJECT */
/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_LTE_DL_MSG

DESCRIPTION
  This function processes LTE DownLink message received event.

DEPENDENCIES
  None.

PARAMETERS
  rx_msg_ptr - pointer to dl msg indication 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaprx_process_lte_dl_msg
(
  lte_irat_tunnel_dl_msg_ind_s *rx_msg_ptr
)
{
  if (rx_msg_ptr->dl_msg_u.dl_info_tx.type != LTE_IRAT_RAT_TYPE_HRPD)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED,"RAT type doesn't match: rcv %d",
                   rx_msg_ptr->dl_msg_u.dl_info_tx.type);
    return;
  }

  switch (rx_msg_ptr->dl_msg_type)
  {
    case LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER:
      HDR_MSG_PROT(MSG_LEGACY_MED,"=RX= Process dl info transfer");
      hdrsaprx_process_dl_info_transfer(
                   &(rx_msg_ptr->dl_msg_u.dl_info_tx.msg_container));
      break;
       
    default: 
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Received unsupported msg %d",
                   rx_msg_ptr->dl_msg_type);
      break;
  }

} /* hdrsaprx_process_lte_dl_msg */
#endif /* FEATURE_IRAT_DSM_SUPPORT */

/* EJECT */
/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_IND

DESCRIPTION
  This function processes indications given to SAPRX.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for SAPTX to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsaprx_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  HDRTRACE_IND(HDRHAI_SAP_RX_MODULE, ind_name, 0);

  switch (ind_name)
  {

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;

  } 
} /* hdrsaprx_process_ind */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAPRX_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to SAPRX.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsaprx_ind_cb (hdrind_ind_name_enum_type ind_name, void *ind_data)
{
  rex_tcb_type *hdrtx_tcb_ptr; /* temp ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "hdrsaprx_ind_cb %d", ind_name );

  /* This protocol runs in the tx task. */
  hdrtx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
  if (rex_self () == hdrtx_tcb_ptr)
  {
    /* Indication is from another protocol in the same task, so consider it
     * an internal indication
     */
    hdrsaprx_process_ind (ind_name, (hdrind_ind_data_union_type *)ind_data);
  }
  else
  {
    hdrrx_queue_ind (HDRHAI_SAP_RX_MODULE, ind_name, ind_data);
  }
} /* hdrsaprx_ind_cb() */

#endif /* FEATURE_LTE_TO_HDR_OH */
