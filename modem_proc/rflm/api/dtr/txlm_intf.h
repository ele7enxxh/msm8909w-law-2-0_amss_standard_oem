#ifndef TXLM_INTF_H
#define TXLM_INTF_H

/*!
   @file
   txlm_intf.h

   @brief
   This file implements the Interface APIs to TxLM functionality. These are
   platform independent.

*/

/*===========================================================================

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/dtr/txlm_intf.h#1 $ 

when       who     what, where, why
------------------------------------------------------------------------------- 
08/04/14   dr      Adding support for foundry dependent IREF gain programming 
06/19/14   dr      Adding new APIs for lm buffer allocation/de-allocation
02/13/14   st      DAC Temperature Compensation Support
02/13/14   cvd     Increase number of DTR-Tx handles to 4, total DTR handles to 18
11/06/13   st      LMEM Info Storage for DRIF Group A/B
10/16/13   st      DAC Control Sequences
10/14/13   st      Reloc IREF LUT Type Definition
10/04/13   st      Bolt DTR-TX New APIs
08/27/13   jf      Added interface for txlm register dump 
08/21/13   cvd     Make TXLM_MAX_BUFFERS 16 for Bolt
04/01/13   dej     Add dynamic settings tables api to TxLM
03/11/13   sar     Updates for APQ, GNSS only target.
12/21/12   cd      All possible 5 TxLM buffers can be allocated to chain 0, as 
                   buffer allocation is chain-agnostic
12/03/12   aca     DIME Dac cal
01/03/12   aro     Added enumeration for TxLM DAC
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
08/04/11   cd      Support for SW Dynamic Updates to LM buffer
                   Support for reading portions of LM buffer
05/26/11   aro     Fixed the TxLM buffer allocation bug which caused buffer
                   mismatch between RF and FW
03/24/11   vrb     LM Intf changes for CMI Compliance 
02/23/11   sty     Fixed warnings
01/21/11   sr      code cleanup based on the code review.
12/22/10   vrb     Updated Signature for rxlm_update_buffer_state
12/14/10   sr      Initial version.
============================================================================*/ 

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


#include "lm_types.h"

/*----------------------------------------------------------------------------*/
#ifdef FEATURE_D3925_FED
#define TXLM_MAX_BUFFERS_PER_CHAIN 5
#else
#define TXLM_MAX_BUFFERS_PER_CHAIN 18
#endif

/*----------------------------------------------------------------------------*/
#define TXLM_MAX_BUFFERS 18

#define RFLM_DTR_TX_DAC_IREF_LUT_LENGTH_AG 20
#define TXDAC_IREF_LUT_LENGTH RFLM_DTR_TX_DAC_IREF_LUT_LENGTH_AG

/*----------------------------------------------------------------------------*/

typedef enum 
{
  TXLM_CHAIN_0,
  TXLM_CHAIN_1,
  TXLM_CHAIN_MAX,
  TXLM_CHAIN_NUM = TXLM_CHAIN_MAX
} txlm_chain_type;

/*----------------------------------------------------------------------------*/
typedef enum 
{
  TXLM_DAC_0,
  TXLM_DAC_1,
  TXLM_DAC_MAX,
  TXLM_DAC_NUM = TXLM_DAC_MAX
} txlm_dac_type;

/*----------------------------------------------------------------------------*/

typedef enum
{
  TXLM_SEQ_DAC_XO_GATE_ENABLE,
  TXLM_SEQ_DAC_XO_GATE_DISABLE,
  TXLM_SEQ_FCAL_CLK_ENABLE,
  TXLM_SEQ_FCAL_CLK_DISABLE, 
  TXLM_SEQ_TXDAC_PWR_DOWN,
  TXLM_SEQ_TXDAC_REACTIVATE,  
  TXLM_SEQ_ETDAC_PWR_DOWN,
  TXLM_SEQ_ETDAC_REACTIVATE,  
  TXLM_SEQ_DAC_IREF_CFG,
  TXLM_SEQ_DAC_IREF_READ_FUSEFLAG,
  TXLM_SEQ_DAC_IREF_READ_QFUSE,  
  TXLM_SEQ_DAC_IREF_READ_OVERFLOW_BIT,
  TXLM_SEQ_DAC_IREF_READ_AVG_ERROR,
  TXLM_SEQ_TYPE_MAX
} txlm_seq_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint16 band;
  uint32 fs_clk;
} txlm_cfg_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint32 freq;
} txlm_dyn_cfg_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 txlm_max_chains;
  uint8 max_buffers[TXLM_CHAIN_MAX];
  uint32 *mem_addrs[TXLM_CHAIN_MAX][TXLM_MAX_BUFFERS_PER_CHAIN];
  uint32 buf_size[TXLM_CHAIN_MAX][TXLM_MAX_BUFFERS_PER_CHAIN];
} txlm_config_type;

/*----------------------------------------------------------------------------*/
typedef struct {
    uint32 iref_val_uA10;
    uint32 txdac_iref_cfg2_mask;
    uint32 txdac_iref_cfg2_shft;
    uint32 txdac_iref_cfg3_mask;
    uint32 txdac_iref_cfg3_shft;
}txdac_iref_t;
/*----------------------------------------------------------------------------*/
typedef struct {
    uint32 txdac_cfg2_mask;
    uint32 txdac_cfg2_shft_val;
    uint32 txdac_cfg3_mask;
    uint32 txdac_cfg3_shft_val;
    boolean txdac_iref_update_enable;
    uint32 txdac_iref_update_start;
    uint32 txdac_iref_update_stop;
}txdac_iref_cfg_t;

typedef struct {
	int32 slope;
	uint32 slope_qfactor;
	int32 ref_temp;
	int32 max_temp;
	int32 min_temp;
	uint32 tsens_id;
}dac_therm_info_t;

typedef struct
{
  uint32 reserved;
} rflm_dtr_tx_get_shared_mem_input_t; 

typedef struct
{
  uint32 mem_groupA;
  uint32 mem_groupB;
} rflm_dtr_tx_get_shared_mem_output_t;

/*----------------------------------------------------------------------------*/
lm_status_type txlm_init(void);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_allocate_buffer(txlm_chain_type chain, lm_tech_type tech, 
                                    lm_handle_type *txlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rflm_allocate_tx_handle(lm_tech_type client_tech, lm_tech_type operation_tech, 
                                       lm_handle_type *rflm_tx_handle);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_deallocate_buffer(lm_handle_type txlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type rflm_deallocate_tx_handle(lm_tech_type client_tech, lm_tech_type operation_tech,
                                          lm_handle_type rflm_tx_handle);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_update_static_settings(lm_handle_type txlm_handle, int32 device, void *cfg);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_update_dynamic_settings(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_get_buffer_content(lm_handle_type txlm_handle, uint32 group_id, uint8 *addr);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_log(void);

/*----------------------------------------------------------------------------*/
txlm_chain_type txlm_get_chain(lm_handle_type txlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_update_dynamic_group_struct(uint32 dyn_functionality, uint32 group_id, uint8 *addr, uint32 table_index);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_set_up_dac_cal(void *cfg);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_dac_cal_cmd(uint32 dac_cal_cmd_enum, uint8 size, uint32 *data);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_execute_sequence(lm_handle_type txlm_handle, uint32 sequence_id, uint32 *txlm_seq_ret_val);

/*----------------------------------------------------------------------------*/
void txlm_set_lmem_info(txlm_chain_type chain, boolean drif_group_A, uint32* lmem_start_addr);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_retrieve_lmem_info(txlm_chain_type chain, uint32** groupA_drif_start_addr, uint32** groupB_drif_start_addr);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_get_shared_mem_addr(lm_handle_type txlm_handle, const rflm_dtr_tx_get_shared_mem_input_t* in, rflm_dtr_tx_get_shared_mem_output_t* out);

/*----------------------------------------------------------------------------*/

lm_status_type txlm_get_shared_mem_addr_dev(int32 device, const rflm_dtr_tx_get_shared_mem_input_t* in, rflm_dtr_tx_get_shared_mem_output_t* out);

/*----------------------------------------------------------------------------*/

lm_status_type txlm_set_shared_mem_addr( uint32 mem0_groupA, uint32 mem1_groupA, uint32 mem0_groupB, uint32 mem1_groupB);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_retrieve_dac_iref_lut
    (
      const uint8 *iref_lut,
      uint8 length
    );

/*----------------------------------------------------------------------------*/
lm_status_type txlm_config_dac_iref( 
                                      lm_handle_type handle,
                                      boolean update_en,
                                      uint32  iref_lut_idx,
                                      uint32 iref_update_start_time,
                                      uint32 iref_update_stop_time
                                    );
/*----------------------------------------------------------------------------*/
lm_status_type txlm_retrieve_dac_therm_info(dac_therm_info_t* dac_therm_info);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_acquire_lock(lm_handle_type txlm_handle);

/*----------------------------------------------------------------------------*/
lm_status_type txlm_release_lock(lm_handle_type txlm_handle);

/*----------------------------------------------------------------------------*/
uint32 txlm_get_foundry_id(void);

/*----------------------------------------------------------------------------*/



#ifdef __cplusplus
}
#endif

#endif /* TXLM_INTF_H */

