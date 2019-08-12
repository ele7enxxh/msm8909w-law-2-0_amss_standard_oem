#ifndef RXLM_RXF_HAL_H
#define RXLM_RXF_HAL_H

/*!
   @file
   rxlm_rxf_hal.h

   @brief
   This file implements the RXLM MDM9K HAL functionality.
   These are platform dependent.

*/

/*===========================================================================

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/inc/rxlm_rxf_hal.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/18/14   sar     Updated return val or rxlm_hal_rxf_get_modem_version and 
                   input param for rxlm_modem_ver_init_ag functions.
01/24/13   dej     Update static settings based on modem version
05/14/12   dej     Support QFuse specific updates to static buffers during rxlm init  
04/07/11   vrb     Support for SW Dynamic Updates to LM Buffer
03/24/11   vrb     LM Intf changes for CMI Compliance
03/04/11   pl      Added bbrx support
02/28/11   aro     Added support to perform SW to FW struct conversion
01/21/11   sr      code cleanup based on the code review.
01/17/11   bmg     Moved rxlm_rxf_init_setting_get to common HAL
12/22/10   vrb     Added extern functions for the mock fw
12/14/10   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "comdef.h"
#include "rxlm_intf.h"
extern rxlm_rxf_config_type *rxlm_hal_rxf_config_get(void);

extern void rxlm_hal_rxf_config_cleanup(void);

extern void mock_fw_configure_chain(uint32 buffer_index, uint32 bw);
extern void mock_fw_disable_chain(uint32 buffer_index);

extern uint32 *rxlm_hal_rxf_get_buffer_addr(uint32 buffer_index);

/*----------------------------------------------------------------------------*/
lm_status_type rxlm_init_settings_get ( rxlm_chain_type chain, lm_tech_type tech,
                                           void *cfg, uint32 *addr, uint32 *size);


lm_status_type rxlm_dynamic_group_offset_addr_get ( 
            uint32 dyn_group ,
            uint32 *offset_addr ,
            uint32 *size 
            ) ;

void rxlm_init_ag ( void );

void rxlm_modem_ver_init_ag (uint32 modem_ver_id);

uint32 rxlm_hal_rxf_get_modem_version( void );

lm_status_type rxlm_update_dynamic_group_struct_ag (
            uint32 dyn_functionality,
            uint32 dyn_group ,
            uint8 * target_struct ,
            uint32 table_index 
            );

#ifndef FEATURE_TRITON_MODEM
lm_status_type rxlm_rxf_hal_allocate_common_notch( lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32* cmn_notch_dyn_block_id );

lm_status_type rxlm_rxf_hal_deallocate_common_notch( lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32 cmn_notch_dyn_block_id );
#endif

#ifdef __cplusplus
}
#endif

#endif /* RXLM_RXF_HAL_H */

