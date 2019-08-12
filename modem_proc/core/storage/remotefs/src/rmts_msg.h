/***********************************************************************
 * rmts_msg.h
 *
 * RMTS Client MSG module
 * Copyright (C) 2014 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/remotefs/src/rmts_msg.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-07-22   dks   Create

===========================================================================*/

#ifndef __RMTS_MSG_H__
#define __RMTS_MSG_H__

#include "msg.h"

#define RMTS_MSG_ERR(fmt)         MSG(MSG_SSID_RMTS, MSG_LVL_ERROR, fmt)
#define RMTS_MSG_ERR1(fmt, ...)   MSG_1(MSG_SSID_RMTS, MSG_LVL_ERROR, fmt, __VA_ARGS__)
#define RMTS_MSG_ERR2(fmt, ...)   MSG_2(MSG_SSID_RMTS, MSG_LVL_ERROR, fmt, __VA_ARGS__)
#define RMTS_MSG_ERR3(fmt, ...)   MSG_3(MSG_SSID_RMTS, MSG_LVL_ERROR, fmt, __VA_ARGS__)

#define RMTS_MSG_HIGH(fmt)          MSG(MSG_SSID_RMTS, MSG_LVL_HIGH, fmt)
#define RMTS_MSG_HIGH1(fmt, ...)   MSG_1(MSG_SSID_RMTS, MSG_LVL_HIGH, fmt, __VA_ARGS__)
#define RMTS_MSG_HIGH2(fmt, ...)   MSG_2(MSG_SSID_RMTS, MSG_LVL_HIGH, fmt, __VA_ARGS__)
#define RMTS_MSG_HIGH3(fmt, ...)   MSG_3(MSG_SSID_RMTS, MSG_LVL_HIGH, fmt, __VA_ARGS__)

#define RMTS_MSG_MED(fmt)         MSG(MSG_SSID_RMTS, MSG_LVL_MED, fmt)
#define RMTS_MSG_MED1(fmt, ...)   MSG_1(MSG_SSID_RMTS, MSG_LVL_MED, fmt, __VA_ARGS__)
#define RMTS_MSG_MED2(fmt, ...)   MSG_2(MSG_SSID_RMTS, MSG_LVL_MED, fmt, __VA_ARGS__)
#define RMTS_MSG_MED3(fmt, ...)   MSG_3(MSG_SSID_RMTS, MSG_LVL_MED, fmt, __VA_ARGS__)

#define RMTS_MSG_LOW(fmt)         MSG(MSG_SSID_RMTS, MSG_LVL_LOW, fmt)
#define RMTS_MSG_LOW1(fmt, ...)   MSG_1(MSG_SSID_RMTS, MSG_LVL_LOW, fmt, __VA_ARGS__)
#define RMTS_MSG_LOW2(fmt, ...)   MSG_2(MSG_SSID_RMTS, MSG_LVL_LOW, fmt, __VA_ARGS__)
#define RMTS_MSG_LOW3(fmt, ...)   MSG_3(MSG_SSID_RMTS, MSG_LVL_LOW, fmt, __VA_ARGS__)

#endif /* not __RMTS_MSG_H__ */
