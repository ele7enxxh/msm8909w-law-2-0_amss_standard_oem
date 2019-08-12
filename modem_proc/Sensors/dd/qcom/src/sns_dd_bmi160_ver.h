/*******************************************************************************
 * Copyright (c) 2014, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

  /****************************************************************************
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ****************************************************************************/

/*==============================================================================
  DESCRIPTION

  Impelements the driver for the Bosch BMI160 part

  EDIT HISTORY FOR FILE


  when        who      what
  ----------  ---      -----------------------------------------------------------
  03/07/14    Albert    values, odr,  range, power mode, fifo  interrupt of accel, gyr, mag sensor motion detect, double tap interrupt of  accel v0.1
  03/21/14    Albert    self test for acc, gyr and mag  v0.2
  03/28/14    Albert    add initializtions for bmm150   v0.3
  04/02/14    Albert    improve get attr   v0.4
  04/08/14    Albert    add range index   v0.5
  04/17/14    lk        v0.6    : review of draft version v0.5
  04/21/14    lk        v0.6.1  : draft version code review and change,
  05/13/14    lk        v0.7    : first version that fixes all compile errors after review of draft versions
  05/15/14    lk        v1.0    : first very basic version working on ossc: accel + gyro both working in polling mode
  06/07/14    lk        v1.1    : fixed the issue: data of gyro does not get updated
  06/09/14    lk        v1.2    : added FIFO support
  06/12/14    lk        v1.3    : FIFO support works for accel
  06/15/14    lk        v1.3.1  : FIFO support: FIFO configured properly
  06/17/14    lk        v1.3.2  : FIFO support: fix the axes remap issue
  06/24/14    lk        v1.3.3  : FIFO support: fix the SNS_DDF_ATTRIB_FIFO issue
  06/24/14    lk        v1.3.4  : FIFO support: added new algorithm ldt for wml calculation
  06/29/14    lk        v1.3.5  : FIFO support: change the timing for samples
  06/29/14    lk        v1.3.6  : FIFO support: FIFO support works for accel and gyro
  06/29/14    lk        v1.3.7  : add temperature so it works with accel or gyro
  06/29/14    lk        v1.3.8  : add mandatory self test
  06/29/14    lk        v1.3.9  : add data_log_pkt
  06/29/14    lk        v1.3.10 : remove API dependency
  06/29/14    lk        v1.3.11 : use a wrapper for port_read for ease migration to SPI
  07/22/14    lk        v1.3.12 : add delay of power mode transition
  07/23/14    lk        v1.3.13 : add chip id for C2 sample
  07/23/14    lk        v1.3.14 : add chip id for C2 sample
  07/23/14    lk        v1.3.15 : add delay in write wrapper
  07/23/14    lk        v1.3.16 : fix the data corruption issue when fc is bigger than wml
  07/23/14    lk        v1.3.17 : fix the issue that streaming stops when accel and gyro both start
  07/24/14    lk        v1.3.18 : support for self test: OEM, HW/SW
  07/24/14    lk        v1.3.19 : fix some errors in self test
  07/30/14    lk        v1.4.0  : code refactor for uimage support
  07/30/14    lk        v1.4.1  : block data / trace log based on qc's suggestion for uimage mode
  07/30/14    lk        v1.4.2  : code clean up for COMPILE-TIME configurations
  07/30/14    lk        v1.4.3  : uimage fix
  08/07/14    lk        v1.4.4  : uimage fix due to floating point division
  08/09/14    lk        v1.5.0  : add support for double-tap
  08/22/14    lk        v1.5.1  : fix a few warnings by new compiler
  08/22/14    lk        v1.5.2  : double tap
  08/22/14    lk        v1.5.3  : - fix the issue in reset() by removing call to reset_state(),
                  - doubletap int can be lost due to delay in handle_irq_fifo()
  08/22/14    lk        v1.5.4  : optimize the status register read in interrupt_handler() so the timing is better
  08/26/14    lk        v1.5.5  : fix issue in get_attr_(ATTRIB_ODR)
  08/26/14    lk        v1.5.6  : fix issue in interrupt_handler(), ignore the int_stat.bits.drdy bit for handle_irq_drdy()
  08/27/14    lk        v1.5.7  : add delay after BMI160_REGV_CMD_SOFT_RESET according to spec
  09/01/14    lk        v1.6.0  : uses new APIs: sns_ddf_malloc_ex, sns_ddf_mfree_ex, sns_ddf_memhandler_malloc_ex
  09/01/14    lk        v1.6.1  : uses new APIs: UMSG, UMSG_[1-4]
  09/01/14    lk        v1.7.0  : start debugging of mag slave, files added:
                  - sns_dd_bmi160_common.h
                  - sns_dd_bmi160_slave.h
                  - sns_dd_bmi160_slave_bmm150.c
                  - sns_dd_bmi160_slave_bmm150_uimg.c
  09/06/14    lk        v1.7.1  : code can compile properly, and mag can show on sensor list
  09/06/14    lk        v1.7.2  : change the prototype of bmi160_sbus_read_wrapper() and bmi160_sbus_write_wrapper() for code size saving
  09/06/14    lk        v1.7.3  : code clean up after refactoring
  09/22/14    lk        v1.7.4  : first basic version with mag data streaming working: mag data are false still
  09/28/14    lk        v1.7.5  : preliminary version for mag support, mag have real values now
  09/28/14    lk        v1.7.6  : use multiplication intead of division for mag data conversion
  09/28/14    lk        v1.7.7  : change slave api: init_private_data();
  09/29/14    lk        v1.8.0  : move enable_sched_data() to big img area
  09/29/14    lk        v1.8.1  : change the MSG SSID to SNS from QDSP6
  09/29/14    lk        v1.8.2  : remove unnecessary delays in sbus_write_wrapper()
  10/02/14    lk        v1.8.3  : change parameter of verify_chip_id() from uint8_t to uint16_t
  10/02/14    lk        v1.8.4  : fix error of position of bmi160_dump_core_reg()
  10/02/14    lk        v1.8.5  : use udelay() instead of mdelay() for slave driver
  10/02/14    lk        v1.8.6  : add adc_resolution_milligauss field
  10/02/14    lk        v1.8.7  : add chip_rev
  10/02/14    lk        v1.8.8  : use edge triggering by default, passed 19hours duration test @fastest rate
  10/02/14    lk        v1.9.0  : fifo_flush()
  10/03/14    lk        v1.9.1  : fifo_flush() before odr change
  10/03/14    lk        v1.9.2  : detailed info for ATTR_DEVICE_INFO
  10/06/14    lk        v1.9.3  : disable dt by default, and fix build error when dt is disabled
  10/06/14    lk        v1.9.4  : disable mag if by default
  10/06/14    lk        v1.9.5  : fix crash issue during init when sensor is not connected (ossc only)
  10/06/14    lk        v1.9.6  : fix typo in ARRAY_SIZE(), and other code clean up
  10/07/14    lk        v1.9.7  : fix compile warnings by new compiler version
  10/15/14    lk        v1.9.8  : match power info to datasheet
  10/15/14    lk        v1.9.9  : fix the sequence of signal_deregister()
  10/16/14    lk        v1.9.10    : fix the typo .r to .l in SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST
  10/16/14    lk        v1.9.11    : sns_dd_bmi160_soft_reset_and_cfg_default()
  10/16/14    lk        v1.9.12    : move data log pkt to uimg code
  10/16/14    lk        v1.9.13    : fix the crash in sns_dd_bmi160_log_data_be()
  10/16/14    lk        v1.9.14    : increase the startup time for testing
  10/16/14    lk        v1.9.15    : add reset() after OEM test is done
  10/21/14    lk        v1.9.16    : change the filename lk_debug.h to sns_dd_bmi160_trace.h
  10/21/14    lk        v1.9.16.1  : call reset_state() in reset() for testing only
  10/27/14    lk        v1.9.17    : fortify support for SPI
  10/27/14    lk        v1.9.18    : centralized data log support
  10/27/14    lk        v1.9.19    : get_attr_odr() for SNS_DDF_SENSOR_TEMP returns the same ODR as major sensor type
  10/27/14    lk        v1.9.20    : fix timing issue after flush_fifo()
  10/30/14    lk        v1.9.21    : fix timing issue after flush_fifo()
  10/31/14    lk        v1.9.22    : fix timing issue after flush_fifo()
  10/31/14    lk        v1.9.23    : improve timing accuracy in handle_irq_drdy()
  11/02/14    lk        v1.10.0    : improve timing accuracy in handle_irq_wml()
  11/02/14    lk        v1.10.1    : minor changes in log
  11/02/14    lk        v1.10.2    : fine-tuning of data logs
  11/03/14    lk        v1.10.3    : fix timing gap between DRI and FIFO
  11/03/14    lk        v1.10.4    : decouple sns_dd_bmi160_reset_state() for modularity
  11/03/14    lk        v1.10.5    : boost FIFO read size regularlly for timing accuracy improvement
  11/03/14    lk        v1.10.6    : minor change to data_log_data() format
  11/05/14    lk        v1.10.7    : fine tuning of debug levels to have better control over debug messages
  11/05/14    lk        v1.10.8    : fix the issue of timing error (this.start <= last.end)
  11/06/14    jacek     v1.10.8    : fix the issue of dd_handle passed in sns_ddf_signal_register()
  11/12/14    lk        v1.10.9    : change the startup time after soft reset
  11/12/14    lk        v1.10.10   : change the file tabs to spaces
  11/13/14    lk        v1.10.11   : fix the issue of timing error when: SMGR set ODR=0, then flush_fifo
  11/13/14    lk        v1.10.12   : move range setting after power state to insure reliable setting
  11/13/14    lk        v1.10.13   : fine tuning of log levels for easier debugging of customer
  11/13/14    lk        v1.10.14   : fix the issue of timing error when ff_count read out is 0
  11/18/14    lk        v1.10.15   : use DD_ATTR_LOCAL to designate the functions and data for bigimage only
  11/18/14    lk        v1.11.00   : remodeling support for temperature sensor
  11/18/14    lk        v1.11.01   : remodeling support for temperature sensor: change one param of set_attr_range() from state to sub_dev
  11/18/14    lk        v1.11.02   : remodeling support for temperature sensor: change one param of set_attr_md() from state to sub_dev
  11/18/14    lk        v1.11.03   : remodeling support for temperature sensor: change one param of set_attr_md() from state to sub_dev
  11/18/14    lk        v1.11.04   : remodeling support for temperature sensor: change one param of set_attr_odr() from state to sub_dev
  11/18/14    lk        v1.11.05   : remodeling support for temperature sensor: change one param of set_attr_fifo() from state to sub_dev
  11/18/14    lk        v1.11.06   : remodeling support for temperature sensor: change one param of enable_sched_data() from state to sub_dev
  11/18/14    lk        v1.11.07   : remodeling support for temperature sensor: change one param of enable_sched_data() from state to sub_dev
  11/18/14    lk        v1.11.08   : interrupt_handler() handle_irq_attach_temp()
  11/18/14    lk        v1.11.09   : temperature data works with new req
  11/16/14    lk        v1.12.00   : use LPM1 for MD
  11/26/14    lk        v1.12.01   : fortify the code for ref_cnt in reset() even when something is wrong with sequence of set_attr_ps()
  11/27/14    lk        v1.12.02   : self triggered low power motion detect
  11/27/14    lk        v1.12.03   : fix compile errors for non-uimg platforms
  12/08/14    lk        v1.12.04   : low power md enhancements for different ver
  12/09/14    lk        v1.12.05   : fix of crash in uimage mode for lpmd
  12/09/14    lk        v1.12.06   : small fix when chip id is not read properly or when chip id is not correct, also some compiler warnings were removed
  12/30/14    lk        v1.12.06.1 : fix debug messages for Timestamp
  12/30/14    lk        v1.12.06.2 : ts injection added
  12/30/14    lk        v1.12.06.3 : fix error in timestamp manipulation in helper functions
  12/30/14    lk        v1.12.06.4 : ut update
  12/30/14    lk        v1.12.06.5 : add safety-check for ff_wml_req_acc/gyr/mag
  12/30/14    lk        v1.12.06.6 : fix error in timestamp manipulation in populate_sample_ts()
  12/30/14    lk        v1.12.06.7 : fix error in timestamp manipulation in handle_irq_drdy()
  01/03/15    lk        v1.12.06.8 : fix error in timestamp manipulation in nlt/net()
  01/04/15    lk        v1.12.06.9 : fix error of ff_avail_xxx
  01/04/15    lk        v1.12.07   : add effective overflow support
  01/04/15    lk        v1.12.08   : add effective overflow support, purge cache when needed
  01/04/15    lk        v1.12.09   : correct the fifo count when wml is big
  01/04/15    mw        v1.12.10   : fix error in get_attr(SNS_DDF_ATTRIB_FIFO);
  01/09/15    lk        v1.12.10.1 : fix issue that data was not flushed completely
  01/15/15    mw        v1.12.10.2 : replaced memcpy() with memscpy(), KW error
  01/16/15    lk        v1.12.10.3 : reset fifo immediately after a FIFO config change
  01/16/15    lk        v1.12.10.4 : fix typo in delay(1000x)
  01/16/15    lk        v1.12.10.5 : disable one block in populate_sample_ts()
  01/20/15    lk        v1.12.10.6 : reset fc when ff_wml_req_[agm] is 0
  01/21/15    lk        v1.12.10.7 : reset ts_last_sample_[agm] to 0 when MD is enabled or when ref_cnt is 0
  02/08/15    lk        v1.12.11   : added support of lower ODRs
  02/08/15    lk        v1.12.12   : ACC TCOC
  02/10/15    lk        v1.12.12.1 : disable TCOC by default
  02/10/15    lk        v1.12.13   : added BSD lic header in sns_dd_bmi160_trace.h
  02/10/15    lk        v1.12.14   : reset avail_last_ts flags when necessary, this is to support the concurrency of MD and accel streaming
  02/10/15    lk        v1.12.15   : slight change in dl_meta as some customers have problem with MSG_5
  02/19/15    mw        v1.12.16   : fix static analysis KW error
  02/22/15    lk        v1.12.17   : fix wrong enum BMI160_ODR_IDX due to merge error
  02/22/15    lk        v1.12.18   : set lowest ODR of accel in NORMAL mode as 13HZ
  02/22/15    lk        v1.12.19   : use ts_curr before sns_ddf_smgr_notify_data() so that dl_meta has more accurate profiling
  02/22/15    lk        v1.13.00   : added BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES as requested by QC
  02/22/15    lk        v1.13.01   : fix typos with avail_last_ts: BST_SET_VAL_BIT -> BST_CLR_VAL_BIT
  02/22/15    lk        v1.13.02   : update ff_sample_sync_info.mark in post_process()
  02/22/15    lk        v1.13.03   : mark avail_master_itvl_est as true eariler so we can have more accurate timestamp in the first batch
  02/22/15    lk        v1.13.04   : add a few comments for the ease on customer's side
  02/22/15    lk        v1.13.05   : improved of profiling of gap in dl_meta so that less unnecessary WARNING are emitted
  02/25/15    lk        v1.14.00   : improved API for slave, updated slave drivers
  02/25/15    lk        v1.14.01   : improved slave if operations for power on/off
  02/25/15    lk        v1.15.00   : improved timing accuracy when ODR is very small
  04/01/15    Mike      v1.15.01   : merge change from 1.13.05: reduce the bytes to read in fifo_flush to improve delay
  04/01/15    Albert    v1.15.02   : merge change from 1.13.05: fix: set the ODR back to 0 when accel exit MD
  04/01/15    Mike,Al   v1.15.03   : merge change from 1.13.05: remove the blocking delay in power mode transition interfaces
  04/01/15    lk        v1.15.04   : add profiling of time spent on interfaces
  04/01/15    lk        v1.15.05   : improvement: reduce i2c read if possible to reduce delay
  04/02/15    lk        v1.15.06   : refactor: added port_handle_real for bmi160_sbus_read_wrapper/bmi160_sbus_write_wrapper
  04/03/15    lk        v1.15.07   : added ts_last_sbus_write and sbus_normal_mode_ready so the sbus_read and write is context aware
  04/03/15    lk        v1.15.08   : added timer for sbus mode
  04/03/15    lk        v1.15.09   : remove unnecessary delays after introduction of contex-aware sbus rw wrapper
  04/03/15    lk        v1.15.10   : move the starting of timer in to the bottom level bus_write function
  04/06/15    Mike      v1.15.11   : merge change from 1.13.05: add (SNS_DDF_SENSOR_TEMP != sensor_type) in set_attr_odr to avoid the unnecessary flush
  04/08/15    Mike      v1.15.12   : merge change from 1.13.05: remove HW flush and conditionally do SW flush in set_attr_fifo()
  04/08/15    Mike      v1.15.13   : merge change from 1.13.05: #if BMI160_FOR_8X26_BUILD
  04/08/15    Mike      v1.15.14   : merge change from 1.13.05: #if FEATURE_BMI160_INT_PIN_CONFLICT_RESOLVE, this is only valid for certain boards which had the HW PIN conflicts
  04/08/15    Mike      v1.15.15   : define FEATURE_BMI160_INT_PIN_CONFLICT_RESOLVE and BMI160_FOR_8X26_BUILD
  04/14/15    lk        v1.15.16   : strengthen the handling ff_count in handle_irq_ff_wml
  04/14/15    lk        v1.15.17   : fix the sytax error of FEATURE_BMI160_INT_PIN_CONFLICT_RESOLVE
  04/14/15    lk        v1.15.18   : remove the overriding of error SNS_DD_ITEST_ZERO_VARIANCE
  04/17/15    lk        v1.15.19   : fix one compiler WARNING
  04/17/15    lk        v1.16.00   : BMI160_CONFIG_TIMESTAMP_64BIT add support for 64-bit timestamp (sns_ddf_time_t changes from 32bit to 64bit)
  04/20/15    lk        v1.16.01   : some cosmetic changes logs and in .h to make configurations easier
  04/22/15    lk        v1.17.00   : add support for TS adjustment of GD
  05/15/15    jacek     v1.17.01   : update bias values after OEM test
  05/16/15    lk        v1.17.02   : clear ff_sample_sync_info.avail_last_ts conditionally
  05/16/15    lk        v1.17.03   : replace BMI160_CONFIG_TIMESTAMP_64BIT by run-time check
  05/20/15    lk        v1.17.04   : SPI clock settings
  05/20/15    lk        v1.17.05   : add DD version into ATTRIB
  05/20/15    lk        v1.17.06   : remove unused SPI flag
  05/27/15    lk        v1.17.07   : call API sns_ddf_comm_bus_spi_set_clk_freq_hz() based on bus_type
  05/27/15    lk        v1.17.08   : wml compensation
  06/02/15    lk        v1.17.09   : turn on Accel before Gyro to prevent from dropping Accel command when Gyro is in 50ms power state transition
  06/03/15    lk        v1.17.10   : bmi160_set_reg_int_latch() used for md_mode0_enter
  06/11/15    lk        v1.17.11.0 : improvement when fifo frames are underrun
  06/12/15    lk        v1.17.11.1 : improvement on 1.17.11
  06/12/15    lk        v1.17.11.2 : introduce ctx into read_out_fifo()
  06/12/15    lk        v1.17.11.3 : bypass ff_bytes_to_read adjustment for context: FIFO_FC_COMPENSATION
  06/12/15    lk        v1.17.11.4 : add a log
  06/12/15    lk        v1.17.11.5 : change from DATA_LOG_CONTEXT to DATA_RPT_CONTEXT
  06/14/15    jacek     v1.17.12   : reduce the delay after SOFT_RESET from 100ms to 80ms
  06/14/15    lk        v1.17.13   : wa: reset ff_frames_underrun_acc/gyr/mag in case trigger_fifo_data() is not called
  06/14/15    lk        v1.17.14   : optimize for spi bus in cor_ff_count()
  06/23/15    lk        v1.17.15   : optimize for FIFO underrun
  06/25/15    lk        v1.17.16   : BMI160_REGV_ODR_MAP removes from the list 0.78HZ which is not compatible with CTS spec
  06/30/15    lk        v1.17.17   : fix for underrun condition: enable hybrid intr
  06/30/15    jacek     v1.17.17.1 : merge BMI160_CONFIG_ENABLE_ATTRIB_FILTER_DELAY
  06/30/15    lk        v1.17.17.2 : sync the configurations in sns_dd_bmi160.h
  07/02/15    lk        v1.17.18   : fix: skipped flush when "no new frames found, get out"
  07/02/15    lk        v1.17.19   : improve: when ODR requested is the same with current ODR, skip a few steps
  07/02/15    lk        v1.17.20   : fix: for the situation when frame comes into FIFO immediately after fifo_config_1 is configured
  07/02/15    lk        v1.17.21   : fix: for the situation when t.e > ts_curr, bound t.e
  07/03/15    lk        v1.17.22   : handle the situation when not all of the sensors are configured properly in registry
  08/03/15    Kaushik   v1.17.22.1 : MD thresholds from 0x10 to 0x04
  08/19/15    Kaushik   v1.17.22.2 : MD thresholds from 0x04 to 0x08
  ============================================================================*/

#ifndef __SNS_DD_BMI160_VER_H
#define __SNS_DD_BMI160_VER_H

#define SNS_DD_MODULE_VER_BMI160        "BMI160_DD_VERSION: 2015/08/03 11:45:00 1.17.22"
#define BMI160_SNS_DDF_ATTRIB_VERSION   (1172202)

#endif

