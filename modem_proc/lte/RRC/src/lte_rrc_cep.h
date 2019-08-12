/*!
  @file lte_rrc_cep.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_cep.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/12   kp        Added code to provide ac-barring info from SIB2 to NAS in srv_ind
04/26/10   da      Added prototype for lte_rrc_cep_get_state
07/29/08   dd      Initial Version
===========================================================================*/

#ifndef LTE_RRC_CEP_H
#define LTE_RRC_CEP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_cep.h>



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Enumeration for access barring status 
*/
typedef enum
{
  LTE_RRC_CEP_ACCESS_BARRED, /*!< 0 */
  LTE_RRC_CEP_ACCESS_NOT_BARRED, /*!< 1 */
  LTE_RRC_CEP_ACCESS_CHECK_FOR_SPECIAL_AC, /*!< 2 */
  LTE_RRC_CEP_ACCESS_UNINITIALIZED, /*!< 3 */
} lte_rrc_cep_access_barring_status_e ;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_cep_get_state

===========================================================================*/
/*!
    @brief
    Returns the current state of RRC CEP

    @return
    stm_state_t

*/
/*=========================================================================*/
stm_state_t lte_rrc_cep_get_state
(
  void
);
/*===========================================================================

  FUNCTION:  lte_rrc_cep_est_cause_to_asn1

===========================================================================*/
/*!
    @brief
    Converts est cause enum to ASN1 enum
    @return
    lte_rrc_EstablishmentCause

*/
/*=========================================================================*/


extern lte_rrc_osys_EstablishmentCause lte_rrc_cep_est_cause_to_asn1
(
  lte_rrc_est_cause_e est_cause /*!< Est cause */
);

/*===========================================================================

  FUNCTION: lte_rrc_cep_init 

===========================================================================*/
/*!
    @brief
    Allocates and initializes dynamic memory
    @return
    void

*/
/*=========================================================================*/

extern void lte_rrc_cep_init(void);

/*===========================================================================

  FUNCTION: lte_rrc_cep_deinit 

===========================================================================*/
/*!
    @brief
    Deallocates  dynamic memory
    @return
    void

*/
/*=========================================================================*/

extern void lte_rrc_cep_deinit(void);

/*===========================================================================

  FUNCTION:  lte_rrc_cep_get_est_cause

===========================================================================*/
/*!
    @brief
    returns the connection establishment cause for the previous Connection Establishment
    @return
    lte_rrc_est_cause_e
*/

lte_rrc_est_cause_e  lte_rrc_cep_get_est_cause
(
  void
);

lte_rrc_conn_est_status_e  lte_rrc_cep_get_est_status
(
  void
);

boolean lte_rrc_cep_proc_conn_control_and_check_for_success
(
  uint64 *tmr_value
);

extern uint32 lte_rrc_cep_get_conn_control_cell_bar_time(void);

void lte_rrc_cep_reset_conn_control_params(void);

/*!
    @brief
    returns the barring status for energency call origination.
    @return
    lte_rrc_est_cause_e
*/
extern lte_rrc_cep_access_barring_status_e lte_rrc_cep_get_emergency_access_barred_status
(
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr,
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr
);

extern boolean lte_rrc_cep_volte_opt_match_plmns
(  
  lte_rrc_plmn_s camped_plmn
);

extern void lte_rrc_cep_get_ac_barring_params
(
  lte_rrc_ac_barring_info_s *ac_barring_info_ptr
);

extern void lte_rrc_cep_send_barring_update_ind
(  
  void
);

extern void lte_rrc_cep_read_volte_opt_plmns
(  
  int8 sub_id
);


/*===========================================================================

  FUNCTION:  lte_rrc_cep_get_ssac_params

===========================================================================*/
/*!
    @brief
    Get the SSAC parameters

    @return
    None

*/
/*=========================================================================*/

extern void lte_rrc_cep_get_ssac_params
(  
  lte_rrc_ssac_params_s* ssac_params,
  boolean* skip_acb_for_volte_call
);

extern boolean lte_rrc_cep_rach_is_in_progress
(
  void
);

extern boolean lte_rrc_cep_get_gcf_flag_status
(
  void
);

extern boolean lte_rrc_cep_get_eab_barring_tmr_status
(
  void
);

extern void lte_rrc_cep_send_eab_update_ind
(
  boolean eab_status
);

#endif /* LTE_RRC_CEP_H */
