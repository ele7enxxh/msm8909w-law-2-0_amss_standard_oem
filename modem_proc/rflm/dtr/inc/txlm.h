#ifndef TXLM_H
#define TXLM_H

/*!
   @file
   txlm.h

   @brief
   This file implements the TXLM functionality.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/inc/txlm.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
10/31/14   dr      Remove the set_foundry_id method 
08/04/14   dr      Adding support for foundry dependent IREF gain programming 
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
04/24/14   ljl     cleanup dac_bringup api
02/13/14   st      DAC Temperature Compensation Support
11/08/13   st      Crit Sect FW Standalone support
11/06/13   st      LMEM Info Storage for DRIF Group A/B
10/04/13   st      Bolt DTR-TX New APIs
05/06/13   dej     Add critical section for LM
04/01/13   dej     Add dynamic settings tables api to TxLM
12/03/12   aca     DIME Dac cal
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
08/04/11   cd      Support for SW Dynamic Updates to LM buffer
                   Support for reading portions of LM buffer 
03/24/11   vrb     LM Intf changes for CMI Compliance
01/21/11   sr      code cleanup based on the code review.
01/12/11   bmg     Changed buffer types to lm_handle_type
01/04/11   bmg     Added TxLM destructor.
12/22/10   vrb     Updated Signature for rxlm_update_buffer_state
12/14/10   sr      Initial version.

============================================================================*/ 

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rflm.h"
#include "comdef.h"
#include "err.h"
#include "txlm_intf.h"
#include "lm_buffer.h"
#include "rflm_dtr.h"
#include "rflm_dtr_tx_lmem.h"
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
#include "rex.h"
#include "rfcommon_locks.h"
#endif
#include "rflm_rw_lock.h"
#include "rflm_dm_api.h"

class txlm
{
public:
  #if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  static rf_lock_data_type txlm_critical_section;
  #endif
  
  static txlm * get_instance(void);

  static dtr_verbose_level txlm_verbose_level;

  int32 device_id_to_shared_mem_map[TXLM_CHAIN_MAX];

  txlm(void);
  ~txlm();
  lm_status_type allocate_buffer(txlm_chain_type chain, lm_tech_type client_tech, 
                                     lm_tech_type operation_tech , lm_handle_type *txlm_handle); 
  lm_status_type deallocate_buffer(lm_handle_type txlm_handle, lm_tech_type client_tech, 
                                     lm_tech_type operation_tech);
  void map_device_id_to_shared_mem(int32 device);
  lm_status_type update_static_settings(lm_handle_type txlm_handle, int32 device, void *cfg);
  lm_status_type update_dynamic_settings(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr);
  lm_status_type get_buffer_content(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr);
  lm_status_type get_chain(lm_handle_type txlm_handle, txlm_chain_type* chain);
  lm_status_type update_dynamic_group_struct(uint32 dyn_functionality, uint32 group_id, uint8 *addr, uint32 table_index);
  lm_status_type set_up_dac_cal(void *txlm_dac_cal_cfg_var);
  lm_status_type dac_cal_cmd(uint32 dac_cal_cmd_enum, uint8 size, uint32 *data);
  lm_status_type execute_sequence(lm_handle_type handle, uint32 seq_id, uint32 *data);
  void set_lmem_info(txlm_chain_type chain, boolean drif_group_A, uint32* lmem_start_addr);
  void set_shared_mem_addr(uint32 mem0_groupA, uint32 mem1_groupA, uint32 mem0_groupB, uint32 mem1_groupB);
  lm_status_type retrieve_lmem_info(txlm_chain_type chain, uint32** groupA_drif_start_addr, uint32** groupB_drif_start_addr);
  lm_status_type get_shared_mem_addr(rflm_handle_tx_t handle_id, const rflm_dtr_tx_get_shared_mem_input_t* in, rflm_dtr_tx_get_shared_mem_output_t* out);
  lm_status_type get_shared_mem_addr(int32 device, const rflm_dtr_tx_get_shared_mem_input_t* in, rflm_dtr_tx_get_shared_mem_output_t* out);
  lm_status_type retrieve_dac_iref_lut (const uint8 *iref_lut, uint8 iref_lut_length);
  lm_status_type config_dac_iref(
                                      lm_handle_type handle,
                                      boolean update_en,
                                      uint32  iref_lut_idx,
                                      uint32 iref_update_start_time,
                                      uint32 iref_update_stop_time);
  
  lm_status_type retrieve_dac_therm_info(dac_therm_info_t* dac_therm_info);
  lm_status_type acquire_lock(lm_handle_type txlm_handle, rflm_rw_lock_t *rflm_dtr_handle_lock);
  lm_status_type release_lock(lm_handle_type txlm_handle, rflm_rw_lock_t *rflm_dtr_handle_lock);

  uint32 get_foundry_id(void); 
  
protected:

private:
  static txlm *txlm_ptr;
  lm_buffer *txlm_buff_ptr[TXLM_MAX_BUFFERS];
  static uint32* groupA_drif_lmem_addr[TXLM_CHAIN_MAX];
  static uint32* groupB_drif_lmem_addr[TXLM_CHAIN_MAX];

  static uint32* groupA_shared_mem_addr[TXLM_CHAIN_MAX];
  static uint32* groupB_shared_mem_addr[TXLM_CHAIN_MAX];

  uint32 msm_id;
  uint32 foundary_id;
  boolean dac_bringup_in_progress;

};

#endif /* TXLM_H */
