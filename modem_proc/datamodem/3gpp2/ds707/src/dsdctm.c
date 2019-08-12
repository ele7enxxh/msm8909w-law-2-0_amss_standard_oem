
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S D C T M

GENERAL DESCRIPTION
  This module contains the coding for DS Data Call Throttle Manager module.
  This module interfaces with CM to report call success/failure.  
  events are posted to this module from ds707_iface4_hdlr.

EXTERNALIZED FUNCTIONS
  These functions are defined in DSDCTM.H 

  dctm_post_call_status_event()
    Posts MIP, SIP call status received from ds707_iface4_hdlr to CM using the
    API provided by CM for this purpose.


  FEATURE_CAL_THROTTLE - Supports Call-throttle based on previous call-fail
                         reasons.

 Copyright (c) 2005-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/dsdctm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
08/09/11    sha    Global variable clean up
09/16/10    ls     Add check to ensure calling DCTM CM API only when in HDR/CDMA
08/19/10    mg     Changed ehrpd auth failure to soft failure.
05/10/07    ac     Lint fix
07/21/06    as     Featured under CALL_THROTTLE and moves dctm_post_sip_fail
                   outside the DS_MOBILE_IP feature
06/28/06    squ    added function dctm_post_sip_fail
07/29/05    kvd    Featurized MIP specific code.
05/02/05    kvd    Created Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_CALL_THROTTLE

#include "data_msg.h"
#include "cm.h"
#include "dsdctm.h"
#include "ds3gmgrint.h"    /* needed to get ds3gi_cm_client_id in cmd toCM */
#include "ds707_pkt_mgr.h" /* needed to get current data session network   */

#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#include "ds707_s101_dctm.h"
#endif

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
                           Pre-processor Constants
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           Enum Typedefs
---------------------------------------------------------------------------*/

/*===========================================================================
                        Variable Definitions
===========================================================================*/


/*---------------------------------------------------------------------------
  Function prototypes to allow the desired ordering of functions within this
  module - forward declarations.
---------------------------------------------------------------------------*/

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

void dctm_post_ppp_call_status_in_direct_mode
(
  ppp_event_e_type       ppp_event,                /* Down / Up            */
  dsdctm_ppp_protocol_e_type protocol,                 /* LCP, IPCP, Auth    */
  ppp_fail_reason_e_type ppp_fail_reason           /* timeout, auth_fail   */
)
{
  cm_ss_orig_thr_reasons_e_type call_status =
                           CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS;
  sys_sys_mode_e_type current_data_sess_network = SYS_SYS_MODE_NO_SRV;
  cm_client_id_type   ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, 
                  "DCTM:Posting PPP status event: %d protocol: %d reason: %d "
                  "in native DO mode",
                  ppp_event, protocol, ppp_fail_reason);

  current_data_sess_network = ds707_pkt_get_current_data_sess_network();

  if ((current_data_sess_network != SYS_SYS_MODE_CDMA) &&
      (current_data_sess_network != SYS_SYS_MODE_HDR))
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "DCTM:Ignore the DCTM PPP report. CDSN is %d",
                     current_data_sess_network );
    return;
  }

  switch(ppp_event)
  {
    /*-----------------------------------------------------------------------
       Post PPP SUCCESS event if call is successful. This will let CM clear 
       the throttle flag if the call failed on the current SID-NID_PZID 
       earlier.
    -----------------------------------------------------------------------*/
    case PPP_UP_EV:
    {
      call_status = CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS;

#ifdef FEATURE_DATA_OPTHO
      /*-----------------------------------------------------------------------
        Clear DCTM over S101 throttling since the call succeeded in direct
        mode
      -----------------------------------------------------------------------*/
      ds707_s101_dctm_reset_throttle();
#endif
      break;
    }

    /*-----------------------------------------------------------------------
       This event should only be posted  if PPP went down while the call is
       coming UP nto after the call is established.
    -----------------------------------------------------------------------*/

    case PPP_DOWN_EV:
    {
      switch(protocol)
      {
        case DSDCTM_PPP_PROTOCOL_LCP:
        case DSDCTM_PPP_PROTOCOL_IPCP:
        case DSDCTM_PPP_PROTOCOL_IPV6CP:
        case DSDCTM_PPP_PROTOCOL_VSNCP:
        {
          DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                          "DCTM: PPP failure posted with fail_reason %d",
                          ppp_fail_reason);
          switch(ppp_fail_reason)
          {
            case PPP_FAIL_REASON_TIMEOUT:
              if(protocol == DSDCTM_PPP_PROTOCOL_LCP)
              {
                call_status = CM_SS_ORIG_THR_DS_PPP_LCP_TIMEOUT;
              }
              else
              {
                call_status = CM_SS_ORIG_THR_DS_PPP_NCP_TIMEOUT;
              }
              break;
            case PPP_FAIL_REASON_NONE:
              call_status = CM_SS_ORIG_THR_DS_PPP_FAILURE;
              break;

            case PPP_FAIL_REASON_OPTION_MISMATCH:
              call_status = CM_SS_ORIG_THR_DS_PPP_FAILURE;
              break;

            case PPP_FAIL_REASON_VSNCP_TIMEOUT:
            case PPP_FAIL_REASON_VSNCP_FAILURE:
              call_status = CM_SS_ORIG_THR_REASON_NONE;
              break;

            default:
              call_status = CM_SS_ORIG_THR_REASON_NONE;
              break;
          }
        }
        break;

        case DSDCTM_PPP_PROTOCOL_AUTH:
        case DSDCTM_PPP_PROTOCOL_EHRPD_AUTH:
          call_status = CM_SS_ORIG_THR_DS_PPP_AUTH_FAILURE;
        break;
        
        default:
          call_status = CM_SS_ORIG_THR_REASON_NONE;
          break;
      }
      break;
    }
    default:
      call_status = CM_SS_ORIG_THR_REASON_NONE;
      break;
  }

  if ( call_status != CM_SS_ORIG_THR_REASON_NONE )
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();
    DATA_IS707_MSG1( MSG_LEGACY_MED, "DCTM: posting ppp call status to CM: %d",
                     call_status );
    if( cm_ss_cmd_orig_thr_update_tbl(
                   NULL,
                   NULL,
                   ds3g_cm_client_id,
                   CM_SS_ORIG_THR_UPDATE_CURR_SS,
                   call_status,
                   CM_CALL_TYPE_PS_DATA,
                   current_data_sess_network,
                   NULL,
                   (sys_band_class_e_type)0,
                   0) == FALSE)
    {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "DCTM:Err posting ppp_call_fail to CM");
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DCTM: CM_SS_ORIG_THR_REASON_NONE, "
                    "Err posting ppp_call_fail to CM");
  }
}
/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
#ifdef FEATURE_DATA_OPTHO
/*===========================================================================

FUNCTION DCTM_CLEAR_PPP_THROTTLE

DESCRIPTION
  This function  posts call success to CM for a particular subnet id.
  Currently this funtion is being used only from s101 pre-registration
  module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_clear_ppp_throttle
(
  hdraddr_type subnet
)
{
  
  cm_client_id_type   ds3g_cm_client_id;
  sys_sys_id_s_type   sys_id;
  uint8               i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  if ( subnet == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                   "DCTM: clear ppp throttle, invalid subnet id passed");
    return;
  }

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "DCTM: posting ppp call success status to CM "
                   "ds3g client id: %d with subnet - ", ds3g_cm_client_id);

  sys_id.id_type = SYS_SYS_ID_TYPE_IS856;
  (void) memscpy(&sys_id.id, sizeof(sys_id.id), 
                 subnet, HDRADDR_LENGTH_IN_BYTES);

  for ( i = 0; i < HDRADDR_LENGTH_IN_BYTES; i++ )
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "subnet: 0x%x", (byte)sys_id.id.is856[i]);
  }
  if( cm_ss_cmd_orig_thr_update_tbl(
                 NULL,
                 NULL,
                 ds3g_cm_client_id,
                 CM_SS_ORIG_THR_UPDATE_SS,
                 CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS,
                 CM_CALL_TYPE_PS_DATA,
                 SYS_SYS_MODE_HDR,
                 &sys_id,
                 (sys_band_class_e_type)0,
                 0) == FALSE)
  {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                  "DCTM:Err posting ppp_call_fail to CM");
  }
}
#endif
/*===========================================================================

FUNCTION DCTM_POST_PPP_CALL_STATUS

DESCRIPTION
  This function  posts call fail type to CM based on PPP call fail/success
  event and the reason passed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_ppp_call_status
(
  ppp_event_e_type       ppp_event,                /* Down / Up            */
  dsdctm_ppp_protocol_e_type protocol,                 /* LCP, IPCP, Auth    */
  ppp_fail_reason_e_type ppp_fail_reason           /* timeout, auth_fail   */
)
{
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, 
                  "DCTM:Posting PPP status event: %d protocol: %d reason: %d",
                  ppp_event, protocol, ppp_fail_reason);

#ifdef FEATURE_DATA_OPTHO
  /* check if UE is in s101 tunnel mode. If yes then call into s101 dctm 
     else continue with direct mode  functionality. */

  if ( ds707_s101_query_sm_state() != DS707_S101_SM_NON_TUNNEL )
  {
    ds707_s101_dctm_post_ppp_call_status( ppp_event,
                                          protocol,
                                          ppp_fail_reason );
  }
  else
#endif
  {
    /* call direct mode dctm post ppp call status */
    dctm_post_ppp_call_status_in_direct_mode( ppp_event,
                                              protocol,
                                              ppp_fail_reason );
  }
  
} /* dctm_post_ppp_call_status() */


#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================

FUNCTION DCTM_POST_MIP_CALL_STATUS

DESCRIPTION
  This function  posts call fail tyep to CM based on MIP call fail/success
  event and the reason passed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_mip_call_status
(
  mip_event_e_type    mip_event,                  /*  SUCCESS / FAIL       */
  mip_fail_reason_e_type  mip_fail_reason/*sol,rrq timeouts,PDSN/FA/HA fail*/
)
{
  cm_ss_orig_thr_reasons_e_type call_status =
                           CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS;

  sys_sys_mode_e_type current_data_sess_network = SYS_SYS_MODE_NO_SRV;
  cm_client_id_type   ds3g_cm_client_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
 
  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "DCTM:Posting MIP status event: %d reason %d",
                  mip_event,
                  mip_fail_reason);

  current_data_sess_network = ds707_pkt_get_current_data_sess_network();

  if ((current_data_sess_network != SYS_SYS_MODE_CDMA) &&
      (current_data_sess_network != SYS_SYS_MODE_HDR))
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "DCTM:Ignore the DCTM MIP report. CDSN is %d",
                     current_data_sess_network );
    return;
  }

  switch(mip_event)
  {
    /*-----------------------------------------------------------------------
       Post PPP SUCCESS event if call is successful. This will let CM clear 
       the throttle flag if the call failed on the current SID-NID_PZID 
       earlier.
    -----------------------------------------------------------------------*/
    case MIP_SUCCESS_EV:
    {
      call_status = CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS;
      break;
    }
    /*-----------------------------------------------------------------------
       This event should only be posted  if PPP went down while the call is
       coming UP nto after the call is established.
    -----------------------------------------------------------------------*/
    case MIP_FAILURE_EV:
    {
      switch(mip_fail_reason)
      {
        case MIP_FAIL_REASON_SOL_TIMEOUT:
          call_status = CM_SS_ORIG_THR_DS_MIP_SOL_TIMEOUT;
          break;
        case MIP_FAIL_REASON_RRQ_TIMEOUT:
          call_status = CM_SS_ORIG_THR_DS_MIP_RRQ_TIMEOUT;
          break;
        case MIP_FAIL_REASON_HA_FAILURE:
          call_status = CM_SS_ORIG_THR_DS_MIP_HA_FAILURE;
          break;
        case MIP_FAIL_REASON_FA_FAILURE:
          call_status = CM_SS_ORIG_THR_DS_MIP_FA_FAILURE;
          break;
        case MIP_FAIL_REASON_PDSN_FAILURE:
          call_status = CM_SS_ORIG_THR_DS_MIP_PDSN_FAILURE;
          break;
        case MIP_FAIL_REASON_NONE:
          call_status = CM_SS_ORIG_THR_DS_MIP_FAILURE;
          return;
        default:
          call_status = CM_SS_ORIG_THR_REASON_NONE;
          break;
      }
      break;
    }
    default:
    {
      call_status = CM_SS_ORIG_THR_REASON_NONE;
      break;
    }
  }
  if ( call_status != CM_SS_ORIG_THR_REASON_NONE )
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();
    if( cm_ss_cmd_orig_thr_update_tbl(
                   NULL,  //CR: add cmts for NULL args.
                   NULL,
                   ds3g_cm_client_id,
                   CM_SS_ORIG_THR_UPDATE_CURR_SS,
                   call_status,
                   CM_CALL_TYPE_PS_DATA,
                   current_data_sess_network,
                   NULL,
                   (sys_band_class_e_type)0,
                   0) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "DCTM:Err posting MIP_call_fail to CM");
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DCTM: CM_SS_ORIG_THR_REASON_NONE, "
                    "Err posting MIP_call_fail to CM");
  }
} /* dctm_post_mip_call_status() */

#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================

FUNCTION DCTM_POST_V6_CALL_STATUS

DESCRIPTION
  This function  posts v6 call fail/success to CM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_v6_call_status
(
  dsdctm_dssnet6_event_type   v6_event       /* NET_UP / NET_DOWN */
)
{
  cm_ss_orig_thr_reasons_e_type call_status =
                           CM_SS_ORIG_THR_DS_V6_SUCCESS;

  sys_sys_mode_e_type current_data_sess_network = SYS_SYS_MODE_NO_SRV;
  cm_client_id_type   ds3g_cm_client_id;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  DATA_IS707_MSG1( MSG_LEGACY_MED, "DCTM: V6 event: %d", v6_event );

  current_data_sess_network = ds707_pkt_get_current_data_sess_network();

  if ((current_data_sess_network != SYS_SYS_MODE_CDMA) &&
      (current_data_sess_network != SYS_SYS_MODE_HDR))
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "DCTM:Ignore the DCTM V6 report. CDSN is %d",
                     current_data_sess_network );
    return;
  } 

  switch(v6_event)
  {
    /*-----------------------------------------------------------------------
       post SUCCESS on NET_UP_EV
    -----------------------------------------------------------------------*/
    case DSDCTM_DSSNET6_NET_UP_EV:
    {
      call_status = CM_SS_ORIG_THR_DS_V6_SUCCESS;
      break;
    }
    /*-----------------------------------------------------------------------
       post failure on NET_DOWN_EV
    -----------------------------------------------------------------------*/
    case DSDCTM_DSSNET6_NET_DOWN_EV:
    {
      call_status = CM_SS_ORIG_THR_DS_V6_FAILURE;
      break;
    }

    default:
    {
      call_status = CM_SS_ORIG_THR_REASON_NONE;
      break;
    }
  }

  DATA_IS707_MSG1( MSG_LEGACY_MED, "DCTM: posting v6 call status to CM: %d",
                   call_status );

  if ( call_status != CM_SS_ORIG_THR_REASON_NONE )
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();
    if( cm_ss_cmd_orig_thr_update_tbl(
                   NULL,  
                   NULL,
                   ds3g_cm_client_id,
                   CM_SS_ORIG_THR_UPDATE_CURR_SS,
                   call_status,
                   CM_CALL_TYPE_PS_DATA,
                   current_data_sess_network,
                   NULL,
                   (sys_band_class_e_type)0,
                   0) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "DCTM:Err posting v6_call_status to CM");
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DCTM: CM_SS_ORIG_THR_REASON_NONE, "
                    "Err posting V6_call_fail to CM");
  }
} /* dctm_post_v6_call_status() */


/*===========================================================================

FUNCTION DCTM_POST_SIP_FAIL

DESCRIPTION
  This function  posts call fail type to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dctm_post_sip_fail(void)
{
  cm_client_id_type ds3g_cm_client_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  sys_sys_mode_e_type current_data_sess_network = SYS_SYS_MODE_NO_SRV;

  current_data_sess_network = ds707_pkt_get_current_data_sess_network();

  if ((current_data_sess_network == SYS_SYS_MODE_CDMA) ||
      (current_data_sess_network == SYS_SYS_MODE_HDR))
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();
    /* DS calls CM API only when in HDR/CDMA mode */
    if( cm_ss_cmd_orig_thr_update_tbl(
                   NULL,
                   NULL,
                   ds3g_cm_client_id,
                   CM_SS_ORIG_THR_UPDATE_CURR_SS,
                   CM_SS_ORIG_THR_DS_PPP_FAILURE,
                   CM_CALL_TYPE_PS_DATA,
                   current_data_sess_network,
                   NULL,
                   (sys_band_class_e_type)0,
                   0) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "DCTM:Err posting ppp_call_sip_fail to CM");
    }
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "DCTM:Ignore the DCTM SIP report. CDSN is %d",
                     current_data_sess_network );
  }
}

#endif /* FEATURE_CALL_THROTTLE */
