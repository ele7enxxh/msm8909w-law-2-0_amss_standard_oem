#ifndef HDRSRCHCMD_H
#define HDRSRCHCMD_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R   S E A R C H   T A S K   C M D   Q U E U E

                       HDR Search Command Header File

GENERAL DESCRIPTION
  This file contains prototypes and declarations for sending commands to
  the search task.

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrsrchcmd_init( ) - Initialization
  hdrsrchcmd_new( )  - Extracts a command buffer from free queue
  hdrsrchcmd_post( ) - Places cmd buffer on command queue & signals task
  hdrsrchcmd_get( )  - Extracts a command buffer from command queue.
  hdrsrchcmd_free( ) - Returns buffer to free queue.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrsrchcmd_init( )

  Copyright (c) 2000 - 2015 
                by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrchcmd.h#1 $
$DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
05/07/2015   vlc     Added HDRSRCH_ABORT_LTE_RESELECT_CMD. 
04/29/2015   vko     Inform TCA processing status, to HDRSRCH during IDLE_MODE_CMD
02/13/2015   vke     Promotion for CR790412
02/04/2015   vke     Changes to ignore sync cmd if processing delay > SCC
12/03/2014   ljl     Declared system lost if CC packet HSTR was incorrect.
11/04/2014   arm     Band avoidance: move power info saving to hdrsrch task and
                     enable BA only for DSDA mode.
05/15/2014   arm     TRM API changes for DR-DSDS. 
05/05/2014   ljl     Added HDRSRCH_CHANNEL_CHANGE_AND_NSET_UPDATE_CMD.
11/14/2013   arm     Merged wakeup manager changes for DSDS.
11/27/2013   dsp/cnx FR17482 for Page blocking management.
09/03/2013   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
05/02/2013   ljl     Added HDRSRCH_SET_SCC_INFO_CMD.
02/26/2013   mbs     Fixed featurization for IRAT code
11/19/2012   smd/dsp Added wakeup conflict adjustment for DIME.
03/08/2012   wsh     Merged NLB support
10/24/2011   ljl     Supported RPM optimized timeline.
09/23/2011   ljl     Added HDRSRCH_SET_CC_START_CMD, HDRSRCH_SET_SCC_OFFSET_CMD,
                     and HDRSRCH_AT_SET_DRC_CMD.
04/27/2011   rmg     Added HDR MCPM support changes.
03/02/2011   vlc     Modified hdrsrch_cmd_type and put back memory heap changes.
01/06/2011   vlc     Switched messages from LTE for DO->LTE reselection to
                     low priority queue.
11/18/2010   vlc     Reverted memory heap changes.
09/10/2010   pxu     Memory heap changes.
09/01/2010   vlc     Added support for DO->LTE pilot measurements/reselection.
05/01/2010   smd     Moved acquire command params structure to hdrsrchtypes.h.
                     Adding retuning_timing_cb in sync params.
03/24/2010   pxu     Removed FEATURE_HDR_QSM features 
03/10/2010   smd     Updated implementation for commands from message route.
03/05/2010   smd     Fixed bugs in LTE pilot measurement feature.
12/29/2009   ljl     Added API to enable firmware HDR App.
09/02/2009   smd     Added messages for LTE to DO pilot measurement.
07/22/2009   ljl     Passed the frame offset to HDR SRCH.
03/20/2009   rkc     Added rup_subtype to hdrsrch_nset_update_params_type.
03/09/2009   smd     Added support for pilot measurement velcro solution.
11/10/2008   ljl     Added HDRSRCH_ACMAC_DISABLE_TA_CMD.
03/27/2008   ljl     Added the avoided channel list to nset update.
10/17/2007   jyw     Added revB set management support.
10/09/2008   lyl     Added qpch_enabled in sleep parameter struct.
09/18/2008   rkc     Added intra-scc data to hdrsrch_sleep_params_type.
08/19/2007   grl     Resolved Lint High warnings.
04/27/2007   grl     Removed tc_mode from TC assignment for access hybrid ctrl.
04/10/2007   mt      Added HDRSRCH_RUP_REQ_CMD.
10/06/2006   grl     Added passing of the negotiated SCI value to hdrsrchsleep.
08/28/2006   sq      Added FEATURE_HDR_QSM
05/03/2006   ljl     Added support for hybrid access
03/22/2006   sq      Added HDRSRCH_UPDATE_BOM_PERIOD_CMD
09/10/2005   grl     Added dynamic TC mode support for HPTS mode.
08/18/2005   ljl     Added cached sector info
07/28/2005   ds      Added hash channel list to NSET update parameters
07/12/2005   sq      Added support for enhanced idle state protocol
03/23/2005   hrk     Added function hdrsrchcmd_purge().
12/02/2004   sq      Obsoleted ACCESS_END command
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
08/30/2004   aaj     Added enum type for bcmcs status
07/30/2003   sq      Added command HDRSRCH_UPDATE_BC_INFO_CMD
11/18/2003   aaj     Added system usability callback
10/09/2003   ajn     Added support for "Advise Unlock RF" command
03/11/2003   ajn     Updated Copyright notice
01/10/2002   ajn     Added FEATURE_HDR_NO_HO_DURING_SCC
11/13/2002   aaj     Added support for acquisition mode
09/09/2002   aaj     Added command number comments for command enums
06/14/2002   ajn     Access H/O Threshold added to ACCESS_MODE_CMD
05/01/2002   aaj     Added HDR traffic tune away selection enum to tca cmd
02/06/2002   ajn     Suspend callbacks added to Idle/TC cmds.
01/30/2002   ajn     Added status to command buffers
01/17/2002   aaj     Added tc ack recived command type for tc hybrid operation
12/07/2001   ajn     Added Pilot to Channel Change command.
11/30/2000   ajn     Code review changes / Directed NSET msg support
07/07/2000   ajn     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "hdrsrchtypes.h"
#include "IxErrno.h"
#include "queue.h"
#include "msgr.h"
#include "hdrsrch_msg.h"
#include "trm.h"
/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/
#define HDRSRCHCMD_QUEUE_SIZE  10
  /* Maximum size of command queue */

#define HDRSRCHCMD_CMD_MASK        0xFFFF
  /* Mask to get mask out high word of command id */

/*--------------------------------------------------------------------------
                               SRCH COMMANDS
--------------------------------------------------------------------------*/

/********** Command types **********/

typedef enum
{
  HDRSRCH_ACQUIRE_CMD,             /* 0  */
  HDRSRCH_SYNCHRONIZE_CMD,         /* 1  */
  HDRSRCH_IDLE_CMD,                /* 2  */
  HDRSRCH_NSET_UPDATE_CMD,         /* 3  */
  HDRSRCH_CHANNEL_CHANGE_CMD,      /* 4  */
  HDRSRCH_SLEEP_CMD,               /* 5  */
  HDRSRCH_ACCESS_CMD,              /* 6  */
  HDRSRCH_ACCESS_HANDOFF_CMD,      /* 7  */
  HDRSRCH_ACCESS_END_CMD_OBSOLETE, /* 8  */
  HDRSRCH_TC_ASSIGNMENT_CMD,       /* 9  */
  HDRSRCH_ASET_UPDATE_CMD,         /* 10 */
  HDRSRCH_PILOT_REPORT_CMD,        /* 11 */
  HDRSRCH_PILOT_REPORT_RESET_CMD,  /* 12 */
  HDRSRCH_TC_ACK_RECEIVED_CMD,     /* 13 */
  HDRSRCH_DEACTIVATE_CMD,          /* 14 */
  HDRSRCH_REENABLE_HANDOFFS_CMD,   /* 15 */
  HDRSRCH_ADVISE_UNLOCK_RF_CMD,    /* 16 */
#ifdef FEATURE_HDR_BCMCS
  HDRSRCH_UPDATE_BC_INFO_CMD,      /* 17 */
#else  /* FEATURE_HDR_BCMCS */
  HDRSRCH_RESERVED17_CMD,
#endif /* FEATURE_HDR_BCMCS */
  HDRSRCH_TC_MODE_CMD,             /* 18 */
#ifdef FEATURE_HDR_BCMCS
  HDRSRCH_UPDATE_BOM_PERIOD_CMD,   /* 19 */
#else  /* FEATURE_HDR_BCMCS */
  HDRSRCH_RESERVED19_CMD,
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_HYBRID_ACCESS
  HDRSRCH_ACMAC_READY_CMD,         /* 20 */
  HDRSRCH_ACMAC_TA_TIME_OK_CMD,    /* 21 */
#else
  HDRSRCH_RESERVED20_CMD,
  HDRSRCH_RESERVED21_CMD,
#endif /* FEATURE_HDR_HYBRID_ACCESS */

  HDRSRCH_RESERVED22_CMD,

  HDRSRCH_RUP_REQ_CMD,             /* 23 */

#ifdef FEATURE_HDR_HYBRID_ACCESS
  HDRSRCH_ACMAC_DISABLE_TA_CMD,     /* 24 */
#else
  HDRSRCH_RESERVED24_CMD,
#endif /* FEATURE_HDR_HYBRID_ACCESS */

  HDRSRCH_PILOT_MEAS_CMD,           /* 25 */

  HDRSRCH_ENABLE_FW_HDR_APP,         /* 26 */

  HDRSRCH_FAST_ACQ_CMD,              /* 27 */

#ifdef FEATURE_HDR_TO_LTE
  HDRSRCH_LTE_NSET_UPDATE_CMD,     /* 28 */
  HDRSRCH_LTE_TRESELECT_EXP_CMD,   /* 29 */
#endif /* FEATURE_HDR_TO_LTE */

  HDRSRCH_SET_CC_START_CMD,            /* 30 */
  HDRSRCH_SET_SCC_OFFSET_CMD,          /* 31 */
  HDRSRCH_AT_SET_DRC_CMD,              /* 32 */

  HDRSRCH_CHANGE_SLEEP_TIMELINE_CMD, /* 33 */

#ifdef FEATURE_HDR_REVC
  HDRSRCH_ENABLE_NLB_CMD,            /* 34 */
  HDRSRCH_UPDATE_LOADADJ_CMD,        /* 35 */
#else
  HDRSRCH_RESERVED34_CMD,
  HDRSRCH_RESERVED35_CMD,
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_DIME_MODEM
  HDRSRCH_WAKEUP_ADJUST_CMD,         /* 36 */
#else
  HDRSRCH_RESERVED36_CMD,
#endif /* FEATURE_HDR_DIME_MODEM */
  HDRSRCH_SET_SCC_INFO_CMD,          /* 37 */

#ifdef FEATURE_HDR_WAKEUP_MANAGER
  HDRSRCH_EXTEND_SLEEP_CMD,          /* 38 */
#else
  HDRSRCH_RESERVED38_CMD,
#endif /* FEATURE_HDR_WAKEUP_MANAGER */ 

  HDRSRCH_CHANNEL_CHANGE_AND_NSET_UPDATE_CMD, /* 39 */

  HDRSRCH_BAND_GRANT_RCVD_CMD, /* 40 */

  HDRSRCH_SYSTEM_LOST_CMD,           /* 41 */

  HDRSRCH_BAND_AVOID_TIMER_CMD, /* 42 */

#ifdef FEATURE_HDR_TO_LTE
  HDRSRCH_ABORT_LTE_RESELECT_CMD,    /* 43 */
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
  /* Above are commands defined by HDR SRCH */
  /* Below are commands generated by message router */
  /* The high word of UMID generated by message route is TECH + Module, the low
     word of UMID is message type + message id. Low byte of HDR_SRCH_xxx_CMD
     is from 0x0100 to 0x199. Low wor of HDR_SRCH_xxx_REQ is from 0x200 to
     0x299. Low word of HDR_SRCH_xxx_RSP is from 0x300 to 0x399
  */
  HDRSRCH_TRAN_SYS_TIME_REQ = HDR_SRCH_TRAN_SYS_TIME_REQ & HDRSRCHCMD_CMD_MASK,    /* 0x200 */      
  HDRSRCH_LTE_PILOT_MEAS_REQ = HDR_SRCH_LTE_PILOT_MEAS_REQ & HDRSRCHCMD_CMD_MASK,   /* 0x201 */        
  HDRSRCH_LTE_PILOT_MEAS_ABORT_REQ = HDR_SRCH_LTE_PILOT_MEAS_ABORT_REQ & HDRSRCHCMD_CMD_MASK,  /* 0x202 */    
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */

  HDRSRCH_MAX_CMD = 0xFFFFFFFF /* Force this enum type to be 32 bits */
}
hdrsrch_cmd_name_enum_type;


/*--------------------------------------------------------------------------
                           SRCH COMMAND PARAMETERS
--------------------------------------------------------------------------*/

/* HDRSRCH_SYNCHRONIZE_CMD */

typedef struct
{
  msgr_hdr_struct_type            msg_hdr;      /* Message header */
  uint64                          sync_time;    /* 26.7ms frames since UTC=0 */
  sys_pilot_pn_type               pilot_pn;     /* Pilot SYNC msg came from */
  uint16                          half_slot;    /* HSTR value @ start of SCC */
  hdrsrch_return_timing_cb_type   return_timing_cb; 
                                               /* Callback to return CDMA timing */
  timetick_type                   sync_cmd_delay;
                                               /* delay in processing cmd*/

}
hdrsrch_synchronize_params_type;


/* HDRSRCH_IDLE_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_idle_ho_cb_type           idle_ho_cb;
  hdrsrch_suspend_cb_type           suspend_cb;
  boolean                           is_tca_processing;
}
hdrsrch_idle_params_type;


/* HDRSRCH_NSET_UPDATE_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_nset_pilot_type           nset_pilots[ HDRSRCH_NSET_MAX_SIZE ];
  uint8                             num_pilots;
  hdrsrch_nset_update_enum_type     update_type;
  sys_link_type                     cached_sects[ HDRSRCH_MAX_CACHE_RECORDS ];
  uint8                             num_sects;

  sys_channel_type                  channel_list[ HDRSRCH_HASH_CHAN_MAX_SIZE ];
    /* Hash channel list from Sector Parameters message*/
  uint8                             num_channels;
    /* Number of hash channels */

  uint8                             num_avoided_channels;
    /* Number of avoided channels */
  sys_channel_type                  avoided_channel_list[ HDRSRCH_AVOIDED_CHAN_MAX_SIZE ];
    /* List of avoided channels */

#ifdef FEATURE_HDR_REVB
  hdrsrch_pilot_gid_type            serving_pilot_gid;
    /* The group id of the pilot from which the neighbor info is received */

  sys_active_prot_e_type            hdr_subtype;
    /* HDR Protocol subtype */

#endif /* FEATURE_HDR_REVB */
}
hdrsrch_nset_update_params_type;


/* HDRSRCH_CHANNEL_CHANGE_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  sys_link_type                     link;
  hdrsrch_chan_change_rpt_cb_type   chan_change_rpt;
}
hdrsrch_channel_change_params_type;

/* HDRSRCH_CHANNEL_CHANGE_AND_NSET_UPDATE_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */

  /* Idle channel change params must be first */
  sys_link_type                     link;
  hdrsrch_chan_change_rpt_cb_type   chan_change_rpt;

  /* Nset update params */
  hdrsrch_nset_pilot_type           nset_pilots[ HDRSRCH_NSET_MAX_SIZE ];
  uint8                             num_pilots;
  hdrsrch_nset_update_enum_type     update_type;
  sys_link_type                     cached_sects[ HDRSRCH_MAX_CACHE_RECORDS ];
  uint8                             num_sects;

  sys_channel_type                  channel_list[ HDRSRCH_HASH_CHAN_MAX_SIZE ];
    /* Hash channel list from Sector Parameters message*/
  uint8                             num_channels;
    /* Number of hash channels */

  uint8                             num_avoided_channels;
    /* Number of avoided channels */
  sys_channel_type                  avoided_channel_list[ HDRSRCH_AVOIDED_CHAN_MAX_SIZE ];
    /* List of avoided channels */

#ifdef FEATURE_HDR_REVB
  hdrsrch_pilot_gid_type            serving_pilot_gid;
    /* The group id of the pilot from which the neighbor info is received */

  sys_active_prot_e_type            hdr_subtype;
    /* HDR Protocol subtype */

#endif /* FEATURE_HDR_REVB */

}
hdrsrch_channel_change_and_nset_update_params_type;

typedef struct
{
  trm_client_enum_t client;    /* Client requested for the band change */
  trm_band_type band;          /* Band class */
  trm_modify_return_enum_type granted;   /* Status of band grant */
  trm_request_tag_t tag;

}
hdrsrch_band_grant_params_type;
/* HDRSRCH_SLEEP_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint8                             hdr_slot_cycle;
    /* Indicates when to wakeup.
       Note: Not used if cc_start_slot_valid is TRUE */

  uint8                             hdr_neg_slot_cycle;
    /* What is the current negotiated SCI which can be different from what
       the protocols are asking for this particular sleep cycle. */

  boolean                           qpch_enabled;
    /* Whether QPCH is enabled or not for next wake-up */

  hdrsrch_wakeup_cb_type            wakeup_cb;
    /* Callback function to indicate when AT awakes. */

  boolean                           cc_start_slot_valid;
    /* Flag used to indicate whether cc_start_slot field is valid */

  uint16                            cc_start_slot;
    /* SCC start in slots.  This field is valid if cc_start_slot_valid
       is TRUE.*/

#ifdef FEATURE_HDR_TO_LTE
  uint8                             ornl_cycle;
    /* Other RAT (Radio Access Technologies) Neighbor List message
       has its own cycle */
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
  boolean                           session_is_open;
    /* whether EVDO session is open */

  boolean                           ovhd_is_updated;
    /* whether OVHD info is updated */
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */

}
hdrsrch_sleep_params_type;


/* HDRSRCH_ACCESS_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_suspend_ho_cb_type        suspend_ho_cb;
  hdrsrch_request_ho_cb_type        request_ho_cb;
  int                               thresh_0p5_db;
}
hdrsrch_access_params_type;


/* HDRSRCH_ACCESS_HANDOFF_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  boolean                           use_new_pilot;
  sys_link_type                     new_pilot;
  hdrsrch_access_ho_cb_type         access_ho_cb;
}
hdrsrch_access_handoff_params_type;


/* HDRSRCH_TC_ASSIGNMENT_CMD */
#ifndef FEATURE_HDR_REVB
typedef struct
{
  msgr_hdr_struct_type                 msg_hdr;      /* Message header */
  sys_channel_type                     channel;
  hdrsrch_aset_pilot_type              aset_pilots[ HDRSRCH_ASET_MAX_SIZE ];
  uint8                                num_pilots;
  hdrsrch_tca_cb_type                  tc_assign_cb;
  hdrsrch_suspend_cb_type              suspend_cb;
  hdrsrch_traffic_tune_away_enum_type  tc_tune_away;
}
hdrsrch_tc_assignment_params_type;
#else
typedef struct
{
  msgr_hdr_struct_type                 msg_hdr;      /* Message header */
  sys_active_prot_e_type               hdr_subtype;
  hdrsrch_subactive_set_type           subaset_list[ HDRSRCH_MAX_NUM_SUBASETS ];
  uint8                                num_subaset;
  uint8                                cc_subactive_set_idx;
  hdrsrch_tca_cb_type                  tc_assign_cb;
  hdrsrch_schdgrp_rpt_cb_type          schdgrp_rpt_cb;
  hdrsrch_suspend_cb_type              suspend_cb;
  hdrsrch_traffic_tune_away_enum_type  tc_tune_away;
  uint8                                frame_offset;
}
hdrsrch_tc_assignment_params_type;
#endif /* FEATURE_HDR_REVB */


/* HDRSRCH_ASET_UPDATE_CMD */
#ifndef FEATURE_HDR_REVB
typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_aset_pilot_type           aset_pilots[ HDRSRCH_ASET_MAX_SIZE ];
  uint8                             num_pilots;
}
hdrsrch_aset_update_params_type;
#else
typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  sys_active_prot_e_type            hdr_subtype;
  hdrsrch_subactive_set_type        subaset_list[ HDRSRCH_MAX_NUM_SUBASETS ];
  uint8                             num_subasets;
  uint8                             cc_subactive_set_idx;
  uint8                             frame_offset;
#ifdef FEATURE_HDR_REVC
  hdrsrch_aset_update_cb_type       update_complete_cb;
#endif /* FEATURE_HDR_REVC */
} hdrsrch_aset_update_params_type;
#endif /* FEATURE_HDR_REVB */

/* HDRSRCH_PILOT_REPORT_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_pilot_rpt_cb_type         pilot_rpt_cb;
}
hdrsrch_pilot_report_params_type;

/* HDRSRCH_SET_CC_START_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint8                             cc_start;
} hdrsrch_set_cc_start_params_type;


/* HDRSRCH_SET_SCC_OFFSET_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint16                            scc_offset;
} hdrsrch_set_scc_offset_params_type;

/* HDRSRCH_SET_SCC_INFO_CMD */
typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint16                            scc_offset;
  uint8                             scc_rate;
  uint8                             link_id;
} hdrsrch_set_scc_info_params_type;

/* HDRSRCH_AT_SET_DRC_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_demod_idx_type            demod_idx;
  uint8                             drc_param;
} hdrsrch_at_set_drc_params_type;

#ifdef FEATURE_HDR_BCMCS
/* HDRSRCH_UPDATE_BC_INFO_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_bcmcs_enum_type           bcmcs_status;
    /* BCMCS status: ON/OFF   */
}
hdrsrch_bc_info_params_type;

/* HDRSRCH_UPDATE_BOM_PERIOD_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint8                             period;
    /* broadcast overhead message period  */
}
hdrsrch_bc_bom_period_params_type;
#endif /* FEATURE_HDR_BCMCS */

/* HDRSRCH_TC_MODE_CMD */

typedef struct
{
  msgr_hdr_struct_type                  msg_hdr;      /* Message header */
  hdrsrch_tc_mode_enum_type             mode;
    /* Specific HDR mode to be enabled/disabled. */
}
hdrsrch_tc_mode_params_type;

#ifdef FEATURE_HDR_HYBRID_ACCESS

/* HDRSRCH_ACMAC_READY_CMD */

typedef struct 
{
  msgr_hdr_struct_type          msg_hdr;      /* Message header */
  hdrsrch_acmac_ta_time_cb_type ta_time_cb;
    /* Callback function to notify ACMAC tune away time */
  
  hdrsrch_acmac_stop_cb_type    stop_cb;
    /* Callback function to stop ACMAC */

}
hdrsrch_acmac_ready_params_type;

/* HDRSRCH_ACMAC_TA_TIME_OK_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  boolean                           ta_time_ok;
    /* Whether tune away time is ok */
}
hdrsrch_acmac_ta_time_ok_params_type;

/* HDRSRCH_ACMAC_DISABLE_TA_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint16                            no_ta_duration;
    /* The duration that tune away should be disabled */
}
hdrsrch_acmac_disable_ta_params_type;

#endif /* FEATURE_HDR_HYBRID_ACCESS */

/* HDRSRCH_CHANGE_SLEEP_TIMELINE_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  boolean                           optimized;
    /* Whether to use optimized sleep timeline */
}
hdrsrch_change_sleep_timeline_params_type;

#ifdef FEATURE_HDR_DIME_MODEM

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint64                            conflict_wakeup_tick;
    /* conflict(usually 1x) wakeup point in unit of XO tick */
}
hdrsrch_wakeup_adjust_params_type;

#endif /* FEATURE_HDR_DIME_MODEM */

#ifdef FEATURE_HDR_WAKEUP_MANAGER
typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  uint32                            num_extension_slots;
    /* num slots to extend sleep by */
}
hdrsrch_extend_sleep_params_type;
#endif  /* FEATURE_HDR_WAKEUP_MANAGER */

/* HDRSRCH_SYSTEM_LOST_CMD */

typedef struct
{
  msgr_hdr_struct_type              msg_hdr;      /* Message header */
  hdrsrch_system_lost_enum_type     reason;
} hdrsrch_system_lost_params_type;

/*--------------------------------------------------------------------------
                    SRCH COMMAND PARAMETERS UNION STRUCTURE
--------------------------------------------------------------------------*/

typedef union
{
  hdrsrch_acquire_params_type           acquire;
  hdrsrch_synchronize_params_type       synchronize;
  hdrsrch_idle_params_type              idle;
  hdrsrch_nset_update_params_type       nset_update;
  hdrsrch_channel_change_params_type    channel_change;
  hdrsrch_sleep_params_type             sleep;
  hdrsrch_access_params_type            access;
  hdrsrch_access_handoff_params_type    access_handoff;
  hdrsrch_tc_assignment_params_type     tc_assign;
  hdrsrch_aset_update_params_type       aset_update;
  hdrsrch_pilot_report_params_type      pilot_report;
#ifdef FEATURE_HDR_BCMCS
  hdrsrch_bc_info_params_type           bc_info;
  hdrsrch_bc_bom_period_params_type     bom;
#endif /* FEATURE_HDR_BCMCS */
  hdrsrch_tc_mode_params_type           tc_mode;
#ifdef FEATURE_HDR_HYBRID_ACCESS
  hdrsrch_acmac_ready_params_type       acmac_ready_params;
  hdrsrch_acmac_ta_time_ok_params_type  ta_time_ok_params;
  hdrsrch_acmac_disable_ta_params_type  acmac_disable_ta_params;
#endif /* FEATURE_HDR_HYBRID_ACCESS */
  hdrsrch_rup_req_params_type           rup_req;
#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
  hdrsrch_pilot_meas_req_params_type    pilot_meas_req;
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */
  hdrsrch_enable_fw_hdr_app_params_type enable_fw_hr_app;
#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
  hdrsrch_fast_acq_params_type          fast_acq_req;
  hdrsrch_tran_sys_time_params_type     tran_sys_time_req;
  hdrsrch_lte_pilot_meas_params_type    lte_pilot_meas_req;
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */
#ifdef FEATURE_HDR_TO_LTE
  hdrsrch_lte_nset_update_params_type   lte_nset_update;
#endif /* FEATURE_HDR_TO_LTE */
  hdrsrch_set_cc_start_params_type      cc_start;
  hdrsrch_set_scc_offset_params_type    scc_offset;
  hdrsrch_set_scc_info_params_type      scc_info;
  hdrsrch_at_set_drc_params_type        at_set_drc;
  hdrsrch_change_sleep_timeline_params_type optimized_sleep_timeline;
#ifdef FEATURE_HDR_REVC
  boolean                               enable_nlb;
  hdrsrch_loadadj_type                  loadadj;
#endif /* FEATURE_HDR_REVC */
#ifdef FEATURE_HDR_DIME_MODEM
  hdrsrch_wakeup_adjust_params_type     wakeup_adjust;
#endif
#ifdef FEATURE_HDR_WAKEUP_MANAGER
  hdrsrch_extend_sleep_params_type      extend_sleep;
#endif /* FEATURE_HDR_WAKEUP_MANAGER */
  hdrsrch_channel_change_and_nset_update_params_type channel_change_and_nset_update;
  hdrsrch_band_grant_params_type                     band_grant;
  hdrsrch_system_lost_params_type       system_lost_reason;
}
hdrsrch_cmd_params_union_type;


/*--------------------------------------------------------------------------
                            SRCH COMMAND STRUCTURE
--------------------------------------------------------------------------*/

/* The status field has been moved to be above the params field to work with
   the MSGR message processor.  MSGR does not allocate memory following the
   params field.  Since MSGR is 64 bit alligned, the padding field has been
   removed as:
     - The name field is 32 bits
     - The status field is 32 bits
   This fits perfectly with the 64 bit allignment in MSGR. */

typedef struct
{
  hdrsrch_cmd_name_enum_type            name;     /* Name of the command */
  errno_enum_type                       status;   /* Status of command */
  hdrsrch_cmd_params_union_type         params;   /* Command parameters */
}
hdrsrch_cmd_type;

typedef struct
{
  q_link_type                          link;

  hdrsrch_cmd_type                     cmd;
}
hdrsrch_cmd_q_item_type;

typedef struct
{
  q_type                    cmd_q;
    /* Queue of currently active SRCH commands */
}
hdrsrchcmd_struct_type;

/* <EJECT> */
/*==========================================================================

                    EXTERN VARIABLES and FUNCTIONS

This section contains variables that are accessible outside this module
==========================================================================*/

extern hdrsrchcmd_struct_type    hdrsrchcmd;

/*==========================================================================

FUNCTION HDRSRCHCMD_INIT

DESCRIPTION
  This function initializes the HDR SEARCH Command Queue Structures.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchcmd_init( void );


/*==========================================================================

FUNCTION HDRSRCHCMD_NEW

DESCRIPTION
  This function requests a free command queue item from the free queue.

DEPENDENCIES
  hdrsrchcmd_init()

PARAMETERS
  cmd_name  - Name of command structure to create

RETURN VALUE
  Pointer to HDR SEARCH command structure, or NULL if free queue is empty

SIDE EFFECTS
  None

==========================================================================*/

hdrsrch_cmd_type* hdrsrchcmd_new
(
  hdrsrch_cmd_name_enum_type cmd_name
    /* Command structure to create */
);


/*==========================================================================

FUNCTION HDRSRCHCMD_POST

DESCRIPTION
  This function posts a command to the Search Task command queue.

DEPENDENCIES
  Command structure must been obtained from a call to hdrsrchcmd_new()

PARAMETERS
  Command to be posted.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/

void hdrsrchcmd_post
(
  hdrsrch_cmd_type  * cmd
    /* Command to be posted on command queue */
);


/*==========================================================================

FUNCTION HDRSRCHCMD_GET

DESCRIPTION
  This function gets a command from the Search Task command queue.

DEPENDENCIES
  hdrsrchcmd_init()

PARAMETERS
  None

RETURN VALUE
  Command structure posted by hdrsrch_cmd_post()

SIDE EFFECTS
  None
==========================================================================*/

hdrsrch_cmd_type* hdrsrchcmd_get( void );


/*==========================================================================

FUNCTION HDRSRCHCMD_FREE

DESCRIPTION
  This function frees a command structure returned by hdrsrchcmd_new() or
  by hdrsrchcmd_get()

DEPENDENCIES
  Command structure must been obtained from a call to hdrsrchcmd_new() or
  hdrsrchcmd_get()

PARAMETERS
  Command structure to be free'd.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/

void hdrsrchcmd_free
(
  hdrsrch_cmd_type  * cmd
    /* Command to be returned to the free queue */
);


/* <EJECT> */

/*==========================================================================

FUNCTION HDRSRCHCMD_PURGE

DESCRIPTION
  This function removes all commands from command queue and puts them in
  free queue.

DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command queue is empty.

==========================================================================*/

void hdrsrchcmd_purge( void );

/* <EJECT> */

#endif /* HDRSRCHCMD_H */
