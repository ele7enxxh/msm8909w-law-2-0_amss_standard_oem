#ifndef __SPIDRIVERTYPES_H__
#define __SPIDRIVERTYPES_H__

/**
@file SpiDriverTypes.h
*/

/*===========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/driver/SpiDriverTypes.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who      what, where, why
----------   ---      ------------------------------------
10/07/15     ddk      Addition of enum for null handle.
09/03/15     ddk      Addition of enums
05/06/2014   vmk      Created

===========================================================================*/
#include "comdef.h"
#include "SpiDevice.h"
#include "DalDevice.h"
#include "spi_errors.h"

#define MAX_SUPPORTED_SPI_INSTANCES 6

#define SPI_DEVICE_INIT_STATE              0x00000001
#define SPI_DEVICE_OPEN_STATE             0x00000004



enum spi_driver_errors 
{
   SPI_ERROR_INVALID_PARAM = SPI_ERROR_CLS_DRIVER,
   SPI_ERROR_DEVICE_UNAVAILABLE, //core is not meant for this core/pd/board
   SPI_ERROR_DEVICE_IN_USE, // 1 or more clients have opened this device, close first
   SPI_ERROR_DEVICE_NOT_OPEN,
   SPI_ERROR_INVALID_MODE, //invalid power mode (e.g. this function can only be executed in big img mode)
   SPI_ERROR_DAL_ATTACH_FAILURE,
   SPI_ERROR_DAL_DETACH_FAILURE,
   SPI_ERROR_DAL_OPEN_FAILURE,
   SPI_ERROR_DAL_CLOSE_FAILURE,
   SPI_ERROR_DAL_CONFIGURE_FAILURE,
   SPI_ERROR_READ_FAILURE,
   SPI_ERROR_WRITE_FAILURE,
   SPI_ERROR_FULL_DUPLEX_FAILURE,
   SPI_ERROR_NULL_HANDLE,
   SPI_ERROR_UNKNOWN 
};

typedef struct spi_dev_info_s{

DalDeviceHandle *phDevice;
uint32 status;

}spi_dev_info_s;


#endif //multiple inclusion guard
