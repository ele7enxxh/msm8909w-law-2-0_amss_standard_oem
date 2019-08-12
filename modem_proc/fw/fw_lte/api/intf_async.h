/*!
  @file
  intf_async.h

  @brief
  This file contains lte LL async command id's and parameters structures

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2008-09 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_async.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_ASYNC_H
#define INTF_ASYNC_H

#include "intf_common.h"
#include "intf_async_msg.h"
#include "msgr.h"
#include "hw_mem_pool_buf.h"
#include "hw_decob_buf.h"
#include "lte_LL1_ue_types.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

// --------------------------------------------------------------------------
//
// enums
//
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//
// Start sys time
//
// --------------------------------------------------------------------------

typedef enum
{
  LTE_LL1_WB_SAMP_RATE_30_72 = 0,     ///< for 20MHz & 15MHz BW
  LTE_LL1_WB_SAMP_RATE_15_36 = 1,     ///< for 10MHz BW
  LTE_LL1_WB_SAMP_RATE_7_68  = 2,     ///< for 5MHz BW
  LTE_LL1_WB_SAMP_RATE_NONE  = 255    ///< for no WB sampling
  
} lte_LL1_wb_samp_rate_e;

/*! @brief  Start sys time request message structure.
            This request is issued to start LL1's 1ms system time
            tick.  After system time has started, subframe message
            processing will occur every 1ms.
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;            ///< common request header
  int64                       start_ref_time;     ///< starting 64-bit reference time
  int16                       start_frame_num;    ///< starting 10ms radio frame #
  int8                        start_subframe_num; ///< starting subframe #
  boolean                     enable_rtc;         ///< enable RTC flag
  lte_LL1_wb_samp_rate_e      wb_samp_rate;       ///< WB sampling rate
} lte_LL1_async_start_sys_time_req_struct;

LTE_LL1_INTF_MSG( async_start_sys_time_req );



/*! @brief  Start sys time confirmation message structure.
            This confirmation is sent to indicate start sys time
            request has been processed. 
*/
typedef struct {                      
  
  lte_LL1_cnf_header_struct cnf_hdr;              ///< common cnf header
  
} lte_LL1_async_start_sys_time_cnf_struct;

LTE_LL1_INTF_MSG( async_start_sys_time_cnf );



// --------------------------------------------------------------------------
//
// Read HW mem
//
// --------------------------------------------------------------------------
// enums for access decob
typedef enum
{
  LTE_LL1_HW_MEM_DECOB_PBCH_DATA,
  LTE_LL1_HW_MEM_DECOB_PBCH_STATUS,
  LTE_LL1_HW_MEM_DECOB_PDCCH_DATA,
  LTE_LL1_HW_MEM_DECOB_PDCCH_STATUS,
  LTE_LL1_HW_MEM_DECOB_PDSCH_DATA,
  LTE_LL1_HW_MEM_DECOB_PDSCH_STATUS,
} lte_LL1_hw_mem_decob_type_e;

// enums for access llrb
typedef enum
{
  LTE_LL1_HW_MEM_LLRB_PDCCH,
  LTE_LL1_HW_MEM_LLRB_PBCH,
  LTE_LL1_HW_MEM_LLRB_PDSCH_CURR,
  LTE_LL1_HW_MEM_LLRB_PDSCH_UNI_HARQ,
  LTE_LL1_HW_MEM_LLRB_PDSCH_SI_HARQ
} lte_LL1_hw_mem_llrb_type_e;

typedef enum
{
  LTE_LL1_HW_MEM_TYPE_MEM_POOL,
  LTE_LL1_HW_MEM_TYPE_LLRB,
  LTE_LL1_HW_MEM_TYPE_ENC_IB,
  LTE_LL1_HW_MEM_TYPE_DEC_OB,
  LTE_LL1_HW_MEM_TYPE_ENC_OB,
  LTE_LL1_HW_MEM_TYPE_CPIW
} lte_LL1_hw_mem_type_e;

#define LTE_LL1_ASYNC_READ_WRITE_SIZE   256

/*! @brief  Read HW mem request message structure.
            This request is issued to read specified memory.
            LL1 should be paused in order for this request to
            return meaning data.
            
            Use of this request during normal FW operation will 
            result in unpredictable behavior.
            
            Note: this request has not been implemented.

*/
typedef struct
{
  lte_LL1_req_header_struct req_hdr;      ///< common request header
  
  lte_LL1_hw_mem_type_e     mem_type;     ///< HW memory type
  
  uint8                     buf_id;       ///< buffer id within specified
                                          ///< memory type. For memory pool,
                                          ///< hw_mem_pool_buf_enum is used as
                                          ///< buf_id.
                                          ///< For Tx ENC HARQ IB, it refers to the
                                          ///< HARQ bank index.
                                          
  uint8                     sub_buf_id;   ///< sub-buffer id (for memory pool,
                                          ///< this refers to rx ant)

  uint8                     tag;          ///< Data / Status indication for LLRB
                                          ///< CP type indication for WMORE CPIW buffer

  uint16                    start_offset; ///< starting offset for memory 
                                          ///< read

  uint16                    read_size;    ///< # values read (1 ~ 
                                          ///< LTE_LL1_ASYNC_READ_WRITE_SIZE)  
                         
} lte_LL1_async_read_hw_mem_req_struct;

LTE_LL1_INTF_MSG( async_read_hw_mem_req );



/*! @brief  Read HW mem confirmation message structure.
            This message contains read data from read he mem request.
*/
typedef struct {                      
  
  lte_LL1_cnf_header_struct cnf_hdr;      ///< common cnf header
  
  uint32                    data[LTE_LL1_ASYNC_READ_WRITE_SIZE];    ///< read data   
  
} lte_LL1_async_read_hw_mem_cnf_struct;

LTE_LL1_INTF_MSG( async_read_hw_mem_cnf );



// --------------------------------------------------------------------------
//
// Write HW mem
//
// --------------------------------------------------------------------------
/*! @brief  Write HW mem request message structure.
            This request is issued to write to specified memory.
            This request is intended for use in testing only.
            
            Use of this request during normal FW operation will 
            result in unpredictable behavior.

            Note: this request has not been implemented.
            
*/
typedef struct
{
  lte_LL1_req_header_struct req_hdr;      ///< common request header
  
  lte_LL1_hw_mem_type_e     mem_type;     ///< HW memory type
  
  uint8                     buf_id;       ///< buffer id within specified
                                          ///< memory type.                   
                                          
  uint8                     sub_buf_id;   ///< sub-buffer id (for memory pool,
                                          ///< this refers to rx ant)

  uint16                    start_offset; ///< starting offset for memory 
                                          ///< read

  uint16                    write_size;   ///< # values read (1 ~ 
                                          ///< LTE_LL1_ASYNC_READ_WRITE_SIZE)  
  
  uint32                    data[LTE_LL1_ASYNC_READ_WRITE_SIZE];      
                         
} lte_LL1_async_write_hw_mem_req_struct;

LTE_LL1_INTF_MSG( async_write_hw_mem_req );



/*! @brief  Read HW mem confirmation message structure.
            This message contains read data from read he mem request.
*/
typedef struct {                      
  
  lte_LL1_cnf_header_struct cnf_hdr;      ///< common cnf header
    
} lte_LL1_async_write_hw_mem_cnf_struct;

LTE_LL1_INTF_MSG( async_write_hw_mem_cnf );



// --------------------------------------------------------------------------
//
// Reset
//     
// --------------------------------------------------------------------------
/*! @brief  Reset request message structure.
            This request will reset FW and HW states.  It should only be
            issued when system time has stopped.  
                        
            Confirmation message will be sent once the reseting has completed.
            
            Not currently support            
*/
typedef struct {                      

  lte_LL1_req_header_struct req_hdr;    ///< common req header
      
} lte_LL1_async_reset_req_struct;

LTE_LL1_INTF_MSG( async_reset_req );


/*! @brief  Reset confirmation message structure.
            
*/
typedef struct {                      
  
  lte_LL1_cnf_header_struct cnf_hdr;      ///< common cnf header
    
} lte_LL1_async_reset_cnf_struct;

LTE_LL1_INTF_MSG( async_reset_cnf );


// --------------------------------------------------------------------------
//
// DOG timeout
//
// --------------------------------------------------------------------------
/*! @brief  Message structure for DOG timeout request message
            
            Upon receiving this request, we fatal error as ML1 has indicated
            that the watchdog has timed out
            
*/
typedef struct {                      

  lte_LL1_req_header_struct req_hdr;                            
} lte_LL1_async_dog_timeout_req_struct;

LTE_LL1_INTF_MSG( async_dog_timeout_req );

// --------------------------------------------------------------------------
//
// TTRANS_WAKEUP, timing transfer wakeup (use other tech's stmr to restore LTE stmr)
// last good snapshot is stored in FW, there is no need for SW to send it
// (do not use the FW stored, use SW sent, 06/14/2011)
// this snapshot info is also sent to SW for other purpose
// the very first time use SYNC_STMR_DUMP to get snapshot
// then the following TTRANS_WAKEUP double as SYNC_STMR_DUMP
//
// --------------------------------------------------------------------------
/*! @brief  timing transfer wakeup, use other tech's stmr to restore LTE stmr
            1. start LTE o_stmr (with invalid value)
            2. use other tech's stmr to restore o_stmr (can skip)
            3. start INT
*/
typedef struct {

  lte_LL1_req_header_struct req_hdr;    ///< common req header

  lte_LL1_stmr_snapshot_s stmr_snapshot;     ///< use SW stored, not FW stored, because SW may translate

} lte_LL1_async_ttrans_wakeup_req_struct;

LTE_LL1_INTF_MSG( async_ttrans_wakeup_req );


/*! @brief  CNF, return sync_stmr_dump data for next TTRANS_WAKEUP
            These 2 lines are moved to intf_sys.h for compile
*/
// typedef lte_LL1_sys_sync_stmr_dump_cnf_struct lte_LL1_async_ttrans_wakeup_cnf_struct;

// LTE_LL1_INTF_MSG( async_ttrans_wakeup_cnf );

// --------------------------------------------------------------------------
//
// Stop rtc
//
// will remove
// --------------------------------------------------------------------------

/*! @brief parameters for stop rtc request
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;              ///< request header
} lte_LL1_async_stop_req_struct;

typedef struct {
  msgr_hdr_struct_type                msg_hdr;      ///< message router header
  lte_LL1_async_stop_req_struct msg_payload;
} lte_LL1_async_stop_req_msg_struct;


/*! @brief parameter structure for stop confirmation
*/
typedef struct {                      
  
  lte_LL1_cnf_header_struct cnf_hdr;
  
} lte_LL1_async_stop_cnf_struct;



// --------------------------------------------------------------------------
//
// run DAC Calibration through async message request when in FTM call, during
// continuous TX calibration waveform transmission
//
// --------------------------------------------------------------------------
typedef struct {
  msgr_hdr_struct_type                            msg_hdr;   ///< message router header
  lte_LL1_async_stop_cnf_struct                   msg_payload; 
} lte_LL1_async_stop_cnf_msg_struct;


/*! @brief async DAC Cal confirmation structure

   This confirmation will be sent when DAC Calibration Result is ready.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;

   boolean is_dac_caled;
} lte_LL1_async_dac_cal_cnf_struct;

LTE_LL1_INTF_MSG( async_dac_cal_cnf );
 
/*! @brief  Message structure for DAC CAL request message
            
            When received, DAC Cal (HW interface) will be called to 
            take effect immediately.
            
            Confirmation to the request is sent after completion of the 
            script.
            
 			The DAC Cal should be issued only during ul FTM calibration
			transmission period.
            
*/
typedef struct {                      

  lte_LL1_req_header_struct req_hdr;
  
  /*! @brief Whether to do dac cal on ant 0 */
  uint16 dacCalAnt0: 1;       /* Bit 0 */
  /*! @brief Whether to do dac cal on ant 1 */
  uint16 dacCalAnt1: 1;       /* Bit 1 */
  uint16 : 14;                /* Bits 15:2 */

} lte_LL1_async_dac_cal_req_struct;

LTE_LL1_INTF_MSG( async_dac_cal_req );

/*! 
  Payload for the message used to increase LTE FW priority to protect LTE from
  the TX of other techs. The way to use it is to specify a starting ustmr time 
  and duration ustmr for the regions were an increase in LTE cxm
  priority is desired. If the start ustmr time is not specified the priority is
  increased at the current ustmr time + 1.5ms (CXM requirement for
  registration). Note if a start time is not specified the current ustmr time is
  read to determine the registration time, therefore, in order to use this mode
  ustmr must be enabled.
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  /*! The registration for high priority is raised from this ustmr value. */
  uint32 raise_prio_start_time_ustmr;

  /*! The duration for the high priority registration  */
  uint32 raise_prio_duration_ustmr;

  /*! cxm params used for registration */
  lte_LL1_change_cxm_params_struct cxm_params;

}lte_LL1_async_multi_rat_raise_cxm_prio_req_struct;

LTE_LL1_INTF_MSG( async_multi_rat_raise_cxm_prio_req );

#endif /* INTF_ASYNC_H */
