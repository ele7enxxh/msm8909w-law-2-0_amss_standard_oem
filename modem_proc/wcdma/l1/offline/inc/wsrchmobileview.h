#ifndef WSRCHMOBILEVIEW_H
#define WSRCHMOBILEVIEW_H

/*===========================================================================
   FILE: wsrchmobileview.h

GENERAL DESCRIPTION:
      This header file contains srch related externalised functions,variables etc for PSC scanner search
      
      
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c)2000-2010 by Qualcomm Technologies, Inc. All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wsrchmobileview.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/10   sv      Mainlined feature FEATURE_WCDMA_OFFLINE_SEARCHER
12/05/07   scm     Fix MIN_DRX_TIME_FOR_PSC_SCAN_1STEP3_MSEC define.
12/04/07   scm     Add MIN_DRX_TIME_FOR_PSC_SCAN_1STEP3_MSEC define.
04/23/07   nd      Added SRCH_MVIEW_PSC_SCAN_GUARD_TIME_FR so that if psc scan
                   gets stuck due to unforeseen reasons,it is terminated
03/08/07   nd      Changed names of some macros
03/07/07   cpe     Wrapped the entire file under FEATURE_WCDMA_L1_PSC_SCANNER
03/07/07   nd      Initial creation
===========================================================================*/
#include "wcdma_variation.h"
#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "log.h"
#include "log_codes.h"
#include "srchcmd.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* PSC scanner srch will pick MAX best 12 peaks from step1 and schedule 2 step2
6 peaks each */
#define SRCH_MVIEW_MAX_STEP1_PEAK_CNT 12
/* MAX number of (psc,ecio,rscp) that can be reported after a PSC scan search */
#define MAX_NUM_PSC_SCAN_RSLT 32

/* Approx time for the S1+ S2+ S3 */
//#define MAX_TIME_PSC_SCAN_1STEP3_MS  80
/* Approx time for the S1+ S2+ S3+S2+S3 */
//#define MAX_TIME_PSC_SCAN_2STEP3_MS  120

/* step1 srch timing 30ms 
    NASTT(unmon) (2048,2) srch timing ~ 6ms/step1 peak; so for 6 step1 peaks =~ 4frames 
    PN srch timing 0.20 ms per task so for max 32 tasks 32*0.2ms= 6.4ms ~ 1 frame 
    timings are in frames/srch HB ticks i.e 10 ms 
*/
#define MAX_TIME_PSC_SCAN_FR(step1,nastt,pn) ((step1)*3+(nastt)* 4 + (pn)*1) 

/* times in frames i.e 10ms units */
#define MIN_DRX_TIME_FOR_PSC_SCAN_12_STEP1_PK  MAX_TIME_PSC_SCAN_FR(1,2,2)
#define MIN_DRX_TIME_FOR_PSC_SCAN_6_STEP1_PK  MAX_TIME_PSC_SCAN_FR(1,1,1)

#define SRCH_PSC_SCAN_ACTIVE() ((srchzz_substate == SRCHZZ_PSC_SCAN_STEP1)     \
             || (srchzz_substate == SRCHZZ_PSC_SCAN_STEP2)                                    \
             || (srchzz_substate == SRCHZZ_PSC_SCAN_STEP3)                                    \
             || (srchzz_substate == SRCHZZ_PSC_SCAN_LIST))

/* PSC scanner states for the event EVENT_WCDMA_PSC_SCANNER_STATE */

#define PSC_SCAN_STARTED_ON_REQ  0x01
#define PSC_SCAN_STOPPED_ON_REQ  0x02
#define PSC_SCAN_STOPPED_EXCEEDING_MAX  0x03

/* MVIEW CMD received in the DIAG REQ/RSP packet */
#define SRCH_MVIEW_CMD_ON   0x01
#define SRCH_MVIEW_CMD_OFF 0x00

/* CMD response */
#define SRCH_MVIEW_STATUS_ACCEPTED   0x01
#define SRCH_MVIEW_STATUS_ERROR 0x00

/* This is the guard timer for PSC scan. Beyond this no scanning activity can proceed.
At the expiry of this timer the PSC scan will be aborted and control will be passed to DRX
MAX search time should be = 1 S1+ 2S2+ 2S3+1LS = 1(3)+2(3)+2(2)+1(2)= 13 frames. Adding some
buffer we have chosen 18 frames to be enough for the srch */
#define SRCH_MVIEW_PSC_SCAN_GUARD_TIME_FR  18
typedef struct
{

  /*keeps a record of the timestamp when the srch was triggered.source srch HB*/
  uint16 start_ts;
  /* keeps the total DRX time available for the psc scan = time to next page Occasion(in 10 ms frames) */
  uint16 tot_sleep_time;
  /* remaining time (in 10 ms frames) to next PO. if any srch does not get a response from mdsp within
  this time, lost srch procedure will be triggered and l1 will go back to DRX  */
  uint16 rem_sleep_time;
  /* number of drx cycles the search has been performed after requested. just to keep 
  track how long the scanning is being done. After a large no N it will be switched off
  automatically assuming that it has received the OFF cmd */
  uint16 num_drx_cycles_scanned;
}srch_psc_scan_time_keeper_struct;

/* This keep track of the time we have during DRX.*/
extern srch_psc_scan_time_keeper_struct srch_mview_time_keeper;

/* the flag stores the command from the diag req/rsp. TRUE indicates to start the scanner and
FALSE indicates to STOP the scanner */
extern boolean srch_mview_psc_scan_cmd;

/* THis flag will allow the psc scan search to happen only once per DRX.The procedure of getting 
into sleep being the same, will result in psc scan multiple times. 
After srch is done this flag will be turned off so srch is done only once per DRX.
It will be activated again when we are going to sleep in next DRX*/
extern boolean srch_mview_srch_current_drx;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*===========================================================================

FUNCTION SRCH_MVIEW_START_SRCH

DESCRIPTION
  This function starts the FULL STEP1/2/3 search for the PSC scanner.
   time to paging -(in # of frames NOT in ms) indicates time to next PO
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PSC scan srch has been started successfully this DRX .
  FALSE- PSC scan could not be started this DRX. UE can sleep now
  
SIDE EFFECTS
 .

===========================================================================*/
extern boolean srch_mview_start_srch(uint16 time_to_paging);

/*===========================================================================

FUNCTION SRCH_MVIEW_STOP_SRCH

DESCRIPTION
  This function will be called whenever the PSC Scanner searches in a DRX cycle
  needs to be turned off
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_mview_end_srch(void);

/*===========================================================================

FUNCTION SRCH_MVIEW_STEP1_SRCH_DONE

DESCRIPTION This function handles the step1 search results from mDSP 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_mview_step1_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCH_MVIEW_STEP2_SRCH_DONE

DESCRIPTION This function handles the step2 srch results from mDSP
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_mview_step2_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCH_MVIEW_STEP3_SRCH_DONE

DESCRIPTION This function handles the step3 search results from mDSP
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_mview_step3_srch_done (srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCH_MVIEW_LIST_SRCH_DONE

DESCRIPTION This function will process the list srch peaks 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_mview_list_srch_done( srch_done_cmd_type* srch_cmd );

/*===========================================================================

FUNCTION SRCH_MVIEW_POST_SCAN_STATE_EVENT

DESCRIPTION This function will post a event regarding the psc scanner state 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_mview_post_scan_state_event( uint8 scan_state );


#endif /* FEATURE_WCDMA_L1_PSC_SCANNER */
#endif /* WSRCHMOBILEVIEW_H*/

