/*!
  @file lte_rrc_taski.h

  @brief
  Internal header file for the RRC task

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_taski.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/08   ask     Ported RRC to POSIX APIs, renamed to lte_rrc_taski.h
08/26/08   ask     Updated code to use COBJWR macros 
07/18/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_TASKI_H
#define LTE_RRC_TASKI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#ifdef FEATURE_SEGMENT_LOADING
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#endif /* FEATURE_SEGMENT_LOADING */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Private data for the RRC task
*/
typedef struct
{
  boolean task_is_initialized; /*!< Flag set when the task gets initialized  */

#ifdef FEATURE_SEGMENT_LOADING
  interface_t *pi_tdscdma;
  interface_t *pi_wcdma;
#endif /* FEATURE_SEGMENT_LOADING */


} lte_rrc_task_s;     

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_RRC_TASKI_H */
