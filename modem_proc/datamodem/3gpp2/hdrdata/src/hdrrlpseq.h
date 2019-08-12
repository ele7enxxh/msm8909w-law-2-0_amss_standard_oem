#ifndef HDRRLPSEQ_H
#define HDRRLPSEQ_H
/*===========================================================================

                  H D R  R A D I O   L I N K   P R O T O C O L
              S E Q U E N C E  N U M B E R  H E A D E R  F I L E

DESCRIPTION
  This header file contains various #defines and macros used to manipulate
  the internal 32-bit sequence number.  The sequence number format is 
  designed to be used by HDR RLP.
  
  Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlpseq.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------
06/26/07   vas   Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Since the 32-bit format does not use the top 2 bits, this value
  can safely represent an impossible sequence number, as all bits are
  non-zero.
---------------------------------------------------------------------------*/
#define HDRRLPSEQ_NO_SEQ                0xFFFFFFFF

/*===========================================================================

MACRO HDRRLPSEQ_GE_SEQ_CHECK

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than or Equal to the 2nd 
  argument (modulo the seq_modulus/2), where seq_modulus is the 3rd argument

  This macro is the same as DSRLPSEQ_GE_SEQ_CHECK

PARAMETERS
  seq1, seq2: Sequence numbers (modulo RLP_SEQ_MODULUS) for comparison.
  seq_modulus: The seq_modulus for that version of RLP
  All the passed arguments must be 32bit words.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if seq_1 >= seq_2 modulo seq_modulus, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define HDRRLPSEQ_GE_SEQ_CHECK( seq1, seq2, seq_modulus) \
  ((((seq1) - (seq2)) & (seq_modulus >> 1)) == 0)


/*===========================================================================

MACRO HDRRLPSEQ_GT_SEQ_CHECK

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than the 2nd argument (modulo 
  input modulus).

  This macro is the same as DSRLPSEQ_GT_SEQ_CHECK

PARAMETERS
  arg1, arg2: Sequence numbers for comparison, and modulo to use.

DEPENDENCIES
  The passed arguments must be words

RETURN VALUE
  TRUE if arg_1 > arg_2 modulo seq_modulus, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define HDRRLPSEQ_GT_SEQ_CHECK( arg1, arg2, seq_modulus) \
  (((arg1) != (arg2)) && ((((arg1) - (arg2)) & (seq_modulus/2)) == 0))

/******HDR Macros*******/
/*===========================================================================

MACRO HDRRLPSEQ_DIFF_SEQ

DESCRIPTION
  Finds the difference of 2 seq numbers with a modulus

PARAMETERS
  two 32-bit seq num and the current sequence modulus

DEPENDENCIES
  The passed arguments must be words
  Assumes that we know for a fact that the first seq # is
  greater than the second one (including modulus logic)

RETURN VALUE
  The absolute difference of the 2 seq #s

SIDE EFFECTS
  None
===========================================================================*/
#define HDRRLPSEQ_DIFF_SEQ(big_seq, small_seq, seq_mod)\
((big_seq >= small_seq) ?\
 ( ((uint32)big_seq - (uint32)small_seq)) :\
 ((((uint32)seq_mod - (uint32)small_seq) + (uint32)big_seq ))\
)

/*===========================================================================

MACRO HDRRLPSEQ_ADD_SEQ

DESCRIPTION
  Determines the sum of seq numbers with a modulus

PARAMETERS
  The two 32-bit seq num and the current sequence modulus

DEPENDENCIES
  None

RETURN VALUE
  The sum of the 2 seq #s

SIDE EFFECTS
  None
===========================================================================*/
#define HDRRLPSEQ_ADD_SEQ(seq, len, seq_mod) \
      ( ( seq + len ) % (seq_mod) )

/*===========================================================================

MACRO HDRRLPSEQ_GET_SEQ

DESCRIPTION
  Returns the seq number with a modulus

PARAMETERS
  The seq num and the sequence modulus

DEPENDENCIES
  None
  
RETURN VALUE
  The sequence number in modulo space

SIDE EFFECTS
  None
===========================================================================*/
#define HDRRLPSEQ_GET_SEQ(seq,seq_mod) \
     (((uint32)seq) % seq_mod)

/*===========================================================================

MACRO HDRRLPSEQ_STORE_SEQ

DESCRIPTION
  Computes the modulo sequence number and stores it into the out_sequnce number

PARAMETERS
  The input seq num and the sequence modulus & the output parameter to store the value 

DEPENDENCIES
  None
  
RETURN VALUE
  The sequence number in modulo space

SIDE EFFECTS
  None
===========================================================================*/
#define HDRRLPSEQ_STORE_SEQ( out_seq, in_seq, seq_mod ) \
  (out_seq = (((uint32)in_seq) % seq_mod))

#endif /* HDRRLPSEQ_H */
