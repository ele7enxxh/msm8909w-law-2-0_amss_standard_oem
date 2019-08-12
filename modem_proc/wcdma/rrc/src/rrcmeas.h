#ifndef RRCMEAS_H
#define RRCMEAS_H
/*===========================================================================
          R R C   M E A S U R E M E N T S   H E A D E R   F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  Measurement Control and Report Procedure.
  
Copyright (c) 2000-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcmeas.h_v   1.7   14 Feb 2002 13:55:20   vkhanna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcmeas.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/14   sp    Made changes for Mav debug snapshot
09/03/14   sp      Made changes to save procedure substates in crash debug info
03/13/14   sn      CSG feature changes
11/29/12   sn    Backed out changes to send hcs_used as FALSE to L1 if SIB19 has valid priority info.
08/31/12   rd    Transfer dedicated priority parameters to/from WCDMA and GERAN
                 during IRAT procedure
01/23/12   sn    Changes for E-UTRA detection
12/05/11   pm    Added code to support cell id request by CM in DCH state"
06/21/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.100.50
01/28/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl    Objective systems migration changes check in 
                 on code base VU_MODEM_WCDMA_RRC.01.80.00
09/09/10   rm    Made changes to hold dedicated priorities during OOS
06/29/10   rm    Added code to support LTE<->WCDMA Inter RAT
06/29/10   rm    Made changes for ZI memory optimization
05/08/09   ss    Updated Copyright Information
02/13/08   rm    Added protype for rrctvm_is_traffic_volume_indicator_to_be_set_in_cu
                 under FEATURE_TVM_IND_IN_CU
12/27/07   rm    Added the prototype for rrcifreq_free_memory_allocated_for_cell_list_in_meas_ind
08/22/06   sgk   Modifed the prototype rrcmeas_save_intra_freq_meas to 
                 rrcmeas_save_intra_inter_freq_meas. 
08/05/05   sgk   Modified prototype of func rrcmeas_append_meas_rpt_on_rach()
06/23/05   sgk   Added #define MAX_QHCS_RSCP 26
01/10/05   sgk   Modified prototype rrcmeas_append_meas_rpt_on_rach to include 
                 procedure id as the second parameter.  
06/07/04   vk    Moved rrcmeas_get_cell_plmn_info() and rrcmeas_plmn_cell_info_struct_type
                 struct to rrclsmif.h at LSM request
06/04/04   vk    Declared prototype for rrcmeas_get_cell_plmn_info() to retrieve
                 cell id, plmn id, psc and freq for LSM. Also declared
				         rrcmeas_plmn_cell_info_struct_type structure
04/11/03   vk    Declared function rrcmeas_save_intra_freq_meas()
02/14/01   vk    Added Code Review Comments
09/14/01   vk	   Added function rrcmeas_init_procedure. This function was
                 earlier defined in rrcmeasi.h and it has been removed from
                 that file
08/30/01   rj    Moved rrcmeas_rpt_stat_e_type and function prototype 
                 rrcmeas_append_meas_rpt_on_rach from rrcmeasi.h
08/29/01   vk    Move all the functions except rrcmeas_procedure_event_handler
                 to rrcmeasi.h
07/31/01   vk    Declared sib 11/12 processing functions

12/19/00   ram   Created file.

===========================================================================*/



/*===========================================================================
**        Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
**        Include Files
**-------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "rrccmd_v.h"
#ifdef FEATURE_FEMTO_CSG
#include "rrccspi.h"
#endif

/*===========================================================================
**        CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/
#define MAX_QHCS_RSCP 26
/*===========================================================================
**        ENUMERATED TYPES
**=========================================================================*/



/*===================================================================
          Enumerated Defn
====================================================================*/


/* Indicates if RACH report available */
typedef enum {
  RRCMEAS_RACH_SUCCESSFUL, 
  RRCMEAS_RACH_FAILURE    
} rrcmeas_rpt_stat_e_type; 


/*===================================================================
          Defines
====================================================================*/



/*===================================================================
          DATA STRUCTURES
====================================================================*/



/*===================================================================
          FUNCTION DECLARATIONS
====================================================================*/
  

/*====================================================================

FUNCTION     RRCMEAS_INIT_PROCEDURE

DESCRIPTION  

  Initialization function which is called on initialization 
  of RRC task. This task registers for state change indication
  manager and also initializes stored variables. Also sets the
  sub state of the measurement state machine to RRCMEAS_INITIAL 
          
          
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/

void rrcmeas_init_procedure
(
  void
);


/*===========================================================================

FUNCTION          RRCMEAS_PROCEDURE_EVENT_HANDLER

DESCRIPTION      
 
  This is the main function handling external events received by the
  RRC Measurement procedure. This function is called by the
  dispatcher whenever an event is received for this module. This 
  function checks the current RRC state and calls the event handler for
  that state. 
             
DEPENDENCIES

  None.
  
RETURN VALUE

  None.
  
SIDE EFFECTS

  None
===========================================================================*/
void rrcmeas_procedure_event_handler
(
  rrc_cmd_type*  cmd_ptr
); 


/*===========================================================================

FUNCTION RRCMEAS_APPEND_MEAS_RPT_ON_RACH

DESCRIPTION

  Appends the measured results to RACH messages if this
  information is requested by the UTRAN. First, it checks
  if the meas report can be appended to the measge. If true,
  then it invokes a l1_process_immediate_meas_req to get 
  intra-freq meas reports from L1.
  
DEPENDENCIES

  RACH reporting must have been activated by utran in sib11 or sib12

RETURN VALUE

  rrc_meas_rpt_stat_e_type
  
SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_rpt_stat_e_type rrcmeas_append_meas_rpt_on_rach
(
  rrc_MeasuredResultsOnRACH* rrcmeas_rach_report_ptr,
  rrc_proc_e_type proc_id,
  boolean exclude_monitored_cell_results
);


/*===========================================================================

FUNCTION   RRCMEAS_SAVE_INTRA_INTER_FREQ_MEAS

DESCRIPTION 

  Tells L1 to save Meas Criteria before it goes to Idle Mode for DCH-->FACH
  directed cell measurements

DEPENDENCIES

  None

RETURN VALUE

  None  

SIDE EFFECTS

  
===========================================================================*/
void rrcmeas_save_intra_inter_freq_meas
(
 void
);

/*==========================================================================
 FUNCTION   rrcifreq_free_memory_allocated_for_cell_list_in_meas_ind

DESCRIPTION

  This function takes care of deallocating the cell list  which was allocated by L1 when it is 
  sending a inter frequency measurement indication to RRC
DEPENDENCIES

    None
RETURN VALUE

    None
SIDE EFFECTS

    None

===========================================================================*/
void rrcifreq_free_memory_allocated_for_cell_list_in_meas_ind
(
  rrc_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION   rrctvm_is_traffic_volume_indicator_to_be_set_in_cu

DESCRIPTION 
  This function checks if there is any TVM with measurement ID 4, if that measurement is event trigerred
  and valid in all states or all states except DCH, if any present calls the mac function to check if the
  traffic volume is above the configured threshold for event 4A for that measurement

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if there is any TVM with measurement ID 4, if that measurement is event trigerred and valid in 
        all states or all states except DCH, if any present calls the mac function and if traffic volume
        is above the configured threshold for event 4A for that measurement
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean rrctvm_is_traffic_volume_indicator_to_be_set_in_cu
(
  void
);

/*====================================================================
FUNCTION: rrcmeas_init_dynamic_ptrs()

DESCRIPTION:
  This function is called to allocate dynamic ptrs

DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrcmeas_init_dynamic_ptrs
(
  void
);
/*====================================================================
FUNCTION: rrcmeas_free_dynamic_ptrs()

DESCRIPTION:
  This function is called to free dynamic ptrs

DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrcmeas_free_dynamic_ptrs
(
  void
);

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/*===========================================================================

FUNCTION   rrcmeas_update_dedicated_priority_info_from_ota

DESCRIPTION 
  This function updates the dedicated priority information stored with dedicated
  priority information in UTRAN Mobility Information (UMI) message.
  If dedicated priorities are configured, will start the validity timer also (T322)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_update_dedicated_priority_info_from_ota
(
  rrc_DedicatedPriorityInformation *ded_pri_info_ptr
);


/*===========================================================================

FUNCTION   rrcmeas_post_dedicated_priority_change_indication

DESCRIPTION 
  This function checks posts RRC_DED_PRI_CHANGE_IND to internal queue on reception of dedicated
  priority information in UTRAN Mobility Information (UMI) message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_post_dedicated_priority_change_indication
(
  void
);

/*===========================================================================

FUNCTION   rrcmeas_invalidate_dedicated_priority_info

DESCRIPTION 
  This function invalidates the dedicated priority information stored and stops the validity timer if running

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_invalidate_dedicated_priority_info
(
  void
);


/*===========================================================================

FUNCTION   rrcmeas_update_dedicated_priority_info_from_irat_req

DESCRIPTION 
  This function updates the dedicated priority information stored with dedicated
   priority information received from interRAT message
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_update_dedicated_priority_info_from_irat_req
(
  lte_irat_dedicated_priority_info_s *irat_ded_priority_list,
  rrc_plmn_identity_type plmn_id
);

/*===========================================================================

FUNCTION   rrcmeas_populate_dedicated_priority_info_for_irat_req

DESCRIPTION 
  This function populates the dedicated priorioty info in inter request with the stored values
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_populate_dedicated_priority_info_for_irat_req
(
  lte_irat_dedicated_priority_info_s *irat_ded_priority_info
);

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*===========================================================================

FUNCTION   rrcmeas_send_clear_ded_pri_ind_set_owner

DESCRIPTION 
  This function checks the current owner of dedicated priorities and send the clear ind to the owner and
  sets owner
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_send_clear_ded_pri_ind_set_owner
(
  void
);
/*===========================================================================

FUNCTION   rrcmeas_send_get_dedicated_priority_req

DESCRIPTION 
  This function checks the current owner of dedicated priorities and send the get req to the owner
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_send_get_dedicated_priority_req
(
  void
);

/*===========================================================================

FUNCTION   rrcmeas_handle_clear_dedicated_pri_ind

DESCRIPTION 
  This function handles the clear dedicated priorities ind from inter RAT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_handle_clear_dedicated_pri_ind
(
  void
);
#endif
#endif

/*===========================================================================
FUNCTION rrc_send_cell_id_cnf_to_cm

DESCRIPTION
  This function is called to send confirmation for cell-info request from CM.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_send_cell_id_cnf_to_cm
(
  rrc_cmd_type *cmd_ptr /* Received command                   */
);

/*===========================================================================
FUNCTION rrc_send_cell_id_ind_to_cm

DESCRIPTION
  This function is called to indicate to CM that cell-info has changed.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_send_cell_id_ind_to_cm
(
  void
);

/*===========================================================================
FUNCTION rrc_send_cell_id_ind_to_cm_in_idle

DESCRIPTION
  This function is called to indicate cell-info in idle mode to CM

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_send_cell_id_cnf_to_cm_in_idle
(
void
);

/*===========================================================================

FUNCTION   rrc_check_eutradetect_ie

DESCRIPTION 
  This function returns TRUE if eutra detection IE is set in SIB19 otherwise returns FALSE
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean rrc_check_eutradetect_ie
(
  void
);
#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO

/*===========================================================================

FUNCTION   rrcmeas_handle_gsm_get_dedicated_pri_req

DESCRIPTION 
  This function handles the get dedicated priorities req from inter RAT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rrcmeas_handle_gsm_get_dedicated_pri_req
(
  rrc_rr_cmd_type* cmd_ptr
);

#if 0

/*===========================================================================

FUNCTION   rrcmeas_send_clear_ded_pri_ind_set_owner_gsm

DESCRIPTION 
  This function checks the current owner of dedicated priorities and send the clear ind to the owner and
  sets owner
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_send_clear_ded_pri_ind_set_owner_gsm
(
  void
);

/*===========================================================================

FUNCTION   rrcmeas_send_get_dedicated_priority_req_to_gsm

DESCRIPTION 
  This function checks the current owner of dedicated priorities and send the get req to the owner
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_send_get_dedicated_priority_req_to_gsm
(
  void
);

#endif
#endif

#ifdef FEATURE_FEMTO_CSG
void rrcmeas_send_csg_mes_info_to_l1(rrc_csp_temp_csg_cell_list *);
#endif
/*===========================================================================

FUNCTION   RRCMEAS_GET_SUBSTATE

DESCRIPTION

  This function retrieves the RRC MEAS SUBSTATE.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  Returns rrcmcm_state in rrcmcm_state_e_type

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcmeas_get_substate( void );
/*===========================================================================
FUNCTION   rrcmeas_return_substate

DESCRIPTION
return current substate

DEPENDENCIES
None

RETURN VALUE
rrcmeas_substate_e_type

SIDE EFFECTS
None
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrcmeas_return_substate(void);
/*===========================================================================
FUNCTION   rrcmeas_get_meas_id_bitmask

DESCRIPTION
return meas id bit mask
DEPENDENCIES
None

RETURN VALUE
rrcmeas_substate_e_type

SIDE EFFECTS
None
===========================================================================*/

#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
uint32 rrcmeas_get_meas_id_bitmask(void);
#endif /* RRCMEAS_H */
