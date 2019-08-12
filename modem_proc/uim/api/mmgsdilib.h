#ifndef MMGSDILIB_H
#define MMGSDILIB_H
/*===========================================================================


            M M G S D I   L I B R A R Y   D E F I N I T I O N S

                      A N D   F U N C T I O N S
=========================================================================== */
/**
@file mmgsdilib.h
@brief This library provides an MMGSDI client with function calls to access
SIM/USIM/RUIM functions.

The MMGSDI interface allows clients to request services, receive events, and use
information from a Universal Integrated Circuit Card (UICC) outside of a smart card application.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The mmgsdi_mainpage.dox file contains all file/group descriptions that are
      in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the mmgsdi_mainpage.dox file or
      contact Tech Pubs.

      The above description for this file is part of the "library" group
      description in the mmgsdi_mainpage.dox file.
===========================================================================*/

/*
  mmgsdi_read_transparent
    Read data from a Transparent file

  mmgsdi_read_record
    Read data from a record in a Linear Fixed or Cyclic file

  mmgsdi_write_transparent
    Write data into a Transparent file

  mmgsdi_write_record
    Write data into a record Linear Fixed or Cyclic file

  mmgsdi_get_file_attr
    Get File Attribute information for the files

  mmgsdi_cmd
    Getting command pointer and copy the data and put into the MMGSDI command
    queue

  mmgsdi_search
    Search for a given pattern in a record file on uicc card.

  mmgsdi_seek
   Search for a given pattern in a record file on icc card.
===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.  All
Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/mmgsdilib.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
10/26/15   nr      Disable MMGSDI PB cache for PBM module
09/25/15   vr      Added support for ERA-GLONASS
03/25/15   stv     API to block card status events
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/10/14   vdc     Expose NV for CDMA select before reading ADN under TELECOM
09/30/14   kk      Enhancements to NV handling
09/30/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
08/27/14   yt      Support for restricted SIM access via MMGSDI session APIs
07/30/14   ar      Revert FR 20469 changes to align with NAS check-in dates
07/25/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
10/18/13   vv      Removed FEATURE_MMGSDI_OP_MODE_SLAVE
10/17/13   at      Support for MMGSDI Cancel APIs
10/08/13   ar      Support to select CDMA before TP by default
10/04/13   vv      Added API to return se13 table release date and source
10/04/13   vv      Added NV support for ENS sub-features
06/28/13   bcho    APIs added to return plmn info from SE13 table.
05/17/13   hn      Support for Alpha for Refresh
04/15/13   tl      Remove SAP init, dereg and pin verify functionality
02/22/13   yt      Support for slot 3
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
08/29/12   at      Support for new API mmgsdi_sap_connect_ext
08/10/12   av      Removed RPC support along with RPC style comments
02/27/12   shr     Added support for Send APDU extension API
11/23/11   shr     Removed legacy GSDI interfaces
06/15/11   shr     Added deprecation notice for Legacy APIs
05/05/11   shr     Add critical section protection for global accesses in
                   GSDI/MMGSDI APIs
09/23/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
01/25/10   vs      Updates for header decomposition
01/13/10   nb      Added prototype for mmgsdi_perso_get_indicators
01/12/10   nb      Update for Rel 7 Enhanced Network Selection
01/06/10   shr     Fixed Session Close handling
12/23/09   shr     Fixed Perso Lock Down handling
12/14/09   shr     Fixed Perso handling and APIs
12/14/09   ssr     Removed special CNF for provisioning
12/10/09   nb      Compilation Fix
12/08/09   shr     Read EF-DIR and cache app data before sending
                   TP request to GSTK
12/08/09   js      Added support for MMGSDI_CSIM_ARR file
12/04/09   mib     Added support for eCall
11/10/09   nb      Moved mmgsdi_onchip_sim_state to mmgsdilib_v.h
10/23/09   nb      CMI changes
10/21/09   js      Fixed issue for no card error evt for card power up
                   request under no sim condition
10/15/09   ssr     Add support for OTASP request
10/07/09   mib     Removed dependency between UIM table and mmgsdi enum
09/28/09   mib     Added enums for LTE files
09/11/09   vs      Added and updated CSIM EF enums
09/09/09   ssr     Added support for client id and event dereg
09/02/09   ssr     Added MMGSDI constant for some files
08/19/09   kk      Updated Meta comments for get app capabilities
08/13/09   kk      Added support for session based get app capabilities API
08/13/09   kk      Added support for session based GET CPHS INFO API
08/13/09   jk      Support for Illegal SIM, Get App Capabilities, OTASP APIs
07/22/09   nb      Moved file enum mapping routines from mmgsdiutil.h
07/10/09   jk      Added Service Table items
06/24/09   js      Update#2 for CSIM Session Management
06/15/09   js      CSIM Session Management Update
05/28/09   yz      New APIs for CSIM and Dual SIM Dual Standby
05/25/09   vs      Updates for CSIM and multi-mode UICC
04/15/09   xz      Fix issue of generating RPC code for modem restart
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/02/09   mib     Added original mode in mmgsdi_refresh_evt_info_type
01/21/09   kk      Added channel id to the session_open cnf data
12/09/08   nk      Fixed RPC Meta Comment for mmgsdi_client_id_reg
11/26/08   nb      Removed Featurization Flags for Common Modem Interface
10/29/08   tml     Modem restart client id dereg clean up
10/20/08   sun     Added new MMGSDI return type
10/06/08   js      Removed inclusion of tmc.h
08/15/08   jk      Added Generate Key / VPM API
08/26/08   js      Updated MMGSDI_MAX_PATH_LEN to 5 and made all uim commands
                   to accept path length of 4
07/31/08   SSR     Update search for blocking the search directions belonging
                   to Record Pointer.
07/25/08   tml     Added modem restart oncrpc support
07/21/08   tml     Added qw.h for dword support
07/17/08   kk      Added support for PKCS 15
06/23/08   ssr     Support of MMGSDI Search functionality
06/21/08   kk      Added PKCS15 Support
06/13/08   sun     Added support for OMA BCAST Srv bit
06/03/08   tml     Added sw1 and sw2 for access and status cnfs
04/30/08   kk      Added support for MMS EFs
03/28/08   sun     Added support for OMA Bcast
03/28/08   sk      Fixed warnings
03/20/08   nk      Added OMH EF Support
01/24/08   nk      Added change me esn flag to get jcdma card info
01/10/08   nk      Added Comment for mmgsdi_cave_esn_type
11/19/07   js      Radio Access Technology Balancing support
12/02/07   sun     Added meta comment
11/15/07   sun     Added support for NAA refresh
11/08/07   vs      Added OMH feature support
09/10/07   tml     Added MFLO support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
08/14/07   sun     Added new Error codes and MUK_DEL Context
07/02/07   sun     Fixed USIM Auth Contexts
06/01/07   sun     Fixed featurization for SAP
05/22/07   nk      Added support for ESN ME change and removed featurization
                   for uniform rpc support
05/22/07   nk      Removed featurization for rpc
05/17/07   wli     Added support for JCDMA RUIM
05/02/07   sun     Added support for Service Available and USIM Auth
04/06/07   sun     Identify the provisioning app
03/07/07   sun     Added support for Disable with Replacement
02/09/07   sk      Added support for 1000 phonebook entries.
12/19/06   tml     Add support for path transparent and record access
11/21/06   sun     Added Meta Comments for Universal Pin Event
10/25/06   tml     Added pin replacement inital support
09/11/06   jk      Correction to RPC Metacomments
08/25/06   jar     Added support for FEATURE_MMGSDI_CARD_INFO
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/16/06   jar     Added support for MMGSDI_TELECOM_ANRC and
                   MMGSDI_TELECOM_ANRC1.  Realigned enums to adjust groupings
                   10.
08/05/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
07/20/06   sun     Fixed featurization
06/29/06   tml     RPC support for new functions
06/12/06   pv      Add MMGSDI_UIM_INTERNALLY_RESET return type and
                   MMGSDI_SESSION_CLOSE_EVT event.
05/23/06   tml     BDN supports
05/03/06   sp      Update mmgsdi pin state
04/15/06   tml     Update #define to enum for onchip support
04/13/06   jar     Added ONCHIP SIM SUPPORT
04/05/06   tml     Add Card Removed support, Add NO_FILE_ENUM
03/21/06   tml     added cfis and missing service table support and lint
02/16/06   sp      Support for EF_EHPLMN
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.h
01/04/06   tml     Updated enum name for SAP and Card powerup and down
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/28/05   tml     PNN and OPL support
11/22/05   jk      Changes to support multiprocessor RPC
11/03/05   tml     Fixed header
11/03/05   sun     Added support for refresh
10/26/05   pv      Change MMGSDI_READ_ENTIRE_FILE_SIZE to 0.  A zero when sent
                   down to UIM would result in reading the rest of the file.
10/25/05   tml     Added ISIM File supports
10/20/05   sun     Added support for BT Deregisteration
09/29/05   tml     Client Deregistration support
08/30/05   jar     Added support for Sending Status commands
08/28/05   tml     Compilation fix
08/27/05   tml     Compilation fix
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Support API to power down and power up SIM/USIM Card
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
2/17/05    tml     Initial Revision


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "qw.h"
#include "mmgsdilib_common.h"


/** @addtogroup library
@{ */

/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/
/*---------------------------------------------------------------------------
  MMGSDI_ENS_SUB_FEATURES_STATUS_LSIT_TYPE (71568) NVITEM related define
---------------------------------------------------------------------------*/
#define MMGSDI_MAX_ITEMS_RESERVED                    8

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/
/*=============================================================================
   ENUM:      MMGSDI_SE13_TABLE_SOURCE
=============================================================================*/
/** @brief Contains information regarding the source of the SE13 table
*/                   
typedef enum {
  MMGSDI_SE13_HARDCODED_SOURCE  = 0,
  MMGSDI_SE13_EFS_SOURCE        = 1
} mmgsdi_se13_table_source;


/*=============================================================================
   ENUM:      MMGSDI_NV_CONTEXT_TYPE
=============================================================================*/
/** @brief Contains information regarding the nv context type
     MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC is used explicitly for Device or ME 
     specific NV/EFS Items
*/    
typedef enum {
  MMGSDI_NV_CONTEXT_PRIMARY = 0,
  MMGSDI_NV_CONTEXT_SECONDARY,
  MMGSDI_NV_CONTEXT_TERTIARY,
  MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC = 0x10, 
  MMGSDI_NV_CONTEXT_INVALID = 0x7f
} mmgsdi_nv_context_type;

/*=============================================================================

                       ABSTRACT DATA TYPE DECLARATIONS

=============================================================================
   STRUCTURE:      MMGSDI_AVAILABLE_APPS_TYPE
=============================================================================*/
/** @brief Stores information about all available applications on ICC/UICC
    cards.
*/
typedef struct {
  int32                 card1_num_app;  /**< Number of available applications
                                             on card 1. */
  int32                 card2_num_app;  /**< Number of available applications
                                             on card 2. */
  int32                 card3_num_app;  /**< Number of available applications 
                                             on card 3 */
  mmgsdi_app_info_type  card1_app_info[MMGSDI_MAX_APP_INFO];
    /**< Array that contains the information of all available applications
         on card 1. */
  mmgsdi_app_info_type  card2_app_info[MMGSDI_MAX_APP_INFO];
    /**< Array that contains the information of all available applications
         on card 2. */
  mmgsdi_app_info_type  card3_app_info[MMGSDI_MAX_APP_INFO];
    /**< Array that contains the information of all available applications
         on card 3. */
} mmgsdi_available_apps_type;

/*=============================================================================
   STRUCTURE:      MMGSDI_OPTION_TYPE
=============================================================================*/
/** @brief Contains optional parameter information.\n

    Optional parameters are defined separately for each function. This
    structure is used as a placeholder for these optional parameters.
*/
typedef struct {
  uint32             tlv_num;  /**< Number of Tag Length Values in the tlvs_ptrs
                                    member variable. */
  void              *tlvs_ptr; /**< Variable structure where Tag = size uint32
                                    and Length = size uint32. */
} mmgsdi_option_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_ALPHA_STRING_TYPE
-----------------------------------------------------------------------------*/
/**
  @brief String information for the commands.
*/
typedef struct {
  mmgsdi_alpha_encoding_enum_type dcs;        /**< GSM default character set (0x04).\n
                                                   UCS-2 formatting (0x08). */
  mmgsdi_data_type                alpha_text; /**< Alpha text. */
}
mmgsdi_alpha_string_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ENS_SUB_FEATURES_STATUS_LIST_TYPE

   DESCRIPTION:
     Data Structure to hold status of ENS sub features read from
     EFS based NV
-------------------------------------------------------------------------------*/
typedef struct {
  uint8  scan_control_application_support;
  uint8  nw_selection_menu_control_application_support;
  uint8  acting_hplmn_rescan_feature_support;
  uint8  rat_balancing_support;
  uint8  reserved_for_future_use[MMGSDI_MAX_ITEMS_RESERVED];
} mmgsdi_ens_sub_features_status_list_type;

/* ----------------------------------------------------------------------------
   ENUM:         MMGSDI_CACHED_NV_ITEM_ENUM
-----------------------------------------------------------------------------*/
/**
  @brief The nv values that MMGSDI supports
 */
typedef enum {
  MMGSDI_NV_UIM_SELECT_DEFAULT_USIM_APP_I,
  MMGSDI_NV_FTM_MODE_I,
  MMGSDI_NV_UIM_GSM_DCS_1800_I,
  MMGSDI_NV_GPRS_ANITE_GCF_I,
  MMGSDI_NV_ENS_ENABLED_I,
  MMGSDI_NV_SCAN_CONTROL_APPLICATION_SUPPORT_I,
  MMGSDI_NV_NETWORK_SELECTION_MENU_CONTROL_APPLICATION_SUPPORT_I,
  MMGSDI_NV_ACTING_HPLMN_RESCAN_FEATURE_SUPPORT_I,
  MMGSDI_NV_RAT_BALANCING_SUPPORT_I,
  MMGSDI_NV_CDMA_DF_SELECT_BEFORE_TP_I,
  MMGSDI_NV_UIM_JCDMA_MODE_I,
  MMGSDI_NV_PREF_MODE_NOT_FROM_RUIM_I,
  MMGSDI_NV_RUIM_ALLOW_ESN_FAIL_I,
  MMGSDI_NV_RUIM_ZERO_ESN_FAIL_I,
  MMGSDI_NV_UIM_RUIM_SUPPORT_SCI_I,
  MMGSDI_NV_UIM_AN_HRPD_FALLBACK_I,
  MMGSDI_NV_UIM_MISCONFIG_RUIM_N5_WORKAROUND_I,
  MMGSDI_NV_SUPPORT_EXTENDED_FPLMN_ICC_I,
  MMGSDI_NV_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ_I
} mmgsdi_cached_nv_item_enum_type;

/*----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_CACHED_NV_ITEM_DATA
-----------------------------------------------------------------------------*/
/**
  @brief Contains the information about the nv to be read and status of the 
         cached value
*/
typedef struct {
  mmgsdi_cached_nv_item_enum_type  nv_item_enum;
  union {
                                   /* NV_UIM_SELECT_DEFAULT_USIM_APP_I */
    boolean                        uim_select_default_usim_app;
                                   /* NV_FTM_MODE_I */
    uint8                          ftm_mode;
                                   /* UIM_GSM_DCS_1800 */
    boolean                        uim_gsm_dcs_1800;
                                   /* NV_GPRS_ANITE_GCF_I */
    boolean                        gprs_anite_gcf;
                                   /* NV_ENS_ENABLED_I */
    boolean                        ens_enabled;
                                   /* ENS_SCAN_CONTROL_APP_SUPPORT */
    boolean                        scan_control_application_support;
                                   /* ENS_NW_SELECTION_MENU_CONTROL_APP_SUPPORT */
    boolean                        nw_selection_menu_control_application_support;
                                   /* ENS_ACTING_HPLMN_RESCAN_FEATURE_SUPPORT */
    boolean                        acting_hplmn_rescan_feature_support;
                                   /* ENS_RAT_BALANCING_SUPPORT */
    boolean                        uicc_rat_balancing_support;
                                   /* NV_CDMA_DF_SELECT_BEFORE_TP_I */
    boolean                        cdma_df_select_before_tp;
                                   /* UIMDRV_JCDMA_MODE */
    boolean                        jcdma_mode;
                                   /* PREF_MODE_NOT_FROM_RUIM */
    boolean                        pref_mode_not_from_ruim;
                                   /* RUIM_ALLOW_ESN_FAIL */
    boolean                        ruim_allow_esn_fail;
                                   /* RUIM_ZERO_ESN_FAIL */
    boolean                        ruim_zero_esn_fail;
                                   /* UIM_RUIM_SUPPORT_SCI */
    boolean                        uim_ruim_support_sci;
                                   /* UIM_AN_HRPD_FALLBACK */
    boolean                        uim_an_hrpd_fallback;
                                   /* UIM_MISCONFIG_RUIM_N5_WORKAROUND */
    boolean                        uim_misconfig_ruim_n5_workaround;
                                   /* NV_SUPPORT_EXTENDED_FPLMN_ICC */
    boolean                        support_extended_fplmn_icc;
                                   /* NV_CDMA_DF_SELECT_BEFORE_TELECOM_ADN_I */
    boolean                        cdma_df_select_before_telecom_adn;
  } nv_item_data;
} mmgsdi_cached_nv_item_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SE13_TABLE_RELEASE_DATE_INFO
-----------------------------------------------------------------------------*/
/**
  @brief Stores the release date of the SE13 Table
*/
typedef struct{
  uint8   mmgsdi_se13_release_day;
  uint8   mmgsdi_se13_release_month;
  uint16  mmgsdi_se13_release_year;
} mmgsdi_se13_table_release_date_info;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SE13_TABLE_RELEASE_DATE_SOURCE_INFO
-----------------------------------------------------------------------------*/
/**
  @brief Stores a struct containing the release date of the SE13 table
  and an enum indicating the source of the table
*/
typedef struct{
  mmgsdi_se13_table_release_date_info  se13_date;
  mmgsdi_se13_table_source             se13_source;
} mmgsdi_se13_table_details_info;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_APDU_PARAMS_TYPE
-----------------------------------------------------------------------------*/
/**
  @brief Structure containing P1, P2, P3 parameters of a command APDU.
  params_valid should be set to FALSE if the parameters are not applicable.
*/
typedef struct{
  boolean   params_valid;
  uint8     p1;
  uint8     p2;
  uint8     p3;
} mmgsdi_apdu_params_type;

/** @} */ /* end_addtogroup library */
/** @addtogroup library
@{ */

/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/*=============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_REG
=========================================================================== */
/**
  Registers a client for a new event callback and allocates a client ID for any
  sessions owned by that client. The new event callback contains additional
  information based on specific events being sent.

  The response to this request returns the client ID to the calling task.

  @param[in] evt_cb_ptr       Pointer to the callback for event notifications.
                              If NULL is passed, no event callback is made.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  None.

  @sideeffects
  The callback is called every time MMGSDI is to notify any client of any
  events.
*/
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_reg (
  mmgsdi_evt_callback_type * evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);

/*=============================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_REG_EXT
=========================================================================== */
/**
  Registers a client for a new event callback and allocates a client ID for any
  sessions owned by that client. The way this differs from
  MMGSDI_CLIENT_ID_AND_EVT_REG is that here the caller can provide a name
  for itself that MMGSDI can use to refer to it, if needed.

  The response to this request returns the client ID to the calling task.

  @param[in] evt_cb_ptr       Pointer to the callback for event notifications.
                              If NULL is passed, no event callback is made.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.
  @param[in] client_name_ptr  Name of the client.
  @param[in] client_name_len  Length of the client name above.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  None.

  @sideeffects
  The callback is called every time MMGSDI is to notify any client of any
  events.
*/
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_reg_ext (
  mmgsdi_evt_callback_type   *evt_cb_ptr,
  mmgsdi_callback_type        response_cb_ptr,
  mmgsdi_client_data_type     client_ref,
  const char*                 client_name_ptr,
  uint8                       client_name_len
);

/*===========================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_DEREG
=========================================================================== */
/**
  Deregisters a client for event callbacks, and deallocates the client ID and
  any sessions owned by that client.

  @param[in] client_id        Client ID.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.\n
  The session must be closed if it is already opened by the client.

  @sideeffects
  The callback is called every time MMGSDI is to notify any client of any
  events.
*/
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_dereg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);

/* ============================================================================
   FUNCTION:      MMGSDI_GET_ALL_AVAILABLE_APPS
=========================================================================== */
/**
  Gets the information of all available applications on UICC/ICC cards.

  @param[in] client_id        Client ID of the requesting client.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_get_all_available_apps (
  mmgsdi_client_id_type              client_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_CONNECT
=========================================================================== */
/**
  Connects/opens a SAP Bluetooth<sup>&reg;</sup> connection.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] sap_mode         Client or Server mode.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_connect(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);


/* ============================================================================
  FUNCTION:  MMGSDI_SAP_CONNECT_EXT
============================================================================ */
/**
  Connects/opens a SAP Bluetooth<sup>&reg;</sup> connection.
 
  Provides an option for the client to specify a condition for the SAP connection
  request. SAP connection request with proceed based on this condition.

  @param[in] client_id          Client ID of the caller.
  @param[in] card_slot          Physical slot for multiple slot targets.
  @param[in] sap_mode           Client or Server mode.
  @param[in] condition          Condition to check for active call status.
  @param[in] response_cb_ptr    Pointer to the response callback.
  @param[in] client_ref         User data returned upon completion of this command.

  @return 
  MMGSDI_SUCCESS          -- Command structure was properly generated and 
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_connect_ext(
    mmgsdi_client_id_type                 client_id,
    mmgsdi_slot_id_enum_type              card_slot,
    mmgsdi_sap_mode_enum_type             sap_mode,
    mmgsdi_sap_conn_condition_enum_type   condition,
    mmgsdi_callback_type                  response_cb_ptr,
    mmgsdi_client_data_type               client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_DISCONNECT
=========================================================================== */
/**
  Disconnects/closes a SAP Bluetooth<sup>&reg;</sup> connection.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] sap_mode         Client or Server mode.
  @param[in] disconnect_mode  Graceful or Immediate disconnect mode.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_disconnect(
  mmgsdi_client_id_type               client_id,
  mmgsdi_slot_id_enum_type            card_slot,
  mmgsdi_sap_mode_enum_type           sap_mode,
  mmgsdi_disconnect_mode_enum_type    disconnect_mode,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_GET_ATR
=========================================================================== */
/**
  Retrieves the Answer-To-Reset information (in the SAP context) for the cards
  currently inserted in the ME.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_SEND_APDU
=========================================================================== */
/**
  Sends an Application Protocol data unit (APDU) command (in the SAP context) to
  the card.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] channel_id       Channel on which the APDU is sent.
  @param[in] apdu_data        APDU to send to the card.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  int32                          channel_id,
  mmgsdi_sap_send_apdu_data_type apdu_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_POWER_ON
=========================================================================== */
/**
  Powers on a card (in the SAP context).

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] sap_mode         Client or Server mode.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
   MMGSDI_CMD_QUEUE_FULL  -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_power_on(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_POWER_OFF
=========================================================================== */
/**
  Powers off a card (in the SAP context).

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] sap_mode         Client or Server mode.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

 @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_power_off(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_CARD_READER_STATUS
=========================================================================== */
/**
  Gets the status of a card reader (in the SAP context).

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] sap_mode         Client or Server mode.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_card_reader_status(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SAP_RESET
=========================================================================== */
/**
  Performs a card reset (in the SAP context).

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] sap_mode         Client or Server mode.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_sap_reset(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref
);

/* ============================================================================
FUNCTION:  MMGSDI_CARD_PUP
=========================================================================== */
/**
  Powers on a card.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] pup_option       Power up and switch to passive.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_card_pup(
    mmgsdi_client_id_type             client_id,
    mmgsdi_slot_id_enum_type          card_slot,
    mmgsdi_callback_type              response_cb_ptr,
    mmgsdi_card_pup_options_enum_type pup_option,
    mmgsdi_client_data_type           client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_CARD_PDOWN
=========================================================================== */
/**
  Powers off a card.

  @param[in] client_id       Client ID of the caller.
  @param[in] card_slot       Physical slot for multiple slot targets.
  @param[in] response_cb_ptr Pointer to the response callback.
  @param[in] pdown_option    Option to notify the Generic SIM Driver interface.
  @param[in] client_ref      User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_card_pdown(
    mmgsdi_client_id_type                  client_id,
    mmgsdi_slot_id_enum_type               card_slot,
    mmgsdi_callback_type                   response_cb_ptr,
    mmgsdi_card_pdown_options_enum_type    pdown_option,
    mmgsdi_client_data_type                client_ref
);

/* ============================================================================
FUNCTION:  MMGSDI_ACTIVATE_ONCHIP_USIM
=========================================================================== */
/**
  Builds an MMGSDI_ACTIVATE_ONCHIP_SIM_REQ request and queues it to the MMGSDI
  task for further processing.

  @param[in] client_id          Client ID of the caller.
  @param[in] card_slot          Physical slot for multiple slot targets.
  @param[in] onchip_sim_config  Data in Tag Length Value (TLV) format.
  @param[in] response_cb_ptr    Pointer to the response callback.
  @param[in] client_ref         User data returned upon completion of this
                                command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

 @dependencies
  The client must have a valid client ID, which is obtained by calling
  mmgsdi_client_id_reg(). If the client ID is not part of the client_id_table, a
  failure is returned in the response.

 @sideeffects
 Access to a physical card is not possible without a power cycle.
*/
mmgsdi_return_enum_type mmgsdi_activate_onchip_usim (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_activate_onchip_suim */

/* ============================================================================
FUNCTION:  MMGSDI_ACTIVATE_ONCHIP_SIM
=========================================================================== */
/**
  Activates an on-chip SIM.

  @param[in] client_id          Client ID of the caller.
  @param[in] card_slot          Physical slot for multiple slot targets.
  @param[in] onchip_sim_config  MMGSDI cache data for on-chip simulation in
                                TLV format.
  @param[in] response_cb_ptr    Pointer to the response callback.
  @param[in] client_ref         User data returned upon completion of this
                                command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.

  @sideeffects
  Access to a physical card is not possible without a power cycle.
*/
mmgsdi_return_enum_type mmgsdi_activate_onchip_sim (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
); /* mmgsdi_activate_onchip_sim */

/*===========================================================================
FUNCTION:  MMGSDI_GET_ESN_ME_CHANGE_FLAG
=========================================================================== */
/**
  Returns the Electronic Serial Number (ESN) ME change flag obtained after PIN1 initialization.

  @param[in] slot  Physical slot for multiple slot targets.

  @return
  MMGSDI_ESN_ME_UNKNOWN              -- ESN status is unknown \n
  MMGSDI_ESN_ME_NO_CHANGE            -- ESN has not changed.\n
  MMGSDI_ESN_ME_CHANGE               -- ESN has changed.\n
  MMGSDI_ESN_ME_UPDATE_PENDING       -- ESN has changed but needs update in card\n
  MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED -- ESN change is not supported.

  @dependencies
  None.
*/
mmgsdi_esn_me_change_enum_type mmgsdi_get_esn_me_change_flag (
  mmgsdi_slot_id_enum_type slot
);

/* ============================================================================
  FUNCTION:  MMGSDI_SEND_APDU
=========================================================================== */
/**
  Sends an APDU command to the card inserted in the specified slot.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] apdu_data        APDU to send to the card.
  @param[in] option           Additional optional parameters; currently none.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_send_apdu(
  mmgsdi_client_id_type      client_id,
  mmgsdi_slot_id_enum_type   card_slot,
  mmgsdi_send_apdu_data_type apdu_data,
  mmgsdi_option_type         option,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_SEND_APDU_EXT
=========================================================================== */
/**
  Sends an APDU command to the card inserted in the specified slot.

  Provides option to request for intermediate responses (i.e. SW1/SW2 of
  0x61/0xXX for UICC cards and 0x9F/0xXX for ICC cards) or the final
  card response.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] apdu_data        APDU to send to the card.
  @param[in] apdu_option      Specify the type of response required.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_send_apdu_ext(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_send_apdu_data_type         apdu_data,
  mmgsdi_send_apdu_options_enum_type apdu_option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ============================================================================
  FUNCTION:  MMGSDI_GET_ATR
=========================================================================== */
/**
  Retrieves the Answer-To-Reset information for the cards currently inserted in
  the ME.

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] option           Additional optional parameters; currently none.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and
                             sent to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type mmgsdi_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_option_type       option,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);

/*===========================================================================
  FUNCTION:  MMGSDI_NAA_REFRESH
=========================================================================== */
/**
  Used by the Generic SIM Toolkit task to request that a refresh be performed
  on the mobile.

  There are five GSM SIM/USIM refresh modes and two USIM refresh modes (as
  defined by mmgsdi_refresh_mode_enum_type):

   - MMGSDI_REFRESH_NAA_INIT_FULL_FCN  (SIM/USIM)
   - MMGSDI_REFRESH_FCN                (SIM/USIM)
   - MMGSDI_REFRESH_NAA_INIT_FCN       (SIM/USIM)
   - MMGSDI_REFRESH_NAA_INIT           (SIM/USIM)
   - MMGSDI_REFRESH_RESET              (SIM/USIM)
   - MMGSDI_REFRESH_NAA_APP_RESET      (USIM)
   - MMGSDI_REFRESH_3G_SESSION_RESET   (USIM)

  @param[in] client_id        Client ID of the caller.
  @param[in] card_slot        Slot on which the refresh is to be performed.
  @param[in] aid              Application ID (applicable to 3G only).
  @param[in] refresh_mode     Refresh mode as sent by the card.
  @param[in] refresh_files    Refresh files, if any (optional).
  @param[in] num_files        Number of refresh files.
  @param[in] alpha_string_ptr Alpha string, if Alpha is present
  @param[in] option           Optional parameter; currently unused.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this command.

  @dependencies
  The client must have a valid client ID.

  @sideeffects
  Depending on the mode requested, various events are generated as a result of
  the refresh performed.
*/
mmgsdi_return_enum_type mmgsdi_naa_refresh(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_aid_type                    aid,
  mmgsdi_refresh_mode_enum_type      refresh_mode,
  mmgsdi_data_type                   refresh_files,
  uint8                              num_files,
  mmgsdi_alpha_string_type           *alpha_string_ptr,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
  FUNCTION:  MMGSDI_GET_SE13_PLMN_NAMES
=========================================================================== */
/**
  This API is used to get plmn names by directly accessing SE13 table for
  given set of PLMN Ids.
 
  Upon successful completion of this function, MMGSDI sends a confirmation to
  client with PLMN name list.
 
  @param[in] client_id:           Client ID
  @param[in] plmn_id_list:        list of PLMN id
  @param[in] response_cb_ptr:     Response callback
  @param[in] client_ref:          User Data returned upon
                                  completion of this cmd.
 
  @dependencies
  The Client ID specified must be valid.
 
  @limitations
  None
 
  @return 
  MMGSDI_SUCCESS          -- The command structure was properly generated and
                                 queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS -- The parameters supplied to the API are not within
                             appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL   -- The command was not queued to the MMGSDI Task
                             because the max number of commands are already queued.

  @sideeffects
  None 
====================================================================== */
mmgsdi_return_enum_type mmgsdi_get_se13_plmn_names(
  mmgsdi_client_id_type              client_id,
  mmgsdi_plmn_id_list_type           plmn_id_list,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*====================================================================
FUNCTION:  MMGSDI_GET_SE13_PLMN_INFO_BY_NAME
=========================================================================== */
/**
  This API is used to search PLMN long name/short name in SE13 table. If both
  long name and short name are provided, search is successful only when both
  are present in SE13 table entry. PLMN name length and plmn name pointer
  should be valid. MMGSDI returns info related to all SE13 table entries in cnf
  where plmn name matches.

  Upon successful completion of this function, MMGSDI sends a confirmation to
  client with MCC, MNC, RAT type, PLMN long name and PLMN short name.

  @param[in] client_id           Client ID
  @param[in] plmn_long_name      plmn long name ptr and length
  @param[in] plmn_short_name     plmn short name ptr and length
  @param[in] rat_type            RAT type, if valid RAT type is
                                 passed MMGSDI returns entries
                                 matching the RAT.
  @param[in] response_cb_ptr     Response callback
  @param[in] client_ref          User Data returned upon
                                 completion of this cmd.

  @dependencies
  The Client ID specified must be valid.
  PLMN name string must be in Default 7 bit coded alphabet encoding.

  @limitations
  None

  @return 
  MMGSDI_SUCCESS           -- The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS  -- The parameters supplied to the API are not
                              within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL    -- The command was not queued to the MMGSDI Task
                              because the max number of commands are already queued.

  @sideeffects
  None 
====================================================================== */
mmgsdi_return_enum_type mmgsdi_get_se13_plmn_info_by_name(
  mmgsdi_client_id_type              client_id,
  mmgsdi_data_type                   plmn_long_name,
  mmgsdi_data_type                   plmn_short_name,
  mmgsdi_rat_enum_type               rat_type,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ============================================================================
   FUNCTION:  MMGSDI_GET_SE13_PLMN_INFO_BY_INDEX_SYNC
=========================================================================== */
/**
  This function will return PLMN info retrieved from SE13 table at given index. 

  @param[in]  client_id             Client ID of the requesting client.
  @param[in]  index                 table index 
  @param[out] se13_network_info_ptr se13 network info ptr

  @dependencies
  A valid Client ID is required, which can be achieved by calling
  mmgsdi_client_id_and_evt_reg()

  @limitations
  None

  @return 
  MMGSDI_SUCCESS            -- successful retrieval of PLMN name      
  MMGSDI_INCORRECT_PARAMS   -- The parameters supplied to the API are not
                               with in appropriate ranges.
  @sideeffects
  None 
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_get_se13_network_info_by_index_sync(
  mmgsdi_client_id_type            client_id,
  uint16                           index,
  mmgsdi_se13_network_info_type  * se13_network_info_ptr
);

/* ===========================================================================
   FUNCTION: MMGSDI_GET_CACHED_NV_ITEM 
=========================================================================== */
/**
  This function provides the cached value of the NV item

  @param[in] nv_item_data_ptr  Contains the NV Item enum to be read and
                               shall hold the NV item value when the API
                               call returns
 
  @dependencies
  None

  @limitations
  API returns valid information only if MMGSDI has already read and
  cached the requested NV item

  @return
  MMGSDI_ERROR   : On error to get from cache
  MMGSDI_SUCCESS : On successful retrival of NV from cache

  @sideeffects
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_cached_nv_item (
  mmgsdi_cached_nv_item_data        *nv_item_data_ptr
);

/* ============================================================================
   FUNCTION:  MMGSDI_GET_SE13_TABLE_RELEASE_INFO
=========================================================================== */
/**
  This function will return SE13 release date and source of the table.

  @param[out] se13_table_details_info_ptr se13 release info ptr

  @dependencies
  None

  @limitations
  None

  @return 
  MMGSDI_SUCCESS            -- successful retrieval of the SE13 release info
  MMGSDI_INCORRECT_PARAMS   -- on null pointer as parameter
  MMGSDI_ERROR              -- failure to retrieve SE13 release info

  @sideeffects
  None 
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_get_se13_table_release_info(
  mmgsdi_se13_table_details_info * se13_table_details_info_ptr
);

/*===========================================================================
  FUNCTION:  MMGSDI_CANCEL_REQUESTS
=========================================================================== */
/**
  This API is used to cancel all the pending requests for the particular 
  client ID.
 
  @param[in] client_id:           Client ID 
 
  @dependencies
  The client must have a valid client ID.
 
  @limitations
  None
 
  @return 
  MMGSDI_SUCCESS          -- The Command was properly processed.
  MMGSDI_INCORRECT_PARAMS -- The parameters supplied to the API are not within
                             appropriate ranges.

  @sideeffects
  None 
====================================================================== */
mmgsdi_return_enum_type mmgsdi_cancel_requests(
  mmgsdi_client_id_type              client_id
);

/* ==========================================================================
FUNCTION:       MMGSDI_SET_BUILTIN_PLMN_LIST
========================================================================== */
/**
  This function is used to set builtin PLMN list which is received from network.
  MMGSI stores this list in EFS and gives it precedence over EF-OPLMNwACT.

  @param[in] client_id:               Client ID 
  @param[in] builtin_plmn_list        list of builtin PLMNs
  @param[in] response_cb_ptr          Response callback
  @param[in] client_ref               User Data returned upon
                                      completion of this cmd.

  @return
  MMGSDI_SUCCESS          -- The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS.-- The parameters supplied to the API are not
                             within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL.  -- The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type  mmgsdi_set_builtin_plmn_list (
  mmgsdi_client_id_type                  client_id,
  mmgsdi_builtin_plmn_list_type          builtin_plmn_list,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
);

/* ===========================================================================
   FUNCTION: MMGSDI_GET_CACHED_NV_ITEM_EXT
=========================================================================== */
/**
  This function provides the cached value of the slot/subs-based NV item
  cached in MMGSDI

  @param[in] nv_item_data_ptr  Contains the NV Item enum to be read and
                               shall hold the NV item value when the API
                               call returns
  @param[in] nv_index          Index of the item in cache based on slot or
                               session type

  @dependencies
  None

  @limitations
  API returns valid information only if MMGSDI has already read and
  cached the requested NV item

  @return
  MMGSDI_ERROR   : On error to get from cache
  MMGSDI_SUCCESS : On successful retrival of NV from cache

  @sideeffects
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_cached_nv_item_ext (
  mmgsdi_cached_nv_item_data    *nv_item_data_ptr,
  uint8                          nv_index
);


/*===============================================================
FUNCTION:       MMGSDI_CONTINUE_AFTER_NV_REFRESH
============================================================== */
/**
This function is used to trigger MMGSDI to continue initialization after NV refresh is done.

  @param[in] slot_id:      Slot on which the NV refresh is complete 
  @param[in] nv_context:   Context where NV refresh is complete
  @param[in] reference_id: Reference Identifier
  @return
  VOID 

  @dependencies
  None
*/
void mmgsdi_continue_after_nv_refresh (
  mmgsdi_slot_id_enum_type  slot_id,
  mmgsdi_nv_context_type    nv_context,
  uint32                    reference_id
);

/* ==========================================================================
FUNCTION:       MMGSDI_RESTRICTED_SIM_ACCESS
========================================================================== */
/**
  This function provides restricted access to the SIM card via certain
  commands. The client passes parameters of the command APDU which are then
  used by MMGSDI by to queue an equivalent MMGSDI request. The asynchronous
  response is returned in APDU format.

  @param[in] client_id:               Client ID
  @param[in] card_slot                Slot on which to send the command
  @param[in] command                  Instruction
  @param[in] apdu_params              P1, P2, P3 parameters for command APDU
  @param[in] data                     Command data (if applicable)
  @param[in] file_id_and_path         Path to file including the file id
                                      path_len = 0 if not applicable
                                      path_len = 1 if only file_id present
  @param[in] response_cb_ptr          Response callback
  @param[in] client_ref               User Data returned upon
                                      completion of this cmd.

  @return
  MMGSDI_SUCCESS          -- The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS.-- The parameters supplied to the API are not
                             within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL.  -- The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type  mmgsdi_restricted_sim_access (
  mmgsdi_client_id_type                  client_id,
  mmgsdi_slot_id_enum_type               card_slot,
  uint8                                  command,
  mmgsdi_apdu_params_type                apdu_params,
  mmgsdi_data_type                       data,
  mmgsdi_path_type                       file_id_and_path,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
);

/*===========================================================================
  FUNCTION:  MMGSDI_BLOCK_CARD_STATUS_EVENTS
=========================================================================== */
/**
  This API is used to block QMI UIM from sending card status events.
 
  @param[in] client_id:           Client ID

  @dependencies
  The client must have a valid client ID.
 
  @limitations
  None
 
  @return
  MMGSDI_SUCCESS          -- The Command was properly processed.
  MMGSDI_INCORRECT_PARAMS -- The parameters supplied to the API are not within
                             appropriate ranges.

  @sideeffects
  None 
====================================================================== */
mmgsdi_return_enum_type mmgsdi_block_card_status_events(
  mmgsdi_client_id_type              client_id
);

/*================================================================
FUNCTION:      MMGSDI_GET_SIM_PROFILE
================================================================*/
/**
  Used to get the profiles supported/available.

  @param[in] client_id        Client ID of the caller.
  @param[in] slot             Specific slot.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this 
                                command.

  @return 
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

  @dependencies 
  The client must have a valid client ID, which is achieved by calling 
  mmgsdi_client_id_and_evt_reg(). If the client ID is not part of the
  client_id_table, a failure is returned in the response.
*/
mmgsdi_return_enum_type mmgsdi_get_sim_profile(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        slot,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SET_SIM_PROFILE
================================================================*/
/**
  Used to switch the profile to the given profile ID.

  @param[in] client_id            Client ID of the caller.
  @param[in] slot                 Specific slot.
  @param[in] activate_profile_id  Profile ID to be activated.
  @param[in] response_cb_ptr      Pointer to the response callback.
  @param[in] client_ref           User data returned upon completion of this 
                                    command.

  @return 
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

  @dependencies 
  The client must have a valid client ID, which is achieved by calling 
  mmgsdi_client_id_and_evt_reg(). If the client ID is not part of the
  client_id_table, a failure is returned in the response.
*/
mmgsdi_return_enum_type mmgsdi_set_sim_profile(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        slot,
  mmgsdi_profile_id_enum_type     activate_profile_id,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
);
/** @} */ /* end_addtogroup library */

#endif /* MMGSDILIB_H */
