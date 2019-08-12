/*===========================================================================

                         Q M I _ P B M . C

DESCRIPTION

 The Qualcomm MSM Interface Phone Book Manager source
 file.

EXTERNALIZED FUNCTIONS

  qmi_pbm_init()
    Register the PBM service with QMUX for all applicable QMI links


Copyright (c) 2014,2015 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/25/16    sp     F3 improvements
11/17/15    stv    Logging improvements
08/05/15    nr     Operator specifc NV ECC numbers configuration
03/10/15    nr     Cross mapping changes
01/13/15    stv    Need not clear the transaction info when client is invalid
11/14/14    NR     Fixing medium and low level compiler warnings
09/29/14    av     Featurize all QMI messages
09/18/14   stv     Fix for logging strings in F3s
07/25/14    NR     Use v2.0 diag macros
07/07/14    NR     Fixing medium and low level compiler warnings
06/08/14    stv    handle idl encode failure due to dsm buffer exhaustion gracefully
05/01/14    NR     FR 19033: Reduce Code Duplication pbm
04/10/14    stv    basic capability info tlv issue during bind subs in get_all_pb_capabilities
03/26/14    stv    hidden capabilities tlv in get_pb and get_all_pb_capabilities
04/04/14    stv    Migration to DATA exposed API usage
02/28/14    stv    get_pb_capabilities new tlv addition packing issue
                   addition of encode/decode idl libraries
01/17/14    NR     LLVM Compiler warnings
01/14/14    NR     QMI-PBM Read Indication is missing when trying to read
                   same Phone Book and Same Session
03/23/10    sk     Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "uim_msg.h"
#include "qmi_pbm.h"
#include "qmi_pbm_if.h"
#include "dsm.h"
#include "nv.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "qmi_pbm_if.h"
#include "qmi_pbm_svc.h"
#include "modem_mem.h"
#include "qmi_pbm_task_cmd.h"
#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "phonebook_manager_service_v01.h"
#include "phonebook_manager_service_impl_v01.h"
#include "stringl/stringl.h"
#include "pbmlib.h"
#include "ds_qmi_fw_common.h"
/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define PBMI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for PBM
---------------------------------------------------------------------------*/
#define PBMI_BASE_VER_MAJOR    (1)
#define PBMI_BASE_VER_MINOR    (4)

#define PBMI_ADDENDUM_VER_MAJOR  (0)
#define PBMI_ADDENDUM_VER_MINOR  (0)

/*---------------------------------------------------------------------------
  DSDS related constants
---------------------------------------------------------------------------*/

#define PBMI_PRIM_SUBS 0
#define PBMI_SEC_SUBS  1
#define PBMI_TERITARY_SUBS  2
#define PBMI_INVALID_SUBS  0xFF


/*---------------------------------------------------------------------------
  Optional TLVs
---------------------------------------------------------------------------*/
#define  PBMI_IND_REG_MASK          0x10

#define  PBMI_GET_CAPS_BASIC         0x10
#define  PBMI_GET_CAPS_GRP           0x11
#define  PBMI_GET_CAPS_AD_NUM        0x12
#define  PBMI_GET_CAPS_EMAIL         0x13
#define  PBMI_GET_CAPS_SECOND_NAME   0x14
#define  PBMI_GET_CAPS_HIDDEN_ENTRY  0x15
#define  PBMI_GET_CAPS_GAS           0x16
#define  PBMI_GET_CAPS_AAS           0x17
#define  PBMI_GET_CAPS_WRITE_ACCESS  0x18
#define  PBMI_GET_CAPS_NUM_PBSETS    0x19
#define  PBMI_GET_CAPS_PBSET_INFO    0x1A


#define  PBMI_REC_READ_SNAME         0x10
#define  PBMI_REC_READ_AD_NUM        0x11
#define  PBMI_REC_READ_GRP           0x12
#define  PBMI_REC_READ_EMAIL         0x13
#define  PBMI_REC_READ_HIDDEN        0x14

#define  PBMI_REC_READ_EXT_UNDECODED_SNAME         0x10
#define  PBMI_REC_READ_EXT_UNDECODED_AD_NUM        0x11
#define  PBMI_REC_READ_EXT_UNDECODED_GRP           0x12
#define  PBMI_REC_READ_EXT_UNDECODED_EMAIL         0x13
#define  PBMI_REC_READ_EXT_UNDECODED_HIDDEN        0x14

#define  PBMI_READ_RECS_RSP_NUM_RECS 0x10

#define  PBMI_READ_RECS_EXT_UNDECODED_RSP_NUM_RECS 0x10

#define  PBMI_READ_PBSET_CAPS_START_ID     0x10
#define  PBMI_READ_PBSET_CAPS_END_ID       0x11
#define  PBMI_READ_PBSET_CAPS_RSP_NUM_PBSETS 0x10


#define  PBMI_WRITE_REC_SNAME        0x10
#define  PBMI_WRITE_REC_AD_NUM       0x11
#define  PBMI_WRITE_REC_GRP          0x12
#define  PBMI_WRITE_REC_EMAIL        0x13
#define  PBMI_WRITE_REC_HIDDEN       0x14
#define  PBMI_WRITE_REC_RSP_INFO     0x10

#define  PBMI_WRITE_REC_EXT_NUMBER       0x10
#define  PBMI_WRITE_REC_EXT_NAME         0x11
#define  PBMI_WRITE_REC_EXT_SNAME        0x12
#define  PBMI_WRITE_REC_EXT_AD_NUM       0x13
#define  PBMI_WRITE_REC_EXT_GRP          0x14
#define  PBMI_WRITE_REC_EXT_EMAIL        0x15
#define  PBMI_WRITE_REC_EXT_HIDDEN       0x16


#define  PBMI_DELETE_REC_ID          0x10

#define  PBMI_SEARCH_REC_NUM_INFO    0x10
#define  PBMI_SEARCH_REC_NAME_INFO   0x11
#define  PBMI_SEARCH_RECS_RESP       0x10

#define  PBMI_HARDCODED_EMER_NUM     		0x01
#define  PBMI_NV_EMER_NUM            		0x10
#define  PBMI_CARD_EMER_NUM          		0x11
#define  PBMI_OTA_EMER_NUM                      0x12
#define  PBMI_HARDCODED_EMER_NUM_EXT            0x13
#define  PBMI_NV_EMER_NUM_EXT                   0x14
#define  PBMI_HARDCODED_EMER_NUM_EXT_CAT 	0x15
#define  PBMI_NV_EMER_NUM_EXT_CAT        	0x16

#define  PBMI_GET_EMER_HCODED           0x10
#define  PBMI_GET_EMER_NV               0x11
#define  PBMI_GET_EMER_CARD             0x12
#define  PBMI_GET_EMER_OTA              0x13
#define  PBMI_GET_EMER_HCODED_EXT       0x14
#define  PBMI_GET_EMER_NV_EXT           0x15
#define  PBMI_GET_EMER_HCODED_EXT_CAT	0x16
#define  PBMI_GET_EMER_NV_EXT_CAT       0x17


#define  PBMI_GET_PB_STATE           0x10

#define  PBMI_GET_ALL_GRP_DATA       0x10

#define  PBMI_SET_GRP_INFO           0x10

#define  PBMI_NEXT_EMPTY_REC_ID      0x10

#define  PBMI_NEXT_NON_EMPTY_REC_ID  0x10

#define PBMI_GET_ALL_AAS_DATA        0x10

#define  PBMI_SET_AAS_INFO           0x10

#define  PBMI_GET_SUBSCRIPTION_BINDING 0x10

#define  PBMI_AAS_ALPHA_EXT         0x10

#define  PBMI_GAS_ALPHA_EXT         0x10

/*---------------------------------------------------------------------------
  Response TLVs
---------------------------------------------------------------------------*/
#define  PBMI_TLV_TAG_SIZE  1
#define  PBMI_TLV_LEN_SIZE  2




/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

#define QMI_PBMI_PULL(sdu_in, value, siz) (siz == dsm_pullup( sdu_in, value, siz ))


#define QMI_MMGSDI_SLOT_TO_QMI_SLOT(x) (((x)==MMGSDI_SLOT_1)?QMI_PBM_SLOT_1:(((x)== MMGSDI_SLOT_2)? QMI_PBM_SLOT_2 : QMI_PBM_SLOT_3 ))

/*===========================================================================

                               DATA TYPES

===========================================================================*/


/*---------------------------------------------------------------------------
  PBM Command enum type - not equal to the actual command values!
  mapping is in qmi_pbm_cmd_callbacks table

  DO NOT REORDER THIS ENUM!
---------------------------------------------------------------------------*/
typedef enum
{
  PBMI_CMD_MIN                           = 0,
  PBMI_CMD_INDICATION_REGISTER           = PBMI_CMD_MIN,
  PBMI_CMD_GET_PB_CAPABILITIES           ,
  PBMI_CMD_GET_ALL_PB_CAPABILITIES       ,
  PBMI_CMD_READ_RECORDS                  ,
  PBMI_CMD_WRITE_RECORD                  ,
  PBMI_CMD_DELETE_RECORD                 ,
  PBMI_CMD_DELETE_ALL_PB_RECORDS         ,
  PBMI_CMD_SEARCH_RECORDS                ,
  PBMI_CMD_GET_EMERGENCY_LIST            ,
  PBMI_CMD_GET_ALL_GROUPS                ,
  PBMI_CMD_SET_GROUP_INFO                ,
  PBMI_CMD_GET_PB_STATE                  ,
  PBMI_CMD_READ_ALL_HIDDEN_RECORDS       ,
  PBMI_CMD_GET_NEXT_EMPTY_RECORD_ID      ,
  PBMI_CMD_GET_NEXT_NON_EMPTY_RECORD_ID  ,
  PBMI_CMD_GET_ALL_AAS                   ,
  PBMI_CMD_SET_AAS                       ,
  PBMI_CMD_BIND_SUBSCRIPTION             ,
  PBMI_CMD_GET_SUBSCRIPTION_BINDING      ,
  PBMI_CMD_READ_PBSET_CAPS               ,
  PBMI_CMD_READ_RECORDS_EXT,
  PBMI_CMD_WRITE_RECORD_EXT,
  PBMI_CMD_SEARCH_RECORDS_EXT,
  PBMI_CMD_READ_ALL_HIDDEN_RECORDS_EXT,
  PBMI_CMD_READ_RECORDS_EXT_UNDECODED,
  PBMI_CMD_MAX,
  PBMI_CMD_WIDTH                    = 0xFFFF
} qmi_pbmi_cmd_e_type;


extern qmi_pbm_info_type pbm_if_info;

/*---------------------------------------------------------------------------
  QMI PBM instance state definition &
  PBM client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_pbmi_client_state_s
{
  qmi_common_client_state_type common; // must be first since we alias
  int16 instance;
  int16 service_id;
  struct
  {
    boolean                      reg_rec_update_events;
    boolean                      reg_emergency_list_events;
    boolean                      reg_pb_ready_events;
    boolean                      reg_hidden_status_events;
    boolean                      reg_aas_update_events;
    boolean                      reg_gas_update_events;
  }pbm_info;
  struct
  {
    boolean                      is_bind_subs_recv;
    uint8                        subs_id;
  }pbm_subs_info;
} qmi_pbmi_client_state_type;

/*---------------------------------------------------------------------------
  Watermark queue to hold deferred responses
---------------------------------------------------------------------------*/
typedef qmi_pbmi_client_state_type* qmi_pbmi_client_state_ptr_type;

typedef struct
{
  uint16                        num_qmi_instances;
  qmi_pbmi_client_state_ptr_type    client[PBMI_MAX_CLIDS];
} qmi_pbmi_state_type;

/*--------------------------------------------------------------------------
  Data to detect the client for sending RECORD_READ_IND
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 sess_type;
  uint16 pb_type;
  uint8 cl_id;
} qmi_pbmi_record_read_client_type;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

static dsm_item_type* qmi_pbmi_indication_register(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_pb_capabilities(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_all_pb_capabilities(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_read_records(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_write_record(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_write_record_ext(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_delete_record(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_search_records(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_delete_all_pb_records(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_emergency_list(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_pb_state(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_all_groups(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_set_group_info(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_read_all_hidden_records(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_next_empty_record_id(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_next_non_empty_record_id(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_all_aas(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_set_aas(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_bind_subscription(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_get_subscription_binding(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_read_pbset_caps(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_pbmi_read_records_ext_undecoded(void*, void*, void*, dsm_item_type **);

static qmi_error_e_type qmi_pbm_create_and_send_cmd(qmi_pbm_cmd_enum_type cmd,
                             qmi_pbm_if_info_item_s* pbm_if_data_ptr);


static dsm_item_type * qmi_pbmi_get_pb_capabilities
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
);

static void qmi_pbm_fw_req_cback
(
  qmi_framework_msg_hdr_type *msg_hdr,
  dsm_item_type **sdu
);

static qmi_pbmi_client_state_type * qmi_pbmi_alloc_cl_sp
(
  uint8 clid
);
static boolean qmi_pbmi_dealloc_cl_sp
(
  uint8 clid
);

/*===========================================================================
  FUNCTION QMI_PBMI_SET_GROUP_OR_AAS_INFO()

  DESCRIPTION
    Used to set the AAS or GAS based on the input cmd type.

  PARAMETERS
    sp               : service provided state pointer
    cmd_buf_p        : ptr to cmd buffer
    cl_sp            : client state pointer
    sdu_in           : incoming request
    cmd              : Input command

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_set_group_or_aas_info
(
  void *                    sp,
  void *                    cmd_buf_p,
  void *                    cl_sp,
  dsm_item_type **          sdu_in,
  qmi_pbm_if_cmd_e_type     cmd
);

#define PBM_HDLR(a,b)  QMI_SVC_HDLR( a, (qmi_svc_hdlr_ftype)b )

static qmi_svc_cmd_hdlr_type  qmi_pbmi_cmd_callbacks[PBMI_CMD_MAX] =
{
  PBM_HDLR( PBMI_CMD_VAL_INDICATION_REGISTER,
                qmi_pbmi_indication_register),
  PBM_HDLR( PBMI_CMD_VAL_GET_PB_CAPABILITIES,
                qmi_pbmi_get_pb_capabilities),
  PBM_HDLR( PBMI_CMD_VAL_GET_ALL_PB_CAPABILITIES,
                qmi_pbmi_get_all_pb_capabilities),
  PBM_HDLR( PBMI_CMD_VAL_READ_RECORDS,
                qmi_pbmi_read_records),
  PBM_HDLR( PBMI_CMD_VAL_READ_RECORDS_EXT,
                qmi_pbmi_read_records),
  PBM_HDLR( PBMI_CMD_VAL_WRITE_RECORD,
                qmi_pbmi_write_record),
  PBM_HDLR( PBMI_CMD_VAL_WRITE_RECORD_EXT,
                qmi_pbmi_write_record_ext),
  PBM_HDLR( PBMI_CMD_VAL_DELETE_RECORD,
                qmi_pbmi_delete_record),
  PBM_HDLR( PBMI_CMD_VAL_DELETE_ALL_PB_RECORDS,
                qmi_pbmi_delete_all_pb_records),
  PBM_HDLR( PBMI_CMD_VAL_SEARCH_RECORDS,
                qmi_pbmi_search_records),
  PBM_HDLR( PBMI_CMD_VAL_SEARCH_RECORDS_EXT,
                qmi_pbmi_search_records),
  PBM_HDLR( PBMI_CMD_VAL_GET_EMERGENCY_LIST,
                qmi_pbmi_get_emergency_list),
  PBM_HDLR( PBMI_CMD_VAL_GET_ALL_GROUPS,
                qmi_pbmi_get_all_groups),
  PBM_HDLR( PBMI_CMD_VAL_SET_GROUP_INFO,
                qmi_pbmi_set_group_info),
  PBM_HDLR( PBMI_CMD_VAL_GET_PB_STATE,
                qmi_pbmi_get_pb_state),
  PBM_HDLR( PBMI_CMD_VAL_READ_ALL_HIDDEN_RECORDS,
                qmi_pbmi_read_all_hidden_records),
  PBM_HDLR( PBMI_CMD_VAL_READ_ALL_HIDDEN_RECORDS_EXT,
                qmi_pbmi_read_all_hidden_records),
  PBM_HDLR( PBMI_CMD_VAL_GET_NEXT_EMPTY_RECORD_ID,
                  qmi_pbmi_get_next_empty_record_id),
  PBM_HDLR( PBMI_CMD_VAL_GET_NEXT_NON_EMPTY_RECORD_ID,
                  qmi_pbmi_get_next_non_empty_record_id),
  PBM_HDLR( PBMI_CMD_VAL_GET_ALL_AAS,
                  qmi_pbmi_get_all_aas),
  PBM_HDLR( PBMI_CMD_VAL_SET_AAS,
                  qmi_pbmi_set_aas),
  PBM_HDLR( PBMI_CMD_VAL_BIND_SUBSCRIPTION,
                qmi_pbmi_bind_subscription),
  PBM_HDLR( PBMI_CMD_VAL_GET_SUBSCRIPTION_BINDING,
                qmi_pbmi_get_subscription_binding),
  PBM_HDLR( PBMI_CMD_VAL_READ_PBSET_CAPS,
                qmi_pbmi_read_pbset_caps),
  PBM_HDLR( PBMI_CMD_VAL_READ_RECORDS_EXT_UNDECODED,
                qmi_pbmi_read_records_ext_undecoded)
};

static qmi_pbmi_state_type  qmi_pbm_state;

/*---------------------------------------------------------------------------
  QMI instance configurations
---------------------------------------------------------------------------*/
static void     qmi_pbmi_reset_client(void *);

static qmi_pbm_svc_config_type  qmi_pbmi_cfg;
/* Client id to send record read indications */

qmi_pbmi_record_read_client_type rec_read_clnt[PBMI_MAX_CLIDS];


/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/


static void qmi_pbmi_get_pb_capabilities_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_get_all_pb_capabilities_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_read_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_read_records_ext_undecoded_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_read_pbset_caps_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_pbset_caps_read_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_write_record_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_delete_record_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_delete_all_pb_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_search_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_get_emergency_list_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_get_pb_state_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_record_update_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_refresh_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_pb_ready_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_sim_ready_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_emergency_list_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);


static void qmi_pbmi_all_pb_init_done_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_record_read_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_record_read_ext_undecoded_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_hidden_record_status_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_get_all_groups_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_set_group_info_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_read_all_hidden_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_get_next_empty_record_id_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_get_next_non_empty_record_id_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_get_all_aas_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_set_aas_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_aas_update_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_gas_update_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
);

static void qmi_pbmi_add_ecc
(
  const char *ecc,
  uint8 len,
  char *buf,
  uint16 dst_buf_max_len
);

static boolean qmi_pbmi_match_subs_session
(
  uint8 subs_id,
  uint8 sess_type
);

/*===========================================================================
  FUNCTION QMI_PBMI_GET_CLIENT_SP_BY_CMD_BUF()

  DESCRIPTION
    Retrieve client sp from the command buffer

  PARAMETERS
    cmd_buf_p_in  :  command buffer

  RETURN VALUE
    cl_sp   :  client sp

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void * qmi_pbmi_get_client_sp_by_cmd_buf
(
  void * cmd_buf_p_in
)
{
  qmi_cmd_buf_type * cmd_buf_p;
  qmi_common_client_state_type *cl_sp;
  qmi_pbmi_state_type *         pbm_sp;
  int i=0;

  if(NULL == cmd_buf_p_in)
  {
    return NULL;
  }
  cmd_buf_p = (qmi_cmd_buf_type *)cmd_buf_p_in;
  if((NULL == cmd_buf_p->x_p) || (NULL == cmd_buf_p->x_p->cl_sp))
  {
    return NULL;
  }
  cl_sp = cmd_buf_p->x_p->cl_sp;
  pbm_sp = &qmi_pbm_state;
  for(i=0; i < PBMI_MAX_CLIDS; i++ )
  {
    if(pbm_sp->client[i] != NULL)
    {
      if( pbm_sp->client[i]->common.clid == cl_sp->clid)
      {
        return pbm_sp->client[i];
      }
    }
  }
  return NULL;
} /* qmi_pbmi_get_client_sp_by_cmd_buf() */


/*===========================================================================
  FUNCTION QMI_PBM_HANDLE_FW_CMD()

  DESCRIPTION
    Process QMI-PBM cmds posted from Framework callbacks

  PARAMETERS
    cmd: qmi_pbm_cmd_enum_type
    user_data_ptr: void ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_handle_fw_cmd
(
  void *            user_data_ptr
)
{
  qmi_pbm_cmd_data_buf_type* data_ptr;
  qmi_pbm_common_cmd_type *qmi_info;

  ASSERT (user_data_ptr);

  data_ptr = (qmi_pbm_cmd_data_buf_type *) user_data_ptr;
  ASSERT(data_ptr->cmd.qmi_ptr);
  ASSERT (data_ptr->cmd.qmi_ptr->id < QMI_PBM_IF_CMD_MAX);
  qmi_info = data_ptr->cmd.qmi_ptr;

  switch(qmi_info->id)
  {
    case QMI_PBM_FW_INIT_CB:
      qmi_pbmi_fw_init_cback_hdlr(qmi_info->data.qmi_fw_info.init_cb.num_instances);
      break;
    case QMI_PBM_FW_ALLOC_CLID_CB:
      qmi_pbmi_fw_alloc_clid_hdlr(&qmi_info->data.qmi_fw_info.alloc_clid.msg_hdr);
      break;
    case QMI_PBM_FW_DEALLOC_CLID_CB:
      qmi_pbmi_fw_dealloc_clid_hdlr(&qmi_info->data.qmi_fw_info.dealloc_clid.msg_hdr);
      break;
    case QMI_PBM_FW_REQ_CB:
      qmi_pbmi_fw_req_hdlr(&qmi_info->data.qmi_fw_info.req_cb.msg_hdr,
                               qmi_info->data.qmi_fw_info.req_cb.sdu_in);
      break;
    default:
      UIM_MSG_ERR_0("Unsupported qmi-pbm fw cmd");
      break;
  }

  QMI_PBM_IF_MEM_FREE(qmi_info);

}/* qmi_pbm_handle_fw_cmd */


/*===========================================================================
  FUNCTION QMI_PBM_FW_INIT_CBACK()

  DESCRIPTION
    Callback for Service initialization from Framework

  PARAMETERS
    num_instances: uint16

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_fw_init_cback
(
  uint16 num_instances
)
{
  qmi_pbm_cmd_data_buf_type *cmd_ptr;

  if((cmd_ptr = qmi_pbm_get_cmd_data_buf()) == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_pbm_cmd_data_buf_type));
  cmd_ptr->cmd.qmi_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_common_cmd_type));

  if( NULL == cmd_ptr->cmd.qmi_ptr )
  {
    qmi_pbm_free_cmd_data_buf(cmd_ptr);
    return;
  }

  cmd_ptr->cmd.qmi_ptr->id = QMI_PBM_FW_INIT_CB;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.init_cb.num_instances = num_instances;

  qmi_pbm_send_cmd(QMI_PBM_CMD_PBM_FW_CB, cmd_ptr);
} /* qmi_pbm_fw_init_cback */


/*===========================================================================
  FUNCTION QMI_PBMI_FW_INIT_CBACK_HDLR()

  DESCRIPTION
    Process the Service initialization request from Framework

  PARAMETERS
    num_instances: uint16

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_init_cback_hdlr
(
  uint16 num_instances
)
{
  qmi_pbmi_state_type *pbm_sp;

  pbm_sp = &qmi_pbm_state;

  memset(pbm_sp, 0, sizeof(qmi_pbmi_state_type));

  pbm_sp->num_qmi_instances = num_instances;

  qmi_pbmi_cfg.svc_sp = pbm_sp;
}/* qmi_pbmi_fw_init_cback_hdlr */


/*===========================================================================
  FUNCTION QMI_PBM_ALLOC_CLID_CBACK()

  DESCRIPTION
    Callback to allocate the client from Framework

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_pbm_fw_alloc_clid_cback
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_pbm_cmd_data_buf_type *cmd_ptr;

  ASSERT(common_msg_hdr);

  if( (cmd_ptr = qmi_pbm_get_cmd_data_buf()) == NULL)
  {
    return FALSE;
  }
  memset(cmd_ptr, 0, sizeof(qmi_pbm_cmd_data_buf_type));
  cmd_ptr->cmd.qmi_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_common_cmd_type));
  if(NULL == cmd_ptr->cmd.qmi_ptr)
  {
    qmi_pbm_free_cmd_data_buf(cmd_ptr);
    return FALSE;
  }

  cmd_ptr->cmd.qmi_ptr->id = QMI_PBM_FW_ALLOC_CLID_CB;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.alloc_clid.msg_hdr = *common_msg_hdr;

  qmi_pbm_send_cmd(QMI_PBM_CMD_PBM_FW_CB, cmd_ptr);
  return TRUE;
}/* qmi_pbm_fw_alloc_clid_cback */


/*===========================================================================
  FUNCTION QMI_PBM_ALLOC_CLID_HDLR()

  DESCRIPTION
    Process the client allocation request from Framework

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_alloc_clid_hdlr
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_pbmi_client_state_type    *cl_sp  = NULL;
  qmi_result_e_type              result = QMI_RESULT_FAILURE;
  qmi_pbmi_state_type           *pbm_sp = NULL;

  ASSERT(common_msg_hdr);

  pbm_sp = &qmi_pbm_state;
  cl_sp = qmi_pbmi_alloc_cl_sp(common_msg_hdr->client_id);

  if(cl_sp)
  {
    memset( &cl_sp->common , 0, sizeof(qmi_common_client_state_type) );
    cl_sp->common.clid  = QMI_SVC_CLID_UNUSED;
    ds_qmi_fw_common_cl_init(pbm_sp , &cl_sp->common);

    cl_sp->pbm_info.reg_rec_update_events      = FALSE;
    cl_sp->pbm_info.reg_pb_ready_events        = FALSE;
    cl_sp->pbm_info.reg_emergency_list_events  = FALSE;
    cl_sp->pbm_info.reg_hidden_status_events   = FALSE;
    cl_sp->pbm_info.reg_aas_update_events      = FALSE;
    cl_sp->pbm_info.reg_gas_update_events      = FALSE;
    cl_sp->pbm_subs_info.subs_id               = PBMI_INVALID_SUBS;
    cl_sp->pbm_subs_info.is_bind_subs_recv     = FALSE;
    cl_sp->common.clid                         = common_msg_hdr->client_id;
    cl_sp->instance                            = (uint16)common_msg_hdr->qmi_instance;
    cl_sp->service_id                          = common_msg_hdr->service ;

    UIM_MSG_HIGH_1("qmi_pbmi clid set to local client state ptr: clid 0x%x",
                   cl_sp->common.clid);
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    UIM_MSG_HIGH_0("No available service clids!");
  }

  /* Notifying the clid allocation back to client via QMI Framework*/
  qmi_framework_svc_send_alloc_clid_result(result, common_msg_hdr);
}/* qmi_pbmi_fw_alloc_clid_hdlr */


/*===========================================================================
  FUNCTION QMI_PBM_FW_DEALLOC_CLID_CBACK()

  DESCRIPTION
    Deallocate the client data.

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_pbm_fw_dealloc_clid_cback
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_pbm_cmd_data_buf_type *cmd_ptr;

  ASSERT(common_msg_hdr);

  if((cmd_ptr = qmi_pbm_get_cmd_data_buf()) == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_pbm_cmd_data_buf_type));

  cmd_ptr->cmd.qmi_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_common_cmd_type));
  if( NULL == cmd_ptr->cmd.qmi_ptr )
  {
    qmi_pbm_free_cmd_data_buf(cmd_ptr);
    return;
  }

  cmd_ptr->cmd.qmi_ptr->id = QMI_PBM_FW_DEALLOC_CLID_CB;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.dealloc_clid.msg_hdr = *common_msg_hdr;

  qmi_pbm_send_cmd(QMI_PBM_CMD_PBM_FW_CB, cmd_ptr);
}/* qmi_pbm_fw_dealloc_clid_cback */


/*===========================================================================
  FUNCTION QMI_PBM_FW_DEALLOC_CLID_HDLR()

  DESCRIPTION
    Process the dealloc clid request from framework.

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_dealloc_clid_hdlr
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_pbmi_client_state_type       *cl_sp  = NULL;
  qmi_result_e_type                 result = QMI_RESULT_FAILURE;

  ASSERT(common_msg_hdr);

  cl_sp = (qmi_pbmi_client_state_type *)
           qmi_pbm_state.client[common_msg_hdr->client_id - 1];

  if(cl_sp != NULL && cl_sp->common.clid == common_msg_hdr->client_id)
  {
    qmi_pbmi_reset_client(cl_sp);
    qmi_pbmi_dealloc_cl_sp(common_msg_hdr->client_id);
    result = QMI_RESULT_SUCCESS;
  }

  UIM_MSG_HIGH_2("qmi_pbmi_fw_dealloc_clid_hdlr for clid:0x%x result: 0x%x",
                  common_msg_hdr->client_id, result);

  /* Notifying the clid deallocation back to client via QMI Framework*/
  qmi_framework_svc_send_dealloc_clid_result(result, common_msg_hdr);
}/* qmi_pbmi_fw_dealloc_clid_hdlr */


/*===========================================================================
  FUNCTION QMI_PBM_FW_REQ_CB()

  DESCRIPTION
    Callback for qmi request from Framework

  PARAMETERS
    msg_hdr: qmi_framework_msg_hdr_type ptr
    sdu: dsm_item_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbm_fw_req_cback
(
  qmi_framework_msg_hdr_type   *msg_hdr,
  dsm_item_type               **sdu
)
{
  qmi_pbm_cmd_data_buf_type *cmd_ptr;

  ASSERT(msg_hdr);

  if((cmd_ptr = qmi_pbm_get_cmd_data_buf()) == NULL)
  {
    return;
  }
  memset(cmd_ptr, 0, sizeof(qmi_pbm_cmd_data_buf_type));
  cmd_ptr->cmd.qmi_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_common_cmd_type));
  if(NULL == cmd_ptr->cmd.qmi_ptr)
  {
    qmi_pbm_free_cmd_data_buf(cmd_ptr);
    return;
  }

  cmd_ptr->cmd.qmi_ptr->id = QMI_PBM_FW_REQ_CB;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.req_cb.msg_hdr = *msg_hdr;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.req_cb.sdu_in  = *sdu;

  qmi_pbm_send_cmd(QMI_PBM_CMD_PBM_FW_CB, cmd_ptr);
}/* qmi_pbm_fw_req_cb */


/*===========================================================================
  FUNCTION QMI_PBM_FW_REQ_HDLR()

  DESCRIPTION
    Process qmi request from Framework

  PARAMETERS
    msg_hdr: qmi_framework_msg_hdr_type ptr
    sdu: dsm_item_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_req_hdlr
(
  qmi_framework_msg_hdr_type      *msg_hdr,
  dsm_item_type                   *sdu_in
)
{
  qmi_pbmi_client_state_type    *cl_sp = NULL;

  ASSERT(msg_hdr);
  ASSERT(sdu_in);
  ASSERT(msg_hdr->common_hdr.client_id != QMI_FRAMEWORK_SVC_MAX_CLIENTS);

  UIM_MSG_HIGH_3("Process QMI PBM svc Request handlr clid 0x%x tx_id 0x%x ctl_flag 0x%x",
                  msg_hdr->common_hdr.client_id,
                  msg_hdr->common_hdr.transaction_id,
                  msg_hdr->msg_ctl_flag);

  /*-------------------------------------------------------------------------
    Get a service state and client state pointer
  -------------------------------------------------------------------------*/
  cl_sp = (qmi_pbmi_client_state_type *)
           qmi_pbm_state.client[msg_hdr->common_hdr.client_id - 1];

  ASSERT(cl_sp);

  /*-------------------------------------------------------------------------
    Invoke the common svc request handler
  -------------------------------------------------------------------------*/
  ds_qmi_fw_recv(&qmi_pbmi_cfg.fw_svc_cfg,
                 qmi_pbmi_cfg.svc_sp,
                 msg_hdr,
                 &cl_sp->common,
                 sdu_in);
}/* qmi_pbmi_fw_req_hdlr */


/*===========================================================================
  FUNCTION QMI_PBMI_SEND_INDICATION()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the PBM service
    indication to client.

  PARAMETERS
    msg_hdr_p : QMI Framework message header
    cmd_type : type of indication
    ind : dsm item

  RETURN VALUE
    TRUE if ind sent to Framework sucessfully
    FALSE otherwise

  DEPENDENCIES
    QMI PBM service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_pbmi_send_indication
(
  qmi_pbmi_client_state_type    *cl_sp,
  uint16                         cmd_type,
  dsm_item_type *                ind
)
{
  qmi_framework_common_msg_hdr_type   common_hdr;
  boolean                             status = TRUE;

  ASSERT(cl_sp);

  /*-----------------------------------------------------------------------
    Fill the QMI Framework message common header
  -----------------------------------------------------------------------*/
  common_hdr.client_id =  cl_sp->common.clid;
  UIM_MSG_HIGH_2("QMI PBM indication(0x%x) being sent to client 0x%x",
                  cmd_type, common_hdr.client_id);
  common_hdr.qmi_instance = cl_sp->instance;
  common_hdr.service = (qmux_service_e_type)cl_sp->service_id;

  status = ds_qmi_fw_send_ind(&common_hdr, cmd_type, ind);
  if(!status && (ind != NULL))
  {
    /* If the msg_ptr is not freed in failure case of send response - free it here now */
    dsm_free_packet(&ind);
  }
  return status;
} /* qmi_pbmi_send_indication */


/*===========================================================================
  FUNCTION QMI_PBMI_ALLOC_CL_SP()

  DESCRIPTION
    Allocate the client data buffer

  PARAMETERS
    clid: uint8

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_pbmi_client_state_type * qmi_pbmi_alloc_cl_sp
(
  uint8 clid
)
{
  qmi_pbmi_client_state_type *  cl_sp = NULL;

  if((clid == QMI_SVC_CLID_UNUSED) ||
     (clid >= QMI_FRAMEWORK_SVC_MAX_CLIENTS))
  {
    return cl_sp;
  }
  cl_sp = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbmi_client_state_type));
  if(cl_sp == NULL)
  {
    ASSERT(cl_sp);
  }
  qmi_pbm_state.client[clid - 1] = cl_sp;
  return cl_sp;
} /* qmi_pbmi_alloc_cl_sp */


/*===========================================================================
  FUNCTION QMI_PBM_FW_DEALLOC_CL_SP()

  DESCRIPTION
    Free the client data buffer for the given clid.

  PARAMETERS
    clid: uint8 type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_pbmi_dealloc_cl_sp
(
  uint8 clid
)
{
  qmi_pbmi_client_state_type *  cl_sp = NULL;

  if ((clid == QMI_SVC_CLID_UNUSED ) ||
      (clid >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ))
  {
    return FALSE;
  }

  cl_sp = (qmi_pbmi_client_state_type *) qmi_pbm_state.client[clid - 1];
  if(cl_sp)
  {
    q_destroy( &cl_sp->common.x_free_q );
    q_destroy( &cl_sp->common.cmd_free_q );
    QMI_PBM_IF_MEM_FREE(cl_sp);
    qmi_pbm_state.client[clid - 1] = NULL;
  }
  else
  {
    return FALSE;
  }

  return TRUE;
} /* qmi_pbmi_dealloc_cl_sp */


/*===========================================================================
  FUNCTION QMI_PBM_INIT()

  DESCRIPTION
    Register the PBM service with QMUX for all applicable QMI links

  PARAMETERS
    QMI Instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_init(void)
{
  qmi_framework_err_e_type      errval = QMI_FRAMEWORK_ERR_NONE;
  uint8                         i      = 0;
  qmi_idl_service_object_type   svc_obj;

  UIM_MSG_HIGH_0 ("QMI PBM qmi_pbm_init");

  svc_obj = pbm_get_service_object_v01();
  (void) qmi_si_register_object(svc_obj,
                                0, /* Service Instance */
                                pbm_get_service_impl_v01());

  /* Set the cmd handlers in QMI PBM task */
  qmi_pbm_set_cmd_handler(QMI_PBM_CMD_PBM_FW_CB, qmi_pbm_handle_fw_cmd);

  qmi_pbm_if_init();

  /* memset the service configuration */
  memset(&qmi_pbmi_cfg.fw_svc_cfg.fw_cfg, 0, sizeof(qmi_framework_svc_config_type));

  /*-----------------------------------------------------------------------
  Step1: Fill configuration
  -----------------------------------------------------------------------*/
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.base_version.major     = PBMI_BASE_VER_MAJOR;
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.base_version.minor     = PBMI_BASE_VER_MINOR;
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.addendum_version.major = PBMI_ADDENDUM_VER_MAJOR;
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.addendum_version.minor = PBMI_ADDENDUM_VER_MINOR;

  /* mandatory callbacks should be filled */
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.cbs.alloc_clid         = qmi_pbm_fw_alloc_clid_cback;
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.cbs.dealloc_clid       = qmi_pbm_fw_dealloc_clid_cback;
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.cbs.init_cback         = qmi_pbm_fw_init_cback;
  qmi_pbmi_cfg.fw_svc_cfg.fw_cfg.cbs.cmd_hdlr           = qmi_pbm_fw_req_cback;

  qmi_pbmi_cfg.fw_svc_cfg.cmd_hdlr_array    = qmi_pbmi_cmd_callbacks;
  qmi_pbmi_cfg.fw_svc_cfg.cmd_num_entries   = PBMI_CMD_MAX;
  qmi_pbmi_cfg.service_id = QMUX_SERVICE_PBM;

    /* Initialize client id data to which record read indications have to be sent */
    memset(rec_read_clnt, 0, sizeof(rec_read_clnt));
    for(i=0;i<PBMI_MAX_CLIDS;i++)
    {
      rec_read_clnt[i].sess_type = QMI_PBM_IF_SESSION_MAX;
    }

  /*-----------------------------------------------------------------------
  step 2:  calling QMI Framework API to register the service.
  ----------------------------------------------------------------------*/
  errval = qmi_framework_reg_service(QMUX_SERVICE_PBM, &qmi_pbmi_cfg.fw_svc_cfg.fw_cfg);
  if (QMI_FRAMEWORK_ERR_NONE != errval)
  {
    UIM_MSG_ERR_2("Service registration failed Service:%d error:%d",
                   QMUX_SERVICE_PBM,errval);
    if (QMI_FRAMEWORK_ERR_FW_NOT_READY == errval)
    {
     /* Start timer and set signal to pbm task and try to do the registration again ?
       Have a max retry count and exit ? TBD */
    }
    /*-----------------------------------------------------------------------
    step 3:  Service registration fails. See error code.
    -----------------------------------------------------------------------*/
    // Registration may have failied due to any one of the following reasons
    // 1. Service already registered
    // 2. Mandatory callback is missing
    // 3. Framework is NOT ready
    // If Framework is not ready, then service needs to retry registration later.
    // Handling different failures is left to the discretion of the QMI service owner
  }
} /* qmi_pbm_init() */

/*===========================================================================
  FUNCTION qmi_pbm_encode_and_send_msg()

  DESCRIPTION
    This is to pack the respective message response and sent that to fw.
    Each request-response will have a different packing as response structure
    is different.

  PARAMETERS
    The data pointer for the message related data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_pbm_encode_and_send_msg
(
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr
)
{
  ASSERT(pbm_if_data_ptr);

  UIM_MSG_HIGH_1 ("In qmi_pbm_encode_and_send_msg: cmd 0x%x", pbm_if_data_ptr->header.cmd);
  ASSERT (pbm_if_data_ptr->header.cmd < QMI_PBM_IF_CMD_MAX);

  switch (pbm_if_data_ptr->header.cmd)
  {
    case QMI_PBM_IF_CMD_GET_PB_CAPABILITIES_RESP:
      qmi_pbmi_get_pb_capabilities_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES_RESP:
      qmi_pbmi_get_all_pb_capabilities_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_READ_RECORDS_RESP:
      qmi_pbmi_read_records_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED_RESP :
      qmi_pbmi_read_records_ext_undecoded_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_READ_PBSET_CAPS_RESP:
      qmi_pbmi_read_pbset_caps_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_WRITE_RECORD_RESP:
      qmi_pbmi_write_record_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_DELETE_RECORD_RESP:
      qmi_pbmi_delete_record_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS_RESP:
      qmi_pbmi_delete_all_pb_records_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_SEARCH_RECORDS_RESP:
      qmi_pbmi_search_records_resp(pbm_if_data_ptr);
      if(pbm_if_data_ptr->data.search_recs_resp != NULL)
      {
        QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr->data.search_recs_resp);
      }
      break;
    case QMI_PBM_IF_CMD_RECORD_UPDATE_IND:
      qmi_pbmi_record_update_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_REFRESH_IND:
      qmi_pbmi_refresh_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_PB_READY_IND:
      qmi_pbmi_pb_ready_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_SIM_READY_IND:
      qmi_pbmi_sim_ready_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_EMERGENCY_LIST_IND:
      qmi_pbmi_emergency_list_ind(pbm_if_data_ptr);
      if(pbm_if_data_ptr->data.emerg_list_ind.emer_data != NULL)
      {
        QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr->data.emerg_list_ind.emer_data);
      }
      break;
    case QMI_PBM_IF_CMD_ALL_PB_INIT_DONE_IND:
      qmi_pbmi_all_pb_init_done_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_RECORD_READ_IND:
    case QMI_PBM_IF_CMD_RECORD_READ_EXT_IND:
      qmi_pbmi_record_read_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_INTERNAL_READ_RECORDS_EXT_UNDECODED_IND :
      qmi_pbmi_record_read_ext_undecoded_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_PBSET_CAPS_READ_IND:
      qmi_pbmi_pbset_caps_read_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_EMERGENCY_LIST_RESP:
      qmi_pbmi_get_emergency_list_resp(pbm_if_data_ptr);
      if(pbm_if_data_ptr->data.emerg_list_resp.emer_data != NULL)
      {
        QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr->data.emerg_list_resp.emer_data);
      }
      break;
    case QMI_PBM_IF_CMD_GET_PB_STATE_RESP:
      qmi_pbmi_get_pb_state_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_ALL_GROUPS_RESP:
      qmi_pbmi_get_all_groups_resp(pbm_if_data_ptr);
      if(pbm_if_data_ptr->data.get_all_grp_resp != NULL)
      {
        QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr->data.get_all_grp_resp);
      }
      break;
    case QMI_PBM_IF_CMD_SET_GROUP_INFO_RESP:
      qmi_pbmi_set_group_info_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_READ_ALL_HIDDEN_RECORDS_RESP:
      qmi_pbmi_read_all_hidden_records_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_HIDDEN_RECORD_STATUS_IND:
      qmi_pbmi_hidden_record_status_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID_RESP:
      qmi_pbmi_get_next_empty_record_id_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID_RESP:
      qmi_pbmi_get_next_non_empty_record_id_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_ALL_AAS_RESP:
      qmi_pbmi_get_all_aas_resp(pbm_if_data_ptr);
      if(pbm_if_data_ptr->data.get_all_aas_resp != NULL)
      {
        QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr->data.get_all_aas_resp);
      }
      break;
    case QMI_PBM_IF_CMD_SET_AAS_RESP:
      qmi_pbmi_set_aas_resp(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_AAS_UPDATE_IND:
      qmi_pbmi_aas_update_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GAS_UPDATE_IND:
      qmi_pbmi_gas_update_ind(pbm_if_data_ptr);
      break;
    default:
      break;
  }
  QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
  return;
} /* qmi_pbm_encode_and_send_msg() */


/*===========================================================================
  FUNCTION QMI_PBMI_INDICATION_REGISTER()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_indication_register
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *              response              = NULL;
  qmi_error_e_type             errval                = QMI_ERR_NONE;
  qmi_result_e_type            result                = QMI_RESULT_SUCCESS;
  boolean                      retval                = TRUE;
  uint32             reg_mask = 0;

#ifdef REMOVE_QMI_PBM_INDICATION_REGISTER_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                        type                  = 0;
  uint16                       len                   = 0;
  uint16                       expected_len          = 0;
  qmi_pbmi_client_state_type * client_sp             = NULL;
  void *                       value                 = NULL;
  boolean                      pb_ready_reg_old      = FALSE;
  boolean                      emer_list_reg_old     = FALSE;
  boolean                      rec_update_reg_new    = FALSE;
  boolean                      pb_ready_reg_new      = FALSE;
  boolean                      emer_list_reg_new     = FALSE;
  boolean                      hidden_status_reg_new = FALSE;
  boolean                      aas_update_reg_new    = FALSE;
  boolean                      gas_update_reg_new    = FALSE;
  boolean                      got_v_in_required     = FALSE;
  boolean                      send_pb_ready_ind     = FALSE;
  boolean                      send_emer_list_ind    = FALSE;
  qmi_pbm_if_info_item_s     * pbm_if_data_ptr       = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  client_sp = (qmi_pbmi_client_state_type *) cl_sp;

  pb_ready_reg_old = client_sp->pbm_info.reg_pb_ready_events;
  emer_list_reg_old = client_sp->pbm_info.reg_emergency_list_events;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(reg_mask);
        value = &reg_mask;
        got_v_in_required = TRUE;
        break;
      default:
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  } /* while */

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if( (reg_mask & QMI_PBM_IF_IND_REG_REC_UPDATE) == QMI_PBM_IF_IND_REG_REC_UPDATE)
  {
    rec_update_reg_new = TRUE;
  }

  if( (reg_mask & QMI_PBM_IF_IND_REG_PB_READY) == QMI_PBM_IF_IND_REG_PB_READY)
  {
    pb_ready_reg_new = TRUE;
  }

  if( (reg_mask & QMI_PBM_IF_IND_REG_EMER_LIST) == QMI_PBM_IF_IND_REG_EMER_LIST)
  {
    emer_list_reg_new = TRUE;
  }

  if( (reg_mask & QMI_PBM_IF_IND_REG_HIDDEN_STATUS) == QMI_PBM_IF_IND_REG_HIDDEN_STATUS)
  {
    hidden_status_reg_new = TRUE;
  }

  if( (reg_mask & QMI_PBM_IF_IND_REG_AAS_UPDATE) == QMI_PBM_IF_IND_REG_AAS_UPDATE)
  {
    aas_update_reg_new = TRUE;
  }

  if( (reg_mask & QMI_PBM_IF_IND_REG_GAS_UPDATE) == QMI_PBM_IF_IND_REG_GAS_UPDATE)
  {
    gas_update_reg_new = TRUE;
  }

  client_sp->pbm_info.reg_rec_update_events = rec_update_reg_new;
  client_sp->pbm_info.reg_pb_ready_events = pb_ready_reg_new;
  client_sp->pbm_info.reg_emergency_list_events = emer_list_reg_new;
  client_sp->pbm_info.reg_hidden_status_events = hidden_status_reg_new;
  client_sp->pbm_info.reg_aas_update_events = aas_update_reg_new;
  client_sp->pbm_info.reg_gas_update_events = gas_update_reg_new;

  /* calculate the reg mask for response */
  reg_mask = 0;
  reg_mask |= rec_update_reg_new ? QMI_PBM_IF_IND_REG_REC_UPDATE : 0;
  reg_mask |= pb_ready_reg_new   ? QMI_PBM_IF_IND_REG_PB_READY : 0;
  reg_mask |= emer_list_reg_new  ? QMI_PBM_IF_IND_REG_EMER_LIST : 0;
  reg_mask |= hidden_status_reg_new ? QMI_PBM_IF_IND_REG_HIDDEN_STATUS : 0;
  reg_mask |= aas_update_reg_new ? QMI_PBM_IF_IND_REG_AAS_UPDATE : 0;
  reg_mask |= gas_update_reg_new ? QMI_PBM_IF_IND_REG_GAS_UPDATE : 0;

  send_pb_ready_ind = (pb_ready_reg_new && !pb_ready_reg_old);
  send_emer_list_ind = (emer_list_reg_new && !emer_list_reg_old);

  if( send_pb_ready_ind || send_emer_list_ind )
  {
    pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
    if( NULL == pbm_if_data_ptr )
    {
      goto send_result;
    }

    memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

    pbm_if_data_ptr->data.ind_reg.send_pb_ready_ind  = send_pb_ready_ind;
    pbm_if_data_ptr->data.ind_reg.send_emer_list_ind = send_emer_list_ind;
    pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_INDICATION_REGISTER;
    pbm_if_data_ptr->header.handle = 0;
    errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  }
#endif /* REMOVE_QMI_PBM_INDICATION_REGISTER_V01 */

send_result:
  if( QMI_ERR_NONE == errval )
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      PBMI_IND_REG_MASK,
                                      sizeof(reg_mask),
                                      (void *) &(reg_mask)))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
}/* qmi_pbmi_indication_register */


/*===========================================================================
  FUNCTION QMI_PBMI_BIND_SUBSCRIPTION()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_bind_subscription
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *              response          = NULL;
  qmi_error_e_type             errval            = QMI_ERR_NONE;
  qmi_result_e_type            result            = QMI_RESULT_SUCCESS;
  boolean                      retval            = TRUE;

#ifdef REMOVE_QMI_PBM_BIND_SUBSCRIPTION_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  qmi_pbmi_client_state_type * client_sp         = NULL;
  uint8                        type              = 0;
  uint16                       len               = 0;
  uint16                       expected_len      = 0;
  boolean                      got_v_in_required = FALSE;
  uint8              subs_type = 0;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  client_sp = (qmi_pbmi_client_state_type *)cl_sp;
  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(subs_type);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !QMI_PBMI_PULL(sdu_in, &subs_type, sizeof(subs_type)) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;

        break;
      default:
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  UIM_MSG_HIGH_1("qmi_pbmi_bind_subscription subs type received %d",subs_type);
  if(subs_type > PBMI_TERITARY_SUBS)
  {
    errval = QMI_ERR_NO_SUBSCRIPTION;
    goto send_result;
  }

  /* store subs type per client */
  client_sp->pbm_subs_info.is_bind_subs_recv = TRUE;
  client_sp->pbm_subs_info.subs_id = subs_type;
#endif /* REMOVE_QMI_PBM_BIND_SUBSCRIPTION_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_bind_subscription() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_SUBSCRIPTION_BINDING()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_subscription_binding
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *              response  = NULL;
  qmi_error_e_type             errval    = QMI_ERR_NONE;
  qmi_result_e_type            result    = QMI_RESULT_SUCCESS;
  boolean                      retval    = TRUE;

#ifdef REMOVE_QMI_PBM_GET_SUBSCRIPTION_BINDING_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  qmi_pbmi_client_state_type * client_sp = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  client_sp = (qmi_pbmi_client_state_type *)cl_sp;

  if(*sdu_in != NULL)
  {
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }


  UIM_MSG_HIGH_1("qmi_pbmi_get_subscription_binding for client %d",client_sp->common.clid);

  if(client_sp->pbm_subs_info.is_bind_subs_recv == TRUE)
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      PBMI_GET_SUBSCRIPTION_BINDING,
                                      sizeof(client_sp->pbm_subs_info.subs_id),
                                      (void *) &(client_sp->pbm_subs_info.subs_id)) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
      goto send_result;
    }
  }
#endif /* REMOVE_QMI_PBM_GET_SUBSCRIPTION_BINDING_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_subscription_binding() */


/*===========================================================================
  FUNCTION QMI_PBMI_SEND_RESPONSE()

  DESCRIPTION
    Send the response to framework

  PARAMETERS
    clid: uint8

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_pbmi_send_response
(
  qmi_error_e_type        errval,
  qmi_cmd_buf_type *      cmd_buf_p,
  dsm_item_type *         msg_ptr,
  boolean                 result_tlv_added
)
{
  qmi_framework_common_msg_hdr_type  common_hdr;
  qmi_pbmi_client_state_type *    cl_sp;
  qmi_result_e_type       result;
  boolean status = TRUE;

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( !result_tlv_added && !qmi_svc_put_result_tlv(&msg_ptr, result, errval))
  {
    UIM_MSG_HIGH_1("Drop send response for 0x%x",cmd_buf_p);
    ds_qmi_fw_free_cmd_buf( &cmd_buf_p );
    dsm_free_packet(&msg_ptr);
    return FALSE;
  }

  cl_sp = (qmi_pbmi_client_state_type *)qmi_pbmi_get_client_sp_by_cmd_buf((void*)cmd_buf_p);
  if(cl_sp == NULL)
  {
    /* Need not free the command transaction information as client is no more valid
       The transactions related to the disconnected client are already taken care
       in qmi_pbmi_reset_client() during de-alloc */
    UIM_MSG_ERR_0("Client no longer valid. Do not send response");
    dsm_free_packet(&msg_ptr);
    return FALSE;
  }
  /* Fill common header and send response */
  memset(&common_hdr, 0, sizeof(common_hdr));
  common_hdr.service = (qmux_service_e_type)cl_sp->service_id;
  common_hdr.client_id = cl_sp->common.clid;
  common_hdr.transaction_id = cmd_buf_p->x_p->x_id;
  common_hdr.qmi_instance = cl_sp->instance;

  /* Send the response */
  status = ds_qmi_fw_send_response( &common_hdr,cmd_buf_p, msg_ptr);
  if(!status && ( msg_ptr != NULL ))
  {
    /* If the msg_ptr is not freed in failure case of send response - free it here now */
    dsm_free_packet(&msg_ptr);
  }
  return status;
}/* qmi_pbmi_send_response */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_PB_CAPABILITIES()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_pb_capabilities
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *           response          = NULL;
  qmi_error_e_type          errval            = QMI_ERR_NONE;
  qmi_result_e_type         result            = QMI_RESULT_SUCCESS;
  boolean                   retval            = TRUE;
  qmi_pbm_if_info_item_s *  pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_GET_PB_CAPABILITIES_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                     type              = 0;
  uint16                    len               = 0;
  uint16                    expected_len      = 0;
  boolean                   got_v_in_required = FALSE;
  uint8                     sess_type         = (uint8)QMI_PBM_IF_SESSION_MAX;
  uint16                    pb_type           = (uint16)PBM_GPB;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));


  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(pbm_if_data_ptr->data.get_pb_caps.session_type) + \
                       sizeof(pbm_if_data_ptr->data.get_pb_caps.pb_type);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &sess_type, sizeof(sess_type)) &&
               QMI_PBMI_PULL(sdu_in, &pb_type, sizeof(pb_type)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        pbm_if_data_ptr->data.get_pb_caps.session_type = sess_type;
        pbm_if_data_ptr->data.get_pb_caps.pb_type = pb_type;
        break;
      default:
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_PB_CAPABILITIES;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_GET_PB_CAPABILITIES_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_pb_capabilities() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_PB_CAPABILITIES_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_pb_capabilities_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  qmi_pbm_if_pb_caps_s    *pb_caps;
  uint8 sess_type = (uint8)QMI_PBM_IF_SESSION_MAX,iter;
  boolean     result_added   =  TRUE;
  pbm_get_pb_capabilities_resp_msg_v01 *resp_ptr=NULL;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_get_pb_capabilities_resp");
    return;
  }

  pb_caps = (qmi_pbm_if_pb_caps_s*)&data_ptr->data.get_pb_caps_resp.pb_caps;

  errval = data_ptr->data.get_pb_caps_resp.error;
  sess_type = data_ptr->data.get_pb_caps_resp.sess_type;

  response = NULL;

  resp_ptr = (pbm_get_pb_capabilities_resp_msg_v01 *) QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_get_pb_capabilities_resp_msg_v01));

  if ( resp_ptr != NULL )
  {
    memset( resp_ptr , 0 , sizeof(pbm_get_pb_capabilities_resp_msg_v01) );
  if( QMI_ERR_NONE == errval )
  {
    resp_ptr->resp.result = QMI_RESULT_SUCCESS_V01 ;
    resp_ptr->resp.error = QMI_ERR_NONE_V01 ;
    if( ((uint16)QMI_PBM_IF_PB_ADN) == pb_caps->pb_type )
    {
      if(pb_caps->pbset_info_valid)
      {
		  resp_ptr->adn_pb_set_info_valid = 0x01 ;
		  resp_ptr->adn_pb_set_info.num_of_pb_sets = pb_caps->pb_set_caps.pb_sets ;
 		  resp_ptr->adn_pb_set_info.aas_caps.max_records  = pb_caps->pb_set_caps.aas_caps.max_records ;
		  resp_ptr->adn_pb_set_info.aas_caps.used_records = pb_caps->pb_set_caps.aas_caps.used_records ;
		  resp_ptr->adn_pb_set_info.aas_caps.max_record_len = pb_caps->pb_set_caps.aas_caps.max_record_len ;

		  resp_ptr->adn_pb_set_info.gas_caps.max_records  = pb_caps->pb_set_caps.gas_caps.max_records ;
		  resp_ptr->adn_pb_set_info.gas_caps.used_records = pb_caps->pb_set_caps.gas_caps.used_records ;
		  resp_ptr->adn_pb_set_info.gas_caps.max_record_len = pb_caps->pb_set_caps.gas_caps.max_record_len ;

		  resp_ptr->adn_pb_set_info.pbset_caps_len = pb_caps->pb_set_caps.pb_set_info_len ;


		  for ( iter = 0 ; iter < QMI_PBM_PB_SET_MAX ; iter ++ )
                  {
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].pb_set_id = pb_caps->pb_set_caps.pb_set_info[iter].pb_set_id ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].adn_caps.used_adn_records = pb_caps->pb_set_caps.pb_set_info[iter].adn_caps.used_adn_records ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].adn_caps.max_adn_records =  pb_caps->pb_set_caps.pb_set_info[iter].adn_caps.max_adn_records ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].adn_caps.max_adn_num_len = pb_caps->pb_set_caps.pb_set_info[iter].adn_caps.max_adn_num_len ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].adn_caps.max_adn_name_len = pb_caps->pb_set_caps.pb_set_info[iter].adn_caps.max_adn_name_len ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].email_caps_len  = pb_caps->pb_set_caps.pb_set_info[iter].email_caps_len ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].sne_caps_len = pb_caps->pb_set_caps.pb_set_info[iter].sne_caps_len ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].grp_caps_len = pb_caps->pb_set_caps.pb_set_info[iter].grp_caps_len ;
		    resp_ptr->adn_pb_set_info.pbset_caps[iter].ad_num_caps_len = pb_caps->pb_set_caps.pb_set_info[iter].ad_num_caps_len ;
		    memscpy(resp_ptr->adn_pb_set_info.pbset_caps[iter].email_caps, sizeof(pbm_usim_capability_ext_type_v01) * QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01 ,
		            pb_caps->pb_set_caps.pb_set_info[iter].email_caps , sizeof(pbm_usim_capability_ext_s_type ) * QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE  );
                    memscpy(resp_ptr->adn_pb_set_info.pbset_caps[iter].ad_num_caps,sizeof(pbm_usim_capability_ext_type_v01) * QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01 ,
                            pb_caps->pb_set_caps.pb_set_info[iter].ad_num_caps ,sizeof(pbm_usim_capability_ext_s_type ) * QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE);
                    memscpy(resp_ptr->adn_pb_set_info.pbset_caps[iter].sne_caps,sizeof(pbm_usim_capability_ext_type_v01) * QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01 ,
                            pb_caps->pb_set_caps.pb_set_info[iter].sne_caps ,sizeof(pbm_usim_capability_ext_s_type ) * QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE);
                    memscpy(resp_ptr->adn_pb_set_info.pbset_caps[iter].grp_caps,sizeof(pbm_usim_capability_ext_type_v01) * QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01 ,
                             pb_caps->pb_set_caps.pb_set_info[iter].grp_caps ,sizeof(pbm_usim_capability_ext_s_type ) * QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE);
                  }
      }


      if(pb_caps->num_pb_sets)
      {
           resp_ptr->num_pb_sets_valid = 0x01;
           resp_ptr->num_pb_sets       = pb_caps->num_pb_sets ;
      }

      if(pb_caps->is_aas_cap_valid)
      {
           resp_ptr->aas_capability_valid               = 0x01;
           resp_ptr->aas_capability.max_records         = pb_caps->aas_cap.max_recs ;
           resp_ptr->aas_capability.used_records        = pb_caps->aas_cap.used_recs ;
           resp_ptr->aas_capability.max_aas_string_len  = pb_caps->aas_cap.max_aas_len;
        }

      if(pb_caps->is_gas_cap_valid)
      {
           resp_ptr->gas_capability_valid               = 0x01;
           resp_ptr->gas_capability.max_records         = pb_caps->gas_cap.max_recs ;
           resp_ptr->gas_capability.used_records        = pb_caps->gas_cap.used_recs ;
           resp_ptr->gas_capability.max_gas_string_len  = pb_caps->gas_cap.max_gas_len;
      }

      resp_ptr->is_hidden_entry_supported_valid = 0x01 ;
      resp_ptr->is_hidden_entry_supported       = pb_caps->is_hidden_entry_supported ;


      if(pb_caps->is_sec_name_supported)
      {
           resp_ptr->max_second_name_len_valid = 0x01 ;
           resp_ptr->max_second_name_len        = pb_caps->max_sec_name_len ;
      }


      if(pb_caps->is_email_valid)
      {
           resp_ptr->email_capability_valid         = 0x01 ;
           resp_ptr->email_capability.max_email     = pb_caps->email_cap.max_emails ;
           resp_ptr->email_capability.max_email_len = pb_caps->email_cap.max_email_len ;
      }


      if(pb_caps->is_ad_num_valid)
      {
           resp_ptr->ad_num_capability_valid              = 0x01 ;
           resp_ptr->ad_num_capability.max_ad_num         = pb_caps->ad_num_cap.max_ad_nums;
           resp_ptr->ad_num_capability.max_ad_num_len     = pb_caps->ad_num_cap.max_ad_num_len ;
           resp_ptr->ad_num_capability.max_ad_num_tag_len = pb_caps->ad_num_cap.max_ad_num_tag_len ;
      }


      if(pb_caps->is_grp_valid)
      {
           resp_ptr->group_capability_valid           = 0x01 ;
           resp_ptr->group_capability.max_grp         = pb_caps->grp_cap.max_grps ;
           resp_ptr->group_capability.max_grp_tag_len = pb_caps->grp_cap.max_grp_tag_len;
      }
    }

    if(pb_caps->write_access_cap.pbm_protection_type != QMI_PBM_IF_NOT_KNOWN)
    {
         resp_ptr->write_access_info_valid             = 0x01 ;
         resp_ptr->write_access_info.pin_mask          = pb_caps->write_access_cap.pbm_pin_mask ;
         resp_ptr->write_access_info.protection_method = pb_caps->write_access_cap.pbm_protection_type ;
    }

    resp_ptr->capability_basic_info_valid         =  0x01 ;
    resp_ptr->capability_basic_info.session_type  =  sess_type  ;
    resp_ptr->capability_basic_info.pb_type       =  pb_caps->pb_type ;
    resp_ptr->capability_basic_info.used_records  =  pb_caps->used_recs ;
    resp_ptr->capability_basic_info.max_records   =  pb_caps->max_recs ;
    resp_ptr->capability_basic_info.max_num_len   =  pb_caps->max_num_len ;
    resp_ptr->capability_basic_info.max_name_len  =  pb_caps->max_name_len ;
  }
  else
  {
      resp_ptr->resp.result = QMI_RESULT_FAILURE_V01 ;
      resp_ptr->resp.error =  (qmi_error_type_v01)errval ;
  }

  if( FALSE ==  qmi_pbm_idl_message_encode(pbm_get_service_object_v01(),
  	                              QMI_IDL_RESPONSE,
  	                              QMI_GET_PB_CAPABILITIES_RESP_V01,
  	                              (void *)resp_ptr,(uint32_t) sizeof(pbm_get_pb_capabilities_resp_msg_v01),
  	                              &response))
  {
       UIM_MSG_HIGH_0 ("Unable to pack response for PBM GET CAPS operation");
       errval = QMI_ERR_INTERNAL;
       result_added = FALSE ;
  }

  QMI_PBM_IF_MEM_FREE(resp_ptr);

 }
 else
 {
     errval = QMI_ERR_NO_MEMORY;
     result_added = FALSE ;
 }

  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,result_added))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET CAPS operation");
  }

} /* qmi_pbmi_get_pb_capabilities_resp() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_ALL_PB_CAPABILITIES()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_all_pb_capabilities
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *           response          = NULL;
  qmi_error_e_type          errval            = QMI_ERR_NONE;
  qmi_result_e_type         result            = QMI_RESULT_SUCCESS;
  boolean                   retval            = TRUE;
  qmi_pbm_if_info_item_s *  pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_GET_ALL_PB_CAPABILITIES_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if(*sdu_in != NULL)
  {
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_GET_ALL_PB_CAPABILITIES_V01 */

send_result:
  result = QMI_RESULT_FAILURE;

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_all_pb_capabilities() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_ALL_PB_CAPABILITIES_RESP()

  DESCRIPTION
    Send all the phonebook capabilites of all the sessions

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_all_pb_capabilities_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type                     *cmd_buf_p;
  dsm_item_type                        *response;
  qmi_error_e_type                     errval;
  const qmi_pbm_if_pb_caps_s           *pb_caps;
  const qmi_pbm_if_session_caps_s      *sess_caps;
  uint16                               l = 0 ;
  uint8                                i = 0, j = 0;
  boolean                              result_added = TRUE;
  pbm_get_all_pb_capabilities_resp_msg_v01 *resp_ptr=NULL;
  qmi_pbmi_client_state_type * cl_sp;


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  cl_sp =  qmi_pbmi_get_client_sp_by_cmd_buf(cmd_buf_p);
  if(cl_sp == NULL)
  {
    UIM_MSG_HIGH_0("Null client state ptr in qmi_pbmi_get_all_pb_capabilities_resp");
    return;
  }

  errval = data_ptr->data.get_all_pb_caps_resp.error;

  response = NULL;

  resp_ptr = (pbm_get_all_pb_capabilities_resp_msg_v01 *) QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_get_all_pb_capabilities_resp_msg_v01));

  if ( resp_ptr != NULL )
  {
    memset( resp_ptr , 0 , sizeof(pbm_get_all_pb_capabilities_resp_msg_v01) );
    if( QMI_ERR_NONE == errval )
    {
      for(i=0; i<data_ptr->data.get_all_pb_caps_resp.num_of_sessions;i++)
      {
         sess_caps = &data_ptr->data.get_all_pb_caps_resp.sess_caps[i];
         if(!cl_sp->pbm_subs_info.is_bind_subs_recv ||
             qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_caps->session_type))
         {
            resp_ptr->capability_basic_info_array_valid = 0x01 ;
            l = resp_ptr->capability_basic_info_array_len;
            resp_ptr->capability_basic_info_array[l].session_type = sess_caps->session_type ;
            resp_ptr->capability_basic_info_array_len++;

            for(j=0;j<sess_caps->num_of_pbs;j++)
            {
               pb_caps = &sess_caps->pb_caps[j];
               resp_ptr->capability_basic_info_array[l].capability_basic_info_len++;
               resp_ptr->capability_basic_info_array[l].capability_basic_info[j].pb_type =  pb_caps->pb_type ;
               resp_ptr->capability_basic_info_array[l].capability_basic_info[j].used_records =  pb_caps->used_recs ;
               resp_ptr->capability_basic_info_array[l].capability_basic_info[j].max_records = pb_caps->max_recs ;
               resp_ptr->capability_basic_info_array[l].capability_basic_info[j].max_num_len =  pb_caps->max_num_len ;
               resp_ptr->capability_basic_info_array[l].capability_basic_info[j].max_name_len = pb_caps->max_name_len ;

               if( (pb_caps->pb_type == (uint16)QMI_PBM_IF_PB_ADN) )
               {

                  if( pb_caps->pbset_info_valid )
                  {
                     resp_ptr->pb_set_info_array_valid = 0x01 ;
                     l = resp_ptr->pb_set_info_array_len;
                     resp_ptr->pb_set_info_array[l].session_type = sess_caps->session_type ;
                     memscpy(&resp_ptr->pb_set_info_array[l].pb_set_info , sizeof(pbm_pbr_cap_info_type_v01),
                             &pb_caps->pb_set_caps,sizeof(pbm_pbr_cap_info_s_type));
                     resp_ptr->pb_set_info_array_len++;
                  }

                  if( pb_caps->is_aas_cap_valid )
                  {
                     resp_ptr->aas_capability_array_valid = 0x01;
                     l = resp_ptr->aas_capability_array_len ;
                     resp_ptr->aas_capability_array[l].session_type = sess_caps->session_type ;
                     resp_ptr->aas_capability_array[l].max_records = pb_caps->aas_cap.max_recs ;
                     resp_ptr->aas_capability_array[l].used_records = pb_caps->aas_cap.used_recs ;
                     resp_ptr->aas_capability_array[l].max_aas_string_len = pb_caps->aas_cap.max_aas_len ;
                     resp_ptr->aas_capability_array_len++ ;
                  }

                  if( pb_caps->is_gas_cap_valid )
                  {
                     resp_ptr->gas_capability_array_valid = 0x01;
                     l = resp_ptr->gas_capability_array_len ;
                     resp_ptr->gas_capability_array[l].session_type = sess_caps->session_type ;
                     resp_ptr->gas_capability_array[l].max_records = pb_caps->gas_cap.max_recs ;
                     resp_ptr->gas_capability_array[l].used_records = pb_caps->gas_cap.used_recs ;
                     resp_ptr->gas_capability_array[l].max_gas_string_len = pb_caps->gas_cap.max_gas_len ;
                     resp_ptr->gas_capability_array_len++;
                  }

                  if( pb_caps->is_sec_name_supported )
                  {
                     resp_ptr->second_name_capability_array_valid = 0x01 ;
                     l = resp_ptr->second_name_capability_array_len ;
                     resp_ptr->second_name_capability_array[l].session_type = sess_caps->session_type ;
                     resp_ptr->second_name_capability_array[l].max_second_name_len  =  pb_caps->max_sec_name_len  ;
                     resp_ptr->second_name_capability_array_len++	;
                  }

                  if( pb_caps->is_email_valid )
                  {
                     resp_ptr->email_capability_array_valid = 0x01 ;
                     l = resp_ptr->email_capability_array_len;
                     resp_ptr->email_capability_array[l].session_type = sess_caps->session_type ;
                     resp_ptr->email_capability_array[l].max_email = pb_caps->email_cap.max_emails ;
                     resp_ptr->email_capability_array[l].max_email_len = pb_caps->email_cap.max_email_len ;
                     resp_ptr->email_capability_array_len++ ;
                  }

                  if( pb_caps->is_ad_num_valid )
                  {
                     resp_ptr->ad_num_capability_array_valid = 0x01 ;
                     l = resp_ptr->ad_num_capability_array_len ;
                     resp_ptr->ad_num_capability_array[l].session_type = sess_caps->session_type ;
                     resp_ptr->ad_num_capability_array[l].max_ad_num = pb_caps->ad_num_cap.max_ad_nums  ;
                     resp_ptr->ad_num_capability_array[l].max_ad_num_len = pb_caps->ad_num_cap.max_ad_num_len ;
                     resp_ptr->ad_num_capability_array[l].max_ad_num_tag_len = pb_caps->ad_num_cap.max_ad_num_tag_len;
                     resp_ptr->ad_num_capability_array_len++ ;
                  }


                  if( pb_caps->is_grp_valid )
                  {
                     resp_ptr->group_capability_array_valid = 0x01 ;
                     l = resp_ptr->group_capability_array_len ;
                     resp_ptr->group_capability_array[l].session_type = sess_caps->session_type ;
                     resp_ptr->group_capability_array[l].max_grp = pb_caps->grp_cap.max_grps ;
                     resp_ptr->group_capability_array[l].max_grp_tag_len = pb_caps->grp_cap.max_grp_tag_len ;
                     resp_ptr->group_capability_array_len++ ;
                  }

                     resp_ptr->hidden_records_capability_array_valid = 0x01 ;
                     l = resp_ptr->hidden_records_capability_array_len ;
                     resp_ptr->hidden_records_capability_array[l].session_type = sess_caps->session_type ;
                     resp_ptr->hidden_records_capability_array[l].is_hidden_entry_supported = pb_caps->is_hidden_entry_supported ;
                     resp_ptr->hidden_records_capability_array_len++ ;
                  }
               }
            }
         }
      }
  else
  {
     resp_ptr->resp.result = QMI_RESULT_FAILURE_V01 ;
     resp_ptr->resp.error =  (qmi_error_type_v01)errval ;
  }

  if( FALSE == qmi_pbm_idl_message_encode(pbm_get_service_object_v01(),
                                          QMI_IDL_RESPONSE,
                                          QMI_PBM_GET_ALL_PB_CAPABILITIES_RESP_V01,
                                          (void *)resp_ptr,(uint32_t) sizeof(pbm_get_all_pb_capabilities_resp_msg_v01),
                                           &response))
  {
      UIM_MSG_HIGH_0 ("Unable to pack response for PBM GET ALL CAPS operation");
      errval = QMI_ERR_INTERNAL;
      result_added = FALSE ;
  }

  QMI_PBM_IF_MEM_FREE(resp_ptr);
 }
 else
 {
   errval = QMI_ERR_NO_MEMORY;
   result_added = FALSE ;
 }


  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,result_added))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET ALL CAPS operation");
  }

} /* qmi_pbmi_get_all_pb_capabilities_resp() */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_RECORDS()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_read_records
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *              response          = NULL;
  qmi_error_e_type             errval            = QMI_ERR_NONE;
  qmi_result_e_type            result            = QMI_RESULT_SUCCESS;
  boolean                      retval            = TRUE;
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr   = NULL;

#if defined (REMOVE_QMI_PBM_READ_RECORDS_V01) || defined (REMOVE_QMI_PBM_READ_RECORDS_EXT_V01)
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                        type              = 0;
  uint16                       len               = 0;
  uint16                       expected_len      = 0;
  boolean                      got_v_in_required = FALSE;
  qmi_pbm_if_read_records_s *  read_recs         = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  read_recs = &pbm_if_data_ptr->data.read_recs;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(read_recs->session_type) + \
                       sizeof(read_recs->pb_type) + \
                       sizeof(read_recs->start_id) + \
                       sizeof(read_recs->end_id);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &read_recs->session_type, sizeof(read_recs->session_type)) &&
               QMI_PBMI_PULL(sdu_in, &read_recs->pb_type, sizeof(read_recs->pb_type)) &&
               QMI_PBMI_PULL(sdu_in, &read_recs->start_id, sizeof(read_recs->start_id))&&
               QMI_PBMI_PULL(sdu_in, &read_recs->end_id, sizeof(read_recs->end_id)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;
      default:
        break;
    }
  }
  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_RECORDS;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_READ_RECORDS_V01 || REMOVE_QMI_PBM_READ_RECORDS_EXT_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
  }
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_read_records() */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_RECORDS_EXT_UNDECODED()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_read_records_ext_undecoded
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{

  dsm_item_type *                           response                = NULL;
  qmi_error_e_type                          errval                  = QMI_ERR_NONE;
  qmi_result_e_type                         result                  = QMI_RESULT_SUCCESS;
  boolean                                   retval                  = TRUE;
  qmi_pbm_if_info_item_s *                  pbm_if_data_ptr         = NULL;

#ifdef REMOVE_QMI_PBM_READ_RECORDS_EXT_UNDECODED_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                                     type                    = 0;
  uint16                                    len                     = 0;
  uint16                                    expected_len            = 0;
  boolean                                   got_v_in_required       = FALSE;
  qmi_pbm_if_read_records_ext_undecoded_s * read_recs_ext_undecoded = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  read_recs_ext_undecoded = &pbm_if_data_ptr->data.read_recs_ext_undecoded;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(read_recs_ext_undecoded->session_type) + \
                       sizeof(read_recs_ext_undecoded->pb_type) + \
                       sizeof(read_recs_ext_undecoded->start_id) + \
                       sizeof(read_recs_ext_undecoded->end_id);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &read_recs_ext_undecoded->session_type, sizeof(read_recs_ext_undecoded->session_type)) &&
               QMI_PBMI_PULL(sdu_in, &read_recs_ext_undecoded->pb_type, sizeof(read_recs_ext_undecoded->pb_type)) &&
               QMI_PBMI_PULL(sdu_in, &read_recs_ext_undecoded->start_id, sizeof(read_recs_ext_undecoded->start_id))&&
               QMI_PBMI_PULL(sdu_in, &read_recs_ext_undecoded->end_id, sizeof(read_recs_ext_undecoded->end_id)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;

      default:
        break;
    }
  }
  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }


  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_READ_RECORDS_EXT_UNDECODED_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
  }
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_read_records_ext_undecoded */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_PBSET_CAPS()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_read_pbset_caps
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *                response          = NULL;
  qmi_error_e_type               errval            = QMI_ERR_NONE;
  qmi_result_e_type              result            = QMI_RESULT_SUCCESS;
  boolean                        retval            = TRUE;
  qmi_pbm_if_info_item_s *       pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_READ_PBSET_CAPS_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                          type              = 0;
  uint16                         len               = 0;
  uint16                         expected_len      = 0;
  boolean                        got_v_in_required = FALSE;
  qmi_pbm_if_read_pbset_caps_s * read_pbset_caps   = NULL;
  boolean start_id_found = FALSE;
  boolean end_id_found = FALSE;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  read_pbset_caps = &pbm_if_data_ptr->data.read_pbset_caps;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(read_pbset_caps->session_type);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !QMI_PBMI_PULL(sdu_in, &read_pbset_caps->session_type, sizeof(read_pbset_caps->session_type)) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;

      case PBMI_READ_PBSET_CAPS_START_ID:
        if ( !QMI_PBMI_PULL(sdu_in, &read_pbset_caps->start_id, sizeof(read_pbset_caps->start_id)) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        start_id_found = TRUE;
        break;
      case PBMI_READ_PBSET_CAPS_END_ID:
        if ( !QMI_PBMI_PULL(sdu_in, &read_pbset_caps->end_id, sizeof(read_pbset_caps->end_id)) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        end_id_found = TRUE;
        break;
      default:
        break;
    }
  }
  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }
  if((read_pbset_caps->start_id == 0 && start_id_found == TRUE)
    || (read_pbset_caps->end_id == 0 && end_id_found == TRUE))
    {
      errval = QMI_ERR_INVALID_ID;
      goto send_result;
    }


  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_PBSET_CAPS;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_READ_PBSET_CAPS_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
  }
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_read_pbset_caps() */


static void qmi_pbmi_set_rec_read_clnt_data
(
  uint8 sess_type,
  uint16 pb_type,
  uint8  cl_id
)
{
  uint8 i;

  for(i=0;i<PBMI_MAX_CLIDS;i++)
  {
    if(rec_read_clnt[i].sess_type == QMI_PBM_IF_SESSION_MAX)
    {
      rec_read_clnt[i].sess_type = sess_type;
      rec_read_clnt[i].pb_type = pb_type;
      rec_read_clnt[i].cl_id = cl_id;
      UIM_MSG_HIGH_3 ("qmi_pbmi_set_rec_read_clnt_data cli_id(%d) for session_type (%d) pb_type (%d)",
		         i,rec_read_clnt[i].sess_type,rec_read_clnt[i].pb_type);
      break;
    }
  }
} /* qmi_pbmi_set_rec_read_clnt_data */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_REC_READ_CL_ID

  DESCRIPTION

  PARAMETERS

  RETURN VALUE
    uint8

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 qmi_pbmi_get_rec_read_cl_id
(
  uint8 sess_type,
  uint16 pb_type
)
{
  uint8 i, cl_id=0;

  for(i=0;i<PBMI_MAX_CLIDS;i++)
  {
    if( (rec_read_clnt[i].sess_type == sess_type) &&
        (rec_read_clnt[i].pb_type == pb_type) )
    {
      UIM_MSG_HIGH_3 ("qmi_pbmi_get_rec_read_cl_id cli_id(%d) for session_type (%d) pb_type (%d)",
		         i,rec_read_clnt[i].sess_type,rec_read_clnt[i].pb_type);
      cl_id = rec_read_clnt[i].cl_id;
      break;
    }
  }
  return cl_id;
} /* qmi_pbmi_get_rec_read_cl_id */


/*===========================================================================
  FUNCTION QMI_PBMI_CLEAR_REC_READ_CLNT_DATA()

  DESCRIPTION

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_clear_rec_read_clnt_data
(
  uint8 sess_type,
  uint16 pb_type
)
{
  uint8 i;

  for(i=0;i<PBMI_MAX_CLIDS;i++)
  {
    if( (rec_read_clnt[i].sess_type == sess_type) &&
        (rec_read_clnt[i].pb_type == pb_type) )
    {
      UIM_MSG_HIGH_3 ("qmi_pbmi_clear_rec_read_clnt_data cli_id(%d) for session_type (%d) pb_type (%d)",
		                 i,rec_read_clnt[i].sess_type,rec_read_clnt[i].pb_type);
      rec_read_clnt[i].sess_type = QMI_PBM_IF_SESSION_MAX;
      break;
    }
  }
} /* qmi_pbmi_clear_rec_read_clnt_data */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_RECORDS_RESP()

  DESCRIPTION
    Return the record data

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_read_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint8 sess_type;
  uint16 pb_type;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_read_records_resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.read_recs_resp.error;
  sess_type = data_ptr->data.read_recs_resp.sess_type;
  pb_type = data_ptr->data.read_recs_resp.pb_type;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    if(data_ptr->data.read_recs_resp.rec_cnt != 0)
    {
      qmi_pbmi_set_rec_read_clnt_data(sess_type, pb_type, cmd_buf_p->x_p->cl_sp->clid);
    }
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                  PBMI_READ_RECS_RSP_NUM_RECS,
                                  sizeof(data_ptr->data.read_recs_resp.rec_cnt),
                                  (void *) &(data_ptr->data.read_recs_resp.rec_cnt)) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
      goto send_result;
    }
  }

send_result:
  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM READ RECORDS operation");
  }
} /* qmi_pbmi_read_records_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_RECORDS_EXT_UNDECODED_RESP()

  DESCRIPTION
    Return the record data

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_read_records_ext_undecoded_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint8 sess_type;
  uint16 pb_type;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_read_records_ext_undecoded_resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.read_recs_ext_undecoded_resp.error;
  sess_type = data_ptr->data.read_recs_ext_undecoded_resp.sess_type;
  pb_type = data_ptr->data.read_recs_ext_undecoded_resp.pb_type;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    if(data_ptr->data.read_recs_ext_undecoded_resp.rec_cnt != 0)
    {
      qmi_pbmi_set_rec_read_clnt_data(sess_type, pb_type, cmd_buf_p->x_p->cl_sp->clid);
    }
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                  PBMI_READ_RECS_EXT_UNDECODED_RSP_NUM_RECS,
                                  sizeof(data_ptr->data.read_recs_ext_undecoded_resp.rec_cnt),
                                  (void *) &(data_ptr->data.read_recs_ext_undecoded_resp.rec_cnt)) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
      goto send_result;
    }
  }

send_result:
  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM READ RECORDS EXT UNDECODED operation");
  }
} /* qmi_pbmi_read_records_ext_undecoded_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_PBSET_CAPS_RESP()

  DESCRIPTION
    Return the record data

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_read_pbset_caps_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint8 sess_type;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_read_records_resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.read_pbset_caps_resp.error;
  sess_type = data_ptr->data.read_pbset_caps_resp.sess_type;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    if(data_ptr->data.read_pbset_caps_resp.pbset_cnt != 0)
    {
      qmi_pbmi_set_rec_read_clnt_data(sess_type, QMI_PBM_IF_PB_ADN, cmd_buf_p->x_p->cl_sp->clid);
    }
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                  PBMI_READ_PBSET_CAPS_RSP_NUM_PBSETS,
                                  sizeof(data_ptr->data.read_pbset_caps_resp.pbset_cnt),
                                  (void *) &(data_ptr->data.read_pbset_caps_resp.pbset_cnt)) )
  {
      errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
      goto send_result;
    }
  }

send_result:
  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM READ RECORDS operation");
  }
} /* qmi_pbmi_read_records_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_WRITE_RECORD()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_write_record
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *             response          = NULL;
  qmi_error_e_type            errval            = QMI_ERR_NONE;
  qmi_result_e_type           result            = QMI_RESULT_SUCCESS;
  boolean                     retval            = TRUE;
  qmi_pbm_if_info_item_s *    pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_WRITE_RECORD_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                       type              = 0;
  uint8                       i                 = 0;
  uint16                      len               = 0;
  uint16                      siz               = 0;
  uint16                      numsiz            = 0;
  boolean                     got_v_in_required = FALSE;
  qmi_pbm_if_write_record_s * write_rec         = NULL;
  qmi_pbm_if_rec_data_s *     rec               = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  write_rec = &pbm_if_data_ptr->data.write_rec;
  rec = &write_rec->rec_data;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        siz = sizeof(write_rec->session_type) + sizeof(write_rec->pb_type);
        numsiz = sizeof(rec->num_info.num_type) + sizeof(rec->num_info.num_plan) + \
                 sizeof(rec->num_info.len);

        if ( !(QMI_PBMI_PULL(sdu_in, &write_rec->session_type, sizeof(write_rec->session_type)) &&
               QMI_PBMI_PULL(sdu_in, &write_rec->pb_type, sizeof(write_rec->pb_type)) &&
               QMI_PBMI_PULL(sdu_in, &rec->rec_id, sizeof(rec->rec_id))&&
               QMI_PBMI_PULL(sdu_in, &rec->num_info, numsiz) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if (rec->num_info.len > sizeof(rec->num_info.num) )
        {
          errval = QMI_ERR_PB_NUMBER_TOO_LONG;
          goto send_result;
        }

        if( !(QMI_PBMI_PULL(sdu_in, rec->num_info.num, rec->num_info.len) &&
               QMI_PBMI_PULL(sdu_in, &rec->name_info.len, sizeof(rec->name_info.len))) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( rec->name_info.len > ((QMI_PBM_IF_MAX_TEXT_EXT_LEN )))
        {
          UIM_MSG_HIGH_0("Invalid Name length in TLV");
          errval = QMI_ERR_PB_TEXT_TOO_LONG;
          goto send_result;
        }

        if( !QMI_PBMI_PULL(sdu_in, rec->name_info.text, rec->name_info.len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }

        len -= (siz + sizeof(rec->rec_id) + numsiz + rec->num_info.len + sizeof(rec->name_info.len) + \
                rec->name_info.len);
        if(len != 0)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;

      case PBMI_WRITE_REC_SNAME:
        if (len > sizeof(rec->sname_info)
            || !QMI_PBMI_PULL(sdu_in, &rec->sname_info, len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_AD_NUM:
        if ( !(QMI_PBMI_PULL(sdu_in, &rec->ad_num_cnt, sizeof(rec->ad_num_cnt))
                && rec->ad_num_cnt <= QMI_PBM_IF_MAX_AD_NUM))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        for(i=0;i<rec->ad_num_cnt;i++)
        {
          qmi_pbm_if_ad_num_data_s *ad = &rec->ad_num_data[i];
          numsiz = sizeof(ad->ad_num.num_type) + sizeof(ad->ad_num.num_plan) + \
                   sizeof(ad->ad_num.len);
          if ( !QMI_PBMI_PULL(sdu_in, &ad->ad_num, numsiz))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if (ad->ad_num.len > sizeof(ad->ad_num.num) ||
                !(QMI_PBMI_PULL(sdu_in, ad->ad_num.num, ad->ad_num.len)&&
                 QMI_PBMI_PULL(sdu_in, &ad->tag_id, sizeof(ad->tag_id)) ) )
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          len -= (numsiz + ad->ad_num.len + sizeof(ad->tag_id));
        }
        len -= sizeof(rec->ad_num_cnt);
        if(len != 0)
        {
          UIM_MSG_HIGH_0("Invalid length in TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_GRP:
        if ( !(QMI_PBMI_PULL(sdu_in, &rec->grp_cnt, sizeof(rec->grp_cnt))
                && rec->grp_cnt <= QMI_PBM_IF_MAX_GRP))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        for(i=0;i<rec->grp_cnt;i++)
        {
          if ( !QMI_PBMI_PULL(sdu_in, &rec->grp_id[i], sizeof(rec->grp_id[i])) )
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          len -= sizeof(rec->grp_id[i]);
        }
        len -= sizeof(rec->grp_cnt);
        if(len != 0)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_EMAIL:
        if ( !(QMI_PBMI_PULL(sdu_in, &rec->email_cnt, sizeof(rec->email_cnt))
               && rec->email_cnt <=  QMI_PBM_IF_MAX_EMAIL))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        for(i=0;i<rec->email_cnt;i++)
        {
          qmi_pbm_if_text_info_s *email = &rec->email_data[i];
          if ( !QMI_PBMI_PULL(sdu_in, &email->len, sizeof(email->len)))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if ( email->len > ((QMI_PBM_IF_MAX_TEXT_EXT_LEN)))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if (!QMI_PBMI_PULL(sdu_in, email->text, email->len) )
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          len -= (sizeof(email->len) + email->len);
        }
        len -= sizeof(rec->email_cnt);
        if(len != 0)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_HIDDEN:
        if (len != sizeof(rec->is_hidden) ||
             !QMI_PBMI_PULL(sdu_in, &rec->is_hidden, len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      default:
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_WRITE_RECORD;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_WRITE_RECORD_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_write_record() */


/*===========================================================================
  FUNCTION QMI_PBMI_WRITE_RECORD_EXT()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_write_record_ext
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *             response          = NULL;
  qmi_error_e_type            errval            = QMI_ERR_NONE;
  qmi_result_e_type           result            = QMI_RESULT_SUCCESS;
  boolean                     retval            = TRUE;
  qmi_pbm_if_info_item_s *    pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_WRITE_RECORD_EXT_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                       type              = 0;
  uint8                       i                 = 0;
  uint16                      len               = 0;
  uint16                      siz               = 0;
  uint16                      numsiz            = 0;
  boolean                     got_v_in_required = FALSE;
  qmi_pbm_if_write_record_s * write_rec         = NULL;
  qmi_pbm_if_rec_data_s *     rec               = NULL;


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  write_rec = &pbm_if_data_ptr->data.write_rec;
  rec = &write_rec->rec_data;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        siz = sizeof(write_rec->session_type) + sizeof(write_rec->pb_type) + sizeof(rec->rec_id);

        if ( !(QMI_PBMI_PULL(sdu_in, &write_rec->session_type, sizeof(write_rec->session_type)) &&
               QMI_PBMI_PULL(sdu_in, &write_rec->pb_type, sizeof(write_rec->pb_type)) &&
               QMI_PBMI_PULL(sdu_in, &rec->rec_id, sizeof(rec->rec_id))) )
        {
          UIM_MSG_HIGH_0("Invalid length in TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if(len != siz)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;
      case PBMI_WRITE_REC_EXT_NUMBER:
        numsiz = sizeof(rec->num_info.num_type) + sizeof(rec->num_info.num_plan) + \
                 sizeof(rec->num_info.len);
        if(!QMI_PBMI_PULL(sdu_in, &rec->num_info, numsiz))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if (rec->num_info.len > sizeof(rec->num_info.num) )
        {
          UIM_MSG_HIGH_0("Invalid Number length in TLV");
          errval = QMI_ERR_PB_NUMBER_TOO_LONG;
          goto send_result;
        }
        if( !(QMI_PBMI_PULL(sdu_in, rec->num_info.num, rec->num_info.len) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;
      case PBMI_WRITE_REC_EXT_NAME:
        if( !(QMI_PBMI_PULL(sdu_in, &rec->name_info.len, sizeof(rec->name_info.len))) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }

        if (!QMI_PBMI_PULL(sdu_in, rec->name_info.text, rec->name_info.len * 2) )
        {
          UIM_MSG_HIGH_0("Invalid length in TLV");
          errval = QMI_ERR_PB_TEXT_TOO_LONG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_EXT_SNAME:
        if(!QMI_PBMI_PULL(sdu_in, &rec->sname_info.len, sizeof(rec->sname_info.len)))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }

        if (!QMI_PBMI_PULL(sdu_in, rec->sname_info.text, rec->sname_info.len * 2) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_EXT_AD_NUM:
        if ( !(QMI_PBMI_PULL(sdu_in, &rec->ad_num_cnt, sizeof(rec->ad_num_cnt))
                && rec->ad_num_cnt <= QMI_PBM_IF_MAX_AD_NUM))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        for(i=0;i<rec->ad_num_cnt;i++)
        {
          qmi_pbm_if_ad_num_data_s *ad = &rec->ad_num_data[i];
          numsiz = sizeof(ad->ad_num.num_type) + sizeof(ad->ad_num.num_plan) + \
                   sizeof(ad->ad_num.len);
          if ( !QMI_PBMI_PULL(sdu_in, &ad->ad_num, numsiz))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if (ad->ad_num.len > sizeof(ad->ad_num.num) ||
                !(QMI_PBMI_PULL(sdu_in, ad->ad_num.num, ad->ad_num.len)&&
                 QMI_PBMI_PULL(sdu_in, &ad->tag_id, sizeof(ad->tag_id)) ) )
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          len -= (numsiz + ad->ad_num.len + sizeof(ad->tag_id));
        }
        len -= sizeof(rec->ad_num_cnt);
        if(len != 0)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_EXT_GRP:
        if ( !(QMI_PBMI_PULL(sdu_in, &rec->grp_cnt, sizeof(rec->grp_cnt))
                && rec->grp_cnt <= QMI_PBM_IF_MAX_GRP))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        for(i=0;i<rec->grp_cnt;i++)
        {
          if ( !QMI_PBMI_PULL(sdu_in, &rec->grp_id[i], sizeof(rec->grp_id[i])) )
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          len -= sizeof(rec->grp_id[i]);
        }
        len -= sizeof(rec->grp_cnt);
        if(len != 0)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_EXT_EMAIL:
        if ( !(QMI_PBMI_PULL(sdu_in, &rec->email_cnt, sizeof(rec->email_cnt))
               && rec->email_cnt <=  QMI_PBM_IF_MAX_EMAIL))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        for(i=0;i<rec->email_cnt;i++)
        {
          qmi_pbm_if_text_info_s *email = &rec->email_data[i];
          if ( !QMI_PBMI_PULL(sdu_in, &email->len, sizeof(email->len)))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          if ( !QMI_PBMI_PULL(sdu_in, email->text, email->len * 2) )
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          len -= (sizeof(email->len) + (email->len * 2));
        }
        len -= sizeof(rec->email_cnt);
        if(len != 0)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      case PBMI_WRITE_REC_EXT_HIDDEN:
        if (len != sizeof(rec->is_hidden) ||
             !QMI_PBMI_PULL(sdu_in, &rec->is_hidden, len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;

      default:
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }
  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_WRITE_RECORD_EXT;

  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_WRITE_RECORD_EXT_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_write_record_ext() */


/*===========================================================================
  FUNCTION QMI_PBMI_WRITE_RECORD_RESP()

  DESCRIPTION
    Response to the record write operation

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_write_record_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  const qmi_pbm_if_write_record_resp_s *rsp;
  uint16 tot_len = 0;
  uint8 tag = PBMI_WRITE_REC_RSP_INFO;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_write_record_resp");
    return;
  }

  rsp = &data_ptr->data.write_rec_resp;

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = rsp->error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    tot_len = sizeof(rsp->rec_id);
    if( !(QMI_SVC_PKT_PUSH(&response, (void*)&rsp->rec_id, sizeof(rsp->rec_id)) &&
          QMI_SVC_PKT_PUSH(&response, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
          QMI_SVC_PKT_PUSH(&response, (void*)&tag, PBMI_TLV_TAG_SIZE) ) )
    {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
    }
  }


  UIM_MSG_HIGH_1 ("Sending the write record resp with rec id %d", rsp->rec_id);
  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))

  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM WRITE RECORD operation");
  }
} /* qmi_pbmi_write_record_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_DELETE_RECORD()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_delete_record
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *              response          = NULL;
  qmi_error_e_type             errval            = QMI_ERR_NONE;
  qmi_result_e_type            result            = QMI_RESULT_SUCCESS;
  boolean                      retval            = TRUE;
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_DELETE_RECORD_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                        type              = 0;
  uint16                       len               = 0;
  uint16                       expected_len      = 0;
  boolean                      got_v_in_required = FALSE;
  qmi_pbm_if_delete_record_s * del_rec           = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  del_rec = &pbm_if_data_ptr->data.delete_rec;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(del_rec->session_type) + \
                                  sizeof(del_rec->pb_type) + \
                                  sizeof(del_rec->record_id);
        if ( (expected_len != len) )
        {
          UIM_MSG_HIGH_2 ("Invalid TLV len (%d) for type (%d)", len, type);
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &del_rec->session_type, sizeof(del_rec->session_type)) &&
                 QMI_PBMI_PULL(sdu_in, &del_rec->pb_type, sizeof(del_rec->pb_type)) &&
                 QMI_PBMI_PULL(sdu_in, &del_rec->record_id, sizeof(del_rec->record_id)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;
      default:
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_DELETE_RECORD;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_DELETE_RECORD_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_delete_record() */


/*===========================================================================
  FUNCTION QMI_PBMI_DELETE_RECORD_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_delete_record_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  qmi_pbm_if_delete_record_resp_s  del_resp;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_delete_record_resp");
    return;
  }


  del_resp.error = data_ptr->data.delete_rec_resp.error;
  del_resp.record_id = data_ptr->data.delete_rec_resp.record_id;

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = del_resp.error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
      if(FALSE == qmi_svc_put_param_tlv(&response,
                                        PBMI_DELETE_REC_ID,
                                        sizeof(del_resp.record_id),
                                        (void *) &(del_resp.record_id)) )
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
  }

send_result:

  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM DELETE RECORD operation");
  }
} /* qmi_pbmi_delete_record_resp() */


/*===========================================================================
  FUNCTION QMI_PBMI_DELETE_ALL_PB_RECORDS()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_delete_all_pb_records
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *                      response          = NULL;
  qmi_error_e_type                     errval            = QMI_ERR_NONE;
  qmi_result_e_type                    result            = QMI_RESULT_SUCCESS;
  boolean                              retval            = TRUE;
  qmi_pbm_if_info_item_s *             pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_DELETE_ALL_PB_RECORDS_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                                type              = 0;
  uint16                               len               = 0;
  uint16                               expected_len      = 0;
  boolean                              got_v_in_required = FALSE;
  qmi_pbm_if_delete_all_pb_records_s * del_all_recs      = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  del_all_recs = &pbm_if_data_ptr->data.delete_all_pb_recs;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(del_all_recs->session_type) + \
                                  sizeof(del_all_recs->pb_type);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &del_all_recs->session_type, sizeof(del_all_recs->session_type)) &&
                 QMI_PBMI_PULL(sdu_in, &del_all_recs->pb_type, sizeof(del_all_recs->pb_type)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;
      default:
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);

  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_DELETE_ALL_PB_RECORDS_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_delete_all_pb_records() */


/*===========================================================================
  FUNCTION QMI_PBM_CREATE_AND_SEND_CMD()

  DESCRIPTION

  PARAMETERS
  qmi_pbm_cmd_enum_type
  qmi_pbm_if_info_item_s*

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_pbm_create_and_send_cmd(qmi_pbm_cmd_enum_type cmd,
                           qmi_pbm_if_info_item_s* pbm_if_data_ptr)
{
  qmi_pbm_cmd_data_buf_type *cmd_ptr;
  ASSERT(pbm_if_data_ptr);

  cmd_ptr = qmi_pbm_get_cmd_data_buf();
  if(NULL == cmd_ptr)
  {
    return QMI_ERR_INTERNAL;
  }
  cmd_ptr->cmd.pbm_if_data_ptr = pbm_if_data_ptr;
  qmi_pbm_send_cmd(cmd, cmd_ptr);
  return QMI_ERR_NONE;
} /* qmi_pbm_create_and_send_cmd() */


/*===========================================================================
  FUNCTION QMI_PBMI_DELETE_ALL_PB_RECORDS_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_delete_all_pb_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_delete_all_pb_records_resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.delete_all_pb_recs_resp.error;

  response = NULL;


  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM DELETE ALL PB RECORDS operation");
  }
} /* qmi_pbmi_delete_all_pb_records_resp() */


/*===========================================================================
  FUNCTION QMI_PBMI_SEARCH_RECORDS()

  DESCRIPTION
    Search for a Number/Name in Records

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_search_records
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *               response           = NULL;
  qmi_error_e_type              errval             = QMI_ERR_NONE;
  qmi_result_e_type             result             = QMI_RESULT_SUCCESS;
  boolean                       retval             = TRUE;
  qmi_pbm_if_info_item_s *      pbm_if_data_ptr    = NULL;

#ifdef REMOVE_QMI_PBM_SEARCH_RECORDS_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                         type               = 0;
  uint16                        len                = 0;
  uint16                        expected_len       = 0;
  boolean                       got_v_in_required  = FALSE;
  uint16                        name_len_in_bytes  = 0;
  qmi_pbm_if_search_records_s * search_recs        = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  search_recs = &pbm_if_data_ptr->data.search_recs;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(search_recs->session_type) + \
                               sizeof(search_recs->pb_type);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &search_recs->session_type, sizeof(search_recs->session_type)) &&
               QMI_PBMI_PULL(sdu_in, &search_recs->pb_type, sizeof(search_recs->pb_type)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;
      case PBMI_SEARCH_REC_NUM_INFO:
        if(!QMI_PBMI_PULL(sdu_in, &search_recs->num_info.num_len, sizeof(search_recs->num_info.num_len)))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if(search_recs->num_info.num_len > QMI_PBM_IF_MAX_NUM_LEN)
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        if(!QMI_PBMI_PULL(sdu_in, search_recs->num_info.number, search_recs->num_info.num_len))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        search_recs->number_info_valid = TRUE;
        break;
      case PBMI_SEARCH_REC_NAME_INFO:
        if(!QMI_PBMI_PULL(sdu_in, &search_recs->name_info.name_len, sizeof(search_recs->name_info.name_len)))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if((search_recs->name_info.name_len > QMI_PBM_IF_MAX_NAME_LEN
              && ((qmi_cmd_buf_type *)cmd_buf_p)->cmd_type == PBMI_CMD_VAL_SEARCH_RECORDS))
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        name_len_in_bytes = search_recs->name_info.name_len;
        if(((qmi_cmd_buf_type *)cmd_buf_p)->cmd_type == PBMI_CMD_VAL_SEARCH_RECORDS_EXT)
        {
          name_len_in_bytes = search_recs->name_info.name_len * 2;
        }
        if(!QMI_PBMI_PULL(sdu_in, search_recs->name_info.name, name_len_in_bytes))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        search_recs->name_info_valid = TRUE;
        break;
      default:
        break;
    }
  }

  if (!got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if ( search_recs->number_info_valid && search_recs->name_info_valid )
  {
    errval = QMI_ERR_INVALID_ARG ;
    UIM_MSG_ERR_0("Either name or number is supported in search");
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_SEARCH_RECORDS;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);

  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_SEARCH_RECORDS_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_search_records() */


/*===========================================================================
  FUNCTION QMI_PBMI_SEARCH_RECORDS_RESP()

  DESCRIPTION
    Response of Search records

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_search_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint16 total_len=0;
  int j=0;
  uint8 rec_tag = PBMI_SEARCH_RECS_RESP;
  const qmi_pbm_if_search_records_resp_s  *search_resp;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_search_records_resp");
    return;
  }
  if(data_ptr->data.search_recs_resp == NULL)
  {
    UIM_MSG_HIGH_0("Search resp data is NULL");
    return;
  }


  search_resp = data_ptr->data.search_recs_resp;

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = search_resp->error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    for(j=search_resp->num_rec_ids;j>0;j--)
    {
      if( !( QMI_SVC_PKT_PUSH(&response, (void*)&search_resp->rec_ids[j-1], sizeof(search_resp->rec_ids[j-1]))))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
      total_len += sizeof(search_resp->rec_ids[j-1]);
    }
    if( !( QMI_SVC_PKT_PUSH(&response, (void*)&search_resp->num_rec_ids, sizeof(search_resp->num_rec_ids)) ) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
      goto send_result;
    }
    total_len +=  sizeof(search_resp->num_rec_ids);
    if( !( QMI_SVC_PKT_PUSH(&response, (void*)&total_len, PBMI_TLV_LEN_SIZE) &&
           QMI_SVC_PKT_PUSH(&response, (void*)&rec_tag, PBMI_TLV_TAG_SIZE) ) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
      goto send_result;
    }
  }

send_result:

  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM SEARCH RECORDS operation");
  }
} /* qmi_pbmi_search_records_resp() */
/*===========================================================================
                          INDICATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION QMI_PBMI_REFRESH_IND()

  DESCRIPTION
    Send refresh indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_refresh_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  uint16 len = 0;
  qmi_pbm_if_refresh_ind_s ind_data;
  uint8 tag = QMI_TYPE_REQUIRED_PARAMETERS;
  uint8 sess_type;

  pbm_sp = &qmi_pbm_state;
  sess_type = data_ptr->data.refresh_ind.session_type;

    /* Re-initialize the cl_sp */
  for( j = 0; j < PBMI_MAX_CLIDS; j++)
    {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
    if((cl_sp != NULL) &&
        cl_sp->common.clid != QMI_SVC_CLID_UNUSED &&
         (!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type)))
      {
        ind = NULL;

        len = sizeof(data_ptr->data.refresh_ind.pb_type) + \
              sizeof(data_ptr->data.refresh_ind.session_type) + \
              sizeof(data_ptr->data.refresh_ind.refresh_status);

        ind_data.pb_type = data_ptr->data.refresh_ind.pb_type;
        ind_data.session_type = data_ptr->data.refresh_ind.session_type;
        ind_data.refresh_status = data_ptr->data.refresh_ind.refresh_status;

        /* Mandatory TLV */
        if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&ind_data.refresh_status, sizeof(ind_data.refresh_status))&&
             QMI_SVC_PKT_PUSH(&ind, (void *)&ind_data.pb_type, sizeof(ind_data.pb_type)) &&
             QMI_SVC_PKT_PUSH(&ind, (void *)&ind_data.session_type, sizeof(ind_data.session_type)) &&
             QMI_SVC_PKT_PUSH(&ind, (void *)&len, PBMI_TLV_LEN_SIZE)&&
             QMI_SVC_PKT_PUSH(&ind, (void *)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          continue;
        }
      (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_REFRESH_IND,
                          ind );
        }
      }
} /* qmi_pbmi_refresh_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_ALL_PB_INIT_DONE_IND()

  DESCRIPTION
    Send all phonebook initialization done indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_all_pb_init_done_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                            j,i;
  dsm_item_type *                ind;
  uint16 len = 0, tot_len;
  qmi_pbm_if_all_pb_init_done_ind_s *all_pb_init;
  uint8 tag = QMI_TYPE_REQUIRED_PARAMETERS;
  uint16 pb_mask;
  uint8  sess_type;
  uint8 num_sess = 0;

  all_pb_init = (qmi_pbm_if_all_pb_init_done_ind_s*)&data_ptr->data.all_pb_init_done_ind;

  pbm_sp = &qmi_pbm_state;
  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
    if(cl_sp != NULL && cl_sp->common.clid != QMI_SVC_CLID_UNUSED )
    {
        ind = NULL;

        len = sizeof(all_pb_init->sess_info[0].sess_type) + \
              sizeof(all_pb_init->sess_info[0].pb_mask);
        for(i=0;i<all_pb_init->num_of_sessions;i++)
        {
          pb_mask = all_pb_init->sess_info[i].pb_mask;
          sess_type = all_pb_init->sess_info[i].sess_type;
          if(!cl_sp->pbm_subs_info.is_bind_subs_recv ||
              qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type))
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&pb_mask, sizeof(pb_mask)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&sess_type, sizeof(sess_type)) ) )
            {
              dsm_free_packet(&ind);
              break;
            }
            num_sess++;
          }
        }

        tot_len = sizeof(all_pb_init->num_of_sessions) + \
                  (len * num_sess);
            /* Put the num_of_sessions, total length, Tag id */
        if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_sess,
                                                    sizeof(all_pb_init->num_of_sessions)) &&
             QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, PBMI_TLV_LEN_SIZE)&&
             QMI_SVC_PKT_PUSH(&ind, (void *)&tag, PBMI_TLV_TAG_SIZE)
             ) )
        {
          dsm_free_packet(&ind);
          continue;
        }
        num_sess = 0;
      (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_ALL_PB_INIT_DONE_IND,
                          ind );
    }
  }
} /* qmi_pbmi_all_pb_init_done_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_RECORD_UPDATE_IND()

  DESCRIPTION
    Send Record Update indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_record_update_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  uint16 len = 0;
  uint16 pb_type;
  uint8  sess_type;
  uint8 oper;
  uint16 rec_id;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;

  pbm_sp = &qmi_pbm_state;
  sess_type = data_ptr->data.rec_update_ind.session_type;
  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if((cl_sp != NULL) &&
         (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
         cl_sp->pbm_info.reg_rec_update_events &&
         (!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type)))
      {
        ind = NULL;

        len = sizeof(data_ptr->data.rec_update_ind.session_type) + \
                sizeof(data_ptr->data.rec_update_ind.pb_type) + \
                sizeof(data_ptr->data.rec_update_ind.operation) + \
                sizeof(data_ptr->data.rec_update_ind.record_id);

        /* Mandatory TLV */
        rec_id = data_ptr->data.rec_update_ind.record_id;
        oper = data_ptr->data.rec_update_ind.operation;
        pb_type = data_ptr->data.rec_update_ind.pb_type;


        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec_id, sizeof(rec_id)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&oper, sizeof(oper)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&pb_type, sizeof(pb_type)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&sess_type, sizeof(sess_type))
        ) )
        {
          dsm_free_packet(&ind);
          break;
        }

        if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&len, PBMI_TLV_LEN_SIZE)&&
         QMI_SVC_PKT_PUSH(&ind, (void *)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          continue;
        }

        (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_RECORD_UPDATE_IND,
                          ind );
      }
    }
} /* qmi_pbmi_record_update_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_FILL_ARRAY_TLV()

  DESCRIPTION
    Fill the data for an array type TLV

  PARAMETERS
    itm       : ptr to dsm_item_type ptr
    tag       : Tag type in TLV
    arr_len   : Array length
    instances : Number of instances in the array

  RETURN VALUE
    TRUE if tlv is filled successfully. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbmi_fill_array_tlv
(
  dsm_item_type **itm,
  uint8           tag,
  uint16          arr_len,
  uint8           instances
)
{
  boolean status = TRUE;

  arr_len += sizeof(instances);
  if( !(QMI_SVC_PKT_PUSH(itm, (void*)&instances, sizeof(instances)) &&
        QMI_SVC_PKT_PUSH(itm, (void*)&arr_len, PBMI_TLV_LEN_SIZE) &&
        QMI_SVC_PKT_PUSH(itm, (void*)&tag, PBMI_TLV_TAG_SIZE)) )
  {
    status = FALSE;
  }
  return status;
}


/*===========================================================================
  FUNCTION QMI_PBMI_PB_READY_IND()

  DESCRIPTION
    Send PB Ready indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_pb_ready_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  uint16 len = 0;
  uint16 pb_type;
  uint8  sess_type;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;

  pbm_sp = &qmi_pbm_state;
  sess_type = data_ptr->data.pb_ready_ind.session_type;

  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if(cl_sp != NULL &&
        (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
         cl_sp->pbm_info.reg_pb_ready_events &&
         (!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type)))
      {
        ind = NULL;

        len = sizeof(data_ptr->data.pb_ready_ind.session_type) + \
                sizeof(data_ptr->data.pb_ready_ind.pb_type);

        /* Mandatory TLV */
        pb_type = data_ptr->data.pb_ready_ind.pb_type;

        if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&pb_type, sizeof(pb_type)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&sess_type, sizeof(sess_type))
        ) )
        {
          dsm_free_packet(&ind);
          break;
        }

        if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&len, PBMI_TLV_LEN_SIZE)&&
         QMI_SVC_PKT_PUSH(&ind, (void *)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          continue;
        }

        (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_PB_READY_IND,
                          ind );
      }
    }
} /* qmi_pbmi_pb_ready_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_RECORD_READ_EXT_UNDECODED_IND()

  DESCRIPTION
    Send Record Read ext undecoded indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_record_read_ext_undecoded_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_cmd_val_e_type cmd_type;
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  dsm_item_type *                ind;
  uint16 pb_type, seq_num;
  uint8  sess_type, cl_id;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;
  uint8                   num_recs,  num_instances = 0;
  const qmi_pbm_if_rec_data_undecoded_s   *rec;
  uint16                   tot_len = 0;
  uint8 hidden_tag = PBMI_REC_READ_EXT_UNDECODED_HIDDEN;
  uint8 email_tag = PBMI_REC_READ_EXT_UNDECODED_EMAIL ;
  uint8 ad_num_tag = PBMI_REC_READ_EXT_UNDECODED_AD_NUM;
  uint8 grp_tag = PBMI_REC_READ_EXT_UNDECODED_GRP;
  uint8 sname_tag = PBMI_REC_READ_EXT_UNDECODED_SNAME;
  int i,j,k;
  qmi_pbm_if_record_read_ext_undecoded_ind_s *ind_data = NULL;

  pbm_sp = &qmi_pbm_state;
  ind_data = (qmi_pbm_if_record_read_ext_undecoded_ind_s*)data_ptr->data.rec_read_ext_ind;
  if(ind_data == NULL)
  {
    UIM_MSG_HIGH_0("Record read ext undecoded indication data is null");
    return;
  }
  cmd_type = PBMI_CMD_VAL_READ_RECORDS_EXT_UNDECODED_IND;


  num_recs = ind_data->num_of_recs;
  sess_type = ind_data->sess_type;
  pb_type = ind_data->pb_type;
  seq_num = ind_data->seq_num;
  cl_id = qmi_pbmi_get_rec_read_cl_id( sess_type, pb_type);

  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if(cl_sp != NULL &&
        (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
          (cl_sp->common.clid == cl_id) )
      {
        ind = NULL;

        if(pb_type == ((uint8)QMI_PBM_IF_PB_ADN))
        {
          /* Fill Hidden info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->is_hidden, sizeof(rec->is_hidden)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id)) ) )
            {
              dsm_free_packet(&ind);
              continue;
            }
            tot_len += sizeof(rec->is_hidden) + sizeof(rec->rec_id);
            num_instances++;
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, hidden_tag, tot_len, num_instances ))
            {
              dsm_free_packet(&ind);
            }
          }

          num_instances = 0;
          tot_len = 0;
          /* Fill Email info if present */
          for(i = (num_recs-1);i >= 0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->email_cnt > 0)
            {
              for(k=0;k < rec->email_cnt; k++)
              {
                uint16 email_siz = rec->email_data[k].len + sizeof(rec->email_data[k].len);

                if( !QMI_SVC_PKT_PUSH(&ind, (void*)&rec->email_data[k], email_siz) )
                {
                  dsm_free_packet(&ind);
                  break;
                }
                tot_len += email_siz;
              }
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec->email_cnt, sizeof(rec->email_cnt)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += sizeof(rec->email_cnt) + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, email_tag, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
            }
          }

          num_instances = 0;
          tot_len = 0;
          /* Fill Group ID info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->grp_cnt > 0)
            {
              for(k=0;k < rec->grp_cnt; k++)
              {
                if( !QMI_SVC_PKT_PUSH(&ind, (void*)&rec->grp_id[k], sizeof(rec->grp_id[k])) )
                {
                  dsm_free_packet(&ind);
                  break;
                }
                tot_len += sizeof(rec->grp_id[k]);
              }
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec->grp_cnt, sizeof(rec->grp_cnt)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += sizeof(rec->grp_cnt) + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, grp_tag, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
            }
          }

          num_instances = 0;
          tot_len = 0;
          /* Fill Additional number info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->ad_num_cnt > 0)
            {
              for(k=0;k < rec->ad_num_cnt; k++)
              {
                uint8 tag_siz = sizeof(rec->ad_num_data[k].tag_id);
                uint8 num_siz = sizeof(rec->ad_num_data[k].ad_num.len) + rec->ad_num_data[k].ad_num.len + \
                                sizeof(rec->ad_num_data[k].ad_num.num_plan) + sizeof(rec->ad_num_data[k].ad_num.num_type);

                if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->ad_num_data[k].tag_id, tag_siz) &&
                      QMI_SVC_PKT_PUSH(&ind, (void*)&rec->ad_num_data[k].ad_num, num_siz) ) )
                {
                  dsm_free_packet(&ind);
                  break;
                }
                tot_len += tag_siz + num_siz;
              }
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec->ad_num_cnt, sizeof(rec->ad_num_cnt)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += sizeof(rec->ad_num_cnt) + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, ad_num_tag, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
            }
          }
          num_instances = 0;
          tot_len = 0;
          /* Fill Second name info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->sname_info.len > 0)
            {
              uint16 siz = sizeof(rec->sname_info.len) + rec->sname_info.len;

              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->sname_info, siz) &&
                    QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id)) ) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += siz + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, sname_tag, tot_len, num_instances ))
            {
              dsm_free_packet(&ind);
            }
          }
        }

        num_instances = 0;
        tot_len = 0;
        /* Fill Basic info */
        for(i = (num_recs-1);i >=0;i--)
        {
          rec = &ind_data->rec_data[i];
          if((rec->name_info.len + rec->num_info.len) > 0)
          {
            uint16 name_siz = 0, num_siz = 0;
            name_siz = sizeof(rec->name_info.len) + rec->name_info.len;
            if(data_ptr->header.cmd == QMI_PBM_IF_CMD_RECORD_READ_EXT_IND)
            {
              //for EXT read CMD max length that can be read is QMI_PBM_IF_MAX_TEXT_EXT_LEN * 2
              name_siz += rec->name_info.len;
            }
            num_siz = sizeof(rec->num_info.num_type) + sizeof(rec->num_info.num_plan) + \
                    sizeof(rec->num_info.len) + rec->num_info.len;

            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->name_info, name_siz) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&rec->num_info, num_siz) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))
                ) )
            {
              dsm_free_packet(&ind);
              UIM_MSG_HIGH_0("Cannot send Record read indication No Memory left!");
              QMI_PBM_IF_MEM_FREE(ind_data);
              return;
            }
            tot_len += name_siz + num_siz + sizeof(rec->rec_id);
            num_instances++;
          }
        }
        tot_len += sizeof(num_instances) + sizeof(pb_type) + sizeof(sess_type) + \
                   sizeof(seq_num);
        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&num_instances, sizeof(num_instances)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&pb_type, sizeof(pb_type)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&sess_type, sizeof(sess_type)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&seq_num, sizeof(seq_num)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          UIM_MSG_HIGH_0("Cannot send Record read ext undecoded indication No Memory left!");
          QMI_PBM_IF_MEM_FREE(ind_data);
          return;
        }

        UIM_MSG_HIGH_2("Record read ext undecoded ind for %d recs is sent with start-id %d",
                              ind_data->num_of_recs, ind_data->rec_data[0].rec_id);


        (void)qmi_pbmi_send_indication( cl_sp,
                          cmd_type,
                          ind );

    }
  }
  if(ind_data->seq_num == QMI_PBM_IF_MAX_SEQ_NUM)
  {
    qmi_pbmi_clear_rec_read_clnt_data(sess_type, pb_type);
  }
  QMI_PBM_IF_MEM_FREE(ind_data);
} /* qmi_pbmi_record_read_ext_undecoded_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_SIM_READY_IND()

  DESCRIPTION
    Send PB Ready indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_sim_ready_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  uint16 len = 0;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;
  qmi_pbm_if_session_type sess_type;

  pbm_sp = &qmi_pbm_state;

  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    boolean subs_binded = FALSE;
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
    if(cl_sp != NULL &&
      (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
       cl_sp->pbm_info.reg_pb_ready_events)
    {
      pbm_session_enum_type        pbm_session;
      mmgsdi_slot_id_enum_type     mmgsdi_slot_id;

      for( sess_type = QMI_PBM_IF_SESSION_GW_PRIMARY;sess_type < QMI_PBM_IF_SESSION_MAX; sess_type++)
      {
        pbm_session     = qmi_pbm_if_qmi_session_to_pbm_session(sess_type);
        mmgsdi_slot_id  = (mmgsdi_slot_id_enum_type)pbm_get_slot_info(pbm_session);
        if(data_ptr->data.sim_ready_ind.slot_id == QMI_MMGSDI_SLOT_TO_QMI_SLOT(mmgsdi_slot_id))
        {
          if(!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type))
          {
            subs_binded = TRUE;
            break;
          }
        }
      }

      if(subs_binded)
      {
        ind = NULL;

        len = sizeof(data_ptr->data.sim_ready_ind.slot_id);

        if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sim_ready_ind.slot_id,
                                     sizeof(data_ptr->data.sim_ready_ind.slot_id))) )
        {
          dsm_free_packet(&ind);
          break;
        }

        if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&len, PBMI_TLV_LEN_SIZE)&&
         QMI_SVC_PKT_PUSH(&ind, (void *)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          continue;
        }

        (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_SIM_READY_IND,
                          ind );
      }
    }
  }
} /* qmi_pbmi_sim_ready_ind */


/*===========================================================================
  FUNCTION QMI_PBM_BINDING_TO_CARD_SESSION()

  DESCRIPTION
    Check if the subs id and session type match

  PARAMETERS
    subs_id   :  Subscription id
    sess_type :  Session Type

  RETURN VALUE
    TRUE if matched.
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_pbm_binding_to_card_session(
  uint8 subs_id
)
{
  boolean     is_match = FALSE;

  switch(subs_id) 
  {
    case PBMI_PRIM_SUBS:
    if(((pbm_get_slot_info(PBM_SESSION_LPB_1X)) == 0 &&
       (pbm_get_slot_info(PBM_SESSION_LPB_GW)) == 0))
    {
      is_match = TRUE;
    }
    break;

    case PBMI_SEC_SUBS:
    if(((pbm_get_slot_info(PBM_SESSION_LPB_1X_SECONDARY)) == 0 &&
       (pbm_get_slot_info(PBM_SESSION_LPB_GW_SECONDARY)) == 0))
    {
      is_match = TRUE;
    }
    break;

    case PBMI_TERITARY_SUBS:
    if(((pbm_get_slot_info(PBM_SESSION_LPB_1X_TERTIARY)) == 0 &&
       (pbm_get_slot_info(PBM_SESSION_LPB_GW_TERTIARY)) == 0))
    {
      is_match = TRUE;
    }
    break;

    default:
    break;;
  }

  UIM_MSG_HIGH_2("qmi_pbm_binding_to_card_session: is_match %d SUBS Id %d",
                  is_match, subs_id);

  return is_match;
} /* qmi_pbm_binding_to_card_session */


/*===========================================================================
  FUNCTION QMI_PBMI_EMERGENCY_LIST_IND()

  DESCRIPTION
    Send Emergency List Indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_emergency_list_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                            j,i,k;
  dsm_item_type *                ind;
  uint8 num_of_ota_sessions=0;
  uint8 num_instances = 0;
  uint8 num_of_card_sessions=0,num_of_nv_entries=0,num_of_hcoded_entries=0;
  const qmi_pbm_if_ecc_session_ota_data *rec_ota;
  const qmi_pbm_if_ecc_session_card_data *rec_card;
  const qmi_pbm_if_cat_ecc_data_type *rec_nv;
  const qmi_pbm_if_cat_ecc_data_type *rec_hcoded;
  uint16                   tot_len = 0;

  if( data_ptr->data.emerg_list_ind.emer_data == NULL )
  {
    UIM_MSG_HIGH_0("NULL emergency data in the indication");
    return;
  }

  pbm_sp = &qmi_pbm_state;
  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if(cl_sp != NULL &&
        (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
         cl_sp->pbm_info.reg_emergency_list_events )
      {
        ind = NULL;
        num_of_ota_sessions = data_ptr->data.emerg_list_ind.emer_data->num_of_ota_sessions;
        num_of_card_sessions = data_ptr->data.emerg_list_ind.emer_data->num_of_card_sessions;
        num_of_nv_entries = data_ptr->data.emerg_list_ind.emer_data->num_nv_ecc_entries;
        num_of_hcoded_entries = data_ptr->data.emerg_list_ind.emer_data->num_hcoded_ecc_entries;
        num_instances = 0;
        tot_len = 0;

      UIM_MSG_HIGH_4("qmi_pbmi_emergency_list_ind num of ota_sessions = 0x%x card_sessions = 0x%x nv_entries = 0x%x hcoded_entries = 0x%x",
                      num_of_ota_sessions,
                      num_of_card_sessions,
                      num_of_nv_entries,
                      num_of_hcoded_entries);

        /* Fill OTA Emergency info if present */
        for(i = (num_of_ota_sessions-1);i >=0;i--)
        {
          rec_ota = &data_ptr->data.emerg_list_ind.emer_data->ota_ecc[i];
          if( (!cl_sp->pbm_subs_info.is_bind_subs_recv ||
               qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,rec_ota->sess_type)) &&
              (rec_ota->ecc_count > 0) )
          {
            char ecc_buf_print[CM_MAX_EMERGENCY_NUM_COUNT * (QMI_PBM_IF_MAX_ECC_LEN + 1)];
            memset(ecc_buf_print, 0, sizeof(ecc_buf_print));

            for(k=(rec_ota->ecc_count-1);k >= 0 ; k--)
            {
              uint8 ecc_siz = rec_ota->ecc_info[k].ecc_len + sizeof(rec_ota->ecc_info[k].ecc_len) + sizeof(rec_ota->ecc_info[k].category);
              tot_len += ecc_siz;
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)rec_ota->ecc_info[k].ecc, rec_ota->ecc_info[k].ecc_len) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec_ota->ecc_info[k].ecc_len, sizeof(rec_ota->ecc_info[k].ecc_len)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec_ota->ecc_info[k].category, sizeof(rec_ota->ecc_info[k].category))))
              {
                dsm_free_packet(&ind);
                break;
              }
              qmi_pbmi_add_ecc(rec_ota->ecc_info[k].ecc,rec_ota->ecc_info[k].ecc_len, ecc_buf_print, sizeof(ecc_buf_print) );
            }
            UIM_PBM_MSG_SPRINTF_2(" OTA ecc for session type %d : %s",rec_ota->sess_type, ecc_buf_print);
            tot_len += sizeof(rec_ota->ecc_count) + sizeof(rec_ota->sess_type);
            if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec_ota->ecc_count, sizeof(rec_ota->ecc_count)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void*)&rec_ota->sess_type, sizeof(rec_ota->sess_type))) )
            {
              dsm_free_packet(&ind);
              break;
            }
            num_instances++;
          }
        }
        if(num_instances > 0)
        {
          if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_OTA_EMER_NUM, tot_len, num_instances))
          {
            dsm_free_packet(&ind);
            break;
          }
        }

        /* Fill CARD Emergency info if present */
        num_instances = 0;
        tot_len = 0;

        for(i = (num_of_card_sessions-1);i >=0;i--)
        {
          rec_card = &data_ptr->data.emerg_list_ind.emer_data->card_ecc[i];
          if((!cl_sp->pbm_subs_info.is_bind_subs_recv ||
               qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,rec_card->sess_type) ||
               qmi_pbm_binding_to_card_session (cl_sp->pbm_subs_info.subs_id)) &&
               (rec_card->ecc_count > 0))
          {
            char *ecc_buf_print = QMI_PBM_IF_MEM_ALLOC(rec_card->ecc_count*(QMI_PBM_IF_MAX_ECC_LEN+1));
            if(ecc_buf_print != NULL)
            {
              memset(ecc_buf_print, 0, (rec_card->ecc_count*(QMI_PBM_IF_MAX_ECC_LEN+1)));
            }

            for(k =(rec_card->ecc_count-1);k >= 0 ; k--)
            {
              uint8 ecc_siz = rec_card->ecc_info[k].ecc_len + sizeof(rec_card->ecc_info[k].ecc_len) + sizeof(rec_card->ecc_info[k].category);
              tot_len += ecc_siz;
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)rec_card->ecc_info[k].ecc, rec_card->ecc_info[k].ecc_len) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec_card->ecc_info[k].ecc_len, sizeof(rec_card->ecc_info[k].ecc_len)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec_card->ecc_info[k].category, sizeof(rec_card->ecc_info[k].category))))
              {
                dsm_free_packet(&ind);
                break;
              }
              if(ecc_buf_print != NULL)
              {
                qmi_pbmi_add_ecc(rec_card->ecc_info[k].ecc,rec_card->ecc_info[k].ecc_len, ecc_buf_print ,  rec_card->ecc_count*(QMI_PBM_IF_MAX_ECC_LEN+1));
              }
            }
            if(ecc_buf_print != NULL)
            {
              UIM_PBM_MSG_SPRINTF_2(" Card ecc for session type %d: %s",rec_card->sess_type, ecc_buf_print);
              QMI_PBM_IF_MEM_FREE(ecc_buf_print);
            }
            tot_len += sizeof(rec_card->ecc_count) + sizeof(rec_card->sess_type);
            if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec_card->ecc_count, sizeof(rec_card->ecc_count)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void*)&rec_card->sess_type, sizeof(rec_card->sess_type))) )
            {
              dsm_free_packet(&ind);
              break;
            }
            num_instances++;
          }
        }
        if(num_instances > 0)
        {
          if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_CARD_EMER_NUM, tot_len, num_instances))
          {
            dsm_free_packet(&ind);
            break;
          }
        }

        /* Fill NV Emergency info if present */
        num_instances = 0;
        tot_len = 0;
        if(num_of_nv_entries != 0)
        {
          char ecc_buf_print[PBM_NV_EMERGENCY_NUMBERS*(QMI_PBM_IF_MAX_ECC_LEN+1)];
          memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
          for(i = (num_of_nv_entries-1);i >=0;i--)
          {
            rec_nv = &data_ptr->data.emerg_list_ind.emer_data->nv_ecc[i];
            if(rec_nv->ecc_len > 0)
            {
              uint8 ecc_siz = rec_nv->ecc_len + sizeof(rec_nv->ecc_len);
              tot_len += ecc_siz;
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)rec_nv->ecc, rec_nv->ecc_len) &&
                      QMI_SVC_PKT_PUSH(&ind, (void*)&rec_nv->ecc_len, sizeof(rec_nv->ecc_len))))
              {
                dsm_free_packet(&ind);
                break;
              }
              qmi_pbmi_add_ecc(rec_nv->ecc,rec_nv->ecc_len, ecc_buf_print , sizeof(ecc_buf_print) );
              num_instances++;
            }
          }
          UIM_PBM_MSG_SPRINTF_1(" NV ECC : %s", ecc_buf_print);
          if(num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_NV_EMER_NUM, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
              break;
            }
          }
        }

        /* Fill HARD CODED Emergency info if present */
        num_instances = 0;
        tot_len = 0;
        if(num_of_hcoded_entries != 0)
        {
          char ecc_buf_print[QMI_PBM_IF_MAX_HCODED_ECC*(QMI_PBM_IF_MAX_ECC_LEN+1)];
          if(num_of_hcoded_entries > QMI_PBM_IF_MAX_HCODED_ECC)
            num_of_hcoded_entries = QMI_PBM_IF_MAX_HCODED_ECC;
          memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
          for(i = (num_of_hcoded_entries-1);i >=0;i--)
          {
            rec_hcoded = &data_ptr->data.emerg_list_ind.emer_data->hcoded_ecc[i];
            if(rec_hcoded->ecc_len > 0)
            {
              uint8 ecc_siz = rec_hcoded->ecc_len + sizeof(rec_hcoded->ecc_len);
              tot_len += ecc_siz;
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)rec_hcoded->ecc, rec_hcoded->ecc_len) &&
                       QMI_SVC_PKT_PUSH(&ind, (void*)&rec_hcoded->ecc_len, sizeof(rec_hcoded->ecc_len))))
              {
                dsm_free_packet(&ind);
                break;
              }
              qmi_pbmi_add_ecc(rec_hcoded->ecc,rec_hcoded->ecc_len, ecc_buf_print , sizeof(ecc_buf_print));
              num_instances++;
            }
          }
          UIM_PBM_MSG_SPRINTF_1(" Hardcoded ECC : %s", ecc_buf_print);
        }
        if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_HARDCODED_EMER_NUM, tot_len, num_instances))
        {
          dsm_free_packet(&ind);
          break;
        }

        /* Fill HARD CODED Emergency info if present */
        num_instances = 0;
        tot_len = 0;
        num_of_hcoded_entries = data_ptr->data.emerg_list_ind.emer_data->num_hcoded_ecc_entries;
        if(num_of_hcoded_entries != 0)
        {
          char ecc_buf_print[QMI_PBM_IF_MAX_HCODED_ECC_EXT*(QMI_PBM_IF_MAX_ECC_LEN+1)];
          memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
          for(i = (num_of_hcoded_entries-1);i >=0;i--)
          {
            rec_hcoded = &data_ptr->data.emerg_list_ind.emer_data->hcoded_ecc[i];
            if(rec_hcoded->ecc_len > 0)
            {
              uint8 ecc_siz = rec_hcoded->ecc_len + sizeof(rec_hcoded->ecc_len);
              tot_len += ecc_siz;
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)rec_hcoded->ecc, rec_hcoded->ecc_len) &&
                       QMI_SVC_PKT_PUSH(&ind, (void*)&rec_hcoded->ecc_len, sizeof(rec_hcoded->ecc_len))))
              {
                dsm_free_packet(&ind);
                break;
              }
              qmi_pbmi_add_ecc(rec_hcoded->ecc,rec_hcoded->ecc_len, ecc_buf_print, sizeof(ecc_buf_print) );
              num_instances++;
            }
          }
          UIM_PBM_MSG_SPRINTF_1(" Hardcoded EXT ECC : %s", ecc_buf_print);
        }
        if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_HARDCODED_EMER_NUM_EXT, tot_len, num_instances))
        {
          dsm_free_packet(&ind);
          break;
        }

        /* Fill HARD CODED Emergency info with category if present */
        num_instances = 0;
        tot_len = 0;
        num_of_hcoded_entries = data_ptr->data.emerg_list_ind.emer_data->num_hcoded_ecc_entries;
        if(num_of_hcoded_entries != 0)
        {
          for(i = (num_of_hcoded_entries-1);i >=0;i--)
          {
            rec_hcoded = &data_ptr->data.emerg_list_ind.emer_data->hcoded_ecc[i];
            if(rec_hcoded->ecc_len > 0)
            {
              uint8 ecc_siz =  sizeof(rec_hcoded->category);
              tot_len += ecc_siz;
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec_hcoded->category, sizeof(rec_hcoded->category)) ) )
              {
                dsm_free_packet(&ind);
                break;
              }
              num_instances++;
            }
          }
        }
        if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_HARDCODED_EMER_NUM_EXT_CAT, tot_len, num_instances))
        {
          dsm_free_packet(&ind);
          break;
        }

        /* Fill NV Emergency info if present */
        num_instances = 0;
        tot_len = 0;
        if(num_of_nv_entries != 0)
        {
          char ecc_buf_print[PBM_NV_EMERGENCY_NUMBERS*(QMI_PBM_IF_MAX_ECC_LEN+1)];
          memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
          for(i = (num_of_nv_entries-1);i >=0;i--)
          {
            rec_nv = &data_ptr->data.emerg_list_ind.emer_data->nv_ecc[i];
            if(rec_nv->ecc_len > 0)
            {
              uint8 ecc_siz = rec_nv->ecc_len + sizeof(rec_nv->ecc_len);
              tot_len += ecc_siz;
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)rec_nv->ecc, rec_nv->ecc_len) &&
                      QMI_SVC_PKT_PUSH(&ind, (void*)&rec_nv->ecc_len, sizeof(rec_nv->ecc_len))))
              {
                dsm_free_packet(&ind);
                break;
              }
              qmi_pbmi_add_ecc(rec_nv->ecc,rec_nv->ecc_len, ecc_buf_print , sizeof(ecc_buf_print));
              num_instances++;
            }
          }
          UIM_PBM_MSG_SPRINTF_1(" NV ECC : %s", ecc_buf_print);
          if(num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_NV_EMER_NUM_EXT, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
              break;
            }
          }
        }

	/* Fill NV Emergency info with category if present */
        num_instances = 0;
        tot_len = 0;
        if(num_of_nv_entries == 0)
        {
          UIM_MSG_HIGH_0("NV emergency numbers not present");
        }
        else
        {
          for(i = (num_of_nv_entries-1);i >=0;i--)
          {
            rec_nv = &data_ptr->data.emerg_list_ind.emer_data->nv_ecc[i];
            if(rec_nv->ecc_len > 0)
            {
              uint8 ecc_siz = sizeof(rec_nv->category);
              tot_len += ecc_siz;
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec_nv->category, sizeof(rec_nv->category)) ))
              {
                dsm_free_packet(&ind);
                break;
              }
              num_instances++;
            }
          }
          if(num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_NV_EMER_NUM_EXT_CAT, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
              break;
            }
          }
        }

        (void)qmi_pbmi_send_indication( cl_sp,
                                PBMI_CMD_VAL_EMERGENCY_LIST_IND,
                                ind );
  }
}
} /* qmi_pbmi_emergency_list_ind() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_EMERGENCY_LIST()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_emergency_list
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *               response           = NULL;
  qmi_error_e_type              errval             = QMI_ERR_NONE;
  qmi_result_e_type             result             = QMI_RESULT_SUCCESS;
  boolean                       retval             = TRUE;
  qmi_pbm_if_info_item_s *      pbm_if_data_ptr    = NULL;

#ifdef REMOVE_QMI_PBM_GET_EMERGENCY_LIST_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if(*sdu_in != NULL)
  {
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_EMERGENCY_LIST;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_GET_EMERGENCY_LIST_V01 */

send_result:
  result = QMI_RESULT_FAILURE;

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_emergency_list() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_EMERGENCY_LIST_RESP()

  DESCRIPTION
    Return all the applicable emergency numbers

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_emergency_list_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint8 num_of_ota_sessions=0;
  uint8 num_instances = 0;
  uint8 num_of_card_sessions=0,num_of_nv_entries=0,num_of_hcoded_entries=0;
  const qmi_pbm_if_ecc_session_ota_data *rec_ota;
  const qmi_pbm_if_ecc_session_card_data *rec_card;
  const qmi_pbm_if_cat_ecc_data_type *rec_nv;
  const qmi_pbm_if_cat_ecc_data_type *rec_hcoded;
  uint16                   tot_len = 0;
  int i, k;
  qmi_pbmi_client_state_type * cl_sp;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_get_emergency_list_resp");
    return;
  }

  cl_sp = qmi_pbmi_get_client_sp_by_cmd_buf(cmd_buf_p);

  if(cl_sp == NULL)
  {
    UIM_MSG_HIGH_0("Null client state ptr in qmi_pbmi_get_emergency_list_resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.emerg_list_resp.error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    if(data_ptr->data.emerg_list_resp.emer_data == NULL)
    {
      UIM_MSG_HIGH_0("Null emergency data in qmi_pbmi_get_emergency_list_resp");
      return;
    }
    num_of_ota_sessions = data_ptr->data.emerg_list_resp.emer_data->num_of_ota_sessions;
    num_of_card_sessions = data_ptr->data.emerg_list_resp.emer_data->num_of_card_sessions;
    num_of_nv_entries = data_ptr->data.emerg_list_resp.emer_data->num_nv_ecc_entries;
    num_of_hcoded_entries = data_ptr->data.emerg_list_resp.emer_data->num_hcoded_ecc_entries;

    /* Fill HARD CODED Emergency info if present */
    num_instances = 0;
    tot_len = 0;
    if(num_of_hcoded_entries == 0)
    {
      UIM_MSG_HIGH_0("Hardcoded emergency numbers not present");
    }
    else
    {
      char ecc_buf_print[QMI_PBM_IF_MAX_HCODED_ECC_EXT*(QMI_PBM_IF_MAX_ECC_LEN+1)];
      memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
      for(i = (num_of_hcoded_entries-1);i >=0;i--)
      {
        rec_hcoded = &data_ptr->data.emerg_list_resp.emer_data->hcoded_ecc[i];
        if(rec_hcoded->ecc_len > 0)
        {
          uint8 ecc_siz = rec_hcoded->ecc_len + sizeof(rec_hcoded->ecc_len);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)rec_hcoded->ecc, rec_hcoded->ecc_len) &&
                QMI_SVC_PKT_PUSH(&response, (void*)&rec_hcoded->ecc_len, sizeof(rec_hcoded->ecc_len))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          qmi_pbmi_add_ecc(rec_hcoded->ecc,rec_hcoded->ecc_len, ecc_buf_print , sizeof(ecc_buf_print) );
          num_instances++;
        }
      }
      UIM_PBM_MSG_SPRINTF_1(" Hardcoded ECC : %s", ecc_buf_print);
      if(num_instances > 0)
      {
        if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_HCODED_EXT, tot_len, num_instances))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
        }
      }
    }

	/* Fill HARD CODED Emergency with category  info if present */
    num_instances = 0;
    tot_len = 0;
    if(num_of_hcoded_entries == 0)
    {
      UIM_MSG_HIGH_0("Hardcoded emergency numbers not present");
    }
    else
    {
      for(i = (num_of_hcoded_entries-1);i >=0;i--)
      {
        rec_hcoded = &data_ptr->data.emerg_list_resp.emer_data->hcoded_ecc[i];
        if(rec_hcoded->ecc_len > 0)
        {
          uint8 ecc_siz =  sizeof(rec_hcoded->category);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)&rec_hcoded->category, sizeof(rec_hcoded->category)) ))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          num_instances++;
        }
      }
      if(num_instances > 0)
      {
        if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_HCODED_EXT_CAT, tot_len, num_instances))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
        }
      }
    }

    num_instances = 0;
    tot_len = 0;
    if(num_of_ota_sessions == 0)
    {
      UIM_MSG_HIGH_0("OTA emergency numbers not present");
    }

    /* Fill OTA Emergency info if present */
    for(i = (num_of_ota_sessions-1);i >=0;i--)
    {
      rec_ota = &data_ptr->data.emerg_list_resp.emer_data->ota_ecc[i];
      if((!cl_sp->pbm_subs_info.is_bind_subs_recv ||
               qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,rec_ota->sess_type)) &&
              (rec_ota->ecc_count > 0))
      {
        char ecc_buf_print[CM_MAX_EMERGENCY_NUM_COUNT * (QMI_PBM_IF_MAX_ECC_LEN + 1)];
        memset(ecc_buf_print, 0, sizeof(ecc_buf_print));

        for((k=rec_ota->ecc_count-1);k >= 0 ; k--)
        {
          uint8 ecc_siz = rec_ota->ecc_info[k].ecc_len + sizeof(rec_ota->ecc_info[k].ecc_len) + sizeof(rec_ota->ecc_info[k].category);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)rec_ota->ecc_info[k].ecc, rec_ota->ecc_info[k].ecc_len) &&
                QMI_SVC_PKT_PUSH(&response, (void*)&rec_ota->ecc_info[k].ecc_len, sizeof(rec_ota->ecc_info[k].ecc_len)) &&
                 QMI_SVC_PKT_PUSH(&response, (void*)&rec_ota->ecc_info[k].category, sizeof(rec_ota->ecc_info[k].category))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          qmi_pbmi_add_ecc(rec_ota->ecc_info[k].ecc,rec_ota->ecc_info[k].ecc_len, ecc_buf_print, sizeof(ecc_buf_print) );
        }
        UIM_PBM_MSG_SPRINTF_2(" OTA ecc for session type %d : %s",rec_ota->sess_type, ecc_buf_print);
        tot_len += sizeof(rec_ota->ecc_count) + sizeof(rec_ota->sess_type);
        if( !( QMI_SVC_PKT_PUSH(&response, (void*)&rec_ota->ecc_count, sizeof(rec_ota->ecc_count)) &&
              QMI_SVC_PKT_PUSH(&response, (void*)&rec_ota->sess_type, sizeof(rec_ota->sess_type))) )
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
          goto send_result;
        }
        num_instances++;
      }
    }
    if(num_instances > 0)
    {
      if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_OTA, tot_len, num_instances))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
    }

    /* Fill CARD Emergency info if present */
    num_instances = 0;
    tot_len = 0;
    if(num_of_card_sessions == 0)
    {
      UIM_MSG_HIGH_0("Card emergency numbers not present");
    }

    for(i = (num_of_card_sessions-1);i >=0;i--)
    {
      rec_card = &data_ptr->data.emerg_list_resp.emer_data->card_ecc[i];
      if(rec_card->ecc_count > 0)
      {
        char *ecc_buf_print = QMI_PBM_IF_MEM_ALLOC(rec_card->ecc_count*(QMI_PBM_IF_MAX_ECC_LEN+1));
        if(ecc_buf_print != NULL)
        {
          memset(ecc_buf_print, 0, (rec_card->ecc_count*(QMI_PBM_IF_MAX_ECC_LEN+1)));
        }

        for(k=(rec_card->ecc_count-1);k >= 0 ; k--)
        {
          uint8 ecc_siz = rec_card->ecc_info[k].ecc_len + sizeof(rec_card->ecc_info[k].ecc_len) + sizeof(rec_card->ecc_info[k].category);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)rec_card->ecc_info[k].ecc, rec_card->ecc_info[k].ecc_len) &&
                  QMI_SVC_PKT_PUSH(&response, (void*)&rec_card->ecc_info[k].ecc_len, sizeof(rec_card->ecc_info[k].ecc_len)) &&
                  QMI_SVC_PKT_PUSH(&response, (void*)&rec_card->ecc_info[k].category, sizeof(rec_card->ecc_info[k].category))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            if(ecc_buf_print != NULL)
            {
            QMI_PBM_IF_MEM_FREE(ecc_buf_print);
            }
            goto send_result;
          }
          if(ecc_buf_print != NULL)
          {
            qmi_pbmi_add_ecc(rec_card->ecc_info[k].ecc,rec_card->ecc_info[k].ecc_len, ecc_buf_print ,rec_card->ecc_count*(QMI_PBM_IF_MAX_ECC_LEN+1));
          }
        }
        if(ecc_buf_print != NULL)
        {
          UIM_PBM_MSG_SPRINTF_2(" Card ecc for session type %d: %s",rec_card->sess_type, ecc_buf_print);
          QMI_PBM_IF_MEM_FREE(ecc_buf_print);
        }
        tot_len += sizeof(rec_card->ecc_count) + sizeof(rec_card->sess_type);
        if( !( QMI_SVC_PKT_PUSH(&response, (void*)&rec_card->ecc_count, sizeof(rec_card->ecc_count)) &&
              QMI_SVC_PKT_PUSH(&response, (void*)&rec_card->sess_type, sizeof(rec_card->sess_type))) )
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
          goto send_result;
        }
        num_instances++;
      }
    }
    if(num_instances > 0)
    {
      if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_CARD, tot_len, num_instances))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
    }

    /* Fill NV Emergency info if present */
    num_instances = 0;
    tot_len = 0;
    if(num_of_nv_entries == 0)
    {
      UIM_MSG_HIGH_0("NV emergency numbers not present");
    }
    else
    {
      char ecc_buf_print[PBM_NV_EMERGENCY_NUMBERS*(QMI_PBM_IF_MAX_ECC_LEN+1)];
      memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
      for(i = (num_of_nv_entries-1);i >=0;i--)
      {
        rec_nv = &data_ptr->data.emerg_list_resp.emer_data->nv_ecc[i];
        if(rec_nv->ecc_len > 0)
        {
          uint8 ecc_siz = rec_nv->ecc_len + sizeof(rec_nv->ecc_len);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)rec_nv->ecc, rec_nv->ecc_len) &&
                  QMI_SVC_PKT_PUSH(&response, (void*)&rec_nv->ecc_len, sizeof(rec_nv->ecc_len))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          qmi_pbmi_add_ecc(rec_nv->ecc,rec_nv->ecc_len, ecc_buf_print , sizeof(ecc_buf_print));
          num_instances++;
        }
      }
      UIM_PBM_MSG_SPRINTF_1(" NV ECC : %s", ecc_buf_print);
      if(num_instances > 0)
      {
        if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_NV, tot_len, num_instances))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
          goto send_result;
        }
      }
    }


    /* Fill HARD CODED Emergency info if present */
    num_instances = 0;
    tot_len = 0;
    if(num_of_hcoded_entries == 0)
    {
      UIM_MSG_HIGH_0("Hardcoded emergency numbers not present");
    }
    else
    {
      char ecc_buf_print[QMI_PBM_IF_MAX_HCODED_ECC*(QMI_PBM_IF_MAX_ECC_LEN+1)];
      if(num_of_hcoded_entries > QMI_PBM_IF_MAX_HCODED_ECC)
        num_of_hcoded_entries = QMI_PBM_IF_MAX_HCODED_ECC;
      memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
      for(i = (num_of_hcoded_entries-1);i >=0;i--)
      {
        rec_hcoded = &data_ptr->data.emerg_list_resp.emer_data->hcoded_ecc[i];
        if(rec_hcoded->ecc_len > 0)
        {
          uint8 ecc_siz = rec_hcoded->ecc_len + sizeof(rec_hcoded->ecc_len);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)rec_hcoded->ecc, rec_hcoded->ecc_len) &&
                QMI_SVC_PKT_PUSH(&response, (void*)&rec_hcoded->ecc_len, sizeof(rec_hcoded->ecc_len))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          qmi_pbmi_add_ecc(rec_hcoded->ecc,rec_hcoded->ecc_len, ecc_buf_print , sizeof(ecc_buf_print) );
          num_instances++;
        }
      }
      UIM_PBM_MSG_SPRINTF_1(" Hardcoded ECC : %s", ecc_buf_print);
      if(num_instances > 0)
      {
        if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_HCODED, tot_len, num_instances))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
        }
      }
    }

    /* Fill NV Emergency info if present */
    num_instances = 0;
    tot_len = 0;
    if(num_of_nv_entries == 0)
    {
      UIM_MSG_HIGH_0("NV emergency numbers not present");
  }
    else
    {
      char ecc_buf_print[PBM_NV_EMERGENCY_NUMBERS*(QMI_PBM_IF_MAX_ECC_LEN+1)];
      memset(ecc_buf_print, 0, sizeof(ecc_buf_print));
      for(i = (num_of_nv_entries-1);i >=0;i--)
      {
        rec_nv = &data_ptr->data.emerg_list_resp.emer_data->nv_ecc[i];
        if(rec_nv->ecc_len > 0)
        {
          uint8 ecc_siz = rec_nv->ecc_len + sizeof(rec_nv->ecc_len);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)rec_nv->ecc, rec_nv->ecc_len) &&
                  QMI_SVC_PKT_PUSH(&response, (void*)&rec_nv->ecc_len, sizeof(rec_nv->ecc_len))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          qmi_pbmi_add_ecc(rec_nv->ecc,rec_nv->ecc_len, ecc_buf_print , sizeof(ecc_buf_print) );
          num_instances++;
        }
      }
      UIM_PBM_MSG_SPRINTF_1(" NV ECC : %s", ecc_buf_print);
      if(num_instances > 0)
      {
        if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_NV_EXT, tot_len, num_instances))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
          goto send_result;
        }
      }
    }

	  /* Fill NV Emergency info with category  if present */
    num_instances = 0;
    tot_len = 0;
    if(num_of_nv_entries == 0)
    {
      UIM_MSG_HIGH_0("NV emergency numbers not present");
  }
    else
    {
      for(i = (num_of_nv_entries-1);i >=0;i--)
      {
        rec_nv = &data_ptr->data.emerg_list_resp.emer_data->nv_ecc[i];
        if(rec_nv->ecc_len > 0)
        {
          uint8 ecc_siz = sizeof(rec_nv->category);
          tot_len += ecc_siz;
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)&rec_nv->category , sizeof(rec_nv->category)) ))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          num_instances++;
        }
      }
      if(num_instances > 0)
      {
        if(!qmi_pbmi_fill_array_tlv(&response, PBMI_GET_EMER_NV_EXT_CAT, tot_len, num_instances))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
          goto send_result;
        }
      }
    }
  }

send_result:
  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET EMERGENCY LIST operation");
  }
} /* qmi_pbmi_get_emergency_list_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_PB_STATE()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_pb_state
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *            response          = NULL;
  qmi_error_e_type           errval            = QMI_ERR_NONE;
  qmi_result_e_type          result            = QMI_RESULT_SUCCESS;
  boolean                    retval            = TRUE;
  qmi_pbm_if_info_item_s *   pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_GET_PB_STATE_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                      type              = 0;
  uint16                     len               = 0;
  uint16                     expected_len      = 0;
  boolean                    got_v_in_required = FALSE;
  uint8                      sess_type         = (uint8)QMI_PBM_IF_SESSION_MAX;
  uint16                     pb_type           = (uint16)PBM_GPB;

  UIM_MSG_HIGH_0 ("QMI PBM qmi_pbmi_get_pb_state");

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));


  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(pbm_if_data_ptr->data.get_pb_state.session_type) + \
                       sizeof(pbm_if_data_ptr->data.get_pb_state.pb_type);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &sess_type, sizeof(sess_type)) &&
               QMI_PBMI_PULL(sdu_in, &pb_type, sizeof(pb_type)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        pbm_if_data_ptr->data.get_pb_state.session_type = sess_type;
        pbm_if_data_ptr->data.get_pb_state.pb_type = pb_type;
        break;
      default:
        UIM_MSG_HIGH_1 ("Unrecognized TLV type (%d)",type);
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_PB_STATE;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /*REMOVE_QMI_PBM_GET_PB_STATE_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_pb_state() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_PB_STATE_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_pb_state_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  qmi_pbm_if_get_pb_state_resp_s  *state_rsp;
  uint16 tot_len = 0;
  uint8 tag = PBMI_GET_PB_STATE;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_get_pb_state_resp");
    return;
  }

  state_rsp = (qmi_pbm_if_get_pb_state_resp_s*)&data_ptr->data;

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = state_rsp->error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    tot_len = sizeof(state_rsp->state) + sizeof(state_rsp->pb_type) + sizeof(state_rsp->sess_type);
    if( !( QMI_SVC_PKT_PUSH(&response, (void*)&state_rsp->state, sizeof(state_rsp->state)) &&
           QMI_SVC_PKT_PUSH(&response, (void*)&state_rsp->pb_type, sizeof(state_rsp->pb_type)) &&
           QMI_SVC_PKT_PUSH(&response, (void*)&state_rsp->sess_type, sizeof(state_rsp->sess_type)) &&
           QMI_SVC_PKT_PUSH(&response, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
           QMI_SVC_PKT_PUSH(&response, (void*)&tag, PBMI_TLV_TAG_SIZE)) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }


  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET PB STATE operation");
  }
} /* qmi_pbmi_get_pb_state_resp() */


/*===========================================================================
  FUNCTION QMI_PBMI_RECORD_READ_IND()

  DESCRIPTION
    Send Record Read indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_record_read_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_cmd_val_e_type cmd_type;
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  dsm_item_type *                ind;
  uint16 pb_type, seq_num;
  uint8  sess_type, cl_id;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;
  uint8                   num_recs,  num_instances = 0;
  const qmi_pbm_if_rec_data_s   *rec;
  uint16                   tot_len = 0;
  uint8 hidden_tag = PBMI_REC_READ_HIDDEN;
  uint8 email_tag = PBMI_REC_READ_EMAIL;
  uint8 ad_num_tag = PBMI_REC_READ_AD_NUM;
  uint8 grp_tag = PBMI_REC_READ_GRP;
  uint8 sname_tag = PBMI_REC_READ_SNAME;
  int i,j,k;
  qmi_pbm_if_record_read_ind_s *ind_data = NULL;

  pbm_sp = &qmi_pbm_state;
  ind_data = (qmi_pbm_if_record_read_ind_s*)data_ptr->data.rec_read_ind;
  if(ind_data == NULL)
  {
    UIM_MSG_HIGH_0("Record read Indication data is null");
    return;
  }
  cmd_type = PBMI_CMD_VAL_RECORD_READ_IND;
  if(data_ptr->header.cmd == QMI_PBM_IF_CMD_RECORD_READ_EXT_IND)
  {
    cmd_type = PBMI_CMD_VAL_READ_RECORDS_EXT_IND;
  }
  num_recs = ind_data->num_of_recs;
  sess_type = ind_data->sess_type;
  pb_type = ind_data->pb_type;
  seq_num = ind_data->seq_num;
  cl_id = qmi_pbmi_get_rec_read_cl_id( sess_type, pb_type);

  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if(cl_sp != NULL &&
        (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
          (cl_sp->common.clid == cl_id) )
      {
        ind = NULL;

        if(pb_type == ((uint8)QMI_PBM_IF_PB_ADN))
        {
          /* Fill Hidden info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->is_hidden, sizeof(rec->is_hidden)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id)) ) )
            {
              dsm_free_packet(&ind);
              continue;
            }
            tot_len += sizeof(rec->is_hidden) + sizeof(rec->rec_id);
            num_instances++;
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, hidden_tag, tot_len, num_instances ))
            {
              dsm_free_packet(&ind);
            }
          }

          num_instances = 0;
          tot_len = 0;
          /* Fill Email info if present */
          for(i = (num_recs-1);i >= 0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->email_cnt > 0)
            {
              for(k=0;k < rec->email_cnt; k++)
              {
                uint16 email_siz = rec->email_data[k].len + sizeof(rec->email_data[k].len);
                if(data_ptr->header.cmd == QMI_PBM_IF_CMD_RECORD_READ_EXT_IND)
                {
                  //for EXT read CMD max length that can be read is QMI_PBM_IF_MAX_TEXT_EXT_LEN * 2
                  email_siz += rec->email_data[k].len;
                }
                if( !QMI_SVC_PKT_PUSH(&ind, (void*)&rec->email_data[k], email_siz) )
                {
                  dsm_free_packet(&ind);
                  break;
                }
                tot_len += email_siz;
              }
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec->email_cnt, sizeof(rec->email_cnt)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += sizeof(rec->email_cnt) + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, email_tag, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
            }
          }

          num_instances = 0;
          tot_len = 0;
          /* Fill Group ID info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->grp_cnt > 0)
            {
              for(k=0;k < rec->grp_cnt; k++)
              {
                if( !QMI_SVC_PKT_PUSH(&ind, (void*)&rec->grp_id[k], sizeof(rec->grp_id[k])) )
                {
                  dsm_free_packet(&ind);
                  break;
                }
                tot_len += sizeof(rec->grp_id[k]);
              }
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec->grp_cnt, sizeof(rec->grp_cnt)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += sizeof(rec->grp_cnt) + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, grp_tag, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
            }
          }

          num_instances = 0;
          tot_len = 0;
          /* Fill Additional number info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->ad_num_cnt > 0)
            {
              for(k=0;k < rec->ad_num_cnt; k++)
              {
                uint8 tag_siz = sizeof(rec->ad_num_data[k].tag_id);
                uint8 num_siz = sizeof(rec->ad_num_data[k].ad_num.len) + rec->ad_num_data[k].ad_num.len + \
                                sizeof(rec->ad_num_data[k].ad_num.num_plan) + sizeof(rec->ad_num_data[k].ad_num.num_type);

                if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->ad_num_data[k].tag_id, tag_siz) &&
                      QMI_SVC_PKT_PUSH(&ind, (void*)&rec->ad_num_data[k].ad_num, num_siz) ) )
                {
                  dsm_free_packet(&ind);
                  break;
                }
                tot_len += tag_siz + num_siz;
              }
              if( !( QMI_SVC_PKT_PUSH(&ind, (void*)&rec->ad_num_cnt, sizeof(rec->ad_num_cnt)) &&
                     QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += sizeof(rec->ad_num_cnt) + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, ad_num_tag, tot_len, num_instances))
            {
              dsm_free_packet(&ind);
            }
          }
          num_instances = 0;
          tot_len = 0;
          /* Fill Second name info if present */
          for(i = (num_recs-1);i >=0;i--)
          {
            rec = &ind_data->rec_data[i];
            if(rec->sname_info.len > 0)
            {
              uint16 siz = sizeof(rec->sname_info.len) + rec->sname_info.len;
              if(data_ptr->header.cmd == QMI_PBM_IF_CMD_RECORD_READ_EXT_IND)
              {
                //for EXT read CMD max length that can be read is QMI_PBM_IF_MAX_TEXT_EXT_LEN * 2
                siz += rec->sname_info.len;
              }
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->sname_info, siz) &&
                    QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id)) ) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += siz + sizeof(rec->rec_id);
              num_instances++;
            }
          }
          if( num_instances > 0)
          {
            if(!qmi_pbmi_fill_array_tlv(&ind, sname_tag, tot_len, num_instances ))
            {
              dsm_free_packet(&ind);
            }
          }
        }

        num_instances = 0;
        tot_len = 0;
        /* Fill Basic info */
        for(i = (num_recs-1);i >=0;i--)
        {
          rec = &ind_data->rec_data[i];
          if((rec->name_info.len + rec->num_info.len) > 0)
          {
            uint16 name_siz = 0, num_siz = 0;
            name_siz = sizeof(rec->name_info.len) + rec->name_info.len;
            if(data_ptr->header.cmd == QMI_PBM_IF_CMD_RECORD_READ_EXT_IND)
            {
              //for EXT read CMD max length that can be read is QMI_PBM_IF_MAX_TEXT_EXT_LEN * 2
              name_siz += rec->name_info.len;
            }
            num_siz = sizeof(rec->num_info.num_type) + sizeof(rec->num_info.num_plan) + \
                    sizeof(rec->num_info.len) + rec->num_info.len;

            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&rec->name_info, name_siz) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&rec->num_info, num_siz) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&rec->rec_id, sizeof(rec->rec_id))
                ) )
            {
              dsm_free_packet(&ind);
              UIM_MSG_HIGH_0("Cannot send Record read indication No Memory left!");
              QMI_PBM_IF_MEM_FREE(ind_data);
              return;
            }
            tot_len += name_siz + num_siz + sizeof(rec->rec_id);
            num_instances++;
          }
        }
        tot_len += sizeof(num_instances) + sizeof(pb_type) + sizeof(sess_type) + \
                   sizeof(seq_num);
        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&num_instances, sizeof(num_instances)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&pb_type, sizeof(pb_type)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&sess_type, sizeof(sess_type)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&seq_num, sizeof(seq_num)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          UIM_MSG_HIGH_0("Cannot send Record read indication No Memory left!");
          QMI_PBM_IF_MEM_FREE(ind_data);
          return;
        }

        UIM_MSG_HIGH_2("Record read ind for %d recs is sent with start-id %d",
                              ind_data->num_of_recs, ind_data->rec_data[0].rec_id);


        (void)qmi_pbmi_send_indication( cl_sp,
                          cmd_type,
                          ind );

    }
  }
  if(ind_data->seq_num == QMI_PBM_IF_MAX_SEQ_NUM)
  {
    qmi_pbmi_clear_rec_read_clnt_data(sess_type, pb_type);
  }
  QMI_PBM_IF_MEM_FREE(ind_data);
} /* qmi_pbmi_record_read_ind() */


/*===========================================================================
  FUNCTION QMI_PBMI_PBSET_CAPS_READ_IND()

  DESCRIPTION
    Send Record Read indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_pbset_caps_read_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  dsm_item_type *                ind;
  uint8  sess_type, cl_id;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;
  uint8                   num_fields = 0;
  const qmi_pbm_if_pbset_cap_data_s   *pbset_caps;
  uint16                   tot_len = 0;
  uint16 i,j;
  qmi_pbm_if_pbset_cap_read_ind_s *ind_data = NULL;


  pbm_sp = &qmi_pbm_state;
  ind_data = (qmi_pbm_if_pbset_cap_read_ind_s*)data_ptr->data.rec_read_ind;
  if(ind_data == NULL)
  {
    UIM_MSG_HIGH_0("Record read Indication data is null");
    return;
  }

  sess_type = ind_data->sess_type;
  cl_id = qmi_pbmi_get_rec_read_cl_id( sess_type, QMI_PBM_IF_PB_ADN);

  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if(cl_sp != NULL &&
        (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
          (cl_sp->common.clid == cl_id) )
      {
        ind = NULL;

        /* Fill Hidden info if present */
        for(i = 1;i <= ind_data->num_of_pbsets;i++)
        {
          uint16 l_field = 0;
          num_fields = 0;
          for(l_field = 0; l_field < PBM_MAX_USIM_FIELDS_IN_PB_SET; l_field++)
          {
            pbset_caps = &ind_data->pbset_cap_data[i-1][l_field];
            if(pbset_caps->num_of_records != 0)
            {
              if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&pbset_caps->record_length, sizeof(pbset_caps->record_length)) &&
                    QMI_SVC_PKT_PUSH(&ind, (void*)&pbset_caps->num_free_records, sizeof(pbset_caps->num_free_records)) &&
                    QMI_SVC_PKT_PUSH(&ind, (void*)&pbset_caps->num_of_records, sizeof(pbset_caps->num_of_records)) &&
                    QMI_SVC_PKT_PUSH(&ind, (void*)&pbset_caps->mapping_type, sizeof(pbset_caps->mapping_type)) &&
                    QMI_SVC_PKT_PUSH(&ind, (void*)&pbset_caps->field_id, sizeof(pbset_caps->field_id)) ) )
              {
                dsm_free_packet(&ind);
                continue;
              }
              tot_len += sizeof(pbset_caps->mapping_type) + sizeof(pbset_caps->field_id) +
                         sizeof(pbset_caps->num_of_records) + sizeof(pbset_caps->num_free_records) +
                         sizeof(pbset_caps->record_length);
              num_fields++;
            }
          }
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&num_fields, sizeof(num_fields)) &&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&i, sizeof(i))))
          {
            dsm_free_packet(&ind);
            continue;
          }
          tot_len += sizeof(num_fields) + sizeof(i);
        }

        tot_len += sizeof(ind_data->num_of_pbsets) + sizeof(sess_type) ;
        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&ind_data->num_of_pbsets, sizeof(ind_data->num_of_pbsets)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&sess_type, sizeof(sess_type)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          UIM_MSG_HIGH_0("Cannot send Record read indication No Memory left!");
          QMI_PBM_IF_MEM_FREE(ind_data);
          return;
        }

        UIM_MSG_HIGH_1("PBSET CAP read ind for %d recs ",ind_data->num_of_pbsets);

        (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_PBSET_CAPS_READ_IND,
                          ind );

       }
    }
  if(ind_data->is_last_ind == TRUE)
  {
    qmi_pbmi_clear_rec_read_clnt_data(sess_type, QMI_PBM_IF_PB_ADN);
  }
  QMI_PBM_IF_MEM_FREE(ind_data);
} /* qmi_pbmi_pbset_caps_read_ind() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_ALL_GROUPS()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_all_groups
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *            response          = NULL;
  qmi_error_e_type           errval            = QMI_ERR_NONE;
  qmi_result_e_type          result            = QMI_RESULT_SUCCESS;
  boolean                    retval            = TRUE;
  qmi_pbm_if_info_item_s *   pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_GET_ALL_GROUPS_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  UIM_MSG_HIGH_0 ("QMI PBM qmi_pbmi_get_all_groups");

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if(*sdu_in != NULL)
  {
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_ALL_GROUPS;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_GET_ALL_GROUPS_V01 */

send_result:
  result = QMI_RESULT_FAILURE;

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_all_groups() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_ALL_GROUPS_RESP()

  DESCRIPTION
    Return all the groups of all the sessions

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_all_groups_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  qmi_pbm_if_get_all_grp_resp_s *rsp;
  qmi_pbm_if_session_grps_s *sess;
  uint16 tot_len = 0, g_data_siz;
  uint8 tag = PBMI_GET_ALL_GRP_DATA, i;
  int k;
  qmi_pbmi_client_state_type *cl_sp;
  uint8 num_sess = 0;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_get_all_groups_resp");
    return;
  }

  cl_sp =  qmi_pbmi_get_client_sp_by_cmd_buf(cmd_buf_p);
  if(cl_sp == NULL)
  {
    UIM_MSG_HIGH_0("Null client state ptr in qmi_pbmi_get_all_groups_resp");
    return;
  }


  rsp = (qmi_pbm_if_get_all_grp_resp_s*)data_ptr->data.get_all_grp_resp;

  if(rsp == NULL)
  {
    UIM_MSG_HIGH_0("NULL data for get all groups resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = rsp->error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    for(i=0;i<rsp->num_of_sessions;i++)
    {
      sess = &rsp->sess_grps[i];
      if(!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess->sess_type))
      {
        for(k=sess->grp_cnt-1;k >= 0;k--)
        {
          g_data_siz = sess->grp_data[k].grp_name_len + sizeof(sess->grp_data[k].grp_name_len) + sizeof(sess->grp_data[k].grp_id);
          tot_len += g_data_siz;
          if( !QMI_SVC_PKT_PUSH(&response, (void*)&sess->grp_data[k], g_data_siz) )
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
          }
        }
        if(sess->grp_cnt > 0)
        {
          tot_len += sizeof(sess->grp_cnt) + sizeof(sess->sess_type);
          if( !(QMI_SVC_PKT_PUSH(&response, (void*)&sess->grp_cnt, sizeof(sess->grp_cnt))&&
                QMI_SVC_PKT_PUSH(&response, (void*)&sess->sess_type, sizeof(sess->sess_type))
             ) )
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
          }
          num_sess++;
        }
      }
    }
    if(num_sess > 0)
    {
      tot_len += sizeof(rsp->num_of_sessions);
      if( !( QMI_SVC_PKT_PUSH(&response, (void*)&num_sess, sizeof(rsp->num_of_sessions)) &&
             QMI_SVC_PKT_PUSH(&response, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
             QMI_SVC_PKT_PUSH(&response, (void*)&tag, PBMI_TLV_TAG_SIZE)) )
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
    }
  }


  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET ALL GROUPS operation");
  }

}/* qmi_pbmi_get_all_groups_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_SET_GROUP_INFO()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_set_group_info
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type          *response = NULL;
  qmi_pbm_if_cmd_e_type   cmd      = QMI_PBM_IF_CMD_MAX;

#ifndef REMOVE_QMI_PBM_SET_GROUP_INFO_V01
  cmd = QMI_PBM_IF_CMD_SET_GROUP_INFO;
  UIM_MSG_HIGH_0 ("QMI PBM qmi_pbmi_set_group_info");
#endif /* !REMOVE_QMI_PBM_SET_GROUP_INFO_V01 */
  response = qmi_pbmi_set_group_or_aas_info(sp,cmd_buf_p,cl_sp,sdu_in,cmd);

  return response;
} /* qmi_pbmi_set_group_info() */


/*===========================================================================
  FUNCTION QMI_PBMI_SET_GROUP_INFO_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_set_group_info_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  struct
  {
    uint8 sess_type;
    uint8 g_id;
  }grp_info;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_set_group_info_resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.set_grp_info_resp.error;
  grp_info.sess_type = data_ptr->data.set_grp_info_resp.sess_type;
  grp_info.g_id = data_ptr->data.set_grp_info_resp.grp_id;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      PBMI_SET_GRP_INFO,
                                      sizeof(grp_info),
                                      (void *) &grp_info))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }


  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM SET GROUP INFO operation");
  }

}/* qmi_pbmi_set_group_info_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_ALL_HIDDEN_RECORDS()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_read_all_hidden_records
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *           response          = NULL;
  qmi_error_e_type          errval            = QMI_ERR_NONE;
  qmi_result_e_type         result            = QMI_RESULT_SUCCESS;
  boolean                   retval            = TRUE;
  qmi_pbm_if_info_item_s *  pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_READ_ALL_HIDDEN_RECORDS_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                     type              = 0;
  uint16                    len               = 0;
  boolean                   got_v_in_required = FALSE;
  void *                    value             = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  while (*sdu_in)
  {
    uint16             expected_len =0;
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(pbm_if_data_ptr->data.read_all_hidden_recs);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        value = &pbm_if_data_ptr->data.read_all_hidden_recs;
        got_v_in_required = TRUE;
        break;
      default:
        break;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_ALL_HIDDEN_RECORDS;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_READ_ALL_HIDDEN_RECORDS_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_read_all_hidden_records() */


/*===========================================================================
  FUNCTION QMI_PBMI_READ_ALL_HIDDEN_RECORDS_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_read_all_hidden_records_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p     = NULL;
  dsm_item_type *         response      = NULL;
  qmi_error_e_type        errval;
  uint8                   sess_type     = 0;
  uint16                  pb_type       = 0;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;
  if(cmd_buf_p == NULL)
  {
    UIM_MSG_MED_0("Null cmd_buf ptr in qmi_pbmi_read_all_hidden_records_resp");
    return;
  }

/* Build the response from back to front */
  errval = data_ptr->data.read_all_hidden_recs_resp.error;
  sess_type = data_ptr->data.read_all_hidden_recs_resp.sess_type;
  pb_type = (uint8)QMI_PBM_IF_PB_ADN;

  response = NULL;

  if(QMI_ERR_NONE == errval)
  {
    if(data_ptr->data.read_all_hidden_recs_resp.rec_cnt != 0)
    {
      qmi_pbmi_set_rec_read_clnt_data(sess_type, pb_type, cmd_buf_p->x_p->cl_sp->clid);
    }
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                       PBMI_READ_RECS_RSP_NUM_RECS,
                                       sizeof(data_ptr->data.read_all_hidden_recs_resp.rec_cnt),
                                       (void *) &(data_ptr->data.read_all_hidden_recs_resp.rec_cnt)) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
      goto send_result;
    }
  }

send_result:

  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM READ ALL HIDDEN RECORDS operation");
  }
}/* qmi_pbmi_read_all_hidden_records_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_HIDDEN_RECORD_STATUS_IND()

  DESCRIPTION
    Hidden record status indication

  PARAMETERS
    data_ptr containing hidden record status

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_hidden_record_status_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type          *pbm_sp    = NULL;
  qmi_pbmi_client_state_type   *cl_sp     = NULL;
  int                           j         = 0;
  dsm_item_type                *ind       = NULL;
  uint8                         sess_type = 0;

  pbm_sp = &qmi_pbm_state;
  sess_type = data_ptr->data.hidden_status_ind.sess_type;

  for(j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
    if(cl_sp != NULL &&
       cl_sp->common.clid != QMI_SVC_CLID_UNUSED &&
       cl_sp->pbm_info.reg_hidden_status_events &&
       (!cl_sp->pbm_subs_info.is_bind_subs_recv ||
         qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type)))
    {
      ind = NULL;

      if(FALSE == qmi_svc_put_param_tlv(&ind,
                                        QMI_TYPE_REQUIRED_PARAMETERS,
                                        sizeof(data_ptr->data.hidden_status_ind),
                                        (void *) &data_ptr->data.hidden_status_ind))
      {
        dsm_free_packet(&ind);
        continue;
      }

      (void)qmi_pbmi_send_indication(cl_sp,
                                     PBMI_CMD_VAL_HIDDEN_RECORD_STATUS_IND,
                                     ind);
    }
  }
} /* qmi_pbmi_hidden_record_status_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_NEXT_EMPTY_RECORD_ID()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_next_empty_record_id
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *                         response          = NULL;
  qmi_error_e_type                        errval            = QMI_ERR_NONE;
  qmi_result_e_type                       result            = QMI_RESULT_SUCCESS;
  boolean                                 retval            = TRUE;
  qmi_pbm_if_info_item_s *                pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                                   type              = 0;
  uint16                                  len               = 0;
  uint16                                  expected_len      = 0;
  boolean                                 got_v_in_required = FALSE;
  qmi_pbm_if_get_next_empty_record_id_s * empty_rec         = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if(NULL == pbm_if_data_ptr)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  empty_rec = &pbm_if_data_ptr->data.nxt_empty_rec_id;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    /* special value 0 = variable length or don't care (unrecognzied TLV) */
    expected_len = 0;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(empty_rec->session_type) +
                       sizeof(empty_rec->pb_type) +
                       sizeof(empty_rec->record_id);
        if(expected_len != len)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if (!(QMI_PBMI_PULL(sdu_in, &empty_rec->session_type, sizeof(empty_rec->session_type)) &&
               QMI_PBMI_PULL(sdu_in, &empty_rec->pb_type, sizeof(empty_rec->pb_type)) &&
               QMI_PBMI_PULL(sdu_in, &empty_rec->record_id, sizeof(empty_rec->record_id))))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;

      default:
        UIM_MSG_MED_1("Unrecognized TLV type 0x%x", type);
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_next_empty_record_id */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_NEXT_EMPTY_RECORD_ID_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_next_empty_record_id_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint16                  nxt_empty_rec_id;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_get_next_empty_record_id_resp");
    return;
  }

  nxt_empty_rec_id = data_ptr->data.nxt_empty_rec_id_resp.record_id;

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.nxt_empty_rec_id_resp.error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
      if(FALSE == qmi_svc_put_param_tlv(&response,
                                        PBMI_NEXT_EMPTY_REC_ID,
                                        sizeof(nxt_empty_rec_id),
                                        (void *) &(nxt_empty_rec_id)) )
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
  }

send_result:
  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET NEXT EMPTY RECORD ID operation");
  }
} /* qmi_pbmi_get_next_empty_record_id_resp() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_NEXT_NON_EMPTY_RECORD_ID()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_next_non_empty_record_id
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *                             response          = NULL;
  qmi_error_e_type                            errval            = QMI_ERR_NONE;
  qmi_result_e_type                           result            = QMI_RESULT_SUCCESS;
  boolean                                     retval            = TRUE;
  qmi_pbm_if_info_item_s *                    pbm_if_data_ptr   = NULL;

#ifdef REMOVE_QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else
  uint8                                       type              = 0;
  uint16                                      len               = 0;
  uint16                                      expected_len      = 0;
  boolean                                     got_v_in_required = FALSE;
  qmi_pbm_if_get_next_non_empty_record_id_s * non_empty_rec     = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  non_empty_rec = &pbm_if_data_ptr->data.nxt_non_empty_rec_id;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(non_empty_rec->session_type) + \
                                  sizeof(non_empty_rec->pb_type) + \
                                  sizeof(non_empty_rec->record_id);
        if ( (expected_len != len) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        if ( !(QMI_PBMI_PULL(sdu_in, &non_empty_rec->session_type, sizeof(non_empty_rec->session_type)) &&
                 QMI_PBMI_PULL(sdu_in, &non_empty_rec->pb_type, sizeof(non_empty_rec->pb_type)) &&
                 QMI_PBMI_PULL(sdu_in, &non_empty_rec->record_id, sizeof(non_empty_rec->record_id)) ) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_required = TRUE;
        break;
      default:
        break;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_V01 */

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_next_non_empty_record_id() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_NEXT_NON_EMPTY_RECORD_ID_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_next_non_empty_record_id_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint16                  nxt_non_empty_rec_id;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_get_next_non_empty_record_id_resp");
    return;
  }

  nxt_non_empty_rec_id = data_ptr->data.nxt_non_empty_rec_id_resp.record_id;

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.nxt_non_empty_rec_id_resp.error;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
      if(FALSE == qmi_svc_put_param_tlv(&response,
                                        PBMI_NEXT_NON_EMPTY_REC_ID,
                                        sizeof(nxt_non_empty_rec_id),
                                        (void *) &(nxt_non_empty_rec_id)) )
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
  }

send_result:

  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET NEXT NON EMPTY RECORD ID operation");
  }
} /* qmi_pbmi_get_next_non_empty_record_id_resp() */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_ALL_AAS()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_get_all_aas
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *              response        = NULL;
  qmi_error_e_type             errval          = QMI_ERR_NONE;
  qmi_result_e_type            result          = QMI_RESULT_SUCCESS;
  boolean                      retval          = TRUE;
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr = NULL;

#ifdef REMOVE_QMI_PBM_GET_ALL_AAS_V01
  UIM_MSG_ERR_0("Unrecognized command");
  (void) sp;
  (void) cmd_buf_p;
  (void) cl_sp;
  (void) sdu_in;
  errval = QMI_ERR_INVALID_QMI_CMD;
  goto send_result;
#else

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if(*sdu_in != NULL)
  {
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_ALL_AAS;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB,pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;
#endif /* REMOVE_QMI_PBM_GET_ALL_AAS_V01 */

send_result:
  result = QMI_RESULT_FAILURE;

  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_get_all_aas */


/*===========================================================================
  FUNCTION QMI_PBMI_GET_ALL_AAS_RESP()

  DESCRIPTION
    Return all the aas of all the sessions

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_get_all_aas_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  qmi_pbm_if_get_all_aas_resp_s *rsp;
  qmi_pbm_if_session_aas_s *sess;
  uint16 tot_len = 0, aas_data_siz;
  uint8 tag = PBMI_GET_ALL_AAS_DATA, i;
  int k;
  qmi_pbmi_client_state_type *cl_sp;
  uint8 num_sess = 0;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_get_all_aas_resp");
    return;
  }

  cl_sp =  qmi_pbmi_get_client_sp_by_cmd_buf(cmd_buf_p);
  if(cl_sp == NULL)
  {
    UIM_MSG_HIGH_0("Null client state ptr in qmi_pbmi_get_all_aas_resp");
    return;
  }

  rsp = (qmi_pbm_if_get_all_aas_resp_s*)data_ptr->data.get_all_aas_resp;

  if(rsp == NULL)
  {
    UIM_MSG_HIGH_0("NULL data for get all aas resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = rsp->error;

  response = NULL;

  if(QMI_ERR_NONE == errval)
  {
    for(i = 0; i < rsp->num_of_sessions; i++)
    {
      sess = &rsp->sess_aas[i];
      if(!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id, sess->sess_type))
      {
        for(k = sess->aas_cnt-1; k >= 0; k--)
        {
          aas_data_siz = sess->aas_data[k].aas_len + sizeof(sess->aas_data[k].aas_len) + sizeof(sess->aas_data[k].aas_id);
          tot_len += aas_data_siz;
          if(!QMI_SVC_PKT_PUSH(&response, (void*)&sess->aas_data[k], aas_data_siz))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
          }
        }
        if(sess->aas_cnt > 0)
        {
          tot_len += sizeof(sess->aas_cnt) + sizeof(sess->sess_type);
          if(!(QMI_SVC_PKT_PUSH(&response, (void*)&sess->aas_cnt, sizeof(sess->aas_cnt))&&
                QMI_SVC_PKT_PUSH(&response, (void*)&sess->sess_type, sizeof(sess->sess_type))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
          }
          num_sess++;
        }
      }
    }
    if(num_sess > 0)
    {
      tot_len += sizeof(rsp->num_of_sessions);
      if(!(QMI_SVC_PKT_PUSH(&response, (void*)&num_sess, sizeof(rsp->num_of_sessions)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&tag, PBMI_TLV_TAG_SIZE)))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
    }
  }

  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM GET ALL AAS operation");
  }
}/* qmi_pbmi_get_all_aas_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_SET_AAS()

  DESCRIPTION


  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_set_aas
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type            *response = NULL;
  qmi_pbm_if_cmd_e_type     cmd      = QMI_PBM_IF_CMD_MAX;

#ifndef REMOVE_QMI_PBM_SET_AAS_V01
  cmd = QMI_PBM_IF_CMD_SET_AAS;
  UIM_MSG_HIGH_0 ("QMI PBM qmi_pbmi_set_aas");
#endif /* !REMOVE_QMI_PBM_SET_AAS_V01 */
  response = qmi_pbmi_set_group_or_aas_info(sp,cmd_buf_p,cl_sp,sdu_in,cmd);

  return response;
} /* qmi_pbmi_set_aas */


/*===========================================================================
  FUNCTION QMI_PBMI_SET_AAS_RESP()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_set_aas_resp
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  struct
  {
    uint8 sess_type;
    uint8 aas_id;
  }aas_info;

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in qmi_pbmi_set_aas_resp");
    return;
  }

/*-------------------------------------------------------------------------
  Build the response from back to front
-------------------------------------------------------------------------*/
  errval = data_ptr->data.set_aas_resp.error;
  aas_info.sess_type = data_ptr->data.set_aas_resp.sess_type;
  aas_info.aas_id = data_ptr->data.set_aas_resp.aas_id;

  response = NULL;

  if( QMI_ERR_NONE == errval )
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      PBMI_SET_AAS_INFO,
                                      sizeof(aas_info),
                                      (void *) &aas_info))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if (FALSE == qmi_pbmi_send_response( errval,
                                      (void*)cmd_buf_p,
                                      response,FALSE))
  {
    UIM_MSG_HIGH_0 ("Unable to send response for PBM SET AAS operation");
  }
}/* qmi_pbmi_set_aas_resp */


/*===========================================================================
  FUNCTION QMI_PBMI_AAS_UPDATE_IND()

  DESCRIPTION
    Send AAS Update indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_aas_update_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                             j;
  dsm_item_type *                ind;
  uint8  sess_type;
  uint16 tot_len = 0;
  uint8 aas_len = 0;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;

  pbm_sp = &qmi_pbm_state;
  sess_type = data_ptr->data.aas_update_ind.session_type;
  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if(cl_sp != NULL &&
        (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
         cl_sp->pbm_info.reg_aas_update_events &&
         (!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type)))
      {
        ind = NULL;
        tot_len = data_ptr->data.aas_update_ind.alpha_len;

        if( !QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.aas_update_ind.alpha),
                                data_ptr->data.aas_update_ind.alpha_len))

        {
          dsm_free_packet(&ind);
          return;
        }
        if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_AAS_ALPHA_EXT, tot_len,
                                     (data_ptr->data.aas_update_ind.alpha_len/2)))
        {
          dsm_free_packet(&ind);
          break;
        }
        if(data_ptr->data.aas_update_ind.alpha_len > QMI_PBM_IF_MAX_SIM_NAME_LEN)
        {
          aas_len = QMI_PBM_IF_MAX_SIM_NAME_LEN;
        }
        else
        {
          aas_len = data_ptr->data.aas_update_ind.alpha_len;
        }
        tot_len = sizeof(data_ptr->data.aas_update_ind.session_type) + sizeof(data_ptr->data.aas_update_ind.operation) +
                  sizeof(data_ptr->data.aas_update_ind.aas_id) +
                  //the below have to be uint8 as per spec the length
                  //should be specified in 1 byte
                  sizeof(uint8)+
                  aas_len;
        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.aas_update_ind.alpha),
                                       aas_len) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&aas_len,
                                       sizeof(aas_len)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.aas_update_ind.aas_id),
                                       sizeof(data_ptr->data.aas_update_ind.aas_id)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.aas_update_ind.operation),
                                       sizeof(data_ptr->data.aas_update_ind.operation)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.aas_update_ind.session_type),
                                sizeof(data_ptr->data.aas_update_ind.session_type)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          UIM_MSG_HIGH_0("Cannot send Record read indication No Memory left!");
          return;
        }

        (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_AAS_UPDATE_IND,
                          ind );

    }
  }
} /* qmi_pbmi_aas_update_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_GAS_UPDATE_IND()

  DESCRIPTION
    Send GAS Update indication

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_gas_update_ind
(
  const qmi_pbm_if_info_item_s *const data_ptr
)
{
  qmi_pbmi_state_type *        pbm_sp;
  qmi_pbmi_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  uint8  sess_type;
  uint16 tot_len = 0;
  uint8 gas_len = 0;
  uint8 tag=QMI_TYPE_REQUIRED_PARAMETERS;

  pbm_sp = &qmi_pbm_state;
  sess_type = data_ptr->data.gas_update_ind.session_type;
  for( j = 0; j < PBMI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_pbmi_client_state_type *)pbm_sp->client[j];
      if(cl_sp != NULL &&
        (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
         cl_sp->pbm_info.reg_gas_update_events &&
         (!cl_sp->pbm_subs_info.is_bind_subs_recv ||
          qmi_pbmi_match_subs_session(cl_sp->pbm_subs_info.subs_id,sess_type)))
      {
        ind = NULL;
        tot_len = data_ptr->data.gas_update_ind.grp_name_len;

        if( !QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.gas_update_ind.grp_name),
                                data_ptr->data.gas_update_ind.grp_name_len))

        {
          dsm_free_packet(&ind);
          return;
        }
        if(!qmi_pbmi_fill_array_tlv(&ind, PBMI_GAS_ALPHA_EXT, tot_len,
                                     (data_ptr->data.gas_update_ind.grp_name_len/2)))
        {
          dsm_free_packet(&ind);
          break;
        }

        if(data_ptr->data.gas_update_ind.grp_name_len > QMI_PBM_IF_MAX_SIM_NAME_LEN)
        {
          gas_len = QMI_PBM_IF_MAX_SIM_NAME_LEN;
        }
        else
        {
          gas_len = data_ptr->data.gas_update_ind.grp_name_len;
        }
        tot_len = sizeof(data_ptr->data.gas_update_ind.session_type) + sizeof(data_ptr->data.gas_update_ind.operation) +
                  sizeof(data_ptr->data.gas_update_ind.gas_id) +
                  //the below have to be uint8 as per spec the length
                  //should be specified in 1 byte
                  sizeof(uint8)+
                  gas_len;
        tag=QMI_TYPE_REQUIRED_PARAMETERS;
        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.gas_update_ind.grp_name),
                                       gas_len) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&gas_len,
                                       sizeof(gas_len)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.gas_update_ind.gas_id),
                                       sizeof(data_ptr->data.gas_update_ind.gas_id)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.gas_update_ind.operation),
                                       sizeof(data_ptr->data.gas_update_ind.operation)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&(data_ptr->data.gas_update_ind.session_type),
                                sizeof(data_ptr->data.gas_update_ind.session_type)) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tot_len, PBMI_TLV_LEN_SIZE) &&
              QMI_SVC_PKT_PUSH(&ind, (void*)&tag, PBMI_TLV_TAG_SIZE)
           ) )
        {
          dsm_free_packet(&ind);
          UIM_MSG_HIGH_0("Cannot send Record read indication No Memory left!");
          return;
        }

        (void)qmi_pbmi_send_indication( cl_sp,
                          PBMI_CMD_VAL_GAS_UPDATE_IND,
                          ind );
      }
    }
} /* qmi_pbmi_gas_update_ind */


/*===========================================================================
  FUNCTION QMI_PBMI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client

  PARAMETERS
    cl_sp :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_reset_client
(
  void * cl_sp
)
{
  qmi_pbmi_client_state_type *  cl_sp_ptr = NULL;

  cl_sp_ptr = (qmi_pbmi_client_state_type *)cl_sp;

  ds_qmi_fw_common_reset_client(&cl_sp_ptr->common);

  cl_sp_ptr->pbm_info.reg_rec_update_events     = FALSE;
  cl_sp_ptr->pbm_info.reg_emergency_list_events = FALSE;
  cl_sp_ptr->pbm_info.reg_pb_ready_events       = FALSE;
  cl_sp_ptr->pbm_info.reg_hidden_status_events  = FALSE;
  cl_sp_ptr->pbm_info.reg_aas_update_events     = FALSE;
  cl_sp_ptr->pbm_info.reg_gas_update_events     = FALSE;
  cl_sp_ptr->pbm_subs_info.is_bind_subs_recv    = FALSE;
  cl_sp_ptr->pbm_subs_info.subs_id              = PBMI_INVALID_SUBS;
} /* qmi_pbmi_reset_client() */


/*===========================================================================
  FUNCTION QMI_PBMI_ADD_ECC()

  DESCRIPTION
    Adds the given emergency number to the end of the input buffer.

  PARAMETERS
    ecc :  Emergency number
    len :  Length of the emergency number
    buf :  Buffer to which the ecc has to be added
    dst_buf_max_len : max length of destination buffer

  RETURN VALUE
  None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pbmi_add_ecc
(
  const char   *ecc,
  uint8         len,
  char         *buf,
  uint16        dst_buf_max_len
)
{
  uint8 buf_len = 0;

  if((buf == NULL) || (ecc == NULL))
  {
    UIM_MSG_MED_0("qmi_pbmi_add_ecc: input data NULL");
    return;
  }

  buf_len = strlen(buf);
  if(buf_len < dst_buf_max_len && buf_len > 0)
  {
    buf[buf_len]=',';
    buf_len++;
  }
  buf = buf + buf_len;
  memscpy(buf, dst_buf_max_len-buf_len, ecc, len);
} /* qmi_pbmi_add_ecc */


/*===========================================================================
  FUNCTION QMI_PBM_MMGSDI_SLOT_TO_CARD_SESSION()

  DESCRIPTION
         Function to return the appropriate card session based on slot information
         avaialble.
  PARAMETERS
    slot_id   :  Slot id

  RETURN VALUE
    uint8 (QMI PBM session value)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_pbm_mmgsdi_slot_to_card_session(
  uint16 slot_id
)
{
  uint8       sess_type = 0;

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      sess_type = QMI_PBM_IF_SESSION_GPB_SLOT1;
      break;

    case MMGSDI_SLOT_2:
      sess_type = QMI_PBM_IF_SESSION_GPB_SLOT2;
      break;

    case MMGSDI_SLOT_3:
      sess_type = QMI_PBM_IF_SESSION_GPB_SLOT3;
      break;

    default:
      sess_type = QMI_PBM_IF_SESSION_MAX;
      break;
  }

  UIM_MSG_HIGH_2("mmgsdi_slot_to_card_session: slot_id 0x%x sess_type 0x%x", slot_id, sess_type);
  return sess_type;
}/* qmi_pbm_mmgsdi_slot_to_card_session */


/*===========================================================================
  FUNCTION QMI_PBM_BINDING_SUBS_ID_TO_PBM_SESSION()

  DESCRIPTION
    Check if the subs id and session type match

  PARAMETERS
    subs_id   :  Subscription id
    sess_type :  Session Type

  RETURN VALUE
    TRUE if matched.
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_pbm_binding_subs_id_to_pbm_session(
  uint8  subs_id,
  uint8  sess_type
)
{
  boolean     is_match = FALSE;

  switch(subs_id) 
  {
    case PBMI_PRIM_SUBS:
      if(qmi_pbm_mmgsdi_slot_to_card_session(pbm_get_slot_info(PBM_SESSION_LPB_1X)) == sess_type ||
         qmi_pbm_mmgsdi_slot_to_card_session(pbm_get_slot_info(PBM_SESSION_LPB_GW)) == sess_type)
      {
        is_match = TRUE;
      }
      break;

    case PBMI_SEC_SUBS:
      if(qmi_pbm_mmgsdi_slot_to_card_session(pbm_get_slot_info(PBM_SESSION_LPB_1X_SECONDARY)) == sess_type ||
         qmi_pbm_mmgsdi_slot_to_card_session(pbm_get_slot_info(PBM_SESSION_LPB_GW_SECONDARY)) == sess_type)
      {
        is_match = TRUE;
      }
      break;

    case PBMI_TERITARY_SUBS:
      if(qmi_pbm_mmgsdi_slot_to_card_session(pbm_get_slot_info(PBM_SESSION_LPB_GW_TERTIARY)) == sess_type ||
         qmi_pbm_mmgsdi_slot_to_card_session(pbm_get_slot_info(PBM_SESSION_LPB_1X_TERTIARY)) == sess_type)
      {
        is_match = TRUE;
      }
      break;

    default:
      UIM_MSG_ERR_1("Binding_subs_id_to_pbm_session: invalid subs id 0x%x", subs_id);
      break;
  }

  return is_match;
}/* qmi_pbm_binding_subs_id_to_pbm_session */


/*===========================================================================
  FUNCTION QMI_PBMI_MATCH_SUBS_SESSION()

  DESCRIPTION
    Check if the subs id and session type match

  PARAMETERS
    subs_id   :  Subscription id
    sess_type :  Session Type

  RETURN VALUE
    TRUE if matched.
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_pbmi_match_subs_session
(
  uint8 subs_id,
  uint8 sess_type
)
{
  boolean is_match = FALSE;
  
  switch(sess_type)
  {
    /* Primary prov map to primary binding always */
    case QMI_PBM_IF_SESSION_GW_PRIMARY:
    case QMI_PBM_IF_SESSION_1X_PRIMARY:
      if(subs_id == PBMI_PRIM_SUBS)
      {
        is_match = TRUE;
      }
      break;

    case QMI_PBM_IF_SESSION_GW_SECONDARY:
    case QMI_PBM_IF_SESSION_1X_SECONDARY:
      if(subs_id == PBMI_SEC_SUBS)
      {
        is_match = TRUE;
      }
      break;

#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GW_TERTIARY:
    case QMI_PBM_IF_SESSION_1X_TERTIARY:
      if(subs_id == PBMI_TERITARY_SUBS)
      {
        is_match = TRUE;
      }
      break;
    case QMI_PBM_IF_SESSION_GPB_SLOT3:
#endif
    case QMI_PBM_IF_SESSION_GPB_SLOT2:
    case QMI_PBM_IF_SESSION_GPB_SLOT1:
      is_match = qmi_pbm_binding_subs_id_to_pbm_session(subs_id,sess_type);
      break;

    default:
      break;
  }

  UIM_MSG_HIGH_3("qmi_pbmi_match_subs_session: sess type 0x%x subs_id 0x%x is_match 0x%x",
                  sess_type, subs_id, is_match);
  return is_match;

}/* qmi_pbmi_match_subs_session */


/*===========================================================================
  FUNCTION QMI_PBMI_SET_GROUP_OR_AAS_INFO()

  DESCRIPTION
       Used to set the AAS or GAS based on the input cmd type.

  PARAMETERS
    sp               : service provided state pointer
    cmd_buf_p        : ptr to cmd buffer
    cl_sp            : client state pointer
    sdu_in           : incoming request
    cmd              : Input command

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_pbmi_set_group_or_aas_info
(
  void                      *sp,
  void                      *cmd_buf_p,
  void                      *cl_sp,
  dsm_item_type            **sdu_in,
  qmi_pbm_if_cmd_e_type      cmd
)
{
  dsm_item_type               *response          = NULL;
  qmi_error_e_type             errval            = QMI_ERR_NONE;
  qmi_result_e_type            result            = QMI_RESULT_SUCCESS;
  boolean                      retval            = TRUE;
  uint8                        type              = 0;
  uint16                       len               = 0;
  uint16                       max_len           = 0;
  boolean                      got_v_in_required = FALSE;
  void                        *value             = NULL;
  qmi_pbm_if_info_item_s      *pbm_if_data_ptr   = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if(cmd != QMI_PBM_IF_CMD_SET_GROUP_INFO &&
     cmd != QMI_PBM_IF_CMD_SET_AAS)
  {
    UIM_MSG_ERR_0("Unrecognized command");
    errval = QMI_ERR_INVALID_QMI_CMD;
    goto send_result;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  while (*sdu_in)
  {
    if(!qmi_svc_get_tl(sdu_in, &type, &len))
    {
      continue;
    }

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        if(cmd == QMI_PBM_IF_CMD_SET_GROUP_INFO)
        {
          value = &pbm_if_data_ptr->data.set_grp_info;
          /* grp name len should not be more than 241. subtract 2 from max size */
          max_len = (sizeof(pbm_if_data_ptr->data.set_grp_info)- 2);
        }
        else if(cmd == QMI_PBM_IF_CMD_SET_AAS)
        {
          value = &pbm_if_data_ptr->data.set_aas;
          /* aas name len should not be more than 241. subtract 2 from max size */
          max_len = (sizeof(pbm_if_data_ptr->data.set_aas)- 2);
        }
        got_v_in_required = TRUE;
        break;

      default:
        UIM_MSG_MED_1("Unrecognized TLV type 0x%x",type);
        break;
    }

    if(len > max_len)
    {
      UIM_MSG_HIGH_3 ("len 0x%x > max_len 0x%x for type 0x%x", len, max_len, type);
      len = max_len;
    }

    if(len != dsm_pullup( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if(!got_v_in_required)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  pbm_if_data_ptr->header.cmd    =  cmd;
  pbm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  errval = qmi_pbm_create_and_send_cmd(QMI_PBM_CMD_PBM_IF_CB, pbm_if_data_ptr);
  return QMI_SVC_RESPONSE_PENDING;

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  if(pbm_if_data_ptr != NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
  }

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_pbmi_set_group_or_aas_info */

