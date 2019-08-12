#ifndef __CVD_CAL_I_H__
#define __CVD_CAL_I_H__

/*
   Copyright (C) 2012, 2013 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/private/cvd_cal_i.h#1 $
   $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"
#include "apr_memmgr.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/* Max number of colunms is 5. */
#define CVD_CAL_KEY_COLUMN_SIZE_IN_BYTES ( 80 )

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

typedef union cvd_cal_na_value_t
{
  uint8_t uint8_val;
    /**< UINT8 N/A value. */
  uint16_t uint16_val;
    /**< UINT16 N/A value. */
  uint32_t uint32_val;
    /**< UINT32 N/A value. */
  uint64_t uint64_val;
    /**< UINT64 N/A value. */
}
cvd_cal_na_value_t;

typedef struct cvd_cal_column_descriptor_t cvd_cal_column_descriptor_t;

#include "apr_pack_begin.h"

struct cvd_cal_column_descriptor_t
{
  uint32_t id;
  uint32_t type;
  cvd_cal_na_value_t na_value;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_table_descriptor_t cvd_cal_table_descriptor_t;

#include "apr_pack_begin.h"

struct cvd_cal_table_descriptor_t
{
  void* start_ptr;
  uint32_t size;
  uint32_t data_mem_handle;
  uint32_t indexing_mem_handle;
  uint32_t num_columns;
  cvd_cal_column_descriptor_t* columns;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_column_t cvd_cal_column_t;

#include "apr_pack_begin.h"

struct cvd_cal_column_t
{
  uint32_t id;
  uint32_t value; /* Currently supported max column width is 32 bits. */
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_table_handle_t cvd_cal_table_handle_t;

#include "apr_pack_begin.h"

struct cvd_cal_table_handle_t
{
  cvd_cal_table_descriptor_t table_descriptor;

  uint8_t reorder_key_buf[ CVD_CAL_KEY_COLUMN_SIZE_IN_BYTES ];
  void* aux_info;

  uint32_t meta_size;
  bool_t is_parsed;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_key_t cvd_cal_key_t;

#include "apr_pack_begin.h"

struct cvd_cal_key_t
{
  uint32_t num_columns;
  cvd_cal_column_t* columns;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_entry_t cvd_cal_entry_t;

#include "apr_pack_begin.h"

struct cvd_cal_entry_t
{
  void* start_ptr;
  uint32_t size;
}
#include "apr_pack_end.h"
;


/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

/** 
  Initialize the CVD calibration utility. Must be called before any other 
  calibration utility APIs can be called, and must be called only once.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_init (
  void
);


/** 
  Deinitialize the calibration utility.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_deinit (
  void
);


/**
  Evaluate the amount of book-keeping memory required, given a calibration
  table.

  @param[in] cal_table_descriptor  Calibration table descriptor.
  @param[out] ret_size             Required indexing memory size in bytes 
                                   for book-keeping.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_eval_indexing_mem_size (
  cvd_cal_table_descriptor_t* cal_table_descriptor,
  uint32_t* ret_size
);


/**
  Parse a calibration table. If no book-keeping memory is provided (i.e.
  indexing_mem_addr == NULL), then linear look-up is performed. 

  @param[in] indexing_mem_addr  Indexing memory address.
  @param[in] indexing_mem_size  Indexing memory size in bytes.
  @param[in] table_descriptor   Calibration table descriptor.
  @param[out] ret_table_handle  CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_parse_table (
  void* indexing_mem_ptr,
  uint32_t indexing_mem_size,
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_table_handle_t** ret_table_handle
);


/**
  Query a parsed calibration table for a set of data entries matching the 
  provided key.

  @param[in] table_handle      CVD calibration table handle.
  @param[in] key               Query key.
  @param[in] entries_buf_size  User buffer size in bytes.
  @param[in/out] ret_entries   Pointer to user buffer.
  @param[out] ret_num_entries  Number of matched entries.

  @return APR_EOK when successful.
  @return APR_ENEEDMORE when user buffer is not bigger enough to store
                        all matched entries.
*/
APR_INTERNAL int32_t cvd_cal_query_table (
  cvd_cal_table_handle_t* table_handle,
  cvd_cal_key_t* key,
  uint32_t entries_buf_size,
  cvd_cal_entry_t* ret_entries,
  uint32_t* ret_num_entries
);


/**
  Discard a calibration table when it has been de-registered with CVD.
  Also discard the book-keeping memory region for this table.

  @param[in] table_hanlde  CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_discard_table (
  cvd_cal_table_handle_t* table_hanlde
);


/**
  Find the min and max value for a specified column.

  @param[in] table_handle    CVD calibration table handle.
  @param[in] column_id       Column ID.
  @param[out] ret_min_value  Min column value.
  @param[out] ret_max_value  Max column value.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_find_min_max_column_value (
  cvd_cal_table_handle_t* table_handle,
  uint32_t column_id,
  uint32_t* ret_min_value,
  uint32_t* ret_max_value
);


#endif /* __CVD_CAL_I_H__ */