#ifndef __TZ_SERVICE_H_
#define __TZ_SERVICE_H_
/*===========================================================================
  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/14    rk     Moved out of tzdrm
04/03/14    tp     Modified by running "atyle --style=allman" command.
03/07/14    rk     Removed unused declarations
02/26/14    rk     Added new structs and definitions used in qsee_fs.h
02/04/14    rk     Merged tzcommon_entry.h from qsapps/libs
04/04/13    dm     Added changes to support internal decrypt testing for QSAPPS
10/09/12    hw     Include header for non-contiguous buffer
06/11/12    cz     Added version contrl support
01/30/12    dm     Replaced protect_mem() with content protection and added new decrypt API.
01/19/12   chm     Added support for QSECOM.
12/22/11    kr     Update for CR#326083.
11/16/11    cz     Added comments for TZ_CM_MAX_NAME_LEN; and added TZ_PR_CMD_FILE_CHOWN_CHMOD
09/19/11    cz     Splited tz_playready.h
02/08/11    vs     Added Provisioning APIs
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
/** @addtogroup tz_playready
  @} */
#include "comdef.h"
#include "stdlib.h"
#include "app_common_svc_cmd.h"


/* Size of PR license or challenge */

#define TZ_CM_MAX_NAME_LEN          256   /* Fixed. Don't increase the size of TZ_CM_MAX_NAME_LEN*/
#define TZ_CM_MAX_DATA_LEN          20000
#define TZ_CM_PROV_PKG_SIZE         10240  //TZ_CM_PROV_PKG_SIZE must be smaller than TZ_CM_MAX_DATA_LEN

#define TZCOMMON_CREATE_CMD(x)  (SVC_TZCOMMMON_ID | x)


typedef void DIR;

typedef struct tzDirent
{
    uint64            d_ino;
    int64             d_off;
    unsigned short    d_reclen;
    unsigned char     d_type;
    char              d_name[TZ_CM_MAX_NAME_LEN];
} __attribute__ ((packed)) tzDirent_t;

/**
  Error Codes for FS
 */
typedef enum
{
    E_FS_SUCCESS         =  0,
    E_FS_FAILURE         = -1,
    E_FS_INVALID_ARG     = -2,
    E_FS_DIR_NOT_EXIST   = -3,
    E_FS_PATH_TOO_LONG   = -4,
} tz_common_error_codes;


/**
  Commands for :
  1) TZ Services requested by HLOS
  2) HLOS services requested by TZ
 */
typedef enum
{
    /* HLOS to TZ commands -
    ** Following commands represent services that HLOS could request from TZ.
    ** This is the traditional use case where HLOS will be the client and TZ will service the following requests.
    */
    TZ_CM_CMD_INVALID           = TZCOMMON_CREATE_CMD(0x00000000),
    TZ_CM_CMD_UNUSED1,            /**< UNUSED */
    TZ_CM_CMD_UNUSED2,            /**< UNUSED */
    TZ_CM_CMD_UNUSED3,            /**< UNUSED*/
    TZ_CM_CMD_REGISTER_LISTENER,
    TZ_CM_CMD_EXEC_TEST_START   = TZCOMMON_CREATE_CMD(0x00000101),
    TZ_CM_CMD_EXEC_TEST,
    TZ_CM_CMD_EXEC_TEST_END,

    /* TZ to HLOS commands -
    ** HLOS gets the TZ version
    */
    TZ_CM_CMD_VERSION_START     = TZCOMMON_CREATE_CMD(0x00000401),
    TZ_CM_CMD_VERSION_GET_VER,
    TZ_CM_CMD_VERSION_END,
    TZ_CM_CMD_CPCHECK_TOGGLE,   /**< turns on/off the content protection feature on TZ */

    TZ_CM_CMD_UNKNOWN           = TZCOMMON_CREATE_CMD(0x7FFFFFFF)
} tz_common_cmd_type;

/** Command structure for getting tzapps version
*/
typedef struct tz_qsappsver_get_ver_req_s
{
    /** First 4 bytes are always command id */
    tz_common_cmd_type       cmd_id;
} __attribute__ ((packed)) tz_qsappsver_get_ver_req_t;

typedef struct tz_qsappsver_get_ver_rsp_s
{
    /** First 4 bytes are always command id */
    tz_common_cmd_type       cmd_id;
    /** Version of tz apps */
    uint32                   version;
    /**<-- Return value for maintenance */
    int32                    ret;
} __attribute__ ((packed)) tz_qsappsver_get_ver_rsp_t;

/** Command structure to set the DRM CP flag
*/
typedef struct tz_qsapp_cpcheck_req_s
{
    /** First 4 bytes should always be command id */
    tz_common_cmd_type          cmd_id;
    /*flag for CP turning on/off*/
    uint8                       bContentProtection;
} __attribute__ ((packed)) tz_qsapp_cpcheck_req_t;

typedef struct tz_qsapp_cpcheck_rsp_s
{
    /** First 4 bytes should always be command id */
    tz_common_cmd_type          cmd_id;
    long                        ret;
} __attribute__ ((packed)) tz_qsapp_cpcheck_rsp_t;

typedef struct tz_unknown_rsp_s
{
    /** First 4 bytes should always be command id */
    tz_common_cmd_type       cmd_id;
} __attribute__ ((packed)) tz_unknown_rsp_t;

#endif /* __TZ_SERVICE_H_ */
