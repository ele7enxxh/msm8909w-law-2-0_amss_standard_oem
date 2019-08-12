#ifndef RFMEAS_FTM_MDSP_H
#define RFMEAS_FTM_MDSP_H

/*! 
  @file
  rfmeas_ftm_mdsp.h
 
  @brief
  This module defines the MDSP interfaces used in FTM mode 
  that are common across all techologies. 
*/

/*==============================================================================

  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/mdsp/qdsp6_meas/inc/rfmeas_ftm_mdsp.h#1 $ 

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/10/13   pl      Update execute irat script interface to allow reset of 
                   IRAT buffer 
10/14/10   pl      Initial Revision.

==============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfmeas_types.h"

/*============================================================================== 
 
                               MACROS AND TYPEDEFS 
 
==============================================================================*/

/*============================================================================== 
 
                             FUNCTION DECLARATIONS
 
==============================================================================*/

/*----------------------------------------------------------------------------*/

void rfmeas_ftm_mdsp_execute_irat_script(rfm_meas_program_script_type script_type, uint32 buffer_index);

#endif /* RFMEAS_FTM_MDSP_H */

