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


/**@file jos.h

Holds general definitions for the Jungo core.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/jos.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef __JOS_H__
#define __JOS_H__

#include <jerrno.h>

#include "jtypes.h"
#include "dev_monitor.h"
#include "lists.h"
#include "compiler.h"
#include "os_port.h"
#include "jutil.h"
#include "controllers.h"


/** @ingroup hsusb_jungo_api 
@{ */

/** Device and driver registration methods.
*/
typedef struct jdevice_s *jdevice_t;

#define HOST_PORT_ENABLE_NO_RESET 0     /**< Host controller IOCTL. */
#define HOST_PORT_ENABLE_RESET    1     /**< Host controller IOCTL. */

/** Enumeration of driver IOCTL codes. 
  Some IOCTLs require an argument, which is passed to the IOCTL's recipient via
  the arg parameter of the IOCTL's handler function. For IOCTLs that do not 
  require an argument, the IOCTLs handler's arg parameter should be set to NULL.
*/
typedef enum {
    DRV_IOCTL_NONE,                          /**< &nbsp; */
    
    DRV_IOCTL_PWR_CHILD_SUSPEND,             /**< &nbsp; */
    DRV_IOCTL_PWR_CHILD_RESUME,              /**< &nbsp; */
    DRV_IOCTL_PWR_HUB_REMOTE_WAKEUP,         /**< &nbsp; */
    
    /* OTG related IOCTLS */
    DRV_IOCTL_DEV_SUSPEND,                   /**< &nbsp; */
    DRV_IOCTL_DEV_RESUME,                    /**< &nbsp; */
    DRV_IOCTL_DEV_RESET,                     /**< &nbsp; */
    DRV_IOCTL_DEV_HNP_ENBL,                  /**< &nbsp; */
    DRV_IOCTL_DEV_HNP_ALT_SUPPORTED,         /**< &nbsp; */
    DRV_IOCTL_HOST_SUSPEND,                  /**< &nbsp; */
    DRV_IOCTL_HOST_RESUME,                   /**< &nbsp; */
    DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED,      /**< &nbsp; */
    DRV_IOCTL_HOST_REMOTE_CONNECTED,         /**< &nbsp; */
    DRV_IOCTL_HOST_REMOTE_DISCONNECTED,      /**< &nbsp; */
    DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED, /**< &nbsp; */
    DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED,     /**< &nbsp; */

    DRV_IOCTL_DEV_ENABLE,                    /**< &nbsp; */
    DRV_IOCTL_DEV_DISABLE,                   /**< &nbsp; */

    DRV_IOCTL_HOST_PORT_ENABLE,              /**< &nbsp; */
    DRV_IOCTL_HOST_PORT_DISABLE,             /**< &nbsp; */
    DRV_IOCTL_HOST_PORT_SUSPEND,             /**< &nbsp; */
    DRV_IOCTL_HOST_PORT_RESUME,              /**< &nbsp; */
    DRV_IOCTL_HOST_ENABLE_HNP,               /**< &nbsp; */

    /* Transceiver Related IOCTLs*/
    DRV_IOCTL_OTG_INTR,                      /**< &nbsp; */

    DRV_IOCTL_DEVICE_INTR,                   /**< &nbsp; */
 
    DRV_IOCTL_HOST_PORT_CHANGE,              /**< &nbsp; */

    DRV_IOCTL_TCD_GET_OPS,                   /**< &nbsp; */
    DRV_IOCTL_TCD_SET_CTX,                   /**< &nbsp; */
    DRV_IOCTL_HOST_SET_PARAMS,               /**< &nbsp; */
    DRV_IOCTL_DEV_SET_PARAMS,                /**< &nbsp; */
    
    /* USB3.0 DCD IOCTLs &*/
    DCD_IOCTL_SET_ISOCH_DELAY,               /**> &nbsp; */
    DCD_IOCTL_SET_SEL,                       /**> &nbsp; */

    DRV_IOCTL_SET_ETEST_MODE,   /**< Electrical host controller tests IOCTL. */

    DRV_IOCTL_LAST              /**< MAX enumeration value in drv_ioctl_t. */
} drv_ioctl_t;

typedef jresult_t (*driver_init_t)(void); 
typedef void     (*driver_uninit_t)(void);
typedef jresult_t (*driver_func_t)(jdevice_t dev);
typedef jresult_t (*driver_ioctl_t)(jdevice_t dev, drv_ioctl_t ioctl, void *arg);

/** Structure of driver callback methods.
  The DCD defines and initializes a \<dcd_name\>_methods structure of this type 
  using one of the controller driver declaration macros. It passes this structure 
  to the JOS layer during its initialization.
*/
typedef struct {
    driver_func_t       match;    /**< N/A -- should be implemented to return zero. */
    driver_func_t       attach;   /**< Device attach callback. */
    driver_func_t       detach;   /**< Device detach callback. */
    driver_func_t       suspend;  /**< N/A -- should be implemented to return zero. */
    driver_func_t       resume;   /**< N/A -- should be implemented to return zero. */
    driver_ioctl_t      ioctl;    /**< IOCTL handler callback. */
    driver_init_t       uninit;   /**< Optional custom driver un-initialization
                                       callback. */
} driver_methods_t;

/** Auto-initialization for non-USB drivers.
*/
typedef struct {
    driver_init_t init;
	     /**< Optional custom DCD initialization function.\ If set, this is 
		      called from within the mandatory DCD initialization function that 
			  the macro defines. */
    driver_uninit_t uninit;
	     /**< Optional custom uninitialization callback function.\ This 
		      parameter is assigned to the uninit field of the driver callback 
			  methods structure that is created by the macro and registered by 
			  the initialization function. */
    jbool_t init_done;
	     /**< Specifies whether initialization has already been performed for 
              this driver. */
} load_arr_t;

/* PCI Methods */
#define BUS_SPACE_BARRIER_READ  0x01    /**< Force a read barrier. */
#define BUS_SPACE_BARRIER_WRITE 0x02    /**< Force a write barrier. */

jint_t jpci_get_powerstate(jdevice_t dev);
jresult_t jpci_set_powerstate(jdevice_t dev, jint_t state);
jint32_t j_pci_get_devid(os_bus_h bus);
juint8_t j_pci_get_class(os_bus_h bus);
juint8_t j_pci_get_subclass(os_bus_h bus);
juint8_t j_pci_get_progif(os_bus_h bus);
juint16_t j_pci_get_vendor(os_bus_h bus);
void j_bus_space_barrier(jint_t flags);

/* Register a Controller with parameters sent as ivars */
jresult_t add_generic_controller_ex(os_bus_h bus, jint_t id, void **handle,
    void *args);
jresult_t del_generic_controller_ex(jdevice_t dev);

/* Device manipulation methods */
void *j_device_get_softc(jdevice_t dev);
void *j_get_usbd_device(jdevice_t dev);
char *j_device_get_desc(jdevice_t dev);
jbus_h j_device_get_bus(jdevice_t dev);

jdevice_t jdevice_add(jdevice_t parent);
void jdevice_delete(jdevice_t device);

jbool_t j_device_is_attached(jdevice_t dev);
jresult_t j_device_detach(jdevice_t dev);
    
jdevice_t j_device_get_parent(jdevice_t dev);
jint16_t j_device_get_unit(jdevice_t dev);
char *j_device_get_nameunit(jdevice_t dev);
void j_device_set_nameunit(jdevice_t dev, char *name, int unit);
jbool_t j_device_attached(jdevice_t dev);

/** Driver information.
*/
typedef struct kobj_class       driver_t;

void jinterrupt_teardown(jbus_resource_h res, void *handle);
jresult_t jinterrupt_setup(jbus_resource_h res, 
    interrupt_handler isr_func, void *isr_arg,
    jthread_func dsr_func, void *dsr_arg, void **handle);

/** Pointer to task structure.
*/
typedef struct jtask_s *jtask_h;

void jtask_uninit(jtask_h task);
void jtask_stop(jtask_h task);
int jtask_pending(jtask_h task);


void j_device_set_desc(jdevice_t dev, const char* desc);
void j_device_set_ivars(jdevice_t dev, void * ivars);
jresult_t j_device_probe_and_attach(jdevice_t dev, void *usbd_device);
jresult_t j_device_suspend(jdevice_t dev);
jresult_t j_device_resume(jdevice_t dev);
jresult_t j_device_ioctl(jdevice_t dev, drv_ioctl_t ioctl, void *arg);

typedef void bus_driver_added_t(jdevice_t _dev, driver_t *_driver);
typedef void bus_child_detached_t(jdevice_t _dev, jdevice_t _child);
typedef jresult_t bus_child_location_str_t(jdevice_t _dev, jdevice_t _child,
    char *buf, juint32_t buflen);
typedef jresult_t bus_child_pnpinfo_str_t(jdevice_t _dev, jdevice_t _child,
    char *buf, juint32_t buflen);

void *j_device_get_ivars(jdevice_t dev);

/* JOS String Manipulation functions
 */
char *j_strdup(const char *str);
char *j_numtostr(char *outstr, juint32_t base, juint32_t min_digits,
    juint32_t max_digits, juint32_t num);
#define NUMERIC_VALUE_INVALID   0xFFFFFFFF
juint32_t j_strtonum(char *instr, juint32_t base, char **last_ptr);
char *j_strstr(char *instr, char *substr);
jbool_t j_is_space(char c);

#define j_bzero(buf, len) j_memset(buf, 0, len)

#define timevalclear(tvp)               ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#define timevalcmp(tvp, uvp, cmp)                                       \
        (((tvp)->tv_sec == (uvp)->tv_sec) ?                             \
            ((tvp)->tv_usec cmp (uvp)->tv_usec) :                       \
            ((tvp)->tv_sec cmp (uvp)->tv_sec))
void j_delay(jint32_t us);

#define SYSCTL_INT(parent, nbr, name, access, ptr, val, descr) 
#define TUNABLE_INT(a, b)

typedef jint32_t off_t;  /* file offset */
typedef void *dev_t;

void j_mark_time_v(char *file, jint32_t line);
#define j_mark_time() j_mark_time_v(__FILE__, __LINE__)
extern jint32_t j_start_perf;
void j_dump_time(void);

jresult_t jfs_initialize(void);
jresult_t jfs_uninitialize(void);

#define DRIVER_CONTROLLER       0x1
#define DRIVER_USB              0x2

jresult_t jdriver_register(jint_t type, driver_methods_t *methods,
    char *name, jint_t id, juint16_t size);

/** Common way to declare a driver. */
#define DRIVER_COMMON_DECLARE(type, name, id, size, init_func, uninit_func) \
        \
        static driver_methods_t __CONCAT(name, _methods) = {    \
                __CONCAT(name, _probe),         \
                __CONCAT(name, _attach),        \
                __CONCAT(name, _detach),        \
                __CONCAT(name, _suspend),       \
                __CONCAT(name, _resume),        \
                __CONCAT(name, _ioctl),         \
                (uninit_func)                   \
        }; \
        \
        jresult_t __CONCAT(name, _init)(void)    \
        {               \
                int rc; \
                \
                if ((init_func) != NULL) \
                { \
                    rc = (init_func)(); \
                    if (rc) \
                        return rc; \
                } \
                \
                rc = jdriver_register(type, &__CONCAT(name, _methods), \
                        #name, id, size); \
                if (rc && (uninit_func) != NULL) \
                    (uninit_func)(); \
                \
                return rc; \
        }

/** Common way to declare a USB driver. */
#define DRIVER_USB_DECLARE(name) \
        DRIVER_COMMON_DECLARE(DRIVER_USB, name, 0, \
            sizeof(struct __CONCAT(name, _softc)), \
            (driver_init_t)NULL, (driver_init_t)NULL)

/** Declares a USB driver with a custom init/uninit function. */
#define DRIVER_USB_DECLARE_CUSTOM_INIT(name, init_func, uninit_func) \
        DRIVER_COMMON_DECLARE(DRIVER_USB, name, 0, \
            sizeof(struct __CONCAT(name, _softc)), \
            init_func, uninit_func)

/** Common way to declare a controller driver. */
#define DRIVER_CONTROLLER_DECLARE(name, size) \
        DRIVER_COMMON_DECLARE(DRIVER_CONTROLLER, name, \
            __CONCAT(controller_type_, name), size, \
            (driver_init_t)NULL, (driver_init_t)NULL)

/** Declares a controller driver with a custom init/uninit function. */
#define DRIVER_CONTROLLER_DECLARE_CUSTOM_INIT(name, size, init_func, uninit_func) \
        DRIVER_COMMON_DECLARE(DRIVER_CONTROLLER, name, \
            __CONCAT(controller_type_, name), size, \
            init_func, uninit_func)

/** DMA handler function that returns a physical address. */
#define DMAADDR(dma, offset) \
        (juint32_t)jdma_phys_addr(*(dma), offset)

/** DMA handler function that returns a virtual address. */
#define KERNADDR(dma, offset) \
        jdma_virt_addr(*(dma), offset)

/** Handle to the DMA allocation. */
typedef void *jdma_handle;

/** Gets the physical address of a block. */
void *jdma_phys_addr(jdma_handle p, juint32_t offset);

/** Gets the virtual address of a block. */
void *jdma_virt_addr(jdma_handle p, juint32_t offset);

/** Sets a block to some dummy values.\n
  Be sure to reset the block back to (0, NULL, NULL) before freeing it.
*/
void jdma_dummy_set(juint32_t size, void *vaddr, void *paddr, jdma_handle p);

/** Allocates a new DMA block. */
jresult_t jdma_alloc(juint32_t size, juint16_t align, 
    void **vaddr, void **paddr, juint16_t flags, jdma_handle *handle);

/** Frees a DMA block. */
void jdma_free(jdma_handle handle);

/** @} */ /* end_group hsusb_jungo_api */

#endif
