/*!
  @file
  ftm_calibration_v3_opcode.c

  @brief
  This module contains common code to handle OpCode processing

  @addtogroup RF_COMMON_FTM_CALV3_OPCODE
 
  @image html ftm_calibration_sweep_opcode.jpg
  @image html ftm_calibration_sweep_opcode_fetching.jpg
 
  @brief
  This module provides a extensions to code to handle OpCode processing
  @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_opcode.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/18/14    xsh  Fix Compiler warning
05/26/14    id     Fix for FBRx DC Cal functionality with Opcode
10/16/13   aro     LLVM compiler compatibility
08/02/13    dj     Moved Feedback Setup to be the first opcode to be serviced
                   This is required for cases where FB and DRx paths are on 
                   different devices
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/13/13   aro     OpCode Fetching illustration
04/19/13    dj     Support for Feedback Setup Opcode
04/16/13   aro     Variable/type/prototype cleanup
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/03/13   aro     Off-target compilation fix
04/02/13   aro     Migrated to inline Math functions
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/28/13   aro     Added results publishing and pushing interfaces
03/27/13   aro     Data structure restructing
03/27/13   aro     Added fetch opcode function
03/27/13   aro     OpCode unpack function
03/27/13   aro     Initial support for opcode sequencing
02/21/13   aro     F3 update
12/07/12   aro     Added 2-bytes reads for OpCode
12/05/12   aro     Fixed the the AND'ing to be bitwise AND
11/20/12   aro     Fixed the error in extarction of 2MSB from variable
                   length OpCode
11/15/12   aro     Mainlined enabling of variable length OpCode
11/15/12   aro     Converted 8byte pointer to byte pointer to fix the
                   exception generated due to loading to misaligned address
11/05/12   aro     Re-organized cal v3 data structure to be more modular
10/29/12   aro     Appended opcode illustration
10/25/12   aro     Compiler warning fix
10/15/12   aro     Interface to query Opcode length
10/22/12   aro     Update OpCode length calculation to be done in terms of
                   number of bytes
10/22/12   aro     Updated interface to detect opcode bit to pass the
                   opcode data from calling function
10/19/12   aro     Interface to check opcode bit
10/18/12   aro     Added opcode data deinit function
10/18/12   aro     Converted size query function to opcode init function
10/18/12   aro     Interface to query the size of opcode from current segment
10/18/12   aro     Initial 

==============================================================================*/

#include "stringl.h" /* for memscpy */
#include "comdef.h"
#include "ftm_calibration_v3_opcode.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_msg.h"
#include "rfcommon_math.h"

/*----------------------------------------------------------------------------*/
/*! Enumeration for invalid Segment Interval */
#define FTM_CAL_SEGMENT_INTREVAL_INVALID 0

/*----------------------------------------------------------------------------*/
/*! Array indicating the sequence of OpCode execution */
static ftm_cal_opcode_bit_type 
oc_sequence[FTM_CAL_OPCODE_MAX] =
{
  FTM_CAL_OPCODE_FEEDBACK_SETUP,
  FTM_CAL_OPCODE_CONFIG_RADIO,
  FTM_CAL_OPCODE_RETUNE_RADIO,
  FTM_CAL_OPCODE_RADIO_SETUP,
  FTM_CAL_OPCODE_CONFIG_TX_WF,
  FTM_CAL_OPCODE_SYNC,
  FTM_CAL_OPCODE_POLLING,
  FTM_CAL_OPCODE_DATA,
  FTM_CAL_OPCODE_CONFIG_TX,
  FTM_CAL_OPCODE_TX_OVERRIDE,
  FTM_CAL_OPCODE_CONFIG_RX,
  FTM_CAL_OPCODE_MEAS_HDET,
  FTM_CAL_OPCODE_TX_MEASURE,
  FTM_CAL_OPCODE_MEAS_RX,
  FTM_CAL_OPCODE_RX_MEASURE,
  FTM_CAL_OPCODE_XPT_NI,
  FTM_CAL_OPCODE_XPT,
  FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION
};

/*----------------------------------------------------------------------------*/
/*! Enumeration to define the number of opcode chunks */
#define OC_CHUNKS_NUM 4

/*----------------------------------------------------------------------------*/
/*! Structure to hold the chunks of a opcode group */
typedef struct
{
  uint16 oc_chunk[OC_CHUNKS_NUM];
  /*!< Array holding opcode chunks */

} ftm_cal_oc_group_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Append four 16-bit OpCode chunk to form a 64-bit full OpCode group.
 
  @details
  This function will concatenate 4 chunks of 16-bit OpCode to form a 64-bit
  OpCode group.
 
  @param oc_chunks
  Pointer to an array containing opcode chunks
 
  @return
  Appended OpCode group
*/
#ifndef T_WINNT
static inline 
#else
#error code not present
#endif
uint64
ftm_cal_opcode_append_chunks
(
  ftm_cal_oc_group_type oc_group
)
{
  uint64 app_oc = 0;
  uint64 oc_chunk_3 = oc_group.oc_chunk[3];
  uint64 oc_chunk_2 = oc_group.oc_chunk[2];
  uint64 oc_chunk_1 = oc_group.oc_chunk[1];
  uint64 oc_chunk_0 = oc_group.oc_chunk[0];

  app_oc = ( ( oc_chunk_3 << 48 ) |
             ( oc_chunk_2 << 32 ) |
             ( oc_chunk_1 << 16 ) |
             ( oc_chunk_0 ) );

  return app_oc;

} /* ftm_cal_opcode_append_chunks */

/*----------------------------------------------------------------------------*/
/*! Macro to store the current read pointer */
#define INS_TRAV_STORE() ( cal_data->cal_config.instraverse_prev_ptr = \
                           (uint32)cal_data->cal_config.read_node_payload_ptr )

/*----------------------------------------------------------------------------*/
/*! Macro to return the the current payload read pointer r */
#define INS_TRAV_CURR() ( cal_data->cal_config.read_node_payload_ptr )

/*----------------------------------------------------------------------------*/
/*! Macro to return the delta between the current payload read pointer and 
previous read pointer */
#define INS_TRAV_DELTA() ( cal_data->cal_config.read_node_payload_ptr -\
                           cal_data->cal_config.instraverse_prev_ptr )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Requence the OpCode ordering
 
  @details
  This function reserquences the OpCode ordering to follow the sequence defined
  in #oc_sequence
 
  @param oc_data
  Pointer to data structure containing Opcode config data
 
  @param opcode_info
  Pointer to data structure containing OpCode info which is to be
  re-sequenced
 
  @return
  Re-sequenced OpCode info
*/
#ifndef T_WINNT
static inline
#else
#error code not present
#endif
boolean
ftm_calibration_unpack_payload
(
  struct ftm_calibration_data_type_t *cal_data
)
{

  uint64 opcode_temp; /* Variable to store the Opcode for the segment. This
  will be modified in the while loop. */
  uint64 opcode_mask = 0xFFFFFFFFFFFFFFFFLL; 
  ftm_cal_opcode_bit_type opcode_bit;
  ftm_cal_opcode_info_type *opcode_info;
  ftm_cal_opcode_info_type *reseq_oc;
  ftm_cal_opcode_data_type *oc_data;
  uint16 new_location;
  boolean ret_val = TRUE;

  uint8 oc_group_mask;
  uint8 oc_group_temp;
  uint8 oc_group_num;

  reseq_oc = &(cal_data->seg_data.opcode_data.oc_info_reseq);
  opcode_info = &(cal_data->seg_data.opcode_data.oc_info);

  reseq_oc->validity = opcode_info->validity;
  reseq_oc->opcode_size = opcode_info->opcode_size;
  reseq_oc->opcode.oc_g0 = 0;
  reseq_oc->opcode.oc_g1 = 0;
  reseq_oc->opcode.oc_g2 = 0;
  reseq_oc->opcode.oc_g3 = 0;

  oc_data = &(cal_data->cal_config.opcode_data);

  /* Since the variable length OpCode is parsed and stored in chunks of
  64-bit container, loop through all the containers to extract payload
  for each enable opcode bit */

  oc_group_temp = opcode_info->opcode_group_mask;

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */
  oc_group_mask = inline_rf_extract_first_one_bit_mask( oc_group_temp );

  while ( oc_group_mask != 0 )
  {
    /* Find the opcode group number */
    oc_group_num = inline_rf_extract_first_one_bit_lsb_location(oc_group_mask);

    /* Get the OpCode for the current instruction: grp_cnt refers to
    the OpCode group for which the payload is to be extracted */
    switch ( oc_group_num )
    {
    case 0:opcode_temp = opcode_info->opcode.oc_g0;break;
    case 1:opcode_temp = opcode_info->opcode.oc_g1;break;
    case 2:opcode_temp = opcode_info->opcode.oc_g2;break;
    case 3:opcode_temp = opcode_info->opcode.oc_g2;break;
    default:
      FTM_MSG_1( FTM_ERROR, "ftm_calibration_unpack_payload: Invalid"
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

      new_location = oc_data->location_map[opcode_bit].new_loc;
      
      if ( new_location < FTM_CAL_OPCODE_MAX )
      {
        if ( oc_data->dispatch_table[new_location].unpack_fp != NULL )
        {
          cal_data->cal_config.read_node_payload_ptr +=
                  oc_data->dispatch_table[new_location].unpack_fp( cal_data,
                               &(oc_data->dispatch_table[new_location]),
                               cal_data->cal_config.read_node_payload_ptr );
        } /* if ( oc_data->dispatch_table[new_location].unpack_fp != NULL ) */
        else
        {
          ret_val = FALSE;
          FTM_MSG_1( FTM_ERROR, "ftm_calibration_unpack_payload: NULL unpack"
                                "funtion for OC-0x%x", opcode_bit );
        } /* if ! ( oc_data->dispatch_table[new_location].unpack_fp != NULL ) */

        /* Print debug message to track instruction Traverse */
        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG_4( FTM_HIGH, "[calv3][Segment %3d][OPCODE] "
                     "INSTRUCTION TRAVERSE :  [ + { %3d } = 0x%8x ] OC-0x%x", 
                     cal_data->seg_data.cal_instance + 1,
                     INS_TRAV_DELTA(), INS_TRAV_CURR(), opcode_bit );
          INS_TRAV_STORE();
        } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
        
        if ( new_location <= 63 )
        {
          reseq_oc->opcode.oc_g0 |= 1<<new_location;
          reseq_oc->opcode_group_mask |= 0x1;
        }
        else if ( ( new_location >= 63 ) && (new_location <= 127) )
        {
          reseq_oc->opcode.oc_g1 |= 1<<new_location;
          reseq_oc->opcode_group_mask |= 0x2;
        }
        else if ( ( new_location >= 128 ) && (new_location <= 191) )
        {
          reseq_oc->opcode.oc_g2 |= 1<<new_location;
          reseq_oc->opcode_group_mask |= 0x4;
        }
        else if ( ( new_location >= 192 ) && (new_location <= 255) )
        {
          reseq_oc->opcode.oc_g3 |= 1<<new_location;
          reseq_oc->opcode_group_mask |= 0x8;
        }
        else
        {
          reseq_oc->opcode.oc_g0 = 0;
          reseq_oc->opcode.oc_g1 = 0;
          reseq_oc->opcode.oc_g2 = 0;
          reseq_oc->opcode.oc_g3 = 0;
          oc_group_temp = 0;
          FTM_MSG_1( FTM_ERROR, "ftm_calibration_unpack_payload: Invalid"
                                "number of OC group Num %d", oc_group_num );
          ret_val = FALSE;
          break;
        }
      }

      /* Update OpCode to set the processed bit to zero. */
      opcode_temp = opcode_temp & (~opcode_mask);

      /* For the current unprocessed Opcode, extract the bit mask for the first
      enabled cal action. The first bit will be from LSB. */
      opcode_mask = inline_rf_extract_first_one_bit_mask_uint64( opcode_temp );
    } /* while ( opcode_mask != 0 ) */
    
    /* Update OpCode to set the processed bit to zero. */
    oc_group_temp = oc_group_temp & (~oc_group_mask);

    /* For the current unprocessed Opcode, extract the bit mask for the first
    enabled cal action. The first bit will be from LSB. */
    oc_group_mask = inline_rf_extract_first_one_bit_mask( oc_group_temp );

  } /* while ( oc_group_mask != 0 ) */

  /* Init the oc_remained with the current re-sequenced opcode */
  memscpy( &(cal_data->seg_data.opcode_data.oc_info_remained), 
           sizeof(ftm_cal_opcode_info_type),
           reseq_oc, sizeof(ftm_cal_opcode_info_type) );

  /* Throw Debug messages */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_7( FTM_HIGH, "[calv3][Segment %3d][OPCODE] Resequence : "
               "OpCode (%d)[ 0x%x . 0x%x . 0x%x . 0x%x ] GM 0x%x", 
               cal_data->seg_data.cal_instance + 1,
               cal_data->seg_data.opcode_data.oc_info_reseq.opcode_size, 
               cal_data->seg_data.opcode_data.oc_info_reseq.opcode.oc_g3,
               cal_data->seg_data.opcode_data.oc_info_reseq.opcode.oc_g2,
               cal_data->seg_data.opcode_data.oc_info_reseq.opcode.oc_g1,
               cal_data->seg_data.opcode_data.oc_info_reseq.opcode.oc_g0,
               cal_data->seg_data.opcode_data.oc_info_reseq.opcode_group_mask );
  } /* if ( cal_data->debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_unpack_payload */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize OpCode
 
  @details
  This function will look at the 2 MSBs from the OpCode and determine the
  length of the OpCode. The size of the opcode is decided based on following
  mapping.
 
  0b00 = 2 bytes
  0b01 = 8 bytes
  0b10 = 16 bytes
  0b11 = 32 bytes
 
  @param cal_data
  Pointer to cal v3 data
 
  @param opcode_ptr
  Pointer to the memory which holds the variable length opcode
  
  @return
  Flag indicating the status of opcode data deinit
*/
#ifndef T_WINNT
static inline
#else
#error code not present
#endif
boolean
ftm_calibration_unpack_opcode
(
  struct ftm_calibration_data_type_t *cal_data,
  uint8 *opcode_ptr
)
{
  uint16 oc_len_ind = 0;
  ftm_cal_opcode_length_type opcode_size = OPCODE_INVALID_BYTES;
  uint8 *oc_ptr = 0; /* 8byte pointer to perform opcode memcpy */
  boolean ret_val = TRUE;
  uint64 oc_g0 = 0;
  uint64 oc_g1 = 0;
  uint64 oc_g2 = 0;
  uint64 oc_g3 = 0;
  uint8 opcode_group_mask = 0;
  ftm_cal_oc_group_type oc_group;

  /* NULL Pointer check */
  if ( ( cal_data == NULL ) || ( opcode_ptr == NULL ) )
  {
    FTM_MSG( FTM_FATAL, "ftm_calibration_unpack_opcode: NULL Param" );
    return FALSE;
  } /* if ( ( cal_data == NULL ) || ( opcode_ptr == NULL ) ) */

  /* Extract the 2 MSBs to decide on the OpCode length */
  memscpy( &oc_len_ind, sizeof(oc_len_ind),opcode_ptr, sizeof(oc_len_ind) );
  oc_len_ind = (oc_len_ind >> 14) & 0x03; /* Shift the 2 MSBs to LSB */

  /* Use byte pointer to point to opcode memory */
  oc_ptr = opcode_ptr;

  /* Based on the opcode length parse the opcode and store for ISR usage.
  Based on opcode length, opcodes will be partitioned into 64bit chunks
  and stored in four 64bit containers. Along with this, the length indicator
  bits (2 MSBs) will be truncated from the container */
  switch ( oc_len_ind )
  {

  case 0x0: /* 0b00 */
    opcode_size = OPCODE_2_BYTES;
    oc_g3 = 0;
    oc_g2 = 0;
    oc_g1 = 0;
    memscpy( &oc_g0, 2,oc_ptr, 2 ); oc_g0 = oc_g0 & 0x3FFF;
    oc_ptr += 8;
    opcode_group_mask = 0x1;
    break;

  case 0x1: /* 0b01 */
    opcode_size = OPCODE_8_BYTES;
    oc_g3 = 0;
    oc_g2 = 0;
    oc_g1 = 0;
    memscpy( &(oc_group.oc_chunk[3]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[2]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[1]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[0]), 2,oc_ptr, 2 ); oc_ptr += 2;
    oc_g0 = ftm_cal_opcode_append_chunks( oc_group );
    oc_g0 = oc_g0 & 0x3FFFFFFFFFFFFFFFLL;
    opcode_group_mask = 0x1;
    break;

  case 0x2: /* 0b10 */
    opcode_size = OPCODE_16_BYTES;
    oc_g3 = 0;
    oc_g2 = 0;
    memscpy( &(oc_group.oc_chunk[3]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[2]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[1]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[0]), 2,oc_ptr, 2 ); oc_ptr += 2;
    oc_g1 = ftm_cal_opcode_append_chunks( oc_group );
    oc_g1 = oc_g1 & 0x3FFFFFFFFFFFFFFFLL;
    memscpy( &(oc_group.oc_chunk[3]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[2]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[1]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[0]), 2,oc_ptr, 2 ); oc_ptr += 2;
    oc_g0 = ftm_cal_opcode_append_chunks( oc_group );
    opcode_group_mask = 0x3;
    break;

  case 0x3: /* 0b11 */
    opcode_size = OPCODE_32_BYTES;
    memscpy( &(oc_group.oc_chunk[3]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[2]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[1]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[0]), 2,oc_ptr, 2 ); oc_ptr += 2;
    oc_g3 = ftm_cal_opcode_append_chunks( oc_group );
    oc_g3 = oc_g3 & 0x3FFFFFFFFFFFFFFFLL;
    memscpy( &(oc_group.oc_chunk[3]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[2]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[1]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[0]), 2,oc_ptr, 2 ); oc_ptr += 2;
    oc_g2 = ftm_cal_opcode_append_chunks( oc_group );
    memscpy( &(oc_group.oc_chunk[3]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[2]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[1]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[0]), 2,oc_ptr, 2 ); oc_ptr += 2;
    oc_g1 = ftm_cal_opcode_append_chunks( oc_group );
    memscpy( &(oc_group.oc_chunk[3]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[2]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[1]), 2,oc_ptr, 2 ); oc_ptr += 2;
    memscpy( &(oc_group.oc_chunk[0]), 2,oc_ptr, 2 ); oc_ptr += 2;
    oc_g0 = ftm_cal_opcode_append_chunks( oc_group );
    opcode_group_mask = 0xF;
    break;

  default:
    opcode_size = OPCODE_INVALID_BYTES;
    oc_g3 = 0;
    oc_g2 = 0;
    oc_g1 = 0;
    oc_g0 = 0;
    opcode_group_mask = 0x0;
    ret_val = FALSE;
    FTM_MSG_1( FTM_FATAL, "ftm_calibration_unpack_opcode: Invalid"
               "OpCode size. Possible bad address 0x%x", opcode_ptr );
    break;

  } /* switch ( (ftm_cal_opcode_length_type)oc_len_ind ) */

  cal_data->seg_data.opcode_data.oc_info.opcode_size = opcode_size;
  cal_data->seg_data.opcode_data.oc_info.opcode.oc_g0 = oc_g0;
  cal_data->seg_data.opcode_data.oc_info.opcode.oc_g1 = oc_g1;
  cal_data->seg_data.opcode_data.oc_info.opcode.oc_g2 = oc_g2;
  cal_data->seg_data.opcode_data.oc_info.opcode.oc_g3 = oc_g3;
  cal_data->seg_data.opcode_data.oc_info.opcode_group_mask = opcode_group_mask;

  /* Flag the OpCode Data as valid */
  cal_data->seg_data.opcode_data.oc_info.validity = TRUE;

  return ret_val;

} /* ftm_calibration_unpack_opcode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize OpCode Module
 
  @details
  This function is called to perform one-time opcode module initialization (per
  cal sweep).
 
  @param oc_data
  Pointer to data structure containing Opcode Data
 
  @return
  Flag indicating the status of opcode initialization
*/
boolean
ftm_calibration_init_opcode
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  uint16 oc_idx;
  ftm_cal_opcode_data_type *oc_data;

  /* NULL pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_calibration_init_opcode: NULL OC Data" );
    return FALSE;
  } /* if ( oc_data == NULL ) */

  oc_data = &(cal_data->cal_config.opcode_data);

  /* Init new_oc_location */
  for ( oc_idx = 0; oc_idx < FTM_CAL_OPCODE_MAX; oc_idx++ )
  {
    oc_data->location_map[oc_sequence[oc_idx]].opcode = oc_sequence[oc_idx];
    oc_data->location_map[oc_sequence[oc_idx]].new_loc = oc_idx;
  } /* for ( oc_idx = 0; oc_idx < FTM_CAL_OPCODE_MAX; oc_idx++ ) */

  return TRUE;

} /* ftm_calibration_init_opcode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-Initialize OpCode Module
 
  @details
  This function is called to perform opcode module deinitialization at the
  end of the the calibration sweep
 
  @param cal_data
  Pointer to data structure containing calv3 Data
 
  @return
  Flag indicating the status of opcode deinitialization
*/
boolean
ftm_calibration_deinit_opcode
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  /* NULL pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_calibration_init_opcode: NULL OC Data" );
    return FALSE;
  } /* if ( cal_data == NULL ) */

  
  memset( &(cal_data->cal_config.opcode_data), 0x0,
          sizeof(ftm_cal_opcode_data_type) );

  return TRUE;

} /* ftm_calibration_deinit_opcode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  OpCode Registration
 
  @details
  This function is by each opcode module to perform opcode dispatch data
  registration
 
  @param oc_data
  Pointer to data structure containing Opcode Data
 
  @param opcode_bit
  OpCode for which the registration is to be done
 
  @param reg_data
  Structure containing opcode registration data
 
  @param process_fp
  Function pointer to perform Opcode processing
 
  @param res_buff_info
  Structure containing results buffer info
 
  @return
  Flag indicating the status of opcode registration
*/
boolean
ftm_calibration_register_opcode
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_bit_type opcode_bit,
  ftm_cal_opcode_reg_data_type reg_data
)
{
  ftm_cal_opcode_bit_type new_oc_location;
  ftm_cal_opcode_data_type *oc_data;

  /* NULL pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_calibration_register_opcode: NULL OC Data" );
    return FALSE;
  } /* if ( cal_data == NULL ) */

  oc_data = &(cal_data->cal_config.opcode_data);

  new_oc_location = oc_data->location_map[opcode_bit].new_loc;

  oc_data->dispatch_table[new_oc_location].new_loc = new_oc_location;
  oc_data->dispatch_table[new_oc_location].oc_bit = opcode_bit;

  /* This will be populated when opcode is decoded in every segment */
  oc_data->dispatch_table[new_oc_location].payload_ptr = NULL;

  oc_data->dispatch_table[new_oc_location].unpack_fp = reg_data.unpack_fp;
  oc_data->dispatch_table[new_oc_location].process_fp = reg_data.process_fp;
  oc_data->dispatch_table[new_oc_location].res_buff_info = 
                                                  reg_data.res_buff_info;
  oc_data->dispatch_table[new_oc_location].validity = TRUE;

  return TRUE;

} /* ftm_calibration_register_opcode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack OpCode payload
 
  @details
  This function will be called from the ISR to unpack the payload associated
  with the given opcode of the segment. This function will store the RF 
  calibration instruction data for the current active segment in calv3
  data structure.
 
  The caller does not need to pass information about the 
  segment number as this information will be maintained internally 
  by this module while traversing through the linked list.

  When the end of link list is reached, NULL data is populated to
  the containiner; and the return value of this function will FALSE. This
  should be an indication to the caller not to call this function anymore
  and cease the ISR.
 
  @param cal_data
  Pointer to cal v3 data
 
  @return
  Flag indicating the status of unpack operation. FALSE indicates that there
  are no more valid opcode. Thus the calling funciton is expected to cease the
  ISR in this case.
*/
boolean
ftm_calibration_unpack_instruction
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;

  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_FATAL, "ftm_calibration_unpack_instruction: NULL Param" );
    return FALSE;
  } /* if ( ( cal_data == NULL ) || ( opcode_ptr == NULL ) ) */

  /* Check "pause_to_poll" state before proceeding - FALSE will allow normal
  fetching of instruction. TRUE will pause the instruction fetching and
  keep on returning the last fetched instruciton */
  if ( cal_data->status.pause_to_poll == FALSE )
  {
    /* Check whether the Node has RF Calibration Configuration. If it is NULL,
    indicate the caller to cease the ISR */
    if ( cal_data->cal_config.read_node == NULL )
    {
      FTM_MSG_1( FTM_HIGH, "[calv3][Segment %3d][OPCODE] "
                 "Unpack Instruction : NULL Read Pointer", 
                 cal_data->seg_data.cal_instance + 1 );
      /* Populate Invalid Param */
      cal_data->seg_data.interval = FTM_CAL_SEGMENT_INTREVAL_INVALID;
      return FALSE;
    } /* if ( cal_data->cal_config.read_node == NULL ) */
  
    /* Reset the Segment pointer to top, when new packet Node is processed */
    /* Instruction segment counter can be used to determine, if the new Node
    is processed. This counter is set to zero for each new node. Thus, based
    on this, update the payload pointer to point to calibration instruction
    of new Node. */
    if ( cal_data->cal_config.read_node_seg_counter == 0 )
    {
      /* Set the Instruction Payload Pointer to the  Calibration Instruction
      pointer of the pkt_node pointer */
      cal_data->cal_config.read_node_payload_ptr = 
        (uint8*)cal_data->cal_config.read_node->cal_instruction;
    } /* if ( cal_data->cal_config.read_node_seg_counter == 0 ) */    

    /* Print debug message to track instruction Traverse */
    if ( cal_data->cal_config.debug_mode_en == TRUE )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][OPCODE] "
                 "INSTRUCTION TRAVERSE : [ + { %3d } = 0x%8x ] "
                 "START ::::::::::..........", 
                 cal_data->seg_data.cal_instance + 1,
                 INS_TRAV_DELTA(), INS_TRAV_CURR() );
      INS_TRAV_STORE();
    } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
       
    ret_val = ftm_calibration_unpack_opcode( cal_data, 
                                cal_data->cal_config.read_node_payload_ptr );

    /* Check if the opcode initialization passed or not */
    if ( ret_val == FALSE )
    {
      FTM_MSG_1( FTM_FATAL, "ftm_calibration_unpack_instruction : OpCode "
                 "Init failed @ 0x%x",
                 cal_data->cal_config.read_node_payload_ptr );
      return FALSE;
    } /* if ( ret_val == FALSE ) */
       
    /* Increment the Instruction payload pointer with the size of OpCode */
    cal_data->cal_config.read_node_payload_ptr += 
                           cal_data->seg_data.opcode_data.oc_info.opcode_size;

    /* Print debug message to track instruction Traverse */
    if ( cal_data->cal_config.debug_mode_en == TRUE )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][OPCODE] "
                 "INSTRUCTION TRAVERSE : [ + { %3d } = 0x%8x ] OpCode", 
                 cal_data->seg_data.cal_instance + 1,
                 INS_TRAV_DELTA(), INS_TRAV_CURR() );
      INS_TRAV_STORE();
    } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
    
    /* Update the Interval length for this instruction segment */
    memscpy( &(cal_data->seg_data.interval), 
             sizeof(ftm_calibration_segment_duration_type),
             cal_data->cal_config.read_node_payload_ptr,
             sizeof(ftm_calibration_segment_duration_type) );
  
    /* Increment the Instruction payload pointer with size of segment
    duration */
    cal_data->cal_config.read_node_payload_ptr += 
                               sizeof(ftm_calibration_segment_duration_type);
  
    /* Print debug message to track instruction Traverse */
    if ( cal_data->cal_config.debug_mode_en == TRUE )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][OPCODE] "
                 "INSTRUCTION TRAVERSE : [ + { %3d } = 0x%8x ] Duration", 
                 cal_data->seg_data.cal_instance + 1,
                 INS_TRAV_DELTA(), INS_TRAV_CURR() );
      INS_TRAV_STORE();
    } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */
        
    /* Re-sequence OpCode */
    ftm_calibration_unpack_payload( cal_data );

    /* Increment the Segment Counter */
    cal_data->cal_config.read_node_seg_counter++;
  
    /* Check if we have processed all the instruction segments. If all are 
    processed, then reset the segment counter and update the current Read Node 
    pointer to next Node.  */
    if ( cal_data->cal_config.read_node_seg_counter == 
         cal_data->cal_config.read_node->num_of_segment )
    {
      cal_data->cal_config.read_node_seg_counter = 0;
  
      cal_data->cal_config.read_node = 
          cal_data->cal_config.read_node->next_config;
    } /* if ( cal_data->cal_config.read_node_seg_counter == 
         cal_data->cal_config.read_node->num_of_segment ) */
  } /* if ( cal_data->pause_to_poll == FALSE ) */

  /* Throw Debug messages */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][OPCODE] INSTRUCTION TRAVERSE : "
               "[ + { %3d } = 0x%8x ] END ::::::", 
               cal_data->seg_data.cal_instance + 1,
               INS_TRAV_DELTA(), INS_TRAV_CURR() );
    INS_TRAV_STORE();

    FTM_MSG_9( FTM_HIGH, "[calv3][Segment %3d][OPCODE] Original : "
               "OpCode (%d)[ 0x%x . 0x%x . 0x%x . 0x%x ] "
               "GM 0x%x, SegDuration %dus [p2p %d]", 
               cal_data->seg_data.cal_instance + 1,
               cal_data->seg_data.opcode_data.oc_info.opcode_size, 
               cal_data->seg_data.opcode_data.oc_info.opcode.oc_g3,
               cal_data->seg_data.opcode_data.oc_info.opcode.oc_g2,
               cal_data->seg_data.opcode_data.oc_info.opcode.oc_g1,
               cal_data->seg_data.opcode_data.oc_info.opcode.oc_g0,
               cal_data->seg_data.opcode_data.oc_info.opcode_group_mask,
               cal_data->seg_data.interval, 
               cal_data->status.pause_to_poll );
  } /* if ( cal_data->debug_mode_en == TRUE ) */
  
  return ret_val;

} /* ftm_calibration_unpack_instruction */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get next opcode for execution
 
  @details
  This function looks at the re-sequenced opcode to determine the next
  opcode to be executed for the current segment. Return status indicates the
  validity of the opcode dispatch data. If the dispatch data is invalid,
  the calling module should cease the fetching of more opcodes.
 
  @param cal_data
  Pointer to the calibration data structure
 
  @param oc_disp
  Pointer to the data structure where the opCode dsispatch data is populated
  for the calling function
 
  @return
  Flag indicates, if the opcode dispatch data is valid or not. FALSE indicates
  that the calling function should stop calling this function for the current
  segment.
*/
boolean
ftm_calibration_get_opcode
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type **oc_disp
)
{
  boolean ret_val = TRUE;

  ftm_cal_opcode_info_type *reseq_oc;

  uint64 opcode_mask; 
  ftm_cal_opcode_bit_type opcode_bit;
  uint8 oc_group_mask;
  uint8 oc_group_num;
  ftm_cal_opcode_data_type *oc_data;
  uint64 opcode_temp;

  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_FATAL, "ftm_calibration_get_opcode: NULL Param" );
    return FALSE;
  } /* if ( ( cal_data == NULL ) || ( opcode_ptr == NULL ) ) */

  oc_data = &(cal_data->cal_config.opcode_data);
  reseq_oc = &(cal_data->seg_data.opcode_data.oc_info_remained);

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */
  oc_group_mask = inline_rf_extract_first_one_bit_mask( 
                                         reseq_oc->opcode_group_mask );

  if ( oc_group_mask != 0 )
  {
    /* Find the opcode group number */
    oc_group_num = inline_rf_extract_first_one_bit_lsb_location(oc_group_mask);

    /* Get the OpCode for the current instruction: grp_cnt refers to
    the OpCode group for which the payload is to be extracted */
    switch ( oc_group_num )
    {
    case 0:opcode_temp = reseq_oc->opcode.oc_g0;break;
    case 1:opcode_temp = reseq_oc->opcode.oc_g1;break;
    case 2:opcode_temp = reseq_oc->opcode.oc_g2;break;
    case 3:opcode_temp = reseq_oc->opcode.oc_g2;break;
    default:
      FTM_MSG_1( FTM_ERROR, "ftm_calibration_get_opcode: Invalid"
                            "number of OC group Num %d", oc_group_num );
      opcode_temp = 0;
      ret_val = FALSE;
      break;
    } /* switch ( oc_group_num ) */

    /* For the current unprocessed Opcode, extract the bit mask for the first
    enabled cal action. The first bit will be from LSB. */
    opcode_mask = inline_rf_extract_first_one_bit_mask_uint64( opcode_temp );

    /* Loop until, the OpCode Mask comes out to be zero */
    if ( opcode_mask != 0 )
    {

      /* Find the bit location and this location will correspond to 
      enumeration in ftm_cal_opcode_bit_type. This number is readjusted
      (grp_cnt*64) as the opcode might fall on any opcode group. */
      opcode_bit = (ftm_cal_opcode_bit_type)(
                    inline_rf_extract_first_one_bit_lsb_location(opcode_mask) +
                    oc_group_num*64 );

      if ( oc_data->dispatch_table[opcode_bit].validity == TRUE )
      {
        *oc_disp = &(oc_data->dispatch_table[opcode_bit]);

        /* Throw Debug messages */
        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG_4( FTM_HIGH, "[calv3][SEGMENT %3d][OPCODE] Fetch OC : "
                     "ocBIT 0x%x newLoc %d [ P_PTR 0x%8x ] ", 
                     cal_data->seg_data.cal_instance,
                     oc_data->dispatch_table[opcode_bit].oc_bit,
                     opcode_bit,
                     oc_data->dispatch_table[opcode_bit].payload_ptr);
        } /* if ( cal_data->debug_mode_en == TRUE ) */
      }
      else
      {
        *oc_disp = NULL;
        ret_val = FALSE;
      }

      /* Update OpCode to set the processed bit to zero. */
      opcode_temp = opcode_temp & (~opcode_mask);

    } /* if ( opcode_mask != 0 ) */
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_calibration_get_opcode: OC Group %d not NULL "
                            "for NULL OC",  oc_group_num);
    }

    /* Get the OpCode for the current instruction: grp_cnt refers to
    the OpCode group for which the payload is to be extracted */
    switch ( oc_group_num )
    {
    case 0:reseq_oc->opcode.oc_g0 = opcode_temp;break;
    case 1:reseq_oc->opcode.oc_g1 = opcode_temp;break;
    case 2:reseq_oc->opcode.oc_g2 = opcode_temp;break;
    case 3:reseq_oc->opcode.oc_g2 = opcode_temp;break;
    default:
      FTM_MSG_1( FTM_ERROR, "ftm_calibration_get_opcode: Invalid"
                            "number of OC group Num %d", oc_group_num );
      opcode_temp = 0;
      ret_val = FALSE;
      break;
    } /* switch ( oc_group_num ) */
    
    /* Reset Group mask, if all the opcodes in the group are done */
    if ( opcode_temp == 0 )
    {
      /* Update OpCode to set the processed bit to zero. */
      reseq_oc->opcode_group_mask &= (~oc_group_mask);
    }

  } /* if ( oc_group_mask != 0 ) */
  else
  {
    ret_val = FALSE;
  } /* if ! ( oc_group_mask != 0 ) */

  return ret_val;
} /* ftm_calibration_get_opcode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-initialize OpCode
 
  @details
  This function will invalidate the OpCode data
 
  @param cal_data
  Pointer to cal v3 data
 
  @return
  Flag indicating the status of opcode data deinit
*/
boolean
ftm_calibration_invalidate_opcode
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_FATAL, "ftm_calibration_invalidate_opcode: NULL Param" );
    return FALSE;
  } /* if ( cal_data == NULL ) */

  memset( &(cal_data->seg_data.opcode_data),
          0x0,
          sizeof(ftm_cal_opcode_per_segment_data_type));
  
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_1( FTM_HIGH, "[calv3][SEGMENT %3d][OPCODE] Invalidate OpCode ", 
               cal_data->seg_data.cal_instance );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return TRUE;

} /* ftm_calibration_deinit_opcode */

/*! @} */


