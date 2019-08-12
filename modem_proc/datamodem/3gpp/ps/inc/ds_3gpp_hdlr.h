/*!
  @file
  ds_3gpp_hdlr.h

  @brief
  REQUIRED brief Interface with DSMGR for 3GPP technologies data services.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_hdlr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/11   ars     Added support to disable multi-PDN
06/21/11   dvk     Fixed compilation warnings.
05/04/11   ttv     Added changes needed for SESSION_PARAMS IOCTL and NSAPI api.
03/10/11   ttv     Added support for LTE watermark tracing.
06/17/10   as      Fusion additions 
04/02/10   vrk     UMTS interRAT support
03/15/10   hs      Made changes to increase PDN connections from 3 to 4
===========================================================================*/

#ifndef DS3GPPHDLR_H
#define DS3GPPHDLR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "sys.h"
#include "cm.h"
#include "rex.h"
#include "dstask_v.h"
#include "dsumtsps.h"
#include "queue.h"
#include "ds3gmgr.h"
#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif /* FEATURE_QXDM_WM_TRACING */
#include "ps_sys.h"
#include "ds3gsubsmgr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#ifdef FEATURE_DATA_MPSS_ULT /* Low Tier */
  #define DS_3GPP_MAX_AVAIL_PDNS 4
#else
  #define DS_3GPP_MAX_AVAIL_PDNS PS_SYS_MAX_AVAIL_PDNS
#endif /* FEATURE_DATA_MPSS_ULT */

#ifdef FEATURE_DATA_DISABLE_MULTI_PDN
#define DS_3GPP_MAX_PDN_CONTEXT 2
#define DS_3GPP_NUM_EMERG_PDN_CONTEXT  1
#else
  #define DS_3GPP_MAX_PDN_CONTEXT DS_3GPP_MAX_AVAIL_PDNS
  #ifdef FEATURE_EMERGENCY_PDN
    #define DS_3GPP_NUM_EMERG_PDN_CONTEXT  1
  #else
    #define DS_3GPP_NUM_EMERG_PDN_CONTEXT  0
  #endif
#endif

#ifdef FEATURE_DATA_LTE
  #define DS_3GPP_MAX_NORMAL_PDN_CONTEXT (DS_3GPP_MAX_PDN_CONTEXT - \
                                 DS_3GPP_NUM_EMERG_PDN_CONTEXT)
  #else
  #define DS_3GPP_MAX_NORMAL_PDN_CONTEXT (DS_3GPP_MAX_PDN_CONTEXT)
#endif

#define DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT 2 /* IPv4, IPv6 */
#define DS_3GPP_PDN_CONTEXT_IPV4_IFACE 0 /* IPv4 */
#define DS_3GPP_PDN_CONTEXT_IPV6_IFACE 1 /* IPv6 */
#define DS_3GPP_IPV4_IFACE 0 /* IPv4 */
#define DS_3GPP_MAX_VOBJ_PER_CONTEXT 2 /* UMTS, EPS */ 

#define DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT 8
#define DS_3GPP_MAX_FLOW_CONTEXT                   20

/* This is a temporary value */
#define DS_3GPP_MAX_FLOW_CONTEXT_PER_BEARER_CONTEXT 4

/* 
  2 for default PDN at attach 8 floating ifaces which can be either V4
  or V6. Max of 5 dual IP PDNs.
*/
#ifdef FEATURE_DATA_DISABLE_MULTI_PDN
#define DS_3GPP_MAX_PS_IFACES 4
#define DS_3GPP_MAX_DPL_IFACE_SUPPORTED 4
#else
  #ifndef FEATURE_DATA_MPSS_ULT /* High Tier */
    #define DS_3GPP_MAX_PS_IFACES (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2) + \
                                  (DS_3GPP_NUM_EMERG_PDN_CONTEXT*1)
    #define DS_3GPP_MAX_DPL_IFACE_SUPPORTED (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2) + \
                                            (DS_3GPP_NUM_EMERG_PDN_CONTEXT*1)
    #define DS_3GPP_EMERGENCY_PDN_V4_PS_IFACE_IDX       (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2)  
    #ifdef FEATURE_DATA_PS_IPV6
      #undef DS_3GPP_MAX_PS_IFACES
      #undef DS_3GPP_MAX_DPL_IFACE_SUPPORTED
      #define DS_3GPP_MAX_PS_IFACES (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2) + \
      	                             (DS_3GPP_NUM_EMERG_PDN_CONTEXT*2)
      #define DS_3GPP_MAX_DPL_IFACE_SUPPORTED (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2) + \
      	                                      (DS_3GPP_NUM_EMERG_PDN_CONTEXT*2)
      #define DS_3GPP_EMERGENCY_PDN_V6_PS_IFACE_IDX    (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2)+1
    #endif /* FEATURE_DATA_PS_IPV6 */
  #else
    #define DS_3GPP_MAX_PS_IFACES (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2)
    #define DS_3GPP_MAX_DPL_IFACE_SUPPORTED (DS_3GPP_MAX_NORMAL_PDN_CONTEXT*2)
  #endif /* FEATURE_DATA_MPSS_ULT */
#endif /* FEATURE_DATA_DISABLE_MULTI_PDN */

#define DS_3GPP_MAX_BEARER_CONTEXT 8
#define DS_3GPP_MAX_PHYS_LINK_PER_BEARER 1

#define DS_3GPP_IPV6_IID_OFFSET 1 /*Offset is 1 coz we treat 64 bits at once*/

#define DS_3GPP_MIN_BEARER_CONTEXT_EMERG   2
#define DS_3GPP_MIN_BEARER_CONTEXT         1     

#define DS_3GPP_MAX_APN_IDENTIFIERS        8

typedef enum
{
  DS_3GPP_LTE_NOT_SET = 0,
  DS_3GPP_LTE_DISABLED,
  DS_3GPP_LTE_ENABLED,
  DS_3GPP_LTE_EV_MAX
}ds_3gpp_lte_status;

typedef union 
{
  cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr; /* UMTS Params*/
  cm_pdn_connectivity_s_type     *lte_ps_orig_params_ptr;/* LTE Params */
} ds_3gpp_orig_params_u;



typedef struct {

sys_sys_mode_e_type   network_mode;
ds_3gpp_orig_params_u orig_params_u;

} ds_3gpp_orig_params_s;

/*===========================================================================
   This Structure is used to populate the context transfer information during
   IRAT between G/W <-> L and after sys change handler is received the cntxt_
   transfer flag and target rat are verfied , only then this context transfer
   is reset.
   Scenario 1: (G -> L)
   Step 1: After recieving Cntxt transfer Ind , the Context Transfer Info is
   populated with target RAT and cntxt_transfer flag ,
   Step 2: Only after receiving Target RAT full service , the structure is reset
   This structure was created due to this scenario.
===========================================================================*/
typedef struct
{
  boolean             cntxt_tranfer_is_recvd;
  sys_sys_mode_e_type target_rat;
} ds_3gpp_cntxt_transfer_info_type;


/*===========================================================================
   This Structure is used to maintain all subscription specific global 
   variables of ds_3gpp_hdlr.c.
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW SUBSCRIPTION 
   SPECIFIC GLOBAL VARIABLE.
===========================================================================*/
typedef struct
{
  /*-----------------------------------------------------------------------------
    Cookie to ensure that the data is not corrupted.
   -----------------------------------------------------------------------------*/
  uint32                            cookie;

  /*-----------------------------------------------------------------------------
    Boolean to keep track of whether the attach process has been initiated or not
   -----------------------------------------------------------------------------*/
  boolean                           attach_is_initiated;

  /*-----------------------------------------------------------------------------
    Boolean to keep track of whether the attach process has been completed or not
  -----------------------------------------------------------------------------*/
  boolean                           attach_is_complete;

  /*-----------------------------------------------------------------------------
    Call mode maintained in the mode handler. This value is updated only if the
    mode switches between LTE, WCDMA and GPRS
  -----------------------------------------------------------------------------*/
  sys_sys_mode_e_type               internal_sys_mode; 

  /*-----------------------------------------------------------------------------
    Last 3GPP Mode which was in Full Service
  -----------------------------------------------------------------------------*/
  boolean                           last_full_srv_mode;

  /*---------------------------------------------------------------------------
    Boolean to track whether the data transmission to the RAT is suspended or
    not.
  ---------------------------------------------------------------------------*/
  boolean                           data_suspend_flag;

  /*---------------------------------------------------------------------------
    Boolean to track whether CM has been informed about the availability of
    new data on the uplink or not during inter-RAT change.
    This is a special case where Routing Area Update is triggered by
    data arriving on the Uplink watermark.
  ---------------------------------------------------------------------------*/
  boolean                           sent_ps_data_available;

#ifdef FEATURE_DATA_LTE
  ds_3gpp_lte_status                current_ds_lte_flag;
#endif /* FEATURE_DATA_LTE */

  /*------------------------------------------------------------------------- 
     This Structure is used to populate the context transfer information during
     IRAT between G/W <-> L
   -------------------------------------------------------------------------*/
  ds_3gpp_cntxt_transfer_info_type  cntxt_transfer_info;

  /*-------------------------------------------------------------------------- 
  This is to cache the previous srv domain from CM
  --------------------------------------------------------------------------*/
  sys_srv_domain_e_type             old_srv_domain;

  sys_modem_as_id_e_type            subs_id;
} ds3gpp_per_subs_info_type;

/*---------------------------------------------------------------------------
  Structure to store info for PLMN
---------------------------------------------------------------------------*/
typedef struct
{
  sys_sys_mode_e_type    sys_mode;
  sys_plmn_id_s_type     plmn_id;
  sys_modem_as_id_e_type subs_id;
} ds_3gpp_plmn_info_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_3GPP_EVENT_ENUM_TYPE

DESCRIPTION
  Lists DS 3GPP events that can be registered for
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_EV_MIN = 0,
  DS_3GPP_OPRT_MODE_CHG_EV = DS_3GPP_EV_MIN,
  DS_3GPP_EV_MAX
}ds_3gpp_event_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_3GPP_EVENT_INFO_U_TYPE

DESCRIPTION
  Union type holding event info for all 3GPP events
---------------------------------------------------------------------------*/
typedef union
{
  sys_oprt_mode_e_type oprt_mode;
}ds_3gpp_event_info_u_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_3GPP_EVENT_CBACK_TYPE

DESCRIPTION
  Defines for the event callback function types.
 
PARAMS
  event_info: info pertaining to the event
  user_data_ptr: data to pass back to the user

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ds_3gpp_event_cback_type)
(
  ds_3gpp_event_info_u_type event_info,
  uint32                    user_data
);


/*---------------------------------------------------------------------------
  Cause codes are differentiated by event and layer of origination/destination
  Format for enums: <event>_<from entity/layer>_<details>_TO_<to entity/layer>
---------------------------------------------------------------------------*/
typedef enum
{
  MAP_UNSPECIFIED = 0,
  PDP_MODIFY_SM_INTERNAL_CAUSE_CODES_TO_PS_EIC,
  PDP_MODIFY_SM_NETWORK_CAUSE_CODES_TO_PS_EIC,
  CALL_END_CM_CAUSE_CODES_TO_PS_EIC,
  CALL_END_DS_CAUSE_CODES_TO_PS_EIC,
  CALL_END_CM_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES,
  CALL_END_DS_CAUSE_CODES_TO_PS_IFACE_CAUSE_CODES,
  CALL_END_ESM_CAUSE_TO_PS_IFACE_CAUSE_CODES,
  CALL_END_EMM_CAUSE_TO_PS_IFACE_CAUSE_CODES,
  MAP_MAXIMUM = 0xFF
}ds3gpp_inter_layer_map_e;

/*-----------------------------------------------------------------------------
  NSAPI values associated with bearer contexts.
  num_nsapis specify how many are valid.
-----------------------------------------------------------------------------*/
typedef struct
{
  uint8  num_nsapis;
  uint8  nsapi_val[DS_3GPP_MAX_BEARER_CONTEXT];
}ds3gpp_nsapi_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS3GPP_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each pdn context and initializes the
  parameters for each pdn context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask containing the REX signals that the handlers want to wait on.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ds3gpp_init( void );

/*===========================================================================
FUNCTION DS3GPP_GET_CURRENT_NETWORK_MODE

DESCRIPTION
  This function returns the current mode within the 3GPP RATs. This mode
  is maintained by the 3GPP mode handler and is used to dispatch mode
  specific functions

PARAMETERS
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type - mode currently in

SIDE EFFECTS
  None.

===========================================================================*/
sys_sys_mode_e_type ds3gpp_get_current_network_mode
( 
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION DS3GPP_UPDATE_NETWORK_MODE

DESCRIPTION
  This function updates the network mode maintained inside the 3GPP mode
  handler.

PARAMETERS
  network_mode - the mode to which we have to update.
  subs_id      - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_update_network_mode
( 
  sys_sys_mode_e_type        network_mode,
  sys_modem_as_id_e_type     subs_id
);


/*===========================================================================
FUNCTION DS3GPP_GET_PER_SUBS_INFO_HNDL
 
DESCRIPTION
  This function returns a handle to subscription-specific information in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  Subscription id

DEPENDENCIES 
  None

RETURN VALUE 
  Handle to subscription-specific information.
 
SIDE EFFECTS 
===========================================================================*/
ds3gpp_per_subs_info_type* ds3gpp_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_VALIDATE_PER_SUBS_INFO_HNDL
 
DESCRIPTION
  This function validates the handle to subscription-specific information in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  1. Handle to Subscription Specific Information

DEPENDENCIES 
  None

RETURN VALUE 
  TRUE, if the handle is VALID
  FALSE, if invalid.
 
SIDE EFFECTS 
===========================================================================*/
boolean ds3gpp_validate_per_subs_info_hndl
(
  ds3gpp_per_subs_info_type *hndl
);

/*===========================================================================
FUNCTION DS3GPP_GET_LAST_FULL_SRV_MODE

DESCRIPTION
  This function fetches the latest 3GPP mode in Full Service.

PARAMETERS
  subs_id      - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Latest 3GPP mode to be in Full Service

SIDE EFFECTS
  None.

===========================================================================*/
sys_sys_mode_e_type ds3gpp_get_last_full_srv_mode
( 
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION DS3GPP_UPDATE_LAST_FULL_SRV_MODE

DESCRIPTION
  This function records the latest 3GPP mode in Full Service.

PARAMETERS
  network_mode - the mode to which we have to update.
  subs_id      - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_update_last_full_srv_mode
( 
  sys_sys_mode_e_type        network_mode,
  sys_modem_as_id_e_type     subs_id
);

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION DS3GPP_SET_DS_LTE_FLAG

DESCRIPTION
  This function sets the lte flag based on whether we have disabled or enabled LTE from ds module
  This is based on the boolean variable 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ds_lte_flag
( 
  boolean                   lte_flag, 
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS3GPP_IS_LTE_FLAG_DISABLED

DESCRIPTION
  This function returns TRUE if LTE Flag is disabled

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_lte_flag_disabled
(
  sys_modem_as_id_e_type    subs_id
);

#endif

/*===========================================================================
FUNCTION DS_3GPP_EVENT_INIT

DESCRIPTION
  This function initializes the 3GPP Events module
 
PARAMETERS 
 None
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_event_init(void);

/*===========================================================================
FUNCTION DS_3GPP_REGISTER_EVENT_CB

DESCRIPTION
  This function registers a callback function DS 3GPP events.
    
DEPENDENCIES
  None

RETURN VALUE
  DS3G_SUCCESS - on successful registration
  DS3G_FAILURE - if registration fails

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_event_register_cb
(
  ds_3gpp_event_enum_type  event,
  ds_3gpp_event_cback_type callback_fptr,
  uint32                   user_data
);

/*===========================================================================
FUNCTION DS_3GPP_EVENT_INVOKE_CBS

DESCRIPTION
  This function invokes all the registered callback functions for a
  specific event
 
PARAMETERS
  event - event for which to invoke the registered callbacks
  event_info - info regarding the event 
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_event_invoke_cbs
(
  ds_3gpp_event_enum_type event,
  ds_3gpp_event_info_u_type event_info
);

/*===========================================================================
FUNCTION DS_3GPP_PROCESS_OPRT_MODE_CHG_CMD

DESCRIPTION
  This function processes change in oprt_mode as advertized by
  CM_PH_EVENT_OPRT_MODE cmd
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_process_oprt_mode_chg_cmd
(
  sys_oprt_mode_e_type  oprt_mode
);

/*===========================================================================

FUNCTION DS3GPP_PROCESS_CMDS

DESCRIPTION 
  This function processes the DS commands 

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_process_cmds(ds_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION DS3GPP_PROCESS_PS_CMD

DESCRIPTION

  This function processes the PS commands received by the DS task.
  The  commands that can be received by the mode specific handler
  are as follows:

  DSUMTSPS_IFACE_UP_CMD: Command to bring up the UMTS Interface

  DSUMTSPS_PHYS_LINK_UP_CMD: Command to bring up the UMTS Traffic Channel

  DSUMTSPS_PHYS_LINK_DOWN_CMD: Command to tear down the UMTS Traffic Channel

  DSUMTSPS_IFACE_DOWN_CMD: Command to tear down the UMTS Interface

  DSUMTSPS_SEND_MT_REJECT_CMD: Command to send MT reject when the MT timer
                               expires

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_processPSCmd(const ds_cmd_type *cmd_ptr);


/*===========================================================================
FUNCTION  ds_3gpp_hdlr_nv_refresh_in_progress

DESCRIPTION
 This functions is used to check whether nv refresh is in progress or not.

PARAMETERS
  subs_id - subscription id

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE- nv refresh in progress
          - FALSE nv refresh in progress.

SIDE EFFECTS
  None.

===========================================================================*/


boolean ds_3gpp_hdlr_nv_refresh_in_progress
(
  sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION  DS3GPP_IS_PS_DATA_AVAIL_IND_ALLOWED

DESCRIPTION
  This function queries ds3gi_srv_status maintained by ds3gmgr to check if
  we need to flow control the phys link and not send PS DATA AVAILABLE ind 
  to CM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ps_data_avail_ind_allowed
(
  sys_modem_as_id_e_type    subs_id
);


/*===========================================================================
FUNCTION DS3GPP_GET_IS_ATTACH_INITIATED_FLAG

DESCRIPTION
  This function returns the flag ds3gpp_is_attach_initiated

PARAMETERS
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  True if attach has been initiated, False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_get_is_attach_initiated_flag
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_SET_IS_ATTACH_INITIATED_FLAG

DESCRIPTION
  This function resets the flag ds3gpp_is_attach_initiated

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_is_attach_initiated_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_SET_IS_ATTACH_COMPLETE_FLAG

DESCRIPTION
  This function sets the flag ds3gpp_is_attach_complete to the value passed in
  the argument

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_is_attach_complete_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_GET_IS_ATTACH_COMPLETE_FLAG

DESCRIPTION
  This function gets the flag ds3gpp_is_attach_complete

PARAMETERS
  Subs_Id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_get_is_attach_complete_flag
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_SET_PS_DATA_SUSPEND_FLAG

DESCRIPTION
  This function sets the PS Data Suspend Flag to the value passed in
  the argument

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ps_data_suspend_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_GET_PS_DATA_SUSPEND_FLAG

DESCRIPTION
  This function gets the PS Data Suspend Flag

PARAMETERS
  Subs_Id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds3gpp_get_ps_data_suspend_flag
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_SET_PS_DATA_AVAILABLE_FLAG

DESCRIPTION
  This function sets the PS Data Available to the value passed in
  the argument

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ps_data_available_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_GET_PS_DATA_AVAILABLE_FLAG

DESCRIPTION
  This function gets the PS Data Available Flag

PARAMETERS
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_get_ps_data_available_flag
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_hdlr_bring_down_phys_link_hdlr

DESCRIPTION
  This function brings down the phys link of all the active bearers for
  the given subscription

PARAMETERS
  subs_id - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  uint32 - call id.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_hdlr_bring_down_phys_link_hdlr
(
  sys_modem_as_id_e_type  subs_id
);


#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS3GPP_REPORT_WM_STATS_CB

DESCRIPTION
  This function internally calls bearer context function to report wm status. 

PARAMETERS
  bearer_instance - bearer context instance
  wm_stat_ptr     - pointer where we store the wm values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_report_wm_stats_cb
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
);
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
FUNCTION DS3GPP_GET_NSAPI_FROM_APN

DESCRIPTION
  This function returns the nsapi values associated with given apn value.
  Currently we are providing onle the nsapi value of the default bearer.

PARAMETERS
  apn         - access point name 
  nsapi_table - pointer where we store nsapi values
  ps_error    - incase any error occured, details of it

DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
       DS_EFAULT   if NULL pointers passed.
       DS_ENETDOWN if PDN contexts are not valid.

SIDE EFFECTS
  None.
===========================================================================*/
int ds3gpp_get_nsapi_from_apn
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
);

/*===========================================================================
FUNCTION DS_3GPP_BSR_RESET_HANDLER

DESCRIPTION
  This handler triggers tearing down all PDN connections without active
  clients when the Better system Reslection(BSR) computation is not in
  progress. Tearing down PDN connections will be triggered only when the
  current sys mode is not 3GPP specific mode
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bsr_reset_handler
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PLMN_CHANGE_HANDLER

DESCRIPTION
  This Handler is triggered while PLMN  changes. this is to reset the 
  network override iptype table for cause code 50 and 51 throttling

PARAMETERS
  old_plmn_id - Old PLMN id
  subs_id     - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_plmn_change_hdlr
(
  sys_plmn_id_s_type	 old_plmn_id,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_EPS_GET_PDN_CONN_IND_HDLR

DESCRIPTION
  This function is used to handle the incoming attach request from CM.
  It dispatches the EPS PDN context function to appropriately send out a
  PDN Connectivity request.

PARAMETERS
  attach_info_p

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_eps_get_pdn_conn_ind_hdlr
(
  ds3g_eps_attach_sm_info_type            *attach_info_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION ds3gpp_init_all_crit_sect

DESCRIPTION
  This function initializes the critical section used by 3gpp

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_init_all_crit_sect( void );

/*===========================================================================
FUNCTION DS_3GPP_SET_CNTXT_TRANSFER_INFO

DESCRIPTION
  This function is used to set ds_3gpp_cntxt_transfer_info parameter with
  target rat and cntxt_transfer_flag

PARAMETERS
  sys_sys_mode_e_type : Target RAT
  Boolean             : cntxt_transfer_flag
  subs_id             : Subscription ID

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_set_cntxt_transfer_info
(
  sys_sys_mode_e_type     target_rat,
  boolean                 cntxt_tranfer_is_recvd,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_GET_TARGET_RAT_FROM_CNTXT_TRANSFER_INFO

DESCRIPTION
  This function is used to get ds_3gpp_cntxt_transfer_info parameter with
  target rat

PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
sys_sys_mode_e_type ds_3gpp_get_target_rat_from_cntxt_transfer_info
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_GET_CNTXT_TRANSFER_IND_IS_RECVD

DESCRIPTION
  This function is used to get ds_3gpp_cntxt_transfer_info parameter with
  target rat

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Context Transfer Indication was received
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_get_cntxt_transfer_ind_is_recvd
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS3GPP_INIT_GLOBAL_CRIT_SECTION
 
DESCRIPTION
  This function initializes the 3GPP Global Critical Section.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
void ds3gpp_init_global_crit_section
(
  void
);

/*===========================================================================
FUNCTION DS3GPP_ENTER_GLOBAL_CRIT_SECTION
 
DESCRIPTION
  This function enters the 3GPP Global Critical Section.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
void ds3gpp_enter_global_crit_section
(
  void
);

/*===========================================================================
FUNCTION DS3GPP_LEAVE_GLOBAL_CRIT_SECTION
 
DESCRIPTION
  This function leaves the 3GPP Global Critical Section.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
void ds3gpp_leave_global_crit_section
(
  void
);

/*===========================================================================
FUNCTION ds3gpp_get_subs_id_for_w_td_mode

DESCRIPTION
  This function returns the Subscription on which the current mode is
  WCDMA or TDSCDMA.
  This temporary hack is used because we do not support W/TD + W/TD
  DSDA configurations yet. 

PARAMETERS
  Sys Mode
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.
 
SIDE EFFECTS

===========================================================================*/
sys_modem_as_id_e_type ds3gpp_get_subs_id_for_w_td_mode
(
  void
);

/*===========================================================================
FUNCTION ds_3gpp_map_radio_access_to_sys_mode

DESCRIPTION
  This function maps sys_radio_access_tech_e_type to sys_sys_mode_e_type.

PARAMETERS
  sys_radio_access - system rat
  
DEPENDENCIES
  None.

RETURN VALUE  
  sys_sys_mode - system mode
 
SIDE EFFECTS 
  None. 

===========================================================================*/
sys_sys_mode_e_type ds_3gpp_map_radio_access_to_sys_mode
(
  sys_radio_access_tech_e_type sys_radio_access
);

/*===========================================================================
FUNCTION ds_3gpp_map_sys_mode_to_ds_sys_rat

DESCRIPTION
  This function maps CM Sys Mode to DS SYS RAT

PARAMETERS
  sys_sys_mode_e_type - CM Sys Mode
  
DEPENDENCIES
  None.

RETURN VALUE  
  ds_sys_rat_ex_enum_type - DS Sys RAT
 
SIDE EFFECTS 
  None. 

===========================================================================*/
ds_sys_rat_ex_enum_type ds_3gpp_map_sys_mode_to_ds_sys_rat
(
  sys_sys_mode_e_type sys_mode
);

/*===========================================================================
FUNCTION DS_3GPP_HDLR_READ_SUBS_ID

DESCRIPTION
  This function reads the subscription related info for each subs_id on
  device mode change like pdn limit,roaming etc.,

PARAMETERS
  subs_id -subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_hdlr_read_subs_id
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION       DS_3GPP_APN_NAME_CHANGE_CB_HDLR

DESCRIPTION    Callback handler to process apn_name_change

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_3gpp_apn_name_change_cb_hdlr
(
  uint16                             profile_num,
  sys_modem_as_id_e_type             subs_id
);


#endif /* FEATURE_DATA_3GPP */
#endif /* DS3GPPHDLR_H */
