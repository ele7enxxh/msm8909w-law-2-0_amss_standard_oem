#ifndef DS_EPS_KAMGR_INT_H
#define DS_EPS_KAMGR_INT_H
/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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
#include "ds_eps_kamgr_ext.h"
#include "ds_3gpp_pdn_context.h"
#include "ds3gmgr.h"
#include "dstask_v.h"
#include "ds_profile_3gppi.h"

/*===========================================================================

                          DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  uint16                             profile_no;
  uint32                             default_timer_val;
  uint32                             incremental_timer_val;
  uint32                             maximum_timer_val;
  uint8                              maximum_retries;
}ds_eps_kamgr_efs_info;

typedef struct
{
/*---------------------------------------------------------------------------
 Tells whether Keep alive manager is active or not
---------------------------------------------------------------------------*/
  uint8                             active_ind;

/*---------------------------------------------------------------------------
 System status, whether LTE is available as full service or limited service
---------------------------------------------------------------------------*/
  boolean                           lte_srv_status;

/*---------------------------------------------------------------------------
 System status, whether WLAN is available or not
---------------------------------------------------------------------------*/
  boolean                           wlan_availability;

/*---------------------------------------------------------------------------
 Keep Alive APN
---------------------------------------------------------------------------*/
  ds3g_apn_name_type                apn_name;

/*---------------------------------------------------------------------------
 Um iface on which cback has to be registered
---------------------------------------------------------------------------*/
  ps_iface_type                     *ps_iface_ptr;

/*---------------------------------------------------------------------------
 Um iface on which cback has to be registered
---------------------------------------------------------------------------*/
  ps_iface_type                     *companion_iface;

/*---------------------------------------------------------------------------
Um iface down event registration buffer
---------------------------------------------------------------------------*/
  void                             *ps_iface_down_ev_buf_ptr;

/*---------------------------------------------------------------------------
Um iface up event registration buffer
---------------------------------------------------------------------------*/
  void                             *ps_iface_up_ev_buf_ptr;

/*---------------------------------------------------------------------------
Um iface bearer tech change event registration buffer
---------------------------------------------------------------------------*/
  void                             *ps_iface_up_all_ev_buf_ptr;

/*---------------------------------------------------------------------------
Wait timer before re-initiating the bring up of keep alive PDN
---------------------------------------------------------------------------*/
  rex_timer_type                    keep_alive_bring_up_timer;

/*---------------------------------------------------------------------------
Timer value required to wait before re-initiating the bring up
---------------------------------------------------------------------------*/
  uint32                            bring_up_timer_val;

/*---------------------------------------------------------------------------
Counter that keep tracks of the occurrence of same net down reason
---------------------------------------------------------------------------*/
  uint16                            retry_counter;

/*---------------------------------------------------------------------------
Previous net down reason received during iface down event cback
---------------------------------------------------------------------------*/
  ps_iface_net_down_reason_type     prev_down_reason;

/*---------------------------------------------------------------------------
Variable to store the profile number read from EFS
---------------------------------------------------------------------------*/
  ds_eps_kamgr_efs_info                 config_info;

/*-------------------------------------------------------------------------- 
 3GPP DS Profile callback function table
---------------------------------------------------------------------------*/
  ds_profile_3gpp_cb_func_tbl_type      ds_profile_kamgr_cb_func_tbl;

}ds_eps_kamgr_info_type;

typedef enum
{
  DS_EPS_KAMGR_EFS_PROFILE_NO = 1,            /**< Keep Alive APN profile No */
  DS_EPS_KAMGR_EFS_DEFAULT_TIMER_VAL,         /**< Default wait timer value */
  DS_EPS_KAMGR_EFS_INCREMENTAL_TIMER_VAL,     /**< Increment to the default timer */
  DS_EPS_KAMGR_EFS_MAXIMUM_TIMER_VAL,         /**< Maximum wait timer value */
  DS_EPS_KAMGR_EFS_MAXIMUM_RETRIES,           /**< Maximum number of retries */

  DS_EPS_KAMGR_EFS_MAX = 0xFF,                /**< Invalid Element type*/
}ds_eps_kamgr_efs_element_enum_type;

/*---------------------------------------------------------------------------
 NV 70315 value for Keep alive manager to kick in
---------------------------------------------------------------------------*/
#define                    DS_EPS_KEEP_ALIVE_EPDG_VALUE      2

/*---------------------------------------------------------------------------
 Default Wait timer after 1st iface down indication
---------------------------------------------------------------------------*/
#define                    DS_EPS_KAMGR_DEFAULT_TIMER_VAL    500

/*---------------------------------------------------------------------------
Maximum retries to be done after we receive first iface down indication
---------------------------------------------------------------------------*/
#define                    DS_EPS_KAMGR_MAX_RETRIES           2

/*---------------------------------------------------------------------------
Constant that stores the maximum timer value
---------------------------------------------------------------------------*/
#define                    DS_EPS_KAMGR_MAX_TIMER_VAL         10000

/*---------------------------------------------------------------------------
Constant that stores the incremental timer value
---------------------------------------------------------------------------*/
#define                    DS_EPS_KAMGR_INCREMENTAL_TIMER_VAL  500

/*---------------------------------------------------------------------------
Constant used to reset Keep Alive Manager active indication flag
---------------------------------------------------------------------------*/
#define                    DS_EPS_KAMGR_RESET                  0x00

/*---------------------------------------------------------------------------
Constant to set Keep Alive Manager active indication flag
---------------------------------------------------------------------------*/
#define                    DS_EPS_SET_KAMGR                    0x01

/*---------------------------------------------------------------------------
Constant to indicate preferred RAT index
---------------------------------------------------------------------------*/
#define                    PREF_RAT_INDEX                      0x00

/*---------------------------------------------------------------------------
Constant to indicate number of EFS tokens
---------------------------------------------------------------------------*/
#define                    DS_EPS_KAMGR_MAX_EFS_TOKENS          02
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_IFACE_DOWN_EV_CBACK

DESCRIPTION
  This function gets called when iface down event is posted
  
PARAMETERS 
  this_iface_ptr:   Iface on which down event was posted
  event:            Event that caused the cback to be invoked
  event_info:       Event info posted by PS
  user_data_ptr:    Data to be passed to the cback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_iface_down_ev_cback
(
  ps_iface_type                           *this_iface_ptr,
  ps_iface_event_enum_type                 event,
  ps_iface_event_info_u_type               event_info,
  void                                    *user_data_ptr
);

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_RETRY_AFTER_FAILURE

DESCRIPTION
  This function incorporates the retry logic after we temporaray cause code
  as net down reason or we are not able to extract throttling info when we
  receive a throttling based cause code
  
PARAMETERS 
  down_reason:                            down reason received from network

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_retry_after_failure
(
  sys_modem_as_id_e_type                           subs_id,
  ps_iface_net_down_reason_type                    down_reason,
  boolean                                          bringup_failure
);

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_IFACE_UP_EV_CBACK

DESCRIPTION
  This function gets called when iface up event is posted
  
PARAMETERS 
  this_iface_ptr:   Iface on which up event was posted
  event:            Event that caused the cback to be invoked
  event_info:       Event info posted by PS
  user_data_ptr:    Data to be passed to the cback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_eps_kamgr_iface_up_ev_cback
(
  ps_iface_type                           *this_iface_ptr,
  ps_iface_event_enum_type                 event,
  ps_iface_event_info_u_type               event_info,
  void                                    *user_data_ptr
);

/*==============================================================================
FUNCTION DS_EPS_KAMGR_DEREG_IFACE_CBACKS

DESCRIPTION
  This function is called to clean up all the cback resources associated with
  an iface
 
PARAMETERS 
  this_iface_ptr:          Iface whose call backs have to be cleaned up
  clean_up:                Complete clean-up required or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

================================================================================*/
void ds_eps_kamgr_dereg_iface_cbacks
(
  ps_iface_type                              *this_iface_ptr,
  ds3gsubsmgr_subs_id_e_type                  subs_indx
);
#endif /* DS_EPS_KAMGR_INT_H */
