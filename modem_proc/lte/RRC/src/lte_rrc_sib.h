/*!
  @file
  lte_rrc_sib.h

  @brief
  SIB module's exposed APIs.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_sib.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/10   sureshs API to get B12 to B17 conversion needed
06/24/10   np      API to check scheduling info from SIB1
08/28/09   amit    Updated code to remove lint warnings
10/08/08   amit    Renamed lte_rrc_get_cell_sib to lte_rrc_sib_get_cell_sib
10/07/08   sureshs Added include of lte_rrc_int_msg.h
10/07/08   sureshs Added sib_db_index to lte_rrc_get_cell_sib
10/03/08   sureshs Added lte_rrc_get_cell_sib function
08/04/08   amit    Initial Version
===========================================================================*/

#ifndef LTE_RRC_SIB_H
#define LTE_RRC_SIB_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "lte_rrc_int_msg.h"
#include "lte_rrc_osys_asn1util.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Maximum number of cells in SIB DB
*/
#define LTE_RRC_SIB_DB_MAX_CELLS 10

/*! @brief Offset when reading the sib mapping info in SIB1
*/
#define LTE_RRC_SIB_MAPPING_INFO_OFFSET 3



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates dynamic memory for dd_ptr of private data and initializes private 
   data */
extern void lte_rrc_sib_dd_init(void);

/* Deallocates dynamic memory part of private data */
extern void lte_rrc_sib_dd_deinit(void);

/* Function used by other modules to get SIBs from sib_db */
extern void *lte_rrc_sib_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib_bitmask, /*!< Sib requested; LTE_RRC_SIB1_BITMASK=SIB1, LTE_RRC_SIB2_BITMASK=SIB2, ...*/
  lte_rrc_sib_db_key_t sib_db_key /*!< Index in sib_db of the cell */
);

/* Is B12 to B17 conversion needed */
extern boolean lte_rrc_sib_b12_to_b17_conversion_is_needed(void);

/* Reads the SIB1 Scheduling Information passed as parameter and finds 
    out the SIBs scheduled. */
extern lte_rrc_sib_bitmask_t lte_rrc_sib_sibs_scheduled_in_sib1_sched_info
(
  lte_rrc_osys_SchedulingInfoList *sched_info_ptr  /*!< Pointer to SIB1 Sched Info */
);

/* Is sib reading is active */
extern boolean lte_rrc_sib_read_is_active(void);

/* IF eMBMS SIB reading is currently active*/
extern boolean lte_rrc_sib_read_active_for_embms(void);

/* Is sib13 scheduled in sib1 */
extern boolean lte_rrc_sib_sib13_is_sched(void);

/*Is SibX scheduled in sib1*/

extern boolean lte_rrc_sib_sibx_is_sched
(
   lte_rrc_sib_bitmask_t sib_mask
);
extern boolean  lte_rrc_sib_get_is_sib14_mandatory
(
);
extern boolean  lte_rrc_sib_get_is_sib14_available
(
);

/* Reads the mfbi_index from SIB DB */
extern uint8 lte_rrc_sib_get_mfbi_index
(
  lte_rrc_sib_db_key_t sib_db_key /*!< Index in sib_db of the cell */
);

/* Reads Value tag from SIB DB */
extern boolean lte_rrc_sib_get_value_tag_in_db
(
  lte_rrc_sib_db_key_t sib_db_key, /*!< Index in sib_db of the cell */
  uint32 * value_tag /*!< Ptr to Value tag */
);

/* Funtions predicts whether bplmn search can proceed based on
    sibs scheduled and time available for plmn search*/
extern boolean lte_rrc_sib_proceed_with_bplmn_check
(
  uint32 time_avail /*!< Time available for Irat BPLMN search */
);

#endif /* LTE_RRC_SIB_H */
