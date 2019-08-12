#ifndef RRC_QCHATUPK_H
#define RRC_QCHATUPK_H
/*===========================================================================

           Q C H A T    W C D M A  R R C  A I R    I N T E R F A C E
               O P T I M I Z A T I O N S    L I B R A R Y
 
GENERAL DESCRIPTION
  This defines the interface provided by the QChat WCDMA air interface
  optimizations library.
 
  QUALCOMM does not grant to you in this Agreement any rights in the OEM
  QChat Porting Kit including the accompanying associated documentation 
  (the Software) for any purpose whatsoever, including without limitation 
  any rights to copy, use or distribute the Software. Your rights to use the 
  Software shall be only as set forth in any separate license between you and
  QUALCOMM which grants to you a license in the Software, and such license
  shall be under the terms and for the limited purposes set forth in that
  agreement.
 
Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/09   suren   Seperated L1 and RRC interfaces.
05/28/09   ali     initial revision 
 
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 
#include "comdef.h"
#include "rrccmd.h"
#include "rrcdata.h"
#include "rrcasn1.h"
#include "gmm_int.h"
#include "mm_umts.h"
/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/

// rrccu.c
typedef enum 
{
  QCHATUPK_DEFER_CU,
  QCHATUPK_ABORT_CU,
  QCHATUPK_SEND_CU
}qchatupk_send_cell_update_status;

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
extern boolean qchatupk_imo_enabled;
extern boolean qchatupk_wftcs_enabled;
extern boolean qchatupk_rdw_enabled;
extern boolean qchatupk_icup_enabled;
extern boolean qchatupk_epe_enabled;
extern boolean qchatupk_asc_enabled;
extern boolean qchatupk_for_enabled;
extern boolean qchatupk_faopts_enabled;
extern boolean qchatupk_fedl_enabled;
extern boolean qchatupk_amssopts_enabled;
/*===========================================================================
FUNCTION qchatupk_wcdma_imo_enabled 
 
DESCRIPTION 
  Determines if QChat IMO optimization is enabled.
  Queries the value of 'qchatupk_imo_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if IMO is enabled or not 
===========================================================================*/
boolean qchatupk_wcdma_imo_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_wftcs_enabled 
 
DESCRIPTION 
  Determines if QChat WFTCS optimization is enabled.
  Queries the value of 'qchatupk_wftcs_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if sbccha is enabled or not  
===========================================================================*/ 
boolean qchatupk_wcdma_wftcs_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_rdw_enabled 
 
DESCRIPTION 
  Determines if QChat RDW optimization is enabled.
  Queries the value of 'qchatupk_rdw_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if RDW is enabled or not 
===========================================================================*/
boolean qchatupk_wcdma_rdw_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_icup_enabled 
 
DESCRIPTION 
  Determines if ICUP optimization is enabled.
  Queries the value of 'qchatupk_icup_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if ICUP is enabled or not  
===========================================================================*/
boolean qchatupk_wcdma_icup_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_epe_enabled 
 
DESCRIPTION 
  Determines if EPE optimization is enabled.
  Queries the value of 'qchatupk_epe_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if EPE is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_epe_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_asc_enabled 
 
DESCRIPTION 
  Determines if ASC ptimization is enabled.
  Queries the value of 'qchatupk_asc_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if ASC is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_asc_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_for_enabled 
 
DESCRIPTION 
  Determines if FOR optimization is enabled.
  Queries the value of 'qchatupk_for_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if FOR is enabled or not 
===========================================================================*/
boolean qchatupk_wcdma_for_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_amssopts_enabled 

DESCRIPTION
   To check if QCHAT AMSS optimization are enabled or not.

RETURN VALUE
   Indicates if AMSS opts is enabled. True/False.
===========================================================================*/
boolean qchatupk_wcdma_amssopts_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_faopts_enabled 
 
DESCRIPTION 
  Determines if FA optimizations are enabled
  Queries the value of 'qchatupk_faopts_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if FA opts are enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_faopts_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_fedl_enabled 
 
DESCRIPTION 
  Determines if Fast EDL Setup optimization is enabled.
  Queries the value of 'qchatupk_fedl_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if Fast EDL Setup optimization is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_fedl_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_ssib7_enabled 
 
DESCRIPTION 
  Determines if SSIB7 optimization is enabled.
  Queries the value of 'qchatupk_ssib7_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if SSIB7 optimization is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_ssib7_enabled(void);

/*===========================================================================
FUNCTION qchatupk_is_tch_setup_in_progress 
===========================================================================*/ 
boolean qchatupk_is_tch_setup_in_progress(void);

/*===========================================================================
FUNCTION qchatupk_is_target_tch_setup_in_progress 
===========================================================================*/ 
boolean qchatupk_is_target_tch_setup_in_progress(void);

/*===========================================================================
FUNCTION qchatupk_rrccu_transition_to_pch
===========================================================================*/
boolean qchatupk_rrccu_transition_to_pch(void);

//rrccu.c
/*===========================================================================
FUNCTION qchatupk_rrccu_process_est_cause 
===========================================================================*/
void qchatupk_rrccu_process_est_cause(rrc_UL_CCCH_Message *loc_msg_ptr);

/*===========================================================================
FUNCTION qchatupk_process_send_cell_update_msg 
===========================================================================*/
qchatupk_send_cell_update_status qchatupk_process_send_cell_update_msg(void);

// rrcdispatcher.c
/*===========================================================================
FUNCTION qchatupk_rrc_process_commands 
===========================================================================*/
void qchatupk_rrc_process_commands(rrc_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION qchatupk_process_rrc_internal_command 
===========================================================================*/
void qchatupk_rrc_process_internal_commands(rrc_cmd_type *cmd_ptr);

//rrcpg1.c
/*===========================================================================
FUNCTION qchatupk_rrcpg1_process_paging_type1_msg
===========================================================================*/
void qchatupk_rrcpg1_process_paging_type1_msg(rrc_PCCH_Message *pcch_msg_ptr);

//rrcllc.c
/*===========================================================================
FUNCTION qchatupk_process_bcch_drop_on_fach_to_dch
===========================================================================*/
void qchatupk_process_bcch_drop_on_fach_to_dch(void);

/*===========================================================================
FUNCTION qchatupk_process_skip_bcch_add_on_pch_to_fach
===========================================================================*/
void qchatupk_process_skip_bcch_add_on_pch_to_fach(void);

/*===========================================================================
FUNCTION qchatupk_delay_hspa_call_status_ind
===========================================================================*/
boolean qchatupk_delay_hspa_call_status_ind(void);

//rrctvm.c
/*===========================================================================
FUNCTION qchatupk_rrctvm_is_traffic_volume_indicator_to_be_set_in_cu
===========================================================================*/
boolean qchatupk_rrctvm_is_traffic_volume_indicator_to_be_set_in_cu(void);

// macrach.c
/*===========================================================================
FUNCTION qchatupk_mac_asc_selection
===========================================================================*/
void qchatupk_mac_asc_selection(uint8 *asc);

/*===========================================================================
FUNCTION qchatupk_mac_reset_rach
===========================================================================*/
void qchatupk_mac_reset_rach(void);

/*===========================================================================
FUNCTION qchatupk_mac_rach_nack
===========================================================================*/
void qchatupk_mac_rach_nack(uint8 npreamble_cycles);

/*===========================================================================
FUNCTION qchatupk_mac_rach_noack
===========================================================================*/
void qchatupk_mac_rach_noack(uint8 npreamble_cycles);

/*===========================================================================
FUNCTION qchatupk_rrc_state_change_handler
===========================================================================*/
void qchatupk_rrc_state_change_handler(rrc_cmd_type *cmd_ptr);

//gmmreg.c
/*===========================================================================
FUNCTION qchatupk_sfor
===========================================================================*/
void qchatupk_sfor(void);

//rrccsp.c & rrcccm.c
void qchatupk_update_location(sys_sys_mode_e_type tech_type);

/*===========================================================================
FUNCTION qchatupk_wcdma_hfn_mismatch_workaround_enabled()
===========================================================================*/
boolean qchatupk_wcdma_hfn_mismatch_workaround_enabled(void);

/*===========================================================================
FUNCTION qchatupk_set_efach_status
===========================================================================*/
void qchatupk_set_efach_status(boolean status);

/*===========================================================================
FUNCTION qchatupk_update_location_cell_dch
===========================================================================*/
void qchatupk_update_location_cell_dch(sys_sys_mode_e_type tech_type, void *cell_info);

#endif /* RRC_QCHATUPK_H */
