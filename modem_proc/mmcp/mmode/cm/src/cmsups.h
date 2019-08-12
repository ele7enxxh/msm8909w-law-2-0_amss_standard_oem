#ifndef CMSUPS_H
#define CMSUPS_H

/*===========================================================================

       C A L L   M A N A G E R

              S U P P L E M E N T A R Y   S E R V I C E S

                     H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface with
  the Call Manager Sups module (CMWSUPS.C)

Copyright (c) 2001 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsups.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/12   xs      Added PS supplemetary service support
===========================================================================*/
#include "mmcp_variation.h"
#include "cmi.h"
#include "cmph.h"

#if defined CM_GW_SUPPORTED || defined FEATURE_IP_CALL


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define SUPS_RETRY_MAX_DURATION   30  /* Seconds */
#define SUPS_NEXT_RETRY_DURATION 5  /* Seconds */

#define CMWSUPS_IS_MT_SUPS_CMD(cmd)\
           (cmd == CM_SUPS_CMD_GET_PASSWORD_RES || \
                         CM_SUPS_CMD_USS_NOTIFY_RES || \
                         CM_SUPS_CMD_USS_RES || \
                         CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES)

#define CMWSUPS_IS_MT_SUPS_RPT(cmd)\
           (cmd == CM_UNSTRUCTURED_SS_IND || \
                         CM_UNSTRUCTURED_SS_NOTIFY_IND || \
                         CM_GET_PASSWORD_IND || \
                         CM_RELEASE_UNSTRUCTURED_SS_IND || \
                         CM_LCS_LOCATION_NOTIFICATION_IND)
                         
/* SUPS ccbs erase cc entry store.
** Stores ss_ref of ccbs deactivation request
** and the corresponding ccbs_indx requested.
** LIMITATION:- Only one ccbs deactivation request can be pending
** since cm_ss_ref can store deactivation request for only one request
*/
typedef struct {

    boolean           is_erase_cc_conf_pend;
      /* TRUE  - ccbs deactivation request has been sent and CM is waiting
      **         for response
      ** FALSE - No deactivation request has been sent
      */

    byte              cm_ss_ref;
      /* unique reference assigned to each sups cmd */

    cm_ccbs_indx_type ccbs_indx;
      /* ccbs deactivation request has this field
      ** value between 1 and 5 corresponds to one ccbs indx
      ** CM_CCBS_INDX_INVALID is used to deactivate all ccbs indexes
      */

} cmwsups_erase_ccbs_indx_s_type;

typedef enum
{
  CMSUPS_CMD_ALLOW_IMS,
  CMSUPS_CMD_ALLOW_CS,
  CMSUPS_CMD_REJECT
}cmsups_cmd_route_e_type;


/** Enumeration of Reject OIR/ CLIR valid values
*/
typedef enum {

  CMSUPS_SS_REJECT_NONE = 0,
  /* Default Value, OIR/ CLIR Interrogation cmd is allowed*/
  CMSUPS_SS_REJECT_OIR,
  /* Only OIR Interrogation cmd is disallowed*/
  CMSUPS_SS_REJECT_CLIR,
  /* Only CLIR Interrogation cmd is disallowed*/
  CMSUPS_SS_REJECT_OIR_CLIR
  /* Both OIR/ CLIR Interrogation cmd are disallowed*/

} cmsups_is_reject_oir_clir_e_type;


typedef struct cmwsups_info_list_s  cmwsups_info_list_s_type;

struct cmwsups_info_list_s {

  cm_client_s_type               *sups_client_ptr;

  cm_sups_cmd_info_s_type    sups_cmd_info;
  
  cm_sups_cmd_e_type  cmd;
  cm_alpha_s_type     cc_result_alpha;
  cm_cc_result_e_type  cc_result;

  dword                          sr_start_uptime;
  /* Uptime when first SUPS cmd was attempted */
  
  dword                          next_orig_uptime;
  /* Uptime after which next reattempt for SUPS cmd 
  ** will be done */

  cm_client_id_type         client_id;
        /* Client ID - each initialized client structure has
        ** a unique client ID */

  cmwsups_info_list_s_type*  next;
  cmwsups_info_list_s_type*  prev;

};

/* USSD object type - holds all the information that is associated
** with USS request
*/
typedef struct cm_sups_uss_data_info_s {
  sys_modem_as_id_e_type              asubs_id;
  /**< Active subscription ID. */
  uint8                               ss_ref;
  /**< Supplementary Services reference. */
  uint8                               ss_operation;
  /**< Supplementary Services operation. */
  uint8                               invoke_id;
  /**< Invocation ID. */
  cm_uss_res_e_type                   uss_res_type;
  /**< Unstructured Supplementary Services RES Type. */
  uss_data_s_type                     uss_data;
  /**< Unstructured Supplementary Services data. */
  ie_cm_ss_error_s_type               ss_error;
  /**< Supplementary Services error. */
  boolean                             is_fallback_to_cs;
  /**< Fallback to CS flag to be set if USS is retried over CS */
} cm_sups_uss_data_info_s_type;

/* MT USSD context type - holds the information about MT USSD route
** This info will be used while MO response come from Upper layer
*/
typedef struct cm_sups_cmd_mt_ussd_context_info  {
  boolean                    is_sups_cmd_mt_ussd_active;
  /**< This variable tracks if a MT USSD Session is active. It has to be set to TRUE, whenever a MT USSD
     session/transaction has starts. It has to be reset, when the MT USSD session/transaction finishes */

  cmsups_cmd_route_e_type          sups_cmd_mt_ussd_route;
  /* this variable indicates the route/domain of the ongoing active MT USSD session */
}cm_sups_cmd_mt_ussd_context_info_s_type;


/* SUPS object type - holds all the information that is associated
** with sups
*/
typedef struct cmsups_s  {

  byte                       init_mask;
    /* Indicate whether this object was properly initialized */

  cm_client_s_type           *sups_client_ptr;
    /* Client originating commands/ answering events */

  cm_sups_info_s_type                         *sups_info;
    /* Sups information to the clients */

  cm_sups_info_s_type                         *hybr_gw_sups_info;
    /* Hybr Gw Sups information to the clients */

  cm_sups_info_s_type                         *hybr_gw3_sups_info;
    /* Hybr Gw 3ups information to the clients */

  cm_sups_uss_data_info_s_type                *uss_info;
    /* Ongoing USS Command info */

  cm_sups_uss_data_info_s_type                *hybr_gw_uss_info;
    /* Ongoing USS Command info */

  cm_sups_uss_data_info_s_type                *hybr_gw3_uss_info;
    /* Ongoing USS Command info */

  cm_sups_cmd_mt_ussd_context_info_s_type      mt_uss_info;
    /* Ongoing MT USS Context info */

  cm_sups_cmd_mt_ussd_context_info_s_type      hybr_gw_mt_uss_info;
    /* Ongoing MT USS Context info */

  cm_sups_cmd_mt_ussd_context_info_s_type      hybr_gw3_mt_uss_info;
    /* Ongoing MT USS Context info */

  #ifdef FEATURE_SUPS_RETRY
  cmwsups_info_list_s_type        *sups_info_list;
  cmwsups_info_list_s_type        *sups_info_list_head;
  cmwsups_info_list_s_type       *hybr_gw_sups_info_list;
  cmwsups_info_list_s_type       *hybr_gw_sups_info_list_head;
  cmwsups_info_list_s_type       *hybr_gw3_sups_info_list;
  cmwsups_info_list_s_type       *hybr_gw3_sups_info_list_head;
  #endif

  uint8                      num_pending_sups_cmds;
    /* Number of pending sups commands */

  cmwsups_erase_ccbs_indx_s_type  ccbs_erase_pend_indx;
    /* Stores sups ccbs deactivation request that
    ** got sent to network.
    */
  sys_supp_srv_domain_pref_e_type  sups_svc_domain_pref[MAX_SIMS];

  cmsups_is_reject_oir_clir_e_type    sups_reject_oir_clir_interrogation[MAX_SIMS];
   /* Stores NV EFS settings to reject OIR/ CLIR Interrogation  */

  cmsups_cmd_route_e_type          sups_cmd_route;

} cmsups_s_type;


/*===========================================================================

FUNCTION cmsups_ptr

DESCRIPTION
  Return a pointer to the one and only sups object.

  CM sups object.
  The sups object is responsible for:
  1. Processing clients' sups commands.
  2. Processing MN replies for clients' sups commands.
  3. Processing MN sups related notifications.
  4. Notifying the client list of sups events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmsups_s_type  *cmsups_ptr( void );


/*===========================================================================

FUNCTION cmsups_init

DESCRIPTION
  Initializing the sups object.

  This function must be called before the sups object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_init( void );



/*===========================================================================

FUNCTION cmsups_rpt_proc

DESCRIPTION
  Process Lower Layer sups notifications / sups command replies.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);

/*===========================================================================
FUNCTION cmsups_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmsups_dem_is_apps_wakeup_from_suspend_event(

    cm_sups_event_e_type   sups_event
        /* SUPS event */
);

/*===========================================================================

FUNCTION CMSUPS_MEM_FREE_SUPS_INFO

DESCRIPTION
  De allocates the memory held by sups_info_ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_mem_free_sups_info(

  cmsups_s_type   *sups_ptr
    /* Pointer to sups obj to be freed */
);

#ifdef FEATURE_SUPS_RETRY
/*===========================================================================

FUNCTION CMWSUPS_MEM_FREE_SUPS_LIST_INFO

DESCRIPTION
  De allocates the memory held by sups_info_ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwsups_mem_free_sups_list_info(

  cmwsups_info_list_s_type   *list,
    /* Pointer to sups obj to be freed */

  cmwsups_info_list_s_type** list_head
  
);
#endif

/*===========================================================================

FUNCTION cmsups_cmd_copy_fields_into_info

DESCRIPTION
  Copy cmd fields into sups info.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_cmd_copy_fields_into_info(

    cm_sups_info_s_type         *sups_info_ptr,
        /* Pointer to info part of sups object */

    const cm_sups_cmd_info_s_type    *cmd_info_ptr
        /* Pointer to a sups command */

);

/*===========================================================================

FUNCTION cmsups_call_control_complete_cb

DESCRIPTION
  Callback for Number Classification to call when call control is complete.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type      *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type      *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                                   *user_data
    /* data passed from original call */

);

/*===========================================================================

FUNCTION cmsups_cmd_copy_fields

DESCRIPTION
  Copy the active fields of a sups command into the sups object.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_cmd_copy_fields(

    cmsups_s_type               *sups_ptr,
        /* Pointer to the sups object */

    const cm_sups_cmd_s_type    *cmd_ptr
        /* Pointer to a sups command */

);

/*===========================================================================

FUNCTION CMSUPS_EVENT

DESCRIPTION
  Notify the client list of a specified sups event.

DEPENDENCIES
  SUPS object must have already been initialized with cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_event(

    cm_ss_e_type            ss,
        /* stack whose info is being sent */

    cm_sups_event_e_type    sups_event
        /* notify client list of this sups event */
);

/*===========================================================================

FUNCTION CMSUPS_INIT_SUPS_INFO

DESCRIPTION
  Initialize sups info buffer.

DEPENDENCIES
  sups_info_ptr should be a valid non-NULL pointer

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern void cmsups_init_sups_info(cm_sups_info_s_type * sups_info_ptr);
/*===========================================================================

FUNCTION: CMSUPS_INFO_GET

DESCRIPTION
  Copy the current sups event information into a specified buffer.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  TRUE if information is valid, FALSE otherwise.

  Note that the sups information only becomes valid after cmsups_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmsups_info_get(

    cm_ss_e_type                 ss,
        /* stack whose sups info needs to be retrieved */

    cm_sups_info_s_type          *sups_info_ptr
        /* Copy the event info into this buffer */

);

/*===========================================================================

FUNCTION cmsups_get_asubs_id

DESCRIPTION
  Get asubs_id for the call obj.

DEPENDENCIES
  None.

RETURN VALUE
  sys_modem_as_id_e_type.

SIDE EFFECTS
  None.
===========================================================================*/
sys_modem_as_id_e_type cmsups_get_asubs_id (
  const  cm_sups_info_s_type  *sups_info_ptr
);

/*===========================================================================

FUNCTION cmsups_get_srv_domain_pref

DESCRIPTION
  Returns the srv_domain_pref for a given asubs_id

DEPENDENCIES
  None

RETURN VALUE
  sys_supp_srv_domain_pref_e_type

SIDE EFFECTS
  None
===========================================================================*/
sys_supp_srv_domain_pref_e_type cmsups_get_srv_domain_pref(
  sys_modem_as_id_e_type sub_id
);

/*===========================================================================

FUNCTION CMSUPS_INFO_ALLOC

DESCRIPTION
  Allocate a sups info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated sups info struct.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_sups_info_s_type  *cmsups_info_alloc( void );


/*===========================================================================

FUNCTION cmsups_cmd_copy_fields

DESCRIPTION
  Copy the active fields of a sups command into the sups object.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_cmd_copy_fields(

    cmsups_s_type               *sups_ptr,
        /* Pointer to the sups object */

    const cm_sups_cmd_s_type    *cmd_ptr
        /* Pointer to a sups command */

);
/*===========================================================================

FUNCTION CMSUPS_GET_SUPS_INFO_PTR

DESCRIPTION
  Return a pointer to Phase 2 ussd request message sent.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

cm_sups_info_s_type* cmsups_get_sups_info_ptr(

    cm_ss_e_type      ss
      /* stack whose sups info ptr to return */
);

/*===========================================================================

FUNCTION cmsups_get_sups_info_ptr_asubs_id

DESCRIPTION
  Returns a pointer to the correct cm_sups_info based off the asubs_id passed in

DEPENDENCIES
  none

RETURN VALUE
  cm_sups_info_s_type* - sups info ptr corresponding to the asubs_id passed in

SIDE EFFECTS
  none

===========================================================================*/
cm_sups_info_s_type* cmsups_get_sups_info_ptr_asubs_id(

    sys_modem_as_id_e_type asubs_id
      /* sub whose sups info ptr to return */
);

extern cm_sups_event_e_type cmsups_cmd_to_event_map(cm_sups_cmd_e_type sups_cmd);
extern cm_sups_event_e_type cmsups_conf_to_event_map(cm_name_type           sups_conf);
extern boolean cmsups_is_there_pending_sups_on_other_subs(sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION cmsups_alloc_sups_info_ptr

DESCRIPTION
  Initialise sups info ptr

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_sups_info_s_type* cmsups_alloc_sups_info_ptr(
  cm_ss_e_type          ss
    /* ss type */
);

/*===========================================================================

FUNCTION cmsups_ensure_sups_info_ptr_asubs_id

DESCRIPTION
  Returns a pointer to the correct cm_sups_info based on the asubs_id passed in. If the
  current sups_info in the global sups object is NULL, a new sups_info is allocated

DEPENDENCIES
  none

RETURN VALUE
  cm_sups_info_s_type* - sups_info_ptr from global sups obj based on asubs_id passed in

SIDE EFFECTS
  none

===========================================================================*/
cm_sups_info_s_type* cmsups_ensure_sups_info_ptr_asubs_id(
  sys_modem_as_id_e_type asubs_id
    /* sub whose sups_info_ptr to return */
);


/*===========================================================================

FUNCTION cmsups_get_uss_info_ptr

DESCRIPTION
Return a pointer to the uss_info of the specified asubs_id

DEPENDENCIES
none

RETURN VALUE
cm_sups_info_s_type*

SIDE EFFECTS
none

===========================================================================*/
cm_sups_uss_data_info_s_type* cmsups_get_uss_info_ptr (
  sys_modem_as_id_e_type   asubs_id,
    /* sub to get sups_info for */
  cm_sups_cmd_e_type       sups_cmd
       /* Sups Command */
);


/*===========================================================================

FUNCTION cmsups_free_uss_info_ptr

DESCRIPTION
Free the uss_info of the specified asubs_id

DEPENDENCIES
none

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
void cmsups_free_uss_info_ptr(
  sys_modem_as_id_e_type   asubs_id
    /* sub to get sups_info for */
);

/*===========================================================================

FUNCTION cmsups_get_mt_uss_context_info_ptr

DESCRIPTION
Return a pointer to the mt_uss_info of the specified asubs_id

DEPENDENCIES
none

RETURN VALUE
cm_sups_cmd_mt_ussd_context_info_s_type*

SIDE EFFECTS
none

===========================================================================*/
cm_sups_cmd_mt_ussd_context_info_s_type* cmsups_get_mt_uss_context_info_ptr (
  sys_modem_as_id_e_type   asubs_id
    /* sub to get MT USS INFO  for */
);


/*===========================================================================

FUNCTION cmsups_reset_mt_uss_context_info

DESCRIPTION
Reset the mt_uss_info context of the specified asubs_id

DEPENDENCIES
none

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/

void cmsups_reset_mt_uss_context_info(
  sys_modem_as_id_e_type   asubs_id
    /* sub to get MT USS INFO  for */
);

/*===========================================================================

FUNCTION cmsups_set_mt_uss_context_info

DESCRIPTION
Set the mt_uss_info context of the specified asubs_id

DEPENDENCIES
none

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmsups_set_mt_uss_context_info(
  sys_modem_as_id_e_type   asubs_id,
    /* sub to get MT USS INFO  for */
  cmsups_cmd_route_e_type  route
   /* MT USS route*/
);

/*===========================================================================

FUNCTION cmsups_get_mt_uss_routing_context_info

DESCRIPTION
Get the mt_uss_info routing context of the specified asubs_id

DEPENDENCIES
none

RETURN VALUE
TRUE if set

SIDE EFFECTS
none

===========================================================================*/
boolean cmsups_get_mt_uss_routing_context_info(
  sys_modem_as_id_e_type    asubs_id,
    /* sub to get MT USS INFO  for */
  cmsups_cmd_route_e_type  *route
   /* MT USS route*/
);


/*===========================================================================

FUNCTION cmipsups_client_cmd_forward_gw

DESCRIPTION
  Forward a sups commands to MN. This request 
  may come from IMS during fallback indication.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
extern void cmwsups_client_cmd_forward_gw(
  sys_modem_as_id_e_type asubs_id,
    /* Subscription ID */
  cm_sups_cmd_e_type          cmd
    /* Pointer to a CM command type */
);

/*===========================================================================

FUNCTION cmwsups_client_lcs_cmd_forward_gw

DESCRIPTION
  Forward a LCS related sups commands to MN. 

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
void cmwsups_client_lcs_cmd_forward_gw(
  cm_sups_cmd_info_s_type   *sups_cmd_info_ptr,
  /* Pointer to a sups command */
  cm_sups_cmd_e_type         sups_cmd
  /* SUPS command type */
);
#endif 

#endif /* CMWSUPS_H */
