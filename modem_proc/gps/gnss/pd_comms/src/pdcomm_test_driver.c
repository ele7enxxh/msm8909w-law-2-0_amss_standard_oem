#include "gps_variation.h"
#ifndef __GRIFFON__
#include "comdef.h"
#include "msg.h"
#include "task.h"
#include "string.h"
#include "pd_comms_api.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "wms.h"

#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
/*#define PORTNUM 5000*/
/*#define PORTNUM 59910*/
/*#define SERVER_IP 0x812EE14B*/
/*#define SERVER_IP "mrawal.qualcomm.com"*/
/*#define TEST_STRING "ARIES PDCOMM Test"*/

/*#define PDCOMM_TD_DOG_TIMER C_PD_COMMS_CLIENT_MSG_IPC_END + 1
#define PDCOMM_TD_CONNECT_TIMER C_PD_COMMS_CLIENT_MSG_IPC_END + 2
#define PDCOMM_TD_ABORT_TIMER C_PD_COMMS_CLIENT_MSG_IPC_END + 3*/

typedef struct
{

uint16 PORTNUM; 
uint32 SERVER_IP_long;
char SERVER_IP[50]; 
boolean use_HostName;
boolean use_security;
boolean test_earlyConnect;
boolean test_abort;    
boolean test_hash;
uint32 q_connectInterval; /*If zero tcp connections won't be tested*/

    
}pdcomm_td_ctrl_type;

typedef enum
{
PDCOMM_TD_DOG_TIMER = C_USR_MSG_ID_FIRST+1,
PDCOMM_TD_CONNECT_TIMER,
PDCOMM_TD_ABORT_TIMER,
PDCOMM_TD_LOCAL_IPC_MSG_ID_END,

PDCOMM_TD_CLIENT_MSG_WRITE_ACK = PDCOMM_TD_LOCAL_IPC_MSG_ID_END,
PDCOMM_TD_CLIENT_MSG_READ_DATA,
PDCOMM_TD_CLIENT_MSG_NOTIFY,
PDCOMM_TD_CLIENT_MSG_IOCTL_ACK,
PDCOMM_TD_CLIENT_MSG_HASH_RESP,
PDCOMM_TD_CLIENT_MSG_HANDLE,
PDCOMM_TD_CLIENT_MSG_SMS_ACK_REPORT,    
} test_driver_ipc_msg_id_type;

typedef struct {
  wms_client_bd_s_type       bds;    /* Bearer data */
  wms_client_message_s_type  msgs;
} v1_mo_type;


v1_mo_type sms_msg;

static pd_comms_handle_type handle;
static pd_comms_handle_type z_sms_handle;
static uint32 transaction_id = 0;
static pd_comms_state_type cur_state = PDCOMM_STATE_DISCONNECTED;
static int bytes_read =0;

static wms_address_s_type z_wms_address;

static boolean read_pending = FALSE;
static int msg_len=0;

static boolean sms_read_pending = FALSE;
static int sms_msg_len=0;

static boolean pdcomm_td_forceDormancy = FALSE;

/*static int pdcomm_max_data= 4500;*/
static int pdcomm_max_data= 30;
static char TEST_STRING[200];
/*Send SUPL_START*/
 static char SUPL_START[] = {0x00,0x23,0x01,0x00,0x00,0x80,0x02,0xC0,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x53,0x00,0x42,0xC3,0xD8,0x5A,0x00,0x00,0x54,0xC1,0x00,0xDC,0x03,0x63,0xD1,0x20,0x40,0x00,0x60};

 static char SUPL_END[] = {0x00,0x19,0x01,0x00,0x00,0xC0,0x02,0xC0,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x00,0x00,0x00,0x05,0x02,0x30,0xD3,0x94,0x00};
static pdcomm_td_ctrl_type pd_comm_ctrl;

static boolean disconnect_called = FALSE;

static os_TimerMsgType* connect_timer;
//static os_TimerMsgType* abort_timer = NULL;

/* If no root certificate exists in EFS, use the default root certificate */
static const uint8 p_default_cert[]={
0x30, 0x82, 0x3, 0x8f, 0x30, 0x82, 0x2, 0xf8, 0xa0, 0x3, 
0x2, 0x1, 0x2, 0x2, 0x1, 0x0, 0x30, 0xd, 0x6, 0x9, 
0x2a, 0x86, 0x48, 0x86, 0xf7, 0xd, 0x1, 0x1, 0x4, 0x5, 
0x0, 0x30, 0x81, 0x92, 0x31, 0x11, 0x30, 0xf, 0x6, 0x3, 
0x55, 0x4, 0xa, 0x13, 0x8, 0x51, 0x75, 0x61, 0x6c, 0x63, 
0x6f, 0x6d, 0x6d, 0x31, 0xc, 0x30, 0xa, 0x6, 0x3, 0x55, 
0x4, 0xb, 0x13, 0x3, 0x51, 0x43, 0x54, 0x31, 0x22, 0x30, 
0x20, 0x6, 0x9, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0xd, 0x1, 
0x9, 0x1, 0x16, 0x13, 0x71, 0x63, 0x75, 0x73, 0x65, 0x72, 
0x40, 0x71, 0x75, 0x61, 0x6c, 0x63, 0x6f, 0x6d, 0x6d, 0x2e, 
0x63, 0x6f, 0x6d, 0x31, 0x11, 0x30, 0xf, 0x6, 0x3, 0x55, 
0x4, 0x7, 0x13, 0x8, 0x43, 0x61, 0x6d, 0x70, 0x62, 0x65, 
0x6c, 0x6c, 0x31, 0xb, 0x30, 0x9, 0x6, 0x3, 0x55, 0x4, 
0x8, 0x13, 0x2, 0x43, 0x41, 0x31, 0xb, 0x30, 0x9, 0x6, 
0x3, 0x55, 0x4, 0x6, 0x13, 0x2, 0x55, 0x53, 0x31, 0x1e, 
0x30, 0x1c, 0x6, 0x3, 0x55, 0x4, 0x3, 0x13, 0x15, 0x63, 
0x61, 0x6d, 0x63, 0x64, 0x67, 0x30, 0x38, 0x2e, 0x71, 0x75, 
0x61, 0x6c, 0x63, 0x6f, 0x6d, 0x6d, 0x2e, 0x63, 0x6f, 0x6d, 
0x30, 0x1e, 0x17, 0xd, 0x30, 0x36, 0x31, 0x31, 0x30, 0x37, 
0x30, 0x30, 0x32, 0x37, 0x34, 0x36, 0x5a, 0x17, 0xd, 0x31, 
0x36, 0x31, 0x31, 0x30, 0x34, 0x30, 0x30, 0x32, 0x37, 0x34, 
0x36, 0x5a, 0x30, 0x81, 0x92, 0x31, 0x11, 0x30, 0xf, 0x6, 
0x3, 0x55, 0x4, 0xa, 0x13, 0x8, 0x51, 0x75, 0x61, 0x6c, 
0x63, 0x6f, 0x6d, 0x6d, 0x31, 0xc, 0x30, 0xa, 0x6, 0x3, 
0x55, 0x4, 0xb, 0x13, 0x3, 0x51, 0x43, 0x54, 0x31, 0x22, 
0x30, 0x20, 0x6, 0x9, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0xd, 
0x1, 0x9, 0x1, 0x16, 0x13, 0x71, 0x63, 0x75, 0x73, 0x65, 
0x72, 0x40, 0x71, 0x75, 0x61, 0x6c, 0x63, 0x6f, 0x6d, 0x6d, 
0x2e, 0x63, 0x6f, 0x6d, 0x31, 0x11, 0x30, 0xf, 0x6, 0x3, 
0x55, 0x4, 0x7, 0x13, 0x8, 0x43, 0x61, 0x6d, 0x70, 0x62, 
0x65, 0x6c, 0x6c, 0x31, 0xb, 0x30, 0x9, 0x6, 0x3, 0x55, 
0x4, 0x8, 0x13, 0x2, 0x43, 0x41, 0x31, 0xb, 0x30, 0x9, 
0x6, 0x3, 0x55, 0x4, 0x6, 0x13, 0x2, 0x55, 0x53, 0x31, 
0x1e, 0x30, 0x1c, 0x6, 0x3, 0x55, 0x4, 0x3, 0x13, 0x15, 
0x63, 0x61, 0x6d, 0x63, 0x64, 0x67, 0x30, 0x38, 0x2e, 0x71, 
0x75, 0x61, 0x6c, 0x63, 0x6f, 0x6d, 0x6d, 0x2e, 0x63, 0x6f, 
0x6d, 0x30, 0x81, 0x9f, 0x30, 0xd, 0x6, 0x9, 0x2a, 0x86, 
0x48, 0x86, 0xf7, 0xd, 0x1, 0x1, 0x1, 0x5, 0x0, 0x3, 
0x81, 0x8d, 0x0, 0x30, 0x81, 0x89, 0x2, 0x81, 0x81, 0x0, 
0xe9, 0x7, 0x9d, 0xbb, 0x24, 0x52, 0x10, 0x49, 0x90, 0x98, 
0x21, 0x32, 0x47, 0xb, 0xa2, 0xb, 0x7c, 0x79, 0x5d, 0x1b, 
0x46, 0x90, 0xb7, 0x18, 0xb6, 0x2f, 0xcd, 0x38, 0xd7, 0x1d, 
0x4e, 0x45, 0x8f, 0xaf, 0x32, 0x3, 0x74, 0xb8, 0x9d, 0x52, 
0x36, 0xc7, 0x9b, 0xd5, 0x7d, 0x2b, 0xa2, 0xd3, 0x50, 0x8a, 
0x4a, 0x60, 0x5b, 0xd, 0x48, 0xcb, 0x8c, 0xa5, 0x47, 0x8b, 
0xfe, 0x4d, 0x7d, 0x32, 0xab, 0xa, 0xe0, 0x72, 0xbc, 0x36, 
0x7a, 0x96, 0x15, 0xf0, 0x2, 0xd5, 0x2, 0x3a, 0x61, 0x7b, 
0x42, 0x2f, 0xec, 0x1e, 0xf8, 0xda, 0xd7, 0x72, 0xd7, 0x5e, 
0x9c, 0x4f, 0x6, 0xef, 0x62, 0x4b, 0x86, 0x46, 0x99, 0xa3, 
0xf0, 0x80, 0xd1, 0xb8, 0xe1, 0x92, 0xb9, 0x21, 0xd1, 0x59, 
0x85, 0x2b, 0x2d, 0xc7, 0x98, 0xf7, 0x52, 0xb4, 0xf1, 0xfa, 
0x52, 0x9f, 0xf1, 0x23, 0xd9, 0x96, 0x3f, 0x73, 0x2, 0x3, 
0x1, 0x0, 0x1, 0xa3, 0x81, 0xf2, 0x30, 0x81, 0xef, 0x30, 
0xc, 0x6, 0x3, 0x55, 0x1d, 0x13, 0x4, 0x5, 0x30, 0x3, 
0x1, 0x1, 0xff, 0x30, 0x1d, 0x6, 0x3, 0x55, 0x1d, 0xe, 
0x4, 0x16, 0x4, 0x14, 0x4b, 0xfe, 0x90, 0xe6, 0xe6, 0x4d, 
0xb3, 0x86, 0x6b, 0x98, 0xd7, 0x11, 0x19, 0x7c, 0x6b, 0x7e, 
0xdd, 0xed, 0xd, 0x4b, 0x30, 0x81, 0xbf, 0x6, 0x3, 0x55, 
0x1d, 0x23, 0x4, 0x81, 0xb7, 0x30, 0x81, 0xb4, 0x80, 0x14, 
0x4b, 0xfe, 0x90, 0xe6, 0xe6, 0x4d, 0xb3, 0x86, 0x6b, 0x98, 
0xd7, 0x11, 0x19, 0x7c, 0x6b, 0x7e, 0xdd, 0xed, 0xd, 0x4b, 
0xa1, 0x81, 0x98, 0xa4, 0x81, 0x95, 0x30, 0x81, 0x92, 0x31, 
0x11, 0x30, 0xf, 0x6, 0x3, 0x55, 0x4, 0xa, 0x13, 0x8, 
0x51, 0x75, 0x61, 0x6c, 0x63, 0x6f, 0x6d, 0x6d, 0x31, 0xc, 
0x30, 0xa, 0x6, 0x3, 0x55, 0x4, 0xb, 0x13, 0x3, 0x51, 
0x43, 0x54, 0x31, 0x22, 0x30, 0x20, 0x6, 0x9, 0x2a, 0x86, 
0x48, 0x86, 0xf7, 0xd, 0x1, 0x9, 0x1, 0x16, 0x13, 0x71, 
0x63, 0x75, 0x73, 0x65, 0x72, 0x40, 0x71, 0x75, 0x61, 0x6c, 
0x63, 0x6f, 0x6d, 0x6d, 0x2e, 0x63, 0x6f, 0x6d, 0x31, 0x11, 
0x30, 0xf, 0x6, 0x3, 0x55, 0x4, 0x7, 0x13, 0x8, 0x43, 
0x61, 0x6d, 0x70, 0x62, 0x65, 0x6c, 0x6c, 0x31, 0xb, 0x30, 
0x9, 0x6, 0x3, 0x55, 0x4, 0x8, 0x13, 0x2, 0x43, 0x41, 
0x31, 0xb, 0x30, 0x9, 0x6, 0x3, 0x55, 0x4, 0x6, 0x13, 
0x2, 0x55, 0x53, 0x31, 0x1e, 0x30, 0x1c, 0x6, 0x3, 0x55, 
0x4, 0x3, 0x13, 0x15, 0x63, 0x61, 0x6d, 0x63, 0x64, 0x67, 
0x30, 0x38, 0x2e, 0x71, 0x75, 0x61, 0x6c, 0x63, 0x6f, 0x6d, 
0x6d, 0x2e, 0x63, 0x6f, 0x6d, 0x82, 0x1, 0x0, 0x30, 0xd, 
0x6, 0x9, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0xd, 0x1, 0x1, 
0x4, 0x5, 0x0, 0x3, 0x81, 0x81, 0x0, 0x34, 0x95, 0x8d, 
0xc7, 0x6d, 0x69, 0x8a, 0xfc, 0xd, 0xde, 0x2a, 0x4c, 0x8e, 
0x10, 0xe2, 0x3d, 0xbe, 0xe2, 0xb3, 0x94, 0x6, 0x80, 0xdd, 
0xc, 0xe9, 0x7b, 0x42, 0xc6, 0x7b, 0xe6, 0x3e, 0xac, 0x5b, 
0x5f, 0x7c, 0x47, 0x7c, 0x84, 0xca, 0x5a, 0x8, 0x31, 0xb9, 
0xe3, 0x8f, 0x34, 0xbd, 0x15, 0x61, 0x41, 0xb0, 0xf1, 0x7d, 
0xc7, 0xfa, 0x51, 0x5e, 0xfe, 0xd9, 0xac, 0xeb, 0xdd, 0xec, 
0x85, 0x8f, 0x6a, 0x14, 0x25, 0x33, 0xd5, 0x2f, 0x9a, 0xc5, 
0x20, 0x6f, 0x30, 0xc, 0x30, 0x2, 0x8a, 0xa7, 0xcd, 0x66, 
0xcc, 0xcb, 0xf6, 0xda, 0x58, 0x48, 0x95, 0x1e, 0x43, 0xff, 
0x36, 0x9b, 0x1d, 0x74, 0x13, 0x8d, 0xb1, 0xe2, 0x5e, 0xf5, 
0xc9, 0xb4, 0xd4, 0x25, 0xfc, 0xda, 0x2, 0x22, 0x2, 0xb8, 
0xc0, 0x77, 0x8a, 0xad, 0x14, 0xb8, 0x28, 0x46, 0xb6, 0x7, 
0xd4, 0x6d, 0x58, 0x23, 0x6a};

static void pdcomm_test_driver_start_connect_timer()
{
  /*connect_timer = os_TimerCreate(PDCOMM_TD_CONNECT_TIMER, THREAD_ID_PDCOMM_TD1);*/
  if(pd_comm_ctrl.q_connectInterval != 0)
    os_TimerStart(connect_timer,pd_comm_ctrl.q_connectInterval,0);
}

static void pdcomm_test_driver_start_abort_timer()
{
  /*so that timer is not started multiple times*/
  /*if(abort_timer == NULL) 
  {
	MSG_HIGH("PDCOMM TD: Abort timer started...",0,0,0);
    abort_timer = os_TimerCreate(PDCOMM_TD_ABORT_TIMER,THREAD_ID_PDCOMM_TD1);
    os_TimerStart(abort_timer,10000,0);
  } */
}

static void pdcomm_test_driver_stop_abort_timer()
{
  /*so that timer is not stopped multiple times*/
  /*if(abort_timer != NULL)
  {
	 MSG_HIGH("PDCOMM TD: Abort timer stopped...",0,0,0);
     os_TimerStop(abort_timer);
	 os_TimerDestroy(abort_timer);
  } */
}

static void pdcomm_test_driver_connect()
{
  pd_comms_connect_params_type params;
  pd_comms_return_type result;

  
  disconnect_called = FALSE;
  params.e_link_type = LINK_TCP;
  params.q_transaction_id = transaction_id++;
  
  
  if(pd_comm_ctrl.use_HostName)
  {
    params.z_link_params.z_tcp_params.e_type = PD_COMMS_HOST_NAME;  
    GNSS_STRLCPY(params.z_link_params.z_tcp_params.p_host_name,
                pd_comm_ctrl.SERVER_IP,
                sizeof(params.z_link_params.z_tcp_params.p_host_name));
  }
  else
  {
    params.z_link_params.z_tcp_params.e_type = PD_COMMS_IP_ADDR;    
    params.z_link_params.z_tcp_params.ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
    params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4 = pd_comm_ctrl.SERVER_IP_long;
  }
    
  
  params.z_link_params.z_tcp_params.w_ip_port = pd_comm_ctrl.PORTNUM;
  
  if(pd_comm_ctrl.use_security)
  {
    params.e_use_security = SECURITY_SUPL;  
  }
  else
  {
    params.e_use_security =  0;  
  }
  
  
  
   result = pd_comms_app_connect(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&params);

  if(result != PDCOMM_RESULT_WAIT)
  {
	MSG_ERROR("PDCOMM TD: pdcomm_client_connect returned error: %d",result,0,0);
  }
  else
  {
      cur_state = PDCOMM_STATE_CONNECTING;
	  pdcomm_test_driver_start_abort_timer();
  }
}

#if 0
static void pdcomm_test_driver_write_string()
{
  pd_comms_return_type result;
  pd_comms_write_params_type write_params;

  write_params.q_transaction_id = transaction_id++;

  write_params.w_data_len = strlen(TEST_STRING) + 1;
  write_params.p_data = (uint8*)TEST_STRING;


  result = pd_comms_app_write(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&write_params);
  if(result != PDCOMM_RESULT_WAIT && result != PDCOMM_RESULT_OK)
  {
    MSG_ERROR("PDCOMM TD: pdcomm_client_write returned error: %d",result,0,0);
  }

}
#endif

static void pdcomm_test_driver_write_supl_start()
{
  pd_comms_return_type result;
  pd_comms_write_params_type write_params;

  write_params.q_transaction_id = transaction_id++;


  write_params.w_data_len = sizeof(SUPL_START);
  write_params.p_data = (uint8*)SUPL_START;

  result = pd_comms_app_write(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&write_params);
  if(result != PDCOMM_RESULT_WAIT && result != PDCOMM_RESULT_OK)
  {
    MSG_ERROR("PDCOMM TD: pdcomm_client_write returned error: %d",result,0,0);
  }

}

static void pdcomm_test_driver_write_supl_end()
{
  pd_comms_return_type result;
  pd_comms_write_params_type write_params;

  write_params.q_transaction_id = transaction_id++;


  write_params.w_data_len = sizeof(SUPL_END);
  write_params.p_data = (uint8*)SUPL_END;

  result = pd_comms_app_write(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&write_params);
  if(result != PDCOMM_RESULT_WAIT && result != PDCOMM_RESULT_OK)
  {
    MSG_ERROR("PDCOMM TD: pdcomm_client_write returned error: %d",result,0,0);
  }

}

static void pdcomm_test_driver_ioctl_set_ssl_cert(void)
{
  pd_comms_ioctl_params_type ioctl_params;

  ioctl_params.q_ioctl_request = PDCOMM_IOCTL_CMD_SET_SSL_CERT;
  ioctl_params.q_transaction_id = transaction_id++;
  ioctl_params.p_argument = (uint8*)p_default_cert;
  ioctl_params.q_argument_len = sizeof(p_default_cert);
  
  pd_comms_app_ioctl(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&ioctl_params);  
}

static void pdcomm_test_driver_disconnect(boolean abort)
{
  pd_comms_return_type result;

  result = pd_comms_app_disconnect(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,transaction_id++,abort);
  disconnect_called = TRUE;
  /*cur_state =  PDCOMM_STATE_DISCONNECTING;*/
  if(result != PDCOMM_RESULT_WAIT && result != PDCOMM_RESULT_OK)
  {
    MSG_ERROR("PDCOMM TD: pdcomm_client_disconnect returned error: %d",result,0,0);
  }
}

static void pdcomm_test_driver_force_dormancy(void)
{
  pd_comms_ioctl_params_type ioctl_params;

  ioctl_params.q_ioctl_request = PDCOMM_IOCTL_CMD_FORCE_DORMANCY;
  ioctl_params.q_transaction_id = transaction_id++;
  ioctl_params.p_argument = NULL;
  ioctl_params.q_argument_len = 0;
  
  pd_comms_app_ioctl(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&ioctl_params);     
}

static void pdcomm_test_driver_unforce_dormancy(void)
{
  pd_comms_ioctl_params_type ioctl_params;

  ioctl_params.q_ioctl_request = PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY;
  ioctl_params.q_transaction_id = transaction_id++;
  ioctl_params.p_argument = NULL;
  ioctl_params.q_argument_len = 0;
  
  pd_comms_app_ioctl(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&ioctl_params);     
}

boolean pdcomm_test_driver_mesgIsForceDormAck(os_IpcMsgType *msg)
{
  pd_comms_ipc_ioctl_ack_hdr_type* hdr_ptr;  

  if(msg && (msg->q_MsgId == PDCOMM_TD_CLIENT_MSG_IOCTL_ACK))
  {
      hdr_ptr = (pd_comms_ipc_ioctl_ack_hdr_type*)msg->p_Data;    

      return(hdr_ptr->q_ioctl_request == PDCOMM_IOCTL_CMD_FORCE_DORMANCY);
  }
  else
  {
    return FALSE;
  }
	  
    
}

boolean pdcomm_test_driver_mesgIsSetSSLCertAck(os_IpcMsgType *msg)
{
  pd_comms_ipc_ioctl_ack_hdr_type* hdr_ptr;  

  if(msg && (msg->q_MsgId == PDCOMM_TD_CLIENT_MSG_IOCTL_ACK))
  {
      hdr_ptr = (pd_comms_ipc_ioctl_ack_hdr_type*)msg->p_Data;    

      return(hdr_ptr->q_ioctl_request == PDCOMM_IOCTL_CMD_SET_SSL_CERT);
  }
  else
  {
    return FALSE;
  }
	  
    
}

boolean pdcomm_test_driver_mesgIsUnforceDormAck(os_IpcMsgType *msg)
{
  pd_comms_ipc_ioctl_ack_hdr_type* hdr_ptr;  

  if(msg && (msg->q_MsgId == PDCOMM_TD_CLIENT_MSG_IOCTL_ACK))
  {
      hdr_ptr = (pd_comms_ipc_ioctl_ack_hdr_type*)msg->p_Data;    

      return(hdr_ptr->q_ioctl_request == PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY);
  }
  else
  {
    return FALSE;
  }
	  
    
}

#if 0
static void pdcomm_test_driver_release_handle(void)
{
  pd_comms_return_type result;
  
  result = pd_comms_app_release_handle(&handle);
  
  if(result != PDCOMM_RESULT_OK)
  {
    MSG_ERROR("PDCOMM TD: pdcomm_client_release_handle returned error: %d",result,0,0);
  }
  
}
#endif

static void pdcomm_test_driver_handle_notify(os_IpcMsgType *msg)
{
  pd_comms_ipc_notification_hdr_type* hdr_ptr = 
	                   (pd_comms_ipc_notification_hdr_type*)msg->p_Data;
  
  if(hdr_ptr->e_type == READ_DATA)
  {
        if ((hdr_ptr->z_handle.q_handle_id == handle.q_handle_id)&&
            (hdr_ptr->z_handle.e_link_type == handle.e_link_type))
        {
            pd_comms_read_params_type z_params;
            MSG_HIGH("PDCOMM TD: READ_EVENT from PDCOMM for TCP link. Callng Read()",0,0,0);
            z_params.q_transaction_id = transaction_id++;
            if (read_pending) {
                z_params.w_data_len = msg_len - 2;  
                read_pending = FALSE;
                z_params.u_wait_for_all = TRUE;
            } else {
                z_params.w_data_len = 2; /*SUPL first two bytes has the msg length*/    
                read_pending = TRUE;
                z_params.u_wait_for_all = FALSE;
            }

            /*z_params.w_data_len = 1500;*/
            pd_comms_app_read(handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&z_params); 
        }
        else
        if ((hdr_ptr->z_handle.q_handle_id == z_sms_handle.q_handle_id)&&
            (hdr_ptr->z_handle.e_link_type == z_sms_handle.e_link_type))
        {
            pd_comms_read_params_type z_params;
            z_params.q_transaction_id = transaction_id++;
            
            MSG_HIGH("PDCOMM TD: READ_EVENT from PDCOMM for SMS link. Callng Read() with transaction-Id: %d",
                     transaction_id,0,0);
            
            if (sms_read_pending) {
                z_params.w_data_len = sms_msg_len - 2;  
                sms_read_pending = FALSE;
                z_params.u_wait_for_all = TRUE;
            } else {
                z_params.w_data_len = 2; /*SUPL first two bytes has the msg length*/    
                sms_read_pending = TRUE;
                z_params.u_wait_for_all = FALSE;
            }

            /*z_params.w_data_len = 1500;*/
            pd_comms_app_read(z_sms_handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&z_params); 
        }
  }
  else
  {
    pd_comms_notification_params_type* params = &hdr_ptr->z_params; 
    pd_comms_state_type old_state = cur_state;

    if(hdr_ptr->e_type == ERROR_RPT)
    {
      if(params->q_result == PDCOMM_IOCTL_ERROR_FORCE_DORMANCY_FAILED)
      {
         MSG_HIGH("PDCOMM TD: Force dormancy operation failed",0,0,0); 
         /*pdcomm_test_driver_write_string();*/
          pdcomm_test_driver_write_supl_start(); 
          return;
      }
      else if (params->q_result == PDCOMM_IOCTL_ERROR_UNFORCE_DORMANCY_FAILED)
      {
         MSG_HIGH("PDCOMM TD: UnForce dormancy operation failed",0,0,0); 
         /*pdcomm_test_driver_write_string();*/
          pdcomm_test_driver_write_supl_start(); 
          return;
      }
    }
    
    /*cur_state = params->new_state;*/
/*    if(cur_state != params->e_new_state)*/
    {
    
      switch (params->e_new_state)
      {

      case PDCOMM_STATE_CONNECTED:
        {
          cur_state = params->e_new_state;
    		pdcomm_test_driver_stop_abort_timer();
    	    MSG_HIGH("PDCOMM TD: state CONNECTED while in state %d",old_state,0,0);
          
          if(pdcomm_td_forceDormancy)
          {
            pdcomm_test_driver_force_dormancy();
          }
          else
          {
            /*pdcomm_test_driver_write_string();*/
            pdcomm_test_driver_write_supl_start();    
          }
        
    	  break;
        }
      case PDCOMM_STATE_DISCONNECTED:
        {
            cur_state = params->e_new_state;
    		pdcomm_test_driver_stop_abort_timer();
    	  MSG_HIGH("PDCOMM TD: state DISCONNECTED while in state %d",old_state,0,0);
    	  if(old_state == PDCOMM_STATE_CONNECTING ||
           old_state == PDCOMM_STATE_CONNECTED  ||
           old_state == PDCOMM_STATE_DISCONNECTING)
    	  {
    		if(!disconnect_called)
    		{
             if(!pd_comm_ctrl.test_earlyConnect)   
               pdcomm_test_driver_disconnect(pd_comm_ctrl.test_abort);	
    		}
          
           if(!pd_comm_ctrl.test_earlyConnect)    
             pdcomm_test_driver_start_connect_timer();

    	  }
    	  else
        if(old_state != PDCOMM_STATE_DISCONNECTED)
    	  {
            if(!pd_comm_ctrl.test_earlyConnect)    
              pdcomm_test_driver_start_connect_timer();

    	  }

    	  break;
    	}
      case PDCOMM_STATE_CONNECTING:
    	{
          cur_state = params->e_new_state;
    	  pdcomm_test_driver_start_abort_timer();
    	  MSG_HIGH("PDCOMM TD: state CONNECTING while in state %d",old_state,0,0);
    	  break;
    	}
      case PDCOMM_STATE_DISCONNECTING:
    	{
          cur_state = params->e_new_state;
    	  pdcomm_test_driver_start_abort_timer();
    	  MSG_HIGH("PDCOMM TD: state DISCONNECTING while in state %d",old_state,0,0);
          if(pd_comm_ctrl.test_earlyConnect)    
            pdcomm_test_driver_connect();
    	  break;
    	}
      default:
    	{
    	  MSG_ERROR("PDCOMM TD: Unknown state recvd: %d while in state %d",params->e_new_state,old_state,0);
    	}

      }/*switch (params->new_state) ends*/
    
    }
  }
}

static int pdcomm_test_driver_handle_read_data(os_IpcMsgType *msg)
{
  int i;
  char* p_read_data;

  /*lint -e415 -e416 */   
  pd_comms_ipc_read_data_hdr_type* hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg->p_Data;

  MSG_HIGH("PDCOMM TD: %d bytes received:",hdr_ptr->w_data_len,0,0);
  p_read_data = (char*)hdr_ptr->p_data;

  if(read_pending == TRUE)
  {
    msg_len = (int)p_read_data[1];
    pdcomm_max_data = msg_len;
  }


  
  if(hdr_ptr->w_data_len > 34)
  {
	  MSG_ERROR("PDCOMM TD: recevied invalid string length",0,0,0);
  }
  else
  {
	for(i=0;i<hdr_ptr->w_data_len ;i++)
	{
	  MSG_HIGH("%x",p_read_data[i],0,0);
	}

    if(hdr_ptr->w_data_len == msg_len-2)
    {
      SUPL_END[16] = p_read_data[14];
      SUPL_END[17] = p_read_data[15];
      SUPL_END[18] = p_read_data[16];
      SUPL_END[19] = p_read_data[17];   
    }
    
    
    
  }
 /*lint +e415 +e416 */   
  return hdr_ptr->w_data_len;

}


void v2_mo_sms_init(void)
{
  wms_client_bd_s_type       * bd_ptr;
  wms_client_message_s_type  * cl_ptr;


  bd_ptr = & sms_msg.bds;

  bd_ptr->mask = WMS_MASK_BD_USER_DATA
                 | WMS_MASK_BD_MSG_ID ;

  bd_ptr->message_id.type       = WMS_BD_TYPE_SUBMIT;
  bd_ptr->message_id.id_number  = 0;
  /* id_number should be incremented every time
  ** for debugging purpose, it is mapped to the index of the
  ** moSms structure
  */
  bd_ptr->user_data.encoding    = WMS_ENCODING_OCTET;
  bd_ptr->user_data.number_of_digits =
  wms_ts_pack_ascii("v1_MOSMS_init",
                    (byte*) bd_ptr->user_data.data,
                    (byte*) & bd_ptr->user_data.data_len,
                    (byte*) & bd_ptr->user_data.padding_bits );


  /* TL fields */
  cl_ptr = & sms_msg.msgs;

  cl_ptr->msg_hdr.index = WMS_DUMMY_MESSAGE_INDEX;
  cl_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
  cl_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  cl_ptr->msg_hdr.tag = WMS_TAG_MT_NOT_READ;                      

  /*cl_ptr->u.cdma_message.teleservice         = (wms_teleservice_e_type)65001;*/
  cl_ptr->u.cdma_message.teleservice         =   (wms_teleservice_e_type)WMS_TELESERVICE_CMT_95;
  cl_ptr->u.cdma_message.address.digit_mode  = WMS_DIGIT_MODE_4_BIT;
  cl_ptr->u.cdma_message.address.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
  cl_ptr->u.cdma_message.address.number_type = WMS_NUMBER_NATIONAL;
  cl_ptr->u.cdma_message.address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
  cl_ptr->u.cdma_message.address.number_of_digits =
  wms_ts_ascii2dtmf( "6196515050", (byte*)cl_ptr->u.cdma_message.address.digits );

  cl_ptr->u.cdma_message.subaddress.number_of_digits = 0;

  bd_ptr->reply_option.user_ack_requested     = FALSE;
  bd_ptr->reply_option.delivery_ack_requested = FALSE;

  //cl_ptr->is_tl_ack_requested = TRUE;
  // transport layer ack?
  cl_ptr->u.cdma_message.is_tl_ack_requested = FALSE;

  //cl_ptr->u.cdma_message. is_broadcast = FALSE; 
  
} /* end v2_mo_sms_init() */

static void pdcomm_test_driver_send_sms(void)
{
  pd_comms_return_type result;
  pd_comms_write_params_type write_params;

  memscpy((void*)&write_params.z_tl_write_params.z_sms_write_params.z_address,
          sizeof(write_params.z_tl_write_params.z_sms_write_params.z_address),
          (void*)&z_wms_address, sizeof(z_wms_address));
  write_params.z_tl_write_params.z_sms_write_params.e_teleservice = (wms_teleservice_e_type)WMS_TELESERVICE_CMT_95;
   
  write_params.q_transaction_id = transaction_id++;

  write_params.w_data_len = sizeof(SUPL_START);
  write_params.p_data = (uint8*)SUPL_START;

  MSG_MED("PDCOMM TD: Sending SMS of size %d",sizeof(SUPL_START),0,0);
  result = pd_comms_app_write(z_sms_handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&write_params);
  if(result != PDCOMM_RESULT_WAIT && result != PDCOMM_RESULT_OK)
  {
     MSG_ERROR("PDCOMM TD: pdcomm_client_write returned error: %d",result,0,0);
  }
      

  
  
}

static int pdcomm_test_driver_handle_sms_read_data(os_IpcMsgType *msg)
{
  int i;
  char* p_read_data;
   
  pd_comms_ipc_read_data_hdr_type* hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg->p_Data;

  /*lint -e415 -e416 -e420*/   
  MSG_HIGH("PDCOMM TD: %d bytes received on SMS link",hdr_ptr->w_data_len,0,0);
  p_read_data = (char*)hdr_ptr->p_data;
  
  /*z_wms_address = *((wms_address_s_type*)hdr_ptr->p_data);*/
  if(sms_read_pending == TRUE)
  {
    sms_msg_len = (int)p_read_data[1];
    
  }
  else
  {
    memscpy(&z_wms_address,sizeof(z_wms_address),(void*)p_read_data,hdr_ptr->w_data_len);  
    pdcomm_test_driver_send_sms();
  }
 
  
  if(hdr_ptr->w_data_len > 100)
  {
	  MSG_ERROR("PDCOMM TD: recevied invalid string length for SMS link",0,0,0);
  }
  else
  {
	for(i=0;i<hdr_ptr->w_data_len ;i++)
	{
	  MSG_HIGH("%x",p_read_data[i],0,0);
	}
  }
   
  return hdr_ptr->w_data_len;
  /*lint +e415 +e416 +e420*/   

}

static void pdcomm_test_driver_handle_write_ack(os_IpcMsgType *msg)
{
pd_comms_ipc_write_ack_hdr_type* hdr_ptr = (pd_comms_ipc_write_ack_hdr_type*)msg->p_Data;

MSG_HIGH("PDCOMM TD: write ack received for %d bytes",hdr_ptr->q_bytes_written,0,0);

}

static void pdcomm_test_driver_handle_sms_ack_report(os_IpcMsgType *msg)
{
pd_comms_ipc_sms_ack_report_hdr_type* hdr_ptr = (pd_comms_ipc_sms_ack_report_hdr_type*)msg->p_Data;

MSG_HIGH("PDCOMM TD: SMS ACK report received: sucess=%d transaction-id=%d",
         hdr_ptr->u_success,hdr_ptr->q_transaction_id,0);    
}

static void pdcomm_test_driver_proc(os_IpcMsgType *msg)
{

  switch (msg->q_MsgId)
  {

  case PDCOMM_TD_CLIENT_MSG_NOTIFY:
    {
	  pdcomm_test_driver_handle_notify(msg);
	  break;
    }
  case PDCOMM_TD_CLIENT_MSG_HANDLE:
	{
	  pd_comms_ipc_handle_hdr_type* hdr_ptr = (pd_comms_ipc_handle_hdr_type*)msg->p_Data;
	  handle = hdr_ptr->z_handle;
	  MSG_HIGH("PDCOMM TD: handle recvd. Handle_id %d",handle.q_handle_id,0,0);

	  break;
	}
  case PDCOMM_TD_CLIENT_MSG_READ_DATA:
   {
	  pd_comms_ipc_read_data_hdr_type* hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg->p_Data;
      
      if( (hdr_ptr->z_handle.q_handle_id == handle.q_handle_id) &&
          (hdr_ptr->z_handle.e_link_type == handle.e_link_type))
      {
        bytes_read += pdcomm_test_driver_handle_read_data(msg);

        if(bytes_read >= pdcomm_max_data)
        {
          pdcomm_test_driver_write_supl_end();  
    	  bytes_read =0; 
          cur_state = PDCOMM_STATE_DISCONNECTING;
    	  pdcomm_test_driver_start_abort_timer();
    	  pdcomm_test_driver_disconnect(pd_comm_ctrl.test_abort);
    	}
      }
      else
      if((hdr_ptr->z_handle.q_handle_id == z_sms_handle.q_handle_id)&&
         (hdr_ptr->z_handle.e_link_type == z_sms_handle.e_link_type))
      {
        pdcomm_test_driver_handle_sms_read_data(msg);
      }

	  break;
	}
  case PDCOMM_TD_CLIENT_MSG_WRITE_ACK:
	{
	  pdcomm_test_driver_handle_write_ack(msg);
	  break;
	}
  case PDCOMM_TD_CLIENT_MSG_IOCTL_ACK:
    {
      MSG_MED("PDCOMM TD: Received IOCTL ACK",0,0,0);
      
      if(pdcomm_td_forceDormancy)
      {
        if(pdcomm_test_driver_mesgIsForceDormAck(msg))
        {
           MSG_HIGH("PDCOMM TD: FORCE DORMANCY successfull. Unforcing Dormancy",0,0,0); 
           pdcomm_test_driver_unforce_dormancy();
        }
        else
        if(pdcomm_test_driver_mesgIsUnforceDormAck(msg))
        {
           MSG_HIGH("PDCOMM TD: UNFORCE DORMANCY successfull.",0,0,0);  
           /*pdcomm_test_driver_write_string();*/
           pdcomm_test_driver_write_supl_start();    
        }
        else
        if (pdcomm_test_driver_mesgIsSetSSLCertAck(msg))
        {
          MSG_HIGH("PDCOMM TD: SET SSL Certificate successful.",0,0,0);  
          return;    
        }
      }
      else
      {
          /*Do nothing*/
      }
      
      break;  
    }
  case PDCOMM_TD_CLIENT_MSG_SMS_ACK_REPORT:
      {
          pdcomm_test_driver_handle_sms_ack_report(msg); 
          break;
      }
  case C_OS_MSG_ID_TIMER_EXPIRY:
	{
	  
	  if(*((uint32*)msg->p_Data) == PDCOMM_TD_CONNECT_TIMER)
	  {
		/*os_TimerDestroy(connect_timer);*/
	    pdcomm_test_driver_connect();
	  }
	  else
	  if(*((uint32*)msg->p_Data) == PDCOMM_TD_ABORT_TIMER)
	  {
	    cur_state = PDCOMM_STATE_DISCONNECTING;
		MSG_ERROR("PDCOMM TD: ABORT timer expired...",0,0,0);
	    /*os_TimerDestroy(abort_timer);*/
	    pdcomm_test_driver_disconnect(TRUE);/*abort connection*/
	  }
	  else
	  {
		MSG_ERROR("PDCOMM TD: Invalid timer id %d in timeout message",*((uint32*)msg->p_Data),0,0);
	  }
	  break;
	}
  default:
	{
	  MSG_ERROR("PDCOMM TD: Unknown Message recvd: %d",msg->q_MsgId,0,0);
	}

  }/*switch (msg->q_MsgId) ends*/


}

boolean pdcomm_test_driver_get_handle()
{
  pd_comms_get_handle_params_type z_getHandleParams;

  z_getHandleParams.e_link_type = LINK_TCP;
  z_getHandleParams.q_base_msg_id = PDCOMM_TD_LOCAL_IPC_MSG_ID_END;

  if(pd_comms_app_get_handle(&z_getHandleParams,&handle) != PDCOMM_RESULT_OK)
  {
     MSG_ERROR("PDCOMM TD: handle allocation failed",0,0,0);
     return FALSE;
  }

  return TRUE;
}


boolean pdcomm_test_driver_parse_sms_cb(const wms_client_message_s_type *p_data)
{
   MSG_HIGH("PDCOMM TD: SMS PARSE CALLBACK CALLED",0,0,0); 
   return TRUE;
}

boolean pdcomm_test_driver_verify_sms_push_cb(
   wms_mt_message_info_s_type *p_data,
   wms_client_ts_data_s_type *p_ts_data,
   uint8 *p_MesgContent,
   uint16 *p_MesgLen,
   uint16 w_MesgContentMaxLen,
   boolean u_isWapPush)
{
  MSG_HIGH("PDCOMM TD: SMS  read message CALLBACK CALLED",0,0,0);
  *p_MesgLen = sizeof(wms_address_s_type)+2;
  *((uint16*)p_MesgContent) = sizeof(wms_address_s_type);
  memscpy(p_MesgContent+2, *p_MesgLen, &p_data->message.u.cdma_message.address,*p_MesgLen);;
      
  return TRUE;
}

void pdcomm_test_driver_test_hash()
{
  pd_comms_generate_hash_params_type p_hash_params;
  pd_comms_return_type retval;
  os_IpcMsgType* msg;
  pd_comms_ipc_hash_resp_hdr_type* p_hash_resp;
  
  p_hash_params.e_hash_type = SHA1;
  p_hash_params.q_transaction_id = 0;
  p_hash_params.p_msg = (uint8*)SUPL_START;
  p_hash_params.q_msg_len = sizeof(SUPL_START);
  p_hash_params.p_key_ptr = (uint8*)pd_comm_ctrl.SERVER_IP;
  p_hash_params.w_key_len = strlen(pd_comm_ctrl.SERVER_IP);
  retval = pd_comms_app_generate_hash(THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&p_hash_params);

  if(retval != PDCOMM_RESULT_WAIT)
  {
    MSG_ERROR("PDCOMM TD: pd_comms_app_generate_hash returned error %d",retval,0,0);
    return;
  }

  msg = os_IpcReceive();

  if(msg == NULL)
  {
    MSG_ERROR("PDCOMM TD: NULL message received while waiting for generate_has response",0,0,0);
    return;
  }
  else
  {
    if(msg->q_MsgId == PDCOMM_TD_CLIENT_MSG_HASH_RESP)
    {
      p_hash_resp =  (pd_comms_ipc_hash_resp_hdr_type*)msg->p_Data; 
      MSG_MED("PDCOMM TD: Hash response received with hash_ouput_len = %d",p_hash_resp->q_output_len,0,0);  
    }
    else
    {
      MSG_ERROR("PDCOMM TD: Unexpected message %d received while waiting for hash reponse",msg->q_MsgId,0,0);  
    }
      
  }
  
  os_IpcDelete(msg);
  
     
}

void pdcomm_test_driver_test_sms_link()
{
   
   pd_comms_get_handle_params_type z_getHandleParams;
   pd_comms_connect_params_type z_params;
   pd_comms_return_type result;
   os_IpcMsgType* msg;
   pd_comms_ipc_notification_hdr_type* hdr_ptr; 
   
   z_getHandleParams.e_link_type = LINK_SMS;
   z_getHandleParams.q_base_msg_id = PDCOMM_TD_LOCAL_IPC_MSG_ID_END;

   if(pd_comms_app_get_handle(&z_getHandleParams,&z_sms_handle) != PDCOMM_RESULT_OK)
   {
      MSG_ERROR("PDCOMM TD: SMS handle allocation failed",0,0,0);
      
   }

   z_params.e_link_type = LINK_SMS;
   z_params.q_transaction_id =0;
   z_params.z_link_params.z_sms_params.p_read_msg_cb_func = pdcomm_test_driver_verify_sms_push_cb; 
   z_params.z_link_params.z_sms_params.p_parse_cb_func = pdcomm_test_driver_parse_sms_cb;
   
   result = pd_comms_app_connect(z_sms_handle,THREAD_ID_PDCOMM_TD1,PDCOMM_TD_LOCAL_IPC_MSG_ID_END,&z_params);

   if(result != PDCOMM_RESULT_WAIT)
   {
	 MSG_ERROR("PDCOMM TD: pdcomm_client_connect returned error: %d",result,0,0);
     return;
   }

   msg = os_IpcReceive();

   if(msg == NULL)
   {
     MSG_ERROR("PDCOMM TD: NULL message received",0,0,0);
     return;
   }
   else
   {
       
     if(msg->q_MsgId == PDCOMM_TD_CLIENT_MSG_NOTIFY)
     {
       hdr_ptr =(pd_comms_ipc_notification_hdr_type*)msg->p_Data;
       if(hdr_ptr->z_params.e_new_state != PDCOMM_STATE_CONNECTED)
       {
          MSG_ERROR("PDCOMM TD: FAILED SMS Link connection",0,0,0);
          os_IpcDelete(msg);
          return;
       }
       else
       {
          MSG_HIGH("PDCOMM TD: SMS Link Connected",0,0,0); 
       }
       
     }
     else
     {
       MSG_ERROR("PDCOMM TD: FAILED SMS Link connection",0,0,0);
       os_IpcDelete(msg);
       return;    
     }
        
     os_IpcDelete(msg);
   }
   
}

void pdcomm_test_driver_init_ctrl(void)
{
pd_comm_ctrl.PORTNUM = 59910;
pd_comm_ctrl.use_HostName = FALSE;
GNSS_STRLCPY(pd_comm_ctrl.SERVER_IP,
            "mrawal.qualcomm.com",
             sizeof(pd_comm_ctrl.SERVER_IP));
pd_comm_ctrl.SERVER_IP_long = 0x812EE14B;
pd_comm_ctrl.use_security = FALSE;
pd_comm_ctrl.q_connectInterval = 7000;
pd_comm_ctrl.test_earlyConnect = FALSE;
pd_comm_ctrl.test_abort = FALSE;
pd_comm_ctrl.test_hash = FALSE;
     
}

void pdcomm_test_driver_task(dword dummy)
{
  int i;

  os_IpcMsgType* msg;

  os_IpcStart(THREAD_ID_PDCOMM_TD1);

  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();

  connect_timer = os_TimerCreate(PDCOMM_TD_CONNECT_TIMER, THREAD_ID_PDCOMM_TD1);

  /*task_start(0,0,0);*/

  /*dog_timer->q_MsgId = PDCOMM_TD_DOG_TIMER;*/
  /*pdcomm_test_driver_handle_dog_timer();*/

  for(i=0;i<199;i++)
  {
	TEST_STRING[i]='A';
  }

  TEST_STRING[i]='\0';

  
  pdcomm_test_driver_test_sms_link(); 
  v2_mo_sms_init();

  pdcomm_test_driver_init_ctrl();
  
  if(pd_comm_ctrl.test_hash)
  {
    pdcomm_test_driver_test_hash();
  }
  
  if(!pdcomm_test_driver_get_handle())
  {
    MSG_ERROR("PDCOMM TD: Test driver failed getting handle from PDCOMM APP",0,0,0);
  }
  else
  {
    pdcomm_test_driver_start_connect_timer();  
  }

  /*set the root ssl cert*/
  pdcomm_test_driver_ioctl_set_ssl_cert();
  /*pdcomm_test_driver_connect();*/

  while(1)
  {

	msg = os_IpcReceive();

    if(msg == NULL)
    {
	  MSG_ERROR("PDCOMM TD: NULL message received",0,0,0);
	  //break;
	}
	else
	{
	  pdcomm_test_driver_proc(msg);
	  os_IpcDelete(msg);
	}


  }/*while(1) ends*/


}
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */
#endif
