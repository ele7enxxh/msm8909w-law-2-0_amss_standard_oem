#ifndef CUSTEFS_H
#define CUSTEFS_H
/*===========================================================================
 * custefs.h
 *
 * DESCRIPTION
 * Configuration for EFS Feature.
 *
 * Copyright (c) 2011  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                    EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/cust/inc/custefs.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08-25-11   rp      Remove compile time definition of EFS2_ON_RMTS.
01/27/11   wek     Made Diag Processor ID depend if building for apps.
01/12/11   vr      Featurize with FEATURE_EFS2 to enable removal of EFS
01/11/11   vr      We dont have RMTS on modem. Changed to IMAGE_QDSP6_PROC.
07/07/10   rp      Created
===========================================================================*/

#ifndef BUILD_BOOT_CHAIN

#ifdef FEATURE_EFS2
  #define FEATURE_FS_BENCHMARK
  #undef FEATURE_FS_UNSAFE_BENCHMARK

  #ifdef IMAGE_QDSP6_PROC
    #define EFS_PARTITION_NAME   MIBIB_EFS2_QDSP_PARTI_NAME
  #endif

  #ifdef IMAGE_APPS_PROC
    #define EFS_DIAG_SUBSYS_PROCESSOR_ID  1
  #else
    #define EFS_DIAG_SUBSYS_PROCESSOR_ID  0
  #endif

  /* Defines DIAG access to EFS on both processors.
   * Q6    => Subsys ID : 19 (ie Primary DIAG_SUBSYS_FS)
   * ARM9  => Subsys ID : 62 (ie Alternate DIAG_SUBSYS_FS_ALTERNATE) */
  #ifdef IMAGE_QDSP6_PROC
    #define EFS_DIAG_USE_THIS_SUBSYS_ID DIAG_SUBSYS_FS
  #else
    #define EFS_DIAG_USE_THIS_SUBSYS_ID DIAG_SUBSYS_FS_ALTERNATE
  #endif


#define HFAT_WR_DATACACHE_SIZE                128

#ifdef FEATURE_EFS_HOTPLUG
  #define HOTPLUG_HAVE_PARTITION_SUPPORT
#endif


/*---------------------------------------------------------------------------
 *          E F S 1    A P I
 *
 *   EFS1 API's are deprecated and will be removed soon.
 *   Following will be deleted when EFS1 API's are removed.
 *
 *--------------------------------------------------------------------------*/
#define FEATURE_EFS
#define FEATURE_EFS_ACCESS_METHODS
#define FEATURE_NV_ITEM_MGR

#endif /* FEATURE_EFS2 */

#else  /* BUILD_BOOT_CHAIN */

  /* Define flags for boot */
  #define FEATURE_EFS2
  #if defined(OSBL) || defined(SSBL)
    #define FEATURE_EFS_EXTFS
    #define FEATURE_EFS_EXTFS_HFAT
  #endif

  /* Tune for RAM usage */
  #define FS_HFAT_MAXVOLUME_COUNT   2
  #define FS_HFAT_MAX_OPEN_FILES    8
  #define FS_VNODE_POOL_SIZE        16

  #define FS_MAX_ITERATORS          2
  #define FS_MAX_EXTFS_MOUNTS       3

  #define FS_PATH_MAX               300
  #define FS_NAME_MAX               100
  #define HFAT_MAX_PATH_UTF16       90
  #define HFAT_MAX_PATH_UTF16_ALLOW 89

#endif /* BUILD_BOOT_CHAIN */

#endif /* CUSTEFS_H */

