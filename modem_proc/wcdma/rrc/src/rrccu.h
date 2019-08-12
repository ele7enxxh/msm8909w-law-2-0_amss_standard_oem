#ifndef RRCCU_H
#define RRCCU_H
/*===========================================================================
              R R C   C E L L   U P D A T E   H E A D E R   F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  Cell Update Procedure.
  
Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccu.h_v   1.4   28 Feb 2002 22:49:24   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccu.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/16   sp     Made changes to reset CU registration info once reconfig procedure is successful
05/26/16   sp      Made changes to log call end info in all cases whenver call is released
05/26/16   ad      Made changes to log important parms from OTAs
04/05/16   sg     Made changes to fail reselection when Channel configuration for CUC is in progress
01/22/15   sr      API to indicate ordered state
01/12/15   ag      Made changes to update staus properly when trimming CU message for 
                   security_revert status set to revert.
09/24/14   sp      Made changes to trim rel-10 non critical extensions if required.
08/06/14   ad      Made changes not to configure C/ERNTI during the channel configuration from CU and 
                   reconfigure MAC later when RNTI update happens.
07/18/14   sp      Made changes to buffer t305 expiry/sib change indication at rrccu when RRC mode is standby.
06/10/14   sg      Made changes to transmit SCRI after CU is completed if close_session is 
                   is received in PCH or URA-PCH states
06/20/14   db      Mainlined FEATURE_WCDMA_HS_RACH_DELAY_CU and introduced NV control
06/18/14   as      Made changes to delay reselection while waiting for L2 ACK for PCH transition
06/17/14   rmn     Made changes so that after Cell Reselection completion in PCH State, 
                   Lock will only be released if CU is not active
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
04/18/13   ad      Made changes to buffer the UL data indication 
                   if RRC is in standby mode and trigger it after resumption
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/13/13   db      Made changes to delay Cell Update when HSRACH Capability changes until Uplink Data Arrives 
09/29/13   db      Made changes to update failure status of procedure upon registration with CU and OC reset
07/31/13   vi      Mainlining the MODEM_STATISTICS feature
07/19/13   db      Made changes to Swap OC and CC after channel config by CU when
                   a procedure is registered and it fails
05/22/13   sa      Made changes for spec Cr5235
01/03/13   db       Made changes to not Swap OC and CC  when DCH_FACH timer expires to an EFACH cell
12/25/12   db      Made changes to requeue transition to disconnected triggered by CU when there is no response
11/07/12   db       Made changes to backup and restore drx co-efficient
08/13/12   md     Added code changes to trim CU message size by skipping rel8 capability indication
                  in CU if CU is triggered for PS/CS session establishment.
08/13/12   md     Added code changes to trim CU message size by nullifying EDRX capability indication 
                  when CU is triggered for CS call.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
11/07/12   ad       Made changes to handle ERNTI correctly. Changes to avoid starting MAC
                    for a HSRACH to HSRACH SIB5 change.Increased the handing rb size.
08/18/11   ad      Made changes to block rab establish indication if rabs have 
                   already been released under WRLF feature
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added suport for mdsp halt recovery
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11   ad      Made changes to remove compilation error
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
03/17/10   gkg     To comply with CMI, featurize Callback function pointer 
                   rrccu_rlc_ul_data_ind with feature FEATURE_DSM_WM_CB
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
11/27/09   gkg     RRC changes required to integrate QChat Porting Kit Optimizations 
                   into AMSS. The changes are under feature FEATURE_QCHAT.
11/02/09   rm      Added support for FEATURE_MODEM_STATISTICS
07/27/09   gkg     Made changes so that CU procedure waits for SIB7
                   to be current whenever CU goes from *_PCH to Cell_FACH.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
07/17/07   rm      Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
06/13/07   kp      Added bit mask to support trimming of CU message if it exceed
                   UL TM RLC size.
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
08/28/06   da      Added support for OOS area handling during transition from DCH.
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/15/06   sgk     Included cu substate typedef, added prototypes for  
                   rrccu_check_rb_in_estab_rab, rrccu_return_cu_substate.
01/21/05   da      Added prototype for rrccu_get_max_timer_t314_t315.
08/30/04  sgk      Added protoypes for rrccu_clear_ura_id and 
                   rrcc_update_ura_id to rrccu.h as they were being called in 
                   rrcrbe.c to fix lint errors
08/07/02   rj      Added prototype for rrccu_register_for_cell_update_to_complete
02/28/02   rj      Added rrccu_init_procedure function prototype which 
                   initializes the Cell Update procedure parameters. Fixed
                   a typo in init function.
02/14/02   rj      Modified rrccu_get_cell_update_status into two interfaces
                   rrccu_get_cell_update_started_status and 
                   rrccu_get_cell_update_complete_status. Former interface is
                   used by the RB Reconfig procedures to process Downlink OTA
                   messages and the later will be used by all procedures to
                   make sure valid C-RNTI while transmitting in CELL_FACH state.
07/27/01   rj      Fixed ARM compilation warning.
07/15/01   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "rrcdata_v.h"
#include "rrccmd_v.h"
#include "rrcrbcommon.h"
/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*Currently used Bit mask used for trimming Cell update message to UL TM RB size*/
/*This Bit mask is used to re-set trim cell update variable & also to check whether a bit is set or not */
#define RRCCU_TRIM_REQ_BIT_MASK_NOT_SET                                         0x00000000
/*This Bit mask is used to indicate monitored cell info is removed in cu message*/
#define RRCCU_REMOVED_MONITORED_CELL_FROM_MEAS_RPT_ON_RACH   0x00000001
/*This Bit mask is used to indicate Measurment on RACH is removed in cu message*/
#define RRCCU_REMOVED_ENTIRE_MEAS_RPT_ON_RACH                               0x00000002
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*This Bit mask is used to indicate that REL8 capabilities are removed in CU message*/
#define RRCCU_REMOVED_REL8_CAP_IND_IF_CS_OR_PS_SESS_PEND            0x00000008
#endif
#define RRCCU_REMOVED_SUPPORT_FOR_TWO_DRX_SCHEMES_IN_PCH          0x000000010
#define RRCCU_REMOVED_SECURITY_REVERT_STATUS_IND                  0x000000020
#define RRCCU_REMOVED_SECURITY_REVERT_STATUS_IND_NORMAL_OPERATION 0x000000040
#define RRCCU_TRANS_TO_DISCON_MAX_TRY 5
#define RRCCU_TRANS_TO_DISON_TIMER 60000
/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


typedef enum 
{
  RRCCU_COMPLETED,                          /* This indicates that the 
                                               Cell Update procedure is
                                               completed and UE has valid
                                               C-RNTI */ 
  RRCCU_NOT_COMPLETED                       /* This indicates that the Cell
                                               Update proc is not  completed 
                                               and is in the process getting
                                               valid C-RNTI. */
  
} rrccu_cell_update_complete_status_e_type; 


typedef enum 
{
  RRCCU_STARTED,                            /* This indicates that the 
                                               Cell Update proc is started */ 

  RRCCU_NOT_STARTED                         /* This indicates that the Cell
                                               Update proc is not started */
  
} rrccu_cell_update_started_status_e_type; 




 /*==========================================================================
  Type for defining the substate of Cell Update Procedure.
===========================================================================*/

typedef enum
{
  RRCCU_INITIAL,                     /* CU initial sub state    */
  RRCCU_WAIT_FOR_UE_TO_CAMP_ON,      /* CU waits for UE to camp on */
  RRCCU_WAIT_FOR_RB0_EST_CNF,        /* CU waits for RB0 conf from RRC LLC */
  RRCCU_WAIT_FOR_RB1_RB4_EST_CNF,    /* CU waits for RB1toRB4 conf from RRC LLC */
  RRCCU_WAIT_FOR_CELL_UPDATE_CONFIRM, /* CU waits for Cell Update Confirm Msg */ 
  RRCCU_WAIT_FOR_URA_UPDATE_CONFIRM,
                                   
  RRCCU_WAIT_FOR_RNTI_UPDATE_CNF,    /* CU waits for RNTI update */
  RRCCU_WAIT_FOR_RB_EST_CNF,         /* CU waits for conf from CSP procedure */
  RRCCU_WAIT_FOR_PCCH_RB_EST_CNF,    /* CU waits for PCCH channel establishment */
  RRCCU_WAIT_FOR_L2_ACK,             /* CU waits for L2-ACK for Resp Msg */
  RRCCU_WAIT_FOR_SRB2_RE_EST_CNF,       /* CU waits for conf from CSP procedure */
  RRCCU_WAIT_FOR_RB_REL_CNF,         /* CU waits for RB Release confirm from
                                      LLC */  
  RRCCU_WAIT_FOR_BPLMN_SUSPEND_CNF,    /* CU waits for BPLMN Suspend CNF */                                     
  RRCCU_WAIT_FOR_PENDING_CONFIG       /* CU waits for pending configuration completion */

  , RRCCU_WAIT_FOR_RB_PROC_COMPL_IND

#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
  , RRCCU_WAIT_FOR_SIB7_CURRENT
#endif /*FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH*/

} rrccu_substate_e_type;

/* Below #defines are for logging purpose only */
  #define rrccu_substate_e_type_value0 RRCCU_INITIAL,                    
  #define rrccu_substate_e_type_value1 RRCCU_WAIT_FOR_UE_TO_CAMP_ON
  #define rrccu_substate_e_type_value2 RRCCU_WAIT_FOR_RB0_EST_CNF
  #define rrccu_substate_e_type_value3 RRCCU_WAIT_FOR_RB1_RB4_EST_CNF
  #define rrccu_substate_e_type_value4 RRCCU_WAIT_FOR_CELL_UPDATE_CONFIRM
  #define rrccu_substate_e_type_value5 RRCCU_WAIT_FOR_URA_UPDATE_CONFIRM
  #define rrccu_substate_e_type_value6 RRCCU_WAIT_FOR_RNTI_UPDATE_CNF
  #define rrccu_substate_e_type_value7 RRCCU_WAIT_FOR_RB_EST_CNF
  #define rrccu_substate_e_type_value8 RRCCU_WAIT_FOR_PCCH_RB_EST_CNF
  #define rrccu_substate_e_type_value9 RRCCU_WAIT_FOR_L2_ACK
  #define rrccu_substate_e_type_value10 RRCCU_WAIT_FOR_SRB2_RE_EST_CNF
  #define rrccu_substate_e_type_value11 RRCCU_WAIT_FOR_RB_REL_CNF
  #define rrccu_substate_e_type_value12 RRCCU_WAIT_FOR_BPLMN_SUSPEND_CNF                           
  #define rrccu_substate_e_type_value13 RRCCU_WAIT_FOR_PENDING_CONFIG       
  #define rrccu_substate_e_type_value14 RRCCU_WAIT_FOR_RB_PROC_COMPL_IND

#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
  #define rrccu_substate_e_type_value15 RRCCU_WAIT_FOR_SIB7_CURRENT
#endif /*FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH*/

#define RRC_T320_INVALID_VALUE 0xFF

/*===========================================================================

                      FUNCTION DECLARATIONS

=========================================================================*/


/*===========================================================================

FUNCTION rrccu_start_t_305_timer

DESCRIPTION
  This function is called during transition from CELL_DCH to CELL_FACH/
  CELL_PCH/URA_PCH to start the periodic cell update timer T_305
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  rrccu_start_t_305_timer(void);
/*===========================================================================

FUNCTION rrccu_check_if_rabs_need_to_be_released

DESCRIPTION
  This function returns the value of rrccu_rl_failure_info.t314_info.need_to_release_rabs
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean  rrccu_get_rabs_need_to_be_released_T314(void);
/*===========================================================================

FUNCTION rrccu_check_if_rabs_need_to_be_released

DESCRIPTION
  This function returns the value of rrccu_rl_failure_info.t314_info.need_to_release_rabs
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean  rrccu_get_rabs_need_to_be_released_T315(void);
#ifdef FEATURE_UMTS_PDCP
/*===========================================================================

FUNCTION   RRCCU_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccu_is_srns_in_progress
(
  void
);
#endif
/*===========================================================================

FUNCTION RRCCU_GET_CELL_UPDATE_STARTED_STATUS

DESCRIPTION

  This function checks whether Cell Update procedure is started or not.
  
DEPENDENCIES


RETURN VALUE

  A value of rrccu_cell_update_started_status_e_type indicating the started
  status of the function.

SIDE EFFECTS

  None

===========================================================================*/
rrccu_cell_update_started_status_e_type rrccu_get_cell_update_started_status
(    
  void
);
/*===========================================================================

FUNCTION RRCCU_GET_CELL_UPDATE_COMPLETE_STATUS

DESCRIPTION

  This function checks whether Cell Update procedure is active or not and 
  returns the status to the procedure.
  
DEPENDENCIES


RETURN VALUE

  A value of rrccu_cell_update_complete_status_e_type indicating the complete
  status of the function.

SIDE EFFECTS

  None

===========================================================================*/
rrccu_cell_update_complete_status_e_type rrccu_get_cell_update_complete_status
(    
  rrc_proc_e_type rrc_proc
);
/*===========================================================================

FUNCTION rrccu_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the Cell Update  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrccu_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION rrccu_init_procedure

DESCRIPTION
  This function will initialize the Cell Update procedure substate to 
  RRCCU_INITIAL Substate. This will called in the RRCTASK modules 
  along with other Init Procedures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrccu_init_procedure
(    
  void  
);

/*===========================================================================

FUNCTION RRCCU_REGISTER_FOR_CELL_UPDATE_TO_COMPLETE

DESCRIPTION

  This function registers RRC proceudre to inform when Cell Update is
  completed.
  
DEPENDENCIES


RETURN VALUE

  A value of uecomdef_status_e_type indicating success or failure of 
  registration..

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type rrccu_register_for_cell_update_to_complete
(    
  rrc_proc_e_type rrc_proc
);
/*===========================================================================

FUNCTION RRCCU_RESET_REGISTRATION_INFO_FOR_PROCEDURE

DESCRIPTION
  This function will reset the registration information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrccu_reset_registration_info_for_procedure
(
  rrc_proc_e_type proc_id
);

/*===========================================================================
      
      FUNCTION RRCCU_VALIDATE_URA_UPDATE_INITIATE_REQ
      
      DESCRIPTION
        
      This function will
      1. Check if the current URA_ID is present in the list of URA identities 
      specified in SIB2 database.
      2. Checks if SIB2 is empty
      3. Checks if SIB2 can't be found.
      
      If any of the above condition is true then it will initiate a URA update request
      with cause "ura_reselection"
        
      DEPENDENCIES
        None
      
      RETURN VALUE
        None
        
      SIDE EFFECTS
      
        None
      
===========================================================================*/
boolean rrccu_check_for_ura_reselection
(
  void
);

/*===========================================================================
   
      FUNCTION RRCCU_CLEAR_URA_ID
      
      DESCRIPTION
        
      This function will clear the URA_ID field. Would set it to 0xffff
        
      DEPENDENCIES
        None
      
      RETURN VALUE
        None
        
      SIDE EFFECTS
      
        None
      
===========================================================================*/

void rrccu_clear_ura_id
(
  void
);

/*===========================================================================
      
      FUNCTION RRCCU_UPDATE_URA_ID
      
      DESCRIPTION
        
      This function will convert the oss_bitstring to the unit32 value
	  and then stores the value of in the URA_ID field.
        
      DEPENDENCIES
        None
      
      RETURN VALUE
        None
        
      SIDE EFFECTS
      
        None
      
===========================================================================*/

void rrccu_update_ura_id
(
  ASN1BitStr32 *ura_id
);
#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
      
      FUNCTION rrccu_get_cu_initiate_reqd
      
      DESCRIPTION
        
      This function will return the rrccu_cu_initiate_reqd.cu_reqd flag
        
      DEPENDENCIES
        None
      
      RETURN VALUE
        None
        
      SIDE EFFECTS
      
        None
      
===========================================================================*/

boolean rrccu_get_cu_initiate_reqd
(
  void
);

/*===========================================================================

FUNCTION rrccu_set_hsrach_sib_change

DESCRIPTION
  Sets the flag .  

DEPENDENCIES
  None.

RETURN VALUE
  NONE

===========================================================================*/
void rrccu_set_hsrach_sib_change
(
  boolean status
);

/*===========================================================================

FUNCTION rrccu_get_hsrach_sib_change

DESCRIPTION
  Gets the flag

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

===========================================================================*/
boolean rrccu_get_hsrach_sib_change
(
  void 
);


/*===========================================================================

FUNCTION rrccu_reset_hsrach_reest_flags

DESCRIPTION
  resets the re-establishment flags for hsrach

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrccu_reset_hsrach_reest_flags
(
  void
);
#endif /*FEATURE_WCDMA_HS_RACH*/

/*===========================================================================
      
      FUNCTION RRCCU_GET_URA_ID
      
      DESCRIPTION
        
      This function will populate the URA_ID in the arguments passed
        
      DEPENDENCIES
        None
      
      RETURN VALUE
        None
        
      SIDE EFFECTS
      
        None
      
===========================================================================*/

void rrccu_get_ura_id
(
  uint32 *ura_id
);

/*===========================================================================

FUNCTION rrccu_get_max_timer_t314_t315

DESCRIPTION
  This procedure returns the max timer value between t-314 and t-315.  

DEPENDENCIES
  None.

RETURN VALUE
  uint32.

===========================================================================*/
uint32 rrccu_get_max_timer_t314_t315
( 
  void 
);



/*===========================================================================

FUNCTION rrccu_process_successful_completion

DESCRIPTION
 This function handles the successful completion of CU CNF or URA Update CNF.

DEPENDENCIES
  None.

RETURN VALUE
  uint32.

===========================================================================*/
void rrccu_process_successful_completion
( 
  void 
);

/*===========================================================================

FUNCTION rrccu_check_rb_in_estab_rab

DESCRIPTION
  This function will check whether the rb is in established rabs, if so it returns
  true, else it returns false and clears cu state

DEPENDENCIES
  None

RETURN VALUE
  boolean
  TRUE  : rb is in established rabs
  FALSE : rb is not in established rabs 

SIDE EFFECTS
  None
===========================================================================*/

boolean rrccu_check_rb_in_estab_rab
(
 void
);
 /*===========================================================================

FUNCTION RRCCU_RETURN_CU_SUBSTATE

DESCRIPTION
  This function returns the cu subtate

  
DEPENDENCIES
  None

RETURN VALUE
  cu substate
  
SIDE EFFECTS

  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
 __attribute__((section(".uncompressible.text")))
#endif
rrccu_substate_e_type rrccu_return_cu_substate
(
  void
);

/*===========================================================================

FUNCTION rrccu_get_cu_cause

DESCRIPTION

  Returns cell update cause
  
DEPENDENCIES


RETURN VALUE

  Returns cell update cause

SIDE EFFECTS

  None

===========================================================================*/

rrc_CellUpdateCause rrccu_get_cu_cause(void);

/*===========================================================================

FUNCTION rrccu_reset_trim_bit_mask

DESCRIPTION
  This procedure Reset Cell Update Trim bit mask.


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccu_reset_trim_bit_mask
(
void
);
/*===========================================================================

FUNCTION rrccu_trim_cu_msg_ul_tm_tb_size

DESCRIPTION
  This procedure is the event handler for the Cell Update procedure.


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

boolean rrccu_trim_cu_msg_ul_tm_tb_size
(
  rrc_UL_CCCH_Message *msg_ptr,
  uint32 pdu_length
);

/*===========================================================================

FUNCTION rrccu_rlc_ul_data_ind

DESCRIPTION

  This function is used as call back function from rlc when uplink data 
  presence has to be indicated in cell_pch or ura_pch state.
  
DEPENDENCIES


RETURN VALUE
           viod


SIDE EFFECTS

  None

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
void rrccu_rlc_ul_data_ind (struct dsm_watermark_type_s *, void *);
#else /*FEATURE_DSM_WM_CB*/
void rrccu_rlc_ul_data_ind (void);
#endif /*FEATURE_DSM_WM_CB*/

/*===========================================================================
FUNCTION rrccu_send_cell_update_initiate_req

DESCRIPTION
  This function posts a cell update request to rrccu itself
  in internal command queue. This function is used to avoid the race conditions. 

DEPENDENCIES
  None.

RETURN VALUE
  None.
===========================================================================*/
void rrccu_send_cell_update_initiate_req
(
  rrc_cell_update_cause_e_type cause
);

/*===========================================================================
FUNCTION       rrccu_init_last_cu_cnf_info

DESCRIPTION
   Initialises all the variables, which was required to send a response to the previous CU Cnf
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_init_last_cu_cnf_info
(
  void
);

/*===========================================================================
FUNCTION RRCCU_CLEAR_PROCEDURE

DESCRIPTION
  This function will clear the global variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_clear_procedure
(
  void
);

/*===========================================================================
FUNCTION RRCCU_SET_CU_SUBSTATE

DESCRIPTION
  This function sets the cu subtate

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_set_cu_substate
(
  rrccu_substate_e_type state
);

/*===========================================================================

FUNCTION rrccu_check_if_cell_selection_in_progress

DESCRIPTION
   Checks if Cell Selection was triggerred to due to the directed cell info present 
   in CU Cnf msg.
   This is used by CCM to to send NEW_CELL_IND to CU in  this scenario
DEPENDENCIES
  None

RETURN VALUE
  TRUE : If the CELL Selection was triggered due to directed cell info in CU CNF
  else
  FALSE
SIDE EFFECTS
  None
===========================================================================*/
boolean rrccu_check_if_cell_selection_in_progress
(
  void 
);


/*===========================================================================
FUNCTION   rrccu_map_cu_cause

DESCRIPTION
  This function maps the OTA format CU cause type to RRC internal format CU cause.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
rrc_cell_update_cause_e_type rrccu_map_cu_cause
(
  rrc_CellUpdateCause rrccu_cell_update_cause
);

/*===========================================================================
FUNCTION rrccu_get_cu_cause_for_wait_timer

DESCRIPTION
  Returns the cause for which CU was requested when the wait timer was running. Basically it is the pending CU cause.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
rrc_cell_update_cause_e_type rrccu_get_cu_cause_for_wait_timer
(
  void
);

/*===========================================================================
FUNCTION rrccu_reset_cu_cause_for_wait_timer

DESCRIPTION
  Resets cu cause for wait timer stored in CU internal DB.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_reset_cu_cause_for_wait_timer
(
  void
);
/*===========================================================================

FUNCTION rrccu_set_cu_cause_for_wait_timer

DESCRIPTION 
  This function sets the variable cu_cause_for_wait_timer with an appropriate CU cause

DEPENDENCIES
    None.

RETURN VALUE
None

===========================================================================*/

void rrccu_set_cu_cause_for_wait_timer(rrc_cell_update_cause_e_type cu_cause);


/*===========================================================================
FUNCTION rrccu_get_cu_wait_timer_value

DESCRIPTION
  Returns the value of the wait timer that was given by N\W in PCH state.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
uint8 rrccu_get_cu_wait_timer_value
(
  void
);

/*===========================================================================
FUNCTION rrccu_reset_cu_wait_timer_value

DESCRIPTION
  Resets wait timer value in CU internal DB.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_reset_cu_wait_timer_value
(
  void
);

#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
/*===========================================================================
FUNCTION   rrccu_handle_mdsp_halt_recov_in_dch

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

boolean rrccu_handle_mdsp_halt_recov_in_dch
(
  void
);
#endif

/*===========================================================================

FUNCTION rrccu_tran_to_disconn_debug_handler

DESCRIPTION 
    This function is the handler when transition to disconnected timer eby CU xpires 
DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
void rrccu_tran_to_disconn_debug_handler
(
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION rrccu_check_if_registered_reconfig_proc_is_successful

DESCRIPTION 
  Checks if any reconfiguration procs hacve registered with Cell Update 
procedure and 
  if the procedure is succesful(if yes, procedure will take care of swapping 
OC and CC ,
  else CU will have to take care

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/

boolean rrccu_check_if_registered_reconfig_proc_is_successful
(
  void 
);


/*===========================================================================

FUNCTION rrccu_update_reconfig_proc_failure_status

DESCRIPTION 
  Updates reconfig procedures failure status so that LLC can swap after chan config by CU is done

DEPENDENCIES
    None.

RETURN VALUE
   NONE
successful.

===========================================================================*/

void rrccu_update_reconfig_proc_failure_status
(
  rrc_proc_e_type  proc_id
);

/*===========================================================================
      
      FUNCTION rrccu_post_buffered_commands
      
      DESCRIPTION
        
      This function will post any commands buffered when.
      UE was in standby mode
        
      DEPENDENCIES
        None
      
      RETURN VALUE
        None
        
      SIDE EFFECTS
      
        None
      
===========================================================================*/
void rrccu_post_buffered_commands
(
  void
);
/*===========================================================================

FUNCTION rrccu_get_dest_state

DESCRIPTION 
  This function returns the destination state

DEPENDENCIES
   None.

RETURN VALUE
   rrc_state_e_type

SIDE EFFECTS      
    None
===========================================================================*/
rrc_state_e_type rrccu_get_dest_state
(
  void
);
/*===========================================================================

FUNCTION rrc_check_cu_started_with_cause_rlfailure_or_rlc_unrecoverable_error

DESCRIPTION 
  This function returns true if cu is sent with cause RL failure or RRC unrecoverable error

DEPENDENCIES
    None.

RETURN VALUE
   boolean
successful.

===========================================================================*/
boolean rrc_check_cu_started_with_cause_rlfailure_rlc_unrecoverable_error
(
  void
);

/*===========================================================================

FUNCTION rrccu_check_cu_active_before_lock_rel

DESCRIPTION 
  This procedure is used to query if CU is active for UL Data Transmission,
  Periodic Cell Update, Paging Response 

DEPENDENCIES
    None.

RETURN VALUE
   NONE
successful.

===========================================================================*/

boolean rrccu_check_cu_active_before_lock_rel
(
  void
);
/*===========================================================================

FUNCTION rrccu_set_cu_cause_to_ul_data_indication

DESCRIPTION 
  This function sets cell update cause to UL data indication. 
  UL data inidication can be directly updated as CU cause as it has highest priority.
  This function should be used only if current state is PCH

DEPENDENCIES
    None.

RETURN VALUE
None

===========================================================================*/
void rrccu_set_cu_cause_to_ul_data_indication();

/*===========================================================================

FUNCTION rrccu_return_ordered_state

DESCRIPTION 
   This function returns the ordered state give by NW in Cell update confirmation

DEPENDENCIES
    None.

RETURN VALUE
    Ordered RRC state

===========================================================================*/

rrc_RRC_StateIndicator rrccu_return_ordered_state(void);
/*===========================================================================
      
      FUNCTION rrccu_check_if_crnti_valid
      
      DESCRIPTION
        
      This function returns rrccu_new_crnti_valid
        
      DEPENDENCIES
        None
      
      RETURN VALUE
        boolean
        
      SIDE EFFECTS
      
        None
      
===========================================================================*/
boolean rrccu_check_if_crnti_valid
(
  void
);
/*===========================================================================

FUNCTION rrccu_check_if_cu_ordered_state_is_pch_and_oc_is_not_set

DESCRIPTION 
   This function checks if we are transitioning to PCH and if OC is not set

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/

boolean rrccu_check_if_cu_ordered_state_is_pch_and_oc_is_not_set(void);
/*===========================================================================

FUNCTION rrccu_is_channel_config_in_process

DESCRIPTION 
   This function indicates whether lower layer configuration is in progress

DEPENDENCIES
    None.

RETURN VALUE
    boolean

===========================================================================*/
boolean rrccu_is_channel_config_in_process();
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION rrcqsh_log_cuc_ota_params

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
None

SIDE EFFECTS
  None

===========================================================================*/

void rrcqsh_log_cuc_ota_params();
/*===========================================================================
FUNCTION rrccu_get_num_rb_rel

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
None

SIDE EFFECTS
  None

===========================================================================*/

uint8 rrccu_get_num_rb_rel();

/*===========================================================================
FUNCTION rrccu_get_num_rb_rel

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
None

SIDE EFFECTS
  None

===========================================================================*/

boolean rrccu_get_cuc_for_srns_relocation();
/*===========================================================================

FUNCTION rrccu_qsh_log_rlf_params

DESCRIPTION
  This function reports rlf parameters to qsh

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/

void rrccu_qsh_log_rlf_params(sys_rlf_weak_signal_e_type cause);
#endif
#endif /* RRCCU_H */
