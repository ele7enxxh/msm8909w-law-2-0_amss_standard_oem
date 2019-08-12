/*============================================================================

FILE:      DalChipInfoInfo.c

DESCRIPTION:
  This file registers the chip info DAL data structures to DAL.


============================================================================
        Copyright (c) 2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult
ChipInfo_DalChipInfo_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalChipInfo_DeviceId[1] = {DALDEVICEID_CHIPINFO};

const DALREG_DriverInfo
DALChipInfo_DriverInfo = { ChipInfo_DalChipInfo_Attach,
     					   1,
						   DalChipInfo_DeviceId
                         };


