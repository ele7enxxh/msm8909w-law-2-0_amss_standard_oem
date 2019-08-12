/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   TM UMTS_UP SUPL module interface with PDCOMM.


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved. QUALCOMM Proprietary and Confidential.
Copyright (c) 2013-2015 QUALCOMM Atheros Incorporated.
All Rights Reserved.QUALCOMM Atheros Proprietary and Confidential.
Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
Confidential and Proprietary - Qualcomm Technologies, Inc.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_up/supl/src/tm_umts_up_supl_comm.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------   
  06/15/16   skm     Discard NI Hash request if already processing previous NI hash request
  06/06/15   skm     SUPL call flow without ESLP in MTES causes a loop retrying SLP if configured in NV.
  03/14/15   rh      Added support for MultiSIM NV
  03/10/15    lt     Proper handling of TLS version in SUPL messages for areaEventTriggered sessions. 
  09/22/14   skm     Add support for E-HSLP url generation for GSM
  07/19/14   skm     Fix for UDP socket closing on READ ERROR.
  05/19/14   skm     Fix mem leak issue in supl_comm_generate_hash
  12/23/13   mj	     Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM
  07/29/13   mj      Redesign ACKing for WMS msgs
  12/06/13    ah     Support for non-deferrable timers
  05/21/13   ssh     Supporting no-usage of autoconfigured SLP addresses.
  05/14/13   ssh     Handle TCP connections in back to back SUPL sessions
  03/26/12    rk     Set the SUPL root cert directory upon boot-up unconditionally.
  12/14/09    gk     send comm failure event
  04/06/11    lt     ASN1 tool migration.
  03/03/10    ns     DSDS Support - read cert from efs if new cert 
                     injected via pdapi,also read imsi from mmgsdi module
  12/14/09    gk     send comm failure event
  11/02/09   atien   additional fix for the case where one segmented MMS msg 
                     wipes out previously stored segmented SUPL-INIT msg in 
                     local WAP PUSH buffer
  09/25/09   atien   Fix problem with dropping SUPL-Init msg while processing
                     multi-segmented non-SUPL msg
===========================================================================*/
#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_CGPS_UMTS_UP_SUPL

/*==========================================================================

                   Include Files

===========================================================================*/
#include <limits.h>
#include "fs_public.h"
#include "secssl.h"
#include "secssldef.h"
#include "sm_log.h"
#include "pd_comms_api.h"
#include "tm_data.h"
#include "tm_common.h"
#include "tm_asn1util.h"

#include "tm_rrlp_asn1.h"


#include "tm_umts_common_utils.h"
#include "tm_sim_utils.h"

#include "sys.h"

#define TM_SUPL_COMM_USE
#include "tm_umts_up_supl.h"
#undef  TM_SUPL_COMM_USE

#define PDCOMM_INFO_DECLARED
#include "tm_umts_up_supl_comm.h"
#undef PDCOMM_INFO_DECLARED


LOCAL pd_comms_connect_params_type z_comms_connect_params_sms;
LOCAL pd_comms_connect_params_type z_comms_connect_params_tcp;
LOCAL pd_comms_connect_params_type z_comms_connect_params_udp;


#define C_SUPL_NUM_BYTES_IN_LENGTH_FIELD    2
#define C_MAX_PDCOMM_RX_BUFF_LENGTH         2048

//LOCAL uint8 pdcomm_rx_buff[C_MAX_PDCOMM_RX_BUFF_LENGTH];
LOCAL uint16 pdcomm_rx_buff_len;

/* will save SUPL INIT source (1X/GSM SMS, WAP-PUSH or UDP)   
  Current implementation has a (rare) race condition when multiple consecutive SUPL 
  Inits are being sent from different source(1xSMS, GSMSMS, WapPush), we'll report 
  the last one (This is a very rare scenario, will be fixed later if necessary) 
  TM/PDCOMM-WMS tasks may access this variable at the same time. */
pdsm_lcs_supl_init_src_e_type z_supl_init_source = PDSM_SUPL_INIT_SOURCE_TYPE_UNKNOWN; 

/* tm_common.h defines the following 
   #define TM_UMTS_UP_TRASACTION_ID_RANGE        0x02000000
   UMTS UP protocol sub-module is allowed to use transaction ID from 0x02000000 - 0x02FFFFFF

   If more UMTS UP submodules are developed, adjust this.
*/
#define TM_UMTS_UP_SUPL_TRASACTION_ID_BASE  TM_UMTS_UP_TRANSACTION_ID_RANGE
#define TM_UMTS_UP_SUPL_TRANSACTION_ID_RANGE 0x02000000 

/* SMS transaction ID: 0x02000000 to 0x0200FFFF inclusive */
#define TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_BASE      TM_UMTS_UP_SUPL_TRASACTION_ID_BASE
#define TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_RANGE     0x00010000

/* TCP transaction ID: 0x02010000 to 0x0201FFFF inclusive */
#define TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_BASE     (TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_BASE + TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_RANGE)
#define TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_RANGE    0x00010000

/* HASH transaction ID: 0x02020000 to 0x0202FFFF inclusive */
#define TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE    (TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_BASE + TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_RANGE)
#define TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_RANGE   0x00010000

/* HASH transaction ID: 0x02030000 to 0x0203FFFF inclusive */
#define TM_UMTS_UP_SUPL_UDP_TRANSACTION_ID_BASE    (TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE + TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_RANGE)
#define TM_UMTS_UP_SUPL_UDP_TRANSACTION_ID_RANGE   0x00010000


/* OMNA assigned number for registered PUSH Application ID */
#define OMNA_APP_ID (0xAF)

typedef enum
{
  TRANSAC_SMS,
  TRANSAC_TCP,
  TRANSAC_HASH,
  TRANSAC_UDP
}transac_id_group_e_type;

static  uint8 wap_push_buffer[1024];
static  uint16 wap_push_buffer_len = 0;
static  uint8 *push_content_ptr;
static  uint16 push_content_len = 0;
static uint8 wap_push_multipart_in_progress = FALSE;
static uint8 wap_push_multipart_msg_ref = 0;
static uint8 wap_push_multipart_total_sm = 0;
static uint8 wap_push_multipart_seq_num = 0;
static uint8 wap_push_multipart_supl_init_found = FALSE;


static  uint32 q_current_hash_transac_id = TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE;

static uint8 numMncDigits = 0;

static uint8 supl_pdcomm_udp_bind_retry_count = 0;

static boolean b_pending_hash = FALSE;

static boolean supl_comm_get_slp_address_portnum( pd_comms_tcp_connect_params_type  *p_connect_params );
static pd_comms_return_type supl_comm_read(uint16 length, pd_comms_link_type link_type, boolean wait_all );
static pd_comms_return_type supl_comm_disconnect(boolean abort );

static boolean supl_comm_hslp_address_build_from_imsi (char * constructedHslpAddr);

static boolean supl_comm_sms_verify_read_cb 
(
  const wms_client_message_s_type *p_data,
  wms_client_ts_data_s_type *p_ts_data,
  uint8 *p_MesgContent,
  uint16 *p_MesgLen,
  uint16 w_MesgContentMaxLen,
  boolean u_isWapPush 
);

static boolean supl_comm_dsm_verify_partial_wap_push
( uint8 *data, uint16 data_len, 
  uint8 **ppPushContent, 
  uint16 *pPushLen );

static boolean supl_comm_dsm_verify_wap_push
( uint8 *data, uint16 data_len, 
  uint8 **ppPushContent, 
  uint16 *pPushLen );

static uint32 supl_comm_getnext_transaction_id(transac_id_group_e_type transac_group);
static boolean supl_comm_generate_hash ( uint8 *p_raw_pdu, uint16 w_raw_pdu_len );

static boolean supl_comm_eslp_address_build_from_mcc_mnc (char * constructedHslpAddr, cgps_CellInfoCachedStruct *p_cached_cell_info);

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_tcp_connect

DESCRIPTION
  This function is used to establish a TCP link to SLP.

RETURN VALUE
  pd_comms_return_type

DEPENDENCIES

===========================================================================*/
pd_comms_return_type tm_umts_up_supl_comm_tcp_connect( uint8 u_supl_version_main )
{
  pd_comms_return_type    pdcomm_connect_ret_value = PDCOMM_RESULT_ERROR_UNKNOWN;
  pd_comms_tcp_connect_params_type  *p_tcp_params;
  
  if(z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED)
  {
    /*Already connected*/
    return PDCOMM_RESULT_OK; 
  }

  if(z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTING)
  {
    /*Already connected*/
    return PDCOMM_RESULT_WAIT; 
  }
  
   /*
   *  Re-fetch the HSLP Address (URL, (optional) IP Address, and
   *  (optional) IP Port) from NV just in case new values have been
   *  injected through PDAPI.
   */

  
  /*
   *  Fetch the "global" HSLP Hostname, IP Addr, etc. and store in locals
   *  for better readability.
   */ 
  p_tcp_params = &z_comms_connect_params_tcp.z_link_params.z_tcp_params;

  if(!supl_comm_get_slp_address_portnum(p_tcp_params))
  {
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  z_comms_connect_params_tcp.e_link_type = LINK_TCP;
  z_comms_connect_params_tcp.q_transaction_id = TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_BASE;

  if ( (z_umts_nv_item_ram_copy.aagps_use_transport_security & 0x1) == 0 )
  {
    z_comms_connect_params_tcp.e_use_security = SECURITY_NONE; 
  }
  else
  {
    z_comms_connect_params_tcp.e_use_security = SECURITY_SUPL;
    z_comms_connect_params_tcp.e_ssl_ver_tls = SSL_VER_TLS_1_0;
    
    if ( u_supl_version_main == SUPL_VERSION_MAIN_2 )
    {
      if ( ( z_umts_nv_item_ram_copy.aagps_use_transport_security & 0x2 ) == 0 )
      {
        z_comms_connect_params_tcp.e_ssl_ver_tls = SSL_VER_TLS_1_1;
      }
    }

    MSG_MED("TLS version being passed is %d", z_comms_connect_params_tcp.e_ssl_ver_tls ,0,0);

  }



  z_comms_connect_params_tcp.q_transaction_id = supl_comm_getnext_transaction_id(TRANSAC_TCP);
  /*If there is a PDCOMM error before disconnect is request, PDCOMM will be using
    connect transaction id for notifying the event*/
  z_pdcomm_tcp_info.disconnect_transaction_id = z_comms_connect_params_tcp.q_transaction_id;
  
  /*Fill in the APN profile type to used*/
  if(supl_session_is_emergency_session())
  {
    z_comms_connect_params_tcp.z_link_params.z_tcp_params.profile_type = PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY;

    z_pdcomm_tcp_info.emergency_connection = TRUE;
  }
  else
  {
    z_comms_connect_params_tcp.z_link_params.z_tcp_params.profile_type = PDCOMM_APN_PROFILE_TYPE_LBS;    
    z_pdcomm_tcp_info.emergency_connection = FALSE;
  }
  pdcomm_connect_ret_value = pd_comms_app_connect( z_pdcomm_handle_tcp, 
                                                   THREAD_ID_SM_TM, 
                                                   TM_CORE_PDCOMM_MSG_ID_BASE, 
                                                   &z_comms_connect_params_tcp );

  MSG_MED("TCP connect tr_id = %lx. CONNECTING", z_comms_connect_params_tcp.q_transaction_id, 0, 0 ); 

  if(PDCOMM_RESULT_WAIT == pdcomm_connect_ret_value)
  {
    z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_CONNECTING;
  }
   
  return pdcomm_connect_ret_value;



}


/*===========================================================================

FUNCTION tm_umts_up_supl_comm_udp_bind

DESCRIPTION
  This function is used to bind a UDP socket.

RETURN VALUE
  void

DEPENDENCIES

===========================================================================*/
void tm_umts_up_supl_comm_udp_bind( boolean srv_system_changed )
{
  pd_comms_bind_params_type bind_params;  
  pd_comms_return_type      pdcomm_bind_ret_value; 

  if(srv_system_changed)
  {
    supl_pdcomm_udp_bind_retry_count = 0;
  }

  if(supl_udp_bind_retry_timer != NULL)
  {
    /*Stop any ongoing UDP bind timer*/
    os_TimerStop(supl_udp_bind_retry_timer);
  }
  
  bind_params.e_link_type = LINK_UDP;
  bind_params.q_transaction_id = supl_comm_getnext_transaction_id(TRANSAC_UDP);
  bind_params.z_link_params.z_tcp_params.w_ip_port = z_comms_connect_params_udp.z_link_params.z_tcp_params.w_ip_port;
  
  pdcomm_bind_ret_value =  pd_comms_app_bind( z_pdcomm_handle_udp, 
                                              THREAD_ID_SM_TM, 
                                              TM_CORE_PDCOMM_MSG_ID_BASE, 
                                              &bind_params );

  
  if ( ( pdcomm_bind_ret_value != PDCOMM_RESULT_OK ) && ( pdcomm_bind_ret_value != PDCOMM_RESULT_WAIT ) )
  {    
    MSG_HIGH("UDP bind failed",0,0,0);
    if(supl_pdcomm_udp_bind_retry_count < SUPL_UDP_BIND_MAX_RETRY_COUNT)
    {
      /*Start the UDP retry timer for retrying*/
      MSG_HIGH("Starting the udp bind retry timer",0,0,0);
      supl_pdcomm_udp_bind_retry_count++;
      (void) os_TimerStart(supl_udp_bind_retry_timer, SUPL_UDP_BIND_RETRY_VALUE_MSEC, NULL );
    }
    else
    {      
      MSG_HIGH("Max number of UDP bind retries for this RAT over",0,0,0);
    }
  }

}

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_abort_udp_bind_retries

DESCRIPTION
  This function is used to abort any UDP ding retries.

RETURN VALUE
  void

DEPENDENCIES

===========================================================================*/
void tm_umts_up_supl_comm_abort_udp_bind_retries(void)
{

  /*Stop any ongoing UDP bind timer*/
  if ( os_TimerCheck( supl_udp_bind_retry_timer) )
  {
    os_TimerStop(supl_udp_bind_retry_timer);
  }

  supl_pdcomm_udp_bind_retry_count = SUPL_UDP_BIND_MAX_RETRY_COUNT;
}

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_get_tcp_state

DESCRIPTION
 Get the current Tcp state of PDCOMM

RETURN VALUE
  Enum representing the current Tcp PDCOMM state

DEPENDENCIES

===========================================================================*/ 
pd_comms_state_type tm_umts_up_supl_comm_get_tcp_state(void)
{
  return z_pdcomm_tcp_info.curr_state;
}

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_init

DESCRIPTION
  This function is used to initialize pdcomm for SUPL.

RETURN VALUE
  Boolean - TRUE for success, FALSE for failure

DEPENDENCIES

===========================================================================*/

boolean tm_umts_up_supl_comm_init ( void )
{
  pd_comms_get_handle_params_type     z_pdcomm_get_handle_params;
  pd_comms_return_type                pdcomm_connect_ret_value; 

  boolean error_flag = FALSE;

  /**** init TCP **************************************************/

  z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_DISCONNECTED;
  z_pdcomm_tcp_info.recv_state = TM_SUPL_COMM_RECV_HEADER;
  z_pdcomm_tcp_info.num_bytes_to_read = 0;
  memset((void *)&z_pdcomm_tcp_info.tx_buffer, 0, sizeof(tm_supl_comm_output_buffer));


  z_pdcomm_get_handle_params.q_base_msg_id = TM_CORE_PDCOMM_MSG_ID_BASE;
  z_pdcomm_get_handle_params.e_link_type = LINK_TCP;
  if ( pd_comms_app_get_handle (&z_pdcomm_get_handle_params, &z_pdcomm_handle_tcp ) != PDCOMM_RESULT_OK )
  {
    MSG_MED("pdcomm tcp handle not acquired at TM start", 0, 0, 0); /* depend on when pdcomm_tcp starts */
  }
  else
  {
    pdcomm_tcp_handle_get_done = TRUE;

    MSG_MED("pdcomm tcp handle acquired at TM start", 0, 0, 0); /* depend on when pdcomm_tcp starts */

    /* update the supl comm module with the root cert dir */       
    (void)tm_umts_up_supl_comm_pass_root_cert_dir();
  } /* TCP handle obtained ok */

  /********* init SMS ********************************************/

  z_pdcomm_sms_info.curr_state = PDCOMM_STATE_DISCONNECTED;
  z_pdcomm_sms_info.recv_state = TM_SUPL_COMM_RECV_HEADER;
  z_pdcomm_sms_info.num_bytes_to_read = 0;
  memset((void *)&z_pdcomm_sms_info.tx_buffer, 0, sizeof(tm_supl_comm_output_buffer));

  z_comms_connect_params_sms.e_link_type = LINK_SMS;
  z_comms_connect_params_sms.e_use_security = SECURITY_NONE; 
  z_comms_connect_params_sms.q_transaction_id = TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_BASE;
  z_comms_connect_params_sms.z_link_params.z_sms_params.p_parse_cb_func = supl_comm_sms_verify_read_cb;
  z_comms_connect_params_sms.z_link_params.z_sms_params.p_read_msg_cb_func = NULL;

  q_current_hash_transac_id = TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE;

  /*create multipart wap push wait timer*/
  supl_multipart_wap_push_wait_timer = os_TimerCreateInCgpsTimerGroup( SUPL_MULTIPART_WAP_PUSH_WAIT_TIMER_ID, THREAD_ID_SM_TM );

  /************************************ Connect WAP PUSH  ****************************/

  z_pdcomm_get_handle_params.q_base_msg_id = TM_CORE_PDCOMM_MSG_ID_BASE;
  z_pdcomm_get_handle_params.e_link_type = LINK_SMS;
  if ( pd_comms_app_get_handle (&z_pdcomm_get_handle_params, &z_pdcomm_handle_sms ) != PDCOMM_RESULT_OK )
  {
    /* shouldn't happen, fatal */
    MSG_ERROR("fails to get pdcomm handle for SMS at TM start", 0, 0, 0);
    error_flag = TRUE;
  }

  else 
  {
    pdcomm_connect_ret_value = pd_comms_app_connect( z_pdcomm_handle_sms, 
                                                     THREAD_ID_SM_TM, 
                                                     TM_CORE_PDCOMM_MSG_ID_BASE, 
                                                     &z_comms_connect_params_sms );

    if ( ( pdcomm_connect_ret_value != PDCOMM_RESULT_OK ) && ( pdcomm_connect_ret_value != PDCOMM_RESULT_WAIT ) )
    {
      /* shouldn't happen, fatal */ 
      MSG_ERROR("fails to connect pdcomm for sms", 0, 0, 0);
      error_flag = TRUE;
    }

  }

  /********* init UDP ********************************************/

  z_pdcomm_udp_info.curr_state = PDCOMM_STATE_DISCONNECTED;
  z_pdcomm_udp_info.recv_state = TM_SUPL_COMM_RECV_HEADER;
  z_pdcomm_udp_info.num_bytes_to_read = 0;
  memset((void *)&z_pdcomm_udp_info.tx_buffer, 0, sizeof(tm_supl_comm_output_buffer));

  q_current_hash_transac_id = TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE;

  
  if(!supl_comm_get_slp_address_portnum(&z_comms_connect_params_udp.z_link_params.z_tcp_params))
  {
    error_flag = TRUE;
  }
  /************************************ Open UDP Socket****************************/

  z_pdcomm_get_handle_params.q_base_msg_id = TM_CORE_PDCOMM_MSG_ID_BASE;
  z_pdcomm_get_handle_params.e_link_type = LINK_UDP;
  if ( pd_comms_app_get_handle (&z_pdcomm_get_handle_params, &z_pdcomm_handle_udp ) != PDCOMM_RESULT_OK )
  {
    /* shouldn't happen, fatal */
    MSG_ERROR("fails to get pdcomm handle for UDP at TM start", 0, 0, 0);
    error_flag = TRUE;
  }

  else 
  {        
    /*create udp retry timer*/
     supl_udp_bind_retry_timer = os_TimerCreate( SUPL_UDP_BIND_RETRY_TIMER_ID, THREAD_ID_SM_TM );
    (void)tm_umts_up_supl_comm_udp_bind(FALSE);
  }

  if (error_flag == TRUE)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}


/*===========================================================================

FUNCTION tm_umts_up_supl_comm_write

DESCRIPTION
  This function is used to write data to socket.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
pd_comms_return_type tm_umts_up_supl_comm_write(uint8 *p_data, uint16 w_size)
{
  pd_comms_return_type ret_val;
  pd_comms_write_params_type write_params;

  write_params.q_transaction_id = supl_comm_getnext_transaction_id(TRANSAC_TCP);
  write_params.w_data_len = w_size;
  write_params.p_data = (uint8*)p_data;

  ret_val = pd_comms_app_write(z_pdcomm_handle_tcp, THREAD_ID_SM_TM, TM_CORE_PDCOMM_MSG_ID_BASE, &write_params);

  if( ret_val != PDCOMM_RESULT_WAIT && ret_val != PDCOMM_RESULT_OK )
  {
    MSG_ERROR("PDCOMM TD: pdcomm_client_write returned error: %d",ret_val,0,0);
  }

  /* log the message: note that the 2nd argument is not used other than for Snapper to
     display a corresponding string to indicate the "connection type", a concept that no longer
     is relevant. Snapper uses 5 for the string "SUPL" */
  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_TX_TYPE, 5, C_LSM_SVR_PROTOCOL_TYPE_SUPL, 
                       w_size, p_data );


  return ret_val;

}


/*===========================================================================

FUNCTION tm_umts_up_supl_comm_close_connection

DESCRIPTION
  This function is used to disconnect with PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_umts_up_supl_comm_close_connection( void )
{
  pd_comms_return_type ret_val = PDCOMM_RESULT_OK;

   /* Resetting the flags so that we can attempt a connection in
       the next session */
    z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
    z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;    
    z_pdcomm_tcp_info.emergency_connection = FALSE;
  /* If we are connected Request PDCOMM to terminate connection.
  */
  switch( z_pdcomm_tcp_info.curr_state )
  {
  case PDCOMM_STATE_DISCONNECTING: 
  case PDCOMM_STATE_DISCONNECTED:
    return; 
    //break;

  case PDCOMM_STATE_CONNECTED:
  case PDCOMM_STATE_CONNECTING:
    z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_DISCONNECTED;
    ret_val = supl_comm_disconnect( FALSE );
    break;
    
  default:
    ret_val = PDCOMM_RESULT_OK;      
    break;
    
  }

  if ( ret_val == PDCOMM_RESULT_OK )
  {
    z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_DISCONNECTED;
  }
  else if (ret_val == PDCOMM_RESULT_WAIT) 
  {
    z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_DISCONNECTING;
  }
  else
  {
    /*  Disconnect Failed. Print an error and
    ** indicate pde connection is closed anyway.
    */
    MSG_LOW( "Failed to Disconnect with SLP ",0,0,0 );
    z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_DISCONNECTED;
  }
} /* End tm_1x_up_comm_close_connection() */



/*===========================================================================

FUNCTION tm_umts_up_supl_comm_handle_read_data

DESCRIPTION
  This function is used to handle READ DATA from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_umts_up_supl_comm_handle_read_data(void *msg_data_ptr)
{
    boolean ret_val= TRUE;
    pd_comms_ipc_read_data_hdr_type* read_hdr_ptr;
    static uint8 *p_pdcomm_rx_buff_local = NULL; 
    tm_supl_pdcomm_info_s_type *p_pdcomm_info;
    
    if (NULL == msg_data_ptr)
    {
       MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"NULL input parameter");
       return FALSE;
    }     

    read_hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg_data_ptr;

    if ( read_hdr_ptr->z_handle.e_link_type == LINK_TCP )
    {
      p_pdcomm_info = &z_pdcomm_tcp_info;
    }
    else if ( read_hdr_ptr->z_handle.e_link_type == LINK_SMS )
    {
      p_pdcomm_info = &z_pdcomm_sms_info;
    }    
    else if ( read_hdr_ptr->z_handle.e_link_type == LINK_UDP )
    {
      p_pdcomm_info = &z_pdcomm_udp_info;
    }
    else
    {
      MSG_HIGH("Wrong link type: %d", read_hdr_ptr->z_handle.e_link_type, 0, 0 );
      return FALSE;
    }

    /* A read event has occured, invoked PDCOMM function to read out the data */
    if ( p_pdcomm_info->curr_state == PDCOMM_STATE_CONNECTED )
    {
      if( read_hdr_ptr->w_data_len != 0 )
      {
        /* Now read the actual data */
        if( p_pdcomm_info->recv_state == TM_SUPL_COMM_RECV_HEADER )
        {

          if ( read_hdr_ptr->w_data_len == 2 )
          {
            uint8 *p_data_to_read;
            uint8 u_num_bytes_to_read_msb, u_num_bytes_to_read_lsb;
            
            p_data_to_read = (uint8 *)&read_hdr_ptr->p_data[0];
            p_pdcomm_info->num_bytes_to_read = *p_data_to_read++;

            /* the ASN1 decoder needs to start from the beginning of PDU */
            u_num_bytes_to_read_msb = (uint8)p_pdcomm_info->num_bytes_to_read;
            
            p_pdcomm_info->num_bytes_to_read <<= 8;

            /* completes uint16 */
            p_pdcomm_info->num_bytes_to_read |= *p_data_to_read; /*lint !e415 */

            /* LSB of the length field in SUPL message buffer */
            u_num_bytes_to_read_lsb = *p_data_to_read;; /*lint !e415 */
            
            if ( p_pdcomm_rx_buff != NULL )
            {
              tm_umts_up_supl_comm_free_pdcomm_rx_buff();
            }  
            
            w_pdcomm_rx_buff_size = p_pdcomm_info->num_bytes_to_read + C_BYTES_FOR_HASH;
            p_pdcomm_rx_buff = (uint8*)os_MemAlloc(w_pdcomm_rx_buff_size, OS_MEM_SCOPE_TASK);
            
            pdcomm_rx_buff_len =   p_pdcomm_info->num_bytes_to_read;
            u_buffer_counter++;
            p_pdcomm_rx_buff_local = p_pdcomm_rx_buff;
            if ( p_pdcomm_rx_buff == NULL )
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
                    "os_MemAlloc fails: %lx", 
                    p_pdcomm_rx_buff);
              w_pdcomm_rx_buff_size = 0;    
              return FALSE; /* fails to allocate memory, exit */      
            }
            else
            {
              MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
                    "os_MemAlloc at %lx for %u bytes", 
                    p_pdcomm_rx_buff,
                    w_pdcomm_rx_buff_size);
            
            }


            *p_pdcomm_rx_buff_local++ = u_num_bytes_to_read_msb; /* MSB of the length field in SUPL message buffer */
            *p_pdcomm_rx_buff_local++ = u_num_bytes_to_read_lsb; /* LSB of the length field in SUPL message buffer */ 
            
            
            p_pdcomm_info->num_bytes_to_read -= 2; /* minus the first two bytes of length */ 

            MSG_LOW("Read data of length %d",p_pdcomm_info->num_bytes_to_read, 0, 0);

            p_pdcomm_info->recv_state = TM_SUPL_COMM_RECV_PAYLOAD;
          }
          else
          {
            MSG_ERROR("PDCOMM data header size wrong: %u", read_hdr_ptr->w_data_len, 0, 0);
            ret_val = FALSE;
          }

        }
        else /*if( p_pdcomm_info->recv_state == TM_SUPL_COMM_RECV_PAYLOAD )*/
        {
        
          //MSG_LOW("%d bytes of SUPL message recv.", read_hdr_ptr->w_data_len, 0, 0);
          
          MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
                "%d bytes of SUPL message recv., pdcomm_rx_buff_cnt: %u",
                read_hdr_ptr->w_data_len, 
                u_buffer_counter);
          

          if  ( ( read_hdr_ptr->z_handle.e_link_type == LINK_UDP ) &&
                ( p_pdcomm_rx_buff != NULL )
              )  
          {
            tm_umts_up_supl_comm_free_pdcomm_rx_buff();
          }
          
          if ( p_pdcomm_rx_buff == NULL )
          {
            w_pdcomm_rx_buff_size = read_hdr_ptr->w_data_len + C_BYTES_FOR_HASH;
            p_pdcomm_rx_buff = (uint8*)os_MemAlloc((read_hdr_ptr->w_data_len + C_BYTES_FOR_HASH), OS_MEM_SCOPE_TASK);
            pdcomm_rx_buff_len =   read_hdr_ptr->w_data_len;
            
            u_buffer_counter++; 
            p_pdcomm_rx_buff_local = p_pdcomm_rx_buff;
            
            if ( p_pdcomm_rx_buff == NULL )
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
                    "os_MemAlloc fails: %lx", 
                    p_pdcomm_rx_buff);
              w_pdcomm_rx_buff_size = 0;    
              return FALSE; /* fails to allocate memory, exit */      
            }
            
            MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
                  "os_MemAlloc at %lx for %u bytes", 
                  p_pdcomm_rx_buff,
                  w_pdcomm_rx_buff_size);
            
          } 
          if ( p_pdcomm_rx_buff_local == NULL )
          {
          
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
                  "pdcomm_rx_buff_local invalid %lx", 
                  p_pdcomm_rx_buff_local);
                  
            return FALSE;
          }
          
          if ( read_hdr_ptr->w_data_len > ( w_pdcomm_rx_buff_size - 2 ) )
          {
            MSG_HIGH("data_len %u: greater than buffer", read_hdr_ptr->w_data_len, 0, 0);            
            tm_umts_up_supl_comm_free_pdcomm_rx_buff();
            return FALSE;
          }

          /* for UPD, this starts at the base of p_pdcomm_rx_buff; otherwise, the pointer
             has already advanced twiced */
          memscpy ( p_pdcomm_rx_buff_local, read_hdr_ptr->w_data_len, read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);
          p_pdcomm_rx_buff_local += read_hdr_ptr->w_data_len;

          if(p_pdcomm_info->num_bytes_to_read > read_hdr_ptr->w_data_len) /*Check so that num_bytes_to_read does not overflow*/
          {
            p_pdcomm_info->num_bytes_to_read -= read_hdr_ptr->w_data_len;
          }
          else 
          {
            p_pdcomm_info->num_bytes_to_read = 0;
          }

          if(p_pdcomm_info->num_bytes_to_read == 0)
          {
            /*Read complete payload*/
            p_pdcomm_info->recv_state = TM_SUPL_COMM_RECV_HEADER;
            
            if ( read_hdr_ptr->z_handle.e_link_type == LINK_SMS ||
                 read_hdr_ptr->z_handle.e_link_type == LINK_UDP )
            {

              if ( read_hdr_ptr->z_handle.e_link_type == LINK_UDP)
              {  
                // SMS source should have been set already, UDP package complete now
                z_supl_init_source = PDSM_SUPL_INIT_SOURCE_TYPE_UDP; 
              }

              /* passed service interaction; get a HASH now */
              if ( ! supl_comm_generate_hash ( p_pdcomm_rx_buff, pdcomm_rx_buff_len) )
              {
                 /*if previous NI mesg is pending Hash resp, drop the hash request 
                   since supl_comm_generate_hash will return FALSE but b_pending_hash flag is true*/
                 if (FALSE == b_pending_hash) 
                 {
                   MSG_ERROR("SUPL INIT Hash Generation Failed", 0, 0, 0 );
                   /* still need to decode SUPL_INIT for returning a message back to the SLP */
                   (void) tm_umts_up_supl_message_decode ( p_pdcomm_rx_buff, pdcomm_rx_buff_len); 
                 }
                 /*Free the RX buffer 
                   1)if NI dropped when waiting for HASH
                   2)if Hash failed */
                 tm_umts_up_supl_comm_free_pdcomm_rx_buff();
              }
            }
            else /* TCP messages: everything else but SUPL_INIT */
            {
              (void) tm_umts_up_supl_message_decode ( p_pdcomm_rx_buff, pdcomm_rx_buff_len ); /* plus 2: the two bytes for length */
              tm_umts_up_supl_comm_free_pdcomm_rx_buff();
            }  
          }
        }
      }

      else
      {
        /* Received data of zero length from TM-Core */
        MSG_HIGH("Received data of zero length from TM-Core",0,0,0);
        ret_val = FALSE;
      }

    }
    else
    {
      /* Print Error in this case, Data when 1X UP is not
      ** connected.
      */
      MSG_HIGH("Received data from TM-Core in error state %d",p_pdcomm_info->curr_state,0,0);
      ret_val = FALSE;
    }

    return ret_val;
}


/*===========================================================================

FUNCTION tm_umts_up_supl_comm_handle_notification

DESCRIPTION
  This function handles PDCOMM notifications. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_comm_handle_notification ( void *msg_data_ptr )
{

  pd_comms_ipc_notification_hdr_type* notify_hdr_ptr;
  pd_comms_notification_params_type* params;
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  notify_hdr_ptr = (pd_comms_ipc_notification_hdr_type*)msg_data_ptr;

  if(notify_hdr_ptr->e_type == STATE_CHANGE) /* notified of state change */
  {

    params = &notify_hdr_ptr->z_params; 

    if ( notify_hdr_ptr->z_handle.e_link_type == LINK_TCP ) 
    {
      MSG_MED("SUPL: got TCP state change: %d", params->e_new_state, 0, 0);
    }

    if ( ( notify_hdr_ptr->z_handle.e_link_type == LINK_TCP )  &&
         ( notify_hdr_ptr->z_handle.q_handle_id == z_pdcomm_handle_tcp.q_handle_id )
       )
    {

      if(params->e_new_state == PDCOMM_STATE_CONNECTED)
      {
        MSG_MED("TCP connection with SLP up",0,0,0);
        z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_CONNECTED;

        /* Resetting the flags when we have establisehd a successfull 
        connection so that we can attempt another one in the next session */
        z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
        z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;        
        z_pdcomm_tcp_info.num_bytes_to_read = 0;        
        z_pdcomm_tcp_info.recv_state = TM_SUPL_COMM_RECV_HEADER;

        tm_umts_up_supl_tcp_connect_notif(); /* this will end UT0 and send SUPL_START if pending */
      }

      else if ( ( params->e_new_state == PDCOMM_STATE_DISCONNECTED ) ||
                ( params->e_new_state == PDCOMM_STATE_DISCONNECTING )
              )
      {
        if(notify_hdr_ptr->q_transaction_id != z_pdcomm_tcp_info.disconnect_transaction_id)
        {
          MSG_MED("Previous session Disconnect received",0,0,0);
          return;
        }        
        
        MSG_MED("TCP disconnecting/disconnected",0,0,0);
        

        if (z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTING ||
            z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_OPENING)
        {
          tm_umts_up_supl_send_comm_failure();
        }
        z_pdcomm_tcp_info.curr_state = params->e_new_state;
        if( (z_pdcomm_tcp_info.used_auto_configured_hslp_address == TRUE) && 
            (z_pdcomm_tcp_info.tls_session_authentication_failure == TRUE) )
        {
          /* Resetting the flags so that we can attempt a connection in
             the next session */
          z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          z_pdcomm_tcp_info.emergency_connection = FALSE;
          /* End the session since we do not want to get into an infinite loop*/

          if( (params->e_new_state == PDCOMM_STATE_DISCONNECTING ) &&
              (p_supl_session_status->u_tcp_connection_attempt_pending)
            )
          {
            MSG_HIGH("Waiting for complete disconnect before proceding",0,0,0);
            return;
          }

          if(p_supl_session_status->u_tcp_connection_attempt_pending)
          {          
            MSG_HIGH("START Next TCP Connection",0,0,0);
            /*Bring up the pending TCP Connection*/            
            p_supl_session_status->u_tcp_connection_attempt_pending = FALSE;
            
              /* Initializing the flags so that we can attempt a fresh 
                 connection*/
            z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
            z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
            
            /*Start SLP contact*/
            tm_umts_up_supl_start_slp_contact();
          }
          else
          {
            tm_umts_up_supl_callflow_disrupt_handler();
          }
          return;
        }

        /* Check if an authentication failure has occurred and if so reattempt the connection to the H-SLP 
        using an auto-configured H-SLP address*/
        if( z_pdcomm_tcp_info.tls_session_authentication_failure )
        {
          if(params->e_new_state == PDCOMM_STATE_DISCONNECTING )
          {            
            MSG_HIGH("Waiting for complete disconnect before proceding",0,0,0);
            return;
          }
        
          switch(p_supl_session_status->u_callflow_kind)
          {  
             case C_SUPL_SESSION_NI:
               if ( supl_comm_generate_hash ( p_pdcomm_rx_buff, pdcomm_rx_buff_len ) )
               {
                  MSG_MED("Re-attempting socket connection with auto-config H-SLP addr",0,0,0);
                  return;
               }
               else
               { 
                  MSG_ERROR("SUPL INIT Hash Generation Failed during socket re-attempt", 0, 0, 0 );
                  return;
               }
               break;
             case C_SUPL_SESSION_UI: 
             default:
               tm_umts_up_supl_start_slp_contact();
               MSG_MED("Re-attempting socket connection with auto-config H-SLP addr",0,0,0);
               return;             
          }
        }

        
        if(p_supl_session_status->u_tcp_connection_attempt_pending)
        {        
          if(params->e_new_state == PDCOMM_STATE_DISCONNECTING )
          {            
            MSG_HIGH("Waiting for complete disconnect before proceding",0,0,0);
            return;
          }
          else
          {            
            MSG_HIGH("START Next TCP Connection",0,0,0);
            /*Bring up the pending TCP Connection*/            
            p_supl_session_status->u_tcp_connection_attempt_pending = FALSE;
            
              /* Initializing the flags so that we can attempt a fresh 
                 connection*/
            z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
            z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
            
            /*Start SLP contact*/
            tm_umts_up_supl_start_slp_contact();
          }          
        }
        else
        {          
          if ( p_supl_session_status->e_cf_state != C_SUPL_CF_STATE_NULL )
          {            
            tm_umts_up_supl_callflow_disrupt_handler();
          }
        }                
      }
    }

    else if ( ( notify_hdr_ptr->z_handle.e_link_type == LINK_SMS  )  &&
              ( notify_hdr_ptr->z_handle.q_handle_id ==  z_pdcomm_handle_sms.q_handle_id )
            )
    {

      if(params->e_new_state == PDCOMM_STATE_CONNECTED)
      {
        MSG_MED("SMS connection with SLP up",0,0,0); /* this should be a one-time occurrance since reset */
        z_pdcomm_sms_info.curr_state = PDCOMM_STATE_CONNECTED;
        z_pdcomm_sms_info.recv_state = TM_SUPL_COMM_RECV_HEADER;
        z_pdcomm_sms_info.num_bytes_to_read = 0;
      }

      else
      {
        MSG_HIGH("recv SMS connection state other than CONNECTED, %d", params->e_new_state, 0, 0); 
        z_pdcomm_sms_info.curr_state = params->e_new_state;
      }

      /* ignore all other states */
    }
    
    else if ( ( notify_hdr_ptr->z_handle.e_link_type == LINK_UDP  )  &&
              ( notify_hdr_ptr->z_handle.q_handle_id ==  z_pdcomm_handle_udp.q_handle_id )
            )
    {

      if(params->e_new_state == PDCOMM_STATE_CONNECTED)
      {
        MSG_MED("UDP socket created",0,0,0); /* this should be a one-time occurrance since reset */
        z_pdcomm_udp_info.curr_state = PDCOMM_STATE_CONNECTED;        
        z_pdcomm_udp_info.recv_state = TM_SUPL_COMM_RECV_HEADER;
        z_pdcomm_udp_info.num_bytes_to_read = 0;        
      }

      else
      {
        MSG_HIGH("recv UDP connection state other than CONNECTED, %d", params->e_new_state, 0, 0); 
        z_pdcomm_udp_info.curr_state = params->e_new_state;
      }

      /* ignore all other states */
    }

  } /* close if-e_type-is-StateChange */

  else if ( notify_hdr_ptr->e_type == READ_DATA )  /* notified of data to read */
  {

    if ( notify_hdr_ptr->z_handle.e_link_type == LINK_TCP )
    {

      if ( z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED )
      {
        if( z_pdcomm_tcp_info.recv_state == TM_SUPL_COMM_RECV_HEADER)
        {
          (void) supl_comm_read( SUPL_LENGTH_HDR_SIZE, LINK_TCP, TRUE ); /* tell PDCOMM to read SUPL_LENGTH_HDR_SIZE bytes */
        }
        else
        {
          (void) supl_comm_read( z_pdcomm_tcp_info.num_bytes_to_read, LINK_TCP, TRUE );
        }
      }
      else
      {
        MSG_HIGH("Read data in error state %d", z_pdcomm_tcp_info.curr_state,0,0);
      }

    } /* close if-TCP */

    else if ( notify_hdr_ptr->z_handle.e_link_type == LINK_SMS )
    {

      MSG_LOW("SMS Read notification from TM-Core",0,0,0);

      if ( z_pdcomm_sms_info.curr_state == PDCOMM_STATE_CONNECTED )
      {
        if( z_pdcomm_sms_info.recv_state == TM_SUPL_COMM_RECV_HEADER)
        {
          MSG_LOW("Read SUPL header from SMS",0,0,0);
          (void) supl_comm_read( SUPL_LENGTH_HDR_SIZE, LINK_SMS, TRUE ); /* tell PDCOMM to read SUPL_LENGTH_HDR_SIZE bytes */
        }
        else
        {
          MSG_LOW("Read SUPL payload of length %d from SMS", z_pdcomm_sms_info.num_bytes_to_read, 0, 0);
          (void) supl_comm_read( z_pdcomm_sms_info.num_bytes_to_read, LINK_SMS, TRUE );
        }
      }
      else
      {
        MSG_HIGH("Read data in error state %d", z_pdcomm_sms_info.curr_state,0,0);
      }

    } /* close if-SMS */
    
    else if ( notify_hdr_ptr->z_handle.e_link_type == LINK_UDP )
    {

      if ( z_pdcomm_udp_info.curr_state == PDCOMM_STATE_CONNECTED )
      {
          /*We can not read partial data from the UDP socket*/
          (void) supl_comm_read( C_MAX_PDCOMM_RX_BUFF_LENGTH, LINK_UDP, FALSE );
          z_pdcomm_udp_info.recv_state = TM_SUPL_COMM_RECV_PAYLOAD;
      }
      else
      {
        MSG_HIGH("Read data in error state %d", z_pdcomm_udp_info.curr_state,0,0);
      }

    } /* close if-TCP */

    else
    {
      /* should not happen: wrong link type igored */
      MSG_ERROR("Read notif. of a wrong link type: %d", notify_hdr_ptr->z_handle.e_link_type, 0, 0 );
    }

  } /* close else-if-READ_DATA */

  else if ( notify_hdr_ptr->e_type == ERROR_RPT )  /* notified of a communication error */
  {
    params = &notify_hdr_ptr->z_params; 

    if ( notify_hdr_ptr->z_handle.e_link_type == LINK_TCP ) 
    {
      MSG_HIGH("SUPL: got TCP Error Report: %d", params->q_result, 0, 0);

      if( params->q_result == PDCOMM_CONNECT_ERROR_SECURITY_FAILURE )
      {
        /*
         * Check if an attempt has been made to establish a TLS connection using
         * the auto-configured H-SLP address and failed
        */
        if( z_pdcomm_tcp_info.used_auto_configured_hslp_address )
        {
          /* Resetting the flags so that we can attempt a connection in
             the next session */
          z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
          z_pdcomm_tcp_info.emergency_connection = FALSE;
          if(!p_supl_session_status->u_tcp_connection_attempt_pending)
          {        
            /* End the session since we do not want to get into an infinite loop*/
            tm_umts_up_supl_callflow_disrupt_handler();
          }

          return;
        }

        /* An authentication failure has occurred */
        z_pdcomm_tcp_info.tls_session_authentication_failure = TRUE;
      }
      else 
      if (z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTING ||
            z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_OPENING)
      {
        tm_umts_up_supl_send_comm_failure();
        
        if ( p_supl_session_status->e_cf_state != C_SUPL_CF_STATE_NULL )
        {
          MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Connection error when curr_state=%u, supl_state=%u", 
                 z_pdcomm_tcp_info.curr_state, p_supl_session_status->e_cf_state);
      
          if(!p_supl_session_status->u_tcp_connection_attempt_pending)
          {
            tm_umts_up_supl_callflow_disrupt_handler();
          }
        }
      }

      /* regardless of any state, when tcp connection error happens unless security
         failure which is processed above already, make sure the following is cleared: */
      if( params->q_result != PDCOMM_CONNECT_ERROR_SECURITY_FAILURE )
      {
        z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;        
        z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
        z_pdcomm_tcp_info.emergency_connection = FALSE;
      }

    }
    else if (notify_hdr_ptr->z_handle.e_link_type == LINK_UDP ) 
    {          
       if ((z_pdcomm_udp_info.curr_state == PDCOMM_STATE_CONNECTED)&&
           (notify_hdr_ptr->z_params.e_new_state == PDCOMM_STATE_CONNECTED)&&
           (notify_hdr_ptr->z_params.q_result == PDCOMM_READ_ERROR_INTERNAL)) 
       {
          /*Server Close event/READ_ERROR occured. PD_COMM UDP socket still Open*/
          MSG_HIGH("UDP read error, Socket still OPEN",0,0,0);
          z_pdcomm_udp_info.recv_state = TM_SUPL_COMM_RECV_HEADER;
          z_pdcomm_udp_info.num_bytes_to_read = 0;
       }
       else
       {
         MSG_HIGH("UDP bind failed",0,0,0);
         if(supl_pdcomm_udp_bind_retry_count < SUPL_UDP_BIND_MAX_RETRY_COUNT)
         {
           /*Start the UDP retry timer for retrying*/
           MSG_HIGH("Starting the udp bind retry timer",0,0,0);
           supl_pdcomm_udp_bind_retry_count++;
           (void) os_TimerStart(supl_udp_bind_retry_timer, SUPL_UDP_BIND_RETRY_VALUE_MSEC, NULL );
         }
         else
         {
           MSG_HIGH("Max number of UDP bind retries for this RAT over",0,0,0);
         }
       }
    }
  } /* close else-if-ERROR_RPT */
}


/*===========================================================================

FUNCTION tm_umts_up_supl_comm_handle_hash_resp

DESCRIPTION
  This function handles hash response from PDCOMM 
  
DEPENDENCIES: none.

Return value: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_comm_handle_hash_resp (void *msg_data_ptr)
{

    pd_comms_ipc_hash_resp_hdr_type* hash_resp_ptr;

    if (NULL == msg_data_ptr) 
    {
       MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"NULL input parameter");
       return ;
    }

    hash_resp_ptr = (pd_comms_ipc_hash_resp_hdr_type*)msg_data_ptr;

    /*Uncheck the Flag. Incoming NI can now be handled*/
    if (TRUE == b_pending_hash) 
    {
       b_pending_hash = FALSE;
    }

    /* SUPL needs to use HMAC-SHA1-64 */
    if ( hash_resp_ptr->q_output_len < 8 )
    {
      MSG_ERROR("SUPL - HASH too short: %u", hash_resp_ptr->q_output_len, 0, 0);   
      return;
    }

    /* arguments: address of the hash response message, length of the first part 
       of the message, which is the original SUPL_INIT */
    tm_umts_up_supl_hash_proc( (uint8 *)&hash_resp_ptr->p_data[0], (uint16)hash_resp_ptr->q_msg_len );

} 

/********************************************************************
*
* tm_umts_up_supl_comm_root_certs_updated
*
* Function description:
*  This function lets PDCOMM know that root certs dir has been updated.
* 
* Parameters: 

* Return:
*  
*********************************************************************
*/
void tm_umts_up_supl_comm_root_certs_updated(void)
{
  if(secsslca_parse_root_folder( TM_UMTS_SUPL_ROOT_CERT_DIR ) != E_SUCCESS)
  {
    MSG_ERROR("Could not update SSL with cert dir", 0, 0, 0);
  }
}

/********************************************************************
*
* tm_umts_up_supl_comm_pass_root_cert_dir
*
* Function description:
*  This function passes a root certificate directory to PDCOMM. This
*  is the directory that will contain all the SUPL SSL certificates. 
* 
* Parameters: 
* efs_data - Pointer to the EFS data sent from PDAPI

* Return:
*  TRUE/FALSE
*  
*********************************************************************
*/
boolean tm_umts_up_supl_comm_pass_root_cert_dir(void)
{
  pd_comms_ioctl_params_type    ioctl_params;
  boolean                       ret_val = FALSE;
  pd_comms_return_type          pdcomms_app_ret_val = PDCOMM_RESULT_ERROR_UNKNOWN;
  
  ioctl_params.q_ioctl_request = PDCOMM_IOCTL_CMD_SET_SSL_CERT_DIR;
  ioctl_params.q_transaction_id = supl_comm_getnext_transaction_id(TRANSAC_TCP);
  ioctl_params.p_argument = (uint8*)TM_UMTS_SUPL_ROOT_CERT_DIR;
  ioctl_params.q_argument_len = sizeof(TM_UMTS_SUPL_ROOT_CERT_DIR);

  pdcomms_app_ret_val = pd_comms_app_ioctl( z_pdcomm_handle_tcp,
                                            (uint32)THREAD_ID_SM_TM, 
                                            TM_CORE_PDCOMM_MSG_ID_BASE, 
                                            &ioctl_params
                                          );

  if ( ( pdcomms_app_ret_val == PDCOMM_RESULT_OK ) ||
       ( pdcomms_app_ret_val == PDCOMM_RESULT_WAIT )
     )
  {
    MSG_MED("SUPL: root cert. passed to pdcomm ok", 0, 0, 0);
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION supl_comm_get_slp_address_portnum

DESCRIPTION
  Obtains slp's address and port number.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
static boolean supl_comm_get_slp_address_portnum( pd_comms_tcp_connect_params_type  *p_connect_params )
{
  static char tempAscii[120];
  boolean b_ret = FALSE;
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  if (NULL == p_connect_params)
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"NULL input parameter");
     return FALSE;
  }

  /* If the H-SLP address is not available in the NV items
     or if an authentication failure has occurred  */
  
  if (p_supl_session_status->z_emergency_data.b_emergency_ni == TRUE)
  {
    /* this is an emergency NI SUPL session. use the E-SLP if received in SUPL INIT*/
    if (p_supl_session_status->z_emergency_data.b_eslp_present == TRUE)
    {
      if (z_pdcomm_tcp_info.tls_session_authentication_failure == FALSE) 
    {
      (void)GNSS_STRLCPY ( p_connect_params->p_host_name,
            p_supl_session_status->z_emergency_data.z_eslp_address.supl_slp_address.server_adrs_url.url, 
              p_supl_session_status->z_emergency_data.z_eslp_address.supl_slp_address.server_adrs_url.url_length + 1 );
      
        MSG_MED("SUPL eslp address length %d ", p_supl_session_status->z_emergency_data.z_eslp_address.supl_slp_address.server_adrs_url.url_length,0,0 );
        p_connect_params->w_ip_port = 7275; // use the well known port if ESLP is given
        p_connect_params->e_type = PD_COMMS_HOST_NAME;
    }
      else if (z_cell_info_cached.u_CachedCellIDValid == TRUE)
      {
        b_ret = supl_comm_eslp_address_build_from_mcc_mnc(p_connect_params->p_host_name, &z_cell_info_cached);
        p_connect_params->w_ip_port = z_umts_nv_item_ram_copy.slp_port_number;
        if (b_ret == TRUE)
        {
          p_connect_params->e_type = PD_COMMS_HOST_NAME;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = TRUE;
        }
        else
        {
          MSG_MED("SUPL: E-SLP address configuration from IMSI failed", 0, 0, 0);
          return FALSE;
        }
      }
    }
    else if (z_umts_nv_item_ram_copy.slp_address[0] != '\0')
    {
      /* if no ESLP provided used the NV configured SLP address */
      (void)GNSS_STRLCPY ( p_connect_params->p_host_name,
                    z_umts_nv_item_ram_copy.slp_address, 
                    C_MAX_URL_SIZE_BYTE );

      p_connect_params->w_ip_port = z_umts_nv_item_ram_copy.slp_port_number;
      /*If MT-ES does not provision E-SLP 
        1)Attempt SLP configured in NV
          (SUPL Spec does not specify any particular behavior, except for auto-config-eslp,
           which is not used in customer case)
        2)Set hslp_address used flag. So that if NV SLP also fails we terminate the session
      */
      z_pdcomm_tcp_info.used_auto_configured_hslp_address = TRUE;
      p_connect_params->e_type = PD_COMMS_HOST_NAME;
    }
    else
    {
      /* construct the slp address from MCC/MNC of the current servign network */
      if (z_cell_info_cached.u_CachedCellIDValid == TRUE)
      {
        b_ret = supl_comm_eslp_address_build_from_mcc_mnc(p_connect_params->p_host_name, &z_cell_info_cached);
        p_connect_params->w_ip_port = z_umts_nv_item_ram_copy.slp_port_number;
        if (b_ret == TRUE)
        {
          p_connect_params->e_type = PD_COMMS_HOST_NAME;
          z_pdcomm_tcp_info.used_auto_configured_hslp_address = TRUE;
        }
        else
        {
          MSG_MED("SUPL: E-SLP address configuration from IMSI failed", 0, 0, 0);
          return FALSE;
        }
      }
      else
      {
        /* to do*/
        return FALSE;
      }
    }
  }
  else
  {
  
  p_connect_params->w_ip_port = z_umts_nv_item_ram_copy.slp_port_number;

  if( (z_umts_nv_item_ram_copy.slp_address[0] == '\0')||
      (z_pdcomm_tcp_info.tls_session_authentication_failure == TRUE) )
  {
    boolean retVal = 0;

    /* We need to construct the H-SLP address from the IMSI*/
    retVal = supl_comm_hslp_address_build_from_imsi(p_connect_params->p_host_name);
    if(retVal != TRUE)
    {
      MSG_MED("SUPL: H-SLP address configuration from IMSI failed", 0, 0, 0);
      return FALSE;
    }
    else
    {
      z_pdcomm_tcp_info.used_auto_configured_hslp_address = TRUE;
    }
    if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
    {
      MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: H-SLP address being used %s", p_connect_params->p_host_name);
    }
  }
  else
  {

  (void)GNSS_STRLCPY ( p_connect_params->p_host_name,
                z_umts_nv_item_ram_copy.slp_address, 
                C_MAX_URL_SIZE_BYTE );
  }

  if (z_umts_nv_item_ram_copy.slp_ip_addr)
  {
    p_connect_params->ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
    p_connect_params->q_ip_addr.q_ip_addr_v4 = z_umts_nv_item_ram_copy.slp_ip_addr;  
  }
  else if(z_umts_nv_item_ram_copy.slp_ipv6_addr[0])
  {
    p_connect_params->ip_addr_type = PD_COMMS_IP_ADDR_IPV6;    
    memscpy(&p_connect_params->q_ip_addr.q_ip_addr_v6.ps_s6_addr16[0],
		    sizeof(p_connect_params->q_ip_addr.q_ip_addr_v6.ps_s6_addr16),
            &z_umts_nv_item_ram_copy.slp_ipv6_addr[0],
            sizeof(z_umts_nv_item_ram_copy.slp_ipv6_addr));
  }

  /*
   *  IF a non-zero IP Address is provided THEN
   *    IF the host name is NULL THEN
   *      provide ONLY an IP Address to PDCOMMS
   *    ELSE
   *      provide BOTH an IP Address and a hostname to PDCOMMS
   *    ENDIF
   *  ELSE IF a non-NULL hostname has been provided THEN
   *    provide the hostname to PDCOMMS
   *  ELSE
   *    return to the caller with an error
   *  ENDIF
   */ 
  if( (z_umts_nv_item_ram_copy.slp_ip_addr !=0)  ||
      (z_umts_nv_item_ram_copy.slp_ipv6_addr[0] != 0) )
  {
    if( p_connect_params->p_host_name[0] == '\0' )
    {
      p_connect_params->e_type = PD_COMMS_IP_ADDR;
    }
    else
    {
      p_connect_params->e_type = PD_COMMS_HOST_NAME_AND_IP_ADDR;
    }
  }
  else if( p_connect_params->p_host_name[0] != '\0' )
  {
    p_connect_params->e_type = PD_COMMS_HOST_NAME;
  }
  else
  {
    return FALSE;
  }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION supl_comm_hslp_address_build_from_imsi

DESCRIPTION
  Constructs the H-SLP address from the provided IMSI according to the algortihm outlined
  in Sec 7.2.2 in OMA-AD-SUPL-V1_0-20070615-A

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static boolean supl_comm_hslp_address_build_from_imsi (char * constructedHslpAddr)
{
#ifndef FEATURE_CGPS_FW_NON_STANDARD_SUPPORT
    /*  The objective is to construct the H-SLP address in the form "h-slp.mnc<MNC>.mcc<MCC>.pub.3gppnetwork.org"
        from the MCC and MNC provided in the IMSI. 
        For eg. If If IMSI in use is "234150999999999", where MCC=234, MNC=15, and MSIN=0999999999, 
        the H-SLP address would be "h-slp.mnc015.mcc234.pub.3gppnetwork.org".
    */
    int lengthAddrPart1, lengthAddrPart2, lengthAddrPart3 = 0;
    char defaultHslpAddrPart1[] = "h-slp.mnc";
    char defaultHslpAddrPart2[] = ".mcc";
    char defaultHslpAddrPart3[] = ".pub.3gppnetwork.org";
    char zeroPadding = '0';
    boolean bRetVal;

    char mcc[4] = "\0";
    char mnc[4] = "\0";

    /* Retrieve the IMSI value from the USIM*/
    bRetVal = tm_umts_up_supl_comm_retrieve_imsi();
    if( !bRetVal )
    {
      return FALSE;
    }
    /*We could make the assumption that mcc & mnc consist of only digits 0-9. So converting them into 
     ascii could be achieved at this point by simply adding hex value of '0'*/
    mcc[2] = (char)((z_umts_nv_item_ram_copy.set_imsi[1] & 0xF)+ 0x30); /* 0x30 is the Hex equ of '0'*/
    mcc[1] = (char)(((z_umts_nv_item_ram_copy.set_imsi[0] & 0xF0) >> 4) + 0x30);
    mcc[0] = (char)((z_umts_nv_item_ram_copy.set_imsi[0] & 0xF)+ 0x30);

    if (numMncDigits == 2)
    {
    mnc[2] = (char)((z_umts_nv_item_ram_copy.set_imsi[2] & 0xF)+ 0x30);
    mnc[1] = (char)(((z_umts_nv_item_ram_copy.set_imsi[1] & 0xF0) >> 4) + 0x30);
    mnc[0] = zeroPadding;
    }
    else
    {
      mnc[2] = (char)(((z_umts_nv_item_ram_copy.set_imsi[2] & 0xF0) >> 4) + 0x30);
      mnc[1] = (char)((z_umts_nv_item_ram_copy.set_imsi[2] & 0xF)+ 0x30);
      mnc[0] = (char)(((z_umts_nv_item_ram_copy.set_imsi[1] & 0xF0) >> 4) + 0x30);
    }

    lengthAddrPart1 = strlen(defaultHslpAddrPart1);
    memscpy(constructedHslpAddr, lengthAddrPart1, defaultHslpAddrPart1,lengthAddrPart1);
    memscpy(&constructedHslpAddr[lengthAddrPart1], 3, mnc, 3);

    lengthAddrPart2 = strlen(defaultHslpAddrPart2);
    memscpy(&constructedHslpAddr[lengthAddrPart1+3], lengthAddrPart2, defaultHslpAddrPart2, lengthAddrPart2);
    memscpy(&constructedHslpAddr[lengthAddrPart1+3+lengthAddrPart2], 3, mcc, 3);

    lengthAddrPart3 = strlen(defaultHslpAddrPart3);
    memscpy(&constructedHslpAddr[lengthAddrPart1+3+lengthAddrPart2+3], lengthAddrPart3, defaultHslpAddrPart3, lengthAddrPart3);
    return TRUE;
    
#else
    MSG_MED("Not constructing HSLP address from MCC MNC",0,0,0);
    return FALSE;
#endif
}



/*===========================================================================
FUNCTION supl_comm_eslp_address_build_from_mcc_mnc

DESCRIPTION
  Constructs the E-SLP address from the MCC-MNC according to the algortihm outlined
  in Sec 6.1.5.1 in OMA-AD-SUPL-V2

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static boolean supl_comm_eslp_address_build_from_mcc_mnc (char * constructedHslpAddr, cgps_CellInfoCachedStruct *p_cached_cell_info)
{
#ifndef FEATURE_CGPS_FW_NON_STANDARD_SUPPORT
    /*  The objective is to construct the H-SLP address in the form "h-slp.mnc<MNC>.mcc<MCC>.pub.3gppnetwork.org"
        from the MCC and MNC of the current network. 
        For eg. if MCC=234, MNC=15, 
        the E-SLP address would be "e-slp.mnc015.mcc234.pub.3gppnetwork.org" or 
        the E-SLP address would be "e-slp.mnc015.mcc234.pub.3gpp2network.org".
    */
    int lengthAddrPart1, lengthAddrPart2, lengthAddrPart3 = 0;
    char defaultHslpAddrPart1[] = "e-slp.mnc";
    char defaultHslpAddrPart2[] = ".mcc";
    char defaultHslpAddrPart3[] = ".pub.3gppnetwork.org";
    char zeroPadding = '0';

    char mcc[4] = "\0";
    char mnc[4] = "\0";


  
    MSG_MED("SUPL: generating SLP from IMSI",0,0,0);

    if (p_cached_cell_info->e_ActiveRAT == CELLDB_ACTIVERAT_GSM)
    {

     /* MCC / MNC is returned by GERAN in the following format:
      **                                        Bit
      **                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
      **                 |===============================================|
      **         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
      **                 |-----------------------------------------------|
      **         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
      **                 |-----------------------------------------------|
      **         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
      **                 |===============================================| */

      mcc[2] = (p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[1] & 0x0F) + 0x30; 
      mcc[1] = ((p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[0] >> 4) & 0x0F) + 0x30;
      mcc[0] = (p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[0] & 0x0F) + 0x30;
      
      if(((p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[1] >> 4) & 0x0F ) != 0x0F) 
      {
         mnc[2] = ((p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[1] >> 4) & 0x0F) + 0x30;
         mnc[1] = ((p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[2] >> 4) & 0x0F) + 0x30; 
         mnc[0] = (p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[2] & 0x0F) + 0x30; 
      }
      else
      {   
         mnc[2] = ((p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[2] >> 4) & 0x0F) + 0x30;
         mnc[1] = (p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[2] & 0x0F) + 0x30;
         mnc[0] = zeroPadding; 
      }
    }
 
    if (p_cached_cell_info->e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA)
    {
      mcc[2] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc[2] + 0x30;
      mcc[1] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc[1] + 0x30;
      mcc[0] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mcc[0] + 0x30;
      
      if (p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits == 2)
      {
        mnc[2] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[1] + 0x30;
        mnc[1] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[0] + 0x30;
        mnc[0] = zeroPadding;
      }
      else
      {
        mnc[2] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[2] + 0x30;
        mnc[1] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[1] + 0x30;
        mnc[0] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.mnc[0] + 0x30;
      }
    }

    if (p_cached_cell_info->e_ActiveRAT == CELLDB_ACTIVERAT_LTE)
    {

 #ifdef FEATURE_CGPS_LTE_CELLDB
      mcc[2] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc[2] + 0x30;
      mcc[1] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc[1] + 0x30;
      mcc[0] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc[0] + 0x30;
      
      if (p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits == 2)
      {
        mnc[2] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[1] + 0x30;
        mnc[1] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[0] + 0x30;
        mnc[0] = zeroPadding;
      }
      else
      {
        mnc[2] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[2] + 0x30;
        mnc[1] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[1] + 0x30;
        mnc[0] = p_cached_cell_info->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[0] + 0x30;
      }
 #endif
    }
    lengthAddrPart1 = strlen(defaultHslpAddrPart1);
    memscpy(constructedHslpAddr, lengthAddrPart1, defaultHslpAddrPart1,lengthAddrPart1);
    memscpy(&constructedHslpAddr[lengthAddrPart1], 3, mnc, 3);

    lengthAddrPart2 = strlen(defaultHslpAddrPart2);
    memscpy(&constructedHslpAddr[lengthAddrPart1+3], lengthAddrPart2, defaultHslpAddrPart2, lengthAddrPart2);
    memscpy(&constructedHslpAddr[lengthAddrPart1+3+lengthAddrPart2], 3, mcc, 3);

    
    if ((p_cached_cell_info->e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA) ||
       (p_cached_cell_info->e_ActiveRAT == CELLDB_ACTIVERAT_LTE) ||
       (p_cached_cell_info->e_ActiveRAT == CELLDB_ACTIVERAT_GSM))
    {
      lengthAddrPart3 = strlen(defaultHslpAddrPart3);
      memscpy(&constructedHslpAddr[lengthAddrPart1+3+lengthAddrPart2+3], 
              lengthAddrPart3, defaultHslpAddrPart3, lengthAddrPart3);
    }
    
    return TRUE;
#else
    MSG_MED("Not constructing ESLP address from MCC MNC",0,0,0);
    return FALSE;
#endif
}

/*===========================================================================

FUNCTION supl_comm_disconnect

DESCRIPTION
  This function is used to close phyiscal path for connecting with SLP

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

pd_comms_return_type supl_comm_disconnect( boolean b_abort )
{
  pd_comms_return_type ret_val;
  uint32 disconnect_transaction_id =0;

  disconnect_transaction_id = supl_comm_getnext_transaction_id(TRANSAC_TCP);
  z_pdcomm_tcp_info.disconnect_transaction_id = disconnect_transaction_id;

  ret_val = pd_comms_app_disconnect( z_pdcomm_handle_tcp, 
                                     THREAD_ID_SM_TM,TM_CORE_PDCOMM_MSG_ID_BASE, 
                                     disconnect_transaction_id, 
                                     b_abort);

  if(ret_val == PDCOMM_RESULT_OK)
  {
    MSG_LOW("TM_UMTS_UP SUPL disconnected", 0, 0, 0);
  }
  else if(ret_val == PDCOMM_RESULT_WAIT)
  {
    MSG_LOW("TM_UMTS_UP SUPL wait_disconnect", 0, 0, 0);
  }
  else
  {
    MSG_HIGH("pd_comms_app_disconnect returned error: %d",ret_val,0,0);
  }
  return ret_val;

} 


/*===========================================================================

FUNCTION supl_comm_read

DESCRIPTION
  This function is used to read data.

RETURN VALUE
  None

DEPENDENCIES      
There has to a successful pd_comms_app_connect() call which returned this handle before this function can be called. 
A pd_comms_app_read() call is not allowed when a previous pd_comms_app_read() has still not returned any data. 
In an attempt to do so, a PDCOMM_RESULT_ERROR_PREVIOUS_READ_PENDING will be returned.

===========================================================================*/
static pd_comms_return_type supl_comm_read(uint16 length, pd_comms_link_type link_type, boolean wait_all )
{
  
  pd_comms_return_type ret_val = PDCOMM_RESULT_OK;
  pd_comms_read_params_type read_params;

  read_params.w_data_len = length;
  read_params.u_wait_for_all = wait_all;

  if ( link_type == LINK_TCP )
  {
    read_params.q_transaction_id =  supl_comm_getnext_transaction_id(TRANSAC_TCP);

    ret_val = pd_comms_app_read(z_pdcomm_handle_tcp, THREAD_ID_SM_TM, TM_CORE_PDCOMM_MSG_ID_BASE, &read_params);

    if ( length > 2 )
    {
      MSG_MED("Ask PDCOMM to read TCP SUPL payload %u bytes", read_params.w_data_len, 0, 0);
    }

    if ( ret_val != PDCOMM_RESULT_WAIT )
    {
      MSG_ERROR("Error reading TCP SUPL payload",0,0,0);
    }

  } /* close-if-TCP */
        
  else if ( link_type == LINK_SMS )
  {
    read_params.q_transaction_id =  supl_comm_getnext_transaction_id(TRANSAC_SMS);

    ret_val = pd_comms_app_read(z_pdcomm_handle_sms, THREAD_ID_SM_TM, TM_CORE_PDCOMM_MSG_ID_BASE, &read_params);

    if ( length > 2 )
    {
      MSG_MED("Ask PDCOMM to read SMS SUPL payload %u bytes", read_params.w_data_len, 0, 0);
    }

    if ( ret_val != PDCOMM_RESULT_WAIT )
    {
      MSG_ERROR("Error reading SMS SUPL payload",0,0,0);
    }

  } /* close-if-SMS */

  else if ( link_type == LINK_UDP )
  {
    read_params.q_transaction_id =  supl_comm_getnext_transaction_id(TRANSAC_UDP);

    ret_val = pd_comms_app_read(z_pdcomm_handle_udp, THREAD_ID_SM_TM, TM_CORE_PDCOMM_MSG_ID_BASE, &read_params);

    if ( length > 2 )
    {
      MSG_HIGH("Ask PDCOMM to read UDP SUPL payload %u bytes", read_params.w_data_len, 0, 0);
    }

    if ( ret_val != PDCOMM_RESULT_WAIT )
    {
      MSG_ERROR("Error reading UDP SUPL payload",0,0,0);
    }

  } /* close-if-UDP */

  else
  {
    /* ignore wrong link type */
    MSG_HIGH("Wrong link type when comm read %d", link_type, 0, 0);
    ret_val = PDCOMM_RESULT_ERROR_UNKNOWN;
  }


  return ret_val;

} 

static boolean supl_mt_sms_gw_hdr_chk(const wms_client_ts_data_s_type *client_ts_data)
{
  boolean ret_value = FALSE;
  
  if (client_ts_data->u.gw_pp.tpdu_type == WMS_TPDU_DELIVER)
  {
    if (client_ts_data->u.gw_pp.u.deliver.user_data_header_present)
    {
      uint8 i=0;
      for(i=0; i<client_ts_data->u.gw_pp.u.deliver.user_data.num_headers; i++)
      {
        if ( (client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].header_id == WMS_UDH_PORT_16)
             && ( client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.dest_port == 
                    SUPL_PORT_NUM_OFFICIAL_V1_0
                )
           )
        {
          /* Is a SUPL MT-SMS ? */
          MSG_MED("MT-SMS dest_port=%u",
                   client_ts_data->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.dest_port,
                   0,
                   0);
          ret_value = TRUE;
          break;
        }
      }
    }
    else
    {
      MSG_HIGH("No user_data header", 0, 0, 0);
    }
  }
  else
  {
    MSG_HIGH("SMS not WMS_TPDU_DELIVER", 0, 0, 0);
  }
  return ret_value;
}

/*=========================================================================
FUNCTION
supl_mt_sms_ck

DESCRIPTION
  Check if a GSM / WCDMA message is a SUPL MT-SMS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Is SUPL MT-SMS
  FALSE: Is Not SUPL MT-SMS

SIDE EFFECTS
  None

=========================================================================*/
boolean supl_mt_sms_ck
(
  const wms_client_message_s_type     *msg_ptr
)
{
  wms_client_ts_data_s_type *client_ts_data = NULL;
  boolean ret_value = FALSE;
  const wms_raw_ts_data_s_type *raw_ts_data_ptr = NULL;

  if (msg_ptr == NULL)
  {
    MSG_ERROR("Null Parameter Passed in supl_mt_sms_ck", 0, 0, 0);
    return FALSE;
  }

  if(msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
  {
    raw_ts_data_ptr = &msg_ptr->u.cdma_message.raw_ts;
  }
  else if (msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
  {
    raw_ts_data_ptr = &msg_ptr->u.gw_message.raw_ts_data;
  }
  else
  {
    MSG_ERROR("Incorrect Message Mode", 0, 0, 0);
    return FALSE;
  }
 
  client_ts_data = 
    (wms_client_ts_data_s_type*)os_MemAlloc(sizeof(wms_client_ts_data_s_type), OS_MEM_SCOPE_TASK);

  if ( client_ts_data == NULL )
  {
    MSG_ERROR("Memory Allocation Failed!", 0, 0, 0);
  }
  else
  {
    if(msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
    {
      MSG_HIGH("Teleservice id recieved %d", msg_ptr->u.cdma_message.teleservice,0,0);
      if(msg_ptr->u.cdma_message.teleservice == 4115)
      {
        ret_value = TRUE;
      }
    }
    else if (msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
    {
      if (wms_ts_decode(raw_ts_data_ptr, client_ts_data) == WMS_OK_S)
      {
        ret_value = supl_mt_sms_gw_hdr_chk(client_ts_data);
      }
      else
      {
        MSG_ERROR("SMS Message Decode Failed!", 0, 0, 0);
      }
    }
    else
    {
      MSG_ERROR("Incorrect Message Mode", 0, 0, 0);       
    }

    os_MemFree((void**)&client_ts_data);
  }
  MSG_MED("supl_mt_sms_ck = %d", ret_value, 0, 0);
  return ret_value;

} 


/*===========================================================================

FUNCTION supl_comm_sms_verify_read_cb

DESCRIPTION
  This function verifies the SMS and reads it for SUPL.
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

VARIABLES:

  const wms_client_message_s_type *p_data:

  wms_client_ts_data_s_type *p_ts_data: pointer to this segment of the SMS

  uint8 *p_MesgContent: Destination pointer, provided; Not used until all segments
                        are in and verified to be legitimate SUPL and this function
                        should copy the WAP_PUSH data payload, i.e. SUPL_INIT to 
                        the memory area pointed to by this pointer; Data pointed to
                        by this pointer will be sent to TM.
   
  uint16 *p_MesgLen: Length of the user data, i.e. SUPL_INIT, to be filled by this 
                    function; Populate with 0 until all segments are in and verified 
                    to be SUPL_INIT;
                        
  uint16 w_MesgContentMaxLen:  Space available (bytes) in the buffer pointed to
                              by p_MesgContent, provided  

  boolean u_isWapPush:        TRUE - is WAP_PUSH,  provided 



===========================================================================*/

static boolean supl_comm_sms_verify_read_cb 
(
  const wms_client_message_s_type *p_data,
  wms_client_ts_data_s_type *p_ts_data,
  uint8 *p_MesgContent,
  uint16 *p_MesgLen,           
  uint16 w_MesgContentMaxLen,
  boolean u_isWapPush 
)
{

  uint8 *p_Src = NULL, *p_Dst = NULL;
  uint16 data_len = 0;
  uint16 space_remaining = 0;
  uint8 cnt = 0;
  uint8 num_headers = 0;
  uint8 msg_ref = 0;
  uint8 total_sm = 0;
  uint8 seq_num = 0;
  boolean u_is_mt_sms = FALSE;
  boolean u_is_from_cdma = FALSE;
  wms_udh_s_type *headers = NULL;

  MSG_MED("supl_comm_sms_verify_read_cb CALLED, %d",u_isWapPush,0,0);

  if ((!p_data) || (!p_ts_data))
  {
    MSG_HIGH("Null pointers for p_data/p_ts_data", 0, 0, 0);
    return FALSE;
  }	

  if((p_data->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) &&
  	 (p_data->u.cdma_message.teleservice != 4115))
  {
     MSG_HIGH("Message mode is CDMA, but not a teleservice-4115 message", 0, 0, 0);
	 return FALSE; 
  }

  if ( !u_isWapPush )
  {
    /* Not WAP_PUSH. Check if it is SUPL MT SMS */
    u_is_mt_sms = supl_mt_sms_ck(p_data);
    if ( !u_is_mt_sms )
    {
      MSG_MED("supl_comm_sms_verify_read_cb: SMS not WAP-PUSH or MT-SUPL", 0, 0, 0);
      *p_MesgLen = 0;
      return FALSE;
    }
  }

  if(p_ts_data->format == WMS_FORMAT_GW_PP)
  {
    headers = &(p_ts_data->u.gw_pp.u.deliver.user_data.headers[0]);    
    p_Src = p_ts_data->u.gw_pp.u.deliver.user_data.sm_data;
    data_len = p_ts_data->u.gw_pp.u.deliver.user_data.sm_len;    
    num_headers = p_ts_data->u.gw_pp.u.deliver.user_data.num_headers;
  }
  else if(p_ts_data->format == WMS_FORMAT_CDMA)
  {
    headers = &(p_ts_data->u.cdma.user_data.headers[0]);    
    p_Src = p_ts_data->u.cdma.user_data.data;
    data_len = p_ts_data->u.cdma.user_data.data_len;    
    num_headers = p_ts_data->u.cdma.user_data.num_headers;

    u_is_from_cdma = TRUE; // msg from CDMA 
  }
  else
  {
    MSG_ERROR("Wrong SMS format",0,0,0);
    return FALSE;
  }

  /* WAP Push could be segmented over multiple SMS messages. Check the concatenation
     headers, if they exist. Assume concatenation information can be found in any of the
     headers and we will use the valid concatenation information in the highest numbered 
     header */

  MSG_MED("supl_comm_sms_verify_read_cb: # of hdrs = %d",num_headers,0,0);

  for ( cnt = 0; cnt < num_headers; cnt++ )
  {
    MSG_MED("supl_comm_sms_verify_read_cb: hdr_id = %x",headers[cnt].header_id,0,0);
 
    if ( headers[cnt].header_id == WMS_UDH_CONCAT_8 )
    {
      /* This is a multi-part message */

      msg_ref = headers[cnt].u.concat_8.msg_ref;

      total_sm = headers[cnt].u.concat_8.total_sm;

      seq_num = headers[cnt].u.concat_8.seq_num;

      MSG_HIGH("supl_comm_sms_verify_read_cb: msg_ref = %d, total_sm = %d, seq_num = %d", msg_ref, total_sm, seq_num);

      if (wap_push_multipart_supl_init_found && wap_push_multipart_in_progress && ( msg_ref != wap_push_multipart_msg_ref ))
      {
        uint8 *push_content_ptr_local;
        uint16 push_content_len_local = 0;

        /*Check whether WAP Push contains SUPL_INIT*/        
        if ( FALSE == supl_comm_dsm_verify_partial_wap_push( p_Src, 
                                          data_len,
                                          &push_content_ptr_local,
                                          &push_content_len_local ) )
        {
          /*NOT a SUPL_INIT, ignore it*/
          MSG_HIGH("Wap Push does not contain SUPL_INIT, ignoring it",0,0,0);
          *p_MesgLen = 0;
           return FALSE; /* Do not claim this message*/
        }
        else
        {
          /*WAP Push has SUPL_INIT, treat as fresh attempt*/
          MSG_HIGH("Inconsistent Wap Push, but contains SUPL_INIT, treating as fresh NI attempt",0,0,0);
          tm_umts_supl_comm_sms_read_state_init();
        }
      }
      
      if ( !wap_push_multipart_in_progress )
      {
        wap_push_multipart_msg_ref = msg_ref;

        if ( total_sm == 0 )
        {
          /* Disregard this header if 0 */
          continue;
        }
        else
        {
          wap_push_multipart_total_sm = total_sm;
        }

        if ( seq_num == 0 )
        {
          /* Disregard this header if 0 */
          continue;
        }
        else
        {
          wap_push_multipart_seq_num = seq_num;
        }

        wap_push_multipart_in_progress = TRUE;
      }
      else
      {
        /* Validate received segment */
        if ( ( seq_num > wap_push_multipart_total_sm ) || ( seq_num != wap_push_multipart_seq_num + 1 ) ||
             ( total_sm != wap_push_multipart_total_sm ))
        {
          MSG_HIGH("supl_comm_sms_verify_read_cb: Inconsistent WAP Push segment received - Discarding",0,0,0);
          tm_umts_supl_comm_sms_read_state_init();
          *p_MesgLen = 0;
          return TRUE; /* claim this message, so nobody else would attempt to use it either */
        }

        wap_push_multipart_seq_num = seq_num;

      } /* close else ( i.e. wap_push_multipart_in_progress ) */
    } /* close if-this-is-a-multipart-message */
  } /* for-loop-num-of-headers */


  /* Make sure the length won't exceed the remaining buffer space */
  space_remaining = sizeof(wap_push_buffer) - wap_push_buffer_len;

  if ( data_len > space_remaining )
    data_len = space_remaining;

  if(data_len > 0)
  {
    p_Dst = (uint8 *)(wap_push_buffer + wap_push_buffer_len);

    memscpy( p_Dst, data_len, p_Src, data_len );

    wap_push_buffer_len += data_len; 
  }
  else
  {
    MSG_ERROR("No space remaining in Wap Push Buffer",0,0,0);
  }

  MSG_MED("supl_comm_sms_verify_read_cb: wap_push_multipart_in_progress = %d, seq_num = %d, wap_push_multipart_total_sm = %d",wap_push_multipart_in_progress,seq_num,wap_push_multipart_total_sm);

  if ( wap_push_multipart_in_progress && 
       seq_num != wap_push_multipart_total_sm &&
       !u_is_mt_sms)
  {
    MSG_HIGH("supl_comm_sms_verify_read_cb: Partial message check for SUPL Init",0,0,0);
    if ( FALSE == supl_comm_dsm_verify_partial_wap_push( wap_push_buffer, 
                                      wap_push_buffer_len,
                                      &push_content_ptr,
                                      &push_content_len ) )
    {
      /* WAP PUSH header shows this is not OMA compliant SUPL message. */

      if ( w_MesgContentMaxLen < wap_push_buffer_len )
      {
        MSG_HIGH("supl_comm_sms_verify_read_cb: WAP_PUSH truncated", 0, 0, 0);
        wap_push_buffer_len = w_MesgContentMaxLen;
      }
    
      wap_push_buffer_len = (wap_push_buffer_len < ARR_SIZE(wap_push_buffer) ) ?  
                            wap_push_buffer_len : ARR_SIZE(wap_push_buffer);
                   

      /* copy the entire WAP PUSH to PDCOMM for possible other clients' 
         consumption */
      if ((p_MesgContent != NULL) && (p_MesgLen != NULL))
      {
        memscpy(p_MesgContent, wap_push_buffer_len, (void*)&wap_push_buffer[0], wap_push_buffer_len );
        *p_MesgLen = wap_push_buffer_len;
      }
      /* clear the static variable, no need to maintain them here anymore */
      tm_umts_supl_comm_sms_read_state_init();
      return FALSE;  
    }
    else
    {
      /* Set the flag to indicate : SUPL INIT header is found and stored in wap_push_buffer */
      wap_push_multipart_supl_init_found = TRUE;
      /*Start multipart_wap_push_wait timer*/
      MSG_HIGH("SUPL MULTIPART WAP PUSH WAIT timer started",0,0,0);
      (void) os_TimerStart(supl_multipart_wap_push_wait_timer, SUPL_MULTIPART_WAP_PUSH_WAIT_MSEC, NULL );
    }
  }

  if ( ( !wap_push_multipart_in_progress ) || 
       ( wap_push_multipart_in_progress && seq_num == wap_push_multipart_total_sm ) )
  {
   
    /* only segment or last segment */

    /*Stop multipart_wap_push_wait timer*/    
    (void) os_TimerStop( supl_multipart_wap_push_wait_timer );

    if ( u_is_mt_sms ) /* if MT-SMS */
    {
      push_content_ptr = wap_push_buffer;
      push_content_len = wap_push_buffer_len;
      
      if ( w_MesgContentMaxLen < push_content_len )
      {
        MSG_HIGH("supl_comm_sms_verify_read_cb: WAP_PUSH truncated", 0, 0, 0);
        push_content_len = w_MesgContentMaxLen;
      }

      /* copy the push content, i.e., SUPL_INIT to PDCOMM */
      if ((p_MesgContent != NULL) && (p_MesgLen != NULL))
      {
        memscpy(p_MesgContent, push_content_len, push_content_ptr, push_content_len );
        *p_MesgLen = push_content_len;
      }
      tm_umts_supl_comm_sms_read_state_init();

      // u_is_mt_sms==TRUE (not WAP Push)
      if (u_is_from_cdma)
      {
        z_supl_init_source = PDSM_SUPL_INIT_SOURCE_TYPE_SMS_1X; /* SUPL INIT came over 1X SMS */
      }
      else
      {
        z_supl_init_source = PDSM_SUPL_INIT_SOURCE_TYPE_SMS_GSM; /* SUPL INIT came over GSM SMS */
      }

      return TRUE; 
    }

    /* WAP-PUSH continues here */
    MSG_MED("supl_comm_sms_verify_read_cb: Complete message check for SUPL Init",0,0,0);
    if ( supl_comm_dsm_verify_wap_push( wap_push_buffer, 
                                      wap_push_buffer_len,
                                      &push_content_ptr,
                                      &push_content_len ) )
    {
      /* WAP PUSH header indicates a valid SUPL message */
      if ( w_MesgContentMaxLen < push_content_len )
      {
        MSG_HIGH("supl_comm_sms_verify_read_cb: WAP_PUSH truncated", 0, 0, 0);
        push_content_len = w_MesgContentMaxLen;
      }

      /* copy the push content, i.e., SUPL_INIT to PDCOMM */
      if ((p_MesgContent != NULL) && (p_MesgLen != NULL))
      {
        memscpy(p_MesgContent, push_content_len, push_content_ptr, push_content_len );
        *p_MesgLen = push_content_len;
      }

      z_supl_init_source = PDSM_SUPL_INIT_SOURCE_TYPE_WAP_PUSH; /* SUPL INIT came over WAP-Push */

      tm_umts_supl_comm_sms_read_state_init();
      return TRUE; 

    }

    else
    {
    
      /* WAP PUSH header shows this is not OMA compliant SUPL message,
         let other possible client to try this message */

      if ( w_MesgContentMaxLen < wap_push_buffer_len )
      {
        MSG_HIGH("supl_comm_sms_verify_read_cb: WAP_PUSH truncated", 0, 0, 0);
        wap_push_buffer_len = w_MesgContentMaxLen;
      }

      wap_push_buffer_len = (wap_push_buffer_len < ARR_SIZE(wap_push_buffer) ) ?  
                            wap_push_buffer_len : ARR_SIZE(wap_push_buffer);
    
      /* copy the entire WAP PUSH to PDCOMM for possible other clients' 
         consumption */
      if ((p_MesgContent != NULL) && (p_MesgLen != NULL))
      {
        memscpy(p_MesgContent, wap_push_buffer_len, (void*)&wap_push_buffer[0], wap_push_buffer_len );
        *p_MesgLen = wap_push_buffer_len;
      }
      tm_umts_supl_comm_sms_read_state_init();
      return FALSE;  
    }

  } /* close-if-onlySegment-or-lastSegment */

  else
  {
    /* non-last in multi-segment */
    MSG_HIGH("supl_comm_sms_verify_read_cb: WAP PUSH Validity not checked as not the last segment",0,0,0);
    *p_MesgLen = 0;
    return TRUE;  
  }

}

/*
 ******************************************************************************
 * supl_comm_dsm_verify_partial_wap_push
 *
 * Function description:
 *
 * This function examines a received partial WAP Push message to determine if it contains
 * a SUPL MT trigger. If so, it goes on to accumulate the rest of the message, payload is
 * not extracted yet. If not, it discards the message.
 *
 * Parameters: 
 *
 * data - Pointer to received WAP Push message
 * data_len - Length in bytes of received WAP Push message
 * ppPushContent - Pointer to a variable in which to store a pointer to the received WAP Push
 *   payload.
 * pPushLen - Pointer to a variable in which to store the length of the WAP Push Payload
 *
 * Return value: 
 *
 * TRUE if a SUPL WAP Push was detected and payload was successfully extracted
 * FALSE otherwise
 *
 ******************************************************************************
*/
static boolean supl_comm_dsm_verify_partial_wap_push( uint8 *data, uint16 data_len, uint8 **ppPushContent, uint16 *pPushLen )
{
  uint8 *pUserData = data;
  uint8 PDUType;
  uint8 PushHeaderLen;
  uint32 q_word;
  uint8 len;

  /* Try to determine if this partial message has a SUPL Init in it. */

  MSG_HIGH("supl_comm_dsm_verify_partial_wap_push: data_len = %d", data_len,0,0);
  if ( data_len < 4 )
  {
    /* Message must be at least 4 bytes long to be able to check that what it has is not
       SUPL Init. If not enough data present, we want to keep accumulating. */
  return ( TRUE );
  }

  pUserData++;                     /* TID */
  PDUType = *pUserData++;          /* PDU Type */
  MSG_HIGH("supl_comm_dsm_verify_partial_wap_push: PDUType = %d", PDUType,0,0);
  if ( PDUType != 0x6 ) /* PDU Type 0x6 == Push */
    return( FALSE );

  PushHeaderLen = *pUserData++; /* Push Header Length */

  MSG_HIGH("supl_comm_dsm_partial_verify_wap_push: PushHeaderLen = %d", PushHeaderLen,0,0);
  if ( PushHeaderLen == 0 )
    return( FALSE );

  if (data_len >= 36)
    len = 33;
  else
    len = data_len - 3;

  /* Check content type field  up to the available number of bytes. */
  if ( !strncmp("application/vnd.omaloc-supl-init", (char *)pUserData, len ))
    return ( TRUE );
  /*check for assinged number for content type ID code of "application/vnd.omaloc-supl-init" */
  else if ( data_len >= 7)
  {
    q_word = (uint32) (*pUserData) << 24;
    q_word |= (uint32) ( *(pUserData + 1) ) << 16;
    q_word |= (uint32) ( *(pUserData + 2) ) << 8;
    q_word |= *(pUserData + 3);

    if (q_word == 0x03020312) /* this is the only legal WAP-encoded assigned number for content type */
    {
      return ( TRUE );
    }
    else
    {
      q_word = (uint32)(*pUserData) <<16;
      q_word |= (uint32)( *(pUserData + 1) ) << 8;
      q_word |= *(pUserData + 2);

      if (q_word == 0x020312) /* this is not really legal but appeared once in a version of OMA-TS-ULP, later removed */
      {
        return ( TRUE );
      }
      else
      {
        q_word = (uint32)(*pUserData) << 8;
        q_word |= *(pUserData + 1);

        if (q_word == 0x312) /* this is not really legal but appeared once in a version of OMA-TS-ULP, later removed */
        {
          return ( TRUE );
        }
        else
        {
          MSG_HIGH("supl_comm_dsm_verify_wap_push: return false",0,0,0);
          return FALSE; /* WAP PUSH not for SUPL */
        }
      }
    }
  }
  else
    /* Not enough bytes to determine, keep accumulating. */
    return ( TRUE );
}

/*
 ******************************************************************************
 * supl_comm_dsm_verify_wap_push
 *
 * Function description:
 *
 * This function examines a received WAP Push message to determine if it contains
 * a SUPL MT trigger. If so it extracts the data payload containing the SUPL INIT message.
 *
 * Parameters: 
 *
 * data - Pointer to received WAP Push message
 * data_len - Length in bytes of received WAP Push message
 * ppPushContent - Pointer to a variable in which to store a pointer to the received WAP Push
 *   payload.
 * pPushLen - Pointer to a variable in which to store the length of the WAP Push Payload
 *
 * Return value: 
 *
 * TRUE if a SUPL WAP Push was detected and payload was successfully extracted
 * FALSE otherwise
 *
 ******************************************************************************
*/
static boolean supl_comm_dsm_verify_wap_push( uint8 *data, uint16 data_len, uint8 **ppPushContent, uint16 *pPushLen )
{
  uint8 *pUserData = data;
  uint8 PDUType;
  uint8 PushHeaderLen;
  uint32 q_word;
  
  /* Determine if this is a message destined for the LCAApp */ 

  MSG_MED("supl_comm_dsm_verify_wap_push: data_len = %d", data_len,0,0);
  if ( data_len < 3 )
  {
    /* Message must be at least 3 bytes to account for mandatory headers, if less then 
     something is wrong */
  return ( FALSE );
  }

  pUserData++;                     /* TID */
  PDUType = *pUserData++;          /* PDU Type */
  MSG_MED("supl_comm_dsm_verify_wap_push: PDUType = %d", PDUType,0,0);
  if ( PDUType != 0x6 ) /* PDU Type 0x6 == Push */
    return( FALSE );

  PushHeaderLen = *pUserData++; /* Push Header Length */

  MSG_MED("supl_comm_dsm_verify_wap_push: PushHeaderLen = %d", PushHeaderLen,0,0);
  if ( PushHeaderLen == 0 )
    return( FALSE );

  /* Sanity check to ensure push header is not longer than the message itself */
  MSG_MED("supl_comm_dsm_verify_wap_push: PushHeaderLen > data_len - 3 = %d", data_len - 3,0,0);
  if ( PushHeaderLen > data_len - 3 ) /* Minus 3 to account for TID,PDUType,PushHeaderLen */
    return( FALSE );

  /* Check content type field */
  if ( !strcmp("application/vnd.omaloc-supl-init", (char *)pUserData ) )
  {
    pUserData += 33; /* content type string plus null terminator */
    PushHeaderLen -= 33;
  }

  /*check for assinged number for content type ID code of "application/vnd.omaloc-supl-init" */
  else
  {
    q_word = (uint32) (*pUserData) << 24;
    q_word |= (uint32) ( *(pUserData + 1) ) << 16;
    q_word |= (uint32) ( *(pUserData + 2) ) << 8;
    q_word |= *(pUserData + 3);

    if (q_word == 0x03020312) /* this is the only legal WAP-encoded assigned number for content type */
    {
      pUserData += 4;
      PushHeaderLen -= 4;
    }
    else
    {
      q_word = (uint32)(*pUserData) <<16;
      q_word |= (uint32)( *(pUserData + 1) ) << 8;
      q_word |= *(pUserData + 2);

      if (q_word == 0x020312) /* this is not really legal but appeared once in a version of OMA-TS-ULP, later removed */
      {
        pUserData += 3;
        PushHeaderLen -= 3;
      }
      else
      {
        q_word = (uint32)(*pUserData) << 8;
        q_word |= *(pUserData + 1);

        if (q_word == 0x312) /* this is not really legal but appeared once in a version of OMA-TS-ULP, later removed */
        {
          pUserData += 2;
          PushHeaderLen -= 2;
        }
        else
        {
          MSG_HIGH("WAPPUSH not for SUPL: ContentType check failed",0,0,0);
          return FALSE; /* WAP PUSH not for SUPL */
        }
      }
    }
  }

  /* Find & check for application ID */
  while (PushHeaderLen > 1)
  {
    if (*pUserData == OMNA_APP_ID)
  {
      /* possible x-wap-application-id found, check if it is followed by x-application-Id-field */

      pUserData++;
      PushHeaderLen--;

      /* Check if x-application-Id-field contains a valid ASCII string */
      if ( PushHeaderLen >= 25 &&
           !strncmp("x-oma-application:ulp.ua", (char *)pUserData, 25) )
  {
        pUserData += 25; /* application id string + null terminator */
        PushHeaderLen -= 25;
    }
      else if ( *pUserData == 0x90 ) /* 0x90 is assigned for code of "x-oma-application:ulp.ua" */
  {
        pUserData++;
        PushHeaderLen--;
    }
    else
    {
        /* 0xAF is not followed by the x-application-ID-field for SUPL; continue searching */
        MSG_MED("0xAF not following by x-application-Id-filed, keep searching",0,0,0);
        continue; 
      }

      /* Skip remaining header bytes, to get to the data payload. */
      MSG_MED("x-application-id and its field valid",0,0,0);
      *ppPushContent = pUserData + (uint8)PushHeaderLen;
      *pPushLen = data_len - (uint8)( *ppPushContent - data );
      return TRUE;
    }

    pUserData++;
    PushHeaderLen--;
  }

  /* Could not find x-wap-application-id field */
  MSG_HIGH("WAPPUSH not for SUPL: no valid x-wap-application-id or its filed",0,0,0);
  return FALSE;
  
}



/*===========================================================================

FUNCTION supl_comm_generate_hash

DESCRIPTION
  This function interfaces SUPL with the pdcomm api function of generating
  HASH. 
  
DEPENDENCIES: none.

Return value: TRUE/FALSE

SIDE EFFECTS: none.

===========================================================================*/

boolean supl_comm_generate_hash ( uint8 *p_raw_pdu, uint16 w_raw_pdu_len )
{
  pd_comms_generate_hash_params_type z_hash_params;
  pd_comms_generate_hash_params_type* p_hash_params = &z_hash_params;

  static char tempAscii[120];
  supl_ULP_PDU z_pdu_decoded;
  supl_ULP_PDU* p_pdu_decoded = &z_pdu_decoded;
  int asn1_decode_status;
  uint32 q_supl_version = 0;
  boolean b_emergency = FALSE;
  boolean b_eslp_present = FALSE;
  boolean b_ret = FALSE;

  if (NULL == p_raw_pdu)
  {
     MSG_ERROR("SUPL: NULL parameter passed for PDU data", 0,0,0);
     return FALSE;
  }
 
  if (TRUE == b_pending_hash) 
  {
     MSG_ERROR("Drop NI request, since Hash Resp pending",0,0,0);
     return FALSE;
  }
   
  /*Initialize local SUPL_PDU structure before using it for decoding incoming supl message*/
  memset(&z_pdu_decoded,0,sizeof(supl_ULP_PDU));

  /*Need to decode the PDU to check the SUPL version to decide on 
    HASH algorithm (SHA1 or SHA256)*/
  tm_osys_asn1_decode_pdu( p_raw_pdu, w_raw_pdu_len, (void *)p_pdu_decoded, PROTO_SUPL, &asn1_decode_status);
  
  if ( asn1_decode_status == 0 )
  {
    supl_version_negotiation(p_pdu_decoded, &q_supl_version, 0xFF);
  }
  else /* decode status showing error */
  {
    MSG_MED("SUPL: ASN1 decoding failed during Hash Gen", 0,0,0);
    if ( asn1_decode_status != C_ASN1_DECODE_BUFFER_NOT_ALLOCATED )
    {
      tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);
    }
    return FALSE;
  }

  p_hash_params->e_hash_type = SHA1;
  /*If SUPL 2, use SHA256 if allowed by NV*/
  if ( (q_supl_version & 0x000000FF) == 2 )
  {
    if ( ( z_umts_nv_item_ram_copy.aagps_use_transport_security & 0x4 ) == 0 )
    {
      p_hash_params->e_hash_type = SHA256;
      
    }
  }

  /* ask PDCOMM to generate hash */  
  p_hash_params->p_msg = p_raw_pdu;
  p_hash_params->q_msg_len = w_raw_pdu_len;
  p_hash_params->q_transaction_id = q_current_hash_transac_id;



  /* all non-SUPL_INIT messages */  
  if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT  )
  {
    const supl_SUPLINIT  *p_suplInit = p_pdu_decoded->message.u.msSUPLINIT; 
    if (p_suplInit->m.notificationPresent == 1  )
    {
      if ( p_suplInit->notification.m.ver2_Notification_extensionPresent == 1 )
      {
        /* if emergency call, then ensure the SUPL INIT cotnains the emergency positioning indication */
        if ( p_suplInit->notification.ver2_Notification_extension.m.emergencyCallLocationPresent == 1 )
        {                                                                                            
          b_emergency = TRUE;
          if ( (p_suplInit->m.ver2_SUPL_INIT_extensionPresent == 1 ) &&
               (p_suplInit->ver2_SUPL_INIT_extension.m.e_SLPAddressPresent == 1))
          {
            MSG_HIGH("SUPL_COMM: Using ESLP to generate hash", 0,0,0);
            GNSS_STRLCPY (z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name, 
              p_suplInit->ver2_SUPL_INIT_extension.e_SLPAddress.u.fQDN, 
                         C_SUPL_FQDN_VISIBLE_STRING_BYTE_LENGTH - 1);        
            // NULL terminate
            z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name[255] = 0;
            b_eslp_present = TRUE;
            b_ret = TRUE;
          }
        }
      }
    }
  } /*Emergency SUPL_INIT */
  if (b_emergency == FALSE )
  {
    b_ret = supl_comm_get_slp_address_portnum(&z_comms_connect_params_tcp.z_link_params.z_tcp_params); 
  }
  else
  {
    if (b_eslp_present == FALSE)
    {
      if (z_umts_nv_item_ram_copy.slp_address[0] != '\0')
      {
        MSG_MED("Using SUPL address from NV for hash emergeny as ESLP is not provided",0,0,0);
        /* if no ESLP provided used the NV configured SLP address */
        (void)GNSS_STRLCPY ( z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name,
                      z_umts_nv_item_ram_copy.slp_address, 
                      C_MAX_URL_SIZE_BYTE );
        /*If MT-ES does not provision E-SLP 
          1)Attempt SLP configured in NV
           (SUPL Spec does not specify any particular behavior, except for auto-config-eslp,
            which is not used in customer case)
          2)Set hslp_address used flag. So that if NV SLP also fails we terminate the session
        */
        z_pdcomm_tcp_info.used_auto_configured_hslp_address = TRUE;
        /*NULL Terminate*/
        z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name[255] = 0;
        b_ret = TRUE;
      }
      else
      {
        /* construct the slp address from MCC/MNC of the current servign network */
        MSG_MED("Using SUPL address from IMSI for hash emergeny as ESLP is not provided",0,0,0);
        if (z_cell_info_cached.u_CachedCellIDValid == TRUE)
        {
          MSG_MED("cached cell id valid ",0,0,0);
          b_ret = supl_comm_eslp_address_build_from_mcc_mnc(z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name, &z_cell_info_cached);
          z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name[255] = 0;
        }
      }
    }
  }

  if (asn1_decode_status== 0 || b_ret == FALSE)
  {
    /*Free the PDU, we do not need it anymore*/
    tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);
    if (b_ret == FALSE)
    {
       MSG_ERROR("SUPL: Hash Generation failed", 0,0,0);
       return FALSE;
    }
  }

  /* ensure that host_name is null terminated */
  z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name[255] = 0;
  if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
  {
    MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: SLP address being used for hash%s", z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name);
  }
  p_hash_params->p_key_ptr = (uint8 *)z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name;  
  p_hash_params->w_key_len = strlen(z_comms_connect_params_tcp.z_link_params.z_tcp_params.p_host_name);  

  if ( ! pd_comms_app_generate_hash ( THREAD_ID_SM_TM,
                                      TM_CORE_PDCOMM_MSG_ID_BASE,
                                      p_hash_params )
     )
  {
    return FALSE;
  }

  /*Set Flag if Hash in progress 
   Uncheck the flag after receiving Hash Resp.
   Any Other SUPL UDP NI's will be dropped until Hash Resp*/
  b_pending_hash = TRUE;

  return TRUE;

}


/*===========================================================================

FUNCTION supl_comm_getnext_transaction_id

DESCRIPTION
  This function generates the next transaction id for the specified group 
  
DEPENDENCIES: none.

SIDE EFFECTS: none.

===========================================================================*/

static uint32 supl_comm_getnext_transaction_id(transac_id_group_e_type transac_group)
{
  uint32 next_transac_id = TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_BASE; /* init, will be replaced */

  if ( transac_group == TRANSAC_SMS )
  {
    if ( ( z_comms_connect_params_sms.q_transaction_id < TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_BASE ) ||
         ( z_comms_connect_params_sms.q_transaction_id >= 
           ( TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_BASE + TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_RANGE )
         )
       )
    {
      next_transac_id = TM_UMTS_UP_SUPL_SMS_TRANSACTION_ID_BASE;
      z_comms_connect_params_sms.q_transaction_id = next_transac_id;
    }
    else
    {
      next_transac_id = z_comms_connect_params_sms.q_transaction_id++;
    }

  }

  else if ( transac_group == TRANSAC_TCP )
  {
    if ( ( z_comms_connect_params_tcp.q_transaction_id < TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_BASE ) ||
         ( z_comms_connect_params_tcp.q_transaction_id >= 
           ( TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_BASE + TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_RANGE )
         )
       )
    {
      next_transac_id = TM_UMTS_UP_SUPL_TCP_TRANSACTION_ID_BASE;
      z_comms_connect_params_tcp.q_transaction_id = next_transac_id;
    }
    else
    {
      next_transac_id = z_comms_connect_params_tcp.q_transaction_id++;
    }
  }

  else if ( transac_group == TRANSAC_HASH )
  {
    if ( ( q_current_hash_transac_id < TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE ) ||
         ( q_current_hash_transac_id >= 
           ( TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE + TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_RANGE )
         )
       )
    {
      next_transac_id = TM_UMTS_UP_SUPL_HASH_TRANSACTION_ID_BASE;
      q_current_hash_transac_id = next_transac_id;
    }
    else
    {
      next_transac_id = q_current_hash_transac_id++;
    }
  }

  else if ( transac_group == TRANSAC_UDP )
  {
    if ( ( z_comms_connect_params_udp.q_transaction_id < TM_UMTS_UP_SUPL_UDP_TRANSACTION_ID_BASE ) ||
         ( z_comms_connect_params_udp.q_transaction_id >= 
           ( TM_UMTS_UP_SUPL_UDP_TRANSACTION_ID_BASE + TM_UMTS_UP_SUPL_UDP_TRANSACTION_ID_RANGE )
         )
       )
    {
      next_transac_id = TM_UMTS_UP_SUPL_UDP_TRANSACTION_ID_BASE;
      z_comms_connect_params_udp.q_transaction_id = next_transac_id;
    }
    else
    {
      next_transac_id = z_comms_connect_params_udp.q_transaction_id++;
    }
  }

  return next_transac_id;

}

/********************************************************************
*
* tm_umts_up_supl_comm_retrieve_imsi
*
* Function description:
*  Retrieve the IMSI value from the the SIM and store a copy 
*  in the Ram (z_umts_nv_item_ram_copy).
*
* Parameters: 
*
* Return:
*  TRUE/FALSE
*  
*********************************************************************
*/
boolean tm_umts_up_supl_comm_retrieve_imsi(void)
{
  boolean bRetVal;
  cgps_IMSIType imsiInfo;

  memset(&imsiInfo, 0x0, sizeof(imsiInfo));
  
  imsiInfo.sub = (uint8) tm_core_get_current_dd_subs_id();

  /* Retrieve the IMSI value from the USIM*/
  bRetVal = tm_sim_retrieve_imsi(&imsiInfo);

  if( !bRetVal )
  {
    return FALSE;
  }
  else
  {
    memscpy(&z_umts_nv_item_ram_copy.set_imsi, sizeof(z_umts_nv_item_ram_copy.set_imsi),
            &imsiInfo.imsi, sizeof(imsiInfo.imsi));
    numMncDigits = imsiInfo.numMNCdigits;
    
    return TRUE;
  }

}

/*===========================================================================
FUNCTION tm_1x_up_comm_app_ioctl

DESCRIPTION
  This function is to send the ioctl to PDCOMM

RETURN VALUE
  Boolean

DEPENDENCIES      

===========================================================================*/
boolean  tm_umts_up_supl_comm_app_ioctl(pd_comms_ioctl_cmd_type ioctl_cmd)
{
  /*This function is presently used by IS801 to request SUPL to send ioctl to PDCOMM*/

  /* Local Variables */
  pd_comms_return_type ret_val;
  pd_comms_ioctl_params_type ioctl_params;

  /* Set the IOCTL parameters*/
  ioctl_params.q_ioctl_request = ioctl_cmd;
  ioctl_params.q_transaction_id = supl_comm_getnext_transaction_id(TRANSAC_TCP);
  ioctl_params.p_argument = NULL;
  ioctl_params.q_argument_len = 0;

  ret_val = pd_comms_app_ioctl(z_pdcomm_handle_tcp,
                               (uint32)THREAD_ID_SM_TM,
                               TM_CORE_PDCOMM_MSG_ID_BASE,
                               &ioctl_params);     

  if(ret_val != PDCOMM_RESULT_WAIT)
  {
    return FALSE;
  }
  return TRUE;
}


/*===========================================================================
FUNCTION tm_umts_supl_comm_sms_read_state_init

DESCRIPTION
  This function is used to initialize sms read state.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_umts_supl_comm_sms_read_state_init(void)
{
  wap_push_buffer_len = 0;
  push_content_len = 0;
  wap_push_multipart_in_progress = FALSE;
  wap_push_multipart_msg_ref = 0;
  wap_push_multipart_total_sm = 0;
  wap_push_multipart_seq_num = 0;
  wap_push_multipart_supl_init_found = FALSE;
  (void) os_TimerStop( supl_multipart_wap_push_wait_timer );
}


/*===========================================================================
FUNCTION tm_umts_up_supl_comm_free_pdcomm_rx_buff

DESCRIPTION
  This function frees the buffer allocated for pdcomm_rx.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_umts_up_supl_comm_free_pdcomm_rx_buff(void)
{
  /* p_pdcomm_rx_buff : global, defined in tm_umts_up_supl.h */
   
  if (p_pdcomm_rx_buff == NULL)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "No need to free pdcomm_rx_buff: NULL %lx", 
          p_pdcomm_rx_buff);
  
  }
  else
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
          "Freeing pdcomm_rx_buff: %lx", 
          p_pdcomm_rx_buff);
  
    os_MemFree((void**)&p_pdcomm_rx_buff);
    p_pdcomm_rx_buff = NULL;
    u_buffer_counter--;
  
  }

}


/*===========================================================================
FUNCTION tm_umts_up_supl_comm_ni_sms

DESCRIPTION
  This function will handle the NI SMS injected from PDAPI

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_umts_up_supl_comm_ni_sms(uint32 length, uint8 *data)
{
  pd_comms_inject_msg_params_type  sms_params;  
  pd_comms_return_type             pdcomm_sms_ret_value; 

  if (length == 0 || length > PDSM_MAX_NI_MSG_LEN || data == NULL)
  {
    MSG_ERROR("Invalid SUPL NI SMS (len=%d data=%x)",length,data,0);
    return;
  }

  sms_params.e_link_type = LINK_SMS;
  sms_params.q_transaction_id = supl_comm_getnext_transaction_id(TRANSAC_SMS);
  sms_params.q_msg_len = length;
  sms_params.p_msg = data; /* buffer will be freed inside PDCOMM WMS */
  
  pdcomm_sms_ret_value =  pd_comms_app_inject_msg( z_pdcomm_handle_sms, 
                                              THREAD_ID_SM_TM, 
                                              TM_CORE_PDCOMM_MSG_ID_BASE, 
                                              &sms_params );

  if ( ( pdcomm_sms_ret_value != PDCOMM_RESULT_OK ) && ( pdcomm_sms_ret_value != PDCOMM_RESULT_WAIT ) )
  {    
    MSG_ERROR("SUPL NI SMS injection failed",0,0,0);
  }

}


#endif
