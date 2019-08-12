/*=============================================================================
                        Boot Utility File 

GENERAL DESCRIPTION     
  This module defines miscellaneous utility function for boot software.
  
Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_util.c#1 $

when       who             what, where, why
--------   ---         --------------------------------------------------
06/06/14   ck           Added booting_from_storage_media
10/25/12   dh           Add boot_count_set_bits
9/27/10    dh           add boot_strlen
8/30/05   John Alayari        Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_util.h"
#include BOOT_PBL_H
 
/*=========================================================================
                            
                            FUNCTION DEFINITIONS

=========================================================================*/


/*===========================================================================

**  Function :  boot_strlen

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion calculate the length of a string, excluding the NULL terminator.
*
* @param[in] str  start address of the string
*
* @par Dependencies
*   None
*   
* @retval
*   length of the string, excluding the NULL terminator.
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_strlen(char *str)
{
   char * ptr;

   for (ptr = str; *ptr; ptr++);
   
   return ptr - str;
}
/*boot_strlen*/


/*===========================================================================

**  Function : bload8

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function takes a byte pointer and it returns the byte value in 
*   in 32 bits. 
*   
*   This function ensures that read opeartions from memory are 32 bit 
*   aligned.
* 
* @par Dependencies
*   None
* 
* @param
*   p        - a Byte  Pointer.
*   
* @retval
*   Byte value in 32 bits.
* 
* @par Side Effects
*   None
*/
uint32 bLoad8 (const uint8* p)
{  
  /* align the address to 32 bits boundary */
  uint32 aligned_p = (uint32) p & (~0x00000003); 
  /* byte position in 32 bit address */  
  uint32 index = (uint32) p & 0x00000003;
  /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - */
  
  /* return the byte in 32 bits after masking other bytes with zero */  
 // return (( *((uint32*) aligned_p) & (0xFF << (index<<3))) >> (index<<3));

return (( *((uint32*) aligned_p) >> (index<<3)) &  0x000000FF ) ;


}
/*===========================================================================

**  Function : bstore8

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function takes 32 bits input value and stores it at the address
*   pointed by the input byte pointer.
*   
*   This function ensures that write opeation to memory are 32 bit aligned.
* 
* @par Dependencies
*   None
* 
* @param
*   p        - Byte pointer.
*   val      - 32 bit data to be stored in memory.
* @retval
*  None.
* 
* @par Side Effects
*   None
*/
void bStor8 (uint8* p, uint32 val)
{  
  /* align the address to 32 bits boundary */
  uint32 aligned_p = (uint32) p & (~0x00000003);
  /* byte position in 32 bit address */  
  uint32 index = (uint32) p & 0x00000003;
  /* read the current data at 32 bit aligned address */
  uint32 current_value = *((uint32*) aligned_p);
  /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - */
  
  /* ------------------------------------------------------------------ */
  /* store the input value after shifting it in the proper position and */
  /* ORing it with current_value after the target byte in 32 bit        */
  /* current_value is cleared by masking it with zero                   */
  /* ------------------------------------------------------------------ */
  *((uint32*) aligned_p) =  ((current_value & ~(0xFF << (index<<3)))) | 
                             (val<< (index <<3));
}
/*===========================================================================

**  Function : bbytecompare

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function compares two buffers byte by byte.
* 
*   This function replaces the memcmp utility in the C library and
*   ensures that read opeartions from memory are 32 bit aligned.
* 
*   Note: 
*   
*   Same optimization as bByteCopy could be applied here. but
*   this is not a high runner code in secure boot, optimization is 
*   left for future.
* 
* @par Dependencies
*   None
* 
* @param
*   value1_ptr  - Pointer to buffer 1.
*   value1_ptr  - Pointer to buffer 2.
*   byte_len    - Lenght to compare
* @retval
*  None.
* 
* @par Side Effects
*   None
*/
uint32 bByteCompare
(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint32  byte_len
)
{
  int   i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( i=0; i< (int) byte_len; i++ )
  {
    if ( bLoad8((uint8*) (value1_ptr++)) != 
         bLoad8((uint8*) (value2_ptr++)) 
       ) 
      return (uint32)(i+1);
  }

  return 0;
} /* bByteCompare */

/*===========================================================================

**  Function : qmemcpy

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function copies a block of memory, handling overlap.
*   This routine implements memcpy, and memmove. And ensures 
*   that read and write operations from/to memory are 32 bit 
*   aligned.
* 
* @par Dependencies
*   None.
* 
* @param
*   source_ptr           - Pointer to source buffer.
*   destination_ptr      - Pointer to destination buffer.
*   len                  - Length to copy.
* 
* @retval
*   None.
* 
* @par Side Effects
*   None.
*/
void qmemcpy( void* destination_ptr, const void* source_ptr, uint32 len )
{
  uint8       *dst_ptr = (uint8*) destination_ptr;
  const uint8 *src_ptr = (const uint8*) source_ptr;
  uint32      remainder;
  uint32      *src32_ptr;
  uint32      *dst32_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - */
  do
  {
    if (len == 0 || (uint32)dst_ptr == (uint32)src_ptr) 
        break;  /* nothing to do */

    if ((uint32)dst_ptr < (uint32)src_ptr) 
    {
        /* Copy forward */
        remainder = (uint32)src_ptr;   /* only need low bits */
        if ((remainder | (uint32)dst_ptr) & WORDMASK)
        {
            /* Try to align operands.  This cannot be done */
            /* unless the low bits match.                  */
            if ((remainder ^ (uint32)dst_ptr) & WORDMASK || len < WORDSIZE)
                remainder = len;
            else
               remainder = WORDSIZE - (remainder & WORDMASK);               
          
            len -= remainder;
            if (remainder)
                do
                   bStor8(dst_ptr++, bLoad8((uint8*)src_ptr++));
                while (--remainder);
        }
    
        /* Copy whole words. */
        
        /*  remainder = len div 4 */
        remainder = len >> 2;    
        if (remainder)
        {
           src32_ptr = (uint32*) src_ptr;
           dst32_ptr = (uint32*) dst_ptr;
           do 
              *dst32_ptr++ = *src32_ptr++;
           while (--remainder);
          
           /* update the byte pointers */
           src_ptr = (uint8*) src32_ptr;
           dst_ptr = (uint8*) dst32_ptr;
        }
 
        /* Then copy any trailing bytes. */
        remainder = len & WORDMASK;
        if (remainder)
            do 
               bStor8(dst_ptr++, bLoad8((uint8*)src_ptr++));
            while (--remainder);
    } 
    else 
    {
        /* Copy backwards.  Otherwise essentially the same. */
        /* Alignment works as before, except that it takes  */
        /* (remainder&WORDMASK) bytes to align, not         */ 
        /* WORDSIZE-(remainder&WORDMASK).                   */
        src_ptr += len;
        dst_ptr += len;
        remainder = (uint32)src_ptr;
        if ((remainder | (uint32)dst_ptr) & WORDMASK) 
        {
            if ((remainder ^ (uint32)dst_ptr) & WORDMASK || len <= WORDSIZE)
               remainder = len;
            else
               remainder &= WORDMASK;
            len -= remainder;
            if (remainder)
                do
                  bStor8(--dst_ptr, bLoad8((uint8*)--src_ptr));
                while (--remainder);
        }
        
        /* Copy whole words. */
        
        /*  remainder = len div 4 */
        remainder = len >> 2;    
        if (remainder)
        {
           src32_ptr = (uint32*) src_ptr;
           dst32_ptr = (uint32*) dst_ptr;
           do 
              *--dst32_ptr = *--src32_ptr;
           while (--remainder);
          
           /* update the byte pointers */
           src_ptr = (uint8*) src32_ptr;
           dst_ptr = (uint8*) dst32_ptr;
        }

        /* Then copy any trailing bytes. */
        remainder = len & WORDMASK;
        if (remainder)
            do 
               bStor8(--dst_ptr, bLoad8((uint8*)--src_ptr));
            while (--remainder);
    }
  } while (0);

}/* qmemcpy */


/*===========================================================================

**  Function : boot_count_set_bits
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function Counts the number of bits set in an uint32.
*   
* @param[in] input_val:  uint32 Input value whose set bits need to be 
*                        counted. 
* 
* @par Dependencies
*    None
* 
* @retval
*    count: uint32 Number of bits set to 1.
* 
* @par Side Effects
*    None
* 
*/
uint32 boot_count_set_bits(uint32 input_val)
{
  uint32 count = 0;
  for(count = 0; input_val; count++)
  {
    input_val &= input_val - 1 ;
  }
  return count;
}


/*===========================================================================

**  Function : booting_from_storage_media
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns value indicating if target was booted off of
*    an internal storage device or not.
*   
* @param[in]
*    None
* 
* @par Dependencies
*    None
* 
* @retval
*    boolean, True if device was booted from internal storage device
              False if flashless
* 
* @par Side Effects
*    None
* 
*/
boot_boolean booting_from_storage_media(void)
{
  boot_boolean booting_from_storage_media = FALSE;
  boot_flash_type flash_type = NO_FLASH;


  /* Get the flash type reported by PBL.  This flash type has become a mixture
     of boot device and flash type so it must be evaluated to report if device
     was booted from an internal storage media or not. */
  flash_type = boot_pbl_get_flash_type();

  switch (flash_type)
  {
  case NAND_FLASH:
  case SDC_FLASH:
  case MMC_FLASH:
  case UFS_FLASH:
    booting_from_storage_media = TRUE;
    break;

  default:
    booting_from_storage_media = FALSE;
  }

  return booting_from_storage_media;
}
