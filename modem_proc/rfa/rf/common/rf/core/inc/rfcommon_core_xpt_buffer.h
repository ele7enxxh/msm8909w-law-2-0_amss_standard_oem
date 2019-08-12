#ifndef RFCOMMON_CORE_XPT_BUFFER_H
#define RFCOMMON_CORE_XPT_BUFFER_H

/*! @brief!
   @file
   rfcommon_core_xpt_buffer.h

   @brief
   Common buffer pool for storing xPT capture data

   @details
    

*/

/*! @brief===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_xpt_buffer.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/28/14   kai     Add API getting the max number of buffers used
12/06/12   jmf     Fixed API name
09/17/12   cri     Initial check-in
10/30/12   cri     Updated for SM storage of DPD capture and results

============================================================================*/

#include "comdef.h"
#include "fw_rf_common_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT 12
/* #define RF_COMMON_XPT_NUM_CONT_SAMPLE_CAPT FW_XPT_NUM_CONC_SAMPLE_CAPT */

/* Structure Definitions */
typedef enum
{
  BUFFER_UNALLOCATED,
  BUFFER_FREE,
  BUFFER_IN_USE,
}xpt_buffer_status_type;

typedef struct
{
  uint8 capt_buf_id;
  uint8 res_buf_id;
}rf_common_xpt_buffer_id_type;

/* Function Declarations */
void rfcommon_core_xpt_init_buffers(void);
boolean rfcommon_core_xpt_get_buffers(rf_common_xpt_buffer_id_type* buffer_ids);
boolean rfcommon_core_xpt_release_capture_buffer(uint8 buffer_id);
boolean rfcommon_core_xpt_release_results_buffer(uint8 buffer_id);
uint32 *rfcommon_core_xpt_get_results_buffer(uint8 buffer_id);
void rfcommon_core_xpt_free_all_buffers(void);
uint8 rfcommon_core_xpt_get_max_buffers_used(void);

#ifdef __cplusplus
}
#endif
#endif /*! @brief RFCOMMON_CORE_XPT_BUFFER_H */
