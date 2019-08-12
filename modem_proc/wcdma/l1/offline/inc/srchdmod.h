#ifndef SRCHDMOD_H
#define SRCHDMOD_H

/*===========================================================================
   FILE: srchdmod.h

GENERAL DESCRIPTION
      This file contains prototypes of function interface to Dmod.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000-2006 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchdmod.h_v   1.8   06 Jun 2002 17:40:14   sidrc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchdmod.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/12/13   jd      Searcher redesign
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   zr      Added structure and prototypes to support attempting ACQ on
                   multiple RLs during reverse HHO
01/20/12   zr      Mainlined FEATURE_WCDMA_SRCHACQ_RETRY 
12/09/08   sv      Mainlined FEATURE_WCDMA_OFFLINE_SEARCHER, FEATURE_RUMI_BRINGUP 
                   and FEATURE_WCDMA_DL_ENHANCED.
10/14/08   sv      Added the "srchdch_stop_dch_measurements" function.
10/26/07   sv      Added searcher support for FEATURE_WCDMA_DL_ENHANCED.
10/16/07   m       Added prototype for function returning SRCH retry count
11/01/06   cpe     Moved dl_acq_pullin_done_cmd_type definition from dlcmd.h to 
                   here. Modified the argument defn for srch_acq_pullin_done()
07/06/06   cpe     Renamed srchdch_start_dch_measurements() as 
                   srchdch_signal_dch_measurements()
01/16/06   gs      Updated RFAPI calls from common header file
04/14/05   cpe     Added an extern for srchdch_start_dch_measurements()
05/03/04   cpe     Redefined SRCH_RF_TX_DELAY to get the value from RF driver.
12/18/03   scm     Move srchacq_rpt() from srchi.h to here.
11/12/03   sh      Added srch_dmod_interf_nbr_cell_ptr for triage to track
                   nbr during cell reselection.
06/04/06   sh      Existing proto for srchacq_pullin_done() only when
                   FEATURE_BARRED_CELL_DURING_CELL_SELECTION not defined.
11/06/02   cpe     Added extern declaration for srch_get_saved_rscp()
06/06/02   src     Added extern declaration corresponding to the new search
                   DCH function srchdch_start_srch_state_machine ().
05/17/02   cpe     Added SRCH_RF_TX_DELAY constant
01/11/01   wjn     added "status" parameter to srch_acq_sttd_done.
01/08/2001 mdl     changed prototype of srch_acq_sttd_done() to have STTD
                   usage as an input parameter rather than the scrambling
                   code.
                   
10/10/00   wjn     Created file.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "srchset.h"
#include "srchinterf.h"
#include "wl1rfmif.h"


/*===========================================================================

                           DATA DECLARATIONS
                     
===========================================================================*/

extern srch_cell_struct_type *srch_dmod_nbr_cell_ptr[SRCHMEAS_MAX_NUM_CARR];
  /* Cell pointer to the Intra frequency neigbor cell whose BCH
     is to be demodulated */

extern srch_interf_cell_struct_type* srch_dmod_interf_nbr_cell_ptr;
  /* Cell pointer to the Inter frequency neigbor cell whose BCH
     is to be demodulated */

/*  In chips. Needed for computing tm  */
#define SRCH_RF_TX_DELAY wl1_rfm_get_enc_btf_val()

/*--------------------------------------------------------------------------
                STRUCTURE: DL_ACQ_PULLIN_DONE_CMD_TYPE

This struture contains the parameters for the DL_ACQ_PULLIN_DONE_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  l1_local_cmd_hdr_type hdr;

  /* whether pull-in passed or failed */
  boolean success;

  /* the (strongest) finger that pull-in was successfully performed with */
  uint8 pullin_fing;

  /* the final rotator value */
  int16 rotator;

} dl_acq_pullin_done_cmd_type;

/* Contains parameters needed to perform an ACQ retry during HHO */
typedef struct
{
  /* Index of the current RL to attempt to acquire. */
  uint8 rl_idx;

  /* Frequency that the scr codes are on */
  uint16 freq;

  /* Size of acq_attempts/scr_code arrays */
  uint8 num_rls;

  /* True if forward HHO, false if reverse */
  boolean forward_hho;

  /* The number of times acquisition has been attempted on each scr code. */
  uint8 acq_attempts;

  /* Array of RLs to attempt to acquire. */
  uint16 scr_code[L1_MAX_RL];
} srchacq_hho_retry_state_type;

/*===========================================================================

                              FUNCTIONS
                     
===========================================================================*/


/*===========================================================================

FUNCTION SRCH_ACQ_PULLIN_DONE

DESCRIPTION
  This function is called by Demod module once pull-in operation is finished.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_acq_pullin_done( dl_acq_pullin_done_cmd_type* cmd );


/*===========================================================================

FUNCTION SRCH_ACQ_STTD_DONE

DESCRIPTION
  This function is called by Demod module once sttd detection operation 
  is finished. The scrambling code passed by function argument will be 
  the same scrambling passed to RRC in CPHY_ACQ_CNF.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_acq_sttd_done (boolean status, boolean sttd_used);

/*===========================================================================
FUNCTION        SRCHDCH_START_SRCH_MACHINE

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  None

SIDE EFFECTS    
===========================================================================*/
extern void srchdch_start_srch_state_machine(uint16 off, uint32 tm);

/*===========================================================================
FUNCTION        SRCHDCH_STOP_DCH_MEASUREMENTS

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
===========================================================================*/
extern void srchdch_stop_dch_measurements(void);

/*===========================================================================
FUNCTION        SRCH_GET_SAVED_RSCP

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
 uint16.

SIDE EFFECTS    
===========================================================================*/
extern uint16 srch_get_saved_rscp(void);

/*===========================================================================

FUNCTION SRCHACQ_RPT

DESCRIPTION
  Reports Acquisition status to CPHY setup procedure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchacq_rpt (boolean acq_status);

/*===========================================================================

FUNCTION SRCHDCH_SIGNAL_DCH_MEASUREMENTS

DESCRIPTION
  Signals to search that measurements be started in DCH state
  in case they aren't already started

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchdch_signal_dch_measurements( void);

/*===========================================================================
  FUNCTION SRCHACQ_RESET_ACQ_RETRY_STATE 

  DESCRIPTION
    This function clears the internal hho retry structure.
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_reset_acq_retry_state(void);

/*===========================================================================
  FUNCTION SRCHACQ_GET_ACQ_ATTEMPT_COUNT 

DESCRIPTION
    This function returns the number of acquisition attempts that have been
    made on the specified RL.

DEPENDENCIES
    None.

RETURN VALUE
    Number of acquisition attempts made on the specified RL.

SIDE EFFECTS
    None.
===========================================================================*/
extern uint8 srchacq_get_acq_attempt_count(void);

/*===========================================================================
  FUNCTION SRCHACQ_REGISTER_ACQ_ATTEMPT

DESCRIPTION
    This function increments the number of acquisition attempts.

DEPENDENCIES
    None.

RETURN VALUE
    Number of acquisition attempts made on current RL.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_register_acq_attempt(void);

/*===========================================================================
  FUNCTION SRCHACQ_SET_CUR_RL_IDX
  
  DESCRIPTION
    This function sets the internal RL index (of srchacq_hho_retry_state.rls).
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_set_cur_rl_idx(uint8 idx);

/*===========================================================================
  FUNCTION SRCHACQ_GET_CUR_RL_IDX

DESCRIPTION
    This function returns the index of the current RL that is attempting to
    be acquired. The index can be used with the srchacq_retry_acq_rls array.

DEPENDENCIES
    None.

RETURN VALUE
    Current RL index.

SIDE EFFECTS
    None.
===========================================================================*/
extern uint8 srchacq_get_cur_rl_idx(void);

/*===========================================================================
  FUNCTION SRCHACQ_GET_WINNING_CELL_IN_HHO

DESCRIPTION
    This function returns the index of the current RL that is attempting to
    be acquired. The index can be used with the srchacq_retry_acq_rls array.

DEPENDENCIES
    None.

RETURN VALUE
    Current RL index.

SIDE EFFECTS
    None.
===========================================================================*/
extern uint16 srchacq_get_winning_cell_in_hho(void);

/*===========================================================================
  FUNCTION SRCHACQ_IS_FORWARD_HHO

DESCRIPTION
    This function returns true if the current HHO state is forward, false
    if reverse.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern boolean srchacq_is_forward_hho(void);

#endif /* !SRCHDMOD_H */
