/*======================================================================

               PDCOMM Application task internal header file

 GENERAL DESCRIPTION
  This file contains MACROS and data-structures internally used by 
  PDCOMM Application.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_wms/src/pd_comms_wms_data.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 14/03/16   kpn  Added new member e_as_id in pd_comms_wms_task_handle_data_type to support multi-SIM functionality
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#ifndef PDCOMM_APP_WMS_DATA_H
#define PDCOMM_APP_WMS_DATA_H

#include "comdef.h"
#include "dssocket.h"
#include "pd_comms_api.h"
#include "pd_comms_app_api.h"
#include "secssl.h"
#include "wms.h"

/****************** Constants/Enums *******************************************/

#define PDCOMM_APP_MSG_LOW( str, a, b, c )       MSG_LOW( "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_MSG_MED( str, a, b, c )       MSG_MED( "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_MSG_ERROR( str, a, b, c )     MSG_ERROR( "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_ERR_FATAL( str, a, b, c )     ERR_FATAL( "=PDCOMM APPLICATION= " str, a, b, c )

#define PDCOMM_MAX_HANDLES 2
#define PDCOMM_WRITE_DATA_BUF_SIZE 2500
#define PDCOMM_READ_DATA_BUF_SIZE 1024
#define PDCOMM_INACTIVITY_TIMEOUT 40000 /*in millisecs*/
#define PDCOMM_INACTIVITY_TIMER_PERIOD  5000/*in millisecs*/

#define PDCOMM_MO_SMS_RETRY_PERIOD 15  /*in secs*/
#define PDCOMM_MO_SMS_RETRY_INTERVAL 3 /*in secs*/


/*This enumeration defines all the internal IPC message IDs 
 *for PDCOMM_APP_TASK. PDCOMM_IPC_END is used to prevent 
 *overlap of these message IDs with the PDCOMM interface message
 *IDs*/
typedef enum
{
  PDCOMM_APP_TASK_SMS_MSG_CB = C_PD_COMMS_APP_MSG_IPC_END+1,  
  PDCOMM_APP_TASK_DOG_TIMER,
  PDCOMM_APP_TASK_INACTIVITY_TIMER
} pd_comms_wms_task_ipc_message_type;

typedef enum
{
  SUCCEEDED,
  FAILED,
  PENDING
} pd_comms_wms_task_operation_status_type;

/****************** Data Structures*********************************************/

/* PDCOMM APP state information */
typedef struct
{
  /*General PD-COMM state*/
  boolean                      in_use;      /*default value FALSE*/
  pd_comms_state_type          e_link_status;/*default value COMM_STATE_DISCONNECTED*/
  pd_comms_connect_params_type z_connect_params;
  uint32                       q_source_thread_id; /*thread_id of the client*/
  uint32                       q_base_msg_id; /*base message ID when responding back to client*/
  pd_comms_handle_type         z_handle;
  boolean                      client_disconnect_recvd; /*default FALSE*/
  boolean                      session_aborted; /*default FALSE*/

  pd_comms_ipc_connect_hdr_type z_pendingConnectReq; /*connect request received while the
                                                       previous connect is still in DISCONNECTING
                                                       state*/
  boolean                       u_connectPending;
  /*Inactivity Timer*/
  int                          l_inactivity_count; /*default value 0*/
                                                        
  /*Write parameters*/
  uint16                       w_num_bytes_to_write;     /* Number of bytes in buffer to send */
  uint16                       w_num_bytes_written;      /* Number of bytes in buffer sent */
  /*MR: Make this dynamic allocation once DMA is implemented in PDCOMM*/
  uint8                        p_write_buff[PDCOMM_WRITE_DATA_BUF_SIZE];          /* data to send */

  /*Read parameters*/
  uint16                       w_num_bytes_to_read;      /* Number of bytes to be read */
  uint16                       w_num_bytes_already_read; /* Number of bytes already read */

  /*Read Buffer*/
  uint16                       w_num_bytes_in_buffer; /* Number of bytes in p_read_buff[]. Used only in SMS*/
  uint8*                       p_read_ptr; /*Used only in SMS*/
  /*MR: Make this dynamic allocation once DMA is implemented in PDCOMM*/
  uint8                        p_read_buff[PDCOMM_READ_DATA_BUF_SIZE];          /* data buffer */

  /* transaction IDs*/
  /*connect_transaction_id stored in connect_params*/
  uint32                      q_disconnect_transaction_id;
  uint32                      q_write_transaction_id;
  uint32                      q_read_transaction_id;
  uint32                      q_ioctl_transaction_id;

  /* WMS params */
  wms_transaction_id_type q_sms_transaction_id;
  boolean u_multiPartActive;

  /* AS ID */
  sys_modem_as_id_e_type      e_as_id;

} pd_comms_wms_task_handle_data_type;




/*The SMS callback functions will send an internal
 *message to PDCOMM_WMS_TASK in pd_comms_wms_task_sms_msg_cb_ipc_hdr_type
 *format to notify PDCOMM_WMS_TASK regarding NETWORK/SOCKET EVENT*/
typedef struct
{
  pd_comms_handle_type handle;
} pd_comms_wms_task_sms_msg_cb_ipc_hdr_type;



/******************************Function declarations**************************/

/*===========================================================================
FUNCTION pd_comms_wms_task_handle_get_handle

DESCRIPTION
  This function handles an incoming get_handle request. It assigns an free
  handle for the connection 

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_return_type

SIDE EFFECTS
  None
===========================================================================*/
extern pd_comms_return_type  pd_comms_wms_task_handle_get_handle( 
  /*contains the get_handle params*/  
  const pd_comms_get_handle_params_type* const p_params,
  /*the assigned handle is returned in this variable */
  pd_comms_handle_type* p_handle  );


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_release_handle

DESCRIPTION
  This function handles an incoming release_handle request. It releases a previously
  assigned handle. If the link is CONNECTED it will abort the connection before
  releasing the handle.

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_return_type

SIDE EFFECTS
  None
===========================================================================*/
extern pd_comms_return_type  pd_comms_wms_task_handle_release_handle( 
  /*the assigned handle is returned in this variable */    
  const pd_comms_handle_type* p_handle);

#endif /* PDCOMM_APP_WMS_DATA_H */
