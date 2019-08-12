#ifndef _HSU_CONF_SEL_STACK_UTILS_H_
#define _HSU_CONF_SEL_STACK_UTILS_H_

/*=============================================================================

     High Speed USB Configuration Selector Stack Utilities
        H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_conf_sel_stack_utils.h

Defines functions that setup, stop, reconfigure, etc., the USB stack.

Externalized functions:
- hsu_conf_sel_stack_utils_init_memory
- hsu_conf_sel_stack_utils_setup_device_stack
- hsu_conf_sel_stack_utils_switch_device_stack
- hsu_conf_sel_stack_utils_handle_chg_connected
- hsu_conf_sel_stack_utils_handle_chg_disconnected
- hsu_conf_sel_stack_utils_enable_perf_boost
- hsu_conf_sel_stack_utils_device_restart_core
- hsu_conf_sel_stack_utils_is_forced_disable
- hsu_conf_sel_stack_utils_force_disable_start
- hsu_conf_sel_stack_utils_force_disable_stop
- hsu_conf_sel_stack_utils_disable
- hsu_conf_sel_stack_utils_enable

Initialization and sequencing requirements:\n
hsu_conf_sel_stack_utils_init_memory should be called before any other 
function, and before the CDC ALs are initialized.
*/
/*=============================================================================    
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_conf_sel_stack_utils.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10 lo  (Tech Pubs) Edited/added Doxygen comments and markup.
12/06/06 ke  Initial version
    
=============================================================================*/
#include "comdef.h"

#include "jslave_init.h"
#include "jtypes.h"

#include "hsu_conf_sel_types.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

/** Index for the high-speed device controller.
*/
#define HSU_DEVICE_CONTROLLER_IDX_0 0

/** Maximum device power in 2 mA units (0xFA = 250 units = 500 mA).
*/
#define HSU_DEVICE_MAX_POWER               0xFA

/** HS-USB configuration selector stack base address.\ Must be 32 bytes
    aligned.
*/
#define HSU_CFG_SELECTOR_STACK_BASE_ADDR  0x30000000

/*=============================================================================
  Macros
=============================================================================*/

/*=============================================================================
  Typedefs
=============================================================================*/
/** The callback to be called on completing _force_disable_start or
    _force_disable_stop.
    This type returns the result of the configuration process.
*/
typedef void (*hsu_conf_sel_stack_utils_force_disable_cb_type)(boolean result);

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_init_memory
=============================================================================*/
/**
Initializes variables related to the memory provided to the stack. 

@dependencies
Called once when the device stack is first setup, before ALs are initialized.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_stack_utils_init_memory(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_alloc_stack_memory
=============================================================================*/
/**
Called by adaptation layers to allocate stack memory.

If an AL must share data with the stack (for example, buffers for Tx/Rx), the
AL must request this memory from hsu_config_selector. It does so by calling
this function with the number of bytes required, and it uses the memory that
begins in the returned address.

@note
The addresses returned by this function are 32 bytes aligned, since the DMOV
requires buffers passed to it to be 32 bytes aligned.

@param required_space_in_bytes [IN] -- Space in bytes.

@dependencies
This function must be called prior to the stack bring-up (jstart_stack). The
memory can be used only until the AL's uninit function is called by the stack.

@sideeffects
None.

@return
The start address of the allocated memory, or NULL if there was not enough 
memory.
*/
byte * hsu_conf_sel_stack_utils_alloc_stack_memory
(
  uint32 required_space_in_bytes
);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_force_disable_start
=============================================================================*/
/**
Disables the USB core and sets the USB PHY in LPM.

@param *cb [IN] -- The callback for completion (of the type
                   hsu_conf_sel_stack_utils_force_disable_cb_type).

@dependencies
This function should be run in the AL TASK command queue.

@sideeffects
None.

@return
TRUE/FALSE returned by the callback function (the cb input parameter).
*/
void hsu_conf_sel_stack_utils_force_disable_start(void* cb);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_force_disable_stop
=============================================================================*/
/**
Starts the USB core,  activates it, and reconnects the PHY.
    
@param *cb [IN] -- The callback for completion (of the type
                   hsu_conf_sel_stack_utils_force_disable_cb_type).

@dependencies
This function should be run in the AL TASK command queue.

@sideeffects
None.

@return
TRUE/FALSE returned by the callback function (the cb input parameter).
*/
void hsu_conf_sel_stack_utils_force_disable_stop(void* cb);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_is_forced_disable
=============================================================================*/
/**
Returns whether the USB core is in forced_disable mode.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The USB core is enabled.\n
FALSE -- The USB core is disabled.
*/
boolean hsu_conf_sel_stack_utils_is_forced_disable(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_enable
=============================================================================*/
/**
Enables the USB core.

This function is called from hsu_conf_sel_stack_force_disable_stop and
hsu_analog_audio_stop.
   
@param core_index             [IN] -- Core index.
 
@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The USB core was enabled.\n
FALSE -- The USB core was not enabled.
*/
boolean hsu_conf_sel_stack_utils_enable(uint32 core_index);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_disable
==============================================================================*/
/**
Disables the USB core.

This function is called from hsu_conf_sel_stack_force_disable_start and 
hsu_analog_audio_start.

@param core_index             [IN] -- Core index.
    
@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The USB core was disabled.\n
FALSE -- The USB core was not disabled.
*/
boolean hsu_conf_sel_stack_utils_disable(uint32 core_index);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_switch_device_stack
=============================================================================*/
/**
Stops the stack and then reconfigures it to include a new set of FDs, as
described by the new composition structure.

This function registers reconfig_cb_func_ptr as the callback function to be
invoked when the stack reconfiguration process is completed. It should be
called after all applications using the old stack have been stopped.

@param core_idx             [IN] -- Core index.
@param reconfig_cb_func_ptr [IN] -- Callback to be called on reconfiguration
                                    completion.
@param *arg_for_reconfig_cb [IN] -- Argument to be sent to the callback.

@dependencies
Assumes that no application is using the stack at the time of the call and that 
all AL-related initializations have already been done.

@sideeffects
None.

@return
jresult_t -- HSU_JSUCCESS for success, other values for failure (currently all
cause an ASSERT).
*/
jresult_t hsu_conf_sel_stack_utils_switch_device_stack
(
 uint8                    core_idx,
  reconfig_complete_cb_t  reconfig_cb_func_ptr,
  void *      arg_for_reconfig_cb
);

/*=============================================================================
FUNCTION    hsu_is_dev_configured
=============================================================================*/
/**
Returns TRUE if the device is configured and FALSE if it is not configured.

@dependencies
  None.

@sideeffects
None.

@return
jbool_t -- dev_is_configured
*/
jbool_t hsu_is_dev_configured(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_handle_chg_connected
=============================================================================*/
/**
Handles a charger connected to an HS-USB port.

This function gets called by the OTG stack connect/disconnect mechanism 
implemented in the hsu_conf_sel_stack_utils.c file, and by a non-OTG connect/disconnect 
mechanism invoked in hsu_os_abstract_bus.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_stack_utils_handle_chg_connected(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_handle_chg_disconnected
=============================================================================*/
/**
Handles a charger disconnected from an HS-USB port.

This function gets called by the OTG stack connect/disconnect mechanism 
implemented in the hsu_conf_sel_stack_utils.c file, and by a non-OTG connect/disconnect 
mechanism invoked in hsu_os_abstract_bus.

@dependencies
  None.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_stack_utils_handle_chg_disconnected(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_enable_perf_boost
=============================================================================*/
/**
Configures various hardware registers for throughput optimizations.

@dependencies
The enabling of the various hardware registers is dependent on the hardware
version.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_stack_utils_enable_perf_boost(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_device_restart_core
=============================================================================*/
/**       
This function is the software workaround for a hardware bug regarding the
stuck PHY issue. 

On each disconnection, this function is called and restarts the core and the
PHY. It is also called when PHY access failed and the PHY is restarted.

@param *core_index_ptr [IN] -- Pointer to the core index.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_stack_utils_device_restart_core
(
 void* core_index_ptr
);

/*=============================================================================
FUNCTION    hsu_get_undp_hsu_comp_id
=============================================================================*/
/**       
Sets the composition for Gobi&tm; &nbsp; products.

@param *nv_composition_id [OUT] -- Pointer to NV HS-USB composition type.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
boolean hsu_get_undp_hsu_comp_id
(
 nv_hs_usb_supported_compositions_type * nv_composition_id
);

/*=============================================================================
FUNCTION    hsu_is_undp_using_static_interface
=============================================================================*/
/**       
Returns whether static interfaces are used for Gobi products

@dependencies
None.

@sideeffects
None.

@return
None.
*/
boolean hsu_is_undp_using_static_interface (void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_CONF_SEL_STACK_UTILS_H_ */
