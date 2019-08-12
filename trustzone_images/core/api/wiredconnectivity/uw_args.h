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


/**@file uw_args.h

Contains Jungo USB stack (USBWare) arguments declarations. 

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_args.h#1 $
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


#ifndef _UW_ARGS_H_
#define _UW_ARGS_H_

/* HSU Addition */
#include "jtypes.h"
/* HSU Addition - End */


/** @ingroup hsusb_jungo_api 
@{ */

/** Device function type.
*/
typedef enum
{
    DEVICE_TYPE_NONE,      /**< &nbsp; */
    DEVICE_TYPE_MSD,       /**< &nbsp; */
    DEVICE_TYPE_HID,       /**< &nbsp; */
    DEVICE_TYPE_SERIAL,    /**< &nbsp; */
    DEVICE_TYPE_LOOPBACK,  /**< &nbsp; */
    DEVICE_TYPE_TEST,      /**< &nbsp; */
    DEVICE_TYPE_CDC_ACM,   /**< &nbsp; */
    DEVICE_TYPE_CDC_ECM,   /**< &nbsp; */
    DEVICE_TYPE_CDC_OBEX,  /**< &nbsp; */
    DEVICE_TYPE_VIDEO,     /**< &nbsp; */
    DEVICE_TYPE_DFU,       /**< &nbsp; */
    DEVICE_TYPE_SICD,      /**< &nbsp; */
    DEVICE_TYPE_AUDIO,     /**< &nbsp; */
    DEVICE_TYPE_VENDOR     /**< &nbsp; */
} fd_type_t;

/** Type that indicates whether USB stack will be started as the Host, 
  Device, or both. 
*/
typedef enum
{   
    UW_INIT_HOST = 1,    /**< -- Initializes the USBWare host stack. */
    UW_INIT_DEVICE,      /**< -- Initializes the USBWare device stack. */
    UW_INIT_HOST_DEVICE, /**< -- Initializes both the USBWare host and device
						         stacks. */
    UW_INIT_OTG          /**< -- Initializes the USBWare OTG stack (includes 
                                 the host and device stacks). */
}
uw_init_opt_t;

typedef jresult_t (*fd_init_func_t)(void *ctx);

/** Function driver instance.
*/
typedef struct 
{
    fd_type_t type;           /**< Function driver type. */
    juint8_t count;           /**< Number of instances of the type. */

    fd_init_func_t init_func;
      /**< Optional parameter for vendor-specific function drivers.\ This 
           requires the type to be FD_TYPE_VENDOR. */

    /* HSU Addtion */
    juint8_t vendor_specific_subtype;
      /**< Used by the config selector to determine the function driver type 
           in case it is a vendor-specific function driver. */
    /* HSU Addition - End */

} fd_instance_t;

typedef jbool_t (*self_powered_func_t)(void);

/** Device information.
*/
typedef struct {
    self_powered_func_t self_powered_func; /**< Function to be called for 
                                                a self-powered device. */
    juint_t self_powered : 1;   /**< Specifies whether the device is
								     self-powered. */
    juint_t srp_support: 1;     /**< Specifies whether SRP is supported. */
    juint_t hnp_support: 1;     /**< Specifies whether HNP is supported. */
    
    juint16_t vendor;           /**< Vendor ID. */
    juint16_t product;          /**< Product ID. */
    juint16_t release;          /**< Release ID. */

    juint8_t dev_class;         /**< Device class. */
    juint8_t dev_sub_class;     /**< Device subclass. */
    juint8_t dev_protocol;      /**< Device protocol. */

    char *product_name;         /**< Product name. */
    char *product_manufacturer; /**< Product manufacturer. */
    char *product_serial;       /**< Product serial number. */
    
    /* HSU temp fix */
    juint16_t bcdUSB;           /**< Binary-coded decimal USB descriptor. */
    /* HSU temp fix */

    juint8_t  U1_exit_latency;  /**< Super-Speed U1 exit latency. */
    juint16_t U2_exit_latency;  /**< Super-Speed U2 exit latency. */

    /*HSU Addition*/
    char *configuration;        /**< String describing the device
								     configuration. */
    /*End of HSU addition*/
} dev_info_t;

/** Device configuration information.
*/
typedef struct
{
    juint8_t num_of_fd_instances;  /**< Number of instances. */
    fd_instance_t *fd_instances;   /**< Pointer of the FD instance. */
    char *config_name;             /**< Configuration name. */
    juint_t self_powered : 1;      /**< Specifies whether the device is 
                                        self-powered. */
    juint_t remote_wakeup: 1;      /**< Specifies whether Remote Wakeup is 
                                        supported. */
    juint8_t max_power;            /**< Maximum power supported in 2 mA
                                        units. */
} config_param_t;

typedef jresult_t (*vendor_cmd_in_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint8_t *resp_buf, juint16_t resp_buf_size, 
    juint16_t *act_resp_buf_size);
typedef jresult_t (*vendor_cmd_out_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint16_t wLength);
typedef jresult_t (*vendor_cmd_out_data_stage_cb_t)(juint8_t *data, 
    juint16_t size);

/*HSU addition*/
typedef jresult_t (*reserved_cmd_in_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint8_t *resp_buf, juint16_t resp_buf_size, 
    juint16_t *act_resp_buf_size);
typedef jresult_t (*reserved_cmd_out_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint16_t wLength);
typedef jresult_t (*reserved_cmd_out_data_stage_cb_t)(juint8_t *data, 
    juint16_t size);
/*End of HSU addition*/

typedef char* (*vendor_get_string_cb_t)(juint8_t index);

/* HSU addition - standard_cmd_cb_t */
typedef void (*standard_cmd_cb_t)(juint8_t bmRequestType,
    juint8_t bRequest, juint16_t wValue, juint16_t wIndex, juint16_t wLength);

/** Device parameters.
*/
typedef struct
{ 
    jbool_t auto_enable;          /**< Specifies whether the device is 
                                       auto-enabled. */
    
    /* HSU addition: removed featurization */
    jbool_t full_speed_only;      /**< Specifies whether the device supports 
                                       FS-USB only. */
    juint8_t max_speed;           /**< Max supported speed */
    dev_info_t *dev_info;         /**< Pointer to device information. */
    juint8_t num_of_configs;      /**< Number of supported configurations. */
    config_param_t *config_param; /**< Configuration parameter. */
    vendor_get_string_cb_t vendor_get_string_cb;
         /**< Callback for getting the Vendor String. */

    vendor_cmd_in_cb_t vendor_cmd_in_cb;
         /**< Callback for incoming vendor-specific requests. */

    vendor_cmd_out_cb_t vendor_cmd_out_cb;
         /**< Callback for outgoing vendor-specific requests. */

    vendor_cmd_out_data_stage_cb_t vendor_cmd_out_data_stage_cb;
         /**< Callback for outgoing vendor-specific requests during the 
              data stage. */

    /*HSU addition*/    
    reserved_cmd_in_cb_t reserved_cmd_in_cb;
         /**< Callback for reserved incoming requests. */

    reserved_cmd_out_cb_t reserved_cmd_out_cb;
         /**< Callback for reserved outgoing requests. */

    reserved_cmd_out_data_stage_cb_t reserved_cmd_out_data_stage_cb; 
         /**< Callback for reserved outgoing requests during the data stage. */
    /*End of HSU addition*/

    /* HSU addition - standard_cmd_cb_t */
     standard_cmd_cb_t standard_cmd_cb;  /**< Callback for standard 
                                              requests. */
                                        
    jbool_t static_interface_number;     /**< Static interface number. */
} device_params_t;

/** USB core parameter.
*/
typedef struct
{
    juint8_t core_number;          /**< Core number. */
    uw_init_opt_t init_opt;        /**< Initialization options information. */
    
    /* Relevant only for device stack */
    device_params_t *device_param; /**< Device parameter (only relevant for
								        device stack). */
} uw_hw_core_param_t;

/** USBWare arguments.
*/
typedef struct 
{
    /* For static memory management */
    void *mem_buf;               /**< Memory buffer. */
    juint32_t mem_buf_size;      /**< Memory buffer size. */

    void *dma_vbuf;              /**< DMA virtual buffer. */
    void *dma_pbuf;              /**< DMA physical buffer. */
    juint32_t dma_buf_size;      /**< DMA buffer size. */
    void *dma_cachable_vbuf;     /**< Pointer to the DMA cachable virtual 
                                      buffer. */
    void *dma_cachable_pbuf;     /**< Pointer to the DMA cachable physical 
                                      buffer. */
    juint32_t dma_cachable_size; /**< DMA cachable buffer size. */

    juint8_t num_of_cores;               /**< Number of USB cores. */
    uw_hw_core_param_t *hw_core_params;  /**< USB core parameters. */
} uw_args_t;

uw_args_t *get_uw_args(void);      

/** @} */ /* end_group hsusb_jungo_api */

#endif
