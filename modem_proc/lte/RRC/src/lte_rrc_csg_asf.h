/*!
  @file
  lte_rrc_csg_asf.h

  @brief
  External API for RRC CSG_ASF.

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_csg_asf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/2012 vatsac  Initial version

===========================================================================*/

#ifndef LTE_RRC_CSG_ASF_H
#define LTE_RRC_CSG_ASF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


extern void lte_rrc_csg_asf_init(void);

extern void lte_rrc_csg_asf_deinit(void);

/* Checks if LTE CSG cell camping is disabled */
extern boolean lte_rrc_csg_asf_l_csg_camping_allowed(void);

/* Checks if LTE CSG ASF is disabled */
extern boolean lte_rrc_csg_asf_l_asf_allowed(void);

/* Checks if LTE to W ASF search is disabled */
extern boolean lte_rrc_csg_asf_l_to_w_asf_allowed(void);

/* Checks if W to LTE ASF search is disabled */
extern boolean lte_rrc_csg_asf_w_to_l_asf_allowed(void);

/* Checks if LTE TDD CSG is disabled */
extern boolean lte_rrc_csg_asf_l_tdd_csg_allowed(void);

/* Checks if LTE Hybrid cell camping is disabled */
extern boolean lte_rrc_csg_asf_l_hybrid_allowed(void);

/* This function checks if the CSG,PLMN is present in the whitelist */
extern boolean lte_rrc_csg_asf_check_csg_in_whitelist
(
 const lte_rrc_plmn_s *plmn_ptr, /*!< PLMN ptr */
 sys_csg_id_type csg_id, /*!< CSG id */
 lte_earfcn_t freq /*!< Freq */
);

/* This function populates the chans list from the csg db */
extern void lte_rrc_csg_asf_populate_lte_chans_list
(
 sys_lte_band_mask_e_type         band_cap,          /*!< Band Capabilities */
 const source_cell_info_type      *w_macro_ptr,       /*!< W MAcro cell info ptr */
 lte_cphy_bplmn_chan_s            *chans_ptr,         /*!< Frequency list */
 uint8                            *num_chans_ptr      /*!< num frequencies */
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#endif /* LTE_RRC_CSG_ASF_H */
