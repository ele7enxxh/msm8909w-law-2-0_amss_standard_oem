#ifndef RXLM_INTF_H
#define RXLM_INTF_H

/*!
   @file
   rxlm_intf.h

   @brief
   This file implements the Interface APIs to RxLM functionality.
   These are platform independent.
 
   @msc
        hscale="1.5", wordwraparcs="true";
 
        L1, RFA, RxLM, SwAutoGen, mdspFw, FwAutoGen;
        |||;
 
        --- [label = "enter mode" ];
        L1=>RxLM [label = "rxlm_allocate_buffer(chain, tech, &rxlm_handle)"];
        L1<<RxLM [label = "lm_status_type"];
        L1=>RFA  [label = "rfm_enter_mode(rxlm_handle)"];
 
        --- [label = "RXF Init settings to be configured"];
        |||;
        RFA=>RxLM [label = "rxlm_update_static_settings(rxlm_handle,rxlm_config)"];
        RxLM=>SwAutoGen [label = "rxlm_rxf_init_setting_get(chain, tech, rxlm_config)"];
        RxLM<<SwAutoGen [label = "return addr of autogenTbl and Size"];
        RxLM box RxLM [label = "copy autogen table into shared memory"];
        RFA<<RxLM     [label = "lm_status_type"];
 
        --- [label = "time to configure the RXF settings"];
        |||;
        L1->mdspFw    [label = "FW CMD (rxlm_handle)"];
        mdspFw=>FwAutoGen [label = "config_rxlm_settings(rxlm_handler)"];
        FwAutoGen box FwAutoGen [label = "HWIO_OUT modem settings based on rxlm_handler"];
 
        --- [label = "Few RXF settings change required"];
        |||;
        RFA=>RxLM [label = "rxlm_update_dynamic_settings(rxlm_handle,rxlm_dyn_config)"];
        RxLM=>SwAutoGen [label = "rxlm_rxf_dynamic_setting_get(chain, tech, rxlm_dyn_config)"];
        RxLM<<SwAutoGen [label = "return addr of changed autogenTbl and Size"];
        RxLM box RxLM [label = "copy autogen table into shared memory"];
        RFA<<RxLM     [label = "lm_status_type"];
        RFA->mdspFw   [label = "FW CMD (rxlm_handler)"];
        mdspFw=>FwAutoGen [label = "config_rxlm_dyn_settings(rxlm_handler)"];
        FwAutoGen box FwAutoGen [label = "HWIO_OUT changed modem settings based on rxlm_handler"];
        --- [label = "DONE"];
 
 
  @endmsc
  
*/

/*===========================================================================

Copyright (c) 2010 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/dtr/rxlm_intf.h#1 $ 
 

when       who     what, where, why
------------------------------------------------------------------------------- 
12/08/14   dr      Tabasco featurization for RxLM - FBRX
09/17/14   dr      Adding Jolokia featurization
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
02/13/14   cvd     Increase number of DTR-Tx handles to 4, total DTR handles to 18
10/03/13   vrb     Removing featurization for branched out modems
08/27/13   jf      Added interface for rxlm register dump  
08/21/13   cvd     Make RXLM_MAX_BUFFERS 16 for Bolt
07/30/13   dej     Increase buffer allocation to 11
06/28/13   dej     Added APIs for common notch allocation\deallocation
09/01/13   sc      Added invalid RXLM buffer definition
11/19/12   dej     Implement API to copy settings from SW Dynamic tables into group structs 
05/31/12   vrb     Updates for Dime Compatibility
01/03/12   aro     Fixed max RXLM ADC number
12/29/11   aro     Added enumeration for RxLM ADC
11/16/11   vrb     Added new API rxlm_get_chain to get rxlm_chain from buf_idx
04/21/11   vrb     Support for Reading portions of LM Buffer
04/07/11   vrb     Support for SW Dynamic Updates to LM Buffer
03/24/11   vrb     LM Intf changes for CMI Compliance 
02/23/11   sty     Fixed warnings
01/26/11   sr      added the mscgen diagrams for documentation.
01/21/11   sr      code cleanup based on the code review.
12/28/10   sr      Added txlm functionality and changed the rxlm to use 
                   common lm_types and lm_buffer.
12/22/10   vrb      Updated Signature for rxlm_update_buffer_state
12/14/10   sr      Initial version.
============================================================================*/ 

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "lm_types.h"



#ifndef FEATURE_D3925_FED
#define RXLM_RXF_MAX_BUFFERS_PER_CHAIN 18
#else
#define RXLM_RXF_MAX_BUFFERS_PER_CHAIN 11
#endif
#define RXLM_MAX_BUFFERS 18


#define RXLM_INVALID_BUFFER 0xDEADD00D

/*----------------------------------------------------------------------------*/
#if (!(defined(FEATURE_DIMEPM_MODEM) || defined (FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
typedef enum  /* Bolt, Bolt+, Thor */
{
  RXLM_CHAIN_0,
  RXLM_CHAIN_1,
  RXLM_CHAIN_2,
  RXLM_CHAIN_3,
  RXLM_CHAIN_FBRX,
#if defined(FEATURE_BOLT_PLUS_MODEM) || defined(T_MDM9X45)
  RXLM_CHAIN_5,
  RXLM_CHAIN_6,
#endif
  RXLM_CHAIN_MAX,
  RXLM_CHAIN_NUM = RXLM_CHAIN_MAX
} rxlm_chain_type;
#else
typedef enum  /*DIMEPM or JOLOKIA  */
{
  RXLM_CHAIN_0,
  RXLM_CHAIN_1,
  RXLM_CHAIN_2,
  RXLM_CHAIN_3,
  RXLM_CHAIN_MAX,
  RXLM_CHAIN_NUM = RXLM_CHAIN_MAX
} rxlm_chain_type;
#endif


/*----------------------------------------------------------------------------*/
typedef enum 
{
  RXLM_ADC_0,
  RXLM_ADC_1,
  RXLM_ADC_2,
  RXLM_ADC_3,
  RXLM_ADC_MAX,
  RXLM_ADC_NUM = RXLM_ADC_MAX
} rxlm_adc_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 rxlm_max_chains;
  uint8 max_buffers[RXLM_CHAIN_MAX];
  uint32 *mem_addrs[RXLM_CHAIN_MAX][RXLM_RXF_MAX_BUFFERS_PER_CHAIN];
  uint32 buf_size[RXLM_CHAIN_MAX][RXLM_RXF_MAX_BUFFERS_PER_CHAIN];
} rxlm_rxf_config_type;


/*----------------------------------------------------------------------------*/
lm_status_type rxlm_init(void);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_allocate_buffer(rxlm_chain_type chain, lm_tech_type tech, 
                                      lm_handle_type *rxlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rflm_allocate_rx_handle(lm_tech_type client_tech, lm_tech_type operation_tech,
                                       lm_handle_type *rflm_rx_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_deallocate_buffer(lm_handle_type rxlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rflm_deallocate_rx_handle(lm_tech_type client_tech, lm_tech_type operation_tech, 
                                         lm_handle_type rflm_rx_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_acquire_lock(lm_handle_type rxlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_release_lock(lm_handle_type rxlm_handle);

/*----------------------------------------------------------------------------*/

lm_status_type rxlm_update_static_settings(lm_handle_type rxlm_handle, void *cfg);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_update_dynamic_settings(lm_handle_type rxlm_handle, uint32 group_id, uint8 *addr);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_get_buffer_content(lm_handle_type rxlm_handle, uint32 group_id, uint8 *addr);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_log(void);

/*----------------------------------------------------------------------------*/
rxlm_chain_type rxlm_get_chain(lm_handle_type rxlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_update_dynamic_group_struct(uint32 dyn_functionality, uint32 group_id, uint8 *addr, uint32 table_index);

#ifndef FEATURE_TRITON_MODEM
/*----------------------------------------------------------------------------*/
lm_status_type rxlm_allocate_common_notch(lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32* cmn_notch_dyn_block_id);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_deallocate_common_notch(lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32 cmn_notch_dyn_block_id);
#endif

#ifdef __cplusplus
}
#endif

#endif /* RXLM_INTF_H */

