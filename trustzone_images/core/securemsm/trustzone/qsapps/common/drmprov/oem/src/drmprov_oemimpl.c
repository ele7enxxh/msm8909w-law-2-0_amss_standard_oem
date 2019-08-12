/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  Copyright (c) 2013-14 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/common/drmprov/oem/src/drmprov_oemimpl.c#1 $

when         who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14    tp      Modified by running "atyle --style=allman" command.
10/22/12    cz      Initial Version.
===========================================================================*/
#include "comdef.h"
#include "string.h"
#include "qsee_log.h"
#include "drmprov_oemimpl.h"



long oem_unwrap_keys(uint8 *wrapped_data, uint32 wrapped_data_len, uint32 posOffset, uint8* unwrapped_data, uint32* unwrapped_data_len)
{
    long ret = 0;
    do
    {
        if ( wrapped_data== NULL || unwrapped_data == NULL )
        {
            TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s: input is invalid",__FUNCTION__);
            ret = -1;
            break;
        }

        /*
        ** dummy reference implementation: just memory copy the input to output buff
        ** The real implementation need to be done by OEMs to unwrap the wrapped keys.
        */
        memscpy(unwrapped_data, *unwrapped_data_len, wrapped_data + posOffset, wrapped_data_len);
        *unwrapped_data_len = wrapped_data_len;

    }
    while(0);

    return ret;
}


