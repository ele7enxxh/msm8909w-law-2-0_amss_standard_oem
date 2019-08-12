/*!
  @file
  rflm_dm_script_buf.h

  @brief
  The header file contains the type definition of the script buffer.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/dm/inc/rflm_dm_script_buf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/13   qwu     Added mulple script buffers
09/30/13   sty     revert to 50 since it causes compile errors due to memory 
                   overflow
09/27/13   sty     Increased buf number to 250 - need the "small" scripts for 
                   pre-compilation of LNA issue seqs
08/15/13   hdz     Increased buf number to 50
07/26/13   jyw     Created the initial version.
===========================================================================*/

#ifndef RFLM_DM_SCRIPT_BUF_H
#define RFLM_DM_SCRIPT_BUF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm_dm_api.h"
#include "rflm_dm_buf_mgr_t.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/


template<uint32 SBUF_SZ_IN_BYTE>
struct rflm_dm_script_buf_s
{
  // FIXED location for pointer reference
  rflm_dm_script_buf_params_s buf_params;
  /*!< point to the 1KB memory location could either inside or outside of the FED */
  uint32 payload_ptr[SBUF_SZ_IN_BYTE];

  /*!< barrier */
  uint32 barrier_pattern;
};


/*! @brief ScriptBuf class is based on the BufMgrTemplate */
template<class SBUF, int32 SBUF_DEPTH>
class ScriptBuf: public MyBufMgrTemplate<SBUF, SBUF_DEPTH>

{
public:
  ScriptBuf();

  void preset_buf(void);
  uint32 * get_new_buf(rflm_dm_script_meta_s* input_meta_ptr);
  boolean set_script_cb(uint32 * buf_ptr, script_exe_callback_func_t cb_ptr);
  boolean delete_buf(uint32* buf_ptr);
  boolean get_meta_info(uint32 * buf_ptr, rflm_dm_script_meta_s* meta_ptr);
  boolean validate_buf_ptr(uint32 * buf_ptr);
  uint32 get_num_free_script_buf(void);

  rflm_dm_buff_size_e script_buf_depth;
};


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* RFLM_DM_SCRIPT_BUF_H */
