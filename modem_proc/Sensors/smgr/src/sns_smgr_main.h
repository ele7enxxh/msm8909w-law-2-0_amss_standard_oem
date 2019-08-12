#ifndef SNS_SMGR_MAIN_H
#define SNS_SMGR_MAIN_H
/*=============================================================================
  @file sns_smgr_main.h

  This header file contains the data definitions used within the DSPS
  Sensor Manager (SMGR)

******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_main.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-04-04  pn   Distinguishes between requested ODR and current ODR
  2016-03-11  bd   Move reserved fifo size to uimage
  2016-03-11  pn   Handles FIFOs delivered in chunks
  2016-01-07  pn   Reboots sensor after its heartbeat stops
  2015-11-03  pn   Moved DDF timer handling to DDF
  2015-09-24  pn   Code cleanup
  2015-09-22  pn   Self-scheduling streaming sensors are monitored for activities
  2015-08-25  tc   Added daf_active field to sns_smgr_sensor_s
  2015-08-21  bd   Do not send sample_ready_sig for buffering report
  2015-08-19  pn   Simplified powerrails control
  2015-07-30  bd   Added num_buffering_reports field in sns_smgr_client_stat_s
  2015-06-05  pn   Used uint16_t for sns_smgr_sensor_s's flags
  2015-05-22  pn   Added current_filter_delay to sns_smgr_ddf_sensor_s
  2015-04-21  pn   Removed deprecated field new_sample_idx
  2015-04-09  pn   Updated timestamps from uint32_t to sns_ddf_time_t
  2015-03-20  pn   Optimized retrieval of latest sample timestamp
  2015-03-10  jms  - Calculate unified FIFO WM from max report rate of sensors in this FIFO
                   - Removed unused/ineffective discrete FIFO synch code
  2015-02-17  pn   Added first_valid_sample_received to sns_smgr_ddf_sensor_s
  2014-11-03  jtl  Add support for timestamp accurate clients in monitor ind
  2014-12-05  pn   Added powerstate to sns_smgr_sensor_s
  2014-09-25  pn   Added bigImage depot
  2014-09-11  pn   Protects all shared sensor state variables, not just client DB
  2014-09-08  pn   Moved PM interface functions to PM_IF module
  2014-07-31  pn   Removed POWER_INTERFACE featurization
  2014-07-09  vy   Added support for non-refactored DD
  2014-06-26  pn   Removed obsolete HP ODR table
  2014-06-16  pn   Updated sns_smgr_client_stat_s
  2014-06-05  vy   Added function declarations   
  2014-05-28  tc   SubHz Sampling
  2014-05-24  vy   Added function declarations
  2014-05-12  tc   Converted frequencies to fixed point, Q16. This is to
                   accommodate frequencies less than 1. Q16 fixed point is
                   used, instead of floating point, because the Q6 does not
                   have hardware-support for floating point division.
  2014-05-15  MW   Make lpf_table q16
  2014-05-13  vy   Refactored for uImage and PM support
  2014-05-09  pn   Removed no longer used sensor range structure definitions
  2014-05-07  pn   Removed unused code
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_driver_if.h"
#include "sns_em.h"
#include "sns_queue.h"
#include "sns_rh.h"
#include "sns_smgr.h"
#include "sns_smgr_depot.h"
#include "sns_smgr_fifo.h"
#include "sns_smgr_hw.h"
#include "sns_pm.h"

/*----------------------------------------------------------------------------
 * Constants
 * -------------------------------------------------------------------------*/
#define MAX_HP_ODR_TB_SIZE                3

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

/*  =============== Schedule structure ===============
    Contains scheduling parameters. One for each requested reporting
    rate, and one for each sensor for sensor-specific scheduling data.
    Report and sensor leader blocks point here for scheduling information.
    This structure is used for deconflicting reporting rates. Deconfliction
    happens when a new rate is introduced to the SOL mix or a rate becomes
    disused or we need to modify the maximum sampling rate.

    When a report is added or has its rate modified it needs to link to one
    of these structures, adding a new structure if introducing a new rate.
    Each active sensor needs a structure here, too. The structures must be
    added when the need is recognized, although they will not be initialized
    until later. If too few blocks are available, the new request must be
    rejected. Excess blocks will be returned when deconfliction happens.
*/
typedef struct  smgr_sched_block_s
{
  /* Chain schedule structures together in order: heartbeat then
     shortest to longest reporting/sampling intervals */
  sns_q_link_s                        sched_link;

  /* Maintain all sensor types belonging to this schedule block */
  sns_q_s                             ddf_sensor_queue;

  /* Requested rate */
  q16_t                               sampling_rate;

  /* The current scheduling interval by clock ticks. This tick is calculated by req_rate */
  uint32_t                            sched_intvl;

  /* Current assigned interval, scheduling clock ticks. Zero if not
     initialized */
  sns_ddf_time_t                      next_tick;
} smgr_sched_block_s;

/* Only valid when smgr_sensor_state_e is SENSOR_STATE_READY */
typedef enum
{
  SENSORTYPE_STATE_IDLE,
  SENSORTYPE_STATE_READ_PENDING,
} smgr_sensor_type_state_e;

typedef struct
{
  sns_q_s                       request_queue;
  sns_smgr_ddf_sensor_s*        ddf_sensor_ptr; /* associated DDF sensor */
  sns_ddf_time_t                last_status_ts; /* ts of the most recent event */
} sns_smgr_sensor_status_s;

typedef struct
{
  /* when one or more of these fields change the report rate must be recomputed */
  sns_ddf_odr_t   odr;
  q16_t           depot_data_rate;
  q16_t           max_req_sampling_rate;
  sns_ddf_time_t  last_updated;
} sns_smgr_client_modifier_s;

typedef struct
{
  sns_smgr_sensor_status_s  sensor_status;

  /* singly-linked list of report items */
  sns_rh_rpt_item_s*        rpt_item_ptr;
  uint8_t                   num_rpt_items;
  uint8_t                   num_periodic_reports;
  uint8_t                   flags;

  /* highest requested sampling rate amongst all reports */
  q16_t                     max_requested_freq_hz;
  /* highest requested report rate amongst all reports */
  q16_t                     max_report_freq_hz;
  sns_smgr_client_modifier_s  client_modifier;

} sns_smgr_client_stat_s;

struct sns_smgr_ddf_sensor_s
{
  /* link to sampling schedule queue */
  sns_q_link_s                sched_link;

  /* the sensor to which this DDF sensor belongs */
  sns_smgr_sensor_s*          sensor_ptr;
  uint8_t                     data_type;    /* primary or secondary */
  uint8_t                     flags;
  bool                        first_valid_sample_received;

  sns_smgr_ddf_sensor_info_s* ddf_sensor_info;

  sns_smgr_sample_depot_s*    uimg_depot_ptr;
  sns_smgr_sample_depot_s*    bimg_depot_ptr;
  OS_EVENT*                   depot_mutex_ptr;
  sns_ddf_time_t              latest_sample_ts;

  sns_smgr_client_stat_s      client_stat;

  /* calibration data */
  sns_smgr_all_cal_s*         all_cal_ptr;

  smgr_sensor_type_state_e    sensor_type_state;
  sns_ddf_time_t              data_poll_ts; /* timestamp when data was last polled */

  /* the ODR configured in device */
  sns_ddf_odr_t               requested_odr;
  sns_ddf_odr_t               current_odr;

  /* the timestamp adjustment, in ticks, that DD applies to samples
     before sending them to SMGR */
  sns_ddf_filter_delay_t      current_filter_delay;

  /* ratio between current ODR and max requested rate;
     only one in device_sampling_factor samples will go into the depot */
  uint32_t                    device_sampling_factor;

  /* the sampling frequency of data in depot; 
     = device_sampling_rate/device_sampling_factor 
     only greater than 1 when HP clients exist and no CIC request */
  q16_t                       depot_data_rate;
  uint32_t                    depot_data_interval;
  uint32_t                    fifo_interval; /* tick count between full FIFOs */
  uint32_t                    heartbeat_interval;

  uint16_t                    num_expected_samples; /* 1 for Polling or DRI, WM for FIFO */
  uint16_t                    num_received_samples; /* since last full FIFO */

  /* highest supported rate; provided by DD at init */
  q16_t                       max_supported_freq_hz;

  /* count down from dri_ratio to 0 */
  uint32_t                    dri_count;

  /*reserved sample count for wakeup/nonwakeup sensor*/
  uint32_t                    reserved_sample_cnt_nonwakeup;

  /* keeps track of number of skipped samples */
  uint8_t                     max_skipped_samples;
};

/*  =============== SensorLeader structure ===============
    Contains VARIABLE parameters to manage collection from a sensor
*/
struct sns_smgr_sensor_s 
{
  /* references the static table entry for the sensor */
  const smgr_sensor_cfg_s*        const_ptr;

  sns_pm_pwr_rail_info_s*         vdd_rail_ptr;
  sns_pm_pwr_rail_info_s*         vddio_rail_ptr;

  /* handle to driver object */
  sns_ddf_handle_t                dd_handle;
  
  /* Registry item parameters for device driver initialization */
  sns_ddf_nv_params_s             reg_item_param;

  /* Define sensors available from this device. The driver populates this
     at initialization */
  uint8_t                         num_data_types;
  sns_smgr_ddf_sensor_s*          ddf_sensor_ptr[SMGR_MAX_DATA_TYPES_PER_DEVICE];

  sns_smgr_protected_q_s          request_queue;

  /* Use memhandler for each device in short read. Minimize time delay between
     reading one sensor and the next */
  sns_ddf_memhandler_s            memhandler;

  /* Number of Low Pass Filters in hardware */
  uint8_t                         num_lpf;
  q16_t                           lpf_table[SMGR_MAX_LPF_FREQ_IN_SENSOR];

  /* ODRs supported by DD */
  sns_ddf_odr_t*                  supported_odrs;
  uint8_t                         num_odrs;

  /* Initialization state */
  sns_smgr_sensor_init_state_e    init_state;

  /* Sensor state (actually applies to a device driver) */
  sns_smgr_sensor_state_e         sensor_state;
  sns_ddf_powerstate_e            powerstate;
  bool                            daf_active;

  /* Flags to coordinate reading registry */
  uint16_t                        flags;
  
  uint8_t                         num_reboots;

  sns_ddf_time_t                  event_done_tick;
  sns_ddf_time_t                  odr_change_tick; /* timestamp at last ODR config */

  OS_EVENT*                       mutex_ptr;

  /* Sensor FIFO Configuration */
  smgr_sensor_fifo_cfg_s          fifo_cfg;

  //flag set to true/false when intr enabled/disabled in sns_smgr_enable_sched_data
  //initially set to false in sns_smgr_sensor_init
  bool                            is_self_sched_intr_enabled; //is intr enabled (is in DRI/self sched mode)?
  bool                            odr_attr_supported;
  
  /* refactored for uImage mode or not */
  bool                            is_uimg_refac;

};

typedef struct
{
  /* input */
  sns_smgr_single_sensor_test_req_msg_v01* self_test_req;
  sns_smgr_ddf_sensor_s*          ddf_sensor_ptr;

  /* output */
  sns_smgr_sensor_state_e         pre_test_state;
  sns_smgr_self_test_result_s     result;
} sns_smgr_self_test_s;

/*  =============== Smgr QMD structure ===============
    Contains QMD related info for SMGR
*/

typedef struct  
{
  /* State of overall device driver initialization */
  sns_smgr_sensor_all_init_state_e  all_init_state;
  sns_ddf_time_t                    init_start_tick;
  sns_smgr_sensor_s                 sensor[SNS_SMGR_NUM_SENSORS_DEFINED];
  intptr_t                          uimg_depot_mem_ptr;
  intptr_t                          bimg_depot_mem_ptr;

  /* the tick value of current scheduling time */
  sns_ddf_time_t               sched_tick;
  sns_ddf_time_t               last_tick;

  /* queue for schedule blocks */
  sns_q_s                      sched_que;

  /* queue of self-sched sensors being monitored */
  sns_q_s                      heartbeat_monitor_que;

  /* flags used for multiple purposes */
  uint8_t                      flags;

  sns_em_timer_obj_t           tmr_obj;
  sns_em_timer_obj_t           sensor_ev_tmr_obj;
  sns_em_timer_obj_t           heartbeat_tmr_obj;

  bool                         md_is_enabled;
  bool                         app_is_awake;
  uint32_t                     app_state_toggle_count;

  OS_FLAG_GRP*                 sig_grp;
  OS_FLAGS                     sig_flag;

  /* SSI sensor dependent reg items state */
  uint16_t                     last_requested_sensor_dep_reg_group_id;
  uint16_t                     last_received_reg_group_id;
  
  sns_smgr_self_test_s         self_test;

} sns_smgr_s;

#define SNS_DRI_SIMULATE       0x1
typedef struct
{
  OS_STK                       DRITask[SNS_MODULE_STK_SIZE_DSPS_PLAYBACK];
  uint8_t                      dri_sim_task_created;
  uint8_t                      start_irq;
  OS_FLAG_GRP                  *sns_dri_sig_grp;
} sns_dri_sim_s;

extern smgr_sensor_cfg_s       smgr_sensor_cfg[SNS_SMGR_NUM_SENSORS_DEFINED ];
extern sns_smgr_s              sns_smgr;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern void sns_smgr_sensor_init(void);

extern void sns_smgr_sampling_cycle(void);

extern void sns_smgr_dd_init(void);

extern void sns_smgr_schedule_sensor_event(uint32_t tick_offset);

extern void smgr_process_sensor_event(void);

extern void sns_smgr_set_all_sensors_state(sns_smgr_sensor_state_e state);

extern void sns_smgr_reboot_sensor(sns_smgr_ddf_sensor_s* ddf_sensor_ptr);

extern void smgr_process_dri_fifo_async_data(
  sns_smgr_ddf_sensor_s*        ddf_sensor_ptr,
  const sns_ddf_sensor_data_s*  data_ptr);

extern void sns_smgr_update_sampling_setting(
  sns_smgr_ddf_sensor_s*  ddf_sensor_ptr);

extern void sns_smgr_timer_cb(void *cb_arg);

extern void sns_smgr_init_timer_cb(void *cb_arg);

extern void sns_smgr_task(void* p_arg);

extern void sns_smgr_task_main(void);

extern void sns_smgr_app_state_change(bool app_is_asleep);

extern void sns_smgr_handle_bigimage_signals(OS_FLAGS sig_flags);

#endif /* #ifndef SNS_SMGR_MAIN_H */

