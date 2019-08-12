#ifndef CMIPAPPI_H
#define CMIPAPPI_H

#include "mmcp_variation.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

               C A L L   M A N A G E R   V o I P   A P P L I C A T I O N

                   I N T E R N A L  H E A D E R   F I L E

DESCRIPTION

   This header describes interfaces that are not exposed to IP apps but are
   internally available to Call Manager.


  Copyright (c) 2005 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmipappi.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/12   xs      Added PS supplemetary service support
04/25/12   gm      Refactor IPAPP callback interface 
01/06/12   gm      Data synchronization changes for freefloating platform
09/19/11   xs      Fix invalid data block sent in call orig.
08/25/11   xs      Add user data block support for some APIs
08/16/10   aj      support VOIP deregistration on ps detach
03/01/09   sv      Adding cmipapp_rpt_s_type
02/26/07   ka      Changes to support Voip deregistration.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
12/11/05   ic      Lint cleanup
10/08/05   ic      Added Header: field 
04/20/05   ka      Informing IP apps when there is a service change
04/05/06   ka      Adding new header functions find_active_app and get_cb_ptrs
03/14/05   dk/ka   Initial revision

===========================================================================*/

#include "cmipapp.h"
#include "cmll_v.h"
#include "cmsups.h"
#include "cmcall.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/


#define CMIPAPP_POOL_SIZE  10
    /* Total number of apps that CM can support  */

#define CMIPAPP_CAP_IDX_INVALID ( 0xFF)
/* Invalid index in the APP capabilities array*/

#define CMIPAPP_TRANS_ID_NONE        (cmipapp_trans_id_type) (-1)
    /* CM-APP Trans id initializer */

#define CMIPAPP_ID_BUFFER                        64
    /* Maximum length of identity buffer */

#define CMIPAPP_VAL_DEFAULT_DATA_BLOCK          0

/* This unit of state indicating the IP App capability. This data can be
** accessed by both IP App and CM. Should be updated in
** cmipapp_crit_sect_ptr() critical section */
typedef struct cmipapp_app_capability_s
{
  cmipapp_call_type_e_type          call_type[CMIPAPP_CAP_ARR_MAX];
    /* Array of app call types supported
    */

  cmipapp_sys_mode_e_type           sys_mode[CMIPAPP_CAP_ARR_MAX];
    /* Array of System modes app can support for calls
       Corresponding to the same call type in call type array
    */
}cmipapp_app_capability_s_type;

/* Stores records for registered apps
*/
typedef struct cmipapp_s {

  cmipapp_id_type                   app_id;
    /* App ID
    */

  cmipapp_app_capability_s_type     app_capability;
    /* IP App capability as advertised by it
    */

  cmipapp_run_status_e_type         app_run_status;
   /* CMIPAPP_RUN_STATUS_ACTIVE  - App is active
   ** CMIPAPP_RUN_STATUS_INACTIVE  - App is inactive
   */


  cmipapp_cb_s_type                  app_cb;
   /* Structure holds function pointers that
   ** are registered by Apps. CM calls these
   ** functions for call related services
   */

  cmipapp_cap_reg_reason_s_type      reason;
    /* reason carries details of success or failure  
    ** with respect to app registeration with network.
    */  

  cmipapp_trans_id_type              active_trans_id;
    /* Trasaction id sent to apps along with
    ** oprt_mode or srv_domain. This id has to be returned
    ** when apps call cmipapp_oprt_mode_cnf or cmipapp_srv_domain_cnf
    ** One trans id can be shared by both indications since 
    ** a) CM processes ph commands one after another
    ** b) CM ensures that the trans id is cleared before moving to the next cmd
    */

  sys_oprt_mode_e_type               oprt_mode_sent;
    /* Operating mode that was last sent
    ** to apps.
    */

	 cm_ph_cmd_e_type									 subs_cmd ;                 
    /* Subscription cmd  that was last sent
    ** to apps.
    */

  cm_srv_domain_pref_e_type          srv_domain_sent;
    /* Service domain pref that was last sent
    ** to apps.
    */

  uint32                             user_data_block;
    /* User token passed to CM at registration. CM will
    ** return it back in callback and other notification 
    ** function
    */

} cmipapp_s_type;


#if defined(FEATURE_IP_CALL)

/* Utility to create combination of app capabilities. Maintaining it for
** backward compatibility with old clients. New approach is to directly
** use ADD_CALL_TYPE_MASK from cm.h
*/
#define ADD_CALLTYPE(X,Y)  (cmipapp_call_type_e_type) ADD_CALL_TYPE_MASK(X,Y)

/**--------------------------------------------------------------------------
** Datatypes - Report structures
** --------------------------------------------------------------------------
*/

/* The header is always present and
** it specifies the command type and attributes.  If the command has
** arguments they follow in the union.
*/
typedef struct {

  /* Header for reports to CM
  */
  cm_hdr_type      hdr;

  /* Union of all possible indications from lower layers
  */
  cmipapp_ind_s_type ip_ind;


  /* Token created and used by registered apps.
  ** App gives CM this value whenever cmipapp_rpt_ind () is called.
  ** CM passes the most recent value given by app while calling
  ** app registered call back functions
  */
  uint32             data_block;


} cmipapp_rpt_s_type;



/**--------------------------------------------------------------------------
**   CM IP APP INTERFACE FUNCTIONS
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmipapp_inform_srv_system_change

DESCRIPTION
  Clients that have a registered function for being informed of service
  change, get notified.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cmipapp_inform_srv_system_change (

   const cm_mm_msim_ss_info_s_type *ss_info_ptr
     /* Serving system information */
);

/*===========================================================================

FUNCTION cmipapp_send_oprt_mode

DESCRIPTION
  Send Phone's operating mode to IP app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  TRUE is send success

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern boolean cmipapp_send_oprt_mode
(
  sys_oprt_mode_e_type  oprt_mode,
    /* Operating mode
    */
  boolean wait_for_cleanup
);

/*===========================================================================

FUNCTION cmipapp_send_subsc_cmd

DESCRIPTION
  Send Phone's subs unavailable cmd  to IP app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  TRUE is send success

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmipapp_send_subsc_cmd
(
  cm_ph_cmd_e_type  subs_cmd,
    /* subs cmd  
    */

  uint8                   subs_id_bitmask,
    /* Bitmask of the subs_id impacted. Based on SYS_MODEM_AS_ID_MASK_*
    */

  uint8                     reason_mask[]

);


/*===========================================================================

FUNCTION cmipapp_send_srv_domain

DESCRIPTION
  Send Phone's srv_domain to IP app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipapp_send_srv_domain
(
  cm_srv_domain_pref_e_type  srv_domain,
    /* srv_domain
    */
   sys_modem_as_id_e_type asubs_id
);


/*===========================================================================

FUNCTION cmipapp_is_wait_on_oprt_mode_cnf

DESCRIPTION
  Checks if ipapp would need to deregister on this request and
  also if inform_oprt_mode callback is registered.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern boolean cmipapp_is_wait_on_oprt_mode_cnf
(
  sys_oprt_mode_e_type  oprt_mode
    /* Operating mode
    */
);



/*===========================================================================

FUNCTION cmipapp_is_wait_on_srv_domain_cnf

DESCRIPTION
  Checks if ipapp would need to deregister on this request and
  also if inform_srv_domain callback is registered.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern boolean cmipapp_is_wait_on_srv_domain_cnf
(
  cm_srv_domain_pref_e_type  srv_domain
    /* Service domain pref
    */
);


/*===========================================================================

FUNCTION cmipapp_is_srv_dom_ps_detach

DESCRIPTION
Checks if CM sent PS-Detach to ipapp 

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
extern boolean cmipapp_is_srv_dom_ps_detach();

/*===========================================================================

FUNCTION cmipsups_rpt_proc

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmipsups_rpt_proc(

    const cm_hdr_type   *rpt_ptr
        /* Pointer to a IP app report */
);

/*===========================================================================

FUNCTION cmsups_client_cmd_forward_ip

DESCRIPTION
  Forward a client sups commands to IMS.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from MN before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmipsups_client_cmd_forward_ip(

    cmsups_s_type  *sups_ptr,
            /* Pointer to the sups object */

    cm_sups_cmd_s_type    *in_sups_cmd_ptr
        /* Pointer to a CM command */
);

/*===========================================================================

FUNCTION cmipsups_route_check

DESCRIPTION
  Check if routing supps command to IMS possible

DEPENDENCIES
  none

RETURN VALUE
  cmsups_cmd_route_e_type

SIDE EFFECTS
  none

===========================================================================*/

extern cmsups_cmd_route_e_type cmipsups_route_check(
  cm_sups_cmd_e_type sups_cmd,
  sys_modem_as_id_e_type sub_id
);

/*===========================================================================

FUNCTION cmipapp_release_reference

DESCRIPTION
Release reference to the reference counted object

DEPENDENCIES

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmipapp_release_reference
(

 const cmipapp_rpt_s_type  *cmipapp_rpt_ptr
 /* Pointer to a IP app report */
);

#endif /* defined(FEATURE_IP_CALL) */

/*===========================================================================

FUNCTION cmipapp_init

DESCRIPTION
  Initializes table to maintain app details. Reads phone identity

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cmipapp_init) */  
extern void cmipapp_init(void);

/*===========================================================================

FUNCTION cmipapp_find_active_app

DESCRIPTION
  Finds an active app with a particular sys_mode and call type

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_id_type cmipapp_find_active_app (

  sys_sys_mode_e_type     sys_mode,
    /* The system mode
    */

  cm_call_type_e_type     call_type
    /* call type
    */

);

/*===========================================================================

FUNCTION cmipapp_find_active_app_sys_mode

DESCRIPTION
  Finds an active app with a particular call type

DEPENDENCIES
  None

RETURN VALUE

  App id

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type  cmipapp_find_active_app_sys_mode (

  cm_call_type_e_type     call_type
    /* call type
    */

);


/*===========================================================================

FUNCTION cmipapp_reset_trans_id

DESCRIPTION
  Reset trans id being used for tracking app's oprt mode cnf

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmipapp_reset_trans_id (void);

/*===========================================================================

FUNCTION cmipapp_set_user_cb_data

DESCRIPTION
  Sets user data block to IP App data.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  CMIPAPP_STATUS_GET_CB_PTRS_FAILED - if o/p variable don't have valid data.
  CMIPAPP_STATUS_OK - Output variables have valid data

SIDE EFFECTS
  None
===========================================================================*/
void cmipapp_set_user_cb_data
(
  cmipapp_id_type   app_id,
  uint32            user_data_block
);

/*===========================================================================

FUNCTION cmipapp_map_sysmode_to_cm_sysmode

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  None.

RETURN VALUE
  CM system mode that corresponds to App system mode

SIDE EFFECTS
  none

===========================================================================*/
extern sys_sys_mode_e_type cmipapp_map_sysmode_to_cm_sysmode(

    cmipapp_sys_mode_e_type      sys_mode
        /* sys mode */
);

/*===========================================================================

FUNCTION cmipapp_release_reference_end_reason

DESCRIPTION
Release reference to the reference counted object during call end 

DEPENDENCIES

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmipapp_release_reference_end_reason
(  
  cmcall_s_type           *call_ptr 
);

/*===========================================================================
FUNCTION cmcall_end_cause_to_cmipcall_end_cause

DESCRIPTION
  Map CM End Cause to CMIPAPP End Cause.

DEPENDENCIES
  None

RETURN VALUE
  CMIPAPP End CAuse

SIDE EFFECTS
  none
===========================================================================*/
cmipapp_end_cause_e_type cmcall_end_cause_to_cmipcall_end_cause
(
  cm_call_end_e_type  cmcall_end_cause
);

/*===========================================================================
FUNCTION cmipapp_is_active_vops

DESCRIPTION
  Finds if there are active app with vops call

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  retun TRUE if IMS registered for Voice/VT or SMS over WiFi

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmipapp_is_active_wlan_apps(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CMIPAPPI_H */
