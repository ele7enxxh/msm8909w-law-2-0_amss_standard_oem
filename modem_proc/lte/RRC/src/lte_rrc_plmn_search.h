/*!
  @file
  lte_rrc_plmn_search.h

  @brief
  External API for RRC PLMN_SEARCH.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_plmn_search.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/2010 vatsac  Initial version

===========================================================================*/

#ifndef LTE_RRC_PLMN_SEARCH_H
#define LTE_RRC_PLMN_SEARCH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* This function returns TRUE if plmn search is in progress */
extern boolean lte_rrc_plmn_search_is_in_progress(void);

/* This function returns TRUE if high priority plmn search is in progress */
extern boolean lte_rrc_plmn_search_high_pri_srch_in_progress(void);

/* This function returns network_selection_mode if plmn search is in progress */
extern sys_network_selection_mode_e_type 
lte_rrc_plmn_search_get_network_select_mode(void);

/*   Memory initialization */
extern void lte_rrc_plmn_search_init(void);

/*   Memory deinitialization */
extern void lte_rrc_plmn_search_deinit(void);

/* This function populates the found_plmn_rat_list from results in 
   found_plmn_table depending upon the network mode and search type */
extern void lte_rrc_plmn_search_copy_lte_plmn
(
 const lte_rrc_plmn_list_s *plmn_ptr, /*!< PLMN pointer */
 uint32 plmn_index, /*!< Index of the PLMN in the PLMN list to be copied */
 sys_plmn_id_s_type *sys_plmn_ptr /*!< Sys PLMN to be copied into */
);

/* Checks if NAS supplied info and SIB1 PLMN list result in a selection. 
   Returns TRUE if selected, FALSE otherwise. If TRUE, plmn_index in SIB1 list
   is filled in */
extern boolean lte_rrc_plmn_search_check_plmn_match
(
 const sys_plmn_rat_list_s_type *req_plmn_list_ptr, /*!< NAS requested info */
 const lte_rrc_plmn_list_s      *sib1_plmn_list_ptr, /*!< SIB1 PLMN list */
 uint32                         *plmn_index_ptr /*!< PLMN index to be filled in */
);

extern lte_earfcn_t lte_rrc_plmn_search_get_alternate_earfcn
(
 sys_sband_lte_e_type old_band,    /*!< Old band */
 lte_earfcn_t old_earfcn, /*!< Old Earfcn */
 sys_sband_lte_e_type new_band    /*!< New band */
);

extern boolean lte_rrc_plmn_search_is_irat_to_lte_search_done(void);

#endif /* LTE_RRC_PLMN_SEARCH_H */
