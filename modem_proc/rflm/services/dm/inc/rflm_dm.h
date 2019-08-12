/*!
  @file
  rflm_dm.h

  @brief
  The header file include data types defined within the RFLM DM. all C or CC file
  is expected to only include this file.

  @detail
  OPTIONAL detailed description of this C header file.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/dm/inc/rflm_dm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/13   qwu     Added mulple script buffers
07/26/13   jyw     Created the file
===========================================================================*/

#ifndef RFLM_DM_H
#define RFLM_DM_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm.h"
#include "rflm_dm_handle.h"
#include "rflm_dm_script_buf.h"
#include "rflm_dm_handle_tech_data.h"
#include "rflm_dm_tech_buf.h"
#include "rflm_dm_dtr_buf.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief DM consolidated structure type
*/

typedef struct
{
  HandlePool   handle_pool;

  ScriptBuf<rflm_dm_script_buf_s<RFLM_DM_SBUF_2HB_UINT32_SIZE>, RFLM_SCRIPT_BUF_2HB_COUNTS> script_buf_2h;
  ScriptBuf<rflm_dm_script_buf_s<RFLM_DM_SBUF_5HB_UINT32_SIZE>, RFLM_SCRIPT_BUF_5HB_COUNTS> script_buf_5h;
  ScriptBuf<rflm_dm_script_buf_s<RFLM_DM_SBUF_1KB_UINT32_SIZE>, RFLM_SCRIPT_BUF_1KB_COUNTS> script_buf_1k;
  ScriptBuf<rflm_dm_script_buf_s<RFLM_DM_SBUF_2KB_UINT32_SIZE>, RFLM_SCRIPT_BUF_2KB_COUNTS> script_buf_2k;
  ScriptBuf<rflm_dm_script_buf_s<RFLM_DM_SBUF_3KB_UINT32_SIZE>, RFLM_SCRIPT_BUF_3KB_COUNTS> script_buf_3k;

  TechDataPool handle_tech_data_pool;
  TechBufPool  tech_buf;
  DtrRxBuf     dtr_rx_pool;
  DtrTxBuf     dtr_tx_pool;
} rflm_dm_s;

extern rflm_dm_s rflm_dm;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
#endif /* RFLM_DM_H */
