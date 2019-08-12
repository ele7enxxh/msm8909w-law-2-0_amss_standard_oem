/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifndef HAL_RBCPR_H
#define HAL_RBCPR_H

#include <stdbool.h>
#include "railway.h"

#define HAL_RBCPR_LOG_SIZE    2
#define HAL_RBCPR_ENUM_32(x)  HAL_RBCPR_##x = 0x7FFFFFFF

#define HAL_RBCPR_MASK_AND_SHIFT_TO_LSB(value, offset_of_lsb, number_of_bits) \
    (((value) << (32-((offset_of_lsb)+(number_of_bits))) ) >> (32-(number_of_bits)))
typedef uint8 HAL_rbcpr_rosc_type;
typedef uint8 HAL_rbcpr_sensor_type;

typedef struct HAL_rbcpr_rail_s const * HAL_rbcpr_rail_t;

typedef enum
{
  HAL_RBCPR_INT_DONE_FLAG = 0x01,
  HAL_RBCPR_INT_MIN_FLAG  = 0x02,
  HAL_RBCPR_INT_DOWN_FLAG = 0x04,
  HAL_RBCPR_INT_MID_FLAG  = 0x08,
  HAL_RBCPR_INT_UP_FLAG   = 0x10,
  HAL_RBCPR_INT_MAX_FLAG  = 0x20,
} HAL_rbcpr_interrupt_src_type;


typedef struct
{
  uint8  interval;
  boolean enable;
} HAL_rbcpr_timer_type;

typedef enum
{
  HAL_RBCPR_QUOT_SLOW,
  HAL_RBCPR_QUOT_FAST,
  HAL_RBCPR_ENUM_32(INVALID_MEASURE),
} HAL_rbcpr_meas_type;

typedef enum
{
  HAL_RBCPR_ALL_AT_ONCE,
  HAL_RBCPR_STAGGERED,
} HAL_rbcpr_count_mode;

typedef enum
{
  HAL_RBCPR_IDLE,
  HAL_RBCPR_BUSY,
} HAL_rbcpr_status;

typedef struct
{
  uint8 ceiling;
  uint8 floor;
  uint8 vlevel;
} HAL_rbcpr_settings;

typedef struct
{
    uint32 rbcpr_timer_interval;
    uint32 rbif_timer_adjust;
    uint32 rbcpr_ctl;
    uint32 rbcpr_result_0;
    uint32 rbcpr_result_1;
    uint32 rbif_irq_en_0;
    uint32 rbif_irq_status;
    uint32 rbcpr_debug0;
    uint32 rbcpr_debug1;
    uint32 rbcpr_debug2;
    uint32 rbcpr_log_0;
    uint32 rbcpr_log_1;
    uint32 rbcpr_log_2;
    uint32 rbcpr_log_3;
    uint32 rbcpr_log_4;
    uint32 rbcpr_log_5;
    uint32 rbcpr_log_6;
    uint32 rbcpr_log_7;
} HAL_rbcpr_dump;

typedef struct HAL_rbcpr_rail_s
{
  HAL_rbcpr_timer_type            timer_config;
  HAL_rbcpr_sensor_type           sensors;
  HAL_rbcpr_settings              settings;
  uint32                          hw_base_address;
  const char*                     name;
  HAL_rbcpr_dump                  core_dump;
} HAL_rbcpr_rail_s;

typedef struct
{
  const HAL_rbcpr_rail_s* const rails;
  int             num_rails;
} HAL_rbcpr_bsp_type;

typedef struct
{
  boolean busy;
  uint16  error;
  uint8   error_steps;
  boolean step_up;
  boolean step_down;
  boolean sw_decision;
} HAL_rbcpr_result_type;

typedef struct
{
  uint32                      rbcpr_result;
  uint32                      timestamp;
  uint32                      log_0;
  railway_corner              corner;
} HAL_rbcpr_log_entry_type;

typedef struct
{
  uint32 limit;
  uint32 step_quot;
  uint32 gcnt_target[8];
  uint32 irq0;
  uint32 irq_status;
} HAL_rbcpr_coredump_type;

typedef struct
{
  HAL_rbcpr_log_entry_type    rbcpr_logs[HAL_RBCPR_LOG_SIZE];
  uint8                       rbcpr_log_index;
  HAL_rbcpr_coredump_type     rbcpr_coredump[2];
  uint8                       coredump_index;
} HAL_rbcpr_log_type;


/* ============================================
FUNCTION HAL_rbcpr_enable_int

DESCRIPTION
  HAL layer helper function to configure a particular
  top level interrupt with the config provided.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_rbcpr_enable_int(HAL_rbcpr_rail_t rail, uint32 interrupt_mask, uint8 enable);
/* ============================================
FUNCTION HAL_RBCPR_PRGM_GCNT_TARGET

DESCRIPTION
  HAL layer function to program gate count and target
  values for a particular ring oscillator.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_prgm_gcnt_target(HAL_rbcpr_rail_t rail, HAL_rbcpr_rosc_type osc, uint32 gcnt, uint32 target);

/* ============================================
FUNCTION HAL_RBCPR_ENABLE_MEAS

DESCRIPTION
  HAL layer helper function to enable measurements

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_enable_meas(HAL_rbcpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_RBCPR_READ_STATUS

DESCRIPTION
  HAL layer helper function to read the status of the measurement

RETURN VALUE
  Returns the status of the measurement

SIDE EFFECTS
  None
 */

uint8 HAL_rbcpr_read_status(HAL_rbcpr_rail_t rail);

/* ============================================
FUNCTION HAL_RBCPR_POLL_RESULT

DESCRIPTION
  HAL layer helper function to read the result of a measurement

RETURN VALUE
  Returns the result of the measurement

SIDE EFFECTS
  None
 */

void HAL_rbcpr_poll_result(HAL_rbcpr_rail_t rail, HAL_rbcpr_result_type *);

/* ============================================
FUNCTION HAL_RBCPR_READ_QUOT

DESCRIPTION
  HAL layer function to read the quot

RETURN VALUE
  Returns the result of the measurement

SIDE EFFECTS
  None
 */
uint32 HAL_rbcpr_read_quot(HAL_rbcpr_rail_t rail, HAL_rbcpr_meas_type);

/* ============================================
FUNCTION HAL_RBCPR_WRITE_STEPQUOT

DESCRIPTION
  HAL layer function to write the stepquot value

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_write_stepquot(HAL_rbcpr_rail_t rail, uint32 stepquot, uint32 idle_clocks);

/* ============================================
FUNCTION HAL_RBCPR_ENABLE_SENSOR

DESCRIPTION
  HAL layer function to enable/disable a sensor

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_enable_sensor(HAL_rbcpr_rail_t rail, HAL_rbcpr_sensor_type, boolean);

/* ============================================
FUNCTION HAL_RBCPR_MASK_SENSOR_ALL

DESCRIPTION
  HAL layer function to enable/disable all sensors

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_mask_sensor_all(HAL_rbcpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_RBCPR_BYPASS_SENSOR

DESCRIPTION
  HAL layer function to bypass/pass through a sensor

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_bypass_sensor(HAL_rbcpr_rail_t rail, HAL_rbcpr_sensor_type, boolean);

/* ============================================
FUNCTION HAL_RBCPR_BYPASS_SENSOR_ALL

DESCRIPTION
  HAL layer function to bypass/pass through all sensors

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_rbcpr_bypass_sensor_all(HAL_rbcpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_CONFIG_TIMERS

DESCRIPTION
  HAL layer function to program the timers.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_config_timers(HAL_rbcpr_rail_t rail);

/* ============================================
FUNCTION HAL_RBCPR_PGM_TIMER

DESCRIPTION
  HAL layer helper function to program the timer interval and enable/disable it.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_pgm_timer(HAL_rbcpr_rail_t rail, uint32 interval, boolean enable);

/* ============================================
FUNCTION HAL_RBCPR_CLEAR_INT

DESCRIPTION
  HAL layer function to clear an interrupt

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_rbcpr_clear_int(HAL_rbcpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_RBCPR_CLEAR_INT_ALL

DESCRIPTION
  HAL layer function to clear all interrupts that are set

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_rbcpr_clear_int_all(HAL_rbcpr_rail_t rail);

/* ============================================
FUNCTION HAL_RBCPR_NEXT_MEAS_EN

DESCRIPTION
  HAL layer function to tell the RBCPR block that the
  recommendation was taken / not taken.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_rbcpr_next_meas_en(HAL_rbcpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_RBCPR_ENABLE_BLOCK

DESCRIPTION
  HAL layer function to enable / disable the block

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_rbcpr_enable_block(HAL_rbcpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_CONFIG_LIMIT

DESCRIPTION
  HAL layer function to configure the limits for the voltage
  levels

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_config_vlimits(HAL_rbcpr_rail_t rail);

/* ============================================
FUNCTION HAL_RBCPR_SAVE_LOG

DESCRIPTION
  HAL layer function save the logs

RETURN VALUE
  None

SIDE EFFECTS
  None
*/
void HAL_rbcpr_save_log(HAL_rbcpr_rail_t rail, HAL_rbcpr_log_type* log_buffer, railway_corner);

/* ============================================
FUNCTION HAL_RBCPR_CONFIGURE_STEP_DN

DESCRIPTION
  Hack: HAL layer function to configure interrupts and SWLEVEL after first voltage switch

RETURN VALUE
  None

SIDE EFFECTS
  None
*/
void HAL_rbcpr_configure_step_dn(HAL_rbcpr_rail_t rail);

/* ============================================
FUNCTION HAL_RBCPR_CLOCK_ENABLE

DESCRIPTION
  HAL layer function to enable the clocks for the RBCPR block

RETURN VALUE
  None

SIDE EFFECTS
  None
*/
void HAL_rbcpr_clock_enable(HAL_rbcpr_rail_t rail);

void HAL_rbcpr_collect_coredump(HAL_rbcpr_rail_t rail, HAL_rbcpr_log_type* log_buffer);

void HAL_rbcpr_set_sw_auto_cont_nack_dn(HAL_rbcpr_rail_t rail, bool enable);

//Will return a handle to the named rail.
//Asserts internally that the rail can be found, so can't return NULL.
HAL_rbcpr_rail_t HAL_rbcpr_get_rail_handle(const char* rail_name);

void HAL_rbcpr_write_up_threshold(HAL_rbcpr_rail_t rail, uint32 error_steps);

void HAL_rbcpr_write_dn_threshold(HAL_rbcpr_rail_t rail, uint32 error_steps);

void HAL_rbcpr_write_consecutive_up(HAL_rbcpr_rail_t rail, uint32 consecutive_up);

void HAL_rbcpr_write_consecutive_dn(HAL_rbcpr_rail_t rail, uint32 consecutive_nd);

void HAL_rbcpr_set_count_mode(HAL_rbcpr_rail_t rail, HAL_rbcpr_count_mode mode);

void HAL_rbcpr_set_sw_auto_cont_ack(HAL_rbcpr_rail_t rail, bool auto_ack_enable);

void HAL_rbcpr_mask_sensor(HAL_rbcpr_rail_t rail, HAL_rbcpr_sensor_type, boolean);
uint32 HAL_rbcpr_interrupt_status(HAL_rbcpr_rail_t rail);

void HAL_rbcpr_set_clamp_timer_interval(HAL_rbcpr_rail_t rail, uint32 interval);

#endif /* HAL_RBCPR_H */

