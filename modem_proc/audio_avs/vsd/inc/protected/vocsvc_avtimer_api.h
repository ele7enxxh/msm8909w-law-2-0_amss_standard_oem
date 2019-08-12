#ifndef __VOCSVC_AVTIMER_API_H__
#define __VOCSVC_AVTIMER_API_H__

/*
  Copyright (C) 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/protected/vocsvc_avtimer_api.h#1 $
  $Author: mplcsds1 $
*/

#include "apr_comdef.h"

typedef struct vocsvc_avtimer_timestamp_t
{
  uint64_t timestamp_us;                
}
  vocsvc_avtimer_timestamp_t;

APR_EXTERNAL int32_t vocsvc_avtimer_open_handle
(
  void
);

APR_EXTERNAL int32_t vocsvc_avtimer_close_handle
(
  void
);

APR_EXTERNAL int32_t vocsvc_avtimer_get_time
(
  vocsvc_avtimer_timestamp_t* avtimer_timestamp
);

#endif  /* __VOCSVC_AVTIMER_API_H__ */

