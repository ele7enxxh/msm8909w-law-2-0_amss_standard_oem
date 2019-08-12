/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   C A R D  I N I T I A L I Z A T I O N  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to handle the Card power-up
  and initalization.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2011 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_card_init.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/16   vdc     Compare ESN data from EF ESN from second byte
10/25/16   dd      Allow store ESN when refresh is successful
10/25/16   dd      Added support for auto reg SMS when ESN is changed
07/20/16   shr     Support for NV based Toolkit disablement
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
07/08/16   bcho    Add attribute cache before creating data cache
06/27/16   bcho    Handle read failures for AUTH related EFs
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
06/17/16   vdc     Added support for SPN based SIMLOCK feature
05/31/16   lm      Added support to detect multi profile card based on EFs
05/26/16   bcho    Fix memory leak exposed when validating EF-EPSNSC/EPSLOCI
05/24/16   ar      Review of macros used by MMGSDI
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/16/16   tkl     F3 log prints cleanup
05/16/16   ar      No need to send PDOWN if EF-DIR fails for eUICC card
05/04/16   bcho    Make CSIM AD an optional EF as it is not being used
04/19/16   nr      Don't reject activation req during ecc call for AP centric
04/11/16   av      Introduce LPA support
01/30/16   vdc     Halt initialization if IMSI is not provisioned in card
01/07/16   kv      UIM PSM handling updates
01/06/16   bcho    parameter type changed for function mmgsdi_cache_delete_item
01/06/16   bcho    Reset early init flag when card error occurs
12/25/15   kk      Send Card and session events based on SFS data in PSM mode
11/23/15   vdc     Send profile switching evt when profile has switched in card
11/05/15   ks      Rel12 polling enhancement
10/16/15   ar      Adding MCCs in list which have default MNC len as 3 digits
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
09/28/15   ar      Add check for 1X_PRI session for NVRUIM control flag init
09/09/15   ar      Moving mandatory flag for EF-KEYS from TRUE to NV dependent
09/07/15   bcho    Migrate to mcfg EFS APIs
09/08/15   bcho    Run SIMLOCK Algo in Onchip SIM
07/30/15   nr      Fix in handling of GCF anite flag update NV:73684 is 6 set
07/09/15   ar      Validate PRL at diff steps if SD task not ready
05/14/15   kk      Update efs context parameter in uim common efs API call
04/30/15   nr      Enhancements in handling of GCF anite flag update
03/25/15   ar      Ability to process app deact while activation is in progress
03/24/15   kk      Subs prov EFS error handling
01/27/15   vdc     Delete EPSLOCI & EPSNSC cache when EPS MMI srv is disabled
12/15/14   kk      Fix MCFG UIM data sent for nv refresh
11/26/14   av      Added new EFS item for activation of apps on multi-slots
11/24/14   vv      Send simlock run algo request if pre-perso caching is done
11/19/14   av      Fix logic of setting GCF NV when card contains CSIM
11/14/14   hh      Fix compilation warnings
11/12/14   vdc     Send card error evt if NO_ATR is received after slot power up
11/06/14   hh      Call GSTK NV API to read GSTK features in NV 65674
10/14/14   tkl     Fix cache activate with imsi_m
10/14/14   av      Handle appropriately zero ICCID reported by uimdrv
10/13/14   ar      Skip CSIM/RUIM App auto-activation when RTRE is NV_ONLY
10/07/14   yt      Add check for test IMSI
09/29/14   tkl     Added support for persistent cache
09/09/14   kk      Enhancements in NV handling
09/25/14   hh      Validate IMSI contents and min length of 4 bytes
08/29/14   hh      Fix compliation warnings
08/28/14   vv      Added support for unlock via REFRESH
08/19/14   hh      Moved 1x_upp related functions to mmgsdi_1x_upp.c
08/14/14   tl      Add support for silent PIN verification
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/25/14   av      Fix incorrect F3 message
07/25/14   av      Auto onchip activation for multi slots
07/16/14   kk      Fix MCFG - MMGSDI interactions
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
06/30/14   vv      Remove Perso code
06/18/14   av      Validate cdmahome EF attributes during power up
06/14/14   kk      Fixed channel data handling after app termination is skipped
06/12/14   av      Cache mandatory EFs used by NVRUIM for critical 1x NV reads
06/11/14   yt      Initialize EF-PRL/EF-EPRL cache to 0 if length is invalid
06/10/14   tl      Introduce EF 3GPDUPPExt
06/03/14   tl      Remove additional parameter variables
06/02/14   bcho    Skip caching of CDMA EPRL file
05/27/14   av      Don't allow access if UST/CST can't be read(pin not verified)
05/13/14   yt      Cache EF-PRL/EF-EPRL based on the actual PRL size
04/01/14   av      NVRUIM headers cleanup
03/26/14   tl      SIM Lock secondary revisions
03/25/14   hh      Make EF_START_HFN optional in case of NV configuration
03/18/14   tl      Introduce new SIM Lock feature
03/13/14   ar      Update correct PRL_VERSION and PRL_REVISION
02/25/14   yt      Store parsed ATR in mmgsdi_data_slot_ptr
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
01/30/14   av      Do not send down a SELECT prior to READ in cache init functions
12/05/13   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/16/14   av      New item for deciding bw card and nv for 3gpd credentials
01/16/14   df      Allow for commercial cards missing EF-Phase
01/13/14   ar      Ignore EF validation if file is not mandatory for init
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
01/08/14   tl      Mark USIM EF-AD and EF-THRESHOLD as mandatory through NV
12/19/13   vv      Reset NV_GPRS_ANITE_GCF_I when non-anite MCCs are detected
12/18/13   yt      Skip app termination as part of session deactivation
12/16/13   tl      Prevent initalizing the cache with partial files
12/13/13   vdc     Ignore error during validation of EF-ACC due to b3 in byte 1
12/11/13   df      Remove unused memory paging feature
11/18/13   vv      Added support for parallel REFRESH
11/05/13   tkl     Add IMSI check for TDS test SIM
11/01/13   spo     Support to determine if ICCID has changed for ICC cards
10/29/13   ar      PRL Mem leak when 2 PRLs are present in file
10/24/13   av      Fix processing of END_FAILED refresh evt for DEACT and FCN
10/21/13   yt      Assign access class at random if all bytes of EF-ACC are 0
10/21/13   bcho    Set EPS MMI svc to un-available if EPS data len is not valid
10/18/13   vv      Removed FEATURE_MMGSDI_OP_MODE_SLAVE
10/18/13   av      Remove unnecessary SELECTS on EF PRL and EF EPRL
10/16/13   df      Removing legacy gsdi conversion functions
10/16/13   tl      Remove MMGSDI_FEATURE_SAP
10/04/13   rp      F3 message reduction
10/02/13   av      Do not activate onchip for non-test IMSIs
10/01/13   vv      Do not perform READ, if EF size or rec length is zero
09/26/13   av      Enable OnChip on bootup based on EFS item
09/24/13   yt      Remove function mmgsdi_util_is_gsdi_task_context()
09/23/13   df      Support for SIM busy
08/28/13   yt      Deprecate VIRTUIM/VIRTSAT module
08/20/13   spo     Automatic Subscription provisioning enhancements in UIM
08/13/13   yt      Reject EF-ACC data if bit for Access Class 10 is 1
08/08/13   vdc     Remove mapping of CSIM/RUIM apps on slot 2/3 to 1X PRI
07/30/13   yt      Perform icc_get_sim_capabilities before pre-perso caching
07/29/13   vv      Even if PRL init fails continue with RUIM card power-up
07/29/13   am      Added support to determine CSG support dynamically
07/19/13   yt      Populate pre-perso cache after TP download
07/15/13   vs      EONS updates to support single baseband SGLTE
07/12/13   tl      Default SSPR rev to 3 for validation of EPRL contents
06/27/13   yt      Flush client req info for pending cmds upon link_est
06/25/13   yt      Populated cache for files require before mmgsdi perso evt
06/24/13   vdc     Added support for triple SIM
06/10/13   tkl     Marked cache not found when UIM return command timeout
05/21/13   vv      Added support for session closed cause
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
04/25/13   yt      Fix compilation for non-CDMA builds
04/17/13   tl      Replace Featurization with NV
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/04/13   bcho    Deactivate session if provisioning fails
03/31/13   tl      Skip GCF NV write when CSIM app is present
03/29/13   yt      Virtual SIM clean-up
03/29/13   vdc     Removed code for selecting DF-CDMA before TP download
03/26/13   vv      Allow init if optional EPRL file is missing
03/15/13   at      Fix for incorrect storage of ICCID in MMGSDI cache
03/07/13   av      Fixed improper memset during link_established
03/04/13   av      Write to NV 947 only if necessary
03/04/13   av      Retrieve ICCID from link_established and cache it
02/28/13   vv      Fixed sending select request based on the app type
02/19/13   av      Fix caching procedure for post pin1 files
02/04/13   vv      Added mmgsdi_session_read_prl support for RUIM card
01/28/13   vv      Fix compiler warnings
01/23/13   vv      Added EF-NASCONFIG to post pin1 cache list
01/20/13   vv      Added EONS support for CSG
01/04/13   tl      Fill request type parameter before write transparent
12/30/12   kk      Fixed async TP for few other scenarios
12/30/12   kk      Reworked AsyncTP for faulty RUIM cards having no GSM info
12/05/12   vv      Added support for parallel processing in MMGSDI
12/05/12   av      Fixed compilation warnings
10/31/12   tl      CSIM conformance updating EF MODEL/MECRP
10/30/12   tl      CSIM conformance updating EF MODEL
10/21/12   av      Added support for new ECC event
10/16/12   abg     Allow initialization if card is missing mandatory
                   GSM EF-Kc file
10/15/12   yt      Flush pending commands from client_req_table if marked
                   as rsp_not_rcvd
10/10/12   tl      Fixed 1x memory leaks
10/01/12   vs      Added utility for context check including logic for SGLTE
10/01/12   tl      Moved NV item read to mmgsdi init for optimization
09/28/12   yt      Update user profile parameters after successful OTASP Commit
09/25/12   av      Skip reading EF-ARR for internal selects
09/04/12   av      Don't send READ req if get_file_attr returns file_not_found
09/03/12   bcho    Trigger function to cache OPL/PNN files changed
07/25/12   tl      Removed store meid/esn for Virtual SIM
07/16/12   tl      Retrieve MAX PRL/EPRL from EFPRL/EFEPRL
06/06/12   nb      Backing out last change
05/22/12   nb      Reworked AsyncTP for faulty RUIM cards having no GSM info
05/03/12   av      Support store meid/esn offtarget
04/25/12   tl      Added support for MCC 0x158 and 0x19E
04/24/12   tl      Updated MEID as the default identification
04/20/12   av      Enabled detouring of nv/efs calls to offtarget framework
03/30/12   av      Replaced feature flags with NVITEM
03/29/12   shr     Enhancements to file access request methods
03/21/12   bcho    External personalization modified
                   1.Send perso event with status based on halt subscription
                     config value from efs
                   2.function mmgsdi_card_init_process_wait_for_sub_ok removed
03/16/12   yt      Modify references to gsd_task_cb
03/12/12   vs      Always request FCP for EF Select
03/02/12   av      Removed feature FEATURE_MMGSDI_NV_ME_CONFIG
02/29/12   bcho    Virtual SIM clean-up
02/18/12   av      Cleaned up remaining ALWAYS OFF/ON feature flags
02/09/12   av      Remove ALWAYS ON 1x feature flags
02/09/12   vv      Allow initialization even if MMGSDI_GSM_LP is missing
01/30/12   bcho    Trigger to OPL/PNN cache modified
01/17/12   kk      Do not allow external cache updates for specific EFs when
                   cache is empty
01/13/12   at      Added NV item caching for NV_DISABLE_CM_CALL_TYPE_I
01/09/12   js      Reset sanity timer value on card error
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   kk      Added USIM ECC caching support
12/21/11   shr     Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "customer.h"
#include "uim_msg.h"
#include "err.h"
#include "queue.h"
#include "dsm.h"
#include "memheap.h"
#include "mmgsdiutil.h"
#include "mmgsdi.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdilib_p.h"
#include "uim_p.h"
#include "mmgsdi_session.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_cphs.h"
#include "mmgsdi_nv.h"
#include "mmgsdicache.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"
#include "mmgsdi_eons_opl_pnn.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_sub_selection.h"
#include "mmgsdi_uim_common.h"
#include "mmgsdi_builtin_plmn.h"
#include "secapi.h"
#include "mmgsdi_file.h"
#include "mmgsdi_silent_pin.h"
#include "simlock_common.h"
#include "mmgsdi_1x_upp.h"
#include "mmgsdi_multi_profile.h"
#include "mmgsdi_psm.h"
#include "mmgsdi_euicc.h"

#ifdef FEATURE_SIMLOCK
#include "mmgsdi_simlock.h"
#include "mmgsdi_simlock_refresh.h"
#endif /* FEATURE_SIMLOCK */

#ifdef FEATURE_GSTK
#include "gstk_p.h"
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI_3GPP2
#include "nvruim_p.h"
#include "prl.h"
#endif /* FEATURE_MMGSDI_3GPP2 */

#ifdef FEATURE_UIM_PERSISTENT_CACHE
#include "mmgsdi_persistent_cache.h"
#endif /* FEATURE_UIM_PERSISTENT_CACHE*/

#include "fs_lib.h"
#include "fs_public.h"

#include "diag.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Max. bytes of response per command to card */
#define MMGSDI_MAX_BYTES_PER_CMD_FROM_UIM    512

/* Max commands pending on UIM to respond */
#define MMGSDI_MAX_UIM_CMDS_PENDING          10

/* Size of MAX PRL Size EF */
#define MMGSDI_SIZE_OF_MAX_PRL_SIZE          4

/* Size of CPRL CRC field */
#define MMGSDI_CPRL_CRC_SIZE                 2

/* Size of CDMAHOME SID NID EF per C.S0065 and C.S0023 */
#define MMGSDI_CDMA_HOME_SID_NID_REC_LEN     5

/* Max size of IMSI data */
#define MMGSDI_MAX_VALID_IMSI_DATA_LEN       9

/* Minimum length of valid IMSI data */
#define MMGSDI_MIN_VALID_IMSI_LEN_INTERNAL   4

/* Size of EF AD data in bytes */
#define MMGSDI_EF_AD_LEN                     4

/* Offset for MNC length in EF-AD. This should always be
   less than MMGSDI_EF_AD_LEN */
#define MMGSDI_MNC_LEN_OFFSET_IN_EF_AD       3

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_GPRS_GCF_ANITE_NV_VALUE
-----------------------------------------------------------------------------*/
/** Identifiers used for GPRS GCF ANITE FLAG update
*/
typedef enum {
  MMGSDI_GPRS_GCF_ANITE_NV_NO_TEST_SIM,
  MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_1,
  MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_2,
  MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_3,
  MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_4,
  MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_5,
  MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_ANY_SLOT,
  MMGSDI_GPRS_GCF_ANITE_NV_LAST_SIM
}mmgsdi_gprs_gcf_anite_nv_value;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_MCC_DATA_TYPE

   DESCRIPTION:
     This structure contains MCC 1-3 values
-------------------------------------------------------------------------- */
typedef struct{
  uint8 mcc1;
  uint8 mcc2;
  uint8 mcc3;
} mmgsdi_mcc_data_type;

/* Reserved MCC value for GPRS ANITE GCF */
mmgsdi_mcc_data_type mmgsdi_reserved_mcc_tbl[] =
{
  {0x00, 0x00, 0x01}, {0x00, 0x00, 0x02}, {0x00, 0x00, 0x03},
  {0x00, 0x00, 0x04}, {0x00, 0x00, 0x05}, {0x00, 0x00, 0x06},
  {0x00, 0x00, 0x07}, {0x00, 0x00, 0x08}, {0x00, 0x00, 0x09},
  {0x00, 0x01, 0x00}, {0x00, 0x01, 0x01}, {0x00, 0x01, 0x02}
};

/* Reserved IMSI value for TDS Tests */
mmgsdi_imsi_data_type mmgsdi_reserved_tds_imsi_tbl[] =
{
  {{0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09}},
  {{0x04, 0x06, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x08, 0x05, 0x00}},
  {{0x04, 0x06, 0x06, 0x00, 0x02, 0x01, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x08, 0x05, 0x00}}
};

/* Reserved test IMSIs for GCF test cases */
mmgsdi_imsi_data_type mmgsdi_reserved_test_imsi_tbl[] =
{ /* From TS 31.121 Section 5.1.2.4.1 */
  {{0x02, 0x04, 0x06, 0x08, 0x01, 0x03, 0x05, 0x07, 0x09, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F}},
  /* From TS 31.121 Section 4.1.1.1 */
  {{0x02, 0x04, 0x06, 0x00, 0x08, 0x01, 0x03, 0x05, 0x07, 0x09, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F}}
};

/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_populate_card_app_slot_info(
  const mmgsdi_slot_link_established_data_type *link_est_data_ptr,
  mmgsdi_slot_id_enum_type                      slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability_sim(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability_ruim(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability_uicc(
  mmgsdi_session_id_type       session_id,
  mmgsdi_app_enum_type         app_type,
  mmgsdi_slot_id_enum_type     slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability(
  mmgsdi_session_id_type       session_id,
  const mmgsdi_slot_data_type *slot_data_ptr,
  mmgsdi_slot_id_enum_type     slot_id,
  mmgsdi_app_enum_type         app_type
);

static mmgsdi_return_enum_type mmgsdi_card_init_hzi_init(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
);

static void mmgsdi_card_init_set_gprs_anite_nv_if_reqd(
  mmgsdi_int_app_info_type *app_info_ptr,
  mmgsdi_slot_id_enum_type  slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_validate_imsi_data (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
);

static mmgsdi_return_enum_type mmgsdi_card_init_validate_netpar (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
);

static mmgsdi_return_enum_type mmgsdi_card_init_validate_rplmnwact (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
);

static mmgsdi_return_enum_type mmgsdi_card_init_validate_hkey_data (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type status,
  const mmgsdi_data_type *data_ptr
);

static mmgsdi_return_enum_type mmgsdi_card_init_validate_acc_data (
  mmgsdi_session_id_type    session_id,
  mmgsdi_return_enum_type   mmgsdi_status,
  const mmgsdi_data_type   *acc_data_ptr
);

static mmgsdi_return_enum_type mmgsdi_card_init_app_cache(
  mmgsdi_session_id_type              session_id,
  mmgsdi_app_enum_type                app_type,
  mmgsdi_slot_id_enum_type            slot,
  const mmgsdi_cache_init_order_type *app_init_table_ptr,
  uint32                              num_entry
);

#ifdef FEATURE_MMGSDI_3GPP2
static void mmgsdi_card_init_1x_srv_nvruim_init(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
);

static void mmgsdi_card_init_csim_write_model_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

static void mmgsdi_card_init_csim_write_me_crp_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_csim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_ruim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);
#endif /* FEATURE_MMGSDI_3GPP2 */

static void mmgsdi_card_init_overwrite_oplmnwact_cache(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_app_enum_type     app_type
);

static mmgsdi_return_enum_type mmgsdi_card_init_usim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_sim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_proc_post_pin1_init(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_app_enum_type     app_type
);

static mmgsdi_return_enum_type mmgsdi_card_init_proc_pre_perso_cache (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_app_enum_type     app_type
);

static void mmgsdi_card_init_3G_ready_status_ind(
  mmgsdi_session_id_type   session_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_process_card_inserted(
  mmgsdi_slot_id_enum_type slot_id
);

static mmgsdi_return_enum_type mmgsdi_card_init_process_prov_session_activated(
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_state_enum_type app_state_req
);

static mmgsdi_return_enum_type mmgsdi_card_init_process_post_pin1(
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_state_enum_type app_state_req
);

static mmgsdi_return_enum_type mmgsdi_card_init_process_post_perso(
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_state_enum_type app_state_req
);

static void mmgsdi_card_init_remove_apps_available_info(
  mmgsdi_slot_data_type *slot_data_ptr,
  mmgsdi_app_enum_type   app_type
);

static void mmgsdi_card_init_process_internal_pup_init_status(
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type internal_pup_status
);

#ifdef FEATURE_GSTK
static boolean mmgsdi_card_init_is_toolkit_allowed(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
);

static void mmgsdi_card_init_perform_post_pin1_tp_if_needed(
  mmgsdi_session_id_type     session_id,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_app_enum_type       app_type
);
#endif /* FEATURE_GSTK */

static mmgsdi_session_closed_cause_enum_type mmgsdi_card_init_map_file_to_session_closed_cause(
  mmgsdi_file_enum_type file,
  boolean               file_content_invalid
);

/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_POPULATE_APP_PIN_TABLE

DESCRIPTION
  This function will populate the MMGSDI App table.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_populate_app_pin_table(
  mmgsdi_slot_id_enum_type        slot_id,
  uint32                          app_index,
  mmgsdi_key_ref_values_enum_type slot_pin2_key_ref)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  uint8                   slot_index    = MMGSDI_SLOT_1_INDEX;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(app_index >= MMGSDI_MAX_EXT_APP_INFO)
  {
    UIM_MSG_ERR_1("Invalid app index", app_index);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Allocating memory for the application*/
  if(mmgsdi_app_pin_table_ptr[slot_index][app_index] != NULL)
  {
    MMGSDI_MSG_ERROR_2("App table is not NULL, slot index 0x%x, app index 0x%x",
                       slot_index, app_index);
    return MMGSDI_ERROR;
  }
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_pin_table_ptr[slot_index][app_index],
    sizeof(mmgsdi_int_app_info_type));

  if(mmgsdi_app_pin_table_ptr[slot_index][app_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Reset PIN2 info */
  slot_data_ptr->app_info_pptr[app_index]->pin2.pin_id  = MMGSDI_PIN2;
  slot_data_ptr->app_info_pptr[app_index]->pin2.key_ref = slot_pin2_key_ref;

  /* Reset Hiddenkey info */
  slot_data_ptr->app_info_pptr[app_index]->hiddenkey.pin_id               = MMGSDI_HIDDENKEY;

  /* Reset Personalization info */
  slot_data_ptr->app_info_pptr[app_index]->perso_info.feature             = MMGSDI_MAX_PERSO_FEATURE_ENUM;
  slot_data_ptr->app_info_pptr[app_index]->perso_info.num_retries         = 0;
  slot_data_ptr->app_info_pptr[app_index]->perso_info.num_unblock_retries = 0;

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_populate_app_pin1_table */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_TRIGGER_TP_DL_FOR_UICC

DESCRIPTION
  This function will trigger TP DL for UICC card.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_trigger_tp_dl_for_uicc(
  mmgsdi_slot_id_enum_type        slot_id,
  boolean                        *fail_reason_card_error_ptr,
  boolean                        *ef_dir_not_found_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_data_type   *slot_data_ptr = NULL;
#ifndef FEATURE_GSTK
  uint32                   num_apps      = 0;
#endif

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* If GSTK is available, send out the TP request to GSTK and
     wait for response. EF-DIR shall be read only after the TP response
     is received. If GSTK is not available, read EF-DIR right away
     and send out the CARD_INSERTED event */
#ifdef FEATURE_GSTK
  (void) fail_reason_card_error_ptr;
  (void) ef_dir_not_found_ptr;

  /* Do a Terminal Profile Download */
  mmgsdi_card_init_gstk_terminal_profile_download(0,
                                                  slot_id,
                                                  MMGSDI_APP_USIM);

  /* Reset MMGSDI status */
  mmgsdi_status = MMGSDI_SUCCESS;
#else
  /* Set UIM proactive to true once UICC mode of operation is identified */
  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      uim_set_proactive_uim(FALSE);
      break;
    case MMGSDI_SLOT_2:
      uim_set_proactive_uim_2(FALSE);
      break;
    case MMGSDI_SLOT_3:
      uim_set_proactive_uim_3(FALSE);
      break;
    default:
      UIM_MSG_ERR_1("Unknown slot 0x%x", slot);
      /* For uknown slot, default behaviour continues */
      break;
  }

  /* Check if there are any valid UICC applications on the card */
  mmgsdi_status = mmgsdi_uicc_get_available_aids(slot_id,
                                                 slot_data_ptr,
                                                 &num_apps,
                                                 fail_reason_card_error_ptr,
                                                 ef_dir_not_found_ptr);

  /* If there are valid UICC apps found... */
  if((mmgsdi_status == MMGSDI_SUCCESS) && (num_apps > 0))
  {
#ifdef FEATURE_MMGSDI_3GPP
    if(mmgsdi_util_get_usim_apps(slot_id))
    {
      slot_data_ptr->card_apps_available |= MMGSDI_USIM_APP_MASK;
    }
#endif /* FEATURE_MMGSDI_3GPP */
#ifdef FEATURE_MMGSDI_3GPP2
    if(mmgsdi_util_get_csim_apps(slot_id))
    {
      slot_data_ptr->card_apps_available |= MMGSDI_CSIM_APP_MASK;
      nvruim_set_uim_dir_present(NVRUIM_CDMA_SUBSCRIP_PRESENT);
    }
#endif /* FEATURE_MMGSDI_3GPP2 */
    UIM_MSG_HIGH_3("No. of Apps present in UICC is 0x%x, USIM present: %x, CSIM present: %x",
                   num_apps,
                   slot_data_ptr->card_apps_available & MMGSDI_USIM_APP_MASK,
                   slot_data_ptr->card_apps_available & MMGSDI_CSIM_APP_MASK);
  }
#endif /* FEATURE_GSTK */

 return mmgsdi_status;
}/* mmgsdi_card_init_trigger_tp_dl_for_uicc */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_POPULATE_CARD_APP_SLOT_INFO

DESCRIPTION
  This function will populate the MMGSDI slot and generic info globals based on
  the information passed in from the Link Established message.

DEPENDENCIES
  UIM Task needs to notify GSDI when a Link has been established. Only then
  is this function called.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_populate_card_app_slot_info(
  const mmgsdi_slot_link_established_data_type *link_est_data_ptr,
  mmgsdi_slot_id_enum_type                      slot_id
)
{
  uint32                            app_index              = 0;
  mmgsdi_session_id_type            card_session_id        = 0;
  mmgsdi_slot_data_type            *slot_data_ptr          = NULL;
  mmgsdi_int_app_info_type        **app_info_pptr          = NULL;
  mmgsdi_return_enum_type           mmgsdi_status          = MMGSDI_ERROR;
  mmgsdi_data_type                  iccid_data;
  mmgsdi_access_type                access_info;
  mmgsdi_key_ref_values_enum_type   slot_pin1_key_ref      = MMGSDI_PIN_APPL_1;
  mmgsdi_key_ref_values_enum_type   slot_pin2_key_ref      = MMGSDI_SEC_PIN_APPL_1;
  uint8                             pin1_index             = 0;
  uint8                             slot_index             = MMGSDI_SLOT_1_INDEX;
  boolean                           mcfg_refresh_status    = FALSE;
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  boolean                           nv_refresh_enabled     = FALSE;
  mcfg_refresh_uimdata_s_type       mcfg_uim_data;
  mcfg_refresh_index_type           mcfg_slot_index        = MCFG_REFRESH_INDEX_INVALID;
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/
  mmgsdi_nv_context_type            nv_context;
  boolean                           fail_reason_card_error = FALSE;
  boolean                           ef_dir_not_found       = FALSE;

  UIM_MSG_HIGH_1("Populate Card Slot data for slot: 0x%x", slot_id);

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(link_est_data_ptr);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  memset(&iccid_data, 0, sizeof(mmgsdi_data_type));

  /* Clear the Slot specific MMGSDI data */
  mmgsdi_util_preinit_mmgsdi(0,
                             MMGSDI_APP_NONE,
                             MMGSDI_REFRESH_RESET,
                             slot_id,
                             FALSE);

  /* --------------------------------------------------------------------------
  Populate card info to MMGSDI slot data
  ----------------------------------------------------------------------- */

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Need to make sure that card matches the card used in previous
     attempt and for which PIN information might be still cached to
     perform extended recovery procedure */
  if(mmgsdi_silent_pin_is_cached(slot_id) &&
     !mmgsdi_silent_pin_match_iccid(&link_est_data_ptr->iccid, slot_id))
  {
    UIM_MSG_HIGH_0("Silent PIN cache ICCID mismatched");

    /* The card being activated does not match the card that was being
       used when recovery was in progress so we can clear the cache. */
    mmgsdi_silent_pin_purge_info(slot_id);
  }

  /* Allocating memory for PIN1 */
  for(pin1_index = 0; pin1_index < MMGSDI_MAX_PIN1_INFO; pin1_index++)
  {
    if(mmgsdi_app_pin1_table_ptr[slot_index][pin1_index] != NULL)
    {
      MMGSDI_MSG_ERROR_2("PIN1 table is not NULL, slot index 0x%x, pin1 index 0x%x",
                         slot_index, pin1_index);
      return MMGSDI_ERROR;
    }
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_app_pin1_table_ptr[slot_index][pin1_index],
      sizeof(mmgsdi_app_pin_info_type));

    if(mmgsdi_app_pin1_table_ptr[slot_index][pin1_index] == NULL)
    {
      return MMGSDI_ERROR;
    }
    mmgsdi_app_pin1_table_ptr[slot_index][pin1_index]->pin_id  = MMGSDI_PIN1;
    mmgsdi_app_pin1_table_ptr[slot_index][pin1_index]->key_ref = slot_pin1_key_ref++;
  }

  /* Allocating memory for Univ PIN*/
  if(mmgsdi_app_univ_pin_table_ptr[slot_index] != NULL)
  {
    MMGSDI_MSG_ERROR_1("Univ PIN table is not NULL, slot index 0x%x",
                       slot_index);
    return MMGSDI_ERROR;
  }
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_univ_pin_table_ptr[slot_index],
    sizeof(mmgsdi_app_pin_info_type));

  if (mmgsdi_app_univ_pin_table_ptr[slot_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_app_univ_pin_table_ptr[slot_index]->pin_id  = MMGSDI_UNIVERSAL_PIN;
  mmgsdi_app_univ_pin_table_ptr[slot_index]->key_ref = MMGSDI_UNIVERSAL_PIN_KEY;

  UIM_MSG_HIGH_3("Slot 0x%x: Card Present: 0x%x, Slot State: 0x%x",
                 slot_id,
                 link_est_data_ptr->card_present,
                 slot_data_ptr->mmgsdi_state);

  if(slot_data_ptr->mmgsdi_state == MMGSDI_STATE_NO_CARD)
  {
    /* Card has been detected but not initialized yet, set state accordingly */
    UIM_MSG_HIGH_0("Changing card state to NOT_INIT");
    slot_data_ptr->mmgsdi_state = MMGSDI_STATE_NOT_INIT;
  }

  /* Is euicc or not */
  slot_data_ptr->is_euicc = link_est_data_ptr->is_euicc;

  /* Copy ATR */
  mmgsdi_memscpy(&slot_data_ptr->card_atr,
                 sizeof(slot_data_ptr->card_atr),
                 &link_est_data_ptr->atr,
                 sizeof(mmgsdi_atr_info_type));

  /* Copy History characters */
  mmgsdi_memscpy(&slot_data_ptr->hist_char,
                 sizeof(slot_data_ptr->hist_char),
                 &link_est_data_ptr->hist_char,
                 sizeof(mmgsdi_hist_char_info_type));

  /* Copy the card protocol */
  slot_data_ptr->protocol = link_est_data_ptr->protocol;

  /* Decode the ATR */
  (void)mmgsdi_util_parse_atr(slot_id);

  app_info_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(slot_id);

  /* Assinging app_info_ptr to App_PIN table */
  slot_data_ptr->app_info_pptr = app_info_pptr;

  /* Get the Card Slot Session ID corresponding to the slot */
  card_session_id = mmgsdi_generic_data_ptr->slot_session_id[slot_index];

  /* Retrieve the ICCD */
  memset(&access_info, 0x00, sizeof(mmgsdi_access_type));
  access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  access_info.file.file_enum = MMGSDI_ICCID;

  if(link_est_data_ptr->iccid.length > 0)
  {
    iccid_data.data_len = (mmgsdi_len_type) link_est_data_ptr->iccid.length;
    iccid_data.data_ptr = (uint8 *)link_est_data_ptr->iccid.data;

    /* Copy the ICCID to the common cache */
    mmgsdi_status = mmgsdi_cache_write(card_session_id,
                                       &access_info,
                                       iccid_data,
                                       0x00,
                                       MMGSDI_DATA_FROM_CARD_COMPLETE);
  }
  else
  {
    /* Drivers could not read ICCID from the card... most certainly it is
       not present on the card. Lets add an entry in common cache indicating
       that ICCID EF is not present on the card */
    mmgsdi_status = mmgsdi_cache_write(card_session_id,
                                       &access_info,
                                       iccid_data,
                                       0x00,
                                       MMGSDI_DATA_NOT_FOUND);
  }

  /* Update NV if previously stored ICCID doesn't match with current ICCID */
  mmgsdi_nv_update_rpm_iccid(iccid_data, slot_id);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  if(mmgsdi_util_get_mcfg_refresh_index(slot_id, &mcfg_slot_index) != MMGSDI_SUCCESS)
  {
     return MMGSDI_ERROR;
  }

  nv_refresh_enabled = mcfg_refresh_autoselect_enabled(mcfg_slot_index);
  UIM_MSG_HIGH_1("MCFG REFRESH status in nv - 0x%x",nv_refresh_enabled);
  if(nv_refresh_enabled)
  {
    memset(&mcfg_uim_data, 0x00, sizeof(mcfg_refresh_uimdata_s_type));
    /* Trigger MCFG NV Refresh */
    mcfg_uim_data.iccid.data_len = iccid_data.data_len;
    mcfg_uim_data.iccid.data_ptr = iccid_data.data_ptr;
    mcfg_uim_data.type = MCFG_REFRESH_UIM_ICCID;
    mcfg_uim_data.slot_index = mcfg_slot_index;
    /* session or sub is not known yet */
    mcfg_uim_data.sub_index = MCFG_REFRESH_INDEX_INVALID;
    mcfg_uim_data.reference_id = ++slot_data_ptr->nv_refresh_reference_count;

    /* Trigger MCFG to perform Refresh of Slot based NV items based on iccid.
       Card inserted evt is not sent out until mcfg responds back with the
       mmgsdi_continue_after_nv_refresh API call
       this is to make sure all the clients perform refresh and only then clients are
       informed of new card */
    mcfg_refresh_status = mcfg_refresh_update_from_uim (&mcfg_uim_data);
    if(mcfg_refresh_status)
    {
      slot_data_ptr->nv_refresh_in_progress = TRUE;
    }
  }
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

  /* Determine the Protocol used at the SIM - ME Interface */
  switch (link_est_data_ptr->protocol)
  {
    case MMGSDI_ICC:
      /* Required for GSM and RUIM with GSM access */
      /* IS GSM DF/APP Available */
      slot_data_ptr->gsm_df_present = link_est_data_ptr->gsm_df_present;
      if(link_est_data_ptr->gsm_df_present)
      {
        UIM_MSG_HIGH_0("GSM DF Present");

        mmgsdi_status = mmgsdi_card_init_populate_app_pin_table(slot_id,
                                                                app_index,
                                                                slot_pin2_key_ref);

        /* PIN2 key reference is incremented to point to next PIN2 for
           assigning this to next app */
        slot_pin2_key_ref++;

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          return mmgsdi_status;
        }

        slot_data_ptr->card_apps_available |= MMGSDI_GSM_APP_MASK;
        slot_data_ptr->app_info_pptr[app_index]->app_data.app_type = MMGSDI_APP_SIM;
        slot_data_ptr->app_info_pptr[app_index]->proprietary_app = FALSE;
        app_index++;
      }

      /* IS CDMA DF/APP Available */
#ifdef FEATURE_MMGSDI_3GPP2
      if(link_est_data_ptr->cdma_df_present)
      {
        UIM_MSG_HIGH_0("CDMA DF Present");

        mmgsdi_status = mmgsdi_card_init_populate_app_pin_table(slot_id,
                                                                app_index,
                                                                slot_pin2_key_ref);

        /* PIN2 key reference is incremented to point to next PIN2 for
           assigning this to next app */
        slot_pin2_key_ref++;

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          return mmgsdi_status;
        }


        slot_data_ptr->card_apps_available |= MMGSDI_RUIM_APP_MASK;
        slot_data_ptr->app_info_pptr[app_index]->app_data.app_type = MMGSDI_APP_RUIM;
        slot_data_ptr->app_info_pptr[app_index]->proprietary_app = FALSE;

        nvruim_set_uim_dir_present(NVRUIM_CDMA_SUBSCRIP_PRESENT);
        app_index++;
      }
#endif /* FEATURE_MMGSDI_3GPP2 */

      if(mcfg_refresh_status)
      {
        /* MCFG refresh started successfully -
           handle rest of initialization in MCFG done API */
        break;
      }

      /* As per spec 10.7 of GSM 11.11, the DF-DCS should be selected if and only if
         DF-GSM (7F20) is absent.

         NOTE 1: The selection of the GSM application using the identifier '7F21',
         if selection by means of the identifier '7F20' fails, is to ensure backwards
         compatibility with those Phase 1 SIMs which only support the DCS 1800 application
         using the Phase 1 directory DFDCS1800 coded '7F21'.

         NOTE 2: To ensure backwards compatibility with those Phase 1 DCS 1800 MEs,
         which have no means to selectDFGSM two options have been specified.
         These options are given in GSM 09.91 [17].
      */
      /* Look for DCS 1800 DF only for 1X/GSM Targets */
      nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

      if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_UIM_GSM_DCS_1800, nv_context) ==
          MMGSDI_FEATURE_ENABLED)
      {
        if(!link_est_data_ptr->gsm_df_present)
        {
          mmgsdi_status = mmgsdi_icc_is_dcs1800_df_present(card_session_id, slot_id);

          if (mmgsdi_status == MMGSDI_SUCCESS)
          {
            UIM_MSG_HIGH_0("DCS 1800 DF Present");

            slot_data_ptr->dcs1800_present = TRUE;
            slot_data_ptr->card_apps_available |= MMGSDI_DCS1800_APP_MASK;
          }
        }
      }
      break;

    case MMGSDI_UICC:
      /* Read the Language Preference EF */
      (void) mmgsdi_card_init_cache_binary(card_session_id, MMGSDI_APP_USIM,
                                           slot_id, MMGSDI_ELP, NULL);

      if(mcfg_refresh_status)
      {
        /* MCFG refresh started successfully -
           handle rest of initialization in MCFG done API */
        break;
      }

      /* Trigger TP DL for UICC Card */
      mmgsdi_status = mmgsdi_card_init_trigger_tp_dl_for_uicc(slot_id,
                                                              &fail_reason_card_error,
                                                              &ef_dir_not_found);
      break;

    default:
      /* A Protocol was not established at the UIM Interface */
      UIM_MSG_HIGH_2("Invalid Protocol 0x%x for slot 0x%x",
                     link_est_data_ptr->protocol, slot_id);
      break;
  }

  if(slot_data_ptr->card_apps_available != 0x00 ||
     slot_data_ptr->is_euicc)
  {
    /* Valid applications were found */
    mmgsdi_status = MMGSDI_SUCCESS;
  }
#ifndef FEATURE_GSTK
  /* If fail_reason_card_error is TRUE, it means UIMDRV has not processed
     this cmd because of some internal error or card_error. In that case,
     there is no need to switch the protocol */
  else if(link_est_data_ptr->protocol == MMGSDI_UICC &&
          fail_reason_card_error == FALSE)
  {
      /* Some eUICCs do not have a correct information in the ATR
       regarding eUICC support. In such cases, slot_data_ptr->is_euicc
       could be set to FALSE. We need to find out if it is really an
       eUICC or not by sending a SELECT on ISDR. If SELECT on ISDR
       succeeds, it is an eUICC, otherwise not */
    if(!slot_data_ptr->is_euicc && ef_dir_not_found)
    {
      boolean is_isdr_found     = FALSE;

      if(MMGSDI_SUCCESS == mmgsdi_euicc_is_isdr_found(slot_id,
                                                      &is_isdr_found) &&
         is_isdr_found)
      {
        slot_data_ptr->is_euicc = TRUE;
      }
    }

    if(slot_data_ptr->is_euicc)
    {
      UIM_MSG_HIGH_1("Not switching protocol to ICC even if Ef-DIR is missing because card is eUICC on slot=0x%x",
                     slot_id);
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    else
    {
      UIM_MSG_HIGH_1("No valid UICC app. found on slot 0x%x, switching protocol to ICC",
                     slot_id);

      mmgsdi_util_preinit_mmgsdi(card_session_id,MMGSDI_APP_NONE,MMGSDI_REFRESH_RESET,
                                 slot_id,FALSE);

      /* Switch card protocol to ICC */
      mmgsdi_status = mmgsdi_gen_card_reset_sync(slot_id, UIM_ICC);

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_UIM_INSTN_CHG_SUCCESS;
      }
    }
  }
#endif /* FEATURE_GSTK */
  return mmgsdi_status;
} /* mmgsdi_card_init_populate_card_app_slot_info */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_GET_APP_CAPABILITY_SIM

DESCRIPTION
  Will be used to get the capabiites of the GSM application.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability_sim(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type          mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_evt_session_notify_type   notify_type;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /*---------------------------------------------------------------------------
    Per GSM 11.11 Section 11.2.1 SIM Initialization
    After activation:
      1.  Detect GSM DF By Selecting an EF under the DF
      2.  Optionally attempt to select EF-ECC
      3.  Read all ECCs if EF-ECC is available
  ---------------------------------------------------------------------------*/
  /* Read EF-ECC */
  mmgsdi_status = mmgsdi_card_init_cache_binary(
                    session_id,
                    MMGSDI_APP_SIM,
                    slot_id,
                    MMGSDI_GSM_ECC,
                    NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("MMGSDI_GSM_ECC read failed 0x%x", mmgsdi_status);
  }

  /* Send out the ECC event (even if the ECC read failed)*/
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;
  notify_type.session_id  = session_id;
  mmgsdi_evt_build_and_send_ecc(notify_type);

  /* Read Language Preference */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                MMGSDI_APP_SIM,
                                                slot_id,
                                                MMGSDI_GSM_LP,
                                                NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("MMGSDI_GSM_LP read failed 0x%x", mmgsdi_status);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_get_app_capability_sim */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_GET_APP_CAPABILITY_RUIM

DESCRIPTION
  Will be used to get the capabiites of the RUIM application.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability_ruim(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type          mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_evt_session_notify_type   notify_type;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /* Read and Cache RUIM ID */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                MMGSDI_APP_RUIM,
                                                slot_id,
                                                MMGSDI_CDMA_RUIM_ID,
                                                NULL);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    return mmgsdi_status;
  }

  /* Get the Emergency Call Codes.
     Optional File - Initialization will not fail if EF is not present */
  mmgsdi_status = mmgsdi_card_init_cache_binary(
                    session_id,
                    MMGSDI_APP_RUIM,
                    slot_id,
                    MMGSDI_CDMA_ECC,
                    NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("MMGSDI_CDMA_ECC Read Fail 0x%x", mmgsdi_status);
  }

  /* Send out the ECC event (even if the ECC read failed)*/
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;
  notify_type.session_id  = session_id;
  mmgsdi_evt_build_and_send_ecc(notify_type);

  /* Get the Language Preference Information
     Mandatory File - Initialization will fail if EF is not present */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                MMGSDI_APP_RUIM,
                                                slot_id,
                                                MMGSDI_CDMA_PREF_LANG,
                                                NULL);

  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid Language Preference, status: 0x%x", mmgsdi_status);
    return mmgsdi_status;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_get_app_capability_ruim */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_GET_APP_CAPABILITY_UICC

DESCRIPTION
  Will be used to get the capabiites of the UICC (USIM/CSIM) application.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability_uicc(
  mmgsdi_session_id_type       session_id,
  mmgsdi_app_enum_type         app_type,
  mmgsdi_slot_id_enum_type     slot_id
)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_file_enum_type               mmgsdi_lang_ind_enum = MMGSDI_USIM_LI;

  /*---------------------------------------------------------------------------
    TS 31.102 5.1.1.2 - Request USIM/CSIM ECCs
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_3GPP2
  /* CSIM ECC read - Optional file */
  if(app_type == MMGSDI_APP_CSIM)
  {
    mmgsdi_evt_session_notify_type   notify_type;

    memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

    mmgsdi_lang_ind_enum = MMGSDI_CSIM_LANG_IND;
    (void)mmgsdi_card_init_cache_binary(
            session_id,
            app_type,
            slot_id,
            MMGSDI_CSIM_ECC,
            NULL);

     /* Send out the ECC event (even if the ECC read failed)*/
    notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
    notify_type.slot_id     = slot_id;
    notify_type.session_id  = session_id;
    mmgsdi_evt_build_and_send_ecc(notify_type);
  }
  /* USIM ECC read */
  else
#endif /* FEATURE_MMGSDI_3GPP2 */
  if(app_type == MMGSDI_APP_USIM)
  {
    mmgsdi_lang_ind_enum = MMGSDI_USIM_LI;
    mmgsdi_status = mmgsdi_cache_init_usim_ecc(session_id,slot_id);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* continue initialization */
      UIM_MSG_ERR_0("USIM ECC Caching failed ");
    }
  }
  else
  {
    UIM_MSG_ERR_1("Invalid app_type in mmgsdi_get_card_capability_uicc 0x%x",
                  app_type);
    return MMGSDI_ERROR;
  }

  /* Language Indicator - Optional */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                app_type,
                                                slot_id,
                                                mmgsdi_lang_ind_enum,
                                                NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("LI 0x%x Read Fail 0x%x", mmgsdi_lang_ind_enum, mmgsdi_status);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_get_app_capability_uicc */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_GET_APP_CAPABILITY

DESCRIPTION
  Depending on the application being intialized, reads and caches
  Language Preference, ECC, RUIM ID.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_get_app_capability(
  mmgsdi_session_id_type       session_id,
  const mmgsdi_slot_data_type *slot_data_ptr,
  mmgsdi_slot_id_enum_type     slot_id,
  mmgsdi_app_enum_type         app_type
)
{
  mmgsdi_return_enum_type    mmgsdi_status                = MMGSDI_SUCCESS;
  boolean                    is_icc                       = TRUE;
  mmgsdi_len_type            data_len                     = 0;
  mmgsdi_access_type         file_access;

  MMGSDIUTIL_RETURN_IF_NULL(slot_data_ptr);

  memset(&file_access, 0x00, sizeof(mmgsdi_access_type));

  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = MMGSDI_ELP;

  /* ELP is common on a per card basis, if it has already been cached,
     we should skip the cache read and write again to avoid unnecessary
     command being sent to the card */
  if(mmgsdi_cache_read_len(session_id,
                           &file_access,
                           &data_len,
                           0,
                           NULL) == MMGSDI_NOT_INIT)
  {
    mmgsdi_status = mmgsdi_util_is_card_icc(slot_data_ptr, &is_icc);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Unknown ICC vs UICC card type! */
      return mmgsdi_status;
    }
    /* Only ICC protocol should have the Language being read here because for
       UICC protocol, the ELP should have been read already */
    if(is_icc)
    {
      mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                    app_type,
                                                    slot_id,
                                                    MMGSDI_ELP,
                                                    NULL);
    }
  }

  /* Application specific capability retrieval */
  switch(app_type)
  {
    case MMGSDI_APP_SIM:
      mmgsdi_status = mmgsdi_card_init_get_app_capability_sim(
                        session_id, slot_id);
      break;

    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_card_init_get_app_capability_ruim(
                        session_id, slot_id);
      break;

    case MMGSDI_APP_CSIM:
    case MMGSDI_APP_USIM:
      mmgsdi_status = mmgsdi_card_init_get_app_capability_uicc(
                        session_id,
                        app_type,
                        slot_id);
      break;

    default:
      UIM_MSG_ERR_1("Invalid app type 0x%x", app_type);
      return MMGSDI_ERROR;
  }

  return mmgsdi_status;
} /* mmgsdi_card_init_get_app_capability */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_HZI_INIT

DESCRIPTION
  Initializes files for homezone

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_hzi_init(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
)
{
  mmgsdi_file_enum_type              file_name        = MMGSDI_MAX_FILE_ENUM;
  int32                              settings_field   = 0x00;
  byte                               version_field    = 0x00;
  boolean                            uiccv1           = FALSE;
  nv_stat_enum_type                  nv_status        = NV_DONE_S;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_write_cnf_type             *cnf_ptr          = NULL;
  mmgsdi_int_app_info_type          *int_app_info_ptr = NULL;
  mmgsdi_data_type                   imsi_data;
  mmgsdi_data_type                   hzi_data;
  mmgsdi_nv_item_cache_type          mmgsdi_nv_data   = {0};


  /* --------------------------------------------------------------------------
     First Check for the NV Item. If present, then proceed
     ------------------------------------------------------------------------*/
  nv_status = mmgsdi_nv_get_item(NV_HOMEZONE_ENABLED_I,
                                 &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  if ( nv_status != NV_DONE_S )
  {
    UIM_MSG_HIGH_0("Could not get Homezone");
    return MMGSDI_SUCCESS;
  }

  if (mmgsdi_nv_data.item_value.homezone_enabled == FALSE)
  {
    UIM_MSG_HIGH_0("Homezone not enabled");
    return MMGSDI_SUCCESS;
  }

  /* --------------------------------------------------------------------------
     Get the IMSI information from the SIM/USIM
     ------------------------------------------------------------------------*/
  if(app_type == MMGSDI_APP_USIM)
  {
    file_name = MMGSDI_USIM_IMSI;
  }
  else if(app_type == MMGSDI_APP_SIM)
  {
    file_name = MMGSDI_GSM_IMSI;
  }
  else
  {
    UIM_MSG_ERR_1("Incorrect app tpye for Homezone init.: 0x%x", app_type);
    return MMGSDI_INCORRECT_PARAMS;
  }

  memset(&imsi_data, 0x00, sizeof(mmgsdi_data_type));

  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                app_type,
                                                slot,
                                                file_name,
                                                &imsi_data);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (imsi_data.data_ptr == NULL))
  {
    UIM_MSG_ERR_0("IMSI could not be read");
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(imsi_data.data_ptr);
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     If the card is an HZI SIM
     ------------------------------------------------------------------------*/
  if(((imsi_data.data_ptr[1] & MMGSDI_HZI_IMSI_FIRST_BYTE) ==
        imsi_data.data_ptr[1])                             &&
     imsi_data.data_ptr[2] == MMGSDI_HZI_IMSI_SECOND_BYTE  &&
     (imsi_data.data_ptr[3] == MMGSDI_HZI_IMSI_THIRD_BYTE  ||
      imsi_data.data_ptr[3] == MMGSDI_HZI_IMSI_THIRD_BYTE_ALT))
  {
    UIM_MSG_HIGH_0("HZI IMSI Present");

    /* Read HZI settings file */
    memset(&hzi_data, 0x00, sizeof(mmgsdi_data_type));

    mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                  app_type,
                                                  slot,
                                                  MMGSDI_SIM_7F43_PROP1_SETTINGS,
                                                  &hzi_data);

    if((mmgsdi_status == MMGSDI_SUCCESS) &&
       (hzi_data.data_ptr != NULL)&&
       (hzi_data.data_len > 0))
    {
      /*Check the Activation bit. IF the bit is FALSE, do not do anything -
      UICC Requirements - UHZI- Files*/
      if((hzi_data.data_ptr[0] & MMGSDI_HZI_ACTIVATED) == FALSE)
      {
        UIM_MSG_ERR_0("HZI not activated");
        MMGSDIUTIL_TMC_MEM_FREE(imsi_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(hzi_data.data_ptr);
        return MMGSDI_SUCCESS;
      }
      /* Write back ME mode information to HZI settings file */
      else if (hzi_data.data_len >= 2)
      {
        mmgsdi_write_req_type write_req;

        settings_field = (int32)hzi_data.data_ptr[0];
        version_field = hzi_data.data_ptr[1];

#if defined(FEATURE_WCDMA) && defined(FEATURE_GSM)
        /* For Dual Mode Phones, add 1 */
        settings_field = settings_field + MMGSDI_HZI_DUAL_MODE_SETTING;
#elif defined(FEATURE_GSM)
        /* For Single Mode Phones, add 2 */
        settings_field = settings_field + MMGSDI_HZI_SINGLE_MODE_SETTING;
#endif

        /*Settings Field could overflow*/
        version_field = int32touint8(settings_field & 0xFF);
        /*update Version fields of U HZI-UE Settings with "Settings"*/
        hzi_data.data_ptr[1] = version_field;

        memset(&write_req, 0x00, sizeof(mmgsdi_write_req_type));

        write_req.request_header.session_id         = session_id;
        write_req.request_header.slot_id            = slot;
        write_req.request_header.request_type       = MMGSDI_WRITE_REQ;
        write_req.request_header.orig_request_type  = MMGSDI_WRITE_REQ;
        write_req.access.access_method              = MMGSDI_EF_ENUM_ACCESS;
        write_req.access.file.file_enum             = MMGSDI_SIM_7F43_PROP1_SETTINGS;
        write_req.rec_num                           = 0;
        write_req.offset                            = 0;
        write_req.file_type                         = MMGSDI_TRANSPARENT_FILE;
        write_req.data.data_len                     = 2;
        write_req.data.data_ptr                     = hzi_data.data_ptr;

        if(app_type == MMGSDI_APP_USIM)
        {
          mmgsdi_status = mmgsdi_uim_uicc_write_transparent(
                            &write_req,
                            TRUE,
                            &cnf_ptr);
        }
        else
        {
          mmgsdi_status = mmgsdi_uim_icc_write_transparent(
                            &write_req,
                            TRUE,
                            &cnf_ptr);
        }

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("Could not write to HZI Settings");
        }
        mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
      }
    }/*Read Setting File*/
    else
    {
      uiccv1 = TRUE;
    }
  }
  else
  {
    UIM_MSG_HIGH_0("HZI IMSI not Present");
    MMGSDIUTIL_TMC_MEM_FREE(imsi_data.data_ptr);
    return MMGSDI_SUCCESS;
  }

  MMGSDIUTIL_TMC_MEM_FREE(imsi_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hzi_data.data_ptr);

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    NULL,
                                                    &int_app_info_ptr);

  if((mmgsdi_status == MMGSDI_SUCCESS) &&
     (int_app_info_ptr != NULL))
  {
    /*Update the HZI version in the app info */
    if(uiccv1)
    {
      int_app_info_ptr->hzi_version = MMGSDI_UHZI_V1;
    }
    else
    {
      int_app_info_ptr->hzi_version = MMGSDI_UHZI_V2;
    }
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_hzi_init */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_SET_GPRS_ANITE_NV_IF_REQD

DESCRIPTION
  This function reads GPRS_ANITE_GCF NV item, compares it against the
  desired MCC and if needed, writes to the item (only if MCC in NV is
  different). In addition to GPRS_ANITE_GCF_NV, it also reads/writes
  if needed, an internal EFS item that indicates whether GSTK should
  operate in test/GCF mode or not. Following algorithm is followed:
  1. If MCC/MNC is test MCC/MNC, MMGSDI will set GPRS_ANITE_NV
     to TRUE irrespective of CSIM presence.
  2. If MCC/MNC is not a test MCC/MNC, MMGSDI will ensure that GPRS_ANITE_NV
     is not set as TRUE already... if it is already set to TRUE, MMGSDI will
     set it to FALSE. Same applies to GSTK GCF EFS item.
  2. If card contains CSIM, and MCC/MNC is test MCC/MNC, MMGSDI will set
     a private GCF_GSTK EFS item to FALSE. This EFS item is only applicable
     to GSTK. This is to make sure that GSTK can operate in non-test/non-GCF
     mode while rest of the modem is operating in test mode. We have had known
     cases of OEM conformance failures if GSTK operated in GCF mode.
  3. If card does not contain CSIM, and MCC/MNC is test MCC/MNC, MMGSDI will
     set a private GCF_GSTK EFS item to TRUE. This EFS item is only applicable
     to GSTK.


DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_set_gprs_anite_nv_if_reqd(
  mmgsdi_int_app_info_type *app_info_ptr,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  nv_item_type                    gsdi_nv_gprs_anite_gcf;
  mmgsdi_nv_item_cache_type       mmgsdi_nv_data         = {0};
  boolean                         set_gstk_gcf_nv        = FALSE;
  boolean                         get_gstk_gcf_nv        = FALSE;
  nv_stat_enum_type               nv_status              = NV_DONE_S;
  uim_common_efs_status_enum_type efs_access_status      = UIM_COMMON_EFS_ERROR;
  mmgsdi_slot_data_type          *slot_data_ptr          = NULL;
  uint8                           i                      = 0;

  if (app_info_ptr == NULL)
  {
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if (slot_data_ptr == NULL ||
      slot_data_ptr->app_info_pptr == NULL)
  {
    return;
  }

  for (i = 0; i < MMGSDI_MAX_PROV_APPS; i++)
  {
    if (slot_data_ptr->app_info_pptr[i] == NULL)
    {
      continue;
    }

    if (slot_data_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_CSIM)
    {
      break;
    }
  }

  if (app_info_ptr->gprs_anite_mcc &&
      i == MMGSDI_MAX_PROV_APPS)
  {
    /* Card is a test card and does not contain CSIM app... we will set GSTK
       GCF EFS item to TRUE so that GSTK can operate in GCF/test mode... more
       details in the header of this function */
    set_gstk_gcf_nv = TRUE;
  }

  /* Lets set the GCF NV item, if needed */
  memset(&gsdi_nv_gprs_anite_gcf, 0x00, sizeof(nv_item_type));

  nv_status = mmgsdi_nv_get_item(NV_GPRS_ANITE_GCF_I, &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  if (nv_status != NV_DONE_S ||
      mmgsdi_nv_data.item_value.gprs_anite_gcf != app_info_ptr->gprs_anite_mcc)
  {
    gsdi_nv_gprs_anite_gcf.gprs_anite_gcf = app_info_ptr->gprs_anite_mcc;
    nv_status = mmgsdi_nv_put_item(NV_GPRS_ANITE_GCF_I,
                                   &gsdi_nv_gprs_anite_gcf,
                                   MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
    if (nv_status == NV_DONE_S)
    {
      UIM_MSG_HIGH_1("Updated GPRS ANITE NV to: 0x%x", app_info_ptr->gprs_anite_mcc);
    }
  }

  /* Now set the internal GSTK GCF efs item, if needed */
  efs_access_status = uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_GCF_GSTK_FILE,
                                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                          UIM_COMMON_EFS_DEVICE,
                                          &get_gstk_gcf_nv,
                                          sizeof(get_gstk_gcf_nv));
  if (efs_access_status != UIM_COMMON_EFS_SUCCESS ||
      get_gstk_gcf_nv != set_gstk_gcf_nv)
  {
    (void) uim_common_efs_write(UIM_COMMON_EFS_MMGSDI_GCF_GSTK_FILE,
                                UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                UIM_COMMON_EFS_DEVICE,
                                (const char*) &set_gstk_gcf_nv,
                                sizeof(set_gstk_gcf_nv));
  }
} /* mmgsdi_card_init_set_gprs_anite_nv_if_reqd */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_IS_IMSI_TEST_IMSI

DESCRIPTION
  This function compares the MCC value in IMSI against set of reserved MCC
  values to figure out if it is a test IMSI.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If it is  a test IMSI
  FALSE: Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_card_init_is_imsi_test_imsi(
  mmgsdi_imsi_data_type     imsi
)
{
  uint8                index    = 0;

  UIM_MSG_HIGH_3("MCC value in the IMSI: %x%x%x",
                 imsi.d[0], imsi.d[1], imsi.d[2]);

  /* Compare MCC against the reserved MCC values */
  for(index = 0;
      index < (sizeof(mmgsdi_reserved_mcc_tbl)/
               sizeof(mmgsdi_mcc_data_type));
      index++)
  {
    if(!memcmp(&imsi.d[0],
               &mmgsdi_reserved_mcc_tbl[index],
               sizeof(mmgsdi_mcc_data_type)))
    {
      return TRUE;
    }
  }

  /* Compare the IMSI value with known test IMSIs from TS 31.121 */
  for(index = 0;
      index < (sizeof(mmgsdi_reserved_test_imsi_tbl)/
               sizeof(mmgsdi_imsi_data_type));
      index++)
  {
    if(!memcmp(&imsi,
               &mmgsdi_reserved_test_imsi_tbl[index],
               sizeof(mmgsdi_imsi_data_type)))
    {
      return TRUE;
    }
  }

  return FALSE;
}/* mmgsdi_card_init_is_imsi_test_imsi */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_VERIFY_TDS_IMSI

DESCRIPTION
  This function compares the MCC value in IMSI against set of reserved MCC
  values

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_verify_tds_imsi(
  mmgsdi_imsi_data_type     imsi,
  mmgsdi_int_app_info_type *app_info_ptr
)
{
  uint8  index = 0;

  if(app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("imsi_data_ptr or app_info_ptr is NULL");
    return;
  }

  /* Set TDS test imsi to FALSE by default */
  app_info_ptr->tds_test_imsi = FALSE;

  /* Compare IMSI against the reserved IMSI values */
  for(index = 0;
      index < (sizeof(mmgsdi_reserved_tds_imsi_tbl)/
               sizeof(mmgsdi_imsi_data_type));
      index++)
  {
    if(!memcmp(&imsi,
               &mmgsdi_reserved_tds_imsi_tbl[index],
               sizeof(mmgsdi_imsi_data_type)))
    {
      UIM_MSG_HIGH_0("IMSI match with reserved IMSI found");
      app_info_ptr->tds_test_imsi = TRUE;
      break;
    }
  }
}/* mmgsdi_card_init_verify_tds_imsi */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_UPDATE_AD_MNC_LEN

DESCRIPTION
  This function updates the fourth byte data of EF AD to 0x2 or 0x3 in cache when the
  EF AD only has 3 bytes data.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_update_ad_mnc_len (
  mmgsdi_session_id_type    session_id,
  mmgsdi_data_type          imsi_data
)
{
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_data_type                   ad_data            = {0, NULL};
  mmgsdi_data_type                   new_ad_data        = {0, NULL};
  mmgsdi_access_type                 ad_file_access;
  mmgsdi_int_app_info_type         * app_info_ptr       = NULL;
  mmgsdi_slot_id_enum_type           slot_id            = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                              num_of_mnc_digit   = 0;

  MMGSDIUTIL_RETURN_IF_NULL(imsi_data.data_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot_id,
                                                   NULL,
                                                   &app_info_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (app_info_ptr == NULL) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    return MMGSDI_ERROR;
  }

  memset(&ad_file_access, 0x00, sizeof(ad_file_access));

  ad_file_access.access_method     = MMGSDI_EF_ENUM_ACCESS;
  switch (app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_SIM:
      ad_file_access.file.file_enum     = MMGSDI_GSM_AD;
      break;
    case MMGSDI_APP_USIM:
      ad_file_access.file.file_enum     = MMGSDI_USIM_AD;
      break;
    default:
      return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_read_cache_file(session_id, &ad_file_access, &ad_data);
  if (mmgsdi_status != MMGSDI_SUCCESS || ad_data.data_ptr == NULL)
  {
    /* EF AD might be missing, return success directly */
    return MMGSDI_SUCCESS;
  }

  if(ad_data.data_len >= MMGSDI_EF_AD_LEN &&
    (((ad_data.data_ptr[MMGSDIUTIL_MNC_LEN_BYTE] & 0x0F) == MMGSDIUTIL_MNC_2_DIGITS ) ||
     ((ad_data.data_ptr[MMGSDIUTIL_MNC_LEN_BYTE] & 0x0F) == MMGSDIUTIL_MNC_3_DIGITS )))
  {
    MMGSDIUTIL_TMC_MEM_FREE(ad_data.data_ptr);
    return MMGSDI_SUCCESS;
  }

  if (ad_data.data_len > MMGSDI_EF_AD_LEN)
  {
    new_ad_data.data_len = ad_data.data_len;
  }
  else
  {
    new_ad_data.data_len = MMGSDI_EF_AD_LEN;
  }

  mmgsdi_status = mmgsdi_util_get_num_of_mnc_digits(
                    imsi_data, ad_data, &num_of_mnc_digit);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(ad_data.data_ptr);
    return mmgsdi_status;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_ad_data.data_ptr,
                                     new_ad_data.data_len);
  if (new_ad_data.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(ad_data.data_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Copy the AD data */
  mmgsdi_memscpy(new_ad_data.data_ptr,
                 new_ad_data.data_len,
                 ad_data.data_ptr,
                 ad_data.data_len);

  MMGSDIUTIL_TMC_MEM_FREE(ad_data.data_ptr);

  new_ad_data.data_ptr[MMGSDI_MNC_LEN_OFFSET_IN_EF_AD] = num_of_mnc_digit;

  /* Let's look into our file attr cache and see if we have already cached
     these attributes only if EF-AD new length is different than original. */
  if (ad_data.data_len != new_ad_data.data_len)
  {
    mmgsdi_attr_cache_element_type   * attr_cache_ptr     = NULL;

    mmgsdi_status = mmgsdi_cache_populate_attr_cache_if_needed(
                      mmgsdi_generic_data_ptr->client_id,
                      session_id,
                      slot_id,
                      TRUE,
                      FALSE,
                      &ad_file_access,
                      &attr_cache_ptr,
                      NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      if (attr_cache_ptr != NULL)
      {
        mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
      }
      MMGSDIUTIL_TMC_MEM_FREE(new_ad_data.data_ptr);
      return mmgsdi_status;
    }

    /* update EF AD attr cache size to 4 */
    if (attr_cache_ptr != NULL)
    {
      if (attr_cache_ptr->file_size < MMGSDI_EF_AD_LEN)
      {
        mmgsdi_file_security_access_type security_access;
        memset(&security_access, 0, sizeof(mmgsdi_file_security_access_type));

        mmgsdi_cache_alloc_and_populate_file_attr_cache(attr_cache_ptr->read,
                                                        &(security_access.read));
        mmgsdi_cache_alloc_and_populate_file_attr_cache(attr_cache_ptr->write,
                                                        &(security_access.write));
        mmgsdi_cache_alloc_and_populate_file_attr_cache(attr_cache_ptr->increase,
                                                        &(security_access.increase));

        attr_cache_ptr->file_size = MMGSDI_EF_AD_LEN;
        mmgsdi_cache_attr_write(session_id,
                                slot_id,
                                &ad_file_access,
                                attr_cache_ptr->file_type,
                                attr_cache_ptr->file_size,
                                attr_cache_ptr->rec_len,
                                attr_cache_ptr->num_of_rec,
                                attr_cache_ptr->sfi,
                                &security_access,
                                &attr_cache_ptr->cyclic_increase_allowed);

        MMGSDIUTIL_TMC_MEM_FREE(
          security_access.read.protection_pin_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(
          security_access.write.protection_pin_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(
          security_access.increase.protection_pin_ptr);
      }
      mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
    }
  }

  /* Delete already existing cache if any */
  mmgsdi_status = mmgsdi_cache_delete_item(session_id,
                                           1,
                                           &ad_file_access.file.file_enum);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Write the new AD data to cache as if it came from the card. */
    mmgsdi_status = mmgsdi_cache_write(session_id,
                                       &ad_file_access,
                                       new_ad_data,
                                       0,
                                       MMGSDI_DATA_FROM_CARD_COMPLETE);
  }
  MMGSDIUTIL_TMC_MEM_FREE(new_ad_data.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_card_init_update_ad_mnc_len */


#ifdef FEATURE_UIM_PERSISTENT_CACHE
/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_ACTIVATE_PERSISTENT_CACHE

DESCRIPTION
  Activate the corresponding persistent cache

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_activate_persistent_cache(
  mmgsdi_session_id_type     session_id,
  mmgsdi_data_type           imsi_data
)
{
  uint8                       i                = 0;
  mmgsdi_access_type          access;
  mmgsdi_iccid_info_type      iccid_data;
  mmgsdi_data_type            iccid;
  mmgsdi_slot_id_enum_type    slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_info_type  * session_info_ptr = NULL;
  mmgsdi_cache_init_enum_type cache_state      = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_return_enum_type     mmgsdi_status    = MMGSDI_SUCCESS;

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&iccid, 0x00, sizeof(mmgsdi_data_type));
  memset(&iccid_data, 0x00, sizeof(mmgsdi_iccid_info_type));

  UIM_MSG_HIGH_0("mmgsdi_card_init_activate_persistent_cache");

  /* Get the slot and app info related to the session */
  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (session_info_ptr == NULL) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_1("Could not Session/app info corresponding to Session ID: 0x%x",
                  session_id);
    return;
  }

  /* Activate persistent cache */
  if(session_info_ptr->session_type != MMGSDI_GW_PROV_PRI_SESSION &&
     session_info_ptr->session_type != MMGSDI_1X_PROV_PRI_SESSION)
  {
    UIM_MSG_LOW_1("persistent cache not available for session type 0x%x",
                  session_info_ptr->session_type);
    return;
  }

  /* Get ICCID for persistent cache activation */
  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_ICCID;

  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                        &access,
                                        &iccid.data_len,
                                        0,
                                        &cache_state);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     cache_state == MMGSDI_CACHE_NOT_FOUND ||
     iccid.data_len > MMGSDI_ICCID_LEN)
  {
    UIM_MSG_ERR_2("failed to read cache len for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                  mmgsdi_status, cache_state);
    return;
  }

  iccid.data_ptr = &iccid_data.data[0];

  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &access,
                                    &iccid,
                                    0,
                                    &cache_state);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("failed to read cache for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                  mmgsdi_status, cache_state);
    return;
  }

  iccid_data.length = iccid.data_len;

  mmgsdi_status = mmgsdi_persistent_cache_activate_profile(slot_id,
                                           iccid_data,
                                           session_info_ptr->session_type,
                                           imsi_data);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to activate persistent cache");
  }
}/* mmgsdi_card_init_activate_persistent_cache */
#endif /* FEATURE_UIM_PERSISTENT_CACHE */


/*=============================================================================
FUNCTION: MMGSDI_CARD_INIT_GCF_FLAG_REQUIRED

DESCRIPTION
  This function to check whether GCF flag need to be set or not based on the
  new NV configured.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
=============================================================================*/
static boolean mmgsdi_card_init_gcf_flag_required(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_imsi_data_type              imsi,
  boolean                           *gprs_anite_mcc_ptr)
{
  uim_common_efs_status_enum_type  efs_access_status       = UIM_COMMON_EFS_ERROR;
  uint8                            gcf_anite_nv_value      = 0;
  boolean                          gcf_flag_check_required = FALSE;

  if(gprs_anite_mcc_ptr == NULL)
  {
    return FALSE;
  }

  /* Get the UIM_COMMON_EFS_MMGSDI_GPRS_GCF_ANITE_CONTROL EFS status */
  efs_access_status = uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_GPRS_GCF_ANITE_CONTROL,
                                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                          UIM_COMMON_EFS_CONTEXT_0,
                                          &gcf_anite_nv_value,
                                          sizeof(uint8));
  if (efs_access_status != UIM_COMMON_EFS_SUCCESS ||
      gcf_anite_nv_value > MMGSDI_GPRS_GCF_ANITE_NV_LAST_SIM)
  {
    /* Setting to default values when new NV is not configured (or)
       Configured with values not fall in the range of 0 to 7 */
#ifdef FEATURE_CUST_1
    gcf_anite_nv_value = MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_1;
#else
    gcf_anite_nv_value = MMGSDI_GPRS_GCF_ANITE_NV_LAST_SIM;
#endif /* FEATURE_CUST_1 */
  }

  *gprs_anite_mcc_ptr = mmgsdi_card_init_is_imsi_test_imsi(imsi);
  if(*gprs_anite_mcc_ptr)
  {
    UIM_MSG_HIGH_0("IMSI match with test IMSI found");
  }

  switch(gcf_anite_nv_value)
  {
    case MMGSDI_GPRS_GCF_ANITE_NV_NO_TEST_SIM:
      /* Setting the GCF flag always to FALSE to act as non GCF mode */
      *gprs_anite_mcc_ptr         = FALSE;
      gcf_flag_check_required     = TRUE;
      break;
    case MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_1:
    case MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_2:
    case MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_3:
      /* Set the GCF flag based on the card is in slot 1~3 */
      if ((uint8)(slot_id - MMGSDI_SLOT_1) ==
            (uint8)(gcf_anite_nv_value - MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_SLOT_1))
      {
        gcf_flag_check_required = TRUE;
      }
      break;
    case MMGSDI_GPRS_GCF_ANITE_NV_TEST_SIM_ANY_SLOT:
      {
        /* Setting the GCF flag to TRUE as soon as any GCF card is detected
           else set FALSE if no GCF card is present */
        mmgsdi_session_type_enum_type  session_type      = MMGSDI_MAX_SESSION_TYPE_ENUM;
        mmgsdi_int_app_info_type      *app_info_ptr      = NULL;
        mmgsdi_return_enum_type        status            = MMGSDI_ERROR;
        mmgsdi_session_id_type         mmgsdi_session_id = 0;
        mmgsdi_attribute_enum_type     i                 = MMGSDI_ATTR_UNKNOWN;

        if (*gprs_anite_mcc_ptr)
        {
          return TRUE;
        }

        /* looping through all GW prov sessions and find provisioning USIM which
           is test USIM and return TRUE as soon as found, else return FALSE */
        for (i = MMGSDI_ATTR_PRIMARY; i < MMGSDI_ATTR_UNKNOWN; i++)
        {
          if(MMGSDI_SUCCESS != mmgsdi_util_get_session_type_from_tech_and_attr(
                                                             MMGSDI_TECH_3GPP,
                                                             i,
                                                             &session_type))
          {
            continue;
          }

          if (MMGSDI_SUCCESS != mmgsdi_util_get_mmgsdi_session_id_from_type(
                                                         session_type,
                                                         &mmgsdi_session_id))
          {
            continue;
          }

          /* Get the slot related to the session */
          status = mmgsdi_util_get_session_app_info(mmgsdi_session_id,
                                                    NULL,
                                                    NULL,
                                                    &app_info_ptr,
                                                    NULL,
                                                    NULL,
                                                    NULL);

          if((status != MMGSDI_SUCCESS) || (app_info_ptr == NULL) )
          {
            continue;
          }
          if(app_info_ptr->gprs_anite_mcc)
          {
            /* If we come here it means NV 947 is already set to 1.
               So no further updates required here.*/
            break;
          }
        }
        if(i == MMGSDI_ATTR_UNKNOWN)
        {
          gcf_flag_check_required = TRUE;
        }
      }
      break;
    case MMGSDI_GPRS_GCF_ANITE_NV_LAST_SIM:
      /* Setting the GCF flag based on latest card */
      gcf_flag_check_required = TRUE;
      break;
  }

  return gcf_flag_check_required;
} /* mmgsdi_card_init_gcf_flag_required */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_IMSI_DATA

DESCRIPTION
  This is the validation function for checking IMSI data received from the
  card

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_imsi_data (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{
  uint8                       imsi_len         = 0xFF;
  mmgsdi_imsi_data_type       imsi             = {{0}};
  mmgsdi_int_app_info_type   *app_info_ptr     = NULL;
  mmgsdi_return_enum_type     status           = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type    slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                       i                = 0;

  (void)mmgsdi_status;

  MMGSDIUTIL_RETURN_IF_NULL_2(data_ptr, data_ptr->data_ptr);

  /* Verify IMSI Meets minimum requirements */
  imsi_len = data_ptr->data_ptr[0];

  UIM_MSG_HIGH_2("mmgsdi_card_init_validate_imsi_data IMSI len 0x%x, IMSI data len 0x%x",
                 imsi_len, data_ptr->data_len);

  /* If IMSI is not provisioned then halt initialization */
  if (imsi_len == MMGSDI_IMSI_NOT_PROVISIONED ||
      imsi_len < MMGSDI_MIN_VALID_IMSI_LEN_INTERNAL ||
      imsi_len > MMGSDI_MAX_VALID_IMSI_LEN)
  {
    return MMGSDI_ERROR;
  }

  /* Validate IMSI contents:
     1. Each digit has value [0,9] except bits 1-4 of 1st byte and bits 5-8 of last byte
     2. Last octect values:
        TS 24.008:
        Odd/even indication (octet 3)
        Bit 4
        0 even number of identity digits
        1 odd number of identity digits
        For the IMSI, IMEI and IMEISV this field is coded using BCD coding.
        If the number of identity digits is even then bits 5 to 8 of the last
        octet shall be filled with an end mark coded as "1111".

        To avoid interop issues, we will not check for parity bit, instead,
        for bits 5-8 of last octet, we will consider both 0xF and values in
        the range of [0,9] valid
   */
  for (i = 1; i <= imsi_len; i++)
  {
    if (((i > 1) && ((data_ptr->data_ptr[i] & MMGSDI_IMSI_LOW_BYTE_MASK) > 0x9)) ||
        (((data_ptr->data_ptr[i] & MMGSDI_IMSI_HIGH_BYTE_MASK) > 0x90) &&
         ((i != imsi_len) ||
          ((data_ptr->data_ptr[i] & MMGSDI_IMSI_HIGH_BYTE_MASK) != MMGSDI_IMSI_HIGH_BYTE_MASK)))
        )
    {
      UIM_MSG_ERR_2("Byte 0x%x, Incorrect value 0x%x",
                    i, data_ptr->data_ptr[i]);
      return MMGSDI_ERROR;
    }
  }

  /* Get the slot related to the session */
  status = mmgsdi_util_get_session_app_info(session_id,
                                            &slot_id,
                                            NULL,
                                            &app_info_ptr,
                                            NULL,
                                            NULL,
                                            NULL);

  if((status != MMGSDI_SUCCESS) ||
     (app_info_ptr == NULL) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_1("Could not Session/app info corresponding to Session ID: 0x%x",
                  session_id);
    return MMGSDI_ERROR;
  }

  status = mmgsdi_util_convert_to_imsi_data_type(data_ptr, &imsi);
  if(status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to convert imsi from mmgsdi_data_type to mmgsdi_imsi_data_type");
    return status;
  }

  (void)mmgsdi_card_init_update_ad_mnc_len(session_id, *data_ptr);

  /* Updating gprs_anite_mcc flag and GPRS Anite NV value(NV_GPRS_ANITE_GCF_I)
     based on the new NV settings and resevered set of MCC table */
  if(mmgsdi_card_init_gcf_flag_required(slot_id, imsi, &app_info_ptr->gprs_anite_mcc))
  {
    /* Update GPRS Anite NV value and gprs anite mcc change if needed */
    mmgsdi_card_init_set_gprs_anite_nv_if_reqd(app_info_ptr, slot_id);
  }

  /* Verify if IMSI matches TDS reserved set */
  mmgsdi_card_init_verify_tds_imsi(imsi, app_info_ptr);

  /* Send Test SIM event to all clients */
  mmgsdi_evt_build_and_send_test_sim_status(TRUE,
                                            (mmgsdi_client_id_type)0,
                                            slot_id);

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_imsi_data */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_NETPAR

DESCRIPTION
  This is the validation function for checking EF NETPAR from USIM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_netpar (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{
  mmgsdi_int_app_info_type  *app_info_ptr = NULL;
  mmgsdi_return_enum_type    status       = MMGSDI_ERROR;

  (void)data_ptr;

  UIM_MSG_HIGH_0("mmgsdi_card_init_validate_netpar");

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Get the app related to the session */
    status = mmgsdi_util_get_prov_session_info(session_id,
                                               NULL,
                                               NULL,
                                               &app_info_ptr);

    if((status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      UIM_MSG_ERR_0("Could not get Prov app info");
      return MMGSDI_ERROR;
    }

    /* Update USIM spec. version */
    app_info_ptr->usim_spec_version = MMGSDI_SPEC_VERSION_R99_2000_12;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_netpar */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_RPLMNWACT

DESCRIPTION
  This is the validation function for checking EF RPLMNWACT from USIM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_rplmnwact (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{
  mmgsdi_int_app_info_type *app_info_ptr = NULL;
  mmgsdi_return_enum_type   status       = MMGSDI_ERROR;

  (void)data_ptr;

  UIM_MSG_HIGH_0("mmgsdi_card_init_validate_rplmnwact");

  /* Get the app related to the session */
  status = mmgsdi_util_get_prov_session_info(session_id,
                                             NULL,
                                             NULL,
                                             &app_info_ptr);

  if((status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get Prov app info");
    return MMGSDI_ERROR;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Read on MMGSDI_USIM_NETPAR Failed      */
    /* Read on MMGSDI_USIM_RPLMNACT Succeeded */
    if ( app_info_ptr->usim_spec_version == MMGSDI_VERSION_UNKNOWN )
    {
      app_info_ptr->usim_spec_version = MMGSDI_SPEC_VERSION_R99_2000_09;
    }
  }
  else
  {
    /* Read on MMGSDI_USIM_NETPAR Failed      */
    /* Read on MMGSDI_USIM_RPLMNACT Failed    */
    if ( app_info_ptr->usim_spec_version == MMGSDI_VERSION_UNKNOWN )
    {
      app_info_ptr->usim_spec_version = MMGSDI_SPEC_VERSION_R99_2000_07;
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_rplmnwact */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_HKEY_DATA

DESCRIPTION
  This is the validation function for checking the Hidden Key data received
  from the card

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_hkey_data (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type status,
  const mmgsdi_data_type *data_ptr
)
{
  mmgsdi_int_app_info_type       *app_info_ptr  = NULL;
  mmgsdi_evt_session_notify_type  notify_type;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &notify_type.slot_id,
                                                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get Prov app info");
    return MMGSDI_SUCCESS;
  }

  /* Identical to the PIN verification status update handling during REFRESH,
     Hiddenkey status is cleared and the Hiddenkey event sent out only if
     the app is completely reinitialized (i.e for  REFRESH APP_RESET,
     REFRESH RESET and REFRESH RESET_AUTO modes) or if EFHiddenkey
     is updated as part of a REFRESH FCN.
     For REFRESH modes that do post-pin1 app. init procedures
     (REFRESH INIT, INIT FCN, INIT FFCN, 3G Session RESET),
     Hiddenkey verification status is retained as is, and no
     event is sent out */

  /* Update Hiddenkey status and send out Hiddenkey event only if status
     is not initialized (i.e. during a regular power-up or a cache
     re-initialization due to a REFRESH APP_RESET/ REFRESH RESET/REFRESH
     RESET_AUTO) */
  if(app_info_ptr->hiddenkey.status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
  {
    if(status == MMGSDI_SUCCESS)
    {
      /* Verify if the Hiddenkey data is valid */
      if(mmgsdi_util_is_hiddenkey_from_card_valid(data_ptr))
      {
        app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
      }
      else
      {
        app_info_ptr->hiddenkey.status = MMGSDI_PIN_DISABLED;
      }
    }
    else if(status == MMGSDI_NOT_FOUND)
    {
      UIM_MSG_HIGH_1("Optional Hiddenkey not found: status 0x%x", mmgsdi_status);
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_NOT_FOUND;
    }

    notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
    notify_type.session_id  = session_id;

    /* Send out the Hiddenkey event */
    mmgsdi_evt_build_and_send_pin(notify_type,
                                  MMGSDI_HIDDENKEY,
                                  app_info_ptr->hiddenkey.status,
                                  (const mmgsdi_int_app_info_type*)app_info_ptr);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_hkey_data */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_UPDATE_ACCESS_CLASS

DESCRIPTION
  This function updates the access class with one of the following:
  1) Sets bit b3 to 0 if it is set to 1 in byte1 of EF-ACC
  2) Sets one access class (between 0 and 9) at random if EF-ACC has all access
     class bits set to 0. The new data is written into the cache

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_update_access_class (
  mmgsdi_session_id_type    session_id,
  const mmgsdi_data_type   *acc_data_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_access_type         file_access;
  mmgsdi_data_type           write_data;
  uint8                      rand_num = 0;
  uint8                      acc_data[MMGSDI_EF_ACC_LEN];
  uint8                      acc_byte_offset = 0;
  uint8                      acc_bit_offset = 0;
  mmgsdi_channel_info_type * channel_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(acc_data_ptr);

  if(acc_data_ptr->data_len != MMGSDI_EF_ACC_LEN)
  {
    UIM_MSG_ERR_0( "ACC data length is invalid");
    return MMGSDI_INCORRECT_PARAMS;
  }

  memset(acc_data, 0, sizeof(acc_data));
  memset(&write_data, 0, sizeof(write_data));

  /* Copy the ACC data */
  mmgsdi_memscpy(acc_data,
                 sizeof(acc_data),
                 acc_data_ptr->data_ptr,
                 acc_data_ptr->data_len);

  /* According to TS 31.102 Section 4.2.15, USIM is invalid if bit b3
     of byte 1 is set to 1. Some non-complaint commercial cards
     come with bit b3 set with 1. To support them, bit b3 is set to 0 in
     cache to continue the initialization */
  if((acc_data[0] & MMGSDI_EF_ACC_BIT3_MASK) != 0)
  {
    UIM_MSG_HIGH_0( "Bit for Access Class 10 is set, Resetting it");
    acc_data[0] &= ~MMGSDI_EF_ACC_BIT3_MASK;
  }

  /* Select an access class between 0 and 9 randomly and set the corresponding
     bit in ACC data. Access classes 0 through 7 are on byte 1 and classes 8
     and 9 are on byte 0 of EF-ACC (with LSBit corresponding to the numerically
     lowest access class) */
  if(acc_data[0] == 0 && acc_data[1] == 0)
  {
    UIM_MSG_HIGH_0("ACC data is invalid; assigning access class at random");
    if(secapi_get_random(SECAPI_SECURE_RANDOM, &rand_num, 1) != E_SUCCESS)
    {
      /* Assign class 0 in the unlikely case of secapi_get_random failing */
      rand_num = 0;
    }
    rand_num = rand_num % 10;
    acc_byte_offset = (rand_num < 8) ? 1 : 0;
    acc_bit_offset = rand_num % 8;
    acc_data[acc_byte_offset] |= (1 << acc_bit_offset);
  }

  write_data.data_ptr = acc_data;
  write_data.data_len = MMGSDI_EF_ACC_LEN;

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(session_id,
                                                           NULL,
                                                           &channel_info_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
     channel_info_ptr == NULL ||
     channel_info_ptr->app_info_ptr == NULL)
  {
    return mmgsdi_status;
  }

  memset(&file_access, 0, sizeof(file_access));
  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  if(channel_info_ptr->app_info_ptr->app_data.app_type == MMGSDI_APP_SIM )
  {
    file_access.file.file_enum = MMGSDI_GSM_ACC;
  }
  else
  {
    file_access.file.file_enum = MMGSDI_USIM_ACC;
  }

  /* Write the new ACC data to cache as if it came from the card. */
  return mmgsdi_cache_write(session_id,
                            &file_access,
                            write_data,
                            0,
                            MMGSDI_DATA_FROM_CARD_COMPLETE);
} /* mmgsdi_card_init_update_access_class */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_ACC_DATA

DESCRIPTION
  This is the validation function for checking ACC data received from the
  card

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_acc_data (
  mmgsdi_session_id_type    session_id,
  mmgsdi_return_enum_type   mmgsdi_status,
  const mmgsdi_data_type   *acc_data_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(acc_data_ptr);

  /* Verify that the input status is success */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
     UIM_MSG_ERR_1( "mmgsdi_status failure: 0x%x", mmgsdi_status);
     return mmgsdi_status;
  }

  /* Verify that the read ACC data pointer is valid */
  if (acc_data_ptr->data_ptr == NULL)
  {
     UIM_MSG_ERR_0( "ACC data pointer is NULL");
     return MMGSDI_INCORRECT_PARAMS;
  }

  /* Verify that the read ACC data length is valid */
  if (acc_data_ptr->data_len != MMGSDI_EF_ACC_LEN)
  {
    UIM_MSG_ERR_0( "ACC data length is invalid");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Verify that at least one Access Class is set except bit b3 of first byte*/
  if(((acc_data_ptr->data_ptr[0] & MMGSDI_EF_ACC_BIT3_MASK) == 0) &&
      (acc_data_ptr->data_ptr[0] != 0 || acc_data_ptr->data_ptr[1] != 0))
  {
    return MMGSDI_SUCCESS;
  }

  return mmgsdi_card_init_update_access_class(session_id, acc_data_ptr);
} /* mmgsdi_card_init_validate_acc_data */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_DISABLE_SRV_IN_UST

DESCRIPTION
  This function is called to disable any usim service in EF UST

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_disable_srv_in_ust (
  mmgsdi_session_id_type   session_id,
  mmgsdi_service_enum_type service_to_disable
)
{
  uint8                   mask           = 0x01;
  uint8                   shift          = 0x0;
  uint32                  data_len       = 0;
  uint32                  offset         = 0x00;
  uint32                  service        = 0;
  mmgsdi_return_enum_type mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_access_type      access;
  mmgsdi_data_type        data;

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&data, 0x00, sizeof(mmgsdi_data_type));
  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_USIM_UST;

  if ((service_to_disable & 0xFF00) != MMGSDI_USIM_SRV)
  {
    return MMGSDI_ERROR;
  }

  /* Get the cache length */
  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                        &access,
                                        &data.data_len,
                                        0,
                                        NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS || data.data_len <= 0)
  {
    return mmgsdi_status;
  }

  data_len = int32touint32(data.data_len);

  /* memory allocate the data with the data_len size */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data.data_ptr, data_len);

  if(data.data_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &access,
                                    &data,
                                    0,
                                    NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* not cached yet */
    MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
    return mmgsdi_status;
  }

  service = service_to_disable - MMGSDI_USIM_SRV;
  offset = service/8;
  shift = (uint8)service%8;
  mask = mask << shift;

  if(offset < int32touint32(data.data_len))
  {
    /* Set service status bit to 0 */
    data.data_ptr[offset] = data.data_ptr[offset] & (~mask);

    /* Update cache to indicate that EPS MM info service is not available */
    mmgsdi_status = mmgsdi_cache_write(session_id,
                                       &access,
                                       data,
                                       0,
                                       MMGSDI_DATA_FROM_CARD_COMPLETE);

    /* Return success if updation of service has failed. This is required to
       continue MMGSDI initialization */
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
      return MMGSDI_SUCCESS;
    }
  }
  MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
  return mmgsdi_status;
}/* mmgsdi_card_init_disable_srv_in_ust */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_DISABLE_EPS_MMI_SRV

DESCRIPTION
  This function is called to disable EPS MMI service 85 when EF-EPSLOCI length is
  not 18 or EF-EPSNSC length is less than 54

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_disable_eps_mmi_srv(
  mmgsdi_session_id_type   session_id
)
{
  mmgsdi_return_enum_type mmgsdi_status  = MMGSDI_ERROR;

  mmgsdi_status = mmgsdi_card_init_disable_srv_in_ust(session_id, MMGSDI_USIM_SRV_EPS_MMI);
  if( mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_file_enum_type  mmgsdi_file[2] = {MMGSDI_USIM_EPSLOCI,
                                             MMGSDI_USIM_EPSNSC};

    /* If EPS MMI service is updated successfully to cache then delete EPSLOCI
      & EPSNSC cache */
    (void)mmgsdi_cache_delete_item(session_id,
                                   (uint16)(sizeof(mmgsdi_file) / sizeof(mmgsdi_file_enum_type)),
                                   mmgsdi_file);

  }
  return mmgsdi_status;
}/* mmgsdi_card_init_disable_eps_mmi_srv */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_EPSNSC

DESCRIPTION
  This is the validation function for checking EF EPSNSC from USIM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_epsnsc (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{
  UIM_MSG_HIGH_0("mmgsdi_card_init_validate_epsnsc");

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      (data_ptr != NULL &&
       data_ptr->data_len < MMGSDI_EF_EPSNSC_LEN))
  {
    /* Disable EPS MMI service 85 if caching fails or EF-EPSNSC length
       is less than 54 */
    return mmgsdi_card_init_disable_eps_mmi_srv(session_id);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_epsnsc */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_EPSLOCI

DESCRIPTION
  This is the validation function for checking EF EPSLOCI from USIM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_epsloci (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{
  UIM_MSG_HIGH_0("mmgsdi_card_init_validate_epsloci");

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      (data_ptr != NULL &&
       data_ptr->data_len != MMGSDI_EF_EPSLOCI_LEN))
  {
    /* Disable EPS MMI service 85 if caching fails or EF-EPSLOCI length
       is not 18 */
    return mmgsdi_card_init_disable_eps_mmi_srv(session_id);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_epsloci */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_UPDATE_USIM_GSM_AUTH_EF_DATA

DESCRIPTION
  This function updates default data for different auth related EFs like
  USIM EF KEYS/KEYSPS or GSM EF KC/KCGPRS cache.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_update_usim_gsm_auth_ef_data (
  mmgsdi_session_id_type  session_id,
  mmgsdi_file_enum_type   file_enum
)
{
  mmgsdi_access_type         file_access;
  mmgsdi_data_type           write_data;
  union {
    uint8                    usim_keys_data[MMGSDI_USIM_EF_KEYS_LEN];
    uint8                    gsm_kc_data[MMGSDI_GSM_EF_KEYS_LEN];
  } data;

  mmgsdi_slot_id_enum_type           slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_file_security_access_type  *security_access_ptr = NULL;
  mmgsdi_file_attributes_type        file_attr;

  memset(&file_attr, 0x00, sizeof(file_attr));

  UIM_MSG_HIGH_0("mmgsdi_card_init_update_usim_gsm_auth_ef_data");

  memset(&write_data, 0, sizeof(write_data));

  /* In some of the non-spec compliant cards, USIM KEYS/KEYSPS or GSM KC
     /KCGRPS EFs might be missing or read on these EFs fail. In order for NAS
     to handle AUTH requests, these EFs are expected to be present, to mimic
     the presence of these EFs, MMGSDI shall update the cache with default data
     with first byte as 07 and rest as 0xFF -According to Annx E 3GPP 31.102. for KEYS/KEYSPS
     and with last byte as 07 and rest as 0xFF-According to Annx D 3GPP 51.011 for GSM KC/KCGPRS

     Any subsequent write to cache from NAS shall fail when the command goes
     to the card and this temporary cache is cleared when the application specific
     cache is cleared during ME power down, card reset or NAA Refresh */
  if (file_enum == MMGSDI_USIM_KEYS ||
      file_enum == MMGSDI_USIM_KEYSPS)
  {
    memset(data.usim_keys_data, 0xFF, sizeof(data.usim_keys_data));
    data.usim_keys_data[0] = 0x07;
    write_data.data_ptr = data.usim_keys_data;
    write_data.data_len = MMGSDI_USIM_EF_KEYS_LEN;
  }
  else if (file_enum == MMGSDI_GSM_KC ||
           file_enum == MMGSDI_GSM_KCGPRS)
  {
    memset(data.gsm_kc_data, 0xFF, sizeof(data.gsm_kc_data));
    data.gsm_kc_data[MMGSDI_GSM_EF_KEYS_LEN - 1] = 0x07;
    write_data.data_ptr = data.gsm_kc_data;
    write_data.data_len = MMGSDI_GSM_EF_KEYS_LEN;
  }
  else
  {
    return MMGSDI_SUCCESS;
  }

  memset(&file_access, 0, sizeof(file_access));
  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_enum;

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot_id,
                                                   NULL, NULL, NULL, NULL, NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  file_attr.file_type = MMGSDI_TRANSPARENT_FILE;
  mmgsdi_status = mmgsdi_file_set_default_sec_attr_per_spec(&file_attr,
                                                            file_enum);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  security_access_ptr = &file_attr.file_info.transparent_file.file_security;

  /* Write the attributes fetched from the static tables to the file attributes cache */
  mmgsdi_status = mmgsdi_cache_attr_write(session_id,
                                          slot_id,
                                          &file_access,
                                          MMGSDI_TRANSPARENT_FILE,
                                          write_data.data_len,
                                          0,
                                          0,
                                          UIM_INVALID_SFI,
                                          security_access_ptr,
                                          NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  return mmgsdi_cache_write(session_id,
                            &file_access,
                            write_data,
                            0,
                            MMGSDI_DATA_FROM_CARD_COMPLETE);
} /* mmgsdi_card_init_update_usim_gsm_auth_ef_data */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_KEYS

DESCRIPTION
  This is the validation function for checking EF KEYS from USIM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_keys (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_validate_keys - updating cache");
    return mmgsdi_card_init_update_usim_gsm_auth_ef_data(session_id,
             MMGSDI_USIM_KEYS);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_keys */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_KEYSPS

DESCRIPTION
  This is the validation function for checking EF KEYSPS from USIM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_keysps (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_validate_keysps - updating cache");
    return mmgsdi_card_init_update_usim_gsm_auth_ef_data(session_id,
             MMGSDI_USIM_KEYSPS);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_keysps */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_USIM_KC_KCGPRS

DESCRIPTION
  This is the validation function for checking EF KC/KCGPRS from USIM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_usim_kc_kcgprs(
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{
  /* If the KC/KCGPRS read fails, disable the GSM access service n27 in UST */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_validate_kc_kcgprs - update n27 SRV in UST");
    return mmgsdi_card_init_disable_srv_in_ust(session_id, MMGSDI_USIM_SRV_GSM_ACCESS);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_usim_kc_kcgprs */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_GSM_KC

DESCRIPTION
  This is the validation function for checking GSM EF KC from GSM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_gsm_kc (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_validate_gsm_kc - update cache");
    return mmgsdi_card_init_update_usim_gsm_auth_ef_data(session_id,
             MMGSDI_GSM_KC);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_gsm_kc */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_VALIDATE_GSM_KCGPRS

DESCRIPTION
  This is the validation function for checking GSM EF KCGPRS from GSM app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_validate_gsm_kcgprs (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type mmgsdi_status,
  const mmgsdi_data_type *data_ptr
)
{

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_validate_gsm_kcgprs - update cache");
    return mmgsdi_card_init_update_usim_gsm_auth_ef_data(session_id,
             MMGSDI_GSM_KCGPRS);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_validate_gsm_kcgprs */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_APP_CACHE

DESCRIPTION
  This utility function will go through the sequence of files passed in,
  issues the read/select command to the card which results in the corresponding
  cache being populated

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_app_cache(
  mmgsdi_session_id_type              session_id,
  mmgsdi_app_enum_type                app_type,
  mmgsdi_slot_id_enum_type            slot,
  const mmgsdi_cache_init_order_type *app_init_table_ptr,
  uint32                              num_entry
)
{
  uint32                                  i               = 0;
  mmgsdi_data_type                        returned_data;
  mmgsdi_return_enum_type                 mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_session_closed_cause_enum_type   cause           = MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;

  memset(&returned_data, 0x00, sizeof(mmgsdi_data_type));

  UIM_MSG_MED_1("mmgsdi_card_init_app_cache with num_entry 0x%x", num_entry);

  MMGSDIUTIL_RETURN_IF_NULL(app_init_table_ptr);

  for (i = 0; i < num_entry; i++)
  {
    if(app_init_table_ptr[i].init_cache_func == NULL)
    {
      if(app_init_table_ptr[i].mandatory)
      {
        UIM_MSG_ERR_0("Null init cache func ptr for Mandatory file, Error");
        return MMGSDI_ERROR;
      }
      continue;
    }

    /* Calling either mmgsdi_card_init_cache_binary or
       mmgsdi_card_init_cache_record or
       mmgsdi_card_init_select_only or
       mmgsdi_card_init_read_prl
       the returned_data.data_ptr is being allocated by the init functions */
    mmgsdi_status = app_init_table_ptr[i].init_cache_func(
                      session_id,
                      app_type,
                      slot,
                      app_init_table_ptr[i].mmgsdi_file,
                      &returned_data);

    if (app_init_table_ptr[i].mandatory)
    {
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_2("Init file 0x%x failed with mmgsdi status 0x%x",
                      app_init_table_ptr[i].mmgsdi_file, mmgsdi_status);

        /* Update the session closed cause. The session changed event
           broadcasts the cause */
        cause = mmgsdi_card_init_map_file_to_session_closed_cause(app_init_table_ptr[i].mmgsdi_file,
                                                                  FALSE);
        mmgsdi_util_update_session_closed_cause(session_id, cause);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(returned_data.data_ptr);
        return MMGSDI_ERROR;
      }
    }
    else
    {
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_2("Init file 0x%x failed with mmgsdi status 0x%x",
                      app_init_table_ptr[i].mmgsdi_file, mmgsdi_status);
      }

      if ( mmgsdi_status == MMGSDI_NOT_SUPPORTED ||
           mmgsdi_status == MMGSDI_NOT_FOUND ||
           mmgsdi_status == MMGSDI_EF_INCONSISTENT ||
           mmgsdi_status == MMGSDI_INCORRECT_PARAMS)
      {
        /* do not reset status here because validation function may require the
           info */
      }
      else if (mmgsdi_status != MMGSDI_SUCCESS &&
               (app_init_table_ptr[i].mmgsdi_file == MMGSDI_USIM_EPSLOCI ||
                app_init_table_ptr[i].mmgsdi_file == MMGSDI_USIM_EPSNSC  ||
                app_init_table_ptr[i].mmgsdi_file == MMGSDI_USIM_KEYS    ||
                app_init_table_ptr[i].mmgsdi_file == MMGSDI_USIM_KEYSPS  ||
                app_init_table_ptr[i].mmgsdi_file == MMGSDI_USIM_KC      ||
                app_init_table_ptr[i].mmgsdi_file == MMGSDI_USIM_KCGPRS  ||
                app_init_table_ptr[i].mmgsdi_file == MMGSDI_GSM_KC       ||
                app_init_table_ptr[i].mmgsdi_file == MMGSDI_GSM_KCGPRS))
      {
        /* Ignore errors for these files to allow the validation function
           to disable the service or update the cache and continue initialization */
      }
      else if ((mmgsdi_status != MMGSDI_SUCCESS)
               && mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_IGNORE_INIT_ERROR, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_DISABLED)
      {
        /* Update the session closed cause. The session changed event
           broadcasts the cause */
        cause = mmgsdi_card_init_map_file_to_session_closed_cause(app_init_table_ptr[i].mmgsdi_file,
                                                                  FALSE);
        mmgsdi_util_update_session_closed_cause(session_id, cause);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(returned_data.data_ptr);
        return MMGSDI_ERROR;
      }
    }

    if(app_init_table_ptr[i].validation_func)
    {
      mmgsdi_status = app_init_table_ptr[i].validation_func(session_id,
                                                            mmgsdi_status,
                                                            &returned_data);

      /* If the file is optional and it is missing, the validation function
         shall return success or maintain the previous error code.
         In this case, we can consider it a success. */
      if (app_init_table_ptr[i].mandatory == FALSE &&
          (mmgsdi_status == MMGSDI_NOT_SUPPORTED || mmgsdi_status == MMGSDI_NOT_FOUND))
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }
    }
    else
    {
      /* If no need to validate, it must be success if we reach this point */
      mmgsdi_status = MMGSDI_SUCCESS;
    }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
    /* Each cache profile in the persistent cache is identified by slot, ICCID,
       session type (USIM / CSIM) and IMSI/IMSI_M values. New persistent cache
       profile is created (if profile is not yet in place) after IMSI / IMSI_M
       is read used to activate persistent cache */
    if(mmgsdi_status == MMGSDI_SUCCESS &&
       (app_init_table_ptr[i].mmgsdi_file == MMGSDI_USIM_IMSI ||
        app_init_table_ptr[i].mmgsdi_file == MMGSDI_CSIM_IMSI_M))
    {
      mmgsdi_card_init_activate_persistent_cache(session_id,
                                                 returned_data);
    }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(returned_data.data_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("gsdi status not success 0x%x", mmgsdi_status);

      /* Halt the initialization without closing the session */
      if(mmgsdi_status == MMGSDI_HALT_INIT_ERROR)
      {
        return mmgsdi_status;
      }

      /* Update the session closed cause. The session changed event
         broadcasts the cause */
      cause = mmgsdi_card_init_map_file_to_session_closed_cause(app_init_table_ptr[i].mmgsdi_file,
                                                                TRUE);
      mmgsdi_util_update_session_closed_cause(session_id, cause);
      return mmgsdi_status;
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_app_cache */


#ifdef FEATURE_MMGSDI_3GPP2
/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_GET_MEID_USAGE_TYPE

DESCRIPTION
  This function returns esn usage type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_card_init_get_meid_usage_type(
  mmgsdi_session_id_type        session_id,
  mmgsdi_app_enum_type          app_type,
  uint8                         usage_ind_data,
  boolean                       nv_meid_avail,
  nvruim_esn_usage_type        *usage_type_ptr
)
{
  uint8                   usage_ind       = 0;
  boolean                 sf_euimid_avail = FALSE;
  mmgsdi_return_enum_type mmgsdi_status   = MMGSDI_SUCCESS;

  if (usage_type_ptr == NULL)
  {
    return;
  }

  /* If ME assigned to ESN, ignore SF_EUIMID and bit 2 of Usage Indicator File*/
  if (!nv_meid_avail)
  {
    usage_ind = (usage_ind_data & GSDI_RUIM_ESN_USAGE_MASK);
    /* Check usage indicator bit 1.
      If set to 1, use UIM ID
      If set to 0, user ESN */
    if (usage_ind)
    {
      *usage_type_ptr = NV_RUIM_USE_RUIM_ID;
    }
    else
    {
      *usage_type_ptr = NV_RUIM_USE_ESN;
    }
    return;
  }

  switch(app_type)
  {
   case MMGSDI_APP_RUIM:
     mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_CDMA_SRV_SF_EUIMID,
                                                  &sf_euimid_avail);
     break;
   case MMGSDI_APP_CSIM:
     mmgsdi_status = mmgsdi_uicc_chk_srv_available(
       mmgsdi_generic_data_ptr->client_id,
       session_id,
       MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
       MMGSDI_CSIM_SRV_SF_EUIMID,
       &sf_euimid_avail);
     break;
   default:
     return;
  }

  if ((mmgsdi_status == MMGSDI_SUCCESS) && sf_euimid_avail)
  {
    usage_ind = (usage_ind_data & GSDI_RUIM_SF_EUIMID_USAGE_MASK);
    switch (usage_ind)
    {
      case GSDI_RUIM_MEID_UIMID_USAGE_IND:
        /* Use MEID for identification and UIMID for authentication */
        *usage_type_ptr = NV_RUIM_USE_MEID_UIMID;
        break;
      case GSDI_RUIM_SF_EUIMID_ESN_USAGE_IND:
        /* Use SF_EUIMID for identification and ESN for authentication */
        *usage_type_ptr = NV_RUIM_USE_SF_EUIMID_ESN;
        break;
      case GSDI_RUIM_SF_EUIMID_USAGE_IND:
        /* Use SF_EUIMID for identification and UIMID for authentication */
        *usage_type_ptr = NV_RUIM_USE_SF_EUIMID;
        break;
      default:
        /* Use MEID for identification and ESN for authentication */
        *usage_type_ptr = NV_RUIM_USE_MEID;
        break;
    } /* switch usage_ind */
  } /* if sf_euimid activated */
  else
  {
    /* Handle LF_EUIMID/RUIMID card */
    if (usage_ind_data & GSDI_RUIM_ESN_USAGE_MASK )
    {
      /* Use MEID for identification and UIMID for authentication */
      *usage_type_ptr = NV_RUIM_USE_MEID_UIMID;
    }
    else
    {
      /* Use MEID for identification and ESN for authentication */
      *usage_type_ptr = NV_RUIM_USE_MEID;
    }
  }
} /* mmgsdi_card_init_get_meid_usage_type */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_UPDATE_ESN_ME_STATUS

DESCRIPTION
  This function reads ESN/MEID in NV and in the card. It updates the ESN
  status by comparing the ESN data. This requirement is to satisfy a specific
  MNO where the store ESN is done after auto registration SMS is sent
  successfully. This is invoked to update the ESN status locally based on ESN
  data in the card.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_card_init_update_esn_me_status(
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_read_req_type    *read_req_ptr  = NULL;
  mmgsdi_read_cnf_type    *read_cnf_ptr  = NULL;
  mmgsdi_slot_data_type   *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if (slot_data_ptr == NULL ||
      (slot_data_ptr->protocol != MMGSDI_ICC &&
       slot_data_ptr->protocol != MMGSDI_UICC))
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
    sizeof(mmgsdi_read_req_type));
  if (read_req_ptr == NULL)
  {
    return;
  }

  /* Read ESN_ME from card */
  read_req_ptr->request_header.session_id        = session_id;
  read_req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  read_req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  read_req_ptr->request_header.slot_id           = slot_id;
  read_req_ptr->request_header.client_data       = 0;
  read_req_ptr->request_header.response_cb       = NULL;
  read_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  read_req_ptr->rec_num                          = 0;
  read_req_ptr->offset                           = 0;
  read_req_ptr->file_type                        = MMGSDI_TRANSPARENT_FILE;

  if(slot_data_ptr->protocol == MMGSDI_UICC)
  {
    read_req_ptr->access.file.file_enum          = MMGSDI_CSIM_ESN_ME;

  mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                   TRUE,
                                                   &read_cnf_ptr);
  }
  else if(slot_data_ptr->protocol == MMGSDI_ICC)
  {
    read_req_ptr->access.file.file_enum          = MMGSDI_CDMA_ESN_ME;

    mmgsdi_status = mmgsdi_uim_icc_read_transparent(read_req_ptr,
                                                    TRUE,
                                                    &read_cnf_ptr);
  }

  MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
  if (read_cnf_ptr == NULL)
  {
    return;
  }

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS)
  {
    void                            *esn_meid_data_ptr  = NULL;
    mmgsdi_esn_me_change_enum_type   esn_me_cng_flag    = MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED;
    mmgsdi_esn_meid_info_type        esn_meid_info;

    memset(&esn_meid_info, 0x00, sizeof(esn_meid_info));

    /* Get ESN / MEID information from from NV */
    mmgsdi_status = mmgsdi_util_get_esn_info(session_id, &esn_meid_info);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return;
    }

    if(esn_meid_info.meid_available)
    {
      esn_meid_data_ptr = &esn_meid_info.meid;
    }
    else
    {
      esn_meid_data_ptr = &esn_meid_info.esn;
    }

    /* If both ESNs match then update ESN change flag to "NO_CHANGE"
       If both ESNs does not match then update ESN to "UPDATE_PENDING.
       This denotes that ESN has changed and yet to store into card.
       MMODE calls store ESN API to store the ESN after auto reg SMS is successful.
       In the response, the ESN change flag is updated to "CHANGE" */
    if(esn_meid_info.esn_length == read_cnf_ptr->read_data.data_ptr[0] &&
       read_cnf_ptr->read_data.data_len >= esn_meid_info.esn_length + 1 &&
       memcmp(esn_meid_data_ptr,
              &read_cnf_ptr->read_data.data_ptr[1],
              esn_meid_info.esn_length) == 0)
    {
      esn_me_cng_flag = MMGSDI_ESN_ME_NO_CHANGE;
    }
    else
    {
      esn_me_cng_flag = MMGSDI_ESN_ME_UPDATE_PENDING;
    }

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    slot_data_ptr->esn_me_change_flag = esn_me_cng_flag;
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)read_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
} /* mmgsdi_card_init_update_esn_me_status */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_UPDATE_ESN_ESN_USAGE_TYPE

DESCRIPTION
  This function reads usage indicator file and updates the esn usage type

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/

static mmgsdi_return_enum_type mmgsdi_card_init_update_esn_usage_type(
  mmgsdi_session_id_type   session_id
)
{
  mmgsdi_return_enum_type     mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type   *app_info_ptr      = NULL;
  mmgsdi_session_info_type   *session_info_ptr  = NULL;
  mmgsdi_slot_id_enum_type    slot              = MMGSDI_MAX_SLOT_ID_ENUM;
  nvruim_esn_usage_type       ruim_usage_type   = NV_RUIM_USE_ESN;
  mmgsdi_file_enum_type       usgind_file       = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_data_type            data_buf;

  memset(&data_buf, 0x00, sizeof(mmgsdi_data_type));

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                           session_id,
                           &slot,
                           NULL,
                           &app_info_ptr,
                           NULL,
                           &session_info_ptr,
                           NULL);

  if(mmgsdi_status    != MMGSDI_SUCCESS          ||
     app_info_ptr     == NULL                    ||
     slot             == MMGSDI_MAX_SLOT_ID_ENUM ||
     session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  switch(app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_RUIM:
      usgind_file = MMGSDI_CDMA_UIM_ID_USAGE_IND;
      break;
    case MMGSDI_APP_CSIM:
      usgind_file = MMGSDI_CSIM_USGIND;
      break;
    default:
      return MMGSDI_ERROR;
  }

  /* EF UIM_ID Usage Indicator */
  /* This assumes that the UIM ID Usage IND has not been initialized or
     has been invalidated because the store ESN command could have changed
     the USAGE IND EF.  So we need the command to go to the card and
     update the cache image if already cached */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                app_info_ptr->app_data.app_type,
                                                slot,
                                                usgind_file,
                                                &data_buf);

  if ((mmgsdi_status == MMGSDI_SUCCESS) && data_buf.data_ptr)
  {
    mmgsdi_meid_data_type      meid_nv_data = mmgsdi_get_meid();

    mmgsdi_card_init_get_meid_usage_type(
      session_id,
      app_info_ptr->app_data.app_type,
      data_buf.data_ptr[0],
      meid_nv_data.meid_available,
      &ruim_usage_type);

    nvruim_set_esn_usage(ruim_usage_type);
  }

  MMGSDIUTIL_TMC_MEM_FREE(data_buf.data_ptr);

  return mmgsdi_status;
} /* mmgsdi_card_init_update_esn_usage_type */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_STORE_ESN

DESCRIPTION
  This function builds the store ESN command to the card based on NV and card
  MEID and ESN support.
  Upon successfully sending the store ESN command, it also initializes the
  cache for UIM ID Usage file.
  The procedure for ESN support is as follow:
  1) If ME assigns MEID and the card's service table bit #9 (MEID support) is
     enabled, the ME should issue a Store ESN with MEID (i.e., P1 == 1)
  2) Otherwise, the ME should issue a Store ESN with ESN (i.e., P1 == 0)
     Note, if ME assigns MEID but the card's service table bit #9 is not allocated
     and activated, the ME should store the ESN with pseudo ESN.  (ESN and pseudo
     ESN is abstracted from NV client)

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_store_esn(
  mmgsdi_session_store_esn_req_type *store_esn_req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type           slot              = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_store_esn_cnf_type *cnf_ptr           = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr      = NULL;
  mmgsdi_session_info_type          *session_info_ptr  = NULL;
  mmgsdi_slot_data_type             *slot_data_ptr     = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(store_esn_req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    store_esn_req_ptr->request_header.session_id,
                    &slot,
                    NULL,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  if(mmgsdi_status    != MMGSDI_SUCCESS          ||
     app_info_ptr     == NULL                    ||
     slot             == MMGSDI_MAX_SLOT_ID_ENUM ||
     session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL ||
     mmgsdi_util_is_1x_prov_session(session_info_ptr->session_type) == FALSE)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_esn_info(store_esn_req_ptr->request_header.session_id,
                                           &store_esn_req_ptr->esn_meid_info);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  switch(app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_uim_common_store_esn(
                        store_esn_req_ptr, TRUE, &cnf_ptr, MMGSDI_ICC);
      break;
    case MMGSDI_APP_CSIM:
      mmgsdi_status = mmgsdi_uim_common_store_esn(
                        store_esn_req_ptr, TRUE, &cnf_ptr, MMGSDI_UICC);
      break;
    default:
      return MMGSDI_ERROR;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      cnf_ptr == NULL ||
      cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {
    if(cnf_ptr)
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Store ESN procedure Failed cnf status: 0x%x",
                               cnf_ptr->response_header.mmgsdi_status);
      mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
      return mmgsdi_status;
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_2("Store ESN procedure Failed: 0x%x or null cnf_ptr 0x%x",
                               mmgsdi_status, cnf_ptr);
      return MMGSDI_ERROR;
    }
  }

  /* Send confirmation if the request is originated by client */
  if(store_esn_req_ptr->request_header.response_cb != NULL)
  {
    mmgsdi_sw_status_type status_word = {0, 0};

    status_word.sw1 = cnf_ptr->response_header.status_word.sw1;
    status_word.sw2 = cnf_ptr->response_header.status_word.sw2;

    (void)mmgsdi_cnf_build_and_queue(cnf_ptr->response_header.mmgsdi_status,
                                     &store_esn_req_ptr->request_header,
                                     NULL,
                                     NULL,
                                     cnf_ptr->response_header.status_word.present,
                                     status_word);
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);

  return mmgsdi_card_init_update_esn_usage_type(store_esn_req_ptr->request_header.session_id);
} /* mmgsdi_card_init_store_esn */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_1X_SRV_NVRUIM_INIT

DESCRIPTION
  This function initializes the control flags in nvruim

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_card_init_1x_srv_nvruim_init(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
)
{
  boolean                       srv_avail                = FALSE;
  boolean                       support_an_hrpd          = FALSE;
  boolean                       support_mip              = FALSE;
  boolean                       support_sip              = FALSE;
  boolean                       service_3gpd_extensions  = FALSE;
  boolean                       support_lbs              = FALSE;
  boolean                       support_bcsms            = FALSE;
  boolean                       support_smscap           = FALSE;
  mmgsdi_client_id_type         client_id                = mmgsdi_generic_data_ptr->client_id;
  mmgsdi_data_type              mmgsdi_data_buf;
  uint16                        imsi_mcc                 = 0;
  mmgsdi_return_enum_type       mmgsdi_status            = MMGSDI_ERROR;
  boolean                       non_3gpd_cdma_card       = FALSE;
  mmgsdi_nv_context_type        nv_context               = MMGSDI_NV_CONTEXT_INVALID;
  mmgsdi_session_type_enum_type session_type             = MMGSDI_MAX_SESSION_TYPE_ENUM;

  UIM_MSG_HIGH_1("In mmgsdi_card_init_1x_srv_nvruim_init, slot: 0x%x", slot);

  /* check for non 3gpd cdma card */
  memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));
  if (app_type == MMGSDI_APP_CSIM)
  {
    mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                  app_type,
                                                  slot,
                                                  MMGSDI_CSIM_IMSI_M,
                                                  &mmgsdi_data_buf);
  }
  else
  {
    mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                  app_type,
                                                  slot,
                                                  MMGSDI_CDMA_IMSI_M,
                                                  &mmgsdi_data_buf);
  }
  if ((mmgsdi_status == MMGSDI_SUCCESS) && ( mmgsdi_data_buf.data_ptr != NULL)
      && (mmgsdi_data_buf.data_len == MMGSDI_CDMA_IMSI_LEN))
  {
    /* Read the MCC code for non 3gpd cdma card */
    imsi_mcc = (mmgsdi_data_buf.data_ptr[MMGSDI_CDMA_IMSI_LEN - 1] << 8);  // MSB
    imsi_mcc |= mmgsdi_data_buf.data_ptr[MMGSDI_CDMA_IMSI_LEN - 2];  // LSB
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);

  if (mmgsdi_util_get_session_type(session_id, &session_type, NULL) != MMGSDI_SUCCESS ||
      !mmgsdi_util_is_1x_prov_session(session_type))
  {
    return;
  }

  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot);

  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND, nv_context) == MMGSDI_FEATURE_ENABLED)
  {
    boolean  mmgsdi_hrpd_disabled_card_flag     = FALSE;
    boolean  mmgsdi_allow_fallback_to_cave_flag = FALSE;

    if (NVRUIM_NON_3GPD_CDMA_CARD_MCC1 == imsi_mcc ||
        NVRUIM_NON_3GPD_CDMA_CARD_MCC2 == imsi_mcc ||
        NVRUIM_NON_3GPD_CDMA_CARD_MCC3 == imsi_mcc)
    {
      mmgsdi_allow_fallback_to_cave_flag = TRUE;
      if (app_type == MMGSDI_APP_CSIM)
      {
        mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                      app_type,
                                                      slot,
                                                      MMGSDI_CSIM_HRPDUPP,
                                                      &mmgsdi_data_buf);
      }
      else
      {
        mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                      app_type,
                                                      slot,
                                                      MMGSDI_CDMA_HRPD_HRPDUPP,
                                                      &mmgsdi_data_buf);
      }

      if(mmgsdi_status != MMGSDI_SUCCESS  || mmgsdi_data_buf.data_ptr  == NULL)
      {
        UIM_MSG_HIGH_0(" HPRD disabled card, Card does not have HRPDUPP EF");
        mmgsdi_hrpd_disabled_card_flag =  TRUE;
      }
      else
      {
        if (((mmgsdi_data_buf.data_ptr[0]==0xFF)&&(mmgsdi_data_buf.data_ptr[1]==0xFF)) ||
           ((mmgsdi_data_buf.data_ptr[0]==0x00)&&(mmgsdi_data_buf.data_ptr[1]==0x00)))
        {
          UIM_MSG_HIGH_0(" HPRD disabled card, HRPDUPP EF does not have valid data");
          mmgsdi_hrpd_disabled_card_flag = TRUE;
        }
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
    }
    nvruim_init_fallback_to_cave(mmgsdi_allow_fallback_to_cave_flag, session_type);
    nvruim_init_hrpd_capability(mmgsdi_hrpd_disabled_card_flag, session_type);
  }

  /* For some carriers, even though the 3GPD EFs are valid, they want to use
     3GPD credentials from NV rather than from the card. Also, for some cards
     the 3GPD EFs are invalid and hence 3GPD credentials cannot be used from
     the card.
     To accomodate these scenarios, when EFS subitem
     MMGSDI_FEATURE_USE_3GPD_CREDENTIALS_FROM_NV is set, we treat the card as
     non-3gpd card and let higher layers choose 3GPD credentials from NV
     instead of relying on the card.
     NOTE: Even though this EFS subitem is now the master controller of whether
     to use the 3GPD credentials from the card or from the NV, the check below
     for specific MCCs is not removed to maintain backwards compatibility so
     that those customers are not all of a sudden now required to set this EFS
     sub item to be able to retain the legacy behavior (of using NV instead of
     card for 3GPD credentials) */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_USE_3GPD_CREDENTIALS_FROM_NV, nv_context) == MMGSDI_FEATURE_ENABLED)
  {
    non_3gpd_cdma_card = TRUE;
  }
  else
  {
    /* Check the IMSI for non 3gpd cdma card */
    if(mmgsdi_util_determine_protocol(slot) != MMGSDI_UICC &&
       (NVRUIM_NON_3GPD_CDMA_CARD_MCC1 == imsi_mcc ||
        NVRUIM_NON_3GPD_CDMA_CARD_MCC2 == imsi_mcc ||
        NVRUIM_NON_3GPD_CDMA_CARD_MCC3 == imsi_mcc))
    {
      /* Check the UPPEXT file data */
      mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                    MMGSDI_APP_RUIM,
                                                    slot,
                                                    MMGSDI_CDMA_3GPD_UPPEXT,
                                                    &mmgsdi_data_buf);
      if ((mmgsdi_status != MMGSDI_SUCCESS) || ( mmgsdi_data_buf.data_ptr == NULL)
           || (mmgsdi_data_buf.data_len <= 0) || (0xFF == mmgsdi_data_buf.data_ptr[0]))
      {
        UIM_MSG_HIGH_0("Card does not have 3GPD ext service");
        non_3gpd_cdma_card = TRUE;
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
    }
  }

  nvruim_init_non_3gpd_cdma_card(non_3gpd_cdma_card, session_type);

  /* an_hrpd_prefrence only for 1X_PRI. Data team also calls this API */
  if (session_type == MMGSDI_1X_PROV_PRI_SESSION)
  {
    if (app_type == MMGSDI_APP_CSIM)
    {
      (void)mmgsdi_uicc_chk_srv_available(client_id, session_id,
                                          MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                          MMGSDI_CSIM_SRV_HRPD,&srv_avail);
    }
    else
    {
      (void)mmgsdi_icc_chk_srv_available(session_id,
                                         MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                         MMGSDI_CDMA_SRV_HRPD,
                                         &srv_avail);
    }

    if(srv_avail)
    {
      support_an_hrpd = TRUE;
    }
    uim_init_an_hrpd_preference(support_an_hrpd);
  }

  if (app_type == MMGSDI_APP_CSIM)
  {
    (void)mmgsdi_uicc_chk_srv_available(client_id, session_id,
                                        MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                        MMGSDI_CSIM_SRV_3GPD_SIP,&srv_avail);
  }
  else
  {
    (void)mmgsdi_icc_chk_srv_available(session_id,
                                       MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                       MMGSDI_CDMA_SRV_3GPD_SIP,
                                       &srv_avail);
  }

  if(srv_avail)
  {
    support_sip = TRUE;
  }

  if (app_type == MMGSDI_APP_CSIM)
  {
    (void)mmgsdi_uicc_chk_srv_available(client_id, session_id,
                                        MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                        MMGSDI_CSIM_SRV_3GPD_MIP,&srv_avail);
  }
  else
  {
    (void)mmgsdi_icc_chk_srv_available(session_id,
                                       MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                       MMGSDI_CDMA_SRV_3GPD_MIP,
                                       &srv_avail);
  }

  if(srv_avail)
  {
    support_mip = TRUE;
  }

  /* 3gpd extension support */
  if (app_type == MMGSDI_APP_CSIM)
  {
    (void)mmgsdi_uicc_chk_srv_available(client_id, session_id,
                                        MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                        MMGSDI_CSIM_SRV_3GPD_EXT,&srv_avail);
  }
  else
  {
    (void)mmgsdi_icc_chk_srv_available(session_id,
                                       MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                       MMGSDI_CDMA_SRV_3GPD_EXT,
                                       &srv_avail);
  }

  if(srv_avail)
  {
    service_3gpd_extensions = TRUE;
  }
  uim_init_3gpd_preference(support_sip, support_mip, session_type);
  nvruim_data_3gpd_init_extensions_support(service_3gpd_extensions, session_type);

  /* lbs support API also called by GPS */
  if (session_type == MMGSDI_1X_PROV_PRI_SESSION)
  {
    if (app_type == MMGSDI_APP_CSIM)
    {
      (void)mmgsdi_uicc_chk_srv_available(client_id, session_id,
                                          MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                          MMGSDI_CSIM_SRV_LBS,&srv_avail);
    }
    else
    {
      (void)mmgsdi_icc_chk_srv_available(session_id,
                                         MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                         MMGSDI_CDMA_SRV_LBS,
                                         &srv_avail);
    }

    if(srv_avail)
    {
      support_lbs = TRUE;
    }
    nvruim_lbs_init_support(support_lbs);

    /* wms services API also called by WMS team */
    if (app_type == MMGSDI_APP_CSIM)
    {
      (void)mmgsdi_uicc_chk_srv_available(client_id, session_id,
                                          MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                          MMGSDI_CSIM_SRV_3GPD_EXT,&srv_avail);
    }
    else
    {
      (void)mmgsdi_icc_chk_srv_available(session_id,
                                         MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                         MMGSDI_CDMA_SRV_3GPD_EXT,
                                         &srv_avail);
    }

    if(srv_avail)
    {
      support_smscap = TRUE;
    }

    if (app_type == MMGSDI_APP_CSIM)
    {
      (void)mmgsdi_uicc_chk_srv_available(client_id, session_id,
                                          MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                          MMGSDI_CSIM_SRV_BCMCS,&srv_avail);
    }
    else
    {
      (void)mmgsdi_icc_chk_srv_available(session_id,
                                         MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                         MMGSDI_CDMA_SRV_BCMCS,
                                         &srv_avail);
    }

    if(srv_avail)
    {
      support_bcsms = TRUE;
    }
    nvruim_init_wms_svc_items(support_bcsms,support_smscap);
  }

  if (app_type == MMGSDI_APP_CSIM)
  {
    mmgsdi_status = mmgsdi_card_init_cache_binary(
                      session_id,
                      MMGSDI_APP_CSIM,
                      slot,
                      MMGSDI_CSIM_SPN,
                      &mmgsdi_data_buf);
  }
  else
  {
    mmgsdi_status = mmgsdi_card_init_cache_binary(
                      session_id,
                      MMGSDI_APP_RUIM,
                      slot,
                      MMGSDI_CDMA_HOME_SVC_PVDR_NAME,
                      &mmgsdi_data_buf);
  }

  if ((mmgsdi_status == MMGSDI_SUCCESS) && ( mmgsdi_data_buf.data_ptr != NULL)
       && (mmgsdi_data_buf.data_len > 0) && (0xFF != mmgsdi_data_buf.data_ptr[0]))
  {
    UIM_MSG_HIGH_0("Card does have HOME SVC PVDR NAME");
  }

  /* Free the memory allocated for read request */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
} /* mmgsdi_card_init_1x_srv_nvruim_init */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_1X_READ_PRL_INIT

DESCRIPTION
  This function initializes the PRL cache data

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_1x_read_prl_init(
  mmgsdi_session_id_type session_id
)
{
  dword                                 pr_list_size       = 0;
  dword                                 pr_list_2_size     = 0;
  prl_sspr_p_rev_e_type                 sspr_p_rev         = PRL_SSPR_P_REV_3;
  boolean                               is_full_validation = TRUE;
  mmgsdi_data_type                      eprl_data;
  mmgsdi_data_type                      eprl_size_data;
  mmgsdi_data_type                      prl_data;
  mmgsdi_data_type                      prl_size_data;
  mmgsdi_data_type                      prl_2_data;
  mmgsdi_data_type                      prl_2_size_data;
  mmgsdi_read_prl_data_type             *cache_prl_2_ptr   = NULL;
  mmgsdi_return_enum_type               mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_access_type                    access;
  dword                                 max_prl_size       = 0;
  dword                                 max_eprl_size      = 0;
  word                                  prl_version        = 0;
  mmgsdi_read_prl_data_type             *cache_prl_ptr     = NULL;
  mmgsdi_read_prl_data_type             *cache_eprl_ptr    = NULL;
  mmgsdi_int_app_info_type              *app_info_ptr      = NULL;
  mmgsdi_session_info_type              *session_info_ptr  = NULL;
  mmgsdi_cache_init_enum_type           cache_state        = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_slot_id_enum_type              slot               = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_closed_cause_enum_type cause              = MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;
  mmgsdi_len_type                       data_len           = 0;
  prl_validation_enum_type              prl_status         = PRL_VALIDATE_MAX_TYPE;
  uint8                                 prl_index          = 0;
  mmgsdi_technology_enum_type           dummy_tech         = MMGSDI_TECH_UNKNOWN;
  mmgsdi_attribute_enum_type            attribute          = MMGSDI_ATTR_UNKNOWN;

  memset(&eprl_data, 0, sizeof(mmgsdi_data_type));
  memset(&eprl_size_data, 0, sizeof(mmgsdi_data_type));
  memset(&prl_data, 0, sizeof(mmgsdi_data_type));
  memset(&prl_size_data, 0, sizeof(mmgsdi_data_type));
  memset(&prl_2_data, 0, sizeof(mmgsdi_data_type));
  memset(&prl_2_size_data, 0, sizeof(mmgsdi_data_type));

  UIM_MSG_HIGH_0("In mmgsdi_card_init_1x_read_prl_init");

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id, &slot,
                             NULL, &app_info_ptr, NULL, &session_info_ptr,NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
     UIM_MSG_ERR_0("Unable to get session/app info in mmgsdi_card_init_1x_read_prl_init");
     return mmgsdi_status;
  }

  if(app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM)
  {
    access.file.file_enum = MMGSDI_CSIM_PRL;
  }
  else if(app_info_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    access.file.file_enum = MMGSDI_CDMA_PRL;
  }
  else
  {
    UIM_MSG_ERR_0("Non 1X-Provisioning app in mmgsdi_card_init_1x_read_prl_init");
    return MMGSDI_ERROR;
  }
  access.access_method = MMGSDI_EF_ENUM_ACCESS;

  if(!mmgsdi_util_is_1x_prov_session(session_info_ptr->session_type))
  {
    UIM_MSG_ERR_0("Non 1X-Provisioning session ID in mmgsdi_card_init_1x_read_prl_init");
    return MMGSDI_ERROR;
  }

  (void)mmgsdi_util_get_tech_and_attr_from_session_type(
                                               session_info_ptr->session_type,
                                               &dummy_tech,
                                               &attribute);
  prl_index = (uint8)attribute;
  if (prl_index > MMGSDI_TER_PRL_INDEX)
  {
    return MMGSDI_ERROR;
  }

  /* Assuming that EF PRL and EF EPRL are already read and cached by MMGSDI
     as part of post pin1 init, get the length of EF PRL from MMGSDI cache */
  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                 &access,
                                        &data_len,
                                        0,
                                        NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     data_len <= 0)
  {
    UIM_MSG_ERR_0("Unable to read EF-PRL file");
    return MMGSDI_ERROR;
  }

  max_prl_size = int32touint32(data_len);
  data_len = 0;

  /* Should not be bigger than buffer */
  if(max_prl_size > NV_ROAMING_LIST_MAX_SIZE)
  {
    UIM_MSG_HIGH_0("PRL MAX size is larger than ME max size, truncating.");
    max_prl_size = NV_ROAMING_LIST_MAX_SIZE;
  }
  UIM_MSG_HIGH_1("Max PRL size: 0x%x.", max_prl_size);

  /* Start the process of reading EF EPRL by retrieving length of EF EPRL from
     MMGSDI cache */
  if(app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM)
  {
    access.file.file_enum = MMGSDI_CSIM_EPRL;
  }
  else
  {
    access.file.file_enum = MMGSDI_CDMA_EPRL;
  }

  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                        &access,
                                        &data_len,
                                        0,
                                        NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     data_len <= 0)
  {
    UIM_MSG_ERR_0("Unable to read EF-EPRL file, falling back to EF-PRL");
  }
  else
  {
    max_eprl_size = int32touint32(data_len);

    /* Should not be bigger than buffer */
    if(max_eprl_size > NV_ROAMING_LIST_MAX_SIZE)
    {
      UIM_MSG_HIGH_0("EPRL MAX size is larger than ME max size, truncating.");
      max_eprl_size = NV_ROAMING_LIST_MAX_SIZE;
    }
    UIM_MSG_HIGH_1("Max EPRL size: 0x%x.", max_eprl_size);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(eprl_size_data.data_ptr,
                                       MMGSDI_SIZE_OF_PRL_SIZE);
    if (eprl_size_data.data_ptr == NULL)
    {
      /* Unable to allocate memory */
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    eprl_size_data.data_len = MMGSDI_SIZE_OF_PRL_SIZE;
    mmgsdi_status = mmgsdi_cache_read(session_id,
                                      &access,
                                      &eprl_size_data,
                                      0, &cache_state);

    if(mmgsdi_status != MMGSDI_SUCCESS || cache_state == MMGSDI_CACHE_NOT_FOUND)
    {
      UIM_MSG_ERR_0("Unable to read EF-EPRL file, falling back to EF-PRL");
      MMGSDIUTIL_TMC_MEM_FREE(eprl_size_data.data_ptr);
    }
    else
    {
      /* Read the rest of EPRL */

      /* Fill up list size */
      pr_list_size = (eprl_size_data.data_ptr[0] << 8) | eprl_size_data.data_ptr[1];

      MMGSDIUTIL_TMC_MEM_FREE(eprl_size_data.data_ptr);
      /* Check to see if the list size is valid */
      if((pr_list_size == 0) || (pr_list_size > max_eprl_size))
      {
        mmgsdi_status = MMGSDI_ERROR;
        UIM_MSG_HIGH_1("EPRL Size = %d", pr_list_size);
      }
      else
      {
        /* Read the eprl to a buffer */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(eprl_data.data_ptr, pr_list_size);
        if(eprl_data.data_ptr == NULL)
        {
          return MMGSDI_ERROR;
        }
        eprl_data.data_len = pr_list_size;

        mmgsdi_status = mmgsdi_cache_read(session_id,
                                          &access,
                                          &eprl_data,
                                          0, &cache_state);

        if(mmgsdi_status != MMGSDI_SUCCESS || cache_state == MMGSDI_CACHE_NOT_FOUND)
        {
          UIM_MSG_ERR_0("Unable to read EF-EPRL Data, Falling Back to EF-PRL");
          MMGSDIUTIL_TMC_MEM_FREE(eprl_data.data_ptr);
        }
        else
        {
           /* Check the validity of the EPRL */
           prl_status = prl_validate(eprl_data.data_ptr, &prl_version,
                                     &sspr_p_rev, is_full_validation);

           UIM_MSG_MED_1("PRL validation status is: 0x%x", prl_status);
           if(prl_status == PRL_VALID)
           {
             MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_eprl_ptr,
               sizeof(mmgsdi_read_prl_data_type));
             if (cache_eprl_ptr == NULL)
             {
               /* Unable to allocate memory */
               MMGSDIUTIL_TMC_MEM_FREE(eprl_data.data_ptr);
               return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
             }
             MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
                 cache_eprl_ptr->roaming_list_ptr);
             MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
                 cache_eprl_ptr->roaming_list_ptr,
                pr_list_size);
             if (cache_eprl_ptr->roaming_list_ptr == NULL)
             {
               /* Unable to allocate memory */
               MMGSDIUTIL_TMC_MEM_FREE(eprl_data.data_ptr);
               MMGSDIUTIL_TMC_MEM_FREE(cache_eprl_ptr);
               return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
             }

             cache_eprl_ptr->size = pr_list_size;
             cache_eprl_ptr->valid = is_full_validation;
             cache_eprl_ptr->prl_version = prl_version;
             cache_eprl_ptr->sspr_p_rev = sspr_p_rev;
             mmgsdi_memscpy(cache_eprl_ptr->roaming_list_ptr,
                            pr_list_size,
                            eprl_data.data_ptr,
                            pr_list_size);
             MMGSDIUTIL_TMC_MEM_FREE(eprl_data.data_ptr);

             if(mmgsdi_1x_prl[prl_index].prl_data_ptr)
             {
               MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_1x_prl[prl_index].prl_data_ptr->roaming_list_ptr);
             }
             MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_1x_prl[prl_index].prl_data_ptr);
             mmgsdi_1x_prl[prl_index].prl_data_ptr  = cache_eprl_ptr;
             mmgsdi_1x_prl[prl_index].prl_validated = TRUE;
             UIM_MSG_HIGH_1("Roaming List retrieved from EF-EPRL for prl_index: 0x%x",
                            prl_index);
             return MMGSDI_SUCCESS;
           } /* EF-EPRL contents valid PRL */

           MMGSDIUTIL_TMC_MEM_FREE(eprl_data.data_ptr);

           if (prl_status == PRL_INVALID_SD_NOT_READY)
           {
             mmgsdi_1x_prl[prl_index].prl_validated = FALSE;
             return MMGSDI_ERROR;
           }
           mmgsdi_1x_prl[prl_index].prl_validated = TRUE;
        }/* EF-EPRL Cannot be read */
      }/* EPRL is an invalid size */
    }/* EF-EPRL size cannot be read */
  }/* EF-EPRL get file attributes is success */

  /* Reset the SSPR_P_REV to 1 for EF-PRL */
  sspr_p_rev = PRL_SSPR_P_REV_1;

  /* READ EF-PRL */
  if(app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM)
  {
    access.file.file_enum = MMGSDI_CSIM_PRL;
  }
  else if(app_info_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    access.file.file_enum = MMGSDI_CDMA_PRL;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(prl_size_data.data_ptr,
                                     MMGSDI_SIZE_OF_PRL_SIZE);
  if (prl_size_data.data_ptr == NULL)
  {
    /* Unable to allocate memory */
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  prl_size_data.data_len = MMGSDI_SIZE_OF_PRL_SIZE;
  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &access,
                                    &prl_size_data,
                                    0, &cache_state);

  if(mmgsdi_status != MMGSDI_SUCCESS || cache_state == MMGSDI_CACHE_NOT_FOUND)
  {
    UIM_MSG_ERR_0("Unable to read EF-PRL size info");
    MMGSDIUTIL_TMC_MEM_FREE(prl_size_data.data_ptr);
    return mmgsdi_status;
  }

  /* Read the rest of PRL */

  /* Fill up list size */
  pr_list_size = (prl_size_data.data_ptr[0] << 8) | prl_size_data.data_ptr[1];

  MMGSDIUTIL_TMC_MEM_FREE(prl_size_data.data_ptr);

  /* Check to see if the list size is valid */
  if((pr_list_size == 0) || (pr_list_size > max_prl_size))
  {
    mmgsdi_status = MMGSDI_ERROR;
    UIM_MSG_HIGH_1("PRL Size = %d", pr_list_size);

    /* Update the session closed cause. The session changed event
       broadcasts the cause */
    cause = mmgsdi_card_init_map_file_to_session_closed_cause(access.file.file_enum,
                                                              TRUE);
    mmgsdi_util_update_session_closed_cause(session_id, cause);
    return mmgsdi_status;
  }
  /* Read the prl to a buffer */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(prl_data.data_ptr,
                                     pr_list_size);
  if(prl_data.data_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  prl_data.data_len = pr_list_size;

  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &access,
                                    &prl_data,
                                    0, &cache_state);

  if(mmgsdi_status != MMGSDI_SUCCESS || cache_state == MMGSDI_CACHE_NOT_FOUND)
  {
    UIM_MSG_ERR_0("Unable to read EF-PRL Data, failing PRL Read");
    MMGSDIUTIL_TMC_MEM_FREE(prl_data.data_ptr);
    return MMGSDI_ERROR;
  }
  /* Check the validity of the PRL */
  prl_status = prl_validate(prl_data.data_ptr, &prl_version,
                            &sspr_p_rev, is_full_validation);

  UIM_MSG_MED_1("PRL validation status is: 0x%x", prl_status);
  if(prl_status == PRL_VALID)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_prl_ptr,
      sizeof(mmgsdi_read_prl_data_type));
    if (cache_prl_ptr == NULL)
    {
      /* Unable to allocate memory */
      MMGSDIUTIL_TMC_MEM_FREE(prl_data.data_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_prl_ptr->roaming_list_ptr,
                                       pr_list_size);
    if (cache_prl_ptr->roaming_list_ptr == NULL)
    {
      /* Unable to allocate memory */
      MMGSDIUTIL_TMC_MEM_FREE(prl_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(cache_prl_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    cache_prl_ptr->size = pr_list_size;
    cache_prl_ptr->valid = is_full_validation;
    cache_prl_ptr->prl_version = prl_version;
    cache_prl_ptr->sspr_p_rev = sspr_p_rev;
    mmgsdi_memscpy(cache_prl_ptr->roaming_list_ptr,
                   pr_list_size,
                   prl_data.data_ptr,
                   pr_list_size);
    MMGSDIUTIL_TMC_MEM_FREE(prl_data.data_ptr);

    if(mmgsdi_1x_prl[prl_index].prl_data_ptr)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_1x_prl[prl_index].prl_data_ptr->roaming_list_ptr);
    }
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_1x_prl[prl_index].prl_data_ptr);
    mmgsdi_1x_prl[prl_index].prl_data_ptr  = cache_prl_ptr;
    mmgsdi_1x_prl[prl_index].prl_validated = TRUE;
    UIM_MSG_HIGH_1("Roaming List retrieved from EF-PRL for prl_index: 0x%x",
                   prl_index);
  } /* EF-PRL contents valid PRL */
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE(prl_data.data_ptr);

    if (prl_status == PRL_INVALID_SD_NOT_READY)
    {
      mmgsdi_1x_prl[prl_index].prl_validated = FALSE;
      return MMGSDI_ERROR;
    }
    mmgsdi_1x_prl[prl_index].prl_validated = TRUE;
    cache_prl_ptr = NULL;
  }/* EF-PRL contains invalid data */

  /* READ CPRL / Second PRL */

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(prl_2_size_data.data_ptr,
                                     MMGSDI_SIZE_OF_PRL_SIZE);
  if (prl_2_size_data.data_ptr == NULL)
  {
    /* Unable to allocate memory */
    return MMGSDI_SUCCESS;
  }
  prl_2_size_data.data_len = MMGSDI_SIZE_OF_PRL_SIZE;
  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &access,
                                    &prl_2_size_data,
                                    pr_list_size, &cache_state);

  if(mmgsdi_status != MMGSDI_SUCCESS || cache_state == MMGSDI_CACHE_NOT_FOUND)
  {
    UIM_MSG_ERR_0("Unable to find second PRL. Using only 1 PRL in EF-PRL.");
    MMGSDIUTIL_TMC_MEM_FREE(prl_2_size_data.data_ptr);
    return MMGSDI_SUCCESS;
  }

  /* Read the rest of PRL */

  /* Fill up list size */
  pr_list_2_size = (prl_2_size_data.data_ptr[0] << 8) | prl_2_size_data.data_ptr[1];

  MMGSDIUTIL_TMC_MEM_FREE(prl_2_size_data.data_ptr);

  /* Check to see if the list size is valid */
  if((pr_list_2_size + pr_list_size == 0) || (pr_list_2_size + pr_list_size > max_prl_size))
  {
    UIM_MSG_HIGH_1("CSIM PRL Size = %d too large with 2nd PRL. Using 1 PRL from EF-PRL",
               pr_list_size + pr_list_2_size);
    return MMGSDI_SUCCESS;
  }
  /* Read the prl to a buffer */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(prl_2_data.data_ptr,
                                     (pr_list_2_size+2));

  if(prl_2_data.data_ptr == NULL)
  {
    return MMGSDI_SUCCESS;
  }
  prl_2_data.data_len = pr_list_2_size + 2;
  mmgsdi_status = mmgsdi_cache_read(session_id,
                                    &access,
                                    &prl_2_data,
                                    pr_list_size, &cache_state);

  if(mmgsdi_status != MMGSDI_SUCCESS || cache_state == MMGSDI_CACHE_NOT_FOUND)
  {
    UIM_MSG_ERR_0("Unable to read 2nd PRL.  Using 1 PRL from EF-PRL");
    MMGSDIUTIL_TMC_MEM_FREE(prl_2_data.data_ptr);
    return MMGSDI_SUCCESS;
  }
  /* Check the validity of the PRL */
  prl_status = prl_validate(prl_2_data.data_ptr, &prl_version,
                            &sspr_p_rev, is_full_validation);

  UIM_MSG_MED_1("PRL validation status is: 0x%x", prl_status);
  if(prl_status == PRL_VALID)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_prl_2_ptr,
      sizeof(mmgsdi_read_prl_data_type));
    if (cache_prl_2_ptr == NULL)
    {
      /* Unable to allocate memory */
      MMGSDIUTIL_TMC_MEM_FREE(prl_data.data_ptr);
      return MMGSDI_SUCCESS;
    }
    if(cache_prl_ptr != NULL)
    {
      /* Two valid PRLs in EF-PRL */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_prl_2_ptr->roaming_list_ptr,
        (pr_list_2_size + pr_list_size + MMGSDI_CPRL_CRC_SIZE));
      if (cache_prl_2_ptr->roaming_list_ptr == NULL)
      {
        /* Unable to allocate memory */
        MMGSDIUTIL_TMC_MEM_FREE(prl_2_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(cache_prl_2_ptr);
        return MMGSDI_SUCCESS;
      }

      cache_prl_2_ptr->size = (pr_list_2_size + pr_list_size + MMGSDI_CPRL_CRC_SIZE);
      cache_prl_2_ptr->valid = is_full_validation;
      cache_prl_2_ptr->prl_version = prl_version;
      cache_prl_2_ptr->sspr_p_rev = sspr_p_rev;
      mmgsdi_memscpy(cache_prl_2_ptr->roaming_list_ptr,
                     pr_list_2_size + pr_list_size + MMGSDI_CPRL_CRC_SIZE,
                     cache_prl_ptr->roaming_list_ptr,
                     pr_list_size);
      mmgsdi_memscpy(&(cache_prl_2_ptr->roaming_list_ptr[pr_list_size]),
                     pr_list_2_size + MMGSDI_CPRL_CRC_SIZE,
                     prl_2_data.data_ptr,
                     pr_list_2_size);
      MMGSDIUTIL_TMC_MEM_FREE(prl_2_data.data_ptr);
    }/* 2 Valid PRLs in EF-PRL */
    else
    {
      /* Only second PRL is valid */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_prl_2_ptr->roaming_list_ptr,
                                         pr_list_2_size);
      if (cache_prl_2_ptr->roaming_list_ptr == NULL)
      {
        /* Unable to allocate memory */
        MMGSDIUTIL_TMC_MEM_FREE(prl_2_data.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(cache_prl_2_ptr);
        return MMGSDI_SUCCESS;
      }

      cache_prl_2_ptr->size = pr_list_2_size;
      cache_prl_2_ptr->valid = is_full_validation;
      cache_prl_2_ptr->prl_version = prl_version;
      cache_prl_2_ptr->sspr_p_rev = sspr_p_rev;
      mmgsdi_memscpy(cache_prl_2_ptr->roaming_list_ptr,
                     pr_list_2_size,
                     prl_2_data.data_ptr,
                     pr_list_2_size);
      MMGSDIUTIL_TMC_MEM_FREE(prl_2_data.data_ptr);
    }/* Only second PRL valid in EF-PRL */

    if(mmgsdi_1x_prl[prl_index].prl_data_ptr)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_1x_prl[prl_index].prl_data_ptr->roaming_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_1x_prl[prl_index].prl_data_ptr);
    }
    mmgsdi_1x_prl[prl_index].prl_data_ptr   = cache_prl_2_ptr;
    mmgsdi_1x_prl[prl_index].prl_validated  = TRUE;
    UIM_MSG_HIGH_1("2nd Roaming List retrieved from EF-PRL for prl_index: 0x%x",
                   prl_index);

    cache_prl_ptr = NULL;
    return MMGSDI_SUCCESS;
  } /* EF-PRL contents valid 2nd PRL */

  MMGSDIUTIL_TMC_MEM_FREE(prl_2_data.data_ptr);

  if (prl_status == PRL_INVALID_SD_NOT_READY)
  {
    mmgsdi_1x_prl[prl_index].prl_validated = FALSE;
    return MMGSDI_ERROR;
  }
  mmgsdi_1x_prl[prl_index].prl_validated = TRUE;
  cache_prl_2_ptr = NULL;
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_1x_read_prl_init */
#endif /* FEATURE_MMGSDI_3GPP2 */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_OVERWRITE_OPLMNWACT_CACHE

DESCRIPTION
  This function overwrites EF OPLMNwACT cache if builtin PLMN list is present
  in EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_overwrite_oplmnwact_cache(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_app_enum_type     app_type
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  if(app_type != MMGSDI_APP_SIM && app_type != MMGSDI_APP_USIM)
  {
    return;
  }

  if((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_OPLMN_LIST_CONFIGURABLE_OTA, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
        MMGSDI_FEATURE_ENABLED) &&
     (mmgsdi_util_is_imsi_plmn_cust_plmn(slot_id)))
  {
    int32  file_size       = 0;
    uint8 *file_buffer_ptr = NULL;

    mmgsdi_status = mmgsdi_get_builtin_plmn_list(&file_size, &file_buffer_ptr);
    if(mmgsdi_status == MMGSDI_SUCCESS && file_size > 0 &&
      file_buffer_ptr != NULL)
    {
      mmgsdi_data_type    data;
      mmgsdi_access_type  access;

      data.data_len = file_size;
      data.data_ptr = file_buffer_ptr;
      access.access_method  = MMGSDI_EF_ENUM_ACCESS;

      if(app_type == MMGSDI_APP_USIM)
      {
        access.file.file_enum = MMGSDI_USIM_OPLMNWACT;
      }
      else
      {
        access.file.file_enum = MMGSDI_GSM_OPLMNWACT;
      }

      /* Delete already existing cache if any */
      (void)mmgsdi_cache_delete_item(session_id,
                                     1,
                                     &access.file.file_enum);

      (void)mmgsdi_cache_write(session_id,
                               &access,
                               data,
                               0,
                               MMGSDI_DATA_FROM_CARD_COMPLETE);
    }
    MMGSDIUTIL_TMC_MEM_FREE(file_buffer_ptr);
  }
}/* mmgsdi_card_init_overwrite_oplmnwact_cache */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_USIM_POST_PIN1_INIT

DESCRIPTION
  This function performs the post pin1 initialization for USIM

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_usim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  /* If mmgsdi feature absent_mandatory_files_init is enabled, file is not
     mandatory */
  boolean is_mandatory =
    (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ABSENT_MANDATORY_FILES_INIT,
       mmgsdi_util_get_efs_item_index_for_slot(slot_id)) ==
      MMGSDI_FEATURE_ENABLED) ? FALSE : TRUE;

  /* When adding EFs to this list, consider adding them to
     mmgsdi_util_find_ef_in_power_up_cache_list also */
  mmgsdi_cache_init_order_type usim_pup_init_table[] =
    /* file name,        mandatory, init func,               validation func */
      /* NETPAR: Determine which version of spec this USIM is operating under */
      /* EF NETPAR is mandatory for R99 2000-12 31.102 spec */
    {{MMGSDI_USIM_NETPAR,    FALSE,        mmgsdi_card_init_cache_binary,
     mmgsdi_card_init_validate_netpar                                         },
      /* EF RPLMNACT is mandatory for R99 2000-09 31.102 spec */
     {MMGSDI_USIM_RPLMNACT,  FALSE,        mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_rplmnwact                                     },
     {MMGSDI_USIM_HPLMN,     is_mandatory, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_HPLMNWACT, FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_PLMNWACT,  FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_OPLMNWACT, FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_FPLMN,     TRUE,         mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_CBMID,     FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_ACM,       FALSE,        mmgsdi_card_init_cache_record, NULL},
     {MMGSDI_USIM_ACM_MAX,   FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_EHPLMN,    FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_EST,       FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_EHPLMNPI,  FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_LRPLMNSI,  FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_HIDDENKEY, FALSE,        mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_hkey_data                                     },
     {MMGSDI_USIM_SPN,       FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_SPDI,      FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_NASCONFIG, FALSE,        mmgsdi_card_init_cache_binary, NULL}
    };

  UIM_MSG_HIGH_1("mmgsdi_card_init_usim_post_pin1_init, slot:0x%x", slot_id);

  mmgsdi_status = mmgsdi_uicc_get_capabilities(session_id, slot_id, MMGSDI_APP_USIM);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  (void)mmgsdi_card_init_hzi_init(session_id, slot_id, MMGSDI_APP_USIM);

  /* Now read files on the card and cache the contents */
  mmgsdi_status = mmgsdi_card_init_app_cache(
                    session_id,
                    MMGSDI_APP_USIM,
                    slot_id,
                    usim_pup_init_table,
                    sizeof(usim_pup_init_table)/
                    sizeof(mmgsdi_cache_init_order_type));

  mmgsdi_card_init_overwrite_oplmnwact_cache(session_id, slot_id,
                                             MMGSDI_APP_USIM);

  return mmgsdi_status;
} /* mmgsdi_card_init_usim_post_pin1_init */


#ifdef FEATURE_MMGSDI_3GPP2
/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_CSIM_WRITE_MODEL_INIT

DESCRIPTION
  This function initializes EF Model

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_csim_write_model_init(
  mmgsdi_session_id_type session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type             mmgsdi_status                   = MMGSDI_SUCCESS;
  uint8                               csim_device_model[MMGSDI_MODEL_EF_SIZE];
  char                                default_model[]                 = "Not Applicable";
  char                                default_manufacturer[]          = "Not Applicable";
  char                                default_version[]               = "Not Applicable";
  mmgsdi_write_req_type             * req_ptr                         = NULL;
  mmgsdi_write_cnf_type             * cnf_ptr                         = NULL;

  memset(csim_device_model, 0xFF, sizeof(csim_device_model));

  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_CSIM_DEVICE_MODEL,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          csim_device_model,
                          sizeof(csim_device_model)) != UIM_COMMON_EFS_SUCCESS)
  {
    memset(csim_device_model, 0x00, sizeof(csim_device_model));

    /* Setting EF model parameters to default */
    csim_device_model[MMGSDI_MODEL_CHAR_ENCODE] = MMGSDI_CSIM_CHAR_ENCODE_DEFAULT;
    csim_device_model[MMGSDI_MODEL_LANG_IND] = MMGSDI_CSIM_CHAR_ENCODE_DEFAULT;
    mmgsdi_memscpy(&csim_device_model[MMGSDI_MODEL_INFORMATION],
                   sizeof(csim_device_model) - MMGSDI_MODEL_INFORMATION,
                   default_model,
                   sizeof(default_model));
    mmgsdi_memscpy(&csim_device_model[MMGSDI_MODEL_MANUFACTURER_NAME],
                   sizeof(csim_device_model) - MMGSDI_MODEL_MANUFACTURER_NAME,
                   default_manufacturer,
                   sizeof(default_manufacturer));
    mmgsdi_memscpy(&csim_device_model[MMGSDI_MODEL_VERSION_INFO],
                   sizeof(csim_device_model) - MMGSDI_MODEL_VERSION_INFO,
                   default_version,
                   sizeof(default_version));
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
                                     sizeof(mmgsdi_write_req_type));
  if(req_ptr == NULL)
  {
    return;
  }

  req_ptr->request_header.session_id         = session_id;
  req_ptr->request_header.slot_id            = slot_id;
  req_ptr->request_header.request_type       = MMGSDI_WRITE_REQ;
  req_ptr->request_header.orig_request_type  = MMGSDI_WRITE_REQ;
  req_ptr->access.access_method              = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum             = MMGSDI_CSIM_DEV_MODEL;
  req_ptr->rec_num                           = 0;
  req_ptr->offset                            = 0;
  req_ptr->file_type                         = MMGSDI_TRANSPARENT_FILE;
  req_ptr->data.data_len                     = sizeof(csim_device_model);
  req_ptr->data.data_ptr                     = csim_device_model;

  mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, TRUE, &cnf_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("Failed to write Model Information: Status = 0x%x",
                   mmgsdi_status);
  }

  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
} /* mmgsdi_card_init_csim_write_model_init */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_CSIM_WRITE_ME_CRP_INIT

DESCRIPTION
  This function initializes EF ME CRP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_csim_write_me_crp_init(
  mmgsdi_session_id_type session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type             mmgsdi_status                   = MMGSDI_SUCCESS;
  uint8                               csim_mecrp[MMGSDI_ME_CRP_EF_SIZE];
  nv_stat_enum_type                   nv_status                       = NV_NOTALLOC_S;
  mmgsdi_write_req_type             * req_ptr                         = NULL;
  mmgsdi_write_cnf_type             * cnf_ptr                         = NULL;
  mmgsdi_nv_item_cache_type           mmgsdi_nv_data                  = {0};

  memset(csim_mecrp, 0x00, sizeof(csim_mecrp));

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
                                     sizeof(mmgsdi_write_req_type));

  if(req_ptr == NULL)
  {
    return;
  }

  req_ptr->request_header.session_id         = session_id;
  req_ptr->request_header.slot_id            = slot_id;
  req_ptr->request_header.request_type       = MMGSDI_WRITE_REQ;
  req_ptr->request_header.orig_request_type  = MMGSDI_WRITE_REQ;
  req_ptr->access.access_method              = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum             = MMGSDI_CSIM_MECRP;
  req_ptr->rec_num                           = 0;
  req_ptr->offset                            = 0;
  req_ptr->file_type                         = MMGSDI_TRANSPARENT_FILE;
  req_ptr->data.data_len                     = sizeof(csim_mecrp);

  nv_status = mmgsdi_nv_get_item(NV_SCM_I, &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  /* Assigning SCM */
  if (nv_status == NV_DONE_S)
  {
    csim_mecrp[MMGSDI_ME_CRP_SCM] = mmgsdi_nv_data.item_value.scm;
  }
  else
  {
    csim_mecrp[MMGSDI_ME_CRP_SCM] = MMGSDI_CSIM_CHAR_ENCODE_DEFAULT;
  }

  nv_status = mmgsdi_nv_get_item(NV_MOB_CAI_REV_I, &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  /* Assigning MOB Revision */
  if (nv_status == NV_DONE_S)
  {
    csim_mecrp[MMGSDI_ME_CRP_MOB_REV] = mmgsdi_nv_data.item_value.mob_cai_rev;
  }
  else
  {
    csim_mecrp[MMGSDI_ME_CRP_MOB_REV] = MMGSDI_CSIM_CHAR_ENCODE_DEFAULT;
  }

  /* Setting local control to default */
  csim_mecrp[MMGSDI_ME_CRP_LOCAL_CONTROL] = MMGSDI_CSIM_CHAR_ENCODE_DEFAULT;

  req_ptr->data.data_ptr = csim_mecrp;

  mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, TRUE, &cnf_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("Failed to write ME specific Configuration Request Params: Status = 0x%x",
                   mmgsdi_status);
  }

  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
} /* mmgsdi_card_init_csim_write_me_crp_init */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_RESET_PRL_CACHE

DESCRIPTION
  This function will initialize the length bytes to the specified value.
  Rest of the data in the cache is set to all zeros to emulate the PRL
  contents as seen in test/commercial cards.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_reset_prl_cache(
  mmgsdi_session_id_type     session_id,
  mmgsdi_slot_id_enum_type   slot,
  const mmgsdi_access_type * access_ptr,
  uint16                     prl_size
)
{
  mmgsdi_return_enum_type             mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_attr_cache_element_type     *attr_cache_ptr = NULL;
  mmgsdi_data_type                    cache_data;

  MMGSDIUTIL_RETURN_IF_NULL(access_ptr);

  mmgsdi_status = mmgsdi_cache_attr_read(session_id,
                                         slot,
                                         access_ptr,
                                         &attr_cache_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     attr_cache_ptr == NULL ||
     attr_cache_ptr->file_size == 0)
  {
    return MMGSDI_ERROR;
  }

  memset(&cache_data, 0x00, sizeof(cache_data));
  cache_data.data_len = attr_cache_ptr->file_size;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_data.data_ptr,
                                     cache_data.data_len);

  if(cache_data.data_ptr != NULL)
  {
    memset(cache_data.data_ptr, 0x00, cache_data.data_len);

    if(cache_data.data_len >= MMGSDI_SIZE_OF_PRL_SIZE)
    {
      cache_data.data_ptr[0] = ((prl_size / 0x100) & 0xFF);
      cache_data.data_ptr[1] = (prl_size & 0xFF);
    }

    mmgsdi_status = mmgsdi_cache_write(session_id,
                                       access_ptr,
                                       cache_data,
                                       0,
                                       MMGSDI_DATA_FROM_CARD_COMPLETE);

    MMGSDIUTIL_TMC_MEM_FREE(cache_data.data_ptr);
  }

  mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);

  return mmgsdi_status;
} /* mmgsdi_card_init_reset_prl_cache */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_CACHE_PRL

DESCRIPTION
  This function will initialize PRL/EPRL data in the cache during CSIM or
  RUIM initialization. The EF-PRL/EF-EPRL is read in two steps: 1) read the
  first two bytes that indicate the size of the PRL data, 2) read only the
  number bytes equal to the size of PRL data from EF-PRL/EF-EPRL. The PRL is
  cached after the second read operation.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_cache_prl(
  mmgsdi_session_id_type    session_id,
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_file_enum_type     file,
  mmgsdi_data_type         *data_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status          = MMGSDI_SUCCESS;
  mmgsdi_read_req_type               *req_ptr                = NULL;
  mmgsdi_read_cnf_type               *cnf_ptr                = NULL;
  uint16                              prl_size               = 0;
  uint16                              prl_2_size             = 0;

  /* No need to return data as PRL validation is done separately and not as
     part of creation of app cache. */
  (void)data_ptr;

  if((app_type != MMGSDI_APP_CSIM) && (app_type != MMGSDI_APP_RUIM))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((file != MMGSDI_CSIM_PRL) && (file != MMGSDI_CSIM_EPRL) &&
     (file != MMGSDI_CDMA_PRL) && (file != MMGSDI_CDMA_EPRL))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
  /* Read from persistent cache */
  if(app_type == MMGSDI_APP_CSIM)
  {
    mmgsdi_access_type                  access;
    mmgsdi_data_type                    data;

    memset(&access, 0, sizeof(mmgsdi_access_type));
    memset(&data, 0x00, sizeof(mmgsdi_data_type));

    mmgsdi_status = mmgsdi_persistent_cache_read(slot,
                                                 file,
                                                 0,
                                                 &(data.data_len),
                                                 &(data.data_ptr));

    if(mmgsdi_status == MMGSDI_SUCCESS &&
       data.data_ptr != NULL)
    {
      access.access_method   = MMGSDI_EF_ENUM_ACCESS;
      access.file.file_enum  = file;

      mmgsdi_status = mmgsdi_cache_write(session_id,
                                         &access,
                                         data,
                                         0,
                                         MMGSDI_DATA_FROM_CARD_COMPLETE);

      MMGSDIUTIL_TMC_MEM_FREE(data.data_ptr);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
    }
  }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

  /* Commands can be sent to UIM in sync mode only in GSDI context */
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_cache_prl() invoked in non GSDI context");
    return MMGSDI_NOT_INIT;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
    sizeof(mmgsdi_read_req_type));
  if (req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  req_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  req_ptr->request_header.session_id        = session_id;
  req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  req_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_read_req_type));
  req_ptr->request_header.payload_len        =
    req_ptr->request_header.request_len -
    uint32toint32(sizeof(mmgsdi_read_req_type));
  req_ptr->request_header.slot_id           = slot;
  req_ptr->request_header.client_data       = 0;
  req_ptr->request_header.response_cb       = NULL;
  req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum            = file;
  req_ptr->data_len                         = MMGSDI_SIZE_OF_PRL_SIZE;
  req_ptr->rec_num                          = 0;
  req_ptr->offset                           = 0;
  req_ptr->file_type                        = MMGSDI_TRANSPARENT_FILE;

  /* Read the first 2 bytes of PRL file which contain the length.
     PRL cache is updated only after reading the data. */
  if(app_type == MMGSDI_APP_RUIM)
  {
    mmgsdi_status = mmgsdi_uim_icc_read_transparent (req_ptr,
                                                     TRUE,
                                                     &cnf_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_uim_uicc_read_transparent (req_ptr,
                                                      TRUE,
                                                      &cnf_ptr);
  }

  if ((cnf_ptr == NULL) ||
      (mmgsdi_status != MMGSDI_SUCCESS) ||
      (cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
      (cnf_ptr->read_data.data_ptr == NULL) ||
      (cnf_ptr->read_data.data_len != MMGSDI_SIZE_OF_PRL_SIZE))
  {
    UIM_MSG_ERR_1("Failed to read length of PRL (EF enum: 0x%x)", file);

    /* Reset EF cache to all zeros. This ensures that the EF cache is present
       when populating the PRL cache in mmgsdi_card_init_1x_read_prl_init(). */
    (void)mmgsdi_card_init_reset_prl_cache(session_id,
                                           slot,
                                           &req_ptr->access,
                                           0);

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
    if(cnf_ptr != NULL)
    {
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type * )cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    }
    return mmgsdi_status;
  }

  prl_size = ((uint16)cnf_ptr->read_data.data_ptr[0] * 0x100) +
                cnf_ptr->read_data.data_ptr[1];
  UIM_MSG_HIGH_1("Size of first PRL: 0x%x", prl_size);

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);

  if(prl_size > NV_ROAMING_LIST_MAX_SIZE)
  {
    UIM_MSG_ERR_2("Invalid size 0x%x of PRL (EF enum: 0x%x)", prl_size, file);

    /* Reset EF cache to all zeros. This ensures that the EF cache is present
       when populating the PRL cache in mmgsdi_card_init_1x_read_prl_init(). */
    mmgsdi_status = mmgsdi_card_init_reset_prl_cache(session_id,
                                                     slot,
                                                     &req_ptr->access,
                                                     prl_size);

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
    return MMGSDI_NOT_SUPPORTED;
  }

  /* In case of EF-PRL, read the size of the second PRL if the size bytes
     of the first PRL indicate a non-zero length less than
     NV_ROAMING_LIST_MAX_SIZE. Read the entire EF if prl_size is 0 or
     read only the first PRL if it is equal to NV_ROAMING_LIST_MAX_SIZE. */
  if((file == MMGSDI_CDMA_PRL || file == MMGSDI_CSIM_PRL) &&
     prl_size > 0 &&
     prl_size != NV_ROAMING_LIST_MAX_SIZE)
  {
    req_ptr->offset = prl_size;
    if(app_type == MMGSDI_APP_RUIM)
    {
      mmgsdi_status = mmgsdi_uim_icc_read_transparent (req_ptr,
                                                       TRUE,
                                                       &cnf_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_uim_uicc_read_transparent (req_ptr,
                                                        TRUE,
                                                        &cnf_ptr);
    }

    if(mmgsdi_status == MMGSDI_SUCCESS &&
       cnf_ptr != NULL &&
       cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS &&
       cnf_ptr->read_data.data_ptr != NULL &&
       cnf_ptr->read_data.data_len == MMGSDI_SIZE_OF_PRL_SIZE)
    {
      prl_2_size = ((uint16)cnf_ptr->read_data.data_ptr[0] * 0x100) +
                     cnf_ptr->read_data.data_ptr[1];
      UIM_MSG_HIGH_1("Size of second PRL: 0x%x", prl_2_size);
    }

    if(cnf_ptr != NULL)
    {
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type * )cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    }
  }

  if(prl_2_size + prl_size > NV_ROAMING_LIST_MAX_SIZE)
  {
    UIM_MSG_ERR_2("PRL Size = 0x%x too large with 2nd PRL. Using 1 PRL from EF 0x%x",
                  (prl_2_size + prl_size), file);
    req_ptr->data_len = prl_size;
  }
  else
  {
    /* For a valid length of 2nd PRL, add CRC size to the requested length.
       This is done to keep the behavior consistent with caching of 2nd PRL
       in mmgsdi_card_init_1x_read_prl_init() */
    if(prl_2_size > 0)
    {
      req_ptr->data_len = prl_size + prl_2_size + MMGSDI_CPRL_CRC_SIZE;
    }
    else
    {
      req_ptr->data_len = prl_size + prl_2_size;
    }
  }
  req_ptr->offset   = 0;

  /* Initialize the PRL file cache if reading the EF partially.
     If requested length is 0, the entire EF is read and will be cached
     as part of response processing. */
  if(req_ptr->data_len > 0)
  {
    /* Get the size of the PRL file from the attributes cache and write dummy
       data into the EF cache for the PRL file. This keeps the file size in sync
       between the EF cache and the attributes cache and ensures that MMGSDI
       returns the PRL data from the cache if a client calls READ API with
       requested length set to the total size of the file. */
    mmgsdi_status = mmgsdi_card_init_reset_prl_cache(session_id,
                                                     slot,
                                                     &req_ptr->access,
                                                     0);
  }

  /* Read the file contents (only the size bytes + PRL data). The EF cache
     is overwritten this time with PRL data from the card. */
  if(app_type == MMGSDI_APP_RUIM)
  {
    mmgsdi_status = mmgsdi_uim_icc_read_transparent (req_ptr,
                                                     TRUE,
                                                     &cnf_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_uim_uicc_read_transparent (req_ptr,
                                                      TRUE,
                                                      &cnf_ptr);
  }

  if(cnf_ptr != NULL)
  {
    mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
  }
  else
  {
    mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
  return mmgsdi_status;
} /* mmgsdi_card_init_cache_prl */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_SELECT_AND_VALIDATE

DESCRIPTION
  This function will perform a SELECT on the file. This function can be used
  to determine if the file is present on the card or not. In addition, this
  function validates key attributes of the file.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_select_and_validate (
  mmgsdi_session_id_type    session_id,
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_file_enum_type     file,
  mmgsdi_data_type         *data_ptr)
{
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_len_type                 rec_len       = 0;
  mmgsdi_file_structure_enum_type file_type     = MMGSDI_MAX_FILE_STRUCTURE_ENUM;

  (void) data_ptr;

  mmgsdi_status = mmgsdi_card_init_get_file_attr(session_id,
                                                 app_type,
                                                 slot,
                                                 file,
                                                 NULL,
                                                 NULL,
                                                 &rec_len,
                                                 NULL,
                                                 &file_type);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* We received the attributes. Now lets validate the key attributes */
  if(file == MMGSDI_CDMA_HOME_SID_NID ||
     file == MMGSDI_CSIM_CDMA_HOME_SID_NID)
  {
    if(rec_len != MMGSDI_CDMA_HOME_SID_NID_REC_LEN ||
       file_type != MMGSDI_LINEAR_FIXED_FILE)
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
  }

  return mmgsdi_status;
} /* mmgsdi_card_init_select_and_validate */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_POST_PIN1_ESN

DESCRIPTION
  This function performs ESN related functionality for CSIM and RUIM

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_post_pin1_esn(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  uim_common_efs_context_type  efs_context    = UIM_COMMON_EFS_CONTEXT_MAX;
  cm_operator_e_type           cm_operator    = OPERATOR_NULL;
  mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_SUCCESS;

  if(mmgsdi_util_get_efs_item_context(session_id, &efs_context) == MMGSDI_SUCCESS)
  {
    (void)uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_OPERATOR_NAME,
                              UIM_COMMON_EFS_ITEM_FILE_TYPE,
                              efs_context,
                              (uint8 *)&cm_operator,
                              sizeof(cm_operator));
  }

  /* If operator is CT then update the ESN change flag locally on the device (not on card)
     and read Usage indicator file and update ESN usage indicator.
     Store ESN will be done after auto reg SMS is sent successfully */
  if(cm_operator == OPERATOR_CT)
  {
    mmgsdi_card_init_update_esn_me_status(
      session_id,
      slot_id);

    mmgsdi_status = mmgsdi_card_init_update_esn_usage_type(session_id);
  }
  else
  {
    mmgsdi_session_store_esn_req_type *store_esn_req_ptr = NULL;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(store_esn_req_ptr,
                                       sizeof(mmgsdi_session_store_esn_req_type));
    if(store_esn_req_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    store_esn_req_ptr->request_header.session_id        = session_id;
    store_esn_req_ptr->request_header.request_type      = MMGSDI_SESSION_STORE_ESN_REQ;
    store_esn_req_ptr->request_header.orig_request_type = MMGSDI_SESSION_STORE_ESN_REQ;
    store_esn_req_ptr->request_header.slot_id           = slot_id;
    store_esn_req_ptr->request_header.client_data       = 0;
    store_esn_req_ptr->request_header.response_cb       = NULL;

    mmgsdi_status = mmgsdi_card_init_store_esn(store_esn_req_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(store_esn_req_ptr);
  }

  return mmgsdi_status;
} /* mmgsdi_card_init_post_pin1_esn */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_CSIM_POST_PIN1_INIT

DESCRIPTION
  This function performs the post pin1 initialization for CSIM

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_csim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_nv_context_type    nv_context    = MMGSDI_NV_CONTEXT_INVALID;

  /* When adding EFs to this list, consider adding them to
     mmgsdi_util_find_ef_in_power_up_cache_list also */
  mmgsdi_cache_init_order_type csim_pup_init_pre_esn_table[] =
    /* file name,        mandatory, init func,               validation func */
    {{MMGSDI_CSIM_CST,               TRUE,  mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_CSIM_EST,               FALSE, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_CSIM_AD,                FALSE, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_CSIM_RUIM_ID,           TRUE,  mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type csim_pup_init_post_esn_table[] =
    /* file name,        mandatory, init func,               validation func */
    {{MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND, TRUE,  mmgsdi_card_init_cache_binary,        NULL},
     {MMGSDI_CSIM_CDMA_SYS_REGN_IND,        TRUE,  mmgsdi_card_init_cache_binary,        NULL},
     {MMGSDI_CSIM_OTAPA_SPC_ENABLE,         TRUE,  mmgsdi_card_init_cache_binary,        NULL},
     {MMGSDI_CSIM_NAM_LOCK,                 TRUE,  mmgsdi_card_init_cache_binary,        NULL},
     {MMGSDI_CSIM_SERVICE_PREF,             TRUE,  mmgsdi_card_init_cache_binary,        NULL},
     {MMGSDI_CSIM_ACCOLC,                   TRUE,  mmgsdi_card_init_cache_binary,        NULL},
     {MMGSDI_CSIM_CDMA_HOME_SID_NID,        TRUE,  mmgsdi_card_init_select_and_validate, NULL},
     {MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND, TRUE,  mmgsdi_card_init_cache_record,        NULL},
     {MMGSDI_CSIM_PRL,                      FALSE, mmgsdi_card_init_cache_prl,           NULL},
     {MMGSDI_CSIM_EPRL,                     FALSE, mmgsdi_card_init_cache_prl,           NULL},
     {MMGSDI_CSIM_MAXIMUM_PRL,              FALSE, mmgsdi_card_init_cache_binary,        NULL},
     {MMGSDI_CSIM_HIDDEN_KEY,               FALSE, mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_hkey_data                                                    },
     {MMGSDI_CSIM_SPN,                      FALSE, mmgsdi_card_init_cache_binary,        NULL}
    };

  UIM_MSG_HIGH_1("mmgsdi_card_init_csim_post_pin1_init, slot:0x%x", slot_id);

  (void)mmgsdi_uicc_get_capabilities(session_id, slot_id, MMGSDI_APP_CSIM);

  mmgsdi_status = mmgsdi_card_init_app_cache(
                    session_id,
                    MMGSDI_APP_CSIM,
                    slot_id,
                    csim_pup_init_pre_esn_table,
                    sizeof(csim_pup_init_pre_esn_table)/
                    sizeof(mmgsdi_cache_init_order_type));

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Some mandatory file cache failed */
    return MMGSDI_UIM_INSTN_CHG_SUCCESS;
  }

  mmgsdi_status = mmgsdi_card_init_post_pin1_esn(session_id, slot_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_UIM_INSTN_CHG_SUCCESS;
  }

  mmgsdi_status = mmgsdi_card_init_app_cache(
                    session_id,
                    MMGSDI_APP_CSIM,
                    slot_id,
                    csim_pup_init_post_esn_table,
                    sizeof(csim_pup_init_post_esn_table)/
                    sizeof(mmgsdi_cache_init_order_type));

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to init CSIM Cache");
    return mmgsdi_status;
  }

  mmgsdi_card_init_1x_srv_nvruim_init(
    session_id,
    slot_id,
    MMGSDI_APP_CSIM);

  mmgsdi_status = mmgsdi_card_init_1x_read_prl_init(session_id);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("Unable to init CSIM PRL");
  }

  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_CSIM_UPP_UPDATE_AFTER_OTASP, nv_context) ==
     MMGSDI_FEATURE_ENABLED)
  {
    mmgsdi_status = mmgsdi_1x_read_mdn_and_validate_upp_files(session_id, NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Unable to read CSIM MDN");
    }
  }

  mmgsdi_card_init_csim_write_model_init(session_id, slot_id);
  mmgsdi_card_init_csim_write_me_crp_init(session_id, slot_id);

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_csim_post_pin1_init */
#endif /* FEATURE_MMGSDI_3GPP2 */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_SIM_POST_PIN1_INIT

DESCRIPTION
  This function performs the post pin1 initialization for SIM

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_sim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  /* If mmgsdi feature absent_mandatory_files_init is enabled, file is not
     mandatory */
  boolean is_mandatory =
    (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ABSENT_MANDATORY_FILES_INIT,
       mmgsdi_util_get_efs_item_index_for_slot(slot_id)) ==
      MMGSDI_FEATURE_ENABLED) ? FALSE : TRUE;

  /* When adding EFs to this list, consider adding them to
     mmgsdi_util_find_ef_in_power_up_cache_list also */
  mmgsdi_cache_init_order_type sim_pup_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_GSM_HPLMN,     is_mandatory, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_LOCI,      TRUE,         mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_FPLMN,     TRUE,         mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_PLMN,      FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_HPLMNACT,  FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_PLMNWACT,  FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_OPLMNWACT, FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_CBMID,     FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_ACM,       FALSE,        mmgsdi_card_init_cache_record, NULL},
     {MMGSDI_GSM_ACM_MAX,   FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_SPN,       FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_SPDI,      FALSE,        mmgsdi_card_init_cache_binary, NULL}};

  UIM_MSG_HIGH_1("mmgsdi_card_init_sim_post_pin1_init, slot:0x%x", slot_id);

  (void)mmgsdi_card_init_hzi_init(session_id, slot_id, MMGSDI_APP_SIM);

  /* Now read files on the card and cache the contents */
  mmgsdi_status = mmgsdi_card_init_app_cache(
                    session_id,
                    MMGSDI_APP_SIM,
                    slot_id,
                    sim_pup_init_table,
                    sizeof(sim_pup_init_table)/
                    sizeof(mmgsdi_cache_init_order_type));

  mmgsdi_card_init_overwrite_oplmnwact_cache(session_id, slot_id,
                                             MMGSDI_APP_SIM);

  return mmgsdi_status;
} /* mmgsdi_card_init_sim_post_pin1_init */


#ifdef FEATURE_MMGSDI_3GPP2
/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_RUIM_POST_PIN1_INIT

DESCRIPTION
  This function performs the post pin1 initialization for RUIM

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_ruim_post_pin1_init (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type      *slot_data_ptr = NULL;

  mmgsdi_init_cache_func_type eprl_init_cache_func =
    (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_RUIM_EPRL_ACCESS,MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_DISABLED) ?
      mmgsdi_card_init_cache_prl : NULL;

  mmgsdi_cache_init_order_type ruim_pup_init_post_esn_table[] =
    /* file name,        mandatory, init func,               validation func */
    {{MMGSDI_CDMA_ACCOLC,              TRUE,  mmgsdi_card_init_cache_binary,         NULL},
     {MMGSDI_CDMA_OTAPA_SPC_ENABLE,    TRUE,  mmgsdi_card_init_cache_binary,         NULL},
     {MMGSDI_CDMA_NAM_LOCK,            TRUE,  mmgsdi_card_init_cache_binary,         NULL},
     {MMGSDI_CDMA_SYS_REGN_IND,        TRUE,  mmgsdi_card_init_cache_binary,         NULL},
     {MMGSDI_CDMA_DIST_BASED_REGN_IND, TRUE,  mmgsdi_card_init_cache_binary,         NULL},
     {MMGSDI_CDMA_ZONE_BASED_REGN_IND, TRUE,  mmgsdi_card_init_cache_record,         NULL},
     {MMGSDI_CDMA_HOME_SID_NID,        TRUE,  mmgsdi_card_init_select_and_validate,  NULL},
     {MMGSDI_CDMA_PRL,                 FALSE, mmgsdi_card_init_cache_prl,            NULL},
     {MMGSDI_CDMA_EPRL,                FALSE, eprl_init_cache_func,                  NULL}};

  UIM_MSG_HIGH_1("mmgsdi_card_init_ruim_post_pin1_init, slot:0x%x", slot_id);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_card_init_post_pin1_esn(session_id, slot_id);
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Add Code to call Diag Function to update Cache */
    diag_otasp_update();
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }
  else if(slot_data_ptr->dcs1800_present)
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_card_init_app_cache(
                    session_id,
                    MMGSDI_APP_RUIM,
                    slot_id,
                    ruim_pup_init_post_esn_table,
                    sizeof(ruim_pup_init_post_esn_table)/
                    sizeof(mmgsdi_cache_init_order_type));

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to init RUIM Cache");
    return mmgsdi_status;
  }

  mmgsdi_card_init_1x_srv_nvruim_init(
    session_id,
    slot_id,
    MMGSDI_APP_RUIM);

  mmgsdi_status = mmgsdi_card_init_1x_read_prl_init(session_id);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("Unable to init RUIM PRL");
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_ruim_post_pin1_init */
#endif /* FEATURE_MMGSDI_3GPP2 */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROC_POST_PIN1_INIT

DESCRIPTION
  Depending on the application being intialized, appropriate post-pin1
  initalization routines are performed.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_proc_post_pin1_init(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_app_enum_type     app_type
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  uim_slot_type           uim_slot      = UIM_SLOT_NONE;
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_client_id_type   client_id     = 0;
#endif /* FEATURE_MMGSDI_3GPP2 */
  mmgsdi_refresh_req_type req;

  memset(&req, 0x00, sizeof(mmgsdi_refresh_req_type));

  /* ------------------------------------------------------------------------
  Vote the UIM Clock On before starting the Post PIN1 Procedures.
  Will keep the clock from being continously turned off / on for
  each one of the requests that follows
  --------------------------------------------------------------------------*/
  /* Convert to UIM slot */
  (void)mmgsdi_util_convert_uim_slot(slot_id,
                                     &uim_slot);

  (void)uim_power_control_ds(uim_slot,UIM_USIM_SESSION, FALSE, UIM_CALL_TYPE_NOT_APPLICABLE);

  /* ------------------------------------------------------------------------
  POST PIN1 Initialization procedures for CPHS.
  -------------------------------------------------------------------------*/
  if ((app_type == MMGSDI_APP_USIM) || (app_type == MMGSDI_APP_SIM))
  {
    mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs;
    read_cphs_efs.read_cphsi     = TRUE;
    read_cphs_efs.read_csp       = TRUE;
    read_cphs_efs.read_ons       = TRUE;
    read_cphs_efs.read_ons_short = TRUE;
    if(MMGSDI_SUCCESS != mmgsdi_proc_cphs_post_pin1_init(session_id,
                                                         slot_id,
                                                         app_type,
                                                         read_cphs_efs))
    {
      UIM_MSG_ERR_0("CPHS post-pin1 Init. failure");
      /* Reset the mmgsdi_status - No CPHS does not mean
         we need to error out */
      mmgsdi_status = MMGSDI_SUCCESS;
    }
  }

  /* ------------------------------------------------------------------------
  Perform POST PIN1 procedures for the application being intialized
  -------------------------------------------------------------------------*/
  switch(app_type)
  {
    case MMGSDI_APP_SIM:
      mmgsdi_status = mmgsdi_card_init_sim_post_pin1_init(session_id, slot_id);
      break;

#ifdef FEATURE_MMGSDI_3GPP2
    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_card_init_ruim_post_pin1_init(session_id, slot_id);
      break;
#endif /* FEATURE_MMGSDI_3GPP2 */

    case MMGSDI_APP_USIM:
      mmgsdi_status = mmgsdi_card_init_usim_post_pin1_init(session_id, slot_id);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        /* ----------------------------------------------------------------------
           Queue in a command to send out a status command to indicate that the
           ME is ready for a 3G session If this is an UICC.  This also solves the
           purpose of sending a case 2 command with le > luic for testing
           GCF test case 7.2.3 if this is an UICC.  For a sim this would
           translate to a regular status command.
           --------------------------------------------------------------------*/
        mmgsdi_card_init_3G_ready_status_ind(session_id);
      }
      break;

#ifdef FEATURE_MMGSDI_3GPP2
    case MMGSDI_APP_CSIM:
      mmgsdi_status = mmgsdi_card_init_csim_post_pin1_init(session_id, slot_id);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        /* ----------------------------------------------------------------------
           Queue in a command to send out a status command to indicate that the
           ME is ready for a 3G session If this is an UICC.  This also solves the
           purpose of sending a case 2 command with le > luic for testing
           GCF test case 7.2.3 if this is an UICC.  For a sim this would
           translate to a regular status command.
           --------------------------------------------------------------------*/
        mmgsdi_card_init_3G_ready_status_ind(session_id);
      }
      /* To address specific test cases: When mandatory files are missing from
         CSIM and there are no USIM apps attempt to re-init the card in RUIM mode */
      else if (mmgsdi_status == MMGSDI_UIM_INSTN_CHG_SUCCESS)
      {
        if(mmgsdi_util_get_usim_apps(slot_id) == 0)
        {
          UIM_MSG_HIGH_0("CSIM post pin1 error. Try RUIM");

          if (MMGSDI_SUCCESS != mmgsdi_util_get_client_id(session_id, &client_id))
          {
            UIM_MSG_HIGH_1("Could not get client id for session_id:0x%x",
                           session_id);
            return MMGSDI_ERROR;
          }

          req.refresh.card_req.refresh_mode    = MMGSDI_REFRESH_RESET_AUTO;
          req.refresh_req_type                 = MMGSDI_REFRESH_ORIG_PUP_REQ;

          req.request_header.request_type      = MMGSDI_REFRESH_REQ;
          req.request_header.orig_request_type = MMGSDI_CARD_PUP_REQ;
          req.request_header.client_id         = client_id;
          req.request_header.session_id        = session_id;
          req.request_header.slot_id           = slot_id;
          req.request_header.client_data       = 0;
          req.request_header.response_cb       = mmgsdi_util_internal_cb;

          req.request_header.request_len = sizeof(mmgsdi_refresh_req_type);
          req.request_header.payload_len = uint32toint32(sizeof(mmgsdi_refresh_req_type)
                                            - sizeof(mmgsdi_request_header_type));

          req.refresh.card_req.switch_protocol = TRUE;
          req.refresh.card_req.source          = MMGSDI_REFRESH_SOURCE_MMGSDI;

          if (MMGSDI_SUCCESS != mmgsdi_refresh_main(&req))
          {
            UIM_MSG_ERR_0("REFRESH ERROR during prtocol switch after missing mandatory CSIM files");
            return MMGSDI_ERROR;
          }

          return MMGSDI_UIM_INSTN_CHG_SUCCESS;
        }
        else
        {
          mmgsdi_status = MMGSDI_ERROR;
        }
      }
      break;
#endif /* FEATURE_MMGSDI_3GPP2 */

    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }

  return mmgsdi_status;
} /* mmgsdi_card_init_proc_post_pin1_init */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_SET_PRE_PERSO_CACHE_STATUS

DESCRIPTION
  This function sets the pre perso files cache status in the app info.
  The MMGSDI checks this status, to determine if the run algorithm request
  to the simlock can be sent for this app.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_set_pre_perso_cache_status (
  mmgsdi_session_id_type                 session_id
)
{
  mmgsdi_return_enum_type                 mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type              * app_info_ptr  = NULL;

  /* Get app info of the application */
  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id, NULL, NULL,
                                                   &app_info_ptr, NULL,
                                                   NULL, NULL);

  if((mmgsdi_status == MMGSDI_SUCCESS) &&
     (app_info_ptr != NULL))
  {
    app_info_ptr->pre_perso_files_cached = TRUE;
  }
} /* mmgsdi_card_init_set_pre_perso_cache_status */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROC_PRE_PERSO_CACHE

DESCRIPTION
  This function performs the pre perso initialization for SIM / USIM. These
  files are cached before personalization to ensure that they are available
  in cache for NAS to run authentication in perso locked state.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_proc_pre_perso_cache (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_app_enum_type     app_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status       = MMGSDI_SUCCESS;
  boolean                  is_sp_lock_active   = TRUE;
  boolean                  is_cp_lock_active   = TRUE;
  boolean                  is_spn_lock_active  = TRUE;

  /* If mmgsdi feature absent_mandatory_files_init is enabled, file is not
     mandatory */
  boolean is_mandatory =
    (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ABSENT_MANDATORY_FILES_INIT,
       mmgsdi_util_get_efs_item_index_for_slot(slot_id)) ==
      MMGSDI_FEATURE_ENABLED) ? FALSE : TRUE;

  /* When adding EFs to this list, consider adding them to
     mmgsdi_util_find_ef_in_power_up_cache_list also */
  mmgsdi_cache_init_order_type sim_pre_perso_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_GSM_AD,        is_mandatory, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_IMSI,      TRUE,         mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_imsi_data                                    },
     {MMGSDI_GSM_ACC,       is_mandatory, mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_acc_data                                     },
     {MMGSDI_GSM_KC,        is_mandatory, mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_gsm_kc},
     {MMGSDI_GSM_LOCIGPRS,  FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_KCGPRS,    FALSE,        mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_gsm_kcgprs}
    };

  mmgsdi_cache_init_order_type sim_pre_perso_gid1_and_gid2_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_GSM_GID1,      FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_GSM_GID2,      FALSE,        mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type sim_pre_perso_gid1_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_GSM_GID1,      FALSE,        mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type sim_pre_perso_spn_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_GSM_SPN,      FALSE,        mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type usim_pre_perso_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_USIM_AD,        is_mandatory, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_IMSI,      TRUE,         mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_imsi_data                                     },
     {MMGSDI_USIM_ACC,       is_mandatory, mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_acc_data                                      },
     {MMGSDI_USIM_LOCI,      is_mandatory, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_PSLOCI,    TRUE,         mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_EPSLOCI,   FALSE,        mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_epsloci                                       },
     {MMGSDI_USIM_KEYS,      is_mandatory, mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_keys},
     {MMGSDI_USIM_KEYSPS,    FALSE,        mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_keysps},
     {MMGSDI_USIM_EPSNSC  ,  FALSE,        mmgsdi_card_init_cache_record,
      mmgsdi_card_init_validate_epsnsc                                        },
     {MMGSDI_USIM_START_HFN, is_mandatory, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_THRESHOLD, is_mandatory, mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_KC,        FALSE,        mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_usim_kc_kcgprs},
     {MMGSDI_USIM_KCGPRS,    FALSE,        mmgsdi_card_init_cache_binary,
      mmgsdi_card_init_validate_usim_kc_kcgprs}
    };

  mmgsdi_cache_init_order_type usim_pre_perso_gid1_and_gid2_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_USIM_GID1,      FALSE,        mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_USIM_GID2,      FALSE,        mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type usim_pre_perso_gid1_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_USIM_GID1,      FALSE,        mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type usim_pre_perso_spn_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_USIM_SPN,      FALSE,        mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type ruim_pre_perso_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_CDMA_IMSI_M,            TRUE,  mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_CDMA_IMSI_T,            TRUE,  mmgsdi_card_init_cache_binary, NULL}
    };

  mmgsdi_cache_init_order_type csim_pre_perso_init_table[] =
    /* file name,        mandatory, init func,               validation func */
    {
     {MMGSDI_CSIM_IMSI_M,            TRUE,  mmgsdi_card_init_cache_binary, NULL},
     {MMGSDI_CSIM_IMSI_T,            TRUE,  mmgsdi_card_init_cache_binary, NULL}
    };

  UIM_MSG_HIGH_2("mmgsdi_card_init_proc_pre_perso_cache, slot:0x%x, app_type: 0x%x",
                 slot_id, app_type);

#ifdef FEATURE_SIMLOCK
  if((app_type == MMGSDI_APP_SIM) ||
     (app_type == MMGSDI_APP_USIM))
  {
    is_sp_lock_active  = mmgsdi_simlock_check_category_status(slot_id, SIMLOCK_CATEGORY_3GPP_SP);
    is_cp_lock_active  = mmgsdi_simlock_check_category_status(slot_id, SIMLOCK_CATEGORY_3GPP_CP);
    is_spn_lock_active = mmgsdi_simlock_check_category_status(slot_id, SIMLOCK_CATEGORY_3GPP_SPN);
  }
#endif /* FEATURE_SIMLOCK*/

  /* read files on the card and cache the contents */
  switch(app_type)
  {
     case MMGSDI_APP_SIM:
       /* Get SIM capabilities to ensure IMSI is rehabilitated before caching */
       (void)mmgsdi_icc_get_sim_capabilities(session_id, slot_id);

       mmgsdi_status = mmgsdi_card_init_app_cache(session_id,
                                                  MMGSDI_APP_SIM,
                                                  slot_id,
                                                  sim_pre_perso_init_table,
                                                  sizeof(sim_pre_perso_init_table)/
                                                  sizeof(mmgsdi_cache_init_order_type));
       if(mmgsdi_status == MMGSDI_SUCCESS)
       {
         /* If CP lock is active, we read both GID1 and GID2 files */
         if(is_cp_lock_active)
         {
           /* we ignore the return status here as GID files are
              not mandatory */
           (void)mmgsdi_card_init_app_cache(session_id,
                                            MMGSDI_APP_SIM,
                                            slot_id,
                                            sim_pre_perso_gid1_and_gid2_init_table,
                                            sizeof(sim_pre_perso_gid1_and_gid2_init_table)/
                                            sizeof(mmgsdi_cache_init_order_type));
         }
         else if(is_sp_lock_active)
         {
           /* we ignore the return status here as GID files are
              not mandatory */
           (void)mmgsdi_card_init_app_cache(session_id,
                                            MMGSDI_APP_SIM,
                                            slot_id,
                                            sim_pre_perso_gid1_init_table,
                                            sizeof(sim_pre_perso_gid1_init_table)/
                                            sizeof(mmgsdi_cache_init_order_type));
         }

         if(is_spn_lock_active)
         {
           /* we ignore the return status here as SPN files are
              not mandatory */
           (void)mmgsdi_card_init_app_cache(session_id,
                                            MMGSDI_APP_SIM,
                                            slot_id,
                                            sim_pre_perso_spn_init_table,
                                            sizeof(sim_pre_perso_spn_init_table)/
                                            sizeof(mmgsdi_cache_init_order_type));
         }
       }
       break;
     case MMGSDI_APP_USIM:
       mmgsdi_status = mmgsdi_card_init_app_cache(session_id,
                                                  MMGSDI_APP_USIM,
                                                  slot_id,
                                                  usim_pre_perso_init_table,
                                                  sizeof(usim_pre_perso_init_table)/
                                                  sizeof(mmgsdi_cache_init_order_type));
       if(mmgsdi_status == MMGSDI_SUCCESS)
       {
         /* If CP lock is active, we read both GID1 and GID2 files */
         if(is_cp_lock_active)
         {
           /* we ignore the return status here as GID files are
              not mandatory */
           (void)mmgsdi_card_init_app_cache(session_id,
                                            MMGSDI_APP_USIM,
                                            slot_id,
                                            usim_pre_perso_gid1_and_gid2_init_table,
                                            sizeof(sim_pre_perso_gid1_and_gid2_init_table)/
                                            sizeof(mmgsdi_cache_init_order_type));
         }
         else if(is_sp_lock_active)
         {
           /* we ignore the return status here as GID files are
              not mandatory */
           (void)mmgsdi_card_init_app_cache(session_id,
                                            MMGSDI_APP_USIM,
                                            slot_id,
                                            usim_pre_perso_gid1_init_table,
                                            sizeof(sim_pre_perso_gid1_init_table)/
                                            sizeof(mmgsdi_cache_init_order_type));
         }

         if(is_spn_lock_active)
         {
           /* we ignore the return status here as SPN files are
              not mandatory */
           (void)mmgsdi_card_init_app_cache(session_id,
                                            MMGSDI_APP_USIM,
                                            slot_id,
                                            usim_pre_perso_spn_init_table,
                                            sizeof(usim_pre_perso_spn_init_table)/
                                            sizeof(mmgsdi_cache_init_order_type));
         }
       }
       break;
     case MMGSDI_APP_RUIM:
       /* Get RUIM capabilities to ensure IMSI is rehabilitated before caching */
       (void)mmgsdi_icc_get_ruim_capabilities(session_id, slot_id);

       mmgsdi_status = mmgsdi_card_init_app_cache(session_id,
                                                  MMGSDI_APP_RUIM,
                                                  slot_id,
                                                  ruim_pre_perso_init_table,
                                                  sizeof(ruim_pre_perso_init_table)/
                                                  sizeof(mmgsdi_cache_init_order_type));
       break;
     case MMGSDI_APP_CSIM:
       mmgsdi_status = mmgsdi_card_init_app_cache(session_id,
                                                  MMGSDI_APP_CSIM,
                                                  slot_id,
                                                  csim_pre_perso_init_table,
                                                  sizeof(csim_pre_perso_init_table)/
                                                  sizeof(mmgsdi_cache_init_order_type));
       break;
     default:
       UIM_MSG_HIGH_1("no files to cache for app_type: 0x%x", app_type);
       break;
  }

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Set the pre-perso cache status in the app info */
    mmgsdi_card_init_set_pre_perso_cache_status(session_id);
  }

  return mmgsdi_status;
} /* mmgsdi_card_init_proc_pre_perso_cache */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_3G_READY_STATUS_IND

DESCRIPTION
  Function used to Queue Back a Command to send a status indicating that the
  ME is ready for a 3G session if it is a UICC card.  The MMGSDI Task will
  process the command at a later time.  This would be translated to a regular
  status command for a non UICC card

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_3G_ready_status_ind(
  mmgsdi_session_id_type   session_id
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_channel_info_type * channel_info_ptr = NULL;

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(session_id,
                                                           NULL,
                                                           &channel_info_ptr);
  if(mmgsdi_status == MMGSDI_SUCCESS &&
     channel_info_ptr != NULL &&
     channel_info_ptr->selective_act_after_app_term_skipped)
  {
    /* Do not send STATUS if the app on the card was not terminated previously */
    UIM_MSG_HIGH_0("Not sending STATUS since the app was not terminated");
    channel_info_ptr->selective_act_after_app_term_skipped = FALSE;
    return;
  }

  mmgsdi_status = mmgsdi_session_send_card_status(
                    session_id,
                    MMGSDI_STATUS_APP_INITIALIZED,
                    MMGSDI_STATUS_TLV_DATA,
                    mmgsdi_util_internal_cb,
                    0x00);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    UIM_MSG_ERR_0("Could not send Card Status Command");
  }
  else
  {
    UIM_MSG_HIGH_0("CARD STATUS REQ Sent");
  }
} /* mmgsdi_card_init_3G_ready_status_ind */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_END_REFRESH_IN_PROGRESS

DESCRIPTION:
  This function is used to end any REFRESH in progress on the application
  whose initalization has failed.

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_end_refresh_in_progress(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_refresh_node_type            *node_ptr        = NULL;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uint8                                slot_index      = MMGSDI_SLOT_1_INDEX;
  mmgsdi_card_refresh_extra_info_type  extra_info;

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    /*No Refresh on Progress, return*/
    UIM_MSG_ERR_0("No Refresh is in progress");
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id != slot_id)
  {
    UIM_MSG_HIGH_0("No Refresh is in progress on current slot");
    return MMGSDI_SUCCESS;
  }

  memset(&extra_info, 0x00, sizeof(mmgsdi_card_refresh_extra_info_type));

  UIM_MSG_HIGH_2("Refresh in progress, mode: 0x%x, slot: 0x%x",
                 mmgsdi_refresh_info_ptr[slot_index]->mode, slot_id);

  switch(mmgsdi_refresh_info_ptr[slot_index]->mode)
  {
    /* If ongoing REFRESH is application specific and corresponds to the
            passed in session_id, stop the REFRESH */
    case MMGSDI_REFRESH_NAA_INIT:
    case MMGSDI_REFRESH_NAA_INIT_FCN:
    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
    case MMGSDI_REFRESH_NAA_APP_RESET:
    case MMGSDI_REFRESH_3G_SESSION_RESET:
    case MMGSDI_REFRESH_APP_RESET_AUTO:
      node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
      while(node_ptr)
      {
        if(node_ptr->session_id == session_id)
        {
          UIM_MSG_HIGH_1("Stopping REFRESH on Session ID: 0x%x", session_id);

          /* Send out REFRESH END_FAILED event to all clients */
          extra_info.mode     = mmgsdi_refresh_info_ptr[slot_index]->mode;
          extra_info.send_evt = TRUE;
          extra_info.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
          mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

          (void)mmgsdi_refresh_send_evt(&extra_info,
                                        &mmgsdi_refresh_info_ptr[slot_index]->request_header);

          /* End ongoing REFRESH and buid confirmation */
          (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                  &mmgsdi_refresh_info_ptr[slot_index]->request_header);
          break;
        }
        node_ptr = node_ptr->next_ptr;
      }
      break;
    case MMGSDI_REFRESH_RESET:
    case MMGSDI_REFRESH_NAA_FCN:
    case MMGSDI_REFRESH_STEERING_OF_ROAMING:
    case MMGSDI_REFRESH_RESET_AUTO:
    case MMGSDI_REFRESH_MODE_ENUM_MAX:
      break;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_end_refresh_in_progress */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_CARD_INSERTED

DESCRIPTION
  This function processes the CARD_INSERTED event for the specific slot and
  calls the session_activate_or_switch_provisioning API to trigger the
  SESSION_CHANGED (Activate = TRUE) event

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_process_card_inserted(
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_slot_data_type             *slot_data_ptr                = NULL;
  mmgsdi_data_type                   aid;
  mmgsdi_nv_item_cache_type          mmgsdi_nv_data               = {0};
  nv_stat_enum_type                  nv_status                    = NV_FAIL_S;
  uint8                              auto_activate_multiple_slots = 1;

  UIM_MSG_HIGH_1("In mmgsdi_card_init_process_card_inserted, slot: 0x%x", slot_id);

  memset(&aid, 0x00, sizeof(mmgsdi_data_type));
  mmgsdi_nv_data.item_value.uim_select_default_usim_app = FALSE;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED)
  {
    UIM_MSG_ERR_2("mmgsdi_state for mmgsdi slot 0x%x != CARD_INSERTED",
                  slot_id, slot_data_ptr->mmgsdi_state);
    return MMGSDI_ERROR;
  }

  /* Find out if MMGSDI needs to do provisioning or not */
  /* IF FEATURE_CUST_1 is defined,
     1. If 4398 is inactive or if it is set to 1:
        1(a). If the auto_activate_slots EFS item is inactive or set to 0,
              auto-activate slot1 apps (at UIM[0]) as PRI. Do not activate
              apps on other slots.
        1(b). If NEW EFS item is active and set to 1, auto-activate slot1
              apps (at UIM[0]) as PRI, slot2 apps (at UIM[1]) as SEC,
              slot3 apps (at UIM[2]]) as TER.
     2. If 4398 is active and set to 0, do not auto-activate any app.
     ELSE
     1. If 4398 is inactive or if it is set to 1, auto-activate slot1
        apps (at UIM[0]) as PRI, slot2 apps (at UIM[1]) as SEC,
        slot3 apps (at UIM[2]]) as TER..
     2. If 4398 is active and set to 0, do not auto-activate any app. */
  nv_status = mmgsdi_nv_get_item(NV_UIM_SELECT_DEFAULT_USIM_APP_I,
                                 &mmgsdi_nv_data, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  UIM_MSG_HIGH_2("NV_UIM_SELECT_DEFAULT_USIM_APP_I Read: Status: 0x%x, Select Default App.: 0x%x",
                 nv_status, mmgsdi_nv_data.item_value.uim_select_default_usim_app);

  if ((nv_status != NV_DONE_S) ||
      (mmgsdi_nv_data.item_value.uim_select_default_usim_app == TRUE))
  {
#ifdef FEATURE_CUST_1
    if (slot_id != MMGSDI_SLOT_1)
    {
      if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_AUTO_ACTIVATE_MULTI_SLOTS_FILE,
                              UIM_COMMON_EFS_ITEM_FILE_TYPE,
                              UIM_COMMON_EFS_DEVICE,
                              &auto_activate_multiple_slots,
                              sizeof(auto_activate_multiple_slots)) != UIM_COMMON_EFS_SUCCESS)
      {
        /* EFS read failed.. reset the variable to 0 */
        auto_activate_multiple_slots = 0;
      }
    }
#endif /* FEATURE_CUST_1 */
    if (auto_activate_multiple_slots > 0)
    {
      mmgsdi_util_activate_first_app(slot_id);
    }
  }
  else
  {
    if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_MULTISIM_AUTO_PROVISIONING, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
       == MMGSDI_FEATURE_ENABLED)
    {
      UIM_MSG_HIGH_1("Feature MMGSDI_FEATURE_MULTISIM_AUTO_PROVISIONING is enabled, Slot 0x%x",
                     slot_id);
      mmgsdi_provision_applications_from_efs(slot_id);
    }
    else
    {
      /* If nv is disabled and efs content is present there will be a mismatch, so delete EFS */
      mmgsdi_delete_efs_subs_prov();
    }
  }

  mmgsdi_init_rtre_config();
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_process_card_inserted */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_PROV_SESSION_ACTIVATED

DESCRIPTION
  This function processes the SESSION_CHANGED (activated) event for the
  provisioning application.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_process_prov_session_activated(
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_state_enum_type app_state_req
)
{
  mmgsdi_return_enum_type       mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type      slot_id             = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_slot_data_type        *slot_data_ptr       = NULL;
  mmgsdi_int_app_info_type     *int_app_info_ptr    = NULL;

  /* Based on the session ID, retrieve the Slot and app info */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      int_app_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_5("In mmgsdi_card_init_process_prov_session_activated, session_id 0x%x, "
                 "slot_id 0x%x, app_type 0x%x, request app_state 0x%x, current app_state 0x%x",
                 session_id,
                 slot_id,
                 int_app_info_ptr->app_data.app_type,
                 app_state_req,
                 int_app_info_ptr->app_state);

  /* Do not continue processing if there is a mismatch in the requested app state
     and the actual app state */
  if(app_state_req != int_app_info_ptr->app_state)
  {
    return MMGSDI_APP_STATE_DENIED;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Get the App capabilities */
  mmgsdi_status = mmgsdi_card_init_get_app_capability(
                  session_id,
                  slot_data_ptr,
                  slot_id,
                  int_app_info_ptr->app_data.app_type);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Get the PIN info for the provisioning application */
  if (mmgsdi_session_get_all_pin_status (session_id,
                                         mmgsdi_util_internal_cb,
                                         0) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_process_prov_session_activated */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_POST_PIN1

DESCRIPTION
  This function processes the PIN1/UPIN event for the provisioning
  application as part of the application initialization.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_process_post_pin1(
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_state_enum_type app_state_req
)
{
  mmgsdi_return_enum_type   mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type  slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type *int_app_info_ptr  = NULL;
  mmgsdi_tp_state_enum_type tp_state          = MMGSDI_MAX_TP_STATE;
  mmgsdi_slot_data_type    *slot_data_ptr     = NULL;
  uint8                     slot_index        = 0;

#ifndef FEATURE_SIMLOCK
  mmgsdi_evt_session_notify_type  notify_type;
  boolean                         mmgsdi_halt_sub_setting = FALSE;
  mmgsdi_perso_status_enum_type   perso_status            = MMGSDI_PERSO_STATUS_NONE;
#endif /* !FEATURE_SIMLOCK */

  UIM_MSG_HIGH_1("In mmgsdi_card_init_process_post_pin1, session_id: 0x%x",
                 session_id);

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Do not continue processing if there is a mismatch in the requested app state
     and the actual app state */
  if(app_state_req != int_app_info_ptr->app_state)
  {
    UIM_MSG_ERR_2("App. state mismatch, Req: 0x%x,Current state: 0x%x",
                  app_state_req, int_app_info_ptr->app_state);
    return MMGSDI_APP_STATE_DENIED;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  tp_state = slot_data_ptr->mmgsdi_tp_state;

#ifdef FEATURE_GSTK
  if (((MMGSDI_TP_STATE_NOT_DONE           == tp_state)   ||
       (MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK == tp_state))   &&
      (int_app_info_ptr->app_tp_state != MMGSDI_TP_STATE_NOT_NEEDED))
  {
    mmgsdi_card_init_perform_post_pin1_tp_if_needed(
      session_id,
      slot_id,
      int_app_info_ptr->app_data.app_type);

    return MMGSDI_SUCCESS;
  }
  else if(MMGSDI_TP_STATE_IN_PROGRESS_OK == tp_state)
  {
    if(int_app_info_ptr->app_tp_state == MMGSDI_TP_STATE_NOT_DONE)
    {
      UIM_MSG_HIGH_1("TP in progress for slot 0x%x, need to set right app state",
                     slot_id);
      int_app_info_ptr->app_tp_state = MMGSDI_TP_STATE_IN_PROGRESS_OK;
      return MMGSDI_SUCCESS;
    }
  }
  else if(MMGSDI_TP_STATE_DONE == tp_state)
  {
    UIM_MSG_HIGH_1("TP Done for slot 0x%x continue with pre-Perso caching",
                   slot_id);
    int_app_info_ptr->app_tp_state = MMGSDI_TP_STATE_DONE;
  }
#endif /* FEATURE_GSTK */

  /* Caching files before MMGSDI Perso event */
  mmgsdi_status = mmgsdi_card_init_proc_pre_perso_cache(session_id,
                                                        slot_id,
                                                        int_app_info_ptr->app_data.app_type);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_card_init_proc_pre_perso_cache with mmgsdi_status:0x%x",
                  mmgsdi_status);

    /* If the error is due to halt of initializatoin then return success.
       This is done to make sure session does not get deactivated for buggy cards */
    if(mmgsdi_status == MMGSDI_HALT_INIT_ERROR)
    {
      mmgsdi_status = MMGSDI_SUCCESS;
    }

    return mmgsdi_status;
  }

#ifdef FEATURE_SIMLOCK
   /* As per the 22.022 spec., one of the OTA deperso (section - 9) mechanisms
     involve updating the control keys in the EF-DCK file and triggering a REFRESH.
     So in case the initialization procedure is being performed as a result
     of a REFRESH, need to check the EF-DCK and perform deperso accordingly. */
   if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
      ((mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT)     ||
       (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN) ||
       (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN) ||
       (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_APP_RESET) ||
       (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET)))
  {
    (void)mmgsdi_simlock_refresh_process_unlock(session_id);
  }

  mmgsdi_status = mmgsdi_simlock_build_and_run_algo(FALSE);
#else
  /* Get halt subscription config value from efs */
  if(uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_HALT_SUBSCRIPTION,
                         UIM_COMMON_EFS_ITEM_FILE_TYPE,
                         UIM_COMMON_EFS_DEVICE,
                         (uint8*) &mmgsdi_halt_sub_setting,
                         sizeof(mmgsdi_halt_sub_setting)) != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_HIGH_0("Halt subscription config read failed. Sending perso evt with DONE status");
    perso_status = MMGSDI_PERSO_STATUS_DONE;
  }
  else if (mmgsdi_halt_sub_setting == FALSE)
  {
    UIM_MSG_HIGH_0("Halt subscription config disabled. Sending perso evt with DONE status");
    perso_status = MMGSDI_PERSO_STATUS_DONE;
  }
  else
  {
    UIM_MSG_HIGH_0("Halting subscription. Waiting for SUB OK request");

    /* No change in state. Continue to remain in wait for SUB OK state */
    perso_status = MMGSDI_PERSO_STATUS_WAIT_FOR_EXT_PERSO;
  }

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;
  mmgsdi_evt_build_and_send_perso(
    notify_type,
    (const mmgsdi_int_app_info_type*)int_app_info_ptr,
    perso_status,
    MMGSDI_MAX_PERSO_FEATURE_ENUM,
    0,
    0);
#endif /* FEATURE_SIMLOCK */
  return mmgsdi_status;
} /*mmgsdi_card_init_process_post_pin1 */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_POST_PERSO

DESCRIPTION
  This function processes the Perso event for the provisioning
  application as part of the application initialization.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_card_init_process_post_perso(
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_state_enum_type app_state_req
)
{
  mmgsdi_return_enum_type     mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type    slot_id            = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type   *int_app_info_ptr   = NULL;

  UIM_MSG_HIGH_1("In mmgsdi_card_init_process_post_perso, session_id: 0x%x",
                 session_id);

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Do not continue processing if there is a mismatch in the requested app state
     and the actual app state */
  if(app_state_req != int_app_info_ptr->app_state)
  {
    UIM_MSG_ERR_2("App. state mismatch, Req: 0x%x,Current state: 0x%x",
                  app_state_req, int_app_info_ptr->app_state);
    return MMGSDI_APP_STATE_DENIED;
  }

  /* We need IMSI M data during OPLMNwACT caching (part of GW post pin1
     init procedure) to determine use of builtin PLMN (stored in EFS) or
     card EF for caching. So if IMSI M is not cached i.e. CSIM/RUIM app is
     not reached to APP_READY we suspends GW post pin1 init procedure.
     Note that IMSI M caching can be performed during personalization of
     1x and we don't need to wait till CSIM/RUIM reaches SUB_READY always.
     But we know personalization can be disabled or external perso can be used
     so it is required to wait till CSIM/RUIM reaches SUB_READY. */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_OPLMN_LIST_CONFIGURABLE_OTA, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
  {
    if((int_app_info_ptr->app_data.app_type == MMGSDI_APP_SIM ||
        int_app_info_ptr->app_data.app_type == MMGSDI_APP_USIM) &&
       (MMGSDI_SUCCESS == mmgsdi_builtin_plmn_list_is_configured()))
    {
      boolean app_in_ready_state = FALSE;
      mmgsdi_status = mmgsdi_util_check_3gpp2_app_in_ready_state(slot_id, &app_in_ready_state);

      /* If 3GPP2 app is not reached to READY state, suspend 3GPP app activation */
      if(app_in_ready_state == FALSE)
      {
        return mmgsdi_status;
      }
    }
  }

  /* Perform specified initialization procedures */
  mmgsdi_status = mmgsdi_card_init_proc_post_pin1_init(
                    session_id,
                    slot_id,
                    int_app_info_ptr->app_data.app_type);

  /*If post-pin1 init. is successful, send Refresh Complete*/
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_refresh_send_complete(0,
                                 session_id,
                                 slot_id,
                                 TRUE);
  }

  return mmgsdi_status;
} /*mmgsdi_card_init_process_post_perso */


/*===========================================================================
FUNCTION:      MMGSDI_REMOVE_APPS_AVAILABLE_INFO

DESCRIPTION
  Function to update the application availability info for the slot based on which
  application type has been removed as a provisioning application

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void mmgsdi_card_init_remove_apps_available_info(
  mmgsdi_slot_data_type *slot_data_ptr,
  mmgsdi_app_enum_type   app_type
)
{
  if(slot_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("slot_data_ptr is NULL");
    return;
  }

  /* Update application availability info for the slot */
  switch(app_type)
  {
    case MMGSDI_APP_SIM:
      slot_data_ptr->card_apps_available &= ~MMGSDI_GSM_APP_MASK;
      break;

    case MMGSDI_APP_RUIM:
      slot_data_ptr->card_apps_available &= ~MMGSDI_RUIM_APP_MASK;
      break;

    case MMGSDI_APP_USIM:
      slot_data_ptr->card_apps_available &= ~MMGSDI_USIM_APP_MASK;
      break;

    case MMGSDI_APP_CSIM:
      slot_data_ptr->card_apps_available &= ~MMGSDI_CSIM_APP_MASK;
      break;

    default:
      UIM_MSG_ERR_1("Unhandled app type 0x%x", app_type);
      break;
  }
} /* mmgsdi_card_init_remove_apps_available_info */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_INTERNAL_PUP_INIT_STATUS

DESCRIPTION
  Function that shall be called upon finishing each internal pup init
  request processing.
  The function will perform necessary clean up to the global data and send
  out relevant MMGSDI event as needed.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void mmgsdi_card_init_process_internal_pup_init_status(
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type internal_pup_status
)
{

  mmgsdi_slot_id_enum_type        slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type         mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type       *int_app_info_ptr = NULL;
  mmgsdi_slot_data_type          *slot_data_ptr    = NULL;
  uim_slot_type                   uim_slot         = UIM_SLOT_NONE;
  mmgsdi_evt_session_notify_type  notify_type;

  UIM_MSG_HIGH_1("In mmgsdi_card_init_process_internal_pup_init_status, status: 0x%x",
                 internal_pup_status);

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  switch(internal_pup_status)
  {
    case MMGSDI_SUCCESS:
      if (int_app_info_ptr->app_state == MMGSDI_APP_STATE_PERSO_VERIFIED)
      {
        /* We need IMSI M data during OPLMNwACT caching (part of GW post pin1
           init procedure) to determine use of builtin PLMN (stored in EFS) or
           card EF for caching. So if IMSI M is not cached i.e. CSIM/RUIM app is
           not reached to APP_READY we suspends GW post pin1 init procedure.
           Note that IMSI M caching can be performed during personalization of
           1x and we don't need to wait till CSIM/RUIM reaches SUB_READY always.
           But we know personalization can be disabled or external perso can be
           used so it is required to wait till CSIM/RUIM reaches SUB_READY. */
        if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_OPLMN_LIST_CONFIGURABLE_OTA, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
        {
          if(MMGSDI_SUCCESS == mmgsdi_builtin_plmn_list_is_configured())
          {
            if((int_app_info_ptr->app_data.app_type == MMGSDI_APP_SIM ||
                int_app_info_ptr->app_data.app_type == MMGSDI_APP_USIM))
            {
              boolean app_in_ready_state = FALSE;

              mmgsdi_status = mmgsdi_util_check_3gpp2_app_in_ready_state(
                                slot_id,
                                &app_in_ready_state);

              /* If 3GPP2 app is not reached to READY state, suspend 3GPP app activation */
              if(app_in_ready_state == FALSE)
              {
                return;
              }
            }
            else if(int_app_info_ptr->app_data.app_type == MMGSDI_APP_RUIM ||
                    int_app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM)
            {
              /* If current APP type is CSIM/RUIM check USIM/GSM app is stuck in
                 PERSO_VERIFIED state or not. If yes, resume their activation because
                 IMSI M caching is done as part of 1x post pin1 init. */
              mmgsdi_util_unblock_apps_for_post_perso(slot_id,
                                                      MMGSDI_TECH_3GPP);
            }
          }
        }

        UIM_MSG_HIGH_0("Publishing subcription");

        mmgsdi_state_update_app_state_for_prov_app(session_id,
          MMGSDI_APP_STATE_PERSO_VERIFIED,
          MMGSDI_APP_STATE_READY,
          NULL);
      }

      if (int_app_info_ptr->app_state == MMGSDI_APP_STATE_READY)
      {
        mmgsdi_nv_context_type nv_context = MMGSDI_NV_CONTEXT_INVALID;

        /* build the subscription_ready event, and broadcast to all */
        notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
        notify_type.slot_id     = slot_id;
        notify_type.session_id  = session_id;
        mmgsdi_evt_build_and_send_subscription_ready(
          notify_type, (const mmgsdi_int_app_info_type*)int_app_info_ptr);

        /* Convert to UIM slot */
        (void)mmgsdi_util_convert_uim_slot(slot_id,
                                           &uim_slot);

        (void)uim_power_control_ds(uim_slot,UIM_USIM_SESSION, TRUE, UIM_CALL_TYPE_NOT_APPLICABLE);

        /* Trigger OPL/PNN read and cache for GW PRI/GW SEC Session*/
        (void)mmgsdi_eons_get_opl_pnn_attr(session_id);

        /* Trigger ACSGL/CSGT/HNBN read and cache for GW PRI/GW SEC Session */
        /* OCSGL/OCSGT/OHNBN EFs reading/caching is triggered later */
        if((mmgsdi_util_get_efs_item_index(session_id, &nv_context) ==
              MMGSDI_SUCCESS) &&
           (mmgsdi_nv_get_external_feature_status(
                  MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG, nv_context) == MMGSDI_FEATURE_ENABLED))
        {
          mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(session_id);
        }
      }
      /* If we are in other state, do nothing, MMGSDI will internally
         move to the next state as needed */
      return;

    case MMGSDI_UIM_INSTN_CHG_SUCCESS:
    case MMGSDI_APP_STATE_DENIED:
      return;

    default:
      switch(int_app_info_ptr->app_data.app_type)
      {
        case MMGSDI_APP_SIM:
        case MMGSDI_APP_RUIM:
        case MMGSDI_APP_USIM:
        case MMGSDI_APP_CSIM:
          /* End any ongoing REFRESH on the Session */
          (void)mmgsdi_card_init_end_refresh_in_progress(session_id,
                                                         slot_id);

          /* Post deactivate session request with internal_deact_req as TRUE */
          (void)mmgsdi_session_create_and_queue_internal_deactivate_req(
                session_id);

          mmgsdi_card_init_remove_apps_available_info(
            slot_data_ptr,
            int_app_info_ptr->app_data.app_type);
          break;
        default:
          UIM_MSG_ERR_1("Invalid prov app type 0x%x",
                        int_app_info_ptr->app_data.app_type);
          return;
      }
      break;
  }
} /* mmgsdi_card_init_process_internal_pup_init_status */


#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_IS_TOOLKIT_ALLOWED

DESCRIPTION
  This function is used to determine whether or not toolkit is allowed.

DEPENDENCIES
  None

RETURNS
    TRUE     Toolkit is allowed
    FALSE    Toolkit is not allowed

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_card_init_is_toolkit_allowed(
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
)
{
  byte                     svc_table_ind    = 0x00;
  mmgsdi_file_enum_type    service_tbl_file = MMGSDI_GSM_SST;
  uint8                    pro_cmd_offset   = 0;
  uint8                    pro_cmd_mask     = 0;
  mmgsdi_return_enum_type  mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type   *slot_data_ptr    = NULL;
  mmgsdi_data_type         mmgsdi_data_buf;

  memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));

  /* Check if Toolkit functionality (Terminal Profile, FETCH,
     Envelopes) is disabled for the slot */
  if(mmgsdi_nv_get_toolkit_enabled_status(slot) == MMGSDI_FEATURE_DISABLED)
  {
    return FALSE;
  }

  switch (app_type)
  {
    case MMGSDI_APP_USIM:
    case MMGSDI_APP_CSIM:
      return TRUE;

    case MMGSDI_APP_SIM:
      slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

      if(slot_data_ptr == NULL)
      {
        return FALSE;
      }

      if(slot_data_ptr->sim_phase == MMGSDI_SIM_PHASE_2_PLUS)
      {
        return TRUE;
      }
#ifdef FEATURE_MMGSDI_3GPP
      service_tbl_file = MMGSDI_GSM_SST;
      pro_cmd_offset = MMGSDI_SST_PROACTIVE_SIM_OFFSET;
      pro_cmd_mask = MMGSDI_SST_PROACTIVE_MASK;
#endif /* FEATURE_MMGSDI_3GPP */
      break;

    case MMGSDI_APP_RUIM:
#ifdef FEATURE_MMGSDI_3GPP2
      service_tbl_file = MMGSDI_CDMA_SVC_TABLE;
      pro_cmd_offset = MMGSDI_CDMA_SST_TOOLKIT_OFFSET;
      pro_cmd_mask = MMGSDI_CDMA_SST_TOOLKIT_MASK;
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    default:
      UIM_MSG_ERR_1("Invalid app type 0x%x", app_type );
      return FALSE;
  }

  /* Read the service table */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                app_type,
                                                slot,
                                                service_tbl_file,
                                                &mmgsdi_data_buf);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("does not support Proactive Command");
    return FALSE;
  }

  if(mmgsdi_data_buf.data_ptr == NULL)
  {
    UIM_MSG_ERR_0("mmgsdi_card_init_is_toolkit_allowed, data is NULL");
    return FALSE;
  }

  if (mmgsdi_data_buf.data_len <= pro_cmd_offset)
  {
    UIM_MSG_ERR_0("Service Table Read does not contain byte for Proactive Cmd info");

    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
    return FALSE;
  }

  svc_table_ind = mmgsdi_data_buf.data_ptr[pro_cmd_offset] & pro_cmd_mask;

  /* Free the memory allocated for the Service Table Read */
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);

  /* Determine if Toolkit is allowed */
  return (svc_table_ind?TRUE:FALSE);
} /* mmgsdi_card_init_is_toolkit_allowed */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_CHECK_EF_PHASE

DESCRIPTION
  Read and cache the EF Phase.

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS   - Successfully updated the slot's sim_phase
    MMGSDI_NOT_FOUND - EF Phase was not on the card and so sim_phase was
                       determined from IMSI attributes or set to a default.
    MMGSDI_ERROR     - The slot's sim_phase could not be updated.

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_check_ef_phase(
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot_id,
  mmgsdi_app_enum_type      app_type
)
{
#ifdef FEATURE_MMGSDI_3GPP
  mmgsdi_return_enum_type  mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type   *slot_data_ptr  = NULL;
  mmgsdi_data_type         mmgsdi_data_buf;

  memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));

  if (app_type != MMGSDI_APP_SIM)
  {
    UIM_MSG_ERR_1("Invalid app_type 0x%x for check EF Phase", app_type);
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                app_type,
                                                slot_id,
                                                MMGSDI_GSM_PHASE,
                                                &mmgsdi_data_buf);
  if ((mmgsdi_status == MMGSDI_SUCCESS)&&
      (mmgsdi_data_buf.data_len > 0) &&
      (mmgsdi_data_buf.data_ptr != NULL))
  {
    switch (mmgsdi_data_buf.data_ptr[0])
    {
      case 0x00:
      case 0x01:
        /* From specs, for EF-Phase contents:
             0x00 indicates phase 1.
             0x01 is not expected, it is unknown if commercial cards
                  exist with these contents, treat as phase 1 */
        slot_data_ptr->sim_phase = MMGSDI_SIM_PHASE_1;
        break;
      case 0x02:
        slot_data_ptr->sim_phase = MMGSDI_SIM_PHASE_2;
        break;
      case 0x03:
      default:
        /* From specs, for EF-Phase contents:
             0x03  indicates phase 2+.
             >0x03 is not expected, it is unknown if commercial cards
                   exist with these contents, treat as phase 2+ */
        slot_data_ptr->sim_phase = MMGSDI_SIM_PHASE_2_PLUS;
        break;
    }
    UIM_MSG_HIGH_2("EF-GSM Phase on card: 0x%x, saved as SIM Phase: 0x%x",
                   mmgsdi_data_buf.data_ptr[0],
                   slot_data_ptr->sim_phase);
  }
  else
  {
    /* Read EF-Phase failed -> default to sim phase 1 */
    slot_data_ptr->sim_phase = MMGSDI_SIM_PHASE_1;

    /* Some commercial cards are missing EF-Phase, even though it is mandatory.
       For these cases, if IMSI is invalidated assume it is phase 2, otherwise
       assume it is phase 1 */
    if (mmgsdi_status == MMGSDI_NOT_FOUND)
    {
      mmgsdi_file_status_type  imsi_status;
      memset(&imsi_status, 0x00,sizeof(mmgsdi_file_status_type));

      mmgsdi_status = mmgsdi_card_init_get_file_attr(session_id,
                                                     MMGSDI_APP_SIM,
                                                     slot_id,
                                                     MMGSDI_GSM_IMSI,
                                                     &imsi_status,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     NULL);

      if ((mmgsdi_status == MMGSDI_SUCCESS)&&
          (imsi_status.file_invalidated))
      {
        slot_data_ptr->sim_phase = MMGSDI_SIM_PHASE_2;
      }
    }
    UIM_MSG_ERR_1("Couldn't read EF Phase, assuming SIM Phase: 0x%x",
                  slot_data_ptr->sim_phase);
    mmgsdi_status = MMGSDI_NOT_FOUND;
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
  return mmgsdi_status;
#else
  (void)session_id;
  (void)slot_id;
  (void)app_type;
  return MMGSDI_SUCCESS;
#endif /* FEATURE_MMGSDI_3GPP */
} /* mmgsdi_card_init_check_ef_phase */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PERFORM_POST_PIN1_TP_IF_NEEDED

DESCRIPTION
  Send Terminal Profile Download if required as part of post-pin1 if required.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_card_init_perform_post_pin1_tp_if_needed(
  mmgsdi_session_id_type     session_id,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_app_enum_type       app_type
)
{
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  /* If TP is in progress/already done, return since only
     one TP is required to be done per slot */
  if((slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_DONE)          ||
     (slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK))
  {
    UIM_MSG_HIGH_2("TP state for MMGSDI slot 0x%x is 0x%x",
                   slot_id, slot_data_ptr->mmgsdi_tp_state);
    return;
  }
  if ((app_type == MMGSDI_APP_USIM) || (app_type == MMGSDI_APP_CSIM))
  {
    UIM_MSG_HIGH_2("Slot 0x%x App type: 0x%x, Sending Terminal Profile to UICC",
                   slot_id, app_type);
    mmgsdi_card_init_gstk_terminal_profile_download(session_id, slot_id, app_type);
    return;
  }

  /* Attempt to read and cache the SIM Phase
     Ignore return value since nothing can be done anyway*/
  (void) mmgsdi_card_init_check_ef_phase(session_id,
                                         slot_id,
                                         app_type);

  mmgsdi_card_init_gstk_terminal_profile_download(session_id,
                                                  slot_id,
                                                  app_type);
}/* mmgsdi_card_init_perform_post_pin1_tp_if_needed */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_GSTK_TERMINAL_PROFILE_DOWNLOAD

DESCRIPTION
  This function is used to notfiy GSTK to perform a Terminal Profile
  download

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Triggers the GSTK task to send down a Terminal Profile Download
===========================================================================*/
void mmgsdi_card_init_gstk_terminal_profile_download (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_app_enum_type     app_type
)
{
  gstk_slot_id_enum_type      gstk_slot       = GSTK_SLOT_ID_MAX;
  mmgsdi_slot_data_type     * slot_data_ptr   = NULL;
  boolean                     ok_to_tp_dl     = FALSE;
  gstk_status_enum_type       gstk_status     = GSTK_ERROR;
  mmgsdi_int_app_info_type  * app_info_ptr    = NULL;
  mmgsdi_return_enum_type     mmgsdi_status   = MMGSDI_ERROR;
  boolean                     fetch_status    = FALSE;

  UIM_MSG_HIGH_2("In mmgsdi_card_init_gstk_terminal_profile_download, slot: 0x%x, app_type: 0x%x",
                 slot, app_type);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  gstk_slot = mmgsdi_util_convert_gstk_slot(slot);
  if(GSTK_SLOT_ID_MAX == gstk_slot)
  {
    return;
  }

  if((app_type == MMGSDI_APP_SIM) || (app_type == MMGSDI_APP_RUIM))
  {
    /* For UICC we need not be concerned about  App info */
    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id, NULL,
                               NULL, &app_info_ptr, NULL, NULL, NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
       UIM_MSG_ERR_1("Unable to get app info while profile download for 0x%x",
                   app_type);
       return;
    }
  }

  /* Current limitation/implementation is 1 TP per slot, so
     if we have a session that has already performed the TP, we
     do not want to instruct GSTK to send another TP for the
     same slot */
  if ((slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_DONE) ||
      (slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK))
  {
    UIM_MSG_HIGH_2("Cannot perform more that one TP per slot, TP state: 0x%x on Slot: 0x%x",
                   slot_data_ptr->mmgsdi_tp_state, slot);
    return;
  }

  /* --------------------------------------------------------------------------
  Determine if the card supports Terminal profile download or not.
  ----------------------------------------------------------------------------*/
  ok_to_tp_dl = mmgsdi_card_init_is_toolkit_allowed(session_id, slot, app_type);
  if(ok_to_tp_dl)
  {
    slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_IN_PROGRESS_OK;
    if(app_info_ptr != NULL)
    {
      app_info_ptr->app_tp_state = MMGSDI_TP_STATE_IN_PROGRESS_OK;
    }
  }
  else
  {
    slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK;
    if(app_info_ptr != NULL)
    {
      app_info_ptr->app_tp_state = MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK;
    }
  }

#ifdef FEATURE_UIM_MULTI_PROFILE
  /* Allow GSTK to fetch the proactive command before sending the TP. We do this
     to fetch the proactive commands (if any) after sending the TP. In case of
     multi profile card, if the profile is in emergency then we have logic to
     send error TR response to the fetched proactive commands*/
  if(mmgsdi_profile_info_ptr != NULL)
  {
    fetch_status = TRUE;
  }
#endif /* FEATURE_UIM_MULTI_PROFILE */

  gstk_gsdi_set_fetch_status(gstk_slot, fetch_status);

  /* Update TP state and Reference count */
  slot_data_ptr->tp_ref_count++;

  /* Do not send TP request to GSTK if Virtual SIM has been enabled,
     but Virtual SAT has not been enabled */
  UIM_MSG_HIGH_2("Sending TP notifcation for App 0x%x, TP state is 0x%x",
                 app_type, slot_data_ptr->mmgsdi_tp_state);
  gstk_status = gstk_slot_send_terminal_profile_dl_command(
                  gstk_slot,
                  ok_to_tp_dl,
                  slot_data_ptr->tp_ref_count,
                  mmgsdi_gstk_terminal_profile_cb);

  if(gstk_status != GSTK_SUCCESS)
  {
    gstk_cmd_from_card_type response;

    memset((void*)&response, 0x00, sizeof(gstk_cmd_from_card_type));

    /* Fake a response */
    UIM_MSG_ERR_1("Unable to send TP indication to GSTK for slot 0x%x", slot);
    response.hdr_cmd.command_id = GSTK_PROFILE_DL_IND_RSP;
    /* command_number, cmd_ref_id are ignored */
    response.hdr_cmd.cmd_detail_reference = 0;
    response.hdr_cmd.command_number = 0;
    response.hdr_cmd.sim_slot_id = gstk_slot;
    response.hdr_cmd.user_data = slot_data_ptr->tp_ref_count;
    response.cmd.terminal_profile_rsp = GSTK_PROFILE_RSP_FAIL;
    mmgsdi_gstk_terminal_profile_cb(&response);
  }
}  /* mmgsdi_card_init_gstk_terminal_profile_download */
#endif /* FEATURE_GSTK */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_GET_FILE_ATTR

DESCRIPTION
  This function build a synchronous command to UIM to obtain the file
  attribute.  Should be used for initialization purposes only.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_get_file_attr(
  mmgsdi_session_id_type           session_id,
  mmgsdi_app_enum_type             app_type,
  mmgsdi_slot_id_enum_type         slot,
  mmgsdi_file_enum_type            file,
  mmgsdi_file_status_type         *file_status_ptr,
  mmgsdi_rec_num_type             *num_of_rec_ptr,
  mmgsdi_len_type                 *rec_len_ptr,
  boolean                         *increase_allowed_ptr,
  mmgsdi_file_structure_enum_type *file_type_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type      *select_ptr    = NULL;
  mmgsdi_get_file_attr_cnf_type      *cnf_ptr       = NULL;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (select_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  select_ptr->request_header.session_id        = session_id;
  select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
  select_ptr->request_header.payload_len        =
    select_ptr->request_header.request_len -
    uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
  select_ptr->request_header.slot_id           = slot;
  select_ptr->request_header.client_data       = 0;
  select_ptr->request_header.response_cb       = NULL;
  select_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  select_ptr->access.file.file_enum            = file;
  select_ptr->activate_aid                     = TRUE;
  /* Do not read the EF-ARR for internal calls */
  select_ptr->skip_uicc_arr                    = TRUE;

  switch(app_type)
  {
    case MMGSDI_APP_SIM:
    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_uim_icc_select (select_ptr,
                                             TRUE,
                                             &cnf_ptr);
      break;
    case MMGSDI_APP_USIM:
    case MMGSDI_APP_CSIM:
      mmgsdi_status = mmgsdi_uim_uicc_select (select_ptr,
                                              TRUE,
                                              &cnf_ptr,
                                              TRUE);
      break;
    default:
      UIM_MSG_ERR_1("Invalid app type 0x%x", app_type);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);
      return MMGSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);

  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cnf Ptr Null after get file attribute");
    return MMGSDI_ERROR;
  }

  if( (mmgsdi_status != MMGSDI_SUCCESS) ||
      (cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
  {
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
    return MMGSDI_ERROR;
  }

  if (file_type_ptr)
  {
    *file_type_ptr = cnf_ptr->file_attrib.file_type;
  }

  switch (cnf_ptr->file_attrib.file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      if (file_status_ptr != NULL)
      {
        mmgsdi_memscpy(file_status_ptr,
                       sizeof(mmgsdi_file_status_type),
                       &cnf_ptr->file_attrib.file_info.linear_fixed_file.file_status,
                       sizeof(mmgsdi_file_status_type));
      }
      if (num_of_rec_ptr != NULL)
      {
        *num_of_rec_ptr =
          cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec;
      }
      if (rec_len_ptr)
      {
        *rec_len_ptr =
          cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len;
      }
      if (increase_allowed_ptr)
      {
        *increase_allowed_ptr = 0;
      }
      break;
    case MMGSDI_TRANSPARENT_FILE:
      if (file_status_ptr != NULL)
      {
        mmgsdi_memscpy(file_status_ptr,
                       sizeof(mmgsdi_file_status_type),
                       &cnf_ptr->file_attrib.file_info.transparent_file.file_status,
                       sizeof(mmgsdi_file_status_type));
      }
      if (num_of_rec_ptr != NULL)
      {
        *num_of_rec_ptr = 0;
      }
      if (rec_len_ptr)
      {
        *rec_len_ptr = 0;
      }
      if (increase_allowed_ptr)
      {
        *increase_allowed_ptr = 0;
      }
      break;
    case MMGSDI_CYCLIC_FILE:
      if (file_status_ptr != NULL)
      {
        mmgsdi_memscpy(file_status_ptr,
                       sizeof(mmgsdi_file_status_type),
                       &cnf_ptr->file_attrib.file_info.cyclic_file.file_status,
                       sizeof(mmgsdi_file_status_type));
      }
      if (num_of_rec_ptr != NULL)
      {
        *num_of_rec_ptr =
          cnf_ptr->file_attrib.file_info.cyclic_file.num_of_rec;
      }
      if (rec_len_ptr)
      {
        *rec_len_ptr =
          cnf_ptr->file_attrib.file_info.cyclic_file.rec_len;
      }
      if (increase_allowed_ptr)
      {
        *increase_allowed_ptr =
          cnf_ptr->file_attrib.file_info.cyclic_file.increase_allowed;
      }
      break;
    default:
      UIM_MSG_ERR_1("Invalid file_type 0x%x", cnf_ptr->file_attrib.file_type);
      mmgsdi_status = MMGSDI_ERROR;
  }
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  return mmgsdi_status;
} /* mmgsdi_card_init_get_file_attr */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_CACHE_BINARY

DESCRIPTION
  This function will initialize data in the cache during the initialization
  process for a binary file. The third argument data_ptr is used to retrieve
  the value of the file: this function allocates the required memory and the
  caller is responsible for freeing it. data_ptr can be NULL in case the
  value is not required.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_cache_binary(
  mmgsdi_session_id_type    session_id,
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_file_enum_type     file,
  mmgsdi_data_type         *data_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status          = MMGSDI_SUCCESS;
  mmgsdi_read_req_type               *req_ptr                = NULL;
  mmgsdi_read_cnf_type               *cnf_ptr                = NULL;
  mmgsdi_access_type                  access;
  uint32                              data_len               = 0;
  mmgsdi_cache_init_enum_type         cache_state            = MMGSDI_CACHE_MAX_ENUM;
#ifdef FEATURE_UIM_PERSISTENT_CACHE
  mmgsdi_data_type                    cache_data;
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

  /* for access to common cache, session_id and app_type are irrelevant */

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file;

  if (!mmgsdi_file_is_ef_under_mf(&access))
  {
    /* Check status of EF in respective service table */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
      mmgsdi_generic_data_ptr->client_id,
      session_id,
      app_type,
      &access);
  }

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_HIGH_1("Service Table Bit not Set for file 0x%x", file);
    return mmgsdi_status;
  }

  /* Check if already in cache */
  if (data_ptr)
  {
    mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                          &access,
                                          &data_ptr->data_len,
                                          0,
                                          &cache_state);

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      if (data_ptr->data_len > 0)
      {
        data_len = int32touint32(data_ptr->data_len);

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr->data_ptr, data_len);
        if (data_ptr->data_ptr == NULL)
        {
          /* Unable to allocate memory */
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
        mmgsdi_status = mmgsdi_cache_read(session_id,
                                          &access,
                                          data_ptr,
                                          0,
                                          &cache_state);

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          data_ptr->data_len = 0;
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data_ptr->data_ptr);

          /* Cache status indicates that the file is not present on the card */
          if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
            (cache_state == MMGSDI_CACHE_NOT_FOUND))
          {
            return mmgsdi_status;
          }
        }
        else
        {
          return MMGSDI_SUCCESS;
        } /* read cache successfully */
      } /* read cache len > 0 */
    } /* read cache len successfully */
    /* Cache status indicates that the file is not present on the card */
    else if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
            (cache_state == MMGSDI_CACHE_NOT_FOUND))
    {
      return mmgsdi_status;
    }
  } /* passed in data_ptr != NULL */

#ifdef FEATURE_UIM_PERSISTENT_CACHE
  /*  if read cache len returned NOT FOUND, then read from persistent cache and
      write the EF data to the cache and skip reading from card */
  memset(&cache_data, 0, sizeof(mmgsdi_data_type));
  mmgsdi_status = mmgsdi_persistent_cache_read(slot,
                                               file,
                                               0,
                                               &(cache_data.data_len),
                                               &(cache_data.data_ptr));
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    (void)mmgsdi_cache_write(session_id,
                             &access,
                             cache_data,
                             0,
                             MMGSDI_DATA_FROM_CARD_COMPLETE);

    if(data_ptr != NULL)
    {
      data_ptr->data_len = cache_data.data_len;
      data_ptr->data_ptr = cache_data.data_ptr;
    }
    else
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_data.data_ptr);
    }
    return MMGSDI_SUCCESS;
  }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

  /* Commands can be sent to UIM in sync mode only in GSDI context */
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_cache_binary() invoked in non GSDI context");
    return MMGSDI_NOT_INIT;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
    sizeof(mmgsdi_read_req_type));
  if (req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  req_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  req_ptr->request_header.session_id        = session_id;
  req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  req_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_read_req_type));
  req_ptr->request_header.payload_len        =
    req_ptr->request_header.request_len -
    uint32toint32(sizeof(mmgsdi_read_req_type));
  req_ptr->request_header.slot_id           = slot;
  req_ptr->request_header.client_data       = 0;
  req_ptr->request_header.response_cb       = NULL;
  req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum            = file;
  req_ptr->rec_num                          = 0;
  req_ptr->offset                           = 0;
  req_ptr->file_type                        = MMGSDI_TRANSPARENT_FILE;

  /* For ICCID, force length to be 10 bytes to avoid issues with
     non spec-compliant cards*/
  if(file == MMGSDI_ICCID)
  {
    req_ptr->data_len = MMGSDI_ICCID_LEN;
  }

  switch(app_type)
  {
    case MMGSDI_APP_SIM:
    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_uim_icc_read_transparent (req_ptr,
                                                       TRUE,
                                                       &cnf_ptr);
      break;
    case MMGSDI_APP_USIM:
    case MMGSDI_APP_CSIM:
      mmgsdi_status = mmgsdi_uim_uicc_read_transparent (req_ptr,
                                                        TRUE,
                                                        &cnf_ptr);
      break;
    default:
      UIM_MSG_ERR_1("Invalid app type 0x%x", app_type);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
      return MMGSDI_ERROR;
  }

  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cnf Ptr Null after read");
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
    return mmgsdi_status;
  }
  if( (mmgsdi_status == MMGSDI_SUCCESS) &&
      (cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
  {
    if (data_ptr != NULL)
    {
      data_ptr->data_len = cnf_ptr->read_data.data_len;

      if (data_ptr->data_len > 0)
      {
        data_len = int32touint32(data_ptr->data_len);

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr->data_ptr, data_len);
        if (data_ptr->data_ptr != NULL)
        {
          mmgsdi_memscpy(data_ptr->data_ptr,
                         data_len,
                         cnf_ptr->read_data.data_ptr,
                         int32touint32(cnf_ptr->read_data.data_len));
        }
        else
        {
          data_ptr->data_len = 0;
          mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
    }
  }

  mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
  return mmgsdi_status;
} /* mmgsdi_card_init_cache_binary */


/*=============================================================================
FUNCTION:      MMGSDI_CARD_INIT_CACHE_RECORD

DESCRIPTION
  This function will initialize data in the cache during the initialization
  process for a record based file. Only first record is read and it will be
  stored in the transparent cache.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_cache_record(
  mmgsdi_session_id_type    session_id,
  mmgsdi_app_enum_type      app_type,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_file_enum_type     file,
  mmgsdi_data_type         *data_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status          = MMGSDI_SUCCESS;
  mmgsdi_read_req_type              *req_ptr               = NULL;
  mmgsdi_read_cnf_type              *cnf_ptr               = NULL;
  mmgsdi_access_type                access;
  uint32                            data_len               = 0;
#ifdef FEATURE_UIM_PERSISTENT_CACHE
  mmgsdi_data_type                  cache_data;
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file;
  if (!mmgsdi_file_is_ef_under_mf(&access))
  {
    /* Check status of EF in respective service table */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
      mmgsdi_generic_data_ptr->client_id,
      session_id,
      app_type,
      &access);
  }

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_HIGH_1("Service Table Bit not Set for file 0x%x", file);
    return mmgsdi_status;
  }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
  /* Read from presistent cache */
  memset(&cache_data, 0, sizeof(mmgsdi_data_type));
  mmgsdi_status = mmgsdi_persistent_cache_read(slot,
                                               file,
                                               0, /* even tho it's reading record, it calls to cache_write which saved as rec_num 0*/
                                               &(cache_data.data_len),
                                               &(cache_data.data_ptr));
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {

    (void)mmgsdi_cache_write(session_id,
                             &access,
                             cache_data,
                             0,
                             MMGSDI_DATA_FROM_CARD_COMPLETE);

    if(data_ptr != NULL)
    {
      data_ptr->data_len = cache_data.data_len;
      data_ptr->data_ptr = cache_data.data_ptr;
    }
    else
    {
      MMGSDIUTIL_TMC_MEM_FREE(cache_data.data_ptr);
    }
    return MMGSDI_SUCCESS;
  }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

  /* Commands can be sent to UIM in sync mode only in GSDI context */
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    UIM_MSG_HIGH_0("mmgsdi_card_init_cache_record() invoked in non GSDI context");
    return MMGSDI_NOT_INIT;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
    sizeof(mmgsdi_read_req_type));
  if (req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  req_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  req_ptr->request_header.session_id        = session_id;
  req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  req_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_read_req_type));
  req_ptr->request_header.payload_len       =
    req_ptr->request_header.request_len -
    uint32toint32(sizeof(mmgsdi_read_req_type));
  req_ptr->request_header.slot_id           = slot;
  req_ptr->request_header.client_data       = 0;
  req_ptr->request_header.response_cb       = NULL;
  req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum            = file;
  req_ptr->rec_num                          = 1; /*always read the 1st record*/
  req_ptr->offset                           = 0;
  req_ptr->file_type                        = MMGSDI_LINEAR_FIXED_FILE;

  switch(app_type)
  {
    case MMGSDI_APP_SIM:
    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_uim_icc_read_record (req_ptr,
                                                  TRUE,
                                                  &cnf_ptr);
      break;
    case MMGSDI_APP_USIM:
    case MMGSDI_APP_CSIM:
      mmgsdi_status = mmgsdi_uim_uicc_read_record (req_ptr,
                                                   TRUE,
                                                   &cnf_ptr);
      break;

    default:
      UIM_MSG_ERR_1("Invalid app type 0x%x", app_type);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
      return MMGSDI_ERROR;
  }

  if((mmgsdi_status == MMGSDI_SUCCESS) &&
     (cnf_ptr != NULL) &&
     (cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
  {
    if (data_ptr != NULL)
    {
      data_ptr->data_len = cnf_ptr->read_data.data_len;
      data_len           = int32touint32(data_ptr->data_len);

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr->data_ptr, data_len);
      if (data_ptr->data_ptr != NULL)
      {
        mmgsdi_memscpy(data_ptr->data_ptr,
                       data_len,
                       cnf_ptr->read_data.data_ptr,
                       int32touint32(cnf_ptr->read_data.data_len));
      }
      else
      {
        data_ptr->data_len = 0;
        mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
        /* do not need to free req_ptr because it has been handled by
           the synchronous portion of the code */
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
  }

  if(cnf_ptr != NULL)
  {
    mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
  /* do not need to free req_ptr because it has been handled by
     the synchronous portion of the code */
  return mmgsdi_status;
} /* mmgsdi_card_init_cache_record */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_LINK_EST

DESCRIPTION:
  This function will handle the Link Established notification

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_link_est(
  const mmgsdi_notify_link_est_req_type *req_ptr
)
{
  mmgsdi_slot_data_type   *slot_data_ptr      = NULL;
  uint32                   max_work_wait_time = 0;
  mmgsdi_return_enum_type  mmgsdi_status      = MMGSDI_SUCCESS;
  uint8                    slot_index         = MMGSDI_SLOT_1_INDEX;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* --------------------------------------------------------------------------
     Validate the slot.
     ----------------------------------------------------------------------- */

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  /* Allocating memory  for Slot data*/
  if(mmgsdi_data_slot_ptr[slot_index] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_data_slot_ptr[slot_index],
                                       sizeof(mmgsdi_slot_data_type));

    if(mmgsdi_data_slot_ptr[slot_index] == NULL)
    {
      return MMGSDI_ERROR;
    }
  }

  /* --------------------------------------------------------------------------
     Populate card info to MMGSDI slot data
     ----------------------------------------------------------------------- */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_3("Slot 0x%x: Card Present: 0x%x, Slot State: 0x%x",
                 req_ptr->request_header.slot_id,
                 req_ptr->link_est_data.card_present,
                 slot_data_ptr->mmgsdi_state);

  /* --------------------------------------------------------------------------
     Link Established implies end of any sim busy state.
     ----------------------------------------------------------------------- */
  slot_data_ptr->sim_busy = FALSE;

  /* Check if OnChip needs to be auto-activated:
     The idea here is that whenever the auto-activate-onchip NV item is set,
     MMGSDI must activate OnChip automatically during boot up. Instead of
     doing that during MMGSDI post task creation initialization because of
     various possible race-conditions, we decided to do that whenever a link
     established message or a card error is received from drivers. And if the
     NV item dictates that the OnChip should be auto-activated, and we are
     able to successfully queue the OnChip activation request, we skip further
     processing of link established and card error messages */
  if(mmgsdi_generic_data_ptr != NULL &&
     mmgsdi_generic_data_ptr->onchip_act_data[slot_index].auto_activation_enabled != FALSE)
  {
    /* EFS item mandates that Onchip be auto activated. So, MMGSDI will try to
       auto activate Onchip and will skip processing of link est message if it
       succeeds */
    if(MMGSDI_SUCCESS == mmgsdi_gen_onchip_auto_activate(
                           req_ptr->request_header.slot_id))
    {
      UIM_MSG_HIGH_0("Onchip active or is being activated; ignoring link_est");
      return MMGSDI_SUCCESS;
    }
  }

  /* --------------------------------------------------------------------------
     Work wait time and Sanity timer calculations
     ----------------------------------------------------------------------- */
  /* Deduce the max. work wait time for each slot */
  max_work_wait_time = req_ptr->link_est_data.work_waiting_time;

  /* Update the Sanity timer value (max. time to wait for
     a response from UIM) */
  slot_data_ptr->mmgsdi_uim_sanity_timer_value =
    max_work_wait_time *
    MMGSDI_MAX_BYTES_PER_CMD_FROM_UIM *
    MMGSDI_MAX_UIM_CMDS_PENDING;

  /* PSM Init Logic
     - Card from the current data slot ptr should be UICC
     - card from link est should be uicc and check for ICCID
   */
  if (mmgsdi_generic_data_ptr != NULL &&
      mmgsdi_generic_data_ptr->num_slots == 1 &&
      req_ptr->request_header.slot_id == MMGSDI_SLOT_1 &&
      mmgsdi_psm_get_status_early_init())
  {
    if (mmgsdi_psm_iccid_and_protocol_match_during_link_est(
          req_ptr->link_est_data.iccid,
          req_ptr->link_est_data.protocol))
    {
      /* Is euicc or not */
      slot_data_ptr->is_euicc = req_ptr->link_est_data.is_euicc;

      /* Copy ATR */
      mmgsdi_memscpy(&slot_data_ptr->card_atr,
                     sizeof(slot_data_ptr->card_atr),
                     &req_ptr->link_est_data.atr,
                     sizeof(mmgsdi_atr_info_type));

      /* Copy History characters */
      mmgsdi_memscpy(&slot_data_ptr->hist_char,
                     sizeof(slot_data_ptr->hist_char),
                     &req_ptr->link_est_data.hist_char,
                     sizeof(mmgsdi_hist_char_info_type));

      /* Decode the ATR */
      (void)mmgsdi_util_parse_atr(MMGSDI_SLOT_1);

      /* Trigger TP DL for UICC Card */
      mmgsdi_status = mmgsdi_card_init_trigger_tp_dl_for_uicc(MMGSDI_SLOT_1, NULL, NULL);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        return MMGSDI_SUCCESS;
      }
    }
  }

  /* If card is present on a Slot  and Card initialization has not yet
      been done, perform card initalization */
  if((req_ptr->link_est_data.card_present) &&
     (slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED) &&
     (slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_NOT_DONE))
  {
    /* Free client request data for any commands that were still pending at
       last card error to avoid leaking client_req_table entries. */
    mmgsdi_free_client_req_data_for_pending_commands(req_ptr->request_header.slot_id);

    mmgsdi_status = mmgsdi_card_init_populate_card_app_slot_info(
                      &req_ptr->link_est_data,
                      req_ptr->request_header.slot_id);

    /* Reset the card error condition to UNKNOWN as we got link established
       notification */
    mmgsdi_data_slot_ptr[slot_index]->card_error_condition =
      MMGSDI_CARD_ERR_UNKNOWN_ERROR;

    if (mmgsdi_status == MMGSDI_UIM_INSTN_CHG_SUCCESS)
    {
      /* Let us not bother processing further since we will get
         a new link established for the reset process just started */
      return mmgsdi_status;
    }
    /* If card is eUICC then even if the EF-DIR read fails, proceed with
       card_inserted as spec doesn't mandate a defualt profile to be present
       in card */
    else if (mmgsdi_status != MMGSDI_SUCCESS && slot_data_ptr->is_euicc == FALSE)
    {
      mmgsdi_evt_build_and_send_card_error(TRUE,
                                           0,
                                           req_ptr->request_header.slot_id,
                                           MMGSDI_CARD_ERR_UNKNOWN_ERROR);
      mmgsdi_silent_pin_purge_info(req_ptr->request_header.slot_id);
      mmgsdi_util_close_all_sessions_cleanup_and_notify(req_ptr->request_header.slot_id);
      mmgsdi_util_reset_app_data(FALSE,req_ptr->request_header.slot_id);
    }
  }

  /* --------------------------------------------------------------------------
     Send out CARD_INSERTED event
     ----------------------------------------------------------------------- */
  /* Only send CARD_INSTERED event for ICC card and for UICC card only if
     a TP request has not been sent to GSTK
     or if no nv refresh from MCFG is pending */
  if ((req_ptr->link_est_data.card_present) &&
      ((slot_data_ptr->mmgsdi_tp_state != MMGSDI_TP_STATE_IN_PROGRESS_OK)      &&
       (slot_data_ptr->mmgsdi_tp_state != MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)) &&
      (slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED) &&
      (slot_data_ptr->nv_refresh_in_progress == FALSE))
  {
    UIM_MSG_HIGH_1("MMGSDI_CARD_INSERTED for SLOT 0x%x",
                   req_ptr->request_header.slot_id);

    mmgsdi_evt_build_and_send_card_inserted(
      TRUE, (mmgsdi_client_id_type)0, req_ptr->request_header.slot_id);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_process_link_est */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_CARD_ERROR

DESCRIPTION:
  This function will handle the Card Error indication

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_card_error(
  const mmgsdi_notify_card_error_req_type *req_ptr
)
{
  mmgsdi_return_enum_type                mmgsdi_status  = MMGSDI_SUCCESS;
  uint8                                  slot_index     = MMGSDI_SLOT_1_INDEX;
  mmgsdi_session_closed_cause_enum_type  cause          = MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;
  mmgsdi_card_err_info_enum_type         prev_err_cause = MMGSDI_CARD_ERR_UNKNOWN_ERROR;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Allocating memory for Slot data*/
  if(mmgsdi_data_slot_ptr[slot_index] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_data_slot_ptr[slot_index],
                                       sizeof(mmgsdi_slot_data_type));

    if(mmgsdi_data_slot_ptr[slot_index] == NULL)
    {
      return MMGSDI_ERROR;
    }
  }

  /* Reset PSM early init flag; it is possible that card error received after
     early initialization from PSM data stored in SFS */
  if(req_ptr->request_header.slot_id == MMGSDI_SLOT_1)
  {
    mmgsdi_psm_reset_early_init();
  }

  /* Check if OnChip needs to be auto-activated:
     The idea here is that whenever the auto-activate-onchip NV item is set,
     MMGSDI must activate OnChip automatically during boot up. Instead of
     doing that during MMGSDI post task creation initialization because of
     various possible race-conditions, we decided to do that whenever a link
     established message or a card error is received from drivers. And if the
     NV item dictates that the OnChip should be auto-activated, and we are
     able to successfully initiate OnChip activation, we skip further
     processing of link established and card error messages */
  if(mmgsdi_generic_data_ptr != NULL &&
     mmgsdi_generic_data_ptr->onchip_act_data[slot_index].auto_activation_enabled != FALSE)
  {
    /* EFS item mandates that Onchip be auto activated. So, MMGSDI will auto
       activate Onchip and will skip processing of link est message */
    if(MMGSDI_SUCCESS == mmgsdi_gen_onchip_auto_activate(
                           req_ptr->request_header.slot_id))
    {
      UIM_MSG_HIGH_0("Onchip active or is being activated; ignoring card_error");
      return MMGSDI_SUCCESS;
    }
  }

  if(req_ptr->temporary_error)
  {
    /* If this is the initial recovery failure, save the required PIN information to
       perform a silent recovery upon subsequent app activation so UI is not notified,
       establishing better user experience. */
    (void)mmgsdi_silent_pin_cache_data(req_ptr->request_header.slot_id);
  }
  else
  {
    mmgsdi_silent_pin_purge_info(req_ptr->request_header.slot_id);
  }

  /* Store the previous and current card error cause */
  prev_err_cause = mmgsdi_data_slot_ptr[slot_index]->card_error_condition;
  mmgsdi_data_slot_ptr[slot_index]->card_error_condition = req_ptr->error_condition;

  if(req_ptr->error_condition == MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY)
  {
    cause = MMGSDI_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN;
  }
  else if (req_ptr->error_condition == MMGSDI_CARD_ERR_CARD_REMOVED)
  {
    cause = MMGSDI_SESSION_CLOSED_CAUSE_CARD_REMOVED;
  }
  else
  {
    cause = MMGSDI_SESSION_CLOSED_CAUSE_CARD_ERROR;
  }

  /* Card error implies end of any sim busy state */
  mmgsdi_data_slot_ptr[slot_index]->sim_busy = FALSE;

  /* Send out the Card Error event and perform Session and app clean-up */
  mmgsdi_evt_build_and_send_card_error(TRUE,
                                       0,
                                       req_ptr->request_header.slot_id,
                                       prev_err_cause);

  /* Update the session closed cause. The session changed event
     broadcasts the cause */
  mmgsdi_util_update_session_closed_cause_for_slot(req_ptr->request_header.slot_id, cause);

  mmgsdi_util_close_all_sessions_cleanup_and_notify(req_ptr->request_header.slot_id);
  mmgsdi_util_reset_app_data(FALSE,req_ptr->request_header.slot_id);

  /* Reset Sanity Timer to default value */
  mmgsdi_data_slot_ptr[slot_index]->mmgsdi_uim_sanity_timer_value
    = MMGSDI_UIM_SANITY_TIMER_VALUE;

 mmgsdi_data_slot_ptr[slot_index]->is_euicc = FALSE;

  /* To support change in single/multi profile cards, reset derived information.
     The following scenarios are possible:
       1) Single profile card is removed and multi profile card is inserted
       2) Multi profile card is removed and single profile card is inserted */
#ifdef FEATURE_UIM_MULTI_PROFILE
  if(req_ptr->request_header.slot_id == MMGSDI_SLOT_1)
  {
    mmgsdi_multi_profile_reset_data();
  }
#endif /* FEATURE_UIM_MULTI_PROFILE */

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_process_card_error */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_SIM_BUSY

DESCRIPTION:
  This function will handle the SIM busy message originating from the UIM
  driver.

  If the sim busy status of a slot has changed then the function saves the new
  status internally and sends a sim busy event to all MMGSDI clients

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_sim_busy(
  const mmgsdi_notify_sim_busy_req_type *req_ptr
)
{
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_SUCCESS;
  uint8                                  slot_index    = MMGSDI_SLOT_1_INDEX;

  /* Validate the input parameters. */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid slot id 0x%x", req_ptr->request_header.slot_id);
    return MMGSDI_ERROR;
  }

  /* Verify slot data already allocated before using pointer */
  if(mmgsdi_data_slot_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_1("Sim busy received for slot with NULL slot data: 0x%x",
                  req_ptr->request_header.slot_id);
    return MMGSDI_ERROR;
  }

  /* Take no action if drivers is sending the same sim busy message twice */
  if (mmgsdi_data_slot_ptr[slot_index]->sim_busy == req_ptr->sim_busy)
  {
    return MMGSDI_SUCCESS;
  }

  /* Store sim busy status internally, this will be used to inform any new
     MMGSDI clients registering late */
  mmgsdi_data_slot_ptr[slot_index]->sim_busy = req_ptr->sim_busy;

  /* Send current sim busy status to all clients */
  mmgsdi_evt_build_and_send_sim_busy(TRUE, 0, req_ptr->request_header.slot_id);

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_process_sim_busy */


/*===========================================================================
FUNCTION MMGSDI_CARD_INIT_BUILD_INTERNAL_PUP_REQ

DESCRIPTION
  Function to build the MMGSDI Internal power up Request and put into the
  MMGSDI  command queue so that MMGSDI can be kicked into the various
  card and provisioning application initialization procedures

DEPENDENCIES
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_build_internal_pup_req(
  mmgsdi_session_id_type      session_id,
  mmgsdi_slot_id_enum_type    slot_id,
  mmgsdi_events_enum_type     evt,
  mmgsdi_card_state_enum_type card_state,
  mmgsdi_app_state_enum_type  app_state
)
{
  mmgsdi_task_cmd_type              * task_cmd_ptr  = NULL;
  mmgsdi_internal_pup_init_req_type * msg_ptr       = NULL;
  int32                               total_mem_len = 0;
  uint32                              task_cmd_len  = 0;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;

  UIM_MSG_HIGH_3("In mmgsdi_card_init_build_internal_pup_req, session_id: 0x%x, card_state: 0x%x, app_state: 0x%x",
                 session_id, card_state, app_state);

  /* --------------------------------------------------------------------------
     Check if Slot ID is valid for internal power-up request w.r.t. CARD_INSERTED
     ------------------------------------------------------------------------*/
  if ((evt == MMGSDI_CARD_INSERTED_EVT) &&
      (slot_id != MMGSDI_SLOT_1) &&
      (slot_id != MMGSDI_SLOT_2) &&
      (slot_id != MMGSDI_SLOT_3))
  {
    UIM_MSG_ERR_1("Invalid Slot: 0x%x for Card Inserted command procesing",
                  slot_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len  = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* --------------------------------------------------------------------------
    No need to Align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.internal_pup_init_req;

  /*---------------------------------------------------------------------------
    Populating internal_pup_init_req header
      1) Client ID
      2) Session ID
      3) Request type
      4) Slot ID
      5) Client Data
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_INTERNAL_PUP_INIT_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_INTERNAL_PUP_INIT_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_INTERNAL_PUP_INIT_REQ;
  msg_ptr->request_header.slot_id           = slot_id;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = mmgsdi_util_internal_cb;

  /*---------------------------------------------------------------------------
    Populating internal_pup_init_req content
  ---------------------------------------------------------------------------*/
  msg_ptr->curr_card_state = card_state;
  msg_ptr->curr_app_state  = app_state;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
    ** command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

    UIM_MSG_ERR_1("Command Queuing MMGSDI_INTERNAL_PUP_INIT_REQ Fail 0x%x",
                  mmgsdi_status);
  }
  else
  {
    UIM_MSG_HIGH_0("Successful queue MMGSDI_INTERNAL_PUP_INIT_REQ command ");
  }

  return mmgsdi_status;
} /* mmgsdi_card_init_build_internal_pup_req */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_INIT_PROCESS_INTERNAL_PUP

DESCRIPTION:
  This function will handle the Internal Power-up requests w.r.t. card and
  provsioning application initialization

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
SIDE EFFECTS:
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_init_process_internal_pup(
  const mmgsdi_internal_pup_init_req_type *req_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type    *app_info_ptr  = NULL;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  UIM_MSG_HIGH_2("Internal power-up, curr_card_state: 0x%x, curr_app_state: 0x%x",
                 req_ptr->curr_card_state, req_ptr->curr_app_state);

  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                      req_ptr->request_header.session_id,
                      NULL,
                      NULL,
                      &app_info_ptr);
  if((mmgsdi_status == MMGSDI_SUCCESS) && (app_info_ptr != NULL))
  {
    if(app_info_ptr->deactivation_in_progress != FALSE)
    {
      /* The app associated with this session is currently being deactivated
         hence do not proceed with internal pup... do not send any further pup
         event to clients, they will receive session deactivation event once
         the deactivation that is currently in progress, completes. */
      return MMGSDI_SUCCESS;
    }
  }

  switch (req_ptr->curr_card_state)
  {
    case MMGSDI_STATE_CARD_INSERTED:
      switch (req_ptr->curr_app_state)
      {
        case MMGSDI_APP_STATE_DETECTED:
          mmgsdi_status = mmgsdi_card_init_process_card_inserted(
                            req_ptr->request_header.slot_id);
          break;

        case MMGSDI_APP_STATE_ACTIVATED:
          mmgsdi_status = mmgsdi_card_init_process_prov_session_activated(
                            req_ptr->request_header.session_id,
                            req_ptr->curr_app_state);
          break;

        case MMGSDI_APP_STATE_READY_FOR_PERSO:
          mmgsdi_status = mmgsdi_card_init_process_post_pin1(
                            req_ptr->request_header.session_id,
                            req_ptr->curr_app_state);
          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
            return MMGSDI_SUCCESS;
          }
          break;

        case MMGSDI_APP_STATE_PERSO_VERIFIED:
          mmgsdi_status = mmgsdi_card_init_process_post_perso(
                            req_ptr->request_header.session_id,
                            req_ptr->curr_app_state);
          break;

        case MMGSDI_APP_STATE_PIN_EVT_SENT:
        case MMGSDI_APP_STATE_READY:
        case MMGSDI_APP_STATE_EXT_PERSO_FAILED:
        case MMGSDI_APP_STATE_DEACTIVATED:
        case MMGSDI_APP_STATE_ILLEGAL:
        case MMGSDI_APP_STATE_WAIT_FOR_SUB_OK:
        case MMGSDI_APP_STATE_UNKNOWN:
        case MMGSDI_APP_STATE_ENUM_MAX:
          return MMGSDI_SUCCESS;
      }
      break;
    case MMGSDI_STATE_NOT_INIT:
    case MMGSDI_STATE_NO_CARD:
    case MMGSDI_STATE_ENUM_MAX:
      return MMGSDI_SUCCESS;
  }

  mmgsdi_card_init_process_internal_pup_init_status(
    req_ptr->request_header.session_id,
    mmgsdi_status);

  return MMGSDI_SUCCESS;
} /* mmgsdi_card_init_process_internal_pup */


/* ============================================================================
FUNCTION MMGSDI_CARD_INIT_MAP_FILE_TO_SESSION_CLOSED_CAUSE

DESCRIPTION
  This is an utility function which maps the file provided to the session closed
  cause.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_session_closed_cause_enum_type

SIDE EFFECTS
  None
============================================================================*/
static mmgsdi_session_closed_cause_enum_type mmgsdi_card_init_map_file_to_session_closed_cause(
  mmgsdi_file_enum_type file,
  boolean               file_content_invalid
)
{
  switch(file)
  {
    case MMGSDI_USIM_AD:
    case MMGSDI_CSIM_AD:
    case MMGSDI_GSM_AD:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_AD;

    case MMGSDI_USIM_IMSI:
    case MMGSDI_GSM_IMSI:
      if (file_content_invalid)
      {
        return MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_IMSI;
      }
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI;

    case MMGSDI_USIM_ACC:
    case MMGSDI_GSM_ACC:
      if (file_content_invalid)
      {
        return MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_ACC;
      }
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACC;

    case MMGSDI_USIM_HPLMN:
    case MMGSDI_GSM_HPLMN:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HPPLMN;

    case MMGSDI_USIM_LOCI:
    case MMGSDI_GSM_LOCI:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_LOCI;

    case MMGSDI_USIM_PSLOCI:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PSLOCI;

    case MMGSDI_USIM_KEYS:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KEYS;

    case MMGSDI_USIM_FPLMN:
    case MMGSDI_GSM_FPLMN:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_FPLMN;

    case MMGSDI_USIM_START_HFN:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_START_HFN;

    case MMGSDI_USIM_THRESHOLD:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_THRESHOLD;

    case MMGSDI_GSM_PHASE:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PHASE;

    case MMGSDI_GSM_KC:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KC;

    case MMGSDI_CSIM_CST:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_CST;

    case MMGSDI_CSIM_RUIM_ID:
    case MMGSDI_CDMA_RUIM_ID:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_RUIM_ID;

    case MMGSDI_CSIM_SERVICE_PREF:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SERVICE_PREF;

    case MMGSDI_CSIM_IMSI_M:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_M;

    case MMGSDI_CSIM_IMSI_T:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_T;

    case MMGSDI_CSIM_ACCOLC:
    case MMGSDI_CDMA_ACCOLC:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACCOLC;

    case MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND:
    case MMGSDI_CDMA_DIST_BASED_REGN_IND:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_DIST_BASED_REGN_IND;

    case MMGSDI_CSIM_CDMA_SYS_REGN_IND:
    case MMGSDI_CDMA_SYS_REGN_IND:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SYS_REGN_IND;

    case MMGSDI_CSIM_OTAPA_SPC_ENABLE:
    case MMGSDI_CDMA_OTAPA_SPC_ENABLE:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_OTAPA_SPC_ENABLE;

    case MMGSDI_CSIM_NAM_LOCK:
    case MMGSDI_CDMA_NAM_LOCK:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_NAM_LOCK;

    case MMGSDI_CSIM_CDMA_HOME_SID_NID:
    case MMGSDI_CDMA_HOME_SID_NID:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HOME_SID_NID;

    case MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND:
    case MMGSDI_CDMA_ZONE_BASED_REGN_IND:
      return MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ZONE_BASED_REGN_IND;

    case MMGSDI_CSIM_PRL:
    case MMGSDI_CDMA_PRL:
      if (file_content_invalid)
      {
        return MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_PRL;
      }
      return MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;

    default:
      return MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN;
  }
} /* mmgsdi_card_init_map_file_to_session_closed_cause */
