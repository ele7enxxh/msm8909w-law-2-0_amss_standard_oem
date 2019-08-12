/*!
   @file
   rxlm.cpp

   @brief
   This file implements the RXLM functionality.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/src/rxlm.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
08/05/14   aw      fixed device_id typo
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
04/18/14   sar     Updated type for modem_version.
02/11/14   ak      Removing the older SMEM interface
01/09/14   dej     Implement Common Notch allocation for Bolt/Bolt+
12/19/13   dej     Add valid handle check for allocation
12/12/13   ka      Add enum support for FBRx
11/21/13   cvd     Add Logging messages
11/11/13   jyu     Added enum support for TDSCDMA
11/08/13   st      Crit Sect FW Standalone support
10/07/13   cvd     Add checks for handle type
09/26/13   JJ      Remove RFLM_USR_C2K
08/08/13   vrb     Integrate with RFLM: DM
07/08/13   dej     Added APIs for common notch allocation\deallocation
05/06/13   dej     Add critical section for LM
01/24/13   dej     Update static settings based on modem version
11/19/12   dej     Implement API to copy settings from SW Dynamic tables into group structs
05/15/12   dej     Call rxlm_init_ag in rxlm constructor
11/16/11   vrb     Added new API rxlm_get_chain to get rxlm_chain from buf_idx
05/02/11   vrb     Adding status checks before LM buffer accesses
04/21/11   vrb     Support for Reading portions of LM Buffer
04/07/11   vrb     Support for SW Dynamic Updates to LM Buffer
03/04/11   aro     Fixed get_buffer_addr function
02/28/11   aro     Added support to perform SW to FW struct conversion
01/21/11   sr      Code cleanup based on the code review.
01/12/11   bmg     Changed buffer types to lm_handle_type
01/03/11   bmg     Added RxLM destructor to free buffer memory
12/28/10   sr      Added txlm functionality and changed the rxlm to use 
                   common lm_types and lm_buffer.
12/22/10   vrb     Updated Signature for rxlm_update_buffer_state
12/14/10   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rflm.h"
#include "rxlm.h"
#include "rxlm_rxf_hal.h"
#include "msg.h"
#include "rflm_dtr.h"

#include "rflm_dm_api.h"
#ifndef FEATURE_D3925_FED
#include "rflm_dtr_rx_struct_ag.h"
#include "rflm_dtr_rx_common_resources.h"
#else
#include "fw_rxlm_intf.h"
#endif
#include <stringl/stringl.h>

/* To be removed or updated based on getting tech from RFLM ; 
Require API to get tech from RFLM */
static lm_tech_type lm_client_tech_of_rflm_handle[RFLM_HANDLE_COUNTS];
static lm_tech_type lm_op_tech_of_rflm_handle[RFLM_HANDLE_COUNTS];

rxlm* rxlm::rxlm_ptr = (rxlm*)NULL;

dtr_verbose_level rxlm::rxlm_verbose_level = DTR_VERBOSE_LEVEL_1;

#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
rf_lock_data_type rxlm::rxlm_critical_section;
#endif

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Provides the rxlm object pointer.
 
   @details
      returns the rxlm object ptr, with which clients can communicate with rxlm
      to allocate/deallocate and update the rxlm buffers.

   @retval rxlm singleton object ptr.
   
*/
rxlm* rxlm::get_instance(void)
{
  if (!rxlm_ptr)
  {
    rxlm_ptr = new rxlm();
  }

  return rxlm_ptr;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      rxlm constructor.
 
   @details
      constructs the rxlm object. Gets the rxf configuration information like
      number of rxf chains, buffers per each chain, address of each buffer and
      the size of each buffer from the rxf hal. Based on the rxf information the
      rxlm buffers will be created.
*/
rxlm::rxlm()
{
  uint8 buffer_id = 0;
  uint8 buffer = 0;
  uint8 chain = 0;
  uint8 max_buffers  = 0;
#ifndef FEATURE_TRITON_MODEM
  uint32 modem_version;
#endif /* FEATURE_TRITON_MODEM */

  /* initilize the rxlm buffers with NULL */
  for (int i = 0; i < RXLM_MAX_BUFFERS; i++)
  {
    rxlm_buff_ptr[i] = NULL;
  }

  /* the modem specific RXF configuration information */
  rxlm_rxf_config_type* rxlm_rxf_config = rxlm_hal_rxf_config_get();

  /* initialize the rxlm buffers based on the RXF configuration */
  for (chain = 0; chain < rxlm_rxf_config->rxlm_max_chains; chain++)
  {
    max_buffers = rxlm_rxf_config->max_buffers[chain];
    for (buffer = 0; buffer < max_buffers; buffer++)
    {
      rxlm_buff_ptr[buffer_id] = new lm_buffer((int32)chain, buffer_id,
                                               (uint8*)rxlm_rxf_config->mem_addrs[chain][buffer],
                                               rxlm_rxf_config->buf_size[chain][buffer]);
      buffer_id++;
    }
  }
  rxlm_init_ag();
#ifndef FEATURE_TRITON_MODEM
  modem_version = rxlm_hal_rxf_get_modem_version();
  rxlm_modem_ver_init_ag(modem_version);
  rflm_dtr_rx_common_notch_init();
#endif /* FEATURE_TRITON_MODEM */

#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_init_critical_section(&rxlm_critical_section);
#endif
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      rxlm destructor.
 
   @details
      Frees buffer memory that was allocated when the RxLM object was created.
*/
rxlm::~rxlm()
{

  /* clean-up the rxlm buffers */
  for (int i = 0; i < RXLM_MAX_BUFFERS; i++)
  {
    if (rxlm_buff_ptr[i] != NULL)
    {
      delete rxlm_buff_ptr[i];
      rxlm_buff_ptr[i] = NULL;
    }
  }
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_deinit_critical_section(&rxlm_critical_section);
#endif

  rxlm_ptr = NULL;
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
           '*rxlm_handle' : the rxlm handle which is reserved.

   @retval LM_SUCCESS if the buffer is allocated sucessfully, otherwise returns
   the appropriate error status.
   
*/
lm_status_type rxlm::allocate_buffer(rxlm_chain_type chain, lm_tech_type client_tech,
                                     lm_tech_type operation_tech, lm_handle_type* rxlm_handle)
{
  lm_status_type status = LM_SUCCESS;

  rflm_dm_handle_meta_s meta_data = { 0 };
  meta_data.type = RFLM_RX_HANDLE;
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

  case LM_FBRX:
    meta_data.owner = RFLM_USR_FBRX;
    break;

  default:
    meta_data.owner = RFLM_NO_USR;
    break;
  }
  /* arguments checking */

  if (rxlm_handle == NULL)
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::allocate_buffer: rxlm_handle == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  if (chain >= RXLM_CHAIN_MAX || operation_tech >= LM_TECH_MAX)
  {
    *rxlm_handle = LM_INVALID_BUFFER_INDEX;
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::allocate_buffer: chain >= RXLM_CHAIN_MAX || operation_tech >= LM_TECH_MAX.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  *rxlm_handle = (lm_handle_type)rflm_dm_allocate_handle(&meta_data);

  if ((lm_handle_type)RFLM_DM_INVALID_HANDLE_ID == *rxlm_handle)
  {
    status = LM_BUFFERS_UNAVAILABLE;
  } else
  {
    lm_op_tech_of_rflm_handle[*rxlm_handle] = operation_tech;
    lm_client_tech_of_rflm_handle[*rxlm_handle] = client_tech;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Un-reserves a rxlm buffer which was previously reserved.
 
   @details
      un-reserves a rxlm buffer which was reserved previously. The associated
      buffer will become available to reserve it for clients.

   @param  'rxlm_handle' : the buffer associated with the rxlm_handle to be
                           un-reserved.

   @retval LM_SUCCESS if the buffer is de-allocated sucessfully, otherwise
   returns the appropriate error status.
   
*/
lm_status_type rxlm::deallocate_buffer(lm_handle_type rxlm_handle, lm_tech_type client_tech,
                                       lm_tech_type operation_tech)
{
  lm_status_type status = LM_SUCCESS;

  /*Printing the techs at the beginning of de-allocation so that we dont have to print during crash again.*/
  RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_NONE, "Handle = %d, Allocating client tech : %d, deallocating client tech : %d", rxlm_handle, lm_client_tech_of_rflm_handle[rxlm_handle], client_tech);
  RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_NONE, "Handle = %d, Allocating operation tech : %d, deallocating operation tech : %d", rxlm_handle, lm_op_tech_of_rflm_handle[rxlm_handle], operation_tech);

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::deallocate_buffer: Handle = %d, Invalid handle type.", rxlm_handle);
    }

    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }
  /*-----
     Check to ensure 
     a.) using the same pair of allocating/deallocating functions(Till both new and old functions exist simultaneously). 
     b.) mismatch in client and/or operation techs while allocating / de-allocating
   -----*/
  if ((client_tech == lm_client_tech_of_rflm_handle[rxlm_handle]) && ((client_tech == LM_TECH_MAX) || (operation_tech == lm_op_tech_of_rflm_handle[rxlm_handle])))
  {
    rflm_dm_set_populated_flag((rflm_dm_handle_id_t)rxlm_handle, FALSE);

    if (rflm_dm_deallocate_handle((rflm_dm_handle_id_t)rxlm_handle))
    {
      status = LM_SUCCESS;
    } 
    else
    {
      if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
      {
        DTR_DIAG_TRACE_MSG_ERROR("rxlm::deallocate_buffer: Handle = %d, rflm_dm_deallocate_handle failed.", rxlm_handle);
      }

      status = LM_INVALID_PARAMETER;
    }
  }
  /*------
  Crash if the above condition is not met.
  --------*/
  else
  {
    RFLM_DIAG_ERR_FATAL("Mismatch in allocating/de-allocating client or operation techs: Handle = %d ", rxlm_handle);
  }
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update the buffer with RXF static settings based on the cfg parameters.
 
   @details
      Update the contents of rxlm buffer which was reserved previously with new
      RXF settings based on the new configuration parameters. 

   @param  'rxlm_handle'  : the buffer associated rxlm_handle to be updated.
           'cfg'          : new configuration parameters based on which
                            rxlm buffer need to be updated.

   @retval LM_SUCCESS if the buffer is updated with static settings, otherwise
   returns the appropriate error status.
*/
lm_status_type rxlm::update_static_settings(lm_handle_type rxlm_handle, void* cfg)
{
  lm_status_type status = LM_SUCCESS;
  uint32 addr = 0;
  uint32 size = 0;
  rxlm_chain_type chain = RXLM_CHAIN_MAX;
  lm_tech_type tech = LM_TECH_MAX;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_static_settings: Invalid handle type. Handle = %d", rxlm_handle);
    }
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

  uint8* dest_addr = 0;
  tech = lm_op_tech_of_rflm_handle[rxlm_handle];
  if (cfg == NULL)
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_static_settings: cfg == NULL. Handle = %d", rxlm_handle);
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  status = rxlm_init_settings_get(chain, tech, cfg, &addr, &size);

  if (LM_SUCCESS == status)
  {
    if (size <= sizeof(rflm_dtr_rx_settings_type_ag))
    {
      dest_addr = (uint8*)rflm_dm_get_dtr_buf_addr((rflm_dm_handle_id_t)rxlm_handle);
      if (dest_addr == NULL)
      {
        if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
        {
          DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_static_settings: dest_addr == NULL. Handle = %d", rxlm_handle);
        }
        status = LM_INVALID_PARAMETER;
        return status;
      }
      memscpy(dest_addr, size, (void*)addr, size);
    }

    rflm_dm_set_populated_flag(rxlm_handle, TRUE);
  } else
  {
    DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_static_settings: rxlm_init_settings_get() failed. Handle = %d", rxlm_handle);
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update rxlm buffer with RXF dynamic settings based on cfg parameters.
 
   @details
      Update the dynamic part of the rxlm buffer which was reserved previously
      with new settings based on the new configuration parameters.

   @param  'rxlm_handle'   : the buffer associated rxlm_handle to be updated.
           'group_id'     : Enum/ID indicates which dynamic group to be updated.
           'addr'         : Address of the populated dynamic structure from SW
                            to be mem-copied into the shared mem buffer

   @retval LM_SUCCESS if the buffer is updated with dynamic settings, otherwise
   returns the appropriate error status.
   
*/
lm_status_type rxlm::update_dynamic_settings(lm_handle_type rxlm_handle, uint32 group_id, uint8* addr)
{
  lm_status_type status = LM_SUCCESS;
  uint32 size = 0;
  uint32 offset = 0;
  uint8* dest_addr = 0;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_dynamic_settings: Invalid handle type.");
    }
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

  status = rxlm_dynamic_group_offset_addr_get(group_id, &offset, &size);

  /*Log addr*/
  if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_2)
  {
    dtr_print_memory_chunk(addr, size);
  }

  if (LM_SUCCESS == status)
  {
#ifndef FEATURE_D3925_FED
    if ((size + offset) <= sizeof(rflm_dtr_rx_settings_type_ag))
    {
      dest_addr = (uint8*)rflm_dm_get_dtr_buf_addr((rflm_dm_handle_id_t)rxlm_handle);
      if (dest_addr == NULL)
      {
        if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
        {
          DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_dynamic_settings: dest_addr == NULL.");
        }
        status = LM_INVALID_PARAMETER;
        return status;
      }
      dest_addr += offset;
      memscpy(dest_addr, size, (void*)addr, size);
    }
#else
    if ((size + offset) <= sizeof(fw_rxlm_settings_type_ag))
    {
      dest_addr = (uint8*)FW_RXLM_BUF_ADDR(rxlm_handle);
      if (dest_addr == NULL)
      {
        if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
        {
          DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_dynamic_settings: dest_addr == NULL.");
        }
        status = LM_INVALID_PARAMETER;
        return status;
      }
      dest_addr += offset;
      memscpy(dest_addr, size, (void*)addr, size);
    }
#endif
  }

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
      Return buffer contents for a portion of lm buffer.
 
   @details
      Returns buffer contents for a portion of lm buffer. This functionality
      is required when a portion of the dynamic settings is requiered to be
      updated; ex: Only 2 of the 6 notch filters need to be updated by SW.

   @param  'rxlm_handle'   : the buffer associated rxlm_handle to be updated.
           'group_id'     : Enum/ID indicates which dynamic group to be updated.
           'addr'         : Address of the populated dynamic structure from SW
                            to be mem-copied into the shared mem buffer

   @retval LM_SUCCESS if the buffer is read, otherwise
   returns the appropriate error status.
   
*/
lm_status_type rxlm::get_buffer_content(lm_handle_type rxlm_handle, uint32 group_id, uint8* addr)
{
  lm_status_type status = LM_SUCCESS;
  uint32 size = 0;
  uint32 offset = 0;
  uint8* buf_src_addr = 0;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::get_buffer_content: Invalid handle type.");
    }
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

  if (addr == NULL)
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::get_buffer_content: addr == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  status = rxlm_dynamic_group_offset_addr_get(group_id, &offset, &size);

  if (LM_SUCCESS == status)
  {
#ifndef FEATURE_D3925_FED
    if ((size + offset) <= sizeof(rflm_dtr_rx_settings_type_ag))
    {
      buf_src_addr = (uint8*)rflm_dm_get_dtr_buf_addr((rflm_dm_handle_id_t)rxlm_handle);
      if (buf_src_addr == NULL)
      {
        if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
        {
          DTR_DIAG_TRACE_MSG_ERROR("rxlm::get_buffer_content: buf_src_addr == NULL.");
        }
        status = LM_INVALID_PARAMETER;
        return status;
      }
      buf_src_addr += offset;
      memscpy((void*)addr, size, buf_src_addr, size);
    }
#else
    if ((size + offset) <= sizeof(fw_rxlm_settings_type_ag))
    {
      buf_src_addr = (uint8*)FW_RXLM_BUF_ADDR(rxlm_handle);
      if (buf_src_addr == NULL)
      {
        if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
        {
          DTR_DIAG_TRACE_MSG_ERROR("rxlm::get_buffer_content: buf_src_addr == NULL.");
        }
        status = LM_INVALID_PARAMETER;
        return status;
      }
      buf_src_addr += offset;
      memscpy((void*)addr, size, buf_src_addr, size);
    }
#endif

    /*Log addr*/
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_2)
    {
      dtr_print_memory_chunk(addr, size);
    }
  }

  return status;
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
rxlm_chain_type rxlm::get_chain(lm_handle_type rxlm_handle)
{
  rxlm_chain_type rxlm_chain = RXLM_CHAIN_MAX;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    return rxlm_chain;
  }

  if (rxlm_buff_ptr[rxlm_handle] != NULL)
  {
    rxlm_chain = (rxlm_chain_type)rxlm_buff_ptr[rxlm_handle]->get_chain();
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
lm_status_type rxlm::update_dynamic_group_struct(uint32 dyn_functionality, uint32 group_id, uint8* addr, uint32 table_index)
{
  lm_status_type status = LM_SUCCESS;

  if (addr == NULL)
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::update_dynamic_group_struct: addr == NULL.");
    }
    status = LM_INVALID_PARAMETER;
    return status;
  }

  status = rxlm_update_dynamic_group_struct_ag(dyn_functionality, group_id, addr, table_index);

  return status;
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
lm_status_type rxlm::allocate_common_notch(lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32* cmn_notch_dyn_block_id)
{
  lm_status_type status = LM_SUCCESS;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::allocate_common_notch: Invalid handle type.");
    }
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_enter_critical_section(&rxlm::rxlm_critical_section);
#endif

  status = rflm_dtr_rx_common_notch_allocate(rxlm_handle, chain, cmn_notch_dyn_block_id);

#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_leave_critical_section(&rxlm::rxlm_critical_section);
#endif

  return status;
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
lm_status_type rxlm::deallocate_common_notch(lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32 cmn_notch_dyn_block_id)
{
  lm_status_type status = LM_SUCCESS;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    if (rxlm::rxlm_verbose_level > DTR_VERBOSE_LEVEL_0)
    {
      DTR_DIAG_TRACE_MSG_ERROR("rxlm::deallocate_common_notch: Invalid handle type.");
    }
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_enter_critical_section(&rxlm::rxlm_critical_section);
#endif

  status = rflm_dtr_rx_common_notch_deallocate(rxlm_handle, chain, cmn_notch_dyn_block_id);

#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  rf_common_leave_critical_section(&rxlm::rxlm_critical_section);
#endif

  return status;
}

/*----------------------------------------------------------------------------*/
lm_status_type rxlm::acquire_lock(lm_handle_type rxlm_handle, rflm_rw_lock_t* rflm_dtr_handle_lock)
{
  lm_status_type status = LM_SUCCESS;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    DTR_DIAG_TRACE_MSG_ERROR("rxlm::acquire_lock: Invalid handle type.");
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

  rflm_rw_lock_write_lock(rflm_dtr_handle_lock);

  return status;
}

/*----------------------------------------------------------------------------*/
lm_status_type rxlm::release_lock(lm_handle_type rxlm_handle, rflm_rw_lock_t* rflm_dtr_handle_lock)
{
  lm_status_type status = LM_SUCCESS;

  if (!rflm_dm_validate_handle_type(rxlm_handle, RFLM_RX_HANDLE))
  {
    DTR_DIAG_TRACE_MSG_ERROR("rxlm::release_lock: Invalid handle type.");
    status = LM_INVALID_BUFFER_INDEX;
    return status;
  }

  rflm_rw_lock_write_unlock(rflm_dtr_handle_lock);

  return status;
}

#endif

