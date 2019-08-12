/*===========================================================================

                  TM - XSPI Module

DESCRIPTION
  This file contains declarations for TM XSPI module.


  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/inc/tm_xspi.h#1 $
$Author: mplcsds1 $
$Created$
$Modified$
===========================================================================*/

#ifndef TM_XSPI_H
#define TM_XSPI_H

#include "comdef.h"   /* Definition for basic types and macros */
#include "customer.h"

#define TM_XSPI_NV_ENABLED_FLAG  0x2

#include "pdapi.h"
#include "tm_data.h"
#include "tm_common.h"
#include "tm_api.h"

#define TM_XSPI_MAX_DATA_MOTION_CONFIDENCE  (100) /* capped at 100 percent confidence */
#define TM_XSPI_CONVERT_SECONDS_TO_MSECS(a) (((uint32)(a)) * (uint32) 1000 )
#define TM_XSPI_DEFAULT_INJECTION_TIMEOUT   (TM_XSPI_CONVERT_SECONDS_TO_MSECS(0)) /* in msec = 0 seconds. 0 = wait forever */
#define TM_XSPI_DEFAULT_STREAM_TIMEOUT      (TM_XSPI_CONVERT_SECONDS_TO_MSECS(1))  /* in msec = 1 second */


typedef enum {
   TM_XSPI_TIMER_INJECTION = TM_XSPI_ID_RANGE,
   TM_XSPI_TIMER_STREAM,

   /* add new timers above this line */
   TM_XSPI_TIMER_MAX
} tm_xspi_timer_types;

/* collect all module data structs in one location */
typedef struct {
   boolean b_xspiNvEnabled; /**< is this feature enabled in NV */
   boolean b_Enabled;    /**< is this feature enabled.
                         see PDSM_PA_EXTERNAL_SPI and tm_xspi_enable().
                         if not enabled, don't raise
                         PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION */

   boolean b_TimerInjectionEnabled; /**< are we ok to inject? this is
                               enabled while the engine is on*/

   uint32  q_InjectionWaitPeriod; /**< how long to wait for injection
                                  before assuming "unknown" motion */

   uint32  q_StreamPeriod; /**< period between injections of
                           cached data to fake a stream for MGP */

   /*timers*/
   os_TimerMsgType* p_timerInjection; /**< how long to wait before
                                      declaring motion as "unknown" */
   os_TimerMsgType* p_timerStream;  /* how long to wait before injecting
                                    cached z_sensorData */


   mgp_ReportSensorDataParm z_sensorData; /**< cached sensor data to be
                                          injected to MGP as a stream */
} tm_xspi_module_data_type;

typedef enum {
   TM_XSPI_STREAM_STATUS_DISABLE = 0,
   TM_XSPI_STREAM_STATUS_ENABLE,
   TM_XSPI_STREAM_STATUS_MAX
} tm_xspi_stream_status_type;

typedef struct {
   uint32 q_stream_status; /* see tm_xspi_stream_status_type */
} tm_xspi_request_type;

/**
@brief initialize xspi module data and read appropriate
       variables from NV RAM
*/
void tm_xspi_init_and_read_nv(void);

/**
@brief initializes given module data struct
@param p_config_data pointer to xsp module data to be initialized
@return FALSE on failure (e..g if p_config_data == NULL), TRUE otherwise
*/
boolean tm_xspi_init_module_data(tm_xspi_module_data_type* p_config_data);

/**
@breif handles injection of xspi data received via pdapi
@param p_cmd pointer to data received from pdapi
@return void
*/
void tm_xspi_handle_status_injection(pdsm_xspi_cmd_s_type *p_cmd);

/**
@brief enables the xspi feature. When enabled PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION
       is raised when MGP requests external SPI data
@param
@return
*/
void tm_xspi_enable(void);

/**
@brief disables the xspi feature. When disabled PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION
       is NOT raised even if MGP requests external SPI data
@param
@return
*/
void tm_xspi_disable(void);

/**
@brief translates injection requests from MGP to PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION events
@param p_reqData pointer to request data denote requested stream mode (on / off)
@return
*/
void tm_xspi_handle_injection_request(tm_xspi_request_type* p_reqData);

/**
@brief handles expiration of various timers
@param timer_data pointer to expired timer information
@return
*/
void tm_xspi_handle_timer_expiry(const os_TimerExpiryType *timer_data);

#endif /* TM_XSPI_H */


