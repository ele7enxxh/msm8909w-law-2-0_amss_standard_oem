#ifndef L1_MATH_UTIL_H
#define L1_MATH_UTIL_H

/*============================================================================
                     L 1 _ M A T H _ U T I L _ H
DESCRIPTION
  This file contains the declarations for the math utilities used by L1.

Copyright (c) 2000 -2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1mathutil.h_v   1.7   10 Dec 2001 13:26:22   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsmathutil.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/25/12    mr       Fixed Compiler warning
05/03/10    rc       Added function prototype for tdsmath_sort_exchange()
09/02/09    sa       Added a comparison function for comparing two 64-bit 
                     unsigned integes.
01/22/07    stk      Modified ABS macro as a function call to avoid lint error
12/05/06    gs       Fixed ROUND macro
03/09/06    au       Moved and renamed quicksort and bubblesort functions 
                     from dlutil
02/13/06    asm      Added bin_srch_is_retval_floor macro
02/13/06    asm      Added generic binary search utility and related comparison
                     functions for the basic types
12/13/05    mc       Moved macro for UNNORMALIZE from l1cmmgr.h to here
06/10/05    vp       Added declaration of tdsmath_log2_interf functions
05/11/05    kps      lint error fix, protect macro arguments of ABS with brackets
08/30/04    gs       Added declaration for macro and function for modulo 2 sum
12/10/01    gs       Protected the macro arguments with brackets
09/20/01    asr      Externalized decl of tdsmath_sub_sfn, tdsmath_add_sfn, and tdsmath_add_cfn
                     which perform mod 4096, mod 256  arithmetic
07/17/01    gs       Added macro and function declaration for BLER LOG function
06/18/01    gs       Added the macro and function declaration for log function.
04/19/01    gs       Added SIGN and ROUND functions
10/30/00    gs       Created File
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsl1def.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define TO_FLOAT_Q3(x)      ((int32) ((x) << 3))
#define FLOAT_Q3_TO_INT(x)  ((x) >> 3)
#define FLOAT_Q3_CEIL(x)    (((x) & 0x7) ? (((x) >> 3) + 1) : (x) >> 3)
#define FLOAT_Q3_FLOOR(x)   (x >> 3)

#define CEIL(x,y)            tdsmath_ceil(x, y)
#define FLOOR(x,y)           tdsmath_floor(x, y)
#define CEIL64(x,y)          tdsmath_ceil_64(x, y)
#define FLOOR64(x,y)         tdsmath_floor_64(x, y)

#ifndef ABS
#define ABS(x)               tdsmath_abs(x)
#endif
#define SIGN(x)              (((x) < 0) ? -1 : 1)
#define ROUND(x,y)           FLOOR(((2 * (x)) + (y)), (2 * (y)))

#define BIG_TO_LITTLE_ENDIAN_32(x) tdsmath_big_to_little_endian_32(x)
#define REV_BYTE_ODR_32(x)         tdsmath_big_to_little_endian_32(x)

#define LOG2_X100(x)         tdsmath_log_base_2_x100(x)
#define LOG_BLER(x,y)        tdsmath_bler_log(x,y)

#define SUM_MODULO_2(x,len)        tdsmath_sum_modulo_2(x, len)
#define EXTRACT_BITS_FROM_BUF(buf,offset_ptr,len) \
  tdsmath_extract_bits_from_buf(buf,offset_ptr,len)

#define UNNORMALIZE(num_to_un_norm, un_norm_ref, un_norm_len) \
  (((num_to_un_norm) < (un_norm_ref)) ? \
    (un_norm_len + (num_to_un_norm)) : (num_to_un_norm))

/*Binary Search macro definition */
#define TDSMATH_BIN_SRCH_FLOOR_CEIL_BMSK	0x01 /* 0 - Floor, 1 - Ceiling */
#define BIN_SRCH_RETVAL_FLOOR		0x00
#define BIN_SRCH_RETVAL_CEIL		0x01
#define TDSMATH_BIN_SRCH_IS_RETVAL_CEIL(a)	(a & TDSMATH_BIN_SRCH_FLOOR_CEIL_BMSK )
#define TDSMATH_BIN_SRCH_IS_RETVAL_FLOOR(a)	!( TDSMATH_BIN_SRCH_IS_RETVAL_CEIL(a) )

#define TDSMATH_BIN_SRCH_ORDER_BMSK 		0x02 /* 0 - Descending, 1 - Ascending */
#define BIN_SRCH_INPUT_ASC_ORDER	0x02	
#define TDSMATH_BIN_SRCH_INPUT_DESC_ORDER	0x00
#define TDSMATH_BIN_SRCH_IS_INPUT_ASC_ORDER(a)	(a & TDSMATH_BIN_SRCH_ORDER_BMSK)

typedef enum{
 TDSMATH_BIN_SRCH_EQUAL   = -1,
 TDSMATH_BIN_SRCH_GREATER =  0,
 TDSMATH_BIN_SRCH_LESSER  =  1
}tdsmath_bin_srch_comp_func_return_type;

typedef enum{
/* tdsmath_binsrch function call has error in arguments */
  TDSMATH_BIN_SRCH_ERR_ARG,

  /* tdsmath_binsrch function search target is 
   * lesser than the least value in table
   */
  TDSMATH_BIN_SRCH_ERR_UNDERFLOW,
 
  /* tdsmath_binsrch function search target is 
   * greater than the greatest value in table
   */
  TDSMATH_BIN_SRCH_ERR_OVERFLOW,

  /* tdsmath_binsrch function has found the target */
  TDSMATH_BIN_SRCH_FOUND,

  /* tdsmath_binsrch function has NOT found the target 
   * and is returning either floor or ceiling values 
   */
  TDSMATH_BIN_SRCH_CEILING,
  TDSMATH_BIN_SRCH_FLOOR

}tdsmath_binsrch_return_status_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef int32 FLOAT_Q3;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
typedef int8 tdsl1_sort_cmp_fn_type(const void*, const void*);
#endif
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION        TDSL1_QCSORT

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void tdsmath_qcsort
(
  /* ptr to base of array to be sorted */
  void *b_addr,

  /* index of first element to be sorted */
  int16 first_idx,

  /* index of last element to be sorted */
  int16 last_idx,

  /* size of elements in bytes */
  uint16 elem_size,

  /* compare funct */
  int8 (*comp_func) (const void *, const void *)
);

/*===========================================================================

FUNCTION        tdsmath_bbsort

DESCRIPTION     This is a buble sort algorithm

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The array whose address was passed in the *b_addr variable
                is sorted in place.  If this is a shared structure make
                sure it is locked before calling this routine.

===========================================================================*/
void tdsmath_bbsort
(
  /* ptr to base of array to be sorted */
  void *b_addr,     
  
  /* index of first element to be sorted */
  int16 first_idx,  

  /* index of last element to be sorted */
  int16 last_idx,   

  /* size of elements in bytes */
  uint16 elem_size,  

  /* compare funct */
  int8 (*comp_func) (const void *, const void *)  
);


/*===========================================================================

FUNCTION        TDSL1_SORT_EXCHANGE

DESCRIPTION     This is an exchange function that is used in the 
                sorting algorithms

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The array whose address was passed in the *b_addr variable
                is sorted in place.  If this is a shared structure make
                sure it is locked before calling this routine.

===========================================================================*/
/* exchange routine to swap two elements */
extern void tdsmath_sort_exchange(void *a, void *b, uint16 size);

/*=========================================================================

FUNCTION tdsmath_ceil

DESCRIPTION
  This function returns the ceiling value of the quotient obtained in a 
  division operation. The rounding is towards +infinity as described in 
  TS 25.212/3.2.

DEPENDENCIES
  None

RETURN VALUE
  ceil value of the division operation

SIDE EFFECTS
  None

=========================================================================*/

extern int32 tdsmath_ceil(
  /* dividend */
  int32 dividend,
  /* divisor */
  int32 divisor);

/*=========================================================================

FUNCTION tdsmath_floor

DESCRIPTION
  This function returns the floor value of the quotient obtained in a
  division operation. The rounding is towards -infinity as described in
  TS 25.212/3.2.

DEPENDENCIES
  None

RETURN VALUE
  floor value of the division operation

SIDE EFFECTS
  None

=========================================================================*/
extern int32 tdsmath_floor(
  /* dividend */
  int32 dividend,
  /* divisor */
  int32 divisor);

/*=========================================================================

FUNCTION TDSMATH_CEIL_64

DESCRIPTION
  This function returns the ceiling value of the quotient obtained in a 
  division operation. The rounding is towards +infinity as described in 
  TS 25.212/3.2.

DEPENDENCIES
  None

RETURN VALUE
  ceil value of the division operation

SIDE EFFECTS
  None

=========================================================================*/
extern int32 tdsmath_ceil_64(
  /* dividend */
  int64 dividend,
  /* divisor */
  int64 divisor);

/*=========================================================================

FUNCTION TDSMATH_FLOOR_64

DESCRIPTION
  This function returns the floor value of the quotient obtained in a
  division operation. The rounding is towards -infinity as described in
  TS 25.212/3.2.

DEPENDENCIES
  None

RETURN VALUE
  floor value of the division operation

SIDE EFFECTS
  None

=========================================================================*/
extern int32 tdsmath_floor_64(
  /* dividend */
  int64 dividend,
  /* divisor */
  int64 divisor);

/*=========================================================================

FUNCTION tdsmath_abs

DESCRIPTION
  This function returns the absolute value (or magnitude) of a number. The 
  absolute value of a real number x is x if x is positive, -x if x is negative.

DEPENDENCIES
  None

RETURN VALUE
  absolute value of the number passed

SIDE EFFECTS
  None

=========================================================================*/

extern int32 tdsmath_abs(
  /* number */
  int32 number);
              
/*=========================================================================
FUNCTION tdsmath_big_to_little_endian_32

DESCRIPTION
  This function changes the 32 bit big endian number to little endian
  number by doing the byte swapping.

DEPENDENCIES
  None

RETURN VALUE
  floor value of the division operation

SIDE EFFECTS
  None
=========================================================================*/

extern uint32 tdsmath_big_to_little_endian_32(
  /* big endian word32 */
  uint32 val_bige);

/*=========================================================================
FUNCTION TDSMATH_LOG_BASE_2_X100

DESCRIPTION
  This function changes the 32 bit big endian number to little endian
  number by doing the byte swapping.

DEPENDENCIES
  None

RETURN VALUE
  floor value of the division operation

SIDE EFFECTS
  None
=========================================================================*/

extern uint16 tdsmath_log_base_2_x100(
  /* 16 bit number for which to fing the log */
  uint16 num);

/*=========================================================================
FUNCTION TDSMATH_BLER_LOG

DESCRIPTION
  This function calculate the BLER log value from the ratio of number of
  CRC received to the number of CRC failed.

DEPENDENCIES
  None

RETURN VALUE
  BLER log value.

SIDE EFFECTS
  None
=========================================================================*/

extern uint8 tdsmath_bler_log(
  /* number of CRC received */
  uint32 crc_count,
  /* number of CRC errors */
  uint32 crc_errors);

/*=========================================================================
FUNCTION tdsmath_sub_sfn (ADD_SFN, ADD_CFN)

DESCRIPTION
  This function subtracts (adds) the second parameter from (to) the first and 
  returns the result 
  as an sfn (i.e modulo 4096, modulo 256 for cfn). 
  The first parameter is assumed to be an sfn (0 .. 4095); 
  or a cfn for the tdsmath_add_cfn function.

DEPENDENCIES
  None

RETURN VALUE
  BLER log value.

SIDE EFFECTS
  None
=========================================================================*/

extern uint16 tdsmath_sub_sfn(uint16 sfn, uint16 subtract_by);
extern uint16 tdsmath_add_sfn(uint16 sfn, uint16 add_by);
extern uint8 tdsmath_add_cfn(uint8 cfn, uint8 add_by);

/*=========================================================================
FUNCTION tdsmath_sub_subframenum (sub_frame_num, subtract_by)

DESCRIPTION
  These pure functions subtract the second parameter from the first and 
  returns the result 
  as an sub-frame number(i.e modulo 8192). 
  The first parameter is assumed to be an sub-frame number(0 .. 8191);

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
=========================================================================*/

extern uint32 tdsmath_sub_sub_fn(uint32 sub_fn, uint32 subtract_by);
extern uint32 tdsmath_add_sub_fn(uint32 sub_fn, uint32 add_by);

/*===========================================================================
FUNCTION     tdsmath_time_diff_in_cx8

DESCRIPTION
  This function returns the time difference in unit of cx8 
  from "time_from" to "time_to"

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  time difference in unit of cx8

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 tdsmath_time_diff_in_cx8
(
  tdsl1_time_info_struct_type time_from,
  tdsl1_time_info_struct_type time_to
);
#if 0
/*===========================================================================
FUNCTION     tdsmath_time_abs_diff_in_cx8

DESCRIPTION
  This function returns the abs time difference in unit of cx8 
  from "time_from" to "time_to".
  It always uses the big time minus smaller time.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  time difference in unit of cx8

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 tdsmath_time_abs_diff_in_cx8
(
  tdsl1_time_info_struct_type time_from,
  tdsl1_time_info_struct_type time_to
);
#endif
/*===========================================================================

FUNCTION TDSMATCH_CONVERT_USEC_TO_SC

DESCRIPTION

    
DEPENDENCIES
    None
    
RETURN VALUE
    

SIDE EFFECTS
    None

===========================================================================*/

extern uint32 tdsmath_convert_usec_to_sc(uint32 usec_cnt, uint32 sleep_clk_mHz);

/*===========================================================================
FUNCTION     tdsmath_convert_usec_to_cx8

DESCRIPTION
  This function convert micro-seconds number to cx8

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 tdsmath_convert_usec_to_cx8(uint32 usec_cnt);

/*===========================================================================
FUNCTION     tdsmath_convert_cx8_to_usec

DESCRIPTION
  This function convert cx8 number to micro-seconds

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 tdsmath_convert_cx8_to_usec(uint32 cx8_cnt);
/*===========================================================================
FUNCTION     tdsmath_time_diff_in_cx8

DESCRIPTION
  This function returns the time difference in unit of cx8 
  from "time_from" to "time_to"

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  time difference in unit of cx8

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 tdsmath_time_diff_in_cx8
(
  tdsl1_time_info_struct_type time_from,
  tdsl1_time_info_struct_type time_to
);

/*===========================================================================
FUNCTION     tdsmath_time_diff_in_usec

DESCRIPTION
  This function returns the time difference in unit of micro-seconds 
  from "time_from" to "time_to"

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  time difference in unit of cx8

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 tdsmath_time_diff_in_usec
(
  tdsl1_time_info_struct_type time_from,
  tdsl1_time_info_struct_type time_to
);

/*===========================================================================
FUNCTION     tdsmath_add_cx8

DESCRIPTION
  This function add a time length (in unit of cx8) to a time point, then return
  the new time point

DEPENDENCIES
  None.

PARAMETERS
  time_bef  --- based time point
  delta_cx8 --- the time length

RETURN VALUE
  new time point delta_cx8 after time_bef

SIDE EFFECTS
  None
===========================================================================*/

extern tdsl1_time_info_struct_type tdsmath_add_cx8
(
  tdsl1_time_info_struct_type time_bef, 
  uint32 delta_cx8
);


/*=========================================================================
FUNCTION TDSMATH_SUM_MODULO_2

DESCRIPTION
  This function calculate modulo 2 sum of bits in 32 bit word upto length
  provided as argument to this function. Bits are located at LSB position.

DEPENDENCIES
  None

RETURN VALUE
  Modulo 2 Sum in LSB of return 8 bit number.
  Values 0 or 1

SIDE EFFECTS
  None
=========================================================================*/

extern uint8 tdsmath_sum_modulo_2(
  /* input number for modulo 2 sum */
  uint32 input_num,
  /* length of number to sum */
  uint8 length);

/*=========================================================================
FUNCTION tdsmath_extract_bits_from_buf

DESCRIPTION
  This function extacts number of bits from buffer organised as 32 bits
  array from specified offset in units of bits and return value upto 32 bits.

DEPENDENCIES
  None

RETURN VALUE
  Extracted bits value (upto 32 bits) are returnd as 32 bit with bits in
  LSB position.

SIDE EFFECTS
  Offset provided as pointer to its value is updated based on bit length
  specified to indicate new position in buffer.
=========================================================================*/

extern uint32 tdsmath_extract_bits_from_buf(
  /* Buffer pointer from which to extract bits */
  uint32 *buf,
  /* pointer to offset where bits are extracted. Iffset is updated to
     indicate new bit position of buffer after reading */
  uint16 *offset,
  /* number of bits to extract, must be less than 32 */
  uint8 length);

/*=========================================================================
FUNCTION tdsmath_binsrch

DESCRIPTION
  This is a generic binary search utility function.
  This function takes as input parameters: 
  1. A pointer to the table to be searched,
  2. The number of elements in the table
  3. A pointer to the target to be searched for
  4. A pointer to the function comparing two elements 
     of the given table
  5. 32-bit Options Bitmasks as described below:  

      |	31 |...............| 3 | 2 | 1 | 0 |
      |    |   |   |   |   |   |   |   |   |
      |-   R E S E R V E D  -  - - |   |   |
                                     -   -
                                     |   |
           BIT 1 - ORDER        -----    |
           BIT 0 - RETURN VALUE ---------

  ORDER - 0 => Descending order
          1 => Ascending order

  RETURN VALUE - 0 => Floor
               - 1 => Ceiling 

  The comparison function should be of the form 
       tdsmath_bin_srch_comp_func_return_type (*comp_func)(void*, int, void*)

  A set of comparison functions are provided for basic types.

DEPENDENCIES
  NONE  

RETURN VALUE
  The function returns the index of the target in the table
  If there is no match, the function returns the floor/ceiling based on options
  Underflow returns 0, whereas overflow returns tbl_length

  In case of multiple matches, it is not deterministic which index will be
  returned

  The function also return the status of the search in a return parameter.
  This status indicates whether the search was successful, or if there were any errors.

SIDE EFFECTS
  NONE
=========================================================================*/
int32 tdsmath_binsrch(
  /* Pointer to the table to be searched */
  void *tbl_ptr, 
  /* Number of elements in the table */
  uint32 tbl_length,  
  /* Pointer to the target to be searched for */
  void *target, 
  /* Pointer to the function used to compare elements in the table */
  tdsmath_bin_srch_comp_func_return_type (*comp_func) (void *, int32, void *),
  /* Options Flags */
  uint32 flags,
  /*return status */
  tdsmath_binsrch_return_status_type *status );

/*****************************************************************
 Directions for usage:
 ---------------------

  To use the binary search facility, call the function as suggested 
  by the example below:

  // Set flags   
  uint32 flags = 0;
  flags = BIN_SRCH_RETVAL_FLOOR | BIN_SRCH_INPUT_ASC_ORDER;

  tdsmath_binsrch_return_status_type status;

  // Call binsrch
  tdsmath_binsrch(
	    (void *)&table_to_be_sorted, 
	    (uint32)number_of_elements,  
	    (void *)&target, 
	    tdsmath_binsrch_compare_signed_word,
	    flags,
            &status
	    );

  Here's an example of how to use this utility to search an array of structures 
  over a particular field within the structure:

   typedef struct
   {
	 int first; 
	 int second;
   }mystruct;

  The comparison function would be something like:

   static tdsmath_bin_srch_comp_func_return_type 
       my_struct_comp_func( void *tbl_ptr, int index, void *tgt)
   {
     int elem1 = ((mystruct *)tbl_ptr)[index].first;

     if (elem1 > *(int *)tgt)
     { 
       return TDSMATH_BIN_SRCH_GREATER;
     }
     else if (elem1 < *(int *)tgt)
     {
	   return TDSMATH_BIN_SRCH_LESSER;
     }
     else 
     {
       return TDSMATH_BIN_SRCH_EQUAL;
     }
   }

  The binsrch function will be used as below after setting flags:

  tdsmath_binsrch(
	    (void *)&table_to_be_sorted, 
	    (uint32)number_of_elements,  
	    (void *)&target, 
	    my_struct_comp_func,
	    flags,
            &status 
	    );
*******************************************************************/

/*The Set of Comparison functions to use for basic types */

/*=========================================================================
FUNCTION L1_BINSRCH_COMPARE_UNSIGNED_BYTE
FUNCTION L1_BINSRCH_COMPARE_SIGNED_BYTE
FUNCTION tdsmath_binsrch_compare_unsigned_word
FUNCTION L1_BINSRCH_COMPARE_SIGNED_WORD
FUNCTION tdsmath_binsrch_compare_unsigned_dword
FUNCTION L1_BINSRCH_COMPARE_SIGNED_DWORD
FUNCTION L1_BINSRCH_COMPARE_UNSIGNED_64BITT_WORD

DESCRIPTION
  Comparison functions for the various basic types
  that can be passed to the above binary search utility.

DEPENDENCIES
  None

RETURN VALUE
  TDSMATH_BIN_SRCH_GREATER when the element is greater than the target
  TDSMATH_BIN_SRCH_LESSER  when the element is lesser than the target
  TDSMATH_BIN_SRCH_EQUAL   when the element is found (equal to target)

SIDE EFFECTS
  None
=========================================================================*/
tdsmath_bin_srch_comp_func_return_type 
  tdsmath_binsrch_compare_unsigned_byte( void *tbl_ptr, int32 index, void *tgt);  

tdsmath_bin_srch_comp_func_return_type 
  tdsmath_binsrch_compare_signed_byte( void *tbl_ptr, int32 index, void *tgt);

tdsmath_bin_srch_comp_func_return_type
  tdsmath_binsrch_compare_unsigned_word( void *tbl_ptr, int32 index, void *tgt);

tdsmath_bin_srch_comp_func_return_type
 tdsmath_binsrch_compare_signed_word( void *tbl_ptr, int32 index, void *tgt);

tdsmath_bin_srch_comp_func_return_type
 tdsmath_binsrch_compare_unsigned_dword( void *tbl_ptr, int32 index, void *tgt);

tdsmath_bin_srch_comp_func_return_type
 tdsmath_binsrch_compare_signed_dword( void *tbl_ptr, int32 index, void *tgt);

tdsmath_bin_srch_comp_func_return_type
 tdsmath_binsrch_compare_unsigned_64bit_word( void *tbl_ptr, int32 index, void *tgt);

#ifdef FEATURE_TDSCDMA_HSUPA

extern int16 tdsmath_dbm_sub_Q5(int16 x, int16 y);

extern int16 tdsmath_dbm_add_Q5(int16 x, int16 y);


#endif

extern int32 tdsmath_dbm_add_Q9(int32 m, int32 n);


#endif /* L1_MATH_UTIL_H */
