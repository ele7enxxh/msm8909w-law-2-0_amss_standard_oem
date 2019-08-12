#ifndef RXLM_H
#define RXLM_H

/*!
   @file
   rxlm.h

   @brief
   This file implements the RXLM functionality.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2010 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/inc/rxlm.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
11/08/13    st      Crit Sect FW Standalone support
07/08/13   dej     Added APIs for common notch allocation\deallocation
05/06/13   dej     Add critical section for LM
11/19/12   dej     Implement API to copy settings from SW Dynamic tables into group structs 
11/16/11   vrb     Added new API rxlm_get_chain to get rxlm_chain from buf_idx
04/21/11   vrb     Support for Reading portions of LM Buffer
04/07/11   vrb     Support for SW Dynamic Updates to LM Buffer
03/24/11   vrb     LM Intf changes for CMI Compliance
02/08/11   sty     Added C++ protection
01/21/11   sr      code cleanup based on the code review.
01/12/11   bmg     Changed buffer types to lm_handle_type
01/03/11   bmg     Added RxLM destructor declaration
12/28/10   sr      Added txlm functionality and changed the rxlm to use 
                   common lm_buffer.
12/22/10   vrb     Updated Signature for rxlm_update_buffer_state
12/14/10   sr      Initial version.

============================================================================*/ 

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rflm.h"
#include "comdef.h"
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
#include "rex.h"
#include "rfcommon_locks.h"
#endif
#include "rxlm_intf.h"
#include "lm_buffer.h"
#include "rflm_dtr.h"
#include "rflm_rw_lock.h"


#ifdef __cplusplus
extern "C" {
#endif

class rxlm
{
public:
  #if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  static rf_lock_data_type rxlm_critical_section;
  #endif

  static dtr_verbose_level rxlm_verbose_level;

  static rxlm * get_instance(void);

  rxlm(void);
  ~rxlm();
  lm_status_type allocate_buffer(rxlm_chain_type chain, lm_tech_type client_tech, 
                                     lm_tech_type operation_tech, lm_handle_type *rxlm_handle);
  lm_status_type deallocate_buffer(lm_handle_type rxlm_handle, lm_tech_type client_tech, 
                                     lm_tech_type operation_tech);
  lm_status_type update_static_settings(lm_handle_type rxlm_handle, void *cfg);
  lm_status_type update_dynamic_settings(lm_handle_type rxlm_handle, uint32 group_id, uint8 *addr);
  lm_status_type get_buffer_content(lm_handle_type rxlm_handle, uint32 group_id, uint8 *addr);
  rxlm_chain_type get_chain(lm_handle_type rxlm_handle);
  lm_status_type update_dynamic_group_struct(uint32 dyn_functionality, uint32 group_id, uint8 *addr, uint32 table_index);
  lm_status_type acquire_lock(lm_handle_type rxlm_handle, rflm_rw_lock_t *rflm_dtr_handle_lock);
  lm_status_type release_lock(lm_handle_type rxlm_handle, rflm_rw_lock_t *rflm_dtr_handle_lock);
#ifndef FEATURE_TRITON_MODEM
  lm_status_type allocate_common_notch(lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32* cmn_notch_dyn_block_id);
  lm_status_type deallocate_common_notch(lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32 cmn_notch_dyn_block_id);
#endif

protected:

private:
  static rxlm *rxlm_ptr;
  lm_buffer *rxlm_buff_ptr[RXLM_MAX_BUFFERS];

};


#ifdef __cplusplus
}
#endif

#endif /* RXLM_H */
