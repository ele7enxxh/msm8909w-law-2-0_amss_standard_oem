#ifndef TLE_API_H
#define TLE_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE Header for Stubs
GENERAL DESCRIPTION
   This file contains the minimal needed to compile while TLE is disabled


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/tle/stub/inc/tle_api.h#1 $
$DateTime: 2016/12/13 07:59:45 $

==============================================================================*/
#include "location_service_v02.h"
#include "gps_common.h"

typedef struct
{
  qmiLocInjectGtpClientDownloadedDataReqMsgT_v02 z_ClienDownloadedReq;
  int             l_Client_handle;
  uint32          q_TransactionId;
} tle_GtpClientDownloadedReqType;

#endif /* TLE_API_H */
