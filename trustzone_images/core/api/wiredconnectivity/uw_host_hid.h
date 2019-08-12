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


/**
@file uw_host_hid.h

Generic USB Human Interface Device (HID) class driver API.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_host_hid.h#1 $
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


/* Generic HID Class Driver API */

#ifndef _UW_HOST_HID_H_
#define _UW_HOST_HID_H_

#include "jtypes.h"
#include <uw_host_hid_common.h> /* Common HID Class Driver API */


/** @ingroup hsusb_jungo_api 
@{ */

/* Device operation flags: */

/** If set, the application's read completion callback (notify_read_complete)
  is called whenever data is available from the device.
  */
#define NON_BLOCKING_READ  0x1

/** Flags bitmask.
*/
typedef juint32_t flag_t;

/** Generic HID class driver API device context.
*/
typedef void* hid_gen_dev_ctx_t;

/** Private HID class driver application data.
*/
typedef void* hid_gen_app_ctx_t;

/************************************************************
 * HID Class Driver Application -> Generic HID Class Driver *
 ************************************************************/
/** HID class driver application notification callbacks structure. 
    Callback parameters:
    - app_ctx          -- Private application context.
    - hid_gen_dev_ctx  -- Generic HID API device context.
    - report_id        -- Report ID.
    - report_size      -- Report size (in bytes). 
 */
typedef struct
{
    jbool_t (*notify_attach)(hid_gen_app_ctx_t app_ctx,
        hid_gen_dev_ctx_t hid_gen_dev_ctx);
      /**< Device attach callback.\ Requirement: Mandatory. */


    void (*notify_detach)(hid_gen_app_ctx_t app_ctx,
        hid_gen_dev_ctx_t hid_gen_dev_ctx);
      /**< Device detach callback.\ Requirement: Mandatory. */

    void (*notify_read_complete)(hid_gen_app_ctx_t app_ctx, 
        hid_gen_dev_ctx_t hid_gen_dev_ctx, juint8_t report_id,
        juint32_t report_size);
      /**< Read completion callback.\ Requirement: Mandatory. */
} notify_t;

#define REPORT_POOL_SIZE 5

/****************************************************************
 * Generic HID Class Driver API -> HID Class Driver Application *
 ****************************************************************/
/* //////////////////////////////
     Device Operation Functions 
   ////////////////////////////// */

/*****************************************************************************
 * hid_general_register: */
/**
  Registers an application with the generic HID class driver API.

  If devices are already attached when the function is called, the function 
  calls the application's attach notification callback for each of the attached 
  devices.
 
  @note This function must be called before any other generic HID class driver 
  function.
 
  @param *notify_callback [IN] -- Pointer to an application notification callback 
                                  structure.
  @param app_ctx          [IN] -- Private application data to be passed as a 
                                  parameter to the application's callback 
                                  functions (see notify_t).
 
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_register(notify_t *notify_callback,
    hid_gen_app_ctx_t app_ctx);

/*****************************************************************************
 * hid_general_open: */
/**
  Enables and starts operating an attached HID device.
 
  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, 
                                 as received in the notify_attach() callback.
  @param flags           [IN] -- A bitmask of device operation flags. The 
                                 currently supported flag is NON_BLOCKING_READ. 
                                 Notifications on data availability are sent to 
                                 the application using the 
                                 notify_read_complete() callback.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  On success, the generic HID class driver API device context to be used in 
  calls to the other generic HID class driver API is enabled and started.\n
  NULL on failure.
*/
hid_gen_dev_ctx_t hid_general_open(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    flag_t flags);

/*****************************************************************************
 * hid_general_close: */
/**
  Disables and stops operating an attached HID device.
 
  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, 
                            as received from hid_general_open().
 
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_close(hid_gen_dev_ctx_t hid_gen_dev_ctx);

/*****************************************************************************
 * hid_general_read: */
/**
  Returns the report data received from an open device.
  
  If no data is available when the function is called:
   - If the NON_BLOCKING_READ flag is turned on, the function returns with the 
     status JEAGAIN, and the application's notify_read_complete() callback
     function will be called when data arrives.
   - If the NON_BLOCKING_READ flag is turned off, the read function will block 
     until data arrives.

  @note This function does not remove the report ID from the report data.

  @param hid_gen_dev_ctx  [IN] -- Generic HID class driver API device context, as 
                                  received from hid_general_open().
  @param *read_buf       [OUT] -- Pointer to a data buffer to be filled with the 
                                  read data.
  @param report_id        [IN] -- Report ID.
 
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_read(hid_gen_dev_ctx_t hid_gen_dev_ctx, void *read_buf, 
    juint8_t report_id);

/*****************************************************************************
 * hid_general_write: */
/**
  Sends report data to an open device.\ The function will block until the data 
  is sent.
 
  @note If the report ID is other than 0, the application must add a report 
  ID in the first byte of the report data.

  @param hid_gen_dev_ctx  [IN] -- Generic HID class driver API device context, as 
                                  received from hid_general_open().
  @param *write_buf      [OUT] -- Pointer to a data buffer containing the data to 
                                  be written to the device.
  @param report_id        [IN] -- Report ID (used for size validation).
  @param type             [IN] -- Report type. Can be either UHID_INPUT_REPORT, 
						  	      UHID_OUTPUT_REPORT, or UHID_FEATURE_REPORT.
 
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_write(hid_gen_dev_ctx_t hid_gen_dev_ctx, void *write_buf,
    juint8_t report_id, juint8_t type);

/*****************************************************************************
 * hid_general_set_idle: */
/**
  Sends a Set_Idle request to an open device.

  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, as 
                                 received from hid_general_open().
  @param report_id       [IN] -- Report ID (0 = all report IDs). 
  @param idle_rate       [IN] -- Idle rate (0 = infinite).
  
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_set_idle(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint8_t report_id, juint8_t idle_rate);

/*****************************************************************************
 * hid_general_get_idle: */
/**
  Sends a Get Idle request to an open device and returns the idle rate value.

  @param hid_gen_dev_ctx  [IN] -- Generic HID class driver API device context, 
                                  as received from hid_general_open().
  @param report_id        [IN] -- Report ID: 0 = all report IDs. 
  @param *idle_rate      [OUT] -- Idle rate: 0 = infinite.
 
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_get_idle(hid_gen_dev_ctx_t hid_gen_dev_ctx, 
    juint8_t report_id, juint8_t *idle_rate);

/*****************************************************************************
 * hid_general_set_protocol: */
/**
  Sends a Set Protocol request to an open device.

  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, as 
                                 received from hid_general_open().
  @param protocol_value  [IN] -- Protocol value: 0 = Boot Protocol, 1 = Report 
                                 Protocol.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_set_protocol(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint8_t protocol_value);

/*****************************************************************************
 * hid_general_get_protocol: */
/**
  Sends a Get Protocol request to an open device and returns the protocol value.

  @param hid_gen_dev_ctx  [IN] -- Generic HID class driver API device context, 
                                  as received from hid_general_open().
  @param *protocol_value [OUT] -- Protocol value: 0 = Boot Protocol, 1 = Report 
                                  Protocol.
  
  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_get_protocol(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint8_t *protocol_value);

/* /////////////////////////////////
     Report Manipulation Functions 
   ///////////////////////////////// */

/*****************************************************************************
 * hid_general_is_collection: */
/**
  Checks if an open HID device's report descriptor has the specified usage item.
        
  @note This function can be used to verify the HID device's type (e.g. mouse, 
  keyboard, gamepad, etc.).
              
  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, as 
                                 received from hid_general_open().
  @param usage           [IN] -- Usage code created using the HID_USAGE2 macro.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  1 = The report descriptor has the specified usage item.\n
  0 = Otherwise.
*/
jbool_t hid_general_is_collection(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint32_t usage);

/*****************************************************************************
 * hid_general_get_field_location: */
/**
  Returns the field location for a given usage code in the specified report.

  The field location should be used in calls to functions that require a field 
  location parameter, such as hid_general_get_field_value() or 
  hid_general_set_field_value().

  @note The returned field location must later be freed by the application 
  using hid_general_free_location().

  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, as 
                                 received from hid_general_open().
  @param usage           [IN] -- Usage code created using the HID_USAGE2 macro.
  @param type            [IN] -- Report type. Can be either UHID_INPUT_REPORT,
						         UHID_OUTPUT_REPORT, or UHID_FEATURE_REPORT.
  @param report_id       [IN] -- Report ID.
  @param **location     [OUT] -- Pointer to a pointer to the field location, to be 
                                  returned by the function.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0)if the specified usage code is found in the report.\n
  JENOENT otherwise.
*/
jresult_t hid_general_get_field_location(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint32_t usage, juint8_t type, juint8_t report_id, void **location);

/*****************************************************************************
 * hid_general_free_location: */
/**
  Frees field location resources previously allocated using 
  hid_general_get_field_location().

  @note This function should be called from the application's device detach
        function for each allocated field location.

  @param **location [IN/OUT] -- Pointer to a pointer to field location
                                resources received from a previous call to 
                                hid_general_get_field_location(). The function 
                                resets the resources pointer to NULL.
 
  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.
*/
void hid_general_free_location(void **location);

/*****************************************************************************
 * hid_general_get_field_value: */
/**
  Returns the value of a given report field, specified by its location.

  @param *location    [IN] -- Pointer to the field location.
  @param *report_data [IN] -- Raw data received from a previous call to 
                              hid_general_read().

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Value of the specified field.
*/
juint32_t hid_general_get_field_value(void *location, void* report_data);

/*****************************************************************************
 * hid_general_set_field_value: */
/**
  Sets the value of a given report field, specified by its location.

  @param  *location    [IN] -- Pointer to the field location.
  @param *report_data [OUT] -- A pre-allocated report data buffer. After the 
                               function successfully sets the specified field 
                               value in this buffer, the calling application is 
                               to pass the buffer hid_general_write() in order 
                               to send the report data to the relevant device.
  @param  value        [IN] -- The field value to set.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_set_field_value(void *location, void* report_data,
    juint32_t value);

/*****************************************************************************
 * hid_general_is_location_of_usage: */
/**
  Verifies usage of a given report field, specified by its location.

  @param usage     [IN] -- Expected usage code, created using the HID_USAGE2 
                           macro.
  @param *location [IN] -- Pointer to the field location.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  TRUE = The actual usage matches the input usage value.\n
  FALSE = Otherwise.
*/
jbool_t hid_general_is_location_of_usage(juint32_t usage, void *location);

/*****************************************************************************
 * hid_general_get_att_from_location: */
/**
  Returns the usage attributes for a given report field, specified by its 
  location.

  @param *location [IN] -- Pointer to the field location.
  @param *att     [OUT] -- Pointer to the usage attributes information.
 
  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.
*/
void hid_general_get_att_from_location(void *location, usage_att_t *att);

/*****************************************************************************
 * hid_general_get_report_size: */
/**
  Returns the size of the specified report.

  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, 
                                 as received from hid_general_open().
  @param report_id       [IN] -- Report ID.
  @param type            [IN] -- Report type. Can be either UHID_INPUT_REPORT,
                                 UHID_OUTPUT_REPORT, or UHID_FEATURE_REPORT.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Size of the report, in bytes.
*/
juint8_t hid_general_get_report_size(hid_gen_dev_ctx_t hid_gen_dev_ctx, 
    juint8_t report_id, juint8_t type);

/*****************************************************************************
 * hid_general_get_usbd_handle: */
/**
  Retrieves the USB core device handle.

  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, 
                                 as received from hid_general_open().

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Pointer to the USB core device handle
*/
void *hid_general_get_usbd_handle(hid_gen_dev_ctx_t hid_gen_dev_ctx);

/*****************************************************************************
 * hid_general_suspend_device: */
/**
  Suspends the specified device.

  @param hid_gen_dev_ctx -- [IN] Generic HID class driver API device context, 
                                 as received from hid_general_open().

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_suspend_device(hid_gen_dev_ctx_t hid_gen_dev_ctx);

/*****************************************************************************
 * hid_general_resume_device: */
/**
  Resumes the specified device.

  @param hid_gen_dev_ctx [IN] -- Generic HID class driver API device context, 
                                 as received from hid_general_open().

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_resume_device(hid_gen_dev_ctx_t hid_gen_dev_ctx);

/* HSU addition starts */
/*****************************************************************************
 * hid_general_get_num_reports: */
/**
  Returns the number of reports in the device report descriptor.

  @param hid_dev [IN] -- Generic HID class driver API device context

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Number of reports.
*/
juint8_t hid_general_get_num_reports(hid_gen_dev_ctx_t hid_dev);
/* HSU addition ends */

/** @} */ /* end_group hsusb_jungo_api */


/** @ingroup hsusb_qc_api 
@{ */

/*=============================================================================
  Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*****************************************************************************
 * hid_general_get_bootprotocol_location:  */
/**
  Retrieves the location from the boot protocol report descriptor.

  @param *bp_desc    [IN] -- Boot protocol report descriptor.
  @param desc_size   [IN] -- Number of bytes in the boot protocol report 
                             descriptor, bp_desc.
  @param usage       [IN] -- Usage to look for.
  @param type        [IN] -- Type of usage.
  @param **location [OUT] -- Pointer to a pointer to the field location.

  @dependencies
  None.

  @sideeffects
  None.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; error code on failure.
*/
jresult_t hid_general_get_bootprotocol_location(
  void * bp_desc,
  juint16_t desc_size,
  juint32_t usage, 
  juint8_t type, 
  void **location );

juint8_t hid_general_get_report_id_by_index
(
  void *sc,
  juint8_t index
);

jresult_t hid_general_get_usage_location(hid_gen_dev_ctx_t hid_dev, 
    juint32_t usage, juint8_t type, juint8_t report_id, void **location);

/* End – Qualcomm HSUSB addition  */ 

/** @} */ /* end_group hsusb_qc_api */

#endif
