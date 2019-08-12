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


/**@file dev_monitor.h

Holds definitions that allow a monitor application to receive device 
notifications.

An application can register to receive device attach/detach notifications and
get information regarding attached devices. The returned information also
includes information regarding the device's parent, thus enabling the
application to maintain a devices tree.

When registering to receive notifications, the application passes a list of
callback methods, which are invoked upon attach and detach of devices. The
application can also force a device detach.

@note Only one monitor application can be registered at any given time.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/dev_monitor.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/24/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _DEV_MONITOR_H_
#define _DEV_MONITOR_H_


/** @ingroup hsusb_jungo_api 
@{ */

typedef void *mdev_t;

/** Device information structure.
*/
typedef struct {
    jint_t dev_class;      /**< Device class. */
    jint_t subclass;       /**< Device subclass. */
    jint_t protocol;       /**< Device protocol. */
    jint_t vendor;         /**< Device vendor ID. */
    jint_t product;        /**< Device product ID. */
    jint_t version;        /**< Device version. */
    char *name;            /**< Device name. */
    char *desc;            /**< Device description. */
    juint8_t depth;        /**< Device distance from the root hub. */
} devmon_info_t;

/** Device monitor messages.
*/
typedef enum {
    DM_NEW_DEVICE,                          /**< &nbsp; */
    DM_NEW_DEVICE_READY,                    /**< &nbsp; */
    DM_NEW_DEVICE_NO_DRIVER,                /**< &nbsp; */
    DM_NEW_DEVICE_ATTACHING,                /**< &nbsp; */
    DM_NEW_DEVICE_ATTACH_DENIED,            /**< &nbsp; */

    DM_DEVICE_DETACHING,                     /**< &nbsp; */
    DM_DEVICE_DETACHED,                      /**< &nbsp; */
    DM_DEVICE_DISCONNECTED,                  /**< &nbsp; */
    DM_DEVICE_SUSPENDED,                     /**< &nbsp; */
    DM_DEVICE_RESUMED,                       /**< &nbsp; */
    DM_DEVICE_CONFIG_SETTING,                /**< &nbsp; */
    DM_DEVICE_CONFIG_SET,                    /**< &nbsp; */

    DM_UCOMP_ATTACH_SUBDEV,                  /**< &nbsp; */
    DM_UCOMP_DETACHING_SUBDEV,               /**< &nbsp; */
    DM_UCOMP_DETACHED_SUBDEV,                /**< &nbsp; */

    DM_ERROR_CANNOT_OPEN_EP0,                /**< &nbsp; */
    DM_ERROR_CANNOT_ALLOC_ADDR,              /**< &nbsp; */
    DM_ERROR_CANNOT_SET_ADDR,                /**< &nbsp; */
    DM_ERROR_CANNOT_GET_FIRST_DESC,          /**< &nbsp; */
    DM_ERROR_BAD_DEV_DESC,                   /**< &nbsp; */
    DM_ERROR_CANNOT_GET_CONFIG,              /**< &nbsp; */
    DM_ERROR_CANNOT_ATTACH,                  /**< &nbsp; */
    DM_ERROR_CANNOT_SET_CONFIG,              /**< &nbsp; */
    DM_ERROR_RESET_FAILED,                   /**< &nbsp; */
    DM_ERROR_OVERCURRENT,                    /**< &nbsp; */
    DM_ERROR_POWER_NEGOTIATION_FAILED,       /**< &nbsp; */

    DM_ERROR_HUB_TOO_DEEP,                   /**< &nbsp; */
    DM_ERROR_HUB_GET_DESC,                   /**< &nbsp; */
    DM_ERROR_HUB_BUSPWR_ATTACHED_TO_BUSPWR,  /**< &nbsp; */
    DM_ERROR_HUB_NO_INTERFACE,               /**< &nbsp; */
    DM_ERROR_HUB_NO_ENDPOINT,                /**< &nbsp; */
    DM_ERROR_HUB_BAD_ENDPOINT,               /**< &nbsp; */
    DM_ERROR_HUB_CANNOT_OPEN_PIPE,           /**< &nbsp; */

    /* HSU temp fix */ 
    DM_DEVICE_PARTIAL_SUPPORTED,             /**< &nbsp; */
    DM_DEVICE_SUPPORTED,                     /**< &nbsp; */
    DM_DEVICE_NOT_SUPPORTED,                 /**< &nbsp; */
    /* HSU temp fix */ 

    DM_LAST                                  /**< &nbsp; */
} devmon_msg_t;

/** Device monitor application callback methods structure.
*/
typedef struct devmon_methods {
    /* Implemented by user */
  void (*detached)(mdev_t dev, mdev_t parent);
       /**< Called when a device is successfully detached from the stack. */

  void (*attached)(mdev_t dev, mdev_t parent);
       /**< Called when a device is successfully attached to the stack. */

  jresult_t (*allow_attach)(mdev_t dev);
       /**< Called when a device attach is detected and before the device is 
	        actually attached to the stack.\ Determines whether to allow the 
			device attach. */

    jresult_t (*devmon_notify_h)(devmon_msg_t msg, mdev_t dev, juint32_t param);
	   /**< Called to perform a notification. */
} devmon_methods_t;

/* Implemented by HOST stack */
jresult_t devmon_notify(devmon_msg_t msg, mdev_t dev, juint32_t param);
jresult_t devmon_register(devmon_methods_t *methods);
void devmon_unregister(void);
jresult_t devmon_force_detach(mdev_t dev);
void devmon_get_info(mdev_t dev, devmon_info_t *info);

/*  Internal host stack functions */
void devmon_handle_detached(mdev_t dev, mdev_t parent);
void devmon_handle_attached(mdev_t dev, mdev_t parent);
jresult_t devmon_allow_attach(mdev_t dev);

/** @} */ /* end_group hsusb_jungo_api */
    
#endif
