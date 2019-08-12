/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          H D R   B R O A D C A S T   R E G I S T R A T I O N 
       
                            M O D U L E       

GENERAL DESCRIPTION
  This module contains the implementation of the HDR  Broadcast Registration
  Module.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2005,2006,2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2008, 2009, 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccpreg.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/13   smd     Used new secapi to get random number.
12/01/09   suren   Added QChat optimization changes.
10/27/09   wsh     Fixed compiler warning for ARM compiler
09/30/09   wsh     Fixed KW warnings
08/06/09   wsh     Support CMI API changes in sys.h 
01/27/09   wsh     Merged BCMCS 2.0 onto mainbranch
12/10/08   smd     Replaced assert.h with amssassert.h
10/01/08   wsh     Fixed critical/high lint warnings
09/11/07   wsh     Removed lint high warnings
08/16/07   wsh     Removed compiler and lint critical warnings
10/19/06   mpa     Ignore subnet info for now when processing reg timer expiry.
10/18/06   mpa     Enable one time registration by default if not active in NV.
07/20/06   mpa     Mainlined REVA_L1 session config.
05/11/06   mpa     Initialize programs struct before building registration list.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/05/06   mpa     Fixed async flow regitration triggers.
04/05/06   mpa     Use format1 in registration message for format1 flows
12/15/05   mpa     Fixed hack to disable subnet handoff handling.
11/15/05   mpa     Do not try to send a registration message if the session
                   is opened but not negotiated.
10/07/05   mpa     Fixed registration startup.
                   Fixed concurrent registration for paging/dynamic. 
08/28/05   mpa     Created Module
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
#include "ran.h"

#include "hdrdebug.h"
#include "hdrerrno.h"
#include "hdrhmp.h"
#include "hdrovhd.h"
#include "hdrsmp.h"
#include "hdrstream.h"
#include "hdrutil.h"
#include "hdrbit.h"
#include "hdrtrace.h"

#include "hdrbccpreg.h"
#include "hdrbccpi.h"
#include "hdrbccps.h"
#include "hdrbccpc.h"
#include "hdrbcovhd.h"
#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk_opts.h"
#include "hdrqchatupk.h"
#endif /*FEATURE_HDR_QCHAT*/
#include "hdraddr_v.h"

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

extern hdrbccp_flow_info_type hdrbccp_flow_info;

/* We might want to be able to disable One Time Registration of newly
   requested flows.  This can be implemented as an NV item if needed. */
#define HDRBCCPREG_DISABLE_ONE_TIME_REG FALSE

/* We might want to be able to disable AT initiated registration on subnet
   boundaries.  This can be implemented as an NV item if needed. */
#define HDRBCCPREG_DISABLE_SUBNET_REG FALSE

/* The lower limit used by the AT for unmonitored reg_interval  */
#define HDRBCCPREG_UNMONITORED_REG_INTERVAL_MIN_MS 10000

/* The maximum number of subnets to keep registration info about */
#define HDRBCCPREG_MAX_SUBNETS 3

#define HDRBCCPREG_FLOW_REG_MSG_PRI  30

/*-----------------------------------------------------------------------------
                          Registration Info
-----------------------------------------------------------------------------*/     
#define HDRBCCPREG_MIN_REG_INTERVAL 0x01

#define HDRBCCPREG_PERIOD_TO_80MS_CONV( period ) \
   ( ( 1 << (period) ) * 80 )
   
/*----------------------------- 
 Registration Types (Bit Mask) 
-----------------------------*/

#define HDRBCCPREG_REGN_NONE      0x00
  /* Registration is disabled */

#define HDRBCCPREG_REGN_PAGING    0x01
  /* Current registration is only for paging */

#define HDRBCCPREG_REGN_DYN_BCAST 0x02
  /* Current registration is only for dynamic broadcast */

#define HDRBCCPREG_REGN_BOTH (HDRBCCPREG_REGN_PAGING | HDRBCCPREG_REGN_DYN_BCAST)
  /* Current registration is for both paging & dynamic broadcast */


/*-------------------------------
  Registration Msg Packing info 
-------------------------------*/
/* Message ID */
#define HDRBCCPREG_REGISTRATION_MSG 0x00

typedef enum
{
  HDRBCCPREG_MSG_ID_SIZE                    = 8,
  HDRBCCPREG_TIMESTAMP_SHORT_INCLUDED_SIZE  = 1,
  HDRBCCPREG_AUTH_SIGNATURE_INCLUDED_SIZE   = 1,
  HDRBCCPREG_BCMCS_FLOW_FORMAT_SIZE         = 1,
  HDRBCCPREG_BCMCS_FLOW_ID_LENGTH_SIZE      = 2,
  HDRBCCPREG_BCMCS_FLOW_COUNT_SIZE          = 6,
  HDRBCCPREG_BCMCS_FLOW_ID_SIZE             = 8,
  HDRBCCPREG_BCMCS_PROG_CNT_SIZE            = 6,
  HDRBCCPREG_BCMCS_PROG_ID_LSB_LGTH_SIZE    = 5,
  HDRBCCPREG_BCMCS_FLOW_DISCR_CNT_SIZE      = 6,
  HDRBCCPREG_BCMCS_FLOW_DISCR_LGTH_SIZE     = 3,       
  HDRBCCPREG_RESERVED_SIZE                  = 7,
}hdrbccpreg_reg_field_enum_type;

/*-------------
  Subnet Info
-------------*/  
typedef struct
{
  boolean is_valid;
    /* Whether the entry is valid */
  
  hdraddr_type subnet;
    /* Subnet we have visited with the same requested flow list.
       The subnet cache is reset when the requested flow list changes.*/

  rex_timer_type reg_timer;
    /* Registration Timer */

  qword last_subnet_reg_time;
    /* Timestamp of when a registration message was successfully sent 
       on this subnet */

  uint32 smaller_reg_periods_cnt;
    /* How many sub registration periods have elapsed */
  
  qword last_dyn_timer_expiry;
  qword last_paging_timer_expiry;
    /* Absolute time of when a registration timer last expired on this subnet */

} hdrbccpreg_subnet_reg_info_type;

/*-------------------------
  Registration Timer Info
-------------------------*/
typedef struct
{

  uint8 reg_type;  
    /* Registration Type Mask */

  uint8 smaller_reg_interval_type; 
    /* Registration Type associated with the smaller period. (The Registration
       timer is always set to MIN[paging_reg_timer_max, dyn_bc_reg_timer_max] )
     */

  uint32 smaller_reg_interval_ms; 
    /* Registration Duration associated with the smaller period. */

  uint32 total_smaller_reg_intervals;  
    /* Number of smaller registration periods contained in a large 
       registration interval */ 

  uint32 monitor_period_ms;

  
} hdrbccpreg_reg_timer_info_type;

typedef struct
{
  /* Still awaiting SLP cb for this registration message */
  boolean awaiting_slp_cb;

  /* List of flow IDs included in the last registration message */
  uint8 flow_count;
  hdrbccp_flow_type flow_id_list[HDRBCCPI_BCMCS_MAX_FLOWS];

} hdrbccpreg_reg_flow_type;

LOCAL struct
{
  /*------------------------------------------------
    Registration Info at subnet boundaries
  ------------------------------------------------*/
  hdrbccpreg_subnet_reg_info_type subnet_info[HDRBCCPREG_MAX_SUBNETS];

  /* Keeps track of the current subnet */
  uint8 curr_subnet_idx;

  /* User defined in NV - Enable / Disable subnet registration */
  boolean disable_subnet_reg;
  
  /*------------------------------------------------
    Registration Timer Info 
  ------------------------------------------------*/
  hdrbccpreg_reg_timer_info_type reg_info;

  /*------------------------------------------------
    Registration List - Cache of the flow IDs 
    included in the last registration message.
  ------------------------------------------------*/
  hdrbccpreg_reg_flow_type last_reg_list; 

  /* User defined in NV - Enable / Disable 
     - disables all async registration
     - periodic registration of des_unav. flows */
  boolean disable_non_required_reg;

  /* Registration Timer is always set to
     min( paging_reg_period, dyn_bcast_reg_period)

     For the registration_type associated with smaller period, we register 
     whenever the timer expires. This variable also keeps track of the number 
     of smaller periods that expired within the larger  period. 
     
     When this var equals larger_period / smaller_period, we send a registration 
     for the registration_type associated with the larger period. 
     The variable is then reset. */

} hdrbccpreg;

/*=============================================================================

                      STATIC FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================
FUNCTION HDRBCCPREG_TIME_HAS_ELAPSED

DESCRIPTION
  Helper function
       
DEPENDENCIES
  None.
  
PARAMETERS
  start_time - Absolute Start Time
  duration   - period that may have lapsed (could be 0)

RETURN VALUE
  TRUE  - current_time > start_time + duration   -- time has elapsed
  FALSE - current_time <= start_time + duration  -- time has not elapsed yet
  
SIDE EFFECTS
  None.
=============================================================================*/
STATIC boolean hdrbccpreg_time_has_elapsed
( 
  qword start_time, 
  uint32 duration
)
{
   qword exp_time;
   qword curr_time;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   time_get_ms(curr_time);

   /* exp_time = start_time + duration */
   qw_equ(exp_time, start_time);
   qw_inc(exp_time, duration);

   /* if curr_time > exp_time , time has elapsed */
   return ( ( qw_cmp(curr_time, exp_time) == 1 ) ? TRUE : FALSE );

} /* hdrbccpreg_time_has_elapsed() */
/*===========================================================================

FUNCTION HDRBCCPREG_REG_TX_OUTCOME_CB 

DESCRIPTION
  This function tells BCCPREG that the Registration message was sent.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - Pointer to data associated with this callback
  err_val - status of the message transmission.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

STATIC void hdrbccpreg_reg_tx_outcome_cb 
( 
  void *data_ptr,
  hdrerrno_enum_type err_val
)
{
  hdrbccps_state_enum_type cur_state = hdrbccps_get_state(); /* Current state */

  hdrbccp_flow_buf_type * q_buf_ptr = NULL; /* Iterator */
  qword curr_time = {0,0};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!hdrbccpreg.last_reg_list.awaiting_slp_cb)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,"Not awaiting SLP cb for reg msg.");
    return;
  }

  switch (err_val)
  {

    case E_SUCCESS:
    {
      HDRTRACE_TXMSG_ACKED( HDRHAI_BROADCAST_PROTOCOL,
                            HDRBCCPREG_REGISTRATION_MSG, cur_state); //lint !e506
        
      time_get_ms(curr_time);
        
      /* Point to the first element in the flow list queue */
      q_buf_ptr = (hdrbccp_flow_buf_type *)
                  q_check(&hdrbccp_flow_info.flow_des_priority_q);

      while (q_buf_ptr != NULL)
      {
        if (q_buf_ptr->is_in_last_reg)
        {
          qw_equ(q_buf_ptr->last_reg_time, curr_time);
          q_buf_ptr->is_in_last_reg = FALSE;
#ifdef FEATURE_HDR_BCMCS_2p0
          #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
        }
        
        /* Move pointer to the next item in the queue */
        q_buf_ptr = (hdrbccp_flow_buf_type *)
                    q_next(&hdrbccp_flow_info.flow_des_priority_q, 
                           &q_buf_ptr->link);
      }
    }
    break;

    default:
    {
      if (err_val == E_ABORTED)
      {
        HDRTRACE_TXMSG_ABORTED(HDRHAI_BROADCAST_PROTOCOL,
        HDRBCCPREG_REGISTRATION_MSG, cur_state); /*lint !e506 */
      }
      else if (err_val == E_FAILURE)
      {
        HDRTRACE_TXMSG_FAILED(HDRHAI_BROADCAST_PROTOCOL,
        HDRBCCPREG_REGISTRATION_MSG, cur_state); /*lint !e506 */
      }

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Sending BFR failure:%d", err_val);
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
    }
    break;
  } /* end switch */

  hdrbccpreg.last_reg_list.awaiting_slp_cb = FALSE;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

} /* hdrbccpreg_reg_tx_outcome_cb */ //lint !e715 data_ptr not used

/* <EJECT> */
/*=============================================================================
FUNCTION HDRBCCPREG_START_REGISTRATION

DESCRIPTION
  This function first starts the broadcast registration timer for the current
  subnet.
      
DEPENDENCIES
  None.
      
PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccpreg_start_registration(void)
{
  /* Larger registration period (dynamic or paging) */
  uint32 larger_reg_interval_ms;
  /* Most Significant 16 Bits of Larger registration period */
  uint16 larger_reg_interval_ms_msb;
  /* Least Significant 16 Bits of Larger registration period */
  uint16 larger_reg_interval_ms_lsb;
 
  /* random start value */
  uint32 random_start_val_ms = 0;
  /* Most Significant 16 Bits of Random Start Value */
  uint16 random_start_val_ms_msb = 0;
  /* Least Significant 16 Bits of Random Start Value */
  uint16 random_start_val_ms_lsb;

  uint32 dyn_bc_reg_period_ms; /* Dynamic Broadcast Registration Interval */
  uint32 paging_reg_period_ms; /* Paging registration Interval */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Don't restart registration if the reg timer is already running 
     (hdrbccpreg.curr_subnet_idx = 0) ) */
  if (rex_get_timer(&hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].reg_timer) != 0)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED,
                   "BCMCS: reg. timer [%d] already running, ignore",
                   hdrbccpreg.curr_subnet_idx);
    return;
  }

  HDR_MSG_PROT(MSG_LEGACY_MED,"BCMCS: start registration");

  hdrbccpreg.last_reg_list.awaiting_slp_cb = FALSE;

  /*---------------------------------------------------------------------------
  Retrieve the monitor period info.
  ---------------------------------------------------------------------------*/
  hdrbccpreg.reg_info.monitor_period_ms = hdrbccpc_get_curr_monitor_period();

  if (hdrbccpc_monitor_period_enabled())
  {
    hdrbccpreg.reg_info.monitor_period_ms = 
      HDRBCCPREG_PERIOD_TO_80MS_CONV(hdrbccpreg.reg_info.monitor_period_ms);
  }

  /*---------------------------------------------------------------------------
  Compute the smaller timer registration period.
  ---------------------------------------------------------------------------*/
  hdrbccpreg.reg_info.reg_type = HDRBCCPREG_REGN_NONE;

  dyn_bc_reg_period_ms = hdrbccpc_get_curr_dyn_bc_reg_period();
  paging_reg_period_ms = hdrbccpc_get_curr_paging_reg_period();

                     
  /* Compute the Dynamic Broadcast Registration Timer Period, if enabled */  
  if ( hdrbccpc_dyn_bc_reg_enabled() )
  {
    dyn_bc_reg_period_ms = HDRBCCPREG_PERIOD_TO_80MS_CONV(dyn_bc_reg_period_ms);
    hdrbccpreg.reg_info.reg_type |= HDRBCCPREG_REGN_DYN_BCAST;

    HDR_MSG_PROT_1(MSG_LEGACY_MED,"Dyn BC reg enabled (period %dms)",
                   dyn_bc_reg_period_ms);
  }

  /* Compute the Paging Registration Timer Period, if enabled */
  if ( hdrbccpc_paging_reg_enabled() )
  {
    paging_reg_period_ms = HDRBCCPREG_PERIOD_TO_80MS_CONV(paging_reg_period_ms);
    hdrbccpreg.reg_info.reg_type |= HDRBCCPREG_REGN_PAGING;
    
    HDR_MSG_PROT_1(MSG_LEGACY_MED,"Paging BC reg enabled (period %dms)",
                   paging_reg_period_ms);
  }

  /* Determine which type of registration is done more often 
    (Paging or Dynamic) */
  if (hdrbccpreg.reg_info.reg_type == HDRBCCPREG_REGN_BOTH)
  {
    if (paging_reg_period_ms < dyn_bc_reg_period_ms)
    {
      hdrbccpreg.reg_info.smaller_reg_interval_type = HDRBCCPREG_REGN_PAGING;
      hdrbccpreg.reg_info.smaller_reg_interval_ms = paging_reg_period_ms;
      
      hdrbccpreg.reg_info.total_smaller_reg_intervals =  
        (dyn_bc_reg_period_ms / paging_reg_period_ms);
      larger_reg_interval_ms = dyn_bc_reg_period_ms;
    }
    else
    {
      hdrbccpreg.reg_info.smaller_reg_interval_type = HDRBCCPREG_REGN_DYN_BCAST;
      hdrbccpreg.reg_info.smaller_reg_interval_ms = dyn_bc_reg_period_ms;
      
      hdrbccpreg.reg_info.total_smaller_reg_intervals =  
        (paging_reg_period_ms / dyn_bc_reg_period_ms);
      larger_reg_interval_ms = paging_reg_period_ms;
    }
  }
  else if (hdrbccpreg.reg_info.reg_type == HDRBCCPREG_REGN_PAGING)
  {
    hdrbccpreg.reg_info.smaller_reg_interval_type = HDRBCCPREG_REGN_PAGING;
    hdrbccpreg.reg_info.smaller_reg_interval_ms = paging_reg_period_ms;
    
    hdrbccpreg.reg_info.total_smaller_reg_intervals = HDRBCCPREG_MIN_REG_INTERVAL;
    larger_reg_interval_ms = paging_reg_period_ms;
  }
  else /* hdrbccpreg.reg_info.reg_type == HDRBCCPREG_REGN_DYN_BCAST */
  {
    hdrbccpreg.reg_info.smaller_reg_interval_type = HDRBCCPREG_REGN_DYN_BCAST;
    hdrbccpreg.reg_info.smaller_reg_interval_ms = dyn_bc_reg_period_ms;

    hdrbccpreg.reg_info.total_smaller_reg_intervals = HDRBCCPREG_MIN_REG_INTERVAL;
    larger_reg_interval_ms = dyn_bc_reg_period_ms;
  }

  /*---------------------------------------------------------------------------
  From C.S0054v1.0 section 2.4.4.3.3.1
  Whenever the access terminal initializes RegistrationEnabled to YES, or 
  changes RegistrationEnabled from NO to YES, it shall set RegistrationTimer to
  a pseudorandom number between zero and 
  Max[2^paging_reg_period , 2^dyn_bc_reg_period] x 80ms.
  ---------------------------------------------------------------------------*/

  /* rand_dist() has a maximum range of 16 bits. Since larger_reg_interval_ms
  is 32 bits, we may need to call it twice */
  larger_reg_interval_ms_lsb = (uint16) (larger_reg_interval_ms & 0xFFFF);

  if (larger_reg_interval_ms > 0xFFFF)
  {
   larger_reg_interval_ms_msb = (uint16) (larger_reg_interval_ms >> 16);
   random_start_val_ms_lsb = ran_dist(hdrutil_get_random(), 0, 0xFFFF);
   random_start_val_ms_msb = ran_dist(hdrutil_get_random(), 0, larger_reg_interval_ms_msb);
  }
  else
  {
   random_start_val_ms_lsb = ran_dist(hdrutil_get_random(), 0, larger_reg_interval_ms_lsb);
  }

  random_start_val_ms = (random_start_val_ms_msb << 16) | random_start_val_ms_lsb;

  /* This many smaller reg periods should be assumed to have been elapsed */
  hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].smaller_reg_periods_cnt = 
    random_start_val_ms / hdrbccpreg.reg_info.smaller_reg_interval_ms;

  /*---------------------------------------------------------------------------
    Start the registration timer
  ---------------------------------------------------------------------------*/
  HDR_MSG_PROT_5(MSG_LEGACY_HIGH,
    "Start reg timer (idx%d ran_start %dms mod sm_regint %dms) = %dms reg_cnt=%d",
    hdrbccpreg.curr_subnet_idx,
    random_start_val_ms, 
    hdrbccpreg.reg_info.smaller_reg_interval_ms,
    random_start_val_ms % hdrbccpreg.reg_info.smaller_reg_interval_ms,
    hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].smaller_reg_periods_cnt);

  (void) rex_set_timer(&hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].reg_timer, 
                (random_start_val_ms % hdrbccpreg.reg_info.smaller_reg_interval_ms)
               );

} /* hdrbccpreg_start_registration() */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPREG_STOP_REGISTRATION

DESCRIPTION
  Stops Broadcast Registration for the current subnet.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrbccpreg_stop_registration(void)
{
   
  if (rex_get_timer(&hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].reg_timer) != 0)
  {
    /* Stop the registration timer for our current subnet */
    (void) rex_clr_timer(&hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].reg_timer);
    HDR_MSG_PROT_1(MSG_LEGACY_MED,"Stop registration timer [%d]",
                   hdrbccpreg.curr_subnet_idx);
  }

} /* hdrbccpreg_stop_registration */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPREG_CONFIGURE_REGISTRATION

DESCRIPTION
  This function updates registration related configuration parameters.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Dynamic and/or paging registration may be started or stopped depending on the 
  value of the parameters configured during session negotiation.

=============================================================================*/
void hdrbccpreg_configure_registration( void )
{

  if ( hdrbccpc_registration_enabled() )
  {
    hdrbccpreg_start_registration();
  }
  else
  {
    hdrbccpreg_stop_registration();
  }

} /* hdrbccpreg_configure_registration() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRBCCPREG_PACK_FORMAT_0_REG

DESCRIPTION
  This function packs the format 0 portion of a registration message.
  
DEPENDENCIES
  None.
  
PARAMETERS
  send_msg_ptr - send message pointer
  bcmcs_flow_id_length - flowID length
  offset       - offset into the 
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/

void hdrbccpreg_pack_format_0_reg
(
  dsm_item_type * send_msg_ptr, 
  uint8 bcmcs_flow_id_length,
  uint16 offset
)
{
  uint8 flow = 0; /* iterator */
  uint8  reserved_field_length = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
    BCMCSFlowCount
  ---------------------------------------------------------------------------*/
  HDRBIT_PACK8((send_msg_ptr),hdrbccpreg.last_reg_list.flow_count, offset,
                HDRBCCPREG_BCMCS_FLOW_COUNT_SIZE);
  offset += HDRBCCPREG_BCMCS_FLOW_COUNT_SIZE;

  for (flow = 0; flow < hdrbccpreg.last_reg_list.flow_count; flow++)
  {
    /*-------------------------------------------------------------------------
      BCMCSFlowID
    -------------------------------------------------------------------------*/
    HDRBIT_PACK32((send_msg_ptr),
                  (uint32)hdrbccpreg.last_reg_list.flow_id_list[flow].flow_id,
                  offset,
                   bcmcs_flow_id_length * HDRBCCPREG_BCMCS_FLOW_ID_SIZE );
    offset += bcmcs_flow_id_length * HDRBCCPREG_BCMCS_FLOW_ID_SIZE;

    HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"BCAST Adding flowId = %d to reg msg",
                   hdrbccpreg.last_reg_list.flow_id_list[flow].flow_id);

    /*-------------------------------------------------------------------------
      AuthSignatureIncluded 0 = No
    -------------------------------------------------------------------------*/
    HDRBIT_PACK8((send_msg_ptr),0,offset,
                 HDRBCCPREG_AUTH_SIGNATURE_INCLUDED_SIZE);
    offset += HDRBCCPREG_AUTH_SIGNATURE_INCLUDED_SIZE;

    /*-------------------------------------------------------------------------
      BAKSequenceNumber (not included)
    -------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------
      AuthSignature (not included)
    -------------------------------------------------------------------------*/
  }

  /* If offset is a multiple of 8 
    (meaning it occupies integer number of octets) */
  reserved_field_length = 8 - (offset %8);

  /* If reserved_field_length is exactly eight, then we don't need any filling.
     Otherwise, we do. */
  if (reserved_field_length < 8 )
  {
    /*-------------------------------------------------------------------------
      Reserved field
    -------------------------------------------------------------------------*/
    HDRBIT_PACK8((send_msg_ptr),0, offset,reserved_field_length);
    offset += reserved_field_length;
  }

  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "BCMCS reg - format0, msglen=%d, flow_cnt=%d",
                 offset, 
                 hdrbccpreg.last_reg_list.flow_count);

} /* hdrbccpreg_pack_format_0_reg() */
/* <EJECT> */
/*===========================================================================
FUNCTION HDRBCCPREG_PACK_FORMAT_1_REG

DESCRIPTION
  This function packs the format 1 portion of a registration message.
  
DEPENDENCIES
  None.
  
PARAMETERS
  send_msg_ptr - send message pointer
  offset       - offset into the 
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/

void hdrbccpreg_pack_format_1_reg
(
  dsm_item_type * send_msg_ptr, 
  uint16 offset
)
{
  uint8 program_count = 0;

  struct
  {
    uint8  num_flows; /* number of flows to register in this program */
    uint8  program_id_len;
    uint32 program_id;
  } programs[HDRBCCPI_BCMCS_MAX_FLOWS];
    /* Keep a list of all programs */

  uint8  reserved_field_length = 0;

  uint8 pidx; /* Program ID Index - iterator */
  uint8 fidx; /* Flow Index - iterator */
  uint8 flow_cnt = 0;

  hdrbccp_flow_type *flow_ptr; /* convenience pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(hdrbccpreg.last_reg_list.flow_count <= HDRBCCPI_BCMCS_MAX_FLOWS);

  memset(programs,0,sizeof(programs));

  /*---------------------------------------------------------------------------
     The last reg list is not sorted by program.  We need to figure out the 
     number of different programs the requested flows belong to. 
     We compile a program list and track the number of flows associated with 
     each program that need registration.
  ---------------------------------------------------------------------------*/
  flow_ptr = &(hdrbccpreg.last_reg_list.flow_id_list[0]);

  programs[0].program_id = flow_ptr->program_id;
  programs[0].program_id_len = flow_ptr->program_id_len;
  programs[0].num_flows++;
  program_count++;

  /* loop through all the flows that need registration and find all distinct
     programs */
  for (fidx=1; fidx < hdrbccpreg.last_reg_list.flow_count; fidx++)
  {
    flow_ptr = &(hdrbccpreg.last_reg_list.flow_id_list[fidx]);

    for (pidx=0; pidx < program_count; pidx++)
    {
      if ( (flow_ptr->program_id == programs[pidx].program_id) &&
           (flow_ptr->program_id_len == programs[pidx].program_id_len)
         )
      {
        /* we've found another flow that belongs to an existing program */
        programs[pidx].num_flows++;
        break;
      }
    }

    /* if we haven't found this programID in the program ID list, add it */
    if (pidx == program_count)
    {
      programs[program_count].program_id = flow_ptr->program_id;
      programs[program_count].program_id_len = flow_ptr->program_id_len;
      programs[program_count].num_flows++;
      program_count++;
    }
  }

  /* We can pack the message now that we know how many different programs the
     flows we are currently registering belong to. */

  /*---------------------------------------------------------------------------
    ProgramCount
  ---------------------------------------------------------------------------*/
  HDRBIT_PACK8((send_msg_ptr),program_count, offset,
                HDRBCCPREG_BCMCS_PROG_CNT_SIZE);
  offset += HDRBCCPREG_BCMCS_PROG_CNT_SIZE;

  for (pidx = 0; pidx < program_count; pidx++)
  {
    /*-------------------------------------------------------------------------
      ProgramIDLSBLength (in bits)
    -------------------------------------------------------------------------*/
    HDRBIT_PACK8( (send_msg_ptr), programs[pidx].program_id_len, offset,
                   HDRBCCPREG_BCMCS_PROG_ID_LSB_LGTH_SIZE );
    offset += HDRBCCPREG_BCMCS_PROG_ID_LSB_LGTH_SIZE;

    /*-------------------------------------------------------------------------
      ProgramIDLSBs
    -------------------------------------------------------------------------*/
    HDRBIT_PACK32( (send_msg_ptr), programs[pidx].program_id, offset,
                   programs[pidx].program_id_len );
    offset += programs[pidx].program_id_len;

    /*------------------------------------------------------------------------
     ** Note ** According to the bc standard:
      " If the access terminal is registering for all Flow Discriminators 
      associated with this ProgramID, then the access terminal shall set this 
      field to '000000' ".  Currently, the AT doesn't check to see if it is 
      registering for the entire program ID - it always lists the individual
      flows in the registration.
      -  If the AT is registering for Desired_Unavailable flows, then it 
      couln't tell anyway because there is no bcovhd msg to compare against.
      - Otherwise (TODO), the AT would need to compare the bcovhd program
      content against the program content of the flows it is currently 
      registering for.  If they match, then register for the whole program 
      instead.
      - What if the FlowDiscriminatorLength is '0' in the bcdb?  Then we should
      probably assume that we are dealing with a whole program.  (TODO?)
    -------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------
      FlowDiscriminatorCount
    -------------------------------------------------------------------------*/
    HDRBIT_PACK8( (send_msg_ptr), programs[pidx].num_flows, offset,
                   HDRBCCPREG_BCMCS_FLOW_DISCR_CNT_SIZE );
    offset += HDRBCCPREG_BCMCS_FLOW_DISCR_CNT_SIZE;

    flow_cnt = 0;

    /* Find all the flows that below to this program */
    for (fidx=0; fidx < hdrbccpreg.last_reg_list.flow_count; fidx++)
    {

      if ( (programs[pidx].program_id == flow_ptr->program_id) &&
           (programs[pidx].program_id_len == flow_ptr->program_id_len)
         )
      {
        flow_ptr = &(hdrbccpreg.last_reg_list.flow_id_list[fidx]);

        if (flow_cnt == 0)
        {
          /*-------------------------------------------------------------------
            FlowDiscriminatorLength
          -------------------------------------------------------------------*/
          HDRBIT_PACK8( (send_msg_ptr), flow_ptr->flow_discrim_len, offset,
                         HDRBCCPREG_BCMCS_FLOW_DISCR_LGTH_SIZE );
          offset += HDRBCCPREG_BCMCS_FLOW_DISCR_LGTH_SIZE;

          /*-------------------------------------------------------------------
            AuthSignatureIncluded1 (not included since flow disc len != 0)
          -------------------------------------------------------------------*/
          /*-------------------------------------------------------------------
            BAKSequenceNumber1 (not included)
          -------------------------------------------------------------------*/
          /*-------------------------------------------------------------------
            AuthSignature1 (not included)
          -------------------------------------------------------------------*/

          flow_cnt++;
        }

        /*---------------------------------------------------------------------
          FlowDiscriminator
        ---------------------------------------------------------------------*/
        HDRBIT_PACK8( (send_msg_ptr), flow_ptr->flow_discrim, offset,
                       flow_ptr->flow_discrim_len );
        offset += flow_ptr->flow_discrim_len;

        /*---------------------------------------------------------------------
          AuthSignatureIncluded2 (0 - no auth sig)
        ---------------------------------------------------------------------*/
        HDRBIT_PACK8( (send_msg_ptr), 0, offset,
                       HDRBCCPREG_AUTH_SIGNATURE_INCLUDED_SIZE );
        offset += HDRBCCPREG_AUTH_SIGNATURE_INCLUDED_SIZE;

        /*---------------------------------------------------------------------
          BAKSequenceNumber2 (not included)
        ---------------------------------------------------------------------*/
        /*---------------------------------------------------------------------
          AuthSignature2 (not included)
        ---------------------------------------------------------------------*/

        /* If we've found all the flows associated with this program, move
           on to the next program if any. */
        if (flow_cnt == programs[pidx].num_flows)
        {
          break;
        }
      } /* end if - found flow for program */

    } /* for - last reg list*/                                                   
  } /* for - program list */

  /* If offset is a multiple of 8 
     (meaning it occupies integer number of octets) */
  reserved_field_length = 8 - (offset %8);

  /* If reserved_field_length is exactly eight, then we don't need any filling.
     Otherwise, we do. */
  if (reserved_field_length < 8 )
  {
    /*-------------------------------------------------------------------------
      Reserved field
    -------------------------------------------------------------------------*/
    HDRBIT_PACK8((send_msg_ptr),0, offset,reserved_field_length);
    offset += reserved_field_length;
  }

  HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                 "BCMCS reg - format1 msglen=%d, pgm_cnt=%d, total_num_flows=%d",
                 offset, 
                 program_count,
                 hdrbccpreg.last_reg_list.flow_count);

} /* hdrbccpreg_pack_format_1_reg */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRBCCPREG_SEND_REGISTRATION_MESSAGE 

DESCRIPTION
  This function packs and sends a registration message.
  
DEPENDENCIES
  None.
  
PARAMETERS
  bundled_msg_mask - message(s) to bundle the FRM with, if any.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
STATIC void hdrbccpreg_send_registration_message 
(
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#else
  void
#endif /* FEATURE_HDR_BCMCS_2p0 */
)
{
  dsm_item_type * send_msg_ptr; 
  uint16 offset = 0; /* Bit offset from start of message to current field */
  uint8  bcmcs_flow_id_length; /* Flow ID Length from BroadcastOverhead msg */
  uint8  bcmcs_flow_format; /* Flow format from BroadcastOverhead msg */
#ifdef FEATURE_HDR_QCHAT
  uint8	 flow;				   /* Generate MATI */
#endif /*FEATURE_HDR_QCHAT*/   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbccpreg.last_reg_list.flow_count == 0)
  {
    /* For timer based registrations,  is considered successful 
      even with zero flows. */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "BCAST reg flow_cnt = 0");
    return;
  }

  send_msg_ptr = hdrutil_new_msg_buffer();
  
  /*---------------------------------------------------------------------------
    MessageID 0x00
  ---------------------------------------------------------------------------*/
  HDRBIT_PACK8((send_msg_ptr),0,offset,HDRBCCPREG_MSG_ID_SIZE);
  offset += HDRBCCPREG_MSG_ID_SIZE;


  /*---------------------------------------------------------------------------
    TimeStampShortIncluded (0 = No)
  ---------------------------------------------------------------------------*/
  HDRBIT_PACK8((send_msg_ptr),0,offset,HDRBCCPREG_TIMESTAMP_SHORT_INCLUDED_SIZE);
  offset += HDRBCCPREG_TIMESTAMP_SHORT_INCLUDED_SIZE;

  /*---------------------------------------------------------------------------
    TimeStampShortLength (not included)
  ---------------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------
    TimeStampShort (not included)
  ---------------------------------------------------------------------------*/
  
  /*---------------------------------------------------------------------------
    BCMCSFlowFormat
  ---------------------------------------------------------------------------*/
  if (hdrbcovhd_get_flow_format(&bcmcs_flow_format) == E_FAILURE)
  {
    /* Assuming that all requested flows have the same format */
    bcmcs_flow_format = hdrbccpreg.last_reg_list.flow_id_list[0].flow_format;

    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "No BroadcastOverhead message rxed yet. Use format %d",
                    bcmcs_flow_format );
  }

  HDRBIT_PACK8((send_msg_ptr),bcmcs_flow_format,
                offset,HDRBCCPREG_BCMCS_FLOW_FORMAT_SIZE);
  offset += HDRBCCPREG_BCMCS_FLOW_FORMAT_SIZE;

  /*---------------------------------------------------------------------------
    BCMCSFlowIDLength
    As received in overhead message. 
    It is one less than what we store in the structure
  ---------------------------------------------------------------------------*/
  bcmcs_flow_id_length = hdrbcovhd_get_flow_id_len();
  if (bcmcs_flow_id_length == 0)
  {
#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#else
    bcmcs_flow_id_length = 4;
#endif /* FEATURE_HDR_BCMCS_2p0 */
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "No BroadcastOverhead message rxed yet.  Use flow_id_len %d",
                    bcmcs_flow_id_length);
  }
  HDRBIT_PACK8( (send_msg_ptr), bcmcs_flow_id_length - 1,
               offset,HDRBCCPREG_BCMCS_FLOW_ID_LENGTH_SIZE);
  offset += HDRBCCPREG_BCMCS_FLOW_ID_LENGTH_SIZE;

#ifdef FEATURE_HDR_QCHAT
  if (hdrqchatupk_mcs_enabled())
  {
    for (flow = 0; flow < hdrbccpreg.last_reg_list.flow_count; flow++)
    {         
      hdrqchatupk_bccpreg_process_reg_list(ntohl(hdrbccpreg.last_reg_list.flow_id_list[flow].ip_port.ip_address.addr.v4),
                                          ntohs(hdrbccpreg.last_reg_list.flow_id_list[flow].ip_port.port_number));
    }
  }
#endif /*FEATURE_HDR_QCHAT*/

  if (bcmcs_flow_format == 0)
  {
    hdrbccpreg_pack_format_0_reg(send_msg_ptr, bcmcs_flow_id_length, offset);
  }
  else
  {
    hdrbccpreg_pack_format_1_reg(send_msg_ptr, offset);
  }

  hdrbccpreg.last_reg_list.awaiting_slp_cb = TRUE;

#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#else
  hdrhmp_send_msg(HDRHAI_BROADCAST_PROTOCOL, /* Broadcast Protocol ID */
                  HDRHAI_AC_OR_RTC,
                  TRUE, /* explicitely reliable on RTC */
                  HDRBCCPREG_FLOW_REG_MSG_PRI, /* Registration Message Priority */
                  HDRHAI_IN_USE, /* Instance of protocol sending message */
                  send_msg_ptr,
                  hdrbccpreg_reg_tx_outcome_cb,
                  NULL
                  );
#endif /* FEATURE_HDR_BCMCS_2p0 */
} /* hdrbccpreg_send_registration_message() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDRBCCPREG_POWERUP_INIT

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
void hdrbccpreg_powerup_init( void )
{

  HDR_MSG_PROT(MSG_LEGACY_HIGH,"Initializing Broadcast Protocol");

  /*-------------------------------------------------------------------------
  Define Timers
  -------------------------------------------------------------------------*/
  rex_def_timer_ex( &hdrbccpreg.subnet_info[0].reg_timer,
                    hdrbccp_timer_cb,
                    HDRBCCP_REG_TIMER_0);

  rex_def_timer_ex( &hdrbccpreg.subnet_info[1].reg_timer,
                    hdrbccp_timer_cb,
                    HDRBCCP_REG_TIMER_1);
  
  rex_def_timer_ex( &hdrbccpreg.subnet_info[2].reg_timer,
                    hdrbccp_timer_cb,
                    HDRBCCP_REG_TIMER_2);


  /*-------------------------------------------------------------------------
  NV Items to Enable / Disable registration features outside of the scope of 
  the standard.
  - Disable one time registration of newly desired flows.
  - Disable registration on subnet boundaries.
  -> Registration rules explicitely defined in the standard still apply.
  -------------------------------------------------------------------------*/
  if ( hdrutil_read_nv_item( NV_HDRBCMCS_DISABLE_ONE_TIME_REG_I,
                             (nv_item_type*)&hdrbccpreg.disable_non_required_reg) 
       != NV_DONE_S)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "BCMCS: Disable One Time Reg not active in NV");
    hdrbccpreg.disable_non_required_reg = FALSE;
  }

  if (hdrutil_read_nv_item( NV_HDRBCMCS_DISABLE_SUBNET_REG_I,
                            (nv_item_type*)&hdrbccpreg.disable_subnet_reg) 
      != NV_DONE_S)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "BCMCS: Disable Subnet Reg not active in NV");
    hdrbccpreg.disable_subnet_reg = TRUE;
  }

  hdrbccpreg.curr_subnet_idx = 0;

} /* hdrbccpreg_powerup_init() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDRBCCPREG_GET_CURRENT_SUBNET

DESCRIPTION
  
DEPENDENCIES
  None.

PARAMETERS
  subnet_ptr- Pointer to allocated memory where the subnet will get copied.

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - otherwise 
  
PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrbccpreg_get_current_subnet
(
  hdraddr_type subnet_ptr
)
{

  if (hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].is_valid)
  {
    hdraddr_set(subnet_ptr,
                hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].subnet);
    return E_SUCCESS;
  }

  ERR("Invalid Subnet info!",0,0,0);
  return E_NO_DATA;

} /* hdrbccpreg_get_current_subnet() */

#ifndef PERLUTF_TDTGEN_EXCLUDE
/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCCPREG_FLOW_IS_PURGEABLE

DESCRIPTION
   This function determines if a flow can be removed from the undesired flow 
   list. A flow is considered useful to keep around, if it was last registered  
   or monitored "recently".
     
DEPENDENCIES
  None.

PARAMETERS
  flow_ptr - Flow entry pointer

RETURN VALUE
  TRUE  - this flow has outlived its useful period
  FALSE - otherwise

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrbccpreg_flow_is_purgeable
(                                         
  hdrbccp_flow_buf_type *flow_ptr
)
{
   dword valid_reg_duration = 0;
   dword valid_mon_duration = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* If the flow was registered within this duration it
   is still useful to keep around
   */
   valid_reg_duration = hdrbccpreg.reg_info.smaller_reg_interval_ms * 
                          hdrbccpreg.reg_info.total_smaller_reg_intervals;

   /* If the flow was monitored within this duration it is still useful
   to keep around*/
   valid_mon_duration = hdrbccpreg.reg_info.monitor_period_ms;

   /* Last registration older than valid_reg_duration? 
   AND last monitored_time older than mon_period? - If so this flow
   can be purged*/
   if ( (hdrbccpreg_time_has_elapsed(flow_ptr->last_reg_time,
                                       valid_reg_duration) == TRUE ) &&
        (hdrbccpreg_time_has_elapsed(flow_ptr->last_mon_time,
                                         valid_mon_duration) == TRUE )

       )
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }

} /* hdrbccpreg_flow_is_purgeable() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCCPREG_PERIODIC_FLOW_REGISTRATION

DESCRIPTION
   Periodic registration of monitored and recently monitored flows ( for
   paging and dynamic broadcast). Periodic registration of unavailable flows..
  
DEPENDENCIES
  None.

PARAMETERS
  reg_type - Type of registration to do (Paging, Dynamic or Both)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccpreg_periodic_flow_registration
(
  uint8 reg_type
)
{
  hdrbccp_flow_buf_type * q_buf_ptr = NULL; /* Iterator */

  boolean reg_for_paging; /* Register for paging set for flow? */
  boolean reg_for_dyn_bc; /* Register for dynamic broadcast set for flow? */

  uint32 flow_reg_interval = 0;  /* NumPeriods x 2^MonitorPeriod x 80ms */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( hdrbccps_get_state() != HDRBCCPS_OPEN_STATE ) 
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Can't do registration while HDR not acquired or inactive");
    return;
  }

  if ( !hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) )
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Can't register until session is opened");
    return;
  }

  if ( !hdrbccpc_registration_enabled() )
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Flow Registration disabled.");
    return;
  }

  if ( hdrbccpreg.last_reg_list.awaiting_slp_cb )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Already awaiting an SLP cb on the current reg msg, skip");
    return;
  }

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Periodic Registration (type: %d)", reg_type);

  /* Initialize registration list*/
  memset(&hdrbccpreg.last_reg_list,0,sizeof(hdrbccpreg.last_reg_list));
  
  /* Compute the Flow Registration Interval (for DESIRED_UNAVAILABLE flows):
     [ NumPeriods x (2^MonitorPeriod) x 80 ]
   */
  if (hdrbccpc_monitor_period_enabled())
  {
    flow_reg_interval =  hdrbccpc_get_curr_num_monitor_period() * 
                         hdrbccpreg.reg_info.monitor_period_ms;
  }

  /* Point to the first element in the flow list queue */
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check(&hdrbccp_flow_info.flow_des_priority_q);

  while (q_buf_ptr != NULL)
  {
    if (hdrbcovhd_get_flow_reg_info( q_buf_ptr->flow_info.flow_id, 
                                     &reg_for_paging, 
                                     &reg_for_dyn_bc ) != E_SUCCESS)
    {
      reg_for_paging = FALSE;
      reg_for_dyn_bc = FALSE;
    }

    /* Initialize flag to FALSE (determines whether this desired flow is
       included in the current registration) */
    q_buf_ptr->is_in_last_reg = FALSE;

    /*------------------------------------------------------------------------- 
       For flows available in ovhd, skip a flow if:
       - the current Registration Type is not allowed for this flow.

       OR

       [ 
         - it WAS included in any flow reg message during the past 
           [NumPeriods x 2^MonitorPeriod x 80ms] 

         AND

         - it WAS NOT monitored within [2^MonitorPeriod x 80ms]
       ]

       Otherwise, include it.
    -------------------------------------------------------------------------*/
    switch (q_buf_ptr->state)
    {
      /*----------------------------------------------------------------------- 
       Flow is currently monitored, set the last monitored time to the current
       time.  This will unconditionally add the flow to the list
      ----------------------------------------------------------------------- */
      case HDRBCCPFS_DESIRED_MONITORED:
      {
        time_get_ms(q_buf_ptr->last_mon_time);

        /* We are currently monitoring this flow, we can register for it if the 
           registration type is enabled for this flow */
        if ( (reg_for_paging && (reg_type & HDRBCCPREG_REGN_PAGING)) ||
             (reg_for_dyn_bc && (reg_type & HDRBCCPREG_REGN_DYN_BCAST)) )
        {
          hdrbccpreg.last_reg_list.flow_id_list[hdrbccpreg.last_reg_list.flow_count++] = 
            q_buf_ptr->flow_info;
    
          q_buf_ptr->is_in_last_reg = TRUE;
        }
      }
      break;

      /*----------------------------------------------------------------------- 
         Flow is not currently monitored but available on another frequency, or
         it is available on the current frequency but we can't monitor it because
         we've reach our max monitored flow limit.
         Check to see if it was monitored recently or not recently included in
         a registration message before adding it to the registration list. 
      -----------------------------------------------------------------------*/
      case HDRBCCPFS_DESIRED_UNMONITORED:
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
      {
        /* First, check that registration is allowed in bcovhd for this flow. */
        if ( (reg_for_paging && (reg_type & HDRBCCPREG_REGN_PAGING)) ||
             (reg_for_dyn_bc && (reg_type & HDRBCCPREG_REGN_DYN_BCAST)) )
        {

          /* Registration _is_ permitted.

             "The access terminal shall not include in a BCMCSFlowRegistration 
             message any BCMCS Flows that were included in any BCMCS Flow 
             registration during the past [NumPeriods x 2^MonitorPeriod x 80ms]
             interval and were NOT monitored by the access terminal during the 
             past [2^MonitorPeriod x 80ms] interval." 

             If MonitorPeriod is disabled - don't include.
          */
          if ( hdrbccpc_monitor_period_enabled() &&
               (
                 /* Monitored recently OR */
                 !hdrbccpreg_time_has_elapsed( q_buf_ptr->last_mon_time, 
                                               hdrbccpreg.reg_info.monitor_period_ms) ||
                 /* Not recently included in reg msg */
                 hdrbccpreg_time_has_elapsed( q_buf_ptr->last_reg_time,
                                              flow_reg_interval)
               )
             )
          {
            hdrbccpreg.last_reg_list.flow_id_list[hdrbccpreg.last_reg_list.flow_count++] = 
              q_buf_ptr->flow_info;

            q_buf_ptr->is_in_last_reg = TRUE;
          }

        }
      }
      break;

      /*----------------------------------------------------------------------- 
         Flow is not monitored and ovhd info unavailable.
         Only include the flow if Dynamic Broadcast Registration is enabled,
         and the flow was not recently included in a registration message.
      -----------------------------------------------------------------------*/
      case HDRBCCPFS_DESIRED_UNAVAILABLE:
      {
        if ( /* This is the registration for dynamic broadcast timer exp. */
             (reg_type & HDRBCCPREG_REGN_DYN_BCAST) &&
             /* Skip if non required registrations are disabled */
             (!hdrbccpreg.disable_non_required_reg) &&
#ifdef FEATURE_HDR_BCMCS_2p0
             #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
             /* Skip if monitor period is disabled */
             hdrbccpc_monitor_period_enabled() &&
             /* Not recently included in reg msg 
                (within [ NumPeriods x (2^MonitorPeriod) x 80 ]) */
             hdrbccpreg_time_has_elapsed( q_buf_ptr->last_reg_time,
                                          flow_reg_interval)
           )
        {
          hdrbccpreg.last_reg_list.flow_id_list[hdrbccpreg.last_reg_list.flow_count++] = 
            q_buf_ptr->flow_info;

          q_buf_ptr->is_in_last_reg = TRUE;
        }
        
      }
      break;

      default:
      {
        ERR("Unexpected state in des flow list",q_buf_ptr->state,0,0);
      }
      break;
    }

    /* Move pointer to the next item in the queue */
    q_buf_ptr = (hdrbccp_flow_buf_type *)
                q_next(&hdrbccp_flow_info.flow_des_priority_q, &q_buf_ptr->link);
  }

  /*--------------------------------------------------------------------------- 
  Send the registration message.
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#else
  hdrbccpreg_send_registration_message();
#endif /* FEATURE_HDR_BCMCS_2p0 */
} /* hdrbccpreg_periodic_flow_registration() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCCPREG_ASYNC_FLOW_REGISTRATION

DESCRIPTION
   Async flow registrations that are not required but allowed by the std.
     
DEPENDENCIES
  None.

PARAMETERS
  TRUE - Registration after a subnet change
  FALSE - Async Registration ()
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccpreg_async_flow_registration 
(
  boolean subnet_changed
)
{
  hdrbccp_flow_buf_type * q_buf_ptr = NULL; /* Iterator */

  uint32 flow_reg_interval = 0;  /* NumPeriods x 2^MonitorPeriod x 80ms */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (subnet_changed && hdrbccpreg.disable_subnet_reg )
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Subnet Registration Disabled");
    return;
  }
  else if ( hdrbccpreg.disable_non_required_reg)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Non required flow registrations disabled");
    return;
  }

  if ( hdrbccps_get_state() != HDRBCCPS_OPEN_STATE ) 
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Can't do registration while HDR not acquired or inactive");
    return;
  }

  if ( !hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) )
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Can't register until session is opened");
    return;
  }

  if ( !hdrbccpc_registration_enabled() )
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Flow Registration not negotiated.");
    return;
  }

  if ( hdrbccpreg.last_reg_list.awaiting_slp_cb )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Already awaiting an SLP cb on the current reg msg, skip");
    return;
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH,"Async Flow Registration");


  /* Initialize registration list*/
  memset(&hdrbccpreg.last_reg_list,0,sizeof(hdrbccpreg.last_reg_list));
  
  /* Compute the Flow Registration Interval :
     [ NumPeriods x (2^MonitorPeriod) x 80 ]
  */
  if (hdrbccpc_monitor_period_enabled())
  {
    flow_reg_interval =  hdrbccpc_get_curr_num_monitor_period() * 
                         hdrbccpreg.reg_info.monitor_period_ms;
  }

  /* Point to the first element in the flow list queue */
  q_buf_ptr = (hdrbccp_flow_buf_type *)
              q_check(&hdrbccp_flow_info.flow_des_priority_q);

  while (q_buf_ptr != NULL)
  {

    /* Initialize flag to FALSE (determines whether this desired flow is
       included in the current registration) */
    q_buf_ptr->is_in_last_reg = FALSE;

#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
    /*------------------------------------------------------------------------- 
       Skip a flow if:
    -------------------------------------------------------------------------*/
    switch (q_buf_ptr->state)
    {
      /*----------------------------------------------------------------------- 
       Flow is currently monitored, set the last monitored time to the current
       time.        
       ----------------------------------------------------------------------- */
      case HDRBCCPFS_DESIRED_MONITORED:
      {
        time_get_ms(q_buf_ptr->last_mon_time);

        /* Registration is permitted for this flow (in session) 

           One time registration of newly monitored flow.
           - Helps AN quickly determine the AT's paging channel.
           - Tell the AN the AT is interested in this flow right away.
           - Only valid if we haven't already registered for this flow in this
             past registration period
         */
        if (!hdrbccpc_monitor_timer_enabled() ||
             hdrbccpreg_time_has_elapsed( q_buf_ptr->last_reg_time,
                                          hdrbccpreg.reg_info.smaller_reg_interval_ms) )
        {
          hdrbccpreg.last_reg_list.flow_id_list[hdrbccpreg.last_reg_list.flow_count++] = 
            q_buf_ptr->flow_info;

          q_buf_ptr->is_in_last_reg = TRUE;
        }
      }
      break;

      /*----------------------------------------------------------------------- 
         Flow is not monitored.
         Only include the flow if Dynamic Broadcast Registration is enabled,
         and the flow was not recently included in a registration message. 
      -----------------------------------------------------------------------*/
      case HDRBCCPFS_DESIRED_UNMONITORED:
      case HDRBCCPFS_DESIRED_UNAVAILABLE:
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
      {
        if ( /* Register for dynamic broadcast procedure - skip if disabled */
             hdrbccpc_dyn_bc_reg_enabled() &&
  
             /* Skip if monitor period is disabled */
             hdrbccpc_monitor_period_enabled() &&
  
             /* Skip if it was already registered for recently.
                (We don't check whether it was monitored recently here.  
                - If the flow was monitored recently, then it was already included in a
                  reg message.  
                - If it's never been monitored, we'll include it because the 
                  last reg time will be '0'.
                - If it hasn't been monitored in a long time, then check if 
                  we've included this in the past 
                  [ NumPeriods x (2^MonitorPeriod) x 80ms ]
             */
             (!hdrbccpc_monitor_timer_enabled() ||
              hdrbccpreg_time_has_elapsed( q_buf_ptr->last_reg_time,
                                           flow_reg_interval)
             )
           )
        {
          hdrbccpreg.last_reg_list.flow_id_list[hdrbccpreg.last_reg_list.flow_count++] = 
            q_buf_ptr->flow_info;

          q_buf_ptr->is_in_last_reg = TRUE;
        }
      }
      break;

      default:
      {
        ERR("Unexpected state in des flow list (%d)",q_buf_ptr->state,0,0);
      }
      break;
    }
#ifdef FEATURE_HDR_BCMCS_2p0
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

    /* Move pointer to the next item in the queue */
    q_buf_ptr = (hdrbccp_flow_buf_type *)
                q_next(&hdrbccp_flow_info.flow_des_priority_q, &q_buf_ptr->link);
  }

  /*--------------------------------------------------------------------------- 
  Send the registration message.
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#else
  hdrbccpreg_send_registration_message();
#endif /* FEATURE_HDR_BCMCS_2p0 */

} /* hdrbccpreg_async_flow_registration() */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRBCCPREG_PROCESS_REG_TIMER_EXP

DESCRIPTION
  
DEPENDENCIES
  None.

PARAMETERS
  timer_id - Which timer expired.  If this isn't the timer for the current 
             subnet, we just restart the timer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrbccpreg_process_reg_timer_exp 
( 
  uint32 timer_id
)
{
  
  hdrbccpreg_subnet_reg_info_type *c_subnet_ptr; /* Convenience pointer */
  uint8 curr_reg_type = 0; /* Type of registration to do this time around */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(timer_id < HDRBCCPREG_MAX_SUBNETS);

  c_subnet_ptr = &hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx];

  /*---------------------------------------------------------------------------
  The registration timer for the current subnet expired.
  Do periodic registration if needed.
  _mpa_ bypass subnet check - revisit if implement subnet based registration.
  ---------------------------------------------------------------------------*/
  if ( /* (c_subnet_ptr->is_valid) && */
       (hdrbccpreg.curr_subnet_idx == timer_id) )
  {
    /* Increment the number of elapsed registration periods */
    c_subnet_ptr->smaller_reg_periods_cnt++;
    
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                   "Reg Timer sig: SmallPeriods[%d]/Max[%d]",
                   c_subnet_ptr->smaller_reg_periods_cnt,
                   hdrbccpreg.reg_info.total_smaller_reg_intervals);

    ASSERT (c_subnet_ptr->smaller_reg_periods_cnt <= 
            hdrbccpreg.reg_info.total_smaller_reg_intervals);

    /* Determine which type of registration we are doing */
    if (hdrbccpreg.reg_info.total_smaller_reg_intervals == 
        c_subnet_ptr->smaller_reg_periods_cnt)
    {
      /* Reset the number of elapsed registration periods to zero if we've reached 
       max */
      c_subnet_ptr->smaller_reg_periods_cnt = 0;

      curr_reg_type = hdrbccpreg.reg_info.reg_type;
    }
    else
    {
      curr_reg_type = hdrbccpreg.reg_info.smaller_reg_interval_type;
    }

    /* Save the timer expiry for our current type of registration */
    if (curr_reg_type & HDRBCCPREG_REGN_PAGING)
    {
      time_get_ms(c_subnet_ptr->last_paging_timer_expiry);
    }

    if (curr_reg_type & HDRBCCPREG_REGN_DYN_BCAST)
    {
      time_get_ms(c_subnet_ptr->last_dyn_timer_expiry);
    }

    hdrbccpreg_periodic_flow_registration(curr_reg_type);

  } /* end if current valid subnet */

  /*---------------------------------------------------------------------------
  Restart the Registration Timer
  ---------------------------------------------------------------------------*/
  if ( hdrbccpc_registration_enabled() )
  {
    HDR_MSG_PROT_3(MSG_LEGACY_MED,"Reset Registration Timer[%d] (%dms), CTimer%d",
                   timer_id,
                   hdrbccpreg.reg_info.smaller_reg_interval_ms,
                   hdrbccpreg.curr_subnet_idx);
    
    /* Restart the registration timer for that subnet. */
    (void) rex_set_timer(&hdrbccpreg.subnet_info[timer_id].reg_timer,
                  hdrbccpreg.reg_info.smaller_reg_interval_ms);

  }
  else
  {
    (void) rex_clr_timer(&hdrbccpreg.subnet_info[timer_id].reg_timer);
  }

} /* hdrbccpreg_process_reg_timer_exp() */

/* <EJECT> */ 
#if 0
/*===========================================================================

FUNCTION HDRBCCPREG_UPDATE_FLOW_REGISTRATION

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
STATIC void hdrbccpreg_update_flow_registration ( void )
{

  hdrbccp_flow_buf_type * q_buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  q_buf_ptr = q_check(&hdrbccp_flow_info.flow_des_priority_q);

  while (q_buf_ptr != NULL)
  {

  }

} /* hdrbccpreg_udpate_flow_registration() */
#endif 
/* <EJECT> */
/*=============================================================================

FUNCTION HDRBCCPREG_SUBNET_CHANGED

DESCRIPTION
  This function updates the subnet info and determines whether subnet 
  registration needs to occur.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  TRUE  - Subnet has changed
  FALSE - We are still on the same subnet or in an invalid state

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrbccpreg_subnet_changed( void )
{

  hdraddr_type new_subnet; /* new subnet info from ovhd*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrbccps_get_state() != HDRBCCPS_OPEN_STATE )
  {
    HDRTRACE_IND_IGNORED_STATE(HDRHAI_BROADCAST_PROTOCOL,
                      HDRIND_OVHD_INFO_UPDATED, hdrbccps_get_state());
    return FALSE;
  }

  if ( hdrovhd_get_subnet(new_subnet, HDRHAI_BROADCAST_PROTOCOL) != E_SUCCESS )
  {
    ERR("Couldn't retrieve subnet info",0,0,0);
    return FALSE;
  }


  /* _mpa_ TODO look for + update the curr_subnet_idx if first after acq */

  /* Current subnet hasn't changed, nothing to do. */
#if 0  /* hack for now, remove later */
  if ( hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].is_valid )
  {
    if ( hdraddr_is_equal(hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].subnet,
                          new_subnet) 
       )
    {
      return FALSE;
    }
  }
  else
#endif 
  {
    hdraddr_set(hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].subnet,
                new_subnet);
    hdrbccpreg.subnet_info[hdrbccpreg.curr_subnet_idx].is_valid = TRUE;

    /* _mpa_ hack for now.  Remove later. 
       Does this ever get called before a flow_request_cmd */
    return FALSE;
  }
} /* hdrbccpreg_subnet_changed() */

#endif /* FEATURE_HDR_BCMCS */

