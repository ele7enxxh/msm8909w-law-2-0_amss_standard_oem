#ifndef __MVA_API_H__
#define __MVA_API_H__

/*
   Copyright (C) 2014 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Qualcomm Technologies, Inc. Confidential and Proprietary.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/private/mva_api.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"
#include "apr_timer.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

APR_INTERNAL uint32_t mva_call
(
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif  /* __MVA_API_H__ */
