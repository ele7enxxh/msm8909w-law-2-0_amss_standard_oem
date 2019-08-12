#ifndef G2W_SRCH_PARM_H
#define G2W_SRCH_PARM_H

/*===========================================================================
   FILE: g2wsrchparm.h

GENERAL DESCRIPTION
       This header file contains parameter definitions used by search module.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2005,2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/g2wsrchparm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/12   mm      CR 344531 G2W drivers new design
21/09/10   cja     Fix Lint and KlocWork errors
23/02/10   ws      Integrate G2W driver from MDM8200 
06/27/07   am      Added STEP1 saturation value
06/11/05   cpe     Updation of parameters for 6275
04/20/05   cpe     Updation of parameters for 6250
03/09/04   cpe     Updated g2w srch parameters for 6250
02/27/04   cpe     FEATURE_SRCH_BITWIDTH_MULTI_FACTOR is added to account
                   for the changed bitwidth on saber
11/14/03   cpe     Changed several idle mode searching parameters to 
                   reflect changed systems algorithms
06/24/03   cpe     Added defines for the G2W dedicated mode
03/06/03   cpe     Added the following #defines
                    -G2W_LIST_SRCH_PATH_ECNO_THRESHOLD
                    -G2W_STEP3_SRCH_PATH_ECNO_THRESHOLD 
02/25/03   cpe     Added the following defines
                    G2W_SRCH_MAX_PSC_PEAKS_TO_KEEP
                   Updated a few others.
11/22/02   cpe     Adjusted several search parameters
07/23/02   cpe     Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
              Max Number of search tasks in LPQ 
---------------------------------------------------------------------------*/
#define G2W_SRCH_LPQ_MAX_TASKS  41

#define G2W_SRCH_CHIPX8_PER_FRAME 307200
#define G2W_SRCH_CHIPX1_PER_FRAME  38400
#define G2W_SRCH_CHIPX2_PER_SLOT    5120
#define G2W_CHIPX8_PER_CHIPX2          4

#include "geran_variation.h"
#define G2W_SRCHASET_NC  31
  /* (31 +1)*64   = 2048      */  
#define G2W_SRCH_ASET_NN 0
  /* corresponding to 1 */

#define  G2W_SRCH_ASET_WIN_SIZ  (128 * 8)
  /* Search window size for active set cells, in chipx8 */

#define  G2W_SRCH_PRI_CPICH_OVSF  0


#ifdef FEATURE_SRCH_BITWIDTH_MULTI_FACTOR
  #define G2W_SRCH_BITWIDTH_MULTI_FACTOR     4
#else
  #define G2W_SRCH_BITWIDTH_MULTI_FACTOR     1
#endif /* FEATURE_SRCH_BITWIDTH_MULTI_FACTOR */
                                
/* Step 1 Search Async Parameters */
#define G2W_SRCH_STEP1_NON_COH_INTG_LEN 45

#define G2W_SRCH_STEP1_ENG_THRESH       747

/* Step1 saturation energy value */
#define G2W_STEP1_SATURATION_ENG 65535

#define G2W_SRCH_MAX_PSC_PEAKS 6 

#define G2W_SRCH_MAX_PSC_PEAKS_TO_KEEP 8

/* Step 2 Search Async Parameters */
#define G2W_SRCH_STEP2_NON_COH_INTG_LEN 3
    /* Non-coherent integration length for Step_2 search */
#define G2W_SRCH_STEP2_ENG_THRESH       365
    /* Energy threshold of integrated SSC code group peaks */
#define G2W_SRCH_MAX_SSC_PEAKS 8


#define  G2W_SRCH_DUP_FRAME_PEAK_DIFF 4
  /* Difference of frame peak positions, in chipx8, used for duplicate
     frame peak detection.
   */

/* Step 3 Search Async Parameters */

#define G2W_SRCH_STEP3_COH_INTG_LEN     8
/* Coherent integration length for Step_3 Search, in unit of 256. 2048 chips */
#define G2W_SRCH_STEP3_NON_COH_INTG_LEN 1
/* Non-coherent integration lenght for Step_3 search */
    

#define G2W_SRCH_STEP3_ENG_THRESH       449
    /* Energy Threshold of Pilot peaks */
#define G2W_SRCH_STEP3_PEAKS_PER_TASK   4
    /* Number of peaks to be reported for each Step_3 task */
#define G2W_SRCH_STEP3_WIN_SIZ          64

    /* Window size for Step_3 Search  */
/* During debug, sometimes we want to return more PN peaks after NAS-TT than
** usual.  So bump this up to support that. */
#define G2W_SRCH_MAX_PN_PILOTS 120

/* Min energy before a path can be accepted as a useful one                       
 40 represents -20 dB in the table g2w_srch_ecio_tab[] 
 as the table is given in 0.5 dB steps */
#define G2W_STEP3_SRCH_PATH_ECNO_THRESHOLD  40
       
       
/* List Search Async Parameters */
#define G2W_SRCH_LIST_LPQ_ENG_THRESH 0
    /* Energy Threshold for LPQ list search */
#define G2W_SRCH_LIST_PEAKS_PER_TASK 4
    /* Number of peaks reported for each task */
#define G2W_SRCH_LIST_LPQ_PEAKS_PER_TASK G2W_SRCH_LIST_PEAKS_PER_TASK
    /* Number of peaks reported for each LPQ task */

/* Desired PN search threshold using STTD is -22.2 dB, or energy value
** 60 in g2w_srch_ecio_tab[], which is index 44 (-22 dB, raw:62). */
#define G2W_LIST_SRCH_PATH_ECNO_THRESHOLD   44

/* Desired Stage 2 search threshold using STTD is -21.1 dB, or energy value
** 76 in g2w_srch_ecio_tab[], which is index 42 (-21 dB, raw:77). */
#define G2W_STAGE2_SRCH_PATH_ECNO_THRESHOLD 42
                                                                                        
                                                          
/******************************************************** 
  Dedicated mode stage1 parameters                                                        
*********************************************************/                                                          
#define G2W_SRCH_STAGE1_NC  15  /*  (15+1) *64 = 1024 */

#define G2W_SRCH_STAGE1_NN  0   /*    1        */

#define G2W_SRCH_STAGE1_WIN_SIZE  448 /* in chips */

#define G2W_SRCH_STAGE1_PEAKS_PER_TASK  1

  /* More peaks handled with this feature.*/
#define G2W_SRCH_STAGE1_NUM_TASKS 140
                       
/******************************************************** 
  Dedicated mode stage2 parameters                                                        
*********************************************************/

#define G2W_SRCH_STAGE2_NC 31  /*  (31+1) *64 = 2048 */

#define G2W_SRCH_STAGE2_NN  0   /*    1        */
      
     
     
#define G2W_SRCH_STAGE2_WIN_SIZE  128 /* in chips */

#define G2W_SRCH_STAGE2_PEAKS_PER_TASK  4

#define G2W_SRCH_STAGE2_NUM_TASKS 35 /* +1 may be added */


#define  G2W_DEDMODE_RECONF_NC  31
  /* Coherent integration length used for reconf search,
     corresponding to 2048 chips */

#define  G2W_DEDMODE_RECONF_NN  0
  /* Non-coherent integration length for reconf search,
     corresponding to 1 coherent integration pass */     

#define G2W_DEDMODE_RECONF_WIN_SIZE_CHIPX8 1024
  /* PN search window size in chipx8 for G2W dedicated mode
     reconfirmation searches */

#define G2W_DEDMODE_RECONF_WIN_SIZE 128 /* chips */

#endif /* G2W_SRCH_PARM_H */
