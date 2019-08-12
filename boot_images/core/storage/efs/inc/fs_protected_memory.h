/***********************************************************************
 * fs_protected_memory.h
 *
 * Section markers for EFS protected memory.
 *
 * Copyright (C) 2007-2009,2013 Qualcomm Technologies, Inc.
 *
 * Files that contain private memory include this header to mark the
 * sections appropriately.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/inc/fs_protected_memory.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-06-17   wek   Move only ZI buffers to DDR.
2013-04-26   wek   Move all buffers to DDR when compiling for boot.
2009-05-21   sh    Changed featurizing to exclude gcc.
2009-05-18   rp    Featurized for ARM compiler.
2008-10-17   yog   Featurized pragma directive to exclude for FS_STANDALONE.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-07-05   umr   Create

===========================================================================*/

#ifndef __FS_PROTECTED_MEMORY_H__
#define __FS_PROTECTED_MEMORY_H__

#if defined __arm && !defined FS_STANDALONE && !defined __GNUC__

#ifdef FEATURE_EFS_BOOT_ENV
  #pragma arm section zidata = "BOOT_DDR_ZI_DATA_ZONE"
#else
/* Use pragma directive for ARM, non-GCC, full AMSS builds */
  #pragma arm section rwdata = "efs_rw", rodata = "efs_ro", zidata = "efs_zi"
#endif
#endif

#endif /* not __FS_PROTECTED_MEMORY_H__ */
