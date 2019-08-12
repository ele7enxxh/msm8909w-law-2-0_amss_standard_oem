/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          I N I T I A L I Z A T I O N  S T A T E  P R O T O C O L

GENERAL DESCRIPTION

   This file contains routines for the Initialization State Protocol

EXTERNALIZED FUNCTIONS (Global)

  hdrinit_acq_cb()        - Callback for the pilot acquisition status
  hdrinit_activate()      - Command function to activate INIT
  hdrinit_deactivate()    - Command function to deactivate INIT
  hdrinit_reacquire()     - Command function to reacquire a system
  hdrinit_ind_cb()        - Callback registered for delivery of
                            indications
  hdrinit_timer_cb()      - Callback registered for expiration of
                            timer
  hdrinit_msg_cb()        - Callback registered for delivery of
                            messages
  hdrinit_init()          - Function called by hdrmc task to
                            initialize INIT.


EXTERNALIZED FUNCTIONS (Regional)

  hdrinit_process_cmd()     - Function called by hdrmc task to
                              process a command for INIT.
  hdrinit_process_ind()     - Function called by hdrmc task to process
                              an indication for INIT
  hdrinit_process_msg()     - Function called by hdrmc task to process
                              a message for INIT.
  hdrinit_process_timer()   - Function called by hdrmc task to process
                              a timer for INIT.
  hdrinit_start_sync_msg_timer  - start sync msg timer
  hdrinit_stop_sync_msg_timer   - stop sync msg timer
  hdrinit_start_pilot_acq_hold_period - starts pilot acq hold period timer

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating
  the protocol.

Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                        EDIT HISTORY FOR MODULE                   

$PVCSPath: O:/src/asw/COMMON/vcs/hdrinit.c_v   1.44   06 Mar 2003 11:30:42   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrinit.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/05/15   vko     Reduce the HDR hysteresis timer for L2D redirection
02/02/15   vko     Pass srch window param during fastacq during cgi req
12/28/14   vko     Debug info for hdr
01/12/14   vko     Added support for scan mode
08/07/14   mbs     Added support for trm lock exchange during IRAT procedures
07/09/14   cnx     Support NV refresh without reset.
06/12/14   sat     Replace memcpy with memscpy
04/29/14   cnx     Convert CDMA system time in unit of chips to 20ms.
04/12/14   vko     Abort RTT, if pilot acq failed for the first time
11/29/13   sat     NV control for Lock Release Enhancement (LRE)
11/15/13   vko     Fixed AT sending 2 UATI request, after moving from LTE to HDR
08/29/13   vko     FR2812 - Pausing sync timer from HDRCP when HDRSRCH is in 2a state
08/01/13   smd     Used new secapi to get random number.
05/06/13   vko     HDR CP not to go to IDLE unless HDR SRCH moves to MONITOR state
01/11/13   smd     Fixed a compiler error when calling hdrsrch_synchronize().
11/05/12   rkc     Fixed compiler errors when enabling FEATURE_HDR_SYNC_CHECK.
10/29/12   mbs     Fixed compiler errors for first Triton build
10/12/12   wsh     Fixed not sending cmds to SRCH if sys_loss in tunnel mode
08/14/12   cnx     Redir abort to SD when hystr timer expires.
07/16/12   cnx     Use a short hystersis timer (2.5s) for RTT.
05/02/12   wsh     Start L2D HO hystr. timer in timing transfer state
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/19/12   cnx     Fixed compiler errors.
10/31/11   sju     Freed msg when msg callback is done.
10/28/11   pba     Optimized handoff phase 2 support
08/04/11   ssu     Reference ftm_mode using ftm_get_mode().       
08/04/11   ssu     Replace hdr_ftm_fwd_curr_state with hdrhitmsg API calls. 
07/27/11   cnx     Merged Optimized Handoff changes.
03/02/11   sju     Removed temporary changes for 9k Bringup.
12/10/10   kss     Fixed sync issue by providing sync pkt hstr for validation.
06/21/10   cnx     Fixed compiler warnings.
06/07/10   pba     Fixed featurization
05/24/10   cnx     Fix compile issue. 
05/19/10   pba     Added support for Reverse Time Transfer.
05/11/10   pba     Added new states to handle LTE to HDR reselection.
12/09/09   wsh     Fixed lint warning 
12/01/09   grl     Added sync message F3 for bringup info.
10/27/09   pba     Klockwork fixes.
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
12/10/08   smd     Replaced assert.h with amssassert.h
10/01/08   wsh     Fixed critical/high lint warnings
08/29/08   wsh     Fixed double UATIReq when AT powers up w/ KA expired
04/03/07   wsh     Fixed error where dsm_item is not freed
12/01/05   dna     Allow SD to return ACQ_GW or any other new reasons
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
02/14/05   mpa     Use proper enum for hdr_ftm_fwd_curr_state.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
12/03/03   mpa     Removed unused pilot acquisition delay support
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
07/23/03   mpa     Merged in MSM6500 baseline
03/05/03   mpa     Changed timers to use REX callbacks instead of signals
                   Added support for holding pilot acquisition on unlock RF
02/26/03   mpa     Fixed compiler warning for hdrsrch_synchronize().
11/12/02   dna     Added support for different acquisition modes from SD.
08/14/02   dna     Add FEATURE_HDR_SYNC_CHECK, which always sends sync to srch
07/18/02   dna     Added feature to accept protocol revision 244.
03/15/02   dna     Removed 1 minute acquisition timer.
03/14/02   dna     Removed pilot acquisition timer.  Leave it up to searcher.
03/12/02   sq      Added support for FTM.
01/18/02   mpa     Added hdrinit_get_sync_time.
01/16/02   dna     Seed random number generator on sync acquisition
01/04/02   sh      Divided the init to powerup init and regular init
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
05/28/01   sh      Removed state change to inactive when issuing exit hdr
                   mode ind.
05/11/01   kss     Changed system time field of sync msg to 37 bits.
02/05/01   dna     Removed hack to force acquisition on channel 950.
01/13/01   DNA     Deactivate CCMAC in deactivate command processing.
10/11/00   SH      Give network acquired indication after changed to inactive
10/10/00   SH      Added hdrinit_reacquire and related functions
05/26/00   SH      Created module.

===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrdebug.h"
#include "err.h"
#include "ran.h"
#include "amssassert.h"
#include "hdrmci.h"
#include "hdrinit.h"
#include "hdriniti.h"
#include "hdrinits.h"
#include "sd.h"
#ifdef FEATURE_LTE_TO_HDR
#include "hdrcp_msg.h"
#include "hdrmc_v.h"
#include "hdrutil.h"
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrind.h"
#include "comdef.h"
#include "hdrcmac.h"
#include "hdrtrace.h"
#include "hdrdsm.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#include "hdrhitmsg.h"
#endif /* FEATURE_FACTORY_TESTMODE */
#include "hdrsmp.h"
#include "hdrsrch.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


/* <EJECT> */
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

          DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* The pilot acquisition timer (1 minute) was removed as part of hybrid mode
 * because searcher already reports when it acquires or when it fails, and
 * we may want to wait longer than one minute for it to fail.  It isn't the
 * business of the protocol to determine acquisition failure
 */
#define HDRINIT_PILOT_ACQ_TIMER_MSECS   60000  /* max time to acquire the pilot */

#define HDRINIT_SYNC_MSG_TIMER_MSECS    5000   /* max time to acquire the sync msg */

#define HDRINIT_SYNC_SUSPEND_TIMER_MSECS    5000   /* max allowed time in sync 
                                                      suspend state */
#ifdef FEATURE_LTE_TO_HDR
#define HDRINIT_LTE_TO_HDR_HO_HYSTR_MSECS  120000
#define HDRINIT_LTE_TO_HDR_HO_RTT_HYSTR_MSECS  2500
#define HDRINIT_MIN_LTE_TO_HDR_HO_HYSTR_MSECS  100000
#endif /* FEATURE_LTE_TO_HDR */

#define HDRINIT_SYNC_MSG_ID             0        /* message id for the sync msg */

/* Address of the TCB for the task in which this protocol runs */
#define HDRINIT_TASK_PTR hdrmc_tcb_ptr

/* the following data structure is used to extract field from the
   syn message */
typedef PACK(struct)
{
    uint8     msg_id[2];               /* array size = number of bits */
    uint8     max_rev[8];
    uint8     min_rev[8];
    uint8     pilot_pn[9];
    uint8     hi_sys_time[5];          /* the MS 5 bit of the system time */
    uint8     lo_sys_time[32];         /* the LS 32 bit of the system time */
}hdrinit_sync_msg_offset_type;

#ifdef FEATURE_HDR_SCAN_MODE
#include "fs_public.h"

#define HDRINIT_SCAN_MODE_SETTING_FILENAME "/nv/item_files/modem/hdr/cp/init/scanmodesetting"
#define HDRINIT_CUSTOM_BAND_NUM_FILENAME "/nv/item_files/modem/hdr/cp/init/custombandnum"
#define HDRINIT_CUSTOM_CHANNEL_NUM_FILENAME "/nv/item_files/modem/hdr/cp/init/customchannelnum"
#endif /* FEATURE_EHRPD */

/*===========================================================================

               DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  HDRINIT_ACQ_SUCCESSFUL, /* Successful acquisition */
  HDRINIT_ACQ_FAILED      /* Failed acqusistion */
} hdrinit_acq_result_enum_type;


/* Message structure */

typedef struct
{
    boolean              valid_msg;
    uint8                msg_id;
    uint8                max_rev;
    uint8                min_rev;
    sys_pilot_pn_type    pilot_pn;
    qword                sys_time;

}hdrinit_sync_msg_type;

typedef enum
{
  HDRINIT_SYNC_SUSPEND,    
  HDRINIT_SYNC_RESUME    
}
hdrinit_sync_result_enum_type;

/* Commands supported by the initialization state protocol */

/*
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

 /* <EJECT> */
/*===========================================================================

          MACROS FOR EXTRACING SYNC MESSAGE FIELDS

===========================================================================*/


#define HDRINIT_GET_SYNC_MSG_ID(src_ptr, des_ptr) \
        HDRBIT_UNPACK8(src_ptr, des_ptr,    \
        (uint16) FPOS(hdrinit_sync_msg_offset_type, msg_id), \
        (uint16) FSIZ(hdrinit_sync_msg_offset_type, msg_id))

#define HDRINIT_GET_MAX_REV(src_ptr, des_ptr) \
        HDRBIT_UNPACK8(src_ptr, des_ptr,    \
        (uint16) FPOS(hdrinit_sync_msg_offset_type, max_rev), \
        (uint16) FSIZ(hdrinit_sync_msg_offset_type, max_rev))

#define HDRINIT_GET_MIN_REV(src_ptr, des_ptr) \
        HDRBIT_UNPACK8(src_ptr, des_ptr,    \
        (uint16) FPOS(hdrinit_sync_msg_offset_type, min_rev), \
        (uint16) FSIZ(hdrinit_sync_msg_offset_type, min_rev))

#define HDRINIT_GET_PILOT_PN(src_ptr, des_ptr) \
        HDRBIT_UNPACK16(src_ptr, des_ptr,    \
        (uint16) FPOS(hdrinit_sync_msg_offset_type, pilot_pn), \
        (uint16) FSIZ(hdrinit_sync_msg_offset_type, pilot_pn))

#define HDRINIT_GET_HI_SYS_TIME(src_ptr, des_ptr) \
        HDRBIT_UNPACK32(src_ptr, des_ptr,    \
        (uint16) FPOS(hdrinit_sync_msg_offset_type, hi_sys_time), \
        (uint16) FSIZ(hdrinit_sync_msg_offset_type, hi_sys_time))

#define HDRINIT_GET_LO_SYS_TIME(src_ptr, des_ptr) \
        HDRBIT_UNPACK32(src_ptr, des_ptr,    \
        (uint16) FPOS(hdrinit_sync_msg_offset_type, lo_sys_time), \
        (uint16) FSIZ(hdrinit_sync_msg_offset_type, lo_sys_time))

typedef enum
{
   HDRINIT_SYNC_MSG_TIMER,
#ifdef FEATURE_LTE_TO_HDR
   HDRINIT_LTE_TO_HDR_HO_HYSTR_TIMER,
#endif /* FEATURE_LTE_TO_HDR */
   HDRINIT_SYNC_SUSPEND_TIMER
} hdrinit_timer_enum_type;

/* Control block of the Initialization State protocol */

LOCAL struct
{
  rex_timer_type           sync_msg_timer;
    /* Sync message timer */
  uint8                    rev_num;
    /* Access terminal's revision number */
  hdrinit_sync_msg_type    syn_msg;
    /* Storage for the received Sync message */
  hdrinit_timing_info_type timing_info;
    /* HDR timing information */
#ifdef FEATURE_LTE_TO_HDR
  rex_timer_type           lte_to_hdr_ho_hystr_timer;
    /* LTE to HDR handoff hystersis timer */
  uint32                   hystr_timer_value;
    /* Hystersis timer value in ms */
  boolean                  hystr_timer_is_expired;
    /* Whether hystersis timer is expired */
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_LTE_TO_HDR_OH
  boolean hrpd_meas_enabled;
#endif /* FEATURE_LTE_TO_HDR_OH */
rex_timer_type             sync_suspend_timer;
rex_timer_cnt_type         saved_syncmsg_timer_ms; 
#ifdef FEATURE_HDR_SCAN_MODE
boolean scan_mode; 
   /* scan mode if enabled, read the custom channel info */
sys_channel_type custom_channel_info;
   /* channel info in scan mode */
#endif /* FEATURE_HDR_SCAN_MODE */
}hdrinit;

#ifdef FEATURE_FACTORY_TESTMODE
hdrinit_sync_msg_type       ftm_sync_msg;
uint32                      ftm_hi_sys_time,ftm_lo_sys_time;
hdrdec_half_slot_time_type  ftm_time_stamp;
#endif /* FEATURE_FACTORY_TESTMODE */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PILOT_ACQ

DESCRIPTION
  This function performs the pilot acquistion for the initialization
  state protocol.

DEPENDENCIES
  None

PARAMETERS
  ss_act_ptr - pointer to the SD action

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_pilot_acq
(
   sd_ss_act_s_type *ss_act_ptr
)
{
  /* Channel to acquire on */
  sys_channel_type channel;

#ifdef FEATURE_LTE_TO_HDR
  /* Pilot PN information provided by LTE  */
  sys_pilot_pn_type pilot_pn[HDR_CP_MAX_PN_LIST_SIZE];

  /* Number of pn's in list */
  uint8 pn_list_size = 0;      

  /* Maximum time allowed by LTE for DO to do brute-force acq and sync. */
  uint16 time_allowed_ms = 0;

  hdrsrch_acq_reason_enum_type reason = HDRSRCH_ACQ_NORMAL;
#endif /* FEATURE_LTE_TO_HDR */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_SCAN_MODE

  if(hdrinit.scan_mode == TRUE)
  {
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "Scan mode enabled %d Read band %d Channel %d", 
		              				    hdrinit.scan_mode, hdrinit.custom_channel_info.band, hdrinit.custom_channel_info.chan_num);
  }
  else
  {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Scan mode disabled");
  }
  
  
#ifdef FEATURE_LTE_TO_HDR
  if (hdrmc_hdr_to_lte_time_transfer_is_in_progress() || hdrmc_lte_to_hdr_redir_is_in_progress())
  {
      if(hdrinit.scan_mode == TRUE)
      {
         hdrinit.scan_mode = FALSE;
         HDR_MSG_PROT( MSG_LEGACY_HIGH,"Scan mode disabled when D2L timer transfer or L2D redir in progress");
      }      
	  /* Disable scan mode for time transfer, redirection. */
  }
#endif /* FEATURE_LTE_TO_HDR */
  
  if(hdrinit.scan_mode == TRUE)
  {
	  /* If scan mode enabled, then get the band/channel from already loaded NV values */
	  channel.band = hdrinit.custom_channel_info.band;
	  channel.chan_num = hdrinit.custom_channel_info.chan_num;
  }
  else
#endif /* FEATURE_HDR_SCAN_MODE */
  {
	  /* Else get the values from SD provided struct */
      channel.band = ss_act_ptr->prm.acq_hdr.band;
      channel.chan_num = ss_act_ptr->prm.acq_hdr.chan;
  }

#ifdef FEATURE_LTE_TO_HDR
  memset ( pilot_pn, 0, (sizeof(sys_pilot_pn_type) * HDR_CP_MAX_PN_LIST_SIZE)); 

  if (hdrmc_hdr_to_lte_time_transfer_is_in_progress())
  {
    hdrmc_get_timing_transfer_pn_list( &pn_list_size, pilot_pn );

    hdrmc_get_duration_allowed_for_time_transfer( &time_allowed_ms );

#ifdef FEATURE_MEMCPY_REMOVAL
    hdrinit.timing_info.sys_info = channel;
#else
    memcpy (&hdrinit.timing_info.sys_info, &channel, sizeof(sys_channel_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
  }

  if (hdrmc_lte_to_hdr_redir_is_in_progress())
  {
    reason = HDRSRCH_ACQ_REDIRECTION;
  }

  hdrlmac_acquire ( channel,
                    ss_act_ptr->prm.acq_hdr.acq_mode,
                    pn_list_size,
                    pilot_pn,
                    time_allowed_ms,
                    reason,
                    HDRHAI_INITIALIZATION_STATE_PROTOCOL );
#else

  hdrlmac_acquire ( channel,
                    ss_act_ptr->prm.acq_hdr.acq_mode,
                    HDRHAI_INITIALIZATION_STATE_PROTOCOL );
#endif /* FEATURE_LTE_TO_HDR */

  hdrinits_change_state( HDRINITS_PILOT_ACQ_STATE);
} /* hdrinit_pilot_acq() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_EXIT_HDR

DESCRIPTION
  This function issues an EXIT HDR indication

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_exit_hdr ( void )
{

  /* set the exit HDR indication */
  hdrind_give_ind(HDRIND_INIT_EXIT_HDR_MODE, NULL);

  /* when this indication is generated INIT will
     be deactivated by the ALMP so INIT do not have
     to change itself to the Inactive State */

} /* hdrinit_exit_hdr() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_SD_ACT

DESCRIPTION
  This function processes the return action from the system determination.

DEPENDENCIES
  Should only be called in the network determination state

PARAMETERS
  ss_act_ptr - pointer to the SD action

RETURN VALUE
  Next state.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(613,hdrinit_process_sd_act) */
LOCAL void hdrinit_process_sd_act
(
  sd_ss_act_s_type *ss_act_ptr
)
{

  ASSERT(ss_act_ptr != NULL);
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "HDR Init proc SD action=%d",
                  ss_act_ptr->act);
  switch (ss_act_ptr->act)
  {

    case SD_SS_ACT_ACQ_HDR:
      /* Initiate pilot acquisition */
      hdrinit_pilot_acq(ss_act_ptr);
      break;

    case SD_SS_ACT_MEAS_DED:
      /* request LMAC to measure channels */
      /* This feature is not implemented yet */
      ERR_FATAL("SD trying to do Channel Measurement",0,0,0);
      break;

    case SD_SS_ACT_CONTINUE:
      if( hdrinits_get_state() != HDRINITS_NET_DET_STATE )
      {
        ERR_FATAL("SD Continue not valid in this state",0,0,0);
      }
      /* We must be off line.  Just stay in network determination for now */
      break;

    default:
      /* Need to exit the HDR mode */
      hdrinit_exit_hdr();
      break;
  }

} /* hdrinit_process_sd_act() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_CONTINUE_ACQ_CMD

DESCRIPTION
  This function processes a continue acquisition command given to the
  the initialization state protocol

DEPENDENCIES
  hdrinit_init must have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_continue_acq_cmd ( void )
{
#ifdef FEATURE_LTE_TO_HDR
  /* LTE RTC counter */
  uint32 lte_ostmr;
  /* CDMA System time */
  uint64 cdma_sys_time;
  /* CDMA System time - higher and lower bits */
  uint32 hi_sys_time;
  uint32 lo_sys_time;
#endif /* FEATURE_LTE_TO_HDR*/

  sd_ss_act_s_type rtrn_ss_act; /* action returned by SD */
/*------------------------------------------------------------------------ */

  if ( !(( hdrinits_get_state() == HDRINITS_INACTIVE_STATE )  ||
         ( hdrinits_get_state() == HDRINITS_NET_DET_STATE  )) )
  {
    /*-----------------------------------------------------------------------
      If the protocol is not in inactive or network determination state, 
      do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_LOW, "INIT Continue Acq CMD ignored");
    return;
  }

#ifdef FEATURE_LTE_TO_HDR

  memset ( &hdrinit.timing_info, 0, sizeof(hdrinit_timing_info_type) );

  if ( hdrmc_lte_to_hdr_resel_is_in_progress() 
       ||
       hdrmc_lte_to_hdr_get_cgi_is_in_progress()
#ifdef FEATURE_LTE_TO_HDR_OH
       ||
       hdrsap_lte_to_hdr_active_ho_is_in_progress() 
#endif /* FEATURE_LTE_TO_HDR_OH */
     )
  {
    /*-----------------------------------------------------------------------
    Set the protocol state to time transfer state.
    -----------------------------------------------------------------------*/

    hdrinits_change_state( HDRINITS_TIME_TRANSFER_STATE );

    /* In case of LTE to HDR reselection, LTE stack provides
       the information for HDR acquisition. */
    hdrmc_get_reselect_timing_info( &lte_ostmr, &cdma_sys_time );

    /* CDMA time in Sync message is in unit of 26.66ms while CDMA time got
     * from LTE is in unit of chip. As 1 chip = 1.2288MHz, the CDMA time 
     * from LTE needs be divided by 2^15 first */
    lo_sys_time = (uint32)((cdma_sys_time >> 15) & 0xFFFFFFFF);
    hi_sys_time = (uint32)(cdma_sys_time >> 47);

    qw_set(hdrinit.syn_msg.sys_time, hi_sys_time, lo_sys_time);

    /* Pass the timing information provided by LTE stack to 
       searcher */
    hdrlmac_time_transfer( lte_ostmr, cdma_sys_time );
  }
  else
#endif /* FEATURE_LTE_TO_HDR */
  {
    /*-----------------------------------------------------------------------
    Set the protocol state to network determination state.
    -----------------------------------------------------------------------*/

    hdrinits_change_state( HDRINITS_NET_DET_STATE);

    /* get the last action */
    (void) sd_ss_hdr_act_get(&rtrn_ss_act);

    /* process the action returned by the SD */
    hdrinit_process_sd_act(&rtrn_ss_act);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_ACTIVATE_CMD

DESCRIPTION
  This function processes an activate command given to the
  the initialization state protocol

DEPENDENCIES
  hdrinit_init should have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_activate_cmd ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( hdrinits_get_state() != HDRINITS_INACTIVE_STATE )
  {
    /*-----------------------------------------------------------------------
      If the protocol has been activated, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_LOW, "INIT Activate CMD ignored");
    return;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if (hdrsap_tunnel_mode_is_enabled(HDRHAI_INITIALIZATION_STATE_PROTOCOL) &&
      !hdrmc_lte_to_hdr_redir_is_in_progress() &&
      !hdrmc_hdr_to_lte_time_transfer_is_in_progress())
  {
    /*-----------------------------------------------------------------------
    If AT is tunnel mode enter NET_DET state and do nothing.
    -----------------------------------------------------------------------*/
    hdrinits_change_state( HDRINITS_NET_DET_STATE);
    return;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  hdrinit_process_continue_acq_cmd();

} /* hdrinit_process_activate_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_DEACTIVATE_CMD

DESCRIPTION
  This function processes an deactivate command given to the
  the initialization state protocol

DEPENDENCIES
  hdrinit_init must have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_deactivate_cmd ( void )
{


  if( hdrinits_get_state() == HDRINITS_INACTIVE_STATE )
  {
    /*-----------------------------------------------------------------------
      If the protocol is in the inactive state, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_LOW, "INIT Inactivate CMD ignored");
    return;
  }

  /* Deactivate the control channel MAC, if it is active.  Otherwise this
   * command is ignored by the control channel MAC.  If we got the SYNC
   * message we are already in INACTIVE state so we return above and not
   * deactivate the CCMAC in the case where we acquire the system.
   */
  hdrcmac_deactivate(HDRHAI_INITIALIZATION_STATE_PROTOCOL);

  /* change to the inactive state */
  hdrinits_change_state(HDRINITS_INACTIVE_STATE);
} /* hdrinit_process_deactivate_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_REACQUIRE_CMD

DESCRIPTION
  This function processes a reacquire command given to the
  the initialization state protocol

DEPENDENCIES
  hdrinit_init must have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_reacquire_cmd ( void )
{


  if( hdrinits_get_state() == HDRINITS_INACTIVE_STATE )
  {
    /*-----------------------------------------------------------------------
      If the protocol is in the inactive state, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_LOW, "INIT Require CMD ignored");
    return;
  }

  /* deactivate the init state protocol */
  hdrinit_process_deactivate_cmd();

  /* activate the init state protocol */
  hdrinit_process_activate_cmd();

}/* hdrinit_process_reacquire_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_PILOT_ACQUIRE_FAILED

DESCRIPTION
  This function processes the pilot acquisition failure case.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_pilot_acquire_failed ( void )
{
  sd_ss_act_s_type rtrn_ss_act; /* action returned by SD */

  memset ( &rtrn_ss_act, 0, sizeof (sd_ss_act_s_type) );

  hdrinits_change_state( HDRINITS_NET_DET_STATE );
  #ifdef FEATURE_LTE_TO_HDR
  if ( hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
  { 
      /* For RTT since we give only one channel to SD 
         it is not expected that SD will give us more channels 
         on Pilot Acq failure. Hence no need to call SD API. 
         Need to exit the HDR mode */
      hdrinit_exit_hdr();   
  }
  else
  #endif
  {
        (void ) sd_ss_ind_hdr_acq_failed_pilot(&rtrn_ss_act);
        hdrinit_process_sd_act(&rtrn_ss_act);
  }  
} /* hdrinit_process_pilot_acquire_failed() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_PILOT_ACQ_RESULT

DESCRIPTION
  This function processes the pilot acquisition result indication

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdrinit_process_pilot_acq_result) */
LOCAL void hdrinit_process_pilot_acq_result
(
  hdrind_ind_name_enum_type    ind_name,
  hdrind_ind_data_union_type   *ind_data_ptr,
  hdrinit_acq_result_enum_type acq_result
)
{

  if(hdrinits_get_state() != HDRINITS_PILOT_ACQ_STATE)
  {

    HDRTRACE_IND_IGNORED_STATE( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                 ind_name,
                 hdrinits_get_state());
    return;
  }


  /* If we want to verify the channel we need to get the link_id with
   * the acquire indication and then retrieve the channel from lmac.
   */
//  if(hdrinit.frequency != ind_data_ptr->pilot_acq.frequency)
//  {
//     /* ignore the indication if channel is not matched */
//    HDR_MSG_PROT (MSG_LEGACY_LOW, "Channel not matched",0,0,0);
//    return;
//  }

  if(acq_result == HDRINIT_ACQ_SUCCESSFUL)
  {
    hdrinits_change_state( HDRINITS_SYNC_STATE );
  }
  else
  {
    hdrinit_process_pilot_acquire_failed();
  }

} /* hdrinit_process_pilot_acq_result() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_CHAN_MEAS

DESCRIPTION
  This function processes the channel measurement results

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdrinit_process_chan_meas) */
LOCAL void hdrinit_process_chan_meas
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
)
{

  sd_ss_act_s_type rtrn_ss_act; /* action returned by SD */


  if( hdrinits_get_state() != HDRINITS_NET_DET_STATE )
  {
     /* ignore the indication if it is not in the net det state */
   HDRTRACE_IND_IGNORED_STATE( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                  ind_name,
                  hdrinits_get_state());
     return;
  }

  memset ( &rtrn_ss_act, 0, sizeof (sd_ss_act_s_type) );

  /* DNA HACK need real measurement ID from request */
  (void) sd_ss_ind_hdr_misc_meas_rprt(0, TRUE, &rtrn_ss_act);  //lint !e530 !e534

  hdrinit_process_sd_act(&rtrn_ss_act);

} /* hdrinit_process_chan_meas() */


/* <EJECT> */
#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRINIT_PROCESS_TIME_SYNC

DESCRIPTION
  This function processes the time sync result

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdrinit_process_time_sync) */
LOCAL void hdrinit_process_time_sync
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
)
{
  /* Action returned by SD */
  sd_ss_act_s_type rtrn_ss_act; 
   
  /* Channel information provided by LTE  */
  sys_channel_type channel;

  /* Pilot PN information provided by LTE  */
  sys_pilot_pn_type pilot_pn;

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsrch_fast_acq_reason_enum_type fast_acq_reason;
#endif /* FEATURE_LTE_TO_HDR_OH */

  uint8 srch_window_size = 0xff;
  /* srch window size provided by LTE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( hdrinits_get_state() != HDRINITS_TIME_TRANSFER_STATE )
  {
    /* ignore the indication if it is not in the net det state */
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                  ind_name,
                  hdrinits_get_state());
    return;
  }

  if (ind_data_ptr->tran_sys_time_result == HDRSRCH_TRAN_SYS_TIME_SUCCEEDED)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
       "IRAT: Time sync succeeded, proceeding with fast acq");

    hdrinits_change_state( HDRINITS_FAST_ACQ_STATE );

#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
    {
      fast_acq_reason = HDRSRCH_FAST_ACQ_ACTIVE_HO;
      pilot_pn = 0;
      memset ( &channel, 0, sizeof(sys_channel_type) );
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
#ifdef FEATURE_LTE_TO_HDR_OH
      fast_acq_reason = HDRSRCH_FAST_ACQ_RESELECTION;
#endif /* FEATURE_LTE_TO_HDR_OH */
      /* Get the channel information from SD */
      (void) sd_ss_hdr_act_get(&rtrn_ss_act);

      /* SD should only tell HDR to acquire any other action is an error */
      ASSERT (rtrn_ss_act.act == SD_SS_ACT_ACQ_HDR);

      /* Get the pilot pn for the reselection */
      hdrmc_get_reselect_pilot_pn( &pilot_pn );

      channel.band = rtrn_ss_act.prm.acq_hdr.band;
      channel.chan_num = rtrn_ss_act.prm.acq_hdr.chan;
    }
    if(hdrmc_lte_to_hdr_get_cgi_is_in_progress() == TRUE)
    {
		 hdrmc_get_cgi_srch_window_size(&srch_window_size);
    }
    /* Once time sync is done pass the channel info to searcher to acquire */
    hdrlmac_fast_acquisition( channel, pilot_pn
#ifdef FEATURE_LTE_TO_HDR_OH
                              ,fast_acq_reason, srch_window_size 
#endif /* FEATURE_LTE_TO_HDR_OH */
                            );
  }
  else if (ind_data_ptr->tran_sys_time_result == HDRSRCH_TRAN_SYS_TIME_FAILED)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
      "IRAT: Time sync failed, auto deactivate in progress");

    hdrinit_exit_hdr();
  }

} /* hdrinit_process_time_sync() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_FAST_ACQ

DESCRIPTION
  This function processes the fast acquistion result

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdrinit_process_time_sync) */
LOCAL void hdrinit_process_fast_acq
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
)
{

  if( hdrinits_get_state() != HDRINITS_FAST_ACQ_STATE )
  {
    /* ignore the indication if it is not in the net det state */
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                  ind_name,
                  hdrinits_get_state());
    return;
  }

  if (ind_data_ptr->fast_acq_result == HDRSRCH_FAST_ACQ_SUCCEEDED)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "IRAT: Fast acq succeeded" );

    if (
#ifdef FEATURE_LTE_TO_HDR_OH
        !hdrsap_lte_to_hdr_active_ho_is_in_progress() &&
#endif /* FEATURE_LTE_TO_HDR_OH */
        !hdrmc_lte_to_hdr_get_cgi_is_in_progress()
        )
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                    "IRAT: Auto activation indication sent to MMOC");
      mmoc_rpt_prot_auto_actd_ind( PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
      hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_ACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
    }

    hdrinits_change_state( HDRINITS_INACTIVE_STATE ); 

    hdrsmp_set_dup_act_detect(HDRSMP_DUP_ACT_DETECT_ENABLED);
    hdrind_give_ind( HDRIND_INIT_NETWORK_ACQUIRED, NULL );
    hdrsmp_set_dup_act_detect(HDRSMP_DUP_ACT_DETECT_DISABLED);
  }
  else if (ind_data_ptr->fast_acq_result == HDRSRCH_FAST_ACQ_FAILED)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
        "IRAT: Fast acq failed, auto deactivate in progress");

    hdrinit_exit_hdr();
  }

} /* hdrinit_process_fast_acq() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_GET_TIMING_INFO

DESCRIPTION
  This function returns the stored timing information.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_get_timing_info ( hdrinit_timing_info_type *timing_info_ptr )
{

#ifdef FEATURE_MEMCPY_REMOVAL
  *timing_info_ptr = hdrinit.timing_info;
#else
  memcpy(timing_info_ptr, &hdrinit.timing_info, sizeof(hdrinit_timing_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
 
} /* hdrinit_get_timing_info() */

/* <EJECT> */
#endif /* FEATURE_LTE_TO_HDR */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_TIMING_INFO

DESCRIPTION
  This function processes 

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdrinit_process_time_sync) */
LOCAL void hdrinit_process_timing_info
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( hdrinits_get_state() != HDRINITS_SYNC_STATE )
  {
    /* ignore the indication if it is not in the net det state */
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                  ind_name,
                  hdrinits_get_state());
    return;
  }

  hdrinit.timing_info.is_valid = FALSE;
  
  /* In the case of HDR to LTE timing transfer INIT protocol waits for 
     timing info from searcher once this is recvd the information is 
     stored, stack is deactivated and the reponse with timing info is 
     sent back to LTE */
#ifdef FEATURE_LTE_TO_HDR
  if ( !hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
#endif /* FEATURE_LTE_TO_HDR */
  {
    hdrinit.syn_msg.valid_msg   = TRUE;
    hdrinits_change_state(HDRINITS_INACTIVE_STATE);    

#ifdef FEATURE_LTE_TO_HDR
    if (hdrmc_lte_to_hdr_ho_is_in_progress())
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                    "IRAT: Sync msg recvd, auto activation indication sent to MMOC");
      mmoc_rpt_prot_auto_actd_ind( PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
      hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_ACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
    }
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_SCAN_MODE
	if(hdrinit.scan_mode == TRUE)
	{
	  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Network acquired. Waiting in scan mode");
	}
	else
#endif /* FEATURE_HDR_SCAN_MODE */
	{

      /* This is a known code path that will lead to double-SMP Activate
          while processing INIT_NETWORK_ACQUIRE, if KA timer has expired. 
          Enable detection specifically for this case to avoid possible
          double SMP Activate */
      hdrsmp_set_dup_act_detect(HDRSMP_DUP_ACT_DETECT_ENABLED);
      hdrind_give_ind( HDRIND_INIT_NETWORK_ACQUIRED, NULL );
      hdrsmp_set_dup_act_detect(HDRSMP_DUP_ACT_DETECT_DISABLED);	
    }
  }
#ifdef FEATURE_LTE_TO_HDR
  else
  {/* timing info valid only during Reverse time transfer */
    if (ind_data_ptr->timing_info.status)
    {
      hdrinit.timing_info.is_valid = TRUE;
      hdrinit.timing_info.cdma_sys_time = ind_data_ptr->timing_info.cdma_sys_time;
      hdrinit.timing_info.lte_ostmr = ind_data_ptr->timing_info.lte_ostmr;
    }  
    hdrinit_exit_hdr();
  }
#endif
} /* hdrinit_process_timing_info() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_TIMER_TIMEOUT

DESCRIPTION
  This function processes the sync suspend timer or sync msg timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_timer_timeout ( void )
{

  sd_ss_act_s_type rtrn_ss_act; /* action returned by SD */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  hdrcmac_deactivate(HDRHAI_INITIALIZATION_STATE_PROTOCOL);

  hdrinits_change_state( HDRINITS_NET_DET_STATE );

  memset ( &rtrn_ss_act, 0, sizeof (sd_ss_act_s_type) );

  (void)sd_ss_ind_hdr_acq_failed_sync(&rtrn_ss_act); //lint !e530 !e534

  hdrinit_process_sd_act(&rtrn_ss_act);
}/* hdrinit_process_timer_timeout() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_SYNC_MSG_TIMEOUT

DESCRIPTION
  This function processes the sync msg timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_sync_msg_timeout ( void )
{

  sd_ss_act_s_type rtrn_ss_act; /* action returned by SD */

  if( hdrinits_get_state() != HDRINITS_SYNC_STATE )
  {
    /*-----------------------------------------------------------------------
      Wrong state or timer is not on, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Syn msg timer timout ignored");
    return;
  }

  if ( hdrlmac_is_lre_enabled() )
  {
  hdrinit_process_timer_timeout();
  }
  else
  {
    hdrcmac_deactivate(HDRHAI_INITIALIZATION_STATE_PROTOCOL);

    hdrinits_change_state( HDRINITS_NET_DET_STATE );
   
    memset ( &rtrn_ss_act, 0, sizeof (sd_ss_act_s_type) );
   
    (void)sd_ss_ind_hdr_acq_failed_sync(&rtrn_ss_act); //lint !e530 !e534
   
    hdrinit_process_sd_act(&rtrn_ss_act);
  }
}/* hdrinit_process_sync_msg_timeout() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_SYNC_SUSPEND_TIMEOUT

DESCRIPTION
  This function processes the sync suspend timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_sync_suspend_timeout ( void )
{

  if( hdrinits_get_state() != HDRINITS_SYNC_SUSPEND_STATE )
  {
    /*-----------------------------------------------------------------------
      Wrong state or timer is not on, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Sync suspend timer timout ignored");
    return;
  }
  hdrinit_process_timer_timeout();

}/* hdrinit_process_sync_suspend_timeout() */

/* <EJECT> */
#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================
FUNCTION HDRINIT_SET_HYSTR_TIMER_IS_EXPIRED

DESCRIPTION
  This function sets the flag that indicating whether LTE to HDR hystersis 
  timer is expired.

DEPENDENCIES
  None

PARAMETERS
  hystr_timer_is_expired - TRUE if hystersis timer is expired.
                           FALSE otherwise.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrinit_set_hystr_timer_is_expired
( 
  boolean hystr_timer_is_expired
)
{
  hdrinit.hystr_timer_is_expired = hystr_timer_is_expired;
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "set hystr_timer_is_expired %d",
                  hdrinit.hystr_timer_is_expired );

} /* hdrinit_set_hystr_timer_is_expired() */


/*===========================================================================
FUNCTION HDRINIT_HYSTR_TIMER_IS_EXPIRED

DESCRIPTION
  This function sets the flag that indicating whether LTE to HDR hystersis 
  timer is expired.

DEPENDENCIES
  None

PARAMETERS
  hystr_timer_is_expired - TRUE if hystersis timer is expired.
                           FALSE otherwise.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrinit_hystr_timer_is_expired( void )
{
  return hdrinit.hystr_timer_is_expired;
} /* hdrinit_hystr_timer_is_expired() */


/*===========================================================================

FUNCTION HDRINIT_PROCESS_LTE_TO_HDR_HO_HYSTR_TIMEOUT

DESCRIPTION
  This function processes the LTE to HDR hystersis timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_lte_to_hdr_ho_hystr_timeout ( void )
{
  if (!hdrmc_lte_to_hdr_ho_is_in_progress() &&
      !hdrmc_hdr_to_lte_time_transfer_is_in_progress())
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, 
           "LTE to HDR HO hystr timer timeout ignored - HO not in progress");
    return;
  }

  if (( hdrinits_get_state() != HDRINITS_PILOT_ACQ_STATE ) &&
      ( hdrinits_get_state() != HDRINITS_FAST_ACQ_STATE ))
  {
    /*-----------------------------------------------------------------------
      Wrong state or timer is not on, do nothing.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_LOW, 
            "LTE to HDR HO hystr timer timeout ignored - wrong state");
    return;
  }

  hdrinit_set_hystr_timer_is_expired(TRUE);

  hdrinit_exit_hdr();

}/* hdrinit_process_lte_to_hdr_ho_hystr_timeout() */
#endif /* FEATURE_LTE_TO_HDR */

#ifndef PERLUTF_TDTGEN_EXCLUDE
/*===========================================================================

FUNCTION HDRINIT_PROCESS_SYNC_MSG

DESCRIPTION
  This function processes the sync message for INIT.

DEPENDENCIES
  Message ID must be HDRINIT_SYNC_MSG_ID

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_sync_msg
(
  dsm_item_type * item_ptr
)
{
  sd_ss_act_s_type                     rtrn_ss_act;
  uint32                               hi_sys_time;
  uint32                               lo_sys_time;
  hdrdec_half_slot_time_type           time_stamp;
  int                                  status = E_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset ( &rtrn_ss_act, 0, sizeof (sd_ss_act_s_type) );

  status |=  HDRINIT_GET_MAX_REV(item_ptr, &(hdrinit.syn_msg.max_rev));
  status |=  HDRINIT_GET_MIN_REV(item_ptr, &(hdrinit.syn_msg.min_rev));

  if (status != E_SUCCESS)
  {
    /* unable to unpack the messages fields discard the message*/
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Discard Sync Msg-invald fields");
    return;
  }
#ifdef FEATURE_HDR_ACCEPT_PROTOCOL_REV_244
  if (((244 < hdrinit.syn_msg.min_rev) ||
       (244 > hdrinit.syn_msg.max_rev))
  &&  ((hdrinit.rev_num < hdrinit.syn_msg.min_rev) ||
       (hdrinit.rev_num > hdrinit.syn_msg.max_rev)))
#else
  if ( (hdrinit.rev_num < hdrinit.syn_msg.min_rev) ||
     (hdrinit.rev_num > hdrinit.syn_msg.max_rev) )
#endif
  {
  /* rev number is not in the valid range */
    HDRTRACE_HDRMC_INIT_PROTOCOL_MISMATCH();

    hdrcmac_deactivate(HDRHAI_INITIALIZATION_STATE_PROTOCOL);
    hdrinits_change_state(HDRINITS_NET_DET_STATE);
    (void) sd_ss_ind_hdr_acq_prot_mis(&rtrn_ss_act); //lint !e530 !e534
    hdrinit_process_sd_act(&rtrn_ss_act);
    return;
  }

  status |= HDRINIT_GET_PILOT_PN(item_ptr, &(hdrinit.syn_msg.pilot_pn));
  status |= HDRINIT_GET_HI_SYS_TIME(item_ptr, &(hi_sys_time));
  status |= HDRINIT_GET_LO_SYS_TIME(item_ptr, &(lo_sys_time));

  if (status != E_SUCCESS)
  {
    /* unable to unpack the messages fields discard the message*/
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Discard Sync Msg-invalid fields");
    return;
  }

  qw_set(hdrinit.syn_msg.sys_time, hi_sys_time, lo_sys_time);

  status = hdrcmac_get_sync_caps_timestamp (HDRDSM_GET_TIMESTAMP(item_ptr), 
                                            &time_stamp);

  if (status != E_SUCCESS)
  {
    ERR("Timestamp not valid",0,0,0);
    return;
  }

  hdrlmac_synchronize((hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr ),
                   hdrinit.syn_msg.pilot_pn,
                   hdrinit.syn_msg.sys_time,
                   time_stamp, HDRHAI_INITIALIZATION_STATE_PROTOCOL);

} /* hdrinit_process_sync_msg() */

/*===========================================================================

FUNCTION HDRINIT_PROCESS_LMAC_SYNC_IND

DESCRIPTION
  This function processes the sync suspend/resume indication for INIT.

DEPENDENCIES
  

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrinit_process_lmac_sync_ind
(
  hdrind_ind_name_enum_type    ind_name,
  hdrinit_sync_result_enum_type sync_result
)
{
   hdrinits_state_enum_type cur_state = hdrinits_get_state();
                                       /* current init state */


  if(sync_result == HDRINIT_SYNC_SUSPEND && cur_state == HDRINITS_SYNC_STATE)
  {
     hdrinits_change_state( HDRINITS_SYNC_SUSPEND_STATE );
  }
  else if(sync_result == HDRINIT_SYNC_RESUME && cur_state == HDRINITS_SYNC_SUSPEND_STATE)
  {
     hdrinits_change_state( HDRINITS_SYNC_STATE );
  }  
  else
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                 ind_name,
                 cur_state);
    HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "Ignored LMAC SYNC indication event=%d curr state = %d",
                    ind_name,cur_state);
    return;
  }
}
 /* hdrinit_process_lmac_sync_ind() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */

/* <EJECT> */
#ifndef PERLUTF_TDTGEN_EXCLUDE
/*===========================================================================

FUNCTION HDRINIT_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRINIT.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrinit_msg_cb
(
  dsm_item_type *item_ptr
)
{
#ifdef FEATURE_FACTORY_TESTMODE
  hdrerrno_enum_type  status = E_SUCCESS; /* message unpacking status */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(( ftm_get_mode() == FTM_MODE ) && ( hdrhitmsg_get_fwd_curr_state() == HDRHITMSG_HDR_FTM_SYNC_STATE ))
  {
      status  |=   HDRINIT_GET_SYNC_MSG_ID( item_ptr, &(ftm_sync_msg.msg_id));
      status  |=   HDRINIT_GET_MAX_REV(item_ptr, &(ftm_sync_msg.max_rev));
      status  |=   HDRINIT_GET_MIN_REV(item_ptr, &(ftm_sync_msg.min_rev));
      status  |=   HDRINIT_GET_PILOT_PN(item_ptr, &(ftm_sync_msg.pilot_pn));
      status  |=   HDRINIT_GET_HI_SYS_TIME(item_ptr, &(ftm_hi_sys_time));
      status  |=   HDRINIT_GET_LO_SYS_TIME(item_ptr, &(ftm_lo_sys_time));

      if (status == E_SUCCESS)
      {
         qw_set(ftm_sync_msg.sys_time, ftm_hi_sys_time, ftm_lo_sys_time);

         (void) hdrcmac_get_sync_caps_timestamp (HDRDSM_GET_TIMESTAMP(item_ptr), 
                                                 &ftm_time_stamp);

         hdrsrch_synchronize( ftm_sync_msg.sys_time, ftm_sync_msg.pilot_pn,
                              ftm_time_stamp, NULL );

         hdrhitmsg_set_fwd_curr_state( HDRHITMSG_HDR_FTM_IDLE );
      }
      else
      {
         ERR("Unable to unpack msg", 0, 0, 0);
      }

      HDR_MSG_PROT_1(MSG_LEGACY_MED, "FTM Sync - free count before: %d", 
                     HDRDSM_GET_FREE_COUNT());
 
      dsm_free_packet( &item_ptr );

      HDR_MSG_PROT_1(MSG_LEGACY_MED, "FTM Sync - free count after: %d", 
                     HDRDSM_GET_FREE_COUNT());

      return;
  }
#endif /* FEATURE_FACTORY_TESTMODE */

  /* This protocol runs in the HDRMC task.*/
  hdrmc_queue_msg( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
    HDRHAI_IN_USE, item_ptr );
} /* hdrinit_msg_cb() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_POWERUP_INIT

DESCRIPTION
  This function performs the powerup initialization.

DEPENDENCIES
  This function must called before the initialization state protocol can be
  activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_powerup_init ( void )
{
#ifdef FEATURE_LTE_TO_HDR
  nv_sd_cfg_items_s_type sd_cfg_items_nv;
/*----------------------------------------------------------------------- */

  memset ( &sd_cfg_items_nv, 0, sizeof(nv_sd_cfg_items_s_type) ); 

  if ( hdrutil_read_nv_item ( NV_SD_CFG_ITEMS_I, 
                      (nv_item_type *) &sd_cfg_items_nv) == NV_DONE_S )
  {
    /* BSR timer value is in the index 1 of the CFG NV item. The value is 
       in seconds convert to ms */
    hdrinit.hystr_timer_value = sd_cfg_items_nv.items[1] * 1000;
  }
  else
  {
    hdrinit.hystr_timer_value = HDRINIT_LTE_TO_HDR_HO_HYSTR_MSECS;
  }
  /* Reduce the timer by 10 sec, if hysteresis timer is atleast 100 sec */
  if(hdrinit.hystr_timer_value >= HDRINIT_MIN_LTE_TO_HDR_HO_HYSTR_MSECS)
  {
    hdrinit.hystr_timer_value = hdrinit.hystr_timer_value - 10000;
  }  
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_SCAN_MODE
  
  if ( (int)efs_get( HDRINIT_SCAN_MODE_SETTING_FILENAME,
	            	   &hdrinit.scan_mode, 
					   1 ) == 1 )
  {
	  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Read scan mode setting value: %d successfully", 
	                   hdrinit.scan_mode );
  }
  else
  {
	  HDR_MSG_PROT( MSG_LEGACY_HIGH, "Read scan mode setting value failed");
	  hdrinit.scan_mode = FALSE;
  }
  
  if(hdrinit.scan_mode == TRUE)
  {
	 if ( ((int)efs_get( HDRINIT_CUSTOM_BAND_NUM_FILENAME, &hdrinit.custom_channel_info.band,1) == 1) &&
		   ((int)efs_get( HDRINIT_CUSTOM_CHANNEL_NUM_FILENAME, &hdrinit.custom_channel_info.chan_num,2) == 2))
	 {
		 HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Read band %d Channel %d from EFS successfully", 
							 hdrinit.custom_channel_info.band, hdrinit.custom_channel_info.chan_num);
  
	 }
	 else
	 {
		 HDR_MSG_PROT( MSG_LEGACY_HIGH, "Read scan mode setting value failed");
		 hdrinit.scan_mode = FALSE;
		 /* Disable scan mode if NV read failed during channel read */
	 }
  }
#endif /* FEATURE_HDR_SCAN_MODE */

  if ( hdrmc_is_first_init() )
  {
    /* Initialize timers. */
    rex_def_timer_ex( &hdrinit.sync_msg_timer,
                      hdrinit_timer_cb,
                      HDRINIT_SYNC_MSG_TIMER );
  }
					
  if ( hdrlmac_is_lre_enabled() && hdrmc_is_first_init() )
  {
    rex_def_timer_ex( &hdrinit.sync_suspend_timer,
                      hdrinit_timer_cb,
                      HDRINIT_SYNC_SUSPEND_TIMER );
  }

#ifdef FEATURE_LTE_TO_HDR
  if ( hdrmc_is_first_init() )
  {
    rex_def_timer_ex( &hdrinit.lte_to_hdr_ho_hystr_timer,
                      hdrinit_timer_cb,
                      HDRINIT_LTE_TO_HDR_HO_HYSTR_TIMER );
  }
  hdrinit_set_hystr_timer_is_expired(FALSE);
#endif /* FEATURE_LTE_TO_HDR */

  HDR_MSG_PROT (MSG_LEGACY_LOW, "INIT Powerup Init");

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrinit.hrpd_meas_enabled = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrinit_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_INIT

DESCRIPTION
  This function initializes the initialzation state protocol.

DEPENDENCIES
  This function must called before the initialization state protocol can be
  activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_init ( void )
{

  hdrinit.rev_num             = HDRHAI_REVISION_NUMBER;
  hdrinit.syn_msg.valid_msg   = FALSE;

  hdrinits_init();

  HDR_MSG_PROT (MSG_LEGACY_LOW, "INIT Initialized");
} /* hdrinit_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_ACTIVATE

DESCRIPTION
  This function commands the initialization state protocol to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_activate ( hdrhai_protocol_name_enum_type sender )
{
  hdrinit_cmd_type hdrinit_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_MED, "Activate INIT");

  /*-------------------------------------------------------------------------
     Fill in the command ID
   -------------------------------------------------------------------------*/

  hdrinit_cmd.cmd = HDRINIT_ACTIVATE_CMD;
  hdrinit_cmd.sender = sender;

  if (rex_self() == HDRINIT_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrinit_process_cmd( &hdrinit_cmd );
  }
  else
  {
    /* Currently no other task ever activates INIT, so we should never
     * get here
     */
    ERR("INIT activated from outside MC!", 0, 0, 0);


    hdrmc_queue_cmd( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                     &hdrinit_cmd,
                     sizeof(hdrinit_cmd_type) );
  }

} /* hdrinit_activate() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_DEACTIVATE

DESCRIPTION
  This function commands the initialization state protocol to deactivate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_deactivate ( hdrhai_protocol_name_enum_type sender )
{
  hdrinit_cmd_type hdrinit_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_MED, "Deactivate INIT");

  hdrinit_cmd.cmd = HDRINIT_DEACTIVATE_CMD;
  hdrinit_cmd.sender = sender;

  if (rex_self() == HDRINIT_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrinit_process_cmd( &hdrinit_cmd );
  }
  else
  {
    /* Currently no other task ever deactivates INIT, so we should never
     * get here
     */
    ERR("INIT deactivated from outside MC!", 0, 0, 0);

    /*-------------------------------------------------------------------------
        Fill in the command ID
    -------------------------------------------------------------------------*/

    hdrmc_queue_cmd( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                     &hdrinit_cmd,
                     sizeof(hdrinit_cmd_type) );
  }
} /* hdrinit_deactivate() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_REACQUIRE

DESCRIPTION
  This function commands the initialization state protocol to reacquire.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_reacquire ( hdrhai_protocol_name_enum_type sender )
{
  hdrinit_cmd_type hdrinit_cmd; /* Build command into local struct */

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "INIT Reacquire");

  /*-------------------------------------------------------------------------
     Fill in the command ID
   -------------------------------------------------------------------------*/

  hdrinit_cmd.cmd = HDRINIT_REACQUIRE_CMD;
  hdrinit_cmd.sender = sender;

  if (rex_self() == HDRINIT_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrinit_process_cmd( &hdrinit_cmd );
  }
  else
  {
    /* Currently no other task ever activates INIT, so we should never
     * get here
     */
    ERR("INIT activated from outside MC!", 0, 0, 0);


    hdrmc_queue_cmd( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                     &hdrinit_cmd,
                     sizeof(hdrinit_cmd_type) );
  }

} /* hdrinit_reacquire() */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRINIT_CONTINUE_ACQUISITION

DESCRIPTION
  This function commands the initialization state protocol to continue with
  acquisition.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_continue_acquisition ( hdrhai_protocol_name_enum_type sender )
{
  hdrinit_cmd_type hdrinit_cmd; /* Build command into local struct */

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "INIT Continue Acq");

  /*-------------------------------------------------------------------------
     Fill in the command ID
   -------------------------------------------------------------------------*/

  hdrinit_cmd.cmd = HDRINIT_CONTINUE_ACQ_CMD;
  hdrinit_cmd.sender = sender;

  if (rex_self() == HDRINIT_TASK_PTR)
  {
    /* call the processing function directly if in the same task */
    hdrinit_process_cmd( &hdrinit_cmd );
  }
  else
  {
    /* Currently no other task ever activates INIT, so we should never
     * get here
     */
    ERR("INIT activated from outside MC!", 0, 0, 0);

    hdrmc_queue_cmd( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                     &hdrinit_cmd,
                     sizeof(hdrinit_cmd_type) );
  }

} /* hdrinit_continue_acquisition() */
#endif /* FEATURE_LTE_TO_HDR_OH */


/*===========================================================================

FUNCTION HDRINIT_START_SYNC_MSG_TIMER

DESCRIPTION
  This function starts the sync msg timer if it is not active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_start_sync_msg_timer ( void )
{
#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() != FTM_MODE )
#endif
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Start Sync msg timer");
    if( (hdrinit.saved_syncmsg_timer_ms != 0) && 
                           hdrlmac_is_lre_enabled() )
	{
	 ( void ) rex_set_timer( &hdrinit.sync_msg_timer,
                       hdrinit.saved_syncmsg_timer_ms );
	 hdrinit.saved_syncmsg_timer_ms = 0;
	}
	else
	{    
       ( void ) rex_set_timer( &hdrinit.sync_msg_timer,
                       HDRINIT_SYNC_MSG_TIMER_MSECS );
	}
  }

} /* hdrinit_start_sync_msg_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_STOP_SYNC_MSG_TIMER

DESCRIPTION
  This function stops the sync msg timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_stop_sync_msg_timer(void)
{
#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() != FTM_MODE )
#endif
  {
    if( rex_get_timer( &hdrinit.sync_msg_timer) !=0 )
    {
      /* stop the timer if it is active */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Stop Sync msg timer");
      ( void ) rex_clr_timer( &hdrinit.sync_msg_timer);
    }
  }
}/* hdrinit_stop_sync_msg_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PAUSE_AND_STOP_SYNC_MSG_TIMER

DESCRIPTION
  This function pauses and stops the sync msg timer 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_pause_and_stop_sync_msg_timer(void)
{
#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() != FTM_MODE )
#endif
  {  
         HDR_MSG_PROT (MSG_LEGACY_HIGH, "Pause and stop Sync timer");   
     hdrinit.saved_syncmsg_timer_ms = rex_get_timer(&hdrinit.sync_msg_timer);
     hdrinit_stop_sync_msg_timer();
  }
}/* hdrinit_pause_sync_msg_timer() */

/*===========================================================================

FUNCTION HDRINIT_RESET_SAVED_SYNC_MSG_TIMER

DESCRIPTION
  This function resets the saved sync msg timer 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_reset_saved_sync_msg_timer(void)
{
     hdrinit.saved_syncmsg_timer_ms = 0;
}/* hdrinit_reset_saved_sync_msg_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_START_SYNC_SUSPEND_TIMER

DESCRIPTION
  This function starts the sync suspend timer 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_start_sync_suspend_timer ( void )
{
#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() != FTM_MODE )
#endif
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Start Sync suspend timer");
    ( void ) rex_set_timer( &hdrinit.sync_suspend_timer,
                    HDRINIT_SYNC_SUSPEND_TIMER_MSECS );
  }

} /* hdrinit_start_sync_suspend_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_STOP_SYNC_SUSPEND_TIMER

DESCRIPTION
  This function stops the sync suspend timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_stop_sync_suspend_timer(void)
{
#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() != FTM_MODE )
#endif
  {
    if( rex_get_timer( &hdrinit.sync_suspend_timer) !=0 )
    {
      /* stop the timer if it is active */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Stop Sync suspend timer");
      ( void ) rex_clr_timer( &hdrinit.sync_suspend_timer);
    }
  }
}/* hdrinit_stop_sync_suspend_timer() */

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRINIT_START_LTE_TO_HDR_HO_HYSTR_TIMER

DESCRIPTION
  This function starts the LTE to HDR handoff hystersis timer if it is not 
  active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_start_lte_to_hdr_ho_hystr_timer ( void )
{
  /* Make sure the hystr_timer_is_expired flag is reset. */
  hdrinit_set_hystr_timer_is_expired(FALSE);

  if ( hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
  {
    /* For RTT, we use a short and hardcoded hystersis timer value.
     * HDR SRCH takes 2s to acq and sync at maximum. SRCH returns failure if
     * acq and sync take longer. This is the reason we put 2.5s as the 
     * RTT hystersis timer. */ 
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Start LTE to HDR hystr timer 2.5s for RTT" );
    ( void ) rex_set_timer( &hdrinit.lte_to_hdr_ho_hystr_timer,
                            HDRINIT_LTE_TO_HDR_HO_RTT_HYSTR_MSECS );
  }
  else
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Start LTE to HDR hystr timer in ms=%d",
                    hdrinit.hystr_timer_value);
    ( void ) rex_set_timer( &hdrinit.lte_to_hdr_ho_hystr_timer,
                            hdrinit.hystr_timer_value );
  }
} /* hdrinit_start_lte_to_hdr_ho_hystr_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_STOP_LTE_TO_HDR_HO_HYSTR_TIMER

DESCRIPTION
  This function stops the LTE to HDR handoff hystersis timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_stop_lte_to_hdr_ho_hystr_timer(void)
{
  if( rex_get_timer( &hdrinit.lte_to_hdr_ho_hystr_timer) !=0 )
  {
    /* stop the timer if it is active */
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Stop LTE to HDR hystr timer");
    ( void ) rex_clr_timer( &hdrinit.lte_to_hdr_ho_hystr_timer);
  }
}/* hdrinit_stop_lte_to_hdr_ho_hystr_timer() */
#endif /* FEATURE_LTE_TO_HDR */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_GET_SYNC_TIME

DESCRIPTION
  This function returns the 64-bit system time in units of 26.666msec time.

DEPENDENCIES
  This function must be called shortly after a network acquired indication is
  received (valid only when a sync msg is received)

PARAMETERS
  None

RETURN VALUE
  The current system time right after acquisition

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_get_sync_time
(
  qword sys_time
   /* address of the qword in which to return the 64-bit time. */
)
{
  qw_equ(sys_time,hdrinit.syn_msg.sys_time);
}/* hdrinit_get_sync_time */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_CMD

DESCRIPTION
  This function processes commands given to the initialization state protocol.

DEPENDENCIES
  None

PARAMETERS
  init_cmd_ptr - Pointer to the command for the initialization state protocol
            to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_process_cmd
(
     hdrinit_cmd_type *init_cmd_ptr
)
{

  HDRTRACE_CMD( HDRHAI_INITIALIZATION_STATE_PROTOCOL, init_cmd_ptr->cmd,
    init_cmd_ptr->sender, hdrinits_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( init_cmd_ptr->cmd )
  {
    case HDRINIT_ACTIVATE_CMD:

      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Receive HDRINIT_ACTIVATE_CMD");
      /* process the activate cmd */
      hdrinit_process_activate_cmd();
      break;

    case HDRINIT_DEACTIVATE_CMD:

      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Receive HDRINIT_DEACTIVATE_CMD");
      /* process the deactivate cmd */
      hdrinit_process_deactivate_cmd();
      break;

    case HDRINIT_REACQUIRE_CMD:

      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Receive HDRINIT_REACQUIRE_CMD");
      /* process the reacquire cmd */
      hdrinit_process_reacquire_cmd();
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRINIT_CONTINUE_ACQ_CMD:

      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Receive HDRINIT_CONTINUE_ACQ_CMD");
      /* process the continue acquisition cmd */
      hdrinit_process_continue_acq_cmd();
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:

      ERR ("Invalid commands for the INIT protocol",0,0,0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrinit_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_PROCESS_IND

DESCRIPTION
  This function processes indications given to INIT.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for INIT to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_process_ind
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_IND( HDRHAI_INITIALIZATION_STATE_PROTOCOL, ind_name,
    hdrinits_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( ind_name )
  {
    case HDRIND_LMAC_ACQUIRED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRIND_LMAC_ACQUIRED");
      hdrinit_process_pilot_acq_result(ind_name, ind_data_ptr, HDRINIT_ACQ_SUCCESSFUL);
      break;

    case HDRIND_LMAC_ACQUISITION_FAILED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRIND_LMAC_ACQUISITION_FAILED");
      hdrinit_process_pilot_acq_result(ind_name, ind_data_ptr, HDRINIT_ACQ_FAILED);
      break;

    case HDRIND_LMAC_CHAN_MEAS_COMPLETE:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRLMAC_IND_CHAN_MEASUREMENT");
      hdrinit_process_chan_meas(ind_name, ind_data_ptr);
      break;

    case HDRIND_LMAC_TIMING_INFO:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRIND_LMAC_TIMING_INFO");
      hdrinit_process_timing_info(ind_name, ind_data_ptr);
      break;
	  
#ifdef FEATURE_LTE_TO_HDR
    case HDRIND_LMAC_TIME_SYNC_DONE:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRIND_LMAC_TIME_SYNC_DONE");
      hdrinit_process_time_sync(ind_name, ind_data_ptr);
      break;

    case HDRIND_LMAC_FAST_ACQ_DONE:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRIND_LMAC_FAST_ACQ_DONE");
      hdrinit_process_fast_acq(ind_name, ind_data_ptr);
      break;

#endif /* FEATURE_LTE_TO_HDR */

    case HDRIND_LMAC_SYNC_SUSPEND:	
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRIND_LMAC_SYNC_SUSPEND");
      hdrinit_process_lmac_sync_ind(ind_name, HDRINIT_SYNC_SUSPEND);
	  break;
	  
    case HDRIND_LMAC_SYNC_RESUME:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Received HDRIND_LMAC_SYNC_RESUME");
      hdrinit_process_lmac_sync_ind(ind_name, HDRINIT_SYNC_RESUME);
	  break;

    default:
      break;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrinit_process_ind() */ //lint !e715

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRINIT.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrinit_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void                   *ind_data_ptr
)
{
  /* This protocol runs in the HDRMC task.*/
  if (rex_self() == HDRINIT_TASK_PTR)
  {
    /* Indication is from another protocol in MC task, so consider it
     * an internal indication
     */
    hdrinit_process_ind( ind_name, (hdrind_ind_data_union_type *)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_INITIALIZATION_STATE_PROTOCOL, ind_name,
      ind_data_ptr );
  }
} /* hdrinit_ind_cb() */


/* <EJECT> */
/*===========================================================================
FUNCTION HDRINIT_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the init state
  protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrinit_process_timer
(
  uint32 timer_id
)
{
  hdrinits_state_enum_type cur_state = hdrinits_get_state();
                                       /* current init state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( (hdrinit_timer_enum_type) timer_id )
  {
    case HDRINIT_SYNC_MSG_TIMER:
      HDRTRACE_HDRMC_INIT_SYNC_MSG_TIMER_SIG( cur_state );
      hdrinit_process_sync_msg_timeout();
      break;

#ifdef FEATURE_LTE_TO_HDR
    case HDRINIT_LTE_TO_HDR_HO_HYSTR_TIMER:
      hdrinit_process_lte_to_hdr_ho_hystr_timeout();
      break;
#endif /* FEATURE_LTE_TO_HDR */

case HDRINIT_SYNC_SUSPEND_TIMER:      
      if ( hdrlmac_is_lre_enabled() )  
      {
      hdrinit_process_sync_suspend_timeout();
      }
      break;

    default :
      ERR("Invalid INIT protocol timer (%d)!",timer_id,0,0);
      break;
  }
}  /* hdrinit_process_timer() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRINIT_TIMER_CB

DESCRIPTION
  This function is the callback for all init state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC).

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrinit_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_INITIALIZATION_STATE_PROTOCOL, timer_id);
} /* hdrinit_timer_cb() */

/* <EJECT> */
#ifndef PERLUTF_TDTGEN_EXCLUDE
/*===========================================================================

FUNCTION HDRINIT_PROCESS_MSG

DESCRIPTION
  This function processes messages for INIT.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
    hdrerrno_enum_type  status = E_SUCCESS; /* message unpacking status */
#ifdef FEATURE_HDR_SYNC_CHECK
    uint32                               hi_sys_time;
    uint32                               lo_sys_time;
    hdrdec_half_slot_time_type           time_stamp;
#endif /* FEATURE_HDR_SYNC_CHECK */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    ASSERT((protocol_instance == HDRHAI_IN_USE) ||
           (protocol_instance == HDRHAI_IN_CONFIG));

   if (hdrinits_get_state() != HDRINITS_SYNC_STATE)
   {
      /* rcv the message in the wrong state */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Discard superflous Sync Msg");
#ifdef FEATURE_HDR_SYNC_CHECK
      /* Still parse the message and send it to searcher */
      status =  HDRINIT_GET_SYNC_MSG_ID( item_ptr, &(hdrinit.syn_msg.msg_id));
      status |= HDRINIT_GET_PILOT_PN(item_ptr, &(hdrinit.syn_msg.pilot_pn));
      status |= HDRINIT_GET_HI_SYS_TIME(item_ptr, &(hi_sys_time));
      status |= HDRINIT_GET_LO_SYS_TIME(item_ptr, &(lo_sys_time));
      status |= hdrcmac_get_sync_caps_timestamp (HDRDSM_GET_TIMESTAMP(item_ptr), &time_stamp);
      if (( hdrinit.syn_msg.msg_id == HDRINIT_SYNC_MSG_ID ) &&
          ( status == E_SUCCESS ) &&
          ( protocol_instance == HDRHAI_IN_USE ))
      {
        qw_set(hdrinit.syn_msg.sys_time, hi_sys_time, lo_sys_time);
        hdrsrch_synchronize( hdrinit.syn_msg.sys_time,
                             hdrinit.syn_msg.pilot_pn, time_stamp, NULL );
      }
#endif /* FEATURE_HDR_SYNC_CHECK */
      /* release the item buffer */
      dsm_free_packet( &item_ptr );
      return;
   }

   status =  HDRINIT_GET_SYNC_MSG_ID( item_ptr, &(hdrinit.syn_msg.msg_id));

   if ( status != E_SUCCESS )
   {
      /* Rcv an unknown msg ID */
      ERR("Unable to unpack the MSD ID",0,0,0);
      /* release the item buffer */
      dsm_free_packet( &item_ptr );
      return;
   }

   /* Currently only the in use instance receives any messages */
   if (protocol_instance != HDRHAI_IN_USE)
   {
     ERR("Msg for wrong instance %d", hdrinit.syn_msg.msg_id, 0, 0 );
     dsm_free_packet( &item_ptr );
     return;
   }

   HDRTRACE_MSG( HDRHAI_INITIALIZATION_STATE_PROTOCOL,
     hdrinit.syn_msg.msg_id, hdrinits_get_state() );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

   if ( hdrinit.syn_msg.msg_id != HDRINIT_SYNC_MSG_ID )
   {
      /* Rcv an unknown msg ID */
      ERR("%d is an unknown Msg ID",
          hdrinit.syn_msg.msg_id,0,0);  /*lint !e516 */
   }
   else
   {
     HDR_MSG_PROT (MSG_LEGACY_HIGH, "Sync message received");
     /* process the sync message */
     hdrinit_process_sync_msg(item_ptr);
   }

   dsm_free_packet( &item_ptr );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrinit_process_msg() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */





