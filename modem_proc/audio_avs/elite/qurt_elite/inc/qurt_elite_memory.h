/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================= */
/**
@file qurt_elite_memory.h

@brief This file contains utilities for memory allocation and release. This
file provides memory allocation functions and macros for both C and C++.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_memory.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/11   ss       qurt_elite Placement new/delete
03/08/10   brs      Edited for doxygen-to-latex process.
02/04/10   mwc      Created file.
========================================================================== */

#ifndef QURT_ELITE_MEMORY_H
#define QURT_ELITE_MEMORY_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_types.h"
#include "qurt_elite_mutex.h"

#ifdef __cplusplus
#include <new>
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_memory
@{ */
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/** Maximum number of heaps supported in the Elite framework. */
#define QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS  10

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

//***************************************************************************
// Heap features
//***************************************************************************

/** ID of the available heap in the system.
*/
typedef enum
{
   QURT_ELITE_HEAP_DEFAULT=0,
   /**< Default heap value. */

   QURT_ELITE_HEAP_OUT_OF_RANGE=QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS+1
   /**< Heap value is out of range. */ //keep at end

}  QURT_ELITE_HEAP_ID;

/**
  Allocates memory with an option to specify a heap identifier.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] unBytes Number of bytes to allocate.
  @param[in] heapId Specifies the heap from which to allocate.

  @return
  Pointer to the allocated block, or NULL if the request failed.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
void *qurt_elite_memory_malloc(uint32_t unBytes, QURT_ELITE_HEAP_ID heapId);

/**
  Frees memory that was allocated with qurt_elite_memory_malloc().

  @param[in] ptr  Pointer to the memory to free.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_memory_free(void *ptr);

/**
  Allocates memory with options to align to a power of 2 and to
  specify a heap identifier.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] unBytes      Number of bytes to allocate.
  @param[in] unAlignBits  Number of bytes to which to align.
                          This number must be a power of 2.
  @param[in] heapId       The heap from which to allocate.

  @return
  Pointer to the allocated block, or NULL if the request failed.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
void *qurt_elite_memory_aligned_malloc(uint32_t unBytes, uint32_t unAlignBits, QURT_ELITE_HEAP_ID heapId);

/**
  Frees memory that was allocated with qurt_elite_memory_aligned_malloc().

  @param[in] ptr Pointer to the aligned memory to free.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_memory_aligned_free(void *ptr);

#ifdef __cplusplus
/**
  Macro that allocates a C++ object using qurt_elite_memory_malloc().

  This macro takes the pointer, type, and heap ID and completes two steps:
  - The memory allocation
  - Followed by a placement new, which calls the object's constructor

  This macro can take additional arguments that can be passed to the
  C++ object's constructor. This does not cause any exceptions. If the
  allocation fails, the returned pointer is NULL. 

  @param[out] pObject   Pointer to set to the new object location.
  @param[in]  typeType  Type of class that is being constructed.
  @param[in]  heapId    QURT_ELITE_HEAP_ID is requested for allocating the
                        new object.
  @param[in]  ...       Variable argument list for parameters that are to be
                        passed to the constructor.
  @dependencies
  None. @newpage
*/
#define qurt_elite_memory_new(pObject, typeType, heapId, ...) { \
   void *pObj = qurt_elite_memory_malloc(sizeof(typeType), heapId); \
   (pObject) = (pObj) ? (new (pObj) (typeType)(__VA_ARGS__)) : NULL; \
}

/**
  Macro that destroys an object created with qurt_elite_memory_new().
  After destroying the object, this macro frees the memory and sets the
  pointer to NULL.

  @param[in,out] pObject   Pointer to the object that is to be destroyed.
  @param[in]     typeType  Class of the object being destroyed.

  @dependencies
  None.
*/
#define qurt_elite_memory_delete(pObject, typeType) { \
   if (pObject) { (pObject)->~typeType(); \
   qurt_elite_memory_free (pObject); \
   (pObject) = NULL; } \
}

/**
  Macro that takes a pointer to the intended class, a pointer to memory already
  allocated, and the type (of class), and invokes the constructor of the class.

  This macro can take additional arguments that may need to be passed to the
  C++ object's constructor. This does not cause any exceptions.

  @param[out] pObj      Pointer to the intended class whose constructor
                        is to be invoked.
  @param[in]  pMemory   Pointer to memory that has been allocated.
  @param[in]  typeType  Type of class that is being constructed.
  @param[in]  ...       Variable argument list for parameters that are
                        to be passed to the constructor.

  @dependencies
  None. @newpage
*/
#define qurt_elite_memory_placement_new(pObj, pMemory, typeType, ...) { \
   (pObj) = (new (pMemory) (typeType)(__VA_ARGS__)); \
}

/**
  Macro that destroys an object created with qurt_elite_memory_placement_new().
  This macro invokes the destructor of the intended class and sets the
  pointer to NULL.

  @param[in,out] pObject   Pointer to the object that is to be destroyed.
  @param[in]     typeType  Class of the object being destroyed.

  @dependencies
  None.
*/
#define qurt_elite_memory_placement_delete(pObject, typeType) { \
   if (pObject) { (pObject)->~typeType(); (pObject) = NULL; }\
}

#endif //__cplusplus for new and delete


/**
  Allocates memory in the heap.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] size     Number of bytes to be allocated.
  @param[in] heap_id  Heap ID.

  @return
  Pointer to the allocated buffer.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
void* qurt_elite_memory_heapmgr_malloc(uint32_t size, QURT_ELITE_HEAP_ID heap_id);

/**
  Frees allocated memory.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] ptr      Pointer to the buffer to be freed.
  @param[in] heap_id  Heap ID.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_memory_heapmgr_free(void *ptr, QURT_ELITE_HEAP_ID heap_id);

/**
  Initializes the heap manager for a specified heap.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] heap_id_ptr     Pointer to the heap ID.
  @param[in] heap_start_ptr  Pointer to the start address of the heap.
  @param[in] heap_size       Size of the heap.

  @return
  Status of the heap manager creation.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
ADSPResult qurt_elite_memory_heapmgr_create(QURT_ELITE_HEAP_ID *heap_id_ptr, void *heap_start_ptr, uint32_t heap_size);

/**
  Uninitializes the heap manager of a specified heap ID.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] heap_id  Heap ID.

  @return
  Status of the heap manager deletion.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
ADSPResult qurt_elite_memory_heapmgr_delete(QURT_ELITE_HEAP_ID heap_id);

/**
  Initializes the heap manager heap table entries.

  @return
  None.

  @dependencies
  None. @newpage
*/
void qurt_elite_heap_table_init(void);


/**
Initializes the variables used for maintaining memory usage statistics for AVS and NON-AVS

@param[in] 
None

  @return
None
*/

void qurt_elite_memory_stats_init(void);

/**
Deinitializes the variables used for maintaining memory usage statistics for AVS and NON-AVS

@param[in] 
None

@return
None
*/
void qurt_elite_memory_stats_deinit(void);

/** @} */ /* end_addtogroup qurt_elite_memory */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_MEMORY_H

