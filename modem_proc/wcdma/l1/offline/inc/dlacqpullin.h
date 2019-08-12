#ifndef DLACQPULLIN_H
#define DLACQPULLIN_H

/*===========================================================================
                        D L A C Q P U L L I N . H

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dlacqpullin.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/16/14    bj      Remove duplicate code in split ACQ code
12/16/13    as      Porting G2W TA from Triton to Dime.
12/16/13    bj      split ACQ feature Porting from Triton to Dime.
08/23/13    pr      MCVS:function declaration
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/23/12    kcm     L/G to W handover optimizations
04/05/12    amj     Reduce pullin timeout for BPLMN scenarios
02/12/12    hk      Nikel feature cleanup
07/24/09    sq      Fixed file merge error
07/22/09    sq      Added function dl_acq_pullin_is_complete
07/16/09    hk      Reorganized the code to remove INC/SRC violations
04/17/09    rgn     Fixed compiler warnings
02/12/09    scm     Modify ACQ_PULLIN_DEL_ROTATOR_THRESH_FOR_BPLMN for offline demod.
02/12/09    scm     Modify ACQ_PULLIN_DEL_ROTATOR_THRESH for offline demod.
12/17/08    ks      Mainline FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT and
                    FEATURE_WCDMA_OFFLINE_SEARCHER 
12/12/08    sv      Use SRCH_PULLIN_SET_MAX instead of SRCH_MAX_PULLIN_PILOTS.
09/13/08    nd      Added a element to dl_acq_pullin_info to remember the last
                    scr code that was assigned a finger during pullin 
08/13/07    vp      Moved dl_acq_pullin_event_handler from dltimeline.c.
08/06/07    vsr     Created file.

===========================================================================*/

/*--------------------------------------------------------------------------
                          Included Header Files
--------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "l1def.h"
#include "srchdmod.h"

#define ACQ_PULLIN_TIMEOUT_COUNT 30

/* RSSI threshold that a finger RSSI must meet in order to satisfy the pull-in
   criteria */
#define ACQ_PULLIN_RSSI_THRESH 235

/* the threshold for the pull-in heartbeat to heartbeat finger rotator
   difference in order to meet the pull-in criteria */
/* Units (7.3242 / 16) Hz. */
#define ACQ_PULLIN_DEL_ROTATOR_THRESH (20 << 4)

/*We do not make any decision during Initial Decision Timeout (IDT = 5 finger dumps).*/
#define ACQ_MIN_TIME_FOR_PULLIN 5

/* Global pullin timeout here */
#define DL_ACQ_PULLIN_GLOBAL_TIMEOUT 60

/* The following are to be used when FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
   feature is turned on */
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

/* The pullin time out value, corresponds to 50ms */
#define ACQ_PULLIN_TIMEOUT_COUNT_FOR_BPLMN 25

/* RSSI threshold that a finger RSSI must meet in order to satisfy the pull-in
   criteria. The value corresponds to -20 dB */
#define ACQ_PULLIN_RSSI_THRESH_FOR_BPLMN 164*2

/* the threshold for the pull-in heartbeat to heartbeat finger rotator
   difference in order to meet the pull-in criteria, corresponds to 140Hz */
/* Units (7.3242 / 16) Hz. */
#define ACQ_PULLIN_DEL_ROTATOR_THRESH_FOR_BPLMN (20 << 4)

/* Global pullin timeout here */
#define DL_ACQ_PULLIN_GLOBAL_TIMEOUT_BPLMN 50

#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */

/******************************************************************************
                         COMMAND: DL_PHYCHAN_CFG_CMD
******************************************************************************/
/*--------------------------------------------------------------------------
                     STRUCTURE: DL_ACQ_PULLIN_CMD_TYPE

This struture contains the parameters for the DL_ACQ_PULLIN_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  l1_local_cmd_hdr_type hdr;

  /* the rf frequency */
  uint16 rf_freq;

  /* the initial finger rotator value */
  int16 init_rotator;

} dl_acq_pullin_cmd_type;


/******************************************************************************
                         COMMAND: DL_ACQ_PULLIN_EVT_CMD
******************************************************************************/
/*--------------------------------------------------------------------------
                STRUCTURE: DL_ACQ_PULLIN_EVT_CMD_TYPE

This struture contains the parameters for the DL_ACQ_PULLIN_EVT_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  l1_local_cmd_hdr_type hdr;

  /* dummy parameter to keep the compiler happy */
  uint16 dummy;
} dl_acq_pullin_evt_cmd_type;

/* -------------------------------------------------------------------------
              STRUCTURE: DL_ACQ_PULLIN_PEAK_STRUCT_TYPE
This structre has variables which are used to save the pos, psc, rssi
of each finger during the pullin process.
--------------------------------------------------------------------------*/
typedef struct{
  /*Index of the strongest pull in finger tracking this PSCs*/
  uint16 pullin_fing_index;
  uint16 psc;
  /*Position correspoding to the strongest pullin finger.*/
  uint32 pos;
  /*Gives the combined RSSI of the strongest pulled in finger*/
  uint32 comb_rssi;
  /*Accumulated combnined RSSI of the fingers tracking this PSC*/
  uint32 accum_cell_rssi;
} dl_acq_pullin_peak_struct_type;

typedef enum
{
  PULLIN_STATE_UNASSIGNED,
  PULLIN_STATE_ONGOING,
  PULLIN_STATE_WEAK_PASS,
  PULLIN_STATE_PASS
}dl_acq_pullin_fing_state_enum_type;

/*--------------------------------------------------------------------------
              STRUCTURE: DL_ACQ_PULLIN_INFO_STRUCT_TYPE

This structure contains variables used for the Acquisition Pull-In process.
--------------------------------------------------------------------------*/
typedef struct {
  /* indicates that the Pull-in algorithm has completed */
  boolean pullin_complete;

  /* how many frames (10 ms) before the pull-in process times out */
  uint16 timeout_ctr;

  /* how many fingers will be used during the pullin process */
  uint16 num_fingers;

  /* storage for the previous finger rotator value so that the delta may be
     computed at each pull-in heartbeat */
  int16 prev_rot_val[MSMHW_MAX_NUM_FINGERS];

  dl_acq_pullin_fing_state_enum_type fing_state[MSMHW_MAX_NUM_FINGERS];

  /* save the info of the cells that pulled in */
  dl_acq_pullin_peak_struct_type finger_pulled_in_psc_list[SRCH_PULLIN_SET_MAX];

  /* the finger that pulled in */
  uint16 pullin_finger;

  /* during SPLIT ACQ fingers will be deassigned post pullin as TRM will be released.
  So remember the cell on which pullin was done */
  uint16 pullin_psc;

  /* the rf frequency */
  uint16 rf_freq;

  /* threshold on step3 search peaks as to what will be considered
     a pullin candidate */
  uint16 path_thresh;

  /* Needed for ignoring the very first status dump since that may result in
     garbage values */
  uint16 status_dump_cnt;

  uint16 num_psc_pulled_in;

  /* once the pullin operation fails this flag will be set to TRUE*/
  boolean pullin_failed;
  /* This is the global timer beyond which pullin cannot continue */
  uint16 global_timeout_ctr;
  /* num of fingers that has pulled in */
  uint8 num_pulled_in_fing;

  /* keep track of the last scr code from which paths has already been allocated */
  uint16 last_allocated_scr_code;

} dl_acq_pullin_info_struct_type;

/******************************************************************************
        VARIABLES TO SUPPORT ACQUISITION PULLIN
******************************************************************************/

extern uint16 dl_pullin_cur_path_index;

extern dl_acq_pullin_info_struct_type dl_acq_pullin_info;


/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_ASSIGN_FINGERS

FILE NAME       dlacqpullin.c

DESCRIPTION     This function assigns fingers to the peaks determined by
                the search module to be those used for the pull-in phase
                of acquisition

DEPENDENCIES    The search pull-in set

RETURN VALUE    None

SIDE EFFECTS    The peaks listed in the search pull-in set will have fingers
                assigned to them.
===========================================================================*/
extern void dl_acq_pullin_assign_fingers(int16 rotator);

/*===========================================================================
FUNCTION        DL_ACQ_ENABLE_PULLIN_EVENTS

FILE NAME       dlacqpullin.c

DESCRIPTION     This function enables or disables the pullin events based
                on the boolean input parameter

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The pullin events will be enabled or disabled
===========================================================================*/
extern void dl_acq_enable_pullin_events(boolean enable);

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_CMD

FILE NAME       dlacqpullin.c

DESCRIPTION     This function begins the frequency pull-in procedure of the
                Acquisition state.

DEPENDENCIES    The search pull-in set.

RETURN VALUE    None

SIDE EFFECTS
===========================================================================*/
extern void dl_acq_pullin_cmd(dl_acq_pullin_cmd_type *cmd);

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_COMPUTE_ACCUM_CELL_RSSI

DESCRIPTION     On a per finger basis, this function gets the psc index 
                from the db of the cell the finger is tracking. Computes
                the combined RSSI of the finger and add that to the 
                accumulated RSSI of all the fingers that were tracking
                a particular cell.  Also this function keeps track of 
                which pull in pass finger among the fingers tracking a 
                specific PSC has the maximum RSSI. This will be the final 
                pull in finger if the cell is finally detected as the 
                strongest cell.

DEPENDENCIES

RETURN VALUE    void

SIDE EFFECTS    
===========================================================================*/

extern void dl_acq_pullin_compute_accum_cell_rssi(void);

/*===========================================================================
FUNCTION        DL_ACQ_FIND_PULLIN_FING

FILE NAME       dlacqpullin.c

DESCRIPTION     This function loops through all the unique PSCs and gets 
                PSC with maximum accumulated energy from the fingers
                that were tracking this cell. Once the strongest cell 
                is obtained,the pull in pass finger tracking that cell
                and having maximum combined RSSI is chosen as the pull
                in finger and this is the return value of this function. 

DEPENDENCIES

RETURN VALUE    The finger index that is selected is the pull in finger index.

SIDE EFFECTS    
===========================================================================*/
extern uint16 dl_acq_find_pullin_fing(void);

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_EVT_CMD

FILE NAME       dlacqpullin.c

DESCRIPTION     This function handles the DL_ACQ_PULLIN_EVT_CMD which is
                posted to the L1 local command queue when one of the
                Pull-in events on the RX timeline fires.  It samples the
                RSSI of the fingers being used for pull-in and also checks
                for timeout.  It will send the appropriate commands to the
                L1 local command queue if pull-in is successful or has timed
                out.

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS    The ACQ Pull-in substate may be exited.
===========================================================================*/
extern void dl_acq_pullin_evt_cmd(dl_acq_pullin_evt_cmd_type *cmd);

/******************************************************************************
                       COMMAND: DL_ACQ_PULLIN_DONE_CMD
******************************************************************************/

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_DONE_CMD

FILE NAME       dlacqpullin.c

DESCRIPTION     This function handles the DL_ACQ_PULLIN_DONE command.

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS
===========================================================================*/
extern void dl_acq_pullin_done_cmd(dl_acq_pullin_done_cmd_type *cmd);

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_EVENT_HANDLER

DESCRIPTION     This function handles the acqusition state pull-in event.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void dl_acq_pullin_event_handler(void);

/*===========================================================================
FUNCTION        dl_acq_resume_pullin_on_cell_cfg

DESCRIPTION     This function resumes pullin procedure by enabling the pullin 
                events following configuration of cells needed for pullin

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS
===========================================================================*/
extern void dl_acq_resume_pullin_on_cell_cfg(void);

/*===========================================================================
FUNCTION        DL_ACQ_RESUME_PULLIN_DONE

FILE NAME       dlacqpulin.c

DESCRIPTION     resume pullin done activity after the non pulled in cells are deconfigured
                and DL calls this function to resume the activity 

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void dl_acq_resume_pullin_done(void);

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_RESUME_AFTER_FIRST_TIMEOUT

FILE NAME       dlacqpulin.c

DESCRIPTION   resume the subsequent pullin operation on the remaining paths 

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void dl_acq_pullin_resume_after_first_timeout(void);

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_ENG_SORT_PATHS_PER_CELL

FILE NAME       dlacqpulin.c

DESCRIPTION     This function sorts the paths from a cell. This is needed when we have paths from
both the antenna (in Rxd mode separate). Paths from each antenna are sorted but when we combine
them and need to pick best N peaks , we need to sort peaks combined from both ant and then sort

DEPENDENCIES    None

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void dl_acq_pullin_eng_sort_paths_per_cell(int16 first_idx, int16 last_idx);

/*===========================================================================
FUNCTION        DL_ACQ_PULLIN_IS_COMPLETE

FILE NAME       dlacqpulin.c

DESCRIPTION    For testing purpose

RETURN VALUE   Returns "pullin_complete" status  

SIDE EFFECTS
===========================================================================*/
extern boolean dl_acq_pullin_is_complete( void );

/*===========================================================================
FUNCTION        DL_ACQ_GET_STTD_PULLIN_PSC

FILE NAME       dlacqpulin.c

DESCRIPTION    

RETURN VALUE   Returns sttd status of pulled in psc

SIDE EFFECTS
===========================================================================*/
extern boolean dl_acq_get_sttd_pullin_psc( void );

/*===========================================================================
FUNCTION        dl_acq_pullin_get_pullin_set_cnt

FILE NAME       dlacqpulin.c

DESCRIPTION     Return the count of pullin set when requested by MCVS module

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern uint8 dl_acq_pullin_get_pullin_set_cnt(void);

#endif /* DACQPULLIN_H */
