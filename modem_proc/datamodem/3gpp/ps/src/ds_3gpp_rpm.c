/*!
  @file
  ds_3gpp_rpm.c

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_rpm.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/31/14   ss      Fixed the issue of hourly timer timestamp overwritten
		   when backoff timer is started.
03/14/12   ss      Fixed incorrect backoff timer calculation during PDP
                   IGNORE scenario.
02/28/12   kv      AEEStd usage for string fucntions.
01/31/12   ss      Fixed issue of incrementing CPDP counters and starting LR3 
                   timer when RPM parameters are not present on UICC. 
01/06/12   dvk     Fixed Klocwork errors
11/09/11   ss      Fixed issue of LR3 timer starting for 0 seconds.
10/27/11   nd      Fixed Klocwork errors.
10/24/11   ss      Reset CPDP count after mmgsdi refresh.
10/24/11   ss      Fixed RPM mmgsdi refresh registration issue.
10/24/11   ss      Fixed issue of reading/writing RPM EF files under USIM.
09/27/11   nd      Fixed compiler warnings.
08/01/11   ss      Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_RPM
#include "mcfg_fs.h"
#include "data_msg.h"
#include "ds3gmmgsdiif.h"
#include "ds3gmgr.h"
#include "ds_3gpp_rpm.h"
#include "dsutil.h"
#include "ds_3gppi_utils.h"
#include "ds3gcfgmgr.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "modem_mem.h"
#include "mmgsdilib_common.h"
#include "ds3gmshif.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_dsd_apm_ext_i.h"
#include "ds3gcmif.h"
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

/*===========================================================================

                   INTERNAL DEFINITIONS

===========================================================================*/

/*----------------------------------------------------------------------------
  EFS file path for saving the RPM parameters to be retained across power 
  cycles.
----------------------------------------------------------------------------*/

#define DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME        "rpm"

#define DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME_LEN    3

#define DS_3GPP_RPM_THROTTLE_PARAM_FILE_NAME       "rpm_throttle_params"

#define DS_3GPP_RPM_THROTTLE_PARAM_FILE_NAME_LEN   19

/*---------------------------------------------------------------------------
  EFS directory name to hold "rpm" :
  TWO additional spaces are req
  one for '/' and the other for the ASCIZ character
---------------------------------------------------------------------------*/
static char ds_3gpp_rpm_dir_name[\
               DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME_LEN + \
               1 + 1] = {NULL};

/*---------------------------------------------------------------------------
  EFS file name for the throttle file "rpm_throttle_params". Allocate an 
  additional character for the ASCIZ terminator
---------------------------------------------------------------------------*/
static char
    ds_3gpp_rpm_file_path[DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME_LEN + \
                          1 + \
                          DS_3GPP_RPM_THROTTLE_PARAM_FILE_NAME_LEN + 
                          1] = {NULL};

/*===========================================================================

                           INTERNAL VARIABLE DEFINITION

===========================================================================*/
static ds_3gpp_rpm_params_type ds_3gpp_rpm_params[SYS_MODEM_AS_ID_MAX] = {{0}};

/* RPM throttle table */
static ds_3gpp_rpm_throttle_tbl_type   
                          *ds_3gpp_rpm_throttle_tbl[SYS_MODEM_AS_ID_MAX][DS_3GPP_MAX_PLMN_SUPPORTED] = {{NULL}};

/* EFS throttle table */
static ds_3gpp_rpm_efs_throttle_tbl_type         
                      *ds_3gpp_rpm_efs_throttle_tbl[SYS_MODEM_AS_ID_MAX][DS_3GPP_MAX_PLMN_SUPPORTED] = {{NULL}};

/* SIM information stored in MCC-MNC format */
static ds_3gpp_rpm_sim_info_s_type  
             ds_3gpp_rpm_supported_sim[SYS_MODEM_AS_ID_MAX][DS_3GPP_RPM_MAX_SIM_SUPPORTED] = {{{{NULL}}}};

static ds_3gpp_rpm_util_params_s_type
	         *ds_3gpp_rpm_util_params[SYS_MODEM_AS_ID_MAX] = {NULL};

/*===========================================================================

                           INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
static boolean ds_3gpp_rpm_is_util_param_valid
(
  sys_modem_as_id_e_type  subs_id
);

static void ds_3gpp_rpm_get_backoff_timer_val
(
  ds_3gpp_rpm_throttle_param_type    *throttle_param_ptr,
  sys_modem_as_id_e_type             subs_id
);

ds_3gpp_rpm_efs_throttle_tbl_type *ds_3gpp_rpm_get_efs_throttle_tbl_ptr
(
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type  subs_id
);

ds_3gpp_rpm_throttle_tbl_type *ds_3gpp_rpm_get_throttle_tbl_ptr
(
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type  subs_id
);

ds_3gpp_rpm_throttle_param_type  *ds_3gpp_rpm_get_throttle_param_ptr
(
  byte     *apn,
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type  subs_id
);

void ds_3gpp_rpm_powerup_lr3_tmr_hdlr
(
  sys_modem_as_id_e_type subs_id
);

boolean ds_3gpp_rpm_mmgsdi_read
(
  mmgsdi_session_id_type         session_id,
  uint16                         rpm_param_path
);

boolean ds_3gpp_rpm_mmgsdi_write
(
  mmgsdi_offset_type             offset,
  mmgsdi_write_data_type         *write_data, 
  sys_modem_as_id_e_type         subs_id
);

void ds_3gpp_rpm_power_up_throttle_hdlr
(
  sys_modem_as_id_e_type  subs_id
);

void ds_3gpp_rpm_print_plmn_id
(
  void *current_plmn
);

void ds_3gpp_rpm_start_lr3_timer
(
  uint32   timestamp,
  uint32   timer_val,
  sys_modem_as_id_e_type subs_id
);

static void ds_3gpp_rpm_update_local_cache
(
  mmgsdi_read_cnf_type    *read_cnf_ptr,
  void                    *local_cache_ptr,
  sys_modem_as_id_e_type  subs_id
);

boolean ds_3gpp_rpm_write_to_efs
(
  ds_3gpp_rpm_data_buffer_type    *write_data,
  uint32                          write_offset,
  boolean                         reset_all_plmns
);

static boolean ds_3gpp_rpm_create_efs_throttle_param_file(void);

/*===========================================================================

                           FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_RPM_IS_UTIL_PARAM_VALID

DESCRIPTION
  This function verifies whether UTIL params are allocated.

PARAMETERS
  subs_id.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_rpm_is_util_param_valid
(
  sys_modem_as_id_e_type subs_id 
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
	  return FALSE;
  }
  
  if (NULL == ds_3gpp_rpm_util_params[subs_id])
  {
    DS_3GPP_MSG1_ERROR("RPM util params not allocated yet for subs_id: %d",
                        subs_id);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_READ_MASK

DESCRIPTION
  This function retrieves the rpm param read_mask.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint8

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_3gpp_rpm_get_read_mask
(
  sys_modem_as_id_e_type subs_id
)
{
  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return 0;
  }
  
  return (ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask);
} /* ds_3gpp_rpm_get_read_mask */

/*===========================================================================
FUNCTION DS_3GPP_RPM_SET_READ_MASK

DESCRIPTION
  This function sets the rpm param read_mask.

PARAMETERS
  mask - read_mask to be set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_set_read_mask
(
  uint8                  mask,
  sys_modem_as_id_e_type subs_id
)
{
  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask |= mask;
} /* ds_3gpp_rpm_set_read_mask */

/*===========================================================================
FUNCTION DS_3GPP_RPM_UNSET_READ_MASK

DESCRIPTION
  This function unsets the rpm param read_mask.

PARAMETERS
  mask - read_mask to be unset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_unset_read_mask
(
  uint8                  mask,
  sys_modem_as_id_e_type subs_id 
)
{
  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask &= mask;
} /* ds_3gpp_rpm_unset_read_mask */

/*===========================================================================
FUNCTION DS_3GPP_RPM_LR3_TMR_CB

DESCRIPTION
  This function is the LR3 timer cb.

PARAMETERS
  *callback_data - callback_data_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_lr3_tmr_cb
(
  unsigned long  callback_data
)
{
  ds_cmd_type   *cmd_buf = NULL;
  sys_modem_as_id_e_type *subs_id_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Timer CB for RPM leak rate");
  
  /*-----------------------------------------------------------------------
    Get a command buffer from the DS Task.
  -----------------------------------------------------------------------*/
  cmd_buf = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
  if((NULL == cmd_buf) || (NULL == cmd_buf->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_buf->hdr.cmd_id = DS_CMD_3GPP_RPM_LR3_TIMER_EXP;
    subs_id_ptr = (sys_modem_as_id_e_type*)cmd_buf->cmd_payload_ptr;
    *subs_id_ptr = (sys_modem_as_id_e_type)callback_data;
    ds_put_cmd(cmd_buf);
  }
} /* ds_3gpp_rpm_lr3_tmr_cb */

/*===========================================================================
FUNCTION DS_3GPP_RPM_LR3_TMR_EXP_HDLR

DESCRIPTION
  This function processes the LR3 timer expiry. Decrements CPDP counts,
  updates the EF file and restarts LR3 timer if required.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_lr3_tmr_exp_hdlr
(
  const ds_cmd_type *cmd_ptr
)
{
  uint32     current_time;
  uint32     write_offset;
  uint8      cpdp_index;
  boolean    restart_lr3 = FALSE;
  mmgsdi_write_data_type  write_data;
  ds_3gpp_rpm_data_buffer_type  efs_write_data;
  sys_modem_as_id_e_type  subs_id;

  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  { 
	DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
					"ds3g_plmn_change_hdlr: invalid cmd_ptr: %d");
	return;
  }

  subs_id = (*(sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr);

  /* Get Current time*/
  current_time = time_get_secs();

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  write_offset = (DS_3GPP_RPM_SUBS_BLK_SIZE * subs_id) +
                 DS_3GPP_RPM_LAST_CPDP_DECREMENT_TIME_OFFSET;

  /* Reset the timer running flag */
  ds_3gpp_rpm_util_params[subs_id]->is_lr3_tmr_running = FALSE;

  for (cpdp_index = 0; cpdp_index< DS_3GPP_RPM_MAX_CPDP_CNT; cpdp_index++)
  {
    /* Decrement the CPDP counters */
    if (ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index] != 0)
    {
      ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index]--;
    }

    if(ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index] != 0)
    {
      restart_lr3 = TRUE;
    }
  }

  memset(&write_data, 0, sizeof(write_data));
  memset(&efs_write_data, 0, sizeof(efs_write_data));
  
  efs_write_data.data_len = sizeof(uint32);
  efs_write_data.data_ptr = modem_mem_alloc(sizeof(uint32), 
                                                MODEM_MEM_CLIENT_DATA);

  if (NULL == efs_write_data.data_ptr)
  {
    DS_3GPP_MSG0_ERROR("Could not allocate memory for efs_write_data");
    return;
  }
  else
  {
    memscpy(efs_write_data.data_ptr, (sizeof(uint32)), 
            &current_time, sizeof(uint32));
  }

  /* Write last CPDP decrement time to EFS */
  if (FALSE == ds_3gpp_rpm_write_to_efs(&efs_write_data, 
                                        write_offset, 
                                        FALSE))
  {
    DS_3GPP_MSG0_ERROR("Write failed for CPDP last dec time");
  }

  modem_mem_free(efs_write_data.data_ptr, MODEM_MEM_CLIENT_DATA);

  memset(&write_data, 0, sizeof(write_data));
  
  write_data.data_len = DS_3GPP_RPM_MAX_CPDP_CNT * sizeof(uint8);
  write_data.data_ptr = (void *)(ds_3gpp_rpm_params[subs_id].cpdp_cnt);

  if (FALSE == ds_3gpp_rpm_mmgsdi_write(
                                        DS_3GPP_RPM_OFFSET_CPDP1,
                                        &write_data,
                                        subs_id
                                        ))
  {
    DS_3GPP_MSG0_ERROR("Write failed for CPDP count");
  }

  /* Restart LR3 and commit the data to SIM if any CPDP count is non-zero */
  if (TRUE == restart_lr3)
  {
    ds_3gpp_rpm_start_lr3_timer(current_time, 
                                (ds_3gpp_rpm_params[subs_id].leak_rate3 * SEC_PER_HOUR),
                                subs_id);
  }
  else
  {
    DS_3GPP_MSG0_HIGH("All CPDP counts are zero not restarting LR3 timer");

    /* Reset the last_cpdp_decrement_time and lr3_timer_timsestamp */
    memset(&efs_write_data, 0, sizeof(efs_write_data));
  
    efs_write_data.data_len = (2*sizeof(uint32));
    efs_write_data.data_ptr = modem_mem_alloc((2*sizeof(uint32)), 
                                                  MODEM_MEM_CLIENT_DATA);
  
    if (NULL == efs_write_data.data_ptr)
    {
      DS_3GPP_MSG0_ERROR("Could not allocate memory for efs_write_data");
      return;
    }
    else
    {
      memset(efs_write_data.data_ptr, 0, (2*sizeof(uint32)));
    }
  
    /* Write last CPDP decrement time to EFS */
    if (FALSE == ds_3gpp_rpm_write_to_efs(&efs_write_data, 
                                          write_offset, 
                                          FALSE))
    {
      DS_3GPP_MSG0_ERROR("Write failed");
    }
  
    modem_mem_free(efs_write_data.data_ptr, MODEM_MEM_CLIENT_DATA);
  }
} /* ds_3gpp_rpm_lr3_tmr_exp_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_CLEAR_THROTTLE_TIMER

DESCRIPTION
  This function processes the LR3 timer expiry. Decrements CPDP counts,
  updates the EF file and restarts LR3 timer if required.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_clear_throttle_timer
(
  byte                   *apn,
  sys_plmn_id_s_type     plmn_id,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr = NULL;

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if (FALSE == ds_3gpp_throt_sm_is_plmn_non_null(plmn_id))
  {
    /* Call throttle SM API to de-throttle PDP context requests to the APN */
    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm(apn, FALSE, 
                                           FALSE,
                                           DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, 
                                           subs_id);
  }
  else
  {
    DS_3GPP_MSG0_HIGH("PLMN id is valid, get pdn throt_sm_ptr frm plmn");
    pdn_throt_sm_ptr 
      = ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn(plmn_id, apn, subs_id);
  }

  if (NULL == pdn_throt_sm_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "pdn_throt_sm_ptr is NULL for apn: %s", apn);
    return;
  }
  else
  {
    if ( TRUE == 
         ds_3gppi_throt_sm_get_is_throttled_flag(pdn_throt_sm_ptr->v4_throttle_sm) )
    {
      ds_3gppi_throt_sm_clear_throttle_timer(pdn_throt_sm_ptr->v4_throttle_sm, 
                                             subs_id);
    }
    else if( TRUE == 
             ds_3gppi_throt_sm_get_is_throttled_flag(pdn_throt_sm_ptr->v6_throttle_sm) )
    {
      ds_3gppi_throt_sm_clear_throttle_timer(pdn_throt_sm_ptr->v6_throttle_sm, 
                                              subs_id);
    }
    else
    {
      DS_3GPP_MSG0_HIGH("APN is not throttled when hourly timer expired");
    }
  }
} /* ds_3gpp_rpm_clear_throttle_timer */

/*===========================================================================
FUNCTION DS_3GPP_RPM_HOURLY_TMR_CB

DESCRIPTION
  This function is the hourly timer cb.

PARAMETERS
  *callback_data - callback_data ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_hourly_tmr_cb
(
  unsigned long callback_data
)
{
  ds_cmd_type  *cmd_buf;

  /*-----------------------------------------------------------------------
    Get a command buffer from the DS Task.
  -----------------------------------------------------------------------*/
  cmd_buf = ds_allocate_cmd_buf(sizeof(uint32));
  if((NULL == cmd_buf) || (NULL == cmd_buf->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }
  else
  {
    memset(cmd_buf, 0, sizeof(ds_cmd_type));
    cmd_buf->hdr.cmd_id = DS_CMD_3GPP_RPM_HOURLY_TIMER_EXP;
    cmd_buf->cmd_payload_ptr= (void *)(callback_data);
    ds_put_cmd(cmd_buf);
  }
} /* ds_3gpp_rpm_hourly_tmr_cb */

/*===========================================================================
FUNCTION DS_3GPP_RPM_HOURLY_TMR_EXP_HDLR

DESCRIPTION
  This function processes the hourly timer expiry. Clears the backoff timers
  and updates EFS throttle table accordingly.

PARAMETERS
  *cmd_ptr - ds cmd ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_hourly_tmr_exp_hdlr
(  
  const ds_cmd_type  *cmd_ptr
)
{
  int8                                   apn_index = 0;
  ds_3gpp_rpm_throttle_param_type        *apn_tbl_ptr;
  sys_plmn_id_s_type                     zero_plmn_id;
  ds_3gpp_rpm_cause_code_enum_type       throttle_cause;
  ds_3gpp_rpm_efs_throttle_param_type    *efs_throttle_param_ptr;
  
  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  { 
	DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
					"ds3g_plmn_change_hdlr: invalid cmd_ptr: %d");
	return;
  }

  memset(&zero_plmn_id, 0, sizeof(zero_plmn_id));

  apn_tbl_ptr = (ds_3gpp_rpm_throttle_param_type *)
                             (cmd_ptr->cmd_payload_ptr);

  if (NULL != apn_tbl_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH, 
                        "Hourly timer expiry for APN: %s Fail cause: %d Subs: %d", 
                        apn_tbl_ptr->apn,
                        apn_tbl_ptr->last_throttle_cause,
                        apn_tbl_ptr->subs_id);

	if (!ds3gsubsmgr_is_subs_id_valid(apn_tbl_ptr->subs_id))
	{
	  return;
	}

    throttle_cause = apn_tbl_ptr->last_throttle_cause;

    /* Clear the timer running flag */
    apn_tbl_ptr->is_hourly_tmr_running = FALSE;

    if (DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS == throttle_cause)
    {
      apn_tbl_ptr->counter_val[throttle_cause - 1] = 0;
    }

    ds_3gpp_rpm_clear_throttle_timer(apn_tbl_ptr->apn, 
		                             zero_plmn_id,
		                             apn_tbl_ptr->subs_id);

    efs_throttle_param_ptr 
      = ds_3gpp_rpm_get_efs_throttle_param_ptr(apn_tbl_ptr->apn,
                                               FALSE, 
                                               &apn_index,
                                               apn_tbl_ptr->subs_id);

    if (NULL == efs_throttle_param_ptr)
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                              "APN: %s not present in efs_throttle_tbl",
                              apn_tbl_ptr->apn);
      return;
    }
    else
    {
      efs_throttle_param_ptr->hourly_tmr_timestamp  = 0;
      efs_throttle_param_ptr->backoff_tmr_timestamp = 0;
    }

    if (FALSE == ds_3gpp_rpm_write_throttle_params(apn_tbl_ptr->apn, 
		                                            apn_tbl_ptr->subs_id))
    {
      DS_3GPP_MSG0_ERROR("Could not write throttle params to EFS");
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("invalid apn_tbl_ptr");
    return;
  }
} /* ds_3gpp_rpm_hourly_tmr_exp_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_UPDATE_EFS_THROTTLE_TBL

DESCRIPTION
  This function populates the efs throttle table with data read from
  EFS file.

PARAMETERS
  plmn_id    - The plmn id.
  *cache_ptr - pointer to the local cache where the throttle params are read.
  apn_index  - the apn index in efs throttle param tbl.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_update_efs_throttle_tbl
(
  sys_plmn_id_s_type                      plmn_id,
  ds_3gpp_rpm_efs_throttle_param_type     *cache_ptr,
  uint8                                   apn_index,
  sys_modem_as_id_e_type                  subs_id
)
{
  uint8  first_free_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  int8  plmn_index;
  ds_3gpp_rpm_efs_throttle_tbl_type         *efs_throttle_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type *efs_throttle_param_ptr;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
	return;
  }

  DS_3GPP_MSG1_HIGH("ds_3gpp_rpm_update_efs_throttle_tbl for subs_id: %d",
  	                 subs_id);

  for (plmn_index = (DS_3GPP_MAX_PLMN_SUPPORTED - 1); 
       plmn_index >= 0; plmn_index--)
  {
    efs_throttle_tbl_ptr = ds_3gpp_rpm_efs_throttle_tbl[subs_id][plmn_index];

    if (NULL == efs_throttle_tbl_ptr)
    {
      first_free_index = plmn_index;
    }
    else if(memcmp(&(plmn_id),
                   &(efs_throttle_tbl_ptr->ds_plmn_id),
                   sizeof(sys_plmn_id_s_type)) == 0)
    {
      first_free_index = plmn_index;
      break;
    }
  }

  if (DS_3GPP_MAX_PLMN_SUPPORTED == first_free_index)
  {
    DS_3GPP_MSG0_ERROR("RPM efs throttle table full");
    return;
  }
  else if(plmn_index < 0 && 
          first_free_index < DS_3GPP_MAX_PLMN_SUPPORTED)
  {
    DS_3GPP_MSG0_HIGH("PLMN match not found in efs throttle tbl."
                      "Allocating memory for PLMN ID:");
    ds_3gpp_rpm_print_plmn_id(&plmn_id);

    efs_throttle_tbl_ptr = ds_3gpp_rpm_efs_throttle_tbl[subs_id][first_free_index];

    efs_throttle_tbl_ptr = (ds_3gpp_rpm_efs_throttle_tbl_type *)
                         modem_mem_alloc(sizeof(ds_3gpp_rpm_efs_throttle_tbl_type),
                                         MODEM_MEM_CLIENT_DATA);

    if (NULL == efs_throttle_tbl_ptr)
    {
      DATA_ERR_FATAL("Could not allocate memory for efs throttle tbl");
    }
    else
    {
      memset(efs_throttle_tbl_ptr, 0,sizeof(ds_3gpp_rpm_efs_throttle_tbl_type));
      ds_3gpp_rpm_efs_throttle_tbl[subs_id][first_free_index] = efs_throttle_tbl_ptr;
      memscpy(&(efs_throttle_tbl_ptr->ds_plmn_id),
	      sizeof(ds_3gpp_rpm_plmn_id_s_type),
              &(plmn_id),
              sizeof(sys_plmn_id_s_type));
    }
  }

  /* Update the APN block */
  efs_throttle_param_ptr = (ds_3gpp_rpm_efs_throttle_tbl[subs_id][first_free_index]->\
                       efs_throttle_param[apn_index]);
  if (TRUE == cache_ptr->is_valid_block)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                            "EFS APN table has valid entry." 
                            "index: %d, apn: %s", 
                            apn_index, 
                            cache_ptr->apn);
    efs_throttle_param_ptr = (ds_3gpp_rpm_efs_throttle_param_type *)
                          modem_mem_alloc(DS_3GPP_RPM_APN_BLK_SIZE,
                                          MODEM_MEM_CLIENT_DATA);

    if (NULL == efs_throttle_param_ptr)
    {
      DATA_ERR_FATAL("Could not allocate memory for APN block");
    }
    else
    {
      memset(efs_throttle_param_ptr, 0, DS_3GPP_RPM_APN_BLK_SIZE);

      ds_3gpp_rpm_efs_throttle_tbl[subs_id][first_free_index]->\
                               efs_throttle_param[apn_index] 
                                          = efs_throttle_param_ptr;
      memscpy(efs_throttle_param_ptr,
	      sizeof(ds_3gpp_rpm_efs_throttle_param_type),
              cache_ptr,
              DS_3GPP_RPM_APN_BLK_SIZE);
    }
  }
} /* ds_3gpp_rpm_update_efs_throttle_tbl */

/*===========================================================================
FUNCTION DS_3GPP_RPM_UPDATE_LOCAL_CACHE

DESCRIPTION
  This function populates the rpm param local cache with the values read
  from SIM.

PARAMETERS
  *read_cnf_ptr    - mmgsdi read_cnf ptr.
  *local_cache_ptr - pointer to the local cache.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_rpm_update_local_cache
(
  mmgsdi_read_cnf_type    *read_cnf_ptr,
  void                    *local_cache_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  uint8                                   index;
  uint32                                  client_ref;

  client_ref = read_cnf_ptr->response_header.client_data;
 
  /*-----------------------------------------------------------------------
    The reading from SIM is successful
  -----------------------------------------------------------------------*/
  memscpy( local_cache_ptr,
           read_cnf_ptr->read_data.data_len,
           read_cnf_ptr->read_data.data_ptr,
           read_cnf_ptr->read_data.data_len );

  switch (client_ref)
  {
    case DS_3GPP_RPM_EF_PATH_IS_RPM_ENABLED_FLAG:
      DS_3GPP_MSG1_HIGH("ds_3gpp_rpm_update_local_cache: RPM is_enabled_flag: %d",
                         ds_3gpp_rpm_params[subs_id].is_rpm_enabled);
      break;

    case DS_3GPP_RPM_EF_PATH_F_CNT:
      for (index = 0; index < DS_3GPP_RPM_MAX_F_CNT; index++)
      {
        DS_3GPP_MSG2_HIGH("ds_3gpp_rpm_update_local_cache: F[%d]: %d",
                          index, 
                          ds_3gpp_rpm_params[subs_id].f_cnt[index]);
      }

      /*---------------------------------------------------------------------- 
        F1 has to reconfigured to F1DS = (F1actual /5) + (F1actual % 5). NAS
        sends 5 PDP context Activation requests per PS call initiation when 
        network ignores the PDP Context requests.
      ----------------------------------------------------------------------*/
      ds_3gpp_rpm_params[subs_id].f_cnt[0] = ((ds_3gpp_rpm_params[subs_id].f_cnt[0]/5) +
                                              (ds_3gpp_rpm_params[subs_id].f_cnt[0]%5));
      break;

    case DS_3GPP_RPM_EF_PATH_LR3:
      DS_3GPP_MSG1_HIGH("ds_3gpp_rpm_update_local_cache: leak_rate: %d hrs",
                         ds_3gpp_rpm_params[subs_id].leak_rate3);
      break;

    case DS_3GPP_RPM_EF_PATH_CPDP_CNT:
      for (index = 0; index < DS_3GPP_RPM_MAX_CPDP_CNT; index++)
      {
        DS_3GPP_MSG2_HIGH("ds_3gpp_rpm_update_local_cache: CPDP[%d]: %d",
                          index, 
                          ds_3gpp_rpm_params[subs_id].cpdp_cnt[index]);
      }
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid clinet_ref: %d specified for RPM param",
                          client_ref);
  }
} /* ds_3gpp_rpm_update_local_cache */

/*===========================================================================
FUNCTION DS_3GPP_RPM_PARAMS_READ_RESP_CB

DESCRIPTION
  This function is the read_resp_cb for the RPM parameters to be read from
  the various EF files.

PARAMETERS
  status    - mmgsdi return status.
  cnf       - mmgsdi cnf type.
  *cnf_ptr  - mmgsdi cnf_ptr which contains the data read from SIM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_rpm_params_read_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_read_cnf_type        *read_cnf_ptr = NULL;
  ds_cmd_type                 *cmd_buf;
  uint32                      client_ref;
  ds3g_mmgsdi_event_info_type *mmgsdi_event_info_ptr = NULL;
  ds_at_mmgsdi_sim_info_type  *cmd_rd_ptr = NULL;
 
  if( NULL == cnf_ptr )
  {
    DS_3GPP_MSG0_ERROR("ds3g_mmgsdi_get_imsi_cb: cnf_ptr NULL");
    return;
  }

  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);
  client_ref   = cnf_ptr->read_cnf.response_header.client_data;
  DS_3GPP_MSG3_HIGH("ds_3gpp_rpm_params_resp_cb: session_id: %d client ref %d status %d",
                     cnf_ptr->response_header.session_id, client_ref, status);

  if (MMGSDI_SUCCESS != status && 
      MMGSDI_NOT_FOUND != status)
  {
    DS_3GPP_MSG1_ERROR("ds_3gpp_rpm_params_resp_cb: mmgsdi returns error: %d",
                        status);
    return;
  }
  else if (MMGSDI_NOT_FOUND == status)
  {
    cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
    if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    else
    {
      mmgsdi_event_info_ptr = 
	  	          (ds3g_mmgsdi_event_info_type*)(cmd_buf->cmd_payload_ptr);
	  
      cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);

	  cmd_rd_ptr->client_ref = cnf_ptr->read_cnf.response_header.client_data;
	  cmd_rd_ptr->session_id = cnf_ptr->read_cnf.response_header.session_id;
      cmd_buf->hdr.cmd_id = DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_FAIL;
      ds_put_cmd(cmd_buf);
    }

    return;
  }
  
  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id( 
    read_cnf_ptr->response_header.client_id ) != TRUE )
  {
    DS_3GPP_MSG1_LOW("ds_3gpp_rpm_params_resp_cb: Invalid client Id:%d",
                     read_cnf_ptr->response_header.client_id);
    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) && 
       ( MMGSDI_READ_CNF == cnf_ptr->read_cnf.response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) && 
         ( MMGSDI_SUCCESS == cnf_ptr->read_cnf.response_header.mmgsdi_status ) )
    {
      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }
      else
      {
        mmgsdi_event_info_ptr = 
	  	          (ds3g_mmgsdi_event_info_type*)(cmd_buf->cmd_payload_ptr);
	  
        cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);
    
        cmd_buf->hdr.cmd_id = DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_CNF;
        // Client data
        cmd_rd_ptr->client_ref = 
          cnf_ptr->read_cnf.response_header.client_data;
        //Session Id
		cmd_rd_ptr->session_id = 
		  cnf_ptr->read_cnf.response_header.session_id;
        // Copy read_data
        cmd_rd_ptr->data_len = 
          cnf_ptr->read_cnf.read_data.data_len;
        memscpy (cmd_rd_ptr->data, 
	         sizeof(cmd_rd_ptr->data),
                 cnf_ptr->read_cnf.read_data.data_ptr, 
                 (uint32)cmd_rd_ptr->data_len);   
        ds_put_cmd(cmd_buf);
      }
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "ds3g_mmgsdi_read_transparent_cb() "
                      "with status error [%d]", status);
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "ds3g_mmgsdi_read_transparent_cb() "
                    "with invalid CNF [%d]", cnf);
  }
} /* ds_3gpp_rpm_params_read_resp_cb */

/*===========================================================================
FUNCTION DS_3GPP_RPM_MMGSDI_READ_RESP_CMD_HDLR

DESCRIPTION
  This function processes the read_resp_cmd. Updates the local cache with
  the values read from SIM and updates the read_bitmask. When all parameters
  are read, posts READ_COMPLETE to DS task.

PARAMETERS
  *cmd_ptr    - ds cmd ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_mmgsdi_read_resp_cmd_hdlr
(
  const ds_cmd_type   *cmd_ptr
)
{
  mmgsdi_read_cnf_type       read_cnf;
  uint32                     client_ref;
  void                       *local_cache_ptr;
  uint8                      read_bitmask;
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr;
  ds3g_mmgsdi_event_info_type *mmgsdi_event_info_ptr = NULL;
  sys_modem_as_id_e_type     subs_id;

  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  { 
	DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
					"ds3g_plmn_change_hdlr: invalid cmd_ptr: %d");
	return;
  }

  mmgsdi_event_info_ptr = (ds3g_mmgsdi_event_info_type *)
  	                      (cmd_ptr->cmd_payload_ptr);

  cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);

  subs_id = ds3g_mmgsdi_get_sub_id_from_session_id(cmd_rd_ptr->session_id);

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  memset(&read_cnf, 0, sizeof(mmgsdi_read_cnf_type));
  read_cnf.read_data.data_ptr = (uint8 *)modem_mem_alloc(
                                (DS_3GPP_RPM_MAX_CAUSE_CODES * 
                                 DS_3GPP_RPM_PARAM_LEN), MODEM_MEM_CLIENT_DATA);

  if (NULL == read_cnf.read_data.data_ptr)
  {
    DS_3GPP_MSG0_ERROR("Could not allocate memory for read_data");
    return;
  }
  else
  {
    memset(read_cnf.read_data.data_ptr, 
           0, 
           (DS_3GPP_RPM_MAX_CAUSE_CODES * DS_3GPP_RPM_PARAM_LEN));
  }

  read_cnf.read_data.data_len          = cmd_rd_ptr->data_len;
  read_cnf.response_header.client_data = client_ref = cmd_rd_ptr->client_ref;

  memscpy((read_cnf.read_data.data_ptr), 
  	  (uint32)cmd_rd_ptr->data_len,
          cmd_rd_ptr->data,
          (uint32)cmd_rd_ptr->data_len);

  switch (client_ref)
  {
    case DS_3GPP_RPM_EF_PATH_IS_RPM_ENABLED_FLAG:
      local_cache_ptr = (void *)&(ds_3gpp_rpm_params[subs_id].is_rpm_enabled);
      read_bitmask = (uint8)DS_3GPP_RPM_ENABLED_FLAG_MASK;
      break;

    case DS_3GPP_RPM_EF_PATH_F_CNT:
      local_cache_ptr = (void *)(ds_3gpp_rpm_params[subs_id].f_cnt);
      read_bitmask = (uint8)DS_3GPP_RPM_F_CNT_MASK;
      break;

    case DS_3GPP_RPM_EF_PATH_LR3:
      local_cache_ptr = (void *)&(ds_3gpp_rpm_params[subs_id].leak_rate3);
      read_bitmask = (uint8)DS_3GPP_RPM_LR3_MASK;
      break;

    case DS_3GPP_RPM_EF_PATH_CPDP_CNT:
      local_cache_ptr = (void *)(ds_3gpp_rpm_params[subs_id].cpdp_cnt);
      read_bitmask = (uint8)DS_3GPP_RPM_CPDP_CNT_MASK;
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid client_ref: %d specified for RPM param",
                          client_ref);
      modem_mem_free((void *)(read_cnf.read_data.data_ptr), 
                     MODEM_MEM_CLIENT_DATA);
      return;
  }

  if ( read_cnf.read_data.data_len != 0 )
  {
    ds_3gpp_rpm_update_local_cache(
                                   &read_cnf,
                                   local_cache_ptr,
                                   subs_id
                                  );
  }

  modem_mem_free((void *)(read_cnf.read_data.data_ptr), 
                 MODEM_MEM_CLIENT_DATA);

  ds_3gpp_rpm_util_params[subs_id]->rpm_params_present_in_sim = TRUE;

  /*---------------------------------------------------------------------
    Update the bitmask for the rpm read_param
  ---------------------------------------------------------------------*/
  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask |= read_bitmask;

  /*---------------------------------------------------------------------
    Process read complete when all RPM parameters are read
  ---------------------------------------------------------------------*/
  if (DS_3GPP_RPM_ALL_PARAMS_READ_MASK == 
  	  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask)
  {
    ds_3gpp_rpm_process_read_complete(subs_id);
  }
} /* ds_3gpp_rpm_mmgsdi_read_resp_cmd_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_MMGSDI_READ_FAIL_CMD_HDLR

DESCRIPTION
  This function processes the read_fail_cmd. Updates the local cache with
  the values read from NV and updates the read_bitmask. When all parameters
  are read, posts READ_COMPLETE to DS task.

PARAMETERS
  *cmd_ptr    - ds cmd ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_mgsdi_read_fail_cmd_hdlr
(
  const ds_cmd_type   *cmd_ptr
)
{
  uint8                      read_bitmask = 0;
  uint32                     client_ref;
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr;  
  ds3g_mmgsdi_event_info_type *mmgsdi_event_info_ptr = NULL;
  mmgsdi_session_id_type     session_id;
  sys_modem_as_id_e_type     subs_id;

  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  { 
	DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
					"ds3g_plmn_change_hdlr: invalid cmd_ptr: %d");
	return;
  }

  mmgsdi_event_info_ptr = (ds3g_mmgsdi_event_info_type *)
  	                      (cmd_ptr->cmd_payload_ptr);

  cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);
  
  client_ref = cmd_rd_ptr->client_ref;
  session_id = cmd_rd_ptr->session_id;

  subs_id = ds3g_mmgsdi_get_sub_id_from_session_id(session_id);

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  ds_3gpp_rpm_util_params[subs_id]->rpm_params_present_in_sim = FALSE;
  
  switch (client_ref)
  {
    case DS_3GPP_RPM_EF_PATH_IS_RPM_ENABLED_FLAG:
      read_bitmask = (uint8)DS_3GPP_RPM_ENABLED_FLAG_MASK;
      break;

    case DS_3GPP_RPM_EF_PATH_F_CNT:
      read_bitmask = (uint8)DS_3GPP_RPM_F_CNT_MASK;

      /*---------------------------------------------------------------------- 
        F1 has to reconfigured to F1DS = (F1actual /5) + (F1actual % 5). NAS
        sends 5 PDP context Activation requests per PS call initiation when 
        network ignores the PDP Context requests.
      ----------------------------------------------------------------------*/
      ds_3gpp_rpm_params[subs_id].f_cnt[0] = ((ds_3gpp_rpm_params[subs_id].f_cnt[0]/5) +
                                              (ds_3gpp_rpm_params[subs_id].f_cnt[0]%5));
      break;

    case DS_3GPP_RPM_EF_PATH_LR3:
      read_bitmask = (uint8)DS_3GPP_RPM_LR3_MASK;
      break;

    case DS_3GPP_RPM_EF_PATH_CPDP_CNT:
      read_bitmask = (uint8)DS_3GPP_RPM_CPDP_CNT_MASK;
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid client_ref: %d specified for RPM param",
                          client_ref);
      return;
  }

  /*---------------------------------------------------------------------
    Update the bitmask for the rpm read_param
  ---------------------------------------------------------------------*/
  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask |= read_bitmask;

  /*---------------------------------------------------------------------
    Post a command to DS task if all RPM parameters are read
  ---------------------------------------------------------------------*/
  if (DS_3GPP_RPM_ALL_PARAMS_READ_MASK == 
  	  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask)
  {
    ds_3gpp_rpm_process_read_complete(subs_id);
  }
}

/*===========================================================================
FUNCTION DS_3GPP_RPM_PARAMS_WRITE_RESP_CB

DESCRIPTION
  This function is the write_resp_cb function for the RPM parameters to be
  written onto the SIM.

PARAMETERS
  status    - mmgsdi return status.
  cnf       - mmgsdi cnf type.
  *cnf_ptr  - mmgsdi cnf_ptr which contains the write status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_rpm_params_write_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
        cnf_ptr->write_cnf.response_header.client_id ) != TRUE )
  {
    DS_3GPP_MSG2_LOW("ds_3gpp_rpm_params_write_resp_cb(): "
                     "Invalid client Id:%d client_ref:%d",
                     cnf_ptr->write_cnf.response_header.client_id,
                     cnf_ptr->write_cnf.response_header.client_data);

    return;
  }

  if ( ( MMGSDI_WRITE_CNF == cnf ) && 
       ( MMGSDI_WRITE_CNF == cnf_ptr->write_cnf.response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) && 
         ( MMGSDI_SUCCESS == cnf_ptr->write_cnf.response_header.mmgsdi_status ) )
    {
      DS_3GPP_MSG2_MED("ds_3gpp_rpm_params_write_resp_cb() with "
                       "client ref [%d] status [%d]", 
                       cnf_ptr->write_cnf.response_header.client_data,
                       status);
    }
    else
    {
      DS_3GPP_MSG2_ERROR("ds_3gpp_rpm_params_write_resp_cb() "
                         "with client_ref [%d], status error [%d]",
                         cnf_ptr->write_cnf.response_header.client_data,
                         status);
    }
  }
  else
  {
    DS_3GPP_MSG2_ERROR("ds_3gpp_rpm_params_write_resp_cb() "
                       "with invalid CNF [%d], clinet_ref [%d]",
                       cnf,
                       cnf_ptr->write_cnf.response_header.client_data);
  }
} /* ds_3gpp_rpm_params_write_resp_cb */

/*===========================================================================
FUNCTION DS_3GPP_RPM_MMGSDI_READ

DESCRIPTION
  This function issues mmgsdi_read to read RPM params from specific EF file
  using path_access method.

PARAMETERS
  session_id     - mmgsdi_session_id_type.
  rpm_param_path - the EF file path which is to be read.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_mmgsdi_read
(
  mmgsdi_session_id_type         session_id,
  uint16                         rpm_param_path
)
{
  mmgsdi_return_enum_type mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_offset_type      offset           = 0;
  mmgsdi_len_type         param_len        = 0;
  mmgsdi_access_type      rpm_access;
  mmgsdi_app_enum_type    app_type;
  uint16                  *rpm_path;
  uint8                   ef_path_idx;

  memset(&rpm_access, 0, sizeof(mmgsdi_access_type));

  app_type = ds3g_mmgsdi_get_app_type_from_session_id(session_id);

  rpm_path = (uint16 *)modem_mem_alloc(DS_3GPP_RPM_USIM_FILE_PATH_LEN*sizeof(uint16),
                                       MODEM_MEM_CLIENT_DATA);

  if (NULL == rpm_path)
  {
    DS_3GPP_MSG0_ERROR("Couldn't allocate memory for rpm_path_ptr");
    return FALSE;
  }
  else
  {
    memset(rpm_path, 0, DS_3GPP_RPM_USIM_FILE_PATH_LEN*sizeof(uint16));
  }

  if (MMGSDI_APP_SIM == app_type)
  {
    rpm_path[0] = DS_3GPP_RPM_MF;
    rpm_path[1] = DS_3GPP_RPM_DF1;
    rpm_path[2] = DS_3GPP_RPM_DF2;
    ef_path_idx = (DS_3GPP_RPM_SIM_FILE_PATH_LEN - 1);
    rpm_access.file.path_type.path_len = DS_3GPP_RPM_SIM_FILE_PATH_LEN;
  }
  else if (MMGSDI_APP_USIM == app_type)
  {
    rpm_path[0] = DS_3GPP_RPM_MF;
    rpm_path[1] = DS_3GPP_RPM_USIM_ADF;
    rpm_path[2] = DS_3GPP_RPM_DF1;
    rpm_path[3] = DS_3GPP_RPM_DF2;
    ef_path_idx = (DS_3GPP_RPM_USIM_FILE_PATH_LEN - 1);
    rpm_access.file.path_type.path_len = DS_3GPP_RPM_USIM_FILE_PATH_LEN;
  }
  else
  {
    DS_3GPP_MSG2_ERROR("Invalid app type %d for session_id: %d",
                        app_type, session_id);
    modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  rpm_path[ef_path_idx] = rpm_param_path;

  DS_3GPP_MSG0_HIGH("Sending Request for Reading RPM parameters");

  /* Get RPM parameters from SIM */
  rpm_access.access_method = MMGSDI_BY_PATH_ACCESS;

  memscpy( &(rpm_access.file.path_type.path_buf), 
  	   (rpm_access.file.path_type.path_len)*sizeof(uint16),
           rpm_path,
           (rpm_access.file.path_type.path_len)*sizeof(uint16) );

  switch (rpm_param_path)
  {
    case DS_3GPP_RPM_EF_PATH_IS_RPM_ENABLED_FLAG:
      offset           = DS_3GPP_RPM_OFFSET_IS_RPM_ENABLED_FLAG;
      param_len        = DS_3GPP_RPM_PARAM_LEN;
      break;

    case DS_3GPP_RPM_EF_PATH_F_CNT:
      offset           = DS_3GPP_RPM_OFFSET_F1;
      param_len        = 4 * DS_3GPP_RPM_PARAM_LEN;
      break;

    case DS_3GPP_RPM_EF_PATH_LR3:
      offset           = DS_3GPP_RPM_OFFSET_LR3;
      param_len        = DS_3GPP_RPM_PARAM_LEN;
      break;

    case DS_3GPP_RPM_EF_PATH_CPDP_CNT:
      offset           = DS_3GPP_RPM_OFFSET_CPDP1;
      param_len        = 4*DS_3GPP_RPM_PARAM_LEN;
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid path: %d specified for RPM param",
                          rpm_param_path);
      modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);
      return FALSE;
  }

  DS_3GPP_MSG3_HIGH("DBG: Path_len: %d, Path_buf[0]: 0x%x Path_buf[1]: 0x%x",
                     rpm_access.file.path_type.path_len,
                     rpm_access.file.path_type.path_buf[0],
                     rpm_access.file.path_type.path_buf[1]);

  DS_3GPP_MSG3_HIGH("DBG: Path_buf[2]: 0x%x Path_buf[3]: 0x%x Path_buf[4]: 0x%x",
                     rpm_access.file.path_type.path_buf[2],
                     rpm_access.file.path_type.path_buf[3],
                     rpm_access.file.path_type.path_buf[4]);


  mmgsdi_status = mmgsdi_session_read_transparent( 
                                                  session_id,
                                                  rpm_access,
                                                  offset,
                                                  param_len,
                                                  ds_3gpp_rpm_params_read_resp_cb,
                                                  (mmgsdi_client_data_type)rpm_param_path
                                                 );

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DS_3GPP_MSG2_ERROR("Failed to read RPM param using session id [%d], status [%d]",
                        session_id, mmgsdi_status);
    modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);

  return TRUE;
} /* ds_3gpp_rpm_mmgsdi_read */

/*===========================================================================
FUNCTION DS_3GPP_RPM_MMGSDI_WRITE

DESCRIPTION
  This function issues mmgsdi_write to write RPM params to specific EF file
  using path_access method.

PARAMETERS
  offset     - offset in the file where the data has to be written.
  *write_data - pointer to the data to be written.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_mmgsdi_write
(
  mmgsdi_offset_type             offset,
  mmgsdi_write_data_type         *write_data,
  sys_modem_as_id_e_type         subs_id
)
{
  mmgsdi_return_enum_type mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_client_data_type client_ref       = 0;
  mmgsdi_session_id_type  session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_access_type      rpm_access;
  mmgsdi_app_enum_type    app_type;
  uint16                  *rpm_path;
  uint8                   ef_path_idx;

  if (write_data == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_rpm_mmgsdi_read: write_data is NULL");
    return FALSE;
  }

  if( FALSE == ds3g_mmgsdi_get_session_id_from_sub_id(subs_id, &session_id) )
  {
    DS_3GPP_MSG1_LOW("Failed to get session id using subs_id: %d", subs_id);
    return FALSE;
  }

  memset(&rpm_access, 0, sizeof(rpm_access));

  client_ref = (mmgsdi_client_data_type)offset;

  app_type = ds3g_mmgsdi_get_app_type_from_session_id(session_id);

  rpm_path = (uint16 *)modem_mem_alloc(DS_3GPP_RPM_USIM_FILE_PATH_LEN*sizeof(uint16),
                                       MODEM_MEM_CLIENT_DATA);

  if (NULL == rpm_path)
  {
    DS_3GPP_MSG0_ERROR("Couldn't allocate memory for rpm_path_ptr");
    return FALSE;
  }
  else
  {
    memset(rpm_path, 0, DS_3GPP_RPM_USIM_FILE_PATH_LEN*sizeof(uint16));
  }

  if (MMGSDI_APP_SIM == app_type)
  {
    rpm_path[0] = DS_3GPP_RPM_MF;
    rpm_path[1] = DS_3GPP_RPM_DF1;
    rpm_path[2] = DS_3GPP_RPM_DF2;
    ef_path_idx = (DS_3GPP_RPM_SIM_FILE_PATH_LEN - 1);
    rpm_access.file.path_type.path_len = DS_3GPP_RPM_SIM_FILE_PATH_LEN;
  }
  else if (MMGSDI_APP_USIM == app_type)
  {
    rpm_path[0] = DS_3GPP_RPM_MF;
    rpm_path[1] = DS_3GPP_RPM_USIM_ADF;
    rpm_path[2] = DS_3GPP_RPM_DF1;
    rpm_path[3] = DS_3GPP_RPM_DF2;
    ef_path_idx = (DS_3GPP_RPM_USIM_FILE_PATH_LEN - 1);
    rpm_access.file.path_type.path_len = DS_3GPP_RPM_USIM_FILE_PATH_LEN;
  }
  else
  {
    DS_3GPP_MSG2_ERROR("Invalid app type %d for session_id: %d",
                        app_type, session_id);
    modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  /* Write RPM parameters to SIM */
  rpm_access.access_method = MMGSDI_BY_PATH_ACCESS;

  switch (offset)
  {
    case DS_3GPP_RPM_OFFSET_CPDP1:
      rpm_path[ef_path_idx] = DS_3GPP_RPM_EF_PATH_CPDP_CNT;
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid offset: %d specified for RPM param",
                          client_ref);
      modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);
      return FALSE;
  }

  memscpy( &(rpm_access.file.path_type.path_buf), 
  	   (rpm_access.file.path_type.path_len)*sizeof(uint16),
           rpm_path,
          (rpm_access.file.path_type.path_len)*sizeof(uint16) );

  DS_3GPP_MSG3_HIGH("DBG: ds_3gpp_rpm_mmgsdi_write: path_len: %d path_buf[0]: 0x%x path_buf[1]: 0x%x",
                    rpm_access.file.path_type.path_len,
                    rpm_access.file.path_type.path_buf[0],
                    rpm_access.file.path_type.path_buf[1]);

  DS_3GPP_MSG3_HIGH("DBG: ds_3gpp_rpm_mmgsdi_write: path_buf[2]: 0x%x"
                    "path_buf[3]: 0x%x path_buf[4]: 0x%x",
                    rpm_access.file.path_type.path_buf[2],
                    rpm_access.file.path_type.path_buf[3],
                    rpm_access.file.path_type.path_buf[4]);

  DS_3GPP_MSG2_HIGH("DBG: ds_3gpp_rpm_mmgsdi_write: write_data_len: %d write_data[0]: %d",
                     write_data->data_len, write_data->data_ptr[0]);

  DS_3GPP_MSG3_HIGH("DBG: ds_3gpp_rpm_mmgsdi_write: write_data[1]: %d"
                     "write_data[2]: %d write_data[3]: %d",
                     write_data->data_ptr[1],
                     write_data->data_ptr[2],
                     write_data->data_ptr[3]);

  mmgsdi_status = mmgsdi_session_write_transparent( 
                                                   session_id,
                                                   rpm_access,
                                                   offset,
                                                   *write_data,
                                                   ds_3gpp_rpm_params_write_resp_cb,
                                                   client_ref
                                                  );

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DS_3GPP_MSG2_ERROR("Failed to write RPM param using session id [%d], status [%d]",
                        session_id, mmgsdi_status);
    modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  modem_mem_free((void *)rpm_path, MODEM_MEM_CLIENT_DATA);

  return TRUE;
} /* ds_3gpp_rpm_mmgsdi_write */

/*===========================================================================
FUNCTION DS_3GPP_RPM_VERIFY_EFS_THROTTLE_PARAM_FILE

DESCRIPTION
  This function verifies the EFS throttle param file if already present.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_rpm_verify_efs_throttle_param_file(void)
{
  fs_size_t  file_size = 0;
  struct fs_stat sbuf;

  memset(&sbuf, 0, sizeof(sbuf));
                        
  if( mcfg_fs_stat(ds_3gpp_rpm_file_path,
                   &sbuf,
                   MCFG_FS_TYPE_EFS,
                   MCFG_FS_SUBID_NONE) != MCFG_FS_STATUS_OK )
  {
    DATA_MSG1_ERROR("Invalid file errno=%d.", mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    ds_3gpp_rpm_create_efs_throttle_param_file();
    return;
  }
  else
  {
    file_size = sbuf.st_size;
		
    if ( (file_size%DS_3GPP_RPM_SUBS_BLK_SIZE) != 0 )
    {
      DS_3GPP_MSG1_ERROR("RPM efs file is not multi-sim capable, resetting: %d",
                          file_size);
      file_size = 0;
    }  
    else
    {
      DS_3GPP_MSG1_MED("RPM efs file is multi-sim capable, file_size: %d",
                        file_size);	
    }
  }

  if (file_size == 0)
  {
    if (FALSE == ds_3gpp_rpm_write_to_efs(NULL, 0, TRUE))
    {
      DS_3GPP_MSG0_ERROR("Error in resetting efs throttle param file");
    }
  }

  return;
  
}/* ds_3gpp_rpm_verify_efs_throttle_param_file */

/*===========================================================================
FUNCTION DS_3GPP_RPM_CREATE_EFS_THROTTLE_PARAM_FILE

DESCRIPTION
  This function creates the EFS throttle param file.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_rpm_create_efs_throttle_param_file( void )
{
  int32  fs_handle;

  fs_handle = mcfg_fopen( ds_3gpp_rpm_file_path,
                          MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                          MCFG_FS_DEFFILEMODE,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
                        
  if ( fs_handle == -1)
  {
    DATA_MSG2_ERROR("efs_open failed for fd=%d errno=%d.",
                    fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------*/
  if (mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1)
  {
    DS_3GPP_MSG3_ERROR("efs_close failed for %d errno = %d", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS), 0);
    return FALSE;
  }

  /* Reset the throttle params */
  if (FALSE == ds_3gpp_rpm_write_to_efs(NULL, 0, TRUE))
  {
    DS_3GPP_MSG0_ERROR("Error in resetting efs throttle param file");
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPP_RPM_READ_FROM_EFS

DESCRIPTION
  This function reads all the valid PLMN throttle tables from EFS.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_read_from_efs
(
  sys_modem_as_id_e_type  subs_id
)
{
  int32                                fs_handle;
  uint8                                plmn_index;
  uint8                                apn_index;
  ds_3gpp_rpm_efs_throttle_param_type  apn_blk;
  sys_plmn_id_s_type                   temp_plmn_id;
  fs_off_t                             file_offset;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Open the file for read 
  ------------------------------------------------------------------*/
  fs_handle = mcfg_fopen( ds_3gpp_rpm_file_path,
                          MCFG_FS_O_RDONLY,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  if ( fs_handle == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_open failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return FALSE;
  }

    
  /* Offset to the subs_id block to read */
  file_offset = (subs_id*DS_3GPP_RPM_SUBS_BLK_SIZE) + 
                 DS_3GPP_RPM_LAST_CPDP_DECREMENT_TIME_OFFSET;

  DS_3GPP_MSG1_ERROR("EFS_DBG: Initial File offset: %d", file_offset);
  
  if( mcfg_lseek(fs_handle,
                file_offset,
                 SEEK_SET,
                 MCFG_FS_TYPE_EFS) == -1 )
  {
    DS_3GPP_MSG2_ERROR("Cannot offset to file_pos: %d for subs_id: %d",
		                file_offset, subs_id);
    return FALSE;
  }
   
  if( mcfg_fread(fs_handle,
               &(ds_3gpp_rpm_util_params[subs_id]->last_cpdp_decrement_time), 
                 sizeof(uint32),
                 MCFG_FS_TYPE_EFS) == -1)
  {
    DS_3GPP_MSG2_ERROR("efs_read failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }
  else
  {
    DS_3GPP_MSG1_HIGH("last_cpdp_decrement_time read from EFS is %d",
                       ds_3gpp_rpm_util_params[subs_id]->last_cpdp_decrement_time);
  }
   
  if(mcfg_fread(fs_handle,
               &(ds_3gpp_rpm_util_params[subs_id]->lr3_tmr_timestamp), 
                sizeof(uint32),
                MCFG_FS_TYPE_EFS) == -1)
  {
    DS_3GPP_MSG2_ERROR("efs_read failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }
  else
  {
    DS_3GPP_MSG1_HIGH("lr3_tmr_timestamp read from EFS is %d",
                       ds_3gpp_rpm_util_params[subs_id]->lr3_tmr_timestamp);
  }
  
  for (plmn_index = 0; plmn_index < DS_3GPP_MAX_PLMN_SUPPORTED; plmn_index++)
  {
    memset(&temp_plmn_id, 0, sizeof(temp_plmn_id));
  
    if( mcfg_fread(fs_handle,
                 (void *)&temp_plmn_id,
                   sizeof(sys_plmn_id_s_type),
                   MCFG_FS_TYPE_EFS) != -1)
    {
      DS_3GPP_MSG1_HIGH("PLMN read from EFS for index: %d is:", plmn_index);
      ds_3gpp_rpm_print_plmn_id((void *)&temp_plmn_id);
  
      if (FALSE == ds_3gpp_throt_sm_is_plmn_non_null(temp_plmn_id))
      {
        DS_3GPP_MSG1_HIGH("PLMN ID zero for index : %d", plmn_index);
        break;
      }
      else
      {
        for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
        {
          memset(&apn_blk, 0, DS_3GPP_RPM_APN_BLK_SIZE);
  
          if( mcfg_fread(fs_handle,
                       (void *)&apn_blk,
                         DS_3GPP_RPM_APN_BLK_SIZE,
                         MCFG_FS_TYPE_EFS) != -1)
          {
            DS_3GPP_MSG3_HIGH("APN block %d has is_valid flag: %d and last fail cause %d",
                               apn_index,
                               apn_blk.is_valid_block,
                               apn_blk.last_throttle_cause
                              );
  
            /*----------------------------------------------------------------
          Check if EFS params are valid. If the is_valid flag is set to
          a value other than 0 or 1, then the EFS values are invalid and
          thus the whole file has to be reset to default values (0).
        -----------------------------------------------------------------*/
            if (apn_blk.is_valid_block > 1)
            {
              DS_3GPP_MSG0_HIGH("RPM efs throttle params are invalid."
                                "Resetting the file");
              
              if (FALSE == ds_3gpp_rpm_write_to_efs(NULL, 0, TRUE))
              {
                DS_3GPP_MSG0_ERROR("Error in resetting efs throttle param file");
              }
              break;
            }
  
            DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                                    "APN read from EFS is %s", 
                                    apn_blk.apn);
            ds_3gpp_rpm_update_efs_throttle_tbl(temp_plmn_id,
                                                &apn_blk, 
                                                apn_index,
                                                subs_id);
          }
          else
          {
            DS_3GPP_MSG2_ERROR( "efs_read failed for %d errno=%d", 
                                fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
            return FALSE;
          }
        }
      }
    }
    else
    {
      DS_3GPP_MSG2_ERROR( "efs_read failed for %d errno=%d", 
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------*/
  if( mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_close failed for %d errno = %d", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  //For Debug: Delete the file
  //if (efs_unlink(ds_3gpp_rpm_file_path) == -1)
  //{
  //  DS_3GPP_MSG1_ERROR("RPM throttle file delete failed with error: %d", efs_errno);
  //}
  

  return TRUE;
} /* ds_3gpp_rpm_read_from_efs */

/*===========================================================================
FUNCTION DS_3GPP_RPM_WRITE_TO_EFS

DESCRIPTION
  This function writes the data passes to it at the specified offset in the
  RPM efs throttle param file.

PARAMETERS
  *write_data      - write data buffer
  write_offset     - offset in the file where the data is to be written.
  reset_all_params - whether to reset all params to zero.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_write_to_efs
(
  ds_3gpp_rpm_data_buffer_type    *write_data,
  uint32                          write_offset,
  boolean                         reset_all_plmns
)
{
  int32    fs_handle;
  uint32   temp_write_data = 0;
  uint8    plmn_index;
  uint8    apn_index;
  ds_3gpp_rpm_plmn_id_s_type           plmn_id;
  ds_3gpp_rpm_efs_throttle_param_type  apn_blk;
  boolean                              ret_val = FALSE;
  uint32                               subs_idx;

  /*-------------------------------------------------------------------
    Open the file for write 
  -------------------------------------------------------------------*/
  fs_handle = mcfg_fopen( ds_3gpp_rpm_file_path,
                          MCFG_FS_O_RDWR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE );
  if ( fs_handle == -1)
  {
    DS_3GPP_MSG2_ERROR("efs_open failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  if (TRUE == reset_all_plmns)
  {
    for (subs_idx = SYS_MODEM_AS_ID_1; subs_idx <= SYS_MODEM_AS_ID_2; subs_idx++)
    {
      /* subs_id */
      if( mcfg_fwrite(fs_handle,
                    &subs_idx, 
                      sizeof(sys_modem_as_id_e_type),
                      MCFG_FS_TYPE_EFS) == -1 )
      {
        DS_3GPP_MSG2_ERROR("efs_write failed for %d errno=%d.", 
                            fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        ret_val = FALSE;
      }
	  // file_pos = (1, 8098)
      /* last cpdp dec time */
      if( mcfg_fwrite(fs_handle,
                    &temp_write_data, 
                      sizeof(uint32),
                      MCFG_FS_TYPE_EFS) == -1 )
      {
        DS_3GPP_MSG2_ERROR("efs_write failed for %d errno=%d.", 
                            fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        ret_val = FALSE;
      }
      // file_pos = (5, 8102)
      temp_write_data =0;
  
      /* lr3 timestamp */
      if( mcfg_fwrite(fs_handle,
                    &temp_write_data, 
                      sizeof(uint32),
                      MCFG_FS_TYPE_EFS) == -1 )
      {
        DS_3GPP_MSG2_ERROR("efs_write failed for %d errno=%d.", 
                            fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        ret_val = FALSE;
      }
      // file_pos = (9, 8106)
      for (plmn_index = 0; plmn_index < DS_3GPP_MAX_PLMN_SUPPORTED; plmn_index++)
      {
        memset(&plmn_id, 0, sizeof(plmn_id));
        /* PLMN ID */
        if( mcfg_fwrite(fs_handle,
                      (void *)&plmn_id, 
                        sizeof(ds_3gpp_rpm_plmn_id_s_type),
                        MCFG_FS_TYPE_EFS) == -1 )
        {
          DS_3GPP_MSG2_ERROR("efs_write failed for %d errno = %d",
                              fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
          ret_val = FALSE;
        }
        else
        {
          for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
          {
            memset(&apn_blk, 0, DS_3GPP_RPM_APN_BLK_SIZE);
            /* APN block */
            if( mcfg_fwrite(fs_handle,
                           (void *)&apn_blk, 
                            (DS_3GPP_RPM_APN_BLK_SIZE),
                            MCFG_FS_TYPE_EFS) == -1 )
            {
              DS_3GPP_MSG2_ERROR("efs_write failed for %d errno = %d",
                                  fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
              ret_val = FALSE;
            }
            else
            {
              ret_val = TRUE;
            }
          }
        }
      }// file_pos = (8097, 16194)
    }
  }
  else
  {
    if( mcfg_lseek(fs_handle,
                  write_offset,
                   SEEK_SET,
                   MCFG_FS_TYPE_EFS) == -1 )
    {
      DATA_MSG2_ERROR( "efs_lseek failed for %d errno=%d",
                       fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }

    if ( mcfg_fwrite(fs_handle,
                     write_data->data_ptr,
                     write_data->data_len,
                     MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG2_ERROR("efs_write failed for %d errno = %d",
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      ret_val = FALSE;
    }
    else
    {
      ret_val = TRUE;
    }
  }

  /*-------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------*/
  if( mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_close failed for %d errno = %d", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    ret_val = FALSE;
  }

  return ret_val;
} /* ds_3gpp_rpm_write_to_efs */

/*===========================================================================
FUNCTION DS_3GPP_RPM_WRITE_EFS_THROTTLE_TBL

DESCRIPTION
  This function writes the plmn block at the specified plmn_index in the 
  RPM efs throttle param file.

PARAMETERS
  *efs_throttle_tbl_ptr - pointer to the plmn block in efs throttle table.
  plmn_index            - index of the plmn in the efs throttle table.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_write_efs_throttle_tbl
(
  ds_3gpp_rpm_efs_throttle_tbl_type  *efs_throttle_tbl_ptr,
  uint8                              plmn_index,
  sys_modem_as_id_e_type             subs_id
)
{
  int8                              apn_index;
  uint32                            write_offset = 0;
  uint32                            subs_id_offset = 0;
  ds_3gpp_rpm_data_buffer_type      write_data;
  ds_3gpp_rpm_efs_throttle_param_type *efs_throttle_param_ptr;

  subs_id_offset = (DS_3GPP_RPM_SUBS_BLK_SIZE * subs_id);
  write_data.data_len = sizeof(sys_plmn_id_s_type);
  write_data.data_ptr = (void *)&(efs_throttle_tbl_ptr->ds_plmn_id);
  write_offset = subs_id_offset + (DS_3GPP_RPM_PLMN_BLK_OFFSET +
                                   (plmn_index * DS_3GPP_RPM_PLMN_BLK_SIZE));

  DS_3GPP_MSG1_HIGH("Writing at offset : %d PLMN ID:", write_offset);
  ds_3gpp_rpm_print_plmn_id((void *)(write_data.data_ptr));
  
  if (FALSE == ds_3gpp_rpm_write_to_efs(&write_data, 
                                        write_offset, 
                                        FALSE))
  {
    DS_3GPP_MSG1_ERROR("Failed to write plmn_id for index : %d",
                        plmn_index);
    return FALSE;
  }

  memset(&write_data, 0, sizeof(write_data));

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    efs_throttle_param_ptr = efs_throttle_tbl_ptr->efs_throttle_param[apn_index];

    if (NULL != efs_throttle_param_ptr)
    {
      write_data.data_len = DS_3GPP_RPM_APN_BLK_SIZE;
      write_data.data_ptr = (void *)efs_throttle_param_ptr;
      write_offset        = subs_id_offset + 
	  	                    (DS_3GPP_RPM_APN_BLK_OFFSET + 
                             (plmn_index * DS_3GPP_RPM_PLMN_BLK_SIZE) +
                             (apn_index * DS_3GPP_RPM_APN_BLK_SIZE));
    
      DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                              "Writing APN block for APN: %s, write_offset: %d",
                              (efs_throttle_param_ptr->apn), write_offset);
    
      if (FALSE == ds_3gpp_rpm_write_to_efs(&write_data, 
                                            write_offset, 
                                            FALSE))
      {
        DS_3GPP_MSG1_ERROR("Failed to write apn block for index : %d",
                            apn_index);
        return FALSE;
      }
    }
  }

  return TRUE;
} /* ds_3gpp_rpm_write_efs_throttle_tbl */

/*===========================================================================
FUNCTION DS_3GPP_RPM_WRITE_THROTTLE_PARAMS

DESCRIPTION
  This function writes the apn block for the specified apn and the current
  plmn_id.

PARAMETERS
  *apn - apn for which the throttle params have to be written.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_write_throttle_params
(
  byte *apn,
  sys_modem_as_id_e_type subs_id
)
{
  int8                              plmn_index = 0;
  int8                              apn_index;
  uint32                            write_offset = 0;
  uint32                            subs_id_offset = 0;
  uint32                            plmn_blk_size;
  uint32                            apn_blk_size;
  ds_3gpp_rpm_data_buffer_type      write_data;
  ds_3gpp_rpm_efs_throttle_tbl_type    *plmn_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type *efs_throttle_param_ptr;

  /* For Debug */
  ds_3gpp_rpm_efs_throttle_param_type  efs_apn_tbl;
  int32                                  fs_handle;
  
  memset(&efs_apn_tbl, 0, sizeof(ds_3gpp_rpm_efs_throttle_param_type));
  memset(&write_data, 0, sizeof(write_data));
  subs_id_offset = (DS_3GPP_RPM_SUBS_BLK_SIZE * subs_id);
  
  plmn_tbl_ptr = ds_3gpp_rpm_get_efs_throttle_tbl_ptr(FALSE, 
  	                                                  &plmn_index,
  	                                                  subs_id);

  if (NULL == plmn_tbl_ptr)
  {
    DS_3GPP_MSG0_ERROR("No entry for current plmn id in efs throttle tbl");
    return FALSE;
  }

  efs_throttle_param_ptr = ds_3gpp_rpm_get_efs_throttle_param_ptr(
                                                              apn,
                                                              FALSE, 
                                                              &apn_index,
                                                              subs_id
                                                             );

  if (NULL == efs_throttle_param_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "efs_throttle_param_ptr is NULL for APN: %s", apn);
    return FALSE;
  }

  if (plmn_index >= 0 && plmn_index < DS_3GPP_MAX_PLMN_SUPPORTED)
  {
    write_data.data_len = sizeof(sys_plmn_id_s_type);
    write_data.data_ptr = (void *)&(plmn_tbl_ptr->ds_plmn_id);
    write_offset = subs_id_offset + (DS_3GPP_RPM_PLMN_BLK_OFFSET +
                                     (plmn_index * DS_3GPP_RPM_PLMN_BLK_SIZE));

    plmn_blk_size = DS_3GPP_RPM_PLMN_BLK_SIZE;

    DS_3GPP_MSG2_HIGH("PLMN index is %d, PLMN blk size is %d", 
                       plmn_index,plmn_blk_size);

    DS_3GPP_MSG1_HIGH("Writing at offset : %d PLMN ID:", write_offset);
    ds_3gpp_rpm_print_plmn_id((void *)(write_data.data_ptr));
    
    if (FALSE == ds_3gpp_rpm_write_to_efs(&write_data, 
                                          write_offset, 
                                          FALSE))
    {
      DS_3GPP_MSG1_ERROR("Failed to write plmn_id for index : %d",
                          plmn_index);
      return FALSE;
    }

    memset(&write_data, 0, sizeof(write_data));

    write_data.data_len = DS_3GPP_RPM_APN_BLK_SIZE;
    write_data.data_ptr = (void *)efs_throttle_param_ptr;
    write_offset        = subs_id_offset + 
		                  (DS_3GPP_RPM_APN_BLK_OFFSET + 
                          (plmn_index * DS_3GPP_RPM_PLMN_BLK_SIZE) +
                          (apn_index * DS_3GPP_RPM_APN_BLK_SIZE));

    apn_blk_size = DS_3GPP_RPM_APN_BLK_SIZE;

    DS_3GPP_MSG2_HIGH("APN index is %d, APN blk size is %d", 
                       apn_index,apn_blk_size);

    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                            "Writing APN block for APN: %s, write_offset: %d",
                            efs_throttle_param_ptr->apn, write_offset);

    /* For Debugging purpose */
    DS_3GPP_MSG0_HIGH("APN table contents to be written are:");
    DS_3GPP_MSG3_HIGH("is_valid_blk: %d, last_throttle_cause: %d, counter_val: %d",
                       efs_throttle_param_ptr->is_valid_block,
                       efs_throttle_param_ptr->last_throttle_cause,
                       efs_throttle_param_ptr->counter_val);

    DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH, 
                            "Hourly timestamp: %d, backoff timestamp: %d, apn: %s", 
                            efs_throttle_param_ptr->hourly_tmr_timestamp, 
                            efs_throttle_param_ptr->backoff_tmr_timestamp, 
                            efs_throttle_param_ptr->apn);

    if (FALSE == ds_3gpp_rpm_write_to_efs(&write_data, 
                                          write_offset, 
                                          FALSE))
    {
      DS_3GPP_MSG1_ERROR("Failed to write apn block for index : %d",
                          apn_index);
      return FALSE;
    }

    /* For Debugging. Read the values written onto EFS file */
    fs_handle = mcfg_fopen(ds_3gpp_rpm_file_path,
                           MCFG_FS_O_RDONLY,
                           MCFG_FS_ALLPERMS,
                           MCFG_FS_TYPE_EFS,
                           MCFG_FS_SUBID_NONE);

    if ( fs_handle == -1)
    {
      DS_3GPP_MSG2_ERROR("efs_open failed for %d errno=%d.", 
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }

    if( mcfg_lseek(fs_handle,
                   write_offset,
                   SEEK_SET,
                   MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG2_ERROR("efs_lseek failed for %d errno=%d.", 
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }

    if( mcfg_fread(fs_handle,
                 (void *)&efs_apn_tbl, 
                   DS_3GPP_RPM_APN_BLK_SIZE,
                   MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG2_ERROR("efs_read failed for %d errno=%d.", 
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }
    else
    {
      DS_3GPP_MSG0_HIGH("APN table contents are:");
      DS_3GPP_MSG3_HIGH("is_valid_blk: %d, last_throttle_cause: %d, counter_val: %d",
                         efs_apn_tbl.is_valid_block,
                         efs_apn_tbl.last_throttle_cause,
                         efs_apn_tbl.counter_val);

      DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH, 
                              "Hourly timestamp: %d, backoff timestamp: %d, apn: %s", 
                              efs_apn_tbl.hourly_tmr_timestamp, 
                              efs_apn_tbl.backoff_tmr_timestamp, 
                              efs_apn_tbl.apn);
    }

    /*-------------------------------------------------------------------
      Close the file
    -------------------------------------------------------------------*/
    if( mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG2_ERROR("efs_close failed for %d errno = %d", 
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }
  }

  return TRUE;
} /* ds_3gpp_rpm_write_throttle_params */

/*===========================================================================
FUNCTION DS_3GPP_RPM_READ_ALL_RPM_PARAMETERS

DESCRIPTION
  This function reads all RPM params from SIM as well as throttle params
  from EFS.

PARAMETERS
  session_id - mmgsdi_session_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_read_all_rpm_parameters
(
  sys_modem_as_id_e_type   subs_id
)
{
  uint8    index;
  mmgsdi_session_id_type session_id;

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  memset(&session_id, 0, sizeof(mmgsdi_session_id_type));

  if (FALSE == ds3g_mmgsdi_get_session_id_from_sub_id(subs_id, &session_id))
  {
    DS_3GPP_MSG1_LOW("Could not get session ID for subs_id: %d", subs_id);
    return;
  }
  
  /* Reset the rpm_read_bitmask */
  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask = 0x00;

  DS_3GPP_MSG0_HIGH("DBG: RPM parameters read from EFS:");
  DS_3GPP_MSG2_HIGH("DBG: RPM is_enabled flag: %d, leak_rate: %d hrs",
                     ds_3gpp_rpm_params[subs_id].is_rpm_enabled,
                     ds_3gpp_rpm_params[subs_id].leak_rate3);

  for (index = 0; index < DS_3GPP_RPM_MAX_F_CNT; index++)
  {
    DS_3GPP_MSG2_HIGH("DBG: F[%d]: %d",
                      index, 
                      ds_3gpp_rpm_params[subs_id].f_cnt[index]);
  }

  for (index = 0; index < DS_3GPP_RPM_MAX_CPDP_CNT; index++)
  {
    DS_3GPP_MSG2_HIGH("DBG: CPDP[%d]: %d",
                      index, 
                      ds_3gpp_rpm_params[subs_id].cpdp_cnt[index]);
  }

  /* Read the RPM enabled flag */
  if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                        session_id,
                                        DS_3GPP_RPM_EF_PATH_IS_RPM_ENABLED_FLAG
                                        ) )
  {
    DS_3GPP_MSG0_ERROR("Failed to read RPM enabled flag");
    return;
  }

  /* Read the F counts */
  if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                        session_id,
                                        DS_3GPP_RPM_EF_PATH_F_CNT
                                        ) )
  {
    DS_3GPP_MSG0_ERROR("Failed to read F counts");
    return;
  }

  /* Read LR3 */
  if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                        session_id,
                                        DS_3GPP_RPM_EF_PATH_LR3
                                        ) )
  {
    DS_3GPP_MSG0_ERROR("Failed to read LR3");
    return;
  }

  /* Read C-PDP */
  if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                        session_id,
                                        DS_3GPP_RPM_EF_PATH_CPDP_CNT
                                        ) )
  {
    DS_3GPP_MSG0_ERROR("Failed to read CPDP");
    return;
  }

  /* Read RPM efs throttle params */
  if ( FALSE == ds_3gpp_rpm_read_from_efs(subs_id) )
  {
    DS_3GPP_MSG0_ERROR("Failed to read efs throttle params");
    return;
  }
  else
  {
    ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask |= 
		                                (uint8)DS_3GPP_RPM_THROTTLE_PARAM_MASK;
  }

  return;
} /* ds_3gpp_rpm_read_all_rpm_parameters */

/*===========================================================================
FUNCTION DS_3GPP_RPM_PROCESS_READ_CMD

DESCRIPTION
  This function processes the read command. This would read specific RPM 
  parameters depening on the bitmask or read all params if read_all_param is
  set.

PARAMETERS
  *cmd_ptr - ds cmd ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_process_read_cmd
(
  boolean                read_all_params,
  sys_modem_as_id_e_type subs_id
)
{
  mmgsdi_session_id_type session_id;
  uint8 read_bitmask = 0;

  memset(&session_id, 0, sizeof(mmgsdi_session_id_type));

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  if (FALSE == ds3g_mmgsdi_get_session_id_from_sub_id(subs_id, &session_id))
  {
    DS_3GPP_MSG1_LOW("Could not get session ID for subs_id: %d", subs_id);
    return;
  }

  if (TRUE == read_all_params)
  {
    ds_3gpp_rpm_read_all_rpm_parameters(subs_id);
  }
  else
  {
    read_bitmask = ds_3gpp_rpm_get_read_mask(subs_id);
    read_bitmask = ~(read_bitmask);
    
    if ( (read_bitmask & (uint8)(DS_3GPP_RPM_ENABLED_FLAG_MASK)) )
    {
      /* Read the RPM enabled flag */
      if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                         session_id,
                                         DS_3GPP_RPM_EF_PATH_IS_RPM_ENABLED_FLAG
                                         ) )
      {
        DS_3GPP_MSG0_ERROR("Failed to read RPM enabled flag");
        return;
      }
    }
    else if ( (read_bitmask & (uint8)(DS_3GPP_RPM_F_CNT_MASK)) )
    {
      /* Read the F Count */
      if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                            session_id,
                                            DS_3GPP_RPM_EF_PATH_F_CNT
                                         ) )
      {
        DS_3GPP_MSG0_ERROR("Failed to read F count");
        return;
      }
    }
    else if ( (read_bitmask & (uint8)(DS_3GPP_RPM_LR3_MASK)) )
    {
      /* Read the LR3 timer value */
      if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                            session_id,
                                            DS_3GPP_RPM_EF_PATH_LR3
                                         ) )
      {
        DS_3GPP_MSG0_ERROR("Failed to read LR3 timer value");
        return;
      }
    }
    else if ( (read_bitmask & (uint8)(DS_3GPP_RPM_CPDP_CNT_MASK)) )
    {
      /* Read the CPDP count */
      if ( FALSE == ds_3gpp_rpm_mmgsdi_read(
                                            session_id,
                                            DS_3GPP_RPM_EF_PATH_CPDP_CNT
                                         ) )
      {
        DS_3GPP_MSG0_ERROR("Failed to read CPDP count");
        return;
      }
    }
    else if ( (read_bitmask & (uint8)(DS_3GPP_RPM_THROTTLE_PARAM_MASK)) )
    {
      /* Read RPM efs throttle params. This would result in error for old file */
      if ( FALSE == ds_3gpp_rpm_read_from_efs(subs_id) )
      {
        DS_3GPP_MSG0_ERROR("Failed to read efs throttle params");
        return;
      }
      else
      {
        ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask |= 
			                            (uint8)DS_3GPP_RPM_THROTTLE_PARAM_MASK;
      }
    }
  }
} /* ds_3gpp_rpm_process_read_cmd */

/*===========================================================================
FUNCTION DS_3GPP_RPM_START_LR3_TIMER

DESCRIPTION
  This function starts the LR3 timer for the duration specified. If LR3 is
  started for the complete duration then the LR3 timestamp is written to EFS.

PARAMETERS
  timestamp - current timestamp.
  timer_val - timer duration in seconds.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_start_lr3_timer
(
  uint32                 timestamp,
  uint32                 timer_val,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_rpm_data_buffer_type  efs_write_data;
  uint32                        write_offset;
  int32                         fs_handle;

  if (timer_val == 0)
  {
    DS_3GPP_MSG0_HIGH("lr3 timer value is 0, not starting lr3 timer");
    return;
  }

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  if (FALSE == ds_3gpp_rpm_util_params[subs_id]->rpm_params_present_in_sim)
  {
    DATA_MSG0_HIGH("RPM parameters are not present in SIM, not starting LR3 timer");
    return;
  }

  if ( FALSE == (ds_3gpp_rpm_util_params[subs_id]->is_lr3_tmr_running) )
  {
    DS_3GPP_RPM_SET_TIMER(&(ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_tmr_lr3), 
                          SEC_TO_MS(timer_val),
                          ds_3gpp_rpm_util_params[subs_id]->is_lr3_tmr_running);

    DS_3GPP_MSG1_HIGH("Started LR3 timer for %d seconds", timer_val);

    if (timer_val == (ds_3gpp_rpm_params[subs_id].leak_rate3 * SEC_PER_HOUR))
    {
      memset(&efs_write_data, 0, sizeof(efs_write_data));
    
      efs_write_data.data_len = sizeof(uint32);
      efs_write_data.data_ptr = modem_mem_alloc(sizeof(uint32), 
                                                MODEM_MEM_CLIENT_DATA);

      if (NULL == efs_write_data.data_ptr)
      {
        DS_3GPP_MSG0_ERROR("Could not allocate memory for efs_write_data");
        return;
      }
      else
      {
        memscpy(efs_write_data.data_ptr, (sizeof(uint32)), &timestamp, sizeof(uint32));
		write_offset = ((DS_3GPP_RPM_SUBS_BLK_SIZE * subs_id) +
			             DS_3GPP_RPM_LR3_TIMESTAMP_OFFSET);
        DS_3GPP_MSG1_HIGH("lr3 timestamp written is : %d",
                           timestamp);
      }
    
      if (FALSE == ds_3gpp_rpm_write_to_efs(&efs_write_data, 
                                            write_offset,
                                            FALSE))
      {
        DS_3GPP_MSG0_ERROR("Write failed for CPDP last dec time");
      }



      /* Debug code */
      fs_handle = mcfg_fopen( ds_3gpp_rpm_file_path,
                              MCFG_FS_O_RDWR,
                              MCFG_FS_ALLPERMS,
                              MCFG_FS_TYPE_EFS,
                              MCFG_FS_SUBID_NONE );
      if ( fs_handle == -1)
      {
        DS_3GPP_MSG2_ERROR("efs_open failed for %d errno=%d.", 
                            fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        return;
      }
      if( mcfg_lseek(fs_handle,
                    write_offset,
                     SEEK_SET,
                     MCFG_FS_TYPE_EFS) == -1 )
      {
        DATA_MSG2_ERROR( "efs_lseek failed for %d errno=%d",
                         fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        return;
      }

      if( mcfg_fread(fs_handle,
                   &(ds_3gpp_rpm_util_params[subs_id]->lr3_tmr_timestamp), 
                   sizeof(uint32),
                   MCFG_FS_TYPE_EFS) == -1 )
      {
        DS_3GPP_MSG2_ERROR("efs_read failed for %d errno=%d.", 
                            fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        return;
      }
      else
      {
        DS_3GPP_MSG1_HIGH("lr3_tmr_timestamp read from EFS is %d",
                           ds_3gpp_rpm_util_params[subs_id]->lr3_tmr_timestamp);
      }

      modem_mem_free(efs_write_data.data_ptr, MODEM_MEM_CLIENT_DATA);
    }
  }
} /* ds_3gpp_rpm_start_lr3_timer */

/*===========================================================================
FUNCTION DS_3GPP_RPM_POWERUP_LR3_TMR_HDLR

DESCRIPTION
  This function handles the LR3 timer restart after powerup. Following three
  conditions dictate the behaviour:
  1. If last_cpdp_dec_timestamp is non-zero, then re-calculate the CPDP count,
     if cpdp changes write back to SIM, update last_cpdp_dec_time in EFS and
     restart LR3 if any CPDP is non-zero after re-calcuating the LR3 timer_val
  2. If last_cpdp_dec_timestamp is 0, and LR3 timer timestamp is non-zero,
     then LR3 timer should be restarted for the remaining duration.
  3. If both the above parameters are zero, then LR3 timer shoudl be started
     if LR3 timer val is non-zero and CPDP counts are non-zero.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_powerup_lr3_tmr_hdlr
(
  sys_modem_as_id_e_type subs_id
)
{
  uint32                        current_time;
  uint32                        time_offset;
  uint8                         cpdp_index;
  int32                         floor_val;
  boolean                       restart_lr3 = FALSE;
  boolean                       cpdp_cnt_changed = FALSE;
  uint32                        new_lr3_timer_val;
  ds_3gpp_rpm_data_buffer_type  efs_write_data;
  mmgsdi_write_data_type        write_data;
  uint32                        last_cpdp_decrement_time;
  uint32                        lr3_tmr_timestamp;
  uint32                        write_offset;

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  current_time = time_get_secs();
  last_cpdp_decrement_time = ds_3gpp_rpm_util_params[subs_id]->last_cpdp_decrement_time;
  lr3_tmr_timestamp = ds_3gpp_rpm_util_params[subs_id]->lr3_tmr_timestamp;
  /*------------------------------------------------------------------------
    Check if LR3 was started and recalculate the LR3 and restart the timer
  ------------------------------------------------------------------------*/
  if (last_cpdp_decrement_time != 0)
  {
    DS_3GPP_MSG0_HIGH("last_cpdp_decrement_time is non-zero");
    time_offset = (current_time - last_cpdp_decrement_time);
    /* Recalculate CPDP values */
    for (cpdp_index = 0; cpdp_index< DS_3GPP_RPM_MAX_CPDP_CNT; cpdp_index++)
    {
      /* Decrement the CPDP counters */
      if (ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index] != 0)
      {
        if ( (floor_val = 
              ds_math_floor(time_offset, 
                            ((ds_3gpp_rpm_params[subs_id].leak_rate3) * SEC_PER_HOUR))) > 0 )
        {
          ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index] -= (uint8)floor_val;
          cpdp_cnt_changed                         = TRUE;
          DS_3GPP_MSG2_HIGH("New cpdp cnt is CPDP[%d] = %d", 
                             cpdp_index, 
                             ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index]);
        }
      }

      if(ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index] != 0)
      {
        restart_lr3 = TRUE;
      }
    }

    /* Write CPDP count in SIM as well save last_cpdp_dec_time if
       cpdp has changed*/
    if (TRUE == cpdp_cnt_changed)
    {
      memset(&efs_write_data, 0, sizeof(efs_write_data));
      efs_write_data.data_len = sizeof(uint32);
  
      efs_write_data.data_ptr = modem_mem_alloc(sizeof(uint32), 
                                                  MODEM_MEM_CLIENT_DATA);
  
      if (NULL == efs_write_data.data_ptr)
      {
        DS_3GPP_MSG0_ERROR("Could not allocate memory for efs_write_data");
        return;
      }
      else
      {
        memscpy(efs_write_data.data_ptr, (sizeof(uint32)), 
                &current_time, sizeof(uint32));
        write_offset = ((DS_3GPP_RPM_SUBS_BLK_SIZE * subs_id) + 
			             DS_3GPP_RPM_LAST_CPDP_DECREMENT_TIME_OFFSET);
      }

	  
  
      /* Write last CPDP decrement time to EFS */
      if (FALSE == ds_3gpp_rpm_write_to_efs(&efs_write_data, 
                                            write_offset, 
                                            FALSE))
      {
        DS_3GPP_MSG0_ERROR("Write failed for CPDP last dec time");
      }
  
      modem_mem_free(efs_write_data.data_ptr, MODEM_MEM_CLIENT_DATA);

      memset(&write_data, 0, sizeof(write_data));
    
      write_data.data_len = DS_3GPP_RPM_MAX_CPDP_CNT * sizeof(uint8);
      write_data.data_ptr = ds_3gpp_rpm_params[subs_id].cpdp_cnt;
  
      if (FALSE == ds_3gpp_rpm_mmgsdi_write(
                                            DS_3GPP_RPM_OFFSET_CPDP1,
                                            &write_data,
                                            subs_id
                                           ))
      {
        DS_3GPP_MSG0_ERROR("Write failed for CPDP count");
      }
    }

    if (restart_lr3 == TRUE)
    {
      new_lr3_timer_val = ( (((ds_3gpp_rpm_params[subs_id].leak_rate3) * SEC_PER_HOUR)*
                    (ds_math_ceil(time_offset, 
                                  ((ds_3gpp_rpm_params[subs_id].leak_rate3) * SEC_PER_HOUR)))) -
                     time_offset );

      DS_3GPP_MSG1_HIGH("New LR3 timer val: %d seconds", 
                         new_lr3_timer_val);
      /*------------------------------------------------------------------
        Restart LR3 timer
      -------------------------------------------------------------------*/
      ds_3gpp_rpm_start_lr3_timer(current_time, new_lr3_timer_val, subs_id);
    }
    
  }
  else if (lr3_tmr_timestamp != 0)
  {
    time_offset = (current_time - lr3_tmr_timestamp);

    if (time_offset < (ds_3gpp_rpm_params[subs_id].leak_rate3 * SEC_PER_HOUR))
    {
      new_lr3_timer_val = ((ds_3gpp_rpm_params[subs_id].leak_rate3 * SEC_PER_HOUR)
                            - time_offset);
      DS_3GPP_MSG1_HIGH("time elapsed since lr3 started is %d",
                         time_offset);
      ds_3gpp_rpm_start_lr3_timer(current_time, new_lr3_timer_val, subs_id);
    }
  }
  else
  {
    /* If the CPDP counts are non-zero then LR3 timer has to be started */
    if ( (ds_3gpp_rpm_params[subs_id].leak_rate3 != 0) &&
         ((ds_3gpp_rpm_params[subs_id].cpdp_cnt[0] != 0) ||
          (ds_3gpp_rpm_params[subs_id].cpdp_cnt[1] != 0) ||
          (ds_3gpp_rpm_params[subs_id].cpdp_cnt[2] != 0) ||
          (ds_3gpp_rpm_params[subs_id].cpdp_cnt[3] != 0)) )
    {
      DS_3GPP_MSG0_HIGH("Starting LR3 timer as CPDP counts are non-zero");
      ds_3gpp_rpm_start_lr3_timer(current_time, 
                                  (ds_3gpp_rpm_params[subs_id].leak_rate3 * SEC_PER_HOUR),
                                  subs_id);
    }
  }
} /* ds_3gpp_rpm_powerup_lr3_tmr_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_POWERUP_THROTTLE_HDLR

DESCRIPTION
  This function handles the throttling after powerup. If current PLMN ID is
  non-zero and is present in the efs throttle tbl, then for each valid APN in
  this plmn block, backoff timer value is calculated if backoff timer
  timestamp is non-zero and the backoff timers are started for the remaining
  duration.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_power_up_throttle_hdlr
(
  sys_modem_as_id_e_type subs_id
)
{
  uint32                  current_time;
  uint32                  elapsed_time_backoff = 0;
  uint32                  elapsed_time_hour = 0;
  int8                    index;
  int8                    apn_index;
  int8                    plmn_index;
  ds_3gpp_rpm_efs_throttle_tbl_type          *efs_throttle_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type  *efs_throttle_param_ptr;
  ds_3gpp_rpm_throttle_param_type         *apn_tbl_ptr;
  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr = NULL;
  ds_3gpp_rpm_cause_code_enum_type      throttle_cause = DS_3GPP_RPM_CAUSE_CODE_MIN;

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  
  efs_throttle_tbl_ptr = ds_3gpp_rpm_get_efs_throttle_tbl_ptr(
                                                             FALSE,
                                                             &plmn_index,
                                                             subs_id
                                                            );

  if (NULL == efs_throttle_tbl_ptr)
  {
    DS_3GPP_MSG0_ERROR("Current PLMN ID not found in efs throttle tbl");
    return;
  }
  else
  {
    current_time = time_get_secs();
    DS_3GPP_MSG1_HIGH("ds_3gpp_rpm_power_up_throttle_hdlr: current time is %d",
                       current_time);
    for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
    {
      efs_throttle_param_ptr = efs_throttle_tbl_ptr->efs_throttle_param[apn_index];
      if (NULL != efs_throttle_param_ptr)
      {
        apn_tbl_ptr = ds_3gpp_rpm_get_throttle_param_ptr(efs_throttle_param_ptr->apn,
                                                         TRUE,
                                                         &index,
                                                         subs_id);

        if (NULL == apn_tbl_ptr)
        {
          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                              "Could not get an entry for APN: %s",
                              efs_throttle_param_ptr->apn);
        }
        else
        {
          DS_3GPP_MSG2_HIGH("Hourly tmr timestamp: %d, backoff tmr timestamp: %d",
                             efs_throttle_param_ptr->hourly_tmr_timestamp,
                             efs_throttle_param_ptr->backoff_tmr_timestamp);

          apn_tbl_ptr->last_throttle_cause = throttle_cause =
                       efs_throttle_param_ptr->last_throttle_cause;

		  apn_tbl_ptr->subs_id = subs_id;

     	  if (apn_tbl_ptr->last_throttle_cause <= DS_3GPP_RPM_CAUSE_CODE_MIN || 
              apn_tbl_ptr->last_throttle_cause > DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS)
     	  {
     	    DS_3GPP_MSG1_ERROR("Invalid throttle cause: %d", 
     			        apn_tbl_ptr->last_throttle_cause);
     	    continue;
     	  }

          apn_tbl_ptr->counter_val[apn_tbl_ptr->last_throttle_cause - 1] =
              efs_throttle_param_ptr->counter_val;

          if (efs_throttle_param_ptr->hourly_tmr_timestamp != 0)
          {
            elapsed_time_hour = (current_time -
                                 efs_throttle_param_ptr->hourly_tmr_timestamp);
          }

          if (elapsed_time_hour != 0 && 
              elapsed_time_hour < SEC_PER_HOUR)
          {
            if (throttle_cause != DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS)
            {
              if (efs_throttle_param_ptr->backoff_tmr_timestamp != 0)
              {
                ds_3gpp_rpm_get_backoff_timer_val(apn_tbl_ptr, subs_id);
                elapsed_time_backoff = (1000*(current_time - 
                                        efs_throttle_param_ptr->backoff_tmr_timestamp));
  
                if (elapsed_time_backoff < apn_tbl_ptr->backoff_timer_val)
                {
                  apn_tbl_ptr->backoff_timer_val -= elapsed_time_backoff;
                }
              }
              else
              {
                DS_3GPP_MSG1_ERROR("Backoff timer timestamp is 0. Fail cause: %d",
                                    throttle_cause);
              }
            }
            else
            {
              if (apn_tbl_ptr->counter_val[throttle_cause -1] == 
                  ds_3gpp_rpm_params[subs_id].f_cnt[throttle_cause - 1])
              {
                apn_tbl_ptr->backoff_timer_val = (1000 *(SEC_PER_HOUR -
                                                  elapsed_time_hour));
              }
            }
          }
          else
          {
            DS_3GPP_MSG1_HIGH("Elapsed time is %d wont restart" 
                              " backoff timers", elapsed_time_hour);
          }

          if (apn_tbl_ptr->backoff_timer_val > 0)
          {
            /* Allocate throttle SM and start the backoff timer */
            DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH, 
                          "Backoff timer val is %d for apn: %s, throttle_cause: %d", 
                          apn_tbl_ptr->backoff_timer_val,
                          apn_tbl_ptr->apn,
                          apn_tbl_ptr->last_throttle_cause);

            ds3gpp_enter_global_crit_section();
            pdn_throt_sm_ptr = 
                ds_3gpp_pdn_throt_sm_get_throttle_sm(apn_tbl_ptr->apn,
                                                     FALSE, 
                                                     TRUE,
						     DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
                                                     subs_id);

            if (NULL == pdn_throt_sm_ptr)
            {
              DS_3GPP_MSG0_ERROR("Could not allocate a throt_sm_ptr");
            }
            else
            {
              ds_3gppi_throt_sm_feed_failure_to_core_sm(
                                            pdn_throt_sm_ptr->v4_throttle_sm,
                                            NULL, subs_id,NULL);
              ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
            }

            ds3gpp_leave_global_crit_section();

            if (elapsed_time_hour != 0 && 
                throttle_cause != DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS)
            {
              /* Start the hourly timer for the remaining duration only
                 for PDP failure cause */
              ds_3gpp_rpm_start_hourly_timer(apn_tbl_ptr->apn,
                                 (1000*(SEC_PER_HOUR - elapsed_time_hour)),
                                 subs_id);
            }

            ds_3gpp_rpm_write_throttle_params(apn_tbl_ptr->apn, subs_id);
          }
          else
          {
            DS_3GPP_MSG1_HIGH("Backoff timer is %d. Not restarting backoff timers", 
                               apn_tbl_ptr->backoff_timer_val);

            if (throttle_cause == DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS)
            {
              DS_3GPP_MSG1_HIGH("Last throttle cause is %d. Resetting success_cnt",
                                 throttle_cause);
              apn_tbl_ptr->counter_val[throttle_cause - 1] = 0;
            }
          }
        }
      }
      else
      {
        DS_3GPP_MSG1_HIGH("efs throttle param tbl is NULL for index %d", 
                           apn_index);
      }
    }
  }

} /* ds_3gpp_rpm_power_up_throttle_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_PROCESS_READ_COMPLETE

DESCRIPTION
  This function processes the read_complete_cmd. Sets the RPM initialized 
  flag to TRUE and invokes the powerup handlers for LR3 and throttling.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_process_read_complete
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_dsd_apm_ind_pay_load  apm_ind_payload;

  DS_3GPP_MSG1_HIGH("In ds_3gpp_rpm_process_read_complete(). subs_id: %d",
  	                 subs_id);

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }

  if (TRUE == ds_3gpp_rpm_params[subs_id].is_rpm_enabled)
  {
    /* This is to ensure that we do not do power-up processing 
         after processing a MMGSDI_REFRESH */
    if (ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_is_initialized == FALSE)
    {
      /* Set RPM initialized flag to TRUE */
      ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_is_initialized = TRUE;
  
      /* LR3 timer processing */
      ds_3gpp_rpm_powerup_lr3_tmr_hdlr(subs_id);
  
      /* Indicate to APM that RPM is enabled */
      apm_ind_payload.subs_id = subs_id;
      (void)ds_dsd_apm_ind(DS_DSD_APM_RPM_INIT_IND, &apm_ind_payload);
    }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("RPM is not enabled");
  }
} /* ds_3gpp_rpm_process_read_complete */

/*===========================================================================
FUNCTION DS_3GPP_RPM_INIT

DESCRIPTION
  This function does the powerup initialization for the RPM module.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_init()
{
  struct fs_stat sbuf;
  sys_modem_as_id_e_type  subs_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&ds_3gpp_rpm_params, 0, sizeof(ds_3gpp_rpm_params));
  subs_id = ds3g_get_ps_subs_id();

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ds_3gpp_rpm_read_efs_per_subs(subs_id);

  /* At this time util params are not allocated */
  //ds_3gpp_rpm_ut ds_3gpp_rpm_is_initialized = FALSE;

  /* Read the RPM default params from EFS */

  strlcpy(ds_3gpp_rpm_dir_name, 
              DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME,
              sizeof(ds_3gpp_rpm_dir_name));

  ds_3gpp_rpm_dir_name[DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME_LEN] = 
                                                      DS_3GPP_FS_DIRECTORY_CHAR;
  ds_3gpp_rpm_dir_name[DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME_LEN + 1] = '\0';

  /* Check if the RPM throttle param file is present */
  strlcpy(ds_3gpp_rpm_file_path, 
              ds_3gpp_rpm_dir_name, 
              sizeof(ds_3gpp_rpm_file_path));

  strlcat(ds_3gpp_rpm_file_path,
              DS_3GPP_RPM_THROTTLE_PARAM_FILE_NAME,
              sizeof(ds_3gpp_rpm_file_path) );

  ds_3gpp_rpm_file_path[DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME_LEN + \
                        1+ \
                        DS_3GPP_RPM_THROTTLE_PARAM_FILE_NAME_LEN] = '\0';

  if( mcfg_fs_stat((char*) ds_3gpp_rpm_dir_name,
                   &sbuf,
                   MCFG_FS_TYPE_EFS,
                   MCFG_FS_SUBID_NONE) == MCFG_FS_STATUS_OK)
  {
    if( mcfg_fs_stat((char*) ds_3gpp_rpm_file_path,
                     &sbuf,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE) != MCFG_FS_STATUS_OK )
    {
      /* RPM throttle param file not present. Create it */
      if (FALSE == ds_3gpp_rpm_create_efs_throttle_param_file())
      {
        DS_3GPP_MSG0_ERROR("Failed to create efs throttle param file");
        return;
      }
    }/* rpm file not present */
	else
	{
	  ds_3gpp_rpm_verify_efs_throttle_param_file();
	}/* rpm file present */
  }/* rpm directory present */
  else
  {
    if( mcfg_mkdir( DS_3GPP_RPM_THROTTLE_PARAM_DIR_NAME,
                    S_IREAD|S_IWRITE|S_IEXEC,
                    MCFG_FS_TYPE_EFS ) == -1 )
    {
      DATA_MSG1_ERROR( "rpm: fs_mkdir failed errno = %d",
                       mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return;
    }

    /* Create the RPM throttle param file */
    if (FALSE == ds_3gpp_rpm_create_efs_throttle_param_file())
    {
      DS_3GPP_MSG0_ERROR("Failed to create efs throttle param file");
      return;
    }
  }/* rpm directory not present */
} /* ds_3gpp_rpm_init */

/*===========================================================================
  FUNCTION ds_3gpp_rpm_read_efs_per_subs

  DESCRIPTION
  This function reads the rpm related efs items per subscription
  
  PARAMETERS  
  subs_id -subscription_id 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_rpm_read_efs_per_subs(sys_modem_as_id_e_type  subs_id)
{
  
  char item_file_path_rpm_params_struct[]
                            = "/nv/item_files/data/3gpp/rpm_params";
  char item_file_path_rpm_supported_sim_struct[]
                            = "/nv/item_files/data/3gpp/rpm_suppported_sim";
  nv_stat_enum_type    return_status;

  return_status = ds3gcfgmgr_read_efs_nv_ex(item_file_path_rpm_params_struct,
                                 (void *)(&ds_3gpp_rpm_params[subs_id]), 
                                 sizeof(ds_3gpp_rpm_params_type),
                                 subs_id);

  if (return_status != NV_DONE_S)
  {
    DS_3GPP_MSG0_HIGH("RPM parameter file not present in EFS");
  }

  /* Read the RPM supported SIMs from EFS */

  return_status = ds3gcfgmgr_read_efs_nv_ex(item_file_path_rpm_supported_sim_struct,
                                 (void *)(&ds_3gpp_rpm_supported_sim[subs_id]), 
                                 (DS_3GPP_RPM_MAX_SIM_SUPPORTED * 
                                 sizeof(ds_3gpp_rpm_sim_info_s_type)),
                                 subs_id);

  if (return_status != NV_DONE_S)
  {
    DS_3GPP_MSG0_HIGH("RPM parameter file not present in EFS");
  }

  return;
} /* ds_3gpp_rpm_read_efs_per_subs */


/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_BACKOFF_TIMER_VAL

DESCRIPTION
  This function calculates the backoff timer val for the APN as per the
  last throttle cause.

PARAMETERS
  *throttle_param_ptr - ptr to the throttle param table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_rpm_get_backoff_timer_val
(
  ds_3gpp_rpm_throttle_param_type    *throttle_param_ptr,
  sys_modem_as_id_e_type             subs_id
)
{ 
  uint32   timer_val; /* In seconds */
  uint8    fail_cnt;
  ds_3gpp_rpm_cause_code_enum_type   throttle_cause;
  uint8    nbackoff;

  throttle_cause = throttle_param_ptr->last_throttle_cause;

  if (throttle_cause <= DS_3GPP_RPM_CAUSE_CODE_MIN || 
      throttle_cause > DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Invalid throttle_cause: %d", throttle_cause);
    return;
  }

  DS_3GPP_MSG3_HIGH("Calculating backoff timer val: F[%d]=%d",
                     throttle_cause, 
                     ds_3gpp_rpm_params[subs_id].f_cnt[throttle_cause - 1],
                     0);  

  nbackoff   = (ds_3gpp_rpm_params[subs_id].f_cnt[throttle_cause - 1] - 1);
  fail_cnt   = throttle_param_ptr->counter_val[throttle_cause - 1];

  /*------------------------------------------------------------------------
    Calculate the backoff timer value as per the RPM algorithm
  ------------------------------------------------------------------------*/
  if (fail_cnt < nbackoff)
  {
    timer_val = (uint32)((2*fail_cnt*SEC_PER_HOUR)/(nbackoff*(nbackoff + 1)));
  }
  else
  {
    timer_val = (uint32)((2*SEC_PER_HOUR)/(nbackoff + 1));
  }

  throttle_param_ptr->backoff_timer_val = (uint32)(SEC_TO_MS(timer_val));
} /* ds_3gpp_rpm_get_backoff_timer_val */

/*===========================================================================
FUNCTION DS_3GPP_IS_RPM_ENABLED

DESCRIPTION
  This function returns whether RPM is enabled. This is decided on the basis
  of two flags viz. the RPM enabled flag in RPM params and RPM is_initialized
  flag.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if RPM is enabled.
  FALSE - if RPM is disabled.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_is_rpm_enabled
(
  sys_modem_as_id_e_type subs_id
)
{
  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return FALSE;
  }
	
  return ((ds_3gpp_rpm_params[subs_id].is_rpm_enabled) && 
  	       ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_is_initialized);
} /* ds_3gpp_is_rpm_enabled */

/*===========================================================================
FUNCTION DS_3GPP_RPM_PRINT_PLMN_ID

DESCRIPTION
  This function prints the plmn_id in MCC-MNC format.

PARAMETERS
  *current_plmn - ptr to the plmn_id to be printed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_print_plmn_id
(
  void *current_plmn
)
{
  uint8                       mcc[3] = {0};
  uint8                       mnc[3] = {0};
  sys_plmn_id_s_type          *current_plmn_id;

  current_plmn_id = (sys_plmn_id_s_type *)current_plmn;
  
  mcc[0] = (current_plmn_id->identity[0] & 0x0F);
  mcc[1] = ((current_plmn_id->identity[0] & 0xF0) >> 4);
  mcc[2] = (current_plmn_id->identity[1] & 0x0F);

  mnc[0] = (current_plmn_id->identity[2] & 0x0F);
  mnc[1] = ((current_plmn_id->identity[2] & 0xF0) >> 4);
  mnc[2] = ((current_plmn_id->identity[1] & 0xF0) >> 4);

  DS_3GPP_MSG3_HIGH("PLMN ID:", 0, 0, 0);
  DS_3GPP_MSG3_HIGH("MCC: %d%d%d", mcc[0], mcc[1], mcc[2]);
  DS_3GPP_MSG3_HIGH("MNC: %d%d%d", mnc[0], mnc[1], mnc[2]);
} /* ds_3gpp_rpm_print_plmn_id */

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_EFS_THROTTLE_TBL_PTR_FROM_PLMN

DESCRIPTION
  This function retrieves the throttle_tbl_ptr for the specified plmn_id.

PARAMETERS
  plmn_id - plmn_id for which the throttle tbl ptr is to be returned.
  *index  - index of the plmn_id in the efs throttle tbl.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to efs throttle table.

SIDE EFFECTS
  None.
===========================================================================*/
ds_3gpp_rpm_efs_throttle_tbl_type 
  *ds_3gpp_rpm_get_efs_throttle_tbl_ptr_from_plmn
(
  sys_plmn_id_s_type     plmn_id,
  int8                   *index,
  sys_modem_as_id_e_type subs_id
)
{
  int8                           plmn_index;
  ds_3gpp_rpm_efs_throttle_tbl_type  *plmn_tbl_ptr = NULL;

  if (FALSE == ds_3gpp_throt_sm_is_plmn_non_null(plmn_id))
  {
    DS_3GPP_MSG0_ERROR("plmn_id passed is NULL");
    return NULL;
  }

  for (plmn_index = (DS_3GPP_MAX_PLMN_SUPPORTED - 1); 
       plmn_index >= 0; plmn_index--)
  {
    plmn_tbl_ptr = ds_3gpp_rpm_efs_throttle_tbl[subs_id][plmn_index];

    if (NULL != plmn_tbl_ptr)
    {
      if ( memcmp(&plmn_id,
                  &(plmn_tbl_ptr->ds_plmn_id),
                  sizeof(sys_plmn_id_s_type)) == 0 )
      {
        DS_3GPP_MSG1_HIGH("PLMN match found at index %d", plmn_index);
        *index = plmn_index;
        break;
      }
    }
  }

  return plmn_tbl_ptr;
} /* ds_3gpp_rpm_get_efs_throttle_tbl_ptr_from_plmn */

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_EFS_THROTTLE_TBL_PTR

DESCRIPTION
  This function retrieves the efs throttle tbl ptr for the current plmn_id.

PARAMETERS
  allow_create - whether a new block for the current plmn_id has to be
                 allocated.
  *index       - index of the plmn_id in the efs throttle tbl.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to efs throttle table.

SIDE EFFECTS
  None.
===========================================================================*/
ds_3gpp_rpm_efs_throttle_tbl_type *ds_3gpp_rpm_get_efs_throttle_tbl_ptr
(
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type subs_id
)
{
  int8                        plmn_index;
  uint8                       first_free_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type          current_plmn_id;
  sys_plmn_id_s_type          plmn_id_in_table;
  ds_3gpp_rpm_efs_throttle_tbl_type  *plmn_tbl_ptr = NULL;

  DS_3GPP_MSG0_HIGH("ds_3gpp_rpm_get_efs_throttle_tbl_ptr");

  memset(&current_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  memset(&plmn_id_in_table, 0, sizeof(sys_plmn_id_s_type));

  ds3g_get_current_plmn_id(&current_plmn_id, subs_id);

  DS_3GPP_MSG0_HIGH("Current PLMN ID is:");
  ds_3gpp_rpm_print_plmn_id((void *)&current_plmn_id);

  if (FALSE == ds_3gpp_throt_sm_is_plmn_non_null(current_plmn_id))
  {
    DS_3GPP_MSG0_ERROR("Current PLMN Id is zero");
    return plmn_tbl_ptr;
  }

  for (plmn_index = (DS_3GPP_MAX_PLMN_SUPPORTED - 1); 
       plmn_index >= 0; plmn_index--)
  {
    plmn_tbl_ptr = ds_3gpp_rpm_efs_throttle_tbl[subs_id][plmn_index];

    if (NULL == plmn_tbl_ptr)
    {
      DS_3GPP_MSG1_MED("RPM efs throttle table index: [%d] is empty", 
                        plmn_index);
      first_free_index = plmn_index;
      continue;
    }
    else
    {
      if ( memcmp(&current_plmn_id,
                  &(plmn_tbl_ptr->ds_plmn_id),
                  sizeof(sys_plmn_id_s_type)) == 0 )
      {
        first_free_index = plmn_index;
        break;
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Current PLMN ID doesn't match with plmn_index: %d"
                           " PLMN in table is: ",
                            plmn_index);
        ds_3gpp_rpm_print_plmn_id((void *)&(plmn_tbl_ptr->ds_plmn_id));
      }
    }
  }

  if (first_free_index == DS_3GPP_MAX_PLMN_SUPPORTED)
  {
    DS_3GPP_MSG0_ERROR("PLMN table full");
    return NULL;
  }
  else if(first_free_index == plmn_index)
  {
    DS_3GPP_MSG1_HIGH("PLMN match found at index: %d", plmn_index);
    *index = plmn_index;
    return plmn_tbl_ptr;
  }
  else
  {
    if (TRUE == allow_create)
    {
      DS_3GPP_MSG1_HIGH("PLMN not found . Allocate a new entry for it at index %d",
                         first_free_index);
      plmn_tbl_ptr = ds_3gpp_rpm_efs_throttle_tbl[subs_id][first_free_index];
  
      plmn_tbl_ptr = (ds_3gpp_rpm_efs_throttle_tbl_type *)
                     modem_mem_alloc(sizeof(ds_3gpp_rpm_efs_throttle_tbl_type), 
                                     MODEM_MEM_CLIENT_DATA);
  
      if (NULL == plmn_tbl_ptr)
      {
        DATA_ERR_FATAL("Could not allocate memory for PLMN table");
      }
      else
      {
        DS_3GPP_MSG0_HIGH("Allocated entry for current PLMN in efs throttle table");
        memset(plmn_tbl_ptr, 0, sizeof(ds_3gpp_rpm_efs_throttle_tbl_type));

        ds_3gpp_rpm_efs_throttle_tbl[subs_id][first_free_index] = plmn_tbl_ptr;
        /* Store the PLMN ID after creating the new entry */
        memscpy(&(plmn_tbl_ptr->ds_plmn_id),
                sizeof(plmn_tbl_ptr->ds_plmn_id),
                &current_plmn_id,
               sizeof(sys_plmn_id_s_type));
        *index = first_free_index;
        return (ds_3gpp_rpm_efs_throttle_tbl[subs_id][first_free_index]);
      }
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Entry for PLMN is not present");
      return NULL;
    }
  }

  return plmn_tbl_ptr;
} /* ds_3gpp_rpm_get_efs_throttle_tbl_ptr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_THROTTLE_TBL_PTR_FROM_PLMN

DESCRIPTION
  This function retrieves the throttle tbl ptr for the specified plmn_id.

PARAMETERS
  plmn_id - plmn_id for which throttle tbl is to be returned.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to the throttle table.

SIDE EFFECTS
  None.
===========================================================================*/
ds_3gpp_rpm_throttle_tbl_type *ds_3gpp_rpm_get_throttle_tbl_ptr_from_plmn
(
  sys_plmn_id_s_type     plmn_id,
  sys_modem_as_id_e_type subs_id
)
{
  int8                          plmn_index;
  ds_3gpp_rpm_throttle_tbl_type  *plmn_tbl_ptr = NULL;

  if (FALSE == ds_3gpp_throt_sm_is_plmn_non_null(plmn_id))
  {
    DS_3GPP_MSG0_ERROR("plmn_id passed is NULL");
    return NULL;
  }

  for (plmn_index = (DS_3GPP_MAX_PLMN_SUPPORTED - 1); 
       plmn_index >= 0; plmn_index--)
  {
    plmn_tbl_ptr = ds_3gpp_rpm_throttle_tbl[subs_id][plmn_index];

    if (NULL != plmn_tbl_ptr)
    {
      if ( memcmp(&plmn_id,
                  &(plmn_tbl_ptr->plmn_id),
                  sizeof(sys_plmn_id_s_type)) == 0 )
      {
        DS_3GPP_MSG1_HIGH("PLMN match found at index %d", plmn_index);
        break;
      }
    }
  }

  return plmn_tbl_ptr;
} /* ds_3gpp_rpm_get_throttle_tbl_ptr_from_plmn */

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_EFS_THROTTLE_TBL_PTR

DESCRIPTION
  This function retrieves the efs throttle tbl ptr for the current plmn_id.

PARAMETERS
  allow_create - whether a new block for the current plmn_id has to be
                 allocated.
  *index       - index of the plmn_id in the efs throttle tbl.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to throttle table.

SIDE EFFECTS
  None.
===========================================================================*/
ds_3gpp_rpm_throttle_tbl_type *ds_3gpp_rpm_get_throttle_tbl_ptr
(
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type subs_id
)
{
  int8                            plmn_index;
  uint8                           first_free_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type              current_plmn_id;
  sys_plmn_id_s_type              plmn_id_in_table;
  ds_3gpp_rpm_throttle_tbl_type   *plmn_tbl_ptr = NULL;

  DS_3GPP_MSG0_HIGH("ds_3gpp_rpm_get_throttle_tbl_ptr");

  memset(&current_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  memset(&plmn_id_in_table, 0, sizeof(sys_plmn_id_s_type));

  ds3g_get_current_plmn_id(&current_plmn_id, subs_id);

  DS_3GPP_MSG0_HIGH("Current PLMN ID is:");
  ds_3gpp_rpm_print_plmn_id((void *)&current_plmn_id);

  if (FALSE == ds_3gpp_throt_sm_is_plmn_non_null(current_plmn_id))
  {
    DS_3GPP_MSG0_ERROR("Current PLMN Id is zero");
    return plmn_tbl_ptr;
  }

  for (plmn_index = (DS_3GPP_MAX_PLMN_SUPPORTED - 1); 
       plmn_index >= 0; plmn_index--)
  {
    plmn_tbl_ptr = ds_3gpp_rpm_throttle_tbl[subs_id][plmn_index];

    if (NULL == plmn_tbl_ptr)
    {
      DS_3GPP_MSG1_MED("RPM PLMN table index: [%d] is empty", 
                        plmn_index);
      first_free_index = plmn_index;
      continue;
    }
    else
    {
      if ( memcmp(&current_plmn_id,
                  &(plmn_tbl_ptr->plmn_id),
                  sizeof(sys_plmn_id_s_type)) == 0 )
      {
        first_free_index = plmn_index;
        break;
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Current PLMN ID doesn't match with plmn_index: %d. PLMN in table is: ",
                            plmn_index);
        ds_3gpp_rpm_print_plmn_id((void *)&(plmn_tbl_ptr->plmn_id));
      }
    }
  }

  if (first_free_index == DS_3GPP_MAX_PLMN_SUPPORTED)
  {
    DS_3GPP_MSG0_ERROR("PLMN table full");
    return NULL;
  }
  else if(first_free_index == plmn_index)
  {
    DS_3GPP_MSG1_HIGH("PLMN match found, index: %d", plmn_index);
    *index = plmn_index;
    return plmn_tbl_ptr;
  }
  else
  {
    if (TRUE == allow_create)
    {
      DS_3GPP_MSG1_HIGH("PLMN not found . Allocate a new entry for it at index %d",
                         first_free_index);

      plmn_tbl_ptr = ds_3gpp_rpm_throttle_tbl[subs_id][first_free_index];
  
      plmn_tbl_ptr = (ds_3gpp_rpm_throttle_tbl_type *)
                          modem_mem_alloc(sizeof(ds_3gpp_rpm_throttle_tbl_type), 
                                          MODEM_MEM_CLIENT_DATA);
  
      if (NULL == plmn_tbl_ptr)
      {
        DATA_ERR_FATAL("Could not allocate memory for RPM failure param");
      }
      else
      {
        DS_3GPP_MSG0_HIGH("Allocated entry for current PLMN");
        memset(plmn_tbl_ptr, 0, sizeof(ds_3gpp_rpm_throttle_tbl_type));
        ds_3gpp_rpm_throttle_tbl[subs_id][first_free_index] = plmn_tbl_ptr;
        /* Store the PLMN ID after creating the new entry */
        memscpy(&(plmn_tbl_ptr->plmn_id),
                sizeof(sys_plmn_id_s_type),
                &current_plmn_id,
               sizeof(sys_plmn_id_s_type));
        *index = first_free_index;
        return (ds_3gpp_rpm_throttle_tbl[subs_id][first_free_index]);
      }
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Entry for PLMN is not present");
      return NULL;
    }
  }

  return plmn_tbl_ptr;
} /* ds_3gpp_rpm_get_throttle_tbl_ptr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_EFS_THROTTLE_PARAM_PTR

DESCRIPTION
  This function retrieves the efs throttle param ptr for the specified apn
  for the current plmn_id.

PARAMETERS
  apn          - apn for which the throttle param ptr is to be returned.
  allow_create - whether a new entry ahs to be created for this APN.
  *index       - index of the apn in the efs throttle param tbl.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to efs throttle param table.

SIDE EFFECTS
  None.
===========================================================================*/
ds_3gpp_rpm_efs_throttle_param_type *ds_3gpp_rpm_get_efs_throttle_param_ptr
(
  byte     *apn,
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type  subs_id 
)
{
  int8                             apn_index;
  int8                             plmn_index;
  uint8                            first_free_index = DS_MAX_APN_NUM;
  ds_3gpp_rpm_efs_throttle_tbl_type          *plmn_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type  *apn_tbl_ptr = NULL;
  

  if (index == NULL)
  {
    DS_3GPP_MSG0_ERROR("Index is NULL");
    return apn_tbl_ptr;
  }

  plmn_tbl_ptr = ds_3gpp_rpm_get_efs_throttle_tbl_ptr(allow_create, 
                                                      &plmn_index,
                                                      subs_id);

  if (NULL == plmn_tbl_ptr)
  {
    return NULL;
  }

  for (apn_index = (DS_MAX_APN_NUM - 1); apn_index >= 0; apn_index--)
  {
    apn_tbl_ptr = plmn_tbl_ptr->efs_throttle_param[apn_index];

    if (NULL == apn_tbl_ptr)
    {
      DS_3GPP_MSG1_MED("RPM efs throttle param table index: [%d] is empty", 
                        apn_index);
      first_free_index = apn_index;
      continue;
    }
    else
    {
      if (NULL != apn_tbl_ptr->apn)
      {
        if ( strcasecmp((char *)apn, (char *)(apn_tbl_ptr->apn)) == 0 )
        {
          first_free_index = apn_index;
          break;
        }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("NULL APN stored in efs throttle param table: index: %d",
                            apn_index);
        return NULL;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Check if the APN match is found or we need to allocate new rpm failure
    param table for this APN 
  --------------------------------------------------------------------------*/
  if (first_free_index == DS_MAX_APN_NUM)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                         "RPM failure param table full. Can't throttle APN: %s",
                         apn);
    return NULL;
  }
  else if(first_free_index == apn_index)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
               "APN match found in RPM failure param table index: [%d] apn: %s",
                apn_index, apn); 
    *index = apn_index;
    return apn_tbl_ptr;
  }
  else
  {
    if (TRUE == allow_create)
    {
      /*-----------------------------------------------------------------------
        APN not found in RPM failure param table. Allocate a new entry for it
      -----------------------------------------------------------------------*/

      apn_tbl_ptr = plmn_tbl_ptr->efs_throttle_param[first_free_index];
  
      apn_tbl_ptr = (ds_3gpp_rpm_efs_throttle_param_type *)
                       modem_mem_alloc(DS_3GPP_RPM_APN_BLK_SIZE, 
                                       MODEM_MEM_CLIENT_DATA);
  
      if (NULL == apn_tbl_ptr)
      {
        DATA_ERR_FATAL("Could not allocate memory for RPM failure param");
      }
      else
      {
        memset(apn_tbl_ptr, 0, sizeof(ds_3gpp_rpm_efs_throttle_param_type));
        *index = apn_index;
        DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
          "Allocated memory for APN: %s in efs throttle param table, index :%d",
          apn, first_free_index); 
        plmn_tbl_ptr->efs_throttle_param[first_free_index] = apn_tbl_ptr;

        /* Update the APN in the table */
        memscpy((apn_tbl_ptr->apn),
                (DS_UMTS_MAX_APN_STRING_LEN + 1),
		 apn, (DS_UMTS_MAX_APN_STRING_LEN + 1));
        return apn_tbl_ptr;
      }
    }
    else
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                              "APN %s not found in APN table",apn);
      return NULL;
    }
  }

  return apn_tbl_ptr;
} /* ds_3gpp_rpm_get_efs_throttle_param_ptr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_THROTTLE_PARAM_PTR

DESCRIPTION
  This function retrieves the throttle param ptr for the specified apn
  for the current plmn_id.

PARAMETERS
  apn          - apn for which the throttle param ptr is to be returned.
  allow_create - whether a new entry ahs to be created for this APN.
  *index       - index of the apn in the throttle param tbl.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to throttle param table.

SIDE EFFECTS
  None.
===========================================================================*/
ds_3gpp_rpm_throttle_param_type  *ds_3gpp_rpm_get_throttle_param_ptr
(
  byte     *apn,
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type subs_id
)
{
  int8                                apn_index;
  int8                                plmn_index;
  uint8                               first_free_index = DS_MAX_APN_NUM;
  ds_3gpp_rpm_throttle_tbl_type       *plmn_tbl_ptr;
  ds_3gpp_rpm_throttle_param_type    *apn_tbl_ptr = NULL;
  

  if (index == NULL)
  {
    DS_3GPP_MSG0_ERROR("Index is NULL");
    return apn_tbl_ptr;
  }

  plmn_tbl_ptr = ds_3gpp_rpm_get_throttle_tbl_ptr(allow_create, &plmn_index, subs_id);

  if (NULL == plmn_tbl_ptr)
  {
    return NULL;
  }

  for (apn_index = (DS_MAX_APN_NUM - 1); apn_index >= 0; apn_index--)
  {
    apn_tbl_ptr = plmn_tbl_ptr->throttle_param[apn_index];

    if (NULL == apn_tbl_ptr)
    {
      DS_3GPP_MSG1_MED("RPM APN table index: [%d] is empty", 
                        apn_index);
      first_free_index = apn_index;
      continue;
    }
    else
    {
      if (NULL != apn_tbl_ptr->apn)
      {
        if ( strcasecmp((char *)apn, (char *)(apn_tbl_ptr->apn)) == 0 )
        {
          first_free_index = apn_index;
          break;
        }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("NULL APN stored in APN table: index: %d",
                            apn_index);
        return NULL;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Check if the APN match is found or we need to allocate new rpm failure
    param table for this APN 
  --------------------------------------------------------------------------*/
  if (first_free_index == DS_MAX_APN_NUM)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                    "RPM failure param table full. Can't throttle APN: %s",apn);
    return NULL;
  }
  else if(first_free_index == apn_index)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                            "APN match found in RPM failure param table."
                            "index: [%d] apn: %s", apn_index, apn);
    *index = apn_index;
    return apn_tbl_ptr;
  }
  else
  {
    if (TRUE == allow_create)
    {
      /*-----------------------------------------------------------------------
        APN not found in RPM failure param table. Allocate a new entry for it
      -----------------------------------------------------------------------*/

      apn_tbl_ptr = plmn_tbl_ptr->throttle_param[first_free_index];
  
      apn_tbl_ptr = (ds_3gpp_rpm_throttle_param_type *)
                          modem_mem_alloc(sizeof(ds_3gpp_rpm_throttle_param_type), 
                                          MODEM_MEM_CLIENT_DATA);
  
      if (NULL == apn_tbl_ptr)
      {
        DATA_ERR_FATAL("Could not allocate memory for RPM failure param");
      }
      else
      {
        memset(apn_tbl_ptr, 0, sizeof(ds_3gpp_rpm_throttle_param_type));
        *index = apn_index;
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                              "Allocated memory for APN: %s in APN table,",apn);
        plmn_tbl_ptr->throttle_param[first_free_index] = apn_tbl_ptr;

        /* Update the APN in the APn table */
        memscpy((apn_tbl_ptr->apn), 
                 (DS_UMTS_MAX_APN_STRING_LEN + 1),
		  apn, (DS_UMTS_MAX_APN_STRING_LEN + 1));

        /* Define the hourly timer */
        rex_def_timer_ex(&(apn_tbl_ptr->tmr_hourly),
                           (rex_timer_cb_type)ds_3gpp_rpm_hourly_tmr_cb,
                           (unsigned long)apn_tbl_ptr);
        return apn_tbl_ptr;
      }
    }
    else
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                              "APN %s not found in APN table",apn);
      return NULL;
    }
  }

  return apn_tbl_ptr;
}/* ds_3gpp_rpm_get_throttle_param_ptr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_HANDLE_PDP_ACT_ACCEPT

DESCRIPTION
  This function handles the PDP activation accept. Increments success_cnt
  in throttle table for this APN and after max success cnt is reached
  backoff timer is started.

PARAMETERS
  *apn - apn for which PDP_ACT_ACCEPT is received.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_handle_pdp_act_accept
(
  byte   *apn,
  sys_modem_as_id_e_type subs_id
)
{
  uint8                                    success_cnt = 0;
  int8                                     apn_index;
  uint32                                   current_time;
  uint32                                   time_elapsed;
  ds_3gpp_rpm_throttle_param_type          *apn_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type   *efs_throttle_param_ptr;
  ds_3gpp_pdn_throttle_sm_s                *pdn_throt_sm_ptr;
  sys_plmn_id_s_type                       zero_plmn_id;
  memset(&zero_plmn_id, 0, sizeof(zero_plmn_id));

  if (NULL == apn)
  {
    DS_3GPP_MSG0_ERROR("NULL APN is not supported");
    return FALSE;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
	  return FALSE;
  }

  DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                          "ds_3gpp_rpm_handle_pdp_act_accept for APN: %s",apn);

  apn_tbl_ptr = ds_3gpp_rpm_get_throttle_param_ptr(
                                                   apn,
                                                   FALSE,
                                                   &apn_index,
                                                   subs_id
                                                  );

  if (NULL != apn_tbl_ptr)
  {
    if (DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS != apn_tbl_ptr->last_throttle_cause)
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                              "Entry for APN: %s exists in param tbl "
                              "clear the throttle timers",apn);
      ds_3gpp_rpm_clear_throttle_timer(apn, zero_plmn_id, subs_id);
      if (FALSE == ds_3gpp_rpm_write_throttle_params(apn, subs_id))
      {
        DS_3GPP_MSG0_ERROR("Could not write throttle params to EFS");
      }

      apn_tbl_ptr->counter_val[DS_3GPP_RPM_CAUSE_CODE_IGNORE - 1]    = 0;
      apn_tbl_ptr->counter_val[DS_3GPP_RPM_CAUSE_CODE_PERMANENT - 1] = 0;
      apn_tbl_ptr->counter_val[DS_3GPP_RPM_CAUSE_CODE_TEMPORARY - 1] = 0;

      if (TRUE == apn_tbl_ptr->is_hourly_tmr_running )
      {
        DS_3GPP_RPM_CLR_TIMER(&(apn_tbl_ptr->tmr_hourly), 
                              apn_tbl_ptr->is_hourly_tmr_running)
      }
    }
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Entry for the APN: %s doesn't exist in param tbl",
                      apn);

    /* Create new entry for this APN */
    apn_tbl_ptr = ds_3gpp_rpm_get_throttle_param_ptr(
                                                     apn,
                                                     TRUE,
                                                     &apn_index,
                                                     subs_id
                                                    );

    if (NULL == apn_tbl_ptr)
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                              "Unable to allocate new entry for APN: %s",apn);
      return FALSE;
    }
    else
    {
      /* New APN block allocated. Reset the counters */
      memset((apn_tbl_ptr->counter_val), 0, 
             (DS_3GPP_RPM_MAX_CAUSE_CODES * sizeof(uint8)));
    }
  }

  /* PDP Success handling */
  efs_throttle_param_ptr = ds_3gpp_rpm_get_efs_throttle_param_ptr(
                                                              apn,
                                                              TRUE,
                                                              &apn_index,
                                                              subs_id
                                                             );

  if(NULL == efs_throttle_param_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "Unable to allocate new entry in efs throttle tbl for" 
                            "APN: %s",apn);
    return FALSE;
  }

  /* Update the success params */
  apn_tbl_ptr->counter_val[DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS - 1]++;

  success_cnt = 
    apn_tbl_ptr->counter_val[DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS - 1];
  apn_tbl_ptr->last_throttle_cause = DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS;

  memscpy(apn_tbl_ptr->apn, (DS_UMTS_MAX_APN_STRING_LEN + 1),
  	  apn, (DS_UMTS_MAX_APN_STRING_LEN + 1));

  efs_throttle_param_ptr->is_valid_block = TRUE;
  efs_throttle_param_ptr->last_throttle_cause 
                                         = DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS;
  efs_throttle_param_ptr->counter_val    = success_cnt;

  memscpy(efs_throttle_param_ptr->apn, (DS_UMTS_MAX_APN_STRING_LEN + 1),
  	  apn, (DS_UMTS_MAX_APN_STRING_LEN + 1));

  DS_3GPP_MSG1_HIGH("Updating success_cnt : %d", success_cnt);

  if (success_cnt == 1)
  {
    ds_3gpp_rpm_start_hourly_timer(apn, SEC_TO_MS(SEC_PER_HOUR), subs_id);
  }

  if (success_cnt ==
      ds_3gpp_rpm_params[subs_id].f_cnt[DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS - 1])
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                            "Success count reached max success: %d for APN: %s",
                            success_cnt, apn);

    current_time = time_get_secs();

    DS_3GPP_MSG1_HIGH("Current time is %d", current_time);
    time_elapsed = (current_time - efs_throttle_param_ptr->hourly_tmr_timestamp);

    if ( time_elapsed < SEC_PER_HOUR)
    {
      DS_3GPP_MSG3_HIGH("Elapsed time since hourly timer start is: %d sec",
                         time_elapsed, 0, 0);
      /* Elapsed time is less than an hour throttle this APN */
      ds3gpp_enter_global_crit_section();
      pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm(apn,FALSE,
                                 TRUE, DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, 
                                   subs_id);

      apn_tbl_ptr->backoff_timer_val 
                        = (uint32)(1000*(SEC_PER_HOUR - time_elapsed));

      if (NULL == pdn_throt_sm_ptr)
      {
        DS_3GPP_MSG0_ERROR("Could not allocate a throt_sm_ptr");
        ds3gpp_leave_global_crit_section();
        return FALSE;
      }
      else
      {
        ds_3gppi_throt_sm_feed_failure_to_core_sm(
                                       pdn_throt_sm_ptr->v4_throttle_sm,
                                       NULL, subs_id,NULL);
      }

      ds3gpp_leave_global_crit_section();
    }
  }

  if (FALSE == ds_3gpp_rpm_write_throttle_params(apn, subs_id))
  {
    DS_3GPP_MSG0_ERROR("Could not write RPM params to EFS");
    return FALSE;
  }

  return TRUE;
} /* ds_3gpp_rpm_handle_pdp_act_accept */

/*===========================================================================
FUNCTION DS_3GPP_RPM_HANDLE_PDP_ACT_REJECT

DESCRIPTION
  This function handles the PDP activation reject. Start backoff timer for
  this APN if the reject cause is one of the following :
  1. PDP activation ignore cause code.
  2. Permanent failure cause code.
  3. Temporary failure cause code.

PARAMETERS
  *apn       - apn for which PDP_ACT_REJECT is received.
  cause_code - PDP activation reject cause code.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_handle_pdp_act_reject
(
  byte                            *apn,
  ps_iface_net_down_reason_type   cause_code,
  sys_modem_as_id_e_type          subs_id
)
{
  int8                                       apn_index;
  int8                                       efs_apn_index;
  ds_3gpp_rpm_cause_code_enum_type           throttle_cause;
  ds_3gpp_rpm_throttle_param_type            *throttle_param_ptr;
  ds_3gpp_rpm_efs_throttle_param_type        *efs_throttle_param_ptr;

  if (NULL == apn)
  {
    DS_3GPP_MSG0_ERROR("NULL APN is not supported");
    return FALSE;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
	  return FALSE;
  }

  switch(cause_code)
  {
    case PS_NET_DOWN_REASON_PDP_ACTIVATE_MAX_RETRY_FAILED:
      throttle_cause = DS_3GPP_RPM_CAUSE_CODE_IGNORE;
      break;

    case PS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING:
    case PS_NET_DOWN_REASON_UNKNOWN_APN:
    case PS_NET_DOWN_REASON_UNKNOWN_PDP:
    case PS_NET_DOWN_REASON_AUTH_FAILED:
    case PS_NET_DOWN_REASON_GGSN_REJECT:
    case PS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED:
    case PS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED:
      throttle_cause = DS_3GPP_RPM_CAUSE_CODE_PERMANENT;
      break;

    case PS_NET_DOWN_REASON_LLC_SNDCP_FAILURE:
    case PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES:
    case PS_NET_DOWN_REASON_ACTIVATION_REJECT:
    case PS_NET_DOWN_REASON_OPTION_TEMP_OOO:
    case PS_NET_DOWN_REASON_NSAPI_ALREADY_USED:
    case PS_NET_DOWN_REASON_NETWORK_FAILURE:
    case PS_NET_DOWN_REASON_PROTOCOL_ERROR:
      throttle_cause = DS_3GPP_RPM_CAUSE_CODE_TEMPORARY;
      break;

    default:
      DS_3GPP_MSG1_HIGH("unhandled cause code: 0x%x", cause_code);
      return FALSE;
  }

  throttle_param_ptr =   ds_3gpp_rpm_get_throttle_param_ptr(apn,
                                                            TRUE,
                                                            &apn_index,
                                                            subs_id);

  if(NULL == throttle_param_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "APN match not found and unable to allocate memory"
                            "for requested APN: %s",apn);
    return FALSE;
  }
  else
  {
    throttle_param_ptr->last_throttle_cause = throttle_cause;
	throttle_param_ptr->subs_id = subs_id;
    throttle_param_ptr->counter_val[throttle_cause - 1]++;
    ds_3gpp_rpm_get_backoff_timer_val(throttle_param_ptr, subs_id);

    /* Allocate memory for efs throttle param if the entry for this APN doesn't
       exist already */
    efs_throttle_param_ptr =  ds_3gpp_rpm_get_efs_throttle_param_ptr(
                                                                 apn,
                                                                 TRUE,
                                                                 &efs_apn_index,
                                                                 subs_id
                                                                );

    if (NULL == efs_throttle_param_ptr)
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "APN match not found and unable to allocate memory"
                            "for requested APN: %s",apn);
    }
    else
    {
      efs_throttle_param_ptr->is_valid_block      = TRUE;
      efs_throttle_param_ptr->last_throttle_cause = throttle_cause;
      efs_throttle_param_ptr->counter_val         = 
               throttle_param_ptr->counter_val[throttle_cause - 1];

      /* If hourly timer is already running, then timestamp is already updated */
      if (throttle_param_ptr->is_hourly_tmr_running == FALSE)
      {
        efs_throttle_param_ptr->hourly_tmr_timestamp = 0;
      }      
      efs_throttle_param_ptr->backoff_tmr_timestamp = 0;
  
      memscpy(efs_throttle_param_ptr->apn, 
	      (DS_UMTS_MAX_APN_STRING_LEN + 1),
              apn, 
              (DS_UMTS_MAX_APN_STRING_LEN + 1));
    }
  }

  return TRUE;
} /* ds_3gpp_rpm_handle_pdp_act_reject */

/*===========================================================================
FUNCTION DS_3GPP_RPM_WRITE_BACKOFF_TMR_TIMESTAMP

DESCRIPTION
  This function updates the backoff timer timestamp for the scpecified apn.

PARAMETERS
  *apn       - apn for which backoff timer timestamp is to be written.
  timestamp  - current timestamp.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_write_backoff_tmr_timestamp
(
  byte    *apn,
  uint32  timestamp,
  sys_modem_as_id_e_type subs_id
)
{
  int8                                   apn_index;
  ds_3gpp_rpm_efs_throttle_param_type *efs_throttle_param_ptr;

  efs_throttle_param_ptr = ds_3gpp_rpm_get_efs_throttle_param_ptr(
                                                              apn, 
                                                              FALSE, 
                                                              &apn_index,
                                                              subs_id);

  if (NULL == efs_throttle_param_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "APN : %s has no entry in the efs throttle tbl",apn);
    return;
  }
  else
  {
    efs_throttle_param_ptr->backoff_tmr_timestamp = timestamp;
  }
} /* ds_3gpp_rpm_write_backoff_tmr_timestamp */

/*===========================================================================
FUNCTION DS_3GPP_RPM_BACKOFF_TMR_EXP_HDLR

DESCRIPTION
  This function handles the backoff timer expiry for a specific APN.

PARAMETERS
  apn_index - apn_index in efs throttle tbl for which backoff timer has
              expired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_backoff_tmr_exp_hdlr
(
  uint8                  apn_index,
  sys_modem_as_id_e_type subs_id
)
{
  int8                                   plmn_index;
  ds_3gpp_rpm_efs_throttle_tbl_type         *efs_throttle_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type *efs_throttle_param_ptr;
  boolean                                is_attach_apn = FALSE;

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if (apn_index >= DS_MAX_APN_NUM)
  {
    DS_3GPP_MSG1_ERROR("ds_3gpp_rpm_write_backoff_tmr_timestamp: apn_index : %d",
                        apn_index);
    return;
  }

  efs_throttle_tbl_ptr = ds_3gpp_rpm_get_efs_throttle_tbl_ptr(
                                                             FALSE, 
                                                             &plmn_index,
                                                             subs_id);

  if (NULL == efs_throttle_tbl_ptr)
  {
    DS_3GPP_MSG0_ERROR("Current PLMN not present in efs throttle tbl");
    return;
  }
  else
  {
    efs_throttle_param_ptr = efs_throttle_tbl_ptr->efs_throttle_param[apn_index];

    if (NULL != efs_throttle_param_ptr)
    {
      efs_throttle_param_ptr->backoff_tmr_timestamp = 0;
      if (FALSE == ds_3gpp_rpm_write_throttle_params(efs_throttle_param_ptr->apn, subs_id))
      {
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR,
                                "Failed to update EFS for apn: %s",
                                efs_throttle_param_ptr->apn);
      }
      if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)efs_throttle_param_ptr->apn))
      {
          is_attach_apn = TRUE;
      }

    }
    else
    {
      DS_3GPP_MSG1_ERROR("apn_index : %d is empty", apn_index);
    }
  }

  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);

#ifdef FEATURE_DATA_LTE
  DS_3GPP_MSG3_HIGH("Trying to enable Lte as plmn has changed",0,0,0);
  if (is_attach_apn)
  {
  ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
  }
#endif /*FEATURE_DATA_LTE*/

} /* ds_3gpp_rpm_backoff_tmr_exp_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_START_HOURLY_TIMER

DESCRIPTION
  This function starts the hourly timer for the specified APN and updates
  hourly timer timestamp in the efs throttle param tbl.

PARAMETERS
  apn       - apn for which hourly timer has to be started.
  timer_val - timer val (to take care of the scenario when the timer has
                         to be restarted for remaining duration after powerup)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_start_hourly_timer
(
  byte   *apn,
  uint32 timer_val,
  sys_modem_as_id_e_type subs_id
)
{
  int8    apn_index;
  uint32  current_time;
  ds_3gpp_rpm_throttle_param_type        *apn_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type *efs_throttle_param_ptr;

  apn_tbl_ptr = ds_3gpp_rpm_get_throttle_param_ptr(apn, 
                                                   FALSE, 
                                                   &apn_index,
                                                   subs_id);

  if (NULL == apn_tbl_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "APN: %s not found in table",apn);
    return;
  }

  if (apn_index >= 0 && apn_index < DS_MAX_APN_NUM)
  {
    if (FALSE == apn_tbl_ptr->is_hourly_tmr_running)
    {
      DS_3GPP_RPM_SET_TIMER(&(apn_tbl_ptr->tmr_hourly), 
                            timer_val,
                            apn_tbl_ptr->is_hourly_tmr_running);

      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                              "Hourly timer started for APN: %s",apn);

      /* Update the timestamp in efs throttle tbl only if the timer is
         started for an hour else it implies that timer is being restarted
         for the remaining duration of an hour after power reset */
      if (timer_val == SEC_TO_MS(SEC_PER_HOUR))
      {
        current_time = time_get_secs();
        efs_throttle_param_ptr = ds_3gpp_rpm_get_efs_throttle_param_ptr(
                                                                    apn, 
                                                                    FALSE, 
                                                                    &apn_index,
                                                                    subs_id);

        if (NULL == efs_throttle_param_ptr)
        {
          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                                  "APN : %s has no entry in the efs throttle tbl",apn);
          return;
        }
        else
        {
          DS_3GPP_MSG1_HIGH("Timestamp stored in efs throttle tbl is %d",
                             current_time);
          efs_throttle_param_ptr->hourly_tmr_timestamp = current_time;
        }
      }
    }
    else
    {
      DS_3GPP_MSG3_HIGH("Hourly timer already running, not restarting", 0,0,0);
    }
  }
  else
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "APN: %s not found in failure param tbl",apn);
  }
} /* ds_3gpp_rpm_start_hourly_timer */

/*===========================================================================
FUNCTION DS_3GPP_RPM_GET_THROTTLE_PARAMS

DESCRIPTION
  This function retrieves the throttle params and updates the throttle_sm_ptr.

PARAMETERS
  apn          - apn for which throttle params are to be retrieved.
  throt_sm_ptr - throt_sm_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean  ds_3gpp_rpm_get_throttle_params
(
  byte                    *apn,
  ds_3gppi_throttle_sm_s* throt_sm_ptr
)
{
  int8    apn_index;
  ds_3gpp_rpm_cause_code_enum_type   throttle_cause;
  ds_3gpp_rpm_throttle_param_type   *apn_tbl_ptr;

  if (NULL == throt_sm_ptr)
  {
    DS_3GPP_MSG0_ERROR("Invalid throt_sm_ptr passed in rpm get throttle timer");
	return FALSE;
  }

  apn_tbl_ptr = ds_3gpp_rpm_get_throttle_param_ptr(apn,
                                                   FALSE,
                                                   &apn_index,
                                                   throt_sm_ptr->subs_id
                                                   );

  if (NULL == apn_tbl_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "Entry not found for APN: %s",apn);
    return FALSE;
  }

  if (apn_index >= 0 && apn_index < DS_MAX_APN_NUM)
  {
    /*-----------------------------------------------------------------------
      APN match found. Update failure count and throttle timer val 
    ------------------------------------------------------------------------*/
    throttle_cause            = apn_tbl_ptr->last_throttle_cause;

    if (throttle_cause <= DS_3GPP_RPM_CAUSE_CODE_MIN ||
        throttle_cause > DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS)
    {
      DS_3GPP_MSG1_ERROR("Invalid throttle_cause: %d", throttle_cause);
      return FALSE;
    }

    throt_sm_ptr->throt_tmr_val 
                          = apn_tbl_ptr->backoff_timer_val;
    throt_sm_ptr->failure_count 
                          = apn_tbl_ptr->counter_val[throttle_cause - 1];
    DS_3GPP_MSG1_HIGH("ds_3gpp_rpm_get_throttle_params: Backoff timer val : %d",
                       throt_sm_ptr->throt_tmr_val);
  }
  else
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                        "APN: %s not found in the RPM failure param table",apn);
    return FALSE;
  }

  return TRUE;
} /* ds_3gpp_rpm_get_throttle_params */

/*===========================================================================
FUNCTION DS_3GPP_RPM_INC_CPDP

DESCRIPTION
  This function increments the cpdp count which corresponds to the last
  throttle cause for the specified apn. Write back the updated cpdp value
  to SIM.

PARAMETERS
  apn - apn for which cpdp count is to be incremented.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_inc_cpdp
(
  byte  *apn,
  sys_modem_as_id_e_type subs_id
)
{
  int8    apn_index;
  uint32  current_time;
  ds_3gpp_rpm_cause_code_enum_type   throttle_cause;
  ds_3gpp_rpm_throttle_param_type   *apn_tbl_ptr;
  mmgsdi_write_data_type            write_data;

  if (NULL == apn)
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_rpm_inc_cpdp: NULL apn is not supported");
    return FALSE;
  }

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return FALSE;
  }

  if (FALSE == ds_3gpp_rpm_util_params[subs_id]->rpm_params_present_in_sim)
  {
    DATA_MSG0_HIGH("RPM parameters are not present in SIM, not incrementing CPDP");
    return FALSE;
  }

  DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                          "ds_3gpp_rpm_inc_cpdp: for APN: %s", apn);

  apn_tbl_ptr = ds_3gpp_rpm_get_throttle_param_ptr(
                                                   apn,
                                                   FALSE,
                                                   &apn_index,
                                                   subs_id
                                                  );

  if (NULL == apn_tbl_ptr)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                            "Entry for APN: %s not found in table",apn);
    return FALSE;
  }

  if (apn_index >= 0 && apn_index < DS_MAX_APN_NUM)
  {
    /*-----------------------------------------------------------------------
      APN match found. Update failure count and throttle timer val 
    ------------------------------------------------------------------------*/
    throttle_cause            = apn_tbl_ptr->last_throttle_cause;
    DS_3GPP_MSG1_HIGH("Throttle cause is %d", throttle_cause);

    if( (throttle_cause > 0) && (throttle_cause <= DS_3GPP_RPM_MAX_CPDP_CNT) )
    {
      ds_3gpp_rpm_params[subs_id].cpdp_cnt[throttle_cause - 1]++;
    }
    current_time = time_get_secs();
    ds_3gpp_rpm_start_lr3_timer(current_time,
                                (ds_3gpp_rpm_params[subs_id].leak_rate3 * SEC_PER_HOUR),
                                subs_id);

    memset(&write_data, 0, sizeof(write_data));

    /*-------------------------------------------------------------------------
      Write the CPDP values onto the SIM 
    -------------------------------------------------------------------------*/
    write_data.data_len = DS_3GPP_RPM_MAX_CPDP_CNT * sizeof(uint8);
    write_data.data_ptr 
                     = (void *)(ds_3gpp_rpm_params[subs_id].cpdp_cnt);

    if (FALSE == ds_3gpp_rpm_mmgsdi_write(
                                          DS_3GPP_RPM_OFFSET_CPDP1,
                                          &write_data,
                                          subs_id
                                          ))
    {
      DS_3GPP_MSG0_ERROR("Write failed for CPDP count");
      return FALSE;
    }
  }
  else
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                        "APN: %s not found in the RPM failure param table",apn);
    return FALSE;
  }

  return TRUE;
} /* ds_3gpp_rpm_inc_cpdp */

/*===========================================================================
FUNCTION DS_3GPP_RPM_RESET_CPDP

DESCRIPTION
  This function resets the cpdp counts.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_reset_cpdp
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8                         cpdp_index;
  uint32                        write_offset;
  mmgsdi_write_data_type        write_data;
  ds_3gpp_rpm_data_buffer_type  efs_write_data;
  ds_3gpp_rpm_util_params_s_type *rpm_util_ptr;

  DS_3GPP_MSG0_HIGH("ds_3gpp_rpm_reset_cpdp: ");

  memset(&write_data, 0, sizeof(write_data));

  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return FALSE;
  }

  if (NULL == (rpm_util_ptr = ds_3gpp_rpm_util_params[subs_id]))
  {
    DS_3GPP_MSG1_ERROR("RPM util params not allocated yet for subs_id: %d",
		                subs_id);
	return FALSE;
  }

  /*-------------------------------------------------------------------------
    Reset the CPDP values and write back to SIM
  -------------------------------------------------------------------------*/
  for (cpdp_index = 0; cpdp_index < DS_3GPP_RPM_MAX_CAUSE_CODES; cpdp_index++)
  {
    ds_3gpp_rpm_params[subs_id].cpdp_cnt[cpdp_index] = 0;
  }
  
  write_data.data_len = DS_3GPP_RPM_MAX_CPDP_CNT * sizeof(uint8);
  write_data.data_ptr 
                   = (void *)(ds_3gpp_rpm_params[subs_id].cpdp_cnt);

  if (FALSE == ds_3gpp_rpm_mmgsdi_write(
                                        DS_3GPP_RPM_OFFSET_CPDP1,
                                        &write_data,
                                        subs_id
                                        ))
  {
    DS_3GPP_MSG0_ERROR("Write failed for CPDP count");
    return FALSE;
  }

  /* Clear the LR3 timer if its running */
  if (TRUE == rpm_util_ptr->is_lr3_tmr_running)
  {
    DS_3GPP_RPM_CLR_TIMER(
		                  &(rpm_util_ptr->ds_3gpp_rpm_tmr_lr3), 
		                  rpm_util_ptr->is_lr3_tmr_running);
  }

  /*-------------------------------------------------------------------------
    Reset the last_cpdp_dec_time and lr3_timestamp in EFS
  -------------------------------------------------------------------------*/
  memset(&efs_write_data, 0, sizeof(efs_write_data));
  
  efs_write_data.data_len = (2*sizeof(uint32));
  efs_write_data.data_ptr = modem_mem_alloc((2*sizeof(uint32)), 
                                                MODEM_MEM_CLIENT_DATA);
  write_offset = (DS_3GPP_RPM_SUBS_BLK_SIZE * subs_id) + 
  	              DS_3GPP_RPM_LAST_CPDP_DECREMENT_TIME_OFFSET;
  
  if (NULL == efs_write_data.data_ptr)
  {
    DS_3GPP_MSG0_ERROR("Could not allocate memory for efs_write_data");
    return FALSE;
  }
  else
  {
    memset(efs_write_data.data_ptr, 0, (2*sizeof(uint32)));
  }
  
  /* Write last CPDP decrement and lr3_timestamp time to EFS */
  if (FALSE == ds_3gpp_rpm_write_to_efs(&efs_write_data, 
                                        write_offset, 
                                        FALSE))
  {
    DS_3GPP_MSG0_ERROR("Write failed");
    return FALSE;
  }
  
  modem_mem_free(efs_write_data.data_ptr, MODEM_MEM_CLIENT_DATA);

  return TRUE;
}


/*===========================================================================
FUNCTION DS_3GPP_RPM_PLMN_ID_CHG_HDLR

DESCRIPTION
  This function handles plmn_id change. Stops the backoff timers for each apn
  on the old_plmn_id. If old_plmn_id is zero, then read_complete_cmd is
  posted.

PARAMETERS
  old_plmn_id - the old_plmn_id.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_plmn_id_chg_hdlr
(
  sys_plmn_id_s_type     old_plmn_id,
  sys_modem_as_id_e_type subs_id
)
{
  int8                                   apn_index;
  int8                                   index = 0;
  uint32                                 read_all_params = FALSE;
  sys_plmn_id_s_type                     new_plmn_id;
  ds_3gpp_rpm_throttle_tbl_type             *plmn_tbl_ptr;
  ds_3gpp_rpm_throttle_param_type        *apn_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_tbl_type         *efs_throttle_tbl_ptr;
  ds_3gpp_rpm_efs_throttle_param_type *efs_throttle_param_ptr;
  boolean                              efs_throttle_pointer_set
                                         [DS_MAX_APN_NUM]; 

  memset(&new_plmn_id, 0, sizeof(new_plmn_id));
  ds3g_get_current_plmn_id(&new_plmn_id, subs_id);
  
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if (TRUE == 
      ds_3gpp_throt_sm_is_plmn_non_null(old_plmn_id))
  {
    /* Stop the backoff timers running for all the APNs on the old_plmn */
    plmn_tbl_ptr = ds_3gpp_rpm_get_throttle_tbl_ptr_from_plmn(old_plmn_id, 
                                                              subs_id);

    if (NULL == plmn_tbl_ptr)
    {
      DS_3GPP_MSG0_ERROR("Entry for the old PLMN ID is not present");
    }
    else
    {
      efs_throttle_tbl_ptr = ds_3gpp_rpm_get_efs_throttle_tbl_ptr_from_plmn(
                                                                   old_plmn_id,
                                                                   &index,
                                                                   subs_id);

      if (NULL == efs_throttle_tbl_ptr)
      {
        DS_3GPP_MSG0_ERROR("efs_throttle_tbl_ptr is NULL");
        return;
      }

      for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
      {
        apn_tbl_ptr        = plmn_tbl_ptr->throttle_param[apn_index];
        efs_throttle_param_ptr = efs_throttle_tbl_ptr->efs_throttle_param[apn_index];
	    efs_throttle_pointer_set[apn_index] = FALSE;

        if (NULL != apn_tbl_ptr)
        {
          if (TRUE == apn_tbl_ptr->is_hourly_tmr_running)
          {
            DS_3GPP_RPM_CLR_TIMER(&(apn_tbl_ptr->tmr_hourly),
                                  apn_tbl_ptr->is_hourly_tmr_running);
          }
          if (DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS 
               == apn_tbl_ptr->last_throttle_cause)
          {
            apn_tbl_ptr->counter_val[apn_tbl_ptr->last_throttle_cause - 1] = 0;
          }
          ds_3gpp_rpm_clear_throttle_timer(apn_tbl_ptr->apn, old_plmn_id, subs_id);
        }

        if (NULL != efs_throttle_param_ptr)
        {
          efs_throttle_param_ptr->hourly_tmr_timestamp  = 0;
          efs_throttle_param_ptr->backoff_tmr_timestamp = 0;

          /* Clear the entry for this APN as the last throttle cause was
             for max_success and as the timers are cleared it would not
             be required to restart them at power-up 
             Also setting the boolean array efs_throttle_pointer_set to 
             TRUE for temporary reference since Freeing is done outside 
             the for loop  */
          if (DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS 
               == efs_throttle_param_ptr->last_throttle_cause)
          {
            memset(efs_throttle_param_ptr, 0, DS_3GPP_RPM_APN_BLK_SIZE);
            efs_throttle_pointer_set[apn_index] = TRUE;
          }
        }
        //Enable Plmn on the old plmn id; if it was earlier disabled
        if( ds_3gpp_pdn_cntx_get_rpm_plmn_blocked() )
        {
 #ifdef FEATURE_DATA_LTE
          ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(old_plmn_id, 
                   CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY, subs_id);
 #endif /*FEATURE_DATA_LTE*/
          ds_3gpp_pdn_cntx_set_rpm_plmn_blocked(FALSE);
        }
      }

      if (FALSE == ds_3gpp_rpm_write_efs_throttle_tbl(
                                                     efs_throttle_tbl_ptr, 
                                                     index,
                                                     subs_id))
      {
        DS_3GPP_MSG0_ERROR("Failed to update EFS throttle params");
      }

      for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
      {
        if (TRUE == efs_throttle_pointer_set[apn_index] )
        {
           efs_throttle_param_ptr = efs_throttle_tbl_ptr->
                                        efs_throttle_param[apn_index];
           if(efs_throttle_param_ptr != NULL)
           {
             DS_3GPP_MSG1_HIGH("Free APN block in efs throttle\
                                table for index : %d", apn_index);
             modem_mem_free(efs_throttle_param_ptr, MODEM_MEM_CLIENT_DATA);
             efs_throttle_tbl_ptr->efs_throttle_param[apn_index] = NULL;
           }
         }
       }
     }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Old PLMN ID is Zero");

	if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
    {
      return;
    }

    /* APN Throttling processing */
    ds_3gpp_rpm_power_up_throttle_hdlr(subs_id);

	if (NULL == ds_3gpp_rpm_util_params[subs_id])
  	{
  	  DS_3GPP_MSG1_ERROR("RPM utils param ptr not allocated yet, subs_id: %d",
	  	                  subs_id);
	  return;
	}

    if (DS_3GPP_RPM_ALL_PARAMS_READ_MASK == 
		ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask)
    {
      ds_3gpp_rpm_process_read_complete(subs_id);
    }
    else
    {
      if (ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_param_read_mask == 0x00)
      {
        read_all_params = TRUE;
      }
      ds_3gpp_rpm_process_read_cmd(read_all_params, subs_id);
    }
  }

  if (TRUE == ds_3gpp_throt_sm_is_plmn_non_null(new_plmn_id))
  {

    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);

#ifdef FEATURE_DATA_LTE
    DS_3GPP_MSG3_HIGH("Trying to enable Lte as plmn has changed",0,0,0);
  ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
#endif /*FEATURE_DATA_LTE*/
  }
} /* ds_3gpp_rpm_plmn_id_chg_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_RPM_MMGSDI_REFRESH_RSP_CB

DESCRIPTION
  This function handles the MMGSDI refresh registration response callback.  

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void ds_3gpp_rpm_mmgsdi_refresh_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ASSERT( cnf_ptr != NULL );

  if ( ( MMGSDI_SUCCESS == status ) && 
       ( MMGSDI_SUCCESS == cnf_ptr->response_header.mmgsdi_status ) )
  {
    MSG_HIGH("ds_3gpp_rpm_mmgsdi_refresh_rsp_cb() for cnf = [%d] with status = [%d]"
             " client_data = [%d]",
             cnf, status, cnf_ptr->read_cnf.response_header.client_data );
  }
  else
  {
    DATA_MSG2_ERROR("ds_3gpp_rpm_mmgsdi_refresh_rsp_cb(): status error [%d] client_data = [%x]",
              status, cnf_ptr->read_cnf.response_header.client_data );
  }
}/* ds_3gpp_rpm_mmgsdi_refresh_rsp_cb */

/*===========================================================================
FUNCTION DS_3GPP_RPM_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers for refresh on the RPM EF files on SIM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_register_for_refresh
(
  sys_modem_as_id_e_type subs_id
)
{
  mmgsdi_refresh_file_list_type   refresh_files;
  mmgsdi_app_enum_type            app_type;
  mmgsdi_session_id_type          session_id = 0;
  mmgsdi_file_enum_type           file_list[DS_3GPP_RPM_NUM_EF_FILES];

  DS_3GPP_MSG0_HIGH("ds_3gpp_rpm_register_for_refresh(): ");

  memset(&refresh_files, 0, sizeof(mmgsdi_refresh_file_list_type));

  if (FALSE == ds3g_mmgsdi_get_session_id_from_sub_id(subs_id, &session_id))
  {
    DS_3GPP_MSG1_LOW("Unable to get session_id from subs_id: %d", subs_id);
    return;
  }

  app_type = ds3g_mmgsdi_get_app_type_from_session_id(session_id);

  DS_3GPP_MSG1_HIGH("app_type: %d", app_type);

  if (MMGSDI_APP_SIM == app_type)
  {
    file_list[0] = MMGSDI_SIM_7F66_PROP1_RPM_EF;
    file_list[1] = MMGSDI_SIM_7F66_PROP1_RPM_PARAM;
    file_list[2] = MMGSDI_SIM_7F66_PROP1_RPM_CNTR_LR;
    file_list[3] = MMGSDI_SIM_7F66_PROP1_RPM_CNTRS;
  }
  else if (MMGSDI_APP_USIM == app_type)
  {
    file_list[0] = MMGSDI_USIM_7F66_PROP1_RPM_EF;
    file_list[1] = MMGSDI_USIM_7F66_PROP1_RPM_PARAM;
    file_list[2] = MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR;
    file_list[3] = MMGSDI_USIM_7F66_PROP1_RPM_CNTRS;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("ds_3gpp_rpm_register_for_refresh(): Invalid app_type: %d",
                        app_type);
    return;
  }

  refresh_files.num_files = DS_3GPP_RPM_NUM_EF_FILES;

  refresh_files.file_list_ptr = &file_list[0];

  if (MMGSDI_SUCCESS != 
       mmgsdi_session_register_for_refresh(session_id, 
                                           refresh_files,
                                           TRUE,
                                           ds_3gpp_rpm_mmgsdi_refresh_rsp_cb,
                                           (mmgsdi_client_data_type) 0x7F66))
  {
    DATA_MSG0_ERROR("Failed to register for refresh of the RPM EF files");
  }

} /* ds_3gpp_rpm_register_for_refresh */

/*===========================================================================
FUNCTION DS_3GPP_RPM_HANDLE_MMGSDI_REFRESH

DESCRIPTION
  This function handles refresh for the RPM EF files. Resets the RPM
  initialized flag and reads the refreshed EF files again.

PARAMETERS
  file_path_ptr - pointer to the file_path_ptr.
  file_path_len - length of the file path.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_handle_mmgsdi_refresh
(
  mmgsdi_file_enum_type     file_enum,
  mmgsdi_session_id_type    session_id
)
{
  uint8       read_bitmask = 0xFF;
  uint32      read_all_params = FALSE;
  sys_modem_as_id_e_type subs_id;

  subs_id = ds3g_mmgsdi_get_sub_id_from_session_id(session_id);
  	
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
	  return;
  }

  DS_3GPP_MSG2_HIGH("ds_3gpp_rpm_handle_mmgsdi_refresh: file_enum: [%x] subs_id: %d", 
                     file_enum, subs_id);

  switch (file_enum)
  {
    case MMGSDI_USIM_7F66_PROP1_RPM_EF:
    case MMGSDI_SIM_7F66_PROP1_RPM_EF:
      read_bitmask &= (uint8)(~(DS_3GPP_RPM_ENABLED_FLAG_MASK));
      break;

    case MMGSDI_USIM_7F66_PROP1_RPM_PARAM:
    case MMGSDI_SIM_7F66_PROP1_RPM_PARAM:
      read_bitmask &= (uint8)(~(DS_3GPP_RPM_F_CNT_MASK));
      ds_3gpp_rpm_reset_cpdp(subs_id);
      break;

    case MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR:
    case MMGSDI_SIM_7F66_PROP1_RPM_CNTR_LR:
      read_bitmask &= (uint8)(~(DS_3GPP_RPM_LR3_MASK));
      ds_3gpp_rpm_reset_cpdp(subs_id);
      break;

    case MMGSDI_USIM_7F66_PROP1_RPM_CNTRS:
    case MMGSDI_SIM_7F66_PROP1_RPM_CNTRS:
      read_bitmask &= (uint8)(~(DS_3GPP_RPM_CPDP_CNT_MASK));
      break;
    default:
      DS_3GPP_MSG1_ERROR("Not a RPM file file_enum: %d", file_enum);
      /* No action required: Not a RPM file */
      return;

  }

  ds_3gpp_rpm_unset_read_mask(read_bitmask, subs_id);

  ds_3gpp_rpm_process_read_cmd(read_all_params, subs_id);

} /* ds_3gpp_rpm_handle_mmgsdi_refresh */

/*===========================================================================
FUNCTION DS_3GPP_RPM_IS_SIM_SUPPORTED

DESCRIPTION
  This function checks whether the SIM represented by the mcc and mnc
  supports RPM.

PARAMETERS
  mcc_info_ptr - pointer to mcc.
  mnc_info_ptr - pointer to mnc.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if RPM is supported on the SIM.
  FALSE - if RPM is not supported on the SIM.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_rpm_is_sim_supported
(
  uint8                  *mcc_info_ptr,
  uint8                  *mnc_info_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  uint8     sim_index = 0;
  uint8     *supported_mcc_ptr = NULL;
  uint8     *supported_mnc_ptr = NULL;
  /*----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  
  for (sim_index = 0; sim_index < DS_3GPP_RPM_MAX_SIM_SUPPORTED; sim_index++)
  {
    supported_mcc_ptr = &(ds_3gpp_rpm_supported_sim[subs_id][sim_index].mcc[0]);
    if (memcmp((void *)mcc_info_ptr, 
               (const void *)supported_mcc_ptr, 
               DS_3GPP_MCC_LEN) == 0)
    {
      supported_mnc_ptr = &(ds_3gpp_rpm_supported_sim[subs_id][sim_index].mnc[0]);
      if (memcmp((void *)(mnc_info_ptr), 
                 (const void *)(supported_mnc_ptr), 
                 DS_3GPP_MCC_LEN) == 0)
      {
        DS_3GPP_MSG0_HIGH("RPM is supported on SIM");
        return TRUE;
      }
    }
  }

  return FALSE;
} /* ds_3gpp_rpm_is_sim_supported */

/*===========================================================================
FUNCTION DS_3GPP_RPM_READ_SIM_INFO

DESCRIPTION
  This function processes the read_sim_info_cmd. Reads the imsi_data from
  ds3g mmgsdi and checks if SIM supports RPM and then posts READ_PARAM_CMD.

PARAMETERS
  subs_id.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_read_sim_info
(
  sys_modem_as_id_e_type subs_id
)
{
  nv_ehrpd_imsi_type   imsi_data;
  boolean              read_all_params = FALSE;
  uint8                ds_3gpp_rpm_param_read_mask = 0;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
	  return;
  }

  memset(&imsi_data, 0, sizeof(nv_ehrpd_imsi_type));

  DS_3GPP_MSG1_HIGH("RPM read sim info for subs_id: %d", subs_id);

  if (FALSE == ds3g_mmgsdi_get_imsi_data_per_subs_id(&imsi_data, subs_id))
  {
    DS_3GPP_MSG0_ERROR("Failed to read IMSI data from SIM");
    return;
  }

  if (imsi_data.num_of_mnc_digits == 2)
  {
    imsi_data.ehrpd_mnc[2] = 0xF;
  }

  if ( TRUE == ds_3gpp_rpm_is_sim_supported(imsi_data.ehrpd_mcc, 
                                            imsi_data.ehrpd_mnc,
                                            subs_id) )
  {

    ds_3gpp_rpm_param_read_mask = ds_3gpp_rpm_get_read_mask(subs_id);
    if (ds_3gpp_rpm_param_read_mask == 0x00)
    {
      read_all_params = TRUE;
    }

	if (ds_3gpp_rpm_util_params[subs_id] == NULL)
	{
	  ds_3gpp_rpm_util_params[subs_id] 
	  	         = (ds_3gpp_rpm_util_params_s_type *)
	  	             modem_mem_alloc(sizeof(ds_3gpp_rpm_util_params_s_type), 
                                     MODEM_MEM_CLIENT_DATA);

	  if (NULL == ds_3gpp_rpm_util_params[subs_id])
	  {
	    DATA_ERR_FATAL("Couldn't allocate memory for RPM utils");
	  }
	  else
	  {
	    memset(ds_3gpp_rpm_util_params[subs_id], 0, 
			   sizeof(ds_3gpp_rpm_util_params_s_type));

		/* Configure the LR3 timer */
        rex_def_timer_ex(&(ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_tmr_lr3), 
                   (rex_timer_cb_type)ds_3gpp_rpm_lr3_tmr_cb, 
                   subs_id);
	  }

      ds_3gpp_rpm_process_read_cmd(read_all_params, subs_id);
    
      ds_3gpp_rpm_register_for_refresh(subs_id);
	}
  }
  else
  {
    DS_3GPP_MSG0_ERROR("RPM is not supported on the SIM:");
    DS_3GPP_MSG3_ERROR("MCC: %d%d%d", 
                        imsi_data.ehrpd_mcc[0],
                        imsi_data.ehrpd_mcc[1],
                        imsi_data.ehrpd_mcc[2]);
    DS_3GPP_MSG3_ERROR("MNC: %d%d%d", 
                        imsi_data.ehrpd_mnc[0],
                        imsi_data.ehrpd_mnc[1],
                        imsi_data.ehrpd_mnc[2]);
  }

  return;
} /* ds_3gpp_rpm_process_read_sim_info_cmd */

/*===========================================================================
FUNCTION DS_3GPP_RPM_CLEANUP_THROTTLE_TBL

DESCRIPTION
  This function frees the throttle tbl and de-allocates the PDN throttle sm.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_cleanup_throttle_tbl
(
  sys_modem_as_id_e_type           subs_id
)
{
  uint8                            plmn_index;
  uint8                            apn_index;
  ds_3gpp_rpm_throttle_tbl_type    *plmn_tbl_ptr = NULL;
  ds_3gpp_rpm_throttle_param_type  *apn_tbl_ptr  = NULL;

  for (plmn_index = 0; plmn_index < DS_3GPP_MAX_PLMN_SUPPORTED; plmn_index++)
  {
    plmn_tbl_ptr = ds_3gpp_rpm_throttle_tbl[subs_id][plmn_index];
  
    if (NULL != plmn_tbl_ptr)
    {
      DS_3GPP_MSG0_HIGH("Cleanup PLMN table ");
      ds_3gpp_rpm_print_plmn_id(&(plmn_tbl_ptr->plmn_id));
 
      for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
      {
        apn_tbl_ptr = plmn_tbl_ptr->throttle_param[apn_index];
  
        if (NULL != apn_tbl_ptr)
        {
          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                                  "Cleanup APN table for APN: %s", 
                                   apn_tbl_ptr->apn);
  
          /* Clear the throttle SM. As RPM throttling is based on APN and not
             pdp_type therefore pdn_pdp_type is passed as V4V6*/
          ds_3gpp_pdn_throt_clear_throttle_state
            (apn_tbl_ptr->apn, DS_UMTS_PDP_IPV4V6, subs_id, 
               DS_3GPP_THROT_RAT_MASK_GLOBAL);
  
          /* Undefine the timer */
          rex_undef_timer(&(apn_tbl_ptr->tmr_hourly));

          /* Free the APN block */
          modem_mem_free(apn_tbl_ptr, MODEM_MEM_CLIENT_DATA);
             plmn_tbl_ptr->throttle_param[apn_index] = NULL;
        }
      }
      /* Free the PLMN block */
      modem_mem_free(plmn_tbl_ptr, MODEM_MEM_CLIENT_DATA);
      ds_3gpp_rpm_throttle_tbl[subs_id][plmn_index] = NULL;
    }
  }
} /* ds_3gpp_rpm_cleanup_throttle_tbl */

/*===========================================================================
FUNCTION DS_3GPP_RPM_CLEANUP_EFS_THROTTLE_TBL

DESCRIPTION
  This function frees the efs throttle tbl and resets the throttle params in
  efs.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_cleanup_efs_throttle_tbl
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint8                                   plmn_index;
  uint8                                   apn_index;
  ds_3gpp_rpm_efs_throttle_tbl_type          *plmn_tbl_ptr = NULL;
  ds_3gpp_rpm_efs_throttle_param_type  *apn_tbl_ptr  = NULL;
  uint32                                subs_offset = 0;

  for (plmn_index = 0; plmn_index < DS_3GPP_MAX_PLMN_SUPPORTED; plmn_index++)
  {
    plmn_tbl_ptr = ds_3gpp_rpm_efs_throttle_tbl[subs_id][plmn_index];
  
    if (NULL != plmn_tbl_ptr)
    {
      DS_3GPP_MSG0_HIGH("Cleanup PLMN table ");
      ds_3gpp_rpm_print_plmn_id(&(plmn_tbl_ptr->ds_plmn_id));
  
      for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
      {
        apn_tbl_ptr = plmn_tbl_ptr->efs_throttle_param[apn_index];
        if (NULL != apn_tbl_ptr)
        {
          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                                  "Cleanup APN table for APN: %s", 
                                   apn_tbl_ptr->apn);
          /* Free the APN block */
          modem_mem_free(apn_tbl_ptr, MODEM_MEM_CLIENT_DATA);
          plmn_tbl_ptr->efs_throttle_param[apn_index] = NULL;
        }
      }
      /* Free the PLMN block */
      modem_mem_free(plmn_tbl_ptr, MODEM_MEM_CLIENT_DATA);
      ds_3gpp_rpm_efs_throttle_tbl[subs_id][plmn_index] = NULL;
    }
  }
  /* Reset the EFS contents */
  subs_offset = (DS_3GPP_RPM_SUBS_BLK_SIZE * subs_id);
  if ( FALSE == (ds_3gpp_rpm_write_to_efs(NULL, subs_offset, TRUE)) )
  {
    DS_3GPP_MSG0_ERROR("Failed to reset throttle params in EFS");
  }
} /* ds_3gpp_rpm_cleanup_efs_throttle_tbl */

/*===========================================================================
FUNCTION DS_3GPP_RPM_PROCESS_ICCID_CHG

DESCRIPTION
  This function processes the ICCID change. Cleans up the throttle tbl and 
  the efs throttle tbl.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_rpm_process_iccid_chg
(
  sys_modem_as_id_e_type subs_id
)
{
  if (!ds_3gpp_rpm_is_util_param_valid(subs_id))
  {
    return;
  }
  
  /* Reset the RPM initialized flag to FALSE. This would be set to TRUE once
     new SIM is read and RPM parameters are configured */
  ds_3gpp_rpm_util_params[subs_id]->ds_3gpp_rpm_is_initialized = FALSE;

  /* Clean up the RPM param table and the throttling module */
  ds_3gpp_rpm_cleanup_throttle_tbl(subs_id);

  /* Clean up the efs throttle table */
  ds_3gpp_rpm_cleanup_efs_throttle_tbl(subs_id);

  return;
} /* ds_3gpp_rpm_process_iccid_chg */

#endif /* FEATURE_DATA_RPM */
