#ifndef SNS_SCM_PRIV_H
#define SNS_SCM_PRIV_H

/*============================================================================
  @file sns_scm_priv.h

  Sensors calibration manager header

  This header file contains the private interface of Sensors Calibration Manager

Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "stdbool.h"
#include "sns_common.h"
#include "sns_debug_api.h"
#include "sns_em.h"
#include "sns_smgr_api_v01.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_sam_amd_v01.h"
#include "sns_queue.h"
#include <qmi_client.h>

/*---------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
#define SNS_SCM_DEBUG

#if defined(SNS_DSPS_BUILD) || (defined(SNS_PCSIM) && !defined(ENABLE_APPS_PLAYBACK))
#define SNS_SCM_MODULE           SNS_MODULE_DSPS_SCM
#define SNS_SCM_MODULE_PRIORITY  SNS_MODULE_PRI_DSPS_SCM
#define SNS_SCM_DBG_MOD          SNS_DBG_MOD_DSPS_SCM
#define SNS_SCM_MODULE_STK_SIZE  SNS_MODULE_STK_SIZE_DSPS_SCM
#else
#define SNS_SCM_MODULE           SNS_MODULE_APPS_SCM
#define SNS_SCM_MODULE_PRIORITY  SNS_MODULE_PRI_APPS_SCM
#define SNS_SCM_DBG_MOD          SNS_DBG_MOD_APPS_SCM
#define SNS_SCM_MODULE_STK_SIZE  SNS_MODULE_STK_SIZE_APPS_SCM
#endif

#ifdef SNS_SCM_DEBUG
#define SNS_SCM_DEBUG0(level,msg)          SNS_SCM_PRINTF0(level,msg)
#define SNS_SCM_DEBUG1(level,msg,p1)       SNS_SCM_PRINTF1(level,msg,p1)
#define SNS_SCM_DEBUG2(level,msg,p1,p2)    SNS_SCM_PRINTF2(level,msg,p1,p2)
#define SNS_SCM_DEBUG3(level,msg,p1,p2,p3) SNS_SCM_PRINTF3(level,msg,p1,p2,p3)

#if defined QDSP6
#ifndef DBG_MEDIUM_PRIO
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#endif
#define SNS_SCM_PRINTF0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO,msg)
#define SNS_SCM_PRINTF1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1)
#define SNS_SCM_PRINTF2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1,p2)
#define SNS_SCM_PRINTF3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1,p2,p3)
#elif defined(ADSP_STANDALONE) || defined(SNS_QDSP_SIM)
#define SNS_SCM_PRINTF0(level,msg)          qurt_printf("\n"); qurt_printf(msg)
#define SNS_SCM_PRINTF1(level,msg,p1)       qurt_printf("\n"); qurt_printf(msg,p1)
#define SNS_SCM_PRINTF2(level,msg,p1,p2)    qurt_printf("\n"); qurt_printf(msg,p1,p2)
#define SNS_SCM_PRINTF3(level,msg,p1,p2,p3) qurt_printf("\n"); qurt_printf(msg,p1,p2,p3)
#else
#define SNS_SCM_PRINTF0(level,msg)          SNS_PRINTF_STRING_##level##_0(SNS_SCM_DBG_MOD,msg)
#define SNS_SCM_PRINTF1(level,msg,p1)       SNS_PRINTF_STRING_##level##_1(SNS_SCM_DBG_MOD,msg,p1)
#define SNS_SCM_PRINTF2(level,msg,p1,p2)    SNS_PRINTF_STRING_##level##_2(SNS_SCM_DBG_MOD,msg,p1,p2)
#define SNS_SCM_PRINTF3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_##level##_3(SNS_SCM_DBG_MOD,msg,p1,p2,p3)
#endif

#else
#define SNS_SCM_DEBUG0(level,msg)
#define SNS_SCM_DEBUG1(level,msg,p1)
#define SNS_SCM_DEBUG2(level,msg,p1,p2)
#define SNS_SCM_DEBUG3(level,msg,p1,p2,p3)

#define SNS_SCM_PRINTF0(level,msg)
#define SNS_SCM_PRINTF1(level,msg,p1)
#define SNS_SCM_PRINTF2(level,msg,p1,p2)
#define SNS_SCM_PRINTF3(level,msg,p1,p2,p3)
#endif

/* SCM - TIMER signal */
#define SNS_SCM_REPORT_TIMER_SIG       (0x1 << 0)

/* SCM - SAM & SMGR signal */
#define SCM_SAM_SMGR_MSG_SIG           (0x1 << 1)

/* SCM - REG signal */
#define  SCM_REG_MSG_SIG               (0x1 << 2)

/* SCM - SAM signals */
#define  SCM_SAM_QCSI_WAIT_SIG         (0x1 << 3)

/* SCM - SMGR signals */
#define  SCM_SMGR_QCSI_WAIT_SIG        (0x1 << 4)

/* SCM - REG signals */
#define  SCM_REG_QCSI_WAIT_SIG         (0x1 << 5)

#define  SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG (0x1 << 9)

/* SCM - SMGR signals */
#define  SCM_SMGR_INT_QCSI_WAIT_SIG    (0x1 << 6)

/* SCM SIGNALS for SAM & SMGR */
#define  SNS_SCM_SAM_SMGR_FLAGS (SCM_SAM_SMGR_MSG_SIG + SNS_SCM_REPORT_TIMER_SIG + SCM_REG_MSG_SIG + SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG)

/* SCM SIGNALS for REG */
#define  SNS_SCM_REG_FLAGS (SNS_SCM_REPORT_TIMER_SIG + SCM_REG_MSG_SIG)

#define SNS_SCM_MAX_ALGO_INSTS               5
#define SNS_SCM_MAX_DATA_REQS                10
#define SNS_SCM_MAX_ALGO_INSTS_PER_DATA_REQ  5
#define SNS_SCM_MAX_SENSORS_PER_DATA_REQ     3
#define SNS_SCM_MAX_SNS_MON                  5
#define SNS_SCM_MAX_SNS_MON_PER_ALGO         2

//All algorithm services must support the following messages
//with specified message ids
#define SNS_SCM_ALGO_ENABLE_REQ  0x0
#define SNS_SCM_ALGO_DISABLE_REQ 0x1

#define SNS_SCM_INVALID_ID       0xFF
#define SNS_SCM_INVALID_SAMPLE_RATE     0xFFFF  /* Not supported by any sensor */
#define SNS_SCM_DEFAULT_SAMPLE_QUALITY  0x0000  /* bitmap, all flags disabled */

#define SNS_SCM_MAX_REG_ITEMS_PER_ALGO (2)

//Sensor Status Hysteresis Period
#define SNS_SCM_SENSOR_STATUS_HYST_PERIOD_USEC   (50000)

/* Does the algo depend on *AMD* stationary information */
typedef enum
{
   MOTION_DETECT_FLAG_ACTIVE_IN_REST,  /* Algo is active when device is stationary.  */
   MOTION_DETECT_FLAG_ACTIVE_IN_MOTION, /* Algo is active when device is moving.  */
  MOTION_DETECT_FLAG_DONT_CARE         /* Algo does not need stationary information */
}sns_scm_motion_flags_e;

/*---------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/
/* SCM message types */
typedef enum {
  SCM_SMGR_IND_MSG,
  SCM_SMGR_INT_IND_MSG,
  SCM_SAM_IND_MSG,
  SCM_SMGR_RESP_MSG,
  SCM_SMGR_INT_RESP_MSG,
  SCM_SAM_RESP_MSG,
  SCM_REG_RESP_MSG
} sns_scm_msg_type_e;

/*=======================================================================*/
//generic structure for algorithm memory management
//(config, state, input, output)
typedef struct {
   uint32_t memSize;
   void *memPtr;
} sns_scm_algo_mem_s;

//Algorithm API
typedef struct {
   //Algorithm state memory requirement query API
   /*=========================================================================
     FUNCTION:  sns_scm_algo_mem_req
     =======================================================================*/
   /*!
       @brief Query interface to get memory requirement of algorithm state
       based on specified configuration

       @param[i] configPtr: Pointer to algorithm configuration

       @return Size of memory required for algorithm state
   */
   /*=======================================================================*/
   int32_t (*sns_scm_algo_mem_req)(void *configPtr);

   /*=========================================================================
     FUNCTION:  sns_scm_algo_reset
     =======================================================================*/
   /*!
       @brief Reset/Initialize the state of the algorithm instance

       @param[i] configPtr: Pointer to algorithm configuration
       @param[i] statePtr: Pointer to algorithm state

       @return Pointer to algorithm state if successful
       NULL if error
   */
   /*=======================================================================*/
   void* (*sns_scm_algo_reset)(
      void *configPtr,
      void *statePtr);

   /*=========================================================================
     FUNCTION:  sns_scm_algo_update
     =======================================================================*/
   /*!
       @brief Execute the algorithm to generate output using specified input

       @param[i] statePtr: Pointer to algorithm state
       @param[i] inputPtr: Pointer to input data
       @param[o] outputPtr: Pointer to output data

       @return Error code
   */
   /*=======================================================================*/
   void (*sns_scm_algo_update)(
      void *statePtr,
      void *inputPtr,
      void *outputPtr);
} sns_scm_algo_api_s;

//registry item type
typedef enum
{
  SNS_SCM_REG_ITEM_TYPE_NONE,
  SNS_SCM_REG_ITEM_TYPE_SINGLE,
  SNS_SCM_REG_ITEM_TYPE_GROUP
} sns_scm_reg_item_type_e;

//algorithm specific information
typedef struct {
   sns_scm_algo_api_s algoApi;         /*algorithm API*/
   sns_scm_algo_mem_s defConfigData;   /*default algorithm configuration*/
   sns_scm_reg_item_type_e regItemType[SNS_SCM_MAX_REG_ITEMS_PER_ALGO];
                                       /*registry item type*/
   sns_em_timer_obj_t timer;           /*timer for periodic execution*/
   uint32_t defInputDataSize;          /*default algorithm input size*/
   uint32_t defOutputDataSize;         /*default algorithm output size*/
   uint32_t regItemId[SNS_SCM_MAX_REG_ITEMS_PER_ALGO];
                                       /*registry item id */
   uint32_t period;                    /*duration of periodic execution*/
   uint8_t serviceId;                  /*algorithm service id*/
   uint8_t sensorIndex;                /*sensor under calibration*/
   uint8_t monitorSensorId[SNS_SCM_MAX_SNS_MON_PER_ALGO];
                                       /*sensor types to be monitored*/
   uint8_t monitorSensorCount;         /*number of sensor types to be monitored*/
   uint8_t monitorSensorStatus;        /*overall status*/
   bool enableAlgo;                    /*toggle algorithm usage*/
   bool timeout;                       /*timer timeout indication*/
   int32_t defSensorReportRate;        /*default sensor report rate*/
   uint8_t motionTrigFlags;       /* Behaviour on motion detection*/
   sns_scm_algo_mem_s persistAutoCalParam; /*persistent estimated auto cal param*/
} sns_scm_algo_s;

//algorithm instance specific information
typedef struct {
   sns_scm_algo_mem_s configData; /*pointer to algorithm config*/
   sns_scm_algo_mem_s stateData;  /*pointer to algorithm state*/
   sns_scm_algo_mem_s inputData;  /*pointer to algorithm input data*/
   sns_scm_algo_mem_s outputData; /*pointer to algorithm output data*/
   uint8_t algoIndex;             /*index to algorithm in database*/
} sns_scm_algo_inst_s;

//sensor information
typedef struct {
   uint8_t sensorId;                /*sensor id*/
   uint8_t dataType;                /*sensor data type*/
   uint8_t status;                  /*sensor status*/
   uint8_t num_clients;
   uint32_t auto_cal_reg_id;        /*auto cal registry id*/
   bool auto_cal_bias_params_valid; /* validity of auto cal bias params */
   int32_t auto_cal_bias_params[3]; /*auto cal bias parameters*/
   bool auto_cal_scale_params_valid; /* validity of auto cal scale params */
   int32_t auto_cal_scale_params[3];/*auto cal scale parameters*/
   int32_t auto_cal_accuracy;       /*auto cal accuracy*/
   sns_em_timer_obj_t timer;           /*sensor status hysteresis timer*/
   bool timeout;                       /*timer timeout indication*/
   uint8_t monitorAlgoIndex;           /*algo index used*/
} sns_scm_sensor_s;

//sensor information
typedef struct {
   uint8_t sensorId;             /*sensor id*/
   uint8_t dataType;             /*sensor data type*/
   uint16_t sampleRate;          /*sample rate*/
   uint16_t sampleQual;          /*sample quality*/
   uint8_t decimationType;      /* Decimation filter type */
} sns_scm_sensor_req_s;

//sensor data request
typedef struct {
   uint32_t reportRate;          /*sensor data report rate*/
   uint8_t sensorCount;          /*sensor count*/
   uint8_t algoInstCount;        /*algorithm instance count*/
   sns_scm_sensor_req_s sensorDbase[SNS_SCM_MAX_SENSORS_PER_DATA_REQ];
                                 /*sensors whose data is requested*/
   uint8_t algoInstDbase[SNS_SCM_MAX_ALGO_INSTS_PER_DATA_REQ];
                                 /*algorithm instances requesting data*/
} sns_scm_data_req_s;

//calibration algorithms
typedef enum
{
   SNS_SCM_GYRO_CAL_SVC = 0,  /* Gyro Calibration      */
   SNS_SCM_ACCEL_CAL_SVC = 1, /* Accel calibration     */
   SNS_SCM_ACCEL_CSM_SVC = 2, /* Accel cal set manager */
   SNS_SCM_MAG_CAL_SVC   = 3, /* Mag Cal Algo          */
   SNS_SCM_NUM_ALGO_SVCS
} sns_scm_algo_svc_e;

typedef struct scm_q_item_s
{
  sns_q_link_s       q_link;
  unsigned int       msg_id;
  sns_scm_msg_type_e scm_msg_type;
  void               *body_ptr;
} scm_q_item_s;

typedef struct scm_motion_state_s
{
  uint8_t                    qmd_inst_id;
  bool                       qmd_inst_id_valid;
  sns_sam_motion_state_e_v01 qmd_state;
  bool                       qmd_enabled;
} scm_motion_state_s;

typedef struct
{
  qmi_client_os_params        os_params;
  qmi_client_type             user_handle;
  qmi_client_type             notifier_handle;
  qmi_client_ind_cb           ind_cb;
  qmi_idl_service_object_type svc_obj;
  uint8_t                     svc_num;
} scm_qmi_client_if_s;


/*---------------------------------------------------------------------------
* Function Declarations
* -------------------------------------------------------------------------*/
/*=========================================================================
  FUNCTION:  sns_scm_qmi_client_send_msg_async
  =========================================================================*/
/*!
  @brief    Wrapper function to qmi_client_send_msg_async to prevent memory
            leaks

  @param[i]   user_handle:         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id:              Message ID
  @param[i]   req_c_struct:        Pointer to the request parameters
  @param[i]   req_c_struct_len:    Length of the request buffer
  @param[i]   resp_c_struct:       Pointer to the response parameters
  @param[i]   resp_c_struct_len:   Length of the response buffer
  @param[i]   resp_cb:             Callback function to handle response parameters
  @param[i]   resp_cb_data:        Callback user data
  @param[i]   txn_handle:          Handle used to identify the transaction

  @return   QMI error type
*/
/*=======================================================================*/
qmi_client_error_type sns_scm_qmi_client_send_msg_async (
   qmi_client_type                 user_handle,
   unsigned int                    msg_id,
   void                            *req_c_struct,
   unsigned int                    req_c_struct_len,
   void                            *resp_c_struct,
   unsigned int                    resp_c_struct_len,
   qmi_client_recv_msg_async_cb    resp_cb,
   void                            *resp_cb_data,
   qmi_txn_handle                  *txn_handle );

/*=========================================================================
  FUNCTION:  sns_scm_reg_init_timer
  =========================================================================*/
/*!
  @brief Register timer for for SCM initialization

  @param[i] sigEventFlags: Event signal flags

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_reg_init_timer(
   OS_FLAG_GRP *sigEventFlags);

/*=========================================================================
  FUNCTION:  sns_scm_dereg_init_timer
  =========================================================================*/
/*!
  @brief Deregister timer for SAM initialization

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_dereg_init_timer(void);

/*=========================================================================
  FUNCTION:  sns_scm_process_init_events
  =========================================================================*/
/*!
  @brief Process initialization events

  @param[i] numRegQuery: Number of queries made to registry
  @param[i] snsDbaseHandle: Handle to the SCM internal sensor database

  @return None
*/
/*=======================================================================*/
void sns_scm_process_init_events(
   uint8_t numRegQuery,
   sns_scm_sensor_s* snsDbaseHandle);

/*=========================================================================
  FUNCTION:  sns_scm_reg_algo
  =========================================================================*/
/*!
  @brief Register specified algorithm

  @param[i] algoPtr: pointer to algorithm

  @return None
*/
/*=======================================================================*/
sns_err_code_e sns_scm_reg_algo(
   sns_scm_algo_s* algoPtr);

/*=========================================================================
  FUNCTION:  sns_scm_reg_sensor_status
  =========================================================================*/
/*!
  @brief Send a request to sensors manager for sensor status report

  @param[i] sensorId: sensor id

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_reg_sensor_status(
   uint8_t sensorId);

/*=========================================================================
  FUNCTION:  sns_scm_get_algo_handle
  =========================================================================*/
/*!
  @brief Get the handle to the specified algorithm in the algorithm database

  @param[i] algoSvcId: algorithm service id

  @return handle to the specified algorithm if found,
          NULL otherwise
*/
/*=======================================================================*/
sns_scm_algo_s* sns_scm_get_algo_handle(
   uint8_t algoSvcId);

/*=========================================================================
  FUNCTION:  sns_scm_get_sensor_index
  =========================================================================*/
/*!
  @brief Get the index into the sensor status database for specified sensor

  @param[i] sensorId: algorithm service id

  @return index for the specified sensor in database if found,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
uint8_t sns_scm_get_sensor_index(
   uint8_t sensorId);

/*=========================================================================
  FUNCTION:  sns_scm_req_reg_data
  =========================================================================*/
/*!
  @brief Request registry data

  @param[i] regItemType: Registry item type (single / group)
  @param[i] regItemId: Registry item identifier

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_req_reg_data(
   sns_scm_reg_item_type_e regItemType,
   uint32_t regItemId);

/*=========================================================================
  FUNCTION:  sns_scm_store_gyro_auto_cal_params_registry
  =========================================================================*/
/*!
  @brief Store gyro auto calibration parameters to registry

  @param[i] gyroCalOutput: pointer to gyro calibration algo output

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_gyro_auto_cal_params_registry(
   void* gyroCalOutput);

/*=========================================================================
  FUNCTION:  sns_scm_smgr_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR response callback

  @detail
  Handles the responses from SMGR to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_resp_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err);

/*=========================================================================
  FUNCTION:  sns_scm_smgr_int_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR Internal response callback

  @detail
  Handles the responses from SMGR Internal to SCM.

  @param[i]   user_handle   Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id        Message ID
  @param[i]   resp_buf      Pointer to the response buffer
  @param[i]   resp_buf_len  Length of the response buffer
  @param[i]   user_cb_data  Pointer to the User-data
  @param[i]   transp_err    Error code

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_int_resp_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err);

/*=========================================================================
  FUNCTION:  sns_scm_sam_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SAM response callback

  @detail
  Handles the responses from SAM to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

    @return None
*/
/*=======================================================================*/
void sns_scm_sam_resp_cb (
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err);

/*=========================================================================
  FUNCTION:  sns_scm_smgr_ind_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR indication callback

  @detail
  Handles the indications from SMGR to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_ind_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *ind_buf,
   unsigned int ind_buf_len,
   void *user_cb_data);

/*=========================================================================
  FUNCTION:  sns_scm_sam_ind_cb
  =========================================================================*/
/*!
  @brief SCM - SAM indication callback

  @detail
  Handles the indications from SAM to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_sam_ind_cb(
  qmi_client_type user_handle,
  unsigned int msg_id,
  void *ind_buf,
  unsigned int ind_buf_len,
  void *user_cb_data);

/*=========================================================================
  FUNCTION:  sns_scm_reg_resp_cb
  =========================================================================*/
/*!
  @brief SCM - REG response callback

  @detail
  Handles the responses from REG to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

  @return None
*/
/*=======================================================================*/
void sns_scm_reg_resp_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err);

/*=========================================================================
  FUNCTION:  sns_scm_q_get
  =========================================================================*/
/*!
  @brief Get the SCM data from QUEUE

  @detail
  Gets the data from QUEUE

  @param[i] qflag: queue flag that is to be picked

  @return Queue data pointer
          NULL otherwise
*/
/*=======================================================================*/
void* sns_scm_q_get (OS_FLAGS qflag);

/*=========================================================================
  FUNCTION:  sns_scm_q_put
  =========================================================================*/
/*!
  @brief Put the SCM data in QUEUE

  @detail
  Puts the SCM adat in QUEUE

  @param[i]   msg_id                Message ID
  @param[i]   buffer                  Pointer to the message buffer
  @param[i]   scm_msg_type     SCM message type
  @param[i]   transp_err           Error code
  @param[i]   buf_len                Length of the message buffer
  @param[i]   decode_msg_len   Length of the message to be decoded

  @return None
*/
/*=======================================================================*/
void sns_scm_q_put(
   unsigned int msg_id,
   void *buffer,
   sns_scm_msg_type_e scm_msg_type,
   qmi_client_error_type transp_err,
   int32 buf_len,
   int32 decode_msg_len);

/*=========================================================================
  FUNCTION:  sns_scm_process_reg_msg
  =========================================================================*/
/*!
  @brief Process messages received from registry

  @return Number of messages processed
*/
/*=======================================================================*/
uint8_t sns_scm_process_reg_msg(void);

/*=========================================================================
  FUNCTION:  sns_scm_detect_smgr_buffering
  =========================================================================*/
/*!
  @brief  Detects support for Buffering in Sensor Manager.

  @return  true, if SMGR supports buffering; false, otherwise
*/
/*=======================================================================*/
bool sns_scm_detect_smgr_buffering(
   OS_FLAG_GRP *sigEventPtr);

/*=========================================================================
  FUNCTION:  sns_scm_check_sensor_report_rate
  =========================================================================*/
/*!
  @brief  Checks registry version to see if default sensor report rates are
          available.

  @return  true, if expected to be available; false, otherwise
*/
/*=======================================================================*/
bool sns_scm_check_sensor_report_rate(
   OS_FLAG_GRP *sigEventPtr);

/*=========================================================================
  FUNCTION:  sns_scm_is_sensor_report_rate_available
  =========================================================================*/
/*!
  @brief    Detects sensor report rate availability in Registry

  @return   'true' if sensor report rate is expected to be available;
            'false' otherwise.
*/
/*=======================================================================*/
bool sns_scm_is_sensor_report_rate_available(void);

/*=========================================================================
  FUNCTION:  sns_scm_store_qmag_auto_cal_params_registry
  =========================================================================*/
/*!
  @brief Store qmag auto calibration parameters to registry

  @param[i] qmagCalOutput: pointer to qmag calibration algo output

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_qmag_auto_cal_params_registry(
   void* qmagCalOutput);

/*=========================================================================
  FUNCTION:  sns_scm_store_qmag_auto_cal_state_algodbase
  =========================================================================*/
/*!
  @brief Store qmag cal state to SCM's algo database

  @param[i] qmagCalState: pointer to qmag calibration algo state
  @param[i] qmagCalAlgoDBConfig: pointer to qmag cal algo config from sns_scm_algo_db

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_qmag_auto_cal_state_algodbase(
   void* qmagCalState,
   void* qmagCalAlgoDBConfig);

/*=========================================================================
  FUNCTION:  sns_scm_store_qmag_cal_state_registry
  =========================================================================*/
/*!
  @brief Store qmag cal state from algo database in registry

  @param[i] qmagCalAlgoDBConfig: pointer to qmag cal algo config from sns_scm_algo_db

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_qmag_auto_cal_state_registry(
   void* qmagCalAlgoDBConfig);

//#define SNS_SCM_TEST_ENABLE
#ifdef SNS_SCM_TEST_ENABLE
void sns_scm_ext_test (void);
#endif //SNS_SCM_TEST_ENABLE

#endif /*#ifndef SNS_SCM_PRIV_H*/
