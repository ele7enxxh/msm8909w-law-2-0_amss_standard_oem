#ifndef L1_MATH_UTIL_H
#define L1_MATH_UTIL_H

/*============================================================================
                     L 1 _ M A T H _ U T I L _ H
DESCRIPTION
  This file contains the declarations for the math utilities used by L1.

Copyright (c) 2000 -2010 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1mathutil.h_v   1.7   10 Dec 2001 13:26:22   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1mathutil.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/14/15    sl       Added new macro to find out max of two numbers
12/06/14    ash      Add new compare function for unsigned word for bin search
04/27/14    jkb      Add check for Feature dual sim with feature qta
10/18/13    as       Continuing port of FEATURE_WCDMA_MODEM_COEXISTENCE_SW
09/29/13    as       Continuing port of Dual Sim from Triton to Dime.
07/22/13    yw       UPH support for DCHSUPA
01/16/13    zr       Adding alignment-aware memory swapping functions
08/24/12    vs       Replacing the usage of the api math_extract_bits_from_buf with 
                     Q6 assembly instructions for processor MIPS savings.
07/24/12   geg       Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/21/12    zr       Added sort comparison function typedef
01/19/12    zr       Mainlined FEATURE_FILTER_INTERF_MEASUREMENTS 
05/03/10    rc       Added function prototype for l1_sort_exchange()
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
06/10/05    vp       Added declaration of log2_interf functions
05/11/05    kps      lint error fix, protect macro arguments of ABS with brackets
08/30/04    gs       Added declaration for macro and function for modulo 2 sum
12/10/01    gs       Protected the macro arguments with brackets
09/20/01    asr      Externalized decl of sub_sfn, add_sfn, and add_cfn
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

#include "comdef.h"
#include "q6protos.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define TO_FLOAT_Q3(x)      ((int32) ((x) << 3))
#define FLOAT_Q3_TO_INT(x)  ((x) >> 3)
#define FLOAT_Q3_CEIL(x)    (((x) & 0x7) ? (((x) >> 3) + 1) : (x) >> 3)
#define FLOAT_Q3_FLOOR(x)   (x >> 3)

#define CEIL(x,y)            math_ceil(x, y)
#define FLOOR(x,y)           math_floor(x, y)
#define CEIL64(x,y)          math_ceil_64(x, y)
#define FLOOR64(x,y)         math_floor_64(x, y)

#define ABS(x)               math_abs(x)
#define SIGN(x)              (((x) < 0) ? -1 : 1)
#define ROUND(x,y)           FLOOR(((2 * (x)) + (y)), (2 * (y)))

#define MAX(x,y)   ((x > y)? x : y)

#define BIG_TO_LITTLE_ENDIAN_32(x) math_big_to_little_endian_32(x)
#define REV_BYTE_ODR_32(x)         math_big_to_little_endian_32(x)

#define LOG2_X100(x)         math_log_base_2_x100(x)
#define LOG_BLER(x,y)        math_bler_log(x,y)

#define SUM_MODULO_2(x,len)        math_sum_modulo_2(x, len)
static inline uint32 EXTRACT_BITS_FROM_BUF(uint32 *buf,uint16 *offset,uint8 length)	 	 	 
	{	 	 	 
	    /*********************************************************************	 	 	 
	    * offset is a bit offset,	 	 	 
            *   so the lower 5 bits are the offset within a 32bit word	 	 	 
	    *   and the remaining bits indicate the word offset	 	 	 
	    *********************************************************************/	 	 	 
	    uint32 bit_offset=((*offset)&0x1F);	 	 	 
	    uint32 word_offset=((*offset)>>5);	 	 	 
	    /*********************************************************************	 	 	 
	    * We want to extract out upto 32 bits, but these could cross a word boundary	 	 	 
	    *  so we read the first word, contains the starting bit(s) and the next word	 	 	 
	    *  combining them into a 64 bit word data64	 	 	 
	    *********************************************************************/	 	 	 
	    uint64 data64=Q6_P_combine_RR(buf[word_offset],buf[word_offset+1]);	 	 	 
	    /*********************************************************************	 	 	 
	    * The extract instruction requires a 64bit control word, into this we	 	 	 
	    *  pack the number of bits and the start bit offset	 	 	 
	    *********************************************************************/	 	 	 
	    uint64 offset64=Q6_P_combine_RR(length,64-bit_offset-length);	 	 	 
	    /*********************************************************************	 	 	 
	    * We can now extract the bits, any unused bits are zero'ed, we extract	 	 	 
	    *  into a 64 bit word, but just return the lower 32 bits	 	 	 
	    *********************************************************************/	 	 	 
	    uint64 result64=Q6_P_extractu_PP(data64,offset64);	 	 	 
	    uint32 result32=(Word32)result64;	 	 	 
	    /*********************************************************************	 	 	 
	    * The bit offset is updated with the number of bits removed	 	 	 
	    *********************************************************************/	 	 	 
	    *offset+=length;	 	 	 
	    return(result32);	 	 	 
	}	 	

#define UNNORMALIZE(num_to_un_norm, un_norm_ref, un_norm_len) \
  (((num_to_un_norm) < (un_norm_ref)) ? \
    (un_norm_len + (num_to_un_norm)) : (num_to_un_norm))

/*Binary Search macro definition */
#define BIN_SRCH_FLOOR_CEIL_BMSK	0x01 /* 0 - Floor, 1 - Ceiling */
#define BIN_SRCH_RETVAL_FLOOR		0x00
#define BIN_SRCH_RETVAL_CEIL		0x01
#define BIN_SRCH_IS_RETVAL_CEIL(a)	(a & BIN_SRCH_FLOOR_CEIL_BMSK )
#define BIN_SRCH_IS_RETVAL_FLOOR(a)	!( BIN_SRCH_IS_RETVAL_CEIL(a) )

#define BIN_SRCH_ORDER_BMSK 		0x02 /* 0 - Descending, 1 - Ascending */
#define BIN_SRCH_INPUT_ASC_ORDER	0x02	
#define BIN_SRCH_INPUT_DESC_ORDER	0x00
#define BIN_SRCH_IS_INPUT_ASC_ORDER(a)	(a & BIN_SRCH_ORDER_BMSK)

#define L1_MATH_LOG_BASE2_NUM 320
extern uint16 math_log_base2_out_x_100[L1_MATH_LOG_BASE2_NUM];

typedef enum{
 BIN_SRCH_EQUAL   = -1,
 BIN_SRCH_GREATER =  0,
 BIN_SRCH_LESSER  =  1
}l1_bin_srch_comp_func_return_type;

typedef enum{
/* L1_Binsrch function call has error in arguments */
  BIN_SRCH_ERR_ARG,

  /* L1_Binsrch function search target is 
   * lesser than the least value in table
   */
  BIN_SRCH_ERR_UNDERFLOW,
 
  /* L1_Binsrch function search target is 
   * greater than the greatest value in table
   */
  BIN_SRCH_ERR_OVERFLOW,

  /* L1_Binsrch function has found the target */
  BIN_SRCH_FOUND,

  /* L1_Binsrch function has NOT found the target 
   * and is returning either floor or ceiling values 
   */
  BIN_SRCH_CEILING,
  BIN_SRCH_FLOOR

}l1_binsrch_return_status_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef int32 FLOAT_Q3;

typedef int8 l1_sort_cmp_fn_type(const void*, const void*);

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION        L1_QCSORT

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void l1_qcsort
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

FUNCTION        L1_BBSORT

DESCRIPTION     This is a buble sort algorithm

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The array whose address was passed in the *b_addr variable
                is sorted in place.  If this is a shared structure make
                sure it is locked before calling this routine.

===========================================================================*/
void l1_bbsort
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

/*=============================================================================
FUNCTION        L1_MEMSWP_8BIT

DESCRIPTION     This function swaps two arbitratily-sized blocks of memory 
                one byte at a time for the given number of bytes. It makes no 
                assumptions about the alignment of the two memory addresses 
                that are passed in.

DEPENDENCIES    Do not call this function directly. Call l1_sort_exchange, 
                which will determine the alignment of the memory addresses
                passed in and route the call to this function, if it is
                the most effective option.

RETURN VALUE    None

SIDE EFFECTS    Swaps the contents of memory pointed to by the two inputs.
=============================================================================*/
void l1_memswp_8bit(void* a, void* b, uint16 size);

/*=============================================================================
FUNCTION        L1_MEMSWP_16BIT

DESCRIPTION     This function swaps two arbitratily-sized blocks of memory 
                two bytes at a time for the given number of bytes. It assumes 
                the memory addresses passed in are halfword-aligned.

DEPENDENCIES    Do not call this function directly. Call l1_sort_exchange, 
                which will determine the alignment of the memory addresses
                passed in and route the call to this function, if it is
                the most effective option.

RETURN VALUE    None

SIDE EFFECTS    Swaps the contents of memory pointed to by the two inputs.
=============================================================================*/
void l1_memswp_16bit(void* a, void* b, uint16 size);

/*=============================================================================
FUNCTION        L1_MEMSWP_32BIT

DESCRIPTION     This function swaps two arbitratily-sized blocks of memory 
                four bytes at a time for the given number of bytes. It assumes 
                the memory addresses passed in are word-aligned.

DEPENDENCIES    Do not call this function directly. Call l1_sort_exchange, 
                which will determine the alignment of the memory addresses
                passed in and route the call to this function, if it is
                the most effective option.

RETURN VALUE    None

SIDE EFFECTS    Swaps the contents of memory pointed to by the two inputs.
=============================================================================*/
void l1_memswp_32bit(void* a, void* b, uint16 size);

/*===========================================================================

FUNCTION        L1_SORT_EXCHANGE

DESCRIPTION     This is an exchange function that is used in the 
                sorting algorithms

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The array whose address was passed in the *b_addr variable
                is sorted in place.  If this is a shared structure make
                sure it is locked before calling this routine.

===========================================================================*/
/* exchange routine to swap two elements */
extern void l1_sort_exchange(void *a, void *b, uint16 size);

/*=========================================================================

FUNCTION MATH_CEIL

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

extern int32 math_ceil(
  /* dividend */
  int32 dividend,
  /* divisor */
  int32 divisor);

/*=========================================================================

FUNCTION MATH_FLOOR

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
extern int32 math_floor(
  /* dividend */
  int32 dividend,
  /* divisor */
  int32 divisor);

/*=========================================================================

FUNCTION MATH_CEIL_64

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
extern int32 math_ceil_64(
  /* dividend */
  int64 dividend,
  /* divisor */
  int64 divisor);

/*=========================================================================

FUNCTION MATH_FLOOR_64

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
extern int32 math_floor_64(
  /* dividend */
  int64 dividend,
  /* divisor */
  int64 divisor);

/*=========================================================================

FUNCTION MATH_ABS

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

extern int32 math_abs(
  /* number */
  int32 number);
              
/*=========================================================================
FUNCTION BIG_TO_LITTLE_ENDIAN_32

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

extern uint32 math_big_to_little_endian_32(
  /* big endian word32 */
  uint32 val_bige);

/*=========================================================================
FUNCTION MATH_LOG_BASE_2_X100

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

extern uint16 math_log_base_2_x100(
  /* 16 bit number for which to fing the log */
  uint16 num);

/*=========================================================================
FUNCTION MATH_BLER_LOG

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

extern uint8 math_bler_log(
  /* number of CRC received */
  uint32 crc_count,
  /* number of CRC errors */
  uint32 crc_errors);

/*=========================================================================
FUNCTION SUB_SFN (ADD_SFN, ADD_CFN)

DESCRIPTION
  This function subtracts (adds) the second parameter from (to) the first and 
  returns the result 
  as an sfn (i.e modulo 4096, modulo 256 for cfn). 
  The first parameter is assumed to be an sfn (0 .. 4095); 
  or a cfn for the add_cfn function.

DEPENDENCIES
  None

RETURN VALUE
  BLER log value.

SIDE EFFECTS
  None
=========================================================================*/

extern uint16 sub_sfn(uint16 sfn, uint16 subtract_by);
extern uint16 add_sfn(uint16 sfn, uint16 add_by);
extern uint8 add_cfn(uint8 cfn, uint8 add_by);
extern uint32 sub_exsfn(uint32 sfn, uint32 subtract_by);
extern uint32 add_exsfn(uint32 sfn, uint32 add_by);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*=========================================================================
FUNCTION SUB_CFN (ADD_CFN, ADD_CFN)

DESCRIPTION
  These pure functions subtract (adds) the second parameter from (to) the first and 
  returns the result 
  as an cfn (i.e modulo 4096, modulo 256 for cfn). 
  The first parameter is assumed to be an cfn (0 .. 4095); or a cfn for the add_cfn function.

DEPENDENCIES
  None

RETURN VALUE
  BLER log value.

SIDE EFFECTS
  None
=========================================================================*/
extern uint8 sub_cfn(uint8 cfn, uint8 subtract_by);
#endif /* FEATURE_QTA */

/*=========================================================================
FUNCTION MATH_SUM_MODULO_2

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

extern uint8 math_sum_modulo_2(
  /* input number for modulo 2 sum */
  uint32 input_num,
  /* length of number to sum */
  uint8 length);

/*=========================================================================
FUNCTION MATH_EXTRACT_BITS_FROM_BUF

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

extern uint32 math_extract_bits_from_buf(
  /* Buffer pointer from which to extract bits */
  uint32 *buf,
  /* pointer to offset where bits are extracted. Iffset is updated to
     indicate new bit position of buffer after reading */
  uint16 *offset,
  /* number of bits to extract, must be less than 32 */
  uint8 length);

/*=========================================================================
FUNCTION LOG2_INTERF

DESCRIPTION
  This function calculates a number 'n' for a range of input 
  parameters('i'): i=(2^n) to i=(((2^(n+1))-1)

DEPENDENCIES
  The input number has to be an unsigned integer greater than zero.

RETURN VALUE
  Pls. see the description above. The function returns 'n'

SIDE EFFECTS
  NONE
=========================================================================*/

extern uint16 log2_interf(uint64 i);

/*=========================================================================
FUNCTION L1_BINSRCH

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
       l1_bin_srch_comp_func_return_type (*comp_func)(void*, int, void*)

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
int32 l1_binsrch(
  /* Pointer to the table to be searched */
  void *tbl_ptr, 
  /* Number of elements in the table */
  uint32 tbl_length,  
  /* Pointer to the target to be searched for */
  void *target, 
  /* Pointer to the function used to compare elements in the table */
  l1_bin_srch_comp_func_return_type (*comp_func) (void *, int32, void *),
  /* Options Flags */
  uint32 flags,
  /*return status */
  l1_binsrch_return_status_type *status );

/*****************************************************************
 Directions for usage:
 ---------------------

  To use the binary search facility, call the function as suggested 
  by the example below:

  // Set flags   
  uint32 flags = 0;
  flags = BIN_SRCH_RETVAL_FLOOR | BIN_SRCH_INPUT_ASC_ORDER;

  l1_binsrch_return_status_type status;

  // Call binsrch
  l1_binsrch(
	    (void *)&table_to_be_sorted, 
	    (uint32)number_of_elements,  
	    (void *)&target, 
	    l1_binsrch_compare_signed_word,
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

   static l1_bin_srch_comp_func_return_type 
       my_struct_comp_func( void *tbl_ptr, int index, void *tgt)
   {
     int elem1 = ((mystruct *)tbl_ptr)[index].first;

     if (elem1 > *(int *)tgt)
     { 
       return BIN_SRCH_GREATER;
     }
     else if (elem1 < *(int *)tgt)
     {
	   return BIN_SRCH_LESSER;
     }
     else 
     {
       return BIN_SRCH_EQUAL;
     }
   }

  The binsrch function will be used as below after setting flags:

  l1_binsrch(
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
FUNCTION L1_BINSRCH_COMPARE_UNSIGNED_BYTE_WORD_TGT
FUNCTION L1_BINSRCH_COMPARE_UNSIGNED_BYTE
FUNCTION L1_BINSRCH_COMPARE_SIGNED_BYTE
FUNCTION L1_BINSRCH_COMPARE_UNSIGNED_WORD
FUNCTION L1_BINSRCH_COMPARE_SIGNED_WORD
FUNCTION L1_BINSRCH_COMPARE_UNSIGNED_DWORD
FUNCTION L1_BINSRCH_COMPARE_SIGNED_DWORD
FUNCTION L1_BINSRCH_COMPARE_UNSIGNED_64BITT_WORD

DESCRIPTION
  Comparison functions for the various basic types
  that can be passed to the above binary search utility.

DEPENDENCIES
  None

RETURN VALUE
  BIN_SRCH_GREATER when the element is greater than the target
  BIN_SRCH_LESSER  when the element is lesser than the target
  BIN_SRCH_EQUAL   when the element is found (equal to target)

SIDE EFFECTS
  None
=========================================================================*/
l1_bin_srch_comp_func_return_type 
 l1_binsrch_compare_unsigned_byte_word_tgt( void *tbl_ptr, int32 index, void *tgt);

l1_bin_srch_comp_func_return_type 
  l1_binsrch_compare_unsigned_byte( void *tbl_ptr, int32 index, void *tgt);  

l1_bin_srch_comp_func_return_type 
  l1_binsrch_compare_signed_byte( void *tbl_ptr, int32 index, void *tgt);

l1_bin_srch_comp_func_return_type
  l1_binsrch_compare_unsigned_word( void *tbl_ptr, int32 index, void *tgt);

l1_bin_srch_comp_func_return_type
 l1_binsrch_compare_signed_word( void *tbl_ptr, int32 index, void *tgt);

l1_bin_srch_comp_func_return_type
 l1_binsrch_compare_unsigned_dword( void *tbl_ptr, int32 index, void *tgt);

l1_bin_srch_comp_func_return_type
 l1_binsrch_compare_signed_dword( void *tbl_ptr, int32 index, void *tgt);

l1_bin_srch_comp_func_return_type
 l1_binsrch_compare_unsigned_64bit_word( void *tbl_ptr, int32 index, void *tgt);

/*=========================================================================
FUNCTION calc_dbm_x10_frm_linear

DESCRIPTION
  This function breaks the input into two parts, the base 16 exponent and 
  the two most significant hex nibbles.
  The log power is then approximately calculated as :
         100 * (log_16(significant nibble) + exponent)) * log_10(16)

 
DEPENDENCIES
  The input number has to be an unsigned integer greater than zero.

RETURN VALUE
  uint16 dB10 equivalent of the input number.

SIDE EFFECTS
  NONE
=========================================================================*/
int16 calc_dbm_x10_frm_linear(uint32 value);

/*=========================================================================
FUNCTION calc_log_base_16_x200

DESCRIPTION
  This function calculates log base 16 of a number times 200.

DEPENDENCIES
  Accepts value only in the range of 1-255. For any other input returns 0.

RETURN VALUE
  uint16 
SIDE EFFECTS
  None
=========================================================================*/
uint16 calc_log_base_16_x200(uint8 num);


#endif /* L1_MATH_UTIL_H */
