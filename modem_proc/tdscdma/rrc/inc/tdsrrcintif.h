#ifndef TDSRRCINTIF_H
#define TDSRRCINTIF_H
/*===========================================================================
         R R C   I N T E R N A L   C O M M A N D    I N T E R F A C E S

DESCRIPTION

  This module contains definitions for commands used 
  for RRC's internal command queue. These are commands sent from
  one RRC procedure to another. These commands can be queued
  only into rrc's internal command queue.


Copyright (c) 2010-2012 Qualcomm Technologies, Incorporated. 
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrcintif.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/11   ysh     Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53
01/14/11   cxd     Remove the following L1 external commands
                   TDSL1_CPHY_CAPABILITY_REQ           =   0x0,
                   TDSL1_CPHY_ASET_UPDATE_REQ          =   0xA,
                   TDSL1_CPHY_HARD_HANDOFF_REQ         =   0xB,
                   TDSL1_CPHY_BCH_SCHEDULE_REQ         =   0xC,
                   TDSL1_CPHY_INTEGRITY_KEY_REQ        =   0x13,
                   TDSL1_CPHY_INTEGRITY_KEY_MAUC_REQ   =   0x14,
                   TDSL1_CPHY_IMMEDIATE_RESELECTION_REQ=   0x15,
                   TDSL1_CPHY_CELL_RESELECTION_RSP     =   0x16,
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad      Added changes to refeaturize the API files
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/24/11   ad      Made changes to fix compilation errors on 76xx target for SW decoupling
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/17/11   ad      Made changes in RRC related to L2 SW Decoupling
01/17/11   ad      Made changes to fix the compilation errors of RRC SW Decoupling
01/12/11   ad      Added changes for RRC SW decoupling
11/08/10   ysh     Add Uplink Physical Channel Control. 3Gpp-8.2.10(3.2.6)
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/14/10   rm      Replaced gstki.h inclusion with gstk_exp_v.h for CMI targets
10/11/10   rm      Added code to support 3GPP CSFB
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM_WCDMA_TUNEAWAY

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsuecomdef.h"
#include "tdsrrcmm_v.h"
#include "tdsrrctask.h"
#include "tdsl1rrcif.h"
#include "tdsl1mtask.h"
#include "tdsl2dltask.h"
#include "tdsl2ultask.h"
#include "sys_plmn_selection.h"
#include "tdsrrcdata.h"
#include "tdsrrccommon.h"

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
#ifdef FEATURE_CMI
#include "gstk_exp_v.h"
#else
#include "gstki.h"
#endif
#endif

#include "lte_rrc_irat_msg.h"
#include "tds_rrc_msg.h"

#include "tdsrrcqsh.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Command structures for all internal commands included here */

/*---------------------------------------*/
/*                                       */
/*     Structure for RRC_GET_ALL_SIBS    */
/*     Command.                          */
/*                                       */
/*---------------------------------------*/


/* Use to send a command to SIB procedure to collect all SIBs for a
new cell When this command is sent it's assumed that the cell is
a serving cell. This command cannot be sent for a Neighbor cell. */
typedef struct
{
  uint32 freq;            /* Frequency of the new cell */
  uint32 cpid; /* Scrambling Code of the new cell */  
  tdsrrc_proc_e_type procedure;  /* The procedure requesting the SIBs */
}tdsrrc_get_all_sibs_req_type;

/* enum type for sib reading to distinguing different handling of GET_SPECIFIC_SIBS for
   normal, background manual vs automatic */
typedef enum
{
  TDSRRC_SIB_NORMAL,
  TDSRRC_SIB_AUTO_BPLMN_SCAN,  
  RRC_SIB_TD2TD_AUTO_BPLMN_SCAN,
  RRC_SIB_TD2TD_MANUAL_BPLMN_SCAN,
  TDSRRC_SIB_MANUAL_BPLMN_SCAN,
  TDSRRC_SIB_MANUAL_SCAN,
  TDSRRC_SIB_CELL_ID_READ_IN_DCH_SCAN,
  TDSRRC_SIB_READ_FROM_SIB_CONTAINER,
  TDSRRC_SIB_DUMMY_PLMN_SCAN
}tdsrrc_sib_scan_e_type;


#define TDSRRC_MAX_SIB_CONT_LEN 0x1000 /*4096 Bytes*/
typedef struct
{
  uint16 cont_len;
  uint8 *cont_ptr;  /*Use a pointer */
} tdsrrc_System_Information_Container_type;

/*---------------------------------------*/
/*                                       */
/*   Structure for RRC_GET_SPECIFIC_SIBS */
/*   Command.                            */
/*                                       */
/*---------------------------------------*/


/* Use to send a command to SIB procedure to collect specific SIBs for a
cell */
typedef struct
{
  
  uint32 freq;            /* Frequency of the cell */
  uint8 cpid; /* Scrambling Code of the cell */
  boolean for_serv_cell;  /* Set to TRUE if this is for the
                          current serving cell. Set to FALSE if
                          it's for a neighbor cell. */
  boolean sib_req[TDSRRC_TOT_SIBS];
                                /* Set each SIB that is needed
                                 to TRUE. Other SIBs should be set
                                 to false. Use "tdsrrc_SIB_Type" from
                                 tdsrrcasn1.h to fill up this array. */
  tdsrrc_proc_e_type  procedure;  /* The procedure requesting the SIBs */
  tdsrrc_sib_scan_e_type  sib_scan_type;
                                
  tdsrrc_System_Information_Container_type  *sys_info_cont_ptr;
}tdsrrc_get_specific_sibs_req_type;

/*---------------------------------------*/
/*                                       */
/*  Structure for TDSRRC_GET_ALL_SIBS_CNF,  */
/*  and TDSRRC_GET_SPECIFIC_SIBS_CNF        */
/*  Commands.                            */
/*                                       */
/*---------------------------------------*/
/* Enumerated type of status to be sent to the calling
entity */
typedef enum
{
  TDSSIB_EVENT_SUCCEEDED,                 /* SIB event succeeded */
  TDSSIB_EVENT_FAILED_LOWER_LAYERS,       /* SIB event failed due to failure to configure lower layers */
  TDSSIB_EVENT_FAILED_INCOMPATIBLE_STATE, /* SIB event failed due to incompatible RRC state */
  TDSSIB_EVENT_FAILED_TIMEOUT,            /* SIB event failed due to SIB timeout */
  TDSSIB_EVENT_FAILED_INVALID_SIBS,       /* SIB event failed due to Invalid SIBs */
  TDSSIB_EVENT_FAILED_INVALID_EVENT,      /* SIB received invalid event */
  TDSSIB_EVENT_VALID_DUPLICATE_UPDATE_IND,/* SIB UPDATE event failed due to new UPDATE TDSEVENT */
  TDSSIB_EVENT_FAILED_FPLMN,              /* SIB event failed due to F(Forbidden)-PLMN  */
  TDSSIB_EVENT_FAILED_CELL_BAR,           /* SIB event aborted due to cell bar */
  TDSSIB_EVENT_FAILED_FREQ_BAR,           /* SIB event aborted due to frequency bar */
  TDSSIB_EVENT_FAILED_FB_LA,              /* SIB event aborted due to forbidden LA */
  TDSSIB_EVENT_FAILED_SCRITERIA,	  /* SIB event aborted due to S-criteria Failure*/
  TDSSIB_EVENT_FAILED_PLMN_MISMATCH,	  /* SIB event aborted due to PLMN mismatch */
  TDSSIB_EVENT_FAILED_OTHER               /* SIB event failed due to any other reason */
  //May need more
}tdsrrc_sib_status_e_type;

typedef enum
{
 TDSRRC_RNTI_UPDATE,                 /* Use the new RNTI */          
 TDSRRC_RNTI_DELETE,                 /* Delete the current RNTI */
 TDSRRC_RNTI_NOCHANGE                /* No change in RNTI status */
}tdsrrc_rnti_cmd_status_e_type;

typedef enum
{
  TDSRRC_FAILURE_NONE,
  TDSRRC_MAX_RACH_ATTEMPTS,
  TDSRRC_CONNECTION_REJECT,
  TDSRRC_CONN_FAILURE_OTHER
}tdsrrc_conn_setup_failure_e_type;

/* Structure to be used when sending an event to confirm the
reception of SIBs. */
typedef struct
{ 
    uint32 freq;                     /* Frequency of the cell */
  uint32 cpid;          /* Scrambling Code of the cell */
   uint32 sib_index_for_cell;       /* Index into SIB database where SIBs for
                                   this cell are stored. */
  
  tdsrrc_int_cell_id_type  cell_id;   /* CELL Id of the cell */
  
  tdsrrc_proc_e_type procedure;    /* The procedure that requested the SIBs */
  tdsrrc_sib_status_e_type status; /* Status of the SIB event-success or failure*/
  /* The following information is valid only if the event succeeded */
  rrc_plmn_identity_type plmn_id;  /* PLMN Id of the cell */

 
#ifdef FEATURE_TDSRRC_BAND_TYPE_DETECTION_FOR_BPLMN
  /* This Boolean will indicate whether SIB5bis is scheduled in this cell or not
   *  Value = TRUE means SIB5bis is scheduled in this cell
   *  Value = FLASE means SIB5 is scheduled in this cell
   *  Currently SIB fills this Boolean for only below CSP Scan types  
   * TDSRRC_CSP_PLMN_LIST_SCAN corresponding SIB scan type ?TDSRRC_SIB_MANUAL_SCAN
   * RRC_CSP_TD2TD_BPLMN_MANUAL_SCAN corresponding SIB scan type ?RRC_SIB_TD2TD_MANUAL_BPLMN_SCAN
   * RRC_CSP_G2TD_BPLMN_MANUAL_SCAN corresponding SIB scan type ?TDSRRC_SIB_MANUAL_BPLMN_SCAN
   * So, CSP to look at this Boolean only for above scan types
   */
  boolean is_sib5bis_scheduled; 
#endif /* FEATURE_TDSRRC_BAND_TYPE_DETECTION_FOR_BPLMN */
#ifdef FEATURE_3GPP_CSFB_TDS
  boolean is_sib_read_from_container;/* To know whether SIBs read from container or over OTA*/
#endif
}tdsrrc_sib_event_cnf_type;


/* This enumerates the status of UTRAN directed redirection */
typedef enum
{
  TDSRRC_REDIRECT_SUCCESS,
  TDSRRC_REDIRECT_FAILURE
}tdsrrc_redirect_status_e_type;

/* This enumerates the cause of success/failure for UTRAN directed redirection */
typedef enum
{
  /* UE was able to camp on the frequency designated by UTRAN */
  TDSRRC_CAMPED_ON_REDIRECTED_FREQ,

  /* UE was able to camp on the frequency and CPID designated by UTRAN */
  TDSRRC_CAMPED_ON_REDIRECTED_CELL,

  /* UE was not able to camp on the redirected frequency, but was able to
   * camp on another frequency different from the last camped frequency on
   * which RRC Connection Request was sent
   */
  TDSRRC_CAMPED_ON_DIFFERENT_FREQ,

  /* UE was not able to camp on the redirected frequency or any other frequency
   * before the expiry of the wait timer. After timer expiry, UE was able to
   * camp on the last camped frequency on which RRC Connection Request was sent
   */
  TDSRRC_CAMPED_ON_ORIGINAL_FREQ,

  /* UE failed to camp on any frequency and hence is OOS */
  TDSRRC_FAILED_TO_CAMP,

  /* UE received inter-frequency redirection which is not supported */
  TDSRRC_FREQ_REDIRECTION_NOT_SUPPORTED,

  /* UE received interRAT redirection which is not supported */
  TDSRRC_RAT_REDIRECTION_NOT_SUPPORTED,                           

  /* UE received redirection which is not supported */
  TDSRRC_REDIRECTION_NOT_SUPPORTED,

  /* RCE passed inconsistent data to CSP or RRC was not in Connecting State
   * when CSP received TDSRRC_REDIRECT_REQ from RCE
   */
  TDSRRC_RCE_INCOMPATIBLE,
  
  /* UE was not able to camp on GSM */
  TDSRRC_REDIRECT_GSM_FAILURE,

  /* UE was not able to camp on GSM and TDS could not be resumed */
  TDSRRC_REDIRECT_RESUME_FAILURE
}tdsrrc_redirect_cause_e_type;



/* Enumerated type for selecting type of re-direction */
typedef enum
{
  TDSRRC_FREQ_REDIRECTION,                    /* Frequency redirection */
  TDSRRC_INTER_SYSTEM_GSM,                   /* Inter-system redirection to GSM */
  TDSRRC_INTER_SYSTEM_LTE,                   /* Inter-system redirection to LTE */
}tdsrrc_redirect_e_type;

typedef struct
{
     uint16 uarfcn_UL;                       /* Uplink UTRA Absolute Radio
                                              Frequency Channel Number */
   uint16 uarfcn_DL;                       /* Downlink UTRA Absolute Radio
                                              Frequency Channel Number */
   boolean uarfcn_UL_present;              /* Boolean to indicate whether 
                                              Downlink UARFCN is included */
 
}tdsrrc_freq_info_type;

typedef enum
{
  TDSRRC_UE_CAPABILITY_ORIGINATOR_UE,        /* UE originated UE Update Capability
                                           procedure */
  TDSRRC_UE_CAPABILITY_ORIGINATOR_UTRAN,      /* UTRAN originated UE Update 
                                             Capability procedure  */

  
/*This is used by UECI procedure to differentiate between COUEC procedure and normal processing.*/

  TDSRRC_UE_CAPABILITY_ORIGINATOR_COUEC,      /* COUEC originated UE Update Capability procedure*/

  TDSRRC_UE_CAPABILITY_ORIGINATOR_NONE
} tdsrrc_ue_capability_originator_e_type;

/* Assigned  values to the enum for CU cause so that even if enums 
   are deleted or added CFA can still parse them
  */
typedef enum
{
  TDSRRC_CELL_RESELECTION =0x0,                   /* Cell Reselection */
  TDSRRC_PERIODIC_CELL_UPDATE =0x1,               /* Periodic Cell Update */
  TDSRRC_UL_DATA_TRANSMISSION=0x2,               /* UL Data Transmission */
  TDSRRC_PAGING_RESPONSE=0x3,                    /* Paging Response */
  TDSRRC_RADIO_LINK_FAILURE=0x4,                 /* Radio Link Failure */
  TDSRRC_REENTERED_SERVICE_AREA=0x5,             /* Re-entered Service area */
  TDSRRC_UNRECOVERABLE_ERROR=0x6,                 /* Unrecoverable error */
  TDSRRC_PHY_CHAN_FAILURE=0x7                    /* Physical Channel Failure */
#ifdef FEATURE_BPLMN_SEARCH_320MS
  ,TDSRRC_LOSSY_PAGE_RECEPTION =0x8
#endif
  ,TDSRRC_E_PCH_TO_FACH_TRANSITION_REQ = 0x9  /* ePCH to eFACH state transition Request */
  ,TDSRRC_CU_CAUSE_NONE =0xFF
} tdsrrc_cell_update_cause_e_type;

/* URA update causes */

typedef enum
{
  TDSRRC_URA_RESELECTION,                      /* Change of URA id */
  TDSRRC_PERIODIC_URA_UPDATE                 /* Periodic URA update */
}tdsrrc_ura_update_cause_e_type;


typedef enum
{
  TDSRRC_CELL_SEL_RADIO_LINK_FAILURE,        /* Cell selection due to radio link
                                             failure */
  TDSRRC_CELL_SEL_RB0_SETUP_FAILURE,         /* Cell selection due to radio link
                                             failure */
  TDSRRC_OUT_OF_SERVICE_AREA_IND,            /* Cell Selection due to Out of 
                                             service area */
  TDSRRC_TIMER_T316_EXPIRED,                 /* Timer T316 expired */

  TDSRRC_TRANSITION_FROM_DCH,                /* This is when cell selection is to 
                                           * be triggered going out from CELL_DCH to
                                           * any other connected mode state.
                                           */
  TDSRRC_TRANSITION_TO_DISCONNECTED,         /* This is when some procedure wants to 
                                           * take UE to IDLE and initiate cell selection
                                           */
  TDSRRC_TRANSITION_FROM_FACH,     

  TDSRRC_CELL_SEL_ON_MDSP_HALT,
  TDSRRC_CELL_SEL_MAX_CAUSE                 

}tdsrrc_cell_sel_cause_e_type;

/* enum type for loopback test mode */
typedef enum
{
  TDSLB_TEST_MODE_NONE,
  TDSLB_TEST_MODE_1,
  TDSLB_TEST_MODE_2
} tdsrrc_lb_test_mode_e_type;

/* enum type for re-establishing radio bearers */
typedef enum
{
  TDSRRC_RE_EST_RB_2,
  TDSRRC_RE_EST_NON_RB_2_UM_AM,
  TDSRRC_RE_EST_RB_INVALID
}tdsrrc_rlc_re_est_for_srns_rb_e_type;

/* enum type for ciphering config */
typedef enum
{
  CIPH_CONFIG_TDS,
  CIPH_CONFIG_G2TD,
  CIPH_CONFIG_L2TD,
  TDSCIPH_CONFIG_CS,
  TDSCIPH_CONFIG_CS_PLUS_PS,
/*Used to Update Up link keys to L1*/
  TDSCIPH_CONFIG_UL_KEYS,
/*Used to Update Down link keys to L1 */
  TDSCIPH_CONFIG_DL_KEYS,
  TDSCIPH_CONFIG_MAX
} tdsrrc_ciph_config_e_type;

typedef enum
{
  /* Revert to old set of ciphering keys */
  TDSRRC_ABORT_CIPH_ACTION_L1KEY_ONLY = 0x1,
  /* ABORT Cipher config sent to RLC */
  TDSRRC_ABORT_CIPH_ACTION_RLC_ONLY = 0x2,
  TDSRRC_ABORT_CIPH_ACTION_L1KEY_RLC_RESUME = 0x3
} tdsrrc_abort_ciph_action_e_type;


#ifdef FEATURE_TDSCDMA_REL7_SPECRS 

#define TDSHSDPA_CAT_INCL 0x01
#define TDSHSUPA_CAT_INCL 0x02

/*Following structure declares the UE capabilities that will
    be maintained throughout the RRC Connection*/
typedef struct
{
  uint8 is_valid_mask;
  uint8 hsdpa_category;
  uint8 hsupa_category;	
} tdsrrc_ue_cap_update_during_rrc_conn_type;

/*Following enum is used by COUEC procedure to notify the status of UE 
    capability change request to requesting procedure */
typedef enum
{
  TDSRRC_UE_CAP_CHANGE_ACCEPTED,
  TDSRRC_UE_CAP_CHANGE_REFUSED,
  TDSRRC_UE_CAP_CHANGE_UNSUPPORTED,
  TDSRRC_UE_CAP_CHANGE_NONE
}tdsrrc_couec_cnf_e_type;



/*Structure for invoking TDSRRC_COUEC_REQ*/
typedef struct 
{
  tdsrrc_proc_e_type proc_id;
  tdsrrc_ue_cap_update_during_rrc_conn_type ue_cap_requested;  	
}tdsrrc_couec_req_type;

/*Structure for the response TDSRRC_COUEC_CNF*/
typedef struct
{
  tdsrrc_proc_e_type proc_id;
  tdsrrc_couec_cnf_e_type ueci_cnf;
}tdsrrc_couec_cnf_type;
#endif


/*Following enum is used by LLC procedure to notify the status of 
CIPHER_CONFIG_REQ sent by SMC */
typedef enum
{
  TDSRRC_CIPHER_CONFIG_SUCCESS,
  TDSRRC_CIPHER_CONFIG_FAIL_DIDNT_CONFIG_LL
}tdsrrc_cipher_config_cnf_e_type;

/*Structure for the response RRC_CIPHER_CONFIG_CNF*/
typedef struct
{
  tdsrrc_cipher_config_cnf_e_type cause;
}tdsrrc_cipher_config_cnf_type;


/*---------------------------------------------------------------------------
  Type for storing event data for RRC_DOWNLINK_SDU_IND_EVENT.
---------------------------------------------------------------------------*/
typedef struct 
{
                  /* Unique identifier for the 
                                              Downlink RLC logical channel.*/
  uint32 dl_sdu_length;                    /* Length of downlink sdu 
                                              in bytes */
  uint32 dl_sdu_type;                      /* SDU type received over the air.
                                              This is same as PDU num in 
                                              ASN1 */
  uint8 *dl_sdu;                           /* Pointer to the Downlink SDU. */

  void *decoded_msg;                       /* Pointer to the decoded 
                                              Downlink OTA message. */      
  rlc_lc_id_type lc_id;                                               
                                      
} tdsrrc_downlink_sdu_ind_type;

/*---------------------------------------------------------------------------
  Type for storing event data for RRC_DOWNLINK_L2ACK_IND_EVENT.
---------------------------------------------------------------------------*/
typedef struct 
{
  rlc_lc_id_type lc_id;                         /* Unique identifier for the 
                                                   Downlink RLC logical channel.*/  
  uint8 mui;                                    /* Message Unit Identifier. */
  uecomdef_status_e_type status;                /* Indicates success or failure of
                                                   transmission of the SDU. */                                        
  rlc_sdu_tx_fail_reason_e_type failure_reason; /* Failure reason(for failure cases) */
} tdsrrc_downlink_l2ack_ind_type;


/* This structure defines the parameters needed to TDSRRC_REDIRECT_REQ
   command */
typedef struct
{
  
  uint32 wait_time;
  
  tdsrrc_cpid_type cpid;
  tdsrrc_proc_e_type proc_id;
  tdsrrc_redirect_e_type choice;              /* Represents whether Freq or
                                              System re-direction */
  union
  { 
    tdsrrc_freq_info_type freq;               /* Re-direction details */
    tdsrr_gsm_cell_list_type GSM_cell_info_list;
    lte_rrc_tdscdma_redir_earfcn_list_s EUTRA_target_cell_info;
  }u;


}tdsrrc_redirect_req_type;

/* This structure defines the parameters needed to TDSRRC_REDIRECT_CNF
   command */

typedef struct
{
  /* Frequency redirection succeeded or failed */
  tdsrrc_redirect_status_e_type status;

  /* Frequency redirection success/failure cause */
  tdsrrc_redirect_cause_e_type  cause;
}tdsrrc_redirect_cnf_type;


/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_new_state_ind_type

This is sent to the RRC state change manager. This contains
the new RRC state and the previous RRC state. When the State
Change Manager receives this information it sends state change
notification to all the procedures that have registered to be
notified of this state transition.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_state_e_type new_state;      /* The newly changed state */
#ifdef FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE
/* This has been added , so that boothe internal commands and the 
 * RRC cmds have the same padding bits added by ARM. 
 * As the size of the padding bits is determined by the highest data type
 * present in the structure in ARM.*/
  uint64 dummy2;
#endif
}tdsrrc_new_state_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_state_change_ind_type

This is the structure for the command sent by the RRC state
change manager to other RRC procedures. This is sent to notify
procedures of a particular state change. Only procedures that
register for this type of state change will receive the notice.
The Dispatcher will look at the procedure name in this structure
and call the appropriate procedure to process this command.

Due to the high number of procedures that need to get state
change indication, one command is used to notify multiple 
procedures. The Dispatcher will send the same command to multiple
procedures if needed.
--------------------------------------------------------------------*/
typedef struct
{
  uint32 num_procedures; /* Number of procedures that should get
                         the command. */
  tdsrrc_state_e_type previous_state; /* RRC's previous state    */ 
  tdsrrc_state_e_type new_state;      /* The newly changed state */
  tdsrrc_proc_e_type  procedure[TDSRRC_TOT_SCN]; 
                         /* Name of each procedure            */
 
} tdsrrc_state_change_ind_type;


/* Enumerated types for Cell Camped on Status */
typedef enum
{
  TDSRRC_CAMPED_ON,
  TDSRRC_NOT_CAMPED_ON
}tdsrrc_camped_on_status_e_type;


/* Enumerated types for New Cell status */
typedef enum
{
  /* A new cell has been selected */
  TDSRRC_NEW_CELL_SELECTED,
  /* The cell has been lost and NO new cell selected */
  TDSRRC_CELL_LOSS_NO_NEW_CELL,


  TDSRRC_MAX_NEW_CELL_STATUS_TYPE
}tdsrrc_new_cell_status_e_type;

/* Enumerates the various causes for cell change */
typedef enum
{
  TDSRRC_CHANGE_CELL_SELECTION,
  TDSRRC_CHANGE_CELL_RESELECTION,
  TDSRRC_CHANGE_RETURN_TO_SERVICE_AREA,
  TDSRRC_CHANGE_CELL_DCH_TO_CELL_FACH,
  TDSRRC_CHANGE_CELL_DCH_TO_URA_PCH,
  TDSRRC_CHANGE_CELL_DCH_TO_CELL_PCH,
  TDSRRC_CHANGE_CELL_FACH_TO_CELL_FACH,
  TDSRRC_CHANGE_CELL_FACH_TO_CELL_PCH,
  TDSRRC_CHANGE_CELL_FACH_TO_URA_PCH
}tdsrrc_cell_change_e_type;

/* Enumerates Cell Change status types */
typedef enum
{
  /* Cell change was successfully accomplished */
  TDSRRC_CELL_CHANGE_SUCCESS,
  /* Cell change was successful but the freq and scr_code yielded a cell 
   * of a PLMN different than one asked 
   */
  TDSRRC_CELL_CHANGE_SUCCESS_DIFF_PLMN,
  /* Cell change was successful but the freq and scr_code yielded a cell 
   * of a Cell ID different than one asked 
   */
  TDSRRC_CELL_CHANGE_SUCCESS_DIFF_CELLID,
  /* Cell change failed due to failure to configure lower layers */
  TDSRRC_CELL_CHANGE_FAILURE_LOWER_LAYERS,
  /* Cell change failed due to Incompatible RRC state */
  TDSRRC_CELL_CHANGE_FAILURE_INCOMPATIBLE_STATE,
  /* Cell change failed, due to any other reason */
  TDSRRC_CELL_CHANGE_FAILURE_OTHER
  //More later
}tdsrrc_cell_change_status_e_type;      

/* Enumerates transition to disconnected reason which is used by CSP.
 * Other reasons should be added as and when they are needed by CSP.
 */
typedef enum
{
  /* This cause is set when an Emergency Call is attempted by the User */
  TDSRRC_TX_TO_DISCON_EM_CALL,                

  /* This cause is set when a RL failure happens the T314/T315 = 0 */
  TDSRRC_TX_TO_DISCON_RL_FAILURE_T314_T315_0, 

  /* This cause is set when hard abort flag is received in TDSRRC_ABORT_REQ from NAS */
  TDSRRC_TX_TO_DISCON_HARD_ABORT,

  /* This cause is GSM redirection in connection reject with dual mode disabled*/
  TDSRRC_TX_TO_DISCON_UNSUPPORTED_REDIRECTION,

  /* This cause is set when the cause is none of the above */
  TDSRRC_TX_TO_DISCON_OTHER,

  /* This cause is set when a RL failure happens and tx to discon other than T314/T315 = 0 */
  TDSRRC_TX_TO_DISCON_RL_FAILURE_OTHER
} tdsrrc_tx_to_discon_reason_e_type;    

/* This structure defines parameters needed for TDSRRC_CELL_CHANGE_REQ */
typedef struct
{
  /* Indicates the procedure requesting Cell Change */
  tdsrrc_proc_e_type   req_proc_id;

  /* Indicates Cell Camping status for the new cell */ 
  tdsrrc_new_cell_status_e_type    new_cell_camp_status;

  mm_as_cell_access_status_T     new_cell_access;

#ifdef FEATURE_DSAC
  /* TRUE  if DSAC is present
     FALSE if DSAC is not present */
  boolean dsac_present;

  /* store the new cs celll access status */
  mm_as_cell_access_status_T     new_cs_cell_access;

  /* store the new ps celll access status */
  mm_as_cell_access_status_T     new_ps_cell_access; 
#endif

  /* The Cell ID as received in SIB 3 */
  tdsrrc_int_cell_id_type          cell_id;

  /* The PLMN ID as received in MIB */
  rrc_plmn_identity_type      plmn_id;

  /* Scrambling Code of the Camped on Cell */
  tdsrrc_cpid_type           cpid;

  /* Frequency of the Camped on Cell */
  tdsrrc_freq_type               freq;

  /* Type of cell change - determines CCM behavior on a Cell Change */
  tdsrrc_cell_change_e_type  cell_change_type;

  boolean is_valid_nw_sharing;
  rrc_plmn_identity_type common_plmn_id;

}tdsrrc_cell_change_req_type;


/* This structure defines parameters needed for TDSRRC_CELL_CHANGE_CNF */
typedef struct
{
  /* The Cell ID as received in SIB 3 */
  tdsrrc_int_cell_id_type          cell_id;

  /* Scrambling Code of the Camped on Cell */
  tdsrrc_cpid_type         cpid;

  /* Frequency of the Camped on Cell */
  tdsrrc_freq_type             freq;

  /* The PLMN ID as received in MIB */
  rrc_plmn_identity_type        plmn_id;

  /* Indicates the procedure for whom Cell Change Confirmation 
   * is intended.
   */
  tdsrrc_proc_e_type   req_proc_id;

  /* Indicates the status of Cell Change. */
  tdsrrc_cell_change_status_e_type  cell_change_status;

  /* Indicates New Cell status  */ 
  tdsrrc_new_cell_status_e_type    new_cell_status;


}tdsrrc_cell_change_cnf_type;


/* This structure defines parameters needed for TDSRRC_NEW_CELL_IND */
typedef struct
{
  /* The Procedure ID that must not be informed of New Cell 
   * Note that this is not the only procedure not to be informed. The
   * procedures to be informed are hardcoded but if the procedure included
   * here is one of those, it should not be informed.
   */
  tdsrrc_proc_e_type   no_ind_proc_id;

  /* This flag indicates if the newly camped on cell is the same as 
   * the one camped on before
   */
  boolean   cell_same_as_before;

  /* This flag indicates if the RCE procedure needs to be sent the 
   * New Cell Indication.
   */
  boolean   inform_procedure_rce;

  /* Indicates New Cell Status */ 
  tdsrrc_new_cell_status_e_type    new_cell_status;

}tdsrrc_new_cell_ind_type;


/*-------------------------------------------------------------------
ENUMERATION: tdsrrc_rb_cfg_e_type

This enum indicates what type of action is to be performed on a per
radio bearer basis.
--------------------------------------------------------------------*/
typedef enum 
{
  TDSNO_ACTION_ON_RB,
  TDSESTABLISH_RB,
  TDSRELEASE_RB,
  TDSRECONFIG_RB,
  TDSRECONFIG_RB_OTA
} tdsrrc_rb_cfg_e_type;

#define TDSMODIFY_RB_CFG         0x0001 /*RB is modified through OTA*/
#define TDSSTOP_RB_CFG             0x0002 /*RB is Stopped*/
#define TDSCONTINUE_RB_CFG     0x0004 /*RB is continue*/
/*-------------------------------------------------------------------
ENUMERATION: tdsrrc_cmd_dest_e_type

This enum indicates which lower layer the command is destined for.
--------------------------------------------------------------------*/
typedef enum 
{
  TDSNO_LAYER,
  TDSLAYER1_PHY,
  TDSLAYER2_MAC_UL,
  TDSLAYER2_MAC_DL,
  TDSLAYER2_RLC_UL,
  TDSLAYER2_RLC_DL
} tdsrrc_cmd_dest_e_type;



/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_rb_config_type

This structure defines the parameters needed to setup/release/reconfig
radio bearers.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_RB_Identity                rb_id;
  tdsrrc_rb_cfg_e_type         rb_config;
  uint16 ul_cfg_mask; /*bit mask for UL Actions*/
  uint16 dl_cfg_mask; /*bit mask for DL Actions*/

} tdsrrc_rb_config_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_llc_cmd_req_type

This structure defines the parameters needed to configure the lower
layers including L1, MAC, and RLC. This encompasses all commands to
the lower layers and provides a single command for all lower layer
configurations.
--------------------------------------------------------------------*/
typedef union
{
  tdsl1_acq_cmd_type                 acq;
  tdsl1_freq_scan_cmd_type           freq_scan;
  tdsl1_cell_parm_cmd_type           cell_parm;
  tdsl1_immediate_cell_eval_cmd_type immed_cell_eval;
  tdsl1_meas_cmd_type                meas;
  tdsl1_sfn_cmd_type                 sfn;
  tdsl1_cfn_cmd_type                 cfn;
  tdsl1_drx_cmd_type                 drx;
  tdsl1_ciphering_key_cmd_type       cipher_key;
  tdsl1_cell_selec_cmd_type          cell_sel;
  tdsl1_act_time_cancel_cmd_type     act_cancel;
  tdsl1_cell_transition_cmd_type     cell_transition;
  tdsl1_cell_bar_cmd_type            cell_bar_req;
  tdsl1_upd_dl_sync_parms_cmd_type   connected_timer_info;
  tdsl1_suspend_cmd_type             suspend_req;
  tdsl1_resume_cmd_type              resume_req;
#if  defined(FEATURE_TDSCDMA_DRX_MANAGER)
  tdsl1_sib_sched_change_ind_type    sib_sched_change_ind_req;
#endif

  tdsl1_upd_ota_parms_ind_cmd_type upd_ota_parms;

  /* for Uplink Physical Channel Control */
  tdsl1_ul_phych_control_cmd_type   upphych;
} tdsl1_req_cmd;

typedef union
{
  tdsmac_dl_config_type             mac_config_info;
  tdsrlc_dl_tm_config_type          dl_tm_config;
  tdsrlc_dl_um_config_type          dl_um_config;
  tdsrlc_dl_cipher_config_type      dl_cipher_config;
  tdsrlc_dl_hfn_type                dl_hfn;
  tdsrlc_dl_register_srvc_type      dl_reg;
} tdsl2_dl_req_cmd;

typedef union
{
  tdsmac_ul_config_type              mac_config_info;
  tdsrlc_ul_tm_config_type           ul_tm_config;
  tdsrlc_ul_um_config_type           ul_um_config;
  tdsrlc_am_config_type              am_config;
  tdsrlc_ul_cipher_config_type       ul_cipher_config;
  tdsrlc_ul_resume_type              ul_resume;
  tdsrlc_ul_hfn_type                 ul_hfn;
  tdsrlc_ul_register_srvc_type       ul_reg;
  tdsrlc_ul_register_for_data_ind_type ul_reg_data_ind;
  tdsrlc_ul_unregister_for_data_ind_type ul_unreg_data_ind;
  tdsmac_ul_traffic_meas_config_type* mac_ul_traffic_vol_config;
  boolean                          block_rach;
  tdsrlc_ul_purge_wm_ind_type ul_purge_wm_ind;
#ifdef FEATURE_TDSCDMA_TVM_BLOCK
  uint16                             block_time_interval; 
#endif
} tdsl2_ul_req_cmd;

typedef struct
{
  tdsrrc_proc_e_type       procedure;
  tdsrrc_cmd_dest_e_type   cmd_dest;
  tdsl1_cmd_hdr_type        l1_cmd;
  tdsl2_dl_cmd_hdr_type     l2_dl_cmd;
  tdsl2_ul_cmd_hdr_type     l2_ul_cmd;
#ifdef FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE
  tdsl1_req_cmd           * l1_req;
  tdsl2_dl_req_cmd        * l2_dl_req;
  tdsl2_ul_req_cmd        * l2_ul_req;
#else/*FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE*/
  tdsl1_req_cmd            l1_req;
  tdsl2_dl_req_cmd         l2_dl_req;
  tdsl2_ul_req_cmd         l2_ul_req;
#endif/*FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE*/
} tdsrrc_llc_cmd_req_type;

/*-------------------------------------------------------------------
ENUM: tdsrrc_ll_cnf_e_type

This enum defines the internal RRC confirmation types that the Lower
Layer Controller (LLC) uses to inform other procdures of command 
confirmations or event indications from L1, MAC and RLC.
--------------------------------------------------------------------*/
typedef enum
{
  /* Layer 1 cnfs and inds */
  TDSRRC_LL_CPHY_ACQ_CNF,
  TDSRRC_LL_CPHY_ERROR_IND,
  TDSRRC_LL_CPHY_CELL_SELECTION_CNF,
  TDSRRC_LL_CPHY_CELL_RESELECTION_IND,
  TDSRRC_LL_CPHY_MEASUREMENT_IND,
  TDSRRC_LL_CPHY_PHYCHAN_ESTABLISHED_IND,
  TDSRRC_LL_CPHY_CELL_TRANSITION_CNF,
  TDSRRC_LL_CPHY_FREQ_SCAN_CNF,
  TDSRRC_LL_CPHY_MEAS_CTRL_CNF,

  TDSRRC_LL_CPHY_SUSPEND_CNF,
  TDSRRC_LL_CPHY_RESUME_CNF,

   TDSRRC_LL_CPHY_ACT_CANC_CNF,

  /* MAC cnfs */
  TDSRRC_LL_CMAC_UL_TRAFFIC_IND,
  TDSRRC_LL_CMAC_STATUS_IND,

  /* RLC cnfs */
  TDSRRC_LL_CRLC_SUSPEND_CNF,
  TDSRRC_LL_CRLC_STATUS_IND

}tdsrrc_ll_cnf_e_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_llc_cmd_cnf_type

This structure defines the parameters needed to configure the lower
layers including L1, MAC, and RLC. This encompasses all commands to
the lower layers and provides a single command for all lower layer
configurations.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type       procedure;
  tdsrrc_ll_cnf_e_type     ll_cnf;

  union
  {
    tdsl1_acq_cnf_type                 acq_cnf;
    tdsl1_freq_scan_cnf_type           freq_scan_cnf;
    tdsl1_cell_select_cnf_type         cell_sel_cnf;
    tdsl1_error_ind_type               err_ind;
    tdsl1_cell_reselec_ind_type        cell_rsel_ind;
    tdsl1_measurement_ind_type         meas_ind;
    tdsl1_phychan_est_ind_type         phy_est_ind;
    tdsl1_cell_transition_cnf_type     cell_tran_cnf;
    tdsl1_meas_ctrl_cnf_type           l1_meas_ctrl_cnf;  
    tdsl1_suspend_cnf_type             suspend_cnf;
    tdsl1_resume_cnf_type              resume_cnf;
    tdsl1_act_time_cancel_cnf_type     act_cancel_cnf;          
  } cnf;

} tdsrrc_llc_cmd_cnf_type;

/*-------------------------------------------------------------------
ENUM: tdsrrcllc_chan_cfg_reason_e_type
This enum defines the reasons of the Channel configuration
--------------------------------------------------------------------*/
typedef enum
{
  TDSRRCLLC_CHAN_CFG_REASON_NONE,             /* Channels config reason none */
  TDSRRCLLC_CHAN_CFG_REASON_RL_FAILURE,     /* Channel config reason RL failure */
  TDSRRCLLC_CHAN_CFG_REASON_RLC_CONFIG_ONLY,     /* Channel config reason RLC config only */
  TDSRRCLLC_CHAN_CFG_REASON_PCH_TO_FACH,     /* Channel config for pch2fach transition */
  TDSRRCLLC_CHAN_CFG_REASON_OUT_OF_SERVICE, /* Channel config for out of service service */
  TDSRRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE, /* Channel config for re-entering service */
  TDSRRCLLC_CHAN_CFG_REASON_PHYCHAN_RESETUP, /* Channel config for appropriate physical channels
                                           * to be resetup after WTOG failure
                                           */
  TDSRRCLLC_CHAN_CFG_REASON_RECONFIG_PCH_FACH,
  TDSRRCLLC_CHAN_CFG_REASON_SIB_READ_IN_DCH, /* Channel config for setting up S-BCCH in CELL DCH */

  TDSRRCLLC_CHAN_CFG_REASON_WITHIN_FACH,
  TDSRRCLLC_CHAN_CFG_REASON_FACH_TO_PCH,
  TDSRRCLLC_CHAN_CFG_REASON_BPLMN_CTCH_CONFIG

  /*add the new reasons below */
} tdsrrcllc_chan_cfg_reason_e_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_channel_config_req_type

This structure defines the parameters needed to setup Idle mode
an connected mode channels.
--------------------------------------------------------------------*/
typedef struct
{
    /* Useful for associating a channel config confirm with a channel
    config request */
    uint32                transaction_id;      

  /* The number of RBs to configure */
  int                   num_rb_to_config;

  /* Only 2 procedures can setup up idle mode channels */
  tdsrrc_proc_e_type       procedure;

             

  /* Indicates if a state changed is required or not */
  boolean               rrc_state_change_required;

 /* Indicates if a confirmation is required or not */
  boolean               rrc_channel_config_cnf_required;

  /* The next state */
  tdsrrc_state_e_type      next_state;



  /* Loopback Test Mode */
  tdsrrc_lb_test_mode_e_type lb_test_mode;

  /* Loopback Mode Signaling Test Mode */
  boolean               dummy_signaling_enabled;

  /* Indicate if all Physical and Transport channels need to be
   * released, leaving the logical channels intact.
   */
  boolean               release_phychls;
  boolean               release_trchls;
  /*reason for channel configuration. It is useful for some conditional
  operations */
  tdsrrcllc_chan_cfg_reason_e_type       chan_config_reason;

  
  /* The maximum number of idle mode RBs to configure is 4 */
  /* The maximum number of signalling RBs to configure is 4 */
  tdsrrc_rb_config_type    rb[TDS_MAX_RB];
} tdsrrc_channel_config_req_type;



/*-------------------------------------------------------------------
ENUM: tdsrrcllc_chan_cfg_status_e_type
This enum defines the status of the Channel configuration
--------------------------------------------------------------------*/
typedef enum
{
  TDSRRCLLC_CHAN_CFG_SUCCESS,             /* Channels configured successfully */
  TDSRRCLLC_CHAN_CFG_FAILURE_PHY_CHL,     /* Channel configuration failed due to 
                                          physical channel failue */
  TDSRRCLLC_CHAN_CFG_FAILURE_LOG_CHL,     /* Channel configuration failed due to
                                          logical channel configuration failure */
  TDSRRCLLC_CHAN_CFG_FAILURE_INCOMPATIBLE_STATE,  /* Channel configuration failed due to
                                          incompatible RRC state */
  TDSRRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG,  /* Channel configuration failed due to*/
  TDSRRCLLC_CHAN_CFG_FAILURE_CFG_CANCELLED, 
  TDSRRCLLC_CHAN_CFG_FAILURE_OTHER        /* Any other LLC internal reason for failure */
} tdsrrcllc_chan_cfg_status_e_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_channel_config_cnf_type

This structure indicates whether the channel config request was 
successful or not. Only the first radio bearer id in the list is
returned if more than one radio bearer was configured.
--------------------------------------------------------------------*/
typedef struct
{
    
  /* Useful for associating a channel config confirm with a channel
    config request. This will currently be used by the SIB procedure. 
    */
  uint32                        transaction_id;  

  tdsrrc_proc_e_type               procedure;
  

  tdsrrcllc_chan_cfg_status_e_type chan_cfg_status;

  boolean inter_freq_hho_status;
} tdsrrc_channel_config_cnf_type;

/*-------------------------------------------------------------------
ENUM: tdsrrcsib_update_sibs_cause_e_type
This enum defines the status of the Channel configuration
--------------------------------------------------------------------*/
typedef enum
{
  TDSRRCSIB_DEFERRED_SIB_READ,             /* Channels configured successfully */
  TDSRRCSIB_SFN_UPDATE,
  TDSRRCSIB_REAL_SIB_MOD,
  TDSRRCSIB_SIX_HR_TMR_EXPIRY,
  TDSRRCSIB_CSFB_CALL_CANCL,
  TDSRRCSIB_OTHER
} tdsrrcsib_update_sibs_cause_e_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_update_sibs_ind_type

This structure indicates defines the parameters for TDSRRC_UPDATE_SIBS_IND
command. 
--------------------------------------------------------------------*/
typedef struct
{
  
  uint32                  mib_value_tag;
  tdsrrc_proc_e_type         procedure;
  tdsrrcsib_update_sibs_cause_e_type cause;

} tdsrrc_update_sibs_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_initiate_ue_cap_req_type

This structure indicates defines the parameters for 
TDSRRC_INITIATE_UE_CAPABILITY_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_ue_capability_originator_e_type originator; /* Originator of UE 
                                                     Update Capability
                                                     procedure */
  uint8 transaction_id;                          /* Transacation Id
                                                     received through
                                                     UE Capability 
                                                     Enquiry.*/
} tdsrrc_initiate_ue_cap_req_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_initiate_cu_req_type

This structure indicates defines the parameters for 
TDSRRC_CELL_UPDATE_INITIATE_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_cell_update_cause_e_type cause;  /* Cause for the Cell Update to
                                          perform */
  boolean cu_complete_cnf_required;    /* This indicates whether 
                                          Cell Update initiated proc
                                          needs completed confirmation
                                          or not.*/
  tdsrrc_proc_e_type         procedure;   /* RRC procedure that initiates Cell
                                          Update*/
} tdsrrc_initiate_cu_req_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_initiate_uu_req_type

This structure indicates defines the parameters for 
TDSRRC_URA_UPDATE_INITIATE_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_ura_update_cause_e_type cause;  /* Cause for the URA Update to
                                          perform */
  boolean uu_complete_cnf_required;    /* This indicates whether 
                                          URA Update initiated proc
                                          needs completed confirmation
                                          or not.*/
  tdsrrc_proc_e_type         procedure;   /* RRC procedure that initiates URA
                                          Update*/
} tdsrrc_initiate_uu_req_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_cu_complete_cnf_type

This structure indicates defines the parameters for 
TDSRRC_CELL_UPDATE_COMPLETE_CNF command. 

--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type         procedure;   /* RRC procedure waiting for 
                                          Cell Update to complete */
} tdsrrc_cu_complete_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_rnti_update_req

This structure indicates defines the parameters for 
TDSRRC_RNTI_UPDATE_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{  
  tdsrrc_proc_e_type  procedure;               /* The procedure ID of the 
                                                sender */
  boolean cnf_required;                     /* Indicates the procedure
                                             needs confirmation */
  tdsrrc_rnti_cmd_status_e_type crnti_action;  /* Action for C-RNTI*/
  tdsrrc_rnti_cmd_status_e_type urnti_action;  /* Action for U-RNTI*/

  tdsrrc_int_c_rnti_type  crnti;               /* New C-RNTI value */


  tdsrrc_int_c_rnti_type  urnti;               /* New C-RNTI value */

  boolean rlc_re_establish_srb;             /* Indicates if RLC entities for
                                             * RB2, RB3 and RB4 need to be 
                                             * re-established.
                                             */

  boolean rlc_re_establish_rab;             /* Indicates if all User plane 
                                             * RBs need to be re-established.
                                             */
} tdsrrc_rnti_update_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_rnti_update_cnf

This structure indicates defines the parameters for 
TDSRRC_RNTI_UPDATE_CNF command. 

--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type  procedure;          /* RRC procedure waiting for 
                                          LLC to configure MAC */
  uecomdef_status_e_type status;       /* Indicates status  of RNTI 
                                          configuration */

} tdsrrc_rnti_update_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_cell_reselection_req

This structure indicates defines the parameters for 
TDSRRC_CELL_RESELECTION_REQ command. 

--------------------------------------------------------------------*/
typedef struct
{
   /* Cell ID */
  tdsrrc_int_cell_id_type        cell_id;
   /* Freq. of the cell to be reselected */
   tdsrrc_freq_type  freq;
   
   /* Scrambling Code of the Cell */
   tdsrrc_cpid_type   cpid;

   
  tdsrrc_proc_e_type  procedure;          /* RRC procedure waiting for 
                                        LLC to configure lower layers */

  /* The PLMN ID  */
  rrc_plmn_identity_type      plmn_id;

 

} tdsrrc_cell_reselection_req_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_cell_reselection_cnf

This structure indicates defines the parameters for 
TDSRRC_CELL_RESELECTION_CNF command. 

--------------------------------------------------------------------*/
typedef struct
{
  uecomdef_status_e_type status;       /* Indicates status  of cell reselection 
                                          configuration */

} tdsrrc_cell_reselection_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_disable_prev_freq_type

This structure indicates defines the parameters for 
TDSRRC_DISABLE_PREV_FREQ_IND command. 

--------------------------------------------------------------------*/
typedef struct
{
  uint32 wait_time;                               /* Duration for which
                                                     previous freq. 
                                                     needs to be excluded.*/
} tdsrrc_disable_prev_freq_type;


/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_cell_selected_meas_req_type

This structure indicates the parameters for 
TDSRRC_CELL_SELECTED_MEAS_REQ command. 

---------------------------------------------------------------------*/
typedef struct
{
  boolean gtow_ho_active;
    /* No parameter */
} tdsrrc_cell_selected_meas_req_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_utran_mob_info_cnf_type

This structure indicates the parameters for
RRC_UTRAN_MOBILITY_INFO_CNF command.

---------------------------------------------------------------------*/
typedef struct
{
  uint8   transaction_id;
} tdsrrc_utran_mob_info_cnf_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_initiate_cell_sel_ind_type

This structure indicates the parameters for 
TDSRRC_INITIATE_CELL_SELECTION_IND command. 

---------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_cell_sel_cause_e_type  cause;     /* Cell Selection Trigger
                                                   cause */
  tdsrrc_proc_e_type            proc;      /* Procedure */
  tdsrrc_state_e_type dest_state;          /* When Cell selection
                                           successfully camped, it needs 
                                           destination state and then
                                           send TDSRRC_NEW_CELL_IND */
  boolean        dest_freq_present;    /* Indicates if Destination frequency is present */
  tdsrrc_freq_type  dest_freq;            /* Destination Frequency - is 
                                        * relevant only in DCH->FACH transitions
                                        */
  tdsrrc_cpid_type  dest_cpid;            /* Destination Pri Scrambling Code - is 
                                                                                     * relevant only in DCH->FACH transitions
                                                                                     */

  boolean            dest_cpid_present;    /* Indicates if Destination CPID is present */

  tdsrrc_tx_to_discon_reason_e_type tx_to_discon_reason; /* Enumerates transition to disconnected reason 
                                                       * which is used by CSP */

} tdsrrc_initiate_cell_sel_ind_type;


/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_sib_change_ind_type

This is the structure for the command sent by the SIB
procedure to other RRC procedures. This is sent to notify
procedures of a particular SIB change. Only procedures that
register for this type of sib change will receive the notice.
The Dispatcher will look at the procedure name in this structure
and call the appropriate procedure to process this command.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_sib_change_mask_type sib_change_mask;   /* Bit mask of all the sibs which have changed */ 

  tdsrrc_proc_e_type  procedure;  /* The procedure that should receive the command */
} tdsrrc_sib_change_ind_type;


/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_mode_change_ind_type

This is the structure for the command sent by the MCM module
to other RRC procedures. This is sent to notify
procedures of a particular RRC mode change. Only procedures that
register for this change will receive the notice.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type  procedure;  /* The procedure that should receive the command */
  tdsrrc_mode_e_type  from_mode;  /* The mode from which transition occurs */
  tdsrrc_mode_e_type  to_mode;    /* The mode to which transition occurs */
} tdsrrc_mode_change_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_stop_lower_layer_req_type

This is the structure for the command sent by the MCM module
to LLC to request LLC to stop TDS mode in the lower layers. 
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type  procedure;  /* The procedure that should receive the command */
  tdsl1_stop_cause_enum_type cause; /* The reason for stopping TDS mode */
} tdsrrc_stop_lower_layer_req_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_stop_lower_layer_cnf_type

This is the structure for the command sent by the LLC module
to MCM to confirm the completion of lower layer TDS deactivation.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type  procedure;  /* The procedure that should receive the command */
} tdsrrc_stop_lower_layer_cnf_type;


/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_sib7_update_status_ind_type

This is the structure for the command sent by the SIB
procedure to other RRC procedures to notify
procedures of SIB7 update status.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type  procedure;  /* The procedure that should receive the command */
  uecomdef_status_e_type status; /*Indicates success or failure of updating sib7*/
} tdsrrc_sib7_update_status_ind_type;

/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on a single 
  Unacknowledged or Acknowledged Mode logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* lc_id */
  uint8  n;                             /* SN 'n' until which RLC does not suspend */
} tdsrrc_ul_suspend_info_type;

/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on one or more Unacknowledged  
  or Acknowledged Mode logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of UM or AM channels to suspend */
  tdsrrc_ul_suspend_info_type   suspend[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Suspend Information for each    */
                                        /* uplink logical channel          */
} tdsrrc_ul_suspend_req_type;

/*---------------------------------------------------------------------------
  Type to resume uplink data transmission on one or more 
  unacknowledged or acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UM/AM ch. to resume 
                       transmission on                   */
  uint8  channel[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to resume */
} tdsrrc_ul_resume_req_type;


/* Enum to identify the reason for change mode request */
typedef enum {
  TDSINTERRAT_REASON_OTHER,
  TDSINTERRAT_RESELECTION,
  TDSINTERRAT_REDIRECTION,
  TDSINTERRAT_OOS,
  TDSINTERRAT_BPLMN,
  TDSINTERRAT_IHO,
  TDSINTERRAT_CCO,
  TDSINTERRAT_DSIM_TUNEAWAY,
  TDSINTERRAT_REASON_NONE
}tdsrrc_change_mode_reason_e_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_change_mode_req_type

This is the structure for the command sent by the IHO or CSP to request
MCM to change RRC mode to the requested mode.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type  procedure; /* The procedure that sends the command */
  tdsrrc_mode_e_type  mode; /* the mode the sender is requested to change to */
  tdsrrc_change_mode_reason_e_type reason; /* reason for the change */
} tdsrrc_change_mode_req_type;

/*-------------------------------------------------------------------
STRUCTURE: tdsrrc_change_mode_cnf_type

This is the structure for the command sent by the MCM to respond to
TDSRRC_CHANGE_MODE_CNF after it completes the mode change.
--------------------------------------------------------------------*/
typedef struct
{
  tdsrrc_proc_e_type  procedure; /* the destination procedure */
  tdsrrc_mode_e_type  mode;      /* the current RRC mode */
  boolean          status;    /* status flag to indicate success/failure */ 
#ifdef FEATURE_TDSCDMA_DSDS
  tdsl1_common_fail_enum_type fail_type;
#endif
} tdsrrc_change_mode_cnf_type;

#if defined(FEATURE_INTERRAT_HANDOVER_G2TD) || defined(FEATURE_TDSCDMA_L2TD_PS_HANDOVER)
typedef struct
{
  uecomdef_status_e_type acq_status;
} tdsrrc_csp_acq_cnf_type;

typedef struct
{
  uint16  freq;                   /* frequency id*/
  uint8   cpid;               /* cell param id */
  int16   rscp;               /* HO target cell rscp, used by L1 to skip step 0 and do reAcq */

#if 0  //FDD only, not needed
  boolean pn_offset_present;      /* TRUE means "pn_offset" value is present.  
                                     FALSE otherwise. */
  uint32  pn_pos;                 /* If present, PN offset of the cell in 
                                     chipx1 units. */
  boolean sttd_indicator_present; /* TRUE means "sttd_indicator" value is 
                                     present.  FALSE otherwise. */
  boolean sttd_indicator;         /* If present, TRUE means the cell is using Space 
                                     Time Transmit Diversity. ** FALSE means it is not.  
                                     Described in TS 25.331, 10.3.6.78. */
#endif

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  uint16            ts0_position; /* ts0 start position from step0 result */
  uint8             lna_state; /* current LNA state */
  uint8             num_of_jds_cells; /* number of JDS cells for interference cancellation. Zero indicates invalid timing */
  uint8             cell_list[TDSIRAT_MAX_TDS_CELLS_PER_FREQ]; /* cell parameter id */

} tdsrrc_csp_acq_req_type;

typedef struct
{
  uecomdef_status_e_type status;  /*status to indicate if ciph config
                                    was successful or not            */
}tdsrrc_ho_ciph_cnf_type;
#endif
/*-------------------------------------------------------------------
STRUCTURE: rrc_delete_meas_req_type

This is the structure for the command sent by the physical channel
reconfiguration procedure to the measurement procedure.
--------------------------------------------------------------------*/

typedef struct
{
  uint16 meas_id; /* Measurement to be deleted */
} tdsrrc_delete_meas_req_type;

/* This is the structure used to send the lower_layer_deact_req to LLC */
typedef struct
{
  sys_stop_mode_reason_e_type deact_reason;
} tdsrrc_lower_layer_deact_req_type;

/* This structure defines the parameters needed to TDSRRC_SIG_CONN_RELEASE_REQ
   command */
typedef struct
{
  rrc_cn_domain_identity_e_type cn_domain_id;   

  /* This indicates whether rrc proc needs confirmation or not.*/
  boolean sig_rel_complete_cnf_required; 

  /* proc_id which is sending sig conn release to SCRR 
   * The same proc_id is used for sending response back to proc */
  tdsrrc_proc_e_type rrc_proc;
}tdsrrc_sig_conn_release_req_type;

/* This structure defines the parameters needed to TDSRRC_SIG_CONN_RELEASE_CNF
   command */
typedef struct
{
  /* Indicated success or failure of sig. connection release */
  uecomdef_status_e_type status;       
  
  /* stores proc_id to which sig conn release cnf needs to be sent */
  tdsrrc_proc_e_type rrc_proc;
}tdsrrc_sig_conn_release_cnf_type;

typedef struct
{
  tdsrrc_rlc_re_est_for_srns_rb_e_type rb_type;       /* rb type to re_est rb2 or others     */
  uint32                            hfn_srb;       /* hfn to re-est rb2                   */
  uint32                            hfn_urb;       /* hfn to re-est rb2                   */  
  uecomdef_umts_cipher_algo_e_type            ciph_algo_srb; /* ciphering algo after re-est for srb */
  uecomdef_umts_cipher_algo_e_type            ciph_algo_urb; /* ciphering algo after re-est for urb */
}tdsrrc_re_est_req_type;

typedef struct
{
  tdsrrc_proc_e_type                   rrc_proc;  /* originating procedure id       */
  tdsrrc_rlc_re_est_for_srns_rb_e_type rb_type;   /* rb type to re_est rb2 or others*/
  boolean                           cnf_reqd;  /* to indicate if cnf is reqd     */
}tdsrrc_re_est_rlc_srns_req_type;

typedef struct
{
  tdsrrc_proc_e_type        rrc_proc;     /* Procedure name to route the cnf       */
  uecomdef_status_e_type status;       /* Indicated success or failure of 
                                          of re-establishment                   */
}tdsrrc_re_est_rlc_srns_cnf_type;
  
typedef struct
{
  tdsrrc_re_est_req_type             am_config;             /* am configuration    */
  tdsrrc_re_est_req_type             ul_um_config;          /* ul-um configuration */
  tdsrrc_re_est_req_type             dl_um_config;          /* dl-um configuration */
}tdsrrc_llc_re_est_req_type;

/* This command indicates the ciphering cause to LLC, whether the cause
   to activate is due to ciphering starting or due to handover        */
typedef struct
{
  tdsrrc_ciph_config_e_type config_cause;
}tdsrrc_ciph_config_req_type;

typedef struct
{
  tdsrrc_abort_ciph_action_e_type abort_action;
} tdsrrc_abort_ciph_config_req_type;


typedef struct
{
  byte  ciphering_key[RRC_MAX_CIPHERING_KEY_LENGTH];
  byte  integrity_key[RRC_MAX_INTEGRITY_KEY_LENGTH];
}tdsrrciho_security_keys_type;

/* This command used by HO_CIPH_REQ to indicate ciphering parameters to be
   passed to SMC procedure from IHO procedure                             */
typedef struct
{
  uecomdef_umts_cipher_algo_e_type ciph_algo;
  rrc_cn_domain_identity_e_type  domain_id;
  tdsrrc_ciph_config_e_type   config_cause;   
  tdsrrciho_security_keys_type  security_keys[RRC_MAX_CN_DOMAINS];
}tdsrrc_ho_ciph_req_type;

typedef struct
{
   uecomdef_status_e_type        camping_status;


   tdsrrc_interrat_reject_reason_T  failure_cause;



   /* Time ( in Sec ) when the same cell should be considered */
   uint32                        time_to_reselect;

   /* Thresholds to be applied to Ec/No measurements */
   int16                         squalmin;

   int16                         srxlevmin;

   /* PLMN Id of the TDSCDMA cell */
   PLMN_id_T                     PLMN;

   /* LAC */
   uint8                         forbidden_lac[2];

} tdsrrc_csp_camp_cnf_type;

typedef struct
{
  uint16                 freq;            /* Frequency */
  uint8                 cpid;        /* Scrambling code */
  rrc_plmn_identity_type requested_plmn;  /* PLMN Id */
  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

} tdsrrc_csp_camp_req_type;

typedef struct
{
  uecomdef_status_e_type status; /* Indicates if Connection was setup successfully or not */
  tdsrrc_conn_setup_failure_e_type conn_failure_reason; /* Indicates why the connection failed */
} tdsrrc_conn_setup_ind_type;


typedef struct
{
  tdsrrc_proc_e_type procedure; /* Interrat procedure that is currently active */
} tdsrrc_irat_abort_ind_type;

typedef struct
{

  tdsrrc_proc_e_type procedure; 
  rrc_cn_domain_identity_e_type domain_id;   
}tdsrrc_idt_complete_cnf;

typedef enum
{
  TDSBPLMN_SUSPEND_REASON_NONE,
  TDSBPLMN_SUSPEND_REASON_BCCH_MODIFICATION,
  TDSBPLMN_SUSPEND_REASON_CELL_RESELECTION,
  TDSBPLMN_SUSPEND_REASON_UPLINK_DATA,
  TDSBPLMN_SUSPEND_REASON_REGISTRATION,
  TDSBPLMN_SUSPEND_REASON_OTHER
} tdsrrc_bplmn_suspend_cause_e_type;


typedef struct
{
  tdsrrc_proc_e_type procedure; 
  tdsrrc_bplmn_suspend_cause_e_type cause;
} tdsrrc_csp_bplmn_suspend_req_type;

typedef struct
{
  tdsrrc_proc_e_type        procedure;
  uecomdef_status_e_type status;
} tdsrrc_csp_bplmn_suspend_cnf_type;

typedef struct
{ 
  tdsrrc_proc_e_type procedure;
   /* No fields */
} tdsrrc_mode_status_ind_type;

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
typedef struct
{
  /* MAX number of cells per freq to return back to GSTK */
  uint8 num_cells;
  /* Report type - Intra, Inter or IRAT */
  gstk_nmr_utran_qualifier_enum_type report_type;
  /*only used for GSTK purpose */
  uint32 *ref_id;
}tdsrrc_gstk_utran_nmr_info_req_type;

#endif /*FEATURE_TDSCDMA_PLI_NMR_UTRAN*/

#ifdef FEATURE_TDSCDMA_DSDS
typedef struct
{
  /* This is set to TRUE if RRC is waiting for L1 to release lock*/
  boolean reset_l1_cnf_needed;
}tdsrrc_reset_l1_req_type;
#endif
typedef enum
{
  TDSRRC_CS_DOMAIN,
  TDSRRC_PS_DOMAIN,
  TDSRRC_CS_PLUS_PS_DOMAIN
}tdsrrc_domain_type;

typedef struct
{
  tdsrrc_domain_type    domain;
}tdsrrc_open_domain_ind_type;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
typedef struct
{
  mcfg_refresh_info_s_type mcfg_refresh_info;
} tds_rrc_mcfg_refresh_ind_type;
#endif

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */




/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
/*===========================================================================

FUNCTION   RRC_GSTK_GET_UTRAN_NMR_INFO

DESCRIPTION

 This function is invoked by GSTK in it's context when it needs some UTRAN 
 network measurment information.  RRC will post an internal command and 
 process the request.

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/
void tdsrrc_gstk_get_utran_nmr_req
(
  uint8 num_cells, gstk_nmr_utran_qualifier_enum_type report_type, uint32 *ref_id
#ifdef FEATURE_TDSCDMA_DSDS
  ,sys_modem_as_id_e_type as_id
#endif
);

#endif /*FEATURE_TDSCDMA_PLI_NMR_UTRAN*/

#endif /* TDSRRCINTIF_H */

