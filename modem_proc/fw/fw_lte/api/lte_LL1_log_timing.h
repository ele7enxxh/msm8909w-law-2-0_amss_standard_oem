/*!
  @file
  lte_LL1_log_timing.h

  @brief
  timing log packet data structures

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Fri Feb 12 16:25:11 2010.
  Command line used: perl w2h.pl log_timing timing 1
   Note: The params, [1], correspond to version numbers for
         each log message in the listed order in the ICD

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_timing.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_TIMING_H
#define LTE_LL1_LOG_TIMING_H


/*==========================================================================*/
/*! @brief
 * 8 LTE LL1 Serving Cell Frame Timing (Log code ID: 0xB114)
 *
 *  This log packet provides information about downlink and uplink timing. When 
 *  enabled, the log record is generated at least once every 20 subframes.  If 
 *  change in frame timing is outside of the timing adjustment range,  a new 
 *  log is generated. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_TIMING_SERVING_CELL_FRAME_TIMING_VERSION 1

/*! @brief serving_cell_frame_timing main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 1 for the log packet structure
                                                         ///< described below
  uint32 number_of_records                : 5;  ///< Range: 1-20.  Each record correspond to a
                                                         ///< subframe.
  uint32 starting_sub_frame_number        : 4;  ///< Sub-frame number (Range 0..9)
  uint32 starting_system_frame_number     : 10;  ///< System frame number (range 0..1023)
  uint32 reserved0                        : 5; 
  uint32 starting_dl_frame_timing_offset  : 19;  ///< Downlink frame timing offset (in Ts units)
                                                         ///< Range 0-307200
  uint32 reserved1                        : 13; 
  uint32 starting_ul_frame_timing_offset  : 19;  ///< Uplink frame timing offset (in Ts units)
                                                         ///< Range 0-307200
  uint32 starting_ul_timing_advance       : 11;  ///< Starting NTA value (in units of 16 Ts)
  uint32 reserved2                        : 2; 
   uint8 timing_adjustment[60]                        ;  ///< (20x24) bits
                                                         ///< Packed array of 24-bit adjustment records.
                                                         ///< Format of each record is:
                                                         ///< Bits 10:0 = change in DL frame timing (in Ts
                                                         ///< units), Range = -512 .. 511; bit10 is sign bit.
                                                         ///< Bits 15:11 = change in UL frame timing (in Ts
                                                         ///< units), Range = -16..15; bit15 is sign bit.
                                                         ///< Bits 23:16 = change in timing advance (in 16 Ts
                                                         ///< units) Range = -128..127; bit23 is sign bit.
}lte_LL1_log_timing_serving_cell_frame_timing_ind_struct;



#endif //LTE_LL1_LOG_TIMING_H
