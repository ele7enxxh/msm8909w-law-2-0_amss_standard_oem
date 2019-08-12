/*!
   @file
   rxlm_intf.cpp

   @brief
   This file implements the Interafce APIs for RXLM functionality.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2010 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/src/rxlm_intf.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
03/13/15   ak      Implementation of new DTR lock mechanism
06/16/14   sar     Added Featurization to fix compilation in off-target build.
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
04/09/14   cvd     Fix KW Errors 
11/21/13   cvd     Add Logging messages
11/10/13   st      FW Compilation fix
10/03/13   vrb     Removing featurization for branched out modems
08/27/13   jf      Added interface for rxlm register dump  
06/28/13   dej     Added APIs for common notch allocation\deallocation
11/19/12   dej     Implement API to copy settings from SW Dynamic tables into group structs 
05/31/12   vrb     Updates for Dime Compatibility
05/31/12   dej     Fix Compiler Warning
05/15/12   dej     Initialize RxLM buffer in rxlm_init 
11/16/11   vrb     Added new API rxlm_get_chain to get rxlm_chain from buf_idx
04/21/11   vrb     Support for Reading portions of LM Buffer
04/07/11   vrb     Support for SW Dynamic Updates to LM Buffer
03/24/11   vrb     LM Intf changes for CMI Compliance
02/23/11   sty     Fixed warnings
01/21/11   sr      code cleanup based on the code review.
01/18/11   bmg     Added basic parameter checking to RxLM APIs
12/28/10   sr      Added txlm functionality and changed the rxlm to use 
                   common lm_types and lm_buffer.
12/22/10   vrb     Updated Signature for rxlm_update_buffer_state
12/14/10   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rflm.h"
#include "rxlm_intf.h"
#include "rxlm.h"
#include "msg.h"
#include "rflm_dtr.h"
#include "qurt.h"
#include "rflm_rw_lock.h"

#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM))
#error code not present
#endif
extern rflm_rw_lock_t rflm_dtr_handle_locks[RXLM_MAX_BUFFERS];
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
      Initializes the RxLM software module.
 
   @details
      Initializes the RxLM software module.

   @retval LM_SUCCESS if the RxLM is initialized sucessfully, otherwise returns
   the appropriate error status.
*/
lm_status_type rxlm_init(void)
{
  lm_status_type rxlm_status = LM_SUCCESS;
  rxlm *rxlm_obj = rxlm::get_instance();

  if (rxlm_obj == NULL) {
      #if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rxlm::get_instance() failed to return a valid object.",1);
      #endif
      rxlm_status = LM_OBJ_NOT_CREATED;
  }
  
  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rxlm_init: void");
  }

  return rxlm_status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Reserves a rxlm buffer if available and provides the reserved rxlm buffer
      id to clients.
 
   @details
      reserves a rxlm buffer for the requested rxf chain. And returns the
      reserved buffer id to clients. The buffer id will be used by clients to
      update or modify the RXF settings in the buffer.

   @param  'chain' : the rxf chain for which the rxlm buffer to be reserved.
           'tech'  : the technology client which is requesting the buffer.
           '*rxlm_handle' : the buffer_id info. which is reserved.

   @retval LM_SUCCESS if the buffer is allocated sucessfully, otherwise returns
   the appropriate error status.
   
*/
lm_status_type rxlm_allocate_buffer(rxlm_chain_type chain, lm_tech_type tech, 
                                      lm_handle_type *rxlm_handle)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  /* On Dime, buffers are selected from a generic pool, not tied to any WB 
     chain. For backward compatibility to older interface, pass in RxLM 
     chain 0 */
  rxlm_status = rxlm_obj->allocate_buffer( RXLM_CHAIN_0, LM_TECH_MAX, tech, rxlm_handle);

// Initialize the handle mutex.
  if (rxlm_status == LM_SUCCESS)
  {
    if (*rxlm_handle < RXLM_MAX_BUFFERS)
    {
      rflm_rw_lock_init(&rflm_dtr_handle_locks[*rxlm_handle], RFLM_DTR_MAX_READER_THREADS); 
      rflm_dtr_handle_locks_rf_locked[*rxlm_handle] = 0;
    }
    else
    {
      rxlm_status = LM_INVALID_BUFFER_INDEX;
    }
  }

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rxlm_allocate_buffer: Chain = %d, Tech = %d, Handle = %d, Status = %d.", 
                           chain, tech, *rxlm_handle, rxlm_status);
  }

  return rxlm_status;
}


lm_status_type rflm_allocate_rx_handle(lm_tech_type client_tech, lm_tech_type operation_tech,
                                       lm_handle_type *rflm_rx_handle)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  /* On Dime, buffers are selected from a generic pool, not tied to any WB 
     chain. For backward compatibility to older interface, pass in RxLM 
     chain 0 */
  rxlm_status = rxlm_obj->allocate_buffer( RXLM_CHAIN_0, client_tech, operation_tech, rflm_rx_handle);

// Initialize the handle mutex.
  if (rxlm_status == LM_SUCCESS)
  {
    if (*rflm_rx_handle < RXLM_MAX_BUFFERS)
    {
      rflm_rw_lock_init(&rflm_dtr_handle_locks[*rflm_rx_handle], RFLM_DTR_MAX_READER_THREADS); 
      rflm_dtr_handle_locks_rf_locked[*rflm_rx_handle] = 0;
    }
    else
    {
      rxlm_status = LM_INVALID_BUFFER_INDEX;
    }
  }

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rflm_allocate_rx_handle: Operation Tech = %d, Handle = %d, Status = %d.", 
                           operation_tech, *rflm_rx_handle, rxlm_status);
  }

  return rxlm_status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Un-reserves a rxlm buffer which was previously reserved.
 
   @details
      un-reserves a rxlm buffer which was reserved previously. The associated
      buffer will become available to reserve it for clients.

   @param  'rxlm_handle' : the buffer associated rxlm_handle to be un-reserved.

   @retval LM_SUCCESS if the buffer is de-allocated sucessfully, otherwise
   returns the appropriate error status.
   
*/
lm_status_type rxlm_deallocate_buffer(lm_handle_type rxlm_handle)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_status = rxlm_obj->deallocate_buffer(rxlm_handle, LM_TECH_MAX, LM_TECH_MAX);

// Destroy the handle mutex
  if (rxlm_status == LM_SUCCESS)
  {
    rflm_rw_lock_destroy(&rflm_dtr_handle_locks[rxlm_handle]); 
    rflm_dtr_handle_locks_rf_locked[rxlm_handle] = 0;
  }

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rxlm_deallocate_buffer: Handle = %d, Status = %d.",
                           rxlm_handle, rxlm_status);
  }

  return rxlm_status;

}



lm_status_type rflm_deallocate_rx_handle(lm_tech_type client_tech, lm_tech_type operation_tech, 
                                         lm_handle_type rflm_rx_handle)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_status = rxlm_obj->deallocate_buffer(rflm_rx_handle, client_tech, operation_tech);

// Destroy the handle mutex
  if (rxlm_status == LM_SUCCESS)
  {
    rflm_rw_lock_destroy(&rflm_dtr_handle_locks[rflm_rx_handle]); 
    rflm_dtr_handle_locks_rf_locked[rflm_rx_handle] = 0;
  }

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rflm_deallocate_rx_handle: Handle = %d, Status = %d.",
                           rflm_rx_handle, rxlm_status);
  }

  return rxlm_status;

}


/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a rxlm buffer which was previously reserved.
 
   @details
      Update the contents of rxlm buffer which was reserved previously with new
      settings based on the new configuration parameters. 

   @param  'rxlm_handle'  : the buffer associated rxlm_handle to be updated.
           'cfg'          : new configuration parameters based on which
                            rxlm buffer need to be updated.

   @retval LM_SUCCESS if the buffer is updated with static settings, otherwise
   returns the appropriate error status.
   
*/
lm_status_type rxlm_update_static_settings(lm_handle_type rxlm_handle, void *cfg)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_status = rxlm_obj->update_static_settings(rxlm_handle, cfg);

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    DTR_DIAG_TRACE_MSG_MED("rxlm_update_static_settings: Handle = %d, Status = %d",
                           rxlm_handle, rxlm_status);
  }

// Unlcok the handle lock, the lock should still be locked from the rxlm_acquire_lock() call.
//  rflm_rw_lock_write_unlock(&rflm_dtr_handle_locks[rxlm_handle]);

  return rxlm_status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update the buffer with RXF dynamic settings based on the cfg parameters.
 
   @details
      Update the dynamic part of the rxlm buffer which was reserved previously
      with new RXF settings based on the new configuration parameters.

   @param  'rxlm_handle'  : the buffer associated rxlm_handle to be updated.
           'group_id'     : Enum/ID indicates which dynamic group to be updated.
           'addr'         : Address of the populated dynamic structure from SW
                            to be mem-copied into the shared mem buffer


   @retval LM_SUCCESS if the buffer is updated with dynamic settings, otherwise
   returns the appropriate error status.
*/
lm_status_type rxlm_update_dynamic_settings(lm_handle_type rxlm_handle, uint32 group_id, uint8 *addr)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_status = rxlm_obj->update_dynamic_settings(rxlm_handle, group_id, addr);

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_update_dynamic_settings: Handle = %d, Group ID = %d, Status = %d.",
                           rxlm_handle, group_id, rxlm_status);
  }

  // Unlcok the handle lock, the lock should still be locked from the rxlm_acquire_lock() call.
//  qurt_rmutex_unlock(&rflm_dtr_handle_locks[rxlm_handle]);

  return rxlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return buffer contents for a portion of lm buffer.
 
  @details
  Returns buffer contents for a portion of lm buffer. This functionality
  is required when a portion of the dynamic settings is requiered to be
  updated; ex: Only 2 of the 6 notch filters need to be updated by SW.

  @param rxlm_handle
  the buffer associated rxlm_handle to be updated.

  @param group_id
  Enum/ID indicates which dynamic group to be updated.

  @param addr
  Address of the populated dynamic structure from SW to be mem-copied
  into the shared mem buffer

  @return
  Status of Buffer Content Read Action
*/
lm_status_type rxlm_get_buffer_content(lm_handle_type rxlm_handle, uint32 group_id, uint8 *addr)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */

  
  rflm_rw_lock_read_lock(&rflm_dtr_handle_locks[rxlm_handle]); 
  

  rxlm_status = rxlm_obj->get_buffer_content(rxlm_handle, group_id, addr);

  // Unlcok the handle lock.
 
    rflm_rw_lock_read_unlock(&rflm_dtr_handle_locks[rxlm_handle]);
  
  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_get_buffer_content: Handle = %d, Group ID = %d, Status = %d.",
                     rxlm_handle, group_id, rxlm_status);
  }

  return rxlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generates RxLM register dump
 
  @param addr
  void
 
  @return
  void
*/
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
extern "C" 
{ 
    void rxlm_generate_reg_dump(); 
}
#endif


lm_status_type rxlm_log(void)
{
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
   rxlm_generate_reg_dump();
#endif

   if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
   {
     RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_log: void.");
   }

   return LM_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return RxLM chain for a given RxLM handle
 
  @param addr
  lm_handle_type RxLM handle
 
  @return
  rxlm_chain_type
*/
rxlm_chain_type rxlm_get_chain(lm_handle_type rxlm_handle)
{
  rxlm_chain_type rxlm_chain = RXLM_CHAIN_NUM;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_chain = rxlm_obj->get_chain(rxlm_handle);

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_get_chain: Handle = %d, Chain = %d.",
                     rxlm_handle, rxlm_chain);
  }

  return rxlm_chain;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Write settings from autogenerated RxLM SW Dynamic settings into a local group struct variable.
 
  
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
lm_status_type rxlm_update_dynamic_group_struct(uint32 dyn_functionality, uint32 group_id, uint8 *addr, uint32 table_index)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_status = rxlm_obj->update_dynamic_group_struct( dyn_functionality, group_id, addr, table_index);

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_update_dynamic_group_struct: Group ID = %d, Table Index = %d, Status = %d.",
                    group_id, table_index, rxlm_status);
  }

  return rxlm_status;

}

#ifndef FEATURE_TRITON_MODEM
/*----------------------------------------------------------------------------*/
/*!
   @brief
      Reserves a common notch block if available and provides the reserved common notch
      id to clients.
 
   @details
      reserves a common notch block for the requested rxf chain\RxLM buffer and returns the reserved
      common notch block id to clients. The common notch block id will be used by clients to update
      or modify the common notch settings in the RxLM buffer.

   @param  'rxlm_handle' : the rxlm buffer_id info. used to reserve common notch block.
           'chain' : the rxf chain for which the common notch block was reserved.
           '*cmn_notch_dyn_block_id'  : the common notch block id reserved.

   @retval LM_SUCCESS if the buffer is allocated successfully, otherwise returns
   the appropriate error status.
   
*/
lm_status_type rxlm_allocate_common_notch( lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32* cmn_notch_dyn_block_id )
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_status = rxlm_obj->allocate_common_notch( rxlm_handle, chain, cmn_notch_dyn_block_id );

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_allocate_common_notch: Handle = %d, Chain- %d, Status = %d.",
                    rxlm_handle, chain, rxlm_status);
  }

  return rxlm_status;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Frees a common notch block for a particular RxLM buffer.
 
   @details
      Frees a common notch block for the requested rxf chain\RxLM buffer.

   @param  'rxlm_handle' : the rxlm buffer_id info. used to reserve common notch block.
           'chain' : the rxf chain for which the common notch block was reserved.
           'cmn_notch_dyn_block_id'  : the common notch block id being freed.

   @retval LM_SUCCESS if the buffer is freed successfully, otherwise returns
   the appropriate error status.
   
*/
lm_status_type rxlm_deallocate_common_notch( lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32 cmn_notch_dyn_block_id )
{
  lm_status_type rxlm_status = LM_SUCCESS;

  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */
  rxlm_status = rxlm_obj->deallocate_common_notch( rxlm_handle, chain, cmn_notch_dyn_block_id );

  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_1) 
  {
    RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_deallocate_common_notch: Handle = %d, Chain = %d, cmn_notch_dyn_block_id = %d, Status = %d.",
                    rxlm_handle, chain, cmn_notch_dyn_block_id, rxlm_status);
  }

  return rxlm_status;

}

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_acquire_lock(lm_handle_type rxlm_handle)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  #ifndef TEST_FRAMEWORK
  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */

  
   rxlm_status = rxlm_obj->acquire_lock(rxlm_handle, &rflm_dtr_handle_locks[rxlm_handle]);   

  RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_acquire_lock:  Handle = %d, Status = %d.",
                    rxlm_handle, rxlm_status);
  #endif /* !TEST_FRAMEWORK */
  return rxlm_status;
}

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_release_lock(lm_handle_type rxlm_handle)
{
  lm_status_type rxlm_status = LM_SUCCESS;

  #ifndef TEST_FRAMEWORK
  rxlm *rxlm_obj = rxlm::get_instance();

  /* the get_instance() should never return NULL as it returns the ptr to rxlm
  object with new operator. So, there is no need to check for rxlm_obj == NULL
  condition */

  rxlm_status = rxlm_obj->release_lock(rxlm_handle, &rflm_dtr_handle_locks[rxlm_handle]);  


  RFLM_DIAG_TRACE_MED(RFLM_DIAG_MSG_SSID_NONE, "rxlm_release_lock Handle = %d, Status = %d.",
                    rxlm_handle, rxlm_status);
  #endif /* !TEST_FRAMEWORK */
  return rxlm_status;
}
#endif

