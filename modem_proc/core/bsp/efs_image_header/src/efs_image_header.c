/***********************************************************************
 * efs_image_header.c
 *
 * EFS Image Header Generation.
 *
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * This file provides a way to specify source file dependency for the
 * fs_image_hdr_builder. The builder should regenerate the efs images when
 * the defines in the .builds file changes. But since the .buidls file
 * comes with different names on different targets cannot directly specify
 * the .builds as a dependency and we need a source that is steady and can
 * be used on any target. This file is used to create that source
 * dependency. The file will reference the defines that will be used by the
 * final builder and the preprocessed output of this file will be used as
 * its source dependency becasue the md5 on the preprocessed file will
 * always change as long as the md5 on the generated cust file containing
 * the defines changes.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/bsp/efs_image_header/src/efs_image_header.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-02-01   nr    Create Dummy variables for dictionary creation.
2012-02-27   dks   Create. Add source file dependency to the builder.

===========================================================================*/

#ifdef CUST_H
  #include CUST_H
#endif

/*
 * Dummy variables to help generate the dictionary for the efs image headers.
 */

#ifdef SCL_MODEM_EFS_RAM_BASE
  const unsigned int FS_MODEM_RAM_ADDR = SCL_MODEM_EFS_RAM_BASE;
#endif

#ifdef SCL_MODEM_EFS_RAM_SIZE
  const unsigned int FS_MODEM_RAM_SIZE = SCL_MODEM_EFS_RAM_SIZE;
#endif

#ifdef SCL_SHARED_EFS_RAM_BASE
  const unsigned int FS_SHARED_RAM_ADDR = SCL_SHARED_EFS_RAM_BASE;
#endif

#ifdef SCL_SHARED_EFS_RAM_SIZE
  const unsigned int FS_SHARED_RAM_SIZE = SCL_SHARED_EFS_RAM_SIZE;
#endif

#ifdef SCL_GOLDEN_EFS_RAM_BASE
  const unsigned int FS_GOLDEN_RAM_ADDR = SCL_GOLDEN_EFS_RAM_BASE;
#endif

#ifdef SCL_GOLDEN_EFS_RAM_SIZE
  const unsigned int FS_GOLDEN_RAM_SIZE = SCL_GOLDEN_EFS_RAM_SIZE;
#endif
