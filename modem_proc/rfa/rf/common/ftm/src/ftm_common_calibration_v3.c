/*!
  @file
  ftm_common_calibration_v3.c

  @brief
  This module contains common RF calibration code
 
  @addtogroup RF_COMMON_FTM_CALIBRATION_V3
 
  @image html ftm_calibration_sweep_design.jpg 
  @image html ftm_calibration_sweep_result_fetch_design.jpg
  @image html ftm_common_calv3_timer.jpg
  @image html ftm_calibration_sweep_result_packet.jpg

  @brief
  This module provides a common framework to perform common calibration Sweep.
  This module is responsible for pushing the calibration intructions to linked
  list, performing sweep to execute the calibration instructions, pushing
  calibration results to result buffer, and sending the results to tools
  from result buffer. The detailed implementation of this framework is depicted 
  in the illustration below.
 
  This framework supports following steps.
 
  <b>Cal Configuration</b>
  - Arrgane all the calibration intructions sent from the PC side
    to a linked list (#ftm_calibration_configure_sweep ).
  - Fetch instructions for each segment (#ftm_calibration_fetch_instruction) in
    ISR context and call tech-specific CallBack functions to perform actions
    related to given opcode for that segment.
  - All techs should provide CB interfaces that should be hooked up to the 
    virtual table; so that the common framwework can call then. Virtual Table is 
    maintained in #ftm_common_data.cal_data.cal_v3.cal_config.cb_fp variable. 
    The description of these callback functions are documented in
    #ftm_calibration_callback_fp_type
  - Push the calibration results (if any) to Result buffer
    (#ftm_calibration_push_results)
 
  <b>Result Fetch</b>
  - Fetch the calibration results from the result buffer and package them
    as per diag limitation to send back to Tools 
    (#ftm_calibration_fetch_sweep_results). Fetch command can be called
    multiple times by Tools to get all calibration results. Indication of
    empty-result-buffer-read will be given by embedded side to send an
    indication to tools that there should be no more calls for result fetch.
 
  Following is the segment processing sequence :
  -# Configure Radio to RxTx State
  -# Configure Tx Waveform
  -# Configure Rx Condition. Rx Condition refers to setting the LNA State
    and LNA gain.
  -# Configure Tx Condition. Tx condition refers to overriding RGI, PA state,
    RGI, and PA bias.
  -# Add additional delay. The delay might be required to settle 
    RxAGC before doing Rx Measurement. However, Tx Condition and Tx 
    Measurement step can be used to allow RxAGC to settle. However, if 
    additional delay is required, then it should be added here.
  -# Measure Tx
  -# Measure Rx
 
  Following are the definitions used for Common Calibtration Sweep.
 
  <b>Packet Node</b>: This refers to a node in a linked list which holds all 
  the calibration configuration parameter received through one request packet.
  Packet Node contains following things.
  - Packet Header: This contains Cal Flag, Number of instruction segments (N), 
  and cumulative instruction payload size.
  - Instruction Segments: These are N number of instruction segments which
  includes OpCode, Segment interval, and Payloads related to each OpCode Mask

  <b>Packet Header</b>: Header of packet containing Cal Flag, Number of
  instruction segments, and cumulative instruction payload size.

  <b>Instruction Segment</b>: Segments containing Opcode, segment duration,
  and payload associated with each opcode mask.

  <b>Instruction Header</b>: Header of each instruction segment containing 
  opcode, and segment interval

  <b>Instruction Payload</b>: Payload associated with each opcode mask. 
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_calibration_v3.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/18/14   xsh     Fix compiler warnings
09/15/14   xsh     Fix KW error
09/10/14   xsh     Add result buffer size check when push individual cal results to response buffer
05/26/14    id      Fix for FBRx DC Cal functionality with Opcode
10/23/13   nrk     Fix compiler warnings
09/09/13   aro     Compiler error fix
09/06/13   aro     Debug F3 message
08/13/13    sk     Added support for compressed ESC Cal Command
07/30/13   aro     Removed extra byte allocation in fetch result response packet
07/15/13   cri     Added SYNC opcode support
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/19/13    dj     Support for Feedback Setup Opcode
04/16/13   aro     Variable/type/prototype cleanup
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/10/13   aro     Account for Tear down time in overhead time calculation
04/05/13   aro     Enabled calv3 optimization
04/02/13   aro     Migrated to inline Math functions
04/02/13   aro     Command for enable debug level for calv3
04/02/13   aro     Debug message update
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
03/28/13   aro     Added results publishing and pushing interfaces
03/28/13   aro     Added Optimized calibration sweep command
03/27/13   aro     Data structure restructing
03/27/13   aro     Added fetch opcode function
03/27/13   aro     OpCode unpack function
03/27/13   aro     Initial support for opcode sequencing
03/26/13   aro     Removed SW to HW timer transition to use ATS timer
03/11/13   sar     Updated file for APQ, GNSS only target.
01/21/13   kai     Added wcdma_calv3_tx_measure and wcdma_calv3_rx_measure
01/02/13   aro     Support for results format revision
12/07/12   aki     Addded ftm_calibration_dev_flag_to_rfm_dev
12/07/12   aro     Fixed unnecessary increment of segment counter during
                   polling segments
12/04/12   cri     Tx meas opcode for LTE
12/03/12   aro     Moved the F3 message
12/03/12   aro     Doc update
12/03/12   aro     Added 1x handler function for Tx measure opcode
12/03/12   aro     Enabled Rx Meas OpCode from common framework
12/03/12   aro     Renamed variable name
12/03/12   aro     Added support for Tx measure results buffer
11/30/12   aro     Initial support for Tx measure opcode
11/30/12   aro     Renamed variable
11/30/12   aro     1x handler function for rx measure opcode
11/28/12   cri     Added Sync handler
11/26/12   cri     Added Tx Override and Sync opcodes
11/26/12   kai     Added wcdma tx override operation support
11/21/12   aro     F3 message update
11/21/12   aro     Support for RX_MEASURE result 
11/20/12   aro     Fixed incorrect pointer for RX_MEASURE
11/19/12   aro     Added 0x4000 Rx Measure opcode
11/19/12   aro     Linked radio Setup and Tx override Opcodes
11/17/12   aro     Added Dime (on onwards) specific XPT OpCode
11/15/12   kai     Added wcdma radio setup operation support
11/16/12   cri     LTE radio setup opcode support
11/15/12   aro     Tx Override 1x handler function
11/15/12   aro     Mainlined enabling of variable length OpCode
11/15/12   aro     Code to enable variable length opcode
11/07/12   aro     Migrated Radio Setup operation to FTM Task
11/05/12   aro     Updated OpCode fetch function to populate payload pointer
                   for ET opcodes
11/05/12   aro     Removed OpCode init from opcode modules to common cal v3
                   init function
11/05/12   aro     Re-organized cal v3 data structure to be more modular
10/29/12   sty     Fixed compile error caused due to merge 
10/22/12   aro     Added v2 function to fetch opcode using variable length
                   opcode infrastructure
08/23/12   aro     Removed usage of coarse field from RF time profiler struct
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/25/12   aro     Segment error in F3 message
07/25/12   aro     Added time profiling for EPT
07/25/12   aro     Enabled XPT debug mode flag
07/25/12   aro     Added payload for each event
07/20/12   aro     Added support to do polling for EPT expansion
07/19/12   aro     Enabled polling opcode in cal v3 infrastructure
07/19/12   aro     Function name update
07/19/12   aro     Function to deinitialize polling data structure
07/19/12   aro     Added polling parameter initialization
07/19/12   aro     Added initial implementation of Polling OpOde
07/19/12   aro     F3 update + XPT variable name change
07/18/12   aro     Moved the XPT result pushing to common XPT framework
07/18/12   aro     Added Error Code and SubCopde fields in XPT results
07/06/12   aro     Rearranged arguments for the XPT process function
06/21/12   aro     Compiler warning fix
06/14/12   anr     Updates and bug fixes to common framework for EPT 
06/13/12   aro     Doxygen documentation update
06/13/12   aro     Updated function to compute paylaod size to pass the pointer
                   to cal v3 data
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Code beautification
06/11/12   cri     Fix pointer access issue with feature_dpd enabled
06/04/12   ks      Add FTM-PC interface for XPT
04/19/12   id      Fixed bug in segment error functionality
04/10/12   id      Make ftm_calibration_deinit_result_buffer function non-static
03/26/12   hy      TD-SCDMA Cal time optimization
03/02/12   aro     Mainlined Enhanced Rx Calibration
03/02/12   aro     Updated Timer interfaces to return enumeration as timer
                   action status
03/02/12   aro     Added ISR overrun detection mechanism
01/12/12   aro     Converted Tx flag and Rx flag to device flag
12/13/11   aro     Tx Measurement Configuration should always be done before 
                   doing Rx measurement configuration to prevent measurement
                   box from measuring Tx power from previous segment.
12/13/11   aro     Updated F3 messages for better debugging
12/08/11   aro     Fixed F3 message error
12/08/11   aro     Updated Work time F3 message to have Current OpCode
12/05/11   aro     Replaced RxAGC settling time with an enum exported by the 
                   CDMA module
11/17/11    hy     Added TDSCDMA V3 Cal 
11/16/11    gh     Move definition of RxAGC settling time for WCDMA to WCDMA 
11/16/11   aro     Renamed CDMA Rx config and measure functions
11/15/11   aro     Added optimized  CDMA Rx config and Measure functions with 
                   cal time reduction of 1 ms per Rx measurement
11/15/11   aro     Added check to return error code for the case when a new
                   CCP is received in the middle of calibration sweep
11/09/11   aro     Enabled OpCode F3 message
11/08/11   pl      Reduced LTE RxAGC settling time to give more headroom in 
                   RxAGC segment.
09/14/11   sar     Fixed KW errors. 
08/29/11   aro     Fixed Negative time wait for Rx settling time
08/25/11   aro     Enabled Segment Error codes + expanded error code to 64 bit
08/25/11   aro     Partitioned Configuration and Results error Code
08/25/11   aro     Renamed functions to better match the functionality
08/24/11    gh     Add back WCDMA support
08/24/11   aro     Updated debug messages
08/23/11   aro     Changed the HW timer configuration to be done on segment 0
08/19/11   aro     Klocworks error fixes
08/19/11   aro     Added feature to enable debug mode using test tree parameter
08/18/11   aro     Documentation Update
08/18/11   aro     Added feature to configure RF in TimerServices based timer
                   following by other RF cal actions in o_stmr based timer
08/18/11   aro     Removed initialization of current mode and tear down flags
08/11/11   aro     Added OpCode in data structure for debugging purpose
08/11/11   aro     Moved result buffer deinit to config init to avoid the
                   resetting of results buffer data after initialization
08/09/11   aro     Added support to log error code for faulty segment
08/09/11   aro     Updated calv3 config Tx waveform function to have return
08/08/11   aro     Fixed the bug where total number of expected results was
                   wrong causing the results to execeed rx results buffer limit
08/08/11   aro     Fixed the bug where sweep cease segment had undefined 
                   current RFM mode
08/04/11   aro     Added support to perform Tx Sweep for different PA states in
                   different Cal sweep without radio tear down
07/28/11   tnt     Update to new common cal v3 intf
07/28/11   aro     [1] Fixed HDET result push crash which caused by memory
                   access misalignment. Replaced with memcpy
                   [2] Added Result Query counter to keep track of number of
                   result queries done by Tools side
                   [3] Debug message update
07/28/11   aro     Removed MC Rx Config Opcode
07/27/11   aro     [1] Added code to ensure that results returned by tech
                   specific measure functions are within limit
                   [2] Updated push results interface to pass cal data
                   [3] Added Tear down radio API for CDMA
07/21/11   aro     Improved the efficiency of the Loop used for Opcode analysis
07/20/11   aro     Updated additional RxAGC settling wait to support new
                   Rx Config and Measure Opcode
07/20/11   aro     Compiler Warning Fix
07/20/11   aro     [1] Added Rx Measurement OpCode
                   [2] Renamed type fields
07/20/11   aro     Renamed Cal v3 Opcode/payload type names for consistency
07/20/11   aro     Updated Push Calibration results function to take arbitrary
                   number of Rx and Tx results
07/19/11   aro     Temporarily disabled Tx config and measure for intf update
07/19/11   aro     [1] Corrected the argument of PUSH result func to
                   ftm_calibration_segment_num_type
                   [2] Temporarily disabled LTE and WCDMA CB function to update
                   to new interface
07/19/11   aro     Compiler Error Fix
07/19/11   aro     Updated Config Rx and Measure Rx functions to handle multiple
                   Device for a given segment
07/08/11    gh     Add WCDMA section for V3 calibration
07/08/11   aro     Updated Rx measurement interface to pass pointer to result
                   container
07/08/11   aro     Code beautification
07/08/11   tnt     Add LTE section for V3 calibration
07/08/11   aro     Added support to return multiple Rx measurement data
07/08/11   aro     [1] Added entry for tech specific RxAGC settling time
                   [2] Added support to dynamically calculate the time to allow
                   RxGAC settle before Rx measurement
                   [3] Added RFM Mode validity check
                   [4] Code documentation + beautification
06/29/11   aro     [1] Removed F3 message in Fetch instruction function
                   [2] Added code for RxAGC settling for the case wherer
                   Tx Measurement opcode is not enabled                   
06/28/11   aro     Renamed header file to ftm_common_calibration_v3
06/28/11   aro     Documentation Update
06/27/11   aro     Added support for Tx waveform config OpCode
06/27/11   aro     Documentation Update
06/27/11   aro     [1] Documentation Update
                   [2] Moved Packet Header to separate type
06/27/11   aro     Documentation Update 2
06/27/11   aro     Documentation Update
06/27/11   aro     [1] Compiler Warning Removal
                   [2] Doxygen Documentation Update
06/27/11   aro     [1] Removed Cal V3 debug flag
                   [2] Compiler warning removal
06/27/11   aro     Added Result Fetch Design illustration to doxygen
06/27/11   aro     Renamed Type name
06/26/11   aro     Merged Read Node Data structure to Cal Config Data
06/26/11   aro     Moved Calibration Config Data to a seprate data str
06/26/11   aro     Added off-target support to test multiple result fetch
06/26/11   aro     [1] Added documentation for result request function
                   [2] Moved Read pointer initialization to result_buff init
06/26/11   aro     [1] Added more documentation for functions used to
                   handle sweep instructions config
                   [2] Renamed Sweep instruction config function names
                   [3] Converted instruction pointer to byte pointer in PUSH
                   sweep configuraiton function
06/26/11   aro     [1] Added more documentation for functions used to
                   handle calibration results
                   [2] Renamed Calibration result function names
06/25/11   aro     [1] Renamed function names
                   [2] Documentation Update
06/25/11   aro     Merged OpCodes to ftm_common_calibration.h
06/25/11   aro     [1] Added support to fetch CalV3 results
                   [2] Added support to teardown RF at the end of Sweep
06/24/11   aro     Moved results to separate data structure
06/24/11   aro     Enabled ISR CB function to push Cal Data to Result Buffer
06/24/11   aro     Added feature to push all Cal data at once
06/23/11   aro     Added support to push Calibration data to result buffer
06/21/11   aro     Switched to Common ISR function from tech specific ISR func
06/21/11   aro     Added calls to Tech specific OpCode Callback function
06/20/11   aro     Added common calibration sweep ISR function
06/20/11   aro     Moved Calibration V3 Data structure to FTM Common Data
06/15/11   aro     Added function to allocation memory for results
06/13/11   aro     [1] Changed the ISR trigger time to 20 mS
                   [2] Temporarily disabled CRC check
                   [3] F3 message update
06/08/11   aro     [1] Reused req_len field for Total instruction size
                   [2] Added functionality to send expected cal data from tools
06/01/11   aro     Added Retune OpCode
05/16/11   aro     Added HDET Measurement OpCode
05/09/11   aro     1x Calibration Sweep ISR function
06/06/11   aro     Doxygen Documentation Update
05/06/11   aro     Updated Calibration Sweep config interface to return
                   FTM response packet
05/05/11   aro     Added support to delete all previously added nodes when CRC
                   failure is detected
05/05/11   aro     [1] Added CRC check for Calibration packet
                   [2] Added error/success codes for return values
                   [3] Removed static variable from get instruction function
                   [4] Type name change
05/05/11   aro     Removed payload pointer to replace with individual opcode
                   payload pointers
05/04/11   aro     Compiler Warning Fix
05/04/11   aro     [1] Documentation Update
                   [2] Renamed types/variable names
05/04/11   aro     [1] Fixed Calibration init not to have multiple execution
                   [2] Updated Get Segment interface to return partitioned data
05/03/11   aro     [1] Fixed Linked List delete node bug where all nodes were
                   not deleted
                   [2] Fixed bug where the Cal Pointer pointer was corrupted
                   [3] Renamed type names
04/25/11   aro     [1] Added interface to get Calibration config for a segment
                   [2] Renamed variables
04/25/11   aro     [1] Added support to configure CB function for various techs
                   [2] Added linked-list implementation to store RF Cal Config
                   [3] Renamed Sweep Config function
04/25/11   aro     Compiler Error Fix
04/22/11   aro     Initial Revision

==============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN

#include "rfa_variation.h"
#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_common_timer.h"
#include "ftm_common_control.h"
#include "ftm_common_data.h"
#include "rfm_internal.h"
#include "rfcommon_math.h"
#include "modem_mem.h"
#include "string.h"
#include "stringl.h" /* for memscpy */
#include "ftm_msg.h"
#include "rfm_internal.h"
#include "DALSys.h" 
#include "rfcommon_time_profile.h" 
#include "zlib.h"

#ifdef FEATURE_FACTORY_TESTMODE

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Delay to fire the first calibration Sweep ISR
 
  @details
  Time in microseconds used to trigger the first FTM Calibration segement. This
  time should be used only once while starting the ISR sweep. Once the
  calibration Sweep ISR starts, the ISR interval should be the one from
  segment Instruction.
*/
#define FTM_CAL_SWEEP_TRIGGER_SETUP_TIME 20

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Slack to be added to Results Buffer
 
  @details
  Additional Slack to be added to the the actual result buffer allocation. This
  slack will be used to avoid accidental buffer overflow while recording
  Calibration Result. The units of this slack is bytes.
*/
#define FTM_CAL_SWEEP_RESULT_BUFFER_SLACK 4

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Diag Response Packet Limit Backoff
 
  @details
  This enum is used as a backoff value from the maximum allowed response packet.
  This backoff is used so that cal results data take up memeory less than Diag 
  Limit leaving room for response packet overheads. The units of this backoff 
  is bytes.
*/
#define FTM_CAL_SWEEP_DIAG_LIMIT_BACKOFF 100

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Diag Response Packet Limit for Off-target
 
  @details
  This enum is used as a Diag Reponse Packet limit to used only for Off-target.
  This allows easy test case implementation to test multiple request for
  calibration results.
*/
#define FTM_CAL_SWEEP_DIAG_LIMIT_FOR_OFT 10


/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Used for setting size of uncompressed packet 
*/
#define FTM_CAL_SWEEP_V3_PACKET_SIZE_MAX 32768

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  ESC Cal v3 Packet Header and Footer size
*/
#define FTM_CAL_SWEEP_V3_PACKET_HDR_FTR  21


/*============================================================================*/
/*!
  @name Calibration Results

  @brief
  This section contains functions for FTM to collect Common Calibration Sweep
  results. This includes function to create and delete result buffer along with
  an accessor function to push the calibration results to the result buffer.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Result buffer
 
  @details
  This function will allocate the results buffer where all the calibrated
  data will be stored. The size of allocation will be based on the summation
  of expected result size per instruction segment sent from the PC
  side. The total buffer size will be added with a fixed Slack value to protect
  potential buffer overflow.
 
  If this function is called twice in a row, then previously allocated
  results buffer will be deleted and a new buffer will be allocated.
 
  The buffer allocated in this function will be deallocated once all results
  data is fetched and sent back to PC
 
  @return
  Flag indicating, if results buffer allcoation is successful or not.
 
  @retval TRUE
  Initialization of results buffer is successful
 
  @retval FALSE
  Initialization of results buffer failed
*/
boolean
ftm_calibration_init_result_buffer
(
  void
)
{
  ftm_calibration_data_type *cal_data; /* Calv3 Data */
  boolean ret_val; /* return value */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Get the cumulative result buffer size that were extracted from the
  segment instruction packet. Add a slack to the buffer size to prevent
  memory leack during calibration results push. */
  cal_data->result.buff_size += FTM_CAL_SWEEP_RESULT_BUFFER_SLACK;

  /* Allocate Results Buffer : Only if buffer size is greater than zero*/
  if ( cal_data->result.buff_size > 0 )
  {
    cal_data->result.buff_ptr = modem_mem_alloc( cal_data->result.buff_size,
                                                 MODEM_MEM_CLIENT_RFA );
  }
  else
  {
    cal_data->result.buff_ptr = NULL;
  }

  if ( cal_data->result.buff_ptr != NULL )
  {
    /* Reset the buffer content to zero */
    memset( cal_data->result.buff_ptr, 0, cal_data->result.buff_size );
  
    /* Reset the Result helper variable  to reset state */
    cal_data->result.buff_read_en = FALSE;  /* Disable Buffer read */
    cal_data->result.buff_written = 0;  /* Reset written Buffer Size */
    cal_data->result.buff_read = 0;  /* Reset read Buffer Size */
    cal_data->result.query_num = 0; /* Results query numbers */
  
    /* Assign the Write pointer to the top of the Results buffer. Initialization
    of Write Pointer to top of the Results buffer should be done once until
    the results buffer is deallocated. */
    cal_data->result.write_ptr = (uint8*)cal_data->result.buff_ptr;
  
    /* Assign the Read pointer to the top of the Results buffer. Initialization
    of Read Pointer to top of the Results buffer should be done once until
    the results buffer is deallocated. */
    cal_data->result.read_ptr = (uint8*)cal_data->result.buff_ptr;

    memset(&(cal_data->seg_data.result_data), 0x0, 
           sizeof(ftm_cal_result_per_segment_data_type) );
    
    ret_val = TRUE;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_init_result_buffer: Result "
             "buffer allocation failed" );
    ret_val = FALSE;
  }
  
  return ret_val;

} /* ftm_calibration_init_result_buffer */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Calibration results
 
  @details
  This function will deallocate the results buffer where all the calibrated
  data were stored. This will also set all the helper vairables used to store
  and fetch results, to a default state.
 
  This function should be called only after all calibrated data is sent back to 
  PC side. Thus it is important that result buffer read is disabled in this
  function. Beside that all variables to keep track of buffer size and buffer
  usage must be reset to zero.
 
  @return
  Status indicating if the deinitialization of sweep results
 
  @retval TRUE
  Deinitialization of results buffer is successful
 
  @retval FALSE
  Deinitialization of results buffer failed due to modem_malloc_free failure
*/
boolean
ftm_calibration_deinit_result_buffer
(
  void
)
{
  ftm_calibration_data_type *cal_data; /* Calv3 Data */
  boolean ret_val = TRUE; /* Return Value */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Deallocate memory, only if it was not done before. This is required to
  support accidental multiple call of this function*/
  if ( cal_data->result.buff_ptr != NULL )
  {
    modem_mem_free( cal_data->result.buff_ptr, MODEM_MEM_CLIENT_RFA );
    cal_data->result.buff_ptr = NULL; /* NULL Buffer Pointer */
  }
  else
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_sweep_deinit_results: Results "
                        "buffer already deallocted" );
    ret_val &= FALSE;
  }
  
  /* Reset the Result helper variable  to reset state */
  cal_data->result.read_ptr = NULL; /* NULL Read Pointer */
  cal_data->result.write_ptr = NULL;  /* NULL Write Pointer */
  cal_data->result.buff_read_en = FALSE;  /* Disable Buffer read */
  cal_data->result.buff_size = 0;  /* Reset Buffer Size */
  cal_data->result.buff_written = 0;  /* Reset written Buffer Size */
  cal_data->result.buff_read = 0;  /* Reset read Buffer Size */
  cal_data->result.query_num = 0; /* Results query numbers */

  return ret_val;

} /* ftm_calibration_deinit_result_buffer */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Publish OpCode result
 
  @details
  This function shall be called when a opcode dispatch resulted in the
  result generation. This publishing will be looked at when the results are
  pushed to the ResultsBuffer at the end of the segment.
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing the opcode dispatch data for
  which the result is to be published
 
  @return
  Falg indicating the status of result publishing
*/
static boolean
ftm_calibration_publish_result
(
  ftm_calibration_data_type *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{

  ftm_cal_result_per_segment_data_type *result_data;
  boolean ret_val = TRUE;

  /* Error checkin */
  if ( oc_disp->res_buff_info.buff_sz == 0 )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_calibration_publish_result: OpCode %d,"
                           "does not produce result", oc_disp->oc_bit );
    return FALSE;
  } /* if ( oc_disp->res_buff_info.buff_sz == 0 ) */

  result_data = &(cal_data->seg_data.result_data);

  /* Update the result count and result size */
  result_data->total_result_cnt++;
  result_data->total_result_sz += oc_disp->res_buff_info.buff_sz;

  /* Update the "result availabe" flag to be used during results push */
  if ( oc_disp->new_loc <= 63 )
  {
    result_data->result_avl.opcode.oc_g0 |= 1<<oc_disp->new_loc;
    result_data->result_avl.opcode_group_mask |= 0x1;
  } /* if ( ( oc_disp->new_loc >= 0 ) && (oc_disp->new_loc <= 63) ) */
  else if ( ( oc_disp->new_loc >= 63 ) && (oc_disp->new_loc <= 127) )
  {
    result_data->result_avl.opcode.oc_g1 |= 1<<oc_disp->new_loc;
    result_data->result_avl.opcode_group_mask |= 0x8;
  } /* else if ( ( oc_disp->new_loc >= 63 ) && (oc_disp->new_loc <= 127) ) */
  else if ( ( oc_disp->new_loc >= 128 ) && (oc_disp->new_loc <= 191) )
  {
    result_data->result_avl.opcode.oc_g2 |= 1<<oc_disp->new_loc;
    result_data->result_avl.opcode_group_mask |= 0x4;
  } /* else if ( ( oc_disp->new_loc >= 128 ) && (oc_disp->new_loc <= 191) ) */
  else if ( ( oc_disp->new_loc >= 192 ) && (oc_disp->new_loc <= 255) )
  {
    result_data->result_avl.opcode.oc_g3 |= 1<<oc_disp->new_loc;
    result_data->result_avl.opcode_group_mask |= 0x8;
  } /* else if ( ( oc_disp->new_loc >= 192 ) && (oc_disp->new_loc <= 255) ) */
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_calibration_publish_result: Invalid"
                          "number of OC group Num %d", oc_disp->new_loc );
    ret_val = FALSE;
  } /* if ! ( ( oc_disp->new_loc >= 0 ) && (oc_disp->new_loc <= 63) ) */

  /* Debug message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
               "Publish :  %d bytes using %d opcodes", 
               cal_data->seg_data.cal_instance,
               result_data->total_result_sz,
               result_data->total_result_cnt );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_publish_result */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Calibration results to Results Buffer
 
  @details
  This function will be called from calibration Sweep ISR to record all 
  calibration results to results buffer. All data in results buffer will
  be packed.
 
  This function should be called once per segemnt, if any calibration data is
  available for that segment. For each call, either both Rx and Tx Cal result
  can be present, or only one of them can be present. This information should
  also be passed to be function via result_mask argument.
 
  Inside this function, it will be checked if there will be a buffer overflow 
  while pushing new cal results into the results buffer. If buffer overflow
  will happen due to the Push, then "data push" will be aborted.
 
  @param cal_data
  Pointer tothe data structure containing calibration data
  
  @return
  Status of execution of Data Push to result buffer
 
  @retval TRUE
  Flag indicating that the calibration results is successfully pushed into
  the results buffer.
 
  @retval FALSE
  Flag indicating that the calibration results could not be pushed
  due to possible buffer overflow.
*/
inline static boolean
ftm_calibration_push_result
(
  ftm_calibration_data_type *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_data_type *oc_cfg;
  ftm_cal_result_per_segment_data_type *res_seg;
  ftm_cal_opcode_per_segment_data_type *oc_seg;
  uint16 txmeas_nloc;
  uint16 rxmeas_nloc; 
  uint8 oc_group_mask;
  uint8 oc_group_num;
  uint64 opcode_mask;
  uint64 opcode_temp;
  ftm_cal_opcode_bit_type opcode_bit;
  ftm_cal_opcode_type b6b4_oc; /* OpCode with FTM_CAL_OPCODE_MEAS_HDET and 
  FTM_CAL_OPCODE_MEAS_RX, For the opcode with b4 and b6 active, the result bufer 
  should be have results populated with Rx result first followed 
  by Tx Result. << SPECIAL CASE FOR BACKWARD COMPATIBILITY */

  oc_cfg = &(cal_data->cal_config.opcode_data);
  res_seg =&(cal_data->seg_data.result_data);
  oc_seg = &(cal_data->seg_data.opcode_data);

  /* Check if we will have buffer overflow while pushing cal data into the
  results buffer. If Memory Leak will happen due to the Push, then abort Data 
  Push */
  if ( ( cal_data->result.buff_written + 
         cal_data->seg_data.result_data.total_result_sz ) >
         cal_data->result.buff_size )
  {
    FTM_MSG_3( FTM_FATAL, "ftm_calibration_push_result: Instance %d : "
                "Aborting Data Push - Buffer Overflow : Written %d of %d bytes",
               cal_data->seg_data.cal_instance, 
               cal_data->result.buff_written,
               cal_data->result.buff_size );
    res_seg->result_avl.opcode.oc_g0 = 0;
    res_seg->result_avl.opcode.oc_g1 = 0;
    res_seg->result_avl.opcode.oc_g2 = 0;
    res_seg->result_avl.opcode.oc_g2 = 0;

    /* Reset the segment result Data */
    res_seg->total_result_cnt = 0;
    res_seg->total_result_sz = 0;

    return FALSE;
  }

  /* Print debug message to track results push */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
               "RESULT PUSH :  [ %3d bytes till 0x%8x ] START ::::::", 
               cal_data->seg_data.cal_instance,
               cal_data->result.buff_written,
               cal_data->result.write_ptr );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  /*--------------------------------------------------------------------------*/
  /* 01 : Push Segment Number to Result buffer */
  /*--------------------------------------------------------------------------*/

  memscpy( cal_data->result.write_ptr, 
           sizeof(ftm_calibration_segment_num_type),
           &(cal_data->seg_data.cal_instance),
           sizeof(ftm_calibration_segment_num_type));

  /* Increment the Result Write pointer and Buffer written variable with the 
  instruction segment interval size. */
  cal_data->result.write_ptr += sizeof(ftm_calibration_segment_num_type);
  cal_data->result.buff_written += sizeof(ftm_calibration_segment_num_type);

  /* Print debug message to track results push */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
               "RESULT PUSH :  [ %3d bytes till 0x%8x ] SegNum", 
               cal_data->seg_data.cal_instance,
               cal_data->result.buff_written,
               cal_data->result.write_ptr );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
  
  /*--------------------------------------------------------------------------*/
  /* 02 : Push Result payload descriptor to Result buffer */
  /*--------------------------------------------------------------------------*/

  if ( cal_data->result.result_rev == FTM_CAL_RESULT_REV_0 )
  {
    memscpy( cal_data->result.write_ptr, sizeof(ftm_calibration_result_desp_type),
             &(res_seg->total_result_sz),sizeof(ftm_calibration_result_desp_type));
  } /* if ( cal_data->result.result_rev = FTM_CAL_RESULT_REV_0 ) */
  else if ( cal_data->result.result_rev == FTM_CAL_RESULT_REV_1 )
  {
    memscpy( cal_data->result.write_ptr, sizeof(ftm_calibration_result_desp_type),
             &(res_seg->total_result_cnt), sizeof(ftm_calibration_result_desp_type));
  } /*if ( cal_data->result.result_rev = FTM_CAL_RESULT_REV_0 ) */
  else
  {
    uint8 num_of_res = 0;
    FTM_MSG_2( FTM_ERROR, "ftm_calibration_push_result: Instance %d : "
               "Invalid Result Revision %d", 
               cal_data->seg_data.cal_instance,
               cal_data->result.result_rev );
    memscpy( cal_data->result.write_ptr, sizeof(ftm_calibration_result_desp_type),
             &(num_of_res), sizeof(ftm_calibration_result_desp_type));
  } /* if ( cal_data->result.result_rev = FTM_CAL_RESULT_REV_0 ) */

  /* Increment the Result Write pointer and Buffer written variable with the 
  size of the calibration data length field. */
  cal_data->result.write_ptr += sizeof(ftm_calibration_result_desp_type);
  cal_data->result.buff_written += sizeof(ftm_calibration_result_desp_type);

  /* Print debug message to track results push */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
               "RESULT PUSH :  [ %3d bytes till 0x%8x ] ResLen", 
               cal_data->seg_data.cal_instance,
               cal_data->result.buff_written,
               cal_data->result.write_ptr );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  /*##########################################################################*/
  /*# SPECIAL CASE for OpCode with FTM_CAL_OPCODE_MEAS_HDET and */
  /*# FTM_CAL_OPCODE_MEAS_RX */
  /*##########################################################################*/
  /*#*/txmeas_nloc = oc_cfg->location_map[FTM_CAL_OPCODE_MEAS_HDET].new_loc;
  /*#*/rxmeas_nloc = oc_cfg->location_map[FTM_CAL_OPCODE_MEAS_RX].new_loc;
  /*#*/b6b4_oc.oc_g0 = (1 << FTM_CAL_OPCODE_MEAS_HDET) | 
  /*#*/                               (1 << FTM_CAL_OPCODE_MEAS_RX);
  /*#*/b6b4_oc.oc_g1 = 0;b6b4_oc.oc_g2 = 0;b6b4_oc.oc_g3 = 0;
  /*#*/if ( (b6b4_oc.oc_g0 & oc_seg->oc_info.opcode.oc_g0) == b6b4_oc.oc_g0 )
  /*#*/{
  /*#*/  /*-----------------------------------------------------------------*/
  /*#*/  /* 03 : Push Rx results length to Result buffer */
  /*#*/  /*-----------------------------------------------------------------*/
  /*#*/
  /*#*/  if ( rxmeas_nloc < FTM_CAL_OPCODE_MAX )
  /*#*/  {
  /*#*/    if ( oc_cfg->dispatch_table[rxmeas_nloc].res_buff_info.buff_sz > 0 )
  /*#*/    {
  /*#*/      memscpy( cal_data->result.write_ptr,
  /*#*/        oc_cfg->dispatch_table[rxmeas_nloc].res_buff_info.buff_sz, 
  /*#*/        oc_cfg->dispatch_table[rxmeas_nloc].res_buff_info.buff_ptr,
  /*#*/        oc_cfg->dispatch_table[rxmeas_nloc].res_buff_info.buff_sz );
  /*#*/    
  /*#*/      /* Increment the Result Write pointer and Buffer written variable*/
  /*#*/      cal_data->result.write_ptr += 
  /*#*/        oc_cfg->dispatch_table[rxmeas_nloc].res_buff_info.buff_sz;
  /*#*/      cal_data->result.buff_written += 
  /*#*/        oc_cfg->dispatch_table[rxmeas_nloc].res_buff_info.buff_sz;
  /*#*/    
  /*#*/      res_seg->result_avl.opcode.oc_g0 &= (~(uint64)(1<<rxmeas_nloc));
  /*#*/    
  /*#*/      /* Print debug message to track results push */
  /*#*/      if ( cal_data->cal_config.debug_mode_en == TRUE )
  /*#*/      {
  /*#*/        FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
  /*#*/                   "RESULT PUSH :  [ %3d bytes till 0x%8x ] Meas Rx", 
  /*#*/                   cal_data->seg_data.cal_instance,
  /*#*/                   cal_data->result.buff_written,
  /*#*/                   cal_data->result.write_ptr );
  /*#*/        
  /*#*/      } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
  /*#*/    } /*if(oc_cfg->dispatch_table[rxmeas_nloc].res_buff_info.buff_sz>0)*/
  /*#*/  } /* if ( rxmeas_nloc < FTM_CAL_OPCODE_MAX ) */ 
  /*#*/  else
  /*#*/  {
  /*#*/    FTM_MSG_1( FTM_ERROR, "ftm_calibration_push_result: Bad RxMeas"
  /*#*/               "loc number", rxmeas_nloc );
  /*#*/  } /* if ! ( rxmeas_nloc < FTM_CAL_OPCODE_MAX ) */ 
  /*#*/
  /*#*/  /*-----------------------------------------------------------------*/
  /*#*/  /* 04 : Push Tx results length to Result buffer */
  /*#*/  /*-----------------------------------------------------------------*/
  /*#*/
  /*#*/  if ( txmeas_nloc < FTM_CAL_OPCODE_MAX )
  /*#*/  {
  /*#*/    if ( oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_sz > 0 )
  /*#*/    {
  /*#*/      memscpy( cal_data->result.write_ptr, 
  /*#*/       oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_sz, 
  /*#*/       oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_ptr,
  /*#*/       oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_sz );
  /*#*/    
  /*#*/      /* Increment the Result Write pointer and Buffer written */
  /*#*/      cal_data->result.write_ptr += 
  /*#*/        oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_sz;
  /*#*/      cal_data->result.buff_written += 
  /*#*/        oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_sz;
  /*#*/    
  /*#*/      res_seg->result_avl.opcode.oc_g0 &= (~(uint64)(1<<txmeas_nloc));
  /*#*/    
  /*#*/      /* Print debug message to track results push */
  /*#*/      if ( cal_data->cal_config.debug_mode_en == TRUE )
  /*#*/      {
  /*#*/        FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
  /*#*/                   "RESULT PUSH :  [ %3d bytes till 0x%8x ] Meas Tx", 
  /*#*/                   cal_data->seg_data.cal_instance,
  /*#*/                   cal_data->result.buff_written,
  /*#*/                   cal_data->result.write_ptr );
  /*#*/
  /*#*/      } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
  /*#*/    } /*if(oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_sz>0)*/
  /*#*/  } /* if ( txmeas_nloc < FTM_CAL_OPCODE_MAX ) */ 
  /*#*/  else
  /*#*/  {
  /*#*/    FTM_MSG_1( FTM_ERROR, "ftm_calibration_push_result: Bad RxMeas"
  /*#*/               "loc number", txmeas_nloc );
  /*#*/  } /* if ! ( txmeas_nloc < FTM_CAL_OPCODE_MAX ) */ 
  /*#*/
  /*#*/  if ( res_seg->result_avl.opcode.oc_g0 == 0 )
  /*#*/  {
  /*#*/    res_seg->result_avl.opcode_group_mask &= (~(uint8)1);
  /*#*/  } /* if ( res_seg->result_avl.opcode.oc_g0 == 0 ) */
  /*#*/
  /*#*/} /* if (b6b4_oc.oc_g0 & oc_seg->oc_info.opcode.oc_g0)==b6b4_oc.oc_g0 )*/
  /*##########################################################################*/

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */

  oc_group_mask = inline_rf_extract_first_one_bit_mask( 
                       res_seg->result_avl.opcode_group_mask );

  while ( oc_group_mask != 0 )
  {
    /* Find the opcode group number */
    oc_group_num = inline_rf_extract_first_one_bit_lsb_location(oc_group_mask);

    /* Get the OpCode for the current instruction: grp_cnt refers to
    the OpCode group for which the payload is to be extracted */
    switch ( oc_group_num )
    {
    case 0:opcode_temp = res_seg->result_avl.opcode.oc_g0;break;
    case 1:opcode_temp = res_seg->result_avl.opcode.oc_g1;break;
    case 2:opcode_temp = res_seg->result_avl.opcode.oc_g2;break;
    case 3:opcode_temp = res_seg->result_avl.opcode.oc_g2;break;
    default:
      FTM_MSG_1( FTM_ERROR, "ftm_calibration_push_result: Invalid"
                            "number of OC group Num %d", oc_group_num );
      opcode_temp = 0;
      ret_val = FALSE;
      break;
    } /* switch ( oc_group_num ) */

    /* For the current unprocessed Opcode, extract the bit mask for the first
    enabled cal action. The first bit will be from LSB. */
    opcode_mask = inline_rf_extract_first_one_bit_mask_uint64( opcode_temp );

    /* Loop until, the OpCode Mask comes out to be zero */
    while ( opcode_mask != 0 )
    {

      /* Find the bit location and this location will correspond to 
      enumeration in ftm_cal_opcode_bit_type. This number is readjusted
      (grp_cnt*64) as the opcode might fall on any opcode group. */
      opcode_bit = (ftm_cal_opcode_bit_type)(
                    inline_rf_extract_first_one_bit_lsb_location(opcode_mask) +
                    oc_group_num*64 );

      if (opcode_bit < FTM_CAL_OPCODE_MAX)
      {
        if ( oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_sz > 0 )
      	{
		  if( cal_data->result.buff_size >= (oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_sz + cal_data->result.buff_written) )
		  {
		    memscpy( cal_data->result.write_ptr, 
                 oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_sz,
                 oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_ptr,
                 oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_sz );
    
        	/* Increment the Result Write pointer and Buffer written */
	        cal_data->result.write_ptr += 
              oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_sz;
            cal_data->result.buff_written += 
              oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_sz;
       
            /* Print debug message to track results push */
            if ( cal_data->cal_config.debug_mode_en == TRUE )
            {
              FTM_MSG_4( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
                     "RESULT PUSH :  [ %3d bytes till 0x%8x ] OC-0x%x", 
                     cal_data->seg_data.cal_instance,
                     cal_data->result.buff_written,
                     cal_data->result.write_ptr,
                     oc_cfg->dispatch_table[opcode_bit].oc_bit );
            } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
		  }
		  else
		  {
    		FTM_MSG_3( FTM_FATAL, "ftm_calibration_push_result: Instance %d : "
                "Aborting Data Push - Buffer Overflow : Requested Written %d of %d left result bytes",
               cal_data->seg_data.cal_instance, 
               oc_cfg->dispatch_table[opcode_bit].res_buff_info.buff_sz,
               (cal_data->result.buff_size - cal_data->result.buff_written) );
			
    		res_seg->result_avl.opcode.oc_g0 = 0;
    		res_seg->result_avl.opcode.oc_g1 = 0;
    		res_seg->result_avl.opcode.oc_g2 = 0;
    		res_seg->result_avl.opcode.oc_g2 = 0;
			
		    /* Reset the segment result Data */
		    res_seg->total_result_cnt = 0;
		    res_seg->total_result_sz = 0;
			
		    return FALSE;			
		  }
        } /*if(oc_cfg->dispatch_table[txmeas_nloc].res_buff_info.buff_sz>0)*/
      }
      /* Update OpCode to set the processed bit to zero. */
      opcode_temp = opcode_temp & (~opcode_mask);
      opcode_mask = inline_rf_extract_first_one_bit_mask_uint64( opcode_temp );
    } /* while ( opcode_mask != 0 ) */
    
    /* Update OpCode to set the processed bit to zero. */
    res_seg->result_avl.opcode_group_mask &= (~oc_group_mask);

    oc_group_mask = inline_rf_extract_first_one_bit_mask( 
                           res_seg->result_avl.opcode_group_mask );

  } /* while ( oc_group_mask != 0 ) */

  res_seg->result_avl.opcode.oc_g0 = 0;
  res_seg->result_avl.opcode.oc_g1 = 0;
  res_seg->result_avl.opcode.oc_g2 = 0;
  res_seg->result_avl.opcode.oc_g2 = 0;

  /* Reset the segment result Data */
  res_seg->total_result_cnt = 0;
  res_seg->total_result_sz = 0;

  /*--------------------------------------------------------------------------*/

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    /* Print debug message to track results push */
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
               "RESULT PUSH :  [ %3d bytes till 0x%8x ] END ::::::", 
               cal_data->seg_data.cal_instance,
               cal_data->result.buff_written,
               cal_data->result.write_ptr );

    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RESULT] "
               "Data Push Success - %d of %d bytes", 
               cal_data->seg_data.cal_instance,
               cal_data->result.buff_written,
               cal_data->result.buff_size );
  } /* if ( cal_data->debug_mode_en == TRUE ) */
  
  return ret_val;

} /* ftm_calibration_push_result */

/*! \} */

/*============================================================================*/
/*!
  @name Sweep Instruction Configuration

  @brief
  This section contains functions for Calibration Sweep to configure and fetch 
  the Calibration instruction.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Calibration Sweep Configuration

  @details
  This function will reset the state for Calibration Sweep brining all the
  parameter to default settings. This function should be called once per
  calibration sweep execution. If this function is called multiple times within
  same calibration sweep, then successive calls will be ignored.
 
  This function will also initialize all the tech specific Opcode Callback
  function.
*/
static void
ftm_calibration_init_sweep
(
  void
)
{
  ftm_calibration_data_type *cal_data; /* Calv3 Data */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* This part of initialization should done once per Calibration Sweep.
  Thus check against the static_init_done variable. This variable will
  be set to FALSE state only when Sweep Deinit is done. Sweep Deinit is done
  only when Calibration Sweep ISR is completed. */
  if ( cal_data->status.cal_config_init_done != TRUE )
  {
    /* Reset the Variable used to keep track of state information for
    Calibration Sweep */
    cal_data->seg_data.cal_instance = -1;

    /* Reset the RFM Mode to Invalid Mode, only if Tear down was done at the
    end of the sweep. For the cases where tear down is not done at the end
    (expecting new CCP without RFConfig  OpCode), the current RFM Mode should
    be retained. */
    if ( cal_data->cal_config.tear_down_radio == TRUE )
    {
      cal_data->status.current_rf_mode = RFM_INVALID_MODE;
    }

    /* Initialize the OpCode Module */
    ftm_calibration_init_opcode(cal_data);

    /* Initialize all the opcodes available to use in the calv3
    infrastructrure */
    ftm_calibration_tear_down_init(cal_data);
    ftm_calibration_radio_config_init(cal_data);
    ftm_calibration_radio_retune_init(cal_data);
    ftm_calibration_tx_waveform_init(cal_data);
    ftm_calibration_radio_setup_init(cal_data);
    ftm_calibration_config_tx_init(cal_data);
    ftm_calibration_meas_hdet_init(cal_data);
    ftm_calibration_tx_override_init(cal_data);
    ftm_calibration_tx_measure_init(cal_data);
    ftm_calibration_config_rx_init(cal_data);
    ftm_calibration_meas_rx_init(cal_data);
    ftm_calibration_rx_measure_init(cal_data);
    ftm_calibration_sync_init(cal_data);
    ftm_calibration_polling_init(cal_data);
    ftm_calibration_xpt_init(cal_data);
    ftm_calibration_data_init(cal_data);
    ftm_calibration_feedback_setup_init(cal_data);
    ftm_calibration_enh_internal_device_cal_init(cal_data);

    /* Flag that static init is done for this Sweep. This variable will be
    reset to FALSE, when calibration Sweep ISR is completed. */
    cal_data->status.cal_config_init_done = TRUE;

    /* Initialize the Segment Error Code to 0 */
    cal_data->status.seg_error_log.first_seg_error = 0;
    cal_data->status.seg_error_log.seg_error = 0;

    /* Initialize the faulty Segment number to 0 */
    cal_data->status.seg_error_log.first_error_seg_num = 0;

    cal_data->cal_config.instraverse_prev_ptr = 0;
    
    /* Check if Results buffer is NULL or not. If not, de-allocate already
    allocated memory and allocate a new one */
    if ( cal_data->result.buff_ptr != NULL )
    {
      ftm_calibration_deinit_result_buffer();
      FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_init_sweep: Result buffer "
               "is not NULL, so de-initialized.");
    }
  }
  else
  {
    return;
  }

} /* ftm_calibration_init_sweep */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-initialize Calibration Sweep

  @details
  This function is used to deinitialize the calibration sweep. This will reset
  all the state variables for calibration Sweep. Along with this, linked list
  created to store the calibration instruction will be deleted.
 
  This function will traverse through all the nodes and delete each of the
  dynamically allocated memory locations.
 
  This function should be called at the end of calibration sweep.
*/
void
ftm_calibration_deinit_sweep
(
  void
)
{
  ftm_calibration_data_type *cal_data; /* Calv3 Data */
  ftm_calibration_packet_node_type *temp_node_ptr; /* Temp Node Pointer used
  for Linked List Deletion */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Iterate through the Linked list starting from Head Node and delete
  the nodes */
  while ( cal_data->cal_config.head_node != NULL )
  {
    /* Temporarily hold the pointer to next Node */
    temp_node_ptr = cal_data->cal_config.head_node->next_config;
    
    /* If the Calibration instruction is not NULL for the current Node, then
    de-allocate the memory */
    if ( cal_data->cal_config.head_node->cal_instruction != NULL )
    {
      modem_mem_free( cal_data->cal_config.head_node->cal_instruction, 
                      MODEM_MEM_CLIENT_RFA);
    }
 
    /* De-allocate the Head Node */
    if ( cal_data->cal_config.head_node != NULL )
    {
      modem_mem_free( cal_data->cal_config.head_node, MODEM_MEM_CLIENT_RFA);
    }
    
    /* Re-instate the head pointer to next node */
    cal_data->cal_config.head_node = temp_node_ptr;
  } /* while ( cal_data->cal_config.head_node != NULL ) */

  /* De-Initialize all the opcodes available to use in the calv3
  infrastructrure */
  ftm_calibration_tear_down_deinit(cal_data);
  ftm_calibration_radio_config_deinit(cal_data);
  ftm_calibration_radio_retune_deinit(cal_data);
  ftm_calibration_tx_waveform_deinit(cal_data);
  ftm_calibration_radio_setup_deinit(cal_data);
  ftm_calibration_config_tx_deinit(cal_data);
  ftm_calibration_meas_hdet_deinit(cal_data);
  ftm_calibration_tx_override_deinit(cal_data);
  ftm_calibration_tx_measure_deinit(cal_data);
  ftm_calibration_config_rx_deinit(cal_data);
  ftm_calibration_meas_rx_deinit(cal_data);
  ftm_calibration_rx_measure_deinit(cal_data);
  ftm_calibration_sync_deinit(cal_data);
  ftm_calibration_polling_deinit(cal_data);
  ftm_calibration_xpt_deinit(cal_data);
  ftm_calibration_data_deinit(cal_data);
  ftm_calibration_feedback_setup_deinit(cal_data);
  ftm_calibration_enh_internal_device_cal_deinit(cal_data);

  /* De-initialize OpCode Module */
  ftm_calibration_deinit_opcode(cal_data);

  /* Reset Data Structure */
  cal_data->status.cal_config_init_done = FALSE;
  cal_data->seg_data.cal_instance = -1;

  /* Reset the RFM Mode to Invalid Mode, only if Tear down was done at the
  end of the sweep. For the cases where tear down is not done at the end
  (expecting new CCP without RFConfig  OpCode), the current RFM Mode should
  be retained. */
  if ( cal_data->cal_config.tear_down_radio == TRUE )
  {
    cal_data->status.current_rf_mode = RFM_INVALID_MODE;
  }

  cal_data->cal_config.read_node = NULL;
  cal_data->cal_config.read_node_seg_counter = 0;
  cal_data->cal_config.read_node_payload_ptr = NULL;
  cal_data->cal_config.instraverse_prev_ptr = 0;

  /* Throw Debug messages */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_deinit_sweep : Done " );
  } /* if ( cal_data->debug_mode_en == TRUE ) */

} /* ftm_calibration_deinit_sweep */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Add a new Calibration configuration Node to the linked list

  @details
  This function will create a new calibration configuraiton node. If there is
  no head Node for the linked list, then this function will create the 
  head as well. To create a new node, linked list will always be traversed
  starting from first node.

  Apart from creating a new linked list node, this function will also
  dynamically allocate memory to hold the RF Calibration configuraiton data.

  CRC check will be performed at the start of this function. If no CRC code
  match is found, then entire linked list should be erased.

  All the nodes and configuration container allocated in this function should
  be free'd at the end of calibration by calling "delete linked list" interface.

  @param instr_segment_nums
  Number of instruction segments present in the calibration packet

  @param instr_size
  Total size of all calibration instructions in this packet

  @param instr_ptr
  Pointer to the first instruction of the packet. This pointer will be used to
  copy RF calibration configuration from Request packet to the Node.

  @return
  Error code produced during allocation of packet node.
*/
static ftm_calibration_config_error_code_type
ftm_calibration_push_configuration
(
  uint16 instr_segment_nums,
  uint16 instr_size,
  uint8 *instr_ptr
)
{
  ftm_calibration_packet_node_type *list_ptr; /* Pointer to Linked List */
  //uint32 crc_addr; /* Address where CRC Code is stored */
  ftm_calibration_config_error_code_type ret_val; /* return value */
  //uint32 *crc_ptr; /* CRC Pointer */
  ftm_calibration_data_type *cal_data; /* Calv3 Data */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Calculate the Address where CRC is stored in calibration Packet */
  //crc_addr = (uint32)instr_ptr + instr_size;

  /* Update the CRC pointer to point to CRC Address */
  //crc_ptr = crc_addr;

  /* CRC check for Packet validation */
  //if ( (uint16)(*crc_ptr) != FTM_CALIBRATION_PACKET_CRC_VAL )
  //{    
  //  /* Use Invalid CRC as error code */
  //  ret_val = FTM_CALIBRATION_INVALID_CRC;
  //}
  //else
  {
    /* Check if the Head node is created */
    if ( cal_data->cal_config.head_node == NULL )
    {
      /* Allocate Memory for the Head Node*/
      cal_data->cal_config.head_node =
      modem_mem_alloc( sizeof(ftm_calibration_packet_node_type),
                       MODEM_MEM_CLIENT_RFA );
    
      if ( cal_data->cal_config.head_node != NULL )
      {
        /* Allocate Memory for Cal Configuration for the Head Node */
        cal_data->cal_config.head_node->cal_instruction = 
                     modem_mem_alloc( instr_size, MODEM_MEM_CLIENT_RFA );
    
        /* Update the number of segments */
        cal_data->cal_config.head_node->num_of_segment = instr_segment_nums;
      
        /* Point the Next Node to NULL */
        cal_data->cal_config.head_node->next_config = NULL;
      
        /* Initialize the Set the Packet data pointer to head node */
        cal_data->cal_config.read_node = cal_data->cal_config.head_node;

        if ( cal_data->cal_config.head_node->cal_instruction != NULL )
        {
          /* Copy RF Calibration instructions to the Packet Node */
          memscpy( cal_data->cal_config.head_node->cal_instruction,
                   instr_size, instr_ptr, instr_size );

          /* Use SUCCESS as error code */
          ret_val = FTM_CALIBRATION_PACKET_ALLOCATED;
        }
        else
        {
          FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_push_configuration : "
                   "Cal instruction allocation failure" );
          /* Flag as failure */
          ret_val = FTM_CALIBRATION_CONFIG_ALLOC_FAILURE;

        }      
      }
      else
      {
        FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_push_configuration : "
                 "Head Node allocation failure" );

        /* Flag as failure */
        ret_val = FTM_CALIBRATION_CONFIG_ALLOC_FAILURE;
      }
    }
    else
    {
    
      /* Assign the variable to point to the start of the Linked list. Linked
      List will be traveresed from this node onwards */
      list_ptr = cal_data->cal_config.head_node;
    
      /* Traverse through the linked list to reach the end Node */
      while( list_ptr->next_config != NULL )
      {
        list_ptr = list_ptr->next_config;
      }
    
      /* Allocate memeory for the New Node */
      list_ptr->next_config = 
                   modem_mem_alloc( sizeof(ftm_calibration_packet_node_type),
                                    MODEM_MEM_CLIENT_RFA );
    
      if ( list_ptr->next_config != NULL )
      {
        /* Allocate memory for Calibration Instruction for new Node */
        list_ptr->next_config->cal_instruction = 
                       modem_mem_alloc( instr_size, MODEM_MEM_CLIENT_RFA );
      
        /* Update the number of segments */
        list_ptr->next_config->num_of_segment = instr_segment_nums;

        /* Point the Next Node to NULL */
        list_ptr->next_config->next_config = NULL;

        if ( list_ptr->next_config->cal_instruction != NULL )
        {
          /* Copy Data to the Calibration Instruction */
          memscpy( list_ptr->next_config->cal_instruction, 
                   instr_size, instr_ptr, instr_size );

          /* Use SUCCESS as error code */
          ret_val = FTM_CALIBRATION_PACKET_ALLOCATED;
        }
        else
        {
          FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_push_configuration : "
                   "Next Config Cal instruction allocation failure" );

          /* Flag as failure */
          ret_val = FTM_CALIBRATION_CONFIG_ALLOC_FAILURE;
        }
      }
      else
      {
        FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_push_configuration : "
                 "Next Config allocation failure" );

        /* Flag as failure */
        ret_val = FTM_CALIBRATION_CONFIG_ALLOC_FAILURE;
      }
    }

    /* Throw Debug messages */
    if ( cal_data->cal_config.debug_mode_en == TRUE )
    {
      FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_push_configuration : "
                         "CCP allocated" );
    } /* if ( cal_data->debug_mode_en == TRUE ) */
  }
  
  /* return */
  return ret_val;

} /* ftm_calibration_push_configuration */

/*! \} */

/*============================================================================*/
/*!
  @name ISR

  @brief
  This section contains functions for FTM to perform Calibration Sweep under
  ISR context.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interrupt handler routine for Calibration Sweep

  @details
  This function will perform one instance of Calibration Sweep. Thus for the
  complete sweep, this functon will be called repeatedly under IST context.

  This function will be registered with the ISR when Common Calibration is
  is configured; and it will be triggered according to the ISR interval
  configured. 

  Inside this function, the first thing done is to request the opcode, its
  associated payloads, and other associated configuration parameter for the 
  current segment from the FTM Common Calibration module.

  Based on the segment duration, next ISR is scheduled. Following that will
  be to execute the RF calibration sequence based on the opcode recieved for
  this segment.

  This function also has holds the responsibility to cease the ISR based on the 
  configuration parameter received.
*/
static void
ftm_calibration_sweep
(
  void
)
{
  boolean instr_flag; /* Flag to indicate validity of Cal intruction */
  ftm_calibration_data_type *cal_data; /* Calibration Sweep Data */
  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val; /* Function Profiling Variable */
  static rf_time_tick_type prof_cal_isr; /* Function Profiling Variable */
  rf_time_type prof_cal_isr_temp = 0; /* Function Profiling Variable */
  rf_time_tick_type oc_proc_tick; /* Function Profiling Variable */
  rf_time_type oc_proc_time = 0; /* Function Profiling Variable */
  ftm_common_timer_status_type timer_status = FTM_TIMER_FAILURE;
  ftm_cal_opcode_dispatch_info_type *oc_disp = NULL;
  boolean oc_proc_s;
  boolean res_pub_s;
  boolean res_push_s;
  boolean need_to_push_results = FALSE;

  /* Get Calibration Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Reset the segment Error to 0 */
  cal_data->status.seg_error_log.seg_error = 0;

  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */  

  /* ------------------------------------------------------------------------ */
  if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
       ( (cal_data->cal_config.dbg_level & CAL_DBG_ISR) == CAL_DBG_ISR ) )
  {
    if ( cal_data->seg_data.cal_instance >= 0 )
    {
      /* Get Time for this ISR\ */
      prof_cal_isr_temp = rf_time_get_elapsed( prof_cal_isr, RF_USEC );
    }
    prof_cal_isr = rf_time_get_tick(); /* Get Time tick for ISR Profile */
  } /* if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
       ( cal_data->cal_config.dbg_level == CAL_DBG_ISR ) ) */
  /* ------------------------------------------------------------------------ */

  /* Unpack instruction */
  instr_flag = ftm_calibration_unpack_instruction(cal_data);
  
  /* Check if the Calibration instruction flag is valid or not. If it is
  not valid, then cease the ISR, followed by necessary cleanup actions */
  if ( instr_flag == TRUE )
  {
    /*------------------------------------------------------------------------*/
    /* Analyize the Opcode, and perform all the necessary actions. Sequence
    for the operations are important. Make sure that Rx Configuration is
    done first followed by Tx configuration and Calibration measurement. 
    Calibration Measurement can be optimized to reduce the calibration time.*/
    /*------------------------------------------------------------------------*/

    /* Increment the Cal Instance, only if it is not in PAUSE_TO_POLL mode. */
    if ( cal_data->status.pause_to_poll == FALSE )
    {
      cal_data->seg_data.cal_instance++;
    } /* if ( cal_data->status.pause_to_poll == FALSE ) */

    /* Update the Next ISR */
    timer_status = ftm_common_update_hw_timer( cal_data->seg_data.interval + 
                     ftm_calibration_sync_get_offset(cal_data, prof_cal_func));

    /* If timer returns FALSE, then Flag segment error */
    if ( timer_status == FTM_TIMER_FAILURE )
    {
      cal_data->status.seg_error_log.seg_error |= 
                              (1 << FTM_CAL_SEG_ERR_TIMER_UPDATE);
      FTM_MSG_1( FTM_ERROR, "[calv3][Segment %3d][ISR] "
                 "Timer Update failed", cal_data->seg_data.cal_instance );
    } /* if ( timer_status == FTM_TIMER_FAILURE ) */
    else if ( timer_status == FTM_TIMER_ISR_OVERRUN_DETECTED )
    {
      cal_data->status.seg_error_log.seg_error |= 
                               (1 << FTM_CAL_SEG_ERR_SEG_DURATION_OVERRUN);
      FTM_MSG_1( FTM_ERROR, "[calv3][Segment %3d][ISR] "
                 "ISR overrun", cal_data->seg_data.cal_instance );
    } /* else if ( timer_status == FTM_TIMER_ISR_OVERRUN_DETECTED ) */

    /* Loop until all the opcode bits present in the given instruction
    are dispatched */
    while ( ftm_calibration_get_opcode( cal_data, &oc_disp ) )
    {
      /* NULL Pointer check */
      if ( ( oc_disp != NULL ) && ( oc_disp->process_fp != NULL ) )
      {
        /* Start Time profling OC process function */
        if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
             ( (cal_data->cal_config.dbg_level & CAL_DBG_ISR) == CAL_DBG_ISR ) )
        {
          oc_proc_tick = rf_time_get_tick();
        } /* if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
                  ( cal_data->cal_config.dbg_level == CAL_DBG_ISR ) ) */

        /* Call the process function for the opcode */
        oc_proc_s = oc_disp->process_fp( cal_data, oc_disp );

        if(oc_proc_s != TRUE)
        {
          FTM_MSG_1( FTM_LOW, "opcode  process function failed: Status = %d ",
                               oc_proc_s);
        }

        /* End Time profling OC process function */
        if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
             ( (cal_data->cal_config.dbg_level & CAL_DBG_ISR) == CAL_DBG_ISR ) )
        {
          oc_proc_time += rf_time_get_elapsed( oc_proc_tick, RF_USEC );
        } /* if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
                  ( cal_data->cal_config.dbg_level == CAL_DBG_ISR ) ) */

        /* If the opcode has calibration results, then the opcode dispatch
        data stcuture (oc_disp) will be updated accordingl by the opcode
        process function. Thus if the results are available, publish them
        so that they can be pushed into the results buffer at the end */
        if ( oc_disp->res_buff_info.buff_sz > 0 )
        {
          /* Publish available results for the opcode */
          res_pub_s = ftm_calibration_publish_result( cal_data, oc_disp );

          /* Flag that the results need to be published */
          if ( res_pub_s == TRUE )
          {
            need_to_push_results = TRUE;
          } /* if ( res_pub_s == TRUE ) */
        } /* if ( oc_disp->res_buff_info.buff_sz > 0 ) */
      } /* if ( ( oc_disp != NULL ) && if ( oc_disp->process_fp != NULL ) ) */
    } /* while ( ftm_calibration_get_opcode( cal_data, oc_disp ) ) */

    /* If there is atleast one opcode in the instruction that has results, then
    push the results to result buffer */
    if ( need_to_push_results == TRUE )
    {
      res_push_s = ftm_calibration_push_result(cal_data);

      if(res_push_s != TRUE)
      {
        FTM_MSG_1(FTM_LOW,"ftm_calibration_push_result() failed: Status = %d ",
                           res_push_s);
      }
    } /* if ( need_to_push_results == TRUE ) */
  } /* if ( instr_flag == TRUE ) */
  else /* Invalid Instruction, cease ISR + Cleanup */
  {
    /* Throw Debug messages */
    if ( cal_data->cal_config.debug_mode_en == TRUE )
    {
      FTM_MSG_1( FTM_HIGH, "[calv3][Segment %3d][ISR] "
                 "Tear down start", cal_data->seg_data.cal_instance );
    } /* if ( cal_data->debug_mode_en == TRUE ) */

    /* Cease the ISR */
    timer_status = ftm_common_uninstall_hw_timer();
    
     /* If timer returns FALSE, then Flag segment error */
    if ( timer_status != FTM_TIMER_INSTALL_DONE )
    {
      cal_data->status.seg_error_log.seg_error |= 
                               (1 << FTM_CAL_SEG_ERR_TIMER_UPDATE);
      FTM_MSG_1( FTM_ERROR, "[calv3][Segment %3d][ISR] "
                 "Timer Uninstall failed", cal_data->seg_data.cal_instance );
    } /* if ( timer_status == FTM_TIMER_FAILURE ) */

    if ( cal_data->cal_config.tear_down_radio == TRUE )
    {
      /* Start Time profling OC process function */
      if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
           ( (cal_data->cal_config.dbg_level & CAL_DBG_ISR) == CAL_DBG_ISR ) )
      {
        oc_proc_tick = rf_time_get_tick();
      } /* if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
                ( cal_data->cal_config.dbg_level == CAL_DBG_ISR ) ) */

      /* Tear Down Radio */
      ftm_calibration_tear_down_process(cal_data);

      /* End Time profling OC process function */
      if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
           ( (cal_data->cal_config.dbg_level & CAL_DBG_ISR) == CAL_DBG_ISR ) )
      {
        oc_proc_time += rf_time_get_elapsed( oc_proc_tick, RF_USEC );
      } /* if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
                ( cal_data->cal_config.dbg_level == CAL_DBG_ISR ) ) */

      FTM_MSG_1( FTM_HIGH, "[calv3][Segment %3d][ISR] "
                 "Tear down done", cal_data->seg_data.cal_instance );
    } /* if ( cal_data->cal_config.tear_down_radio == TRUE ) */
    else
    {
      FTM_MSG_1( FTM_HIGH, "[calv3][Segment %3d][ISR] "
                 "Tear Down SKIPPED", cal_data->seg_data.cal_instance );
    } /* if ( cal_data->cal_config.tear_down_radio == TRUE ) */

    /* Reset Cal Data */
    cal_data->seg_data.cal_instance = -1;

    /* Enable Result Buffer Read. Until this is done, results cannot be sent
    back to PC*/
    cal_data->result.buff_read_en = TRUE;

    /* De-initialize the Common Calibration Sweep */
    ftm_calibration_deinit_sweep();

    /* Flag that Calibration Sweep is inactive from this point */
    cal_data->status.is_sweep_active = FALSE;
    
  } /* Is instruction valid */

  /* Log the info for the first faulty Segment. Logging should done,
  only if segment_error is zero (current segment is faulty) and the
  segment_error is cal_data is not zero (we are updating this varaiable
  for the first time, which means current segment is the first faulty
  segment) */ 
  if ( ( cal_data->status.seg_error_log.seg_error != 0 ) &&
       ( cal_data->status.seg_error_log.first_seg_error == 0 ) )
  {
    /* Log the first faulty segement number */
    cal_data->status.seg_error_log.first_error_seg_num = 
                               cal_data->seg_data.cal_instance;
    /* Log the error code for first segment number */
    cal_data->status.seg_error_log.first_seg_error = 
                               cal_data->status.seg_error_log.seg_error;

  } /* Push Segement Error to Data structure */

  /* ------------------------------------------------------------------------ */
  if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
       ( (cal_data->cal_config.dbg_level & CAL_DBG_ISR) == CAL_DBG_ISR ) )
  {
    /* Get time taken for this function to execute */
    prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );
  
    /* Display work duration of current segment and actual ISR duration between 
    last segment and current segment. */
    FTM_MSG_7( FTM_HIGH, "[calv3][Segment %3d][ISR] [OpCode 0x%10x] "
               "SegErr 0x%x::%d - [Work Time %6d] [ISR Time %6d] "
               "[overhead %4d]", 
               cal_data->seg_data.cal_instance, 
               cal_data->seg_data.opcode_data.oc_info.opcode.oc_g0,
               cal_data->status.seg_error_log.first_seg_error,
               cal_data->status.seg_error_log.first_error_seg_num,
               prof_cal_func_val, prof_cal_isr_temp,
               (prof_cal_func_val - oc_proc_time) );
  } /* if ( ( cal_data->cal_config.debug_mode_en == TRUE ) ||
       ( cal_data->cal_config.dbg_level == CAL_DBG_ISR ) ) */
  /* ------------------------------------------------------------------------ */

  /* De-initialize the OpCode */
  ftm_calibration_invalidate_opcode(cal_data);
  
} /* ftm_calibration_sweep */

/*! \} */

/*============================================================================*/
/*!
  @name Common Dispatch Interface

  @brief
  This section contains functions for FTM Common Dispatch to perform Common 
  Calibration Sweep and query results for the calibration Sweep.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Core function to configure calibration sweep

  @details
  This function will configure the Calibration v3 based on the data
  segments received from PC side. This function can be called multiple times
  before triggering the ISR to start calibration. During each call, this
  function will store the configuration parameter in a dynamically allocated
  memory. Each consecutive configuration parameter will be linked together
  using a linked list.

  All the memory locations that are dynamically allocated will be freed up at
  the end of calibration sweep.

  This function should not be called again, after "CALIBRATION START" is sent
  and before the response packet is reveived by the tools. If the function
  is called in between, then the integrity of configuration data cannot be
  guaranteed resulting is possible calibration failure.

  @param header
  Pointer to the data structure containing the command payload for
  sweep configuration
 
  @param cal_data
  Pointer to calv3 state data structure

  @return
  Status indicating the state of sweep configuration
*/
static int32
calibration_configure_sweep
(
  ftm_cal_sweep_config_req_pkt_type* header,
  ftm_calibration_data_type *cal_data
)
{
   ftm_calibration_config_error_code_type ret_val = FTM_CALIBRATION_ISR_UNDEFINED;
  ftm_calibration_config_packet_type *cal_pkt; /* Calibration Packet */
  ftm_common_timer_status_type timer_status = FTM_TIMER_FAILURE;

  /* Check if the calibration sweep is active or not. If the sweep is active,
  then the CCP cannot be processed. This indicates that Tools sent the
  request to early. */
  if ( cal_data->status.is_sweep_active == TRUE )
  {
    /* Indicate error code */
    ret_val = FTM_CALIBRATION_CCP_WHILE_ACTIVE_SWEEP;

    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_configure_sweep: "
                        "Sweep is active - Cannot take new CCP" );

  } /* if ( cal_data->is_sweep_active == TRUE ) */
  else
  {
    /* Typecast cal_param to extract the command header */
    cal_pkt = (ftm_calibration_config_packet_type*)(&(header->cal_param));
  
    /* Based on the Flag, enable or disable the debug mode */
    cal_data->cal_config.debug_mode_en = 
      (boolean)((cal_pkt->packet_header.res_rev_dbg_mode) & 0x1);
  
    /* Store the results mode for this sweep */
    cal_data->result.result_rev = 
      (((cal_pkt->packet_header.res_rev_dbg_mode) >> 0x1) & 0x7F );
      
    /* Initialize FTM Calibration Sweep Data */
    ftm_calibration_init_sweep();
  
    /* Push the Calibration Request Data to New Node */
    ret_val = ftm_calibration_push_configuration( 
                           cal_pkt->packet_header.num_of_segments,
                           cal_pkt->packet_header.tot_instr_size,
                           &(cal_pkt->instr_ptr) );
  
    /* Add the expected size of calibration data for this node to
    cal_data_rsp_size */
    cal_data->result.buff_size += cal_pkt->packet_header.cal_data_len;
  
    /* If the start calibration command is set to high, trigger the ISR */
    if ( ( ret_val == FTM_CALIBRATION_PACKET_ALLOCATED ) &&
         ( (boolean)cal_pkt->packet_header.start_cal_cmd == TRUE ) )
    {
      /* Allocate Memory for results before triggering the ISR. This memory
      should de-allocated once the results are flushed to PC */
      if ( ftm_calibration_init_result_buffer() )
      {
        /* Flag that Calibration Sweep is active from this point */
        cal_data->status.is_sweep_active = TRUE;

        /* Install Timer */
        timer_status = ftm_common_install_hw_timer( 
                                  FTM_CAL_SWEEP_TRIGGER_SETUP_TIME,
                                  ftm_calibration_sweep );

        if ( timer_status != FTM_TIMER_INSTALL_DONE )
        {
          /* Flag success as failure */
          ret_val = FTM_CALIBRATION_ISR_SCH_FAILURE;
        }
        else
        {
          /* Flag success as return value */
          ret_val = FTM_CALIBRATION_ISR_SCHEDULED;
        }
  
        /* Based on the last_sweep update the tear down flag */
        if ( cal_pkt->packet_header.last_sweep == 1 )
        {
          cal_data->cal_config.tear_down_radio = TRUE;
        }
        else
        {
          cal_data->cal_config.tear_down_radio = FALSE;
        }
  
        /* Throw Debug messages */
        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_configure_sweep : "
                             "ISR Scheduled" );
        } /* if ( cal_data->debug_mode_en == TRUE ) */
      }
      else
      {
        /* Flag the error code as invalid callback */
        ret_val = (ftm_calibration_config_error_code_type) FTM_CALIBRATION_RESULTS_ALLOC_FAILURE;
  
        /* Throw Debug messages */
        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_configure_sweep : "
                             "Result buffer allocation failed" );
        } /* if ( cal_data->debug_mode_en == TRUE ) */
      }
    }
  
    /* If the Packet Node allocation and failed due any reason, then delete
    the entire linked list. Once the tools side receives this error code, then
    entire process should be restarted. */
    if ( ret_val < 0 )
    {
      /* De-initialize Calibration Sweep */
      ftm_calibration_deinit_sweep();
    }
  } /* if NOT ( cal_data->is_sweep_active = TRUE ) */
  
  /* return the status */
  return ret_val;

} /* calibration_configure_sweep */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Calibration Sweep

  @details
  This function will configure the Calibration v3 based on the data
  segments received from PC side. This function can be called multiple times
  before triggering the ISR to start calibration. During each call, this
  function will store the configuration parameter in a dynamically allocated
  memory. Each consecutive configuration parameter will be linked together
  using a linked list.

  All the memory locations that are dynamically allocated will be freed up at
  the end of calibration sweep.

  This function should not be called again, after "CALIBRATION START" is sent
  and before the response packet is reveived by the tools. If the function
  is called in between, then the integrity of configuration data cannot be
  guaranteed resulting is possible calibration failure.

  @param ftm_req_data
  Pointer to the RF Calibration Configruation message. This will be the pointer
  of the FTM packet. This does not include the Diag header. The parameter 
  structure can be seen in the following illustration.

  @return
  Reponse Packet for configuration of Calibration sweep
*/
ftm_rsp_pkt_type
ftm_calibration_configure_sweep
(
  void *ftm_req_data
)
{
  /* Request Packet */
  ftm_cal_sweep_config_req_pkt_type* header =
    (ftm_cal_sweep_config_req_pkt_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cal_sweep_config_rsp_pkt_type *rsp_pkt; /*Packet Payload for each cmd*/

  /* Other defines */
  ftm_calibration_config_error_code_type ret_val; /* Return value */
  ftm_calibration_data_type *cal_data; /* Calv3 Data */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                                 sizeof(ftm_cal_sweep_config_rsp_pkt_type) ); 

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_cal_sweep_config_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  if (rsp_pkt == NULL  )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_configure_sweep : Failed "
                        "during Payload malloc" );
    return ftm_rsp_data;
  } /* if (rsp_pkt == NULL  ) */

  /*  NULL pointer check for calv3 data container */
  if (cal_data == NULL )
  {
     ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
     FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_configure_sweep : NULL Calv3 "
                         "Data" );
     return ftm_rsp_data;
  } /* if (cal_data == NULL ) */

  ret_val = calibration_configure_sweep( header, cal_data );
  
  /* Update the Response Packet Field */
  rsp_pkt->cfg_sweep_status = ret_val;

  /* Send back a BAD response packet for failures */
  if ( ret_val < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;

    /* Throw Debug messages */
    FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_configure_sweep : Failed " );
  }
  else
  {
    /* Throw Debug messages */
    FTM_MSG_1( FTM_HIGH, "Cal v3 ftm_calibration_configure_sweep : [Done] "
                         "ResultRevision %d", cal_data->result.result_rev );
  }

  /* Return */
  return ftm_rsp_data;

} /* ftm_calibration_configure_sweep */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Calibration Sweep based on a compressed payload

  @details
  This function has same functionality as ftm_calibration_configure_sweep
  with the difference being that the payload is compressed. Note the header
  and footer are not compressed.
 
  The function will handle setting the correct request instruction size after
  uncompressing the request data.
 
  Known Limitation: Only supports single compressed request packet.

  @param ftm_req_data
  Pointer to the RF Calibration FTM packet. This includes the Diag header.

  @return
  Reponse Packet for configuration of Calibration sweep
*/
ftm_rsp_pkt_type
ftm_calibration_configure_sweep_compressed
(
  void *ftm_req_data
)
{
  /* Packet Uncompression Status */
  int8 uncompress_status = Z_ERRNO;
  /* Size after decompression*/
  uint32 uncompress_size=FTM_CAL_SWEEP_V3_PACKET_SIZE_MAX 
                         - FTM_CAL_SWEEP_V3_PACKET_HDR_FTR;
  /* Uncompressed Packet */
  ftm_common_pkt_type *uncompressed_pkt = NULL;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cal_sweep_config_rsp_pkt_type *rsp_pkt; /*Packet Payload for each cmd*/

  /* Access Pointers */
  ftm_cal_sweep_config_req_pkt_type*  uncmp_req_ptr;
  ftm_cal_sweep_config_req_pkt_type*  orig_req_ptr;
  ftm_calibration_config_packet_type* uncmp_cal_ptr;
  ftm_calibration_config_packet_type* orig_cal_ptr;
  //uint16* uncmp_crc_ptr;
  //uint16* orig_crc_ptr;

  /* Other defines */
  ftm_calibration_config_error_code_type ret_val; /* Return value */

  ftm_calibration_data_type *cal_data; /* Calv3 Data */

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                                 sizeof(ftm_cal_sweep_config_rsp_pkt_type) ); 

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_cal_sweep_config_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  if (rsp_pkt == NULL  )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_configure_sweep : Failed "
                        "during Payload malloc" );
    return ftm_rsp_data;
  } /* if (rsp_pkt == NULL  ) */

  uncompressed_pkt = (ftm_common_pkt_type *)modem_mem_alloc(
                         FTM_CAL_SWEEP_V3_PACKET_SIZE_MAX, MODEM_MEM_CLIENT_RFA);

  if (uncompressed_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_configure_sweep : Failed "
                        "During Payload Uncompression Malloc" );
    return ftm_rsp_data;
  } /* if (uncompressed_pkt == NULL) */

  FTM_MSG( FTM_HIGH, "ftm_calibration_configure_sweep_compressed : uncmp buffer created");

  /*Access Pointers Setup */
  uncmp_req_ptr  = (ftm_cal_sweep_config_req_pkt_type*)uncompressed_pkt;
  orig_req_ptr   = (ftm_cal_sweep_config_req_pkt_type*)ftm_req_data;
  uncmp_cal_ptr = (ftm_calibration_config_packet_type*)(&(uncmp_req_ptr->cal_param));
  orig_cal_ptr   = (ftm_calibration_config_packet_type*)(&(orig_req_ptr->cal_param));


  /* Assemble uncompressed packet new packet */ 
  uncmp_req_ptr->cmd               =  orig_req_ptr->cmd;
  uncmp_req_ptr->sub_sys           =  orig_req_ptr->sub_sys;
  uncmp_req_ptr->mode              =  orig_req_ptr->mode;
  uncmp_req_ptr->cdma_cmd          =  orig_req_ptr->cdma_cmd;
  uncmp_req_ptr->req_len           =  orig_req_ptr->req_len;
  uncmp_req_ptr->rsp_len           =  orig_req_ptr->rsp_len;
  uncmp_cal_ptr->packet_header.start_cal_cmd    = 
     orig_cal_ptr->packet_header.start_cal_cmd;
  uncmp_cal_ptr->packet_header.last_sweep       = 
     orig_cal_ptr->packet_header.last_sweep;
  uncmp_cal_ptr->packet_header.res_rev_dbg_mode = 
     orig_cal_ptr->packet_header.res_rev_dbg_mode;
  uncmp_cal_ptr->packet_header.num_of_segments  = 
     orig_cal_ptr->packet_header.num_of_segments;
  uncmp_cal_ptr->packet_header.cal_data_len     = 
     orig_cal_ptr->packet_header.cal_data_len;

  /* Decompress FTM Payload Section */
  uncompress_status = uncompress((uint8 *)(&uncmp_cal_ptr->instr_ptr),   
                                 &uncompress_size,
                                 (const uint8 *)(&orig_cal_ptr->instr_ptr), 
                                 (uint32)orig_cal_ptr->packet_header.tot_instr_size);

  if (uncompress_status != Z_OK)
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_configure_sweep : Failed "
                        "During Payload Uncompression Malloc" );
    ret_val = FTM_CALIBRATION_UNCOMPRESSION_FAILURE;
  }
  else
  {
     FTM_MSG( FTM_HIGH, "ftm_calibration_configure_sweep_compressed : uncmp done");
     FTM_MSG_1(FTM_HIGH, "uncompression stats : compressed size : %d",orig_cal_ptr->packet_header.tot_instr_size);
     FTM_MSG_1(FTM_HIGH, "uncompression stats : uncompressed size : %d",uncompress_size);

     /* Set Instruction Size After Uncompression */
     uncmp_cal_ptr->packet_header.tot_instr_size   = uncompress_size;

     /* Call down to sweep function */
     ret_val = calibration_configure_sweep( (ftm_cal_sweep_config_req_pkt_type*)uncompressed_pkt, 
                                            cal_data );
  }

  /* Update the Response Packet Field */
  rsp_pkt->cfg_sweep_status = ret_val;

  /* Send back a BAD response packet for failures */
  if ( ret_val < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;

    /* Throw Debug messages */
    FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_configure_sweep : Failed " );
  }
  else
  {
    /* Throw Debug messages */
    FTM_MSG_1( FTM_HIGH, "Cal v3 ftm_calibration_configure_sweep : [Done] "
                         "ResultRevision %d", cal_data->result.result_rev );
  }

  /* uncompressed packet has been used, so free it */
  modem_mem_free(uncompressed_pkt,MODEM_MEM_CLIENT_RFA);

  /* Return */
  return ftm_rsp_data;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Calibration Sweep Results

  @details
  This function can be called multiple times from PC to request for the
  calibration results which were collected during Calibration Sweep.
 
  Based on the maximum allowed response size by Diag, this function will
  fetch calibration results from the result buffer and form a response packet.
  If all results from result buffer is sent to PC, then this function will also
  send a flag indicating that the result buffer is empty. On the other hand,
  if all results are not yet sent, then this function will send a flag 
  indicating that the reult buffer is not empty. Based on these information, PC 
  will decide whether to send new Request packet for results.
 
  This function is expected to be called anytime. Thus if PC sends a command to
  get results while embedded side is still doing ISR sweep for calibration, then
  this function sends back an error code indicating that result buffer read
  is disabled.
 
  While sending back results back to PC, this function will deallocate the
  result buffer as well, if all result transfer is completed.

  @param ftm_req_data
  Pointer to the RF Calibration "Get Result" message. This will be the pointer
  of the FTM packet. This does not include the Diag header. The parameter 
  structure can be seen in the following illustration.

  @return
  Response packet to the request for Fetch sweep results
*/
ftm_rsp_pkt_type
ftm_calibration_fetch_sweep_results
(
  void *ftm_req_data
)
{
  int16 diag_limit = 0; /* Diag Limitataion in bytes */
  int16 result_len = 0; /* Length of calibration results to be sent */
  int16 unread_buff_size = 0; /* Unread Result Buffer Sizer */
  ftm_calibration_data_type *cal_data; /* Calv3 Data */
  uint8 result_query_count = 0; /* Local copy of result query count */

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cal_sweep_get_results_rsp_pkt_type *rsp_pkt; /*Packet Payload*/

  /* Get Pointer to Cal V3 Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  /* Increment the result quer count */
  cal_data->result.query_num++;

  /* Store the local copy of result query count */
  result_query_count = cal_data->result.query_num;

  if ( cal_data->result.buff_read_en == TRUE )
  {
    /* For Off-target Testing, set the Diag Limit to very low number so
    that multiple result request testing can be done. */
    #ifdef T_WINNT
    #error code not present
#else
    /* Get Diag Limitation on Max size of Response packet that can be sent */
    diag_limit = 4096;
  
    /* Allow some margin to Diag Limitation. Thus backoff the diag_limit with
    some margin. This margin is used so that cal results data take up
    memeory less than Diag Limit leaving room for response packet overheads. */
    diag_limit = diag_limit - FTM_CAL_SWEEP_DIAG_LIMIT_BACKOFF;
    #endif /* T_WINNT */
  
    /* Calculate unread buffer size */
    unread_buff_size = cal_data->result.buff_written - 
                                       cal_data->result.buff_read;

    /* The length of result to be sent should be the lower one between the Diag 
    Limit and the total unread result buffer. If the length of result is
    less than zero, then the final result len ceil'ed up to zero. */
    result_len = rf_max_s16(rf_min_s16( diag_limit, unread_buff_size ), 0);
  
    /* Create a new reponse packet in the ftm heap. This will also update first 
    10-bytes field in Packet Payload in the FTM response packet. These fields 
    are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
    Request Packet Length (2), and Response packet Lenth (2). */
    ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
      FPOS( ftm_cal_sweep_get_results_rsp_pkt_type, cal_result ) + result_len); 

    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    rsp_pkt = (ftm_cal_sweep_get_results_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

    /* Send back a BAD response packet for failures */
    if ( rsp_pkt == NULL)
    {
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
      FTM_MSG_1( FTM_HIGH, "[calv3][FETCH_RESULT][%d] : Bad Response", 
                 result_query_count);
      return ftm_rsp_data;
    }

    /* Update the Segment Error Code */
    rsp_pkt->seg_error_code =
      cal_data->status.seg_error_log.first_seg_error;

    /* If the seg_error_code is non-zero, populate the faulty segment number
    as well. */
    if ( rsp_pkt->seg_error_code != 0 )
    {
      rsp_pkt->faulty_segment_num = 
        cal_data->status.seg_error_log.first_error_seg_num;
    } /* if ( rsp_pkt->seg_error_code != 0 ) */
    else
    {
      rsp_pkt->faulty_segment_num = 0;
    } /* if ( rsp_pkt->seg_error_code != 0 ) */

    /* Proceed only if the unread buff_size is greater thean zero */
    if ( unread_buff_size > 0 )
    {
      if ( cal_data->cal_config.debug_mode_en == TRUE )
      {
        FTM_MSG_2( FTM_HIGH, "[calv3][FETCH_RESULT][%d] "
                   "RESULT TRAVERSE : [Start 0x%8x ]", 
                   result_query_count, cal_data->result.read_ptr );
      } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

      /* Copy the Chunk of result from result buffer to the response packet
      payload */
      memscpy( &(rsp_pkt->cal_result), result_len,
              cal_data->result.read_ptr , result_len );
      
      /* Update the read pointer and the total buffer read bytes */
      cal_data->result.read_ptr += result_len;
      cal_data->result.buff_read += result_len;

      if ( cal_data->cal_config.debug_mode_en == TRUE )
      {
        FTM_MSG_3( FTM_HIGH, "[calv3][FETCH_RESULT][%d] "
                   "RESULT TRAVERSE : [ + %3d -> 0x%8x ]", 
                   result_query_count, result_len, cal_data->result.read_ptr );
      } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */


      /* Calculate new unread buffer size. This is done to check, if there
      is more data to be read. Based on that empty_result_buffer flag is set;
      so that the PC can decide on whether to sent another request packet
      for results or not. */
      unread_buff_size = cal_data->result.buff_written - 
                                          cal_data->result.buff_read;

      /* Update the Response Packet Field */
      rsp_pkt->status = FTM_CALIBRATION_RESULTS_READ_SUCCESS;
      rsp_pkt->cal_result_len = result_len;

      /* Based on current unread buffer, indicate to PC whether the result 
      buffer is empty or not */
      if ( unread_buff_size > 0 )
      {
        rsp_pkt->empty_result_buffer = 0;

        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_HIGH, "[calv3][FETCH_RESULT][%d] : Result Buffer has "
                     "unflushed Data", result_query_count );
        } /* if ( cal_data->debug_mode_en == TRUE ) */
      }
      else
      {
        /* Result buffer is empty */
        rsp_pkt->empty_result_buffer = 1;

        /* Deallocate Result buffer */
        ftm_calibration_deinit_result_buffer();

        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_HIGH, "[calv3][FETCH_RESULT][%d] : Result Buffer "
                     "flushed all data, thus deinit'ing", result_query_count );
        } /* if ( cal_data->debug_mode_en == TRUE ) */
      }
    } /* if ( unread_buff_size > 0 ) */
    else
    {
      /* Update the Response Packet Field */
      rsp_pkt->status = FTM_CALIBRATION_RESULTS_EMPTY_BUFFER;
      rsp_pkt->cal_result_len = 0;
      rsp_pkt->empty_result_buffer = 1;

      if ( cal_data->cal_config.debug_mode_en == TRUE )
      {
        FTM_MSG_1( FTM_HIGH, "[calv3][FETCH_RESULT][%d] : Empty Results Buffer",
                   result_query_count);
      } /* if ( cal_data->debug_mode_en == TRUE ) */
    } /* if ( unread_buff_size > 0 ) */
  } /* if ( cal_data->result.buff_read_en == TRUE ) */
  else
  {
    /* Create a new reponse packet in the ftm heap. This will also update first 
    10-bytes field in Packet Payload in the FTM response packet. These fields 
    are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
    Request Packet Length (2), and Response packet Lenth (2). */
    ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_cal_sweep_get_results_rsp_pkt_type)); 

    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    rsp_pkt = (ftm_cal_sweep_get_results_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

    if (rsp_pkt == NULL )
    {
      ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
      FTM_MSG_1( FTM_ERROR, "[calv3][FETCH_RESULT][%d] : rsp_pkt is NULL ",
                 result_query_count );
      return ftm_rsp_data;
    } /* if (rsp_pkt == NULL ) */

    /* Update the Response Packet Field */
    rsp_pkt->status = FTM_CALIBRATION_RESULTS_READ_DISABLED;
    rsp_pkt->empty_result_buffer = 0;
    rsp_pkt->cal_result_len = 0;

    if ( cal_data->cal_config.debug_mode_en == TRUE )
    {
      FTM_MSG_1( FTM_HIGH, "[calv3][FETCH_RESULT][%d] : Read Disabled", 
                 result_query_count);
    } /* if ( cal_data->debug_mode_en == TRUE ) */
  } /* if ( cal_data->result.buff_read_en == TRUE ) */

  /* Send back a BAD response packet for failures */
  if ( ( rsp_pkt->status < 0 ) && 
      ( rsp_pkt->status != FTM_CALIBRATION_RESULTS_READ_DISABLED ) )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_HIGH, "[calv3][FETCH_RESULT][%d] : "
             "Bad Response", result_query_count);
  }
  else
  {
    FTM_MSG_1( FTM_HIGH, "[calv3][FETCH_RESULT][%d] : "
               "Result Set Sent Back", 
               result_query_count );
  }
  
  /* Return */
  return ftm_rsp_data;

} /* ftm_calibration_fetch_sweep_results */

/*! \} */


/*============================================================================*/
/*!
  @name Debug Functions

  @brief
  This section contains functions for V3 Calibration function used to control
  debug mode
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set debug level
 
  @details
  This function is used to set the calv3 f3 message debug level.
 
  @param dbg_lvl
  Enum indicating the debug level
*/
void
ftm_calibration_set_debug_level
(
  ftm_cal_debug_level_type dbg_lvl,
  boolean active_flag
)
{
  ftm_calibration_data_type *cal_data; /* Calibration Sweep Data */

  /* Get Calibration Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  if ( active_flag == TRUE )
  {
    cal_data->cal_config.dbg_level |= dbg_lvl;
  }
  else
  {
    cal_data->cal_config.dbg_level &= (~dbg_lvl);
  }

  return;

} /* ftm_calibration_set_debug_level */


/*! \} */


/*============================================================================*/
/*!
  @name Common Helper Functions

  @brief
  This section contains functions for V3 Calibration function to convert 
  device flag to RFM enumeration.
 
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert device flag to RFM device enum
 
  @param device_flag
  Device flag from sweep config
 
  @return
  RFM device enum, RFM_INVALID_DEVICE if device is not supported
*/
rfm_device_enum_type 
ftm_calibration_dev_flag_to_rfm_dev 
(
   uint32 device_flag
)
{
  rfm_device_enum_type rfm_device = RFM_INVALID_DEVICE;

  switch ( device_flag )
  {

  case DEVICE_FLAG_0:
    rfm_device = RFM_DEVICE_0;
    break;

  case DEVICE_FLAG_1:
    rfm_device = RFM_DEVICE_1;
    break;

  case DEVICE_FLAG_2:
    rfm_device = RFM_DEVICE_2;
    break;

  case DEVICE_FLAG_3:
    rfm_device = RFM_DEVICE_3;
    break;

  case DEVICE_FLAG_4:
    rfm_device = RFM_DEVICE_4;
    break;

  default:
    rfm_device = RFM_INVALID_DEVICE;
    break;

  } /* switch ( device_flag ) */

  return rfm_device;
}

/*! \} */


#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
/*! @} */


