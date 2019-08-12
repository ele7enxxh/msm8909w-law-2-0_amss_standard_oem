
#ifndef DSDCTM_H
#define DSDCTM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S D C T M 

GENERAL DESCRIPTION
  This file  contains extern definistions for DCTM ( Data Call Throttle 
  Manager) module. This entity posts data session success/failure events 
  to CM.

EXTERNALIZED FUNCTIONS

  dsdctm_post_event()
    Send data session status events to CM.


  FEATURE_CALL_THROTTLE   - Supports Call-throttle based on previous fail 
                            reasons.

Copyright (c) 2004-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/dsdctm.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/04/09    ss     CMI SU level modifications.
04/23/09    ls     Add the support to report MIP rrp code
06/28/06    squ    added function dctm_post_sip_fail
07/29/05    kvd    Featurized MIP specific code.
10/08/04    kvd    Created Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "dsmip.h"
#include "dsmip_v.h"
#include "ps_ppp_defs.h"
#include "ds707_extif.h"
#include "hdraddr.h"

/*---------------------------------------------------------------------------
ENUM DSDCTM_PPP_PROTOCOL_E_TYPE 

DESCRIPTION
  protocol to report in ppp fail/success
  (values 0 - 4 same as ppp_protocol_e_type, added ehrpd_auth to differentiate
  between pap/chap and eap)
---------------------------------------------------------------------------*/
typedef enum 
{
  DSDCTM_PPP_PROTOCOL_MIN        = PPP_PROTOCOL_MIN,       /* 0 */
  DSDCTM_PPP_PROTOCOL_LCP        = PPP_PROTOCOL_LCP,       /* 0 */
  DSDCTM_PPP_PROTOCOL_IPCP       = PPP_PROTOCOL_IPCP,      /* 1 */
  DSDCTM_PPP_PROTOCOL_IPV6CP     = PPP_PROTOCOL_IPV6CP,    /* 2 */
  DSDCTM_PPP_PROTOCOL_AUTH       = PPP_PROTOCOL_AUTH,      /* 3 */
  DSDCTM_PPP_PROTOCOL_VSNCP,                               /* 4 */
  DSDCTM_PPP_PROTOCOL_EHRPD_AUTH,                          /* 5 */
  DSDCTM_PPP_PROTOCOL_MAX
} dsdctm_ppp_protocol_e_type;

/*---------------------------------------------------------------------------
ENUM DSDCTM_DSSNET6_EVENT_TYPE 

DESCRIPTION
  dssnet6 sm event to report to dctm (similar to DSSNET6_SM_EVENT_TYPE enum)
---------------------------------------------------------------------------*/
typedef enum
{
  DSDCTM_DSSNET6_NET_UP_EV        = 1,
  DSDCTM_DSSNET6_NET_DOWN_EV      = 2,
  DSDCTM_DSSNET6_MAX_EV        
} dsdctm_dssnet6_event_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_MIP_PPPP_STATUS

DESCRIPTION
  struct to report MIP , PPP fail/success to dssnet instance.
---------------------------------------------------------------------------*/
typedef struct ds_mip_ppp_status
{
  ppp_protocol_e_type     ppp_protocol;
  ppp_fail_reason_e_type  ppp_fail_reason;
#ifdef FEATURE_DS_MOBILE_IP
  mip_fail_reason_e_type  mip_fail_reason;
  byte                    mip_rrp_code;
#endif /* FEATURE_DS_MOBILE_IP */
  void*                   dssnet_instance;
} ds_mip_ppp_status;

/*---------------------------------------------------------------------------
TYPEDEF DS_V6_PPPP_STATUS

DESCRIPTION
  struct to report V6 , PPP fail/success to dssnet instance.
---------------------------------------------------------------------------*/
typedef struct ds_v6_ppp_status
{
  ppp_protocol_e_type     ppp_protocol;
  ppp_fail_reason_e_type  ppp_fail_reason;
  void*                   dssnet_instance;
} ds_v6_ppp_status;

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
  mip_event_e_type    mip_event,                 /* SUCCESS | FAIL         */
  mip_fail_reason_e_type  mip_fail_reason/*sol,rrq timeouts,PDSN/FA/HA fail*/
);
#endif /* FEATURE_DS_MOBILE_IP */

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
  ppp_event_e_type    ppp_event,                 /* Down/Up                */
  dsdctm_ppp_protocol_e_type protocol,    /* LCP. IPCP                   */
  ppp_fail_reason_e_type ppp_fail_reason  /* LCP, NCP Timeout, auth_fail   */
);

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
);
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
);
#endif
#endif /* DSDCTM_H */
