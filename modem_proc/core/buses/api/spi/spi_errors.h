#ifndef _SPI_ERRORS_H_
#define _SPI_ERRORS_H_
/*
===========================================================================

FILE:   spi_errors.h

DESCRIPTION:
    This file contains the result definitions 

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/spi/spi_errors.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/03/15 DDK    Added change in success enum type	
09/26/11 AG     Created

===========================================================================
        Copyright c 2011-2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

typedef enum
{
   SPI_OP_SUCCESS =0,
   SPI_ERROR_CLS_DEVICE     = 0x01000000,
   SPI_ERROR_CLS_DRIVER     = 0x04000000,
   SPI_ERROR_CLS_PLATFORM   = 0x0E000000,
   SPI_ERROR_CLS_OS         = 0x0F000000,
} SPI_RESULT;

#endif /* _SPI_ERRORS_H_*/
