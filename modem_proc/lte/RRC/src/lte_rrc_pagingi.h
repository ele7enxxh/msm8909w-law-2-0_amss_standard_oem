/*!
  @file lte_rrc_pagingi.h

  @brief
  Internal definitions for Paging procedure

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_pagingi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/10   amit    Added etws_supported to static data 
04/16/10   amit    Moved default_paging_cycle from static data to dynamic data
04/16/09   amit    Added page_ind to lte_rrc_paging_s
09/10/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_PAGINGI_H
#define LTE_RRC_PAGINGI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_int_msg.h>

/*! @brief Default value for mod_per_coeff in SIB2
*/
#define LTE_RRC_PAGING_MOD_PER_COEFF_DEFAULT 2

/*! @brief Maximum time duration to ignore pages in Conn 
without triggering out of sync condition
*/
#define LTE_RRC_PAGING_CONN_PAGE_IGNORE_DURATION 10000

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief This structure holds the Static part of private data
*/
typedef struct
{
  lte_rrc_paging_cycle_e    default_paging_cycle;  /*!< Default paging cycle for the cell */
  lte_rrc_paging_cycle_e    specific_paging_cycle; /*!< UE-specific paging cycle */
  lte_rrc_ue_id_s ue_id[LTE_RRC_UE_ID_MAX];
  
#ifdef FEATURE_SET_UEID_FOR_PG
  lte_rrc_imsi_s efs_imsi_for_page;
#endif

  boolean ue_id_valid[LTE_RRC_UE_ID_MAX];
  /*!< Boolean indicating whether ETWS is supported */
  boolean etws_supported;
  /*!< Boolean indicating whether CMAS is supported */
  boolean cmas_supported;
  /*!< Timer duration to ignore the pages in Conn Mode */
  uint16 conn_page_ignore_duration;
  /*!< Scaling factor for the drx cycle */
  uint8 drx_scaling_factor;
  /*!< Boolean indicating whether number of drx cycles has to be reduced by half or not*/
  boolean half_drx_cfg_valid;
} lte_rrc_paging_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  lte_rrc_nb_e              nb;
  lte_rrc_page_ind_s page_ind;
  /*!< Holds phy_cell_id and freq of the current serving cell*/
  lte_rrc_cell_id_s cell_id;
  boolean page_ind_buffered;
  /*!< mod period coefficient read from SIB2 */
  uint16 mod_per_coeff;
  /*!< time snapshot when UE enters connected */
  uint64 conn_entry_timestamp;
} lte_rrc_paging_dynamic_data_s;

/*! @brief Typedef of variables internal to module lte_rrc_paging.c
*/
typedef struct
{
  /*!< Static part of private data */
  lte_rrc_paging_static_data_s *sd_ptr;
  /*!< Dynamic part of private data */
  lte_rrc_paging_dynamic_data_s *dd_ptr;
} lte_rrc_paging_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_RRC_PAGINGI_H */
