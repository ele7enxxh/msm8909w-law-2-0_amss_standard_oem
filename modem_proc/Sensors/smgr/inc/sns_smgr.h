#ifndef SNS_SMGR_H
#define SNS_SMGR_H

/*============================================================================

  @file sns_smgr.h

  @brief
  This file contains definition for Sensor Manager

******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/inc/sns_smgr.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-11  bd    Move reserved fifo size to uimage
  2016-03-08  pn    Removed ENABLE_MD and DISABLE_MD signal bits
  2015-10-20  bd    Add reserved fifo size support
  2015-09-24  pn    Removed SNS_SMGR_DEPOT_COPY_SIG
  2015-09-22  pn    Added SNS_SMGR_HEARTBEAT_SIG
  2015-09-10  pn    Added SMGR_MSG_5() and SNS_SMGR_PRINTF5()
  2015-06-29  agk   Added power rail per sensor
  2015-01-22  BD    Changed the way of putting functions in uImage to use sections
  2014-10-13  jc    Increased the max number of reports to 40
  2014-09-25  pn    Added support for bigImage depots
  2014-09-08  jms   Added SPI support
  2014-09-07  vy    Added a signal for voting mips/bw
  2014-08-08  pn    Added SMGR_ANY_ALLOC_STRUCT()
  2014-07-31  pn    Removed PM task interface signals
  2014-07-29  pn    Added message macros for ISLAND build; reverted 07/27 change
  2014-07-27  vy    Message logging conditional as not supported in uImage
  2014-07-21  pn    Enabled message logging unconditionally
  2014-05-13  vy    Refactored for uImage
  2014-05-07  pn    Added support for MD deregistration 
  2014-04-24  pn    Initial version
 
============================================================================*/
#include "sns_smgr_api_v01.h"
#include "sns_smgr_restricted_api_v01.h"
#include "sns_common.h"
#include "sns_queue.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_smgr_sensor_config.h"
#include "sns_debug_api.h"

/*===========================================================================

                   SMGR MACRO

===========================================================================*/
/*---------------------------------------------------------------------------
                   DBG PRINTF MACROS
---------------------------------------------------------------------------*/
#ifdef SNS_PCSIM
  #define SMGR_LOG printf
#else
  #define SMGR_LOG(_Format, ...)
#endif

#define SNS_MSG_MACRO_MSG
#ifdef SNS_MSG_MACRO_MSG
#if defined(VIRTIO_BRINGUP)
#define SMGR_MSG_0(xx_pri, xx_fmt)                            qurt_printf("\n"); qurt_printf(__FILENAME__ " - " xx_fmt)
#define SMGR_MSG_1(xx_pri, xx_fmt, xx_arg1)                   qurt_printf("\n"); qurt_printf(__FILENAME__ " - " xx_fmt, xx_arg1)
#define SMGR_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)          qurt_printf("\n"); qurt_printf(__FILENAME__ " - " xx_fmt, xx_arg1, xx_arg2)
#define SMGR_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3) qurt_printf("\n"); qurt_printf(__FILENAME__ " - " xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#define SMGR_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) qurt_printf("\n"); qurt_printf(__FILENAME__ " - " xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
#define SMGR_MSG_5(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)
#elif defined SNS_USES_ISLAND
#define SMGR_MSG_0(xx_pri, xx_fmt)                            UMSG(MSG_SSID_SNS, xx_pri, xx_fmt)
#define SMGR_MSG_1(xx_pri, xx_fmt, xx_arg1)                   UMSG_1(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1)
#define SMGR_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)          UMSG_2(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2)
#define SMGR_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3) UMSG_3(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#define SMGR_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) UMSG_4(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
#define SMGR_MSG_5(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) UMSG_5(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)
#elif defined QDSP6
#define SMGR_MSG_0(xx_pri, xx_fmt)                            MSG(MSG_SSID_SNS, xx_pri, xx_fmt)
#define SMGR_MSG_1(xx_pri, xx_fmt, xx_arg1)                   MSG_1(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1)
#define SMGR_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)          MSG_2(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2)
#define SMGR_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3) MSG_3(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#define SMGR_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) MSG_4(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
#define SMGR_MSG_5(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) MSG_5(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)
#else
#define SMGR_MSG_0(xx_pri, xx_fmt)                            SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING0)
#define SMGR_MSG_1(xx_pri, xx_fmt, xx_arg1)                   SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING1, xx_arg1)
#define SMGR_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)          SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING2, xx_arg1, xx_arg2)
#define SMGR_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3) SNS_PRINTF_STRING_ID_MEDIUM_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, xx_arg1, xx_arg2, xx_arg3)
#define SMGR_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
#define SMGR_MSG_5(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)
#endif //QDSP6
#else
#define SMGR_MSG_0(xx_pri, xx_fmt)
#define SMGR_MSG_1(xx_pri, xx_fmt, xx_arg1)
#define SMGR_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)
#define SMGR_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#define SMGR_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
#define SMGR_MSG_5(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)
#endif //SNS_MSG_MACRO_MSG

#define SNS_SMGR_PRINTF0(level,msg)                SMGR_MSG_0(DBG_##level##_PRIO,msg)
#define SNS_SMGR_PRINTF1(level,msg,p1)             SMGR_MSG_1(DBG_##level##_PRIO,msg,p1)
#define SNS_SMGR_PRINTF2(level,msg,p1,p2)          SMGR_MSG_2(DBG_##level##_PRIO,msg,p1,p2)
#define SNS_SMGR_PRINTF3(level,msg,p1,p2,p3)       SMGR_MSG_3(DBG_##level##_PRIO,msg,p1,p2,p3)
#define SNS_SMGR_PRINTF4(level,msg,p1,p2,p3,p4)    SMGR_MSG_4(DBG_##level##_PRIO,msg,p1,p2,p3,p4)
#define SNS_SMGR_PRINTF5(level,msg,p1,p2,p3,p4,p5) SMGR_MSG_5(DBG_##level##_PRIO,msg,p1,p2,p3,p4,p5)

/*---------------------------------------------------------------------------
                   BIT FLAGS MACROS
---------------------------------------------------------------------------*/
/** This macro's value is TRUE if the bit specified by b is set in flags f. */
#define SMGR_BIT_TEST(f,b)   (((f) & (b)) == (b))

/** This macro's value is TRUE if the bit specified by b is not set in flags f. */
#define SMGR_BIT_CLEAR_TEST(f, b)  (((f) & (b)) == 0)

/** This macro sets the bit specified by b in flags f. */
#define SMGR_BIT_SET(f,b)    ((f) |= (b))

/** This macro clears the bit specified by b in flags f. */
#define SMGR_BIT_CLEAR(f,b)  ((f) &= ~(b))

/*---------------------------------------------------------------------------
                   OTHER MACROS
---------------------------------------------------------------------------*/
#define SMGR_ALLOC_STRUCT(t) ((t*)SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(t)))
#define SMGR_U_ALLOC_STRUCT(t) ((t*)SNS_OS_U_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(t)))
#define SMGR_ANY_ALLOC_STRUCT(t) ((t*)SNS_OS_ANY_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(t)))

/* sns_q macros */
#define SMGR_QUE_HEADER_FROM_LINK(link_ptr) ((NULL==(link_ptr))? NULL:((link_ptr)->q_ptr))
#define SMGR_QUE_SELF_FROM_LINK(link_ptr)   ((NULL==(link_ptr))?NULL:(((sns_q_link_s*)(link_ptr))->self_ptr))
#define SMGR_NEXT_Q_ITEM(q,ip,l) ((ip)=SMGR_QUE_SELF_FROM_LINK(sns_q_next((q),&((ip)->l))))
#define SMGR_FOR_EACH_Q_ITEM(q,ip,l) for((ip)=sns_q_check(q); (ip)!=NULL; SMGR_NEXT_Q_ITEM(q,ip,l))

#ifndef VIRTIO_BRINGUP
#define SMGR_STATIC static
#else
#define SMGR_STATIC
#endif

#ifdef SNS_USES_ISLAND
#define SNS_SMGR_UIMAGE_CODE __attribute__((section (".text.SMGR")))
#define SNS_SMGR_UIMAGE_DATA __attribute__((section (".data.SMGR")))
#else
#define SNS_SMGR_UIMAGE_CODE
#define SNS_SMGR_UIMAGE_DATA
#endif /* USES_ISLAND */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define SNS_SMGR_MAX_REPORT_CNT             40

#define SMGR_SCHEDULE_GRP_TOGETHER_US       2000
#define SMGR_SCHEDULE_GRP_TOGETHER_TICK (sns_em_convert_usec_to_dspstick(SMGR_SCHEDULE_GRP_TOGETHER_US))

#define SNS_SMGR_SENSOR_READ_SIG            0x00000001
#define SNS_SMGR_DD_SERVICE_SIG             0x00000002
#define SNS_SMGR_SENSOR_STATE_SIG           0x00000004
#define SNS_SMGR_PWR_RAIL_SIG               0x00000020
#define SNS_SMGR_VOTE_MIPS_BW               0x00000040
#define SNS_SMGR_APP_WAKE_SIG               0x00000100
#define SNS_SMGR_APP_SLEEP_SIG              0x00000200
#define SNS_SMGR_HEARTBEAT_SIG              0x00000400
#define SNS_SMGR_QMI_MSG_SIG                0x00000800
#define SNS_SMGR_QMI_REG_CL_MSG_SIG         0x00004000
#define SNS_SMGR_QMI_REG_CL_TIMER_SIG       0x00008000
#define SNS_SMGR_INIT_TIMER_SIG             0x00010000

 /* If SMGR_MAX_DATA_TYPES_PER_DEVICE is expanded, define corresponding
    SNS_SMGR_SENSOR_xx_DATA_TYPE_y for additional y. */
#define SMGR_MAX_DATA_TYPES_PER_DEVICE       2

/* define sensor_id enumeration type */
typedef uint8_t smgr_sensor_id_e;

/* Forward declarations */
typedef struct sns_smgr_sensor_s        sns_smgr_sensor_s;
typedef struct sns_smgr_ddf_sensor_s    sns_smgr_ddf_sensor_s;

/* Sensor data structure for CONSTANT info common to all sensors */
typedef struct
{
  sns_ddf_driver_if_s*                  drv_fn_ptr;
  /* The UUID associated with the device driver */
  uint8_t                               uuid[SNS_SMGR_UUID_LENGTH_V01];
  /* Normal time (ticks) delay from power off to idle */
  uint16_t                              off_to_idle_time;
  /* Normal time (ticks) delay from low power mode until ready for command */
  uint16_t                              idle_to_ready_time;
  /* External sensor ID */
  smgr_sensor_id_e                      sensor_id;
  /* Address of slave device on the bus*/
  uint8_t                               bus_slave_addr; //buscfg smgr internal
  /* Which data type has range and or lpf attribute (pri/sec) */
  uint8_t                               range_sensor;
  /* Default sensitivity in terms of device driver command */
  uint8_t                               sensitivity_default;
  /* Registry item type for driver init */
  nv_id_t                               driver_reg_type;
  /* Registry item identifier for driver initialization */
  uint32_t                              driver_reg_id;
/* Registry item type for primary factory calibration */
  nv_id_t                               primary_cal_reg_type;
  /* Registry item identifier for primary data calibration */
  uint32_t                              primary_cal_reg_id;
  /* Identify an instance of a device driven by a device driver */
  uint8_t                               device_select;
 /* Flags as defined in FLAG VALUES in sns_smgr_sensor_config.h */
  uint8_t                               flags;
  /* Data types defined for this sensor device */
  sns_ddf_sensor_e                      data_types[SMGR_MAX_DATA_TYPES_PER_DEVICE];
  /* First GPIO assigned to the device. If the value is 0xffff, gpio is not assigned */
  uint16_t                              first_gpio;
  /* Second GPIO assigned to the device.If the value is 0xffff, gpio is not assigned */
  uint16_t                              second_gpio;
  /* Bus instance ID ( bus_address/bus_id ) - SMGR populates this to ddf and dd_init */
  uint16_t                              bus_instance; //buscfg smgr internal (was uint8_t)

  /* Power and IO rails for the sensor */
  /* MSB specifies rail type; LVS=0, LDO=1, rest of the bits specify rail num */
  uint8_t                               vdd_rail;
  uint8_t                               vddio_rail;
} smgr_sensor_cfg_s;

typedef struct
{
  uint32_t  depth;      /* max number of samples the depot can hold */
  uint32_t  mem_size;   /* size of depot in bytes */
} sns_smgr_depot_info_s;

typedef struct
{
  sns_smgr_ddf_sensor_s*  ddf_sensor_ptr;
  char      name[SNS_SMGR_MAX_SENSOR_NAME_SIZE_V01];
  char      vendor[SNS_SMGR_MAX_VENDOR_NAME_SIZE_V01];
  uint8_t   num_supported_odrs;
  uint16_t  odr_list[50];      // in ascending order
  uint16_t  max_sampling_rate; // last item in odr_list should not exceed this value
  uint16_t  idle_power;
  uint16_t  active_power;
  uint32_t  version;
  int32_t   max_range;
  int32_t   resolution;
  uint64_t  suid;

  sns_smgr_depot_info_s uimg_depot_info; /* sample depot in uImage */
  sns_smgr_depot_info_s bimg_depot_info; /* sample depot in bigImage */
} sns_smgr_ddf_sensor_info_s;

typedef struct 
{
  sns_smgr_ddf_sensor_info_s    ddf_sensor_info[SMGR_MAX_DATA_TYPES_PER_DEVICE];
} sns_smgr_sensor_info_s;

typedef struct
{
  sns_q_s                       protected_q;
  OS_EVENT*                     mutex_ptr;
} sns_smgr_protected_q_s;

typedef enum
{
   SNS_SMGR_REG_ITEM_TYPE_NONE,
   SNS_SMGR_REG_ITEM_TYPE_SINGLE,
   SNS_SMGR_REG_ITEM_TYPE_GROUP
} sns_smgr_RegItemType_e;


/*  =============== Sensor Calibration structure =============== */
typedef struct 
{
  bool      used;           /* indicate if calibration is applicable */
  bool      need_to_log;    /* mark filed if the sample need to be logged when sending a response */
  /*
    Q16 format (16 bits for integer part, 16 bits for decimal part),
    indicating the zero bias that is to be added (in nominal engineering units);
  */
  int32_t   zero_bias[SNS_SMGR_SENSOR_DIMENSION_V01];

  /*
    Q16 format, a multiplier that indicates scale factor need to be multiplied to current data .
    For example, enter 1.01 if the scaling is 1% less aggressive or 0.95 if it is 5% more aggressive.
  */
  int32_t   scale_factor[SNS_SMGR_SENSOR_DIMENSION_V01];

  /*  
    If the Compensation Matrix is valid, the scale_factor above is ignored. 
    The calibrated sample (Sc) is copmuted as 
    Sc = CM(Sr - Bias)
    where : 
        Sc = Calibrated sensor sample
        Sr = Read sensor sample
        CM = Compensation Matrix
        Bias =

    Matrix elements are in Q16 format in row major order, ie: 
    CM =  CM0  CM1  CM2
          CM3  CM4  CM5
          CM6  CM7  CM8
  */
  bool      compensation_matrix_valid; /* True if CM is valid, false otherwise */
  int32_t   compensation_matrix[SNS_SMGR_COMPENSATION_MATRIX_SIZE_V01];

  uint32_t  calibration_accuracy;
  /* The higher the better : Default 0 - 5 */
} sns_smgr_cal_s;

typedef struct
{
  sns_smgr_cal_s            factory_cal;
  sns_smgr_cal_s            auto_cal;
  sns_smgr_cal_s            full_cal;     /* factory cal + auto_cal */
} sns_smgr_all_cal_s;

typedef struct
{
  sns_q_link_s              q_link;
  void*                     conn_handle;
  void*                     req_handle; // actually msg header 
  sns_smgr_ddf_sensor_s*    ddf_sensor_ptr;
  uint16_t                  msg_id;
  uint8_t                   svc_num;

  /* request and response messages are allocated and freed by RH;
     not all requests need to be responded by RH */
  union
  {
    sns_smgr_sensor_cal_req_msg_v01*         calibration_ptr;
    sns_smgr_single_sensor_test_req_msg_v01* self_test_ptr;
    sns_smgr_driver_access_req_msg_v01*      driver_access_ptr;
  } req;

  union
  {
    sns_common_resp_s_v01*                   common_resp_ptr;
    sns_smgr_driver_access_resp_msg_v01*     driver_access_ptr;
    sns_smgr_sensor_cal_resp_msg_v01*        calibration_ptr;
  } resp;

} sns_smgr_request_response_s;

typedef struct
{
  sns_smgr_test_result_e_v01  result;
  uint32_t                    test_specific_error;

} sns_smgr_self_test_result_s;

typedef struct
{
  sns_q_link_s              q_link;
  void*                     conn_handle;
  uint16_t                  ind_len;     
  uint16_t                  msg_id;

  /* indication messages are allocated by filled by SMGR and freed by RH */
  union
  {
    sns_smgr_driver_access_ind_msg_v01      driver_access;
  } ind;

} sns_smgr_indication_s;

extern sns_q_s                sns_msgr_lo_q, sns_msgr_hi_q;
extern sns_smgr_sensor_info_s sns_smgr_sensor_info[SNS_SMGR_NUM_SENSORS_DEFINED];

/*===========================================================================

                            FUNCTIONS

===========================================================================*/
#endif /* SNS_SMGR_H */
