#ifndef _SPIDEVICEERROR_H_
#define _SPIDEVICEERROR_H_
/*
===========================================================================

FILE:   SpiDeviceError.h

DESCRIPTION:
    This file contains the result definitions 

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/api/spi/SpiDeviceError.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/11/12 DK     Added review comments. 
09/26/11 AG     Created

===========================================================================
        Copyright c 2011-2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/


typedef enum  ISpiDevice_Result
{
   SPIDEVICE_RES_SUCCESS =0,

  /* Error Classes */
   SPIDEVICE_RES_ERROR_CLS_SPI_CORE            = 0x01000000,

   SPIDEVICE_RES_ERROR_CLS_QUP_DEV_TIMEOUT     = 0x02000000,

   SPIDEVICE_RES_ERROR_CLS_QUP_DEV             = 0x03000000,

   /* Driver error class */
   SPIDEVICE_RES_ERROR_CLS_SPI_DRV             = 0x04000000,

   /* Some future classes */

   SPIDEVICE_RES_ERROR_CLS_DEV_PLATFORM       = 0x0E000000,
   SPIDEVICE_RES_ERROR_CLS_DEV_OS             = 0x0F000000,

} ISpiDevice_Result;

#endif /* _SPIDEVICEERROR_H_ */
