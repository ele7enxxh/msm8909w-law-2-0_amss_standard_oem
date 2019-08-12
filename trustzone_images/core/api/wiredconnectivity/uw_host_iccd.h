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


/**@file uw_host_iccd.h

Contains the support for the ICCD Class driver.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_host_iccd.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/26/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/

#ifndef _UW_HOST_ICCD_H_
#define _UW_HOST_ICCD_H_


/** @ingroup hsusb_jungo_api 
@{ */

/*
 * Note: "ICCD Class Specification" in this file refers to the:
 * "Universal Serial Bus, Device Class: Smart Card, ICCD Specification for
 * USB Integrated Circuit(s) Card Devices, Revision 1.0, April 22nd 2005"
*/

/** Values of the bResponseType field.
*/
typedef juint8_t iccd_response_type_t;
#define UWH_ICCD_BRESPONSE_INFORMATION 0x00
#define UWH_ICCD_BRESPONSE_BSTATUS_ERROR 0x40
#define UWH_ICCD_BRESPONSE_POLLING 0x80

/** Values of the bStatus bmIccStatus field.
*/
typedef juint8_t iccd_status_t;
#define UWH_ICCD_BSTATUS_PRESENT_ACTIVE 0x0
#define UWH_ICCD_BSTATUS_PRESENT_NOT_ACTIVE 0x1
#define UWH_ICCD_BSTATUS_VIRTUALLY_NOT_PRESENT 0x2

/** abData type for polling.
*/
typedef juint16_t delay_time_t;

/** Values that abData field may contain.
*/
typedef union {
    iccd_status_t status;  /**< ICCD status field. */
    delay_time_t delay;    /**< Delay to use for ICCD. */
} ab_data_t;

/** Values of the bError field.
*/
typedef jint8_t iccd_error_t;

#define UWH_ICCD_BERROR_ICC_MUTE -2
#define UWH_ICCD_BERROR_XFER_OVERRUN -4
#define UWH_ICCD_BERROR_ICC_HW_ERROR -5

/** ICCD class driver context type.
*/
typedef void *uwh_iccd_drv_h;

/** Application context type.
*/
typedef void *uwh_iccd_app_h;

/** ICCD class descriptor (see USB CCID Class Specification 1.1 for complete
    details).
*/
typedef struct {
    juint8_t length;                  /**< Descriptor length. */
    juint8_t descriptor_type;         /**< Descriptor type. */
    juint16_t release_number;         /**< ICCD specification release number
									       (BCD). */
    juint8_t max_slot_index;          /**< Index of the highest available
									       slot. */
    juint8_t voltage_support;         /**< Voltage support. */
    juint32_t protocols;              /**< Protocols supported. */
    juint32_t default_clock;          /**< Default clock value (not relevant
									       for legacy). */
    juint32_t maximum_clock;          /**< Maximum clock value. */
    juint8_t num_clock_supported;     /**< Number of clocks supported (not
									       relevant for legacy). */
    juint32_t data_rate;              /**< Data rate (not relevant for
									       legacy). */
    juint32_t max_data_rate;          /**< Maximum data rate (not relevant for
                                           legacy). */
    juint8_t num_data_rate_supported; /**< Number of data rates supported (not 
                                           relevant for legacy). */
    juint32_t max_ifsd;               /**< Maximum IFSD supported (not
									       relevant for legacy). */
    juint32_t synch_protocols;        /**< Sync protocols supported (not 
                                           relevant for legacy). */
    juint32_t mechanical;             /**< Mechanical (not relevant for 
                                           legacy). */
    juint32_t features;               /**< Features. */
    juint32_t max_ccid_message_length;  /**< Maximum CCID message length. */
    juint8_t class_get_response;      /**< Class get response. */
    juint8_t class_envelope;          /**< Class envelope. */
    juint16_t lcd_layout;             /**< LCD layout (not relevant for
									       legacy). */
    juint8_t pin_support;             /**< Pin support (not relevant for
									       legacy). */
    juint8_t max_ccid_busy_slots;     /**< Maximum CCID busy slots. */
} uwh_iccd_descriptor_t;


/** Application callbacks structure.
*/
typedef struct {
   /* ===== Callback field: attach */
   /**
     Device attach application callback.

     Performs the required application initialization for handling the new 
     device, such as resources allocation.

     @param  drv_ctx  [IN] -- Driver context.
     @param *app_ctx [OUT] -- Pointer to the private application context 
                              (uwh_iccd_app_h), which should be used in calls 
                              to the other application callbacks.
    
     @dependencies
     None.

     @sideeffects
     None.

     @return
     Zero (0) on success; error code on failure.

     @scope
     Static.
   */
   jresult_t (*attach)(uwh_iccd_drv_h drv_ctx, uwh_iccd_app_h *app_ctx);

   /* ===== Callback field: detach */
   /**
     Device detach application callback.

     @param app_ctx [IN] -- Application context.
   
     @dependencies
     None.

     @sideeffects
     None.

     @return
     None.

     @scope
     Static.
   */
   void (*detach)(uwh_iccd_app_h app_ctx);

   /* ===== Callback field: slot_change */
   /*
   N/A -- currently not supported.
   */
   void (*slot_change)(juint8_t message_type, juint8_t slot_status, 
        uwh_iccd_app_h app_ctx);
} uwh_iccd_callbacks_t;

/***************************************************************************/
/*                        ICCD Class Driver API                            */
/***************************************************************************/
typedef void (*uwh_iccd_cb)(uwh_iccd_app_h app_ctx, 
    void *data, juint16_t size, iccd_response_type_t response_type,
    ab_data_t ab_data, iccd_error_t error, void *arg, jresult_t usb_status);


/* ===== Function name:  uwh_iccd_register */
/**
  Registers an ICCD application with the ICCD Class Driver.

  The class driver will be operational only after the registration function is 
  called and returns successfully. The application should call this function 
  during its initialization.

  @param *callbacks [IN] -- Pointer to a structure containing the application's
                            callback functions.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.

  @scope
  Global.
*/
jresult_t uwh_iccd_register(uwh_iccd_callbacks_t *callbacks);


/* ===== Function name:  uwh_iccd_unregister */
/**
  Unregisters an application from the ICCD Class Driver.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void uwh_iccd_unregister(void);


/* ===== Function name:  uwh_iccd_power */
/**
  Sends an ICC_POWER_ON or ICC_POWER_OFF request.

  The ICC_POWER_ON and ICC_POWER_OFF requests are described in Tables 6.2-9 
  and 6.2-10 of the ICCD Class Specification. 

  @param drv_ctx  [IN] -- Driver context.
  @param power_on [IN] -- TRUE = Send an ICC_POWER_ON request.\n
                          FALSE = Send an ICC_POWER_OFF request.
  @param cb       [IN] -- Callback function to call at the end of the transfer.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.

  @scope
  Global.
*/
jresult_t uwh_iccd_power(uwh_iccd_drv_h drv_ctx, jbool_t power_on, 
    uwh_iccd_cb cb);


/* ===== Function name:  uwh_iccd_xfr_block */
/**
  Transfers data from the host to the USB-ICC, using the XFER_BLOCK ICCD 
  request (see Table 6.2-11 of the ICCD Class Specification).

  @param drv_ctx [IN] -- Driver context.
  @param *data   [IN] -- Handle to a buffer containing the data to send.
  @param size    [IN] -- Buffer size, in bytes.
  @param cb      [IN] -- Callback function to call at the end of the transfer.
  @param *arg    [IN] -- Pointer to arguments for the callback function.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.

  @scope
  Global.
*/
jresult_t uwh_iccd_xfr_block(uwh_iccd_drv_h drv_ctx, void *data, 
    juint16_t size, uwh_iccd_cb cb, void *arg);


/* ===== Function name:  uwh_iccd_data_block */
/**
  Transfers data from the USB-ICC to the host, using the DATA_BLOCK ICCD 
  request (see Table 6.2-12 of the ICCD Class Specification).

  @param drv_ctx [IN] -- Driver context.
  @param *data   [IN] -- Handle to a buffer to be filled with the read data.
  @param size    [IN] -- Buffer size, in bytes.
  @param cb      [IN] -- Callback function to call at the end of the transfer.
  @param *arg    [IN] -- Pointer to arguments for the callback function.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.

  @scope
  Global.
*/
jresult_t uwh_iccd_data_block(uwh_iccd_drv_h drv_ctx, void *data, 
    juint16_t size, uwh_iccd_cb cb, void *arg);


/* ===== Function name:  uwh_get_info */
/**
  Returns ICCD descriptor information (uwh_iccd_descriptor_t).

  The ICCD descriptor structure is defined in Table 5.1-1 of the ICCD Class 
  Specification.
   
  @param drv_ctx [IN] -- Driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Pointer to an ICCD descriptor information structure (uwh_iccd_descriptor_t).

  @scope
  Global.
*/
uwh_iccd_descriptor_t *uwh_get_info(uwh_iccd_drv_h drv_ctx);


/* ===== Function name:  uwh_iccd_suspend */
/**
  Suspends an ICCD (Selective Suspend).\ After the device is suspended, the 
  host stack also attempts to suspend all other inactive bus segments.
 
  @param drv_ctx [IN] -- Driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.

  @scope
  Global.
*/
jresult_t uwh_iccd_suspend(uwh_iccd_drv_h drv_ctx);


/* ===== Function name:  uwh_iccd_resume */
/**
  Resumes normal operation of a suspended ICCD.
    
  @param drv_ctx [IN] -- Driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.

  @scope
  Global.
*/
jresult_t uwh_iccd_resume(uwh_iccd_drv_h drv_ctx);

/** @} */ /* end_group hsusb_jungo_api */

#endif
