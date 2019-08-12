/*!
  @file
  lte_rrc_crpi.h

  @brief
  Internal header file of RRC Connection Release Procedure containing private 
  data.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_crpi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/2010 sureshs Split data into static and dynamic parts
04/07/2008 sureshs Removed timer is running flag
04/02/2008 sureshs Added message pointer field
12/09/2008 sureshs Added more cell barring params
11/04/2008 sureshs Added redir info
10/29/2008 sureshs Removed conn_rel_msg_ptr
09/10/2008 sureshs Added cell_barring_is_reqd field
09/09/2008 sureshs Added fields for conn_rel_msg_ptr and tau_is_reqd
09/06/2008 sureshs Added timer flag and connection release reason
09/05/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CRPI_H
#define LTE_RRC_CRPI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_utils.h"
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_int_msg.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Dynamic data type
*/
typedef struct
{
  lte_rrc_tmr_s conn_rel_tmr; /*!< Connection Release Timer */
  lte_rrc_conn_rel_reason_e conn_rel_reason; /*!< Connection Release reason */
  uint32 trans_id; /*!< Transaction ID for LLC config request */
  boolean load_balancing_tau_is_reqd; /*!< Is TAU needed to be done by upper
                                           layers */
  boolean cs_FallbackHighPriority; /*!< Is Connection release associated with redirection 
                                        and cs Fall back high priority */
  boolean cell_barring_is_reqd; /*!< Is cell barring required by CSP */
  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id of cell to be barred */
  lte_earfcn_t earfcn;  /*!< Frequency of cell to be barred */
  boolean redir_info_is_present; /*!< Is redirection info present? */
  lte_rrc_redir_info_s redir_info; /*!< Redirection info - only valid if 
                                        redir_info_is_present is TRUE */
  lte_rrc_redir_opt_info_s redir_opt_info; /*!< Neighors info for redirection */
  uint64 t_320_ms; /*!< T320 timer value */
  lte_irat_dedicated_priority_list_s freq_prio_list; /*!< Dedicated priority list */
  lte_rrc_ml1_status_type_e ml1_status; /*!< ML1 State */
  boolean                               is_acq_needed; /*!< Connection Release to do ACQ */
  lte_cphy_conn_release_acq_cell_info_s acq_cell_info;/*!< ACQ cell info valid only if 
                                                                                   is_acq_needed is TRUE*/
  uint16 extendedWaitTime_r10; /*!<  value comes in RRC Connection release DLM*/
} lte_rrc_crp_dyn_data_s;

/*! @brief Private data type
*/
typedef struct
{
  lte_rrc_crp_dyn_data_s *dd_ptr; /*!< Dynamic data pointer */
} lte_rrc_crp_s;

#endif /* LTE_RRC_CRPI_H */


