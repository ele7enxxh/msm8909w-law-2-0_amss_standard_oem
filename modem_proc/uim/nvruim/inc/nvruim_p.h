#ifndef NVRUIM_P_H
#define NVRUIM_P_H
/*==========================================================================

         N V    R - U I M    I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal interface definitions for
  the NVRUIM component that are used within the UIM module.

REFERENCES
  IS-820, Removable User Identity Module (RUIM) for Spread Spectrum Systems

Copyright (c) 2009 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/nvruim/inc/nvruim_p.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/15   ar      Add check for 1X_PRI session for NVRUIM control flag init
09/04/14   ar      Remove NVRUIM cache
04/02/14   av      Fix to make sure non-CRM mob compiles
04/01/14   av      Moving definitions used by UIM tasks under nvruim/inc
02/22/14   av      Replace UIM enums with MMGSDI enums
10/17/13   at      SIM busy support in NVRUIM
05/13/13   vs      Decoupling NV-EFS from NV-RUIM
05/10/12   tl      Fixed MCC 414 to correct spec value
04/25/12   tl      Added support for MCC 0x158 and 0x19E
03/12/12   yt      Move data types from public header
11/11/11   ssr     Ignore the fallback request for non CT card
06/27/11   yt      Removed featurization from UIM public headers
05/16/11   ssr     Added NVRUIM thread safe support
02/08/11   ssr     Moved HRPD check from nvruim to mmgsdi layer
11/02/10   rm      Externize the nv_rtre_control API
08/12/10   ssr     Fixed nvruim 1x primary session for slot2
08/10/10   js      Updated nvruim get data sync to support 3gpd control data
06/18/10   ssr     Add non 3gpd cdma card check
06/17/10   js      Support for fetching nvruim data
                   on fusion
05/13/10   vs      Fix for NV and GSDI deadlock
04/30/10   ssr     Fixed nvruim clear cache
04/23/10   ssr     Fixed UIM 3GPD NV support
02/18/10   vs      Subscription mask fix
02/15/10   nb      Moved for Dual Slot Changes
11/17/09   yb      Moving nvruim_init_operator_card to nvruimi_v.h
09/30/09   mib     Added PACKED_POST for Q6 compilation
08/24/09   ssr     Fixed compilation error
02/24/09   nb      Initial Revision

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "nv.h"
#include "nvruimi.h"
#include "mmgsdilib_common.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
/*=========================================================================*/
/* The following present the Files found in the card */
#define NVRUIM_NO_DIR_PRESENT               0x00
#define NVRUIM_CDMA_SUBSCRIP_PRESENT        0x01
#define NVRUIM_CDMA_SUBSCRIP_READY          0x10

/* Number of bytes in the array to hold digits from converted MIN */
#define NVRUIM_NUM_MIN_DIGITS                 15

/* Non legacy cdma card   */
#define NVRUIM_NON_3GPD_CDMA_CARD_MCC1     0x167
#define NVRUIM_NON_3GPD_CDMA_CARD_MCC2     0x158
#define NVRUIM_NON_3GPD_CDMA_CARD_MCC3     0x12F

typedef enum {
  NV_RUIM_USE_ESN             = 0,    /* Use ESN as ESN */
  NV_RUIM_USE_RUIM_ID         = 1,    /* Use R-UIM ID as ESN */
  NV_RUIM_ZERO_ESN            = 2,    /* Zero the ESN */
  NV_RUIM_USE_MEID            = 3,    /* Use MEID as MEID */
  NV_RUIM_USE_SF_EUIMID       = 4,    /* Use SF RUIM ID as MEID */
  NV_RUIM_USE_SF_EUIMID_ESN   = 5,    /* Use SF EUIMID for identification and
                                         Use ESN for authentication*/
  NV_RUIM_USE_MEID_UIMID      = 6     /* Use MEID for identification and use
                                         UIMID for authentication */
} nvruim_esn_usage_type;

extern byte nvruim_dir_present;

/* Externize the nv_rtre_control() */
extern nv_rtre_control_type nv_rtre_control( void );

extern boolean nvruim_3gpd_sip_svc;
extern boolean nvruim_3gpd_mip_svc;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION NVRUIM_SET_UIM_DIR_PRESENT

DESCRIPTION
  The function set the flag to indicate which of the MF, CDMA DF, GSM DF, or DCS
  1800 DF exist.

DEPENDENCIES
  All above files have to be selected first.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void nvruim_set_uim_dir_present
(
  byte dir
);

/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONFIGURATION

DESCRIPTION
  This function queries the R-UIM card to determine if it supports
  R-UIM functionality (existance of CDMA DF) and if it supports
  phonebook (existance of Telcom DF).

DEPENDENCIES
  This function should only be called by the NV_CMD function to process
  an NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  NV_DONE_S  Operation is complete.

SIDE EFFECTS
  This function updates the RTRE control passed by reference based on
  the selects made to the UIM card.

===========================================================================*/
nv_stat_enum_type nvruim_process_rtre_configuration
(
  nv_cmd_type                  *cmd_ptr,
  nv_rtre_control_type         *nv_rtre_control,
  nv_rtre_polling_control_type *nv_rtre_polling_control
);


/*===========================================================================

FUNCTION NVRUIM_INIT_NON_3GPD_CDMA_CARD

DESCRIPTION
  This function is called from mmgsdi task during post pin initialization.
  It passes in a boolean, where
    TRUE : it is 3gpd disabled card
    FALSE: it is 3gpd enabled  card

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
none
===========================================================================*/
void nvruim_init_non_3gpd_cdma_card
(
  boolean                       non_3gpd_cdma_card,
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================

FUNCTION NVRUIM_INIT_HRPD_CAPABILITY

DESCRIPTION
  This function is called from the GSDI task during nvruim initialization.
  It passes in a boolean which indicates whether or not the RUIM have a
  HRPD capability

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_hrpd_control global variable for RUIM support of HRPD
===========================================================================*/
void nvruim_init_hrpd_capability
(
  boolean                       hrpd_disabled_card_flag,
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================

FUNCTION NVRUIM_INIT_FALLBACK_TO_CAVE

DESCRIPTION
  This function is called from the GSDI task during nvruim initialization.
  It passes in a boolean which indicates fallback to CAVE is allowed or
  not after the MD5 failure

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void nvruim_init_fallback_to_cave
(
  boolean                       allow_to_fallback,
  mmgsdi_session_type_enum_type session_type
);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

FUNCTION NVRUIM_SET_ESN_USAGE

DESCRIPTION
   This function sets the usage indicator that is used by the NV-RUIM module
   to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None.

SIDE EFFECTS
  Updates the usage indicator variable nvruim_esn_usage.

===========================================================================*/
void nvruim_set_esn_usage
(
  nvruim_esn_usage_type esn_usage
);

/*===========================================================================

FUNCTION NVRUIM_GET_ESN_USAGE

DESCRIPTION
   This function gets the usage indicator that is used by the NV-RUIM module
   to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
nvruim_esn_usage_type nvruim_get_esn_usage
(
  void
);

/*===========================================================================

FUNCTION UIM_INIT_3GPD_PREFERENCE

DESCRIPTION
  This function is called from the MMGSDI task during initialization for 3GPD.
  It passes in a boolean which indicates whether or not the RUIM provides
  for 3GPD services

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the uim_3gpd_support_available global variable for RUIM support of 3GPD
  Sets the uim_3gpd_cdma_svc global variable to indicate CDMA service support


===========================================================================*/
void uim_init_3gpd_preference
(
  boolean                       service_3gpd_sip,
  boolean                       service_3gpd_mip,
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_INIT_EXTENSIONS_SUPPORT

DESCRIPTION
  This function is called from the MMGSDI task during initialization for 3GPD
  Extentions Support. It passes in a boolean value which indicates whether
  or not the RUIM card have support for service 15 (Messagind and 3GPD
  Extentions) in CDMA service table. The requirement on the RUIM card for
  this serviec is defined in Open Market Handset RUIM Specification
  (CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_3gpd_ext_support global variable for extended 3GPD RUIM support
  Sets the nvruim_3gpd_ext_use_card variable for current status of 3GPD ext usage
===========================================================================*/
void nvruim_data_3gpd_init_extensions_support
(
  boolean                       service_3gpd_extensions,
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================

FUNCTION NVRUIM_SESSION_INIT

DESCRIPTION
  This function will register with MMGSDI, obtain a client id and open a
  session with the default CDMA subscription

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void nvruim_session_init
(
  void
);

#endif /* NVRUIM_P_H */
