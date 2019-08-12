#ifndef DS_DSSNET_H
#define DS_DSSNET_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S G E N _ I F A C E _ H D L R . H

                           (formerly dssnet.h)
                           
DESCRIPTION
  Header file defining the API for the dssnet state machines. This file 
  provides the external API to the dssnet state machines and also has some
  of the common functionality that is used by both the dssnet state machines.

DEPENDENCIES
  The module MUST execute in the DS context.

EXTERNALIZED FUNCTIONS  
  dssnet_bring_up_iface_cmd()
   This function is registered with ps_iface to bring up the associated iface 
   during call establishment.
    
  dssnet_tear_down_iface_cmd()
   This function is registered with ps_iface to tear down the associated 
   iface during call release.
   
  dssnet_ppp_ncp_setup()
   This function is used by the dssnet state machines to setup the 
   corresponding NCPs (IPCP/IP6CP) during PPP establishemnt.

 Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/dsgen_iface_hdlr.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/26/12    msh    Read internal auth nv item 
01/16/12    ack    Fix to post a cmd for iface bringup/teardown
01/02/12    msh    Coan: Feature cleanup
06/06/11    ash    Added support for PPP Partial Context.
03/12/09  ssh/spn  PPP VSNCP interface changes
02/09/09    spn    Added support for VSNCP for eHRPD. 
07/23/07    sc     Fixed errors caused by non featurized 707 specific code 
                   in dssne_ppp_setup_ncp(), dssne_ppp_setup_lcp()
07/02/07    sc     Changed the signature of dssnet_ppp_setup_ncp() to support
                   Application Profiles.
07/29/04    kvd    Replaced ds707_bring_up/tear_down with dssnet_start/stop.
03/20/04    ak     Changed name at top of file, to match file name change.
01/12/04    aku    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"
#include "ps_ppp.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_data_session_profile.h"
#include "dsdctm.h"
#endif


#ifdef FEATURE_EHRPD
#include "ps_ppp_vsncp_3gpp2.h"
#endif

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  structure to store the call status for a v6 call. This structure will be 
  used by mode handlers, to propogate the error code in a failure case. 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum
{
  DS_CALL_CATEGORY_PPP_TYPE = 0,
  DS_CALL_CATEGORY_NON_PPP_TYPE  = 1,
  DS_CALL_CATEGORY_MAX
} ds_call_category_enum_type;

typedef struct
{
  ds_call_category_enum_type call_type;
  union{
#ifdef FEATURE_DATA_IS707
    ds_v6_ppp_status   ppp_status;
#endif
    int32              call_status;
  }call_end_reason_info;
  void *dssnet_instance;
} ds_v6_call_status_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSGEN_IFACE_HDLR_INIT()

DESCRIPTION
  This function is called in ds707_pkt_mgr_init to initialize dsgen_iface_hdlr

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsgen_iface_hdlr_init
(
  void
);

/*===========================================================================
FUNCTION DSGEN_IFACE_HDLR_USE_INTERNAL_AUTH()

DESCRIPTION
  This function is used to read the internal auth NV item. Return value 1
  means credentials from NV should be used, and 0 means credentials are
  passed in from client.

DEPENDENCIES
  None

RETURN VALUE
  1 internal auth should be used
  0 use auth from client

SIDE EFFECTS
  None
===========================================================================*/
boolean dsgen_iface_hdlr_use_internal_auth
(
  void
);

/*===========================================================================
FUNCTION DSSNET_START()

DESCRIPTION
   This function is registered with ps_iface to bring up the associated iface 
   during call establishment.

PARAMETERS
  Pointer to ps_iface on which to operate
  Void ptr to client data

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int dssnet_start
(
  ps_iface_type *this_iface_ptr,
  void* dssnet_instance,
  int16 *p_errno
);

/*===========================================================================
FUNCTION DSSNET_STOP()

DESCRIPTION
   This function is registered with ps_iface to tear down the associated 
   iface during call establishment.

PARAMETERS
  Pointer to ps_iface on which to operate
  Void ptr to client data

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int dssnet_stop
(
  ps_iface_type *this_iface_ptr,
  void* dssnet_instance
);

/*===========================================================================
FUNCTION DSSNET_PPP_SETUP_NCP()

DESCRIPTION
   This function is used by the dssnet state machines to setup the 
   corresponding NCPs (IPCP/IP6CP) during PPP establishemnt.

PARAMETERS
  Pointer to ppp config type which needs to populated
  Pointer to ps_iface on which to operate
  boolean to indicate whether this is a MIP call. 
  boolean to indicate whether to negotiate DNS using PPP (IPCP).
  Pointer to user info.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int dssnet_ppp_setup_ncp
(
  ppp_dev_opts_type                    *ppp_config_ptr,
  ps_iface_type                        *iface_ptr,
  boolean                              mip_call,
  boolean                              disable_ipcp_dns_opt,
  void                                 *user_info_ptr
);
#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION DSSNET_PPP_SETUP_LCP()

DESCRIPTION
   This function is used by the dssnet state machines to setup the 
   corresponding LCPs during PPP establishemnt.

PARAMETERS
  Pointer to ppp config structure which needs to be populated.
  Pointer to data sess profile type which has PPP param info.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssnet_ppp_setup_lcp
(
  ppp_dev_opts_type                    *ppp_config_ptr,
  ds707_data_session_profile_info_type *data_sess_profile_info
);
#endif /* FEATURE_DATA_IS707 */

#if defined (FEATURE_EHRPD) && defined (FEATURE_DATA_IS707)
/*===========================================================================
FUNCTION      DSSNET4I_CONFIGURE_VSNCP

DESCRIPTION   Configures the VSNCP Parameters for a given iface

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dssnet_configure_vsncp
(
  void*  dssnet_instance_ptr, 
  int pdn_instance,
  ps_iface_type      *ps_iface_ptr,
  ppp_vsncp_3gpp2_config_type* ncp_config_ptr,
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr
);
#endif /* FEATURE_EHRPD && FEATURE_DATA_IS707 */

#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================

FUNCTION dssnet_set_epc_handoff_ctxt
DESCRIPTION
  This function is called from MH ACLs to set IP context passed through
  the ACL policy. The transferred IP context will be stored in the MH.
 
PARAMETERS
  
  ps_iface_type* - if_ptr
  acl_policy_info_type* - acl_policy_info_ptr
  
DEPENDENCIES
  None

RETURN VALUE
  0  - If successful tranfer of IP context
  -1 - If error in transferring IP context

SIDE EFFECTS
  None

===========================================================================*/
int dssnet_set_epc_handoff_ctxt
(
  ps_iface_type* if_ptr,
  acl_policy_info_type* acl_policy_info_ptr
);
#endif
#ifdef FEATURE_DATA_IS707
/*===========================================================================

FUNCTION dssnet_call_allowed
DESCRIPTION
  This function is called from MH ACLs and phys link up processing to
  determine if the packet call is allowed with the current configuration.
  This checks if the profile can be used with the current configuration.
  It also checks for the handed off call.
 
PARAMETERS
  
  
  
DEPENDENCIES
  None

RETURN VALUE
  0  - If successful tranfer of IP context
  -1 - If error in transferring IP context

SIDE EFFECTS
  None

===========================================================================*/
boolean dssnet_call_allowed
(
  ds707_data_session_profile_info_type *prof_ptr,
  boolean                        handover_attach,
  ps_iface_net_down_reason_type *call_end_reason
);

#ifdef FEATURE_NIQ_EHRPD
/*===========================================================================
FUNCTION DSGEN_IFACE_HANDLE_NIQ_SBCM

DESCRIPTION
  This handles getting S-BCM value from PPP & providing it to
  nw_inint_qos_mgr. This is applicable for n/w init QOS scenario

DEPENDENCIES  None

RETURN VALUE  None
===========================================================================*/
void dsgen_iface_handle_niq_sbcm 
(
  ps_iface_type   *ps_iface_ptr
);
#endif /* FEATURE_NIQ_EHRPD */

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION DSGEN_IFACE_HANDLE_VSNCP_DEFAULT_APN_IND

DESCRIPTION
  This function checks if negotiated PDN connection is on default APN from
  VSNCP request and ack message sent from NW. If so, store the default APN
  for future default APN requests

DEPENDENCIES  None

RETURN VALUE  None
===========================================================================*/
void dsgen_iface_handle_vsncp_default_apn_ind
(
   ps_iface_type   *ps_iface_ptr
);
#endif /* FEATURE_EHRPD */
#endif /*FEATURE_DATA_IS707*/
#endif /* DS_DSSNET_H */
