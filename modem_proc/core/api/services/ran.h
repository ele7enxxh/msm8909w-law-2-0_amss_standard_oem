#ifndef RAN_H
#define RAN_H
/**
  @file ran.h
  @brief This file contains declarations for use with the Pseudo-random Number
  Services.
*/ 
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_random" 
      group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*=========================================================================== 
This module contains functions and variables to support the generation
 of a pseudo-random sequence of numbers in accordance with the algorithm
 described in section 2.6.9 of the CDMA Digital CAI Standard. This algorithm
 is also presented, in great detail, in an article entitled "Random
 Number Generators: Good Ones Are Hard To Find", by Stephen K. Park and
 Keith W. Miller, Communications of the ACM, October 1988, Volume 31,
 Number 10.

 The algorithm uses the following iteration:

 z(n) = a x z(n-1) mod m

 where z(n) are integers, a = 7**5 = 16807, m = 2**31 - 1 = 2147483647.

 A generator based on this algorithm is known formally as a prime
 modulus multiplicative linear congruential generator.

 ---------------

 This module also contains a function to perform a hash of the mobile ESN and 
 some other relevant inputs:

 R = floor( N* (( 40503 * ( L ^ H ^ DECORR )) % 65536 ) / 65536 )

 L is bits 0-16 of the ESN, H is bits 16-31 of the ESN
 N and DECORR are chosen depending on the application ( CAI 2-216 )

 This formula is adapted from Knuth, "The Art of Computer Programming,
 Vol 3, Sorting and Searching," ( Reading, Mass: Addison-Wesley, 1973 ),
 pp. 508-513

===========================================================================*/
/*===========================================================================

         P S E U D O - R A N D O M    N U M B E R    S E R V I C E S

DESCRIPTION
  This file contains declarations for use with the Pseudo-random Number
  Services.

Copyright (c) 1991-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/ran.h_v   1.0   Aug 21 2000 11:51:08   lpetrucc  $
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/ran.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/16/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
12/02/10   pc      Removed featurization around inclusion of rex.h
11/10/10   EBR     Doxygenated File
12/23/04   ty      Added function prototype for ran_init()
12/16/98   jct     Removed 80186 support
09/21/98   jct     Updated copyright, now includes comdef.h
09/01/98   jct     Added cdecl declaration
05/06/91   rdb     Created module.

===========================================================================*/
/** @addtogroup utils_random
@{ */

#include "comdef.h"

#include "rex.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/**
  Initializes the pseudorandom number generator with a combination of 
  the Power Management Integrated Circuit (PMIC) Real-Time Clock (RTC) time 
  and the phone's International Mobile Equipment Identification (IMEI). 
  This combination is an attempt to use a different seed for every mobile 
  and keep the seed consistent to give a better random number distribution.
  This function is called from tmc_libraries_init_after_tasks_start().
 
  @param[in] task_ptr      Pointer to the calling task's TCB.
  @param[in] task_wait_sig Task signal to wait for when reading from NV.
  @param[in] rex_sigs_type Task's wait function to be called when reading 
                           from NV.

  @return
   None.

  @dependencies
  ran_seed() \n
  nv_read \n
  pm_rtc_rw_cmd \n
  clk_julian_to_secs*/
extern void   ran_init ( rex_tcb_type  *task_ptr,
                         rex_sigs_type task_wait_sig,
                         void (*task_wait_handler)(rex_sigs_type) );

/**
  Seeds the pseudorandom number generator with a specified
  value. This function must be called before using other 
  Random Number Generation Utility Services.

  @param[in] seed Value used to seed the pseudorandom number generator;
                  MUST be in the range of 0 < seed < 2**31 - 1. Note that 
                  the seed must NOT be zero and must NOT be 2**31 - 1.

  @return
  None.

  @dependencies
  This function stores the specified seed in the ran_reg variable, 
  which is defined globally for this module.

  @sideeffects
  The ran_reg variable is set to the specified seed value, which influences 
  the sequence of pseudorandom numbers that will be generated.
*/
extern void   ran_seed ( dword seed );

/**
  Performs the next iteration for the pseudorandom number sequence 
  and returns the resulting value.

  @return
  A left justified, 31-bit psuedorandom dword representing a binary fraction 
  in the range of 0 < binary fraction < 1. Note that bit zero of the return 
  value is always zero. 
  @par
  @latexonly \vspace{-0.15in} @endlatexonly  
  Typically, the return value of this function is used as input to ran_dist()
  for selecting a random number from a specified range of numbers. 
  @par
  @latexonly \vspace{-0.15in} @endlatexonly
  If the return value of this function is to be used directly, the
  most significant bits must be used because they will be more random than
  the least significant bits.
 
  @sideeffects
  The ran_reg variable is set to the next number in the pseudorandom 
  sequence.
*/
extern dword  ran_next ( void );

/**
  Chooses a word value from a specified range of values based
  on a specific binary fraction multiplier.
 
  @param[in] ran_num 31-bit, left justified, binary fraction representing 
                     a value in the range of 0 < ran_num < 1. Typically, this 
                     value is obtained from ran_next().
  @param[in] lo_val  Inclusive lower limit for the integer range.
  @param[in] hi_val  Exclusive upper limit for the integer range.

  @return
  An integer word value chosen from the specified range of numbers. The
  return value is in the range of lo_val <= return value < hi_val, where
  lo_val and hi_val are specified parameters to the function. Note that
  the return value may be equal to lo_val, but will be less than hi_val,
  unless lo_val = hi_val.

  @dependencies
  None.
*/
extern word   ran_dist ( dword  ran_num, word lo_val, word hi_val );


/**
  Returns a hash of the mobile ESN and other selected inputs. The equation is:
  
  R = floor( N* (( 40503 * ( L ^ H ^ DECORR )) % 65536 ) / 65536 )

  Where:
  - L is bits 0 to 16 of the ESN
  - H is bits 16 to 31 of the ESN
  - N and DECORR are chosen depending on the application (per CDMA digital
  Common Air Interface (CAI) standards)

  @param[in] hash_key Hash key.
  @param[in] n        Range specified by the CDMA digital CAI standards.
  @param[in] decorr   Parameter from the CDMA digital CAI standards.

  @return
  A number between 0 and N-1. Note that in some cases, the 
  CDMA digital CAI standards require that 1 be added to this value; 
  adding 1 is up to the caller.

  @dependencies
  None.
*/
extern word   hash     ( dword hash_key, word  n, word  decorr );

/** @} */ /* end_addtogroup utils_random */
#endif /* RAN_H */

