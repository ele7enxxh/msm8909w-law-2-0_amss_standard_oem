#ifndef __DDR_DEBUG_COMMON_H__
#define __DDR_DEBUG_COMMON_H__


/*==============================================================================
                      Warning: This file is auto-generated
================================================================================
                   Copyright 2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"

/* Length of transportation buffer in characters */
#define BUFFER_LENGTH   1024 //512

#define DEFAULT_SAMPLE_SIZE   (32)

#define MAX_STEP_POWER (16) 

/* Maximum length of command in characters */
#define CMD_MAX_LENGTH  64

/* Watchdog bark and bite time */
#define WATCHDOG_BARK_TIME_SCLK  0x7FFF
#define WATCHDOG_BITE_TIME_SCLK  0xFFFF

/* DDR tuning parameters */
#define DDR_TUNING_LOOP_COUNT  128
#define DQ_ROUT_MAX            0x7
#define DQ_PSLEW_MAX           0x3
#define DQ_NSLEW_MAX           0x3

#define RESET_DEBUG_SW_ENTRY_ENABLE  0x1
#define BITFLIP_TEST_SIZE 16*1024*1024
extern char str[BUFFER_LENGTH];
extern char str_error_log[BUFFER_LENGTH];
extern int32  dq_rd_cdc_value , dq_wr_cdc_value;
extern volatile uint32 dq_rout, dq_pslew, dq_nslew, read_cdc_delay, write_cdc_delay,dq_wr_scan_byte;
/*==============================================================================
                                  Function
==============================================================================*/
/* =============================================================================
**	Function : ddr_debug_rx_cb
** =============================================================================
*/
/**
*	@brief
*	Callback for transportation layer receiving.
*
*	@param[in]	bytes_read	Number of bytes read
*	@param[in]	err_code	Error code returned
*
*	@retval  None
*
*	@dependencies
*	None
*
*	@sideeffects
*	None
*
*	@sa
*	None
*/
void ddr_debug_rx_cb(uint32 bytes_read, uint32 err_code);

/* =============================================================================
**  Function : ddr_debug_tx_cb
** =============================================================================
*/
/**
*   @brief
*   Callback for transportation layer transmitting.
*
*   @param[in]  bytes_written  Number of bytes written
*   @param[in]  err_code       Error code returned
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
 void ddr_debug_tx_cb(uint32 bytes_written, uint32 err_code);

/* =============================================================================
**  Function : ddr_debug_input
** =============================================================================
*/
/**
*   @brief
*   Get input command for DDR Debug Mode.
*
*   @param  None
*
*   @retval  Input command
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
const char *ddr_debug_input( void );

/* =============================================================================
**  Function : ddr_debug_output
** =============================================================================
*/
/**
*   @brief
*   Output message for DDR Debug Mode.
*
*   @param[in]  msg  Message to be output
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_debug_output(const char *msg);
boolean ddr_debug_test(uint32 *ddr_base, uint32 failures[4]);
//boolean ddr_defect_interval_test_mem_map( uint32 base, uint32 limit);
boolean ddr_function_defect_test_bit_flip_tuning(const uint32 base, const uint32 size, uint32 failures[4]);
void ddr_debug_tune_cdc(boolean in_progress);
//void ddr_debug_tune_all(boolean in_progress);

#ifdef SEC_WDOG_ENABLE
void sysdbg_enable(void);
void ddr_debug_dog_init(boolean normal);
void ddr_debug_dog_enable(boolean enable);
void ddr_debug_dog_kick( void );
#endif

void ddr_debug_do_tuning(boolean in_progress, boolean is_all, boolean is_training);
void ddr_debug_dq_training(boolean in_progress);
boolean ddr_function_defect_test_bit_flip(const uint32 base, const uint32 size);
boolean ddr_defect_interval_test(uint32 test_select,SDRAM_INTERFACE interface,SDRAM_CHIPSELECT  chip_select);
void ddr_debug_ddr_training(void);
void ddr_debug_tuning_vddq(void);
void ddr_debug_cdc_tuning_manually(void);
boolean ddr_defect_interval_test
(  
  uint32            test_select,
  SDRAM_INTERFACE   interface,
  SDRAM_CHIPSELECT  chip_select
);

#endif

