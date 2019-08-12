#ifndef RRCPCRC_H
#define RRCPCRC_H
/*===========================================================================
         R R C   R A D I O   B E A R E R   R E L E A S E   M O D U L E 

DESCRIPTION

  This module is the header module for the Radio Bearer Release module.
  This contains the external interface functions to be used by
  by other RRC modules. This must not be used by modules outside RRC.
  
Copyright (c) 2001-2004, 2006-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcpcreconfig.h_v   1.1   06 Jun 2002 20:43:28   upuntamb  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcpcreconfig.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/15   as      Added code to support LTA durting activation time
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
09/29/13   db      Made changes to update failure status of procedure upon registration with CU and OC reset
07/19/13   db      Made changes to Swap OC and CC after channel config by CU when
                   a procedure is registered and it fails
05/08/09   ss      Updated Copyright Information
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   da      Added prototype for rrcpcrc_get_new_crnti_valid()
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
08/09/04   kc      Interface change for CU-build-rrcpcreconfig msg to check fo
                   RB-Re-est of TM-bearers when transitioning from non-DCH state 
                   to DCH state.
07/14/04   sk      Added prototype for function to get the current substate
02/03/04   sk      SRNS relocation support in rbr complete message triggered by 
                   CU proc. Added a new field.
08/28/02   vk      Defined prototype for fn rrcpcrc_dch_compressed_mode_handler(..)
                   to send a physical channel reconfiguration failure
                   message to UTRAN and to post an internal command to 
                   measurement procedure to indicate the deleted measurement
                   identity. Changes have been put under FEATURE CM SUPPORTED
06/06/02   upn     Added prototype for rrcrbr_build_pcr_complete_message
05/10/01   ram     Created file.


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
  
/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION  RRCPCRC_INIT_PROCEDURE

DESCRIPTION

  This function initializes the RBR procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcpcrc_init_procedure( void );

#ifdef FEATURE_UMTS_PDCP
/*===========================================================================

FUNCTION   RRCPCRC_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcpcrc_is_srns_in_progress
(
  void 
);
#endif
/*===========================================================================

FUNCTION   RRCPCRC_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcpcrc_get_new_crnti_valid
(
  void
);
/*===========================================================================

FUNCTION   RRCPCRC_GET_CURRENT_SUB_STATE()

DESCRIPTION
 This function is used to return current substate


        
DEPENDENCIES

  None.
 
RETURN VALUE

  rrcrb_substate_e_type current_substate.

SIDE EFFECTS

  None.

===========================================================================*/\
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrcrb_substate_e_type rrcpcrc_get_current_sub_state
(
   void
);
 /*===========================================================================

FUNCTION   rrcpcrc_get_dest_state()

DESCRIPTION
 This function is used to return destination state
 indicated in OTA.
DEPENDENCIES

  None.

RETURN VALUE

  rrc_state_e_type.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_state_e_type rrcpcrc_get_dest_state(void);
/*===========================================================================

FUNCTION   rrcpcrc_is_proc_successful()

DESCRIPTION
 This function is used to return failure status

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcpcrc_is_proc_successful
(
void
);


/*===========================================================================

FUNCTION   rrcpcrc_update_failure_status()

DESCRIPTION
  Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rrcpcrc_update_failure_status
(
  rrcpc_msg_status_e_type status
);



/*===========================================================================

FUNCTION rrcpcr_build_pcr_complete_message

DESCRIPTION
  This function builds the PCR Complete message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcpcr_build_pcr_complete_message
(    
  rrc_RRC_TransactionIdentifier trans_id,        /* Transaction Id */
  rrc_UL_DCCH_Message *msg_ptr,                  /* Pointer to Uplink DCCH Msg */
  boolean              srns_reloc,               /* Indicates whether SRNS reloc is involved */ 
  boolean              cipher_update_required    /* Indicates whether TM ciphering is present */
);




/*===========================================================================

FUNCTION RRCPCRC_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Radio Bearer Release procedure. This function is called by the
  dispatcher whenever an event is received for this module. This function
  checks the current RRC state and calls the event handler for
  that state. 
  
  Note that the Radio Bearer Release procedure can be initiated
  only in the CELL_FACH and CELL_DCH states. However, while the
  procedure is active, the RRC state may change to any of the other
  states. Hence all RRC states are checked by this module.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrcpcrc_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);


/*===========================================================================

FUNCTION  RRCPCRC_DCH_COMPRESSED_MODE_HANDLER

DESCRIPTION

  This function sends a physical channle reconfiguration failure
  message to UTRAN to indicate a compressed mode run time error
  indication and also posts an internal command to Measurements
  procedure to indicate the deleted Measurement Identity
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcpcrc_dch_compressed_mode_handler
(
  l1_compressed_mode_ind_type *cm_ptr
);

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrcpcrc_set_failure_cause

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
void rrcpcrc_set_failure_cause(rrcrb_msg_status_e_type cause);

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

#endif /* RRCPCRC_H */

