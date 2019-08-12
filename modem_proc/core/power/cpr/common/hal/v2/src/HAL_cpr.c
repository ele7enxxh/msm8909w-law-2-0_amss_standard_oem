/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "comdef.h"
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "HAL_cpr_v2.h"
#include "HAL_cpr_hwio.h"
#include "CoreVerify.h"
#include "cpr_logging.h"
#include "cpr_voltage_ranges.h"
//#define ENABLE_HAL_CPR_LOGGING

#define HAL_CPR_RBIF_SW_VLEVEL_DEFAULT          0x20
#define HAL_CPR_RBIF_LIMIT_CEILING_DEFAULT      0x3F
#define HAL_CPR_RBIF_LIMIT_FLOOR_DEFAULT        0x0
#define HAL_CPR_STEP_QUOT_IDLE_CLOCKS_DEFAULT   0xF

void HAL_cpr_configure_hw_defaults(HAL_cpr_rail_t rail)
{
    HWIO_OUTX(rail->hw_base_address, RBIF_SW_VLEVEL, HAL_CPR_RBIF_SW_VLEVEL_DEFAULT);

    /* Write the ceiling and floor limits to the register */
    HWIO_OUTXF(rail->hw_base_address, RBIF_LIMIT, CEILING, HAL_CPR_RBIF_LIMIT_CEILING_DEFAULT);

    HWIO_OUTXF(rail->hw_base_address, RBIF_LIMIT, FLOOR, HAL_CPR_RBIF_LIMIT_FLOOR_DEFAULT);

    HWIO_OUTXF(rail->hw_base_address, RBCPR_STEP_QUOT, IDLE_CLOCKS, HAL_CPR_STEP_QUOT_IDLE_CLOCKS_DEFAULT);
}

void HAL_cpr_enable_block(HAL_cpr_rail_t rail, boolean enable)
{
#ifdef ENABLE_HAL_CPR_LOGGING
    CPR_LOGGING(1, "HAL_cpr_enable_block :%x", enable);
#endif
    if (enable)
    {
        HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, HW_TO_PMIC_EN, 0x0);
        HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, LOOP_EN , 0x1);
    }
    else
    {
        HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, LOOP_EN , 0x0);
    }
}

void HAL_cpr_next_meas_en(HAL_cpr_rail_t rail, boolean recom_taken)
{
    if (recom_taken)
    {
        /* Any value will send a pulse to the block */
        HWIO_OUTX(rail->hw_base_address, RBIF_CONT_ACK_CMD, 0x1);
    }
    else
    {
        /* Any value will send a pulse to the block */
        HWIO_OUTX(rail->hw_base_address, RBIF_CONT_NACK_CMD, 0x1);
    }
}

void HAL_cpr_set_sw_auto_cont_ack(HAL_cpr_rail_t rail, bool auto_ack_enable)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, SW_AUTO_CONT_ACK_EN, (auto_ack_enable ? 0x1 : 0x0));
}

void HAL_cpr_prgm_gcnt_target(HAL_cpr_rail_t rail, HAL_cpr_rosc_type osc, uint32 gcnt, uint32 target)
{
    uint32 val;
    uint32 *reg;

    /* For the mask we use TARGET0, GCNT0 register */
    val = (gcnt << HWIO_SHFT(RBCPR_GCNT_TARGET0, GCNT0))|target;

    /* Find the offset of the register from the first register */
    reg = (uint32 *)(HWIO_ADDRX(rail->hw_base_address, RBCPR_GCNT_TARGET0) + (osc*4));

    /* Now write the value */
    *reg = val;
}

void HAL_cpr_write_stepquot(HAL_cpr_rail_t rail, uint32 step_quot)
{
	HWIO_OUTXF(rail->hw_base_address, RBCPR_STEP_QUOT, STEP_QUOT, step_quot);
}

void HAL_cpr_write_up_threshold(HAL_cpr_rail_t rail, uint32 error_steps)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, UP_THRESHOLD, error_steps);
}

void HAL_cpr_write_dn_threshold(HAL_cpr_rail_t rail, uint32 error_steps)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, DN_THRESHOLD, error_steps);
}

void HAL_cpr_write_consecutive_up(HAL_cpr_rail_t rail, uint32 consecutive_up)
{
    HWIO_OUTXF(rail->hw_base_address, RBIF_TIMER_ADJUST, CONSECUTIVE_UP, consecutive_up);
}

void HAL_cpr_write_consecutive_dn(HAL_cpr_rail_t rail, uint32 consecutive_nd)
{
    HWIO_OUTXF(rail->hw_base_address, RBIF_TIMER_ADJUST, CONSECUTIVE_DN, consecutive_nd);
}

void HAL_cpr_poll_status_and_dont_return_until_not_busy(HAL_cpr_rail_t rail)
{
    int fail_count=0;
    do{
        do{
            do{
                fail_count++;
                CORE_VERIFY(fail_count<100);
                while (HAL_CPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
            } while (HAL_CPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
        } while (HAL_CPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
    } while (HAL_CPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
}

void HAL_cpr_poll_result(HAL_cpr_rail_t rail, HAL_cpr_result_type *cpr_result)
{
    HAL_cpr_poll_status_and_dont_return_until_not_busy(rail);

    /* After polling status */
    cpr_result->busy = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY);
    CORE_VERIFY(!cpr_result->busy);
    cpr_result->error = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, ERROR);
    cpr_result->error_steps = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, ERROR_STEPS);
    cpr_result->step_up = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, STEP_UP);
    cpr_result->step_down = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, STEP_DN);
    cpr_result->sw_decision = TRUE;
}

void HAL_cpr_mask_sensor(HAL_cpr_rail_t rail, HAL_cpr_sensor_type sensor, boolean mask)
{
    uint32 reg, bit, bmask;
    volatile uint32 *reg_addr;

    /* Compute the register to write to and bit position  */
    reg = (sensor / 32);
    bit = (sensor % 32);
    bmask = (1 << bit);

    /* Find the offset of the register from the first register */
    reg_addr = &((volatile uint32 *)HWIO_ADDRX(rail->hw_base_address, RBCPR_SENSOR_MASK0))[reg];

    /* Mask Sensor */
    if (mask)
    {
        *reg_addr |= bmask;
    }
    else /* Unmask sensor */
    {
        *reg_addr = *reg_addr&(~bmask);
    }
}

void HAL_cpr_mask_sensor_all(HAL_cpr_rail_t rail, boolean mask)
{
    /* Mask all sensorss */
    if (mask)
    {
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_MASK0, 0xFFFFFFFF);
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_MASK1, 0xFFFFFFFF);
    }
    else /* Unmask all the sensors */
    {
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_MASK0, 0x0);
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_MASK1, 0x0);
    }
}

void HAL_cpr_bypass_sensor(HAL_cpr_rail_t rail, HAL_cpr_sensor_type sensor, boolean bypass)
{
    uint8 reg, bit, bmask;
    uint32 *reg_addr;

    /* Compute the register to write to and bit position  */
    reg = (sensor / 32);
    bit = (sensor % 32);
    bmask = (1 << bit);

    /* Find the offset of the register from the first register */
    reg_addr = (uint32 *)(HWIO_ADDRX(rail->hw_base_address, RBCPR_SENSOR_BYPASS0) + reg*4);

    /* Mask Sensor */
    if (bypass)
    {
        *reg_addr |= bmask;
    }
    else /* Unmask sensor */
    {
        *reg_addr = *reg_addr &(~bmask);
    }
}

void HAL_cpr_bypass_sensor_all(HAL_cpr_rail_t rail, boolean bypass)
{
    /* Bypass all sensorss */
    if (bypass)
    {
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_BYPASS0, 0xFFFFFFFF);
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_BYPASS1, 0xFFFFFFFF);
    }
    else /* Bypass none of the sensors */
    {
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_BYPASS0, 0x0);
        HWIO_OUTX(rail->hw_base_address, RBCPR_SENSOR_BYPASS1, 0x0);
    }
}

#define HAL_CPR_TIMER_DEFAULT_INTERVAL  (5 * 19200)   //5ms * 19.2MHz

void HAL_config_timers(HAL_cpr_rail_t rail)
{
    /* Program the timer */
    HWIO_OUTX(rail->hw_base_address, RBCPR_TIMER_INTERVAL, HAL_CPR_TIMER_DEFAULT_INTERVAL);
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, TIMER_EN, 0x1);    //Enable the timer.
}

void HAL_cpr_enable_int(HAL_cpr_rail_t rail, uint32 interrupt_mask, uint8 enable)
{
    /* Find the offset of the register from the first register */
    // for now we only use interrupt 0.
    volatile uint32 *reg_addr = (volatile uint32 *)(HWIO_ADDRX(rail->hw_base_address, RBIF_IRQ_EN_0));
    uint32 reg = *reg_addr;

    if(enable)
    {
        reg |= interrupt_mask;
    }
    else
    {
        reg &= ~interrupt_mask;
    }

    /* Now write to the register */
    *reg_addr = reg;
}

void HAL_cpr_clear_int(HAL_cpr_rail_t rail, boolean enable)
{
    uint8 bitmask = 0, status;

    /* Read the interrupt status register */
    status = HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS);
    /* Zero out the mask to clear the set bits. Might need to revisit as this is pointless */
    bitmask = status ^ status ;
    /* Now write to the register */
    HWIO_OUTX(rail->hw_base_address, RBIF_IRQ_CLEAR, bitmask);
}

void HAL_cpr_clear_int_all(HAL_cpr_rail_t rail)
{
    /* We want to clear all the interrupts. So write 0x7F */
    HWIO_OUTX(rail->hw_base_address, RBIF_IRQ_CLEAR, ((1 << 7) - 1));
}

void HAL_cpr_set_sw_auto_cont_nack_dn(HAL_cpr_rail_t rail, bool enable)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, SW_AUTO_CONT_NACK_DN_EN, (enable ? 0x1 : 0));
}

void HAL_cpr_set_count_mode(HAL_cpr_rail_t rail, HAL_cpr_count_mode mode)
{
    switch(mode)
    {
        case HAL_CPR_ALL_AT_ONCE:
            HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, COUNT_MODE , 0x0);
            break;
        case HAL_CPR_STAGGERED:
            HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, COUNT_MODE , 0x1);
            break;
        default:
            CORE_VERIFY(0);
    };
}

void HAL_cpr_save_hw_state(HAL_cpr_rail_t rail, HAL_cpr_dump_t core_dump)
{
    core_dump->cpr_timer_interval = HWIO_INX(rail->hw_base_address, RBCPR_TIMER_INTERVAL);
    core_dump->rbif_timer_adjust = HWIO_INX(rail->hw_base_address, RBIF_TIMER_ADJUST);
    core_dump->cpr_ctl = HWIO_INX(rail->hw_base_address, RBCPR_CTL);
    core_dump->cpr_result_0 = HWIO_INX(rail->hw_base_address, RBCPR_RESULT_0);
    core_dump->cpr_result_1 = HWIO_INX(rail->hw_base_address, RBCPR_RESULT_1);
    core_dump->rbif_irq_en_0 = HWIO_INX(rail->hw_base_address, RBIF_IRQ_EN_0);
    core_dump->rbif_irq_status = HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS);
    core_dump->cpr_debug0 = HWIO_INX(rail->hw_base_address, RBCPR_DEBUG0);
    core_dump->cpr_debug1 = HWIO_INX(rail->hw_base_address, RBCPR_DEBUG1);
    core_dump->cpr_debug2 = HWIO_INX(rail->hw_base_address, RBCPR_DEBUG2);
    core_dump->cpr_log_0 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_0);
    core_dump->cpr_log_1 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_1);
    core_dump->cpr_log_2 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_2);
    core_dump->cpr_log_3 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_3);
    core_dump->cpr_log_4 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_4);
    core_dump->cpr_log_5 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_5);
    core_dump->cpr_log_6 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_6);
    core_dump->cpr_log_7 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_7);
}

uint32 HAL_cpr_interrupt_status(HAL_cpr_rail_t rail)
{
    return HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS);
}

void HAL_cpr_set_clamp_timer_interval(HAL_cpr_rail_t rail, uint32 interval)
{
    HWIO_OUTXF(rail->hw_base_address, RBIF_TIMER_ADJUST, CLAMP_TIMER_INTERVAL , interval);
}

HAL_cpr_dump_t HAL_cpr_alloc_cpr_dump_buffer(void)
{
    HAL_cpr_dump_t dump_buffer = (HAL_cpr_dump_t)malloc(sizeof(HAL_cpr_dump_s));
    CORE_VERIFY_PTR(dump_buffer);
    memset(dump_buffer, 0, sizeof(HAL_cpr_dump_s));
    return dump_buffer;
}

void HAL_cpr_set_age_mode(HAL_cpr_rail_t rail, bool enable)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_HTOL_AGE, AGE_PAGE, enable);
}

uint32 HAL_cpr_age_data_valid(HAL_cpr_rail_t rail)
{
    return HWIO_INXF(rail->hw_base_address, RBCPR_HTOL_AGE, CPR_AGE_DATA_STATUS);
}

uint8 HAL_cpr_get_cpr_rev(void)
{
   return (CPR_HWIO_IN_MASK(cpr_bsp_misc_info_config.cpr_rev_fuse.fuse_address, cpr_bsp_misc_info_config.cpr_rev_fuse.mask) >> cpr_bsp_misc_info_config.cpr_rev_fuse.offset);
}