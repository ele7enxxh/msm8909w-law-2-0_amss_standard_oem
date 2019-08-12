#ifndef SRCHHHO_H
#define SRCHHHO_H
/*===========================================================================
FILE: srchhho.h

GENERAL DESCRIPTION:
      This file contains implementation of external functions and utility
      functions shared by search routines at various states.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright(c) 2000-2010 by Qualcomm Technologies, Inc. All Rights Reserved.
        
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchhho.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/14   jd      Reset the window size of cell to recovery after hho failure.
01/07/14   amj     Cleaned up old ASET update code.
10/28/13   km      Revert HHO logic to pre 514621
09/12/13   jd      Searcher redesign 
07/19/13   km      Do not assume TxD to be ON during Interf HHO
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   zr      Added support for attempting multiple RLs during reverse HHO
06/11/12   vs      MBMS feature cleanup.
05/04/12   amj     Changed function prototype to include whether STTD info is available or not
01/04/12   zr      Added function definitions to access and modify intraf
                   HHO timing info known flag
03/17/10   sv      Changed some API names.
03/03/09   sv      Externed the "srch_retry_interf_redirection_acq" function.
02/27/09   sv      Cleaned up some code.
10/03/08   hk      Bringing in 7k Mailine fixes
08/02/05   m       Moved a prototype to srchset.h as function also moved
06/02/05   m       Modified few prototypes to match the change in c files
05/13/05   m       Modified prototype for the few functions
05/03/05   m       Fixed bugs reported in integration of srch optimization
                   feature and resume on HHO failure
04/13/05   m       Added more support for srch optimization for HHO
03/28/05   m       Created file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "l1rrcif.h"
#include "srchset.h"
#include "srchcmd.h"
#include "srchdmod.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================
                          FUNCTION PROTOTYPES
                          
The followings are prototypes of functions exported to external modules.
===========================================================================*/

/*===========================================================================

FUNCTION SRCH_START_HHO_ACQ_SEARCH

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srch_start_hho_acq_search(l1_acq_cmd_type *cmd);

/*===========================================================================
FUNCTION SRCH_RESET_CELL_WINSIZE_ON_HHO_FAILURE

DESCRIPTION
    This function sets the window size of the cell that failed hho pn to recovery.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
 
===========================================================================*/
extern void srch_reset_cell_winsize_on_hho_failure( void );

/*===========================================================================

FUNCTION SRCH_PROCESS_HHO_STEP1_RESULT

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srch_process_hho_acq_step1_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCH_PROCESS_HHO_LIST_SRCH_RESULT

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean srch_process_hho_acq_list_srch(void);

/*===========================================================================

FUNCTION SRCH_PROCESS_HHO_LIST_SRCH_PEAKS

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srch_process_hho_dch_list_srch(void);
  
/*===========================================================================

FUNCTION SRCH_HHO_SRCH_FAILED

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srch_hho_srch_failed(void);
  
/*===========================================================================

FUNCTION SRCH_CLEAR_HHO_FLAGS

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srch_clear_hho_flags(void);


/*===========================================================================
FUNCTION SRCH_IS_HHO_SRCH_IN_DCH

DESCRIPTION   API used to indicate whether a SET_A_NO_PN search is
              ongoing in DCH  

DEPENDENCIES

RETURN VALUE 
        TRUE: if SET_A_NO_PN search ongoing in DCH
        FALSE: otherwise. 

SIDE EFFECTS
===========================================================================*/
extern boolean srch_is_hho_srch_in_dch( void );

/*===========================================================================
FUNCTION SRCH_PROGRAM_HHO_FULL_WINDOW_PN_SRCH

DESCRIPTION
  Programs a full window search when retrying searches during HHO.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Kicks off full window PN search in ACQ mode
===========================================================================*/
extern void srch_program_hho_full_window_pn_srch(uint16 scr_code, uint8 sttd_val);

/*===========================================================================
FUNCTION SRCH_HHO_SET_INTRAF_TIMING_KNOWN_HHO_PROG

DESCRIPTION   Function to set the flag TRUE or FALSE depending upon whether
              search is going in HHO during DCH state. 

DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/
extern void srch_hho_set_intraf_timing_known_hho_prog(boolean in_prog);

/*===========================================================================
FUNCTION SRCH_HHO_INTRAF_TIMING_KNOWN_HHO_PROG

DESCRIPTION   Function to get the current state of the hho intraf timing
              known flag.

DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/
extern boolean srch_hho_intraf_timing_known_hho_prog(void);

/*===========================================================================
  FUNCTION SRCHHHO_ACQ_FIND_NEXT_CELL

DESCRIPTION
    This function determines the next RL to attempt to acquire, prioritizing   
    RLs with known timing information first. The function tracks the number   
    of times acquisition has been attempted on each RL, and will not allow   
    more than SRCHACQ_MAX_ATTEMPT_CNT attempts to be made.

DEPENDENCIES
    None.

RETURN VALUE
    A pointer to the RL to use, or NULL if there are no suitable RLs   
    available.

SIDE EFFECTS
  Resets ASET and HSET
===========================================================================*/
extern srch_cell_struct_type* srchhho_acq_find_next_cell(void);

/*===========================================================================
FUNCTION       
    SRCHHHO_ATTEMPT_TIMING_UNKNOWN_HHO

DESCRIPTION
    This function attempts to perform HHO to a PSC. It should only be called   
    in forward HHO after all the cells with known timing info have been   
    exhausted.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    Performs full window PN search.
===========================================================================*/
extern void srchhho_attempt_timing_unknown_hho(void);

#endif
