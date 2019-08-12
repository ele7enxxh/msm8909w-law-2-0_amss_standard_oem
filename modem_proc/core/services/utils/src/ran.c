
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         P S E U D O - R A N D O M    N U M B E R    S E R V I C E S

GENERAL DESCRIPTION
  This module contains functions and variables to support the generation
  of a pseudo-random sequence of numbers in accordance with the algorithm
  described in section 2.6.9 of the CDMA Digital CAI Standard. This algo-
  rithm is also presented, in great detail, in an article entitled "Random
  Number Generators: Good Ones Are Hard To Find", by Stephen K. Park and
  Keith W. Miller, Communications of the ACM, October 1988, Volume 31,
  Number 10.

  The algorithm uses the following iteration:

    z(n) = a x z(n-1) mod m

  where z(n) are integers, a = 7**5 = 16807, m = 2**31 - 1 = 2147483647.

  A generator based on this algorithm is known formally as a prime
  modulus multiplicative linear congruential generator.

  ---------------

  It also contains a function to perform a hash of the mobile ESN and some
  other relevant inputs:

    R = floor( N* (( 40503 * ( L ^ H ^ DECORR )) % 65536 ) / 65536 )

    L is bits 0-16 of the ESN, H is bits 16-31 of the ESN
    N and DECORR are chosen depending on the application ( CAI 2-216 )

  This formula is adapted from Knuth, "The Art of Computer Programming,
  Vol 3, Sorting and Searching," ( Reading, Mass: Addison-Wesley, 1973 ),
  pp. 508-513


EXTERNALIZED FUNCTIONS
  ran_seed - for seeding the pseudo-random number generator.
  ran_next - for generating the next pseudo-random number.
  ran_dist - for pseudo-randomly picking a number from a range of numbers.

  hash     - for generating the hashed number

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Prior to using the other ran_* services, function ran_seed should
  be called to seed the generator.

  Note: these services are re-entrant, i.e., they may be called concurrently
  from multiple tasks and/or interrupt handlers.

Copyright (c) 1990-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/ran.c_v   1.0   Aug 21 2000 11:51:08   lpetrucc  $
$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/ran.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/13   rks     CR540459:Replace memcpy with memscpy
04/06/07    ck     Removed target specification that was introduced by the 
                   UMTS team
12/23/04    ty     Added ran_init() to init ran_seed as function of PMIC time
                   and IMEI.
04/09/99    ms     Lint cleanup.
03/17/99    ms     Changes for Virtual Boot Block.
                   Removed bb_ prefix
                   Not including bbjmpout.h
12/16/98   jct     Removed 80186 support
09/21/98   jct     Incorporated ARM changes, and correct commenting style
09/01/98   jct     Incorporated cdecl changes to allow Pascal switch to be
                   used with compiler
01/12/98   kss     Changed initialized data declaration to use the new
                   BOOT_DATA() macro.
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/28/96   dna     Prepended bb_ to function names and moved module into the
                   Boot Block.
10/31/94   rdh     Removed explicit "far" decl causing glnk segment probs.
10/24/94   rdh     Substituted "mov"s for "lds"- compiler bungles far ptrs.
06/24/94   jah     changed ran_reg to far to fix assembly bug with near/far.
07/23/92   gb      DMSS port.
09/24/91   rrd     Added Hashing
05/06/91   rdb     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "qurt.h"
#include "comdef.h"
#include "ran.h"
#include "msg.h"
#include "assert.h"
#include <stringl/stringl.h>

#ifdef FEATURE_SET_RAN_SEED
#ifdef USES_SEC
#include "secutil.h"
#endif
#include "nv.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


#define  RAN_A_VAL  16807
  /* This defines the 'a' value used by the generator. */

#define  RAN_M_VAL  2147483647
  /* This defines the 'm' value used by the generator. */

#define RANDOM_NUMBER_BYTES 4
/*Amount of Random Data */

dword ran_reg = 1;

static int ran_initialized = 0;

#ifdef FEATURE_BLAST
  static qurt_mutex_t *ran_q6_mutex = NULL;

  /*function protect the critical section of ran code*/
  static void ran_qmutex_init(void);

  /*MACRO for locking/unlocking the qmutex*/
  #define LOCK_RAN_MUTEX()  \
  if (NULL==ran_q6_mutex)       \
    ran_qmutex_init();         \
  qurt_pimutex_lock(ran_q6_mutex); \
  
  #define UNLOCK_RAN_MUTEX()  \
  qurt_pimutex_unlock(ran_q6_mutex); \

#else 
  static rex_crit_sect_type ran_crit_sect;
  static int                ran_crit_sect_initialized = 0;            

  /*MACRO for locking/unlocking the ran mutex*/
  #define LOCK_RAN_MUTEX()  \
  if (0==ran_crit_sect_initialized)       \
  {                                       \
    rex_init_crit_sect(&ran_crit_sect);   \
    ran_crit_sect_initialized = 1;        \
  }                                       \
  rex_enter_crit_sect(&ran_crit_sect); \
  
  #define UNLOCK_RAN_MUTEX()  \
  rex_leave_crit_sect(&ran_crit_sect); \

#endif


/*===========================================================================

FUNCTION RAN_INIT

DESCRIPTION
  This function initializes the pseudo-random number generator with
  the combination of the PMIC RTC time and the phone's IMEI. This
  in an attempt to use a different seed for every mobile, and keep the
  seed consistent to give better random number distribution.
  This function is called from tmc_libraries_init_after_tasks_start()

DEPENDENCIES
  ran_seed
  nv_read
  pm_rtc_rw_cmd

RETURN VALUE
  None.

SIDE EFFECTS
  The random seed is initialized by ran_seed
===========================================================================*/
void ran_init
(
    /* Calling task's TCB pointer */
    rex_tcb_type  *task_ptr,
    /* Task signal to wait for when reading NV */
    rex_sigs_type task_wait_sig,
    /* Task's wait fn to be called when reading NV */
    void          (*task_wait_handler)(rex_sigs_type)

)
{
#ifdef FEATURE_SET_RAN_SEED
  nv_cmd_type         nv_cmd_buf; /* to be used in TMC */
  nv_item_type        nv_data_buf; /* NV data buffer */
  uint8   random_seed[RANDOM_NUMBER_BYTES];

  /*start off the seed with some random value*/
  uint32              seed = timetick_get();

  /*For apps processor, security module is not defined and hence we can just read
   the seed from sclk*/
  #if (defined IMAGE_APPS_PROC || (defined T_MDM9X00 && !defined FEATURE_QDSP6)) 
  if(seed)
  {
    seed = timetick_get();
  }
  else /*if not successful, then try to get it from IMEI*/
  #else /*for modem processor, or Q6 on 9K*/
  /*try to get random number from security module*/
  #ifdef USES_SEC 
  if(E_SUCCESS == secutil_get_random(random_seed, (uint16)RANDOM_NUMBER_BYTES))
  {
    /*copy over the random seed value to seed*/
    memscpy(&seed, sizeof(uint32), &random_seed[0],RANDOM_NUMBER_BYTES);
  }
  else /*if not successful, then try to get it from IMEI*/
  #endif
  #endif
  {    
    if ((task_ptr == NULL) || (task_wait_sig == NULL) || (task_wait_handler == NULL) )
    {
      MSG_HIGH("ran_init() NV_READ can't run, seeded with just time", 0, 0, 0 );

      LOCK_RAN_MUTEX();
      ran_seed( seed );
      ran_initialized = 1;
      UNLOCK_RAN_MUTEX();
      return;
    }
    /* time to use NV services to read IMEI */
    /* set up command buffer */
    nv_cmd_buf.cmd = NV_READ_F;                /* Read request         */
    nv_cmd_buf.tcb_ptr = task_ptr;             /* Notify back to me    */
    nv_cmd_buf.sigs = task_wait_sig;           /* With this signal     */
    nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
    nv_cmd_buf.item = NV_UE_IMEI_I;            /* Item to get          */
    nv_cmd_buf.data_ptr = &nv_data_buf;            /* Where to return it   */
  
    /* Clear signal, issue the command, and wait for the respone. */
    /* Clear signal for NV  */
    (void) rex_clr_sigs(task_ptr, task_wait_sig);
  
    /* Issue the request    */
    nv_cmd(&nv_cmd_buf);
  
    /* Wait for completion  */
    task_wait_handler(task_wait_sig);
  
    /* clear the signal again */
    (void) rex_clr_sigs( task_ptr, task_wait_sig );
  
    /* Now, IMEI should have been read in */
    if (nv_cmd_buf.status != NV_DONE_S )
    {
      MSG_HIGH("ran_init() Random Seed Init failed to read IMEI from NV", 0, 0, 0);
    }
    else
    {
      uint32 IMEI_value=0;
      uint32 multiplier=1;
  
      int bcd_idx, bcd_len, bcd_end;
      bcd_len = nv_data_buf.ue_imei.ue_imei[0]; /* first item is the length */
      /* if bcd_len==0, then IMEI is unset, but IMEI_value==0, so xor with 0 */
      if (bcd_len >= NV_UE_IMEI_SIZE)
      {
        bcd_len=0;
        MSG_HIGH("ran_init() Random Seed Init failed to read valid IMEI from NV", 0, 0, 0);
      }
      /* ue_imei[1] is IMEI identifier, expected to be always 0x0A, not used */
      /* limit use of IMEI to decimal 3,000,000,000 to avoid overflow */
      bcd_end = (bcd_len - 4 > 0) ? bcd_len-4 : 0;
  
      for (bcd_idx = bcd_len ; bcd_idx >= bcd_end ; bcd_idx--)
      {
        /* IMEI stored in BCD Big Endian */
        IMEI_value += ((nv_data_buf.ue_imei.ue_imei[bcd_idx] & 0xF0) >> 4 ) * multiplier;
        multiplier *= 10;
        if (bcd_idx == bcd_end)
          IMEI_value += (nv_data_buf.ue_imei.ue_imei[bcd_idx] & 0x0F) % 4 * multiplier;
        else
          IMEI_value += (nv_data_buf.ue_imei.ue_imei[bcd_idx] & 0x0F) * multiplier;
        multiplier *= 10;
      }
      seed ^= IMEI_value;
    }
  }
  /* Seed the random function with time from clk. */
   LOCK_RAN_MUTEX();
   ran_seed( seed );
   ran_initialized = 1;
   UNLOCK_RAN_MUTEX();
#else
  MSG_HIGH("ran_init(): not implemented",0,0,0);

  /*if the feature is not defined, set the initialized flag
   to one, so that it stops the seed being set to sclk*/
  ran_initialized = 1;
#endif
  
}


/*===========================================================================

FUNCTION RAN_SEED

DESCRIPTION
  This function seeds the pseudo-random number generator with a specified
  value. It should be called prior to using the other pseudo-random number
  services.

DEPENDENCIES
  This function stores the specified seed into the variable ran_reg,
  defined global to this module.

RETURN VALUE
  None.

SIDE EFFECTS
  The ran_reg variable is set to the specified seed value. This influences
  the sequence of pseudo-random numbers that will be generated.
===========================================================================*/

void ran_seed
(
  dword  seed
    /* This value is used to seed the pseudo-random number generator.
       It MUST be in the following range: 0 < seed < 2**31 - 1. Notice
       that the seed must NOT be zero and must NOT be 2**31 - 1. */
)
{
  /* ensure seed is within expected range and if the seed == 1 
   then populate it with sclk time */
  if(1 != seed)
  {
    seed = seed >> 1;
  }
  else /*if seed == 1*/
  {
    /*randomize the seed using if seed == 1*/
    seed = timetick_get();
  }
  
  if (seed == 0x7FFFFFFFL)
  {
    seed = seed & ((uint32)0xFFFFFFFEL);
  }

#ifdef _lint
  seed = seed;
#endif

  LOCK_RAN_MUTEX();
  ran_reg = seed << 1;
  UNLOCK_RAN_MUTEX();

  return;
} /* END ran_seed */


/*===========================================================================

FUNCTION RAN_NEXT

DESCRIPTION
  This function performs the next iteration for the pseudo-random number
  sequence and returns the resulting value.

DEPENDENCIES
  This function uses the variable ran_reg which is declared global to this
  module. Prior to calling this function, the ran_reg should be seeded by
  calling function ran_seed.

RETURN VALUE
  This function a returns a left justified, 31-bit psuedo-random dword
  representing a binary fraction in the range: 0 < binary fraction < 1.
  Notice that bit zero of the return value will always be zero. Typically,
  the return value of this function is used as input to function ran_dist
  for selecting a random number from a specified range of numbers.

  If the return value of this function is to be used directly, then the
  most significant bits should be used as they will be more random than
  the least significant bits.

SIDE EFFECTS
  The variable ran_reg will be set to the next number in the pseudo-random
  sequence.

===========================================================================*/

dword ran_next ( void )
{
  /*------------------------------------------------------------------------
    This function calculates the next value of ran_reg using the formula:

          z(n) = a * z(n-1) mod( 2^31 - 1 )

    The value of z(n) is calculated as follows using a 32-bit wordsize:

    1. Shift z(n-1) left by one bit so it is right justified.

    2. Multiply a*z(n-1) and place the result into two 32-bit words called
       az_hi and az_lo.

       At this point, we have:   a*z(n-1) = az_hi*(2^32) + az_lo

    3. Keep the lower 31 bits of the product in the 32-bit word az_lo, and
       place the remaining upper 15 bits in the 32-bit word az_hi.

       At this point, we have:   a*z(n-1) = az_hi*(2^31) + az_lo
       which can be rewritten:   a*z(n-1) = az_hi*(2^31-1) + (az_lo+az_hi)

    4. To get a*z(n-1)mod(2^31-1), add az_hi to az_lo and check to see if
       the result is equal to or greater than 2^31-1.  If it is, subtract
       off 2^31-1. (This is why z(n-1) was right justified in step 1)

    5. Finally, shift the result left by one bit to make z(n) a left
       justified binary fraction again.
  ------------------------------------------------------------------------*/

  dword az_hi,az_lo,temp_ran_reg;

  /* Shift ran_reg left one bit so we can use MSB as an overflow bit
  */

  LOCK_RAN_MUTEX();
/*if FEATURE_SET_RAN_SEED is not defined, we do not want to 
 set the seed since it is assumed that the clients of ran.c will
 set the seed themselves */
#ifdef FEATURE_SET_RAN_SEED
  if(0 == ran_initialized)    
  {
    /* Init RAN if not already done */
    ran_init(NULL,0,NULL);
  }  
#endif


  ran_reg >>= 1;

  /* Multiply a*z(n-1), putting the lower 31 bits into az_lo, and the
  ** remaining upper bits into az_hi
  */

  az_lo = RAN_A_VAL * (ran_reg & 0xFFFF);
  az_hi = RAN_A_VAL * (ran_reg >> 16) + (az_lo >> 16);

  az_lo = ( (az_hi << 16) | (az_lo & 0xFFFF) ) & RAN_M_VAL;
  az_hi >>= 15;

  /* Add the upper bits to the lower bits to get a*z(n-1)mod(2^31-1), if the
  ** result is not less than the modulus, subtract the modulus from the result
  */

  az_lo += az_hi;

  if ( az_lo >= RAN_M_VAL ){
    az_lo -= RAN_M_VAL;
  }

  /* Place the result into ran_reg and shift it back to a left justified
  ** binary fraction
  */

  ran_reg = az_lo << 1;
  temp_ran_reg = ran_reg;

  UNLOCK_RAN_MUTEX();

  return temp_ran_reg;

} /* END ran_next */


/*===========================================================================

FUNCTION RAN_DIST

DESCRIPTION
  This function chooses a word value from a specified range of values based
  on a specified binary fraction multiplier.

DEPENDENCIES
  None.

RETURN VALUE
  An integer word value chosen from the specified range of numbers. The
  return value will be in the range: lo_val <= return value < hi_val, where
  lo_val and hi_val are specified parameters to the function. Notice that
  the return value may be equal to lo_val, but will be less than hi_val,
  unless of course lo_val = hi_val.

SIDE EFFECTS
  None.

===========================================================================*/

word ran_dist
(
  dword  ran_num,
    /* 31-bit, left justified, binary fraction representing a value in the
       range: 0 < ran_num < 1. Typically, this value will be obtained from
       function ran_next. */

  word  lo_val,
    /* Inclusive lower limit for the integer range over which to pick. */

  word  hi_val
    /* Exclusive upper limit for the integer range over which to pick. */
)
{
  dword result;

/*if FEATURE_SET_RAN_SEED is not defined, we do not want to 
 set the seed since it is assumed that the clients of ran.c will
 set the seed themselves */
#ifdef FEATURE_SET_RAN_SEED
  LOCK_RAN_MUTEX();
  if(0 == ran_initialized)    
  {
    /*set the seed manually. Pass a value of 1 so that the seed 
     will be reset to timetick_get() in ran_seed*/
    ran_seed(1);
  }
  UNLOCK_RAN_MUTEX();
#endif

  /*------------------------------------------------------------------------
    This function calculates a random integer within a given range using
    the formula:

    result = ran_num * ( hi_val - lo_val ) + lo_val

    The random number ran_num is a left justified, 32-bit binary fraction.
    When it is multiplied by a 16-bit integer representing the input range,
    the resulting extended precision real number will have the radix point
    to the left of the lower 32 bits.  This result can be truncated to an
    integer by simply discarding the lower 32-bit word of the product.
  ------------------------------------------------------------------------*/

  /* Multiply the low word of ran_num by the input range
  */

  result = (0xFFFF & ran_num) * (hi_val - lo_val);

  /* Multiply the high word of ran_num by the input range and add in the
  ** high word of the previous result to get the upper dword of the product
  */

  result = (result >> 16) + (ran_num >> 16) * (hi_val - lo_val);

  /* Truncate the product to an integer word and add the offset
  */

  result = (result >> 16) + lo_val ;

  return (word)result;  /* Loss of precision is OK. See comment above. */

} /* END ran_dist */


/*===========================================================================

FUNCTION HASH

DESCRIPTION
  Return a hash of the mobile ESN and other selected inputs ( see CAI 2-216 )
    R = floor( N* (( 40503 * ( L ^ H ^ DECORR )) % 65536 ) / 65536 )

DEPENDENCIES
  None.

RETURN VALUE
  A number between 0 and N-1.  Note that in some cases the CAI requires that
  1 be added to this.  This is up to the caller.

SIDE EFFECTS
  None.

===========================================================================*/

word hash
(
  dword hash_key,                 /* hash key */
  word  n,                        /* range given by CAI */
  word  decorr                    /* parameter from CAI */
)
{
  word temp;

  temp = (decorr ^ ( hash_key & 0xFFFF ) ^ (( hash_key>>16 ) & 0xFFFF ));
  temp = ( temp * 40503L ) & 0xFFFF;
  temp = ( (long) temp * n ) >> 16;
  return( temp );
} /* END hash */


#ifdef FEATURE_QDSP6
/*=============================================================================

FUNCTION ran_QMUTEX_INIT

DESCRIPTION
  This function initializes the qmutex that will be used for protecting
  the crit-section portion of ran code.

DEPENDENCIES
  This function is applicable only for BLAST.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void ran_qmutex_init(void)
{

  ran_q6_mutex = (qurt_mutex_t*)malloc(sizeof(qurt_mutex_t));
  if(ran_q6_mutex == NULL)
  {
   ASSERT(0);
   return;
  }
  qurt_pimutex_init(ran_q6_mutex);
  
}

#endif

