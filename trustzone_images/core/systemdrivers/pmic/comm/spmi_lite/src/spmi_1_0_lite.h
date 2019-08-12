#ifndef SPMI_1_0_LITE_H
#define SPMI_1_0_LITE_H

/*! \file
 *  \n
 *  \brief  SPMI_1_0_lite.h 
 *  \details  
 *  \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/comm/spmi_lite/src/spmi_1_0_lite.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/12   hs      Created
========================================================================== */

/*! \file 
*  \n
*  \brief  SPMI_1_0_lite.h ---- 
*  \details This header file contains declarations of
*    Version 2.0 of the light version of SPMI 2.0 HW communictions interface
*  \n
*  \n &copy; 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

#include "pm_qc_pmic.h"

pm_err_flag_type pm_spmi_lite_read_byte
(
  uint32 uSlaveId , 
  uint16 addr, 
  uint8* data, 
  uint8 priority
);

pm_err_flag_type pm_spmi_lite_write_byte
(
  uint32 uSlaveId ,
  uint16 addr , 
  uint8  data, 
  uint8 priority
);

pm_err_flag_type  pm_spmi_lite_write_byte_mask
(
  uint32 uSlaveId , 
  uint16 addr, 
  uint8 mask, 
  uint8 value, 
  uint8 priority
);

pm_err_flag_type  pm_spmi_lite_read_byte_array 
( 
  uint32 uSlaveId ,
  uint16 addr , 
  uint32 numBYtes , 
  uint8* data, 
  uint8 priority
) ;

#endif // SPMI_1_0_LITE_H

