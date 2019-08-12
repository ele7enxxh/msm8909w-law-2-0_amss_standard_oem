#ifndef __IOVEC_H__
#define __IOVEC_H__

/**
   @file iovec.h

   @brief 
   Input Output Vector Header File.

   Externalized Functions:
   None. 

   Initialization and sequencing requirements:
   None.
*/
/*============================================================================

                I N P U T - O U T P U T   V E C T O R 

                     H E A D E R   F I L E 

Copyright (c) 2006-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=========================================================================== */

/*=========================================================================
                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/iovec.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $  

when       who     what, where, why
--------   ---     --------------------------------------------------------
01/10/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
09/30/10   nk      Removed feature UXIOVEC_DEBUG_PRINT from api file.
07/20/10   nk      Added doxgen related string manuplation function descriptions
01/12/06   nk      Added Header information.

=========================================================================== */
/** @addtogroup SMECOMUTIL_IOVEC 
@{ 
*/

//#define UXIOVEC_DEBUG_PRINT

/**
 Defines the default IOVEC table size.
*/
#define DEFAULT_IOVEC_TABLE_SIZE 8

/**
   Defines the IOVEC page size.
*/
#define IOVEC_PAGE_SIZE 4096

/**
   Used as a placeholder for an unsigned 8-bit integer. The native type is
   unsigned character. 
*/
typedef unsigned char uint8;

/**
   Used as a placeholder for an unsigned 32-bit integer. The native type is
   unsigned long. 
*/
typedef unsigned long uint32;

/**
   Defines the type of buffer pool (local or shared).
*/
typedef enum UxBufPool_
{
  BUF_POOL_LOCAL,               /**< Local buffer pool.         */
  BUF_POOL_SHMEM,               /**< Shared memory buffer pool. */ 
UxBufPool_SIZER = 0x0FFFFFFF    /**< Buffer pool boundary.      */
} UxBufPool;

/**
   @brief Place holder for the starting address and length in bytes
   of an IOVEC.
*/
typedef struct {
void   *pvBase;             /**< Starting address. */
uint32  dwLen;              /**< Length in bytes.  */
}UxIOVEC;

/**
   @brief Place holder for the IOVEC pointer, IOVEC size, maximum size, total byte
   count, offset, reference counter, and self-referential pointer to the IOVEC
   table.
*/
typedef struct tUxIOVEC_T {
UxIOVEC *iov;                  /**< Pointer to the IOVEC table.                 */
uint32 size;                   /**< Number of IOVECs (size of the IOVEC table). */
uint32 max_size;               /**< Maximum number of IOVECs that can be 
                                    allocated.                                  */
uint32 tot_byte_cnt;           /**< Total byte count.                           */
uint32 offset;                 /**< Starting data byte.                         */
uint32 m_refcnt;               /**< Reference count.                            */
struct tUxIOVEC_T *m_refptr;   /**< Pointer to the original IOVEC table
                                    referenced.                                 */
UxBufPool pooltype;
}UxIOVEC_T;

/**
   Gets the IOVEC size (the number of bytes in the IOVEC array).

   @param[in] iovt Pointer to the IOVEC array.

   @return
   Returns the size of the IOVEC array.

   @dependencies 
   None.
*/
uint32 uxiovect_size(UxIOVEC_T *iovt);

/**
   Gets the IOVEC length (the total byte count of the IOVEC array).

   @param[in] iovt Pointer to the IOVEC array.

   @return
   Returns the byte count of the IOVEC array.

   @dependencies 
   None
*/
uint32 uxiovect_length(UxIOVEC_T *iovt);

/**
   Extracts the number of bytes defined in the parameters.

   This function extracts the exact number of bytes, starting at the offset from
   the beginning to the destination, from the IOVEC with count members. This 
   keeps the IOVEC intact.

   @param[in] iovt           Pointer to the IOVEC array.
   @param[in] offset         Offset of the IOVEC to be extracted.
   @param[in,out] dest       Destination pointer.
   @param[in] num            Number of bytes to be extracted.
   @param[in,out] bytes_read Pointer to the bytes read.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   The offset must be set before using this function.
*/
int uxiovect_extract(UxIOVEC_T *iovt, uint32 offset, uint8 *dest, uint32 num, uint32 *bytes_read);

/**
   Writes the number of bytes defined in parameters from the source to the
   IOVEC, starting at iovt->offset.

   @param[in,out] iovt          Pointer to the IOVEC table.
   @param[in] offset            Offset value from which to write.
   @param[in] src               Pointer to the source.
   @param[in] num               Number of bytes.
   @param[in,out] bytes_written Pointer to the bytes written.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_write(UxIOVEC_T *iovt, uint32 offset, uint8* src, uint32 num, uint32 *bytes_written);

/**
   Assigns the data passed to the IOVEC table.

   @note1hang The contents of the IOVEC table are purged and the passed data
              is assigned to the table. 

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] src      Pointer to the source data.
   @param[in] num      Number of bytes.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_assign(UxIOVEC_T *iovt, uint8 *src, uint32 num);

/**
   Assigns one IOVEC table to another.

   @note1hang The contents of the destination IOVEC table are purged and the
   new table data is assigned to the table.

   @param[in,out] iovt Pointer to the destination IOVEC table.
   @param[in] src      Pointer to the source IOVEC table.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_assign_iovect(UxIOVEC_T *iovt, UxIOVEC_T *src);

/**
   Appends the data passed to the IOVEC table. The function appends the number
   of bytes to the end of the IOVEC table. 

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] src      Pointer to the source data.
   @param[in] num      Number of bytes to be appended.

   @return
   Returns 0 on success and -1 on failure.\n
   Implicitly returns the number of bytes appended.

   @dependencies 
   The buffer must be preallocated.\n
   The IOVEC table must be large enough to hold an extra vector.
*/
int uxiovect_append(UxIOVEC_T* iovt, uint8* src, uint32 num);

/**
   Appends one IOVEC table to another. The source and destination IOVEC table
   pointers are passed as parameters.

   @param[in,out] dest Pointer to the destination IOVEC table.
   @param[in] src      Pointer to the source IOVEC table.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_append_iovect(UxIOVEC_T* dest, UxIOVEC_T *src);

/**
   @brief 
   Appends the IOVEC table after allocating memory for copying contents.

   This function appends a quantity of num bytes to the end of a preallocated
   IOVEC table. The difference between this call and the uxiovect_append()
   call is that append_copy first allocates a new buffer and then copies the
   contents into it. This way, the IOVEC table owns all the memory and cannot
   run into a problem with a static buffer being appended to it.  

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] src      Pointer to the source.
   @param[in] num      Number of bytes to be appended.

   @return
   Returns 0 on success and -1 on failure.\n
   Implicitly returns the number of bytes appended.

   @dependencies 
   The IOVEC table must be large enough to hold an extra vector.
*/
int uxiovect_append_copy(UxIOVEC_T* iovt, uint8* src, uint32 num);

/**
  Adds space to the end of an IOVEC table.

   This function adds a quantity of num bytes to the end of the specified IOVEC
   table for use as a preallocated buffer.

   @note1hang The difference between this function and uxiovect_append() is that
              this function knows that this space is just empty space and does not contain
              valid data. The append function, however, increases the tot_byte_cnt counter
              because it is assumed to be valid data that is being added.

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] src      Pointer to the source.
   @param[in] num      Number of bytes to be added.

   @return
   Returns 0 on success and -1 on failure.\n
   Implicitly returns the number of bytes appended.

   @dependencies 
   The IOVEC table must be large enough to hold an extra vector.
*/
int uxiovect_addspace(UxIOVEC_T* iovt, uint8* src, uint32 num);

/**
   Inserts a preallocated buffer into the IOVEC table, adjusting its size.

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] src      Pointer to the source.
   @param[in] num      Number of bytes to be inserted.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_prepend(UxIOVEC_T* iovt, uint8* src, uint32 num);

/**
   Prepends one IOVEC table to another. 

   @param[in,out] dest Pointer to the destination IOVEC table.
   @param[in] src      Pointer to the source IOVEC table.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_prepend_iovect(UxIOVEC_T* dest, UxIOVEC_T *src);

/**
   Truncates bytes in the IOVEC table from the offset to the end.

   This function shortens the IOVEC table by a total number of bytes in the
   offset.

   @note1hang This truncate implementation does not actually clean or remove
              the bytes from the offset to the end of the table; only the size of table is
              reduced. This way, the items stay in the table, but are never used, so 
              uxiovect_entry() must be called to actually free the bytes. 

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] offset   Offset bytes.

   @return
   Returns 0 on success and -1 on failure.\n
   Implicitly returns the number of bytes truncated.

   @dependencies 
   None.
*/
int uxiovect_trunc(UxIOVEC_T *iovt, uint32 offset);

/**
   Removes a quantity of num bytes starting at the offset. If
   (offset + num) > table length, the function removes all bytes 
   from the offset to the end of the table. 

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] offset   Offset bytes.
   @param[in] num      Number of bytes to be trimmed.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_trim(UxIOVEC_T* iovt, uint32 offset, uint32 num);

/**
   Duplicates an IOVEC table.

   This function does a shallow copy of one table to another. The shallow
   copy spans a range from offset to offset plus length and copies the segments
   of the source IOVEC table to the destination table. The IOVEC pointers in the
   destination table will point to the same data as the src table pointer. 

   @param[in,out] dest Pointer to the destination IOVEC table.
   @param[in] src      Pointer to the source IOVEC table.
   @param[in] offset   Offset bytes.
   @param[in] deep     Deep flag. If TRUE, performs a deep copy rather than a
                       shallow copy.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_duplicate(UxIOVEC_T *dest, UxIOVEC_T *src, uint32 offset, uint32 len, bool deep);

/**
   Inserts a previously allocated IOVEC into an existing IOVEC table. If
   no IOVEC is currently in the table, the inserted IOVEC becomes the only
   one in the table. 

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] index    Zero-based index of the IOVEC to insert before the
                       indexed IOVEC.
   @param[in] src Pointer to the preallocated buffer for the new IOVEC.
   @param[in] len Length of the new IOVEC.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_insert_index(UxIOVEC_T *iovt, uint32 index, uint8* src, uint32 len);

/**
   Inserts a previously allocated IOVEC into an existing IOVEC table at
   the specified offset. This will split any IOVEC that the offset lands on and
   insert the data into the table at that point. 

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] offset   Offset bytes.
   @param[in] src      Pointer to the preallocated buffer for the new IOVEC.
   @param[in] len      Length of the new IOVEC.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_insert(UxIOVEC_T *iovt, uint32 offset, uint8* src, uint32 len);

/**
   Removes an IOVEC from an existing IOVEC table based on a specified
   index. 

   @param[in,out] iovt Pointer to the IOVEC table.
   @param[in] index    Index value.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_remove_index(UxIOVEC_T *iovt, uint32 index);

/**
   Removes all IOVECs from an IOVEC table. The application is responsible for
   deallocating the memory to which the removed IOVEC objects refer. For table 
   handle deallocation, call uxiovect_entry(). 

   @param[in,out] iovt Pointer to the IOVEC table.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.

   @sideeffects 
   Caller or application must free allocated memory.
*/
int uxiovect_removeall(UxIOVEC_T *iovt);

/**
   Gets the IOVEC that contains the offset. 

   @param[in] iovt   Pointer to the IOVEC table.
   @param[in] offset Offset bytes.

   @return
   Returns the IOVEC that contains the offset.

   @dependencies 
   None.
*/
UxIOVEC *uxiovect_getsegment(UxIOVEC_T *iovt, uint32 offset);

/**
   Intializes the IOVEC table with default values and allocates the memory
   required for the table. 

   @param[in] pool Buffer pool type.

   @return
   Returns a pointer to the intialized IOVEC table.

   @dependencies 
   None.
*/
UxIOVEC_T *uxiovect_init(UxBufPool pool);

/**
   Deletes an IOVEC table and frees all allocated memory. 

   @param[in] iovt Pointer to the IOVEC table.

   @return
   None.

   @dependencies 
   None.
*/
void uxiovect_delete(UxIOVEC_T *iovt);

/**
   Removes an IOVEC table entry and frees memory. 

   @param[in] iovt Pointer to the IOVEC table.

   @return
   None.

   @dependencies 
   None.
*/
void uxiovect_entry(UxIOVEC_T *iovt);

/**
   Allocates an IOVEC table.

   @param[in] iovt   Pointer to the IOVEC table.
   @param[in] offset Offset bytes.
   @param[in] size   Size of the IOVEC table.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_alloc(UxIOVEC_T *iovt, uint32 offset, uint32 size);

/**
   Grows (creates) a new IOVEC table. If an IOVEC table runs out of space, a new 
   table is created that is double the size of the old table. The contents from
   the original table are copied into the new table and the original table is
   then freed. 

   @param[in,out] iovt Pointer to the IOVEC table.

   @return
   None.

   @dependencies 
   None.
*/
int uxiovect_growtable(UxIOVEC_T *iovt);

/**
   Frees all IOVECs in an IOVEC table.

   @note1hang This function should not be called unless it is certain that all 
   memory is accounted for. If, for example, an IOVEC record is in two places,
   the function will fail on the second attempt to free the same memory item.

   @param[in] iovt Pointer to the IOVEC table.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_freeall(UxIOVEC_T *iovt);

/**
   Resets an IOVEC table.

   This function removes the IOVEC table entries but does not delete or free the
   data. This effectively resets all IOVEC values to the values it contained
   when the table was first created. 

   @param[in,out] iovt Pointer to the IOVEC table.

   @return
   Returns 0 on success and -1 on failure.

   @dependencies 
   None.
*/
int uxiovect_reset(UxIOVEC_T *iovt);

/**
   Prints the IOVEC table.

   @note1hang This function is used for debugging purposes only.

   @param[in] iovt Pointer to the IOVEC table.

   @return
   None.

   @dependencies 
   None.
*/
void uxiovect_print(UxIOVEC_T *iovt);

/** @} */  /* end_addtogroup SMECOMUTIL_IOVEC */

#endif // __IOVEC_H__
