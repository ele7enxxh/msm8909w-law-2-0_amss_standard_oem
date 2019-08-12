#ifndef __DDR_COEX_NPA_H__
#define __DDR_COEX_NPA_H__
/*!
  @file ddr_coex_npa.h

  @brief
   NPA support for DDR freq plan update
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/ddr_coex/inc/ddr_coex_npa.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/21/13   jm      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  ddr_coex_npa_update_freq_plan

=============================================================================*/
/*!
    @brief
    Checks if freq plan has changed and sends a notifiction via NPA to RPM
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_npa_update_freq_plan
(
  uint32 freq_plan
);

/*=============================================================================

  FUNCTION:  ddr_coex_npa_init

=============================================================================*/
/*!
    @brief
    Registers with NPA for the resource available callback
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_npa_init(void);

#endif /* __DDR_COEX_NPA_H__ */
