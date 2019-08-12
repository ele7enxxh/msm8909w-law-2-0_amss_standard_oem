#ifndef NVRUIMI_H
#define NVRUIMI_H
/*==========================================================================

         N V    R - U I M    I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file contains the interface definitions for NVRUIM.

REFERENCES
  IS-820, Removable User Identity Module (RUIM) for Spread Spectrum Systems

Copyright (c) 2000 - 2010,2012,2014 byQUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/nvruimi.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/14   av      Cleanup nvruim headers and move nvruimi.h under api folder
12/13/13   av      Replace UIM enums with MMGSDI enums
08/10/12   av      Removed RPC support along with RPC style comments
07/03/12   av      Provided a nvruim_init() routine for crit sect init
03/12/12   yt      Moving data types to nvruimi_v.h
07/02/10   nmb     Removed Featurization
05/21/10   ssr     Add non 3gpd cdma card check
03/25/10   ssr     Added nvruim_data_3gpd_ext_support
02/15/10   nb      Moved Dual Slot Changes
11/17/09   yb      Fixed bad RUIM card issue with HRPD Service n5 in CDMA
                   Service Table improperly set
                   Moving nvruim_init_operator_card to nvruimi_v.h
08/24/09   ssr     Fixed compilation error
07/02/09   ssr     Operator MCC Check for 3GPD
03/02/09   sun     Removed Private header inclusion
02/24/09   nb      Common Modem Interface
06/20/08   ssr     Support of OMH LBS Items
04/25/08   ssr     Support of OMH Data Items
03/26/08   jk      OMH Phase 1B Support - Multiple SIP Profiles
01/17/08   nk      Added CDMA Refresh Support
10/10/07   jk      Changes to support Open Market Handset
10/08/07   vs/nk   Removed inclusion of header file "uiuint.h"
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
08/03/07   nk      Added usage indicator for EUIMID support
07/10/07   nk      Added EUIMID support
05/11/06   wli     Extern nvruim_decode_min() for perso
01/25/06   jk      Added uiuint.h include file for old targets which require
                   this include in nvimr.c
11/22/05   jk      Added support for multiprocessor RPC
11/22/04   jk      Finished adding support for 3GPD.
09/01/04   jk      Added support for HRPD
12/19/03   jk      Added interface for R-UIM 3GPD Services.
09/25/03   tml     Featurized uiuint.h
11/25/02   ts      Expose the directories present variable for the UIM power
                   up/wake up procedures to set accordingly.
06/19/02   ts      Added function prototype to process RTRE configuration.
03/28/02   wli     Added NAM_LOCK mask for IS820 Addendum 1 changes.
02/13/02   wli     Added a function to indicate if some DFs exist.
12/13/01   wli     Added funciton to flush SMS cache.
07/23/01   ck      Added zeroing of ESN as an option in the esn usage enum.
05/18/01   ck      Modified the NVRUIM module to interface with the UIM server.
03/07/01   wli     Defined constants.
03/06/01   wli     Support more NV items for registration indicators.
02/22/01   ck      Added definitions to handle accesses to the items OTAPA
                   enable and SPC enable.
02/07/01   wli     Deleted invalidate and rehabilitate interface functions.
1/28/01    wli     Added phone book support which includes speed dial(NV_DIAL_I)
                   and call history (NV_STACK_I).
                   Added a new interface function that enable user to invalidate
                   or rehabilitate an EF.
12/11/01   ts      Added nvruim_flush_cache to be accessed when the RUIM is
                   reset.
11/15/00   ck      Added the enum definitions for esn usage indicator.
04/24/00   jah     Initial Revision

===========================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "nv.h"


/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/* Types of return values for 3GPD control check from Data Services */
typedef enum {
  NVRUIM_3GPD_MIP_RUIM_SIP_RUIM = 0,    /* ME should use R-UIM for 3GPD */
  NVRUIM_3GPD_MIP_NV_SIP_NV     = 1,    /* ME should use NV/SW for 3GPD  */
  NVRUIM_3GPD_MIP_RUIM_SIP_NV   = 2,    /* ME only supports MIP 3GPD */
  NVRUIM_3GPD_MIP_NV_SIP_RUIM   = 3     /* ME only supports SIP 3GPD */
} nvruim_3gpd_support_status;

/* Types of return values for HRPD control check from Data Services */
typedef enum {
  NVRUIM_AN_HRPD_SUPPORT        = 0,    /* ME should use R-UIM for HRPD   */
  NVRUIM_AN_HRPD_NO_SUPPORT     = 1,    /* ME should use NV/SW for HRPD  */
  NVRUIM_AN_HRPD_USE_CAVE       = 2     /* ME should use Cave Algo for HRPD*/
} nvruim_an_hrpd_support_status;

/* Types of return values for nv 3gpd check from Data Services */
typedef enum {
  NVRUIM_NO_CARD                = 0,    /* NO Card  */
  NVRUIM_NON_3GPD_CARD          = 1,    /* 3GPD disabled card  */
  NVRUIM_3GPD_CARD              = 2     /* 3GPD enabled card   */
} nvruim_3gpd_nv_card_status;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION NVRUIM_FLUSH_CACHE

DESCRIPTION
  This function clears the cache control so that all items are read from the
  RUIM the next time they are requested.

DEPENDENCIES
  This function affects the cache control variable and therefore affects the
  operation of the read of cached NV items from the RUIM.

RETURN VALUE
  None

SIDE EFFECTS
  This function affects the operation of the cache algorithm.

===========================================================================*/
void nvruim_flush_cache
(
  void
);

/*===========================================================================

FUNCTION UIM_3GPD_SUPPORT

DESCRIPTION
  This function checks whether 3GPD data and processing should come from the
  RUIM or from NV

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
nvruim_3gpd_support_status uim_3gpd_support
(
  void
);

/*===========================================================================

FUNCTION UIM_AN_HRPD_SUPPORT

DESCRIPTION
  This function checks whether HRPD data and processing should come from the
  RUIM or from NV

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
nvruim_an_hrpd_support_status uim_an_hrpd_support
(
  void
);

/*===========================================================================

FUNCTION nvruim_init_wms_svc_items

DESCRIPTION
  This function is called from the WMS task during initialization for SMS.
  It passes in boolean values to indicate whether or not the RUIM provides
  for BCSMS and SMS Capabilities provisioning.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_bcsms_svc and nvruim_smscap_svc
  global variables to indicate service support.

===========================================================================*/
void nvruim_init_wms_svc_items
(
  boolean service_bcsms,
  boolean service_smscap
);

/*===================================================================
FUNCTION NVRUIM_PPP_CAVE_FALLBACK

DESCRIPTION
  Sets the hrpd control variable to Cave

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==================================================================*/
void nvruim_ppp_cave_fallback
(
  void
);

/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_EXT_SUPPORT

DESCRIPTION
This function returns 3GPD_EXT service availability

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
 TRUE : When RUIM card has 3GPD_EXT service available
 FALSE: When RUIM card does not have 3GPD_EXT service

SIDE EFFECTS
None.

===========================================================================*/
boolean nvruim_data_3gpd_ext_support
(
  void
);

/*===========================================================================

FUNCTION NVRUIM_CHECK_NON_3GPD_CDMA_CARD

DESCRIPTION
 This function returns the non 3gpd cdma card presence.

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
  nvruim_3gpd_nv_card_status where
  NVRUIM_NO_CARD        : NO Card
  NVRUIM_NON_3GPD_CARD  : 3GPD disabled card
  NVRUIM_3GPD_CARD      : 3GPD enabled card

SIDE EFFECTS
none
===========================================================================*/
nvruim_3gpd_nv_card_status nvruim_check_non_3gpd_cdma_card
(
  void
);

/*===========================================================================

FUNCTION NVRUIM_LBS_INIT_SUPPORT

DESCRIPTION
  This function is called from the Data task during initialization for LBS
  Support. It passes in a boolean value which indicates whether or not the
  RUIM card have support for service 24 (LBS)in its CDMA Service Table.
  The requirement on the RUIM card for this serviec is defined in Open Market
  Handset RUIM Specification(CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_lbs_support global variable for LBS support
===========================================================================*/
void nvruim_lbs_init_support
(
  boolean service_lbs
);

/*===========================================================================

FUNCTION UIM_INIT_AN_HRPD_PREFERENCE

DESCRIPTION
  This function is called from the PS task during initialization for HRPD.
  It passes in a boolean which indicates whether or not the RUIM provides
  for HRPD services, and a boolean which indicates whether cave is to be used

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_hrpd_control global variable for RUIM support of HRPD
  Sets the nvruim_hrpd_cdma_svc global variable to indicate CDMA service support
===========================================================================*/
void uim_init_an_hrpd_preference
(
  boolean service_hrpd
);

/*===========================================================================

FUNCTION NVRUIM_INIT

DESCRIPTION
This function is called by rcinit in order to initialize the nvruim_crit_sect.
For newer targets, the tasks are free-floating and NV and DIAG tasks
get initiliazed way before mmgsdi. So, when a nv read or write is performed,
this nvruim crit sect must be initialized (even though gsdi task is not up at
that point).

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void nvruim_init
(
  void
);

#endif /* NVRUIMI_H */
