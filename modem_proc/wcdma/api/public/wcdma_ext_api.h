#ifndef WCDMA_EXT_API_H
#define WCDMA_EXT_API_H

/*==========================================================================

      Diagnostic Services Packet Processing Command Code Defintions

Description
  This file contains the definitions for the external APIs used by non Modem
  modules.  

Copyright (c) 1993 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/public/wcdma_ext_api.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    -----------------------------------------------------------
04/30/01   pr    Created file for AVS compilation error in non WCDMA builds
                 and unification of all external APIs
===========================================================================*/
/*Do not include any WCDMA .c files here*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "sys.h"
#include "msg.h"
#include "wcdmamvsif.h"

#define WCDMA_STATUS_GOOD         ( 0x00000000 ) /**< Success / completed / no errors. */
#define WCDMA_STATUS_FAILED       ( 0x00000001 ) /**< General failure. */
#define WCDMA_STATUS_BADPARAMS    ( 0x00000002 ) /**< Bad operation parameter(s). */
#define WCDMA_STATUS_UNSUPPORTED  ( 0x00000003 ) /**< Unsupported routine/operation. */
#define WCDMA_STATUS_NOMEMORY     ( 0x00000004 ) /**< Unable to allocate enough memory. */


#define WCDMA_MVS_REGISTER_FOR_MODEM_TIMING ( 0x00000000 )
#define WCDMA_GET_L2_UL_CMD_BUFFER ( 0x00000001 )
#define WCDMA_PUT_L2_UL_CMD ( 0x00000002 )
#define WCDMA_GET_L2_DL_CMD_BUFFER ( 0x00000003 )
#define WCDMA_PUT_L2_DL_CMD ( 0x00000004 )

typedef struct
{
  l2_ul_cmd_type* service;
    /* The caller provides a pointer to the UL service registration data structure. */
}wcdma_cmd_l2_ul_cmd_buffer_t;

typedef struct
{
  l2_dl_cmd_type* service;
    /* The caller provides a pointer to the DL service registration data structure. */
}wcdma_cmd_l2_dl_cmd_buffer_t;

extern uint32 wcdma_ext_api(uint32 cmd_id, void* params, uint32 size);

#endif  /* #ifndef WCDMA_EXT_API_H */
