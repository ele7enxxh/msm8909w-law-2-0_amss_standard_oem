#ifndef L1CONST_H
#define L1CONST_H
/*===========================================================================

                    L 1 C O N S T . H

DESCRIPTION
  This file contains constants useful across all of L1 and prototypes for
  common L1 utility functions.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1const.h_v   1.16   11 Jul 2002 21:26:14   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1const.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/06/10   ks      Added Enhanced FDPCH support
03/30/10   rgn     Changed cpich threshold for reliable detection of cltd 135 
01/12/10   ksr     Merged the BLAST support updates
09/29/09   hk      Added support for Extended SFN counter
05/29/09   ks      Corrected WL1_GET_BIT_UXX macro wherein input was wrongly typecasetd 
04/24/09   rgn     Added macros to extract word from dword
12/09/08   uk      Mainlining FEATURE_TDSCDMA_DL_ENHANCED
10/24/08   hk      Lint Fixes
10/03/08   hk      Added WCDMA defines for interrupt lock and task lock for WPLT
10/03/08   hk      Bringing in 7k Mailine fixes
08/08/08   ks      Added WL1 bit manipulation macros
01/29/08   scm     Add CHIPX8_PER_256_CHIP_UNITS define.
01/21/08   vsr     Added macro needed for API for WCB size calculation
10/24/07   vsr     Mainlining MBMS and Enhanced DL changes 
12/08/06   rmak    Removed MDSP CMD START AGC SETTLING DELAY USEC definitions
06/05/06   sv      Add definition for mid-frame PN position.
03/23/06   bbd     Added some chipx16 macros.
03/09/06   au      Moved Quicksort and Bubblesort functions to l1mathutil
03/17/05   ms      6280 Change: Featurize UINT_MAX constants under FEATURE_L4
                   to prevent redefinition in L4 builds (which define these in stdint.h
                   and limits.h)
08/12/05   mg      6275 Change: AGC settling time is 800us under feature 
08/01/05   mg      6275 Change: AGC settling time is 1.5ms 
07/18/05   gs      Enhanced GSM ARFCN compare macros to by ading brakets
                   around input arguments.
07/14/05   mg      6275 Change: Increase AGC settling time to 3ms
05/12/05   scm     Add #define MSEC_PER_FRAME 10.
11/03/04   asr     Added #define for CONVERT_256_CHIP_TO_CHIPX8
06/10/04   asr     Moved the AGC settling time constant here, and define it depending
                   on FEATURE_CAGC (less settling time with coarse grain AGC)
12/07/03   yus     Added GSM ARFCN_T MACROS.
08/03/03   gs      Externalized function bbsort
06/18/03   src     Added a new hash-define NO_SEC_SCRAMBLING_CODE for invalid
                   secondary scrambling code.
07/11/02   sh      Added Min and Max quantities for different uint/int
                   data types.
05/19/02   src     Added a bunch of constants related to bit-shifting and per-
                   unit multipliers, etc.
09/26/01   asr     Changes DRX_CYCLE_MAX to 9 (from 12) to comply with Spec
09/20/01   asr     Added SFN_MASK (mask of lower 12 bits) to help with mod 4096 math
                   and CFN_MASK
09/05/01   sh      Added PICH_PI_BITS_PER_FRAME.
06/06/01   sh      moved RF_TUNE_TIME to tdsseq.h.
05/21/2001 sh      added RF_TUNE_TIME.

04/27/2001 mdl     added NO_RL_IDX and NO_RL_SET_IDX

02/21/2001 mdl     added #define CHIP_PER_STMR_TICK
01/17/2001 mdl     added #defines for NO_PATH_PTR, NO_CELL, NO_CELL_PTR,
                   and NO_TPC

01/12/2001 mdl     removed ECIO_TO_ENG_TABLE_SIZE (moved to dlutil.c)
                   added prototype for rssi_to_ecio()

01/04/2001 mdl     added bounds for DRX cycle and PICH power offset,
                   DRX_CYCLE{MIN, MAX}, PICH_PWR_OFFSET{MIN, MAX}

01/03/2001 mdl     added bounds for scrambling codes MAX_PRI_SCR_CODE,
                   MAX_SEC_SCR_CODE

12/14/2000 mdl     moved NULL value constants here from dldem.h
12/12/2000 mdl     added prototype for normalize()
12/12/2000 mdl     Added CHIP_PER_256_CHIP_UNITS
11/01/00   sh      Fixed STMR_TICKS_PER_FRAME
09/27/00   sh      Created File
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#include "tdscdma_variation.h"
#ifdef FEATURE_GSM_ARFCN_STRUCT
#include "sys_type.h"
#endif

#if defined(FEATURE_BLAST)
#include "stdint.h"
#endif

#define CHIPX8_PER_CHIP 8
#define CHIPX4_PER_CHIP 4
#define CHIPX2_PER_CHIP 2

#define CHIPX8_TO_CHIP_RSHIFT 3
#define CHIPX4_TO_CHIP_RSHIFT 2
#define CHIPX2_TO_CHIP_RSHIFT 1

#define CHIPX8_PER_CHIPX2 4
#define CHIPX4_PER_CHIPX2 2

#define CHIPX16_PER_CHIP   16
#define CHIPX16_PER_CHIPX2  8
#define CHIPX16_PER_CHIPX4  4

#define CHIPX8_TO_CHIPX2_RSHIFT 2
#define CHIPX4_TO_CHIPX2_RSHIFT 1

#define CHIPX8_PER_CHIPX4 2

#define CHIPX8_TO_CHIPX4_RSHIFT 1

#define CHIPX8_PER_FRAME  307200L
#define CHIPX4_PER_FRAME  153600L
#define CHIPX8_MID_PN_POS 153600L
#define CHIPX2_PER_FRAME  76800L
#define CHIP_PER_FRAME    38400
#define MSEC_PER_FRAME    10

#define CHIPX8_PER_SLOT   20480
#define CHIPX4_PER_SLOT   10240
#define CHIPX2_PER_SLOT   5120
#define CHIP_PER_SLOT     2560
/* + defang 01/25/2011 */
#define TDSL1_CHIP_PER_SLOT 864
#define TDSL1_CHIP_DWPTS    96
#define TDSL1_CHIP_UPPTS    160
#define TDSL1_CHIP_GP       96
/* - defang 01/25/2011 */

#define CHIPX8_PER_256_CHIP_UNITS    2048
#define CHIP_PER_256_CHIP_UNITS       256
#define NUM_256_CHIP_UNITS_PER_FRAME  150
#define NUM_256_CHIP_UNITS_PER_SLOT    10

#define SLOTS_PER_FRAME 15

#define MAX_SLOTS_FOR_TX 5


#define CHIP_PER_STMR_TICK 256
#define STMR_TICKS_PER_SLOT 10
#define STMR_TICKS_PER_FRAME (STMR_TICKS_PER_SLOT * SLOTS_PER_FRAME)

/*Extended SFN is a 32 bit signed quantity*/
#define MAX_EXTENDED_SFN_COUNT (0x7FFFFFFF)
/* + defang 01/25/2011 */
#define MAX_SUBSFN_COUNT 8192
/* - defang 01/25/2011 */
#define MAX_SFN_COUNT 4096
#define MAX_CFN_COUNT 256
#define SFN_MASK 0x0fff // lower 12 bits (4096 = 2^12)
#define CFN_MASK 0x00ff   // lower  8 bits (256 = 2^8)

#define MAX_PRI_SCR_CODE 512
#define MAX_SEC_SCR_CODE 16

#define DRX_CYCLE_MIN 3
#define DRX_CYCLE_MAX 9

#define PICH_PWR_OFFSET_MIN -10
#define PICH_PWR_OFFSET_MAX 5

/* Number of bits used to carry paging indicators in a PICH frame */
#define PICH_PI_BITS_PER_FRAME 288

#define AICH_BITS_PER_IND 16

/* DPCH transmit frame offset with regard to DPCH receive frames */
#define UL_DL_DPCH_CHIP_OFFSET   1024
#define UL_DL_DPCH_CHIPX8_OFFSET (8*UL_DL_DPCH_CHIP_OFFSET)

#define CONVERT_256_CHIP_TO_CHIPX8  (CHIP_PER_256_CHIP_UNITS * CHIPX8_PER_CHIP)

#define SRCH_ECIO_NOISE_FLOOR   63

/*--------------------------------------------------------------------------
                          NULL VALUE INDICATORS

Due to the fact that 0 is a valid value for many indices of interest and
yet there is still a need to know when an item has a NULL value these
constants are defined to indicate when a given item has no value.
--------------------------------------------------------------------------*/
/* denotes no finger, as in the neighbor reference finger is no finger if
   we are not listening to a neighbor cell PCCPCH.  Also used throughout
   triage */
#define NO_FINGER 0xFF

/* indicates that no CCTrCH table entry is chosen */
#define NO_CCTRCH_INDEX 0xFF

/* indicates no scrambling code */
#define NO_SCRAMBLING_CODE     0xFFFF
#define NO_SEC_SCRAMBLING_CODE 0xFF

/* indicates no cell parameter id */
#define NO_CELL_PARAMETER_ID     0xFF

/* indicates no frequency */
#define NO_FREQUENCY 0xFFFF

/* indicates no channel */
#define NO_CHANNEL 0xFF

/* indicates no PN position */
#define NO_PN_POS 0xFFFFFFFF

/* indicates no path */
#define NO_PATH 0xFF

/* indicates no pointer to a path */
#define NO_PATH_PTR (void *) NULL

/* indicates no cell */
#define NO_CELL  0xFF

/* indicates no pointer to a cell */
#define NO_CELL_PTR (void *) NULL

/* indicates no cell db index */
#define NO_CELL_DB_IDX 0xFF

/* indicates no radio link index */
#define NO_RL_IDX 0xFF

/* indicates no radio link set index */
#define NO_RL_SET_IDX 0xFF

/* indicates no TPC index */
#define NO_TPC 0xFF

/* indicates invalid combiner id */
#define NO_COMBINER_ID 0xFF

/* Min and Max quantities for different data types */

#if !defined(FEATURE_BLAST)
#ifndef UINT32_MAX
  #define UINT32_MAX (0xffffffff)
#endif
#ifndef UINT16_MAX
  #define UINT16_MAX (0xffff)
#endif
#ifndef UINT8_MAX
  #define UINT8_MAX  (0xff)
#endif
#ifndef INT32_MIN
  #define INT32_MIN  (-2147483648)
#endif
#ifndef INT32_MAX
  #define INT32_MAX  (2147483647)
  #endif
#ifndef INT16_MIN
  #define INT16_MIN  (-32768)
  #endif
#ifndef INT16_MAX
  #define INT16_MAX  (32767)
  #endif
#ifndef INT8_MIN
  #define INT8_MIN   (-128)
  #endif
#ifndef INT8_MAX
  #define INT8_MAX   (127)
  #endif
#endif

#ifndef UINT32_MIN
#define UINT32_MIN (0)
#endif
#ifndef UINT16_MIN
#define UINT16_MIN (0)
#endif
#ifndef UINT8_MIN
#define UINT8_MIN  (0)
#endif

/*--------------------------------------------------------------------------
                        Constants for the PCCPCH
--------------------------------------------------------------------------*/
#define PCCPCH_SLOT_FORMAT       0
#define PCCPCH_SPREAD_FACTOR     (tdsl1_sf_enum_type) TDSL1_SF_256
#define PCCPCH_PILOT_BITS        0
#define PCCPCH_OVSF_CODE         0x0001
#define PCCPCH_TIME_OFFSET       0
#define PCCPCH_NUM_DATA_BITS_PER_FRAME 270

/*--------------------------------------------------------------------------
                        Constants for the PICH
--------------------------------------------------------------------------*/
#define PICH_SLOT_FORMAT          0
#define PICH_SPREAD_FACTOR        (tdsl1_sf_enum_type) TDSL1_SF_256
#define PICH_PILOT_BITS           0

/* The PICH frame boundary occurs 3 slots prior to the associated SCCPCH
   frame boundary.  The mDSP wants timing offset in units of 256 chips */
#define PICH_OFFSET_FROM_SCCPCH   (3 * NUM_256_CHIP_UNITS_PER_SLOT)


/*--------------------------------------------------------------------------
                        Constants for the AICH
--------------------------------------------------------------------------*/
#define AICH_SLOT_FORMAT          0
#define AICH_SPREAD_FACTOR        (tdsl1_sf_enum_type) TDSL1_SF_256
#define AICH_PILOT_BITS           0
#define AICH_TIME_OFFSET          0

#define AICH_MIN_PWR_OFFSET       -22

/******************************************************************************
                          MISCELLANEOUS CONSTANTS
******************************************************************************/

/* indicates that no slot format has been selected */
#define NO_SLOT_FORMAT 0xFF

#define INVALID_FRAME_OFFSET 0xFF

#define MDSP_SYNC_PARMS_AICH_OFFSET 0
#define MDSP_SYNC_PARMS_PICH_OFFSET 0


/*--------------------------------------------------------------------------
                   INVALID HARDWARE CHANNEL INDICATOR

This is used by tdsdlchmgr_dl_phychan_lookup as a return value when the requested
reference physical channel was not found to be active.
--------------------------------------------------------------------------*/
#define DL_INVALID_HW_CH 0xFF

/*--------------------------------------------------------------------------
                HASH-DEFINES FOR HARDWARE CHANNEL NUMBERS
--------------------------------------------------------------------------*/
#define DL_HARDWARE_CHANNEL_ZERO   0
#define DL_HARDWARE_CHANNEL_ONE    1
#define DL_HARDWARE_CHANNEL_TWO    2
#define DL_HARDWARE_CHANNEL_THREE  3

#ifdef FEATURE_TDSCDMA_HSUPA
#define DL_HARDWARE_CHANNEL_FOUR   4    /* Used for RGCH */
#define DL_HARDWARE_CHANNEL_FIVE   5    /* Used for HICH */
#endif /* FEATURE_TDSCDMA_HSUPA */

/******************************************************************************
*******************************************************************************
                PROTOTYPES FOR FUNCTIONS APPEARING IN L1_UTIL.C
*******************************************************************************
******************************************************************************/



/*===========================================================================
FUNCTION        TDS_NORMALIZE

FILE NAME       tdsl1util.c

DESCRIPTION     This function will normalize a passed number, parm, to the
                range [0..max-1], where max is also a passed parameter.

                Note that parm is signed but the return value is not.  Also
                note that parm and the return value are 32 bits.  Casting
                may be necessary in order to use this function.

DEPENDENCIES    None

RETURN VALUE    The normalized value of parm

SIDE EFFECTS    None
===========================================================================*/
uint32 tds_normalize
(
  /* the parameter to be normalized */
  int32 parm,

  /* the maximum valid range of the normalized parameter */
  uint32 max
);

/****************************
**  GSM ARFCN_T MACROS     **
****************************/
#ifdef FEATURE_GSM_ARFCN_STRUCT
#define CONVERT_ARFCN_TO_NUM(x) ((x).num)
#define W_ARFCN_T ARFCN_T

#ifdef FEATURE_GSM_QUADBAND
#define COMPARE_ARFCN(x,y) (((x).num == (y).num) && ((x).band == (y).band))
#else
#define COMPARE_ARFCN(x,y) ((x).num == (y).num)
#endif /* FEATURE_GSM_QUADBAND */                                          

#else
#define CONVERT_ARFCN_TO_NUM(x) (x)
#define W_ARFCN_T uint16
#define COMPARE_ARFCN(x,y) ((x) == (y))
#endif /* FEATURE_GSM_ARFCN_STRUCT */

/***********************************
**  WL1 BIT MANIPULATION MACROS     **
***********************************/
 
/* transform a byte count into a bit count (multiply by 8) */
#define WL1_BYTES_TO_BITS(x) ((x)<<3)  

/* transform a bit count into a byte count (divide by 8) */
#define WL1_BITS_TO_BYTES(x) ((x)>>3)


/* get the position of the rightmost bit to '1' in "value" 63..0 */
#define WL1_GET_LSB_POS_U64(value) (tds_rmbd64(value, 0))

/* get the position of the rightmost bit to '1' in "value" 31..0 */
#define WL1_GET_LSB_POS_U32(value) (tds_rmbd32(value, 0))

/* get the position of the rightmost bit to '1' in "value" 15..0 */
// #define WL1_GET_LSB_POS_U16(value) (__rmbd(value, WL1_BYTES_TO_BITS(sizeof(value))))
#define WL1_GET_LSB_POS_U16(value) (tds_rmbd16(value, 0))

/* get the position of the rightmost bit to '1' in "value"  7..0 */
#define WL1_GET_LSB_POS_U8(value) (tds_rmbd8(value, 0))


/* get the position of the leftmost bit to '1' in "value" 63..0 */
#define WL1_GET_MSB_POS_U64(value) ( tds_lmbd64(value, 0) )

/* get the position of the leftmost bit to '1' in "value" 31..0 */
#define WL1_GET_MSB_POS_U32(value) ( tds_lmbd32(value, 0) )

/* get the position of the leftmost bit to '1' in "value" 15..0 */
#define WL1_GET_MSB_POS_U16(value) ( tds_lmbd16(value, 0) )

/* get the position of the leftmost bit to '1' in "value" 7..0 */
#define WL1_GET_MSB_POS_U8(value) ( tds_lmbd8(value, 0) )


/* get number of bits set in "value" */
#define WL1_BITCOUNT_U64(value) (tdsnum_set_bits64(value))

/* get number of bits set in "value" */
#define WL1_BITCOUNT_U32(value) (tdsnum_set_bits32(value))

/* get number of bits set in "value" */
#define WL1_BITCOUNT_U16(value) (tdsnum_set_bits16(value))

/* get number of bits set in "value" */
#define WL1_BITCOUNT_U8(value) (tdsnum_set_bits8(value))


/*  ==================================================================
  Macro          : U8 U16 U32 U64 WL1_GET_SET_LSB_POS_FORM_START_POS(sourceX, start_pos)

  DESCRIPTION:  return the 1st bit position with bit value '1' in "sourceX" starting from "start_pos".
                          PARAMETERS:
                          sourceX: value in which set bit is scanned (X: 8/16/32/64)
                          start_pos   : start position for search
                          
Example : 

source32 
      ---------------------------------------------------------------------------------------
     |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
     ---------------------------------------------------------------------------------------
     |0||0||0||1||0||0||0||0||1||0||0||0||1||0||0||0||1||1||0||1||0||0||0||1||1||0||0||0||0|                                                
                                                                                                          *        --
                                                                                                                     |
                                                                                                                      ----> start_pos = 10

 return value in this example would be 12 (which is the bit position of 1st SET bit after start_pos)
                                                                                                                      
DEPENDENCIES

RETURN VALUE: 1st occurence of "1" after start_pos

SIDE EFFECTS: Input mask will get updated   
 ============================================================ */

#define WL1_GET_SET_LSB_POS_FROM_START_POS_U64(value, start_pos)  (tds_rmbd64(value, start_pos))

#define WL1_GET_SET_LSB_POS_FROM_START_POS_U32(value, start_pos)  (tds_rmbd32(value, start_pos))

#define WL1_GET_SET_LSB_POS_FROM_START_POS_U16(value, start_pos)   (tds_rmbd16(value, start_pos))

#define WL1_GET_SET_LSB_POS_FROM_START_POS_U8(value, start_pos)  (tds_rmbd8(value, start_pos))        


/*  ==================================================================
  Macro          : U8 U16 U32 U64 WL1_GET_SET_MSB_POS_TILL_STOP_POS(sourceX, start_pos)

  DESCRIPTION:  return the 1st bit position with bit value '1' in "sourceX" ending at "stop_pos".
                          PARAMETERS:
                          sourceX: value in which set bit is scanned (X: 8/16/32/64)
                          stop_pos   : stop position for search
                          
Example : 

source32 
      ---------------------------------------------------------------------------------------
     |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
     ---------------------------------------------------------------------------------------
     |0||0||0||0||0||0||0||0||0||0||0||0||1||0||0||0||1||1||0||1||0||0||0||1||1||0||0||0||0|                                                
                                                          --
                                                           |
                                                           ----> stop_pos = 21                                                                    

 return value in this example would be 0  (as none of the bit set before stop_pos=21). Scanning happen from
 31..21
                                  
                                                                                                                      
DEPENDENCIES

RETURN VALUE: 1st occurence of "1" before stop_pos

SIDE EFFECTS: Input mask will get updated   
 ============================================================ */

#define WL1_GET_SET_MSB_POS_TILL_STOP_POS_U64(value, stop_pos) (tds_lmbd64(value, stop_pos))

#define WL1_GET_SET_MSB_POS_TILL_STOP_POS_U32(value, stop_pos) (tds_lmbd32(value, stop_pos))

#define WL1_GET_SET_MSB_POS_TILL_STOP_POS_U16(value, stop_pos)  (tds_lmbd16(value, stop_pos))

#define WL1_GET_SET_MSB_POS_TILL_STOP_POS_U8(value, stop_pos)  (tds_lmbd8(value, stop_pos))         
         



/*  =========================================================
  Macro          : U8 U16 U32 U64 WL1_UX_SET_BIT(sourceX, offset)

  DESCRIPTION:  Set to '1' the bit numbered 'offset' in the value pointed
                          by sourceX and let all other bits unchanged.
                          PARAMETERS:
                          sourceX: value in which the bit is set (X: 8/16/32/64)
                          Offset   : Index of the bit. 
                          
Example : Offset = 21

source32
---------------------------------------------------------------------------------------
|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
---------------------------------------------------------------------------------------
                                                     | 1|

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS: Input mask will get updated   
 ============================================================ */

#define WL1_SET_BIT_U64(source64, offset)  (source64) = (((uint64) (source64)) | ((uint64) ((uint64)1 << (offset))))
   

#define WL1_SET_BIT_U32(source32, offset) (source32) =  (((uint32) (source32)) | ((uint32) (1 << (offset))))


#define WL1_SET_BIT_U16(source16, offset)  (source16) = (((uint16) (source16)) | ((uint16) (1 << (offset))))
        

#define WL1_SET_BIT_U8(source8, offset) (source8) = (((uint8) (source8)) | ((uint8) (1 << (offset))))
  

        

/*  =========================================================
  Macro          : U8 U16 U32 U64 WL1_UX_CLR_BIT(sourceX, offset)

  DESCRIPTION:  Set to '0' the bit numbered 'offset' in the value pointed
                          by sourceX and let all other bits unchanged.
                          PARAMETERS:
                          sourceX: value in which the bit is set (X: 8/16/32/64)
                          Offset   : Index of the bit.
 
                          
Example : Offset = 21

source32
---------------------------------------------------------------------------------------
|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
---------------------------------------------------------------------------------------
                                                     | 0|

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS: Input mask will get updated  
============================================================ */
#define WL1_CLR_BIT_U64(source64, offset) (source64) = (((uint64) (source64)) & (~((uint64) (((uint64)1) << (offset)))))

#define WL1_CLR_BIT_U32(source32, offset)    (source32) = (((uint32) (source32)) & (~((uint32) (1 << (offset)))))

#define WL1_CLR_BIT_U16(source16, offset)  (source16) =  (((uint16) (source16)) & (~((uint16) (1 << (offset))))) 

#define WL1_CLR_BIT_U8(source8, offset)   (source8) =  (((uint8) (source8)) & (~((uint8) (1 << (offset))))) 

 
/*  =========================================================
  Macro          : U8 U16 U32 U64 WL1_GET_BIT_UX(sourceX, offset)

  DESCRIPTION: Get the value of the bit numbered 'offset' in sourceX
                         and place this bit in LSB of the return value, padding
                         the MSB with '0'
                         PARAMETERS:
                         sourceX: value from which bit needs to be read (X: 8/16/32/64)
                         offset   : Index of the bit.
 
                          
Example : Offset = 23

source32
|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
---------------------------------------------------------------------------------------
                                      | z|
                                      ----
                                        |
                                         ----------------- > > > ----------------------
                                                                                       |
 result of macro                                                                   ---
  ---------------------------------------------------------------------------------------
  |0 | 0|                       ...0...                                           |0|0|z|
  ---------------------------------------------------------------------------------------


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS: Input mask will get updated  
============================================================ */

#define WL1_GET_BIT_U64(source64, offset)  ((((uint64) (source64)) >> (offset)) & 0x1)       

#define WL1_GET_BIT_U32(source32, offset)  ((((uint32) (source32)) >> (offset)) & 0x1)

#define WL1_GET_BIT_U16(source16, offset)  ((((uint16) (source16)) >> (offset)) & 0x1)

#define WL1_GET_BIT_U8(source8, offset)     ((((uint8) (source8)) >> (offset)) & 0x1)        

#define WL1_GET_LOWER_WORD_FROM_DWORD(source32) (((uint32)(source32) & 0xFFFF))

#define WL1_GET_UPPER_WORD_FROM_DWORD(source32) (((uint32)(source32) >> 16))

#ifdef  FEATURE_TDSCDMA_PLT
/* WCDMA defines for interrupt lock and task lock. */
/********************************************************************/
#if 0 // remove this session 
#define TDS_INTLOCK() INTLOCK()
#define TDS_INTFREE() INTFREE()
#define TDS_INTLOCK_SAV(sav) INTLOCK_SAV(sav)
#define TDS_INTFREE_SAV(sav) INTFREE_SAV(sav)

#define TDS_TASKLOCK() TASKLOCK()
#define TDS_TASKFREE() TASKFREE()
#define TDS_FIQLOCK() FIQLOCK()
#define TDS_FIQFREE() FIQFREE()
#endif /* FEATURE_TDSCDMA_PLT*/
#endif //remove this session 
#endif

