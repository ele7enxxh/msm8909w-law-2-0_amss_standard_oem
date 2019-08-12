#ifndef __LOWI_DEFINES_H__
#define __LOWI_DEFINES_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Defines Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIDefines

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>

/* This header should contain types that are shared amongst QUIPC modules */
#define int8 int8_t
#define uint8  uint8_t
#define uint32 uint32_t
#define int32  int32_t
#define uint16 uint16_t
#define int16  int16_t
#define uint64 uint64_t
#define int64  int64_t
#define boolean uint8_t
#define byte uint8_t

typedef double DOUBLE;
typedef float FLOAT;
typedef long long INT64;
typedef unsigned long long UINT64;
typedef int INT32;
typedef unsigned int UINT32;
typedef short int INT16;
typedef unsigned short int UINT16;
typedef char INT8;
typedef unsigned char UINT8;
typedef bool BOOL;

// This is here because for some reason strlcat and strlcpy
// are not available on Ubuntu but on Android
#ifndef __ANDROID__
#define strlcpy strncpy
#define strlcat strncat
#endif

// Config file
#ifdef __ANDROID__
#define CONFIG_NAME "/etc/lowi.conf"
#else
#define CONFIG_NAME "lowi.conf"
#endif // #ifdef __ANDROID__

#endif //#ifndef __LOWI_DEFINES_H__
