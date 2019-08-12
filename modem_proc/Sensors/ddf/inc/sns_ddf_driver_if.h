/** 
 * @file sns_ddf_driver_if.h 
 * @brief Device Driver Interface. 
 *  
 * A collection of definitions that device drivers require in order to interact 
 * with the Sensors Manager "above" and devices "below. 
 *  
 * Copyright (c) 2010-2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential. 
 *  
 * @see sns_ddf_smgr_if.h 
 * @see sns_ddf_comm.h 
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2015-03-31 hw   Remove the redefinition of typedef sns_ddf_device_access_s
  2014-09-02 jms  Added SPI support
  2014-03-24 tc   Added cancel_daf_trans
  2014-02-14 tc   Added process_daf_req
  2013-07-12 lka  Added compile-time configurations
  2013-05-05 sd   Added trigger_fifo_data
  2013-02-14 jtl  Added probe function to the driver interface.
  2011-12-26 yk   Modified the definition of handle_irq to use GPIO number.
                  Added GPIO numbers to sns_ddf_device_access_s.
                  Added support for self-scheduled data retrieval.
  2011-08-16 yk   Added sensor test interface function.
  2011-04-12 yk   Added irq signal handling interface.
  2010-09-24 yk   Added memhandler object and associated changes. Renamed some
                  parameters. Removed length parameter from get_data().
  2010-08-16 yk   Changed driver instance pointer to be of a handle type.
                  Changed device_access_s::io_handle to a port_config_t.
                  Renamed sns_ddf_config_s to sns_ddf_nv_params_s.
  2010-08-14 yk   Renamed identifiers to conform to DSPS naming convention.
  2010-07-30 yk   Initial revision
==============================================================================*/

#ifndef SNS_DDF_DRIVER_IF_H
#define SNS_DDF_DRIVER_IF_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_memhandler.h"


/*----------------------------------------------------------------------------
 * Forward Declarations
 * -------------------------------------------------------------------------*/

/** Temporarily here until the NV module becomes available. */
typedef uint32_t nv_id_t;


/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

/** 
 * Device access information.
 */
struct sns_ddf_device_access_s
{
     /** Device select when a family of devices is controlled by one driver. */
    uint8_t                device_select;

     /** Handle used to identify I/O port info when accessing a device. */
    sns_ddf_port_config_s  port_config;

    /** 
     * GPIO numbers assigned to this device's interrupt lines. The actual
     * meaning of "first" and "second" is defined by the driver and configured
     * where the driver is installed. A value of 0 means no GPIO.
     */
    uint32_t  first_gpio;
    uint32_t  second_gpio;
#ifdef ADSP_HWCONFIG_L
    uint8_t   flags;
#endif

};


/** 
 * Driver non-volatile configuration data retrieved from the sensors registry.
 */
typedef struct
{
    nv_id_t           nvitem_grp; /**< Type of item (NONE, SINGLE, or GROUP). */
    uint32_t          nvitem_id;  /**< Identifier of the NV item retrieved. */
    sns_ddf_status_e  status;     /**< Success status of NV item retrieval. */
    uint8_t*          data;       /**< NV item value, if gotten successfully. */
    uint16_t          data_len;   /**< Length of @a data in bytes. */
    
} sns_ddf_nv_params_s;


/**
 * @brief Device driver (DD) interface. 
 *  
 * The interface by which the Sensors Manager accesses drivers. Drivers must
 * implement all functions, and export them such that they are available to the 
 * static installer at compile time. Drivers may define additional functions and 
 * variables, but none may be global. I.e. all functions operate on a specific 
 * instance of a driver so that multiple instances of the same driver may be 
 * instantiated. A driver instance is defined by the driver and allocated in 
 * its init() function. A handle to that instance is then passed into every 
 * function so it is managed throughout its scope.
 */
typedef struct
{
    /** 
     * @brief Initializes the driver and sets up devices.
     *  
     * Allocates a handle to a driver instance, opens a communication port to 
     * associated devices, configures the driver and devices, and places 
     * the devices in the default power state. Returns the instance handle along 
     * with a list of supported sensors. This function will be called at init 
     * time.
     *  
     * @param[out] dd_handle_ptr  Pointer that this function must malloc and 
     *                            populate. This is a handle to the driver
     *                            instance that will be passed in to all other
     *                            functions. NB: Do not use @a memhandler to
     *                            allocate this memory.
     * @param[in]  smgr_handle    Handle used to identify this driver when it 
     *                            calls into Sensors Manager functions.
     * @param[in]  nv_params      NV parameters retrieved for the driver.
     * @param[in]  device_info    Access info for physical devices controlled by 
     *                            this driver. Used to configure the bus
     *                            and talk to the devices.
     * @param[in]  num_devices    Number of elements in @a device_info. 
     * @param[in]  memhandler     Memory handler used to dynamically allocate 
     *                            output parameters, if applicable. NB: Do not
     *                            use memhandler to allocate memory for
     *                            @a dd_handle_ptr.
     * @param[in/out] sensors     List of supported sensors, allocated, 
     *                            populated, and returned by this function.
     * @param[in/out] num_sensors Number of elements in @a sensors.
     *
     * @return Success if @a dd_handle_ptr was allocated and the driver was 
     *         configured properly. Otherwise a specific error code is returned.
     */
    sns_ddf_status_e (*init)(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e**       sensors,
        uint32_t*                num_sensors);
        
    /** 
     * @brief Retrieves a single set of sensor data.
     *  
     * Requests a single sample of sensor data from each of the specified
     * sensors. Data is returned in one of two ways: (1) immediately after being 
     * read from the sensor, in which case data is populated in the same order 
     * it was requested, or (2) in cases where the sensor requires several steps 
     * to be read, this function will return with the status SNS_DDF_PENDING, 
     * and provide the data asynchronously via @a sns_ddf_smgr_data_notify() 
     * when it is ready. Note that @a sns_ddf_smgr_data_notify() must be called 
     * even in the event of an error in order to report a failed status. An 
     * asynchronous notification is also expected in the case of mixed data 
     * (i.e. synchronous and asynchronous).
     * 
     * @note In the case where multiple sensors are requested, the driver must 
     *       attempt to collect data from all requested sensors, meaning that
     *       the time it takes to execute this function will be determined by
     *       the number of sensors sampled, and their various delays. Drivers
     *       must never return partial responses. If a sensor has failed or
     *       isn't available, @a sns_ddf_sensor_data_s.status must be used to
     *       reflect this status. 
     * 
     * @param[in]  dd_handle    Handle to a driver instance.
     * @param[in]  sensors      List of sensors for which data is requested.
     * @param[in]  num_sensors  Number of elements in @a sensors.
     * @param[in]  memhandler   Memory handler used to dynamically allocate 
     *                          output parameters, if applicable.
     * @param[out] data         Sampled sensor data. The number of elements must 
     *                          match @a num_sensors.
     *
     * @return SNS_DDF_SUCCESS if data was populated successfully. If any of the 
     *         sensors queried are to be read asynchronously SNS_DDF_PENDING is
     *         returned and data is via @a sns_ddf_smgr_data_notify() when
     *         available. Otherwise a specific error code is returned.
     *  
     * @see sns_ddf_data_notify()
     */
    sns_ddf_status_e (*get_data)(
        sns_ddf_handle_t         dd_handle,
        sns_ddf_sensor_e         sensors[],
        uint32_t                 num_sensors,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_data_s**  data);

    /**
     * @brief Sets a sensor attribute to a specific value.
     *
     * @param[in] dd_handle  Handle to a driver instance.
     * @param[in] sensor     Sensor for which this attribute is to be set. When 
     *                       addressing an attribute that refers to the driver
     *                       this value is set to SNS_DDF_SENSOR__ALL.
     * @param[in] attrib     Attribute to be set.
     * @param[in] value      Value to set this attribute.
     *
     * @return Success if the value of the attribute was set properly. Otherwise 
     *         a specific error code is returned.
     */
    sns_ddf_status_e (*set_attrib)(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

    /**
     * @brief Retrieves the value of an attribute for a sensor.
     * 
     * @param[in]  dd_handle   Handle to a driver instance.
     * @param[in]  sensor      Sensor whose attribute is to be retrieved. When 
     *                         addressing an attribute that refers to the driver
     *                         this value is set to SNS_DDF_SENSOR__ALL.
     * @param[in]  attrib      Attribute to be retrieved.
     * @param[in]  memhandler  Memory handler used to dynamically allocate 
     *                         output parameters, if applicable.
     * @param[out] value       Pointer that this function will allocate or set 
     *                         to the attribute's value.
     * @param[out] num_elems   Number of elements in @a value.
     *  
     * @return Success if the attribute was retrieved and the buffer was 
     *         populated. Otherwise a specific error code is returned.
     */
    sns_ddf_status_e (*get_attrib)(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);
      
   /**
    * @brief Called when the timer set by this driver has expired. This must be 
    *        the callback function submitted when initializing a timer.
    *  
    * @note This will be called within the context of the Sensors Manager task.
    * 
    * @param[in] dd_handle  Handle to a driver instance.
    * @param[in] arg        The argument submitted when the timer was set. 
    * 
    * @see sns_ddf_set_timer() 
    */
    void (*handle_timer)(sns_ddf_handle_t dd_handle, void* arg);

   /** 
    * @brief Called in response to an interrupt for this driver.
    *  
    * @note This function will be called within the context of the SMGR task, 
    *       *not* the ISR. 
    *
    * @param[in] dd_handle  Handle to a driver instance. 
    * @param[in] gpio_num   GPIO number that triggered this interrupt.
    * @param[in] timestamp  Time at which interrupt happened.
    */
    void (*handle_irq)(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp);

   /**
    * @brief Resets the driver and device so they return to the state they were 
    *        in after init() was called.
    *
    * @param[in] dd_handle  Handle to a driver instance.
    * 
    * @return Success if the driver was able to reset its state and the device.
    *         Otherwise a specific error code is returned. 
    */
    sns_ddf_status_e (*reset)(sns_ddf_handle_t dd_handle);

    /**
     * @brief Runs a factory test case.
     *  
     * Tests may include embedded hardware tests in cases where the sensor 
     * supports it, as well as driver based sensor tests. This is generally run 
     * in a factory setting and must not be called while a device is streaming 
     * data. 
     *  
     * @param[in]  dd_handle  Handle to a driver instance. 
     * @param[in]  sensor     Sensor on which to run the test. 
     * @param[in]  test       Test case to run. 
     * @param[out] err        Optional driver-specific error code.
     *  
     * @return One of the following error codes:
     *     SNS_DDF_SUCCESS        - Test passed.
     *     SNS_DDF_PENDING        - Test result will be sent as an event.
     *     SNS_DDF_EDEVICE_BUSY   - Device is busy streaming, cannot run test.
     *     SNS_DDF_EINVALID_TEST  - Test is not defined for this sensor.
     *     SNS_DDF_EINVALID_PARAM - One of the parameters is invalid.
     *     SNS_DDF_EFAIL          - Unknown error occurred.
     */
    sns_ddf_status_e (*run_test)(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err);

    /**
     * @brief Begins device-scheduled sampling and enables notification via Data 
     *        Ready Interrupts (DRI).
     *
     * The driver commands the device to begin sampling at the configured 
     * ODR (@a SNS_DDF_ATTRIB_ODR) and enables DRI. When data is ready, the 
     * driver's handle_irq() function is called and the driver notifies 
     * SMGR of the event via @a sns_ddf_smgr_notify_event() and @a 
     * SNS_DDF_EVENT_DATAREADY. 
     *  
     * @param[in] handle  Handle to the driver's instance.
     * @param[in] sensor  Sensor to be sampled.
     * @param[in] enable  True to enable or false to disable data stream.
     * 
     * @return SNS_DDF_SUCCESS if sensor was successfully configured and 
     *         internal sampling has commenced or ceased. Otherwise an
     *         appropriate error code.
     */
    sns_ddf_status_e (*enable_sched_data)(
        sns_ddf_handle_t  handle,
        sns_ddf_sensor_e  sensor,
        bool              enable);

    /**
     * @brief Probes for the device with a given configuration.
     *
     * This commands the driver to look for the device with the specified
     * configuration (ie, I2C address/bus defined in the sns_ddf_device_access_s
     * struct.
     *
     * @param[in]  dev_info    Access info for physical devices controlled by 
     *                         this driver. Used to determine if the device is
     *                         physically present.
     * @param[in]  memhandler  Memory handler used to dynamically allocate 
     *                         output parameters, if applicable.
     * @param[out] num_sensors Number of sensors supported. 0 if none.
     * @param[out] sensor_type Array of sensor types supported, with num_sensor
     *                         elements. Allocated by this function.
     *
     * @return SNS_DDF_SUCCESS if the part was probed function completed, even
     *         if no device was found (in which case num_sensors will be set to
     *         0).
     */
    sns_ddf_status_e(*probe)(
        sns_ddf_device_access_s* device_info,
        sns_ddf_memhandler_s*    memhandler,
        uint32_t*                num_sensors,
        sns_ddf_sensor_e**       sensors );
        

    /**
     * @brief Retrieves a set of sensor data. Asynchronous API
     *
     * Requests sample of sensor data from the specified sensor.
     *
     * @note If a sensor has failed or
     *       isn't available, @a sns_ddf_sensor_data_s.status must be used to
     *       reflect this status.
     *
     * @param[in]  dd_handle    	Handle to a driver instance.
     * @param[in]  sensor      	sensor for which data is requested.
     *
     * @param[in]  num_samples  	number of samples to retrieve as available. Drain the FIFO if value is set to Zero.
     * @param[in]  trigger now  	trigger notify fifo data now or
     *       later when trigger_now is set to true.
     *
     *
     * @return SNS_DDF_SUCCESS 	if data was populated successfully.
     *         via sns_ddf_smgr_data_notify() or if trigger_now is
     *         set to false; Otherwise a specific error code is
     *         returned.
     *
     * @see sns_ddf_data_notify_data() as this will be used to report the data.
     */
    sns_ddf_status_e (*trigger_fifo_data)(
        sns_ddf_handle_t         dd_handle,
        sns_ddf_sensor_e         sensor,
        uint16_t                 num_samples,
        bool                     trigger_now);


    /**
     * @brief Delivers a Driver Access Framework message to the driver.
     *        Asynchronous/Synchronous API.
     *
     * @detail
     *
     * @param[in]  dd_handle     Handle to a driver instance.
     * @param[in]  req_id        Request identifier.
     * @param[in]  req_msg       Request message in the opaque payload. If no
     *                           payload is supplied, then this pointer will be
     *                           null.
     * @param[in]  req_size      Number of bytes in @req_msg. If req_msg is empty,
     *                           this value must be 0.
     * @param[in]  memhandler    Memory handler used to dynamically allocate
     *                           output parameters, if applicable.
     * @param[out] resp_msg      Pointer to the output message pointer. The output
     *                           message must be allocated first using @memhandler.
     * @param[out] resp_size     Pointer to number of bytes in @resp_msg. If there
     *                           is no DAF response message for the request, then
     *                           this must be 0 to show that the DAF response is
     *                           not present. Response messages are limited in
     *                           size to @SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01 bytes.
     *                           Any response message larger than
     *                           @SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01 bytes will be
     *                           truncated.
     * @param[in]  trans_id_ptr  Pointer to the optional transaction identifier.
                                 This will be null if a transaction ID was not
                                 provided.
     * @param[in]  conn_handle   The connection handle for the request message.
     *                           This value must be saved if the particular request
     *                           is expected to generate indications. Upon
     *                           notifying the SMGR of an indication, this value
     *                           must be provided to the SMGR.
     *
     * @return Success if the message was retrieved and the buffer was correctly
     *         populated. Otherwise a specific error code is returned.
     */
     sns_ddf_status_e (*process_daf_req)(
         sns_ddf_handle_t       dd_handle,
         uint32_t               req_id,
         const void*            req_msg,
         uint32_t               req_size,
         sns_ddf_memhandler_s*  memhandler,
         void**                 resp_msg,
         uint32_t*              resp_size,
         const uint8_t*         trans_id_ptr,
         void*                  conn_handle);


    /**
     * @brief Cancels all of the driver's current Driver Access Framework
     *        asynchronous transactions for the provided connection handle.
     *
     * @note This does not have to cancel a response message in the process of
     *       being created.
     *       This function does not have to be implemented for drivers that do
     *       not support or implement any asynchronous messages (these messages
     *       require the usage of sns_ddf_smgr_notify_daf_ind).
     *
     * @param[in]  dd_handle     Handle to a driver instance.
     * @param[in]  conn_handle   The connection handle for the client that is
     *                           cancelling the Driver Access Framework
     *                           transaction.
     */
     void (*cancel_daf_trans)(
         sns_ddf_handle_t       dd_handle,
         void*                  conn_handle);

} sns_ddf_driver_if_s;

#endif
