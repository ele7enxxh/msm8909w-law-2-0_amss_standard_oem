#ifndef RRCRBE_H
#define RRCRBE_H
/*===========================================================================
  R R C   R A D I O   B E A R E R   E S T A B L I S H M E N T   M O D U L E 

DESCRIPTION

  This module is the header module for the Radio Bearer Establishment module.
  This contains the external interface functions to be used by
  by other RRC modules. This must not be used by modules outside RRC.
  
Copyright (c) 2000-2004, 2006-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrbe.h_v   1.2   06 Jun 2002 20:43:32   upuntamb  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrbe.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   sp      Made changes to log serving cell QSH metrics at metric config 
05/09/16   ad      Made changes to log important parms from OTAs
10/16/15   as      Added code to support LTA durting activation time
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
05/30/14  sa       Made changes to support 64x64 single PS RAB detection
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
11/29/13   sp      Made changes to calculate the count c activation time for a fach 
                   to dch after moving to DCH state
09/29/13   db      Made changes to update failure status of procedure upon registration with CU and OC reset
07/19/13   db      Made changes to Swap OC and CC after channel config by CU when
                   a procedure is registered and it fails
06/27/13   sa      Added changes to support call type mask for AMR with PS0x0 config
11/22/12   sa      Made changes to update the call type mask for CS data
10/19/12   sa      Made the changes to update call type mask for cs voice and cs data
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
05/08/09   ss      Updated Copyright Information
05/23/07   ps      Added support for "RB Info to Change" IE in RB-Setup msg.
                   Change are made under feature flag FEATURE_DEFAULT_CONFIG_IN_RB_SETUP.
04/10/07   ps      Added support for default configs sent through Rb-Setup msg
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   da      Added prototype for rrcrbe_get_new_crnti_valid()
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
07/14/04   sk      Added prototype for function to get the current substate
09/27/02   kc      Corrected typo in FEATURE defn. for CIPHERING
09/04/02   kc      Added interface fn. to get established rabs during RB 
                   establishment
06/06/02   upen    Removed reconfig complete message prototype functions.
03/08/02   bu      Added prototypes for functions rrcpcr_build_pcr_complete_message,
                   rrctcr_build_tcr_complete_message and
                   rrcrbrc_build_rbrc_complete_message.
04/06/01   ram     Created file.


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
  #include "rrcrbcommon.h"


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

FUNCTION  RRCRBE_INIT_PROCEDURE

DESCRIPTION

  This function initializes the RBE procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrbe_init_procedure( void );

#ifdef FEATURE_UMTS_PDCP
/*===========================================================================

FUNCTION   RRCRBE_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbe_is_srns_in_progress
(
  void 
);
#endif
/*===========================================================================

FUNCTION   RRCRBE_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbe_get_new_crnti_valid
(
  void
);
/*===========================================================================

FUNCTION   RRCRBE_GET_CURRENT_SUB_STATE()

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
rrcrb_substate_e_type rrcrbe_get_current_sub_state
(
   void
);
/*===========================================================================

FUNCTION   rrcrbe_get_dest_state()

DESCRIPTION
 This function is used to return dest state
DEPENDENCIES
  None.
RETURN VALUE
  rrc_state_e_type .

SIDE EFFECTS

  None.

===========================================================================*/
rrc_state_e_type rrcrbe_get_dest_state(void);
/*===========================================================================

FUNCTION   rrcrbe_is_proc_successful()

DESCRIPTION
 This function is used to return failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcrbe_is_proc_successful
(
  void
);

/*===========================================================================

FUNCTION   rrcrbe_update_failure_status()

DESCRIPTION
 Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
void rrcrbe_update_failure_status
(
  rrcrb_msg_status_e_type status
);


/*===========================================================================

FUNCTION rrcrbe_get_est_rab_ptr

DESCRIPTION

  This function returns ptr to established rabs to requesting
  procedures
    
DEPENDENCIES

  None.
 
RETURN VALUE

  Pointer to rab info

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_info_per_msg_type *rrcrbe_get_est_rab_ptr(void);
/*===========================================================================

FUNCTION RRCRBE_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Radio Bearer Establishment procedure. This function is called by the
  dispatcher whenever an event is received for this module. This function
  checks the current RRC state and calls the event handler for
  that state. 
  
  Note that the Radio Bearer Establishment procedure can be initiated
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
void rrcrbe_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================
FUNCTION   rrcrbe_validate_rbs_in_given_rab

DESCRIPTION

DEPENDENCIES
  YTBD
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcrbe_validate_rbs_in_given_rab
(
  rrc_RB_InformationSetupList_r7 *rb_setup_list,
  uint8 rab_idx
);

/*===========================================================================
FUNCTION   rrcrbe_prevalidate_rab_setup_list_r7

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rrcrbe_prevalidate_rab_setup_list_r7
(
  rrc_RAB_InformationSetupList_r7 *rab_list_ptr,
  boolean *new_tm_rb
);


/*===========================================================================
FUNCTION   rrcrbe_validate_rab_setup_list_r7

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rrcrbe_validate_rab_setup_list_r7
(
  rrc_RAB_InformationSetupList_r7 * rab_list_ptr,
  uint32 no_of_rabs
 #ifdef FEATURE_VOC_AMR_WB
  ,mvs_mode_type nas_sync_ind
 #endif /*FEATURE_VOC_AMR_WB*/  
);

/*===========================================================================

FUNCTION   RRCRBE_SEND_INDICATION_TO_UPPER_LAYERS

DESCRIPTION
  This function sends RABMAS_RAB_ESTABLISH_IND primitive
  to the RABM layer of NAS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void rrcrbe_send_rabm_est_ind_to_upper_layers
(
  void
);
/*===========================================================================

FUNCTION   rrcrbe_is_cs_data_rab_present

DESCRIPTION

  This function indicates whether any voice RAB is present or not in est_rabs database.
  This function is going to be used by MAC/RRC to verify whether Voice RAB is present 
  or not.
  This function has been modified such that it returns TRUE only if CS_VOICE_RAB is of DCH type.        
DEPENDENCIES

None

RETURN VALUE

  TRUE: if CS DATA RAB exist in est_rabs database
  FALSE: if CS DATA RAB doesnt exist in est_rabs database

SIDE EFFECTS

  None.

===========================================================================*/

boolean rrcrbe_is_cs_data_rab_present (void);
/*===========================================================================

FUNCTION rrcrbe_is_dl_ps_zero_zero_size_configured_for_all_rabs

DESCRIPTION

  This function fills in the transport block size per PS RAB

  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE if PS RAB id doesn't find the size.  Otherwise returns SUCCESS.
 

SIDE EFFECTS

  None

===========================================================================*/
rrc_ps_data_rate_e_type rrcrbe_is_dl_ps_zero_zero_size_configured_for_all_rabs
(
void
);

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrcrbe_set_failure_cause

DESCRIPTION


  
DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
void rrcrbe_set_failure_cause(rrcrb_msg_status_e_type cause);

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

/*===========================================================================
FUNCTION rrc_l1_query_psrab_data_64_64_is_present

DESCRIPTION

  This function fills in the transport block size per PS RAB

  
DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
l1_rrc_config_rab_enum_type rrc_l1_query_psrab_data_64_64_is_present(void);
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void rrc_get_mapping_combo_rb(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr);
/*===========================================================================
FUNCTION rrc_get_mapping_combo_srb
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void rrc_get_mapping_combo_srb(rrc_overall_ota_info_s* overall_ota_info_ptr);

/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
uint8 rrc_get_mapping_combo(uint8 rb_id);
/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_get_one_sided_re_est_flag(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr);

/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrcqsh_get_num_rbs_added(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr);

/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_get_rlc_size_type(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr);
/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

uint8 rrbrbe_srns_relocation(void);

#endif
#endif /* RRCRBE_H */

