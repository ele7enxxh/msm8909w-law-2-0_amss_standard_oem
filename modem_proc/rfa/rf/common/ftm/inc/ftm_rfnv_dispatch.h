/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            F T M  R F

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RFNV Dispatch commands coming 
  from the diag service 

Copyright (c) 2010             by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_rfnv_dispatch.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/28/13   ems     Added support for NV items larger than 4K  
07/28/10   aki     Added rfnv_item_code to write response
07/22/10   aki     Added fields cmd_data_len and cmd_rsp_pkt_size to rfnv header
07/01/10   aki     Merging RFNV FTM changes from dev/rfnv_dev 
05/05/10   aki     Initial version. 
===========================================================================*/

#ifndef FTM_RFNV_DISPATCH_H
#define FTM_RFNV_DISPATCH_H

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "comdef.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "rfcommon_nv.h"

#define RFNV_MAX_TRANSFER_SIZE 3800

// RFNV COMMON HEADER
typedef PACK(struct) {
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  word    cmd_data_len;      /* set to 0 for RFNV */
  word    cmd_rsp_pkt_size;  /* set to 0 for RFNV */
} ftm_rfnv_common_header_type;


//RFNV READ REQUEST
typedef PACK(struct) {
  PACK(struct) {
    ftm_rfnv_common_header_type cmn_header;
  } header;
  uint2   rfnv_item_code;
} ftm_rfnv_read_request_type;


//RFNV READ RESPONSE
typedef PACK(struct) {
  ftm_rfnv_common_header_type cmn_header;
  uint2   err_code;
  uint2   rfnv_item_code;
  uint2   rfnv_item_size;    
} ftm_rfnv_read_response_header_type;

typedef PACK(struct) {
   ftm_rfnv_read_response_header_type header;
   uint1   rfnv_item_data[RFNV_DATA_SIZE_MAX];
} ftm_rfnv_read_response_type;

//RFNV READ REQUEST V2
typedef PACK(struct) {
  PACK(struct) {
    ftm_rfnv_common_header_type cmn_header;
  } header;
  uint2            rfnv_item_code;
  unsigned long    rfnv_offset;
} ftm_rfnv_read_request_v2_type;

//RFNV READ RESPONSE V2
typedef PACK(struct) {
   ftm_rfnv_read_response_header_type header;
   unsigned long    rfnv_bytes_remaining;
   uint1            rfnv_item_data[RFNV_DATA_SIZE_MAX];
} ftm_rfnv_read_response_v2_type;

// RFNV WRITE REQUEST
typedef PACK(struct) {
  ftm_rfnv_common_header_type cmn_header;
  uint2   rfnv_item_code;
  uint2   rfnv_item_size;
} ftm_rfnv_write_request_header_type;

typedef PACK(struct) {
   ftm_rfnv_write_request_header_type header;
   uint1   rfnv_item_data[RFNV_DATA_SIZE_MAX];
} ftm_rfnv_write_request_type;


// RFNV WRITE RESPONSE
typedef PACK(struct) {
  PACK(struct) {
    ftm_rfnv_common_header_type cmn_header;
  } header;  
  uint2   err_code;
  uint2   rfnv_item_code;
} ftm_rfnv_write_response_type;

// RFNV WRITE REQUEST V2
typedef PACK(struct) {
   ftm_rfnv_write_request_header_type header;
   uint1   rfnv_append_data;
   uint1   rfnv_item_data[RFNV_DATA_SIZE_MAX];
} ftm_rfnv_write_request_v2_type;


// RFNV WRITE RESPONSE V2
typedef PACK(struct) {
  PACK(struct) {
    ftm_rfnv_common_header_type cmn_header;
  } header;  
  uint2   err_code;
  uint2   rfnv_item_code;
} ftm_rfnv_write_response_v2_type;

// RFNV GET MAX TRANSFER SIZE REQUEST
typedef PACK(struct) {
   ftm_rfnv_write_request_header_type header;
} ftm_rfnv_get_max_transfer_size_request_type;

// RFNV GET MAX TRANSFER SIZE RESPONSE
typedef PACK(struct) {
  PACK(struct) {
    ftm_rfnv_common_header_type cmn_header;
  } header;  
  uint2   err_code;
  uint2   rfnv_max_transfer_size;
} ftm_rfnv_get_max_transfer_size_response_type;

//PROTOTYPES
ftm_rsp_pkt_type ftm_rfnv_dispatch(ftm_rfnv_common_header_type *ftm_rfnv_cmd);

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_RFNV_DISPATCH_H */
