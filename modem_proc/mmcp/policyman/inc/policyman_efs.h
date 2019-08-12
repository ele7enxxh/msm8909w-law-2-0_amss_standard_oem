#ifndef _POLICYMAN_EFS_H_
#define _POLICYMAN_EFS_H_

/**
  @file policyman_efs.h

  @brief PM abstraction layer above EFS
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_efs.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/


#define POLICYMAN_EFS_ROOT_DIR                 "/policyman"
#define POLICYMAN_EFS_ROOT_PATH                "/policyman/"

#define POLICYMAN_CARRIER_POLICY_XML_NAME      "carrier_policy.xml"
#define POLICYMAN_CARRIER_POLICY_EFS_PATH      POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_CARRIER_POLICY_XML_NAME

#define POLICYMAN_DEVICE_CONFIG_XML_NAME       "device_config.xml"
#define POLICYMAN_DEVICE_CONFIG_EFS_PATH       POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_DEVICE_CONFIG_XML_NAME

#define POLICYMAN_SEGMENT_LOADING_XML_NAME     "segment_loading.xml"
#define POLICYMAN_SEGMENT_LOADING_EFS_PATH     POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_SEGMENT_LOADING_XML_NAME

#define POLICYMAN_RESTRICTIONS_XML_NAME        "restrictions.xml"
#define POLICYMAN_RESTRICTIONS_EFS_PATH        POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_RESTRICTIONS_XML_NAME

#define POLICYMAN_RAT_ORDER_XML_NAME           "rat_order.xml"
#define POLICYMAN_RAT_ORDER_EFS_PATH           POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_RAT_ORDER_XML_NAME

#define POLICYMAN_CURRENT_MCC_PERSIST_NAME     "current_mcc"
#define POLICYMAN_CURRENT_MCC_NV_PATH          POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_CURRENT_MCC_PERSIST_NAME

#define POLICYMAN_CALL_MODE_PERSIST_NAME       "call_mode"
#define POLICYMAN_CALL_MODE_NV_PATH            POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_CALL_MODE_PERSIST_NAME

#define POLICYMAN_UE_MODE_PERSIST_NAME         "ue_mode"
#define POLICYMAN_UE_MODE_NV_PATH              POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_UE_MODE_PERSIST_NAME

#define POLICYMAN_RAT_MASK_PERSIST_NAME        "rat_mask"
#define POLICYMAN_RAT_MASK_NV_PATH             POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_RAT_MASK_PERSIST_NAME

#define POLICYMAN_SVC_MODE_PERSIST_NAME        "svc_mode"
#define POLICYMAN_SVC_MODE_NV_PATH             POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_SVC_MODE_PERSIST_NAME

#define POLICYMAN_DEVICE_CONFIG_PERSIST_NAME   "device_config"
#define POLICYMAN_DEVICE_CONFIG_NV_PATH        POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_DEVICE_CONFIG_PERSIST_NAME

#define POLICYMAN_TEST_MODE_NAME               "test_mode"
#define POLICYMAN_TEST_MODE_EFS_PATH           POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_TEST_MODE_NAME

#define POLICYMAN_LOCKED_NAME                  "locked"
#define POLICYMAN_LOCKED_EFS_PATH              POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_LOCKED_NAME

#define POLICYMAN_TIMERS_DIR_NAME              "timers"
#define POLICYMAN_TIMERS_EFS_DIR               POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_TIMERS_DIR_NAME
#define POLICYMAN_TIMERS_EFS_PATH_PREFIX       POLICYMAN_TIMERS_EFS_DIR "/"

#define POLICYMAN_FREQ_LIST_PERSIST_NAME       "freq_list"
#define POLICYMAN_FREQ_LIST_NV_PATH            POLICYMAN_EFS_ROOT_DIR "/" POLICYMAN_FREQ_LIST_PERSIST_NAME

#define DEVICE_MODE_NV_FILE                    "/nv/item_files/modem/mmode/device_mode"
#define POLICYMAN_VOICE_DOMAIN_NV_PATH         "/nv/item_files/modem/mmode/voice_domain_pref"
#define POLICYMAN_RAT_ORDER_NV_PATH            "/sd/rat_acq_order"


/*-------- policyman_efs_initialize --------*/
/**
@brief  Initialize the PM EFS subsystem.  Private function for the use of the
        PM task code.

@return
  - POLICYMAN_EFS_STATUS_SUCCESS if intiialization was successful.
  - POLICYMAN_EFS_STATUS_ERROR if unsuccessful.
*/
void policyman_efs_initialize(
  void
);


/*-------- policyman_efs_deinitialize --------*/
/**
@brief  Deinitialize the PM EFS subsystem.  Private function for the use of the
        PM task code.

@return
  None
*/
void policyman_efs_deinitialize(
  void
);

/*-------- policyman_efs_obliterate_subs_ext --------*/
/**
@brief  Private function to obliterate EFS files with a check to delete XMLs.

@return
  Number of files deleted
*/
int policyman_efs_obliterate_subs_ext(
  sys_modem_as_id_e_type  subsId,
  boolean                 deleteXMLs
);

#endif /* _POLICYMAN_EFS_H_ */
