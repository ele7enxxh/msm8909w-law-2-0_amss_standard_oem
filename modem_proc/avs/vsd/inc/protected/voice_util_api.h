#ifndef __VOICE_UTIL_API_H__
#define __VOICE_UTIL_API_H__

/*
   Copyright (C) 2014 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Qualcomm Technologies, Inc. Confidential and Proprietary.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/protected/voice_util_api.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"

/**
  Functions takes media_id and meta-data of media_id and copies the corresponding 
  homing vector to frame location. 
 
  @par Payload

  INPUT PARAM:
  @param[in] media_id : Supported media id is VOICEAPP_MEDIA_ID_AMR and VOICEAPP_MEDIA_ID_AMRWB. 
  @param[in] frame_info: media_id determines the frame_info, it contains meta-data for media_id.
  @param[in,out] frame : Location at which ( header + homing frame ) has to be copied. 
  @param[in,out] frame_size: Size of ( header + homing frame ) that was copied to location frame. 
  
  @return 
  APR_EOK on success.

  @dependencies
  none

*/
APR_INTERNAL uint32_t voice_util_get_homing_frame 
(
  uint32_t media_id,
  void* frame_info,
  uint8_t* frame,
  uint32_t* frame_size
);

#endif  /* __VOICE_UTIL_API_H__ */
