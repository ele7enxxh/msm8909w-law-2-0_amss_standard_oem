#ifndef SNS_SMGR_PRIV_H
#define SNS_SMGR_PRIV_H
/*=============================================================================
  @file sns_smgr_priv.h

  This header file contains the data definitions used within SMGR

******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_priv.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-01-07  pn   Added SMGR_MAX_REBOOTS
  2015-11-19  tc   Increased SMGR_MAX_EIS_QUERY_REQUESTS from 2 to 4
  2015-09-24  pn   Code cleanup
  2015-09-10  pn   Obsoleted SMGR_ALIGN_FREQ
  2015-08-19  pn   Simplified powerrails control
  2015-06-05  pn   Added SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B
  2015-05-14  pn   Reduced SMGR_ODR_THRESHOLD_DELTA_PCT to 10 percent
  2015-04-09  pn   Added support for 56-bit timestamp
  2015-03-20  pn   Added TICK_MIN() and TICK_MAX()
  2015-02-17  pn   Added SMGR_SENSOR_FLAGS_FIRST_VALID_SAMPLE_B
  2014-12-08  pn   Updated time comparison macros
  2014-11-11  pn   Updated SMGR_MAX_TICKS; removed unused definitions
  2014-08-05  pn   Added ODR_CHANGED and FLUSH_FIFO to sensors bit flags
  2014-06-16  pn   Removed unused definitions
  2014-05-07  pn   Added support for MD deregistration
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_smgr.h"
#include "sns_osa.h"
#include "sns_ddf_common.h"
#include "sns_ddf_util.h"
#include "sns_profiling.h"
#include "sns_memmgr.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Nominal microseconds per tick of the scheduling clock. It is understood
     that the clock can drift because of clock crystal variation. This
     may look like a FLT number, but the compiler uses it in a constant
     expression which is then cast to an integer.
*/
#define SNS_SMGR_USEC_PER_TICK            30.51
#define SNS_SMGR_POWERRAIL_HYSTERESIS_TIMER_VALUE_TICK              30510

#define SNS_SMGR_TIMER_QUEUE_SIZE   SNS_SMGR_NUM_SENSORS_DEFINED + 1
#define SNS_SMGR_IRQ_QUEUE_SIZE     SNS_SMGR_NUM_SENSORS_DEFINED

/* reg_request_flags */
#define REG_REQ_DRIVER                    0x01
#define REG_REQ_CAL_PRI                   0x02

#define SMGR_TOO_SHORT_US_FOR_TIMER       100
#define SMGR_TOO_SHORT_TICKS_FOR_TIMER    (TIMETICK_SCLK_FROM_US(SMGR_TOO_SHORT_US_FOR_TIMER))

#define SMGR_ODR_THRESHOLD_DELTA_PCT      10
#define SMGR_MAX_EIS_QUERY_SAMPLES        50
#define SMGR_MAX_EIS_QUERY_REQUESTS       4

#define SMGR_GET_ENTRY(ptr, type, member) \
  ((type *)((unsigned long)(ptr)-((unsigned long)(&((type *)0x10000)->member) - 0x10000)))

/* loops through singly-linked list of report item associated with each ddf sensor */
#define SMGR_FOR_EACH_ASSOC_ITEM(ddf_sensor_ptr,ip,ipp) \
  for ( ipp=&(ddf_sensor_ptr)->client_stat.rpt_item_ptr; *ipp!=NULL && ((ip)=*ipp)!=FALSE; \
        ipp=&(*ipp)->next_item_ptr )

#define SMGR_MAX_REBOOTS                    3
#define SMGR_MAX_TICKS                      0x7FFFFFF
#define SMGR_SENSOR_CLK_ACCURACY            (1.1)
#define SMGR_LPF_DURATION(lpf_odr)          \
  ((lpf_odr==0)?0:TIMETICK_SCLK_FROM_US((1000000*SMGR_SENSOR_CLK_ACCURACY+SMGR_SCHEDULE_GRP_TOGETHER_US)/(lpf_odr+4)))

#define SMGR_DELAY_US(delay_usec)           sns_ddf_delay(delay_usec)

/* TRUE if t1 is not in the past of t2 */
#define TICK1_GEQ_TICK2(t1,t2) ((t1) >= (t2))

/* TRUE if t1 is in future of t2 */
#define TICK1_GTR_TICK2(t1,t2) ((t1) >  (t2))

#define TICK_MIN(t1,t2) (TICK1_GTR_TICK2((t1),(t2)) ? (t2) : (t1))

#define TICK_MAX(t1,t2) (TICK1_GTR_TICK2((t1),(t2)) ? (t1) : (t2))

#define SMGR_OK_TO_SEND(r) (((r)->proc_type != SNS_PROC_APPS_V01) || \
                            sns_smgr.app_is_awake || \
                            (r)->send_ind_during_suspend)

#define SMGR_DRV_FN_PTR(s) ((s)->const_ptr->drv_fn_ptr)
#define SMGR_SENSOR_ID(s) ((s)->const_ptr->sensor_id)
#define SMGR_SENSOR_IS_ACCEL(s) ( (SNS_SMGR_ID_ACCEL_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_2_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_3_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_4_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_5_V01 == SMGR_SENSOR_ID(s)) )
#define SMGR_SENSOR_IS_GYRO(s)  ( (SNS_SMGR_ID_GYRO_V01  == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_2_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_3_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_4_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_5_V01 == SMGR_SENSOR_ID(s)) )
#define SMGR_SENSOR_IS_MAG(s)   (SNS_SMGR_ID_MAG_V01          == SMGR_SENSOR_ID(s))
#define SMGR_SENSOR_IS_PROX(s)  (SNS_SMGR_ID_PROX_LIGHT_V01   == SMGR_SENSOR_ID(s))
#define SMGR_SENSOR_IS_SELF_SCHED(s) ((s)->const_ptr->flags & SNS_SMGR_DD_SELF_SCHED)
#define SMGR_SENSOR_FIFO_IS_ENABLE(s) ((s)->const_ptr->flags & SNS_SMGR_ENABLE_FIFO) 
#define SMGR_SENSOR_TYPE(s,i) ((s)->const_ptr->data_types[i])
#define SMGR_SENSOR_TYPE_PRIMARY(s) ((s)->const_ptr->data_types[SNS_SMGR_DATA_TYPE_PRIMARY_V01])
#define SMGR_SENSOR_TYPE_SECONDARY(s) ((s)->const_ptr->data_types[SNS_SMGR_DATA_TYPE_SECONDARY_V01])
#define SMGR_SENSOR_FIFO_IS_SUPPORTED(s) (((s)->fifo_cfg).is_fifo_supported)
#define SMGR_SENSOR_FIFO_CAN_BE_USED(s) (((s)->fifo_cfg).can_fifo_be_used)
#define SMGR_PRIMARY_SENSOR(s) ((s)->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01])
#define SMGR_SECONDARY_SENSOR(s) ((s)->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_SECONDARY_V01])

#define SMGR_HANDLE_VALID(h) \
  (((h)>=&sns_smgr.sensor[0]) && ((h)<=&sns_smgr.sensor[ARR_SIZE(sns_smgr.sensor)-1]))

#define SMGR_DATATYPE_VALID(d) \
  ((d==SNS_SMGR_DATA_TYPE_PRIMARY_V01) || (d==SNS_SMGR_DATA_TYPE_SECONDARY_V01))

#define SMGR_SENSOR_INTERRUPT_ENABLED(sensor_ptr) ((sensor_ptr)->const_ptr->first_gpio != 0xffff)

#define SMGR_SENSOR_IN_USE(s) \
  (((s)->sensor_state > SENSOR_STATE_FAILED) && \
   (((s)->sensor_state > SENSOR_STATE_IDLE) || ((s)->flags != 0) || \
    (sns_q_cnt(&(s)->request_queue.protected_q) > 0) || ((s)->daf_active)))

#define SMGR_MSG_HISTORY_MAX_COUNT           16
#define SMGR_MSG_HISTORY_BYTES2SAVE          32

/* bits assigned to smgr_sensor_s.flags */
#define SMGR_SENSOR_FLAGS_ITEM_ADD_B                0x0001 /* adding a client              */
#define SMGR_SENSOR_FLAGS_ITEM_DEL_B                0x0002 /* deleting a client            */
#define SMGR_SENSOR_FLAGS_ENABLE_MD_B               0x0004 /* enabling MD interrupt        */
#define SMGR_SENSOR_FLAGS_DISABLE_MD_B              0x0008 /* disabling MD interrupt       */
#define SMGR_SENSOR_FLAGS_SELF_TEST_DONE_B          0x0010 /* self-test completed          */
#define SMGR_SENSOR_FLAGS_ODR_CHANGED_B             0x0020 /* ODR changed due to a sibling */
#define SMGR_SENSOR_FLAGS_FLUSH_FIFO_B              0x0040 /* RH requesting FIFO flush     */
#define SMGR_SENSOR_FLAGS_FIRST_VALID_SAMPLE_B      0x0080 /* first valid sample received  */
#define SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B         0x0100 /* WM changed due to a sibling  */

// The macro generates compile-time error if cond is 'false', otherwise no code
// is generated. Assumes cond can be evaluated at compile time.
#define SMGR_ASSERT_AT_COMPILE(cond) ( (void) sizeof(char[1 - 2*!!!(cond)]) )

/*----------------------------------------------------------------------------
 * Enum Definitions
 * -------------------------------------------------------------------------*/
typedef enum
{
  SENSOR_STATE_PRE_INIT,
  SENSOR_STATE_FAILED,      /*1 No success over N successive sample attempts */
  SENSOR_STATE_OFF,         /*2 Power off state because of POWER RAIL off */
  SENSOR_STATE_POWERING_UP, /*3 Power rail coming up */
  SENSOR_STATE_IDLE,        /*4 Low power mode or sleep mode */
  SENSOR_STATE_CONFIGURING, /*5 Being configured */
  SENSOR_STATE_READY,       /*6 Active, ready to command */
  SENSOR_STATE_MD,          /*7 MD enabled */
  SENSOR_STATE_TESTING      /*7 In test mode */
} sns_smgr_sensor_state_e;

/* State of overall device driver initialization */
typedef enum
{
  SENSOR_ALL_INIT_NOT_STARTED,
  SENSOR_ALL_INIT_WAITING_CFG,
  SENSOR_ALL_INIT_WAITING_AUTODETECT,
  SENSOR_ALL_INIT_AUTODETECT_DONE,
  SENSOR_ALL_INIT_CONFIGURED,
  SENSOR_ALL_INIT_IN_PROGRESS,
  SENSOR_ALL_INIT_DONE
} sns_smgr_sensor_all_init_state_e;

/* State of initialization for a device driver */
typedef enum
{
  SENSOR_INIT_NOT_STARTED,
  SENSOR_INIT_WAITING_REG,
  SENSOR_INIT_REG_READY,
  SENSOR_INIT_WAITING_TIMER,
  SENSOR_INIT_FAIL,
  SENSOR_INIT_SUCCESS
} sns_smgr_sensor_init_state_e;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
extern void sns_smgr_signal_me(uint32_t sig);

extern void sns_smgr_cancel_service(void* connection_handle);

#endif /* #ifndef SNS_SMGR_PRIV_H */

