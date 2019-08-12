#ifndef SRCHCPCDRX_H
#define SRCHCPCDRX_H

/*===========================================================================
                           S R C H C P C D R X . H

GENERAL DESCRIPTION

  This file contains variable declaration and function prototypes for CPC
  DRX feature.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000- 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchcpcdrx.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
07/27/16    bj      CPC-DRX system delay handling for first wakeup.
02/24/15    ap      Initial check-in for WL1 QSH framework 
01/08/15    vn      Fixing compiler warnings
12/10/14    vn      Externing new function to handle CPC DRX local command processing..
09/04/14    sr      Clean up of logging step 3 in DCH for CPC
07/24/14    amj     Changes to ensure NN params not changed if srch or abort in progress
06/06/14    amj     Add API for expiring the ASET PN search count for CPC.
07/29/14    amj/jd  Correct CPC macros for invoking scheduler 
07/24/14    amj     Correct Conditional logic for CPC scheduler
05/27/14    amj     Do not perform searches if QTA is active or WL1 is preparing for QTA
05/20/14    amj     Clean up some unused code and name macros appropriately.
01/02/14    da      Fixed search measurement related KW errors
11/14/13    amj     Added code to ensure a CPC sleep cycle in between a Step1
                    and subsequent NASTT searches
11/08/13    amj     Add support for keeping track of time elapsed during full searches
10/23/13    sm      Search modifications for RxDPM with cpc-drx
10/23/13    amj     Added CPC DRX Support with Search Scheduler
09/12/13    jd      Searcher redesign
07/23/13    jd      parallel step1 changes
10/01/12    zr      Fixed compiler warning
08/20/12    ag      Added debug function to catch when SRCH is stuck.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/08/12    amj     Added macro to return whether SRCH CPC is in active awake state.
02/22/12    amj     Restructured code to use TLM APIs.
                    Made changes to make sure Searcher doesn't get stuck.
09/03/11    sv      Added element in drx structure to check if step1 NN change needed 
07/07/11    hk      Up-merge from offline/main/latest
11/03/10    sa      Added code to support CM during CPC DRX searches and 
                    rename some global structure according to file name.
10/18/10    dm      Made changes for logging search type in 0x423A log packet.
09/28/10    dm      Made changes to log search abort in 0x423A log packet.
08/20/10    dm      Added protoype for function srchcpcdrx_update_srch_exec_time.
09/18/09    sa      Initial creation
                    
===========================================================================*/


#include "wcdma_variation.h"
#include "stmr.h"
#include "srchcmd.h"
#include "mcalwcdma_srch.h"
#include "tlm.h"
#include "srchmeas.h"

/* Different search substate's during the DRX mode */
typedef enum
{
  /* DRX mode paramaters has been intialized*/
  DRX_START_INITIATED,
  /*DRX mode is active and wakes SRCH wakes up for first time */
  DRX_FIRST_WAKEUP,
  /* DRX mode is active and SRCH in sleep state */
  DRX_ACTIVE_SLEEP,
  /* DRX mode is active and SRCH in awake state */
  DRX_ACTIVE_AWAKE,
  /* DRX mode is not active*/
  DRX_INACTIVE
}srchcpcdrx_substate_enum_type;

typedef enum 
{
  SRCHCPCDRX_NORMAL_HB_MODE,
  SRCHCPCDRX_DRX_MODE
} srchcpcdrx_mode_enum_type;

typedef struct
{
   /* scheduling count for aset srches*/
   int8                           aset_cset_drx_schedule_cnt;
   /* scheduling count for nset searches*/
   int8                           nset_drx_schedule_cnt[MAX_CARR];

   /* number of the drx cyles given by the drx ctrl to scheduled step1*/
   uint8                          step1_delta_cnt[MAX_CARR];
   /*number of the drx cyles given by the drx ctrl to scheduled nastt */
   uint8                          nastt_delta_cnt[MAX_CARR];
   /* number of drx cyles count for all cell search count , this search is 
   only for logging all searched cells info*/
   uint8                          all_cell_srch_drx_cnt;

}srchcpcdrx_srch_cnts_struct_type;

/* Structure to store the search paramters during the DRX mode */ 
typedef struct
{
   /* Index to scheduling table */
   uint8                          drx_indx; 
   uint8                          drx_cycle_len ;
   /* mode in whcih srch is scheduled */
   srchcpcdrx_mode_enum_type         mode;  
   /* srch is in sleep state or active state */
   srchcpcdrx_substate_enum_type     substate;  

   /* Srch counts to schedule searches in the DRX mode */
   srchcpcdrx_srch_cnts_struct_type  srch_cnts;
   /* SRCH ID number given by the DRX MGR to SRCH*/
   uint8                          srch_id;   
   /* DRX aligning timing i,e timing given by the SRCH to DRX to program the wakeup occassion*/
   tlm_cfn_struct_type    drx_align_time;
   
   /*time line from the STRM dynamic count at which DRX will call the SRCH_CB function */
   tlm_cfn_struct_type    cur_time;
   
   /*programmed wakeup time from the STMR dynamic REF count ,just for debugging */
   tlm_cfn_struct_type    programmed_wakeup_time;
   
   /* time period for whcih SRCH was active in the current DRX cycle */
   tlm_cfn_struct_type    time_elapsed;
   
   /* drx mode schduling will be freezed by drx ctrl  during  reception of any cphy_setup_req ,so this flag indicates that
   drx have freeze the srch schduling mode */
   boolean                        drx_mode_freeze;
   
   /* hb mode is enable step1/2 searches during CM gap and till this flag is true 
   we will be in HB mode only */
   boolean                        hb_mode_for_step12;
   
#ifdef FEATURE_WCDMA_DC_HSDPA
   /* flag to indicate which all nset searches ( on anchor,secondry carrier) needs to issue 
     in this drx cycle */
   uint32                         nset_srch_mask;
   /* flag to indicate which all aset searches ( on anchor,secondry carrier) needs to issue 
      in this drx cycle */
   uint32                         aset_srch_mask;
#endif
   /* variable to check if step1 NN change is needed */
   boolean                        step1_nn_change;

   /* variable of step1 NN parameter for start/stop CPC */
   uint8                           step1_nn_value;

}srchcpcdrx_db_struct_type;

typedef enum
{
  /* intialize the drx mode parameter for srch scheduling */
  SRCH_CPC_DRX_CONFIG,
  /*align the search wakeup time with hs-scch pattern */
  SRCH_CPC_DRX_ALIGN_INITIAL_WAKEUP_TIMING,
  
  /* freeze SRCH scheduling based on DRX counts while CPHY_SETUP req received from RRC
  and switch SRCH schedling to noraml HB*/
  SRCH_CPC_DRX_FREEZE_DRX_MODE_SCHEDULING,
  
  /* unfreeze SRCH scheduling i,e switch SRCH schedling to DRX counts while CPHY_SETUP_CNF
  send received from RRC*/
  SRCH_CPC_DRX_UNFREEZE_DRX_MODE_SCHEDULING,
  
  /*cleanup the drx mode parameter in srch  */
  SRCH_CPC_DRX_CLEANUP
}srchcpcdrx_cmd_enum_type;

typedef struct
{
 /* drx cycle length */
  uint8 drx_cycle_len;
}srchcpcdrx_drx_init_struct_type;

typedef struct
{
  /* cfn to program srch next wakeup occasion */ 
  uint8 start_cfn;
  /* tick in a cfn  to program th srch wakeup occasion */ 
  uint8 tick_offset;
}srchcpcdrx_align_wakeup_time_struct_type;

typedef union
{
  srchcpcdrx_drx_init_struct_type srch_init;
  srchcpcdrx_align_wakeup_time_struct_type align_wakeup_time;
}srchcpcdrx_cmd_param_union_type;

typedef struct
{
  srchcpcdrx_cmd_enum_type cmd_type;
  srchcpcdrx_cmd_param_union_type cmd_param;
}srchdrx_drxctrl_cmd_struct_type;


typedef enum
{
  FILT_TAB_INDX_FOR_20_MS_SRCH_INTVL,
  FILT_TAB_INDX_FOR_80_MS_SRCH_INTVL,
  FILT_TAB_INDX_FOR_40_MS_SRCH_INTVL,
  FILT_TAB_INDX_FOR_32_MS_SRCH_INTVL,
  FILT_TAB_INDX_FOR_64_MS_SRCH_INTVL
}srchcpcdrx_filt_tab_indx_enum_type;

typedef struct
{
  srchcpcdrx_filt_tab_indx_enum_type  aset_cset_filt_coef_tab_indx; 
  srchcpcdrx_filt_tab_indx_enum_type  sync_set_filt_coef_tab_indx; 

}srchcpcdrx_filt_tab_indx_struct_type;

#define SRCH_NUM_CX1_IN_SUBFRAME            7680
#define SRCH_NUM_SUBFRAME_IN_FRAME            5



#define MAX_NUM_DRX_CYCLES          6
#define FOUR_SUBFRAME_DRX_CYCLE     4
#define FIVE_SUBFRAME_DRX_CYCLE     5
#define EIGHT_SUBFRAME_DRX_CYCLE    8
#define TEN_SUBFRAME_DRX_CYCLE      10
#define SIXTEEN_SUBFRAME_DRX_CYCLE  16
#define TWENTY_SUBFRAME_DRX_CYCLE   20

#define MAX_STMR_DYNAMIC_CNT        4096

#define SRCHCPCDRX_ASET_SRCH_MIN_PERIOD 5
#define SRCHCPCDRX_NSET_SRCH_MIN_PERIOD 10

#define SRCHCPCDRX_INIT_TIME_DIFF_OFFSET 2

#define IS_SRCH_CPC_DRX_ACTIVE_AWAKE()  \
         (srchcpcdrx_db.substate == DRX_ACTIVE_AWAKE)

#define IS_SRCH_CPC_DRX_ACTIVE_ASLEEP()  \
         (srchcpcdrx_db.substate == DRX_ACTIVE_SLEEP)

#define IS_SRCH_CPC_DRX_ACTIVE()    \
         ((srchcpcdrx_db.substate != DRX_INACTIVE) && (srchcpcdrx_db.substate != DRX_START_INITIATED))

#define SRCHCPC_DRX_SCHEDULING_ACTIVE()   \
       (srchcpcdrx_db.mode == SRCHCPCDRX_DRX_MODE)

#define SRCH_DRX_MODE_NOT_FREEZE()   \
       (srchcpcdrx_db.drx_mode_freeze == FALSE)

#define SRCH_DRX_MODE_FROZEN()   \
       (srchcpcdrx_db.drx_mode_freeze == TRUE)

#define IS_AWAKE_TIME_CAL_NEEDED() \
       ((srchcpcdrx_db.substate == DRX_ACTIVE_AWAKE) && SRCH_DRX_MODE_NOT_FREEZE() && (srchcpcdrx_db.hb_mode_for_step12 == TRUE))

#define IS_SRCH_READY_FOR_DRX_SCHEDULING()  \
        (IS_SRCH_CPC_DRX_ACTIVE() && \
             SRCH_DRX_MODE_NOT_FREEZE() && (srchcpcdrx_db.hb_mode_for_step12 == FALSE))

#define  SRCHCPCDRX_USE_NSET_MASK_FROM_DRX_SCHEDULER() \
         (IS_SRCH_CPC_DRX_ACTIVE() && SRCH_DRX_MODE_NOT_FREEZE() && \
         SRCHCPC_DRX_SCHEDULING_ACTIVE())
         
#define  SRCHDCH_DRX_SET_STEP1_NN(nn)   \
   do{           \
      (srch_step1_parms.srchStep1NonCohIntgLen = nn); \
       mcalwcdma_srch_update_srch_async_parms(MCALWCDMA_SRCH_STEP1,(mcalwcdma_srch_async_parms_type *) (&srch_step1_parms)); \
     }while(0)

 extern srchcpcdrx_db_struct_type srchcpcdrx_db;
 
 extern int16 srchcpcdrx_async_srch_time;
 
 extern const srchcpcdrx_filt_tab_indx_struct_type srchcpcdrx_filt_coef_indx_tab[MAX_NUM_DRX_CYCLES];

 extern const uint16 srch_filter_coef_tab[ SRCH_FILTER_COEF_TABLE_K_VALUE ][ SRCH_FILTER_COEF_TABLE_PN_SRCH_INTERVAL ];

/*===========================================================================

FUNCTION SRCHCPCDRX_INIT

DESCRIPTION
  This function will initialize the search scheduling parameter for the DRX mode searches.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_init(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_CONFIIGURE_DRX_MODE

DESCRIPTION
   This function will configure the DRX mode parameter and start DRX mode
   in SRCH side.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_configure_drx_mode(uint8 dch_drx_cycle_len);


/*===========================================================================

FUNCTION SRCHCPCDRX_CLEANUP

DESCRIPTION
  This function will cleanup the srch scheduling parameter used for DRX mode searches and
  scheduled the searches back to on ragular HB.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_cleanup(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_REMAIN_WAKEUP

DESCRIPTION
  This function will switch the serch scheduling mode from DRX cnt to ragular HB

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_wakeup_cb(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_SCHEDULE_SERCHES

DESCRIPTION
  This function will schedule the searches in DRX mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/


extern void srchcpcdrx_schedule_searches(void);


/*===========================================================================

FUNCTION SRCHCPCDRX_SWITCH_SRCH_MODE

DESCRIPTION
  This function will switch the searches scheduling mode from 
  DRX scheduling<-> Ragular HB

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_switch_srch_mode(srchcpcdrx_mode_enum_type srch_mode);

/*===========================================================================

FUNCTION SRCHCPCDRX_SLEEP_CB

DESCRIPTION
  This function will be called by DRX MGR whwn SRCH will say that 
  it is ready to SLEEP. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_sleep_cb(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_PROGRAM_NEXT_WAKEUP_OCCASION

DESCRIPTION
  This function will program the next wakeup occasion with DRX MGR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void srchcpcdrx_program_next_wakeup_occasion(void);

/*===========================================================================

FUNCTION SRCHCPCDCH_TIME_ELAPSED_FOR_SRCH_ACTIVITY

DESCRIPTION
  This function will give the time elapsed between two time instances.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void srchcpcdrx_time_elapsed_for_srch_activity(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_START_DRX_MODE

DESCRIPTION
  This function will start the drx mode .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_start_drx_mode(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_CONFIG_DRX_MODE

DESCRIPTION
  This function will be called from local command processing context and 
  based on whether it is wakeup time init or dldrx schedule update
  respective functions are called from here.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcpcdrx_config_cmd_handler( srchcpcdrx_config_cmd_type *config_cmd_ptr);


/*===========================================================================

FUNCTION SRCHCPCDRX_STOP_DRX_MODE

DESCRIPTION
  This function will stop the drx mode .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void srchcpcdrx_stop_drx_mode(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_CHECK_EARLY_LATE_WAKEUP

DESCRIPTION
  This function will whether the DRX CTRL wakes up SRCH early or late  .

DEPENDENCIES
  None.

RETURN VALUE
  TRUE ->  if  DRX CTRL wakes up SRCH early .
  FALSE -> if  DRX CTRL wakes up SRCH late.

SIDE EFFECTS
  None.

===========================================================================*/

boolean srchcpcdrx_check_early_late_wakeup(tlm_cfn_struct_type *out_time);

/*===========================================================================

FUNCTION SRCHCPCDRX_PROCESS_DRXCTRL_COMMANDS

DESCRIPTION
  This function will process all the commands given by DRX ctrl to SRCH. This is
  the only function that is exposed to DRX MGR to interface with SRCH module.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/


void srchcpcdrx_process_drxctrl_commands(srchdrx_drxctrl_cmd_struct_type *cmd_ptr);

/*===========================================================================

FUNCTION    SRCHCPCDRX_CONFIG_DRX_FILT_MAX_LEN

DESCRIPTION
    This function will intialize the maximum fillter length during the DRX 
    mode sheduling switching.

DEPENDENCIES
  None.

PARAMETERS

 
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/

extern void srchcpcdrx_config_drx_filt_max_len(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_INIT_WAKEUP_TIME

DESCRIPTION
  This function will be start drx mode and program the SRCH wakeup occassion.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void srchcpcdrx_init_wakeup_time(uint8 start_cfn,
                              uint8 tick_offset);

/*===========================================================================

FUNCTION SRCHCPCDRX_FREEZE_SRCH_SCHEDULING

DESCRIPTION
  This function will freeze the drx mode srch scheduling and will switch the 
  scheduling back to normal HB .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void srchcpcdrx_freeze_srch_scheduling(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_UNFREEZE_SRCH_SCHEDULING

DESCRIPTION
  This function will unfreeze the drx mode srch scheduling and will switch the 
  scheduling back to DRX mode .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void srchcpcdrx_unfreeze_srch_scheduling(void);

/*===========================================================================

FUNCTION  SRCHCPCDRX_DISCARD_COMMAND

DESCRIPTION
    This function will check if the srch command need to process or not

DEPENDENCIES
  None.

PARAMETERS

 
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
 
 boolean srchcpcdrx_discard_command(srch_ops_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION  SRCHDRX_STEP1_ABORT_PROC

DESCRIPTION
  This function will be called whenever step1 will be aborted in the CM gap to 
  speed up next step1 command.

DEPENDENCIES
  None.

PARAMETERS

 
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void srchcpcdrx_step1_abort_proc(void);

/*===========================================================================

FUNCTION  SRCHDRX_STEP2_ABORT_PROC

DESCRIPTION
  This function will be called whenever step2 will be aborted in the CM gap to 
  speed up next step2 command.

DEPENDENCIES
  None.

PARAMETERS

 
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void srchcpcdrx_step2_abort_proc(void);

/*===========================================================================
FUNCTION srchcpcdrx_log_step1_srch_start

DESCRIPTION
  This function is the pre-command callback called when MDSP starts processing step1
  search command. This function is used for logging purpose to indicate the
  start of search.

INPUT

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void srchcpcdrx_log_step1_srch_start(void);

/*===========================================================================
FUNCTION srchcpcdrx_log_step2_srch_start

DESCRIPTION
  This function is the pre-command callback called when MDSP starts processing step2
  search command. This function is used for logging purpose to indicate the
  start of search.

INPUT

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcpcdrx_log_step2_srch_start(void);

/*===========================================================================
FUNCTION srchcpcdrx_log_list_srch_start

DESCRIPTION
  This function is the pre-command callback called when MDSP starts processing list
  search command. This function is used for logging purpose to indicate the
  start of search.

INPUT

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcpcdrx_log_list_srch_start(void);

/*===========================================================================
FUNCTION srchcpcdrx_log_srch_stop

DESCRIPTION
 This function is used for logging purpose to indicate the stop of search.

INPUT

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcpcdrx_log_srch_stop(void);

/*===========================================================================

FUNCTION  srchcpcdrx_check_if_searches_are_programmed

DESCRIPTION
  This function increments a count every time DL DCH maintenance event is called 
  (every 10 ms) if CPC is active and DRX is in ACTIVE_AWAKE. If that count exceeds
  100 then we ERROR FATAL.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
extern void srchcpcdrx_check_if_searches_are_programmed(void);

/*===========================================================================

FUNCTION  srchcpcdrx_increment_counts_upon_wakeup

DESCRIPTION
  This function increments the pn search CPC counts upon wakeup 
  callback being invoked by the DLDRXCTRL

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void srchcpcdrx_increment_counts_upon_wakeup(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_CALCULATE_AND_SCHEDULE_NEXT_WAKEUP_OCCASION

DESCRIPTION
  This function will calculate the next wakeup occasion and will program it with the DRX Controller

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchcpcdrx_calculate_and_schedule_next_wakeup_occasion(void);

/*===========================================================================

FUNCTION SRCHCPCDRX_EXPIRE_ASET_PN_COUNT

DESCRIPTION
  This function expires the ASET PN search count for CPC DRX.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void srchcpcdrx_expire_aset_pn_count(void);

/*===========================================================================

FUNCTION  srchcpcdrx_set_step1_nn_parameters

DESCRIPTION
  This function increments a count every time DL DCH maintenance event is 
  called (every 10 ms) if CPC is active and DRX is in ACTIVE_AWAKE. If that 
  count exceeds 100 then we ERROR FATAL.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcpcdrx_set_step1_nn_parameters(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================

FUNCTION  SRCHCPCDRX_QSH_DUMP_CB

DESCRIPTION
  This function collects srch cpc info for QSH module during crash.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void srchcpcdrx_qsh_dump_cb(wl1_srcher_mini_debug_info_struct_type *addr_ptr_t);
#endif


/*===========================================================================

FUNCTION        srchcpcdrx_delta_time_from_ref_time

DESCRIPTION     This function calculates the difference between a reference time   
                and an event time 

DEPENDENCIES    None

RETURN VALUE     
TRUE -delta time is -ve i.e. evt time is in the past w.r.t ref_time
FALSE - delta time is +ve i.e. evt_time in future w.r.t ref_time

SIDE EFFECTS    None

===========================================================================*/
boolean srchcpcdrx_delta_time_from_ref_time(tlm_cfn_struct_type *ref_time, tlm_cfn_struct_type *evt_time, tlm_cfn_struct_type* delta_time);

#endif  /* SRCHCPCDRX_H */
