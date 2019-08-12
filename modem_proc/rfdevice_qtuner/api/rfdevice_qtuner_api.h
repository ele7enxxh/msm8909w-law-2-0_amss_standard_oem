#ifndef RFDEVICE_QTUNER_API_H
#define RFDEVICE_QTUNER_API_H

/*=============================================================================

Copyright (c) 2013,2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. 
  Government. Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qtuner/api/rfdevice_qtuner_api.h#1 $
  $DateTime: 2016/12/13 07:58:06 $
  $Author: mplcsds1 $
 
when         who   what, where, why
--------   ---   --------------------------------------------------------------- 
08/21/14   vv     Clean up unused device creation APIs()
07/01/14   yb     Add support to make tuner transactions non-extended via rfc
05/27/14   vv     Physical device support
04/09/14  ndb     Added extended_cmd in create_common_qtuner()
03/25/14   yb     Added support for linearizer
03/11/14   yb     Added enum for intermodulation distortion
12/13/13   ndb    Initial version
 
=============================================================================*/
  
/*=============================================================================

                            INCLUDE FILES FOR MODULE
                            

=============================================================================*/
#include "rfc_common.h"
#include "rfdevice_antenna_tuner.h"
#ifdef __cplusplus
#include "rfdevice_physical_device.h"
#endif
/*! @brief Intermodulation Distortion type for Linearizer settings

*/
typedef enum
{
  RFDEVICE_QTUNER_IM_DISTORTION_DEFAULT = 1,
  RFDEVICE_QTUNER_IM_DISTORTION_IP3,
  RFDEVICE_QTUNER_IM_DISTORTION_HD3,
  RFDEVICE_QTUNER_IM_DISTORTION_IHD3,
  RFDEVICE_QTUNER_IM_DISTORTION_IP2,
  RFDEVICE_QTUNER_IM_DISTORTION_HD2,
  RFDEVICE_QTUNER_IM_DISTORTION_IHD2,
  RFDEVICE_QTUNER_IM_DISTORTION_COMMON,
  RFDEVICE_QTUNER_IM_DISTORTION_MAX,
  RFDEVICE_QTUNER_IM_DISTORTION_INVALID = 0xFF
}rfdevice_qtuner_distortion_config_type;

#ifdef __cplusplus

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function for creating the common driver qtuner object in Bolt & DPM2.0

  @details
  The function is used to create the Common Qtuner driver for the passed configuration.

  @param phy_obj_ptr
  Parent physical device object pointer
 
  @param phy_device_info
  physical device config info
 
  @param logical_device_info
  logical device config info
 
  @param ext_cmd
  extended vs. non extended transactions with ccs

  @rfdevice_antenna_tuner
  ptr to the common qtuner driver
*/
rfdevice_antenna_tuner* phy_create_common_qtuner 
(
  rfdevice_physical_device* phy_obj_ptr,
  rfc_phy_device_info_type *phy_device_info, 
  rfc_logical_device_info_type *logical_device_info,
  boolean ext_cmd
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function for  performing R tuner cal on tuner.

  @details
  perform_r_tuner_cal_qtuner()

  @param r_tuner_val
  pointer to R Tune code array.

  @num
  num of R tune codes  that needs to be generated.

  @return
  boolean indicating pass (TRUE) or fail (FALSE)	  
 
*/
boolean perform_r_tuner_cal_qtuner
(
  rfdevice_antenna_tuner* qtuner_obj,
  uint8* r_tuner_val,
  uint8 num
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function for  performing C tuner cal on tuner.

  @details
  perform_c_tuner_cal_qtuner()

  @param c_tuner_idx
  pointer to C Tune code array.

  @num
  num of C tune codes  that needs to be generated.

  @return
  boolean indicating pass (TRUE) or fail (FALSE)	  
 
*/
boolean perform_c_tuner_cal_qtuner 
(
  rfdevice_antenna_tuner* qtuner_obj,
  uint8* c_tuner_idx,
  uint8 num
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function for  Loading R tuner cal  data on tuner.

  @details
  load_r_tuner_cal_qtuner()

  @param r_tuner_val
  pointer to R Tune code array.

  @num
  num of R tune codes  that needs to be generated.

  @return
  boolean indicating pass (TRUE) or fail (FALSE)	  
 
*/
boolean load_r_tuner_cal_qtuner 
(
  rfdevice_antenna_tuner* qtuner_obj,
  uint8* r_tuner_val,
  uint8 num
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function for  Loading C tuner cal  data on tuner.

  @details
  load_c_tuner_cal_qtuner()

  @param c_tuner_idx
  pointer to C Tune code array.

  @num
  num of C tune codes  that needs to be generated.

  @return
  boolean indicating pass (TRUE) or fail (FALSE)	  
 
*/
boolean load_c_tuner_cal_qtuner
(
  rfdevice_antenna_tuner* qtuner_obj,
  uint8* c_tuner_idx,
  uint8 num
);
#endif

#endif/*RFDEVICE_QTUNER_API_H*/