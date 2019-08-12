#ifndef TZBSP_MRC_FUSEPROV_H
#define TZBSP_MRC_FUSEPROV_H

/**
@file tzbsp_mrc_fuseprov.h
@brief fuse provision for mrc feature
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM Technology, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/17/14    hw      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/**
 * blow root cert index to switch the root cert in multiple root cert feature.
 * the default index is 0, so the function does no-op if passing integer 0.
 *
 * @param [in] apps_root_cert_index   the decimal index of the root cert on apps side.
 *             msa_root_cert_index   the decimal index of the root cert on msa side.
 *
 * @return 0 on call success
 */
int secboot_switch_mrc_index(uint32 apps_root_cert_index, uint32 msa_root_cert_index);

#endif /* TZBSP_MRC_FUSEPROV_H */
