#ifndef TRM_CLIENT_H
#define TRM_CLIENT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Internal Header File

GENERAL DESCRIPTION

  This file provides some common definitions for trm.cpp & trmlog.cpp


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trm_client.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/16/2015   jm      Adding TRM metric query support (CR: 778020)
12/11/2014   mn      LTE EMBMS functional changes (CR: 769538).
12/02/2014   mn      Populate and send subs Id to RF (CR: 744539)
11/24/2014   sr      Change reason to IRAT based on tech info (724649)
10/13/2014   sr      Use wake up idenfier info for detecting redundant
                     reserve at(CR:735314)
10/10/2014   sk      Fix same priority issue(700035)
10/13/2014   sk      Support for tx sharing DSDA
10/08/2014   sk      Added support for cross WTR PBR(708194)
09/26/2014   sr      Change reason with min duration (715526)
09/08/2014   sk      Fix idle tech to chain where QTA started (684738)
08/21/2014   sr      Support for Path concurrency restriction (699612)
08/14/2014   sk      wlan IRAT conflict support(708790)
08/12/2014   mn      Support for a unified IRAT mechanism which works across all 
                      TRM modes (CR: 705286).
08/04/2014   sk      Support to enable disable hopping for L+G coex issue(678827)
08/01/2014   sr      W is being allotted dev-3 for diversity when W primary is 
                     on dev-0 (703520)
07/31/2014   sr      Support for PBR Variance (681568)
07/28/2014   sr      Increment PBR count once for denials in 100 ms window (692593)
07/24/2014   mn      TRM needs to keep change reason pending if it is not 
                      allowed immediately. (CR: 694927).
07/07/2014   sr      1xSRLTE:-  Requirement for PBR to treat multiple 1x denials 
                      within 100 ms as redundant and get incremented by unity 
                      (CR: 685920).
07/17/2014   mn      Support for Adaptive Page Skipping (CR: 694722).
07/01/2014   sk      NV control for disabling idle mode hopping re-added(684328) 
06/26/2014   mn      DR band capability should be checked only for non-Diverisity
                      reasons. (CR: 686577).
04/11/2014   mn      TRM re-factoring. 

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"
#include "modem_mcs_defs.h"

extern "C"
{
  #include "trm.h"
}

#include "trm_client_array.h"
#include "trmi.h"
#include "trm_band_handler.h"
#include "queue.h"

/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/*----------------------------------------------------------------------------
  Client state information
----------------------------------------------------------------------------*/

/* Client is requesting an RF chain */
#define IS_REQUEST(s) ((s)>=TRM_RESERVED_AT  &&  (s)<=TRM_REQUEST_AND_NOTIFY)
#define IS_LOCKED(s)  ((s) >= TRM_LOCK_RETAINED)
#define IS_GPS_REQUEST(res) ((res) == TRM_RX_GPS)
#define IS_SECONDARY(res) ( ((res) == TRM_RX_DIVERSITY) || ((res) == TRM_RX_CA_SECONDARY))
#define IS_DR_RESOURCE(res) ((res) == TRM_RX_SECONDARY)
#define IS_DR_SECONDARY_RESOURCE(res) ((res) == TRM_RX_DIVERSITY_SECONDARY)
#define IS_SMODE_RESOURCE(res) ((res) == TRM_RXTX_BEST_SMODE)
#define IS_LOCK_RETAINED(state) ( (state == TRM_LOCK_RETAINED)|| \
                                  (state == TRM_LOCK_RETAINED_ADVANCED)|| \
                                  (state == TRM_LOCK_RETAINED_ENH) )
#define IS_REQUEST_AND_NOTIFY(res) (((res) == TRM_REQUEST_AND_NOTIFY) || \
                                    (((res) == TRM_REQUEST_AND_NOTIFY_ENH)))

/*----------------------------------------------------------------------------
  Feature related definitions 
----------------------------------------------------------------------------*/
#define TRM_REASONS_NONE  (uint64) 0
#define TRM_REASONS_ALL   (uint64)(~(TRM_REASONS_NONE))
#define REASON_BASED_FEAT_MASK  (uint8) (1<<TRM_FEATURE_APS)

#define TRM_CLIENT_HAS_FEATURE(id) ( trm.client[id].feature.feat_active_mask != \
								 TRM_FEATURE_NONE )

#define TRM_ASSOCIATED_RX_GROUP(group) ((group == CHAIN_0_GROUP) ? \
                                          CHAIN_1_GROUP : \
                                            CHAIN_3_GROUP)

#define TRM_IS_SEARCH_ACTIVE(client)    ((client).search_activity_q.cnt != 0)

#define TRM_CONNECTED_HOP_SUPPORTED(id) ( trm.hop_config.connected_hop_info.hop_enabled && \
                                           (trm.client[id].hopping_info.hop_state == \
                                           TRM_HOP_STATE_NO_HOP || \
                                           trm.client[id].hopping_info.hop_state == \
                                           TRM_HOP_STATE_PENDING) && \
                                          (trm.client[id].hopping_info.hop_reasons & \
                                           ((uint64)1) << trm.client[id].reason ) && \
                                           !TRM_MSK_BIT_ON(trm.hop_config.hop_config_mask, \
                                                           TRM_HOPPING_TYPE_CONNECTED_MODE) )

/*-----------------------------------------------------------------------------
  Earliest Usage Info type

    Structure to capture the earliest usage for each group

-----------------------------------------------------------------------------*/
typedef struct
{
  /* The earliest lock request times for each RF group */
  int32                     time;

  /* The earliest client needing the group */
  trm_client_enum_t         client;

} trm_earliest_usage_info_type;

/*----------------------------------------------------------------------------
  Lock extension request data
----------------------------------------------------------------------------*/

typedef struct 
{
  /* The desired lock extension ... maximum, if partial_ok == TRUE */
  int32                     duration;

  /* The computed maximum available */
  int32                     maximum;

  /* Whether or not a partial extension is acceptable */
  boolean                   partial_ok;

  /* Whether or not the extension is granted */
  boolean                   granted;
} trm_extension_struct;

/*----------------------------------------------------------------------------
  Group Assignment Type Enum
  This enum represents the types that could be returned when chain allocation
  is initiated.
----------------------------------------------------------------------------*/
typedef enum
{
  TRM_GROUP_UNASSIGNED = 0,
  TRM_GROUP_ASSIGNED,
  TRM_NO_CHAIN_AVAILABLE, /* Cant be allocated since no chain is free */
  TRM_MODE_INCOMPATIBILE, /* Ran into mode incompatibility */
  TRM_BAND_INCOMPATIBLE,  /* Ran into band incompatibility */
  TRM_PAM_INCOMPATIBLE,  /* Ran into PAM incompatibility */
  TRM_CHAIN_OCCUPIED,    /* Chain requested is already occupied */
  TRM_BP_ANY_COMBO_INCOMPATIBLE, /* Clients not compatible according to BP ANY combo */
  TRM_BAND_NOT_SUPPORTED,/* Chain requested does not support the given band */
  TRM_FEAT_INCOMPATIBLE,  /* Incompatible feature active */
  TRM_GROUP_DISALLOWED,    /* Group has been disabled */
  TRM_PATH_CONCURRENCY_INCOMPATIBLE, /* Concurrency disallowed due to PATH dependencies */
  TRM_INCOMPATIBLE_ONGOING_IFS_IRAT, /* There is an ongoing IRAT/IFS operation by another client */
  TRM_HOP_INITIATOR, /* tech has initiated hopping and waiting for hop to complete */
  TRM_WINNING_EXCEPTION, /* There is a winning exception due to which the losing client 
                            wins over winning client */
} trm_group_assignment_type;

/*----------------------------------------------------------------------------
  PBR information
----------------------------------------------------------------------------*/
typedef struct 
{
  /* number of times page has been denied for registered modes */
  uint32              pbr_count;

  /* drx cycle of the tech */
  uint32              drx_cycle;

  /* priority of the sim - 0: chain was assigned last call. 1: chain hasn't been assigned*/
  uint8               pbr_priority;

 /* Storing the previous priority. This is needed for redundant calls like res_at followed
    by a request */
  uint8               pbr_previous_priority;

  /* bias between the PBRs that has been set in EFS */
  uint32              pbr_bias;

  /* PBR for the current request has been accounted or not */
  boolean             pbr_accounted;

  /* Backup of pbr_accounted */
  boolean             pbr_previous_accounted;

  /* Modes for which PBR is being accounted for */
  uint64              pbr_modes;          
  
  /* If the request is coming within 100 ms */
  boolean             pbr_redundant; 

  /* Count for PBR Unnormalized Count */
  uint32              pbr_unnormalized_count;

  /* if a client is has a PBR conflict with lower PBR value,
     but also has feature like APS set due to which it got denied group,
     then give APS preference over PBR */
  boolean             skip_increment; 

  boolean             denied_in_redundant;

  boolean             variance_flag;

  /* If a tech loses due to pBR conflict, mark it */
  boolean             pbr_conflict;
} trm_pbr_info_type;

/*----------------------------------------------------------------------------
 Client feature attributes
----------------------------------------------------------------------------*/
typedef struct 
    {
  /* Bit mask representing active features, features that are reason dependent
     although enabled may not get active based on reason tech is using. The bit
     mask is represented by the enum: trm_feature_enum_t */
  uint8    feat_active_mask;

  /* bit mask representing enabled features.  The bit mask is represented by the
     enum: trm_feature_enum_t */
  uint8    feat_enabled_mask;

  /* Bitmask representing feature Incompatibilities */
  uint8    incompatible_mask;

  /* Chain associated with the feature */
  trm_group    feat_group;

  /* Indicates if it is primary tech for the feature */
  boolean  is_primary;

}trm_client_feature_info_t;

/*----------------------------------------------------------------------------
  Hop state enum type
  This enum represents the current state of the client
----------------------------------------------------------------------------*/
typedef enum
{
  /* Indicates that the client has not registered
     for device hopping */
  TRM_HOP_STATE_NOT_SUPPORTED,

  /* Indicates that the client supports device hopping
     but no hop is pending */
  TRM_HOP_STATE_NO_HOP,

  /* Indicates that the client supports device hopping
     but has declined to perform hops */
  TRM_HOP_STATE_NO_HOP_NO_RETRY,

  /* Indicates that the client supports device hopping
     and a hop is currently in progress */
  TRM_HOP_STATE_HOP,

  /* Indicates that hop is supported but is pending due to
     IRAT/IFS activity or band tune */
  TRM_HOP_STATE_PENDING,

  /* indicates the status of initiator waiting for hopping
     to complete */
  TRM_HOP_STATE_WAIT

} trm_hop_state_enum_type;

typedef enum
{
  CONNECTED_HOP_PENDING_NONE,

  CONNECTED_HOP_PENDING_BAND_TUNE,

  CONNECTED_HOP_PENDING_IRAT_IFS

}trm_connected_hop_pending_type;

typedef enum
{
  /* Indicates that the client has not registered
     for device hopping */
  CONNECTED_MODE_HOP_SUCCESS,

  /* Indicates that the client supports device hopping
     but no hop is pending */
  CONNECTED_MODE_HOP_NOT_ENABLED,

  /* Indicates that the client supports device hopping
     but has declined to perform hops */
  CONNECTED_MODE_HOP_NOT_SUPPORTED,

  /* Indicates that the client supports device hopping
     and a hop is currently pending */
  CONNECTED_MODE_HOP_BAND_NOT_SUPPORTED,

  /* indicates hop chain is occupied */
  CONNECTED_MODE_HOP_CHAIN_OCCUPIED,

  /* indicates hop rquest is kept pending */
  CONNECTED_MODE_HOP_PENDING


} trm_hop_engine_ret_type;

/*----------------------------------------------------------------------------
  IRAT/IFS Operation Data
----------------------------------------------------------------------------*/
typedef struct
{
  /* Link */
  q_link_type                link;

  /* Denotes whether the client is performing IRAT or IFS */
  trm_client_state_enum_type activity_type;

  /* Denotes whether this is a start event or an end event */
  trm_state_oper_enum_type   operation;

  /* Measured Client: Needed only for Start Operation */
  trm_client_enum_t         measured_client;

  /* Frequency Input: Needed only for Start Operation */
  trm_band_handler_freq_input_type freq_info;

  /* IRAT Identifier for start or end of operation */
  trm_irat_identifier       transaction_id;

  /* Duration: Needed only for Start Operation */
  trm_time_t                duration;

  /* Time stamp at which activity was started */
  trm_timestamp_t           time; 

  /* Time elapsed since the activity started */
  trm_timestamp_t           start;

  /* time remaining for the activity to end */
  trm_timestamp_t           end;

  /* Primary RF device on which search can be performed */
  rfm_device_enum_type      primary_device;

  /* Secondary RF device on which search can be performed */
  rfm_device_enum_type      diversity_device;

  /* Flag to indicate if diversity is needed for the search */
  boolean                   is_diversity_needed;

  boolean                   should_change_reason_to_irat;

} trm_client_search_activity_data_type;

/*----------------------------------------------------------------------------
  Hop state info types
----------------------------------------------------------------------------*/
typedef struct
{
  rfm_device_enum_type          hop_to_rf_device;
  trm_hop_state_enum_type       hop_state;
  boolean                       hop_callback_pending;
  uint64                        hop_reasons;

} trm_hopping_state_info_type;

/*----------------------------------------------------------------------------
  Hop config info type
----------------------------------------------------------------------------*/
typedef struct
{
   /* flag to indicated if hopping is enabled
      this is based on the device mode */
   boolean     hop_enabled;

   /* mask that indicate techs with active hopping
      bit position corresponds to client id */
   uint32      hopping_tech_mask;

   trm_client_enum_t             hop_causing_tech;

   trm_conn_mode_hop_type        hop_type;

}conected_mode_hop_info_type;

/*----------------------------------------------------------------------------
  Hop config info type
----------------------------------------------------------------------------*/
typedef struct
{
   /* flag to indicated if hopping is enabled
      this is based on the device mode */
   boolean     hop_enabled;

   
}idle_mode_hop_info_type;

/*----------------------------------------------------------------------------
  Hop config info type
----------------------------------------------------------------------------*/
typedef struct
{
   /* global flag to enable disable connected mode hopping */
  conected_mode_hop_info_type    connected_hop_info;

  /* global flag to enable/disable idle mode hopping
     it will be used to configure the device mapping */
  idle_mode_hop_info_type        idle_hop_info;

  /* this mask can be used to override(disable when set if it is enabled
     globally) hopping behavior due to various reasons,
     currently it will be set/reset whenever coex threshold is met in case
     of SV+G DSDA
     the bit position is based on trm_hopping_type enum */
  uint8       hop_config_mask;

} trm_hopping_config_info_type;

/*----------------------------------------------------------------------------
  Modify reason info type
----------------------------------------------------------------------------*/
typedef struct
{
  trm_reason_enum_t             new_reason;

  trm_reason_enum_t             curr_reason;

  boolean                       is_new_reason_allowed;

  trm_duration_t                min_duration;

} trm_modify_reason_info_type;

/*----------------------------------------------------------------------------
  Transceiver client request information
----------------------------------------------------------------------------*/

struct trm_client_lock : public trm_client_state
{
  /* Time stamp lock is required at */
  trm_timestamp_t           time; 

  trm_timestamp_t           last_request_time;

  /* Required lock duration */
  trm_duration_t            duration;

  /* "Now" based start and end times, for RF granting calculations */
  int64                     start, end;

  /* Anonymous payload linking trm_request_and_notify()/grant_cb() pairs */
  trm_tag_type         tag;

  /* Callback for the client */
  trm_grant_callback_type   grant_callback;


  /* Grant Callback for the client for the speacial 
     trm_request_and_notify_enhanced()*/
  trm_grant_enh_callback_type     grant_enh_callback;


#ifdef TEST_FRAMEWORK
#error code not present
#endif



  /* Time stamp retain lock starts at */
  trm_timestamp_t           retain_time;

  /* "Now" based retain start time, for RF granting calculations */
  int64                     retain_start;

  /* Retain lock duration */
  trm_duration_t            retain_duration;


  /* Winning client */
  trm_client_info_t               winning_client;
};

/*----------------------------------------------------------------------------
  Transceiver client information
----------------------------------------------------------------------------*/

class TRMClient : public trm_client_lock
{
public:
  /* The client who's structure this is */
  trm_client_enum_t         id;

  /* Client/reason compatibility with other client/reason pairs */
  trm_compatible_mask_t     mode, compatibility;

  /* Client state log buffer */
  trm_client_state          state_log;

  /* Urgent release needed */
  boolean                   urgent_release;

  /* Band incompatible release needed */
  boolean                   bc_incompatible_release;

  /* Is Secondary flag */
  boolean                   is_secondary;

  /* Associated client Primary/secondary client this client is 
     associated to */
  trm_client_enum_t         associated_client;

  /* RF Mode Information */
  trm_rf_mode_map_type*     rf_map;

  /* Priority of the client */
  trm_client_priority_enum_t  client_prio;

  trm_group                 requested_group;

  /* Chains used for BP/ANY resource requests */
  trm_chain_enum            first_config_chain;

  /* Used for alternative chain */
  trm_chain_enum            second_config_chain;
  
  /* Chains used for diversity */
  trm_chain_enum            associated_rx_chain;

  /* Cache var for last chain used for simultaneous rx as SHDR */
  trm_chain_enum            simultaneous_rx_chain;

  /* Signifies if the client can be present on multiple subscriptions */
  boolean                   is_multisub_client;

  /* Hopping state and config info */
  trm_hopping_state_info_type     hopping_info;

  /* bitmask of reasons supporting APS */
  uint64                    aps_reasons;

  boolean                   swap_for_any_allowed;

  trm_pbr_info_type         pbr_info;

  trm_group_assignment_type group_assignment_val;

  /* Feature info */
  trm_client_feature_info_t feature;

  trm_rat_group_type        rat_group;

  uint32                    async_cb_pending_bitmask;

  trm_tag_type              unlock_tag;

  trm_modify_reason_info_type modify_reason_info;

  trm_tech_info_mask_type   tech_info_bitmask;

  trm_request_criticality_type   req_criticality_info;

  boolean                     path_concurrency_restriction;

  trm_client_enum_t           path_concurrency_winner;

  void*                     operator new[](size_t size) throw();

  /* Does this client hold an RF lock? */
  boolean                   is_locked() const
  {
    return granted != TRM_DENIED;
  }

  /* Does this client hold an RF lock? */
  boolean                   is_compatible_with(trm_client_enum_t) const;

  /* Update the compatibility mask in case there were updates */
  void                      update_compatibility( void );

  /* Does this client overlap with another client? */
  boolean                   overlaps(const TRMClient &c) const;

  boolean overlaps_extension(const TRMClient & client) const;

  /* Set the reason a lock is being held for */
  void                      set_reason( trm_reason_enum_t reason );

  /* SV scheduler */

  /* Get higher priority clients & RF owners which overlap this client */
  void                      get_conflicts(TRMConflictArray &, boolean, trm_client_search_activity_data_type* search_data) const;

  /* Get maximum possible extension. A side effect is it will also mark
     the client as having to release urgently if a higher priority client
     is requesting the lock with an urgent reason. */
  int32                     get_maximum_extension( void );

  /* Apply the lock extension */
  void                      apply_extension( int32 extension );

  /* Extend a currently held RF lock as much as possible */
  void                      extend_chain_owner( void );

  /* Extend a currently held RF lock, subject to given control info */
  void                      extend_chain_owner(trm_extension_struct& extend);

  /* Assign the client to an RF group, if possible */
  void                      assign_group( trm_timestamp_t now );

  /* Check if the new reason is compatible with conflicting clients */  
  boolean                   check_new_reason(trm_timestamp_t now);

  /* Update now-based start/end times */
  void                      update_start_end( trm_timestamp_t now );

  /* Update the start and end times for IFS and IRAT operations */
  void                      update_ifs_irat_start_end( trm_timestamp_t now );

  /* Inform the client that it has been granted an RF chain */
  void                      band_grant_pending();

  /* Is the band compatible with the other client ? */
  boolean                   is_band_compatible(trm_client_enum_t);

  /* Is the request PAM compatible with the other client ? */
  boolean                   is_pam_compatible(trm_client_enum_t);


  /* Inform the client that it has been granted an RF chain */
  void                      grant_pending();

  /* Inform the client that its request for an enhanced lock
     has been granted. */
  void                      grant_enh_pending();

  /* Update unlock state */
  void                      update_unlock_state();

  /* Checks if there is a band class dependency with a client
     which is in lock until state. This is mainly required
     when a lower priority client in locked until state
     has a band class incompatibility with the client
     changing band class.*/
  void check_bc_dependancy_with_lock_until ( void );


  /* Start: Chain Allocation Logic Helper Functions */

  /* This method verifies if the chain under comparison time overlaps
     another chain owner or a retain lock request */
  trm_client_enum_t check_for_overlap(trm_chain_enum compare_chain );


  /* This method is to swap the chains if the transaction is fair and acceptable
     to the transaction client */
  boolean check_and_swap(
  trm_chain_enum& chain_to_get,     /* chain that is being sought */
  trm_chain_enum& chain_to_trade,   /* chain that we are ready to trade */
  TRMClient&     transaction_client /* client with which trading is sought */
  );


  /* This method is used to assign chain for best possible configuration */
  trm_group_assignment_type assign_best_possible_configuration(
  trm_timestamp_t now
  );

  /* This method is used to assign chain for any configuration */
  trm_group_assignment_type assign_any_chain_configuration(
  trm_timestamp_t now
  );

  trm_group_assignment_type assign_best_possible_2_configuration(
  trm_timestamp_t now
  ); 

/* This method is used to assign chain for reverse best possible configuration */
  trm_group_assignment_type assign_reverse_best_possible_configuration(
  trm_timestamp_t now
  );

  boolean check_for_other_client_activity_on_chain( trm_chain_enum compare_chain );


  /* This method is used to assign chain for reverse any configuration */
  trm_group_assignment_type assign_reverse_any_chain_configuration(
  trm_timestamp_t now
  );

  /* This method is used to assign chain for best possible modified configuration */
  trm_group_assignment_type assign_best_possible_mod_configuration(
  trm_timestamp_t now
  );

  /* This method is used to assign chain for best possible modified 2 configuration */
  trm_group_assignment_type assign_best_possible_mod_2_configuration(
  trm_timestamp_t now
  );

  trm_group_assignment_type assign_second_preferred(
  trm_timestamp_t now
  );  

  /* Holds the core logic for group assignment */
  trm_group_assignment_type group_assignment_logic( trm_group grp );



  /* End: Chain Allocation Logic Helper Functions */


  /* Start: Functions representing the algorithm Steps */

  /* Does the local optima prediction for chain allocation */
  void local_optima_prediction(trm_timestamp_t now);

  /* Does the local chain assignment. Uses group assignment logic */
  trm_group_assignment_type local_assignment(
  trm_group& group_sought, /* group that is being sought */
  trm_timestamp_t now
  );


  /* Provides the feedback after the local chain assignment */
  void local_feedback(const trm_group_assignment_type& feedback_data);

  /* Does any local correction that might need to be done */
  void local_correction(
  const trm_group_assignment_type& group_assign_value, /* allocation feedback */
  const trm_group& group_sought                    /* group requested */
  );

  void trm_modify_client_reason 
  (
  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t*       reason
  );
  /* End: functions representing the algorithm steps */

  /* PBR Algorithm Information */

  /* requested chain */
  trm_chain_enum      req_chain;

  /* Function to compare the PBR with the PBR of the compared client */
  boolean is_pbr_greater(trm_client_enum_t compared_client);

  void handle_redundant_pbr_request(trm_timestamp_t now, trm_reason_enum_t new_reason);

  void pbr_set_participation(trm_reason_enum_t new_reason, uint32 new_drx_cycle, 
                             boolean avoid_reset_if_cbt_in_range);

  void handle_device_hop_response
  (
    trm_async_event_response_device_hop_req_struct_type* response
  );
  trm_hop_engine_ret_type run_device_hop_engine(trm_chain_enum);

  void set_chain_owner(trm_grant_event_enum_t grant);

  trm_chain_enum check_chain_with_conflicting_clients(trm_timestamp_t now);

  void unlock(void);

  static void internal_grant_cb
  ( 
    /* The client which is being informed of an event */
    trm_client_enum_t               client_id,

    /* The event being sent to the client */
    trm_chain_grant_return_type     chain_grant,

    /* Anonymous payload echoed from trm_request_and_notify_enhanced() */
    trm_request_tag_t               tag
  );

  /* New async call-back type */
  trm_async_event_cb_type     async_cb;

  /* Optional Aysnc event bitmask. Register or unregister for notification
     Bit-0: Send information about DR Regions in DR-DSDS */
  uint32                           async_event_bitmask;

  /* BITMASK FOR MODIFY OPERTATIONS BELOW:
     BIT 0: Modify Duration
     BIT 1: Modify Band
     BIT 2: Modify Reason
     BIT 3: Modify DR-DSDS
  */

  /* bitmask to store information about modification request. This would need to be
     updated in the apis and after processing in the async call-back handler */
  uint32                        modify_req_bitmask;

  /* bitmask to store information about modification grant. This would be need to be
     updated by the scheduler*/
  uint32                        modify_grant_bitmask;

  /* This would be updated by the async call-back to avoid sending multiple pending call-backs. This
     would need to be reset in the apis every-time there is a request for a new modification */
  uint32                        modify_pending_sent_bitmask;


  /* Band Tune Action */
  trm_band_tune_action_type     band_tune_action;

  /* RF device */
  rfm_device_enum_type          rf_device;

  /* Queue that commands are written to to be sent to GSC. */
  q_type                        search_activity_q;

  /* Current transaction Id for search activities */
  trm_irat_identifier           curr_transaction_id;

  uint32                        num_irat_activities;

  trm_reason_enum_t             pre_irat_reason;

  /* time of last valid request  */
  trm_timestamp_t               time_of_request;

  uint64                        wakeup_identifier;

  /* feature related APIs */
  void enable_feature( trm_feature_enum_t feat, trm_group group);

  void disable_feature( trm_feature_enum_t feat );

  boolean is_compatible( uint8 feature_mask );

  void update_feature( trm_feature_enum_t   feat, 
                       uint64               feat_reasons, 
                       TRMFeatureArray*     feat_list );

  boolean is_feature_compatible(trm_group grp);
  /* feature related end */

  void run_rf_path_concurrency_checks(trm_group grp);

  boolean is_valid_bp_any_combo(trm_client_enum_t client);

  void register_for_connected_mode_hopping( boolean register_flag, uint64 reasons );

  void send_request_and_notify_async_info(void);
  void send_request_and_notify_enh_async_info(void);
  void send_modify_chain_state_async_info(void);
  void send_unlock_chain_async_info(void);
  void send_connected_mode_async_info(void);
  void send_dual_receive_async_info(void);
  void send_modify_reason_async_info(void);
  void send_modify_band_async_info(void);
  void send_modify_duration_async_info(void);
  void send_irat_conflict_unlock_event(trm_client_enum_t, trm_duration_t);
  static void send_async_event(void);

  void update_quick_tune_away_info
  (
     trm_qta_data_type* input
  );

  void handle_band_modification(trm_modify_chain_state_input_info *modify_state_info);

  boolean check_for_conflict_with_other_clients
  (
    trm_timestamp_t now, 
    trm_client_search_activity_data_type* data
  );

  void update_adapt_page_skip_info
  (
     trm_aps_data_type* input
  );

  void update_pbr_info(trm_modify_pbr_data *pbr_info_ptr);

  boolean schedule_search_activity
  (
    trm_client_search_activity_data_type* activity_ptr
  );

  void free_search_activity_entry(trm_irat_identifier transaction_id);

  trm_irat_identifier generate_search_activity_transaction_id(void)
  {
    return curr_transaction_id++;
  }

  boolean is_chain_allowed(trm_chain_enum comparison_chain);

  boolean auxiliary_client_check(void);

  static void try_new_reasons(trm_timestamp_t now);

  static void rollback_new_reasons(void); 

  /* Returns pointer to the client structure */
  TRMClient* get_pointer();

  /* Returns the device where the client can perform IFS or IRAT operation */
  rfm_device_enum_type get_rf_device_for_search_activity
  (
    trm_client_search_activity_data_type* activity_ptr
  );

  void handle_ifs_event
  (
    trm_timestamp_t now,
    trm_set_client_state_input_type* input, 
    trm_set_client_state_output_data* output
  );

  void handle_irat_event
  (
    trm_timestamp_t now,
    trm_set_client_state_input_type* input, 
    trm_set_client_state_output_data* output
  );

  boolean is_search_activity_allowed_on_devices
  (
    trm_client_search_activity_data_type* data
  );

  boolean is_compatible_with_ongoing_search_activities
  (
    trm_client_enum_t client_i
  );
};

#endif /* TRM_CLIENT_H */
