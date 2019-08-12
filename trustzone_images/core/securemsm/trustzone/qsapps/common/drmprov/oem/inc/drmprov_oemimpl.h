#ifndef DRMPROV_OEMIMPL_H
#define DRMPROV_OEMIMPL_H
/*==========================================================================

Description: Handlers for Generic provisining of DRM keys.
Copyright (c) 2011-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*==========================================================================

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/common/drmprov/oem/inc/drmprov_oemimpl.h#1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
04/03/14      tp     Modified by running "atyle --style=allman" command.
10/21/13      cz     Initial Version.
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "comdef.h"
#include "stringl.h"


    /**
      oem unwrap key api

      @return
      Zero for Success, non-zero for Failure

      @param[in]    *wrapped_data        input wrapped data buffer
      @param[in]     wrapped_data_len    input buffer len
      @param[in]    *posOffset           the offset from the beginning of wrapped_data
      @param[in]    *unwrapped_data      output buffer pointer
      @param[out]   *unwrapped_data_len  return how many bytes of data are unwrapped

      @dependencies
      None

      @sideeffects
      None
    */

    long oem_unwrap_keys
    (
        uint8 *wrapped_data,
        uint32 wrapped_data_len,
        uint32 posOffset,
        uint8* unwrapped_data,
        uint32* unwrapped_data_len
    );

#ifdef __cplusplus
}
#endif

#endif /* DRMPROV_OEMIMPL_H */

