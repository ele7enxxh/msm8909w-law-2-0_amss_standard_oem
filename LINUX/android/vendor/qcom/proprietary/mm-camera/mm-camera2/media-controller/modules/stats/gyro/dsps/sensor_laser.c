/* sensor_laser.c
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/
#include <pthread.h>
#include "sensor_laser.h"
//#include "stats_debug.h"
#include <hardware/sensors.h>
#include <dlfcn.h>
#include "camera_dbg.h"
#if 0
#undef CDBG
#define CDBG ALOGE
#endif

#undef CDBG_HIGH
#if 1
#define CDBG_HIGH ALOGE
#else
#define CDBG_HIGH
#endif

#undef CDBG_LOW
#if 0
#define CDBG_LOW ALOGE
#else
#define CDBG_LOW
#endif

/** sensor_laser_thread_handler
 *
 * @arg: arguments to the thread
 *
 * Sensor laser thread handler creates connection with laser
 * sensor module, poll for data and give callback.
 *
 **/
static void* sensor_laser_thread_handler(void *arg)
{
  sensor_laser_obj_t *sensor_l_obj = (sensor_laser_obj_t *)arg;
  struct sensors_poll_device_t *device = sensor_l_obj->device;
  sensors_event_t event;
  int num_of_events = NUMBER_OF_EVENTS_POLLED;
  sensor_laser_data_cb_t cb_event_data;
  int err;
  int count;

  /* set the thread status to ON */
  sensor_l_obj->thread_status = SENSOR_LASER_THREAD_ON;

  CDBG_LOW("Inside thread handler. Start polling!");
  do {
    count = device->poll(device, &event, num_of_events);
    if (count < 0) {
      CDBG_ERROR("Failure polling the sensor! Error: %s", strerror(-count));
      break;
    }

    CDBG_LOW("Event:\n distance: %f cm\n tv_sec: %f\n tv_usec: %f\n "
      "distance: %fmm\n error_code: %f\n signalRate_mcps: %f\n "
      "rtnAmbRate(kcps): %f\n rtnConvTime: %f\n dmax_sq: %f\n",
      event.data[0], event.data[1], event.data[2], event.data[3],
      event.data[4], event.data[5], event.data[6],event.data[7],
      event.data[8]);

    /* Update event cb data and send callback */
    cb_event_data.range_cm = event.data[0];
    cb_event_data.tv_sec = event.data[1];
    cb_event_data.tv_usec = event.data[2];
    cb_event_data.range_mm = event.data[3];
    cb_event_data.error_status = event.data[4];
    cb_event_data.signal_rate = event.data[5];
    cb_event_data.rtn_amb_rate = event.data[6];
    cb_event_data.rtn_conv_time = event.data[7];
    cb_event_data.dmax_sq = event.data[8];

    sensor_l_obj->data_cb(sensor_l_obj->cb_hndl, cb_event_data);
  } while (sensor_l_obj->thread_status == SENSOR_LASER_THREAD_ON);

  return NULL;
}



/** sensor_laser_init
 *
 *  @handle: handle
 *  @cb_handle: callback handle to be passed back
 *  @cb: data callback
 *
 *  Initialization of sensor laser module.
 *
 *    Return - sensor_laser_ret_type
 **/
sensor_laser_ret_type sensor_laser_init(
  void **handle,
  void *cb_hndl,
  sensor_laser_data_cb cb)
{
  sensor_laser_obj_t  *sensor_l_obj = NULL;
  struct sensors_poll_device_t *device;
  struct sensors_module_t *module;
  void *sensor_lib = NULL;
  struct sensor_t const *list;
  int sensor_count = 0;
  int err = 0;
  int i = 0;

  if (NULL == cb) {
    CDBG_ERROR("Callback function is NULL!!!");
    return SENSOR_LASER_RET_BAD_PARAMS;
  }

  sensor_l_obj = calloc(1, sizeof(sensor_laser_obj_t));
  if (NULL == sensor_l_obj) {
    CDBG_ERROR("Failure allocating memory for internal object!");
    return SENSOR_LASER_RET_NO_MEMMORY;
  }

  /* Save the input cb parameters */
  sensor_l_obj->data_cb = cb;
  sensor_l_obj->cb_hndl = cb_hndl;


  /* Open the sensor library */
  CDBG_HIGH("Open sensor module library: %s!", HAL_LIB_NAME_DEF);
  sensor_lib = dlopen(HAL_LIB_NAME_DEF, RTLD_LAZY);
  if (NULL == sensor_lib) {
    /* Try opening 32 bit library if present */
    CDBG_HIGH("Failed opening %s. Try opening %s",
      HAL_LIB_NAME_DEF, HAL_LIB_NAME_32);
    sensor_lib = dlopen(HAL_LIB_NAME_32, RTLD_LAZY);
    if (NULL == sensor_lib) {
      CDBG_ERROR("Failure opening sensor library! Error: %s", dlerror());
      free(sensor_l_obj);
      sensor_l_obj = NULL;
      return SENSOR_LASER_RET_HW_NOT_FOUND;
    }
  }
  sensor_l_obj->lib_handle = sensor_lib;


  /* Get sensor module handle */
  CDBG_LOW("Get sensor module handle!");
  dlerror();
  module = dlsym(sensor_lib, HAL_MODULE_INFO_SYM_AS_STR);
  if ((NULL != dlerror())|| (NULL == module)) {
    CDBG_ERROR("Failure getting sensor module handle!");
    dlclose(sensor_lib);
    free(sensor_l_obj);
    sensor_l_obj = NULL;
    return SENSOR_LASER_RET_SYM_NOT_FOUND;
  }

  /* Update internal parameter */
  sensor_l_obj->module = module;

  /* For debugging print id and name of the hw module */
  CDBG_ERROR("HW module: id: %s name: %s",
    sensor_l_obj->module->common.id,
    sensor_l_obj->module->common.name);

  /* Found the hardware. Call open sensor for the module */
  err = module->common.methods->open(&module->common,
    SENSORS_HARDWARE_POLL, (hw_device_t **)&device);
  if (0 != err) {
    CDBG_ERROR("Failure opening sensor module! Error: %s!",
      strerror(-err));
    dlclose(sensor_lib);
    free(sensor_l_obj);
    sensor_l_obj = NULL;
    return SENSOR_LASER_RET_OPEN_FAILURE;
  }
  sensor_l_obj->device = device;



  /* Get list of sensors supported */
  sensor_count = module->get_sensors_list(module, &list);
  if (0 == sensor_count) {
    CDBG_ERROR("0 sensor supported");
    device->common.close((hw_device_t *)&device->common);
    dlclose(sensor_lib);
    free(sensor_l_obj);
    sensor_l_obj = NULL;
    return SENSOR_LASER_RET_HW_NOT_FOUND;
  }
  sensor_l_obj->sensor_list = list;

  CDBG_LOW("Number of sensors supported: %d", sensor_count);

  /* There should only be one sensor but let's iterate through the count */
  for (i = 0; i < sensor_count; i++) {
    CDBG_LOW("name: %s\nvendor: %s\nhandle: %d\ntype: %d\nmaxRange: %f\n"
      "Resolution: %f\nPower: %f mA\nminDelay: %d\n",
      list[i].name, list[i].vendor, list[i].handle, list[i].type,
      list[i].maxRange, list[i].resolution, list[i].power,
      list[i].minDelay);

    /* Get the sensor list handle for laser sensor*/
    if (list[i].type == SENSOR_TYPE_TIME_OF_FLIGHT) {
      sensor_l_obj->sensor_handle = list[i].handle;
    }
  }


  /* Now activate the laser sensor */
  CDBG_LOW("Activating the laser sensor!");
  err = device->activate(device, sensor_l_obj->sensor_handle, 1);
  if (0 != err) {
    CDBG_LOW("Failure Activating the sensor! Error: %s", strerror(-err));
    device->common.close((hw_device_t *)&device->common);
    dlclose(sensor_lib);
    free(sensor_l_obj);
    sensor_l_obj = NULL;
    return SENSOR_LASER_RET_FAILURE_ENABLE;
  }

  /* Initialize and create sensor laser thread to poll the data */
  pthread_create(&sensor_l_obj->thread_id, NULL, sensor_laser_thread_handler,
    (void *)sensor_l_obj);


  *handle = (void *)sensor_l_obj;
  return SENSOR_LASER_RET_SUCCESS;
}


/** sensor_laser_deinit
 *
 *  @handle: handle
 *
 *  Deinitialize sensor laser module.
 *
 *    Return - sensor_laser_ret_type
 **/
sensor_laser_ret_type sensor_laser_deinit(void *handle)
{
  sensor_laser_obj_t *sensor_l_obj = (sensor_laser_obj_t *)handle;

  if (NULL == sensor_l_obj) {
    CDBG_ERROR("Invalid object!");
    return SENSOR_LASER_RET_BAD_PARAMS;
  }

  /*Stop the thread */
  CDBG_LOW("Updating SENSOR thread status to OFF");
  sensor_l_obj->thread_status = SENSOR_LASER_THREAD_OFF;

  CDBG_LOW("Waiting for thread to join!");
  pthread_join(sensor_l_obj->thread_id, NULL);


  if (NULL != sensor_l_obj->device) {
    /* Deactivate the sensor */
    sensor_l_obj->device->activate(sensor_l_obj->device, sensor_l_obj->sensor_handle, 0);

    /* Close the sensor */
    sensor_l_obj->device->common.close((hw_device_t *)&sensor_l_obj->device->common);
  }


  if (NULL != sensor_l_obj->lib_handle) {
    /* Unload the dynamic library loaded */
    dlclose(sensor_l_obj->lib_handle);
  }

  /* Free all the allocated memories */
  free(sensor_l_obj);

  return SENSOR_LASER_RET_SUCCESS;
}








