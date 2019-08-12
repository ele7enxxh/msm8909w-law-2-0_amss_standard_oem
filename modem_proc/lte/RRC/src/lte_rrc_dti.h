/*!
  @file
  lte_rrc_dti.h

  @brief
  Internal API for RRC DT Message Handler.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_dti.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
01/06/11   mm       Changed Max UL Data records size to one more than what MH can handle
12/10/2008 vatsac   Initial version
===========================================================================*/

#ifndef LTE_RRC_DTI_H
#define LTE_RRC_DTI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_dt.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_int_msg.h>
#include "lte_rrc_mh.h"
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief Data type for DT SM
*/
typedef struct
{
    boolean record_is_valid;            /*!< Record validity */
    uint8 mu_id;                        /*!< MH unique id */
    uint32 trans_id;                    /*!< NAS transaction id */
    boolean cnf_is_reqd;                /*!< Confirmation required to NAS */
} lte_rrc_dt_ul_data_record_s;

#define LTE_RRC_DT_MAX_UL_DATA_RECORDS (LTE_RRC_MH_MAX_BOOK_KEEPING_MSG + 1)  /*!< Max records in table */

/*! @brief Dynaimc Private context of DT SM
*/
typedef struct
{
    /*! Circular buffer to keep track of UL transactions from NAS */
    lte_rrc_dt_ul_data_record_s
    lte_rrc_dt_ul_data_table[LTE_RRC_DT_MAX_UL_DATA_RECORDS];

    /*! Circular buffer to keep track of IRAT transactions from upper layers */
    lte_rrc_dt_ul_data_record_s
    lte_rrc_dt_irat_ul_data_table[LTE_RRC_DT_MAX_UL_DATA_RECORDS];

    uint8 ul_data_table_buffer_head;

    uint8 irat_ul_data_table_buffer_head;

               /*!< Table to keep track of UL transactions state from NAS */
    lte_rrc_ul_msg_fail_cause_e lte_rrc_blocked_state_cause;
               /*!< Reason to be in BLOCKED state*/
} lte_rrc_dt_dynamic_data_s;

/*! @brief Private context of DT SM
*/
typedef struct
{
    lte_rrc_dt_dynamic_data_s *d_ptr;  /*!< Dynamic data pointer */
} lte_rrc_dt_data_s;

#endif /* LTE_RRC_DTI_H */

