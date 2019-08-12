/******************************************************************************
  @file    qmi_qmux_if.h
  @brief   Common, external QMI definitions

  DESCRIPTION
  This file contains common, external header file definitions for QMI
  interface library.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_init() must be called at client process initialization time.
  qmi_connection_init() must be called for each QMI control channel that
  you want to send messages on.
  qmi_release() must be called prior to client process exiting

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/qmi_qmux_if.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $
  ---------------------------------------------------------------------------
  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef QMI_QMUX_IF_H
#define QMI_QMUX_IF_H

#include "qmi.h"

#ifdef __cplusplus
extern "C" {
#endif


#define QMI_QMUX_IF_PLATFORM_SPECIFIC_HDR_SIZE 0

/* QMUX I/F message ID/structure definitions.  See IMPORTANT note below
** about adding system indication message ID's */

typedef enum
{
  QMI_QMUX_IF_FIRST_MSG_ID = 0,
  QMI_QMUX_IF_QMI_MSG_ID = QMI_QMUX_IF_FIRST_MSG_ID,
  QMI_QMUX_IF_ALLOC_QMI_CLIENT_ID_MSG_ID = 1,
  QMI_QMUX_IF_RELEASE_QMI_CLIENT_ID_MSG_ID = 2,
  QMI_QMUX_IF_SET_DATA_FORMAT_MSG_ID = 3,
  QMI_QMUX_IF_ADD_QMUX_CLIENT_MSG_ID = 4,
  QMI_QMUX_IF_DELETE_QMUX_CLIENT_MSG_ID = 5,
  QMI_QMUX_IF_REG_PWR_SAVE_MODE_MSG_ID = 6,
  QMI_QMUX_IF_CONFIG_PWR_SAVE_SETTINGS_MSG_ID = 7,
  QMI_QMUX_IF_SET_PWR_STATE_MSG_ID = 8,
  QMI_QMUX_IF_GET_PWR_STATE_MSG_ID = 9,
  QMI_QMUX_IF_GET_VERSION_INFO = 10,
  QMI_QMUX_IF_OPEN_CONNECTION_MSG_ID = 11,
  QMI_QMUX_IF_REG_SRVC_AVAIL_MSG_ID = 12,
  QMI_QMUX_IF_SET_EVENT_REPORT_MSG_ID = 13,

  /* IMPORTANT!!! Put ALL system indications in this block of ID's (between
  ** QMI_QMUX_IF_FIRST_SYS_IND_MSG_ID and QMI_QMUX_IF_LAST_SYS_IND_MSG_ID (move
  ** definition of QMI_QMUX_IF_LAST_SYS_IND_MSG_ID if necessary  
  */
  QMI_QMUX_IF_PWR_STATE_IND_MSG_ID = 14,
  QMI_QMUX_IF_FIRST_SYS_IND_MSG_ID = QMI_QMUX_IF_PWR_STATE_IND_MSG_ID,
  QMI_QMUX_IF_SYNC_IND_MSG_ID = 15,
  QMI_QMUX_IF_MODEM_OUT_OF_SERVICE_MSG_ID = 16,
  QMI_QMUX_IF_MODEM_IN_SERVICE_MSG_ID = 17,
  QMI_QMUX_IF_NEW_SRVC_AVAIL_MSG_ID = 18,
  QMI_QMUX_IF_LAST_SYS_IND_MSG_ID = QMI_QMUX_IF_NEW_SRVC_AVAIL_MSG_ID,

  QMI_QMUX_IF_MAX_NUM_MSGS /* Must stay last in enum */
} qmi_qmux_if_msg_id_type;



typedef struct
{
  qmi_qmux_if_msg_id_type    msg_id;
  int                        qmux_client_id;
  unsigned long              qmux_txn_id;
  int                        sys_err_code;
  int                        qmi_err_code;
  qmi_connection_id_type     qmi_conn_id;
  qmi_service_id_type        qmi_service_id;
  qmi_client_id_type         qmi_client_id;
  unsigned char              control_flags;
} qmi_qmux_if_msg_hdr_type;


/* QMI message header sizes */
#define QMI_QMUX_HDR_SIZE         (6)
#define QMI_SRVC_STD_TXN_HDR_SIZE (3)
#define QMI_SRVC_STD_MSG_HDR_SIZE (4)

/* The service header will be the bigger of the QMUX header and the 
** SRVC <-> QMUX message header.  The two will never be needed at
** the same time.  Current assumption is that the qmi_qmux_if_msg_hdr_type
** will always be bigger than the size of a QMUX header (6 bytes).
*/
#define QMI_QMUX_IF_HDR_SIZE  (sizeof (qmi_qmux_if_msg_hdr_type))

/* Size of header that goes on each message buffer */
#define QMI_MAX_HDR_SIZE (QMI_QMUX_IF_PLATFORM_SPECIFIC_HDR_SIZE + \
                          QMI_QMUX_IF_HDR_SIZE + \
                          QMI_SRVC_STD_TXN_HDR_SIZE + \
                          QMI_SRVC_STD_MSG_HDR_SIZE)


/* Min size of a message (header info) between SRVC <-> QMUX */
#define QMI_QMUX_IF_MSG_HDR_SIZE  (QMI_QMUX_IF_HDR_SIZE + QMI_QMUX_IF_PLATFORM_SPECIFIC_HDR_SIZE)

/* This cmd/rsp data will be sent between client and QMUX for CTL 
** and other control related messages 
*/

typedef struct {
    unsigned char   qmi_svc_type; /*  QMI service type */
    unsigned short  major_ver;    /*  Major version number */
    unsigned short  minor_ver;    /*  Minor version number */
}qmi_version_type;


typedef struct
{
    int               qmi_service_version_len;
    qmi_version_type  qmi_service_version[QMI_MAX_SERVICE_VERSIONS];
    unsigned char     addendum_label_len;
    unsigned char     addendum_label[QMI_MAX_ADDENDUM_LABEL];
    int               qmi_addendum_instances;
    qmi_version_type  addendum_version_list[QMI_MAX_SERVICE_VERSIONS];
}qmi_service_version_list_type;


typedef union
{
  struct 
  {
    qmi_data_format_qos_hdr_state_type  qos_hdr_state;
    qmi_link_layer_protocol_type        link_protocol;
  } qmi_qmux_if_set_data_format_req;

  struct 
  {
    qmi_link_layer_protocol_type        link_protocol;
  } qmi_qmux_if_set_data_format_rsp;

  struct 
  {
    qmi_service_id_type service_id;
  } qmi_qmux_if_alloc_client_req;

  struct 
  {
    qmi_client_id_type new_client_id;
  } qmi_qmux_if_alloc_client_rsp;

  struct 
  {
    qmi_service_id_type  delete_service_id;
    qmi_client_id_type   delete_client_id;
  } qmi_qmux_if_release_client_req;

  struct 
  {
    int     qmux_client_id;
  } qmi_qmux_add_delete_qmux_client_req_rsp;

  struct
  {
    qmi_pwr_report_type  report_state;
  } qmi_qmux_if_reg_pwr_save_mode_req;

  struct 
  {
    int curr_pwr_state_hndl;
    int prev_pwr_state_hndl;
  } qmi_qmux_if_pwr_state_ind;

  struct 
  {
    int                   pwr_state_hndl;
    qmi_service_id_type   service_id;
    int                   num_indication_ids;
    unsigned short        indication_ids[QMI_MAX_PWR_INDICATIONS];
  } qmi_qmux_if_config_pwr_save_settings_req;

  struct
  {
    unsigned long new_pwr_state;
  } qmi_qmux_if_set_pwr_save_mode_req;

  struct
  {
    unsigned long curr_pwr_state;
  } qmi_qmux_if_get_pwr_save_mode_rsp;  
      
  struct
  {
    qmi_connection_id_type conn_id;
  } qmi_qmux_if_sync_ind;
  
  struct
  {
    qmi_connection_id_type conn_id;
  } qmi_qmux_if_open_connection_req;  
      
  qmi_service_version_list_type qmi_qmux_if_get_version_info_rsp;

  struct
  {
    qmi_service_id_type service_id;
  } qmi_qmux_if_reg_srvc_req;

} qmi_qmux_if_cmd_rsp_type;

typedef void (*qmi_qmux_if_rx_msg_hdlr_type)( qmi_connection_id_type  conn_id,
                                                qmi_service_id_type     service_id,
                                                qmi_client_id_type      client_id,
                                                unsigned char           control_flags,
                                                unsigned char           *rx_msg, 
                                                int                     rx_msg_len);

/*===========================================================================
  FUNCTION  qmi_qmux_if_alloc_service_client
===========================================================================*/
/*!
@brief 
  Calls the QMUX layer to add a service client 
  
@return 
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int
qmi_qmux_if_alloc_service_client
(
  qmi_connection_id_type  conn_id,
  qmi_service_id_type     service_id,
  qmi_client_id_type      *client_id,
  int                     *qmi_err_code
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_release_service_client
===========================================================================*/
/*!
@brief 
  Calls the QMUX layer to remove a service client 
  
@return 
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int
qmi_qmux_if_release_service_client
(
  qmi_connection_id_type  conn_id,
  qmi_service_id_type     service_id,
  qmi_client_id_type      client_id,
  int                     *qmi_err_code
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_set_data_format
===========================================================================*/
/*!
@brief 
  Calls the QMUX layer to set the data format of the port associated
  with the input connection ID. 
  
@return 
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise.
  If QMI_SERVICE_ERR, then the qmi_err_code will be valid and contain
  the error code returned by QMI on the modem.

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int
qmi_qmux_if_set_data_format
(
  qmi_connection_id_type                conn_id,
  qmi_data_format_qos_hdr_state_type    qos_hdr_state,
  qmi_link_layer_protocol_type          *link_protocol,
  int                                   *qmi_err_code
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_is_conn_active
===========================================================================*/
/*!
@brief 
  Calls the QMUX layer to query the active state of a connection 
  
@return 
  TRUE if connection is active, FALSE otherwise

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int
qmi_qmux_if_is_conn_active
(
  qmi_connection_id_type  conn_id
);


/*===========================================================================
  FUNCTION  qmi_qmux_if_send_qmi_msg
===========================================================================*/
/*!
@brief 
  Sends a QMI message for the connection/service/client specified 
  
@return 
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int 
qmi_qmux_if_send_qmi_msg
(
  qmi_connection_id_type  conn_id,
  qmi_service_id_type     service_id,
  qmi_client_id_type      client_id,
  unsigned char           *msg_buf,
  int                     msg_buf_size
);




/*===========================================================================
  FUNCTION  qmi_qmux_if_open_connection
===========================================================================*/
/*!
@brief 
  Opens up a new connection for the QMUX client
  
@return 
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int 
qmi_qmux_if_open_connection
(
  qmi_connection_id_type    conn_id
);


/*===========================================================================
  FUNCTION  qmi_qmux_if_pwr_up_init
===========================================================================*/
/*!
@brief 
  Starts up a new "QMUX" client.  There will be one QMUX client per PD.
  
@return 
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int 
qmi_qmux_if_pwr_up_init
(
  qmi_qmux_if_rx_msg_hdlr_type rx_msg_hdlr,
  qmi_sys_event_rx_hdlr   sys_event_rx_hdlr,
  void                    *sys_event_user_data
);


/*===========================================================================
  FUNCTION  qmi_qmux_if_pwr_down_release
===========================================================================*/
/*!
@brief 
  Called on client shutdown.
  
@return 
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
int 
qmi_qmux_if_pwr_down_release
(
  void
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_rx_hdlr
===========================================================================*/
/*!
@brief 
  QMI QMUX service RX handler.  This is to be called by QMUX layer to
  report incoming message.
  
@return 
  None.

@note 

  - Side Effects
    - 
    
*/    
/*=========================================================================*/
void
qmi_qmux_if_rx_msg
(
  unsigned char *msg, 
  int            msg_len
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_reg_pwr_save_mode
===========================================================================*/
/*!
@brief 
  This function is used to register/de-register for power state change
  events.  Calls relevant QMI_QMUX function
     
  
@return 
  0 if operation was sucessful, < 0 if not.  If return code is 
  QMI_SERVICE_ERR, then the qmi_err_code will be valid and will 
  indicate which QMI error occurred.

@note

  - Side Effects
    - Talks to modem processor
*/    
/*=========================================================================*/
int
qmi_qmux_if_reg_pwr_save_mode
(
  qmi_pwr_report_type   report_state,
  int                   *qmi_err_code
);


/*===========================================================================
  FUNCTION  qmi_qmux_if_config_pwr_save_settings
===========================================================================*/
/*!
@brief 
  Configures the power state indication filter for each connection.
     
  
@return 
  0 if operation was sucessful, < 0 if not.  If return code is 
  QMI_SERVICE_ERR, then the qmi_err_code will be valid and will 
  indicate which QMI error occurred.

@note

  - Side Effects
    - Talks to modem processor
*/    
/*=========================================================================*/
int
qmi_qmux_if_config_pwr_save_settings
(
  int                  pwr_state_hndl,
  qmi_service_id_type  service_id,
  int                  num_indication_ids,
  unsigned short       indication_ids[],
  int                  *qmi_err_code
);


/*===========================================================================
  FUNCTION  qmi_qmux_if_set_pwr_state
===========================================================================*/
/*!
@brief 
  Sets power state for each connection.
     
  
@return 
  0 if operation was sucessful, < 0 if not.  If return code is 
  QMI_SERVICE_ERR, then the qmi_err_code will be valid and will 
  indicate which QMI error occurred.

@note

  - Side Effects
    - Talks to modem processor
    - Modem will not send filtered indications until later power state change. 
*/    
/*=========================================================================*/
int
qmi_qmux_if_set_pwr_state
(
  unsigned long        pwr_state,
  int                  *qmi_err_code
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_get_pwr_state
===========================================================================*/
/*!
@brief 
  Sets power state.  Calls relevant QMI QMUX function to do so  
     
  
@return 
  0 if operation was sucessful, < 0 if not.  If return code is 
  QMI_SERVICE_ERR, then the qmi_err_code will be valid and will 
  indicate which QMI error occurred.

@note

  - Side Effects
    - Talks to modem processor
*/    
/*=========================================================================*/
int
qmi_qmux_if_get_pwr_state
(
  unsigned long        *pwr_state,
  int                  *qmi_err_code
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_get_version_list
===========================================================================*/
/*!
@brief 
  Get the service version information
     
  
@return 
  0 if operation was sucessful, < 0 if not.  If return code is 
  QMI_SERVICE_ERR, then the qmi_err_code will be valid and will 
  indicate which QMI error occurred.

@note

  - Side Effects
    - Talks to modem processor
*/    
/*=========================================================================*/

int 
qmi_qmux_if_get_version_list
(
    qmi_connection_id_type         conn_id,
    qmi_service_id_type            service_id,
    unsigned short                 *major_ver,	/*  Major version number */
    unsigned short                 *minor_ver,	/*  Minor version number */
    int                            *qmi_err_code
);

/*===========================================================================
  FUNCTION  qmi_qmux_if_reg_service_avail
===========================================================================*/
/*!
@brief
  This function is used to register for service availability indication
  from modem.

@return
  QMI_NO_ERR if no error occurred, QMI_*_ERR error code otherwise

@note

  - Side Effects
  - None
*/
/*=========================================================================*/
int
qmi_qmux_if_reg_srvc_avail
(
  qmi_connection_id_type conn_id,
  qmi_service_id_type    service_id,
  int                    *qmi_err_code
);

#ifdef __cplusplus
}
#endif

#endif /* QMI_QMUX_IF_H */