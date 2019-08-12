#ifndef _SPIDEVICESYSTYPES_H_
#define _SPIDEVICESYSTYPES_H_
/*
===========================================================================

FILE:   SpiDeviceTypes.h

DESCRIPTION:
    This file contains the type definitions used in SPI driver.

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/spi/SpiDeviceTypes.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/30/13 sg      Added size_t.
06/11/12 ddk     Added review comments.
09/26/11 ag		 Created

===========================================================================
        Copyright c 2011-2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

/* The following definitions are the same accross platforms.  This first
** group are the sanctioned types.
*/


#ifndef _ARM_ASM_
#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;     /* Boolean value type. */
#define _BOOLEAN_DEFINED
#endif


#if defined(DALSTDDEF_H) /* guards against a known re-definer */
#define _BOOLEAN_DEFINED
#define _UINT32_DEFINED
#define _UINT16_DEFINED
#define _UINT8_DEFINED
#define _INT32_DEFINED
#define _INT16_DEFINED
#define _INT8_DEFINED
#define _UINT64_DEFINED
#define _INT64_DEFINED
#define _BYTE_DEFINED
#endif /* #if !defined(DALSTDDEF_H) */


#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

#ifndef _BYTE_DEFINED
typedef  unsigned char      byte;        /* byte type */
#define  _BYTE_DEFINED
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;		/* size_t type */
#endif


#ifndef TRUE
#define TRUE   1   /* Boolean true value. */
#endif

#ifndef FALSE
#define FALSE  0   /* Boolean false value. */
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef SPIDEVICE_UNREFERENCED_PARAMETER
#define SPIDEVICE_UNREFERENCED_PARAMETER(P)          \
    /*lint -save -e527 -e530 */ \
    { \
        (P) = (P); \
    } 
#endif

#endif /* _ARM_ASM_ */

typedef void* SPIDEVICE_BUFF_HANDLE;
typedef void* SPIDEVICE_TRANSFER_HANDLE;
typedef void* SPIDEVICE_PLATBAM_HANDLE;
typedef void* IBUSPLATCFG_HANDLE;

#endif /* _SPIDEVICESYSTYPES_H_*/

