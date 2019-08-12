#ifndef CUSTNVM_H
#define CUSTNVM_H
/*===========================================================================

DESCRIPTION
  Configuration for NVM.

  Copyright (c) 2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custnvm.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/02   jct     Created
===========================================================================*/

/* NV task will auto-detect the EEPROM size and automatically
** scale the roaming list size
*/
#undef  FEATURE_NV_AUTOSIZE

/* Phone's NV memory will support one NAM of 625 bytes, and the one-time
   subsidy lock item
*/
#define  FEATURE_NV_ONE_NAM_RL_LARGE

/* This feature configures NV to have two small roaming lists for a two NAM phone,
** as opposed to having one large roaming list for a one NAM phone.
*/
#undef  FEATURE_NV_TWO_NAMS_RL_SMALL

/* Define to enable the use of a trimode rf calibration file to derive
** NV items.  Also triggers the NV section to support this feature.
*/
#define FEATURE_TRIMODE_ITEMS

/* Enable reading of BTF value from NV
*/
#define FEATURE_ENC_BTF_IN_NV

#if defined FEATURE_BUILD_CDMA_TRIMODE || \
    defined FEATURE_BUILD_CDMA_PCS
      /* Define to enable the use of a trimode rf calibration file to derive
      ** NV items.  Also triggers the NV section to support this feature.
      */
      #define FEATURE_TRIMODE_ITEMS
#endif

#endif /* CUSTNVM_H */

