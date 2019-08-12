#ifndef HYP_HVCAPI_H
#define HYP_HVCAPI_H
/**
@file hyp_hvcapi.h
@brief Hypervisor HVC call API definitions

This file contains the definition of hypervisor HVC call API 
info 

*/
/*===========================================================================
   Copyright (c) 2013-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/inc/hyp_hvcapi.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------ 
5/29/14    dc       Created 
=============================================================================*/

/*===========================================================================
                      HVC CALL
===========================================================================*/
#define HVC32_GET_VERSION         0x82000000
#define HVC32_GRANT_ACCESS        0x82000001
#define HVC32_REMOVE_ACCESS       0x82000002


#define HVC_VERSION_INFO          0x00000001 // 0x0100 : AArch64
                                             // 0x0000 : AArch32


/*===========================================================================
                      HVC Result
===========================================================================*/
#define HVC_CALL_SUCCESS                   0
#define HVC_CALL_NON_SUPPORT_ID           -1
#define HVC_CALL_ERROR                    -2

#endif 
