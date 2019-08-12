/***********************************************************************
 * fs_cookie.h
 *
 * EFS Cookie Handler Interface for boot.
 * Copyright (C) 2012 Qualcomm Technologies, Inc.
 *
 * Provides interface function for boot to call to invoke handling of EFS
 * cookies.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/inc/fs_cookie.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-08-09   dks   Moved file to inc folder and renamed from fs_rmts_cookie.h
2012-06-12   dks   Create

===========================================================================*/

#ifndef __FS_COOKIE_H__
#define __FS_COOKIE_H__

/**
 * Handles all the EFS cookies that are set. Invoked by boot code.
 *
 * @par Synopsis
 *  @code
 *    #include "fs_cookie.h"
 *    void efs_handle_cookies (void);
 *  @endcode
 *
 *************************************************************************/

void efs_handle_cookies (void);


#endif /* not __FS_COOKIE_H__ */
