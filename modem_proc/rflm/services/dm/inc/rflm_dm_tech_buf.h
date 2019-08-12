/*!
  @file
  rflm_dm_tech_buf.h

  @brief
  The header file include the type definition to the tech buffer.

  @detail
  OPTIONAL detailed description of this C header file.

*/

/*===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/dm/inc/rflm_dm_tech_buf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/14   jc      Force tech buf to be 64-bit aligned 
10/18/13   qwu     Changed char to uint32
7/19/13    jyw     initial check in
===========================================================================*/

#ifndef RFLM_DM_TECH_BUF_H
#define RFLM_DM_TECH_BUF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm_dm_api.h"
#include "rflm_dm_buf_mgr_t.h"

/*!@todo include the tech buf type header file here */


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef union
{
  /*!< point to the memory location */
  uint32 uint32_ptr[RFLM_TECH_BUF_SIZE_UINT32]; 

  /*!@todo Tech data type add below */
}rflm_dm_tech_buf_u;


/*! @brief tech buf struct; struct is not associated by the rflm handle */
typedef struct
{
  /*! buffer ptr is shared */
  rflm_dm_tech_buf_u tech_buf ALIGN(8);

  /*!< barrier */
  uint32 barrier_pattern;

  /*!< meta info; buffer usage description  */
  rflm_dm_tech_buf_meta_s meta ALIGN(8);

  /*!< allocate/deallocate caller history */
  uint32 caller_history[RFLM_DM_MAX_CALLER_HISTORY];

  /*!< allocate/deallocate caller history index*/
  atomic_word_t history_idx;

}rflm_dm_tech_buf_s;

#ifndef TEST_FRAMEWORK
/*lint -save -d__alignof__(x) -e24 */
COMPILE_ASSERT( __alignof__(rflm_dm_tech_buf_s) == 8);
/*lint -restore */
#endif

/*! @brief Tech buff pool class definition derived from the generic buffer model */
class TechBufPool: private MyBufMgrTemplate<rflm_dm_tech_buf_s, RFLM_TECH_BUF_POOL_SIZE>
{
public:
  TechBufPool(){init(); preset_buf();}
  void preset_buf(void);
  uint32* new_tech_buf(const rflm_dm_tech_buf_meta_s* input_meta_ptr);
  boolean delete_tech_buf(uint32* buf_ptr);
  boolean get_meta_info(uint32* buf_ptr, rflm_dm_tech_buf_meta_s* meta_ptr);
  boolean validate_buf_ptr(uint32* buf_ptr);
  int32  get_num_free_tech_buf(void);
  void update_caller_history(rflm_dm_tech_buf_s * tech_buf);
};

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* RFLM_DM_TECH_BUF_H */
