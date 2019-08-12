#ifndef RRCRBCOMMON_H
#define RRCRBCOMMON_H
/*===========================================================================
         R R C   R A D I O   B E A R E R   C O M M O N   M O D U L E 

DESCRIPTION

  This module is the header module for the Radio Bearer Common module.
  This contains the external interface functions to be used by
  by other RRC Radio Bearers modules. This must not be used by modules 
  outside RRC.
  
Copyright (c) 2000, 2001, 2003-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrbcommon.h_v   1.2   08 May 2001 08:49:32   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrbcommon.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/14   sp      Made changes to mainline DCH-FACH transition optimization changes
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/04/14   as      Made changes for DCH-FACH transition time optimization
05/16/14   vi      Made changes to support MOB framework
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
10/09/13   sp      Added changes for rel-10 ASU message that were missed in the rel 10 signalling changes
09/29/13   db      Made changes to update failure status of procedure upon registration with CU and OC reset
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
06/27/13   sa      Added changes to support call type mask for AMR with PS0x0 config
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
11/22/12   sa      Made changes to update the call type mask for CS data 
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/04/12   pm      Made sure SRB's are present in Chan_config_req during FACH to DCH transition
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11   rm      Added prototype of rrcrb_proc_waiting_for_l2_ack
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/30/10   gkg     Fixed Compiler Warning.
04/06/10   gkg     Made changes to check for R8 OTA choice if R8 OTA is being processed.
03/17/10   as      Resolved Errors due to ASN.1 8.9 Migration
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
10/01/09   gkg     Made changes so that SCUDIF is handled based on derived
                   configuration instead of relying on MM.
05/08/09   ss      Updated Copyright Information
04/09/09   ps      Made changes to send reconfig ind in CU/or go to idle
                   if RL failure is triggered while l2 ack for reconfig complete
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of OTA messages, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
04/17/08   ps      Fixed compiler error when FEATURE_SCUDIF_ENHANCEMENT and 
                   FEATURE_VOC_AMR_WB are enabled 
04/14/08   gkg     Modified prototype of rrc_config_vt_to_voice().
01/08/08   ps/gk   Made changes for SCUDIF under feature flag 
                   FEATURE_SCUDIF_ENHANCEMENT
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
05/22/07   sm      Corrected feature definition requirement for function
                   rrcllc_validate_rb_info_reconfig_list_r6()
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/07/07   ps      Made changes to handle Standalone Rb-Mappings
02/02/07   da      Removed prototype rrcrb_get_dch_fach_trans_delay.
11/15/06   sm      Added function rrcllc_validate_rb_info_reconfig_list_r6()
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   da      Added prototye for  rrcrb_get_new_crnti_valid()
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/15/06   sgk     Added define for RRCRB_PND_CFG_DELAY.This define time in ms
                   for which the rb procedure waits to send cell update
                   after sending the complete message, for scenarios where
                   there has been an RLC unrecoverable error on non srb2 during
                   a pending configuration.
12/01/05   da      Added prototype for rrcrb_get_dch_fach_trans_delay()
08/15/05   da      Removed prototype for rrc_query_mac_rab_status()
07/29/05   da      Added prototypes for 2 new funcs. rrc_query_mac_rab_status()
                   and rrcrb_update_mac_rab_status()
06/15/05   da      Added value RRCRB_CELL_UPDATE_OCCURED to rrcrb_msg_status_e_type
11/23/04   sm      Removed element 'curr_rab_type' from structure rrcrb_rab_info_per_msg_type.
                   Moved ENUM rrcrb_rab_e_type from this file to rrcdata_v.h.
10/21/04   da      Removed FEATURE_CELLPCH_URAPCH.  CELLPCH and URAPCH states should always
                   be handled.  Deleted code that handles case when CELLPCH and URAPCH is not
                   defined.
10/19/04   da      Removed FEATURE SRNS RELOCATION.  SRNS_RELOCATION should always be 
                   handled.  Deleted code that handles case when SRNS_RELOCATION is not
                   defined.
08/20/04   jh(kc)  Added function rrcrb_update_amr_cctrch_id_for_reconfig().
07/14/04   sk      Added prototype for function to get the current substate
                   Added enum rrcrb_substate_e_type. This can be used by
                   all RB procedures.
05/04/04   sk      Changed rrcrb_send_rnti_update_req() input parms.
04/06/04   ttl     Handle multiple RABs setup in a single RBSetupMessage.
02/04/04   sk      Moved common function from all rb procedures to
                   common file.
                   Added function to send DRX req in common file
11/14/03   kc      Added interface definitions to update RNTI info in a common file
                   also added interface definitions to re-establish RB's for SRNS
                   relocation.
05/08/01   ram     Fixed typo in constant definition RRCRB_INVALID_TRANSACTION_ID.
05/02/01   ram     Added data structures to temporarily store information
                   associated with RABs and RBs till ESTABLSIHED_RABS can
                   be updated. Updated enum type with more failure causes.
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

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Enumerated type of status of validating an RB message                   */
typedef enum
{
  RRCRB_MSG_VALID,              /* Message is valid                        */
  RRCRB_MSG_UNSUPPORTED_CONFIG, /* Message has un-supported configuration  */
  RRCRB_PHY_CHAN_FAILURE,       /* Physical channel setup failed           */
  RRCRB_SIMULTANEOUS_RECONFIG,  /* Incompatible simultaneous reconfig      */
  RRCRB_MSG_PROTOCOL_ERROR,     /* Message has protocol error              */
  RRCRB_MSG_INVALID_CONFIG,     /* Message has invalid configuration       */
  RRCRB_MSG_INCOMPLETE_CONFIG,  /* Message has incomplete configuration    */
  RRCRB_CELL_UPDATE_OCCURED     /* Cell update with oss occured            */
}rrcrb_msg_status_e_type;

typedef enum
{
  RRCTC_MSG_VALID,              /* Message is valid                        */
  RRCTC_MSG_UNSUPPORTED_CONFIG, /* Message has un-supported configuration  */
  RRCTC_PHY_CHAN_FAILURE,       /* Physical channel setup failed           */
  RRCTC_SIMULTANEOUS_RECONFIG,  /* Incompatible simultaneous reconfig      */
  RRCTC_MSG_PROTOCOL_ERROR,     /* Message has protocol error              */
  RRCTC_MSG_INVALID_CONFIG,     /* Message has invalid configuration       */
  RRCTC_MSG_INCOMPLETE_CONFIG,   /* Message has incomplete configuration    */
  RRCTC_CELL_UPDATE_OCCURED
}rrctc_msg_status_e_type;

/* Enumerated type of status of validating an PCRC message                   */
typedef enum
{
  RRCPC_MSG_VALID,              /* Message is valid                        */
  RRCPC_MSG_UNSUPPORTED_CONFIG, /* Message has un-supported configuration  */
  RRCPC_PHY_CHAN_FAILURE,       /* Physical channel setup failed           */
  RRCPC_SIMULTANEOUS_RECONFIG,  /* Incompatible simultaneous reconfig      */
  RRCPC_MSG_PROTOCOL_ERROR,     /* Message has protocol error              */
  RRCPC_MSG_INVALID_CONFIG,     /* Message has invalid configuration       */
  RRCPC_MSG_INCOMPLETE_CONFIG,   /* Message has incomplete configuration    */
  RRCPC_CELL_UPDATE_OCCURED     /* CELL update due to OOS occured          */
}rrcpc_msg_status_e_type;


/* Enumerated type of the different substates for RB procedures           */
typedef enum
{
  RRCRB_INITIAL,                   /* Initial Substate                    */
  RRCRB_WAIT_FOR_CELL_UPDATE_CNF,  /* Wait for cell update                */
  RRCRB_WAIT_FOR_CELL_SELECTION_CNF,  /* wait for cell selection      */
  RRCRB_WAIT_FOR_CHAN_CFG_CNF,     /* Waiting for confirmation of channel  
                                    config from LLC                        */                      
  RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF,     /* Waiting for CRNTI update cnf   */                      
  RRCRB_WAIT_FOR_L2_ACK,           /* Waiting for Layer 2 ack for uplink
                                    message transmission                   */
  RRCRB_WAIT_FOR_REESTABLISH_SRB2, /*waiting for re-establishment of srb2 */
  RRCRB_MAX_SUBSTATES
}rrcrb_substate_e_type;
/* Below #defines are added for logging purpose only */
#define rrcrb_substate_e_type_value0 RRCRB_INITIAL
#define rrcrb_substate_e_type_value1 RRCRB_WAIT_FOR_CELL_UPDATE_CNF
#define rrcrb_substate_e_type_value2 RRCRB_WAIT_FOR_CELL_SELECTION_CNF
#define rrcrb_substate_e_type_value3 RRCRB_WAIT_FOR_CHAN_CFG_CNF		 
#define rrcrb_substate_e_type_value4 RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF		   
#define rrcrb_substate_e_type_value5 RRCRB_WAIT_FOR_L2_ACK 
#define rrcrb_substate_e_type_value6 RRCRB_WAIT_FOR_REESTABLISH_SRB2
#define rrcrb_substate_e_type_value7 RRCRB_MAX_SUBSTATES

/* structure to be used for RNTI update request by rb procedures. */
typedef struct
{
  rrc_proc_e_type            procedure;            /* procedure requesting rnti update       */
  rrc_rnti_cmd_status_e_type crnti_action;    /* indicates C-RNTI needs tobe updated    */
  rrc_rnti_cmd_status_e_type urnti_action;    /* indicates U-RNTI nees tobe updated     */
  uint16                     crnti;                /* C-RNTI value                           */
  rrc_int_u_rnti_type        urnti;                /* U-RNTI value                           */
  boolean                    rlc_re_establish_srb; /* indicates rlc reestablishment for srbs */
  boolean                    rlc_re_establish_rab; /* indicates rlc reestablishment for rab  */
  boolean                    cnf_reqd;             /* indicates confirmation reqd for update */
} rrcrb_rnti_update_info_type;

/* structure to be used for directed cell info (freq and scr) */
typedef struct
{
  boolean                    freq_info_present; /* indicates whether freq info present */
  uint32                     uarfcn_dl;
  boolean                    pscr_present;      /* indicates rwhether PSCR info present */
  uint32                     pscr;
} rrcrb_directed_cell_info_type;

/* Define an invalid transaction id - This is used to initialize the local
transaction ids. This should not be a valid transaction id and should be
replaced by the first received transaction id for a message                */
#define RRCRB_INVALID_TRANSACTION_ID  10


/* Maximum RABs that can be contained in a Radio Bearer message. Although
this number can be larger, it's defined as 2 now. We support only one RAB
per message currently. */
#define RRCRB_MAX_RAB_PER_MSG 8

/* This define time in ms for which the rb procedure waits to send cell update
 * after sending the complete message, for scenarios where there has been 
   an RLC unrecoverable error on non srb2 during a pending configuration*/
#define RRCRB_PND_CFG_DELAY 120


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Structure to store transaction Ids. for any Radio Bearer Procedure      */
typedef struct
{
  uint8 accepted_transaction;   /* The latest accepted transaction id     */
  uint8 rejected_transaction;   /* The latest rejected transaction id     */
}rrcrb_transaction_id_type;  

/* Structure to store failure cause and protocol error cause for a msg     */
typedef struct
{
  rrcrb_msg_status_e_type failure_status; /* Failure cause                 */
  rrc_ProtocolErrorCause  prot_err_cause; /* Protocol Error cause          */
}rrcrb_msg_failure_cause_type;

/* For each Radio Bearer message, the following structure can hold
the information related to the RABs contained in that message till they
are processed.                                                             */
typedef struct
{
  uint32 num_rabs_in_msg;      /* Number of RABs contained in the message  */
  boolean chan_config_needed[RRCRB_MAX_RAB_PER_MSG]; /* TRUE if channel
                                                     config is needed for
                                                     this RAB              */
  rrc_est_rab_info_type rabs_in_msg[RRCRB_MAX_RAB_PER_MSG]; /* Info for each
                                                            RAB            */
}rrcrb_rab_info_per_msg_type;

/*Currently, this data structure is needed for RBE/IHO/CU procedure to remember whether the normal 
DCH voice call is being setup, whether CS OVER HSPA voice is being setup, or any transition of voice is happening.*/

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


#ifdef FEATURE_WCDMA_REL7_SPECRS
extern boolean rrc_change_of_ue_cap_response;
#endif
/* =======================================================================
**                        MACRO Declarations
** ======================================================================= */




#define RRCRB_R6_CRITICAL_EXT_HO_TO_UTRAN_PRESENT( \
  msg, msg_choice_prefix, r6_msg_choice_prefix) \
  ((T_##msg_choice_prefix##_criticalExtensions_4_criticalExtensions & \
          msg->u.criticalExtensions.t) && \
          (T_##msg_choice_prefix##_criticalExtensions_3_criticalExtensions == \
            msg->u.criticalExtensions.u.criticalExtensions.t) && \
            (T_##r6_msg_choice_prefix##_criticalExtensions_2_r6 == \
             msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))

#define RRCRB_R6_CRITICAL_EXT_PRESENT( \
  msg, msg_choice_prefix) \
  ((T_##msg_choice_prefix##criticalExtensions_5_criticalExtensions == \
          msg.u.later_than_r3.criticalExtensions.t) && \
          (T_##msg_choice_prefix##criticalExtensions_4_criticalExtensions == \
            msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
            (T_##msg_choice_prefix##criticalExtensions_3_r6 == \
             msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))

#define RRCRB_R6_UL_ADD_RECFG_TRCH_INFO_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.ul_AddReconfTransChInfoListPresent))

#define RRCRB_R6_UL_DELETED_TRCH_INFO_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.ul_deletedTransChInfoListPresent))

#define RRCRB_R6_UL_COMMON_TRCH_INFO_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.ul_CommonTransChInfoPresent))

#define RRCRB_R6_DL_ADD_RECFG_TRCH_INFO_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.dl_AddReconfTransChInfoListPresent))

#define RRCRB_R6_DL_DELETED_TRCH_INFO_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.dl_DeletedTransChInfoListPresent))

#define RRCRB_R6_DL_COMMON_TRCH_INFO_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.dl_CommonTransChInfoPresent))

#define RRCRB_R6_DL_COUNTER_SYNC_INFO_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.dl_CounterSynchronisationInfoPresent))

#define RRCRB_R6_SRB_INFO_SETUP_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.srb_InformationSetupListPresent))

#define RRCRB_R6_RAB_INFO_SETUP_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent))


#define RRCRB_R6_RB_INFO_RECONFIG_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent))

#define RRCRB_R6_RB_INFO_AFFECTED_LIST_PRESENT(msg_ptr, msg_type_prefix) \
        ((msg_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent))

#define RRCRB_R6_DL_INFO_PERRL_EXT_PRESENT(msg_ptr, msg_type_prefix,r6_msg_choice_prefix) \
  (  ( msg_ptr->m.v6b0NonCriticalExtensionsPresent) \
              && (msg_ptr->v6b0NonCriticalExtensions.r6_msg_choice_prefix##_v6b0ext.m.dl_InformationPerRL_List_v6b0extPresent )\
     )

#define RRCRB_R6_DL_INFO_PERRL_EXT_PRESENT_IN_CU(msg_ptr, msg_type_prefix,r6_msg_choice_prefix) \
  (  (   msg_ptr->message.u.r6_msg_choice_prefix.u.later_than_r3.criticalExtensions. \
            u.criticalExtensions.u.criticalExtensions.u.r6.m.v6b0NonCriticalExtensionsPresent) \
              && ( msg_ptr->message.u.r6_msg_choice_prefix.u.later_than_r3.criticalExtensions. \
            u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.r6_msg_choice_prefix##_v6b0ext.m.dl_InformationPerRL_List_v6b0extPresent )\
     )

#define RRCRB_R7_CRITICAL_EXT_HO_TO_UTRAN_PRESENT( \
  msg, msg_choice_prefix,r7_msg_choice_prefix) \
  ((T_##msg_choice_prefix##_criticalExtensions_4_criticalExtensions == \
          msg->u.criticalExtensions.t) && \
          (T_##msg_choice_prefix##_criticalExtensions_3_criticalExtensions == \
            msg->u.criticalExtensions.u.criticalExtensions.t) && \
            (T_##r7_msg_choice_prefix##_criticalExtensions_2_criticalExtensions == \
             msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
            (T_##r7_msg_choice_prefix##_criticalExtensions_1_r7 == \
             msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))


#define RRCRB_R7_CRITICAL_EXT_PRESENT( \
  msg, msg_choice_prefix) \
    ((T_##msg_choice_prefix##criticalExtensions_5_criticalExtensions == \
                  msg.u.later_than_r3.criticalExtensions.t) && \
              (T_##msg_choice_prefix##criticalExtensions_4_criticalExtensions == \
                    msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
                (T_##msg_choice_prefix##criticalExtensions_3_criticalExtensions == \
                    msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
                (T_##msg_choice_prefix##criticalExtensions_2_r7 & \
                 msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))

#define RRCRB_R7_CRITICAL_EXT_PRESENT_PTR( \
  msg, msg_choice_prefix) \
  ((T_##msg_choice_prefix##_##criticalExtensions_5_criticalExtensions == \
                  msg->u.later_than_r3.criticalExtensions.t) && \
              (T_##msg_choice_prefix##_##criticalExtensions_4_criticalExtensions == \
                    msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
                (T_##msg_choice_prefix##_##criticalExtensions_3_criticalExtensions == \
                    msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
                (T_##msg_choice_prefix##_##criticalExtensions_2_r7 & \
                 msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))
             
#define RRCRBR_R7_MSG_IE_PRESENT(\
    msg,msg_type_prefix,msg_bitmask_prefix)\
    ((msg)->m.msg_bitmask_prefix##Present)

#define RRCRB_UL_DPCH_POWER_CONTROL_INFO_PRESENT(msg_ptr, rel_version) \
        ((msg_ptr->bit_mask) & \
        (rrc_UL_DPCH_Info_r##rel_version##_ul_DPCH_PowerControlInfo_present))

#define RRCRB_UL_DPCH_MODESPECIFIC_INFO(msg_ptr, rel_version) \
        ((msg_ptr->modeSpecificInfo.choice ) & \
        (rrc_UL_DPCH_Info_r##rel_version##_modeSpecificInfo_fdd_chosen))
        

/*===================================================================
MACRO       :   RRCRB_VALIDATE_SRNS_RELCOATION_R6


DESCRIPTION :   This MACRO checks whether SRNS relocation info is present                                                  
                                                                        
DEPENDENCIES:   None

RETURN VALUE:   None.

SIDE EFFECTS:   None.

=====================================================================*/
#define RRCRB_VALIDATE_SRNS_RELCOATION_R6(proc_id, msg_type_prefix1, \
                                          msg_type_prefix2, srns_reloc) \
{ \
 \
  if (proc_id == RRC_PROCEDURE_RBE) \
  { \
    if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u. \
         criticalExtensions.u.r6.radioBearerSetup_r6.specificationMode.choice & \
         rrc_RadioBearerSetup_r6_IEs_specificationMode_complete_chosen) &&  \
        (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u. \
         criticalExtensions.u.r6.radioBearerSetup_r6.specificationMode.complete.bitmask & \
         rrc_complete_dl_CounterSynchronisationInfo_present)) \
      { \
        srns_reloc = TRUE; \
    } \
  } \
  else if  (proc_id == RRC_PROCEDURE_RBRC)\
  { \
    if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions. \
         u.msg_type_prefix1##_r6.bitmask & rrc_##msg_type_prefix2##_r6_IEs_new_U_RNTI_present)) \
      { \
        srns_reloc = TRUE; \
    } \
  }\
  else \
  { \
    if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u. \
         criticalExtensions.u.rrc_##msg_type_prefix1##_r6.bitmask & \
         rrc_##msg_type_prefix2##_r6_IEs_dl_CounterSynchronisationInfo_present)) \
      { \
        srns_reloc = TRUE; \
    } \
  } \
}

			

#define RRCRB_R8_CRITICAL_EXT_PRESENT_PTR( \
  msg, msg_choice_prefix) \
  ((T_##msg_choice_prefix##_##criticalExtensions_5_criticalExtensions == \
				  msg->u.later_than_r3.criticalExtensions.t) && \
			  (T_##msg_choice_prefix##_##criticalExtensions_4_criticalExtensions == \
					msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
				(T_##msg_choice_prefix##_##criticalExtensions_3_criticalExtensions == \
					msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
					(T_##msg_choice_prefix##_##criticalExtensions_2_criticalExtensions == \
					msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
				(T_##msg_choice_prefix##_##criticalExtensions_1_r8 & \
				 msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))

#define RRCRB_R8_CRITICAL_EXT_PRESENT( \
  msg, msg_choice_prefix) \
  ((T_##msg_choice_prefix##criticalExtensions_5_criticalExtensions == \
				  msg.u.later_than_r3.criticalExtensions.t) && \
			  (T_##msg_choice_prefix##criticalExtensions_4_criticalExtensions == \
					msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
				(T_##msg_choice_prefix##criticalExtensions_3_criticalExtensions == \
					msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
					(T_##msg_choice_prefix##criticalExtensions_2_criticalExtensions == \
					msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
				(T_##msg_choice_prefix##criticalExtensions_1_r8 & \
				 msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))
				 

#define RRCRB_R8_CU_CRITICAL_EXT_PRESENT_PTR(msg_ptr, cu_msg_prefix)  \
            ((T_##cu_msg_prefix##_later_than_r3 == \
            msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.t) && \
            (T_##cu_msg_prefix##_criticalExtensions_4_criticalExtensions == \
            msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
            (T_##cu_msg_prefix##_criticalExtensions_3_criticalExtensions == \
             msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) \
             && (T_##cu_msg_prefix##_criticalExtensions_2_criticalExtensions == \
             msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) \
             && (T_##cu_msg_prefix##_criticalExtensions_1_r8 == \
             msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))

#define RRCRB_R8_ASU_CRITICAL_EXT_PRESENT_PTR \
  ((T_rrc_ActiveSetUpdate_criticalExtensions_3_criticalExtensions == \
  dcch_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.t) \
  && (T_rrc_ActiveSetUpdate_criticalExtensions_2_criticalExtensions == \
  dcch_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) \
  && (T_rrc_ActiveSetUpdate_criticalExtensions_1_r8 == \
  dcch_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)) 

#define RRCRB_R8_CRITICAL_EXT_HO_TO_UTRAN_PRESENT( \
  msg, msg_choice_prefix, r8_msg_choice_prefix) \
            ((T_##msg_choice_prefix##_criticalExtensions_4_criticalExtensions == \
            msg->u.criticalExtensions.t) && \
            (T_##msg_choice_prefix##_criticalExtensions_3_criticalExtensions ==  \
            msg->u.criticalExtensions.u.criticalExtensions.t) && \
            (T_##r8_msg_choice_prefix##_criticalExtensions_2_criticalExtensions == \
            msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
            (T_##r8_msg_choice_prefix##_criticalExtensions_1_criticalExtensions == \
            msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) && \
            (T_##r8_msg_choice_prefix##_criticalExtensions_r8 == \
            msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t))

/*The following macro is added for more readability for R8 MSG IEs.*/
#define RRCRB_R8_MSG_IE_PRESENT(\
    msg,msg_type_prefix,msg_bitmask_prefix)\
    (msg->m.msg_bitmask_prefix##Present )

#ifdef FEATURE_WCDMA_REL9


#define RRCRB_R9_CRITICAL_EXT_PRESENT_PTR( \
  msg, msg_choice_prefix) \
  ((T_##msg_choice_prefix##_##criticalExtensions_5_criticalExtensions == \
				  msg->u.later_than_r3.criticalExtensions.t) && \
			  (T_##msg_choice_prefix##_##criticalExtensions_4_criticalExtensions == \
					msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
				(T_##msg_choice_prefix##_##criticalExtensions_3_criticalExtensions == \
					msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
					(T_##msg_choice_prefix##_##criticalExtensions_2_criticalExtensions == \
					msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
				(T_##msg_choice_prefix##_##criticalExtensions_1_later_r8_criticalExtensions & \
				 msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
				 (T_##msg_choice_prefix##_##later_r8_criticalExtensions_r9 & \
				 msg->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t))

#define RRCRB_R9_CRITICAL_EXT_PRESENT( \
  msg, msg_choice_prefix) \
  ((T_##msg_choice_prefix##criticalExtensions_5_criticalExtensions == \
				  msg.u.later_than_r3.criticalExtensions.t) && \
			  (T_##msg_choice_prefix##criticalExtensions_4_criticalExtensions == \
					msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
				(T_##msg_choice_prefix##criticalExtensions_3_criticalExtensions == \
					msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
					(T_##msg_choice_prefix##criticalExtensions_2_criticalExtensions == \
					msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
				(T_##msg_choice_prefix##criticalExtensions_1_later_r8_criticalExtensions & \
				 msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
				 (T_##msg_choice_prefix##later_r8_criticalExtensions_r9 & \
				 msg.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t))
				 

#define RRCRB_R9_CU_CRITICAL_EXT_PRESENT_PTR(msg_ptr, cu_msg_prefix)  \
            ((T_##cu_msg_prefix##_later_than_r3 == \
            msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.t) && \
            (T_##cu_msg_prefix##_criticalExtensions_4_criticalExtensions == \
            msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) && \
            (T_##cu_msg_prefix##_criticalExtensions_3_criticalExtensions == \
             msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) \
             && (T_##cu_msg_prefix##_criticalExtensions_2_criticalExtensions == \
             msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) \
             && (T_##cu_msg_prefix##_criticalExtensions_1_later_r8_criticalExtensions == \
             msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) \
             && (T_##cu_msg_prefix##_later_r8_criticalExtensions_r9 == \
             msg_ptr->message.u.cellUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t))




#define RRCRB_R9_ASU_CRITICAL_EXT_PRESENT_PTR \
  ((T_rrc_ActiveSetUpdate_criticalExtensions_3_criticalExtensions == \
  dcch_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.t) \
  && (T_rrc_ActiveSetUpdate_criticalExtensions_2_criticalExtensions == \
  dcch_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.t) \
  && (T_rrc_ActiveSetUpdate_criticalExtensions_1_later_r8_criticalExtensions == \
  dcch_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) \
  &&(T_rrc_ActiveSetUpdate_later_r8_criticalExtensions_r9 == \
  dcch_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t)) 

#define RRCRB_R9_CRITICAL_EXT_HO_TO_UTRAN_PRESENT( \
  msg, msg_choice_prefix, r9_msg_choice_prefix) \
            ((T_##msg_choice_prefix##_criticalExtensions_4_criticalExtensions == \
            msg->u.criticalExtensions.t) && \
            (T_##msg_choice_prefix##_criticalExtensions_3_criticalExtensions ==  \
            msg->u.criticalExtensions.u.criticalExtensions.t) && \
            (T_##r9_msg_choice_prefix##_criticalExtensions_2_criticalExtensions == \
            msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
            (T_##r9_msg_choice_prefix##_criticalExtensions_1_criticalExtensions == \
            msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t) && \
            (T_##r9_msg_choice_prefix##_criticalExtensions_later_r8_criticalExtensions == \
            msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)&& \
            (T_##r9_msg_choice_prefix##_later_r8_criticalExtensions_r9 == \
            msg->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t))

/*The following macro is added for more readability for R9 MSG IEs.*/
#define RRCRB_R9_MSG_IE_PRESENT(\
    msg,msg_bitmask_prefix)\
    (msg->m.msg_bitmask_prefix##Present )

#endif /*FEATURE_WCDMA_REL9*/

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*============================================================================
FUNCTION: rrcllc_validate_rb_info_reconfig_list_r6()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfigList_r6 and return
  SUCCESS or FAILURE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_validate_rb_info_reconfig_list_r6
(
  rrc_RB_InformationReconfigList_r6 *rrc_rb_information_reconfig_list
);


/*===========================================================================

FUNCTION   RRCRB_SEND_RB_PROC_COMPL_IND

DESCRIPTION

  This function is called by all RB procedures and it sends RRC_RB_PROC_COMPL_IND to 
  CELL UPDATE procedure.
  
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.
 
SIDE EFFECTS

  None.

===========================================================================*/
void rrcrb_send_rb_proc_compl_ind(void);



#ifdef FEATURE_UMTS_PDCP
/*===========================================================================

FUNCTION   RRCRB_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrb_is_srns_in_progress
( 
void
);
#endif
/*===========================================================================

FUNCTION   rrcrb_is_cu_required()

DESCRIPTION
 This function returns TRUE if cell update is required after
 the reconfig msg.  Else returns FALSE
      
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrb_is_cu_required
(
  void
);
/*===========================================================================

FUNCTION   RRCRB_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrb_get_new_crnti_valid
(
  void
);


/*===========================================================================

FUNCTION   RRCRB_UPDATE_MAC_RAB_STATUS

DESCRIPTION

  This function updates variable mac_rab_status. Parameters to this func 
  are proc id, which identifies the calling procedure, cn_domain, used by RBE
  to identify which domain RABS are being setup for, RB_InformationReleaseList,
  used by RBE and CU procedures, and boolean go_back_to_old_status, which
  indicates to update mac_rab_status from scratch by looking into est_rabs only.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrb_update_mac_rab_status
(
  rrc_proc_e_type proc_id,
  rrc_cn_domain_identity_e_type cn_domain,
  rrc_RB_InformationReleaseList  *rel_list_ptr,
  boolean go_back_to_old_status
);
/*===========================================================================
FUNCTION   rrc_query_cs_data_status

DESCRIPTION
This function will be called by MAC at configuration time to determine
if it is a CS only or PS only call. Based on this MAC will determine how
the AMR rate changes need to be applied based on the power.

DEPENDENCIES
  None.

RETURN VALUE
Function returns if the call is of type CS only, PS only or CS+PS call.

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_query_cs_data_status
(
  void
);
/*===========================================================================
FUNCTION   rrc_query_ps_zero_tb_size_status

DESCRIPTION
This function will be called by MAC at configuration time to determine
if it is a CS only or PS only call. Based on this MAC will determine how
the AMR rate changes need to be applied based on the power.

DEPENDENCIES
  None.

RETURN VALUE
Function returns if the call is of type CS only, PS only or CS+PS call.

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_query_ps_zero_tb_size_status
(
  void
);
/*===========================================================================

FUNCTION          RRCRB_TRANSLATE_GSM_MAP_RAB_ID


DESCRIPTION       

  This function translates a GSM MAP RAB Id type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  None
  
RETURN VALUE
                  
  The GSM MAP RAB Id converted to the internal type rrc_rab_id_type

SIDE EFFECTS
                  
  None

===========================================================================*/
rrc_rab_id_type rrcrb_translate_gsm_map_rab_id
(
  ASN1BitStr32  *ext_rab_id         /* Pointer to external RAB ID */
);


 /*===========================================================================

FUNCTION rrcrb_validate_directed_cell_info

DESCRIPTION
  This function validates the directed cell info and returns a boolean
  that indicates whether a cellupdate is required or not. 

DEPENDENCIES
  None.

RETURN VALUE
  Boolean TRUE  : Cell update is required.
          FALSE : Cell update is not required required.

===========================================================================*/
boolean  rrcrb_validate_directed_cell_info
(
  rrcrb_directed_cell_info_type  directed_cell_info
);

 /*===========================================================================

FUNCTION rrcrb_send_rnti_update_req

DESCRIPTION
  This function sends the RNTI Update request to LLC to update
  with RNTIs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void  rrcrb_send_rnti_update_req
(
   rrcrb_rnti_update_info_type  rnti_update_info
);

/*===========================================================================

FUNCTION rrcrb_send_rlc_re_est_req_to_smc

DESCRIPTION
  This function sends the rlc_reestablishment request to 
  SMC procedure when reconfig message has SRNS relocation info
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_rlc_re_est_req_to_smc
(
  rrc_proc_e_type rrc_proc,
  rrc_rlc_re_est_for_srns_rb_e_type rb_type,
  boolean cnf_reqd
);
 /*===========================================================================

FUNCTION          RRCRB_SEND_L1_DRX_REQ

DESCRIPTION       This function is responsible for sending CPHY_DRX_REQ
                  to L1.

DEPENDENCIES
                  

RETURN VALUE
                  Void
SIDE EFFECTS      None.
===========================================================================*/
void rrcrb_send_l1_drx_req
(
  rrc_proc_e_type rrc_proc
);


 /*===========================================================================

FUNCTION          rrcrb_send_upd_ota_parms_ind

DESCRIPTION       This function is responsible for sending CPHY_UPD_OTA_PARMS_IND to L1.

DEPENDENCIES
                  
RETURN VALUE
                  
SIDE EFFECTS      None.
===========================================================================*/
void rrcrb_send_upd_ota_parms_ind
(
  rrc_proc_e_type rrc_proc, 
  uint8 bit_mask_ind
);

/*===========================================================================

FUNCTION rrcrb_send_cell_update_initiate_req

DESCRIPTION
  This function sends the cell update initiate request 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_cell_update_initiate_req
(
  rrc_proc_e_type              procedure,
  rrc_cell_update_cause_e_type cause,
  boolean                      cnf_reqd
);

/*===========================================================================

FUNCTION rrcrbrc_send_ura_update_initiate_req

DESCRIPTION
  This function sends the ura update initiate request 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_ura_update_initiate_req
(
  rrc_proc_e_type              procedure,
  rrc_ura_update_cause_e_type cause,
  boolean                      cnf_reqd
);

/*===========================================================================

FUNCTION rrcrb_send_initiate_cell_selection_req

DESCRIPTION
  This function send the cell selection initiate request.
DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_initiate_cell_selection_req
(
   rrc_proc_e_type              procedure,
   rrc_cell_sel_cause_e_type    cause,
   rrc_state_e_type             dest_state
);


/*===========================================================================

FUNCTION   RRCRB_REGISTER_WITH_RLC_FOR_UL_DATA_IND

DESCRIPTION

  This function sends a command to RLC to register for uplink data indication
  when UE moves to CELL_PCH/URA_PCH state. It always indicates that registration
  is required for signalling RBs. To include user plane RBs it looks 
  at established_rabs variable 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrb_register_with_rlc_for_ul_data_ind
(
  rrc_proc_e_type              procedure
);

/*===========================================================================

FUNCTION rrcrb_get_proc_sub_state

DESCRIPTION
  This function returns the respective procedure substate.
  
DEPENDENCIES
  None.

RETURN VALUE
  rrcrb_substate_e_type proc_sub_state.

===========================================================================*/
rrcrb_substate_e_type rrcrb_get_proc_sub_state
(
  rrc_proc_e_type rrc_proc
);


/*===========================================================================
FUNCTION rrcrb_update_amr_cctrch_id_for_reconfig
DESCRIPTION
DEPENDENCIES
  None.
RETURN VALUE
  rrcrb_substate_e_type proc_sub_state.
===========================================================================*/
#ifdef FEATURE_VOC_AMR_WB
  void rrcrb_update_amr_cctrch_id_for_reconfig
( 
  mvs_mode_type codec_type 
);
#else/*FEATURE_VOC_AMR_WB*/
void rrcrb_update_amr_cctrch_id_for_reconfig
( 
  void 
);
#endif/*FEATURE_VOC_AMR_WB*/

/*====================================================================
FUNCTION       rrcrb_get_mac_d_hfn()

DESCRIPTION    This function extract mac_d_hfn field from mac_d_hfn_intial_value_ptr
               if "mac_d_hfn_intial_value_present" indicates TRUE.

REUSABILITY: R5/R6
DEPENDENCIES   None

RETURN VALUE   value of mac_d_hfn if present else, invalid value

====================================================================*/

uecomdef_status_e_type  rrcrb_get_mac_d_hfn
(
  OSS_UINT32 mac_d_hfn_intial_value_present,
  ASN1BitStr32 *mac_d_hfn_intial_value_ptr,
  uint32 *mac_d_val
);

/*===========================================================================
FUNCTION rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch()

DESCRIPTION
  This function fills chan_config_req with RBs in rrc_rb_list_mapped_to_dch 
  if they are not already present in chan_config_req.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch
(
  uint32 *rb_count,
  rrc_channel_config_req_type *chan_config_req
);

/*===========================================================================

FUNCTION rrc_fill_rb_id_chan_req_from_est_rabs()

DESCRIPTION
  This function fills existing SRBs and PS RBs in chan_config_req if they are 
  already not part of chan_config_req.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_fill_rb_id_chan_req_from_est_rabs
(
  uint32 *rb_count,
  rrc_channel_config_req_type *chan_config_req
);
/*===========================================================================

FUNCTION rrc_fill_srb_in_chan_config_req()

DESCRIPTION
  This function fills SRB's in chan_config_req.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_fill_srb_in_chan_config_req
(
  uint32 *rb_count,
  rrc_channel_config_req_type *chan_config_req
);

/*===========================================================================

FUNCTION   rrc_fill_rb_id_chan_req_from_standalone_db

DESCRIPTION
  Function to Extract the RB affected list based on Current and Ordered 
    Standalone database

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rrc_fill_rb_id_chan_req_from_standalone_db
(
  uint32 *rb_count,
  rrc_channel_config_req_type *chan_config_req
);

/*===========================================================================

FUNCTION rrc_update_rab_info_reconfig_param

DESCRIPTION


  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/
rrcrb_msg_status_e_type 
rrc_update_rab_info_reconfig_param
(
  rrc_RAB_InformationReconfigList *rab_list_ptr,
  rrcrb_rab_info_per_msg_type   * rabs_info
#ifdef FEATURE_VOC_AMR_WB
  ,mvs_mode_type * nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/

);


/*============================================================================
FUNCTION: rrcllc_validate_rb_info_reconfig_list_r7()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfigList_r7 and return
  SUCCESS or FAILURE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_validate_rb_info_reconfig_list_r7
(
  rrc_RB_InformationReconfigList_r7 *rrc_rb_information_reconfig_list
);

/*===========================================================================

FUNCTION rrcrb_check_if_proc_waiting_for_new_cell_ind


DESCRIPTION
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/

boolean rrcrb_check_if_proc_waiting_for_new_cell_ind
(
  rrc_proc_e_type *                proc_id        
);

#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/* FEATURE_SCUDIF_ENHANCEMENT*/

/*===========================================================================

FUNCTION rrc_update_reconfig_waiting_for_l2ack_dch

DESCRIPTION
Updates the procedure waiting for L2 Ack for the complete msg for the reconfig  in DCH  state

  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/     
void rrc_update_reconfig_waiting_for_l2ack_dch
(
    rrc_proc_e_type proc_id
);

/*===========================================================================

FUNCTION rrc_check_any_proc_waiting_for_l2_ack

DESCRIPTION
Determines, if any procedure is waiting for the L2 ACk msg for the
reconfig  complete msg sent in DCH

  
DEPENDENCIES


RETURN VALUE
  TRUE: Some prcocedure is waiting for l2 ack
  else FALSE
 

SIDE EFFECTS

  None

===========================================================================*/
boolean rrc_check_any_proc_waiting_for_l2_ack
(
  void
);

/*===========================================================================
FUNCTION rrc_update_rab_info_reconfig_param_r8

DESCRIPTION
  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS
  None
===========================================================================*/
rrcrb_msg_status_e_type rrc_update_rab_info_reconfig_param_r8
(
  rrc_RAB_InformationReconfigList_r8 *rab_list_ptr,
  rrcrb_rab_info_per_msg_type   * rabs_info
#ifdef FEATURE_VOC_AMR_WB
  ,mvs_mode_type * nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
);

#ifdef FEATURE_UMTS_PDCP
/*============================================================================
FUNCTION: rrcllc_validate_pdcp_info_in_rb_info_reconfig_list_r8()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfig_r8 for PDCP Info return
  TRUE or FALSE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_validate_pdcp_info_in_rb_info_reconfig_list_r8
(
  rrc_RB_InformationReconfig_r8 * rb_reconfig_ptr
);

/*============================================================================
FUNCTION: rrcllc_validate_pdcp_info_in_rb_info_setup_list_r8()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationSetupList_r8 for PDCP Info return
  TRUE or FALSE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_validate_pdcp_info_in_rb_info_setup_list_r8
(
  rrc_RB_InformationSetup_r8 * rb_setup_ptr
);
#endif /*FEATURE_UMTS_PDCP*/

/*============================================================================
FUNCTION: rrcllc_validate_rb_info_reconfig_list_r8()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfigList_r8 and return
  SUCCESS or FAILURE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_validate_rb_info_reconfig_list_r8
(
  rrc_RB_InformationReconfigList_r8 *rrc_rb_information_reconfig_list
);

/*============================================================================
FUNCTION: rrcrb_proc_waiting_for_l2_ack()

DESCRIPTION:
  This function checks is the procedure that set OC is waiting for L2 ACK
  This function will check for RB procedures only

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE is OC set procedure is waiting for L2 ACK
  FALSE otherwise

SIDE EFFECTS:
=============================================================================*/
boolean rrcrb_proc_waiting_for_l2_ack
(
  rrc_proc_e_type procedure
);
/*============================================================================
FUNCTION: rrcrb_proc_waiting_for_l2_ack()

DESCRIPTION:
  This function checks if CU is going to be triggeed or not

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:
=============================================================================*/
boolean rrcrb_is_cu_going_to_be_triggered
(
  rrc_proc_e_type procedure
);
#endif /* RRCRBCOMMON_H */

