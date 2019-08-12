/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdbool.h>
#include <string.h>

#include "comdef.h"
#include "HALhwio.h"
#include "HAL_rbcpr.h"
#include "HAL_rbcpr_hwio.h"
#include "err.h"
#include "timetick.h"
#include "CoreVerify.h"

extern const HAL_rbcpr_bsp_type HAL_rbcpr_bsp_data;

void HAL_rbcpr_enable_block(HAL_rbcpr_rail_t rail, boolean enable)
{
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

void HAL_rbcpr_next_meas_en(HAL_rbcpr_rail_t rail, boolean recom_taken)
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

void HAL_rbcpr_set_sw_auto_cont_ack(HAL_rbcpr_rail_t rail, bool auto_ack_enable)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, SW_AUTO_CONT_ACK_EN, (auto_ack_enable ? 0x1 : 0x0));
}

void HAL_rbcpr_prgm_gcnt_target(HAL_rbcpr_rail_t rail, HAL_rbcpr_rosc_type osc, uint32 gcnt, uint32 target)
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

void HAL_rbcpr_write_stepquot(HAL_rbcpr_rail_t rail, uint32 step_quot, uint32 idle_clocks)
{
	HWIO_OUTXF(rail->hw_base_address, RBCPR_STEP_QUOT, STEP_QUOT, step_quot);
    HWIO_OUTXF(rail->hw_base_address, RBCPR_STEP_QUOT, IDLE_CLOCKS, idle_clocks);
}

void HAL_rbcpr_write_up_threshold(HAL_rbcpr_rail_t rail, uint32 error_steps)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, UP_THRESHOLD, error_steps);
}

void HAL_rbcpr_write_dn_threshold(HAL_rbcpr_rail_t rail, uint32 error_steps)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, DN_THRESHOLD, error_steps);
}

void HAL_rbcpr_write_consecutive_up(HAL_rbcpr_rail_t rail, uint32 consecutive_up)
{
    HWIO_OUTXF(rail->hw_base_address, RBIF_TIMER_ADJUST, CONSECUTIVE_UP, consecutive_up);
}

void HAL_rbcpr_write_consecutive_dn(HAL_rbcpr_rail_t rail, uint32 consecutive_nd)
{
    HWIO_OUTXF(rail->hw_base_address, RBIF_TIMER_ADJUST, CONSECUTIVE_DN, consecutive_nd);
}

void HAL_rbcpr_poll_status_and_dont_return_until_not_busy(HAL_rbcpr_rail_t rail)
{
    int fail_count=0;
    do{
        do{
            do{
                fail_count++;
                assert(fail_count<100);
                while (HAL_RBCPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
            } while (HAL_RBCPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
        } while (HAL_RBCPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
    } while (HAL_RBCPR_IDLE!=HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY));
}

uint32 HAL_rbcpr_read_quot(HAL_rbcpr_rail_t rail, HAL_rbcpr_meas_type measure)
{
    uint32 val;

    /* Poll until the value is ready */
    HAL_rbcpr_poll_status_and_dont_return_until_not_busy(rail);

    switch (measure)
    {
        case HAL_RBCPR_QUOT_SLOW:
            val = HWIO_INXF(rail->hw_base_address, RBCPR_DEBUG1, QUOT_SLOW);
            break;

        case HAL_RBCPR_QUOT_FAST:
            val = HWIO_INXF(rail->hw_base_address, RBCPR_DEBUG1, QUOT_FAST);
            break;

        default:
            val = 0x0;
            break;
    }

    return val;
}

//To do - rework this, get it out of the HAL I think.
void HAL_rbcpr_save_log(HAL_rbcpr_rail_t rail, HAL_rbcpr_log_type* log_buffer, railway_corner corner)
{
    uint8 index = log_buffer->rbcpr_log_index;
   
    log_buffer->rbcpr_logs[index].corner = corner;
    log_buffer->rbcpr_logs[index].timestamp = (uint32)timetick_get_safe();
    log_buffer->rbcpr_logs[index].log_0 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_0);
    log_buffer->rbcpr_logs[index].rbcpr_result = HWIO_INX(rail->hw_base_address, RBCPR_RESULT_0);

    log_buffer->rbcpr_log_index = (log_buffer->rbcpr_log_index + 1)%HAL_RBCPR_LOG_SIZE;
}

void HAL_rbcpr_poll_result(HAL_rbcpr_rail_t rail, HAL_rbcpr_result_type *rbcpr_result)
{
    HAL_rbcpr_poll_status_and_dont_return_until_not_busy(rail);

    /* After polling status */
    rbcpr_result->busy = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, BUSY);
    assert(!rbcpr_result->busy);
    rbcpr_result->error = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, ERROR);
    rbcpr_result->error_steps = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, ERROR_STEPS);
    rbcpr_result->step_up = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, STEP_UP);
    rbcpr_result->step_down = HWIO_INXF(rail->hw_base_address, RBCPR_RESULT_0, STEP_DN);
    rbcpr_result->sw_decision = TRUE;
}

void HAL_rbcpr_mask_sensor(HAL_rbcpr_rail_t rail, HAL_rbcpr_sensor_type sensor, boolean mask)
{
    uint32 reg, bit, bmask;
    volatile uint32 *reg_addr;

    /* Enable that particular sensor */
    if (sensor >= rail->sensors)
        ERR_FATAL("",0, 0, 0);

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

void HAL_rbcpr_mask_sensor_all(HAL_rbcpr_rail_t rail, boolean mask)
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

void HAL_rbcpr_bypass_sensor(HAL_rbcpr_rail_t rail, HAL_rbcpr_sensor_type sensor, boolean bypass)
{
    uint32 reg, bit, bmask;
    uint32 *reg_addr;

    /* Enable that particular sensor */
    if (sensor >= rail->sensors)
        ERR_FATAL("",0, 0, 0);

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

void HAL_rbcpr_bypass_sensor_all(HAL_rbcpr_rail_t rail, boolean bypass)
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

void HAL_config_timers(HAL_rbcpr_rail_t rail)
{
    // Convert the time in msecs to clock cycles
    // Assume clock is 19.2 MHz XO clock */
    uint32 interval = rail->timer_config.interval * 19200;
    HAL_rbcpr_pgm_timer(rail, interval, rail->timer_config.enable);
}

void HAL_rbcpr_pgm_timer(HAL_rbcpr_rail_t rail, uint32 interval, boolean enable)
{
    /* Program the timer */
    HWIO_OUTX(rail->hw_base_address, RBCPR_TIMER_INTERVAL, interval);

    /* Enable the timer */
    if (enable)
    {
        HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, TIMER_EN, 0x1);
    }
    else
    {
        HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, TIMER_EN, 0x0);
    }
}

void HAL_rbcpr_enable_int(HAL_rbcpr_rail_t rail, uint32 interrupt_mask, uint8 enable)
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

void HAL_rbcpr_clear_int(HAL_rbcpr_rail_t rail, boolean enable)
{
    uint8 bitmask = 0, status;

    /* Read the interrupt status register */
    status = HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS);
    /* Zero out the mask to clear the set bits. Might need to revisit as this is pointless */
    bitmask = status ^ status ;
    /* Now write to the register */
    HWIO_OUTX(rail->hw_base_address, RBIF_IRQ_CLEAR, bitmask);
}

void HAL_rbcpr_clear_int_all(HAL_rbcpr_rail_t rail)
{
    /* We want to clear all the interrupts. So write 0x7F */
    HWIO_OUTX(rail->hw_base_address, RBIF_IRQ_CLEAR, ((1 << 7) - 1));
}

void HAL_config_vlimits(HAL_rbcpr_rail_t rail)
{
    /* Write to the SW_VLEVEL register as default values don't work */
    HWIO_OUTX(rail->hw_base_address, RBIF_SW_VLEVEL, rail->settings.vlevel);

    /* Write the ceiling and floor limits to the register */
    HWIO_OUTX(rail->hw_base_address, RBIF_LIMIT, rail->settings.ceiling<<HWIO_SHFT(RBIF_LIMIT, CEILING)|rail->settings.floor);
}

void HAL_rbcpr_set_sw_auto_cont_nack_dn(HAL_rbcpr_rail_t rail, bool enable)
{
    HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, SW_AUTO_CONT_NACK_DN_EN, (enable ? 0x1 : 0));
}

void HAL_rbcpr_collect_coredump(HAL_rbcpr_rail_t rail, HAL_rbcpr_log_type* log_buffer)
{
    uint8 i, index;
    HAL_rbcpr_coredump_type *coredump;

    index = log_buffer->coredump_index;
    coredump = &log_buffer->rbcpr_coredump[index];

    coredump->limit = HWIO_INX(rail->hw_base_address, RBIF_LIMIT);
    coredump->step_quot = HWIO_INX(rail->hw_base_address, RBCPR_STEP_QUOT);

    for(i=0; i<8; i++)
        coredump->gcnt_target[i] = *((uint32 *)(HWIO_ADDRX(rail->hw_base_address, RBCPR_GCNT_TARGET0) + i*4));
    coredump->irq0 = HWIO_INX(rail->hw_base_address, RBIF_IRQ_EN_0);
    coredump->irq_status = HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS);

    index ^= 0x1;
    log_buffer->coredump_index = index;
}

HAL_rbcpr_rail_t HAL_rbcpr_get_rail_handle(const char* rail_name)
{
    const HAL_rbcpr_rail_s *rail_data = NULL;
    for(int i=0; i<HAL_rbcpr_bsp_data.num_rails; i++)
    {
        rail_data = &HAL_rbcpr_bsp_data.rails[i];
        if(!strcmp(rail_name, rail_data->name))
            return rail_data;
    }
    assert(0);
    return NULL;    //Can't actually get here
}

void HAL_rbcpr_set_count_mode(HAL_rbcpr_rail_t rail, HAL_rbcpr_count_mode mode)
{
    switch(mode)
    {
        case HAL_RBCPR_ALL_AT_ONCE:
            HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, COUNT_MODE , 0x0);
            break;
        case HAL_RBCPR_STAGGERED:
            HWIO_OUTXF(rail->hw_base_address, RBCPR_CTL, COUNT_MODE , 0x1);
            break;
        default:
            abort();
    };
}

void HAL_rbcpr_save_hw_state(HAL_rbcpr_rail_t rail)
{
    HAL_rbcpr_dump* dump = (HAL_rbcpr_dump*)&rail->core_dump;
    dump->rbcpr_timer_interval = HWIO_INX(rail->hw_base_address, RBCPR_TIMER_INTERVAL);
    dump->rbif_timer_adjust = HWIO_INX(rail->hw_base_address, RBIF_TIMER_ADJUST);
    dump->rbcpr_ctl = HWIO_INX(rail->hw_base_address, RBCPR_CTL);
    dump->rbcpr_result_0 = HWIO_INX(rail->hw_base_address, RBCPR_RESULT_0);
    dump->rbcpr_result_1 = HWIO_INX(rail->hw_base_address, RBCPR_RESULT_1);
    dump->rbif_irq_en_0 = HWIO_INX(rail->hw_base_address, RBIF_IRQ_EN_0);
    dump->rbif_irq_status = HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS);
    dump->rbcpr_debug0 = HWIO_INX(rail->hw_base_address, RBCPR_DEBUG0);
    dump->rbcpr_debug1 = HWIO_INX(rail->hw_base_address, RBCPR_DEBUG1);
    dump->rbcpr_debug2 = HWIO_INX(rail->hw_base_address, RBCPR_DEBUG2);
    dump->rbcpr_log_0 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_0);
    dump->rbcpr_log_1 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_1);
    dump->rbcpr_log_2 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_2);
    dump->rbcpr_log_3 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_3);
    dump->rbcpr_log_4 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_4);
    dump->rbcpr_log_5 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_5);
    dump->rbcpr_log_6 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_6);
    dump->rbcpr_log_7 = HWIO_INX(rail->hw_base_address, RBCPR_LOG_7);
}

uint32 HAL_rbcpr_interrupt_status(HAL_rbcpr_rail_t rail)
{
    return HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS);
}

void HAL_rbcpr_set_clamp_timer_interval(HAL_rbcpr_rail_t rail, uint32 interval)
{
    HWIO_OUTXF(rail->hw_base_address, RBIF_TIMER_ADJUST, CLAMP_TIMER_INTERVAL , interval);
}
