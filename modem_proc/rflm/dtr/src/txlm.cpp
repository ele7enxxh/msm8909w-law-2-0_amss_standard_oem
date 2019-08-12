/*!
   @file
   txlm.cpp

   @brief
   This file implements the txlm functionality.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2010 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/src/txlm.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
12/22/15   sbo     Fix KW
10/31/14   dr      Move DAC foundry id retrieval to TxLM object layer and remove the set_foundry_id method
08/05/14   aw      fixed device_id typo
08/04/14   dr      Adding support for foundry dependent IREF gain programming 
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
04/24/14   ljl     cleanup dac_bringup api
03/04/14   kai     Initialize dac_bringup_in_progress
02/13/14   st      DAC Temperature Compensation Support
12/19/13   dej     Add valid handle check for allocation
12/04/13   vrb     Convert some diag messages to Trace logging for timeline
11/21/13   cvd     Add Logging messages
11/11/13   jyu     Added enum support for TDSCDMA
11/08/13   st      Crit Sect FW Standalone support
11/06/13   st      LMEM Info Storage for DRIF Group A/B
10/07/13   cvd     Add checks for handle type
10/29/13   st      Reintroduce DAC Powerup to Bootup
10/25/13   st      Remove DAC Powerup from Bootup
10/04/13   st      Bolt DTR-TX New APIs
09/26/13   JJ      Remove RFLM_USR_C2K
08/22/13   dej     Integrate with RFLM: DM
05/06/13   dej     Add critical section for LM
04/01/13   dej     Add dynamic settings tables api to TxLM
01/16/12   aca     Featurization update for dime vs triton
01/14/12   aca     DIME Dac cal- migrated to AG functions
12/03/12   aca     DIME Dac cal
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
08/04/11   cd      Support for SW dynamic updates to LM buffer
                   Support for reading portions of the TxLM buffer 
05/02/11   vrb     Compiler Warnings Fix
01/21/11   sr      code cleanup based on the code review.
01/12/11   bmg     Changed buffer types to lm_handle_type
01/04/11   bmg     Added TxLM destructor to free buffer memory.
12/22/10   vrb     Updated Signature for txlm_update_buffer_state
12/14/10   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "txlm.h"
#include "rflm_dtr.h"

extern "C" {
#include "txlm_hal.h"
#include "txlm_dac_cal_ag.h"
}

#include "rflm_dm_api.h"
#ifndef FEATURE_D3925_FED
#include "rflm_dtr_tx_struct_ag.h"
#else
#include "fw_txlm_intf.h"
#endif
#include <stringl/stringl.h>

/* To be removed or updated based on getting tech from RFLM ; 
Require API to get tech from RFLM */
static lm_tech_type lm_op_tech_of_rflm_handle[RFLM_HANDLE_COUNTS];
static lm_tech_type lm_client_tech_of_rflm_handle[RFLM_HANDLE_COUNTS];

txlm *txlm::txlm_ptr = (txlm *)NULL;

dtr_verbose_level txlm::txlm_verbose_level = DTR_VERBOSE_LEVEL_1;

#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
rf_lock_data_type txlm::txlm_critical_section;// = (rf_lock_data_type)0ULL;
#endif

uint32* txlm::groupA_drif_lmem_addr[] = {NULL,NULL};
uint32* txlm::groupB_drif_lmem_addr[] = {NULL,NULL};

uint32* txlm::groupA_shared_mem_addr[] = {NULL,NULL};
uint32* txlm::groupB_shared_mem_addr[] = {NULL,NULL};


/*----------------------------------------------------------------------------*/
/*!
   @brief
      Provides the txlm object pointer.
 
   @details
      returns the txlm object ptr, with which clients can communicate with txlm
      to allocate/deallocate and update the txlm buffers.

   @return txlm singleton object ptr.
   
*/
txlm * txlm::get_instance(void)
{
  if (!txlm_ptr)
  {
    txlm_ptr = new txlm();
  }

  return txlm_ptr;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      txlm constructor.
 
   @details
      constructs the txlm object. Gets the rxf configuration information like
      number of rxf chains, buffers per each chain, address of each buffer and
      the size of each buffer from the rxf hal. Based on the rxf information the
      txlm buffers will be created.
*/
txlm::txlm()
{
  uint8 buffer_id = 0;
  uint8 i = 0;
  uint8 chain = 0;
  uint8 max_buffers  = 0;

  txlm_config_type *txlm_config = NULL;

  /* initilize the txlm buffers with NULL */ 
  for (i = 0; i < TXLM_MAX_BUFFERS; i++) 
  {
    txlm_buff_ptr[i] = NULL;
  }

  /* get the modem specific TX configuration */
  txlm_config = txlm_hal_config_get();

  msm_id = 0;
  foundary_id = 0;
  dac_bringup_in_progress = FALSE;

  /* based on the Tx Configuration create the required number of LM buffers */ 
  for (chain = 0; chain < txlm_config->txlm_max_chains; chain++) 
  {
    max_buffers = txlm_config->max_buffers[chain]; 
    for (i = 0; i < max_buffers; i++) 
    {
      txlm_buff_ptr[buffer_id] = new lm_buffer((int32)chain, buffer_id,  
                                               (uint8 *)txlm_config->mem_addrs[chain][i],
                                               txlm_config->buf_size[chain][i]); 
      buffer_id++;
    }
    
    groupA_drif_lmem_addr[chain] = NULL;
    groupB_drif_lmem_addr[chain] = NULL;

    groupA_shared_mem_addr[chain] = NULL;
    groupB_shared_mem_addr[chain] = NULL;

    msm_id = 0;
    foundary_id = 0;
    dac_bringup_in_progress = FALSE;

    device_id_to_shared_mem_map[chain] = RFLM_INVALID_DEVICE;
  }
#ifndef TEST_FRAMEWORK
  /*This will read the foundary register. Different implementations for different modems*/  
  foundary_id = txlm_read_foundry_register();
#endif /* TEST_FRAMEWORK */
  
  #if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_init_critical_section(&txlm_critical_section);
  #endif
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      txlm destructor.
 
   @details
      Frees buffer memory that was allocated when the TxLM object was created.
*/
txlm::~txlm()
{

  /* clean-up the txlm buffers */ 
  for (int i = 0; i < TXLM_MAX_BUFFERS; i++) 
  {
    if (txlm_buff_ptr[i] != NULL)
    {
      delete txlm_buff_ptr[i];
      txlm_buff_ptr[i] = NULL;
    }    
  }
  #if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_deinit_critical_section(&txlm_critical_section);
  #endif

  // initialize the static txlm obj ptr to NULL
  txlm_ptr = NULL;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Reserves a txlm buffer if available and provides the reserved txlm buffer
      id to clients.
 
   @details
      reserves a txlm buffer for the requested rxf chain. And returns the
      reserved buffer id to clients. The buffer id will be used by clients to
      update or modify the RXF settings in the buffer.

   @param  'chain' : the rxf chain for which the txlm buffer to be reserved.
           'tech'  : the technology client which is requesting the buffer.
           '*buffer_index' : the buffer_id info. which is reserved.

   @retval txlm_err_status : txlm error information if any.
   
*/
lm_status_type txlm::allocate_buffer(txlm_chain_type chain, lm_tech_type client_tech, 
                                     lm_tech_type operation_tech, lm_handle_type *txlm_handle) 
{
  lm_status_type status = LM_SUCCESS;

  rflm_dm_handle_meta_s meta_data = {0};
  meta_data.type = RFLM_TX_HANDLE;
  meta_data.device_id = 0;
  meta_data.external_user_data = TRUE;
  switch (operation_tech)
  {
  case LM_TDSCDMA:
    meta_data.owner = RFLM_USR_TDSCDMA;
    break;

  case LM_LTE:
    meta_data.owner = RFLM_USR_LTE;
    break;

  case LM_GSM:
    meta_data.owner = RFLM_USR_GSM;
    break;

  case LM_1X:
    meta_data.owner = RFLM_USR_CDMA;
    break;

  case LM_HDR:
    meta_data.owner = RFLM_USR_EVDO;
    break;

  case LM_UMTS:
    meta_data.owner = RFLM_USR_UMTS;
    break;

  default:
    meta_data.owner = RFLM_NO_USR;
    break;
  }
  /* arguments checking */

  if (txlm_handle == NULL)
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::allocate_buffer: txlm_handle == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  if (chain >= TXLM_CHAIN_MAX || operation_tech >= LM_TECH_MAX)
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::allocate_buffer: chain >= TXLM_CHAIN_MAX || operation_tech >= LM_TECH_MAX.");
    }
    *txlm_handle = LM_INVALID_BUFFER_INDEX;
    status = LM_INVALID_PARAMETER;
    return status;
  }

  *txlm_handle = (lm_handle_type)rflm_dm_allocate_handle(&meta_data);
  if ((lm_handle_type)RFLM_DM_INVALID_HANDLE_ID == *txlm_handle) 
  {
     status = LM_BUFFERS_UNAVAILABLE;
  }
  else 
  {
     lm_op_tech_of_rflm_handle[*txlm_handle] = operation_tech;
     lm_client_tech_of_rflm_handle[*txlm_handle] = client_tech;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Un-reserves a txlm buffer which was previously reserved.
 
   @details
      un-reserves a txlm buffer which was reserved previously. The associated
      buffer will become available to reserve it for clients.

   @param  'buffer_index' : the associated buffer_index to be un-reserved.

   @retval txlm_err_status : txlm error information if any.
   
*/
lm_status_type txlm::deallocate_buffer(lm_handle_type txlm_handle, lm_tech_type client_tech, 
                                     lm_tech_type operation_tech)
{
  lm_status_type status = LM_SUCCESS;

  /*Printing the techs at the beginning of de-allocation so that we dont have to print during crash again.*/
  RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_NONE, "Allocating client tech : %d, deallocating client tech : %d", lm_client_tech_of_rflm_handle [txlm_handle], client_tech);
  RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_NONE, "Allocating operation tech : %d, deallocating operation tech : %d", lm_op_tech_of_rflm_handle[txlm_handle], operation_tech);


  if ( !rflm_dm_validate_handle_type(txlm_handle, RFLM_TX_HANDLE) ) 
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::deallocate_buffer: Invalid handle type.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }
  /*-----
     Check to ensure 
     a.) using the same pair of allocating/deallocating functions(Till both new and old functions exist simultaneously). 
     b.) mismatch in client and/or operation techs while allocating / de-allocating
   -----*/
  if ((client_tech == lm_client_tech_of_rflm_handle [txlm_handle]) && ((client_tech == LM_TECH_MAX) || (operation_tech == lm_op_tech_of_rflm_handle [txlm_handle])))
  {
    rflm_dm_set_device_id(txlm_handle, RFLM_INVALID_DEVICE);
    rflm_dm_set_populated_flag((rflm_dm_handle_id_t)txlm_handle, FALSE);

  if ( rflm_dm_deallocate_handle((rflm_dm_handle_id_t)txlm_handle) )
  {
    status = LM_SUCCESS;
  } 
  else
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::deallocate_buffer: rflm_dm_deallocate_handle returned FALSE.");
    }
    status = LM_INVALID_PARAMETER;
  }
  }
 /*------
 Crash if the above condition is not met.
 --------*/
 else
 {
    RFLM_DIAG_ERR_FATAL("Mismatch in allocating/de-allocating client or operation techs ");
 }
  return status;
}

void txlm::map_device_id_to_shared_mem(int32 device)
{
  if (device_id_to_shared_mem_map[0] == device || device_id_to_shared_mem_map[1] == device)
  {
    return;
  }
  else if (device_id_to_shared_mem_map[0] == RFLM_INVALID_DEVICE)
  {
    device_id_to_shared_mem_map[0] = device;
  }
  else if (device_id_to_shared_mem_map[1] == RFLM_INVALID_DEVICE)
  {
    device_id_to_shared_mem_map[1] = device;
  }
  else
  {
    DTR_DIAG_TRACE_MSG_ERROR("txlm::map_device_id_to_shared_mem: Map already full.");
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a txlm buffer which was previously reserved.
 
   @details
      Update the contents of txlm buffer which was reserved previously with new
      settings based on the new configuration parameters. 

   @param  'buffer_index' : the associated buffer_index to be updated.
           'cfg'          : new configuration parameters based on which
                            txlm buffer need to be updated.

   @retval txlm_err_status : txlm error information if any.
   
*/
lm_status_type txlm::update_static_settings(lm_handle_type txlm_handle, int32 device, void *cfg)
{
  lm_status_type status = LM_SUCCESS;
  uint32 addr = 0;
  uint32 size = 0;
  txlm_chain_type chain = TXLM_CHAIN_MAX;
  lm_tech_type tech = LM_TECH_MAX;

  uint8 *dest_addr = 0;

  if ( !rflm_dm_validate_handle_type(txlm_handle, RFLM_TX_HANDLE) ) 
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::update_static_settings: Invalid handle type.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }
  
  tech = lm_op_tech_of_rflm_handle[txlm_handle];
  if (cfg == NULL)
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::update_static_settings: cfg == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  status = txlm_init_settings_get(chain, tech, cfg, &addr, &size);

  if (status == LM_SUCCESS)
  {
    if ( size <= sizeof(rflm_dtr_tx_settings_type_ag))
    {
      dest_addr = (uint8 *)rflm_dm_get_dtr_buf_addr( (rflm_dm_handle_id_t) txlm_handle );
      if ( dest_addr == NULL )
      {
            if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
            {
              DTR_DIAG_TRACE_MSG_ERROR("txlm::update_static_settings: dest_addr == NULL.");
            }
            status = LM_INVALID_PARAMETER;
        return status;
      }
      memscpy(dest_addr, size, (void *)addr, size);
    }

    rflm_dm_set_device_id((rflm_dm_handle_id_t)txlm_handle, device);

    map_device_id_to_shared_mem(device);

    rflm_dm_set_populated_flag(txlm_handle, TRUE);
  }
  else
  {
    DTR_DIAG_TRACE_MSG_ERROR("txlm::update_static_settings: txlm_init_settings_get() failed. Handle = %d", txlm_handle);
  }
  
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a dynamic portion of txlm buffer which was previously reserved.
 
   @details
      Update the dynamic part of the txlm buffer which was reserved previously
      with new settings based on the new configuration parameters.

   @param  'buffer_index' : the associated buffer_index to be updated.
           'group_id'     : Enum/ID indicates which dynamic group to be updated.
           'addr'         : Address of the populated dynamic structure from SW
                            to be mem-copied into the shared mem buffer
 
   @retval txlm_err_status : txlm error information if any.
   
*/
lm_status_type txlm::update_dynamic_settings(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr)
{
  lm_status_type status = LM_SUCCESS;
  uint32 size = 0;
  uint32 offset = 0;
  uint8 *dest_addr = 0;

  if ( !rflm_dm_validate_handle_type(txlm_handle, RFLM_TX_HANDLE) ) 
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::update_dynamic_settings: Invalid handle type.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  if ( addr == NULL)
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::update_dynamic_settings: addr == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  status = txlm_dynamic_group_offset_addr_get(group_id, &offset, &size);

  /*Log addr*/
  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_2) 
  {
    dtr_print_memory_chunk(addr,size);
  }

  #ifndef FEATURE_D3925_FED
  if ( (size + offset ) <= sizeof(rflm_dtr_tx_settings_type_ag))
  {
    dest_addr = (uint8 *)rflm_dm_get_dtr_buf_addr( (rflm_dm_handle_id_t) txlm_handle );
    if ( dest_addr == NULL )
    {
          if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
          {
            DTR_DIAG_TRACE_MSG_ERROR("txlm::update_dynamic_settings: dest_addr == NULL.");
          }
          status = LM_INVALID_PARAMETER;
      return status;
    }
    dest_addr += offset;
    memscpy(dest_addr, size, (void *)addr, size);
  }
  #else
    if ( size <= sizeof(fw_txlm_settings_type_ag))
    {
      dest_addr = (uint8 *)FW_TXLM_BUF_ADDR( txlm_handle );
      if ( dest_addr == NULL )
      {
            if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
            {
              DTR_DIAG_TRACE_MSG_ERROR("txlm::update_dynamic_settings: dest_addr == NULL.");
            }
            status = LM_INVALID_PARAMETER;
            return status;
      }
      dest_addr += offset;
      memscpy(dest_addr, size, (void *)addr, size);
    }
  #endif

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Return buffer contents for a portion of lm buffer.
 
   @details
      Returns buffer contents for a portion of lm buffer. This functionality
      is required when a portion of the dynamic settings is requiered to be
      updated; ex: Set only the iq mismatch spectral inversion bit.

   @param  'txlm_handle'  : The buffer associated txlm_handle to be updated.
           'group_id'     : Enum/ID indicates which dynamic group to be updated.
           'addr'         : Address of the populated dynamic structure from SW
                            to be mem-copied into the shared mem buffer

   @retval LM_SUCCESS if the buffer is read, otherwise
   returns the appropriate error status.
   
*/
lm_status_type txlm::get_buffer_content(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr)
{
  lm_status_type status = LM_SUCCESS;
  uint32 size = 0;
  uint32 offset = 0;
  uint8 *buf_src_addr = 0;

  if ( !rflm_dm_validate_handle_type(txlm_handle, RFLM_TX_HANDLE) ) 
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::get_buffer_content: Invalid handle type.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  if ( addr == NULL)
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::get_buffer_content: addr == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  status = txlm_dynamic_group_offset_addr_get(group_id, &offset, &size);

  #ifndef FEATURE_D3925_FED
  if ( (size + offset ) <= sizeof(rflm_dtr_tx_settings_type_ag))
  {
    buf_src_addr = (uint8 *)rflm_dm_get_dtr_buf_addr( (rflm_dm_handle_id_t) txlm_handle );
    if ( buf_src_addr == NULL )
    {
          if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
          {
            DTR_DIAG_TRACE_MSG_ERROR("txlm::get_buffer_content: buf_src_addr == NULL.");
          }
      status = LM_INVALID_PARAMETER;
      return status;
    }
    buf_src_addr += offset;
    memscpy((void *)addr, size, buf_src_addr, size);
  }
  #else
  if ( (size + offset ) <= sizeof(fw_txlm_settings_type_ag))
  {
    buf_src_addr = (uint8 *)FW_TXLM_BUF_ADDR( txlm_handle );
    if ( buf_src_addr == NULL )
    {
          if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
          {
            DTR_DIAG_TRACE_MSG_ERROR("txlm::get_buffer_content: buf_src_addr == NULL.");
          }
      status = LM_INVALID_PARAMETER;
      return status;
    }
    buf_src_addr += offset;
    memscpy((void *)addr, size, buf_src_addr, size);
  }
  #endif

  /*Log addr*/
  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_2) 
  {
    dtr_print_memory_chunk(addr,size);
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return TxLM chain for a given TxLM handle
 
  @param addr
  lm_handle_type TxLM handle
 
  @return
  txlm_chain_type
*/
lm_status_type txlm::get_chain(lm_handle_type txlm_handle, txlm_chain_type* chain)
{
  lm_status_type status = LM_SUCCESS;

  #ifndef FEATURE_D3925_FED
  status = txlm_hal_get_chain(txlm_handle,chain);
  #else
  status = LM_INVALID_PARAMETER;
  #endif
  
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Write settings from autogenerated TxLM SW Dynamic settings into a local group struct variable.
 
  
  @param  dyn_functionality
  Enum/ID indicates the sub-group that has dynamic functionality enabled.
  @param  group_id
  Enum/ID indicates which dynamic group to be updated.
  @param  addr
  Address of the populated dynamic structure to be updated.
  @param  column_enum
  Enum/ID indicates which column of the SW dynamic table to
  use to update the dynamic structure pointed to by addr.
 
  @return
  LM_SUCCESS if the operation requested was valid
*/
lm_status_type txlm::update_dynamic_group_struct(uint32 dyn_functionality,
                               uint32 group_id, uint8 *addr, uint32 table_index)
{
  lm_status_type status = LM_SUCCESS;

  if (addr == NULL)
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::update_dynamic_group_struct: addr == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  status = txlm_update_dynamic_group_struct_ag(dyn_functionality, group_id, 
                                                             addr, table_index);

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a txlm buffer which was previously reserved.
 
   @details
      Update the contents of txlm buffer which was reserved previously with new
      settings based on the new configuration parameters. 

   @param  'buffer_index' : the associated buffer_index to be updated.
           'cfg'          : new configuration parameters based on which
                            txlm buffer need to be updated.

   @retval txlm_err_status : txlm error information if any.
   
*/
lm_status_type txlm::set_up_dac_cal(void *cfg)
{
  lm_status_type status = LM_SUCCESS;

  if (cfg == NULL)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::set_up_dac_cal: cfg == NULL.");
    status = LM_INVALID_PARAMETER;
    return status;
  }
  txlm_dac_cal_config_type *txlm_dac_cal_cfg_var = (txlm_dac_cal_config_type *)cfg;

  status = txlm_dac_cal_write_settings(txlm_dac_cal_cfg_var);

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a txlm buffer which was previously reserved.
 
   @details
      Update the contents of txlm buffer which was reserved previously with new
      settings based on the new configuration parameters. 

   @param  'buffer_index' : the associated buffer_index to be updated.
           'cfg'          : new configuration parameters based on which
                            txlm buffer need to be updated.

   @retval txlm_err_status : txlm error information if any.
   
*/
lm_status_type txlm::dac_cal_cmd(uint32 dac_cal_cmd_enum, uint8 size, uint32 *data)
{
  lm_status_type status = LM_SUCCESS;

  if( size > 0 && data == NULL)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::dac_cal_cmd: size > 0 && data == NULL.");
    status = LM_INVALID_PARAMETER;
  }
  else
  {
    status = txlm_dac_cal_cmd_dispatch((txlm_dac_cal_cmd_enum_type)dac_cal_cmd_enum, size, data);
  }

  return status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform TXLM Sequences
 
  @details
  Perform sequences such as:
  - FCAL Clock Enable
  - ET DAC Enable

  @param txlm_handle
  The buffer associated txlm_handle

  @param sequence_id
  Enum/ID indicates which seq to be executed

  @param *txlm_seq_ret_val
  meta data
  
   @return
  Status of Execute Sequences
*/

lm_status_type txlm::execute_sequence(lm_handle_type handle, uint32 seq_id, uint32 *data)
{
  lm_status_type status = LM_SUCCESS;
  txlm_chain_type chain;

  if ( !rflm_dm_validate_handle_type(handle, RFLM_TX_HANDLE) ) 
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::execute_sequence: Invalid handle type.");
    status = LM_INVALID_PARAMETER;
    return status;
  }
  
  txlm *txlm_obj = txlm::get_instance();  

  status = txlm_hal_get_chain(handle, &chain);

  /* Can not perform sequences while DAC is being powered up */  
  #ifndef FEATURE_D3925_FED
  if (txlm_obj->dac_bringup_in_progress == FALSE
    && status == LM_SUCCESS)
  {    
    status = txlm_hal_execute_sequence(chain, seq_id, data);  
  }
  else
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::execute_sequence: (txlm_obj->dac_bringup_in_progress == FALSE && status == LM_SUCCESS) is FALSE.");
    status = LM_INVALID_PARAMETER;
  }
  #else
  status = LM_INVALID_PARAMETER;
  #endif
  
  return status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform TXLM Sequences
 
  @details
  Perform sequences such as:
  - FCAL Clock Enable
  - ET DAC Enable

  @param txlm_handle
  The buffer associated txlm_handle

  @param sequence_id
  Enum/ID indicates which seq to be executed

  @param *txlm_seq_ret_val
  meta data
  
   @return
  Status of Execute Sequences
*/

void txlm::set_lmem_info(txlm_chain_type chain, boolean drif_group_A, uint32* lmem_start_addr)
{
  #ifndef FEATURE_D3925_FED
  if(chain < TXLM_CHAIN_MAX
    || lmem_start_addr != NULL)
  {
    if (drif_group_A == TRUE)
    {
      txlm::groupA_drif_lmem_addr[chain] = lmem_start_addr;
    }
    else
    {
      txlm::groupB_drif_lmem_addr[chain] = lmem_start_addr;
    }
  }  
  #endif
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set lmem addresses
 
  @details
  Set lmem addresses for group A and B

   @return
   void
*/

void txlm::set_shared_mem_addr
(
  uint32 mem0_groupA,
  uint32 mem1_groupA,
  uint32 mem0_groupB,
  uint32 mem1_groupB
)
{
  if (mem0_groupA != NULL && mem1_groupA != NULL &&mem0_groupB != NULL && mem1_groupB != NULL)
  {
    txlm::groupA_shared_mem_addr[0] = (uint32*)mem0_groupA; 
    txlm::groupA_shared_mem_addr[1] = (uint32*)mem1_groupA;
    txlm::groupB_shared_mem_addr[0] = (uint32*)mem0_groupB;
    txlm::groupB_shared_mem_addr[1] = (uint32*)mem1_groupB;
  }
  else
  {
    RFLM_DIAG_MSG_ERROR("txlm::set_shared_mem_addr: mem group is NULL. mem0_groupA = 0x%x, mem1_groupA = 0x%x, mem0_groupB = 0x%x, mem1_groupB = 0x%x",
                         mem0_groupA, mem1_groupA, mem0_groupB, mem1_groupB);
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform TXLM Sequences
  
  @details
  Perform sequences such as:
  - FCAL Clock Enable
  - ET DAC Enable
  
  @param txlm_handle
  The buffer associated txlm_handle
  
  @param sequence_id
  Enum/ID indicates which seq to be executed
  
  @param *txlm_seq_ret_val
  meta data
  
   @return
  Status of Execute Sequences
*/

lm_status_type txlm::retrieve_lmem_info (                      
                                                  txlm_chain_type chain,
                                                  uint32** groupA_drif_start_addr,
                                                  uint32** groupB_drif_start_addr
                                                )
{
  lm_status_type lm_status = LM_SUCCESS; 

  uint32* txfe_lmem_start_addr_groupA[TXLM_CHAIN_MAX]; 
  uint32* txfe_lmem_start_addr_groupB[TXLM_CHAIN_MAX]; 

  /* Backup if FW doesnt store addresses */
  txfe_lmem_start_addr_groupA[TXLM_CHAIN_0] = (uint32*)0xEDC3B600;
  txfe_lmem_start_addr_groupA[TXLM_CHAIN_1] = (uint32*)0xEDC0F520;
  txfe_lmem_start_addr_groupB[TXLM_CHAIN_0] = (uint32*)0xEDC14000;
  txfe_lmem_start_addr_groupB[TXLM_CHAIN_1] = (uint32*)0xEDC14120;

  #ifndef FEATURE_D3925_FED
  if(chain >= TXLM_CHAIN_MAX)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::retrieve_lmem_info: chain >= TXLM_CHAIN_MAX.");
    lm_status = LM_INVALID_PARAMETER;
  }
  else
  {
    *groupA_drif_start_addr = (uint32*)txlm::groupA_drif_lmem_addr[chain];
    *groupB_drif_start_addr = (uint32*)txlm::groupB_drif_lmem_addr[chain];

    if (*groupA_drif_start_addr == NULL)
    {
      *groupA_drif_start_addr = (uint32*)txfe_lmem_start_addr_groupA[chain];
    }

    if (*groupB_drif_start_addr == NULL)
    {
      *groupB_drif_start_addr = (uint32*)txfe_lmem_start_addr_groupB[chain];
    }

  }     
  #else
  lm_status = LM_INVALID_PARAMETER;
  #endif

  return lm_status;
}

lm_status_type txlm::get_shared_mem_addr(
    rflm_handle_tx_t handle_id,
    const rflm_dtr_tx_get_shared_mem_input_t* in,
    rflm_dtr_tx_get_shared_mem_output_t* out
    )
{
  lm_status_type lm_status = LM_SUCCESS; 
  uint32 shared_mem_id = 0;

  int32 device = RFLM_INVALID_DEVICE;

  if ( !rflm_dm_validate_handle_type(handle_id, RFLM_TX_HANDLE) ) 
  {
    if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
    {
      DTR_DIAG_TRACE_MSG_ERROR("txlm::get_shared_mem_info: Invalid handle type.");
    }
    lm_status = LM_INVALID_PARAMETER;
    return lm_status;
  }

  device = rflm_dm_get_device_id(handle_id);

  return get_shared_mem_addr(device, in, out);
}

lm_status_type txlm::get_shared_mem_addr(
    int32 device,
    const rflm_dtr_tx_get_shared_mem_input_t* in,
    rflm_dtr_tx_get_shared_mem_output_t* out
    )
{
  lm_status_type lm_status = LM_SUCCESS; 
  uint32 shared_mem_id = 0;

  if (device != RFLM_INVALID_DEVICE)
  {
    for (shared_mem_id = 0; shared_mem_id < TXLM_CHAIN_MAX; shared_mem_id++)
    {
      if (device_id_to_shared_mem_map[shared_mem_id] == device)
      {
        break;
      }
    }

    if (shared_mem_id < TXLM_CHAIN_MAX )
    {
      out->mem_groupA = (uint32)txlm::groupA_shared_mem_addr[shared_mem_id];
      out->mem_groupB = (uint32)txlm::groupB_shared_mem_addr[shared_mem_id];
    }
    else
    {
      RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::get_shared_mem_info: Device not in map.");
      lm_status = LM_INVALID_PARAMETER;
    }
  }
  else
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::get_shared_mem_info: Device not set in handle.");
    lm_status = LM_INVALID_PARAMETER;
  }

  return lm_status;
}


lm_status_type txlm::retrieve_dac_iref_lut (const uint8 *iref_lut, uint8 iref_lut_length)
{
  lm_status_type lm_status = LM_SUCCESS; 

  #ifndef FEATURE_D3925_FED
  lm_status = txlm_hal_retrieve_dac_iref_lut(iref_lut,iref_lut_length);
  #else
  lm_status = LM_INVALID_PARAMETER;
  #endif

  return lm_status;
}


lm_status_type txlm::config_dac_iref(
                                    lm_handle_type handle,
                                    boolean update_en,
                                    uint32  iref_lut_idx,
                                    uint32 iref_update_start_time,
                                    uint32 iref_update_stop_time)
{
  lm_status_type lm_status = LM_SUCCESS;  

  txlm_chain_type chain;

  if ( !rflm_dm_validate_handle_type(handle, RFLM_TX_HANDLE) ) 
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::config_dac_iref: Invalid handle type.");
    lm_status = LM_INVALID_PARAMETER;
    return lm_status;
  }

  lm_status = txlm_hal_get_chain(handle,&chain);

  #ifndef FEATURE_D3925_FED
  if (lm_status == LM_SUCCESS)
  {
    txlm_hal_config_dac_iref(chain,
                            update_en,
                            iref_lut_idx,
                            iref_update_start_time,
                            iref_update_stop_time);
  }
  else
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "txlm::config_dac_iref: txlm_hal_get_chain did not return LM_SUCCESS.");
    lm_status = LM_INVALID_PARAMETER;
  }
  #else
  lm_status = LM_INVALID_PARAMETER;
  #endif

  return lm_status;
}

lm_status_type txlm::retrieve_dac_therm_info(dac_therm_info_t* dac_therm_info)
{
  lm_status_type status = LM_SUCCESS;

  status = txlm_hal_retrieve_dac_therm_info(txlm::msm_id, txlm::foundary_id, dac_therm_info);

  return status;
}

/*----------------------------------------------------------------------------*/
lm_status_type txlm::acquire_lock(lm_handle_type txlm_handle, rflm_rw_lock_t *rflm_dtr_handle_lock)
{
  lm_status_type status = LM_SUCCESS;

  if ( !rflm_dm_validate_handle_type(txlm_handle, RFLM_TX_HANDLE) ) 
  {
    DTR_DIAG_TRACE_MSG_ERROR("txlm::acquire_lock: Invalid handle type.");
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

  rflm_rw_lock_write_lock(rflm_dtr_handle_lock);

  return status;
}

/*----------------------------------------------------------------------------*/
lm_status_type txlm::release_lock(lm_handle_type txlm_handle, rflm_rw_lock_t *rflm_dtr_handle_lock)
{
  lm_status_type status = LM_SUCCESS;

  if ( !rflm_dm_validate_handle_type(txlm_handle, RFLM_TX_HANDLE) ) 
  {
    DTR_DIAG_TRACE_MSG_ERROR("txlm::release_lock: Invalid handle type.");
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

  rflm_rw_lock_write_unlock(rflm_dtr_handle_lock);

  return status;
}

uint32 txlm::get_foundry_id()
{
  return foundary_id;
}

