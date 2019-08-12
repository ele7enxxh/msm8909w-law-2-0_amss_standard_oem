#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_
/*===========================================================================
  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/15    wt     Removed AOSTLM
07/24/14    wt     Added AOSTLM app
04/17/14    rk     Moved out of tzdrm
04/03/14    tp     Modified by running "atyle --style=allman" command.
02/04/14    rk     Removed warning pragma as part of fs/time cleanup
06/29/13    cz     Added widevine dash ID
12/11/12    cz     Added a comment of SVC_DRMPROV_ID
05/19/12    rk     Added new cmd handler for provisioning code.
01/19/12   chm     Added support for QSECOM.
12/17/11   chm     Additional security checks after QPSI review.
12/14/11   chm     Added support for sample application.
12/07/11   chm     Adding Input Validation of non-secure buffers sent to TZ.
05/11/11    cz     Added a path in tz_prov_provision_rsp_t to support chmod/chown
04/28/10   chm     Added support for decryption using TZBSP Crypto Driver.
04/28/11   chm     Added support for Memory protection API's.
03/24/11   jct     Added testdir request and response structures
03/03/11   jct     Added fs and time command id's
02/09/11   ssm     Initial Version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "stdlib.h"
#include "qsee_core.h"

#define APP_CORE_SVC_ID             0x07000000

/* Add your Services here ;
   you could have a single service that
   handles all your API's or you can create
   different handlers for each service that
   your application provides */
#define SVC_TZCOMMMON_ID            0x00000000

/* No command + response should be larger than this. */
#define TZ_CM_MAX_CMD_LEN 20 * 1024

/* tzcommon_cmd_handler for tzcommon services
   by adding a weak link we allow for a service to
   be conditionally compiled
*/
//__weak void tzcommon_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);
void tzcommon_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);

/* playready_cmd_handler for playready services */
#define SVC_PLAYREADY_ID             0x00010000
//__weak void playready_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);
void playready_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);


/* widevine_cmd_handler for widevine services */
#define SVC_WIDEVINE_ID              0x00020000
//__weak void widevine_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);
void widevine_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);

/* sampleapp_cmd_handler for sample services */
#define SVC_SAMPLEAPP_ID              0x00040000
void sampleapp_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);

/* drmprov_cmd_handler for drm key provisioning */
/* DRMPROV is not a seperate image, it is common to other images*/
#define SVC_DRMPROV_ID              0x00050000
void drmprov_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);

/* widevine_dash_cmd_handler for drm key provisioning */
#define SVC_WIDEVINE_DASH_ID        0x00060000
void widevine_dash_cmd_handler(void* cmd,uint32 cmdlen,void* rsp,uint32 rsplen);

#ifdef ASSURANCETEST
#define SVC_ASSURANCETEST_ID       0x00060000
void assurancetest_cmd_handler(void * cmd, uint32 cmdlen, void * rsp, uint32 rsplen);
#endif

/**
  Commands for TZ OS Core
 */
#define APP_CORE_CREATE_CMD(x)             (APP_CORE_SVC_ID | x)

#define APP_CORE_CMD_INVALID                APP_CORE_CREATE_CMD(0)
#define APP_CORE_CMD_INIT_FN_TABLE          APP_CORE_CREATE_CMD(1)     /**< Initialize the function table from TZBSP */

/* undefined commands */
#define APP_CMD_UNKNOWN          0x7FFFFFFE
#define APP_CMD_MAX              0x7FFFFFFF


/* A common structure that can be used by any service to
   respond to an unsupported command request */
typedef struct app_unknown_rsp_s
{
    /** First 4 bytes should always be command id */
    uint32          core_cmd;
} __attribute__ ((packed)) app_unknown_rsp_t;

#endif /* _APP_MAIN_H_ */
