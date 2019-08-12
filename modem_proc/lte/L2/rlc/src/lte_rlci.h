/*!
  @file
  lte_rlci.h

  @brief
  Top level RLC file including all data structures, enumerations etc. internal
  to RLC module.

  @author
  araina, axiao

*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlci.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/02/16   ru      CR992840: Fix reachable assert in overlapping PDU segments
05/27/14   mm      CR 670482: Changes for new QSH API
04/11/14   mg      CR647594: QSH Phase 1
04/13/09   ax      add overlapping segment support
02/09/09   ax      remove dynamic memory allocation macro, use L2 common macro
02/06/09   ax      remove function call within assert macros
01/20/09   ax      add malloc RLC macro
12/29/08   ax      add include<pthread.h> to make this header self-contained
12/29/08   ax      add LTE_RLCI_NTOHS() such that it can be shared with UL code
12/26/08   ar      Added LTE_RLCI_LOCK_MUTEX and LTE_RLCI_UNLOCK_MUTEX macros
04/12/08   ar      Added AM related macro LTE_RLCI_HDR_LSF_BIT_MASK
11/17/08   ax      use lte_rb_cfg_idx_t as a unique identifier across AS
11/07/08   ar      Added payload structure for LTE_RLCDL_CTRL_PDU_IND
11/04/08   ax      AM related macros
10/30/08   ar      Removed duplicate definition of LTE_RLCI_HDR_SN10_E_MASK
10/29/08   ar      Added some common UM/AM macros
10/21/08   ax      remove obsolete macro
08/19/2008 ax      initial version
==============================================================================*/

#ifndef LTE_RLCI_H
#define LTE_RLCI_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_assert.h>
#include <lte_as.h>
#include <msgr.h>
#include <dsm_item.h>
#include <pthread.h>
#include "qsh.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*! @brief modulus for fixed 10 bit seq length
*/
#define LTE_RLCI_SN10_MODULUS        1024

/*! @brief sequence mask for fixed 10 bit seq length
*/
#define LTE_RLCI_SN10_MASK           0x3ff

/*! @brief modulus for fixed 5 bit seq length
*/
#define LTE_RLCI_SN5_MODULUS           32

/*! @brief sequence mask for fixed 5 bit seq length
*/
#define LTE_RLCI_SN5_MASK             0x1f

/*! @brief half modulus for AM: 512
*/
#define LTE_RLCI_AM_HALF_MOD          512

/*! @brief half modulus for 10 bit seq length: 512
*/
#define LTE_RLCI_SN10_HALF_MOD        512

/*! @brief half modulus for 5 bit seq length: 512
*/
#define LTE_RLCI_SN5_HALF_MOD          16

/*! @brief AM sequence space mask, also the highest SN
*/
#define LTE_RLCI_AM_MOD_MASK       0x03FF

/*! @brief SO (SOstart, SOend) size in bits
*/
#define LTE_RLCI_CTRL_PDU_SO_SIZE                     15

/*! @brief 15 bits = D/C (1) + CPT (3) + ACK_SN(10) + E1 (1)
*/
#define LTE_RLCI_CTRL_PDU_MIN_SIZE                    15

/*! @brief LTE_RLCI_CTRL_PDU_MIN_SIZE in bytes
*/
#define LTE_RLCI_CTRL_PDU_MIN_BYTE                    2

/*! @brief additional 12 bits = NACK_SN (10) + E1 (1) + E2 (1) each NAK
*/
#define LTE_RLCI_CTRL_PDU_NAK_SIZE                    12

/*! @brief additional 42 bits each NAK segment
    42 bits = NACK_SN (10) + E1 (1) + E2 (1) + SOstart (15) + SOend(15)
*/
#define LTE_RLCI_CTRL_PDU_NAK_SEG_SIZE                42

/*! @brief size in bytes for 10 Bit SN PDU header fixed portion
*/
#define LTE_RLCI_FIXED_HDR_SIZE_SN10                   2

/*! @brief size in bytes for LSF & SO field
*/
#define LTE_RLCI_FIXED_HDR_SIZE_SO                     2

/*! @brief size in bytes for 5 Bit SN PDU header fixed portion
*/
#define LTE_RLCI_FIXED_HDR_SIZE_SN5                    1

/*! @brief sequence length of 10
*/
#define LTE_RLCI_SN_LEN_10                             10

/*! @brief sequence length of 5
*/
#define LTE_RLCI_SN_LEN_5                               5

/*! @brief sequence number mask in fixed header for sequence length=5 RB
*/
#define LTE_RLCI_HDR_SN5_SN_MASK                        0x1f

/*! @brief FI field mask in fixed header for sequence length=5 RB
*/
#define LTE_RLCI_HDR_SN5_FI_MASK                        0xC0

/*! @brief FI field position in fixed header for sequence length=5 RB
*/
#define LTE_RLCI_HDR_SN5_FI_POS                         6

/*! @brief Mask for the first bit of the Framing Info field of 10bit SN header.
 * This mask is used when the first sdu doesn not start in the pdu
*/
#define LTE_RLCI_SN10_SDU_START_NOT_ALIGNED             0x10

/*! @brief Mask for the second bit of the Framing Info field of 10bit SN header.
 * This mask is used when the last sdu doesnt end in the pdu
*/
#define LTE_RLCI_SN10_SDU_END_NOT_ALIGNED               0x08

/*! @brief Mask for the first bit of the Framing Info field of 5bit SN header.
 * This mask is used when the first sdu doesn not start in the pdu
*/
#define LTE_RLCI_SN5_SDU_START_NOT_ALIGNED              0x80

/*! @brief Mask for the second bit of the Framing Info field of 5bit SN header.
 * This mask is used when the last sdu doesnt end in the pdu
*/
#define LTE_RLCI_SN5_SDU_END_NOT_ALIGNED                0x40

/*! @brief E field mask in fixed header for sequence length=5 RB
*/
#define LTE_RLCI_HDR_SN5_E_MASK                         0x20

/*! @brief E field position in fixed header for sequence length=5 RB
*/
#define LTE_RLCI_HDR_SN5_E_POS                          5

/*! @brief sequence number mask in fixed header for sequence length=10 RB
*/
#define LTE_RLCI_HDR_SN10_SN_MASK                       0x03ff

/*! @brief FI field mask n fixed header for sequence length=10 RB
*/
#define LTE_RLCI_HDR_SN10_FI_MASK                       0x1800

/*! @brief FI field position in fixed header for sequence length=10 RB
*/
#define LTE_RLCI_HDR_SN10_FI_POS                        11

/*! @brief E field position in fixed header for sequence length=10 RB
*/
#define LTE_RLCI_HDR_SN10_E_POS                         10

/*! @brief Size in bytes required for adding a new ODD LI (E+LI+padding) to the
 * pdu header
*/
#define LTE_RLCI_BYTES_NEEDED_TO_ADD_ODD_LI             2

/*! @brief Size in bytes required for adding a new EVEN LI (E+LI+padding) to the
 * pdu header
*/
#define LTE_RLCI_BYTES_NEEDED_TO_ADD_EVEN_LI            1

/*! @brief Mask for the E bit in the previous Odd LI
*/
#define LTE_RLCI_HDR_ODD_LI_E_MASK       0x80

/*! @brief Mask for the E bit in the previous even LI
*/
#define LTE_RLCI_HDR_EVEN_LI_E_MASK      0x08

/*! @brief E field mask in fixed header for sequence length=10 RB
*/
#define LTE_RLCI_HDR_SN10_E_MASK         0x0400

/*! @brief Mask for the Re-fragmentation bit in the pdu header. This bit
 * signifies if the current pdu is an AMD PDU or and AMD PDU segment
*/
#define LTE_RLCI_HDR_RF_BIT_MASK         0x40

/*! @brief Mask for the Data/Control bit in the pdu header
*/
#define LTE_RLCI_HDR_DC_BIT_MASK         0x80

/*! @brief Mask for the E bit in the segment offset byte
*/
#define LTE_RLCI_HDR_SN10_SEG_OFF_E_MASK 0x10

/*! @brief Mask for the LSF(Last Segment Flag) bit in the AMD PDU segment header
*/
#define LTE_RLCI_HDR_LSF_BIT_MASK        0x80

/*! @brief D/C field interpretation
*/
#define LTE_RLCI_HDR_DC_CONTROL_PDU                             0

/*! @brief D/C field interpretation
*/
#define LTE_RLCI_HDR_DC_DATA_PDU                                1

/*! @brief RF field interpretation
*/
#define LTE_RLCI_HDR_RF_AMD_PDU                                 0

/*! @brief RF field interpretation
*/
#define LTE_RLCI_HDR_RF_AMD_PDU_SEGMENT                         1

/*! @brief LSF field interpretation
*/
#define LTE_RLCI_HDR_LSF_NOT_LAST                               0

/*! @brief LSF field interpretation
*/
#define LTE_RLCI_HDR_LSF_LAST                                   1

/*! @brief FI field interpretation
*/
#define LTE_RLCI_HDR_FI_NOT_FIRST                               1

/*! @brief FI field interpretation
*/
#define LTE_RLCI_HDR_FI_IS_FIRST                                0

/*! @brief FI field interpretation
*/
#define LTE_RLCI_HDR_FI_NOT_LAST                                1

/*! @brief FI field interpretation
*/
#define LTE_RLCI_HDR_FI_IS_LAST                                 0

/*! @brief Poll field interpretation
*/
#define LTE_RLCI_HDR_P_NOT_SET                                  0

/*! @brief Poll field interpretation
*/
#define LTE_RLCI_HDR_P_SET                                      1

/*! @brief CPT (Control PDU Type) field interpretation
*/
#define LTE_RLCI_HDR_CPT_STATUS                                 0

/*! @brief minimal segment offset starting from 0. Maybe changed to 1 later
*/
#define LTE_RLCI_SO_MIN                                         0

/*! @brief special value of SO end signifying the last byte of the AMD PDU
*/
#define LTE_RLCI_SO_END_LAST_BYTE                          0x7fff
/*! @brief special value of end signifying the last byte of the AMD PDU
*/
#define LTE_RLCI_END_LAST_BYTE                             0xffff

/*! @brief Whether to print the RLC_QSH_ULOGS or not
*/
#define LTE_RLCI_PRINT_QSH                                      0


/*===========================================================================
**                  MACROS
**=========================================================================*/
/*! @brief increases the passed in sequence number by 1 bound by the mask
*/
#define LTE_RLCI_SEQ_INC(seq,mask) (((seq)+1) & (mask))

/*! @brief increases the passed in sequence number by 1 bound by the am mask
*/
#define LTE_RLCI_AM_SEQ_INC(seq) (((seq)+1) & LTE_RLCI_AM_MOD_MASK)

/*! @brief decrement the passed in sequence number by 1 bound by the mask
*/
#define LTE_RLCI_SEQ_DEC(seq,mask) (((seq)+(mask)) & (mask))

/*! @brief decrement the passed in sequence number by 1 bound by the am mask
*/
#define LTE_RLCI_AM_SEQ_DEC(seq) \
        (((seq)+LTE_RLCI_AM_MOD_MASK) & LTE_RLCI_AM_MOD_MASK)

/*! @brief return TRUE if the 1st argument is Greater than or equal to
     the 2nd argument (modulo input seq_modulus).
     DO NOT USE THIS MACRO IN LTE. USE OTHER MODE SPEC COMPARISON INSTEAD
*/
#define LTE_RLCI_SEQ_GE( seq1, seq2, seq_modulus) \
  ((((seq1) - (seq2)) & ((seq_modulus) >> 1)) == 0)

/*! @brief this is LTE specific UM sequence number comparison macro, it will
    replace legacy implementation as seen in LTE_RLCI_SEQ_GE
    It returns TRUE if the 1st argument is Greater than or equal to
    the 2nd argument for UM
*/
#define LTE_RLCI_UM_SEQ_GE( seq1, seq2, vr_h, modulus, mask)       \
(                                                                  \
  (((modulus) + (seq1) + ((modulus) >> 1) - (vr_h) ) & (mask)) >=  \
  (((modulus) + (seq2) + ((modulus) >> 1) - (vr_h) ) & (mask))     \
)

/*! @brief this is LTE specific UM sequence number comparison macro, it will
    replace legacy implementation as seen in LTE_RLCI_SEQ_GT
    It returns TRUE if the 1st argument is Greater than
    the 2nd argument for UM.
*/
#define LTE_RLCI_UM_SEQ_GT( seq1, seq2, vr_h, modulus, mask)       \
(                                                                  \
  (((modulus) + (seq1) + ((modulus) >> 1) - (vr_h) ) & (mask)) >   \
  (((modulus) + (seq2) + ((modulus) >> 1) - (vr_h) ) & (mask))     \
)

/*! @brief this is LTE specific AM sequence number comparison macro, it will
    replace legacy implementation as seen in LTE_RLCI_SEQ_GE
    It returns TRUE if the 1st argument is Greater than or equal to
    the 2nd argument for AM
*/
#define LTE_RLCI_AM_SEQ_GE( seq1, seq2, vr_r)                          \
(                                                                      \
  ((LTE_RLCI_SN10_MODULUS + (seq1) - (vr_r)) & LTE_RLCI_SN10_MASK) >=  \
  ((LTE_RLCI_SN10_MODULUS + (seq2) - (vr_r)) & LTE_RLCI_SN10_MASK)     \
)

/*! @brief this is LTE specific AM sequence number comparison macro, it will
    replace legacy implementation as seen in LTE_RLCI_SEQ_GT
    It returns TRUE if the 1st argument is Greater than
    the 2nd argument for AM.
*/
#define LTE_RLCI_AM_SEQ_GT( seq1, seq2, vr_r)                          \
(                                                                      \
  ((LTE_RLCI_SN10_MODULUS + (seq1) - (vr_r)) & LTE_RLCI_SN10_MASK) >   \
  ((LTE_RLCI_SN10_MODULUS + (seq2) - (vr_r)) & LTE_RLCI_SN10_MASK)     \
)

/*! @brief This macro finds the difference between two sequence numbers.
*/
#define LTE_RLCI_SEQ_DIFF(seq1, seq2, seq_modulus) \
(((seq1) >= (seq2)) ?\
 ((seq1) - (seq2)) :\
 (((seq_modulus) - (seq2)) + (seq1)))

/*! @brief This macro locks a mutex
*/
#define LTE_RLCI_LOCK_MUTEX(mutex)                 pthread_mutex_lock(mutex)

/*! @brief This macro unlocks a mutex
*/
#define LTE_RLCI_UNLOCK_MUTEX(mutex)             pthread_mutex_unlock(mutex)

/*! @brief generate start offset name
*/
#define LTE_RLCI_FIELD_POS( fname ) fname ## _POS

/*! @brief Generate field mask
*/
#define LTE_RLCI_FIELD_MASK( fname ) fname ## _MASK

/*! @brief LTE_RLC_ASSERT
*/
#define LTE_RLC_ASSERT(thing)                       LTE_ASSERT(thing)

/*! @brief convert network byte order byte array into host byte order uint16
*/
#define LTE_RLCI_NTOHS(byte0, byte1)                                     \
(                                                                        \
  ((byte0) << 8) | (byte1)                                               \
)

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================
  
  FUNCTION:  lte_rlculi_qsh_proc

==============================================================================*/
/*!
    @brief
    go through all the active RB's execute the QSH routine.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_rlculi_qsh_proc(qsh_action_e action, uint32 category_mask);




#endif /* LTE_RLCI_H */
