#ifndef HDRPAC_RLPCTL_H
#define HDRPAC_RLPCTL_H

/*===========================================================================


                      HDRPAC RLP CONTROL MODULE                       
                         

GENERAL DESCRIPTION  

Copyright (c) 2004-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrpacrlpctl.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
03/26/07   sk      Added EMPA MPA handoff 
05/03/07   sk      Support to process bundle response message from AN
12/23/05   ksu     EMPA RSP: two rev watermarks per ip flow
07/07/05   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_HDR_QOS

#include "hdrcom_api.h"
#include "hdrmrlpresv.h"
#include "ds707.h"
#include "ps_iface_defs.h"
#include "dsrlp_v.h"
#include "IxErrno.h"
#include "ds707_qosdefs.h"
#include "dsm.h"

/* <EJECT> */

/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/
typedef void (* hdrpacrlpctl_session_change_cb_type )
  (
    ds707_sys_qos_state_enum_type sys_qos_state,
    hdrcom_hai_app_subtype_enum_type  old_app_subtype,
    hdrcom_hai_app_subtype_enum_type  new_app_subtype,
    dsrlp_rlp_identity_type rlp,
    boolean is_default,
    boolean activated,
    uint8  resv_count,
    uint8  resv_label[], 
    boolean resv_bound,
    ds707_sec_pkt_protocol_cb_enum_type cback_qualifier
  );

typedef void (*hdrpacrlpctl_implicit_resv_on_cb_type )
                                           ( uint8 rev_label,
                                             ds707_direction_enum_type dirn);

typedef void (*hdrpacrlpctl_implicit_resv_off_cb_type )
                                           ( uint8 rev_label,
                                             ds707_direction_enum_type dirn);


/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/
boolean hdrpacrlpctl_is_rlp_activated
(
  dsrlp_rlp_identity_type rlp
);

/*===========================================================================
FUNCTION       HDRPACRLPCTL_GET_RESV_FOR_RLP

DESCRIPTION    
  

DEPENDENCIES   
  None

RETURN VALUE   
  None

SIDE EFFECTS   
  None.
  
===========================================================================*/
void  hdrpacrlpctl_get_resv_for_rlp
( 
  dsrlp_rlp_identity_type rlp,
  uint8 max_resv,
  uint8 *num_resv, 
  uint8 *resv 
);

/*===========================================================================
FUNCTION       HDRPAC_RLPCTL_REGISTER_FOR_NOTIFICATIONS

DESCRIPTION    
  

DEPENDENCIES   
  None

RETURN VALUE   
  None

SIDE EFFECTS   
  None.
  
===========================================================================*/
errno_enum_type hdrpacrlpctl_register_for_notifications
(
  hdrpacrlpctl_session_change_cb_type session_state_change_notification_cb    
);

#endif /* FEATURE_HDR_QOS */

/*===========================================================================
FUNCTION       HDRPACRLPCTL_BIND_WM_RLP

DESCRIPTION    
  This function binds a particular watermark to a particular RLP

DEPENDENCIES   
  None

RETURN VALUE   
  None

SIDE EFFECTS   
  None.
  
===========================================================================*/
void hdrpacrlpctl_bind_wm_rlp
( 
  dsrlp_rlp_identity_type rlp, 
  dsm_watermark_type tx_wm_list[]
);

/*===========================================================================
FUNCTION       HDRPACRLPCTL_UNBIND_WM_RLP

DESCRIPTION    
  This function binds a particular watermark to a particular RLP

DEPENDENCIES   
  None

RETURN VALUE   
  None

SIDE EFFECTS   
  None.
  
===========================================================================*/
void hdrpacrlpctl_unbind_wm_rlp
( 
  dsrlp_rlp_identity_type rlp, 
  dsm_watermark_type tx_wm_list[] 
);

/*===========================================================================

FUNCTION HDRMRLPSCHED_UNBIND_WM_RLP_EX

DESCRIPTION Wrapper around hdrmrlpsched_unbind_wm_rlp
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrmrlpsched_unbind_wm_rlp_ex
(
   byte                rscb_index, 
   dsm_watermark_type  del_wm_list[]
);

/*===========================================================================

FUNCTION HDRMRLPSCHED_BIND_WM_RLP_EX

DESCRIPTION  Wrapper around hdrmrlpsched_bind_wm_rlp to call it for each individual 
             route
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void  hdrmrlpsched_bind_wm_rlp_ex
(
   byte                rscb_index,
   dsm_watermark_type  add_wm_list[]
);

#endif /* HDRPAC_RLPCTL_H */
