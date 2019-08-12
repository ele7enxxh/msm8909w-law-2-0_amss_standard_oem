#ifndef SRCHMC_I_H
#define SRCHMC_I_H
/*===========================================================================

             S E A R C H   to   M C   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations to be used by the MC task.

  Copyright (c) 2004 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srchmc_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/16   pk      Added payload to SRCH_TUNE_AWAY_INV_PRIO_F
02/05/16   ab      1x2GTA Feature changes
08/08/14   as      Fix IRAT for Dual Receive
06/09/14   pk      Changes for FR-17879 (Extended EARFCN and band range)
03/03/14   ssh     1xSRLTE feature code.
12/05/13   bph     FR3839 - IRATMan dynamic Tbackoff 
10/09/13   vks     Fix setting up sch in ftm mode
09/10/13   vks     srch combiner cleanup
08/05/13   pk      Mainlined feature FEATURE_MODEM_STATISTICS
05/23/13   nee     FEATURE LTE Deprioritization changes.
07/16/12   srk     Feature Cleanup
12/21/11   cjb     IRAT: Changes to schedule slots for ATIM operation.
11/30/11   adw     Add HHO flag in Aset info type.
10/31/11   jtm     Feature clean up.
08/29/11   cjb     Renamed FEATURE_MODEM_1X_IRAT_1XTOL to FEATURE_1X_TO_LTE.
08/26/11   cjb     Changes for IRAT:1x->LTE. Initial working version.
06/14/11   cjb     Interface to receive LTE N-List for 1x to LTE meas case.
04/28/11   cjb     Changes needed to enable FEATURE_LTE_TO_1X for NikeL.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
12/16/10   vks     Remove FEATURE_IS2000_REL_C_DV and FEATURE_IS2000_REL_D_DV
                   support.
11/18/10   jtm     Moved SRCH_CFNSET_MAX to srchmc_v.h.
11/01/10   jtm     Modem 1x SU API changes.
09/30/10   bb      Change the cdma_sys_time type to uint64
06/18/10   bb      Change the lte_ostmr value type uint32
03/19/10   pk      Modem Statistics Report Feature - Enable Reporting of
                   Pilots info,Rx pwr and Tx pwr to Call Processing
02/04/10   jtm     Removed FEATURE_PLT.
10/20/09   bb      Added support for "LTE to 1X IRAT Core" feature
09/14/09   bb      Added slew_opt field to the srch_slew_cmd_type structure
                   for slew state otimization feature
08/29/09   bb      Added nghbr_srch_mode field to the srch_nset_cmd_type
08/04/09   mca     Added support for 1x Advanced
04/24/09   adw     Lint fixes.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
11/19/08   adw     Merge from main/latest, based on VU_MODEM_1X_SRCH.12.00.24
11/19/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
09/22/08   sst     Increase MAX_SYS_MEAS_LIST_SIZ
10/18/06   bn      Added new structure to support timing transfering from
                   DO to 1x
10/12/06   awj     Added featurized pseudo-idle support for UMTS/1x
06/05/06   rkc     Added srch_agc_to_dbm.
02/02/06   awj     Added SRCH_MAX_SETS
09/21/05   pa      Changed value of NO_SYS_MEAS_RESULTS
09/20/05   pa      Added AGC gated ecio measurement enum.
08/29/05   grl     Added header to SRCH_ACCESS_F message.
08/08/05   kwo     Added pgslot field to idle_cmd_type
08/03/05   ejv     Mainline FEATURE_TRANS_MANAGER.
08/02/05   ejv     Removed unnecessary includes.
07/24/05   fh      Renamed SRCH_FIND_STRONGEST_PILOT_F, SRCH_STRONG_PILOT_F
                   and related report names, data types, and data structures
07/13/05   ejv     Remove srch_access_inform.  Added srch_notify_block_gps.
06/24/05   ejv     Remove srch_tune_rfm.
06/01/05   grl     Temporarily added srch_tune_rfm for calls from mccidl.c
05/31/05   rng     Merged in from SHDR Sandbox.
04/12/05   fc/ejv  Added support for DCA.
03/21/05   ejv     Removed runtime Cset sizes.
02/07/05   ejv     Added srch_get_rx_agc from srch.h.
01/24/05   ejv     Added srch_get_pilot_ecio from srch.h.
12/10/04   ejv     Updates to the RelD interface.
12/06/04   kwo     Added SRCH_ACCESS_F and SRCH_FIND_STRONGEST_PILOT_F
11/12/04   ejv     Replaced magic number.  Remove use_sch flag.
11/10/04   ejv     Added srch_tune_rfm and srch_get_rx_div_cap from srch.h.
11/03/04   kwo     Removed obsolete fade and unslew command support
10/15/04   ejv     Initial version.  Moved MC-specific data from srch.h.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"

/* Cp */
#include "cai.h"
#include "cai_v.h"
#include "cai_i.h"
#include "caii_v.h"
#include "caii_i.h"

/* Srch */
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"
#include "srchmc.h"
#include "srchmc_v.h"

/* Other */
#include "cmd.h"
#include "qw.h"

#include "sys.h"
#include "lte_as.h"

/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
                          SEARCH COMMAND CODES

The following codes are for use with the 'cmd' field of the search command
header type defined below, to define the operation requested by the command.
--------------------------------------------------------------------------*/

typedef enum
{
  SRCH_CDMA_F,      /* Enter CDMA mode */
  SRCH_ACQ_F,       /* Acquire pilot PN */
  SRCH_SC_F,        /* Begin sync channel operation */
  SRCH_SLEW_F,      /* Slew timing to align with system time */
  SRCH_IDLE_F,      /* Begin idle operation */
  SRCH_ACCESS_F,    /* Begin access operation */
  SRCH_SLEEP_F,     /* Put the system to sleep until next slot */
  SRCH_WAKEUP_F,    /* Initiate a wakeup now */
  SRCH_TC_F,        /* Begin traffic channel operation */

  /* The above commands all request a state change. */

  SRCH_CHAN_CONFIG_F, /* Channel configuration update */
  SRCH_BC_INFO_F,
  SRCH_PARM_F,      /* Change search parameters */
  SRCH_MEAS_F,      /* Return Pilot Strength Measurements */
  SRCH_FPC_MEAS_F,  /* Return Forward Power Control Pilot Measurements */
  SRCH_IDLE_ASET_F, /* Active set update for idle */
  SRCH_IDLE_NSET_F, /* Neighbor set update for idle */
  SRCH_PC_ASET_F,   /* Active set update for paging */
  SRCH_PC_NSET_F,   /* Neighbor set update for paging */
  SRCH_SYS_MEAS_F,  /* System measurement request */
  SRCH_LIST_SIZE_F, /* srch_meas_list_size > 0 */
  SRCH_RESELECT_MEAS_F,  /* Start PC-state system reselection monitoring */
  SRCH_MCC_IDLE_F,        /* MCC's Idle state signal */
  SRCH_MCC_ACC_ENTRY_F,   /* MCC's Access Entry state signal */
  SRCH_MCC_ACC_PROBE_F,   /* MCC's Access (Probe) State signal */
  SRCH_IDLE_FIND_STRONG_PILOT_F,  /* Find the best pilot to entery access on */
  SRCH_START_PPSM_F,      /* Start PPSMM timer */
  SRCH_STOP_PPSM_F,       /* Stop PPSMM timer */
  SRCH_PPSM_F,            /* One time PPSMM reporting */
  SRCH_CF_NSET_F,         /* Candidate Frequency Neighbor Infor. */
  SRCH_START_CFS_F,       /* Start Candidate Freq search */
  SRCH_STOP_CFS_F,        /* Stop Candidate Freq search */
  SRCH_CFS_MEAS_F,        /* Perform pilot measurement on CF */
  SRCH_START_RIF_HHO_F,   /* Start IS95B Hard handoff */
  SRCH_RIF_HHO_SUCCESS_F, /* A successful hard handoff */
  SRCH_RIF_HHO_FAILURE_F, /* An unsuccessful hard handoff */
  SRCH_REACQ_ASET_F,      /* Hard handoff failure, restore the SF */
  SRCH_PILOT_LIST_F,      /* Pilot Reporting request */
  SRCH_TC_ASET_F,         /* Active set update for traffic */
  SRCH_TC_UPDATE_SUP_F,   /* Supplemental channel update */
  SRCH_TC_ADD_SCH_F,      /* Keep until 3GDATA prototype is not needed */
  SRCH_TC_NSET_F,   /* Neighbor set update for traffic */
  SRCH_PC_ASSIGN_F, /* Search pilot list given in paging channel assign */
  SRCH_POWERDOWN_F, /* MC wants to power down - perform housekeeping */
  SRCH_OFF_TX_F,    /* TX is off - hold off autonomous PSMMs temporarily */
  SRCH_ON_TX_F,     /* TX is on now - resume reporting autonomous PSMMs */
  SRCH_RAND_BITS_F, /* MC wants a buffer for A-key seed */
  SRCH_HO_F,        /* MC sends handoff information to Search */
  SRCH_SCRM_MEAS_F, /* SCRM pilot measurements request */
  SRCH_TC_FIND_STRONG_PILOTS_F, /* MC wants a list of pilots */
  SRCH_MC_GPS_VISIT_OKAY_F,
  SRCH_MC_RESUME_CDMA_F,

  SRCH_DEACTIVATE_F,
  SRCH_AMPS_F,
  SRCH_TT_F,        /* begin do HDR to 1x timing transfer */
  SRCH_LTE_1X_TT_F, /* begin to do LTE to 1x timing transfer
                       for reselection */
  SRCH_LTE_NSET_UPDATE_PARAMS_F, /* To get the LTE neighbors info from 1xCP */

  SRCH_TUNE_AWAY_RESP_F,    /* Start or Reject TuneAway Request */          
  SRCH_TUNE_AWAY_PROCEED_F, /* Begin TuneAway */
  SRCH_TUNE_AWAY_CNTL_F,    /* Allow or disallow TuneAway Requests  */    
  SRCH_TUNE_AWAY_INV_PRIO_F, /* CP sends this cmd to Invert the chain priority*/
  SRCH_TUNE_AWAY_LOG_F,      /* CP sends this cmd after tuneaway is completed */


  SRCH_MAX_F,     /* Tells number of commands. MUST be last enum item. */
                  /* except if you look at the else block below, it's not. */

  SRCH_LAST_COMMAND_F=SRCH_MAX_F
}
srch_cmd_code_type;


/*--------------------------------------------------------------------------
                        CAI and Other Constants
--------------------------------------------------------------------------*/

#define  SRCH_CFNSET_MAX          SRCH_NSET_MAX

/* Max size of System Measurement lists */
#define MAX_SYS_MEAS_LIST_SIZ        255

/* maximum number of pilot PN offsets received in
 * SRCH_LTE_1X_TT_F command and "SRCH_LTE_SECTOR_LIST_MAX_SIZE"
 * value in the LTE header should match this value.
 */
#define SRCH_LTE_1X_PN_MAX  32


/*--------------------------------------------------------------------------
                DATA TYPES FOR SYS MEAS COMMANDS
--------------------------------------------------------------------------*/

typedef enum
{
  MEAS_MEMBER_AMPS,    /* Member is AMPS */
  MEAS_MEMBER_CDMA,    /* Member is CDMA */

  MEAS_MEMBER_MAX_NUM  /* Max number of members possible */
}
srch_meas_mem_enum_type;

typedef enum
{
  MEAS_ACTION_RXONLY,   /* Measure RX power only */
  MEAS_ACTION_RXECIO,   /* Measure RX and EcIo (if possible) */
  MEAS_ACTION_GATEDECIO,/* Measure Rx power and EcIo (if Rx passes threshold) */
  MEAS_ACTION_MAX_NUM   /* Max number of actions possible */
}
srch_meas_act_enum_type;

typedef struct
{
  /* The first channel in the block of AMPS channels to search */
  uint16 first_chan;

  /* The last channel in the block of AMPS channels to search */
  uint16 last_chan;
}
srch_meas_amps_sys_type;

typedef struct
{
  /* The CDMA channel to search */
  srch_rx_chan_type cdma_freq;

  /* The CDMA band class to search */
  srch_rx_band_type band_class;
}
srch_meas_cdma_sys_type;


/*--------------------------------------------------------------------------
              STRUCTURES FOR CHANNEL SPECIFIC INFO
--------------------------------------------------------------------------*/

typedef struct
{
  /* Walsh channel to be used on the traffic channel with the pilot */
  uint16 walsh_chn;

  /* QOF mask ID (0-3), where 0 gives true walsh function */
  uint8  qof_mask_id;

  /* Indicates if this SCH is active */
  boolean active;
}
/* Data for supplemental channels */
srch_sch_info_type;

typedef struct
{
  /* Channel walsh code */
  uint16             walsh_chn;

  /* Channel date rate */
  cai_bcch_rate_type rate;

  /* Channel code rate */
  cai_code_rate_type c_rate;

}
/* Data for broadcast control channel */
srch_bcch_info_type;

typedef struct
{
  /*  */
  uint8                     fccch_id;

  /* Channel walsh code */
  uint16                    walsh_chn;

  /* Channel date rate */
  cai_fccch_data_rate_type  rate;

  /* Channel frame size */
  cai_fccch_frame_size_type size;

  /* Channel code rate */
  cai_code_rate_type        c_rate;

}
/* Data for common control channel */
srch_fccch_info_type;

typedef struct
{
  /* Channel walsh code */
  uint16             walsh_chn;

  /* Channel date rate */
  cai_bcch_rate_type rate;

  /* Channel code rate */
  cai_code_rate_type c_rate;

  /* Units are 40msec frames */
  qword              time;

}
/* Data for 2ndary broadcast control channels */
srch_bcch_2ndary_info_type;

typedef struct
{
  /* Channel walsh code */
  uint16            walsh_chn;

  /* Channel date rate */
  cai_pch_rate_type rate;

}
/* Data for paging channel */
srch_pch_info_type;

typedef struct
{
  cai_rx_channel_id_type chan_id;

  union
  {
    /* Add the common channel structures to the union */
    srch_bcch_info_type  bcch;
    srch_fccch_info_type fccch;
    srch_pch_info_type   pch;
  }
  config;
}
/* Contains the currently active IDLE channel */
srch_idle_chan_info_type;

#define SRCH_MAX_BCCH_SUPPORTED CAI_MAX_NON_PRI_BCCH_MONITOR
#define SRCH_NO_BCCH            ((byte)~0)

typedef struct
{
  /* Number of broadcast data slot included */
  uint8                      num_bcch;

  /* BCCH info */
  srch_bcch_2ndary_info_type bcast_slot[ CAI_MAX_NON_PRI_BCCH_CHAN ];
}
/* Structure to hold the secondary BCCH configuration information
   and the next broadcast data slot to wakeup. This is an array
   of all the secondary BCCH channels, indexed by the BCN-2: the
   first item of the array is BCN 2, the second is BCN 3, ... */
srch_bcch_bcast_slot_type;

typedef struct
{
  /* Indicates if the current wakeup is for a EBCSMS slot */
  boolean                    valid_now;

  /* Indicates if the last wakeup was for a EBCSMS slot */
  boolean                    last_slot;

  /* Number of broadcast data slot included */
  uint8                      num_bcch;

  /* Points to the next earlist BCCH data slots in the bcast_slot array,
     up to the number we can demod simultaneously.  Also indicates the
     BCN - 2 */
  uint8                      bcn_inx[ SRCH_MAX_BCCH_SUPPORTED ];

  /* BCSMS channel data is kept here for all the 2ndary BCCH channels */
  srch_bcch_2ndary_info_type bcast_slot[ CAI_MAX_NON_PRI_BCCH_CHAN ];
}
/* Data for BCCH SMS */
srch_bcch_bcast_info_type;

typedef struct
{
  /* Indicates if F-PDCH is supported */
  boolean included;

  /* Minimum interruption seen by the MS, in 10ms units,
     when switching between pilots different groups */
  uint16   soft_switching_delay;

  /* Minimum interruption seen by the MS, in 10ms units,
     when switching between pilots in the same group */
  uint16   softer_switching_delay;
}
/* Config data for packet data channel */
srch_pdch_cfg_type;

typedef struct
{
  /* Indicates if R-RCCH is supported */
  boolean included;

  /* F-RCCH subchannel repetition factor
     Currently not used by SRCH (programmed by RXC) */
  uint8   repetition;
}
/* Config data for rate control channel */
srch_rcch_cfg_type;

typedef struct
{
  /* Configuration data for F-PDCH */
  srch_pdch_cfg_type pdch;

  /* Configuration data for F-RCCH */
  srch_rcch_cfg_type rcch;
}
/* Supoer-struct of all DV channel config info */
srch_dv_cfg_type;


/*--------------------------------------------------------------------------
              TRAFFIC CHANNEL ACTIVE SET UPDATE COMMAND

This command replaces the Active Set on the traffic channel.
--------------------------------------------------------------------------*/

/* Type definition for each member of the new Active Set. */
/* Include as part of the command */
typedef struct
{
  /* Pilot PN index of this Active Set member */
  int16                   pilot;

  /* Type indicator for additional pilot record types */
  cai_pilot_rec_type      pilot_rec_type;

  /* Union of possible additional pilot record types */
  caii_add_pilot_rec_type pilot_rec;

  /* Parameters for the FCH */
  srch_fch_info_type      fch_info;

  /* Parameters for the DCCH */
  srch_dcch_info_type     dcch_info;

  /* Parameters for the SCCH of this pilot */
  srch_scch_info_type     scch_info;

  /* Neighbor Window Search Offset for the pilot */
  uint8                   srch_offset_nghbr;

  /* Parameters for the SCH of this pilot.
     Maximum of 2 SCH in IS-2000. TIGER2 supports only 1 */
  srch_sch_info_type      sch_info[ SRCH_SCH_MAX ];

  /* Parameters for CPCCH */
  srch_cpcch_info_type    cpcch_info;

  /* Information related to PDCH that may change per pilot */
  srch_dv_pilot_info_type dv_pilot_info;

  /* Flag: nonzero if the power control subchannel on this pilot
     contains the same symbols as that on the previous pilot. */
  uint8                   pwr_comb_ind;
}
srch_tc_aset_type;

/* For IS2000 changes, tc_aset_cmd_type is obsolete, instead the active
   set is retrieved by means of this new srch_tc_aset_info_type data struct */

/* The following interface is defined for SRCH to make a
   function call into MC to get the active set in case of a
   SHO and SCH assignment at the action time */

typedef struct
{
  /* CDMA Channel number */
  srch_rx_chan_type cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type band_class;

  /* Number of Active Set members */
  uint16            aset_cnt;

  /* Flag indicating Active Set is being updated for a hard-handoff */
  boolean           is_hho;

  /* Pilot offset indices and walsh channels for active set members */
  srch_tc_aset_type aset[ SRCH_ASET_MAX ];
}
srch_tc_aset_info_type;


/*--------------------------------------------------------------------------
                          SEARCH COMMAND HEADER TYPE

This header type is contained in every search command. It contains info
common to all commands.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Includes the queue links,  pointer to requesting task TCB,
     signal to set on completion,  and done queue */
  cmd_hdr_type         cmd_hdr;

  /* Specifies which command to perform. See enumerated command codes above. */
  srch_cmd_code_type   cmd;

  /* Specifies the status of the command. Initially, this field is set
     to SRCH_BUSY_S by function srch_cmd. When command processing is
     finished, this field is set to one of the statuses enumerated above. */
  srch_cmd_status_type status;
}
srch_cmd_hdr_type;

/*--------------------------------------------------------------------------
                      ENTER CDMA MODE COMMAND

This command is sent to make the transition from INACTIVE state to CDMA state.
This must be done before beginning to acquire the CDMA signal.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_cdma_cmd_type;


/*--------------------------------------------------------------------------
                      ACQUIRE PILOT PN COMMAND

This command is sent to move from CDMA, SC, or PC states to the ACQ state,
and begin searching for a pilot signal.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* CDMA Channel number */
  srch_rx_chan_type cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type band_class;

  /* The acquisition mode, Full, Mini, Micro */
  db_acq_mode_type acq_mode;

}
srch_acq_cmd_type;

/*--------------------------------------------------------------------------
                   BEGIN SYNC CHANNEL OPERATION COMMAND

This command is sent to move from the ACQ, or PC states
to the SC state, and begin receiving the sync channel.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_sc_cmd_type;

/*--------------------------------------------------------------------------
                      SLEW TO SYSTEM TIMING COMMAND

This command is given to move from the SC state to the SLEW state, and
to begin moving demod chip timing from sync channel timing to system
timing (the jump to hyperspace).  The parameters contain some of the
information received from the sync channel message - just enough to let
us decide how many 26ms periods to slide before reporting (with a
SRCH_SLEW_R report) that we are aligned for the jump to hyperspace on
the next 26ms boundary.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type        hdr;

  /* Which pilot offset index the acquired cell is using */
  int16                    pilot_index;

  /* Which 80 millisecond phase (0,1 or 2) the start of the
     sync channel message was received in */
  uint16                   phase_80ms;

  /* variable to indicate the number of sync frames combined
     due to slew state optimization feature */
  uint8                    slew_opt;

  srch_rx_chan_type        cdma_freq;
   /* CDMA Channel number */

  srch_rx_band_type        band_class;
    /* CDMA Band Class */
}
srch_slew_cmd_type;


/*--------------------------------------------------------------------------
                   BEGIN PAGING CHANNEL OPERATION COMMAND

This message is sent immediately after the SRCH_SLEW_R report is received,
to move from the SLEW state to the PC state, make the jump to hyperspace,
and begin receiving the paging channel.  This command MUST arrive between
the 26ms boundary that triggered the SRCH_SLEW_R and the next 26ms boundary.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* System time value extracted from sync message */
  /* This time is applicable on the xth 80ms boundary after the
     start of the sync message, so that's when we will jump to
     hyperspace. */
  qword             sys_time;

  /* PN long code state to be used at hyperspace boundary */
  qword             pn_state;

  /* PN long code mask to be used on paging channel */
  qword             pn_mask;

  /* Walsh channel number to tune to for the paging channel */
  uint16            walsh_chn;

  /* Data rate for paging channel */
  uint16            rate;

  /* CDMA Channel number for paging channel */
  srch_rx_chan_type cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type band_class;
}
srch_pc_cmd_type;

/*--------------------------------------------------------------------------
                   BEGIN IDLE STATE OPERATION COMMAND

This message is sent immediately after the SRCH_SLEW_R report is received,
to move from the SLEW state to the Idle state, make the jump to hyperspace,
and begin receiving the common channel.  This command MUST arrive between
the 26ms boundary that triggered the SRCH_SLEW_R and the next 26ms boundary.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type        hdr;

  /* System time value extracted from sync message */
  /* This time is applicable on the xth 80ms boundary after the
     start of the sync message, so that's when we will jump to
     hyperspace. */
  qword                    sys_time;

  /* PN long code state to be used at hyperspace boundary */
  qword                    pn_state;

  /* PN long code mask to be used on paging channel */
  qword                    pn_mask;

  /* CDMA Channel number for paging channel */
  srch_rx_chan_type        cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type        band_class;

  /* Which slot we're supposed to listen in.  This value has been
     computed from our ESN. */
  /* 0 to 2047 as per IS_95, 6.6.2.1.1.3.1 */
  uint16                   pgslot;

  /* Active pilot PN */
  uint16                   pilot_pn;

  /* Active pilot type indicator */
  cai_pilot_rec_type       pilot_rec_type;

  /* Additional active pilot parameters */
  caii_add_pilot_rec_type  pilot_rec;

  /* Active channel configuration */
  srch_idle_chan_info_type chan_info;
}
srch_idle_cmd_type;

/*--------------------------------------------------------------------------
                   BEGIN ACCESS OPERATION COMMAND

This message is sent to inform search to switch the access state.  Search
should immediately start trying to aquire the resources required to access
and report its status to MC.
--------------------------------------------------------------------------*/
typedef enum
{
  SRCH_HIGH_PERF_ACCESS,  /* High Performace (typically activate rdiv) */
  SRCH_LOW_POWER_ACCESS   /* Low Power (typically no rdiv) */
} srch_access_perf_enum;

typedef enum
{
  SRCH_ACCESS_MOBILE_ORIG_TYPE,
  SRCH_ACCESS_MOBILE_ORIG_IMMED_TYPE,
  SRCH_ACCESS_MOBILE_TERM_TYPE,
  SRCH_ACCESS_MOBILE_TERM_IMMED_TYPE
} srch_access_type_enum;

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type        hdr;

  /* Access performance information (Diversity setting for example) */
  srch_access_perf_enum access_perf;

  /* Access type information (Orig, Orig Immediate, Mobile Terminated) */
  srch_access_type_enum access_type;
} srch_access_cmd_type;


/*--------------------------------------------------------------------------
             COMMON CHANNEL CONFIGURATION UPDATE COMMAND
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type        hdr;

  /* Number of channel configurations specified in the priority list */
  uint8                    num_chan;

  /* Priority list of channel configurations */
  srch_idle_chan_info_type chan_info[CAI_MAX_NUM_CHAN_MONITOR];
}
srch_chan_config_cmd_type;

/*--------------------------------------------------------------------------
                           SLEEP STATE COMMAND

This message is sent while on the Paging Channel to allow the searcher
to put the MSM and itself to sleep until the next Paging Channel slot.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Optional deep sleep interval in seconds */
  /* NOT USED! Obsolete since inactive rewrite */
  uint16            duration;

  /* Indicates what type of slot has just finished */
  /* NOT USED! Obsolete since ZZ2 rewrite */
  uint8             cur_slot_mask;

  boolean           monitor_next_slot;

  /* This variable would make sure SRCH goes to sleep even if the next slot 
   * is a wakeup slot for BCSMS/GPS/etc. This is primarily needed where other
   * Technology can access the TX/RX chains by sending SRCH to Sleep. */
  boolean           forced_sleep;
}
srch_sleep_cmd_type;

/*--------------------------------------------------------------------------
                           BROADCAST INFO COMMAND

--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* TRUE if following data corresponds to BCCH, FALSE if for PCH */
  boolean           monitor_bcch;

  union
  {
    /* The next paging slot to wake up to receive broadcast
       messages. If no more messages, 0 is assigned */
    qword                     time;

    /* The next BCCH data slots to wake up. */
    srch_bcch_bcast_slot_type bcch;
  }
  next_bcast_slot;
}
srch_bc_info_cmd_type;


/*--------------------------------------------------------------------------
                   TRAFFIC CHANNEL RADIO CONFIG TYPE

This structure holds the radio configuration for a channel.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Indicates if channel is included */
  boolean               included;

  /* RC of channel */
  cai_radio_config_type rc;
}
srch_tc_rc_type;


/*--------------------------------------------------------------------------
                   BEGIN TRAFFIC CHANNEL OPERATION COMMAND

This command is sent to move from PC state to TC state, and switch from
receiving the paging channel to receiving the assigned traffic channel.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type     hdr;

  /* PN long code mask to be used on the traffic channel */
  qword                 pn_mask;

  /* Number of Active Set members */
  uint16                aset_cnt;

  /* For each member of the new Active Set: */
  struct
  {
    /* Pilot PN index of this Active Set member */
    int16                   pilot;

    /* Type indicator for additional pilot record types */
    cai_pilot_rec_type      pilot_rec_type;

    /* Union of possible additional pilot record types */
    caii_add_pilot_rec_type pilot_rec;

    /* Parameters for the FCH */
    srch_fch_info_type      fch_info;

    /* Parameters for the DCCH */
    srch_dcch_info_type     dcch_info;

    /* Parameters for CPCCH */
    srch_cpcch_info_type    cpcch_info;

    /* information related to PDCH that may change per pilot */
    srch_dv_pilot_info_type dv_pilot_info;

    /* Flag: nonzero if the power control subchannel on this pilot
       contains the same symbols as that on the previous pilot. */
    uint8                   pwr_comb_ind;
  }
  /* Pilot offset indices and walsh channels for active set members */
  aset[ SRCH_ASET_MAX ];

  /* CDMA Channel number for the traffic channel */
  srch_rx_chan_type     cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type     band_class;

  /* Which channel has power control */
  cai_fpc_pri_chan_type fpc_pri_chan;

  /* The RC for the fundamantel channel */
  srch_tc_rc_type       fch_rc;

  /* The RC for the dedicated control channel */
  srch_tc_rc_type       dcch_rc;

  /* Information related to PDCH that is common
     for all pilots in the active set */
  srch_dv_cfg_type      dv_cfg;

  /* Radio configuration parameter records */
  cai_rcp_srch_type     rc_params_recs;
}
srch_tc_cmd_type;

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Number of SCH */
  uint16            num_sch;

  /* For each member of the new SCH */
  struct
  {
    /* Pilot PN index of this Active Set member */
    int16  pilot;

    /* Walsh channel to be used on the traffic channel with the pilot */
    uint16 walsh_chn;

    /* QOF mask ID (0-3), where 0 gives true walsh fns) */
    uint8  qof_mask_id;

    /* SCH RC */
    cai_radio_config_type rc;

    /* SCH rate: 1X, 2X, 4X, 8X, 16X */
    cai_sch_rate_type     sch_rate;

    /* SCH coding type: 0 - convolutional, 1 - turbo*/
    cai_sch_coding_type   code_type;
  }
  /* Pilot offset indices and walsh channels for active set members */
  /* Maximum of 2 SCH in IS-2000. TIGER2 supports only 1 */
  srch_tc_sch_type[ SRCH_SCH_MAX ];

}
srch_tc_add_sch_cmd_type;

/*--------------------------------------------------------------------------
      SUPPLEMENTAL CHANNEL REQUEST PILOT STRENGTH MEASUREMENTS COMMAND

This message requests that the Searcher generate a Supplemental Channel
Request Pilot Strength Measurement Message.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* SCRM pilot strength reporting offset */
  uint8             t_mulchan;
}
srch_scrm_meas_cmd_type;



/*--------------------------------------------------------------------------
                   CHANGE PILOT SEARCH PARAMETERS COMMAND

This command provides the Searcher Task with search parameters from
base station messages.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Active set search window size index    */
  uint8             win_a;

  /* Neighbor set search window size index  */
  uint8             win_n;

  /* Remaining set search window size index */
  uint8             win_r;

  /* Threshold of energy at which a pilot moves up to the Candidate Set */
  uint8             t_add;

  /* Threshold of energy at which a pilot should be dropped from the
     Active Set or Candidate Set */
  uint8             t_drop;

  /* Margin by which a pilot must exceed an Active Set pilot to trigger a
     new Power Measurement Report */
  uint8             t_comp;

  /* Index of the duration for which a pilot must be below t_drop before
     being dropped or reported */
  uint8             t_tdrop;

  /* Maximum "age" for a pilot to remain in the Neighbor Set */
  uint8             nghbr_max_age;

  /* Threshold slope value */
  uint8             soft_slope;

  /* T_ADD threshold intercept value, in 0.5dB units */
  int8              add_intercept;

  /* T_DROP threshold intercept value, in 0.5dB units */
  int8              drop_intercept;

  /* Flag tells if the re-selection thresholds included */
  boolean           sys_reselect_included;

  /* Pilot power threshold */
  uint8             ec_thresh;

  /* Pilot Ec/Io threshold */
  uint8             ec_io_thresh;

  /* The slot cycle index in use by the base station. This indicates
     how long the mobile can sleep between slots */
  int8              slot_cycle_index;

  /* Which slot we're supposed to listen in.  This value has been
     computed from our ESN. */
  /* 0 to 2047 as per IS_95, 6.6.2.1.1.3.1 */
  uint16            pgslot;

  /* Indicates the cycle that the mobile wakes up on to receive broadcast
     messages or pages */
  /* 0 to 7 as per CAI */
  uint8             bcast_index;

  /* MC commands searcher to go to pseudo-idle operation */
  cai_idle_mode_type idle_mode;

  #ifdef FEATURE_1X_TO_LTE
  /* MC commands searcher to enable the ATIM operations */
  boolean           atim_enabled;

  /* Indicates the cycle that mobile wakes up on to receive the ATIM message.
     This field needs to be ignored if atim_enabled is set as FALSE. */
  /* Range: 0 to 5 as per spec. Validation done at CP. */
  byte              atim_cycle_index ;
  #endif /* FEATURE_1X_TO_LTE */

  /* System identification */
  word              sid;

  /* Network identification */
  word              nid;

  /* Base station identification */
  word              base_id;
}
srch_parm_cmd_type;

/*--------------------------------------------------------------------------
                   BEGIN TIME TRANSFER OPERATION COMMAND

This message is sent to inform search to switch to time transfer state during
the DO to 1x handoff. Search should immediately start trying to get the timing
from DO and start set the forward link timing, then and report its status to MC.

--------------------------------------------------------------------------*/
typedef struct
{
  /* Common header */
  srch_cmd_hdr_type        hdr;
}
srch_tt_cmd_type;


/*--------------------------------------------------------------------------
                   BEGIN LTE to 1X TIME TRANSFER OPERATION COMMAND

This message is sent to inform search to perform time transfer procedure
using CDMA system time received in SIB8 message and corresponding LTE RTC value.
This command will be sent by MC for reselection scenarios.

--------------------------------------------------------------------------*/
typedef struct
{
  uint32    lte_ostmr;        /* LTE RTC counter value */
  uint64    cdma_sys_time;    /* CDMA System time */

} srch_lte_tt_info_type;

typedef struct
{
   /* common header */
   srch_cmd_hdr_type      hdr;

   srch_parm_cmd_type     parm;       /* parameter info */

   srch_lte_tt_info_type  tt_info;    /* Time Transfer information */

   /* Spacing of potential pilot offset in use in this system.  The
      remaining set consists of all pilot offsets which are multiples
      of this value (but are not in another set). */
   uint8                  pilot_inc;

   sys_channel_type       band_chan_info; /* band and channel info */

   uint16                 aset_cnt;  /* indicates number of active pilots */
   int16                  active_pilot_list[SRCH_ASET_MAX];

   uint16                 nghbr_cnt;  /* indicates number of neighbor pilots */
   int16                  nghbr_pilot_list[SRCH_NSET_MAX];
} srch_lte_1x_tt_cmd_type;


/*--------------------------------------------------------------------------
        1X_CP --> 1X_SEARCHER : LTE NEIGHBOR_SET LIST UPDATE COMMAND

This message is sent by MC for 1x->LTE measurement scenarios to update searcher
with the NSET list of LTE frequencies. This list will be used by 1x-searcher to
perform LTE measurements just before entering sleep.

--------------------------------------------------------------------------*/
#define MAX_NUM_RADIO_INTERFACE 15
#define MAX_NUM_EUTRA_FREQ 7
#define MAX_NUM_PLMNIDS 8

typedef struct
{
  lte_earfcn_t     earfcn;                           /* EARFCN */
  int8    earfcn_pri;                       /* EARFCNPriority */
  byte    thresh_x;                         /* ThreshX */
  byte    rx_lev_min_eutra;                 /* RxLevMinEUTRA */
  byte    pe_max;                           /* PEMax */
  boolean rx_lev_min_eutra_offset_inc;      /* RxLevMinEUTRAOffset Incl */
  byte    rx_lev_min_eutra_offset;          /* RxLevMinEUTRAOffset */
  byte    meas_band_width;                  /* MeasurementBandwidth */
  byte    plmn_same_as_prev_chn;            /* PLMNSameAsPreviousChannel */
  byte    num_plmnids;                      /* NumPLMNIDs */

  struct
  {
    dword plmn;                             /* plmn id */
  }plmnid[ MAX_NUM_PLMNIDS ];

} srch_lte_freq_params_type;

typedef struct
{
  boolean pri_inc;                          /* Priority included */
  byte    pri;                              /* Serving Priority */
  byte    thresh_srv;                       /* Thresh Serving */
  boolean perearfcn_params_inc;             /* PerEARFCNParamsIncluded */
  byte    rx_lev_min_eutra_common;          /* RxLevMinEUTRACommon */
  byte    pe_max_common;                    /* PEMaxCommon */
  boolean rx_lev_min_eutra_off_common_inc;  /* RxLevMinEUTRAOffsetCommonIncl */
  byte    rx_lev_min_eutra_off_common;      /* RxLevMinEUTRAOffsetCommon */
  boolean max_resel_timer_inc;              /* MaxReselectionTimerIncluded */
  byte    max_resel_timer;                  /* MaxReselectionTimer */
  boolean srch_backoff_timer_inc;           /* SearchBackOffTimerIncluded */
  byte    min_meas_back_off;                /* MinMeasurementBackoff */
  byte    max_meas_back_off;                /* MaxMeasurementBackoff */
  boolean plmnid_inc;                       /* PLMNIDIncluded */
  byte    num_eutra_freq;                   /* NumEUTRAFrequencies */

  /* LTE frequency parameters */
  srch_lte_freq_params_type eutra_freq[ MAX_NUM_EUTRA_FREQ ];

} srch_lte_radio_interface_type;

typedef struct
{
  srch_cmd_hdr_type     hdr;                /* Common header */

  byte                  num_radio_interface;/* Number of radio interface */

  struct
  {
    /* Radio interface type */
    byte radio_interface_type;

    /* Radio interface type specific fields */
    srch_lte_radio_interface_type radio_interface;

  } radio_interface_rec[ MAX_NUM_RADIO_INTERFACE ];

} srch_lte_nset_update_params_type;


/*--------------------------------------------------------------------------
                 RETURN PILOT STRENGTH MEASUREMENTS COMMAND

This message requests that the Searcher generate a Pilot Strength
Measurement Message,  as if autonomously.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_meas_cmd_type;

/*--------------------------------------------------------------------------
       RETURN FORWARD POWER CONTROL PILOT STRENGTH MEASUREMENTS COMMAND

This command is sent to request the information required to send a
Power Measurement Report Message from the Searcher Task.

  Note: this command can fail, returning SRCH_ERR_S status, if the last
      active set update contained duplicates.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* When the command is processed, this field is updated
     to reflect the actual number of measurements returned. */
  uint16            meas_cnt;

  struct
  {
    uint8 ecio;
  }
  /* When the command is processed, the current filtered pilot strengths
     of the active set sectors are placed here, in the order of the last
     active set update. */
  meas[ SRCH_ASET_MAX ];
}
srch_fpc_meas_cmd_type;

/*--------------------------------------------------------------------------
               SYSTEM SELECTION MEASUREMENT COMMAND

This command tells the searcher to search the list of CDMA and AMPS systems
and return Rx Power and peak EcIo (if CDMA) for each item in the list.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type       hdr;

  /* Measurement ID of this request */
  uint16                  meas_id;

  /* The action desired to take on all
     elements in the list below */
  srch_meas_act_enum_type action;

  /* Number of systems in the list below */
  uint16                  list_cnt;

  struct
  {
    /* What type of member is this? */
    srch_meas_mem_enum_type member_type;

    union
    {
      /* AMPS system particulars */
      srch_meas_amps_sys_type amps_member;

      /* CDMA system particulars */
      srch_meas_cdma_sys_type cdma_member;
    }
    member;
  }
  /* List of members to perform system measurements on */
  meas_list[ MAX_SYS_MEAS_LIST_SIZ ];
}
srch_sys_meas_cmd_type;

/*--------------------------------------------------------------------------
                START SYSTEM RESELECTION MONITORING COMMAND

This command is sent to tell SRCH to begin reselection monitoring on
the Paging channel.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_reselect_cmd_type;


/*--------------------------------------------------------------------------
               PAGING CHANNEL ACTIVE SET UPDATE COMMAND

This command tells the searcher to perform an idle handoff from one
Paging Channel pilot to another.
--------------------------------------------------------------------------*/

typedef struct
{
  /* The name of the specified channel (CAI_PCH, CAI_BCCH, CAI_F_CCCH) */
  cai_rx_channel_id_type chan_id;

  /* The walsh channel to be used with the specified channel */
  uint16                 aset_walsh;

  /* PN long code mask to be used on the specified channel */
  qword                  pn_mask;
}
srch_pc_aset_info_type;

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type       hdr;

  /* The pilot PN index of the new Active Set pilot */
  int16                   aset_pilot;

  /* CDMA Channel number of the new paging channel */
  srch_rx_chan_type       cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type       band_class;

  /* Type indicator for additional pilot record types */
  cai_pilot_rec_type      pilot_rec_type;

  /* Union of possible additional pilot record types */
  caii_add_pilot_rec_type pilot_rec;

  /* Number of channels specified in the priority list */
  uint8                   num_chan;

  /* The walsh channel and PN mask to be used with specified channels */
  srch_pc_aset_info_type  aset_info[CAI_MAX_NUM_CHAN_MONITOR];
}
srch_pc_aset_cmd_type;


/*--------------------------------------------------------------------------
                      NEIGHBOR SET UPDATE COMMAND

This command updates the Neighbor Set.

Note: this command format is used for both paging and traffic channel
variants.  On the traffic channel, this command is sent for each and
every Neighbor List Update message received.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Spacing of potential pilot offset in use in this system.  The
     remaining set consists of all pilot offsets which are multiples
     of this value (but are not in another set). */
  uint8             pilot_inc;

  /* neighbor search mode value of neighbor pilots */
  byte              nghbr_srch_mode;

  /* Number of neighbor set members */
  uint16            nset_cnt;

  struct
  {
    /* The pilot PN index of the neighbor set pilot */
    int16                   pilot;

    /* Type indicator for additional pilot record types */
    cai_pilot_rec_type      pilot_rec_type;

    /* Union of possible additional pilot record types */
    caii_add_pilot_rec_type pilot_rec;

    /* The Neighbor config flag */
    uint8                   config;

    /* The priority of the pilot */
    uint8                   pri;

    /* Flag tells if the frequency information is included */
    boolean                 freq_incl;

    /* CDMA Band Class */
    srch_rx_band_type       band_class;

    /* CDMA Channel number of the pilot */
    srch_rx_chan_type       cdma_freq;

    /* Flag tells whether we have attempted to access this pilot.
       Note: SRCH doesn't use this flag, it's merely for MC coding
             convenience */
    boolean                 access_attempted;

    /* If Access Entry HO allowed */
    boolean                 access_entry_ho;

    /* If ACCESS HO and Access Probe allowed */
    boolean                 access_ho_allowed;

    /* Neighbor window for the pilot */
    uint8                   srch_win_nghbr;

    /* Neighbor Window Search Offset for the pilot */
    uint8                   srch_offset_nghbr;

    /* If Timing included for the Neighbor */
    boolean                 timing_incl;

    /* Neighbor TX offset */
    uint8                   nghbr_tx_offset;

    /* Neighbor TX Duration */
    uint8                   nghbr_tx_duration;

    /* Neighbor TX PERIOD */
    uint8                   nghbr_tx_period;

    /* Indicates if BCCH is supported on this pilot */
    boolean                 bcch_supported;
  }
  /* Pilot offset indices for neighbor set members */
  nset[ SRCH_NSET_MAX ];
}
srch_nset_cmd_type;


/* This command is used for communicating active set to SRCH during a HHO */
typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* CDMA Channel number */
  srch_rx_chan_type cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type band_class;

  /* Number of Active Set members */
  uint16            aset_cnt;

  /* Pilot offset indices and walsh channels for active set members */
  srch_tc_aset_type aset[ SRCH_ASET_MAX ];
}
srch_tc_aset_cmd_type;

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type     hdr;

  /* Flag indicating whether to restore long code mask after RIF-HHO */
  boolean               lcm_change_ind;

  /* Long code mask */
  qword                 lcm;

  /* CDMA Channel number */
  srch_rx_chan_type    cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type    band_class;

  /* Active set search window size index */
  uint8                 win_a;

  /* Threshold of energy at which a pilot moves up to the Candidate Set */
  uint8                 t_add;

  /* Threshold of energy at which a pilot should be dropped from the
     Active Set or Candidate Set */
  uint8                 t_drop;

  /* Margin by which a pilot must exceed an Active Set pilot to trigger a
     new Power Measurement Report */
  uint8                 t_comp;

  /* Index of the duration for which a pilot must be below t_drop before
     being dropped or reported */
  uint8                 t_tdrop;

  /* Threshold slope value */
  uint8                 soft_slope;

  /* T_ADD threshold intercept value, in 0.5dB units */
  int8                  add_intercept;

  /* T_DROP threshold intercept value, in 0.5dB units */
  int8                  drop_intercept;

  /* Neighbor set search window size index  */
  uint8                 win_n;

  /* Remaining set search window size index */
  uint8                 win_r;

  /* Number of Active Set members */
  uint16                aset_cnt;

  /* Which channel has power control */
  cai_fpc_pri_chan_type fpc_pri_chan;

  /* Pilot offset indices and walsh channels for active set members */
  srch_tc_aset_type     aset[ SRCH_ASET_MAX ];
}
/* This data struct is used for reacq only */
srch_tc_reacq_cmd_type;


/*--------------------------------------------------------------------------
              TRAFFIC SUPPLEMENTAL CHANNEL UPDATE COMMAND

This command updates supplemental channels on the traffic channel.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Pilot PN index of this Active Set member */
  int16  pilot;

  /* Number of supplemental channels included in command, can vary
  from 0 to 7. */
  uint8  num_sup_channels;

  /* Walsh channels of supplementals with the pilot */
  uint16 sup_walsh_codes[ SRCH_SUP_CHANNEL_MAX ];

  /* QOF ids of supplementals with the pilot */
  uint16 sup_qof_id[ SRCH_SUP_CHANNEL_MAX ];
}
srch_tc_sup_type;

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* For each pilot in supplemental active set */
  uint8             num_sup_pilots;

  /* Details on each pilot in supplemental active set.  See above. */
  srch_tc_sup_type  sup_pilots[ SRCH_ASET_MAX ];
}
srch_tc_update_sup_cmd_type;


/*--------------------------------------------------------------------------
               PAGING CHANNEL ASSIGNMENT COMMAND

This command tells the searcher to find the strongest pilot_pn in the given
list on the given cdma frequency.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* CDMA Channel number of assignment */
  srch_rx_chan_type cdma_freq;

  /* CDMA Band Class */
  srch_rx_band_type band_class;

  /* Number of pilots given in assignment */
  uint16            assign_set_cnt;

  struct
  {
    int16 pilot;
  }
  /* Pilot offset indices for pilots on new cdma frequency */
  assign_set[ SRCH_NSET_MAX ];
}
srch_pc_assign_cmd_type;

/*--------------------------------------------------------------------------
                     TC TX OFF/ON PROCESSING COMMAND
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_tx_off_on_cmd_type;

/*--------------------------------------------------------------------------
                     PC NEIGHBOR SET PILOT REPORTING COMMAND
This command tells the searcher to report the neighbor pilot strength.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_pilot_rpt_cmd_type;

/*--------------------------------------------------------------------------
                     DCA PILOT MEASUREMENTS REPORTING COMMAND
This command tells the searcher to report whether a strong pilot in MC
specified pilot set is found for Traffic state to System Access state
transition to perform DCA recovery.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Number of pilots specifed in pilot set. */
  uint16            num_pilots;

  struct
  {
    /* Pilot offset indices for pilots on new cdma frequency */
    int16                   pilot_pn;

    /* CDMA Channel number of assignment */
    srch_rx_chan_type       cdma_freq;

    /* CDMA Band Class */
    srch_rx_band_type       band_class;

    /* Type indicator for additional pilot record types */
    cai_pilot_rec_type      pilot_rec_type;

    /* Union of possible additional pilot record types */
    /* Currently not used, but may be necessary in future
       for additional functionality (SRCH tuning for DCA
       measurements needs to know possible TD info) */
    caii_add_pilot_rec_type pilot_rec;
  }
  pilots[ SRCH_NSET_MAX + 1 ];
    /* This can include entire Neighbour set plus idle Active */
}
srch_dca_meas_cmd_type;

/*--------------------------------------------------------------------------
                        TC FILL AKEY SEED BUFFER
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_rand_cmd_type;

/*--------------------------------------------------------------------------
                 PERIODIC POWER STRENGTH MEASUREMENT MESSAGE
This command tells the searcher to do Periodic PSMM.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Periodic timer value in ms */
  uint16            per_timer;

  /* Min. Pilot Ec threshold */
  uint8             min_pil_pwr_thr;

  /* Min. Pilot Ec/Io threshold */
  uint8             min_pil_ec_io_thr;
}
srch_ppm_cmd_type;

/*--------------------------------------------------------------------------
                 START CANDIDATE FREQUENCY SEARCH MESSAGE
This command tells the searcher to start the Candidate Frequency search.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* The sequence number for the current search period */
  uint8             cfs_seq;

  /* Search type: 01 single search; 11 periodic search. */
  uint8             search_type;
}
srch_start_cfs_cmd_type;

/*--------------------------------------------------------------------------
                 STOP CANDIDATE FREQUENCY SEARCH MESSAGE
This command tells the searcher to stop the Candidate Frequency search.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* The sequence number for the current search period */
  uint8             cfs_seq;
}
srch_stop_cfs_cmd_type;

/*--------------------------------------------------------------------------
                START CANDIDATE FREQUENCY MEAS MESSAGE
This command tells the searcher to start the Candidate Frequency measurement.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* The sequence number for the current search period */
  uint8             cfs_seq;

  srch_rx_band_type band_class;
    /* Band class of the CF */

  srch_rx_chan_type cdma_freq;
    /* CDMA frequency for the CF */
}
srch_cfs_meas_cmd_type;

/*--------------------------------------------------------------------------
            START HARD HANDOFF MESSAGE
This command tells the searcher to start a hard handoff.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* If a Hard Handoff with a frequency change */
  boolean           freq_chg;
}
srch_start_hho_cmd_type;

/*--------------------------------------------------------------------------
                HARD HANDOFF SUCCESS MESSAGE
This command tells the searcher a successful hard handoff.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_hho_success_cmd_type;

/*--------------------------------------------------------------------------
                HARD HANDOFF FAILURE MESSAGE
This command tells the searcher an unsuccessful hard handoff.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_hho_fail_cmd_type;


/*--------------------------------------------------------------------------
                     MAIN CONTROL GPS VISIT OKAY COMMAND
This command tells searcher if MC is ready for the GPS visit or not.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Indicate if MC is ready for the GPS visit or not.
     TRUE - MC is ready for the GPS visit, FALSE - else */
  boolean           status;
}
srch_mc_gps_visit_okay_cmd_type;

/*--------------------------------------------------------------------------
                     MAIN CONTROL RESUME CDMA COMMAND
This command tells searcher to resume the normal CDMA operation.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_mc_resume_cdma_cmd_type;


/*--------------------------------------------------------------------------
                 SRCH TUNE AWAY             
This command tells the searcher that TUNE AWAY can be done now 
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Flag to indicate if SRCH can proceed with the TuneAway*/
  boolean tuneaway_proceed;

  /* Type of TuneAway to be done */
  srch_ta_rsn_type  type;
}
srch_ta_resp_type;

/*--------------------------------------------------------------------------
                 SRCH_TUNE_AWAY_CNTL_F
This command tells the searcher to Allow TuneAway
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* Band Class */
  boolean  tuneaway_allowed;
}
srch_ta_ctrl_type;

/*--------------------------------------------------------------------------
                 SRCH_TUNE_AWAY_INV_PRIO_F
This command tells the searcher to Invert the TRM priority
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;

  /* TRUE- SRCH need to bump TRM priority to Traffic,
   * FALSE- SRCH need to lower TRM priority to BG_Traffic*/
  boolean inv_prio;
}
srch_ta_inv_prio_type;

/*--------------------------------------------------------------------------
                 SRCH TUNE AWAY PROCEED       
This command tells the searcher to start the TUNE AWAY.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_ta_proceed_type;

/*--------------------------------------------------------------------------
                 SRCH_TUNE_AWAY_LOG_F       
This command tells the searcher to log the tuneaway params.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Common header */
  srch_cmd_hdr_type hdr;
}
srch_ta_log_type;

/*--------------------------------------------------------------------------
                      UNION OF ALL SEARCH COMMANDS

--------------------------------------------------------------------------*/

typedef union
{
  srch_cmd_hdr_type               hdr;
  srch_cdma_cmd_type              cdma;
  srch_acq_cmd_type               acq;
  srch_sc_cmd_type                sc;
  srch_slew_cmd_type              slew;
  srch_idle_cmd_type              idle;
  srch_access_cmd_type            access;
  srch_chan_config_cmd_type       chan_config;
  srch_bc_info_cmd_type           bc;
  srch_pc_cmd_type                pc;
  srch_sleep_cmd_type             sleep;
  srch_tc_cmd_type                tc;
  srch_parm_cmd_type              parm;
  srch_meas_cmd_type              meas;
  srch_sys_meas_cmd_type          sys_meas;
  srch_reselect_cmd_type          sys_reselect;
  srch_fpc_meas_cmd_type          fpc_meas;
  srch_pc_aset_cmd_type           pc_aset;
  srch_nset_cmd_type              nset;
  srch_tc_aset_cmd_type           tc_aset;
  srch_tc_update_sup_cmd_type     tc_sup;
  srch_tc_add_sch_cmd_type        tc_sch;
  srch_scrm_meas_cmd_type         scrm_meas;
  srch_pc_assign_cmd_type         pc_assign;
  srch_tx_off_on_cmd_type         off_on;
  srch_rand_cmd_type              rand;
  srch_pilot_rpt_cmd_type         pilot_rpt;
  srch_ppm_cmd_type               ppsmm;
  srch_start_cfs_cmd_type         start_cfs;
  srch_stop_cfs_cmd_type          stop_cfs;
  srch_cfs_meas_cmd_type          cfs_meas;
  srch_start_hho_cmd_type         hho_st;
  srch_hho_success_cmd_type       hho_ok;
  srch_hho_fail_cmd_type          hho_fail;
  srch_tc_reacq_cmd_type          hho_reacq;
  srch_dca_meas_cmd_type          dca_meas;
  srch_mc_gps_visit_okay_cmd_type gps_okay;
  srch_mc_resume_cdma_cmd_type    resume_cdma;
  srch_tt_cmd_type                time_transfer;
  srch_lte_1x_tt_cmd_type         lte_tt_info;
  srch_lte_nset_update_params_type lte_nset_params;
  srch_ta_resp_type               tuneaway_resp;
  srch_ta_ctrl_type               tuneaway_ctrl;
  srch_ta_proceed_type            tuneaway_proceed;
  srch_ta_inv_prio_type           inv_priority;
  srch_ta_log_type                log_type; 
}
srch_cmd_type;


/*--------------------------------------------------------------------------
                          Slot Masks
--------------------------------------------------------------------------*/

/* Indicates an assigned page slot */
#define PAGE_SLOT                                          0x0001

/* Indicates an assigned BCSMS page slot */
#define BC_PAGE_SLOT                                       0x0002

/* Indicates a BCSMS data slot */
#define BC_DATA_SLOT                                       0x0004

/* Indicates an ATIM slot */
#define ATIM_SLOT                                          0x0008

/*--------------------------------------------------------------------------
         Akey rand bits variables
--------------------------------------------------------------------------*/

/* Number of bytes required for akey seed */
#define SRCH_MAX_RAND_BUF_SIZE  320

/* This is the buffer where we will store the data */
extern uint8 srch_rand_buf[ SRCH_MAX_RAND_BUF_SIZE ];


/*--------------------------------------------------------------------------
                        CAI and Other Constants
--------------------------------------------------------------------------*/

/* Maximum number of all sets, including RSET of 1 */
#define SRCH_MAX_SETS             ( SRCH_ASET_MAX + SRCH_CSET_MAX + \
                                    SRCH_NSET_MAX + 1 )

/* Maximum SCH Active Set size */
#define  SRCH_SCH_ASET_MAX        CAI_N6M

/* Maximum PN that can be reported for SCRM */
#define SRCH_MAX_SCRM_RPT_PN      CAI_MAX_SCRM_RPT_PN

/* Max size of the reported pilot set in PC, which is all possible neighbors
   plus ane active pilot. */
#define  SRCH_PILOTS_MAX          (SRCH_NSET_MAX + 1)

#define SRCH_NO_CFS       0x0   /* No CF search    */
#define SRCH_SINGLE_CFS   0x1   /* Single search   */
#define SRCH_RSV_CFS      0x2   /* Reserved (i.e. invalid) SEARCH_TYPE value */
#define SRCH_PERIODIC_CFS 0x3   /* Periodic search */

#define SRCH_CF_NO_PRI_OR_WIN 0x0 /* No search priorities/windows specified */
#define SRCH_CF_PRI_ONLY      0x1 /* Search priorities specified */
#define SRCH_CF_WIN_ONLY      0x2 /* Search windows specified */
#define SRCH_CF_PRI_AND_WIN   0x3 /* Search priorities and windows specified */

/* Value used to indicate no results were found.
 * AGC is in units of -1db and EcIo is in units of -0.5 db. So returned
 * value should always be a positive value. If we return 0xFFFF, SD will get
 * confused since it interprets it as a signed number. SD wants a 255 here.
 */
#define NO_SYS_MEAS_RESULTS   ((uint16) 255)


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION SRCH_CSFB_HANDOVER_DONE

DESCRIPTION    Handler for CP indication of Pass/Fail

DEPENDENCIES   CP to call before CDMA_F.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_csfb_handover_done
(
  boolean status
);

/*===========================================================================

FUNCTION SRCH_UTIL_GET_DMOD_ID

DESCRIPTION    This function returns the version number ID of the
               demodulator ASIC or demod section of the MSM.

DEPENDENCIES   None.

RETURN VALUE   The hardware revision number.
               Bits [31:28] - VERSION_ID
               Bits [27:12] - PARTNUM
               Bits [11:01] - QUALCOMM_MFG_ID
               Bits [  :00] - START_BIT

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_util_get_dmod_id ( void );

/*===========================================================================

FUNCTION SRCH_SCOMB_SET_LC_MASK

DESCRIPTION    This function sets the demod long code mask passed in to all 
               the channels. To set the mask to a specific channel use
               srch_scomb_set_chan_lc_mask()

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void  srch_scomb_set_lc_mask
(
  qword pn_mask       /* PN long code mask to load into the mask register */
);

/*===========================================================================

FUNCTION SRCH_CFS_OFF_TIME_FWD

DESCRIPTION    This function provides the max and total off time of forward
               channel of CFS. It's only used for MC to estimated the off time.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_max_off_time_fwd
               and srch_max_off_time_fwd are updated.

===========================================================================*/
extern void srch_cfs_off_time_fwd
(
  uint16 max_win,    /* The maximum window size index of the neighbors */
  uint16 num_set,    /* Number of the total CF neighbor set */
  uint16 *max_off,   /* Pointer to max off time address*/
  uint16 *total_off  /* Pointer to total off time address */
);

/*===========================================================================

FUNCTION  SRCH_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION    This function is called by mode controller when SRCH is not
     responding to the commands sent to it. The command to which SRCH did
     not respond is passed in.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   error fatal created.

===========================================================================*/
extern void srch_multimode_sanity_err_fatal
(
  srch_cmd_code_type  cmd_type  /* command for which there was no response */
);

/*===========================================================================

FUNCTION SRCH_GET_RX_DIV_CAP

DESCRIPTION    This function returns the Rx diversity capability of the
               terminal to MC protocols. One of the place where MC would
               use this function is to not advertise Transmit Diversity
               capability if the terminal is Rx Diversity capable.

DEPENDENCIES   None.

RETURN VALUE   True: Terminal is CDMA RX DIVERSITY capable
               False: Terminal is not CDMA RX DIVERSITY capable

SIDE EFFECTS   None.

===========================================================================*/
extern boolean  srch_get_rx_div_cap( void );

/*===========================================================================

FUNCTION SRCH_GET_PILOT_ECIO

DESCRIPTION    This function returns a byte representing the current
               active pilot Ec/Io value.

DEPENDENCIES    srch_aset[], srch_aset_cnt.

RETURN VALUE    The 6-bit Ec/Io value.  If the energy is out of range,
                the return value is clipped to 0 or 31.5. This is in
                db unit.

SIDE EFFECTS    None.

===========================================================================*/
extern byte srch_get_pilot_ecio  (void);

/*===========================================================================

FUNCTION       SRCH_NOTIFY_BLOCK_GPS
DESCRIPTION    This function prevents to allows a GPS visit from MC
               perspective by setting or clearing the BLOCK_GPS signa
               in the srch_tcb.
DEPENDENCIES   None.

RETURN VALUE   None.
SIDE EFFECTS   srch_tcb signal mask is updated

===========================================================================*/
extern void srch_notify_block_gps
(
  boolean block  /* notify whether or not to block GPS visit */
);

/*===========================================================================

FUNCTION SRCH_AGC_TO_DBM

DESCRIPTION    This function converts an Rx AGC value to dBm.

DEPENDENCIES   None.

RETURN VALUE   The AGC value in dBm units.

SIDE EFFECTS   None.

===========================================================================*/

extern int2  srch_agc_to_dbm
(
  byte rx_agc
    /* CDMA RX AGC value */
);

/*===========================================================================

FUNCTION SRCH_GET_STATS_INFO
DESCRIPTION    Provides information about Active pilots, Rx and Tx Pwr to
               Call Processing (MC)
DEPENDENCIES   None.

RETURN VALUE   None.
SIDE EFFECTS   None

===========================================================================*/
extern void srch_get_stats_info
(
  sys_cdma_information_s_type * cdma_stats
    /* CDMA Status Info maintained at MC */
);

/*===========================================================================

FUNCTION SRCH_CMD

DESCRIPTION    This function places a specified command onto the Search
      Task's command queue for processing by the Search Task. Notice that
      this function is invoked by tasks other than the Search Task.

DEPENDENCIES   This function assumes that the 'srch_cmd_q' has been properly
      initialized.

RETURN VALUE   None.

SIDE EFFECTS   The Search Task dequeues and processes the specified command.
===========================================================================*/
extern void srch_cmd
(
  /* Pointer to searcher command. Upon queueing, the 'status'
     field of the command block is set to 'SRCH_BUSY_S'. */
  srch_cmd_type *cmd_ptr
);

#endif /*SRCHMC_I_H*/
