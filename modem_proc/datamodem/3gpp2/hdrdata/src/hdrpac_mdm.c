/*===========================================================================

                         H D R P A C _ M D M . C
GENERAL DESCRIPTION
This file contains the HDRPAC functions in the modem side (in a multi processor environment.

 Copyright (c) 2009-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrpac_mdm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sc      Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
12/06/11   vpk     HDR QCHAT API
11/18/11   vpk     HDR SU API cleanup
08/16/11   vpk     Move hdr log packet definitions to hdrdata
07/26/11   vpk     Move away from hdrbit header
07/20/11   vpk     Initialize critical sections on powerup
06/20/11   ps      added hdrpac_api.h header file
05/12/11   op      Migrated to MSG 2.0 macros
02/08/11   ls      Global variable cleanup
12/27/10   ms      Replaced REX premption macros with critical section for 
                   mutual exclusion.
12/21/09   spn     Fix Mis-Align issue, by copying into a local variable 
                   instead of dereferencing the parameter directly.
12/18/09   ms      Qchat related API changes.
04/09/09   ls      Added hdrpac_get_rtcmac_flow() 
11/07/08   cs      Compiler warning fix.
04/10/07   ac      Fix the AN authentication error caused by dsrlp_init()
01/08/07   yll     Added support for CSNA.
10/16/06   kvd     Moved hdrpac_get_sn_ppp_status_from_nv from hdrpac.c
09/08/06   spn     Routing the messages to reservation module. 
07/24/06   mpa     Moved hdrpac_get_rtcmac_flow() to hdrpac.c
                   Initialize hdrmrlp NV items in HDRMC task context.
07/11/06   vrk     handle cases when hdrcom_ind_data_union_type can be null
07/11/06   mpa     Added various includes to fix compilation issues on 6800.
                   Fixed call to hdrrmac3_get_flow_mapping()
06/20/06   mpa     Moved hdrpac_init() from hdrpacc_powerup_init() to 
                   hdrpac_mdm_powerup_init() - called from hdrmc.
01/30/06   vrk     File created to support RPC for DO Rev 0. This hdrpac file 
                   resides in the modem and is mainly created by moving functions 
                   from hdrpacdsif and hdrpac
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "data_msg.h"
#include <stringl/stringl.h>

#if defined(FEATURE_HDR)

#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_EMPA
#include "hdrrsp.h"
#endif /* FEATURE_HDR_EMPA */
#endif /* FEATURE_HDR_QOS */



#ifdef FEATURE_HDR_QOS
#include "hdrmrlpresv.h"
#endif /* FEATURE_HDR_QOS */


#include "data_err_suppr.h"

#include "dshdr_an_mgr.h"
#include "hdrcom_api.h"
#include "hdrds.h"
#include "hdrrlp_util.h"
#include "hdrpac.h"
#include "hdrpac_api.h"
#include "hdrmrlp.h"
#include "hdrmrlpctl.h"
#include "dsutil.h"
#include "hdrrlp.h"


#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================

            MACRO DEFINITIONS FOR THE MODULE

===========================================================================*/

/* Mesage ID Unpacking routine */
#define HDRPAC_UNPACK_MSG_ID( ptr, val_ptr )  \
           HDRBIT_UNPACK8( ptr, val_ptr, 0,  8 )

/*===========================================================================
                              EXTERN VARIABLE DECLARATIONS AND DEFINITIONS
===========================================================================*/
extern rex_crit_sect_type hdrrlp_enhanced_dormant_bitmap_crit_sect;

/*===========================================================================

            FUNCTION DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================
FUNCTION      HDRPAC_DIPSW_ENABLE_VT_RATE_INERTIA

DESCRIPTION   To allow a test interface to enable vt rate inertia, 
              we need an interface that doesn't return an errno.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

static void hdrpac_dipsw_enable_vt_rate_inertia (void)
{ 
  (void) hdrpac_enable_vt_rate_inertia(); 
}

/*===========================================================================
FUNCTION      HDRPAC_DIPSW_DISABLE_VT_RATE_INERTIA

DESCRIPTION   To allow a test interface to disable vt rate inertia, 
              we need an interface that doesn't return an errno.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void hdrpac_dipsw_disable_vt_rate_inertia (void)
{ 
  (void) hdrpac_disable_vt_rate_inertia(); 
}

/*===========================================================================
FUNCTION  HDRPAC_MDM_POWERUP_INIT

DESCRIPTION
  This function does the initializations for the PAC on power up.

DEPENDENCIES
  Needs to be called after hdrstream initialization.
  Init for DS.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_mdm_powerup_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdrcom_log_set_dipsw_cb(HDRLOG_DIPSW_VT_RATE_INERTIA,
                      hdrpac_dipsw_enable_vt_rate_inertia,
                      hdrpac_dipsw_disable_vt_rate_inertia);

  hdrmrlp_powerup_init();

  /*---------------------------------------------------------------------------
  Call the function to initialize PAC. Notice that this function needs to be
  processed only after the hdrstream_powerup_init has happened. That is why
  the function ends up queuing a command for the DS task. Whenever a DS command
  is processed, we can be sure that the hdrstream_powerup_init has completed.
  ---------------------------------------------------------------------------*/
  hdrpac_init();

} /* hdrpac_mdm_powerup_init() */

/*===========================================================================
FUNCTION HDRPAC_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAPP1, HDRAPP2, HDRAPP3. Indications given 
  to PAC are processed in DS task context.

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
void hdrpac_ind_cb
(
  hdrcom_ind_name_enum_type ind_name,
  void *ind_data_ptr
)
{
  hdrcom_ind_data_union_type dummy_ind_data;
  DATA_MDM_ARG_NOT_CONST(ind_data_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  memset ( &dummy_ind_data, 0, sizeof(hdrcom_ind_data_union_type) );
  if(ind_data_ptr!=NULL)
  {
    memscpy ( &dummy_ind_data, 
              sizeof(hdrcom_ind_data_union_type), 
              (hdrcom_ind_data_union_type*)ind_data_ptr,
              sizeof(hdrcom_ind_data_union_type) );

    hdrds_queue_ind_ext( HDRHAI_PACKET_APPLICATION_CONTROLLER, ind_name, 
                         dummy_ind_data );
  }
  else
  {
    /* handle cases where ind_data_ptr can be null as in HDRIND_RUP_CONNECTION_INITIATED */
    dummy_ind_data.gaup_trans_id = 0;
    hdrds_queue_ind_ext( HDRHAI_PACKET_APPLICATION_CONTROLLER, ind_name, dummy_ind_data );
  }
} /* hdrpac_ind_cb() */


/*===========================================================================

FUNCTION   HDRPAC_MDM_AN_PROCESS_CM_CALL_CONNECTED_CMD

DESCRIPTION   
  This function processes the Call Connected Event. 

DEPENDENCIES  
  Note that this function gets called in the HDRMC task context. So this
  function needs to be task safe.
  
RETURN VALUE  
  None.
  
SIDE EFFECTS  
  AN RLP may be resumed.
  
===========================================================================*/
void hdrpac_mdm_an_process_cm_call_connected_cmd
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  

  if(hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_AN) )
  {
    DATA_HDR_MSG0(MSG_LEGACY_MED, "Start AN PPP. Init AN RLP");
    
    /* The AN RLP is already initalized on session open, now start the RLP 
       timers */
      dsrlp_init( (uint8)DSRLP_HDR_AN_AUTH_SR_ID_INDEX,
        DSRLPI_HDR_AN_AUTH_SR_ID_VALUE,
          DSRLP_VER_HDR,
          FALSE,
          DSRLP_VER_MFPA );

#ifdef FEATURE_DATA_OPTHO
        /*--------------------------------------------------------------------- 
          In Tunnel Mode, AN does not need to wait for AT to transmit
          reverse link (which is the case with direct mode) and it can
          send RLP data immediately after sending LinkOpenConf.
          So we didnot enable AN flow when the session was opened and only
          after RLP is inited here, we enable it. There is a off chance where
          even though optho is disabled, we might not have enabled AN flow
          when session opened. Ensure that we enable it here (if disabled)
          since AN call got connected
        ---------------------------------------------------------------------*/
        if(hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_AN)== FALSE )
        {
          DATA_HDR_MSG0(MSG_LEGACY_MED,"Enabling flow on AN stream");
          hdrpac_flow_enable( HDRHAI_DEF_PACKET_APP_AN );
        }
#endif /* FEATURE_DATA_OPTHO */
  }
  else
  {
    DATA_HDR_MSG0(MSG_LEGACY_LOW,"AN stream not negotiated. Call Conn ignored");
  }
} /* hdrpac_mdm_an_process_cm_call_connected_cmd() */

/*===========================================================================

FUNCTION   HDRPAC_MDM_AN_PROCESS_CM_CALL_END_CMD

DESCRIPTION   
  This function processes the Call End Event from CM. 

DEPENDENCIES  
  Note that this function gets called in the HDRMC task context. So this
  function needs to be task safe.

RETURN VALUE  
  None
  
SIDE EFFECTS  
  AN RLP may be suspended and reset.
  
===========================================================================*/
void hdrpac_mdm_an_process_cm_call_end_cmd
(
  void
)
{
  uint8 rlp_id;    /* RLP Identifier */
  boolean rlp_up;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  
  /* ----------------------------------------------------------------------
    Note that we do not check for whether the AN stream is assigned or not,
    simply whether the RLP has been initialized. This is because the end 
    command may be received after the HDR session is closed, in which case,
    the check for if session is open or if stream is assigned for AN will
    fail. By checking if it is initialized, we ensure any time the AN RLP
    has been initialized, we clean it up. The AN RLP will only get 
    initialized if the AN stream had been negotiated.
  -----------------------------------------------------------------------*/
  rlp_up = dsrlp_is_inited( DSRLP_HDR_AN_AUTH_SR_ID_INDEX );

  if( rlp_up == TRUE )
  {
    /* ----------------------------------------------------------------------
      Note that we do not use the PAC macros hdrpac_map_app_subtype_to_rlp_id
      since the HDR session may have been closed when this function is called.
      Using dsrlp_get_sr_id() ensures that the RLP that had been initialized 
      is cleaned up. 
    -----------------------------------------------------------------------*/
    rlp_id = dsrlp_get_sr_id( DSRLP_HDR_AN_AUTH_SR_ID_INDEX );
    
    
    DATA_HDR_MSG0(MSG_LEGACY_MED, "Cleanup AN RLP");
    
    ASSERT( rlp_id == DSRLPI_HDR_AN_AUTH_SR_ID_VALUE );
    /*lint -e774 */
    if ( rlp_id == DSRLPI_HDR_AN_AUTH_SR_ID_VALUE )
    {
      /* Cleanup AN RLP */
      dsrlp_cleanup( rlp_id, DSRLP_VER_HDR );
    }
    /*lint +e774 */
    else
    {
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Invalid AN RLP ID:%d", rlp_id);
    }
  }
  else
  {
    DATA_HDR_MSG0(MSG_LEGACY_LOW,"AN RLP not initialized. Call End ignored");
  }
} /* hdrpac_mdm_an_process_cm_call_end_cmd() */

/*===========================================================================
FUNCTION      HDRPAC_MDM_SN_INIT

DESCRIPTION   Initializes RLP for HDR SN Default Packet App.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpac_mdm_sn_init
(
  void
)
{
  uint32 hi = 0;
  uint32 lo = 0;
  
  DATA_HDR_MSG0(MSG_LEGACY_MED, "Init SN RLP"); 

  /*-----------------------------------------------------------------------
    Enter critical section to protect RLP state info.
  -----------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&hdrrlp_enhanced_dormant_bitmap_crit_sect);
    qw_set(*get_hdrrlp_enhanced_dormant_bitmap_ptr(), hi, lo);

  /*-----------------------------------------------------------------------
    Leave critical section.
  -----------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&hdrrlp_enhanced_dormant_bitmap_crit_sect);

  if(hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN) )
  {
    /*---------------------------------------------------------------------
      Initialize the HDR SN RLP. 
    ---------------------------------------------------------------------*/
    dsrlp_init( (uint8)DSRLP_HDR_SN_RLP_SR_ID_INDEX,
        DSRLP_HDR_SN_RLP_SR_ID_VALUE,
                DSRLP_VER_HDR,
                FALSE,
                DSRLP_VER_MFPA );

  }

#ifdef FEATURE_HDR_QOS
  /*---------------------------------------------------------------------
  If the MultiFlow RLP stream is defined, then initialize MRLP
  ---------------------------------------------------------------------*/
  if(hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
     hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
	 hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) )
  {
    hdrmrlpctl_init_all_rlp();
  }
#endif /* FEATURE_HDR_QOS */

  /*-------------------------------------------------------------------------
    Initialize the AN RLP
  -------------------------------------------------------------------------*/
  hdrpac_mdm_an_process_cm_call_connected_cmd();

#ifdef FEATURE_HDR_EMPA
  /*---------------------------------------------------------------------
  Init RSP, if doing EMPA
  ---------------------------------------------------------------------*/
  if (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
	  hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    hdrrsp_init();
  }
#endif /* FEATURE_HDR_EMPA */
} /* hdrpac_mdm_sn_init() */

/*===========================================================================
FUNCTION      HDRPAC_MDM_SN_CLEANUP

DESCRIPTION   Cleans up RLP for HDR SN Default Packet App. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpac_mdm_sn_cleanup
(
  void
)
{
  uint8 sr_id;
  byte rscb_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  rscb_index = DSRLPI_INVALID_RSCB_INDEX;
  /*---------------------------------------------------------------------
    If the HDR SN RLP has been initialized, then clean it up. We
    determine whether the RLP is initialized by trying to get the rscb
    index for the RLP. If it returns an invalid value, that means the
    RLP no longer exists and there is no need to clean it up. Otherwise
    need to clean. 
    Note that the function hdrpac_map_app_subtype_to_rlp_id returns
    the rlp_id for default AN and SN even if session is closed because
    AN and SN have fixed sr_id values.
  ---------------------------------------------------------------------*/
  sr_id = DSRLP_HDR_SN_RLP_SR_ID_VALUE; 
  rscb_index = dsrlp_get_rlp_rscb_index(sr_id, DSRLP_VER_HDR );
  if( rscb_index != DSRLPI_INVALID_RSCB_INDEX )
  {
    dsrlp_cleanup(sr_id, DSRLP_VER_HDR);
  }
#ifdef FEATURE_HDR_QOS
  /*---------------------------------------------------------------------
  Cleanup any Multiflow RLPs
  ---------------------------------------------------------------------*/
 hdrmrlpctl_cleanup_all_rlp();
#endif /* FEATURE_HDR_QOS */

  /*-------------------------------------------------------------------------
    Cleanup the AN RLP
  -------------------------------------------------------------------------*/
  hdrpac_mdm_an_process_cm_call_end_cmd();

#ifdef FEATURE_HDR_EMPA
  /*---------------------------------------------------------------------
  Cleanup RSP, if doing EMPA
  ---------------------------------------------------------------------*/
  if (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
	  hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    hdrrsp_cleanup();
  }
#endif /* FEATURE_HDR_EMPA */
} /* hdrpac_mdm_sn_cleanup() */

/*===========================================================================
FUNCTION HDRPAC_CLOSE_CONNECTION

DESCRIPTION
  This function closes a connection through the hdr protocol (ALMP). 

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void hdrpac_close_connection( void )
{
  
  hdrcp_almp_close_connection( HDRHAI_ABORT_NORMAL_CLOSE, 
                            HDRHAI_PACKET_APPLICATION_CONTROLLER );

} /* hdrpac_close_connection() */

/*===========================================================================

FUNCTION HDRPAC_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-use instance.
 
  in-use messages are routed further based on the particular APP that is 
  bound to the stream on which message was received and specific protocol 
  (FCP, RLP or LUP) for which this message is meant for, in that 
  application.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpac_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  uint8 msg_id;      /* Message ID */
  uint8 protocol_id; /* Protocol ID (stream the message was received on)*/
  hdrcom_hai_stream_enum_type stream_id; /* stream the message was rxed on */
  hdrcom_hai_app_subtype_enum_type app_subtype[4]; /* app subtypes for associated 
                                     streams */
  uint8 def_pkt_app_rlp_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Ensure the msg_ptr is valid.  If not, log an error and just return,
    since it is already NULL.
  -------------------------------------------------------------------------*/
  if (msg_ptr == NULL)
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "NULL msg_ptr passed to hdrpac_msg_cb()");
    return;
  }   

  /* Extract the Message ID */
  if( HDRPAC_UNPACK_MSG_ID( msg_ptr, &msg_id ) != E_SUCCESS )
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Error Unpacking Msg ID");
    dsm_free_packet( &msg_ptr );
    return;
  }
  
  /*---------------------------------------------------------------------------
     Get the protocol id (stream) associated with the message.   
  ---------------------------------------------------------------------------*/
  protocol_id = (uint8)HDRCOM_DSM_GET_PROT_ID(msg_ptr) ;

  switch( protocol_id )
  {
    case HDRHAI_N_APP1_TYPE:
      stream_id = HDRHAI_STREAM1;
      break;

    case HDRHAI_N_APP2_TYPE:
      stream_id = HDRHAI_STREAM2;
      break;

    case HDRHAI_N_APP3_TYPE:
      stream_id = HDRHAI_STREAM3;
      break;

    default:
      /* Should never happen, since Stream 0 message should never be routed
      ** to PAC */
      stream_id = HDRHAI_STREAM0;
      ERR_FATAL("Invalid Protocol ID %d", protocol_id, 0, 0 );
      break;
  }

  
  if ( hdrcp_stream_get_stream_config( app_subtype ) == FALSE )
  {
    ERR_FATAL("Not Able to get Stream Config ", 0, 0, 0 );
  }


  switch( app_subtype[stream_id] )
  {
    case HDRHAI_DEF_PACKET_APP_AN:
    case HDRHAI_DEF_PACKET_APP_SN:

      switch( msg_id )
      {
        case HDRRLPI_SIG_RST :
        case HDRRLPI_SIG_RSTACK:
        case HDRRLPI_SIG_NAK:
              
          def_pkt_app_rlp_id = hdrrlp_get_rlp_id_for_def_app_subtype( 
                                    app_subtype[stream_id] );
          /* Route message to RLP */
          hdrrlp_msg_cb( msg_ptr, def_pkt_app_rlp_id );
          break;
    
        case HDRCP_LUP_LOC_REQ_MSG:
        case HDRCP_LUP_LOC_ASSIGN_MSG:
          /* Route message to LUP */
          hdrcp_lup_msg_cb( msg_ptr );
          break;
    
        case HDRCP_FCP_XON_RSP_MSG_ID:
        case HDRCP_FCP_XOFF_RSP_MSG_ID:
        case HDRCP_FCP_DATA_READY_MSG_ID:
          /* Route message to FCP */
          hdrcp_fcp_msg_cb( msg_ptr );
          break;
    
        default:
          DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Unknown in use Msg ID: %d", msg_id);
          dsm_free_packet( &msg_ptr );
          break;
      }
    break;

#ifdef FEATURE_HDR_QOS
    case HDRHAI_MULTIFLOW_PACKET_APP_SN:
    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
    case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
      
      switch( msg_id )
      {
        case HDRMRLP_SIG_RESET_TX_INDICATION:
        case HDRMRLP_SIG_RESET_RX_INDICATION:
        case HDRMRLP_SIG_NAK:
        case HDRMRLP_SIG_RESET_TX_INDICATION_ACK:
        case HDRMRLP_SIG_RESET_TX_COMPLETE:
        case HDRMRLP_SIG_RESET_RX_COMPLETE:
	case HDRMRLP_SIG_FLUSH:
          /* route this message to ml-rlp. */
          hdrmrlp_msg_cb( msg_ptr, app_subtype[stream_id] );
          break;

        case HDRLMRLP_SIG_RESERVATION_ACCEPT:
        case HDRLMRLP_SIG_RESERVATION_REJECT:
        case HDRLMRLP_SIG_REV_RESERVATION_ON:
        case HDRLMRLP_SIG_REV_RESERVATION_OFF:
        case HDRLMRLP_SIG_FWD_RESERVATION_OFF:
        case HDRLMRLP_SIG_FWD_RESERVATION_ON:
          hdrmrlpresv_msg_cb( msg_ptr );
          break;
        
        case HDRCP_LUP_LOC_REQ_MSG:
        case HDRCP_LUP_LOC_ASSIGN_MSG:
        case HDRCP_LUP_SNID_REQ_MSG:
        case HDRCP_LUP_SNID_ASSIGN_MSG:
          /* Route message to LUP */
          hdrcp_lup_msg_cb( msg_ptr );
          break;
        
        case HDRCP_FCP_XON_RSP_MSG_ID:
        case HDRCP_FCP_XOFF_RSP_MSG_ID:
        case HDRCP_FCP_DATA_READY_MSG_ID:
          /* Route message to FCP */
          hdrcp_fcp_msg_cb( msg_ptr );
          break;

#ifdef FEATURE_HDR_DOS
        case HDRCP_DOS_DATA_OVER_SIGNALING_MSG_ID:
#ifdef FEATURE_QCHAT 
        if (hdrcp_qchatupk_mod_enabled()) 
        { 
          hdrcp_qchatupk_handle_data_over_signalling_msg_id(); 
          /* Route message to DOS */ 
          hdrcp_dos_msg_cb( msg_ptr ); 
          break; 
        }  
#endif /* FEATURE_QCHAT */ 

        case HDRCP_DOS_DATA_OVER_SIGNALING_ACK_MSG_ID:
          /* Route message to DOS */
          hdrcp_dos_msg_cb( msg_ptr );
          break;
#endif /* FEATURE_HDR_DOS */

#ifdef FEATURE_HDR_EMPA
        case HDRRSP_ROUTE_SELECT_MSG_ID:
        case HDRRSP_ACTIVATE_ROUTE_MSG_ID:
            /* Route message to RSP */
          hdrrsp_msg_cb( &msg_ptr );
          break;
#endif /* FEATURE_HDR_EMPA */

        default:
          DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Unknown in use Msg ID: %d", msg_id);
          dsm_free_packet( &msg_ptr );
          break;
      }
    break;
  
  case HDRHAI_MULTIFLOW_PACKET_APP_AN:
    DATA_HDR_MSG1(MSG_LEGACY_ERROR,
                  "Got ML-RLP Sig Msg[%d] for AN MFPA. Ignoring",
                  msg_id);
    break;
#endif /* FEATURE_HDR_QOS */

    case HDRHAI_DEF_PACKET_APP_IS890:
    case HDRHAI_ENH_TEST_APP_IS890A:
    case HDRHAI_MC_TEST_APP_IS890B:
#if defined ( FEATURE_HDR_IS890 ) || defined( FEATURE_HDR_IS890A )
      hdrcp_is890_msg_cb( msg_ptr );
#else
      DATA_HDR_MSG0(MSG_LEGACY_ERROR, "IS-890/a not supported in this build");
      dsm_free_packet( &msg_ptr );
#endif /* FEATURE_HDR_IS890 or FEATURE_HDR_IS890A */
    break;

#ifdef FEATURE_HDR_CSNA
    case HDRHAI_3G1X_CSN_APP:
      /* Route messages to CSNA */
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "CSNA: 3G1X message (id=%d) received",
                    msg_id);
      hdrcsna_msg_cb( msg_ptr );
      break;
#endif /* FEATURE_HDR_CSNA */

    default:
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Found invalid app subtype %d",
                    app_subtype[stream_id]);
      dsm_free_packet( &msg_ptr );
    break;
  } /* switch app_subtype */
} /* hdrpac_msg_cb() */

/*===========================================================================
FUNCTION HDRPAC_GET_SN_PPP_STATUS_FROM_NV

DESCRIPTION
  This function reads the SN PPP status from NV. It return a boolean PPP
  status. 

DEPENDENCIES
  This function should be invoked only from hdrmc context. 

PARAMETERS

RETURN VALUE
  boolean SN PPP status

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrpac_get_sn_ppp_status_from_nv
(
  void
)
{
  nv_item_type hdrpac_nv_item;

  if( hdrcom_util_read_nv_item (NV_SN_PPP_STATUS_I, &hdrpac_nv_item) != NV_DONE_S )
  {
    /* Set the ppp status to false if it has never be written to */
    hdrpac_nv_item.sn_ppp_status = FALSE;
  }

  return hdrpac_nv_item.sn_ppp_status;

} /* hdrpac_get_sn_ppp_status_from_nv() */
/*===========================================================================
FUNCTION HDRPAC_GET_RTCMAC_FLOW

DESCRIPTION
  This function returns the RTCMAC Flow ID corresponding to the corresponding
  stream/substream. If the RTCMAC is SubType 0, then the Flow ID returned is
  0. If the RTCMAC is of SubType 3, then the appropriate RTCMAC Flow ID is
  returned. If the stream, substream_id combination is invalid, then the rtcmac
  flow returned is Invalid. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdrpac_get_rtcmac_flow
(
hdrcom_hai_stream_enum_type stream_id,
uint8 sub_stream_id 
)
{
  uint8 ret_val_rmac_flow_id = HDRCP_PCP_INVALID_MAC_FLOW_ID;
  uint16 rmac_subtype;
#ifdef FEATURE_HDR_REVA_L1
  uint8 rmac_flow_idx;
  int app_flow_idx;
  hdrl1_rmac_flow_mapping_struct_type flow_mapping_table[HDRL1_RMAC_MAX_NUM_MAC_FLOWS];
#endif /* FEATURE_HDR_REVA_L1 */
  boolean ignore_sub_stream_id = FALSE;
  hdrcom_hai_app_subtype_enum_type app_subtype;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
   Determine the RTCMAC Subtype 
  ---------------------------------------------------------------------*/
  rmac_subtype = hdrcp_scp_get_current_subtype( HDRHAI_RTC_MAC_PROTOCOL );

  /*---------------------------------------------------------------------
   Determine whether the sub_stream id should be looked at or not. For
   the Default Packet Apps (including Def Packet App bound to AN & AN
   and the Test Apps), or for the signaling packet app, the sub stream id 
   does not make sense
  ---------------------------------------------------------------------*/
  app_subtype = hdrpac_get_app_subtype_for_stream(stream_id);
  if (( app_subtype == HDRHAI_DEF_PACKET_APP_AN ) ||
      ( app_subtype == HDRHAI_DEF_PACKET_APP_SN ) ||
      ( app_subtype == HDRHAI_DEF_PACKET_APP_IS890 ) ||
      ( app_subtype == HDRHAI_DEF_SIG_APP ))
  {
    ignore_sub_stream_id = TRUE;
  }

  /*---------------------------------------------------------------------
   Based on the RTCMAC Subtype, determine the appropriate RMAC flow value
  ---------------------------------------------------------------------*/
  if ( rmac_subtype == HDRCP_SCP_DEFAULT_RMAC )
  {
    /* RTCMAC Subtype 0 is configured. Return the RMAC Flow value for this
       subtype */
    return HDRPCP_RMAC0_FLOW_ID;
  }
  else if ( rmac_subtype == HDRCP_SCP_SUB1_RMAC )
  {
    /* RTCMAC Subtype 1 is configured. Return the RMAC Flow value for this
       subtype */
    return HDRPCP_RMAC1_FLOW_ID;
  }
#ifdef FEATURE_HDR_REVA_L1
  else if (( rmac_subtype == HDRCP_SCP_SUB3_RMAC )
#ifdef FEATURE_HDR_REVB
           || (rmac_subtype == HDRCP_SCP_MC_RMAC)
#endif /* FEATURE_HDR_REVB */
          )
  {

    /*---------------------------------------------------------------------
     Go through the AssociatedFlowsNN attribute for each RMAC flow and check
     whether if there is a match for the stream/substream pair. If so, then
     mark the RMAC flow corrsponding to this.
    ---------------------------------------------------------------------*/
    for ( rmac_flow_idx=0;
        rmac_flow_idx < HDRL1_RMAC_MAX_NUM_MAC_FLOWS; 
        rmac_flow_idx++)
    {
      /*---------------------------------------------------------------------
       Get the entire flow mapping from RMAC
      ---------------------------------------------------------------------*/
      hdrl1_rmac_get_flow_mapping( &(flow_mapping_table[rmac_flow_idx]), 
                                 rmac_flow_idx );

      for ( app_flow_idx=0;
          app_flow_idx < flow_mapping_table[rmac_flow_idx].flow_count;
          app_flow_idx++ )
      {
        /*-------------------------------------------------------------------
         If the Stream ID for the RMAC flow indicates this is the default
         RMAC Flow, then in case the {stream,substream} is not bound to a 
         particular RLP, this will be the RMAC Flow that the {stream,substream} 
         gets bound to.
        -------------------------------------------------------------------*/
        if ( flow_mapping_table[rmac_flow_idx].app_flows[app_flow_idx].stream_id ==
             HDRL1_RMAC_FLOW_ID_DEFAULT)
        {
          ret_val_rmac_flow_id = flow_mapping_table[rmac_flow_idx].mac_flow_id;
        }

        if ( ignore_sub_stream_id == FALSE )
        {
		  /*lint -save -e641 supress enum to int warning 641*/

          if ((stream_id == 
               flow_mapping_table[rmac_flow_idx].app_flows[app_flow_idx].stream_id) &&
              (sub_stream_id ==
               flow_mapping_table[rmac_flow_idx].app_flows[app_flow_idx].sub_stream_id))
		  /*lint -restore Restore lint error 641*/

          {
            /*-------------------------------------------------------------------
             Got a match. Return the value of the RMAC Flow.
            -------------------------------------------------------------------*/
            ret_val_rmac_flow_id = flow_mapping_table[rmac_flow_idx].mac_flow_id;
            break;
          }
        }
        else
        {
		  /*lint -save -e641 supress enum to int warning 641*/
          if (stream_id == 
              flow_mapping_table[rmac_flow_idx].app_flows[app_flow_idx].stream_id)
          {
            /*-------------------------------------------------------------------
             Got a match. Return the value of the RMAC Flow.
            -------------------------------------------------------------------*/
            ret_val_rmac_flow_id = flow_mapping_table[rmac_flow_idx].mac_flow_id;
          }
          /*lint -restore Restore lint error 641*/
        }
      }
    }
  }
#endif /* FEATURE_HDR_REVA_L1 */

  return ret_val_rmac_flow_id;
} /* hdrpac_get_rtcmac_flow() */

/* <EJECT> */

#endif /* defined(FEATURE_HDR) */
