/*===========================================================================
                      DS_ANDSF_CONFIG.H

DESCRIPTION
 Config header file for ANDSF.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
06/09/14    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_CONFIG_H
#define DS_ANDSF_CONFIG_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF

#include "ds_andsf_wqe_prof_mgr.h"

/*===========================================================================
                   EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/
#define DS_ANDSF_CFG_FILE_ROOT_PATH          "/data"
#define DS_ANDSF_CFG_FILE_PATH               "/data/ds_andsf_config.txt"
#define DS_ANDSF_CFG_WQE_WIFI_THRESHOLD_ROOT_PATH "/data"

#ifdef __cplusplus
extern "C"
{
#endif


/*===========================================================================
                    EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
FUNCTION      DS_ANDSF_CFG_INIT

DESCRIPTION   This function initializes the andsf efs and configuration module. This is during 
              power up. 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void   ds_andsf_cfg_init(void);

uint8  ds_andsf_cfg_get_rule_mgr_active_status(void);
void   ds_andsf_check_policy_in_efs(uint32 subs_idx);
float  ds_andsf_cfg_get_lte_meas_alpha(void);
uint32 ds_andsf_cfg_get_lte_sample_interval(void);
uint32 ds_andsf_cfg_get_lte_avg_interval(void);
float  ds_andsf_cfg_get_wifi_meas_alpha(void);
uint32 ds_andsf_cfg_get_wifi_sample_interval(void);
uint32 ds_andsf_cfg_get_wifi_avg_interval(void);
float  ds_andsf_cfg_get_1x_meas_alpha(void);
uint32 ds_andsf_cfg_get_1x_avg_interval(void);
uint32 ds_andsf_cfg_get_1x_acq_hyst_interval(void);
uint32 ds_andsf_cfg_get_1x_lost_hyst_interval(void);
uint32 ds_andsf_cfg_get_1x_sampling_interval(void);
uint16 ds_andsf_cfg_get_pdn_profile_id(void);
char*  ds_andsf_cfg_get_policy_server_url(void);
uint8  ds_andsf_cfg_get_policy_max_retry(void);
uint32 ds_andsf_cfg_get_policy_retry_interval(void);
uint32 ds_andsf_cfg_get_policy_refresh_interval(void);
uint16 ds_andsf_cfg_get_sms_dest_port(void);
void   ds_andsf_cfg_get_http_username(char *username);
void   ds_andsf_cfg_get_http_password(char *password);
boolean ds_andsf_cfg_wqe_construct_wifi_thres_efs(
   ds_andsf_wqe_prof_wifi_thres_info_type *threshold_cache_ptr);
void ds_andsf_cfg_wqe_read_from_efs(void); 
void ds_andsf_remove_threshold_cache_in_efs(uint32 subs_idx);
#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_ANDSF */
#endif /* DS_ANDSF_CONFIG_H */
