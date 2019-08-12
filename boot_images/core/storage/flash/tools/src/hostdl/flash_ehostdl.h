#ifndef FLASH_EHOSTDL_H
#define FLASH_EHOSTDL_H

/*==================================================================
 *
 * FILE:        flash_ehostdl.h
 *
 * SERVICES:
 *
 * DESCRIPTION:
 *    This file contains the macros to be used in e-hostdl scl file
 *    to map RW/ZI/CONSTDATA of e-hostdl object files (except DDR)
 *    to IMEM
 *
 * Copyright (c) 2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/flash_ehostdl.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/29/08     bb      Initial Revision
 *==================================================================*/

#ifdef FLASH_INCLUDE_EHOSTDL_H

  /* RW Modules that needs to be loaded onto IMEM */
  #define FLASH_EHOSTDL_RW_MODULES                                 \
                                     pm*.##o (+RW)                 \
                                     DAL*.##o (+RW)                \
                                     flash_dal*.##o (+RW)          \
                                     busywait*.##o (+RW)           \
                                     DalSBI*.##o (+RW)

  /* ZI Modules that needs to be loaded onto IMEM */
  #define FLASH_EHOSTDL_ZI_MODULES                                 \
                                     DALModEnv##.##o (+ZI)         \
                                     HALsbi*.##o (+ZI)             \
                                     busywait*.##o (+ZI)           \
                                     pm*.##o (+ZI)

  /* Constdata Modules that needs to be loaded onto IMEM */
  #define FLASH_EHOSTDL_CONST_MODULES                              \
                                     pm*.##o (.##constdata)        \
                                     DAL*.##o (.##constdata)       \
                                     flash_dal*.##o (.##constdata) \
                                     HALsbi*.##o (.##constdata)    \
                                     busywait*.##o (.##constdata)  \
                                     DALSBI*.##o (.##constdata)
#else

  #define FLASH_EHOSTDL_RW_MODULES
  #define FLASH_EHOSTDL_ZI_MODULES
  #define FLASH_EHOSTDL_CONST_MODULES

#endif /* FLASH_INCLUDE_EHOSTDL_H */
#endif /* FLASH_EHOSTDL_H */
