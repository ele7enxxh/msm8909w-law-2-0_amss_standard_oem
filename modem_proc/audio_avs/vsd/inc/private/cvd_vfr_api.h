#ifndef __CVD_VFR_API_H__
#define __CVD_VFR_API_H__

/*
   Copyright (C) 2014 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/private/cvd_vfr_api.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"
#include "drv_api.h"

APR_EXTERNAL int32_t cvd_vfr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif /* __CVD_VFR_API_H__ */

