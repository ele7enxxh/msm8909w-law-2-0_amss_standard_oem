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


/**@file uw_otg_app.h

Contains the declarations for the On-The-Go (OTG) application. 

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_otg_app.h#1 $
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

#ifndef __OTG_APP_H__ 
#define __OTG_APP_H__ 


/** @ingroup hsusb_jungo_api 
@{ */

/** OTG core context.
*/
typedef void *otg_ctx_t;

/** Application context.
*/
typedef void *otg_appctx_t;

/** Error description.
*/
typedef enum {
    OTG_ERROR_OK,        /**< &nbsp; */
    OTG_ERROR_VBUS,      /**< &nbsp; */
    OTG_ERROR_SRP_FAILED /**< Session Request Protocol failure. */

} otg_error_t;

/** HNP state.
*/
typedef enum {
    OTG_HNP_DISABLED,       /**< &nbsp; */
    OTG_HNP_SUPPORTED,      /**< &nbsp; */
    OTG_HNP_ALT_SUPPORTED,  /**< &nbsp; */
    OTG_HNP_ENABLED         /**< &nbsp; */
} otg_hnp_state_t;

/** OTG state.
*/
typedef enum {
    OTG_PERIPH_SUPPORTED,          /**< &nbsp; */
    OTG_PERIPH_PARTIAL_SUPPORTED,  /**< &nbsp; */
    OTG_PERIPH_NOT_SUPPORTED       /**< &nbsp; */
} otg_peripheral_state_t;

/** OTG device mode.
*/
typedef struct {
#define PERIPH_MATCH_BY_VENDOR_ID       0x01
#define PERIPH_MATCH_BY_PRODUCT_ID      0x02
#define PERIPH_MATCH_BY_MIN_BCD_DEVICE  0x04
#define PERIPH_MATCH_BY_MAX_BCD_DEVICE  0x08
#define PERIPH_MATCH_BY_USB_CLASS       0x10
#define PERIPH_MATCH_BY_USB_SUB_CLASS   0x20
#define PERIPH_MATCH_BY_USB_PROTOCOL    0x40
    juint8_t flags;             /**< OTG flags. */

    juint16_t vendor_id;        /**< Vendor ID. */
    juint16_t product_id;       /**< Product ID. */
    juint16_t min_bcd_device;   /**< Minimum Binary-Coded Decimal device. */
    juint16_t max_bcd_device;   /**< Maximum Binary-Coded Decimal device. */

    juint8_t usb_class;         /**< USB Class ID. */
    juint8_t usb_sub_class;     /**< USB Subclass ID. */
    juint8_t usb_protocol;      /**< USB Protocol. */
} peripheral_matcher_t;

/** OTG device mode.
*/
typedef enum {
    OTG_DEVICE_MODE_NONE,        /**< &nbsp; */
    OTG_DEVICE_MODE_HOST,        /**< &nbsp; */
    OTG_DEVICE_MODE_PERIPHERAL   /**< &nbsp; */
} otg_device_mode_t;

/* ******************************************** *
 *      Application --> OTG Core Functions      *
 * ******************************************** */

/** OTG application callback functions structure.
*/
typedef struct {
    /* ===== Function name:  notify_peripheral_state */
    /**
      Notifies the application of the host's degree of support for the 
      connected peripheral.
    
      @param appctx [IN] -- Application context, as received from otg_app_init().
      @param state  [IN] -- State of the connected peripheral (in terms of the
                            host's support).
    
      @dependencies
      None.

      @sideeffects
      None.

      @return
      None. 
    
      @requirement
      Mandatory.

      @scope
      Static.
    */
    void (*notify_peripheral_state)(otg_appctx_t appctx, 
        otg_peripheral_state_t state);

    /* ===== Function name:  notify_hnp_state */
    /**
      Notifies the application of the current HNP state.

      @param appctx [IN] -- Application context, as received from otg_app_init().
      @param state  [IN] -- Current HNP state.
    
      @dependencies
      None.

      @sideeffects
      None.

      @return
      None. 
    
      @requirement
      Mandatory.

      @scope
      Static.
    */
    void (*notify_hnp_state)(otg_appctx_t appctx, otg_hnp_state_t state);

    /* ===== Function name:  notify_srp_state */
    /**
      Notifies the application of the device's current SRP state, which determines 
      whether the device can start an SRP process.
 
      @param appctx     [IN] -- Application context, as received from 
                                otg_app_init().
      @param is_enabled [IN] -- Current SRP state: 1 = Enabled, 0 = Disabled.

      @dependencies
      None.

      @sideeffects
      None.

      @return
      None. 

      @requirement
      Mandatory.

      @scope
      Static.
    */
    void (*notify_srp_state)(otg_appctx_t appctx, jbool_t is_enabled);
    
    /* ===== Function name:  notify_error */
    /**
      Notifies the application that an error of type otg_error_t has occurred.
  
      @param appctx [IN] -- Application context, as received from otg_app_init().
      @param err    [IN] -- Type of error that occurred.

      @return
      None. 

      @requirement
      Mandatory.

      @scope
      Static.
    */
    void (*notify_error)(otg_appctx_t appctx, otg_error_t err);

    /* ===== Function name:  notify_device_mode */
    /**
      Notifies the application of the current device mode (Host or Peripheral).

      @param appctx [IN] -- Application context, as received from otg_app_init().
      @param mode   [IN] -- Current device mode.
           
      @dependencies
      None.

      @sideeffects
      None.

      @return
      None. 

      @requirement
      Mandatory.

      @scope
      Static.
    */
    void (*notify_device_mode)(otg_appctx_t appctx, otg_device_mode_t mode);

#define DEVICE_STATUS_DISCONNECTED 0
#define DEVICE_STATUS_CONNECTED 1

    /* ===== Function name: notify_device_connect_status */
    /**
      Notifies the application of one of the following connection status 
      changes:
       - An A-type plug has been inserted and the device moved from a B_IDLE to 
         an A_IDLE state (status=TRUE).\ After receiving this notification, a 
         VBUS power-up can be requested at any time until the A-type plug is 
         disconnected.
       - An A-type plug has been disconnected and the device moved from an 
         A_IDLE to a B_IDLE state (status=FALSE).

      @param appctx  -[IN] - Application context, as received from otg_app_init().
      @param status  [IN] -- TRUE if an A-type plug has been inserted; FALSE if 
                             an A-type plug has been disconnected.

      @dependencies
      None.

      @sideeffects
      None.

      @return
      None. 

      @requirement
      Mandatory.

      @scope
      Static.
    */
    void (*notify_device_connect_status)(otg_appctx_t appctx, jbool_t status);

    /* ===== Function name: respond_srp  */
    /**
      Notifies the application that the connected B-device has initiated SRP 
      after the application had instructed the OTG core to drop the VBUS.\ The 
      application must decide whether the OTG core should respond to the SRP 
      (Session Request Protocol).
        
      @param appctx [IN] -- Application context, as received from otg_app_init().
           
      @dependencies
      None.

      @sideeffects
      None.

      @return
      TRUE to respond to the SRP.\n
      FALSE to disregard the request.

      @requirement
      Mandatory.

      @scope
      Static.
    */ 
    jbool_t (*respond_srp)(otg_appctx_t appctx);
  
    /* ===== Function name:  otg_app_uninit */
    /**
      Uninitializes the OTG application.
    
      @param appctx [IN] -- Application context, as received from otg_app_init().
           
      @dependencies
      None.

      @sideeffects
      None.

      @return
      Zero (0) on success; otherwise, an error code. 

      @requirement
      Optional.

      @scope
      Static.
    */
    jresult_t (*otg_app_uninit)(otg_appctx_t appctx);
      /*HSU TEMP FIX*/

    /* ===== Function name: notify_b_peripheral_state_transition */
    /**
      Notifies the application of a transition to the b_peripheral_state and that 
      VBUS rose above the B-Device Session Valid threshold.
      
      @param appctx [IN] -- Application context, as received from otg_app_init().

      @dependencies
      None.

      @sideeffects
      None.

      @return
      None.

      @requirement
      Mandatory.

      @scope
      Static.
    */
    void (*notify_b_peripheral_state_transition)(otg_appctx_t appctx);
       /*END OF HSU TEMP FIX*/
} otg_app_callbacks_t;

/** OTG application info.
*/
typedef struct {
#define DROP_VBUS_TIMEOUT_INFINITE 0xFFFF
    juint16_t drop_vbus_timeout;           /**< VBUS drop timeout. */
} otg_app_info_t;

/* ===== Function name:  otg_app_init */
/**
  Initializes the OTG application.

  This function receives and stores the OTG core context required by the OTG 
  core functions. It returns to the OTG core:
   - The application's callback functions
   - The application context to be passed to these callbacks

  @param core_ctx       [IN] -- OTG core context to be passed as an argument 
                                to the OTG core functions.
  @param *otg_app_cbs  [OUT] -- Pointer to a pre-allocated application callback
                                functions structure to be filled by the function,
  @param *otg_app_info [OUT] -- Pointer to a pre-allocated application information 
                                data structure to be filled by the function.
  @param *appctx       [OUT] -- Pointer to the application context to be passed 
                                as an argument to the application's callbacks.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0)on success; otherwise, an error code. 

  @scope
  Global.
*/
jresult_t otg_app_init(otg_ctx_t core_ctx, otg_app_callbacks_t *otg_app_cbs, 
    otg_app_info_t *otg_app_info, otg_appctx_t *appctx);

/* ******************************************** *
 *      OTG Core --> Application Functions      *
 * ******************************************** */
/* ===== Function name:  otg_enable_hnp */
/**
  Instructs the OTG core, while acting as a host, to enable role switching.

  While acting as an A-device, the OTG core does the following so the connected
  B-device can initiate the HNP: 
   - Sends a SetFeature command for enabling HNP on the connected B-device
   - Switches to suspend

  While acting as a B-device, the OTG core switches to suspend so the connected 
  A-device can revert back to acting as host.

  @param core_ctx [IN] -- OTG core context, as received by otg_app_init().

  @dependencies
  None.

  @sideeffects
  None.

  @return
   Zero (0)on success; otherwise, an error code. 

  @scope
  Global.
*/
jresult_t otg_enable_hnp(otg_ctx_t core_ctx);


/* ===== Function name:  otg_start_hnp */
/**
  Instructs the OTG core, while acting as a B-device, to start HNP in order to 
  become a host.

  @note A B-device can start HNP only after it has been HNP enabled.

  @param core_ctx [IN] -- OTG core context, as received by otg_app_init().

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0)on success; otherwise, an error code. 

  @scope
  Global.
*/
jresult_t otg_start_hnp(otg_ctx_t core_ctx);


/* ===== Function name:  otg_start_srp */
/**
  Initializes an SRP process.
 
  @param core_ctx [IN] -- OTG core context, as received by otg_app_init().

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0)on success; otherwise, an error code.

  @scope
  Global.
*/
jresult_t otg_start_srp(otg_ctx_t core_ctx);


/* ===== Function name:  otg_set_whitelist */
/**
  Sets the host's peripherals whitelist, which defines the types of peripherals 
  that the host supports.

  @param core_ctx       [IN] -- OTG core context, as received by otg_app_init().
  @param whitelist_size [IN] -- Number of elements in the whitelist array.
  @param *whitelist     [IN] -- Array of peripheral matcher structures, which
                                define the host's peripherals whitelist (i.e. 
                                the type of peripherals that the host supports).
       
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0)on success; otherwise, an error code. 

  @scope
  Global.
*/
jresult_t otg_set_whitelist(otg_ctx_t core_ctx, juint32_t whitelist_size,
    peripheral_matcher_t *whitelist);


/* ===== Function name:  otg_vbus_shutdown */
/**
  Shuts down the VBUS to the connected device.

  @param core_ctx [IN] -- OTG core context, as received by otg_app_init().
       
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0)on success; otherwise, an error code. 

  @scope
  Global.
*/
jresult_t otg_vbus_shutdown(otg_ctx_t core_ctx);


/* ===== Function name:  otg_vbus_powerup */
/**
  Powers-up the VBUS to the connected device.

  @param core_ctx [IN] -- OTG core context, as received by otg_app_init().
       
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0)on success; otherwise, an error code. 

  @scope
  Global.
*/
jresult_t otg_vbus_powerup(otg_ctx_t core_ctx);

/** @} */ /* end_group hsusb_jungo_api */

#endif
