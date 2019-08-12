/*!
  @file
  rflm_ccs.h

  @brief
  RFC (CCS) driver.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary

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

===========================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dime_ccs/inc/rflm_ccs.h#1 $
$Date: 2016/12/13 $
when         who     what, where, why
--------   ---     -----------------------------------------------------------
11/25/13   Saul    CCS. New CSS Data Buffer Allocation For All Techs.
09/04/13   ra      Support off-target builds
09/02/13   ra      Use MCS HWIO
08/02/13   ra      Initial port to Bolt RF SW
==============================================================================*/

#ifndef RFLM_CCS_H
#define RFLM_CCS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "rflm_ccs_intf.h"
#include "rflm.h"
//#include "msm.h"
#include "rflm_hwintf.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* 
*/
#define RFLM_CCS_DMAC_WAITON(n)  HWIO_FVAL(MSS_RFC_DMACnCTL, WAITON, n)

/*! @brief The size of the RFLM_CCS PDMEM in bytes/words. */
#define RFLM_CCS_PDMEM_SIZE_BYTES  (HWIO_PHYS(MSS_PDMEM_MAX) - HWIO_PHYS(MSS_PDMEM_MIN) + 4)
#define RFLM_CCS_PDMEM_SIZE_WORDS  (RFLM_CCS_PDMEM_SIZE_BYTES/4)


/*! @brief Given an address inside PDMEM in DSP AHB space, convert to the same
    address in RFLM_CCS local PDMEM addressing (which uses only the LSb).
    This function is NOT suitable for anything other than PDMEM range - use
    RFLM_CCS_PDMEM_REG_ADDR() for registers.
*/
#define RFLM_CCS_PDMEM_LOCAL_ADDR(addr) ((uint32) (addr) & 0xFFFF)

/*! @brief Given an address of an AHB register in DSP AHB space, convert to the 
    same address in RFLM_CCS local addressing.
    Only valid within the RFLM_CCS local peripheral range.
*/
#define RFLM_CCS_REG_LOCAL_ADDR(addr)   ((uint32) (addr) & 0xFFFF)


/*! @brief Generate a DSP-to-RFLM_CCS interrupt. 
*/
#define RFLM_CCS_GEN_INTERRUPT(id) \
   HWIO_OUT(MSS_RFC_IPC, HWIO_FVAL(MSS_RFC_IPC,Q6_TO_M3_EVT,(1<<(id))))
//#define RFLM_CCS_GEN_INTERRUPT(id) HWIO_MSS_RFC_IPC_OUT(id)

/*! @brief DMAC descriptor.
*/
typedef struct
{
  uint32 src;     /*!< Source byte address or inline data (MSS_RFC_DMACnSRC) */
  uint32 dst;     /*!< Destination byte address (MSS_RFC_DMACnDST) */
  uint32 lnk;     /*!< Link to next descriptor (MSS_RFC_DMACnLNK) */
  uint32 ctl;     /*!< Control fields (MSS_RFC_DMACnCTL) */
  /* Use HWIO_FVAL() macro to specify ctl fields. */

} rflm_ccs_dmac_descriptor_t;

/*! @brief Select a WAITON enum for DMAC ctl field.
    Note this is relative to bit 0, so use with HWIO_FVAL().
*/
// ruben remove temporarily and move to rflm_ccs_rf_seq.cc #define RFLM_CCS_DMAC_WAITON(n)  (HWIO_MSS_RFC_DMACnCTL_WAITON_##n##_FVAL)
// will switch back support here when mcs_hwio_modem.h supports this.

/*! @brief Alias for no wait in DMA ctl.
*/
#define HWIO_MSS_RFC_DMACnCTL_WAITON_NO_WAIT_FVAL  HWIO_MSS_RFC_DMACnCTL_WAITON_NOWAIT0_FVAL



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

extern void rflm_ccs_init(void);
extern rflm_ccs_intf_t *rflm_ccs_get_interface(void);
extern void rflm_ccs_enable(void);
extern void rflm_ccs_disable(void);
extern void rflm_ccs_download_image(void);
extern void rflm_ccs_run(void);
extern void rflm_ccs_wait_ready(void);
extern void rflm_ccs_halt(void);
extern void rflm_ccs_dump(void);

extern uint32 * 
rflm_ccs_get_data_buf_item_ptr
( 
   rflm_ccs_data_buf_item_type data_buf_item
);



/*===========================================================================

  FUNCTION:  rflm_ccs_read_uint16

===========================================================================*/
/*!
    @brief
    Read a single uint16 from RFLM_CCS memory.

    @detail
    Read and return a 16-bit value from RFLM_CCS memory, by performing a 32-bit
    read and masking off the desired byte (RFLM_CCS memory only supports 32-bit access).
    rflm_ccs_src_ptr is a byte address within RFLM_CCS PDMEM, and must be 16-bit aligned.

    @return
    uint16 at address rflm_ccs_src_ptr.
*/
/*=========================================================================*/
INLINE uint16 rflm_ccs_read_uint16
(
  volatile void *rflm_ccs_src_ptr   /*!< Address to read */
)
{
  volatile uint32 *src = (volatile uint32 *) ((uint32) rflm_ccs_src_ptr & ~0x2);
  uint32 ofs = ((uint32) rflm_ccs_src_ptr & 0x2) * 8;
  uint32 val = *src;
  val = fextractuw(val, 16, ofs);
  return val;

} /* rflm_ccs_read_uint16() */


/*===========================================================================

  FUNCTION:  rflm_ccs_write_uint16

===========================================================================*/
/*!
    @brief
    Write a single uint16 to RFLM_CCS memory.

    @detail
    Write the given 16-bit word to RFLM_CCS memory by doing a read-modify-write
    on a 32-bit word (RFLM_CCS memory only supports 32-bit access).
    rfc_dest_ptr is a byte address within RFLM_CCS PDMEM, and must be 16-bit aligned.

    NOTE: Due to the read-modify-write, the remainder of the 32-bit word must
    not be modified by RFLM_CCS, or its value may be lost.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_write_uint16
(
  volatile void *rfc_dest_ptr,  /*!< Address to write */
  uint16 value                  /*!< Value to write */
)
{
  volatile uint32 *dest = (volatile uint32 *) ((uint32) rfc_dest_ptr & ~0x2);
  uint32 ofs = ((uint32) rfc_dest_ptr & 0x2) * 8;
  uint32 d = *dest;
  d = finsertuw(d, value, 16, ofs);
  *dest = d;

} /* rflm_ccs_write_uint16() */


/*===========================================================================

  FUNCTION:  rflm_ccs_read_uint8

===========================================================================*/
/*!
    @brief
    Read a single uint8 from RFLM_CCS memory.

    @detail
    Read and return an 8-bit value from RFLM_CCS memory, by performing a 32-bit
    read and masking off the desired byte (RFLM_CCS memory only supports 32-bit access).
    rflm_ccs_src_ptr is a byte address within RFLM_CCS PDMEM.

    @return
    Byte at address rflm_ccs_ src_ptr.
*/
/*=========================================================================*/
INLINE uint8 rflm_ccs_read_uint8
(
  volatile void *rflm_ccs_src_ptr /*!< Address to read */
)
{
  volatile uint32 *src = (volatile uint32 *) ((uint32) rflm_ccs_src_ptr & ~0x3);
  uint32 ofs = ((uint32) rflm_ccs_src_ptr & 0x3) * 8;
  uint32 val = *src;
  val = fextractuw(val, 8, ofs);
  return val;

} /* rflm_ccs_read_uint8() */


/*===========================================================================

  FUNCTION:  rflm_ccs_write_uint8

===========================================================================*/
/*!
    @brief
    Write a single uint8 to RFLM_CCS memory.

    @detail
    Write the given 8-bit word to RFLM_CCS memory by doing a read-modify-write
    on a 32-bit word (RFLM_CCS memory only supports 32-bit access).
    rfc_dest_ptr is a byte address within RFLM_CCS PDMEM.

    NOTE: Due to the read-modify-write, the remainder of the 32-bit word must
    not be modified by RFLM_CCS, or its value may be lost.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_write_uint8
(
  volatile void *rfc_dest_ptr, /*!< Address to write */
  uint8 value                  /*!< Value to write */
)
{
  volatile uint32 *dest = (volatile uint32 *) ((uint32) rfc_dest_ptr & ~0x3);
  uint32 ofs = ((uint32) rfc_dest_ptr & 0x3) * 8;
  uint32 d = *dest;
  d = finsertuw(d, value, 8, ofs);
  *dest = d;

} /* rflm_ccs_write_uint8() */



/*===========================================================================

  FUNCTION:  rflm_ccs_read_memory

===========================================================================*/
/*!
    @brief
    Read from RFLM_CCS memory.

    @detail
    Copy from RFLM_CCS internal memory to the destination buffer.
    Source and destination address must be 32-bit aligned, consistent with
    RFLM_CCS access size.
    The size is in bytes, but must be a multiple of 4.
    RFLM_CCS memory is assumed to be uncached.
    Violations *may* generate a fatal error.

    The copy will be done in the most efficient method available, using inlining
    or burst transactions if possible.

    See the following functions/macros:
    rflm_ccs_read_memory_small(): 
      The "small" function is compact and has adequate performance for 
      short transfers.
    rflm_ccs_read_memory_fast():
      The "fast" function groups reads into 64-bit words for better AHB
      throughput, but has higher overhead and is larger.
   
    If the length is known at compile time, the appropriate function is selected
    based on an assumed tradeoff between size/space. 
    If desired, the rflm_ccs_read_memory_small/fast functions can be called 
    directly.   

    @return
    None
*/
/*=========================================================================*/
#define rflm_ccs_read_memory(dest_ptr, rflm_ccs_src_ptr, num_bytes) \
  do { \
    if (__builtin_constant_p(num_bytes)) { \
      /*lint --e{506} */ \
      /* If length is known at compile time, select the function based on size. */ \
      if ((num_bytes) <= 16) { rflm_ccs_read_memory_small(dest_ptr,rflm_ccs_src_ptr,num_bytes); } \
      else { rflm_ccs_read_memory_fast(dest_ptr,rflm_ccs_src_ptr,num_bytes); } \
    } else { \
      /* If length is not known at compile time, use fast function. */ \
      /* Assumed to be a rare case, so the size overhead is acceptable. */ \
      rflm_ccs_read_memory_fast(dest_ptr,rflm_ccs_src_ptr,num_bytes); \
    } \
  } while (0)



/*===========================================================================

  FUNCTION:  rflm_ccs_read_memory_small

===========================================================================*/
/*!
    @brief
    Read from RFLM_CCS memory. Optimized for space.

    @detail
    See rflm_ccs_read_memory, which would normally be used instead.

    This function favors code size over throughput, and is preferable
    for short (e.g. <= 16 byte) transfers.
    If num_bytes is known at compile time, the compiler will unroll this
    into individual writes, and optimize with surrounding code, so it should 
    be as efficient as a direct memory access.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_read_memory_small
(
  void *dest_ptr,             /*!< Destination buffer, in DSP memory */
  volatile void *ahb_src_ptr, /*!< Source buffer, in RFLM_CCS memory */
  uint32 num_bytes            /*!< Size of buffer to copy, in bytes */
)
{

  /* We use void * as arguments to this function for type convenience, but
     they're really uint32 aligned. 
     Ensure that the source buffer is unioned with uint32 to avoid strict-aliasing
     warnings/bugs. */
  uint32 *dest = (uint32 *) dest_ptr;
  volatile uint32 *src = (volatile uint32 *) ahb_src_ptr;
  uint32 len = num_bytes / sizeof(uint32); 
  uint32 i;

  /* Copy word-by-word. */
  for (i=0; i < len; ++i)
  {
    *dest++ = *src++;
  }

} /* rflm_ccs_read_memory_small() */



/*===========================================================================

  FUNCTION:  rflm_ccs_read_memory_fast

===========================================================================*/
/*!
    @brief
    Read from RFLM_CCS memory. Optimized for speed.

    @detail
    See rflm_ccs_read_memory, which would normally be used instead.

    This function favors throughput over code size, and is preferable
    for long (e.g. > 16 byte) transfers.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_read_memory_fast
(
  void *dest_ptr,             /*!< Destination buffer, in DSP memory */
  volatile void *ahb_src_ptr, /*!< Source buffer, in RFLM_CCS memory */
  uint32 num_bytes            /*!< Size of buffer to copy, in bytes */
)
{

  /* We use void * as arguments to this function for type convenience, but
     they're really uint32 aligned. 
     Ensure that the source buffer is unioned with uint32 to avoid strict-aliasing
     warnings/bugs. */
  uint32 *dest = (uint32 *) dest_ptr;
  volatile uint32 *src32 = (volatile uint32 *) ahb_src_ptr;
  uint32 len32 = num_bytes / sizeof(uint32); 
  volatile uint64 *src64;
  uint32 len128;  
  uint32 i;

  /* If source is not 64-bit aligned, copy the first 32-bit word. */
  if ((num_bytes >= 4) && ((uint32) src32 & 0x4)) {
    *dest++ = *src32++;
    --len32;
  }

  /* Read pairs of 64-bit words. Both will be posted to BIU in parallel, and
     they will be split into 32-bit access on the BIU-to-AHB interface, so 
     the latency will overlap. 
     Write data as 32-bit words to avoid complicated realignment of dest ptr. */
  src64 = (volatile uint64 *) (volatile void *) src32;
  len128 = len32 / 4;  
  for (i=0; i < len128; ++i) {
    vect64 val0,val1;
    val0.d = src64[0];
    val1.d = src64[1];
    src64 += 2;
    dest[0] = val0.w[0];
    dest[1] = val0.w[1];
    dest[2] = val1.w[0];
    dest[3] = val1.w[1];
    dest += 4;
  }

  /* Read remaining 64-bit and 32-bit words, if any. */
  if (len32 & BIT(1)) {
    vect64 val;
    val.d = *src64++;
    dest[0] = val.w[0];
    dest[1] = val.w[1];
    dest += 2;
  }
  src32 = (volatile uint32 *) src64;
  if (len32 & BIT(0)) {
    *dest++ = *src32++;
  }

} /* rflm_ccs_read_memory_fast() */


/*===========================================================================

  FUNCTION:  rflm_ccs_write_memory

===========================================================================*/
/*!
    @brief
    Write to RFLM_CCS memory.

    @detail
    Copy the source buffer to RFLM_CCS internal memory.
    Source and destination address must be 32-bit aligned, consistent with
    RFLM_CCS access size.
    The size is in bytes, but must be a multiple of 4.
    RFLM_CCS memory is assumed to be uncached.
    Violations *may* generate a fatal error.

    The copy will be done in the most efficient method available, using 
    inlining or burst transactions if possible.
    See the following functions:
    rflm_ccs_write_memory_small(): 
      The "small" function is compact and has adequate performance for 
      short transfers.
    rflm_ccs_write_memory_fast():
      The "fast" function groups writes into 64-bit words for better AHB
      throughput, but has higher overhead and is larger.
   
    If the length is known at compile time, the appropriate function is selected
    based on an assumed tradeoff between size/space. 
    If desired, the rflm_ccs_write_memory_small/fast functions can be called 
    directly.   

    @return
    None
*/
/*=========================================================================*/
#define rflm_ccs_write_memory(rfc_dest_ptr, src_ptr, num_bytes) \
  do { \
    if (__builtin_constant_p(num_bytes)) { \
      /*lint --e{506} */ \
      /* If length is known at compile time, select the function based on size. */ \
      if ((num_bytes) <= 64) { rflm_ccs_write_memory_small(rfc_dest_ptr,src_ptr,num_bytes); } \
      else { rflm_ccs_write_memory_fast(rfc_dest_ptr,src_ptr,num_bytes); } \
    } else { \
      /* If length is not known at compile time, use fast function. */ \
      /* Assumed to be a rare case, so the size overhead is acceptable. */ \
      rflm_ccs_write_memory_fast(rfc_dest_ptr,src_ptr,num_bytes); \
    } \
  } while (0)



/*===========================================================================

  FUNCTION:  rflm_ccs_write_memory_small

===========================================================================*/
/*!
    @brief
    Write to RFLM_CCS memory. Optimized for space.

    @detail
    See rflm_ccs_write_memory, which would normally be used instead.
    This function favors code size over throughput, and is preferable
    for short (e.g. <= 64 byte) transfers.
    If num_bytes is known at compile time, the compiler will unroll this
    into individual writes, and optimize with surrounding code, so it should 
    be as efficient as a direct memory access.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_write_memory_small
(
  volatile void *ahb_dest_ptr,   /*!< Destination buffer, in RFLM_CCS memory */
  const void *src_ptr,           /*!< Source buffer, in DSP memory */
  uint32 num_bytes               /*!< Size of buffer to copy, in bytes */
)
{

  /* We use void * as arguments to this function for type convenience, but
     they're really uint32 aligned. 
     Ensure that the buffers are unioned with uint32 to avoid strict-aliasing
     warnings/bugs. */
  volatile uint32 *dest32 = (volatile uint32 *) ahb_dest_ptr;
  uint32 *src = (uint32 *) src_ptr;
  uint32 len32 = num_bytes / sizeof(uint32); 
  uint32 i;

  /* Copy word-by-word */
  for (i=0; i < len32; ++i)
  {
    *dest32++ = *src++;
  }

} /* rflm_ccs_write_memory_small() */


/*===========================================================================

  FUNCTION:  rflm_ccs_write_memory_fast

===========================================================================*/
/*!
    @brief
    Write to RFLM_CCS memory. Optimized for speed.

    @detail
    See rflm_ccs_write_memory, which would normally be used instead.
    This function favors throughput over code size, and is preferable
    for long (e.g. > 64 byte) transfers.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_write_memory_fast
(
  volatile void *ahb_dest_ptr,   /*!< Destination buffer, in RFLM_CCS memory */
  const void *src_ptr,           /*!< Source buffer, in DSP memory */
  uint32 num_bytes               /*!< Size of buffer to copy, in bytes */
)
{

  /* We use void * as arguments to this function for type convenience, but
     they're really uint32 aligned. 
     Ensure that the buffers are unioned with uint32 to avoid strict-aliasing
     warnings/bugs. */
  volatile uint32 *dest32 = (volatile uint32 *) ahb_dest_ptr;
  uint32 *src = (uint32 *) src_ptr;
  uint32 len32 = num_bytes / sizeof(uint32); 
  volatile uint64 *dest64;
  uint32 len64;
  uint32 i;

  /* If destination is not 64-bit aligned, copy the first 32-bit word. */
  if ((num_bytes >= 4) && ((uint32) dest32 & BIT(2))) {
    *dest32++ = *src++;
    --len32;
  }
  
  /* Write 64-bit words. They will be split into 32-bit access on the 
     BIU-to-AHB interface, which avoids extra arbitration overhead. 
     We expect to be AHB throughput limited, so read source data as
     32-bit words - that saves more complicated realignment of source ptr. */
  dest64 = (volatile uint64 *) (volatile void *) dest32;
  len64 = len32 / 2;
  for (i=0; i < len64; ++i) {
    vect64 val;
    val.w[0] = src[0];
    val.w[1] = src[1];
    src += 2;
    *dest64++ = val.d;
  }
  
  /* Write remaining 32-bit word, if any. */
  dest32 = (volatile uint32 *) dest64;
  if (len32 & BIT(0)) {
    *dest32++ = *src++;
  }

} /* rflm_ccs_write_memory_fast() */




/*===========================================================================

  FUNCTION:  rflm_ccs_set_memory

===========================================================================*/
/*!
    @brief
    Set RFLM_CCS memory.

    @detail
    Write the given constant to all locations of the destination buffer in
    RFLM_CCS internal memory.
    Destination address must be 32-bit aligned, consistent with
    RFLM_CCS access size.
    The size is in bytes, but must be a multiple of 4.
    RFLM_CCS memory is assumed to be uncached.
    Violations *may* generate a fatal error.

    The writes will be done in the most efficient method available, using 
    inlining or burst transactions if possible.
    See the following functions:
    rflm_ccs_set_memory_small(): 
      The "small" function is compact and has adequate performance for 
      short transfers.
    rflm_ccs_set_memory_fast():
      The "fast" function groups writes into 64-bit words for better AHB
      throughput, but has higher overhead and is larger.
   
    If the length is known at compile time, the appropriate function is selected
    based on an assumed tradeoff between size/space. 
    If desired, the rflm_ccs_write_memory_small/fast functions can be called 
    directly.  

    @return
    None
*/
/*=========================================================================*/
#define rflm_ccs_set_memory(rfc_dest_ptr, value, num_bytes) \
  do { \
    if (__builtin_constant_p(num_bytes)) { \
      /*lint --e{506} */ \
      /* If length is known at compile time, select the function based on size. */ \
      if ((num_bytes) <= 64) { rflm_ccs_set_memory_small(rfc_dest_ptr,value,num_bytes); } \
      else { rflm_ccs_set_memory_fast(rfc_dest_ptr,value,num_bytes); } \
    } else { \
      /* If length is not known at compile time, use fast function. */ \
      /* Assumed to be a rare case, so the size overhead is acceptable. */ \
      rflm_ccs_set_memory_fast(rfc_dest_ptr,value,num_bytes); \
    } \
  } while (0)



/*===========================================================================

  FUNCTION:  rflm_ccs_set_memory_small

===========================================================================*/
/*!
    @brief
    Set RFLM_CCS memory. Optimized for space.

    @detail
    See rflm_ccs_set_memory, which would normally be used instead.
    This function favors code size over throughput, and is preferable
    for short (e.g. <= 64 byte) transfers.
    If num_bytes is known at compile time, the compiler will unroll this
    into individual writes, and optimize with surrounding code, so it should 
    be as efficient as a direct memory access.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_set_memory_small
(
  volatile void *ahb_dest_ptr,   /*!< Destination buffer, in RFLM_CCS memory */
  uint32 value,                  /*!< Constant value to write */
  uint32 num_bytes               /*!< Size to write, in bytes */
)
{

  /* We use void * as arguments to this function for type convenience, but
     they're really uint32 aligned. 
     Ensure that the buffers are unioned with uint32 to avoid strict-aliasing
     warnings/bugs. */
  volatile uint32 *dest32 = (volatile uint32 *) ahb_dest_ptr;
  uint32 len32 = num_bytes / sizeof(uint32); 
  uint32 i;

  /* Write word-by-word */
  for (i=0; i < len32; ++i)
  {
    *dest32++ = value;
  }

} /* rflm_ccs_set_memory_small() */


/*===========================================================================

  FUNCTION:  rflm_ccs_set_memory_fast

===========================================================================*/
/*!
    @brief
    Set RFLM_CCS memory. Optimized for speed.

    @detail
    See rflm_ccs_set_memory, which would normally be used instead.
    This function favors throughput over code size, and is preferable
    for long (e.g. > 64 byte) transfers.

    @return
    None
*/
/*=========================================================================*/
INLINE void rflm_ccs_set_memory_fast
(
  volatile void *ahb_dest_ptr,   /*!< Destination buffer, in RFLM_CCS memory */
  uint32 value,                  /*!< Constant value to write */
  uint32 num_bytes               /*!< Size to write, in bytes */
)
{

  /* We use void * as arguments to this function for type convenience, but
     they're really uint32 aligned. 
     Ensure that the buffers are unioned with uint32 to avoid strict-aliasing
     warnings/bugs. */
  volatile uint32 *dest32 = (volatile uint32 *) ahb_dest_ptr;
  uint32 len32 = num_bytes / sizeof(uint32); 
  volatile uint64 *dest64;
  uint32 len64;
  vect64 value64;
  uint32 i;

  /* If destination is not 64-bit aligned, write the first 32-bit word. */
  if ((num_bytes >= 4) && ((uint32) dest32 & BIT(2))) {
    *dest32++ = value;
    --len32;
  }
  
  /* Write 64-bit words. They will be split into 32-bit access on the 
     BIU-to-AHB interface, which avoids extra arbitration overhead. */
  dest64 = (volatile uint64 *) (volatile void *) dest32;
  len64 = len32 / 2;
  value64.w[0] = value;
  value64.w[1] = value;
  for (i=0; i < len64; ++i) {
    *dest64++ = value64.d;
  }
  
  /* Write remaining 32-bit word, if any. */
  dest32 = (volatile uint32 *) dest64;
  if (len32 & BIT(0)) {
    *dest32++ = value;
  }

} /* rflm_ccs_set_memory_fast() */



#ifdef __cplusplus
}  // extern "C"
#endif



#endif /* RFLM_CCS_H */





