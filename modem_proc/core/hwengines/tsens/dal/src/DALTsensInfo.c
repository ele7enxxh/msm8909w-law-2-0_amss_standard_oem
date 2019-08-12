/*============================================================================
  FILE:         DALTsensInfo.c

  OVERVIEW:     Implementation of the Tsens DAL Info

  DEPENDENCIES: None
 
                Copyright (c) 2010, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/tsens/dal/src/DALTsensInfo.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-02-03  jjo  Ported to modem.

============================================================================*/
#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
Tsens_DalTsens_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalTsens_DeviceId[1] = {DALDEVICEID_TSENS};

const DALREG_DriverInfo DALTsens_DriverInfo = {
   Tsens_DalTsens_Attach,
   1,
   DalTsens_DeviceId};

