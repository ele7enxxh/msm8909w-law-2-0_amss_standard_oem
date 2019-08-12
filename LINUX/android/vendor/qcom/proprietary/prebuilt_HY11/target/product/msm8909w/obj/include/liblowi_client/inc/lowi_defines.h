#ifndef __LOWI_DEFINES_H__
#define __LOWI_DEFINES_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Defines Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIDefines

Copyright (c) 2012 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include "comdef.h"

// This is here because for some reason strlcat and strlcpy
// are not available on Ubuntu but on Android
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

// Config file
#ifdef __ANDROID__
#define CONFIG_NAME "/etc/lowi.conf"
#else
#define CONFIG_NAME "lowi.conf"
#endif // #ifdef __ANDROID__

#endif //#ifndef __LOWI_DEFINES_H__
