/*===========================================================================

                         D S _ R M N E T _ S M _ E X T  . C

DESCRIPTION

 The Data Services RMNET SM configuration Source file. 
 
EXTERNALIZED FUNCTIONS

   rmnet_sm_config_net_params()
     Configure the network params before bring up.


Copyright (c) 2006-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_sm_ext.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/26/11    ua     In RUIM only mode, SIM and MIP NAI's should not be 
                   read from NV.
03/11/11    kk     Removing logic to clear up auth params when
                   FEATURE_DATA_PS_INTERNAL_AUTH is turned ON.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
10/01/07    mct    Added support for IPv6 RmNet.
07/18/07    ks     Use the regular UM NAI if the Tethered NAI is not present.
11/29/06    jd     Added DCTM support to RmNet
11/13/06    ks     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DATA_RM_NET
#include "amssassert.h"

#ifdef FEATURE_DS_MOBILE_IP            
#ifdef FEATURE_DS_MULTIPLE_NAI
#include "dsmip.h"
#include "dsmip_api.h"
#endif /* FEATURE_DS_MULTIPLE_NAI */
#endif /* FEATURE_DS_MOBILE_IP */

#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "nv.h"
#include "ds_qmi_svc.h"
#include "ds_rmnet_sm_ext.h"
#ifdef FEATURE_CALL_THROTTLE
#include "ds707_extif.h"
#endif /* FEATURE_CALL_THROTTLE */

#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

#include "nvruimi.h"


/*===========================================================================

                                 DEFINITIONS

===========================================================================*/


/*===========================================================================

                               FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION RMNET_SM_EXT_CALL_FAILURE()

DESCRIPTION
  Callback executed when call failure occurs during RmNet call setup

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rmnet_sm_ext_call_failure
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_CALL_THROTTLE
#ifdef FEATURE_DATA_IS707
  LOG_MSG_INFO2_0("Report call setup failure");
  /* report call setup failure to CM via DCTM
     This will need to change to RMNET_DHCP_FAILURE if dctm does something
     different based on failure code in the future. */
  dctm_post_sip_fail();
#endif  /* FEATURE_DATA_IS707 */
#endif /* FEATURE_CALL_THROTTLE */

} /* rmnet_sm_ext_call_failure() */


#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MULTIPLE_NAI

/*===========================================================================
FUNCTION RMNET_SM_GET_MIP_SESSION_INFO_CB()

DESCRIPTION
  Function registered with MIP to be called at the end of MIP config session
  to update NAI information for rment calls. 

PARAMETERS
  session: pointer to the mip session control block where info is stored
  profile: profile number associated with the mip session.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_sm_get_mip_session_info_cb
(
  byte                     active_mip_profile,
  mip_session_info_type *  session
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mip_reg_ses_info_cb( NULL );
  if(mip_get_rm_nai(session,active_mip_profile)!=active_mip_profile)
  {
    LOG_MSG_ERROR_0(" Error reading tethered NAIs from NV");
    return;
  }
} /* rmnet_sm_get_mip_session_info_cb() */

#endif /* FEATURE_DS_MULTIPLE_NAI */
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
  FUNCTION RMNET_SM_CONFIG_NET_PARAMS()

  DESCRIPTION
    Configure the network params before bring up.
    Today used only to correctly configure auth credentials.

  PARAMETERS
    net_params - ptr to network params structure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_sm_config_net_params
(
  network_params_info_type *  net_params,
  rmnet_meta_sm_call_e_type   call_type
)
{
#ifdef FEATURE_DS_MULTIPLE_NAI
  nv_stat_enum_type nv_status;
  nv_item_type	   *dcc_nv_item_ptr;
  byte              nai_len;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( RMNET_CALL_TYPE_LAPTOP == call_type )
  {
#ifdef FEATURE_DS_MULTIPLE_NAI
    /*-------------------------------------------------------------------------
      If MULTIPLE_NAI feature is defined, use the respective RM_NAI nv items
      for MIP and SIP calls
      - Register mip_cb fn which is called if MIP call comes 
        up, reads the MIP_RM_NAI item and uses it.
      - Store the SIP_RM_NAI in net_params, so that it is used by 
        iface4_hdlr if sip call comes up 
    -------------------------------------------------------------------------*/
         
#ifdef FEATURE_DS_MOBILE_IP
     /* To read MIP from NV, UIM_3GPD_SUPPORT should return MIP to be NV and SIP 
        either NV or RUIM its dont care */
     if(uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_RUIM || 
        uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)
     {
       mip_reg_ses_info_cb( rmnet_sm_get_mip_session_info_cb );
     }

#endif /* FEATURE_DS_MOBILE_IP */

   /* To read SIP from NV, UIM_3GPD_SUPPORT should return SIP to be NV and MIP
       could be either NV or RUIM its dont care */
   if(uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV || 
       uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)
    {
      /*-------------------------------------------------------------------------
        Allocate temporary memory for the NV item
      -------------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(dcc_nv_item_ptr, sizeof(nv_item_type),
                                      nv_item_type*);
      if( dcc_nv_item_ptr == NULL )
      {
        return;
      }

      nv_status = dcc_get_nv_item(NV_DS_SIP_RM_NAI_I, dcc_nv_item_ptr);
    
      if( (nv_status != NV_DONE_S) || 
          (dcc_nv_item_ptr->ds_sip_rm_nai.nai_length == 0))
      {
        /*-----------------------------------------------------------------------
          If the NV item is not written, fall back to using the regular NAI
          i.e. Don't set username in net_params and let dssnet get it from NV
        -----------------------------------------------------------------------*/  
        PS_SYSTEM_HEAP_MEM_FREE(dcc_nv_item_ptr);
        return;
      }
    
      net_params->valid_fields |= NET_CFG_PRM_AUTH_USERNAME_MASK;
    
      /*-----------------------------------------------------------------------
        Only copy NAI if its length is valid.  If NAI length is > max allowed
        assume a memory/flash scribble and set length to 0.
      -----------------------------------------------------------------------*/
      nai_len = dcc_nv_item_ptr->ds_sip_rm_nai.nai_length;
      if( nai_len <= NV_MAX_NAI_LENGTH && nai_len > 0 )
      {
        net_params->auth_info.params.user_id_len = nai_len;
        memscpy(net_params->auth_info.params.user_id_info,
                nai_len,
               (char*) dcc_nv_item_ptr->ds_sip_rm_nai.nai,
               nai_len);
      }
      else /*if NV NAI length is incorrect, set it to zero */
      {
        net_params->auth_info.params.user_id_len = 0;
      }
      PS_SYSTEM_HEAP_MEM_FREE(dcc_nv_item_ptr);
    }
#endif /* FEATURE_DS_MULTIPLE_NAI */
  }
  
} /* rmnet_sm_config_net_params() */

#endif /* FEATURE_DATA_RM_NET */
