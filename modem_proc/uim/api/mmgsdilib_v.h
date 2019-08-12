#ifndef MMGSDILIB_V_H
#define MMGSDILIB_V_H
/*===========================================================================


            M M G S D I   L I B R A R Y   D E F I N I T I O N S

                      A N D   F U N C T I O N S


  This file contains Library function that MMGSDI client can call to access
  various SIM/USIM/RUIM functions

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2008 - 2013 QUALCOMM Technologies, Inc (QTI) and its licensors.  
All Rights Reserved.QUALCOMM Technologies Proprietary.  
Export of this technology or software is regulated by the
U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/mmgsdilib_v.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/11/13   vv      Re-design handling of VPE response from the card
09/05/12   av      Handling of VPE response from the card
12/21/11   shr     Removed legacy GSDI interfaces
11/15/11   nmb     RPM Updates
05/17/11   shr     Updates for Thread safety
11/02/10   vs      Update to process NVRUIM cmds via MMGSDI high priority q
01/12/10   nb      Update for Rel 7 Enhanced Network Selection
12/10/09   nb      Compilation Fix
11/10/09   nb      Moved mmgsdi_onchip_sim_state from mmgsdilib.h
11/17/08   nb      Initial Revision

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "uim.h"
#include "uim_v.h"
#include "mmgsdilib.h"


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_READ_ENTIRE_FILE_SIZE

   DESCRIPTION: The MAX File Size is currently set to 512 Bytes.  Thus, when a
                Client requests the maximum number of bytes to be read, it is
                an indication that the data to be returned is capped at 512.
  -----------------------------------------------------------------------------*/
#define MMGSDI_READ_ENTIRE_FILE_SIZE     0

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_DF_TAG_SIZE_IN_BYTES

   DESCRIPTION: Indicates the size of the DF Tag used when parsing / packing
                the PATH provided into a TLV.
  -----------------------------------------------------------------------------*/
#define MMGSDI_DF_TAG_SIZE_IN_BYTES        1

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_DF_TAG_LEN_SIZE_IN_BYTES

   DESCRIPTION: Indicates the size of the DF Tag Len used when parsing / packing
                the PATH provided into a TLV.  This Len will indicate how many
                bytes are to follow for the "Value" (i.e. PATH Value).
  -----------------------------------------------------------------------------*/
#define MMGSDI_DF_TAG_LEN_SIZE_IN_BYTES    1

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE

   DESCRIPTION: Defines the maximum value allowable to be used for configuring
                the ONCHIP SIM CACHE.
  ---------------------------------------------------------------------------*/
#define MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE 512

/* -------------------------------------------------------------
   DEFINE:      MMGSDI_MFLO_SUBSCRIBER_ID_LEN

   DESCRIPTION: Length of the MediaFLO subscriber ID.  The len is
                specified in the "Qualcomm MediaFLO Authentication
                Manager Applet Functional Specification"
  ----------------------------------------------------------------*/
#define MMGSDI_MFLO_SUBSCRIBER_ID_LEN     0x08

/* -------------------------------------------------------------
   DEFINE:      MMGSDI_MFLO_PUBLIC_KEY_LEN

   DESCRIPTION: Length of the MediaFLO public key.  The len is
                specified in the "Qualcomm MediaFLO Authentication
                Manager Applet Functional Specification"
----------------------------------------------------------------*/
#define MMGSDI_MFLO_PUBLIC_KEY_LEN     0x83

/* -------------------------------------------------------------
   DEFINE:      MMGSDI_MFLO_ACT_PARAMS_LEN

   DESCRIPTION: Length of the MediaFLO activation parameters for
                the initialization procedure.  The len is
                specified in the "Qualcomm MediaFLO Authentication
                Manager Applet Functional Specification"
  ----------------------------------------------------------------*/
#define MMGSDI_MFLO_ACT_PARAMS_LEN     0x88

/*----------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_SEARCH_RECORD_NUMBER

  DESCRIPTION: Defined the maximum Record number.
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_SEARCH_RECORD_NUMBER  0xFE

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_ENHANCED_SEARCH_RECORD_OFFSET

  DESCRIPTION: Defined the maximum offset of a record.
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_ENHANCED_SEARCH_RECORD_OFFSET  0xFE

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_ENHANCED_SEARCH_CHARACTER_VALUE

  DESCRIPTION: Defind the maximum character value .
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_ENHANCED_SEARCH_CHARACTER_VALUE  0xFF

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_MAX_SEEK_PATTERN_LENGTH

  DESCRIPTION: Defined maximum pattern length
  -----------------------------------------------------------------------*/
#define MMGSDI_MAX_SEEK_PATTERN_LENGTH  0x10

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_CLIENT_ID_ZERO

  DESCRIPTION: Identifies an Invalid Zero value Client ID
  -----------------------------------------------------------------------*/
#define MMGSDI_CLIENT_ID_ZERO 0x00

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_SESSION_ID_ZERO

  DESCRIPTION: Identifies an Invalid Zero value Session ID
  -----------------------------------------------------------------------*/
#define MMGSDI_SESSION_ID_ZERO 0x0

/* ------------------------------------------------------------------------
  DEFINE:      MMGSDI_INVALID_SESSION_ID

  DESCRIPTION: Identifies an Invalid Session ID
  -----------------------------------------------------------------------*/
#define MMGSDI_INVALID_SESSION_ID 0x00000000FFFFFFFF

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_EHPLMNPI_LEN

   DESCRIPTION: Length of EF Equivalent HPLMN Presentation Indicator
  ---------------------------------------------------------------------------*/
#define MMGSDI_EHPLMNPI_LEN      0x01

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_LRPLMNSI_LEN

   DESCRIPTION: Length of EF Last RPLMN Selection Indicator
  ---------------------------------------------------------------------------*/
#define MMGSDI_LRPLMNSI_LEN      0x01

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_NVRUIM_CLIENT_CMD

   DESCRIPTION: Identifier to determine commands originated by NVRUIM
  ---------------------------------------------------------------------------*/
#define MMGSDI_NVRUIM_CLIENT_CMD 0x11111110

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/
/*===========================================================================
   ENUM:      MMGSDI_TAG_ENUM_TYPE

   DESCRIPTION:
     Enums for Tag value in the MMGSDI TLV structure
     Enumerations for the Directory File (DF) Path provided by the client.
     This will be used to build the TLV for the Path provided by the client.

     NOTE:  This is required to allow the client to pass the information
     with an embedded Application Directory File (ADF)
=============================================================================*/
typedef enum {
  MMGSDI_MF_TAG       = 0x00000000,
  MMGSDI_DF1_TAG,
  MMGSDI_DF2_TAG,
  MMGSDO_DF3_TAG,
  MMGSDI_EF_TAG,
  MMGSDI_ADF_TAG,
  MMGSDI_MAX_TAG_ENUM = 0x7FFFFFFF
}mmgsdi_tag_enum_type;

/*=============================================================================
   ENUM:         MMGSDI_LOCATION_STATUS_ENUM_TYPE

   DESCRIPTION:
     Location status information
=============================================================================*/
typedef enum
{
  /*  values based on standard */
  MMGSDI_LOC_STATUS_NORMAL_SERVICE    = 0x00, /**< UE is in normal service. */
  MMGSDI_LOC_STATUS_LIMITED_SERVICE   = 0x01, /**< UE is in limited service. */
  MMGSDI_LOC_STATUS_NO_SERVICE        = 0x02  /**< UE is in no service. */
} mmgsdi_location_status_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PTR_TYPE

   DESCRIPTION:    The generic pointer type.
  ---------------------------------------------------------------------------*/
typedef uint32 mmgsdi_ptr_type;

/*=============================================================================

                     FUNCTION DECLARATIONS

=============================================================================*/
/* ============================================================================
   FUNCTION       MMGSDI_GET_ALL_AVAILABLE_APPS_SYNC

   DESCRIPTION
     This function will build a request to the MMGSDI Task for the information
     of all available applications from the UICC/ICC Card(s).
     The client calling this function expects an instant response.

     This function uses critical sections to protect accesses to
     MMGSDI global data. Hence, it is advisable to pet the dog
     before invoking the function.

   PARAMETERS:
     client_id:         Client ID of the requesting client.
     available_apps:    Buffer to store the information of all available
                        applications on the UICC/ICC Card(s).

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_reg().

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_INIT:         Called before CARD_INSERTED on either slot

   SIDE EFFECTS:

===============================================================================*/
mmgsdi_return_enum_type mmgsdi_get_all_available_apps_sync (
  mmgsdi_client_id_type              client_id,
  mmgsdi_available_apps_type       * available_apps
);


/*================================================================
FUNCTION  MMGSDI_RPM_HAS_ICCID_CHANGED

DESCRIPTION:
  This function is called determine if the ICCID has changed since
  the previous card inserted

  INPUT PARAMETERS:
    mmgsdi_slot: Slot to check
    status_ptr : out param for whether the ICCID has changed or not


DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          status is:
                             TRUE : the ICCID has changed
                             FALSE: the ICCID is the same
                                    as the last card inserted
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    FEATURE_MMGSDI_RPM_SUPPORT is
                             not defined

SIDE EFFECTS:
  status is updated
===============================================================*/
mmgsdi_return_enum_type mmgsdi_rpm_has_iccid_changed (
  mmgsdi_slot_id_enum_type            mmgsdi_slot,
  boolean                            *status_ptr);

/*================================================================
FUNCTION  MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED

DESCRIPTION:
  This function is called by gstk to indicate that a response
  to VPE has been received from the card with status 90 xx.
  If card has responded to VPE with an IMSI switch, an internal
  refresh will be triggered by MMGSDI if MMGSDI has a stale value
  of IMSI in its cache.
  Caller should not expect any response from the API as such.

  INPUT PARAMETERS:
    slot_id:          Slot where the card is question exists
    location_status:  Location status
    mcc:              MCC
    mnc:              MNC
    response_cb_ptr:  Call back function when the command execution is done.
    client_ref     :  User Data returned upon completion of this cmd.


DEPENDENCIES:
  None

LIMITATIONS:
  The request will go through ONLY IF this is the first time GSTK
  called this API since card inserted, that is, if the API is
  called by GSTK in response to the FIRST VPE response (90 xx)
  from the card.

RETURN VALUE:
  None

SIDE EFFECTS:
  MMGSDI's global count for the VPEs is incremented everytime the
  function is called.
  NAA_INIT Refresh is triggered if IMSI change is reported by the
  card.
===============================================================*/
void mmgsdi_gstk_loc_env_rsp_received (
  mmgsdi_slot_id_enum_type            slot_id,
  mmgsdi_location_status_enum_type    loc_status,
  uint16                              mcc,
  uint16                              mnc,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

#endif /* MMGSDILIB_V_H */

