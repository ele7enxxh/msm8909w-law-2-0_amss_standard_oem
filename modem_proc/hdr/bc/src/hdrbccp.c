/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 H D R   B R O A D C A S T   C O N T R O L 
       
                             P R O T O C O L       

GENERAL DESCRIPTION
  This module contains the implementation of the HDR  Broadcast Control 
  Protocol.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2004 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
10/15/10   cnx     Fixed a buffer overflow issue.  
09/30/09   wsh     Fixed KW warnings
09/18/09   wsh     CMI: support changes from modem DATA
08/06/09   wsh     Support CMI API changes in sys.h 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/27/09   wsh     Merged BCMCS 2.0 on to mainbranch
12/10/08   smd     Replaced assert.h with amssassert.h
10/01/08   wsh     Fixed critical/high lint warnings
09/18/08   smd     Added fix for off-target build.
12/12/07   sju     Renamed hdr channel record type structure
09/11/07   wsh     Removed lint high warnings
08/16/07   wsh     Removed compiler and lint critical warnings
08/08/07   mpa     Don't close connection for BCMCS when in HPT.
07/13/07   sju     Changed channel record type to hdr channel record type 
06/04/07   mpa     Fixed sending FlowStatusUpdate cmd on BroadcastReject
05/16/07   mpa     Fixed port 0 lookup processing.
04/13/07   pba     Added check for session change reason 
                   HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS.
10/18/06   mpa     Close BCMCS if session reconfigured without generic bcmcs.
09/27/06   mpa     Reset DB lookups upon entering CLOSED state.
07/20/06   mpa     Mainlined REVA_L1 session config (must have on 6800)
07/10/06   mpa     Fixed searcher page cycle notification.
07/05/06   mpa     Use session changed ind payload to check for session status.
05/31/06   mpa     Use dsbcdb_cmd_handle_type instead of uin32 for bcdb handles.
05/15/06   mpa     Added wrapper fns to update flow monitoring status.
05/08/06   mpa     Queue db lookup cb so this is processed in hdrmc task.
05/03/06   mpa     Ignore format check for test flows.
05/03/06   mpa     Only return preferred bc channel when desired q not empty.
04/07/06   dna     Add support for programs with FlowDiscriminatorCount == 0
04/05/06   mpa     Use format 1 registration format for format 1 flows
04/05/06   mpa     Set broadcast page cycle when session is reconfigured.
03/28/06   mpa     Allow FlowRequests in CLOSED state (db lookup is postponed
                   until activation.)
02/13/06   mpa     Added support for Broadcast Reject message.
12/15/05   mpa     Make sure that subnet handoff code is never exercised (temp)
12/13/05   mpa     Give Broadcast status to SD on first acq for bc_ovhd_updated
12/13/05   mpa     Removed FEATURE_JCDMA.
12/04/05   mpa     Pass session close ind down to hdrbccpc (set prior session)
11/29/05   dna     Tune back to original channel on BCMCS ending if necessary
11/18/05   hal     Uses CM API instead of directly accessing cm_mc_rpt_free_q
11/15/05   mpa     Only close the connection in traffic to tune to a better
                   bc channel if the last flow request was also rxed in traffic.
11/15/05   mpa     Remove BCMCS dependency on HDR session
11/15/05   mpa     Added flow_id parameter to hdrbccp_get_preferred_bc_channel
11/14/05   dlb     Fixed type-casting for RVCT compiler.
11/07/05   mpa     Fixed bcdb handle IDs
                   Cancel db lookups on DiscontinueBroadcast
10/21/05   pba     Added Broadcast protocol to SCM
10/07/05   mpa     Fixed registration initialization.
                   Initialize the bc state machine to close state.
10/05/05   as      Updated error code in hdrbccp_dsbcdb_lookup_cb.
10/05/05   mpa     Fixed tune back to SP hash when reference flow becomes NULL
09/30/05   mpa     Don't send flow status updates to CM/DS for test flows.
                   Activate bccp on session open.
           dna/mpa Fixed hdrbccp_find_best_bc_channel().
09/21/05   mpa     Added hdrbccp_are_any_flows_requested().
09/19/05   mpa     Added Multifrequency support.
09/12/05   mpa     Fixed requesting of multiple test flows.
08/28/05   mpa     HDR Broadcast Control Protocol rearchitecture
03/09/05   pba     Lint cleanup
12/20/04   pba     Changed generation of random_start_val_ms for reg period
12/10/04   hal     Added header file hdrutil.h.
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
11/18/04   mpa     Fixed ovhd info updating in the flow table.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
09/30/04   mpa     Fixed typo when negotiating attribs for default bc protocol.
08/05/04   kss     Add a data callback function to API; remove special case
                   for test mode.
08/10/04   mpa     Fixed calling hdrlmac_update_bc_info() upon monitoring 
                   first flow.
08/05/04   mpa     Track total number of monitored flows 
07/22/04   mpa     Fixed Reg Msg packing error and registration timers.
07/21/04   mpa     (ss) Added multifrequency support and new flow states
07/14/04   kss     Removed debug messages.
07/08/04   kss     Changed include of hdrbcmac.h to hdrbcmaci.h.
07/02/04   mpa     Added Session Negotiation + protocol events path.  
                   Code Cleanup.
                   Moved flow table handling here (from hdrbcovhd)
06/14/04   ss      (mpa for) Removed redundant flow handling
06/07/04   kss     Moved flow enable/disable for data path (mac, etc.)
                   to hdrbctask.c
===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "amssassert.h"

#ifdef FEATURE_HDR_BCMCS

#include "hdrbccp.h"
#include "hdrbccpi.h"
#include "hdrbccpc.h"
#include "hdrbccpfs.h"
#include "hdrbccpreg.h"
#include "hdrbccps.h"
#include "hdrbcmaci.h"
#include "hdrbcslot.h"
#include "hdrbcovhd.h"
#include "hdrbctask.h"
#include "hdridles.h" /* Peak at the idle state... */

#include "hdrhmp.h"
#include "hdralmp.h"
#include "hdrlmac.h"
#include "hdrrup.h"
#include "hdrmci.h"
#include "hdrscp.h"
#include "hdrsrchtypes.h"

#include "hdrerrno.h"
#include "hdrbit.h"
#include "hdrdebug.h"
#include "hdrtrace.h"
#include "hdrutil.h"
#include "ran.h"

#include "cmll.h"

#ifdef FEATURE_CMI
#include "cmll_v.h"
#endif

#include "cmxll.h"

#ifdef FEATURE_DS
#include "dsbcmcs_defs.h"
#include "dsbcdb_api.h"

#endif /* FEATURE_DS  */

/* Debugging messages */
#undef FEATURE_HDR_BCCP_DEBUG

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRBCCP_TASK_PTR hdrmc_tcb_ptr

/* Message IDs */

#define HDRBCCP_FLOW_REG_MSG    0x00
#define HDRBCCP_SEC_PARAMS_MSG  0x01
#define HDRBCCP_RESET_MSG       0x02
#define HDRBCCP_OVHD_MSG        0x03
#define HDRBCCP_REJECT_MSG      0x04

/*===========================================================================

                    MESSAGE PACKING / UNPACKING MACROS

===========================================================================*/

/*---------------------------------------------------------------------------
               Packing and unpacking for common fields
---------------------------------------------------------------------------*/

typedef uint8 hdrbccp_msg_id_type;

/* Unpack  */
#define HDRBCCP_UNPACK_MSG_ID( ptr, val_ptr )  \
        HDRBIT_UNPACK8( (ptr), (val_ptr), 0, 8 )

/* Pack */
#define HDRBCCP_PACK_MSG_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 0, 8 )

/*---------------------------------------------------------------------------
               BroadcastReject Message Field Extraction Macros
---------------------------------------------------------------------------*/
#define HDRBCCP_REJECT_MSG_ID_SIZE             8
#define HDRBCCP_REJECT_FLOW_FMT_SIZE           1
#define HDRBCCP_REJECT_FLOW_ID_LGTH_SIZE       2

#define HDRBCCP_REJECT_FLOW_COUNT_SIZE         6
#define HDRBCCP_REJECT_FLOW_ID_SIZE(length) \
                                 ((length+1) * 8)

#define HDRBCCP_REJECT_PROGRAM_COUNT_SIZE      6
#define HDRBCCP_REJECT_PROGRAM_ID_LSB_LENGTH_SIZE 5
#define HDRBCCP_REJECT_FLOW_DISCRIMINATOR_LENGTH_SIZE 3
                          
#define HDRBCCP_REJECT_SAME_AS_PREV_FLOW_SIZE  1
#define HDRBCCP_REJECT_REASON_SIZE             4

/* BCMCS Flow Format */
#define HDRBCCP_REJECT_FLOW_FMT(ptr, val_ptr) \
          HDRBIT_UNPACK8( ptr, val_ptr, 8, 1)
          
/* BCMCS Flow ID Length  */
#define HDRBCCP_REJECT_FLOW_ID_LGTH(ptr, val_ptr) \
          HDRBIT_UNPACK8( ptr, val_ptr, 9, 2)

/*==================
     Format 0
==================*/
          
/* BCMCS Flow Count */
#define HDRBCCP_REJECT_FLOW_COUNT(ptr, val_ptr, offset, length) \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, length)
          
/* BCMCS Flow ID */
#define HDRBCCP_REJECT_FLOW_ID(ptr, val_ptr, offset, length) \
        HDRBIT_UNPACK32( ptr, val_ptr, (uint16) offset, (uint16) length )

/*==================
     Format 1
==================*/
/* Program Count */
#define HDRBCCP_REJECT_PROGRAM_COUNT(ptr, val_ptr, offset, length) \
          HDRBIT_UNPACK8( ptr, val_ptr, offset, length)

/* Program ID LSB Length*/
#define HDRBCCP_REJECT_PID_LSB_LENGTH(ptr, val_ptr, offset, length) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset, length)

/* Program ID LSBs */
#define HDRBCCP_REJECT_PID_LSBS(ptr, val_ptr, offset, pid_lsb_length) \
          HDRBIT_UNPACK32( ptr, val_ptr, (uint16) offset, pid_lsb_length)

/* Flow Discriminator Length */
#define HDRBCCP_REJECT_FLOW_DISCRIM_LENGTH(ptr, val_ptr, offset, length) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset, length)

/* Flow Discriminator */
#define HDRBCCP_REJECT_FLOW_DISCRIMINATOR(ptr, val_ptr, offset, flow_discrim_length) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset, flow_discrim_length)

/*=====================
 Valid in both formats
=====================*/

/* Same As Previous BCMCS Flow/Program */
#define HDRBCCP_REJECT_SAME_AS_PREV_FLOW(ptr, val_ptr, offset, length) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset, length)
          
/* Reject Reason */
#define HDRBCCP_REJECT_REASON(ptr, val_ptr, offset, length) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset, length)



/*---------------------------------------------------------------------------
         Check channel range for Band Class 0 Subclass 2 and 3
                -- This is used for FEATURE_JCDMA --
---------------------------------------------------------------------------*/

/* From 3GPP2 C.P0057-A Version 0.03 Table 2.1.1-4 "CDMA Channel Numbers 
   and Corresponding Frequencies for Band Class 0 and Spreading Rate 1"
   Ranges include "invalid" channel numbers at the edge of the band

   Subclass 2: 1-166, 991-1023
   Subclass 3: 1-166, 991-1023, 1024-1323
 */

#define HDRBCCP_IS_BC0_SC2( c )  \
        ( ((c).band == SYS_BAND_BC0) && \
         ( (((c).chan_num >= 1   ) && ((c).chan_num <= 166 )) || \
           (((c).chan_num >= 991 ) && ((c).chan_num <= 1023)) ) )

#define HDRBCCP_IS_BC0_SC3( c )  \
        ( ((c).band == SYS_BAND_BC0) && \
         ( (((c).chan_num >= 1   ) && ((c).chan_num <= 166 )) || \
           (((c).chan_num >= 991 ) && ((c).chan_num <= 1023)) || \
           (((c).chan_num >= 1024) && ((c).chan_num <= 1323)) ) )

#define HDRBCCP_IS_BC0_SC3_NOT_SC2( c )  \
        ( ((c).band == SYS_BAND_BC0) && \
         ( ((c).chan_num >= 1024) && ((c).chan_num <= 1323) ) )

/*==================
     Format 1
==================*/
/* Not supported at this time */

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/


/* This is the lower limit used by the AT for 
 unmonitored reg_interval  */
#define HDRBCCP_UNMONITORED_REG_INTERVAL_MIN_MS 10000

#define HDRBCCP_MAX_UINT16 65535

#define HDRBCCP_CACHE_ENTRY_INVALID 0xFF

/*-----------------------------------------------------------------------------
                     BroadcastReject Message Structure
-----------------------------------------------------------------------------*/                      
typedef enum
{
  HDRBCCP_REJECT_FLOW_ID_UNAV     = 0,
  HDRBCCP_REJECT_FLOW_ID_NOT_XMIT = 1,
  HDRBCCP_REJECT_INV_AUTH_SIG     = 2,

  HDRBCCP_REJECT_RESERVED_MAX     = 3  /* Reject Reasons >= MAX are reserved */
} hdrbccp_reject_reason_enum_type;

typedef struct
{
  sys_bcmcs_flow_id_type flow_id;
  boolean same_as_prev_flow;
  hdrbccp_reject_reason_enum_type reject_reason;
} hdrbccp_reject_msg_flow_type;

typedef struct
{
  uint8 msg_id;
  uint8 flow_format;
  uint8 flow_id_length_in_bits;
  
  /* Format 0 */  
  uint8 flow_count;
  hdrbccp_reject_msg_flow_type flow[HDRBCCPI_BCMCS_MAX_FLOWS];

  /* Format 1*/
  /* We convert Format 1 flows into flow_id format */

} hdrbccp_reject_msg_type;

/*-----------------------------------------------------------------------------
                                Flow list Info
-----------------------------------------------------------------------------*/                      
typedef struct
{
  boolean lookup_success;
  boolean flow_already_requested;

  /*-------------------------------- 
   Flow info:
   - IP/Port# -> Flow ID mapping info 
   - format specific info
   -------------------------------*/
  hdrbccp_flow_type flow_info;

  /*--------------------------------
   CM flow status
   -------------------------------*/
  sys_bcmcs_flow_status_e_type status;

  /*--------------------------------
   BCDBAPP query info
   -------------------------------*/
  dsbcdb_cmd_handle_type db_query_handle;
    /* BCCP sends a query to BCDBAPP for each {IP, port#} in the requested flow 
       list lookup the unique mapping from {subnet, IP, port#} to flow ID. 
       The query handle is saved to match a database query with the flow the 
       query was for. */
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif
} hdrbccp_db_lookup_cache_entry_type;

typedef enum
{
  HDRBCCP_DB_NO_QUERY,            /* No query pending */
  HDRBCCP_DB_FLOW_REQ_QUERY,      /* dsdb query due to FlowRequest */
  HDRBCCP_DB_SUBNET_CHANGE_QUERY  /* dsdb query due to Subnet Change */
} hdrbccp_db_query_enum_type;

typedef struct
{ 
  boolean db_query_in_progress;

  uint8 transaction_id;
    /* CM Flow Request ID */

  uint8 req_flows;
    /* Original number of flows requested */

  uint8 total_flows;
    /* Number of flows in the list.  total_flows could be greater than 
       req_flows if there were port 0 lookups */

  uint8 num_addr_pending_lookup;
    /* Number of addresses still pending a callback from the db lookup */

  hdrbccp_db_query_enum_type query_type;
    /* BCCP copies its current desired flow list into the cache to do a dsdb
       query on subnet handoff.  BCCP aborts its current db lookups for 
       subnet changes if a new flow request update cmd comes in.  A new set
       of db lookups will be generated for the newly requested flow list. */
  
  hdrbccp_db_lookup_cache_entry_type flow_req_list[HDRBCCPI_BCMCS_MAX_FLOWS];
    /* BCCP caches the Flow Request list until a db lookup is completed for 
       each flow in the request.  Note that the Flow Request List always  
       contains the full set of currently requested flows.  So if a new request  
       comes in before the current one is still being processed, then the 
       current request is cancelled and we start processing the new one.
       
       The AT will still send a FlowRegSuccess report to CM for the cancelled 
       request.  Each flow status is set to SYS_BCMCS_FLOW_STATUS_CANCELLED
       in such a report.
       
    */

  dsbcdb_cmd_handle_type handle;
    /* Allocate handle for each flow requested. (_mpa_ wrap around...?)*/

} hdrbccp_db_lookup_cache_type;

LOCAL struct
{
  rex_crit_sect_type crit_sect; /* Critical Section */

  /*------------------------------------------------
    DB lookup Cache Info for Flow Request List
  ------------------------------------------------*/
  hdrbccp_db_lookup_cache_type db_cache;

  /*--------------------------------------------------------------------------- 
  Broadcast is available if the Generic Broadcast subtype is negotiated.  
  The broadcast status is only updated on powerup and when a new session is 
  opened.  i.e. the current status is not affected by a SessionClose.
  This removes the bcmcs dependency on the HDR session opening/closing so 
  broadcast could continue to run while the session is closed and until bcmcs
  is negotiated again.

  Note that the broadcast available status used here is different from the 
  Broadcast indicator kept by SD/CM.  SD uses the protocols broadcast status
  in conjuction with the HDR system loss indicator to indicate whether
  broadcast is available (to apps / CM).
  ---------------------------------------------------------------------------*/
  boolean broadcast_available;

} hdrbccp;

LOCAL typedef enum
{
  HDRBCCP_BC_OVHD_UPDATED         = 0,
  HDRBCCP_BC_DISABLED_SYS_LOST    = 1,
  HDRBCCP_BC_DISABLED_SUPERVISION = 2,
  HDRBCCP_BC_REJECTED             = 3
} hdrbccp_flow_status_cmd_reason;

/*------------------------------------------------
  Flow Info - externalized struct for modularizing
  hdrbccp Flow State machine and registration 
  (only used within HDRBCCP modules)
------------------------------------------------*/
hdrbccp_flow_info_type hdrbccp_flow_info;

/*=============================================================================

                      STATIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
FUNCTION HDRBCCP_TIMER_CB

DESCRIPTION
  This function is the callback for all broadcast protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC)    
  
DEPENDENCIES
  None.

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_timer_cb
(
  uint32 timer_id
)
{
  
  hdrmc_queue_timer(HDRHAI_BROADCAST_PROTOCOL, timer_id);

} /* hdrbccp_timer_cb() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDRBCCP_POWERUP_INIT

DESCRIPTION
  This function initializes the Broadcast protocol Suite on power up.
  It mainly defines some timers.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_powerup_init( void )
{

  uint8 i; /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_HIGH,"Initializing Broadcast Protocol");
  
  /*-------------------------------------------------------------------------
  Initialize the BCCP Critical Section
  -------------------------------------------------------------------------*/
  rex_init_crit_sect( &hdrbccp.crit_sect );

  /*-------------------------------------------------------------------------
  Initialize broadcast state module 
  -------------------------------------------------------------------------*/ 
  hdrbccps_powerup_init();

  /*-------------------------------------------------------------------------
  Retrieve the Session Configuration Parameters
  -------------------------------------------------------------------------*/
  hdrbccpc_powerup_init();

  /*-------------------------------------------------------------------------
  Initialize Registration Related Info 
  -------------------------------------------------------------------------*/
  hdrbccpreg_powerup_init();

  /*-------------------------------------------------------------------------
  Initialize the flow List info
  -------------------------------------------------------------------------*/
  
  /* Initialize the flow queues and put all the buffers on the free queue. */
  (void)q_init( &hdrbccp_flow_info.flow_des_priority_q );
  (void)q_init( &hdrbccp_flow_info.flow_undes_priority_q );
  (void)q_init( &hdrbccp_flow_info.flow_free_q );

  for( i=0; i < HDRBCCPFS_MAX_FLOWS_TOTAL; i++ )
  {
    (void)q_link( &hdrbccp_flow_info.flow_buf[i], 
                  &hdrbccp_flow_info.flow_buf[i].link );
    q_put( &hdrbccp_flow_info.flow_free_q, 
           &hdrbccp_flow_info.flow_buf[i].link );
  }  

  hdrbccp_flow_info.mon_flow_cnt      = 0;
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
  hdrbccp_flow_info.test_flow_enabled = FALSE;
  
  /*-------------------------------------------------------------------------
  Initialize multifrequency parameters
  -------------------------------------------------------------------------*/
  hdrbccp_flow_info.ref_flow_ptr            = NULL;
  hdrbccp_flow_info.exit_traffic_for_best_bc_channel = FALSE;

  /*-------------------------------------------------------------------------
  Initialize the Broadcast Available protocol status.  
  -------------------------------------------------------------------------*/
  hdrbccp.broadcast_available = 
    (hdrscp_get_current_subtype(HDRHAI_BROADCAST_PROTOCOL) == 
       HDRSCP_GENERIC_BCMCS) ? TRUE : FALSE;

} /* hdrbccp_powerup_init() */

/*===========================================================================

FUNCTION HDRBCCP_CM_MC_RPT

DESCRIPTION
  This function sends a report to CM.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - The pointer to the command to be sent to CM.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
STATIC void hdrbccp_cm_mc_rpt( cm_mc_rpt_type * cmd_ptr )
{
  cmd_ptr->hdr.task_ptr = NULL;

  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "CM MC rpt %d", cmd_ptr->hdr.cmd);
  cm_mc_rpt( cmd_ptr );
} /* hdrbccp_cm_mc_rpt() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCCP_GET_FLOW_STATUS_CMD_PTR

DESCRIPTION
  This function gets a CM flow status command buffer.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  cmd_ptr - Pointer to cmd buffer (NULL if can't allocate)

SIDE EFFECTS
  None.

===========================================================================*/
STATIC cm_mc_rpt_type* hdrbccp_get_flow_status_cmd_ptr( void )
{
  cm_mc_rpt_type* cmd_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    cmd_ptr->bcmcs_rpt.hdr.cmd = CM_HDR_BCMCS_FLOW_STATUS_F;

    /* HDR Broadcast */
    cmd_ptr->bcmcs_rpt.protocol = SYS_SYS_MODE_HDR;

    /* No Transaction ID associated with a Flow Status */
    cmd_ptr->bcmcs_rpt.transaction_id = 0;

    /* Fill in the flow info and status */
    cmd_ptr->bcmcs_rpt.num_of_flows = q_cnt(&hdrbccp_flow_info.flow_des_priority_q);

    /* Initialize the flow_status_array */
    memset( cmd_ptr->bcmcs_rpt.flow_status_array,
            0,
            HDRBCCPI_BCMCS_MAX_FLOWS * sizeof( sys_bcmcs_flow_status_element_s_type ) );

  }
  else
  {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

  return cmd_ptr;

} /* hdrbccp_get_flow_status_cmd_ptr() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCCP_SEND_FLOW_STATUS_TO_CM

DESCRIPTION
  This function sends a Flow Status report to CM.

DEPENDENCIES
  None.

PARAMETERS
  reason - Why we are sending a flow status

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
STATIC void hdrbccp_send_flow_status_to_cm
( 
  hdrbccp_flow_status_cmd_reason reason
)
{
  cm_mc_rpt_type *cmd_ptr = NULL;         
    /* Pointer to handset command */

  hdrbccp_flow_buf_type *q_buf_ptr = NULL;
    /* Iterator for desired flow list */

  uint8 index = 0;
    /* Iterators */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cmd_ptr = hdrbccp_get_flow_status_cmd_ptr()) == NULL)
  {
    return;
  }

  /*-----------------------------------------------------------------------
  Go through the flow table 
  -----------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type*) 
               q_check(&hdrbccp_flow_info.flow_des_priority_q);
    
    
  for (index=0; index < cmd_ptr->bcmcs_rpt.num_of_flows && q_buf_ptr != NULL; index++)
  {
    /* Copy the flow address (IP/Port#) for this flow*/ 
    cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_address = 
      q_buf_ptr->flow_info.ip_port;
                                         
    switch (reason)
    {
      /*---------------------------------------------------------------------
        Broadcast Overhead Updated
      ---------------------------------------------------------------------*/
      case HDRBCCP_BC_OVHD_UPDATED:
      {
        /* Determine the flow status based on the current flow state. */
        if (q_buf_ptr->state == HDRBCCPFS_DESIRED_MONITORED)
        {
          cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
            SYS_BCMCS_FLOW_STATUS_MONITORED;
        }
        else if (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNAVAILABLE)
        {
          cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
            SYS_BCMCS_FLOW_STATUS_REQUESTED;
        }
#ifdef FEATURE_HDR_BCMCS_2p0
        #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */       
        else if (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED)
        {
          cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
            SYS_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR;
        }
        else
        {
          ERR_FATAL("Invalid flow state in this context (%d)",
                    q_buf_ptr->state,0,0);
        }
      }
      break;

      /*---------------------------------------------------------------------
        Lost acquisition (all flows are now unavailable)
      ---------------------------------------------------------------------*/
      case HDRBCCP_BC_DISABLED_SYS_LOST:
      {
        cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
          SYS_BCMCS_FLOW_STATUS_LOST;
      }
      break;

      /*---------------------------------------------------------------------
        Broadcast supervision timeout (all flows are now unavailable)
      ---------------------------------------------------------------------*/
      case HDRBCCP_BC_DISABLED_SUPERVISION:
      {
        cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
          SYS_BCMCS_FLOW_STATUS_TIMEOUT;
      }
      break;

      /*---------------------------------------------------------------------
        AN sent a FlowReject msg
        Should not use the generic function for this.
      ---------------------------------------------------------------------*/
      case HDRBCCP_BC_REJECTED:
      default:
        ERR("Unknown Flow Status cmd reason (%d) - ignore",reason,0,0);
      break;
    }

    HDR_MSG_PROT_4(MSG_LEGACY_MED,"FlowID:%d IP:0x%x Port#:%d status:%d",
                   q_buf_ptr->flow_info.flow_id,
                   q_buf_ptr->flow_info.ip_port.ip_address.addr.v4,
                   q_buf_ptr->flow_info.ip_port.port_number,
                   cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status
                   );

    q_buf_ptr = (hdrbccp_flow_buf_type *)           
                q_next( &hdrbccp_flow_info.flow_des_priority_q,
                        &q_buf_ptr->link);

  }

  HDR_MSG_PROT_2 (MSG_LEGACY_MED,  
                "Sent CM_HDR_BCMCS_FLOW_STATUS_F (NumFlows: %d, reason %d)",
                  cmd_ptr->bcmcs_rpt.num_of_flows,
                  reason);

  hdrbccp_cm_mc_rpt( cmd_ptr );

} /* hdrbccp_send_flow_status_to_cm() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCCP_SEND_FLOW_STATUS_TO_CM_2

DESCRIPTION
  This function sends a Flow Status report to CM.

DEPENDENCIES
  None.

PARAMETERS
  reason - Why we are sending a flow status
  reject_msg_ptr - Pointer to a broadcast reject message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
STATIC void hdrbccp_send_flow_status_to_cm_2
( 
  hdrbccp_flow_status_cmd_reason reason,
  hdrbccp_reject_msg_type *reject_msg_ptr
)
{
  cm_mc_rpt_type *cmd_ptr = NULL;         
    /* Pointer to handset command */

  hdrbccp_flow_buf_type *q_buf_ptr = NULL;
    /* Iterator for desired flow list */

  uint8 index, cnt = 0;
    /* Iterators */

  boolean flow_rejected = FALSE;
    /* Keeps track of whether a desired flow was rejected 
       (for AN Rejected flow status updates) */ 

  uint32 program_id_lsbs = 0;
    /* Program ID lsb of current Flow for format 1 
       (for AN Rejected flow status updates) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cmd_ptr = hdrbccp_get_flow_status_cmd_ptr()) == NULL)
  {
    return;
  }

  /*-----------------------------------------------------------------------
  Go through the flow table 
  -----------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type*) 
               q_check(&hdrbccp_flow_info.flow_des_priority_q);
    
    
  for (index=0; 
       index < cmd_ptr->bcmcs_rpt.num_of_flows && q_buf_ptr != NULL; 
       index++)
  {
    /* Copy the flow address (IP/Port#) for this flow*/ 
    cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_address = 
      q_buf_ptr->flow_info.ip_port;
                                         
    switch (reason)
    {
      /*---------------------------------------------------------------------
        AN sent a FlowReject msg
        Should not use the generic function for this.
      ---------------------------------------------------------------------*/
      case HDRBCCP_BC_REJECTED:
      {
        /* Go through all the flows in the reject message to see if the 
           desired flow we are currently looking at was rejected */
        for (cnt=0; cnt < reject_msg_ptr->flow_count; cnt++)
        {
          /* Program Reject:
             If we are dealing with a FlowFormat 1 & the FlowDiscriminatorLength 
             is 0 (upper 3 bits of the ID), the entire program was rejected. 
             In this case we need to compare the programIDLSBs of the 
             requested flow to see if it matches the ProgramIDLSB of the 
             Rejected program.*/
          if ( (reject_msg_ptr->flow_format == 1) &&
               (reject_msg_ptr->flow[cnt].flow_id >> 
               (reject_msg_ptr->flow_id_length_in_bits - 3) == 0) )
          {
            if ( (hdrbcovhd_get_program_id_lsbs(q_buf_ptr->flow_info.flow_id,
                                          &program_id_lsbs) == E_SUCCESS ) &&
                 (program_id_lsbs == reject_msg_ptr->flow[cnt].flow_id) )
            {
              flow_rejected = TRUE;
              break;
            }
            else
            {
              ERR("Could not retrieve pid lsbs for flow %d",
                  q_buf_ptr->flow_info.flow_id, 0, 0);
            }
          }
          /* Flow Reject */
          else
          {
            if (q_buf_ptr->flow_info.flow_id == reject_msg_ptr->flow[cnt].flow_id)
            {
              flow_rejected = TRUE;
              break;
            }
          }
        } /* end for */

        /* If the flow was rejected, translate the Reject failure into a 
           flow status failure */
        if ( flow_rejected )
        {
          if ( reject_msg_ptr->flow[cnt].reject_reason == 
                                            HDRBCCP_REJECT_FLOW_ID_NOT_XMIT )
          {
            cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
              SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_TRANSMITTED;
          }
          else if ( reject_msg_ptr->flow[cnt].reject_reason == 
                                                HDRBCCP_REJECT_INV_AUTH_SIG )
          {
            cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
              SYS_BCMCS_FLOW_STATUS_REJECTED_INVALID_AUTH_SIG;
          }
          else /* if ( reject_msg_ptr->flow[cnt].reject_reason == 
                                             HDRBCCP_REJECT_FLOW_ID_UNAV ) */
          {
            cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
              SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_AVAILABLE;
          }
        }
        else /* flow not rejected, status based on current flow state */
        {
          if (q_buf_ptr->state == HDRBCCPFS_DESIRED_MONITORED)
          {
             cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
               SYS_BCMCS_FLOW_STATUS_MONITORED;
          }
          else if (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNAVAILABLE)
          {
            cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
              SYS_BCMCS_FLOW_STATUS_REQUESTED;
          }
          else if (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED)
          {
            cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
              SYS_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR;
          }
#ifdef FEATURE_HDR_BCMCS_2p0
          #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
          else
          {
            ERR_FATAL("Invalid flow state in this context (%d)",
                      q_buf_ptr->state,0,0);
          }
        }

        break;
      }

      default:
        ERR("Unknown Flow Status cmd reason (%d) - ignore",reason,0,0);
      break;

    }

    HDR_MSG_PROT_4(MSG_LEGACY_MED,"FlowID:%d IP:0x%x Port#:%d status:%d",
                   q_buf_ptr->flow_info.flow_id,
                   q_buf_ptr->flow_info.ip_port.ip_address.addr.v4,
                   q_buf_ptr->flow_info.ip_port.port_number,
                   cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status
                   );

    q_buf_ptr = (hdrbccp_flow_buf_type *)           
                q_next( &hdrbccp_flow_info.flow_des_priority_q,
                        &q_buf_ptr->link);

  } /* end for */

  HDR_MSG_PROT_2 (MSG_LEGACY_MED,  
                "Sent CM_HDR_BCMCS_FLOW_STATUS_F (NumFlows: %d, reason %d)",
                  cmd_ptr->bcmcs_rpt.num_of_flows,
                  reason);

  hdrbccp_cm_mc_rpt( cmd_ptr );

} /* hdrbccp_send_flow_status_to_cm_2() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_SEND_FLOW_REQ_SUCCESS_RPT_TO_CM

DESCRIPTION
  This function sends a Flow Request Success report to CM in response to
  a CM Flow Request Update command.
  
  This command is sent in the following cases:
  - (only) after all the flows in the corresponding Flow Request Update command 
    have been looked up in the DS database.
  - if a new request is received before BCCP is finished with the current one.
    All the flow statuses are set to SYS_BCMCS_FLOW_STATUS_CANCELLED in this
    situation.
  - if a new request is received when BCCP is available but in CLOSED state
    (can't do a db lookup yet until BCMCS is activated)

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
STATIC void hdrbccp_send_flow_req_success_rpt_to_cm( void )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
  uint8 index;                     /* iterator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(hdrbccp.db_cache.total_flows <= HDRBCCPI_BCMCS_MAX_FLOWS);

  /* Save the trans ID for the last flow req msg we processed from CM. */
  //hdrbccp_flow_info.transaction_id = hdrbccp.db_cache.transaction_id;

  if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    cmd_ptr->bcmcs_rpt.hdr.cmd = CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F;

    /* HDR Broadcast */
    cmd_ptr->bcmcs_rpt.protocol = SYS_SYS_MODE_HDR;

    /* Transaction ID associated with the corresponding flow request */
    cmd_ptr->bcmcs_rpt.transaction_id = hdrbccp.db_cache.transaction_id;

    /* Number of flows in the request (including additional port 0 flows)  */
    cmd_ptr->bcmcs_rpt.num_of_flows = hdrbccp.db_cache.total_flows;

    /* Fill in the flow status for each flow.
       - Note - there may be more flows than originally requested (port 0 
                requests may map to more than one flow.  CM only includes 
                the IP:Port# address in the cmd.  For port0 addresses, the
                same addresses could be repeated multiple times with different
                statuses depending on which FlowID we are giving a status for.
               _mpa_ How does DS act on those/differentiates? 
     */
    for (index=0; index < hdrbccp.db_cache.total_flows; index++)
    {
      memcpy( &(cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_address),
              &(hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port),
              sizeof(sys_bcmcs_flow_addr_s_type) );
      
      cmd_ptr->bcmcs_rpt.flow_status_array[index].flow_status = 
        hdrbccp.db_cache.flow_req_list[index].status;

#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#else
      HDR_MSG_PROT_4(MSG_LEGACY_MED,"FlowID:%d IP:0x%x Port#:%d status:%d",
        hdrbccp.db_cache.flow_req_list[index].flow_info.flow_id,
        hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.ip_address.addr.v4,
        hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.port_number,
        hdrbccp.db_cache.flow_req_list[index].status
        );
#endif /* FEATURE_HDR_BCMCS_2p0 */
    }
    
    HDR_MSG_PROT_2 (MSG_LEGACY_MED,  
            "Sent CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F (TID: %d, flows: %d)",
            cmd_ptr->bcmcs_rpt.transaction_id,
            cmd_ptr->bcmcs_rpt.num_of_flows);

    hdrbccp_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

} /* hdrbccp_send_flow_req_success_rpt_to_cm() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_SEND_BROADCAST_RESET_RPT_TO_CM

DESCRIPTION
  This function sends a Broadcast Reset Report to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
STATIC void hdrbccp_send_broadcast_reset_rpt_to_cm( void )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    cmd_ptr->bcmcs_rpt.hdr.cmd = CM_HDR_BCMCS_BROADCAST_RESET_F;

    /* HDR Broadcast */
    cmd_ptr->bcmcs_rpt.protocol = SYS_SYS_MODE_HDR;

    /* Initialize the remainding parameters - unused */
    cmd_ptr->bcmcs_rpt.transaction_id = 0;
    cmd_ptr->bcmcs_rpt.num_of_flows = 0;
    memset( cmd_ptr->bcmcs_rpt.flow_status_array,
            0,
            HDRBCCPI_BCMCS_MAX_FLOWS * sizeof( sys_bcmcs_flow_status_element_s_type ) );
    
    HDR_MSG_PROT (MSG_LEGACY_MED,  "Sent CM_HDR_BCMCS_BROADCAST_RESET_F rpt");

    hdrbccp_cm_mc_rpt( cmd_ptr );

  }
  else
  {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

} /* hdrbccp_send_broadcast_reset_rpt_to_cm() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_SEND_BROADCAST_DISCONTINUE_RPT_TO_CM

DESCRIPTION
  This function sends a Broadcast Discontinue Report to CM as an ACK to CM's
  BroadcastDiscontinue command.

DEPENDENCIES
  None.

PARAMETERS
  transaction_id - Transaction ID of the discontinue report given by CM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
STATIC void hdrbccp_send_broadcast_discontinue_rpt_to_cm
( 
  uint8 transaction_id
)
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    cmd_ptr->bcmcs_rpt.hdr.cmd = CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F;

    /* HDR Broadcast */
    cmd_ptr->bcmcs_rpt.protocol = SYS_SYS_MODE_HDR;

    /* Last Transaction ID included in the */
    cmd_ptr->bcmcs_rpt.transaction_id = transaction_id;

    /* Initialize the remainding parameters - unused */
    cmd_ptr->bcmcs_rpt.num_of_flows = 0;
    memset( cmd_ptr->bcmcs_rpt.flow_status_array,
            0,
            HDRBCCPI_BCMCS_MAX_FLOWS * sizeof( sys_bcmcs_flow_status_element_s_type ) );
    
    HDR_MSG_PROT (MSG_LEGACY_MED, 
                  "Sent CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F rpt");

    hdrbccp_cm_mc_rpt( cmd_ptr );

  }
  else
  {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

} /* hdrbccp_send_broadcast_discontinue_rpt_to_cm() */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/*===========================================================================

FUNCTION HDRBCCP_SEND_FLOW_MONITORING_STATUS_TO_CM

DESCRIPTION
  This function sends a Flow Monitoring report to CM.

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
STATIC void hdrbccp_send_flow_monitoring_status_to_cm
( 
  boolean flows_are_monitored
)
{

  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    if (flows_are_monitored)
    {
      cmd_ptr->bcmcs_flow_monitoring_rpt.hdr.cmd = CM_HDR_BCMCS_FLOW_MONITORING_START_F;
      HDR_MSG_PROT (MSG_LEGACY_MED,  
                    "Sent CM_HDR_BCMCS_FLOW_MONITORING_START_F rpt");
    }
    else
    {
      cmd_ptr->bcmcs_flow_monitoring_rpt.hdr.cmd = CM_HDR_BCMCS_FLOW_MONITORING_END_F;
      HDR_MSG_PROT (MSG_LEGACY_MED,  
                    "Sent CM_HDR_BCMCS_FLOW_MONITORING_END_F rpt");
    }
    
    hdrbccp_cm_mc_rpt( cmd_ptr );

  }
  else
  {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

} /* hdrbccp_send_flow_monitoring_status_to_cm() */

/* <EJECT> */ 
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
void hdrbccp_update_flow_monitoring_status
( 
  boolean flows_are_monitored
)
{
  /* Update SD */
  if (flows_are_monitored)
  {
    (void) sd_ss_ind_hdr_opr_start_activity( NULL, SD_SS_HDR_ACT_MASK_BCMCS );
  }
  else
  {
    (void) sd_ss_ind_hdr_opr_stop_activity( NULL, SD_SS_HDR_ACT_MASK_BCMCS );
  }

  /* Update HDR searcher */
  hdrlmac_update_bc_info(flows_are_monitored);

  /* Update Call Manager */
  hdrbccp_send_flow_monitoring_status_to_cm(flows_are_monitored);

} /* hdrbccp_update_flow_monitoring_status() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_DATA_CALLBACK

DESCRIPTION
  Callback function for handling the data interface for broadcast packets.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbccp_data_callback( dsm_item_type *data_pkt)
{
#ifdef FEATURE_DS
  /* Send to data stack */
  (void) dsbc_1xhdr_put_pkt ( &data_pkt,
                       NULL,
                       NULL );
#endif /* FEATURE_DS //DA */

} /* hdrbccp_data_callback() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_ARE_ANY_FLOWS_MONITORED 

DESCRIPTION
   This function returns TRUE if it finds at least one flow in Monitored state.
  
DEPENDENCIES
  None.

PARAMETERS
  
RETURN VALUE
  TRUE  - At least one flow is in DESIRED MONITORED state
  FALSE - No active flows

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrbccp_are_any_flows_monitored()
{

  return ( (hdrbccp_flow_info.mon_flow_cnt == 0) ? FALSE : TRUE );

} /* hdrbccp_are_any_flows_monitored() */

/* <EJECT> */
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
boolean hdrbccp_are_any_flows_requested( void )
{

  return ( (q_cnt(&hdrbccp_flow_info.flow_des_priority_q) > 0) ? TRUE : FALSE);

} /* hdrbccp_are_any_flows_requested() */
/* <EJECT> */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to BROADCAST OVHD.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
)
{
  /*-------------------------------------------------------------------------
  If the ind callback called from within the same task, process it
  immediately, else queue it & tag it for the overhead protocol.
  -------------------------------------------------------------------------*/
  if (rex_self() == HDRBCCP_TASK_PTR)
  {
    hdrbccp_process_ind( ind_name, (hdrind_ind_data_union_type*)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_BROADCAST_PROTOCOL, ind_name,
                     ind_data_ptr );
  }
} /* hdrbccp_ind_cb() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRBCCP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to the Broadcast Protocol

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
                         
SIDE EFFECTS
  None.
===========================================================================*/
void hdrbccp_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  hdrbccp_msg_id_type msg_id; /* Message ID field buffer. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( HDRBCCP_UNPACK_MSG_ID( msg_ptr, &msg_id ) != E_SUCCESS )
  {
    ERR("Message ID extraction failed.", 0, 0, 0);
    dsm_free_packet( &msg_ptr );
  }
  else if (!hdrbccp.broadcast_available)
  {
    HDR_MSG_PROT (MSG_LEGACY_MED, 
                  "Message for BCCP ignored; broadcast not negotiated.");
    dsm_free_packet( &msg_ptr );
  }
  else
  {
    switch ( msg_id )
    {
      case HDRBCCP_OVHD_MSG:
        hdrmc_queue_msg( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL, 
                         HDRHAI_IN_USE, msg_ptr );
        break;

      default:
        hdrmc_queue_msg( HDRHAI_BROADCAST_PROTOCOL, HDRHAI_IN_USE, msg_ptr );
        break;
    }
  }

                    
} /* hdrbccp_msg_cb() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRBCCP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  config message to the Broadcast Protocol

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void hdrbccp_config_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  hdrmc_queue_msg( HDRHAI_BROADCAST_PROTOCOL,
                   HDRHAI_IN_CONFIG, msg_ptr );
} /* hdrbccp_config_msg_cb() */
/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRBCCP_DELETE_FLOW_ENTRY

DESCRIPTION
  This function deletes a flow entry from the flow table 

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void hdrbccp_delete_flow_entry
(
  hdrbccp_flow_buf_type * q_buf_ptr
)
{
  
  hdrbccpfs_change_state(q_buf_ptr, HDRBCCPFS_NULL);

} /* hdrbccp_delete_flow_entry() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_PURGE_OLD_UNDES_FLOWS

DESCRIPTION
  This function removes old undesired flows from the flow table.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None                                            

SIDE EFFECTS
  None

===========================================================================*/
STATIC void hdrbccp_purge_old_undes_flows(void)
{
  
  hdrbccp_flow_buf_type * q_buf_ptr = NULL;
  hdrbccp_flow_buf_type * q_next_buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_LOW,"Purge old undesired flows if needed");

  /*-------------------------------------------------------------------------
  Search the Undesired flow list .
  -------------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check( &hdrbccp_flow_info.flow_undes_priority_q );

  while ( q_buf_ptr != NULL )
  { 
    q_next_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_undes_priority_q, 
                         &q_buf_ptr->link );
    
    if ( hdrbccpreg_flow_is_purgeable(q_buf_ptr) )
    {
      hdrbccp_delete_flow_entry(q_buf_ptr);
    }

    q_buf_ptr = q_next_buf_ptr;
  }

} /* hdrbccp_purge_old_undes_flows() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_FLOW_ID_ALREADY_REQUESTED

DESCRIPTION
  This function determines whether a particular flow has already been 
  requested (and hence is already in the desired flow list)
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id  - Unique Flow ID
    
RETURN VALUE
  TRUE  - FlowID already exists in the desired flow list
  FALSE - FlowID not found in the desired flow list
  
SIDE EFFECTS
  None

===========================================================================*/
STATIC boolean hdrbccp_flow_id_already_requested
(
  uint32 flow_id
)
{
  hdrbccp_flow_buf_type * q_buf_ptr = NULL; /* flow queue iterator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Search the Desired flow list to see if the flow already exists  
  -------------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check( &hdrbccp_flow_info.flow_des_priority_q );

  while ( q_buf_ptr != NULL )
  {
    if (q_buf_ptr->flow_info.flow_id == flow_id)
    {
      return TRUE;
    }

    q_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                         &q_buf_ptr->link );
  }

  return FALSE;

} /* hdrbccp_flow_id_already_requested() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_ADD_NEW_DES_FLOW

DESCRIPTION
  This function adds a desired flow entry to the flow table 

DEPENDENCIES
  None.

PARAMETERS
  flow_info_ptr - flow related info
  data_pkt - to register data packet routing function
  priority - priority for this flow. For flows of equal priority, the oldest
             flow takes precedence.
  
RETURN VALUE
  E_SUCCESS - Flow entry added successfully
  E_FAILURE - Queue insertion error, unable to add new flow

SIDE EFFECTS
  None

===========================================================================*/
STATIC void hdrbccp_add_new_des_flow
(
  hdrbccp_flow_type *flow_info_ptr,
  void (*data_cb)(dsm_item_type * data_pkt),
  uint8                    priority
)
{
  hdrbccp_flow_buf_type * q_buf_ptr = NULL; /* flow queue iterator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_3(MSG_LEGACY_MED,
                 "Add new desired flow (%d), ip:n(port) (%u:n(%d))",
                 flow_info_ptr->flow_id,
                 flow_info_ptr->ip_port.ip_address.addr.v4,
                 flow_info_ptr->ip_port.port_number);

  /*-------------------------------------------------------------------------
  Search the Undesired flow list to see if this flow had recently become
  undesired and is desired again.
  -------------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check( &hdrbccp_flow_info.flow_undes_priority_q );

  while ( q_buf_ptr != NULL )
  { 
    if ( (q_buf_ptr->flow_info.ip_port.ip_address.addr.v4 == 
           flow_info_ptr->ip_port.ip_address.addr.v4) && 
         (q_buf_ptr->flow_info.ip_port.port_number == 
           flow_info_ptr->ip_port.port_number) &&
         (q_buf_ptr->flow_info.flow_id == flow_info_ptr->flow_id) )
    {
      /* Found it! */
      q_buf_ptr->priority = priority;
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
      break;
    }

    q_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_undes_priority_q, 
                         &q_buf_ptr->link );
  }

  /*-------------------------------------------------------------------------
  Create a new flow entry: No UndesiredFlow found with this address
  -------------------------------------------------------------------------*/
  if (q_buf_ptr == NULL)
  {
    /* We are adding a new flow.  It's a good time to clean up old flows
       from the undesired flow list if needed. */
    hdrbccp_purge_old_undes_flows();

    /*------------------------------------------------------------------------- 
       If there are too many flows on the undesired queue, 
       remove the oldest one.  It should never get to this.  
       Adjust the total flow size if it does. 
       
       We do this here instead of hdrbccpfs so we wouldn't have to initiate
       a state transition within a state transition processing if the 
       undes flow list is maxed out.  
    -------------------------------------------------------------------------*/
    if ( q_cnt(&hdrbccp_flow_info.flow_free_q) == 0 )
    {
      /* Max desired entries is guaranteed. Remove the oldest Undesired flow 
         entry (tail entry) and use it for the new Desired one.  */
      q_buf_ptr = (hdrbccp_flow_buf_type*) 
                  q_last_check(&hdrbccp_flow_info.flow_undes_priority_q);

      if (q_buf_ptr != NULL)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                       "Too many undesired flows, remove flow %d",
                       q_buf_ptr->flow_info.flow_id);
        hdrbccp_delete_flow_entry(q_buf_ptr);
      }
      else
      {
        /* Should never happen as the size of the des flow q is bound */
        ERR_FATAL("No free buffers undes OR free flow q!",0,0,0);
      }
    }

    /* The flow doesn't exist on any of the queues, get a new buffer from
       the free queue to use to transition to DESIRED_UNAVAILABLE state. */
    q_buf_ptr = hdrbccpfs_get_new_flow_ptr();

    /* Populate the flow info fields */
    q_buf_ptr->flow_info = *(flow_info_ptr);
    q_buf_ptr->data_cb = data_cb;

    qw_set(q_buf_ptr->last_mon_time, 0, 0);
    qw_set(q_buf_ptr->last_reg_time, 0, 0);

    q_buf_ptr->state = HDRBCCPFS_NULL;
    q_buf_ptr->priority = priority;
#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

  }

  /*-------------------------------------------------------------------------
     Since this is a new flow, it won't be parsed in bcovhd yet even if it is
     already contained in the bcovhd message.  We always transition through
     DESIRED_UNAVAILABLE first because of this.
     After new flows are added, BC OVHD is notified of the change in desired
     flow list. It reparses its bc ovhd info and sends a bc ovhd info updated
     indication back to BCCP.  The newly requested flows will be enabled at 
     that point if available on the current frequency. Registration is done
     at that time too.
  -------------------------------------------------------------------------*/
  hdrbccpfs_change_state(q_buf_ptr, HDRBCCPFS_DESIRED_UNAVAILABLE);


} /* hdrbccp_add_new_des_flow() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCCP_IS_FLOW_AVAILABLE_IN_CURRENT_FREQUENCY

DESCRIPTION
  Determines whether the flow is available on the current frequency.

  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - Flow ID of the flow to look up


RETURN VALUE
  TRUE  - The flow is available on the current frequency
  FALSE - The flow isn't available on the current frequency or 
          it isn't in the bc ovhd message.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrbccp_is_flow_available_in_curr_freq
(
  sys_bcmcs_flow_id_type flow_id
)
{
#ifdef FEATURE_HDR_BCMCS_MFRQ
  uint8 phys_chan_count = 0; /* Physical channel count for given FlowID */
  boolean ret_val = FALSE;   /* Return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd_get_phys_chan_count(flow_id, &phys_chan_count) == 
      E_SUCCESS)
  {
    ret_val = (phys_chan_count == 0)? FALSE : TRUE;
  }

  HDR_MSG_PROT_2(MSG_LEGACY_MED,
                 "Flow %d is available in current frequency (%d)",flow_id,ret_val);

  return ret_val;
#else
  return TRUE;
#endif /* FEATURE_HDR_BCMCS_MFRQ */
} /* hdrbccp_is_flow_available_in_curr_freq() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCCP_GET_DESIRED_FLOW_LIST

DESCRIPTION
  This function returns the list of desired flows.
  This function can only be called from hdrbcovhd.c.
  
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id_list_ptr       - Pointer to array of flow_ids of desired flows
                           to be filled in by this function
  max_flow_id_list_size  - Maximum size of flow_id_list

RETURN VALUE
  Number of desired flows found.
  List and count of desired flows returned through parameters
  flow_id_list_ptr and flow_count.

SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrbccp_get_desired_flow_list
(
  hdrbcovhd_flow_id_type *flow_id_list_ptr,
  uint8 max_flow_id_list_size
)
{
  uint8 flow_count = 0;
  uint8 max_count  = 0; /* number of flows found */
  
  hdrbccp_flow_buf_type * q_buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  max_count = q_cnt(&hdrbccp_flow_info.flow_des_priority_q);

  if (max_count > max_flow_id_list_size)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "Desired Flow Cnt greater than ovhd flow list (%d > %d)",
                   max_count, max_flow_id_list_size);
    max_count = max_flow_id_list_size;
  }

  /* Walk through the desired flow list to extract the new set of desired 
     flow IDs */
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check( &hdrbccp_flow_info.flow_des_priority_q );

  while ( (flow_count != max_count) && (q_buf_ptr != NULL) )
  { 
    flow_id_list_ptr[flow_count].flow_id = q_buf_ptr->flow_info.flow_id;
    flow_id_list_ptr[flow_count].flow_discrim_len = 
      q_buf_ptr->flow_info.flow_discrim_len;

    HDR_MSG_PROT_2(MSG_LEGACY_MED,"Desired flow[%d] - %d ",
                   flow_count,q_buf_ptr->flow_info.flow_id);

    flow_count++;
    q_buf_ptr = (hdrbccp_flow_buf_type *)
                q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                        &q_buf_ptr->link );
  }

  return (flow_count);

} /* hdrbccp_get_desired_flow_list() */

/* <EJECT> */
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
)
{
  switch ( (hdrbccp_timer_enum_type) timer_id )
  {
    case HDRBCCP_REG_TIMER_0:
    case HDRBCCP_REG_TIMER_1:
    case HDRBCCP_REG_TIMER_2:
      /*-----------------------------------------------------------------------
      Broadcast Registration Timer: 
      -----------------------------------------------------------------------*/      
      hdrbccpreg_process_reg_timer_exp(timer_id);
    break;
    
    default:
      /*-----------------------------------------------------------------------
      Unrecognized Timer: 
      -----------------------------------------------------------------------*/      
      ERR("Invalid Timer ID (%d)",timer_id,0,0);

    break;
  }
} /* hdrbccp_process_timer() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_REJECT_MSG_FORMAT_0

DESCRIPTION
  This function unpacks the Broadcast Reject message for format 0.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  reject_msg_ptr - Location into which to extract the received message
  offset       - Offset in message where format 1 specific information starts

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
STATIC hdrerrno_enum_type hdrbccp_unpack_reject_msg_format_0
(
  dsm_item_type *msg_ptr,
  hdrbccp_reject_msg_type *reject_msg_ptr,
  uint16 offset
)
{
  int err = 0;   /* Error Value (0 = E_SUCCESS) */
  uint8 cnt = 0; /* Index counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
    Flow Count = Number of Flows Included 
  ---------------------------------------------------------------------------*/
  err |= (int)HDRBCCP_REJECT_FLOW_COUNT(msg_ptr, 
                                        &reject_msg_ptr->flow_count,
                                        offset,
                                        HDRBCCP_REJECT_FLOW_COUNT_SIZE);

  offset += HDRBCCP_REJECT_FLOW_COUNT_SIZE;

  /* The AN shall not set this field to zero. */
  if (reject_msg_ptr->flow_count == 0)
  {
    ERR("BCMCSFlowCount shall not be set to 0, ignore Reject msg.",0,0,0);
    return E_DATA_INVALID;
  }
  else if (reject_msg_ptr->flow_count > HDRBCCPI_BCMCS_MAX_FLOWS)
  {
    ERR( "BCMCSFlowCount: %d, only support less than %d.",
         reject_msg_ptr->flow_count, HDRBCCPI_BCMCS_MAX_FLOWS, 0 );
    return E_DATA_INVALID;
  }

  for (cnt=0; cnt < reject_msg_ptr->flow_count; cnt++)
  {
    /*-------------------------------------------------------------------------
      Flow ID 
    -------------------------------------------------------------------------*/
    err |= (int)HDRBCCP_REJECT_FLOW_ID(
                  msg_ptr, 
                  (uint32*)&reject_msg_ptr->flow[cnt].flow_id,
                  offset,
                  reject_msg_ptr->flow_id_length_in_bits);

    offset += reject_msg_ptr->flow_id_length_in_bits;

    /*-------------------------------------------------------------------------
      Reject Reason Same As Previous BCMCS Flow 
    -------------------------------------------------------------------------*/
    err |= (int)HDRBCCP_REJECT_SAME_AS_PREV_FLOW(
                  msg_ptr,
                  &reject_msg_ptr->flow[cnt].same_as_prev_flow,
                  offset,
                  HDRBCCP_REJECT_SAME_AS_PREV_FLOW_SIZE);

    offset += HDRBCCP_REJECT_SAME_AS_PREV_FLOW_SIZE;

    /* Reject Reason is not the same as the previous flow */
    if (reject_msg_ptr->flow[cnt].same_as_prev_flow == 0)
    {          
      /*-----------------------------------------------------------------------
        Reject Reason 
      -----------------------------------------------------------------------*/
      err |= (int)HDRBCCP_REJECT_REASON(
                    msg_ptr,
                    (uint8 *)&(reject_msg_ptr->flow[cnt].reject_reason),
                    offset,
                    HDRBCCP_REJECT_REASON_SIZE );

      offset += HDRBCCP_REJECT_REASON_SIZE; 
    }  
    else
    {
      if (cnt == 0)
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR,
          "RejectMsg: same_as_prev_flow!=0 not allowed for first flow");
        return E_DATA_INVALID;
      }
      else
      {
        reject_msg_ptr->flow[cnt].reject_reason = 
          reject_msg_ptr->flow[cnt - 1].reject_reason;
      }
    }

    HDR_MSG_PROT_3(MSG_LEGACY_MED,
                   "Reject0[%d] - Flow ID(%d), Reason(%d)", 
                   cnt, reject_msg_ptr->flow[cnt].flow_id,
                   reject_msg_ptr->flow[cnt].reject_reason);

  }

  return (hdrerrno_enum_type) ((err == 0) ? E_SUCCESS : E_NO_DATA ) ;

} /* hdrbccp_unpack_reject_msg_format_0() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_REJECT_MSG_FORMAT_1

DESCRIPTION
  This function unpacks the Broadcast Reject message for format 1.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  reject_msg_ptr - Location into which to extract the received message
  offset       - Offset in message where format 1 specific information starts

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
STATIC hdrerrno_enum_type hdrbccp_unpack_reject_msg_format_1
(
  dsm_item_type *msg_ptr,
  hdrbccp_reject_msg_type *reject_msg_ptr,
  uint16 offset
)
{
  int err = 0;        /* Error Value (0 = E_SUCCESS) */

  /* Variables to hold key values specific to format 1 */
  uint8 program_count;
  uint8 program_id_lsb_length;
  uint32 program_id_lsbs;
  uint8 flow_discriminator_count;
  uint8 flow_discriminator_length;
  uint8 flow_discriminator;

  uint8 pidx; /* Program ID Index - iterator */
  uint8 fidx; /* Flow count iterator */

  uint8 *flow_cnt; /* Convenience pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(reject_msg_ptr != NULL);

  /* Initialize the flow count.  
     If a program does not have any flow discriminators, the program is counted 
     as a flow. All requested flows associated with this program ID will have
     a 'rejected' status. */ 
  flow_cnt = &reject_msg_ptr->flow_count;
  *flow_cnt = 0;
  
  /*-------------------------------------------------------------------------
    ProgramCount
  -------------------------------------------------------------------------*/
  err |= (int) HDRBCCP_REJECT_PROGRAM_COUNT( 
                 msg_ptr, 
                 &program_count,
                 offset,
                 HDRBCCP_REJECT_PROGRAM_COUNT_SIZE );

  offset += HDRBCCP_REJECT_PROGRAM_COUNT_SIZE;

  if (err != 0){ return E_NO_DATA; }

  for ( pidx = 0; pidx < program_count; pidx++ )
  {
    if (*flow_cnt >= HDRBCCPI_BCMCS_MAX_FLOWS)
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
        "RejectMsg: flow_cnt %d reached max",
        *flow_cnt);
      return E_DATA_INVALID;
    }
    /*-------------------------------------------------------------------------
      ProgramIDLSBLength
    -------------------------------------------------------------------------*/
    err |= (int) HDRBCCP_REJECT_PID_LSB_LENGTH( 
                   msg_ptr, 
                   &program_id_lsb_length, 
                   offset,
                   HDRBCCP_REJECT_PROGRAM_ID_LSB_LENGTH_SIZE );

    offset += HDRBCCP_REJECT_PROGRAM_ID_LSB_LENGTH_SIZE;

    if (err != 0){ return E_NO_DATA; }

    /*-------------------------------------------------------------------------
      ProgramIDLSBs
    -------------------------------------------------------------------------*/
    err |= (int) HDRBCCP_REJECT_PID_LSBS( msg_ptr, 
                                          &program_id_lsbs, 
                                          offset,
                                          program_id_lsb_length );
    offset += program_id_lsb_length;

    /*-------------------------------------------------------------------------
      FlowDiscriminatorCount
    -------------------------------------------------------------------------*/
    err |= (int)HDRBCCP_REJECT_FLOW_COUNT(msg_ptr, 
                                          &flow_discriminator_count,
                                          offset,
                                          HDRBCCP_REJECT_FLOW_COUNT_SIZE);

    offset += HDRBCCP_REJECT_FLOW_COUNT_SIZE;

    if (err != 0){ return E_NO_DATA; }

    /* No flow discriminators associated with this program ID. 
       Rejecting the entire program. */
    if ( flow_discriminator_count == 0 )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
        "flow_disc_cnt=0, all flows in this program will be rejected" );

      /*-------------------------------------------------------------------------
        Reject Reason Same As Previous BCMCS Flow 
      -------------------------------------------------------------------------*/
      err |= (int)HDRBCCP_REJECT_SAME_AS_PREV_FLOW(
        msg_ptr,
        &reject_msg_ptr->flow[*flow_cnt].same_as_prev_flow,
        offset,
        HDRBCCP_REJECT_SAME_AS_PREV_FLOW_SIZE );

      offset += HDRBCCP_REJECT_SAME_AS_PREV_FLOW_SIZE;

      if (err != 0){ return E_NO_DATA; }

      /* Reject Reason is not the same as the previous flow */
      if (reject_msg_ptr->flow[*flow_cnt].same_as_prev_flow == 0)
      {          
        /*-----------------------------------------------------------------------
          Reject Reason 
        -----------------------------------------------------------------------*/
        err |= (int)HDRBCCP_REJECT_REASON(
                      msg_ptr,
                      (uint8 *)&(reject_msg_ptr->flow[*flow_cnt].reject_reason),
                      offset,
                      HDRBCCP_REJECT_REASON_SIZE);

        offset += HDRBCCP_REJECT_REASON_SIZE; 
      }
      else /* Program reject reason same as previous */
      {
        if (*flow_cnt == 0)
        {
          HDR_MSG_PROT(MSG_LEGACY_ERROR,
            "RejectMsg: same_as_prev_flow!=0 not allowed for first flow");
          return E_DATA_INVALID;
        }
        else
        {
          /* Store the previous program's reject reason */
          reject_msg_ptr->flow[*flow_cnt].reject_reason = 
            reject_msg_ptr->flow[(*flow_cnt) - 1].reject_reason;
        }
      }

      /* We still store a BCMCS Flow ID even if there is no flow discriminator */
      reject_msg_ptr->flow[*flow_cnt].flow_id = 
        hdrbcovhd_create_bcmcs_flow_id( reject_msg_ptr->flow_id_length_in_bits,
                                        0,
                                        0,
                                        program_id_lsbs );

      HDR_MSG_PROT_3(MSG_LEGACY_MED,
                     "Reject1[%d] - Program(%d), Reason(%d)", 
                     *flow_cnt, reject_msg_ptr->flow[*flow_cnt].flow_id,
                     reject_msg_ptr->flow[*flow_cnt].reject_reason);

      /* Count the Program as a flow - all requested flows associated with this
         pid will be in reject state. */
      (*flow_cnt)++;

    }
    else /* flow discriminator count != 0 */
    {
      /*-----------------------------------------------------------------------
        FlowDiscriminatorLength
      -----------------------------------------------------------------------*/
      err |= HDRBCCP_REJECT_FLOW_DISCRIM_LENGTH( 
               msg_ptr, 
               &flow_discriminator_length,
               offset,  
               HDRBCCP_REJECT_FLOW_DISCRIMINATOR_LENGTH_SIZE );

      offset += HDRBCCP_REJECT_FLOW_DISCRIMINATOR_LENGTH_SIZE;

      if (err != 0){ return E_NO_DATA; }

      if ( flow_discriminator_length == 0 )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
          "FlowDiscriminatorLength = 0 in Reject msg! Ignore program." );
        continue;
      }

      for ( fidx = 0; fidx < flow_discriminator_count; fidx++ )
      {
        if (*flow_cnt >= HDRBCCPI_BCMCS_MAX_FLOWS)
        {
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
            "RejectMsg: flow_cnt %d reached max",
            *flow_cnt);
          return E_DATA_INVALID;
        }

        /*---------------------------------------------------------------------
          FlowDiscriminator
        ---------------------------------------------------------------------*/
        err |= (int) HDRBCCP_REJECT_FLOW_DISCRIMINATOR( msg_ptr, 
                       &flow_discriminator,
                       offset,  
                       flow_discriminator_length );

        offset += flow_discriminator_length;

        if (err != 0){ return E_NO_DATA; }    

        /* Create BCMCS Flow ID */
        reject_msg_ptr->flow[*flow_cnt].flow_id = 
          hdrbcovhd_create_bcmcs_flow_id( reject_msg_ptr->flow_id_length_in_bits,
                                          flow_discriminator_length,
                                          flow_discriminator,
                                          program_id_lsbs );

        /*---------------------------------------------------------------------
          Reject Reason Same As Previous BCMCS Flow 
        ---------------------------------------------------------------------*/
        err |= (int)HDRBCCP_REJECT_SAME_AS_PREV_FLOW(
                    msg_ptr,
                    &reject_msg_ptr->flow[*flow_cnt].same_as_prev_flow,
                    offset,
                    HDRBCCP_REJECT_SAME_AS_PREV_FLOW_SIZE );

        offset += HDRBCCP_REJECT_SAME_AS_PREV_FLOW_SIZE;

        if (err != 0){ return E_NO_DATA; }

        /* Reject Reason is not the same as the previous flow */
        if (reject_msg_ptr->flow[*flow_cnt].same_as_prev_flow == 0)
        {          
          /*-------------------------------------------------------------------
            Reject Reason 
          -------------------------------------------------------------------*/
          err |= (int)HDRBCCP_REJECT_REASON(
            msg_ptr,
            (uint8 *)&(reject_msg_ptr->flow[*flow_cnt].reject_reason),
            offset,
            HDRBCCP_REJECT_REASON_SIZE );

          offset += HDRBCCP_REJECT_REASON_SIZE;
        }
        else /* Reject reason same as previous flow */
        {
          if (*flow_cnt == 0)
          {
            HDR_MSG_PROT(MSG_LEGACY_ERROR,
              "RejectMsg: same_as_prev_flow!=0 not allowed for first flow");
            return E_DATA_INVALID;
          }
          else
          {
            reject_msg_ptr->flow[*flow_cnt].reject_reason = 
              reject_msg_ptr->flow[(*flow_cnt) - 1].reject_reason;
          }
        }

        HDR_MSG_PROT_3(MSG_LEGACY_MED,
                       "Reject1[%d] - Flow ID(%d), Reason(%d)", 
                       *flow_cnt, reject_msg_ptr->flow[*flow_cnt].flow_id,
                       reject_msg_ptr->flow[*flow_cnt].reject_reason);

        (*flow_cnt)++;
      } /* for (flow discriminator count) */

    } /* else (flow discriminator count != 0) */
  } /* for (program index) */

  return (hdrerrno_enum_type) ((err == 0) ? E_SUCCESS : E_NO_DATA ) ;

} /* hdrbccp_unpack_reject_msg_format_1() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_REJECT_MSG

DESCRIPTION
  This function unpacks the Broadcast Reject message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  reject_msg_ptr - Location into which to extract the received message

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
STATIC hdrerrno_enum_type hdrbccp_unpack_reject_msg
(
  dsm_item_type *msg_ptr,
  hdrbccp_reject_msg_type *reject_msg_ptr
)
{
  int err = 0;    /* Error Value */
  errno_enum_type return_errno = E_SUCCESS; /* Value to be returned */

  uint16 offset;  /* Offset(bits) of field from msg start */
  uint8 flow_id_len = 0; /* Flow ID length in octets */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  offset = HDRBCCP_REJECT_MSG_ID_SIZE;

  /*===========================================================================
    Flow Reject Msg Preamble
  ===========================================================================*/
  /*---------------------------------------------------------------------------
    Flow Format 
  ---------------------------------------------------------------------------*/
  err |= (int)HDRBCCP_REJECT_FLOW_FMT(msg_ptr, 
                                      &reject_msg_ptr->flow_format);
  offset += HDRBCCP_REJECT_FLOW_FMT_SIZE;
     
  /*---------------------------------------------------------------------------
    Flow ID Length 
    - One less than the length of the BCMCS Flow identifier in octets.  
    - Shall no be '00'.
  ---------------------------------------------------------------------------*/
  err |= (int)HDRBCCP_REJECT_FLOW_ID_LGTH(msg_ptr, 
                                          &flow_id_len);
  offset += HDRBCCP_REJECT_FLOW_ID_LGTH_SIZE;

  if (flow_id_len == 0)
  {
    ERR("BCMCSFlowIDLength shall not be set to '00', ignore Reject msg.",0,0,0);
    return E_DATA_INVALID;
  }
  else
  {
    /* Store the flow ID length in bits */
    reject_msg_ptr->flow_id_length_in_bits = 
      HDRBCCP_REJECT_FLOW_ID_SIZE(flow_id_len);
  }

  if (err != 0)
  {    
    /* Errors unpacking the fields, no need to continue. */
    return_errno = E_NO_DATA;
  }
  else if (reject_msg_ptr->flow_format == 0)
  {
    /*-------------------------------------------------------------------------
      Unpack Flow Format 0
    -------------------------------------------------------------------------*/
    return_errno = 
      hdrbccp_unpack_reject_msg_format_0(msg_ptr, reject_msg_ptr, offset);
  }
  else /* if (reject_msg_ptr->flow_format == 1) */
  {
    /*-------------------------------------------------------------------------
      Unpack Flow Format 1
    -------------------------------------------------------------------------*/
    return_errno = 
      hdrbccp_unpack_reject_msg_format_1(msg_ptr, reject_msg_ptr, offset);
  }

  return (return_errno) ;

} /* hdrbccp_unpack_reject_msg() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_REJECT_MSG

DESCRIPTION
  This function processes the Broadcast Reject message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
STATIC void hdrbccp_process_reject_msg
(
  dsm_item_type *item_ptr
)
{
  hdrbccp_reject_msg_type reject_msg;  /* Broadcast Reject Message struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrbccp_unpack_reject_msg(item_ptr, &reject_msg) != E_SUCCESS )
  {
    ERR("Error unpacking Broadcast Reject msg.  Ignoring it.",0,0,0);
    return;
  }

  hdrbccp_send_flow_status_to_cm_2( HDRBCCP_BC_REJECTED, &reject_msg );
  
} /* hdrbccp_process_reject_msg() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_MSG

DESCRIPTION
  This function processes messages for BCAST.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrbccp_process_msg
(
  dsm_item_type * msg_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
)
{

  uint8 msg_id;           /* Message identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG)); 

  /*---------------------------------------------------------------------------
  Extract the first byte of the received message. This is the message ID.
  ---------------------------------------------------------------------------*/
  if ( HDRBCCP_UNPACK_MSG_ID( msg_ptr, &msg_id ) != E_SUCCESS )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Could not extract msg_id");
    dsm_free_packet( &msg_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_BROADCAST_PROTOCOL, msg_id, hdrbccps_get_state());
  
  /*---------------------------------------------------------------------------
  IN-USE Instance of the Broadcast protocol
  ---------------------------------------------------------------------------*/
  if ( protocol_instance == HDRHAI_IN_USE )
  {
     /*------------------------------------------------------------------------
     Determine which InUse message has been received: GENERIC BROADCAST subtype
     ------------------------------------------------------------------------*/
    if ( hdrscp_get_current_subtype(HDRHAI_BROADCAST_PROTOCOL)
         == HDRSCP_GENERIC_BCMCS )
    {
      switch ( msg_id )
      {
        
        case HDRBCCP_SEC_PARAMS_MSG:
          /*------------------------------------------------------------------
              Security Parameters Message
           ------------------------------------------------------------------*/
          HDR_MSG_PROT(MSG_LEGACY_ERROR,
                       "Security Parameters message not supported");
          break;

        case HDRBCCP_RESET_MSG:
          /*------------------------------------------------------------------
              Broadcast Reset Message
           ------------------------------------------------------------------*/

          /* Notify CM/DS */
          hdrbccp_send_broadcast_reset_rpt_to_cm();

          break;

        case HDRBCCP_REJECT_MSG:
          /*------------------------------------------------------------------
              Broadcast Reject Message
           ------------------------------------------------------------------*/
          hdrbccp_process_reject_msg( msg_ptr );
          break;

        default:
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                         "Unrecognized message received 0x%x", msg_id);
          break;
      } /* switch */
    }
    else /* DEFAULT Broadcast Protocol 0x00 */
    {
      HDR_MSG_PROT_1(MSG_LEGACY_LOW, 
                     "Unknown msg for default BCMCS (0x%x)",msg_id);
    }

  }
    
  /*---------------------------------------------------------------------------
   Finished processing message. Free the dsm item containing the message.
  ---------------------------------------------------------------------------*/
  dsm_free_packet( &msg_ptr );

} /* hdrbccp_process_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCCP_CLEAR_DSDB_CACHE

DESCRIPTION
  This helper function clears the dsdb cache.
  
DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void hdrbccp_clear_dsdb_cache( void )
{

   uint32 handle;
#ifdef FEATURE_HDR_BCMCS_2p0
   #error code not present
#endif 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   HDR_MSG_PROT(MSG_LEGACY_MED,"Clear db query cache");

   handle = hdrbccp.db_cache.handle;

   memset((void*)&hdrbccp.db_cache, 0, 
          sizeof(hdrbccp_db_lookup_cache_type));

   hdrbccp.db_cache.handle = handle;

#ifdef FEATURE_HDR_BCMCS_2p0
   #error code not present
#endif

} /* hdrbccp_clear_dsdb_cache() */ 

/* <EJECT> */

/*===========================================================================

FUNCTION HDRBCCP_CHECK_DSDB_LOOKUP_COMPLETION

DESCRIPTION
  This function 

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void hdrbccp_check_dsdb_lookup_completion ( void )
{

  uint8 i; 
    /* iterator */

  hdrbccp_flow_buf_type *f_buf_ptr = NULL;
    /* Flow Info Ptr */

  hdrbccp_flow_buf_type *f_next_buf_ptr = NULL;
    /* Pointer to next flow buffer in the queue */

  hdrbccp_db_lookup_cache_entry_type *c_ptr = NULL; 
    /* Convenience cache pointer */

  boolean desired_list_changed = FALSE;
    /* Keeps track of whether the new requested list has changed */

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif

  uint8 curr_des_flow_cnt = 0;

  boolean flow_entry_found = FALSE;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We only finish processing the current FlowRequest when all the db 
     lookups are completed. */
  if (hdrbccp.db_cache.num_addr_pending_lookup > 0)
  {
    return;
  }

  hdrbccp.db_cache.db_query_in_progress = FALSE;

  curr_des_flow_cnt = q_cnt(&hdrbccp_flow_info.flow_des_priority_q);

  /* All the lookups are done, finish processing the flow request command */

  /*---------------------------------------------------------------------------  
  1) If BCCP did a db lookup in response to a DS FlowRequest, send a 
     FlowRequestSuccess Report immediately to CM with the current status for  
     each flow.  The report is sent immediately so CM will get a reply in a 
     timely matter.  
     
     After the desired flow list is updated, we will send a FlowUpdate report 
     to CM if any of the flow statuses have changed again. 
  ---------------------------------------------------------------------------*/
  if (hdrbccp.db_cache.query_type == HDRBCCP_DB_FLOW_REQ_QUERY)
  {
    hdrbccp_send_flow_req_success_rpt_to_cm();
  }

  /* Update the flow list (both desired and undesired - 2 pass process)*/

  /*---------------------------------------------------------------------------  
  2a) Look at the desired flows and check to see if any of them are no longer 
      desired.
  ---------------------------------------------------------------------------*/

  /* Point to the head of the queue */
  f_buf_ptr = (hdrbccp_flow_buf_type *)
                q_check( &hdrbccp_flow_info.flow_des_priority_q );

  while ( f_buf_ptr != NULL )
  { 
    f_next_buf_ptr = (hdrbccp_flow_buf_type *)
                      q_next( &hdrbccp_flow_info.flow_des_priority_q,
                              &f_buf_ptr->link);
    
    flow_entry_found = FALSE;

    /* Check to see if it is a port 0 request. */
    //port0_request = (f_buf_ptr->ip_port.port_number == 0) ? TRUE : FALSE;

    /* Search through the entries in the cached request list to see if a flow
       in the current flow table is already desired */
    for (i=0; i < hdrbccp.db_cache.total_flows; i++)
    {
      c_ptr = &hdrbccp.db_cache.flow_req_list[i];

      /* Check for a matching entry (ip:port# -> FlowID) */
      if ( !(c_ptr->flow_already_requested) &&
           (f_buf_ptr->flow_info.ip_port.ip_address.addr.v4 == 
              c_ptr->flow_info.ip_port.ip_address.addr.v4) &&
           (f_buf_ptr->flow_info.ip_port.port_number == 
              c_ptr->flow_info.ip_port.port_number) )
      {

        /* An entry is matched in the temp ds flow request cache if either:
        
           - the db lookup was successful and the ip:port# -> FlowID mapping is
             still the same (it may only be different if we have moved to a new
             subnet, the mapping is guaranteed to be unique within a subnet).
             _mpa_ If it is a format 1 flow, we might want to make sure that we
             check the format info (make sure that those fields haven't changed 
             in the ds flow request cache vs flow table).
             
           - or the db lookup was un-successful (db busy, etc): ds will send 
             another FlowRequest (Desired flow state will remain unchanged).
             
             _mpa_ potential issue: if we have a subnet change scenario where the
             ip:port#->FlowID mapping changes, then it is possible that we could 
             have 2 flows with the same FlowID if a db lookup failed because the
             db was busy for example.  Perhaps hold off until we get a request 
             that would result in all successes to enable/disable flows?
        */
        if ( (c_ptr->lookup_success && 
              (f_buf_ptr->flow_info.flow_id == c_ptr->flow_info.flow_id) ) ||
             (!c_ptr->lookup_success) )
        {
            flow_entry_found = TRUE;
            c_ptr->flow_already_requested = TRUE;
#ifdef FEATURE_HDR_BCMCS_2p0
            #error code not present
#endif
            break;
        }

        /* we keep searching in case of a port0 request.  Could look explicitely
           for port#0 as well. */
      }
    }

    /* If a currently desired flow is no longer requested (not found in the cache),
       then the flow becomes undesired.  If it was monitored, it is disabled.  */
    if ( !flow_entry_found )
    {
      desired_list_changed = TRUE;

      hdrbccpfs_change_state( f_buf_ptr, HDRBCCPFS_UNDESIRED );

    }
        
    /* Update the flow buffer pointer to point to the next entry in the queue.  We
       don't call q_next on f_buf_ptr in case the desired entry was removed from 
       it's current location in the queue because the flow is no longer desired */
    f_buf_ptr = f_next_buf_ptr;

  } /* end while */

  /*---------------------------------------------------------------------------  
   2b) Second pass: Now that all the flows that are no longer desired have been
     removed from the desired flow list, search through the cached flow request
     list to find any newly desired flows.  Note that all the flows that are 
     already present in the desired flow list have been marked as "already 
     requested" in the requested flow list cache, so we just need to look at the
     remaining entries.
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#else /* #ifndef FEATURE_HDR_BCMCS_2p0 */
  for (i=0; i < hdrbccp.db_cache.total_flows; i++)
  {
    c_ptr = &hdrbccp.db_cache.flow_req_list[i];

    /* Found a new flow, add it to the desired flow list. */
    if ( !c_ptr->flow_already_requested && c_ptr-> lookup_success )
    {
      desired_list_changed = TRUE;

      /* The flows in the flow request list are in priority order.
         Pass in the index as the priority order.  Additional priorities can
         be decided upon based on the reference flow and the greatest number
         of requested flows that may be decoded on a particular channel */
      hdrbccp_add_new_des_flow( &(c_ptr->flow_info), 
                                hdrbccp_data_callback, 
                                i
                              );
    }
  }
#endif /* FEATURE_HDR_BCMCS_2p0 */

  /* Notify hdrbcovhd if our list of requested flows has changed. */
  if ( desired_list_changed )
  {
    /* We didn't have any desired flows before this request.  Notify searcher
       that it needs to go into dual slotted sleep mode. */
    if ( (curr_des_flow_cnt == 0) &&
         (q_cnt(&hdrbccp_flow_info.flow_des_priority_q) > 0) )
    {
      hdrlmac_set_bcmcs_page_cycle(hdrbccpc_get_curr_bc_ovhd_period());
    }
    
    /* Could happen if we get a flow request with flows that are not in the
       dsdb - won't be in the desired flow list in this case. 
      _mpa_ do we really need this? Search should have already been notified */
    if (q_cnt(&hdrbccp_flow_info.flow_des_priority_q) == 0)
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,"Current desired flow count is 0");
      hdrlmac_set_bcmcs_page_cycle(0);
    }

    hdrovhd_reparse_bc_msg( HDRHAI_BROADCAST_PROTOCOL );

    /* We don't register for new flows (if any) quite yet.  
       Wait for bcovhd to reparse its bc msg first to see if the flow exists.
       If it does it will transition immediately out of UNAVAILABLE 
       state into MONITORED or UNMONITORED state. 
       Registration is done accordingly upon processing a BC OVHD udpated
       indication.
     */
  }

} /* hdrbccp_check_dsdb_lookup_completion() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_DSBCDB_LOOKUP_CB

DESCRIPTION
  db lookup completion callback

DEPENDENCIES
  None

PARAMETERS
  handle    - db lookup handle (one per query)
  flow_list - if lookup successful, includes list of ip/port addresses
              matching this lookup.
  err_number     - db lookup completion status

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbccp_dsbcdb_lookup_cb
(
  dsbcdb_cmd_handle_type handle,
  const dsbcdb_flow_list_type *flow_list, /* The callback function should
                                              make a copy of this list. The
                                              passed list struct would no
                                              longer be valid after the
                                              callback function returns.
                                           */
  sint15 err_number
)
{
  hdrbccp_cmd_type hdrbccp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Only process a DS bcdb lookup cb in OPEN state. The AT will re-issue
     the query with a different handle when it returns to OPEN state, so it
     is okay to discard. */
  if (hdrbccps_get_state() == HDRBCCPS_CLOSE_STATE)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED,
                   "Ignore dsbcdb lookup cb for handle %d (BCCP CLOSED)",
                   handle);
    return;
  }

  /* Fill in all members of the hdrbccp_cmd_type for this command */
  hdrbccp_cmd.name = HDRBCCP_DB_LOOKUP_COMPLETED_CMD;
  hdrbccp_cmd.sender = HDRHAI_BROADCAST_PROTOCOL;  
  hdrbccp_cmd.params.db_lookup.handle = handle;
  hdrbccp_cmd.params.db_lookup.err_number = err_number;

  memcpy (&hdrbccp_cmd.params.db_lookup.flow_list, 
          flow_list, 
          sizeof(dsbcdb_flow_list_type));

  if (rex_self() == HDRBCCP_TASK_PTR)
  {
    hdrbccp_process_cmd( &hdrbccp_cmd );
  }
  else
  {
    hdrmc_queue_cmd( HDRHAI_BROADCAST_PROTOCOL, &hdrbccp_cmd, 
                     sizeof(hdrbccp_cmd_type));
  }


} /* hdrbccp_dsbcdb_lookup_cb() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_PROCESS_DSBCDB_LOOKUP_CB

DESCRIPTION
  This function processes a dsbcdb lookup callback command

DEPENDENCIES
  None

PARAMETERS
  hdrbccp_cmd - hdrbccp command payload

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbccp_process_dsbcdb_lookup_cb 
(
  hdrbccp_cmd_type *hdrbccp_cmd
)
{
  uint8 i;  /* iterator */

  hdrbccp_db_lookup_cache_entry_type *flow_entry_ptr = NULL; 
  dsbcdb_flow_list_type *flow_list_ptr = NULL; 
    /* Convenience pointers */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  flow_list_ptr = &hdrbccp_cmd->params.db_lookup.flow_list;

  ASSERT(flow_list_ptr);
  ASSERT(flow_list_ptr->num_flows <= DSBCDB_MAX_FLOWS_PER_IP);

  /* Find the db lookup cache entry corresponding to this callback. */
     
  for (i=0; i < hdrbccp.db_cache.req_flows; i++)
  {

    if ( hdrbccp.db_cache.flow_req_list[i].db_query_handle == 
         hdrbccp_cmd->params.db_lookup.handle )
    {
      /* IP for this handle and the cache should always match.  We don't
         check for the port# here in case the query was not successful
         (port# not returned in this case - will check for it in the
          case statement later.) */
      ASSERT( hdrbccp.db_cache.flow_req_list[i].flow_info.ip_port.ip_address.addr.v4 ==
              flow_list_ptr->multicast_ip.addr.v4 );

#ifdef FEATURE_HDR_BCCP_DEBUG      
      HDR_MSG_PROT_5(MSG_LEGACY_HIGH,"h%d ip_cache:%d:%d, ip_cmd:%d:%d",
                     hdrbccp_cmd->params.db_lookup.handle,
                     hdrbccp.db_cache.flow_req_list[i].flow_info.ip_port.ip_address.addr.v4,
                     hdrbccp.db_cache.flow_req_list[i].flow_info.ip_port.port_number,
                     flow_list_ptr->multicast_ip.addr.v4,
                     flow_list_ptr->multicast_ip.port_number);
#endif /* FEATURE_HDR_BCCP_DEBUG */
      flow_entry_ptr = &hdrbccp.db_cache.flow_req_list[i];
      break;
    }

  }

  /* If there is no matching handle in the cache, then this is an outstanding  
     callback from a previously cancelled Flow Request.  We just ignore it. */
  if ( flow_entry_ptr == NULL )
  {
     HDR_MSG_PROT_1( MSG_LEGACY_MED,
                     "Outstanding db lookup cb for handle (%d) - discard", 
                     hdrbccp_cmd->params.db_lookup.handle );
     
     return;
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED,
                    "hdrbccp_dsbcdb_lookup_cb for handle (%d), result (%d)",
                    hdrbccp_cmd->params.db_lookup.handle,
                    hdrbccp_cmd->params.db_lookup.err_number);
  }

  /* The assumption is that each cb is only called once for every lookup. */
  if (hdrbccp.db_cache.num_addr_pending_lookup > 0)
  {
    hdrbccp.db_cache.num_addr_pending_lookup--;
  }
  else
  {
    ERR("Not expecting any more db lookup callbacks", 0, 0, 0);
  }

  /* Process db lookup result */
  switch ( (errno_enum_type)hdrbccp_cmd->params.db_lookup.err_number )
  {
    /*-------------------------------------------------------------------------
      ** Successful db lookup **
    -------------------------------------------------------------------------*/

    case E_SUCCESS:
    case E_BUF_OVERFLOW:

      ASSERT( (flow_entry_ptr->flow_info.ip_port.port_number == 0) ||
              ( flow_entry_ptr->flow_info.ip_port.port_number == 
                flow_list_ptr->flow_info[0].port )
            );

#ifdef FEATURE_HDR_BCCP_DEBUG
        HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                     "Success but cache_port:%d cb_port:%d",
                     flow_entry_ptr->flow_info.ip_port.port_number,
                     flow_list_ptr->flow_info[0].port);
#endif /* FEATURE_HDR_BCCP_DEBUG */

      flow_entry_ptr->status = SYS_BCMCS_FLOW_STATUS_REQUESTED;
      flow_entry_ptr->lookup_success = TRUE;

      /* Copy the flow_id info for this ip/port# address */
      flow_entry_ptr->flow_info.flow_id = 
        flow_list_ptr->flow_info[0].flow_id.hdr_flow_id;
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
      flow_entry_ptr->flow_info.flow_discrim_len = 
        flow_list_ptr->flow_info[0].flow_discrim_len;
      flow_entry_ptr->flow_info.flow_discrim = 
        flow_list_ptr->flow_info[0].flow_discrim;
      flow_entry_ptr->flow_info.program_id_len = 
        flow_list_ptr->flow_info[0].program_id_len;
      flow_entry_ptr->flow_info.program_id = 
        flow_list_ptr->flow_info[0].program_id;
      flow_entry_ptr->flow_info.flow_format = 
        flow_list_ptr->flow_info[0].flow_format;
                        
      /* An IP:Port 0 address may have more than one flow associated with it */
      if (flow_entry_ptr->flow_info.ip_port.port_number == 0)
      {
        flow_entry_ptr->flow_info.ip_port.port_number = flow_list_ptr->flow_info[0].port;


        HDR_MSG_PROT_4(MSG_LEGACY_MED,"port0 lookup: [%d] id(%d) ip(0x%x) port(%d)",
                       i, flow_entry_ptr->flow_info.flow_id,
                       flow_entry_ptr->flow_info.ip_port.ip_address.addr.v4,
                       flow_entry_ptr->flow_info.ip_port.port_number);

        /* The total number of requested flow (including port 0 requests) 
           should never exceed HDRBCCPI_BCMCS_MAX_FLOWS.  This should already be
           guaranteed by CM/DS at the time of request */
        ASSERT ( (hdrbccp.db_cache.total_flows + flow_list_ptr->num_flows) 
                 <= HDRBCCPI_BCMCS_MAX_FLOWS );

        /* Create new entries at the end of the cache for those additional 
           flows in the db cache list. */
        for (i=1; 
             i < flow_list_ptr->num_flows;
             i++)
        {          
          flow_entry_ptr = &hdrbccp.db_cache.flow_req_list[hdrbccp.db_cache.total_flows];

          flow_entry_ptr->status = SYS_BCMCS_FLOW_STATUS_REQUESTED; 
          flow_entry_ptr->lookup_success = TRUE;

          flow_entry_ptr->flow_info.flow_id = 
            flow_list_ptr->flow_info[i].flow_id.hdr_flow_id;
#ifdef FEATURE_HDR_BCMCS_2p0
          #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

           flow_entry_ptr->flow_info.ip_port.ip_address.addr.v4 = 
             flow_list_ptr->multicast_ip.addr.v4;

           flow_entry_ptr->flow_info.ip_port.port_number =
             flow_list_ptr->flow_info[i].port;

           /* _mpa_ I don't think this was right - flow_entry_ptr not set again... check for phase-2 */
           flow_entry_ptr->flow_info.flow_discrim_len = 
             flow_list_ptr->flow_info[i].flow_discrim_len;
           flow_entry_ptr->flow_info.flow_discrim = 
             flow_list_ptr->flow_info[i].flow_discrim;
           flow_entry_ptr->flow_info.program_id_len = 
             flow_list_ptr->flow_info[i].program_id_len;
           flow_entry_ptr->flow_info.program_id = 
             flow_list_ptr->flow_info[i].program_id;
           flow_entry_ptr->flow_info.flow_format = 
             flow_list_ptr->flow_info[i].flow_format;

           hdrbccp.db_cache.total_flows++;

           HDR_MSG_PROT_4(MSG_LEGACY_MED,"port0 lookup: [%d] id(%d) ip(0x%x) port(%d)",
                          i, flow_list_ptr->flow_info[i].flow_id.hdr_flow_id,
                          flow_list_ptr->multicast_ip.addr.v4,
                          flow_list_ptr->flow_info[i].port);
        }
#ifdef FEATURE_HDR_BCMCS_2p0
        #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
        HDR_MSG_PROT_1(MSG_LEGACY_MED,"Port#0 lookup, maps to %d flows", i);
      }

    break;

    /*-------------------------------------------------------------------------
      ** Un-successful db lookup **
      
      If a db lookup fails, we are unable to map an IP/Port# address to a 
      FlowID.  In this case, the flow request is not added to the BCCP flow 
      request list as it would be unable to register for this flow.  
      The lookup_success field will remain unset (FALSE).
      
      Depending on the failure reason, DS might choose to remove those flows from 
      a its Flow Request list and send another FlowRequestUpdate command.
       
    -------------------------------------------------------------------------*/

    /* _mpa_ Right now, we don't check to see if the flow associated with this 
       failed lookup is already in the bccp desired flow list. 
       If it was, then we need to handle this at least for the E_FLOW_NOT_IN_ZONE 
       and E_ZONE_NOT_IN_DB cases (could occur on Subnet handoff).  
       Keep it around for registration purposes if we come back to that subnet
       but which state should it be in - flowID will become invalid?   
      
       
       For an E_DB_INVALID case, we should probably 
       leave it alone.  DS should be sending another FlowRequestUpdate when it 
       sees that the db was unavailable for queries at that time. */

    case E_FLOW_NOT_IN_ZONE:
      
      flow_entry_ptr->status = 
        SYS_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP;

    break;

    case E_ZONE_NOT_IN_DB:

      flow_entry_ptr->status = SYS_BCMCS_FLOW_STATUS_NO_MAPPING;

    break;

    case E_DB_INVALID:
      /* _mpa_ We should have some kind of retry mechanism or make sure that
         data sends another flow request when the db becomes available again
         and it had just received a response with UNAVAILABLE status. */
      flow_entry_ptr->status = SYS_BCMCS_FLOW_STATUS_DB_UNAVAILABLE;
    break;

    default:
      ERR("Unknown db status %d",hdrbccp_cmd->params.db_lookup.err_number,0,0);
      flow_entry_ptr->status = SYS_BCMCS_FLOW_STATUS_CANCELLED;
    break;
  }

  /* Check to see if all the db lookups are completed for this Flow Request.
     Flow Request Cmd handling is only performed after all the lookups are 
     done. */
  hdrbccp_check_dsdb_lookup_completion();

} /* hdrbccp_process_dsbcdb_lookup_cb() */

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
void hdrbccp_closing_reset_dsdb_query( void )
{

  uint8 index; /* Loop index */
  hdrbccp_db_lookup_cache_entry_type * cache_ptr; /* Convenience ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is no db query in progress, nothing to do. */
  if (!hdrbccp.db_cache.db_query_in_progress)
  {
    return;
  }

  HDR_MSG_PROT_1(MSG_LEGACY_MED,
                 "Cancel dsdb query in progress for TID %d",
                 hdrbccp.db_cache.transaction_id);

  for (index = 0; index < hdrbccp.db_cache.req_flows; index++)
  {
    cache_ptr = &hdrbccp.db_cache.flow_req_list[index];

    cache_ptr->status = SYS_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE;
    cache_ptr->lookup_success = FALSE; 
    cache_ptr->flow_info.flow_id = 0;

    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,"DB query postponed for IP:0x%x, port#:%d",
      cache_ptr->flow_info.ip_port.ip_address.addr.v4,
      cache_ptr->flow_info.ip_port.port_number);
  }

  hdrbccp.db_cache.total_flows = hdrbccp.db_cache.req_flows;
  hdrbccp.db_cache.num_addr_pending_lookup = hdrbccp.db_cache.req_flows;

  /* Send a Flow Request success report to CM (ack) with UNAVAILABLE status 
  for each flow. */
  hdrbccp_send_flow_req_success_rpt_to_cm();

} /* hdrbccp_closing_reset_dsdb_query() */

/*===========================================================================

FUNCTION HDRBCCP_QUERY_DSDB

DESCRIPTION
  This helper function requests handles for each flow in the dsdb lookup
  cache.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void hdrbccp_query_dsdb( void )
{

  uint8 index; /* Loop index */
  dsbcmcs_zone_type zone; /* Zone info for the DS flow info cmd */
  hdrbccp_db_lookup_cache_entry_type * cache_ptr; /* Convenience ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  zone.type = DSBCMCS_ZONE_HDR;

  if ( hdrbccpreg_get_current_subnet(zone.zone.subnet) == E_NO_DATA )
  {
    ERR("Couldn't retrieve subnet info - use 0",0,0,0);
    memset(zone.zone.subnet,0,sizeof(zone.zone.subnet));
  }

  hdrbccp.db_cache.db_query_in_progress = TRUE;

  for (index = 0; index < hdrbccp.db_cache.req_flows; index++)
  {
    cache_ptr = &hdrbccp.db_cache.flow_req_list[index];

    /* hdrbccp allocates a handle for this db query.  It will be used
       as a transaction id to match the response in the callback */
    hdrbccp.db_cache.handle++;

    /* Skip over DSBCDB_INVALID_CMD_HANDLE and wrap around */
    if (hdrbccp.db_cache.handle == DSBCDB_INVALID_CMD_HANDLE)
    {
      hdrbccp.db_cache.handle = 0;
    }

    /* Lookup the database for this flow */
    cache_ptr->db_query_handle = dsbcdb_post_get_flow_info_cmd( 
                                   hdrbccp.db_cache.handle, 
                                   zone,
                                   cache_ptr->flow_info.ip_port.ip_address,
                                   cache_ptr->flow_info.ip_port.port_number,
                                   hdrbccp_dsbcdb_lookup_cb
                                 );

    cache_ptr->lookup_success = FALSE;
    cache_ptr->flow_already_requested = FALSE;

#ifdef FEATURE_HDR_BCCP_DEBUG
    HDR_MSG_PROT_9(MSG_LEGACY_HIGH,"queried w/ dsdb handle:%d IP:0x%x port%d zone:%x:%x:%x:%x:%x:%x",
                   cache_ptr->db_query_handle,
                   cache_ptr->flow_info.ip_port.ip_address.addr.v4,
                   cache_ptr->flow_info.ip_port.port_number,
                   zone.zone.subnet[5]zone.zone.subnet[4],zone.zone.subnet[3],
                   zone.zone.subnet[2],zone.zone.subnet[1],zone.zone.subnet[0]);
#endif /* FEATURE_HDR_BCCP_DEBUG */

    if ( cache_ptr->db_query_handle == 
           DSBCDB_INVALID_CMD_HANDLE )
    {
       ERR("Invalid handle for IP %d:Port %d db lookup",
           cache_ptr->flow_info.ip_port.ip_address.addr.v4,
           cache_ptr->flow_info.ip_port.port_number,0);

       cache_ptr->status = SYS_BCMCS_FLOW_STATUS_DB_UNAVAILABLE;

       if (hdrbccp.db_cache.num_addr_pending_lookup > 0)
       {
         hdrbccp.db_cache.num_addr_pending_lookup--;
       }
       else
       {
         ERR("Not expecting any more db lookup callbacks", 0, 0, 0);
       }

    }
    else
    {
      cache_ptr->status = SYS_BCMCS_FLOW_STATUS_NONE;
    }

  }

  /* Check to see if all the db lookups are completed for this desired flow 
     list. Changes to the flow list are only handled after all the lookups 
     are completed. */
  hdrbccp_check_dsdb_lookup_completion();

} /* hdrbccp_query_dsdb() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_SUBNET_CHANGED

DESCRIPTION
  This function requeries the dsdb for every flow because the subnet has 
  changed.  

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/

STATIC void hdrbccp_process_subnet_changed( void )
{
  
  hdrbccp_flow_buf_type *des_flow_ptr = NULL; /* Desired flow pointer */

  uint8 cache_idx = 0; /* DSDB cache index */
  hdrbccp_db_lookup_cache_entry_type *cache_ptr = NULL; /* Convenience ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* hack for now - unfinished functionality: double check that this is never
     called */

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_suppress 111
#endif
  /*lint --e{527} suppress unreachable code until hakc is removed */
  return;

  /* We only do a db lookup for desired flows.  Return if there aren't any. */
  if ( q_cnt(&hdrbccp_flow_info.flow_des_priority_q) == 0)
  {
    return;
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Subnet changed - update desired flow list");

  /* If we were in the middle of a db lookup, re-do the db queries
     with the new subnet info. (Pending lookups could be either for a DS flow
     request, or another Subnet change query).
          
     _mpa_ TODO: check for need to mutex
   */
  if ( hdrbccp.db_cache.num_addr_pending_lookup != 0 )
  {

    switch (hdrbccp.db_cache.query_type)
    {
      case HDRBCCP_DB_FLOW_REQ_QUERY:
      {
      /* Reset the number of pending lookups to the original number of 
         requested flows. */
          
      /* _mpa_ TODO: port0 additional flows are currently added at the end of 
         the cache (not sorted).  So the original requested flow addresses 
         are still in the first "req_flows" entries. These are the ones
         we re-query, the additional port0 ones are ignored. */
         hdrbccp.db_cache.num_addr_pending_lookup = hdrbccp.db_cache.req_flows;
         hdrbccp_query_dsdb();
      }                   
      break;

      case HDRBCCP_DB_SUBNET_CHANGE_QUERY:
      {
        /* We've switched subnets again before we could complete the db
           lookup on the current one.  We */

      }
      break;

      case HDRBCCP_DB_NO_QUERY:
      default:
        ERR("Invalid dsdb cache state",0,0,0);
      break;
    }
  }
  else
  {
    hdrbccp.db_cache.query_type = HDRBCCP_DB_SUBNET_CHANGE_QUERY;
    hdrbccp.db_cache.transaction_id = 0;

    hdrbccp.db_cache.req_flows = q_cnt(&hdrbccp_flow_info.flow_des_priority_q);
    hdrbccp.db_cache.num_addr_pending_lookup = hdrbccp.db_cache.req_flows;

    /* Copy the desired flow addresses into the dsdb cache. Do a db lookup
       on each flow to see if any of the ip:port# -> FlowID mappings have
       changed.  Update the flow statuses accordingly and send a FlowStatus
       report to CM/DS */

    des_flow_ptr = (hdrbccp_flow_buf_type*) 
                   q_check( &hdrbccp_flow_info.flow_des_priority_q );

    while (des_flow_ptr != NULL)
    {

      cache_ptr = &hdrbccp.db_cache.flow_req_list[cache_idx];

      memcpy(&cache_ptr->flow_info.ip_port,
             &des_flow_ptr->flow_info.ip_port,
             sizeof(sys_bcmcs_flow_addr_s_type) );

      cache_ptr->flow_info.flow_id = 0;
      cache_idx++;

      des_flow_ptr = (hdrbccp_flow_buf_type *)
                      q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                              &des_flow_ptr->link );

    }

    hdrbccp_query_dsdb();

  }

  /* _mpa_ do we really want to do this now?  Wait until db lookup completion 
           + see if any flows have a different mapping. */
  hdrbccpreg_async_flow_registration(TRUE);
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_default 111
#endif

} /* hdrbccp_process_subnet_changed() */

#ifdef FEATURE_HDR_BCMCS_MFRQ
/*=============================================================================
FUNCTION  HDRBCCP_FIND_BEST_BC_CHANNEL

DESCRIPTION
  Finds the best broadcast idle channel.  This is the channel on which the 
  reference flow and the greatest number of other desired flows are available.
  If more than one channel meets these criteria, the one listed first in the 
  BroadcastOverhead message is chosen. 
  
DEPENDENCIES
  The reference flow must be set before calling this function.

PARAMETERS
  ref_flow_ptr - Pointer to our reference flow
  curr_channel - The channel the AT is currently monitoring.

RETURN VALUE
  best_bc_channel - the best bc idle channel

SIDE EFFECTS
  None.
=============================================================================*/
sys_channel_type hdrbccp_find_best_bc_channel 
(
  hdrbccp_flow_buf_type * ref_flow_ptr,
  sys_channel_type curr_channel
)
{
  /* The best broadcast channel we should be on (max count of desired available
     flows on a given flow) */
  uint8 best_bc_count;
  sys_channel_type best_bc_channel;

  /* Keeps track of how many flows could be monitored on the channel we are 
     currently looking up */
  uint8 curr_count = 0;

  /* Additional CDMA channel info. */
  errno_enum_type err_number;
  uint8 additional_cdma_channel_count = 0;
  sys_hdr_channel_record_s_type 
    additional_cdma_channel[HDRBCOVHD_MAX_CDMA_CHANNELS];

  /* Iterators */
  uint8 i;
  hdrbccp_flow_buf_type *q_buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ref_flow_ptr != NULL);
  ASSERT( (ref_flow_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED) ||
          (ref_flow_ptr->state == HDRBCCPFS_DESIRED_MONITORED) );

  best_bc_channel = curr_channel;

  err_number = hdrbcovhd_get_additional_cdma_channel_list( 
            ref_flow_ptr->flow_info.flow_id,
            &additional_cdma_channel_count,
            additional_cdma_channel,
            HDRBCOVHD_MAX_CDMA_CHANNELS );

  ASSERT (err_number == E_SUCCESS);

  /* The number of monitored flows is currently our max number of flows that
     are monitored on the current channel.  We'll check to see if we can do 
     better by moving to a different channel. If the reference flow is not 
     monitored, then we must move to a different channel. */
  if (ref_flow_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED)
  {
     best_bc_count = 0;

     /* Ref. flow is not on the current channel, and there is only one additional
        CDMA channel: no need to do a look up - tune to this channel */
     if (additional_cdma_channel_count == 1)
     {
       return additional_cdma_channel[0].channel;
     }

  }
  else /* if HDRBCCPFS_DESIRED_MONITORED */
  {   
    /* The number of flows currently monitored on the current channel.  We 
       assume that flows advertised in the additional cdma channel list are
       setup correctly on the channel they advertise (i.e. flow can be 
       monitored on that hash channel) */
    best_bc_count   = hdrbccp_flow_info.mon_flow_cnt;

#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
  }

  if (additional_cdma_channel_count == 0)
  {
    if (ref_flow_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED)
    {
      /* This combination shouldn't be allowed.  If the flow is in ovhd but not
         on the current channel, then it'd better be listed on another channel.
         Just stay on the current frequency in this case. */
      HDR_MSG_PROT_1(MSG_LEGACY_FATAL,
         "Addl CDMA chan cnt == 0 (flow %d) but flow not on curr chan either!",
         ref_flow_ptr->flow_info.flow_id);
    }
    else
    {
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                     "Ref. flow %d is not available on other channels",
                     ref_flow_ptr->flow_info.flow_id);
    }

    return best_bc_channel;
  }

  /* Search through the additional CDMA channel list for the reference flow
     to determine if there is a better channel the AT should be tuning to. */
  for (i=0; i < additional_cdma_channel_count; i++)
  {
    q_buf_ptr = ref_flow_ptr;

    /* Start at the current reference flow, and look through the rest of the
       desired flow list.  We don't need to start from the beginning of the 
       desired flow list, as our reference flow is always the highest priority
       flow.  If flows are higher in the (sorted) desired list, they must not
       be available. */
    q_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                         &q_buf_ptr->link );

    curr_count = 1;

    while ( q_buf_ptr != NULL )
    {
      if (hdrbcovhd_is_channel_in_additional_cdma_channel_list( 
            q_buf_ptr->flow_info.flow_id,
            additional_cdma_channel[i].channel)
         )
      {
        curr_count++;
      }

      q_buf_ptr = (hdrbccp_flow_buf_type *)
                   q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                           &q_buf_ptr->link );
    }

    /* We've found a channel that could accomodate more flows, save it and
       keep looking for an even better one. */
    if ( (curr_count > best_bc_count)
#ifdef FEATURE_HDR_BCMCS_2p0
         #error code not present
#endif /*  FEATURE_HDR_BCMCS_2p0 */
       )
    {
      best_bc_count   = curr_count;
      best_bc_channel = additional_cdma_channel[i].channel;
    }
  }

  return best_bc_channel;

} /* hdrbccp_find_best_bc_channel() */

/*=============================================================================
FUNCTION  HDRBCCP_FIND_BEST_BC_FREQ

DESCRIPTION
   If no flows are currently monitored, and at least one flow is in desired
   available unmonitored state, then tune to the frequency of that unmonitored
   flow. 
  
DEPENDENCIES
  None.

PARAMETERS
  current_link_ptr - Current channel and pilot


RETURN VALUE
  TRUE  - AT found a better broadcast channel that the current one.
          AT needs to tune to there  
  FALSE - AT can stay on the current channel

SIDE EFFECTS
  None.
=============================================================================*/
static boolean hdrbccp_find_best_bc_freq
( 
  const sys_link_type       *current_link_ptr
)
{
  hdrbccp_flow_buf_type *q_buf_ptr = NULL;

  boolean flows_are_available = FALSE;
    /* Whether any of our requested flow are available in bcovhd. 
       (monitored or not) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbccps_get_state() != HDRBCCPS_OPEN_STATE)
  {
    ERR("HDRBC not in OPEN state",0,0,0);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
  Update our reference flow if necessary
  -----------------------------------------------------------------------*/

  q_buf_ptr = (hdrbccp_flow_buf_type*) 
               q_check(&hdrbccp_flow_info.flow_des_priority_q);

  while ( q_buf_ptr != NULL )
  {
    /* Find the first (oldest / highest priority) flow in the list that is
       listed in overhead - it will become our reference flow. */

    if ( ((q_buf_ptr->state == HDRBCCPFS_DESIRED_MONITORED) ||
          (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED)) )
    {

      flows_are_available = TRUE;

      /* The first flow in the requested flows list that is available is the
         reference flow (i.e. highest priority). */

      hdrbccp_flow_info.bc_hash_channel = 
        hdrbccp_find_best_bc_channel( q_buf_ptr, current_link_ptr->channel );

      if (hdrbccp_flow_info.ref_flow_ptr == NULL)
      {
        /* The current channel is the channel we are tuning away from in order
           to monitor BCMCS. */
        hdrbccp_flow_info.original_channel = current_link_ptr->channel;
        hdrbccp_flow_info.tune_back_to_original_channel_now = FALSE;
      }

      if ( hdrbccp_flow_info.ref_flow_ptr != q_buf_ptr )
      {
        hdrbccp_flow_info.ref_flow_ptr = q_buf_ptr;

        HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
                        "Reference Flow Update (FlowID %d, chan %d:%d)",
                        hdrbccp_flow_info.ref_flow_ptr->flow_info.flow_id,
                        hdrbccp_flow_info.bc_hash_channel.band,
                        hdrbccp_flow_info.bc_hash_channel.chan_num);
      }

      break;

    }

    q_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                         &q_buf_ptr->link );
  }

  /* If no flows are available, make sure that the reference flow is NULL. 
     If we had a reference flow, then the AT will tune when bcovhd reparses
     its flow request list: no need to tune here. */
  if (!flows_are_available)
  {
    if (hdrbccp_flow_info.ref_flow_ptr != NULL)
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH,"Reference Flow Update (NULL)");
      hdrbccp_flow_info.ref_flow_ptr = NULL;

      /* Our reference flow became unavailable.  We may need to tune back to the
         SP hash channel if it is different than the bc hash channel. */
      hdrbccp_flow_info.tune_back_to_original_channel_now = TRUE;
      return TRUE;
    }
  }
  else if ( !SYS_CHAN_CMP_EQUAL(hdrbccp_flow_info.bc_hash_channel, 
                                current_link_ptr->channel) )
  {

    ASSERT(hdrbccp_flow_info.ref_flow_ptr != NULL);

    /* If the last flow request was received during the current traffic
       connection and we found a better channel to tune to, then 
       close the connection. This will cause the AT to return to idle state and 
       tune to the best bc channel. The connection will be re-opened if 
       there is data to be sent.

       Note that if the current connection is opened for session negotiation or
       High Priority Traffic, we will not close the connection.  When the 
       session is reconfigured or the HPT call ends, we will get new ovhd and 
       can handle tuning to a new channel then.    
     */
    if ( hdrbccp_flow_info.exit_traffic_for_best_bc_channel &&
         ( 
           hdrrup_is_tc_assigned( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL ) ||
           (hdridles_get_state() == HDRIDLES_CONN_SETUP_STATE) 
         )
       )
    {
      if ( !hdrscp_is_configuring() &&
           (hdrrup_get_traffic_mode() != HDRSRCH_S1XHDR_HP_TC_MODE) )
      {
        HDR_MSG_PROT_1(MSG_LEGACY_MED,
                       "Last flow request cmd rxed in traffic, close conn and tune (bc_channel %d)",
                        hdrbccp_flow_info.bc_hash_channel.chan_num);
        hdralmp_close_connection(HDRHAI_ABORT_NORMAL_CLOSE,
                                 HDRHAI_BROADCAST_PROTOCOL);
      }
      else
      {
        HDR_MSG_PROT_1(MSG_LEGACY_MED,
           "Last flow request cmd rxed in traffic, ignore best bc channel %d (session or HPT traffic)",
           hdrbccp_flow_info.bc_hash_channel.chan_num);
      }
    }
    else
    {
      /* If not in traffic and the best bc channel is not our current channel,
         then we can tune. */
      return TRUE;
    }
  }

  return FALSE;

} /* hdrbccp_find_best_bc_freq() */
#endif /* FEATURE_HDR_BCMCS_MFRQ */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRBCCP_PROCESS_BC_OVHD_UPDATED_10

DESCRIPTION
  The Broadcast Overhead message has changed.  Update the flow table based on 
  the flows that were received in the Broadcast Overhead message.
  
DEPENDENCIES
  None.
  
PARAMETERS
  current_link_ptr - Current channel and pilot
  is_first_after_acq - Whether this is the first ovhd updated indication
                       after acquisition.

RETURN VALUE
  TRUE  - AT needs to tune to a new bc channel
  FALSE - AT can stay on current channel
  
SIDE EFFECTS
  This is called directly from ovhd - acts as a command.
===========================================================================*/
boolean hdrbccp_process_bc_ovhd_updated_10
(
  const sys_link_type       *current_link_ptr,
  boolean is_first_after_acq
)
{
  hdrbccp_flow_buf_type *q_buf_ptr = NULL;
  boolean flow_status_changed = FALSE; 
    /* To keep track of whether any of the flows changed status */
  uint8 flow_format; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbcovhd_get_flow_format(&flow_format) != E_SUCCESS)
  {
    flow_format = 0;
    ERR("Couldn't retrieve the BCMCS flow format from OVHD!",0,0,0);
  }

  /*-----------------------------------------------------------------------
  Go through the flow table and find out which flows were not received
  in the bcovhd message  
  -----------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type*) 
               q_check(&hdrbccp_flow_info.flow_des_priority_q);

  while ( q_buf_ptr != NULL )
  {        
    if ( hdrbcovhd_is_flow_available(q_buf_ptr->flow_info.flow_id) )
    {
      /* Flow is in bcovhd, and available in the current frequency.
         If it isn't already monitored, enable the flow. */
      if (hdrbccp_is_flow_available_in_curr_freq(q_buf_ptr->flow_info.flow_id))
      {

        if ( (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNAVAILABLE) ||
             (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED) )
        {      
          /* If for some reason, the bcdb and ovhd format don't agree, then 
             don't enable that flow (will remain in DESIRED_UNAVAILABLE state) */
          if ( (q_buf_ptr->flow_info.flow_format != flow_format ) &&
               (!hdrbccp_flow_info.test_flow_enabled) )
          {
            HDR_MSG_PROT(MSG_LEGACY_ERROR,"Flow format mismatch!");

            if ( q_buf_ptr->state != HDRBCCPFS_DESIRED_UNAVAILABLE )
            {
              hdrbccpfs_change_state( q_buf_ptr, HDRBCCPFS_DESIRED_UNAVAILABLE );
              flow_status_changed = TRUE;
            }
          }
          else
          {
            hdrbccpfs_change_state( q_buf_ptr, HDRBCCPFS_DESIRED_MONITORED );
            flow_status_changed = TRUE;
          }
        }
      }
      /* Flow is in bcovhd, but _not_ available on the current frequency.
         If it is currently monitored, disable the flow. */
      else
      {
        if (  (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNAVAILABLE) ||
              (q_buf_ptr->state == HDRBCCPFS_DESIRED_MONITORED) )
        {
          /* If for some reason, the bcdb and ovhd format don't agree, then 
             don't enable that flow (will remain in DESIRED_UNAVAILABLE state) */
          if ( q_buf_ptr->flow_info.flow_format != flow_format )
          {
            HDR_MSG_PROT(MSG_LEGACY_ERROR,"Flow format mismatch!");

            if ( q_buf_ptr->state != HDRBCCPFS_DESIRED_UNAVAILABLE )
            {
              hdrbccpfs_change_state( q_buf_ptr, HDRBCCPFS_DESIRED_UNAVAILABLE );
              flow_status_changed = TRUE;
            }
          }
          else
          {
            hdrbccpfs_change_state( q_buf_ptr, HDRBCCPFS_DESIRED_UNMONITORED );
            flow_status_changed = TRUE;
          }
        }
      }
    }
    else
    {
      /* Flow is _not_ in bcovhd. If it was previously available 
         (monitored or not), make it unavailable.*/
      if ( (q_buf_ptr->state == HDRBCCPFS_DESIRED_MONITORED) ||
           (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED) )
      {
        hdrbccpfs_change_state(q_buf_ptr, HDRBCCPFS_DESIRED_UNAVAILABLE );
        flow_status_changed = TRUE;
      }
    }

    q_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                         &q_buf_ptr->link );
  }

  hdrbccpreg_async_flow_registration(FALSE);
 
  /* Notify CM/DS if any of the flows changed status.
     DS is unaware of test flows, so there is no need to notify CM/DS
      of flow status changes for test flows. */
  if (flow_status_changed && !hdrbccp_flow_info.test_flow_enabled)
  {
    hdrbccp_send_flow_status_to_cm(HDRBCCP_BC_OVHD_UPDATED);
  }

#ifdef FEATURE_HDR_BCMCS_MFRQ

  /* Check if our current channel is the best channel to be on.
     If a better one is found, tune to that frequency. */
  return hdrbccp_find_best_bc_freq( current_link_ptr );
#else
  return FALSE; 
#endif /* FEATURE_HDR_BCMCS_MFRQ */

} /* hdrbccp_process_bc_ovhd_updated_10() */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */


/*===========================================================================
FUNCTION HDRBCCP_PROCESS_BC_OVHD_UPDATED

DESCRIPTION
  The Broadcast Overhead message has changed.  Update the flow table based on 
  the flows that were received in the Broadcast Overhead message.
  
DEPENDENCIES
  None.
  
PARAMETERS
  current_link_ptr - Current channel and pilot
  is_first_after_acq - Whether this is the first ovhd updated indication
                       after acquisition.

RETURN VALUE
  TRUE  - AT needs to tune to a new bc channel
  FALSE - AT can stay on current channel
  
SIDE EFFECTS
  This is called directly from ovhd - acts as a command.
===========================================================================*/
boolean hdrbccp_process_bc_ovhd_updated
(
  const sys_link_type       *current_link_ptr,
  boolean is_first_after_acq
)
{

  if ( !hdrbccp.broadcast_available ||
       ( hdrbccps_get_state() != HDRBCCPS_OPEN_STATE ) )
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Ignore bc ovhd updated");
    return FALSE;
  }

  /*--------------------------------------------------------------------------- 
     If there is already an entry in the cache for the current sector on
     first acquisition, then then process_bc_ovhd_updated is called before
     the ovhd updated indication is given in process_cache_entry().

     This causes the AT to reenable whatever flows it had saved when losing
     acquisition.  The new flow update is passed up to DS at that time. SD/CM/DS
     need to be aware of the broadcast availability before we try to enable
     any flows. We will also update the status on first acq upon receiving
     the Ovhd_Updated indication. 
  ---------------------------------------------------------------------------*/
  if ( is_first_after_acq )
  {
    (void)sd_ss_ind_misc_hdr_bcmcs_srv_status(
      hdrbccp.broadcast_available ? SYS_BCMCS_SRV_STATUS_SRV : SYS_BCMCS_SRV_STATUS_NO_SRV, 
      NULL);  

    HDR_MSG_PROT_1(MSG_LEGACY_MED,"Broadcast availability update (%d)",
                   hdrbccp.broadcast_available);

    /* We received a flow request while HDR was not acquired, proceed with
       the db lookup now. */
    if ( (hdrbccp.db_cache.num_addr_pending_lookup != 0) &&
         (hdrbccp.db_cache.db_query_in_progress == FALSE)
       )
    {
      hdrbccp_query_dsdb();
    }
  }

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#else
  return hdrbccp_process_bc_ovhd_updated_10(current_link_ptr, is_first_after_acq);
#endif /* FEATURE_HDR_BCMCS_2p0 */

} /* hdrbccp_process_bc_ovhd_updated() */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/* <EJECT> */ 
/*=============================================================================

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

=============================================================================*/
void hdrbccp_process_ind
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type *ind_data_ptr
)
{

  HDRTRACE_IND( HDRHAI_BROADCAST_PROTOCOL, ind_name, hdrbccps_get_state());

  /*---------------------------------------------------------------------------
  Determine which indication has been received.
  ---------------------------------------------------------------------------*/
  switch ( ind_name )
  {
    case HDRIND_SCM_SESSION_CHANGED:

      if ( (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_NORMAL) ||
           (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR) ||
           (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS)
         )
      {
        hdrbccp.broadcast_available = 
        (hdrscp_get_current_subtype(HDRHAI_BROADCAST_PROTOCOL) == 
         HDRSCP_GENERIC_BCMCS);

        /*---------------------------------------------------------------------
        Notify SD of the broadcast availability.
        ---------------------------------------------------------------------*/
        HDR_MSG_PROT_1(MSG_LEGACY_MED,"Broadcast srv status (%d)",
                       hdrbccp.broadcast_available);
        (void)sd_ss_ind_misc_hdr_bcmcs_srv_status(
          hdrbccp.broadcast_available ? 
          SYS_BCMCS_SRV_STATUS_SRV : SYS_BCMCS_SRV_STATUS_NO_SRV, 
          NULL);

        /*---------------------------------------------------------------------
        We only retrieve the current attributes if the session is opened.
        If the session is in closed state, we do not set the current parameters
        back to default.  This will allow us to continue the registration until
        the session is reopened.  At that time, if broadcast has become 
        unavailable (default broadcast negotiated), we will set the current
        attributes to their default value.
        ---------------------------------------------------------------------*/
        hdrbccpc_process_ind(ind_name,ind_data_ptr);

        /* Set the page cycle - BOM period might have changed because of
           session negotiation*/
        if ( q_cnt(&hdrbccp_flow_info.flow_des_priority_q) > 0 )
        {
          hdrlmac_set_bcmcs_page_cycle(hdrbccpc_get_curr_bc_ovhd_period());
        }
        else
        {
          hdrlmac_set_bcmcs_page_cycle(0);
        }

        /*---------------------------------------------------------------------
        The Broadcast protocol is enabled/disabled on acquisition/SystemLoss. 
        If the AT powers up on a system with a closed session, ALMP will not
        enable the broadcast protocol at that time because we don't know if bc
        is available yet (unavailable on powerup, unless we have session info).

        In this case, we need to enable broadcast when the session is opened if
        it is still in closed state.
        ---------------------------------------------------------------------*/
        if ( ( hdrbccp.broadcast_available )&&
             ( hdrbccps_get_state() == HDRBCCPS_CLOSE_STATE ) )
        {
          hdrbccp_activate(HDRHAI_BROADCAST_PROTOCOL);
        }
        else if ( !( hdrbccp.broadcast_available ) &&
              ( hdrbccps_get_state() == HDRBCCPS_OPEN_STATE) )
        {
          hdrbccp_deactivate(HDRHAI_BROADCAST_PROTOCOL);
        }

        /*---------------------------------------------------------------------
        We configure the registration every time the session is opened, even if
        bc is unavailable (default).  If broadcast was available in the previous
        session and it is no longer available in this one, then we need to stop
        the registration timer (paging and dynamic reg attributes now set to 0).
        ---------------------------------------------------------------------*/
        hdrbccpreg_configure_registration();
      }

    break;
  
    case HDRIND_OVHD_INFO_UPDATED:
      /* This is the first ovhd info updated indication after acquisition.
         Treat it like an lmac.acquired indication.
         Restore the flow list if needed and notify SD. */
      if (ind_data_ptr->ovhd_updated_is_first_after_acq)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_MED,"Broadcast srv status (%d)",
                       hdrbccp.broadcast_available);

        (void)sd_ss_ind_misc_hdr_bcmcs_srv_status(
          hdrbccp.broadcast_available ? SYS_BCMCS_SRV_STATUS_SRV : SYS_BCMCS_SRV_STATUS_NO_SRV, 
          NULL);  

        if ( (hdrbccp.db_cache.num_addr_pending_lookup != 0) &&
             (hdrbccp.db_cache.db_query_in_progress == FALSE)
           )
        {
          hdrbccp_query_dsdb();
        }

      }

      if (hdrbccp.broadcast_available)
      {
        /* If the subnet has changed, we need to update our flow registration 
           and query the database for new flow info. */
        if ( hdrbccpreg_subnet_changed() )
        {
          hdrbccp_process_subnet_changed();
        }

        /* Set up the registration timer.  

           The timer is set up upon receiving the first ovhd info after 
           acquisition instead of upon receiving a network acquired 
           indication because the registration timers are tied to a particular
           subnet.  So we wait until we have current ovhd to start the timer 
           for our subnet.  */
        if (ind_data_ptr->ovhd_updated_is_first_after_acq)
        {
          hdrbccpreg_configure_registration();
        }
      }

    break;

    case HDRIND_ALMP_RETURNING_TO_IDLE:
    case HDRIND_IDLE_MONITOR_CONNECTION_FAILED:
      /* Reset the flag when we the current connection is closed or when the
         connection setup fails. */
      hdrbccp_flow_info.exit_traffic_for_best_bc_channel = FALSE;
    break;

    default:
      /*-----------------------------------------------------------------------
      Default: Indication not recognized.
      -----------------------------------------------------------------------*/
      ERR("BCCP: Unrecognized Indication: %d", ind_name, 0, 0 );
      break;
  }

}  /* hdrbccp_process_ind() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCCP_ACTIVATE

DESCRIPTION
   This function commands BCCP to activate 
     
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol sending the command
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_activate
(
  hdrhai_protocol_name_enum_type sender
)
{

  hdrbccp_cmd_type hdrbccp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_LOW, "hdrbccp_activate");

  /* Fill in all members of the hdrbccp_cmd_type for this command */
  hdrbccp_cmd.name = HDRBCCP_ACTIVATE_CMD;
  hdrbccp_cmd.sender = sender;

  if (rex_self() == HDRBCCP_TASK_PTR)
  {
    hdrbccp_process_cmd( &hdrbccp_cmd );
  }
  else
  {
    ERR("Activate Cmd from outside HDRMC",0,0,0);
    hdrmc_queue_cmd( HDRHAI_BROADCAST_PROTOCOL, &hdrbccp_cmd, 
                     sizeof(hdrbccp_cmd_type));
  }

} /* hdrbccp_activate() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_ACTIVATE_CMD

DESCRIPTION
  This function processes an activate command given to the BCCP protocol 
        
DEPENDENCIES
  None.

PARAMETERS
  bccp_cmd_ptr - Pointer to the command for BCCP to process
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_process_activate_cmd
(
  hdrbccp_cmd_type * bccp_cmd_ptr 
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  If the Protocol has already been activated, do nothing.
  ---------------------------------------------------------------------------*/
  if (hdrbccps_get_state() == HDRBCCPS_OPEN_STATE)
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_BROADCAST_PROTOCOL,
        bccp_cmd_ptr->name , bccp_cmd_ptr->sender, hdrbccps_get_state() );
    return;
  }

  /*---------------------------------------------------------------------------
  If the generic broadcast protocol isn't negotiated, ignore the cmd.
  ---------------------------------------------------------------------------*/
  if (!hdrbccp.broadcast_available)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Can't activate BCCP: default bcmcs negotiated");
    return;
  }

  hdrbccps_change_state(HDRBCCPS_OPEN_STATE);
                          

} /* hdrbccp_process_activate_cmd() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCCP_DEACTIVATE

DESCRIPTION
   This function commands BCCP to deactivate 
     
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol sending the command
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/

void hdrbccp_deactivate
(
  hdrhai_protocol_name_enum_type sender
)
{

  hdrbccp_cmd_type hdrbccp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_LOW, "hdrbccp_deactivate");

  /* Fill in all members of the hdrbccp_cmd_type for this command */
  hdrbccp_cmd.name = HDRBCCP_DEACTIVATE_CMD;
  hdrbccp_cmd.sender = sender;

  if (rex_self() == HDRBCCP_TASK_PTR)
  {
    hdrbccp_process_cmd( &hdrbccp_cmd );
  }
  else
  {
    ERR("Deactivate Cmd from outside HDRMC",0,0,0);
    hdrmc_queue_cmd( HDRHAI_BROADCAST_PROTOCOL, &hdrbccp_cmd, 
                     sizeof(hdrbccp_cmd_type));
  }

} /* hdrbccp_deactivate() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_DEACTIVATE_CMD

DESCRIPTION
  This function processes a deactivate command given to BCCP. 
        
DEPENDENCIES
  None.

PARAMETERS
  bccp_cmd_ptr - Pointer to the command for BCCP to process
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_process_deactivate_cmd
(
  hdrbccp_cmd_type * bccp_cmd_ptr 
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Deactivate is received when ALMP goes back to INIT or INACTIVE state.
     (Acquisition loss). */
  hdrbccp_flow_info.exit_traffic_for_best_bc_channel = FALSE;

  /*---------------------------------------------------------------------------
  If the Protocol has already been deactivated, do nothing.
  ---------------------------------------------------------------------------*/
  if ( !hdrbccp.broadcast_available  ||
       hdrbccps_get_state() == HDRBCCPS_CLOSE_STATE)
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_BROADCAST_PROTOCOL,
        bccp_cmd_ptr->name , bccp_cmd_ptr->sender, hdrbccps_get_state() );
    return;
  }

  hdrbccps_change_state(HDRBCCPS_CLOSE_STATE);

} /* hdrbccp_process_deactivate_cmd() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_TEST_FLOW_ENABLE

DESCRIPTION
  Sends a Test Flow Enable Cmd to HDRMC.
  
DEPENDENCIES
  Disallow a test flow enable cmd in CLOSE state.

PARAMETERS
  flow_id   - Flow ID of the test flow to enable 
              (No IP:Port# address provided for test flows)   
  data_cb   - The callback function that is used to deliver broadcast data.
  sender    - Protocol sending the command
  
RETURN VALUE
  E_SUCCESS - enable test flow command successful
  E_FAILURE - failure processing test flow command

SIDE EFFECTS
  - Test flows may only be enabled when no DS flows are currently desired.
  - All information left
   
=============================================================================*/
hdrerrno_enum_type hdrbccp_test_flow_enable
(
  sys_bcmcs_flow_id_type flow_id, 
  void (*data_cb)(dsm_item_type * data_pkt),
  hdrhai_protocol_name_enum_type sender
)
{
  hdrbccp_cmd_type hdrbccp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Flows can't be enabled in close state */
   if (hdrbccps_get_state() == HDRBCCPS_CLOSE_STATE)
   {
     HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                    "Can't Enable Test Flow %d in CLOSE state - ignore",
                    flow_id);
     return E_FAILURE;
   }

   /* We don't allow test flows while some app already has desired flows */
   if ( (q_cnt(&hdrbccp_flow_info.flow_des_priority_q) != 0) &&
        !hdrbccp_flow_info.test_flow_enabled )
   {
     HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                    "Can't Enable Test Flow %d: DS flows already existing.",
                    flow_id);
     return E_FAILURE;
   }

   if ( data_cb == NULL) 
   {
     HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
      "Null callback provided; abort enable (flow_id: %d)",
      flow_id);
     return E_FAILURE;
   }

   HDR_MSG_PROT_1(MSG_LEGACY_MED, "hdrbccp_test_flow_enable (FlowID %d)",
                  flow_id);
   
   /* Fill in all members of the hdrbccp_cmd_type for this command */
   hdrbccp_cmd.name = HDRBCCP_TEST_FLOW_ENABLE_CMD;
   hdrbccp_cmd.sender = sender;  
   hdrbccp_cmd.params.flow_en.flow_id = flow_id;
   hdrbccp_cmd.params.flow_en.data_cb = data_cb;

   if (rex_self() == HDRBCCP_TASK_PTR)
   {
     hdrbccp_process_cmd( &hdrbccp_cmd );
   }
   else
   {
     hdrmc_queue_cmd( HDRHAI_BROADCAST_PROTOCOL, &hdrbccp_cmd, 
                      sizeof(hdrbccp_cmd_type));
   }

   return E_SUCCESS;

} /* hdrbccp_test_flow_enable() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_TEST_FLOW_ENABLE_CMD

DESCRIPTION
  Enables a broadcast test flow.
  
DEPENDENCIES
  None.

PARAMETERS
  hdrbccp_cmd_ptr - pointer to the bccp cmd to process
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_process_test_flow_enable
(
  hdrbccp_cmd_type *hdrbccp_cmd_ptr
)
{

  hdrbccp_flow_type flow_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  flow_info.ip_port.ip_address.addr.v4 = 0;
  flow_info.ip_port.ip_address.type = SYS_IP_ADDR_INVALID;
  flow_info.ip_port.port_number = 0;
  flow_info.flow_format = 0;
  
  flow_info.flow_id = hdrbccp_cmd_ptr->params.flow_en.flow_id;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
                              
  /* If this test flow hasn't already been requested, 
     add it to the desired flow list */
  if (!hdrbccp_flow_id_already_requested(hdrbccp_cmd_ptr->params.flow_en.flow_id))
  {
    /* Add this flow to the desired flow list */
    hdrbccp_add_new_des_flow(&flow_info,
                             hdrbccp_cmd_ptr->params.flow_en.data_cb,
#ifdef FEATURE_HDR_BCMCS_2p0
                             #error code not present
#else
                             q_cnt(&hdrbccp_flow_info.flow_des_priority_q)
#endif /* FEATURE_HDR_BCMCS_2p0 */
                             );


    /* If this is the first requested flow, notify searcher so we could enter
       dual slotted mode */
    if ( q_cnt(&hdrbccp_flow_info.flow_des_priority_q) == 1 )
    {
      hdrlmac_set_bcmcs_page_cycle(hdrbccpc_get_curr_bc_ovhd_period());
    }

    hdrbccp_flow_info.test_flow_enabled = TRUE;

    hdrovhd_reparse_bc_msg( HDRHAI_BROADCAST_PROTOCOL );
  }

} /* hdrbccp_process_test_flow_enable() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_TEST_FLOW_DISABLE

DESCRIPTION
  Sends a Test Flow Disable Cmd to HDRMC.
  
DEPENDENCIES
  Disallow a test flow disnable cmd in CLOSE state.

PARAMETERS
  flow_id   - Flow ID of the test flow to disable 
              (No IP:Port# address provided for test flows)   
  sender    - Protocol sending the command              
  
RETURN VALUE
  E_SUCCESS - disable test flow command successful
  E_FAILURE - failure processing test flow command

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrbccp_test_flow_disable
(
  sys_bcmcs_flow_id_type flow_id,
  hdrhai_protocol_name_enum_type sender
)
{
   hdrbccp_cmd_type hdrbccp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Flows can't be enabled in close state */
   if (hdrbccps_get_state() == HDRBCCPS_CLOSE_STATE)
   {
     HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                    "Can't Disable Test Flow %d in CLOSE state - ignore",
                    flow_id);
     return E_FAILURE;
   }

   HDR_MSG_PROT_1(MSG_LEGACY_MED, "hdrbccp_test_flow_disable (FlowID %d)",
                  flow_id);
   
   /* Fill in all members of the hdrbccp_cmd_type for this command */
   hdrbccp_cmd.name = HDRBCCP_TEST_FLOW_DISABLE_CMD;
   hdrbccp_cmd.sender = sender;  
   hdrbccp_cmd.params.flow_dis.flow_id = flow_id;

   if (rex_self() == HDRBCCP_TASK_PTR)
   {
     hdrbccp_process_cmd( &hdrbccp_cmd );
   }
   else
   {
     hdrmc_queue_cmd( HDRHAI_BROADCAST_PROTOCOL, &hdrbccp_cmd, 
                      sizeof(hdrbccp_cmd_type));
   }

   return E_SUCCESS;

} /* hdrbccp_test_flow_enable() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_TEST_FLOW_DISABLE

DESCRIPTION
  Disables a broadcast test flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id   - Flow ID of the test flow to disable   
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void hdrbccp_process_test_flow_disable
(
  hdrbccp_cmd_type *hdrbccp_cmd
)
{

  hdrbccp_flow_buf_type *q_buf_ptr = NULL;
  uint32 flow_id; /* Convenience variable. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  flow_id = hdrbccp_cmd->params.flow_dis.flow_id; 

  if (!hdrbccp_flow_info.test_flow_enabled)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,"No Test flows currently enabled");
    return;
  }

  q_buf_ptr = (hdrbccp_flow_buf_type*) 
               q_check(&hdrbccp_flow_info.flow_des_priority_q);

  while ( q_buf_ptr != NULL )
  {
    if (q_buf_ptr->flow_info.flow_id == flow_id)
    {
      hdrbccpfs_change_state(q_buf_ptr,HDRBCCPFS_UNDESIRED);
      break;
    }

    q_buf_ptr = (hdrbccp_flow_buf_type*)
                q_next(&hdrbccp_flow_info.flow_des_priority_q,
                       &q_buf_ptr->link);
  }

  /* We got to the end of the desired flow list without finding the flow. */
  if (q_buf_ptr == NULL)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"Can't disable Test flow %d, not found.",flow_id);
  }
  else
  {
    /* The desired flow list changed: notify ovhd. */
    hdrovhd_reparse_bc_msg( HDRHAI_BROADCAST_PROTOCOL );
  }

  /* No test flows enabled left */
  if (q_cnt(&hdrbccp_flow_info.flow_des_priority_q) == 0)
  {
    hdrbccp_flow_info.test_flow_enabled = FALSE;

    /* We can go back to single slotted sleep. */
    hdrlmac_set_bcmcs_page_cycle(0);
  }

} /* hdrbccp_process_test_flow_disable() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_UPDATE_FLOW_REQUEST

DESCRIPTION
  This function commands BCCP to update its requested flow list.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  transaction_id - Transaction ID
  num_flows      - Number of bcmcs flows included in req_flow_list
  req_flow_list  - The list of requested flows (IP:port# address) 
                   and their statuses
  reg_setup_list - Registration setup flag for each of the requested flows

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbccp_update_flow_request
(
  hdrhai_protocol_name_enum_type sender,
  uint8 transaction_id,
  uint8 num_flows,
  sys_bcmcs_flow_addr_s_type *req_flow_list
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
)
{
  hdrbccp_cmd_type hdrbccp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(num_flows <= HDRBCCPI_BCMCS_MAX_FLOWS);

  if (num_flows == 0)
  {
    ERR("CM sent UpdateFlowReq with empty list: Discontinue Broadcast", 0,0,0);

    /* This should not happen!
       To recover, the AT sends a discontinue broadcast report to CM instead 
       of a FlowReqSuccess and makes all flows undesired if needed */
    hdrbccp_discontinue_broadcast( HDRHAI_BROADCAST_PROTOCOL, 
                                   transaction_id );
    return;
  }

  if (!hdrbccp.broadcast_available)
  {
    ERR("Broadcast not negotiated, ignore FlowRequest (tid %d)",
        transaction_id,0,0);
    return;
  }

  HDR_MSG_PROT(MSG_LEGACY_LOW, "hdrbccp_update_flow_request");

  /* Fill in all members of the hdrbccp_cmd_type for this command */
  hdrbccp_cmd.name = HDRBCCP_UPDATE_FLOW_REQ_CMD;
  hdrbccp_cmd.sender = sender;
  hdrbccp_cmd.params.update_flow.num_flows = num_flows;
  hdrbccp_cmd.params.update_flow.transaction_id = transaction_id;
  
  memcpy (hdrbccp_cmd.params.update_flow.req_flow_list, 
          req_flow_list, 
          num_flows * sizeof(sys_bcmcs_flow_addr_s_type));

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

  ASSERT (rex_self() == HDRBCCP_TASK_PTR);
  hdrbccp_process_cmd( &hdrbccp_cmd );

} /* hdrbccp_update_flow_request() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_UPDATE_FLOW_REQ_CMD

DESCRIPTION
   This function processes an UpdateFlowRequest command given to the BCCP 
   protocol
   
DEPENDENCIES
  None.

PARAMETERS
  bccp_cmd_ptr - Pointer to the command for BCCP to process  
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_process_update_flow_req_cmd
(
  hdrbccp_cmd_type * bccp_cmd_ptr
)
{
  uint8 index;
  hdrbccp_update_flow_req_params_type * flow_req_ptr; /* Convenience ptr */
  hdrbccp_flow_buf_type *q_buf_ptr = NULL;
  hdrbccp_flow_buf_type *q_next_buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  flow_req_ptr = &bccp_cmd_ptr->params.update_flow;

  ASSERT( flow_req_ptr->num_flows != 0 );

  /* The AT accepts FlowRequests in CLOSED state providing that broadcast is
     available.  The AT will store the request in the db cache, but will not
     do a DB lookup until BCMCS is activated. */
  if ( hdrbccps_get_state() == HDRBCCPS_CLOSE_STATE )
  {
    hdrbccp_clear_dsdb_cache();
  }

  /* We always process FlowRequests from CM (even if for a duplicate query) */
  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"Process UpdateFlowRequest cmd (tid: %d)",
                 flow_req_ptr->transaction_id);

  /*--------------------------------------------------------------------------- 
     If we have any test flows enabled, disable them first before allowing 
     new flows.
  ---------------------------------------------------------------------------*/
  if (hdrbccp_flow_info.test_flow_enabled)
  {

    HDR_MSG_PROT(MSG_LEGACY_HIGH,
                 "Disable test flows b4 honoring the UPDATE_FLOW_REQUEST cmd");

    /* Clean up desired flow list */
    q_buf_ptr = (hdrbccp_flow_buf_type *)
                q_check( &hdrbccp_flow_info.flow_des_priority_q );

    while (q_buf_ptr != NULL)
    {

      q_next_buf_ptr = (hdrbccp_flow_buf_type *)
                        q_next(&hdrbccp_flow_info.flow_des_priority_q,
                        &q_buf_ptr->link);

      hdrbccp_delete_flow_entry(q_buf_ptr);

      q_buf_ptr = q_next_buf_ptr;

    }

    /* Clean up undesired flow list */
    q_buf_ptr = (hdrbccp_flow_buf_type *)
                q_check( &hdrbccp_flow_info.flow_undes_priority_q );

    while (q_buf_ptr != NULL)
    {

      q_next_buf_ptr = (hdrbccp_flow_buf_type *)
                        q_next(&hdrbccp_flow_info.flow_undes_priority_q,
                        &q_buf_ptr->link);

      hdrbccp_delete_flow_entry(q_buf_ptr);

      q_buf_ptr = q_next_buf_ptr;

    }

    hdrbccp_flow_info.test_flow_enabled = FALSE;

  } /* end if test flows enabled */

  /*--------------------------------------------------------------------------- 
    Make sure that there are no pending flow request updates.  If there are,
    a new set of db lookups will be generated for the latest flow request 
    list and the current round of lookups will be ignored. 
  ---------------------------------------------------------------------------*/  
  if (hdrbccp.db_cache.num_addr_pending_lookup != 0)
  {
    /* db lookups are pending for an old DS flow request cmd, send a 
       cancel rpt to CM. */
    switch ( hdrbccp.db_cache.query_type)
    {
      case HDRBCCP_DB_FLOW_REQ_QUERY:
      {     

        HDR_MSG_PROT(MSG_LEGACY_HIGH,
                     "db lookups still pending: cancel and requery");

        for (index = 0; index < hdrbccp.db_cache.req_flows; index++)
        {
          hdrbccp.db_cache.flow_req_list[index].status = 
            SYS_BCMCS_FLOW_STATUS_CANCELLED;

          HDR_MSG_PROT_3(MSG_LEGACY_HIGH,"Cancel query h:%d, IP:0x%x, port#:%d",
            hdrbccp.db_cache.flow_req_list[index].db_query_handle,
            hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.ip_address.addr.v4,
            hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.port_number);

        }
    
        /* Send a cancelled report to CM through the Flow Request success report */
        hdrbccp_send_flow_req_success_rpt_to_cm();
      }

      /* ***** Fall through! ***** */

      /* db lookups are still pending for a subnet change query, we just cancel
         the lookups. */
      case HDRBCCP_DB_SUBNET_CHANGE_QUERY:
      {
        /* Clean up the cache */
        hdrbccp_clear_dsdb_cache();
      }
      break;

      case HDRBCCP_DB_NO_QUERY:
      default:
      ERR("Invalid dsdb cache state",0,0,0);
      break;
    }
  } /* end if old addresses still pending look up */

  /* Copy the flow request info in local cache */
  hdrbccp.db_cache.query_type              = HDRBCCP_DB_FLOW_REQ_QUERY;
  hdrbccp.db_cache.transaction_id          = flow_req_ptr->transaction_id;
  hdrbccp.db_cache.total_flows             = flow_req_ptr->num_flows;
  hdrbccp.db_cache.req_flows               = flow_req_ptr->num_flows;
  hdrbccp.db_cache.num_addr_pending_lookup = flow_req_ptr->num_flows;

#ifdef FEATURE_HDR_BCCP_DEBUG
  HDR_MSG_PROT(MSG_LEGACY_MED,"Copy flow request to db lookup cache");
  HDR_MSG_PROT_2(MSG_LEGACY_MED,"trans_id: %d num_req_flows: %d", 
                 hdrbccp.db_cache.transaction_id,
                 hdrbccp.db_cache.req_flows);
#endif /* FEATURE_HDR_BCCP_DEBUG */

  /* Copy the flow request info */
  for (index = 0; index < flow_req_ptr->num_flows; index++)
  {

    memcpy( &(hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port),
            &(flow_req_ptr->req_flow_list[index]),
            sizeof(sys_bcmcs_flow_addr_s_type) );

    hdrbccp.db_cache.flow_req_list[index].flow_info.flow_id = 0;

#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

    HDR_MSG_PROT_3(MSG_LEGACY_MED,"db query [%d] IP:0x%x port:%d",
      index,
      hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.ip_address.addr.v4,
      hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.port_number);
  }

  /* Remember if the last FlowRequest was received in traffic (or connection 
     setup). */
  if (hdrbccps_get_state() == HDRBCCPS_OPEN_STATE)
  {
    if (hdrrup_is_tc_assigned( HDRHAI_BROADCAST_PROTOCOL ))
    {
      hdrbccp_flow_info.exit_traffic_for_best_bc_channel = TRUE;
      HDR_MSG_PROT( MSG_LEGACY_MED, "Flow Request received in traffic" );
    }
    else if (hdridles_get_state() == HDRIDLES_CONN_SETUP_STATE)
    {
      hdrbccp_flow_info.exit_traffic_for_best_bc_channel = TRUE;
      HDR_MSG_PROT( MSG_LEGACY_MED, "Flow Request received during conn setup" );
    }
    else
    {
      hdrbccp_flow_info.exit_traffic_for_best_bc_channel = FALSE;
      HDR_MSG_PROT( MSG_LEGACY_MED, "Flow Request cmd received while idle" );
    }

    hdrbccp_query_dsdb();
  }
  /* If we are in closed state, save the db lookup until BCMCS is activated */
  else /* HDRBCCPS_CLOSE_STATE */
  {
    hdrbccp_flow_info.exit_traffic_for_best_bc_channel = FALSE;
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Flow Request received in BC CLOSED state, can't query db" );

    for (index = 0; index < hdrbccp.db_cache.req_flows; index++)
    {
      hdrbccp.db_cache.flow_req_list[index].status = 
        SYS_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE;

      HDR_MSG_PROT_2(MSG_LEGACY_HIGH,"DB query postponed for IP:0x%x, port#:%d",
        hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.ip_address.addr.v4,
        hdrbccp.db_cache.flow_req_list[index].flow_info.ip_port.port_number);
    }

    /* Send a Flow Request success report to CM (ack) with UNAVAILABLE status 
    for each flow. */
    hdrbccp_send_flow_req_success_rpt_to_cm();

  }

} /* hdrbccp_process_update_flow_req_cmd() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCCP_DISCONTINUE_BROADCAST

DESCRIPTION
   This function commands BCCP to Discontinue Broadcast Operations.
     
DEPENDENCIES
  None.

PARAMETERS
  sender         - Protocol processing the command
  transaction_id - Transaction ID
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccp_discontinue_broadcast
(
  hdrhai_protocol_name_enum_type sender,
  uint8 transaction_id
)
{

  hdrbccp_cmd_type hdrbccp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  HDR_MSG_PROT(MSG_LEGACY_LOW, "hdrbccp_discontinue_broadcast");

  /* Fill in all members of the hdrbccp_cmd_type for this command */
  hdrbccp_cmd.name = HDRBCCP_DISCONTINUE_BROADCAST_CMD;
  hdrbccp_cmd.sender = sender;
  hdrbccp_cmd.params.stop_bc.transaction_id = transaction_id;

  /* We should be able to process a Discontinue Broadcast command in any 
     HDR state.  If HDR is not active, process the cmd directly without
     queueing it. */
  if ( (rex_self() == HDRBCCP_TASK_PTR) ||
       (hdrbccps_get_state() == HDRBCCPS_CLOSE_STATE) )
  {
    hdrbccp_process_cmd( &hdrbccp_cmd );
  }
  else
  {
    ERR("Discontinue Broadcast Cmd from outside HDRMC",0,0,0);
    hdrmc_queue_cmd( HDRHAI_BROADCAST_PROTOCOL, &hdrbccp_cmd, 
                     sizeof(hdrbccp_cmd_type));
  }

} /* hdrbccp_discontinue_broadcast() */

/* <EJECT> */ 
/*=============================================================================

FUNCTION HDRBCCP_PROCESS_DISCONTINUE_BC_CMD

DESCRIPTION
  This function processes a DiscontinueBroadcast command given to the BCCP
   protocol. 
        
DEPENDENCIES
  None.

PARAMETERS
  bccp_cmd_ptr - Pointer to the command for BCCP to process  
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Disables all flows that were currently monitored.
=============================================================================*/
void hdrbccp_process_discontinue_bc_cmd
(
  hdrbccp_cmd_type * bccp_cmd_ptr
)
{
  hdrbccp_flow_buf_type * q_buf_ptr = NULL;
  hdrbccp_flow_buf_type * q_next_buf_ptr = NULL;   
    /* Queue traversal pointers */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the db lookup cache if lookups are still pending.  We don't send
     a flow status to CM/DS since we will be sending a DiscontinueBroadcast
     ACK cmd instead. */
  if (hdrbccp.db_cache.num_addr_pending_lookup != 0)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,
                 "Rxed DiscontinueBroadcast cmd - cancel pending db lookups.");

    hdrbccp_clear_dsdb_cache();
  }

  /*-------------------------------------------------------------------------
  Go through the desired flow list and make all the flows undesired.
  All flows that were currently monitored will be disabled as a side effects.
  -------------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check( &hdrbccp_flow_info.flow_des_priority_q );

  while ( q_buf_ptr != NULL )
  { 

    q_next_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                         &q_buf_ptr->link );

    hdrbccpfs_change_state(q_buf_ptr, HDRBCCPFS_UNDESIRED);

    /* Update the flow buffer pointer to point to the next entry in the queue.  We
       don't call q_next on q_buf_ptr because the desired entry is moved to the
       undesired queue when the flow is no longer desired */
    q_buf_ptr = q_next_buf_ptr;

  }

  
  /* Discontinuing broadcast - notify searcher that we no longer need to be
     in dual slotted sleep. */
  if ( q_cnt(&hdrbccp_flow_info.flow_des_priority_q) == 0)
  {
    /* Searcher accepts the page cycle cmd in inactive/acq state, so update
       searcher here */
    hdrlmac_set_bcmcs_page_cycle(0);

#ifdef FEATURE_HDR_BCMCS_MFRQ
    if ( hdrbccps_get_state() != HDRBCCPS_CLOSE_STATE )
    {
      hdrovhd_reparse_bc_msg( HDRHAI_BROADCAST_PROTOCOL );
    }
#endif /* FEATURE_HDR_BCMCS_MFRQ */
  }

  /*-------------------------------------------------------------------------
  Send a DiscontinueBroadcast report (ACK) to CM.
  -------------------------------------------------------------------------*/
  hdrbccp_send_broadcast_discontinue_rpt_to_cm(
    bccp_cmd_ptr->params.stop_bc.transaction_id);

} /* hdrbccp_process_discontinue_bc_cmd() */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCCP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to BCCP.

DEPENDENCIES
  None

PARAMETERS
  almp_cmd_ptr - Pointer to the command for BCCP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbccp_process_cmd
( 
  hdrbccp_cmd_type * bccp_cmd_ptr 
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( bccp_cmd_ptr->name != HDRBCCP_DB_LOOKUP_COMPLETED_CMD )
  {
    HDRTRACE_CMD( HDRHAI_BROADCAST_PROTOCOL, bccp_cmd_ptr->name,
      bccp_cmd_ptr->sender, hdrbccps_get_state() );
  }

  switch ( bccp_cmd_ptr->name )
  {
    case HDRBCCP_ACTIVATE_CMD:
      hdrbccp_process_activate_cmd( bccp_cmd_ptr ); 
    break;

    case HDRBCCP_DEACTIVATE_CMD:
      hdrbccp_process_deactivate_cmd( bccp_cmd_ptr );
    break;

    case HDRBCCP_TEST_FLOW_ENABLE_CMD:
      hdrbccp_process_test_flow_enable( bccp_cmd_ptr );
    break;

    case HDRBCCP_TEST_FLOW_DISABLE_CMD:
      hdrbccp_process_test_flow_disable( bccp_cmd_ptr ); 
    break;

    case HDRBCCP_UPDATE_FLOW_REQ_CMD:
      hdrbccp_process_update_flow_req_cmd( bccp_cmd_ptr );
    break;

    case HDRBCCP_DISCONTINUE_BROADCAST_CMD:
      hdrbccp_process_discontinue_bc_cmd( bccp_cmd_ptr );
    break;

    case HDRBCCP_DB_LOOKUP_COMPLETED_CMD:
      hdrbccp_process_dsbcdb_lookup_cb( bccp_cmd_ptr );
    break;

#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

    default:
      ERR_FATAL( "Unknown command (%d)", bccp_cmd_ptr->name, 0, 0 );
      break;
  }

} /* hdrbccp_process_cmd() */

/* <EJECT> */ 

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
hdrerrno_enum_type hdrbccp_get_preferred_bc_channel
(
  sys_channel_type *bc_idle_channel_ptr,
  sys_bcmcs_flow_id_type *ref_flow_id_ptr
)
{
  if (!hdrbccp.broadcast_available)
  {
    return E_FAILURE;
  }

  if ( (hdrbccp_flow_info.ref_flow_ptr != NULL) &&
       (q_cnt(&hdrbccp_flow_info.flow_des_priority_q) != 0) &&
       (bc_idle_channel_ptr != NULL) )
  {
    *bc_idle_channel_ptr = hdrbccp_flow_info.bc_hash_channel;

    if (ref_flow_id_ptr != NULL)
    {
      *ref_flow_id_ptr = hdrbccp_flow_info.ref_flow_ptr->flow_info.flow_id;
    }

    HDR_MSG_PROT_2(MSG_LEGACY_MED,"preferred_bc_channel %d (flow %d)",
                   hdrbccp_flow_info.bc_hash_channel.chan_num,
                   hdrbccp_flow_info.ref_flow_ptr->flow_info.flow_id);

    return E_SUCCESS;
  }

  HDR_MSG_PROT(MSG_LEGACY_MED,"No preferred BC channel...");
  return E_NOT_AVAILABLE;

} /* hdrbccp_get_preferred_bc_channel() */

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
  This is used for FEATURE_JCDMA.

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
hdrerrno_enum_type hdrbccp_get_original_channel
(
  sys_channel_type *current_channel_ptr,
  sys_channel_type *original_channel_ptr
)
{
  hdrerrno_enum_type err_number = E_NOT_AVAILABLE; /* Value to be returned */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((!hdrbccp.broadcast_available) || (original_channel_ptr == NULL))
  {
    return E_FAILURE;
  }

  if ((hdrbccp_flow_info.ref_flow_ptr == NULL) &&
      (hdrbccp_flow_info.tune_back_to_original_channel_now))
  {
    hdrbccp_flow_info.tune_back_to_original_channel_now = FALSE;
    /* If the original channel is Band Class 0 Subclass 3 but not
       within Subclass 2, and the broadcast channel is within Subclass 2, 
       then upon ending broadcast, tune back out of Subclass 2 regardless 
       of the hash channel (which would keep us in Subclass 2)
     */

    if ( HDRBCCP_IS_BC0_SC3_NOT_SC2( hdrbccp_flow_info.original_channel ) &&
         HDRBCCP_IS_BC0_SC2( *current_channel_ptr  ) )
    {
      *original_channel_ptr = hdrbccp_flow_info.original_channel;
      err_number = E_SUCCESS;
    }
    
  }

  return err_number;

} /* hdrbccp_get_original_channel() */
#endif /* FEATURE_HDR_BCMCS_MFRQ */

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
void hdrbccp_disable_all_monitored_flows
( 
  sys_bcmcs_flow_status_e_type status
)
{

  hdrbccp_flow_buf_type * q_buf_ptr = NULL; 
    /* Queue traversal pointer */

  boolean flow_status_changed = FALSE; 
    /* Whether any of the flows changed status */    
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"Disable all available flows (reason: %d)", 
                 status);

  /*-------------------------------------------------------------------------
  Go through the desired flow list and disable all the flows that are 
  in monitored state (still desired, just no longer available) 
  -------------------------------------------------------------------------*/
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check( &hdrbccp_flow_info.flow_des_priority_q );

  while ( q_buf_ptr != NULL )
  { 

    if ( (q_buf_ptr->state == HDRBCCPFS_DESIRED_MONITORED) ||
         (q_buf_ptr->state == HDRBCCPFS_DESIRED_UNMONITORED)
#ifdef FEATURE_HDR_BCMCS_2p0
         #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
       )
    {
      flow_status_changed = TRUE;
      hdrbccpfs_change_state(q_buf_ptr, HDRBCCPFS_DESIRED_UNAVAILABLE);
    }

    /* Update the flow buffer pointer to point to the next entry in the queue.  We
       don't call q_next on q_buf_ptr because the desired entry is moved to the
       undesired queue when the flow is no longer desired */
    q_buf_ptr = (hdrbccp_flow_buf_type *)
                 q_next( &hdrbccp_flow_info.flow_des_priority_q, 
                         &q_buf_ptr->link );

  }

  /* Send a flow update to cm if any of the flows changed status */
  if (flow_status_changed)
  {  
    /* Don't notify CM/DS of test flow status changes.
       (DS is unaware of test flows) */
    if (!hdrbccp_flow_info.test_flow_enabled)
    {
      if (status == SYS_BCMCS_FLOW_STATUS_TIMEOUT)
      {
        hdrbccp_send_flow_status_to_cm(HDRBCCP_BC_DISABLED_SUPERVISION);
      }
      else /* if (status == SYS_BCMCS_FLOW_STATUS_LOST) */
      {
        hdrbccp_send_flow_status_to_cm(HDRBCCP_BC_DISABLED_SYS_LOST);
      }
    }

    if (hdrbccp_flow_info.ref_flow_ptr != NULL)
    {
      HDR_MSG_PROT_3(MSG_LEGACY_HIGH,
                     "Deactivate - save ref. flowID(%d) (chan%d band%d)",
                     hdrbccp_flow_info.ref_flow_ptr->flow_info.flow_id,
                     hdrbccp_flow_info.bc_hash_channel.chan_num,
                     hdrbccp_flow_info.bc_hash_channel.band);
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,"We should have a valid ref. flow ptr!");
    }
  }

} /* hdrbccp_disable_all_monitored_flows */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#endif /* FEATURE_HDR_BCMCS */

