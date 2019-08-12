#ifndef QFE2340V3P0_TYPEDEF_H
#define QFE2340V3P0_TYPEDEF_H

/*!
  @file
  qfe2340v3p0_typedef.h

  @brief
  QFE2340 Data Type Definition header

  @details
  This file contains the QFE2340 Data Type Definition RFFE data type
  to support the interaction with the QUALCOMM QFE2340 chip

  @addtogroup QFE2340 Data Type Definition
  @{
*/

/*=============================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2340/api/qfe2340v3p0_typedef.h#1 $

=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"

typedef struct
{
  uint8 reg;
  uint8 chip_rev;
  uint8 variant;
}qfe2340v3p0_ccs_data_type;


typedef struct
{
uint8 bus_id;
uint8 assigned_usid;
uint8 chip_rev;
uint8 variant;
uint8 buffer_size;
}qfe2340v3p0_common_driver_settings_type;

#endif
