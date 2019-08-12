/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   S S   E V E N T   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the event handling CM ss events for MMGSDI

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   bcho    EONS F3 reduction
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/15/15   stv     SIMLOCK Time changes
02/23/15   ar      Use mm_info RAT for NITZ when Mobility Mask is SET
12/03/14   bcho    featurize support to use stored rplmn name across operators
11/14/14   hh      Fix compilation warnings
09/09/14   kk      Enhancements in NV handling
08/14/14   tl      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_eons_nitz.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_ss_event.h"
#include "simlock_time_nitz.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
         E V E N T    N O T I F I C A T I O N   F U N C T I O N S
============================================================================*/

/*===========================================================================
  FUNCTION mmgsdi_simlock_queue_ss_event_req

DESCRIPTION
  Function to create ss event cmd and queue it

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_simlock_queue_ss_event_req
(
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        slot_id
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     sizeof(mmgsdi_task_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  task_cmd_ptr->cmd.cmd_enum   = MMGSDI_FULL_SERVICE_REQ;
  task_cmd_ptr->cmd.cmd.simlock_ss_event_req.request_header.request_type      =
    MMGSDI_FULL_SERVICE_REQ;
  task_cmd_ptr->cmd.cmd.simlock_ss_event_req.request_header.orig_request_type =
    MMGSDI_FULL_SERVICE_REQ;
  task_cmd_ptr->cmd.cmd.simlock_ss_event_req.request_header.slot_id =
    slot_id;
  task_cmd_ptr->cmd.cmd.simlock_ss_event_req.request_header.session_id =
    session_id;

  /*---------------------------------------------------------------------------
       Calculate the payload len and request len
    ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd.simlock_ss_event_req.request_header.request_len =
    sizeof(mmgsdi_task_cmd_type);
  task_cmd_ptr->cmd.cmd.simlock_ss_event_req.request_header.payload_len =
    sizeof(mmgsdi_task_cmd_type) - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*Free the pointer since the task_cmd_ptr has not been put
      onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  return mmgsdi_status;
} /* mmgsdi_simlock_queue_ss_event_req */


/*===========================================================================
FUNCTION MMGSDI_EONS_COMPUTE_ALIGNED_SS_PLMN_NAME_LEN

DESCRIPTION
  Function to calculate aligned data length for PLMN names in SS info

DEPENDENCIES
  None

RETURN VALUE
  int32

SIDE EFFECTS
  None
===========================================================================*/
static int32 mmgsdi_eons_compute_aligned_ss_plmn_name_len(
  mmgsdi_eons_ss_data_type *ss_info_ptr
)
{
  int32 aligned_plmn_name_len = 0;
  int32 var_len               = 0;

  if (ss_info_ptr == NULL)
  {
    return var_len;
  }

  mmgsdi_util_cal_align_size(
     ss_info_ptr->first_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len,
     &aligned_plmn_name_len);

  var_len += aligned_plmn_name_len;

  aligned_plmn_name_len = 0;
  mmgsdi_util_cal_align_size(
     ss_info_ptr->first_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_len,
     &aligned_plmn_name_len);

  var_len += aligned_plmn_name_len;

  aligned_plmn_name_len = 0;
  mmgsdi_util_cal_align_size(
     ss_info_ptr->sec_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len,
     &aligned_plmn_name_len);

  var_len += aligned_plmn_name_len;

  aligned_plmn_name_len = 0;
  mmgsdi_util_cal_align_size(
     ss_info_ptr->sec_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_len,
     &aligned_plmn_name_len);

  var_len += aligned_plmn_name_len;

  return (sizeof(uint8) * var_len);
} /* mmgsdi_eons_compute_aligned_ss_plmn_name_len */


/*===========================================================================
  FUNCTION mmgsdi_eons_create_cmd_and_queue

DESCRIPTION
  Function to create ss event cmd and queue it

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_create_cmd_and_queue
(
  mmgsdi_eons_ss_data_type       ss_info
)
{
  mmgsdi_task_cmd_type            * task_cmd_ptr   = NULL;
  mmgsdi_eons_ss_event_req_type   * msg_ptr        = NULL;
  mmgsdi_return_enum_type           mmgsdi_status  = MMGSDI_ERROR;
  int32                             total_mem_len  = 0;
  uint32                            task_cmd_len   = 0;
  int32                             var_len        = 0;
  int32                             temp_mem       = 0;


  var_len = mmgsdi_eons_compute_aligned_ss_plmn_name_len(&ss_info);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  mmgsdi_util_cal_align_size(total_mem_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len,temp_mem);

  total_mem_len = temp_mem + var_len;

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if(task_cmd_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.eons_ss_event_req;

  msg_ptr->ss_info.first_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len =
    ss_info.first_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len;

  msg_ptr->ss_info.first_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_len =
    ss_info.first_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_len;

  msg_ptr->ss_info.sec_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len =
    ss_info.sec_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len;

  msg_ptr->ss_info.sec_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_len =
    ss_info.sec_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_len;

  mmgsdi_util_align_mem(msg_ptr, MMGSDI_EONS_SS_EVENT_REQ);

  /*---------------------------------------------------------------------------
       Populating mmgsdi_eons_ss_event_req_type header
        1) Client ID
        2) Request TYPE
        3) Client Data Pointer
        4) Response CallBack
        5) Payload len and request len will be populated after the content
            has been populated
  ---------------------------------------------------------------------------*/
  switch(ss_info.session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      msg_ptr->request_header.session_id =
        mmgsdi_generic_data_ptr->pri_gw_session_id;
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      msg_ptr->request_header.session_id =
        mmgsdi_generic_data_ptr->sec_gw_session_id;
       break;

    case MMGSDI_GW_PROV_TER_SESSION:
      msg_ptr->request_header.session_id =
        mmgsdi_generic_data_ptr->ter_gw_session_id;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      msg_ptr->request_header.session_id =
        mmgsdi_generic_data_ptr->pri_1x_session_id;
      break;

    case MMGSDI_1X_PROV_SEC_SESSION:
      msg_ptr->request_header.session_id =
        mmgsdi_generic_data_ptr->sec_1x_session_id;
      break;

    default:
      MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
      return MMGSDI_ERROR;
  }

  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_EONS_SS_EVENT_REQ;
  msg_ptr->request_header.request_type        = MMGSDI_EONS_SS_EVENT_REQ;
  msg_ptr->request_header.orig_request_type   = MMGSDI_EONS_SS_EVENT_REQ;
  msg_ptr->request_header.response_cb         = mmgsdi_util_internal_cb;
  msg_ptr->ss_info.session_type               = ss_info.session_type;

  if (ss_info.first_stack_ss.is_valid)
  {
    (void)mmgsdi_eons_copy_per_stack_ss_info(&(ss_info.first_stack_ss),
                                             &(msg_ptr->ss_info.first_stack_ss),
                                             FALSE);
  }

  if (ss_info.sec_stack_ss.is_valid)
  {
    (void)mmgsdi_eons_copy_per_stack_ss_info(&(ss_info.sec_stack_ss),
                                             &(msg_ptr->ss_info.sec_stack_ss),
                                             FALSE);
  }

  /*---------------------------------------------------------------------------
       Calculate the payload len and request len
    ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
  total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*Free the pointer since the task_cmd_ptr has not been put
      onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_EONS_SS_EVENT_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_eons_create_cmd_and_queue */


/*===========================================================================
  FUNCTION: MMGSDI_SIMLOCK_CM_SS_EVENT_CB

  DESCRIPTION:
    Handler for CM_SS_EVENT_SRV_CHANGED. It determines whether slot 1
    has reached full service.

  PARAMETERS:
    ss_event         -> cm ss event
    mm_ss_info_ptr   -> more information regarding the ss event
===========================================================================*/
static void mmgsdi_simlock_cm_ss_event_cb(
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type * mm_ss_info_ptr
)
{
  mmgsdi_session_type_enum_type   session_type    = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_id_type          session_id      = 0;
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type        slot_id         = MMGSDI_MAX_SLOT_ID_ENUM;
  sys_srv_status_e_type           srv_status      = SYS_SRV_STATUS_NONE;
  mmgsdi_int_app_info_type      * app_info_ptr    = NULL;

  if(ss_event != CM_SS_EVENT_SRV_CHANGED || mm_ss_info_ptr == NULL)
  {
    return;
  }

  switch (mm_ss_info_ptr->asubs_id)
  {
    case SYS_MODEM_AS_ID_1:
      if (mm_ss_info_ptr->stack_info[0].is_operational)
      {
        if(MMGSDI_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode))
        {
          session_type   = MMGSDI_GW_PROV_PRI_SESSION;
        }
        else if(MMGSDI_3GPP2_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode))
        {
          session_type   = MMGSDI_1X_PROV_PRI_SESSION;
        }
        srv_status       = mm_ss_info_ptr->stack_info[0].srv_status;
      }
      else if(mm_ss_info_ptr->stack_info[1].is_operational &&
              MMGSDI_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[1].sys_mode))
      {
        session_type     = MMGSDI_GW_PROV_PRI_SESSION;
        srv_status       = mm_ss_info_ptr->stack_info[1].srv_status;
      }
      break;

    case SYS_MODEM_AS_ID_2:
      if (mm_ss_info_ptr->stack_info[0].is_operational)
      {
        if(MMGSDI_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode))
        {
          session_type   = MMGSDI_GW_PROV_SEC_SESSION;
        }
        else if(MMGSDI_3GPP2_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode))
        {
          session_type   = MMGSDI_1X_PROV_SEC_SESSION;
        }
        srv_status       = mm_ss_info_ptr->stack_info[0].srv_status;
      }
      break;

    case SYS_MODEM_AS_ID_3:
      if (mm_ss_info_ptr->stack_info[0].is_operational)
      {
        if(MMGSDI_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode))
        {
          session_type   = MMGSDI_GW_PROV_TER_SESSION;
        }
        else if(MMGSDI_3GPP2_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode))
        {
          session_type   = MMGSDI_1X_PROV_TER_SESSION;
        }
        srv_status       = mm_ss_info_ptr->stack_info[0].srv_status;
      }
      break;

    default:
      return;
  }

  if (srv_status != SYS_SRV_STATUS_SRV)
  {
    return;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT

  do
  {
    mmgsdi_status = mmgsdi_util_get_mmgsdi_session_id_from_type(session_type,
                                                                &session_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    mmgsdi_status = mmgsdi_util_get_prov_session_info(
                           session_id,
                           NULL,
                           &slot_id,
                           &app_info_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* If the application is already in Full Service then no
       need to post message to MMGSDI as it would be redundant. */
    if (app_info_ptr->service_status == SIMLOCK_SRV_STATUS_FULL)
    {
      break;
    }

    (void)mmgsdi_simlock_queue_ss_event_req(session_id, slot_id);
  } while (0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT
} /* mmgsdi_simlock_cm_ss_event_cb */


/*===========================================================================
FUNCTION MMGSDI_EONS_COPY_SS_EVENT_MM_INFO

DESCRIPTION
  Function to copy MM information from SS event data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_copy_ss_event_mm_info(
  const sys_mm_information_s_type     * sys_mm_info_ptr,
  mmgsdi_eons_nitz_data_type          * nitz_info_ptr)
{
  if(nitz_info_ptr == NULL || sys_mm_info_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_6("full name available: 0x%x, length: 0x%x, encoding: 0x%x short name available: 0x%x, length: 0x%x, encoding: 0x%x",
                 sys_mm_info_ptr->full_name_avail,
                 sys_mm_info_ptr->full_name.length,
                 sys_mm_info_ptr->full_name.coding_scheme,
                 sys_mm_info_ptr->short_name_avail,
                 sys_mm_info_ptr->short_name.length,
                 sys_mm_info_ptr->short_name.coding_scheme);

  nitz_info_ptr->long_name_available =
    sys_mm_info_ptr->full_name_avail;

  if(sys_mm_info_ptr->full_name_avail &&
     sys_mm_info_ptr->full_name.length > 0)
  {
    nitz_info_ptr->long_name.plmn_name.eons_data.data_len =
      sys_mm_info_ptr->full_name.length;

    nitz_info_ptr->long_name.plmn_name.eons_encoding =
      mmgsdi_eons_map_network_coding_scheme(
        sys_mm_info_ptr->full_name.coding_scheme);

    nitz_info_ptr->long_name.plmn_name_ci =
      sys_mm_info_ptr->full_name.add_country_initials;

    nitz_info_ptr->long_name.plmn_name_spare_bits =
      sys_mm_info_ptr->full_name.spare_bits;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
      nitz_info_ptr->long_name.plmn_name.eons_data.data_len);

    if(nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
                     nitz_info_ptr->long_name.plmn_name.eons_data.data_len,
                     sys_mm_info_ptr->full_name.name,
                     sys_mm_info_ptr->full_name.length);
    }
  }

  nitz_info_ptr->short_name_available =
    sys_mm_info_ptr->short_name_avail;

  if(sys_mm_info_ptr->short_name_avail &&
     sys_mm_info_ptr->short_name.length > 0)
  {
    nitz_info_ptr->short_name.plmn_name.eons_data.data_len =
      sys_mm_info_ptr->short_name.length;

    nitz_info_ptr->short_name.plmn_name.eons_encoding =
      mmgsdi_eons_map_network_coding_scheme(
        sys_mm_info_ptr->short_name.coding_scheme);

    nitz_info_ptr->short_name.plmn_name_ci =
      sys_mm_info_ptr->short_name.add_country_initials;

    nitz_info_ptr->short_name.plmn_name_spare_bits =
      sys_mm_info_ptr->short_name.spare_bits;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
      nitz_info_ptr->short_name.plmn_name.eons_data.data_len);

    if(nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
                     nitz_info_ptr->short_name.plmn_name.eons_data.data_len,
                     sys_mm_info_ptr->short_name.name,
                     sys_mm_info_ptr->short_name.length);
    }
  }
}/* mmgsdi_eons_copy_ss_event_mm_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_COPY_SS_EVENT_RPLMN_LAC_TAC_DATA

DESCRIPTION
  Function to copy RPLMN, LAC, TAC from SS event data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_copy_ss_event_rplmn_lac_tac_data(
  const sys_sys_id_s_type     * sys_id_ptr,
  mmgsdi_eons_nitz_data_type  * nitz_info_ptr,
  sys_sys_mode_e_type           sys_mode,
  sys_lac_type                  tac)
{

  if (sys_id_ptr == NULL || nitz_info_ptr == NULL)
  {
    return;
  }

  if(MMGSDI_EONS_PLMN_ID_VALIDITY_CHECK(sys_id_ptr->id.plmn.identity))
  {
    nitz_info_ptr->rplmn_available = TRUE;

    mmgsdi_memscpy(nitz_info_ptr->rplmn_id.plmn_id_val,
                   sizeof(nitz_info_ptr->rplmn_id.plmn_id_val),
                   sys_id_ptr->id.plmn.identity,
                   MMGSDI_PLMN_ID_SIZE);
  }

  if(sys_mode == SYS_SYS_MODE_LTE)
  {
    if(tac != 0xFFFF)
    {
      nitz_info_ptr->lac_available = TRUE;

      /*TAC is copied from same location (mm_ss_info_ptr->tac) for both MAIN and
      GW HYBRID stack because LTE can be active on only one of MAIN or HYBRID
      GW stack at any given time.*/
      nitz_info_ptr->lac = tac;
    }
  }
  else
  {
    if(sys_id_ptr->id.plmn_lac.lac != 0xFFFF)
    {
      nitz_info_ptr->lac_available = TRUE;
      nitz_info_ptr->lac = sys_id_ptr->id.plmn_lac.lac;
    }
  }

  UIM_MSG_HIGH_6("Received: plmn id [0]:0x%x, id[1]: 0x%x, id[2]: 0x%x RPLMN available: 0x%x, LAC available: 0x%x, LAC: 0x%x",
                sys_id_ptr->id.plmn.identity[0],
                sys_id_ptr->id.plmn.identity[1],
                sys_id_ptr->id.plmn.identity[2],
                nitz_info_ptr->rplmn_available,
                nitz_info_ptr->lac_available,
                nitz_info_ptr->lac);
} /* mmgsdi_eons_copy_ss_event_rplmn_lac_tac_data */


/*===========================================================================
FUNCTION MMGSDI_EONS_COPY_SS_EVENT_CSG_DATA

DESCRIPTION
  Function to copy CSG data from SS event data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_copy_ss_event_csg_data(
  const sys_csg_info_s_type   * csg_info_ptr,
  mmgsdi_eons_nitz_data_type  * nitz_info_ptr,
  uint64                        changed_fields,
  sys_modem_as_id_e_type        asubs_id
)
{
  mmgsdi_nv_context_type efs_index = MMGSDI_NV_CONTEXT_INVALID;

  if (csg_info_ptr == NULL || nitz_info_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_2("Received csg id 0x%x, len: 0x%x",
                 csg_info_ptr->csg_id, csg_info_ptr->hnb_name.length);

  nitz_info_ptr->csg_info.csg_id = MMGSDI_EONS_INVALID_CSG_ID;

  switch (asubs_id)
  {
    case SYS_MODEM_AS_ID_1:
      efs_index = MMGSDI_NV_CONTEXT_PRIMARY;
      break;
    case SYS_MODEM_AS_ID_2:
      efs_index = MMGSDI_NV_CONTEXT_SECONDARY;
      break;
    case SYS_MODEM_AS_ID_3:
      efs_index = MMGSDI_NV_CONTEXT_TERTIARY;
      break;
    default:
      efs_index = MMGSDI_NV_CONTEXT_PRIMARY;
      break;
  }

  if ((mmgsdi_nv_get_external_feature_status(
            MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG, efs_index) == MMGSDI_FEATURE_ENABLED))
  {
    if((changed_fields & CM_SS_EVT_CSG_INFO_CHANGED_MASK) &&
       (csg_info_ptr->csg_id != SYS_CSG_ID_INVALID))
    {
      nitz_info_ptr->csg_info.csg_id = csg_info_ptr->csg_id;

      if((csg_info_ptr->hnb_name.length > 0) &&
         (csg_info_ptr->hnb_name.length <= SYS_HNB_NAME_LEN))
      {
        nitz_info_ptr->csg_info.hnb_name.length =
          csg_info_ptr->hnb_name.length;

        mmgsdi_memscpy(nitz_info_ptr->csg_info.hnb_name.name,
                       nitz_info_ptr->csg_info.hnb_name.length,
                       csg_info_ptr->hnb_name.name,
                       csg_info_ptr->hnb_name.length);
      }
    }
  }
} /* mmgsdi_eons_copy_ss_event_csg_data */


/*===========================================================================
FUNCTION MMGSDI_EONS_MAP_CM_RAT_TO_MMGSDI_RAT

DESCRIPTION
  Function to map CM radio access technology to MMGSDI RAT enum type

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RAT_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_rat_enum_type mmgsdi_eons_map_cm_rat_to_mmgsdi_rat(
  sys_radio_access_tech_e_type            cm_rat_type
)
{
  mmgsdi_rat_enum_type  mmgsdi_rat_type = MMGSDI_RAT_MAX;

  switch(cm_rat_type)
  {
    case SYS_RAT_GSM_RADIO_ACCESS:
      mmgsdi_rat_type = MMGSDI_RAT_GSM;
      break;

    case SYS_RAT_UMTS_RADIO_ACCESS:
      mmgsdi_rat_type = MMGSDI_RAT_UMTS;
      break;

    case SYS_RAT_LTE_RADIO_ACCESS:
      mmgsdi_rat_type = MMGSDI_RAT_LTE;
      break;

    case SYS_RAT_TDS_RADIO_ACCESS:
      mmgsdi_rat_type = MMGSDI_RAT_TDS;
      break;

    default:
      mmgsdi_rat_type = MMGSDI_RAT_NONE;
      break;
  }
  return mmgsdi_rat_type;
}/* mmgsdi_eons_map_cm_rat_to_mmgsdi_rat */


/*===========================================================================
FUNCTION MMGSDI_EONS_COPY_SS_EVENT_DATA

DESCRIPTION
  Function to copy MM, RPLMN, LAC, TAC, RAC, and CSG data from SS event data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_copy_ss_event_data(
  mmgsdi_eons_nitz_data_type                 * nitz_info_ptr,
  sys_sys_mode_e_type                        * sys_mode_ptr,
  const cm_mm_msim_ss_stack_info_s_type      * stack_info_ptr,
  sys_modem_as_id_e_type                       asubs_id
)
{
  const sys_csg_info_s_type * csg_info_ptr  = NULL;
  sys_sys_id_s_type         * sys_id_ptr    = NULL;
  mmgsdi_rat_enum_type        mmgsdi_rat    = MMGSDI_RAT_MAX;


  if ((nitz_info_ptr == NULL) || (stack_info_ptr == NULL) || (sys_mode_ptr == NULL))
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    sys_id_ptr,
    sizeof(sys_sys_id_s_type));

  if(sys_id_ptr == NULL)
  {
    return;
  }

  /*
  1)  For CSG information, always use the sys_mode present inside the sys_id.
      Do not use the sys_mode derived from RAT in mm_info for this.
  2)  When mm_info changed (CM_SS_EVT_MOBILITY_MGMT_MASK is Set) and also the
      PLMN info is valid (plmn_avail flag is TRUE) then use the NITZ info
      (RAT, PLMN-ID, LAC) from mm_info otherwise use the info from sys_id.
  */

  mmgsdi_eons_copy_ss_event_mm_info(&(stack_info_ptr->mode_info.gw_info.mm_information),
                                    nitz_info_ptr);

  if((stack_info_ptr->changed_fields & CM_SS_EVT_MOBILITY_MGMT_MASK) &&
     stack_info_ptr->mode_info.gw_info.mm_information.plmn_avail &&
     MMGSDI_EONS_PLMN_ID_VALIDITY_CHECK(stack_info_ptr->mode_info.gw_info.mm_information.plmn.identity))
  {
    mmgsdi_memscpy(&sys_id_ptr->id.plmn,
                   sizeof(sys_plmn_id_s_type),
                   &stack_info_ptr->mode_info.gw_info.mm_information.plmn,
                   sizeof(sys_plmn_id_s_type));
    sys_id_ptr->id.plmn_lac.lac = stack_info_ptr->mode_info.gw_info.mm_information.lac_id;
    mmgsdi_rat = mmgsdi_eons_map_cm_rat_to_mmgsdi_rat(stack_info_ptr->mode_info.gw_info.mm_information.rat);
    *sys_mode_ptr = mmgsdi_eons_map_rat_type_to_sys_mode(mmgsdi_rat);
  }
  else
  {
    mmgsdi_memscpy(sys_id_ptr,
                   sizeof(sys_sys_id_s_type),
                   &stack_info_ptr->sys_id,
                   sizeof(sys_sys_id_s_type));
    *sys_mode_ptr = stack_info_ptr->sys_mode;
  }

  mmgsdi_eons_copy_ss_event_rplmn_lac_tac_data(sys_id_ptr,
                                               nitz_info_ptr,
                                               (*sys_mode_ptr),
                                               (*sys_mode_ptr == SYS_SYS_MODE_LTE) ?
                                                 stack_info_ptr->rat_mode_info.lte_mode_info.tac:
                                                 sys_id_ptr->id.plmn_lac.lac);
  MMGSDIUTIL_TMC_MEM_FREE(sys_id_ptr);

  switch (stack_info_ptr->sys_mode)
  {
    case SYS_SYS_MODE_LTE:
      csg_info_ptr = &(stack_info_ptr->rat_mode_info.lte_mode_info.csg_info);
      break;
    case SYS_SYS_MODE_WCDMA:
      csg_info_ptr = &(stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info);
      break;
    case SYS_SYS_MODE_TDS:
      csg_info_ptr = &(stack_info_ptr->rat_mode_info.tds_mode_info.csg_info);
      break;
    case SYS_SYS_MODE_GSM:
      csg_info_ptr = &(stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info);
      break;
    default:
      break;
  }

  mmgsdi_eons_copy_ss_event_csg_data(csg_info_ptr,
                                     nitz_info_ptr,
                                     stack_info_ptr->changed_fields,
                                     asubs_id);
} /* mmgsdi_eons_copy_ss_event_data */


/*===========================================================================
  FUNCTION: MMGSDI_EONS_CM_SS_EVENT_CB

  DESCRIPTION:
    Handler for CM_SS_EVENT_SRV_CHANGED. It decides whether it is required to
    copy the CM SS event data based on SYS MODE, SRV STATUS and CHANGED FIELDS
    and queues MMGSDI_EONS_SS_EVENT_REQ command.

  PARAMETERS:
    ss_event         -> cm ss event
    mm_ss_info_ptr   -> more information regarding the ss event
===========================================================================*/
static void mmgsdi_eons_cm_ss_event_cb(
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type * mm_ss_info_ptr
)
{
  boolean                    ss_info_changed                      = FALSE;
  mmgsdi_eons_ss_data_type   ss_info;
  uint8                      i                                    = 0;
  mmgsdi_rat_enum_type       mmgsdi_rat                           = MMGSDI_RAT_MAX;
  boolean                    primary_sub_first_stack_gw_changed   = FALSE;
  boolean                    primary_sub_first_stack_1x_changed   = FALSE;
  boolean                    primary_sub_second_stack_gw_changed  = FALSE;
  boolean                    secondary_sub_first_stack_gw_changed = FALSE;
  boolean                    secondary_sub_first_stack_1x_changed = FALSE;
  boolean                    tertiary_sub_first_stack_gw_changed  = FALSE;

  if(ss_event != CM_SS_EVENT_SRV_CHANGED || mm_ss_info_ptr == NULL)
  {
    return;
  }

  memset(&ss_info, 0x00, sizeof(mmgsdi_eons_ss_data_type));

  for (i = 0; i < CM_NO_STACKS; i++)
  {
    if (mm_ss_info_ptr->stack_info[i].is_operational)
    {
      if (mm_ss_info_ptr->stack_info[i].sys_mode == SYS_SYS_MODE_LTE)
      {
        UIM_MSG_HIGH_7("CM SS Event, stack:0x%x is operational, ASUBS ID:0x%x, PLMN: 0x%x, Service Status: 0x%x Sys Mode: 0x%x Changed fields: 0x%x, TAC: 0x%x",
                       i,
                       mm_ss_info_ptr->asubs_id,
                       mm_ss_info_ptr->stack_info[i].sys_id.id.plmn.identity[0] * 0x10000 +
                       mm_ss_info_ptr->stack_info[i].sys_id.id.plmn.identity[1] * 0x100 +
                       mm_ss_info_ptr->stack_info[i].sys_id.id.plmn.identity[2],
                       mm_ss_info_ptr->stack_info[i].srv_status,
                       mm_ss_info_ptr->stack_info[i].sys_mode,
                       mm_ss_info_ptr->stack_info[i].changed_fields,
                       mm_ss_info_ptr->stack_info[i].rat_mode_info.lte_mode_info.tac);
      }
      else
      {
        UIM_MSG_HIGH_8("CM SS Event, stack:0x%x is operational, ASUBS ID:0x%x, Service Status: 0x%x Sys Mode: 0x%x Changed fields: 0x%x, LAC: 0x%x Sys id type: 0x%x",
                       i,
                       mm_ss_info_ptr->asubs_id,
                       mm_ss_info_ptr->stack_info[i].sys_id.id.plmn.identity[0] * 0x10000 +
                       mm_ss_info_ptr->stack_info[i].sys_id.id.plmn.identity[1] * 0x100 +
                       mm_ss_info_ptr->stack_info[i].sys_id.id.plmn.identity[2],
                       mm_ss_info_ptr->stack_info[i].srv_status,
                       mm_ss_info_ptr->stack_info[i].sys_mode,
                       mm_ss_info_ptr->stack_info[i].changed_fields,
                       mm_ss_info_ptr->stack_info[i].sys_id.id.plmn_lac.lac,
                       mm_ss_info_ptr->stack_info[i].sys_id.id_type);
      }
    }
  }

  /*In case of LTE/GSM/WCDMA network, copy plmn id, LAC (TAC in case of LTE),
    sys mode and mobility mgmt info only when mobility mgmt info or service
    status is changed and service status is limited or full.
    In case of CDMA/HDR network, copy sys mode and roam status only when
    service status or roam status is changed and service status is limited or full */
  /*For the SVLTE and SGLTE single baseband configurations there is only a single
    subscription from 1 slot but 2 call stacks can be active hence the second
    GW stack related SS informtion has to be processed */
  if(mm_ss_info_ptr->asubs_id == SYS_MODEM_AS_ID_1)
  {
    if (mm_ss_info_ptr->stack_info[0].is_operational)
    {
      if(MMGSDI_SS_INFO_CHANGED(mm_ss_info_ptr->stack_info[0].changed_fields) &&
         MMGSDI_SRV_STATUS_CHANGED(mm_ss_info_ptr->stack_info[0].srv_status))
      {
        if(MMGSDI_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode) &&
           MMGSDI_3GPP_SS_INFO_CHANGED(mm_ss_info_ptr->stack_info[0].changed_fields))
        {
          primary_sub_first_stack_gw_changed = TRUE;
          ss_info.session_type                = MMGSDI_GW_PROV_PRI_SESSION;
          ss_info.first_stack_ss.is_valid     = TRUE;
          ss_info.first_stack_ss.srv_status   = mm_ss_info_ptr->stack_info[0].srv_status;
          ss_info.first_stack_ss.roam_status  = mm_ss_info_ptr->stack_info[0].roam_status;
          ss_info_changed                     = TRUE;

          mmgsdi_eons_copy_ss_event_data(
             &(ss_info.first_stack_ss.nitz_info),
             &(ss_info.first_stack_ss.sys_mode),
             &(mm_ss_info_ptr->stack_info[0]),
             mm_ss_info_ptr->asubs_id);
        }
        else if(MMGSDI_3GPP2_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode) &&
                MMGSDI_3GPP2_SS_INFO_CHANGED(mm_ss_info_ptr->stack_info[0].changed_fields) &&
                (mm_ss_info_ptr->stack_info[0].srv_status == SYS_SRV_STATUS_SRV))
        {
          primary_sub_first_stack_1x_changed  = TRUE;
          ss_info.session_type                = MMGSDI_1X_PROV_PRI_SESSION;
          ss_info.first_stack_ss.is_valid     = TRUE;
          ss_info.first_stack_ss.srv_status   = mm_ss_info_ptr->stack_info[0].srv_status;
          ss_info.first_stack_ss.sys_mode     = mm_ss_info_ptr->stack_info[0].sys_mode;
          ss_info.first_stack_ss.roam_status  = mm_ss_info_ptr->stack_info[0].roam_status;
          ss_info_changed                     = TRUE;
        }
      }
    }

    /* Presently only one configuration is supported where a 2nd stack related
       events is processed for the primary subscription. It is the single
       baseband SGLTE. Since only G can be present on the second stack we check
       for sys mode to be G and only process in that case */
    if(mm_ss_info_ptr->stack_info[1].is_operational  &&
       MMGSDI_3GPP_SS_INFO_CHANGED(mm_ss_info_ptr->stack_info[1].changed_fields) &&
       MMGSDI_SRV_STATUS_CHANGED(mm_ss_info_ptr->stack_info[1].srv_status))
    {
      sys_sys_id_s_type       * sys_id_ptr    = NULL;

      primary_sub_second_stack_gw_changed = TRUE;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      sys_id_ptr,
      sizeof(sys_sys_id_s_type));

      if(sys_id_ptr == NULL)
      {
        return;
      }

      if((mm_ss_info_ptr->stack_info[1].changed_fields & CM_SS_EVT_MOBILITY_MGMT_MASK) &&
         mm_ss_info_ptr->stack_info[1].mode_info.gw_info.mm_information.plmn_avail &&
         MMGSDI_EONS_PLMN_ID_VALIDITY_CHECK(mm_ss_info_ptr->stack_info[1].mode_info.gw_info.mm_information.plmn.identity))
      {
        mmgsdi_memscpy(&sys_id_ptr->id.plmn,
                       sizeof(sys_plmn_id_s_type),
                       &mm_ss_info_ptr->stack_info[1].mode_info.gw_info.mm_information.plmn,
                       sizeof(sys_plmn_id_s_type));
        sys_id_ptr->id.plmn_lac.lac = mm_ss_info_ptr->stack_info[1].mode_info.gw_info.mm_information.lac_id;
        mmgsdi_rat = mmgsdi_eons_map_cm_rat_to_mmgsdi_rat(
                       mm_ss_info_ptr->stack_info[1].mode_info.gw_info.mm_information.rat);
        ss_info.sec_stack_ss.sys_mode = mmgsdi_eons_map_rat_type_to_sys_mode(mmgsdi_rat);
      }
      else
      {
        mmgsdi_memscpy(sys_id_ptr,
                       sizeof(sys_sys_id_s_type),
                       &mm_ss_info_ptr->stack_info[1].sys_id,
                       sizeof(sys_sys_id_s_type));
        ss_info.sec_stack_ss.sys_mode = mm_ss_info_ptr->stack_info[1].sys_mode;
      }
      ss_info.session_type              = MMGSDI_GW_PROV_PRI_SESSION;
      ss_info.sec_stack_ss.is_valid     = TRUE;
      ss_info.sec_stack_ss.srv_status   = mm_ss_info_ptr->stack_info[1].srv_status;
      ss_info.sec_stack_ss.roam_status  = mm_ss_info_ptr->stack_info[1].roam_status;
      ss_info_changed                   = TRUE;

      mmgsdi_eons_copy_ss_event_mm_info(
        &(mm_ss_info_ptr->stack_info[1].mode_info.gw_info.mm_information),
        &(ss_info.sec_stack_ss.nitz_info));

      mmgsdi_eons_copy_ss_event_rplmn_lac_tac_data(
        sys_id_ptr, &(ss_info.sec_stack_ss.nitz_info),
        ss_info.sec_stack_ss.sys_mode, sys_id_ptr->id.plmn_lac.lac);

      MMGSDIUTIL_TMC_MEM_FREE(sys_id_ptr);

      /* Setting CSG ID as invalid for secondary stack because CSG is currently
         not supported for 2 concurrent call stacks */
      ss_info.sec_stack_ss.nitz_info.csg_info.csg_id = MMGSDI_EONS_INVALID_CSG_ID;
    }
  }

  /*SS event is received for AS_ID_2.
    In case of GSM/WCDMA network, copy plmn id, LAC, sys mode and mobility
    mgmt info only when mobility mgmt info or service status is changed
    and service status is limited or FULL.*/
  if (mm_ss_info_ptr->asubs_id == SYS_MODEM_AS_ID_2 )
  {
    memset(&ss_info, 0x00, sizeof(mmgsdi_eons_ss_data_type));

    if (mm_ss_info_ptr->stack_info[0].is_operational)
    {
      if(MMGSDI_3GPP_SS_INFO_CHANGED(mm_ss_info_ptr->stack_info[0].changed_fields) &&
         MMGSDI_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode) &&
         MMGSDI_SRV_STATUS_CHANGED(mm_ss_info_ptr->stack_info[0].srv_status))
      {
        secondary_sub_first_stack_gw_changed = TRUE;
        ss_info.session_type                 = MMGSDI_GW_PROV_SEC_SESSION;
        ss_info.first_stack_ss.is_valid      = TRUE;
        ss_info.first_stack_ss.srv_status    = mm_ss_info_ptr->stack_info[0].srv_status;
        ss_info.first_stack_ss.roam_status   = mm_ss_info_ptr->stack_info[0].roam_status;
        ss_info_changed                      = TRUE;

        mmgsdi_eons_copy_ss_event_data(
           &(ss_info.first_stack_ss.nitz_info),
           &(ss_info.first_stack_ss.sys_mode),
           &(mm_ss_info_ptr->stack_info[0]),
           mm_ss_info_ptr->asubs_id);
      }
      else if(MMGSDI_3GPP2_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode) &&
              MMGSDI_3GPP2_SS_INFO_CHANGED(mm_ss_info_ptr->stack_info[0].changed_fields) &&
              (mm_ss_info_ptr->stack_info[0].srv_status == SYS_SRV_STATUS_SRV))
      {
        secondary_sub_first_stack_1x_changed = TRUE;
        ss_info.session_type                 = MMGSDI_1X_PROV_SEC_SESSION;
        ss_info.first_stack_ss.is_valid      = TRUE;
        ss_info.first_stack_ss.srv_status    = mm_ss_info_ptr->stack_info[0].srv_status;
        ss_info.first_stack_ss.sys_mode      = mm_ss_info_ptr->stack_info[0].sys_mode;
        ss_info.first_stack_ss.roam_status   = mm_ss_info_ptr->stack_info[0].roam_status;
        ss_info_changed                      = TRUE;
      }
    }
  }

  if (mm_ss_info_ptr->asubs_id == SYS_MODEM_AS_ID_3 )
  {
    memset(&ss_info, 0x00, sizeof(mmgsdi_eons_ss_data_type));

    if (mm_ss_info_ptr->stack_info[0].is_operational)
    {
      if(MMGSDI_3GPP_SS_INFO_CHANGED(mm_ss_info_ptr->stack_info[0].changed_fields) &&
         MMGSDI_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode) &&
         MMGSDI_SRV_STATUS_CHANGED(mm_ss_info_ptr->stack_info[0].srv_status))
      {
        tertiary_sub_first_stack_gw_changed = TRUE;
        ss_info.session_type                = MMGSDI_GW_PROV_TER_SESSION;
        ss_info.first_stack_ss.is_valid     = TRUE;
        ss_info.first_stack_ss.srv_status   = mm_ss_info_ptr->stack_info[0].srv_status;
        ss_info.first_stack_ss.roam_status  = mm_ss_info_ptr->stack_info[0].roam_status;
        ss_info_changed                     = TRUE;

        mmgsdi_eons_copy_ss_event_data(
           &(ss_info.first_stack_ss.nitz_info),
           &(ss_info.first_stack_ss.sys_mode),
           &(mm_ss_info_ptr->stack_info[0]),
           mm_ss_info_ptr->asubs_id);
      }
    }
  }

  /*put in command queue*/
  if(ss_info_changed == TRUE)
  {
    UIM_MSG_HIGH_6("Changed: Primary sub First stack GW SS info 0x%x 1X SS info 0x%x Second stack GW SS info 0x%x Secondary Sub GW SS info 0x%x 1X SS info 0x%x Tertiary Sub GW SS info 0x%x",
                   primary_sub_first_stack_gw_changed,
                   primary_sub_first_stack_1x_changed,
                   primary_sub_second_stack_gw_changed,
                   secondary_sub_first_stack_gw_changed,
                   secondary_sub_first_stack_1x_changed,
                   tertiary_sub_first_stack_gw_changed);
    (void)mmgsdi_eons_create_cmd_and_queue(ss_info);
  }

  mmgsdi_eons_free_nitz_data_ptrs(&(ss_info.first_stack_ss.nitz_info));
  mmgsdi_eons_free_nitz_data_ptrs(&(ss_info.sec_stack_ss.nitz_info));
} /* mmgsdi_eons_cm_ss_event_cb */


/*===========================================================================
  FUNCTION MMGSDI_EONS_COPY_PER_STACK_SS_INFO

DESCRIPTION
  Utility function to copy per stack SS info

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_copy_per_stack_ss_info(
  const mmgsdi_eons_per_stack_ss_data_type  * in_ss_info_ptr,
  mmgsdi_eons_per_stack_ss_data_type        * out_ss_info_ptr,
  boolean                                     allocate_mem
)
{
  const mmgsdi_eons_nitz_data_type * in_nitz_info_ptr  = NULL;
  mmgsdi_eons_nitz_data_type       * out_nitz_info_ptr = NULL;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_SUCCESS;

  if (in_ss_info_ptr == NULL || out_ss_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  out_ss_info_ptr->is_valid    = in_ss_info_ptr->is_valid;
  out_ss_info_ptr->sys_mode    = in_ss_info_ptr->sys_mode;
  out_ss_info_ptr->srv_status  = in_ss_info_ptr->srv_status;
  out_ss_info_ptr->roam_status = in_ss_info_ptr->roam_status;

  /* If the sys mode is one of the 3GPP2 RATs there is no NITZ info to copy. */
  if (MMGSDI_3GPP2_SYS_MODE(out_ss_info_ptr->sys_mode))
  {
    return MMGSDI_SUCCESS;
  }

  /* If the sys mode is not 3GPP, it's an unknown sys mode so we return */
  if (MMGSDI_3GPP_SYS_MODE(out_ss_info_ptr->sys_mode) == FALSE)
  {
    return MMGSDI_SUCCESS;
  }

  in_nitz_info_ptr  = &(in_ss_info_ptr->nitz_info);
  out_nitz_info_ptr = &(out_ss_info_ptr->nitz_info);

  out_nitz_info_ptr->lac                  = in_nitz_info_ptr->lac;
  out_nitz_info_ptr->lac_available        = in_nitz_info_ptr->lac_available;
  out_nitz_info_ptr->rplmn_available      = in_nitz_info_ptr->rplmn_available;
  out_nitz_info_ptr->csg_info.csg_id      = in_nitz_info_ptr->csg_info.csg_id;
  out_nitz_info_ptr->long_name_available  = in_nitz_info_ptr->long_name_available;
  out_nitz_info_ptr->short_name_available = in_nitz_info_ptr->short_name_available;

  if(in_nitz_info_ptr->rplmn_available)
  {
    mmgsdi_memscpy(out_nitz_info_ptr->rplmn_id.plmn_id_val,
                   sizeof(out_nitz_info_ptr->rplmn_id.plmn_id_val),
                   in_nitz_info_ptr->rplmn_id.plmn_id_val,
                   sizeof(in_nitz_info_ptr->rplmn_id.plmn_id_val));

    out_nitz_info_ptr->rplmn_id.rat =
      mmgsdi_eons_map_sys_mode_to_rat_type(out_ss_info_ptr->sys_mode);

    out_nitz_info_ptr->rplmn_id.csg_id = out_nitz_info_ptr->csg_info.csg_id;
  }

  if (in_nitz_info_ptr->csg_info.csg_id != MMGSDI_EONS_INVALID_CSG_ID)
  {
    if(in_nitz_info_ptr->csg_info.hnb_name.length > 0 &&
       in_nitz_info_ptr->csg_info.hnb_name.length <= SYS_HNB_NAME_LEN)
    {
      out_nitz_info_ptr->csg_info.hnb_name.length =
          in_nitz_info_ptr->csg_info.hnb_name.length;

      mmgsdi_memscpy(out_nitz_info_ptr->csg_info.hnb_name.name,
                     out_nitz_info_ptr->csg_info.hnb_name.length,
                     in_nitz_info_ptr->csg_info.hnb_name.name,
                     in_nitz_info_ptr->csg_info.hnb_name.length);
    }
  }

  if (in_nitz_info_ptr->long_name_available)
  {
    mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(
                      &(in_nitz_info_ptr->long_name),
                      &(out_nitz_info_ptr->long_name),
                      allocate_mem);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  if (in_nitz_info_ptr->short_name_available)
  {
    mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(
                      &(in_nitz_info_ptr->short_name),
                      &(out_nitz_info_ptr->short_name),
                      allocate_mem);
  }

  return mmgsdi_status;
} /* mmgsdi_eons_copy_per_stack_ss_info */


/*===========================================================================
FUNCTION MMGSDI_CM_SS_EVENT_CB

DESCRIPTION
  Handler for CM_SS_EVENT events.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
void mmgsdi_cm_ss_event_cb (
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type * mm_ss_info_ptr
)
{
  if(mm_ss_info_ptr == NULL)
  {
    return;
  }

  /* Callback to process the CM SS event by EONS */
  mmgsdi_eons_cm_ss_event_cb(ss_event, mm_ss_info_ptr);

#if defined(FEATURE_SIMLOCK) && defined(FEATURE_SIMLOCK_RSU)
  /* Callback to post a command to SIMLOCK for NITZ information */
  simlock_time_cm_ss_event_cb(ss_event, mm_ss_info_ptr);
#endif /* FEATURE_SIMLOCK && FEATURE_SIMLOCK_RSU */

  /* Callback to process the CM SS event by SimLock */
  mmgsdi_simlock_cm_ss_event_cb(ss_event, mm_ss_info_ptr);
} /* mmgsdi_cm_ss_event_cb */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_EONS_SS_EVENT

DESCRIPTION
  This function processes any relevant MM, PLMN, LAC, TAC, CSG information
  obtained from an SS event

DEPENDENCIES
  None

LIMITATIONS:
  None

RETURN VALUE
  mmgsdi_return_enum_type
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_eons_ss_event(
  mmgsdi_eons_ss_event_req_type * req_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  int32                                index           = 0;
  mmgsdi_client_req_extra_info_type   *extra_param_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /*----------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -----------------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_EONS_SS_EVENT_REQ,
                    (void*)req_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }/* end of if (mmgsdi_status != MMGSDI_SUCCESS) */

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    NULL,
                    extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index,
                    MMGSDI_EONS_SS_EVENT_REQ,
                    mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_process_eons_ss_event */


/*===========================================================================
  FUNCTION MMGSDI_EONS_PRINT_PER_STACK_SS_INFO

DESCRIPTION
  Utility function to copy per stack SS info

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_print_per_stack_ss_info(
  const mmgsdi_eons_per_stack_ss_data_type  * in_ss_info_ptr,
  boolean                                     new_data
)
{
  if (in_ss_info_ptr == NULL)
  {
    return;
  }

  UIM_MSG_MED_8("is data new:0x%x sys mode:0x%x, srv status:0x%x, roam status:0x%x is LAC available:0x%x, LAC:0x%x, is rplmn available :0x%x rplmn_id.plmn_id_val: 0x%x",
                new_data,
                in_ss_info_ptr->sys_mode,
                in_ss_info_ptr->srv_status,
                in_ss_info_ptr->roam_status,
                in_ss_info_ptr->nitz_info.lac_available,
                in_ss_info_ptr->nitz_info.lac,
                in_ss_info_ptr->nitz_info.rplmn_available,
                in_ss_info_ptr->nitz_info.rplmn_id.plmn_id_val[0] * 0x10000 +
                in_ss_info_ptr->nitz_info.rplmn_id.plmn_id_val[1] * 0x100 +
                in_ss_info_ptr->nitz_info.rplmn_id.plmn_id_val[2]);

  UIM_MSG_MED_6("is long name available:0x%x, length:0x%x, encoding:0x%x is short name available:0x%x, length:0x%x, encoding:0x%x",
               in_ss_info_ptr->nitz_info.long_name_available,
               in_ss_info_ptr->nitz_info.long_name.plmn_name.eons_data.data_len,
               in_ss_info_ptr->nitz_info.long_name.plmn_name.eons_encoding,
               in_ss_info_ptr->nitz_info.short_name_available,
               in_ss_info_ptr->nitz_info.short_name.plmn_name.eons_data.data_len,
               in_ss_info_ptr->nitz_info.short_name.plmn_name.eons_encoding);
} /* mmgsdi_eons_print_per_stack_ss_info */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_EONS_SS_EVENT_RESPONSE

DESCRIPTION
  This function, process the response for an SS event. It will determine
  if the MMGSDI EONS event needs to be broadcasted.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_eons_ss_event_response(
  const mmgsdi_uim_report_rsp_type  * rsp_ptr
)
{
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_id_type               session_id       = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_int_app_info_type           * int_app_info_ptr = NULL;
  mmgsdi_eons_ss_data_type           * ss_info_ptr      = NULL;
  int32                                index            = 0;
  mmgsdi_request_header_type           req_header;
  uim_cmd_type                       * uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr  = NULL;
  mmgsdi_eons_info_type              * session_eons_ptr    = NULL;
  mmgsdi_list_plmn_name_type           plmn_name_list;
  mmgsdi_plmn_id_list_type             plmn_id_list;

  MMGSDIUTIL_RETURN_IF_NULL(rsp_ptr);

  UIM_MSG_HIGH_1("MMGSDI PROCESS SS EVENT RESPONSE, status: 0x%x",
                rsp_ptr->mmgsdi_error_status);

  if(rsp_ptr->mmgsdi_error_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  index = rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(index,
                   &req_header,
                   &uim_cmd_ptr,
                   &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  ss_info_ptr = &(extra_param_ptr->ss_event_data.ss_info);

  switch(ss_info_ptr->session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      session_id = mmgsdi_generic_data_ptr->pri_gw_session_id;
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      session_id = mmgsdi_generic_data_ptr->sec_gw_session_id;
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      session_id = mmgsdi_generic_data_ptr->ter_gw_session_id;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      session_id= mmgsdi_generic_data_ptr->pri_1x_session_id;
      break;

    case MMGSDI_1X_PROV_SEC_SESSION:
      session_id= mmgsdi_generic_data_ptr->sec_1x_session_id;
      break;

   default:
     return mmgsdi_status;

  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                  session_id,
                  NULL,
                  NULL,
                  &int_app_info_ptr,
                  NULL,
                  NULL,
                  NULL);

  UIM_MSG_HIGH_4("App Info ptr 0x%x Session type 0x%x first stack valid 0x%x second stack valid 0x%x",
                 int_app_info_ptr, ss_info_ptr->session_type, 
                 ss_info_ptr->first_stack_ss.is_valid,
                 ss_info_ptr->sec_stack_ss.is_valid);

  /* When there is no card and/or session is not activated, copy nitz data into
     the global eons data */
  if(int_app_info_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_eons_info_type *generic_data_eons_ptr;

    generic_data_eons_ptr =
      &(mmgsdi_generic_data_ptr->mmgsdi_eons_global_info);

    session_eons_ptr = &int_app_info_ptr->eons_info;

    mmgsdi_eons_free_nitz_data_ptrs(
       &(generic_data_eons_ptr->ss_info.first_stack_ss.nitz_info));
    mmgsdi_eons_free_nitz_data_ptrs(
       &(generic_data_eons_ptr->ss_info.sec_stack_ss.nitz_info));
    mmgsdi_eons_free_rplmn_info_ptrs(
       &(generic_data_eons_ptr->rplmn_info.first_stack_rplmn));
    mmgsdi_eons_free_rplmn_info_ptrs(
       &(generic_data_eons_ptr->rplmn_info.sec_stack_rplmn));

    memset(generic_data_eons_ptr, 0x00, sizeof(mmgsdi_eons_info_type));
  }
  else
  {
    session_eons_ptr =
      &(mmgsdi_generic_data_ptr->mmgsdi_eons_global_info);
  }

  if (ss_info_ptr->first_stack_ss.is_valid)
  {
    mmgsdi_eons_print_per_stack_ss_info(
       &(session_eons_ptr->ss_info.first_stack_ss), FALSE);

    mmgsdi_eons_print_per_stack_ss_info(&(ss_info_ptr->first_stack_ss), TRUE);

    (void)mmgsdi_eons_copy_per_stack_ss_info(
            &(ss_info_ptr->first_stack_ss), &(session_eons_ptr->ss_info.first_stack_ss), TRUE);
  }

  if (ss_info_ptr->sec_stack_ss.is_valid)
  {
    mmgsdi_eons_print_per_stack_ss_info(
       &(session_eons_ptr->ss_info.sec_stack_ss), FALSE);

    mmgsdi_eons_print_per_stack_ss_info(&(ss_info_ptr->sec_stack_ss), TRUE);

    (void)mmgsdi_eons_copy_per_stack_ss_info(
            &(ss_info_ptr->sec_stack_ss), &(session_eons_ptr->ss_info.sec_stack_ss), TRUE);
  }

  /* Send an event to clients if any one active call stack has full service */
  if((ss_info_ptr->first_stack_ss.is_valid &&
      ss_info_ptr->first_stack_ss.srv_status == SYS_SRV_STATUS_SRV) ||
     (ss_info_ptr->sec_stack_ss.is_valid &&
      ss_info_ptr->sec_stack_ss.srv_status == SYS_SRV_STATUS_SRV))
  {
    mmgsdi_eons_send_event(session_id, MMGSDI_EONS_TRIGGER_SS_EVENT, FALSE);
  }
  else
  {
    memset(&plmn_id_list, 0x0, sizeof(mmgsdi_plmn_id_list_type));
    memset(&plmn_name_list, 0x0, sizeof(mmgsdi_list_plmn_name_type));

    /* run the algorithm to retrieve RPLMN name and store it in globals */
    (void)mmgsdi_eons_run_algo(
            session_id,
            MMGSDI_EONS_TRIGGER_SS_EVENT,
            plmn_id_list,
            MMGSDI_EONS_IGNORE_DISPLAY_COND_NONE,
            NULL,
            NULL,
            &plmn_name_list,
            NULL);

    /* Free memory allocated to plmn info ptr while running EONS algo */
    if(plmn_name_list.plmn_info_ptr != NULL)
    {
      uint8 loop_count = 0;
      for (loop_count = 0; loop_count < plmn_name_list.num_of_plmn_ids; loop_count++)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          plmn_name_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          plmn_name_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_ptr);
      }
      MMGSDIUTIL_TMC_MEM_FREE(plmn_name_list.plmn_info_ptr);
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_process_eons_ss_event_response */

