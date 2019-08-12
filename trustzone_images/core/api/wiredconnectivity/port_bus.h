/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file port_bus.h

Contains the high-speed USB implementation of the REX OS abstraction layer 
bus API.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/port_bus.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _PORT_BUS_
#define _PORT_BUS_


/** @ingroup hsusb_jungo_api 
@{ */

jresult_t hsu_os_interrupt_setup(os_bus_resource_h res, 
    interrupt_handler isr_func, void *isr_arg,
    jthread_func dsr_func, void *dsr_arg);

jresult_t hsu_os_interrupt_teardown(os_bus_resource_h res);

jresult_t hsu_os_hw_init(void);
void hsu_os_hw_uninit(void);

/** Interrupt lines. */
#define SYS_RES_IRQ     1 

/** Input/output memory. */
#define SYS_RES_MEMORY  3

/** Input/output ports. */
#define SYS_RES_IOPORT  4

/** Indicates whether the transceiver controller is connected to a port. */
#define SYS_RES_TRANSCEIVER  5

os_bus_resource_h hsu_os_bus_alloc_resource(os_bus_h bus, jint_t type, jint_t id);
void hsu_os_bus_release_resource(os_bus_h bus, os_bus_resource_h res);

juint8_t hsu_os_bus_space_read_1(os_bus_resource_h res, juint32_t offset);
juint16_t hsu_os_bus_space_read_2(os_bus_resource_h res, juint32_t offset);
juint32_t hsu_os_bus_space_read_4(os_bus_resource_h res, juint32_t offset);
void hsu_os_bus_space_write_1(os_bus_resource_h res, juint32_t offset,
    juint8_t value);
void hsu_os_bus_space_write_2(os_bus_resource_h res, juint32_t offset, 
    juint16_t value);
void hsu_os_bus_space_write_4(os_bus_resource_h res, juint32_t offset, 
    juint32_t value);

os_bus_h hsu_os_bus_get_transceiver(os_bus_resource_h res);
jint_t hsu_os_bus_get_controller_id(os_bus_resource_h res);

/** Port information.
*/
typedef struct {
    jbool_t is_interchip; /**< Indicates whether this is an interchip port. */
} port_info_t;

jbool_t hsu_os_power_request(juint8_t voltage_class, juint8_t max_current, 
    juint8_t *set_voltage_class, juint8_t *set_max_current);

void hsu_os_get_port_info(os_bus_h bus, juint8_t port, port_info_t *info);

/** @} */ /* end_group hsusb_jungo_api */

#endif
