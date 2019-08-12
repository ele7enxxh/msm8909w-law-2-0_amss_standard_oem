/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifndef HAL_CPR_V2_H
#define HAL_CPR_V2_H

#include <stdbool.h>
#include "HAL_cpr.h"
#include "HAL_cpr_hwio.h"

typedef uint8 HAL_cpr_rosc_type;
typedef uint8 HAL_cpr_sensor_type;

typedef enum
{
  HAL_CPR_INT_DONE_FLAG = HWIO_CPR_IRQ_EN_t_CPR_DONE_EN_BMSK,
  HAL_CPR_INT_DOWN_FLAG = HWIO_CPR_IRQ_EN_t_DOWN_FLAG_EN_BMSK,
  HAL_CPR_INT_MID_FLAG  = HWIO_CPR_IRQ_EN_t_MID_FLAG_EN_BMSK,
  HAL_CPR_INT_UP_FLAG   = HWIO_CPR_IRQ_EN_t_UP_FLAG_EN_BMSK,
} HAL_cpr_interrupt_src_type;

typedef enum
{
  HAL_CPR_QUOT_SLOW,
  HAL_CPR_QUOT_FAST,
} HAL_cpr_meas_type;

typedef enum
{
  HAL_CPR_ALL_AT_ONCE,
  HAL_CPR_STAGGERED,
} HAL_cpr_count_mode;

typedef enum
{
  HAL_CPR_IDLE,
  HAL_CPR_BUSY,
} HAL_cpr_status;

typedef struct
{
  boolean busy;
  uint16  error;
  uint8   error_steps;
  boolean step_up;
  boolean step_down;
  boolean sw_decision;
} HAL_cpr_result_type;

typedef struct HAL_cpr_dump_s
{
    uint32 cpr_timer_interval;
    uint32 rbif_timer_adjust;
    uint32 cpr_ctl;
    uint32 cpr_result_0;
    uint32 cpr_result_1;
    uint32 rbif_irq_en_0;
    uint32 rbif_irq_status;
    uint32 cpr_debug0;
    uint32 cpr_debug1;
    uint32 cpr_debug2;
    uint32 cpr_log_0;
    uint32 cpr_log_1;
    uint32 cpr_log_2;
    uint32 cpr_log_3;
    uint32 cpr_log_4;
    uint32 cpr_log_5;
    uint32 cpr_log_6;
    uint32 cpr_log_7;
} HAL_cpr_dump_s;


/* ============================================
FUNCTION HAL_cpr_enable_int

DESCRIPTION
  HAL layer helper function to configure a particular
  top level interrupt with the config provided.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_cpr_enable_int(HAL_cpr_rail_t rail, uint32 interrupt_mask, uint8 enable);
/* ============================================
FUNCTION HAL_CPR_PRGM_GCNT_TARGET

DESCRIPTION
  HAL layer function to program gate count and target
  values for a particular ring oscillator.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_cpr_prgm_gcnt_target(HAL_cpr_rail_t rail, HAL_cpr_rosc_type osc, uint32 gcnt, uint32 target);

/* ============================================
FUNCTION HAL_CPR_ENABLE_MEAS

DESCRIPTION
  HAL layer helper function to enable measurements

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_cpr_enable_meas(HAL_cpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_CPR_READ_STATUS

DESCRIPTION
  HAL layer helper function to read the status of the measurement

RETURN VALUE
  Returns the status of the measurement

SIDE EFFECTS
  None
 */

uint8 HAL_cpr_read_status(HAL_cpr_rail_t rail);

/* ============================================
FUNCTION HAL_CPR_POLL_RESULT

DESCRIPTION
  HAL layer helper function to read the result of a measurement

RETURN VALUE
  Returns the result of the measurement

SIDE EFFECTS
  None
 */

void HAL_cpr_poll_result(HAL_cpr_rail_t rail, HAL_cpr_result_type *);


/* ============================================
FUNCTION HAL_CPR_WRITE_STEPQUOT

DESCRIPTION
  HAL layer function to write the stepquot value

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_cpr_write_stepquot(HAL_cpr_rail_t rail, uint32 stepquot);

/* ============================================
FUNCTION HAL_CPR_ENABLE_SENSOR

DESCRIPTION
  HAL layer function to enable/disable a sensor

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_cpr_enable_sensor(HAL_cpr_rail_t rail, HAL_cpr_sensor_type, boolean);

/* ============================================
FUNCTION HAL_CPR_BYPASS_SENSOR

DESCRIPTION
  HAL layer function to bypass/pass through a sensor

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_cpr_bypass_sensor(HAL_cpr_rail_t rail, HAL_cpr_sensor_type, boolean);

/* ============================================
FUNCTION HAL_CPR_CLEAR_INT_ALL

DESCRIPTION
  HAL layer function to clear all interrupts that are set

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_cpr_clear_int_all(HAL_cpr_rail_t rail);

/* ============================================
FUNCTION HAL_CPR_NEXT_MEAS_EN

DESCRIPTION
  HAL layer function to tell the CPR block that the
  recommendation was taken / not taken.

RETURN VALUE
  None

SIDE EFFECTS
  None
 */

void HAL_cpr_next_meas_en(HAL_cpr_rail_t rail, boolean);

/* ============================================
FUNCTION HAL_CPR_ENABLE_BLOCK

DESCRIPTION
  HAL layer function to enable / disable the block

RETURN VALUE
  None

SIDE EFFECTS
  None
 */
void HAL_cpr_enable_block(HAL_cpr_rail_t rail, boolean);

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
void HAL_config_vlimits(HAL_cpr_rail_t rail);

/* ============================================
FUNCTION HAL_CPR_CONFIGURE_STEP_DN

DESCRIPTION
  Hack: HAL layer function to configure interrupts and SWLEVEL after first voltage switch

RETURN VALUE
  None

SIDE EFFECTS
  None
*/
void HAL_cpr_configure_step_dn(HAL_cpr_rail_t rail);

/* ============================================
FUNCTION HAL_CPR_CLOCK_ENABLE

DESCRIPTION
  HAL layer function to enable the clocks for the CPR block

RETURN VALUE
  None

SIDE EFFECTS
  None
*/
void HAL_cpr_clock_enable(HAL_cpr_rail_t rail);

void HAL_cpr_set_sw_auto_cont_nack_dn(HAL_cpr_rail_t rail, bool enable);

void HAL_cpr_write_up_threshold(HAL_cpr_rail_t rail, uint32 error_steps);

void HAL_cpr_write_dn_threshold(HAL_cpr_rail_t rail, uint32 error_steps);

void HAL_cpr_write_consecutive_up(HAL_cpr_rail_t rail, uint32 consecutive_up);

void HAL_cpr_write_consecutive_dn(HAL_cpr_rail_t rail, uint32 consecutive_nd);

void HAL_cpr_set_count_mode(HAL_cpr_rail_t rail, HAL_cpr_count_mode mode);

void HAL_cpr_set_sw_auto_cont_ack(HAL_cpr_rail_t rail, bool auto_ack_enable);

uint32 HAL_cpr_interrupt_status(HAL_cpr_rail_t rail);

void HAL_cpr_set_clamp_timer_interval(HAL_cpr_rail_t rail, uint32 interval);

void HAL_cpr_assign_sensor(HAL_cpr_rail_t rail, HAL_cpr_sensor_type sensor);

//Bucket list of all things we need to configure for the HW.
void HAL_cpr_configure_hw_defaults(HAL_cpr_rail_t rail);

void HAL_cpr_set_age_mode(HAL_cpr_rail_t rail, bool enable);

uint32 HAL_cpr_age_data_valid(HAL_cpr_rail_t rail);

#endif /* HAL_CPR_V2_H */

