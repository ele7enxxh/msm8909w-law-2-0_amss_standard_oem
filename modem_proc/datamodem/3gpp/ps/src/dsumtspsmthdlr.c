/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  U M T S ( W C D M A / G P R S )  P A C K E T - S W I T C H E D  M O B I L E

                  T E R M I N A T E D   P A C K E T   H A N D L E R 
  
  
GENERAL DESCRIPTION
  This file contains data and function definitions for UMTS (wcdma/gprs)
  Mobile Terminated packet Mode-Specific Handler for PDP Type PPP, IP.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004 - 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/dsumtspsmthdlr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/19/12    nd     MT PDP pkt call.
05/09/12    nd     Memory reduction effort in 3gpp.
02/08/12    vs     Added support for 3GPP IPv6 over PPP
03/24/11    sa     Added MT PDP Auto answer support.
10/06/10    ttv    Added messages, which explain reason for asserting.
08/31/10    sa     Added support for default pdp type as IPv4v6.
11/15/07    asn    Added support for new feature MT-PDP terminating in TE
02/15/07    aw     Removed the banned string APIs.
06/09/06    vk     A round of Lint fixes.
03/13/06    rr     Replaced call_connected in dsumtsps_pdp_call_info with
                   call_state
05/11/05    rr     Fixed Lint errors/warnings.
11/30/04    mv     Added the entry in mt_timer_table to store the offered
                   PDP address.
11/24/04    mv     Fixed the bug, where incorrect parameters were being 
                   passed to the dsumtsps_store_encoded_apn() function.
10/21/04    mv     Created the file.   

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include <string.h>
#include "comdef.h"
#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)
#include "msg.h"
#include "dsm.h"
#include "rex.h"
#include "amssassert.h"
#include "ds3gmshif.h"
#include "ds3gmgr.h"
#include "ps_route.h"
#include "ps_iface.h"
#include "ds_3gpp_pdn_context.h"
#include "dsumtspdpreg.h"
#include "dsumtspsmthdlr.h"
#include "ds_3gpp_hdlr.h"
#include "dsumtspdpreg.h"
#include "smix.h"

#include <stringl/stringl.h>

#include "sm_common.h"
#include "ds_system_heap.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define DSUMTSPS_MT_TIMER_VAL 5000 /* MT timer value                       */

uint32 dsumtsps_mt_handle_base_val = 1;   /* Base value for MT handle      */

#ifdef FEATURE_DATA_TE_MT_PDP

#define DSUMTSPS_MT_TE_TIMER_VAL 10000

typedef enum
{
  DSUMTSPS_MT_UMSM_INVALID_STATE          = 0,
  DSUMTSPS_MT_UMSM_NULL_STATE             = 1,
  DSUMTSPS_MT_UMSM_BRINGING_RM_UP_STATE   = 2,
  DSUMTSPS_MT_UMSM_MAX_STATE              = 3
}dsumtsps_mt_umsm_state_enum_type;
#endif /* FEATURE_DATA_TE_MT_PDP */

/*---------------------------------------------------------------------------
   MT table for storing info while registering the callback. This table
   stores the apn name, pdp type, handle value and iface ptr.              
---------------------------------------------------------------------------*/
typedef struct          
{                                   
  byte                              apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
                                                 /* Stores the apn name    */

  uint8                             pdp_type;    /* PDP type,PPP or IP     */
  dss_iface_ioctl_mt_handle_type    handle;      /* handle to the callback */
  ps_iface_type                     *iface_ptr;  /*  iface pointer         */
}dsumtsps_mt_reg_table_type;       

/*---------------------------------------------------------------------------
  MT table that stores the info for timer, call_id etc for each incoming 
  request
---------------------------------------------------------------------------*/ 
typedef struct
{
  cm_call_id_type                   call_id;      /* Call id of inc page   */
  uint8                             pdp_type;     /* pdp type              */

  byte                              apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
                                                  /* Stores the apn        */

  word                              apn_len;      /* Length of the APN str */
  pdp_address_T                     address;      /* offered PDP address in 
                                                     the incoming page     */

  rex_timer_type*                   mt_timer_ptr; /* MT timer pointer      */

#ifdef FEATURE_DATA_TE_MT_PDP
  uint8                             mt_index;      /* Stores index to MT timer table */
  dsumts_rmsm_rm_dev_instance_type  rmsm_inst;     /* Corresponding RMSM instance */
  dsumtsps_mt_umsm_state_enum_type  um_state;      /* state of UM */
  rex_crit_sect_type                mt_um_crit_sec;/* Critical section */
#endif /* FEATURE_DATA_TE_MT_PDP */
}dsumtsps_mt_timer_table_type;    

dsumtsps_mt_reg_table_type *dsumtsps_mt_reg_table_ptr[DSUMTSPS_MT_MAX_CB];
                                       
dsumtsps_mt_timer_table_type *dsumtsps_mt_timer_table_ptr[DSUMTSPS_MT_MAX_CB];


#define DSUMTSPS_MT_INDEX_IS_VALID( mt_index ) \
  ( ((uint8)mt_index < DSUMTSPS_MT_MAX_CB) && (dsumtsps_mt_timer_table_ptr[mt_index]) )

#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================

FUNCTION dsumtsps_mt_umsm_clean_up_state

DESCRIPTION
  Cleanup state information from MT TIMER TABLE

PARAMETERS
  mt_index - index to MT TIMER TABLE

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

COMMENTS
  This routine is internal to DSUMTSPS MT HDLR

===========================================================================*/
void dsumtsps_mt_umsm_clean_up_state
(
  uint32 mt_index
)
{
  rex_timer_type *mt_timer_ptr;
  if ( !DSUMTSPS_MT_INDEX_IS_VALID(mt_index) )
  {
    DS_3GPP_MSG1_ERROR("MT_UMSM_CLEAN_UP: invalid index, mt_index [%d]",
                        mt_index);
    return;
  }

  DS_3GPP_MSG1_HIGH("MT_UMSM_CLEAN_UP: cleaning up MT-UMSM, mt_index [%d]",
                     mt_index);
  if( dsumtsps_mt_timer_table_ptr[mt_index] != NULL )
  {
    mt_timer_ptr = dsumtsps_mt_timer_table_ptr[mt_index]->mt_timer_ptr;
    dsumtsps_mt_timer_table_ptr[mt_index]->mt_timer_ptr = NULL;
  
    if( mt_timer_ptr != NULL )
    {
      (void) rex_clr_timer( mt_timer_ptr );
      rex_delete_timer_ex( mt_timer_ptr );
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Timer ptr is NULL, mt_index %d ", mt_index);
  }
  ds_system_heap_mem_free(dsumtsps_mt_timer_table_ptr[mt_index]);
  dsumtsps_mt_timer_table_ptr[mt_index] = NULL;

}/* dsumtsps_mt_umsm_clean_up_state() */


/*===========================================================================

FUNCTION dsumtsps_mt_umsm_transition_state

DESCRIPTION
  Handles event posted to MT_UMSM. Entry Actions are performed for that 
  MT_UMSM state and event posted

PARAMETERS
  mt_index: index into MT table
  new_um_state: MT_UMSM state
  *event_data_ptr: pointer to relevant data
  
DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  

COMMENTS
  This routine is internal to DSUMTSPS MT HDLR

===========================================================================*/
static void dsumtsps_mt_umsm_transition_state
(
  uint32 mt_index,
  dsumtsps_mt_umsm_state_enum_type new_um_state,
  void *event_data
)
{
  dsumts_rmsm_mt_info_type rmsm_mt_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( !DSUMTSPS_MT_INDEX_IS_VALID ( mt_index ) ) 
  {
    DS_3GPP_MSG1_ERROR("MT_UMSM_TRANS_STATE: invalid mt_index [%d]", 
                        mt_index);
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG3_MED("MT_UMSM_TRANS_STATE: mt_index [%d], new MT-UMSM state [%d]\
                    old MT-UMSM state [%d] ", mt_index, new_um_state,
                    dsumtsps_mt_timer_table_ptr[mt_index]->um_state);


  /* Change state */
  dsumtsps_mt_timer_table_ptr[mt_index]->um_state = new_um_state;

  /* Perform Entry Action into states */

  switch (new_um_state) 
  {
    case DSUMTSPS_MT_UMSM_BRINGING_RM_UP_STATE:
      /*-------------------------------------------------------------------------
        We currently pass 0 as the instance number. When we support multiple 
        Rm devices, we would need to use a different RMSM instance for each Rm 
        device
      -------------------------------------------------------------------------*/
    dsumtsps_mt_timer_table_ptr[mt_index]->rmsm_inst = 0;

    rmsm_mt_info.pdp_type  = dsumtsps_mt_timer_table_ptr[mt_index]->pdp_type;
    rmsm_mt_info.address   = dsumtsps_mt_timer_table_ptr[mt_index]->address;
    rmsm_mt_info.apn_len   = dsumtsps_mt_timer_table_ptr[mt_index]->apn_len;
    rmsm_mt_info.apn_name  = dsumtsps_mt_timer_table_ptr[mt_index]->apn_name;
    rmsm_mt_info.rmsm_inst = dsumtsps_mt_timer_table_ptr[mt_index]->rmsm_inst;
    rmsm_mt_info.mt_index  = dsumtsps_mt_timer_table_ptr[mt_index]->mt_index;

    dsumts_rmsm_mt_ring_te( &rmsm_mt_info );
    DS_3GPP_MSG0_HIGH("UMSM_TRANSITION_STATE: MT_UM_BRINGING_RMSM_UP_STATE\
                       entry actions completed!");
  break;

  case DSUMTSPS_MT_UMSM_NULL_STATE:
    /*-------------------------------------------------------------------------
      ENTRY ACTION into NULL STATE
    -------------------------------------------------------------------------*/
    dsumtsps_mt_umsm_clean_up_state( mt_index );

    DS_3GPP_MSG0_MED("UMSM_TRANSITION_STATE: MT_UM_NULL_STATE entry actions completed!");
    break;
  default:
    DS_3GPP_MSG1_HIGH("Unhandled UMSM_TRANSITION_STATE: %d", new_um_state);
  }
  return;
}/*dsumtsps_mt_umsm_transition_state()*/


/*===========================================================================

FUNCTION dsumtsps_mt_umsm_ev_hdlr

DESCRIPTION
  Handles events posted to MT_UMSM. Exit Actions are performed for current 
  state and state transition is performed.

PARAMETERS
  mt_index: index into MT table
  event: event for MT_UMSM
  *event_data_ptr: pointer to relevant data
  
DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  
COMMENTS
  This routine must be executed in DS context
  This routine is internal to DSUMTSPS MT HDLR

===========================================================================*/
static void dsumtsps_mt_umsm_ev_hdlr
(
  uint8 mt_index,
  dsumts_mt_umsm_ev_type event,
  void *event_data
)
{
  rex_timer_type*  mt_timer_ptr;
  dsumts_rmsm_instance_type inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( !DSUMTSPS_MT_INDEX_IS_VALID ( mt_index ) ) 
  {
    DS_3GPP_MSG1_ERROR("MT_UMSM_EV_HDLR: invalid mt_index %d", 
                        mt_index);
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG2_MED("MT_UMSM_EV_HDLR: current MT-UMSM state [%d], event posted [%d]",
                    dsumtsps_mt_timer_table_ptr[mt_index]->
                    um_state, event);

  /*--------------------------------------------------------------
    Perform EXIT Actions for the current state and then 
    do transition to new state
  --------------------------------------------------------------*/

  switch( event )
  {

  case MT_UMSM_CALL_RING_EV:
    /*---------------------------------------------------------------------------
      Perform exit actions:
      (1) Check that MT ANS timer has started
      (2) Mark RMSM for MT
      then, transition to MT_UM_NULL_STATE
    ---------------------------------------------------------------------------*/
    if ( dsumtsps_mt_timer_table_ptr[mt_index]->um_state == DSUMTSPS_MT_UMSM_NULL_STATE ) 
    { 
      dsumtsps_mt_umsm_transition_state( mt_index,
                                         DSUMTSPS_MT_UMSM_BRINGING_RM_UP_STATE,
                                         event_data );
    }
    else /* !MT_UM_NULL_STATE */
    {
      DS_3GPP_MSG2_ERROR("MT_UMSM_EV_HDLR: trap, MT-UMSM invalid state [%d], mt_index [%d]!",
                          dsumtsps_mt_timer_table_ptr[mt_index]->um_state, mt_index);
      ASSERT(0);
      return;
    }
  break;

  case MT_UMSM_CALL_REJECT_EV:
    /*---------------------------------------------------------------------------
      This event is posted by Rm interface, either by Rm MT Ring hdlr or RMSM on
      failing to bring-up call.
      Perform Exit Actions:
      (1) Stop and clear MT answer-timeout time
      then, transition to MT_UM_NULL_STATE
    ---------------------------------------------------------------------------*/
    if ( dsumtsps_mt_timer_table_ptr[mt_index]->um_state == DSUMTSPS_MT_UMSM_BRINGING_RM_UP_STATE )
    {
      /*--------------------------------------------------------------
        Exit Actions: stop timer, REJECT MT call and post REJECT_EV 
        to RMSM
      --------------------------------------------------------------*/
      mt_timer_ptr = dsumtsps_mt_timer_table_ptr[mt_index]->mt_timer_ptr;
      dsumtsps_mt_timer_table_ptr[mt_index]->mt_timer_ptr = NULL;

      if( mt_timer_ptr != NULL )
      {
        (void) rex_clr_timer( mt_timer_ptr );
        rex_delete_timer_ex( mt_timer_ptr );
      }

      /* The context is DS Task, REJECT MT-PDP call */
      dsumtsps_mt_send_reject( mt_index );

      DS_3GPP_MSG0_MED("MT_UMSM_EV_HDLR: MT_UM_BRINGING_RMSM_UP_STATE exit actions completed!");
      dsumtsps_mt_umsm_transition_state ( mt_index, 
                                          DSUMTSPS_MT_UMSM_NULL_STATE, event_data );
    }
    else
    {
      DS_3GPP_MSG2_ERROR("MT_UMSM_EV_HDLR: trapped REJECT_EV, \
                          MT-UMSM invalid state [%d], mt_index [%d]!",
                          dsumtsps_mt_timer_table_ptr[mt_index]->um_state,
                          mt_index);
      ASSERT(0);
      return;
    }    
    
  break;

  case MT_UMSM_CALL_ANS_TIMEOUT_EV:
    /*---------------------------------------------------------------------------
      This event is posted only by Rex Timer CB for MT answer timeout. Timer is 
      already stopped.
      Perform exit actions:
      (1) Post UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV to RMSM
      then, transition to MT_UM_NULL_STATE
    ---------------------------------------------------------------------------*/
    if ( dsumtsps_mt_timer_table_ptr[mt_index]->um_state ==
                                        DSUMTSPS_MT_UMSM_BRINGING_RM_UP_STATE )
    {
      inst.rm_dev_instance = dsumtsps_mt_timer_table_ptr[mt_index]->rmsm_inst;
      inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
      dsumts_rmsm_post_event( &inst,
                              UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV );
    }
    else
    {
      DS_3GPP_MSG2_ERROR("MT_UMSM_EV_HDLR: trap TIMEOUT_EV, \
                          invalid state [%d], mt_index [%d]!",
                          dsumtsps_mt_timer_table_ptr[mt_index]->um_state,
                          mt_index);
      ASSERT(0);
      return;
    }

    break;

  default:
    DS_3GPP_MSG1_ERROR("MT_PPP_EV_HDLR: Unknown event:%d",event);
    ASSERT(0);
    return;

  }/*switch(event)*/

  return;
}/*dsumtsps_mt_umsm_ev_hdlr()*/


#endif /*FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================

FUNCTION DSUMTSPS_MT_CVT_NAS_PDP_TYPE_TO_DS_FMT

DESCRIPTION
  Converts NAS PDP TYPE value to DS PDP TYPE.

PARAMETERS
  nas_pdp_type - This is NAS PDP TYPE.

DEPENDENCIES
  None.

RETURN VALUE
  ds_umts_pdp_type_enum_type - PDP type in DS format.

SIDE EFFECTS
  None.

===========================================================================*/
ds_umts_pdp_type_enum_type dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt
(
    uint8 nas_pdp_type
)
{
  /* VRK:TODO For Dual IP, Need to add V4V6 */
  ds_umts_pdp_type_enum_type ds_pdp_type = DS_UMTS_PDP_MAX;

  if(nas_pdp_type == IPV4_TYPE)
  {
    ds_pdp_type = DS_UMTS_PDP_IP;
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(nas_pdp_type == IPV6_TYPE)
  {
    ds_pdp_type = DS_UMTS_PDP_IPV6;
  }
#endif
  else if(nas_pdp_type == PPP_TYPE)
  {
    ds_pdp_type = DS_UMTS_PDP_PPP;
  }
#ifdef FEATURE_DUAL_IP_BEARER
  else if (nas_pdp_type == IPV4V6_TYPE)
  {
    ds_pdp_type = DS_UMTS_PDP_IPV4V6;
  }
#endif /* FEATURE_DUAL_IP_BEARER */
  return ds_pdp_type;
}/* dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt() */


/*===========================================================================

FUNCTION DSUMTSPS_MT_CVT_PDP_TYPE_TO_NAS_FMT

DESCRIPTION
  Converts DS-specific PDP TYPE value to NAS PDP TYPE.

PARAMETERS
  local_pdp_type - This is the pdp type in ds_umts_pdp_type_enum_type fmt.

DEPENDENCIES
  None.

RETURN VALUE
  uint8 - PDP type in NAS format.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dsumtsps_mt_cvt_pdp_type_to_nas_fmt
(
    ds_umts_pdp_type_enum_type local_pdp_type
)
{
  uint8 nas_pdp_type = 0x00;

  if(local_pdp_type == DS_UMTS_PDP_IP)
  {
    nas_pdp_type = IPV4_TYPE;
  }
  else if(local_pdp_type == DS_UMTS_PDP_IPV6)
  {
    nas_pdp_type = IPV6_TYPE;
  }
  else if(local_pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    nas_pdp_type = IPV4V6_TYPE;
  }
  else if(local_pdp_type == DS_UMTS_PDP_PPP)
  {
    nas_pdp_type = PPP_TYPE;
  }
  else 
  {
    DS_3GPP_MSG1_ERROR("Invalid PDP type, local_pdp_type = %d",
                        (int)local_pdp_type);
    ASSERT(0);
  }
  return nas_pdp_type;
}

/*===========================================================================

FUNCTION DSUMTSPS_MT_REGISTER_CALLBACK

DESCRIPTION
  Called from application context when an app registers for a MT request.

PARAMETERS
  * callback_info - This contains the callback name, profile number
                    and a pointer to the handle for the callback.The value
                    of the handle is assigned by this function and returned  
                    to the app. 
                    The app uses the handle to de-register the callback.

DEPENDENCIES
  None.

RETURN VALUE
  

SIDE EFFECTS


===========================================================================*/
int dsumtsps_mt_register_callback
(
  ps_iface_ioctl_mt_reg_cb_type *callback_info,
  ps_iface_type                 *iface_ptr,
  sint15                        *ps_errno
)
{
  acl_policy_info_type *acl_policy_info;
  int profile_num;                                 /* pdp profile number   */
  byte apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];     /* apn name             */
  uint8 pdp_type;                                  /* pdp type             */
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_context_type    pdp_context_data;    /* PDP context data     */
  uint32 i;
  if(callback_info == NULL)
  {
    DATA_ERR_FATAL("No Ioctl info available");
    *ps_errno = DS_EFAULT;
    return -1;
    
  }
  /*-------------------------------------------------------------------------
    Extract the profile number from the acl policy info that has been passed
    in the callback.
  -------------------------------------------------------------------------*/
  acl_policy_info = (acl_policy_info_type *)(callback_info->acl_pol_ptr);
  profile_num = (int)acl_policy_info->pdp_info;

  /*-------------------------------------------------------------------------
    Get the profile context info for the app from the profile number.
  -------------------------------------------------------------------------*/

  status = ds_umts_get_pdp_profile_context_info((uint16)profile_num,
                                                      &pdp_context_data);
  if( status != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d",status);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Valid context , retrieve the apn
  -------------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  strlcpy((char*)apn_name,(char*)pdp_context_data.apn,
              DS_UMTS_MAX_APN_STRING_LEN+1);
  apn_name[DS_UMTS_MAX_APN_STRING_LEN] = '\0';
  /*lint +e534*/

  if ( pdp_context_data.pdp_type == DS_UMTS_PDP_IPV4V6 )
  {
    if( PS_IFACE_IS_ADDR_FAMILY_V4(iface_ptr))
    {
      pdp_context_data.pdp_type = DS_UMTS_PDP_IPV4;
    }
    else
    {
      pdp_context_data.pdp_type = DS_UMTS_PDP_IPV6;
    }
  }
  /* Get PDP type in NAS format */
  pdp_type = dsumtsps_mt_cvt_pdp_type_to_nas_fmt(pdp_context_data.pdp_type);

  /*-------------------------------------------------------------------------
    If the PDP type is IP, store it as IPv4 type as this is the format for
    the NAS information.
  -------------------------------------------------------------------------*/

  if (pdp_type == PPP_TYPE)
  {
    DS_3GPP_MSG0_ERROR("PPP socket calls are not supported, reg MT failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    If PDP type is not IPv4 or IPv6, fail the registration of the callback 
    as only these types of socket calls are supported in UMTS.
  -------------------------------------------------------------------------*/
  else if (pdp_type == DS_UMTS_MT_PDP_TYPE_INVALID)
  {
    DS_3GPP_MSG1_ERROR("Unsupported type (%d) of socket call, reg MT failed", 
                        pdp_type);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Find the first empty slot in the array and store the 
    callback info like APN name, PDP type and iface pointer in the array.
  -------------------------------------------------------------------------*/
  for(i=0;i<DSUMTSPS_MT_MAX_CB; i++)
  {
    if(dsumtsps_mt_reg_table_ptr[i] == NULL)
    {
      /*-------------------------------------------------------------------------
        Alocate memory for entry and store the callback info like APN name,
        PDP type and iface pointer in the array.
      -------------------------------------------------------------------------*/
      dsumtsps_mt_reg_table_ptr[i] = 
      (dsumtsps_mt_reg_table_type *)ds_system_heap_mem_alloc(
                                           sizeof(dsumtsps_mt_reg_table_type));
      if(dsumtsps_mt_reg_table_ptr[i] == NULL)
      {
        DS_3GPP_MSG2_ERROR("Memory allocation failure for client:%d, size:%d",
                            MODEM_MEM_CLIENT_DATA,
                            sizeof(dsumtsps_mt_reg_table_type));
        *ps_errno = DS_ENOMEM;
        return -1;
      }
                                                 
      dsumtsps_mt_reg_table_ptr[i]->handle = dsumtsps_mt_handle_base_val+ i;
      callback_info->handle = & (dsumtsps_mt_reg_table_ptr[i]->handle);

      /*---------------------------------------------------------------------
        Store tha APN name in the encoded format so that it can be matched
        to the APN coming in the MT request.
      ---------------------------------------------------------------------*/
      (void) ds_pdn_cntxt_store_encoded_apn(dsumtsps_mt_reg_table_ptr[i]->apn_name,
                                                                  apn_name);
      dsumtsps_mt_reg_table_ptr[i]->pdp_type  = pdp_type;
      dsumtsps_mt_reg_table_ptr[i]->iface_ptr = iface_ptr;
      DS_3GPP_MSG2_HIGH("Reg MT cb, handle %d, pdp_type %d",
                         dsumtsps_mt_reg_table_ptr[i]->handle,
                         dsumtsps_mt_reg_table_ptr[i]->pdp_type);
      return 0;
    }
      
  } /* for */

  /*-------------------------------------------------------------------------
    If no empty slot is found, return an error.
  -------------------------------------------------------------------------*/
    DS_3GPP_MSG2_ERROR("Could not get an idx in MT table, base_val %d,i %d", 
                        dsumtsps_mt_handle_base_val, i);
    *ps_errno = DS_EFAULT;
    return -1;
} /* dsumtsps_mt_register_callback */

/*===========================================================================
FUNCTION DSUMTSPS_MT_DEREGISTER_CALLBACK

DESCRIPTION
  Called from application context when an app de-registers for a MT request.

PARAMETERS
  * callback_info - This contains the handle returned to the app when the 
                    callback was registered. The callback is removed from
                    list of stored callbacks for MT request.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS


===========================================================================*/
int dsumtsps_mt_deregister_callback 
(
  const ps_iface_ioctl_mt_dereg_cb_type *callback_info,
  sint15                                *ps_errno
)
{
  int index =0;
  if(callback_info == NULL)
  {
    DATA_ERR_FATAL("No Ioctl info for de-registering cb");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  /*-------------------------------------------------------------------------
    If the handle value is 0, then it is an invalid value, fail the 
    de-registering of the callback.
  -------------------------------------------------------------------------*/
  if(callback_info->handle == 0)
  {
    DS_3GPP_MSG1_ERROR("Invalid handle %d for de-registering MT callback", 
                        callback_info->handle);
    *ps_errno = DS_EFAULT;
    return -1;
  }
  /*-------------------------------------------------------------------------
    Get the index of the array from the handle. Since the initial value of
    handle base was 1, we need to subtract 1 to get the index to the table.
  -------------------------------------------------------------------------*/
  index = (callback_info->handle - 1) % DSUMTSPS_MT_MAX_CB; 

  /*-------------------------------------------------------------------------
    If the handle value matches that stored in the mt_reg_table, then
    clear the values stored in the reg table else report an error.
  -------------------------------------------------------------------------*/
  if( (dsumtsps_mt_reg_table_ptr[index] != NULL ) &&
      (dsumtsps_mt_reg_table_ptr[index]->handle == callback_info->handle) )
  {
    /*-----------------------------------------------------------------------
      Free the memory of reg table element for that callback.
    -----------------------------------------------------------------------*/
    ds_system_heap_mem_free(dsumtsps_mt_reg_table_ptr[index]);
    dsumtsps_mt_reg_table_ptr[index] = NULL;

    /*-----------------------------------------------------------------------
      Increase the MT handle base value, so that the apps never get the same
      handle value.
    -----------------------------------------------------------------------*/
    dsumtsps_mt_handle_base_val             += DSUMTSPS_MT_MAX_CB;
    DS_3GPP_MSG3_HIGH("De-reg MT cb, handle %d, base_val %d, idx %d", 
                  callback_info->handle, dsumtsps_mt_handle_base_val, index);
    return 0;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Invalid handle value %d", callback_info->handle); 
    *ps_errno = DS_EFAULT;
    return -1;
  }

  
} /* dsumtsps_mt_deregister_callback */


/*===========================================================================

FUNCTION DSUMTSPS_MT_HANDLE_TIMER_CB

DESCRIPTION
  This posts a command to DS task to send a MT reject response
  to dsmgr.

PARAMETERS
  cb_param - This contains the index to the mt table that the timer 
             running for that callback.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void dsumtsps_mt_handle_timer_cb
(
  uint32 cb_param
)
{
  DS_3GPP_MSG0_HIGH("dsumtsps_mt_handle_timer_cb reg not used. Return");

  return;
} /* dsumtsps_mt_handle_timer_cb */
 
#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================

FUNCTION dsumtsps_mt_ans_timeout_cb

DESCRIPTION
  Callback to handle expiry of TE MT ANSWER TIMER

PARAMETERS
  cb_param - callback parameter, in this case index into MT TIMER TABLE
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

COMMENTS
  This routine is internal to DSUMTSPS MT HDLR

===========================================================================*/
void dsumtsps_mt_ans_timeout_cb
(
  uint32 cb_param
)
{
  ds_cmd_type  *cmd_ptr = NULL;                  /* DS command ptr */
  uint8        *data_ptr = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    MT timer will be cleaned in MT_REJECT handler in DS task context.
    Post DSUMTSPS_SEND_MT_ANS_TIMEOUT_CMD to DS task
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(uint8));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_MT_TE_ANS_TIMEOUT;
  data_ptr = (uint8*)cmd_ptr->cmd_payload_ptr;

  *data_ptr = (uint8)cb_param;

  DS_3GPP_MSG2_HIGH("RM_ANS_TIMEOUT: MT timer expired,\
                     call_id [%d], mt_index [%d]",
                     dsumtsps_mt_timer_table_ptr[cb_param]->call_id,
                     cb_param);
  ds_put_cmd(cmd_ptr);
  
  return;
}/* dsumtsps_mt_ans_timeout_cb() */

#endif /* FEATURE_DATA_TE_MT_PDP */

/*===========================================================================
FUNCTION DSUMTSPS_MT_REG_TABLE_MATCH_PROFILE

DESCRIPTION
  Called from DS context when a MT request is received. This function
  matches the incoming profile info such as APN and PDP type to that
  stored in the callback table. If a match occurs, a callback is sent
  to the app and a TRUE is returned else a FALSE is returned and a 
  reject is sent.

PARAMETERS
  * cmd_ptr -    This contains the info such as apn name, pdp_type
    call_id -    This contains the call_id of the incoming call. This
                 is stored in the MT table along with the timer. 

DEPENDENCIES
  None.

RETURN VALUE
  True or False

SIDE EFFECTS


===========================================================================*/
boolean dsumtsps_mt_reg_table_match_emb_profile
(
  const ds_cmd_type *cmd_ptr,
  cm_call_id_type    call_id
)
{
  int                         i = 0;
  uint8                       index = 0;
  boolean                     app_registered = FALSE;
  void                       *handle;
  uint8                       nas_pdp_type;
  ds_umts_pdp_type_enum_type  ds_pdp_type = DS_UMTS_PDP_MAX;
  ds_umts_pdp_context_type    profile_context_info;
  ds3g_call_info_cmd_type    *call_info_ptr = NULL;

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_MED("Null ptr");
    return FALSE;
  }

  call_info_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  memset((char *)&profile_context_info, 0x0, sizeof(ds_umts_pdp_context_type));
  nas_pdp_type = call_info_ptr->mode_info.info.gw_ps_call.pdp_addr.pdp_type_num;
  ds_pdp_type = dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt(nas_pdp_type);

  /*-------------------------------------------------------------------------
    For all callbacks, match the incoming call info to that stored in the MT
    reg table.
  -------------------------------------------------------------------------*/
  switch(ds_pdp_type)
  {
    case DS_UMTS_PDP_IP:
#ifdef FEATURE_DATA_PS_IPV6
    case DS_UMTS_PDP_IPV6:
#endif /*FEATURE_DATA_PS_IPV6*/
  DS_3GPP_MSG0_MED("MTHDLR_MATCH_EMB_PROFILE: PDP-IP call!");

  for(i=0;i<DSUMTSPS_MT_MAX_CB; i++)
  {
    if( (dsumtsps_mt_reg_table_ptr[i] != NULL)     &&
        (dsumtsps_mt_reg_table_ptr[i]->handle != 0) && 
        (strncmp((char *)dsumtsps_mt_reg_table_ptr[i]->apn_name, 
        (char *)call_info_ptr->mode_info.info.gw_ps_call.apn_name.address,
        call_info_ptr->mode_info.info.gw_ps_call.apn_name.apn_addr_len) == 0) &&
       (dsumtsps_mt_reg_table_ptr[i]->pdp_type == nas_pdp_type))
    {

      DS_3GPP_MSG0_MED("MTHDLR_MATCH_EMB_PROFILE: APN and PDP type matched!");

      /*---------------------------------------------------------------------
        Start only one timer for a MT request, though multiple apps may need
        to be notified of the MT request.
      ---------------------------------------------------------------------*/
      if(app_registered == FALSE)
      {
        /*-------------------------------------------------------------------
          Store the call_id, apn, pdp type and offered address etc in the 
          mt table and start the timer for MT callback.
        -------------------------------------------------------------------*/
        for(index = 0; index < DSUMTSPS_MT_MAX_CB; index ++)
        {
           if( dsumtsps_mt_timer_table_ptr[index] == NULL )
           {
             dsumtsps_mt_timer_table_ptr[index] = 
             	(dsumtsps_mt_timer_table_type *)ds_system_heap_mem_alloc(
             						   sizeof(dsumtsps_mt_timer_table_type));
             if(dsumtsps_mt_timer_table_ptr[index] == NULL)
             {
               DS_3GPP_MSG2_ERROR("Memory allocation failure for client:%d, size:%d",
                         MODEM_MEM_CLIENT_DATA,
                                   sizeof(dsumtsps_mt_timer_table_type));
               return FALSE;
             }

             /*--------------------------------------------------------------
               App that has registered on that callback has been found.
             --------------------------------------------------------------*/
             app_registered = TRUE;
             dsumtsps_mt_timer_table_ptr[index]->call_id = call_id;
             dsumtsps_mt_timer_table_ptr[index]->address = 
                   call_info_ptr->mode_info.info.gw_ps_call.pdp_addr;
             dsumtsps_mt_timer_table_ptr[index]->pdp_type = nas_pdp_type;
             /*lint -e534 ignoring return code from function*/
             strlcpy(
                (char *)dsumtsps_mt_timer_table_ptr[index]->apn_name,
                (char *)call_info_ptr->mode_info.info.gw_ps_call.apn_name.address,
                DS_UMTS_MAX_APN_STRING_LEN+1);
             /*lint +e534*/

             dsumtsps_mt_timer_table_ptr[index]->apn_name
                                           [DS_UMTS_MAX_APN_STRING_LEN]='\0';

             /*--------------------------------------------------------------
               If it is a NULL APN, then set apn length to 1 since we would 
               need to compare the first byte of the apn string to determine
               a MT call origination.Else, set the apn_len value to that
               specified in the incoming request.
             --------------------------------------------------------------*/
             if(strlen((char *)dsumtsps_mt_timer_table_ptr[index]->apn_name) != 0)
             {
               dsumtsps_mt_timer_table_ptr[index]->apn_len = 
                  call_info_ptr->mode_info.info.gw_ps_call.apn_name.apn_addr_len;
             }
             else
             {
                dsumtsps_mt_timer_table_ptr[index]->apn_len = 1;
             }
             /*--------------------------------------------------------------
               Create a rex timer for the MT request and start the timer.
             --------------------------------------------------------------*/
             if((dsumtsps_mt_timer_table_ptr[index]->mt_timer_ptr =
                   rex_create_timer_ex(dsumtsps_mt_handle_timer_cb,
                     (uint32)(index))) != NULL )
             {
               DS_3GPP_MSG3_HIGH("MTHDLR_MATCH_PROFILE: Start tmr for MT, callid %d, mt_idx %d, idx %d",
                   call_id, i, index);
               (void) rex_set_timer(dsumtsps_mt_timer_table_ptr[index]->mt_timer_ptr,
                           DSUMTSPS_MT_TIMER_VAL);
             } 
             else
             {
               DS_3GPP_MSG0_ERROR("MTHDLR_MATCH_PROFILE: Tmr ptr for rex timer is NULL"); 
               ds_system_heap_mem_free(dsumtsps_mt_timer_table_ptr[index]);
               return FALSE;
             }
             break;
           } /* If MT timer table index is found */

        } /* for */

        /*-------------------------------------------------------------------
          If there is no entry available store the mt, then reject the MT 
          call.
        -------------------------------------------------------------------*/
        if(index == DSUMTSPS_MT_MAX_CB)
        {
          DS_3GPP_MSG0_ERROR("MTHDLR_MATCH_PROFILE: No index available for MT timer");
          return app_registered;
        }
      } /* app_registered == FALSE */

      /*---------------------------------------------------------------------
        Send the callback to the app to initiate a call.
      ---------------------------------------------------------------------*/
      if(app_registered == TRUE)
      {
        handle = (void *)dsumtsps_mt_reg_table_ptr[i]->handle;
        ps_iface_generic_ind(dsumtsps_mt_reg_table_ptr[i]->iface_ptr,
             IFACE_MT_REQUEST_EV, &handle);
         
      }

    }/* If the incoming call parameters match those stored in MT reg table */

  } /*for */
  break;

  default:
    DS_3GPP_MSG1_ERROR("MTHDLR_MATCH_EMB_PROFILE: Invalid PDP Type! ds_pdp_type:%d",
                        ds_pdp_type);
    return FALSE;
  
  }/* switch(ds_pdp_type) */

  return app_registered;

} /* dsumtsps_mt_reg_table_match_emb_profile() */


#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================
FUNCTION dsumtsps_mt_um_table_store_info

DESCRIPTION
  This is used to post event to MT-UMSM 

PARAMETERS
  mt_index: index of MT TIMER TBL
 *cmd_ptr: CM provided CMD ptr
  cm_call_id: CM call id

 
DEPENDENCIES
  None.

RETURN VALUE
  none

RESTRICTION
  Must be executed in DS Task context

SIDE EFFECTS

===========================================================================*/
boolean dsumtsps_mt_um_table_store_info
(
  const ds_cmd_type *cmd_ptr,
  cm_call_id_type    call_id,
  uint8 *mt_index 
)
{
  uint8 index = 0;
  uint8 nas_pdp_type;
  ds_umts_pdp_type_enum_type ds_pdp_type = DS_UMTS_MT_PDP_TYPE_INVALID;
  ds_umts_pdp_context_type profile_context_info;
  const apn_T *apn;
  uint8 apn_len = 0;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR( MSG_LEGACY_ERROR, "NULL pointer passed" );
    return FALSE;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  memset((char *)&profile_context_info, 0x0, sizeof(ds_umts_pdp_context_type));
  nas_pdp_type = call_info_cmd_ptr->mode_info.info.gw_ps_call.pdp_addr.pdp_type_num;
  ds_pdp_type = dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt(nas_pdp_type);

  /*-------------------------------------------------------------------------
    For all callbacks, match the incoming call info to that stored in the MT
    reg table.
  -------------------------------------------------------------------------*/
  switch(ds_pdp_type)
  {
  case DS_UMTS_PDP_IP:
  case DS_UMTS_PDP_IPV4V6:
  case DS_UMTS_PDP_IPV6:
  case DS_UMTS_PDP_PPP:

    DS_3GPP_MSG1_MED("MT_REG_TBL_STORE_INFO: pdp_type [%d], PDP activ req, store in MT tbl",
                      ds_pdp_type);

  /*-------------------------------------------------------------------
    Store the call_id, apn, pdp type and offered address etc in the 
    mt timer table and start the timer for MT callback.
  -------------------------------------------------------------------*/
  for(index = 0; index < DSUMTSPS_MT_MAX_CB; index ++)
  {
    if(dsumtsps_mt_timer_table_ptr[index] == NULL)
    {
      dsumtsps_mt_timer_table_ptr[index] = 
         (dsumtsps_mt_timer_table_type *)ds_system_heap_mem_alloc(
                    sizeof(dsumtsps_mt_timer_table_type));
      if(dsumtsps_mt_timer_table_ptr[index] == NULL)
      {
        DS_3GPP_MSG3_ERROR("Memory allocation failure for client:%d, size:%d",
            MODEM_MEM_CLIENT_DATA, sizeof(dsumtsps_mt_timer_table_type),
              0);
        return FALSE;
      }
      memset( dsumtsps_mt_timer_table_ptr[index], 0x0,
              sizeof(dsumtsps_mt_timer_table_type) );
      *mt_index = index;
      /*--------------------------------------------------------------
        TE has registered for MT PDP PPP call, populate table
      --------------------------------------------------------------*/
      dsumtsps_mt_timer_table_ptr[index]->mt_index = index;
      dsumtsps_mt_timer_table_ptr[index]->call_id = call_id;
      memscpy(&dsumtsps_mt_timer_table_ptr[index]->address,
	      sizeof (pdp_address_T)),
              &call_info_cmd_ptr->mode_info.info.gw_ps_call.pdp_addr,
              sizeof (pdp_address_T));
      dsumtsps_mt_timer_table_ptr[index]->pdp_type = nas_pdp_type;
      /*lint -e534 ignoring return code from function*/
      apn = &call_info_cmd_ptr->mode_info.info.gw_ps_call.apn_name;
      if ( apn->valid )
      {
        apn_len = (apn->apn_addr_len > DS_UMTS_MAX_APN_STRING_LEN)?
                    DS_UMTS_MAX_APN_STRING_LEN:apn->apn_addr_len ;
        strlcpy((char *)dsumtsps_mt_timer_table_ptr[index]->apn_name, 
           (char *)&apn->address, apn_len+1);
      }
      else
      {
        DS_3GPP_MSG0_HIGH("apn->valid flag not set, apn_name not\
                           filled in table");
      }
      /*lint +e534*/
      dsumtsps_mt_timer_table_ptr[index]->apn_name[apn_len + 1]='\0';
      dsumtsps_mt_timer_table_ptr[index]->apn_len = apn_len;
      /* Initialize MT-UMSM to NULL state */
      dsumtsps_mt_timer_table_ptr[index]->um_state =
                                          DSUMTSPS_MT_UMSM_NULL_STATE;
      DS_3GPP_MSG3_HIGH("Stored in table mt_index %d call_id %d\
                         pdp_type %d", index, call_id, nas_pdp_type);
       /*--------------------------------------------------------------
         Create a rex timer for the MT request and start the timer.
       --------------------------------------------------------------*/
       if((dsumtsps_mt_timer_table_ptr[index]->mt_timer_ptr =
             rex_create_timer_ex(dsumtsps_mt_ans_timeout_cb,
               (uint32)(index))) != NULL )
       {
         DS_3GPP_MSG2_HIGH("MT_REG_TBL_STORE_INFO: Start tmr for MT, cm_call_id [%d], mt_index [%d]",
                            call_id, index);
         (void) rex_set_timer(dsumtsps_mt_timer_table_ptr[index]->mt_timer_ptr,
                               DSUMTSPS_MT_TE_TIMER_VAL );
       } 
       else
       {
         DS_3GPP_MSG0_ERROR("MT_REG_TBL_STORE_INFO: Tmr ptr for rex timer is NULL"); 
         ds_system_heap_mem_free(dsumtsps_mt_timer_table_ptr[index]);
         dsumtsps_mt_timer_table_ptr[index] = NULL;
         return FALSE;
       }
       break;
     } /* If a free MT timer table index is found */

  } /* for(index) */

  /*-------------------------------------------------------------------
    If there is no entry available to store the mt, then reject the MT 
    call.
  -------------------------------------------------------------------*/
  if(index == DSUMTSPS_MT_MAX_CB)
  {
    DS_3GPP_MSG0_ERROR("MT_REG_TBL_STORE_INFO: No index available for MT timer");
    return FALSE;
  }

  return TRUE;

  break;/* case: PPP or IP or IPv6*/

  default:
    DS_3GPP_MSG1_ERROR("MT_REG_TBL_STORE_INFO: Invalid PDP Type! ds_pdp_type [%d]",
                        ds_pdp_type);
    return FALSE;

  }
}/* dsumtsps_mt_reg_table_store_info() */


boolean dsumtsps_mt_um_get_info
(
  uint8 mt_index,
  dsumts_rmsm_mt_info_type *mt_info_ptr
)
{

  if ( (mt_info_ptr == NULL) || !DSUMTSPS_MT_INDEX_IS_VALID( mt_index ) ) 
  {
    DS_3GPP_MSG1_ERROR("MT_GET_MT_INFO: invalid input params, mt_index [%d]",
                        mt_index );
    return FALSE;
  }

  /* Filling up data structure for RMSM */
  mt_info_ptr->pdp_type  = dsumtsps_mt_timer_table_ptr[mt_index]->pdp_type;
  mt_info_ptr->address   = dsumtsps_mt_timer_table_ptr[mt_index]->address;
  mt_info_ptr->apn_len   = dsumtsps_mt_timer_table_ptr[mt_index]->apn_len;
  mt_info_ptr->apn_name  = dsumtsps_mt_timer_table_ptr[mt_index]->apn_name;
  mt_info_ptr->rmsm_inst = dsumtsps_mt_timer_table_ptr[mt_index]->rmsm_inst;
  mt_info_ptr->mt_index  = dsumtsps_mt_timer_table_ptr[mt_index]->mt_index;
  DS_3GPP_MSG3_HIGH("MT_GET_MT_INFO: pdp_type %d apn_len %d rmsm_inst %d",
                     mt_info_ptr->pdp_type, mt_info_ptr->apn_len,
                     mt_info_ptr->rmsm_inst);
  return TRUE;
}/*dsumtsps_mt_get_mt_info()*/


/*===========================================================================
FUNCTION DSUMTSPS_MT_UMSM_POST_EV

DESCRIPTION
  This is used to post to event to MT-UMSM 

PARAMETERS
  mt_index: index of MT TIMER TBL
  event: event to be posted
 *event_data: event data
 
DEPENDENCIES
  None.

RETURN VALUE
  none

RESTRICTION
  Must be executed in DS Task context

SIDE EFFECTS

===========================================================================*/
void dsumtsps_mt_umsm_post_ev
(
  uint8 mt_index,
  dsumts_mt_umsm_ev_type event,
  void *event_data
)
{

  /* The context is DS Task, handle event */
  dsumtsps_mt_umsm_ev_hdlr( mt_index,
                           event,
                           event_data );
}/* dsumtsps_mt_umsm_post_ev() */


void dsumtsps_mt_umsm_init_state
(
  void
)
{
  uint32 index = 0;

  for(index = 0; index < DSUMTSPS_MT_MAX_CB; index ++)
  {
    dsumtsps_mt_umsm_clean_up_state(index);
  } /* for(index) */
   
}

#endif /*FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================
FUNCTION DSUMTSPS_MT_CHECK_CALL_ORIG

DESCRIPTION
 This function checks whether a call origination is due to a MT request that
 was received. It matches the APN name and PDP type of the originating call
 with that of any MT request that was received and fills in the call id
 if a match occurs.

PARAMETERS
    call_id -    This contains the call_id of the incoming call that
                 was stored in the MT table.

DEPENDENCIES
  None.

RETURN VALUE

  Call ID of the MT request.

SIDE EFFECTS


===========================================================================*/
cm_call_id_type dsumtsps_mt_check_call_orig
(
  ds_pdn_context_s    *pdn_cntx_p
)
{
  int j = 0;
  ps_iface_type *          iface_ptr ;
  uint32                    pdp_profile_num;
  byte                     apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
  byte                     encoded_apn[DS_UMTS_MAX_APN_STRING_LEN+1];
  uint8                    encoded_apn_len = 0;
  uint8                    pdp_type;
  ds_umts_pdp_profile_status_etype status;
  ds_umts_pdp_context_type    pdp_context_data; /* PDP context data        */
  cm_call_id_type             call_id = 0xFF;   /* call id to be returned  */
#ifdef FEATURE_DATA_TE_MT_PDP
  dsumts_rmsm_instance_type   inst;
  ds_umts_pdn_context_s *umts_pdn_cntxt_p=NULL;
#endif /* FEATURE_DATA_TE_MT_PDP */

  /*-------------------------------------------------------------------------
    Get the pdp profile number, APN info and PDP type for the originating
    call.   
  -------------------------------------------------------------------------*/
  if ((pdn_cntx_p == NULL) || (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p)))
  {
    return call_id;
  }

#ifdef FEATURE_DATA_TE_MT_PDP
  umts_pdn_cntxt_p= ( (ds_umts_pdn_context_s*)(pdn_cntx_p->ds_pdn_context_dyn_p->
           vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ]) );

   if(!dsUmtsPdnCntx_validate_pdn_context(umts_pdn_cntxt_p))
  {
     DS_3GPP_MSG0_ERROR("Umts PDN cntxt passed is not valid");
     return call_id;
   }
#endif /* FEATURE_DATA_TE_MT_PDP */

  pdp_profile_num = pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num;
  iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
  if (iface_ptr == NULL )
  { 
    /* get V6 iface pointer */
    iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);
    if (iface_ptr == NULL )
    {
      DS_3GPP_MSG0_ERROR("No valid iface pointer in mt_check_call_orig");
      return call_id;
    }
  }
  /*-------------------------------------------------------------------------
    Get the profile context info for the app from the profile number.
  -------------------------------------------------------------------------*/

  status = ds_umts_get_pdp_profile_context_info((uint16)pdp_profile_num,
                                                      &pdp_context_data);
  if( status != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d",status);
    return call_id; 
  }
  /*-------------------------------------------------------------------------
    Valid context , retrieve the apn
  -------------------------------------------------------------------------*/
  /*lint -e534 ignoring return code from function*/
  strlcpy((char*)apn_name,(char*)pdp_context_data.apn,DS_UMTS_MAX_APN_STRING_LEN+1);
  apn_name[DS_UMTS_MAX_APN_STRING_LEN] = '\0';
  /*lint +e534*/

  /*-------------------------------------------------------------------------
    Store the encoded value of APN in apn_name.
  -------------------------------------------------------------------------*/
  encoded_apn_len = ds_pdn_cntxt_store_encoded_apn(encoded_apn, apn_name);
  if(encoded_apn_len > DS_UMTS_MAX_APN_STRING_LEN)
  {
     DS_3GPP_MSG1_ERROR("encoded_app_len is %d that greater than DS_UMTS_MAX_APN_STRING_LEN", 
                      encoded_apn_len);
     return call_id;
  }
  encoded_apn[encoded_apn_len] = '\0';

  /*-------------------------------------------------------------------------
    Get the PDP type ( IP or PPP) for that profile.
  -------------------------------------------------------------------------*/
  pdp_type = dsumtsps_mt_cvt_pdp_type_to_nas_fmt(pdp_context_data.pdp_type);

  if(pdp_type == DS_UMTS_MT_PDP_TYPE_INVALID)
  {
    DS_3GPP_MSG2_ERROR(" Unknown PDP type in profile, profile %d, PDP type %d", 
                         pdp_profile_num, (uint8)pdp_context_data.pdp_type);
    return call_id;
  }

  /*-------------------------------------------------------------------------
    If APN name and PDP type matches that for incoming request, 
    stop the MT timer and delete the timer. Copy the offered address info 
    the UMTS call table and return the call_id for the MT request.
  -------------------------------------------------------------------------*/

  for(j=0;j<DSUMTSPS_MT_MAX_CB; j++)
  {
    if(dsumtsps_mt_timer_table_ptr[j] != NULL)
    {
      /*-----------------------------------------------------------------------
       If incoming pdp_type and apn_name is same as profile pass it,
       else if profile has IPV4V6 as pdp_type and incoming pdp_type
       is other than PPP then also pass if apn_name matches with profile.
       Finally if incoming request itself is IPV4V6 then pass if profile's
       pdp_type is not PPP ofcourse apn_name should match in all cases.
      -----------------------------------------------------------------------*/
      DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"dsumtsps_mt_check_call_orig"
                            "apn to match %s APN in table: %s table index %d,",
                            (char *) encoded_apn,
                            (char *)dsumtsps_mt_timer_table_ptr[j]->apn_name, j);

      if(((dsumtsps_mt_timer_table_ptr[j]->pdp_type == pdp_type)||
          ((pdp_type == IPV4V6_TYPE)&&
           ((dsumtsps_mt_timer_table_ptr[j]->pdp_type == IPV4_TYPE)||
           (dsumtsps_mt_timer_table_ptr[j]->pdp_type == IPV6_TYPE)))||
          ((dsumtsps_mt_timer_table_ptr[j]->pdp_type == IPV4V6_TYPE)&&
           ((pdp_type == IPV4_TYPE)||(pdp_type == IPV6_TYPE))))&&
        ((strcasecmp((char *)dsumtsps_mt_timer_table_ptr[j]->apn_name, 
                     (char *)encoded_apn))== 0 ||
         (( 0 == strlen( (char *)apn_name ) ))))
      {
        DS_3GPP_MSG1_HIGH("MT call orig, delete rex timer, call_id %d",
                           dsumtsps_mt_timer_table_ptr[j]->call_id);
#ifdef FEATURE_DATA_TE_MT_PDP
        call_id = dsumtsps_mt_timer_table_ptr[j]->call_id;
        umts_pdn_cntxt_p->ds_umts_pdn_context_dyn_p->offered_mt_address =
                       dsumtsps_mt_timer_table_ptr[j]->address;
        /* No need of this table entry.now, we can clean this */
        inst.rm_dev_instance = dsumtsps_mt_timer_table_ptr[j]->rmsm_inst;
        inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_LOCKED_EV );
        dsumtsps_mt_umsm_clean_up_state(j);
#endif /*FEATURE_DATA_TE_MT_PDP*/
      }
    } /* if MT timer table is in use */

  }/* for */
  DS_3GPP_MSG1_HIGH("Mt_check_call_orig, returning call_id %d", call_id);
  return call_id;

} /* dsumtsps_mt_check_call_orig */


/*===========================================================================
FUNCTION DSUMTSPS_MT_SEND_REJECT

DESCRIPTION
  This function sends a reject for a MT call when the MT timer expires but
  the app has not originated a call.
  
PARAMETERS
  index -    This is the index to the MT table that stores the timer and 
             other info like APN, PDP type, call_id etc.
                 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void dsumtsps_mt_send_reject 
(
  uint8 index
)
{
  cm_call_id_type                call_id = CM_CALL_ID_INVALID;  /* CM call id */
  ds_3gpp_pdn_call_info_type     mode_sp_info;       /* Mode specific info    */

  if ( !DSUMTSPS_MT_INDEX_IS_VALID(index) )
  {
    DS_3GPP_MSG1_ERROR("dsumtsps_mt_send_reject: invalid index, mt_index [%d]",
                        index);
    return;
  }
  call_id = dsumtsps_mt_timer_table_ptr[index]->call_id;

  DS_3GPP_MSG2_MED("MT_SEND_REJECT: mt_index [%d], call id %d HANGUP_CALL()!", 
                    index, call_id);
  mode_sp_info.mt_flag = TRUE;
  /*-----------------------------------------------------------------------
    Not freeing timer in dsumtsps_mt_send_reject will be freed when
    moved to null state.
  -----------------------------------------------------------------------*/
  if (ds3g_msh_hangup_call_ex(call_id, &mode_sp_info, ds3g_get_ps_subs_id()) 
                                                             == DS3G_FAILURE)
  {
    /*-----------------------------------------------------------------------
      The call has already ended.  Do nothing here, as the call_ended handler
      has been/will be called.  That will do most of the work. Since 
      application did not originate a call, do not set call_state of dsumtsps_
      call_tbl as call_instance for call_id is invalid.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG1_HIGH("MT_SEND_REJECT: Call already ended call_id :%d", 
                       call_id);
  }

} /* dsumtsps_mt_send_reject */

#endif /* defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS) */
