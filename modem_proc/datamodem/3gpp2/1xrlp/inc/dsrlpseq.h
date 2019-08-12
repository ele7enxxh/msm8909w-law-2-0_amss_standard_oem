#ifndef _DSRLPSEQ_H
#define _DSRLPSEQ_H
/*===========================================================================

                     R A D I O   L I N K   P R O T O C O L
              S E Q U E N C E  N U M B E R  H E A D E R  F I L E

DESCRIPTION
  This header file contains various #defines and macros used to manipulate
  the internal 32-bit sequence number.  The sequence number format is 
  designed to be used by RLP 1, RLP 2, RLP 3 and RLP-HDR.
  
  
  FORMAT 
  
  The sequence number format for all RLP versions is shown below:
  
  
         MSB                                    LSB
          |                                      |
          0000  aaaa  bbbbcccc  ddddeeeeffff  0000
          
  where:
  
    0000         = fixed bit values
    aaaa         = 4-bit SEQ_HI value  (RLP 2 & 3)
    bbbbcccc     = 8-bit SEQ           (RLP 1,2,3)
    ddddeeeeffff = 12-bit S_SEQ        (RLP 3) 
      
  
  Copyright (c) 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlpseq.h_v   1.4   28 Jan 2003 16:58:26   kdevara  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsrlpseq.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/05   ksu     EMPA: packet stream, octet sequencing (forward link also)
09/29/05   ksu     MFPA: short sequence numbers work for flow with naks disabled
01/28/03   kvd        Modified 4 dsrlpseq macros DSRLPSEQ_GET_NEXT_SEQ,
                      DSRLPSEQ_INCREMENT_SEQ, DSRLPSEQ_INCREMENT_S_SEQ,
                      DSRLPSEQ_INCREMENT_32BIT_SEQ to include modulus as
                      an argument
06/29/01   ak      Fixed prob in GEN_FROM_SEQ function.
05/24/01   hcg     Made changes to support HDR (1xEV).
09/29/00   ak/igt  Fixed DIFF_SEQ comparison error when both operands are
                   equal.
03/14/00   hcg     Fixed Lint errors.
11/16/99 ak/na/hcg Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#ifdef FEATURE_DS
#include "dsrlp_v.h"
#include "dsrlpi.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Since the 32-bit format does not use the top & bottom 4 bits, this value
  can safely represent an impossible sequence number, as all bits are
  non-zero.
---------------------------------------------------------------------------*/
#define DSRLPSEQ_NO_SEQ                0xFFFFFFFF

/*---------------------------------------------------------------------------
  This is the modulus used when representing the sequence numbers as 
  32-bit numbers.
---------------------------------------------------------------------------*/
#define DSRLPSEQ_32BIT_SEQ_MODULUS     0x10000000   /* 2 to the 28th power */



/*===========================================================================

                      EXTERNAL MACRO DECLARATIONS

===========================================================================*/


/*===========================================================================

MACRO DSRLPSEQ_GE_SEQ_CHECK

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than or Equal to the 2nd 
  argument (modulo the seq_modulus/2), where seq_modulus is the 3rd argument

PARAMETERS
  seq1, seq2: Sequence numbers (modulo RLP_SEQ_MODULUS) for comparison.
  seq_modulus: The seq_modulus for that version of RLP( 256 for RLP1, 
  4096 for RLP2)
  All the passed arguments must be 32bit words.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if seq_1 >= seq_2 modulo seq_modulus, else FALSE

SIDE EFFECTS
  None
===========================================================================*/

#define DSRLPSEQ_GE_SEQ_CHECK( seq1, seq2, seq_modulus) \
  ((((seq1) - (seq2)) & (seq_modulus >> 1)) == 0)


/*===========================================================================

MACRO DSRLPSEQ_GT_SEQ_CHECK

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than the 2nd argument (modulo 
  input modulus).

PARAMETERS
  arg1, arg2: Sequence numbers for comparison, and modulo to use.

DEPENDENCIES
  The passed arguments must be words

RETURN VALUE
  TRUE if arg_1 > arg_2 modulo seq_modulus, else FALSE

SIDE EFFECTS
  None
===========================================================================*/

#define DSRLPSEQ_GT_SEQ_CHECK( arg1, arg2, seq_modulus) \
  (((arg1) != (arg2)) && ((((arg1) - (arg2)) & (seq_modulus/2)) == 0))


/*===========================================================================

MACRO DSRLPSEQ_GET_SEQ

DESCRIPTION
  This macro will return the 8-bit sequence number from the 32-bit seq
  num value.  The 32-bit value has the following struct:
  
         MSB                                    LSB
          |                                      |
          0000  aaaa  bbbbcccc  ddddeeeeffff  0000
          
  where:
  
    0000         = fixed bit values
    aaaa         = 4-bit SEQ_HI value
    bbbbcccc     = 8-bit SEQ
    ddddeeeeffff = 12-bit S_SEQ

PARAMETERS
  the 32-bit seq number.

DEPENDENCIES
  None

RETURN VALUE
  the 8-bit sequence number

SIDE EFFECTS
  None
===========================================================================*/

#define DSRLPSEQ_GET_SEQ( seq ) \
  (byte)((((uint32)seq) >> 16) & 0x000000FF)


/*===========================================================================

MACRO DSRLPSEQ_GET_SEQ_HI

DESCRIPTION
  This macro will return the 4-bit SEQ_HI number from the 32-bit seq
  num value.  The 32-bit value has the following struct:
  
         MSB                                    LSB
          |                                      |
          0000  aaaa  bbbbcccc  ddddeeeeffff  0000
          
  where:
  
    0000         = fixed bit values
    aaaa         = 4-bit SEQ_HI value
    bbbbcccc     = 8-bit SEQ
    ddddeeeeffff = 12-bit S_SEQ

PARAMETERS
  the 32-bit seq number.

DEPENDENCIES
  None

RETURN VALUE
  the 4-bit seq hi

SIDE EFFECTS
  None
===========================================================================*/

#define DSRLPSEQ_GET_SEQ_HI( seq ) \
  (byte)((((uint32)seq) >> 24) & 0x0000000F)

/*===========================================================================

MACRO          DSRLPSEQ_GET_S_SEQ

DESCRIPTION    Gets the 12-bit S_SEQ from the internal 32-bit representation.

PARAMETERS     a 32-bit seq num.
                                           
DEPENDENCIES   NONE

RETURN VALUE   the 12-bit value, properly shifted into the lower two
               bytes.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_GET_S_SEQ(seq) \
     (uint16)((((uint32)seq) >> 4) & 0x00000FFF)

/*===========================================================================

MACRO          DSRLPSEQ_GET_12BIT_FRAME_SEQ

DESCRIPTION    Gets the 12-bit SEQ and SEQ_HI from the internal 32-bit
               representation.

PARAMETERS     a 32-bit seq num.
                                           
DEPENDENCIES   NONE

RETURN VALUE   the 12-bit value, properly shifted into the lower two
               bytes.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_GET_12BIT_FRAME_SEQ(seq) \
     (uint16)((((uint32)seq) >> 16) & 0x00000FFF)

/*===========================================================================

MACRO DSRLPSEQ_STRIP_S_SEQ

DESCRIPTION
  This macro will zero out the S_SEQ part of the input 32-bit number.
  
         MSB                                    LSB
          |                                      |
          0000  aaaa  bbbbcccc  ddddeeeeffff  0000
          
  where:
  
    0000         = fixed bit values
    aaaa         = 4-bit SEQ_HI value
    bbbbcccc     = 8-bit SEQ
    ddddeeeeffff = 12-bit S_SEQ

PARAMETERS
  the 32-bit seq number.

DEPENDENCIES
  None

RETURN VALUE
  the 32-bit seq number, with S_SEQ = 0

SIDE EFFECTS
  None
===========================================================================*/

#define DSRLPSEQ_STRIP_S_SEQ( seq_num ) \
  (((uint32)seq_num) & 0x0FFF0000)

/*===========================================================================

MACRO DSRLPSEQ_PUT_SEQ

DESCRIPTION
  This macro will take an 8-bit sequence number (SEQ) and store it in the 
  appropriate bit position location of the internal representation of 
  32-bit sequence numbers.  The 32-bit value has the following struct:

         MSB                                    LSB
          |                                      |
          0000  aaaa  bbbbcccc  ddddeeeeffff  0000          
  where:
  

    0000         = fixed bit values
    aaaa         = 4-bit SEQ_HI value
    bbbbcccc     = 8-bit SEQ
    ddddeeeeffff = 12-bit S_SEQ

PARAMETERS
  seq_32     - the 32-bit sequence number that is being generated.
  seq_8      - the 8-bit sequence number that is to be shifted in.

DEPENDENCIES
  None

RETURN VALUE
  the 32-bit sequence number with the 8-bit parameter bit-shifted in

SIDE EFFECTS
  None
===========================================================================*/
#define DSRLPSEQ_PUT_SEQ( seq_32, seq_8 ) \
  (seq_32 = (seq_32 & 0x0F00FFF0) | (uint32)(((uint32)seq_8) << 16))

/*===========================================================================

MACRO DSRLPSEQ_PUT_SEQ_HI

DESCRIPTION
  This macro will take a 4-bit sequence number (SEQ_HI) and store it in the 
  appropriate bit position location of the internal representation of 32-bit
  sequence numbers.  The 32-bit value has the following struct:

         MSB                                    LSB
          |                                      |
          0000  aaaa  bbbbcccc  ddddeeeeffff  0000          

  where:
  

    0000         = fixed bit values
    aaaa         = 4-bit SEQ_HI value
    bbbbcccc     = 8-bit SEQ
    ddddeeeeffff = 12-bit S_SEQ

PARAMETERS
  seq_32   - the 32-bit sequence number that is being generated.
  seq_4    - the 4-bit sequence number that is to be shifted in.

DEPENDENCIES
  None

RETURN VALUE
  the 32-bit sequence number with the 4-bit parameter bit-shifted in

SIDE EFFECTS
  None
===========================================================================*/
#define DSRLPSEQ_PUT_SEQ_HI( seq_32, seq_4 ) \
  (seq_32 = (seq_32 & 0x00FFFFF0) | (uint32)(((uint32)seq_4) << 24))

/*===========================================================================

MACRO DSRLPSEQ_PUT_S_SEQ

DESCRIPTION
  This macro will take a 12-bit sequence number (S_SEQ) and store it in the 
  appropriate bit position location of the internal representation of 32-bit
  sequence numbers.  The 32-bit value has the following struct:

         MSB                                    LSB
          |                                      |
          0000  aaaa  bbbbcccc  ddddeeeeffff  0000          

  where:
  

    0000         = fixed bit values
    aaaa         = 4-bit SEQ_HI value
    bbbbcccc     = 8-bit SEQ
    ddddeeeeffff = 12-bit S_SEQ

PARAMETERS
  seq_32   - the 32-bit sequence number that is being generated.
  seq_12   - the 12-bit sequence number that is to be shifted in.

DEPENDENCIES
  None

RETURN VALUE
  the 32-bit sequence number with the 12-bit parameter bit-shifted in

SIDE EFFECTS
  None
===========================================================================*/
#define DSRLPSEQ_PUT_S_SEQ( seq_32, seq_12 ) \
  (seq_32 = ((((uint32)seq_32) & 0x0FFF0000) | \
             (((uint32)seq_12) << 4)))

/*===========================================================================

MACRO          DSRLPSEQ_PUT_12BIT_FRAME_SEQ

DESCRIPTION    Puts the 12-bit SEQ and SEQ_HI into the internal 32-bit
               representation.

PARAMETERS     a 12-bit seq num.
                                           
DEPENDENCIES   NONE

RETURN VALUE   the 32-bit value, properly shifted into the upper byte
               positions for internal format.  Preserves any value that may
               be in the S_SEQ position.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_PUT_12BIT_FRAME_SEQ(seq) \
  (seq = ((((uint32)seq) << 16) & 0x0FFFFFF0))

/*===========================================================================

MACRO DSRLPSEQ_SEQ_GEN_FROM_SEQ_HI

DESCRIPTION
  This macro will generate the 12-bit L_SEQ sequence number, given the 4-bit
  SEQ_HI field, and the 8-bit SEQ field.  The resultant 12-bit sequence 
  number will be stored using the 32-bit internal representation.

PARAMETERS
  seq_32   - the 32-bit sequence number that is being generated.
  seq_hi   - the 4-bit SEQ_HI field.
  seq      - the 8-bit SEQ sequence number.

DEPENDENCIES
  None

RETURN VALUE
  the sequence number in 32-bit internal representation

SIDE EFFECTS
  None
===========================================================================*/
#define DSRLPSEQ_SEQ_GEN_FROM_SEQ_HI( seq_32, seq_hi, seq ) \
  DSRLPSEQ_PUT_SEQ_HI(seq_32, seq_hi);             \
  DSRLPSEQ_PUT_SEQ(seq_32, seq)                   

/*===========================================================================

MACRO DSRLPSEQ_GEN_FROM_SEQ

DESCRIPTION
  This macro will generate the 12-bit L_SEQ sequence number, given the 8-bit
  OTA SEQ field.  The resultant 12-bit sequence number will be stored using 
  the 32-bit internal representation.  The 12-bit L_SEQ is calculated as:

         L_SEQ = {L_V(R) + [256 + SEQ - V(R)] mod 256} mod 4096

PARAMETERS
  seq_32   - the 32-bit sequence number that is being generated.
  seq      - the 8-bit SEQ sequence number.
  state    - RLP3 state information structure
  ver_ptr  - pointer to RLP version specific information

DEPENDENCIES
  None

RETURN VALUE
  the sequence number in 32-bit internal representation

SIDE EFFECTS
  None
===========================================================================*/
#define DSRLPSEQ_GEN_FROM_SEQ( seq_32, seq , L_VR, seq_mod)                 \
{                                                                           \
  uint32 S_VR = L_VR & 0x0FFF0000;                                          \
  uint32 VR   = L_VR & 0x00FF0000;                                          \
  seq_32 = ((S_VR +(0x01000000 + (seq <<16) - VR) % 0x01000000) % seq_mod); \
}

/*===========================================================================

MACRO DSRLPSEQ_NTOHL

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.  This macro is used for converting OTA sequence numbers from
  network to host byte ordering.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define DSRLPSEQ_NTOHL(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))

/*===========================================================================

MACRO          DSRLPSEQ_INCREMENT_SEQ

DESCRIPTION    Increments the embedded 8bit value within the 32-bit 
               sequence number.  The AND makes sure that the leading
               4-bits and the trailing 4-bits maintain their 0000 
               state.  This also takes care of any roll-over that may
               occur.

PARAMETERS     The 32-bit sequence number.
                                           
DEPENDENCIES   None.

RETURN VALUE   The incremented value

SIDE EFFECTS   SEQ_HI may also be incremented.

===========================================================================*/

#define DSRLPSEQ_INCREMENT_SEQ(x,seq_mod)   \
        ((x + (uint32)0x10000) % (seq_mod))

/*===========================================================================

MACRO          DSRLPSEQ_INCREMENT_S_SEQ

DESCRIPTION    Increments the embedded 12-bit S_SEQ in the 32-bit
               sequence number.  The AND makes sure that the leading
               4-bits and the trailing 4-bits maintain their 0000 
               state.  This also takes care of any roll-over that may
               occur.

PARAMETERS     The 32-bit sequence number.
                                           
DEPENDENCIES   None.

RETURN VALUE   The incremented value

SIDE EFFECTS   NONE

===========================================================================*/

#define DSRLPSEQ_INCREMENT_S_SEQ(x,seq_mod)  \
        ((x + (uint32)0x10) % (seq_mod))

/*===========================================================================

MACRO          DSRLPSEQ_INCREMENT_32BIT_SEQ

DESCRIPTION    Increments the 32 bit value sequence number.  Increment
               is based on whether or not the 12-bit S_SEQ is zero/non-
               zero.
               
               By and'ing, any bit-rollover is eliminated.

PARAMETERS     Current 32-bit sequence number.
                                           
DEPENDENCIES   None.

RETURN VALUE   The incremented value

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_INCREMENT_32BIT_SEQ(x,seq_mod) \
   (DSRLPSEQ_HAS_S_SEQ(x)?DSRLPSEQ_INCREMENT_S_SEQ(x,seq_mod):DSRLPSEQ_INCREMENT_SEQ(x,seq_mod))


/*===========================================================================

MACRO          DSRLPSEQ_HAS_S_SEQ

DESCRIPTION    Returns non-zero if the input 32-bit sequence number has
               a non-zero 12-bit S_SEQ.

PARAMETERS     The 32-bit sequence number
                                           
DEPENDENCIES   None.

RETURN VALUE   Non-zero if S_SEQ is non-zero

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_HAS_S_SEQ(x) \
    (x & 0x0000FFF0)

/*===========================================================================

MACRO          DSRLPSEQ_HAS_SAME_SEQ

DESCRIPTION    Tells us if the two arguments have the same 4-bit and 8-bit
               SEQ numbers.

PARAMETERS     2 32-bit sequence numbers
                                           
DEPENDENCIES   None.

RETURN VALUE   Non-zero if same 

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_HAS_SAME_SEQ(x,y) \
   ((x & 0x0FFF0000) == (y & 0x0FFF0000))


/*===========================================================================

MACRO          DSRLPSEQ_GET_NEXT_SEQ

DESCRIPTION    Given a 32-bit number (which may or may not have 12-bit
               S_SEQ), will get the next 32-bit sequence number which 
               has a zero 12-bit S_SEQ.  The 8-bit S_SEQ is what is 
               incremented.  Example (32-bit hex value is spaced out):
               
               0x0 4 09 345 0  => 0x0 4 0A 000 0

PARAMETERS     a 32-bit sequence numbers
                                           
DEPENDENCIES   None.

RETURN VALUE   Incremented 32-bit seq.

SIDE EFFECTS   None.

===========================================================================*/

#define DSRLPSEQ_GET_NEXT_SEQ(x,seq_mod) \
     (((x + (uint32)0x00010000)%(seq_mod)) & 0x0FFF0000)


/*===========================================================================

MACRO          DSRLPSEQ_ADD_S_SEQ

DESCRIPTION    Adds the given length to the input 32-bit seq number, with
               a given modulus.               

PARAMETERS     a 32-bit seq num and a word.
                                           
DEPENDENCIES   Assumed that length is less than 0x0FFF.

RETURN VALUE   the new value.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_ADD_S_SEQ(seq, len, seq_mod) \
      ( ( seq + ( ( (uint32)len ) << 4) ) % (seq_mod) )


/*===========================================================================

MACRO          DSRLPSEQ_ADD_SEQ

DESCRIPTION    Adds the given value to the SEQ portion of the seq number.

PARAMETERS     a 32-bit seq num and a word.
                                           
DEPENDENCIES   Assumed that length is less than 0x0FFF.

RETURN VALUE   the new value.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_ADD_SEQ(seq,addulo, seq_mod) \
   ((seq + (((uint32)(addulo)) << 16)) % (seq_mod))


/*===========================================================================

MACRO          DSRLPRSEQ_DIFF_SEQ

DESCRIPTION    Finds the difference of 2 seq numbers with a modulus
      

PARAMETERS     two 32-bit seq num and the current sequence modulus
                                           
DEPENDENCIES   Assumes that we know for a fact that the first seq # is
               greater than the second one (including modulus logic)

RETURN VALUE   the absolute difference of the 2 seq #s

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPSEQ_DIFF_SEQ(big_seq, small_seq, seq_mod)\
((big_seq >= small_seq) ?\
 ( ((uint32)big_seq - (uint32)small_seq) >> 4 ) :\
 ((((uint32)seq_mod - (uint32)small_seq) + (uint32)big_seq ) >> 4 )\
)

#endif /* FEATURE_DS */
#endif /* _DSRLPSEQ_H */
