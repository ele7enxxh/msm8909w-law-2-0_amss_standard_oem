#ifndef __CVD_CAL_NONLINEAR_SEARCH_I_H__
#define __CVD_CAL_NONLINEAR_SEARCH_I_H__

/*
   Copyright (C) 2012, 2013 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/private/cvd_cal_nonlinear_search_i.h#1 $
   $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"


/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

uint32_t cvd_cal_nonlinear_search_eval_index_mem_size (
  uint32_t num_entries,
  uint32_t* ret_size
);

APR_INTERNAL int32_t cvd_cal_nonlinear_search_parse_table (
  void* indexing_mem_ptr,
  uint32_t indexing_mem_size,
  cvd_cal_table_handle_t* ret_table_handle
);

APR_INTERNAL int32_t cvd_cal_nonlinear_search_query (
  cvd_cal_table_handle_t* table_handle,
  cvd_cal_key_t* key,
  uint32_t entries_buf_size,
  cvd_cal_entry_t* ret_entries,
  uint32_t* ret_num_entries
);

#endif /* __CVD_CAL_NONLINEAR_SEARCH_I_H__ */
