/*!
  @file lte_rrc_paging.h

  @brief
  Includes for the RRC paging procedure.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_paging.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_PAGING_H
#define LTE_RRC_PAGING_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates dynamic memory for dd_ptr of private data and initializes private 
   data */
extern void lte_rrc_paging_dd_init(void);

/* Deallocates dynamic memory part of private data */
extern void lte_rrc_paging_dd_deinit(void);

/* Get UE specific paging cycle */
extern uint16 lte_rrc_paging_get_ue_specific_paging_cycle(void);

#endif /* LTE_RRC_PAGING_H */
