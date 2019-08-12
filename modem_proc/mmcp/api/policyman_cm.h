/**
  @file policyman_cm.h

  @brief Interface to Policy Manager for Call Manager APIs
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/policyman_cm.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#ifndef _POLICYMAN_CM_H_
#define _POLICYMAN_CM_H_

#include "policyman_msg.h"
#include "policyman.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include <sys/types.h>
#include "sys.h"
#include "sd.h"
#include "cm.h"
#include "cfcm.h"

#ifdef __cplusplus
}
#endif


/**  Database
*/

typedef struct
{
  sys_modem_stack_id_e_type stack;        /* Stack id in operation */
  sys_modem_as_id_e_type  asubs_id;       /* Subscription id*/
  sys_sys_id_type_e_type  id_type;        /* PLMN id type */
  sys_plmn_id_s_type      plmn;           /* PLMN of the serving system */
  sys_srv_domain_e_type   srv_domain;     /* Service Domain */
  sys_sys_mode_e_type     sys_mode;     /* System's mode */
  sys_srv_status_e_type   srv_status;   /* Service Status */  
} policyman_cm_serving_info_t;


typedef struct 
{
  sys_modem_as_id_e_type subsId;
  boolean                isPermanent;
} dds_info_t;

typedef enum
{
  SESSION_AVAILABLE     = 1,
  SESSION_NOT_AVAILABLE = 2,
  SESSION_REFRESHED     = 3
} policyman_session_avail_enum_t;

/*-------- policyman_cm_serving_info_cb --------*/
/**
@brief Get the serving system information from Call Manager.

@param[in]  pCmServingInfo    Pointer to CM serving system information.

@return
  None

*/

void policyman_cm_serving_info_cb(
  policyman_cm_serving_info_t *pCmServingInfo
  );


/*-------- policyman_report_msg_free --------*/
/**
@brief  Frees the MSGR message returned by policyman APIs.

@param[in]  pMsg  Pointer to the message returned by
            policyman APIs.

@return
  None.
*/
void policyman_report_msg_free(
  msgr_hdr_s  *pMsg
  );


/*-------- policyman_report_preference_change --------*/
/**
@brief  CM calls this interface to report a preference change.

@param[in]  asubs_id  Subscription ID whose preference is changing.
@param[in]  pPref     Pointer to an allocated cm_sys_sel_pref_params_s_type
                      structure.  Policyman will take ownership of this memory
                      after this function is called - CM should not free it.

@return
  This function returns a pointer to a MSGR message identical to that received
  by a client of the POLICYMAN_CFG_UPDATE_IND message.  Handling of this is
  identical to that of the POLICYMAN_CFG_UPDATE_IND message, except that after
  releasing all attachments the caller of this API should free it with
  modem_mem_free(), rather than msgr_rex_free_msg_buf(), as this has not passed
  through MSGR.
*/
msgr_hdr_s *policyman_report_preference_change(
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  );

/*-------- policyman_compute_policy_with_newpref --------*/
/**
@brief  CM calls this interface to know if there would be a change in current policy 
           with the preference set passed in.

@param[in]  asubs_id  Subscription ID whose preference is changing.
@param[in]  pPref     Pointer to an allocated cm_sys_sel_pref_params_s_type
                      structure.

@return
  This function returns a pointer to a MSGR message identical to that received
  by a client of the POLICYMAN_CFG_UPDATE_IND message.  Handling of this is
  identical to that of the POLICYMAN_CFG_UPDATE_IND message, except that after
  releasing all attachments the caller of this API should free it with
  modem_mem_free(), rather than msgr_rex_free_msg_buf(), as this has not passed
  through MSGR.
*/
msgr_hdr_s *policyman_compute_policy_with_newpref(
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  );


/*-------- policyman_report_preference_msg_free --------*/
/**
@brief  Frees the MSGR message returned by policyman_report_preference_change().

@param[in]  pMsg  Pointer to the message returned by
            policyman_report_preference_change().

@return
  None.
*/
void policyman_report_preference_msg_free(
  msgr_hdr_s  *pMsg
  );

/*-------- policyman_report_multimode_subs --------*/
/**
@brief  CM calls this interface to report a change in number of subs.

@param[in]  active_subs_mask   Mask of active subscriptions

@return
  This function returns a pointer to a MSGR message identical to that received
  by a client of the POLICYMAN_CFG_UPDATE_IND message.  Handling of this is
  identical to that of the POLICYMAN_CFG_UPDATE_IND message, except that after
  releasing all attachments the caller of this API should free it with
  modem_mem_free(), rather than msgr_rex_free_msg_buf(), as this has not passed
  through MSGR.
*/
msgr_hdr_s *policyman_report_multimode_subs(
  uint8 active_subs_mask
  );

/*-------- policyman_report_thermal_state --------*/
/**
@brief blocking api, a client can call to report the thermal monitor status
@param[in] pCfcm_cmd : pointer to cfcm cmd structure received in cfcm msgr message

@return msgr_hdr_s*:    a pointer to a MSGR message identical to that received
  by a client of the POLICYMAN_CFG_UPDATE_IND message.  Handling of this is
  identical to that of the POLICYMAN_CFG_UPDATE_IND message, except that after
  releasing all attachments the caller of this API should free it with
  modem_mem_free(), rather than msgr_rex_free_msg_buf(), as this has not passed
  through MSGR.
*/
msgr_hdr_s *policyman_report_thermal_state(  
cfcm_cmd_type_s const *pCfcm_cmd 
);

/*-------- policyman_get_persisted_user_plmn --------*/
/**
@brief blocking api, a client can call to get the User selected PLMN information
@param[in] subs  : Subs id for which client need user selected plmn
@param[in] clear : After retrieving the persisted user PLMN , Policyman will clear the persisted PLMN
@param[in] pPlmn : User selected PLMN will be update in pPlmn

@return boolean : True if a user-selected PLMN was persisted for the subs id 
*/
boolean policyman_get_persisted_user_plmn(
  sys_modem_as_id_e_type   subs,
  boolean                  clear,
  sys_plmn_id_s_type      *pPlmn 
);

/*-------- policyman_msim_report_subs_info --------*/
/**
@brief  Report on which subscription DDS is active or SUBSCRIPTION is
        Activated/deactivated.

@param[in]  pSubsInfo         Pointer to an allocated structure of dds_info_t type for SUBS info.
@param[in]  active_subs_mask  Mask of active subscriptions

@return
  This function returns a pointer to a MSGR message identical to that received
  by a client of the POLICYMAN_CFG_UPDATE_IND message.  Handling of this is
  identical to that of the POLICYMAN_CFG_UPDATE_IND message, except that after
  releasing all attachments the caller of this API should free it with
  modem_mem_free(), rather than msgr_rex_free_msg_buf(), as this has not passed
  through MSGR.
*/
msgr_hdr_s * policyman_msim_report_subs_info(
  dds_info_t const *pSubsInfo,
  uint8             active_subs_mask
);

/*-------- policyman_device_config_get_e911_subphone_capability --------*/
/**
@brief  Get the emergency subphone RAT capability for device configuration.

@param[in]   pItem         Pointer to a POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
@param[in]   numItems    Number of items in the storage array for which e911 config is required.
@param[out] pRatArray   Pointer to Array for storage for a bitmask of the supported e911 RATs for the subphones.  
                       The bitmask is to be interpreted as a mask of 
                       sys_sys_mode_mask_e_type bits as defined in sys.h.

@return
  - POLICYMAN_STATUS_SUCCESS if no errors.
  - POLICYMAN_STATUS_ERR_INVALID_ITEM_ID if pItem does not point to a
    POLICYMAN_ITEM_DEVICE_CONFIGURATION item.
  - POLICYMAN_STATUS_ERR_INVALID_ARGS if pRatCfg is NULL.
*/
policyman_status_t policyman_get_e911_subphone_capability(
  policyman_item_t const  *pItem,
  size_t                   numItems,
  uint32                  *pRatArray
);

/*-------- policyman_report_acq_fail --------*/
/**
@brief CM calls whenever there is an acq failure
@param[in] subs_id : subscription id corresponding to acq failure
@param[in] stack_id : stack id corresponding to acq failure

@return
  None
*/
void policyman_report_acq_fail(
  sys_modem_as_id_e_type         subs_id,
  sys_modem_stack_id_e_type      stack_id 
);

/*-------- policyman_report_operational_radio_stacks --------*/
/**
@brief CM calls it to report operation status of radio stacks
@param[in] subs_id : subscription id corresponding to the stacks
@param[in] stackMask: Bitmask of operational stacks listed in enum sys_modem_stack_id_e_type

@return
  None
*/
void policyman_report_operational_radio_stacks(
  sys_modem_as_id_e_type subs_id,
  uint32                 stackMask
);


/*-------- policyman_reset_acq_fail --------*/
/**
@brief CM calls whenever there is fresh acquisition starts
@param[in] subs_id   : subscription id corresponding to acq failure reset
@param[in] stackMask : Bit mask on which acq fail count reset is required

@return
  None
*/
void policyman_reset_acq_fail(
  sys_modem_as_id_e_type  subs_id,
  uint8                   stackMask
);


/*-------- policyman_report_session_availability --------*/
/**
@brief CM calls this whenever an MMGSDI session transitions between available
       and unavailable  (both directions) or when a refresh has occurred.
@param[in] notifyType  : Indicates the type of notification
                         SESSION_AVAILABLE, SESSION_NOT_AVAILABLE, SESSION_REFRESHED
@param[in] sessionType : The type of session (GW_PRI, 1X_PRI, GW_SEC, GW_PRI)
                         which this notification is associated with.
@param[in] appType     : The card app type, MMGSDI_APP_XXX  (SIM, USIM, etc).
                         This is used only for SESSION_AVAILABLE notifications
@return
  This function returns a pointer to a MSGR message identical to that received
  by a client of the POLICYMAN_CFG_UPDATE_IND message.  Handling of this is
  identical to that of the POLICYMAN_CFG_UPDATE_IND message, except that after
  releasing all attachments the caller of this API should free it with
  modem_mem_free(), rather than msgr_rex_free_msg_buf(), as this has not passed
  through MSGR.
*/
msgr_hdr_s * policyman_report_session_availability(
  policyman_session_avail_enum_t notifyType,
  mmgsdi_session_type_enum_type  sessionType,
  mmgsdi_app_enum_type           appType
);

#endif /* _POLICYMAN_CM_H_ */
