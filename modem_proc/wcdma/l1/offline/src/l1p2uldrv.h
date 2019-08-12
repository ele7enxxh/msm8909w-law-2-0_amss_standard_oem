/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L1 COPY ENGINE DRIVER PRIVATE DEFINITIONS                   

GENERAL DESCRIPTION

  This file contains the private variable and function declarations for the 
  L1 Uplink copy-engine driver.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2010 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/l1p2uldrv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---   ----------------------------------------------------------
11/24/11   gnk    Removed unused features
03/01/10   rvs    Add P4 info to header.
08/28/09   gv     Checkin EUL PP changes protected within the feature
                  DEBUG_FEATURE_WCDMA_PP_HW_UL
05/20/09   rm     Fixed compilation error
05/20/09   gnk    Fixed critical lint errors seen on off target build
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

//#include "l1rlcif.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                               Constants                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define L1_UL_P2_ERAM_BANK_DATA_SECTION_OFFSET       0x03E8
#define L1_UL_P2_ERAM_BANK_HDR_SECTION_OFFSET        0x0DA8

//                 0x0   _ _ _ _ _ _ _ _ _ _ _ _
//                      |                       |
//                      |                       |
//                      |      BUFFER           |
//                      |                       |
//                      |                       |
//                      |                       |
//               0x3E8   _ _ _ _ _ _ _ _ _ _ _ _
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |     DATA SECTION      |
//                      |     ( 624 words )     |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                      |                       |
//                       _ _ _ _ _ _ _ _ _ _ _ _
//               0xDA8  |                       |
//                      |                       |
//                      |    HDR SECTION        |
//                      |    ( 150 words )      |
//                      |                       |
//                      |                       |
//                      |                       |
//              0x1000  ------------------------

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                                 MACROS                                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Bitmask and shift definitions for the COPY engine control tasks */
#define UMTS_P2_UL_SET_CTRL_WRD_TASK_TYPE_SHFT             31
#define UMTS_P2_UL_SET_CTRL_WRD_LAST_TASK_SHFT             30
#define UMTS_P2_UL_SET_CTRL_WRD_BASE_ENDIANNESS_SHFT       17
#define UMTS_P2_UL_SET_CTRL_WRD_IGNORE_RMW_SHFT            16
#define UMTS_P2_UL_SET_CTRL_WRD_DATA_LENGTH_SHFT           0
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_INTERNAL_SHFT          25
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_CONST_SHFT             24
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_ENDIANNESS_SHFT        23
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_ADDR_SHFT              5
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_OFST_SHFT              0
#define UMTS_P2_UL_SET_CTRL_WRD_DST_INTERNAL_SHFT          25
#define UMTS_P2_UL_SET_CTRL_WRD_DST_CONST_SHFT             24
#define UMTS_P2_UL_SET_CTRL_WRD_DST_ENDIANNESS_SHFT        23
#define UMTS_P2_UL_SET_CTRL_WRD_DST_ADDR_SHFT              5
#define UMTS_P2_UL_SET_CTRL_WRD_DST_OFST_SHFT              0
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_ADDR_ERAM_BANK_SHFT    20

// Macro to increment address to next word 
#define UMTS_P2_UL_SET_ADDR_TO_NEXT_WRD( x )   x = (( x + 3 ) & 0xFFFFFFFC)


/* Copy-Engine Control word format Macros */
/*
 *
 *
 *   Word 0 - Task info
 *
 * 31     TASK_TYPE         Always Set to 0 for single-task-type
 * 30     LAST_TASK         0: Another task is following
 *                          1: Last task for this trigger
 * 29:18  RESERVED
 * 17     BASE_ENDIANESS    0: Data is internally processed in little-endian format
 *                          1: Data is internally processed in big-endian format
 *
 * 16     IGNORE_RMW        0: Perform read-modify-write if writing partial word
 *                          1: Do not perform read-modify-write if writing partial word (speed-up)
 *
 * 15:0   LENGTH            Length in bits of the data to copy
 *                          Allows to copy up to 8k Bytes per copy operation
 */

#define UMTS_P2_UL_CTRL_WRD_SINGLE_TASK     0
#define UMTS_P2_UL_CTRL_WRD_REPEAT_TASK     1

#define UMTS_P2_UL_SET_CTRL_WRD_TASK_TYPE(x, y)         x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_TASK_TYPE_SHFT )
                                                        
#define UMTS_P2_UL_SET_CTRL_WRD_LAST_TASK(x, y)         x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_LAST_TASK_SHFT )
                                                        
#define UMTS_P2_UL_CTRL_WRD_BASE_LITTLE_ENDIAN  0       
#define UMTS_P2_UL_CTRL_WRD_BASE_BIG_ENDIAN     1       
                                                        
#define UMTS_P2_UL_SET_CTRL_WRD_BASE_ENDIANNESS(x, y)   x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_BASE_ENDIANNESS_SHFT )
                                                        
#define UMTS_P2_UL_CTRL_WRD_PERFORM_RMW       0         
#define UMTS_P2_UL_CTRL_WRD_NOT_PERFORM_RMW   1         
                                                        
#define UMTS_P2_UL_SET_CTRL_WRD_IGNORE_RMW(x, y)        x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_IGNORE_RMW_SHFT )
                                                        
#define UMTS_P2_UL_SET_CTRL_WRD_LENGTH_BITS(x, y)       x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_DATA_LENGTH_SHFT )

/* Just use this macro to set all values in one go */
#define UMTS_P2_UL_SET_CTRL_WRD_TASK_INFO(x, type, last_task, endian, rmw, len_bits)   \
              UMTS_P2_UL_SET_CTRL_WRD_LAST_TASK(x, last_task);                          \
              UMTS_P2_UL_SET_CTRL_WRD_TASK_TYPE(x, type);                               \
              UMTS_P2_UL_SET_CTRL_WRD_BASE_ENDIANNESS(x, endian);                       \
              UMTS_P2_UL_SET_CTRL_WRD_IGNORE_RMW(x, rmw);                               \
              UMTS_P2_UL_SET_CTRL_WRD_LENGTH_BITS(x, (len_bits )); /* Pass length in bits */


/*   Word 1 - Src info
 *
 * 31:26  RESERVED
 * 25     SRC_INTERNAL      Intended for HW use only, SW to set this field to 0
 *                          0: Use RAM address for addressing (SW always uses this mode)
 *                          1: Use a P2 internal register for addressing
 *
 * 24     SRC_CONST         Intended for HW use only, SW to set this field to 0
 *                          0: Increments the address word-by-word within one repetition
 *                          1: Keeps address constant at the value specified
 *
 * 23     SRC_ENDIANESS     0: Data is stored in little-endian format
 *                          1: Data is stored in big-endian format
 *
 * 22:5   SRC_ADDR          Source address in word-resolution
 *                          Allows to address 256k words
 *
 * 4:0    SRC_OFFSET        Source Offset in bit-resolution
 *                          If ENDIANESS is little-endian, offset is measured from LSB
 *                          If ENDIANESS is big-endian, offset is measured from MSB
 */
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_INTERNAL(x, y)   x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_SRC_INTERNAL_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_SRC_CONST(x, y)      x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_SRC_CONST_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_SRC_ENDIANNESS(x, y) x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_SRC_ENDIANNESS_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_SRC_ADDR(x, y)       x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_SRC_ADDR_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_SRC_OFST(x, y)       x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_SRC_OFST_SHFT )

/* Just use this macro to set all values in one go */
#define UMTS_P2_UL_SET_CTRL_WRD_SRC_INFO(x, endian, src_addr, src_ofst)   \
       	      UMTS_P2_UL_SET_CTRL_WRD_SRC_INTERNAL(x, 0);                  \
       	      UMTS_P2_UL_SET_CTRL_WRD_SRC_CONST(x, 0);                     \
       	      UMTS_P2_UL_SET_CTRL_WRD_SRC_ENDIANNESS(x, endian);           \
       	      UMTS_P2_UL_SET_CTRL_WRD_SRC_ADDR(x, (src_addr));               \
       	      UMTS_P2_UL_SET_CTRL_WRD_SRC_OFST(x, src_ofst);      


/*
 * Word 2 - Dest info
 *
 * 31:26  RESERVED
 * 25     DST_INTERNAL      Intended for HW use only, SW to set this field to 0
 *                          0: Use RAM address for addressing
 *                          1: Use a P2 internal register for addressing
 *
 * 24     DST_CONST         Intended for HW use only, SW to set this field to 0
 *                          0: Increments the address word-by-word within one repetition
 *                          1: Keeps address constant at the value specified
 *
 * 23     DST_ENDIANESS     0: Data is stored in little-endian format
 *                          1: Data is stored in big-endian format
 *
 * 22:5   DST_ADDR          Destination address in word-resolution
 *                          Allows to address 256k words
 *
 * 4:0    DST_OFFSET        Destination Offset in bit-resolution
 *                          If destination data is stored in little-endian, offset is measured from LSB
 *                          If destination data is stored in big-endian, offset is measured from MSB
 */
#define UMTS_P2_UL_SET_CTRL_WRD_DST_INTERNAL(x, y)    x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_DST_INTERNAL_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_DST_CONST(x, y)       x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_DST_CONST_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_DST_ENDIANNESS(x, y)  x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_DST_ENDIANNESS_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_DST_ADDR(x, y)        x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_DST_ADDR_SHFT )

#define UMTS_P2_UL_SET_CTRL_WRD_DST_OFST(x, y)        x |= ( y << UMTS_P2_UL_SET_CTRL_WRD_DST_OFST_SHFT )

/* Just use this macro to set all values in one go */
#define UMTS_P2_UL_SET_CTRL_WRD_DST_INFO(x, endian, dest_addr, dest_ofst)   \
       	      UMTS_P2_UL_SET_CTRL_WRD_DST_INTERNAL(x, 0);                    \
       	      UMTS_P2_UL_SET_CTRL_WRD_DST_CONST(x, 0);                       \
       	      UMTS_P2_UL_SET_CTRL_WRD_DST_ENDIANNESS(x, endian);             \
       	      UMTS_P2_UL_SET_CTRL_WRD_DST_ADDR(x, (dest_addr));                \
       	      UMTS_P2_UL_SET_CTRL_WRD_DST_OFST(x, dest_ofst);    


