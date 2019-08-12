#ifndef DS_3GPP_RPM_H
#define DS_3GPP_RPM_H
/*!
  @file
  ds_3gpp_rpm.h

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_rpm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/11   ss      Fixed RPM mmgsdi refresh registration issue.
10/24/11   ss      Fixed reading/writing RPM EF files under USIM.
08/01/11   ss      Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_RPM
#include "sys.h"
#include "mmgsdilib_common.h"
#include "ps_iface_defs.h"
#include "dsumtspdpreg.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_throttle_sm.h"
#include "dstask_v.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define   SEC_PER_HOUR         3600
#define   SEC_TO_MS(x)         x*1000

#define   DS_3GPP_FS_DIRECTORY_CHAR ('/')

#define  DS_3GPP_MCC_LEN       3
#define  DS_3GPP_MNC_LEN       3

/*--------------------------------------------------------------------------
  RPM parameters offset in SIM EF as per ATT spec
--------------------------------------------------------------------------*/
#define DS_3GPP_RPM_OFFSET_IS_RPM_ENABLED_FLAG    0

#define DS_3GPP_RPM_OFFSET_F1                     2
#define DS_3GPP_RPM_OFFSET_F2                     DS_3GPP_RPM_OFFSET_F1 + 1
#define DS_3GPP_RPM_OFFSET_F3                     DS_3GPP_RPM_OFFSET_F1 + 2
#define DS_3GPP_RPM_OFFSET_F4                     DS_3GPP_RPM_OFFSET_F1 + 3

#define DS_3GPP_RPM_OFFSET_LR3                    2

#define DS_3GPP_RPM_OFFSET_CPDP1                  2
#define DS_3GPP_RPM_OFFSET_CPDP2                  DS_3GPP_RPM_OFFSET_CPDP1 + 1
#define DS_3GPP_RPM_OFFSET_CPDP3                  DS_3GPP_RPM_OFFSET_CPDP1 + 2
#define DS_3GPP_RPM_OFFSET_CPDP4                  DS_3GPP_RPM_OFFSET_CPDP1 + 3

/*---------------------------------------------------------------------------
  SIM EF path for RPM parameters as per ATT spec
---------------------------------------------------------------------------*/
#define DS_3GPP_RPM_MF                            0x3F00
#define DS_3GPP_RPM_USIM_ADF                      0x7FFF
#define DS_3GPP_RPM_DF1                           0x7F66
#define DS_3GPP_RPM_DF2                           0x5F40

#define DS_3GPP_RPM_EF_PATH_IS_RPM_ENABLED_FLAG   0x4F40
#define DS_3GPP_RPM_EF_PATH_F_CNT                 0x4F41
#define DS_3GPP_RPM_EF_PATH_LR3                   0x4F42
#define DS_3GPP_RPM_EF_PATH_CPDP_CNT              0x4F43

#define DS_3GPP_RPM_DIR_PATH                      0x3F007F665F40

#define DS_3GPP_RPM_NUM_EF_FILES                   4

#define DS_3GPP_RPM_SIM_EF_FILE_PATH_LEN           8

#define DS_3GPP_RPM_USIM_EF_FILE_PATH_LEN         10

#define DS_3GPP_RPM_SIM_FILE_PATH_LEN              4

#define DS_3GPP_RPM_USIM_FILE_PATH_LEN             5

#define DS_3GPP_RPM_PARAM_LEN                      1

#define DS_3GPP_RPM_MAX_F_CNT                      4

#define DS_3GPP_RPM_MAX_CPDP_CNT                   4

#define DS_3GPP_RPM_ALL_PARAMS_READ_MASK           0x1F

#define DS_3GPP_RPM_MAX_CAUSE_CODES                4

#define DS_3GPP_RPM_SIM_EF_PATH_OFFSET             6

#define DS_3GPP_RPM_USIM_EF_PATH_OFFSET            8

#define DS_3GPP_RPM_EF_PATH_LEN                    2

#define DS_3GPP_RPM_MAX_SIM_SUPPORTED             10

#define DS_3GPP_RPM_LAST_CPDP_DECREMENT_TIME_OFFSET sizeof(sys_modem_as_id_e_type)

#define DS_3GPP_RPM_LR3_TIMESTAMP_OFFSET          (DS_3GPP_RPM_LAST_CPDP_DECREMENT_TIME_OFFSET + \
	                                               sizeof(uint32))

#define DS_3GPP_RPM_PLMN_BLK_OFFSET              (DS_3GPP_RPM_LR3_TIMESTAMP_OFFSET + \
                                                  sizeof(uint32))

#define DS_3GPP_RPM_APN_BLK_OFFSET           (DS_3GPP_RPM_PLMN_BLK_OFFSET + \
                                             sizeof(sys_plmn_id_s_type))

#define DS_3GPP_RPM_APN_BLK_SIZE            sizeof(ds_3gpp_rpm_efs_throttle_param_type)

#define DS_3GPP_RPM_PLMN_BLK_SIZE                 (sizeof(sys_plmn_id_s_type) + \
                                    (DS_MAX_APN_NUM * DS_3GPP_RPM_APN_BLK_SIZE))

#define DS_3GPP_RPM_SUBS_BLK_SIZE  ((sizeof(sys_modem_as_id_e_type) + 2*sizeof(uint32)) + \
	                                 (DS_3GPP_MAX_PLMN_SUPPORTED * DS_3GPP_RPM_PLMN_BLK_SIZE))


/*-----------------------------------------------------------------------------
  Macro to start the timer with a given value.
-----------------------------------------------------------------------------*/
#define DS_3GPP_RPM_SET_TIMER(timer, time_value, is_tmr_running)\
  {\
    (void)rex_set_timer(timer, time_value);\
    is_tmr_running = TRUE;\
  }

/*-----------------------------------------------------------------------------
  Macro to stop the timer.
-----------------------------------------------------------------------------*/
#define DS_3GPP_RPM_CLR_TIMER(timer, is_tmr_running)\
  {\
    (void)rex_clr_timer(timer);\
    is_tmr_running = FALSE;\
  }

typedef enum
{
  DS_3GPP_RPM_ENABLED_FLAG_MASK     = 0x01,
  DS_3GPP_RPM_F_CNT_MASK            = 0x02,
  DS_3GPP_RPM_LR3_MASK              = 0x04,
  DS_3GPP_RPM_CPDP_CNT_MASK         = 0x08,
  DS_3GPP_RPM_THROTTLE_PARAM_MASK   = 0x10
}ds_3gpp_rpm_param_read_bitmask_enum_type;

typedef enum
{
  DS_3GPP_RPM_CAUSE_CODE_MIN         = 0,
  DS_3GPP_RPM_CAUSE_CODE_IGNORE      = 1,
  DS_3GPP_RPM_CAUSE_CODE_PERMANENT   = 2,
  DS_3GPP_RPM_CAUSE_CODE_TEMPORARY   = 3,
  DS_3GPP_RPM_CAUSE_CODE_MAX_SUCCESS = 4,
  DS_3GPP_RPM_CAUSE_CODE_MAX         = 0xFF
}ds_3gpp_rpm_cause_code_enum_type;

typedef struct
{
  uint8  mcc[3];
  uint8  mnc[3];
}ds_3gpp_rpm_sim_info_s_type;

typedef PACKED struct PACKED_POST
{
  boolean  is_rpm_enabled;
  uint8    f_cnt[DS_3GPP_RPM_MAX_F_CNT];
  uint8    cpdp_cnt[DS_3GPP_RPM_MAX_CPDP_CNT];
  uint8    leak_rate3;
}ds_3gpp_rpm_params_type;

typedef struct
{
  sys_modem_as_id_e_type             subs_id;
  ds_3gpp_rpm_cause_code_enum_type   last_throttle_cause;
  uint32                             backoff_timer_val;
  boolean                            is_hourly_tmr_running;
  rex_timer_type                     tmr_hourly;
  uint8                              counter_val[DS_3GPP_RPM_MAX_CAUSE_CODES];
  byte                               apn[DS_UMTS_MAX_APN_STRING_LEN+1];
}ds_3gpp_rpm_throttle_param_type;

typedef struct
{
  sys_plmn_id_s_type                     plmn_id;
  ds_3gpp_rpm_throttle_param_type       *throttle_param[DS_MAX_APN_NUM];
}ds_3gpp_rpm_throttle_tbl_type;

typedef PACKED struct PACKED_POST
{
  boolean                            is_valid_block;
  uint32                             hourly_tmr_timestamp;
  uint32                             backoff_tmr_timestamp;
  ds_3gpp_rpm_cause_code_enum_type   last_throttle_cause;
  uint8                              counter_val;
  byte                               apn[DS_UMTS_MAX_APN_STRING_LEN+1];
}ds_3gpp_rpm_efs_throttle_param_type;

typedef PACKED struct PACKED_POST
{
  byte  ds_plmn[3];
}ds_3gpp_rpm_plmn_id_s_type;

typedef PACKED struct PACKED_POST
{
  ds_3gpp_rpm_plmn_id_s_type             ds_plmn_id;
  ds_3gpp_rpm_efs_throttle_param_type    *efs_throttle_param[DS_MAX_APN_NUM];
}ds_3gpp_rpm_efs_throttle_tbl_type;

typedef struct
{
  void   *data_ptr;
  uint32  data_len;
}ds_3gpp_rpm_data_buffer_type;

typedef struct
{
  /* CPDP decrement timestamp and LR3 timestamp. Values populated from EFS */
  uint32  last_cpdp_decrement_time;
  uint32  lr3_tmr_timestamp;
  
  /* RPM initialization params */
  uint8  ds_3gpp_rpm_param_read_mask;
  boolean ds_3gpp_rpm_is_initialized;
  
  /* LR3 timer */
  rex_timer_type ds_3gpp_rpm_tmr_lr3;
  boolean		 is_lr3_tmr_running;

  /* Flag to indicate if RPM parameters are present on SIM/USIM */
  boolean rpm_params_present_in_sim;
}ds_3gpp_rpm_util_params_s_type;

/*===========================================================================

                           EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
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
void ds_3gpp_rpm_init(void);

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
  sys_modem_as_id_e_type	subs_id
);

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
  sys_modem_as_id_e_type  subs_id
);

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
  sys_modem_as_id_e_type  subs_id
);

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
);

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
  sys_plmn_id_s_type   old_plmn_id,
  sys_modem_as_id_e_type  subs_id
);

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
);

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
  mmgsdi_file_enum_type file_enum,
  mmgsdi_session_id_type session_id
);

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
void  ds_3gpp_rpm_hourly_tmr_exp_hdlr
(
  const ds_cmd_type  *cmd_ptr
);

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
  const ds_cmd_type *ds_cmd_ptr
);

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
  sys_modem_as_id_e_type	subs_id
);

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
  boolean                 read_all_params,
  sys_modem_as_id_e_type  subs_id
);

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
);

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
  sys_modem_as_id_e_type  subs_id
);

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
  sys_modem_as_id_e_type  subs_id
);

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
  sys_modem_as_id_e_type  subs_id
);

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
);

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
);

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
  uint8    apn_index,
  sys_modem_as_id_e_type subs_id
);

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
  sys_modem_as_id_e_type	subs_id
);

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
ds_3gpp_rpm_efs_throttle_param_type  *ds_3gpp_rpm_get_efs_throttle_param_ptr
(
  byte     *apn,
  boolean  allow_create,
  int8     *index,
  sys_modem_as_id_e_type  subs_id
);

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
);

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
);

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
);

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

void ds_3gpp_rpm_read_efs_per_subs
(
  sys_modem_as_id_e_type  subs_id
);

#endif /* FEATURE_DATA_RPM */

#endif /* DS_3GPP_RPM_H */
