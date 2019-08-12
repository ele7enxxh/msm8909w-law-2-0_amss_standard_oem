/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "comdef.h"
#include "HAL_cpr_v3.h"
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "HAL_cpr_hwio.h"
#include "CoreVerify.h"
#include "cpr_logging.h"

#define HAL_CPR_TIMER_DEFAULT_INTERVAL  (5 * 19200)   //5ms * 19.2MHz

//#define ENABLE_HAL_CPR_LOGGING

void HAL_cpr_configure_hw_defaults(HAL_cpr_rail_t rail)
{
    //CPR_HWIO_OUT_FIELD(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(rail->hw_base_address), HWIO_CPR_BIST_CHAIN_CHECK0_SCLK_CNT_EXPECTED, 32*6);
    CPR_HWIO_OUT( HWIO_CPR_TIMER_AUTO_CONT_ADDR( rail->hw_base_address ), HAL_CPR_TIMER_DEFAULT_INTERVAL );
    CPR_HWIO_OUT_FIELD( HWIO_CPR_FSM_CTL_ADDR( rail->hw_base_address ), HWIO_CPR_FSM_CTL_IDLE_CLOCKS, 0x1f );
}

void HAL_cpr_enable_block(HAL_cpr_rail_t rail, boolean enable)
{
#ifdef ENABLE_HAL_CPR_LOGGING
    CPR_LOGGING(1, "HAL_cpr_enable_block :%x", enable);
#endif

    if(enable) {
        CPR_HWIO_OUT_FIELD( HWIO_CPR_FSM_CTL_ADDR( rail->hw_base_address ), HWIO_CPR_FSM_CTL_LOOP_EN, TRUE );
    } else {   //TODO: Turning off Loop En fixes an issue on 8996 MSS but may not work on systems with more than one CPR thread.
        CPR_HWIO_OUT_FIELD( HWIO_CPR_FSM_CTL_ADDR( rail->hw_base_address ), HWIO_CPR_FSM_CTL_LOOP_EN, FALSE );
    }
    
    CPR_HWIO_OUT_FIELD( HWIO_CPR_MASK_THREAD_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_MASK_THREAD_t_DISABLE_THREAD, !enable );
}


void HAL_cpr_next_meas_en(HAL_cpr_rail_t rail, boolean recom_taken)
{
    CPR_HWIO_OUT( HWIO_CPR_CONT_CMD_t_ADDR( rail->hw_base_address, rail->thread ), recom_taken );
}


void HAL_cpr_prgm_gcnt_target(HAL_cpr_rail_t rail, HAL_cpr_rosc_type osc, uint32 gcnt, uint32 target)
{
    uint32 targetOffset = (HWIO_CPR_TARGET1_t_m_ADDR( 0, 0, 0 ) - HWIO_CPR_TARGET0_t_m_ADDR( 0, 0, 0 )) * osc;
    uint32 gcntOffset = (HWIO_CPR_GCNT1_ADDR( 0 ) - HWIO_CPR_GCNT0_ADDR( 0 )) * osc;
    
#ifdef ENABLE_HAL_CPR_LOGGING
    CPR_LOGGING(3, "HAL_cpr_prgm_gcnt_target: target:%x  gcnt:%x osc:%x", target, gcnt, osc);
#endif
    
    
    // Mask off RO for this rail if target isn't set
    if(target == 0)
    {
        // RO_MASK4THREAD
        CPR_HWIO_OUT_SET( HWIO_CPR_MASK_THREAD_t_ADDR( rail->hw_base_address, rail->thread ), 1 << osc );
    }
    else
    {
        // TODO: Hardcoded to mode 0 (default)
        CPR_HWIO_OUT_FIELD( HWIO_CPR_GCNT0_ADDR( rail->hw_base_address ) + gcntOffset, HWIO_CPR_GCNT0_GCNT0, gcnt );
        CPR_HWIO_OUT_FIELD( HWIO_CPR_TARGET0_t_m_ADDR( rail->hw_base_address, rail->thread, 0 ) + targetOffset,
                            HWIO_CPR_TARGET0_t_m_TARGET0, target );
    }
}

void HAL_cpr_write_stepquot(HAL_cpr_rail_t rail, uint32 step_quot)
{
    CPR_HWIO_OUT_FIELD( HWIO_CPR_STEP_QUOT_INIT_ADDR( rail->hw_base_address ), HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MAX, step_quot );
    CPR_HWIO_OUT_FIELD( HWIO_CPR_STEP_QUOT_INIT_ADDR( rail->hw_base_address ), HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MIN, step_quot );
}

void HAL_cpr_write_up_threshold(HAL_cpr_rail_t rail, uint32 error_steps)
{
    CPR_HWIO_OUT_FIELD( HWIO_CPR_THRESHOLD_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_THRESHOLD_t_UP_THRESHOLD, error_steps );
}

void HAL_cpr_write_dn_threshold(HAL_cpr_rail_t rail, uint32 error_steps)
{
    CPR_HWIO_OUT_FIELD( HWIO_CPR_THRESHOLD_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_THRESHOLD_t_DN_THRESHOLD, error_steps );
}

void HAL_cpr_write_consecutive_up(HAL_cpr_rail_t rail, uint32 consecutive_up)
{
    CPR_HWIO_OUT_FIELD( HWIO_CPR_THRESHOLD_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_THRESHOLD_t_CONSECUTIVE_UP, consecutive_up );
}

void HAL_cpr_write_consecutive_dn(HAL_cpr_rail_t rail, uint32 consecutive_nd)
{
    CPR_HWIO_OUT_FIELD( HWIO_CPR_THRESHOLD_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_THRESHOLD_t_CONSECUTIVE_DN, consecutive_nd );
}

void HAL_cpr_poll_status_and_dont_return_until_not_busy(HAL_cpr_rail_t rail)
{
    int fail_count = 0;

    // maybe just sleep for a few us?    
    do {
        do {
            do {
                fail_count++;
                CORE_VERIFY( fail_count < 100 );
                while(CPR_HWIO_IN_FIELD( HWIO_CPR_RESULT0_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_RESULT0_t_BUSY ));
            } while(CPR_HWIO_IN_FIELD( HWIO_CPR_RESULT0_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_RESULT0_t_BUSY ));
        } while(CPR_HWIO_IN_FIELD( HWIO_CPR_RESULT0_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_RESULT0_t_BUSY ));
    } while(CPR_HWIO_IN_FIELD( HWIO_CPR_RESULT0_t_ADDR( rail->hw_base_address, rail->thread ), HWIO_CPR_RESULT0_t_BUSY ));
}

void HAL_cpr_poll_result(HAL_cpr_rail_t rail, HAL_cpr_result_type *cpr_result)
{
    HAL_cpr_poll_status_and_dont_return_until_not_busy( rail );

    /* After polling status */
    uint32 val = CPR_HWIO_IN( HWIO_CPR_RESULT0_t_ADDR( rail->hw_base_address, rail->thread ) );

    cpr_result->busy = CPR_HWIO_GET_FIELD_VALUE( val, HWIO_CPR_RESULT0_t_BUSY );
    cpr_result->error = CPR_HWIO_GET_FIELD_VALUE( val, HWIO_CPR_RESULT0_t_ERROR );
    cpr_result->error_steps = CPR_HWIO_GET_FIELD_VALUE( val, HWIO_CPR_RESULT0_t_ERROR_STEPS );
    cpr_result->step_up = CPR_HWIO_GET_FIELD_VALUE( val, HWIO_CPR_RESULT0_t_STEP_UP );
    cpr_result->step_down = CPR_HWIO_GET_FIELD_VALUE( val, HWIO_CPR_RESULT0_t_STEP_DN );
    cpr_result->sw_decision = TRUE;

    CORE_VERIFY( !cpr_result->busy );
}

void HAL_cpr_assign_sensor(HAL_cpr_rail_t rail, HAL_cpr_sensor_type sensor)
{
    CPR_HWIO_OUT( HWIO_CPR_SENSOR_THREAD_n_ADDR( rail->hw_base_address, sensor ), rail->thread );
}

void HAL_cpr_bypass_sensor(HAL_cpr_rail_t rail, HAL_cpr_sensor_type sensor, boolean bypass)
{
    /* Compute the register to write to and bit position  */
    uint32 reg = (sensor / 32);
    uint32 bit = (sensor % 32);
    uint32 bmask = (1 << bit);

    uint32 val = CPR_HWIO_IN( HWIO_CPR_SENSOR_BYPASS_READn_ADDR( rail->hw_base_address, reg ) );

    /* Mask Sensor */
    if(bypass) {
        val |= bmask;
    }
    else /* Unmask sensor */ {
        val &= ~bmask;
    }
    
    CPR_HWIO_OUT( HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR( rail->hw_base_address, reg ), val );
}

void HAL_cpr_enable_int(HAL_cpr_rail_t rail, uint32 interrupt_mask, uint8 enable)
{
    if(enable) {
        CPR_HWIO_OUT_SET( HWIO_CPR_IRQ_EN_t_ADDR( rail->hw_base_address, rail->thread ), interrupt_mask );
    }
    else {
        CPR_HWIO_OUT_CLEAR( HWIO_CPR_IRQ_EN_t_ADDR( rail->hw_base_address, rail->thread ), interrupt_mask );
    }
}

void HAL_cpr_clear_int_all(HAL_cpr_rail_t rail)
{
    CPR_HWIO_OUT( HWIO_CPR_IRQ_CLEAR_t_ADDR( rail->hw_base_address, rail->thread ), ~0 );
}

void HAL_cpr_set_count_mode(HAL_cpr_rail_t rail, HAL_cpr_count_mode mode)
{
    switch(mode)
    {
        case HAL_CPR_ALL_AT_ONCE:
            CPR_HWIO_OUT_FIELD( HWIO_CPR_FSM_CTL_ADDR( rail->hw_base_address ), HWIO_CPR_FSM_CTL_COUNT_MODE, 0 );
            
            // SW can program value 1 into this CSR in order to mimic Rev2 legacy ALL_AT_ONCE mode
            CPR_HWIO_OUT_FIELD( HWIO_CPR_FSM_CTL_ADDR( rail->hw_base_address ), HWIO_CPR_FSM_CTL_COUNT_REPEAT, 1 );
            break;
        case HAL_CPR_STAGGERED:
            CPR_HWIO_OUT_FIELD( HWIO_CPR_FSM_CTL_ADDR( rail->hw_base_address ), HWIO_CPR_FSM_CTL_COUNT_MODE, 1 );
            break;
        default:
            CORE_VERIFY( 0 );
    };
}

void HAL_cpr_save_hw_state(HAL_cpr_rail_t rail, HAL_cpr_dump_t core_dump)
{
    core_dump->cpr_timer_interval = CPR_HWIO_IN( HWIO_CPR_TIMER_AUTO_CONT_ADDR( rail->hw_base_address ) );
    core_dump->cpr_ctl = CPR_HWIO_IN( HWIO_CPR_FSM_CTL_ADDR( rail->hw_base_address ) );
    core_dump->cpr_result_0 = CPR_HWIO_IN( HWIO_CPR_RESULT0_t_ADDR( rail->hw_base_address, rail->thread ) );
    core_dump->cpr_result_1 = CPR_HWIO_IN( HWIO_CPR_RESULT1_t_ADDR( rail->hw_base_address, rail->thread ) );
    core_dump->rbif_irq_en_0 = CPR_HWIO_IN( HWIO_CPR_IRQ_EN_t_ADDR( rail->hw_base_address, rail->thread ) );
    core_dump->rbif_irq_status = CPR_HWIO_IN( HWIO_CPR_IRQ_STATUS_t_ADDR( rail->hw_base_address, rail->thread ) );
    core_dump->cpr_debug0 = CPR_HWIO_IN( HWIO_CPR_DEBUG_ADDR( rail->hw_base_address ) );
    core_dump->cpr_log_0 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 0 ) );
    core_dump->cpr_log_1 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 1 ) );
    core_dump->cpr_log_2 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 2 ) );
    core_dump->cpr_log_3 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 3 ) );
    core_dump->cpr_log_4 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 4 ) );
    core_dump->cpr_log_5 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 5 ) );
    core_dump->cpr_log_6 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 6 ) );
    core_dump->cpr_log_7 = CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR( rail->hw_base_address, 7 ) );
}

uint32 HAL_cpr_interrupt_status(HAL_cpr_rail_t rail)
{
    return CPR_HWIO_IN( HWIO_CPR_IRQ_STATUS_t_ADDR( rail->hw_base_address, rail->thread ) );
}

void HAL_cpr_set_clamp_timer_interval(HAL_cpr_rail_t rail, uint32 interval)
{
    CPR_HWIO_OUT_FIELD( HWIO_CPR_TIMER_CLAMP_ADDR( rail->hw_base_address ), HWIO_CPR_TIMER_CLAMP_CLAMP_TIMER_INTERVAL, interval );
}

HAL_cpr_dump_t HAL_cpr_alloc_cpr_dump_buffer(void)
{
    HAL_cpr_dump_t dump_buffer = (HAL_cpr_dump_t) malloc( sizeof (HAL_cpr_dump_s) );
    CORE_VERIFY_PTR( dump_buffer );
    memset( dump_buffer, 0, sizeof (HAL_cpr_dump_s) );
    return dump_buffer;
}
void HAL_cpr_set_age_mode(HAL_cpr_rail_t rail, bool enable)
{
}
uint32 HAL_cpr_age_data_valid(HAL_cpr_rail_t rail)
{
     return 0;
}