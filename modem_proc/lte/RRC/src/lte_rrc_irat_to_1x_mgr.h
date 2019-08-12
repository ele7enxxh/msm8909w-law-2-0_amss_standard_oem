/*!
  @file
  lte_rrc_irat_to_1x_mgr.h

  @brief
  External API for RRC 1x IRAT Mgr.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_to_1x_mgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/10   np      Add helper function to check if e1xCSFB is supported
04/05/10   np      Initial version
===========================================================================*/

#ifndef LTE_RRC_IRAT_TO_1X_H
#define LTE_RRC_IRAT_TO_1X_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates and initializes dynamic memory */
extern void lte_rrc_irat_to_1x_mgr_init(void);

/* Deallocates dynamic memory */
extern void lte_rrc_irat_to_1x_mgr_deinit(void);

extern boolean lte_rrc_irat_to_1x_csfb_supported(void);

extern void lte_rrc_irat_to_1x_mgr_send_sib8_unavailable_to_1x(void);

extern boolean lte_rrc_irat_to_1x_mgr_ho_in_prog(void);

#endif /* LTE_RRC_IRAT_TO_1X_H */

