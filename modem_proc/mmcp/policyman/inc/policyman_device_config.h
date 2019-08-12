#ifndef _POLICYMAN_DEVICE_CONFIG_H_
#define _POLICYMAN_DEVICE_CONFIG_H_

/**
  @file policyman_device_config.h

  @brief  Internal definitions and declarations for the device configuration
          policy.  This is used by both the policy and its unit tests.
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_device_config.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_cfgitem.h"
#include "policyman_diag.h"
#include "policyman_rat_capability.h"


/*-------- policyman_device_config_action_new --------*/
/**
@brief  API to create a new policyman action for the device_configuration
        action in the policy_info.

@param[in]  pElem     Pointer to the XML document element for the
                      <device_configuration> node.
@param[out] ppAction  Pointer to storage for a pointer to the policyman_action_t
                      object created from the XML.

@return
  Status
*/
mre_status_t policyman_device_config_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t                 **ppAction
  );


/*-------- policyman_device_config_if_action_new --------*/
/**
@brief  API to create a new policyman action for the device_configuration
        action in the policy_info if no item exist in database.

@param[in]  pElem     Pointer to the XML document element for the
                      <device_configuration> node.
@param[out] ppAction  Pointer to storage for a pointer to the policyman_action_t
                      object created from the XML.

@return
  Status
*/
mre_status_t policyman_device_config_if_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*-------- policyman_device_config_define_new --------*/
/**
@brief  Define a named device configuration.
*/
mre_status_t policyman_device_config_define_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*-------- policyman_device_config_use_new --------*/
/**
@brief  Use a named device configuration.
*/
mre_status_t policyman_device_config_use_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*-------- policyman_device_config_use_if_new --------*/
/**
@brief  Use a named device configuration only if there is no device configuration.
*/
mre_status_t policyman_device_config_use_if_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*-------- policyman_device_config_feature_new --------*/
/**
@brief  API to create a new <feature> action.  This replaces the "feature"
        attribute of <device_configuration>

@param

@return

*/
mre_status_t policyman_device_config_feature_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*-------- policyman_device_config_specialization_new --------*/
/**
@brief  API to create a new <specialization> action.  This replaces the "specialization"
        attribute of <device_configuration>

@param

@return

*/
mre_status_t policyman_device_config_specialization_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);

/*-------- policyman_policy_device_config --------*/
/**
@brief  The device_configuration policy.
*/
extern policyman_policy_t  policyman_policy_device_config;


#define DEV_CONFIG_NUM_SIMS_MODIFIED  0x00000001
#define DEV_CONFIG_MAX_SUBS_MODIFIED  0x00000002
#define DEV_CONFIG_FEATURE_MODIFIED   0x00000004
#define DEV_CONFIG_MAX_DATA_SUBS_MODIFIED  0x00000008



/*-------- subs_info_t --------*/
typedef struct
{
  sys_subs_feature_t    feature;
  uint32                ratInfo;
} subs_info_t;


/*-------- device_configuration_t --------*/
/**
@brief  Configuration item structure for device_configuration.
*/
typedef struct
{
  POLICYMAN_ITEM;

  uint32                 hardware_mask;
  size_t                 num_sims;
  size_t                 max_active_voice;
  size_t                 max_active_data;
  size_t                 current_cfg_index;    /* Index to indicate which config is currently active on both subphones */
  int                    explicit_index;       /* Tells if current config Index is under HLOS or Modem control */
  uint32                 modified;             /* Configuration has been modified to fit hardware */
  sys_specialization_t   specialization;
  size_t                 num_cfgs;
  subs_info_t           *pConfigs;             /* subphone config array */
  size_t                 e911_num_cfgs;        /* Number of e911 configurations */
  uint32                *pECallRatCfg;         /* e911 subphone Rat Masks */
  boolean                forceUpdate;          /* Tells update code to update despite no changes */
} device_configuration_t;

/* Structure version info */
#define POLICYMAN_DEVICE_CONFIG_VERSION_TAG   0xDECF0000
#define POLICYMAN_DEVICE_CONFIG_VERSION_MASK  0x0000FFFF
#define POLICYMAN_DEVICE_CONFIG_VERSION_NUM   3

/* Structures used to read/write to EFS config item */
typedef struct
{
  size_t                version;
  size_t                num_sims;
  size_t                max_active_voice;
  sys_specialization_t  specialization;
  subs_info_t           config;
} device_config_efs_v1_t;

typedef struct
{
  size_t                version;
  size_t                num_sims;
  size_t                max_active_voice;
  sys_specialization_t  specialization;
  subs_info_t           config;
  size_t                max_active_data;
} device_config_efs_v2_t;

typedef struct
{
  size_t                version;
  size_t                num_sims;
  size_t                max_active_voice;
  sys_specialization_t  specialization;
  size_t                max_active_data;
  int                   explicit_index;
} device_config_efs_v3_t;

// Structure used in retrieve_device_config() parameter list
typedef struct
{
  size_t                num_sims;
  size_t                max_active_voice;
  size_t                max_active_data;
  sys_specialization_t  specialization;
  size_t                num_cfgs;
  int                   explicit_index;
} device_context_t;

/*-------- policyman_device_config_init --------*/
void policyman_device_config_init(
 void
);

/*-------- policyman_device_config_item_new --------*/
device_configuration_t *policyman_device_config_item_new(
  void
);


/*-------- policyman_device_config_item_clone --------*/
device_configuration_t *policyman_device_config_item_clone(
  device_configuration_t  *pdc
);


/*-------- policyman_device_config_update_to_efs --------*/
/**
@brief  Update the device configuration item into EFS
*/
void policyman_device_config_update_to_efs(
  policyman_item_t const *pItem
);


/*-------- policyman_rat_capability_get_default_from_efs --------*/
/**
@brief  Get the default configuration for POLICYMAN_ITEM_DEVICE_CONFIGURATION
        from saved EFS file 'device_config'
*/
policyman_status_t policyman_device_config_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
);


/*-------- policyman_device_config_get_default --------*/
/**
@brief  Get the default configuration for POLICYMAN_ITEM_DEVICE_CONFIGURATION
        if there is no XML configuration file.
*/
policyman_item_t * policyman_device_config_get_default(
  policy_execute_ctx_t *pCtx
);


/*-------- device_config_get_base_config --------*/
/**
@brief  API to get the base sub-phone RAT capability for a specific subscription and configuration.
        Used internally, exported for unit tests.
*/
uint32 device_config_get_base_config(
  device_configuration_t const *pdc,
  size_t                        nCfg,
  size_t                        nSim
);


/*-------- device_config_get_config --------*/
/**
@brief  API to get the RAT capability intersected with hardware capability for a specific 
          subscription and configuration. Used internally, exported for unit tests.
*/
uint32 device_config_get_config(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
);


/*-------- device_config_get_current_config --------*/
/**
@brief  API to get the RAT capability for a current active configuration on a given subsription.
        Used internally, exported for unit tests.
*/
uint32 device_config_get_current_config(
  device_configuration_t const  *pdc,
  size_t                         nSim
);


/*-------- device_config_get_subs_feature --------*/
/**
@brief  API to get the feature for a specific subscription and configuration.
        Used internally, exported for unit tests.
*/
sys_subs_feature_t device_config_get_subs_feature(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
);



/*-------- policyman_device_configuration_compare --------*/
/**
@brief
*/
boolean policyman_device_config_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
);


/*-------- policyman_device_config_display --------*/
/**
@brief
*/
void policyman_device_config_display(
  policyman_item_t  *pItem
);

/*-------- policyman_get_current_num_sim --------*/
policyman_status_t policyman_get_current_num_sim(
  size_t *pnSim
);

/*-------- policyman_device_imei_print --------*/
void policyman_device_imei_print(
  byte  *pImei
);


/*-------- policyman_condition_imei_new --------*/
/**
@brief  Condition to test the IMEI of the device.
*/
mre_status_t policyman_condition_imei_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
);

/*-------- policyman_get_current_device_config --------*/
policyman_status_t policyman_get_current_device_config(
  policyman_item_t  **pdc
);

/*-------- policyman_feature_is_supported_by_hardware --------*/
boolean policyman_feature_is_supported_by_hardware(
  policyman_set_t   *pItemSet
);

/*-------- policyman_device_config_create_and_send_diag_cfg_log --------*/
void policyman_device_config_create_and_send_diag_cfg_log(
  policyman_item_t          *pCfgItem,
  int                        notifyHistoryIndex
);

/*-------- policyman_device_config_fill_diag_cfg_log --------*/
void policyman_device_config_fill_diag_cfg_log(
  device_configuration_t    *pdc,
  LOG_PM_CONFIG_INFO_type   *log_ptr
);

/*-------- policyman_ensure_configurations --------*/
void policyman_ensure_configurations(
  policyman_set_t       *pItemSet
);

/*-------- policyman_device_config_update_subphone_config --------*/
void policyman_device_config_update_subphone_config(
  sys_modem_as_id_e_type  ddsSubs,
  policyman_set_t        *pItemSet
);

/*-------- policyman_device_config_select_config_new --------*/
mre_status_t policyman_device_config_select_config_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
);

/*-------- policyman_device_config_reset_subs_feature --------*/
void policyman_device_config_reset_subs_feature(
  sys_modem_as_id_e_type subsId
);

/*-------- policyman_get_device_config_from_itemset --------*/
device_configuration_t *policyman_get_device_config_from_itemset(
  policyman_set_t *pItemSet
);

/*-------- policyman_device_configuration_get_limited_item --------*/
void policyman_device_configuration_get_limited_item(
  mre_set_t              *pItemSet,
  sys_modem_as_id_e_type  subsId
);

/*-------- policyman_device_config_reset_max_active_data --------*/
void policyman_device_config_reset_max_active_data(
  mre_set_t *pItemSet
);

#endif /* _POLICYMAN_DEVICE_CONFIG_H_ */
