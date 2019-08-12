#ifndef HDRBCCPI_H
#define HDRBCCPI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R   B R O A D C A S T  C O N T R O L 
       
                             P R O T O C O L
       
                 I N T E R N A L   D E F I N I T I O N S
      

GENERAL DESCRIPTION
  This contains the declarations for regional types and variables used
  in the different files of BCCP.
    
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2005 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/api/hdrbccpi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/09   wsh     CMI: support changes from modem DATA
08/06/09   wsh     Support CMI API changes in sys.h 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/27/09   wsh     Merged BCMCS 2.0 on to main branch
09/18/08   smd     Added fix for off-target build.
09/27/06   mpa     Added hdrbccp_closing_reset_dsdb_query().
05/15/06   mpa     Added hdrbccp_update_flow_monitoring_status().
05/08/06   mpa     Added new cmd for queueing db lookup cb processing.
04/05/06   mpa     Keep track of format 1 program info in flow list.
02/13/06   mpa     Added parameter to hdrbccp_disable_all_monitored_flows().
12/13/05   mpa     Added parameter to hdrbccp_process_bc_ovhd_udpated().
12/13/05   mpa     Removed FEATURE_JCDMA.
11/30/05   grl     Added minor syntactical fix.
11/29/05   dna     Tune back to original channel on BCMCS ending if necessary
11/15/05   mpa     Added exit_traffic_for_best_bc_channel
11/13/05   mpa     Added parameter to hdrbccp_get_preferred_bc_channel()
09/21/05   mpa     Added hdrbccp_are_any_flows_requested()
09/19/05   mpa     Added multifrequency support
08/28/05   mpa     Created Module
===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrbccpfsi.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#ifdef FEATURE_CMI
#include "dsbcdb_api.h"
#endif

#include "dsbcdb_api.h"

/*===========================================================================

                      TYPE DECLARATIONS

===========================================================================*/

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#else
#define HDRBCCP_MAX_DESIRED_FLOWS 10
  /* Maximum number of desired(requested) flows. */

#define HDRBCCP_MAX_UNDESIRED_FLOWS 12
  /* Maximum number of undesired flows */

#define HDRBCCPI_BCMCS_MAX_FLOWS SYS_BCMCS_10_MAX_FLOWS

#endif /* FEATURE_HDR_BCMCS_2p0 */

#define HDRBCCPFS_MAX_FLOWS_TOTAL \
        ( HDRBCCP_MAX_DESIRED_FLOWS + HDRBCCP_MAX_UNDESIRED_FLOWS )

typedef enum
{
  HDRBCCP_ACTIVATE_CMD              = 0, /* Activate the BC protocol suite */
  HDRBCCP_DEACTIVATE_CMD            = 1, /* Deactivates the BC protocol suite */
  HDRBCCP_TEST_FLOW_ENABLE_CMD      = 2, /* Enables a test flow */
  HDRBCCP_TEST_FLOW_DISABLE_CMD     = 3, /* Disables a test flow */
  HDRBCCP_UPDATE_FLOW_REQ_CMD       = 4, /* Update Requested flow list */
  HDRBCCP_DISCONTINUE_BROADCAST_CMD = 5, /* Discontinue broadcast ops */
  HDRBCCP_DB_LOOKUP_COMPLETED_CMD   = 6 /* db lookup completed */
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS */
} hdrbccp_cmd_name_enum_type;

typedef struct
{
  uint8 transaction_id;
  uint8 num_flows;
  sys_bcmcs_flow_addr_s_type req_flow_list[HDRBCCPI_BCMCS_MAX_FLOWS];
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0*/
} hdrbccp_update_flow_req_params_type;

typedef struct
{
  uint8 transaction_id;
} hdrbccp_discontinue_bc_params_type;

typedef struct
{
  sys_bcmcs_flow_id_type flow_id; 
  void (*data_cb)(dsm_item_type * data_pkt);
} hdrbccp_test_flow_enable_params_type;

typedef struct
{
  sys_bcmcs_flow_id_type flow_id; 
} hdrbccp_test_flow_disable_params_type;

typedef struct
{
  dsbcdb_cmd_handle_type handle;
  dsbcdb_flow_list_type flow_list;
  sint15 err_number;
} hdrbccp_db_lookup_params_type;

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

typedef union
{
  hdrbccp_update_flow_req_params_type update_flow;
  hdrbccp_discontinue_bc_params_type stop_bc;
  hdrbccp_test_flow_enable_params_type flow_en;
  hdrbccp_test_flow_disable_params_type flow_dis;
  hdrbccp_db_lookup_params_type db_lookup;
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
} hdrbccp_cmd_params_union_type;

typedef struct
{
  hdrbccp_cmd_name_enum_type name;       /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* Protocol giving cmd, for debug */
  hdrbccp_cmd_params_union_type params;  /* Common Command Fields */
} hdrbccp_cmd_type;

typedef enum
{
  /* We keep track of registration timers for up to 3 subnets we
     have visited while the current FlowRequest list remains unchanged. 
     The registration timer list is reset (except for the current one) when 
     the FlowRequest list is modified.
     
     The Timer enum values must not be changed as they match the Subnet 
     registration indices for hdrbccpreg_subnet_reg_info_type */
  HDRBCCP_REG_TIMER_0 = 0,   
  HDRBCCP_REG_TIMER_1 = 1,
  HDRBCCP_REG_TIMER_2 = 2
} hdrbccp_timer_enum_type;

typedef struct
{
  /*--------------------------------
   Flow Address (ip:port#->FlowID)
  -------------------------------*/
  sys_bcmcs_flow_addr_s_type ip_port;
  sys_bcmcs_flow_id_type   flow_id;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

  /*--------------------------------
   Flow format specific info
  -------------------------------*/
  uint8  flow_format;

  /* format 1 specific */
  uint8  program_id_len;
  uint32 program_id;
  uint8  flow_discrim_len;
  uint8  flow_discrim;

} hdrbccp_flow_type;

typedef struct
{
  /*-------------------------------- 
   Link to Flow queue (priority order)
  -------------------------------*/
  q_link_type link;

  /*--------------------------------
   Flow Info
  -------------------------------*/
  hdrbccp_flow_type flow_info;
   
  /*--------------------------------
   Flow State
  -------------------------------*/
  hdrbccpfs_state_enum_type state;

  /*--------------------------------
   Registration Info 
  -------------------------------*/
  qword last_mon_time;
  qword last_reg_time;
  boolean is_in_last_reg;
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

  /*--------------------------------
   Priority (0 = highest)
  -------------------------------*/
  uint8 priority;

  /*--------------------------------
   Data pkt handling Info
  -------------------------------*/
  void (*data_cb)(dsm_item_type * data_pkt);
     /* Callback function to deliver broadcast data to (DS stack or Test App) */

} hdrbccp_flow_buf_type;

typedef struct
{
#if 0
  /* Transaction ID of the last report sent to CM */
  uint8 cm_rpt_trans_id;
#endif

  /* Queues and buffers used to maintain the list of current
   flows.  All the desired flows are listed first in the 
   list and flows that become undesired are kept at the end
   of the list for the time they are still needed for 
   registration purposes. */
  q_type flow_des_priority_q;
  q_type flow_undes_priority_q;
  q_type flow_free_q;
  hdrbccp_flow_buf_type flow_buf[HDRBCCPFS_MAX_FLOWS_TOTAL];

  /* Number of flows that are currently monitored 
     (Both desired and available on the current frequency) */
  uint8 mon_flow_cnt;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

  /* We don't allow test flows and DS flows to be enabled concurrently.
     DS requests take precedence of test flows. */
  boolean test_flow_enabled;

  /*---------------------------------------------------------------------------
   Broadcast Multifrequency info
   --------------------------------------------------------------------------*/
  hdrbccp_flow_buf_type * ref_flow_ptr; 
    /* Pointer to the reference flow */

  sys_channel_type bc_hash_channel;
    /* The BC hash channel to monitor */

  sys_channel_type original_channel;
    /* The channel we tuned away from to monitor a flow */

  boolean tune_back_to_original_channel_now;
    /* If we should tune back to the original channel next time we are asked */

  boolean exit_traffic_for_best_bc_channel;
    /* Flag if a flow request cmd was received during the current traffic 
       connection. If the AT finds a better frequency to tune to during the 
       current connection, it  will close its connection so it could tune to 
       that frequency.
      
       Note that this flag is reset as soon as the current traffic connection
       (or setup attempt) is torn down.
     */
} hdrbccp_flow_info_type;

/* <EJECT> */
/*=============================================================================

                  LOCAL FUNCTION DECLARATIONS FOR MODULE

=============================================================================*/

/*===========================================================================

FUNCTION HDRBCCP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to BCCP.

DEPENDENCIES
  None

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for BCCP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbccp_process_cmd
( 
  hdrbccp_cmd_type *bccp_cmd_ptr 
);

/*=============================================================================
FUNCTION HDRBCCP_PROCESS_TIMER

DESCRIPTION
  Callback function that gets called when a timer for hdrbcmcs expires
    
DEPENDENCIES
  None.

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/

void hdrbccp_process_timer
(
  uint32 timer_id
);

/*=============================================================================

FUNCTION HDRBCCP_PROCESS_MSG

DESCRIPTION
  This function processes messages for BCAST.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrbccp_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);


/*===========================================================================

FUNCTION HDRBCCP_PROCESS_IND

DESCRIPTION
  This function processes indications given to BROADCAST OVHD.

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication for OVHD to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbccp_process_ind
(
  hdrind_ind_name_enum_type   ind_name, 
  hdrind_ind_data_union_type *ind_data_ptr 
);

/*=============================================================================
FUNCTION HDRBCCP_ARE_ANY_FLOWS_MONITORED 

DESCRIPTION
   This function returns TRUE if it finds at least one flow in Monitored state.
                                     
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - At least one flow is DESIRED AVAILABLE and monitored.
  FALSE - No active flows

SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrbccp_are_any_flows_monitored( void );

/*=============================================================================

FUNCTION HDRBCCP_ARE_ANY_FLOWS_REQUESTED

DESCRIPTION
   This function returns TRUE if it finds at least one flow in the desired flow
   list.
  
DEPENDENCIES
  None.

PARAMETERS
  
RETURN VALUE
  TRUE  - At least one flow is in the desired flow list
  FALSE - No flows are requested

SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrbccp_are_any_flows_requested( void );

/*=============================================================================

FUNCTION HDRBCCP_ARE_ANY_FLOWS_SETUP_FOR_REG_HO

DESCRIPTION
   This function returns TRUE if it finds at least one flow in the desired flow
   list that is setup for additional registrations on HO.
  
DEPENDENCIES
  None.

PARAMETERS
  
RETURN VALUE
  TRUE  - At least one flow is setup for additional registrations on HO.
  FALSE - Additional registrations not needed.

SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrbccp_are_any_flows_setup_for_reg_ho( void );

/*=============================================================================
FUNCTION HDRBCCP_DISABLE_ALL_MONITORED_FLOWS 

DESCRIPTION
   Helper function to disable all the monitored flows.
   This will occur when:
   - the AT looses acquisition
   - the broadcast supervision timer expires
                                     
DEPENDENCIES
  None.

PARAMETERS
  status - reason for disabling all the monitored flows.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccp_disable_all_monitored_flows
( 
  sys_bcmcs_flow_status_e_type status
);

/*===========================================================================

FUNCTION HDRBCCP_CLOSING_RESET_DSDB_QUERY

DESCRIPTION
  This helper function resets the current dsdb lookups if the db lookups are
  in progress for this flow request and bccp is going into CLOSED state.  

  BCCP will re-request the db lookups when BCCP returns to OPEN state if the 
  flows haven't been discontinued.
  

DEPENDENCIES
  Only called upon entering CLOSED state.

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbccp_closing_reset_dsdb_query( void );

#ifdef FEATURE_HDR_BCMCS_MFRQ
/*=============================================================================
FUNCTION HDRBCCP_GET_PREFERRED_BC_CHANNEL 

DESCRIPTION
   Helper function to query the AT's preferred bc idle channel.
                                     
DEPENDENCIES
  None.

PARAMETERS
  bc_idle_channel_ptr - returns the preferred bc idle channel if it exists
  ref_flow_id_ptr - returns the reference FlowID the bc idle channel was based 
                    on. (currently only used for sw trace)
                
RETURN VALUE
  E_SUCCESS - successful query (found a reference flow)
  E_FAILURE - Broadcast protocol not negotiated (inactive)
  E_NOT_AVAILABLE - couldn't retrieve the idle bc channel info (no ref. flow)

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrbccp_get_preferred_bc_channel
(
  sys_channel_type *bc_idle_channel_ptr,
  sys_bcmcs_flow_id_type *ref_flow_id_ptr
);

/*=============================================================================
FUNCTION HDRBCCP_GET_ORIGINAL_CHANNEL 

DESCRIPTION
  This function is used at the time the AT stops using BCMCS, in the event 
  the AT needs to tune back to the channel it was on prior to starting BCMCS.
  In this case, the first time this routine is called after stopping BCMCS
  it will return the channel the AT was on when BCMCS was started i.e. when
  the reference flow first became non-null.  This function will only return
  E_SUCCESS the first time it is called after BCMCS is stopped.  Subsequent
  calls may return E_NOT_AVAILABLE.  This allows hdrovhd to call this function
  whenever it wants, but only be instructed to "tune back" one time.
  
  
DEPENDENCIES
  Used for FEATURE_JCDMA.

PARAMETERS
  current_channel_ptr - The channel the AT is currently tuned to
  original_channel_ptr - returns the channel in use prior to starting BCMCS
                
RETURN VALUE
  E_SUCCESS - There is an original channel that should be tuned to
  E_FAILURE - Broadcast protocol not negotiated (inactive)
  E_NOT_AVAILABLE - There is currently no original channel that needs to be 
                    tuned to.

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrbccp_get_original_channel
(
  sys_channel_type *current_channel_ptr,
  sys_channel_type *original_channel_ptr
);

#endif /* FEATURE_HDR_BCMCS_MFRQ */

/*===========================================================================
FUNCTION HDRBCCP_PROCESS_BC_OVHD_UPDATED

DESCRIPTION
  The Broadcast Overhead message has changed.  Update the flow table based on 
  the flows that were received in the Broadcast Overhead message.
  
DEPENDENCIES
  None.
  
PARAMETERS
  current_link_ptr   - Current channel and pilot
  is_first_after_acq - Whether this is the first ovhd updated indication
                       after acquisition.

RETURN VALUE
  TRUE  - AT needs to tune to a new bc channel
  FALSE - AT can stay on current channel
  
SIDE EFFECTS
  This is called directly from ovhd - acts as a command.
===========================================================================*/
extern boolean hdrbccp_process_bc_ovhd_updated
(
  const sys_link_type       *current_link_ptr,
  boolean is_first_after_acq
);

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/*===========================================================================

FUNCTION HDRBCCP_UPDATE_FLOW_MONITORING_STATUS

DESCRIPTION
  This function updates the BCMCS monitoring status for searcher and CM.

DEPENDENCIES
  None.

PARAMETERS
  TRUE  - the AT is monitoring flows
  FALSE - the AT is not monitoring any flows 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbccp_update_flow_monitoring_status
( 
  boolean flows_are_monitored
);

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */


#endif /* HDRBCCPI_H */


