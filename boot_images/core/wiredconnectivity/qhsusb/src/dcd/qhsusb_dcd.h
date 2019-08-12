#ifndef QHSUSB_DCD_H
#define QHSUSB_DCD_H
/*=======================================================================*//**
  @file         qhsusb_dcd.h
 
  @brief        DCD API
 
  @details      Defines structures and functions required to initialize DCD

                In order to function, DCD should be provided with device
                definition. This includes definitions for device,
                its configurations, interfaces, endpoints, strings etc.

                For every element, there are 2 structures representing it:
                hardware (USB descriptor) and software one. Software structure 
                holds all relevant data, including internal states;
                it also holds pointer to USB descriptor.

                It is responsibility of upper layer to initialize device
                definition structures prior to DCD initialization.

                USB descriptors should be allocated in such way that
                all descriptors related to particular configuration follows
                its configuration descriptor, in order specified in 
                USB 2.0 spec, clause 9.4.3.
 
                For class specific requests, 'setup' callbacks may be defined
                at device, interface, endpoint levels. If callback is defined,
                it is called with any non-standard SETUP.
 
               Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dcd/qhsusb_dcd.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_ch9.h"
#include "qhsusb_urb.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*=====features====*/
/**
 * support for charger, both wall charger and USB host 
 */
//#define FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
/**
 * Multiple configurations support.
 *
 * In single configuration mode, it is assumed that defice have 1 configuration; 
 */
//#define QHSUSB_HAVE_MULTI_CONFIG 1
/**
 * Multiple functions support.
 *
 * In single function mode, it is assumed that defice serve one function;
 * in USB, function == interface.
 * device may have multiple configurations
 * each configuration have 1 interface with 1 alt. setting.
 */
//#define QHSUSB_HAVE_MULTI_FUNCTIONS 1
/* attachment detection */
/* multi speed */
/* shutdown */
/**
 * Event log facility
 */
//#define HAVE_DCD_EVENT_LOG 1

/**
 * Charger uses multi-config composition 
 */
#if defined(FEATURE_QHSUSB_DEAD_BATTERY_CHARGING)
   #if !defined(QHSUSB_HAVE_MULTI_CONFIG)
      #define QHSUSB_HAVE_MULTI_CONFIG 1
   #endif
#endif

/**
 * CDC/ACM uses multi-functions composition 
 */
#if defined(FEATURE_QHSUSB_HDLC_CDCACM) || (defined FEATURE_QHSUSB_MS)
   #if !defined(QHSUSB_HAVE_MULTI_FUNCTIONS)
      #define QHSUSB_HAVE_MULTI_FUNCTIONS 1
   #endif
#endif
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct qhsusb_dcd_dsc_endpoint;
struct qhsusb_dcd_dsc_interface;
struct qhsusb_dcd_dsc_configuration;
struct qhsusb_dcd_dsc_string;


struct qhsusb_dcd_dsc_device {
   /*===========supplied by application==============*/
   struct usb_desc_device* descriptor;
   struct qhsusb_dcd_dsc_configuration* configurations;
   struct qhsusb_dcd_dsc_string* strings;
   uint32 core_id;
   /**
    * Event notification 
    *  
    * Called when device state changed. Device state is updated 
    * with new values before calling this function. 
    */
   void (*notify)(struct qhsusb_dcd_dsc_device* dev, uint32 event);
#define QHSUSB_DCD_EVT_ATTACH  (0) /**< is_attached changed */
#define QHSUSB_DCD_EVT_SPEED   (1) /**< speed changed */
#define QHSUSB_DCD_EVT_ADDRESS (2) /**< address changed */
#define QHSUSB_DCD_EVT_CONFIG  (3) /**< config_value changed */
#define QHSUSB_DCD_EVT_CURRENT (4) /**< max_current changed */
#define QHSUSB_DCD_EVT_SUSPEND (5) /**< is_suspended changed (suspend/resume) */
#define QHSUSB_DCD_EVT_OTHER   (6) /**< any other event */
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
   /**
    * Custom Event Notification
	* This is called to inform about a disconnect event or a Reset
	* Event to the Application layer.
	*/
#define QHSUSB_DCD_EVT_DISCONNECT (7) /**< device disconnected */
#endif
 
   /**
    * Custom SETUP handler.
    * 
    * Called for all device level SETUP not understood by DCD
    * 
    * Return number of bytes transmitted;
    * for "set" type commands return 0.
    * In case of error, or if SETUP was not handled, return
    * negative error.
    */
   int (*setup)(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req);

   /*==============private===============*/
   /* current state */
   int is_attached; /**< is Vbus connected? */
   int speed; /**< Low - 1, Full - 12, High - 480, wall charger - -1 */
#define QHSUSB_DCD_SPEED_UNKNOWN      (0)
#define QHSUSB_DCD_SPEED_LOW          (1)
#define QHSUSB_DCD_SPEED_FULL        (12)
#define QHSUSB_DCD_SPEED_HIGH       (480)
#define QHSUSB_DCD_SPEED_WALL        (-1)

   int max_current; /**< in ma */
   int is_suspended;
   uint8 address;
   uint8 config_value;
   struct qhsusb_dcd_dsc_configuration* active_configuration;
   /* for setup transfers */
   uint8* setup_buffer;
   struct qhsusb_urb setup_tx_urb;
   struct qhsusb_urb setup_rx_urb;
   boolean qhsusb_test_mode_enabled;
   qhsusb_usb_port_test_mode test_mode;
};

struct qhsusb_dcd_dsc_configuration {
   /**
    * list of all configurations for device
    */
   struct qhsusb_dcd_dsc_configuration* next;
   struct usb_desc_configuration* descriptor;
   struct qhsusb_dcd_dsc_device* device;
   struct qhsusb_dcd_dsc_interface* interfaces;

};

struct qhsusb_dcd_dsc_string {
   struct qhsusb_dcd_dsc_string* next;
   struct usb_desc_header* descriptor;
   uint16 langid;
   uint8  index;
};

struct qhsusb_dcd_dsc_interface {
   /**
    * list of all interfaces/altsettings for config
    */
   struct qhsusb_dcd_dsc_interface* next;
   struct usb_desc_interface* descriptor;
   struct qhsusb_dcd_dsc_configuration* configuration;
   struct qhsusb_dcd_dsc_endpoint* endpoints;
   /**
    * Custom SETUP handler.
    * 
    * Called for all interface level SETUP not understood by DCD
    * 
    * Return number of bytes transmitted;
    * for "set" type commands return 0.
    * In case of error, or if SETUP was not handled, return
    * negative error.
    */
   int (*setup)(struct qhsusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req);

   /**
    * currently selected alt setting for this interface.
    * 
    * set simultaneously for all altsettings for interface
    */
   uint8 current_altsetting;
};

struct qhsusb_dcd_dsc_endpoint {
   /**
    * list of endpoints for interface
    */
   struct qhsusb_dcd_dsc_endpoint* next;
   usb_desc_endpoint_t* descriptor;
   struct qhsusb_dcd_dsc_interface* ifc;
   /**
    * Custom SETUP handler.
    * 
    * Called for all endpoint level SETUP not understood by DCD
    * 
    * Return number of bytes transmitted;
    * for "set" type commands return 0.
    * In case of error, or if SETUP was not handled, return
    * negative error.
    */
   int (*setup)(struct qhsusb_dcd_dsc_endpoint* ep, struct usb_setup_packet* req);
   /* private data */
   uint32 is_halt:1;
};

enum qhsusb_log_event {
   evt_zero = 0,
   evt_init,
   evt_reset,
   evt_set_addr,
   evt_set_config,
   evt_get_config,
   evt_set_power,
   evt_raw_setup,
   evt_suspend,
   evt_resume,
   evt_attach,
   evt_speed,
   evt_urb_err,
   evt_notify,
   evt_portsc,
};
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
 * Initialize DCD.
 * 
 * Should be called prior to any other USB activity
 * 
 * @param device Descriptor for current device composition.
 */
void qhsusb_dcd_init(struct qhsusb_dcd_dsc_device* device);

/**
 * Should be periodically invoked by upper layer
 * 
 * Non blocking
 * 
 * @param device Device to poll.
 *               
 *               May be NULL; in this case, no actions performed
 *               to protect from use of un-initialized device.
 */
void qhsusb_dcd_poll(struct qhsusb_dcd_dsc_device* device);

/**
 * Shutdown DCD
 * 
 * Move device to unconfigured state
 * cancell all pending URB's
 * 
 * @param device
 */
void qhsusb_dcd_shutdown(struct qhsusb_dcd_dsc_device* device);

/**
 * Initiate control transfer
 * 
 * Used in custom SETUP flow.
 * 
 * @param dev
 * @param is_tx  1 if Tx, 0 if Rx
 * @param buff   buffer to transmit;
 *               should not be modified till transmit completion.
 * @param len    number of bytes to transmit
 * 
 * @return number of bytes transferred; or negative error code
 */
int qhsusb_dcd_control_transfer(struct qhsusb_dcd_dsc_device* dev, int is_tx, void* buff, uint32 len);

/**
 * Set new configuration value, 
 * as if SET_CONFIG was received.
 * 
 * Use to configure device to run with 
 * already defined configuration value.
 * It assumes enumeration has already been done and
 * host issued SET_CONFIG before QHSUSB stack initialized.
 * 
 * @param dev
 * @param new_config
 * 
 * @return 
 */
int qhsusb_dcd_set_config(struct qhsusb_dcd_dsc_device* dev, uint8 new_config);

/**
 * Force re-evaluation of USB port conditions.
 * 
 * Conditions include:
 * 
 * - attachment state
 * - connection speed
 * - suspended state 
 *  
 * Suitable for DCI callback in case of "port status changed" 
 * interrupt 
 * 
 * @param dev
 */
void qhsusb_dcd_port_status_changed(struct qhsusb_dcd_dsc_device* dev);

#ifdef HAVE_DCD_EVENT_LOG
void qhsusb_dcd_log_event(struct qhsusb_dcd_dsc_device* dev,
                          enum qhsusb_log_event evt,
                          uint32 data0,
                          uint32 data1);
#else
static inline void qhsusb_dcd_log_event(struct qhsusb_dcd_dsc_device* dev,
                          enum qhsusb_log_event evt,
                          uint32 data0,
                          uint32 data1)
{/* empty function, will be optimized out by compiler*/ }
#endif /* #ifdef HAVE_DCD_EVENT_LOG */


/**
* Check if the USB device is enumerated.
* 
* 
* @param dev The device handle
*
* @return TRUE if the device is enumerated. FALSE otherwise.
*/
boolean qhsusb_dcd_is_enumerated(struct qhsusb_dcd_dsc_device* dev);

#endif /*QHSUSB_DCD_H*/

