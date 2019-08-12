/*!
  @file lte_rrc.h

  @brief
  Exports all externally callable RRC functions and externally visible RRC
  data members

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/inc/lte_rrc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/09   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_H
#define LTE_RRC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_task_init

===========================================================================*/
/*!
  @brief
  Initializes the RRC task

  @return
  Thread id.
*/
/*=========================================================================*/
pthread_t lte_rrc_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);


#endif /* LTE_RRC_H */
