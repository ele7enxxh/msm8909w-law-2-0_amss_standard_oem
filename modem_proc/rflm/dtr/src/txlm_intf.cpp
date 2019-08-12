/*!
   @file
   txlm_intf.cpp

   @brief
   This file implements the Interafce APIs for TXLM functionality. These are
   platform independent.

*/

/*===========================================================================

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/src/txlm_intf.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
03/13/15   ak      Implementation of new DTR lock mechanism
10/31/14   dr      Move DAC foundry id retrieval to TxLM object layer
08/04/14   dr      Adding support for foundry dependent IREF gain programming 
06/16/14   sar     Added Featurization to fix compilation in off-target build.
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
04/09/14   cvd     Fix KW Errors 
02/18/14   st      Fixed Compiler Warning
02/13/14   st      DAC Temperature Compensation Support
02/05/14   st       Removed DAC Mission Mode setup from TXLM Initialization
12/04/13   vrb     Remove some Trace logging for timeline considerations
12/04/13   vrb     Convert some diag messages to Trace logging for timeline
11/21/13   cvd     Add Logging messages
10/04/13   st      Bolt DTR-TX New APIs
10/03/13   vrb     Removing featurization for branched out modems
08/27/13   jf      Added interface for txlm register dump
04/01/13   dej     Add dynamic settings tables api to TxLM
01/14/12   aca     DIME Dac cal- migrated to AG functions
12/03/12   aca     DIME Dac cal
08/02/12   vrb     Updates for Dime Compatibility
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
08/04/11   cd      Support for SW Dynamic Updates to LM buffer
                   Support for reading portions of LM buffer
03/24/11   vrb     LM Intf changes for CMI Compliance
02/23/11   sty     Fixed warnings
01/21/11   sr      code cleanup based on the code review.
12/22/10   vrb     Updated Signature for rxlm_update_buffer_state
12/14/10   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rflm.h"
#include "txlm_intf.h"
#include "rflm_dtr.h"
#include "txlm.h"
#include "qurt.h"
#include "rflm_hwintf.h"
#include "rflm_rw_lock.h"


#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM))
#error code not present
#endif
extern rflm_rw_lock_t rflm_dtr_handle_locks[TXLM_MAX_BUFFERS];
extern uint32 rflm_dtr_handle_locks_rf_locked[RXLM_MAX_BUFFERS];
#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM))
#error code not present
#endif

/*===========================================================================
                           FUNCTION DEFINITIONS
===========================================================================*/



/*----------------------------------------------------------------------------*/
/*!
   @brief
      Initializes the TxLM software module.
 
   @details
      Initializes the TxLM software module.
   
*/
lm_status_type txlm_init(void)
{
  lm_status_type txlm_status = LM_SUCCESS; 
#ifndef T_MDM9X45  
   txlm *txlm_obj = txlm::get_instance();

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("txlm_init: void");
  }
#endif  /* T_MDM9X45 */
  return txlm_status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Reserves a txlm buffer if available and provides the reserved txlm buffer
      id to clients.
 
   @details
      reserves a txlm buffer for the requested Tx chain. And returns the
      reserved buffer id to clients. The buffer id will be used by clients to
      update or modify the TX settings in the buffer.

   @param  'chain' : the tx chain for which the txlm buffer to be reserved.
           'tech'  : the technology client which is requesting the buffer.
           '*txlm_handle' : the txlm buffer handle, which is reserved.

   @retval LM_SUCCESS if the buffer is allocated sucessfully, otherwise returns
   the appropriate error status.
   
*/
lm_status_type txlm_allocate_buffer(txlm_chain_type chain, lm_tech_type tech, 
                                    lm_handle_type *txlm_handle)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

  txlm_status = txlm_obj->allocate_buffer( TXLM_CHAIN_0, LM_TECH_MAX, tech, txlm_handle);

  // Initialize the handle mutex.
  if (txlm_status == LM_SUCCESS)
  {
    if (*txlm_handle < TXLM_MAX_BUFFERS)
    {
      rflm_rw_lock_init(&rflm_dtr_handle_locks[*txlm_handle], RFLM_DTR_MAX_READER_THREADS); 
      rflm_dtr_handle_locks_rf_locked[*txlm_handle] = 0;
    }
    else 
    {
      txlm_status = LM_INVALID_BUFFER_INDEX;
    }
  }

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("txlm_allocate_buffer: Chain = %d, Tech = %d, Handle = %d, Status = %d.",
                           chain, tech, *txlm_handle, txlm_status);
  }

  return txlm_status;

}




lm_status_type rflm_allocate_tx_handle(lm_tech_type client_tech, lm_tech_type operation_tech,
                                       lm_handle_type *rflm_tx_handle)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

  txlm_status = txlm_obj->allocate_buffer( TXLM_CHAIN_0, client_tech, operation_tech, rflm_tx_handle);

  // Initialize the handle mutex.
  if (txlm_status == LM_SUCCESS)
  {
    if (*rflm_tx_handle < TXLM_MAX_BUFFERS)
    {
      rflm_rw_lock_init(&rflm_dtr_handle_locks[*rflm_tx_handle], RFLM_DTR_MAX_READER_THREADS); 
      rflm_dtr_handle_locks_rf_locked[*rflm_tx_handle] = 0;
    }
    else 
    {
      txlm_status = LM_INVALID_BUFFER_INDEX;
    }
  }

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rflm_allocate_tx_handle: Operation Tech = %d, Handle = %d, Status = %d.",
                            operation_tech, *rflm_tx_handle, txlm_status);
  }

  return txlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Un-reserves a txlm buffer which was previously reserved.
 
   @details
      un-reserves a txlm buffer which was reserved previously. The associated
      buffer will become available to reserve it for clients.

   @param  'txlm_handle' : the associated buffer_index to be un-reserved.

   @retval LM_SUCCESS if the buffer is de-allocated sucessfully, otherwise
   returns the appropriate error status.
   
*/
lm_status_type txlm_deallocate_buffer(lm_handle_type txlm_handle)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

  txlm_status = txlm_obj->deallocate_buffer(txlm_handle, LM_TECH_MAX, LM_TECH_MAX);

  // Destroy the handle lock
  if (txlm_status == LM_SUCCESS)
  {
    rflm_rw_lock_destroy(&rflm_dtr_handle_locks[txlm_handle]);
    rflm_dtr_handle_locks_rf_locked[txlm_handle] = 0;
  }


  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("txlm_deallocate_buffer: Handle = %d, Status = %d.",
                           txlm_handle, txlm_status);
  }

  return txlm_status;

}


lm_status_type rflm_deallocate_tx_handle(lm_tech_type client_tech, lm_tech_type operation_tech,
                                          lm_handle_type rflm_tx_handle)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

  txlm_status = txlm_obj->deallocate_buffer(rflm_tx_handle, client_tech, operation_tech);

  // Destroy the handle lock
  if (txlm_status == LM_SUCCESS)
  {
    rflm_rw_lock_destroy(&rflm_dtr_handle_locks[rflm_tx_handle]);
    rflm_dtr_handle_locks_rf_locked[rflm_tx_handle] = 0;
  }

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rflm_deallocate_tx_handle: Handle = %d, Status = %d.",
                           rflm_tx_handle, txlm_status);
  }

  return txlm_status;

}


/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a txlm buffer which was previously reserved.
 
   @details
      Update the contents of txlm buffer which was reserved previously with new
      settings based on the new configuration parameters. 

   @param  'txlm_handle' : the associated buffer_index to be updated.
           'cfg'          : new configuration parameters based on which
                            txlm buffer need to be updated.

   @retval LM_SUCCESS if the buffer is updated with static settings, otherwise
   returns the appropriate error status.
   
*/
lm_status_type txlm_update_static_settings(lm_handle_type txlm_handle, int32 device, void *cfg)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

  txlm_status = txlm_obj->update_static_settings(txlm_handle, device, cfg);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("txlm_update_static_settings: Handle = %d, Status = %d",
                           txlm_handle, txlm_status);
  }

  return txlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a dynamic portion of txlm buffer which was previously reserved.
 
   @details
      Update the dynamic part of the txlm buffer which was reserved previously
      with new settings based on the new configuration parameters.

   @param  'txlm_handle' : the associated txlm handle to be updated.
           'group_id'     : Enum/ID indicates which dynamic group to be updated.
           'addr'         : Address of the populated dynamic structure from SW
                            to be mem-copied into the shared mem buffer

   @retval LM_SUCCESS if the buffer is updated with dynamic settings, otherwise
   returns the appropriate error status.
   
*/
lm_status_type txlm_update_dynamic_settings(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr)
{
  lm_status_type txlm_status = LM_SUCCESS;


  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

  txlm_status = txlm_obj->update_dynamic_settings(txlm_handle, group_id, addr);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_update_dynamic_settings: Handle = %d, Group ID = %d, Status = %d.",
                           txlm_handle, group_id, txlm_status);
  }

  return txlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return buffer contents for a portion of lm buffer.
 
  @details
  Returns buffer contents for a portion of lm buffer. This functionality
  is required when a portion of the dynamic settings is requiered to be
  updated; ex: Only 2 of the 6 notch filters need to be updated by SW.

  @param txlm_handle
  The buffer associated txlm_handle to be updated.

  @param group_id
  Enum/ID indicates which dynamic group to be updated.

  @param addr
  Address of the populated dynamic structure from SW to be mem-copied
  into the shared mem buffer

  @return
  Status of Buffer Content Read Action
*/
lm_status_type txlm_get_buffer_content(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

    rflm_rw_lock_read_lock(&rflm_dtr_handle_locks[txlm_handle]); 

  txlm_status = txlm_obj->get_buffer_content(txlm_handle, group_id, addr);

    rflm_rw_lock_read_unlock(&rflm_dtr_handle_locks[txlm_handle]);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_get_buffer_content: Handle = %d, Group ID = %d, Status = %d.",
                        txlm_handle, group_id, txlm_status);
  }

  return txlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generates TxLM register dump
 
  @param addr
  void
 
  @return
  void
*/
extern "C" 
{ 
    void txlm_generate_reg_dump(); 
}

lm_status_type txlm_log(void)
{
   txlm_generate_reg_dump();

   if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
   {
     RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_log: void.");
   }

   return LM_SUCCESS;
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
txlm_chain_type txlm_get_chain(lm_handle_type txlm_handle)
{
  lm_status_type status = LM_SUCCESS;
  txlm_chain_type txlm_chain = TXLM_CHAIN_NUM;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

    rflm_rw_lock_read_lock(&rflm_dtr_handle_locks[txlm_handle]); 
  
  status = txlm_obj->get_chain(txlm_handle, &txlm_chain);

  // Unlcok the handle lock.
    rflm_rw_lock_read_unlock(&rflm_dtr_handle_locks[txlm_handle]);

  txlm_chain = (status == LM_SUCCESS) ? txlm_chain : TXLM_CHAIN_NUM;
  
  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_get_chain: Handle = %d, Chain = %d.",
                        txlm_handle, txlm_chain);   
  }

  return txlm_chain;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Write settings from autogenerated TxLM SW Dynamic settings into a local group struct variable.
 
  
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
lm_status_type txlm_update_dynamic_group_struct(uint32 dyn_functionality, uint32 group_id, uint8 *addr, uint32 table_index)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  txlm_status = txlm_obj->update_dynamic_group_struct(dyn_functionality, group_id, addr, table_index);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_update_dynamic_group_struct: Group ID = %d, Table Index = %d, Status = %d.",
                        group_id, table_index, txlm_status);
  }

  return txlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a dynamic portion of txlm buffer which was previously reserved.
 
   @details
      Update the dynamic part of the txlm buffer which was reserved previously
      with new settings based on the new configuration parameters.

   @param  'txlm_handle' : the associated txlm handle to be updated.
           'group_id'     : Enum/ID indicates which dynamic group to be updated.
           'addr'         : Address of the populated dynamic structure from SW
                            to be mem-copied into the shared mem buffer

   @retval LM_SUCCESS if the buffer is updated with dynamic settings, otherwise
   returns the appropriate error status.
   
*/
lm_status_type txlm_set_up_dac_cal(void *cfg)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

  txlm_status = txlm_obj->set_up_dac_cal(cfg);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_set_up_dac_cal: Status = %d.", txlm_status);
  }

  return txlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return buffer contents for a portion of lm buffer.
 
  @details
  Returns buffer contents for a portion of lm buffer. This functionality
  is required when a portion of the dynamic settings is requiered to be
  updated; ex: Only 2 of the 6 notch filters need to be updated by SW.

  @param txlm_handle
  The buffer associated txlm_handle to be updated.

  @param group_id
  Enum/ID indicates which dynamic group to be updated.

  @param addr
  Address of the populated dynamic structure from SW to be mem-copied
  into the shared mem buffer

  @param data
  Data can be NULL if it is a write command (size = 0). All read commands expect a valid pointer for data

  @return
  Status of Buffer Content Read Action
*/
lm_status_type txlm_dac_cal_cmd(uint32 dac_cal_cmd_enum, uint8 size, uint32 *data)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */
  txlm_status = txlm_obj->dac_cal_cmd(dac_cal_cmd_enum, size, data);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_dac_cal_cmd: DAC Cal cmd = %d, Size = %d, Status = %d.",
                        dac_cal_cmd_enum, size, txlm_status);
  }

  return txlm_status;

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
lm_status_type txlm_execute_sequence(lm_handle_type txlm_handle, uint32 sequence_id, uint32 *txlm_seq_ret_val)
{
  
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */
  txlm_status = txlm_obj->execute_sequence(txlm_handle, sequence_id, txlm_seq_ret_val);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_execute_sequence: Handle = %d, Seq ID = %d, Seq Ret Value = %d, Status = %d.",
                        txlm_handle, sequence_id, *txlm_seq_ret_val, txlm_status);
  }
  
  return txlm_status;
  
}

void txlm_set_lmem_info(txlm_chain_type chain, boolean drif_group_A, uint32* lmem_start_addr)
{ 
  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

  txlm_obj->set_lmem_info(chain,drif_group_A,lmem_start_addr);
  
  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_set_lmem_info: Chain = %d, DRIF Group A = %d.",
                          chain, drif_group_A);  
  }
}

lm_status_type txlm_retrieve_lmem_info
    (
      txlm_chain_type chain,
      uint32** groupA_drif_start_addr,
      uint32** groupB_drif_start_addr
    )
{
  lm_status_type txlm_status = LM_SUCCESS;
  
  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

  txlm_status = txlm_obj->retrieve_lmem_info(chain,groupA_drif_start_addr,groupB_drif_start_addr);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_retrieve_lmem_info: Chain = %d, Status = %d.",
                        chain, txlm_status); 
  }

  return txlm_status;
}

lm_status_type txlm_get_shared_mem_addr
(
    lm_handle_type txlm_handle,
    const rflm_dtr_tx_get_shared_mem_input_t* in,
    rflm_dtr_tx_get_shared_mem_output_t* out
)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

  txlm_obj->get_shared_mem_addr(txlm_handle, in, out);

  return txlm_status;
}

lm_status_type txlm_get_shared_mem_addr_dev
(
    int32 device,
    const rflm_dtr_tx_get_shared_mem_input_t* in,
    rflm_dtr_tx_get_shared_mem_output_t* out
)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

  txlm_obj->get_shared_mem_addr(device, in, out);

  return txlm_status;
}

lm_status_type txlm_set_shared_mem_addr
(
  uint32 mem0_groupA,
  uint32 mem1_groupA,
  uint32 mem0_groupB,
  uint32 mem1_groupB
)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

  txlm_obj->set_shared_mem_addr(mem0_groupA, mem1_groupA, mem0_groupB, mem1_groupB);

  return txlm_status;
}

lm_status_type txlm_retrieve_dac_iref_lut
    (
      const uint8 *iref_lut,
      uint8 length
    )
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

  txlm_status = txlm_obj->retrieve_dac_iref_lut(iref_lut,length);

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_retrieve_dac_iref_lut: Chain = %d, Length = %d, Status = %d.",
                        *iref_lut, length, txlm_status); 
  }

  return txlm_status;
  
}

lm_status_type txlm_config_dac_iref( 
                                      lm_handle_type handle,
                                      boolean update_en,
                                      uint32  iref_lut_idx,
                                      uint32 iref_update_start_time,
                                      uint32 iref_update_stop_time
                                    )

{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

    rflm_rw_lock_read_lock(&rflm_dtr_handle_locks[handle]);

  txlm_status = txlm_obj->config_dac_iref(handle,
                                               update_en,
                                               iref_lut_idx,
                                               iref_update_start_time,
                                               iref_update_stop_time);

  // Unlcok the handle lock.
 
    rflm_rw_lock_read_unlock(&rflm_dtr_handle_locks[handle]);
 

  if (txlm::txlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_config_dac_iref: Handle = %d, Update En = %d, IREF LUT idx = %d, Start Time = %d, Stop Time = %d.",
                        handle, update_en, iref_lut_idx, iref_update_start_time, iref_update_stop_time); 
  }

  return txlm_status;
}

lm_status_type txlm_retrieve_dac_therm_info(dac_therm_info_t* dac_therm_info)
{
  lm_status_type txlm_status = LM_SUCCESS;
  
  txlm *txlm_obj = txlm::get_instance();
  
  /* the get_instance() should never return NULL as it returns the ptr to txlm
   object with new operator. So, there is no need to check for txlm_obj == NULL
   condition */

  txlm_status = txlm_obj->retrieve_dac_therm_info(dac_therm_info);
  
  return txlm_status;
}

/*----------------------------------------------------------------------------*/
lm_status_type txlm_acquire_lock(lm_handle_type txlm_handle)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

    txlm_status = txlm_obj->acquire_lock(txlm_handle, &rflm_dtr_handle_locks[txlm_handle]);


  RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_acquire_lock: Handle = %d, Status = %d.",
                    txlm_handle, txlm_status);

  return txlm_status;
}

/*----------------------------------------------------------------------------*/
lm_status_type txlm_release_lock(lm_handle_type txlm_handle)
{
  lm_status_type txlm_status = LM_SUCCESS;

  txlm *txlm_obj = txlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to txlm
  object with new operator. So, there is no need to check for txlm_obj == NULL
  condition */

    txlm_status = txlm_obj->release_lock(txlm_handle, &rflm_dtr_handle_locks[txlm_handle]);
 

  RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "txlm_release_lock: Handle = %d, Status = %d.",
                    txlm_handle, txlm_status);

  return txlm_status;
}

uint32 txlm_get_foundry_id()
{
   txlm *txlm_obj = txlm::get_instance();
  return txlm_obj->get_foundry_id();
}
