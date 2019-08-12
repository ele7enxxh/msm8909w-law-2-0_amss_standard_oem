#ifndef DS707_SEC_PKT_MGR_H
#define DS707_SEC_PKT_MGR_H
/*===========================================================================

                      D S 7 0 7 _ S E C _ P K T _ M G R
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.h_v   1.4   27 Nov 2002 11:02:18   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_sec_pkt_mgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

-----------------------------------------------------------------------------
when        who    what, where, why
--------    ---    -------------------------------------------------------
11/18/11    vpk    HDR SU API cleanup
06/15/11    ls     Send DOS only when RLP is not inited which means that
                   HDR connection is not open and does not registers 
                   PCP call back
12/15/10    ms     Handle Iface related events by posting command to DS task.
03/09/09    sa     AU level CMI modifications.
03/05/07    sk     Added support for ANSupported Profile change
07/06/06    sk     QoS 2.0 Support
05/08/06    az     Changes for code re-structuring so that failure cases 
                   in request/release of QoS are handled appropriately
03/16/06    sk     Updated watermark behavior
03/12/06    az     When PPP resync happens, send Go NULL indication to 
                   apps whose filters were in DELETING state.
02/27/06    sk     Added handoff support
02/21/06    az     Added method to silently cleanup all secondary flows
01/05/06    ksu    EMPA: rx queues per tc state
12/12/05    spn    Added method to get RLP ID given a ps flow ptr and dirn
11/29/05    msr    Added support for dynamic QoS
11/19/05    sk     Added handoff functionality
11/07/05    sk     Added API bundling
03/29/05    gr     relocated the GET_TC_FROM_PHYS_LINK macro to pri pkt mgr
03/07/05    vas    Moved macro definitions from sec_pkt_mgri.h to this file
11/22/04    vas    Changes to Support GAUP/QOS Release

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_DATA_IS707
#include "ps_iface_defs.h"
#include "dstask_v.h"
#include "ds707_pkt_mgri.h"
#include "hdrmrlpcif.h"

/*===========================================================================
                            TYPEDEFS
===========================================================================*/


#define DS707_SEC_PKT_ATTEMPT_TO_CLEAN_DS_FLOW 0
#define DS707_SEC_PKT_FORCEFULLY_CLEAN_DS_FLOW 1

typedef struct
{
  ps_flow_type   *flow_ptr[MAX_QOS_OPERATIONS_PER_IOCTL]; /*PS flow array  */
  void           *client_data_ptr; /* Pointer to data associated with flow  */
  uint8           num_of_flows;    /* number of flows                       */
  ps_iface_type  * ps_iface_ptr;   /* iface ptr */
  boolean         is_nw_init_flow; /* If this is network initiated flow*/
  uint32          nw_init_resv_tid; /* NW initiated RESV msg transation ID*/
} ds707_sec_pkt_flow_cmd_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Struct used to pass info about Iface to Sec pkt mgr.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  ps_iface_event_enum_type      event; /* Event name */
  ps_iface_event_info_u_type    event_info; /* Event Info */
  void                          *client_data_ptr; /* Pointer to data associated with flow  */
} ds707_sec_pkt_iface_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Struct used to notify network initiated PPP down to sec pkt mgr
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  ps_iface_type              *ps_iface_ptr; /* iface on which PPP is gone */
} ds707_sec_pkt_ppp_cmd_type;

/* This enum describes the type of cleanup to perform - whether silent or
   non-silent for DQOS and/or AQOS.
*/
typedef enum
{
  DS707_SEC_PKT_MGR_RELEASE_TYPE_NULL  = 0,

  DS707_SEC_PKT_MGR_DQOS_NS            = 1,
  DS707_SEC_PKT_MGR_DQOS_S             = 2,

  DS707_SEC_PKT_MGR_AQOS_NS            = 4,
  DS707_SEC_PKT_MGR_AQOS_S             = 8,

  DS707_SEC_PKT_MGR_AQOS_NS_DQOS_NS    = DS707_SEC_PKT_MGR_AQOS_NS | DS707_SEC_PKT_MGR_DQOS_NS,
  DS707_SEC_PKT_MGR_AQOS_NS_DQOS_S     = DS707_SEC_PKT_MGR_AQOS_NS | DS707_SEC_PKT_MGR_DQOS_S,
  DS707_SEC_PKT_MGR_AQOS_S_DQOS_NS     = DS707_SEC_PKT_MGR_AQOS_S  | DS707_SEC_PKT_MGR_DQOS_NS,
  DS707_SEC_PKT_MGR_AQOS_S_DQOS_S      = DS707_SEC_PKT_MGR_AQOS_S  | DS707_SEC_PKT_MGR_DQOS_S,

  DS707_SEC_PKT_MGR_UE_FORCE_RELEASE   = 16

} ds707_sec_pkt_mgr_qos_resources_release_enum_type;

/*===========================================================================
                              MACROS
===========================================================================*/


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_RELEASE_QOS_RESOURCES

DESCRIPTION   
  This is a central function that releases QoS resources. The parameters
  supplied define whether to release silently or nonsilently both AQOS
  and/or DQOS.

  Once the cleanup is done, an attempt is made to assemble the release.
  
DEPENDENCIES  
  One cannot have both silent cleanup and non-silent cleanup for 
  either AQOS or DQOS.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_release_qos_resources(
  ps_flow_type                             ** ps_flow_ptr_arr,
  uint8                                       num_ps_flow_ptr,
  uint8                                       release_type,
  uint8                                       cleanup_type,
  ps_extended_info_code_enum_type             info_code
  );



/*===========================================================================
FUNCTION     DS707_SEC_PKT_PPP_DOWN_CMD

DESCRIPTION

  Posts a command for sec pkt mgr. This is to notify that PPP has been
  released by the network.
  
PARAMETERS

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_sec_pkt_network_initiated_ppp_down_cmd(
  ps_iface_type              *ps_iface_ptr
  );

/*===========================================================================
FUNCTION     DS707_SEC_PKT_MGR_PROCESS_IFACE_EVENT_HANDLER

DESCRIPTION
  Processes the Iface related events. 
  
PARAMETERS
  DS command buffer.
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/

void ds707_sec_pkt_mgr_process_iface_event_handler(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_PROCESS_CMD

DESCRIPTION   Processes commands issued to the sec pkt mgr.

DEPENDENCIES  This should be called from DS task.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ds707_sec_pkt_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_IS_QOS_AVAILABLE_ON_SYSTEM

DESCRIPTION

DEPENDENCIES  This should be called from DS task.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_sec_pkt_is_qos_available_on_system
(
  void
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_RLP_ID

DESCRIPTION    Finds the RLP ID for a particular flow pointer and the direction.

PARAMETERS     PS_FLOW_PTR and direction,
               return value of uint8 which is the rlp ID.

DEPENDENCIES   NONE

RETURN VALUE   TRUE if the returned value is valid

SIDE EFFECTS   
===========================================================================*/
boolean ds707_sec_pkt_rlp_id
(
  ps_flow_type* flow_ptr, 
  dsrlp_ver_enum_type ver,
  uint8 *rlpid
);   

/*===========================================================================
FUNCTION       DS707_SEC_PKT_INIT

DESCRIPTION
  This should be called once at powerup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_sec_pkt_init(void);

/*==========================================================================*
FUNCTION DS707_SEC_PKT_MGR_IS_DQOS_SUPPORTED

DESCRIPTION
  Returns whether dynamic QOS is supported or not

PARAMETERS
  NONE                                           

RETURN VALUE
  Whether dynamic qos is supported or not

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_sec_pkt_is_dqos_supported(void);


/*===========================================================================
FUNCTION       DS707_SEC_PKT_DQOS_INIT

DESCRIPTION
  This should be called once at powerup. Initializes sec pkt mgr to interact
  with DQOS module

DEPENDENCIES
  Ifaces must have been created before this function is called

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_sec_pkt_dqos_init
(
  boolean dqos_supported  /* Is D-QoS Supported or not */
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_IS_RLP_ACTIVE

DESCRIPTION    Returns if an RLP is active or not

PARAMETERS     PS_FLOW_PTR and direction

DEPENDENCIES   NONE

RETURN VALUE   TRUE if the rlp is active, FALSE if the rlp is inactive

SIDE EFFECTS
===========================================================================*/
boolean ds707_sec_pkt_is_rlp_active(ps_flow_type* flow_ptr, dsrlp_ver_enum_type ver );

/*===========================================================================
FUNCTION       DS707_SEC_PKT_IS_RLP_INITED

DESCRIPTION    This function is only called by DOS before sending DSM 
               to CM. It just needs to make sure that connection open 
               has not been called by HDR layer after the call is in
               dormancy. If the connection open is called, RLP ver_ptr 
               shall not be NULL. 

PARAMETERS     DS_FLOW_PTR: DS flow pointer 
               VER: RLP version

DEPENDENCIES   NONE

RETURN VALUE   TRUE if the rlp is inited or other sanity check failed
               FALSE if the rlp is not inited

SIDE EFFECTS
===========================================================================*/
boolean ds707_sec_pkt_is_rlp_inited
(
  const ds707_flow_type* ds_flow_ptr, 
  dsrlp_ver_enum_type ver 
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_RESV_BOUND_TO_RLP

DESCRIPTION    This is a registered function to get any notification if there
               is any reservation bound to RLP. This is notified by AN

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Reservations are bound to the RLP
===========================================================================*/
void ds707_sec_pkt_resv_bound_to_rlp
(
  dsrlp_rlp_identity_type rlp,
  int    num_resv,
  const uint8 *resv_ptr
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_RESV_UNBOUND_FROM_RLP

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_sec_pkt_resv_unbound_from_rlp
(
  dsrlp_rlp_identity_type rlp,
  int    num_resv,
  const uint8 *resv_ptr
);


/*===========================================================================

FUNCTION       DS707_SEC_PKT_MGR_CLEANUP_ALL_SECONDARY_FLOWS

DESCRIPTION    

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sec_pkt_mgr_cleanup_all_secondary_flows(
  ps_iface_type              *ps_iface_ptr,

  /* boolean to indicate that all flows, regardless of IPv4 or IPv6 
     are to be deleted. If you are releasing only v4 or only v6 flows,
     this should be set to FALSE. */
  boolean                      release_all_flows

  );


/*===========================================================================
FUNCTION       DS707_SEC_PKT_MGR_FLOW_VALIDATE

DESCRIPTION    

PARAMETERS     

DEPENDENCIES   

RETURN VALUE   

SIDE EFFECTS   
===========================================================================*/
boolean ds707_sec_pkt_mgr_flow_validate(
  ip_flow_type              * ip_flow_ptr,
  qos_spec_field_mask_enum_type     flow_type 
);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_FLTR_VALIDATE

DESCRIPTION   This function is called to validate filter parameters.

DEPENDENCIES  None.

RETURN VALUE  TRUE  - filter spec set is valid
              FALSE - one or more parameters in filter spec are invalid

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_sec_pkt_mgr_fltr_validate(
  ip_filter_type           * fi_ptr,
  qos_spec_field_mask_enum_type    flow_type
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_BUNDLE_FLOWS_VALIDATE

DESCRIPTION   This function is called to see if there is duplicate flow on
              the list of flow array

DEPENDENCIES  None.

RETURN VALUE  TRUE  - no duplicate flow found
              FALSE - duplicate flow found

SIDE EFFECTS  None.

===========================================================================*/
/*lint -save -e715 */
boolean ds707_sec_pkt_mgr_bundle_flows_validate(
  ps_flow_type              * ps_flow[],
  uint8                       num_flows
);
/*===========================================================================

FUNCTION       DS707_SEC_PKT_MGR_SEND_NULL_IND_ON_IFACE_UP

DESCRIPTION    Send NULL indication to apps whose filters were in DELETING 
               state. Then update their state to DELETED.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sec_pkt_mgr_cleanup_obsolete_fltrs(

  /* the iface ptr on which the processing needs to be performed */
  ps_iface_type               * ps_iface_ptr
  );

/*===========================================================================

FUNCTION      DS707_SEC_PKT_FLOW_SUSPEND_IND

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  TRUE  - filter spec set is valid
              FALSE - one or more parameters in filter spec are invalid

SIDE EFFECTS  None.

===========================================================================*/
void ds707_sec_pkt_flow_suspend_ind
(
  ps_flow_type                     * ps_flow_ptr,
  ps_extended_info_code_enum_type    info_code
);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_FLOW_GO_NULL_IND

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  TRUE  - filter spec set is valid
              FALSE - one or more parameters in filter spec are invalid

SIDE EFFECTS  None.

===========================================================================*/
void ds707_sec_pkt_flow_go_null_ind
(
  ps_flow_type                     * ps_flow_ptr,
  ps_extended_info_code_enum_type    info_code
);
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_GET_AVAILABLE_RESV_COUNT

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_get_available_resv_count(uint8 *fwd, uint8 *rev);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_RLP_ACTIVATED

DESCRIPTION    This function processes RLP activate command sent from AN.
               As a part of activation TC is allocated.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   RLP is activated
===========================================================================*/
void ds707_sec_pkt_rlp_activated
(
  dsrlp_rlp_identity_type rlp,
  boolean is_default
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_RLP_DEACTIVATED

DESCRIPTION    This function processes the RLP deactivate command sent from 
               AN. As a part of deactivation, the TC is made free and any 
               reservation label bound to this RLP is unbounded.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   RLP is deactivated.
===========================================================================*/
void ds707_sec_pkt_rlp_deactivated
(
  dsrlp_rlp_identity_type rlp,
  boolean is_default,
  hdrcom_hai_app_subtype_enum_type  app_subtype
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_AN_SUP_PROFILE_CHANGE

DESCRIPTION   
     
DEPENDENCIES  None
 
RETURN VALUE  None

SIDE EFFECTS  Action is taken based on callback
===========================================================================*/
void ds707_sec_pkt_mgr_an_sup_profile_change(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_GET_AN_SUP_PROF

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_get_an_sup_prof(
  hdrmrlpcif_an_supported_qos_profile_type *an_supported_profiles
);

#ifdef FEATURE_NIQ_EHRPD
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_IS_NW_INIT_FLOW

DESCRIPTION   Check if the ps_flow_ptr passed is UE initiated flow or NW 
              initiated flow. 

DEPENDENCIES  None.

RETURN VALUE  True: the flow passed is network initiated QOS flow 
              False: the flow passed is not network initiated QOS flow 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sec_pkt_mgr_is_nw_init_flow
(
  ps_flow_type                   *ps_flow_ptr
);


/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_IF_CONTAIN_NW_INIT_FLOW

DESCRIPTION   This function is called to see if there is a network initiated 
              QOS flow contained in the list of flow array.

DEPENDENCIES  None.

RETURN VALUE  TRUE  - network initiated flow contained.
              FALSE - no network initiated flow contained

SIDE EFFECTS  None.

===========================================================================*/
/*lint -save -e715 */
boolean ds707_sec_pkt_mgr_if_contain_nw_init_flow
(
  ps_flow_type              * ps_flow[],
  uint8                       num_flows
);
#endif /*FEATURE_NIQ_EHRPD*/
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_HDR_QOS */
#endif /* DS707_SEC_PKT_MGR_H */
