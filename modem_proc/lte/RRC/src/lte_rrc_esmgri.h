/*!
  @file
  lte_rrc_esmgri.h

  @brief
  Header file internal to RRC eMBMS service Manager

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the Config module and its UTF test cases.

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_esmgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/11   da      initial version
===========================================================================*/

#ifndef LTE_RRC_ESMGR_IH
#define LTE_RRC_ESMGR_IH

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_rrc_utils.h>

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/* Unused debug trace id value */
#define LTE_RRC_ESMGR_UNUSED_DEBUG_TRACE_ID -1

/* Act TMGI timer value in ms */
#define LTE_RRC_ESMGR_ACT_TMGI_GUARD_TMR_VALUE 300000

/* MBMS interest indi timer value in ms */
#define LTE_RRC_ESMGR_MBMS_INTEREST_IND_TMR_VALUE 1000

/* Max num freq to search for a TMGI */
#define LTE_RRC_ESMGR_MAX_NUM_FREQ_TO_SRCH LTE_CPHY_MEAS_MAX_NUM_FREQ

/* Max Activate TMGI requests that can be pended */
#define LTE_RRC_ESMGR_MAX_PENDED_ACT_TMGI_REQ 6

/* Max num freq to maintain in interested freq list */
#define LTE_RRC_ESMGR_MAX_NUM_INTERESTED_FREQ 5

/* Max num freq that can be transmitted in MBMS interested Ind ULM */
#define LTE_RRC_ESMGR_MAX_NUM_FREQ_IN_MBMS_INTRST_IND 1

/* Bitmask to determine the bit position defined for eMBMS preference over unicast 
configuration in embms_priority_cfg EFS */
#define LTE_RRC_ESMGR_EMBMS_PREFERRED_OVER_UNICAST_MASK (0x2)

/* Neighbor cell config denoting MBSFN allocation in neighbor cells */
#define LTE_RRC_ESMGR_NEIGH_CELL_CFG_00 0

/* Neighbor cell config denoting no MBSFN allocation */
#define LTE_RRC_ESMGR_NEIGH_CELL_CFG_01 1

/* Neighbor cell config denoting MBSFN allocation same or subset of serving cell */
#define LTE_RRC_ESMGR_NEIGH_CELL_CFG_10 2

/* Neighbor cell config denoting MBSFN allocation cannot be determined for the neighbor cell */
#define LTE_RRC_ESMGR_NEIGH_CELL_CFG_11 3

/* Wait timer value in ms before clearing CFL */
#define LTE_RRC_ESMGR_CLEAR_CFL_WAIT_TMR_VALUE 5000

/* eMBMS coverage status value used for CMAPI when both SIB13 and SIB15 are not present */
#define LTE_RRC_ESMGR_CMAPI_DEFAULT_COV_STATUS 0

/* eMBMS coverage status value used for CMAPI when SIB13 is present */
#define LTE_RRC_ESMGR_CMAPI_COV_STATUS_SIB13 1

/* eMBMS coverage status value used for CMAPI when SIB15 alone is present */
#define LTE_RRC_ESMGR_CMAPI_COV_STATUS_SIB15 2

/* eMBMS Wait timer for SLTE to be back in coverage in msec*/
#define LTE_RRC_ESMGR_SLTE_IN_COV_TMR_VALUE 10000

/*! @brief Enum to indicate which one or more MBMS interest ind procedure is in process
*/
typedef enum
{
  LTE_RRC_ESMGR_MBMS_PROC_NONE = 0x0,                      /*!< MBMS Interest Indication VOID */
  LTE_RRC_ESMGR_MBMS_PROC_INTEREST_IND_REQD = 0x1,         /*!< MBMS Interest Indication required */
  LTE_RRC_ESMGR_MBMS_PROC_BEST_NEIGH_CNF_PEND = 0x2,       /*!< Best neigh freq CNF from ML1 required */
  LTE_RRC_ESMGR_MBMS_PROC_BEST_NEIGH_CNF_PASS = 0x4,       /*!< Best neigh freq CNF rcvd with PASS */
  LTE_RRC_ESMGR_MBMS_PROC_BEST_NEIGH_CNF_FAIL = 0x8,       /*!< Best neigh freq CNF rcvd with FAIL */
  LTE_RRC_ESMGR_MBMS_PROC_INTEREST_IND_TMR_RUNNING = 0x10, /*!< MBMS Interest Indication sent and 1s timer is running */
  LTE_RRC_ESMGR_MBMS_PROC_WAIT_FOR_SIB1_AFTER_HO = 0x20,   /*!< MBMS Interest Indication required after HO, but waiting for SIB1 */
  LTE_RRC_ESMGR_MBMS_PROC_HO_CRE_COMPLETE = 0x40,          /*!< MBMS Interest Indication required due to HO/CRE in < 1sec */
  LTE_RRC_ESMGR_MBMS_PROC_BEST_NEIGH_REQ_PENDED = 0x80,    /*!< Best neigh freq REQ pended */
  LTE_RRC_ESMGR_MBMS_PROC_INTEREST_IND_RFRSH_TMR_RUNNING = 0x100,    /*!< MBMS Interest Ind 1sec tmr over, and refresh timer is > 1 sec */
  LTE_RRC_ESMGR_MBMS_PROC_INTEREST_IND_RFRSH_REQD = 0x200,    /*!< MBMS Interest Ind refresh needed */
  LTE_RRC_ESMGR_MBMS_PROC_MAX
} lte_rrc_esmgr_mbms_interest_ind_proc_mask_e;

/*! @brief Structure for Act TMGI list info  
*/
typedef struct
{ 
  /* EMBMS session info */
  lte_rrc_embms_bearer_info_s tmgi_info;

  /* Number of SAIs in the list below */
  uint8 num_sai_to_search; 

  /* SAIs where the TMGI is available */
  uint16 sai_list[LTE_RRC_EMBMS_MAX_NUM_SAI_TO_SRCH]; 

  /* If TMGI should be found in all SAIs */
  boolean all_sai_in_asl; 

  /* Number of EARFCNs in the list below */
  uint8 num_earfcn_to_search; 

  /* List of EARFCNs where TMGI can be found */
  lte_earfcn_t earfcn_list[LTE_RRC_EMBMS_MAX_NUM_FREQ_TO_SRCH]; 

  /* If TMGI should be found in all EARFCNs */
  boolean all_earfcn_in_afl; 
                          
  /* If TMGI is waiting to be activated in lower layers */
  boolean waiting_activate;

  /* MBSFN AREA ID*/
  lte_mbsfn_area_id_t area_id;

}lte_rrc_esmgr_act_tmgi_list_s;

/*! @brief Structure for Candidate freq list, contains the list of 
  freqs that carry the SAIs in CSL or if SAIs are not used, contains
  Overlapping list of freq among all active TMGIs that are available in SIB5
  of the serving cell
*/
typedef struct
{ 
  /*! Size of Candidate freq list */
  uint8 cfl_size;

  /*! Candidate freq list */
  lte_earfcn_t candidate_freq_list[LTE_RRC_ESMGR_MAX_NUM_FREQ_TO_SRCH]; 

  /*! Cell ID where CFL was computed */
  lte_rrc_cell_id_s cfl_cell_id;

}lte_rrc_esmgr_cfl_s;

/*! @brief Structure for Interested freq list, contains the list of 
  freqs that the UE is interested in for its MBMS service
*/
typedef struct
{ 
  /*! Size of Candidate freq list */
  uint8 ifl_size;

  /*! Candidate freq list */
  lte_earfcn_t interested_freq_list[LTE_RRC_ESMGR_MAX_NUM_INTERESTED_FREQ]; 

}lte_rrc_esmgr_ifl_s;


/*! @brief Structure for private data variables that can be allocated dynamically
     from the heap when LTE is activated, and deallocated when LTE is deactivated  
*/
typedef struct
{    
  /*!< curr cmd being processed */
  msgr_umid_type curr_req_cmd; 
 /*< union of lower layer cmds */
  union 
  {
    /*!< Activate TMGI req */
    lte_rrc_embms_act_tmgi_req_s  act_tmgi_req;
    /*!< Deactivate TMGI req */
    lte_rrc_embms_deact_tmgi_req_s  deact_tmgi_req;
    /*!< Activate/Deactivate TMGI req */
    lte_rrc_embms_act_deact_tmgi_req_s  act_deact_tmgi_req;
    /*!< Activate TLB_CFG req for loopback mode C*/
    lte_tlb_rrc_cfg_req_s  tlb_rrc_cfg_req;
    
  }curr_cmd_u;

  /* If TMGI should be found in all SAIs */
  boolean curr_cmd_tmgi_in_all_sai;

  /* If TMGI should be found in all EARFCNs */
  boolean curr_cmd_tmgi_in_all_earfcn;

  /*! Saved status for activate during act_deact_tmgi_req processing */
  lte_embms_status_e saved_actdeact_act_status;

  /*! Saved status for deactivate during act_deact_tmgi_req processing */
  lte_embms_status_e saved_actdeact_deact_status;
 
  /*! Number of tmgi in tmgi_list array */
  uint8 num_act_tmgi; 

  /*! List of TMGIs */
  lte_rrc_esmgr_act_tmgi_list_s act_tmgi_list[LTE_MAX_ACTIVE_MRB]; 
  
  /*! EMP warning reason */
  lte_rrc_emp_warn_reason_e emp_warn_reason; 

  /*! Act TMGI guard timer, when the timer expires DEACT_TMGI_IND will be sent 
      for all TMGIs pending activation in lower layers */
  lte_rrc_tmr_s act_tmgi_guard_tmr; 

  /*! Flag to indicate if act_tmgi_guard_tmr is running */
  boolean act_tmgi_guard_tmr_running;

  /*! Candidate freq list */
  lte_rrc_esmgr_cfl_s cfl; 

  /*! 1s timer to track HO/RLF after transmitting interest indication */
  lte_rrc_tmr_s mbms_interest_ind_tmr; 

  /*! Bitmask to track the mbms interest ind proc */
  uint16 mbms_interest_ind_proc_mask;

  /*! Interested freq list */
  lte_rrc_esmgr_ifl_s ifl;

  /*! Flag if loopback mode is ON*/
  boolean loopback_mode;

  /*! Flag indicates if ML1 signal strength req should be sent */
  boolean send_ml1_signal_strength_req;

  /*! wait timer, to allow some time before 
  clearing the CFL due to service deactivation */
  lte_rrc_tmr_s clear_cfl_wait_tmr; 

  /*! Flag indicates whether CFL clearing is pending */
  boolean clear_cfl_pending;

  /*! Flag indicates whether sib15 was previously scheduled or not to detect 
    mobility from rel 11 network to rel 9 network*/
  boolean sib15_scheduled;

  /*! Flag indicates whether Activate rcvd is the first one after ENABLE_REQ */
  boolean first_act_after_enable;

  /*! Boolean to check if we are in coverage for SLTE in 1xSRLTE mode. 
      Even when RF is available in SRLTE mode but primary antenna can be tuned away
      we need to have this coverage as FALSE*/
  boolean cov_due_to_1xslte;

  /*! wait timer, to allow some time before 
  moving from out of coverage to in coverage in SRLTE/SLTE mode */
  lte_rrc_tmr_s slte_in_cov_wt_tmr; 

  /*! Flag indicates whether Deactivate All TMGI request is already in progress */
  boolean deact_all_in_progress;
  
} lte_rrc_esmgr_dd_s;



/*! @brief Structure for private data variables that is statically allocated 
*/
typedef struct
{
  /*! boolean to keep track of embms enable status */
  boolean embms_enable_status;

  /*! boolean to see if embms is over unicast - controlled via EFS */
  boolean embms_over_unicast;

  /*! enum to keep track of embms enable status */
  lte_rrc_embms_coverage_status_e embms_cov_status;

  /*! boolean to enable embms feature support */
  boolean embms_feature_status;

  /*! boolean to see if multicast is preferred over unicast */
  boolean embms_preferred;

  /*! boolean to see if eMBMS freqs need to be prioritized */
  boolean prefer_all_embms_freq;

  /*! Duration to wait before clearing the CFL
  Can be configured via EFS or assume the default value of 5s */
  uint32 clear_cfl_wait_tmr_value;

  /*! boolean to read EFS value to enable embms feature support */
  boolean efs_value_embms_feature_status;

  /*! Duration to wait before refreshing ML1 for best freq to be sent in MII
  Can be configured via EFS or assume the default value of 1s */
  uint32 mii_refresh_tmr_value;

  /*! boolean to keep track of emergency_mode_status*/
  boolean esmgr_emer_mode_status;

  /*! boolean to read EFS value to keep embms enabled during RF tuneaways */
  boolean efs_value_embms_during_rf_tuneaways;

  /*! Flag indicating if eMBMS is allowed on current TRM PRIORITY status*/
  boolean trm_prio_suitable_for_embms;

} lte_rrc_esmgr_sd_s;


/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /*!< Dynamically allocated part of private data */
  lte_rrc_esmgr_dd_s *dd_ptr;

  /*!< Dynamically allocated part of private data */
  lte_rrc_esmgr_sd_s *sd_ptr;

} lte_rrc_esmgr_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* LTE_RRC_ESMGR_IH */
