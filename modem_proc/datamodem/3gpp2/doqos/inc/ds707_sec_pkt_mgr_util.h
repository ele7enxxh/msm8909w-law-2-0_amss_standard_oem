#ifndef DS707_SEC_PKT_MGR_UTIL_H
#define DS707_SEC_PKT_MGR_UTIL_H

/*===========================================================================

                        DS707 SECONDARY PACKET MANAGER
                        
GENERAL DESCRIPTION
  This module has utility/helper functions that are used by the other
  sec pkt mgr modules.
      
EXTERNALIZED FUNCTIONS
  ds707_sec_pkt_get_free_ds_flow
  ds707_sec_pkt_claim_used_ds_flow
  ds707_sec_pkt_is_reva_phys_layer_supported()

  
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_sec_pkt_mgr_util.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/23/12    ssb    Merged the info code UE_NOT_AUTHORIZED for DQOS
                   CHECK failure cases.
11/18/11    vpk    HDR SU API cleanup
04/07/11    ms     Global variables cleanup.
05/26/10    gc     Added code changes to include support for RMAC4(RevB)
                   subtype for QOS call.
02/19/09    sk     eHRPD: UE initiated QoS
07/09/08    spn    Added new function to get granted profile IDs for QOS flow
08/20/07    sk     API is provided to count free DS FLOW
05/01/07    sk     QoS Modify support
12/21/06    spn    Added functions to enable/disable, timestamping capability
07/06/06    sk     QoS 2.0 Support
05/08/06    az     Changes for code re-structuring so that failure cases 
                   in request/release of QoS are handled appropriately
03/30/06    spn    Removed some functions and placed it in 
                   ds707_pkt_mgri.h
03/12/06    az     Added utility function prototype to get all flows 
                   with active filters.
02/14/06    sy     Added support for multiple RLP 
                   flows in the forward direction.
01/26/06    ksu    EMPA: don't bind secondary flows to primary phys link
11/19/05    sk     Added handoff functionality
06/13/05    vas    Created module

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgri.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define GRANT_TIMER 9500 /* QoS Grant timer value in msec. */

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------	
	 Typedef for current ehrpd mode of UE
	 MODE_EHPRD when RLP is active and session is MMPA / EMPA and
	            flow proto id is VSNP
	 MODE_NON_EHRPD when RLP is active but session is not EMPA and MMPA OR
		    flow proto id is not VSNP
  MODE_UNKNOWN when RLP is not active
---------------------------------------------------------------------------*/
typedef enum
{
  MODE_NON_EHRPD = 0,
  MODE_EHRPD = 1,
  MODE_UNKNOWN = 2,
} ds707_sec_pkt_mgr_ehrpd_mode_type;
/*===========================================================================

                    FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================
FUNCTION      DS707_PKTI_REG_ALLTXFLOWS_WMK_REORIG_CBACKS

DESCRIPTION   Called when need to register the secondary watermarks
              which can trigger us out of dormancy.  Called when either
              we originate, get an incoming call, or connect, or go NULL.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_reg_alltxflows_wmk_reorig_cbacks_for_all_ifaces(void);

/*===========================================================================
FUNCTION      DS707_PKTI_DEREG_ALLTXFLOWS_WMK_REORIG_CBACKS

DESCRIPTION   Called when need to de-register the secondary watermarks
              which can trigger us out of dormancy.  Called when either
              we originate, get an incoming call, or connect, or go NULL.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_dereg_alltxflows_wmk_reorig_cbacks_for_all_ifaces(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_CLEANUP_RESERVATIONS

DESCRIPTION   

PARAMETERS    NONE

RETURN VALUE  NONE

SIDE EFFECTS  All the DS resources are freed and claimed back

===========================================================================*/
void ds707_sec_pkt_mgr_cleanup_reservations(
  ds707_flow_type                   * ds_flow_ptr
  );


/*===========================================================================
FUNCTION       DS707_SEC_IS_QOS_GRANTED_FOR_FLOW

DESCRIPTION   
  The function checks the reservation associated with the ps flow and then
  checks whether QoS has been granted for all the reservation.
    
DEPENDENCIES   
  None

RETURN VALUE   
  TRUE if QoS is granted on all the reservations associated with the flow
  FALSE otherwise

SIDE EFFECTS   
  None.
===========================================================================*/
boolean ds707_sec_is_qos_granted_for_flow
(
  ds707_flow_type *ds_flow_ptr
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_FIND_DS_FLOW

DESCRIPTION   
  This function searches through the pool of ds707 flows and returns a 
  pointer to the DS flow associated with the specified reservation.
  
DEPENDENCIES   
  None

RETURN VALUE   
  Pointer to the ds flow corresponding to the resevation, if it is found, 
  NULL otherwise

SIDE EFFECTS   
  None.
===========================================================================*/
ds707_flow_type *ds707_sec_pkt_find_ds_flow
( 
  uint8 resv,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_IS_TC_STATE_ALLOCATED

DESCRIPTION   
  This function return TRUE if the given tc_state 
  is already being used.
  
DEPENDENCIES   
  None

RETURN VALUE   
  Pointer to a tc_state.  

SIDE EFFECTS   
  None.
===========================================================================*/
boolean ds707_sec_pkt_is_tc_state_allocated
(
  ds707_tc_state_type *tc_state_ptr
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_GET_FREE_DS_FLOW_COUNT

DESCRIPTION
  This function searches through the pool of ds707 flows and returns available
  free ds_flows

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void  ds707_sec_pkt_get_free_ds_flow_count(uint8 *flow_count);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_GET_FREE_DS_FLOW

DESCRIPTION   
  This function searches through the pool of ds707 flows and returns a 
  pointer to an unallocated flow.
  
DEPENDENCIES   
  None

RETURN VALUE   
  Pointer to a free ds flow, if there is one available.
  NULL if there are no free ds flows

SIDE EFFECTS   
  None.
===========================================================================*/
ds707_flow_type *ds707_sec_pkt_get_free_ds_flow
( 
  ps_flow_type *ps_flow_ptr,
  ps_iface_type * ps_iface_ptr
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_CLAIM_USED_DS_FLOW

DESCRIPTION    Returns the DS Flow to the pool of DS Flows
  
DEPENDENCIES   
  None

RETURN VALUE  TRUE: Successfully claimed
              FALSE: Not able to claim

SIDE EFFECTS   
  None.
===========================================================================*/
boolean ds707_sec_pkt_claim_used_ds_flow
( 
  ds707_flow_type *used_ds_flow
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_FIND_TC_STATE

DESCRIPTION   
  This function searches through the pool of ds707 TC state structures for 
  the TC associated with the specified RLP and returns a pointer to it.
  
DEPENDENCIES   
  None

RETURN VALUE   
  Pointer to the TC state corresponding to the RLP, if found. NULL otherwise.

SIDE EFFECTS   
  None.
===========================================================================*/
ds707_tc_state_type *ds707_sec_pkt_find_tc_state
( 
  dsrlp_rlp_identity_type rlp /* RLP ID to allocate this TC for */
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_ALLOC_TC_STATE

DESCRIPTION   
  This function searches through the pool of ds707 TC state structures for 
  an unallocated one and allocates it for the RLP specified. If the RLP happens
  to be a forward MRLP structure, then it allocates an HDLC instance for the
  RLP too.
  
DEPENDENCIES   
  None

RETURN VALUE   
  Pointer to a free TC state structure, if there is one available.
  NULL if there are no free TC state structures

SIDE EFFECTS   
  The TC state strucutre is assigned for the RLP specified.
===========================================================================*/
ds707_tc_state_type *ds707_sec_pkt_alloc_tc_state
( 
  dsrlp_rlp_identity_type rlp /* RLP ID to allocate this TC for */
);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_FREE_TC_STATE

DESCRIPTION   
  This function frees the specified TC structure
  
DEPENDENCIES   
  None

RETURN VALUE   
  None
  
SIDE EFFECTS   
  None
===========================================================================*/
void ds707_sec_pkt_free_tc_state
( 
  ds707_tc_state_type *tc_state_ptr,
  dsrlp_rlp_identity_type rlp
);

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_GENERIC_IND

DESCRIPTION   Calls ps_iface_generic_ind() for both ifaces

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_generic_ind(
  ps_iface_event_enum_type             event,
  void                                *event_data_ptr
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_ENABLE_FILTER

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_enable_filter(
  ps_flow_type* flow_ptr
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_DISABLE_FILTER

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_disable_filter(
  ps_flow_type* flow_ptr
);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_IS_REVA_PHYS_LAYER_SUPPORTED

DESCRIPTION   Determines if the RevA Physical Layer (i.e. HDRSCP_SUB3_RMAC )
              is supported or not

DEPENDENCIES  None.

RETURN VALUE  TRUE implies that it is supported
              FALSE implies that it is not supported

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_sec_pkt_is_reva_phys_layer_supported
(
  void
);

#ifdef FEATURE_HDR_REVB
/*===========================================================================

FUNCTION      DS707_SEC_PKT_IS_REVB_PHYS_LAYER_SUPPORTED

DESCRIPTION   Determines if the RevB Physical Layer (i.e. HDRSCP_MC_RMAC)
              is supported or not

DEPENDENCIES  None.

RETURN VALUE  TRUE implies that it is supported
              FALSE implies that it is not supported

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_sec_pkt_is_revb_phys_layer_supported
(
  void
);
#endif /* FEATURE_HDR_REVB */

/*===========================================================================
FUNCTION       

DESCRIPTION   
  
DEPENDENCIES   
  None

RETURN VALUE   

SIDE EFFECTS   
  None.
===========================================================================*/
ds707_flow_type *ds707_sec_pkt_mgr_get_allocated_ds_flow
( 
  uint8 ds_flow_index
);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_CREATE_FLOW

DESCRIPTION   This function is called to create a new ps_flow given a QOS
              specification.

DEPENDENCIES  None.

RETURN VALUE  Ptr to new ps_flow created.

SIDE EFFECTS  None.

===========================================================================*/
ps_flow_type * ds707_sec_pkt_mgr_create_flow (
  qos_spec_type      *qos,          /* Qos specification                   */
  ps_iface_ipfltr_subset_id_type subset_id, /* ID assigned to filter spec  */
  ps_iface_type      *iface_ptr,     /* Iface ptr                           */
  boolean             is_nw_initiated, /* Indicates whether QoS 
                                                   is initiated by NW or UE */
  ps_iface_ioctl_qos_request_ex_opcode_enum_type	 opcode /* Request or Configure opcode */
);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_MODIFY_FLOW

DESCRIPTION   This function is called to modify a ps_flow given a QOS
              specification.

DEPENDENCIES  None.

RETURN VALUE  TRUE if modified, FALSE otherwise

SIDE EFFECTS  None.


===========================================================================*/
boolean ds707_sec_pkt_mgr_modify_flow(
  qos_spec_type                 * qos,          /* Qos specification         */
  ps_iface_ipfltr_subset_id_type  subset_id,    /* ID assigned to filter spec*/
  ps_iface_type                 * iface_ptr,     /* Iface ptr                 */
  ps_flow_type                  * ps_flow_ptr
);
/*===========================================================================
FUNCTION      DS707_SEC_PKT_GRANTED_SETID_ZERO

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sec_pkt_granted_setid_non_zero(ps_flow_type  * ps_flow_ptr, boolean *set_id_non_zero);

/*===========================================================================

FUNCTION       DS707_SEC_PKT_GET_ALL_ACTIVE_SECONDARY_FLOWS

DESCRIPTION    

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sec_pkt_get_all_active_secondary_flows(

  /* iface ptr for which flows are requested */
  ps_iface_type     * this_iface_ptr,

  /* array of ds flow pointers with active fltrs.. OUTPUT parameter */
  ds707_flow_type   * ds_flow_ptr_arr[],

  /* number of ds flows in the above array.. OUTPUT parameter */
  uint8          * num_ds_flows

  );


/*===========================================================================
FUNCTION DS_DQOS_MGRI_GET_ALL_FLOWS_WITH_ACTIVE_FLTRS

DESCRIPTION

PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_sec_pkt_mgr_get_all_flows_with_active_fltrs(

  /* iface ptr for which flows are requested */
  ps_iface_type     * this_iface_ptr,

  /* array of ds flow pointers with active fltrs.. OUTPUT parameter */
  ds707_flow_type   * ds_flow_ptr_arr[],

  /* number of ds flows in the above array.. OUTPUT parameter */
  uint8                                       * num_ds_flows,

  /* indicate the operation is applicable for which flow type */
  ds707_flow_applicable_type                    flow_type

  );

/*===========================================================================
FUNCTION       ds707_sec_pkt_mgr_grant_timer_init

DESCRIPTION    Initializes the QoS Grant Timer ..

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sec_pkt_mgr_grant_timer_init( void );

/*===========================================================================

FUNCTION       DS707_SEC_PKT_MGR_START_QOS_GRANT_TIMER

DESCRIPTION    Starts the QoS Grant Timer for the Specified Duration.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sec_pkt_mgr_start_qos_grant_timer( rex_timer_cnt_type  duration);


/*===========================================================================

FUNCTION       DS707_SEC_PKT_MGR_STOP_QOS_GRANT_TIMER

DESCRIPTION    Stop the QoS Grant Timer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_sec_pkt_mgr_stop_qos_grant_timer(void);

/*===========================================================================
FUNCTION       DS707_SEC_PKT_MGR_QOS_GRANT_TIMER_PROCESSING

DESCRIPTION    
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_sec_pkt_mgr_qos_grant_timer_expiry_processing(void);

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
boolean ds707_sec_pkt_is_dqos_accepted(ds707_flow_type *ds_flow_ptr);


/*===========================================================================
FUNCTION       DS707_SEC_PKT_QOS_RELEASED

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void ds707_sec_pkt_qos_released
(
  ds707_flow_type         *ds_flow_ptr
);


/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_FLTR_OPERATION_FAILED

DESCRIPTION   
  Processes the failure in installing/releasing filters at the PDSN.

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_fltr_operation_failed(
  ds707_flow_type             ** ds707_flow_ptr_arr,
  uint8                          num_ds_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_FLTR_OPERATION_TIMEOUT

DESCRIPTION   
  Processes the time out in installing/releasing filters at the PDSN.

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_fltr_operation_timeout(
  ds707_flow_type             ** ds707_flow_ptr_arr,
  uint8                          num_ds_flow_ptr
);


/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_ASSEMBLE_ACTIVATE

DESCRIPTION   
  Assemble the indications from DQOS and AQOS. When both are completed,
  send an activate indication to the app.

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_assemble_activate(
  ds707_flow_type                  * ds_flow_ptr,
  ps_extended_info_code_enum_type    activate_info_code
);
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_ASSEMBLE_SUSPEND

DESCRIPTION   
  Assemble the indications from DQOS and AQOS. When both are completed,
  send an suspend indication to the app.

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_assemble_suspend(
  ds707_flow_type                  * ds_flow_ptr,
  ps_extended_info_code_enum_type    suspend_info_code
);

/*===========================================================================
FUNCTION      ds707_sec_pkt_mgr_assemble_release

DESCRIPTION   
  Assemble the indications from DQOS and AQOS. When both are completed,
   (i)   post a GO_NULL indication to the app, 
   (ii)  Delete the ps flow,
   (iii) Delete the ds flow.

  If ds flow is null or cleanup type indicates a forceful cleanup of
  ds and ps flow, it does not validate that all conditions for release
  have been satisfied.

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_assemble_release(

  ps_flow_type                               * ps_flow,
  uint8                                        cleanup_type,
  ps_extended_info_code_enum_type              release_info_code
  );

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_QOS_GRANT_TIMER_RUNNING

DESCRIPTION   This is a utility function. Upon query, this will provide if 
              the grant timer is running or not

DEPENDENCIES  None.

RETURN VALUE  If the timer is running or not

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sec_pkt_mgr_qos_grant_timer_running(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_ENABLE_TIME_STAMP

DESCRIPTION   This is a utility function. On calling this function, it will 
              notify PS to starting time stamping the packets written to the 
              sockets

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_enable_time_stamp(
  dsrlp_rlp_identity_type rlp /* RLP ID for which we need to time stamp */
);  

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_DISABLE_TIME_STAMP

DESCRIPTION   This is a utility function. On calling this function, it will 
              notify PS to stop time stamping the packets written to the 
              sockets

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_disable_time_stamp(
  dsrlp_rlp_identity_type rlp /* RLP ID for which we need to time stamp */
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_RELEASE_ALL_FLOWS_SILENT

DESCRIPTION   This is a utility function. This will release all the flows silently

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  Releases all flows silently
===========================================================================*/
void ds707_sec_pkt_mgr_release_all_flows_silent(void);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_GET_PROFILE_IDS

DESCRIPTION   This is a utility function. Returns the profile IDs of a given
              PS flow

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sec_pkt_mgr_get_profile_ids
(
  ps_flow_type *ps_flow_ptr,
  cdma_flow_spec_profile_id_type *tx_profile_id,
  cdma_flow_spec_profile_id_type *rx_profile_id
);

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_GET_FLOW_PROT_ID_FWD

DESCRIPTION   This is a utility function. Returns flow proto id for fwd rlp

DEPENDENCIES  None.

RETURN VALUE  TRUE: if eHRPD mode of operation
              FALSE: if non eHRPD mode of operation

SIDE EFFECTS  None
===========================================================================*/
hdrcp_mrlpc_flow_prot_id_type ds707_sec_pkt_mgr_get_flow_prot_id(void);
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_GET_EHRPD_MODE

DESCRIPTION   This is a utility function. Returns the eHRPD mode

DEPENDENCIES  None.

RETURN VALUE  MODE_EHPRD - when RLP is active and session is MMPA / EMPA and
	             flow proto id is VSNP
              MODE_NON_EHRPD - when RLP is active but session is not EMPA and
		     MMPA OR flow proto id is not VSNP
              MODE_UNKNOWN - when RLP is not active

SIDE EFFECTS  None
===========================================================================*/
ds707_sec_pkt_mgr_ehrpd_mode_type ds707_sec_pkt_mgr_get_ehrpd_mode(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_SET_EHRPD_MODE

DESCRIPTION   This is a utility function. Sets the correct eHRPD mode and
              also generates bearer tech notification.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sec_pkt_mgr_set_ehrpd_mode(dsrlp_rlp_identity_type rlp, boolean rlp_activated);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_EHRPD_NV_READ

DESCRIPTION   This is a utility function. The eHRPD mode is read

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sec_pkt_mgr_ehrpd_nv_read(void);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_IS_EHRPD_CAPABLE

DESCRIPTION   This is a utility function returns the eHRPD capability

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sec_pkt_mgr_is_ehrpd_capable(void);

/*===========================================================================
FUNCTION        DS707_SEC_PKT_MGR_GET_NW_INIT_FLOWS

DESCRIPTION     This is utility function to get all NW init 
                DS flows on an iface.
  
PARAMETERS      this_iface_ptr : Interface ptr of which the flows to be taken
                ds_flow_ptr_arr: The array of ds flow pointer to be returned
                num_ds_flows:    number of ds flows in ds_flow_ptr_arr

RETURN VALUE    None

DEPENDENCIES    None

SIDE EFFECTS    ds_flow_ptr_arr is populated.
                num_da_flows is populated.
===========================================================================*/
void ds707_sec_pkt_mgr_get_nw_init_flows(

  /* iface ptr for which flows are requested */
  ps_iface_type     * this_iface_ptr,

  /* array of ds flow pointers with active fltrs.. OUTPUT parameter */
  ds707_flow_type   * ds_flow_ptr_arr[],

  /* number of ds flows in the above array.. OUTPUT parameter */
  uint8          * num_ds_flows

  );


/*===========================================================================
FUNCTION        DS707_SEC_PKT_MGR_GET_ALL_FLOWS_FOR_QOS_CHECK

DESCRIPTION     This is utility function to get all the DS flows on an iface.
  
PARAMETERS      this_iface_ptr : Interface ptr of which the flows to be taken
                ds_flow_ptr_arr: The array of ds flow pointer to be returned
                num_ds_flows:    number of ds flows in ds_flow_ptr_arr

RETURN VALUE    None

DEPENDENCIES    None

SIDE EFFECTS    ds_flow_ptr_arr is populated.
                num_da_flows is populated.
===========================================================================*/
void ds707_sec_pkt_mgr_get_all_flows_for_qos_check(

  /* iface ptr for which flows are requested */
  ps_iface_type     * this_iface_ptr,

  /* array of ds flow pointers with active fltrs.. OUTPUT parameter */
  ds707_flow_type   * ds_flow_ptr_arr[],

  /* number of ds flows in the above array.. OUTPUT parameter */
  uint8          * num_ds_flows

  );

/*=============================================================================
FUNCTION      DS707_SEC_PKT_MGR_UTIL_IS_CONFIRM_PROFILE_DIFFERENT

DESCRIPTION   This is a utility function and returns if previous confirmed 
              profile is different from new confirm profile list

DEPENDENCIES  None.

RETURN VALUE  TRUE: if two confirm profiles are different
              FALSE: otherwise

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_sec_pkt_mgr_util_is_confirm_profile_different(
  ds707_flow_type * ds_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_UTIL_IS_SUBSET_AUTHENTICATED

DESCRIPTION   This is a utility function and returns if confirmed profile is subset
              of authenticated profiles.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if confirm profile is subset of authenticated profile
              FALSE: otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sec_pkt_mgr_util_is_subset_authenticated
(
  ds707_flow_type                 *ds_flow_ptr,
  ps_extended_info_code_enum_type *info_code
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_UTIL_IS_PROFILE_CONFIRMED

DESCRIPTION   This is a utility function and returns TURE if the profile is found on 
              confirm list.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if profile is in confirm list
              FALSE: otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sec_pkt_mgr_util_is_profile_confirmed(
  ps_flow_type *ps_flow_ptr,
  ds707_direction_enum_type direction,
  uint16 profile_id 
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_UTIL_IS_PROFILE_IN_CONFIRM_LIST

DESCRIPTION   This is a utility function to check if confirm list corresponding
              to a flow is valid.

DEPENDENCIES  None.

RETURN VALUE  TURE: if confirm list is valid
              FALSE: otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sec_pkt_mgr_util_is_confirm_list_valid(
  ps_flow_type *ps_flow_ptr,
  ds707_direction_enum_type direction
);
#endif /* FEATURE_EHRPD*/

/*=============================================================================
FUNCTION      DS707_SEC_PKT_MGR_UTIL_REVOKE_INACTIVITY_TIMER

DESCRIPTION   This is a utility function that revokes inacitivity timer for 
              from RLP for the given ds flow.

DEPENDENCIES  ds_flow_ptr should be valid

RETURN VALUE  None

SIDE EFFECTS  None
=============================================================================*/
void ds707_sec_pkt_mgr_util_revoke_inactivity_timer
(
  ds707_flow_type *ds_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_RESOURCE_VALIDATE

DESCRIPTION   This function computes the availability of resource
              in the system to support the requested ioctl

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sec_pkt_mgr_resource_validate(
  uint8 fwd_requested_resv_total, 
  uint8 rev_requested_resv_total
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_UTIL_GET_PS_FLOW

DESCRIPTION   Get the ps_flow_ptr by given iface_ptr, flow id and direction. 
              This function is only used to get the NW initiated QOS flow 
     
DEPENDENCIES  None
 
RETURN VALUE  None

SIDE EFFECTS  Action is taken based on callback
===========================================================================*/
ps_flow_type*        ds707_sec_pkt_mgr_util_get_nw_init_ps_flow
(
  ps_iface_type             *this_iface_ptr,
  uint8                      this_flow_id,
  ds707_direction_enum_type  dirn
);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_UTIL_GET_PS_FLOW_ID

DESCRIPTION   Get the flow id of a given PS flow. Flow id is recored by the 
              reservation label. 
     
DEPENDENCIES  None
 
RETURN VALUE  None

SIDE EFFECTS  Action is taken based on callback
===========================================================================*/
uint8  ds707_sec_pkt_mgr_util_get_ps_flow_id
(
  ps_flow_type*    ps_flow_ptr  
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SEC_PKT_MGR_UTIL_H */

