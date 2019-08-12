/*
===========================================================================

FILE:   DALSPIInfo.C

DESCRIPTION:
    This file implements the common SPI DeviceDriver Info.

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/driver/DalSpiInfo.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/06/12 ddk    Added dynamic device context initialisation requirement.
04/13/12 ddk	Added the device numbers for 8974.
10/19/11 ag 	Include DalSpiTarget.h to get DALSPI_NUM_DEVICES define.
07/15/10 ag 	Created for 86660 SPI.

===========================================================================
        Copyright c 2011-2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "DALReg.h"
#include "DALDeviceId.h"

/* Maximum no: of devices supported */
#define SPI_MAX_DEVICES 12

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
Spi_DalSpi_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalSpi_DeviceId[SPI_MAX_DEVICES] =
{
  DALDEVICEID_SPI_DEVICE_1,
  DALDEVICEID_SPI_DEVICE_2,
  DALDEVICEID_SPI_DEVICE_3,
  DALDEVICEID_SPI_DEVICE_4,
  DALDEVICEID_SPI_DEVICE_5,
  DALDEVICEID_SPI_DEVICE_6,
  DALDEVICEID_SPI_DEVICE_7,
  DALDEVICEID_SPI_DEVICE_8,
  DALDEVICEID_SPI_DEVICE_9,
  DALDEVICEID_SPI_DEVICE_10,
  DALDEVICEID_SPI_DEVICE_11,
  DALDEVICEID_SPI_DEVICE_12,
};
const DALREG_DriverInfo
DALSpi_DriverInfo = { Spi_DalSpi_Attach,
                      SPI_MAX_DEVICES,
                      DalSpi_DeviceId
                    };
