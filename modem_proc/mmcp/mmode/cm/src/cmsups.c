/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

C A L L   M A N A G E R


S U P P L E M E N T A R Y   S E R V I C E   M O D U L E

GENERAL DESCRIPTION
This module contains the Call Manager Sups Object functions for WCDMA/GSM and IP.


EXTERNALIZED FUNCTIONS

cmsups_init
Initializing the sups object.

cmsups_client_cmd_proc
Process call independent sups commands from the clients.

cmsups_rpt_proc
Process Lower Layer sups notifications/command replies.


INITIALIZATION AND SEQUENCING REQUIREMENTS
cmsups_init() must be called to initialize this module before any other
function declared in this module is being called.


Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsups.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/13   sm      Add error string for IP SUPS CONF 
11/22/12   xs      Added PS supplemetary service support
===========================================================================*/
#include "cm.h"
#include "cmsups.h"
#include "cmi.h"
#include "cmclient.h"
#include "cmtaski.h"
#include "cmidbg.h"
#include "cmph.h"
#include "cmwsups.h"
#if defined (CM_GW_SUPPORTED) || defined (FEATURE_IP_CALL)
/*===========================================================================

FUNCTION CMSUPS_PTR

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
cmsups_s_type  *cmsups_ptr( void )
{
  static cmsups_s_type cmsups;
  /* The one and only sups object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmsups;
}

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

void cmsups_init_sups_info(cm_sups_info_s_type * sups_info_ptr)
{

  /* Initialize sups parameters
  */
  memset (sups_info_ptr, CM_DEFAULT_VALUE, sizeof(cm_sups_info_s_type));

  /* client id */
  sups_info_ptr->sups_client_id                 = CM_CLIENT_ID_ANONYMOUS;

  /* Reset sups cmd reference  field */
  sups_info_ptr->ss_ref                         = CM_SS_REF_UNASSIGNED;

  /* Reset ss code  field */
  sups_info_ptr->ss_code                        = CM_SS_CODE_UNASSIGNED;

  /* Reset ss operation field */
  sups_info_ptr->ss_operation                   = CM_SS_OPERATION_UNASSIGNED;

  /* Reset ss invoke id field */
  sups_info_ptr->invoke_id                      = CM_SS_INVOKE_ID_UNASSIGNED;


  /* Reset guidance_info */
  sups_info_ptr->guidance_info                  = enterPW;

  /* Reset conf_type to no info */
  sups_info_ptr->conf_type                      = NO_INFO;

  /* Reset uss_data_type */
  sups_info_ptr->uss_data_type                  = CM_USS_DATA_NONE;

#if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))

  /* LCS MOLR Response does not carry any data */
  sups_info_ptr->lcs_molr_res.data_type              = NO_INFO;

#endif

  /* Reset client id */
  sups_info_ptr->requesting_client_id           = CM_CLIENT_ID_ANONYMOUS;

  /* Reset ccbs indx */
  sups_info_ptr->ccbs_indx                      = CM_CCBS_INDX_INVALID;
  
  #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
  sups_info_ptr->sups_ss_error_text = NULL;
  sups_info_ptr->sups_time_info.time_present = FALSE;
  #endif

}


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
cm_sups_info_s_type  *cmsups_info_alloc( void )
{
  cm_sups_info_s_type  *sups_info_ptr;

  /*
  ** Dynamically allocate a sups info object
  */
  sups_info_ptr = (cm_sups_info_s_type *)cm_mem_malloc(
    sizeof(cm_sups_info_s_type));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize sups parameters
  */

  cmsups_init_sups_info(sups_info_ptr);

  /* Return a pointer to the dynamically allocated sups info buffer. */

  return sups_info_ptr;

}

/*===========================================================================

FUNCTION cmsups_uss_info_init

DESCRIPTION
Initialize sups info buffer.

DEPENDENCIES
sups_info_ptr should be a valid non-NULL pointer

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmsups_uss_info_init(
  cm_sups_uss_data_info_s_type     *uss_info
)
{
  /* Initialize USS parameters
  */
  memset (uss_info, CM_DEFAULT_VALUE, sizeof(cm_sups_uss_data_info_s_type));
  /* Reset ss operation field */
  uss_info->ss_operation                   = CM_SS_OPERATION_UNASSIGNED;

  /* Reset ss invoke id field */
  uss_info->invoke_id                      = CM_SS_INVOKE_ID_UNASSIGNED;

  /* Reset sups cmd reference  field */
  uss_info->ss_ref                         = CM_SS_REF_UNASSIGNED;

  /* Reset sups cmd reference  field */
  uss_info->uss_res_type                   = CM_USS_RES_SS_NONE;

} /* cmsups_uss_info_init() */


/*===========================================================================

FUNCTION cmsups_uss_info_alloc

DESCRIPTION
Allocate a sups info buffer.

DEPENDENCIES
none

RETURN VALUE
Pointer to allocated sups info struct.

SIDE EFFECTS
none

===========================================================================*/
cm_sups_uss_data_info_s_type *cmsups_uss_info_alloc( void )
{
  cm_sups_uss_data_info_s_type     *uss_info;

  /* Dynamically allocate a uss info object
  */
  uss_info = (cm_sups_uss_data_info_s_type *)cm_mem_malloc(sizeof(cm_sups_uss_data_info_s_type));

  /* Initialize sups parameters
  */
  cmsups_uss_info_init(uss_info);

  /* Return a pointer to the dynamically allocated uss info buffer */
  return uss_info;
} /* cmsups_uss_info_alloc() */


/*===========================================================================

FUNCTION cmsups_uss_info_cmd_copy_fields

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
void cmsups_uss_info_cmd_copy_fields(
  cm_sups_uss_data_info_s_type    *uss_info_ptr,
    /* Pointer to info part of sups object */
  const cm_sups_cmd_info_s_type   *cmd_info_ptr
    /* Pointer to a sups command */
)
{
  if (uss_info_ptr == NULL || cmd_info_ptr == NULL)
  {
    return;
  }
  
  /* SS reference */
  uss_info_ptr->ss_ref = cmd_info_ptr->ss_ref;

  /* SS invoke ID */
  uss_info_ptr->invoke_id = cmd_info_ptr->invoke_id;

  /* SS operation */
  uss_info_ptr->ss_operation = cmd_info_ptr->ss_operation;

  /* uss response type */
  uss_info_ptr->uss_res_type = cmd_info_ptr->uss_res_type;

  /* SS error type */
  uss_info_ptr->ss_error = cmd_info_ptr->ss_error;
  
  /* USS data needs to be filled in case of PROCESS_USS_RES and PROCESS_USS */
  uss_info_ptr->uss_data = cmd_info_ptr->uss_data;

  uss_info_ptr->asubs_id = cmd_info_ptr->cmd_subs_id;
  
  return;
} /* cmsups_uss_info_cmd_copy_fields() */


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
)
{
  cm_sups_uss_data_info_s_type  **uss_info = NULL;
  cm_ss_e_type                    ss;

#ifdef CM_GW_SUPPORTED
  if ( cmph_is_subs_feature_mode_sglte( asubs_id ) )
  {
    cmwsups_sglte_determine_ss_for_sups_cmd( asubs_id, sups_cmd, &ss );
  }
  else
#endif
  {
    ss = cmph_map_subs_to_ss( asubs_id );
  }


  if( ss == CM_SS_HYBR_2 )
  {
    uss_info = &cmsups_ptr()->hybr_gw_uss_info;
  }
  else
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( ss == CM_SS_HYBR_3 )
  {
    uss_info =  &cmsups_ptr()->hybr_gw3_uss_info;
  }
  else
#endif
  {
    uss_info =  &cmsups_ptr()->uss_info;
  }

  if (*uss_info == NULL) {
    *uss_info = cmsups_uss_info_alloc();
  }

  return *uss_info;
} /* cmsups_get_uss_info_ptr() */


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
)
{
  cm_ss_e_type                    ss       = cmph_map_subs_to_ss(asubs_id);

  if( ss == CM_SS_HYBR_2 && cmsups_ptr()->uss_info != NULL)
  {
    cm_mem_free(cmsups_ptr()->hybr_gw_uss_info);
    cmsups_ptr()->hybr_gw_uss_info = NULL;
  }
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  else if( ss == CM_SS_HYBR_3 && cmsups_ptr()->hybr_gw3_uss_info != NULL)
  {
    cm_mem_free(cmsups_ptr()->hybr_gw3_uss_info);
    cmsups_ptr()->hybr_gw3_uss_info = NULL;
  }
#endif
  else if(cmsups_ptr()->uss_info != NULL)
  {
    cm_mem_free(cmsups_ptr()->uss_info);
    cmsups_ptr()->uss_info = NULL;
  }

  return;
} /* cmsups_free_uss_info_ptr()*/


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
)
{
  cm_sups_cmd_mt_ussd_context_info_s_type  *mt_uss_info = NULL;
  cm_ss_e_type  ss = cmph_map_subs_to_ss( asubs_id );

  if( ss == CM_SS_HYBR_2 )
  {
    mt_uss_info = &cmsups_ptr()->hybr_gw_mt_uss_info;
  }
  else
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( ss == CM_SS_HYBR_3 )
  {
    mt_uss_info =  &cmsups_ptr()->hybr_gw3_mt_uss_info;
  }
  else
#endif
  {
    mt_uss_info =  &cmsups_ptr()->mt_uss_info;
  }

  return mt_uss_info;
} /* cmsups_get_mt_uss_context_info_ptr() */

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
)
{
  cm_sups_cmd_mt_ussd_context_info_s_type  *mt_uss_info = 
    cmsups_get_mt_uss_context_info_ptr( asubs_id );
  
  mt_uss_info->is_sups_cmd_mt_ussd_active = FALSE;
  return;
}

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
)
{
  cm_sups_cmd_mt_ussd_context_info_s_type  *mt_uss_info = 
    cmsups_get_mt_uss_context_info_ptr( asubs_id );
  
  mt_uss_info->is_sups_cmd_mt_ussd_active = TRUE;
  mt_uss_info->sups_cmd_mt_ussd_route = route;
  return;
}

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
)
{
  cm_sups_cmd_mt_ussd_context_info_s_type  *mt_uss_info = 
    cmsups_get_mt_uss_context_info_ptr( asubs_id );

  *route = mt_uss_info->sups_cmd_mt_ussd_route;
  return mt_uss_info->is_sups_cmd_mt_ussd_active;
}



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
  )
{
  if( ss == CM_SS_HYBR_2 )
  {
    return cmsups_ptr()->hybr_gw_sups_info;
  }
  else
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( ss == CM_SS_HYBR_3 )
  {
    return cmsups_ptr()->hybr_gw3_sups_info;
  }
  else
 #endif
  {
    return cmsups_ptr()->sups_info;
  }
}

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
)
{
  return cmsups_get_sups_info_ptr(cmph_map_subs_to_ss(asubs_id));
}

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
)
{
  cmsups_s_type         *sups_ptr = cmsups_ptr();
  cm_sups_info_s_type   *sups_info_ptr = cmsups_info_alloc();

  if ( ss == CM_SS_HYBR_2 )
  {
    sups_ptr->hybr_gw_sups_info = sups_info_ptr;
  }
  else
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if ( ss == CM_SS_HYBR_3 )
  {
    sups_ptr->hybr_gw3_sups_info = sups_info_ptr;
  }
  else
  #endif /* FEATURE_MMODE_TRIPLE_SIM */
  {
    sups_ptr->sups_info = sups_info_ptr;
  }

  return sups_info_ptr;
}

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
)
{
  cm_sups_info_s_type* sups_info_ptr = cmsups_get_sups_info_ptr_asubs_id(asubs_id);

  if(sups_info_ptr == NULL)
  {
    sups_info_ptr = cmsups_alloc_sups_info_ptr(cmph_map_subs_to_ss(asubs_id));
  }

  return sups_info_ptr;
}

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
boolean cmsups_info_get(

                        cm_ss_e_type                 ss,
                        /* stack whose sups info needs to be retrieved */

                        cm_sups_info_s_type          *sups_info_ptr
                        /* Copy the event info into this buffer */

                        )
{

  cmsups_s_type       *sups_ptr  =  cmsups_ptr();
  /* get the SUPS object */

  cm_sups_info_s_type  *info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_info_ptr != NULL );
  CM_ASSERT( sups_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(sups_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  info_ptr = cmsups_get_sups_info_ptr(ss);


  /* If we have not allocated memory for info_ptr, that means
  ** no sups info need to be copied. Return from here.
  */
  if ( info_ptr == NULL )
  {
    /* Return boolean indicating whether the
    * information is valid or not
    */
    return (CM_BOOLEAN( CM_INIT_CHECK(sups_ptr->init_mask) ) );
  } 


  /* Copy sups parameters into sups_info struct from the sups object.
  */
  *sups_info_ptr = *info_ptr;

  sups_info_ptr->sups_client_id = cmclient_get_client_id( sups_ptr->sups_client_ptr );

  sups_info_ptr->asubs_id = cmsups_get_asubs_id(info_ptr);

  CM_MSG_HIGH_4("mod_ss_params: cc result %d, call_id %d, length %d, cc_result_alpha %d",
                 sups_info_ptr->mod_ss_params.call_control_result,
                sups_info_ptr->mod_ss_params.call_id,
                sups_info_ptr->cc_result_alpha.len,
                sups_info_ptr->cc_result_alpha.buf[0]);

  /* Return boolean indicating whether the
  * information is valid or not
  */
  return (CM_BOOLEAN( CM_INIT_CHECK(sups_ptr->init_mask) ) );
}

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
)
{
  if (sups_info_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
    return SYS_MODEM_AS_ID_NONE;
  }

  if (cmph_is_msim())
  {
    return sups_info_ptr->asubs_id;
  }
  else
  {
    return SYS_MODEM_AS_ID_1;
  }
}

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
)
{
  cmsups_s_type *sups_ptr = cmsups_ptr();

  if(sub_id <= SYS_MODEM_AS_ID_NONE || sub_id >= MAX_SIMS)
  {
    CM_MSG_HIGH_1("cmsups_get_srv_domain_pref: invalid sub_id %d", sub_id);
    sub_id = SYS_MODEM_AS_ID_1;
  }

  return sups_ptr->sups_svc_domain_pref[sub_id];
}

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
void cmsups_event(

                  cm_ss_e_type            ss,
                  /* stack whose info is being sent */

                  cm_sups_event_e_type    sups_event
                  /* notify client list of this sups event */
                  )
{
  cm_sups_info_s_type *sups_info_ptr = cmsups_info_alloc();
  /* Allocate a sups info buffer to copy the sups event info*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(sups_event, CM_SUPS_EVENT_NONE, CM_SUPS_EVENT_MAX) );
  CM_ASSERT( sups_info_ptr != NULL );
  CM_MSG_HIGH_2( "SUPS event=%d on ss %d",sups_event, ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy all information into
  ** allocated sups info buffer.
  */
  (void) cmsups_info_get( ss, sups_info_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Notify clients list of sups event.
  */
  cmclient_list_sups_event_ntfy( sups_event, sups_info_ptr );

  /* Free a sups info buffer after sending sups event info */
  cm_mem_free(sups_info_ptr);

}

/*===========================================================================

FUNCTION cmsups_is_uss_sups_cmd

DESCRIPTION
  Checks if SUPS cmd is of USSD type

DEPENDENCIES
  None

RETURN VALUE
  TRUE if cmd type is USS type

SIDE EFFECTS
  None

===========================================================================*/
boolean cmsups_is_uss_sups_cmd(
  cm_sups_cmd_e_type cmd
)
{
  if(cmd == CM_SUPS_CMD_PROCESS_USS ||
     cmd == CM_SUPS_CMD_USS_RES ||
     cmd == CM_SUPS_CMD_USS_NOTIFY_RES ||
     cmd == CM_SUPS_CMD_RELEASE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* cmsups_is_uss_sups_cmd() */


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

                                      )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_info_ptr != NULL )

  if (sups_info_ptr == NULL || cmd_info_ptr == NULL)
  {
    CM_ERR_2("sups_info_ptr/cmd_info_ptr NULL ptr", sups_info_ptr, cmd_info_ptr);
    return;
  }
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* SS Code */
    if( cmd_info_ptr->ss_code != CM_SS_CODE_UNASSIGNED )
    {  
      sups_info_ptr->ss_code = cmd_info_ptr->ss_code;
    }

    /* SS operation */
    if( cmd_info_ptr->ss_operation != CM_SS_OPERATION_UNASSIGNED )
    {  
      sups_info_ptr->ss_operation = cmd_info_ptr->ss_operation;
    }

    /* SS reference */
    if( cmd_info_ptr->ss_ref != CM_SS_REF_UNASSIGNED )
    {  
      sups_info_ptr->ss_ref = cmd_info_ptr->ss_ref;
    }

    /* SS invoke ID */
    if( cmd_info_ptr->invoke_id != CM_SS_INVOKE_ID_UNASSIGNED )
    {  
      sups_info_ptr->invoke_id = cmd_info_ptr->invoke_id;
    }

    /* Basic service */
    if( cmd_info_ptr->basic_service.present )
    {  
      sups_info_ptr->basic_service =  cmd_info_ptr->basic_service;
    }

    /* Forwarded to Number */
    if( cmd_info_ptr->forwarded_to_number.present )
    {  
      sups_info_ptr->forwarded_to_number = cmd_info_ptr->forwarded_to_number;
    }

    /* Forwarded to subaddress */
    if( cmd_info_ptr->forwarded_to_subaddress.present )
    {  
      sups_info_ptr->forwarded_to_subaddress =  cmd_info_ptr->forwarded_to_subaddress;
    }

    /* No Reply Condition Timer */
    if( cmd_info_ptr->nr_timer.present)
    {  
      sups_info_ptr->nr_timer = cmd_info_ptr->nr_timer;
    }

    /* SS password */
    if( cmd_info_ptr->ss_password.present )
    {  
      sups_info_ptr->ss_password = cmd_info_ptr->ss_password;
    }

    /* SS new password */
    if( cmd_info_ptr->ss_new_password.present)
    {  
      sups_info_ptr->ss_new_password = cmd_info_ptr->ss_new_password;
    }

    /* SS new password confirmed */
    if( cmd_info_ptr->ss_new_password_again.present)
    {  
      sups_info_ptr->ss_new_password_again = cmd_info_ptr->ss_new_password_again;
    }

    if( cmd_info_ptr->code.present )
    {  
      sups_info_ptr->code = cmd_info_ptr->code;
    }

  if( cmd_info_ptr->call_barring_num_list)
  {
	sups_info_ptr->call_barring_num_list = cmd_info_ptr->call_barring_num_list;
        CM_MSG_HIGH_0("Copied call_barring_num_list to sups_info_ptr");
  }

    /* uss response type */
    if( cmd_info_ptr->uss_res_type == CM_USS_RES_SS_ERROR )
    {
      sups_info_ptr->ss_error = cmd_info_ptr->ss_error;
      sups_info_ptr->conf_type = SS_ERROR_INFO;
    }
    else if ( cmd_info_ptr->uss_res_type == CM_USS_RES_SS_DATA )
    {
      sups_info_ptr->uss_data = cmd_info_ptr->uss_data;
      sups_info_ptr->conf_type = USS_DATA_INFO;
    }

    CM_MSG_HIGH_3 ("Copied uss data res type: %d conf type: %d error: %d", 
                   cmd_info_ptr->uss_res_type, 
                   sups_info_ptr->conf_type,
                   sups_info_ptr->ss_error.error_code);

    /* Copy ccbs index supplied by client
    */
    if (cmd_info_ptr->ccbs_indx != CM_CCBS_INDX_INVALID)
    {
      sups_info_ptr->ccbs_indx = cmd_info_ptr->ccbs_indx;
    }

    sups_info_ptr->asubs_id = cmd_info_ptr->cmd_subs_id;

} /* cmsups_cmd_copy_fields_into_info () */

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
void cmsups_cmd_copy_fields(

                            cmsups_s_type               *sups_ptr,
                            /* Pointer to the sups object */

                            const cm_sups_cmd_s_type    *cmd_ptr
                            /* Pointer to a sups command */

                            )
{
  cm_sups_info_s_type               *sups_info_ptr;

  cm_ss_e_type                       sups_cmd_ss = CM_SS_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_ptr != NULL )
  CM_ASSERT( cmd_ptr != NULL )

  if (sups_ptr == NULL || cmd_ptr == NULL)
  {
    CM_ERR_2("sups_ptr or cmd_ptr NULL ptr", sups_ptr, cmd_ptr);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  {
    sups_cmd_ss = cmph_map_subs_to_ss( cmd_ptr->cmd_info.cmd_subs_id );

    #ifdef CM_GW_SUPPORTED
    if ( cmph_is_subs_feature_mode_sglte( cmd_ptr->cmd_info.cmd_subs_id ) )
    {
      cmwsups_sglte_determine_ss_for_sups_cmd( cmd_ptr->cmd_info.cmd_subs_id,
                                               cmd_ptr->cmd, &sups_cmd_ss );
    }
    #endif

    sups_info_ptr = cmsups_get_sups_info_ptr( sups_cmd_ss );
  }

  if (sups_info_ptr == NULL)
  {
    CM_ERR_1("sups_info_ptr NULL ptr subs_id = %d", 
                  cmd_ptr->cmd_info.cmd_subs_id);
    return;
  }

  if( sups_cmd_ss == CM_SS_HYBR_3 )
  {
    sups_info_ptr = sups_ptr->hybr_gw3_sups_info;
    #ifdef FEATURE_SUPS_RETRY
    if(sups_ptr->hybr_gw3_sups_info_list != NULL)
    {
      sups_ptr->hybr_gw3_sups_info_list->cmd = cmd_ptr->cmd;
      *(&(sups_ptr->hybr_gw3_sups_info_list->sups_cmd_info)) = *(SUPS_CMD_INFO_PTR( cmd_ptr ));
      sups_ptr->hybr_gw3_sups_info_list->cc_result_alpha = cmd_ptr->cc_result_alpha;
      sups_ptr->hybr_gw3_sups_info_list->cc_result = cmd_ptr->cc_result;
      sups_ptr->hybr_gw3_sups_info_list->sups_client_ptr   = cmd_ptr->client_ptr;
    }
    #endif
  }
  else if( sups_cmd_ss == CM_SS_HYBR_2 )
  {
    sups_info_ptr = sups_ptr->hybr_gw_sups_info;
    #ifdef FEATURE_SUPS_RETRY
    if(sups_ptr->hybr_gw_sups_info_list != NULL)
    {
      sups_ptr->hybr_gw_sups_info_list->cmd = cmd_ptr->cmd;

      *(&(sups_ptr->hybr_gw_sups_info_list->sups_cmd_info)) = *(SUPS_CMD_INFO_PTR( cmd_ptr ));
      sups_ptr->hybr_gw_sups_info_list->cc_result_alpha = cmd_ptr->cc_result_alpha;
      sups_ptr->hybr_gw_sups_info_list->cc_result = cmd_ptr->cc_result;
      sups_ptr->hybr_gw_sups_info_list->sups_client_ptr   = cmd_ptr->client_ptr;
    }
    #endif
  }
  else
  {
    sups_info_ptr = sups_ptr->sups_info;
    #ifdef FEATURE_SUPS_RETRY
    if(sups_ptr->sups_info_list != NULL)
    {
      sups_ptr->sups_info_list->cmd = cmd_ptr->cmd;

      *(&(sups_ptr->sups_info_list->sups_cmd_info)) = *(SUPS_CMD_INFO_PTR( cmd_ptr ));
      sups_ptr->sups_info_list->cc_result_alpha = cmd_ptr->cc_result_alpha;
      sups_ptr->sups_info_list->cc_result = cmd_ptr->cc_result;
      sups_ptr->sups_info_list->sups_client_ptr = cmd_ptr->client_ptr;
    }
    #endif
  }

  /* client information */
  sups_ptr->sups_client_ptr = cmd_ptr->client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmsups_cmd_copy_fields_into_info (sups_info_ptr, SUPS_CMD_INFO_PTR(cmd_ptr) );

  /* Copy USS related info into uss_info_ptr */
  if (cmsups_is_uss_sups_cmd(cmd_ptr->cmd)){
    cmsups_uss_info_cmd_copy_fields(cmsups_get_uss_info_ptr(cmd_ptr->cmd_info.cmd_subs_id, cmd_ptr->cmd), SUPS_CMD_INFO_PTR(cmd_ptr));
  }
  /* Copy Alpha from CC result */
  sups_info_ptr->cc_result_alpha = cmd_ptr->cc_result_alpha;

  /* Callc ontrol result */
  sups_info_ptr->mod_ss_params.call_control_result = cmd_ptr->cc_result;

}

cm_sups_event_e_type cmsups_cmd_to_event_map(cm_sups_cmd_e_type sups_cmd)
{
  switch(sups_cmd)
  {
  case CM_SUPS_CMD_ACTIVATE:
    return CM_SUPS_EVENT_ACTIVATE;
  case CM_SUPS_CMD_DEACTIVATE:
    return CM_SUPS_EVENT_DEACTIVATE;
  case CM_SUPS_CMD_ERASE:
    return CM_SUPS_EVENT_ERASE;
  case CM_SUPS_CMD_INTERROGATE:
    return CM_SUPS_EVENT_INTERROGATE;
  case CM_SUPS_CMD_REGISTER:
    return CM_SUPS_EVENT_REGISTER;
  case CM_SUPS_CMD_PROCESS_USS:
    return CM_SUPS_EVENT_PROCESS_USS;
  case CM_SUPS_CMD_REG_PASSWORD:
    return CM_SUPS_EVENT_REG_PASSWORD;
  case CM_SUPS_CMD_RELEASE:
    return CM_SUPS_EVENT_RELEASE;
  case CM_SUPS_CMD_GET_PASSWORD_RES:
    return CM_SUPS_EVENT_GET_PASSWORD_RES;
  case CM_SUPS_CMD_USS_NOTIFY_RES:
    return CM_SUPS_EVENT_USS_NOTIFY_RES;
  case CM_SUPS_CMD_USS_RES:
    return CM_SUPS_EVENT_USS_RES;
  case CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES:
    return CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES;
  case CM_SUPS_CMD_LCS_MOLR:
    return CM_SUPS_EVENT_LCS_MOLR;
  case CM_SUPS_CMD_LCS_MOLR_COMPLETED:
    return CM_SUPS_EVENT_LCS_MOLR_COMPLETED;
  case CM_SUPS_CMD_ERASE_CC_ENTRY:
    return CM_SUPS_EVENT_DEACTIVATE;
  default:
    CM_ERR_1("cmsups_cmd_map: unsupport sups command %d",sups_cmd);
    return CM_SUPS_EVENT_NONE;
  }
}

cm_sups_event_e_type cmsups_conf_to_event_map(cm_name_type           sups_conf)
{
  switch(sups_conf)
  {
  case CM_IP_SUPS_ACTIVATE_SS_CONF:
    return CM_SUPS_EVENT_ACTIVATE_CONF;
  case CM_IP_SUPS_DEACTIVATE_SS_CONF:
    return CM_SUPS_EVENT_DEACTIVATE_CONF;
  case CM_IP_SUPS_ERASE_SS_CONF:
    return CM_SUPS_EVENT_ERASE_CONF;
  case CM_IP_SUPS_INTERROGATE_SS_CONF:
    return CM_SUPS_EVENT_INTERROGATE_CONF;
  case CM_IP_SUPS_REGISTER_SS_CONF:
    return CM_SUPS_EVENT_REGISTER_CONF;
  case CM_ACTIVATE_SS_CONF:
    return CM_SUPS_EVENT_ACTIVATE_CONF;
  case CM_DEACTIVATE_SS_CONF:
    return CM_SUPS_EVENT_DEACTIVATE_CONF;
  case CM_ERASE_SS_CONF:
    return CM_SUPS_EVENT_ERASE_CONF;
  case CM_INTERROGATE_SS_CONF:
    return CM_SUPS_EVENT_INTERROGATE_CONF;
  case CM_REGISTER_PASSWORD_CONF:
    return CM_SUPS_EVENT_REG_PASSWORD_CONF;
  case CM_REGISTER_SS_CONF:
    return CM_SUPS_EVENT_REGISTER_CONF;
  case CM_PROCESS_UNSTRUCTURED_SS_CONF:
  case CM_IP_SUPS_PROCESS_UNSTRUCTURED_SS_CONF:
    return CM_SUPS_EVENT_PROCESS_USS_CONF;
  case CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF:
    return CM_SUPS_EVENT_PROCESS_USS_CONF;
  case CM_UNSTRUCTURED_SS_NOTIFY_IND:
  case CM_IP_SUPS_UNSTRUCTURED_SS_NOTIFY_IND:
    return CM_SUPS_EVENT_USS_NOTIFY_IND;
  case CM_GET_PASSWORD_IND:
    return CM_SUPS_EVENT_GET_PASSWORD_IND;
  case CM_RELEASE_UNSTRUCTURED_SS_IND:
  case CM_IP_SUPS_RELEASE_UNSTRUCTURED_SS_IND:
    return CM_SUPS_EVENT_RELEASE_USS_IND;
  case CM_LCS_LOCATION_NOTIFICATION_IND:
    return CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND;
  case CM_LCS_MOLR_CONF:
    return CM_SUPS_EVENT_LCS_MOLR_CONF;
  case CM_ERASE_CC_ENTRY_CONF:
    return CM_SUPS_EVENT_DEACTIVATE_CONF;
  case CM_UNSTRUCTURED_SS_IND:
  case CM_IP_SUPS_UNSTRUCTURED_SS_IND:
    return CM_SUPS_EVENT_USS_IND;
  case CM_FORWARD_CHECK_SS_INDICATION_IND:
    return CM_SUPS_EVENT_FWD_CHECK_IND;
  default:
    CM_ERR_1("cmsups_conf_map: unsupport sups cnf %d",sups_conf);
    return CM_SUPS_EVENT_NONE;
  }
}

/*===========================================================================

FUNCTION cmsups_is_there_pending_sups_on_other_subs

DESCRIPTION
Check if there is a pending SUPS on other subs

RETURN VALUE
TRUE: There is pending SUPS on other subs
FALSE: There is no pending SUPS on other subs

SIDE EFFECTS
none

===========================================================================*/

boolean cmsups_is_there_pending_sups_on_other_subs(
  sys_modem_as_id_e_type        asubs_id
)
{
  cmsups_s_type          *sups_ptr         = cmsups_ptr();
  if ( sups_ptr->num_pending_sups_cmds == 0)
  {
    return FALSE;
  }

  if ( sups_ptr->sups_info != NULL &&
       sups_ptr->sups_info->asubs_id != asubs_id )
  {
    return TRUE;
  }

  if ( sups_ptr->hybr_gw_sups_info != NULL &&
       sups_ptr->hybr_gw_sups_info->asubs_id != asubs_id )
  {
    return TRUE;
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if ( sups_ptr->hybr_gw3_sups_info != NULL &&
       sups_ptr->hybr_gw3_sups_info->asubs_id != asubs_id )
  {
    return TRUE;
  }
  #endif

  return FALSE;
}

#endif
