#ifndef TDSRRCDATA_I_H
#define TDSRRCDATA_I_H
/*===========================================================================
                      R R C   D A T A   I N T E R N A L   H E A D E R

DESCRIPTION

  This module contains the internal header information for the RRC
  Data and other module


Copyright (c) 2010-2012 Qualcomm Technologies, Incorporated. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrcdata.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/12   mkh     Reverting prev changes (CL: 2199119)
02/10/12   mkh     Changes to Return the current DL and UL data rate based on 
                   RRC internal stored category. On behalf of JZ.
10/20/11   dhf    Clean up redundant include file
08/25/11   dhf    Split from tdsrrcdata.h
===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */   
#include "tdscdma_variation.h"
#include "tdsrrcasn1.h"
#include "tdsrrcdataext.h"
#include "tdsrrcmm_v.h"
#include "tdsmacrrcif.h"
#include "tdsrlcrrcif.h"
#include "tdsrrcdata_v.h"

#ifdef FEATURE_MVS
#include <mvsamr.h>
#endif /* FEATURE_MVS */
#include <mvs.h>

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define TDSNUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE 0x03/* Number of bits for DL CCCH
                                             Message type  */

/* MACROs to be used in RRC to set global variable tdsrrc_nv_rel_indicator */
#define TDSINVALID_DL_AM_PDU_RLC_SIZE 0xffffffff

#define TDSRRC_NV_REL_INDICATOR_REL9 0x10
#define TDSRRC_NV_REL_INDICATOR_REL8 0x8
#define TDSRRC_NV_REL_INDICATOR_REL7 0x4
#define TDSRRC_NV_REL_INDICATOR_REL6 0x2
#define TDSRRC_NV_REL_INDICATOR_REL5 0x1
#define TDSRRC_NV_REL_INDICATOR_REL4 0x0

#define TDSRRC_INVALID_RAB_ID (MAX_RAB_TO_SETUP+1) /* Invalid RAB id
                               used in ESTABLISHED_RABS to indicate
                               an unused RAB slot                    */ 

#define TDSRRC_INVALID_RB_ID  TDS_MAX_RB /* Invalid RAB id used in the
                               variable ESTABLISHED_RABS to indicate
                               an unused RB slot                     */

#ifdef __NON_DEMAND_PAGED_FUNCTION__ 
#define __TDS_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ __NON_DEMAND_PAGED_FUNCTION__ 
#else
#define __TDS_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
#endif

#ifdef __NON_DEMAND_PAGED_FUNCTION_END__
#define __TDS_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ __NON_DEMAND_PAGED_FUNCTION_END__
#else
#define __TDS_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
#endif

#define TDSNUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT 0x01
                                          /* Number of bits for integrity
                                                                                                         Check Info present */

#define TDSNUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO 0x24
                                          /* Number of bits for integrity
                                                                                                         Check Info */

/* The following Bit values for December version of 25.331 specification. 
   If ASN1 for future revisions changes, then these values needs to be 
   updated accordingly */
#define TDSINTEGRITY_CHECK_INFO_PRESENT_POS 0x00 
                                          /* Bit position for integrity 
                                             Check info present */


/*! @brief Macros for remaining free memory in DSM small item pool
*/
#define TDSRRC_REM_SMALL_ITEM_POOL_SIZE (DSM_DS_SMALL_ITEM_SIZ * DSM_DS_SMALL_ITEM_CNT)

/*! @brief DSM Pool to use
*/
#define TDSRRC_DSM_POOL_ID(size) ((size <= TDSRRC_REM_SMALL_ITEM_POOL_SIZE) ? \
                                      (DSM_DS_SMALL_ITEM_POOL) : (DSM_DS_LARGE_ITEM_POOL))

#ifdef FEATURE_TDSCDMA_DSDS
extern rex_tcb_type tds_rrc_tcb;  // for Triton compilation only, TBD
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* This defines the Cell ID as a binary number after decoding the 
 * link list format in ASN1
 */
typedef  uint32  tdsrrc_int_cell_id_type;

typedef  uint32  tdsrrc_int_ura_id_type;

typedef  uint32  tdsrrc_int_u_rnti_type;

typedef  uint32  tdsrrc_int_c_rnti_type;


typedef struct {
  uint8 channel_type;                       /* Variable for channel type */
  uint8 msg_type;                       /* Variable for message type */
  uint32 transactionid;                       /* Variable for transaction id */
  uint8 direction;
  uint8 *raw_sdu;
  uint32 raw_sdu_length;
  log_tdscdma_trch_type transport_channel_type;

} tdslog_message_type;

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


/* Used in tdsrlc_size_per_rb_struct_type to indicate DL RLC AM PDU size of DL RLC LI sIZE */
typedef enum 
{
  TDSRRC_DL_AM_RLC,
  TDSRRC_DL_UM_RLC,
  TDSRRC_DL_INVALID_RLC
}tdsrrc_rlc_dl_entity_e_type;

typedef enum 
{
  TDSRRC_DL_UM_RLC_LI_SIZE_7, /* 7 bits */
  TDSRRC_DL_UM_RLC_LI_SIZE_15, /* 15 Bits */
  TDSRRC_DL_UM_INVALID_RLC
}tdsrrc_rlc_dl_um_li_e_type;

typedef enum
{
  TDSRRC_PROCEDURE_NONE =0x00,                      /* None RRC Procedure. */
  TDSRRC_PROCEDURE_CSP =0x01,                       /* Cell Selection Procedure. */
  TDSRRC_PROCEDURE_CCM=0x02,                       /* Cell Change Manager */
  TDSRRC_PROCEDURE_SIB=0x03,                       /* SIB Processing Procedure. */
  TDSRRC_PROCEDURE_PG1=0x04,                       /* Page Type 1 Procedure. */
  TDSRRC_PROCEDURE_RCE=0x05,                       /* RRC Connection Establishment 
                                              Procedure. */
  TDSRRC_PROCEDURE_RCR=0x06,                       /* RRC Connection Release 
                                              Procedure. */
  TDSRRC_PROCEDURE_RCRE=0x07,                      /* RRC Connection Re-establishment
                                              Procedure. */
  TDSRRC_PROCEDURE_UECI=0x08,                      /* UE Capability Information 
                                              Procedure. */
  TDSRRC_PROCEDURE_UECE=0x09,                      /* UE Capability Enquiry 
                                              Procedure. */
  TDSRRC_PROCEDURE_IDT=0x0A,                       /* Initial Direct Transfer
                                              Procedure. */
  TDSRRC_PROCEDURE_UDT=0x0B,                       /* Uplink Direct Transfer
                                              Procedure. */
  TDSRRC_PROCEDURE_DDT=0x0C,                       /* Downlink Direct Transfer
                                              Procedure. */
  TDSRRC_PROCEDURE_PG2=0x0D,                       /* Paging Type 2 Procedure. */
  TDSRRC_PROCEDURE_SMC=0x0E,                       /* Security Mode Control
                                              Procedure. */
  TDSRRC_PROCEDURE_SCR=0x0F,                       /* Signalling Connection Release
                                              Procedure. */
  TDSRRC_PROCEDURE_SCRR=0x10,                      /* Signalling Connection Release
                                              Request Procedure. */
  TDSRRC_PROCEDURE_CC=0x11,                        /* Counter Check Procedure. */
  TDSRRC_PROCEDURE_RBE=0x12,                       /* Radio Bearer Establishment
                                              Procedure. */
  TDSRRC_PROCEDURE_RBRC=0x13,                      /* Radio Bearer Re-configuration
                                              Procedure. */
  TDSRRC_PROCEDURE_RBR=0x14,                       /* Radio Bearer Release 
                                              Procedure. */
  TDSRRC_PROCEDURE_TCR=0x15,                       /* Transport Channel
                                              Re-configuration Procedure. */
  TDSRRC_PROCEDURE_TFC=0x16,                       /* Transport Format Combination
                                              Control Procedure. */
  TDSRRC_PROCEDURE_PCR=0x17,                       /* Physical Channel 
                                              Re-configuration Procedure. */
  TDSRRC_PROCEDURE_CU=0x18,                        /* Cell Update Procedure. */
  TDSRRC_PROCEDURE_UU=0x19,                        /* URA Update Procedure. */
  TDSRRC_PROCEDURE_UMI=0x20,                       /* UTRAN Mobility Information
                                              Procedure. */  
  TDSRRC_PROCEDURE_ISHTU=0x21,                     /* Inter-System Handover to 
                                              UTRAN. */
  TDSRRC_PROCEDURE_ISHFU=0x22,                     /* Inter-System Handover from
                                              UTRAN. */
  TDSRRC_PROCEDURE_ISCTU=0x23,                     /* Inter-System Cell Reselection
                                              to UTRAN. */
  TDSRRC_PROCEDURE_ISCFU=0x24,                     /* Inter-System Cell Reselection
                                              from UTRAN. */
  TDSRRC_PROCEDURE_MCMR=0x25,                      /* Measurement Control and 
                                              Measurement Report Procedure. */
  TDSRRC_PROCEDURE_LBT=0x26,                       /* Loopback Test module */

  TDSRRC_PROCEDURE_MCM=0x27,                       /* Mode Control module */

  TDSRRC_PROCEDURE_IHO=0x28,                       /* Inter-RAT handover module */
  TDSRRC_PROCEDURE_LLC=0x29,                       /* Lower Layer Controller */
  TDSRRC_PROCEDURE_CHO=0x2A,                       /* Cell Change Order */
  TDSRRC_PROCEDURE_CHK=0x2B,                       /* RRC support for Counter Check procedure */ 
  TDSRRC_PROCEDURE_MISC=0x2C,  
#ifdef FEATURE_TDSCDMA_GPS_CELL_ID_READING
  TDSRRC_PROCEDURE_GPS=0x2D,
#endif/*FEATURE_TDSCDMA_GPS_CELL_ID_READING*/
#ifdef FEATURE_TDSCDMA_REL7_SPECRS
  TDSRRC_PROCEDURE_COUEC=0x2E,                    /*Change of UE Capability Procedure.*/ 
#endif


  TDSRRC_PROCEDURE_DORMANCY=0x2F,

  TDSRRC_PROCEDURE_UPCC=0x30,                        /* Uplink Physical Channel Control Procedure */
  TDSRRC_PROCEDURE_QSH=0x31,                      /* QSH Debug Support Procedure */
  TDSMAX_NUM_OF_PROCEDURES                    /* Total number of RRC 
                                          Procedures. */
}tdsrrc_proc_e_type;

/* Enumerated type to give results of removal of an RB from 
the variable ESTABLISHED_RABS                                        */
typedef enum 
{
  TDSRRC_RB_NOT_FOUND, /* The given RB was not found in ESTABLISHED_RABS */
  TDSRRC_RB_REMOVED,   /* The given RB was removed. */
  TDSRRC_RB_AND_RAB_REMOVED /* The RB was removed, and due to that a RAB
                         was also removed (it had no other RBs mapped
                         to it */
}tdsrrc_rb_remove_e_type;

/* Indicates generic success or failure from a function */

#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
typedef enum
{
  TDSRRC_CS_VOICE_OVER_DCH,
  TDSRRC_CS_VOICE_OVER_HSPA,
  TDSRRC_CS_VOICE_OVER_INVALID
} tdsrrc_cs_voice_e_type;
#endif /*FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA*/



/* Enumerated type to give results of searching for a RB in
the variable ESTABLISHED_RABS                                        */
typedef enum 
{
  TDSRRC_RB_NOT_PRESENT,
  TDSRRC_RB_PRESENT
}tdsrrc_rb_search_e_type;




/* MESSAGE VERSION. */
typedef enum
{
  TDSMSG_REL99,
  TDSMSG_REL4,
  TDSMSG_REL5,
  TDSMSG_REL6,
  TDSMSG_REL7,
#ifdef FEATURE_TDSCDMA_REL8
  TDSMSG_REL8,
#endif /*FEATURE_TDSCDMA_REL8*/
#ifdef FEATURE_TDSCDMA_REL9
  TDSMSG_REL9,
#endif /* FEATURE_TDSCDMA_REL9 */
  TDSMSG_INVALID
} tdsrrc_msg_ver_enum_type;



/* Enumerated type of System Specific Capability Update Requirement */

typedef enum {
  TDSRRC_GSM_CAP
  ,TDSRRC_GERANIU_CAP
#ifdef FEATURE_TDSCDMA_REL8
  ,TDSRRC_EUTRA_CAP
#endif /*FEATURE_TDSCDMA_REL8*/
} tdsrrc_system_specific_cap_update_req_e_type;


typedef enum
{
  TDSRRC_STATE_DISCONNECTED,
  TDSRRC_STATE_CONNECTING,
  TDSRRC_STATE_CELL_FACH,
  TDSRRC_STATE_CELL_DCH,
  TDSRRC_STATE_CELL_PCH,
  TDSRRC_STATE_URA_PCH,
  TDSRRC_STATE_WILDCARD,   /* This is used only for the purposes of state
                        change notification. When a procedure needs to
                        receive state change notification for a transition
                        from any state or to any state, it needs to use
                        this wild card state while registering with state
                        change manager. */
  TDSRRC_STATE_MAX
}tdsrrc_state_e_type;


typedef enum
{
    TDSRRC_MODE_INACTIVE = 0,
    TDSRRC_MODE_ACTIVE,
    TDSRRC_MODE_STANDBY,
    TDSRRC_MODE_INTERRAT_IN_PROGRESS,
    TDSRRC_MODE_CHANGE,
    TDSRRC_MODE_WILDCARD,
    TDSRRC_MODE_MAX
} tdsrrc_mode_e_type;

/* Enumerated type for the types of RABs that need to be setup */
typedef enum
{
  TDSRRCRB_CS_VOICE_RAB,  /* Circuit Switched Voice */
  TDSRRCRB_CS_DATA_RAB,   /* Circuit Switched Data  */
  TDSRRCRB_PS_DATA_RAB,   /* Packet Switched Data   */
  TDSRRCRB_UNKNOWN_RAB    /* Unknown (invalid) RAB  */
}tdsrrcrb_rab_e_type;

/* Enumerated type for status of a Radio Bearer                      */
typedef enum
{
  TDSRRC_RB_STARTED,  /* Radio Bearer has been started (Default).       */
  TDSRRC_RB_STOPPED   /* Radio Bearer has been stopped                  */
}tdsrrc_rb_status_e_type;
#ifdef FEATURE_MVS
/* Enum identifying the bit rates of AMR mode */
typedef enum tdsrrc_amr_mode_bitrate_e {
  TDSRRC_AMR_MODE_LOW_BITRATE  = 0,
  TDSRRC_AMR_MODE_HIGH_BITRATE = 1,
  TDSRRC_AMR_MODE_MAX
} tdsrrc_amr_mode_bitrate_e_type;
#endif /* FEATURE_MVS */

typedef enum
{
  TDSINTRA_FREQ_CEL_RESEL_SUCCESS = 0,
  TDSINTRA_FREQ_CEL_RESEL_NBCCH_SETUP_FAILURE,
  TDSINTRA_FREQ_CEL_RESEL_SIB_WAIT_TIMEOUT,
  TDSINTRA_FREQ_CEL_RESEL_TRANSITION_FAILURE,
  TDSINTRA_FREQ_CEL_RESEL_FAILED_OTHER_CAUSE,
  TD2G_CEL_RESEL_TRIGGERED,
  TD2G_CEL_RESEL_FAILURE,
  TDSOOS_IN_IDLE_MODE,
  TDSOOS_IN_CONNECTED_MODE,
  TDSRACH_SUCCFUL_RRC_RETRY,
  TDSRACH_RRC_RETRY_FAIL_DUE_TO_CMAC_STATUS_IND,
  TDSRACH_RRC_RETRY_FAIL_DUE_TO_OTHER_CAUSE,
  TDSINTER_FREQ_CEL_RESEL_SUCCESS,
  TDSINTER_FREQ_CEL_RESEL_NBCCH_SETUP_FAILURE,
  TDSINTER_FREQ_CEL_RESEL_SIB_WAIT_TIMEOUT,
  TDSINTER_FREQ_CEL_RESEL_TRANSITION_FAILURE,
  TDSINTER_FREQ_CEL_RESEL_FAILED_OTHER_CAUSE,
  TDSOOS_IN_CONNECTED_MODE_CELL_FACH,
  TDSOOS_IN_CONNECTED_MODE_CELL_DCH,
  TDSOOS_IN_CONNECTED_MODE_CELL_PCH,
  TDSOOS_IN_CONNECTED_MODE_URA_PCH,
  TD2G_CEL_RESEL_FAILURE_NBCCH_SETUP_FAILURE,
  TD2G_CEL_RESEL_FAILURE_SIB_WAIT_TIMEOUT,
  TD2G_CEL_RESEL_FAILURE_ACQ_FAILURE,
  TD2G_CEL_RESEL_FAILURE_FAILED_OTHER_CAUSE,
  TD2L_CEL_RESEL_TRIGGERED,
  TD2L_CEL_RESEL_FAILURE,
  TD2L_CEL_RESEL_FAILURE_NBCCH_SETUP_FAILURE,
  TD2L_CEL_RESEL_FAILURE_SIB_WAIT_TIMEOUT,
  TD2L_CEL_RESEL_FAILURE_ACQ_FAILURE,
  TD2L_CEL_RESEL_FAILURE_FAILED_OTHER_CAUSE,
  G2TD_CEL_RESEL_TRIGGERED,
  G2TD_CEL_RESEL_FAILURE,
  G2TD_CEL_RESEL_FAILURE_NBCCH_SETUP_FAILURE,
  G2TD_CEL_RESEL_FAILURE_SIB_WAIT_TIMEOUT,
  G2TD_CEL_RESEL_FAILURE_ACQ_FAILURE,
  G2TD_CEL_RESEL_FAILURE_FAILED_OTHER_CAUSE,
  L2TD_CEL_RESEL_TRIGGERED,
  L2TD_CEL_RESEL_FAILURE,
  L2TD_CEL_RESEL_FAILURE_NBCCH_SETUP_FAILURE,
  L2TD_CEL_RESEL_FAILURE_SIB_WAIT_TIMEOUT,
  L2TD_CEL_RESEL_FAILURE_ACQ_FAILURE,
  L2TD_CEL_RESEL_FAILURE_FAILED_OTHER_CAUSE,
  TDSRRC_ACQ_TRIGGERED,
  TDSRRC_ACQ_SUCCEED,
  TDSRRC_ACQ_FAILED,
  TDSRRC_BPLMN_TD2TD,
  TDSRRC_BPLMN_TD2G,
  TDSRRC_BPLMN_TD2L,
  TDSRRC_BPLMN_L2TD,
  TDSRRC_BPLMN_G2TD,
  TDSRRC_G2TD_FAST_RETURN_TRIGGERED,
  TDSRRC_G2TD_FAST_RETURN_SUCCEED,
  TDSRRC_RCE_ATTEMP,
  TDSRRC_RCE_SUCCEED,
  TDSRRC_RCE_FAIL
} tdsrrc_debug_stats_enum_type;

/* This enumerates all Access Class indices used to access RRC Internal
 * Access Class barred list.
 */
typedef enum
{
  TDSRRC_INT_ACCESS_CLASS_0_INDEX,
  TDSRRC_INT_ACCESS_CLASS_1_INDEX,
  TDSRRC_INT_ACCESS_CLASS_2_INDEX,
  TDSRRC_INT_ACCESS_CLASS_3_INDEX,
  TDSRRC_INT_ACCESS_CLASS_4_INDEX,
  TDSRRC_INT_ACCESS_CLASS_5_INDEX,
  TDSRRC_INT_ACCESS_CLASS_6_INDEX,
  TDSRRC_INT_ACCESS_CLASS_7_INDEX,
  TDSRRC_INT_ACCESS_CLASS_8_INDEX,
  TDSRRC_INT_ACCESS_CLASS_9_INDEX,
  TDSRRC_INT_ACCESS_CLASS_10_INDEX,
  TDSRRC_INT_ACCESS_CLASS_11_INDEX,
  TDSRRC_INT_ACCESS_CLASS_12_INDEX,
  TDSRRC_INT_ACCESS_CLASS_13_INDEX,
  TDSRRC_INT_ACCESS_CLASS_14_INDEX,
  TDSRRC_INT_ACCESS_CLASS_15_INDEX,
  TDSRRC_INT_ACCESS_CLASS_MAX_INDEX
}tdsrrc_int_access_class_index_e_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* This structure is used to set the DL RLC AM PDU Size/DL RLC UM L1 Ind in RLC structure */
typedef struct
{
  uint8 no_of_rb;
  struct
  {
    rb_id_type rb_id;

    tdsrrc_rlc_dl_entity_e_type rlc_dl_entity;
      union
    {
      struct 
      {
        boolean dl_rlc_am_pdu_size_present;
        uint32 dl_rlc_am_pdu_size; /* Specified in Reconfig Msg */
      } dl_rlc_am_pdu_info;
      struct
      {
        boolean dl_rlc_um_rlc_li_size_present;
        tdsrrc_rlc_dl_um_li_e_type dl_rlc_um_rlc_l1_size; /* Specified in Reconfig Msg */
      } dl_rlc_um_pdu_info;
    } u;
    
  } rb_specified[TDS_MAX_RB];

} tdsrlc_size_per_rb_struct_type;


/* Strcuture with the procedure Id and RLC Size Change Status */
typedef struct
{
  tdsrrc_proc_e_type proc_id;
  boolean is_rlc_size_changed;
  rb_id_type rb_id;
  tdsrlc_reestablish_e_type direction;
} tdsrlc_size_change_struct_type;


/* Data structure to store System Specific Capability Update 
   Requirement */ 
typedef struct  {
  uint8  count;                  /* Number of valid System Specific
                                     Capability Requirement Update Req */
  tdsrrc_system_specific_cap_update_req_e_type value[16];
} tdsrrc_system_specific_cap_update_req_list_type;

/* Data structure to store UE Capability Update Requirement 
   received through RRC Connection Setup Message */
typedef struct {
  boolean  system_specific_cap_update_req_present;
                               /* This indicates whether System Specific 
                                  Capability Update Requirement is present
                                  or not */
  boolean  ue_radio_capability_update_requirement;
                               /* This indicates whether Capability Update
                                  is required or not */
  tdsrrc_system_specific_cap_update_req_list_type system_specific_cap_update_req_list;  
                                  
} tdsrrc_capability_update_requirement_type;


/* Variable to hold a single RB that maps to a RAB                   */
typedef struct
{
  uint32 subflow;         /* Subflow value for this RB               */
  tdsrrc_rb_status_e_type rb_status; /* Status - started or stopped.    */
  tdsrrc_RB_Identity rb_id;		   /* RB identity							  */

}tdsrrc_rb_for_rab_type;


/* Variable to hold RAB Information. Note that this supports
only GSM_MAP RAB Ids now. When ASNI-41 RABs are supported
this needs to be expanded.                                           */
typedef struct
{
  boolean rab_id_is_gsm_map;       /* Set to TRUE if the RAB id is GSM_MAP */
  tdsrrcrb_rab_e_type rab_type;      /* Type of RAB - PS, CS-DATA, CS-VOICE */ 
  rrc_rab_id_type rab_id;          /* GSM-MAP RAB Identity                 */
  rrc_cn_domain_identity_e_type cn_domain; /* CN domain for this RAB               */
  uint8  num_rbs_for_rab;          /* Number of RBs mapped to this RAB     */
  tdsrrc_Re_EstablishmentTimer re_est_timer;  /* Re-est timer associated with RAB    */
  //NAS-Sync indicator may need to be stored later. It's not needed for now.
  tdsrrc_rb_for_rab_type rb_for_rab[MAX_RB_PER_RAB];
#ifdef FEATURE_TDSCDMA_VOC_AMR_WB
  mvs_mode_type nas_Synchronisation_Indicator;  /* the codec type requested in OTA msg*/ 
#endif /*FEATURE_TDSCDMA_VOC_AMR_WB*/

#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
  boolean is_cs_over_hs_rab;            /*To keep track whether the RAB in question is for CS over HSPA or not.*/
#endif /*FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA*/
}tdsrrc_est_rab_info_type;

/* Variable to hold Signalling Radio Bearer information for the
"Established RAB's variable. The onle information needed here is
whether the Radio Bearer has been started or stopped                */
typedef struct
{
  tdsrrc_rb_status_e_type srb_status; /* Status for each SRB           */
}tdsrrc_est_srb_info_type;


/* The Variable "ESTABLISHED_RABS" as defined by the spec.          */
typedef struct
{
  tdsrrc_est_rab_info_type rabs[MAX_RAB_TO_SETUP]; /* Info for each RAB */
  tdsrrc_est_srb_info_type srbs[MAX_SRB_SETUP];    /* Info for each SRB */
}tdsrrc_established_rabs_type;

/* Structure to hold results of removing an RB from ESTABLISHED_RABS */
typedef struct
{
  tdsrrc_rb_remove_e_type result; /* Result of RB removal */
  rrc_rab_id_type rab_id;      /* GSM-MAP RAB Identity of the RAB
                               that was removed. Will be filled
                               only if the result is TDSRRC_RB_AND_RAB_REMOVED */
  rrc_cn_domain_identity_e_type cn_domain; /* CN domain for this RAB. Will be filled
                               only if the result is TDSRRC_RB_AND_RAB_REMOVED */
  tdsrrcrb_rab_e_type rab_type; /*Voice, CS Data or PS RAB*/
#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
  tdsrrc_cs_voice_e_type voice_type;
#endif /*FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA*/
} tdsrrc_rb_remove_result_type;


/* This structure is passed to get_amr_mapping info from procedures by
populating the rb id info */

typedef struct
{
  tdsrrcrb_rab_e_type rab_type;      /* Type of RAB - PS, CS-DATA, CS-VOICE */ 
  uint32 num_rbs_for_rab;          /* Number of RBs mapped to this RAB     */
  rb_id_type  rb_id[MAX_RB_PER_RAB];
}tdsrrc_rb_info_for_amr_type;



typedef struct
{
  boolean  barred[TDSRRC_INT_ACCESS_CLASS_MAX_INDEX];
}tdsrrc_int_access_class_barred_list_type;

//add local type for n312 and n315 instead of using asn1 type.

typedef struct
{
  uint16 n312;
  uint16 n315;
}tdsrrc_conn_timers_and_constants;

typedef struct
{
  uint8 dl_category;        /* max DL CAT supported by UE               */
  uint16 ul_category;        /* max UL CAT supported by UE               */
  uint32 dl_ded_rate;       /* max DL data rate supported by UE               */
  uint32 ul_ded_rate;       /* max UL data rate supported by UE               */
} tdsrrc_data_rate_info_type;

typedef void TDSRRC_MMAL_LOG_CB_TYPE(tdslog_message_type *log_message);

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/*macros to return UE Connected Mode Constants.  There will be used frequently
 *hence a macro is defined rather than a function
 */
#define TDSRRC_GET_N301()    tdsrrc_ue_conn_timers_and_constants.n_301
#define TDSRRC_GET_N302()    tdsrrc_ue_conn_timers_and_constants.n_302
#define TDSRRC_GET_N304()    tdsrrc_ue_conn_timers_and_constants.n_304
#define TDSRRC_GET_N310()    tdsrrc_ue_conn_timers_and_constants.n_310
#define TDSRRC_GET_N312()    tdsconn_timers_and_constants.n312
#define TDSRRC_GET_N313()    tdsrrc_ue_conn_timers_and_constants.n_313
#define TDSRRC_GET_N315()    tdsconn_timers_and_constants.n315

#define TDSRRC_GET_T301()    tdsrrc_ue_conn_timers_and_constants.t_301
#define TDSRRC_GET_T302()    tdsrrc_ue_conn_timers_and_constants.t_302
#define TDSRRC_GET_T304()    tdsrrc_ue_conn_timers_and_constants.t_304
#define TDSRRC_GET_T305()    tdsrrc_ue_conn_timers_and_constants.t_305
#define TDSRRC_GET_T307()    tdsrrc_ue_conn_timers_and_constants.t_307
#define TDSRRC_GET_T308()    tdsrrc_ue_conn_timers_and_constants.t_308
#define TDSRRC_GET_T309()    tdsrrc_ue_conn_timers_and_constants.t_309
#define TDSRRC_GET_T310()    tdsrrc_ue_conn_timers_and_constants.t_310
#define TDSRRC_GET_T311()    tdsrrc_ue_conn_timers_and_constants.t_311
#define TDSRRC_GET_T312()    tdsrrc_ue_conn_timers_and_constants.t_312
#define TDSRRC_GET_T313()    tdsrrc_ue_conn_timers_and_constants.t_313
#define TDSRRC_GET_T314()    tdsrrc_ue_conn_timers_and_constants.t_314
#define TDSRRC_GET_T315()    tdsrrc_ue_conn_timers_and_constants.t_315
#define TDSRRC_GET_T316()    tdsrrc_ue_conn_timers_and_constants.t_316
#define TDSRRC_GET_T317()    tdsrrc_ue_conn_timers_and_constants.t_317


                                             
/* This macro coverts the MCC from the array format in rrc_plmn_identity_type 
 * to the integer format.
 */
#define   TDSRRC_CSP_GET_INT_MCC(plmn_id)  \
  ( plmn_id.mcc[0] * 100 +  plmn_id.mcc[1] * 10 + plmn_id.mcc[2] )


/* This macro coverts the MNC from the array format in rrc_plmn_identity_type
 * to the integer format.
 */
#define   TDSRRC_CSP_GET_INT_MNC(plmn_id)  \
( \
  (plmn_id.num_mnc_digits == 2) ? \
    (plmn_id.mnc[0] * 10 + plmn_id.mnc[1]) : \
    (plmn_id.mnc[0] * 100 + plmn_id.mnc[1] * 10 + plmn_id.mnc[2]) \
)

#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
#define RRC_IS_TDS_UI_EVENT_ENABLED(event_bit_mask) tds_UI_event_bit_mask & event_bit_mask
#endif

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/*      EXTERNALIZED RRC COMMAND QUEUES                   */
/*                                                        */
/*  All Command Queues for the RRC Task are defined here  */
/*--------------------------------------------------------*/

/* Queue for internal RRC commands within the RRC task    */
extern q_type tdsrrc_int_cmd_q;

/* Queue for RRC commands sent by L1 task                 */
extern q_type tdsrrc_l1_cmd_q;

/* Queue for RRC commands sent by MAC task                */
extern q_type tdsrrc_mac_cmd_q;

/* Queue for RRC commands sent by RLC task                */
extern q_type tdsrrc_rlc_cmd_q;

/* Queue for RRC commands sent by MM task                 */
extern q_type tdsrrc_mm_cmd_q;

/* Queue for RRC commands sent by TC task                 */
extern q_type tdsrrc_tc_cmd_q;

/* Queue for RRC commands sent by GSM RR task             */
extern q_type tdsrrc_rr_cmd_q;

/* Stores timestamp (in CFN ) when a Downlink SDU is received */
extern  uint16  tdsrrc_cfn_before_decode[TDSMAX_NUM_OF_PROCEDURES];

/*Stores cell_id for each RL given in OTA msg*/
extern uint32  tdscell_id_per_rl[TDSL1_MAX_RL]; 

extern tdsrrc_capability_update_requirement_type tdsrrc_capability_update_requirement;

#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
extern tds_UI_event_info_type tds_UI_event_info;
extern uint16 tds_UI_event_bit_mask;
#endif

extern uint8 tdsrrc_nv_rel_indicator;
extern boolean tdsps_rb_re_establish_indicator;
extern uint8 tdsrrc_nv_rel_indicator;

#if defined(FEATURE_DUAL_SIM)||defined(FEATURE_TDSCDMA_DSDS) 
extern boolean tdsrrc_is_sim_id_valid;
extern sys_modem_as_id_e_type tdsrrc_sim_id_recvd;
//#define WRM_DEF_RESOURCE_TYPE TRM_RXTX_BEST
extern boolean tds_is_ue_in_dual_standby;
#endif

extern tdsrrc_ProtocolErrorIndicator tdsrrc_protocol_error_indicator;

extern rrc_establish_cause_e_type tdsrrc_establishment_cause;

extern rrc_initial_ue_id_choice_e_type tdsrrc_initial_ue_identity_choice;

extern rrc_initial_ue_id_type tdsrrc_initial_ue_identity;

/*stores the current Conn Timers and constants.  updated in tdsrrcdata.c file*/
extern tdsrrc_UE_ConnTimersAndConstants tdsrrc_ue_conn_timers_and_constants;

/* Stores RSSI value - Defined for RSSI enhancements */
#ifdef FEATURE_TDSCDMA_DATA_FOR_UI_BAR_DISPLAY
extern int16  tdsrrc_rscp;
extern int16  tdsrrc_ecio;
extern int16  tdsrrc_rssi;
extern boolean tdsrrc_use_ecio_for_ui_display;
#else

extern uint8 tdsrrc_rssi;
#endif

#ifdef FEATURE_TDSCDMA_VOC_AMR_WB
/* This contains the current Codec mode being used  */
extern mvs_mode_type tdsrrc_active_codec;
/* Contains codec info received in the OTA msg, so that it can be sent to MAC in ordered config  */
extern mvs_mode_type tdsrrc_ota_codec_mode_status;
#endif /*FEATURE_TDSCDMA_VOC_AMR_WB*/

#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
extern tdsrrc_cs_over_hs_status_e_type tdsrrc_cs_over_hs_status;
extern boolean tdsrrc_cs_over_hspa_enabled_nv;
#endif /*FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA*/

extern tdsrrc_conn_timers_and_constants tdsconn_timers_and_constants;
/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*===========================================================================

FUNCTION   tdsrrc_update_rab_type_in_rab_info_r4

DESCRIPTION
 This function is updates the rab type in 'tdsrrc_est_rab_info_type' structure
 using information stored in ASN1 'tdsrrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'tdsrrc_RAB_InformationSetup' is already validated.

  The function is copy from tdsrrc_update_rab_type_in_rab_info_r5

ARGUMENTS:

  tdsrrc_RAB_InformationSetup *: pointer to ASN1 structure,
  tdsrrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

tdsrrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_update_rab_type_in_rab_info_r4
(
  tdsrrc_RAB_InformationSetup_r4 *rab_info_setup_ptr,
  tdsrrc_est_rab_info_type *est_rab_info_ptr 
);

/*===========================================================================

FUNCTION  RRC_RETURN_LC_INFO_FOR_RAB

DESCRIPTION

  This function returns the logical channel ids for a given RAB.
  The calling function must initialize a structure of type
  tdsrrc_user_plane_lc_info_type and fill in the RAB id before
  passing the pointer to this function. This function will fill
  in the LC ids for uplink and downlink for that RAB to the given
  pointer.
    
DEPENDENCIES

  Currently this is only supported for CS data RABs and PS RABs.
  This function must not be used for any other type of RABs at
  present.
 
RETURN VALUE

  A a value of type rrc_rab_status_e_type. TDSRRC_RAB_NOT_FOUND is returned
  if the given RAB is not a valid RAB that has been setup by RRC. TDSRRC_RAB_FOUND
  is returned if the RAB is valid.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_rab_search_e_type tdsrrc_return_lc_info_for_rab
(
  tdsrrc_user_plane_lc_info_type *ptr
);

#ifdef FEATURE_TDSCDMA_REL8
/*===========================================================================

FUNCTION   tdsrrc_update_rab_type_in_rab_info_r8

DESCRIPTION
 This function is updates the rab type in 'tdsrrc_est_rab_info_type' structure
 using information stored in ASN1 'tdsrrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'tdsrrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  tdsrrc_RAB_InformationSetup *: pointer to ASN1 structure,
  tdsrrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

tdsrrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_update_rab_type_in_rab_info_r8
(
  tdsrrc_RAB_InformationSetup_r8 *rab_info_setup_ptr,
  tdsrrc_est_rab_info_type *est_rab_info_ptr 
);
#endif /*FEATURE_TDSCDMA_REL8*/


/*===========================================================================

FUNCTION   tdsrrc_update_rab_type_in_rab_info_r7

DESCRIPTION
 This function is updates the rab type in 'tdsrrc_est_rab_info_type' structure
 using information stored in ASN1 'tdsrrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'tdsrrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  tdsrrc_RAB_InformationSetup *: pointer to ASN1 structure,
  tdsrrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

tdsrrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_update_rab_type_in_rab_info_r7
(
  tdsrrc_RAB_InformationSetup_r7 *rab_info_setup_ptr,
  tdsrrc_est_rab_info_type *est_rab_info_ptr 
);

/*===========================================================================

FUNCTION   tdsrrc_update_rab_type_in_rab_info

DESCRIPTION
 This function is updates the rab type in 'tdsrrc_est_rab_info_type' structure
 using information stored in ASN1 'tdsrrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'tdsrrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  tdsrrc_RAB_InformationSetup *: pointer to ASN1 structure,
  tdsrrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

tdsrrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_update_rab_type_in_rab_info
(
  tdsrrc_RAB_InformationSetup *rab_info_setup_ptr,
  tdsrrc_est_rab_info_type *est_rab_info_ptr 
);

/*===========================================================================

FUNCTION   tdsrrc_update_rab_type_in_rab_info_r5

DESCRIPTION
 This function is updates the rab type in 'tdsrrc_est_rab_info_type' structure
 using information stored in ASN1 'tdsrrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'tdsrrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  tdsrrc_RAB_InformationSetup *: pointer to ASN1 structure,
  tdsrrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

tdsrrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_update_rab_type_in_rab_info_r5
(
  tdsrrc_RAB_InformationSetup_r5 *rab_info_setup_ptr,
  tdsrrc_est_rab_info_type *est_rab_info_ptr 
);




/*===========================================================================

FUNCTION   tdsrrc_update_rab_type_in_rab_info_r6

DESCRIPTION
 This function is updates the rab type in 'tdsrrc_est_rab_info_type' structure
 using information stored in ASN1 'tdsrrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'tdsrrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  tdsrrc_RAB_InformationSetup *: pointer to ASN1 structure,
  tdsrrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

tdsrrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_update_rab_type_in_rab_info_r6
(
  tdsrrc_RAB_InformationSetup_r6 *rab_info_setup_ptr,
  tdsrrc_est_rab_info_type *est_rab_info_ptr 
);



/*===========================================================================

FUNCTION   tdsrrc_is_nw_shared

DESCRIPTION

 This function will indicate if this N/W is shared or not.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_is_nw_shared
(
  tdsrrc_MasterInformationBlock *mib_ptr
);


/*===========================================================================

FUNCTION  tdsrrc_debug_get_rrc_retry_fail_due_to_others_cnt

DESCRIPTION This function gives counter value which is incremented 
when RRC retransmit message Bcoz of receiving L2 ACK IND with cause
failure for previously attempted transmission of RRC message on RACH 
.
DEPENDENCIES
  None
  
RETURN   Number of times RRC transmission or retry fail due to L2 ACK IND.
===========================================================================*/
uint32 tdsrrc_debug_get_rrc_retry_fail_due_to_others_cnt
(
  void
);

/*===========================================================================

FUNCTION   RRC_INITIALIZE_ESTABLISHED_RABS

DESCRIPTION

  This function initializes the "ESTABLISHED_RABS" variable.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_initialize_established_rabs( void );

/*===========================================================================

FUNCTION   RRC_FIND_IF_VOICE_RAB_EXISTS_IN_ESTABLISHED_RABS

DESCRIPTION

  This function checks if the given RAB id is present in the
  "ESTABLISHED_RABS" variable & belongs to CS domain. 
  If so, it returns TRUE.
  Else it returns FALSE.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean
tdsrrc_find_if_voice_rab_exists_in_established_rabs(rrc_rab_id_type rab_id /* Id of the received RAB */);

/*===========================================================================

FUNCTION   RRC_UPDATE_RB_INFO_TO_EXISTING_VOICE_RAB

DESCRIPTION

  This function checks if the given RAB id is present in the
  "ESTABLISHED_RABS" variable & belongs to CS domain. 
  If so, it updates the RB info.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_update_rb_info_to_existing_rab(tdsrrc_est_rab_info_type *rbe_ptr);

/*===========================================================================

FUNCTION   RRC_CLEAR_ESTABLISHED_RABS

DESCRIPTION

  This function clears the "ESTABLISHED_RABS" variable. All the RABS
  will be removed from the variable. This is to be used when the RRC
  connection is released or if all the RABs have to be released due to
  some failure condition.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_clear_established_rabs( void );

/*===========================================================================

FUNCTION   tdsrrc_send_rab_rel_ind_on_ps_release

DESCRIPTION

  This function will send a rab-rel-ind which will follow immediately after 
  RRC_REL_IND for PS rabs.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_send_rab_rel_ind_on_ps_release(void);


#if defined(FEATURE_INTERRAT_HANDOVER_G2TD) || defined(FEATURE_TDSCDMA_L2TD_PS_HANDOVER)
/*===========================================================================

FUNCTION          RRC_TRANSLATE_URNTI_SHORT


DESCRIPTION       

  This function translates U-RNTI Short type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external U-RNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void tdsrrc_translate_urnti_short

(
  struct tdsrrc_U_RNTI_Short  *ext_urnti,     /* Pointer to external U-RNTI type */
  tdsrrc_int_u_rnti_type      *int_urnti    /* Pointer to internal U-RNTI type */

); 
#endif /* FEATURE_INTERRAT_HANDOVER_G2TD */

/*===========================================================================
FUNCTION  tdsrrc_bcch_plmns_match

DESCRIPTION
  This function does a matching of two PLMNs obtained from BCCH.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If BCCH PLMNs match.
  FALSE: If BCCH PLMNs do not match.

SIDE EFFECTS
  The two passed PLMNs have to be both from BCCH. For comparison between a
  NAS supplied PLMN and a BCCH PLMN, DO NOT call this function.
===========================================================================*/
boolean tdsrrc_bcch_plmns_match(rrc_plmn_identity_type plmn1, rrc_plmn_identity_type plmn2);

/*====================================================================
FUNCTION: tdsrrc_translate_n313

DESCRIPTION:
  This function translates the enum for timer N313 into an int16 value.

DEPENDENCIES:
  None

RETURN VALUE:
  uint16 value corresponding to the enum passed, 0 if invalid

SIDE EFFECTS:
====================================================================*/
uint16 tdsrrc_translate_n313(tdsrrc_N_313 val);

/*===========================================================================

FUNCTION   tdsrrc_is_voice_rab_present

DESCRIPTION

  This function indicates whether any voice RAB is present or not in est_rabs database.
  This function is going to be used by MAC/RRC to verify whether Voice RAB is present 
  or not.
        
DEPENDENCIES

None

RETURN VALUE

  TRUE: if voice RAB exist in est_rabs database
  FALSE: if voice RAB doesnt exist in est_rabs database

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_is_voice_rab_present (void);

/*===========================================================================

FUNCTION   tdsrrc_is_cs_data_rab_present

DESCRIPTION

  This function indicates whether any cs data RAB is present or not in est_rabs database.
  This function is going to be used by RRC to verify whether cs data RAB is present 
  or not.
  This function has been modified such that it returns TRUE only if RRCRB_CS_DATA_RAB is of DCH type.        
DEPENDENCIES

None

RETURN VALUE

  TRUE: if cs data RAB exist in est_rabs database
  FALSE: if cs data RAB doesnt exist in est_rabs database

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_is_cs_data_rab_present (void);

#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
/*===========================================================================
FUNCTION   tdsrrc_get_cs_over_hs_rb_id

DESCRIPTION
  This function retrieves CSoHS RB Id from EST_RABs
        
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
rb_id_type tdsrrc_get_cs_over_hs_rb_id
(
  void
);

/*===========================================================================
FUNCTION   tdsrrc_get_cs_voice_rab_type

DESCRIPTION
  This function returns CS voice RAB type from the EST_RABs. In case, no Voice RAB is present, it returns 
  TDSRRC_CS_VOICE_OVER_INVALID.
        
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
tdsrrc_cs_voice_e_type tdsrrc_get_cs_voice_rab_type 
(
  void
);

/*===========================================================================
FUNCTION   tdsrrc_clear_rbs_for_given_rab_from_est_rabs

DESCRIPTION
 This function inits the RBs for a given RAB. Currently, it's used to remove RBs during CSoDCH->CSoHSPA or 
 vice-versa voice transitions.
        
DEPENDENCIES
  None

ARGUMENTS:
  None

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void tdsrrc_clear_rbs_for_given_rab_from_est_rabs
(
  rrc_rab_id_type rab_id
);

/*===========================================================================
FUNCTION   tdsrrc_get_cs_over_hs_status

DESCRIPTION
  This function retrieves CS over HSPA status from RRC global tdsrrc_cs_over_hs_status.
        
DEPENDENCIES

ARGUMENTS:

RETURN VALUE
  CSoHSPA status.

SIDE EFFECTS
  None.
===========================================================================*/
tdsrrc_cs_over_hs_status_e_type tdsrrc_get_cs_over_hs_status
(
  void
);

/*===========================================================================
FUNCTION   tdsrrc_set_cs_over_hs_status

DESCRIPTION
  This function sets CSoHSPA status in RRC global tdsrrc_cs_over_hs_status.
        
DEPENDENCIES

ARGUMENTS:

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void tdsrrc_set_cs_over_hs_status
(
  tdsrrc_cs_over_hs_status_e_type cs_over_hs_status
);

/*===========================================================================
FUNCTION rrc_get_cs_over_hspa_nv

DESCRIPTION
  This  function gets the NV item NV_WCDMA_CS_VOICE_OVER_HSPA_ENABLED_I and stores it in 
  rrc global variable 'tdsrrc_cs_over_hspa_enabled_nv'

DEPENDENCIES
  None.

RETURN VALUE
  None.
===========================================================================*/
void tdsrrc_get_tds_cs_over_hspa_enabled_nv
(
  void
);
#endif

/*===========================================================================
FUNCTION   tdsrrc_is_ps_domain_rab_present

DESCRIPTION
  This function returns TRUE if any PS RAB is present in EST_RABs.
        
DEPENDENCIES
  None

ARGUMENTS:
  None

RETURN VALUE
  TRUE - PS RAB is present.
  FALSE - PS RAB isn't present.

SIDE EFFECTS
  None.
===========================================================================*/
boolean tdsrrc_is_ps_domain_rab_present
(
  void
);

/*===========================================================================

FUNCTION   tdsrrc_indicate_layer1_next_config_is_for_voice

DESCRIPTION

  This function will indicate layer1 that next configuration is for AMR by calling some 
  feature specific call back function provided by layer1.
        
DEPENDENCIES

  None
 
RETURN VALUE

  None
SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_indicate_layer1_next_config_is_for_voice (void);

/*===========================================================================

FUNCTION   tdsrrc_indicate_layer1_next_config_will_release_voice_rab

DESCRIPTION

  This function will indicate layer1 that next configuration to physical layer
  will release AMR. This is done by calling some feature specific call back
  function provided by layer1.
        
DEPENDENCIES

  None
 
RETURN VALUE

  None
SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_indicate_layer1_next_config_will_release_voice_rab (void);

/*===========================================================================

FUNCTION   rrc_get_hsdpa_cell_capability_indicator

DESCRIPTION

  This function will indicate if the current cell supports HSDPA.
        
DEPENDENCIES

  None
 
RETURN VALUE

  None
SIDE EFFECTS

  None.

===========================================================================*/
extern sys_hs_ind_e_type tdsrrc_get_3g_cell_capability_indicator (void);

/*===========================================================================

FUNCTION          RRC_SEND_3G_CELL_CAPABILITY_INFORMATION

DESCRIPTION       This function sends 3G cell related info to MM

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void tdsrrc_send_3g_cell_capability_information(sys_hs_ind_e_type hsdpa_hsupa_indication);

#ifdef FEATURE_HSPA_CALL_STATUS_IND
/*===========================================================================

FUNCTION          tdsrrc_send_hspa_call_status_info

DESCRIPTION       This function sends 3G cell related info to MM

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void tdsrrc_send_hspa_call_status_info
(
  void
);
#endif /* FEATURE_HSPA_CALL_STATUS_IND */

/*====================================================================
FUNCTION: tdsrrc_set_rlc_size_change_status()

DESCRIPTION:
  This function sets the RLC PDU Size per RB in the global
  rlc size change structure

DEPENDENCIES:
  None

RETURN VALUE:
  RLC Size

SIDE EFFECTS:
  None.
====================================================================*/
void tdsrrc_set_rlc_size_change_status(tdsrrc_proc_e_type proc_id, boolean rlc_size_change_status,
                                       tdsrrc_RB_Identity rb_id, tdsrlc_reestablish_e_type direction);

/*====================================================================
FUNCTION: rrc_get_rlc_size_change_status()

DESCRIPTION:
  This function gets the RLC Size Change Status based on the Procedure Id

DEPENDENCIES:
  None

RETURN VALUE:
  RLC Size

SIDE EFFECTS:
  None.
====================================================================*/
boolean tdsrrc_get_rlc_size_change_status_for_procedure_id(tdsrrc_proc_e_type proc_id);



/*===========================================================================

FUNCTION tdsrrc_print_supported_asn1_rel_version

DESCRIPTION
  This  function prints the ASN1 release version supported by the UE as per 
  NV item "NV_WCDMA_RRC_VERSION_I" i.e. global variable 'tdsrrc_nv_rel_indicator'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsrrc_print_supported_asn1_rel_version(void);


#ifdef FEATURE_MVS
/*===========================================================================

FUNCTION tdsrrc_get_amr_mode_bitrate

DESCRIPTION
  This  function takes amr mode as argument & returns the AMR rate category

DEPENDENCIES
  None.

RETURN VALUE
  tdsrrc_amr_mode_bitrate_e_type.

===========================================================================*/
tdsrrc_amr_mode_bitrate_e_type tdsrrc_get_amr_mode_bitrate(mvs_amr_mode_type amr_mode);
#endif /* FEATURE_MVS */


/*===========================================================================

FUNCTION  tdsrrc_extract_rac_from_sib_info

DESCRIPTION This function extracts RAC from NAS CN Domain System Information List.

DEPENDENCIES
  None
  
RETURN VALUE          
  TRUE: RAC is found
  FALSE: RAC is not found

===========================================================================*/
boolean  tdsrrc_extract_rac_from_sib_info
(
  rrc_rai_type *rai,
  tdsrrc_CN_DomainSysInfoList  *cn_domain_sys_info_list_ptr
);


/*===========================================================================

FUNCTION  tdsrrc_debug_get_rrc_retry_fail_due_to_l2_ack_ind_cnt

FUNCTION  tdsrrc_debug_get_rrc_retry_fail_due_to_l2_ack_ind_cnt

DESCRIPTION This function gives counter value which is incremented 
when MAC hits max re-try while RACHing & sends CMAC_STATUS_IND
with cause as hit MAX re-try.
.
DEPENDENCIES
  None
  
RETURN  Number of times max RACH transmission hit by MAC.
===========================================================================*/
uint32 tdsrrc_debug_get_rrc_retry_fail_due_to_l2_ack_ind_cnt
(
  void
);


/*===========================================================================

FUNCTION  tdsrrc_debug_get_succful_rrc_trans_retry_cnt

DESCRIPTION This function gives counter value which is  incremented 
when RRC is successful in receiving RRC procedure level response
message from N/W for any RRC message sent on common channel i.e. RACH. 
.
DEPENDENCIES
  None
  
RETURN  Number of times RRC is successful in receiving RRC procedure level response
message from N/W since last power up

===========================================================================*/
uint32 tdsrrc_debug_get_succful_rrc_trans_retry_cnt
(
  void
);


/*===========================================================================

FUNCTION  tdsrrc_debug_get_oos_connected_mode_cnt

DESCRIPTION This function gives how many times the OOS is triggered in connected mode.

DEPENDENCIES
  None
  
RETURN  Number of times OOS is triggered in connected mode since last power up

===========================================================================*/
uint32 tdsrrc_debug_get_oos_connected_mode_cnt
(
  void
);


/*===========================================================================

FUNCTION  tdsrrc_debug_get_oos_idle_mode_cnt

DESCRIPTION  This function gives how many times the OOS is triggered in idle mode.

DEPENDENCIES
  None
  
RETURN  Number of times OOS is triggered in idle mode since last power up

===========================================================================*/
uint32 tdsrrc_debug_get_oos_idle_mode_cnt
(
  void
);



/*===========================================================================

FUNCTION  rrc_debug_get_td2g_cel_resel_fail_cnt

DESCRIPTION  This function gives the TDSTOG cell reselection fail count.

DEPENDENCIES
  None
  
RETURN  TDSTOG cell reselection failure count since last power up

===========================================================================*/
uint32 rrc_debug_get_td2g_cel_resel_fail_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_wtog_cel_resel_success_cnt

DESCRIPTION  This function gives the number of times the TDSTOG cell reselection is triggered 

DEPENDENCIES
  None
  
RETURN  TDSTOG cell reselection count since last power up

===========================================================================*/
uint32 rrc_debug_get_td2g_cel_resel_cnt
(
  void
);



/*===========================================================================

FUNCTION  tdsrrc_debug_get_cel_resel_failed_other_causes_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 the causes other than n-bcch setup failure, sib wait time out and transition failure.

DEPENDENCIES
  None
  
RETURN  Intra frequency reselection failure count due to other causes since last power up

===========================================================================*/
uint32 tdsrrc_debug_get_cel_resel_failed_other_causes_cnt
(
  void
);

/*===========================================================================

FUNCTION  tdsrrc_debug_get_cel_resel_transition_fail_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 transition failure(s-criteria failure).

DEPENDENCIES
  None
  
RETURN  Intra frequency reselection failure count due to transition failures since last power up 

===========================================================================*/
uint32 tdsrrc_debug_get_cel_resel_transition_fail_cnt
(
  void
);


/*===========================================================================

FUNCTION  tdsrrc_debug_get_cel_resel_sib_wait_timeout_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 SIB wait time out.

DEPENDENCIES
  None
  
RETURN   Intra frequency reselection failure count due to SIB wait time out since last power up

===========================================================================*/
uint32 tdsrrc_debug_get_cel_resel_sib_wait_timeout_cnt
(
  void
);

/*===========================================================================

FUNCTION  tdsrrc_debug_get_cel_resel_nbcch_set_up_fail_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 N-BCCH set up failure.

DEPENDENCIES
  None
  
RETURN  Intra frequency reselection failure count due to N-BCCH set up failue since last power up

===========================================================================*/
uint32 tdsrrc_debug_get_cel_resel_nbcch_set_up_fail_cnt
(
  void
);

/*===========================================================================

FUNCTION  tdsrrc_debug_get_cel_resel_success_cnt

DESCRIPTION   This function gives the intra frequency cell reselection success count

DEPENDENCIES
  None
  
RETURN VALUE  Intra frequency reselection success count since last power up

===========================================================================*/
uint32 tdsrrc_debug_get_cel_resel_success_cnt
(
  void
);

/*===========================================================================

FUNCTION  tdsrrc_debug_get_global_var

DESCRIPTION   This function gives the pointer of tdsrrc_debug_global

DEPENDENCIES
  None
  
RETURN VALUE  Intra frequency reselection success count since last power up

===========================================================================*/
void tdsrrc_debug_get_global_var
(
tdsrrc_debug_global_var_type** tdsrrc_debug_ptr
);
/*===========================================================================

FUNCTION  tdsrrc_debug_clear_stats

DESCRIPTION This function clears all the debug statistics

DEPENDENCIES
  None
  
RETURN VALUE
  None

===========================================================================*/
void tdsrrc_debug_clear_stats
(
  void
);


/*===========================================================================

FUNCTION  tdsrrc_debug_stats

DESCRIPTION This function increments the debug statistics

DEPENDENCIES
  None
  
RETURN VALUE
  None

===========================================================================*/
void tdsrrc_debug_stats
(
  tdsrrc_debug_stats_enum_type rrc_debug_cause
);

#ifdef FEATURE_MVS
/*===========================================================================

FUNCTION tdsrrc_get_dl_amr_mode_given_rbs

DESCRIPTION
  This  function operates on tdsamr_rb_info global var & returns previous & new
  DL AMR modes. If changed, returns that the mode has changed(TRUE).

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean tdsrrc_get_dl_amr_mode_given_rbs
(
  mvs_amr_mode_type *prev_mode, 
  mvs_amr_mode_type *new_mode,
  tdsrrc_rb_info_for_amr_type *tdsamr_rb_info
);
#endif /* FEATURE_MVS */


/*===========================================================================

FUNCTION tdsrrc_get_tds_rrc_version_nv

DESCRIPTION
  This  function gets the NV item NV_WCDMA_RRC_VERSION_I and stores it in 
  rrc global variable 'tdsrrc_nv_rel_indicator'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsrrc_get_tds_rrc_version_nv
(
void
);


/*====================================================================
FUNCTION: tdsrrc_update_constants_n312_and_n315_r5

DESCRIPTION:
  This function updates the global constants for N312 & N315 for R5.
  
DEPENDENCIES:
  None

RETURN VALUE:
  uint16 value corresponding to the enum passed, 0 if invalid

SIDE EFFECTS:
====================================================================*/
uint16 tdsrrc_update_constants_n312_and_n315_r5
(
tdsrrc_UE_ConnTimersAndConstants_r5 *conn_timers
);


/*===========================================================================

FUNCTION tdsrrc_get_lac_id 

DESCRIPTION
  
This function returns LAC id for the serving cell.
  
DEPENDENCIES
  None

RETURN VALUE
  SUCCESS or FAILURE
  
SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type tdsrrc_get_lac_id
(
  rrc_lai_type *lai
);


/*===========================================================================

FUNCTION tdsrrc_set_dl_um_li

DESCRIPTION

  This function sets the DL UM LI indication as per the RB specified in the 
  global structure tdsrlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type tdsrrc_set_dl_um_li
(
  tdsrrc_RB_Identity rb_id,
  tdsrrc_DL_UM_RLC_LI_size dl_rlc_um_li
);

#if 0
#ifdef FEATURE_TDSCDMA_SRB5_SUPPORT
/*===========================================================================
FUNCTION   is_ctch_present_in_serving_cell

DESCRIPTION
  This function returns TRUE if CTCH is present on current cell otherwise
  returns false.
 
DEPENDENCIES
  None.
 
RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean is_ctch_present_in_serving_cell(tdsrrc_state_e_type next_rrc_state);
#endif
#endif

/*===========================================================================

FUNCTION tdsrrc_set_dl_am_rlc_pdu_size

DESCRIPTION

  This function sets the DL AM RLC PDU Size as per the RB specified in the 
  global structure tdsrlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  DL AM RLC PDU Size if found else TDSINVALID_DL_AM_PDU_RLC_SIZE
 

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type tdsrrc_set_dl_am_rlc_pdu_size
(
  tdsrrc_RB_Identity rb_id,
  uint32 dl_rlc_am_pdu_size
);


/*===========================================================================

FUNCTION tdsrrc_get_dl_um_li

DESCRIPTION

  This function finds the DL UM LI as per the RB specified from the 
  global structure tdsrlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  DL UM LI Size if found else TDSINVALID_DL_AM_PDU_RLC_SIZE
 

SIDE EFFECTS

  None

===========================================================================*/

tdsrrc_rlc_dl_um_li_e_type tdsrrc_get_dl_um_li
(
  tdsrrc_RB_Identity rb_id
);



/*===========================================================================

FUNCTION tdsrrc_get_dl_am_rlc_pdu_size

DESCRIPTION

  This function finds the DL AM RLC PDU Size as per the RB specified from the 
  global structure tdsrlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  DL AM RLC PDU Size if found else TDSINVALID_DL_AM_PDU_RLC_SIZE
 

SIDE EFFECTS

  None

===========================================================================*/

uint32 tdsrrc_get_dl_am_rlc_pdu_size
(
  tdsrrc_RB_Identity rb_id
);


/*===========================================================================

FUNCTION RRC_SET_HSDPA_STATUS

DESCRIPTION

  This function sets the HSDPA Status Variable.
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/

void tdsrrc_set_hsdpa_status
(
  tdsrrc_hsdpa_status_enum_type status
);

/*==========================================================
FUNCTION  : tdsrrc_get_hsdpa_action()

DESCRIPTION : Gets the HSDPA Action variable

DEPENDENCIES: None.

RETURN VALUE: Action

SIDE EFFECTS: None.
============================================================*/
tdshsdpa_action_enum_type tdsrrc_get_hsdpa_action
(
  void
);


/*===========================================================================

FUNCTION RRC_SET_HSDPA_ACTION

DESCRIPTION

  This function sets the HSDPA Action Variable.
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/

void tdsrrc_set_hsdpa_action
(
  tdshsdpa_action_enum_type action
);


/*===========================================================================

FUNCTION          RRC_TRANSLATE_HRNTI


DESCRIPTION       

  This function translates HRNTI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external HRNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  Success/Failure

SIDE EFFECTS
                  
  None

===========================================================================*/
uecomdef_status_e_type tdsrrc_translate_hrnti
(
  ASN1BitStr32  *ext_hrnti,          /* Pointer to external HRNTI type */
  uint16  *int_hrnti    /* Pointer to internal HRNTI type */
);


/*===========================================================================

FUNCTION tdsrrc_initialize_rlc_size_per_rb

DESCRIPTION

  This function initializes the RLC Size per RB in the global structure
  tdsrlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/

void tdsrrc_initialize_rlc_size_per_rb
(
  void
);



/*===========================================================================

FUNCTION tdsrrc_initialize_rlc_size_change

DESCRIPTION

  This function initializes the RLC Size Change structure
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/
void tdsrrc_initialize_rlc_size_change
(
  void
);

/*===========================================================================

FUNCTION   tdsrrc_is_given_rb_rel_list_releases_all_rb_for_voice_rab

DESCRIPTION

  This function determines whether the given 'tdsrrc_RB_InformationReleaseList'
  will result in release of voice call or not.
        
DEPENDENCIES

  None
 
RETURN VALUE

  TRUE: If voice RAB is not present in est_rabs or if rb_release_list will result
        in releasing all RBs associated with RAB.
  FALSE: If voice RAB still exist after processing rb_release_list.

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_is_given_rb_rel_list_releases_all_rb_for_voice_rab 
(
  tdsrrc_RB_InformationReleaseList *rb_release_list_ptr
);


#ifdef FEATURE_TDSCDMA_HSUPA
/*===========================================================================

FUNCTION          RRC_TRANSLATE_ERNTI


DESCRIPTION       

  This function translates Primary and Secondary ERNTI type defined by ASN.1 
  into the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external Primary ERNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  Success/Failure

SIDE EFFECTS
                  
  None

===========================================================================*/
uecomdef_status_e_type tdsrrc_translate_ernti
(
  ASN1BitStr32  *ext_ernti,          /* Pointer to external ERNTI type */
  uint16  *int_ernti    /* Pointer to internal ERNTI type */
);

#endif //FEATURE_TDSCDMA_HSUPA 



/*===========================================================================

FUNCTION RRC_HSDPA_INITIAL_VALIDATION_R6

DESCRIPTION

  This function does the initial level validation of HSDPA.
  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type tdsrrc_hsdpa_initial_validation_r6
(    
  tdsrrc_RRC_StateIndicator rrc_state_from_msg,
  tdsrrc_DL_HSPDSCH_Information_r6* hspdsch_info_ptr
);


/*===========================================================================

FUNCTION RRC_HSDPA_INITIAL_VALIDATION

DESCRIPTION

  This function does the initial level validation of HSDPA.
  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type tdsrrc_hsdpa_initial_validation
(    
  tdsrrc_RRC_StateIndicator rrc_state_from_msg,
   boolean rrc_mode_Specific_Info_fdd_chosen
);


/*===========================================================================

FUNCTION   tdsrrc_get_idx_in_rrc_est_rabs_for_rab_id

DESCRIPTION

  This function returns the index in tdsrrc_est_rabs for a given rab_id.
        
ARGUMENTS

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
uint8 tdsrrc_get_idx_in_rrc_est_rabs_for_rab_id
(
  rrc_rab_id_type rab_id
);


/*===========================================================================

FUNCTION   tdsrrc_get_rab_type_for_given_rab_id 

DESCRIPTION

  This function returns the RAB type of a given RAB id. This function returns
  TDSRRCRB_UNKNOWN_RAB if RAB with a given RAB id is not found.
        
DEPENDENCIES

  None.

ARGUMENTS
  rab_Id: RAB Id

RETURN VALUE

  tdsrrcrb_rab_e_type: rab type

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_get_rab_type_for_given_rab_id 
(
  uint32 rab_id
);


/*===========================================================================

FUNCTION   tdsrrc_get_rab_type_for_given_rb_id 

DESCRIPTION

  This function returns the RAB type of a RAB associated with a given RB id. This function
  returns TDSRRCRB_UNKNOWN_RAB if there is no RAB exist for a given RB id.
        
DEPENDENCIES

  None.

ARGUMENTS
  rbId: rb Id

RETURN VALUE

  tdsrrcrb_rab_e_type: rab type

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrcrb_rab_e_type tdsrrc_get_rab_type_for_given_rb_id 
(
  uint32 rb_id
);


/*===========================================================================

FUNCTION   tdsrrc_get_rb_id_list_associated_with_voice_rab

DESCRIPTION

  This function gives the list of RB ids associated with a Voice RAB.
        
DEPENDENCIES

  None.

ARGUMENTS
  *num_rb: number of rb associated with Voice RAB, set to 0 if Voice RAB is not established.
                set 'num_rb' to NULL if this function needs to be called to know whether AMR is 
                established or not.
  rb_id_list: list of rbIds associated with Voice RAB

RETURN VALUE

  boolean: TRUE if Voice RAB is established elase FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_get_rb_id_list_associated_with_voice_rab 
(
  uint32 *num_rb,
  uint32 rb_id_list[MAX_RB_PER_RAB]
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_URA_ID


DESCRIPTION       

  This function translates URA_ID type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external URA_ID pointers must be initialized.
  If either one is NULL, the conversion will return .

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void tdsrrc_translate_ura_id
(
  ASN1BitStr32  *ext_ura_id,          /* Pointer to external C-RNTI type */
  tdsrrc_int_ura_id_type  *int_ura_id    /* Pointer to internal C-RNTI type */
);

/*===========================================================================
FUNCTION   RRC_FIND_CN_DOMAIN_ID_FOR_RB_IN_EST_RABS

DESCRIPTION
  For user plane RB ids it checks whether the rb is present in the established rbs. 
  It returns the corresponding cn_domain

DEPENDENCIES
  None.
 
RETURN VALUE
  rrc_rab_id_type rab_id

SIDE EFFECTS
  None.
===========================================================================*/
boolean tdsrrc_find_cn_domain_id_for_rb_in_est_rabs
(
  tdsrrc_RB_Identity rb_id,
  rrc_cn_domain_identity_e_type *cn_domain_id
);



/*===========================================================================

FUNCTION RRC_CHECK_RABS_ASSOCIATED_WITH_CN_DOMAIN

DESCRIPTION

  This function looks into the ESTABLISHED_RABS and checks if there are any
  RABs associated with the particular CN Domain
  
DEPENDENCIES


RETURN VALUE
  Returns TRUE if there are any RABs associated with that particular 
  CN Domain. Otherwise it returns FALSE.
 

SIDE EFFECTS

  None

===========================================================================*/
boolean tdsrrc_check_rabs_associated_with_cn_domain
(    
  rrc_cn_domain_identity_e_type rrc_cn_domain_id
);


/*===========================================================================

FUNCTION RRC_TRANSLATE_STATE_INDICATOR_TO_STATE

DESCRIPTION
  This function converts the Received RRC state indicator into internal
  RRC state format
  
DEPENDENCIES
  None

RETURN VALUE
  tdsrrc_state_e_type : returns the RRC state
  
SIDE EFFECTS

  None

===========================================================================*/
tdsrrc_state_e_type tdsrrc_translate_state_indicator_to_state
(    
  tdsrrc_RRC_StateIndicator state_indicator
);


/*===========================================================================

FUNCTION RRC_CONVERT_T305_TO_MS

DESCRIPTION
  This function converts the tdsrrc_T_305 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 tdsrrc_convert_t305_to_ms
(
  tdsrrc_T_305 rrccu_t_305                    /* enum for Timer T305 */
);


/*===========================================================================

FUNCTION RRC_CHECK_RBS_ASSOCIATED_WITH_RE_EST_TIMER

DESCRIPTION

  This function looks into the ESTABLISHED_RABS and checks if there are any
  RBs associated with the particular re-estblishment timer
  
DEPENDENCIES


RETURN VALUE
  Returns TRUE if there are any RBs associated with that particular 
  re_establishment timer. Otherwise it returns FALSE.
 

SIDE EFFECTS

  None

===========================================================================*/
boolean tdsrrc_check_rbs_associated_with_re_est_timer
(    
  tdsrrc_Re_EstablishmentTimer re_est_timer
);


/*===========================================================================
FUNCTION  tdsrrc_initialize_ue_conn_timers_and_constants

DESCRIPTION
  This function Initializes tdsrrc_ue_conn_timers_and_constants to default values
  as all parameters are mandatory default
  
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  Initializes the global variable tdsrrc_ue_conn_timers_and_constants with
  default values as all parameters are Mandatory Default
===========================================================================*/
void tdsrrc_initialize_ue_conn_timers_and_constants
(
  void
);

/*===========================================================================
FUNCTION  tdsrrc_update_ue_conn_timers_and_constants

DESCRIPTION
  This function updates tdsrrc_ue_conn_timers_and_constants based on updates
  received in sib1 or Utran Mobility Info messages
  
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  Updates the global variable tdsrrc_ue_conn_timers_and_constants
===========================================================================*/
uecomdef_status_e_type tdsrrc_update_ue_conn_timers_and_constants
(
  tdsrrc_UE_ConnTimersAndConstants* ue_conn_timers_and_constants_ptr,
  tdsrrc_UE_ConnTimersAndConstants_v3a0ext * ext_ptr
#if defined(FEATURE_3GPP_FAST_DORMANCY)
  ,tdsrrc_UE_ConnTimersAndConstants_v860ext *ue_conn_timers_and_constants_v860ext_ptr
#endif
);

 /*===========================================================================
FUNCTION  tdsrrc_bcch_and_nas_plmns_match

DESCRIPTION
  This function does a matching of BCCH and NAS supplied PLMNs. It further
  calls NAS supplied finctions to take care of the special case when NAS
  PLMN is a Home PLMN. This is bacause in that special case, a simple
  memcmp is not correct.

DEPENDENCIES
  Dependency on NAS supplied routines.

RETURN VALUE
  TRUE:  If NAS and BCCH PLMNs match.
  FALSE: If NAS and BCCH PLMNs do not match.

SIDE EFFECTS
  The two passed PLMNs have to be in the correct order. The first parameter
  is expected as the PLMN received on BCCH and the second one is NAS supplied
  PLMN.
  This utility is specifically for comparison between NAS and BCCH PLMNs
  and should NEVER be used for PLMN comparison between 2 NAS supplied PLMNs
  or 2 BCCH PLMNs. 
===========================================================================*/
boolean   tdsrrc_bcch_and_nas_plmns_match
(
  rrc_plmn_identity_type  bcch_plmn,
  rrc_plmn_identity_type  nas_plmn
);



/*===========================================================================

FUNCTION  tdsrrc_rssi_ber_timeout_handler

DESCRIPTION

  This function is invoked when the RSSI&BER repoting timer expires. It calls
  to L1 to get RSSI and BER info. If the value of RSSI or BER is changed,
  the function will send a RRC-CM interface primitive to inform the changes.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void tdsrrc_rssi_ber_timeout_handler
( 
  void 
);


/*===========================================================================

FUNCTION    tdsrrc_get_ul_dcch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.
SIDE EFFECTS
  None

===========================================================================*/
uint8 tdsrrc_get_ul_dcch_message_type(
  uint8 *ul_sdu,                            /* Pointer received byte stream */
  uint32 ul_sdu_length                      /* Length of SDU received */
);


/*===========================================================================

FUNCTION    tdsrrc_get_dl_ccch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.

SIDE EFFECTS
  None

===========================================================================*/
uint8 tdsrrc_get_dl_ccch_message_type(
  uint8 *dl_sdu,                            /* Pointer received byte stream */
  uint32 dl_sdu_length                      /* Length of SDU received */
);
/*===========================================================================

FUNCTION    tdsrrc_get_dl_dcch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.
SIDE EFFECTS
  None

===========================================================================*/
uint8 tdsrrc_get_dl_dcch_message_type(
  uint8 *dl_sdu,                            /* Pointer received byte stream */
  uint32 dl_sdu_length                      /* Length of SDU received */
);

/*===========================================================================

FUNCTION    tdsrrc_get_ul_ccch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.

SIDE EFFECTS
  None

===========================================================================*/
uint8 tdsrrc_get_ul_ccch_message_type(
  uint8 *ul_sdu,                            /* Pointer received byte stream */
  uint32 ul_sdu_length                      /* Length of SDU received */
);

/*===========================================================================

FUNCTION tdsrrc_convert_initial_ue_identity_into_int_value

DESCRIPTION
  This function converts Initial UE Identity from internal format to integer value.


DEPENDENCIES
  None

RETURN VALUE
  uint64: returns an integer value of the Initial UE dentity

SIDE EFFECTS
  None
===========================================================================*/
uint64 tdsrrc_convert_initial_ue_identity_into_int_value
(
  void
);


/*===========================================================================

FUNCTION          RRC_CONVERT_GSM_MAP_IMSI_INTO_INT_VALUE


DESCRIPTION       

  This function translates a GSM-MAP IMSI internal type  into
  an interger value type used by RRC LLC. 

DEPENDENCIES
  None

RETURN VALUE
                  
  uint64: returns an integer value of the IMSI

SIDE EFFECTS
                  
  None

===========================================================================*/
uint64 tdsrrc_convert_gsm_map_imsi_into_int_value 
(
  rrc_imsi_gsm_map_type  int_imsi    /* Pointer to internal IMSI type */
);


/*===========================================================================

FUNCTION          RRC_TRANSLATE_CRNTI


DESCRIPTION       

  This function translates C-RNTI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external C-RNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void tdsrrc_translate_crnti
(
  ASN1BitStr32  *ext_crnti,         /* Pointer to external C-RNTI type */
  tdsrrc_int_c_rnti_type  *int_crnti    /* Pointer to internal C-RNTI type */
); 


/*===========================================================================

FUNCTION          tdsrrc_translate_urnti


DESCRIPTION       

  This function translates U-RNTI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external U-RNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void tdsrrc_translate_urnti
(
  struct tdsrrc_U_RNTI  *ext_urnti,     /* Pointer to external U-RNTI type */
  tdsrrc_int_u_rnti_type  *int_urnti    /* Pointer to internal U-RNTI type */
); 

/*===========================================================================

FUNCTION   tdsrrc_remove_rb_from_established_rabs

DESCRIPTION

  This function removes the given RB id if it's present in the
  "ESTABLISHED_RABS" variable. If the removal of the RB causes
  a RAB to have no RBs mapped to it, the RAB will also be
  removed. 
        
DEPENDENCIES

  Pointer to hold the results must be initialized by the
  calling function.
 
RETURN VALUE

  None. (Results ptr is filled out by this function).

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_remove_rb_from_established_rabs
(
  tdsrrc_RB_Identity rb_id,                  /* RB id to be removed */
  tdsrrc_rb_remove_result_type *ptr /* Pointer for holding the results */
);


/*===========================================================================

FUNCTION   tdsrrc_remove_rab_from_established_rabs

DESCRIPTION

  This function removes the given RAB id if it's present in the
  "ESTABLISHED_RABS" variable. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_remove_rab_from_established_rabs
(
  rrc_rab_id_type rab_id /* Id of the RAB */
);



/*===========================================================================

FUNCTION   VALIDATE_RB_ID_IN_RELEASE_LIST

DESCRIPTION

  This function validates the rb_ids in the received RB Release Message.
  If the release is for rb-id 0 through 3 then this returns TDSRRC_RB_NOT_PRESENT
  because SRB0-SRB3 should not be in the release list. For user plane RB ids and RB4,
  it checks whether the rb to be released is present in the established rbs. 
  If not this function returns TDSRRC_RB_NOT_PRESENT.

DEPENDENCIES

  None.
 
RETURN VALUE

  tdsrrc_rb_search_e_type 
          TDSRRC_RB_PRESENT     :rb-id exists in the ESTABLISHED_RAB variable or
                              it is a signalling RB.
          TDSRRC_RB_NOT_PRESENT :rb-id is signalling RB <= SRB3 or 
                              rb-id is not a signalling RB and it also
                              does not exist in the ESTABLISHED_RAB variable.

SIDE EFFECTS

  None.

===========================================================================*/

tdsrrc_rb_search_e_type tdsrrc_validate_rb_id_in_release_list
(
  tdsrrc_RB_Identity rb_id
);


/*===========================================================================

FUNCTION   VALIDATE_RB_ID_IN_RECONFIG_LIST

DESCRIPTION

  This function validates the rb_ids in the received RB Reconfig Message.
  If signalling RB id is present it returns TDSRRC_RB_PRESENT. For user plane RB ids
  and SRB4 it checks whether the rb to be reconfigured or affected is present in the 
  established rbs. If not this function returns TDSRRC_RB_NOT_PRESENT.

DEPENDENCIES

  None.
 
RETURN VALUE

  tdsrrc_rb_search_e_type 
          TDSRRC_RB_PRESENT     :rb-id exists in the ESTABLISHED_RAB variable or
                              it is a signalling RB.
          TDSRRC_RB_NOT_PRESENT :rb-id is not a signalling RB and it also
                              does not exist in the ESTABLISHED_RAB variable.

SIDE EFFECTS

  None.

===========================================================================*/

tdsrrc_rb_search_e_type tdsrrc_validate_rb_id_in_reconfig_list
(
  tdsrrc_RB_Identity rb_id
);

/*===========================================================================

FUNCTION   tdsrrc_find_rb_in_est_rabs

DESCRIPTION

  This function validates the rb_ids in the received RBR Message.
  For user plane RB ids it checks whether the rb to be released
  is present in the established rbs. If not this function returns FALSE.

DEPENDENCIES

  None.
 
RETURN VALUE

  Boolean 
          RRC_RB_FOUND     :rb-id exists in the ESTABLISHED_RAB variable.
          TDSRRC_RB_NOT_FOUND :rb-id does not exist in the ESTABLISHED_RAB variable.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_rb_search_e_type tdsrrc_find_rb_in_est_rabs
(
  tdsrrc_RB_Identity rb_id
);



/*===========================================================================

FUNCTION   tdsrrc_find_rab_in_established_rabs

DESCRIPTION

  This function checks if the given RAB id is present in the
  "ESTABLISHED_RABS" variable. If so, it returns TDSRRC_RAB_FOUND.
  Else it returns TDSRRC_RAB_NOT_FOUND.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_rab_search_e_type tdsrrc_find_rab_in_established_rabs
(
  rrc_rab_id_type rab_id /* Id of the received RAB */
);

/*===========================================================================

FUNCTION   tdsrrc_add_rab_to_established_rabs

DESCRIPTION

  This function adds a new RAB to the ESTABLISHED_RABS variable.
  The caller must verify that the RAB is not already in the
  variable by calling RRC_FIND_RAB_IN_ESTABLISHED_RABS.
        
DEPENDENCIES

  All the relevant information for the RAB must be included
  in the pointer that's passed in to this function.
  
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_add_rab_to_established_rabs
(
  tdsrrc_est_rab_info_type *ptr /* Pointer to the RAB information */
);

#ifdef FEATURE_UMTS_PDCP
/*===========================================================================

FUNCTION tdsrrc_get_tds_rrc_version_nv

DESCRIPTION
  This  function gets the NV item NV_WCDMA_RRC_VERSION_I and stores it in 
  rrc global variable 'tdsrrc_nv_rel_indicator'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsrrc_get_tds_rrc_pdcp_disabled_nv
(
void
);

#endif /* FEATURE_UMTS_PDCP */

/*===========================================================================

FUNCTION          RRC_TRANSLATE_GSM_MAP_IMSI


DESCRIPTION       

  This function translates a GSM-MAP IMSI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external IMSI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void tdsrrc_translate_gsm_map_imsi
(
  tdsrrc_IMSI_GSM_MAP *ext_imsi, /* Pointer to external IMSI type */
  rrc_imsi_gsm_map_type  *int_imsi   /* Pointer to internal IMSI type */
);

/*===========================================================================

FUNCTION          tdsrrc_translate_cell_id


DESCRIPTION       

  This function translates a Cell Id type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  None
  
RETURN VALUE
                  
  The Cell_id converted to the internal type tdsrrc_int_cell_id_type

SIDE EFFECTS
                  
  None

===========================================================================*/
tdsrrc_int_cell_id_type tdsrrc_translate_cell_id
(
  ASN1BitStr32  *ext_cell_id         /* Pointer to external PLMN type */
);


/*===========================================================================

FUNCTION          tdsrrc_translate_gsm_map_plmn_id_with_optional_mcc


DESCRIPTION       

  This function translates a GSM-MAP PLMN Id with optinal MCC type defined by ASN.1 into
  the internal type used by RRC and RRC-MM interface. And also indicates whether the MCC 
  of the externel PLMN type is present or not.

DEPENDENCIES

  Both the internal and external PLMN pointers must be initialized.
  If either one is NULL, the convertion will fail. MCC of external
  PLMN type is optional. Otherwise the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void tdsrrc_translate_gsm_map_plmn_id_with_optional_mcc
(
  boolean *is_mcc_valid,
  tdsrrc_PLMN_IdentityWithOptionalMCC_r6  *ext_plmn,      /* Pointer to external PLMN type */
  rrc_plmn_identity_type  *int_plmn  /* Pointer to internal PLMN type */
);

/*===========================================================================

FUNCTION          tdsrrc_translate_gsm_map_plmn_id


DESCRIPTION       

  This function translates a GSM-MAP PLMN Id type defined by ASN.1 into
  the internal type used by RRC and RRC-MM interface. 

DEPENDENCIES

  Both the internal and external PLMN pointers must be initialized.
  If either one is NULL, the convertion will fail. MCC must have 3
  digits and MNC must have either 2 or 3 digits in the external
  PLMN type. Otherwise the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void tdsrrc_translate_gsm_map_plmn_id
(
  tdsrrc_PLMN_Identity  *ext_plmn,      /* Pointer to external PLMN type */
  rrc_plmn_identity_type  *int_plmn  /* Pointer to internal PLMN type */
);

/*==========================================================================

FUNCTION tdsrrc_translate_establishment_cause

DESCRIPTION
  This function maps the external(ASN1) RRC Connection Establishment cause
  for the internal RRC Connection Established cause received from the NAS 
  through RRC_EST_REQ/OPEN_SESSION_REQ primitive.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  tdsrrc_translate_establishment_cause
(
   rrc_establish_cause_e_type est_cause_int,
   tdsrrc_EstablishmentCause *est_cause_ext   
);

#ifdef FEATURE_TDSCDMA_VOC_AMR_WB
/*===========================================================================

FUNCTION tdsrrc_get_current_codec_mode

DESCRIPTION
  This  function returns that the Current codec mode type that is set 

DEPENDENCIES
  None.

RETURN VALUE
  mvs_mode_type: rteurns the  Current codec mode type that is set 

===========================================================================*/
mvs_mode_type tdsrrc_get_current_codec_mode
(
  void
);

/*===========================================================================

FUNCTION tdsrrc_set_current_codec_mode

DESCRIPTION
  This  function sets the current codec mode with value configured in MVS
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void tdsrrc_set_current_codec_mode
(
  mvs_mode_type new_codec_mode
);

/*===========================================================================

FUNCTION  tdsrrc_translate_nas_sync_indicator

DESCRIPTION This function translates a Nas-Syncronisation-indicator defined by
 ASN.1 into the internal type used by RRC. 

DEPENDENCIES

  The algorithm used assumes that the processor is Little Endian.
  
RETURN VALUE
                  
  Nas-Syncronisation-indicator converted to the internal type tdsrrc_codec_types


===========================================================================*/
mvs_mode_type tdsrrc_translate_nas_sync_indicator
(
  ASN1BitStr32  *nas_sync_indicator         /* Pointer to external PLMN type */
);

/*===========================================================================

FUNCTION  tdsrrc_set_ordered_codec_mode

DESCRIPTION This function sets the codec mode ordered in the ota messages

DEPENDENCIES
  None
  
RETURN VALUE          
  None

===========================================================================*/
void tdsrrc_set_ordered_codec_mode
(
  mvs_mode_type nas_sync_ind
);

/*===========================================================================

FUNCTION  tdsrrc_get_ordered_codec_mode

DESCRIPTION This function sets the codec mode ordered in the ota messages

DEPENDENCIES
  None
  
RETURN VALUE          
  None

===========================================================================*/
mvs_mode_type tdsrrc_get_ordered_codec_mode
(
  void
);

#endif /*FEATURE_TDSCDMA_VOC_AMR_WB*/

#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif /* FEATURE_SCUDIF_ENHANCEMENT */

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION TDSRRC_SET_AS_ID

DESCRIPTION           This function sets the AS ID
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrrc_set_as_id
(
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION RRC_GET_AS_ID

DESCRIPTION           This function returns as_id received from NAS if received
                             If not received, returns SYS_MODEM_AS_ID_1
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
sys_modem_as_id_e_type tdsrrc_get_as_id
(
  void
);
#endif

//controversial (call by l1)
#if defined FEATURE_TDSCDMA_TRM || defined(FEATURE_TDSCDMA_DSDS)
/*===========================================================================

FUNCTION tdsrrc_check_if_tds_can_be_stopped

DESCRIPTION
  This  function is called by L1 to check if TDS stack can be stopped ie PNR
  (Point of no return) has not been reached. If PNR has not been reached,
  the function calls  cm function to post a special stop to RRC and returns
  TRUE to L1. 

  If PNR has been reached the function returns FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean tdsrrc_check_if_tds_can_be_stopped
(
  void
);
#endif

//controversial (call by nas)
#if  defined(FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING)
/*===========================================================================

FUNCTION rrc_register_mvs_callbacks_to_l1

DESCRIPTION
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrrc_register_mvs_callback_to_l1
(
  mvssup_wcdma_blk_size_cb_set_type * tdsamr_cb_set
);
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING*/

#if defined (FEATURE_TDSCDMA_RRC_INDICATE_CODEC_TO_CM) || defined (FEATURE_TDSCDMA_VOC_AMR_WB)
/*===========================================================================
FUNCTION tdsrrc_get_ordered_codec_status

DESCRIPTION
  This  function returns that the ordred codec mode status

DEPENDENCIES
  None.

RETURN VALUE
  ordered codec status

===========================================================================*/
boolean tdsrrc_get_ordered_codec_status
(
  void
);

/*===========================================================================

FUNCTION tdsrrc_set_ordered_codec_status

DESCRIPTION
  This  function sets the ordered codec status
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void tdsrrc_set_ordered_codec_status(boolean);
#endif

#ifdef FEATURE_TDSCDMA_RRC_INDICATE_CODEC_TO_CM
/*===========================================================================

FUNCTION tdsrrc_send_hho_cmd_to_cm

DESCRIPTION


  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
 void tdsrrc_send_hho_cmd_to_cm(sys_codec_ho_cmd );

/*===========================================================================

FUNCTION tdsrrc_send_codec_info_to_cm

DESCRIPTION


  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
 void tdsrrc_send_codec_info_to_cm(mvs_mode_type );
/*===========================================================================

FUNCTION tdsrrc_send_codec_info_on_activation_timer_expiry

DESCRIPTION
  This function sends codec info to CM on activation timer expiry for L1

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_send_codec_info_on_activation_timer_expiry(void);
#endif

#if defined(FEATURE_INTERRAT_HANDOVER_G2TD) || defined (FEATURE_TDSCDMA_L2TD_PS_HANDOVER)
/*===========================================================================

FUNCTION  tdsrrc_is_x2td_irat_hho_pending

DESCRIPTION
  This function get the state from iho to check if there is x2td irat
  hho is pending channel configuration. 

DEPENDENCIES


RETURN VALUE
  TRUE: there is pending channel configuration from IHO
  FALSE: no pending channel configuration from IHO

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsrrc_is_x2td_irat_hho_pending(void);

/*===========================================================================

FUNCTION  tdsrrc_is_x2td_irat_hho_timing

DESCRIPTION
  This function get the status from iho to check if there is x2td irat
  hho ongoing with timing info from x rat. 

DEPENDENCIES


RETURN VALUE
  TRUE: there is ongoing x2td irat hho with timing info from x rat
  FALSE: otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsrrc_is_x2td_irat_hho_timing(void);
#endif

extern void tds_rrc_init_tcb(void);

extern rex_tcb_type* tds_rrc_get_tcb(void);
#ifdef FEATURE_TDSCDMA_HSUPA
/*===========================================================================

FUNCTION tdsrrc_event_log_hsupa_support_status

DESCRIPTION

  Event logging for HSUPA support status
  
DEPENDENCIES


RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void tdsrrc_event_log_hsupa_support_status(sys_hs_ind_e_type hsdpa_hsupa_indication);
#endif
#ifdef FEATURE_TDSCDMA_DSDS_TUNEAWAY
/*===========================================================================

FUNCTION  tdsrrc_ok_for_tune_away

DESCRIPTION

  This function checks whether UE allows tune away or not
    
DEPENDENCIES
  None.
 
RETURN VALUE

 None
 
SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrc_ok_for_tune_away(void);
/*===========================================================================

FUNCTION  tdsrrc_check_send_change_priority

DESCRIPTION

  This function sends change priority request to WRM if necessary.
    
DEPENDENCIES
 This functon is called during addition , modification and deletion of RABs
  None.
 
RETURN VALUE

  Lock status.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_check_send_change_priority
(
  void
);
/*===========================================================================

FUNCTION  tdsrrc_change_priority_on_tuneaway_disable

DESCRIPTION

  This change priority on disabling tuneaway

DEPENDENCIES
  None.
 
RETURN VALUE

  Lock status.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_change_priority_on_tuneaway_disable
(
  void
);
#endif

/*===========================================================================

FUNCTION  tdsrrc_check_L1_accumulated_rscp

DESCRIPTION

  This function calls L1 to get serving cell signal status info (RSSI/RSCP/ECIO/SINR/BLER)
  and return RSCP *10

DEPENDENCIES

  None.

RETURN VALUE

  RSCP *10

SIDE EFFECTS

  None.

===========================================================================*/
int16 tdsrrc_check_L1_accumulated_rscp( void );

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION tdsrrc_register_mcfg_refresh_notify

DESCRIPTION

  This function registers notify callback for MCFG refresh event. 
  
DEPENDENCIES


RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void tdsrrc_register_mcfg_refresh_notify(void);

/* Move declaration of tdsrrc_process_mcfg_refresh_ind to tdsrrcdispatcher.c, as tdsrrc_cmd_type is defined in tdsrrccmd_v.h,
   if we include this file (tdsrrccmd_v.h) in tdsrrcdata.h, it will show many compilation errors */
#if 0 
/*===========================================================================

FUNCTION tdsrrc_process_mcfg_refresh_ind

DESCRIPTION

  This function processes tdsrc internal msg TDSRRC_MCFG_REFRESH_IND, and calls MCFG API to notify TDS completes
  handling MCFG refresh event. Currently this function only logs the mcfg refresh event info without re-reading NVs, since
  we only have one MCFG setting for TDS.
  
DEPENDENCIES


RETURN VALUE

None

SIDE EFFECTS

  None

===========================================================================*/
void tdsrrc_process_mcfg_refresh_ind
(
  tdsrrc_cmd_type *cmd_ptr
);
#endif
#endif

#endif
