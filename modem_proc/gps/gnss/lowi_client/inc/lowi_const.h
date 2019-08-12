#ifndef __LOWI_CONST_H__
#define __LOWI_CONST_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Const Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIConst

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#if APSS
#include <ctype.h>
#include <inttypes.h>
#include <time.h>
#include <new>
#include <stdint.h>
#include <base_util/vector.h>
#include <inc/lowi_defines.h>
#endif
namespace qc_loc_fw
{

#define BSSID_LEN 6
#define SSID_LEN 32
#define MAX_CHAN_INFO_SIZE 16
#define CLIENT_NAME_LEN 128
#define CELL_POWER_NOT_FOUND 0x7F

const char* const LOWI_CLIENT_SUFFIX = "-LOWIClient";
const char* const SERVER_NAME = "LOWI-SERVER";
const char* const NOT_AVAILABLE = "NA";
const int ASYNC_DISCOVERY_REQ_VALIDITY_PERIOD_SEC_MAX = 3600;
const int MAX_IFNAME_LEN = 32;


} // namespace qc_loc_fw
#endif //#ifndef __LOWI_CONST_H__
