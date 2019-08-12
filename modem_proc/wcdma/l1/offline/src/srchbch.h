#ifndef SRCHBCH_H
#define SRCHBCH_H
/*===========================================================================

   S R C H    B C H    S T A T E   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2001 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchbch.h_v   1.1   11 Jul 2002 21:28:06   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchbch.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/14   da      Searcher changes to support FR23776 : LTE to WCDMA CSFB delay optimizations
07/01/14   amj     Clean up old ASET update code.
09/12/13   jd      Searcher redesign
02/20/13   hk      Removed FEATURE_INTER_SIB_SLEEP
05/03/10   sv      Fixed compiler warnings.
02/06/10   sv      Mainlined feature FEATURE_WCDMA_OFFLINE_SEARCHER.
05/08/09   sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                   srchlog.h to wdec.h, wenci.h, wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
04/29/09   sq      Removed compiler warnings
04/22/09   stk     Compiler warning fixes	
05/26/06   kps     Added function prototype get_rx_agc_cache() to return a cached 
                   value of the Rx AGC.
02/13/06   ub      Added function prototype for evaluating S criteria of target
                   inter ngbr cell - srchbch_cell_select_eval_inter()
11/24/05  nd/gv    Modified prototype of srchbch_cell_select_eval() to use it
                   for W to W BPLMN search
08/30/05   sh      Added proto srchbch_cell_select_eval().
04/05/05   gv      Mainlined the feature FEATURE_L1_CELL_SELECTION and 
                   FEATURE_L1_CELL_RESELECTION.Also added a macro for
                   Cell Selection time out.
07/11/02   sh      Added srchbch_declare_reacq_status().
04/29/02   sh      Added srchbch substates for cell reselection.
03/27/02   sh      Created file. Moved declarations from srchbch.c to here.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "wsrchi.h"
#include "srchcmd.h"
#include "srchset.h"
#include "l1rrcif.h"
#include "wl1m.h"

/*--------------------------------------------------------------------------
                          BCH SEARCH SUBSTATES

--------------------------------------------------------------------------*/
typedef enum
{
  SRCHBCH_INIT,           /* Initialization */
  SRCHBCH_INACTIVE,       /* No search activity */
  SRCHBCH_ASET,           /* ASET Search */
  SRCHBCH_ASET_CR,        /* ASET Search for cell reselection */
  SRCHBCH_NSET_STEP1,     /* NSET Step 1 search */
  SRCHBCH_NSET_NASTT,     /* NSET NASTT search */
  SRCHBCH_NSET_LIST,      /* NSET PN search   */
  SRCHBCH_STATE_MAX       /* Last item in enum. */
} srchbch_substate_enum_type;


extern srchbch_substate_enum_type  srchbch_substate;

extern boolean  srchbch_nset_info_rxed;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SRCHBCH_PROC_HPQ_SRCH

DESCRIPTION
  This function processes search results from HPQ. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_proc_hpq_srch( void );

/*===========================================================================

FUNCTION SRCHBCH_PROC_SYNC_NSET_SRCH

DESCRIPTION
  This function processes Sync NSET search results from LPQ. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_proc_sync_nset_srch( void );

/*===========================================================================

FUNCTION SRCHBCH_NEXT_SRCH_CMD

DESCRIPTION
  This function first check if there is any step of neighbor search command
  to be issued following an ASET (HPQ) search done. if yes, the functions
  issues the search command to DSP. 

  The priority of neighbor searches is (from highest to lowest): Step1, 
  List, Step2/3. Search sequency will be something like: 
  
  ASET, Step1, ASET, STEP2, ASET, STEP3, ASET, NSET, ASET, STEP2, ASET,
  STEP3, ASET, .....

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_next_srch_cmd (void);

/*===========================================================================

FUNCTION SRCHBCH_ASET_SRCH_DONE

DESCRIPTION
  This function processes HPQ list search Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_aset_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCHBCH_NSET_SRCH_DONE

DESCRIPTION
  This function processes LPQ neighbor list search Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_nset_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCHBCH_STEP1_DONE

DESCRIPTION
  This function processes step_1 Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_step1_done(srchsched_step1_nastt_status_struct_type *step1_nastt_status);
void srchbch_nastt_done (srchsched_step1_nastt_status_struct_type* step1_nastt_status);

/*===========================================================================

FUNCTION SRCHBCH_STEP2_DONE

DESCRIPTION
  This function processes step_2 Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_step2_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCHBCH_STEP3_DONE

DESCRIPTION
  This function processes step_3 Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchbch_step3_done (srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCHBCH_CELL_SELECT_EVAL

DESCRIPTION
  This function evaluates S criteria of serving or target intra frequency 
  neighbor cell if reselection is in progress

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if S criteria passes, FALSE otherwise
  
SIDE EFFECTS
  None

===========================================================================*/
extern boolean srchbch_cell_select_eval( int16 q_qualmin, int16 q_rxlevmin, srch_cell_struct_type* cell_ptr, int16 max_tx_pwr);

/*===========================================================================

FUNCTION SRCHBCH_CELL_SELECT_EVAL_INTER

DESCRIPTION
  This function evaluates S criteria of target inter frequency neighbor cell

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if S criteria passes, FALSE otherwise
  
SIDE EFFECTS
  None

===========================================================================*/
extern boolean srchbch_cell_select_eval_inter( int16 q_qualmin, int16 q_rxlevmin, srch_interf_cell_struct_type* cell_ptr, int16 max_tx_pwr);
#endif /* SRCHBCH_H */
