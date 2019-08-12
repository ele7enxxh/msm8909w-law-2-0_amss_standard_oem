/*!
   @file
   test_rfcommon_core_device_manager.h

   @brief
   This file contains function prototypes for off-targer testing for
   RF Common Core Device Manager for the RF Device[s].
*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/test_rfcommon_core_device_manager.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
01/16/12   pv      RF Common Device Manager Implementation. 
01/16/12   pv      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfcommon_core_device_manager.h"

/*!
  @brief
  Interface API to retreive Device Manager GLOBAL Structure.
*/

/*----------------------------------------------------------------------------*/

/* Interface API for RF Common Core Device Manager off-target testing */
rfcmn_core_mgr_type* rfcommon_core_get_core_mgr_data(void);

