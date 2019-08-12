/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          NMEA functions definition file

GENERAL DESCRIPTION
  This file contains the functions to generate NMEA strings from the fix and
  measurement data received from MGP.
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_nmea.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $ 
  
when       who   what, where, why
--------   ---   ----------------------------------------------------------
04/05/16   gk    handle QZSS empty string 
01/05/16   mc    Dynamically allocate variables for huge structures to reduce stack usage
11/25/15   mc    Removed debug message in tm_nmea_port_put()
05/21/15   nl    Refactor code
03/14/15   rh    Added support for MultiSIM NV  
08/17/14   jv    Added Galileo Support
08/08/14   jv    Added 16-bit SVID support
10/08/13   ah    Removed DGPS fix reporting for MS-Assisted fixes
11/30/12   mj    Removed unused function tm_nmea_clear_gnss_meas_update
11/06/13   rk    Fixed minutes rounding error - displayed as 60 instead 59.99x.
01/20/10   rb    Populate Magnetic Deviation field in RMC string
06/10/09   gk      Added Get UTC offset from NV
08/25/08   mr    Added Geoidal seperation table to convert HAE to MSL
08/06/08   jlp   Changed NMEA output to report C/No at the antenna instead
                 of the baseband reading.
01/08/08   lt    Moved the converstion to UTC time into a TM util. function
05/23/07   mr    Added code to generate proprietary NMEA string
08/07/06   mr    File created
===========================================================================*/

#include "gps_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "cgps_api.h"
#include "sm_api.h"
#include "tm_nmea.h"
#include "tm_common.h"
#include "aries_os_api.h"
#include "tm_pdapi_client.h"
#include "tm_data.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <locale.h>
#include "location_common.h"
#include "comdef.h"
#include <inttypes.h>

#include "sio.h"   
#include "dsm.h"
#include "task.h"

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#endif

#ifdef FEATURE_CGPS_LBS_TASK
#include "lbs_pdapi_iface.h"
#endif

#include "sm_util.h"

/* Constant definitions:
*/
#define TM_NMEA_MAX_NMEA_STRING_SIZE 640 
#define TM_NMEA_GGA_MAX_SV_COUNT     12
  
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*---------------------------------------------------------------------------
  Request packets are sent from the DM to the mobile.  Request packet is 
  reconstructed from pieces received from SIO.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data structures needed for communicating with SIO.
---------------------------------------------------------------------------*/
int tm_nmea_sio_flag = FALSE;

#ifndef __GRIFFON__
sio_stream_id_type       tm_nmea_sio_stream_id = SIO_NO_STREAM_ID;
                                              /* Stream ID returned by 
                                                 sio_open                   */
#endif
LOCAL dsm_watermark_type tm_nmea_to_sio_wmq;     /* WaterMark Queue used when
                                                 data is transmitted to SIO */
LOCAL q_type             tm_nmea_to_sio_q;       /* Queue around which
                                                 tm_nmea_to_sio_wmq is built   */
#ifndef __GRIFFON__
sio_open_type            tm_nmea_open;           /* Data structure used in 
                                                 opening SIO stream.        */
#endif
tm_nmea_diag_open_port_id_e_type  tm_nmea_diag_port_to_open
                                           = TM_NMEA_DIAG_OPEN_PORT_NONE;

/*---------------------------------------------------------------------------
  Global variables needed for OEMDR tunnel detection enablement
---------------------------------------------------------------------------*/
static uint32  q_LastGPSMeasRefCount;
static uint32  q_LastGLOMeasRefCount;
static boolean b_IsFreshGPSMeas;
static boolean b_IsFreshGLOMeas;

/*===========================================================================
  FUNCTION PROTOTYPES
===========================================================================*/
static boolean tm_nmea_is_differential_fix(const sm_GnssFixRptStructType *p_fix);
static boolean tm_nmea_is_propagated_fix  (const sm_GnssFixRptStructType *p_fix);

/*===========================================================================

FUNCTION tm_nmea_send_message_no_payload

DESCRIPTION
 Send IPC to TM task with no IPC payload.

DEPENDENCIES
None.

RETURN VALUE
None.  This procedure does not return.

SIDE EFFECTS
None.

===========================================================================*/
void tm_nmea_send_message_no_payload(uint32 q_ipcMsg)
{
  os_IpcMsgType              *p_ipc_msg;

  /*No payload required for this callback message*/
  p_ipc_msg = os_IpcCreate(0,IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (p_ipc_msg != NULL)
  {
    /* Set the msg id and init the command which failed. */
    p_ipc_msg->q_MsgId = q_ipcMsg;

    if (!os_IpcSend(p_ipc_msg, (uint32)THREAD_ID_SM_TM))
    {
      (void)os_IpcDelete(p_ipc_msg);   
      MSG_ERROR("Failed to send %d IPC",q_ipcMsg,0,0); 
    }
  } /* End p_ipc_msg */
  else
  {
    MSG_ERROR("Failed to create %d IPC",q_ipcMsg,0,0); 
  } /* End p_ipc_msg */
}


#ifdef FEATURE_RUNTIME_DEVMAP
/*===========================================================================

FUNCTION tm_nmea_open_cb

DESCRIPTION
  This callback is called when RDM wants NMEA to open a port.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_nmea_open_cb (sio_port_id_type port_id)
{
  os_IpcMsgType              *p_ipc_msg;
    
  MSG_MED("Received RDM OPEN CB for port %d",(uint32)port_id,0,0); /*lint !e571 */
  /*No payload required for this callback message*/
  p_ipc_msg = os_IpcCreate(sizeof(sio_port_id_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (p_ipc_msg != NULL)
  {
    /* Set the msg id and init the command which failed. */
    p_ipc_msg->q_MsgId = TM_CORE_NMEA_MSG_ID_SIO_OPEN_CB;
    p_ipc_msg->q_Size  = sizeof(sio_port_id_type);
    memscpy(p_ipc_msg->p_Data, p_ipc_msg->q_Size, &port_id, sizeof(sio_port_id_type));
    p_ipc_msg->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;

    if (!os_IpcSend(p_ipc_msg, (uint32)THREAD_ID_SM_TM))
    {
      (void)os_IpcDelete(p_ipc_msg);   
      MSG_ERROR("Failed to send RDM OPEN CB IPC mesasge",0,0,0); 
    }
  } /* End p_ipc_msg */
  else
  {
    MSG_ERROR("Failed to create RDM OPEN CB IPC message",0,0,0); 
  } /* End p_ipc_msg */
  
}

/*===========================================================================

FUNCTION tm_nmea_close_cb

DESCRIPTION
  This callback is called when RDM wants NMEA to close its port.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_nmea_close_cb (void)
{
  os_IpcMsgType              *p_ipc_msg;  
  
  MSG_MED("Received RDM CLOSE CB",0,0,0); 
  tm_nmea_open.port_id = SIO_PORT_NULL;
  
  /*No payload required for this callback message*/
  p_ipc_msg = os_IpcCreate(0,IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (p_ipc_msg != NULL)
  {
    /* Set the msg id and init the command which failed. */
    p_ipc_msg->q_MsgId = TM_CORE_NMEA_MSG_ID_SIO_CLOSE_CB;

    if (!os_IpcSend(p_ipc_msg, (uint32)THREAD_ID_SM_TM))
    {
      (void)os_IpcDelete(p_ipc_msg);   
      MSG_ERROR("Failed to send RDM OPEN CB IPC mesasge",0,0,0); 
    }
  } /* End p_ipc_msg */
  else
  {
    MSG_ERROR("Failed to create RDM OPEN CB IPC message",0,0,0); 
  } /* End p_ipc_msg */
  
  rdm_notify(RDM_NMEA_SRVC, RDM_DONE_S);
}

/*===========================================================================

FUNCTION tm_nmea_close

DESCRIPTION
  This callback is closes the NMEA connection, if there is one.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_nmea_close(void (*close_func_ptr)(void)) 
{
  MSG_MED("NMEA Close Port: %d",(int32)tm_nmea_sio_stream_id,0,0);

  if (tm_nmea_sio_stream_id != SIO_NO_STREAM_ID)
  {
    sio_close(tm_nmea_sio_stream_id, close_func_ptr);
    tm_nmea_sio_flag      = FALSE;
    tm_nmea_sio_stream_id = SIO_NO_STREAM_ID;
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION tm_nmea_sio_close_cb

DESCRIPTION
  This callback is closes the NMEA connection, if there is one.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_nmea_sio_close_cb(void)
{
  MSG_MED("Received SIO CLOSE CB",0,0,0); 
  tm_nmea_send_message_no_payload(TM_CORE_NMEA_MSG_ID_SIO_CLOSE_CB_2);
}

/*===========================================================================

FUNCTION tm_nmea_open_handler_part2

DESCRIPTION
  This function is used to open NMEA message output port.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_nmea_open_handler_part2(void)
{
  MSG_MED("NMEA open handler part2",0,0,0); 
  tm_nmea_sio_stream_id = sio_open(&tm_nmea_open);

  if (tm_nmea_sio_stream_id != SIO_NO_STREAM_ID)
  {
    tm_nmea_sio_flag = TRUE; 
    rdm_notify(RDM_NMEA_SRVC, RDM_DONE_S);
  }
  else
  {
    tm_nmea_sio_flag = FALSE;
    rdm_notify(RDM_NMEA_SRVC, RDM_NOT_ALLOWED_S); 
  }
}

/*===========================================================================

FUNCTION tm_nmea_open_handler

DESCRIPTION
This function is used to open NMEA message output port.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/
void tm_nmea_open_handler(void *p_data)
{
  MSG_MED("RDM OPEN CB handler called",0,0,0);   

  if (NULL == p_data) {
    MSG_ERROR("Null pointer argument",0,0,0);
    return;
  }
  /* We were told to open, so try to do so.  Start by closing 
  ** any existing connection.
  */

  /* save the port to open once close callback is called */
  tm_nmea_open.port_id = *((sio_port_id_type*) p_data);

  MSG_MED("RDM OPEN CB handler port: %d", (int32) tm_nmea_open.port_id,0,0);

  if(!tm_nmea_close(tm_nmea_sio_close_cb)) {
    tm_nmea_open_handler_part2();
  }
}

/*===========================================================================

FUNCTION tm_nmea_close_handler

DESCRIPTION
  This function is used to close NMEA message's output port .

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_nmea_close_handler(void)
{
  MSG_MED("RDM CLOSE CB handler called",0,0,0);     
  /* We were told to close, so close.
  */
  tm_nmea_close(NULL);
}
#endif /* FEATURE_RUNTIME_DEVMAP */



#ifndef __GRIFFON__

/*===========================================================================

FUNCTION tm_nmea_sio_init

DESCRIPTION
  This function contains the initialization for the NMEA sio.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_nmea_sio_init (void) /* Not used by diag_task */
{
  /* Set up Queue for sending data to SIO for transmission.
  */
  (void) q_init( &tm_nmea_to_sio_q);
  tm_nmea_to_sio_wmq.q_ptr                = &tm_nmea_to_sio_q;

  /* Set up watermarks for WaterMark Queue.  Also, set-up
  ** functions which will be called when the watermarks are
  ** reached.

  ** While NMEA's own flow control is handled by the new DSM's
  ** flow management table, SIO still uses this watermark to store
  ** DSM items when the FIFO is full.
  */
  tm_nmea_to_sio_wmq.lo_watermark         = DSM_DS_SMALL_ITEM_SIZ; /*lint !e26 !e10 */
  tm_nmea_to_sio_wmq.hi_watermark         = 100;
  tm_nmea_to_sio_wmq.dont_exceed_cnt      = 100*5;
  tm_nmea_to_sio_wmq.gone_empty_func_ptr  = NULL; 
  tm_nmea_to_sio_wmq.non_empty_func_ptr   = NULL;
  tm_nmea_to_sio_wmq.lowater_func_ptr     = NULL;
  tm_nmea_to_sio_wmq.hiwater_func_ptr     = NULL;

  /* WaterMark Queue starts out empty.
  */
  tm_nmea_to_sio_wmq.current_cnt          = 0;

  if (!tm_nmea_to_sio_wmq.lock) { 
       /* dsm_init() is commented out because it gets called in TMC at bootup */
       /* Calling it twice causes System instability since dsm_init() cannot 
        * handle multiple inits */
   
   //    dsm_init();
    dsm_queue_init(&tm_nmea_to_sio_wmq,
      (int)tm_nmea_to_sio_wmq.dont_exceed_cnt, &tm_nmea_to_sio_q ); 
  }

  /* Prepare SIO opening parameters. */
  tm_nmea_open.stream_mode    = SIO_GENERIC_MODE; 
  tm_nmea_open.tx_queue       = &tm_nmea_to_sio_wmq;

  tm_nmea_open.port_id        = SIO_PORT_NULL;

  /* SIO is configured to look for tail character.
  */
  tm_nmea_open.tail_char_used = TRUE;
  tm_nmea_open.tail_char      = 0x07e;
  tm_nmea_open.rx_func_ptr    = NULL;

  /* Use whatever the flow control method is currently.
  */
  tm_nmea_open.rx_flow        = SIO_FCTL_OFF;
  tm_nmea_open.tx_flow        = SIO_FCTL_OFF;

  /* Use 115.2Kbps for now
  */
  tm_nmea_open.rx_bitrate     = SIO_BITRATE_115200;
  tm_nmea_open.tx_bitrate     = SIO_BITRATE_115200;

  /* Initialize the structure lconv for snprintf */
  (void)setlocale( LC_ALL, "C" );

  return;
}
/*===========================================================================

FUNCTION tm_nmea_sio_transmit

DESCRIPTION
  This function contains the transmit call for the NMEA sio.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_nmea_sio_transmit(char *p_str)
{
  dsm_mempool_id_enum_type   dsm_pool_id;
  dsm_item_type *tx_item_ptr = NULL; /* Packet into which outgoing packet */ 

  /* Allocate a DSM item */
  if( tx_item_ptr == NULL )
  {
    dsm_pool_id = DSM_DS_POOL_SIZE(TM_NMEA_MAX_NMEA_STRING_SIZE);

    tx_item_ptr = dsm_new_buffer( dsm_pool_id );      

    if( tx_item_ptr == NULL )
    {
      MSG_ERROR("Ran out of DSM buffers",0,0,0);  
      return; 
    }
    else
    {
      tx_item_ptr->used = 0;
    }
  }

  /* Copy data to tx_item_ptr */
  while( *p_str != '\0' )
  {
    tx_item_ptr->data_ptr[tx_item_ptr->used++] = *p_str;
    p_str++;
  }
  if (tm_nmea_sio_stream_id != SIO_NO_STREAM_ID) 
  {
    sio_transmit( tm_nmea_sio_stream_id, tx_item_ptr );
  } 
  else
  {
    (void)dsm_free_buffer (tx_item_ptr);
  }  
  tx_item_ptr = NULL;

  return;
}


#endif  // __GRIFFON__


/*===========================================================================
FUNCTION tm_nmea_port_put

DESCRIPTION
  tm_nmea_port_put() puts the constructed NMEA string out to a serial port 
  (to be picked up by Street Atlas, for example).

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
void tm_nmea_port_put( uint8 u_Port, char *p_str )
{
  /* dont send NMEA out for on demand sessions and internal sessions */
  if (tm_core_is_on_demand_session_on() != TRUE ||
      tm_core_is_internal_session_on() != TRUE)
  {
    if( tm_nmea_sio_flag == TRUE )
    {

#ifndef __GRIFFON__

      tm_nmea_sio_transmit( p_str );

#endif  // __GRIFFON__

    }
  }
}
/* End of tm_nmea_port_put(). */


/*===========================================================================
FUNCTION tm_nmea_pdapi_send

DESCRIPTION
  tm_nmea_pdapi_send() puts the constructed NMEA sends the NMEA strings on
  PDAPI interface.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
void tm_nmea_pdapi_send(char *p_str, pdsm_pa_nmea_type e_nmea_type)
{
  pdsm_ext_status_info_s_type *pz_z_ext_status_info_type = NULL;

  if(NULL == p_str)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL NMEA string received for PDAPI send");
    return;
  }

  pz_z_ext_status_info_type = (pdsm_ext_status_info_s_type *)os_MemAlloc(sizeof(pdsm_ext_status_info_s_type), OS_MEM_SCOPE_TASK);
  if (NULL == pz_z_ext_status_info_type)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_ext_status_info_s_type in tm_nmea_pdapi_send");
    return;
  }
  memset( pz_z_ext_status_info_type, 0, sizeof(*pz_z_ext_status_info_type) );
 

  pz_z_ext_status_info_type->client_id = tm_core_info.session_info.pd_param.client_id;
  pz_z_ext_status_info_type->ext_status_info.ext_nmea_pos_report_type.nmea_type = e_nmea_type; 
  pz_z_ext_status_info_type->ext_status_info.ext_nmea_pos_report_type.nmea_length = strlen(p_str);    
  pz_z_ext_status_info_type->ext_status_type = PDSM_EXT_STATUS_NMEA_POS_REPORT;
  (void)GNSS_STRLCPY((char*)(pz_z_ext_status_info_type->ext_status_info.ext_nmea_pos_report_type.nmea_data),
            p_str,PDSM_NMEA_DATA_LENGTH);
  
  tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_NMEA_POS_REPORT, 
                                       pz_z_ext_status_info_type); 
  (void)os_MemFree((void **)&pz_z_ext_status_info_type);
}

/*===========================================================================
FUNCTION tm_nmea_sentenceTerminate
 
DESCRIPTION
  tm_nmea_sentenceTerminate() is used to append the NMEA sentence with the 
  standard terminator. This consists of a "*0x2.2X\r\n" fragment. 

 'The checksum is the 8-bit exclusive OR (no start or stop bits) of all the
  characters in the sentence, including the "," delimiters, between but not
  including the "$" and the "*" delimiters.'

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
static void tm_nmea_sentenceTerminate( char *p_string )
{
  uint8 u_Checksum = 0;

  p_string++;                     /* Skip the $ token. */

  while (*p_string != '\0' )      /* Scan up to the end of the sentence. */
  {
    u_Checksum ^= *p_string++;
  }

  (void)snprintf( p_string, 200,"*%02X\r\n", u_Checksum );
    
}/* End of tm_nmea_sentenceTerminate(). */

/*===========================================================================
FUNCTION tm_nmea_terminate_and_put
 
DESCRIPTION
  tm_nmea_terminate_and_put() is used to transmit the nmea message after
  terminating the nmea sentence

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
static void tm_nmea_terminate_and_put(char *p_str, pdsm_pa_nmea_type e_nmea_type)
{
  /* Append the "*CS\r\n" to the end of the sentenvce */
  tm_nmea_sentenceTerminate( p_str );
  /* Transmit the constructed sentence */
  tm_nmea_port_put(0, p_str);
  /*Send NMEA string over to PDAPI*/
  tm_nmea_pdapi_send(p_str,e_nmea_type);

}

/*===========================================================================
FUNCTION tm_nmea_empty_put
 
DESCRIPTION
  tm_nmea_empty_put() is used to prepare and transmit the empty message 

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/

static void tm_nmea_empty_put(char *p_str, int q_buf_len, pdsm_pa_nmea_type e_nmea_type)
{
  int q_len = 0 ;                  

  if ( (NULL == p_str) || (q_buf_len <= 0) )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_nmea_empty_put(): Received NULL Pointer or Invalid Buffer Length");
    return;
  }

  /* Check if NMEA matches with configured constellation types */
  switch(e_nmea_type)
  {
    case PDAPI_NMEA_GPGGA:        
    case PDAPI_NMEA_GPRMC:
    case PDAPI_NMEA_GPGSV:
    case PDAPI_NMEA_GPGSA:
    case PDAPI_NMEA_GPVTG:
        /*GPS is always enabled*/
        break;
    case PDAPI_NMEA_GLGSV:        
      if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_GLO))
      {
         return;
      }
      break;

    case PDAPI_NMEA_GNGSA:
    case PDAPI_NMEA_GNGNS:
    case PDAPI_NMEA_PQXFI:
    case PDAPI_NMEA_PSTIS:
      break;

    case PDAPI_NMEA_GAGGA:        
    case PDAPI_NMEA_GARMC:
    case PDAPI_NMEA_GAGSV:
    case PDAPI_NMEA_GAGSA:
    case PDAPI_NMEA_GAVTG:
      if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_GAL))
      {
         return;
      }
      break;
      
    case PDAPI_NMEA_PQGSA:
    case PDAPI_NMEA_PQGSV:
      if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_QZSS))
      {
        return;
      }
      break;  
    default:
    {
      MSG_ERROR("NMEA type not supported",0,0,0);
      return;
    }
  }  

  switch(e_nmea_type)
  {
    case PDAPI_NMEA_GPGGA:        
      q_len = snprintf( p_str, q_buf_len,"$GPGGA,,,,,,0,,,,,,,," );
      break;
    case PDAPI_NMEA_GPRMC:
      q_len = snprintf( p_str, q_buf_len, "$GPRMC,,V,,,,,,,,,,N" );
      break;
    case PDAPI_NMEA_GPGSV:
      q_len = snprintf( p_str, q_buf_len,"$GPGSV,1,1,0," );
      break;
    case PDAPI_NMEA_GPGSA :
      q_len = snprintf( p_str , q_buf_len,"$GPGSA,A,1,,,,,,,,,,,,,,,");
      break;
    case PDAPI_NMEA_GPVTG:
      q_len = snprintf( p_str, q_buf_len,"$GPVTG,,T,,M,,N,,K,N" );
      break;
    case PDAPI_NMEA_GLGSV:
      q_len = snprintf( p_str, q_buf_len,"$GLGSV,1,1,0," );
      break;
    case PDAPI_NMEA_GNGSA:
      q_len = snprintf( p_str , q_buf_len,"$GNGSA,A,1,,,,,,,,,,,,,,,");
      break;
    case PDAPI_NMEA_GNGNS:
      q_len = snprintf( p_str, q_buf_len,"$GNGNS,,,,,,NNN,,,,,," );
      break;
    case PDAPI_NMEA_PQGSA:
      q_len = snprintf( p_str , q_buf_len,"$PQGSA,A,1,,,,,,,,,,,,,,,");
      break;
    case PDAPI_NMEA_PQGSV:
      q_len = snprintf( p_str, q_buf_len,"$PQGSV,1,1,0," );
      break;
    case PDAPI_NMEA_PQXFI:
      q_len = snprintf( p_str, q_buf_len,"$PQXFI,,,,,,,,,," );
      break;
    case PDAPI_NMEA_PSTIS:
      q_len = snprintf( p_str, q_buf_len,"$PSTIS," );
      break;
    case PDAPI_NMEA_GAGGA:        
      q_len = snprintf( p_str, q_buf_len,"$GAGGA,,,,,,0,,,,,,,," );
      break;
    case PDAPI_NMEA_GARMC:
      q_len = snprintf( p_str, q_buf_len, "$GARMC,,V,,,,,,,,,,N" );
      break;
    case PDAPI_NMEA_GAGSV:
      q_len = snprintf( p_str, q_buf_len,"$GAGSV,1,1,0," );
      break;
    case PDAPI_NMEA_GAGSA:
      q_len = snprintf( p_str , q_buf_len,"$GAGSA,A,1,,,,,,,,,,,,,,,");
      break;
    case PDAPI_NMEA_GAVTG:
      q_len = snprintf( p_str, q_buf_len,"$GAVTG,,T,,M,,N,,K,N" );
      break;
    default:
    {
      MSG_ERROR("NMEA type not supported",0,0,0);
      return;
    }
  }
  
  if(q_len < 0 || q_len >= q_buf_len)
  {
    MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
    return;
  }
  
  tm_nmea_terminate_and_put(p_str, e_nmea_type);  
}

/*===========================================================================
FUNCTION tm_nmea_get_gps_fix_from_refpos

This function takes a reference position of type gps_RefLocStructType and
populates a sm_GpsFixRptStructType with only the fields required to generate 
NMEA strings. This is used in case of MSA where the final fix from the server
doesn't contain all the information required to generate NMEA.
   
DESCRIPTION

DEPENDENCIES

RETURN VALUE sm_GpsFixRptStructType* 

SIDE EFFECTS
 
===========================================================================*/
void tm_nmea_get_gps_fix_from_refpos(const gps_RefLocStructType *p_refpos,
                                     sm_GnssFixRptStructType *p_fix)
{
  uint32 q_LeapSeconds;
    
  if(p_refpos == NULL || p_fix == NULL)
  {
    return;
  }

  memset(p_fix,0,sizeof(sm_GnssFixRptStructType));

  /* Compute UTC time of fix, adjusting for the GPS leap seconds. */
  q_LeapSeconds = cgps_GetNvUtcOffset(); /*Hardcode this for now, call a PE provided API when available*/

  if (p_refpos->q_GpsMsec/1000L < q_LeapSeconds)
  {
    p_fix->z_NavPos.q_UtcTimeMs = p_refpos->q_GpsMsec +
          (WEEK_SECS - q_LeapSeconds) * 1000L;
  }
  else
  {
    p_fix->z_NavPos.q_UtcTimeMs = p_refpos->q_GpsMsec - q_LeapSeconds * 1000L;
  }
  
  p_fix->z_NavPos.d_PosLla[0] = p_refpos->d_Latitude;
  p_fix->z_NavPos.d_PosLla[1] = p_refpos->d_Longitude;
  
  p_fix->z_NavPos.f_PuncLatLonMeters[0] = p_refpos->f_LocUncrtntySemiMajor;
  p_fix->z_NavPos.f_PuncLatLonMeters[1] = p_refpos->f_LocUncrtntySemiMinor;
  
  if (p_refpos->u_AltitudeIsValid)
  {
    p_fix->z_NavPos.d_PosLla[2] = (DBL)p_refpos->f_Altitude;
  }
  
  p_fix->z_NavPos.z_PosFlags.b_Is3D = TRUE;
  p_fix->z_NavPos.z_PosFlags.b_IsExternal = TRUE;

  if(p_refpos->u_VelocityIsValid && p_refpos->u_HeadingIsValid)
  {
    p_fix->z_NavPos.z_PosFlags.b_IsVelValid = TRUE;
    p_fix->z_NavPos.f_VelEnu[0]      = p_refpos->f_VelHor * cos(p_refpos->f_Heading);
    p_fix->z_NavPos.f_VelEnu[1]      = p_refpos->f_VelHor * sin(p_refpos->f_Heading); 
    p_fix->z_NavPos.f_VelEnu[2]      = p_refpos->f_VelVert;

    /*Calculate Heading here*/
    if(p_fix->z_NavPos.f_VelEnu[0] !=0 || p_fix->z_NavPos.f_VelEnu[1] != 0)
    {
      FLT d_azimuth; 
      
      d_azimuth  =  (FLT)atan2( p_fix->z_NavPos.f_VelEnu[0] , p_fix->z_NavPos.f_VelEnu[1] );      
      if (d_azimuth < (FLT)0.0) 
      {
        d_azimuth += (FLT)(2.0*C_PI);
      }
      
      p_fix->z_NavPos.f_HeadingRad = d_azimuth;
      p_fix->z_NavPos.z_PosFlags.b_IsHeadingValid = TRUE;
    }
    else
    {
      p_fix->z_NavPos.f_HeadingRad = 0;
    }
  }
  
  p_fix->z_NavPos.w_GpsWeek   = p_refpos->w_GpsWeek;
  p_fix->z_NavPos.q_GpsTimeMs = p_refpos->q_GpsMsec;
  
  /*Information not provided by protocol sub-modules. 
    Set default values*/     
  memset((void *)&p_fix->z_NavSvInfo, 0, sizeof(p_fix->z_NavSvInfo));
 
  p_fix->z_NavPos.f_HDOP = 0.1;
  p_fix->z_NavPos.f_VDOP = 0.1;
  p_fix->z_NavPos.f_PDOP = 0.14142; /*PDOP = sqrt( HDOP*HDOP + VDOP*VDOP )*/
  
}/*tm_nmea_get_gps_fix_from_refpos*/

/*===========================================================================
FUNCTION tm_nmea_gpgga_put

DESCRIPTION

 tm_nmea_gpgga_put() is used to build and transmit the NMEA GGA sentence. 
 
 Global Positioning System Fix Data.
 
  Time, position and fix related data for a GPS receiver.
 
  1 2   3         4       5 6        7 8 9  10 11  12 13 14 15  16   17
  | |   |         |       | |        | | |  |   |   | |   | |   |    |
 $aaccc,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>
 
 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Universal Time Coordinated (UTC) of position.
  4) Latitude  (DDmm.mm)    DD = two digits of degrees, mm.mm = minutes.
  5) N or S (North or South).
  6) Longitude (DDDmm.mm).
  7) E or W (East or West).
  8) GPS Quality Indicator,
     0 - fix not available or invalid,
     1 - GPS SPS Mode, fix valid,
     2 - Differential GPS, SPS Mode, fix valid
     3 - GPS PPS Mode, fix valid
     4 - Real Time Kinematic:  Satellite system used in RTK mode with fixed
         integers,
     5 - Float RTK:  Satellite system used in RTK mode, floating integers
     6 - Estimated (dead reckoning) Mode,
     7 - Manual Input Mode,
     8 - Simulator Mode.
  9) Number of satellites in use, 00 - 12.
 10) Horizontal Dilution of Precision (HDOP).
 11) Antenna Altitude above/below mean-sea-level (geoid), meters.
 12) Units of antenna altitude, meters.
 13) Geoidal separation, the difference between the WGS-84 earth ellipsoid and
     the mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid.
 14) Units of geoidal separation, meters.
 15) Age of differential GPS data, time in seconds since last RTCM-SC104 
     Type 1 or Type 9 update (null field when DGPS is not used).
 16) Differential reference station ID, 0000-1023.
 17) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
void tm_nmea_gpgga_put( const sm_GnssFixRptStructType *p_fix ,boolean u_no_fix )
{
  char b_sentence[ 200 ];
  char *p_b = b_sentence;
  int q_buf_len = sizeof(b_sentence);
  int q_len = 0 ;
  boolean b_utc_valid = FALSE;
  tm_CalendarTimeT z_Time;
  float f_geoidal_sep = 0.0;

  if( p_fix == NULL )
  {
    tm_nmea_empty_put(b_sentence, q_buf_len, PDAPI_NMEA_GPGGA);
    return;
  }
       
    /*  Universal Time Coordinated (UTC) of position: */
  b_utc_valid = tm_util_get_fix_time(p_fix, &z_Time);

  if (b_utc_valid == TRUE)
  {
    q_len = snprintf( p_b, q_buf_len,
                     "$GPGGA,%02" PRIu32 // Hour
                     "%02" PRIu32        // Minute
                     "%02" PRIu32        // Second
                     ".%02" PRIu32 "," , // Millisecond/10
                     z_Time.q_Hour, z_Time.q_Minute,
                     z_Time.q_Second, z_Time.q_Millisecond / 10);
  }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
  /* END - UTC of position */

  if(u_no_fix)
  {
    /* Construct the 'know nothing position sentence */
      q_len = snprintf( p_b, q_buf_len,",,,,0,,,,,,,," );  

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
  }
  else
  {
      /* Latitude and longitude: */
    q_len = tm_util_calculate_position_print(p_fix, p_b, q_buf_len);
    if(q_len <= 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }
        p_b += q_len;
        q_buf_len -= q_len;
    /* END - Latitude and Longitude */

      /* GPS quality issues: */
      {
        uint8 u_GpsQual = 0;       /* Fix available or. */        
        uint8 u_numSv;

        if (p_fix->z_NavPos.z_PosFlags.b_IsValid)
        {
          if (tm_nmea_is_differential_fix(p_fix))
        {
              u_GpsQual = 2;  /* DGPS fix */
          }
          else
          {
            if (tm_nmea_is_propagated_fix(p_fix) ||
                ((p_fix->z_NavSvInfo.u_GpsNumSvPosFix == 0) &&
                 (p_fix->z_NavSvInfo.u_GloNumSvPosFix == 0) &&
                 (p_fix->z_NavPos.z_PosFlags.b_IsInsOnlySln)) )
            {
              u_GpsQual = 6; /* Estimated (dead reckoning) fix */
            }
            else
            {
              u_GpsQual = 1; /* GPS fix */
            }
          }
        }
        else
        {
          u_GpsQual = 0; /* Invalid fix */
        }

        u_numSv = p_fix->z_NavSvInfo.u_GpsNumSvPosFix;

        /*Limiting the number of SV's to 12*/
        if(u_numSv > TM_NMEA_GGA_MAX_SV_COUNT)
        {
          u_numSv = TM_NMEA_GGA_MAX_SV_COUNT;
        }

        q_len = snprintf( p_b, q_buf_len, "%d,%02d,%.1f," , 
                        u_GpsQual, u_numSv, p_fix->z_NavPos.f_HDOP );

        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
      }

      /* Altitude, etc.: */
      
        /*Find the Geoidal seperation before signs are manipulated*/
    f_geoidal_sep = loc_GetGeoidalSeparation(
      p_fix->z_NavPos.d_PosLla[0] * (180.0 / C_PI), 
      p_fix->z_NavPos.d_PosLla[1] * (180.0 / C_PI) );

      q_len = snprintf( p_b, q_buf_len,"%.1lf,M,%.1f,M,," , 
                       p_fix->z_NavPos.d_PosLla[2]-f_geoidal_sep , f_geoidal_sep );

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
    /* END - Altitude */
      /* TBD: 
       * 11) Antenna Altitude above/below mean-sea-level - Currently reporting HAE
       * 13) Geoidal separation  
       * 15) Age of differential GPS data                 
       * 16) Differential reference station ID            
       *   The "null" entries are the ,,,, portion of the string. 
      */
    }/*else for if(u_no_fix)*/

  tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_GPGGA);  
  MSG_MED("Generated NMEA $GPGGA",0,0,0);
}

/*===========================================================================
FUNCTION tm_nmea_gngns_put

DESCRIPTION

 tm_nmea_gngns_put() is used to build and transmit the NMEA GGA sentence. 
 
 GNSS Fix data
 
  Time, position and fix related data for a GNSS receiver.
 
  1 2   3         4       5 6        7 8    9  10  11  12  13  14  15  
  | |   |         |       | |        | |    |  |   |   |   |   |   |   
 $aaccc,hhmmss.ss,llll.ll,a,yyyyy.yy,a,c--c,xx,x.x,x.x,x.x,x.x,x.x*hh<CR><LF>
 
 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Universal Time Coordinated (UTC) of position.
  4) Latitude  (DDmm.mm)    DD = two digits of degrees, mm.mm = minutes.
  5) N or S (North or South).
  6) Longitude (DDDmm.mm).
  7) E or W (East or West).
  8) Mode Indicator,
     N - fix not available or invalid,
     A - Autonomous. Satellite system used in non-differential mode in position fix
     D - Differential GPS, SPS Mode, fix valid
     P - GPS PPS Mode, fix valid
     R - Real Time Kinematic:  Satellite system used in RTK mode with fixed
         integers,
     F - Float RTK:  Satellite system used in RTK mode, floating integers
     E - Estimated (dead reckoning) Mode,
     M - Manual Input Mode,
     S - Simulator Mode.
  9) Number of satellites in use, (gps+glo+gal).
 10) Horizontal Dilution of Precision (HDOP).
 11) Antenna Altitude above/below mean-sea-level (geoid), meters.
 12) Geoidal separation, the difference between the WGS-84 earth ellipsoid and
     the mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid.
 13) Age of differential GPS data, time in seconds since last RTCM-SC104 
     Type 1 or Type 9 update (null field when DGPS is not used).
 14) Differential reference station ID.
 15) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
void tm_nmea_gngns_put( const sm_GnssFixRptStructType *p_fix ,boolean u_no_fix )
{
  char b_sentence[ 200 ];
  char *p_b = b_sentence;
  int q_buf_len = sizeof(b_sentence);
  int q_len = 0 ;                  
  boolean b_utc_valid = FALSE;
  tm_CalendarTimeT z_utc_time;
  float f_geoidal_sep = 0.0;

  if( p_fix == NULL )
  {
    tm_nmea_empty_put(b_sentence, q_buf_len, PDAPI_NMEA_GNGNS);
    return;
  }

    /*  Universal Time Coordinated (UTC) of position: */
  b_utc_valid = tm_util_get_fix_time(p_fix, &z_utc_time);
  if (b_utc_valid == TRUE)
  {
    q_len = snprintf(p_b, q_buf_len,
                     "$GNGNS,%02" PRIu32 // Hour
                     "%02" PRIu32        // Minute
                     "%02" PRIu32        // Second
                     ".%02" PRIu32 "," , // Millisecond/10
                     z_utc_time.q_Hour, z_utc_time.q_Minute,
                     z_utc_time.q_Second, z_utc_time.q_Millisecond / 10);
  }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
  /* END - UTC of position */

  if(u_no_fix)
  {
    /* Construct the 'know nothing position sentence */
      q_len = snprintf( p_b, q_buf_len,",,,,NNN,,,,,," );  

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
  }
  else
  {
      /* Latitude and longitude: */
    q_len = tm_util_calculate_position_print(p_fix, p_b, q_buf_len);
    if(q_len <= 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }
        p_b += q_len;
        q_buf_len -= q_len;
    /* END - Latitude and Longitude */

      /* Mode Indicator: */
      {
        char u_GpsQual_gps = 'N';      
        char u_GpsQual_glonass = 'N';
        char u_GpsQual_galileo = 'N';

        if (p_fix->z_NavPos.z_PosFlags.b_IsValid)
        {
          if (tm_nmea_is_differential_fix(p_fix))
          {
               /* DGPS fix */
            if (p_fix->z_NavSvInfo.u_GpsNumSvPosFix > 0)
            {
              u_GpsQual_gps     = 'D'; /* Differential GPS, SPS Mode, fix valid */
            }
            if (p_fix->z_NavSvInfo.u_GloNumSvPosFix > 0)
            {
              u_GpsQual_glonass = 'D'; /* Differential GPS, SPS Mode, fix valid */
            }
            if (p_fix->z_NavSvInfo.u_GalNumSvPosFix > 0)
            {
              u_GpsQual_galileo = 'D'; /* Differential GPS, SPS Mode, fix valid */
            }
          }
          else
          {
            if (tm_nmea_is_propagated_fix(p_fix) ||
                ((p_fix->z_NavSvInfo.u_GpsNumSvPosFix == 0) &&
                 (p_fix->z_NavSvInfo.u_GloNumSvPosFix == 0) &&
                 (p_fix->z_NavSvInfo.u_GalNumSvPosFix == 0) &&
                 (p_fix->z_NavPos.z_PosFlags.b_IsInsOnlySln)) )
            {
              u_GpsQual_gps     = 'E'; /* Estimated (dead reckoning) fix */
              u_GpsQual_glonass = 'E'; /* Estimated (dead reckoning) fix */
              u_GpsQual_galileo = 'E'; /* Estimated (dead reckoning) fix */
          }
          else   
          {
               /* Non DGPS fix */
              if (p_fix->z_NavSvInfo.u_GpsNumSvPosFix > 0)
              {
                u_GpsQual_gps     = 'A'; /* Autonomous fix */
              }
              if (p_fix->z_NavSvInfo.u_GloNumSvPosFix > 0)
              {
                u_GpsQual_glonass = 'A'; /* Autonomous fix */
              }
              if (p_fix->z_NavSvInfo.u_GalNumSvPosFix > 0)
              {
                u_GpsQual_galileo = 'A'; /* Autonomous fix */
              }
            }
          }
          }
        else
        {
          u_GpsQual_gps     = 'N'; /* Invalid fix */
          u_GpsQual_glonass = 'N'; /* Invalid fix */ 
          u_GpsQual_galileo = 'N'; /* Invalid fix */ 
        }        

        q_len = snprintf( p_b, q_buf_len, "%c%c%c,%02d,%.1f," , 
                        u_GpsQual_gps, u_GpsQual_glonass,u_GpsQual_galileo,
                        p_fix->z_NavSvInfo.u_GpsNumSvPosFix + p_fix->z_NavSvInfo.u_GloNumSvPosFix + p_fix->z_NavSvInfo.u_GalNumSvPosFix,
                        p_fix->z_NavPos.f_HDOP );

        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
      }

      /* Altitude, etc.: */
      
        /*Find the Geoidal seperation before signs are manipulated*/
    f_geoidal_sep = loc_GetGeoidalSeparation(
      p_fix->z_NavPos.d_PosLla[0] * (180.0 / C_PI), 
      p_fix->z_NavPos.d_PosLla[1] * (180.0 / C_PI) );

      q_len = snprintf( p_b, q_buf_len,"%.1lf,%.1f,," , 
                       p_fix->z_NavPos.d_PosLla[2]-f_geoidal_sep , f_geoidal_sep );

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
    /* END - Altitude */
    }/*else for if(u_no_fix)*/

  tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_GNGNS);  
  MSG_MED("Generated NMEA $GNGNS",0,0,0);

}

/*===========================================================================
FUNCTION tm_nmea_pqxfi_put

DESCRIPTION

tm_nmea_pqxfi_put() is used to build and transmit the NMEA 
Proprietary Qualcomm eXtended Fix Information message. 

Global Positioning System Extended Fix Data.

Extended fix related data for a GPS receiver.

 1     2         3       4 5        6        7 8 9  10 11  12 13 14 15  16   17
 |     |         |       | |        |        | | |  |   |   | |   | |   |    |
$PQXFI,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>

Field Number: 
1) Proprietary Qualcommm eXtended Fix Information.
2) Universal Time Coordinated (UTC) of position.
3) Latitude  (DDmm.mm)    DD = two digits of degrees, mm.mm = minutes.
4) N or S (North or South).
5) Longitude (DDDmm.mm).
6) E or W (East or West).
7) Antenna Altitude above/below mean-sea-level (geoid), meters.
8) HEPE (m, ~63% confidence, 2D horizontal error estimate = root-sum-square of 68% confidence east error estimate and north error estimate)
9) Vertical Uncertainty (m, 68% confidence, 1-sided, 1-dimensional)
10) Velocity Uncertainty (m/s, 20% confidence, 3D = root-sum-square of 68% confidence error estimates on east, north & up velocities) 
11) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_CGPS_LBS_TASK
void tm_nmea_pqxfi_put( const sm_GnssFixRptStructType *p_fix, boolean u_no_fix )
{
  char b_sentence[ 200 ];
  char *p_b = b_sentence;
  int q_buf_len = sizeof(b_sentence);
  int q_len = 0 ;                  
  boolean b_utc_valid = FALSE;
  utc_time z_utc_time;
  float f_geoidal_sep = 0.0;
  double d_Hepe = 0.0;

  if( p_fix == NULL )
  {
    tm_nmea_empty_put(b_sentence, q_buf_len, PDAPI_NMEA_PQXFI); 
    return; 
  }

    /*  Universal Time Coordinated (UTC) of position: */
  b_utc_valid = tm_util_calculate_utc(p_fix, NULL, &z_utc_time);
  if (b_utc_valid == TRUE)
    {
      q_len = snprintf( p_b, q_buf_len,"$PQXFI,%02ld%02ld%02ld.%01ld," ,
           z_utc_time.q_hours, z_utc_time.q_mins, 
           z_utc_time.q_secs, z_utc_time.q_tenths );
  }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
  /* END - UTC of position */

    if(u_no_fix)
    {
      /* Construct the 'know nothing position sentence */
      q_len = snprintf( p_b, q_buf_len,",,,,,,," );  

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
    }
    else /* if(u_no_fix) */
    {
      /* Lat, Lon, Alt, and Position Errors */
    /* Latitude and longitude*/
    q_len = tm_util_calculate_position_print(p_fix, p_b, q_buf_len);    
        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
    /*End - Lat Long */

        /* Altitude, etc.: */
    /*Find the Geoidal seperation before signs are manipulated*/
    f_geoidal_sep = loc_GetGeoidalSeparation(
      p_fix->z_NavPos.d_PosLla[0] * (180.0 / C_PI), 
      p_fix->z_NavPos.d_PosLla[1] * (180.0 / C_PI) );

        q_len = snprintf( p_b, q_buf_len,"%.1lf," , 
          p_fix->z_NavPos.d_PosLla[2]-f_geoidal_sep );

        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;

    /* END - Altitude */

        /* compute RSS of the 2d uncertainty to get HEPE */
        d_Hepe =  sqrt((p_fix->z_NavPos.f_PuncLatLonMeters[0] * 
                        p_fix->z_NavPos.f_PuncLatLonMeters[0]) + 
                       (p_fix->z_NavPos.f_PuncLatLonMeters[1] * 
                        p_fix->z_NavPos.f_PuncLatLonMeters[1]));

        /* error information */
        q_len = snprintf( p_b, q_buf_len,"%.2lf,%.2f,%.2f" ,
           d_Hepe,
           p_fix->z_NavPos.f_PuncVertMeters,
           p_fix->z_NavPos.f_Vunc3dMps);

        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;

    }/*else for if(u_no_fix)*/

  tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_PQXFI);  
  MSG_MED("Generated NMEA $PQXFI",0,0,0);
}
#endif /* FEATURE_CGPS_LBS_TASK */
/*===========================================================================
FUNCTION tm_nmea_gagsv_put

DESCRIPTION
  tm_nmea_gpgsv_put() is used to transmit the GSV - Galileo Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15
  | |   | | |  |  |  |   |       |      |  |  |   |  | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 3.
  4) Message number, 1 to 3.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number.
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     Galileo satelites are identified by their PRN numbers, ranging 1 to 36.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_gagsv_put(const sm_GpsMeasRptStructType * p_meas)
{
  char b_sentence[200],*p_b;
  uint8 u_i, u_msg, u_total_messages,u_sv_i, u_total_sv, u_valid_svs;
  const gnss_MeasStructType *p_meas_report = NULL;
  int q_buf_len = sizeof(b_sentence),q_len = 0 ;                  

  if(p_meas == NULL)
  {
    return;
  }

  /*If the source of measurement is not GPS then build the standard empty string*/
  if(p_meas->z_MeasBlk.e_MeasBlkSrc !=  GNSS_MEAS_BLK_SRC_GAL)
  {  
    return;    
  }
  
  p_meas_report = p_meas->z_MeasBlk.z_Gnss;

  u_total_sv = p_meas->z_MeasBlk.u_NumSvs;
  
  if(u_total_sv > N_SV) 
  {
    u_total_sv = N_SV;
  }
      
  /*for(u_sv_i=0, u_valid_svs=0; u_sv_i<u_total_sv ; u_sv_i++)
  {
    if (p_meas_report[u_sv_i].u_SvDirValid && (p_meas_report[u_sv_i].f_Elevation > 0))  
    {
      u_valid_svs++;    
    }
  } */

  
  u_valid_svs = u_total_sv;
  /* Restrict the number of SVs per message to 4 (length limitation) */
  /* We can send multiple messages */
  u_total_messages = u_valid_svs >> 2;
  if(u_valid_svs != (u_total_messages << 2))
  {
    u_total_messages++;
  }
  
  for (u_sv_i=0, u_msg=1; u_sv_i < u_total_sv; u_msg++) 
  {
    /* Total number of messages, message number, total number of 
     * satellites in view. 
    */
    p_b = b_sentence;
    q_buf_len = sizeof(b_sentence);
    q_len = 0;                  
    
    q_len = snprintf( p_b, q_buf_len, "$GAGSV,%d,%d,%02d", 
                        u_total_messages , u_msg , u_valid_svs );

    if(q_len < 0 || q_len >= q_buf_len)
    {
      MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
      return;
    }

    p_b += q_len;
    q_buf_len -= q_len;

    u_i = 0;    /* We'll have up to four SVs in the message. */

    while ((u_sv_i < u_total_sv) && (u_i < 4))
    {
      u_i++;

      if( ( GAL_ID_OK(p_meas_report->w_Sv) ||
          (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED))
        )
      {      
        q_len = snprintf( p_b , q_buf_len,",%02d",p_meas_report->w_Sv-GAL_SV_ID_OFFSET); //Remove offset, range 1-36
      }
      else
      {
          q_len = snprintf( p_b , q_buf_len,"," );
      }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
      
      if (p_meas_report->u_SvDirValid && (p_meas_report->f_Elevation > 0))
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          float x_elev;
          float l_azim;

          x_elev = ((float)p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
          l_azim = ((float)p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */


          /*elevation, azimuth  */
          q_len = snprintf( p_b , q_buf_len,",%.1f,%.1f,",
                                      x_elev,
                                      l_azim );  
        }
        else
        {
          int16 x_elev;
          uint32 l_azim;

        x_elev = (p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
        l_azim = (p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */

        
        /*elevation, azimuth  */
        q_len = snprintf( p_b , q_buf_len,",%02d,%03ld,",
                                    x_elev,
                                    l_azim );                      
        }

      }
      else
      {
        /* NULL elevation, NULL azimuth*/
        q_len = snprintf( p_b , q_buf_len,",,," );
      } 

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
      
      p_b += q_len;
      q_buf_len -= q_len;

      if (p_meas_report->w_Cno)     /* Tracked. */
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          float cno_report = ((float)(p_meas_report->w_Cno) + (float) (p_meas->z_MeasBlk.z_Gnss[0].w_RfLoss)) / 10;
          
          q_len = snprintf( p_b , q_buf_len,"%.1f" , cno_report);
        }
        else
        {
        /* Add 5 to round up */
          uint16 cno_report = (p_meas_report->w_Cno + p_meas->z_MeasBlk.z_Gnss[0].w_RfLoss+5)/10;
          
          q_len = snprintf( p_b , q_buf_len,"%d" , cno_report);
        }
        
        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
      }
    
      u_sv_i++;
      p_meas_report++;          
    }

    tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_GAGSV);  
    MSG_MED("Generated NMEA $GAGSV",0,0,0);
  }    
}

/*===========================================================================
FUNCTION tm_nmea_gpgsv_put

DESCRIPTION
  tm_nmea_gpgsv_put() is used to transmit the GSV - GPS and SBAS Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15
  | |   | | |  |  |  |   |       |      |  |  |   |  | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 4.
  4) Message number, 1 to 4.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number.
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     GPS satelites are identified by their PRN numbers, ranging 1 to 32.
     SBAS satellites are identified by their PRN numbers, ranging 33 to 64.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_gpgsv_put(const sm_GpsMeasRptStructType * p_meas_gps, const sm_GpsMeasRptStructType * p_meas_sbas)
{
  char b_sentence[200],*p_b;
  uint8 u_i, u_msg, u_total_messages,u_sv_i, u_total_sv=0,u_total_sv_gps=0,u_total_sv_sbas=0, u_valid_svs=0;
  const gnss_MeasStructType *p_meas_report = NULL;
  int q_buf_len = sizeof(b_sentence),q_len = 0; 
  const sm_GpsMeasRptStructType *p_meas = NULL;
  boolean v_sbas_range = FALSE;
  
  if((NULL == p_meas_gps) && (NULL == p_meas_sbas))
  {
    /*should not happen, only one of the parameters can be NULL, not both*/
    MSG_ERROR("NMEA: Null parameters",0,0,0);  
    return;
  }

  if(p_meas_gps != NULL && (GNSS_MEAS_BLK_SRC_GPS == p_meas_gps->z_MeasBlk.e_MeasBlkSrc))
  {
      u_total_sv_gps = p_meas_gps->z_MeasBlk.u_NumSvs;
  }

  if((p_meas_sbas != NULL) && (GNSS_MEAS_BLK_SRC_QZSS_SBAS == p_meas_sbas->z_MeasBlk.e_MeasBlkSrc))
  {
      u_total_sv_sbas = p_meas_sbas->z_MeasBlk.u_NumSvs;
  }

  if(u_total_sv_gps > N_ACTIVE_CHAN) 
  {
    /* this should not ever happen*/
    MSG_ERROR("NMEA: Too many GPS SVs (%u) reported in GPS measurement block",u_total_sv_gps,0,0);  
    u_total_sv_gps = N_ACTIVE_CHAN;
  }

  if(u_total_sv_sbas > N_ACTIVE_CHAN) 
  {
    /* this should not ever happen*/
    MSG_ERROR("NMEA: Too many SBAS SVs (%u) reported in QZSS/SBAS measurement block",u_total_sv_sbas,0,0);  
    u_total_sv_sbas = N_ACTIVE_CHAN;
  }

  u_total_sv = u_total_sv_gps+u_total_sv_sbas;

  /*Nothing to do, build the standard empty string*/
  if(u_total_sv == 0)
  {  
    return;    
  }
  else
  {
    MSG_LOW("Generate NMEA $GPGSV with %u GPS and %u SBAS SVs",u_total_sv_gps,u_total_sv_sbas,0);
  }
    
  if(u_total_sv_gps > 0)
  {
     p_meas_report = p_meas_gps->z_MeasBlk.z_Gnss;
     p_meas = p_meas_gps;
  }
  else if(u_total_sv_sbas > 0)
  {
     p_meas_report = p_meas_sbas->z_MeasBlk.z_Gnss;
     p_meas = p_meas_sbas;
  }
   
  u_valid_svs = u_total_sv;
  /* Restrict the number of SVs per message to 4 (length limitation) */
  /* We can send multiple messages */
  u_total_messages = u_valid_svs >> 2;
  if(u_valid_svs != (u_total_messages << 2))
  {
    u_total_messages++;
  }
  
  for (u_sv_i=0, u_msg=1; u_sv_i < u_total_sv; u_msg++) 
  {
    /* Total number of messages, message number, total number of 
     * satellites in view. 
    */
    p_b = b_sentence;
    q_buf_len = sizeof(b_sentence);
    q_len = 0;                  
    
    q_len = snprintf( p_b, q_buf_len, "$GPGSV,%d,%d,%02d", 
                        u_total_messages , u_msg , u_valid_svs );

    if(q_len < 0 || q_len >= q_buf_len)
    {
      MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
      return;
    }

    p_b += q_len;
    q_buf_len -= q_len;

    u_i = 0;    /* We'll have up to four SVs in the message. */

    while ((u_sv_i < u_total_sv) && (u_i < 4))
    {
      u_i++;
    
#define TM_CORE_2_BIT_SVID_MAX 99

      if( (p_meas_report->w_Sv <= TM_CORE_2_BIT_SVID_MAX) ||
          (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        )
      {      
        q_len = snprintf( p_b , q_buf_len,",%02d",p_meas_report->w_Sv);
      }
      else
      {
          q_len = snprintf( p_b , q_buf_len,"," );
      }

        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }
        
        p_b += q_len;
        q_buf_len -= q_len;

      if (p_meas_report->u_SvDirValid && (p_meas_report->f_Elevation > 0))
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          float x_elev;
          float l_azim;

          x_elev = ((float)p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
          l_azim = ((float)p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */


          /*elevation, azimuth  */
          q_len = snprintf( p_b , q_buf_len,",%.1f,%.1f,",
                                      x_elev,
                                      l_azim );  
        }
        else
        {
          int16 x_elev;
          uint32 l_azim;

        x_elev = (p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
        l_azim = (p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */

        
        /*elevation, azimuth  */
        q_len = snprintf( p_b , q_buf_len,",%02d,%03ld,",
                                    x_elev,
                                    l_azim );                      
      }

      }
      else
      {
        /* NULL elevation, NULL azimuth*/
        q_len = snprintf( p_b , q_buf_len,",,," );
      } 

        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
    
      if (p_meas_report->w_Cno)     /* Tracked. */
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          float cno_report = ((float)(p_meas_report->w_Cno) + (float) (p_meas->z_MeasBlk.z_Gnss[0].w_RfLoss)) / 10;
          
          q_len = snprintf( p_b , q_buf_len,"%.1f" , cno_report);
      } 
        else
      {
        /* Add 5 to round up */
          uint16 cno_report = (p_meas_report->w_Cno + p_meas->z_MeasBlk.z_Gnss[0].w_RfLoss+5)/10;
          
          q_len = snprintf( p_b , q_buf_len,"%d" , cno_report);
        }
        
        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
      }
    
      u_sv_i++;
      p_meas_report++; 
      
      if ( (u_total_sv_gps > 0) && (u_total_sv_sbas > 0) && (u_sv_i>=u_total_sv_gps)
           && (FALSE == v_sbas_range))
      {
        /*continue from SBAS data*/
        v_sbas_range = TRUE;
        p_meas_report = p_meas_sbas->z_MeasBlk.z_Gnss;
        p_meas = p_meas_sbas;
      }
    }/*while ((u_sv_i < u_total_sv) && (u_i < 4))*/

    tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_GPGSV);  
    MSG_MED("Generated NMEA $GPGSV",0,0,0);
  }    
}

/*===========================================================================
FUNCTION tm_nmea_pqgsv_put

DESCRIPTION
  tm_nmea_gpgsv_put() is used to transmit the GSV - GNSS Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15  16
  | |   | | |  |  |  |   |       |       |  |  |   |  |   | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx,h,h*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 20.
  4) Message number, 1 to 20.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number. (3 digits, proprietary)
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Signal ID (0 for all signals) 
 16  GNSS System ID (BDS:4, QZSS:5)
 16) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     BDS and QZSS satelites are identified by their PRN numbers. BDS: 1-37 (offset -200), QZSS: 1-4 (offset -192)

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
static void tm_nmea_pqgsv_put_internal(const sm_GpsMeasRptStructType * p_meas)
{
  char b_sentence[200],*p_b;
  uint8 u_i, u_msg, u_total_messages,u_sv_i, u_total_sv, u_valid_svs,u_gnss_system_id,u_sv_offset=0;
  const gnss_MeasStructType *p_meas_report = NULL;
  int q_buf_len = sizeof(b_sentence),q_len = 0 ;                  

  if(NULL == p_meas)
  {
    MSG_ERROR("Null pointer argument",0,0,0);
    return;
  }
  else
  {
    p_meas_report = p_meas->z_MeasBlk.z_Gnss;
    u_total_sv = p_meas->z_MeasBlk.u_NumSvs;
  }

  /*If the source of measurement is not BDS or QZSS  then build the standard empty string*/
  if(p_meas->z_MeasBlk.e_MeasBlkSrc ==  GNSS_MEAS_BLK_SRC_BDS)
  {
    if(u_total_sv > N_BDS_SV) 
    {
      u_total_sv = N_BDS_SV;
    }
    u_gnss_system_id = (uint8)TM_NMEA_SYSTEM_ID_BDS;
    u_sv_offset = PDSM_PD_SV_ID_BDS_MIN-1;
  }
  else if(p_meas->z_MeasBlk.e_MeasBlkSrc ==  GNSS_MEAS_BLK_SRC_QZSS_SBAS)
  {
    if(u_total_sv > N_QZSS_SV) 
    {
      u_total_sv = N_QZSS_SV;
    }
    u_gnss_system_id = (uint8)TM_NMEA_SYSTEM_ID_QZSS;
    u_sv_offset = PDSM_PD_SV_ID_QZSS_MIN-1;
  }
  else
  {
    return;
  }

  u_valid_svs = u_total_sv;

  /* Restrict the number of SVs per message to 4 (length limitation) */
  /* We can send multiple messages */
  u_total_messages = u_valid_svs / 4;
  if(u_valid_svs != (u_total_messages * 4))
  {
    u_total_messages++;
  }
  
  for (u_sv_i=0, u_msg=1; u_sv_i < u_total_sv; u_msg++) 
  {
    /* Total number of messages, message number, total number of 
     * satellites in view. 
    */
    p_b = b_sentence;
    q_buf_len = sizeof(b_sentence);
    q_len = 0;                  
    
    q_len = snprintf( p_b, q_buf_len, "$PQGSV,%d,%d,%02d", 
                        u_total_messages , u_msg , u_valid_svs );

    if(q_len < 0 || q_len >= q_buf_len)
    {
      MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
      return;
    }

    p_b += q_len;
    q_buf_len -= q_len;

    u_i = 0;    /* We'll have up to four SVs in the message. */

    while ((u_sv_i < u_total_sv) && (u_i < 4))
    {
      u_i++;
    
      if( (p_meas_report->w_Sv >= PDSM_PD_SV_ID_BDS_MIN  && p_meas_report->w_Sv <= PDSM_PD_SV_ID_BDS_MAX)||
          (p_meas_report->w_Sv >= PDSM_PD_SV_ID_QZSS_MIN && p_meas_report->w_Sv <= PDSM_PD_SV_ID_QZSS_MAX)    
         )         
      {      
        q_len = snprintf( p_b , q_buf_len,",%02d",p_meas_report->w_Sv-u_sv_offset);
      }
      else
      {
        MSG_ERROR("NMEA: Invalid PRN Range (prn=%d)",p_meas_report->w_Sv,0,0);  
        q_len = snprintf( p_b , q_buf_len,"," );
      }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
        
        p_b += q_len;
        q_buf_len -= q_len;

      if (p_meas_report->u_SvDirValid && (p_meas_report->f_Elevation > 0))
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          float x_elev;
          float l_azim;
          x_elev = ((float)p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
          l_azim = ((float)p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */
      
          /*elevation, azimuth  */
          q_len = snprintf( p_b , q_buf_len,",%.1f,%.1f,",
                               x_elev,
                               l_azim );  
        }
        else
        {
          int16 x_elev;
          uint32 l_azim;

          x_elev = (p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
          l_azim = (p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */
        
            /*elevation, azimuth  */
          q_len = snprintf( p_b , q_buf_len,",%02d,%03ld,",
                                      x_elev, l_azim );                      
        }
      }
      else
      {
        /* NULL elevation, NULL azimuth*/
        q_len = snprintf( p_b , q_buf_len,",,," );
      } 

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
    
      if (p_meas_report->w_Cno)     /* Tracked. */
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          float cno_report = ((float)(p_meas_report->w_Cno) + (float) (p_meas->z_MeasBlk.z_Gnss[0].w_RfLoss)) / 10;
          q_len = snprintf( p_b , q_buf_len,"%.1f" , cno_report);
        } 
        else
        {
          /* Add 5 to round up */
          uint16 cno_report = (p_meas_report->w_Cno + p_meas->z_MeasBlk.z_Gnss[0].w_RfLoss+5)/10;     
          q_len = snprintf( p_b , q_buf_len,"%d" , cno_report);
        }
        
        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
      }
    
      u_sv_i++;
      p_meas_report++;          
    }/* while() */

    q_len = snprintf( p_b , q_buf_len,",%d,%d" , 0,u_gnss_system_id);

    if(q_len < 0 || q_len >= q_buf_len)
    {
       MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
       return;
    }
   
    p_b += q_len;
    q_buf_len -= q_len;

    tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_PQGSV);  
    MSG_MED("Generated NMEA $PQGSV",0,0,0);
  }/*for (u_sv_i=0, u_msg=1; u_sv_i < u_total_sv; u_msg++) */    
}

/*===========================================================================
FUNCTION tm_nmea_pqgsv_put

DESCRIPTION
  tm_nmea_gpgsv_put() is used to transmit the GSV - GNSS Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15  16
  | |   | | |  |  |  |   |       |       |  |  |   |  |   | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx,h,h*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 20.
  4) Message number, 1 to 20.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number. (3 digits, proprietary)
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Signal ID (0 for all signals) 
 16  GNSS System ID (BDS:4, QZSS:5)
 16) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     BDS and QZSS satelites are identified by their PRN numbers. BDS: 1-37 (offset -200), QZSS: 1-4 (offset -192)

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_pqgsv_put(const sm_GpsMeasRptStructType * p_meas)
{
  if(NULL == p_meas)
  {
    MSG_ERROR("Null pointer argument",0,0,0);
    return;
  }
  else
  {
    if(GNSS_MEAS_BLK_SRC_BDS == p_meas->z_MeasBlk.e_MeasBlkSrc)
    {
      tm_nmea_pqgsv_put_internal(p_meas);
    }
    else if(GNSS_MEAS_BLK_SRC_QZSS_SBAS == p_meas->z_MeasBlk.e_MeasBlkSrc)
    {
      sm_GpsMeasRptStructType meas_copy;
      /*remove SBAS prns*/
      memscpy((void*)&meas_copy,sizeof(meas_copy),(void*)p_meas,sizeof(sm_GpsMeasRptStructType));
      tm_util_clean_qzss_sbas_meas_rpt(&meas_copy,FALSE);
      tm_nmea_pqgsv_put_internal(&meas_copy);
    }
  }
}

/*===========================================================================
FUNCTION tm_nmea_glgsv_put

DESCRIPTION
  tm_nmea_glgsv_put() is used to transmit the GSV - GPS Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15
  | |   | | |  |  |  |   |       |      |  |  |   |  | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 3.
  4) Message number, 1 to 3.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number.
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     GPS satelites are identified by their PRN numbers, ranging 1 to 32.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_glgsv_put(const sm_GpsMeasRptStructType * p_meas)
{
  char b_sentence[200],*p_b;
  uint8 u_i, u_msg, u_total_messages,u_sv_i, u_total_sv, u_valid_svs;
  const gnss_MeasStructType *p_meas_report = NULL;
  int q_buf_len = sizeof(b_sentence),q_len = 0 ;                  

  if(p_meas == NULL)
  {
    return;
  }

  /*If the source of measurement is not Glonass then build the standard empty string*/
  if(p_meas->z_MeasBlk.e_MeasBlkSrc !=  GNSS_MEAS_BLK_SRC_GLO)
  {
    return;    
  }
  
  p_meas_report = p_meas->z_MeasBlk.z_Gnss;

  u_total_sv = p_meas->z_MeasBlk.u_NumSvs;
  
  if(u_total_sv > N_SV) 
  {
    u_total_sv = N_SV;
  }
        
  u_valid_svs = u_total_sv;
  /* Restrict the number of SVs per message to 4 (length limitation) */
  /* We can send multiple messages */
  u_total_messages = u_valid_svs >> 2;
  if(u_valid_svs != (u_total_messages << 2))
  {
    u_total_messages++;
  }
  
  for (u_sv_i=0, u_msg=1; u_sv_i < u_total_sv; u_msg++) 
  {
    /* Total number of messages, message number, total number of 
     * satellites in view. 
    */
    p_b = b_sentence;
    q_buf_len = sizeof(b_sentence);
    q_len = 0;                  
    
    q_len = snprintf( p_b, q_buf_len, "$GLGSV,%d,%d,%02d", 
                        u_total_messages , u_msg , u_valid_svs );

    if(q_len < 0 || q_len >= q_buf_len)
    {
      MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
      return;
    }

    p_b += q_len;
    q_buf_len -= q_len;

    u_i = 0;    /* We'll have up to four SVs in the message. */

    while ((u_sv_i < u_total_sv) && (u_i < 4))
    {
      u_i++;
      
      if(p_meas_report->w_Sv == GLO_SLOTID_UNKNOWN)
      {          
        q_len = snprintf( p_b , q_buf_len,",,");
      }
      else
      {
        
        q_len = snprintf( p_b , q_buf_len,",%02d,",
                                    p_meas_report->w_Sv);
      }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
    
      p_b += q_len;
      q_buf_len -= q_len;
    
      if (p_meas_report->u_SvDirValid && (p_meas_report->f_Elevation > 0))
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          float x_elev;
          float l_azim;
        
          x_elev = ((float)p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
          l_azim = ((float)p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */
        
        
          /*elevation, azimuth  */
          q_len = snprintf( p_b , q_buf_len,"%.1f,%.1f,",
                                      x_elev,
                                      l_azim );                      
        }        
        else
        {
          int16 x_elev;
          uint32 l_azim;
        
        x_elev = (p_meas_report->f_Elevation * 180.0)  / C_PI; /*lint !e524 */
        l_azim = (p_meas_report->f_Azimuth   * 180.0) / C_PI; /*lint !e524 */


          /*elevation, azimuth  */
        q_len = snprintf( p_b , q_buf_len,"%02d,%03ld,",
                                    x_elev,
                                    l_azim );
      }
      }
      else
      {
        /* NULL elevation, NULL azimuth*/
        q_len = snprintf( p_b , q_buf_len,",,");
    
      } 
      
      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
      
      p_b += q_len;
      q_buf_len -= q_len;

      if (p_meas_report->w_Cno)     /* Tracked. */
      {
        if(TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GSV_EXTENDED)
        {
          /*Use RfLoss specific to GLO frequency*/
          float cno_report = ((float)(p_meas_report->w_Cno) + (float) (p_meas_report->w_RfLoss)) / 10;
          
          q_len = snprintf( p_b , q_buf_len,"%.1f" , cno_report);
        }
        else
        {
        /* Add 5 to round up */
        /*Use RfLoss specific to GLO frequency*/
          uint16 cno_report = (p_meas_report->w_Cno + p_meas_report->w_RfLoss+5)/10;
          
          q_len = snprintf( p_b , q_buf_len,"%d" , cno_report);
        }
        
        if(q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }

        p_b += q_len;
        q_buf_len -= q_len;
      }
    
      u_sv_i++;
      p_meas_report++;          
    }

    tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_GLGSV);  
    MSG_MED("Generated NMEA $GLGSV",0,0,0);
  }    
}


   
 

/*===========================================================================
FUNCTION tm_nmea_gpvtg_put

DESCRIPTION
  tm_nmea_gpvtg_put() is used to transmit VTG - Course Over Ground and Ground Speed.
     
    The actual course and speed relative to the ground.

  1 2   3   4 5   6 7   8 9  10 11
  | |   |   | |   | |   | |   | |
 $aaccc,x.x,T,x.x,M,x.x,N,x.x,K*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Course, degrees.
  4) T = True.
  5) Course, degrees.
  6) M = Magnetic.
  7) Speed, knots.
  8) N = Knots.
  9) Speed Kilometers Per Hour.
 10) K = Kilometers Per Hour.
 11) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_gpvtg_put( const sm_GnssFixRptStructType *p_fix )
{
  char b_sentence[200], *p_b = b_sentence;
  double  d_true_course, d_azimuth, d_speed, d_mag_course;
  double  d_north_vel, d_east_vel;
  int q_buf_len = sizeof(b_sentence),q_len = 0 ;                  
  
  if ( (p_fix == NULL) ||
      (!p_fix->z_NavPos.z_PosFlags.b_IsValid) ||  /* Position invalid */
      (!p_fix->z_NavPos.z_PosFlags.b_IsVelValid) )
  {
    tm_nmea_empty_put(b_sentence, q_buf_len, PDAPI_NMEA_GPVTG);
    return;
  }
    d_east_vel  = p_fix->z_NavPos.f_VelEnu[0];
    d_north_vel = p_fix->z_NavPos.f_VelEnu[1];

    d_speed = sqrt( d_north_vel * d_north_vel + d_east_vel * d_east_vel );

    /* Azimuth moves from north (0) to east (90), wrapping around.
     * Note that we are ignoring the vertical velocity.
     * Also, default azimuth to 0 if there is no North/East velocity
     */

    if(p_fix->z_NavPos.z_PosFlags.b_IsHeadingValid) 
    {
      d_azimuth = p_fix->z_NavPos.f_HeadingRad * (180.0 / C_PI);  
      d_true_course = d_azimuth;
      q_len = snprintf( p_b, q_buf_len,"$GPVTG,%.1lf,T,",d_true_course);
    }
    else
    {
      d_azimuth = 0.0;
      q_len = snprintf( p_b, q_buf_len,"$GPVTG,,T,");
    }
    

    if (q_len < 0 || q_len >= q_buf_len)
    {
      MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
      return;
    }

    p_b += q_len;
    q_buf_len -= q_len;

    /* magnetic deviation. */
    if (p_fix->z_NavPos.u_MagDevGood == TRUE)
  {
        d_mag_course = d_azimuth - p_fix->z_NavPos.f_MagDeviation;
        if (d_mag_course < 0.0) 
        {
          d_mag_course += 360.0;
        }
        else if (d_mag_course > 360.0) 
        {
          d_mag_course -= 360.0;
        }
  }
  else
  {
    d_mag_course = d_azimuth;
  }    

    {
      char u_ModeIndicator = 'N';
      
      if (tm_nmea_is_differential_fix(p_fix))
      {
        u_ModeIndicator = 'D'; /* Differential (DGPS) */
      }
      else
      {
        if (tm_nmea_is_propagated_fix(p_fix) ||
            ((p_fix->z_NavSvInfo.u_GpsNumSvPosFix == 0) &&
             (p_fix->z_NavSvInfo.u_GloNumSvPosFix == 0) &&
             (p_fix->z_NavPos.z_PosFlags.b_IsInsOnlySln)) )
      {
          u_ModeIndicator = 'E'; /* Estimated (dead reckoning) */
      }
      else
      {
          u_ModeIndicator = 'A'; /* Autonomous (Non DGPS) */
        }
      }

      if (d_speed < TM_CORE_MIN_VELOCTY)
      {
        /* If Speed < 0.15 m/sec e.g NF Heading Filter value of 0.1m/s, zero the NMEA speed report */
        d_speed = 0;
      }
    
      q_len = snprintf( p_b, q_buf_len,"%.1lf,M,%.1lf,N,%.1lf,K,%c",
                              d_mag_course,                /* Degrees. */
                              d_speed * (3600.0/1852.0),   /* Knots */
                              d_speed * 3.6,               /* Km / hour */
                              u_ModeIndicator);

    if (q_len < 0 || q_len >= q_buf_len)
    {
      MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
      return;
    }

    p_b += q_len;
    q_buf_len -= q_len;
    }
  tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_GPVTG);  
  MSG_MED("Generated NMEA $GPVTG",0,0,0);
}

/*===========================================================================
FUNCTION tm_nmea_gsa_put

DESCRIPTION
  tm_nmea_gsa_put() is used to transmit GPGSA OR GNGSA
     

  1 2   3 4 5                                   6   7   8    9  10
  | |   | | |                                   |   |   |    |  |
 $aaccc,d,e,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x,[h]*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Mode, M = Manual, A = Automatic  2D/3D.
  4) Mode, 1 = No fix, 2 = 2D, 3 = 3D.
  5) PRN used in the position solution.  Using "null" (",") for no SV for
     a total of 12 tokens.
  6) PDOP - Position Dilution Of Precision.
  7) HDOP - Horizontal Dilution Of Precision.
  8) VDOP - Vertical Dilution Of Precision.
  9) [GNSS System ID for PQGSA only. BDS:4 QZSS:5]
  10) Checksum.

DEPENDENCIES

  In case of PQGSA the pdsm_pa_nmea_type contains also gnss system id
  TM_NMEA_SYSTEM_ID_BDS or TM_NMEA_SYSTEM_ID_QZSS. This is needed for
  setting the GNSS System ID field (9) correctly for empty PQGSV messages.

RETURN VALUE none

SIDE EFFECTS 
===========================================================================*/

static void tm_nmea_gsa_put( const sm_GnssFixRptStructType *p_fix, const uint8 num_svs, 
                      const uint16 svs[], pdsm_pa_nmea_type e_nmea_type, tm_nmea_system_id_e_type e_system)
{
  char b_sentence[200],b_auto, *p_b = b_sentence;
  uint8  u_i, u_mode,u_gnss_system_id=(uint8)TM_NMEA_SYSTEM_ID_DEFAULT, u_sv_offset=0;
  int q_buf_len = sizeof(b_sentence),q_len = 0 ;                  
  
  b_auto = 'A';

  if(e_nmea_type == PDAPI_NMEA_GNGSA)
  {
    q_len = snprintf( p_b , q_buf_len, "$GNGSA,");      
  }
  else if(e_nmea_type == PDAPI_NMEA_GPGSA)
  {
    q_len = snprintf( p_b , q_buf_len, "$GPGSA,");
  }
  else if((e_nmea_type == (PDAPI_NMEA_PQGSA+TM_NMEA_SYSTEM_ID_BDS) ||
          (e_nmea_type == (PDAPI_NMEA_PQGSA+TM_NMEA_SYSTEM_ID_QZSS)))) 
  {
    q_len = snprintf( p_b , q_buf_len, "$PQGSA,");   
  
    u_gnss_system_id = (uint8)((pdsm_pa_nmea_type)e_nmea_type-PDAPI_NMEA_PQGSA);
 
    if((uint8)TM_NMEA_SYSTEM_ID_BDS == u_gnss_system_id)
    {
        u_sv_offset = PDSM_PD_SV_ID_BDS_MIN-1;        
    }
    else if((uint8)TM_NMEA_SYSTEM_ID_QZSS == u_gnss_system_id)
    {
        u_sv_offset = PDSM_PD_SV_ID_QZSS_MIN-1;       
    }
  }
  else
  {
    MSG_ERROR("NMEA: Sentence type not expected in GSA",0,0,0);
    return;
  }

  if(q_len < 0 || q_len >= q_buf_len)
  {
    MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
    return;
  }

  p_b += q_len;
  q_buf_len -= q_len;

  /* 2D or 3D. */
  if(p_fix->z_NavPos.z_PosFlags.b_Is3D)
  {
    u_mode = 3;
  }
  else
  {
    u_mode = 2;
  }

  q_len = snprintf( p_b , q_buf_len, "%c,%d," , b_auto, u_mode );

  if(q_len < 0 || q_len >= q_buf_len)
  {
    MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
    return;
  }

  p_b += q_len;
  q_buf_len -= q_len;

  for (u_i=0; u_i < 12; u_i++)
  {
    if (u_i < num_svs)
    {
      uint16 q_svid = svs[u_i];

      if (GAL_ID_OK(q_svid))
      {
          q_svid=q_svid-GAL_SV_ID_OFFSET;
      }
      q_len = snprintf( p_b , q_buf_len,"%02d," ,
                          svs[u_i]-u_sv_offset );
      
      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
    }
    else    /* Filler for this record. */
    {
      q_len = snprintf( p_b , q_buf_len, "," );
      
      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
      
      p_b += q_len;
      q_buf_len -= q_len;
    }
  }
  /* NMEA v4.10 GNSS System ID field */
  if((e_system>TM_NMEA_SYSTEM_ID_DEFAULT) && (e_system<TM_NMEA_SYSTEM_ID_LAST) && (e_nmea_type == PDAPI_NMEA_GNGSA))
  {
     q_len = snprintf( p_b , q_buf_len, "%.1f,%.1f,%.1f,%d",
                          p_fix->z_NavPos.f_PDOP, 
                          p_fix->z_NavPos.f_HDOP, 
                          p_fix->z_NavPos.f_VDOP,e_system );
  }
  else
  {

     q_len = snprintf( p_b , q_buf_len, "%.1f,%.1f,%.1f",
                          p_fix->z_NavPos.f_PDOP, 
                          p_fix->z_NavPos.f_HDOP, 
                          p_fix->z_NavPos.f_VDOP );
  }

  if(q_len < 0 || q_len >= q_buf_len)
  {
    MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
    return;
  }
  p_b += q_len;
  q_buf_len -= q_len;

  //GNSS ID for PQGSA
  if (u_gnss_system_id != (uint8)TM_NMEA_SYSTEM_ID_DEFAULT)
  {
      q_len = snprintf( p_b , q_buf_len,",%d" ,
                          u_gnss_system_id);
      
      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }

      p_b += q_len;
      q_buf_len -= q_len;
  }

  tm_nmea_terminate_and_put(b_sentence, e_nmea_type);  
}

/*===========================================================================
FUNCTION tm_nmea_gpgsa_put

DESCRIPTION
  tm_nmea_gpsa_put() is used to transmit GSA - GPS/Glonass and Active Satellites.
     

  1 2   3 4 5                   6   7   8   9
  | |   | | |                                   |   |   |   |
 $aaccc,d,e,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Mode, M = Manual, A = Automatic  2D/3D.
  4) Mode, 1 = No fix, 2 = 2D, 3 = 3D.
  5) PRN used in the position solution.  Using "null" (",") for no SV for
     a total of 12 tokens.
  6) PDOP - Position Dilution Of Precision.
  7) HDOP - Horizontal Dilution Of Precision.
  8) VDOP - Vertical Dilution Of Precision.
  9) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

 Notes: If we send a 2D position to Street Altas, we will get a blinking 
 question mark sitting on the satellite icon within the 'GPS Status' window.
 I assume the program thinks that we've lost too many SVs and we're now
 in 2D mode.  We only set the 2D mode if we only have 3 SVs with an altitude-
 hold meaurement.  In 2D mode, the positions on the map will be in yellow.
 
===========================================================================*/
void tm_nmea_gpgsa_put( const sm_GnssFixRptStructType *p_fix )
{
  char b_sentence[200];
  
  if ( ( p_fix == NULL ) ||
       (!p_fix->z_NavPos.z_PosFlags.b_IsValid) ||  /* Position invalid */
       ( p_fix->z_NavSvInfo.u_GpsNumSvPosFix == 0 ) /* No GPS SV's used */ )
  {
    tm_nmea_empty_put(b_sentence, (int)sizeof(b_sentence), PDAPI_NMEA_GPGSA);
    return;
  }
  else
  {
    uint16 z_SvIDs[N_ACTIVE_GPS_CHAN];

    sm_generate_sv_array_from_svinfo_struct(z_SvIDs, 
                                            ARR_SIZE(z_SvIDs),
                                            p_fix->z_NavSvInfo.z_GpsSvInfo,
                                            ARR_SIZE(p_fix->z_NavSvInfo.z_GpsSvInfo),
                                            PDSM_GNSS_SV_INFO_USED_IN_POS_FIX);

    tm_nmea_gsa_put(p_fix, p_fix->z_NavSvInfo.u_GpsNumSvPosFix, z_SvIDs, PDAPI_NMEA_GPGSA,TM_NMEA_SYSTEM_ID_DEFAULT);
  }
  MSG_MED("Generated NMEA $GPGSA",0,0,0);
}

/*===========================================================================
FUNCTION tm_nmea_gngsa_put

DESCRIPTION
  tm_nmea_gngsa_put() is used to transmit GSA - GPS/Glonass DOP and Active Satellites.
     

  1 2   3 4 5                                   6   7   8   9 10
  | |   | | |                                   |   |   |   |  |
 $aaccc,d,e,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x,h*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Mode, M = Manual, A = Automatic  2D/3D.
  4) Mode, 1 = No fix, 2 = 2D, 3 = 3D.
  5) PRN used in the position solution.  Using "null" (",") for no SV for
     a total of 12 tokens.
  6) PDOP - Position Dilution Of Precision.
  7) HDOP - Horizontal Dilution Of Precision.
  8) VDOP - Vertical Dilution Of Precision.
  9) GNSS System ID (GPS = 1, GLONASS = 2, GALILEO = 3, RESERVED = {4..F}
  10) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

 Notes: If we send a 2D position to Street Altas, we will get a blinking 
 question mark sitting on the satellite icon within the 'GPS Status' window.
 I assume the program thinks that we've lost too many SVs and we're now
 in 2D mode.  We only set the 2D mode if we only have 3 SVs with an altitude-
 hold meaurement.  In 2D mode, the positions on the map will be in yellow.
 
===========================================================================*/
void tm_nmea_gngsa_put( const sm_GnssFixRptStructType *p_fix )
{
  char b_sentence[200];
  uint16 z_SvIDs[MAX_ACTIVE_GNSS_CHAN];

  if ( ( p_fix == NULL ) ||
       (!p_fix->z_NavPos.z_PosFlags.b_IsValid) ) /* Position invalid */
  {
    tm_nmea_empty_put(b_sentence, (int)sizeof(b_sentence), PDAPI_NMEA_GNGSA);
    return;
  }

    sm_generate_sv_array_from_svinfo_struct(z_SvIDs,
                                            ARR_SIZE(z_SvIDs),
                                            p_fix->z_NavSvInfo.z_GpsSvInfo,
                                            ARR_SIZE(p_fix->z_NavSvInfo.z_GpsSvInfo),
                                            PDSM_GNSS_SV_INFO_USED_IN_POS_FIX);
    tm_nmea_gsa_put(p_fix, p_fix->z_NavSvInfo.u_GpsNumSvPosFix, z_SvIDs,PDAPI_NMEA_GNGSA,TM_NMEA_SYSTEM_ID_GP);

    if (TRUE == sm_is_constellation_supported(SM_CCFG_RT_GLO))
    {
       sm_generate_sv_array_from_svinfo_struct(z_SvIDs,
                                            ARR_SIZE(z_SvIDs),
                                            p_fix->z_NavSvInfo.z_GloSvInfo,
                                            ARR_SIZE(p_fix->z_NavSvInfo.z_GloSvInfo),
                                            PDSM_GNSS_SV_INFO_USED_IN_POS_FIX);
       tm_nmea_gsa_put(p_fix, p_fix->z_NavSvInfo.u_GloNumSvPosFix, z_SvIDs,PDAPI_NMEA_GNGSA,TM_NMEA_SYSTEM_ID_GL);
    }

    if (TRUE == sm_is_constellation_supported(SM_CCFG_RT_GAL))
    {
       sm_generate_sv_array_from_svinfo_struct(z_SvIDs,
                                            ARR_SIZE(z_SvIDs),
                                            p_fix->z_NavSvInfo.z_GalSvInfo,
                                            ARR_SIZE(p_fix->z_NavSvInfo.z_GalSvInfo),
                                            PDSM_GNSS_SV_INFO_USED_IN_POS_FIX);
       tm_nmea_gsa_put(p_fix, p_fix->z_NavSvInfo.u_GalNumSvPosFix, z_SvIDs,PDAPI_NMEA_GNGSA,TM_NMEA_SYSTEM_ID_GA);
    }
}

/*===========================================================================
FUNCTION tm_nmea_pqgsa_put

DESCRIPTION
  tm_nmea_pqgsa_put() is used to transmit proprietary GSA - BDS/QZSS DOP and Active Satellites.
     

  1 2   3 4 5                                   6   7   8    9  10
  | |   | | |                                   |   |   |    |  |
 $aaccc,d,e,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x,[h]*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Mode, M = Manual, A = Automatic  2D/3D.
  4) Mode, 1 = No fix, 2 = 2D, 3 = 3D.
  5) PRN used in the position solution.  Using "null" (",") for no SV for
     a total of 12 tokens.
  6) PDOP - Position Dilution Of Precision.
  7) HDOP - Horizontal Dilution Of Precision.
  8) VDOP - Vertical Dilution Of Precision.
  9) [GNSS System ID for PQGSA only. BDS:4 QZSS:5]
  10) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

 Notes: If we send a 2D position to Street Altas, we will get a blinking 
 question mark sitting on the satellite icon within the 'GPS Status' window.
 I assume the program thinks that we've lost too many SVs and we're now
 in 2D mode.  We only set the 2D mode if we only have 3 SVs with an altitude-
 hold meaurement.  In 2D mode, the positions on the map will be in yellow.
 
===========================================================================*/
void tm_nmea_pqgsa_put( const sm_GnssFixRptStructType *p_fix )
{
  char b_sentence[200];

  if ( ( p_fix == NULL ) ||
       (!p_fix->z_NavPos.z_PosFlags.b_IsValid) ) /* Position invalid */
  {
    tm_nmea_empty_put(b_sentence, (int)sizeof(b_sentence), PDAPI_NMEA_PQGSA);
    return;
  }
  else
  {
    uint16 z_SvIDs[MAX_ACTIVE_GNSS_CHAN];  // max 16 active 

    sm_generate_sv_array_from_svinfo_struct(z_SvIDs,
                                            ARR_SIZE(z_SvIDs),
                                            p_fix->z_NavSvInfo.z_BdsSvInfo,
                                            ARR_SIZE(p_fix->z_NavSvInfo.z_BdsSvInfo),
                                            PDSM_GNSS_SV_INFO_USED_IN_POS_FIX);
    tm_nmea_gsa_put(p_fix, p_fix->z_NavSvInfo.u_BdsNumSvPosFix, z_SvIDs,PDAPI_NMEA_PQGSA+(pdsm_pa_nmea_type)TM_NMEA_SYSTEM_ID_BDS,TM_NMEA_SYSTEM_ID_BDS);

    sm_generate_sv_array_from_svinfo_struct(z_SvIDs,
                                            ARR_SIZE(z_SvIDs),
                                            p_fix->z_NavSvInfo.z_QzssSvInfo,
                                            ARR_SIZE(p_fix->z_NavSvInfo.z_QzssSvInfo),
                                            PDSM_GNSS_SV_INFO_USED_IN_POS_FIX);
    tm_nmea_gsa_put(p_fix, p_fix->z_NavSvInfo.u_QzssNumSvPosFix, z_SvIDs,PDAPI_NMEA_PQGSA+(pdsm_pa_nmea_type)TM_NMEA_SYSTEM_ID_QZSS,TM_NMEA_SYSTEM_ID_QZSS);
    
  }
  MSG_MED("Generated NMEA $PQGSA",0,0,0);
}

/*===========================================================================
FUNCTION tm_nmea_gprmc_put
 
DESCRIPTION

 tm_nmea_gprmc_put() is used to build and transmit the NMEA RMC sentence. 

 Global Positioning System Fix Data.

 Time, position and fix related data for a GPS receiver.

  1 2   3         4 5       6 7        8 9  10  11     12  13 14 15
  | |   |         | |       | |        | |   |   |      |   | |  | 
 $aaccc,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxxxx,x.x,a,a*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type. (RMC)
  3) Universal Time Coordinated (UTC) of position.
  4) Status (A = Valid, V = Invalid)
  5) Latitude  (DDmm.mm)    DD = two digits of degrees, mm.mm = minutes.
  6) N or S (North or South).
  7) Longitude (DDDmm.mm).
  8) E or W (East or West).
  9) Speed (Knots)
 10) Course. Degrees True.
 11) Date ddmmyy
 12) Magnetic variation 
 13) Mag variation direction. (E/W). E subtracts mag var from true, W adds mag var to true.
 14) Mode indicator. 
 15) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_gprmc_put(const sm_GnssFixRptStructType *p_fix )
{
  char b_sentence[ 200 ];
  char *p_B = b_sentence;
  int q_buf_len = sizeof(b_sentence);
  int q_len = 0 ;
  boolean b_utc_valid = FALSE;
  tm_CalendarTimeT z_utc_time;

  if ( (p_fix == NULL) ||
      (!p_fix->z_NavPos.z_PosFlags.b_IsValid) ) /* Position invalid */
  {
    tm_nmea_empty_put(b_sentence, q_buf_len, PDAPI_NMEA_GPRMC);
    return;
  }

  /*  Universal Time Coordinated (UTC) of position: */
  {
    b_utc_valid = tm_util_get_fix_time(p_fix, &z_utc_time);
    if (b_utc_valid == TRUE)
    {
      q_len = snprintf( p_B, q_buf_len,
                       "$GPRMC,%02" PRIu32 // Hour
                       "%02" PRIu32        // Minute
                       "%02" PRIu32        // Second
                       ".%02" PRIu32 ",A," , // Millisecond/10
                       z_utc_time.q_Hour, z_utc_time.q_Minute,
                       z_utc_time.q_Second, z_utc_time.q_Millisecond/10);
    }
    else
    {
      q_len = snprintf( p_B, q_buf_len, "$GPRMC,,A,");
    }

    if(q_len < 0 || q_len >= q_buf_len)
    {
      MSG_ERROR("NMEA: Error in string formatting",0,0,0);
      return;
    }

    p_B += q_len;
    q_buf_len -= q_len;
  }

    /* Latitude and longitude: */
  q_len = tm_util_calculate_position_print(p_fix, p_B, q_buf_len);
  if(q_len <= 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
      p_B += q_len;
      q_buf_len -= q_len;
  /* END - Latitude and Longitude */

    /* Velocity estimate */
    {
      double  d_north_vel, d_east_vel, d_knots, d_speed, d_azimuth;
      boolean heading_valid = FALSE;

      d_east_vel  = p_fix->z_NavPos.f_VelEnu[0];
      d_north_vel = p_fix->z_NavPos.f_VelEnu[1];

      d_speed = sqrt( d_north_vel * d_north_vel + d_east_vel * d_east_vel );

      if (p_fix->z_NavPos.z_PosFlags.b_IsVelValid) 
      {
        if (d_speed >= TM_CORE_MIN_VELOCTY)
        {
          d_knots = d_speed * (3600.0/1852.0);
        }
        else
        {
          d_knots = 0.0;
        }

    /* Azimuth moves from north (0) to east (90), wrapping around.
       Note that we are ignoring the vertical velocity. */
        if(p_fix->z_NavPos.z_PosFlags.b_IsHeadingValid ) 
        {
          d_azimuth = p_fix->z_NavPos.f_HeadingRad  * (180.0 / C_PI);
          heading_valid = TRUE;
        }
        else
        {
          d_azimuth = 0.0;
          heading_valid = FALSE;
        }

        /* Don't worry about magnetic deviation. We can come back 
           and add this later if a customer has a burning need. */
        if(heading_valid == TRUE)
        {
          q_len = snprintf( p_B, q_buf_len, "%.1lf,%.1lf,",
                          d_knots,      /* Knots */
                          d_azimuth );    /* Degrees. */
        }
        else
        {
          q_len = snprintf( p_B, q_buf_len, "%.1lf,,",
                          d_knots);      /* Knots */
        }
        
        if (q_len < 0 || q_len >= q_buf_len)
        {
          if(q_len < 0 || q_len >= q_buf_len)
          {
            MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
            return;
          }
          
          p_B += q_len;
          q_buf_len -= q_len;
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }
          
        p_B += q_len;
        q_buf_len -= q_len;
      }
      else /* Velocity not available */
      {
        q_len = snprintf( p_B, q_buf_len, ",," );

        if (q_len < 0 || q_len >= q_buf_len)
        {
          MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
          return;
        }
    
        p_B += q_len;
        q_buf_len -= q_len;
      }
    }

    /* Days, Months, Years */
    {
      if (b_utc_valid == TRUE)
      {
        q_len = snprintf( p_B, q_buf_len, "%2.2lu%2.2lu%2.2lu,",
            z_utc_time.q_Day, z_utc_time.q_Month, z_utc_time.q_Year % 100 );
      }
      else
      {
        q_len = snprintf( p_B, q_buf_len, ",");
      }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
      
      p_B += q_len;
      q_buf_len -= q_len;
    }

    /* Magnetic variation and direction */
    {
      double d_MagVar;
      char u_VarDir = 'E';

      if (p_fix->z_NavPos.u_MagDevGood)
      {
        d_MagVar = p_fix->z_NavPos.f_MagDeviation;
        if (d_MagVar < 0.0)
        {
          u_VarDir = 'W';
          d_MagVar *= -1.0;
        }
        q_len = snprintf( p_B, q_buf_len, "%.1lf,%c,", d_MagVar, u_VarDir);
      }
      else
      {
        q_len = snprintf( p_B, q_buf_len, ",,");
      }

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
      
      p_B += q_len;
      q_buf_len -= q_len;
    }

    /* Mode Indicator */
    {
      char u_mode_indicator = 'N'; /* Data Not Valid */

      if (tm_nmea_is_differential_fix(p_fix))
      {
        u_mode_indicator = 'D'; /* Differential (DGPS) */
      }
      else
      {
        if (tm_nmea_is_propagated_fix(p_fix) ||
            ((p_fix->z_NavSvInfo.u_GpsNumSvPosFix == 0) &&
             (p_fix->z_NavSvInfo.u_GloNumSvPosFix == 0) &&
             (p_fix->z_NavPos.z_PosFlags.b_IsInsOnlySln)) )
      {
          u_mode_indicator = 'E'; /* Estimated (dead reckoning) */
      }
      else
      {
          u_mode_indicator = 'A'; /* Autonomous (Non DGPS) */
        }
      }

      q_len = snprintf( p_B, q_buf_len, "%c", u_mode_indicator);

      if(q_len < 0 || q_len >= q_buf_len)
      {
        MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
        return;
      }
      
      p_B += q_len;
      q_buf_len -= q_len;
    }

  tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_GPRMC);    
  MSG_MED("Generated NMEA $GPRMC",0,0,0);
}

/*===========================================================================
FUNCTION tm_nmea_pstis_put

DESCRIPTION
  tm_nmea_pstis_put() is used to transmit PSTIS - GPS session start indication
     
  12      3    
  ||      |      
  $PSTIS,*hh<CR><LF>

  Field Description: 
  1) Talker ID.
  2) PSTIS for Proprietary SnapTrack Inc. session control message.
  3) Checksum


DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_pstis_put(void)
{
  char b_sentence[200], *p_b = b_sentence;
  int q_buf_len = sizeof(b_sentence),q_len = 0 ;                  
  
  q_len = snprintf( p_b, 200,"$PSTIS," );

  if(q_len < 0 || q_len >= q_buf_len)
  {
    MSG_ERROR("NMEA: Error in string formatting",0,0,0);  
    return;
  }
  
  p_b += q_len;
  q_buf_len -= q_len;
  
  
  tm_nmea_terminate_and_put(b_sentence, PDAPI_NMEA_PSTIS);  
  
   
  MSG_MED("Generated NMEA $PSTIS",0,0,0);
  /*Send NMEA string over to PDAPI*/
  /*tm_nmea_pdapi_send(b_sentence,(pdsm_pa_nmea_type)PDAPI_NMEA_VTG);*/
}

/*===========================================================================

FUNCTION tm_nmea_diag_sio_close_cb

DESCRIPTION
This callback is closes the NMEA connection, if there is one.

DEPENDENCIES
None.

RETURN VALUE
None.  This procedure does not return.

SIDE EFFECTS
None.

===========================================================================*/
void tm_nmea_diag_sio_close_cb(void)
{
  MSG_MED("Received DIAG SIO CLOSE CB",0,0,0); 
  tm_nmea_send_message_no_payload(TM_CORE_NMEA_MSG_ID_DIAG_SIO_CLOSE_CB_2);
}

/*===========================================================================

FUNCTION tm_nmea_diag_close_port

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_nmea_diag_close_port(void (*close_func_ptr)(void))
{
#ifndef __GRIFFON__
  MSG_MED("Diag closing nmea port: %d, %x", (int32)tm_nmea_sio_stream_id, close_func_ptr, 0); /*lint !e571 */
  
  if (tm_nmea_sio_stream_id != SIO_NO_STREAM_ID)
  {
    sio_close(tm_nmea_sio_stream_id, close_func_ptr);
    tm_nmea_sio_flag      = FALSE;
    tm_nmea_sio_stream_id = SIO_NO_STREAM_ID;
    return TRUE;
  }
#endif
  return FALSE;
}

/*===========================================================================

 FUNCTION tm_nmea_diag_open_port_part2

 DESCRIPTION

 DEPENDENCIES
 None.

 RETURN VALUE
 None.  This procedure does not return.

 SIDE EFFECTS
 None.

 ===========================================================================*/
void tm_nmea_diag_open_port_part2(void)
{
#ifndef __GRIFFON__
  MSG_HIGH("DIAG NMEA Open Port_2: %d", (int32)tm_nmea_diag_port_to_open, 0, 0);

  if(tm_nmea_diag_port_to_open == TM_NMEA_DIAG_OPEN_PORT_USB)
  {
    MSG_HIGH("USB NMEA port is not supported", 0, 0, 0); /*lint !e571 */
    return;
  }
  else if(tm_nmea_diag_port_to_open == TM_NMEA_DIAG_OPEN_PORT_UART1)
  {
    tm_nmea_open.port_id = SIO_PORT_UART_MAIN;  
  }
  else if(tm_nmea_diag_port_to_open == TM_NMEA_DIAG_OPEN_PORT_UART2)
  {
#ifdef FEATURE_SECOND_UART
    tm_nmea_open.port_id = SIO_PORT_UART_AUX;
#else
    MSG_HIGH("UART AUX NMEA port is not supported", 0, 0, 0); /*lint !e571 */
    return;
#endif /* FEATURE_SECOND_UART */
  }  
  else
  {
    tm_nmea_open.port_id = SIO_PORT_SMD_GPS_NMEA;
  }

  tm_nmea_sio_stream_id = sio_open(&tm_nmea_open);

  if (tm_nmea_sio_stream_id != SIO_NO_STREAM_ID)
  {
    MSG_HIGH("NMEA port opened", 0, 0, 0); /*lint !e571 */
    tm_nmea_sio_flag = TRUE; 
  }
  else
  {
    MSG_ERROR("NMEA port opening failed", 0, 0, 0); /*lint !e571 */ 
    tm_nmea_sio_flag = FALSE;
  }
#endif
}

/*===========================================================================

FUNCTION tm_nmea_diag_open_port

DESCRIPTION

DEPENDENCIES
None.

RETURN VALUE
None.  This procedure does not return.

SIDE EFFECTS
None.

===========================================================================*/
void tm_nmea_diag_open_port(tm_nmea_diag_open_port_id_e_type port_to_open)
{
#ifndef __GRIFFON__
  /* for Gobi, TM should not open the NMEA port */
#ifdef FEATURE_CGPS_LBS_TASK_NMEA_PORT
  return;
#else

  MSG_MED("Diag opening nmea port: %d", (int32)port_to_open, 0, 0); /*lint !e571 */

  /* We were told to open, so try to do so.  Start by closing 
  ** any existing connection.
  */
  tm_nmea_diag_port_to_open = port_to_open;

  if(!tm_nmea_diag_close_port(tm_nmea_diag_sio_close_cb)) {
    tm_nmea_diag_open_port_part2();
  }
#endif
#endif
}

/*===========================================================================

FUNCTION tm_nmea_debug

DESCRIPTION
  This function sends the debug NMEA sentences to LBS and also
  on the NMEA COM port if it is open.

  The debug NMEA string is already NMEA and NULL terminated.
  The string is sent out as is.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_nmea_debug(tm_nmea_data_s_type* pDebugNMEA)
{

#ifndef __GRIFFON__
#ifdef FEATURE_CGPS_LBS_TASK
  // LBS is NOT part of Griffon.

  (void) lbs_send_nmea_dbg_str(
    (const char*)&(pDebugNMEA->p_nmea_sentence[0]),
    pDebugNMEA->q_sentence_len);
#endif
#endif  // __GRIFFON__

  tm_nmea_port_put(0, (char*) &(pDebugNMEA->p_nmea_sentence[0]));

  MSG_LOW("Sent Debug NMEA",0,0,0);

}

/*===========================================================================
FUNCTION tm_nmea_is_fresh_meas

DESCRIPTION
  Internal function to check if GPS Measurement is fresh

DEPENDENCIES

RETURN VALUE
  Boolean: TRUE if GPS Measurement is fresh, FALSE otherwise

SIDE EFFECTS

===========================================================================*/
static boolean tm_nmea_is_fresh_meas(const sm_GpsMeasRptStructType* p_Meas)
{
  uint32 q_I;
  const gnss_MeasBlkStructType *p_MeasBlk;
  const gnss_MeasStructType    *p_GpsMeas;
  const gnss_MeasStructType    *p_GloMeas;

  /* Sanity Check */
  if (p_Meas == NULL)
  {
    MSG_ERROR("OEMDRETD Is Fresh Meas is NULL", 0, 0, 0);
    return FALSE;
  }

  /* Update the reported measurement latency based on updated clock information */
  p_MeasBlk = (const gnss_MeasBlkStructType*) &(p_Meas->z_MeasBlk);

  if (p_MeasBlk->e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS)
  {
    p_GpsMeas = (const gnss_MeasStructType*) &(p_MeasBlk->z_Gnss[0]);

    for (q_I = p_MeasBlk->u_NumSvs; q_I; q_I--, p_GpsMeas++)
    {
      if (p_GpsMeas->w_Sv > N_GPS_SV)
      {
        continue; /* not a GPS SV */
      }

      if ( (p_GpsMeas->q_MeasStatus & (SM_MEAS_STATUS_INVALID_FLAGS|SM_MEAS_STATUS_SM_VALID)) ==
                      SM_MEAS_STATUS_SM_VALID )
      {
         /* Now check if GPS Meas is fresh (just need one SV to be fresh) */
        if (p_GpsMeas->q_MeasStatus & MEAS_STATUS_GNSS_FRESH_MEAS)
        {
          MSG_HIGH("OEMDRETD Is Fresh Meas: GPS MeasStatus=0x%x using SV%d TRUE", p_GpsMeas->q_MeasStatus, p_GpsMeas->w_Sv, 0);
          return TRUE;
        }
      }
    }
  }
  else if (p_MeasBlk->e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GLO)
  {
    p_GloMeas = (const gnss_MeasStructType*) &(p_MeasBlk->z_Gnss[0]);

    for (q_I = p_MeasBlk->u_NumSvs; q_I; q_I--, p_GloMeas++)
    {
      if ( (p_GloMeas->w_Sv <= GLO_SLOTID_OFFSET) || (p_GloMeas->w_Sv > (N_GLO_SV + GLO_SLOTID_OFFSET)) )
      {
        continue; /* not a Glonass SV */
      }

      if ( (p_GloMeas->q_MeasStatus & (SM_MEAS_STATUS_INVALID_FLAGS|SM_MEAS_STATUS_SM_VALID)) ==
                      SM_MEAS_STATUS_SM_VALID )
      {
         /* Now check if Glonass Meas is fresh (just need one SV to be fresh) */
        if (p_GloMeas->q_MeasStatus & MEAS_STATUS_GNSS_FRESH_MEAS)
        {
          MSG_HIGH("OEMDRETD Is Fresh Meas: GLO MeasStatus=0x%x using SV%d TRUE", p_GloMeas->q_MeasStatus, p_GloMeas->w_Sv, 0);
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION tm_nmea_set_gnss_meas_update

DESCRIPTION
  Stores the FCount from GPS measurement report

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_set_gnss_meas_update(const sm_GpsMeasRptStructType* p_Meas)
{
  const gnss_MeasBlkStructType *p_MeasBlk;

  /* Sanity Check */
  if (p_Meas == NULL)
  {
    MSG_ERROR("OEMDRETD Set Meas Update is NULL", 0, 0, 0);
    return;
  }

  p_MeasBlk = (const gnss_MeasBlkStructType*) &(p_Meas->z_MeasBlk);

  if (p_MeasBlk->e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS)
  {
    q_LastGPSMeasRefCount = p_MeasBlk->z_MeasClock.q_RefFCount;
    b_IsFreshGPSMeas      = tm_nmea_is_fresh_meas(p_Meas);
  }
  else if (p_MeasBlk->e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GLO)
  {
    q_LastGLOMeasRefCount = p_MeasBlk->z_MeasClock.q_RefFCount;
    b_IsFreshGLOMeas      = tm_nmea_is_fresh_meas(p_Meas);
  }
}

/*===========================================================================
FUNCTION tm_nmea_is_differential_fix

DESCRIPTION
  Checks to see if the fix is differential or not

DEPENDENCIES

RETURN VALUE
  Boolean:
    TRUE  if the fix is differential (DGPS fix)
    FALSE if the fix is non-differential (non-DGPS fix)

SIDE EFFECTS

===========================================================================*/
static boolean tm_nmea_is_differential_fix(const sm_GnssFixRptStructType *p_fix)
{
  if (p_fix == NULL)
  {
    MSG_ERROR("Is Differential Fix is NULL", 0, 0, 0);
    return FALSE;
  }

  /* Differential GPS is currently not supported by Callisto Position Engine */
#if 0
  if (p_fix->z_NavPos.u_IonoCorrected && p_fix->z_NavPos.u_TropoCorrected)
  {
    return TRUE;
  }
#endif /* 0 */

  return FALSE;
}

/*===========================================================================
FUNCTION tm_nmea_is_propagated_fix

DESCRIPTION
  Checks to see if the fix is propagated (estimated) or not (valid)

DEPENDENCIES

RETURN VALUE
  Boolean:
    TRUE if the fix is invalid (propagated fix)
    FALSE if the fix is valid (non-propagated fix)

SIDE EFFECTS

===========================================================================*/
static boolean tm_nmea_is_propagated_fix(const sm_GnssFixRptStructType *p_fix)
{
  int32 l_FcountDiff;

  if (p_fix == NULL)
  {
    MSG_ERROR("OEMDRETD Is Propagated Fix is NULL", 0, 0, 0);
    return FALSE;
  }

  if (tm_core_info.config_info.oemdre_td_ctrl == FALSE)
  {
    MSG_MED("OEMDRETD Is Propagated Fix Control disabled", 0, 0, 0);
    return FALSE;
  }

  if (p_fix->z_NavSvInfo.u_GpsNumSvPosFix > 0)
  {
    l_FcountDiff = p_fix->z_NavPos.q_RefFCount - q_LastGPSMeasRefCount;

    if (labs(l_FcountDiff) < 1000)
    {
      return !b_IsFreshGPSMeas;
    }
  }
  if (p_fix->z_NavSvInfo.u_GloNumSvPosFix > 0)
  {
    l_FcountDiff = p_fix->z_NavPos.q_RefFCount - q_LastGLOMeasRefCount;

    if (labs(l_FcountDiff) < 1000)
    {
      return !b_IsFreshGLOMeas;
    }
  }

  return FALSE;
}
