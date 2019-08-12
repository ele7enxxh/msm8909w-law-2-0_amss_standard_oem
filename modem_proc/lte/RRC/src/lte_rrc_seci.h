/*!
  @file lte_rrc_seci.h
  

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_seci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/11    kp       Added code to pre-compute keys so that processing time of SMC is reduced.
08/01/09    dd       Security 2.0 and March RRC spec migration
02/17/09    dd       Initial Version
===========================================================================*/

#ifndef LTE_RRC_SECI_H
#define LTE_RRC_SECI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_int_msg.h"
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_rrc_utils.h>
#include <lte.h>
#include <lte_pdcp_msg.h>
#include "lte_security.h"


#define LTE_RRC_SEC_SMC_FAIL_TIME 500
#define LTE_RRC_SEC_NUM_AS_CONTEXTS 2
#define LTE_RRC_SEC_NUM_NAS_CONTEXTS 2
#define LTE_RRC_SEC_MAX_SMC_SIZE 1000
#define LTE_RRC_SEC_MAX_VERTICAL_KEY_DEPTH 8
#define LTE_RRC_SEC_A2_KEY_GROUPS 5
#define LTE_RRC_SEC_INVALID_NAS_CONTEXT_INDEX 0xff
#define LTE_RRC_SEC_NUM_DRB 32
#define LTE_RRC_SEC_COUNTER_CHECK_BIT_SHIFT 7
#define LTE_RRC_SEC_COUNTER_CHECK_MAX_DRB 11

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief AS Security Context
*/

typedef struct
{
  /*pre computed keys gen at sim_update time*/
  uint8 k_rrc_int_snow3g_algo[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc_snow3g_algo[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc_snow3g_algo[LTE_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/

  uint8 k_rrc_int_aes_algo[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc_aes_algo[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc_aes_algo[LTE_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/

  uint8 k_rrc_int_zuc_algo[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc_zuc_algo[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc_zuc_algo[LTE_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/
}lte_rrc_sec_pre_comp_as_security_keys_s;

typedef struct
{
  lte_integrity_algo_e integrity_algo;/*! Integrity Algo*/
  lte_cipher_algo_e cipher_algo;/*! Cipher Algo*/
  uint8 ncc;  /*!Next Hop Chaining Count (NCC) */
  uint8 k_enb[LTE_GENERATOR_KEY_LEN];
  uint8 k_rrc_int[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc[LTE_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc[LTE_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/
  boolean is_initial_k_enb; /*!Is this K_eNB derived from a new NAS context ?*/
  uint8 nh[LTE_GENERATOR_KEY_LEN]; /*! NextHop (NH) Key*/

  lte_rrc_sec_pre_comp_as_security_keys_s pre_comp_keys;
  
  boolean freq_pci_is_valid;/*! Is the DL-EARFCN and PCI info valid ?
  This is set to FALSE from the time of SMC to the first handover. It is set to TRUE 
  from then onwards*/
  lte_phy_cell_id_t phy_cell_id;  /*!Phy cell ID of the target cell*/
  lte_earfcn_t freq ;   /*!Downlink EARFCN of the target cell*/
}lte_rrc_sec_as_security_context_s;



/*! @brief NAS Security Context
*/

typedef struct
{
  lte_rrc_security_info_s sec_info_from_nas;/*! Security Info from NAS*/
  uint8 k_enb_initial[LTE_GENERATOR_KEY_LEN];/*!Initial K eNB derived from K-ASME*/
}lte_rrc_sec_nas_security_context_s;

typedef struct
{
  uint32 ul_count;/*!< UL Count*/
  uint32 dl_count;/*!< DL Count*/
  boolean count_is_valid; /*!< Is count valid ?*/
  boolean included_in_counter_check; /*!< Is included in counter check*/
}lte_rrc_sec_count_info_s;

/*! @brief Private data type for security module 
*/
typedef struct
{
  lte_rrc_osys_RRC_TransactionIdentifier dlm_transaction_id;
                                        /*!Transaction ID for the latest recvd DLM...
                                        SMC or Reconfig*/ 
  uint32 cfg_transaction_id;/*!< Transaction ID for Config Request*/ 
  uint8 ulm_muid;/*!Messaage Handler MUID UL messages*/ 

  uint8 encode_status; /*!Encode status for SecModeComplete*/

  lte_rrc_tmr_s smc_fail_tmr; /*!< SMC fail timer*/
  boolean smc_fail_timer_is_running;/*!< Is SMC timer running ?*/

  lte_rrc_sec_as_security_context_s as_security_context[LTE_RRC_SEC_NUM_AS_CONTEXTS];
          /*!< AS security Contexts*/

  uint8 current_as_security_context_index; /*!< Current AS Security Context index*/

  uint8 new_as_security_context_index; /*New AS Security Context index */ 

  uint8 a2_key_index_n; /*!<Key indices are 3*n, 3*n+1 and 3*n+2 */ 

  lte_rrc_sec_nas_security_context_s nas_security_context[LTE_RRC_SEC_NUM_NAS_CONTEXTS]; 
   /*!< NAS Security contexts*/

  uint8 current_nas_security_context_index; /*!< Current NAS Security Context index*/

  int new_nas_security_context_index; /*New NAS Security Context index */

  boolean nas_security_context_in_use; /*!< Is NAS security context valid ?*/

  boolean security_is_configured; /*Is security Configured ?*/

  boolean null_integrity_is_valid; /*Is NULL integrity valid ?*/

  boolean eia0_is_valid; /*Is EIA0 valid ?*/

  boolean smc_maci_is_zero; /*Is MAC-I 0 for SMC ?*/

  /*Valid drb_CountMSB_InfoList (containing Counter Check Req)*/
  lte_rrc_osys_DRB_CountMSB_InfoList drb_CountMSB_InfoList;

  boolean counter_check_is_in_progress; /*Is counter check Procedure in progress ?*/

  lte_pdcp_count_info_s pdcpdl_count_info[LTE_MAX_ACTIVE_RB]; /*PDCP DL count info*/
  uint8 num_active_drb_dl;    /*Number of Active DRBs in DL*/ 

  lte_pdcp_count_info_s pdcpul_count_info[LTE_MAX_ACTIVE_RB]; /*PDCP UL count info*/
  uint8 num_active_drb_ul; /*Number of Active DRBs in UL*/

  lte_rrc_sec_count_info_s count_info[LTE_RRC_SEC_NUM_DRB];/*!< Count info*/

  lte_sec_errno_e err_status; /*!< Error status returned from lte_security_xyz function*/

  boolean irat_to_l_ho_in_progress;/*!< IRAT X to L is in progress*/
  
} lte_rrc_sec_dyn_data_s;
/*! @brief Static private data type for SEC
*/
typedef struct
{
  uint8 eia0_allowed; /*!< Allow EIA0*/
} lte_rrc_sec_static_data_s;


/*! @brief Private data type for CEP
*/
typedef struct
{
  lte_rrc_sec_static_data_s *sd_ptr;/*!< Static data pointer*/ 
  lte_rrc_sec_dyn_data_s *dd_ptr; /*!< Dynamic data pointer */
} lte_rrc_sec_s;

  
typedef struct {
   lte_rrc_osys_DRB_Identity drb_id;
   uint32 ul_count_msb;
   uint32 dl_count_msb;
} lte_rrc_sec_counter_check_info_s;

typedef struct {
  uint8 num_ele;
  lte_rrc_sec_counter_check_info_s drb_info[LTE_RRC_SEC_COUNTER_CHECK_MAX_DRB];
} lte_rrc_sec_counter_check_list_s;


  
#endif /* LTE_RRC_SECI_H */
