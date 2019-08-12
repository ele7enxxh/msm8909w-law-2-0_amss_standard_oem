#ifndef TDSSRCH_CMD_H
#define TDSSRCH_CMD_H

/*===========================================================================

                         Search Command Handlers

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdssrchcmd.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/14/11   cxd     Remove the following L1 external commands
                   TDSL1_CPHY_IMMEDIATE_RESELECTION_REQ=   0x15,
                   TDSL1_CPHY_CELL_RESELECTION_RSP     =   0x16,
11/17/10    sq      Initial draft
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "tdsl1rrcif.h"
#include "tdsl1def.h"
#include "tdssrch.h"
#include "tdssrchset.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                        SEARCH DONE COMMAND

This command must be sent as a parameter of funtion "l1_srch_proc_srch_done".
--------------------------------------------------------------------------*/

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
    /* LPQ or HPQ */
  boolean blind_decoding;
  /* flag that indicates whether UE is going to do blind decoding next or not */
}
tdssrchcmd_done_cmd_type;


/*--------------------------------------------------------------------------
                      SEARCH OPERATION COMMAND

This command must be sent as a parameter of funtion "l1_srch_proc_op_cmd".
--------------------------------------------------------------------------*/

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
    /* generic command header */
  tdssrch_ops_code_struct_type   ops_code;
}
tdssrchcmd_ops_cmd_type;


/*===========================================================================

FUNCTION tdssrch_proc_srch_ops

DESCRIPTION
  This function processes "Search Operation Command" from local command queue. 
  A search operation command is sent to local queue when "srch_heartbeat_isr()
  detect that a search operation is due. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tdssrch_proc_srch_ops
( 
  tdssrchcmd_ops_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION        SRCH_FS_REQ_CMD

DESCRIPTION
  This function performs frequency scan requested by RRC.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void tdssrchfs_req_cmd (tdsl1_freq_scan_cmd_type *cmd);



/*===========================================================================

FUNCTION      tdssrchacq_step1_srch_cmd

DESCRIPTION
  This function schedules a step1 search in ACQ state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tdssrchacq_step1_srch_cmd(uint8 freq_idx);

/*===========================================================================

FUNCTION tdssrch_cell_parm_update

DESCRIPTION
  This function update search control parameters based on information conveyed 
  by TDSL1_CPHY_CELL_PARM_REQ. 

  The function implementation is in srch.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tdssrch_cell_parm_update ( tdsl1_cell_parm_cmd_type *cmd_ptr );



/*===========================================================================

FUNCTION tdssrchmeas_parm_update  

DESCRIPTION
  This function updates measurement control parmmeters based on 
  TDSL1_CPHY_MEASUREMENT_REQ. 
  
  The function implementation is in srch.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean tdssrchmeas_parm_update
( 
  tdsl1_meas_cmd_type *cmd_ptr
);



/*===========================================================================

FUNCTION SRCH_CELL_SELECT_EVAL 

DESCRIPTION
  This function processes TDSL1_CPHY_CELL_SELECTION_REQ from RRC, and starts cell 
  selection searches on the current WCDMA frequency. 
  
  The function impleentation is in srhbch.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tdssrchbch_cell_selection_req
( 
  tdsl1_cell_selec_cmd_type *cmd_ptr
);

#ifdef FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING
/*===========================================================================

FUNCTION        SRCHFACH_PROCESS_IMMEDIATE_INTERF_MEAS_REQ

DESCRIPTION     .

DEPENDENCIES    NONE

RETURN VALUE    NONE
  
SIDE EFFECTS    Update related variables

===========================================================================*/
extern void tdssrchfach_process_immediate_interf_meas_req(tdsl1_immediate_meas_type_tmp *meas);
#endif


/*===========================================================================

FUNCTION SRCHCR_CELL_BAR_REQ

DESCRIPTION
  This function processes cell bar req from RRC
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tdssrchcr_cell_bar_req 
( 
   tdsl1_cell_bar_cmd_type *cmd
);

/*===========================================================================

FUNCTION SRCHCR_CHECK_ACQ_CELL_BARRED

DESCRIPTION
  This function prevent acquistion on a barred cell
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if cell is barred.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean tdssrchcr_check_acq_cell_barred 
(
   tdsl1_acq_cmd_type *cmd
);

/*===========================================================================

FUNCTION SRCHCR_CHECK_CELL_BARRED

DESCRIPTION
  This function prevent check whether a tdd cell is barred.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if cell is barred.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tdssrchcr_check_cell_barred
(
  uint16 freq,
  uint16 scr_code
);
  

/*===========================================================================

FUNCTION      tdssrchacq_req_cmd

DESCRIPTION
  This function starts acquisiton procedure.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdssrchacq_req_cmd(tdsl1_acq_cmd_type *cmd);

#endif
