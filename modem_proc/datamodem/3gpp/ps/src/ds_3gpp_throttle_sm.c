/*!
  @file
  ds_3gpp_throttle_sm.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_throttle_sm.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
09/05/11     ss      Changes to support RPM.
08/30/10     hs      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#include "ds_3gppi_utils.h"
#include "modem_mem.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_nv_manager.h"
#include "dsutil.h"
#include "ran.h"
#include "ps_utils.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "ds_3gpp_hdlr.h"
#include "sys_v.h"
#include "ds3gpp_api.h"
#include "ds3gcmif.h"
#include "ds3geventmgr.h"
#include "ds3gpp_api.h"
#include "ds_3gpp_pdn_throt_config.h"
#include "ds_3gpp_srv_req_throt_config.h"
#include "ds_3gpp_cc_sp_throt_config.h"

#include <stringl/stringl.h>
#include "ds_3gpp_pdn_context.h"

#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */
#include "cm_gw.h"

#ifdef FEATURE_DATA_LTE
#include "ds_dsd_apm_ext_i.h"
#include "ds_eps_pdn_context.h"
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_eps_pdn_throttle_sm.h"
#endif
/*===========================================================================

                         GLOBAL VARIABLES

===========================================================================*/


#define THROTLLE_SM_COOKIE 0xbeabeabe

/*-----------------------------------------------------------------------------
  Global critical section needed to protect the SM info.
  The timer call back modified the state info of the Throttle SM. Since this
  happens in a context that is different from the DS task context, we have to
  protect access to the SM info.
-----------------------------------------------------------------------------*/
rex_crit_sect_type ds_3gppi_throttle_sm_crit_sect = {{NULL}};

/*------------------------------------------------------------------------------
  This is the flag that indicates if the power up procedures have been completed
  for the core state machine.
------------------------------------------------------------------------------*/
static boolean ds_3gppi_is_core_sm_init_performed = FALSE;

/*------------------------------------------------------------------------------
  This is a system wide throttling state machine. Any failure (service rejects
  due to DRB set up service requests, service rejects due to PDN connection
  requests or service requests due to Bearer resource allocation requests) can
  be fed into this state machine.

  Whenever a service request is to be sent out (arising out of MO Data or DS
  level signaling requests), NAS will consult DS to find out if it can send
  the request out on that PLMN. DS will consult its throttling SM based on the
  PLMN provided by NAS in the API just mentioned and return status to NAS
  accordingly.

  This is the pointer to the system wide service request throttling state 
  machine. It is initialized with NULL. However, during power up, memory is
  allocated from modem heap and assigned to this pointer
------------------------------------------------------------------------------*/
static ds_3gpp_throttle_sm_s* 
         ds_3gpp_service_req_throt_sm_ptr[DS3GSUBSMGR_SUBS_ID_MAX] = {NULL};

/*------------------------------------------------------------------------------
Specifies the behavior for the service request throttle SM. It can be configured
via EFS to either block a PLMN or disable LTE selection when we reach a threshold 
number of failed service requests */
static ds_3gpp_srv_req_throttle_behavior_e ds_3gpp_srv_req_throttle_behavior[DS3GSUBSMGR_SUBS_ID_MAX]
                                             = {DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN};
/*initializing to  DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN works as it is zero*/
/*------------------------------------------------------------------------------
specifies whether global throttling enabled or not
-------------------------------------------------------------------------------*/
static ds_3gpp_throt_sys_sel_retry_sm_s ds_3gpp_throt_sys_sel_retry_sm = {NULL};
static boolean ds_3gpp_cc_sp_throttling[DS3GSUBSMGR_SUBS_ID_MAX] = {FALSE};
/*initializing to  FALSE works as it is zero*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_TIMER_CB
 
DESCRIPTION
  This function is the timer call back for the core state machine timer.
  This changes the state of the SM from throttled to unthrottled. Since this
  runs in the timer context
  
  
PARAMETERS  
  callback_data - points to the core throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
LOCAL void ds_3gppi_throt_sm_timer_cb
(
  unsigned long callback_data
);


/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_TIMER_EXP_CMD_HDLR
 
DESCRIPTION
  This function is used to process the throttling timer expiry
  
  
PARAMETERS  
  callback_data - points to the core throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  
===========================================================================*/
LOCAL void ds_3gppi_throt_sm_timer_exp_cmd_hdlr
(
  unsigned long callback_data
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_RETRY_TIMER_EXPIRY

DESCRIPTION
  This function posts a cmd to retry sys selection enable/disable

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_retry_timer_expiry
(
     unsigned long callback_data /*unused*/
);


/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_NO_PLMN_BLOCKING
DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs throttling without plmn blocking.
 
PARAMETERS  
  sm_ptr - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
static void  ds_3gpp_throt_sm_no_plmn_blocking
(
  void* sm_ptr,
  void* cb_data
);


/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_BLOCK_PLMN
DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs plmn blocking.
 
PARAMETERS  
  sm_ptr - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void ds_3gpp_throt_sm_block_plmn
(
  void* sm_ptr,
  void* cb_data
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_BLOCK_PLMN_PREF_T3402

DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs plmn blocking with preference 
  t3402 and resets the core SM.
 
PARAMETERS  
  void * - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void ds_3gpp_throt_sm_block_plmn_pref_t3402
(
  void * sm_ptr,
  void * cb_data
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_NO_PLMN_BLOCKING_NO_THROTTLING
DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs no throttling and no plmn blocking.
 
PARAMETERS  
  sm_ptr - Pointer to the core throttling SM
  cb_data- Callback data 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
static void  ds_3gpp_throt_sm_no_plmn_blocking_no_throttling
(
  void* sm_ptr,
  void* cb_data
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_ENABLE_LTE_SELECTION

DESCRIPTION
  This function is called by the core throttlig machine when the
  throttle_timer expires. This function enables LTE selection.
 
PARAMETERS  
  void * - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void ds_3gpp_throt_sm_enable_lte
(
  void *sm_ptr,
  void * cb_data
);

/*--------------------------------------------------------------------------
  If the global throttling NV is not enabled, pdn_throttle.failure_timers from 
  7 - 10 will be updated with the 6th failure timer value. so even if we increase 
  counter to 10 for 7-10 we will get timer value as 6th failure value only
----------------------------------------------------------------------------*/
#define DS_3GPP_THROTTLE_SM_INCREMENT_FAILURE_COUNTER(count) \
  {\
    if (count < DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS)\
      count++;\
  }

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_FIND_FREE_INDEX_FOR_CORE_SM

DESCRIPTION
  This function checks if the PLMN passed into this function is non-NULL
  
PARAMETERS  
  wrapper_sm_ptr - The wrapper sm that houses several core sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0 - (DS_3GPP_MAX_PLMN_SUPPORTED-1) is a free index can be located
  DS_3GPP_MAX_PLMN_SUPPORTED if no free entry can be located

SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_3gpp_throt_sm_find_free_index_for_core_sm
(
  ds_3gpp_throttle_sm_s* wrapper_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_CORE_SM_FROM_PLMN

DESCRIPTION
  This function is used to get the core throttling state machine given a 
  PLMN ID and the wrapper state machine

PARAMETERS  
  wrapper_sm_ptr - The wrapper sm that houses several core sm
  plmn_id        - The PLMN Id used to look up the core throt sm
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  index into the core sm pointer array
  value between 0 and DS_3GPP_MAX_PLMN_SUPPORTED if core sm found
  DS_3GPP_MAX_PLMN_SUPPORTED if core sm is not found
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_3gpp_throt_sm_get_core_sm_from_plmn
(
  ds_3gpp_throttle_sm_s  *wrapper_sm_ptr,
  sys_plmn_id_s_type     plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_IS_PLMN_NON_NULL

DESCRIPTION
  This function checks if the PLMN passed into this function is non-NULL
  
PARAMETERS  
  plmn_id - PLMN ID to be checked
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If PLMN is non-NULL
  FALSE - otherwise

SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL boolean ds_3gpp_throt_sm_is_plmn_non_null
(
  sys_plmn_id_s_type plmn_id
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_IS_REL_10_THROTTLE

DESCRIPTION
  This function is used to check whether release 10 throttling mechanism
  is being performed or not. Release 10 throttling will kick in only for
  perticular cause codes so we will check only those cause codes.

PARAMETERS    
  cause_code - cause_code indicated in reject message.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE - thrpttle sm is performing Rel 10 throttling.
  FALSE - otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL boolean ds_3gppi_throt_sm_is_rel_10_throttle
(
  void *cause_code,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_CORE_SM_PTR
 
DESCRIPTION
  This function fetches the Core SM Ptr corresponding to a particular index
  and subs id.
  
PARAMETERS   : 
  1. Core SM Index
  2. Subs Id

DEPENDENCIES
  None.

RETURN VALUE
  Throt SM Ptr
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gppi_throttle_sm_s* ds_3gpp_throt_sm_get_core_sm_ptr
(
  int                         core_sm_index, 
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_SET_CORE_SM_PTR
 
DESCRIPTION
  This function sets the Core Throt SM Ptr corresponding to a particular Index
  and Subs Id.
  
PARAMETERS   : 
  1. Core SM Index
  2. Subs Id
  3. Throt SM Ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_throt_sm_set_core_sm_ptr
(
  int                         core_sm_index, 
  sys_modem_as_id_e_type      subs_id,
  ds_3gppi_throttle_sm_s*     throt_sm_ptr
);


/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_CORE_SM_INIT

DESCRIPTION
  This function is used to do the power up procedures needed for the proper
  functioning of the core state machine. This initialization is not a per SM
  init. Irrespective of the number of core sm created, this has to be done
  once. It is safe to call this function multiple times since we perform init
  only the first time

PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_core_sm_init
(
  void
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds_3gppi_is_core_sm_init_performed == FALSE)
  {
    DS_3GPP_MSG0_MED("Core throttling SM init - done only once");
    /*--------------------------------------------------------------------------
      Set the boolean to TRUE so that we do not do initialization when a 2nd
      core sm is created.
    --------------------------------------------------------------------------*/
    ds_3gppi_is_core_sm_init_performed = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_MED("Core throttling SM init done already. Ignore");
  }
  return;
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_TIMER_CB

DESCRIPTION
  This function is the timer call back for the core state machine timer.
  This changes the state of the SM from throttled to unthrottled. Since this
  runs in the timer context
  
PARAMETERS  
  callback_data - points to the core throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
LOCAL void ds_3gppi_throt_sm_timer_cb
(
  unsigned long callback_data
)
{
  ds_cmd_type    *cmd_buf     = NULL;
  unsigned long  *cb_data_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf = ds_allocate_cmd_buf(sizeof(unsigned long));
  if( (NULL == cmd_buf) || (NULL == cmd_buf->cmd_payload_ptr) )
  {
    return;
  }
  else
  {
    cmd_buf->hdr.cmd_id = DS_CMD_3GPP_THROT_TMR_EXP;
    cb_data_ptr = (unsigned long*)cmd_buf->cmd_payload_ptr;
    *cb_data_ptr = callback_data;
    ds_put_cmd(cmd_buf);
  }
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_TIMER_EXP_CMD_HDLR
 
DESCRIPTION
  This function is used to process the throttling timer expiry
  
  
PARAMETERS  
  callback_data - points to the core throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  
===========================================================================*/
LOCAL void ds_3gppi_throt_sm_timer_exp_cmd_hdlr
(
  unsigned long callback_data
)
{
  ds_3gppi_throttle_sm_s          *throt_sm_ptr = NULL;
#ifdef FEATURE_DATA_RPM
  int8                             apn_index = DS_MAX_APN_NUM;
#endif /* FEATURE_DATA_RPM */
  sys_modem_as_id_e_type           subs_id = SYS_MODEM_AS_ID_NONE;
  ds_umts_pdp_type_enum_type  pdn_pdp_type = DS_UMTS_PDP_IPV4;
  ds_3gpp_pdn_throttle_sm_s  *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s   *pdn_throt_sm_info_ptr = NULL; 
  boolean                          is_attach_apn = FALSE;
  ds_3gpp_throt_rat_e_type         rat_index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  throt_sm_ptr = (ds_3gppi_throttle_sm_s*) callback_data;

  if(throt_sm_ptr != NULL &&
     throt_sm_ptr->cookie == THROTLLE_SM_COOKIE)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                            "Clearing the is_throt_flag in core sm for APN: %s", 
                            throt_sm_ptr->apn);
    
    subs_id = throt_sm_ptr->subs_id;
    
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->is_throttled = FALSE;
    /*------------------------------------------------------------------------- 
      Log this throttling event
    -------------------------------------------------------------------------*/
    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                      (throt_sm_ptr->apn,FALSE,FALSE,
                       DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
                       subs_id);

    if((pdn_throt_sm_ptr != NULL) && 
       (pdn_throt_sm_ptr->v4_throttle_sm == throt_sm_ptr))
    {
      pdn_pdp_type = DS_UMTS_PDP_IPV4;
      ds_3gpp_pdn_throt_log_throt_info(throt_sm_ptr,throt_sm_ptr->apn,
                                       pdn_pdp_type);
      ds_3gpp_pdn_throt_clr_log_entry(pdn_throt_sm_ptr->v4_throttle_sm);

    }
    else if ((pdn_throt_sm_ptr != NULL) && 
             (pdn_throt_sm_ptr->v6_throttle_sm == throt_sm_ptr))
    {
      pdn_pdp_type = DS_UMTS_PDP_IPV6;
      ds_3gpp_pdn_throt_log_throt_info(throt_sm_ptr,throt_sm_ptr->apn,
                                       pdn_pdp_type);
      ds_3gpp_pdn_throt_clr_log_entry(pdn_throt_sm_ptr->v6_throttle_sm);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("pdn throt sm ptr could not be found, "
                         "Not logging throt info");
    }

    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

    pdn_throt_sm_ptr = NULL;
    pdn_throt_sm_info_ptr = (ds_3gpp_pdn_throttle_sm_info_s  *)
                              ds_3gpp_pdn_throt_sm_get_throttle_sm_info_from_plmn(
                                                             throt_sm_ptr->plmn_id,
                                                             throt_sm_ptr->apn,
                                                             subs_id); 
    while (pdn_throt_sm_info_ptr != NULL && 
          rat_index < DS_3GPP_THROT_RAT_ILLEGAL)
    {
      pdn_throt_sm_ptr = &(pdn_throt_sm_info_ptr->per_rat[rat_index]);
      if ((rat_index == DS_3GPP_THROT_RAT_LTE) || 
          (rat_index == DS_3GPP_THROT_RAT_GLOBAL))
      {
        if (pdn_throt_sm_ptr!=NULL && 
           (pdn_throt_sm_ptr->sm_type != DS_3GPP_THROTTLE_TYPE_T3396 ) &&
           ((pdn_throt_sm_ptr->v4_throttle_sm == throt_sm_ptr )||
           (pdn_throt_sm_ptr->v6_throttle_sm == throt_sm_ptr )))
        { 
#ifdef FEATURE_DATA_LTE
          if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)throt_sm_ptr->apn))
          {
            is_attach_apn = TRUE; 
          }
#endif /* FEATURE_DATA_LTE */
          break;
        }
      }
      rat_index++;
    }
 
    if(throt_sm_ptr->throttle_sm_timer_expiry_cb != NULL)
    {
      throt_sm_ptr->throttle_sm_timer_expiry_cb
        ((void *)throt_sm_ptr, throt_sm_ptr->throttle_sm_timer_expiry_cb_data);
    }

#ifdef FEATURE_DATA_RPM
    if (TRUE == ds_3gpp_is_rpm_enabled(subs_id))
    {
      (void)ds_3gpp_rpm_get_efs_throttle_param_ptr(throt_sm_ptr->apn, 
                                                   FALSE, 
                                                   &apn_index,
                                                   subs_id);

      if (DS_MAX_APN_NUM == apn_index)
      {
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR,
                                "APN : %s not found in RPM backup tbl",
                                throt_sm_ptr->apn);
        return;
      }

      ds_3gpp_rpm_backoff_tmr_exp_hdlr(apn_index, subs_id);
    }
#endif /* FEATURE_DATA_RPM */
    if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) != TRUE )||
		(ds_3gpp_pdn_throt_is_apn_globally_throttled(
                    throt_sm_ptr->apn, throt_sm_ptr->subs_id)))
    {
      ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
    }

    if (is_attach_apn && 
        !ds_3gpp_pdn_throt_is_apn_blocked_allplmns_or_roaming(throt_sm_ptr->apn,
                                                       subs_id))
    {
       ds_3gpp_pdn_cntxt_flush_plmn_from_list(throt_sm_ptr->plmn_id, 
                                              subs_id);
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL CB Data in throttle timer CB");
  }
} /* ds_3gppi_throt_sm_timer_exp_cmd_hdlr */

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_CORE_THROTTLE_SM

DESCRIPTION
  This function gets a new throttle state machine instance to a client. It
  also initializes all the fields appropriately.

  This is the internals of state machine implementation - Will also be refered
  to as the core state machine
  
PARAMETERS  
  plmn_id - PLMN Id for which this core sm is being created
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to the newly obtained throttle SM.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_3gppi_throttle_sm_s* ds_3gppi_throt_sm_get_core_throttle_sm
(
  sys_plmn_id_s_type           plmn_id,
  sys_modem_as_id_e_type       subs_id
)
{
  ds_3gppi_throttle_sm_s* throt_sm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------------
    Allocate a core state machine from heap. Return pointer to this core SM.
  ----------------------------------------------------------------------------*/

  DS_3GPP_MSG1_MED("Get core throttling SM for Subs Id: %d", subs_id);
   
  throt_sm_ptr = (ds_3gppi_throttle_sm_s*)
                  modem_mem_alloc(sizeof(ds_3gppi_throttle_sm_s),
                                  MODEM_MEM_CLIENT_DATA);
  if(throt_sm_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Initialize the state machine with the correct values.
    --------------------------------------------------------------------------*/
    throt_sm_ptr->is_throttled = FALSE;
    throt_sm_ptr->failure_count = 0;
    throt_sm_ptr->throt_tmr_val = 0;
    throt_sm_ptr->throttle_sm_timer_start_cb = NULL;
    throt_sm_ptr->throttle_sm_timer_expiry_cb = NULL;
    throt_sm_ptr->throttle_sm_get_timer_val_cb = NULL;
    throt_sm_ptr->throttle_sm_timer_start_cb_data = NULL;
    throt_sm_ptr->throttle_sm_timer_expiry_cb_data = NULL;
    throt_sm_ptr->throttle_sm_get_timer_val_cb_data = NULL;
    throt_sm_ptr->subs_id = subs_id;
    throt_sm_ptr->cookie = THROTLLE_SM_COOKIE;

    memscpy((void*)&(throt_sm_ptr->plmn_id),
	    sizeof(sys_plmn_id_s_type),
            (void*)&(plmn_id), 
            sizeof(sys_plmn_id_s_type));

    /*--------------------------------------------------------------------------
      Initialize the throttling timer
      The call back data will be the pointer to the core state machine
    --------------------------------------------------------------------------*/
      rex_def_timer_ex(&(throt_sm_ptr->throttle_timer),
                     (rex_timer_cb_type) ds_3gppi_throt_sm_timer_cb,
                     (unsigned long)(throt_sm_ptr));

    memset(&throt_sm_ptr->user_data,0,sizeof(throt_sm_ptr->user_data));
    throt_sm_ptr->user_data.auth_info_p = NULL;
  }
  return throt_sm_ptr;

}
/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_REGISTER_TIMER_START_EV_CB

DESCRIPTION
  This function registers the throttle_timer_start_ev_cb for the core SM
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cb_fptr - callback function to register
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to the newly obtained throttle SM.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_register_timer_start_ev_cb
(
  void*                              throt_ptr,
  ds_3gppi_throttle_sm_timer_ev_cb_f ev_cb_fptr,
  void*                              cb_data
)
{
  ds_3gppi_throttle_sm_s* throt_sm_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  throt_sm_ptr = (ds_3gppi_throttle_sm_s*) throt_ptr;
  if (throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL throttle_sm_ptr");
    return;
  }
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  throt_sm_ptr->throttle_sm_timer_start_cb = ev_cb_fptr;
  throt_sm_ptr->throttle_sm_timer_start_cb_data = cb_data;
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_REGISTER_TIMER_EXPIRY_EV_CB

DESCRIPTION
  This function registers the throttle_timer_start_ev_cb for the core SM
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cb_fptr - callback function to register
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to the newly obtained throttle SM.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_register_timer_expiry_ev_cb
(
  void*                              throt_ptr,
  ds_3gppi_throttle_sm_timer_ev_cb_f ev_cb_fptr,
  void*                              cb_data
)
{
  ds_3gppi_throttle_sm_s* throt_sm_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  throt_sm_ptr = (ds_3gppi_throttle_sm_s*) throt_ptr;
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  throt_sm_ptr->throttle_sm_timer_expiry_cb = ev_cb_fptr;
  throt_sm_ptr->throttle_sm_timer_expiry_cb_data = cb_data;
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_REGISTER_GET_THROTTLE_TIMER_VAL_CB

DESCRIPTION
  This function registers the get_throttle_timer_val_cb for the core SM
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cb_fptr - callback function to register
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_register_get_throttle_timer_val_cb
(
  void*                                 throt_ptr,
  ds_3gppi_throttle_sm_get_timer_cb_f   ev_cb_fptr,
  void*                                 cb_data
)
{
  ds_3gppi_throttle_sm_s* throt_sm_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  throt_sm_ptr = (ds_3gppi_throttle_sm_s*) throt_ptr;
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  throt_sm_ptr->throttle_sm_get_timer_val_cb = ev_cb_fptr;
  throt_sm_ptr->throttle_sm_get_timer_val_cb_data = cb_data; 
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
}
/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_FREE_CORE_STATE_MACHINE

DESCRIPTION
  This function destroys a throttle SM that a client has been using. This can
  be called whenever throttling is stopped.

  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - Pointer to the throttling SM to be released.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_free_core_state_machine
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------------
    Free the core state machine
  ----------------------------------------------------------------------------*/
  if (throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL throttle_sm_ptr");
    return;
  }

  if(throt_sm_ptr->user_data.auth_info_p != NULL)
  {
    modem_mem_free(throt_sm_ptr->user_data.auth_info_p,
                   MODEM_MEM_CLIENT_DATA);
    throt_sm_ptr->user_data.auth_info_p = NULL;
  }

  memset(throt_sm_ptr, 0, sizeof(ds_3gppi_throttle_sm_s));
  modem_mem_free(throt_sm_ptr, MODEM_MEM_CLIENT_DATA);
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_IS_REL_10_THROTTLE

DESCRIPTION
  This function is used to check whether release 10 throttling mechanism
  is being performed or not. Release 10 throttling will kick in only for
  perticular cause codes so we will check only those cause codes.

PARAMETERS    
  cause_code - cause_code indicated in reject message.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE - thrpttle sm is performing Rel 10 throttling.
  FALSE - otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL boolean ds_3gppi_throt_sm_is_rel_10_throttle
(
  void *cause_code,
  sys_modem_as_id_e_type  subs_id
)
{
  boolean rel_10_throttle = FALSE;
  /*------------------------------------------------*/
 #if defined(FEATURE_DATA_LTE) && defined(FEATURE_DATA_REL10)

  if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id) != TRUE)
  {
     return  FALSE;
  }

#else
   return FALSE;
#endif

  if(cause_code!=NULL)
  {
    switch((ps_iface_net_down_reason_type)cause_code)
    {
     case PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES:
     case PS_NET_DOWN_REASON_UNKNOWN_APN:
        rel_10_throttle = TRUE;

     default:
        rel_10_throttle = FALSE;
    }
  }

  return rel_10_throttle;
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_FEED_FAILURE_TO_CORE_SM

DESCRIPTION
  This function is used to apply throttling. This performs the following
  operations:
  1. Set the throttle SM to THROTTLED state
  2. Increment the counter corresponding to the number of rejects
  3. Get the throttle timer value based on the number of rejects
  4. Start the throttling timer
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr         -   Pointer to the throttling SM to be released.
  cause_code           -   cause_code indicated in reject message.
  ds_pdn_context_s        *pdn_context_p
  subs_id                  subs_id
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_feed_failure_to_core_sm
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr,
  void*                   cause_code,
  sys_modem_as_id_e_type  subs_id,
  ds_pdn_context_s       *pdn_context_p
)
{
#ifdef FEATURE_DATA_RPM
  boolean    is_rpm_enabled = FALSE;
#endif /* FEATURE_DATA_RPM */
  boolean serv_req_throttling = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_3GPP_MSG3_HIGH("is_throt_flag: %d, fail_count: %d, timer_val: %d",
                      throt_sm_ptr->is_throttled, throt_sm_ptr->failure_count,
                      throt_sm_ptr->throt_tmr_val);
    /*--------------------------------------------------------------------------
      All operations in this function are to be done in a critical section.
      This is because we will be changing the info in the SM. Since this can
      happen from different context, we want to ensure that the SM is in a 
      consistent state
    --------------------------------------------------------------------------*/
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

    serv_req_throttling = ds_3gpp_throt_is_serv_req_throt_sm(throt_sm_ptr);

#ifdef FEATURE_DATA_RPM
    is_rpm_enabled = ds_3gpp_is_rpm_enabled(subs_id);

    /* Reset throttle timer value to 0 before getting new timer val */
    throt_sm_ptr->throt_tmr_val = 0;

    if (is_rpm_enabled && (serv_req_throttling == FALSE))
    {
      if (FALSE == ds_3gpp_rpm_get_throttle_params(throt_sm_ptr->apn, 
                                                   throt_sm_ptr))
      {
        DS_3GPP_MSG3_ERROR("Could not get throttle params from RPM", 0,0,0);
      }
    }

    /*------------------------------------------------------------------------
      Check if RPM throttle timer is valid. If RPM is enabled then backoff 
      timer and failure count values need to be obtained from RPM module.
      If RPM throttle timer is not configured then it implies that throttling
      is not triggered by RPM and hence its service request throttling or
      pdn throttling.
    ------------------------------------------------------------------------*/
    if (0 == throt_sm_ptr->throt_tmr_val)
#endif /* FEATURE_DATA_RPM */
    {
      /*----------------------------------------------------------------------
        Increment the count of consecutive failures
      --------------------------------------------------------------------------*/
      DS_3GPP_THROTTLE_SM_INCREMENT_FAILURE_COUNTER(throt_sm_ptr->failure_count);
  
      /*--------------------------------------------------------------------------
        Get the timer value so that we can start the throttle timer
      --------------------------------------------------------------------------*/
      throt_sm_ptr->throt_tmr_val = 
                   ds_3gpp_throt_sm_get_throttle_failure_timer(throt_sm_ptr,
                                                               cause_code,
                                                               pdn_context_p,
                                                               serv_req_throttling);

      if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(throt_sm_ptr->subs_id) == TRUE) &&
         (throt_sm_ptr->throt_tmr_val == DS_3GPP_PDN_THROT_INFINITE_THROT))
      {
        DS_3GPP_MSG1_HIGH("CC_SP infinite throttling for given cause code timer:%d", 
                           throt_sm_ptr->throt_tmr_val );
        ds_3gppi_throt_sm_set_is_throttled_flag(throt_sm_ptr,
                                              TRUE);
        ds_3gppi_throt_sm_set_throttle_timer_val(throt_sm_ptr,
                                             DS_3GPP_PDN_THROT_INFINITE_THROT);
#ifdef FEATURE_DATA_LTE 
       if(ds_eps_pdn_throt_is_any_lte_plmn_blocked(throt_sm_ptr->subs_id) == TRUE)
       {
         ds_eps_pdn_throt_start_lte_unblock_plmn_timer(
                                    DS_EPS_PDN_THROT_2_HOURS_TIMER, 
                                    throt_sm_ptr->subs_id);
       }
#endif /* FEATURE_DATA_LTE */
		
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
        return;
      }
	  
      }

    if(throt_sm_ptr->throt_tmr_val > 0)
    {
      /*------------------------------------------------------------------------
        Change the state of the SM to throttled
      ------------------------------------------------------------------------*/
      throt_sm_ptr->is_throttled = TRUE;

     /*------------------------------------------------------------------------
       throttle_sm_timer_start_cb will be registered only for service reject and
       release 10 throttling. The other scenarios will be PDN rejects.
     ------------------------------------------------------------------------*/
      if(throt_sm_ptr->throttle_sm_timer_start_cb != NULL)
      {
        throt_sm_ptr->throttle_sm_timer_start_cb
          ((void *)throt_sm_ptr,throt_sm_ptr->throttle_sm_timer_start_cb_data);
      }
      else
      {
        /*========================================================================
         For 3rd PDN reject the length of the timer shall be one minute plus a 
         random value; the random value shall have an upper bound of 15 seconds
         and this addition should not be done for release 10 cause code and if RPM
         is enabled and if it is global throttling
        ========================================================================*/ 
        if ( (throt_sm_ptr->failure_count == 3) &&
             (ds_3gpp_srv_req_throttle_behavior[subs_id]
                                        == DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN))
        {
          if((!ds_3gppi_throt_sm_is_rel_10_throttle(cause_code,subs_id)) &&
             (!ds_3gpp_nv_manager_get_global_throttling(subs_id)) &&
             (!ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id))
  #ifdef FEATURE_DATA_LTE
             &&(!ds_3gpp_nv_manager_get_reg_pdn_throttling(subs_id))
  #endif			 
  #ifdef FEATURE_DATA_RPM
              && ((ds_3gpp_is_rpm_enabled(subs_id) == FALSE) )
  #endif /* FEATURE_DATA_RPM */	          
            )
          {
            throt_sm_ptr->throt_tmr_val += 
               (ps_utils_generate_16bit_rand_num_by_range(
               DS_3GPP_PDN_REJECT_TIMER_MIN_SECOND, 
               DS_3GPP_PDN_REJECT_TIMER_MAX_SECOND) * 
               DS_3GPP_SECOND_TO_MILISECOND_MULTIPLIER );
          }
        }
      }

      /*------------------------------------------------------------------------
        Now start the timer with the value that was received
      ------------------------------------------------------------------------*/
      (void)rex_set_timer(&(throt_sm_ptr->throttle_timer), 
                          throt_sm_ptr->throt_tmr_val);

      DS_3GPP_MSG1_HIGH("Started throtlling timer for %d ms",
                         throt_sm_ptr->throt_tmr_val);

     if(((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) != TRUE)||
	     (ds_3gpp_pdn_throt_is_apn_globally_throttled(throt_sm_ptr->apn, 
               throt_sm_ptr->subs_id)))&& (serv_req_throttling == FALSE))
      {
        ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm(subs_id);

#ifdef FEATURE_DATA_RPM
        if (TRUE == is_rpm_enabled)
        {
          ds_3gpp_rpm_write_backoff_tmr_timestamp(throt_sm_ptr->apn,
                                                  time_get_secs(),
                                                  throt_sm_ptr->subs_id);
        }
#endif /* FEATURE_DATA_RPM */
      }
    } /* if throttling timer greater than zero */
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

    DS_3GPP_MSG3_HIGH("is_throt_flag: %d, fail_count: %d, timer_val: %d",
                      throt_sm_ptr->is_throttled, throt_sm_ptr->failure_count,
                      throt_sm_ptr->throt_tmr_val);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Null pointer in core sm feed failure fn. Ignore");
  }

}

/*===========================================================================
FUNCTION DS_3GPPI_GET_IS_THROTTLED_FLAG

DESCRIPTION
  This function examines if a throttling SM is in throttled or unthrottled
  state
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If the SM is in throttled state
  FALSE - If the SM is not in throttled state
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_get_is_throttled_flag
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (throt_sm_ptr)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    ret_val = throt_sm_ptr->is_throttled;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  }

 return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPPI_SET_IS_THROTTLED_FLAG

DESCRIPTION
  This function sets a throttling SM to throttled or unthrottled
  state
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  is_throttled - boolean value to set the is_throttled flag
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None. 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gppi_throt_sm_set_is_throttled_flag
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  boolean                 is_throttled
)
{
  if (throt_sm_ptr != NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->is_throttled = is_throttled;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    DS_3GPP_MSG1_HIGH("IS_THROTTLED flag set to %d",is_throttled);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL argument passed in set is_throttled. Ignore");
  }
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_FAILURE_COUNT

DESCRIPTION
  This function is used to get the count of failed attempts that is maintained
  by the core state machine. Note that such an API will not be exposed from
  any wrapper. Failures will always be fed into the wrapper. The wrapper has
  the logic needed to increment counter values.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  failure_count_ptr - out param pointing to the failure count
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - if the incoming params were non-NULL
  FALSE - incoming params NULL
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_get_failure_count
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  uint8                  *failure_count_ptr
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    *failure_count_ptr = throt_sm_ptr->failure_count;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL pointer in core sm get count function.");
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_FAILURE_COUNT

DESCRIPTION
  This function is used to set the count of failed attempts that is maintained
  by the core state machine. Note that such an API will not be exposed from
  any wrapper. Failures will always be fed into the wrapper. The wrapper has
  the logic needed to increment counter values.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr  - pointer to the throttling state machine
  failure_count - failure_count to be stored in the core sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - On success
  FALSE - On any failure
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_set_failure_count
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  uint8                  failure_count
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->failure_count = failure_count;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL argument passed in set fail count. Ignore");
  }
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_THROTTLE_TIMER_VAL

DESCRIPTION
  This function returns the timer value for which throttling is to be done.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  uint32 - the value that the throttling timer was started with.  

SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gppi_throt_sm_get_throttle_timer_val
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
)
{
  uint32 ret_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  
  if(throt_sm_ptr != NULL)
  {
    ret_val = throt_sm_ptr->throt_tmr_val;
  }
  
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_PLMN_ID

DESCRIPTION
  This function returns the plmn_id for the core SM

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  sys_plmn_id_s_type - plmn_id of the core SM. 

SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_id_s_type ds_3gppi_throt_sm_get_plmn_id
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
)
{
  sys_plmn_id_s_type ret_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset((void*)&ret_val, 0, sizeof(sys_plmn_id_s_type));
  
  if (throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL throttle_sm_ptr");
    return ret_val;
  }
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  ret_val = throt_sm_ptr->plmn_id;
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_THROTTLE_TIMER_VAL

DESCRIPTION
  This function sets the timer value for which throttling is to be done.
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine
 
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  timer_val    - Value to be set
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_set_throttle_timer_val
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr,
  uint32                  timer_val 
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (throt_sm_ptr != NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->throt_tmr_val = timer_val;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    DS_3GPP_MSG1_HIGH("Throttle timer value set to %d",timer_val);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL argument passed in set throttle_timer_val. Ignore");
  }
}

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_REMAINING_THROTTLE_TIME

DESCRIPTION
  This function returns the remaining time in milliseconds for which
  throttling occurs
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  uint32 - remaining throttle time.  

SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gppi_throt_sm_get_remaining_throttle_time
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
)
{
  uint32 ret_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  if(throt_sm_ptr->throt_tmr_val == DS_3GPP_PDN_THROT_INFINITE_THROT)
  {
    ret_val = DS_3GPP_PDN_THROT_INFINITE_THROT;
  }
  else
  {
    ret_val = rex_get_timer(& (throt_sm_ptr->throttle_timer));
  }
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

  return ret_val;
}

#ifdef FEATURE_DATA_RPM
/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_CLEAR_THROTTLE_TIMER

DESCRIPTION
  This function clears the throttling timer for the specified core throt_sm.

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gppi_throt_sm_clear_throttle_timer
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                            "Clearing the is_throt_flag in core sm for apn: %s",
                            throt_sm_ptr->apn);

    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_sm_ptr->is_throttled = FALSE;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    (void)rex_clr_timer(&(throt_sm_ptr->throttle_timer));
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL throttle_sm_ptr");
  }
} /* ds_3gppi_throt_sm_clear_throttle_timer */
#endif /* FEATURE_DATA_RPM */

/*------------------------------------------------------------------------------
  This Macro is used to determine if the index returned by the functions (to
  find core SM based on PLMN or to allocate a free index  in the Wrapper SM)
   is valid.
------------------------------------------------------------------------------*/
#define DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) \
  ((core_sm_index >= 0) && (core_sm_index < DS_3GPP_MAX_PLMN_SUPPORTED))? \
                                                                 TRUE:FALSE


/*------------------------------------------------------------------------------
  This Macro is used to access the core SM given the index in the Wrapper SM
------------------------------------------------------------------------------*/
#define DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id) \
          ds_3gpp_throt_sm_get_core_sm_ptr(core_sm_index, subs_id)

#define DS_3GPP_THROT_SM_SET_CORE_SM_PTR(core_sm_index, subs_id, throt_ptr) \
          ds_3gpp_throt_sm_set_core_sm_ptr(core_sm_index, subs_id, throt_ptr)


/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_INIT

DESCRIPTION
  This function initializes Service Request Throttle SM for a particular
  Subscription.

PARAMETERS  
  Subs Id

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_srv_req_throt_init
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_throttle_sm_s* service_req_throt_sm_ptr = NULL;
  uint8                  index = 0;
  uint32                 ser_req_throt_sm_nv;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ser_req_throt_sm_nv = ds_3gpp_nv_manager_get_ser_req_thor_sm_nv(subs_id);

   if((ds_3gpp_srv_req_throttle_behavior_e)ser_req_throt_sm_nv
                                        >= DS_3GPP_NO_SRV_REQ_THROT_MAX)
  {
    DS_3GPP_MSG1_ERROR("ds_3gppi_throt_sm_core_sm_init: EFS item "
                      "DS_3GPP_CFG_SRV_REQ_THROTTLE_BEHAVIOR configured " 
                      "badly,using behavior %d", 
                       ds_3gpp_srv_req_throttle_behavior[subs_id]);
  }
  else
  {
    ds_3gpp_srv_req_throttle_behavior[subs_id] = 
                     (ds_3gpp_srv_req_throttle_behavior_e)ser_req_throt_sm_nv;
  }
  /*----------------------------------------------------------------------------
    Initialize the system wide throttling state machine. Here we create a handle
    to the singleton object that will be used by multiple clients.
  ----------------------------------------------------------------------------*/
  service_req_throt_sm_ptr = ds_3gpp_service_req_throt_sm_ptr[subs_id];


  if (service_req_throt_sm_ptr == NULL)
  {
    service_req_throt_sm_ptr = 
      (ds_3gpp_throttle_sm_s*) 
        modem_mem_alloc(sizeof(ds_3gpp_throttle_sm_s), MODEM_MEM_CLIENT_DATA);
    
    /*----------------------------------------------------------------------------
      If modem_mem_alloc returned a NULL pointer, print out an error message 
      saying service request throttling cannot be done
    ----------------------------------------------------------------------------*/
    if(service_req_throt_sm_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Could not allocate mem for throttling SM.");
      return;
    }

    ds_3gpp_service_req_throt_sm_ptr[subs_id] = service_req_throt_sm_ptr;
    
    /*--------------------------------------------------------------------------
      Initialize all the core sm pointers in this throt sm to NULL
    --------------------------------------------------------------------------*/
    for(index = 0; index < DS_3GPP_MAX_PLMN_SUPPORTED; index++) 
    {
      service_req_throt_sm_ptr->sm_ptr[index] = NULL;
    }
    
    /*--------------------------------------------------------------------------
      Initialize the free count in the sys wide service req throt sm to 8
    --------------------------------------------------------------------------*/
    service_req_throt_sm_ptr->free_count = DS_3GPP_MAX_PLMN_SUPPORTED;

  }
  else
  {
    DS_3GPP_MSG1_ERROR("Srv req throt SM already allocated for subs id %d", 
                       subs_id);
  }
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_INIT

DESCRIPTION
  This function initializes the singleton systemwide throttling state machine
  This makes use of the functionality available in the core state machine. In
  other words, this is the client of the core state machine.
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_init(void)
{

  sys_modem_as_id_e_type        subs_id;

  subs_id = ds3g_get_ps_subs_id();

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ds_3gpp_srv_req_throt_init(subs_id);

  DS_3GPP_MSG1_HIGH("ds_3gppi_throt_sm_core_sm_init: service req throtlle SM "
                     "configured as %d", ds_3gpp_srv_req_throttle_behavior[subs_id]);
   
  
   ds_3gppi_throt_sm_core_sm_init();

   DS_3GPP_MSG0_MED("Defining the ds_3gpp_throt_sys_sel_retry_sm timer");

   memset((void *)&ds_3gpp_throt_sys_sel_retry_sm,0,
           sizeof(ds_3gpp_throt_sys_sel_retry_sm_s));

   rex_def_timer_ex(&(ds_3gpp_throt_sys_sel_retry_sm.sys_sel_retry_timer),
   (rex_timer_cb_type)ds_3gpp_throt_sys_sel_retry_timer_expiry,0);

}/*ds_3gpp_throt_sm_init*/


/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_CONFIGURE_TIMER_EV_CBS

DESCRIPTION
  This function registers handlers for the core SM's throttle timer start
  and expiry events based on the NV item:DS_3GPP_CFG_THROTTLE_TYPE
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void ds_3gpp_throt_sm_configure_timer_ev_cbs
(
  ds_3gppi_throttle_sm_s  *throt_sm_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(ds_3gpp_srv_req_throttle_behavior[subs_id])
  {
    case DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN:
      ds_3gppi_throt_sm_register_timer_start_ev_cb
        (throt_sm_ptr, &ds_3gpp_throt_sm_block_plmn, (void*)subs_id);
      break;

    case DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402:
      ds_3gppi_throt_sm_register_timer_start_ev_cb
        (throt_sm_ptr, &ds_3gpp_throt_sm_block_plmn_pref_t3402, (void*)subs_id);
      break;

    case DS_3GPP_SRV_REQ_THROT_NO_PLMN_BLOCK:
      ds_3gppi_throt_sm_register_timer_start_ev_cb
        (throt_sm_ptr, &ds_3gpp_throt_sm_no_plmn_blocking, (void*)subs_id);
      break;

    case DS_3GPP_NO_SRV_REQ_THROT_NO_BLOCK_PLMN:
      ds_3gppi_throt_sm_register_timer_start_ev_cb
        (throt_sm_ptr, &ds_3gpp_throt_sm_no_plmn_blocking_no_throttling, 
         (void*)subs_id);
      break;
	  
	default:
	  break;
  }
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_CLR_SRV_REQ_THROT

DESCRIPTION
  This function is registered as callback for LPM event. It clears the
  service request throttling state machine.
  
PARAMETERS  
  event_info - info about the LPM event
  user_data  - ignored as of now
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_clr_srv_req_throt
(
  ds_3gpp_event_info_u_type event_info,
  uint32                    user_data
)
{
  int i=0;
  sys_modem_as_id_e_type    subs_id=ds3g_get_ps_subs_id();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* In case of Mode LPM/offline, 3GPP MH needs to reset all the throt sm */
  if(event_info.oprt_mode == SYS_OPRT_MODE_LPM || 
     event_info.oprt_mode == SYS_OPRT_MODE_OFFLINE)
  {
    if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      return;
    }

      DS_3GPP_MSG1_HIGH("ds_3gpp_throt_sm_oprt_mode_chg_hdlr: Mode: %d. Resetting"
                        " all core SMs for the service request throt SM", 
                        event_info.oprt_mode);

    for (i=0; i < DS3GSUBSMGR_SUBS_ID_MAX; i++ )
    {
      ds_3gpp_throt_sm_go_null(i,TRUE);
    }
  } /* SYS_OPRT_MODE_LPM or SYS_OPRT_MODE_OFFLINE */
} /* ds_3gpp_throt_sm_clr_srv_req_throt */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_CORE_SM_FROM_PLMN

DESCRIPTION
  This function is used to get the core throttling state machine given a 
  PLMN ID and the wrapper state machine

PARAMETERS  
  wrapper_sm_ptr - The wrapper sm that houses several core sm
  plmn_id        - The PLMN Id used to look up the core throt sm
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  index into the core sm pointer array
  value between 0 and DS_3GPP_MAX_PLMN_SUPPORTED if core sm found
  DS_3GPP_MAX_PLMN_SUPPORTED if core sm is not found
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_3gpp_throt_sm_get_core_sm_from_plmn
(
  ds_3gpp_throttle_sm_s  *wrapper_sm_ptr,
  sys_plmn_id_s_type     plmn_id
)
{
  int                     loop_index;
  ds_3gppi_throttle_sm_s* core_sm_ptr = NULL;
  int                     ret_val = DS_3GPP_MAX_PLMN_SUPPORTED;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(wrapper_sm_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Service reqyest throttling state machine exists. Loop over all core sm
      that may exist to find if there is already a core sm corresponding to
      this PLMN
    --------------------------------------------------------------------------*/
    for(loop_index = 0; loop_index < DS_3GPP_MAX_PLMN_SUPPORTED; loop_index++)
    {
      core_sm_ptr = wrapper_sm_ptr->sm_ptr[loop_index];
      if(core_sm_ptr == NULL)
      {
        DS_3GPP_MSG1_LOW("Core SM ptr at index: %d is NULL. Continue",
                         loop_index);
        continue;
      }
      else if(sys_plmn_match(plmn_id, core_sm_ptr->plmn_id) == TRUE)
      {
        DS_3GPP_MSG1_MED("Core SM at index: %d matched PLMN",loop_index);
        ret_val = loop_index;
        break;
      }
      else
      {
        DS_3GPP_MSG1_MED("CSM at index: %d did not match PLMN. Continue",
                         loop_index);
        continue;
      }
    }//loop over all core sm in the Service request throt sm
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Could not find Service request throttling SM");
  }
  return ret_val;
} /* ds_3gpp_throt_sm_get_core_sm_from_plmn */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_IS_PLMN_NON_NULL

DESCRIPTION
  This function checks if the PLMN passed into this function is non-NULL
  
PARAMETERS  
  plmn_id - PLMN ID to be checked
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If PLMN is non-NULL
  FALSE - otherwise

SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_is_plmn_non_null(sys_plmn_id_s_type plmn_id)
{
  boolean            ret_val = FALSE;
  sys_plmn_id_s_type zero_plmn_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));

  if(memcmp((void*)&(plmn_id),
            (void*)&(zero_plmn_id), 
            sizeof(sys_plmn_id_s_type)) != 0)
  {
    DS_3GPP_MSG0_MED("PLMN being inspected is non-zero");
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_MED("PLMN being inspected is zero");
  }
  return ret_val;
} /* ds_3gpp_throt_sm_is_plmn_non_null */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_FIND_FREE_INDEX_FOR_CORE_SM

DESCRIPTION
  This function checks if the PLMN passed into this function is non-NULL
  
PARAMETERS  
  wrapper_sm_ptr - The wrapper sm that houses several core sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0 - (DS_3GPP_MAX_PLMN_SUPPORTED-1) is a free index can be located
  DS_3GPP_MAX_PLMN_SUPPORTED if no free entry can be located

SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int ds_3gpp_throt_sm_find_free_index_for_core_sm
(
  ds_3gpp_throttle_sm_s* wrapper_sm_ptr
)
{
  int loop_index;
  int ret_val = DS_3GPP_MAX_PLMN_SUPPORTED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(wrapper_sm_ptr != NULL && wrapper_sm_ptr->free_count > 0)
  {
    for(loop_index = 0; loop_index < DS_3GPP_MAX_PLMN_SUPPORTED; loop_index++)
    {
      /*----------------------------------------------------------------------
        If the entry in the table of core sm ptrs is NULL, it is considered a
        free entry
      ----------------------------------------------------------------------*/
      if(wrapper_sm_ptr->sm_ptr[loop_index] == NULL)
      {
        DS_3GPP_MSG1_LOW("Found free index: %d for core SM",loop_index);
        ret_val = loop_index;
        break;
      }// check if the core_sm_ptr is null in this index
    }//loop over the array of core SM ptr
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Wrapper SM ptr NULL or Free count is zero");
  }
  return ret_val;
} /* ds_3gpp_throt_sm_find_free_index_for_core_sm */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_THROTTLING_SM

DESCRIPTION
  This function will be invoked by clients to get a handle to the singleton
  object that will be maintained for system wide throttling of service reqs.
  
PARAMETERS  
  create_if_not_present - This flag is used to determine if we have to create
                          a core sm if the wrapper sm does not have one for
                          the current PLMN
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void* - A non-null void pointer will be given to clients. This is an opaque
          handle for the clients

  NULL  - If the singleton object for system wide throttling of service req was
          not created during power up. 

SIDE EFFECTS 
  None.
  
===========================================================================*/
void* ds_3gpp_throt_sm_get_throttling_sm
(
  boolean                create_if_not_present,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_throttle_sm_s *    ret_val = NULL;
  ds_3gppi_throttle_sm_s*    core_sm_ptr = NULL;
  int                        core_sm_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type         current_plmn;
  ds_3gpp_throttle_sm_s*     service_req_throt_sm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return NULL;
  }
  
  /*----------------------------------------------------------------------------
    Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn,0,sizeof(sys_plmn_id_s_type)); 

  /*----------------------------------------------------------------------------
    Query the Current PLMN that is maintained in PDN context
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  service_req_throt_sm_ptr = ds_3gpp_service_req_throt_sm_ptr[subs_id];

  /*----------------------------------------------------------------------------
    If the system wide state machine was intialized during start up, we have to
    go ahead and allocate memory for the core state machine that is going to
    perform all the needed operations.

    Apart from checking if the sys wide srv req throt sm has been allocated, we
    also have to ensure that the current plmn is non-NULL. If it is NULL, NAS
    has not sent us the PLMN change notification
  ----------------------------------------------------------------------------*/
  if( (service_req_throt_sm_ptr != NULL) && 
      (ds_3gpp_throt_sm_is_plmn_non_null(current_plmn) == TRUE) )
  {

    /*--------------------------------------------------------------------------
      First find if there is a core SM for the PLMN under consideration
      If there is one, we have to return that instead of allocating a new one
    --------------------------------------------------------------------------*/
    core_sm_index = 
      ds_3gpp_throt_sm_get_core_sm_from_plmn( service_req_throt_sm_ptr, 
                                              current_plmn);

    if( !(DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index)) )
    {
      /*------------------------------------------------------------------------
        Could not find a core SM for the PLMN.
        Find a free index in the Sys wide Srv req throttling SM to allocate the
        new core SM - This is to be done only if the client needs one created
        if it does not exist.
        Perform bounds check before using index
      ------------------------------------------------------------------------*/
      if (create_if_not_present == TRUE) 
      {
        core_sm_index = ds_3gpp_throt_sm_find_free_index_for_core_sm
                                             (service_req_throt_sm_ptr);

      
        if( DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) )
        {
          /*--------------------------------------------------------------------
            found a free index. Reduce the number of free count in Wrapper throt
            sm if a core SM can be allocated
          --------------------------------------------------------------------*/
          DS_3GPP_THROT_SM_SET_CORE_SM_PTR
            (core_sm_index, subs_id, 
              ds_3gppi_throt_sm_get_core_throttle_sm(current_plmn, subs_id));

          core_sm_ptr = DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id);

          if(core_sm_ptr == NULL)
          {
            DS_3GPP_MSG0_ERROR("Could not alloc mem for core throt SM");
          }
          else
          {
            ds_3gpp_throt_sm_configure_timer_ev_cbs(core_sm_ptr, subs_id);
                        
            DS_3GPP_MSG1_MED("Core SM allocated successfully at index: %d",
                             core_sm_index);
            /*------------------------------------------------------------------
              Reduce the free count
            ------------------------------------------------------------------*/
            service_req_throt_sm_ptr->free_count--;
          }// did we get a valid core sm index to allocate the SM
        } // validity of the index in the array of core sm pointers
        else
        {
          DS_3GPP_MSG1_ERROR("Invalid Index: %d. Cant create core SM. No throt",
                             core_sm_index);
        }
      } //if there is a need to create a new core SM
      else
      {
        DS_3GPP_MSG1_MED("Index: %d. Don't create core SM. No throt SM",
                         core_sm_index);
      }
    }
    else
    {
      DS_3GPP_MSG0_MED("Retrieved core SM for PLMN");
      core_sm_ptr = DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id);
    }

    if(core_sm_ptr != NULL)
    {
      ret_val = service_req_throt_sm_ptr;
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Couldnt find Srv req throt SM or PLMN is NULL");
  }
  return (void*) ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_BLOCK_PLMN

DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs plmn blocking.
 
PARAMETERS  
  void * - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_block_plmn
(
  void* sm_ptr,
  void* cb_data

)
{
#ifdef FEATURE_DATA_LTE
  sys_plmn_id_s_type       plmn_id;
  uint32                   timer_val;
  ds_3gppi_throttle_sm_s  *throt_sm_ptr;
#ifdef FEATURE_DATA_RPM
  boolean                  is_rpm_enabled = FALSE;
#endif /* FEATURE_DATA_RPM */
  uint8                    failure_count = 0;
  boolean                  random_timer_added = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sys_modem_as_id_e_type   subs_id = (sys_modem_as_id_e_type)cb_data;
  throt_sm_ptr = (ds_3gppi_throttle_sm_s *) sm_ptr;
  /*----------------------------------------------------------------------- 
    For now do not read the random throttling timers.
	Initialize them to 0.
  -----------------------------------------------------------------------*/
  throt_sm_ptr->min_random_timer_val = 0;
  throt_sm_ptr->max_random_timer_val = 0;

  timer_val = ds_3gppi_throt_sm_get_throttle_timer_val(throt_sm_ptr);
  plmn_id = ds_3gppi_throt_sm_get_plmn_id(throt_sm_ptr);
  ds_3gppi_throt_sm_get_failure_count(throt_sm_ptr,&failure_count);

#ifdef FEATURE_DATA_RPM
  is_rpm_enabled = ds_3gpp_is_rpm_enabled(subs_id);
  if ( FALSE == is_rpm_enabled )
#endif /* FEATURE_DATA_RPM */
  {

    /*----------------------------------------------------------------------- 
      If random value is configured in the efs config txt, increment the
      timer value. If min/max random timer values are the same, simply
      add the min_random timer value to the throttle timer value.
      -----------------------------------------------------------------------*/
    if((throt_sm_ptr->min_random_timer_val > 0) &&
       (throt_sm_ptr->min_random_timer_val != throt_sm_ptr->max_random_timer_val)) 
    {
      timer_val += (ps_utils_generate_16bit_rand_num_by_range(
                                      throt_sm_ptr->min_random_timer_val,
                                      throt_sm_ptr->max_random_timer_val) *
                                      DS_3GPP_SECOND_TO_MILISECOND_MULTIPLIER);

      ds_3gppi_throt_sm_set_throttle_timer_val(throt_sm_ptr,
                                               timer_val);

      random_timer_added = TRUE;
    }
    else if((throt_sm_ptr->min_random_timer_val > 0) &&
            (throt_sm_ptr->min_random_timer_val == throt_sm_ptr->max_random_timer_val))
    {
      timer_val += throt_sm_ptr->min_random_timer_val;
      ds_3gppi_throt_sm_set_throttle_timer_val(throt_sm_ptr,
                                               timer_val);
      random_timer_added = TRUE;
    }
    /*------------------------------------------------------------------------
      for third try, according to retry algorithm, we need add a random value 
      between 1-15 sec onto timer
    ------------------------------------------------------------------------*/
    if ((throt_sm_ptr->failure_count == 3) && (!random_timer_added))
    {     
      timer_val += 
               (ps_utils_generate_16bit_rand_num_by_range(
               DS_3GPP_PDN_REJECT_TIMER_MIN_SECOND, 
               DS_3GPP_PDN_REJECT_TIMER_MAX_SECOND) * 
               DS_3GPP_SECOND_TO_MILISECOND_MULTIPLIER );
      ds_3gppi_throt_sm_set_throttle_timer_val(throt_sm_ptr,
                            timer_val);
    }
  }

  DS_3GPP_MSG1_HIGH("Performing PLMN blocking for time: %d ms", timer_val);
  (void)ds_3gpp_throt_prep_for_lte_blocking_and_log
    (plmn_id,timer_val, subs_id,failure_count, FALSE);

#endif

}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_BLOCK_PLMN_PREF_T3402

DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs plmn blocking with preference 
  t3402 and resets the core SM.
 
PARAMETERS  
  void * - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_block_plmn_pref_t3402
(
  void * sm_ptr,
  void * cb_data
)
{

#ifdef FEATURE_DATA_LTE
  ds_3gppi_throttle_sm_s* throt_sm_ptr;
  sys_plmn_id_s_type      plmn_id; 
  uint32                  ds_t3402_value;
  uint32                  timer_val;
  sys_modem_as_id_e_type  subs_id = (sys_modem_as_id_e_type)cb_data;
  uint8  failure_count = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  throt_sm_ptr = (ds_3gppi_throttle_sm_s *) sm_ptr;
  timer_val = ds_3gppi_throt_sm_get_throttle_timer_val(throt_sm_ptr);
  ds_3gppi_throt_sm_get_failure_count(throt_sm_ptr,&failure_count);
  /*---------------------------------------------------------------------------
     The time for which LTE selection is disabled is determined by
     (a) PDN throttle timer valure determined using the core SM's failure count
     (b) Value of T3402.
     T3402 takes precedence.
  ----------------------------------------------------------------------------*/

  if((ds_t3402_value = ds_eps_pdn_cntxt_get_t3402_timer_val()))
  {
    DS_3GPP_MSG1_HIGH("ds_3gpp_throt_disable_lte: Using T3402 timer val %d", 
                                                    ds_t3402_value);
    ds_3gppi_throt_sm_set_throttle_timer_val(throt_sm_ptr, ds_t3402_value);
    timer_val = ds_t3402_value;
  }

  plmn_id = ds_3gppi_throt_sm_get_plmn_id(throt_sm_ptr);
  
  DS_3GPP_MSG1_HIGH("Blocking Plmn for time: %d ms",timer_val);

  (void)ds_3gpp_throt_prep_for_lte_blocking_and_log(plmn_id,timer_val, 
                                              subs_id,failure_count, FALSE);
   
  ds_3gppi_throt_sm_set_failure_count(throt_sm_ptr, 0);

  memset(&throt_sm_ptr->user_data,0,sizeof(throt_sm_ptr->user_data));

#endif /* FEATURE_DATA_LTE */

}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_NO_PLMN_BLOCKING
DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs throttling without plmn blocking.
 
PARAMETERS  
  sm_ptr - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void  ds_3gpp_throt_sm_no_plmn_blocking
(
  void* sm_ptr,
  void* cb_data
)
{
#ifdef FEATURE_DATA_LTE
  ds_3gppi_throttle_sm_s* throt_sm_ptr;
  uint32                  timer_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  throt_sm_ptr = (ds_3gppi_throttle_sm_s *) sm_ptr;
  timer_val = ds_3gppi_throt_sm_get_throttle_timer_val(throt_sm_ptr);

  if(throt_sm_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("Throttle PTR is NULL");
    return;
  }

  /*----------------------------------------------------------------------- 
    If random value is configured in the efs config txt, increment the
    timer value. If min/max random timer values are the same, simply
    add the min_random timer value to the throttle timer value.
  -----------------------------------------------------------------------*/
  if((throt_sm_ptr->min_random_timer_val > 0) &&
     (throt_sm_ptr->min_random_timer_val != throt_sm_ptr->max_random_timer_val)) 
  {
    timer_val += (ps_utils_generate_16bit_rand_num_by_range(
                                    throt_sm_ptr->min_random_timer_val,
                                    throt_sm_ptr->max_random_timer_val) *
                                    DS_3GPP_SECOND_TO_MILISECOND_MULTIPLIER);

    ds_3gppi_throt_sm_set_throttle_timer_val(throt_sm_ptr,
                                             timer_val);

  }
  else if((throt_sm_ptr->min_random_timer_val > 0) &&
          (throt_sm_ptr->min_random_timer_val == throt_sm_ptr->max_random_timer_val))
  {
    timer_val += throt_sm_ptr->min_random_timer_val;

    ds_3gppi_throt_sm_set_throttle_timer_val(throt_sm_ptr,
                                             timer_val);
  }
#endif
  return;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_NO_PLMN_BLOCKING_NO_THROTTLING
DESCRIPTION
  This function is called by the core throttlig machine when it starts the
  throttle_timer. This function performs no throttling and no plmn blocking.
 
PARAMETERS  
  sm_ptr - Pointer to the core throttling SM
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void  ds_3gpp_throt_sm_no_plmn_blocking_no_throttling
(
  void* sm_ptr,
  void* cb_data
)
{
#ifdef FEATURE_DATA_LTE
  ds_3gppi_throttle_sm_s* throt_sm_ptr;
  uint32                  timer_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  throt_sm_ptr = (ds_3gppi_throttle_sm_s *) sm_ptr;
  timer_val = ds_3gppi_throt_sm_get_throttle_timer_val(throt_sm_ptr);

  if(throt_sm_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("Throttle PTR is NULL");
    return;
  }
  /*-------------------------------------------------------------------------- 
    Reset the is_throttle_flag to 0 because we are never throttling
  --------------------------------------------------------------------------*/
  throt_sm_ptr->is_throttled = FALSE;

#endif
  return;

}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_FEED_FAILURE

DESCRIPTION
  This function feeds Service reject into the throttlig machine. This uses
  the core sm to start back off. The core sm also increases the failure
  count and kicks the sm to the correct state.

PARAMETERS  
  void*                  - Pointer to the throttling SM - opaque handle from client
  sys_modem_as_id_e_type   subs_id
  ds_pdn_context_s       - *pdn_cntxt_ptr
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_feed_failure
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id,
  ds_pdn_context_s       *pdn_cntxt_ptr
)
{
  ds_3gpp_throttle_sm_s        *throt_sm_local_ptr = NULL;
  int                          core_sm_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type           current_plmn;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*----------------------------------------------------------------------------
    Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn,0,sizeof(sys_plmn_id_s_type));

  /*----------------------------------------------------------------------------
    Query the Current PLMN that is maintained in PDN context
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  /*----------------------------------------------------------------------------
    Check if the incoming pointer to sys wide srv throt sm is NULL.
    Also ensure that the current PLMN is non-NULL
  ----------------------------------------------------------------------------*/
  if( (throt_sm_ptr != NULL) && 
      (ds_3gpp_throt_sm_is_plmn_non_null(current_plmn) == TRUE) )
  {
    throt_sm_local_ptr = (ds_3gpp_throttle_sm_s*)throt_sm_ptr;

    core_sm_index = ds_3gpp_throt_sm_get_core_sm_from_plmn(throt_sm_local_ptr,
                                                                current_plmn);

    if( DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) )
    {
      DS_3GPP_MSG0_HIGH("Feeding failure to core sm");
      ds_3gppi_throt_sm_feed_failure_to_core_sm
                            ( DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id), 
                              NULL, 
                              subs_id, 
                              pdn_cntxt_ptr);
    } //check if core SM exisits for this PLMN
    else
    {
      DS_3GPP_MSG0_ERROR("Could not locate core SM for current PLMN");
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL ptr for Throt SM or PLMN NULL. Feed fail ignored");
  }
  
}
/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_VALIDATE_MIN_FAILURE_COUNT

DESCRIPTION
  This function used to set the min failure count to given value if it is less 
 than that

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
  sys_modem_as_id_e_type subs id  
uint16 fail_count
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_throt_sm_validate_min_failure_count
(
  void * throt_sm_ptr, 
  sys_modem_as_id_e_type subs_id, 
  uint16 fail_count 
)
{
  ds_3gpp_throttle_sm_s		 *throt_sm_local_ptr = NULL;
  int						   core_sm_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type		   current_plmn;
  ds_3gppi_throttle_sm_s *ser_req_throt_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /*----------------------------------------------------------------------------
	Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn,0,sizeof(sys_plmn_id_s_type));

  /*----------------------------------------------------------------------------
	Query the Current PLMN that is maintained in PDN context
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  /*----------------------------------------------------------------------------
	Check if the incoming pointer to sys wide srv throt sm is NULL.
	Also ensure that the current PLMN is non-NULL
  ----------------------------------------------------------------------------*/
  if( (throt_sm_ptr != NULL) && 
	  (ds_3gpp_throt_sm_is_plmn_non_null(current_plmn) == TRUE) )
  {
	throt_sm_local_ptr = (ds_3gpp_throttle_sm_s*)throt_sm_ptr;

	core_sm_index = ds_3gpp_throt_sm_get_core_sm_from_plmn(throt_sm_local_ptr,
																current_plmn);

	if( DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) )
	{
	 ser_req_throt_ptr = DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id);
	 if(ser_req_throt_ptr != NULL &&ser_req_throt_ptr->failure_count < fail_count)
     {
         ser_req_throt_ptr->failure_count = fail_count;
     }
    }
  }
}/*ds_3gpp_throt_sm_validate_min_failure_count*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GO_NULL

DESCRIPTION
  This function is used to clean up the service request throttling state machine.
  This function internally cleans up the core state machine.

PARAMETERS  
 subs_id
 enable_plmn
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_go_null
(
  sys_modem_as_id_e_type subs_id,
  boolean                enable_plmn
)
{
  int core_sm_index = 0;
  ds_3gppi_throttle_sm_s      *temp_core_sm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*----------------------------------------------------------------------------- 
  If enable_plmn is set to TRUE, Check if the srv req throt NV's are configured
  to one of PLMN's blocking, only then we will need to enable PLMN
  -----------------------------------------------------------------------------*/

  if((enable_plmn) &&
     (ds_3gpp_srv_req_throttle_behavior[subs_id] != 
      DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN)&&
     (ds_3gpp_srv_req_throttle_behavior[subs_id] != 
      DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402))
  {
    enable_plmn = FALSE;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if (ds_3gpp_service_req_throt_sm_ptr[subs_id] != NULL)
  {
    for(core_sm_index = 0; core_sm_index < DS_3GPP_MAX_PLMN_SUPPORTED; 
          core_sm_index ++)
    {
      temp_core_sm_ptr = DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id); 
      if(temp_core_sm_ptr != NULL)
      {
        DS_3GPP_MSG1_HIGH("Resetting core SM at index %d. "
                          "Freeing memory for same", core_sm_index);
        /*------------------------------------------------------------------------
          Stop the timer in the core sm
        ------------------------------------------------------------------------*/
        (void)rex_clr_timer(&(temp_core_sm_ptr->throttle_timer));
        /*--------------------------------------------------------------------- 
          If enable_plmn is set then it is necessary to unblock the plmn
          ---------------------------------------------------------------------*/
 #ifdef FEATURE_DATA_LTE
        if(enable_plmn)
        {
          ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(
                               temp_core_sm_ptr->plmn_id, 
                               CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY, 
                               subs_id);
        }
#endif
        ds_3gppi_throt_sm_free_core_state_machine(temp_core_sm_ptr);
        /*------------------------------------------------------------------------
          Reset the core sm pointer maintained in the Sys level state machine
        ------------------------------------------------------------------------*/
        DS_3GPP_THROT_SM_SET_CORE_SM_PTR(core_sm_index, subs_id, NULL);
      }
    }

    ds_3gpp_service_req_throt_sm_ptr[subs_id]->free_count = 
      DS_3GPP_MAX_PLMN_SUPPORTED;            
  } 
  else
  {
    DS_3GPP_MSG1_ERROR("Throttle SM is NULL for sub id %d ", subs_id);
  }           

  return;
} 

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_RESET

DESCRIPTION
  This function is used to reset to state machine. This function internally
  cleans up the core state machine.

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_reset
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_throttle_sm_s  *throt_sm_local_ptr = NULL;
  int                     core_sm_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type      current_plmn;
  boolean                 throttled_flag;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn,0,sizeof(sys_plmn_id_s_type));

  /*----------------------------------------------------------------------------
    Query the Current PLMN that is maintained in PDN context
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  /*----------------------------------------------------------------------------
    Check if the incoming pointer to sys wide srv throt sm is NULL.
    Also ensure that the current PLMN is non-NULL
  ----------------------------------------------------------------------------*/
  if( (throt_sm_ptr != NULL) && 
      (ds_3gpp_throt_sm_is_plmn_non_null(current_plmn) == TRUE) )
  {
    throt_sm_local_ptr = (ds_3gpp_throttle_sm_s*)throt_sm_ptr;

    core_sm_index = ds_3gpp_throt_sm_get_core_sm_from_plmn(throt_sm_local_ptr, 
                                                                current_plmn);

    if( DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) )
    {
      /*--------------------------------------------------------------------------- 
      If throttle SM is configured for DAM behavior (Block Plmn with Preference T3402) 
      and the core SM timer is non-zero, it means the corresponding rex_timer() to enable back
      LTE is running. This also means that we are trying to reset the SM from UMTS
      RAT. In this scenario, we just ignore the reset and let the rex_timer expire
      so as to enable LTE again. If the timer is zero, we go a RAB_REESTAB_IND or 
      bearer up and we reset the core SM.
      ----------------------------------------------------------------------------*/ 

      if ((DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id)) != NULL)
      {
        throttled_flag = ds_3gppi_throt_sm_get_is_throttled_flag
                           (DS_3GPP_THROT_SM_CORE_SM_PTR
                              (core_sm_index, subs_id));
        if (ds_3gpp_srv_req_throttle_behavior[subs_id] == 
              DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402 && 
                throttled_flag == TRUE)
        {
          DS_3GPP_MSG0_HIGH("Ignore reset for throttling SM. LTE is currently"
                            "disabled");
          return;
        }

        DS_3GPP_MSG0_HIGH("Resetting core SM and freeing memory for same");
        /*------------------------------------------------------------------------
          Stop the timer in the core sm
        ------------------------------------------------------------------------*/
        (void)rex_clr_timer(  
                &( (DS_3GPP_THROT_SM_CORE_SM_PTR
                    (core_sm_index, subs_id))->throttle_timer ));

        ds_3gppi_throt_sm_free_core_state_machine
          (DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id));
      }

      /*------------------------------------------------------------------------
        Reset the core sm pointer maintained in the Sys level state machine
      ------------------------------------------------------------------------*/
      DS_3GPP_THROT_SM_SET_CORE_SM_PTR(core_sm_index, subs_id, NULL);

      /*------------------------------------------------------------------------
        Increase the free count of available indices in the Wrapper throt SM
      ------------------------------------------------------------------------*/
      throt_sm_local_ptr->free_count++;

    } //core sm exists
    else
    {
      DS_3GPP_MSG3_ERROR("Invalid Core SM index, Reset ignored",
                       0,0,0);
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL pointer passed for Throt SM. Reset ignored");
  }
}
/*FUNCTION DS_3GPP_THROT_SM_RESET_REL10_PER_PLMN_SM

DESCRIPTION
  This function resets the throttle statae machine 
  paramaters asscoiated with release 10 paramters in case of per plmn
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 

   
SIDE EFFECTS 
  None.

===========================================================================*/

void ds_3gpp_throt_sm_reset_rel_10_per_plmn_sm
(
  void*                   throt_sm_ptr
)
{
  ds_3gppi_throttle_sm_s *throt_sm_local_ptr = NULL;
  throt_sm_local_ptr = (ds_3gppi_throttle_sm_s*)throt_sm_ptr;

  ds_3gppi_throt_sm_set_is_throttled_flag(
     throt_sm_local_ptr,
     FALSE);

  ds_3gppi_throt_sm_set_throttle_timer_val(
     throt_sm_local_ptr,
     0);
  ds_3gppi_throt_sm_register_timer_start_ev_cb(
    throt_sm_local_ptr,
     NULL,
     NULL);

  ds_3gppi_throt_sm_register_timer_expiry_ev_cb(
    throt_sm_local_ptr,
    NULL,
    NULL);

 ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
   throt_sm_local_ptr,
   NULL,
   NULL);

}

/*===========================================================================
FUNCTION DS_3GPP_GET_IS_THROTTLED_FLAG

DESCRIPTION
  This function examines the presence of a core SM. The abscence of a core
  SM means no throttling is in place. If the core sm is present, then we
  query the core sm about the throttling state.
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If the SM is in throttled state
  FALSE - If the SM is not in throttled state
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_get_is_throttled_flag
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_throttle_sm_s *throt_sm_local_ptr = NULL;
  boolean                ret_val = FALSE; //Assume no throttling
  int                    core_sm_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type     current_plmn;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------
    Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn,0,sizeof(sys_plmn_id_s_type));

  /*----------------------------------------------------------------------------
    Query the Current PLMN that is maintained in PDN context
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  /*----------------------------------------------------------------------------
    Check if the incoming pointer to sys wide srv throt sm is NULL.
    Also ensure that the current PLMN is non-NULL
  ----------------------------------------------------------------------------*/
  if( (throt_sm_ptr != NULL) && 
      (ds_3gpp_throt_sm_is_plmn_non_null(current_plmn) == TRUE) )
  {
    throt_sm_local_ptr = (ds_3gpp_throttle_sm_s*)throt_sm_ptr;

    core_sm_index = ds_3gpp_throt_sm_get_core_sm_from_plmn(throt_sm_local_ptr,
                                                                current_plmn);

    /*--------------------------------------------------------------------------
      Check if a core SM is present. If yes go ahead and query the core sm about
      the throttling state
    --------------------------------------------------------------------------*/
    if( DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) )
    {
      DS_3GPP_MSG0_MED("core SM present. Querying throt state");
      ret_val = 
        ds_3gppi_throt_sm_get_is_throttled_flag(
           DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id));
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Core SM not present. Ret is_throt_flag: FALSE");
    }
  } /* Non -null input params*/
  else
  {
    DS_3GPP_MSG0_ERROR("NULL pointer passed to SM. get_is_throt_flag. ignored");
  } /* Null input param */

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_BACK_OFF_TIMER_VAL

DESCRIPTION
  This function is used to get the time for which throttling will happen.
  The client can use this any way it wants.

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
  uint32*       - Pointer to the timer_val - OUT param 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If all went fine
  FALSE - If anything fails.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_get_back_off_timer_val
(
  void*                  throt_sm_ptr, 
  uint32*                timer_val_ptr,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_throttle_sm_s      *throt_sm_local_ptr = NULL;
  boolean                    ret_val = FALSE;
  int                        core_sm_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type         current_plmn;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------------
    Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn,0,sizeof(sys_plmn_id_s_type));

  /*----------------------------------------------------------------------------
    Query the Current PLMN that is maintained in PDN context
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  /*----------------------------------------------------------------------------
    Check if the incoming pointer to sys wide srv throt sm is NULL.
    Also ensure that the current PLMN is non-NULL
  ----------------------------------------------------------------------------*/
  if( (throt_sm_ptr != NULL) &&
      (timer_val_ptr != NULL) &&
      (ds_3gpp_throt_sm_is_plmn_non_null(current_plmn) == TRUE) )
  {
    throt_sm_local_ptr = (ds_3gpp_throttle_sm_s*)throt_sm_ptr;

    core_sm_index = ds_3gpp_throt_sm_get_core_sm_from_plmn(throt_sm_local_ptr,
                                                    current_plmn);

    /*--------------------------------------------------------------------------
      Check if a core SM is present. If yes go ahead and query the core sm about
      the throttling state
    --------------------------------------------------------------------------*/
    if( DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) )
    {      
      DS_3GPP_MSG0_MED("core SM present. Querying throt timer val");

      *timer_val_ptr = 
         ds_3gppi_throt_sm_get_throttle_timer_val(
           DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id));
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Core SM not present. Timer val set to 0");
      *timer_val_ptr = 0;
    }
    ret_val = TRUE;
  } // valid incoming argument
  else
  {
    DS_3GPP_MSG0_ERROR("Opaque hdl NULL. Get timer value bailing out");
  }
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_THROTTLING_STATE

DESCRIPTION
  This function is used to get the state of the throttling sm

PARAMETERS  
  throt_sm_ptr         - Pointer to the throttling SM - opaque handle from client
  throt_state_ptr      - Pointer to the throt_state - OUT param 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If all went fine
  FALSE - If anything fails.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_get_throttling_state
(
  void                    *throt_sm_ptr, 
  boolean                 *throt_state_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  ds_3gpp_throttle_sm_s *throt_sm_local_ptr = NULL;
  boolean                ret_val = FALSE;
  int                    core_sm_index = DS_3GPP_MAX_PLMN_SUPPORTED;
  sys_plmn_id_s_type     current_plmn;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------------
    Memset the current plmn - this is a local 
  ----------------------------------------------------------------------------*/
  memset((void*)&current_plmn,0,sizeof(sys_plmn_id_s_type));

  /*----------------------------------------------------------------------------
    Query the Current PLMN that is maintained in PDN context
  ----------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  /*----------------------------------------------------------------------------
    Check if the incoming pointer to sys wide srv throt sm is NULL.
    Also ensure that the current PLMN is non-NULL
  ----------------------------------------------------------------------------*/
  if( (throt_sm_ptr != NULL) &&
      (throt_state_ptr != NULL) &&
      (ds_3gpp_throt_sm_is_plmn_non_null(current_plmn) == TRUE) )
  {
    throt_sm_local_ptr = (ds_3gpp_throttle_sm_s*)throt_sm_ptr;

    core_sm_index = ds_3gpp_throt_sm_get_core_sm_from_plmn(throt_sm_local_ptr,
                                                                current_plmn);

    /*--------------------------------------------------------------------------
      Check if a core SM is present. If yes go ahead and query the core sm about
      the throttling state
    --------------------------------------------------------------------------*/
    if( DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index) )
    {      
      DS_3GPP_MSG0_MED("core SM present. Querying is_throt_flag");

      *throt_state_ptr = 
             ds_3gppi_throt_sm_get_is_throttled_flag
               (DS_3GPP_THROT_SM_CORE_SM_PTR(core_sm_index, subs_id));
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Core SM not present. is_throt_flag set to TRUE");
      *throt_state_ptr = TRUE;
    }
    ret_val = TRUE;
  } // valid incoming argument
  else
  {
    DS_3GPP_MSG0_ERROR("Opaque hdl NULL. Get timer value bailing out");
  }
  return ret_val;
} /* ds_3gpp_throt_sm_get_throttling_state */

/*===========================================================================
FUNCTION DS_3GPPI_GET_USER_DATA

DESCRIPTION
  This function returns the user_data contained in the core throttle sm.
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  ds_3gpp_throt_user_data
 
SIDE EFFECTS 
  None.
  
===========================================================================*/

ds_3gpp_throt_user_data *ds_3gppi_throt_sm_get_user_data
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr
)
{
   ds_3gpp_throt_user_data  *throt_user_data_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(throt_sm_ptr != NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    throt_user_data_p = &throt_sm_ptr->user_data;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL argument passed in clear_user_data. Ignore");
  }

  return throt_user_data_p;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION DS_3GPP_THROT_GET_FAILURE_TIMER_FROM_PROFILE

DESCRIPTION
  This function returns the throttle failure timer based on the failure counter
  This value is fetched from the configured profile values

PARAMETERS  
  uint8                   failure_counter
  ds_pdn_context_s       *pdn_context_p
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void *
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_throt_sm_get_failure_timer_from_profile
(
  uint8                   failure_counter,
  ds_pdn_context_s       *pdn_context_p
)
{
  uint32 timer_val = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return timer_val;
  }
  
  if((failure_counter > DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS) || 
     (failure_counter < DS_3GPP_PDN_THROTTLE_MIN_FAILURE_TIMERS)) 
  {
    DS_3GPP_MSG1_ERROR("Failure counter is invalid:%d",failure_counter);
    return timer_val;
  }

  timer_val = 
    SEC_TO_MSEC(pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.
                failure_timers[failure_counter-1]);

  DS_3GPP_MSG1_HIGH("Failure timer returned is %d msec",timer_val);
  return timer_val;

}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_THROTTLE_FAILURE_TIMER

DESCRIPTION
  This function returns the throttle failure timer based on cause_code

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cause_code - cause_code provided by network in reject message
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void *
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_throt_sm_get_throttle_failure_timer
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  void                   *cause_code,
  ds_pdn_context_s       *pdn_context_p,
  boolean                 srv_req_throt     
)
{
  uint32           throt_tmr_val = 0;
  boolean          cc_sp_throttle_needed = FALSE;
  uint32           throt_failure_cmp_val[DS_3GPP_MAX_FAILURE_TIMERS] = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (throt_sm_ptr->throttle_sm_get_timer_val_cb != NULL)
  {
    throt_tmr_val =
      throt_sm_ptr->throttle_sm_get_timer_val_cb
        ((void*)throt_sm_ptr, throt_sm_ptr->throttle_sm_get_timer_val_cb_data);
  }
  else
  {
    /*---------------------------------------------------------------------------- 
      For CC SP get the timer value from below
      ----------------------------------------------------------------------------*/
    if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(throt_sm_ptr->subs_id) == TRUE)
    {
      cc_sp_throttle_needed = TRUE;
    }
    /*---------------------------------------------------------------------------- 
      If srv req throttling is enabled get the timer value from the EFS
      ----------------------------------------------------------------------------*/
    else if(srv_req_throt)
    {
      throt_tmr_val = ds_3gpp_srv_req_throt_get_failure_timer(
                                                 throt_sm_ptr->failure_count,
                                                 throt_sm_ptr->subs_id);

     /*-------------------------------------------------------------------------- 
       Get the random throttling timers.
       Also if min_ramdom_timer is non zero and max_random_timer is zero then
       Set the max_random_timer to min_random_timer
      --------------------------------------------------------------------------*/
      ds_3gpp_srv_req_throt_get_random_timers(throt_sm_ptr->failure_count,
                                         throt_sm_ptr->subs_id,
                                        &throt_sm_ptr->min_random_timer_val,
                                        &throt_sm_ptr->max_random_timer_val);

      if(((throt_sm_ptr->max_random_timer_val == 0) && 
         (throt_sm_ptr->min_random_timer_val != 0)) || 
         (throt_sm_ptr->max_random_timer_val < throt_sm_ptr->min_random_timer_val))
      {
         throt_sm_ptr->max_random_timer_val = throt_sm_ptr->min_random_timer_val;
      }

      DS_3GPP_MSG3_HIGH("Srv req throttle timer values in secs are:throt_timer_val:%d:"
                        "rand_min:%d:rand_max:%d",throt_tmr_val,
                         throt_sm_ptr->min_random_timer_val,
                         throt_sm_ptr->max_random_timer_val);
    }
    else
    {


     /*------------------------------------------------------------------------------ 
       Check if profile is configured. If it is configured read from the profile
       If not configured, read from the EFS
       ------------------------------------------------------------------------------*/

      if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p) &&
          (memcmp(pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.failure_timers,
           throt_failure_cmp_val,(sizeof(uint32)*DS_3GPP_MAX_FAILURE_TIMERS)) != 0))
      {

    /*--------------------------------------------------------------------------
       Get the configured timer values from profile
       --------------------------------------------------------------------------*/
        throt_tmr_val = 
                     ds_3gpp_throt_sm_get_failure_timer_from_profile(
                                                      throt_sm_ptr->failure_count,
                                                      pdn_context_p);

        DS_3GPP_MSG2_HIGH("Timer Value from Profile :%d for cause_code: %d", 
                          throt_tmr_val,(uint32)cause_code);
      }
      else
      {
        throt_tmr_val = ds_3gpp_pdn_throt_get_failure_timer(throt_sm_ptr->failure_count,
                                                          throt_sm_ptr->subs_id);

      }
    }  
  }

  if(cc_sp_throttle_needed == TRUE)
  {

     throt_tmr_val = ds_3gpp_cc_sp_throt_get_timer( 
   	                 ((ps_iface_net_down_reason_type)cause_code)&(0x0000FFFF), 
                                             throt_sm_ptr->failure_count,
                                             throt_sm_ptr->subs_id);
     DS_3GPP_MSG2_HIGH("Timer Value from EFS :%d for cause_code: %d ", 
                       throt_tmr_val, (uint32)(cause_code));
  }

  return throt_tmr_val;

} /*ds_3gpp_throt_sm_get_throttle_failure_timer*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_ENABLE_CC_SP_THROTTLING

DESCRIPTION
  This function returns the NV item value of the cc_sp_throttling

PARAMETERS  
None
DEPENDENCIES 
  None.
  
RETURN VALUE 
boolean
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_set_cc_sp_throttling
(
   sys_modem_as_id_e_type  subs_id,
   boolean value
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ds_3gpp_cc_sp_throttling[subs_id] = value;
}/*ds_3gpp_pdn_context_enable_cc_sp_throttling*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_IS_CC_SP_THROTTLING_ENABLED

DESCRIPTION
  This function returns the NV item value of the cc_sp_throttling

PARAMETERS  
None
DEPENDENCIES 
  None.
  
RETURN VALUE 
boolean
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_is_cc_sp_throttling_enabled
(
  sys_modem_as_id_e_type  subs_id
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;    
  }

 return ds_3gpp_cc_sp_throttling[subs_id];
}/*ds_3gpp_throt_sm_is_cc_sp_throttling_enabled*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_CORE_SM_PTR
 
DESCRIPTION
  This function fetches the Core SM Ptr corresponding to a particular index
  and subs id.
  
PARAMETERS   : 
  1. Core SM Index
  2. Subs Id

DEPENDENCIES
  None.

RETURN VALUE
  Throt SM Ptr
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gppi_throttle_sm_s* ds_3gpp_throt_sm_get_core_sm_ptr
(
  int                    core_sm_index, 
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gppi_throttle_sm_s*     throttle_sm_p = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  /*---------------------------------------------------------------------------*/
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  if(ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_ERROR, "invalid subs index:%d return error",
                     subs_indx);
    return NULL;
  }

  if (!DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index))
  {
    DS_3GPP_MSG1_ERROR("Core sm index %d is invalid",core_sm_index);
    return NULL;
  }

  if (ds_3gpp_service_req_throt_sm_ptr[subs_indx] != NULL)
  {
    throttle_sm_p = ds_3gpp_service_req_throt_sm_ptr[subs_indx]->
                        sm_ptr[core_sm_index];
  }
  else
  {
    DS_3GPP_MSG2_ERROR("Throttle SM is NULL for Core SM index %d and Subs "
                       "Index %d ",core_sm_index ,subs_indx);
  }

  return throttle_sm_p ; 
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_SET_CORE_SM_PTR
 
DESCRIPTION
  This function sets the Core Throt SM Ptr corresponding to a particular Index
  and Subs Id.
  
PARAMETERS   : 
  1. Core SM Index
  2. Subs Id
  3. Throt SM Ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_throt_sm_set_core_sm_ptr
(
  int                         core_sm_index, 
  sys_modem_as_id_e_type      subs_id,
  ds_3gppi_throttle_sm_s*     throt_sm_ptr
)
{
  /*--------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if (!DS_3GPP_THROT_SM_VALIDATE_CORE_SM_INDEX(core_sm_index))
  {
    DS_3GPP_MSG1_ERROR("Core sm index %d is invalid",core_sm_index);
    return;
  }

  DS_3GPP_MSG2_MED ("Core SM Ptr for Subs Id: %d is 0x%x", 
                     subs_id,throt_sm_ptr);

  ds_3gpp_service_req_throt_sm_ptr[subs_id]->sm_ptr[core_sm_index] = 
    throt_sm_ptr;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_SYS_SEL_PREF_ERR_CMD_HDLR
 
DESCRIPTION
  This function handles when sys selection enable/disable has resulted in
  an error, It establishes a retry mechanism to enable/disable LTE
  
PARAMETERS   : 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_throt_sm_sys_sel_pref_err_cmd_hdlr
(
  void
)
{
  uint16 retry_timer_val = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  retry_timer_val = ds_3gpp_throt_sys_sel_retry_sm.retry_timer_val;

  /*---------------------------------------------------------------------
    Retry mechanism is
    1,2,4,8,16,16,16 until retry timer is reset
  -----------------------------------------------------------------------*/
  if (retry_timer_val < DS_3GPP_SYS_SEL_RETRY_MAX_VAL)
  {
    if (retry_timer_val == 0 )
    {
      retry_timer_val = 1;
    }
    else
    {
      retry_timer_val = (retry_timer_val << 1); /* 2 ^ retry_timer_val */
    }
  }

  ds_3gpp_throt_sys_sel_retry_sm.retry_timer_val = retry_timer_val;

  DS_3GPP_MSG1_HIGH("Retry Timer value in secs:%d",retry_timer_val);

  retry_timer_val = retry_timer_val * DS_3GPP_SECOND_TO_MILISECOND_MULTIPLIER;

  /*------------------------------------------------------------------------ 
    Start the Retry timer 
  -------------------------------------------------------------------------*/
  (void)rex_set_timer(&(ds_3gpp_throt_sys_sel_retry_sm.sys_sel_retry_timer), 
                        retry_timer_val);

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_SYS_SEL_PREF_NO_ERR_CMD_HDLR
 
DESCRIPTION
  This function handles when sys selection enable/disable has resulted in
  an no error, This function clears all the retry/timer values
  
PARAMETERS   : 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_throt_sm_sys_sel_pref_no_err_cmd_hdlr
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DATA_LTE
  if (ds_3gpp_throt_sys_sel_retry_sm.retry_type == DS_3GPP_LTE_DISABLE_RETRY)
  {
    DS_3GPP_MSG0_MED("LTE disabled");
    ds3gpp_set_ds_lte_flag(FALSE,ds_3gpp_throt_sys_sel_retry_sm.subs_id); 
  }
  else if(ds_3gpp_throt_sys_sel_retry_sm.retry_type ==
            DS_3GPP_LTE_ENABLE_RETRY)
  {
    DS_3GPP_MSG0_MED("LTE enabled");
    ds3gpp_set_ds_lte_flag(TRUE,ds_3gpp_throt_sys_sel_retry_sm.subs_id); 
  }
#endif /* FEATURE_DATA_LTE */

  if (ds_3gpp_throt_sys_sel_retry_sm.retry_timer_val > 0)
  {
    DS_3GPP_MSG0_MED("Clearing sys sel retry timer");
    ds_3gpp_throt_sys_sel_clr_retry_sm();
  }

  return;
}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_RETRY_TIMER_EXPIRY

DESCRIPTION
  This function posts a cmd to retry sys selection enable/disable

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_retry_timer_expiry
(
     unsigned long callback_data /*unused*/
)
{
  ds_cmd_type *cmd_buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_throt_sys_sel_retry_timer_expiry()");

  if((cmd_buf = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    return;
  }
  else
  {
    cmd_buf->hdr.cmd_id = DS_CMD_3GPP_SYS_SEL_RETRY_TIMER_EXP;
    ds_put_cmd(cmd_buf);
  }

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_UPDATE_PLMN_BLK_INTERVAL_AFTER_RETRY

DESCRIPTION
  This function returns the value for which PLMN blocking has to be performed
  after retry
 
PARAMETERS  
  void
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean - Indicates if a retry must be performed or not.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_throt_update_plmn_blk_interval_and_retry
(
   void
)
{
  int32 blk_interval = 0;
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  blk_interval =  ds_3gpp_throt_sys_sel_retry_sm.plmn_blocking_info.
  blocking_interval - (ds_3gpp_throt_sys_sel_retry_sm.retry_timer_val *
                       DS_3GPP_SECOND_TO_MILISECOND_MULTIPLIER);

  if (blk_interval > 0)
  {
    ds_3gpp_throt_sys_sel_retry_sm.plmn_blocking_info.blocking_interval = 
      blk_interval;
    ret_val = TRUE;
  }

  DS_3GPP_MSG3_HIGH("ds_3gpp_throt_update_plmn_blk_interval_and_retry returns %d",
                    ret_val,0,0);
  return ret_val;

}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_RETRY_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function would retry to enable/disable sys selection

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_retry_timer_expiry_cmd_hdlr
(
   void
)
{
  cm_ph_cmd_cb_f_type *sys_sel_cb_f_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------- 
    Populate the sys_sel_cb_f_ptr for RETRY
    Also at this point the retry sm must always be cleared because DS
    has no other notifications from CM/NAS when the PLMN is unblocked.
  ---------------------------------------------------------------------*/
  sys_sel_cb_f_ptr = (cm_ph_cmd_cb_f_type *)ds3g_get_sys_sel_cb_fn_ptr();

  if (sys_sel_cb_f_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("sys_sel_cb_f_ptr is NULL");
  }


  DS_3GPP_MSG1_HIGH("ds_3gpp_throt_sys_sel_retry_timer_expiry_cmd_hdlr:"
                    "Retry:%d",ds_3gpp_throt_sys_sel_retry_sm.retry_type);

  switch (ds_3gpp_throt_sys_sel_retry_sm.retry_type)
  {

    case DS_3GPP_LTE_PLMN_BLOCK_RETRY:
      /*----------------------------------------------------------------- 
         Reduce the timer interval with the retry time that has elapsed
       ------------------------------------------------------------------*/
      if(ds_3gpp_throt_update_plmn_blk_interval_and_retry())
      {
    #ifdef FEATURE_DATA_LTE
        if(ds3g_msh_disable_lte_per_plmn_ex(ds_3gpp_throt_sys_sel_retry_sm.
                                            plmn_blocking_info) != TRUE)
        {
           DS_3GPP_MSG0_ERROR("Error while invoking CM API to block PLMN");
        }
   #endif /* FEATURE_DATA_LTE */
        ds_3gpp_throt_hdl_srv_throt_gen_event(ds_3gpp_throt_sys_sel_retry_sm.
                                              svc_event_type.counter,
                                              ds_3gpp_throt_sys_sel_retry_sm.
                                              svc_event_type.duration,
                                              ds_3gpp_throt_sys_sel_retry_sm.
                                              svc_event_type.is_throttled,
                                              ds_3gpp_throt_sys_sel_retry_sm.
                                              svc_event_type.throttle_action,
                                              ds_3gpp_throt_sys_sel_retry_sm.
                                              svc_event_type.subs_id,
                                              &ds_3gpp_throt_sys_sel_retry_sm.
                                              plmn_blocking_info.plmn);
      }
      else
      {
        DS_3GPP_MSG0_HIGH("No PLMN BLOCKING RETRY");
      } 
      break;

    case DS_3GPP_LTE_DISABLE_RETRY:
      if(cm_ph_cmd_sys_sel_pref_5(
                 sys_sel_cb_f_ptr,
                 NULL,
                 ds3g_get_cm_client_id(),
                 ds_3gpp_throt_sys_sel_retry_sm.subs_id,
                 0,
                 &ds_3gpp_throt_sys_sel_retry_sm.sys_sel_pref_params)!= TRUE)
      {
        DS_3GPP_MSG0_ERROR("Error while invoking CM API to disable LTE");
      }
      break;

    case DS_3GPP_LTE_ENABLE_RETRY:
      if(cm_ph_cmd_sys_sel_pref_5(
                 sys_sel_cb_f_ptr,
                 NULL,
                 ds3g_get_cm_client_id(),
                 ds_3gpp_throt_sys_sel_retry_sm.subs_id,
                 0,
                 &ds_3gpp_throt_sys_sel_retry_sm.sys_sel_pref_params)!= TRUE)
      {
        DS_3GPP_MSG0_ERROR("Error while invoking CM API to disable LTE");
      }
      break;

    case DS_3GPP_LTE_PLMN_UNBLOCK_RETRY:
 #ifdef FEATURE_DATA_LTE
     if(ds3g_msh_disable_lte_per_plmn_ex(ds_3gpp_throt_sys_sel_retry_sm.
                                            plmn_blocking_info) != TRUE)
     {
       DS_3GPP_MSG0_ERROR("Error while invoking CM API to block PLMN");
     }
 #endif /* FEATURE_DATA_LTE */
     break;
    default:
      DS_3GPP_MSG0_ERROR("Incorrect sys_sel retry type");
      break;
  }

  return;
}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_CLR_RETRY_SM

DESCRIPTION
  This function clears sys_sel_retry_sm 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_clr_retry_sm
(
   void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("sys_sel_clr_retry_sm");

  (void)rex_clr_timer(&(ds_3gpp_throt_sys_sel_retry_sm.sys_sel_retry_timer));

  /*------------------------------------------------------------------------ 
    Do not memset ds_3gpp_throt_sys_sel_retry_sm because retry timer will
    not be initialized
  ------------------------------------------------------------------------*/
  memset((void *)&ds_3gpp_throt_sys_sel_retry_sm.plmn_blocking_info,0,
         sizeof(cm_plmn_blocking_info_s_type)); 

  ds_3gpp_throt_sys_sel_retry_sm.retry_timer_val = 0;

  ds_3gpp_throt_sys_sel_retry_sm.retry_type = DS_3GPP_NO_SYS_SEL_RETRY;
}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_RETRY_TYPE

DESCRIPTION
  This function sets the retry type

PARAMETERS
  retry_type           
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_retry_type
(
   ds_3gpp_throt_sys_sel_retry_e_type     retry_type
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds_3gpp_throt_sys_sel_retry_sm.retry_type = retry_type;
  return;

}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_SUBS_ID

DESCRIPTION
  This function sets the subs id

PARAMETERS
  subs_id           
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_subs_id
(
   sys_modem_as_id_e_type  subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds_3gpp_throt_sys_sel_retry_sm.subs_id = subs_id;
  return;
}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_PLMN_BLK_INFO

DESCRIPTION
  This function sets the PLMN BLK INFO, This is needed to 
  recalculate the retry time when a disable fails.

PARAMETERS 
  PLMN blocking info BLOB 
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_plmn_blk_info
(
   cm_plmn_blocking_info_s_type *plmn_blocking_info_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (plmn_blocking_info_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("sys_sel_params_p is NULL");
    return;
  }

  memscpy((void*)&(ds_3gpp_throt_sys_sel_retry_sm.plmn_blocking_info),
          sizeof(cm_plmn_blocking_info_s_type),
          (void*)(plmn_blocking_info_p), 
          sizeof(cm_plmn_blocking_info_s_type));

}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_SVC_THROT_EV_INFO

DESCRIPTION
  This function sets the svc throttle info necessary for logging 

PARAMETERS 
  
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_svc_throt_ev_info
(
   ds_3gpp_throt_svc_event_type *svc_throt_ev_info
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (svc_throt_ev_info == NULL)
  {
    DS_3GPP_MSG0_ERROR("svc_throt_ev_info is NULL");
    return;
  }

  memscpy((void*)&(ds_3gpp_throt_sys_sel_retry_sm.svc_event_type),
          sizeof(ds_3gpp_throt_svc_event_type),
          (void*)(svc_throt_ev_info), 
          sizeof(ds_3gpp_throt_svc_event_type));
}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SRV_THROT_EV_ALLOC_MEM

DESCRIPTION
  This function would allocate memory for svc_throt_event_ptr from heap

PARAMETERS
  Void
 
DEPENDENCIES
  None.

RETURN VALUE 
  ds_3gpp_throt_svc_event_type ptr
 
SIDE EFFECTS
  None.

===========================================================================*/

ds_3gpp_throt_svc_event_type *ds_3gpp_throt_srv_throt_ev_alloc_mem
(
   void 
)
{
  ds_3gpp_throt_svc_event_type *srv_throt_ev_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  srv_throt_ev_ptr = (ds_3gpp_throt_svc_event_type *)modem_mem_alloc
                     (sizeof(ds_3gpp_throt_svc_event_type),
                      MODEM_MEM_CLIENT_DATA);

  if (srv_throt_ev_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Throt ev ptr mem cant be allocated from heap");
  }

  return srv_throt_ev_ptr;
}


/*===========================================================================
FUNCTION  DS_3GPP_THROT_SRV_THROT_GEN_EVENT

DESCRIPTION
  This function would generate an event when service request throttling
  happens

PARAMETERS
  ds_3gpp_throt_svc_event_type  ptr
 
DEPENDENCIES
  The ptr must be populated with appropriate values

RETURN VALUE 
  void
 
SIDE EFFECTS
  None.

===========================================================================*/

void  ds_3gpp_throt_srv_throt_gen_event
(
   ds_3gpp_throt_svc_event_type *srv_throt_ev_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (srv_throt_ev_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("srv_throt_ev_ptr passed is unallocated");
    return;
  }

  DS_3GPP_MSG0_MED("Generating SRV REQ THROT MSG");

  event_report_payload(EVENT_DS_3GPP_SVC_THROTTLE,
                      sizeof(ds_3gpp_throt_svc_event_type),
                      (void*)srv_throt_ev_ptr);
}


/*===========================================================================
FUNCTION  DS_3GPP_THROT_HDL_SRV_THROT_GEN_EVENT

DESCRIPTION
  This function would populate the structure and generate an event
  when service request throttling happens

PARAMETERS
   uint8                            failure_count
   uint16                           timer_val
   boolean                          is_throttled
   ds_3gpp_throt_svc_action_e_type  svc_action_type
   sys_modem_as_id_e_type           subs_id
   sys_plmn_id_s_type               *plmn_id
 
DEPENDENCIES

RETURN VALUE 
  void
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_hdl_srv_throt_gen_event
(
   uint8                            failure_count,
   uint32                           timer_val,
   boolean                          is_throttled,
   ds_3gpp_throt_svc_action_e_type  svc_action_type,
   sys_modem_as_id_e_type           subs_id,
   sys_plmn_id_s_type               *plmn_id
)
{
  ds_3gpp_throt_svc_event_type *srv_throt_ev_ptr = NULL; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------ 
    Perform necessary action for SRV THROTTLING EV LOGGING
    After generating the event, free the memory allocated from heap
  --------------------------------------------------------------------------*/
  srv_throt_ev_ptr = ds_3gpp_throt_srv_throt_ev_alloc_mem();

  if (srv_throt_ev_ptr != NULL)
  {
    memset((void *)srv_throt_ev_ptr,0,sizeof(ds_3gpp_throt_svc_event_type));
    srv_throt_ev_ptr->counter = failure_count;
    srv_throt_ev_ptr->duration = (uint16)(timer_val/1000);
    srv_throt_ev_ptr->is_throttled = is_throttled;
    srv_throt_ev_ptr->throttle_action = (uint8)svc_action_type;
    srv_throt_ev_ptr->subs_id = (uint8)subs_id;
    ds_3gpp_extract_mcc_mnc_from_plmn_id(plmn_id,
                                         &srv_throt_ev_ptr->mcc,
                                         &srv_throt_ev_ptr->mnc);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("srv_throt_ev_ptr unallocated from heap");
    return;
  }

  ds_3gpp_throt_srv_throt_gen_event(srv_throt_ev_ptr);

  ds_3gpp_throt_sys_sel_set_svc_throt_ev_info(srv_throt_ev_ptr);

  modem_mem_free((void *)srv_throt_ev_ptr, MODEM_MEM_CLIENT_DATA);

  srv_throt_ev_ptr = NULL;
}
/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_SYS_SEL_PREF_PARAMS

DESCRIPTION
  This function sets the sys_sel_pref_params, This is needed to 
  send out the LTE disable retry request

PARAMETERS 
  sys_sel_pref_params_p 
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_sys_sel_pref_params
(
   cm_sys_sel_pref_params_s_type *sys_sel_pref_params_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (sys_sel_pref_params_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("sys_sel_params_p is NULL");
    return;
  }

  memscpy((void*)&(ds_3gpp_throt_sys_sel_retry_sm.sys_sel_pref_params),
          sizeof(cm_sys_sel_pref_params_s_type),
          (void*)(sys_sel_pref_params_p), 
          sizeof(cm_sys_sel_pref_params_s_type));
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_PROCESS_CMDS

DESCRIPTION
  This function processes the commands sent to Throttle SM.

PARAMETERS  
  CMD Ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_process_cmds
(
  const ds_cmd_type *cmd_ptr 
)
{
  unsigned long cb_data;

  ASSERT(cmd_ptr != NULL);
  ASSERT(cmd_ptr->cmd_payload_ptr != NULL);

  switch (cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_THROT_TMR_EXP:
    {
      cb_data = (unsigned long)(*((unsigned long*)(cmd_ptr->cmd_payload_ptr)));
      ds_3gppi_throt_sm_timer_exp_cmd_hdlr( cb_data );
      break;
    }
#ifdef FEATURE_DATA_LTE	
    case DS_CMD_3GPP_PLMN_BLOCK_TMR_EXP_CMD:
    {
      ds_eps_pdn_throt_plmn_unblock_timer_exp_hdlr
                             (*(unsigned long*)cmd_ptr->cmd_payload_ptr);

      break;
    }
    case DS_CMD_3GPP_EPS_THROT_TMR_EXP_CMD:
    {
       ds_eps_pdn_throt_sm_timer_exp_hdlr
                    (*(unsigned long*)cmd_ptr->cmd_payload_ptr);
       break;
    }
    case DS_CMD_3GPP_EPS_THROT_T3402_TMR_EXP_CMD:
    {
       ds_eps_pdn_throt_t3402_timer_expiry_cmd_hdlr
              (*(unsigned long*)cmd_ptr->cmd_payload_ptr);
        break;
   }
#endif /*FEATURE_DATA_LTE*/
    case DS_CMD_3GPP_RAU_COMPLTE_CMD:
      if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(
	  	     *(sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr) == TRUE)
      {		
#ifdef FEATURE_DATA_LTE	
        ds_eps_pdn_throt_rau_complete_ind(
                 *(sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr);
#endif /*FEATURE_DATA_LTE*/	  
        ds_3gpp_pdn_throt_reset_pdn_throttle(
                *(sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr);
      }
      break;

    default:
    {
      DS_3GPP_MSG1_ERROR("Unrecognized cmd: %d, ignoring",cmd_ptr->hdr.cmd_id);
      break;
    }
  }

  return;
} /*ds_3gpp_throt_sm_process_cmds*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_IS_SRV_REG_THROTTLE_BEHAVIOUR_T3402_ENABLED

DESCRIPTION
  This function checks if T3402 throttle is enabled.

PARAMETERS  
  CMD Ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean TRUE :  T3402 enabled
          FALSE : T3402 disabled
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throttle_is_srv_req_throttle_behavior_t3402_enabled
(
  sys_modem_as_id_e_type  subs_id
)
{
   boolean t3402_enabled = FALSE;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return t3402_enabled;
  }


   if (ds_3gpp_srv_req_throttle_behavior[subs_id] == DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402) 
   {
      t3402_enabled = TRUE; 
   }
   DS_3GPP_MSG1_LOW("ds_3gpp_srv_req_throttle_behavior is %d", t3402_enabled);
   return t3402_enabled ;
}

/*===========================================================================
FUNCTION DS_3GPP_THROT_PREP_FOR_LTE_BLOCKING_AND_LOG

DESCRIPTION
  This function prepares all the necessary variables, throttle retry sm's
  before calling ds3gmshif function to actually block/disable LTE
 
PARAMETERS  
  sys_plmn_id_s_type      plmn_id,
  uint32                  timer_val,
  sys_modem_as_id_e_type  subs_id,
  uint8                   failure_count
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE                    If the function succeeds
  FALSE                   Otherwise
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_prep_for_lte_blocking_and_log
(
  sys_plmn_id_s_type      plmn_id,
  uint32                  timer_val,
  sys_modem_as_id_e_type  subs_id,
  uint8                   failure_count,
  boolean                 detach
)
{
  sys_plmn_id_s_type                 zero_plmn_id;
  cm_plmn_blocking_info_s_type       plmn_blocking_info;
  ds_3gpp_throt_sys_sel_retry_e_type retry_type = DS_3GPP_LTE_PLMN_BLOCK_RETRY;
  ds_3gpp_throt_svc_action_e_type    svc_action_type = DISABLE_LTE_PER_PLMN;
  boolean                            ret_val = FALSE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(! ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;    
  }

  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));

  if(memcmp((void*)&(plmn_id),
            (void*)&(zero_plmn_id), 
            sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG3_MED("PLMN being inspected is zero",0,0,0);
    return ret_val;
  }

  memset((void*)&plmn_blocking_info, 0,
                                    sizeof(cm_plmn_blocking_info_s_type));
  
  /*----------------------------------------------------------------------
    Populate the contents of the PLMN blocking structure
  ----------------------------------------------------------------------*/
  if (detach)
  {
    plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING;
  }
  else
  {
    plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY;
  }

  plmn_blocking_info.blocking_interval = timer_val;
  plmn_blocking_info.asubs_id = subs_id;
    
  memscpy((void*)&(plmn_blocking_info.plmn),sizeof(sys_plmn_id_s_type),
          (void*)&(plmn_id),sizeof(sys_plmn_id_s_type));


  subs_id = plmn_blocking_info.asubs_id;

  ds_3gpp_throt_sys_sel_clr_retry_sm();

  ds_3gpp_throt_sys_sel_set_retry_type(retry_type);

  ds_3gpp_throt_sys_sel_set_plmn_blk_info(&plmn_blocking_info);

#ifdef FEATURE_DATA_LTE
  ret_val = ds3g_msh_disable_lte_per_plmn_ex(plmn_blocking_info);
#endif /* FEATURE_DATA_LTE */

  if (ret_val)
  {
    ds_3gpp_throt_hdl_srv_throt_gen_event(failure_count,timer_val,TRUE,
                                          svc_action_type,subs_id,&plmn_id);
  }

  return ret_val;

}

/*===========================================================================
FUNCTION DS_3GPP_THROT_IS_SERV_REQ_THROT_SM

DESCRIPTION
  This function checks whether the throt_sm_ptr passed is an 
   instance of service req throttling.
 
PARAMETERS  
  ds_3gppi_throttle_sm_s      throt_sm_ptr,
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE                    If throt_sm_ptr is service_req_throt_sm
  FALSE                   Otherwise
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_is_serv_req_throt_sm
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
)
{
  int                         serv_req_core_sm_index = 0;
  sys_plmn_id_s_type          current_plmn_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL throttle_sm_ptr");
    return FALSE;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(throt_sm_ptr->subs_id);
  if(ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_ERROR, "invalid subs index:%d return error",
                     subs_indx);
    return FALSE;
  }

  memset(&current_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  ds3g_get_current_plmn_id(&current_plmn_id, throt_sm_ptr->subs_id);

  serv_req_core_sm_index = ds_3gpp_throt_sm_get_core_sm_from_plmn
                     (
                       ds_3gpp_service_req_throt_sm_ptr[subs_indx],
                       current_plmn_id
                     );

  if (throt_sm_ptr ==
      DS_3GPP_THROT_SM_CORE_SM_PTR(serv_req_core_sm_index, throt_sm_ptr->subs_id))
  {
    DS_3GPP_MSG1_HIGH("Service request Throttle SM: 0x%x", throt_sm_ptr);
    return TRUE;
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Not service request throt sm: 0x%x", throt_sm_ptr);
    return FALSE;
  }
}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_GET_SRV_REQ_THROTTLE_BEHAVIOR

DESCRIPTION
  This function returns the 3GPP srv req throttle behavior

PARAMETERS
   subs id - subs id for the throttle behavior            
 
DEPENDENCIES
  None.

RETURN VALUE 
   ds_3gpp_srv_req_throttle_behavior - 3GPP srv req throttle behavior

SIDE EFFECTS
  None.

===========================================================================*/
ds_3gpp_srv_req_throttle_behavior_e ds_3gpp_throt_get_srv_req_throttle_behavior
(
  sys_modem_as_id_e_type   subs_id
)
{
  DS_3GPP_MSG1_HIGH("srv req throttle behavior %d", 
                    ds_3gpp_srv_req_throttle_behavior);
  return ds_3gpp_srv_req_throttle_behavior[subs_id];
}

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SEND_PER_RAT_THROT_INFO

DESCRIPTION
  This function sends per rat throttling information to PS SYS CLients
  and MPPM.
  
PARAMETERS 
  ds3gevent_mgr_throttle_info_type*  - Throttling Info Pointer
  sys_plmn_id_s_type                 - Current PLMN
  ds3geventmgr_filter_type           - Filter Info,
  ds3gevent_mgr_throt_rat_type       - RAT Priority list
  uint8                              - Length of RAT Priority List
  sys_modem_as_id_e_type             - Subs id          

DEPENDENCIES
  None.

RETURN VALUE 
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_throt_send_per_rat_throt_info
(
  ds3gevent_mgr_throttle_info_type *throttle_info_p,
  sys_plmn_id_s_type                current_plmn_id,
  ds3geventmgr_filter_type          filter_info,
  ds3gevent_mgr_throt_rat_type     *priority_rat_arr,
  uint8                             priority_rat_arr_len,
  sys_modem_as_id_e_type               subs_id
)
{
  ds_3gpp_throt_rat_e_type              rat_index = 0;
  ds3gevent_mgr_throt_rat_type          index = 0;
  int16                                 ps_error = -1;
  /*-----------------------------------------------------------------------*/

  ASSERT(throttle_info_p != NULL);
  ASSERT(priority_rat_arr != NULL);

  /* Determine the current RAT */ 
  rat_index = ds_3gpp_throt_map_cm_sys_mode_to_rat_type
                ((ds_3gpp_pdn_throt_determine_mode(subs_id)));

  /*------------------------------------------------------------------------
    For current plmn, get throttle info for all apns 
   -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
    (&current_plmn_id, &(throttle_info_p->pdn_throttle_info), 
     rat_index, subs_id);

  ps_sys_event_ind_ex(
                      PS_SYS_TECH_3GPP,
                      PS_SYS_EVENT_PDN_THROTTLE_INFO,
                      (ps_sys_subscription_enum_type)
                        ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                      (void *)&(throttle_info_p->pdn_throttle_info),
                       &ps_error
                      );

#ifdef FEATURE_DATA_WLAN_MAPCON

  for (index=0; index < priority_rat_arr_len; index++)
  {
    memset(throttle_info_p,0, sizeof(ds3gevent_mgr_throttle_info_type));

    throttle_info_p->throttled_rat = priority_rat_arr[index];

    if (throttle_info_p->throttled_rat == DS3GEVENT_MGR_THROT_RAT_LTE)
    {
      rat_index = DS_3GPP_THROT_RAT_LTE;
    }
    else if (throttle_info_p->throttled_rat == DS3GEVENT_MGR_THROT_RAT_UMTS)
    {
      rat_index = DS_3GPP_THROT_RAT_UMTS;
  }

    ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
      (&current_plmn_id, &(throttle_info_p->pdn_throttle_info), rat_index, subs_id);
  
    DS_3GPP_MSG2_HIGH("Generated System PDN throttle event, DS3G RAT Index: %d "
                      "with ps_error: %d", 
                      throttle_info_p->throttled_rat, ps_error);

    if(ds3geventmgr_set_event_info(DS3GEVENTMGR_PDN_THROTTLE_INFO_EV,
                                   (void *)throttle_info_p,
                                   &filter_info) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Unable to provide throttle information to internal"
                         " clients");
    }     
  }
#endif /* #ifdef FEATURE_DATA_WLAN_MAPCON */
} /* ds_3gpp_throt_send_per_rat_throt_info */

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SEND_PER_RAT_THROT_INFO

DESCRIPTION
  This function sends per rat throttling information to PS SYS CLients
  and MPPM.

PARAMETERS
  ds3gevent_mgr_throttle_info_type*  - Throttling Info Pointer
  sys_plmn_id_s_type                 - Current PLMN
  ds3geventmgr_filter_type           - Filter Info,
  ds3gevent_mgr_throt_rat_type       - RAT Priority list
  uint8                              - Length of RAT Priority List
  sys_modem_as_id_e_type             - Subs id          
 
DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  None.
===========================================================================*/

static void ds_3gpp_throt_send_throt_info
(
  ds3gevent_mgr_throttle_info_type *throttle_info_p,
  sys_plmn_id_s_type                current_plmn_id,
  ds3geventmgr_filter_type          filter_info,
  ds3gevent_mgr_throt_rat_type     *priority_rat_arr,
  uint8                             priority_rat_arr_len,
  sys_modem_as_id_e_type            subs_id
)
{
  ds_3gpp_throt_rat_e_type              rat_index = 0;
  ds3gevent_mgr_throt_rat_type          index = 0;
  int16                                 ps_error = -1;
  /*-----------------------------------------------------------------------*/

  ASSERT(throttle_info_p != NULL);
  ASSERT(priority_rat_arr != NULL);

  /* Use the global RAT index */ 
  rat_index = DS_3GPP_THROT_RAT_GLOBAL;
  /*------------------------------------------------------------------------
    For current plmn, get throttle info for all apns 
   -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
    (&current_plmn_id, &(throttle_info_p->pdn_throttle_info), rat_index, subs_id);

  ps_sys_event_ind_ex(
                      PS_SYS_TECH_3GPP,
                      PS_SYS_EVENT_PDN_THROTTLE_INFO,
                      (ps_sys_subscription_enum_type)
                        ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                      (void *)&(throttle_info_p->pdn_throttle_info),
                       &ps_error
                      );

#ifdef FEATURE_DATA_WLAN_MAPCON

  for (index=0; index < priority_rat_arr_len; index++)
  {
    throttle_info_p->throttled_rat = priority_rat_arr[index];

    DS_3GPP_MSG2_HIGH("Generated System PDN throttle event, DS3G RAT Index: %d "
                       "with ps_error: %d", 
                       throttle_info_p->throttled_rat, ps_error);
 
    if(ds3geventmgr_set_event_info(DS3GEVENTMGR_PDN_THROTTLE_INFO_EV,
                                   (void *)throttle_info_p,
                                   &filter_info) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Unable to provide throttle information to internal"
                         " clients");
    }     
  }

#endif /* FEATURE_DATA_WLAN_MAPCON */
} /* ds_3gpp_throt_send_throt_info */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_ADVERTISE_BLOCKED_APNS_TO_MPPM

DESCRIPTION
  This function sends the currently blocked list of of APNs to MPPM
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE 
  void 
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm
(
  sys_modem_as_id_e_type               subs_id
)
{
  sys_plmn_id_s_type                    current_plmn_id;
  /*--------------------------------------------------------------------- 
    Passing the throttling information to MPPM based on the current PLMN
    --------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn_id, subs_id);

  ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm_on_plmn (subs_id, current_plmn_id);

}/* ds_3gpp_pdn_throt_advertise_blocked_apns_to_mppm */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_ADVERTISE_BLOCKED_APNS_TO_MPPM

DESCRIPTION
  This function sends the currently blocked list of of APNs to MPPM
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE 
  void 
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm_on_plmn
(
  sys_modem_as_id_e_type               subs_id,
  sys_plmn_id_s_type               plmn_id
)
{
  ds3gevent_mgr_throttle_info_type     *throttle_info_p = NULL;
  ds3geventmgr_filter_type              filter_info;
  ds3gevent_mgr_throt_rat_type          priority_rat_arr
                                          [DS3GEVENT_MGR_THROT_RAT_MAX - 1];
  /*-------------------------------------------------------------------------*/

  DS_3GPP_MSG0_MED("Advertizing throttling information to MPPM");

  throttle_info_p = 
    (ds3gevent_mgr_throttle_info_type *)
       modem_mem_alloc(sizeof(ds3gevent_mgr_throttle_info_type),
                       MODEM_MEM_CLIENT_DATA_CRIT );

  if (throttle_info_p == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for "
                       " ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm");
    return;
  }

  memset(throttle_info_p,0,
         sizeof(ds3gevent_mgr_throttle_info_type));

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

  filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                                    ds3gsubsmgr_subs_id_cm_to_ds(subs_id);
  filter_info.tech = PS_SYS_TECH_3GPP;

  /*--------------------------------------------------------------------- 
    We populate a RAT Priority Array here.    
    When sending throttling information to MPPM, we need to send throttling 
    information on current RAT first. This is done to avoid race conditions 
    during ePDG-3GPP handovers.
  ----------------------------------------------------------------------*/
  if (ds3gpp_get_current_network_mode(subs_id) == SYS_SYS_MODE_LTE)
  {
    priority_rat_arr[0] = DS3GEVENT_MGR_THROT_RAT_LTE;
    priority_rat_arr[1] = DS3GEVENT_MGR_THROT_RAT_UMTS;
  }
  else
  {
    priority_rat_arr[0] = DS3GEVENT_MGR_THROT_RAT_UMTS;
    priority_rat_arr[1] = DS3GEVENT_MGR_THROT_RAT_LTE;
  }

  /*--------------------------------------------------------------------- 
    If PDN Throt per RAT is enabled, send throttling information on the
    current RAT to PS Sys Clients.
    If PDN Throt per RAT is not enabled, send throttling information on
    the global RAT to PS Sys Clients.
    Send throttle information on all RATs to MPPM - use RAT Priority Array
    to determine the order in which throttling information should be
    advertised to MPPM.
  ---------------------------------------------------------------------*/

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    ds_3gpp_throt_send_per_rat_throt_info(throttle_info_p, 
                                          plmn_id, 
                                          filter_info, 
                                          priority_rat_arr,
                                          DS3GEVENT_MGR_THROT_RAT_MAX - 1,
                                          subs_id);

  }
  else
  {
    ds_3gpp_throt_send_throt_info(throttle_info_p, 
                                  plmn_id, 
                                  filter_info, 
                                  priority_rat_arr,
                                  DS3GEVENT_MGR_THROT_RAT_MAX - 1,
                                  subs_id);

  } /* if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id)) */

  modem_mem_free(throttle_info_p, MODEM_MEM_CLIENT_DATA_CRIT); 

}/* ds_3gpp_pdn_throt_advertise_blocked_apns_to_mppm */

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_ADVERTISE_BLOCKED_APNS_TO_EXTERNAL_CLIENTS

DESCRIPTION
  This function sends the currently blocked list of of APNs to
  external clients
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE 
  void 
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_sm_advertise_blocked_apns_to_external_clients
(
  sys_modem_as_id_e_type               subs_id
)
{
  ps_sys_ioctl_pdn_throttle_info_type  *globally_blocked_apn_info = NULL;
  sys_plmn_id_s_type                    current_plmn_id;
  int16                                 ps_error = -1;
  ds_3gpp_throt_rat_e_type              rat_index = DS_3GPP_THROT_RAT_GLOBAL;
  /*-------------------------------------------------------------------------*/

  DS_3GPP_MSG0_MED("Advertizing throttling information to MPPM");

  globally_blocked_apn_info = (ps_sys_ioctl_pdn_throttle_info_type *)
            modem_mem_alloc(sizeof(ps_sys_ioctl_pdn_throttle_info_type),
                            MODEM_MEM_CLIENT_DATA_CRIT );

  if (globally_blocked_apn_info == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for "
                       " ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm");
    return;
  }

  
  memset(globally_blocked_apn_info,0,
         sizeof(ps_sys_ioctl_pdn_throttle_info_type));

  /*--------------------------------------------------------------------- 
    Passing the throttling information to MPPM based on the current PLMN
  ----------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn_id, subs_id);

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    rat_index = ds_3gpp_throt_map_cm_sys_mode_to_rat_type
                  ((ds_3gpp_pdn_throt_determine_mode(subs_id)));
  } 
  else
  {
    rat_index = DS_3GPP_THROT_RAT_GLOBAL;
  }

  /*------------------------------------------------------------------------
    For current plmn, get throttle info for all apns 
   -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
    (&current_plmn_id, globally_blocked_apn_info, rat_index, subs_id);

  ps_sys_event_ind_ex(
                      PS_SYS_TECH_3GPP,
                      PS_SYS_EVENT_PDN_THROTTLE_INFO,
                      (ps_sys_subscription_enum_type)
                        ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                      (void *)globally_blocked_apn_info,
                       &ps_error
                      );

  modem_mem_free(globally_blocked_apn_info, MODEM_MEM_CLIENT_DATA_CRIT);

} /* ds_3gpp_throt_sm_advertise_blocked_apns_to_external_clients */

/*===========================================================================
FUNCTION DS_3GPP_MAP_CM_SYS_MODE_TO_RAT_MASK

DESCRIPTION
  This function returns the RAT Mask corresponding to the given CM Sys Mode.
 
PARAMETERS  
  mode: CM Sys Mode
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  RAT Mask
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_map_cm_sys_mode_to_rat_mask
(
  sys_sys_mode_e_type mode
)
{
  uint32 rat_mask = 0;
/*----------------------------------------------------------------------*/

  switch (mode) 
  {
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      rat_mask = DS_3GPP_THROT_RAT_MASK_UMTS;
      break;
    case SYS_SYS_MODE_LTE:
      rat_mask = DS_3GPP_THROT_RAT_MASK_LTE;
      break;
    default:
      rat_mask = DS_3GPP_THROT_RAT_MASK_NONE;
      break;
  }
  return rat_mask;
} /* ds_3gpp_map_cm_sys_mode_to_rat_mask */

/*===========================================================================
FUNCTION DS_3GPP_MAP_CM_SYS_MODE_TO_RAT_TYPE

DESCRIPTION
  This function returns the RAT Type corresponding to the given CM Sys Mode.
 
PARAMETERS  
  mode: CM Sys Mode
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  RAT Type
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_3gpp_throt_rat_e_type ds_3gpp_throt_map_cm_sys_mode_to_rat_type
(
 sys_sys_mode_e_type sys_mode
)
{
  ds_3gpp_throt_rat_e_type rat_type = DS_3GPP_THROT_RAT_ILLEGAL;
/*----------------------------------------------------------------------*/

  switch (sys_mode) 
  {
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_TDS:
      rat_type = DS_3GPP_THROT_RAT_UMTS;
      break;

    case SYS_SYS_MODE_LTE:
      rat_type = DS_3GPP_THROT_RAT_LTE;
      break;

    default:
      rat_type = DS_3GPP_THROT_RAT_ILLEGAL;
      break;
  }
  return rat_type;
} /* ds_3gpp_throt_map_cm_sys_mode_to_rat_type */
