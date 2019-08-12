#ifndef QFE_CMN_INTF_H
#define QFE_CMN_INTF_H

/*!
  @file
  rfdevice_cmn_intf.h

  @brief
  This file provides common interface functions to configure the RTR device.

  
*/

/*===========================================================================

Copyright (c) 2010, 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/qfe_intf/inc/qfe_cmn_intf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/12   kai     Removed QFE1510 support
09/26/12   kai     Changed name of K sensor comp apis to ant common api
09/26/12   kai     Add QFE1510 HDET and K val comp support for 1x
08/02/12   gh      Add support for k sensor comp and cleanup
08/02/12   kai     Add QFE1510 HDET and K val comp support for W online mode
07/10/12   kai     Add support for Qtuner FTM command
06/26/12   gh      Add HDET gain states
06/21/12   vb      Added support for QFE1510
12/17/11   kg      Using "qfe_intf_cmd.h" 
09/13/11   kg      Initial version
============================================================================*/

#include "rfcom.h"
#include "rfdevice_type_defs.h"
#include "qfe_intf_cmd.h"
#include "rfdevice_cmn_intf.h"

typedef struct qfecmn_device qfe_cmn_type;

/*================================================ 
      Data types related to QFE QPA
 ================================================*/

typedef int32 (*qfe_cmn_cmd_dispatch_type)
(
 qfe_cmn_type*,
 qfe_cmd_enum_type, 
 void*
); 


struct qfecmn_device
{
  const qfe_enum_type device_id;
  qfe_cmn_cmd_dispatch_type cmd_dispatch_fp;
  void* data; /*!< should be void* "const" data for robustness */
};


void qfe_cmn_cmd_dispatch
(
  rf_path_enum_type path,
  qfe_cmd_enum_type cmd,
  void *data
);
/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfdevice_cmn : provides common interface
  
*/
extern qfe_cmn_type *qfe_cmn;  /* <- qfe_cmn needs to be deleted and only qfe_cmn_device_type needs to be used*/

#endif /* RFDEVICE_CMN_INTF_H */

