
#ifndef DLTRI_PARM_H
#define DLTRI_PARM_H

/*===========================================================================
   FILE: dltriparm.h

GENERAL DESCRIPTION:
      This header file contains parameter definitions used  for triage operation.
      
      
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000, 2001, 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dltriparm.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/14   hk      DL Features Mainline
05/20/13   pr      SW support for FW Shadow Cell
04/10/13   pr      Revert change for finger reslam limit of 64cx1. 
12/18/12   dm      Change finger reslam limit to 64 cx1.
09/26/12   kcm     Changes to reslam OOL fingers under certain criterion
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   pv      Parallel reacq changes for buckshot.
06/06/12   kcm     Optimization of bounding conditions for standoff/sidelobe algorithms 
04/22/12   kcm     Reference finger algorithm changes
03/27/12   mmr     Limiting the Buckshot spread to 6 for primary/diversity.
01/30/12   vs      Feature cleanup.
01/26/12   kcm     Added New standoff alogrithm changes.
12/02/11   stk     Added support for FEATURE_WCDMA_RXD_PCH and FEATURE_IDLEMODE_FILTER
11/15/11   hk      Added new defines for combiner lock threshold values.   
10/11/11   ks      Finger standoff age changed from 10 to 100 triage cycle
09/19/11   ks      Added new defines for Assymetric alpha for buckshot and
                   threshold value update.
03/17/10   ks      Added 2nd and 3rd sidelobe energy ratio
12/17/09   ks      Added finger jump threshold macro. Finger can't be reslammed
                   by more than this constant (512 Cx1)
08/21/09   sv      Increase the allowable path spread between fingers on same 
                   cell to 224 cx1.
12/22/08   ks      Mainlining FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
10/24/08   mmr     Updated the combiner & soft threshold values for taxis.
10/10/08   ks      Added path spres constant DL_TRI_PATH_SPREAD
09/02/08   mmr     Added definitions for 2nd and 3rd sidelobe detection.
01/25/08   mmr     Added a definition for Max_num ngbr fingers.
05/22/07   rmak    Define MSMHW_MAX_NUM_FINGERS_FOR_LOW_MDSP_CLK_SPEED to 4 fingers
05/14/07   mmr     Added Dynamic fing granularity thresholds.
03/21/07   bbd     Removed srch timing adjust feature.
01/30/07   bbd     Added bounded JTT threshold.
10/06/06   bbd     Corrected nodeB diversity requirement.
07/07/06   bbd     Added transmit diversity specific sidelobe ratios.
03/20/06   bbd     Corrected max unlock age macro.
03/16/06   bbd     Under FEATURE_SHALLOW_BUCKSHOT_SEARCH set threshold to 1/3.
02/08/06   bbd     Added DL_TRI_COMB_LOCK_THRESH.
11/01/05   bbd     Added support for finger buckshot code.
10/11/05   bbd     Increased max and lower finger counts based on FW requirements.
                   Added support for finger manager sub-module.
07/07/05   bbd     Removed FEATURE_SRCH_GRANULARITY_WORKAROUND and created separate
                   granularity definitions to support run-time selection.
04/28/05   bbd     Mainlined FEATURE_WCDMA_VCTCXO.
03/25/05   bbd     Under FEATURE_SRCH_GRANULARITY_WORKAROUND, increase finger and
                   path granularity from 6 to 7 WCx8.
02/03/05   bbd     Mainlined FEATURE_FING_PERF_ENHANCE.
10/14/04   bbd     Changed DLTRI_SIDELOOP_RATIO to DLTRI_SIDELOBE_RATIO.
07/27/04   bbd     Added DL_TRI_FING_STANDOFF_MAX_AGE for finger standoff detection
07/21/04   bbd     Added DL_TRI_REF_FING_DIST_DISCRIMINATOR and MAX_DELAY_TRIAGE_CYCLES
04/28/04   bbd     Moved DLTRI_SIDELOOP_RATIO out of source code and into header file.
02/11/04   bbd     De-coupled MSM5200_NUM_FINGERS from FEATURE_FING_MERGE_PROTECTION
                   Added FEATURE_ENABLE_8_FINGERS.
06/05/03   scm     Remove INITIAL_FINGER_ROTATOR define. It's in demoddrv.h.
06/02/03   yus     Define MSM5200_NUM_FINGER_FOR_LOW_MDSP_CLK.
05/29/03   src     Removed hash-defined MSM5200_NUM_FINGERS_NON_DCH symbol as
                   it is no longer needed.
04/28/03   yus     Added support for FEATURE_FING_PERF_ENHANCE.
04/22/03   cpe     Renamed SRCH_PATH_THRESHOLD as SRCH_NORMAL_MODE_PATH_THRESHOLD.
09/17/02   yus     Added FEATURE_FINGER_REUSE.
07/19/02   yus     Added support for finger merge protection
06/19/02   yus     Changed DL_TRI_SOFT_LOCK_THRESH to 240.
06/04/02   sh      Changed DL_TRI_PATH_THRESH to SRCH_PATH_THRESHOLD.
05/08/02   sh      Defined MSM5200_NUM_FINGERS_NON_DCH - number of fingers
                   in non-DCH state to be 3.
12/18/01   sh      Changed DL_TRI_PATH_THRESH value to 5000.
11/27/01   cpe      Renamed DL_TRI_MIN(MAX)_DIST_FOR_PATHLOBE as 
                    DL_TRI_MIN(MAX)_DIST_FOR_SIDELOBE 
11/13/01   cpe     Changed DL_PATH_THRESH to 2309                   
10/25/01   cpe     Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "srchparm.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define MSMHW_LOWER_NUM_FINGERS                       10

#define MSMHW_MAX_NUM_FINGERS_FOR_LOW_MDSP_CLK_SPEED   4

#define MSM5200_PEAKS_PER_CELL  4

/* Max num of ngbr fings */
#define DL_TRI_MAX_NUM_NGBR_FINGERS 4

/* When checking if two fingers are so close together that
 they aren't both useful, the minimum offset that will
 be accepted as distinct. */
#define DL_TRI_PATH_GRAN    6

/* When trying to match up existing finger positions with
 paths found, the maximum offset that will be accepted
 as a match */           

#define DL_TRI_FING_GRAN    6


#define DL_TRI_WIN_GRAN     (1 * CHIPX8_PER_CHIP)

/* The minimum energy in searcher units that will be accepted
 as a possibly useful path, when the searcher is using
 standard parameters. Corresponds to -19 dB Ec/Io. */
/* corresponding to Ecio -23.5 dB */
#define DL_TRI_PATH_THRESH  SRCH_CELL_PATH_THRESH_ENG(TRUE)

#define DL_TRI_LOCK_MIN    3

#define DL_TRI_RSSI_RATIO  4

  /* a path has to be at least this stronger than the finger that is to
     be re-assigned to this path */
#define DL_TRI_PATH_RATIO   2

/* This corresponds to approximately -27dB, according to the new table for taxis */
#define DL_TRI_COMB_LOCK_THRESH   131
/* This corresponds to approximately -24dB, according to the new table for taxis */
#define DL_TRI_SOFT_LOCK_THRESH   260

/* This corresponds to approximately -29dB - Q13 Format
  10 log (82/(8*2^13)) = -29dB */
#define DL_TRI_COMB_LOCK_THRESH_29   82

/* This corresponds to approximately -15dB */
#define DL_TRI_COMB_LOCK_CHANGE_THRESH_TRIG_15   2050

/* This corresponds to approximately -20dB */
#define DL_TRI_COMB_FING_RSSI_TRIG_20   656

/* This corresponds to approximately -20dB */
#define DL_TRI_RSSI_THRESH_20DB   656

/* This corresponds to approximately -10dB */
#define DL_TRI_RSSI_THRESH_10DB   6554

/* This corresponds to approximately -16dB */
#define DL_TRI_COMB_FING_RSSI_TRIG_16   1647

/*max age to reset the num_tri_cyc_comb_rssi_thresh_satisfied counter*/
#define DL_TRI_COMB_FING_RSSI_COUNTER_MAX_AGE 5

#define DL_TRI_CELL_DIV_REQ_CNT   3

#define DL_TRI_MAX_UNLOCK_AGE  60

#define DL_TRI_MIN_DIST_FOR_SIDELOBE  10

#define DL_TRI_MAX_DIST_FOR_SIDELOBE   14

#define DL_TRI_MIN_DIST_FOR_SECOND_SIDELOBE  18

#define DL_TRI_MAX_DIST_FOR_SECOND_SIDELOBE  22

#define DL_TRI_MIN_DIST_FOR_THIRD_SIDELOBE   26

#define DL_TRI_MAX_DIST_FOR_THIRD_SIDELOBE   30

/* Minimun distance between two fingers that
   must be maintained */
#define FINGER_MIN_DIST         7

/* Energy ratio for main sidelobe */
#define DLTRI_SIDELOBE_RATIO    3
/* Energy ratio for sidelobes that are closer than normal */
#define DLTRI_CLOSE_SIDELOBE_RATIO        6
/* Energy ratio for sidelobes that are closer than normal and STTD enabled */
#define DLTRI_CLOSE_SIDELOBE_RATIO_STTD   5

/* Energy ratio for second sidelobe, 15dB */
#define DLTRI_SEC_SIDELOBE_RATIO 31

/* energy ratio for third sidelobe, 18dB */
#define DLTRI_THIRD_SIDELOBE_RATIO 61


/* the maximum number of times triage can be delayed */
#define MAX_DELAY_TRIAGE_CYCLES     12

/* the number of Cx8's to discriminate between potential Ghost peaks and real ones */
#define DL_TRI_REF_FING_DIST_DISCRIMINATOR  80

/* 70cx1 in cx8*/
#define DL_TRI_REF_FING_DIST_DISCRIMINATOR_70CX1  560

/* Corresponds to 1000Hz  in 16/7.32 format*/
#define DL_TRI_OOL_FING_RESLAM_FREQ_DELTA_THRESH  2185

/* maximum number of triage cycles two fingers can be in potential standoff before
   deassign the weaker finger */
#define DL_TRI_FING_STANDOFF_MAX_AGE 100

/*In good conditions (i.e if combined fing RSSI > -20dB) the standoff age is 10 triage cycles*/
#define DL_TRI_FING_STANDOFF_AGE_IN_GOOD_CONDS 10

/* min distnace between two paths of same cell in cx8 */
#define DL_TRI_PATH_SPREAD (256 * 8)

/* Finger reslam limit in Cx8: A finger can't be slammed more than this limit */
#define DL_TRI_FING_RESLAM_THRESH (512 * 8)

/* number of initial inputs that must be zero to slam filter state to threshold */
#define DL_TRI_MAX_INITIAL_INPUTS               4

/* alpha = .05, scaled by 1024 = 51 */
#define DL_TRI_FATPATH_DETECT_ALPHA            51
/* (1-alpha) = .95 scaled by 1024 = 973 */
#define DL_TRI_FATPATH_DETECT_1_MINUS_ALPHA   973

/* Using Assymetric alpha */
/* Step down alpha = .05, scaled by 1024 = 51 */
#define DL_TRI_FATPATH_DETECT_STEP_DOWN_ALPHA   51
/* Step up alpha = .25, scaled by 1024 = 256 */
#define DL_TRI_FATPATH_DETECT_STEP_UP_ALPHA     256
/* (1-step_down_alpha) = .95 scaled by 1024 = 973 */
#define DL_TRI_FATPATH_DETECT_1_MINUS_STEP_DOWN_ALPHA   973
/* (1-step_up_alpha) = .75 scaled by 1024 = 768 */
#define DL_TRI_FATPATH_DETECT_1_MINUS_STEP_UP_ALPHA     768
/* scaling factor for the fat path detection filter */
#define DL_TRI_FATPATH_DETECT_SCALING_FACTOR 1024
/* Initialize the fat path detection value to this */
#define DL_TRI_INITIAL_FATPATH_DETECT_VALUE  (1*DL_TRI_FATPATH_DETECT_SCALING_FACTOR)
/* Declare fatpath when detection value exceeds this threshold, (2048,2) search */
#define DL_TRI_FATPATH_DETECT_THRESH         210
/* Maximum Buckshot spread in half chip units, 3 chips = 6 half chip units
   x 2 for each direction (advance/retard) */
#define DL_TRI_MAX_BUCKSHOT_SPREAD              6*2
#define DL_TRI_MAX_BUCKSHOT_SPREAD_DIV          6

/* Maximum Buckshot spread in half chip units, -4 cx8 and +4 cx8 around the 4 strongest fingers*/
#define DL_TRI_MAX_BUCKSHOT_SPREAD_PICH_TL_REDUCTION              4*2
#define DL_TRI_MAX_BUCKSHOT_SPREAD_PRLL_REACQ 1*2
#define DL_TRI_MAX_BUCKSHOT_SPREAD_PRLL_REACQ_DIV 1*2

/* Maximum separation that two fingers of a pair can deviate
   in PN Position */
#define DL_TRI_MAX_FING_PAIR_SEPARATION  3

#endif
