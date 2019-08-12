#ifndef DS_EPS_KAMGR_EXT_H
#define DS_EPS_KAMGR_EXT_H
/*===========================================================================

  Copyright (c) 2009-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "datamodem_variation.h"
#include "sys.h"
#include "msg.h"
#include "event.h"
#include "queue.h"

#include "ps_sys_conf.h"
#include "ps_sys_event.h"

#include "dsutil.h"
#include "ds3gmgr.h"

/*===========================================================================

                          PUBLIC FUNCTION DEFINATIONS

===========================================================================*/


/*===========================================================================
FUNCTION  DS_EPS_KAMGR_INIT

DESCRIPTION
  Initialization of all the required structures and variable for Keep alive
  manager
  
PARAMETERS
 None
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_init (void);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


/*===========================================================================
FUNCTION  DS_EPS_KAMGR_READ_EFS_INFO

DESCRIPTION
  This function is responsible for reading the EFS file and WLAN offload config
  NV and deciding whether Keep alive manager should be enabled or not
  
PARAMETERS 
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_eps_kamgr_read_efs_info
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION       DS_EPS_KAMGR_PROCESS_CMD

DESCRIPTION    Process DS cmds 
 
PARAMETERS     cmd_ptr :              Command information pointer 

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_eps_kamgr_process_cmd
(
  const ds_cmd_type                    *cmd_ptr
);

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_CHECK_FOR_PDN_TEARDOWN

DESCRIPTION
  This function validates DATA UI/Data Roaming Setting along with WLAN
  availability and In use flag of iface to decide whether KAMGR's PDN
  can be torn down or not
  
PARAMETERS 
  subs_id: Currently active Default data subs

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If PDN is eligible for teardown
  FALSE: If PDN doesn't need to be torn down

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_kamgr_check_for_pdn_teardown
(
  sys_modem_as_id_e_type                 subs_id
);

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_APN_NAME_CHG

DESCRIPTION
  This function is responsible for update the internal structure with the
  updated APN name
  
PARAMETERS 
  profile_ptr:                  pointer to profile number

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_process_apn_name_chg
(
  uint16                        profile_id,
  sys_modem_as_id_e_type        subs_id
);

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_ROAMING_STATUS_CHG_CBACK

DESCRIPTION
  This function is called from roaming handler when roaming status changes
  
PARAMETERS 
  subs_id: Current Active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_handle_roaming_status_chg
(
  sys_modem_as_id_e_type                  subs_id
);

/*==============================================================================
FUNCTION  DS_EPS_KAMGR_DEALLOC_KAMGR_INSTANCE

DESCRIPTION
  This function is called to clean-up KAMGR global instance on an NV refresh
  or during KAMGR resource clean-up
  
PARAMETERS 
  subs_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

================================================================================*/
void ds_eps_kamgr_dealloc_kamgr_instance
(
   sys_modem_as_id_e_type  subs_id
);
#endif /* DS_EPS_KAMGR_EXT_H */
