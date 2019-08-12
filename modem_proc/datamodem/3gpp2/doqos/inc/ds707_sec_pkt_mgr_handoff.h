#ifndef DS707_SEC_PKT_MGR_HANDOFF_H
#define DS707_SEC_PKT_MGR_HANDOFF_H
/*===========================================================================

                        DS707 SECONDARY PACKET MANAGER HANDOFF

GENERAL DESCRIPTION
      
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_sec_pkt_mgr_handoff.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/18/11    vpk   HDR SU API cleanup
03/28/07    sk    Added support to handle RF condition
03/26/07    sk    Added EMPA MPA handoff 
03/05/07    sk    Added support to release previoius QoS
07/06/06    sk    QoS 2.0 Support
11/16/05    sk    Added support for handoff

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR_QOS
#include "rex.h"
#include "errno.h"
#include "ds707_qosdefs.h"
#include "ds707_pkt_mgr.h"
#include "ds707_cback.h"
#include "ps_iface_defs.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
typedef enum
{
  HANDOFF_CLIENT_IDM = 0,
  HANDOFF_CLIENT_HDR_PROTOCOL =1,
  HANDOFF_CLIENT_INVALID
}handoff_client_enum_type;

typedef struct
{
  sys_sys_mode_e_type curr_network;
  sys_sys_mode_e_type new_network;
}ds707_sec_pkt_mgr_handoff_network_change;

typedef union
{
  ds707_sec_pkt_mgr_handoff_network_change data_network;
  ps_iface_rf_conditions_enum_type rf_type;
}handoff_client_data;
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_INIT

DESCRIPTION   Register a call back function to get notification if there is 
              system changed from 1x to DO or DO to 1x

PARAMETERS

RETURN VALUE

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
void ds707_sec_pkt_mgr_handoff_init(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_LAST_PKT_APP

DESCRIPTION   

PARAMETERS    NONE

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  NONE
===========================================================================*/
hdrcom_hai_app_subtype_enum_type ds707_sec_pkt_mgr_handoff_last_pkt_app(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_SIGNAL_CHANGE_HDLR

DESCRIPTION   

PARAMETERS    NONE

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  NONE
===========================================================================*/
void ds707_sec_pkt_mgr_handoff_signal_change_hdlr(sys_sys_mode_e_type new_mode);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_MGR_HANDOFF_707_EVENT_CBACK

DESCRIPTION    This is the function registered with 707 pkt mgr to get 
               DS707_CBACK_NEW_NET_EV. This event indicates, the network moved
               from 1x to DO or DO to 1x. If the network changed from 1x to DO 
               and if the system is DORA (& the last network was CDMA), we 
               generated QOS_AWARE event to PS. Similarly, if the network moved 
               from DORA to 1x, we generate QOS_UNWARE event to PS.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sec_pkt_mgr_handoff_707_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_HANDOFF_TO_QOS_AWARE_SYS

DESCRIPTION   The function enables the filters for all the suspended flows and
              posts a generic IFACE indication to PS indicating that the 
              network is QOS_AWARE

PARAMETERS    NONE

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  NONE
===========================================================================*/
void ds707_sec_pkt_handoff_to_qos_aware_sys(boolean session_opened);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_HANDOFF_TO_QOS_UNAWARE_SYS

DESCRIPTION   The function acts based on the PS flow state as follows:
              ACTIVATING - clear the resource and post go_null_ind()
              GOING NULL - clear the resources and post go_null_ind()
              ACTIVATED, SUSPENDING, SUSPENDED, RESUMING - suspend the flow

PARAMETERS    NONE

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  The flow states may change
===========================================================================*/
void ds707_sec_pkt_handoff_to_qos_unaware_sys(boolean session_closed);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_RELEASE_PREV_AQOS

DESCRIPTION   

PARAMETERS    NONE

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  NONE
===========================================================================*/
uint8 ds707_sec_pkt_mgr_handoff_release_prev_aqos(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_RELEASE_PREV_QOS

DESCRIPTION   

PARAMETERS    NONE

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  NONE
===========================================================================*/
void ds707_sec_pkt_mgr_handoff_release_prev_qos(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_GET_BORDER_RF_CONDITION

DESCRIPTION   Register a call back function to get notification if there is 
              system changed from 1x to DO or DO to 1x

PARAMETERS

RETURN VALUE

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
ps_iface_rf_conditions_enum_type ds707_sec_pkt_mgr_handoff_get_border_rf_condition(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_RF_CHANGE

DESCRIPTION   Provides indication to apps

PARAMETERS

RETURN VALUE

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
void ds707_sec_pkt_mgr_handoff_rf_change(handoff_client_enum_type client_id, 
                                         void *client_ptr);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_HANDOFF_SESSION_CHANGED

DESCRIPTION    

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sec_pkt_handoff_session_changed(
  ds707_sys_qos_state_enum_type sys_qos_state, 
  hdrcom_hai_app_subtype_enum_type  old_app_subtype,
  hdrcom_hai_app_subtype_enum_type  new_app_subtype
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_HANDOFF_IS_QOS_AVAILABLE_ON_SYSTEM

DESCRIPTION   Utility function to identify QoS Aware/Unaware system

PARAMETERS

RETURN VALUE TRUE: If QoS is avalibale
             FALSE: If QoS is not available

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean ds707_sec_pkt_mgr_handoff_is_qos_available_on_system(void);

/*===========================================================================

                    FUNCTION DECLARATIONS

===========================================================================*/

#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SEC_PKT_MGR_UTIL_H */
