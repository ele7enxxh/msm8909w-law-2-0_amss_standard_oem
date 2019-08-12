#ifndef RRCIHO_H
#define RRCIHO_H
/*===========================================================================
         R R C   I N T E R - R A T   H A N D O V E R   M O D U L E 

DESCRIPTION

  This module is the header module for the inter-RAT handover module.
  This contains the external interface functions to be used by
  by other RRC modules. This module is only used for dual-mode.
  
Copyright (c) 2001-2003, 2005-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrciho.h#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------\
05/09/16   ad      Made changes to log important parms from OTAs
09/03/14   sp      Made changes to save procedure substates in crash debug info
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
06/24/13   as      Made chagnes to send RRC_CIPHER_SYNC_IND to G whenever new keys are applied 
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
04/19/13   sa      Made changes to update the call type mask for inter system handover from G to W with default config id present 
01/03/13   ad      Made changes to reduce the redundant F3s
10/04/12   pm      Removed unnecessary comments
04/26/12   as      Added code to support feature  NB/WB handover support
03/14/12   gv      Merged changes to not wait for L2 Ack for HO complete and send rab_est_ind
                   to RABM immediately
11/30/11   md      Prototypes of functions rrc_vocoder_control_enabled and rrc_vocoder_acquired
                   have been moved to Rrcmnif.h.
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
02/09/11   su      Added code to free the gtow_info.sib16_ptr before calling 
                   mem_deinit_heap during G2W HO failure to avoid Error Fatal.
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
08/20/10   rm      Fixed compiler warnings
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
08/21/06   tkk     Added SRB#5 support.
07/05/05   kc/vr   New interface defn.to check if W->G IHO is active or not.
03/04/05   kc      Added support for Resume-Failure after a WtoG failure
02/23/05   kc      Interrat HO optimization support
05/27/03   kc      Lint related Fixes.
04/28/03   kc      Prototype to append Activation Time in case of WtoG handover
10/10/02   kc      Added a prototype for returning SIB16 ptr and to send
                   SYNC_IND to MM after HO to UTRAN
10/10/02   xfg     Added a prototype for rrciho_init_procedure
07/29/02   xfg     Created file.


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "rrccmd_v.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* Enumerated type of the different substates for this procedure           */
typedef enum
{
  RRCIHO_INITIAL,                   /* Initial Substate                    */
  RRCIHO_WAIT_STANDBY_CNF,          /* Waiting for mode change comfirmation
                                       from MCM (to Standby)               */
  RRCIHO_WAIT_INTERRAT_HO_CNF,      /* Waiting for inter-RAT Handover 
                                       comfirmation from the GSM RR        */
  RRCIHO_WAIT_ACTIVE_CNF,           /* Waiting for mode change comfirmation
                                       from MCM (to Active)                */
  RRCIHO_WAIT_CHANNEL_CONFIG_CNF,   /* Waiting for revert back to UTRAN
                                       configuration comfirmation from LLC */
  RRCIHO_WAIT_CELL_UPDATE_CNF,      /* Waiting for cell update comfirmation
                                       from CU                             */
  RRCIHO_WAIT_ABORT_CNF,            /* Waiting for abort cnf from RR       */

  
  RRCIHO_WAIT_ACQ_CNF,              /* Waiting for acq cnf from LLC        */
  RRCIHO_WAIT_IRAT_IN_PROGRESS_CNF, /* Waiting for Inter-rat in progress 
                                       Cnf from MCM   */  
  RRCIHO_WAIT_INACTIVE_CNF,         /* Waiting for Inactive Cnf from MCM   */
  RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF, /* Waiting for Ch Config Cnf from LLC
                                       when HO from GSM to WCDMA           */
  RRCIHO_WAIT_CIPH_CONFIG_GTOW_CNF, /* Waiting for Ciph Config Cnf from SMC
                                       when config. Ciph for GtoW HO       */
  RRCIHO_MAX_STATES
} rrciho_state_e_type;
  
/* Below #defines are added for logging purpose only */
#define rrciho_state_e_type_value0 RRCIHO_INITIAL
#define rrciho_state_e_type_value1 RRCIHO_WAIT_STANDBY_CNF
#define rrciho_state_e_type_value2 RRCIHO_WAIT_INTERRAT_HO_CNF
#define rrciho_state_e_type_value3 RRCIHO_WAIT_ACTIVE_CNF
#define rrciho_state_e_type_value4 RRCIHO_WAIT_CHANNEL_CONFIG_CNF
#define rrciho_state_e_type_value5 RRCIHO_WAIT_CELL_UPDATE_CNF
#define rrciho_state_e_type_value6 RRCIHO_WAIT_ABORT_CNF
#define rrciho_state_e_type_value7 RRCIHO_WAIT_ACQ_CNF
#define rrciho_state_e_type_value8 RRCIHO_WAIT_IRAT_IN_PROGRESS_CNF
#define rrciho_state_e_type_value9 RRCIHO_WAIT_INACTIVE_CNF
#define rrciho_state_e_type_value10 RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF
#define rrciho_state_e_type_value11 RRCIHO_WAIT_CIPH_CONFIG_GTOW_CNF
#define rrciho_state_e_type_value12 RRCIHO_MAX_STATES

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

FUNCTION RRCIHO_INIT_PROCEDURE


DESCRIPTION

  This function initializes the Inter-RAT handover module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrciho_init_procedure(void);

/*===========================================================================

FUNCTION RRCIHO_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Inter-RAT Handover module. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrciho_event_handler(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================

FUNCTION   rrciho_get_sib16_ptr

DESCRIPTION

  This function gets the Sib16 ptr either from the actual sib16 or the
  SIB16 version of Default Configuration

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_SysInfoType16 * rrciho_get_sib16_ptr(void);

/*===========================================================================

FUNCTION   rrciho_get_rbs_ptr

DESCRIPTION

  This function gets the RBS ptr either from the actual RBS or the
  RBS version of Default Configuration

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
void * rrciho_get_rbs_ptr(uint32 *df_config_id);

/*===========================================================================

FUNCTION   rrciho_send_sync_ind_for_gtow_ho

DESCRIPTION
  Sends SYNC_IND to MM after a successful Activation Response has been
  received for transitioning to WCDMA
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/  
void rrciho_send_sync_ind_for_gtow_ho(void);

/*===========================================================================

FUNCTION   rrciho_get_activation_time_for_wtog_ho

DESCRIPTION

  This function returns the activation time for WtoG handover and also returns
  status Success/Failure if Activation Time was added or not.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  Success/Failure depending on whether activation time was appended or not

SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrciho_get_activation_time_for_wtog_ho (uint16 * act_time);

#ifdef FEATURE_INTER_RAT_HO_OPT
/*===========================================================================
FUNCTION   rrciho_cs_voice_rab_released()

DESCRIPTION

  This is to inform IHO procedure that a cs voice rab has been released
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.
===========================================================================*/
void rrciho_cs_voice_rab_released(void);

#endif /* FEATURE_INTER_RAT_HO_OPT */

/*===========================================================================

FUNCTION RRCIHO_IS_WTOG_HO_ACTIVE


DESCRIPTION

  API to indicate if WTOG HO is active and ongoing.  Used by other procedures
  to query if WTOG HO is ongoing
  
DEPENDENCIES

  None

RETURN VALUE

TRUE: If WTOG HO is active
FALSE: IF WTOG is not active

SIDE EFFECTS

  None
  
===========================================================================*/
boolean rrciho_is_wtog_ho_active
(
 void
);

/*===========================================================================

FUNCTION   rrciho_initialize_srb_list_r8

DESCRIPTION

  Initializes GTOW information for Signalling Radio Bearer list


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_initialize_srb_list_r8
(
  rrc_SRB_InformationSetupList_r8 *srb_ptr
);

/*===========================================================================

FUNCTION   rrciho_initialize_rb_list

DESCRIPTION

  Initializes GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_initialize_rb_list_r8
(
  rrc_RB_InformationSetupList_r8 *rb_ptr
#ifdef FEATURE_LTE_TO_WCDMA
, uint8 index
#endif
);

/*===========================================================================

FUNCTION   rrciho_update_rab_info_r8

DESCRIPTION

  updates GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_RAB_InformationSetupList_r8 *  rrciho_update_rab_info_r8
(
    uint32 * rab_identity_choice
#ifdef FEATURE_WCDMA_REL9
    ,rrc_msg_ver_enum_type release
#endif /* FEATURE_WCDMA_REL9 */

);


/*===========================================================================

FUNCTION   is_iho_w2g_active

DESCRIPTION

 Returns whether IRAT W2G is active or not

DEPENDENCIES

  None.

RETURN VALUE

  boolean

SIDE EFFECTS

  None.

===========================================================================*/
boolean is_iho_w2g_active
(
  void
);


/*===========================================================================

FUNCTION   rrciho_free_sib16

DESCRIPTION

 Frees the ptr to sysInfoType16 previously allocated.

DEPENDENCIES

  None.

RETURN VALUE

   NONE


SIDE EFFECTS

  None.

===========================================================================*/

void rrciho_free_sib16
(
  void
);


/*===========================================================================

FUNCTION rrciho_is_gtow_ho_active

DESCRIPTION 
    This function returns status of GTOW HO.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
boolean rrciho_is_gtow_ho_active(void);
#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
/*===========================================================================

FUNCTION rrciho_check_and_handle_mdsp_halt_during_gtow_handover

DESCRIPTION 
    This funcrion takes care of mDSP halt recovery when GTOW HO
    is in progress.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
boolean rrciho_check_and_handle_mdsp_halt_during_gtow_handover(void);

/*===========================================================================

FUNCTION rrciho_check_and_handle_mdsp_halt_during_wtog_handover

DESCRIPTION 
    This function checks if WTOG HO is in progress, and then takes
    care of mDSP halt recovery.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
boolean rrciho_check_and_handle_mdsp_halt_during_wtog_handover(void);
#endif
/*===========================================================================

FUNCTION   rrciho_is_cs_data_rab_present

DESCRIPTION
  This function indicates whether any voice RAB is present or not in gtow est_rabs database.

      
DEPENDENCIES

  None.

RETURN VALUE

  TRUE: if CS DATA RAB exist in gtow est_rabs database
  FALSE: if CS DATA RAB doesnt exist gtow in est_rabs database


SIDE EFFECTS

  None.

===========================================================================*/  
boolean rrciho_is_cs_data_rab_present(void);
/*===========================================================================

FUNCTION rrciho_is_dl_ps_zero_zero_size_configured_for_all_rabs

DESCRIPTION

  This function fills in the transport block size per PS RAB

  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE if PS RAB id doesn't find the size.  Otherwise returns SUCCESS.
 

SIDE EFFECTS

  None

===========================================================================*/
rrc_ps_data_rate_e_type rrciho_is_dl_ps_zero_zero_size_configured_for_all_rabs
(
void
);
/*===========================================================================

FUNCTION   rrciho_send_cipher_sync_ind

DESCRIPTION

  This function sends indication to RR that the new keys are activated

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_send_cipher_sync_ind(void);
/*===========================================================================
FUNCTION rrciho_get_current_substate()

DESCRIPTION
  This function will return current RRC IHO
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current iho sub state.
SIDE EFFECTS
none
===========================================================================*/
uint8  rrciho_get_current_substate(void);
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
boolean rrciho_return_interrat_type(void);

/*===========================================================================
FUNCTION   rrciho_return_iho_abort

DESCRIPTION
returns iho abort

DEPENDENCIES
  None.

RETURN VALUE
uint8

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrciho_return_iho_abort(void);


/*===========================================================================
FUNCTION   rrciho_return_iho_substate

DESCRIPTION
returns iho substate

DEPENDENCIES
  None.

RETURN VALUE
rrciho_state_e_type


SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrciho_return_iho_substate(void);
/*===========================================================================
FUNCTION   rrciho_return_default_config_id

DESCRIPTION
returns default config id

DEPENDENCIES
  None.

RETURN VALUE
uint32

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint32 rrciho_return_default_config_id(void);

/*===========================================================================
FUNCTION   rrciho_return_config_type

DESCRIPTION
returns config type

DEPENDENCIES
  None.

RETURN VALUE
rrciho_gtow_config_e_type

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrciho_return_config_type(void);
/*===========================================================================
FUNCTION   rrc_qsh_log_ho_from_utran_fail_params

DESCRIPTION
returns config type

DEPENDENCIES
  None.

RETURN VALUE
rrciho_gtow_config_e_type

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_qsh_log_ho_from_utran_fail_params(uint32 fail_cause);

/*===========================================================================
FUNCTION   rrc_qsh_log_ho_to_gsm_params

DESCRIPTION
returns config type

DEPENDENCIES
  None.

RETURN VALUE
rrciho_gtow_config_e_type

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_qsh_log_ho_to_gsm_params(rrc_log_rrc_ho_gsm_ota *ho_gsm_params);
#endif /* RRCIHO_H */

