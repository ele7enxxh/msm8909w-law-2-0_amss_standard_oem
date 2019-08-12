#ifndef QFE2320_TYPEDEF_H
#define QFE2320_TYPEDEF_H

/*!
  @file
  qfe2320_typedef.h

  @brief
  QFE2320 Data Type Definition header 

  @details
  This file contains the QFE2320 Data Type Definition RFFE data type 
  to support the interaction with the QUALCOMM QFE2320 chip

  @addtogroup QFE2320 Data Type Definition
  @{
*/

/*=============================================================================

Copyright (c) 2012,2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2320/api/qfe2320_typedef.h#1 $
  
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
}qfe2320_ccs_data_type;


typedef struct
{
uint8 bus_id;
uint8 assigned_usid;
uint8 chip_rev;
uint8 variant;
uint8 buffer_size;
}qfe2320_common_driver_settings_type;

#endif
