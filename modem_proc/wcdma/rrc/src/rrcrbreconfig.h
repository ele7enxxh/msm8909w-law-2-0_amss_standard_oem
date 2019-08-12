#ifndef RRCRBRRC_H
#define RRCRBRRC_H
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

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrbreconfig.h_v   1.1   06 Jun 2002 20:43:16   upuntamb  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrbreconfig.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/15   as      Added code to support LTA durting activation time
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
09/29/13   db      Made changes to update failure status of procedure upon registration with CU and OC reset
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
07/19/13   db      Made changes to Swap OC and CC after channel config by CU when
                   a procedure is registered and it fails
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/14/09   kp      Made changes send RAB_EST_IND only if PS doamin is open.
05/08/09   ss      Updated Copyright Information
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   da      Added prototype for rrcrbrc_get_new_crnti_valid()
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
08/09/04   kc      Interface change for CU-build-rbreconfig msg to check fo
                   RB-Re-est of TM-bearers when transitioning from non-DCH state 
                   to DCH state.
07/14/04   sk      Added prototype for function to get the current substate
02/03/04   sk      SRNS relocation support in rbreconfig complete message triggered by 
                   CU proc. Added a new field.
06/06/02   upn     Added prototype for rrcrbr_build_rbrc_complete_message
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
#include "rrcdt.h"

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

FUNCTION  RRCRBRC_INIT_PROCEDURE

DESCRIPTION

  This function initializes the RBR procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrbrc_init_procedure( void );

#ifdef FEATURE_UMTS_PDCP
/*===========================================================================

FUNCTION   RRCRBRC_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbrc_is_srns_in_progress
(
  void 
);
#endif
/*===========================================================================

FUNCTION   RRCRBRC_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbrc_get_new_crnti_valid
(
  void
);

/*===========================================================================

FUNCTION   RRCRBRC_GET_CURRENT_SUB_STATE()

DESCRIPTION
 This function is used to return current substate


        
DEPENDENCIES

  None.
 
RETURN VALUE

  rrcrb_substate_e_type current_substate.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrcrb_substate_e_type rrcrbrc_get_current_sub_state
(
   void
) ;
/*===========================================================================

FUNCTION   rrcrbrc_get_dest_state()

DESCRIPTION
 This function is used to return destination state



DEPENDENCIES

  None.

RETURN VALUE

  rrc_state_e_type

SIDE EFFECTS

  None.

===========================================================================*/
rrc_state_e_type rrcrbrc_get_dest_state(void);
/*===========================================================================

FUNCTION   rrcrbrc_is_proc_successful()

DESCRIPTION
 This function is used to return failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcrbrc_is_proc_successful
(
  void
);

/*===========================================================================

FUNCTION   rrcrbrc_update_failure_status()

DESCRIPTION
  Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
void rrcrbrc_update_failure_status
(
  rrcrb_msg_status_e_type status
);

/*===========================================================================

FUNCTION rrcrbrc_build_rbrc_complete_message

DESCRIPTION
  This function builds the RBRC Complete Message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcrbrc_build_rbrc_complete_message
(    
  rrc_RRC_TransactionIdentifier trans_id,        /* Transaction Id */
  rrc_UL_DCCH_Message *msg_ptr,                  /* Pointer to Uplink DCCH Msg */
  boolean              srns_reloc,               /* Indicates whether SRNS reloc is involved */ 
  boolean              cipher_update_required    /* Indicates whether TM ciphering is present */
);





/*===========================================================================

FUNCTION RRCRBRC_PROCEDURE_EVENT_HANDLER


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
void rrcrbrc_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);


#ifdef FEATURE_WCDMA_REL9


/*===========================================================================
FUNCTION   rrcrbrc_process_after_oc_config_set_r9

DESCRIPTION
  This function processes the R9 RBRC OTA after OC is set by this OTA. So if we call this function, it means
  that ORDERED_CONFIG_SET is returned while setting OC by RBRC OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcrbrc_process_after_oc_config_set_r9
(
  rrc_DL_DCCH_Message * dcch_msg_ptr, 
  rrc_RadioBearerReconfiguration_r9_IEs  *rbrc_ptr
);

#endif /*FEATURE_WCDMA_REL9*/

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif  /*FEATURE_WCDMA_REL10*/

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrcrbrc_set_failure_cause

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
void rrcrbrc_set_failure_cause(rrcrb_msg_status_e_type cause);

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

#endif /* RRCRBRRC_H */

