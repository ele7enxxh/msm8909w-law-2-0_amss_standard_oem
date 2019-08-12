#ifndef HSU_OS_ABSTRACT_BUS_EXT_H
#define HSU_OS_ABSTRACT_BUS_EXT_H

/*=============================================================================

                H S U _ O S _ A B S T R A C T _ B U S _ E X T . H

=============================================================================*/

/**
@file hsu_os_abstract_bus_ext.h

High-speed USB OS abstraction layer bus externalized API declarations.

The APIs declared in this file are intended for the layers above the OS 
abstraction layer and not for the HS-USB core driver.

The following functions are externalized to outside of the OS abstraction layer:
 - hsu_os_bus_read_from_phy()
 - hsu_os_bus_write_to_phy()

There are no initialization and sequencing requirements.
*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_os_abstract_bus_ext.h#1 $

when      who     what, where, why
-------- ---    --------------------------------------------------------------
06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
03/10/08  ds      Initial version


=============================================================================*/

/*=============================================================================
Copyright (c) 2008-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                               INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include "port.h"
#include "rex.h"


/** @ingroup hsusb_qc_api 
@{ */


/*=============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================
  Typedefs
=============================================================================*/

/* Interrupt resource info type. */


/*=============================================================================
  Constants and Macros
=============================================================================*/
  
  /* events : add bit mapped events below. */
/** Code for USB disconnect event.
*/
#define HSU_OS_BUS_DISCONNECT_EVT 0x00000001

/** Code for USB connect event.
*/
#define HSU_OS_BUS_CONNECT_EVT    0x00000002

/** Code for USB resume event.
*/
#define HSU_OS_BUS_RESUME_EVT     0x00000004


/*=============================================================================

                          FUNCTION DECLARATIONS

=============================================================================*/


/*=============================================================================
FUNCTION    hsu_os_bus_read_from_phy
=============================================================================*/
/** 
Reads from a PHY register using the USB core's ULPI_VIEWPORT register.

@param phy_register [IN]  -- PHY register number.
@param *read_value  [OUT] -- Value of the PHY register.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The PHY read operation was successful.\n
FALSE -- The PHY read operation was not successful.
*/
boolean hsu_os_bus_read_from_phy(uint8 phy_register, uint8* read_value);

/*=============================================================================
FUNCTION    hsu_os_bus_write_to_phy
=============================================================================*/
/** 
Writes to a PHY register using the USB core's ULPI_VIEWPORT register.

@param phy_register  [IN] -- PHY Register number.
@param written_value [IN] -- Value to be written.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The PHY write operation was successful.\n
FALSE -- The PHY write operation was not successful.
*/
boolean hsu_os_bus_write_to_phy(uint8 phy_register, uint8 written_value);

/*=============================================================================
FUNCTION    hsu_os_bus_save_mem_dump
=============================================================================*/
/** 
Saves USB device/host controller registers and ULPI registers in pre-defined 
static memory.

@dependencies
None.

@sideeffects
None.

@return
None
*/
void hsu_os_bus_save_mem_dump(void);

/*=============================================================================
FUNCTION    hsu_os_bus_handle_pmic_event
=============================================================================*/
/** 
Handles PMIC PHY events when the PMIC PHY is used.

@param core_index [IN] -- Core index.
@param event      [IN] -- Event code for event that occured.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_os_bus_handle_pmic_event(uint32 core_index, uint32 event);

/*=============================================================================
FUNCTION    hsu_handle_sleep_vote
=============================================================================*/
/** 
Called when entering Low Power Mode (LPM).

This function votes for sleep if all cores are in LPM. This function also disables 
the vote for sleep if one of the cores exits from LPM.

@param assert_okts [IN] -- Whether a sleep vote is allowed.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_handle_sleep_vote(boolean assert_okts);

/*=============================================================================
FUNCTION    hsu_os_bus_memory_barrier
=============================================================================*/
/** 
Performs a strongly ordered write to system memory, which causes flushing of all 
buffered data such that it will be written to the system memory.

Memory barrier operation is necessary on an ARM11&tm; &nbsp; using buffered 
memory that is shared between the ARM11 and USB hardware (the LINK controller).

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_os_bus_memory_barrier( void );

/*=============================================================================
FUNCTION    hsu_os_bus_get_num_cores
=============================================================================*/
/** 
Returns the number of cores supported.

@dependencies
None.

@sideeffects
None.

@return
Number of supported cores.
*/
uint32 hsu_os_bus_get_num_cores( void );

/*=============================================================================
FUNCTION    hsu_os_bus_get_core_description
=============================================================================*/
/** 
Returns the core description (name).

@param core_index [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
Pointer to the core description.
*/
const char* hsu_os_bus_get_core_description( uint32 core_index );

/*=============================================================================
FUNCTION    hsu_os_bus_get_core_lpm_status
=============================================================================*/
/** 
Returns the core LPM status.

@param core_index [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The core is in LPM.\n
FALSE -- The core is not in LPM.
*/
boolean hsu_os_bus_get_core_lpm_status( uint32 core_index );

/*=============================================================================
FUNCTION    hsu_os_bus_get_usb_vote_for_sleep
=============================================================================*/
/** 
Returns the USB vote for sleep status.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The USB voted for sleep.\n
FALSE -- The USB did not vote for sleep.
*/
boolean hsu_os_bus_get_usb_vote_for_sleep( void );

/*=============================================================================
FUNCTION    hsu_os_bus_read_from_phy_no_reset
=============================================================================*/
/** 
Reads from a PHY register using the USB core's ULPI_VIEWPORT register.\ 
If the PHY access fails, the core is not reset.

@param phy_register [IN]  -- PHY register number.
@param *read_value  [OUT] -- Value that was read from the register.

@dependencies
hsu_os_bus_init() must have been called before using this function.

@sideeffects
None.

@return
TRUE  -- The PHY read operation was successful.\n
FALSE -- The PHY read operation was not successful.
*/
boolean hsu_os_bus_read_from_phy_no_reset( uint8 phy_register, uint8* read_value);

/*=============================================================================
FUNCTION    hsu_os_bus_write_to_phy_no_reset
=============================================================================*/
/** 
Writes to a PHY register using the USB core's ULPI_VIEWPORT register. 
If the PHY assess fails, the core is not reset.

@param phy_register  [IN] -- PHY register number.
@param written_value [IN] -- Value to be written.

@dependencies
hsu_os_bus_init() must have been called before using this function.

@sideeffects
None.

@return
TRUE  -- The PHY write operation was successful.\n
FALSE -- The PHY write operation was not successful.
*/
boolean hsu_os_bus_write_to_phy_no_reset(uint8 phy_register, uint8 written_value);

/*===========================================================================
FUNCTION    hsu_os_bus_enter_phy_into_lpm
==============================================================================*/
/** 
Puts the HS-USB PHY into LPM. This function is invoked from 
the HS-USB module's main LPM entry function.

@param core_index [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The PHY successfully entered LPM.\n
FALSE -- The PHY did not enter LPM.
*/
boolean hsu_os_bus_enter_phy_into_lpm(uint32 core_index);

/*=============================================================================
FUNCTION    hsu_os_bus_exit_phy_from_lpm
=============================================================================*/
/** 
Takes the HS-USB PHY out of Low Power Mode. This function is invoked 
from the HS-USB module's main LPM entry function.

@param core_index [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The PHY successfully exited from LPM.\n
FALSE -- The PHY did not exit from LPM.
*/
boolean hsu_os_bus_exit_phy_from_lpm(uint32 core_index);

/*=============================================================================
FUNCTION    hsu_os_bus_interrupt_init
=============================================================================*/
/** 
Initializes a USB core's interrupt. This function is called when the core's 
interrupt has registered its first interrupt service routine 
(using the os_interrupt_setup() call).

@param core_index [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) on success. Otherwise, one of the error codes defined in jerrno.h.
*/
jresult_t hsu_os_bus_interrupt_init(uint32 core_index);


/*=============================================================================
FUNCTION    hsu_os_bus_interrupt_uninit
=============================================================================*/
/** 
Un-initializes the HS-USB interrupt.

This function is called by the bus un-initialization function
hsu_os_bus_uninit().

@param core_index [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) on success. Otherwise, one of the error codes defined in jerrno.h.
*/
jresult_t hsu_os_bus_interrupt_uninit(uint32 core_index);

/*=============================================================================
FUNCTION    hsu_os_bus_config_intg_phy_for_lpm
=============================================================================*/
/** 
Configures the HS-USB integrated PHY before putting it into LPM.

@param core_index [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The HS-USB integrated PHY was successfully configured before entering
         LPM.\n
FALSE -- The HS-USB integrated PHY was not configured before entering LPM.
*/
boolean hsu_os_bus_config_intg_phy_for_lpm(uint32 core_index);

/*=============================================================================
FUNCTION    hsu_os_bus_hal_core_id
=============================================================================*/
/** 
Externalizes the macro OS_BUS_TO_HAL_CORE_ID.

@param core_id [IN] -- Core index internal to hsu_os_abstract_bus.

@dependencies
None.

@sideeffects
None.

@return
HAL core index.
*/
uint8 hsu_os_bus_hal_core_id(uint8 core_id);

/** @} */ /* endgroup hsusb_qc_api */

/*=============================================================================
FUNCTION    hsu_hal_os_bus_core_id
=============================================================================*/
/** 
Externalizes the macro OS_BUS_FROM_HAL_CORE_ID.

@param core_id [IN] -- HAL core index.

@dependencies
None.

@sideeffects
None.

@return
Core index internal to hsu_os_abstract_bus.
*/
uint8 hsu_hal_os_bus_core_id(uint8 core_id);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_OS_ABSTRACT_BUS_EXT_H */
