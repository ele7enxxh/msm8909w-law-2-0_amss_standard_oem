#ifndef PLAYREADY_H
#define PLAYREADY_H

/** @file playready.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces that provides a wrapper for the PlayReady
 * API's
 */
/*===========================================================================
  Copyright (c) 2008 - 2014 QUALCOMM Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/playready/pkwrapper/shared/inc/playready.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/14    dm     Added support to toggle the IV constraint check at runtime
04/03/14    tp     Modified by running "atyle --style=allman" command.
11/14/13    wt     Added Media DRM API
09/03/13    ck     Added playready_duplicate_ctx()
05/20/13    dm     Added support to remove license.hds in playready_delete_keys
04/04/13    dm     Added changes to support internal decrypt testing for QSAPPS
04/03/13    dm     Added support to store AES and keyfile for internal testing purpose
03/15/13    kr     Updated playready_reinitialize() with more parameters
02/15/13    dm     Used enumeration for stream type
02/09/13    dm     Modified encrypt function to take type of stream as input argument
01/29/13    rk     Added new message to set robustness version dynamically
01/16/13    rk     Removed generic provisiong API prototype
12/14/12    dm     Modified encrypt function and removed the featurization
11/07/12    rk     Fixed pverify issue (after provdel) with PR.2.0
09/18/12   chm     Added support for licence deletion with header & KID BASE64 encoded.
09/13/12    dm     Added support to disable non-silent url parsing for envelope test cases
08/21/12    rk     Modified the interfaces to support PK2.0
06/07/12    rp     Added key hardcode toggle support for decrypt & domain testcases
05/30/12    dm     Added support for drm reinitialize and modified get property API.
04/16/12    kr     Additional PlayReady API support for WMDRM
04/11/12    rp     Added the support for turning on/off content protection check on TZ
03/26/12    dm     Added a new parameter in process license response.
03/21/12    kr     Add generic key provisioning APIs support
01/30/12    dm     Replaced protect_mem() with content protection and changed decrypt API
01/19/12    dm     Added audio decryption and tamepoering check.
12/14/11    dm     Added support for bind,commit,MTP,domaind and envelope.
10/14/11    cz     Cleaned up v1 functions and replace v1 funcation names with v2 names
10/12/11    cz     Added playready v2 functions and disabled v1 functions
02/08/11    vs     Added Provisioning APIs
04/28/10   chm     Added support for decryption using TZBSP Crypto Driver.
04/28/10   chm     Added support for memory protection.
01/20/10   ChM     Add support for WMDRM.
11/30/10   ChM     Challenge should be INOUT parameter.
10/19/10   ChM     Initial Version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup playready
  @} */
#include "comdef.h"
#include "playready_entry.h"

#define PLAYREADY_MAX_CHALLENGE_LENGTH 30000

/**
  Decrypt WM DRM Content Data.

  @return
  Success or Failure

  @param[in,out]  pbData						Pointer to encrypted/decrypted data.
  @param[in,out]  cbData						Size of encrypted/decrypted data.

  @dependencies
  Need to have called playready_initialize before calling this function.

  @sideeffects
  None
*/
long playready_reader_decrypt_wmdrm
(
    uint32       pbData,
    uint32       cbData
);


/**
  Create AES Key file if doesn't exist.
  Update AES Key file with new key if it
  already exists.  Updating with new key
  is the case of re-provisioning when there
  is a breach to the device.

  @return
  Success or Failure

  @param[in]       key_size        Size of key ID/AES Key.
  @param[in]       create_file      Flag which tells whether to create the file.

  @dependencies
  None

  @sideeffects
  None
*/
int playready_update_aes_file
(
    uint32         key_size,
    uint8          create_file
);

/**
  Save the keys.

  @return
  Success or Failure

  @param[in]     key             key file to store the data in
  @param[in]	  *msg_data        Pointer to file data
  @param[in]	   msg_len         Size of msg_data.

  @dependencies
  None

  @sideeffects
  None
*/
long playready_save_keys
(
    tz_pr_keys_type key,
    uint8         * msg_data,
    uint32          msg_len,
    uint8         * prt_path
);

/**
  Return the path to drmms folder after verifying the keys.

  @return
  Success or Failure

  @param[in]	  *prt_path        Pointer to file data

  @dependencies
  None

  @sideeffects
  None
*/
long playready_verify_keys
(
    uint8         * prt_path,
    uint8         * prt_path_drmms_sfs
);

/**
  Return the path to drmms folder after verifying the keys.

  @return
  Success or Failure

  @param
  None

  @dependencies
  None

  @sideeffects
  None
*/
long playready_delete_keys(void);

/**
  Initializes PlayReady DRM.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[out]    *app_ctx              Pointer to the Application context

  @dependencies
  Needs to be called before any other DRM functions are called.

  @sideeffects
  None
*/
long playready_initialize
(
    uint32         *app_ctx
);

/**
  UnInitializes PlayReady DRM.

  @return
  none

  @param[in]     app_ctx           Application context
  @param[in]     dec_ctx           Decrypt context
  @param[in]     dom_ctx           Domain context

  @dependencies
  Needs to be called to free up resources.

  @sideeffects
  None
*/
void playready_uninitialize
(
    uint32         app_ctx,
    uint32         dec_ctx,
    uint32         dom_ctx
);

/**
  Duplicates app context ID and return the new context ID.

  @return
  none

  @param[in]     ctx_id            Context ID.
  @param[in]     new_ctx_id        New context ID duplicated.

  @dependencies

  @sideeffects
  None
*/
long playready_duplicate_ctx
(
    uint32            ctx_id,
    uint32            *new_ctx_id
);

/**
  Get's a license challenge from the DRM Agent.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context
  @param[in]     **strRights        Array of pointers to the rights array.
  @param[in]       cRights          Number of elements in rights array.
  @param[in]      *domainID         Pointer to the domain ID.
  @param[in]      *pchCustomData    Pointer to the custom data.
  @param[in]       pcchCustomData   Size of the custom data.
  @param[out]     *pchSilentUrl     Contains the silent license acquisition URL.
  @param[in,out]  *pcchSilentUrl    Contains the size of the silent license acquisition URL.
  @param[out]     *pchNonSilentUrl  Contains the nonsilent license acquisition URL.
  @param[in,out]  *pcchNonSilentUrl Contains the size of the nonsilent license acquisition URL.
  @param[out]     *pbChallenge      Contains the resulting license challenge.
  @param[in,out]  *pcbChallenge     Contains the size of the resulting license challenge.

  @dependencies
  Need to have called playready_initialize before calling this function.

  @sideeffects
  None
*/
long playready_licacq_generate_challenge
(
    uint32             app_ctx,
    tzconststring_t  **strRights,
    uint32             cRights,
    tzdrmdomainid_t   *domainID,
    uint8             *pchCustomData,
    uint32             pcchCustomData,
    uint8             *pchSilentUrl,
    uint32            *pcchSilentUrl,
    uint8             *pchNonSilentUrl,
    uint32            *pcchNonSilentUrl,
    uint8             *pbChallenge,
    uint32            *pcbChallenge
);

/**
  Store a license in the license database.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     app_ctx                 Application context
  @param[in]    *pbResponse              Pointer to license response.
  @param[in]     cbResponse              Size of license response.
  @param[out]   *poLicenseResponse       Pointer to receive the result of the license processing response

  @dependencies
  Need to have called playready_set_header() before calling this function.

  @sideeffects
  None
*/
unsigned long playready_licacq_process_response
(
    uint32                   app_ctx,
    uint8                   *pbResponse,
    uint32                   cbResponse,
    tzdrmlicensersp_t       *poLicenseResponse
);

/**
  Delete's a license from the store

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]    app_ctx      Application context
  @param[in]    KID          Pointer to the Key ID (not base64 encoded). Expected to be 16 characters long.

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_delete_licenses
(
    uint32            app_ctx,
    char             *KID
);

/**
  Delete's a license from the store

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]    app_ctx      Application context
  @param[in]    pKID         Pointer to the Key ID (base64 encoded).
  @param[in]    cKIDLegth    Length of KID. Expected to be 24 wide characters long, or 48 bytes long.
  @param[out]   pLicDeleted  Contains the number of licenses deleted.

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_storemgmt_deletelicenses
(
    uint32            app_ctx,
    uint8            *pKID,
    uint32            cKIDLength,
    uint32           *pLicDeleted
);

/**
  Set DRM Header.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]      app_ctx          Application context
  @param[in]     *pbHeader         Pointer to DRM Header
  @param[in]      cbHeader         Size of DRM Header.

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_set_header
(
    uint32            app_ctx,
    uint8            *pbHeader,
    uint32            cbHeader
);

/**
  Gets the license state for the protected file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]      app_ctx                Application context
  @param[in]     *licenseStateData       Pointer to the license state information

  @dependencies
  Need to have called playready_set_header() before calling this function.

  @sideeffects
  None
*/
long playready_get_license_rights
(
    uint32                         app_ctx,
    tzlicensestatedata_t         * licenseStateData
);

/**
  Binds to a valid license from the data store to render the content.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context
  @param[in]     **strRights        Array of pointers to the rights array.
  @param[in]       cRights          Number of elements in rights array.
  @param[in]       dec_ctx          Decrypt context

  @dependencies
  Need to have called playready_set_header() before calling this function.

  @sideeffects
  None
*/
long playready_reader_bind
(
    uint32             app_ctx,
    tzconststring_t  **strRights,
    uint32             cRights,
    uint32            *dec_ctx
);

/**
  Commits all state data to the data store.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context

  @dependencies
  Need to have called playready_reader_bind() before calling this function.

  @sideeffects
  None
*/
long playready_reader_commit
(
    uint32             app_ctx
);

/**
  Returns the synchronization list from PC.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]      app_ctx              Application context
  @param[in]      cMaxCount            Maximum counts remaining
  @param[in]      cMaxHoursSize        Maximum hours remaining
  @param[in]      iStart               Index of the first item to process in the sync list
  @param[in]      cItemsToProcess      Total number of items to process in the sync list
  @param[out]    *iNextStart           Pointer to the next item in the sync list
  @param[out]    *cProcessed           Contains the number of items processed
  @param[out]    *pbChallenge          Contains the resulting synchronization challenge data
  @param[in,out] *cbChallenge          Contains the size of the resulting synchronization challenge data

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_sync_generate_challenge
(
    uint32         app_ctx,
    uint32         cMaxCount,
    uint32         cMaxHours,
    uint32         iStart,
    uint32         cItemsToProcess,
    uint32        *iNextStart,
    uint32        *cProcessed,
    uint8         *pbChallenge,
    uint32        *cbChallenge
);

/**
  Gets the metering data for a specific metering ID.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]      app_ctx            Application context
  @param[in]     *pbMeterCert        Pointer to the metering certificate
  @param[in]      cbMeterCert        Size of the metering certificate
  @param[out]    *pbChallenge        Contains the resulting metering challenge
  @param[in,out] *cbChallenge        Contains the size of the metering challenge

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_metering_generate_challenge
(
    uint32          app_ctx,
    uint8          *pbMeterCert,
    uint32          cbMeterCert,
    uint8          *pbChallenge,
    uint32         *cbChallenge
);

/**
  Sets the metering response on the device.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]    app_ctx             Application context
  @param[in]   *pbResponse          Pointer to the metering response
  @param[in]    cbResponse          Size of the metering response
  @param[out]  *fFlagsOut           Pointer to the MeterResponseFlags

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
unsigned long playready_metering_process_response
(
    uint32          app_ctx,
    uint8          *pbResponse,
    uint32          cbResponse,
    uint32         *fFlagsOut
);

/**
  Cleans the device data store.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context
  @param[in]       flags            Flags to indicate what type of clean operations to perform.  Valid values are:
                                       - DRM_STORE_CLEANUP_ALL
                                       - DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES
                                       - DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES


  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
unsigned long playready_cleanup_data_store
(
    uint32         app_ctx,
    uint32         flags
);

/**
  Resets PlayReady DRM

  @return

  @param[in]       flags            Flags to indicate what type of reset operations to perform.  Valid values are:

  @dependencies

  @sideeffects
  None
*/
unsigned long playready_reset
(
    uint32         flags
);

/**
  Gets the license state of the media content.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]    app_ctx           Application context
  @param[in]   *pwszString        Pointer to the license state query data
  @param[in]    cchString         Size of the license state query data
  @param[out]  *lsCategory        Contains the number of play count/hours remaining

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
unsigned long playready_get_license_state
(
    uint32                       app_ctx,
    const unsigned short        *pwszString,
    unsigned long                cchString,
    uint32                      *lsCategory
);

/**
  Gets the device certificate stored in SFS

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[out]    *pbDevCert           Pointer to the device certificate
  @param[in,out] *cbDevCert           Size of the device certificate

  @dependencies
  None

  @sideeffects
  None
*/
long playready_get_device_certificate
(
    uint8           *pbDevCert,
    uint32          *cbDevCert
);

/**
  Gets the status of the command sent for processing.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]    app_ctx             Application context
  @param[in]    opcode              Operation code for the command to process
  @param[in]    requestarg1         Request argument
  @param[in]    requestarg2         Request argument
  @param[in]    requestarg3         Request argument
  @param[in]    requestarg4         Request argument
  @param[in]   *requestdata         Pointer to the requestdata
  @param[in]    requestdatalen      Size of the requestdata
  @param[out]  *response1           Contains the response parameter
  @param[out]  *response2           Contains the response parameter
  @param[out]  *response3           Contains the response parameter
  @param[out]  *response4           Contains the response parameter

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_process_command
(
    uint32         app_ctx,
    uint32         opcode,
    uint32         requestarg1,
    uint32         requestarg2,
    uint32         requestarg3,
    uint32         requestarg4,
    uint8         *requestdata,
    uint32         requestdatalen,
    uint32        *response1,
    uint32        *response2,
    uint32        *response3,
    uint32        *response4
);

/**
  Gets the status of the data request being sent.

  @return
  DRM_E_NOTIMPL - This function is not implemented.

  @param[in]    app_ctx             Application context
  @param[in]    opcode              Operation code for the command to process
  @param[in]    requestarg1         Request argument
  @param[in]    requestarg2         Request argument
  @param[in]    requestarg3         Request argument
  @param[in]    requestarg4         Request argument
  @param[out]  *responsedata        Pointer to the responsedata
  @param[out]  *responsedatalen     Size of the responsedata
  @param[out]  *response1           Contains the response parameter
  @param[out]  *response2           Contains the response parameter
  @param[out]  *response3           Contains the response parameter
  @param[out]  *response4           Contains the response parameter

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  Not implemented in Playready Porting Kit.
*/
long playready_process_request
(
    uint32         app_ctx,
    uint32         opcode,
    uint32         requestarg1,
    uint32         requestarg2,
    uint32         requestarg3,
    uint32         requestarg4,
    uint8         *responsedata,
    uint32        *responsedatalen,
    uint32        *response1,
    uint32        *response2,
    uint32        *response3,
    uint32        *response4
);

/**
  Opens the enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]   app_ctx             Application context
  @param[in]  *filename            Pointer to the filename
  @param[out] *env_ctx             Pointer to the Envelope context

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_open
(
    uint32           app_ctx,
    unsigned short  *filename,
    uint32          *env_ctx
);

/**
  Closes an enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]   env_ctx             Envelope context

  @dependencies
  Need to have called playready_envelope_open() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_close
(
    uint32          env_ctx
);

/**
  Associates the decrypt context with the enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]   env_ctx             Envelope context
  @param[in]   dec_ctx             Decrypt context

  @dependencies
  Need to have called playready_envelope_open() and playready_reader_bind() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_initialize_read
(
    uint32       env_ctx,
    uint32       dec_ctx
);

/**
  Reads clear data from the enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     env_ctx             Envelope context
  @param[out]   *pbBuffer            Pointer to the buffer that receives the clear data
  @param[in]     cbToRead            Number of bytes to read
  @param[out]   *pcbBytesRead        Pointer to the number of bytes read

  @dependencies
  Need to have called playready_envelope_initialize_read() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_read
(
    uint32           env_ctx,
    uint8           *pbBuffer,
    uint32           cbToRead,
    uint32          *pcbBytesRead
);

/**
  Seeks within an enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     env_ctx             Envelope context
  @param[in]     lDistanceToMove     Distance by which to move the file pointer
  @param[in]     dwMoveMethod        Reference point by which to move the file pointer
  @param[out]   *pdwNewFilePointer   Pointer to the new position in the file

  @dependencies
  Need to have called playready_envelope_initialize_read() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_seek
(
    uint32           env_ctx,
    long             lDistanceToMove,
    uint32           dwMoveMethod,
    uint32          *pdwNewFilePointer
);

/**
  Duplicates an envelope file handle.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     env_ctx             Envelope context
  @param[in]    *pwszFilename        Filename to re-open
  @param[out]    env_ctx_new         Pointer to the duplicate envelope context

  @dependencies
  Need to have called playready_envelope_open() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_duplicate_filecontext
(
    uint32                  env_ctx,
    const unsigned short   *pwszFilename,
    uint32                 *env_ctx_new
);

/**
  Gets the original filename for the enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       env_ctx                 Envelope context
  @param[out]     *pwszOriginalFilename    Filename to re-open
  @param[in,out]  *pcchOriginalFilename    Size of the filename

  @dependencies
  Need to have called playready_envelope_open() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_get_originalfilename
(
    uint32                  env_ctx,
    unsigned short         *pwszOriginalFilename,
    uint32                 *pcchOriginalFilename
);

/**
  Gets the size of the enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       env_ctx                 Envelope context
  @param[out]     *pcbFileSize             Size of the enveloped file

  @dependencies
  Need to have called playready_envelope_open() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_get_size
(
    uint32                  env_ctx,
    uint32                 *pcbFileSize
);

/**
  Updates the embedded store of the Playready object with the license from the license store.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]   app_ctx             Application context

  @dependencies
  Need to have called playready_licacq_process_response_v2() before calling this function.

  @sideeffects
  None
*/
long playready_update_embeddedstore
(
    uint32               app_ctx
);

/**
  Writes the Playready object to the enveloped file.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx              Application context
  @param[in]      *pwszFilename         Pointer to the filename where the object is to be written
  @param[in,out]  *env_ctx              Pointer to the envelope context

  @dependencies
  Need to have called playready_get_property() before calling this function.

  @sideeffects
  None
*/
long playready_envelope_write_playreadyobject
(
    uint32                  app_ctx,
    unsigned short         *pwszFilename,
    uint32                 *env_ctx
);

/**
  Commits the transactions to the embedded store.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]   app_ctx             Application context

  @dependencies
  Need to have called playready_envelope_write_playreadyobject() before calling this function.

  @sideeffects
  None
*/
long playready_update_embeddedstore_commit
(
    uint32               app_ctx
);

/**
  Get DRM header property.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]          app_ctx                Application context
  @param[in]          eproperty              Header property to retreive
  @param[out]        *pbPropertyData         Pointer to DRM Property Data.
  @param[in,out]     *pcbPropertyData        Size of DRM Property Data.

  @dependencies
  Need to have called playready_set_header() before calling this function.

  @sideeffects
  None
*/
long playready_get_property
(
    uint32               app_ctx,
    tzdrmgetproptype     eproperty,
    uint8               *pbPropertyData,
    uint32              *pcbPropertyData
);

/**
  Set Domain ID.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     *pServiceID       Pointer to the service ID
  @param[in]      pcServiceID      Size of the service ID
  @param[in]     *pAccountID       Pointer to the account ID
  @param[in]      pcAccountID      Size of the account ID
  @param[in]      pRevision        Pointer to the revision ID
  @param[in]     *pDomainID        Pointer to the domain ID

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_set_domainid
(
    uint8                 *pServiceID,
    uint32                 pcServiceID,
    uint8                 *pAccountID,
    uint32                 pcAccountID,
    uint32                 pRevision,
    tzdrmdomainid_t       *pDomainID
);

/**
  Gets the join domain challenge from the DRM Agent.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context
  @param[in]       dwFlags          Flag to indicate the type of custom data
  @param[in]      *pDomainID        Pointer to the domain ID
  @param[in]      *pchFriendlyName  Pointer to the friendlyname
  @param[in]       cchFriendlyName  Size of the friendlyname
  @param[in]      *pchData          Pointer to the custom data
  @param[in]       cchData          Size of the custom data
  @param[out]     *pbChallenge      Contains the resulting join domain challenge
  @param[in,out]  *pcbChallenge     Contains the size of the resulting join domain challenge

  @dependencies
  Need to have called playready_set_domainid() before calling this function.

  @sideeffects
  None
*/
long playready_joindomain_generate_challenge
(
    uint32                 app_ctx,
    uint32                 dwFlags,
    tzdrmdomainid_t       *pDomainID,
    uint8                 *pchFriendlyName,
    uint32                 cchFriendlyName,
    uint8                 *pchData,
    uint32                 cchData,
    uint8                 *pbChallenge,
    uint32                *pcbChallenge
);

/**
  Sets the join domain response on the device.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     app_ctx                 Application context
  @param[in]    *pbResponse              Pointer to join domain response
  @param[in]     cbResponse              Size of join domain response
  @param[out]   *pResult                 Pointer to the status code
  @param[out]   *pDomainID               Pointer to the domain ID

  @dependencies
  Need to have called playready_joindomain_generate_challenge() before calling this function.

  @sideeffects
  None
*/
long playready_joindomain_process_response
(
    uint32                app_ctx,
    uint8                *pbResponse,
    uint32                cbResponse,
    long                 *pResult,
    tzdrmdomainid_t      *poDomainID
);

/**
  Gets the leave domain challenge from the DRM Agent.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context
  @param[in]       dwFlags          Flag to indicate the type of custom data
  @param[in]      *pDomainID        Pointer to the domain ID
  @param[in]      *pchData          Pointer to the custom data
  @param[in]       cchData          Size of the custom data
  @param[out]     *pbChallenge      Contains the resulting leave domain challenge
  @param[in,out]  *pcbChallenge     Contains the size of the resulting leave domain challenge

  @dependencies
  Need to have called playready_set_domainid() before calling this function.

  @sideeffects
  None
*/
long playready_leavedomain_generate_challenge
(
    uint32                 app_ctx,
    uint32                 dwFlags,
    tzdrmdomainid_t       *pDomainID,
    uint8                 *pchData,
    uint32                 cchData,
    uint8                 *pbChallenge,
    uint32                *pcbChallenge
);

/**
  Sets the leave domain response on the device.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     app_ctx                 Application context
  @param[in]    *pbResponse              Pointer to leave domain response
  @param[in]     cbResponse              Size of leave domain response
  @param[out]   *pResult                 Pointer to the status code

  @dependencies
  Need to have called playready_leavedomain_generate_challenge() before calling this function.

  @sideeffects
  None
*/
long playready_leavedomain_process_response
(
    uint32                app_ctx,
    uint8                *pbResponse,
    uint32                cbResponse,
    long                 *pResult
);

/**
  Finds the domain certificate from the domain store.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context
  @param[in]      *pDomainID        Pointer to the domain ID
  @param[out]     *pbDomainCert     Contains the resulting domain certificate
  @param[in,out]  *pcbDomainCert    Contains the size of the resulting domain certificate

  @dependencies
  Need to have called playready_set_domainid() before calling this function.

  @sideeffects
  None
*/
long playready_domaincert_find
(
    uint32                  app_ctx,
    tzdrmdomainid_t        *poDomainID,
    uint8                  *pbDomainCert,
    uint32                 *pcbDomainCert
);

/**
  Initializes the domain certificate enumeration context.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx          Application context
  @param[out]      dom_ctx          Pointer to the Domain context

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_domaincert_initenum
(
    uint32          app_ctx,
    uint32         *dom_ctx
);

/**
  Retrieves the next element in a domain certificate enumeration context.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]         dom_ctx           Domain context
  @param[out]       *pcchDomainCert    Contains the size of the enumerated domain certificate
  @param[out]       *poDomainCertInfo  Pointer to the domain certificate info

  @dependencies
  Need to have called playready_domaincert_initenum() before calling this function.

  @sideeffects
  None
*/
long playready_domaincert_enumnext
(
    uint32                   dom_ctx,
    uint32                  *pcchDomainCert,
    tzdrmdomaincertinfo_t   *poDomainCertInfo
);

/**
  Encrypts the plain text block.

  @return
  DRM_SUCCESS - Success
  DRM_S_FALSE - Failure

  @param[in]       dec_ctx                   Decrypt context
  @param[in]       pbData                    Pointer to the plain text block
  @param[in]       cbData                    Size of the plain text block.
  @param[in]       streamType                Type of stream

  @dependencies
  None.

  @sideeffects
  None
*/
long playready_encrypt
(
    uint32             dec_ctx,
    uint8             *pbData,
    uint32             cbData,
    tz_pr_stream_type  streamType
);

/**
  Decrypt PlayReady DRM Audio Stream.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       dec_ctx                     Decrypt context
  @param[in]      *encryptionMetaData          IV, Offset, Block , Byte for decryption.
  @param[in]       numEncryptedUnits           How many entries in the encryption Metadata.
  @param[in,out]   data_ptr                    Pointer to encrypted buffer.
  @param[in]       data_len                    Contains the length of the encrypted buffer

  @dependencies
  Need to have called playready_set_header() before calling this function.

  @sideeffects
  None
*/
long playready_reader_decrypt_audio
(
    uint32                          dec_ctx,
    tzEncryptionMetaData_t         *encryptionMetaData,
    uint8                           numEncryptedUnits,
    uint8                          *data_ptr,
    uint32                          data_len
);

/**
  Decrypt PlayReady DRM video stream with content protection.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]        dec_ctx                    Decrypt context
  @param[in]       *encryptionMetaData         IV, Offset, Block , Byte for decryption.
  @param[in]        numEncryptedUnits          How many entries in the encryption Metadata.
  @param[in]       *in_phy_addr                Pointer to the input buffer
  @param[in]        in_len                     Length of the input buffer
  @param[in,out]    out_phy_addr               Physical address of the output buffer

  @dependencies
  Need to have called playready_set_header() before calling this function.

  @sideeffects
  None
*/
long playready_reader_decrypt
(
    uint32                          dec_ctx,
    tzEncryptionMetaData_t         *encryptionMetaData,
    uint8                           numEncryptedUnits,
    uint8                          *in_phy_addr,
    uint32                          in_len,
    tz_mem_array_s_t               *out_phy_addr
);

/**
  Initialize the state data for PlayReady cocktail DRM Stream Decryption.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       dec_ctx                     Decrypt context
  @param[in]       data_ptr                    Pointer to the last 15 bytes of the encrypted payload.
  @param[in]       data_len                    length of the encrypted payload.

  @dependencies
  Need to have called playready_set_header() before calling this function.
  Need to call this function before calling playready_reader_decrypt()/playready_reader_decrypt_audio() for cocktail decryption

  @sideeffects
  None
*/
long playready_reader_initdecrypt
(
    uint32                          dec_ctx,
    uint8                          *data_ptr,
    uint32                          data_len
);

/**
  Removes the header information stored in Application context structure.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]       app_ctx                     Application context

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long playready_reinitialize
(
    uint32            app_ctx,
    uint32            dec_ctx,
    uint32            dom_ctx
);

/**
  Retrieves the data element from the server response.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]     app_ctx_id              Application context
  @param[in]    *pbResponse              Pointer to server response.
  @param[in]     cbResponse              Size of server response.
  @param[in]     dwDataType              Data element type.
  @param[out]  *pchDataString          Pointer to the data string
  @param[in,out]  *pcchDataString          Size of the data string

  @dependencies
  Need to have called playready_licacq_generate_challenge() before calling this function.

  @sideeffects
  None
*/
long playready_getadditional_responsedata
(
    uint32               app_ctx_id,
    uint8               *pbResponse,
    uint32               cbResponse,
    uint32               dwDataType,
    uint8               *pchDataString,
    uint32              *pcchDataString
);

/**
  Re-provision the keys

  @return
  Success or Failure

  @param[in]     key             key file to store the data in
  @param[in]	  *msg_data   Pointer to file data
  @param[in]	   msg_len      Size of msg_data.

  @dependencies
  None

  @sideeffects
  None
*/
long playready_reprov_keys
(
    tz_pr_keys_type key,
    uint8         * msg_data,
    uint32          msg_len,
    uint8         * prt_path
);

/**
  Enables or disables parsing the non-silent URL.

  @return
  E_SUCCESS   - Success.
  E_FAILED    - Failure.

  @param[in]    bParsenonsilenturl - 0/1- off/on the parsing.

  @dependencies
  To be used only for envelope testing using the demo apk

  @sideeffects
  None
*/
long playready_nonsilenturl_parsing_toggle
(
    uint8             bParsenonsilenturl
);

/**
  Set robustness version in Porting kit to the value passed
  as argument.

  @return
  E_SUCCESS   - Success.
  E_FAILED    - Failure.

  @param[in]    robustness version

  @dependencies
  To be used only with internal testing

  @sideeffects
  None
*/
long playready_set_robustness_version
(
    uint32            robustness_version
);

/**
  Save the AES key and keyfile.

  @return
  Success or Failure

  @param[in]       key               Key file to store the data in
  @param[in]      *msg_data          Pointer to msg data
  @param[in]       msg_len           Size of msg data.
  @param[out]     *prt_path          Path to the created file

  @dependencies
  None

  @sideeffects
  None
*/
long playready_save_aes_keyfile
(
    tz_pr_aes_keyfile_type key,
    uint8 *                msg_data,
    uint32                 msg_len,
    uint8 *                prt_path
);

/**
  Set Playready Content Property.

  @return
  Refer to Microsoft Playready Porting Kit for the error codes.

  @param[in]      app_ctx_id         DRM_APP_CONTEXT structure with ID
  @param[in]      set_property_type  DRM_CONTENT_SET_PROPERTY type, the property to set
  @param[in]    * data               Property specific data
  @param[in]      size               Size of data

  @dependencies
  Need to have called playready_initialize() before calling this function.

  @sideeffects
  None
*/
long Playready_Content_SetProperty
(
    uint32        app_ctx_id,
    uint32        set_property_type,
    uint8 *       data,
    uint32        size
);

/**
  Enables or disables the IV constraint check.

  @return
  E_SUCCESS   - Success.
  E_FAILED    - Failure.

  @param[in]    bIVconstraint - 0/1- off/on the IV constraint check.

  @dependencies
  To be used only for internal playready unit testing.

  @sideeffects
  None
*/
long playready_IVconstraintcheck_toogle
(
    uint8             bIVconstraint
);

#endif //PLAYREADY_H
