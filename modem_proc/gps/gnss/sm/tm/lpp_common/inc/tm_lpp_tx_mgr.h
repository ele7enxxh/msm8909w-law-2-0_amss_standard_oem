#ifndef _TM_LPP_TX_MGR_H
#define _TM_LPP_TX_MGR_H
/*===========================================================================
  
                            TM_LPP_TX_MGR.H  
                   
DESCRIPTION

  
  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/inc/tm_lpp_tx_mgr.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  03/29/16   kpn     Use of subscription ID while handling NAS DL message for LPP CP
  02/12/15   gk      Ack processing incorrect for seq #0
  10/27/14   rk      Added LPP RSTD >24 measurement processing support
  03/05/14   sj      Fixed LPP CP Fuzz crash  
  11/03/11   gk      Initial Checkin
===========================================================================*/

#include "tm_asn1_common.h"
#include "tm_umts_common_utils.h"
#include "tm_lpp_asn1.h"

typedef enum
{
  TM_LPP_SERVER_ACK_NONE,
  TM_LPP_SERVER_ACK_EXPLICIT,
  TM_LPP_SERVER_ACK_IMPLICIT
}tm_lpp_ack_e_type;


#define TM_LPP_TX_MGR_CLIENT_MAX 2
#define TM_LPP_TX_MGR_CLIENT_TYPE_NULL  0
#define TM_LPP_TX_MGR_CLIENT_TYPE_SUPL  1
#define TM_LPP_TX_MGR_CLIENT_TYPE_CP    2
#define TM_LPP_TX_MGR_TRANSACTIONS_MAX 10



typedef enum
{
  TM_LPP_MESG_NONE,
  TM_LPP_MESG_REQ_CAP,
  TM_LPP_MESG_PROV_CAP,
  TM_LPP_MESG_REQ_AD,
  TM_LPP_MESG_PROV_AD,
  TM_LPP_MESG_REQ_LOC_INFO,
  TM_LPP_MESG_PROV_LOC_INFO,
  TM_LPP_MESG_ABORT,
  TM_LPP_MESG_ERROR,
  TM_LPP_MESG_LAST = TM_LPP_MESG_ERROR
}tm_lpp_message_e_type;

typedef enum
{
  TM_LPP_ENCODE_TX,
  TM_LPP_DECODE_TX
} tm_lpp_handle_tx_e_type;

typedef enum
{
  TM_LPP_GET_CLIENT_CAP,
  TM_LPP_GET_CLIENT_AD_LIST,
  TM_LPP_PROV_CLIENT_AD,
  TM_LPP_REQ_CLIENT_LOC_INFO,
  TM_LPP_PROV_LOC_INFO,
  TM_LPP_REQ_CLIENT_CAP ,
  TM_LPP_GET_ERROR_INFO,
  TM_LPP_GET_ABORT_INFO,
  TM_LPP_HANDLE_ERROR,
  TM_LPP_HANDLE_ABORT
} tm_lpp_tx_mgr_client_cmd_e_type;

typedef enum
{
  TM_LPP_TRANS_ID_INIT_SERVER,
  TM_LPP_TRANS_ID_INIT_SET
} tm_lpp_trans_id_e_type;

/* Structure for PD COMM transaction id info */
typedef struct 
{
   struct 
   {
     unsigned u_trans_id_valid : 1;   /* if transaction id is valid. server can send a tx wih no txid.*/
   } m;
  uint8  u_transaction_id;            /* valid value is 0-255 */
  tm_lpp_trans_id_e_type u_initiator; /* if the id is initiated by set or server */
} tm_lpp_transaction_id_s_type;



tm_lpp_transaction_id_s_type z_lpp_transaction_id;

typedef struct 
{
  uint8 u_pos_mode_supported;
} tm_lpp_agnss_cap_s_type;


/* flags for REQ CAP */
#define LPP_REQ_CAP_AGNSS_REQ 0x0001
#define LPP_REQ_CAP_OTDOA_REQ 0x0002
#define LPP_REQ_CAP_ECID_REQ  0x0004
#define LPP_REQ_CAP_EPDU_REQ 0x0008

typedef struct 
{
  uint32 q_flags;
  tm_lpp_agnss_cap_s_type z_agnss_capabilities;
  boolean u_otdoa_supported;
  boolean u_glonass_supported;
  boolean u_ecid_supported;
} tm_lpp_provide_cap_s_type;

#define LPP_PROV_CAP_VALID 0x0001
#define LPP_ASSIST_DATA_VALID 0x0002
#define LPP_CELL_INFO_VALID 0x0004
#define LPP_PHYS_CELL_ID_VALID 0x0008
#define LPP_PRM_DATA_VALID 0x0020
#define LPP_POS_DATA_VALID 0x0040
#define LPP_ERROR_DATA_VALID 0x0080
#define LPP_REQ_LOC_TYPE_VALID 0x0100
#define LPP_ENC_PRM_DATA 0x0200
#define LPP_ENC_OTDOA_DATA 0x0400
#define LPP_ENC_POS_DATA 0x0800
#define LPP_ENC_PROV_LOC_PERIODIC_RPT        0x1000  /* 1st to n-1 periodic report */
#define LPP_ENC_PROV_LOC_PERIODIC_RPT_FINAL  0x2000  /* last periodic report       */
#define LPP_ENC_ECID_DATA 0x4000
#define LPP_ECID_DATA_VALID 0x8000

typedef struct
{
  uint32    q_fix_time_gps_msec; /* GPS time: ms in week */
  uint32    q_fix_time_utc_msec; /* time adjusted for leap seconds */
  uint16    w_GpsWeek;           /* fix week number in GPS time */
  uint32    q_pos_sources;       /* indicate GPS/GLONASS etc */

  float     f_ClockBias;         /* Receiver's clock bias (m) */
  float     f_ClockBiasUncMs;    /* Receiver's clock uncertainty (m) */
  gad_ellip_alt_unc_ellip_type z_pos; /* all data except lat/long/alt are used from this struct
                                         below data w_Alt, q_Lat and l_Long are used for alt/lat/long */
  boolean   vel_valid;
  float     f_VelEnu[3];         /* East, North, Up velocity (m/s) */
  float     f_Vunc3dMps;
  float     f_velVertUnc;

  /* GAD9 values, but LPP needs them in different format than what RRLP uses, hence saving it here */
  uint16    w_Alt;
  uint32    q_Lat;
  int32     l_Long;

  uint32    q_GloTodMs; /* Glonass time of day (milliseconds). 0..86399999.
                           Valid only when q_pos_sources is Glonass only (C_POS_SOURCE_GLO) */
} tm_lpp_pending_pos_to_network_s_type;

#define LPP_ERROR_NO_ERR                     0x00
#define LPP_ERROR_TYPE_LPP_ERROR             0x01
#define LPP_ERROR_TYPE_PROV_LOC_COMMON_ERROR 0x02
#define LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR  0x04
#define LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR  0x08
#define LPP_ERROR_TYPE_PROV_LOC_ECID_ERROR   0x10
#define LPP_ERROR_TYPE_PROV_LOC_EPDU_ERROR   0x20
#define LPP_ERROR_TYPE_LPP_ABORT             0x40

typedef struct
{
  uint8 u_lpp_error_type;
  uint8 u_lpp_error_value;
} tm_lpp_error_type;

typedef struct
{
  boolean                               b_agnss;       /* AGNSS Req Cap present */
  A_GNSS_RequestCapabilities            agnss_req_cap;
  boolean                               b_otdoa;       /* OTDOA Req Cap present */
  OTDOA_RequestCapabilities             otdoa_req_cap;
  boolean                               b_ecid;       /* OTDOA Req Cap present */
  ECID_RequestCapabilities              ecid_req_cap;
} tm_lpp_req_cap_s_type;

#define  C_LPP_AGPS_MODE_UNDEF       0x00
#define  C_LPP_AGPS_MODE_UE_BASED    0x01
#define  C_LPP_AGPS_MODE_UE_ASSISTED 0x02 /* A-GNSS UEA positioning mode*/
#define  C_LPP_AGPS_NO_POSITION      0x04
#define  C_LPP_AGPS_MODE_AUTO        0x08
#define  C_LPP_MODE_UE_OTDOA         0x10 /* OTDOA positiong mode. For OTDOA only pos mode this bit is set & rest are zero. */
#define  C_LPP_MODE_UE_ECID          0x20

typedef struct
{
  uint32 q_flags; /* validity flags for the data below */
  
  tm_lpp_req_cap_s_type                 z_lpp_req_cap;
  
  tm_lpp_provide_cap_s_type *p_lpp_prov_cap_type;
  sm_InternalAssistDataStatusStructType* p_AssistData;   /**< gps assistance data */
  boolean b_otdoa_needed;
  boolean b_earlyResponse_needed;                  /*if earlyResponse is requested by network.*/
  uint16 w_phys_cell_id ;
  cgps_CellInfoCachedStruct z_cached_cell_info;
  sm_loc_lte_otdoa_meas_info_type *p_otoda_data;    /* Pointer to main OTDOA data struct contain RSTD measurements.
                                                       If this pointer is not NULL, then OTDOA data is VALID. */
  boolean b_status_check_for_prm_data;
  sm_GpsMeasRptStructType *p_prm_data;            /* GPS only measurement report. 
                                                     Stop using this field when LPP_CP module needs Glonass MR available
                                                     in p_gnss_meas_report field.*/
  SmGnssMeasReportStructType *p_gnss_meas_report; /* GNSS measurement report. data is copied
                                     from a static buffer. no dynamic memory allocation  */
  tm_lpp_pending_pos_to_network_s_type  z_pos_info;

  tm_pdu_buf_type  z_pdu_buf;                   /* ASN.1 encoded LPP_Message */
  LPP_Message *p_pdu_ptr;                       /* OSYS memory for decoded LPP_Message */
  uint32                                req_loc_type;
  tm_lpp_error_type lpp_error;
  uint8         req_loc_gnss_method_bmap;             /* Store server choice of gnss_Methods (GPS, Glonass) type from 
                                                         LPP ReqLocInfo. Uses C_POS_SOURCE_XXX bit positions to 
                                                         set/unset gnss methods */

  /* Pointers below used only for LPP UP!!! */
  /***************** Pointers below used only for LPP UP!!! *****************/
  GNSS_AuxiliaryInformation *p_gnss_auxi_info;        /* Hold Auxiliary info AD until AcqAssist is available to process 
                                                         & post the steering info to MGP */
  GNSS_AcquisitionAssistance z_gnss_acqassist;  /* Store AcqAssist AD until GPS UTCModel or Glonass TimeModel AD 
                                                         is available */
  /* Pointers above used only for LPP UP!!! */
  /***************** Pointers below used only for LPP UP!!! *****************/
  gnss_lpp_ECID_ProvideLocationInformation *p_ecid_data;
} tm_lpp_data_type;

#define TM_LPP_SESSION_TYPE_UP (0)
#define TM_LPP_SESSION_TYPE_CP (1)

typedef enum
{
  TM_LPP_ACK_RSP_NONE,
  TM_LPP_ACK_RSP_EXPLICIT,
  TM_LPP_ACK_RSP_IMPLICIT
} tm_lpp_ack_rsp_type;

typedef struct
{
  uint8 u_client_handle; 
  boolean b_in_use; /* transaction info in use*/
  tm_lpp_transaction_id_s_type z_current_trans_id; /* transaction id */
  boolean b_new_tx;
  tm_lpp_message_e_type        e_messages_id; /* messages type. */ 
  tm_lpp_message_e_type        e_next_messages_id; /* messages type. */
  uint8                        u_lpp_session_type;/* TM_LPP_SESSION_TYPE_UP (CP) */
  os_TimerMsgType*    p_lpp_transaction_timer; /* ?? */
  void                *p_session_info_ptr; /* SUPL session pointer */
  boolean             b_abnomal_transaction; /* if this transaction has abnormal/error condition */
  boolean             b_end_sent; /* if end is sent for this transaction */
  boolean             b_unsolicited; /* if this is an unsolicited transaction and no resp is reqd */
  boolean             b_pending_ue_tx; 
  /* this value is set to TRUE if we have to still send a response to server  */
  boolean             b_pending_server_tx; 
  /* this value is set to TRUE if we have to still recv response from server  */
  uint8               u_pos_mode; /* Positioning mode for the session */
  tm_lpp_data_type    z_lpp_data_type; /* lpp_data */

  /* for LPP CP use only */  
  uint8                        u_sequence_num;              /* incoming sequence number */
  uint8                        u_out_sequence_num;          /* outgoing sequence number */
  uint8                        u_wait_ack;                  /* bit 0 - Wait server ACK */
  uint32                       nas_session_id;              /* NAS ID */
  boolean                      b_in_sequence_num_valid;    /* if incoming seq # is valid */
  boolean                      b_out_sequence_num_valid;    /* if outgoing seq # is valid */
  uint32                       q_timestamp;                 /* Timestamp of the transaction */
  sys_modem_as_id_e_type       e_as_id;                     /* Subscripton Id.*/
} tm_lpp_transaction_info_s_type;
                  
typedef struct tm_lpp_transaction_list_s
{
  tm_lpp_transaction_info_s_type     elem;
  struct tm_lpp_transaction_list_s  *next;  /* next tm_lpp_transaction_list_s_type in the transaction list */
} tm_lpp_transaction_list_s_type;

typedef struct tm_lpp_transaction_list_summary_s
{
  uint8                           num_trans;
  tm_lpp_transaction_list_s_type  *head;
} tm_lpp_transaction_list_summary_s_type;


/* Dynamic linked list of outstanding LPP transaction for a given LPP Session */  
extern tm_lpp_transaction_list_summary_s_type   tm_lpp_tx_summary;
/* client call back function that can be called by the LPP transaction manager once LPP transaction      
     mananger is done with this transaction 
*/
typedef boolean (tm_lpp_tx_mgr_client_cb_func) (

    tm_lpp_tx_mgr_client_cmd_e_type  e_cmd_type,

    tm_lpp_transaction_info_s_type *p_lpp_tx_type
);

typedef struct
{
  uint8 client_id; /*A unique client id*/  
  /* callback fn ptr */
     /**< callback notification function */
   tm_lpp_tx_mgr_client_cb_func *client_cb_func; 
    
} tm_lpp_tx_mgr_client_type;

typedef struct
{
  boolean b_in_use;
  tm_lpp_tx_mgr_client_type client;
  
} tm_lpp_tx_mgr_client_list_type;

/* Number of LPP clients. */
extern tm_lpp_tx_mgr_client_list_type tm_lpp_tx_mgr_client_list[TM_LPP_TX_MGR_CLIENT_MAX];


/*===========================================================================
  tm_lpp_tx_mgr_init
 
  Description:
    LPP function to init the transaction manager 
  
  Parameters
 
  Returns 
    none
===========================================================================*/
void tm_lpp_tx_mgr_init(uint8 u_client_id);


/*===========================================================================
  tm_lpp_decode message

  Description:

     This function is called by the LPP transaction manager to decode LPP transaction

  Parameters: 

     *p_payload - payload of the LPP transaction
      *decode_status_ptr - status of the decoding

  Return value:                                 
     pointer to the decoded message                            
                                 
=============================================================================*/
void *  tm_lpp_decode_message (const tm_pdu_buf_type *p_payload, 
                                /**< encoded data */
                                int *decode_status_ptr
);



/*===========================================================================
  tm_lpp_tx_mgr_handle_encode_transaction

  Description:

     This function is called by the LPP clients to process a LPP transaction

  Parameters: 
      client_handle - handle to the client
      message_type - message to be encoded
      p_lpp_tx_id  - pointer to transaction id of the handle
      p_lpp_tx_type - transaction table element
      lpp session type - UP or CP

  Return value: none
=============================================================================*/
boolean tm_lpp_tx_mgr_handle_encode_transaction ( 
      uint8 u_client_handle,
      tm_lpp_message_e_type z_message_type,
      tm_lpp_transaction_id_s_type *p_lpp_tx_id,
      tm_lpp_transaction_info_s_type **p_lpp_tx_type,
      uint8                         lpp_session_type
);

/*===========================================================================
  tm_lpp_tx_mgr_handle_decode_transaction

  Description:

     This function is called by the LPP clients to process a LPP transaction

  Parameters: 
      client_handle - handle to the client
      p_payload - lpp payload to be decoded
      p_lpp_tx_type - pointer to the transaction table
      lpp session type - UP or CP      
      tm_lpp_error_type - Pointer to LPP Error found during decode

  Return value: none
=============================================================================*/
 boolean tm_lpp_tx_mgr_handle_decode_transaction ( 
      uint8 u_client_handle,
      const tm_pdu_buf_type *p_payload,
      tm_lpp_transaction_info_s_type **p_lpp_tx_type,
      uint8                         lpp_session_type,
      tm_lpp_error_type              *p_lpp_error
);


/*===========================================================================
  tm_lpp_tx_mgr_client_register

  Description:

     This function is used by clients to register with LPP transaction manager

  Parameters: 

  *p_client: Pointer to client structure
  *handle: Pointer to client handle which will be filled by this function

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_lpp_tx_mgr_client_register(const tm_lpp_tx_mgr_client_type * p_client, uint8 *handle);


/*===========================================================================
  tm_lpp_tx_mgr_client_deregister

  Description:

     This function is used by clients to deregister to Geofencing Services.

  Parameters: 

    uint8 client_id: A unique client identifier.

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_lpp_tx_mgr_client_deregister(uint8 client_handle);


/*===========================================================================
  tm_lpp_tx_mgr_client_deregister

  Description:

     This function is used by clients to deregister to Geofencing Services.

  Parameters: 

    uint8 client_id: A unique client identifier.
    uint8* error_coe: Error code in case of error will be returned in this param.

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
tm_lpp_tx_mgr_client_type *tm_lpp_tx_mgr_client_get_client_ptr(uint8 client_handle);


/*=========================================================================
  tm_lpp_encode_ack

  Description:
    LPP function to encode Explicit LPP ACK message.
    
  Returns:
    TRUE if the encoding passed, FALSE otherwise    
==========================================================================*/
boolean tm_lpp_encode_ack (uint8 u_seq_number, tm_pdu_buf_type  *p_pdu_buf, boolean end_flag);

/*===========================================================================
  tm_lpp_tx_mgr_find_seq_number
 
  Description: 
    LPP function to find the transaction with matching sequence number.
  
  Parameters
     u_directionn - 0 - incoming ; 1 - outgoing
     u_seq_number - sequence number to search for a match
    **p_lpp_tx_type- pointer to the transaction table with a match
 
  Returns
    TRUE if a match, FALSE otherwise
=========================================================================*/
boolean tm_lpp_tx_mgr_find_seq_number( 
  uint8                           u_direction,
  uint8                           u_seq_number,
  tm_lpp_transaction_info_s_type **p_lpp_tx_type
);

/*===========================================================================
  tm_lpp_tx_mgr_remove_entry_by_clientid
 
  Description: 
    Remove all transactions associated with the client id.
 
  Parameters:
   u_client_id - client id.
   
  Returns: 
   None
=============================================================================*/
void tm_lpp_tx_mgr_remove_entry_by_clientid( uint8 const u_client_id );

/*===========================================================================
  tm_lpp_tx_mgr_remove_entry
 
  Description: 
    LPP function to remove one or all transactions associated with a transaction id.
 
  Parameters:
   *p_lpp_tx_type- pointer to the transaction to be removed
   
  Returns: 
   None
=============================================================================*/
void tm_lpp_tx_mgr_remove_entry(tm_lpp_transaction_info_s_type **p_lpp_tx_to_remove);

/*===========================================================================
  tm_lpp_tx_mgr_get_num_tx
 
  Description: 
    LPP function to get number of outstanding transactions associated with a client 
  Parameters:
  u_client_handle - Client Handle
                                 
  Returns: 
   uint8 - number of transaction entries 
=============================================================================*/

uint8 tm_lpp_tx_mgr_get_num_tx(
        uint8 u_client_handle);


/*===========================================================================
  tm_lpp_tx_mgr_find_entry_by_refloc
 
  Description: 
    Function to find the transaction in the table by Request Location Type.
  Parameters:
    req_loc_type - Find the LPP RLI transaction of this type.
    u_client_handle - Client handle
   
  Returns: 
   None
=============================================================================*/
tm_lpp_transaction_info_s_type* tm_lpp_tx_mgr_find_entry_by_refloc( uint32 const req_loc_type, uint8 const u_client_handle );

/*===========================================================================
  tm_lpp_tx_mgr_insert_entry
 
  Description: 
    LPP function to insert a transaction in transaction 
          manager table
 
  Parameters:
   *p_lpp_tx_id  - pointer to transaction id type
   *p_lpp_tx_type- pointer to the transaction table
    client_handle - the client_handle to which this transaction belongs to

  Returns: 
    Returns the TRUE or FALSE. 
=============================================================================*/
boolean  tm_lpp_tx_mgr_insert_entry( const tm_lpp_transaction_id_s_type *p_lpp_tx_id,
                                     tm_lpp_transaction_info_s_type **p_lpp_tx_type,
                                     uint8 u_client_handle );
#endif 
