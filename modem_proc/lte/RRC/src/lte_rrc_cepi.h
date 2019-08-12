/*!
  @file lte_rrc_cepi.h
  

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_cepi.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/03/12   ap       Removed SIB2 wait timer related MACROs and members from db
03/27/12   kp       Added EFS file to enable trigger of OOS after 2nd T300 expiry.
03/13/12   kp        Added code to perform ac-barring for AC-11 to 15 from param provided 
                     by NAS when SIB2 says no AC barring for AC-11 to 15.  
01/11/11   mm       Added rach abort reason enum to CEP internal data
6/10/09    dd       Added resel_started flag
7/29/08    dd       Initial Version
===========================================================================*/

#ifndef LTE_RRC_CEPI_H
#define LTE_RRC_CEPI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_int_msg.h"
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_utils.h>
#include "lte_rrc_osys_asn1.h"

 
#define LTE_RRC_CEP_DEFAULT_MAX_TRIES 1

#define LTE_RRC_CEP_RANDOM_SEED 1001

#define LTE_RRC_CEP_MAX_NUM_ACCESS_PROBS 16

#define LTE_RRC_CEP_MAX_NUM_ACCESS_BARRING_TIME 8

#define LTE_RRC_CEP_MAX_NUM_T300_TIME 8

#define LTE_RRC_CEP_T300_VALUE_1_SEC 1000

#define LTE_RRC_CEP_RANDOM_NUMBER_ATTEMPTS_50 50
/*Length of random numbers to be generated in bytes*/
#define LTE_RRC_CEP_RANDOM_NUMBER_LENGTH 4
#define LTE_RRC_CEP_UE_ID_RANDOM_NUMBER_LENGTH 5

/*Min WaitTime is 1 sec*/
#define LTE_RRC_CEP_WAIT_TIME_MIN 1

/*Min WaitTime is 16 sec*/
#define LTE_RRC_CEP_WAIT_TIME_MAX 16

/*Reselection wait time.. Time to wait for CSP to come back with a response
when Reselection is being performed*/
#define LTE_RRC_CEP_RESELECTION_WAIT_TIME 35000

/*RF UNAVAILABLE max wait time.. Time to wait for RF_AVAILABLE_IND to come back from mL1*/
#define LTE_RRC_CEP_RF_UNAVAILABLE_WAIT_TIME 4500
//#define LTE_RRC_CEP_RF_UNAVAILABLE_WAIT_TIME 1

#define LTE_RRC_CEP_T300_TOTAL_DURATION_RF_UNAVAILABLE_0 0

#define LTE_RRC_CEP_T300_TOTAL_DURATION_RF_UNAVAILABLE_DEFAULT 40
#define LTE_RRC_CEP_ACCESS_BARRING_MASK_1 0x80

#define LTE_RRC_CEP_CONN_CONTROL_WAIT_TIME 60000

/*Mask to check if AC 11-15 is present*/
#define LTE_RRC_CEP_ACCESS_CLASS_11_TO_15_MASK (0xF800)

/*Mask to check if AC 10-15 is present*/
#define LTE_RRC_CEP_ACCESS_CLASS_10_TO_15_MASK (0xFC00)

/*Mask to check if Conn Control is enabled at RRC level*/
#define LTE_RRC_CEP_RRC_CONN_CONTROL_ENABLE_MASK (0x1)

#define LTE_RRC_CEP_TRY_COUNT_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/try_count"

#define LTE_RRC_CEP_TRIGGER_OOS_AFTER_2_T300_EXPIRY_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/oos_after_2_t300_expiry"

#define LTE_RRC_CEP_CONN_CONTROL_ENABLE_EFS_FILENAME "/nv/item_files/modem/lte_connection_control"

#define LTE_RRC_CEP_CONN_CONTROL_PARAM_CFG_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/conn_control_param_cfg"

#define LTE_RRC_CEP_CONN_CONTROL_BARRING_OPTMZ_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/conn_control_barring_optmz_enable"

#define LTE_RRC_CEP_VOLTE_OPT_PLMN_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/volte_opt_plmn"

#define LTE_RRC_CEP_VOLTE_OPT_MMTEL_VIDEO_PARAM_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/mmtel_video_param"

#define LTE_RRC_CEP_VOLTE_OPT_MMTEL_VOICE_PARAM_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/mmtel_voice_param"

#define LTE_RRC_CEP_T300_TOTAL_DURATION_RF_UNAVAILABLE_THRESHOLD_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/t300_tmr_rf_unavail"

#define LTE_RRC_CEP_R12_SSAC_IN_CONN_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/r12_ssac_in_conn"

#define LTE_RRC_CEP_CONN_CONTROL_PARAM_CFG_V2_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/conn_control_param_cfg_v2"

#define LTE_RRC_CEP_R12_SMART_CONGESTION_MITIGATION__EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/r12_smart_congestion_mitigation"

#define LTE_RRC_CEP_CONN_CONTROL_V2_ENABLE_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/conn_control_v2_enable"

#define LTE_RRC_CEP_CONN_CONTROL_PARAM_CFG_V2_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cep/conn_control_param_cfg_v2"

#define LTE_RRC_CEP_EFS_PLMN_MAX_LENGTH 40

/*
** Maximum length of the PLMN list.
*/
#define LTE_RRC_CEP_PLMN_LIST_LENGTH 128 

/*
** Deadlock timer value in normal mode of operation.
*/
#define LTE_RRC_CEP_DEADLOCK_TMR_VALUE 1000 

/*
** Deadlock timer value in SLTE mode of operation.
*/
#define LTE_RRC_CEP_DEADLOCK_TMR_VALUE_SLTE 3000 


/* This macro coverts the MCC from the array format in sys_plmn_identity_type 
 * to the integer format.
 */
#define   LTE_RRC_CEP_GET_INT_MCC(plmn_id)  \
  ( plmn_id.mcc[0] * 100 +  plmn_id.mcc[1] * 10 + plmn_id.mcc[2] )


/* This macro coverts the MNC from the array format in sys_plmn_identity_type
 * to the integer format.
 */
#define   LTE_RRC_CEP_GET_INT_MNC(plmn_id)  \
( \
  (plmn_id.num_mnc_digits == 2) ? \
    (plmn_id.mnc[0] * 10 + plmn_id.mnc[1]) : \
    (plmn_id.mnc[0] * 100 + plmn_id.mnc[1] * 10 + plmn_id.mnc[2]) \
)


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Enumeration for MAC Access abort reason with the enum values 
  denoting the priorities of the RACH abort event. While RACH abort is being
  processed, if a higher priority event for RACH abort is received, the abort
  reason is updated to the higher priority event reason. The action on receiving
  abort CNF will be for the higher priority event.
*/
typedef enum
{
  LTE_RRC_CEP_RACH_ABORT_REASON_NONE,         /*!< 0 - lowest priority */
  LTE_RRC_CEP_RACH_ABORT_REASON_RESEL_STARTED, /*!< 1 */
  LTE_RRC_CEP_RACH_ABORT_REASON_NOT_CAMPED,   /*!< 2 */
  LTE_RRC_CEP_RACH_ABORT_REASON_T300_EXPIRY,  /*!< 3 */
  LTE_RRC_CEP_RACH_ABORT_REASON_CONN_ABORT,   /*!< 4 */
  LTE_RRC_CEP_RACH_ABORT_REASON_RRC_STOPPED,  /*!< 5 - highest priority */
} lte_rrc_cep_rach_abort_reason_e ;

/*! @brief Enumeration for storing PLMN list for Volte optimization
*/
typedef struct
{
  uint8                   length;
  lte_rrc_plmn_s       plmn[LTE_RRC_CEP_PLMN_LIST_LENGTH];
} lte_rrc_cep_volte_plmn_list_s_type;


/*! @brief CEP Conn Control structure
*/
typedef struct
{
  /*! Count to keep track of Conn Rej/T300 expiry */
  uint8 c_rrc;

  /*! Timestamp of the first conn reject */
  uint64 timestamp_first;
  
  /*! Timestamp of themost recent conn reject */
  uint64 timestamp_last;

  /*! Cell considered for Conn Control processing */
  lte_rrc_cell_id_s cell_id;
}lte_rrc_cep_conn_control_s;

/*! @brief Dynamic data type 
*/
typedef struct
{
  lte_rrc_est_cause_e est_cause; /*!< Establishment cause from the most recent 
                                     ConnEstReq */
  uint8 *encoded_conn_request_ptr; /*!< Most Recently sent ConnectionRequest 
                                    (in encoded format)*/    
  boolean encoded_conn_request_is_valid; 

  uint32 encoded_conn_request_length; /*!< Length of the encoded 
                                           ConnectionRequest message */
  uint32 rel_cfg_transaction_id; /*!< Transaction ID for Release Config Req to LLC */

  boolean rel_cfg_sent; /*!< Release cfg req sent*/

  lte_rrc_osys_UL_CCCH_Message *unencoded_conn_request_ul_ccch_ptr;
                                      /*!< Most recently sent
                                       UL CCCH Messaage container for
                                       ConnectionRequest
                                     (in unencoded format)*/
  boolean unencoded_conn_request_is_valid;
  
  lte_rrc_conn_est_req_s conn_est_req; /*!< Most recently received
                                    Connection Request from NAS*/
                                 
  lte_rrc_osys_DL_CCCH_Message *dl_ccch_msg_ptr;  /*!< Pointer to Decoded DL CCCH msg */

  uint32 cfg_transaction_id; /*!< Transaction ID for the LLC Config Req*/

  lte_rrc_osys_RRC_TransactionIdentifier conn_setup_transaction_id;
                            /*!Transaction ID for the latest
                              Conn Establishment. Received in ConnSetup msg*/ 

  uint8 conn_setup_complete_muid;/*!Messaage Handler
                                  MUID for Connection Setup Complete*/ 

  uint8 encode_status; /*!Message Handler encode status for
                          ConnectionSetupComplete*/

  lte_rrc_tmr_s t300_tmr; /*!< T300 timer */
  boolean t300_timer_is_running;/*!< Is T300 timer running ?*/

  lte_rrc_tmr_s resel_tmr; /*!< Reselection timer*/
  boolean resel_timer_is_running;/*!< Is the reselection timer running ?*/ 

  uint8 conn_establishment_retry_count; /*!Retry count*/
  uint8 resel_count; /*!Reselection count*/

  boolean resel_started; /*!State variable to keep track if reselection started*/

  uint64 t300_tmr_value;/*!< Timer value for T300*/

  uint64 t300_tmr_rf_unavail;/*!< Timer which tells how long RF
                                             was unavailable when T300 is running*/

  uint8 access_barring_prob[LTE_RRC_CEP_MAX_NUM_ACCESS_PROBS] ;
  /*!< Access Barring Probability Table */
  uint16 access_barring_time[LTE_RRC_CEP_MAX_NUM_ACCESS_BARRING_TIME] ; 
  /*!< Access Barring Timer Value Table */
  uint16 t300_time[LTE_RRC_CEP_MAX_NUM_T300_TIME];
  /*!< T300 Timer (Connection Setup timer) Value Table */

  uint32 random_number; /*!< Random number for test purposes*/

  boolean rach_in_progress;/*!< Is RACH in progress ?*/

  lte_rrc_cep_rach_abort_reason_e rach_abort_reason; /*!< Reason for aborting 
                                                          RACH, if any */

  uint32 conn_est_transaction_id; /*< Transaction ID for ConnEstReq*/

  lte_rrc_conn_est_status_e conn_est_status; /*!< Connection Establishment 
                                                  status, if applicable */
  
  boolean revoke_requested;/*!< This flag is set to TRUE if a 
                                revoke request has been sent by CEP*/ 

  uint16 tac; /*!< Tracking area code of the cell on which the previous 
                   connection establishment was made*/

  lte_rrc_plmn_s plmn; /*!< PLMN ID for the cell on which the previous
                            connection establishment was made*/

  //CEP opt 
  uint16 cep_opt_count; /*!< Value of the opt count  */
  //CEP opt for IM3
  uint16 max_cep_im3_opt_count; /*!< Value of the IM3 opt count  */
  boolean hplmn_ehplmn_is_valid;/*Are we EHPLN OR (HPLMN if EHPLMN list is empty)?*/

  boolean home_country_is_valid;/*Are we in Home Country ?*/

  boolean hplmn_is_valid;/*Is the UE in home Country?*/

  boolean is_irat_psho_to_lte_in_progress;/*Is PSHO to LTE in progress?*/

  boolean conn_control_proc_is_pending; /*! Flag to indicate that Conn Control processing is pended 
                                                 till MAC access abort cnf whose req was raised by t300 expiry*/
                                                 	
  boolean is_rf_unavailable;/*Is RF unavailable?*/

  lte_rrc_tmr_s rf_unavailable_tmr; /*!< rf unavailable timer*/

  boolean rf_unavailable_timer_is_running;/*!< Is the rf unavailable timer running ?*/ 
                                                 	
  boolean conn_abort_in_progress;/*!< Is CONN_ABORT in progress ?*/
                                                 	
  uint16 rem_deadlock_time; /*!< Remaining time with which Deadlock timer needs to be restarted after rf_avail*/
 
  /*! Delay before RACH is started for special AC 11-15 when criterion is met*/
  uint16 conn_est_delay;

  /*! Cumulative delay before initiating RACH (should be <= 1 secs)*/
  uint16 time_conn_est_delayed;

  /*! Timer Duration for which RACH will be delayed for AC 11-15 when criterion is met*/
  lte_rrc_tmr_s acb_delay_rach_tmr;                                               	
  
  /* Boolean which keeps track of first MSG1 Tx ind 
    received from ML1 UL*/    
  boolean first_msg1_tx_ind_sent;                      	
  
  /* Boolean which keeps track of whether EAB barring timer is running at NAS or not*/    
  boolean eab_barring_tmr_running; 
} lte_rrc_cep_dyn_data_s;  

/*! @brief Dynamic data type 
*/
typedef struct
{ 
  lte_rrc_tmr_s t302_tmr; /*!< T302 timer*/
  uint64 t302_tmr_value;/*!< Timer value for T302*/

  lte_rrc_tmr_s t303_tmr; /*!< T303 timer*/
  lte_rrc_tmr_s t305_tmr; /*!< T305 timer*/
  lte_rrc_tmr_s t306_tmr; /*!< T305 timer*/

  uint8 num_max_tries; /*Number of maximum Connection Establishment Tries*/

  uint8 trigger_oos_after_2_t300_expiry; /*Trigger OOS after 2nd T300 expiry*/

  //CEP opt
  uint16 max_cep_opt_count; /*!< Value of the opt count  */

 //CEP opt for IM3
  uint16 max_cep_im3_opt_count; /*!< Value of the IM3 opt count  */

  //Conn Control enable/disable cfg
  uint8 rrc_conn_control_enable; /*!< Conn Control enable if set to 1, else 0  */

  //Use of barring time optmz (with-held further conn req) enable/disable cfg
  uint16 conn_control_barring_optmz_enable;

  //Conn Control cfg params
  uint8 c_rrc_thres; /*Number of minimum Connection Establishment Rejects/T300 expiries to declare OOS*/
  uint32 t_rrc_tmr_value; /*Time (in seconds) to bar the cell when declaring OOS due to Conn Control*/
  uint64 max_time_diff; /*Time (in ms) allowed between two connection rejects or t300 expiry*/
  uint32 conn_est_latency; /*Time (in ms) considered as the minimum time duration for conn est procedure*/
  uint32 conn_control_wait_time; /*Time (in ms) considered as time window to track the Crrc */

  lte_rrc_cep_conn_control_s conn_control_db;  

  lte_rrc_cep_volte_plmn_list_s_type volte_opt_plmn_list;

  /*!< SSAC params - valid only if svc_status is LTE_RRC_SVC_STATUS_AVAILABLE */
  lte_rrc_ssac_params_s ssac_params; 
  
  /*!< Value read from EFS */
  uint8 t300_tmr_rf_unavail_thresh;

 /*!< Value read from EFS to enable/disable R-12 SSAC feature*/
  uint8 r12_ssac_in_conn;

  /*!< Value read from EFS to enable/disable AC Barring param restriction for special AC 11-15*/
  uint8 ac_barring_restriction_param;

  lte_rrc_tmr_s deadlock_tmr; /*!< Deadlock timer for RACH_ABORT_REQ*/
 
  /*! RACH delay for special AC 11-15 feature enabled*/
  uint8 acb_delay_rach_enabled;
  /*! Internal barring factor array: Received from NAS in conn_est_req */
  uint8 acb_delay_rach_barring_factor_array[LTE_RRC_CEP_MAX_NUM_ACCESS_PROBS];
  /*! Boolean which indicates if GCF flag is set*/
  boolean gcf_flag_is_set;
 
  /*!< Value read from EFS to enable/disable R-12 smart congestion mitigation feature*/
  uint8 r12_smart_congestion_mitigation;

  //Conn Control v2 enable/disable cfg - Another method of Conn Control-Tweaked
  uint8 rrc_conn_control_v2_enable;
}lte_rrc_cep_static_data_s; 
/*! @brief Private data type for CEP
*/
typedef struct
{
  lte_rrc_cep_static_data_s *sd_ptr; /*!< Dynamic data pointer */

  lte_rrc_cep_dyn_data_s *dd_ptr; /*!< Dynamic data pointer */
} lte_rrc_cep_s;
#endif /* LTE_RRC_CEPI_H */
