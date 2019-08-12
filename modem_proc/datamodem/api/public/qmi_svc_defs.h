#ifndef QMI_SVC_DEFS_H
#define QMI_SVC_DEFS_H
/*===========================================================================

                         Q M I _ S V C _ D E F S . H

DESCRIPTION

 The QMI Service Definitions header file. It Contains common service definitions
 and types to all QMI services.
 
EXTERNALIZED DATA 
  qmi_result_e_type
    Enumeration for qmi result code to service response.
 
  qmi_error_e_type
    Enumeration for qmi Error code to service response.
 
  qmux_service_e_type
    Enum for QMI Service type.
 
  qmi_cmd_buf_type
    Type for QMI Cmd buf.
 
  qmi_transaction_type
    Type for QMI service transaction.
 
  qmi_common_client_state_type
    Type for QMI client state.
 
  qmi_common_svc_state_type
    Type for QMI service state.
 
  qmi_svc_hdlr_ftype
    QMI service command handler type.   
 
  qmux_version_type
    Type for version definition.
 
  qmux_svc_cbs_type
    Callback functions for QMI service configuration.
 
  qmux_svc_config_type
    QMI Service config type.
 
  qmux_svc_info_type
    QMI Service Info type.
 
Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  
when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
03/14/12    wc      Increase QMUX_SERVICE_QC_MAX
02/20/12    sb      Fix for Invalid errors from QMI PBM.
01/05/12    sa      Added QMUX_SERVICE_CSVT
09/14/11    sy      Added QMUX_SERVICE_WDA.
08/30/11    rk      Added bundle message support to QMI Framework   
07/19/11    wc      Merged PB error codes from other branches
01/20/11    rk      Created Module 
===========================================================================*/

#include "comdef.h"
#include "dsm.h"
#include "queue.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*--------------------------------------------------------------------------- 
  QMI_TYPE_* - for common tlv definition to all qmi services.
---------------------------------------------------------------------------*/                        
#define QMI_TYPE_REQUIRED_PARAMETERS  (0x01)
#define QMI_TYPE_RESULT_CODE          (0x02)
#define QMI_TYPE_EXT_ERROR_INFO       (0xE0)
#define QMI_TYPE_PARTIAL_OPER_FAILURE (0xE1)

/*---------------------------------------------------------------------------
  QMI MSG CONTROL FLAGS 
---------------------------------------------------------------------------*/                        
#define QMI_FLAG_MASK_COMPOUND        (0x01)
#define QMI_FLAG_MASK_MSGTYPE         (0x06)

#define QMI_FLAG_MSGTYPE_CMD          (0x00)
#define QMI_FLAG_MSGTYPE_RESP         (0x02)
#define QMI_FLAG_MSGTYPE_IND          (0x04)

/*---------------------------------------------------------------------------
  QMI_MAX_* - constants for QMI service transactions and commands.
---------------------------------------------------------------------------*/                                                   
#define MAX_PENDING_TRANSACTIONS (15)
#define MAX_PENDING_COMMANDS     (18)
#define MAX_MSGS_IN_BUNDLE       (3)

/* With QMI_ Prefix */
#define QMI_MAX_PENDING_TRANSACTIONS    MAX_PENDING_TRANSACTIONS
#define QMI_MAX_PENDING_COMMANDS        MAX_PENDING_COMMANDS
#define QMI_MAX_MSGS_IN_BUNDLE          MAX_MSGS_IN_BUNDLE

/*---------------------------------------------------------------------------
  Special client ID value to indicate client state is "not in use".  This 
  value may not be assigned as a valid client ID by a generic QMI service.
---------------------------------------------------------------------------*/
#define QMI_SVC_CLID_UNUSED    (0)     /* make sure alloc skips broadcast  */
#define QMI_SVC_CLID_BROADCAST (0xFF)  /* special broadcast client ID      */

/*---------------------------------------------------------------------------
  Command handler return value to indicate response is pending
  Also used as special values for QMI command buffer response pointer
---------------------------------------------------------------------------*/
#define QMI_SVC_RESPONSE_PENDING  ((void *) (0x00000001LU))
#define QMI_SVC_RESPONSE_NONE     ((void *) (0x00000000LU))

/*===========================================================================

                               DATA TYPES

===========================================================================*/

/*===========================================================================
TYPEDEF QMI_RESULT_E_TYPE

DESCRIPTION
  Enumeration for qmi result code to service response.
===========================================================================*/
typedef enum
{
  QMI_RESULT_SUCCESS  = 0,
  QMI_RESULT_FAILURE  = 1,
  QMI_RESULT_MAX,
  QMI_RESULT_WIDTH    = 0xFFFF
} qmi_result_e_type;

/*===========================================================================
TYPEDEF QMI_ERROR_E_TYPE

DESCRIPTION
  Enumeration for qmi Error code to service response.
===========================================================================*/
typedef enum
{
  QMI_ERR_NONE                            = 0,
  QMI_ERR_MALFORMED_MSG                   = 1,
  QMI_ERR_NO_MEMORY                       = 2,
  QMI_ERR_INTERNAL                        = 3,
  QMI_ERR_ABORTED                         = 4,
  QMI_ERR_CLIENT_IDS_EXHAUSTED            = 5,
  QMI_ERR_UNABORTABLE_TRANSACTION         = 6,
  QMI_ERR_INVALID_CLIENT_ID               = 7,
  QMI_ERR_NO_THRESHOLDS                   = 8,
  QMI_ERR_INVALID_HANDLE                  = 9,
  QMI_ERR_INVALID_PROFILE                 = 10,
  QMI_ERR_INVALID_PINID                   = 11,
  QMI_ERR_INCORRECT_PIN                   = 12,
  QMI_ERR_NO_NETWORK_FOUND                = 13,
  QMI_ERR_CALL_FAILED                     = 14,
  QMI_ERR_OUT_OF_CALL                     = 15,
  QMI_ERR_NOT_PROVISIONED                 = 16,  
  QMI_ERR_MISSING_ARG                     = 17,
  QMI_ERR_ARG_TOO_LONG                    = 19,
  QMI_ERR_INVALID_TX_ID                   = 22,
  QMI_ERR_DEVICE_IN_USE                   = 23,
  QMI_ERR_OP_NETWORK_UNSUPPORTED          = 24,
  QMI_ERR_OP_DEVICE_UNSUPPORTED           = 25,
  QMI_ERR_NO_EFFECT                       = 26,
  QMI_ERR_NO_FREE_PROFILE                 = 27,
  QMI_ERR_INVALID_PDP_TYPE                = 28,
  QMI_ERR_INVALID_TECH_PREF               = 29,
  QMI_ERR_INVALID_PROFILE_TYPE            = 30,
  QMI_ERR_INVALID_SERVICE_TYPE            = 31,
  QMI_ERR_INVALID_REGISTER_ACTION         = 32,
  QMI_ERR_INVALID_PS_ATTACH_ACTION        = 33,
  QMI_ERR_AUTHENTICATION_FAILED           = 34,
  QMI_ERR_PIN_BLOCKED                     = 35,
  QMI_ERR_PIN_PERM_BLOCKED                = 36,
  QMI_ERR_SIM_NOT_INITIALIZED             = 37,
  QMI_ERR_MAX_QOS_REQUESTS_IN_USE         = 38,
  QMI_ERR_INCORRECT_FLOW_FILTER           = 39,
  QMI_ERR_NETWORK_QOS_UNAWARE             = 40,
  QMI_ERR_INVALID_ID                      = 41, 
  QMI_ERR_INVALID_QOS_ID                  = QMI_ERR_INVALID_ID,
  QMI_ERR_REQUESTED_NUM_UNSUPPORTED       = 42,
  QMI_ERR_INTERFACE_NOT_FOUND             = 43,
  QMI_ERR_FLOW_SUSPENDED                  = 44,
  QMI_ERR_INVALID_DATA_FORMAT             = 45,
  QMI_ERR_GENERAL                         = 46,
  QMI_ERR_UNKNOWN                         = 47,
  QMI_ERR_INVALID_ARG                     = 48,
  QMI_ERR_INVALID_INDEX                   = 49,
  QMI_ERR_NO_ENTRY                        = 50,
  QMI_ERR_DEVICE_STORAGE_FULL             = 51,
  QMI_ERR_DEVICE_NOT_READY                = 52,
  QMI_ERR_NETWORK_NOT_READY               = 53,
  QMI_ERR_CAUSE_CODE                      = 54,
  QMI_ERR_MESSAGE_NOT_SENT                = 55,
  QMI_ERR_MESSAGE_DELIVERY_FAILURE        = 56,
  QMI_ERR_INVALID_MESSAGE_ID              = 57,
  QMI_ERR_ENCODING                        = 58,
  QMI_ERR_AUTHENTICATION_LOCK             = 59,
  QMI_ERR_INVALID_TRANSITION              = 60,
  QMI_ERR_NOT_A_MCAST_IFACE               = 61,
  QMI_ERR_MAX_MCAST_REQUESTS_IN_USE       = 62,
  QMI_ERR_INVALID_MCAST_HANDLE            = 63, 
  QMI_ERR_INVALID_IP_FAMILY_PREF          = 64,
  QMI_ERR_SESSION_INACTIVE                = 65,
  QMI_ERR_SESSION_INVALID                 = 66,
  QMI_ERR_SESSION_OWNERSHIP               = 67,
  QMI_ERR_INSUFFICIENT_RESOURCES          = 68,
  QMI_ERR_DISABLED                        = 69,
  QMI_ERR_INVALID_OPERATION               = 70,
  QMI_ERR_INVALID_QMI_CMD                 = 71,
  QMI_ERR_TPDU_TYPE                       = 72,
  QMI_ERR_SMSC_ADDR                       = 73,
  QMI_ERR_INFO_UNAVAILABLE                = 74,
  QMI_ERR_SEGMENT_TOO_LONG                = 75,
  QMI_ERR_SEGMENT_ORDER                   = 76,
  QMI_ERR_BUNDLING_NOT_SUPPORTED          = 77,
  QMI_ERR_OP_PARTIAL_FAILURE              = 78,
  QMI_ERR_POLICY_MISMATCH                 = 79,
  QMI_ERR_SIM_FILE_NOT_FOUND              = 80,
  QMI_ERR_EXTENDED_INTERNAL               = 81,
  QMI_ERR_ACCESS_DENIED                   = 82,
  QMI_ERR_HARDWARE_RESTRICTED             = 83, 
  QMI_ERR_ACK_NOT_SENT                    = 84,
  QMI_ERR_INJECT_TIMEOUT                  = 85,
  /* Sups Errors  */
  QMI_ERR_INCOMPATIBLE_STATE              = 90,
  QMI_ERR_FDN_RESTRICT                    = 91,
  QMI_ERR_SUPS_FAILURE_CAUSE              = 92,
  QMI_ERR_NO_RADIO                        = 93,
  QMI_ERR_NOT_SUPPORTED                   = 94,
  QMI_ERR_NO_SUBSCRIPTION                 = 95,
  QMI_ERR_CARD_CALL_CONTROL_FAILED        = 96,
  QMI_ERR_NETWORK_ABORTED                 = 97,
  QMI_ERR_MSG_BLOCKED                     = 98,
  /* Error 99 is open */
  /* QMI PBM Errors */
  QMI_ERR_INVALID_SESSION_TYPE            = 100,
  QMI_ERR_INVALID_PB_TYPE                 = 101,
  QMI_ERR_NO_SIM                          = 102,
  QMI_ERR_PB_NOT_READY                    = 103,
  QMI_ERR_PIN_RESTRICTION                 = 104,
  QMI_ERR_PIN2_RESTRICTION                = 105,
  QMI_ERR_PUK_RESTRICTION                 = 106,
  QMI_ERR_PUK2_RESTRICTION                = 107,
  QMI_ERR_PB_ACCESS_RESTRICTED            = 108,
  QMI_ERR_PB_DELETE_IN_PROG               = 109,
  QMI_ERR_PB_TEXT_TOO_LONG                = 110,
  QMI_ERR_PB_NUMBER_TOO_LONG              = 111,
  QMI_ERR_PB_HIDDEN_KEY_RESTRICTION       = 112,
  QMI_ERR_PB_NOT_AVAILABLE                = 113,
  QMI_ERR_DEVICE_MEMORY_ERROR             = 114,
  /* New error codes must only be added based on enums defined in 
   * common_v01.idl. For new PLs/branches the common_v01.h defined
   * enums are preferred. 
   */
  QMI_ERR_MAX,
  QMI_ERR_WIDTH                           = 0xFFFF
} qmi_error_e_type;

/*===========================================================================
TYPEDEF QMI_SERVICE_E_TYPE

DESCRIPTION
  Enum for QMI Service type.
  Important Note: DO NOT change Qualcomm predefined values. 
===========================================================================*/
typedef enum
{
  QMUX_SERVICE_MIN        = 0,
  QMUX_SERVICE_CTL        = 0,             /* QMI service control           */
  QMUX_SERVICE_WDS        = 1,             /* Wireless Data service         */
  QMUX_SERVICE_DMS        = 2,             /* Device Management service     */
  QMUX_SERVICE_NAS        = 3,             /* Network Access service        */
  QMUX_SERVICE_QOS        = 4,             /* Quality Of Service            */
  QMUX_SERVICE_WMS        = 5,             /* Wireless Message Service      */
  QMUX_SERVICE_PDS        = 6,             /* Position Determination Service*/
  QMUX_SERVICE_AUTH       = 7,             /* AUTH service                  */
  QMUX_SERVICE_AT         = 8,             /* Atcop Service                 */
  QMUX_SERVICE_VOICE      = 9,             /* Voice service                 */
  QMUX_SERVICE_CAT        = 10,            /* Card Apps Toolkit Service     */
  QMUX_SERVICE_UIM        = 11,            /* UIM service                   */
  QMUX_SERVICE_PBM        = 12,            /* Phone Book Manager service    */
                                           /* 13 to 16 reserved IDs         */
  QMUX_SERVICE_SAR        = 17,            /* Specific Absorption Rate service */
  QMUX_SERVICE_WDA        = 26,            /* Wireless data administrative service */
  QMUX_SERVICE_CSVT       = 29,            /* Circuit-switched Video Telephony Service */
  QMUX_SERVICE_CAT2       = 224,           /* Card Apps Toolkit Service 2   */

  /*-----------------------------------------------------------------------
    ATTENTION VENDORS: 
    - Maximum number of vendor-specific services is 27.
    - Any new vendor services must be within QMUX_SERVICE_VENDOR_MIN and 
      QMUX_SERVICE_VENDOR_MAX.  
    - The first vendor service must be assigned a value of 
      QMUX_SERVICE_VENDOR_MIN(=227) Eg: QMUX_VS_SERVICE_1 = 227, etc.
    - QMUX_SERVICE_VENDOR_MAX must have value of 
      QMUX_SERVICE_VENDOR_MIN + number of vendor services
    - For example if there are no vendor services then 
      QMUX_SERVICE_VENDOR_MAX has value of QMUX_SERVICE_VENDOR_MIN
  -----------------------------------------------------------------------*/
  QMUX_SERVICE_VENDOR_MIN = 227,
  QMUX_SERVICE_VENDOR_MAX = 227,

  /*-----------------------------------------------------------------------
    Maximum QMUX service must have a value less than 254.
  -----------------------------------------------------------------------*/
  QMUX_SERVICE_MAX        = QMUX_SERVICE_VENDOR_MAX
} qmux_service_e_type;

/*===========================================================================

                             QMI CLIENT STATE TYPES

===========================================================================*/

/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/
typedef struct qmi_cmd_buf_s             qmi_cmd_buf_type;
typedef struct qmi_transaction_s         qmi_transaction_type;
typedef struct qmi_common_client_state_s qmi_common_client_state_type;

/*===========================================================================
TYPEDEF QMI_TRANSACTION_TYPE

DESCRIPTION
  The QMI transaction type.
  Each Transaction has a link to common client state, service state.
  Command list, req and response list.
  It has a transaction ID and control flag.
===========================================================================*/
struct qmi_transaction_s
{
  q_link_type                    link;  
  qmi_common_client_state_type * cl_sp;       /* common client state ptr */
  void *                         svc_sp;      /* Service state ptr */
  uint16                         x_id;        /* transaction ID - 0 means not in use */
  uint8                          ctl;         /* control flags  */
  byte                           n_cmds;      /* no of commands in the rx'ed bundle */
  qmi_cmd_buf_type *             cmd_list[MAX_MSGS_IN_BUNDLE]; /* cmd list   */
  dsm_item_type *                req_list[MAX_MSGS_IN_BUNDLE]; /* req list   */
  dsm_item_type *                resp_list[MAX_MSGS_IN_BUNDLE];/* resp list  */
};

/*===========================================================================
TYPEDEF QMI_CMD_BUF_TYPE

DESCRIPTION
  The QMI command buf type.
  Each command buf a link to Transaction.
  It has a command type, ID and progress status.
===========================================================================*/
struct qmi_cmd_buf_s
{
  q_link_type                   link;         /* must be first            */
  qmi_transaction_type *        x_p;          /* Transaction ptr          */
  uint8                         cmd_no;       /* index into x_p->cmd_list */
  uint16                        cmd_type;     /* Command Type             */
  boolean                       in_progress;  /* Status of the Command    */
};

/*===========================================================================
TYPEDEF QMI_COMMON_CLIENT_STATE_TYPE

DESCRIPTION
  The type has QMI common instance state definition & client state definition.
  Each state link to transaction and command buf type.
  
===========================================================================*/
struct qmi_common_client_state_s
{
  uint8     clid;                          /* client id */
  qmi_transaction_type  transactions[MAX_PENDING_TRANSACTIONS];
  qmi_cmd_buf_type      cmd_buf[MAX_PENDING_COMMANDS];
  q_type  x_free_q;
  q_type  cmd_free_q;
  uint16   seq_no;
};

/*===========================================================================

                             QMI SERVICE STATE TYPES

===========================================================================*/

/*===========================================================================
TYPEDEF QMI_COMMON_SVC_STATE_TYPE

DESCRIPTION
  Type for common qmi service state.
===========================================================================*/
typedef struct
{
  void *                      qmux_svc_handle;
  uint8                       next_clid;
  qmi_cmd_buf_type            bcast_ind_buf; 
  qmi_transaction_type        bcast_transaction; 
  boolean                     bcast_transaction_buf_free;
} qmi_common_svc_state_type;

/*===========================================================================
TYPEDEF QMI_SVC_HDLR_FTYPE

DESCRIPTION
  QMI service command handler type.
 
  Request Handler Parameters:
    sp         : Service provided state pointer.
    cmd_buf_p  : Command buffer.
    cl_sp      : Client state pointer.
    sdu        : DSM item containing request to be processed.

  The sp and cl_sp should be appropriately casted to the service state 
  and client state structures respectively in the command handlers.
---------------------------------------------------------------------------*/
typedef dsm_item_type* (* qmi_svc_hdlr_ftype)
                          ( 
                            void * sp,
                            void * cmd_buf_p,
                            void * cl_sp,
                            dsm_item_type ** sdu 
                          );

/*===========================================================================
TYPEDEF QMI_SVC_CMD_HDLR_TYPE

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/
typedef struct
{
  uint16              cmd_type;
  char *              msg_desc;
  qmi_svc_hdlr_ftype  request_hdlr;
} qmi_svc_cmd_hdlr_type;

/*===========================================================================

                             QMI SERVICE CONFIGURATION TYPES

===========================================================================*/

/*===========================================================================
TYPEDEF QMI_VERSION_TYPE

DESCRIPTION
  type for version definition.
===========================================================================*/
typedef struct
{
  uint16  major; 
  uint16  minor; 
} qmux_version_type;

/*===========================================================================
TYPEDEF QMUX_SVC_CBS_TYPE

DESCRIPTION 
  Callback functions for service configuration. 
  A service can opt to not register for the below config members by defining
  them as NULL or zero appropriately.
  Note: This callback can not used by QMI new Framework.
---------------------------------------------------------------------------*/
typedef struct _qmux_svc_callbacks
{
  byte    (* alloc_clid)         ( void * sp );
  boolean (* is_valid_clid)      ( void * sp, byte clid );
  boolean (* dealloc_clid)       ( void * sp, byte clid );
  void    (* qmux_closed)        ( void * sp );
  void    (* recv)               ( void * sp, byte clid, dsm_item_type * sdu_in );
  void *  (* get_next_client_sp) ( void * sp, void * cl_sp);
  void    (* reset_client)       ( void *);
} qmux_svc_cbs_type;

/*===========================================================================
TYPEDEF QMUX_SVC_CONFIG_TYPE

DESCRIPTION 
  QMI Service config type. 
  Note: This callback can not used by QMI new Framework.  
---------------------------------------------------------------------------*/
typedef struct
{
  qmux_version_type         base_version;     /* Base version               */ 
  qmux_version_type         addendum_version; /* Addendum version           */ 
  qmux_svc_cbs_type         cbs;              /* Call back used for svc reg */ 
  qmi_svc_cmd_hdlr_type *   cmd_hdlr_array;   /* service cmd hdlr array     */ 
  uint16                    cmd_num_entries;  /* Number of cmds per array   */ 
  void *                    sp;               /* service provided state 
                                                         pointer(user data) */
} qmux_svc_config_type;

/*===========================================================================
TYPEDEF QMUX_SVC_INFO_TYPE

DESCRIPTION 
  QMI Service Info type. 
---------------------------------------------------------------------------*/
typedef struct
{
  boolean               registered;   /* Service registration statu   */ 
  qmux_service_e_type   service_id;   /* Service Type                 */ 
  qmux_svc_config_type  cfg;          /* Service Configuration        */ 
} qmux_svc_info_type;

#endif /* QMI_SVC_DEFS_H */
