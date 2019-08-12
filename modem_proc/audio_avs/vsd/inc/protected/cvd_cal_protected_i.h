#ifndef __CVD_CAL_PROTECTED_I_H__
#define __CVD_CAL_PROTECTED_I_H__

/*
   Copyright (C) 2014 QUALCOMM Technologies Incorporated.
   All rights reserved.
   QUALCOMM Confidential and Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/protected/cvd_cal_protected_i.h#1 $
   $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_CAL_PARAM_MINOR_VERSION_0 ( 0 )

typedef struct cvd_cal_param_t cvd_cal_param_t;

#include "apr_pack_begin.h"

struct cvd_cal_param_t
{
  uint32_t minor_version; 
/**
  * supported versions:
  *   CVD_CAL_PARAM_VERSION_0
*/
  uint32_t module_id;
  uint32_t param_id;
  uint32_t param_data_size;
  void* param_data;
}
#include "apr_pack_end.h"
;

/**
  Calibration query callback function prototype.

  @param[in] cal_param: A calibration parameter to apply.
  @param[in] client_data: Pointer to a token/data supplied by the client 
                      through MVM_CMDID_CAL_QUERY .

*/
  typedef void ( *cvd_cal_query_cb_fn_t ) ( cvd_cal_param_t* cal_params,
                                            void* client_data );

#endif /* __CVD_CAL_PROTECTED_I_H__ */
