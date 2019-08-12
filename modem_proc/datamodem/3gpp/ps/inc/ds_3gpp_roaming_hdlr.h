#ifndef DS_3GPP_ROAMING_HDLR_H
#define DS_3GPP_ROAMING_HDLR_H
/*===========================================================================

            DS 3GPP ROAMING HDLR

===========================================================================*/
/*!
  @file
  ds_3gpp_roaming_hdlr.h

  @brief
  This header file externalizes 3GPP Roaming Handler functionality

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_roaming_hdlr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
1/21/14    vs      Initial File
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ds3gsubsmgr.h"
#include "sys_v.h"
#include "mcfg_fs.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------- 
  Maximum number of Home PLMNs that can be configured in the EFS file
-----------------------------------------------------------------------------*/
#define DS_3GPP_ROAMING_MAX_SUPPORTED_HOME_PLMNS 8
#define DS_3GPP_HOME_PLMN_LIST_FILE              "/data/3gpp/home_plmn_list.xml"
#define DS_3GPP_HOME_PLMN_LIST_TAG               "HPLMN_list"
#define DS_3GPP_EXCEPTION_HPLMN_LIST_TAG         "Exception_HPLMN_list"
#define DS_3GPP_PLMN_NAME_TAG                    "MCC-MNC"
#define DS_3GPP_PLMN_LIST_NUM                    2
#define DS_3GPP_MCC_MNC_SIZE                     7

/*---------------------------------------------------------------------------- 
  Structure which incorporates a single MCC/MNC info entry
-----------------------------------------------------------------------------*/
typedef struct
{
  uint8  mcc[3];
  uint8  mnc[3];
}ds_3gpp_roaming_mcc_mnc_info_s;

/*---------------------------------------------------------------------------- 
  Structure which represents the home PLMN List Information read from the EFS
-----------------------------------------------------------------------------*/
typedef struct
{
  uint8                          num_valid_entries;
                                        /* Number of valid entries in the EFS*/
  ds_3gpp_roaming_mcc_mnc_info_s mcc_mnc_info
                                   [DS_3GPP_ROAMING_MAX_SUPPORTED_HOME_PLMNS];
                                       /* Home PLMN information stored in
                                          MCC-MNC format                     */
} ds_3gpp_roaming_efs_info_s;

typedef struct
{
  uint16                 profile_num;
  boolean                roaming_disallowed_flag;
  int                    subs_id;
}ds_3gpp_roam_info_type;

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_POWERUP_INIT

  DESCRIPTION
  This function performs Powerup Initialization of the Module
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_hdlr_powerup_init
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_POWERUP_INIT_PER_SUBS_ID

  DESCRIPTION
  This function performs initialization of the roaming module per subscription
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_roaming_hdlr_powerup_init_per_subs_id
(
  sys_modem_as_id_e_type     subs_id 
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_CLEAR_PER_SUBS_ID

  DESCRIPTION
  This function performs resetting of the roaming module per subscription
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_roaming_hdlr_clear_per_subs_id
(
  sys_modem_as_id_e_type     subs_id 
);
/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_STATUS

  DESCRIPTION
  This function tells whether the UE is Roaming or not.
  
  PARAMETERS  
  Subscription Id 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE,  if UE is roaming
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_roaming_get_status
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_INFORM_NEW_PLMN_INFO

  DESCRIPTION
  This function is used to notify the Roaming Module of PLMN change.
  
  PARAMETERS  
  Subscription Id
  New PLMN Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE,  if UE is roaming
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_inform_new_plmn_info
(
  sys_modem_as_id_e_type subs_id,
  sys_plmn_id_s_type     new_plmn_id  
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CONVERT_TO_PLMN_FORMAT

  DESCRIPTION
  This function converts MCC MNC to PLMN Format
  
  PARAMETERS  
  MCC MNC Info
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PLMN Id
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_id_s_type ds_3gpp_roaming_convert_to_plmn_format
(
  ds_3gpp_roaming_mcc_mnc_info_s mcc_mnc_info
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_PROCESS_DISALLOWED_FLAG_CHG_CMD

  DESCRIPTION
  This function is used to process the change in Roaming Disallowed Flag
  
  PARAMETERS  
  Profile Number
  Roaming Disallowed Flag
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_roaming_process_disallowed_flag_chg_cmd
(
  uint16                 profile_num,
  boolean                roaming_disallowed_flag,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_PROCESS_CMDS

  DESCRIPTION
  This function processes any commands that need to be handled by
  Roaming Handler Module.
  
  PARAMETERS
  1. Cmd Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_hdlr_process_cmds
(
  const ds_cmd_type      *cmd_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_IS_APN_DISALLOWED

  DESCRIPTION
  This function checks whether PDN Connection to given APN is allowed or not.
  
  PARAMETERS
  1. APN Name
  2. Subscription Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE, if PDN Connection is allowed
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_roaming_is_apn_disallowed
(
  byte                   *apn,
  sys_modem_as_id_e_type subs_id
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_HOME_PLMN_LIST_SIZE

  DESCRIPTION
  This function gets the size of the Home PLMN List
  
  PARAMETERS
  Subscription Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Number of Home PLMNs configured
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_roaming_get_home_plmn_list_size
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_EXCP_HOME_PLMN_LIST_SIZE

  DESCRIPTION
  This function gets the size of the exception Home PLMN List
  
  PARAMETERS
  Subscription Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Number of Home PLMNs configured
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_roaming_get_excp_home_plmn_list_size
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_SET_STATUS

  DESCRIPTION
  This function sets the roaming status.
  
  PARAMETERS  
  Subscription Id
  Roaming Status
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_set_status
(
  sys_modem_as_id_e_type subs_id,
  boolean                status
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CLEAR_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the wait timer corresponding to the given
  profile.
  
  PARAMETERS  
  Profile Num
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/ 
void ds_3gpp_roaming_clear_wait_timer_entry
(
  uint8 profile_num  
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_FLUSH_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the entire wait timer list.
  
  PARAMETERS  
  None.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_roaming_flush_wait_timer_list
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ADVERTISE_APNS_BLOCKED_DUE_TO_ROAMING

  DESCRIPTION
  This function advertises all Roaming Blocked APNs to its clients
  
  PARAMETERS  
  Subscription id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
void ds_3gpp_roaming_advertise_apns_blocked_due_to_roaming
(
  sys_modem_as_id_e_type subs_id,
  boolean                advertise_empty_list
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UPDATE_PLMN_INFO_ON_SUBS_READY

  DESCRIPTION
  This function is invoked when subscription ready ev is received.
  This function checks if EHPLMN NV is enabled and if enabled calls
  roaming_inform_new_plnn_onfo which will then take action to perform
  blocking/unblocking.
  
  PARAMETERS  
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_update_plmn_info_on_subs_ready
(
  sys_modem_as_id_e_type subs_id
);

#endif /* DS_3GPP_ROAMING_HDLR_H */



