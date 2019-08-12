/*===========================================================================

                         Q M I _ P B M _ I F . C

DESCRIPTION

 The Qualcomm Phonebook service PBM interface source file.

EXTERNALIZED FUNCTIONS

  qmi_pbm_if_init()
    Initialize internal data structures.


Copyright (c) 2004-2015 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_if.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/25/16    sp     F3 improvements
11/17/15   stv     Logging improvements
07/13/15    nr     Updating TON/NPI only if we have number info in write req
12/10/14    stv    Send indication with record data for undecoded read request if required
11/07/14    NR     Fixing possible Memory Overread when memcopying the secondary text buffer
11/07/14    NR     Fixing possible Memory Overread when memcopying the ADN number
11/07/14    NR     Fixing possible Memory Overread when memcopying the alpha text buffer
11/06/14    NR     Fixing possible Memory Overread when memcopying the email text buffer
11/06/14    NR     Fixing possible Memory Overread when memcopying the addtional number
11/05/14    NR     Fixing possible Memory Overread when memcopying the aasname buffer
11/05/14    NR     Fixing possible Memory Overread when memcopying the grpname buffer
11/05/14   stv     Return error code for card session in Hidden records read request 
09/23/14    NR     Prepending '+' for ADN contact if the number type is international
09/18/14   stv     Fix for logging strings in F3s 
08/27/14   stv     Return card error correctly when queried for by clients 
07/25/14    NR     Use v2.0 diag macros
05/14/14    stv    add anr_ext mapping as well to map to qmi_pbm usim field
05/13/14    stv    Reset the group counter for each session in get_all_groups handling 
05/01/14    NR     FR 19033: Reduce Code Duplication pbm
04/18/14    stv    Check the incoming AAS/GAS length in set req (KW)
03/19/14    stv    Move F3 print buffer from stack to heap
03/19/14    stv    Removal of duplicate nv/hcc entries in ecc indication
03/03/14    stv    search api cannot provide correct information for search string
                   (name/number) greater than 4 bytes
02/28/14    stv    get_pb_capabilities new tlv addition packing issue
                   addition of encode/decode idl libraries
02/16/14    NR     PBM/QMI_PBM KW Critical warnings
01/22/14    NR     QMI-PBM : Issue with Hiddenkey not displaying name
01/21/14    NR     PBM/QMI_PBM valid_pb_mask is not resetting after REFRESH EVENT
01/17/14    NR     LLVM Compiler warnings
01/09/14    NR     Dereferencing of NULL pointer in qmi_pbm_if.c
12/27/13    NR     Need to free the rec_id_list pointer allocated in function
                   qmi_pbm_if_handle_pb_refresh()
12/19/13    NR     PBM: LLVM compiler warnings
03/26/10    sk     Created Module.
===========================================================================*/

/*===========================================================================

  INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "dsm.h"
#include "pbmlib.h"
#include "fs_public.h"
#include "msg.h"
#include "err.h"
#include "modem_mem.h"
#include "qmi_pbm_if.h"
#include "qmi_pbm_cmd_list.h"
#include <stdarg.h>
#include "IxErrno.h"
#include "mmgsdisessionlib.h"
#include "pbm.h"
#include "qmi_framework.h"
#include "qmi_pbm_svc.h"
#include "qmi_pbm_task_cmd.h"
#include "stringl/stringl.h"
#include "pbmgsdi.h"
#include "uim_msg.h"
/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

#define QMI_PBM_IF_MAX_FIELDS_IN_REC  44
#define QMI_PBM_IF_NUM_TYPE_SIZE      1
#define QMI_PBM_IF_NUM_PLAN_SIZE      1
#define QMI_PBM_IF_TON_NPI_SIZE       1
#define QMI_PBM_IF_GRP_ID_SIZE        2
#define QMI_PBM_IF_HIDDEN_FIELD_SIZE  1
#define QMI_PBM_IF_GET_FIRST_AVAILABLE_ID 0
#define QMI_PBM_IF_HIDDEN_RECORDS_VALID 1
#define QMI_PBM_IF_HIDDEN_RECORDS_INVALID 2
#define QMI_PBM_IF_MAX_ANR_SUB_FIELDS  3
#define QMI_PBM_IF_ANR_TYPE_SIZE      1
#define QMI_PBM_CLEAR_BIT(val,pos) ((val) &= ~(pos))


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/


qmi_pbm_info_type pbm_if_info;


/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/* ---PBM Interface wrapper functions--- */
static void qmi_pbm_if_indication_register
(
  qmi_pbm_if_info_item_s *const params_ptr
);
static uint16 qmi_pbm_if_fetch_alpha_string
(
  pbm_record_id_type  pbm_rec_id,
  uint8              *alpha_buf,
  int                 alpha_buf_len
);

qmi_pbm_if_ecc_list_type* qmi_pbm_populate_ecc_list(void);
/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_PBM_IF_INIT()

  DESCRIPTION
    Initialize qmi pbm interface

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_init
(
  void
)
{
  pbm_return_type       ret = PBM_ERROR;
  uint8                 i   = 0;

  qmi_pbm_set_cmd_handler(QMI_PBM_CMD_PBM_EVT_CB,qmi_pbm_if_process_cmd);

  qmi_pbm_set_cmd_handler(QMI_PBM_CMD_PBM_IF_CB,qmi_pbm_if_process_cmd);

  /* Register for pbm events */
  ret = pbm_notify_register(qmi_pbm_if_event_callback, NULL);
  if(ret != PBM_SUCCESS)
  {
    UIM_MSG_ERR_1("Registration with PBM Failed with ret = 0x%x",ret);
  }

  /* Initialize the pbm information stored locally */
  memset(&pbm_if_info, 0, sizeof(pbm_if_info));

  for( i = 0; i < QMI_PBM_IF_MAX_SESSIONS; i++)
  {
    pbm_if_info.sess_data[i].sess_type = QMI_PBM_IF_SESSION_MAX;
  }

  /* Initialize the command list */
  qmi_pbm_cmd_list_init();
} /* qmi_pbm_if_init() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_SEND_RESPONSE()

  DESCRIPTION
    Invoke the response from Pbm service.

  PARAMETERS
    handle                     : Holds the address of qmi_cmd_buf_type ptr
                                 (Uniquely identifies a command.)
    cmd                        : PBM command identifier
    err_val                    : Unique PBM Interface error
    rsp                        : Response data
    rsp_len                    : Response length
    rem_entry_from_cmd_list    : To decide if cmdlist entry is to be removed

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_send_response
(
  uint32                      handle,
  uint16                      cmd,
  qmi_error_e_type            err_val,
  void*                       rsp,
  size_t                      rsp_len,
  boolean                     rem_entry_from_cmd_list
)
{
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr;

  UIM_MSG_HIGH_2 ("QMI PBM qmi_pbm_if_send_response cmd %d handle %lu",cmd,handle);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    return;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if ( rem_entry_from_cmd_list )
  {
    /* check if entry is found in cmd list */
    if ( qmi_pbm_cmd_list_free( handle ) != E_SUCCESS )
    {
      UIM_MSG_ERR_0("Entry not found in cmd list ");
    }
  }

  /* Fill the response params */

  /* Fill the common data */
  pbm_if_data_ptr->header.handle = handle;

  switch(cmd)
  {
    case QMI_PBM_IF_CMD_GET_PB_CAPABILITIES:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_PB_CAPABILITIES_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.get_pb_caps_resp, sizeof(qmi_pbm_if_get_pb_capabilities_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.get_pb_caps_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.get_all_pb_caps_resp, sizeof(qmi_pbm_if_get_all_pb_capabilities_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.get_all_pb_caps_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_READ_RECORDS:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_RECORDS_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.read_recs_resp, sizeof(qmi_pbm_if_read_records_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.read_recs_resp.error = err_val;
      }
      break;
    case QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED :
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.read_recs_ext_undecoded_resp, sizeof(qmi_pbm_if_read_records_ext_undecoded_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.read_recs_ext_undecoded_resp.error = err_val;
      }
      break;
   case QMI_PBM_IF_CMD_READ_PBSET_CAPS:
     pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_PBSET_CAPS_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.read_pbset_caps_resp, sizeof(qmi_pbm_if_read_pbset_caps_resp_s), rsp, rsp_len);
      }
      else
      {
       pbm_if_data_ptr->data.read_pbset_caps_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_WRITE_RECORD:
    case QMI_PBM_IF_CMD_WRITE_RECORD_EXT:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_WRITE_RECORD_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.write_rec_resp, sizeof(qmi_pbm_if_write_record_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.write_rec_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_DELETE_RECORD:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_DELETE_RECORD_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.delete_rec_resp, sizeof(qmi_pbm_if_delete_record_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.delete_rec_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS_RESP;
      pbm_if_data_ptr->data.delete_all_pb_recs_resp.error = err_val;
      break;

    case QMI_PBM_IF_CMD_GET_EMERGENCY_LIST:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_EMERGENCY_LIST_RESP;
      pbm_if_data_ptr->data.emerg_list_resp.error = err_val;
      if(QMI_ERR_NONE == err_val)
      {
        pbm_if_data_ptr->data.emerg_list_resp.emer_data = rsp ;
      }
      break;

    case QMI_PBM_IF_CMD_GET_PB_STATE:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_PB_STATE_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.get_pb_state_resp, sizeof(qmi_pbm_if_get_pb_state_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.get_pb_state_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_SET_GROUP_INFO:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_SET_GROUP_INFO_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.set_grp_info_resp, sizeof(qmi_pbm_if_set_grp_info_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.set_grp_info_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_READ_ALL_HIDDEN_RECORDS:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_READ_ALL_HIDDEN_RECORDS_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.read_all_hidden_recs_resp, sizeof(qmi_pbm_if_read_all_hidden_recs_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.read_all_hidden_recs_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.nxt_empty_rec_id_resp, sizeof(qmi_pbm_if_get_next_empty_record_id_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.nxt_empty_rec_id_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.nxt_non_empty_rec_id_resp, sizeof(qmi_pbm_if_get_next_non_empty_record_id_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.nxt_non_empty_rec_id_resp.error = err_val;
      }
      break;

    case QMI_PBM_IF_CMD_SET_AAS:
      pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_SET_AAS_RESP;
      if(QMI_ERR_NONE == err_val && rsp_len > 0)
      {
        memscpy(&pbm_if_data_ptr->data.set_aas_resp, sizeof(qmi_pbm_if_set_aas_resp_s), rsp, rsp_len);
      }
      else
      {
        pbm_if_data_ptr->data.set_aas_resp.error = err_val;
      }
      break;

    default:
      UIM_MSG_ERR_1(" qmi_pbm_if_response_error: Invalid cmd %d", cmd);
      QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      return;
  } /* switch */

  qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
  return;
}


/*===========================================================================
  FUNCTION QMI_PBM_IF_SEND_IND()

  DESCRIPTION
    Send the command to PBM service for indication to be sent
    to the control point.

  PARAMETERS
    ind                        : Uniquely identifies a Indication
    ind_data                   : Ptr to indication data sent to control point
    ind_data_len               : Length of the indication data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_send_ind
(
  qmi_pbm_if_cmd_e_type      ind_type,
  void                      *ind_data,
  int                        ind_data_len
)
{
  qmi_pbm_cmd_enum_type dcc_cmd = QMI_PBM_CMD_PBM_CB;
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr;

  UIM_MSG_HIGH_3("qmi_pbm_if_send_ind: indication 0x%x is ind_data : 0x%x ind_data_len 0x%x",
                  ind_type,
                  ind_data,
                  ind_data_len);

  if(NULL == ind_data || ind_data_len <= 0)
  {
    return;
  }

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    return;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  /* Fill common data */
  pbm_if_data_ptr->header.cmd = ind_type;

  switch(ind_type)
  {
    case QMI_PBM_IF_CMD_REFRESH_IND:
      memscpy(&pbm_if_data_ptr->data.refresh_ind, sizeof(qmi_pbm_if_refresh_ind_s), ind_data, ind_data_len);
      break;
    case QMI_PBM_IF_CMD_ALL_PB_INIT_DONE_IND:
      memscpy(&pbm_if_data_ptr->data.all_pb_init_done_ind, sizeof(qmi_pbm_if_all_pb_init_done_ind_s), ind_data, ind_data_len);
      break;
    case QMI_PBM_IF_CMD_RECORD_UPDATE_IND:
      memscpy(&pbm_if_data_ptr->data.rec_update_ind, sizeof(qmi_pbm_if_record_update_ind_s), ind_data, ind_data_len);
      break;
    case QMI_PBM_IF_CMD_PB_READY_IND:
      memscpy(&pbm_if_data_ptr->data.pb_ready_ind, sizeof(qmi_pbm_if_pb_ready_ind_s), ind_data, ind_data_len);
      break;
    case QMI_PBM_IF_CMD_SIM_READY_IND:
      memscpy(&pbm_if_data_ptr->data.sim_ready_ind, sizeof(qmi_pbm_if_sim_ready_ind_s), ind_data, ind_data_len);
      break;
    case QMI_PBM_IF_CMD_EMERGENCY_LIST_IND:
      pbm_if_data_ptr->data.emerg_list_ind.emer_data = ind_data;
      break;
    case QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND:
    case QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND:
      memscpy(&pbm_if_data_ptr->data.intr_rec_read_ind, sizeof(qmi_pbm_if_internal_record_read_ind_s), ind_data, ind_data_len);
      dcc_cmd = QMI_PBM_CMD_PBM_IF_CB;
      break;
    case QMI_PBM_IF_CMD_INTERNAL_PBSET_CAPS_READ_IND:
      memscpy(&pbm_if_data_ptr->data.intr_pbset_cap_read_ind, sizeof(qmi_pbm_if_internal_pbset_caps_read_ind_s), ind_data, ind_data_len);
      dcc_cmd = QMI_PBM_CMD_PBM_IF_CB;
      break;
    case QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND:
    case QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND:
      memscpy(&pbm_if_data_ptr->data.intr_hidd_rec_ind, sizeof(qmi_pbm_if_internal_hidden_recs_read_ind_s), ind_data, ind_data_len);
      dcc_cmd = QMI_PBM_CMD_PBM_IF_CB;
      break;
    case QMI_PBM_IF_CMD_INTERNAL_READ_RECORDS_EXT_UNDECODED_IND:
      memscpy(&pbm_if_data_ptr->data.intr_rec_read_ext_undecoded_ind,
              sizeof(qmi_pbm_if_internal_record_read_ext_undecoded_ind_s),
              ind_data,
              ind_data_len);
      dcc_cmd = QMI_PBM_CMD_PBM_IF_CB;
      break;
    case QMI_PBM_IF_CMD_HIDDEN_RECORD_STATUS_IND:
      memscpy(&pbm_if_data_ptr->data.hidden_status_ind, sizeof(qmi_pbm_if_hidden_record_status_ind_s), ind_data, ind_data_len);
      break;
    case QMI_PBM_IF_CMD_GAS_UPDATE_IND:
      memscpy(&pbm_if_data_ptr->data.gas_update_ind, sizeof(qmi_pbm_if_gas_update_ind_s), ind_data, ind_data_len);
      break;
    case QMI_PBM_IF_CMD_AAS_UPDATE_IND:
      memscpy(&pbm_if_data_ptr->data.aas_update_ind, sizeof(qmi_pbm_if_aas_update_ind_s), ind_data, ind_data_len);
      break;
    default:
      QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      return;
  }
  if(dcc_cmd == QMI_PBM_CMD_PBM_IF_CB)
  {
    qmi_pbm_cmd_data_buf_type *cmd_ptr = NULL;
    cmd_ptr = qmi_pbm_get_cmd_data_buf();
    if(NULL == cmd_ptr)
    {
      QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      return;
  }

    cmd_ptr->cmd.pbm_if_data_ptr = pbm_if_data_ptr;
    cmd_ptr->cmd.pbm_if_data_ptr->header.handle = 0;
    qmi_pbm_send_cmd(QMI_PBM_CMD_PBM_IF_CB, cmd_ptr);
  }
  else
  {
    qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
  }
} /* qmi_pbm_if_send_ind */


/*===========================================================================
  FUNCTION QMI_PBM_IF_MAP_PBM_ERR_TO_QMI()

  DESCRIPTION
    Map the AMSS PBM error to the QMI error.

  PARAMETERS
    pbm_err        : AMSS PBM error

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_pbm_if_map_pbm_err_to_qmi
(
  pbm_return_type pbm_err
)
{
  qmi_error_e_type qmi_err;

  switch(pbm_err)
  {
    case PBM_SUCCESS:
     qmi_err = QMI_ERR_NONE;
     break;
    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_NOT_FOUND:
     qmi_err = QMI_ERR_INVALID_ID;
     break;
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_SIM_FAIL:
     qmi_err = QMI_ERR_NO_SIM;
     break;
    case PBM_ERROR_NOT_READY:
     qmi_err = QMI_ERR_PB_NOT_READY;
     break;
    case PBM_ERROR_SIM_PIN_REQ:
      qmi_err = QMI_ERR_PIN_RESTRICTION;
      break;
    case PBM_ERROR_SIM_MEMORY_PROBLEM:
      qmi_err = QMI_ERR_DEVICE_MEMORY_ERROR;
      break;
    case PBM_ERROR_PIN2_REQ:
      qmi_err = QMI_ERR_PIN2_RESTRICTION;
      break;
    case PBM_ERROR_SIM_PUC_REQ:
      qmi_err = QMI_ERR_PUK_RESTRICTION;
      break;
    case PBM_ERROR_PUC2_REQ:
      qmi_err = QMI_ERR_PUK2_RESTRICTION;
      break;
    case PBM_ERROR_RESTRICTED:
      qmi_err = QMI_ERR_PB_ACCESS_RESTRICTED;
      break;
    case PBM_ERROR_TEXT_TOO_LONG:
      qmi_err = QMI_ERR_PB_TEXT_TOO_LONG;
      break;
    case PBM_ERROR_NUMBER_TOO_LONG:
      qmi_err = QMI_ERR_PB_NUMBER_TOO_LONG;
      break;
    case PBM_ERROR_NOT_AVAILABLE:
      qmi_err = QMI_ERR_PB_NOT_AVAILABLE;
      break;
    case PBM_ERROR_INVALID_REC:
    default:
      qmi_err = QMI_ERR_INTERNAL;
      break;
  }
  return qmi_err;
} /* qmi_pbm_if_map_pbm_err_to_qmi */

/*===========================================================================
  FUNCTION QMI_PBM_IF_MAP_SLOT_TO_QMI()

  DESCRIPTION
    Map the number plan to the qmi number plan

  PARAMETERS
    npi        : Number plan as per 24.008

  RETURN VALUE
    QMI Number plan value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_pbm_slot_type qmi_pbm_if_map_slot_to_qmi
(
  pbm_slot_type amss_slot
)
{
  qmi_pbm_slot_type slot = QMI_PBM_SLOT_NONE;

  switch(amss_slot)
  {
    case PBM_SLOT_1:
      slot = QMI_PBM_SLOT_1;
      break;
    case PBM_SLOT_2:
      slot = QMI_PBM_SLOT_2;
      break;
#ifdef FEATURE_TRIPLE_SIM
    case PBM_SLOT_3:
      slot = QMI_PBM_SLOT_3;
      break;
#endif
    default:
      break;
  }
  return (slot);
} /* qmi_pbm_if_map_slot_to_qmi */


/*===========================================================================
  FUNCTION QMI_PBM_IF_MAP_NPI_TO_QMI()

  DESCRIPTION
    Map the number plan to the qmi number plan

  PARAMETERS
    npi        : Number plan as per 24.008

  RETURN VALUE
    QMI Number plan value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_pbm_if_map_npi_to_qmi
(
  qmi_pbm_if_amss_npi_e_type npi
)
{
  qmi_pbm_if_num_plan_e_type plan = QMI_PBM_IF_NUM_PLAN_UNKNOWN;

  switch(npi)
  {
    case QMI_PBM_IF_AMSS_NPI_UNKNOWN:
      plan = QMI_PBM_IF_NUM_PLAN_UNKNOWN;
      break;
    case QMI_PBM_IF_AMSS_NPI_ISDN:
      plan = QMI_PBM_IF_NUM_PLAN_ISDN;
      break;
    case QMI_PBM_IF_AMSS_NPI_DATA:
      plan = QMI_PBM_IF_NUM_PLAN_DATA;
      break;
    case QMI_PBM_IF_AMSS_NPI_TELEX:
      plan = QMI_PBM_IF_NUM_PLAN_TELEX;
      break;
    case QMI_PBM_IF_AMSS_NPI_NATIONAL:
      plan = QMI_PBM_IF_NUM_PLAN_NATIONAL;
      break;
    case QMI_PBM_IF_AMSS_NPI_PRIVATE:
      plan = QMI_PBM_IF_NUM_PLAN_PRIVATE;
      break;
    default:
      break;
  }
  return ((uint8)plan);
} /* qmi_pbm_if_map_npi_to_qmi */

qmi_pbm_if_protection_e_type qmi_pbm_if_map_protection_to_qmi
(
  mmgsdi_file_security_enum_type pbm_security_method
)
{
  qmi_pbm_if_protection_e_type qmi_protection_method = QMI_PBM_IF_ALLOWED;

  switch(pbm_security_method)
  {
    case MMGSDI_ALWAYS_ALLOWED:
      qmi_protection_method = QMI_PBM_IF_ALLOWED;
      break;
    case MMGSDI_NEVER_ALLOWED:
      qmi_protection_method = QMI_PBM_IF_NEVER_ALLOWED;
      break;
    case MMGSDI_AND_ALLOWED:
      qmi_protection_method = QMI_PBM_IF_AND_ALLOWED;
      break;
    case MMGSDI_OR_ALLOWED:
      qmi_protection_method = QMI_PBM_IF_OR_ALLOWED;
      break;
    case MMGSDI_SINGLE_ALLOWED:
      qmi_protection_method = QMI_PBM_IF_SINGLE_ALLOWED;
      break;
    default:
      break;
  }
  return (qmi_protection_method);
} /* qmi_pbm_if_map_npi_to_qmi */


uint32 qmi_pbm_if_map_protection_mask_to_qmi
(
  uint32 pbm_pin_mask
)
{
  uint32 qmi_pin_protection_mask = 0x00;
  uint8 i = 0;

  for(i=0; i<PBM_PIN_RESERVED; i++)
  {
    switch(pbm_pin_mask & (1 << i))
    {
      case (1 << PBM_PIN_PIN1):
        qmi_pin_protection_mask = qmi_pin_protection_mask | QMI_PBM_IF_CARD_PIN1;
        break;
      case (1 << PBM_PIN_PIN2):
        qmi_pin_protection_mask = qmi_pin_protection_mask | QMI_PBM_IF_CARD_PIN2;
        break;
      case (1 << PBM_PIN_ADM):
        qmi_pin_protection_mask = qmi_pin_protection_mask | QMI_PBM_IF_CARD_ADM1;
        break;
      default:
        UIM_MSG_HIGH_1("PIN type (%d) not supported ",(qmi_pin_protection_mask & (1 << i)));
    }
  }

  return qmi_pin_protection_mask;
} /* qmi_pbm_if_map_npi_to_qmi */


/*===========================================================================
  FUNCTION QMI_PBM_IF_MAP_QMI_NPI_TO_PBM()

  DESCRIPTION
    Map the qmi number plan to the pbm number plan

  PARAMETERS
    npi        : Number plan as per 24.008

  RETURN VALUE
    pbm Number plan value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_pbm_if_amss_npi_e_type qmi_pbm_if_map_qmi_npi_to_pbm
(
  qmi_pbm_if_num_plan_e_type npi
)
{
  qmi_pbm_if_amss_npi_e_type plan = QMI_PBM_IF_AMSS_NPI_UNKNOWN;

  switch(npi)
  {
    case QMI_PBM_IF_NUM_PLAN_ISDN:
      plan = QMI_PBM_IF_AMSS_NPI_ISDN;
      break;
    case QMI_PBM_IF_NUM_PLAN_DATA:
      plan = QMI_PBM_IF_AMSS_NPI_DATA;
      break;
    case QMI_PBM_IF_NUM_PLAN_TELEX:
      plan = QMI_PBM_IF_AMSS_NPI_TELEX;
      break;
    case QMI_PBM_IF_NUM_PLAN_NATIONAL:
      plan = QMI_PBM_IF_AMSS_NPI_NATIONAL;
      break;
    case QMI_PBM_IF_NUM_PLAN_PRIVATE:
      plan = QMI_PBM_IF_AMSS_NPI_PRIVATE;
      break;
    default:
      break;
  }
  return plan;
} /* qmi_pbm_if_map_qmi_npi_to_pbm */


/*===========================================================================
  FUNCTION QMI_PBM_IF_IS_SESSION_TYPE_VALID()

  DESCRIPTION
    Check if the session type is valid.

  PARAMETERS
    sess_type        : PBM Session Type

  RETURN VALUE
    TRUE if Session is valid FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_if_is_session_type_valid
(
  uint8 sess_type
)
{
  switch (sess_type)
  {
    case QMI_PBM_IF_SESSION_GW_PRIMARY:
    case QMI_PBM_IF_SESSION_1X_PRIMARY:
    case QMI_PBM_IF_SESSION_GW_SECONDARY:
    case QMI_PBM_IF_SESSION_1X_SECONDARY:
    case QMI_PBM_IF_SESSION_GPB_SLOT1:
    case QMI_PBM_IF_SESSION_GPB_SLOT2:
#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GW_TERTIARY:
    case QMI_PBM_IF_SESSION_1X_TERTIARY:
    case QMI_PBM_IF_SESSION_GPB_SLOT3:
#endif
      return TRUE;
    default:
      return FALSE;
  }
} /* qmi_pbm_if_is_session_type_valid */


/*===========================================================================
  FUNCTION QMI_PBM_IF_IS_PB_TYPE_VALID()

  DESCRIPTION
    Check if the phonebook type is valid.

  PARAMETERS
    pb_type        : PBM PB Type

  RETURN VALUE
    TRUE if pb is valid FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_if_is_pb_type_valid
(
  uint16 pb_type
)
{
  switch (pb_type)
  {
    case QMI_PBM_IF_PB_ADN:
    case QMI_PBM_IF_PB_FDN:
    case QMI_PBM_IF_PB_MSISDN:
    case QMI_PBM_IF_PB_MBDN:
    case QMI_PBM_IF_PB_MBN:
    case QMI_PBM_IF_PB_SDN:
    case QMI_PBM_IF_PB_BDN:
    case QMI_PBM_IF_PB_LND:
    case QMI_PBM_IF_PB_GAS:
    case QMI_PBM_IF_PB_AAS:
    case QMI_PBM_DEVICE_TYPE_ALL:
      return TRUE;
    default:
      return FALSE;
  }
} /* qmi_pbm_if_is_pb_type_valid */


/*===========================================================================
  FUNCTION QMI_PBM_IF_PB_TYPE_TO_CAT()

  DESCRIPTION
    Get the corresponding pbm cat from phonebook type.

  PARAMETERS
    pb_type        : PBM PB Type

  RETURN VALUE
    pbm_cat_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
pbm_cat_e_type qmi_pbm_if_pb_type_to_cat
(
  qmi_pbm_if_pb_type pb_type
)
{
  switch(pb_type)
  {
    case QMI_PBM_IF_PB_ADN:
      return PBM_CAT_SIM_ADN;
    case QMI_PBM_IF_PB_FDN:
      return PBM_CAT_SIM_FDN;
    case QMI_PBM_IF_PB_MSISDN:
      return PBM_CAT_SIM_MSISDN;
    case QMI_PBM_IF_PB_MBDN:
      return PBM_CAT_SIM_MBDN;
    case QMI_PBM_IF_PB_MBN:
      return PBM_CAT_SIM_MBN;
    case QMI_PBM_IF_PB_SDN:
      return PBM_CAT_SIM_SDN;
    case QMI_PBM_IF_PB_BDN:
      return PBM_CAT_SIM_BDN;
    case QMI_PBM_IF_PB_LND:
      return PBM_CAT_NONE;  /* PBM_CAT_SIM_LND not present */
    case QMI_PBM_IF_PB_GAS:
      return PBM_CAT_GRPNAME;
    case QMI_PBM_IF_PB_AAS:
      return PBM_CAT_AAS;
    default:
      return PBM_CAT_NONE;
  }

} /* qmi_pbm_if_pb_type_to_cat */


/*===========================================================================
  FUNCTION QMI_PBM_IF_DEVICE_TYPE_TO_PB_TYPE()

  DESCRIPTION
    Get the phonebook type from corresponding pbm device type.

  PARAMETERS
    pb_type        : PBM PB Type

  RETURN VALUE
    pbm_device_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_pbm_if_pb_type qmi_pbm_if_device_type_to_pb_type
(
  pbm_device_type  dev_type
)
{
  switch(dev_type)
  {
    case PBM_ADN:
      return QMI_PBM_IF_PB_ADN;
    case PBM_FDN:
      return QMI_PBM_IF_PB_FDN;
    case PBM_MSISDN:
      return QMI_PBM_IF_PB_MSISDN;
    case PBM_MBDN:
      return QMI_PBM_IF_PB_MBDN;
    case PBM_MBN:
      return QMI_PBM_IF_PB_MBN;
    case PBM_SDN:
      return QMI_PBM_IF_PB_SDN;
    case PBM_BDN:
      return QMI_PBM_IF_PB_BDN;
    case PBM_LND:
      return QMI_PBM_IF_PB_LND;
    case PBM_GRPNAME:
      return QMI_PBM_IF_PB_GAS;
    case PBM_AAS:
      return QMI_PBM_IF_PB_AAS;
    case PBM_DEVICE_TYPE_ALL:
      return QMI_PBM_DEVICE_TYPE_ALL;
    default:
      return QMI_PBM_IF_PB_DEFAULT;
  }
} /* qmi_pbm_if_device_type_to_pb_type */


qmi_pbm_if_pb_rec_update_op_type qmi_pbm_if_map_pbm_event_to_operation
(
  pbm_notify_event_e_type  rec_update_event
)
{
  switch(rec_update_event)
  {
    case PBM_EVENT_REC_ADD:
      return QMI_PBM_IF_PB_REC_OP_ADD;
    case PBM_EVENT_REC_UPDATE:
      return QMI_PBM_IF_PB_REC_OP_MODIFY;
    case PBM_EVENT_REC_DELETE:
      return QMI_PBM_IF_PB_REC_OP_DELETE;
    default:
      return QMI_PBM_IF_PB_REC_OP_DEFAULT;
  }
}

/*===========================================================================
  FUNCTION QMI_PBM_IF_PB_TYPE_TO_DEVICE_TYPE()

  DESCRIPTION
    Get the corresponding pbm device type from phonebook type.

  PARAMETERS
    pb_type        : PBM PB Type

  RETURN VALUE
    pbm_device_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
pbm_device_type qmi_pbm_if_pb_type_to_device_type
(
  qmi_pbm_if_pb_type pb_type
)
{
  switch(pb_type)
  {
    case QMI_PBM_IF_PB_ADN:
      return PBM_ADN;
    case QMI_PBM_IF_PB_FDN:
      return PBM_FDN;
    case QMI_PBM_IF_PB_MSISDN:
      return PBM_MSISDN;
    case QMI_PBM_IF_PB_MBDN:
      return PBM_MBDN;
    case QMI_PBM_IF_PB_MBN:
      return PBM_MBN;
    case QMI_PBM_IF_PB_SDN:
      return PBM_SDN;
    case QMI_PBM_IF_PB_BDN:
      return PBM_BDN;
    case QMI_PBM_IF_PB_LND:
      return PBM_LND;
    case QMI_PBM_IF_PB_GAS:
      return PBM_GRPNAME;
    case QMI_PBM_IF_PB_AAS:
      return PBM_AAS;
    default:
      return PBM_DEFAULT;
  }

} /* qmi_pbm_if_pb_type_to_device_type */


/*===========================================================================
  FUNCTION QMI_PBM_IF_DERIVE_PB_CATEGORY_FROM_SESSION()

  DESCRIPTION
    Get the pb category (GPB/LPB) from session type.

  PARAMETERS
    sess_type        : Session Type

  RETURN VALUE
    pbm_pb_category_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
pbm_pb_category_enum_type qmi_pbm_if_derive_pb_category_from_session
(
  uint8 sess_type
)
{

  switch(sess_type)
  {
    case QMI_PBM_IF_SESSION_GW_PRIMARY:
    case QMI_PBM_IF_SESSION_1X_PRIMARY:
    case QMI_PBM_IF_SESSION_GW_SECONDARY:
    case QMI_PBM_IF_SESSION_1X_SECONDARY:
#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GW_TERTIARY:
    case QMI_PBM_IF_SESSION_1X_TERTIARY:
#endif
      return PBM_LPB;

    case QMI_PBM_IF_SESSION_GPB_SLOT1:
    case QMI_PBM_IF_SESSION_GPB_SLOT2:
#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GPB_SLOT3:
#endif
      return PBM_GPB;

    default:
      return ((pbm_pb_category_enum_type)QMI_PBM_IF_INVALID_PB_CAT);
  }
} /* qmi_pbm_if_derive_pb_category_from_session */


/*===========================================================================
  FUNCTION QMI_PBM_IF_DERIVE_SESSION_FROM_PROV_TYPE()

  DESCRIPTION
    Get the corresponding session type of pbm provisioning type.

  PARAMETERS
    sess_type        : Session Type

  RETURN VALUE
    QMI pbm session type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_pbm_if_derive_session_from_prov_type
(
  pbm_provision_enum_type prov_type
)
{

  uint8 sess_type;

  switch(prov_type)
  {
    case PBM_PROVISION_GW_PRIMARY:
      sess_type = QMI_PBM_IF_SESSION_GW_PRIMARY;
      break;
    case PBM_PROVISION_1X_PRIMARY:
      sess_type = QMI_PBM_IF_SESSION_1X_PRIMARY;
      break;
    case PBM_PROVISION_GW_SECONDARY:
      sess_type = QMI_PBM_IF_SESSION_GW_SECONDARY;
      break;
    case PBM_PROVISION_1X_SECONDARY:
      sess_type = QMI_PBM_IF_SESSION_1X_SECONDARY;
      break;
#ifdef FEATURE_TRIPLE_SIM
    case PBM_PROVISION_GW_TERTIARY:
      sess_type = QMI_PBM_IF_SESSION_GW_TERTIARY;
      break;
    case PBM_PROVISION_1X_TERTIARY:
      sess_type = QMI_PBM_IF_SESSION_1X_TERTIARY;
      break;
#endif
    default:
      sess_type = QMI_PBM_IF_SESSION_MAX;
      break;
  }

  return sess_type;

} /* qmi_pbm_if_derive_session_from_prov_type */


/*===========================================================================
  FUNCTION QMI_PBM_IF_DERIVE_PROV_TYPE_FROM_SESSION()

  DESCRIPTION
    Get the corresponding pbm provisioning type from session type.

  PARAMETERS
    sess_type        : Session Type

  RETURN VALUE
    pbm_device_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
pbm_provision_enum_type qmi_pbm_if_derive_prov_type_from_session
(
  uint8 sess_type
)
{

  switch(sess_type)
  {
    case QMI_PBM_IF_SESSION_GW_PRIMARY:
      return PBM_PROVISION_GW_PRIMARY;
    case QMI_PBM_IF_SESSION_1X_PRIMARY:
      return PBM_PROVISION_1X_PRIMARY;
    case QMI_PBM_IF_SESSION_GW_SECONDARY:
      return PBM_PROVISION_GW_SECONDARY;
    case QMI_PBM_IF_SESSION_1X_SECONDARY:
      return PBM_PROVISION_1X_SECONDARY;
#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GW_TERTIARY:
      return PBM_PROVISION_GW_TERTIARY;
    case QMI_PBM_IF_SESSION_1X_TERTIARY:
      return PBM_PROVISION_1X_TERTIARY;
#endif
    default:
      return PBM_PROVISION_NONE;
  }

} /* qmi_pbm_if_derive_prov_type_from_session */


/*===========================================================================
  FUNCTION QMI_PBM_IF_DERIVE_SLOT_FROM_SESSION()

  DESCRIPTION
    Derive the slot id from session type.

  PARAMETERS
    uint8        : Session Type

  RETURN VALUE
    pbm_slot_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
pbm_slot_type qmi_pbm_if_derive_slot_from_session
(
  uint8 sess_type
)
{
  switch(sess_type)
  {
    case QMI_PBM_IF_SESSION_GW_PRIMARY:
    case QMI_PBM_IF_SESSION_1X_PRIMARY:
    case QMI_PBM_IF_SESSION_GW_SECONDARY:
    case QMI_PBM_IF_SESSION_1X_SECONDARY:
#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GW_TERTIARY:
    case QMI_PBM_IF_SESSION_1X_TERTIARY:
#endif
      return ((pbm_slot_type)QMI_PBM_IF_INVALID_SLOT);

    case QMI_PBM_IF_SESSION_GPB_SLOT1:
      return PBM_SLOT_1;

    case QMI_PBM_IF_SESSION_GPB_SLOT2:
      return PBM_SLOT_2;

#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GPB_SLOT3:
      return PBM_SLOT_3;
#endif
    default:
      return ((pbm_slot_type)QMI_PBM_IF_INVALID_SLOT);
  }

} /* qmi_pbm_if_derive_slot_from_session */


/*===========================================================================
  FUNCTION QMI_PBM_IF_DERIVE_SESSION_FROM_PB_TYPE()

  DESCRIPTION
    Derive the session data from pb_type.

  PARAMETERS
    uint8        : Session Type

  RETURN VALUE
    pbm_slot_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_pbm_if_derive_session_from_pb_type
(
  pbm_phonebook_type *pb_info
)
{
  uint8 sess_type;

  ASSERT(pb_info);

  if(pb_info->prov_type == PBM_PROVISION_NONE)
  {
    if(pb_info->slot_id == PBM_SLOT_1)
    {
      sess_type = QMI_PBM_IF_SESSION_GPB_SLOT1;
    }
    else if ( pb_info->slot_id == PBM_SLOT_2 )
    {
      sess_type = QMI_PBM_IF_SESSION_GPB_SLOT2;
    }
#ifdef FEATURE_TRIPLE_SIM
    else if ( pb_info->slot_id == PBM_SLOT_3 )
    {
      sess_type = QMI_PBM_IF_SESSION_GPB_SLOT3;
    }
#endif
    else
    {
      UIM_MSG_HIGH_2("Invalid pb type info prov_type=%d, slot=%d", pb_info->prov_type, pb_info->slot_id );
      sess_type = QMI_PBM_IF_SESSION_MAX;
    }
  }
  else
  {
    sess_type = qmi_pbm_if_derive_session_from_prov_type(pb_info->prov_type);
  }
  return sess_type;
} /* qmi_pbm_if_derive_session_from_pb_type */


/*===========================================================================
  FUNCTION QMI_PBM_IF_MAP_RETURN_TYPE_TO_PB_STATE()

  DESCRIPTION
    Map the AMSS-PBM return type to the phonebook status.

  PARAMETERS
    ret        : pbm_return_type

  RETURN VALUE
    state of the phonebook qmi_pbm_if_pb_state_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_pbm_if_pb_state_e_type qmi_pbm_if_map_return_type_to_pb_state
(
  pbm_return_type ret
)
{
  qmi_pbm_if_pb_state_e_type state = QMI_PBM_IF_PB_STATE_MAX;

  switch(ret)
  {
    case PBM_SUCCESS:
      state = QMI_PBM_IF_PB_STATE_READY;
      break;
    case PBM_ERROR_NOT_AVAILABLE:
      state = QMI_PBM_IF_PB_STATE_NOT_AVAILABLE;
      break;
    case PBM_ERROR_NOT_READY:
      state = QMI_PBM_IF_PB_STATE_NOT_READY;
      break;
    case PBM_ERROR_SIM_PIN_REQ:
      state = QMI_PBM_IF_PB_STATE_PIN_RESTRICTION;
      break;
    case PBM_ERROR_SIM_PUC_REQ:
      state = QMI_PBM_IF_PB_STATE_PUK_RESTRICTION;
      break;
    case PBM_ERROR_RESTRICTED:
      state = QMI_PBM_IF_PB_STATE_INVALIDATED;
      break;
    case PBM_ERROR_SYNC:
      state = QMI_PBM_IF_PB_STATE_SYNC;
      break;
    default:
      break;
  }
  return state;
} /* qmi_pbm_if_map_return_type_to_pb_state */


/*===========================================================================
  FUNCTION qmi_pbm_if_qmi_session_to_pbm_session()

  DESCRIPTION
    Map the AMSS-QMI session type to the PBM session type.

  PARAMETERS
    ret        : pbm_return_type

  RETURN VALUE
    state of the phonebook pbm_session_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

pbm_session_enum_type qmi_pbm_if_qmi_session_to_pbm_session
(
  qmi_pbm_if_session_type qmi_sess
)
{
  pbm_session_enum_type pbm_session = PBM_SESSION_MAX;

  switch(qmi_sess)
  {
    case QMI_PBM_IF_SESSION_GPB_SLOT1:
      pbm_session = PBM_SESSION_GPB_1;
      break;
    case QMI_PBM_IF_SESSION_GPB_SLOT2:
      pbm_session = PBM_SESSION_GPB_2;
      break;
    case QMI_PBM_IF_SESSION_GW_PRIMARY:
      pbm_session = PBM_SESSION_LPB_GW;
      break;
    case QMI_PBM_IF_SESSION_1X_PRIMARY:
      pbm_session = PBM_SESSION_LPB_1X;
      break;
    case QMI_PBM_IF_SESSION_GW_SECONDARY:
      pbm_session = PBM_SESSION_LPB_GW_SECONDARY;
      break;
    case QMI_PBM_IF_SESSION_1X_SECONDARY:
      pbm_session = PBM_SESSION_LPB_1X_SECONDARY;
      break;
#ifdef FEATURE_TRIPLE_SIM
    case QMI_PBM_IF_SESSION_GPB_SLOT3:
      pbm_session = PBM_SESSION_GPB_3;
      break;
    case QMI_PBM_IF_SESSION_GW_TERTIARY:
      pbm_session = PBM_SESSION_LPB_GW_TERTIARY;
      break;
    case QMI_PBM_IF_SESSION_1X_TERTIARY:
      pbm_session = PBM_SESSION_LPB_1X_TERTIARY;
      break;
#endif
    default:
      break;
  }
  return pbm_session;
} /* qmi_pbm_if_pbm_session_to_qmi_session */


/*===========================================================================
  FUNCTION QMI_PBM_IF_FILL_PB_TYPE_INFO()

  DESCRIPTION
    Fill the pbm_phonebook_type data from session, pb_type.

  PARAMETERS
    uint8        : Session Type
    uint16       : Phonebook Type
    pb_info      : Pointer to pbm_phonebook_type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_fill_pb_type_info
(
  uint8               sess_type,
  uint16              pb_type,
  pbm_phonebook_type *pb_info

)
{
  ASSERT(pb_info != NULL);

  pb_info->device_type = qmi_pbm_if_pb_type_to_device_type((qmi_pbm_if_pb_type)pb_type);
  pb_info->pb_category = qmi_pbm_if_derive_pb_category_from_session(sess_type);
  pb_info->prov_type = qmi_pbm_if_derive_prov_type_from_session(sess_type);
  pb_info->slot_id = qmi_pbm_if_derive_slot_from_session(sess_type);

  UIM_MSG_HIGH_4(" phonebook %d, prov_type %d, slot_id %d, pb_cat %d ", pb_info->device_type,
                                        pb_info->prov_type, pb_info->slot_id, pb_info->pb_category);

} /* qmi_pbm_if_fill_pb_type_info */


/*===========================================================================
  FUNCTION QMI_PBM_IF_IS_REC_ID_IN_RANGE()

  DESCRIPTION
    Check if the given record is in the allowed range of record ids

  PARAMETERS
    sess_type        : Session Type
    pb_type          : Phonebook Type
    rec_id           : Record id

  RETURN VALUE
    TRUE if the record is in range. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_if_is_rec_id_in_range
(
  uint8    sess_type,
  uint16   pb_type,
  uint16   rec_id
)
{
  uint8 i;
  uint16 max_recs;
  boolean ret = FALSE;

  for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
  {
    if(pbm_if_info.sess_data[i].sess_type == sess_type)
    {
      switch(pb_type)
      {
        case QMI_PBM_IF_PB_ADN:
          max_recs = pbm_if_info.sess_data[i].pb_data.max_adn_recs;
          break;
        case QMI_PBM_IF_PB_FDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_fdn_recs;
          break;
        case QMI_PBM_IF_PB_MSISDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_msisdn_recs;
          break;
        case QMI_PBM_IF_PB_MBDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_mbdn_recs;
          break;
        case QMI_PBM_IF_PB_MBN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_mbn_recs;
          break;
        case QMI_PBM_IF_PB_SDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_sdn_recs;
          break;
        case QMI_PBM_IF_PB_BDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_bdn_recs;
          break;
        case QMI_PBM_IF_PB_LND:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_lnd_recs;
          break;
        case QMI_PBM_IF_PB_GAS:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_gas_recs;
          break;
        case QMI_PBM_IF_PB_AAS:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_aas_recs;
          break;
        default:
          return ret;
      }
      if((rec_id >= 1) && (rec_id <= max_recs))
      {
        ret = TRUE;
        break;
      }
    }
  }
  return ret;
} /* qmi_pbm_if_is_rec_id_in_range */


/*===========================================================================
  FUNCTION QMI_PBM_IF_IS_ANY_SESSION_INIT_DONE()

  DESCRIPTION
    check if any of the sessions is initialized.

  PARAMETERS
    None

  RETURN VALUE
    TRUE if atleast one session is initialized.
    FALSE if no sessions are initialized.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_if_is_any_session_init_done
(
  void
)
{
  return (pbm_if_info.num_of_sessions > 0);
} /* qmi_pbm_if_is_any_session_init_done */


/*===========================================================================
  FUNCTION QMI_PBM_IF_SET_SESSION_INIT_STATUS()

  DESCRIPTION
    Set and store the session initialization status

  PARAMETERS


  RETURN VALUE
    None


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_set_session_init_status
(
  uint8      sess_type
)
{
	uint8 i;

	if(sess_type >= QMI_PBM_IF_SESSION_MAX)
	{
          UIM_MSG_HIGH_1("qmi_pbm_if_set_session_init_status:Invalid session type %d",sess_type);
		return;
	}

	for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
	{
		if( pbm_if_info.sess_data[i].sess_type == sess_type )
		{
			break;
		}
	}

	if ( i < QMI_PBM_IF_MAX_SESSIONS)
	{
		if(!pbm_if_info.sess_data[i].is_initialized)
		{
			pbm_if_info.sess_data[i].is_initialized = TRUE;
			pbm_if_info.num_of_sessions++;
		}
	}
	else
	{
		/*  If we are here it means that sess_type is not initialized yet */
		/*  in other words there was no pb_ready event from pbm for this session */
		/*  Get the first free session and fill the data */
		for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
		{
			if( pbm_if_info.sess_data[i].sess_type == QMI_PBM_IF_SESSION_MAX )
			{
				pbm_if_info.sess_data[i].sess_type = sess_type;
				pbm_if_info.sess_data[i].is_initialized = TRUE;
				pbm_if_info.num_of_sessions++;
				break;
			}
		}
	}
} /* qmi_pbm_if_set_session_init_status */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_MAX_RECORDS()

  DESCRIPTION
    Get the max records for the given phonebook and session

  PARAMETERS


  RETURN VALUE
    max_recs for the given phonebook and session


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16 qmi_pbm_if_get_max_records
(
  uint8      sess_type,
  uint16     pb_type
)
{
  uint8 i;
  uint16 max_recs = 0;

  for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
  {
    if(pbm_if_info.sess_data[i].sess_type == sess_type)
    {
      switch(pb_type)
      {
        case QMI_PBM_IF_PB_ADN:
          max_recs = pbm_if_info.sess_data[i].pb_data.max_adn_recs;
          break;
        case QMI_PBM_IF_PB_FDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_fdn_recs;
          break;
        case QMI_PBM_IF_PB_MSISDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_msisdn_recs;
          break;
        case QMI_PBM_IF_PB_MBDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_mbdn_recs;
          break;
        case QMI_PBM_IF_PB_MBN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_mbn_recs;
          break;
        case QMI_PBM_IF_PB_SDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_sdn_recs;
          break;
        case QMI_PBM_IF_PB_BDN:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_bdn_recs;
          break;
        case QMI_PBM_IF_PB_LND:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_lnd_recs;
          break;
        case QMI_PBM_IF_PB_GAS:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_gas_recs;
          break;
        case QMI_PBM_IF_PB_AAS:
          max_recs = (uint16)pbm_if_info.sess_data[i].pb_data.max_aas_recs;
          break;
        default:
          break;
      }
    }
  }
  return max_recs;
} /* qmi_pbm_if_get_max_records */


/*===========================================================================
  FUNCTION QMI_PBM_IF_STORE_MAX_RECORDS_DATA()

  DESCRIPTION
    Store the max records possible for each phonebook

  PARAMETERS


  RETURN VALUE
    None


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_store_max_records_data
(
  uint8      sess_type,
  uint16     pb_type,
  uint16     max_recs
)
{
  uint8 i;

  for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
  {
    if(pbm_if_info.sess_data[i].sess_type == sess_type)
    {
      switch(pb_type)
      {
        case QMI_PBM_IF_PB_ADN:
          pbm_if_info.sess_data[i].pb_data.max_adn_recs = max_recs;
          break;
        case QMI_PBM_IF_PB_FDN:
          pbm_if_info.sess_data[i].pb_data.max_fdn_recs = (uint8)max_recs;
          break;
        case QMI_PBM_IF_PB_MSISDN:
          pbm_if_info.sess_data[i].pb_data.max_msisdn_recs = (uint8) max_recs;
          break;
        case QMI_PBM_IF_PB_MBDN:
          pbm_if_info.sess_data[i].pb_data.max_mbdn_recs =(uint8) max_recs;
          break;
        case QMI_PBM_IF_PB_MBN:
          pbm_if_info.sess_data[i].pb_data.max_mbn_recs =(uint8) max_recs;
          break;
        case QMI_PBM_IF_PB_SDN:
          pbm_if_info.sess_data[i].pb_data.max_sdn_recs = (uint8)max_recs;
          break;
        case QMI_PBM_IF_PB_BDN:
          pbm_if_info.sess_data[i].pb_data.max_bdn_recs = (uint8)max_recs;
          break;
        case QMI_PBM_IF_PB_LND:
          pbm_if_info.sess_data[i].pb_data.max_lnd_recs = (uint8)max_recs;
          break;
        case QMI_PBM_IF_PB_GAS:
          pbm_if_info.sess_data[i].pb_data.max_gas_recs = max_recs;
          break;
        case QMI_PBM_IF_PB_AAS:
          pbm_if_info.sess_data[i].pb_data.max_aas_recs = max_recs;
          break;
        default:
          return;
      }
    }
  }
} /* qmi_pbm_if_store_max_records_data */


/*===========================================================================
  FUNCTION QMI_PBM_IF_STORE_DELETE_ALL_PB_INFO()

  DESCRIPTION
    Store the phonebook data for which the delete operation is started

  PARAMETERS
    sess_type        : Session Type
    pb_type          : Phonebook Type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_store_delete_all_pb_info
(
  uint8 sess_type,
  uint16 pb_type
)
{
  uint8 i;

  for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
  {
    if(pbm_if_info.sess_data[i].sess_type == sess_type)
    {
      pbm_if_info.sess_data[i].del_all_pb_mask |= pb_type;
      break;
    }
  }
}/* qmi_pbm_if_store_delete_all_pb_info */


/*===========================================================================
  FUNCTION QMI_PBM_IF_CLEAR_DELETE_ALL_PB_INFO()

  DESCRIPTION
    Clear the phonebook data for which the delete operation is complete

  PARAMETERS
    sess_type        : Session Type
    pb_type          : Phonebook Type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_clear_delete_all_pb_info
(
  uint8 sess_type,
  uint16 pb_type
)
{
  uint8 i;

  for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
  {
    if(pbm_if_info.sess_data[i].sess_type == sess_type)
    {
      pbm_if_info.sess_data[i].del_all_pb_mask &= ~pb_type;
      break;
    }
  }
}/* qmi_pbm_if_clear_delete_all_pb_info */


/*===========================================================================
  FUNCTION QMI_PBM_IF_IS_PB_BEING_DELETED()

  DESCRIPTION
    Check if the given phonebook is being deleted

  PARAMETERS
    sess_type        : Session Type
    pb_type          : Phonebook Type

  RETURN VALUE
    TRUE if the phonebook is being deleted. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_if_is_pb_being_deleted
(
  uint8   sess_type,
  uint16  pb_type
)
{
  uint8    i = 0;

  for( i = 0; i < QMI_PBM_IF_MAX_SESSIONS; i++)
  {
    if((pbm_if_info.sess_data[i].sess_type == sess_type) &&
       ((pbm_if_info.sess_data[i].del_all_pb_mask & pb_type) == pb_type))
    {
      UIM_MSG_MED_2("Phonebook is being deleted for session 0x%x pb_type 0x%x", sess_type, pb_type);
      return TRUE;
    }
  }
  return FALSE;
} /* qmi_pbm_if_is_pb_being_deleted */


/*===========================================================================
  FUNCTION QMI_PBM_IF_FILL_PB_CAPS()

  DESCRIPTION
    Fill the phonebook capabilities of a specific phonebook.

  PARAMETERS
    sess_type        : Session type
    pb_type          : Phonebook type
    params_ptr        : the qmi_pbm_if_pb_caps_s pointer

  RETURN VALUE
    pbm_return_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
pbm_return_type qmi_pbm_if_fill_pb_caps
(
  uint8 sess_type,
  uint16 pb_type,
  qmi_pbm_if_pb_caps_s *pb_caps
)
{
  pbm_phonebook_type pb_info, type3_pb_info;
  pbm_cat_e_type cat;
  pbm_extended_fileinfo_s_type recs_info, type3_recs_info;
  pbm_field_info_s_type *fields;
  int  f_cnt = 0; /* Number of fields per record */
  pbm_return_type ret = PBM_SUCCESS;

  ASSERT( pb_caps != NULL );

  memset(pb_caps, 0, sizeof(qmi_pbm_if_pb_caps_s));

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(sess_type, pb_type, &pb_info);

  /* Map the corresponding pbm cat */
  cat = qmi_pbm_if_pb_type_to_cat((qmi_pbm_if_pb_type)pb_type);

  UIM_MSG_HIGH_1(" qmi_pbm_if_pb_type_to_cat returned cat %d", cat);

  /* Get the number of records in use and total records for the pb */
  ret = pbm_session_extended_file_info(pb_info, &recs_info);
  if(ret != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_1("pbm_session_extended_file_info: returned %d ", ret);
    return ret;
  }
  if(pb_type == QMI_PBM_IF_PB_ADN)
  {
    pbm_all_usim_field_capabilities_s_type        *info     = NULL;
    pbm_adn_capabilites_pbset_s_type              *adn_info = NULL ;
    uint8                                         i,j;

    pb_caps->pbset_info_valid = TRUE ;
    adn_info =  QMI_PBM_IF_MEM_ALLOC( sizeof(pbm_adn_capabilites_pbset_s_type));
    if(adn_info == NULL)
    {
      return PBM_ERROR;
    }
    ret = pbm_get_adn_pbset_capibilities(pb_info,adn_info);
    if(ret != PBM_SUCCESS)
    {
      UIM_MSG_HIGH_1("pbm_get_adn_pbset_capibilities: returned %d ", ret);
      QMI_PBM_IF_MEM_FREE(adn_info);
      return ret;
    }

    for( i=0; i < adn_info->num_pb_sets ; i++ )
    {
      pb_caps->pb_set_caps.pb_set_info[i].pb_set_id = i+1;
      pb_caps->pb_set_caps.pb_set_info[i].adn_caps.used_adn_records =
                               adn_info->adn_caps[i].used_adn_records;
      pb_caps->pb_set_caps.pb_set_info[i].adn_caps.max_adn_records  =
                               adn_info->adn_caps[i].max_adn_records;
      pb_caps->pb_set_caps.pb_set_info[i].adn_caps.max_adn_num_len  =
                               adn_info->adn_caps[i].max_num_len;
      pb_caps->pb_set_caps.pb_set_info[i].adn_caps.max_adn_name_len =
                               adn_info->adn_caps[i].max_text_len;
    }

    pb_caps->pb_set_caps.pb_set_info_len = adn_info->num_pb_sets ;
    pb_caps->pb_set_caps.pb_sets = adn_info->num_pb_sets ;
    QMI_PBM_IF_MEM_FREE(adn_info);

    info = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_all_usim_field_capabilities_s_type));
    if(info == NULL)
    {
      return PBM_ERROR;
    }
    ret = pbm_get_phonebook_usim_fields_capibilities (pb_info,info);
    if(ret != PBM_SUCCESS)
    {
      UIM_MSG_HIGH_1("pbm_get_phonebook_usim_fields_capibilities: returned %d ", ret);
      QMI_PBM_IF_MEM_FREE(info);
      return ret;
    }

    for( i=0; i < info->total_pbsets ; i++ )
    {
      uint8 email_cnt=0,anr_cnt=0,sne_cnt=0,grp_cnt=0;
      for( j=0;j < PBM_MAX_USIM_FIELDS_IN_PB_SET; j++ )
      {
        switch( info->pbm_all_usim_capibilities[i][j].field_id )
        {
          case PBM_FIELD_EMAIL :
          {
            pb_caps->pb_set_caps.pb_set_info[i].email_caps[email_cnt].record_caps.max_record_len =
                              info->pbm_all_usim_capibilities[i][j].record_length;
            pb_caps->pb_set_caps.pb_set_info[i].email_caps[email_cnt].record_caps.max_records =
                              info->pbm_all_usim_capibilities[i][j].num_of_records;
            pb_caps->pb_set_caps.pb_set_info[i].email_caps[email_cnt].record_caps.used_records =
                              info->pbm_all_usim_capibilities[i][j].records_in_use;
            pb_caps->pb_set_caps.pb_set_info[i].email_caps[email_cnt].mapping_type =
                              info->pbm_all_usim_capibilities[i][j].mapping_type;
            email_cnt++ ;
          }
          break;

          case PBM_FIELD_ADDITIONAL_NUMBER_EXT :
          {
            pb_caps->pb_set_caps.pb_set_info[i].ad_num_caps[anr_cnt].record_caps.max_record_len =
                              info->pbm_all_usim_capibilities[i][j].record_length;
            pb_caps->pb_set_caps.pb_set_info[i].ad_num_caps[anr_cnt].record_caps.max_records =
                              info->pbm_all_usim_capibilities[i][j].num_of_records;
            pb_caps->pb_set_caps.pb_set_info[i].ad_num_caps[anr_cnt].record_caps.used_records =
                              info->pbm_all_usim_capibilities[i][j].records_in_use;
            pb_caps->pb_set_caps.pb_set_info[i].ad_num_caps[anr_cnt].mapping_type =
                              info->pbm_all_usim_capibilities[i][j].mapping_type;
            anr_cnt++;
          }
          break;

          case PBM_FIELD_OTHERNAME :
          {
            pb_caps->pb_set_caps.pb_set_info[i].sne_caps[sne_cnt].record_caps.max_record_len =
                              info->pbm_all_usim_capibilities[i][j].record_length;
            pb_caps->pb_set_caps.pb_set_info[i].sne_caps[sne_cnt].record_caps.max_records =
                              info->pbm_all_usim_capibilities[i][j].num_of_records;
            pb_caps->pb_set_caps.pb_set_info[i].sne_caps[sne_cnt].record_caps.used_records =
                              info->pbm_all_usim_capibilities[i][j].records_in_use;
            pb_caps->pb_set_caps.pb_set_info[i].sne_caps[sne_cnt].mapping_type =
                              info->pbm_all_usim_capibilities[i][j].mapping_type;
            sne_cnt++;
          }
          break;

          case PBM_FIELD_GRPNAME_ID :
          {
            pb_caps->pb_set_caps.pb_set_info[i].grp_caps[grp_cnt].record_caps.max_record_len =
                              info->pbm_all_usim_capibilities[i][j].record_length;
            pb_caps->pb_set_caps.pb_set_info[i].grp_caps[grp_cnt].record_caps.max_records =
                              info->pbm_all_usim_capibilities[i][j].num_of_records;
            pb_caps->pb_set_caps.pb_set_info[i].grp_caps[grp_cnt].record_caps.used_records =
                              info->pbm_all_usim_capibilities[i][j].records_in_use;
            pb_caps->pb_set_caps.pb_set_info[i].grp_caps[grp_cnt].mapping_type =
                              info->pbm_all_usim_capibilities[i][j].mapping_type;
            grp_cnt++;
          }
          break;

          default:
          break;
        }
      }

      pb_caps->pb_set_caps.pb_set_info[i].email_caps_len = email_cnt;
      pb_caps->pb_set_caps.pb_set_info[i].ad_num_caps_len = anr_cnt ;
      pb_caps->pb_set_caps.pb_set_info[i].sne_caps_len= sne_cnt ;
      pb_caps->pb_set_caps.pb_set_info[i].grp_caps_len = grp_cnt ;
    }

    pb_caps->num_pb_sets = info->total_pbsets;
    pb_caps->pb_set_caps.pb_set_info_len =  info->total_pbsets ;
    QMI_PBM_IF_MEM_FREE(info);
  }
  pb_caps->pb_type   = pb_type;
  pb_caps->used_recs = recs_info.records_used;
  pb_caps->max_recs  = recs_info.number_of_records;
  pb_caps->write_access_cap.pbm_protection_type = QMI_PBM_IF_NOT_KNOWN;
  if(recs_info.pb_write_status.write_status == PBM_WRITE_STATUS_ALLOWED)
  {
    pb_caps->write_access_cap.pbm_protection_type = QMI_PBM_IF_ALLOWED;
  }
  else if(recs_info.pb_write_status.write_status == PBM_WRITE_STATUS_NOT_ALLOWED)
  {
    pb_caps->write_access_cap.pbm_protection_type =
        qmi_pbm_if_map_protection_to_qmi(recs_info.pb_write_status.protection_method);
    pb_caps->write_access_cap.pbm_pin_mask =
        qmi_pbm_if_map_protection_mask_to_qmi(recs_info.pb_write_status.pin_protection_mask);
  }

  UIM_MSG_HIGH_4("Max records = %d, Used records = %d for pb %d and session %d",
                   recs_info.number_of_records, recs_info.records_used, pb_type, sess_type);

  /* Get the number of field types supported in a record */
  ret = pbm_session_get_field_info_count(pb_info, cat, &f_cnt);
  if(ret != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_1("pbm_session_get_field_info_count: returned %d ", ret);
    return ret;
  }

  if(f_cnt > 0)
  {
    uint8 i;
    int fields_size = f_cnt*sizeof(pbm_field_info_s_type);

    fields = QMI_PBM_IF_MEM_ALLOC(fields_size);
    if( NULL == fields )
    {
      return PBM_ERROR;
    }

    /* Get the fields capabilities from pbm */
    ret = pbm_session_get_field_info(pb_info, cat, fields, fields_size);
    if(ret != PBM_SUCCESS)
    {
      UIM_MSG_HIGH_1("pbm_session_get_field_info: returned %d ",ret);
      /* Free the mem used for holding the field data */
      QMI_PBM_IF_MEM_FREE(fields);
      return ret;
    }
    for(i = 0; i < f_cnt; i++)
    {
      switch(fields[i].field_id)
      {
        case PBM_FIELD_PHONE_GENERIC:
          /* Subract 2 from max_length as PBM includes 2 extra bytes for nul and + */
          pb_caps->max_num_len = fields[i].max_length - 2;
          UIM_MSG_HIGH_2("Max number length sent is %d for pb_type %d", pb_caps->max_num_len, pb_type);
          break;

        case PBM_FIELD_NAME:
          if((fields[i].max_length-2) > QMI_PBM_IF_MAX_SIM_NAME_LEN)
          {
            /* Maximum alpha length on card is 241 as per 3gpp 31.102 */
            pb_caps->max_name_len = QMI_PBM_IF_MAX_SIM_NAME_LEN;
          }
          else
          {
            /* Subract 2 from max_length as PBM includes 2 bytes for UCS2 null char */
            pb_caps->max_name_len = fields[i].max_length - 2;
          }
          UIM_MSG_HIGH_2("Max name length sent is %d for pb_type %d", pb_caps->max_name_len, pb_type);
          break;

        case PBM_FIELD_ADDITIONAL_NUMBER:
          if(fields[i].num_items > 0)
          {
            pb_caps->is_ad_num_valid = TRUE;
            if(fields[i].num_items <= QMI_PBM_IF_MAX_AD_NUM)
            {
              pb_caps->ad_num_cap.max_ad_nums = fields[i].num_items;
            }
            else
            {
              pb_caps->ad_num_cap.max_ad_nums = QMI_PBM_IF_MAX_AD_NUM;
            }
            /* Subract 2 from max_length as PBM includes 2 extra bytes for nul and + */
            pb_caps->ad_num_cap.max_ad_num_len = fields[i].max_length - 2;
            UIM_MSG_HIGH_3("Max adnumbers 0x%x, max adnum length sent 0x%x for pb_type 0x%x",
                            fields[i].num_items, pb_caps->ad_num_cap.max_ad_num_len, pb_type);
            /* Additional number alpha string not supported in AMSS PBM */
            /* Get the AAS capabilities here */

            /* AAS pbm_phonebook_type info is same as the ADN info except for
               device_type */
            type3_pb_info = pb_info;
            type3_pb_info.device_type = PBM_AAS;

            memset(&type3_recs_info,0,sizeof(type3_recs_info));
            if(pbm_session_extended_file_info(type3_pb_info, &type3_recs_info) != PBM_SUCCESS)
            {
              UIM_MSG_HIGH_0 ("pbm_session_extended_file_info returned error");
          }
            else
            {
              pb_caps->ad_num_cap.max_ad_num_tag_len = type3_recs_info.text_len;

              /* AAS Capabilities */
              pb_caps->is_aas_cap_valid = TRUE;
              pb_caps->aas_cap.max_recs = type3_recs_info.number_of_records;
              pb_caps->aas_cap.used_recs = type3_recs_info.records_used;
              pb_caps->aas_cap.max_aas_len = type3_recs_info.text_len;

              pb_caps->pb_set_caps.aas_caps.max_records = type3_recs_info.number_of_records;
              pb_caps->pb_set_caps.aas_caps.max_record_len = type3_recs_info.text_len ;
              pb_caps->pb_set_caps.aas_caps.used_records = type3_recs_info.records_used ;
            }
            UIM_MSG_HIGH_5("ANR Caps: Max ANR 0x%x, Max string length 0x%x AAS Caps: Max recs 0x%x, Used recs 0x%x, Max AAS len 0x%x",
                            fields[i].num_items, type3_recs_info.text_len,
                            pb_caps->aas_cap.max_recs, pb_caps->aas_cap.used_recs,
                                                                                    pb_caps->aas_cap.max_aas_len);
          }
          break;

        case PBM_FIELD_GRPNAME_ID:
          if(fields[i].num_items > 0)
          {
            pb_caps->is_grp_valid = TRUE;
            pb_caps->grp_cap.max_grps = fields[i].num_items;

            /* Get the GAS capabilities here */

            /* GAS pbm_phonebook_type info is same as the ADN info except for
               device_type */
            type3_pb_info = pb_info;
            type3_pb_info.device_type = PBM_GRPNAME;

            memset(&type3_recs_info,0,sizeof(type3_recs_info));
            if(pbm_session_extended_file_info(type3_pb_info, &type3_recs_info) != PBM_SUCCESS)
            {
              UIM_MSG_HIGH_0 ("pbm_session_extended_file_info returned error");
            }
            else
            {
              pb_caps->grp_cap.max_grp_tag_len = type3_recs_info.text_len;

              /* GAS Capabilities */
              pb_caps->is_gas_cap_valid = TRUE;
              pb_caps->gas_cap.max_recs = type3_recs_info.number_of_records;
              pb_caps->gas_cap.used_recs = type3_recs_info.records_used;
              pb_caps->gas_cap.max_gas_len = type3_recs_info.text_len;

              pb_caps->pb_set_caps.gas_caps.max_records = type3_recs_info.number_of_records;
              pb_caps->pb_set_caps.gas_caps.max_record_len = type3_recs_info.text_len ;
              pb_caps->pb_set_caps.gas_caps.used_records = type3_recs_info.records_used ;
            }
            UIM_MSG_HIGH_5("Group Caps:  Max Grps 0x%x, Max GrpName length 0x%x GAS Caps:  Max recs 0x%x, Used recs 0x%x, Max GAS len 0x%x", 
                            fields[i].num_items, type3_recs_info.text_len,
                            pb_caps->gas_cap.max_recs, pb_caps->gas_cap.used_recs,
                            pb_caps->gas_cap.max_gas_len);
          }
          break;

        case PBM_FIELD_OTHERNAME:
          if(fields[i].num_items > 0)
          {
            if((fields[i].max_length-2) > QMI_PBM_IF_MAX_SIM_NAME_LEN)
            {
              /* Maximum alpha length on card is 241 as per 3gpp 31.102 */
              pb_caps->max_sec_name_len = QMI_PBM_IF_MAX_SIM_NAME_LEN;
            }
            else
            {
              /* Subract 2 from max_length as PBM includes 2 bytes for UCS2 null char */
              pb_caps->max_sec_name_len = fields[i].max_length - 2;
            }
            pb_caps->is_sec_name_supported = TRUE;
            UIM_MSG_HIGH_2("Max second name length sent is %d for pb_type %d", pb_caps->max_sec_name_len, pb_type);
          }
          break;

        case PBM_FIELD_EMAIL:
          if(fields[i].num_items > 0)
          {
            pb_caps->is_email_valid = TRUE;
            pb_caps->email_cap.max_emails = fields[i].num_items;
            if((fields[i].max_length-2) > QMI_PBM_IF_MAX_SIM_NAME_LEN)
            {
              /* Maximum alpha length on card is 241 as per 3gpp 31.102 */
              pb_caps->email_cap.max_email_len = QMI_PBM_IF_MAX_SIM_NAME_LEN;
            }
            else
            {
              /* Subract 2 from max_length as PBM includes 2 bytes for UCS2 null char */
              pb_caps->email_cap.max_email_len = fields[i].max_length - 2;
            }
            UIM_MSG_HIGH_3("Max Emails and max email length sent is %d, %d for pb_type %d", fields[i].num_items, pb_caps->email_cap.max_email_len, pb_type);
          }
          break;

        case PBM_FIELD_HIDDEN:
          if(fields[i].num_items > 0)
          {
            UIM_MSG_HIGH_0("Hidden field is supported");
            pb_caps->is_hidden_entry_supported = TRUE;
          }
          break;

        default:
          break;
      }
    } /* for */
    /* Free the mem allocated for holding the field data */
    QMI_PBM_IF_MEM_FREE(fields);
  }
  else
  {
    /* This case should ideally not arise */
    UIM_MSG_ERR_0("Zero number of fields! ");
    ret = PBM_ERROR;
  }

  return ret;
}

/*===========================================================================
  FUNCTION QMI_PBM_IF_CHECK_SESSION_PB_VALIDITY()

  DESCRIPTION
    Check if the session type and phonebook type are valid

  PARAMETERS
    sess_type        : Session Type
    pb_type          : Phonebook Type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_pbm_if_check_session_pb_validity
(
  uint8   sess_type,
  uint16  pb_type
)
{
  qmi_error_e_type err = QMI_ERR_NONE;
  /* check if session type, pb type are valid */
  if(!qmi_pbm_if_is_session_type_valid(sess_type))
  {
    err = QMI_ERR_INVALID_SESSION_TYPE;
  }
  else if(!qmi_pbm_if_is_pb_type_valid(pb_type))
  {
    err = QMI_ERR_INVALID_PB_TYPE;
  }

  return err;
} /* qmi_pbm_if_check_session_pb_validity */



/*=========================================================================

  FUNCTION:  qmi_pbm_if_fill_ecc_list

===========================================================================*/
/*!
    @brief
    Update the local list of emergency numbers based on the field id

    @return
    errno_enum_type
*/
/*=========================================================================*/
IxErrnoType qmi_pbm_if_fill_ecc_list
(
  uint8 session_type,
  pbm_field_id_e_type pbm_field_id,
  uint8 *emer_categ,
  char *ecc_val,
  uint16 ecc_len,
  qmi_pbm_if_ecc_list_type* pbm_ecc_ptr
)
{
  IxErrnoType ret = E_SUCCESS;
  uint8 i=0, emer_len = 0;
  boolean is_set_card_ecc = FALSE;
  boolean is_set_ota_ecc = FALSE;
  /*-----------------------------------------------------------------------*/

  ASSERT(ecc_val != NULL);

  /* Ignore Nul char sent in the numbers from PBM */
  emer_len = ecc_len-1;
  emer_len = MIN(emer_len,QMI_PBM_IF_MAX_ECC_LEN);
  UIM_PBM_MSG_SPRINTF_3("ECC to be added %s with field=%d len=%d",ecc_val, pbm_field_id, emer_len);

  /* Check for max length supported */
  if(emer_len > QMI_PBM_IF_MAX_ECC_LEN)
  {
    return E_FAILURE;
  }

  if ( pbm_ecc_ptr == NULL )
  {
	UIM_MSG_HIGH_0("pbm_ecc_pointer is not allocated memory");
	return E_FAILURE;
  }

  if(ecc_val != NULL)
  {
    switch(pbm_field_id)
    {
      case PBM_FIELD_NV_ECC:
        if( pbm_ecc_ptr->num_nv_ecc_entries < PBM_NV_EMERGENCY_NUMBERS )
        {
          /*Copy the ecc value in the ecc cache*/
		  pbm_ecc_ptr->nv_ecc[pbm_ecc_ptr->num_nv_ecc_entries].ecc_len = emer_len;
          pbm_ecc_ptr->nv_ecc[pbm_ecc_ptr->num_nv_ecc_entries].category= *emer_categ;
          memscpy( pbm_ecc_ptr->nv_ecc[pbm_ecc_ptr->num_nv_ecc_entries++].ecc, QMI_PBM_IF_MAX_ECC_LEN, ecc_val, ecc_len-1);
        }
        break;
      case PBM_FIELD_SIM1_ECC:
      case PBM_FIELD_SIM2_ECC:
        if(pbm_ecc_ptr->num_of_card_sessions <= QMI_PBM_IF_MAX_SESSIONS)
        {
          /* Loop through the number of sessions and fill the caps of pbs */
          if(pbm_ecc_ptr->num_of_card_sessions != 0)
          {
            for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
            {
              if( pbm_ecc_ptr->card_ecc[i].sess_type == session_type)
              {
                if(pbm_ecc_ptr->card_ecc[i].ecc_count < QMI_PBM_IF_MAX_CARD_ECC)
                {
                  pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc_len = emer_len;
                  pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].category = *emer_categ;
                  memscpy( pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc ,
				  	sizeof(pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc), ecc_val, ecc_len-1);
                  pbm_ecc_ptr->card_ecc[i].ecc_count++;
                  is_set_card_ecc = TRUE;
                }
                break;
              }
            }
            if(is_set_card_ecc == FALSE)
            {
              for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
              {
                if( pbm_ecc_ptr->card_ecc[i].sess_type == QMI_PBM_IF_SESSION_MAX)
                {
                  if(pbm_ecc_ptr->card_ecc[i].ecc_count < QMI_PBM_IF_MAX_CARD_ECC)
                  {
                    pbm_ecc_ptr->card_ecc[i].sess_type = session_type;
                    pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc_len = emer_len;
                    pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].category = *emer_categ;
                    memscpy( pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc ,
						sizeof(pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc), ecc_val, ecc_len-1);
                    pbm_ecc_ptr->card_ecc[i].ecc_count++;
                    pbm_ecc_ptr->num_of_card_sessions++;
                  }
                  break;
                }
              }
            }
          }
          else
          {
            for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
            {
              if( pbm_ecc_ptr->card_ecc[i].sess_type == QMI_PBM_IF_SESSION_MAX)
              {
                if(pbm_ecc_ptr->card_ecc[i].ecc_count < QMI_PBM_IF_MAX_CARD_ECC)
                {
                  pbm_ecc_ptr->card_ecc[i].sess_type = session_type;
                  pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc_len = emer_len;
                  pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].category = *emer_categ;
                  memscpy( pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc ,
				  	sizeof(pbm_ecc_ptr->card_ecc[i].ecc_info[pbm_ecc_ptr->card_ecc[i].ecc_count].ecc), ecc_val, ecc_len-1);
                  pbm_ecc_ptr->card_ecc[i].ecc_count++;
                  pbm_ecc_ptr->num_of_card_sessions++;
                }
                break;
              }
            }
          }
        }
        break;

      case PBM_FIELD_HARDCODED_ECC:
        if( pbm_ecc_ptr->num_hcoded_ecc_entries < QMI_PBM_IF_MAX_HCODED_ECC_EXT )
        {
          /*Copy the ecc value in the ecc cache*/
          pbm_ecc_ptr->hcoded_ecc[pbm_ecc_ptr->num_hcoded_ecc_entries].ecc_len = emer_len;
          pbm_ecc_ptr->hcoded_ecc[pbm_ecc_ptr->num_hcoded_ecc_entries].category = *emer_categ;
          memscpy( pbm_ecc_ptr->hcoded_ecc[pbm_ecc_ptr->num_hcoded_ecc_entries++].ecc,
		  	sizeof(pbm_ecc_ptr->hcoded_ecc[pbm_ecc_ptr->num_hcoded_ecc_entries++].ecc), ecc_val, ecc_len-1);
        }
        break;

      case PBM_FIELD_NETWORK_ECC:
        if( pbm_ecc_ptr->num_of_ota_sessions <= QMI_PBM_IF_MAX_SESSIONS )
        {
          /* Loop through the number of sessions and fill the caps of pbs */
          if(pbm_ecc_ptr->num_of_ota_sessions != 0)
          {
            for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
            {
              if( pbm_ecc_ptr->ota_ecc[i].sess_type == session_type)
              {
                if(pbm_ecc_ptr->ota_ecc[i].ecc_count < CM_MAX_EMERGENCY_NUM_COUNT)
                {
                  pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc_len = emer_len;
                  pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].category = *emer_categ;
                  memscpy( pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc ,
				  	sizeof(pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc), ecc_val, ecc_len-1);
                  pbm_ecc_ptr->ota_ecc[i].ecc_count++;
                  is_set_ota_ecc = TRUE;
                }
                break;
              }
            }
            if(is_set_ota_ecc == FALSE)
            {
              for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
              {
                if( pbm_ecc_ptr->ota_ecc[i].sess_type == QMI_PBM_IF_SESSION_MAX)
                {
                  if(pbm_ecc_ptr->ota_ecc[i].ecc_count < CM_MAX_EMERGENCY_NUM_COUNT)
                  {
                    pbm_ecc_ptr->ota_ecc[i].sess_type = session_type;
                    pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc_len = emer_len;
                    pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].category = *emer_categ;
                    memscpy( pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc ,
						sizeof(pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc), ecc_val, ecc_len-1);
                    pbm_ecc_ptr->ota_ecc[i].ecc_count++;
                    pbm_ecc_ptr->num_of_ota_sessions++;
                  }
                  break;
                }
              }
            }
          }
          else
          {
            for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
            {
              if( pbm_ecc_ptr->ota_ecc[i].sess_type == QMI_PBM_IF_SESSION_MAX)
              {
                if(pbm_ecc_ptr->ota_ecc[i].ecc_count < CM_MAX_EMERGENCY_NUM_COUNT)
                {
                  pbm_ecc_ptr->ota_ecc[i].sess_type = session_type;
                  pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc_len = emer_len;
                  pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].category = *emer_categ;
                  memscpy( pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc ,
				  	sizeof(pbm_ecc_ptr->ota_ecc[i].ecc_info[pbm_ecc_ptr->ota_ecc[i].ecc_count].ecc), ecc_val, ecc_len-1);
                  pbm_ecc_ptr->ota_ecc[i].ecc_count++;
                  pbm_ecc_ptr->num_of_ota_sessions++;
                }
                break;
              }
            }
          }
        }
        break;

      default:
        ret = E_DATA_INVALID;
        break;
    }
  }
  else
  {
    UIM_MSG_HIGH_0("qmi_pbm_if_fill_ecc_list : ecc_val passed is NULL");
    ret = E_FAILURE;
  }

  return ret;

} /* qmi_pbm_if_fill_ecc_list */
/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_ECC()

  DESCRIPTION
    Get Emergency numbers from PBM depending on the Session & Field ID passed.
    PBM_FIELD_NONE, PBM_FIELD_HARDCODED_ECC are passed as Field ID
    If PBM_FIELD_NONE is passed, get all the ECC (NV, SIM, Hardcoded,
    network). PBM_FIELD_HARDCODED_ECC just gets the hardcoded ECC which
    are applicable when sim is absent.

  PARAMETERS
    pbm_field_id_e_type  pbm_field_id

  RETURN VALUE
    IxErrnoType

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
IxErrnoType qmi_pbm_if_get_ecc_for_session
(
  uint8                     session_type,
  pbm_field_id_e_type       pbm_field_id,
  qmi_pbm_if_ecc_list_type *pbm_ecc_ptr,
  pbm_record_id_type       *rec_id_list
)
{
  int                 num_fields    = 0;
  int                 offset        = 0;
  int                 i             = 0;
  uint16              cat           = 0;
  uint8               emer_categ    = 0;
  uint8               handle        = 0;
  uint32              k             = 0;
  uint32              data_buf_size = 0;
  IxErrnoType         ret_errno     = E_SUCCESS;
  boolean             rec_id_found  = FALSE;  
  char               *num           = NULL;
  pbm_field_s_type   *data_buf      = NULL;
  pbm_record_id_type  rec_id        = 0;
  pbm_phonebook_type  pb_id;

  pb_id.device_type = PBM_ECC;
  pb_id.pb_category = qmi_pbm_if_derive_pb_category_from_session(session_type);
  pb_id.prov_type   = qmi_pbm_if_derive_prov_type_from_session(session_type);
  pb_id.slot_id     = qmi_pbm_if_derive_slot_from_session(session_type);

  /*Trigger the PBM record list formation the ECC records matching the cache, category, field id*/
  if(PBM_SUCCESS == pbm_session_enum_rec_init_ext(pb_id, (uint16)PBM_CAT_ECC, pbm_field_id, NULL, 0,0,&handle) )
  {
    /*The record list in PBM is formed. Get the record ids one by one*/
    while(PBM_SUCCESS == pbm_session_enum_next_rec_id_ext(&rec_id,handle))
    {
      k = 0 ;
      rec_id_found = FALSE ;

      while(rec_id_list[k] != 0)
      {
        if(rec_id == rec_id_list[k++])
        {
          rec_id_found = TRUE;
          break;
        }
      }

      if(rec_id_found)
      {
        continue;
      }

      rec_id_list[k] = rec_id;

      /*Get the total size of all the fields in the record using the record id*/
      data_buf_size = pbm_session_calculate_fields_size_from_id(rec_id);
      if ((data_buf = QMI_PBM_IF_MEM_ALLOC(data_buf_size)) == NULL)
      {
        pbm_session_enum_rec_init_ext_free_handle(handle);
        return E_FAILURE;
      }
      memset(data_buf,'\0',data_buf_size);

      /*read the record*/
      if(PBM_SUCCESS == pbm_session_record_read(rec_id,&cat,&num_fields,(uint8*)data_buf, data_buf_size))
      {
        emer_categ = 0;
        for(i = 0; i < num_fields;i++)
        {
          if((data_buf[i].field_type == PBM_FT_BYTE) && (data_buf[i].field_id == PBM_FIELD_ECC_SERVICE_CATEGORY))
          {
            /*extract the number from the record field*/
            offset = data_buf[i].buffer_offset;
            emer_categ = *((uint8 *)&(data_buf[i]) + offset);
            break;
          }
        }
        for(i = 0; i < num_fields;i++)
        {
          /*Check if the field type is that of ECC number.
          Network ECC are not handled here to avoid redundancy*/
          if(data_buf[i].field_type == PBM_FT_PHONE)
          {
            /*extract the number from the record field*/
            offset = data_buf[i].buffer_offset;
            num = (char *)&(data_buf[i]) + offset;
            /*Store the ECC num in list. */
            if( qmi_pbm_if_fill_ecc_list(session_type,data_buf[i].field_id,&emer_categ, num, data_buf[i].data_len,pbm_ecc_ptr) != E_SUCCESS )
            {
              break;
            }
          }
        }
      }
      QMI_PBM_IF_MEM_FREE(data_buf);
    }
    pbm_session_enum_rec_init_ext_free_handle(handle);
  }
  else
  {
    ret_errno = E_FAILURE;
  }

  return ret_errno;
} /* qmi_pbm_if_get_ecc_for_session */


/*===========================================================================
  FUNCTION QMI_PBM_IF_IS_PB_READY()

  DESCRIPTION
    Check if phonebook is ready

  PARAMETERS
    sess_type        : Session Type
    pb_type          : Phonebook Type

  RETURN VALUE
    TRUE if phonebook is ready. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_if_is_pb_ready
(
  uint8   sess_type,
  uint16  pb_type
)
{
  boolean   ret = FALSE;
  uint8     i   = 0;

  for(i = 0; i < QMI_PBM_IF_MAX_SESSIONS; i++)
  {
    if(pbm_if_info.sess_data[i].sess_type == sess_type)
    {
      if((pbm_if_info.sess_data[i].valid_pb_mask & pb_type) == pb_type)
      {
        ret = TRUE;
        break;
      }
    }
  }
  return ret;
} /* qmi_pbm_if_is_pb_ready */


/*=========================================================================

  FUNCTION:  qmi_pbm_populate_ecc_list

===========================================================================*/
/*!
    @brief
    Update the local list of emergency numbers based on the field id

    @return
    errno_enum_type
*/
/*=========================================================================*/
qmi_pbm_if_ecc_list_type* qmi_pbm_populate_ecc_list(
  void
)
{
  IxErrnoType                ret_err              = E_FAILURE;
  uint32                     rec_id_list_max_size = 0;
  uint8                      i                    = 0;
  qmi_pbm_if_ecc_list_type  *pbm_ecc_ptr          = NULL;
  pbm_record_id_type        *rec_id_list          = NULL;

  UIM_MSG_MED_0("Inside qmi_pbm_populate_ecc_list");

  pbm_ecc_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_ecc_list_type));

  if(pbm_ecc_ptr == NULL)
  {
    return NULL;
  }

  rec_id_list_max_size = (6*QMI_PBM_IF_MAX_CARD_ECC) + QMI_PBM_IF_MAX_HCODED_ECC_EXT + PBM_NV_EMERGENCY_NUMBERS;
  rec_id_list = QMI_PBM_IF_MEM_ALLOC(rec_id_list_max_size * sizeof(pbm_record_id_type));
  if(rec_id_list == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_ecc_ptr);
    return NULL;
  }

  memset(rec_id_list, 0, rec_id_list_max_size * sizeof(pbm_record_id_type));
  memset(pbm_ecc_ptr, 0, sizeof(qmi_pbm_if_ecc_list_type));

  for(i = QMI_PBM_IF_SESSION_GW_PRIMARY; i < QMI_PBM_IF_MAX_SESSIONS; i++)
  {
    pbm_ecc_ptr->card_ecc[i].sess_type = QMI_PBM_IF_SESSION_MAX;
    pbm_ecc_ptr->ota_ecc[i].sess_type = QMI_PBM_IF_SESSION_MAX;
  }

  for(i = QMI_PBM_IF_SESSION_GW_PRIMARY; i < QMI_PBM_IF_SESSION_MAX; i++)
  {
    if(qmi_pbm_if_is_session_type_valid(i))
    {
      if((ret_err = qmi_pbm_if_get_ecc_for_session( i, PBM_FIELD_NONE , pbm_ecc_ptr , rec_id_list)) != E_SUCCESS)
      {
        UIM_MSG_ERR_1("Failure in getting ECC from PBM for session 0x%x" , i);
      }
    }
  }

  QMI_PBM_IF_MEM_FREE(rec_id_list);

  return pbm_ecc_ptr;
}/* qmi_pbm_populate_ecc_list */


/*===========================================================================
  FUNCTION QMI_PBM_IF_INDICATION_REGISTER()

  DESCRIPTION
    Send emergency list, phonebook ready indications

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_indication_register
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  qmi_pbm_if_ecc_list_type   *pbm_ecc_ptr=NULL;

  ASSERT(params_ptr != NULL);

  UIM_MSG_HIGH_0("qmi_pbm_if_indication_register");

  if(params_ptr->data.ind_reg.send_pb_ready_ind && qmi_pbm_if_is_any_session_init_done())
  {
    /* Check if there are any sessions with all the phonebooks ready.
    If so send a all_pb_init_done indication */
    qmi_pbm_if_send_all_pb_init_done_ind(QMI_PBM_IF_SESSION_MAX);
  }

  if(params_ptr->data.ind_reg.send_pb_ready_ind)
  {
    qmi_pbm_if_sim_ready_ind_s sim_ready_ind;
    if( pbm_if_info.sim_ready[QMI_PBM_SLOT_1-1] == TRUE )
    {
      sim_ready_ind.slot_id = QMI_PBM_SLOT_1;
      qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_SIM_READY_IND, &sim_ready_ind, sizeof(sim_ready_ind));
    }
    if( pbm_if_info.sim_ready[QMI_PBM_SLOT_2-1] == TRUE )
    {
      sim_ready_ind.slot_id = QMI_PBM_SLOT_2;
      qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_SIM_READY_IND, &sim_ready_ind, sizeof(sim_ready_ind));
    }
#ifdef FEATURE_TRIPLE_SIM
    if( pbm_if_info.sim_ready[QMI_PBM_SLOT_3-1] == TRUE )
    {
      sim_ready_ind.slot_id = QMI_PBM_SLOT_3;
      qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_SIM_READY_IND, &sim_ready_ind, sizeof(sim_ready_ind));
    }
#endif
  }

  if(params_ptr->data.ind_reg.send_emer_list_ind)
  {
    /* send the list of applicable emergency numbers */
    UIM_MSG_MED_0("Inside qmi_pbm_if_indication_register : Sending Emergency list Ind");

    pbm_ecc_ptr = qmi_pbm_populate_ecc_list();

    if(pbm_ecc_ptr != NULL)
    {
      qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_EMERGENCY_LIST_IND, pbm_ecc_ptr, sizeof(qmi_pbm_if_ecc_list_type));
    }
  }
}/* qmi_pbm_if_indication_register */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_PB_CAPABILITIES()

  DESCRIPTION
    Get the phonebook capabilities of a specific phonebook.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_pb_capabilities
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  uint8                                  sess_type = 0;
  uint16                                 pb_type   = 0;
  pbm_return_type                        ret       = PBM_SUCCESS;
  qmi_error_e_type                       err_val   = QMI_ERR_NONE;
  qmi_pbm_if_get_pb_capabilities_resp_s  caps_rsp;

  ASSERT(params_ptr != NULL);

  sess_type = params_ptr->data.get_pb_caps.session_type;
  pb_type = params_ptr->data.get_pb_caps.pb_type;

  UIM_MSG_HIGH_2("qmi_pbm_if_get_pb_capabilities(): Sess_type 0x%x, pb_type 0x%x",sess_type, pb_type );

  /* check if session type, pb type are valid */
  if((err_val = qmi_pbm_if_check_session_pb_validity(sess_type, pb_type)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err_val, NULL, 0, FALSE);
    return;
  }

  /* fill the phonebook capabilities */
  ret = qmi_pbm_if_fill_pb_caps(sess_type, pb_type, &caps_rsp.pb_caps);
  if(ret != PBM_SUCCESS)
  {
    UIM_MSG_ERR_1("qmi_pbm_if_fill_pb_caps: returned 0x%x", ret);
    err_val = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err_val, NULL, 0, FALSE);
  }
  else
  {
    caps_rsp.error = err_val;
    caps_rsp.sess_type = sess_type;
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err_val, (void*)&caps_rsp, sizeof(caps_rsp), FALSE);
  }
}/* qmi_pbm_if_get_pb_capabilities */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_ALL_PB_CAPABILITIES()

  DESCRIPTION
    Get the phonebook capabilities of all phonebooks in from all sessions.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_all_pb_capabilities
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  boolean                                      card_present = FALSE;
  uint16                                       pb_type      = 0;
  uint8                                        sess_type    = 0;
  uint8                                        i            = 0;
  uint8                                        j            = 0;
  uint8                                        k            = 0;
  uint8                                        sess_cnt     = 0;
  pbm_return_type                              ret          = PBM_SUCCESS;
  qmi_error_e_type                             err_val      = QMI_ERR_NONE;
  qmi_pbm_if_get_all_pb_capabilities_resp_s   *all_pb_caps  = NULL;

  ASSERT(params_ptr != NULL);

  UIM_MSG_HIGH_0("qmi_pbm_if_get_all_pb_capabilities");

  all_pb_caps = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_get_all_pb_capabilities_resp_s));
  if(all_pb_caps == NULL)
  {
    return;
  }

  memset(all_pb_caps,0,sizeof(qmi_pbm_if_get_all_pb_capabilities_resp_s));
  /* Get the number of sessions */
  all_pb_caps->num_of_sessions = pbm_if_info.num_of_sessions;
  for(i = (uint8)QMI_PBM_SLOT_1; i <= (uint8)QMI_MAX_SLOTS; i++)
  {
    if(pbm_is_card_present_in_slot(i))
    {
      card_present = TRUE;
      break;
    }
  }

  if(card_present)
  {
    /* Loop through the number of sessions and fill the caps of pbs */
    for(i = 0; i < QMI_PBM_IF_MAX_SESSIONS; i++)
    {
      if(pbm_if_info.sess_data[i].sess_type != QMI_PBM_IF_SESSION_MAX )
      {
        sess_type = pbm_if_info.sess_data[i].sess_type;
        for(k = 0; k < QMI_PBM_IF_MAX_PBS; k++)
        {
          pb_type = (1 << k);
          /* check if the phonebook is ready/valid */
          if(((pb_type & pbm_if_info.sess_data[i].valid_pb_mask) == pb_type) && j < QMI_PBM_IF_MAX_PBS )
          {
            /* Fill each of the phonebook caps */
            ret = qmi_pbm_if_fill_pb_caps(sess_type, pb_type, &all_pb_caps->sess_caps[i].pb_caps[j]);
            if(ret != PBM_SUCCESS)
            {
              UIM_MSG_ERR_1("qmi_pbm_if_fill_pb_caps: returned 0x%x", ret);
              err_val = qmi_pbm_if_map_pbm_err_to_qmi(ret);
              qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err_val,NULL,0,FALSE);
              QMI_PBM_IF_MEM_FREE(all_pb_caps);
              return;
            }
            j++;
          }
        }/* for k*/

        all_pb_caps->sess_caps[i].session_type = sess_type;
        all_pb_caps->sess_caps[i].num_of_pbs = j;
        j = 0;
        sess_cnt++;
      }
    }/* for i*/
  }
  else
  {
    err_val = QMI_ERR_NO_SIM;
  }

  if(sess_cnt == 0)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err_val, NULL, 0, FALSE);
  }
  else
  {
    all_pb_caps->num_of_sessions = sess_cnt;
    all_pb_caps->error = err_val;
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err_val, (void*)all_pb_caps, sizeof(*all_pb_caps), FALSE);
  }

  QMI_PBM_IF_MEM_FREE(all_pb_caps);
} /* qmi_pbm_if_get_all_pb_capabilities */


/*===========================================================================
  FUNCTION QMI_PBM_IF_READ_RECORDS()

  DESCRIPTION
    Read records with record ids between start and end ids.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_read_records
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  qmi_pbm_if_read_records_s              *data                  = NULL;
  uint16                                  rec_id                = 0;
  uint16                                  rec_cnt               = 0;
  uint16                                  last_non_empty_rec_id = 0;
  qmi_cmd_buf_type                       *cmd_buf_p             = NULL;
  qmi_pbm_if_cmd_e_type                   ind_type              = QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND;
  qmi_error_e_type                        err                   = QMI_ERR_NONE;
  qmi_pbm_if_internal_record_read_ind_s   int_ind;
  qmi_pbm_if_read_records_resp_s          resp;
  pbm_phonebook_type                      pb_info;
  pbm_record_id_type                      pbm_rec_id;

  ASSERT(params_ptr != NULL);
  cmd_buf_p =(qmi_cmd_buf_type*)params_ptr->header.handle;
  ASSERT(cmd_buf_p != NULL);

  data = (qmi_pbm_if_read_records_s*)&params_ptr->data.read_recs;

  UIM_MSG_HIGH_4("qmi_pbm_if_read_records : Session type 0x%x, Phonebook type 0x%x, start_id = 0x%x, end_id = 0x%x",
                  data->session_type, data->pb_type, data->start_id, data->end_id);

  /* check if session type, pb type are valid */
  if((err = qmi_pbm_if_check_session_pb_validity(data->session_type, data->pb_type)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err, NULL, 0, FALSE);
    return;
  }

  /* Check if the record ids are proper and in the valid range */
  if( (data->start_id > data->end_id) )
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_ARG, NULL, 0, FALSE);
    return;
  }
  if(!qmi_pbm_if_is_rec_id_in_range(data->session_type, data->pb_type, data->start_id) ||
     !qmi_pbm_if_is_rec_id_in_range(data->session_type, data->pb_type, data->end_id))
  {
    uint16       max_recs = 0;

    max_recs = qmi_pbm_if_get_max_records(data->session_type, data->pb_type);
    UIM_MSG_HIGH_1("Maximum records possible for the phonebook is %d",max_recs );
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_ID, NULL, 0, FALSE);
    return;
  }

  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(data->session_type, data->pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_PB_DELETE_IN_PROG, NULL, 0, FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(data->session_type, data->pb_type, &pb_info);

  for(rec_id = data->start_id; rec_id <= data->end_id;rec_id++)
  {
    pbm_rec_id = pbm_session_location_to_record_id(pb_info,rec_id);
    if(pbm_session_calculate_fields_size_from_id(pbm_rec_id) != 0)
    {
      rec_cnt++;
      last_non_empty_rec_id = rec_id;
    }
  }

  resp.error = QMI_ERR_NONE;
  resp.sess_type = data->session_type;
  resp.pb_type =  data->pb_type;
  resp.rec_cnt = rec_cnt;

  if(cmd_buf_p->cmd_type == PBMI_CMD_VAL_READ_RECORDS_EXT)
  {
    ind_type = QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND;
  }
  else
  {
    ind_type = QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND;
  }
  qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NONE,&resp,sizeof(resp),FALSE);

  if(rec_cnt > 0)
  {
    memset(&int_ind, 0, sizeof(int_ind));
    int_ind.sess_type = data->session_type;
    int_ind.pb_type = data->pb_type;
    int_ind.start_id = data->start_id;
    int_ind.end_id = last_non_empty_rec_id;
    qmi_pbm_if_send_ind(ind_type,&int_ind,sizeof(int_ind));
  }
} /* qmi_pbm_if_read_records */


/*===========================================================================
  FUNCTION qmi_pbm_if_read_records_ext_undecoded()

  DESCRIPTION
    Read records with record ids between start and end ids.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_read_records_ext_undecoded
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  qmi_pbm_if_read_records_ext_undecoded_s                 *data;
  qmi_error_e_type                                         err;
  qmi_pbm_if_internal_record_read_ext_undecoded_ind_s      int_ind;
  qmi_pbm_if_read_records_ext_undecoded_resp_s             resp;
  pbm_phonebook_type                                       pb_info;
  pbm_record_id_type                                       pbm_rec_id            = 0;
  uint16                                                   rec_id                = 0;
  uint16                                                   rec_cnt               = 0;
  uint16                                                   last_non_empty_rec_id = 0;
  qmi_pbm_if_cmd_e_type                                    ind_type              = 
                                                           QMI_PBM_IF_CMD_INTERNAL_READ_RECORDS_EXT_UNDECODED_IND;
  qmi_cmd_buf_type                                        *cmd_buf_p             = NULL;

  ASSERT(params_ptr != NULL);
  cmd_buf_p =(qmi_cmd_buf_type*)params_ptr->header.handle;
  ASSERT(cmd_buf_p != NULL);

  data = (qmi_pbm_if_read_records_ext_undecoded_s*)&params_ptr->data.read_recs_ext_undecoded;

  UIM_MSG_HIGH_4("qmi_pbm_if_read_records_ext_undecoded : Session Type %d, Phonebook type %d, start_id=%d, end_id=%d ",
                  data->session_type, data->pb_type, data->start_id, data->end_id);

  /* check if session type, pb type are valid */
  if((err = qmi_pbm_if_check_session_pb_validity(data->session_type, data->pb_type)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err, NULL, 0, FALSE);
    return;
  }

  /* Check if the record ids are proper and in the valid range */
  if((data->start_id > data->end_id))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_ARG, NULL, 0, FALSE);
    return;
  }

  if(!qmi_pbm_if_is_rec_id_in_range(data->session_type, data->pb_type, data->start_id) ||
     !qmi_pbm_if_is_rec_id_in_range(data->session_type, data->pb_type, data->end_id))
  {
    uint16 max_recs = 0;

    max_recs = qmi_pbm_if_get_max_records(data->session_type, data->pb_type);
    UIM_MSG_MED_1("Maximum records possible for the phonebook is 0x%x", max_recs);
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_ID, NULL, 0, FALSE);
    return;
  }

  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(data->session_type, data->pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_PB_DELETE_IN_PROG,NULL,0,FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(data->session_type, data->pb_type, &pb_info);

  for(rec_id = data->start_id; rec_id <= data->end_id;rec_id++)
  {
    pbm_rec_id = pbm_session_location_to_record_id(pb_info, rec_id);
    if(pbm_session_calculate_undecoded_fields_size_from_id(pbm_rec_id) != 0)
    {
      rec_cnt++;
      last_non_empty_rec_id = rec_id;
    }
  }

  resp.error = QMI_ERR_NONE;
  resp.sess_type = data->session_type;
  resp.pb_type = data->pb_type;
  resp.rec_cnt = rec_cnt;

  qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NONE, &resp, sizeof(resp), FALSE);

  if(rec_cnt > 0)
  {
    memset(&int_ind, 0, sizeof(int_ind));
    int_ind.sess_type = data->session_type;
    int_ind.pb_type = data->pb_type;
    int_ind.start_id = data->start_id;
    int_ind.end_id = last_non_empty_rec_id;
    qmi_pbm_if_send_ind(ind_type, &int_ind, sizeof(int_ind));
  }
}/* qmi_pbm_if_read_records_ext_undecoded */


/*===========================================================================
  FUNCTION QMI_PBM_IF_READ_PBSET_CAPS()

  DESCRIPTION
    Read records with record ids between start and end ids.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_read_pbset_caps
(
  qmi_pbm_if_info_item_s  *const params_ptr
)
{
  qmi_pbm_if_read_pbset_caps_s                  *data    = NULL;
  pbm_return_type                                pbm_ret = PBM_ERROR;
  pbm_all_usim_field_capabilities_s_type        *info    = NULL;
  qmi_error_e_type                               err     = QMI_ERR_NONE;
  qmi_pbm_if_internal_pbset_caps_read_ind_s      int_ind;
  qmi_pbm_if_read_pbset_caps_resp_s              resp;
  pbm_phonebook_type                             pb_info;

  ASSERT( params_ptr != NULL );

  data = (qmi_pbm_if_read_pbset_caps_s*)&params_ptr->data.read_pbset_caps;

  UIM_MSG_HIGH_3("qmi_pbm_if_read_pbset_caps Session Type 0x%x, start_id = 0x%x, end_id = 0x%x ",
                  data->session_type, data->start_id, data->end_id);

  /* check if session type, pb type are valid */
  if((err = qmi_pbm_if_check_session_pb_validity(data->session_type, QMI_PBM_IF_PB_ADN)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err, NULL, 0, FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(data->session_type, QMI_PBM_IF_PB_ADN, &pb_info);
  info = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_all_usim_field_capabilities_s_type));
  if(info == NULL)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
    return;
  }
  pbm_ret = pbm_get_phonebook_usim_fields_capibilities(pb_info, info);
  if(pbm_ret != PBM_SUCCESS)
  {
    UIM_MSG_ERR_1("pbm_get_phonebook_usim_fields_capibilities: returned 0x%x", pbm_ret);
    QMI_PBM_IF_MEM_FREE(info);
    qmi_pbm_if_send_response(params_ptr->header.handle,
                             params_ptr->header.cmd,
                             qmi_pbm_if_map_pbm_err_to_qmi(pbm_ret),
                             NULL, 0, TRUE);
    return;
  }

  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(data->session_type, QMI_PBM_IF_PB_ADN))
  {
    QMI_PBM_IF_MEM_FREE(info);
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_PB_DELETE_IN_PROG, NULL, 0, FALSE);
    return;
  }
  if(data->start_id == 0)
  {
    data->start_id = 1;
  }
  if(data->end_id == 0)
  {
    data->end_id = info->total_pbsets;
  }
  if(data->start_id < 1 || data->start_id > info->total_pbsets ||
     data->end_id < 1 || data->end_id > info->total_pbsets ||
     data->start_id > data->end_id)
  {
    QMI_PBM_IF_MEM_FREE(info);
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_ID, NULL, 0, FALSE);
    return;
  }

  resp.error = QMI_ERR_NONE;
  resp.sess_type = data->session_type;
  resp.pbset_cnt = info->total_pbsets;
  qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NONE, &resp, sizeof(resp), FALSE);

  if(resp.pbset_cnt > 0)
  {
    memset(&int_ind, 0, sizeof(int_ind));
    int_ind.sess_type = data->session_type;
    int_ind.start_id = data->start_id;
    int_ind.end_id = data->end_id;
    int_ind.usim_cap_info = info;
    qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_INTERNAL_PBSET_CAPS_READ_IND, &int_ind, sizeof(int_ind));
  }
  else
  {
    QMI_PBM_IF_MEM_FREE(info);
  }
} /* qmi_pbm_if_read_pbset_caps */


/*===========================================================================
  FUNCTION qmi_copy_pbm_rec_to_qmi_ind ()

  DESCRIPTION
    Copy the PBM record content to QMI READ indication to clients.

  PARAMETERS
    data_buf        : the pbm_field_s_type pointer
    int                    num_fields,
    qmi_pbm_if_rec_data_s *rec_data,
    qmi_pbm_if_cmd_e_type  cmd_type

  RETURN VALUE
    qmi_result_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_result_e_type qmi_copy_pbm_rec_to_qmi_ind
(
  pbm_field_s_type          *data_buf,
  int                        num_fields,
  qmi_pbm_if_rec_data_s     *rec_data,
  qmi_pbm_if_cmd_e_type      cmd_type)
{
  uint8                         k                                                  = 0;
  uint8                         ad_num_len                                         = 0;
  uint8                         ad_num_cnt                                         = 0;
  uint8                         sf_cnt                                             = 0;
  uint8                         email_cnt                                          = 0;
  uint8                         grp_cnt                                            = 0;
  uint8                         num_len                                            = 0;
  uint8                        *ton                                                = NULL;
  uint8                        *npi                                                = NULL;
  uint8                        *grp_id                                             = NULL;
  uint8                        *aas_id                                             = NULL;
  uint8                        *temp                                               = NULL;
  void                         *field_val                                          = NULL;
  void                         *sub_field_val                                      = NULL;
  char                         *ad_num                                             = NULL;
  char                         *f_val                                              = NULL;
  pbm_field_s_type             *sfields_ptr                                        = NULL;
  pbm_complex_field_s_type     *cmplx                                              = NULL;
  boolean                       is_ad_num_data_present                             = FALSE;
  uint32                        anr_sub_field_sizes[QMI_PBM_IF_MAX_ANR_SUB_FIELDS] = {0};
  uint32                        cmplx_size                                         = 0;
  uint32                        num_sfields                                        = 0;

  for(k=0;k<num_fields;k++)
  {
    field_val = pbm_get_field_data_ptr(data_buf,k);
    if(field_val == NULL)
    {
      UIM_MSG_ERR_1("Field data[0x%x] from PBM is NULL", k);
      return QMI_RESULT_FAILURE;
    }
    switch(data_buf[k].field_id)
    {
      case PBM_FIELD_NUMBER_TYPE:
        ton = (uint8*)field_val;
        UIM_MSG_HIGH_1("Number type value from PBM is %d", *ton);
        rec_data->num_info.num_type = ((*ton)& 0x7);
        break;

      case PBM_FIELD_NUMBER_PLAN:
        npi = (uint8*)field_val;
        UIM_MSG_HIGH_1("Number plan value from PBM is %d", *npi);
        rec_data->num_info.num_plan= qmi_pbm_if_map_npi_to_qmi((qmi_pbm_if_amss_npi_e_type)*npi);
        break;

      case PBM_FIELD_GSM_TONNPI:
        /* Amss pbm normally gives the ton, npi together */
        temp =(uint8*) field_val;
        UIM_MSG_HIGH_1("TONNPI value from PBM is %d",*temp );
        if(*temp == 0xFF)
        {
          break;
        }

        rec_data->num_info.num_plan = qmi_pbm_if_map_npi_to_qmi((qmi_pbm_if_amss_npi_e_type)((*temp) & 0xF));
        rec_data->num_info.num_type = (*temp >>4)& 0x7;
        break;

      case PBM_FIELD_PHONE_GENERIC:
        f_val = (char*)field_val;
        num_len = (uint8)data_buf[k].data_len;
        /* Phone number will be in Ascii. Copy as it is */
        UIM_PBM_MSG_SPRINTF_2("Phone number of length %d from PBM is %s",num_len,f_val );
        /* Exclude the terminating nul char */
        num_len--;
        memscpy(rec_data->num_info.num, sizeof(rec_data->num_info.num), f_val, num_len);
        rec_data->num_info.len = num_len ;
        break;

      case PBM_FIELD_NAME:
        /* Name will be in UCS2. Copy as it is */
        if((cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND ||
            cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND)&&
            data_buf[k].data_len > QMI_PBM_IF_MAX_TEXT_LEN)
        {
          data_buf[k].data_len = QMI_PBM_IF_MAX_TEXT_LEN;
          rec_data->name_info.len = QMI_PBM_IF_MAX_TEXT_LEN - 2;
        }
        else if(cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND ||
                cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND)
        {
          /* Exclude the terminating nul char(of 2 bytes) */
          /* convert the byte length to unit16 length
             since alpha in EXT_IND is based on uint16 */
          rec_data->name_info.len = MIN(QMI_PBM_IF_MAX_TEXT_EXT_LEN,(data_buf[k].data_len-2)/2);
        }
        else if(cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND ||
                cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND)
        {
          /* Exclude the terminating nul char(of 2 bytes) */
          rec_data->name_info.len = data_buf[k].data_len-2;
        }

        memscpy(rec_data->name_info.text, sizeof(rec_data->name_info.text), field_val,(data_buf[k].data_len-2));
        break;

      case PBM_FIELD_OTHERNAME:
        /* Name will be in UCS2. Copy as it is */
        if((cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND ||
            cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND)
           && data_buf[k].data_len > QMI_PBM_IF_MAX_TEXT_LEN)
        {
          data_buf[k].data_len = QMI_PBM_IF_MAX_TEXT_LEN;
          rec_data->sname_info.len = QMI_PBM_IF_MAX_TEXT_LEN - 2;
        }
        else if(cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND ||
                cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND)
        {
          /* Exclude the terminating nul char(of 2 bytes) */
          /* convert the byte length to unit16 length
             since alpha in EXT_IND is based on uint16 */
          rec_data->sname_info.len =
             MIN(QMI_PBM_IF_MAX_TEXT_EXT_LEN,(data_buf[k].data_len-2)/2);
        }
        else if(cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND ||
                cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND)
        {
          /* Exclude the terminating nul char(of 2 bytes) */
          rec_data->sname_info.len = data_buf[k].data_len-2;
        }
        memscpy(rec_data->sname_info.text, sizeof(rec_data->sname_info.text), field_val, (data_buf[k].data_len-2));
        break;

      case PBM_FIELD_GRPNAME_ID:
        if(grp_cnt >= QMI_PBM_IF_MAX_GRP)
        {
          UIM_MSG_HIGH_1("Reached the max GRP count %d. skip this field",grp_cnt);
          continue;
        }
        grp_id = (uint8 *)field_val;
        UIM_MSG_HIGH_1("Group id from from PBM is %d", *grp_id);
        /* Group id is one byte as per 31.102 */
        rec_data->grp_id[grp_cnt++] =(uint8) *grp_id;
        break;

      case PBM_FIELD_EMAIL:
        /* Name will be in UCS2. Copy as it is */
        if((email_cnt < QMI_PBM_IF_MAX_EMAIL ) &&
           (cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND ||
            cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND)&&
            (data_buf[k].data_len > QMI_PBM_IF_MAX_TEXT_LEN))
        {
          data_buf[k].data_len = QMI_PBM_IF_MAX_TEXT_LEN;
          rec_data->email_data[email_cnt].len = QMI_PBM_IF_MAX_TEXT_LEN - 2;
        }
        else if(cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND ||
                cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND)
        {
          /* Exclude the terminating nul char(of 2 bytes) */
          /* convert the byte length to unit16 length
             since alpha in EXT_IND is based on uint16 */
          rec_data->email_data[email_cnt].len =
             MIN(QMI_PBM_IF_MAX_TEXT_EXT_LEN,(data_buf[k].data_len-2)/2);
        }
        else if(cmd_type == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND ||
                cmd_type == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND)
        {
          /* Exclude the terminating nul char(of 2 bytes) */
          rec_data->email_data[email_cnt].len = data_buf[k].data_len-2;
        }
        memscpy(rec_data->email_data[email_cnt].text, sizeof(rec_data->email_data[email_cnt].text), field_val, (data_buf[k].data_len-2));
        email_cnt++;
        break;

      case PBM_FIELD_HIDDEN:
        rec_data->is_hidden = (*((uint8*)field_val) == 1) ? TRUE : FALSE;
        UIM_MSG_HIGH_1("Hidden status from PBM is %d", rec_data->is_hidden );
        break;

      case PBM_FIELD_COMPLEX:
        if(ad_num_cnt >= QMI_PBM_IF_MAX_AD_NUM)
        {
          UIM_MSG_HIGH_1("Reached the max ANR count %d. skip this field",ad_num_cnt);
          continue;
        }
        cmplx = (pbm_complex_field_s_type *)field_val;
        num_sfields = pbm_complex_fill_sizes(anr_sub_field_sizes, QMI_PBM_IF_MAX_ANR_SUB_FIELDS,
                                             cmplx);

        /* Allocate the memory for the sub fields */
        if((num_sfields == 0)||
            (!pbm_allocate_fields((uint8**)&sfields_ptr, (uint32 *)&cmplx_size, num_sfields, (const uint32 *)anr_sub_field_sizes)))
        {
          UIM_MSG_HIGH_1("Cannot extract %d sub fields from complex field ", num_sfields);
          continue;
        }

        /* Unwrap the complex field */
        if(pbm_get_fields_from_complex(cmplx, sfields_ptr,(int*) &num_sfields) == PBM_SUCCESS)
        {
          for(sf_cnt = 0; sf_cnt < num_sfields; sf_cnt++)
          {
            sub_field_val = pbm_get_field_data_ptr(sfields_ptr,sf_cnt);
            if(sub_field_val == NULL)
            {
              UIM_MSG_HIGH_1("Sub Field data[%d] from PBM is NULL", sf_cnt);
              pbm_free_fields((uint8 **)&sfields_ptr,num_sfields);
              return QMI_RESULT_FAILURE;
            }
            switch(sfields_ptr[sf_cnt].field_id)
            {
              case PBM_FIELD_ADDITIONAL_NUMBER:
                ad_num = (char *) sub_field_val;
                ad_num_len = (uint8)sfields_ptr[sf_cnt].data_len;
                /* Additional number will be in ascii. copy as it is */
                memscpy(rec_data->ad_num_data[ad_num_cnt].ad_num.num, sizeof(rec_data->ad_num_data[ad_num_cnt].ad_num.num), ad_num, ad_num_len);
                /* Exclude the terminating nul char */
                rec_data->ad_num_data[ad_num_cnt].ad_num.len = ad_num_len - 1;
                UIM_PBM_MSG_SPRINTF_1("Additional number from PBM is %s", ad_num);
                is_ad_num_data_present = TRUE;
                break;

              case PBM_FIELD_GSM_TONNPI:
                temp =(uint8*) sub_field_val;
                UIM_MSG_HIGH_1("TONNPI for ANR from PBM is %d",*temp );
                rec_data->ad_num_data[ad_num_cnt].ad_num.num_plan = qmi_pbm_if_map_npi_to_qmi((qmi_pbm_if_amss_npi_e_type)((*temp) & 0xF));
                rec_data->ad_num_data[ad_num_cnt].ad_num.num_type = (*temp >>4)& 0x7;
                break;

              case PBM_FIELD_ADDITIONAL_NUMBER_TYPE:
                aas_id = (uint8*)sub_field_val;
                UIM_MSG_HIGH_1("AAS location from PBM is %d",*aas_id );
                rec_data->ad_num_data[ad_num_cnt].tag_id = *aas_id;
                break;

              default:
                UIM_MSG_HIGH_1("Unsupported sub field %d of ANR",sfields_ptr[sf_cnt].field_id);
                break;
            }
          }
          if(is_ad_num_data_present)
          {
            ad_num_cnt++;
            is_ad_num_data_present = FALSE;
          }
        }
        pbm_free_fields((uint8 **)&sfields_ptr,num_sfields);
        break;

      default:
        UIM_MSG_HIGH_1("PBM Field id (%d) not supported", data_buf[k].field_id);
        break;
    } /* end of switch */
  }

  rec_data->ad_num_cnt = ad_num_cnt;
  rec_data->grp_cnt = grp_cnt;
  rec_data->email_cnt = email_cnt;

  return QMI_RESULT_SUCCESS;
}

static qmi_result_e_type qmi_copy_pbm_rec_to_qmi_undecoded_ind
                                  (pbm_field_s_type *data_buf,
                                   int num_fields,
                                   qmi_pbm_if_rec_data_undecoded_s *rec_data,
                                   qmi_pbm_if_cmd_e_type cmd_type)
{
  uint8  k, ad_num_len, ad_num_cnt,sf_cnt = 0;
  uint8  email_cnt, grp_cnt;
  void *field_val, *sub_field_val;
  uint8 *ton, *npi, *temp;
  char *ad_num, *f_val;
  uint8 *grp_id, *aas_id;
  uint8 num_len = 0;
  boolean is_ad_num_data_present= FALSE;
  pbm_complex_field_s_type *cmplx;
  uint32 anr_sub_field_sizes[QMI_PBM_IF_MAX_ANR_SUB_FIELDS] = {0};
  uint32 cmplx_size = 0, num_sfields=0;
  pbm_field_s_type *sfields_ptr;

    ad_num_cnt = 0;
    grp_cnt = 0;
    email_cnt = 0;


    for(k=0;k<num_fields;k++)
    {
      field_val = pbm_get_field_data_ptr(data_buf,k);
      if(field_val == NULL)
      {
        UIM_MSG_ERR_1("Field data[%d] from PBM is NULL", k);
        return QMI_RESULT_FAILURE;
      }
      switch(data_buf[k].field_id)
      {
        case PBM_FIELD_NUMBER_TYPE:
          ton = (uint8*)field_val;
          UIM_MSG_HIGH_1("Number type value from PBM is %d", *ton);
          rec_data->num_info.num_type = ((*ton)& 0x7);
          break;
        case PBM_FIELD_NUMBER_PLAN:
          npi = (uint8*)field_val;
          UIM_MSG_HIGH_1("Number plan value from PBM is %d", *npi);
          rec_data->num_info.num_plan= qmi_pbm_if_map_npi_to_qmi((qmi_pbm_if_amss_npi_e_type)*npi);
          break;
        case PBM_FIELD_GSM_TONNPI:
          /* Amss pbm normally gives the ton, npi together */
          temp =(uint8*) field_val;
          UIM_MSG_HIGH_1("TONNPI value from PBM is %d",*temp );
          rec_data->num_info.num_plan = qmi_pbm_if_map_npi_to_qmi((qmi_pbm_if_amss_npi_e_type)((*temp) & 0xF));
          rec_data->num_info.num_type = (*temp >>4)& 0x7;
          break;
        case PBM_FIELD_PHONE_GENERIC:
          f_val = (char*)field_val;
          num_len = (uint8)data_buf[k].data_len;
          /* Phone number will be in Ascii. Copy as it is */
          UIM_PBM_MSG_SPRINTF_2("Phone number of length %d from PBM is %s",num_len,f_val );
          /* If the number is international, skip the + as number type will be set
          to international */
          if( *f_val == '+' )
          {
            f_val++;
            num_len--;
          }

          /* Exclude the terminating nul char */
          num_len--;
          memscpy(rec_data->num_info.num,sizeof(rec_data->num_info.num), f_val,num_len);
          rec_data->num_info.len = num_len ;
          break;
        case PBM_FIELD_NAME:
          if( data_buf[k].data_len > QMI_PBM_IF_MAX_TEXT_EXT_LEN )
          {
            rec_data->name_info.len = QMI_PBM_IF_MAX_TEXT_EXT_LEN ;
          }
          else
          {
            rec_data->name_info.len = data_buf[k].data_len-1;
          }

          memscpy(rec_data->name_info.text,sizeof(rec_data->name_info.text), field_val,
                  data_buf[k].data_len-1);
          break;

        case PBM_FIELD_OTHERNAME:
          if( data_buf[k].data_len > QMI_PBM_IF_MAX_TEXT_EXT_LEN )
          {
            rec_data->sname_info.len = QMI_PBM_IF_MAX_TEXT_EXT_LEN ;
          }
          else
          {
            rec_data->sname_info.len = data_buf[k].data_len-1 ;
          }

          memscpy(rec_data->sname_info.text,sizeof(rec_data->sname_info.text), field_val,
                  data_buf[k].data_len-1);
          break;

        case PBM_FIELD_GRPNAME_ID:
          if( grp_cnt >= QMI_PBM_IF_MAX_GRP )
          {
            UIM_MSG_HIGH_1("Reached the max GRP count %d. skip this field",grp_cnt);
            continue;
          }
          grp_id = (uint8 *)field_val;
          UIM_MSG_HIGH_1("Group id from from PBM is %d", *grp_id);
          /* Group id is one byte as per 31.102 */
        rec_data->grp_id[grp_cnt++] =(uint8) *grp_id;
          break;
        case PBM_FIELD_EMAIL:
          if( email_cnt < QMI_PBM_IF_MAX_EMAIL )
          {
            if( data_buf[k].data_len > QMI_PBM_IF_MAX_TEXT_EXT_LEN )
            {
              rec_data->email_data[email_cnt].len = QMI_PBM_IF_MAX_TEXT_EXT_LEN ;
            }
            else
            {
              rec_data->email_data[email_cnt].len = data_buf[k].data_len-1 ;
            }
          }

          memscpy(rec_data->email_data[email_cnt].text, sizeof(rec_data->email_data[email_cnt].text),
                  field_val, data_buf[k].data_len-1);
          email_cnt++;
        break;

        case PBM_FIELD_HIDDEN:
          rec_data->is_hidden = (*((uint8*)field_val) == 1) ? TRUE : FALSE;
          UIM_MSG_HIGH_1("Hidden status from PBM is 0x%x", rec_data->is_hidden);
          break;

        case PBM_FIELD_COMPLEX:
          if(ad_num_cnt >= QMI_PBM_IF_MAX_AD_NUM)
          {
            UIM_MSG_HIGH_1("Reached the max ANR count 0x%x. skip this field",ad_num_cnt);
            continue;
          }
          cmplx = (pbm_complex_field_s_type *)field_val;
          num_sfields = pbm_complex_fill_sizes(anr_sub_field_sizes,
                                               QMI_PBM_IF_MAX_ANR_SUB_FIELDS,
                                               cmplx);

          /* Allocate the memory for the sub fields */
          if((num_sfields == 0)||(!pbm_allocate_fields((uint8**)&sfields_ptr, (uint32 *)&cmplx_size, num_sfields, (const uint32 *)anr_sub_field_sizes)))
          {
            UIM_MSG_HIGH_1("Cannot extract %d sub fields from complex field ", num_sfields);
             continue;
          }
          /* Unwrap the complex field */
          if(pbm_get_fields_from_complex(cmplx, sfields_ptr, (int*)&num_sfields) == PBM_SUCCESS)
          {
            for(sf_cnt = 0; sf_cnt < num_sfields; sf_cnt++)
            {
              sub_field_val = pbm_get_field_data_ptr(sfields_ptr, sf_cnt);
              if(sub_field_val == NULL)
              {
                UIM_MSG_HIGH_1("Sub Field data[0x%x] from PBM is NULL", sf_cnt);
                pbm_free_fields((uint8 **)&sfields_ptr, num_sfields);
                return QMI_RESULT_FAILURE;
              }
              switch(sfields_ptr[sf_cnt].field_id)
              {
                case PBM_FIELD_ADDITIONAL_NUMBER:
                  ad_num = (char *) sub_field_val;
                  ad_num_len = (uint8)sfields_ptr[sf_cnt].data_len;
                  /* Additional number will be in ascii. copy as it is */
                  memscpy(rec_data->ad_num_data[ad_num_cnt].ad_num.num, sizeof(rec_data->ad_num_data[ad_num_cnt].ad_num.num),
                           ad_num,ad_num_len);
                  /* Exclude the terminating nul char */
                  rec_data->ad_num_data[ad_num_cnt].ad_num.len = ad_num_len - 1;
                  UIM_PBM_MSG_SPRINTF_1("Additional number from PBM is %s", ad_num);
                  is_ad_num_data_present = TRUE;
                  break;

                case PBM_FIELD_GSM_TONNPI:
                  temp =(uint8*) sub_field_val;
                  UIM_MSG_HIGH_1("TONNPI for ANR from PBM is %d",*temp );
                  rec_data->ad_num_data[ad_num_cnt].ad_num.num_plan = qmi_pbm_if_map_npi_to_qmi((qmi_pbm_if_amss_npi_e_type)((*temp) & 0xF));
                  rec_data->ad_num_data[ad_num_cnt].ad_num.num_type = (*temp >>4)& 0x7;
                  break;

                case PBM_FIELD_ADDITIONAL_NUMBER_TYPE:
                  aas_id = (uint8*)sub_field_val;
                  UIM_MSG_HIGH_1("AAS location from PBM is %d",*aas_id );
                  rec_data->ad_num_data[ad_num_cnt].tag_id = *aas_id;
                  break;

                default:
                  UIM_MSG_HIGH_1("Unsupported sub field %d of ANR",sfields_ptr[sf_cnt].field_id);
                  break;
              }
            }
            if(is_ad_num_data_present)
            {
              ad_num_cnt++;
              is_ad_num_data_present = FALSE;
            }
          }
          pbm_free_fields((uint8 **)&sfields_ptr,num_sfields);
          break;

        default:
          UIM_MSG_HIGH_1("PBM Field id (%d) not supported", data_buf[k].field_id);
          break;
      } /* end of switch */
  }
  rec_data->ad_num_cnt = ad_num_cnt;
  rec_data->grp_cnt = grp_cnt;
  rec_data->email_cnt = email_cnt;
  return QMI_RESULT_SUCCESS;
}

/*===========================================================================
  FUNCTION QMI_PBM_IF_INTERNAL_RECORD_READ_IND()

  DESCRIPTION
    Send each RECORD_READ indication with a maximum data for 10 records.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_internal_record_read_ind
(
  qmi_pbm_if_info_item_s  *const params_ptr
)
{
  qmi_pbm_if_internal_record_read_ind_s intr_ind;
  qmi_pbm_if_record_read_ind_s *read_ind;
  uint8   i = 0;
  uint16 rec_id,category = 0;
  uint16 cmd_type;
  uint32 pbm_rec_id, data_buf_size;
  pbm_field_s_type *data_buf = NULL;
  pbm_return_type ret = PBM_ERROR;
  int num_fields;
  pbm_phonebook_type pb_info;
  boolean is_last_ind = FALSE;
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr;

  ASSERT( params_ptr != NULL );

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    return;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_RECORD_READ_IND;
  cmd_type = params_ptr->header.cmd;
  if(params_ptr->header.cmd == QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND)
  {
    pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_RECORD_READ_EXT_IND;
  }

  intr_ind = params_ptr->data.intr_rec_read_ind;
  UIM_MSG_HIGH_2("qmi_pbm_if_internal_record_read_ind [start_id = %d, end_id = %d]",
                      intr_ind.start_id, intr_ind.end_id);

  if ((pbm_if_data_ptr->data.rec_read_ind = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_record_read_ind_s))) == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    return;
  }

  read_ind = (qmi_pbm_if_record_read_ind_s *)pbm_if_data_ptr->data.rec_read_ind;

  memset(read_ind, 0, sizeof(qmi_pbm_if_record_read_ind_s));

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(intr_ind.sess_type, intr_ind.pb_type, &pb_info);

  for(rec_id = intr_ind.start_id;rec_id <= intr_ind.end_id;rec_id++)
  {
    pbm_rec_id = pbm_session_location_to_record_id(pb_info,rec_id);
    data_buf_size = pbm_session_calculate_fields_size_from_id(pbm_rec_id);
    if(0 == data_buf_size)
    {
      UIM_MSG_HIGH_1("Record with id (%d) is empty on card",rec_id);
      continue;
    }
    if ((data_buf = QMI_PBM_IF_MEM_ALLOC(data_buf_size)) == NULL)
    {
      UIM_MSG_ERR_0("modem_mem_alloc() failed for data_buf");
      QMI_PBM_IF_MEM_FREE(read_ind);
      QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      return;
    }
    memset(data_buf,0,data_buf_size);
    UIM_MSG_HIGH_1("Record with id %d is being fetched from pbm", rec_id);

    /* READ PBM RECORD */
    ret = pbm_session_record_read(pbm_rec_id,&category,&num_fields,(uint8*)data_buf,data_buf_size);
    if( ret != PBM_SUCCESS )
    {
      UIM_MSG_HIGH_2("pbm_session_record_read failed(err %d) for pbm_rec_id %d",ret,pbm_rec_id);
      QMI_PBM_IF_MEM_FREE(data_buf);
      continue;
    }
    read_ind->rec_data[i].rec_id = rec_id;

    if(QMI_RESULT_SUCCESS != qmi_copy_pbm_rec_to_qmi_ind
                                  (data_buf,
                                   num_fields,
                                   &(read_ind->rec_data[i]),
                                   cmd_type))
    {
      QMI_PBM_IF_MEM_FREE(data_buf);
      QMI_PBM_IF_MEM_FREE(read_ind);
      QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      return;
    }

    QMI_PBM_IF_MEM_FREE(data_buf);
    i++;
    UIM_MSG_HIGH_2("%d th Record with id %d is fetched successfully",i, rec_id);
    if( QMI_PBM_IF_READ_RECORDS_MAX == i )
    {
      read_ind->sess_type = intr_ind.sess_type;
      read_ind->pb_type = intr_ind.pb_type;
      read_ind->num_of_recs = QMI_PBM_IF_READ_RECORDS_MAX;
      if( rec_id == intr_ind.end_id )
      {
        read_ind->seq_num = QMI_PBM_IF_MAX_SEQ_NUM;
        is_last_ind = TRUE;
      }
      else
      {
        intr_ind.seq_num++;
        read_ind->seq_num = intr_ind.seq_num;
      }
      /* Send record read indication */
      qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);

      if(!is_last_ind)
      {
        intr_ind.start_id = rec_id + 1;
        qmi_pbm_if_send_ind(params_ptr->header.cmd,&intr_ind,sizeof(intr_ind));
      }
      return;
    }
  }/* for num of records*/

  read_ind->sess_type = intr_ind.sess_type;
  read_ind->pb_type = intr_ind.pb_type;
  read_ind->num_of_recs = i;
  read_ind->seq_num = QMI_PBM_IF_MAX_SEQ_NUM;
  /* Send the last indication */
  qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
} /* qmi_pbm_if_internal_record_read_ind */


/*===========================================================================
  FUNCTION QMI_PBM_IF_INTERNAL_RECORD_READ__EXT_UNDECODED_IND()

  DESCRIPTION
    Send each RECORD_READ indication with a maximum data for 10 records.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_internal_record_read_ext_undecoded_ind
(
  qmi_pbm_if_info_item_s  *const params_ptr
)
{
  pbm_phonebook_type                                        pb_info;
  qmi_pbm_if_internal_record_read_ext_undecoded_ind_s       intr_ind;
  qmi_pbm_if_record_read_ext_undecoded_ind_s               *read_ind        = NULL;
  uint8                                                     i               = 0;
  uint16                                                    rec_id          = 0;
  uint16                                                    category        = 0;
  uint16                                                    cmd_type        = 0;
  uint32                                                    pbm_rec_id      = 0;
  uint32                                                    data_buf_size   = 0;
  int                                                       num_fields      = 0;
  boolean                                                   is_last_ind     = FALSE;
  pbm_field_s_type                                         *data_buf        = NULL;
  pbm_return_type                                           ret             = PBM_ERROR;
  qmi_pbm_if_info_item_s                                   *pbm_if_data_ptr = NULL;

  ASSERT(params_ptr != NULL);

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    return;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_INTERNAL_READ_RECORDS_EXT_UNDECODED_IND;
  cmd_type = params_ptr->header.cmd;
  intr_ind = params_ptr->data.intr_rec_read_ext_undecoded_ind;

  UIM_MSG_HIGH_2("qmi_pbm_if_internal_record_read_ext_undecoded_ind start_id = 0x%x, end_id = 0x%x",
                  intr_ind.start_id, intr_ind.end_id);

  if((pbm_if_data_ptr->data.rec_read_ext_ind = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_record_read_ext_undecoded_ind_s))) == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    return;
  }

  read_ind = (qmi_pbm_if_record_read_ext_undecoded_ind_s *)pbm_if_data_ptr->data.rec_read_ext_ind;

  memset(read_ind, 0, sizeof(qmi_pbm_if_record_read_ext_undecoded_ind_s));

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(intr_ind.sess_type, intr_ind.pb_type, &pb_info);

  for(rec_id = intr_ind.start_id; rec_id <= intr_ind.end_id; rec_id++)
  {
    pbm_rec_id = pbm_session_location_to_record_id(pb_info, rec_id);
    data_buf_size = pbm_session_calculate_undecoded_fields_size_from_id(pbm_rec_id);
    if(0 == data_buf_size)
    {
      continue;
    }
    if ((data_buf = QMI_PBM_IF_MEM_ALLOC(data_buf_size)) == NULL)
    {
      QMI_PBM_IF_MEM_FREE(read_ind);
      QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      return;
    }
    memset(data_buf, 0, data_buf_size);

    /* READ PBM RECORD */
    ret = pbm_session_record_read_undecoded(pbm_rec_id, &category, &num_fields, (uint8*)data_buf, data_buf_size);
    if( ret != PBM_SUCCESS )
    {
      UIM_MSG_HIGH_2("pbm_session_record_read failed(err %d) for pbm_rec_id %d",ret, pbm_rec_id);
      QMI_PBM_IF_MEM_FREE(data_buf);
      continue;
    }
    read_ind->rec_data[i].rec_id = rec_id;

    if(QMI_RESULT_SUCCESS != qmi_copy_pbm_rec_to_qmi_undecoded_ind(data_buf,
                                                                   num_fields,
                                                                   &(read_ind->rec_data[i]),
                                                                   cmd_type))
    {
      QMI_PBM_IF_MEM_FREE(data_buf);
      QMI_PBM_IF_MEM_FREE(read_ind);
      QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      return;
    }

    QMI_PBM_IF_MEM_FREE(data_buf);
    i++;
    UIM_MSG_HIGH_2("0x%x - Record with id 0x%x is fetched successfully",i, rec_id);
    if( QMI_PBM_IF_READ_RECORDS_MAX == i )
    {
      read_ind->sess_type = intr_ind.sess_type;
      read_ind->pb_type = intr_ind.pb_type;
      read_ind->num_of_recs = QMI_PBM_IF_READ_RECORDS_MAX;
      if( rec_id == intr_ind.end_id )
      {
        read_ind->seq_num = QMI_PBM_IF_MAX_SEQ_NUM;
        is_last_ind = TRUE;
      }
      else
      {
        intr_ind.seq_num++;
        read_ind->seq_num = intr_ind.seq_num;
      }
      /* Send record read indication */
      qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);

      if(!is_last_ind)
      {
        intr_ind.start_id = rec_id + 1;
        qmi_pbm_if_send_ind(params_ptr->header.cmd, &intr_ind, sizeof(intr_ind));
      }
      return;
    }
  }/* for num of records*/

  read_ind->sess_type = intr_ind.sess_type;
  read_ind->pb_type = intr_ind.pb_type;
  read_ind->num_of_recs = i;
  read_ind->seq_num = QMI_PBM_IF_MAX_SEQ_NUM;
  /* Send the last indication */
  qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
} /* qmi_pbm_if_internal_record_read_ext_undecoded_ind */


/*===========================================================================
  FUNCTION qmi_pbm_if_map_mapping_to_qmi()

  DESCRIPTION
    Mapping the PBM PBR file mapping type to that of QMI PBM.

  PARAMETERS
   pbm_pbr_mapping

  RETURN VALUE
    qmi_pbm_mapping_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_pbm_mapping_type qmi_pbm_if_map_mapping_to_qmi
(
  pbm_pbr_mapping_enum_type pbm_pbr_mapping
)
{
  qmi_pbm_mapping_type qmi_mapping = QMI_PBM_MAPPING_TYPE_NONE;

  switch(pbm_pbr_mapping)
  {
    case PBM_MAPPING_TYPE_1:
      qmi_mapping = QMI_PBM_MAPPING_TYPE_1;
      break;
    case PBM_MAPPING_TYPE_2:
      qmi_mapping = QMI_PBM_MAPPING_TYPE_2;
      break;
    default:
      break;
  }
  return (qmi_mapping);
} /* qmi_pbm_if_map_mapping_to_qmi */


/*===========================================================================
  FUNCTION qmi_pbm_if_map_field_to_qmi()

  DESCRIPTION
    Mapping the PBM field ID to that of QMI PBM.

  PARAMETERS
    pbm_field

  RETURN VALUE
    qmi_pbm_field_id_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_pbm_field_id_type qmi_pbm_if_map_field_to_qmi
(
  pbm_field_id_e_type pbm_field
)
{
  qmi_pbm_field_id_type qmi_pbm_field = QMI_PBM_FIELD_NONE;

  switch(pbm_field)
  {
    case PBM_FIELD_EMAIL:
      qmi_pbm_field = QMI_PBM_FIELD_EMAIL;
      break;
    case PBM_FIELD_ADDITIONAL_NUMBER:
    case PBM_FIELD_ADDITIONAL_NUMBER_EXT :
      qmi_pbm_field = QMI_PBM_FIELD_ANR;
      break;
    case PBM_FIELD_OTHERNAME:
      qmi_pbm_field = QMI_PBM_FIELD_SNE;
      break;
    case PBM_FIELD_GRPNAME_ID:
      qmi_pbm_field = QMI_PBM_FIELD_GRP;
      break;
    default:
      UIM_MSG_HIGH_1("invalid field-id 0x%x",pbm_field);
      break;
  }
  return (qmi_pbm_field);
} /* qmi_pbm_if_map_npi_to_qmi */


/*===========================================================================
  FUNCTION QMI_PBM_IF_INTERNAL_PBSET_CAPS_READ_IND()

  DESCRIPTION
    Send each RECORD_READ indication with a maximum data for 10 records.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_internal_pbset_caps_read_ind
(
  qmi_pbm_if_info_item_s  *const params_ptr
)
{
  qmi_pbm_if_internal_pbset_caps_read_ind_s      intr_ind;
  boolean                                        is_last_ind        = FALSE;
  qmi_pbm_if_info_item_s                        *pbm_if_data_ptr    = NULL;
  qmi_pbm_if_pbset_cap_read_ind_s               *pbset_cap_read_ind = NULL;
  uint8                                          i                  = 0;
  uint16                                         pbset_id           = 0;

  ASSERT( params_ptr != NULL );

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    QMI_PBM_IF_MEM_FREE(params_ptr->data.intr_pbset_cap_read_ind.usim_cap_info);
    return;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_PBSET_CAPS_READ_IND;

  intr_ind = params_ptr->data.intr_pbset_cap_read_ind;
  UIM_MSG_HIGH_2("qmi_pbm_if_internal_pbset_caps_read_ind start_id = 0x%x, end_id = 0x%x",
                  intr_ind.start_id, intr_ind.end_id);

  if ((pbm_if_data_ptr->data.pbset_cap_read_ind = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_pbset_cap_read_ind_s))) == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    QMI_PBM_IF_MEM_FREE(params_ptr->data.intr_pbset_cap_read_ind.usim_cap_info);
    return;
  }

  pbset_cap_read_ind = (qmi_pbm_if_pbset_cap_read_ind_s *)pbm_if_data_ptr->data.pbset_cap_read_ind;

  memset(pbset_cap_read_ind, 0, sizeof(qmi_pbm_if_pbset_cap_read_ind_s));

  for(pbset_id = intr_ind.start_id-1; pbset_id < intr_ind.end_id; pbset_id++)
  {
    uint16 l_field = 0;

    for(l_field = 0; l_field < PBM_MAX_USIM_FIELDS_IN_PB_SET; l_field++)
    {
      qmi_pbm_if_pbset_cap_data_s      *l_qmi_usim_caps = NULL;
      pbm_usim_field_capability_s_type *l_pbm_usim_caps = NULL;

      l_pbm_usim_caps = &intr_ind.usim_cap_info->pbm_all_usim_capibilities[pbset_id][l_field];
      l_qmi_usim_caps = &pbset_cap_read_ind->pbset_cap_data[i][l_field];
      if(l_pbm_usim_caps->field_id != PBM_FIELDID_NONE)
      {
        l_qmi_usim_caps->field_id = qmi_pbm_if_map_field_to_qmi(l_pbm_usim_caps->field_id);
        l_qmi_usim_caps->mapping_type = qmi_pbm_if_map_mapping_to_qmi(l_pbm_usim_caps->mapping_type);
        l_qmi_usim_caps->num_of_records = l_pbm_usim_caps->num_of_records;
        l_qmi_usim_caps->num_free_records = l_pbm_usim_caps->num_of_records - l_pbm_usim_caps->records_in_use;
        l_qmi_usim_caps->record_length = l_pbm_usim_caps->record_length;
        UIM_MSG_HIGH_3("pbset_id :0x%x l_qmi_usim_caps->field_id :0x%x num_of_records :0x%x",pbset_id+1, l_field, l_qmi_usim_caps->num_of_records);
      }
    }
    i++;
    if(QMI_PBM_IF_READ_RECORDS_MAX == i)
    {
      pbset_cap_read_ind->sess_type = intr_ind.sess_type;
      pbset_cap_read_ind->num_of_pbsets = QMI_PBM_IF_READ_RECORDS_MAX;
      if( pbset_id == intr_ind.end_id )
      {
        pbset_cap_read_ind->is_last_ind = TRUE;
        is_last_ind = TRUE;
      }

      /* Send record read indication */
      qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);

      if(!is_last_ind)
      {
        intr_ind.start_id = pbset_id + 1;
        qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_INTERNAL_PBSET_CAPS_READ_IND,&intr_ind,sizeof(intr_ind));
      }
      return;
    }
  }/* for num of records*/

  pbset_cap_read_ind->sess_type = intr_ind.sess_type;
  pbset_cap_read_ind->num_of_pbsets = i;
  pbset_cap_read_ind->is_last_ind = TRUE;
  /* Send the last indication */
  qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
  QMI_PBM_IF_MEM_FREE(params_ptr->data.intr_pbset_cap_read_ind.usim_cap_info);
} /* qmi_pbm_if_internal_record_read_ind */


/*===========================================================================
  FUNCTION QMI_PBM_IF_INTERNAL_HIDDEN_RECORDS_READ_IND()

  DESCRIPTION
    Send each RECORD_READ indication with a maximum data for 10 hidden
    records.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_internal_hidden_records_read_ind
(
  qmi_pbm_if_info_item_s  *const params_ptr
)
{
  qmi_pbm_if_internal_hidden_recs_read_ind_s intr_ind;
  qmi_pbm_if_record_read_ind_s *read_ind = NULL;
  uint8  i = 0;
  uint16 rec_id,category = 0;
  uint32 pbm_rec_id, data_buf_size;
  pbm_field_s_type *data_buf = NULL;
  pbm_return_type ret = PBM_ERROR;
  int num_fields;
  uint16 seq_num=0;
  pbm_phonebook_type pb_info;
  boolean is_last_ind = FALSE;
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr=NULL;
  boolean allocate_memory = TRUE;
  qmi_pbm_if_cmd_e_type cmd_type=QMI_PBM_IF_CMD_MIN;

  ASSERT( params_ptr != NULL );

  intr_ind = params_ptr->data.intr_hidd_rec_ind;
  UIM_MSG_HIGH_1("qmi_pbm_if_internal_hidden_records_read_ind: session type %d ",
                      intr_ind.sess_type);

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(intr_ind.sess_type, (uint16)QMI_PBM_IF_PB_ADN, &pb_info);

  if (PBM_SUCCESS == pbm_session_enum_rec_init(pb_info, (uint16)PBM_CAT_NONE, PBM_FIELD_HIDDEN, NULL, 0,0) )
  {
    /*The record list in PBM is formed. Get the record ids one by one*/
    while(PBM_SUCCESS == pbm_session_enum_next_rec_id(&pbm_rec_id))
    {
      if(TRUE == allocate_memory)
      {
        allocate_memory = FALSE;
        pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
        if( NULL == pbm_if_data_ptr )
        {
          return;
        }

        memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

        pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_RECORD_READ_IND;
        cmd_type = params_ptr->header.cmd;
        if(params_ptr->header.cmd == QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND)
        {
          pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_RECORD_READ_EXT_IND;
        }

        if ((pbm_if_data_ptr->data.rec_read_ind = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_record_read_ind_s))) == NULL)
        {
          QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
          return;
        }
        read_ind = (qmi_pbm_if_record_read_ind_s *)pbm_if_data_ptr->data.rec_read_ind;
        memset(read_ind, 0, sizeof(qmi_pbm_if_record_read_ind_s));
      }
      rec_id = pbm_session_record_id_to_location(pbm_rec_id);
      data_buf_size = pbm_session_calculate_fields_size_from_id(pbm_rec_id);
      if(0 == data_buf_size)
      {
        continue;
      }
      if ((data_buf = QMI_PBM_IF_MEM_ALLOC(data_buf_size)) == NULL)
      {
        QMI_PBM_IF_MEM_FREE(read_ind);
        QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
        return;
      }
      memset(data_buf,0,data_buf_size);

      /* READ PBM RECORD */
      ret = pbm_session_record_read(pbm_rec_id,&category,&num_fields,(uint8*)data_buf,data_buf_size);
      if( ret != PBM_SUCCESS )
      {
        UIM_MSG_HIGH_2("pbm_session_record_read failed(err %d) for pbm_rec_id %d",ret,pbm_rec_id);
        QMI_PBM_IF_MEM_FREE(data_buf);
        continue;
      }
      read_ind->rec_data[i].rec_id = rec_id;


      if(QMI_RESULT_SUCCESS != qmi_copy_pbm_rec_to_qmi_ind
                              (data_buf,
                               num_fields,
                               &(read_ind->rec_data[i]),
                               cmd_type))
      {
          QMI_PBM_IF_MEM_FREE(data_buf);
          QMI_PBM_IF_MEM_FREE(read_ind);
          QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
          return;
      } /* for num_of_fields */
      QMI_PBM_IF_MEM_FREE(data_buf);
      i++;

      if(QMI_PBM_IF_READ_RECORDS_MAX == i)
      {
        read_ind->sess_type = intr_ind.sess_type;
        read_ind->pb_type = QMI_PBM_IF_PB_ADN;
        read_ind->num_of_recs = QMI_PBM_IF_READ_RECORDS_MAX;
        if(pbm_rec_id == intr_ind.last_hidden_rec)
        {
          read_ind->seq_num = QMI_PBM_IF_MAX_SEQ_NUM;
          is_last_ind = TRUE;
        }
        else
        {
          seq_num++;
          read_ind->seq_num = seq_num;
        }
        /* Send record read indication */
        qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
        if(is_last_ind)
        {
          return;
        }
        i = 0;
        allocate_memory = TRUE;
      }
    }
    if(i > 0)
    {
      read_ind->sess_type   = intr_ind.sess_type;
      read_ind->pb_type     = QMI_PBM_IF_PB_ADN;
      read_ind->seq_num     = QMI_PBM_IF_MAX_SEQ_NUM;
      read_ind->num_of_recs = i;
      qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
    }
    if (i == 0 && allocate_memory == FALSE)
    {
      if(read_ind)
      {
        QMI_PBM_IF_MEM_FREE(read_ind);
      }
      if(pbm_if_data_ptr)
      {
        QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
      }
    }
  }
} /* qmi_pbm_if_internal_hidden_records_read_ind */


/*===========================================================================
  FUNCTION QMI_PBM_IF_WRITE_RECORD()

  DESCRIPTION
    Write record.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_write_record(
  qmi_pbm_if_info_item_s   *const params_ptr
)
{
  uint8                         f_cnt = 0;
  qmi_pbm_if_rec_data_s         *rec = NULL;
  qmi_error_e_type              err;
  pbm_phonebook_type            pb_info;
  uint8                         sess_type, i,g_idx=0, ad_idx=0, e_idx=0, k = 0;
  uint16                        pb_type,grp_id;
  uint32                        fields_ptr_size=0, rec_id=0, sub_fields_ptr_size=0;
  uint8                         *fields_ptr = NULL, *sub_fields_ptr = NULL;
  void                          *field_data_ptr = NULL;
  pbm_return_type               ret;
  pbm_cat_e_type                cat;
  qmi_pbm_if_amss_npi_e_type    npi;
  uint8 ton_npi = 0;
  pbm_extended_fileinfo_s_type  recs_info;
  qmi_cmd_buf_type              *cmd_buf_p = NULL;

  ASSERT( params_ptr != NULL );
  cmd_buf_p = (qmi_cmd_buf_type*)params_ptr->header.handle;
  ASSERT( cmd_buf_p != NULL );

  rec       = &params_ptr->data.write_rec.rec_data;
  sess_type = params_ptr->data.write_rec.session_type;
  pb_type   = params_ptr->data.write_rec.pb_type;

  UIM_MSG_HIGH_2("qmi_pbm_if_write_record Session Type 0x%x, Phonebook type 0x%x", sess_type, pb_type);

  /* check if session type, pb type are valid */
  if((err = qmi_pbm_if_check_session_pb_validity(sess_type, pb_type)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err, NULL, 0, FALSE);
    return;
  }

  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(sess_type, pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_PB_DELETE_IN_PROG, NULL, 0, FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(sess_type, pb_type, &pb_info);

  /* Get the category */
  cat = qmi_pbm_if_pb_type_to_cat((qmi_pbm_if_pb_type)pb_type);

  UIM_MSG_HIGH_2("Category is 0x%x and record id is 0x%x", cat, rec->rec_id);
  if((ret = pbm_session_extended_file_info(pb_info, &recs_info)) != PBM_SUCCESS)
  {
    err = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err,NULL,0,FALSE);
    return;
  }

  if(rec->rec_id == QMI_PBM_IF_GET_FIRST_AVAILABLE_ID)
  {
    /* New record addition */
    rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
  }
  else
  {
    /* Update an existing record */
    if(!qmi_pbm_if_is_rec_id_in_range(sess_type, pb_type, rec->rec_id) )
    {
      UIM_MSG_MED_0("record id data not in valid range");
      qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_ID, NULL, 0, FALSE);
      return;
    }
    /* Retrieve the pbm record id from the sim record index. */
    rec_id = pbm_session_location_to_record_id(pb_info, rec->rec_id);
  }

  {
    pbm_field_id_e_type      *field_id   = NULL;
    pbm_field_type_e_type    *field_type = NULL;
    uint32                   *field_size = NULL;
    int                      anr_num_sub_fields[QMI_PBM_IF_MAX_AD_NUM] = {0};
    int                      anr_complex_size[QMI_PBM_IF_MAX_AD_NUM]   = {0};
    pbm_field_s_type         *ptyped_fields_ptr,*ptyped_sub_fields_ptr[QMI_PBM_IF_MAX_AD_NUM] = {NULL};
    uint8                    *email_text_local = NULL;
    uint8                    *alpha_text_local = NULL;
    uint8                     num_local[QMI_PBM_IF_MAX_NUM_LEN + 1] = {0};
    uint8                    *sne_text_local = NULL;

    field_id = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_field_id_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_id == NULL)
    {
      qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
      return;
    }

    field_type = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_field_type_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_type == NULL)
    {
      qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
      QMI_PBM_IF_MEM_FREE(field_id);
      return;
    }

    field_size = QMI_PBM_IF_MEM_ALLOC(sizeof(uint32) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_size == NULL)
    {
      qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
      QMI_PBM_IF_MEM_FREE(field_id);
      QMI_PBM_IF_MEM_FREE(field_type);
      return;
    }

    memset(field_id, 0, sizeof(pbm_field_id_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    memset(field_type, 0, sizeof(pbm_field_type_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    memset(field_size, 0, sizeof(uint32) * QMI_PBM_IF_MAX_FIELDS_IN_REC);

    if(rec->num_info.len > 0)
    {
      /* Update TON/NPI field only if we have proper number info in write request */
      if((rec->num_info.num_type != QMI_PBM_IF_NUM_TYPE_UNKNOWN) ||
         (rec->num_info.num_plan != QMI_PBM_IF_NUM_PLAN_UNKNOWN))
      {
        field_id[f_cnt]   = PBM_FIELD_GSM_TONNPI;
        field_type[f_cnt] = PBM_FT_BYTE;
        field_size[f_cnt] = QMI_PBM_IF_TON_NPI_SIZE;
        f_cnt++;
      }

      field_id[f_cnt]   = PBM_FIELD_PHONE_GENERIC;
      field_type[f_cnt] = PBM_FT_PHONE;
      /* Add 1 for terminating nul char. Amss pbm expects a
         nul char at the end of the number*/
      field_size[f_cnt] = rec->num_info.len + 1;
      f_cnt++;
    }

    if(rec->name_info.len > 0)
    {
      field_id[f_cnt] = PBM_FIELD_NAME;
      field_type[f_cnt] = PBM_FT_UCS2;
      /* Since ucs2 char is of 2 bytes, Add 2 for terminating nul char.
               Amss pbm expects a nul char at the end of the name. */
      if(cmd_buf_p->cmd_type == PBMI_CMD_VAL_WRITE_RECORD_EXT)
      {
        field_size[f_cnt] = (2 * rec->name_info.len) + 2;;
      }
      else
      {
        field_size[f_cnt] = rec->name_info.len + 2;;
      }

      f_cnt++;
    }

    /* Hidden entry support not available in AMSS PBM */
    if(rec->sname_info.len > 0)
    {
      field_id[f_cnt] = PBM_FIELD_OTHERNAME;
      field_type[f_cnt] = PBM_FT_UCS2;
      /* Since ucs2 char is of 2 bytes, Add 2 for terminating nul char.
              Amss pbm expects a nul char at the end of the name. */
      if(cmd_buf_p->cmd_type == PBMI_CMD_VAL_WRITE_RECORD_EXT)
      {
        field_size[f_cnt] = (2 * rec->sname_info.len) + 2;;
      }
      else
      {
        field_size[f_cnt] = rec->sname_info.len + 2;;
      }

      f_cnt++;
    }

    if(rec->ad_num_cnt > 0)
    {
      pbm_field_id_e_type   anr_sfield_id[QMI_PBM_IF_MAX_ANR_SUB_FIELDS]   = {0};
      pbm_field_type_e_type anr_sfield_type[QMI_PBM_IF_MAX_ANR_SUB_FIELDS] = {0};
      uint32                anr_sfield_size[QMI_PBM_IF_MAX_ANR_SUB_FIELDS] = {0};
      uint8                 sf_cnt                                         = 0;
      uint8                 ad_num_local[QMI_PBM_IF_MAX_NUM_LEN + 1] = {0};

      for(i=0;i<rec->ad_num_cnt;i++)
      {
        field_id[f_cnt] = PBM_FIELD_COMPLEX;
        field_type[f_cnt] = PBM_FT_COMPLEX;
        sf_cnt = 0;
        /* Write the additional number */
        anr_sfield_id[sf_cnt] = PBM_FIELD_ADDITIONAL_NUMBER;
        anr_sfield_type[sf_cnt] = PBM_FT_PHONE;
        anr_sfield_size[sf_cnt] = rec->ad_num_data[i].ad_num.len + 1; /* +1 for nul char */
        memset(ad_num_local, 0 , sizeof(ad_num_local));
        memscpy(ad_num_local, sizeof(ad_num_local), rec->ad_num_data[i].ad_num.num, rec->ad_num_data[i].ad_num.len);
        sf_cnt++;
        if( (rec->ad_num_data[i].ad_num.num_plan != 0)|| (rec->ad_num_data[i].ad_num.num_type != 0))
        {
          /* Write the ton npi info here */
          anr_sfield_id[sf_cnt] = PBM_FIELD_GSM_TONNPI;
          anr_sfield_type[sf_cnt] = PBM_FT_BYTE;
          anr_sfield_size[sf_cnt] = QMI_PBM_IF_TON_NPI_SIZE;
          sf_cnt++;
        }

        if(rec->ad_num_data[i].tag_id != 0)
        {
          /* Tie the additional number to the specific type */
          anr_sfield_id[sf_cnt] = PBM_FIELD_ADDITIONAL_NUMBER_TYPE;
          anr_sfield_type[sf_cnt] = PBM_FT_BYTE;
          anr_sfield_size[sf_cnt] = QMI_PBM_IF_ANR_TYPE_SIZE;
          sf_cnt++;
        }

        UIM_MSG_HIGH_1("Number of ANR sub fields is %d ", sf_cnt);
        for(k = 0; k < sf_cnt; k++)\
        {
          UIM_MSG_HIGH_4("Sub Field_id[0x%x]=0x%x, Field_type=0x%x, Field_size=0x%x",
                          k, anr_sfield_id[k],anr_sfield_type[k], anr_sfield_size[k]);
        }

        /* Allocate the memory for the sub fields */
        if(!pbm_allocate_fields(&sub_fields_ptr, (uint32 *)&sub_fields_ptr_size,
                        sf_cnt, (const uint32 *)anr_sfield_size))
        {
          qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
          QMI_PBM_IF_MEM_FREE(field_id);
          QMI_PBM_IF_MEM_FREE(field_type);
          QMI_PBM_IF_MEM_FREE(field_size);
          return;
        }

        ptyped_sub_fields_ptr[ad_idx] = (pbm_field_s_type*)sub_fields_ptr;

        for(k=0;k<sf_cnt;k++)
        {
          switch(anr_sfield_id[k])
          {
            case PBM_FIELD_ADDITIONAL_NUMBER:
              UIM_PBM_MSG_SPRINTF_2("Additional number 0x%x in complex field is %s", (i+1), (char *)rec->ad_num_data[i].ad_num.num );
              pbm_fill_field(&ptyped_sub_fields_ptr[ad_idx][k],  anr_sfield_id[k], anr_sfield_type[k], (const byte *)ad_num_local, (uint16)anr_sfield_size[k]);
              break;
            case PBM_FIELD_GSM_TONNPI:
              UIM_MSG_HIGH_2("Numtype 0x%x and Numplan 0x%x received for ANR",rec->ad_num_data[i].ad_num.num_type, rec->ad_num_data[i].ad_num.num_plan);
              npi = qmi_pbm_if_map_qmi_npi_to_pbm((qmi_pbm_if_num_plan_e_type)rec->ad_num_data[i].ad_num.num_plan);
              ton_npi = ((rec->ad_num_data[i].ad_num.num_type & 0x07)<<4) | (((uint8)npi)& 0x0F);
              UIM_MSG_HIGH_1("ANR TON-NPI value in complex field is 0x%x",ton_npi);
              pbm_fill_field(&ptyped_sub_fields_ptr[ad_idx][k], anr_sfield_id[k], anr_sfield_type[k], (const byte *)&ton_npi, (uint16)anr_sfield_size[k]);
              break;
            case PBM_FIELD_ADDITIONAL_NUMBER_TYPE:
              UIM_MSG_HIGH_1("AAS id for this number is 0x%x", rec->ad_num_data[i].tag_id);
              pbm_fill_field(&ptyped_sub_fields_ptr[ad_idx][k], anr_sfield_id[k], anr_sfield_type[k], (const byte *)&rec->ad_num_data[i].tag_id, (uint16)anr_sfield_size[k]);
              break;
            default:
              UIM_MSG_HIGH_1("Unsupported sub field id 0x%x", anr_sfield_id[k]);
              break;
          }
        }

        /* Get the complex field size */
        field_size[f_cnt] = pbm_cal_complex_field_size(sf_cnt, anr_sfield_size);
        anr_complex_size[ad_idx] = (int)sub_fields_ptr_size;
        anr_num_sub_fields[ad_idx++] = sf_cnt;
        f_cnt++;
      }

      /* Reset the anr index to be used again */
      ad_idx = 0;
    }

    if(rec->grp_cnt > 0)
    {
      for(i=0;i<rec->grp_cnt;i++)
      {
        field_id[f_cnt] = PBM_FIELD_GRPNAME_ID;
        field_type[f_cnt] = PBM_FT_WORD;
        field_size[f_cnt] = QMI_PBM_IF_GRP_ID_SIZE;
        f_cnt++;
        /* Grouping alpha string not supported in AMSS-PBM */
      }
    }

    if(rec->email_cnt > 0)
    {
      for(i=0;i<rec->email_cnt;i++)
      {
        field_id[f_cnt] = PBM_FIELD_EMAIL;
        field_type[f_cnt] = PBM_FT_UCS2;
        /* Since ucs2 char is of 2 bytes, Add 2 for terminating nul char.
                  Amss pbm expects a nul char at the end of the name. */
        if(cmd_buf_p->cmd_type == PBMI_CMD_VAL_WRITE_RECORD_EXT)
        {
          field_size[f_cnt] = (2 * rec->email_data[i].len) + 2;;
        }
        else
        {
          field_size[f_cnt] = rec->email_data[i].len + 2;;
        }

        f_cnt++;
      }
    }

    if(rec->is_hidden)
    {
      field_id[f_cnt] = PBM_FIELD_HIDDEN;
      field_type[f_cnt] = PBM_FT_BOOLEAN;
      field_size[f_cnt] = QMI_PBM_IF_HIDDEN_FIELD_SIZE;
      f_cnt++;
    }

    UIM_MSG_HIGH_1("Number of fields is 0x%x", f_cnt);
    for( i = 0; i < f_cnt; i++)
    {
      UIM_MSG_HIGH_4("Field_id[0x%x]=0x%x, Field_type=0x%x, Field_size=0x%x",
                      i, field_id[i], field_type[i], field_size[i]);
    }

    /* Allocate the memory for the fields */
    if(!pbm_allocate_fields(&fields_ptr, (uint32 *)&fields_ptr_size, f_cnt, (const uint32 *)field_size))
    {
      qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
      for(i=0; i < rec->ad_num_cnt; i++)
      {
        pbm_free_fields((uint8**)&ptyped_sub_fields_ptr[i],anr_num_sub_fields[i]);
      }

      QMI_PBM_IF_MEM_FREE(field_id);
      QMI_PBM_IF_MEM_FREE(field_type);
      QMI_PBM_IF_MEM_FREE(field_size);
      return;
    }

    ptyped_fields_ptr = (pbm_field_s_type*)fields_ptr;

    /* Now fill each field */
    for(i=0;i<f_cnt;i++)
    {
      switch(field_id[i])
      {
        case PBM_FIELD_NAME:
          alpha_text_local = QMI_PBM_IF_MEM_ALLOC(field_size[i]);
          if(alpha_text_local == NULL)
          {
            qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
            for(i=0; i < rec->ad_num_cnt; i++)
            {
              pbm_free_fields((uint8**)&ptyped_sub_fields_ptr[i], anr_num_sub_fields[i]);
            }
            QMI_PBM_IF_MEM_FREE(field_id);
            QMI_PBM_IF_MEM_FREE(field_type);
            QMI_PBM_IF_MEM_FREE(field_size);
            pbm_free_fields(&fields_ptr,f_cnt);
            return;
          }
          memset(alpha_text_local, 0, field_size[i]);
          /* copying alpha text to local to avoid 2 bytes over read */
          memscpy(alpha_text_local, field_size[i], rec->name_info.text, field_size[i] - 2);
          pbm_fill_field(&ptyped_fields_ptr[i], field_id[i], field_type[i], (const byte *)alpha_text_local, (uint16)field_size[i]);
          QMI_PBM_IF_MEM_FREE(alpha_text_local);
          break;
        case PBM_FIELD_PHONE_GENERIC:
          UIM_PBM_MSG_SPRINTF_2("Phone number of length (%d) sent to pbm is %s",field_size[i], rec->num_info.num);
          /* copying ADN number to local buffer t oavoid 1 byte buffer over read */
          memset(num_local, 0 , sizeof(num_local));
          memscpy(num_local, sizeof(num_local), rec->num_info.num, rec->num_info.len);
          pbm_fill_field(&ptyped_fields_ptr[i], field_id[i], field_type[i], (const byte *)num_local, (uint16)field_size[i]);
          break;
        case PBM_FIELD_GSM_TONNPI:
          /* Send ton npi together as Amss pbm has incorrect handling if both are sent separately */
          npi = qmi_pbm_if_map_qmi_npi_to_pbm((qmi_pbm_if_num_plan_e_type)rec->num_info.num_plan);
          ton_npi = ((rec->num_info.num_type & 0x07)<<4) | (((uint8)npi)& 0x0F);
          UIM_MSG_HIGH_3("Number type 0x%x Number plan 0x%x TON-NPI value 0x%x",rec->num_info.num_type, rec->num_info.num_plan, ton_npi);
          pbm_fill_field(&ptyped_fields_ptr[i], field_id[i], field_type[i], (const byte *)&ton_npi, (uint16)field_size[i]);
          break;
        case PBM_FIELD_OTHERNAME:
          sne_text_local = QMI_PBM_IF_MEM_ALLOC(field_size[i]);
          if(sne_text_local == NULL)
          {
            qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
            for(i=0; i < rec->ad_num_cnt; i++)
            {
              pbm_free_fields((uint8**)&ptyped_sub_fields_ptr[i], anr_num_sub_fields[i]);
            }

            QMI_PBM_IF_MEM_FREE(field_id);
            QMI_PBM_IF_MEM_FREE(field_type);
            QMI_PBM_IF_MEM_FREE(field_size);
            pbm_free_fields(&fields_ptr,f_cnt);
            return;
          }
          memset(sne_text_local, 0, field_size[i]);
          /* copying alpha text to local to avoid 2 bytes over read */
          memscpy(sne_text_local, field_size[i], rec->sname_info.text, field_size[i] - 2);
          pbm_fill_field(&ptyped_fields_ptr[i], field_id[i], field_type[i], (const byte *)sne_text_local, (uint16)field_size[i]);
          QMI_PBM_IF_MEM_FREE(sne_text_local);
          break;
        case PBM_FIELD_GRPNAME_ID:
          grp_id = (uint16)rec->grp_id[g_idx];
          UIM_MSG_HIGH_1("Group id sent to pbm is 0x%x",grp_id);
          pbm_fill_field(&ptyped_fields_ptr[i], field_id[i], field_type[i], (const byte *)&grp_id, (uint16)field_size[i]);
          g_idx++;
          break;
        case PBM_FIELD_EMAIL:
          /* To support empty email address as per winmob requirement */
          ptyped_fields_ptr[i].field_id   = field_id[i];
          ptyped_fields_ptr[i].field_type = field_type[i];
          field_data_ptr = pbm_get_field_data_ptr((pbm_field_s_type *)&ptyped_fields_ptr[i],0);
          if(field_data_ptr != NULL)
          {
            email_text_local = QMI_PBM_IF_MEM_ALLOC(field_size[i]);
            if(email_text_local == NULL)
            {
              qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
              for(i=0; i < rec->ad_num_cnt; i++)
              {
                pbm_free_fields((uint8**)&ptyped_sub_fields_ptr[i], anr_num_sub_fields[i]);
              }

              QMI_PBM_IF_MEM_FREE(field_id);
              QMI_PBM_IF_MEM_FREE(field_type);
              QMI_PBM_IF_MEM_FREE(field_size);
              pbm_free_fields(&fields_ptr,f_cnt);
              return;
            }
            memset(email_text_local, 0, field_size[i]);
            memscpy(email_text_local, field_size[i], rec->email_data[e_idx].text, field_size[i] - 2);
            memscpy(field_data_ptr, (uint16)field_size[i],
                  (const byte *)email_text_local, (uint16)field_size[i]);
            QMI_PBM_IF_MEM_FREE(email_text_local);
            ptyped_fields_ptr[i].data_len   = (uint16)field_size[i];
            e_idx++;
          }
          else
          {
            UIM_MSG_HIGH_0("pbm_get_field_data_ptr returned null for email");
          }
          break;
        case PBM_FIELD_HIDDEN:
          pbm_fill_field(&ptyped_fields_ptr[i], field_id[i], field_type[i], (const byte *)&rec->is_hidden, (uint16)field_size[i]);
          break;
        case PBM_FIELD_COMPLEX:
          if(ptyped_sub_fields_ptr[ad_idx] != NULL)
          {
            pbm_fill_complex_field(anr_num_sub_fields[ad_idx],(const uint8 *)ptyped_sub_fields_ptr[ad_idx] ,anr_complex_size[ad_idx],&ptyped_fields_ptr[i]);
            pbm_free_fields((uint8**)&ptyped_sub_fields_ptr[ad_idx],anr_num_sub_fields[ad_idx]);
            ad_idx++;
          }
          break;
        default:
          UIM_MSG_HIGH_1("UNKNOWN FIELD ID 0x%x",field_id[i]);
          break;
      }
    }

    QMI_PBM_IF_MEM_FREE(field_id);
    QMI_PBM_IF_MEM_FREE(field_type);
    QMI_PBM_IF_MEM_FREE(field_size);
  }

  /* Add entry to the command list and wait for callback */
  if ( qmi_pbm_cmd_list_new(params_ptr->header.cmd,
                            params_ptr->header.handle,
                            QMI_PBM_CMD_AWAITING_CALLBACK,
                            rec_id) != E_SUCCESS)
  {
    UIM_MSG_HIGH_0("Could not add entry to the list");
    /* Out of cmd_list buffer, send QMI_ERR_NO_MEMORY response */
    qmi_pbm_if_send_response( params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Validation successful. Write the record now */
  ret = pbm_session_record_write(pb_info, &rec_id, cat, (int)f_cnt, fields_ptr, (int)fields_ptr_size,
        (PBM_WRITE_CB_FUNC)qmi_pbm_if_write_callback, (void*)params_ptr->header.handle);
  if (ret != PBM_SUCCESS)
  {
    err = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err, NULL, 0, TRUE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Free the fields_ptr here */
  pbm_free_fields(&fields_ptr,f_cnt);
} /* qmi_pbm_if_write_record */


/*===========================================================================
  FUNCTION QMI_PBM_IF_DELETE_RECORD()

  DESCRIPTION
    Delete a Phone book record.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_delete_record
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  uint8                 sess_type       = 0;
  uint16                pb_type         = 0;
  uint16                rec_id          = 0;
  pbm_record_id_type    mapped_index_id = 0;
  pbm_phonebook_type    pb_info;
  pbm_cat_e_type        cat             = PBM_CAT_NONE;
  pbm_return_type       ret             = PBM_SUCCESS;
  qmi_error_e_type      err_val         = QMI_ERR_NONE;

  ASSERT(params_ptr != NULL);

  sess_type = params_ptr->data.delete_rec.session_type;
  pb_type = params_ptr->data.delete_rec.pb_type;
  rec_id = params_ptr->data.delete_rec.record_id;

  UIM_MSG_HIGH_3("qmi_pbm_if_delete_record(): Sess_type 0x%x pb_type 0x%x rec_id 0x%x",sess_type, pb_type,rec_id );

  /* check if session type, pb type are valid */
  if(!qmi_pbm_if_is_session_type_valid(sess_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_SESSION_TYPE, NULL, 0, FALSE);
    return;
  }
  if(!qmi_pbm_if_is_pb_type_valid(pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_PB_TYPE, NULL, 0, FALSE);
    return;
  }
  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(sess_type, pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_PB_DELETE_IN_PROG, NULL, 0, FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(sess_type, pb_type, &pb_info);
  mapped_index_id = pbm_session_location_to_record_id(pb_info,rec_id);
  cat = qmi_pbm_if_pb_type_to_cat((qmi_pbm_if_pb_type)pb_type);

  if(qmi_pbm_cmd_list_new(params_ptr->header.cmd,
                          params_ptr->header.handle,
                          QMI_PBM_CMD_AWAITING_CALLBACK,
                          mapped_index_id) != E_SUCCESS)
  {
    UIM_MSG_HIGH_0("Could not add entry to the list");
    /* Out of cmd_list buffer, send QMI_ERR_NO_MEMORY response */
    qmi_pbm_if_send_response( params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
    return;
  }

  ret =  pbm_session_record_write(pb_info, &mapped_index_id, cat, 0, NULL, 0, qmi_pbm_if_write_callback,(void*)params_ptr->header.handle);
  if(ret != PBM_SUCCESS)
  {
    UIM_MSG_ERR_1("qmi_pbm_if_delete_record: returned 0x%x", ret);
    err_val = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err_val,NULL,0,TRUE);
    return;
  }

}/* qmi_pbm_if_delete_record */

/*===========================================================================
  FUNCTION QMI_PBM_IF_DELETE_ALL_PB_RECORDS()

  DESCRIPTION
    Delete all Phonebook records.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_delete_all_pb_records
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  uint8                            sess_type;
  uint16                           pb_type, rec_id, max_recs, last_non_empty_rec_id = 0;
  pbm_return_type                  ret = PBM_SUCCESS;
  pbm_phonebook_type               pb_info;
  pbm_record_id_type               pbm_rec_id=0;
  pbm_cat_e_type                   cat;
  int                              data_buf_size=0;
  qmi_error_e_type                 err_val = QMI_ERR_NONE;

  ASSERT( params_ptr != NULL );

  sess_type = params_ptr->data.delete_all_pb_recs.session_type;
  pb_type = params_ptr->data.delete_all_pb_recs.pb_type;

  UIM_MSG_HIGH_2("qmi_pbm_if_delete_all_pb_records(): Sess_type %d, pb_type %d",sess_type, pb_type);

  /* check if session type, pb type are valid */
  if(!qmi_pbm_if_is_session_type_valid(sess_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_SESSION_TYPE, NULL, 0, FALSE);
    return;
  }
  if(!qmi_pbm_if_is_pb_type_valid(pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_INVALID_PB_TYPE, NULL, 0, FALSE);
    return;
  }

  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(sess_type, pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_PB_DELETE_IN_PROG, NULL, 0, FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(sess_type, pb_type, &pb_info);

  /* Get the max records */
  max_recs = qmi_pbm_if_get_max_records(sess_type, pb_type);

  for(rec_id = 1;rec_id <= max_recs ;rec_id++)
  {
    pbm_rec_id = pbm_session_location_to_record_id(pb_info, rec_id);
    if(pbm_session_calculate_fields_size_from_id(pbm_rec_id) != 0)
    {
      last_non_empty_rec_id = rec_id;
    }
  }

  if(last_non_empty_rec_id > 0)
  {
    /* Add entry to the command list and wait for the last record delete callback */
    if(qmi_pbm_cmd_list_new(params_ptr->header.cmd,
                            params_ptr->header.handle,
                            QMI_PBM_CMD_AWAITING_CALLBACK,
                            (uint32)last_non_empty_rec_id) != E_SUCCESS)
    {
      UIM_MSG_HIGH_0("Could not add entry to the list");
      /* Out of cmd_list buffer, send QMI_ERR_NO_MEMORY response */
      qmi_pbm_if_send_response( params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NO_MEMORY, NULL, 0, FALSE);
      return;
    }
    /* Fill the delete all info */
    qmi_pbm_if_store_delete_all_pb_info(sess_type, pb_type);

    for(rec_id = 1; rec_id <= last_non_empty_rec_id; rec_id++)
    {
      pbm_rec_id = pbm_session_location_to_record_id(pb_info,rec_id);
      data_buf_size = pbm_session_calculate_fields_size_from_id(pbm_rec_id);
      if(0 == data_buf_size)
      {
        continue;
      }
      cat = qmi_pbm_if_pb_type_to_cat((qmi_pbm_if_pb_type)pb_type);
      UIM_MSG_HIGH_1("Attempting to delete record with id %d",rec_id);
      ret =  pbm_session_record_write (pb_info, &pbm_rec_id, cat, 0, NULL, 0, qmi_pbm_if_write_callback, (void*)params_ptr->header.handle);
      if(ret != PBM_SUCCESS)
      {
        qmi_pbm_cmd_list_public_type cmd_list_info;

        UIM_MSG_HIGH_2("pbm_session_record_write: returned %d for rec_id %d", ret, rec_id);
        qmi_pbm_if_clear_delete_all_pb_info( sess_type, pb_type);
        if(qmi_pbm_cmd_list_query(params_ptr->header.handle, &cmd_list_info) == E_SUCCESS)
        {
          if(qmi_pbm_cmd_list_free(cmd_list_info.hdl) != E_SUCCESS)
          {
            UIM_MSG_ERR_0("Delete all Entry not found in cmd list ");
          }
        }
      }
      break;
    }
  }
  if(ret != PBM_SUCCESS || last_non_empty_rec_id == 0)
  {
    //this condition address below failures
    //Delete operation failed     (OR)
    //No records to be deleted
    //In the above cases send response here
    err_val = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, err_val, NULL, 0, FALSE);
  }
}/*qmi_pbm_if_delete_all_pb_records*/


/*===========================================================================
  FUNCTION QMI_PBM_IF_SEARCH_RECORDS()

  DESCRIPTION
    Search a Phonebook record based on number/name.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_search_records
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  uint8                              sess_type          = 0;
  uint8                              num_len            = 0;
  uint16                             nam_len            = 0;
  uint16                             pb_type            = 0;
  uint16                             rec_count          = 0;
  char                              *number             = NULL;
  char                              *name               = NULL;
  pbm_return_type                    ret                = PBM_SUCCESS;
  pbm_return_type                    ret_init           = PBM_SUCCESS;
  qmi_error_e_type                   err_val            = QMI_ERR_NONE;
  qmi_pbm_if_search_records_s       *in_data_ptr        = NULL;
  qmi_pbm_if_search_records_resp_s  *resp               = NULL;
  qmi_pbm_if_info_item_s            *pbm_if_data_ptr    = NULL;
  qmi_cmd_buf_type                  *cmd_buf_p          = NULL;
  pbm_phonebook_type                 pb_id;
  pbm_cat_e_type                     category;
  pbm_record_id_type                 rec_id;

  ASSERT(params_ptr != NULL);
  cmd_buf_p = (qmi_cmd_buf_type*)params_ptr->header.handle;
  ASSERT(cmd_buf_p != NULL);
  in_data_ptr=(qmi_pbm_if_search_records_s *)&params_ptr->data;
  ASSERT(in_data_ptr != NULL);

  UIM_MSG_HIGH_0("Inside qmi_pbm_if_search_records");

  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if(NULL == pbm_if_data_ptr)
  {
    return;
  }
  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if((pbm_if_data_ptr->data.search_recs_resp = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_search_records_resp_s))) == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    return;
  }

  resp = (qmi_pbm_if_search_records_resp_s *)pbm_if_data_ptr->data.search_recs_resp;

  memset(resp, 0, sizeof(qmi_pbm_if_search_records_resp_s));

  number = QMI_PBM_IF_MEM_ALLOC(sizeof(char) * QMI_PBM_IF_MAX_NUM_LEN);
  if(number == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr->data.search_recs_resp);
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    return;
  }

  memset(number, 0, sizeof(char) * QMI_PBM_IF_MAX_NUM_LEN);

  name = QMI_PBM_IF_MEM_ALLOC(sizeof(char) * QMI_PBM_IF_MAX_TEXT_EXT_LEN * 2);
  if(name == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr->data.search_recs_resp);
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    QMI_PBM_IF_MEM_FREE(number);
    return;
  }

  memset(name, 0, sizeof(char) * QMI_PBM_IF_MAX_TEXT_EXT_LEN * 2);

  sess_type = in_data_ptr->session_type;
  pb_type = in_data_ptr->pb_type;

  UIM_MSG_HIGH_2( "qmi_pbm_if_search_records: sess_type=0x%x pb_type=0x%x",sess_type,pb_type);

  /* check if session type, pb type are valid */
  if(!qmi_pbm_if_is_session_type_valid(sess_type))
  {
    err_val = QMI_ERR_INVALID_SESSION_TYPE;
  }
  else if(!qmi_pbm_if_is_pb_type_valid(pb_type))
  {
    err_val = QMI_ERR_INVALID_PB_TYPE;
  }
  else
  {
    /* Perform other argument checks */
    if(in_data_ptr->number_info_valid == TRUE)
    {
      num_len = in_data_ptr->num_info.num_len;
      memscpy(number, sizeof(char)*QMI_PBM_IF_MAX_NUM_LEN, in_data_ptr->num_info.number, num_len);
      UIM_PBM_MSG_SPRINTF_1( "Search Number in ASCII : %s",number );
      num_len += 1; /* For nul char inclusion as PBM expects this way */
    }
    else if(in_data_ptr->name_info_valid == TRUE)
    {
      nam_len = in_data_ptr->name_info.name_len;
      if(cmd_buf_p->cmd_type == PBMI_CMD_VAL_SEARCH_RECORDS_EXT)
      {
        nam_len = in_data_ptr->name_info.name_len * 2;
      }

      memscpy(name, sizeof(char)*QMI_PBM_IF_MAX_TEXT_EXT_LEN*2, in_data_ptr->name_info.name, nam_len);
      nam_len += 2;/* For nul ucs2 char (0x00 0x00) as PBM expects this way*/
    }
  }

  if(err_val == QMI_ERR_NONE)
  {
    pb_id.device_type = qmi_pbm_if_pb_type_to_device_type((qmi_pbm_if_pb_type)pb_type);
    pb_id.pb_category = qmi_pbm_if_derive_pb_category_from_session(sess_type);
    pb_id.prov_type = qmi_pbm_if_derive_prov_type_from_session(sess_type);
    pb_id.slot_id = qmi_pbm_if_derive_slot_from_session(sess_type);
    category = qmi_pbm_if_pb_type_to_cat((qmi_pbm_if_pb_type)pb_type);

    if(in_data_ptr->number_info_valid == TRUE)
    {
      ret_init = pbm_session_enum_rec_init(pb_id,category, PBM_FIELD_PHONE_GENERIC, (const uint8 *)number, num_len, PBM_CASE_INSENSITIVE|PBM_SEARCHTYPE_STARTSWITH);
    }

    if(in_data_ptr->name_info_valid == TRUE)
    {
      ret_init = pbm_session_enum_rec_init(pb_id,category, PBM_FIELD_NAME, (const uint8 *)name, nam_len, PBM_STRTYPE_UCS2|PBM_CASE_INSENSITIVE|PBM_SEARCHTYPE_STARTSWITH);
    }

    if ( in_data_ptr->number_info_valid == FALSE && in_data_ptr->name_info_valid == FALSE)
    {
      ret_init = pbm_session_enum_rec_init(pb_id, category, PBM_FIELD_NONE,NULL, 0, 0);
    }

    UIM_MSG_HIGH_1("Search rec_init return val : %d", ret_init);

    if(ret_init == PBM_SUCCESS)
    {
      while((ret = pbm_session_enum_next_rec_id(&rec_id)) != PBM_EOF)
      {
        if(ret == PBM_SUCCESS)
        {
          resp->rec_ids[rec_count] = pbm_session_record_id_to_location(rec_id);
          rec_count++;
        }
        else if(ret == PBM_ERROR_NOT_FOUND)
        {
          break;
        }
        else
        {
          err_val=qmi_pbm_if_map_pbm_err_to_qmi(ret);
          break;
        }
      }
    }
    else
    {
      err_val=qmi_pbm_if_map_pbm_err_to_qmi(ret_init);
    }
  }

  resp->error = err_val;
  resp->num_rec_ids = rec_count;
  UIM_MSG_HIGH_1("Number of recs from search operation =%d",rec_count);

  /* Send response here */
  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_SEARCH_RECORDS_RESP;
  pbm_if_data_ptr->header.handle = params_ptr->header.handle;
  qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);

  QMI_PBM_IF_MEM_FREE(number);
  QMI_PBM_IF_MEM_FREE(name);
}/* qmi_pbm_if_search_records */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_EMERGENCY_LIST()

  DESCRIPTION
    Return the consolidated list of emergency.numbers

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_emergency_list
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{

  qmi_pbm_if_ecc_list_type *pbm_ecc_ptr = NULL;

  UIM_MSG_HIGH_0("qmi_pbm_if_get_emergency_list" );

  pbm_ecc_ptr = qmi_pbm_populate_ecc_list();

  if(pbm_ecc_ptr != NULL)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,
                             params_ptr->header.cmd,
                             QMI_ERR_NONE,
                             (void*)pbm_ecc_ptr,
                             sizeof(qmi_pbm_if_ecc_list_type),
                             FALSE);
  }
  else
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,
                             params_ptr->header.cmd,
                             QMI_ERR_NO_MEMORY,
                             NULL,
                             0,
                             FALSE);
  }
} /* qmi_pbm_if_get_emergency_list */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_PB_STATE()

  DESCRIPTION
    Return the current state of the phonebook.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_pb_state
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  uint8 sess_type;
  uint16 pb_type;
  pbm_return_type ret;
  pbm_phonebook_type pb_info;
  qmi_pbm_if_get_pb_state_resp_s rsp;
  pbm_extended_fileinfo_s_type recs_info;
  qmi_error_e_type err;
  qmi_pbm_if_pb_state_e_type state;

  ASSERT( params_ptr != NULL );

  sess_type = params_ptr->data.get_pb_state.session_type;
  pb_type = params_ptr->data.get_pb_state.pb_type;

  UIM_MSG_HIGH_2("qmi_pbm_if_get_pb_state with sess_type=%d, pb_type=%d",sess_type,pb_type );

  /* check if session type, pb type are valid */
  if((err = qmi_pbm_if_check_session_pb_validity(sess_type, pb_type)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err,NULL,0,FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(sess_type, pb_type, &pb_info);

  /* Extract the status of the phonebook from the return value */
  ret = pbm_session_extended_file_info(pb_info, &recs_info);
  UIM_MSG_HIGH_1("pbm_session_extended_file_info returned %d", ret );
  state = qmi_pbm_if_map_return_type_to_pb_state(ret);

  if(state != QMI_PBM_IF_PB_STATE_MAX)
  {
    rsp.state = (uint8) state;
    rsp.error = QMI_ERR_NONE;
    rsp.sess_type = sess_type;
    rsp.pb_type = pb_type;
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NONE,
                              (void*)&rsp,sizeof(rsp),FALSE);
  }
  else
  {
    err = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err,NULL,0,FALSE);
  }
} /* qmi_pbm_if_get_pb_state */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_ALL_GROUPS()

  DESCRIPTION
    Return all the groups..

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_all_groups
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  pbm_phonebook_type                        pb_id;
  uint8                                     i, s_cnt=0, g_cnt=0, k;
  uint8                                     sess_type = QMI_PBM_IF_SESSION_MAX;
  pbm_record_id_type                        rec_id;
  int                                       data_buf_size = 0;
  pbm_field_s_type                          *data_buf;
  int                                       num_fields = 0;
  uint16                                    category;
  void                                      *field_val;
  qmi_pbm_if_get_all_grp_resp_s             *grp_rsp;
  boolean                                   card_present=FALSE;
  qmi_pbm_if_info_item_s                    *pbm_if_data_ptr;

  ASSERT( params_ptr != NULL );
  UIM_MSG_HIGH_0("qmi_pbm_if_get_all_groups ");
  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    return;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if ((pbm_if_data_ptr->data.get_all_grp_resp = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_get_all_grp_resp_s))) == NULL)
  {
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    return;
  }

  grp_rsp = (qmi_pbm_if_get_all_grp_resp_s *)pbm_if_data_ptr->data.get_all_grp_resp;

  memset(grp_rsp, 0, sizeof(qmi_pbm_if_get_all_grp_resp_s));
  for( i = (uint8)QMI_PBM_SLOT_1; i <= (uint8)QMI_MAX_SLOTS; i++)
  {
    if( pbm_is_card_present_in_slot(i) )
    {
      card_present = TRUE;
      break;
    }
  }

 if( card_present )
 {
   for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
   {
     sess_type = pbm_if_info.sess_data[i].sess_type;
     /* reset the group counter for each session */     
     g_cnt = 0 ; 
     /* Check if ADN phonebook is ready */
     if( (sess_type != QMI_PBM_IF_SESSION_MAX) &&
        ((pbm_if_info.sess_data[i].valid_pb_mask & QMI_PBM_IF_PB_ADN) == QMI_PBM_IF_PB_ADN) &&
        (s_cnt < QMI_PBM_IF_MAX_GRP_SESSIONS))
     {
       pb_id.device_type = PBM_GRPNAME;
       pb_id.pb_category = qmi_pbm_if_derive_pb_category_from_session(sess_type);
       pb_id.prov_type = qmi_pbm_if_derive_prov_type_from_session(sess_type);
       pb_id.slot_id = qmi_pbm_if_derive_slot_from_session(sess_type);

       if (PBM_SUCCESS == pbm_session_enum_rec_init(pb_id, (uint16)PBM_CAT_GRPNAME, PBM_FIELD_NONE, NULL, 0,0) )
       {
         /*The record list in PBM is formed. Get the record ids one by one*/
         while(PBM_SUCCESS == pbm_session_enum_next_rec_id(&rec_id))
         {
           /*Get the total size of all the fields in the record using the record id*/
           data_buf_size = pbm_session_calculate_fields_size_from_id(rec_id);
           if ((data_buf = QMI_PBM_IF_MEM_ALLOC(data_buf_size)) == NULL)
           {
             QMI_PBM_IF_MEM_FREE(grp_rsp);
             QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
             return;
           }
           memset(data_buf,'\0',data_buf_size);

           /*read the record*/
           if(PBM_SUCCESS == pbm_session_record_read(rec_id,&category,&num_fields,(uint8*)data_buf, data_buf_size))
           {
             /* Extract the fields in the record */
             for(k=0;k<num_fields;k++)
             {
               field_val = pbm_get_field_data_ptr(data_buf,k);
               if(field_val == NULL)
               {
                 UIM_MSG_ERR_1("Field data[%d] from PBM is NULL", k);
                 break;
               }
               switch(data_buf[k].field_id)
               {
                 case PBM_FIELD_NAME:
                   if(g_cnt < QMI_PBM_IF_MAX_GRP_RECS)
                   {
                     /* Name will be in UCS2. Copy as it is */
                     memscpy(grp_rsp->sess_grps[s_cnt].grp_data[g_cnt].grp_name,
                             sizeof(grp_rsp->sess_grps[s_cnt].grp_data[g_cnt].grp_name),
                             field_val, (data_buf[k].data_len-2));
                     /* Exclude the terminating nul char(of 2 bytes) */
                     grp_rsp->sess_grps[s_cnt].grp_data[g_cnt].grp_name_len = data_buf[k].data_len - 2;
                     grp_rsp->sess_grps[s_cnt].grp_data[g_cnt].grp_id = (uint8) pbm_session_record_id_to_location(rec_id);
                     g_cnt++;
                   }
                   break;
                 default:
                   UIM_MSG_ERR_1("Invalid field with id %d", data_buf[k].field_id);
                   break;
               }
             }
           }
           QMI_PBM_IF_MEM_FREE(data_buf);
         }
       }
       grp_rsp->sess_grps[s_cnt].grp_cnt = g_cnt;
       grp_rsp->sess_grps[s_cnt].sess_type = sess_type;
       s_cnt++;
     }
   }
   grp_rsp->num_of_sessions = s_cnt;
   grp_rsp->error = QMI_ERR_NONE;
 }
 else
 {
   grp_rsp->error = QMI_ERR_NO_SIM;
 }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_ALL_GROUPS_RESP;
  pbm_if_data_ptr->header.handle = params_ptr->header.handle;

  qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
} /* qmi_pbm_if_get_all_groups */


/*===========================================================================
  FUNCTION QMI_PBM_IF_SET_GROUP_INFO()

  DESCRIPTION
    Set the group information..

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_set_group_info
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  qmi_pbm_if_set_grp_info_s  g_info;
  pbm_phonebook_type         pb_id;
  uint8                      f_cnt = 0;
  qmi_error_e_type           err;
  uint32                     fields_ptr_size=0, rec_id=0;
  uint8                      *fields_ptr = NULL;
  pbm_field_s_type           *ptyped_fields_ptr = NULL;
  pbm_return_type            ret;
  pbm_cat_e_type             cat;

  ASSERT( params_ptr != NULL );

  UIM_MSG_HIGH_0("qmi_pbm_if_set_group_info ");

  g_info = params_ptr->data.set_grp_info;

  /* check if session type is valid */
  if(!qmi_pbm_if_is_session_type_valid(g_info.sess_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_INVALID_SESSION_TYPE,NULL,0,FALSE);
    return;
  }
  if((g_info.operation != QMI_PBM_IF_GRP_OPER_ADD) &&
     (g_info.operation != QMI_PBM_IF_GRP_OPER_MODIFY) &&
     (g_info.operation != QMI_PBM_IF_GRP_OPER_DELETE)
    )
  {
    UIM_MSG_HIGH_1("Invalid group operation %d",g_info.operation);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_INTERNAL,NULL,0,FALSE);
    return;
  }

  if( g_info.grp_name_len  >  QMI_PBM_IF_MAX_NAME_LEN  )
  {
    UIM_MSG_HIGH_1("Too long GAS length %d",g_info.grp_name_len);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_PB_TEXT_TOO_LONG,NULL,0,FALSE);
    return;
  }

  pb_id.device_type  = PBM_GRPNAME;
  pb_id.pb_category  = qmi_pbm_if_derive_pb_category_from_session(g_info.sess_type);
  pb_id.prov_type    = qmi_pbm_if_derive_prov_type_from_session(g_info.sess_type);
  pb_id.slot_id      = qmi_pbm_if_derive_slot_from_session(g_info.sess_type);
  cat                = PBM_CAT_GRPNAME;

  if(g_info.grp_id == QMI_PBM_IF_GET_FIRST_AVAILABLE_ID)
  {
    /* New record addition */
    rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
  }
  else
  {
    rec_id = pbm_session_location_to_record_id(pb_id,g_info.grp_id);
  }

  if( (g_info.grp_name_len > 0) && (g_info.operation != QMI_PBM_IF_GRP_OPER_DELETE))
  {
    pbm_field_id_e_type        *field_id   = NULL;
    pbm_field_type_e_type      *field_type = NULL;
    uint32                     *field_size = NULL;
    uint8                       grp_name_local[QMI_PBM_IF_MAX_NAME_LEN + 2];

    field_id = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_field_id_e_type)*QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_id == NULL)
    {
      return;
    }

    memset(field_id,0,sizeof(pbm_field_id_e_type)*QMI_PBM_IF_MAX_FIELDS_IN_REC);

    field_type = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_field_type_e_type)*QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_type == NULL)
    {
      QMI_PBM_IF_MEM_FREE(field_id);
      return;
    }

    memset(field_type,0,sizeof(pbm_field_type_e_type)*QMI_PBM_IF_MAX_FIELDS_IN_REC);

    field_size = QMI_PBM_IF_MEM_ALLOC(sizeof(uint32)*QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_size == NULL)
    {
      QMI_PBM_IF_MEM_FREE(field_id);
      QMI_PBM_IF_MEM_FREE(field_type);
      return;
    }

    memset(field_size,0,sizeof(uint32)*QMI_PBM_IF_MAX_FIELDS_IN_REC);

    field_id[f_cnt]   = PBM_FIELD_NAME;
    field_type[f_cnt] = PBM_FT_UCS2;
    /* Since ucs2 char is of 2 bytes, Add 2 for terminating nul char.
       Amss pbm expects a nul char at the end of the name. */
    field_size[f_cnt] = g_info.grp_name_len + 2;
    f_cnt++;

    /* Allocate the memory for the fields */
    if(!pbm_allocate_fields(&fields_ptr, (uint32 *)&fields_ptr_size, f_cnt, (const uint32 *)field_size))
    {
      qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
      QMI_PBM_IF_MEM_FREE(field_id);
      QMI_PBM_IF_MEM_FREE(field_type);
      QMI_PBM_IF_MEM_FREE(field_size);
      return;
    }

    ptyped_fields_ptr = (pbm_field_s_type*)fields_ptr;

    memset(grp_name_local, 0, sizeof(grp_name_local));
    memscpy(grp_name_local, sizeof(grp_name_local), g_info.grp_name, g_info.grp_name_len);
    pbm_fill_field(&ptyped_fields_ptr[f_cnt-1], field_id[f_cnt-1], field_type[f_cnt-1], (const byte *)grp_name_local, (uint16)field_size[f_cnt-1]);

    QMI_PBM_IF_MEM_FREE(field_id);
    QMI_PBM_IF_MEM_FREE(field_type);
    QMI_PBM_IF_MEM_FREE(field_size);
  }
  else
  {
    UIM_MSG_HIGH_1("Group with id %d being deleted",g_info.grp_id);
  }

  /* Validate the Record */
  ret =  pbm_session_record_validate(pb_id, rec_id, cat, fields_ptr,(int)fields_ptr_size, (int)f_cnt);
  if (ret != PBM_SUCCESS)
  {
    err = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err,NULL,0,FALSE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Add entry to the command list and wait for callback */
  if ( qmi_pbm_cmd_list_new( params_ptr->header.cmd, params_ptr->header.handle,
                                                 QMI_PBM_CMD_AWAITING_CALLBACK,
                                                 (uint32)g_info.sess_type) != E_SUCCESS )
  {
    UIM_MSG_HIGH_0( "Could not add entry to the list ");
    /* Out of cmd_list buffer, send QMI_ERR_NO_MEMORY response */
    qmi_pbm_if_send_response( params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Validation successful. Write the record now */
  ret = pbm_session_record_write(pb_id, &rec_id, cat, (int)f_cnt, fields_ptr, (int)fields_ptr_size,
        (PBM_WRITE_CB_FUNC)qmi_pbm_if_write_callback, (void*)params_ptr->header.handle);
  if (ret != PBM_SUCCESS)
  {
    err = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err,NULL,0,TRUE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Free the fields_ptr here */
  pbm_free_fields(&fields_ptr,f_cnt);
} /* qmi_pbm_if_set_group_info */


/*===========================================================================
  FUNCTION QMI_PBM_IF_READ_ALL_HIDDEN_RECORDS()

  DESCRIPTION
    Initiate the hidden records read operation.

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_read_all_hidden_records
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  qmi_pbm_if_internal_hidden_recs_read_ind_s int_ind;
  qmi_pbm_if_read_all_hidden_recs_resp_s resp;
  pbm_phonebook_type pb_id;
  uint16 rec_cnt = 0;
  pbm_record_id_type rec_id, last_hidden_rec = 0;
  uint8 sess_type;
  qmi_pbm_if_cmd_e_type ind_type;
  qmi_cmd_buf_type* cmd_buf_p ;
  ASSERT( params_ptr != NULL );
  cmd_buf_p = (qmi_cmd_buf_type*)params_ptr->header.handle;
  ASSERT( cmd_buf_p != NULL );

  sess_type = params_ptr->data.read_all_hidden_recs.sess_type;

  UIM_MSG_HIGH_1("qmi_pbm_if_read_all_hidden_records: Session Type is %d  ",
                    sess_type);

  /* check if session type is valid */
  if(!qmi_pbm_if_is_session_type_valid(sess_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_INVALID_SESSION_TYPE,NULL,0,FALSE);
    return;
  }

  /* Hidden Records not supported for other than provisioing sessions eg. card sessions */
  if( qmi_pbm_if_derive_pb_category_from_session(sess_type) != PBM_LPB )
  {
    qmi_pbm_if_send_response(params_ptr->header.handle, params_ptr->header.cmd, QMI_ERR_NOT_SUPPORTED, NULL, 0, FALSE);
    return;
  }

  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(sess_type, (uint16)QMI_PBM_IF_PB_ADN))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_PB_DELETE_IN_PROG,NULL,0,FALSE);
    return;
  }

  pb_id.device_type = PBM_ADN;
  pb_id.pb_category = qmi_pbm_if_derive_pb_category_from_session(sess_type);
  pb_id.prov_type = qmi_pbm_if_derive_prov_type_from_session(sess_type);
  pb_id.slot_id = qmi_pbm_if_derive_slot_from_session(sess_type);

  if (PBM_SUCCESS == pbm_session_enum_rec_init(pb_id, (uint16)PBM_CAT_NONE, PBM_FIELD_HIDDEN, NULL, 0,0) )
  {
    /*The record list in PBM is formed. Get the record ids one by one*/
    while(PBM_SUCCESS == pbm_session_enum_next_rec_id(&rec_id))
    {
      UIM_MSG_HIGH_1( "pbm_calculate_fields_size_from_id %d",rec_id);
      /*Get the total size of all the fields in the record using the record id*/
      if(pbm_session_calculate_fields_size_from_id(rec_id) != 0)
      {
        rec_cnt++;
        last_hidden_rec = rec_id;
      }
    }
  }
  resp.error = QMI_ERR_NONE;
  resp.sess_type = sess_type;
  resp.rec_cnt = rec_cnt;
  //copy the ind_type before cmd_buf_p releases from send response
  if(cmd_buf_p->cmd_type == PBMI_CMD_VAL_READ_ALL_HIDDEN_RECORDS_EXT)
  {
    ind_type = QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND;
  }
  else
  {
    ind_type = QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND;
  }
  qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NONE,&resp,sizeof(resp),FALSE);

  if(rec_cnt > 0)
  {
    int_ind.sess_type = sess_type;
    int_ind.last_hidden_rec = last_hidden_rec;
    qmi_pbm_if_send_ind(ind_type,&int_ind,sizeof(int_ind));
  }


} /* qmi_pbm_if_read_all_hidden_records */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_NEXT_EMPTY_RECORD_ID()

  DESCRIPTION
    Get next empty record identifier

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_next_empty_record_id
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  uint8 sess_type;
  uint16 pb_type;
  uint16 rec_id, temp_rec_id, max_recs;
  pbm_record_id_type pbm_rec_id=0;
  pbm_phonebook_type pb_info;
  qmi_error_e_type err_val = QMI_ERR_NONE;
  qmi_pbm_if_get_next_empty_record_id_resp_s resp;
  pbm_return_type pbm_ret = PBM_ERROR;
  pbm_extended_fileinfo_s_type info;

  ASSERT( params_ptr != NULL );

  sess_type = params_ptr->data.nxt_empty_rec_id.session_type;
  pb_type = params_ptr->data.nxt_empty_rec_id.pb_type;
  rec_id = params_ptr->data.nxt_empty_rec_id.record_id;

  UIM_MSG_HIGH_3("qmi_pbm_if_get_next_empty_record_id(): Sess_type %d, pb_type %d rec_id %d",sess_type, pb_type,rec_id );

  /* check if session type, pb type are valid */
  if((err_val = qmi_pbm_if_check_session_pb_validity(sess_type,pb_type)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err_val,NULL,0,FALSE);
    return;
  }

  max_recs = qmi_pbm_if_get_max_records(sess_type, pb_type);
  UIM_MSG_HIGH_1("Last record id in the phonebook is %d",max_recs);
  /* Check if record id is within range */
  if( rec_id > max_recs )
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_INVALID_ID,NULL,0,FALSE);
    return;
  }
  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(sess_type, pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_PB_DELETE_IN_PROG,NULL,0,FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(sess_type, pb_type, &pb_info);
  /* Check if the phonebook is not Ready */
  pbm_ret = pbm_session_extended_file_info(pb_info,&info);
  if(pbm_ret != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_2("Phonebook (%d) is Not Ready (%d) ",pb_info.device_type,pbm_ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,
                             qmi_pbm_if_map_pbm_err_to_qmi(pbm_ret),NULL,0,FALSE);
    return;
  }



  memset(&resp,0,sizeof(resp) );
  resp.error = QMI_ERR_NONE;


  /* Search for the empty record */
  for(temp_rec_id = (rec_id + 1);temp_rec_id <= max_recs ;temp_rec_id++)
  {
    pbm_rec_id = pbm_session_location_to_record_id(pb_info,temp_rec_id);
    if(pbm_session_calculate_fields_size_from_id(pbm_rec_id) == 0)
    {
      resp.record_id = temp_rec_id;
      UIM_MSG_HIGH_1("Empty record found with id %d", temp_rec_id);
      break;
    }
  }

  qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NONE,&resp,sizeof(resp),FALSE);
} /* qmi_pbm_if_get_next_empty_record_id */


/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_NEXT_NON_EMPTY_RECORD_ID()

  DESCRIPTION
    Get next non empty record identifier

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_next_non_empty_record_id
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  uint8 sess_type;
  uint16 pb_type;
  uint16 rec_id, temp_rec_id, max_recs;
  pbm_record_id_type pbm_rec_id=0;
  pbm_phonebook_type pb_info;
  qmi_error_e_type err_val = QMI_ERR_NONE;
  qmi_pbm_if_get_next_non_empty_record_id_resp_s resp;
  pbm_return_type pbm_ret = PBM_ERROR;
  pbm_extended_fileinfo_s_type info;

  ASSERT( params_ptr != NULL );

  sess_type = params_ptr->data.nxt_non_empty_rec_id.session_type;
  pb_type = params_ptr->data.nxt_non_empty_rec_id.pb_type;
  rec_id = params_ptr->data.nxt_non_empty_rec_id.record_id;

  UIM_MSG_HIGH_3("qmi_pbm_if_get_next_non_empty_record_id(): Sess_type %d, pb_type %d rec_id %d",sess_type, pb_type,rec_id );

  /* check if session type, pb type are valid */
  if((err_val = qmi_pbm_if_check_session_pb_validity(sess_type,pb_type)) != QMI_ERR_NONE)
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err_val,NULL,0,FALSE);
    return;
  }

  max_recs = qmi_pbm_if_get_max_records(sess_type, pb_type);
  UIM_MSG_HIGH_1("Last record id in the phonebook is %d",max_recs);
  /* Check if record id is within range */
  if( rec_id > max_recs )
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_INVALID_ID,NULL,0,FALSE);
    return;
  }
  /* Check if the phonebook is being deleted */
  if(qmi_pbm_if_is_pb_being_deleted(sess_type, pb_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_PB_DELETE_IN_PROG,NULL,0,FALSE);
    return;
  }

  /* Fill the pbm_phonebook_type */
  qmi_pbm_if_fill_pb_type_info(sess_type, pb_type, &pb_info);
  /* Check if the phonebook is not Ready */
  pbm_ret = pbm_session_extended_file_info(pb_info,&info);
  if(pbm_ret != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_2("Phonebook (%d) is Not Ready (%d) ",pb_info.device_type,pbm_ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,
                             qmi_pbm_if_map_pbm_err_to_qmi(pbm_ret),NULL,0,FALSE);
    return;
  }

  memset(&resp,0,sizeof(resp) );
  resp.error = QMI_ERR_NONE;


  /* Search for the non empty record */
  for(temp_rec_id = (rec_id + 1);temp_rec_id <= max_recs ;temp_rec_id++)
  {
    pbm_rec_id = pbm_session_location_to_record_id(pb_info,temp_rec_id);
    if(pbm_session_calculate_fields_size_from_id(pbm_rec_id) != 0)
    {
      resp.record_id = temp_rec_id;
      UIM_MSG_HIGH_1("Non Empty record found with id %d", temp_rec_id);
      break;
    }
  }

  qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NONE,&resp,sizeof(resp),FALSE);
} /* qmi_pbm_if_get_next_non_empty_record_id */



/*===========================================================================
  FUNCTION QMI_PBM_IF_GET_ALL_AAS()

  DESCRIPTION
    Return all the Additional number Alpha strings..

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_get_all_aas
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  pbm_phonebook_type                       pb_id;
  uint8                                    i, s_cnt=0, a_cnt=0, k;
  uint8                                    sess_type = QMI_PBM_IF_SESSION_MAX;
  pbm_record_id_type                       rec_id;
  int                                      data_buf_size = 0;
  pbm_field_s_type                         *data_buf;
  int                                      num_fields = 0;
  uint16                                   category;
  void                                     *field_val;
  qmi_pbm_if_get_all_aas_resp_s            *aas_rsp;
  boolean                                  card_present=FALSE;
  qmi_pbm_if_info_item_s                   *pbm_if_data_ptr;

  ASSERT( params_ptr != NULL );
  UIM_MSG_HIGH_0("qmi_pbm_if_get_all_aas ");
  pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(pbm_if_data_ptr)));
  if( NULL == pbm_if_data_ptr )
  {
    return;
  }

  memset(pbm_if_data_ptr, 0, sizeof(*(pbm_if_data_ptr)));

  if ((pbm_if_data_ptr->data.get_all_aas_resp = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_get_all_aas_resp_s))) == NULL)
  {
    UIM_MSG_HIGH_0("modem_mem_alloc() failed for get all AAS");
    QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
    return;
  }

  aas_rsp = (qmi_pbm_if_get_all_aas_resp_s *)pbm_if_data_ptr->data.get_all_aas_resp;

  memset(aas_rsp, 0, sizeof(qmi_pbm_if_get_all_aas_resp_s));
  for( i = (uint8)QMI_PBM_SLOT_1; i <= (uint8)QMI_MAX_SLOTS; i++)
  {
    if( pbm_is_card_present_in_slot(i) )
    {
      card_present = TRUE;
      break;
    }
  }

  if( card_present )
  {
  for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
  {
    sess_type = pbm_if_info.sess_data[i].sess_type;
    /* Check if ADN phonebook is ready */
    if( (sess_type != QMI_PBM_IF_SESSION_MAX) &&
        ((pbm_if_info.sess_data[i].valid_pb_mask & QMI_PBM_IF_PB_ADN) == QMI_PBM_IF_PB_ADN) &&
        (s_cnt < QMI_PBM_IF_MAX_AAS_SESSIONS))
    {
      pb_id.device_type = PBM_AAS;
      pb_id.pb_category = qmi_pbm_if_derive_pb_category_from_session(sess_type);
      pb_id.prov_type = qmi_pbm_if_derive_prov_type_from_session(sess_type);
      pb_id.slot_id = qmi_pbm_if_derive_slot_from_session(sess_type);
      a_cnt=0;

      if (PBM_SUCCESS == pbm_session_enum_rec_init(pb_id, (uint16)PBM_CAT_AAS, PBM_FIELD_NONE, NULL, 0,0) )
      {
        /*The record list in PBM is formed. Get the record ids one by one*/
        while(PBM_SUCCESS == pbm_session_enum_next_rec_id(&rec_id))
        {
          /*Get the total size of all the fields in the record using the record id*/
          data_buf_size = pbm_session_calculate_fields_size_from_id(rec_id);
          if ((data_buf = QMI_PBM_IF_MEM_ALLOC(data_buf_size)) == NULL)
          {
            UIM_MSG_ERR_1("modem_mem_alloc() failed for data_buf of size %d",data_buf_size);
            QMI_PBM_IF_MEM_FREE(aas_rsp);
            QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
            return;
          }
          memset(data_buf,'\0',data_buf_size);

          UIM_MSG_HIGH_1( "pbm_session_record_read() for rec_id %d", rec_id );
          /*read the record*/
          if(PBM_SUCCESS == pbm_session_record_read(rec_id,&category,&num_fields,(uint8*)data_buf, data_buf_size))
          {
            /* Extract the fields in the record */
            for(k=0;k<num_fields;k++)
            {
              field_val = pbm_get_field_data_ptr(data_buf,k);
              if(field_val == NULL)
              {
                UIM_MSG_ERR_1("Field data[%d] from PBM is NULL", k);
                break;
              }
              switch(data_buf[k].field_id)
              {
                case PBM_FIELD_NAME:
                  if(a_cnt < QMI_PBM_IF_MAX_AAS_RECS)
                  {
                    /* Name will be in UCS2. Copy as is */
                    memscpy(aas_rsp->sess_aas[s_cnt].aas_data[a_cnt].alpha,
                            sizeof(aas_rsp->sess_aas[s_cnt].aas_data[a_cnt].alpha),
                            field_val, (data_buf[k].data_len-2));
                    /* Exclude the terminating nul char(of 2 bytes) */
                    aas_rsp->sess_aas[s_cnt].aas_data[a_cnt].aas_len = data_buf[k].data_len - 2;
                    aas_rsp->sess_aas[s_cnt].aas_data[a_cnt].aas_id = (uint8) pbm_session_record_id_to_location(rec_id);
                    a_cnt++;
                  }
                  break;
                default:
                  UIM_MSG_ERR_1("Invalid field with id %d", data_buf[k].field_id);
                  break;
              }
            }
          }
          QMI_PBM_IF_MEM_FREE(data_buf);
        }
      }
      aas_rsp->sess_aas[s_cnt].aas_cnt = a_cnt;
      aas_rsp->sess_aas[s_cnt].sess_type = sess_type;
      s_cnt++;
    }
  }
  aas_rsp->num_of_sessions = s_cnt;
  aas_rsp->error = QMI_ERR_NONE;
  }
  else
  {
    aas_rsp->error = QMI_ERR_NO_SIM;
  }

  pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_GET_ALL_AAS_RESP;
  pbm_if_data_ptr->header.handle = params_ptr->header.handle;

  qmi_pbm_encode_and_send_msg(pbm_if_data_ptr);
} /* qmi_pbm_if_get_all_aas */


/*===========================================================================
  FUNCTION QMI_PBM_IF_SET_AAS()

  DESCRIPTION
    Set the AAS information..

  PARAMETERS
    params_ptr        : the qmi_pbm_if_info_item_s pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_set_aas
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  qmi_pbm_if_set_aas_s    a_info;
  pbm_phonebook_type      pb_id;
  uint8                   f_cnt = 0;
  qmi_error_e_type        err;
  uint32                  fields_ptr_size=0, rec_id=0;
  uint8                   *fields_ptr = NULL;
  pbm_field_s_type        *ptyped_fields_ptr = NULL;
  pbm_return_type         ret;
  pbm_cat_e_type          cat;

  ASSERT( params_ptr != NULL );

  UIM_MSG_HIGH_0("qmi_pbm_if_set_aas ");

  a_info = params_ptr->data.set_aas;

  /* check if session type is valid */
  if(!qmi_pbm_if_is_session_type_valid(a_info.sess_type))
  {
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_INVALID_SESSION_TYPE,NULL,0,FALSE);
    return;
  }
  if((a_info.operation != QMI_PBM_IF_AAS_OPER_ADD) &&
     (a_info.operation != QMI_PBM_IF_AAS_OPER_MODIFY) &&
     (a_info.operation != QMI_PBM_IF_AAS_OPER_DELETE)
    )
  {
    UIM_MSG_HIGH_1("Invalid AAS operation %d",a_info.operation);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_INTERNAL,NULL,0,FALSE);
    return;
  }

  if(  a_info.alpha_len  > QMI_PBM_IF_MAX_NAME_LEN )
  {
      UIM_MSG_HIGH_1("Too long AAS length  %d ", a_info.alpha_len);
      qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_PB_TEXT_TOO_LONG,NULL,0,FALSE);
      return;
  }

  pb_id.device_type = PBM_AAS;
  pb_id.pb_category = qmi_pbm_if_derive_pb_category_from_session(a_info.sess_type);
  pb_id.prov_type = qmi_pbm_if_derive_prov_type_from_session(a_info.sess_type);
  pb_id.slot_id = qmi_pbm_if_derive_slot_from_session(a_info.sess_type);
  cat = PBM_CAT_AAS;

  if(a_info.aas_id == QMI_PBM_IF_GET_FIRST_AVAILABLE_ID)
  {
    /* New record addition */
    rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
  }
  else
  {
    rec_id = pbm_session_location_to_record_id(pb_id,a_info.aas_id);
  }

  if( (a_info.alpha_len > 0) && (a_info.operation != QMI_PBM_IF_AAS_OPER_DELETE))
  {

    pbm_field_id_e_type        *field_id   = NULL;
    pbm_field_type_e_type      *field_type = NULL;
    uint32                     *field_size = NULL;
    uint8                       alpha_local[QMI_PBM_IF_MAX_NAME_LEN + 2];

    field_id = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_field_id_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_id == NULL)
    {
      return;
    }

    memset(field_id,0,sizeof(pbm_field_id_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);

    field_type = QMI_PBM_IF_MEM_ALLOC(sizeof(pbm_field_type_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_type == NULL)
    {
      QMI_PBM_IF_MEM_FREE(field_id);
      return;
    }

    memset(field_type,0,sizeof(pbm_field_type_e_type) * QMI_PBM_IF_MAX_FIELDS_IN_REC);

    field_size = QMI_PBM_IF_MEM_ALLOC(sizeof(uint32) * QMI_PBM_IF_MAX_FIELDS_IN_REC);
    if(field_size == NULL)
    {
      QMI_PBM_IF_MEM_FREE(field_id);
      QMI_PBM_IF_MEM_FREE(field_type);
      return;
    }

    memset(field_size, 0, sizeof(uint32) * QMI_PBM_IF_MAX_FIELDS_IN_REC);

    field_id[f_cnt]   = PBM_FIELD_NAME;
    field_type[f_cnt] = PBM_FT_UCS2;
    /* Since ucs2 char is of 2 bytes, Add 2 for terminating nul char.
       Amss pbm expects a nul char at the end of the name. */
    field_size[f_cnt] = a_info.alpha_len + 2;
    f_cnt++;

    /* Allocate the memory for the fields */
    if(!pbm_allocate_fields(&fields_ptr, (uint32 *)&fields_ptr_size, f_cnt, (const uint32 *)field_size))
    {
      qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
      QMI_PBM_IF_MEM_FREE(field_id);
      QMI_PBM_IF_MEM_FREE(field_type);
      QMI_PBM_IF_MEM_FREE(field_size);
      return;
    }

    ptyped_fields_ptr = (pbm_field_s_type*)fields_ptr;

    /* Now fill each field */
    memset(alpha_local, 0, sizeof(alpha_local));
    memscpy(alpha_local, sizeof(alpha_local), a_info.alpha, a_info.alpha_len);
    pbm_fill_field(&ptyped_fields_ptr[f_cnt-1], field_id[f_cnt-1], field_type[f_cnt-1], (const byte *)alpha_local, (uint16)field_size[f_cnt-1]);

    QMI_PBM_IF_MEM_FREE(field_id);
    QMI_PBM_IF_MEM_FREE(field_type);
    QMI_PBM_IF_MEM_FREE(field_size);
  }
  else
  {
    UIM_MSG_HIGH_1("AAS with id %d being deleted",a_info.aas_id);
  }

  /* Validate the Record */
  ret =  pbm_session_record_validate(pb_id, rec_id, cat, fields_ptr,(int)fields_ptr_size, (int)f_cnt);
  if (ret != PBM_SUCCESS)
  {
    err = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err,NULL,0,FALSE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Add entry to the command list and wait for callback */
  if ( qmi_pbm_cmd_list_new( params_ptr->header.cmd, params_ptr->header.handle,
                                                 QMI_PBM_CMD_AWAITING_CALLBACK,
                                                 (uint32)a_info.sess_type) != E_SUCCESS )
  {
    UIM_MSG_HIGH_0( "Could not add entry to the list ");
    /* Out of cmd_list buffer, send QMI_ERR_NO_MEMORY response */
    qmi_pbm_if_send_response( params_ptr->header.handle,params_ptr->header.cmd,QMI_ERR_NO_MEMORY,NULL,0,FALSE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Validation successful. Write the record now */
  ret = pbm_session_record_write(pb_id, &rec_id, cat, (int)f_cnt, fields_ptr, (int)fields_ptr_size,
        (PBM_WRITE_CB_FUNC)qmi_pbm_if_write_callback, (void*)params_ptr->header.handle);
  if (ret != PBM_SUCCESS)
  {
    err = qmi_pbm_if_map_pbm_err_to_qmi(ret);
    qmi_pbm_if_send_response(params_ptr->header.handle,params_ptr->header.cmd,err,NULL,0,TRUE);
    pbm_free_fields(&fields_ptr,f_cnt);
    return;
  }

  /* Free the fields_ptr here */
  pbm_free_fields(&fields_ptr,f_cnt);
} /* qmi_pbm_if_set_aas */


/*===========================================================================
  FUNCTION qmi_pbm_if_handle_pb_cache_status_update()

  DESCRIPTION
    Handle phonebook cache status update event

  PARAMETERS
    notify_data  : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_handle_pb_cache_status_update
(
  pbm_notify_data_s_type *notify_data
)
{
  uint8       i = 0;
  uint8       session_type;
  uint16      pb_type;

  UIM_MSG_HIGH_2("qmi_pbm_if_handle_pb_cache_status_update: Device %d, prov type %d received from PBM",
                    notify_data->session_data.pb_id.device_type,
                    notify_data->session_data.pb_id.prov_type );

  session_type = qmi_pbm_if_derive_session_from_pb_type(&notify_data->session_data.pb_id);
  pb_type      = qmi_pbm_if_device_type_to_pb_type(notify_data->session_data.pb_id.device_type);
  if(!qmi_pbm_if_is_session_type_valid(session_type))
  {
    UIM_MSG_ERR_1( "Invalid Session type %d in qmi_pbm_if_handle_pb_cache_status_update",session_type);
    return;
  }

  if(!qmi_pbm_if_is_pb_type_valid(pb_type))
  {
    UIM_MSG_ERR_1( "Invalid device type= %d in qmi_pbm_if_handle_pb_cache_status_update",
                       notify_data->session_data.pb_id.device_type);
    return;
  }

  for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
  {
    if( pbm_if_info.sess_data[i].sess_type == session_type )
    {
      break;
    }
  }

  if(i >= QMI_PBM_IF_MAX_SESSIONS)
    return;

  switch(notify_data->session_data.pb_id.device_type)
  {
    case PBM_ADN:
    {
      pbm_extended_fileinfo_s_type recs_info;
      pbm_return_type ret;

      /* Extract the status of the phonebook from the return value */
      ret = pbm_session_extended_file_info(notify_data->session_data.pb_id,&recs_info);
      UIM_MSG_HIGH_1("pbm_session_extended_file_info returned %d", ret );
      pbm_if_info.sess_data[i].pb_data.max_adn_recs = 0;
      if(ret == PBM_ERROR_RESTRICTED)
      {
        /* Clearing FDN bit mask and max_fdn_recs when adn status restricted */
        QMI_PBM_CLEAR_BIT(pbm_if_info.sess_data[i].valid_pb_mask,QMI_PBM_IF_PB_FDN);
        pbm_if_info.sess_data[i].pb_data.max_fdn_recs = 0;
      }
    }
      break;
    case PBM_FDN:
      pbm_if_info.sess_data[i].pb_data.max_fdn_recs = 0;
      break;
    case PBM_MSISDN:
      pbm_if_info.sess_data[i].pb_data.max_msisdn_recs = 0;
      break;
    case PBM_MBDN:
      pbm_if_info.sess_data[i].pb_data.max_mbdn_recs = 0;
      break;
    case PBM_MBN:
      pbm_if_info.sess_data[i].pb_data.max_mbn_recs = 0;
      break;
    case PBM_SDN:
      pbm_if_info.sess_data[i].pb_data.max_sdn_recs = 0;
      break;
    case PBM_BDN:
      pbm_if_info.sess_data[i].pb_data.max_bdn_recs = 0;
      break;
    case PBM_LND:
      pbm_if_info.sess_data[i].pb_data.max_lnd_recs = 0;
      break;
    case PBM_GRPNAME:
      pbm_if_info.sess_data[i].pb_data.max_gas_recs = 0;
      break;
    case PBM_AAS:
      pbm_if_info.sess_data[i].pb_data.max_aas_recs = 0;
      break;
    case PBM_DEVICE_TYPE_ALL:
    {
      pbm_if_info.sess_data[i].pb_data.max_adn_recs     = 0;
      pbm_if_info.sess_data[i].pb_data.max_fdn_recs     = 0;
      pbm_if_info.sess_data[i].pb_data.max_msisdn_recs  = 0;
      pbm_if_info.sess_data[i].pb_data.max_mbdn_recs    = 0;
      pbm_if_info.sess_data[i].pb_data.max_mbn_recs     = 0;
      pbm_if_info.sess_data[i].pb_data.max_sdn_recs     = 0;
      pbm_if_info.sess_data[i].pb_data.max_bdn_recs     = 0;
      pbm_if_info.sess_data[i].pb_data.max_lnd_recs     = 0;
      pbm_if_info.sess_data[i].pb_data.max_gas_recs     = 0;
      pbm_if_info.sess_data[i].pb_data.max_aas_recs     = 0;
      /* Clearing PB bit mask */
      pbm_if_info.sess_data[i].valid_pb_mask = 0;
      return;
    }
    default:
      UIM_MSG_HIGH_1("Device type (%d) not supported ",
                      notify_data->session_data.pb_id.device_type);
      return;
    }

    /* Clearing PB bit mask and max_adn_recs */
    QMI_PBM_CLEAR_BIT(pbm_if_info.sess_data[i].valid_pb_mask,pb_type);
} /* qmi_pbm_if_handle_pb_cache_status_update() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_HANDLE_PB_READY()

  DESCRIPTION
    Handle phonebook ready event

  PARAMETERS
    notify_data        : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_handle_pb_ready
(
  pbm_notify_data_s_type *notify_data
)
{
  qmi_pbm_if_pb_ready_ind_s pb_ready_ind;
  uint8 i=0;
  pbm_return_type file_attr_result=PBM_SUCCESS;
  pbm_extended_fileinfo_s_type file_info;
  qmi_pbm_if_ecc_list_type *pbm_ecc_ptr=NULL;
  boolean session_already_set=FALSE;

  UIM_MSG_HIGH_2("PB ReadyInd: Device %d, prov type %d received from PBM", notify_data->session_data.pb_id.device_type,
                            notify_data->session_data.pb_id.prov_type );

  memset(&pb_ready_ind,0,sizeof(qmi_pbm_if_pb_ready_ind_s));

  pb_ready_ind.session_type = qmi_pbm_if_derive_session_from_pb_type(&notify_data->session_data.pb_id);
  pb_ready_ind.pb_type = qmi_pbm_if_device_type_to_pb_type(notify_data->session_data.pb_id.device_type);

  switch(notify_data->session_data.pb_id.device_type)
  {
    case PBM_ECC:
    case PBM_ECC_OTHER:
      break;
    default:
      if(!qmi_pbm_if_is_session_type_valid(pb_ready_ind.session_type))
      {
        UIM_MSG_ERR_1( "Invalid Session type %d in PB Ready",pb_ready_ind.session_type);
        return;
      }
      if(!qmi_pbm_if_is_pb_type_valid(pb_ready_ind.pb_type))
      {
        UIM_MSG_ERR_1( "Invalid device type= %d in PB Ready",notify_data->session_data.pb_id.device_type);
        return;
      }
      for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
      {
        if( pbm_if_info.sess_data[i].sess_type == pb_ready_ind.session_type )
        {
          pbm_if_info.sess_data[i].valid_pb_mask |= pb_ready_ind.pb_type;
          session_already_set = TRUE;
          break;
        }
      }
      if(session_already_set == FALSE)
      {
        for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
        {
          if( pbm_if_info.sess_data[i].sess_type == QMI_PBM_IF_SESSION_MAX )
          {
            pbm_if_info.sess_data[i].sess_type = pb_ready_ind.session_type;
            pbm_if_info.sess_data[i].valid_pb_mask |= pb_ready_ind.pb_type;
            break;
          }
        }
      }
      file_attr_result = pbm_session_extended_file_info (notify_data->session_data.pb_id,&file_info);
      if(file_attr_result == PBM_SUCCESS)
      {
        qmi_pbm_if_store_max_records_data(pb_ready_ind.session_type,pb_ready_ind.pb_type,(uint16)file_info.number_of_records);
      }
      else
      {
        UIM_MSG_ERR_1("Error in PB Ready in getting file info file_attr_result:%d",file_attr_result);
      }
      break;
  }

  /* Send the emergency indication to control point */
  if( (notify_data->session_data.pb_id.device_type == PBM_ECC) || (notify_data->session_data.pb_id.device_type == PBM_ECC_OTHER) )
  {

   pbm_ecc_ptr = qmi_pbm_populate_ecc_list();

    if ( pbm_ecc_ptr != NULL )
      {
        UIM_MSG_ERR_1("Inside qmi_pbm_if_handle_pb_ready  Sending emergency list Ind 0x%x" , pbm_ecc_ptr);
        qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_EMERGENCY_LIST_IND, pbm_ecc_ptr , sizeof(qmi_pbm_if_ecc_list_type));
    }
  }
  else
  {
    qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_PB_READY_IND, &pb_ready_ind, sizeof(pb_ready_ind));
  }

} /* qmi_pbm_if_handle_pb_ready() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_HANDLE_SIM_READY()

  DESCRIPTION
    Handle phonebook ready event

  PARAMETERS
    notify_data        : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_handle_sim_ready
(
  pbm_notify_data_s_type *notify_data
)
{
  qmi_pbm_if_sim_ready_ind_s sim_ready_ind;

  UIM_MSG_HIGH_1("SIM ReadyInd: slot %d received from PBM",
                                 notify_data->data.sim_id);

  memset(&sim_ready_ind,0,sizeof(qmi_pbm_if_sim_ready_ind_s));

  sim_ready_ind.slot_id= qmi_pbm_if_map_slot_to_qmi(notify_data->data.sim_id);
  if ( sim_ready_ind.slot_id == QMI_PBM_SLOT_1 || sim_ready_ind.slot_id == QMI_PBM_SLOT_2
#ifdef FEATURE_TRIPLE_SIM
       || sim_ready_ind.slot_id == QMI_PBM_SLOT_3
#endif
     )
  {
  pbm_if_info.sim_ready[sim_ready_ind.slot_id-1] =  TRUE;
  }
  qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_SIM_READY_IND, &sim_ready_ind, sizeof(sim_ready_ind));

} /* qmi_pbm_if_handle_sim_ready() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_HANDLE_PB_REFRESH()

  DESCRIPTION
    Handle phonebook refresh event

  PARAMETERS
    notify_data        : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_handle_pb_refresh
(
  pbm_notify_data_s_type *notify_data
)
{
  qmi_pbm_if_refresh_ind_s         ind;

  UIM_MSG_HIGH_2("Device %d, prov type %d received from PBM",
                    notify_data->session_data.pb_id.device_type,
                    notify_data->session_data.pb_id.prov_type );

  ind.pb_type      = qmi_pbm_if_device_type_to_pb_type(notify_data->session_data.pb_id.device_type);
  ind.session_type = qmi_pbm_if_derive_session_from_pb_type(&notify_data->session_data.pb_id);

  if((ind.pb_type != QMI_PBM_IF_PB_DEFAULT) && (ind.session_type != QMI_PBM_IF_SESSION_MAX))
  {
    if(notify_data->event == PBM_EVENT_PB_REFRESH_START)
    {
      ind.refresh_status = QMI_PBM_IF_REFRESH_START;
    }
    else if(notify_data->event == PBM_EVENT_PB_REFRESH_DONE)
    {
      ind.refresh_status = QMI_PBM_IF_REFRESH_END;
    }
    else
    {
      UIM_MSG_ERR_1("Invalid pbm event %d handling",notify_data->event);
      return;
    }

    qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_REFRESH_IND, &ind, sizeof(ind));
  }
} /* qmi_pbm_if_handle_pb_refresh() */

/*===========================================================================
  FUNCTION QMI_PBM_IF_SEND_ALL_PB_INIT_DONE_IND()

  DESCRIPTION
    Send the all pb init done indication for the given session
    if sess_type is QMI_PBM_IF_SESSION_MAX, send the data for
    all the current initialized sessions.

  PARAMETERS
    sess_type        : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_send_all_pb_init_done_ind
(
  uint8 sess_type
)
{
  uint8 i, j=0;
  qmi_pbm_if_all_pb_init_done_ind_s ind;

  UIM_MSG_HIGH_0("qmi_pbm_if_send_all_pb_init_done_ind");
  memset(&ind,0, sizeof(ind));

  if(sess_type != QMI_PBM_IF_SESSION_MAX)
  {
    /* Fill the data for just this session */
    ind.num_of_sessions = 1;
    ind.sess_info[0].sess_type = sess_type;
    for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
    {
      if(pbm_if_info.sess_data[i].sess_type == sess_type)
      {
        ind.sess_info[0].pb_mask = pbm_if_info.sess_data[i].valid_pb_mask;
        UIM_MSG_HIGH_2("qmi_pbm_if_send_all_pb_init_done_ind: sess %d, pbmask %d", sess_type,ind.sess_info[0].pb_mask);
        break;
      }
    }
  }
  else
  {
    /* Fill the data for all initialized sessions */
    ind.num_of_sessions = pbm_if_info.num_of_sessions;
    for(i=0;i<QMI_PBM_IF_MAX_SESSIONS;i++)
    {
      if(pbm_if_info.sess_data[i].is_initialized)
      {
        ind.sess_info[j].sess_type = pbm_if_info.sess_data[i].sess_type;
        ind.sess_info[j].pb_mask = pbm_if_info.sess_data[i].valid_pb_mask;
        UIM_MSG_HIGH_2("qmi_pbm_if_send_all_pb_init_done_ind: sess %d, pbmask %d",
                          ind.sess_info[j].sess_type, ind.sess_info[j].pb_mask);
        j++;
      }
    }
  }

  qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_ALL_PB_INIT_DONE_IND,&ind, sizeof(ind));

}/* qmi_pbm_if_send_all_pb_init_done_ind */


/*===========================================================================
  FUNCTION QMI_PBM_IF_HANDLE_SESSION_INIT_DONE()

  DESCRIPTION
    Handle Session init done from pbm

  PARAMETERS
    notify_data        : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_handle_session_init_done
(
  pbm_notify_data_s_type *notify_data
)
{
  uint8               sess_type = 0;
  pbm_phonebook_type *pb_type   = NULL;

  ASSERT(notify_data != NULL);

  pb_type = (pbm_phonebook_type *)&notify_data->session_data.pb_id;
  sess_type = qmi_pbm_if_derive_session_from_pb_type (pb_type);
  UIM_MSG_HIGH_2("qmi_pbm_if_handle_session_init_done Device 0x%x, prov type 0x%x received from PBM", 
                  notify_data->session_data.pb_id.device_type,
                  notify_data->session_data.pb_id.prov_type);

  if(sess_type != QMI_PBM_IF_SESSION_MAX)
  {
    /* Store the initialization status */
    qmi_pbm_if_set_session_init_status(sess_type);

    /* Send all phonebook init done indication for the session */
    qmi_pbm_if_send_all_pb_init_done_ind(sess_type);
  }
  else
  {
    UIM_MSG_ERR_0("Invalid data from pbm for SESSION_INIT_DONE");
  }
} /* qmi_pbm_if_handle_session_init_done */


/*===========================================================================
  FUNCTION QMI_PBM_IF_HANDLE_PB_RECORD_UPDATE()

  DESCRIPTION
    Handle phonebook Record Update event

  PARAMETERS
    notify_data        : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_handle_pb_record_update
(
  pbm_notify_data_s_type *const notify_data
)
{
  qmi_pbm_if_record_update_ind_s  *ind       = NULL;
  qmi_pbm_if_aas_update_ind_s     *aas_ind   = NULL;
  qmi_pbm_if_gas_update_ind_s     *gas_ind   = NULL;
  uint8                            sess_type = 0;
  uint8                            operation = 0;
  uint16                           pb_type   = 0;
  uint16                           rec_id    = 0;
  int                              ind_len   = 0;
  void                            *ind_data  = NULL;
  qmi_pbm_if_cmd_e_type            ind_type;
  pbm_phonebook_type               pb_id;

  ASSERT(notify_data != NULL);

  pb_id = pbm_session_record_id_to_phonebook_type(notify_data->session_data.rec_id);
  sess_type = qmi_pbm_if_derive_session_from_pb_type(&pb_id);
  pb_type = qmi_pbm_if_device_type_to_pb_type(pb_id.device_type);
  operation = (uint8)qmi_pbm_if_map_pbm_event_to_operation(notify_data->event);
  rec_id = pbm_session_record_id_to_location(notify_data->session_data.rec_id);

  UIM_MSG_HIGH_2("qmi_pbm_if_handle_pb_record_update Device 0x%x prov type 0x%x received from PBM",
                  pb_id.device_type, pb_id.prov_type);

  if((sess_type != QMI_PBM_IF_SESSION_MAX) &&
     (operation != QMI_PBM_IF_PB_REC_OP_DEFAULT))
  {
    if(PBM_GRPNAME == pb_id.device_type)
    {
      gas_ind = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_gas_update_ind_s));
      if(gas_ind == NULL)
      {
        return;
      }

      memset(gas_ind, 0, sizeof(qmi_pbm_if_gas_update_ind_s));
      gas_ind->session_type = sess_type;
      gas_ind->operation = operation;
      gas_ind->gas_id = rec_id;
      if(operation != ((uint8)QMI_PBM_IF_PB_REC_OP_DELETE))
      {
        /* Fetch modified GAS string */
        gas_ind->grp_name_len = qmi_pbm_if_fetch_alpha_string(notify_data->session_data.rec_id, gas_ind->grp_name, sizeof(gas_ind->grp_name));
      }

      ind_type = QMI_PBM_IF_CMD_GAS_UPDATE_IND;
      ind_len = sizeof(*gas_ind);
      ind_data = (void*)gas_ind;
    }
    else if(PBM_AAS == pb_id.device_type)
    {
      aas_ind = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_aas_update_ind_s));
      if(aas_ind == NULL)
      {
        return;
      }

      memset(aas_ind, 0, sizeof(qmi_pbm_if_aas_update_ind_s));
      aas_ind->session_type = sess_type;
      aas_ind->operation = operation;
      aas_ind->aas_id = rec_id;
      if(operation != ((uint8)QMI_PBM_IF_PB_REC_OP_DELETE))
      {
        /* Fetch modified AAS string */
        aas_ind->alpha_len = qmi_pbm_if_fetch_alpha_string(notify_data->session_data.rec_id, aas_ind->alpha, sizeof(aas_ind->alpha));
      }

      ind_type = QMI_PBM_IF_CMD_AAS_UPDATE_IND;
      ind_len = sizeof(*aas_ind);
      ind_data = (void*)aas_ind;
    }
    else if(pb_type != QMI_PBM_IF_PB_DEFAULT)
    {
      ind = QMI_PBM_IF_MEM_ALLOC(sizeof(qmi_pbm_if_record_update_ind_s));
      if(ind == NULL)
      {
        return;
      }

      memset(ind, 0, sizeof(qmi_pbm_if_record_update_ind_s));
      ind->session_type = sess_type;
      ind->operation = operation;
      ind->pb_type = pb_type;
      ind->record_id = rec_id;
      ind_type = QMI_PBM_IF_CMD_RECORD_UPDATE_IND;
      ind_len = sizeof(*ind);
      ind_data = (void*)ind;
    }
    else
    {
      UIM_MSG_HIGH_1("Unsupported PBM device type %d", pb_id.device_type );
    }

    if(ind_data != NULL)
    {
      qmi_pbm_if_send_ind(ind_type, ind_data, ind_len);
    }

    QMI_PBM_IF_MEM_FREE(ind);
    QMI_PBM_IF_MEM_FREE(aas_ind);
    QMI_PBM_IF_MEM_FREE(gas_ind);
  }
} /* qmi_pbm_if_handle_pb_record_update() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_FETCH_ALPHA_STRING()

  DESCRIPTION
    Get the AAS, GAS string from PBM for a given record id

  PARAMETERS
    pbm_record_id_type        : PBM record-id
    alpha_buf                 : Pointer to buf for carrying the alpha string

  RETURN VALUE
    Length of the alpha string

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16 qmi_pbm_if_fetch_alpha_string
(
  pbm_record_id_type  pbm_rec_id,
  uint8              *alpha_buf,
  int                 alpha_buf_len
)
{
  int num_fields=0, k;
  uint16 category=0, rec_id=0;
  pbm_field_s_type *data_buf = NULL;
  uint32 data_buf_size = 0;
  uint16 alpha_len = 0;
  pbm_return_type ret = PBM_SUCCESS;
  void *field_val;

  ASSERT(alpha_buf != NULL);

  rec_id = pbm_session_record_id_to_location(pbm_rec_id);
  /* Get the size of record */
  data_buf_size = pbm_session_calculate_fields_size_from_id(pbm_rec_id);
  if(0 == data_buf_size)
  {
    return alpha_len;
  }
  if ((data_buf = QMI_PBM_IF_MEM_ALLOC(data_buf_size)) == NULL)
  {
    return alpha_len;
  }
  memset(data_buf,0,data_buf_size);
  UIM_MSG_HIGH_1("Fetch alpha string with %d from pbm", rec_id);

  /* READ PBM RECORD */
  ret = pbm_session_record_read(pbm_rec_id,&category,&num_fields,(uint8*)data_buf,data_buf_size);
  if( ret != PBM_SUCCESS )
  {
    UIM_MSG_HIGH_2("pbm_session_record_read failed(err %d) for pbm_rec_id %d",ret,pbm_rec_id);
    QMI_PBM_IF_MEM_FREE(data_buf);
    return alpha_len ;
  }

  for(k=0;k<num_fields;k++)
  {
    field_val = pbm_get_field_data_ptr(data_buf,k);
    if(field_val == NULL)
    {
      UIM_MSG_ERR_1("Field data[%d] from PBM is NULL", k);
      QMI_PBM_IF_MEM_FREE(data_buf);
      return alpha_len;
    }
    switch(data_buf[k].field_id)
    {
      case PBM_FIELD_NAME:
        /* Name will be in UCS2 */
        /* Exclude the terminating nul char(of 2 bytes) */
        /* Max supported length is 241 */
        alpha_len = ((data_buf[k].data_len-2) > (QMI_PBM_IF_MAX_TEXT_EXT_LEN*2)) ?
                     (QMI_PBM_IF_MAX_TEXT_EXT_LEN*2) \
                     :(data_buf[k].data_len - 2);
        memscpy(alpha_buf, alpha_buf_len, field_val, alpha_len);
        break;
      default:
        UIM_MSG_HIGH_1("Unsupported field id 0x%x", data_buf[k].field_id);
        break;
    }
  }
  QMI_PBM_IF_MEM_FREE(data_buf);

  return alpha_len;
} /* qmi_pbm_if_fetch_alpha_string() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_HANDLE_HIDDEN_CONTACTS_STATUS()

  DESCRIPTION
    Handle hidden contacts status from pbm

  PARAMETERS
    notify_data        : the pbm_notify_data_s_type pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_handle_hidden_contacts_status
(
  pbm_notify_data_s_type *const notify_data
)
{
  qmi_pbm_if_hidden_record_status_ind_s ind;

  ASSERT(notify_data != NULL);

  UIM_MSG_HIGH_0("qmi_pbm_if_handle_hidden_contacts_status");

  ind.sess_type = qmi_pbm_if_derive_session_from_pb_type(&notify_data->session_data.hidden_contact_status.pb_id);

  if(notify_data->session_data.hidden_contact_status.status == PBM_HIDDEN_CONTACTS_VALID)
  {
    ind.status = QMI_PBM_IF_HIDDEN_RECORDS_VALID;
  }
  else if ( notify_data->session_data.hidden_contact_status.status == PBM_HIDDEN_CONTACTS_INVALID)
  {
    ind.status = QMI_PBM_IF_HIDDEN_RECORDS_INVALID;
  }
  else
  {
    UIM_MSG_HIGH_1("Invalid hidden status from pbm %d", notify_data->session_data.hidden_contact_status.status);
    return;
  }
  qmi_pbm_if_send_ind(QMI_PBM_IF_CMD_HIDDEN_RECORD_STATUS_IND, &ind, sizeof(ind));
} /* qmi_pbm_if_handle_hidden_contacts_status */


/*===========================================================================
  FUNCTION QMI_PBM_IF_EVT_CB_HANDLER()

  DESCRIPTION
    Handles PBM events

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_evt_cb_handler
(
  pbm_notify_data_s_type *notify_data
)
{

  ASSERT(notify_data != NULL);

  switch(notify_data->event)
  {
    case PBM_EVENT_REC_ADD:
    case PBM_EVENT_REC_UPDATE:
    case PBM_EVENT_REC_DELETE:
      qmi_pbm_if_handle_pb_record_update(notify_data);
      break;
    case PBM_EVENT_PB_REFRESH_START:
    case PBM_EVENT_PB_REFRESH_DONE:
      qmi_pbm_if_handle_pb_refresh(notify_data);
      break;
    case PBM_EVENT_PB_READY:
      qmi_pbm_if_handle_pb_ready(notify_data);
      break;
    case PBM_EVENT_CACHE_STATUS_UPDATE:
      qmi_pbm_if_handle_pb_cache_status_update(notify_data);
      break;
    case PBM_EVENT_SIM_INIT_DONE:
      qmi_pbm_if_handle_sim_ready(notify_data);
      break;
    case PBM_EVENT_SESSION_INIT_DONE:
      qmi_pbm_if_handle_session_init_done(notify_data);
      break;
    case PBM_EVENT_HIDDEN_CONTACTS_STATUS:
      qmi_pbm_if_handle_hidden_contacts_status(notify_data);
      break;
    default:
      return;
  }
}/* qmi_pbm_if_evt_cb_handler() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_CMD_CB_HANDLER()

  DESCRIPTION
    Handles PBM CMD CALLBACK

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_cmd_cb_handler
(
  qmi_pbm_if_info_item_s *const params_ptr
)
{
  qmi_error_e_type err_val = QMI_ERR_NONE;
  boolean cmd_exec_success = TRUE;
  qmi_pbm_if_pbm_write_cb_s  *pbm_cmd_cb = (qmi_pbm_if_pbm_write_cb_s*)(&params_ptr->data);
  qmi_pbm_if_delete_record_resp_s delete_rec_resp;
  qmi_pbm_if_write_record_resp_s  write_resp;
  qmi_pbm_if_set_grp_info_resp_s  grp_info_resp;
  qmi_pbm_if_set_aas_resp_s  aas_resp;
  qmi_pbm_cmd_list_public_type cmd_list_info;
  uint16 rec_id;
  uint8 sess_type;
  pbm_phonebook_type pb_info;
  pbm_record_id_type pbm_rec_id=0;
  pbm_cat_e_type cat;
  int data_buf_size=0;
  qmi_pbm_if_pb_type pb_type;
  pbm_return_type ret;

  ASSERT(params_ptr != NULL);
  ASSERT(pbm_cmd_cb != NULL);

  UIM_MSG_HIGH_2("qmi_pbm_if_cmd_cb_handler callback error 0x%x received for handle 0x%x from PBM",
                  pbm_cmd_cb->data.ret, params_ptr->header.handle);

  if(pbm_cmd_cb->data.ret != PBM_SUCCESS)
  {
    cmd_exec_success = FALSE;
    err_val = qmi_pbm_if_map_pbm_err_to_qmi(pbm_cmd_cb->data.ret);
  }

  if(qmi_pbm_cmd_list_query(params_ptr->header.handle,&cmd_list_info) == E_SUCCESS)
  {
    if ( !cmd_exec_success && (cmd_list_info.cmd != (uint8)QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS))
    {
      /* Send cmd FAILURE response */
      qmi_pbm_if_send_response( cmd_list_info.hdl, (uint16)cmd_list_info.cmd, err_val, NULL ,0, TRUE );
      return;
    }
    else
    {
      switch(cmd_list_info.cmd)
      {
        case QMI_PBM_IF_CMD_WRITE_RECORD:
        case QMI_PBM_IF_CMD_WRITE_RECORD_EXT:
          write_resp.error = err_val;
          write_resp.rec_id = pbm_session_record_id_to_location(pbm_cmd_cb->data.sess_rec_id);
          qmi_pbm_if_send_response(cmd_list_info.hdl,(uint16)cmd_list_info.cmd,err_val,(void*)&write_resp,sizeof(write_resp),TRUE);
          break;

        case QMI_PBM_IF_CMD_DELETE_RECORD:
          delete_rec_resp.record_id = pbm_session_record_id_to_location(pbm_cmd_cb->data.sess_rec_id);
          delete_rec_resp.error = err_val;
          qmi_pbm_if_send_response(cmd_list_info.hdl,(uint16)cmd_list_info.cmd,err_val,(void*)&delete_rec_resp,sizeof(delete_rec_resp),TRUE);
          break;

        case QMI_PBM_IF_CMD_SET_GROUP_INFO:
          grp_info_resp.error = err_val;
          grp_info_resp.grp_id = (uint8)pbm_session_record_id_to_location(pbm_cmd_cb->data.sess_rec_id);
          grp_info_resp.sess_type = (uint8)cmd_list_info.sub_id;
          qmi_pbm_if_send_response(cmd_list_info.hdl,(uint16)cmd_list_info.cmd,err_val,(void*)&grp_info_resp,sizeof(grp_info_resp),TRUE);
          break;

        case QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS:
          rec_id = pbm_session_record_id_to_location(pbm_cmd_cb->data.sess_rec_id);
          pb_info = pbm_session_record_id_to_phonebook_type(pbm_cmd_cb->data.sess_rec_id);
          sess_type = qmi_pbm_if_derive_session_from_pb_type(&pb_info);
          pb_type = qmi_pbm_if_device_type_to_pb_type(pb_info.device_type);
          if(cmd_exec_success)
          {
          if( rec_id < ((uint16)cmd_list_info.sub_id) )
          {
            for(rec_id =(rec_id+1);rec_id <= ((uint16)cmd_list_info.sub_id);rec_id++)
            {
              pbm_rec_id = pbm_session_location_to_record_id(pb_info,rec_id);
              data_buf_size = pbm_session_calculate_fields_size_from_id(pbm_rec_id);
              if(0 == data_buf_size)
              {
                UIM_MSG_HIGH_1("Record with id (%d) is empty on card",rec_id);
                continue;
              }
              cat = qmi_pbm_if_pb_type_to_cat(pb_type);
              UIM_MSG_HIGH_1("Attempting to delete record with id %d",rec_id);
              ret =  pbm_session_record_write ( pb_info,&pbm_rec_id,cat,0,NULL,0,qmi_pbm_if_write_callback,(void*)cmd_list_info.hdl);
              if(ret != PBM_SUCCESS)
              {
                UIM_MSG_HIGH_2("pbm_session_record_write: returned %d for rec_id %d", ret, rec_id);
                  err_val = qmi_pbm_if_map_pbm_err_to_qmi(ret);
                  break;
              }
              return;
            }
          }
          UIM_MSG_HIGH_2("DELETE ALL RECORDS completed for sess %d, pb_type %d", sess_type, pb_type);

          }
          else
          {
            UIM_MSG_HIGH_1("Delete record failed for pbm record id %d", pbm_cmd_cb->data.sess_rec_id);
          }
          qmi_pbm_if_clear_delete_all_pb_info( sess_type, pb_type);
          if ( qmi_pbm_cmd_list_free( cmd_list_info.hdl ) != E_SUCCESS )
          {
            UIM_MSG_ERR_0("Delete all Entry not found in cmd list ");
          }
          qmi_pbm_if_send_response(cmd_list_info.hdl,(uint16)cmd_list_info.cmd,err_val,NULL,0,FALSE);
          break;

        case QMI_PBM_IF_CMD_SET_AAS:
          aas_resp.error = err_val;
          aas_resp.aas_id = (uint8)pbm_session_record_id_to_location(pbm_cmd_cb->data.sess_rec_id);
          aas_resp.sess_type = (uint8)cmd_list_info.sub_id;
          qmi_pbm_if_send_response(cmd_list_info.hdl,(uint16)cmd_list_info.cmd,err_val,(void*)&aas_resp,sizeof(aas_resp),TRUE);
          break;

        default :
          UIM_MSG_ERR_2("PBM command callback recieved for wrong cmd: %d received for handle %lu from PBM", cmd_list_info.cmd, params_ptr->header.handle);
          break;
      }
    }
  }
} /* qmi_pbm_if_cmd_cb_handler */


/*===========================================================================
  FUNCTION QMI_PBM_IF_WRITE_CALLBACK()

  DESCRIPTION
    Pbm Command callback

  PARAMETERS
    cb_data                        : pointer to pbm_writecb_data_s_type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_write_callback
(
  pbm_writecb_data_s_type *cb_data
)
{
  qmi_pbm_cmd_data_buf_type *cmd_ptr;

  ASSERT( cb_data != NULL );

  cmd_ptr = qmi_pbm_get_cmd_data_buf();
  if(NULL == cmd_ptr)
  {
    return;
  }

  cmd_ptr->cmd.pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(cmd_ptr->cmd.pbm_if_data_ptr)));
  if( NULL == cmd_ptr->cmd.pbm_if_data_ptr )
  {
    qmi_pbm_free_cmd_data_buf(cmd_ptr);
    return;
  }

  memset(cmd_ptr->cmd.pbm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.pbm_if_data_ptr)));

  UIM_MSG_HIGH_0("Inside: qmi_pbm_if_write_callback");
  cmd_ptr->cmd.pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_WRITE_CB;
  cmd_ptr->cmd.pbm_if_data_ptr->header.handle = (uint32)cb_data->user_data;
  memscpy( &cmd_ptr->cmd.pbm_if_data_ptr->data.write_cb.data, sizeof(pbm_writecb_data_s_type), cb_data, sizeof(pbm_writecb_data_s_type));

  qmi_pbm_send_cmd(QMI_PBM_CMD_PBM_IF_CB, cmd_ptr);
}/* qmi_pbm_if_write_callback() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_EVENT_CALLBACK()

  DESCRIPTION
    Pbm event callback

  PARAMETERS
    user_data                        : User data
    notify_data                      : Event data from PBM

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_event_callback
(
  void                    *user_data,
  pbm_notify_data_s_type  *notify_data
)
{
  qmi_pbm_cmd_data_buf_type   *cmd_ptr   = NULL;

  ASSERT( notify_data != NULL );

  cmd_ptr = qmi_pbm_get_cmd_data_buf();
  if(NULL == cmd_ptr)
  {
    return;
  }

  cmd_ptr->cmd.pbm_if_data_ptr = QMI_PBM_IF_MEM_ALLOC(sizeof(*(cmd_ptr->cmd.pbm_if_data_ptr)));
  if( NULL == cmd_ptr->cmd.pbm_if_data_ptr )
  {
    qmi_pbm_free_cmd_data_buf(cmd_ptr);
    return;
  }

  memset(cmd_ptr->cmd.pbm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.pbm_if_data_ptr)));

  cmd_ptr->cmd.pbm_if_data_ptr->header.cmd = QMI_PBM_IF_CMD_EVT_CB;
  cmd_ptr->cmd.pbm_if_data_ptr->header.handle = 0;
  memscpy( &cmd_ptr->cmd.pbm_if_data_ptr->data.pbm_evt_cb.data, sizeof(pbm_notify_data_s_type), notify_data, sizeof(pbm_notify_data_s_type));

  qmi_pbm_send_cmd(QMI_PBM_CMD_PBM_EVT_CB, cmd_ptr);
}/* qmi_pbm_if_event_callback() */


/*===========================================================================
  FUNCTION QMI_PBM_IF_PROCESS_CMD()

  DESCRIPTION
    Process the commands posted to PBM Interface layer

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_if_process_cmd
(
  void  * user_data_ptr
)
{
  qmi_pbm_if_info_item_s *     pbm_if_data_ptr = NULL;

  pbm_if_data_ptr = ((qmi_pbm_cmd_data_buf_type*)user_data_ptr)->cmd.pbm_if_data_ptr;

  ASSERT (pbm_if_data_ptr->header.cmd < QMI_PBM_IF_CMD_MAX);

  switch (pbm_if_data_ptr->header.cmd)
  {
    case QMI_PBM_IF_CMD_INDICATION_REGISTER:
      qmi_pbm_if_indication_register(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_PB_CAPABILITIES:
      qmi_pbm_if_get_pb_capabilities(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES:
      qmi_pbm_if_get_all_pb_capabilities(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_READ_RECORDS:
      qmi_pbm_if_read_records(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_READ_PBSET_CAPS:
      qmi_pbm_if_read_pbset_caps(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_WRITE_RECORD:
    case QMI_PBM_IF_CMD_WRITE_RECORD_EXT:
      qmi_pbm_if_write_record(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_DELETE_RECORD:
      qmi_pbm_if_delete_record(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS:
      qmi_pbm_if_delete_all_pb_records(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_SEARCH_RECORDS:
      qmi_pbm_if_search_records(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_EMERGENCY_LIST:
      qmi_pbm_if_get_emergency_list(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_PB_STATE:
      qmi_pbm_if_get_pb_state(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_ALL_GROUPS:
      qmi_pbm_if_get_all_groups(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_SET_GROUP_INFO:
      qmi_pbm_if_set_group_info(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_READ_ALL_HIDDEN_RECORDS:
      qmi_pbm_if_read_all_hidden_records(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID:
      qmi_pbm_if_get_next_empty_record_id(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID:
      qmi_pbm_if_get_next_non_empty_record_id(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_GET_ALL_AAS:
      qmi_pbm_if_get_all_aas(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_SET_AAS:
      qmi_pbm_if_set_aas(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND:
    case QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND:
      qmi_pbm_if_internal_record_read_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_INTERNAL_PBSET_CAPS_READ_IND:
      qmi_pbm_if_internal_pbset_caps_read_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND:
    case QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND:
      qmi_pbm_if_internal_hidden_records_read_ind(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_WRITE_CB:
      qmi_pbm_if_cmd_cb_handler(pbm_if_data_ptr);
      break;
    case QMI_PBM_IF_CMD_EVT_CB:
      qmi_pbm_if_evt_cb_handler( &pbm_if_data_ptr->data.pbm_evt_cb.data);
      break;
   case QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED :
      qmi_pbm_if_read_records_ext_undecoded(pbm_if_data_ptr);
      break;
   case QMI_PBM_IF_CMD_INTERNAL_READ_RECORDS_EXT_UNDECODED_IND :
      qmi_pbm_if_internal_record_read_ext_undecoded_ind(pbm_if_data_ptr);
      break;
    default:
      UIM_MSG_HIGH_1 ("Ignoring cmd 0x%x", pbm_if_data_ptr->header.cmd);
      break;
  }
  QMI_PBM_IF_MEM_FREE(pbm_if_data_ptr);
  return;
}/* qmi_pbm_if_process_cmd */


/*===========================================================================
  FUNCTION QMI_PBM_IF_LOOKUP_CMD_NAME()

  DESCRIPTION
    Look up the qmi pbm interface command name

  PARAMETERS
    cmd        : QMI PBM Iterface Command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
const char *qmi_pbm_if_lookup_cmd_name
(
  uint8 cmd
)
{
  switch(cmd)
  {
    case QMI_PBM_IF_CMD_INDICATION_REGISTER:
      return "PBM IF INDICATION REGISTER";
    case QMI_PBM_IF_CMD_GET_PB_CAPABILITIES:
      return "PBM IF GET PB CAPABILITIES";
    case QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES:
      return "PBM IF GET ALL PB CAPABILITIES";
    case QMI_PBM_IF_CMD_READ_RECORDS:
      return "PBM IF READ RECORDS";
    case QMI_PBM_IF_CMD_WRITE_RECORD:
    case QMI_PBM_IF_CMD_WRITE_RECORD_EXT:
      return "PBM IF WRITE RECORD";
    case QMI_PBM_IF_CMD_DELETE_RECORD:
      return "PBM IF DELETE RECORD";
    case QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS:
        return "PBM IF DELETE ALL PB RECORDS";
    case QMI_PBM_IF_CMD_SEARCH_RECORDS:
      return "PBM IF SEARCH RECORDS";
    case QMI_PBM_IF_CMD_RECORD_UPDATE_IND:
      return "PBM IF RECORD UPDATE IND";
    case QMI_PBM_IF_CMD_REFRESH_IND:
      return "PBM IF REFRESH IND";
    case QMI_PBM_IF_CMD_PB_READY_IND:
      return "PBM IF PB READY IND";
    case QMI_PBM_IF_CMD_EMERGENCY_LIST_IND:
      return "PBM IF EMERGENCY LIST IND";
    case QMI_PBM_IF_CMD_WRITE_CB:
      return "PBM IF COMMAND CB";
    case QMI_PBM_IF_CMD_EVT_CB:
      return "PBM IF EVENT CB";
    case QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID:
      return "PBM IF NEXT EMPTY RECORD ID";
    case QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID:
      return "PBM IF NEXT NON EMPTY RECORD ID";
    case QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED:
      return "PBM IF READ RECORDS EXT UNDECODED";
    default:
      return "<UNKNOWN PBM COMMAND>?";
  }
}/* qmi_pbm_if_lookup_cmd_name() */

