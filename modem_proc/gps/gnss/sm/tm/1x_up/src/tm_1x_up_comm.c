/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   TM 1X UP module interface with PDCOMM.


GENERAL DESCRIPTION
  This module contains the PDCOMM Interface block.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Confidential and Proprietary - Qualcomm Technologies, Inc. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/src/tm_1x_up_comm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/15    rh      Added support for MultiSIM NV  
05/13/14    skm     tm_1x_up_comm_send Interger & buffer overflow check
05/27/07    rw      Modified to support JGPS module
12/22/06    ank     Added support for MSBased Throttling feature.
12/14/06    ank     Changes to use IP addr and Port from NV for default option.
11/15/06    ank     Removed tm_1x_up_comm_create_custom_header.
                    Added wrapper for Force_Dormancy, Unforce_Dormancy
10/25/06    ank     Initial Release
===========================================================================*/

/*==========================================================================

                   Include Files

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stdlib.h>

#include "msg.h"
#include "event.h"
#include "aries_os_globals.h"
#include "tm_data.h"
#include "tm_common.h"
#include "tm_1x_up.h"
#include "tm_1x_up_comm.h"
#include "tm_1x_up_is801_driver_i.h"
#include "paixlate.h"
#include "tm_is801_util.h"
#include "tm_1x_up_smod_iface.h"
#include "tm_1x_up_is801_drv_iface.h"
#include "tm_umts_up_supl.h"
#include "tm_umts_up_supl_comm.h"


tm_1x_up_pdcomm_info_s_type   pdcomm_info;        /* Transport layer information */
extern pdsm_pd_info_s_type tm_1x_up_info_buffer; 
LOCAL pd_comms_connect_params_type connect_param;

#ifndef FEATURE_CGPS_UMTS_UP_SUPL
void tm_umts_up_supl_start_slp_contact(void)
{
  return;
}
boolean tm_umts_up_supl_SuplPos_is801_tx (uint8 *p_is801_data, const uint16 w_byte_cnt)
{
  return TRUE;
}
void tm_umts_up_supl_is801_nt_assist_done (boolean sess_end_sent)
{
  return;
}
boolean  tm_umts_up_supl_comm_app_ioctl(pd_comms_ioctl_cmd_type ioctl_cmd)
{
  return TRUE;
}  
pd_comms_state_type tm_umts_up_supl_comm_get_tcp_state(void)
{
  return PDCOMM_STATE_DISCONNECTED;
}  
#endif /* ifndef FEATURE_CGPS_USES_UMTS: dummy function for builds without UMTS-SUPL */


/*===========================================================================

FUNCTION tm_1x_up_comm_get_next_transaction_id

DESCRIPTION
  This function returns next transaction Id.
  
DEPENDENCIES
  None.

RETURN VALUE
  transaction id

SIDE EFFECTS
  None.

===========================================================================*/
uint32 tm_1x_up_comm_get_next_transaction_id(void)
{
  if( (pdcomm_info.tr_id_info.curr_id < pdcomm_info.tr_id_info.base)
        ||(pdcomm_info.tr_id_info.curr_id >= (pdcomm_info.tr_id_info.base + pdcomm_info.tr_id_info.range)))
  {
    pdcomm_info.tr_id_info.curr_id = pdcomm_info.tr_id_info.base;
    MSG_MED("Transaction id restart at base 0x%x",pdcomm_info.tr_id_info.curr_id,0,0);
  }
  else
  {
    MSG_MED("Current transaction id 0x%x",pdcomm_info.tr_id_info.curr_id,0,0);
  }
  return pdcomm_info.tr_id_info.curr_id++;
}

/*===========================================================================
FUNCTION tm_1x_up_comm_get_pde_address

DESCRIPTION
  Obtains PDE IP address

RETURN VALUE
  PDE IP Addr

DEPENDENCIES

===========================================================================*/
dword tm_1x_up_comm_get_pde_address( void )
{
  if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_option == PDSM_SERVER_OPTION_USE_DEFAULT)
  {
    /* Use PDE server address stored in NV */
    return(tm_1x_up_config.pde_adrs);
  }
  else
  {
    return(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs);
  }
}

/*===========================================================================
FUNCTION tm_1x_up_comm_get_pde_port_id

DESCRIPTION
  Obtains PDE port id

RETURN VALUE
  PDE Port Id

DEPENDENCIES

===========================================================================*/
dword tm_1x_up_comm_get_pde_port_id( void )
{
  if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_option == PDSM_SERVER_OPTION_USE_DEFAULT)
  {
    /* Use Port stored in NV */
    return(tm_1x_up_config.port_id);
  }
  else
  {
    return(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id);
  }
}


/*===========================================================================
FUNCTION tm_1x_up_comm_get_pde_url_addr

DESCRIPTION
  Obtains PDE URL

RETURN VALUE
  True/False

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_comm_get_pde_url_addr( void )
{
  char port_string[6];
  uint16 i,j;

  /* url format is "www.yahoo.com:4911", 
  */
  if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_option == PDSM_SERVER_OPTION_USE_DEFAULT)
  {
    MSG_MED("Default server option", 0, 0, 0);
    i=0;
    while ( (tm_1x_up_config.pde_addr_url[i] != ':') && 
            (tm_1x_up_config.pde_addr_url[i] != '\0') && 
            ( i < ( TM_PDE_MAX_URL_SIZE - 1 ) ) )
    {
      connect_param.z_link_params.z_tcp_params.p_host_name[i] = tm_1x_up_config.pde_addr_url[i] ;
      i++;
    }
    connect_param.z_link_params.z_tcp_params.p_host_name[i] = '\0';

    i++;
    if ( i >= TM_PDE_MAX_URL_SIZE )
    { 
      MSG_ERROR("PDE URL addr length exceeds maximum", 0, 0, 0);
      return FALSE;
    }
    else
    {
      j = i;
      while ( (tm_1x_up_config.pde_addr_url[i] != '\0') && 
              ( i < ( TM_PDE_MAX_URL_SIZE - 1 ) ) )
      {
        if(((i-j) >= 0) &&
              ((i-j) < 5))
        {
          port_string[i-j] = tm_1x_up_config.pde_addr_url[i];
          i++;
        }
        else
        {
          MSG_ERROR("Got malicious port number", 0, 0, 0);
          return FALSE;
        }
      }

      if(((i-j) > 0) &&
         ((i-j) < 6))
      {
      port_string[i-j] = '\0';  /* null terminated string */
      }

      connect_param.z_link_params.z_tcp_params.w_ip_port = (uint16) atoi(port_string);
      if ( connect_param.z_link_params.z_tcp_params.w_ip_port == 0 )
      {
        MSG_ERROR("Wrong PDE port", 0, 0, 0);
        return FALSE;
      }
      MSG_MED("PDE port is decoded as %d", connect_param.z_link_params.z_tcp_params.w_ip_port, 0, 0);
    }
  }
  else /* Local */
  {
      MSG_MED("Local server option", 0, 0, 0);
      i=0;
      while ( (tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url[i] != ':') && 
              (tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url[i] != '\0') && 
              ( i < ( TM_PDE_MAX_URL_SIZE - 1 ) ) )
      {
        connect_param.z_link_params.z_tcp_params.p_host_name[i] = tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url[i] ;
        i++;
      }
      connect_param.z_link_params.z_tcp_params.p_host_name[i] = '\0';

      i++;
      if ( i >= TM_PDE_MAX_URL_SIZE )
      { 
        MSG_ERROR("PDE URL addr length exceeds maximum", 0, 0, 0);
        return FALSE;
      }
      else
      {
        j = i;
        while ( (tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url[i] != '\0') && 
                ( i < ( TM_PDE_MAX_URL_SIZE - 1 ) ))
        {
          if(((i-j) >= 0) &&
                ((i-j) < 5))
          {
            port_string[i-j] = tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_url.url[i];
            i++;
          }
          else
          {
            MSG_ERROR("Got malicious port number", 0, 0, 0);
            return FALSE;
          }
        }

        if(((i-j) > 0) &&
           ((i-j) < 6))
        {
        port_string[i-j] = '\0';  /* null terminated string */
        }

        connect_param.z_link_params.z_tcp_params.w_ip_port = (uint16) atoi(port_string);
        if ( connect_param.z_link_params.z_tcp_params.w_ip_port == 0 )
        {
          MSG_ERROR("Wrong PDE port", 0, 0, 0);
          return FALSE;
        }
        MSG_MED("PDE port is decoded as %d", connect_param.z_link_params.z_tcp_params.w_ip_port, 0, 0);
      }
  }
  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_up_comm_connect

DESCRIPTION
  This function is used connect to PDE.

RETURN VALUE
  boolean

DEPENDENCIES
  Called by registered clients.

===========================================================================*/
boolean tm_1x_up_comm_connect( void )
{
  pd_comms_return_type ret_val = FALSE;

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) != 0)
  {
#if 0
    pd_comms_return_type ret;

    ret = tm_umts_up_supl_comm_tcp_connect();

    if( ret == PDCOMM_RESULT_OK)
    {
      /*Got connected*/
      tm_umts_up_supl_tcp_connect_notif();
    }
    else if( ret > PDCOMM_RESULT_WAIT)
    {
      MSG_HIGH("Supl failed to open connection to PDE",0,0,0);
      return FALSE;
    }
    return TRUE;
#endif

    tm_umts_up_supl_start_slp_contact();
    return TRUE;
  }

  /* Fire COMM_BEGIN event */
  tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
  tm_1x_up_info_buffer.pd_info.comm_data.protocol_type = PDSM_PD_COMM_PROTOCOL_1X;
  (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                          tm_1x_up_session_info.sess_handle,
                          PDSM_PD_EVENT_COMM_BEGIN,
                          &tm_1x_up_info_buffer,
                          NULL);

  if(!tm_1x_up_comm_get_handle())
  {
    MSG_ERROR("Can't get handle!",0,0,0);
    return FALSE;
  }

  memset((void *)&connect_param, 0, sizeof(pd_comms_connect_params_type));
  connect_param.e_link_type = LINK_TCP;
  connect_param.e_use_security =  SECURITY_NONE;
  pdcomm_info.connect_tr_id = tm_1x_up_comm_get_next_transaction_id();
  connect_param.q_transaction_id = pdcomm_info.connect_tr_id;

  /* Default: If both NV items for URL and IP addr are populated, choose IP addr */
  if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_option == PDSM_SERVER_OPTION_USE_DEFAULT)
  {
    if(tm_1x_up_config.pde_adrs)
    {
      connect_param.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4 = tm_1x_up_config.pde_adrs;
      connect_param.z_link_params.z_tcp_params.ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
      connect_param.z_link_params.z_tcp_params.w_ip_port = tm_1x_up_config.port_id;
    }
    else if(tm_1x_up_config.pde_addr_url[0] != '\0')
    {
      if(tm_1x_up_comm_get_pde_url_addr() == FALSE)
      {
        MSG_ERROR("Can not get PDE URL addr",0,0,0);
        return FALSE;
      }
    }
    else
    {
      MSG_ERROR("Neither IP nor URL addr is available",0,0,0);
      return FALSE;
    }
  }
  else if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_option == PDSM_SERVER_OPTION_USE_LOCAL)
  {
    if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS)
    {
      connect_param.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4 = tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs;  
      connect_param.z_link_params.z_tcp_params.ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
      connect_param.z_link_params.z_tcp_params.w_ip_port = tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id;
    }
    else if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS)
    {
      if(tm_1x_up_comm_get_pde_url_addr() == FALSE)
      {
        MSG_ERROR("Can not get PDE URL addr",0,0,0);
        return FALSE;
      }
    }
    else
    {
      MSG_ERROR("Incorrect server addr_type",0,0,0);
      return FALSE;
    }
  }
  else
  {
    MSG_ERROR("Incorrect server option",0,0,0);
    return FALSE;
  }

  /*
   *  IF a non-zero IP Address is provided THEN
   *      provide ONLY an IP Address to PDCOMMS
   *  ELSE IF a non-NULL hostname has been provided THEN
   *    provide the hostname to PDCOMMS
   *  ELSE
   *    return to the caller with an error
   *  ENDIF
   */
  if( connect_param.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4 )
  {
    connect_param.z_link_params.z_tcp_params.e_type = PD_COMMS_IP_ADDR;
  }
  else if( connect_param.z_link_params.z_tcp_params.p_host_name[0] != '\0' )
  {
    connect_param.z_link_params.z_tcp_params.e_type = PD_COMMS_HOST_NAME;
  }
  else
  {
    MSG_ERROR("Neither IP nor URL addr is available",0,0,0);
    return FALSE;
  }

  /*Fill in the APN profile type to used*/
  connect_param.z_link_params.z_tcp_params.profile_type = PDCOMM_APN_PROFILE_TYPE_LBS;

  ret_val = pd_comms_app_connect(pdcomm_info.handle,
                                 (uint32)THREAD_ID_SM_TM,
                                 TM_CORE_PDCOMM_MSG_ID_BASE,
                                 &connect_param);

  if(ret_val != PDCOMM_RESULT_WAIT)
  {
    MSG_ERROR("pdcomm_client_connect returned error: %d",ret_val,0,0);
    return FALSE;
  }

  pdcomm_info.curr_state = PDCOMM_STATE_CONNECTING;
  pdcomm_info.cust_hdr_sent = FALSE;
  MSG_HIGH("pdcomm_client_connect returned: %d",ret_val,0,0);
  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_up_comm_get_handle

DESCRIPTION
  This function is used to get a new pdcomm handle. For V1/V2, connection to MPC and connection to
  PDE will be indetified by separate handles.

RETURN VALUE
  boolean

DEPENDENCIES
  Called by registered clients.

===========================================================================*/
boolean tm_1x_up_comm_get_handle( void )
{
  pd_comms_get_handle_params_type get_param;
  pd_comms_return_type ret_val;

  /* Make sure we have valid pdcomm handle first */
  if(pdcomm_info.handle_valid == FALSE)
  {
    get_param.e_link_type = LINK_TCP;
    get_param.q_base_msg_id = TM_CORE_PDCOMM_MSG_ID_BASE;
          
    ret_val = pd_comms_app_get_handle(&get_param,&(pdcomm_info.handle));
          
    if(ret_val != PDCOMM_RESULT_OK)
    {
      MSG_ERROR("pd_comms_app_get_handle returned error: %d",ret_val,0,0);
      return FALSE;
    }
    else
    {
      pdcomm_info.handle_valid = TRUE;
      MSG_HIGH("pd_comms_app_get_handle returned handle: %d",pdcomm_info.handle.q_handle_id,0,0);
      return TRUE; 
    }
  }
  return TRUE; 
}


/*===========================================================================

FUNCTION tm_1x_up_comm_write

DESCRIPTION
  This function is used to write data to socket.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_comm_write
(
    byte *data, 
    uint16 size
)
{
  boolean ret_val = TRUE;
  pd_comms_return_type comm_ret_val;
  pd_comms_write_params_type write_params;

  /* Check input parameters */
  if( data == NULL || size == 0 )
  {
    MSG_ERROR("Invalid error: Data is null or size (%d)", size, 0,0);
    return FALSE;
  }

  /*Since IS801 is working under SUPL context, send the data to SUPL, 
  which will include it in its SUPL_POS message*/
  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) !=0 )
  {
  	return tm_umts_up_supl_SuplPos_is801_tx(data, size);
  }

  /* Only send if the comm state is connected*/
  if ( tm_1x_up_comm_get_state() == PDCOMM_STATE_CONNECTED )
  { 
    write_params.q_transaction_id = tm_1x_up_comm_get_next_transaction_id();
    write_params.w_data_len = size;
    write_params.p_data = (uint8*)data;
  
    comm_ret_val = pd_comms_app_write(pdcomm_info.handle,
                                 (uint32)THREAD_ID_SM_TM,
                                 TM_CORE_PDCOMM_MSG_ID_BASE,
                                 &write_params);
  
    if(comm_ret_val != PDCOMM_RESULT_WAIT && comm_ret_val != PDCOMM_RESULT_OK)
    {
      MSG_ERROR("PDCOMM TD: pdcomm_client_write returned error: %d",comm_ret_val,0,0);
      ret_val = FALSE;
    }
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_comm_send

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_1x_up_comm_send
(
    byte* start,
    uint16 length,
    uint8  pddm_type
)
{
  word                cust_hdr_size = 0;

  /* Check to make sure the pdcomm_info.buffer to be enqueued is at least a header */
  if (length < PDDM_HDR_LEN)
  {
    MSG_ERROR( "tm_1x_up_comm_send:  Error in length passed in.", 0, 0, 0);
    return FALSE;
  }

  if (start == NULL)
  {
    MSG_ERROR( "tm_1x_up_comm_send:  Null pointer passed in.", 0, 0, 0);
    return FALSE;
  }

  pdcomm_info.last_pddm_sent  = start;
  pdcomm_info.last_pddm_len   = length;
  pdcomm_info.last_pddm_type  = pddm_type;

  /* Store the pdcomm_info.buffer in the output queue and set the parameters for it */
  if (FALSE == pdcomm_info.cust_hdr_sent
      /*If IS801 is working under SUPL context, then custom hdr is not used*/
      && (TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) == 0)
     )
  {
    cust_hdr_size = tm_1x_up_create_custom_header(&(pdcomm_info.buffer.data[MSID_HDR_START]));

    if (0 == cust_hdr_size)
    {
      MSG_ERROR( "tm_1x_up_comm_send:  Failure making custom header .", 0, 0, 0);
      /* failure making custom header */
      return FALSE;
    }
    /*Make sure integer overflow does not occur when calculating num_bytes_to_write*/
    if ((length >= TM_MAX_BUFFSIZE) || (cust_hdr_size >= TM_MAX_BUFFSIZE))
    {
      MSG_ERROR( "tm_1x_up_comm_send: length:%d cust_hdr_size: %d, exceeds TM_MAX_BUFFSIZE.",length, cust_hdr_size, 0);
      return FALSE;
    }


    /* length of custom header + reserved field */
    pdcomm_info.buffer.data[0] = cust_hdr_size + RESERVED_SIZE;  
    /* reserved value (0x00) */
    pdcomm_info.buffer.data[1] = TM_1X_UP_CUSTOM_IP_RESERVED_VAL;
    if (TM_1X_UP_COMM_DATA_SIZE <= MSID_HDR_START + cust_hdr_size)
    {
      MSG_ERROR( "tm_1x_up_comm_send:  custom header too long %d .", cust_hdr_size, 0, 0);
      /* failure making custom header */
      return FALSE;
    }
    pdcomm_info.buffer.data[MSID_HDR_START+cust_hdr_size] = length;    /* length of payload */
    pdcomm_info.buffer.cust_hdr_size = MSID_HDR_START + cust_hdr_size + LENGTH_HDR_SIZE;   /* header size, used for logging */

    pdcomm_info.buffer.num_bytes_to_write = MSID_HDR_START + cust_hdr_size + LENGTH_HDR_SIZE + length; /* overall packet size */

    if ((pdcomm_info.buffer.cust_hdr_size < TM_MAX_BUFFSIZE) &&
        (length < (TM_MAX_BUFFSIZE - pdcomm_info.buffer.cust_hdr_size)))
    {
	  	/*lint -e{669} lint is treating arg 1 as char[1] */
	  memscpy((void*)&(pdcomm_info.buffer.data[MSID_HDR_START + cust_hdr_size + LENGTH_HDR_SIZE]),
	           sizeof(pdcomm_info.buffer.data) - pdcomm_info.buffer.cust_hdr_size, (void*)start, length);
    }
	else
	{
	  MSG_ERROR(" Overall packet size cannot exceed TM_MAX_BUFFSIZE", 0, 0, 0);
	  return FALSE;
	}
	
    MSG_HIGH( "tm_1x_up_comm_send:  Sending %d bytes with custom header", pdcomm_info.buffer.num_bytes_to_write, 0, 0);
    pdcomm_info.cust_hdr_sent = TRUE;
  }
  else
  {
    uint16 max_data_size =0;
    uint16 offset = 0;
    MSG_HIGH( "tm_1x_up_comm_send:  Sending %d bytes", pdcomm_info.buffer.num_bytes_to_write, 0, 0);

		/*These might get reset below if working under SUPL context*/
    pdcomm_info.buffer.data[0] = length;
    pdcomm_info.buffer.num_bytes_to_write = LENGTH_HDR_SIZE + length;
    pdcomm_info.buffer.cust_hdr_size = LENGTH_HDR_SIZE;
    offset = LENGTH_HDR_SIZE;

		max_data_size = TM_1X_UP_COMM_DATA_SIZE;
    if( TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) != 0 )
    {
      pdcomm_info.buffer.num_bytes_to_write = length;
      pdcomm_info.buffer.cust_hdr_size = 0;
      offset = 0;
    }
		
    if(length < max_data_size)
    {
      memscpy((void*)&(pdcomm_info.buffer.data[offset]), sizeof (pdcomm_info.buffer.data) - offset, (const void*)start, length);
    }
    else
    {
      MSG_ERROR(" Overall packet size should be less than length = %d", length, 0, 0);
	  return FALSE;
    }
  }

  pdcomm_info.buffer.num_bytes_written = 0;

  /*-----------------------------------------------------------------------------------
    Call 1x UP sub-module interface to send data, else 1x UP (IS-801 Driver will
    send the data itself
  -----------------------------------------------------------------------------------*/   
  if( TM_1X_UP_SMOD_DATA_PIPE_REQ_FAIL ==
          tm_1x_up_smod_data_pipe_req(FALSE, /* do NOT close the connection */
                                      TM_1X_UP_SMOD_DATA_CMD_IS801_RLINK,
                                      pdcomm_info.buffer.data, 
                                      pdcomm_info.buffer.num_bytes_to_write) 
    )
  {
    return FALSE;
  }

  return TRUE;
} /* tm_1x_up_comm_send() */

/*===========================================================================

FUNCTION tm_1x_up_comm_disconnect

DESCRIPTION
  This function is used to close phyiscal path with PDE.

RETURN VALUE
  pd_comms_return_type

DEPENDENCIES

===========================================================================*/
pd_comms_return_type tm_1x_up_comm_disconnect(boolean abrt )
{
  pd_comms_return_type ret_val;

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) !=0 )
  {
    /*Since IS801 is working under SUPL context, do not close the connection 
    	but let SUPL know that IS801 is done using the network assistance*/
    tm_umts_up_supl_is801_nt_assist_done(tm_1x_up_session_info.sess_end_sent);

    /*Stop the disconnect timer, as if connection is already diconnected 
    	and reset the PDCOMM relared info*/
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_disconnect_timer);
    tm_1x_up_comm_reset();
    return PDCOMM_RESULT_OK;
  }
  pdcomm_info.disconnect_tr_id = tm_1x_up_comm_get_next_transaction_id();
  ret_val = pd_comms_app_disconnect(pdcomm_info.handle,
                                    (uint32)THREAD_ID_SM_TM,
                                    TM_CORE_PDCOMM_MSG_ID_BASE,
                                    pdcomm_info.disconnect_tr_id,
                                    abrt);
  if(ret_val == PDCOMM_RESULT_OK)
  {
    MSG_MED("TM 1X UP disconnected", 0, 0, 0);
    pdcomm_info.curr_state = PDCOMM_STATE_DISCONNECTED; 
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_disconnect_timer);
    tm_1x_up_comm_reset();
  }
  else if(ret_val == PDCOMM_RESULT_WAIT)
  {
    MSG_MED("TM 1X UP disconnecting", 0, 0, 0);
    pdcomm_info.curr_state = PDCOMM_STATE_DISCONNECTING;       
  }
  else
  {
    MSG_ERROR("pd_comms_app_disconnect returned error: %d",ret_val,0,0);
    pdcomm_info.curr_state = PDCOMM_STATE_DISCONNECTED;       
  }

  /* Send the COMM_DONE event if the connection is already disconnected */
  if (pdcomm_info.curr_state == PDCOMM_STATE_DISCONNECTED)
  {
    /* Send COMM_DONE event */
    tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
    (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                            tm_1x_up_session_info.sess_handle,
                            PDSM_PD_EVENT_COMM_DONE,
                            &tm_1x_up_info_buffer,
                            NULL);
  }
  return ret_val;
} /* End tm_1x_up_comm_disconnect() */


/*===========================================================================

FUNCTION tm_1x_up_comm_read

DESCRIPTION
  This function is used to read data.

RETURN VALUE
  Boolean

DEPENDENCIES      
There has to a successful pd_comms_app_connect() call which returned this 
handle before this function can be called. A pd_comms_app_read() call is not 
allowed when a previous pd_comms_app_read() has still not returned any data. 
In an attempt to do so, a PDCOMM_RESULT_ERROR_PREVIOUS_READ_PENDING will be 
returned.

===========================================================================*/
boolean tm_1x_up_comm_read(uint16 length, boolean wait_all )
{
  boolean ret_val = FALSE;
  pd_comms_return_type comm_ret_val = PDCOMM_RESULT_OK;
  pd_comms_read_params_type read_params;

  /* Only read if the comm state is connected*/
  if ( tm_1x_up_comm_get_state() == PDCOMM_STATE_CONNECTED )
  {   
    read_params.w_data_len = length;
    read_params.u_wait_for_all = wait_all;
    read_params.q_transaction_id = tm_1x_up_comm_get_next_transaction_id();
  
    comm_ret_val = pd_comms_app_read(pdcomm_info.handle,
                                (uint32)THREAD_ID_SM_TM,
                                TM_CORE_PDCOMM_MSG_ID_BASE, 
                                &read_params);
  
    if ( comm_ret_val != PDCOMM_RESULT_WAIT )
    {
      MSG_ERROR("Error reading the data",0,0,0);
      return FALSE;
    }
  
    ret_val = TRUE;
  }
    
  return ret_val;
} /* End tm_1x_up_comm_read() */


/*===========================================================================

FUNCTION tm_1x_up_comm_app_ioctl

DESCRIPTION
  This function is to send the ioctl to PDCOMM

RETURN VALUE
  Boolean

DEPENDENCIES      

===========================================================================*/
boolean tm_1x_up_comm_app_ioctl(pd_comms_ioctl_cmd_type ioctl_cmd)
{
  /* Local Variables */
  pd_comms_return_type ret_val;
  pd_comms_ioctl_params_type ioctl_params;

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) != 0 )
  {
    /*Since IS801 is working under SUPL, let SUPL send ioctl to PDCOMM*/
    return tm_umts_up_supl_comm_app_ioctl(ioctl_cmd);
  }
  
  /* Set the IOCTL parameters*/
  ioctl_params.q_ioctl_request = ioctl_cmd;
  ioctl_params.q_transaction_id = tm_1x_up_comm_get_next_transaction_id();
  ioctl_params.p_argument = NULL;
  ioctl_params.q_argument_len = 0;
  
  ret_val = pd_comms_app_ioctl(pdcomm_info.handle,
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

FUNCTION tm_1x_up_comm_force_dormancy

DESCRIPTION
  This function is used to force dormancy.

RETURN VALUE
  Boolean

DEPENDENCIES      

===========================================================================*/
boolean tm_1x_up_comm_force_dormancy(void)
{
  return tm_1x_up_comm_app_ioctl(PDCOMM_IOCTL_CMD_FORCE_DORMANCY);
}

/*===========================================================================

FUNCTION tm_1x_up_unforce_dormancy

DESCRIPTION
  This function is used to unforce dormancy.

RETURN VALUE
  Boolean

DEPENDENCIES      

===========================================================================*/
boolean tm_1x_up_comm_unforce_dormancy(void)
{
  return tm_1x_up_comm_app_ioctl(PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY);
}


/*===========================================================================

FUNCTION tm_1x_up_comm_connect_with_pde

DESCRIPTION
  This function is used to connect with PDE. It gets invoked when client requests
  for a Position location.
  It invokes PDCOMM functions to open and then connect to PDE. The result of connection
  request is available through asynchronous callbacks.

RETURN VALUE
  Boolean

DEPENDENCIES
  This function makes calls to asynchronous PDCOMM functions.

===========================================================================*/

boolean tm_1x_up_comm_connect_with_pde(void)
{
  if ( tm_1x_up_comm_get_state() == PDCOMM_STATE_CONNECTED)
  {
    MSG_HIGH("Already connected!",0,0,0); 
    /* Already connected, start normal functionality. */
    tm_1x_up_is801_drv_handle_pde_connected(FALSE);
    return TRUE;
  }
  else if ( tm_1x_up_comm_get_state() == PDCOMM_STATE_CONNECTING)            
  {
    /* Waiting to get connected. 
    */
    MSG_ERROR("Waiting to get connected",0,0,0); 
    return TRUE;
  }
  else  /* DISCONNECTING or DISCONNECTED */
  {
    /* Connect to PDE.
    */
    return tm_1x_up_comm_connect();
  } /* End else if disconnected */
} /* End tm_1x_up_comm_connect_with_ms( ) */

/*===========================================================================

FUNCTION tm_1x_up_comm_close_connection

DESCRIPTION
  This function is used to disconnect with PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_1x_up_comm_close_connection(void)
{
  MSG_HIGH("pdcomm_state=%d",tm_1x_up_comm_get_state(),0,0);

  if(pdcomm_info.wait_for_sess_end_ack == TRUE)
  {
    MSG_HIGH("Delay close connection as waiting for sess_end ACK",0,0,0);
    return;
  }
  /* If we are connected Request PDCOMM to terminate connection.
  */
  switch( tm_1x_up_comm_get_state() )
  {
    case PDCOMM_STATE_DISCONNECTING: 
    case PDCOMM_STATE_DISCONNECTED:
      return; 

    case PDCOMM_STATE_CONNECTED:
    case PDCOMM_STATE_CONNECTING:
      /* Start recovery timer. 
      */
      if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_disconnect_timer,TM_1X_UP_DISCONNECT_TIMEOUT,NULL))
      {
        MSG_ERROR("Failed starting disconnect timer",0,0,0);
      }

      /*------------------------------------------------------------------------
          Call 1x UP sub-module interface to close connection, 
          else 1x UP IS-801 Driver will close connection itself
      -------------------------------------------------------------------------*/   
      if( TM_1X_UP_SMOD_DATA_PIPE_REQ_CLOSE_GRANT ==
                 tm_1x_up_smod_data_pipe_req(TRUE, /* request to close the connection */
                                             TM_1X_UP_SMOD_DATA_CMD_NONE,
                                             NULL, 
                                             0) 
        )
      {
        (void) tm_1x_up_comm_disconnect( FALSE );
      }
      break;
    
    default:
      break;
    
  }
} /* End tm_1x_up_comm_close_connection() */


/*===========================================================================

FUNCTION tm_1x_up_comm_abort_connection

DESCRIPTION
  This function is used to abort connection with PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_1x_up_comm_abort_connection( void )
{
  MSG_HIGH("pdcomm_state=%d",tm_1x_up_comm_get_state(),0,0);

  (void) tm_1x_up_comm_disconnect(TRUE);
} /* End tm_1x_up_comm_close_connection() */

/*===========================================================================

FUNCTION tm_1x_up_handle_unknown_pde_version

DESCRIPTION
  This function is used to handle the first messages from the PDE, when its 
  not known what IS801 version the PDE supports. The function first passes the
  message from PDE to the Is801 decoder, and if any bytes are left to the 
  Is801B decoder. If any bytes are decoded by the Is801B decoder, the version
  the PDE is set to IS801B, otherwise IS801-1.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif
/*===========================================================================

FUNCTION tm_1x_up_handle_comm_nofification

DESCRIPTION
  This function is used to handle READ DATA from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/

boolean tm_1x_up_handle_comm_read_data(void *msg_data_ptr)
{
    pd_comms_ipc_read_data_hdr_type* read_hdr_ptr = NULL;
    boolean     ret_val      = TRUE;
    byte        bytes_consumed_801_1  = 0;
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#endif

    if(msg_data_ptr == NULL)
    {
      MSG_ERROR("Received Null msg_data_ptr",0,0,0);
      return FALSE;
    }
    
    
    read_hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg_data_ptr;

    if(read_hdr_ptr->w_data_len == 0)
    {
      /* Received data of zero length from TM-Core */
      MSG_ERROR("Received data of zero length from TM-Core",0,0,0);
      return FALSE;
    }

    if ( tm_1x_up_comm_get_state()!= PDCOMM_STATE_CONNECTED )
    {
      /* Print Error in this case, Data when 1X UP is not
      ** connected.
      */
      MSG_HIGH("Received data from TM-Core in error state %d",
                tm_1x_up_comm_get_state(), 0, 0);
      return FALSE;
    }

    /* A read event has occured, invoked PDCOMM function to read out the data
    */
    /* Now read the actual data */
    if(pdcomm_info.recv_state == TM_1X_UP_COMM_RECV_PAYLOAD)
    {
      MSG_HIGH("Read data of length %d",read_hdr_ptr->p_data[0],0,0);
      pdcomm_info.num_bytes_to_read = read_hdr_ptr->p_data[0]; 
    }
    else
    {
      MSG_HIGH("Forward data of length %d to IS801 module for decoding",
                read_hdr_ptr->w_data_len, 0, 0);
      /* TBD handle when protocol unknown, handle when protocol known */
      is801_util_log_flink_msg((void *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);

      #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
      #error code not present
#else
      /* Fwd PDDM to IS801-1 decoder */
      (void)is801_decode_fwd_link_pddm( TM_PRTL_TYPE_1X_UP, 
                                        (byte *)read_hdr_ptr->p_data,
                                        read_hdr_ptr->w_data_len, 
                                        &bytes_consumed_801_1 );
      #endif
    }
    return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_handle_comm_nofification

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_handle_comm_ioctl_ack(void *msg_data_ptr)
{
  pd_comms_ipc_ioctl_ack_hdr_type* ioctl_data_ptr;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Null msg_data_ptr",0,0,0);
    return FALSE;
  }

  ioctl_data_ptr = (pd_comms_ipc_ioctl_ack_hdr_type*)msg_data_ptr;
  MSG_MED("IOCTL Ack for req %d",ioctl_data_ptr->q_ioctl_request,0,0);
#ifdef FEATURE_GPSONE_UPDATE_BSID  
  if(ioctl_data_ptr->q_ioctl_request == PDCOMM_IOCTL_CMD_FORCE_DORMANCY)
  {
    MSG_MED("force dorm opn successful",0,0,0);
    if((tm_1x_up_is801_drv_session_active() == TRUE) && (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_FD))
    {
      tm_1x_up_session_info.dormancy_event_mask |= TM_1X_UP_DORMANCY_EVENT_MASK_RECVD_PHY_LINK_DOWN;

      /* If TC_EXIT is recevied too, try to do unforce dormancy */
      if(tm_1x_up_session_info.dormancy_event_mask & TM_1X_UP_DORMANCY_EVENT_MASK_RECVD_TC_EXIT)
      {
        tm_1x_up_is801_drv_handle_dormancy_success();
        tm_1x_up_session_info.dormancy_event_mask = TM_1X_UP_DORMANCY_EVENT_MASK_NONE;
      }
    }
    else
    {
      MSG_ERROR("Wrong state!",0,0,0);
      tm_1x_up_is801_drv_handle_dormancy_failure(TM_1X_UP_DORMANCY_FAILURE_TYPE_ABORT);
    }
  }

  if(ioctl_data_ptr->q_ioctl_request == PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY)
  {
    MSG_MED("unforce dorm opn successful",0,0,0);
  }
#endif  
  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_up_handle_comm_nofification

DESCRIPTION
  This function is used to handle WRITE ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_handle_comm_write_ack(void *msg_data_ptr)
{
  pd_comms_ipc_write_ack_hdr_type* write_hdr_ptr;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }
  write_hdr_ptr = (pd_comms_ipc_write_ack_hdr_type*)msg_data_ptr;
  MSG_HIGH("PDCOMM TD: write ack received for %d bytes",write_hdr_ptr->q_bytes_written,0,0);
  pdcomm_info.buffer.num_bytes_written = write_hdr_ptr->q_bytes_written;

  if(tm_1x_up_is801_drv_send_and_log_rlink_pddm())
  {
    if(pdcomm_info.wait_for_sess_end_ack == TRUE)
    {
      MSG_HIGH("Received sess_end ACK",0,0,0);
      pdcomm_info.wait_for_sess_end_ack = FALSE;

      /* For optimal modes, if fallback MSA session is pending, start it now */
      if(tm_1x_up_session_info.mode_info.pending_fallback_msa_fix == TRUE)
      {
        tm_1x_up_is801_drv_start_fallback_msa_fix();
      }
      else if(tm_1x_up_session_info.mode_info.waiting_msb_lr)
      {
        MSG_LOW("Waiting for LR in MSB fix",0,0,0);
        tm_1x_up_comm_close_connection();
      }
      else
      {
        tm_1x_up_is801_drv_cleanup(FALSE);
      }
    }
  }
  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_up_handle_comm_nofification

DESCRIPTION
  This function is used to handle notifications from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_handle_comm_notification(void *msg_data_ptr)
{
    pd_comms_ipc_notification_hdr_type* notify_hdr_ptr;
    pd_comms_notification_params_type* params;
    boolean ret_val = TRUE;

    if(msg_data_ptr == NULL)
    {
      MSG_ERROR("Received Null msg_data_ptr",0,0,0);
      return FALSE;
    }
    notify_hdr_ptr = (pd_comms_ipc_notification_hdr_type*)msg_data_ptr;

    MSG_HIGH("PDCOMM status update %d", notify_hdr_ptr->e_type, 0, 0);  
    if(notify_hdr_ptr->e_type == READ_DATA)
    {
      MSG_HIGH("Read notification from TM-Core",0,0,0);
      if ( tm_1x_up_comm_get_state() == PDCOMM_STATE_CONNECTED )
      {
        if(pdcomm_info.recv_state == TM_1X_UP_COMM_RECV_HEADER)
        {
          MSG_HIGH("Read IS801 header",0,0,0);
          /* First read the IS801 header */
          if(tm_1x_up_comm_read(IS801_LENGTH_HDR_SIZE,FALSE))
          {
            pdcomm_info.recv_state = TM_1X_UP_COMM_RECV_PAYLOAD;
          }
        }
        else
        {
          MSG_HIGH("Read IS801 payload of length %d",pdcomm_info.num_bytes_to_read,0,0);
          if(tm_1x_up_comm_read(pdcomm_info.num_bytes_to_read,TRUE))
          {
            pdcomm_info.recv_state = TM_1X_UP_COMM_RECV_HEADER;
          }
        }
      }
      else
      {
        MSG_ERROR("Read data in error state %d", tm_1x_up_comm_get_state(),0,0);
      }
    }
    else if(notify_hdr_ptr->e_type == STATE_CHANGE)
    {
      params = &notify_hdr_ptr->z_params; 
      MSG_HIGH("State change notification New state %d Curr state %d",params->e_new_state,tm_1x_up_comm_get_state(),0);

      if(params->e_new_state == PDCOMM_STATE_CONNECTED)
      {
        MSG_HIGH(" Connected With PDE",0,0,0);
        pdcomm_info.curr_state = PDCOMM_STATE_CONNECTED;

        /* Already connected, start normal functionality. */
        tm_1x_up_is801_drv_handle_pde_connected(FALSE);
      }
      else if(params->e_new_state == PDCOMM_STATE_DISCONNECTED)
      {
        MSG_HIGH("Curr state=%d Prev DisConnect TrId= %x PDCOMM TrId 0x%x",tm_1x_up_comm_get_state(),pdcomm_info.disconnect_tr_id,notify_hdr_ptr->q_transaction_id);
        pdcomm_info.curr_state = PDCOMM_STATE_DISCONNECTED;

        /* Send COMM_DONE event */
        tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
        (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                                tm_1x_up_session_info.sess_handle,
                                PDSM_PD_EVENT_COMM_DONE,
                                &tm_1x_up_info_buffer,
                                NULL);

        if ( notify_hdr_ptr->q_transaction_id == pdcomm_info.disconnect_tr_id )
        {
          MSG_MED("Previous disconnect completed",0,0,0);
          (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_disconnect_timer);
          tm_1x_up_comm_reset();
        }
      }
      else if(params->e_new_state == PDCOMM_STATE_CONNECTING)
      {
        pdcomm_info.curr_state = PDCOMM_STATE_CONNECTING;
#if defined(FEATURE_CALL_THROTTLE)
        MSG_HIGH("PPP opened",0,0,0);
        tm_1x_up_set_throttling_backoff_time();
#endif /* FEATURE_CALL_THROTTLE */
      }
      else if(params->e_new_state == PDCOMM_STATE_DISCONNECTING)
      {
        /* If we got Disconnecting while trying to CONNECT, */
        if((notify_hdr_ptr->q_transaction_id == pdcomm_info.connect_tr_id) &&
           (tm_1x_up_comm_get_state() == PDCOMM_STATE_CONNECTING ||
            tm_1x_up_comm_get_state() == PDCOMM_STATE_CONNECTED))
        {
          tm_1x_up_is801_drv_handle_comm_error();
        }

        if ( notify_hdr_ptr->q_transaction_id == pdcomm_info.disconnect_tr_id )
        {
          MSG_MED("Previous disconnect in progress",0,0,0);
          (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_disconnect_timer);
          tm_1x_up_comm_reset();
        }
      }
      else
      {
        MSG_ERROR("Unknown state %d",params->e_new_state,0,0);
      }
    }
    else
    {
      MSG_ERROR("Error report",0,0,0);
      params = &notify_hdr_ptr->z_params; 
      pdcomm_info.curr_state = params->e_new_state;
      //if new_state != DISCONNECTED, issue disconnect
      if(tm_1x_up_comm_get_state() != PDCOMM_STATE_DISCONNECTED)
      {
        //Issue disconnect to PDCOMM
        tm_1x_up_comm_close_connection();
      }
      else /* DISCONNECTED */
      {
        (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_disconnect_timer);
      }
      /* reset PDCOMM info */
      tm_1x_up_comm_reset();
      ret_val = FALSE;
    }
    return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_handle_comm_msg

DESCRIPTION
  This function is used to handle messages from PD COMM.

    pd_comms_ipc_write_ack_hdr_type;
    pd_comms_ipc_read_data_hdr_type;
    pd_comms_ipc_notification_hdr_type;
    pd_comms_ipc_ioctl_ack_hdr_type;
    pd_comms_ipc_hash_resp_hdr_type;  

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_handle_comm_msg
(
    pd_comms_client_ipc_message_type  msg_type,
    void                              *msg_data_ptr
)
{
    boolean ret_val = FALSE;

    if(msg_data_ptr == NULL)
    {
      MSG_ERROR("Null msg_data_ptr", 0, 0, 0);
      return FALSE;
    }

    if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) !=0 )
    {
      /*If working under SUPL context, all comm msg will be handled by SUPL
      	and we should not find ourselves in this state*/
      MSG_ERROR("Not expecting to handle comm message", 0, 0, 0);
      return FALSE;
    }
	
    switch(msg_type)
    {
    case C_PD_COMMS_CLIENT_MSG_NOTIFY:
        ret_val = tm_1x_up_handle_comm_notification(msg_data_ptr);
        break;

    case C_PD_COMMS_CLIENT_MSG_WRITE_ACK:    
        ret_val = tm_1x_up_handle_comm_write_ack(msg_data_ptr);
        break;

    case C_PD_COMMS_CLIENT_MSG_IOCTL_ACK:
        ret_val = tm_1x_up_handle_comm_ioctl_ack(msg_data_ptr);
        break;

    case C_PD_COMMS_CLIENT_MSG_READ_DATA:
        ret_val = tm_1x_up_handle_comm_read_data(msg_data_ptr);
        break;

    default:
        MSG_ERROR("Received unknown message", 0, 0, 0);
        ret_val = FALSE;
        break;
    }

    return ret_val;
} /* End tm_1x_up_handle_comm_msg () */


/*===========================================================================

FUNCTION tm_1x_up_comm_reset

DESCRIPTION
  Reset PDCOMM related data at end of every session.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_comm_reset(void)
{
  MSG_MED("Reset PDCOMM related info",0,0,0);
  pdcomm_info.wait_for_sess_end_ack = FALSE;
  pdcomm_info.abort = FALSE;
  pdcomm_info.cust_hdr_sent = FALSE;
  pdcomm_info.num_rev_pddm_801_1 = 0;
  pdcomm_info.recv_state = TM_1X_UP_COMM_RECV_HEADER;
  pdcomm_info.pde_version = IS801_VERSION_UNKNOWN;
  pdcomm_info.num_bytes_to_read = 0;
  memset( (void *)&pdcomm_info.buffer, 0, 
          sizeof(tm_1x_up_comm_output_buffer));
  memset( (void *)&pdcomm_info.rev_pddm, 0, 
          sizeof(is801_rev_link_pddm_info_s_type));
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif
}

/*===========================================================================

FUNCTION tm_1x_up_comm_init

DESCRIPTION
  Initialize PDCOMM related data at startup

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_comm_init(void)
{
  pdcomm_info.handle_valid = FALSE;
  pdcomm_info.tr_id_info.base     = TM_1X_UP_IS801_DRV_TRANSACTION_ID_BASE;
  pdcomm_info.tr_id_info.range    = TM_1X_UP_IS801_DRV_TRANSACTION_ID_RANGE;
  pdcomm_info.tr_id_info.curr_id  = TM_1X_UP_IS801_DRV_TRANSACTION_ID_BASE; 
  tm_1x_up_comm_reset();
}


/*===========================================================================

FUNCTION tm_1x_up_comm_get_state

DESCRIPTION
 Get the current state of PDCOMM

RETURN VALUE
  Enum representing the current PDCOMM state

DEPENDENCIES

===========================================================================*/
pd_comms_state_type tm_1x_up_comm_get_state(void)
{
  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) !=0 )
  {
    /*Since IS801 is working under SUPL context, SUPL is handling the 
    	communication with SLP, so ask SUPL for the pdcomm state*/
    return tm_umts_up_supl_comm_get_tcp_state();
  }
	
	return pdcomm_info.curr_state;
}

