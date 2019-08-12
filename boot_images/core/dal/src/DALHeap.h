#ifndef _DAL_HEAP
#define _DAL_HEAP
/*==============================================================================
                           DALHeap.h

GENERAL DESCRIPTION
    This file publish heap implementation for BOOT2.0

        Copyright © 2007-2012 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

    $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/dal/src/DALHeap.h#1 $
 
when       who     description
--------   ---     ---------------------------------------------------------- 
  09-05-2012  aa  Initial Version 
==============================================================================*/
/*=============================================================================
                           PROTOTYPE DECLARATIONS
=============================================================================*/
/**
  @brief Initialize internal and external heap

  Users of this api must invoke only to publish availability of new heap 
  to heap manager.

  @param pHeapBase  : Base address of the heap
  @param dwHeapSz   : Size of the heap
  @param dwInitType : Type of heap

  @return void
  
  @note : Inorder to initialize internal heap dwInitType passed to api must be 0.
          for any external api, like DDR any unsigned non zero value , 1 preferred.

*/
void DALSYS_HeapInit(void *pHeapBase, uint32 dwHeapSz, uint32 dwInitType);

/**
  @brief Return the external memory state.

  Users must invoke this API to check if external(DDR) memory is up and initialized.

  @param void 

  @return 0 : external memory is not available
          1 : external memory available

*/
uint32 IsExternalMemReady(void);

#endif //_DAL_HEAP

