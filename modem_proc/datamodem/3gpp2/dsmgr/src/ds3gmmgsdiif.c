/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

             M M G S D I   I N T E R F A C E   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



  Copyright (c) 2008 - 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gmmgsdiif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/14   hr      Sim swap feature
06/11/14   ms      Terminate the PPP gracefully on receiving card removed event from
                   CM and ignore MMGSDI Card removed event to allow IMS de-reg
                   to happen OTA.
05/12/14   sd      Fix to avoid incorrect slot based MMGSDI event handling
                   in case of MULTI APP card.
04/21/14   sd      Fix to avoid incorrect MMGSDI event handling for session
                   changed event in case of MULTI APP card.
03/13/14   rs      Added changes to call ds_mppm_process_mmgsdi_subscription_event
09/15/13   rs      Common build changes for dual sim and triple sim
08/23/13   rs      Fixed KW errors
05/24/13   sd      TSTS changes for DS3G.
10/29/12   sd      Fixed compiler warnings.
09/24/12   ms      Avoid array out-of-bounds for profile apn list.
08/14/12   jee     Invalidate IMSI only for USIM refresh
07/22/12   jee     Ignore MMGSDI REFRESH during invalid refresh stages
05/18/12   jee     To process FCN when REFRESH event is received
05/15/12   nc      Handling REFRESH gracefully for un-registered files.
05/04/12   jee     Mainlined FEATURE_UIM_SUPPORT_HOTSWAP
04/10/12   jee     To handle MMGSDI REFRESH events
04/19/12   sb      Feature Cleanup: Always On Flag FEATURE_RUIM_CDMA_REFRESH
03/23/12   msh     TCB changes due to Core changes (fix for MOB)
03/21/12   msh     TCB changes due to Core changes
03/16/12   msh     Should not reset acl_read_from_usim on card error/reset
01/02/12   msh     Coan: Feature cleanup
12/07/11   op      Prorperly handle mis-configured APN control list in UICC
11/17/11   vpk     Update the slot info during refresh complete
11/01/11   msh     Corrected masks for SIP, MIP and MIP with SIP falllback
10/24/11   ss      Fixed RPM mmgsdi refresh registration issue.
09/30/11   var     To resolve DS task not voting OK to INIT in refresh
09/27/11   vpk     Compiler warnings fix
09/27/11   nd      Fixed compiler warnings.
09/15/11   vpk     Tear down all the ifaces when an UIM refresh happens
09/09/11   vpk     Fix for empty username during AN authentication
09/05/11   ss      Changes to support RPM.
08/29/11   ms      Allocate memory for variable ds_mmgsdi_event_info dynamically
07/12/11   dvk     Fixed Klocwork errors
05/06/11   op      Check for NULL APN when parsing ACL
04/22/11   ash     Clear PS signal before issuing MMGSDI command.
04/11/11   ttv     Added support for Hot Swap feature.
03/23/11   sn      Globals cleanup for free floating of tasks.
03/15/11   mg      Global variable cleanup
03/03/11   bs      Added check for refresh files.
01/31/11   op      Cleaning up usage of UIM API
01/13/11   op      Compiler warning fix
01/04/11   op      Added check for ACL available/enabled before reading ACL.
                   ACL parsing corrected, as per spec.
11/22/10   ttv     Added support for DSDS.
10/20/10   ls      In C2K mode, DS votes YES to USIM refresh and in GW/LTE mode
                   DS votes YES to CSIM refresh
09/09/10   op      Migrated to MSG 2.0 macros
06/28/10   ss      Fixed compiler warnings.
21/06/10   ms      Fixed KW errors.
06/17/10   as      API to access ds3gi_mode for long term and Fusion purposes
06/04/10   bs      MMGSDI SESSION Migration.
05/25/10   ls      Add refresh voting support for C2K
21/04/10   ms      Fixed KW errors.
03/08/10   ls      Removed all stub functions and fixed LINT errors
02/16/10   ls      Fixed IMSI length issue
02/05/10   ls      Vote for refresh for 1x
01/16/10   ls      All mmgsdi API shall be called under DS context
01/11/10   ms      Replaced UIM_3GPD Constants with NVRUIM_3GPD.
11/25/09   ls      Add support for retrieving slot info
11/09/09   ls      Add support for retrieving IMSI info from USIM card
10/21/09   ls      Support 1x pri/sec and GW pri/sec
09/22/09   sn      Fixed klocwork errors.
09/22/09   vrk     Merged LTE changes.
08/07/09   ls      Support union structure for CSIM feature
07/17/09   ls      Remove CDMA service table reading and updating since UIM
                   team is taking care of this. Change FEATURE_CSIM to
                   FEATURE_MMGSDI_SESSION_LIB
06/04/09   ls      Support CSIM feature
04/29/09   ms      Fixed Lint High Errors.
11/26/08   sn      Fixed off-target lint errors.
10/23/08   yz      Fixed featurization issues
10/10/08   yz      Initial version of the file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#include "amssassert.h"
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

#include "ds3gmgr.h"
#include "ds3gmgrint.h"
#include "ds3gutil.h"
#include "ds3gmmgsdiif.h"
#include "modem_mem.h"
#include "data_msg.h"
#include "rex.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_3gpp_rpm.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_low_prio_sig_hdlr.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_3gpp_roaming_hdlr.h"
#ifdef FEATURE_MMGSDI
#include "err.h"
#include "uim.h"
#include "nvruimi.h"
#include "ps_svc.h"
#include "mmgsdilib_common.h"

#if defined(FEATURE_DATA_IS707)
#include "ds707_pkt_mgr.h"
#include "psi.h"
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_EHRPD
#include "ds707_data_session_profile.h"
#endif /* FEATURE_EHRPD */
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#include "dsucsdhdlr.h"
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

#include "nv.h"
#include "nv_items.h"

#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */

#include "rcinit_rex.h"

#include "data_err_suppr.h"

/* MAX number of refresh files ( UMTS EF's +CDMA EF's) */
#define DS3G_MMGSDI_MAX_NUM_REFRESH_FILES  4

#define DS3G_MMGSDI_ISIM_AID_CMP_LENGTH 7

/* All ISIM apps start with this identifier: 3GPP spec 31-103 */
const uint8 ds3g_mmgsdi_isim_aid_value[DS3G_MMGSDI_ISIM_AID_CMP_LENGTH]
                                      = {0xA0,0x00,0x00,0x00,0x87,0x10,0x04};

#define ACC_CLASS_11_15_MASK  0xF8
/* MMGSDI info table */
static ds3g_mmgsdi_info_type ds3g_mmgsdi_info;

/* ISIM app info */
static ds3g_mmgsdi_non_prov_isim_session_info_type
             ds3g_mmgsdi_isim_info[MAX_DS3G_MMGSDI_NON_PROV_SESSIONS] = {{0}};

#ifdef FEATURE_CDMA

/* Define RUIM SUPPORT mask */
#define DS3G_MMGSDI_SUPPORT_SIMPLE_IP_MASK 0x01
#define DS3G_MMGSDI_SUPPORT_MOBILE_IP_MASK 0x02
#define DS3G_MMGSDI_SUPPORT_FALLBACK_MASK  0x04

/* MIP info table */
static ds3g_mmgsdi_mip_info_type ds3g_mmgsdi_mip_info;

#endif /* FEATURE_CDMA */
/* The following table records IMSI info reading from USIM
   and it is specific for EHRPD
 */

#define DS3G_MMGSDI_IMSI_OFFSET      0
#define DS3G_MMGSDI_MCC_DIGITS       3
#define DS3G_MMGSDI_MNC_OFFSET       3
#define DS3G_MMGSDI_IMPI_TAG                    0x80
#define DS3G_MMGSDI_IMPI_LEN_OFFSET             0x01
#define DS3G_MMGSDI_IMPI_DATA_OFFSET            0x02
#define DS3G_MMGSDI_IMSI_DIGIT_1_OFFSET         0x01
#define DS3G_MMGSDI_IMSI_DIGIT_2_OFFSET         0x02
#define DS3G_MMGSDI_IMSI_DIGIT_3_OFFSET         0x02
#define DS3G_MMGSDI_IMSI_DIGIT_4_OFFSET         0x03
#define DS3G_MMGSDI_IMSI_DIGIT_5_OFFSET         0x03
#define DS3G_MMGSDI_IMSI_DIGIT_6_OFFSET         0x04
#define DS3G_MMGSDI_IMSI_DIGIT_7_OFFSET         0x04
#define DS3G_MMGSDI_IMSI_DIGIT_8_OFFSET         0x05
#define DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK        0xF0
#define DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK         0x0F

#define DS3G_MMGSDI_SUBSCRIPTION_READY_MASK     0x01
#define DS3G_MMGSDI_SUBS_ID_READY_MASK          0x02
#define DS3G_MMGSDI_USIM_SUBSCRIPTION_READY_MASK     0x04
#define DS3G_MMGSDI_USIM_SUBS_ID_READY_MASK          0x08

/* IMSI info table */
static ds3g_mmgsdi_imsi_info_type
  ds3g_mmgsdi_imsi_info[DS3GSUBSMGR_SUBS_ID_MAX] = {{{NULL}}};

/* IMPI info table */
static ds3g_mmgsdi_impi_info_type
  ds3g_mmgsdi_impi_info[DS3GSUBSMGR_SUBS_ID_MAX] = {{{NULL}}};

#ifdef FEATURE_EHRPD
/* ACL response object */
static ds3g_mmgsdi_acl_info_type ds3g_mmgsdi_acl_info = {NULL};
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_HDR
/* Callback for subscription ready event */
static ds3g_mmgsdi_subscription_ready_cb *ds3g_mmgsdi_on_subscription_ready = NULL;
#endif /* FEATURE_HDR */


/* Storing UE EFacc values for each GW session */
static uint32 ds3g_mmgsdi_efacc_class_value[MAX_DS3G_MMGSDI_SESSIONS] = {0};

static ds3g_mmgsdi_subscription_ready_bit_mask_type
      ds3g_mmgsdi_subscription_ready_status[MAX_DS3G_MMGSDI_SESSIONS] = {{0}};
/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
void ds3g_mmgsdi_client_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);

void ds3g_mmgsdi_client_id_reg_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

void ds3g_mmgsdi_client_evt_reg_cb
(
  const mmgsdi_event_data_type *event
);

void ds3g_mmgsdi_session_reg_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

boolean ds3g_mmgsdi_get_session_type_idx
(
  mmgsdi_session_type_enum_type session_type,
  uint8* index
);

boolean ds3g_mmgsdi_get_session_id_idx
(
  mmgsdi_session_id_type session_id,
  uint8* index
);

void ds3g_mmgsdi_read_transparent_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

void ds3g_mmgsdi_write_transparent_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

void ds3g_mmgsdi_service_available_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

void ds3g_mmgsdi_isim_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
);

void ds3g_mmgsdi_isim_voting_for_refresh
(
  mmgsdi_session_id_type         session_id,
  mmgsdi_session_type_enum_type  session_type
);

#ifdef FEATURE_CDMA
void ds3g_mmgsdi_mip_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

void ds3g_mmgsdi_uim_operational_capability
(
  mmgsdi_session_type_enum_type session_type
);
#endif /* FEATURE_CDMA */

boolean ds3g_mmgsdi_read_imsi
(
  mmgsdi_session_type_enum_type session_type
);

boolean ds3g_mmgsdi_read_impi
(
  mmgsdi_session_type_enum_type  non_prov_session_type
);

void ds3g_mmgsdi_imsi_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

void ds3g_mmgsdi_impi_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

void ds3g_mmgsdi_mnc_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

void ds3g_mmgsdi_format_imsi
(
  mmgsdi_session_id_type session_id
);

void ds3g_mmgsdi_format_imsi_from_impi
(
  mmgsdi_slot_id_enum_type slot_id
);

void ds3g_mmgsdi_read_imsi_from_session_type
(
  mmgsdi_session_type_enum_type session_type
);

void ds3g_mmgsdi_read_impi_from_non_prov_session_type
(
  mmgsdi_session_type_enum_type  non_prov_session_type
);

#ifdef FEATURE_EHRPD
boolean ds3g_mmgsdi_read_acl
(
  mmgsdi_session_type_enum_type session_type
);

void ds3g_mmgsdi_acl_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

boolean ds3g_mmgsdi_if_acl_enabled_read_acl
(
  mmgsdi_session_type_enum_type session_type
);

void ds3g_mmgsdi_if_acl_enabled_read_acl_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

void ds3g_mmgsdi_parse_acl
(
  void
);

int ds3g_mmgsdi_decode_APN
(
  char  *encoded,
  char  *decoded,
  uint8  apn_len
);

void ds3g_mmgsdi_acl_process_done( void );

void ds3g_mmgsdi_acl_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
);
#endif /* FEATURE_EHRPD */


void ds3g_mmgsdi_efacc_update_cache
(
  const ds_cmd_type   *cmd_ptr
);

void ds3g_mmgsdi_reset_efacc_value
(
  mmgsdi_session_type_enum_type session_type
);

void ds3g_mmgsdi_non_prov_isim_session_reg_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

void ds3g_mmgsdi_open_non_prov_isim_session
(
  mmgsdi_slot_id_enum_type slot_id
);

void ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb
(
  const mmgsdi_event_data_type *event
);

mmgsdi_app_info_type* ds3g_mmgsdi_get_isim_app_ptr_from_available_apps
(
   mmgsdi_slot_id_enum_type       slot_id,
   mmgsdi_available_apps_type    *mmgsdi_available_apps_ptr
);

boolean ds3g_mmgsdi_is_isim_app
(
  mmgsdi_aid_type app_data
);

mmgsdi_session_type_enum_type ds3g_mmgsdi_slot_to_non_prov_session_type
(
  mmgsdi_slot_id_enum_type slot
);

boolean ds3g_mmgsdi_get_non_prov_isim_session_type_idx
(
  mmgsdi_session_type_enum_type  session_type,
  uint8                         *index
);

boolean ds3g_mmgsdi_get_non_prov_isim_session_id
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_session_id_type         *session_id_ptr
);

void ds3g_mmgsdi_non_prov_isim_session_register_for_refresh
(
  uint8 index
);

/*===========================================================================
FUNCTION  DS3G_MMGSDI_GET_IMPI_INFO_FROM_SLOT

DESCRIPTION
  This function fetches the IMPI info based on Slot Id

PARAMETERS
  Slot

DEPENDENCIES
  None.

RETURN VALUE
  IMPI Info Ptr

SIDE EFFECTS
  None.
===========================================================================*/
ds3g_mmgsdi_impi_info_type* ds3g_mmgsdi_get_impi_info_from_slot
(
  mmgsdi_slot_id_enum_type  slot_id
);

/*===========================================================================
FUNCTION      DS3G_MMGSDI_GET_IMSI_INFO_FROM_SESSION_ID

DESCRIPTION   This function fetches the IMSI info based on Session Id

PARAMETERS    Session Id

DEPENDENCIES  None.

RETURN VALUE  IMSI Info Ptr

SIDE EFFECTS  None.
===========================================================================*/
ds3g_mmgsdi_imsi_info_type* ds3g_mmgsdi_get_imsi_info_from_session_id
(
  mmgsdi_session_id_type session_id
);

/*===========================================================================
FUNCTION      DS3G_MMGSDI_GET_IMSI_INFO_FROM_SESSION_ID

DESCRIPTION   This function fetches the IMSI info based on Session Type

PARAMETERS    Session Type

DEPENDENCIES  None.

RETURN VALUE  IMSI Info Ptr

SIDE EFFECTS  None.
===========================================================================*/
ds3g_mmgsdi_imsi_info_type* ds3g_mmgsdi_get_imsi_info_from_session_type
(
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================

FUNCTION ds3gpp_mmgsdi_is_reset_cc_sp_throttle_required

DESCRIPTION
  A command processing function is called based on the type of command received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_reset_cc_sp_throttle_if_required
(
  ds_cmd_type  *cmd_ptr
);

boolean ds3g_mmgsdi_get_subscription_ready_status_idx
(
  mmgsdi_session_type_enum_type session_type,
  uint8*                        index
);

boolean ds3g_mmgsdi_set_and_notify_subscription_ready_status
(
  uint8                         mask,
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_NOTIFY_IMSI_INFO_EV

DESCRIPTION
  This function notifes the IMSI info to the registered clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_notify_imsi_info_ev
(
  sys_modem_as_id_e_type       subs_id,
  mmgsdi_app_enum_type         app_type,
  ds3g_mmgsdi_imsi_info_type  *imsi_info_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_NOTIFY_IMPI_INFO_EV

DESCRIPTION
  This function notifes the IMPI info to the registered clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_notify_impi_info_ev
(
  sys_modem_as_id_e_type       subs_id,
  ds3g_mmgsdi_impi_info_type  *impi_info_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_RESET_IMSI_INFO

DESCRIPTION
  This function resets the IMSI info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_reset_imsi_info
(
  sys_modem_as_id_e_type       subs_id,
  mmgsdi_app_enum_type         app_type,
  ds3g_mmgsdi_imsi_info_type  *imsi_info_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_RESET_IMPI_INFO

DESCRIPTION
  This function resets the IMPI info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_reset_impi_info
(
  sys_modem_as_id_e_type       subs_id
);

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_MMGSDI_INIT

DESCRIPTION   This Function registers DS manager as a client of MMGSDI

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_mmgsdi_init
(
  void
)
{
  mmgsdi_return_enum_type        status = MMGSDI_SUCCESS;
  uint8                          i = 0;
  mmgsdi_session_type_enum_type  ds3g_mmgsdi_session_type[MAX_DS3G_MMGSDI_SESSIONS] =
                                   { MMGSDI_GW_PROV_PRI_SESSION,
                                     MMGSDI_1X_PROV_PRI_SESSION,
                                     MMGSDI_GW_PROV_SEC_SESSION,
                                     MMGSDI_1X_PROV_SEC_SESSION
                                     #ifdef FEATURE_TRIPLE_SIM
                                     ,MMGSDI_GW_PROV_TER_SESSION
                                     ,MMGSDI_1X_PROV_TER_SESSION
                                     #endif /* FEATURE_TRIPLE_SIM */
                                   };

  mmgsdi_session_type_enum_type
    ds3g_mmgsdi_non_prov_isim_session_type[MAX_DS3G_MMGSDI_NON_PROV_SESSIONS] =
  { MMGSDI_NON_PROV_SESSION_SLOT_1,
    MMGSDI_NON_PROV_SESSION_SLOT_2
#ifdef FEATURE_TRIPLE_SIM
    ,MMGSDI_NON_PROV_SESSION_SLOT_3
#endif /* FEATURE_TRIPLE_SIM */
  };

#ifdef FEATURE_EHRPD
  ds707_data_session_acl_info_type *ds707_acl_info = NULL;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the MMGSDI info table */
  ds3g_mmgsdi_info.client_id = DS3G_MMGSDI_INVALID_CLIENT_ID;
  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id =
                                             DS3G_MMGSDI_INVALID_SESSION_ID;
    ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type =
                                             ds3g_mmgsdi_session_type[i];
    ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].app_type = MMGSDI_APP_NONE;
    ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].is_operational = FALSE;
    ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].sub_id = SYS_MODEM_AS_ID_NONE;

    ds3g_mmgsdi_subscription_ready_status[i].session_type =
                                               ds3g_mmgsdi_session_type[i];
    ds3g_mmgsdi_subscription_ready_status[i].subscription_ready_status = 0;
  }

  for ( i = 0; i < MAX_DS3G_MMGSDI_NON_PROV_SESSIONS; i++ )
  {
    ds3g_mmgsdi_isim_info[i].is_operational = FALSE;
    ds3g_mmgsdi_isim_info[i].session_type =
      ds3g_mmgsdi_non_prov_isim_session_type[i];
    memset(&ds3g_mmgsdi_isim_info[i].app_info,0,sizeof(mmgsdi_aid_type));
    ds3g_mmgsdi_isim_info[i].channel_id = 0;
    ds3g_mmgsdi_isim_info[i].session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
    ds3g_mmgsdi_isim_info[i].slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
  }

#ifdef FEATURE_CDMA
  /* Initialize the MIP info table */
  memset(ds3g_mmgsdi_mip_info.mip_response,
         0,
         UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH );

  ds3g_mmgsdi_mip_info.status = FALSE;
#endif /* FEATURE_CDMA */

  /* Initialize the IMSI info table */
  for (i=0; i< DS3GSUBSMGR_SUBS_ID_MAX; i++)
  {
    memset( ds3g_mmgsdi_imsi_info[i].imsi_response, 0, DS3G_MMGSDI_IMSI_LEN );

    ds3g_mmgsdi_imsi_info[i].mnc_response[0] = 0;

    memset( (void*)&ds3g_mmgsdi_imsi_info[i].imsi_info,
            0,
            sizeof(nv_ehrpd_imsi_type) );

    ds3g_mmgsdi_imsi_info[i].ready_status = FALSE;
  }

  /* Initialize the IMPI info table */
  for (i=0; i< DS3GSUBSMGR_SUBS_ID_MAX; i++)
  {
    memset( (void*)&ds3g_mmgsdi_impi_info[i].impi_info, 
            0,
            sizeof(mmgsdi_data_type) );

    memset( (void*)&ds3g_mmgsdi_impi_info[i].imsi_info,
            0,
            sizeof(nv_ehrpd_imsi_type) );

    ds3g_mmgsdi_impi_info[i].ready_status = FALSE;
  }

#ifdef FEATURE_EHRPD
  /*---------------------------------------------------
    Initialize ACL information for mmsgdi module
  ---------------------------------------------------*/
  ds3g_mmgsdi_acl_info.acl_response = NULL;
  ds3g_mmgsdi_acl_info.acl_response_len = 0;

  /*---------------------------------------------------
    For profile re-arch, data session profile init
    will occur before mmgsdi init. This is reverted
    back for hotswap feature.
  ---------------------------------------------------*/
  ds707_acl_info = ds707_data_session_get_acl_info_ptr();
  memset(ds707_acl_info,
         0,
         sizeof(ds707_data_session_acl_info_type) );
  ds707_acl_info->num_apn = -1;
  ds707_acl_info->ds3g_mmgsdi_init = FALSE;

  /*---------------------------------------------------
    Reading nv item 06862
  ---------------------------------------------------*/
  ds3g_mmgsdi_acl_info.acl_read_from_usim = ds3g_get_ehrpd_auth_in_usim();

  /*---------------------------------------------------
    If acl read mode is S/W then don't read ACL from
    card and mark the initialization flag to TRUE.
  ---------------------------------------------------*/
  if( ds3g_mmgsdi_acl_info.acl_read_from_usim == FALSE )
  {
    ds3g_mmgsdi_acl_process_done();
  }
#endif /* FEATURE_EHRPD */

  /* Register Client Id with MMGSDI */
  status = mmgsdi_client_id_and_evt_reg( ds3g_mmgsdi_client_evt_reg_cb,
                                         ds3g_mmgsdi_client_id_reg_cb,
                                         0 );
  if ( status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "DS3G MMGSDI client registration failed [%d]", status);
  }

  return;
} /* ds3g_mmgsdi_init */

/*===========================================================================
FUNCTION DS3G_MMGSDI_CLIENT_ID_REG_CB

DESCRIPTION
  This function handles the MMGSDI client ID registration command callback.
  It will assign the global client ID value which will be used in mmgsdi
  function calls

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_client_id_reg_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type                  *cmd_ptr = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cnf_ptr != NULL );

  if (status == MMGSDI_SUCCESS)
  {
    if (cnf == MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)
    {
      /* We get client Id */
      ds3g_mmgsdi_info.client_id =
        cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                      "ds3g_mmgsdi_client_id_reg_cb(): Client Id = [%d]",
                      ds3g_mmgsdi_info.client_id);

      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }
      else
      {
        cmd_ptr->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
        mmgsdi_event_info_ptr
          = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

        mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_CLIENT_ID_EVENT;
        ds_put_cmd(cmd_ptr);
      }
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "ds3g_mmgsdi_client_id_reg_cb(): Invalid CNF rcvd:%d",
                      cnf);
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "ds3g_mmgsdi_client_id_reg_cb(): "
                    "Client ID Registration Failed [%d]",status);
  }

  return;
} /* ds3g_mmgsdi_client_id_reg_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SESSION_REG_CB

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback.
  It will assign the global Session ID value which will be used in mmgsdi
  function calls

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_session_reg_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  uint8  index = 0;
  uint8  i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cnf_ptr)
  {
    DATA_ERR_FATAL("cnf_ptr is NULL");
    return;
  }

  if (MMGSDI_SUCCESS == status)
  {
    if (MMGSDI_SESSION_OPEN_EXT_CNF == cnf)
    {
      for (i = 0; i < cnf_ptr->session_open_ext_cnf.num_sessions; i++)
      {
        if ( ds3g_mmgsdi_get_session_type_idx(
                cnf_ptr->session_open_ext_cnf.session_info_ptr[i].session_type,
               &index ) == TRUE )
        {
          ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_id =
            cnf_ptr->session_open_ext_cnf.session_info_ptr[i].session_id;
          DATA_3GMGR_MSG2(MSG_LEGACY_MED,
               "Received Session Id = [%d], Session Type = [%d],",
               ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_id,
               ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_type);

          /* Post client & session info to ATCoP */
          dsat_mmgsdi_client_init(&(cnf_ptr->session_open_ext_cnf.session_info_ptr[i]),
                                  cnf_ptr->session_open_ext_cnf.response_header.client_id);
        }
        else
        {
          DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
               "Failed to find Session Type = [%d] from the table",
               cnf_ptr->session_open_ext_cnf.session_info_ptr[i].session_type);
        }
      }
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Received Invalid CNF [%d] from mmgsdi", cnf);
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Session ID Registration Failed [%d]", status);
  }

  return;
} /* ds3g_mmgsdi_session_reg_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_CLIENT_RSP_CB

DESCRIPTION
  This function handles the MMGSDI registration response callback.

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_client_rsp_cb
(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  ASSERT( cnf_ptr != NULL );

  DATA_3GMGR_MSG4(MSG_LEGACY_HIGH,
                  "ds3g_mmgsdi_client_rsp_cb() for cnf = [%d] "
                  "with status = [%d] mmgsdi_status = [%d] client_data = [%d]",
                  cnf, status,
                  cnf_ptr->response_header.mmgsdi_status,
                  cnf_ptr->read_cnf.response_header.client_data );

  return;
}/* ds3g_mmgsdi_client_rsp_cb */

/*==============================================================================
FUNCTION DS3G_MMGSDI_CLIENT_EVT_REG_CB

DESCRIPTION
  This function handles the MMGSDI client event registration callback.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_client_evt_reg_cb
(
  const mmgsdi_event_data_type  *event
)
{
  ds_cmd_type                   *cmd_ptr = NULL;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  ds_at_mmgsdi_event_type       *cmd_rd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( event == NULL )
  {
    /* if not event info, we return right away */
    return;
  }

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if ( ds3g_mmgsdi_validate_client_id( event->client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_client_evt_reg_cb(): Invalid client Id [%d]",
                    event->client_id);
    return;
  }

  DATA_3GMGR_MSG3(
     MSG_LEGACY_HIGH,
     "ds3g_mmgsdi_client_evt_reg_cb(): event:%d client_id:%d session_id:%d",
     event->evt, event->client_id, event->session_id);

  /* Post the event to ATCoP */
  dsat_mmgsdi_event_cb(event);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  mmgsdi_event_info_ptr
    = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  mmgsdi_event_info_ptr->session_id = DS3G_MMGSDI_INVALID_SESSION_ID;

  switch( event->evt )
  {
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      /* send command to ds task to process MMGSDI_SUBSCRIPTION_READY_EVT */
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT;
      mmgsdi_event_info_ptr->session_id = event->session_id;
      // record application type
      mmgsdi_event_info_ptr->app_type
        = event->data.subscription_ready.app_info.app_data.app_type;
      // record slot info
      mmgsdi_event_info_ptr->slot
        = event->data.subscription_ready.app_info.slot;

      /*----------------------------------------------------------------------
        This just a quick fix to inform PS about this event, instead of PS
        doing client registration and other stuff. This is in-line with
        consolidating all DATA entities to have a single interface to MMGSDI
        [the mainline solution]!
      -------------------------------------------------------------------------*/
      ps_send_cmd(PS_SIM_EVENT_HANDLER_CMD, (void *)event->evt);
      DATA_3GMGR_MSG4(MSG_LEGACY_HIGH,
                      "MMGSDI_SUBSCRIPTION_READY_EVT: session_id:[%d],"
                      "event_type:[%d] app_type:[%d] slot:[%d]",
                      mmgsdi_event_info_ptr->session_id,
                      mmgsdi_event_info_ptr->event_type,
                      mmgsdi_event_info_ptr->app_type,
                      mmgsdi_event_info_ptr->slot );
      break;

    case MMGSDI_REFRESH_EVT:
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "MMGSDI_REFRESH_EVT: refresh stage [%d] ",
                  event->data.refresh.stage);

      if (event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT)
      {
         /* send refresh voting command to ds task */
         mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_REFRESH_VOTING_EVENT;
         mmgsdi_event_info_ptr->session_id = event->session_id;
      }
      else if ( event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN )
      {
         /* send OK refresh command to ds task */
         mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_OK_REFRESH_EVENT;
         mmgsdi_event_info_ptr->session_id = event->session_id;
      }
      else if( event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START )
      {
        if (MMGSDI_REFRESH_NAA_FCN == event->data.refresh.mode)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                          "Refresh mode is MMGSDI_REFRESH_NAA_FCN, ignoring");
          ds_release_cmd_buf(&cmd_ptr);
          return;
        }

        mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_REFRESH_START_EVENT;
        mmgsdi_event_info_ptr->slot = event->data.refresh.slot;
        mmgsdi_event_info_ptr->app_type
          = event->data.refresh.app_info.app_data.app_type;

        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "process MMGSDI_REFRESH_STAGE_START");
      }
      else if(event->data.refresh.stage == MMGSDI_REFRESH_STAGE_END_SUCCESS)
      {
         cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_event_info);

        if((NULL == event->data.refresh.refresh_files.file_list_ptr) ||
           (0 == event->data.refresh.refresh_files.num_files) ||
           (NULL == cmd_rd_ptr))
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "ds3g_mmgsdi_client_evt_reg_cb(): NULL PTR");
          /* Free DS cmd buffer, this being a error case */
          ds_release_cmd_buf(&cmd_ptr);
          return;
        }

        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Received file change notification [%d]",
                        event->data.refresh.refresh_files.num_files);
        /* Fill in command type */
        cmd_rd_ptr->mmgsdi_event = event->evt;
        cmd_rd_ptr->cmd_status =(int16) MMGSDI_SUCCESS;

        /* Copy data to command buffer */
        cmd_rd_ptr->u.fcn_ind.num_files =
        event->data.refresh.refresh_files.num_files;
        memscpy(cmd_rd_ptr->u.fcn_ind.file_list,
                sizeof(cmd_rd_ptr->u.fcn_ind.file_list),
                event->data.refresh.refresh_files.file_list_ptr,
                (MIN(event->data.refresh.refresh_files.num_files,
                     DS3G_MMGSDI_MAX_NUM_REFRESH_FILES) *
                 sizeof(mmgsdi_file_enum_type)));

        /* send File change event command to DS task */
        mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_REFRESH_COMPLETE_EVENT;
        mmgsdi_event_info_ptr->session_id = event->session_id;
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Process MMGSDI_REFRESH_NAA_FCN");
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Ignoring MMGSDI REFRESH event");
        /* Free DS cmd buffer, this being a error case */
        ds_release_cmd_buf(&cmd_ptr);
        return;
      }
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_SESSION_CHANGED_EVENT;
      mmgsdi_event_info_ptr->slot = event->data.session_changed.app_info.slot;
      mmgsdi_event_info_ptr->session_id = event->session_id;
      memscpy( &(mmgsdi_event_info_ptr->session_changed_info),
               sizeof(mmgsdi_session_changed_evt_info_type),
               &(event->data.session_changed),
               sizeof(mmgsdi_session_changed_evt_info_type) );
      break;

    case MMGSDI_CARD_ERROR_EVT:
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_CARD_ERROR_EVENT;
      mmgsdi_event_info_ptr->slot = event->data.card_error.slot;

      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                      "MMGSDI_CARD_ERROR_EVT: event_type [%d], slot [%d]",
                      mmgsdi_event_info_ptr->event_type,
                      mmgsdi_event_info_ptr->slot);
      break;

    default:
      // all other events are not of interest to DS manager
       DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "not of interest to DS3G, Ignoring event");
       /* Free DS cmd buffer, this being a error case */
       ds_release_cmd_buf(&cmd_ptr);
       return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
  ds_put_cmd( cmd_ptr );

  return;
} /* ds3g_mmgsdi_client_evt_reg_cb */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_CLIENT_ID_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_CLIENT_ID_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_client_id_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  uint32                   session_mask = 0;
  mmgsdi_return_enum_type  status = MMGSDI_ERROR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);

  /* check session type */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid client id [%d]",
                    ds3g_mmgsdi_info.client_id);
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Client ID [%d] is registered",
                  ds3g_mmgsdi_info.client_id);

  session_mask = MMGSDI_GW_PROV_PRI_SESSION_MASK |
                 MMGSDI_1X_PROV_PRI_SESSION_MASK |
                 MMGSDI_GW_PROV_SEC_SESSION_MASK |
                 MMGSDI_1X_PROV_SEC_SESSION_MASK
#ifdef FEATURE_TRIPLE_SIM
                 | MMGSDI_GW_PROV_TER_SESSION_MASK
                 | MMGSDI_1X_PROV_TER_SESSION_MASK
#endif /* FEATURE_TRIPLE_SIM */
                 ;

  /* open 6 sessions */
  status = mmgsdi_session_open_ext(ds3g_mmgsdi_info.client_id,
                                   session_mask,
                                   ds3g_mmgsdi_client_evt_reg_cb,
                                   0,
                                   ds3g_mmgsdi_session_reg_cb,
                                   0);
  if ( status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_client_id_reg_cb(): Session Open Registration "
                    "Failed status = [%d]",
                    status);
  }

  return;
}/* ds3g_mmgsdi_client_id_event_process */

/*===========================================================================

FUNCTION ds3g_mmgsdi_update_subs_ready_ev

DESCRIPTION
  A command processing function is called based on the type of command received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_update_subs_ready_ev
(
   sys_modem_as_id_e_type         subs_id
)
{
 /*- - - - - - - - - - - - - - -- - - - - -- - - - - - - - - - - - - - - */
  ds_3gpp_roaming_update_plmn_info_on_subs_ready(subs_id);
  return;
} /*ds3g_mmgsdi_update_subs_ready_ev  */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SUBS_READY_TO_PROCESS_USIM

DESCRIPTION
  Function to determine if both subscription ready event and subs id info
  has been received for USIM session to process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3g_mmgsdi_subs_ready_to_process_usim
(
  uint8                          mask,
  mmgsdi_session_type_enum_type  session_type
)
{
  uint8                          index = 0;
  uint8                          non_prov_index = 0;
  uint8                          ready_mask = 0;
  sys_modem_as_id_e_type         subs_id;
  mmgsdi_slot_id_enum_type       slot_id = 0;   
  mmgsdi_session_type_enum_type  non_prov_session_type
                                            = MMGSDI_MAX_SESSION_TYPE_ENUM;
 /*- - - - - - - - - - - - - - -- - - - - -- - - - - - - - - - - - - - - */

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "ds3g_mmgsdi_subs_ready_to_process_usim, mask %d session type %d",
                  mask, session_type);

  if ((mask != DS3G_MMGSDI_USIM_SUBSCRIPTION_READY_MASK) &&
      (mask != DS3G_MMGSDI_USIM_SUBS_ID_READY_MASK))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Invalid ds3g mmgsdi status usim mask %d ",
                    mask);
    return FALSE;
  }

  if(session_type == MMGSDI_GW_PROV_PRI_SESSION ||
     session_type == MMGSDI_GW_PROV_SEC_SESSION
#ifdef FEATURE_TRIPLE_SIM
     || session_type == MMGSDI_GW_PROV_TER_SESSION
#endif /* FEATURE_TRIPLE_SIM */
    )
  {
    if (ds3g_mmgsdi_get_subscription_ready_status_idx(session_type,&index)
          == FALSE)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Failed to get subscription ready status index from "
                      "session type %d ",
                      session_type);
      return FALSE;
    }

    ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status |= mask;

    ready_mask = DS3G_MMGSDI_USIM_SUBSCRIPTION_READY_MASK |
                 DS3G_MMGSDI_USIM_SUBS_ID_READY_MASK;

    if ((ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status & 
         ready_mask) == ready_mask)
    {
      ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status &=
                 (~ready_mask);

      if(ds3g_mmgsdi_get_slot(session_type, &slot_id) == FALSE)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                       "Failed to get slot id from session type %d ",
                       session_type);
        return FALSE;
      }

      non_prov_session_type = ds3g_mmgsdi_slot_to_non_prov_session_type(slot_id);

      if (ds3g_mmgsdi_get_non_prov_isim_session_type_idx(non_prov_session_type,
                                                        &non_prov_index)
             == FALSE)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                        "Could not get index for non prov session %d",
                        non_prov_session_type);
      }
      else
      {
        if (ds3g_mmgsdi_isim_info[non_prov_index].is_operational == FALSE)
        {
          ds3g_mmgsdi_open_non_prov_isim_session(slot_id);
        }
        else
        {
          DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                          "non-prov ISIM session already open on slot %d",
                          slot_id);
        }
      }

      ds3g_mmgsdi_read_imsi_from_session_type(session_type);

      subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);
      //ds3g_mmgsdi_update_subs_ready_ev(subs_id);

      return TRUE;
    }
  }

  return FALSE;
} /* ds3g_mmgsdi_subs_ready_to_process_usim */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_SUBSCRIPTION_READY_EVENT
and it should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_subscription_ready_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
  uint8                          index = 0;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  sys_modem_as_id_e_type         subs_id;
#ifdef FEATURE_DATA_RPM
  mmgsdi_return_enum_type        ret_val = MMGSDI_MAX_RETURN_ENUM;
  boolean                        iccid_changed = FALSE;
  
#endif /* FEATURE_DATA_RPM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;
  session_id = mmgsdi_event_info_ptr->session_id;

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Subscription Ready Event: session_id:%d app_type:%d",
                  session_id, mmgsdi_event_info_ptr->app_type);

  /* Get session id index from the table */
  if ( ds3g_mmgsdi_get_session_id_idx( session_id, &index ) == TRUE )
  {
     /* update operational flag in the table */
     ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].is_operational = TRUE;
     ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].app_type
       = mmgsdi_event_info_ptr->app_type;
     ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].slot
       = mmgsdi_event_info_ptr->slot;
     session_type = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_type;

     ds3g_mmgsdi_notify_event( DS3GEVENTMGR_SUB_AVAIL_EV,
                               session_type,
                               NULL);
     if( ds3g_mmgsdi_set_and_notify_subscription_ready_status(
            DS3G_MMGSDI_SUBSCRIPTION_READY_MASK,
            session_type) == FALSE )
     {
       DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                       "cannot set DS3G_MMGSDI_SUBSCRIPTION_READY_MASK");
     }

     switch ( session_type )
     {
       case MMGSDI_1X_PROV_PRI_SESSION:
       case MMGSDI_1X_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
       case MMGSDI_1X_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */
         DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                         "Subscription Ready for a CDMA session:%d",
                         session_type);

#ifdef FEATURE_CDMA
         /* synchronize NV items for 1x */
         (void)ds707_pkt_mgr_mmgsdi_update(DS707_MMGSDI_CDMA_NV_UPDATE);

        /* Register with MMGSDI for refresh handler */
         ds3g_mmgsdi_1x_register_for_refresh( session_type );

         /* DS does not need to check CDMA service table, UIM shall read
            SIP/MIP/HRPD/3GPD_ext during the power on and update
            the table by themselves. */
         //check the 3GPD operation vlaue
         if ( NVRUIM_3GPD_MIP_NV_SIP_NV != uim_3gpd_support())
         {
           //Update operation table
           ds3g_mmgsdi_uim_operational_capability( session_type );
         }
         /*-------------------------------------------------------------------------
            Read Profile parameters from RUIM when it gets intialized
         -------------------------------------------------------------------------*/
         (void)ds707_pkt_mgr_mmgsdi_update( DS707_MMGSDI_CDMA_PROFILE_UPDATE );

#ifdef FEATURE_HDR
        /* Notify the users that subscription is ready */
        if (NULL != ds3g_mmgsdi_on_subscription_ready)
        {
          ds3g_mmgsdi_on_subscription_ready(TRUE);
        }
#endif /* FEATURE_HDR */

#ifdef FEATURE_MMGSDI_SESSION_LIB
#ifdef FEATURE_DATA_IS707
          ds707_read_jcdma_card_info(session_id);
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_CDMA */
         break;

      case MMGSDI_GW_PROV_PRI_SESSION:
      case MMGSDI_GW_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
      case MMGSDI_GW_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */
         DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                         "Subscription Ready for a UMTS session:%d",
                         session_type);
         /* Invoke ISIM data refresh voting */
         /* Register for refresh voting */
         ds3g_mmgsdi_isim_register_for_refresh( session_type );

         if(ds3g_mmgsdi_subs_ready_to_process_usim(
                           DS3G_MMGSDI_USIM_SUBSCRIPTION_READY_MASK,
                           session_type) == FALSE)
           {
           DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                           "subs not ready to process usim session");
         }

         /* Read register for EFacc file refresh */
         ds3g_mmgsdi_efacc_register_for_refresh(session_type);
         ds3g_mmgsdi_read_efacc(session_type);

         subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);
         ds3g_mmgsdi_update_subs_ready_ev(subs_id);

#ifdef FEATURE_EHRPD
         /* Register for acl refresh */
         ( void )ds3g_mmgsdi_acl_register_for_refresh( session_type );

         /*-----------------------------------------------------
           Read ACL from card based if NV item (6862) is TRUE.
         -----------------------------------------------------*/
         if( ds3g_mmgsdi_acl_info.acl_read_from_usim == TRUE )
         {
           /* Check if ACL service is enabled, and then read ACL */
           ( void )ds3g_mmgsdi_if_acl_enabled_read_acl( session_type );
         }
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_MMGSDI_SESSION_LIB
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
       /* ------------------------------------------------------------------------
          Now read the BCD number and sub address from SIM.
          We read only if the app_type is MMGSDI_APP_SIM or MMGSDI_APP_USIM
       -------------------------------------------------------------------------*/
       ds_ucsd_read_calling_party_subaddress(session_id);
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#ifdef FEATURE_DATA_RPM
       ret_val = mmgsdi_rpm_has_iccid_changed(mmgsdi_event_info_ptr->slot,
                                              &iccid_changed);
       
       if (MMGSDI_SUCCESS == ret_val)
       {
         if (TRUE == iccid_changed)
         {
           subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);
           ds_3gpp_rpm_process_iccid_chg(subs_id);
         }
         else
         {
           DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ICCID hasn't change is");
         }
       }
       else
       {
         DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                         "mmgsdi_rpm_has_iccid_changed returned: %d",ret_val);
       }
#endif /* FEATURE_DATA_RPM */
       break;

      default:
         DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                         "Subscription Ready for an invalid session type [%d]",
                         session_type);
         break;
     }
  }
  else
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_LOW,
                     "Session ID not matched. Session ID = %d",
                     session_id);
  }
  return;
} /* ds3g_mmgsdi_subscription_ready_event_process */

/*===========================================================================
FUNCTION DS3G_MMGSDI_REFRESH_VOTING_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_REFRESH_EVT with
MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT from MMGSDI interface.
This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_refresh_voting_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  sys_modem_as_id_e_type         cm_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_MED,"ds3g_mmgsdi_refresh_voting_event_process()");

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id,
                                     &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an activated Session Id:%d from the table,"
                    " Voting YES to Refresh",
                    session_id);
    /* Voting OK to Refresh as session ID is not active and it doesnt effect DS */
    mmgsdi_status = mmgsdi_session_ok_to_refresh( session_id,
                                                  TRUE,
                                                  ds3g_mmgsdi_client_rsp_cb,
                                                ( mmgsdi_client_data_type ) 0 );
    /* Check to see if REQ was OK.  */
    if ( mmgsdi_status != MMGSDI_SUCCESS)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Failed to call MMGSDI OK to Refresh [%d]",
                      mmgsdi_status);
    }
    return;
  }

  ds3g_mmgsdi_notify_event(DS3GEVENTMGR_MMGSDI_REFRESH_VOTING_EV,
                           session_type,
                           NULL);

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "Refresh voting with session_id [%d] session type [%d]",
                  session_id, session_type);

  cm_subs_id = ds3g_mmgsdi_get_sub_id_from_session_id(session_id);
  if( FALSE == ds3g_get_refresh_vote_ok_info(cm_subs_id) )
  {
    ds3g_mmgsdi_isim_voting_for_refresh( session_id, session_type );
  }

  else
  {
    mmgsdi_status = ds3g_mmgsdi_ok_to_refresh( TRUE, session_type );
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "Refresh voting failed with return status [%d], "
                      "session_type [%d]", mmgsdi_status, session_type);
    }
  }
  return;
} /* ds3g_mmgsdi_refresh_voting_event_process */


/*===========================================================================
FUNCTION DS3G_MMGSDI_OK_REFRESH_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_REFRESH_EVT with
MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN from MMGSDI interface.
This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_ok_refresh_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id, &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Failed to find an activated "
                    "Session Id = [%d] from the table", session_id);
    return;
  }

  ds3g_mmgsdi_notify_event(DS3GEVENTMGR_MMGSDI_OK_REFRESH_EV,
                           session_type,
                           NULL);

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "OK to refresh with session_id [%d], session type [%d]",
                  session_id, session_type);

  /* Invoke OK to refresh to a session */
  mmgsdi_status = ds3g_mmgsdi_ok_to_refresh( TRUE, session_type );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "OK to refresh failed with  status:%d session_type:%d",
                    mmgsdi_status, session_type);
  }
  return;
} /* ds3g_mmgsdi_ok_refresh_event_process */

/*===========================================================================
FUNCTION DS3G_MMGSDI_REFRESH_FCN_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving file change notification from
MMGSDI interface. This function should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_refresh_fcn_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  boolean                        pass_fail = TRUE;
  uint8                          index = 0;
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  dsat_fcn_ind_type              fcn_indication;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  boolean                        atcop_refresh = FALSE;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&fcn_indication, 0, sizeof(dsat_fcn_ind_type));

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id, &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an activated Session Id:%d from the table",
                    session_id);
    return;
  }

  ds3g_mmgsdi_notify_event(DS3GEVENTMGR_MMGSDI_REFRESH_FCN_EV,
                           session_type,
                           NULL);

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "refresh fcn event process:session_id:%d session_type:%d",
                  session_id,
                  session_type);

  if (( session_type == MMGSDI_1X_PROV_PRI_SESSION ) ||
      ( session_type == MMGSDI_1X_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
      || ( session_type == MMGSDI_1X_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
     )
  {
    fcn_indication =
      mmgsdi_event_info_ptr->cmd_info.mmgsdi_event_info.u.fcn_ind;

    if(( fcn_indication.num_files == 0 )||
       ( fcn_indication.num_files > DS3G_MMGSDI_MAX_NUM_REFRESH_FILES ))
    {
      pass_fail = FALSE;
    }
    else
    {
      while(( index < fcn_indication.num_files )&&
            ( pass_fail != FALSE ))
      {
        switch( fcn_indication.file_list[index] )
        {
#if defined( FEATURE_DS_MOBILE_IP ) && defined(FEATURE_DATA_IS707)
          case MMGSDI_CDMA_3GPD_3GPDOPM:
            /* update 1xMH */
            pass_fail=ds707_pkt_mgr_mmgsdi_update(
              DS707_MMGSDI_CDMA_3GPD_3GPDOPM_UPDATE );
            break;

          case MMGSDI_CDMA_3GPD_MIPSP:
            /* update 1xMH */
            pass_fail=ds707_pkt_mgr_mmgsdi_update(
              DS707_MMGSDI_CDMA_3GPD_MIPSP_UPDATE );
            break;
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DATA_IS707 */

          default:
            DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                            "Unknown file [%d] in file list ,pass_fail = FALSE",
                            fcn_indication.file_list[index]);
            pass_fail=FALSE;
            break;
        }/* Switch end */
        index++;
      }/* While end */
    }

    mmgsdi_status = ds3g_mmgsdi_refresh_complete( pass_fail, session_type );
    if ( mmgsdi_status != MMGSDI_SUCCESS )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Sending Refresh Complete to MMGSDI "
                      "status [%d]", mmgsdi_status);
    }
  }
  else if (( session_type == MMGSDI_GW_PROV_PRI_SESSION ) ||
           ( session_type == MMGSDI_GW_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
           || ( session_type == MMGSDI_GW_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
          )
  {
    fcn_indication =
      mmgsdi_event_info_ptr->cmd_info.mmgsdi_event_info.u.fcn_ind;

    if(( fcn_indication.num_files == 0 )||
       ( fcn_indication.num_files > DS3G_MMGSDI_MAX_NUM_REFRESH_FILES ))
    {
      pass_fail = FALSE;
    }
    else
    {
      while(( index < fcn_indication.num_files )&&
            ( pass_fail != FALSE ))
      {
        switch( fcn_indication.file_list[index] )
        {
#ifdef FEATURE_EHRPD
          case MMGSDI_USIM_ACL:
            pass_fail = TRUE;
            /*-----------------------------------------------------
              Read NV item (6862), if set then only read ACL
              from card.
            -----------------------------------------------------*/
            if( ds3g_get_ehrpd_auth_in_usim() == TRUE )
            {
              /* Check if ACL service is enabled, and then read ACL  */
              pass_fail = ds3g_mmgsdi_if_acl_enabled_read_acl( session_type );
            }
            break;
#endif /* FEATURE_EHRPD */

          case MMGSDI_GSM_PNN:
          case MMGSDI_GSM_SPDI:
          case MMGSDI_GSM_CSP:
          case MMGSDI_GSM_ONS:
          case MMGSDI_GSM_SST:
          case MMGSDI_GSM_OPL:
          case MMGSDI_GSM_SPN:
          case MMGSDI_USIM_PNN:
          case MMGSDI_USIM_SPDI:
          case MMGSDI_USIM_CSP:
          case MMGSDI_USIM_ONS:
          case MMGSDI_USIM_OPL:
          case MMGSDI_USIM_SPN:
            DATA_3GMGR_MSG1( MSG_LEGACY_HIGH, "ATCoP file [%d] in file list", 
                             fcn_indication.file_list[index] );
            atcop_refresh = TRUE;
            break;

#ifdef FEATURE_DATA_RPM
          /* RPM EF enums */
          case MMGSDI_USIM_7F66_PROP1_RPM_EF:
          case MMGSDI_USIM_7F66_PROP1_RPM_PARAM:
          case MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR:
          case MMGSDI_USIM_7F66_PROP1_RPM_CNTRS:

          case MMGSDI_SIM_7F66_PROP1_RPM_EF:
          case MMGSDI_SIM_7F66_PROP1_RPM_PARAM:
          case MMGSDI_SIM_7F66_PROP1_RPM_CNTR_LR:
          case MMGSDI_SIM_7F66_PROP1_RPM_CNTRS:
            ds_3gpp_rpm_handle_mmgsdi_refresh(fcn_indication.file_list[index],
                                              session_id);
            break;
#endif /* FEATURE_DATA_RPM */


          case MMGSDI_USIM_ACC:
          case MMGSDI_GSM_ACC:
            ds3g_mmgsdi_read_efacc(session_type);
            break;

          default:
            DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                            "Unknown file [%d] in file list ; handling gracefully",
                            fcn_indication.file_list[index]);
            pass_fail=TRUE;
            break;
        }/* Switch end */
        index++;
      }/* While end */
    }

    if(atcop_refresh == TRUE)
    {
      return;
    }

    mmgsdi_status = ds3g_mmgsdi_refresh_complete( pass_fail, session_type );
    if ( mmgsdi_status != MMGSDI_SUCCESS )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                      "Sending Refresh Complete to MMGSDI status [%d]",
                      mmgsdi_status);
    }
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "ds3g_mmgsdi_refresh_fcn_event_process() "
                  "FCN number of files:%d pass_fail:%d",
                  fcn_indication.num_files,
                  (int)pass_fail);

  return;
} /* ds3g_mmgsdi_refresh_fcn_event_process */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_SIM_READ_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI SIM REDAD events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_sim_read_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_client_data_type        client_ref = 0;
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MDM_SYMBOL_NOT_USED(client_ref);

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id, &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Failed to find an activated "
                    "Session Id = [%d] from the table", session_id);
    return;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                "ds3g_mmgsdi_sim_read_event_process() session_id:%d "
                "session_type:%d client_ref:%d",
                  session_id, session_type,
                  mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info.client_ref);

  if (( session_type == MMGSDI_1X_PROV_PRI_SESSION ) ||
      ( session_type == MMGSDI_1X_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
      || ( session_type == MMGSDI_1X_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
     )
  {
    client_ref =
      (mmgsdi_client_data_type)
        mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info.client_ref;
  }
  else if (( session_type == MMGSDI_GW_PROV_PRI_SESSION ) ||
           ( session_type == MMGSDI_GW_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
           || ( session_type == MMGSDI_GW_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
          )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_sim_read_event_process with a UMTS session:%d",
                    session_type);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW, 
                    "Failed with session_id [%d], session type [%d]",
                    session_id, session_type);
  }
  return;
}/* ds3g_mmgsdi_sim_read_event_process */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_SIM_WRITE_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI SIM WRITE events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_sim_write_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;
  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id, &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Failed to find an Activated "
                    "Session Id = [%d] from the table", session_id);
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "ds3g_mmgsdi_sim_write_event_process() session_type:%d",
                  session_type);

  if (( session_type == MMGSDI_1X_PROV_PRI_SESSION ) ||
      ( session_type == MMGSDI_1X_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
      || ( session_type == MMGSDI_1X_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
     )
  {
    /* Check the status */
    if( mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info.cmd_status
        == MMGSDI_SUCCESS )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "3GPD Operational Capabilities Set on Card");
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
           "3GPD Operational Capabilities NOT SET ON CARD! status = [%d]",
           mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info.cmd_status);
    }
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
           "3GPD Operational Capabilities status:%d",
           mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info.cmd_status);
  }
  else if (( session_type == MMGSDI_GW_PROV_PRI_SESSION ) ||
           ( session_type == MMGSDI_GW_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
           || ( session_type == MMGSDI_GW_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
          )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_sim_write_event_process with UMTS session:%d",
                    session_type);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW, 
                    "Failed with session_id [%d], session type [%d]",
                    session_id, session_type);
  }
  return;
}/* ds3g_mmgsdi_sim_write_event_process */

/*===========================================================================
FUNCTION DS3G_MMGSDI_IMSI_RESP_CB()

DESCRIPTION
  Gets IMSI data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_imsi_resp_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_read_cnf_type         *read_cnf_ptr = NULL;
  ds_cmd_type                  *cmd_buf = NULL;
  ds3g_mmgsdi_imsi_info_type   *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == cnf_ptr )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_mmgsdi_get_imsi_cb: cnf_ptr NULL");

    return;
  }

  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);

  imsi_info_ptr = ds3g_mmgsdi_get_imsi_info_from_session_id
                    (cnf_ptr->read_cnf.response_header.session_id);
  if (imsi_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "IMSI info ptr is NULL ");
    return;
  }

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(read_cnf_ptr->response_header.client_id )
      != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                    "ds3g_mmgsdi_get_imsi_cb: Invalid client Id [%d] ",
                    read_cnf_ptr->response_header.client_id);

    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
       ( MMGSDI_READ_CNF == read_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS == read_cnf_ptr->response_header.mmgsdi_status ) )
    {
      /* Initialize the IMSI info table */
      memset( imsi_info_ptr->imsi_response,
              0,
              DS3G_MMGSDI_IMSI_LEN );
    /*-----------------------------------------------------------------------
      The reading from IMSI efs was successful.  This copies the data from
      the mmgsdi response
    -----------------------------------------------------------------------*/
      if ( read_cnf_ptr->read_data.data_len <= DS3G_MMGSDI_IMSI_LEN )
      {
        memscpy( imsi_info_ptr->imsi_response,
                 DS3G_MMGSDI_IMSI_LEN,
                 read_cnf_ptr->read_data.data_ptr,
                 read_cnf_ptr->read_data.data_len );
      }
      else
      {
        memscpy( imsi_info_ptr->imsi_response,
                 DS3G_MMGSDI_IMSI_LEN,
                 read_cnf_ptr->read_data.data_ptr,
                 DS3G_MMGSDI_IMSI_LEN );
      }

      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;

      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_IMSI_INFO_EVENT;
        // Session Id
      mmgsdi_event_info_ptr->session_id =
          cnf_ptr->read_cnf.response_header.session_id;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Command failure status [%d] and mmgsdi_status [%d] ",
                      status, read_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                    "Improper command confirmation cnf[%d] response_type[%d]",
                    cnf, read_cnf_ptr->response_header.response_type);
  }

  return;
} /* ds3g_mmgsdi_imsi_resp_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_IMPI_RESP_CB()

DESCRIPTION
  Gets IMPI data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_impi_resp_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_read_cnf_type         *read_cnf_ptr = NULL;
  ds3g_mmgsdi_impi_info_type   *impi_info_ptr = NULL;
  mmgsdi_slot_id_enum_type      slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == cnf_ptr )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_mmgsdi_impi_resp_cb: cnf_ptr NULL");

    return;
  }

  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);

  slot_id = cnf_ptr->read_cnf.response_header.slot_id;
  impi_info_ptr = ds3g_mmgsdi_get_impi_info_from_slot(slot_id);

  if (impi_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "IMPI info ptr is NULL ");
    return;
  }

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(read_cnf_ptr->response_header.client_id )
      != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                    "ds3g_mmgsdi_impi_resp_cb: Invalid client Id [%d] ",
                    read_cnf_ptr->response_header.client_id);
    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
       ( MMGSDI_READ_CNF == read_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS == read_cnf_ptr->response_header.mmgsdi_status ) )
    {

      if ( read_cnf_ptr->read_data.data_ptr == NULL ||
           read_cnf_ptr->read_data.data_ptr[0] != DS3G_MMGSDI_IMPI_TAG ) 
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                        "ds3g_mmgsdi_impi_resp_cb: data ptr is NULL or "
                        "invalid tag for impi, 0x%x ", 
                        read_cnf_ptr->read_data.data_ptr[0] );
        return;
      }

      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                      "ds3g_mmgsdi_impi_resp_cb: read data len [%d] ",
                      read_cnf_ptr->read_data.data_len);

      if ( ( read_cnf_ptr->read_data.data_len <= DS3G_MMGSDI_IMPI_DATA_OFFSET ) ||
           ( read_cnf_ptr->read_data.data_len < 
             (uint32)read_cnf_ptr->read_data.data_ptr[DS3G_MMGSDI_IMPI_LEN_OFFSET] + 
             DS3G_MMGSDI_IMPI_DATA_OFFSET ) ) 
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, 
                      "ds3g_mmgsdi_impi_resp_cb: invalid data len passed" );
        return;
      }

      /*-----------------------------------------------------------------------
        If IMPI info is already populated and read IMPI is called again, for
        REFRESH event, then clear & invalidate previous IMPI info.
      -----------------------------------------------------------------------*/
      if ( impi_info_ptr->impi_info.data_ptr != NULL )
      {
        modem_mem_free( impi_info_ptr->impi_info.data_ptr,
                        MODEM_MEM_CLIENT_DATA_CRIT );
        impi_info_ptr->impi_info.data_len = 0;
      }

      /*-----------------------------------------------------------------------
        IMPI raw data read from UIM is in the following format:
        "Tag"(80) + "Length" + "IMPI".
        If IMPI contains IMSI, it will be in ASCII format:
        "IMSI@domain".
        IMPI raw data example: "80   21  33 31 30 00 00 38 39 38 ...  "
                              (tag)(len) (IMPI)
        IMPI extracted example: 310008989513883@ims1.qualcomm.com
      -----------------------------------------------------------------------*/

      /*-----------------------------------------------------------------------
        Store IMPI data length
      -----------------------------------------------------------------------*/
      impi_info_ptr->impi_info.data_len = 
          read_cnf_ptr->read_data.data_ptr[DS3G_MMGSDI_IMPI_LEN_OFFSET];

      /* Allocate & Initialize the IMPI info data */
      impi_info_ptr->impi_info.data_ptr =
        modem_mem_alloc( impi_info_ptr->impi_info.data_len,
                         MODEM_MEM_CLIENT_DATA_CRIT );

      if ( impi_info_ptr->impi_info.data_ptr == NULL )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                     "Could not allocate memory for IMPI info data");
        return;
      }

      DATA_3GMGR_MSG5(MSG_LEGACY_LOW,
                  "IMPI read raw data: [0x%x], [0x%x], [0x%x], [0x%x], [0x%x]", 
                  read_cnf_ptr->read_data.data_ptr[0],
                  read_cnf_ptr->read_data.data_ptr[1],
                  read_cnf_ptr->read_data.data_ptr[2],
                  read_cnf_ptr->read_data.data_ptr[3],
                  read_cnf_ptr->read_data.data_ptr[4] );

      memset( impi_info_ptr->impi_info.data_ptr,
              0,
              impi_info_ptr->impi_info.data_len);
     /*-----------------------------------------------------------------------
       The reading from IMPI efs was successful. This copies the data from
       the mmgsdi response
     -----------------------------------------------------------------------*/
      memscpy( impi_info_ptr->impi_info.data_ptr,
               impi_info_ptr->impi_info.data_len,
               &(read_cnf_ptr->read_data.data_ptr[DS3G_MMGSDI_IMPI_DATA_OFFSET]),
               impi_info_ptr->impi_info.data_len );

      /* Extract IMSI content from IMPI */
      ds3g_mmgsdi_format_imsi_from_impi(slot_id);

      /* Set Ready to read for clients*/
      impi_info_ptr->ready_status = TRUE;

      subs_id = ds3g_mmgsdi_get_sub_id_from_slot(slot_id);
      if ( ds3gsubsmgr_is_subs_id_valid(subs_id))
      {
        ds3g_mmgsdi_notify_impi_info_ev(subs_id,impi_info_ptr);
      }
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "Command failure status [%d] and mmgsdi_status [%d] ",
                      status, read_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Improper command confirmation cnf[%d] response_type[%d]",
                    cnf, read_cnf_ptr->response_header.response_type);
  }

  return;
} /* ds3g_mmgsdi_impi_resp_cb */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_IMSI_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_IMSI_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_imsi_info_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_access_type             imsi_access;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id, &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an Activated Session Id %d from the table",
                    session_id);
    return;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Read AD file with Session Type [%d], Session ID [%d]",
                  session_type, session_id);

  /* Get AD from USIM */
  imsi_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  imsi_access.file.file_enum = MMGSDI_USIM_AD;

  /* Read the Administrative Data Elementary File */
  mmgsdi_status = ds3g_mmgsdi_read_transparent( imsi_access,
                                                DS3G_MMGSDI_MNC_OFFSET,
                                                DS3G_MMGSDI_MNC_LEN,    // 1 Byte
                                                ds3g_mmgsdi_mnc_resp_cb,
                                                session_type );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Failed to read IMSI using "
                    "MMGSDI_GW_PROV_PRI_SESSION, status [%d]", mmgsdi_status);
    return;
  }

  return;
}/* ds3g_mmgsdi_imsi_info_event_process */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_MNC_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_MNC_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_mnc_info_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  ds3g_mmgsdi_imsi_info_type    *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  mmgsdi_app_enum_type           app_type = MMGSDI_APP_NONE;

#if defined (FEATURE_DATA_REL10) || defined (FEATURE_DATA_RPM)
  ds_cmd_type                   *cmd_buf = NULL;
#endif /*FEATURE_DATA_REL10 || FEATURE_DATA_RPM*/
  sys_modem_as_id_e_type        *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  subs_id = ds3g_mmgsdi_get_sub_id_from_session_id(session_id);
  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id, &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Failed to find an Activated "
                    "Session Id = [%d] from the table", session_id);
    return;
  }

  imsi_info_ptr = ds3g_mmgsdi_get_imsi_info_from_session_id (session_id);
  if (imsi_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "IMSI info ptr is NULL");
    return;
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "Format IMSI info using Session Type [%d], Session ID [%d]",
                  session_type, session_id);


  /* Convert IMSI content from EFS format to NV format */
  ds3g_mmgsdi_format_imsi(session_id);

  /* Set Ready to read for clients*/
  imsi_info_ptr->ready_status = TRUE;

  app_type = ds3g_mmgsdi_get_app_type_from_session_id(session_id);
  ds3g_mmgsdi_notify_imsi_info_ev(subs_id,app_type,imsi_info_ptr);

  cmd_buf = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
  if( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_buf->hdr.cmd_id = DS_CMD_3GPP_PDN_THROT_IMSI_INFO_AVAIL;
  data_ptr = (sys_modem_as_id_e_type*)cmd_buf->cmd_payload_ptr;

  *data_ptr = subs_id;

  ds_put_cmd(cmd_buf);
}/* ds3g_mmgsdi_mnc_info_event_process */

/*===========================================================================
FUNCTION DS3G_MMGSDI_MNC_RESP_CB()

DESCRIPTION
  Gets MNC data length from the mmgsdi response, and copies into variables
  local to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_mnc_resp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_read_cnf_type         *read_cnf_ptr = NULL;
  ds_cmd_type                  *cmd_buf;
  ds3g_mmgsdi_imsi_info_type   *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == cnf_ptr )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_mmgsdi_mnc_resp_cb: cnf_ptr NULL");
    return;
  }

  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);

  imsi_info_ptr = ds3g_mmgsdi_get_imsi_info_from_session_id
                    (read_cnf_ptr->response_header.session_id);
  if (imsi_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "IMSI info ptr is NULL ");
    return;
  }

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
    read_cnf_ptr->response_header.client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_mnc_resp_cb: Invalid client Id [%d] ",
                    read_cnf_ptr->response_header.client_id);

    return;
  }
  if ( ( MMGSDI_READ_CNF == cnf ) &&
       ( MMGSDI_READ_CNF ==
         read_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS == read_cnf_ptr->response_header.mmgsdi_status )
       )
    {
      /* Initialize the MNC info table */
      memset( imsi_info_ptr->mnc_response,
              0,
              DS3G_MMGSDI_MNC_LEN );
    /*-----------------------------------------------------------------------
      The reading from AD efs was successful.  This copies the data from
      the mmgsdi response
    -----------------------------------------------------------------------*/
      if ( read_cnf_ptr->read_data.data_len <=
           DS3G_MMGSDI_MNC_LEN )
      {
        memscpy( imsi_info_ptr->mnc_response,
                 DS3G_MMGSDI_MNC_LEN,
                 read_cnf_ptr->read_data.data_ptr,
                 read_cnf_ptr->read_data.data_len );
      }
      else
      {
        memscpy( imsi_info_ptr->mnc_response,
                 DS3G_MMGSDI_MNC_LEN,
                 read_cnf_ptr->read_data.data_ptr,
                 DS3G_MMGSDI_MNC_LEN );
      }

      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_MNC_INFO_EVENT;

      // Session Id
      mmgsdi_event_info_ptr->session_id
        = cnf_ptr->read_cnf.response_header.session_id;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Command failure status [%d] and mmgsdi_status [%d]",
                      status, read_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                    "Improper command confirmation cnf [%d] response_type [%d]",
                    cnf, read_cnf_ptr->response_header.response_type);
  }
  return;
} /* ds3g_mmgsdi_mnc_resp_cb */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_IMSI

DESCRIPTION
  Get IMSI info afer receiving Subscription Ready Events from GW sessions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_read_imsi
(
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_access_type       imsi_access;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return FALSE;
  }

  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Sending Request for Reading IMSI");

  /* Get IMSI from USIM */
  imsi_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  imsi_access.file.file_enum = MMGSDI_USIM_IMSI;

  /* Read the data */
  mmgsdi_status = ds3g_mmgsdi_read_transparent( imsi_access,
                                                DS3G_MMGSDI_IMSI_OFFSET,
                                                DS3G_MMGSDI_IMSI_LEN,    // 9 Bytes
                                                ds3g_mmgsdi_imsi_resp_cb,
                                                session_type );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Failed to read IMSI using session type [%d], status [%d]",
                    session_type, mmgsdi_status);
    return FALSE;
  }

  return TRUE;
}/* ds3g_mmgsdi_read_imsi */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_IMPI

DESCRIPTION
  Get IMPI info afer opening non_provision_isim_session

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_read_impi
(
  mmgsdi_session_type_enum_type  non_prov_session_type
)
{
  mmgsdi_access_type       impi_access;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type   session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return FALSE;
  }

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Sending Request for Reading IMPI");

  /* Get IMPI from ISIM */
  impi_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  impi_access.file.file_enum = MMGSDI_ISIM_IMPI;

   /* check session id */
  if ( ds3g_mmgsdi_get_non_prov_isim_session_id( non_prov_session_type, 
                                                 &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
            "Failed to get non prov Session Id using non prov Session Type [%d] ",
             non_prov_session_type);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_session_read_transparent( session_id,
                                                   impi_access,
                                                   0,
                                                   0,
                                                   ds3g_mmgsdi_impi_resp_cb,
                                                   ( mmgsdi_client_data_type ) 0);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                    "Failed to read IMPI using session type [%d], status [%d]",
                    non_prov_session_type, mmgsdi_status);
    return FALSE;
  }

  return TRUE;
}/* ds3g_mmgsdi_read_impi */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_EFACC

DESCRIPTION
  Read EF ACC info during Subscription Ready/Refresh Events for GW sessions

DEPENDENCIES
  None.

PARAMETERS
  mmgsdi_session_type_enum_type session_type

RETURN VALUE
  TRUE-if the read is successful.
  FALSE-otherwise.

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_read_efacc
(
  mmgsdi_session_type_enum_type  session_type
)
{
   mmgsdi_session_id_type  session_id;
   mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
   mmgsdi_access_type      ef_access;
   mmgsdi_app_enum_type    app_type;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(session_type == MMGSDI_MAX_SESSION_TYPE_ENUM)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid session_type [%d]",
                    session_type);
    return FALSE;
  }

  session_id = ds3g_mmgsdi_get_session_id_from_session_type(session_type);
  app_type   = ds3g_mmgsdi_get_app_type_from_session_id(session_id);

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,"App type[%d] Session id =[%d] from the table",
                  app_type, session_id);

  if(MMGSDI_APP_USIM == app_type)
  {
    ef_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    ef_access.file.file_enum = MMGSDI_USIM_ACC;
  }
  else if(MMGSDI_APP_SIM == app_type)
  {
    ef_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    ef_access.file.file_enum = MMGSDI_GSM_ACC;
  }
  else
  {
    /* Ef acc is not valid for non-GW session type .. ignoring */
    return FALSE;
  }

  mmgsdi_status = ds3g_mmgsdi_read_transparent(
                          ef_access,
                          0,// Read from start of the file.
                          0,// 0 for entire file.
                          ds3g_mmgsdi_efacc_read_resp_hdlr_cb,
                          session_type);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Failed to read EFACC using session id [%d], status [%d]",
                    session_id,
                    mmgsdi_status);
    return FALSE;
  }
  return TRUE;
}/* ds3g_mmgsdi_read_efacc */

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_READ_RESP_HDLR_CB

DESCRIPTION
  Process EFACC data from the mmgsdi response, and posts a cmd to dstask for read
  confirmation.This is a callback function called from the mmgsdi task
  context.

PARAMETERS
  mmgsdi_return_enum_type   status
  mmgsdi_cnf_enum_type      cnf
  const mmgsdi_cnf_type     *cnf_ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_efacc_read_resp_hdlr_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type                  *cmd_buf = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
  ds_at_mmgsdi_sim_info_type   *cmd_rd_ptr  = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (NULL == cnf_ptr) || (status != MMGSDI_SUCCESS))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_efacc_read_resp_hdlr_cb:cnf_ptr NULL");
    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
       ( MMGSDI_READ_CNF == cnf_ptr->read_cnf.response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS == cnf_ptr->read_cnf.response_header.mmgsdi_status ) )
    {
      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr =
        (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;

      cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_EFACC_READ_CNF_EVENT;
      /* Session ID */
      cmd_rd_ptr->session_id = cnf_ptr->read_cnf.response_header.session_id;
      /* Copy read_data */
      cmd_rd_ptr->data_len = cnf_ptr->read_cnf.read_data.data_len;

      memscpy (cmd_rd_ptr->data,
               sizeof(cmd_rd_ptr->data),
               cnf_ptr->read_cnf.read_data.data_ptr,
               (uint32)cmd_rd_ptr->data_len);
      ds_put_cmd(cmd_buf);
    }
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR, "Improper command confirmation cnf [%d] "
                    "response_type [%d]",
                    cnf,
                    cnf_ptr->read_cnf.response_header.response_type);
  }
}/* ds3g_mmgsdi_efacc_read_resp_hdlr_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_UPDATE_CACHE

DESCRIPTION
  This function updates the cache with the values read from SIM for the corresponding
  session type.

PARAMETERS
  const ds_cmd_type   *cmd_ptr

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_mmgsdi_efacc_update_cache
(
  const ds_cmd_type   *cmd_ptr
)
{
  ds_at_mmgsdi_sim_info_type    *cmd_rd_ptr = NULL;
  mmgsdi_session_id_type        session_id;
  mmgsdi_session_type_enum_type session_type;
  uint8                         index = 0;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  mmgsdi_event_info_ptr = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  cmd_rd_ptr = (ds_at_mmgsdi_sim_info_type *)
                   &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);

  if(NULL == cmd_rd_ptr)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_efacc_update_cache(): NULL CMD_RD_PTR");
    return;
  }

  session_id = cmd_rd_ptr->session_id;
  session_type = ds3g_mmgsdi_get_session_type_from_session_id(session_id);

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      index = 0;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      index = 1;
      break;
#ifdef FEATURE_TRIPLE_SIM
    case MMGSDI_GW_PROV_TER_SESSION:
      index = 2;
      break;
#endif /* FEATURE_TRIPLE_SIM */
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                     "Error: Invalid session type: 0x%x", session_type);
      return;
  }

  memscpy(&ds3g_mmgsdi_efacc_class_value[index],
          sizeof(uint32),
          cmd_rd_ptr->data,
          (uint32)cmd_rd_ptr->data_len);

}/* ds3g_mmgsdi_efacc_update_cache */

/*===========================================================================
FUNCTION DS3G_MMGSDI_RESET_EFACC_VALUE

DESCRIPTION
  This function is called when  DS3G_MMGSDI_CARD_ERROR_EVENT is posted.
   It reset the efacc values to zero for the corresponding GW session.

PARAMETERS
  mmgsdi_session_type_enum_type session_type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_reset_efacc_value
(
  mmgsdi_session_type_enum_type  session_type
)
{
  uint8 index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
     index = 0;
     break;
    case MMGSDI_GW_PROV_SEC_SESSION:
     index = 1;
     break;
#ifdef FEATURE_TRIPLE_SIM
    case MMGSDI_GW_PROV_TER_SESSION:
     index = 2;
     break;
#endif /* FEATURE_TRIPLE_SIM */
    default:
      /* Ignore for invalid or non-GW sessions */
      return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                        "Resetting EFACC value for index[%d]",index);
  ds3g_mmgsdi_efacc_class_value[index] = 0;
}/* ds3g_mmgsdi_reset_efacc_value */

/*===========================================================================
  FUNCTION DS3G_MMGSDI_IS_EFACC_CLASS_SET

  DESCRIPTION
   Returns true if any of the bits from b4-b8 is set in the byte 1
   i.e 11 -15 as shown below.

   Byte 1:
   b8 b7 b6 b5 b4 b3 b2 b1
   15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)

   if any of the bits from 11-15 are enabled then EFACC flag is set to true.
   Spec reference: TS 3GPP: 31.102
   Byte 2:
   b8 b7 b6 b5 b4 b3 b2 b1
   07 06 05 04 03 02 01 00 Number of the ACC

  PARAMETERS
    sys_modem_as_id_e_type subs_id.

  DEPENDENCIES
    None.

  RETURN VALUE
    TRUE  - if bits b4-b8 are set in the byte 1 of ACC
    FALSE - otherwise

  SIDE EFFECTS
    None.
============================================================================*/
boolean ds3g_mmgsdi_is_efacc_class_set
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint8 index =0;
  mmgsdi_session_type_enum_type session_type;
  mmgsdi_session_id_type        session_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* -------------------------------------------------------------------
   Byte 1:
   b8 b7 b6 b5 b4 b3 b2 b1
   15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)

   Byte 2:
   b8 b7 b6 b5 b4 b3 b2 b1
   07 06 05 04 03 02 01 00 Number of the ACC
  -----------------------------------------------------------------------*/
  if(FALSE == ds3g_mmgsdi_get_session_id_from_sub_id(subs_id, &session_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Error: Invalid session id: 0x%x", session_id);
    return FALSE;
  }

  session_type = ds3g_mmgsdi_get_session_type_from_session_id(session_id);
  if(session_type == MMGSDI_MAX_SESSION_TYPE_ENUM )
  {
    DS_3GPP_MSG1_ERROR("Invalid Session type:%d", session_type);
    return FALSE;
  }

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      index = 0;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      index = 1;
      break;
#ifdef FEATURE_TRIPLE_SIM
    case MMGSDI_GW_PROV_TER_SESSION:
      index = 2;
      break;
#endif /* FEATURE_TRIPLE_SIM */
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Error: Invalid session type: 0x%x", session_type);
       return FALSE;
  }
  if((ds3g_mmgsdi_efacc_class_value[index] & ACC_CLASS_11_15_MASK) != 0)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "efacc class is enabled for session type [%d]",
                    session_type);
    return TRUE;
  }
  return FALSE;
}/* ds3g_mmgsdi_is_efacc_class_set */


/*===========================================================================
  FUNCTION DS3G_MMGSDI_GET_EFACC_CLASS
  
  DESCRIPTION
   Returns true if any of the bits from b4-b8 is set in the byte 1 
   i.e 11 -15 as shown below. 
  
   Byte 1:
   b8 b7 b6 b5 b4 b3 b2 b1
   15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)
   
   if any of the bits from 11-15 are enabled then EFACC flag is set to true.
   Spec reference: TS 3GPP: 31.102
   Byte 2:
   b8 b7 b6 b5 b4 b3 b2 b1
   07 06 05 04 03 02 01 00 Number of the ACC
 
   In addition this returns the EFACC CLASS value
  
  PARAMETERS
    sys_modem_as_id_e_type subs_id.
    Pointer to retrieve EFACC class
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    TRUE  - if bits b4-b8 are set in the byte 1 of ACC
    FALSE - otherwise
  
  SIDE EFFECTS
    None.
============================================================================*/
boolean ds3g_mmgsdi_get_efacc_class
(
  sys_modem_as_id_e_type subs_id,
  uint32                *efacc_class
)
{
  uint8 index =0;
  mmgsdi_session_type_enum_type session_type;
  mmgsdi_session_id_type        session_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if (efacc_class == NULL)
  {
    DS_3GPP_MSG0_ERROR("EFACC CLASS PTR is NULL");
    return FALSE;
  }

  *efacc_class = 0;
  /* -------------------------------------------------------------------
   Byte 1:
   b8 b7 b6 b5 b4 b3 b2 b1
   15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)

   Byte 2:
   b8 b7 b6 b5 b4 b3 b2 b1
   07 06 05 04 03 02 01 00 Number of the ACC
  -----------------------------------------------------------------------*/
  if(FALSE == ds3g_mmgsdi_get_session_id_from_sub_id(subs_id,&session_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Error: Invalid session id: 0x%x", session_id);
    return FALSE;
  }
  session_type = ds3g_mmgsdi_get_session_type_from_session_id(session_id);

  if(session_type == MMGSDI_MAX_SESSION_TYPE_ENUM )
   {
     DS_3GPP_MSG0_ERROR("Invalid Session type (MMGSDI_MAX_SESSION_TYPE_ENUM)");
     return FALSE;
   }

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      index = 0;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      index = 1;
      break;
#ifdef FEATURE_TRIPLE_SIM
    case MMGSDI_GW_PROV_TER_SESSION:
      index = 2;
      break;
#endif /* FEATURE_TRIPLE_SIM */
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Error: Invalid session type: 0x%x", session_type);
       return FALSE;
  }
  if((ds3g_mmgsdi_efacc_class_value[index] & ACC_CLASS_11_15_MASK) != 0) 
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "efacc class is enabled for session type [%d]",
                                    session_type);
    *efacc_class = ds3g_mmgsdi_efacc_class_value[index];
    return TRUE;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                  "EFACC Class is %d", *efacc_class);
  return FALSE;
}/* ds3g_mmgsdi_get_efacc_class */

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers for refresh on the EFacc files on SIM/USIM MMGSDI
  interface.

PARAMETERS
  mmgsdi_session_type_enum_type session_type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_efacc_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type refresh_files = {0};
  mmgsdi_file_enum_type         file_list;
  mmgsdi_app_enum_type          app_type;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(session_type == MMGSDI_MAX_SESSION_TYPE_ENUM)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Invalid session_type passed %d", session_type);
    return;
  }
  if ( ds3g_mmgsdi_get_app_type( session_type, &app_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an activated Session Type = [%d]",
                    session_type);
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "app_type:%d", app_type);
  if ( app_type == MMGSDI_APP_USIM )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Registers UMTS file change from USIM");
    file_list = MMGSDI_USIM_ACC; /* EF for Access  Control Class */
    refresh_files.num_files = 1;
  }
  else if(app_type == MMGSDI_APP_SIM)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Registers UMTS file change from USIM");
    file_list = MMGSDI_GSM_ACC; /* EF for Access Control Class */
    refresh_files.num_files = 1;
  }
  else
  {
    return;
  }
  refresh_files.file_list_ptr = &file_list ;
  mmgsdi_status = ds3g_mmgsdi_register_for_refresh( refresh_files,
                                                    session_type );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Failed to Request Refresh FCN Registration");
  }
}/* ds3g_mmgsdi_efacc_register_for_refresh */

/*===========================================================================
FUNCTION DS3G_MMGSDI_FORMAT_IMSI()

DESCRIPTION
  Based on content getting from MMGSDI_USIM_AD and MMGSDI_USIM_IMSI, convert
  content into nv_ehrpd_imsi_type for any client to query this info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_format_imsi
(
  mmgsdi_session_id_type session_id
)
{
  byte      digit                 = 0;
  uint8     num_mnc_digits        = 0;
  uint8     index                 = 0;
  uint8     step                  = 0;
  uint8     offset                = 0;
  uint8     len                   = 0;
  uint16    mcc_value             = 0;
  boolean   found                 = FALSE;
  uint16    mcc_pcs1900_na_list[] = { 302, 310, 311, 312, 313,
                                      314, 315, 316, 334, 348 };
  ds3g_mmgsdi_imsi_info_type* imsi_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  imsi_info_ptr = ds3g_mmgsdi_get_imsi_info_from_session_id (session_id);
  if (imsi_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "IMSI info ptr is NULL ");
    return;
  }

  /* Old SIM cards had only 2 digits MNC and EF-AD with 3 bytes only.
     Default to 2 digits to keep backward compatibility */
  num_mnc_digits = DS3G_MMGSDI_MNC_2_DIGITS;

  /* 3GPP 23.122 Annex 'A'
     For PCS1900 for North America, regulations mandate that a 3-digit MNC
     shall be used; however during a transition period, a 2 digit MNC may
     be broadcast by the Network and, in this case, the 3rd digit of the
     SIM is stored as 0 (this is the 0 suffix rule).
     For these specific country codes, MNC length shall default to 3. */
  mcc_value  = (uint16)((imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_1_OFFSET] >> 4) &
                DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK) * 100;
  mcc_value += (uint16)((imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_2_OFFSET]) &
                DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK) * 10;
  mcc_value += (uint16)((imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_2_OFFSET] >> 4) &
                DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK);


  /* Search mcc from the table to see if matched */
  for ( index = 0, found = FALSE;
        index < sizeof(mcc_pcs1900_na_list) / sizeof(uint16);
        index++ )
  {
    if (mcc_value == mcc_pcs1900_na_list[index])
    {
      num_mnc_digits = DS3G_MMGSDI_MNC_3_DIGITS;
      found = TRUE;
      break;
    }
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_MED, "MCC value [%d], Found flag [%d]", 
                  mcc_value, found);

  /* if not found in North America table, we shall get
   * the length from mnc_response which is read from
   * AD table
   */
  if ( found == FALSE )
  {
    if ( (imsi_info_ptr->mnc_response[0] == DS3G_MMGSDI_MNC_2_DIGITS ) ||
         (imsi_info_ptr->mnc_response[0] == DS3G_MMGSDI_MNC_3_DIGITS ) )
    {
      num_mnc_digits = imsi_info_ptr->mnc_response[0];
    }
    else
  {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "EF-AD has invalid MNC length: 0x%x",
                      imsi_info_ptr->mnc_response[0]);
    }
  }


  /* --------------------------------------------------------------------------
     Now format the IMSI from:
     B1       B2       B3       B4     B5   B6   B7    B8
     MCC1PB   MCC3MCC2 MNC2MNC1 D1MNC3 D3D2 D5D4 D7D6 D9D8

     ------------------------------------------------------------------------*/
  /* Get MCC 3 digits */
  imsi_info_ptr->imsi_info.ehrpd_mcc[0] =
    ( imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_1_OFFSET] &
      DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK ) >> 4;
  imsi_info_ptr->imsi_info.ehrpd_mcc[1] =
     imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_2_OFFSET] &
     DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK;
  imsi_info_ptr->imsi_info.ehrpd_mcc[2] =
    ( imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_3_OFFSET] &
      DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK ) >> 4;
  imsi_info_ptr->imsi_info.num_of_mcc_digits = 3;

  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "MCC Digit Len [%d]", 
                  imsi_info_ptr->imsi_info.num_of_mcc_digits);

  /* Get MNC digits */
  imsi_info_ptr->imsi_info.num_of_mnc_digits = num_mnc_digits;

  imsi_info_ptr->imsi_info.ehrpd_mnc[0] =
     imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_4_OFFSET] &
     DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK;
  imsi_info_ptr->imsi_info.ehrpd_mnc[1] =
    ( imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_5_OFFSET] &
      DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK ) >> 4;

  DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                  "MNC Digits Len [%d], Digit1 [0x%x], Digit2[0x%x]", 
                  imsi_info_ptr->imsi_info.num_of_mnc_digits,
                  imsi_info_ptr->imsi_info.ehrpd_mnc[0],
                  imsi_info_ptr->imsi_info.ehrpd_mnc[1] );

  if ( num_mnc_digits == DS3G_MMGSDI_MNC_3_DIGITS )
  {
    imsi_info_ptr->imsi_info.ehrpd_mnc[2] =
      imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_6_OFFSET] &
      DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK;

    imsi_info_ptr->imsi_info.ehrpd_msin[0] =
      ( imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_7_OFFSET] &
        DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK ) >> 4;

    /* Set offset for next loop */
    offset = 1;

    DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                    "MNC Digit3 [0x%x], MSIN Digit1 [0x%x], offset [%d]",
                    imsi_info_ptr->imsi_info.ehrpd_mnc[2],
                    imsi_info_ptr->imsi_info.ehrpd_msin[0],
                    offset );
    // Max length(MCC+MNC+MSIN) is 15.
    // MCC len is always 3. IF MNC len is 3, then adjust length
    len = MAX_MSIN_DIGITS - offset -1;
  }
  else
  {
    imsi_info_ptr->imsi_info.ehrpd_msin[0] =
      imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_6_OFFSET] &
      DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK;

    imsi_info_ptr->imsi_info.ehrpd_msin[1] =
      ( imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_7_OFFSET] &
        DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK ) >> 4;

    /* Set offset for next loop */
    offset = 2;

    DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                    "MSIN Digit1 [0x%x], Digit2 [0x%x], Offset [%d]",
                    imsi_info_ptr->imsi_info.ehrpd_msin[0],
                    imsi_info_ptr->imsi_info.ehrpd_msin[1],
                    offset );
    // Max length(MCC+MNC+MSIN) is 15.
    // MCC len is always 3. IF MNC len is 2, then adjust length
    len = MAX_MSIN_DIGITS - offset;
  }

  /* copy MSIN digits */
  for (index = 0, step = 0; index < len; index++, step++ )
  {
    if( DS3G_MMGSDI_IMSI_DIGIT_8_OFFSET + step >= DS3G_MMGSDI_IMSI_LEN )
    {
      break;
    }

    digit = imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_8_OFFSET+step] &
            DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK;
    if ( digit == DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK )
    {
      /* encounter unused nibble 'F'*/
      break;
    }
    imsi_info_ptr->imsi_info.ehrpd_msin[offset+index] = digit;
    DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                    "MSIN Digit [0x%x], index [%d], step [%d]",
                    digit, index, step );

    index++;
    if ( index == len )
    {
      // if it reaches to Max digits, we go out of the loop
      break;
    }

    digit = ( imsi_info_ptr->imsi_response[DS3G_MMGSDI_IMSI_DIGIT_8_OFFSET+step] &
              DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK ) >> 4;

    if ( digit == DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK )
    {
      /* encounter unused nibble 'F'*/
      break;
  }
    imsi_info_ptr->imsi_info.ehrpd_msin[offset+index] = digit;

    DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                    "MSIN Digit [0x%x], index [%d], step [%d]",
                    digit, index, step );

  }
  imsi_info_ptr->imsi_info.num_of_msin_digits = offset+index;
  DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                  "num_of_msin_digits [0x%d], index [%d], offset [%d]",
                  imsi_info_ptr->imsi_info.num_of_msin_digits,
                  index,
                  offset );

} /* ds3g_mmgsdi_format_imsi */

/*===========================================================================
FUNCTION DS3G_MMGSDI_CHECK_IF_IMPI_CONTAIN_IMSI()

DESCRIPTION
  Check the content before '@' in IMPI to see whether it satisfies IMSI format

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_check_if_impi_contain_imsi
(
  ds3g_mmgsdi_impi_info_type   *impi_info_ptr,
  uint8                        *imsi_len_ptr
)
{
  uint32                        idx      = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_mmgsdi_check_if_impi_contain_imsi");

  if ( impi_info_ptr == NULL || imsi_len_ptr == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_mmgsdi_check_if_impi_contain_imsi,"
                                     "input ptr is NULL");
    return FALSE;
  }

  while ( idx < impi_info_ptr->impi_info.data_len &&
          impi_info_ptr->impi_info.data_ptr[idx] != '@')
  {
    if ( impi_info_ptr->impi_info.data_ptr[idx] - '0' < 0 ||
         impi_info_ptr->impi_info.data_ptr[idx] - '0' > 9 ) 
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3g_mmgsdi_check_if_impi_contain_imsi, "
                                       "impi data is not a digit, idx is %d", idx);
      return FALSE;
    }
    idx++; 
  }

  if ( idx >= impi_info_ptr->impi_info.data_len ||
       idx > DS3G_MMGSDI_MAX_IMSI_LEN ||
       idx < DS3G_MMGSDI_MIN_IMSI_LEN ) 
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, 
                   "ds3g_mmgsdi_check_if_impi_contain_imsi, invalid imsi, idx is %d",
                   idx );
    return FALSE;
  }

  *imsi_len_ptr = idx;
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                  "ds3g_mmgsdi_check_if_impi_contain_imsi, imsi_len is %d",
                  *imsi_len_ptr );

  return TRUE;
} /* ds3g_mmgsdi_check_if_impi_contain_imsi */

/*===========================================================================
FUNCTION DS3G_MMGSDI_FORMAT_IMSI_FROM_IMPI()

DESCRIPTION
  Extract the IMSI content and convert into nv_ehrpd_imsi_type from IMPI info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_format_imsi_from_impi
(
  mmgsdi_slot_id_enum_type     slot_id
)
{
  uint8                        num_mnc_digits = 0;
  uint8                        offset         = 0;
  uint8                        imsi_len       = 0;
  int                          i              = 0;
  ds3g_mmgsdi_impi_info_type  *impi_info_ptr  = NULL;
  sys_modem_as_id_e_type       subs_id        = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, 
                  "ds3g_mmgsdi_format_imsi_from_impi" );

  impi_info_ptr = ds3g_mmgsdi_get_impi_info_from_slot(slot_id);

  if (!ds3g_mmgsdi_check_if_impi_contain_imsi(impi_info_ptr, &imsi_len))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, 
                  "ds3g_mmgsdi_format_imsi_from_impi, impi does not contain imsi" );
    return;
  }

  impi_info_ptr->imsi_info.num_of_mcc_digits = DS3G_MMGSDI_MCC_DIGITS;
  for (i = 0; i < DS3G_MMGSDI_MCC_DIGITS; i++)
  {
    impi_info_ptr->imsi_info.ehrpd_mcc[i] = 
        impi_info_ptr->impi_info.data_ptr[i]-'0';
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "MCC Digit1 [0x%x], Digit2[0x%x], Digit3[0x%x]", 
                  impi_info_ptr->imsi_info.ehrpd_mcc[0],
                  impi_info_ptr->imsi_info.ehrpd_mcc[1],
                  impi_info_ptr->imsi_info.ehrpd_mcc[2] );

  /* Read number of mnc digits from mnc_len NV. If the NV is not configured,
     default number of mnc digits is 3. */

  subs_id = ds3g_mmgsdi_get_sub_id_from_slot(slot_id);

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  num_mnc_digits = ds_3gpp_nv_manager_get_mnc_len(subs_id);

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, 
              "ds3g_mmgsdi_format_imsi_from_impi, "
              "num_mnc_digits is %d", num_mnc_digits );

  if (num_mnc_digits + DS3G_MMGSDI_MCC_DIGITS > imsi_len)
  {
    return;
  }

  impi_info_ptr->imsi_info.num_of_mnc_digits = num_mnc_digits; 
  for (i = 0; i < num_mnc_digits; i++)
  {
    impi_info_ptr->imsi_info.ehrpd_mnc[i] = 
        impi_info_ptr->impi_info.data_ptr[i+DS3G_MMGSDI_MCC_DIGITS]-'0';
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "MNC Digits Len [%d], Digit1 [0x%x], Digit2[0x%x]", 
                  impi_info_ptr->imsi_info.num_of_mnc_digits,
                  impi_info_ptr->imsi_info.ehrpd_mnc[0],
                  impi_info_ptr->imsi_info.ehrpd_mnc[1] );

  offset = DS3G_MMGSDI_MCC_DIGITS + num_mnc_digits;
  impi_info_ptr->imsi_info.num_of_msin_digits = imsi_len - offset;

  for (i=0; i<impi_info_ptr->imsi_info.num_of_msin_digits; i++) 
  {
    impi_info_ptr->imsi_info.ehrpd_msin[i] =
        impi_info_ptr->impi_info.data_ptr[i+offset]-'0'; 
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "MSIN Digits Len [%d], offset [%d]",
                  impi_info_ptr->imsi_info.num_of_msin_digits,
                  offset );

} /* ds3g_mmgsdi_format_imsi_from_impi */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMSI_DATA()

DESCRIPTION
  Passed cached IMSI data to client

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMSI data
  TRUE: Read IMSI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_imsi_data
(
  nv_ehrpd_imsi_type* imsi_data_ptr
)
{
  return  ds3g_mmgsdi_get_imsi_data_per_subs_id( imsi_data_ptr,
                                                 ds3g_get_ps_subs_id());
}

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMSI_DATA_PER_SUBS_ID()

DESCRIPTION
  Passed cached IMSI data to client for passed subs id

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMSI data
  TRUE: Read IMSI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_imsi_data_per_subs_id
(
  nv_ehrpd_imsi_type*    imsi_data_ptr,
  sys_modem_as_id_e_type subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }

  if ( imsi_data_ptr == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_get_imsi_data: imsi_data_ptr NULL");
    return FALSE;
  }
  else
  {
    if ( ds3g_mmgsdi_imsi_info[subs_id].ready_status == TRUE )
    {
      memscpy( (void*)imsi_data_ptr,
               sizeof(nv_ehrpd_imsi_type),
               (void*)&ds3g_mmgsdi_imsi_info[subs_id].imsi_info,
               sizeof(nv_ehrpd_imsi_type) );
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "IMSI data is not ready for querying");
      return FALSE;
    }
  }
  return TRUE;
} /* ds3g_mmgsdi_get_imsi_data_per_subs_id */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMPI_DATA()

DESCRIPTION
  Passed cached IMPI data to client

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMPI data
  TRUE: Read IMPI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_impi_data
(
  mmgsdi_data_type       *impi_data_ptr,
  nv_ehrpd_imsi_type     *imsi_data_ptr
)
{
  return  ds3g_mmgsdi_get_impi_data_per_subs_id( ds3g_get_ps_subs_id(),
                                                 impi_data_ptr,
                                                 imsi_data_ptr );
}

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMPI_DATA_PER_SUBS_ID()

DESCRIPTION
  Passed cached IMPI data to client for passed subs id

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMPI data
  TRUE: Read IMPI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_impi_data_per_subs_id
(
  sys_modem_as_id_e_type  subs_id,
  mmgsdi_data_type       *impi_data_ptr,
  nv_ehrpd_imsi_type     *imsi_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }

  if ( impi_data_ptr == NULL || imsi_data_ptr == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_get_impi_data: impi_data_ptr or "
                    "imsi_data_ptr is NULL");
    return FALSE;
  }
  else
  {
    if ( ds3g_mmgsdi_impi_info[subs_id].ready_status == TRUE )
    {
      memscpy( (void*)impi_data_ptr,
               sizeof(mmgsdi_data_type),
               (void*)&ds3g_mmgsdi_impi_info[subs_id].impi_info,
               sizeof(mmgsdi_data_type) );
      memscpy( (void*)imsi_data_ptr,
               sizeof(nv_ehrpd_imsi_type),
               (void*)&ds3g_mmgsdi_impi_info[subs_id].imsi_info,
               sizeof(nv_ehrpd_imsi_type) );
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "IMPI data is not ready for querying");
      return FALSE;
    }
}
  return TRUE;
}/* ds3g_mmgsdi_get_impi_data_per_subs_id */

/*============================================================================
FUNCTION  DS3G_MMGSDI_CARD_REFRESH_EVENT_PROCESS 

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void ds3g_mmgsdi_card_refresh_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  uint8                           index;
  mmgsdi_session_type_enum_type   session_type;
  ds3g_mmgsdi_imsi_info_type     *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type    *mmgsdi_event_info_ptr = NULL;
  sys_modem_as_id_e_type          subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Card refresh event processing");
  if( (NULL == cmd_ptr) || (NULL ==  cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

#ifdef FEATURE_EHRPD
  /* Finish ACL processing, because no Card is present */
  ds3g_mmgsdi_acl_process_done();
#endif /* FEATURE_EHRPD */
  for ( index = 0; index < MAX_DS3G_MMGSDI_SESSIONS; index++ )
  {
    if( (mmgsdi_event_info_ptr->slot ==
         ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].slot) &&
        (mmgsdi_event_info_ptr->session_id ==
         ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_id) )
    {
      /* update operational flag in the table */
      session_type = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_type;

      imsi_info_ptr = ds3g_mmgsdi_get_imsi_info_from_session_type (session_type);
      if (imsi_info_ptr == NULL)
      {
       DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "IMSI info ptr is NULL ");
       return;
      }

      ds3g_mmgsdi_notify_event(DS3GEVENTMGR_MMGSDI_CARD_REFRESH_COMPLETE_EV,
                               session_type,
                               NULL);

      DATA_3GMGR_MSG1( MSG_LEGACY_HIGH,
                       "ds3g_mmgsdi_card_refresh_or_error_event_process:"
                       " session_type = %d", session_type);
      switch ( session_type )
      {
        case MMGSDI_1X_PROV_PRI_SESSION:
        case MMGSDI_1X_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
        case MMGSDI_1X_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */
#ifdef FEATURE_CDMA
          ds707_pkt_mgr_mmgsdi_reset();
#ifdef FEATURE_MMGSDI_SESSION_LIB
#ifdef FEATURE_DATA_IS707
          ds707_invalidate_jcdma_card_info();
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_CDMA */
          break;

        case MMGSDI_GW_PROV_PRI_SESSION:
        case MMGSDI_GW_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
        case MMGSDI_GW_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */
          /* Invalidate the IMSI only if the app type is USIM */
          if (MMGSDI_APP_USIM == mmgsdi_event_info_ptr->app_type)
          {
            subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);
            if (ds3gsubsmgr_is_subs_id_valid(subs_id))
            {
              ds3g_mmgsdi_reset_imsi_info(subs_id,
                                          mmgsdi_event_info_ptr->app_type,
                                          imsi_info_ptr);
            }

#ifdef FEATURE_EHRPD
            ds3g_mmgsdi_acl_info.acl_response = NULL;
            ds3g_mmgsdi_acl_info.acl_response_len = 0;

#endif /* FEATURE_EHRPD */

#ifdef FEATURE_MMGSDI_SESSION_LIB
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
            ds_ucsd_invalidate_sub_addr_info(session_type);
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
          }
          break;

        default:
          DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                          "Card refreshed/removed event for an invalid "
                          "session type:%d",
                          session_type);
          break;
      }
    }
  }/* for(.. */
  return;
}

/*============================================================================
FUNCTION  DS3G_MMGSDI_CARD_ERROR_EVENT_PROCESS 

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void ds3g_mmgsdi_card_error_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  uint8                           index;
  mmgsdi_session_type_enum_type   session_type;
  ds_cmd_type                    *cmd_buf = NULL;
  sys_modem_as_id_e_type          prev_subs_id = SYS_MODEM_AS_ID_NONE;
  ds3g_mmgsdi_imsi_info_type     *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type    *mmgsdi_event_info_ptr = NULL;
  sys_modem_as_id_e_type         *data_ptr = NULL;
  sys_modem_as_id_e_type          subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Card error event processing");

#ifdef FEATURE_EHRPD
  /* Finish ACL processing, because no Card is present */
  ds3g_mmgsdi_acl_process_done();
#endif /* FEATURE_EHRPD */

  for ( index = 0; index < MAX_DS3G_MMGSDI_SESSIONS; index++ )
  {
    if( mmgsdi_event_info_ptr->slot ==
         ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].slot)
    {
      /* update operational flag in the table */
      session_type = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_type;
      imsi_info_ptr = ds3g_mmgsdi_get_imsi_info_from_session_type (session_type);

      if (imsi_info_ptr == NULL)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "IMSI info ptr is NULL for session type[%d]",
                        session_type);
        return;
      }

      ds3g_mmgsdi_notify_event(DS3GEVENTMGR_MMGSDI_CARD_ERROR_EV,
                               session_type,
                               NULL);

      DATA_3GMGR_MSG1( MSG_LEGACY_HIGH,
                       "ds3g_mmgsdi_card_refresh_or_error_event_process: "
                       "session_type:%d",
                       session_type);

      switch ( session_type )
      {
        case MMGSDI_1X_PROV_PRI_SESSION:
        case MMGSDI_1X_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
        case MMGSDI_1X_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */
#ifdef FEATURE_CDMA

#if 0
          ds707_pkt_mgr_mmgsdi_reset();
          /*-----------------------------------------------------------------
           With the FR19634, DS will no longer trigger PPP graceful
           termination on receiving card removed event from MMGSDI.
           Rather CM sends the card removed event via registered DS
           callback function and then DS will start graceful PPP
           termination. This is required so that IMS can send de-registration
           OTA as part of card removed event.
          ------------------------------------------------------------------*/
#endif

#ifdef FEATURE_HDR
          /* Notify the users that subscription is ready */
          if (NULL != ds3g_mmgsdi_on_subscription_ready)
          {
            ds3g_mmgsdi_on_subscription_ready(FALSE);
          }
#endif /* FEATURE_HDR */
#ifdef FEATURE_MMGSDI_SESSION_LIB
#ifdef FEATURE_DATA_IS707
          ds707_invalidate_jcdma_card_info();
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_CDMA */
          break;

        case MMGSDI_GW_PROV_PRI_SESSION:
        case MMGSDI_GW_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
        case MMGSDI_GW_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */

          subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);
          if (ds3gsubsmgr_is_subs_id_valid(subs_id))
          {
            ds3g_mmgsdi_reset_imsi_info(subs_id,
                                        mmgsdi_event_info_ptr->app_type,
                                        imsi_info_ptr);

            ds3g_mmgsdi_reset_impi_info(subs_id);
          }

#ifdef FEATURE_EHRPD
          ds3g_mmgsdi_acl_info.acl_response = NULL;
          ds3g_mmgsdi_acl_info.acl_response_len = 0;

#endif /* FEATURE_EHRPD */

#ifdef FEATURE_MMGSDI_SESSION_LIB
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
          ds_ucsd_invalidate_sub_addr_info(session_type);
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
          break;

        default:
          DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                          "Card refreshed/removed event for an invalid "
                          "session type [%d]", session_type);
          break;
      }

      if (prev_subs_id != ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].sub_id )
      {
        prev_subs_id = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].sub_id;
        /*-------------------------------------------------------------------
          Post a DS command to clear Infinite Throttling
        -------------------------------------------------------------------*/
        cmd_buf = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
        if( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }

        cmd_buf->hdr.cmd_id = DS_CMD_3GPP_MMGSDI_CARD_ERROR;
        data_ptr = (sys_modem_as_id_e_type*)cmd_buf->cmd_payload_ptr;

        *data_ptr = prev_subs_id;

        ds_put_cmd_ext(cmd_buf);
      }

      if(ds_3gppi_is_rel_11_supported())
      {
        ds3g_mmgsdi_reset_efacc_value(session_type);
      }
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].app_type =
                            MMGSDI_APP_NONE;
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].is_operational =
                            FALSE;
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].sub_id =
                            SYS_MODEM_AS_ID_NONE;
      ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status = 0;

    }
  }/* for(.. */

  return;
}

/*===========================================================================
FUNCTION  DS3G_MMGSDI_SESSION_CHANGED_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_session_changed_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_changed_evt_info_type  session_changed_info;
  uint8                                 index;
  uint8                                 non_prov_index;
  mmgsdi_session_type_enum_type         session_type
                                         = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_type_enum_type         non_prov_session_type
                                                 = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type               mmgsdi_status = MMGSDI_ERROR;
  ds3g_mmgsdi_event_info_type          *mmgsdi_event_info_ptr = NULL;
  sys_modem_as_id_e_type                subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  memscpy( &session_changed_info,
           sizeof(mmgsdi_session_changed_evt_info_type),
           &(mmgsdi_event_info_ptr->session_changed_info),
           sizeof(mmgsdi_session_changed_evt_info_type) );

  DATA_MSG1(MSG_LEGACY_MED,
            "ds3g_mmgsdi_session_changed_event_process activated = %d",
            (int)session_changed_info.activated);

  for ( index = 0; index < MAX_DS3G_MMGSDI_SESSIONS; index++ )
  {
    if( (mmgsdi_event_info_ptr->slot ==
         ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].slot) &&
        (mmgsdi_event_info_ptr->session_id ==
         ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_id) )
    {
      /* update operational flag in the table */
      session_type
        = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_type;
      if( session_changed_info.activated == FALSE )
      {

        ds3g_mmgsdi_notify_event(DS3GEVENTMGR_MMGSDI_SESSION_CHANGED_EV,
                                 session_type,
                                 &session_changed_info);

        ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].is_operational = FALSE;
        ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status = 0;

        switch( session_type )
        {
          case MMGSDI_GW_PROV_PRI_SESSION:
          case MMGSDI_GW_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
          case MMGSDI_GW_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
            ds_ucsd_invalidate_sub_addr_info(session_type);
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

            /* Close the ISIM session */
            non_prov_session_type =
              ds3g_mmgsdi_slot_to_non_prov_session_type(
                 mmgsdi_event_info_ptr->slot);

            if (ds3g_mmgsdi_get_non_prov_isim_session_type_idx(
                                                        non_prov_session_type,
                                                        &non_prov_index)
                == FALSE)
            {
              DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                              "Could not get index for non prov session %d",
                              non_prov_session_type);
            }
            else
            {
              mmgsdi_status = mmgsdi_session_close(
                               ds3g_mmgsdi_isim_info[non_prov_index].session_id,
                               ds3g_mmgsdi_client_rsp_cb,
                               (mmgsdi_client_data_type)0);

              if (mmgsdi_status != MMGSDI_SUCCESS)
              {
                DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                              "non-prov ISIM session close failed "
                              "mmgsdi_status [%d] session id [%d]",
                              mmgsdi_status,
                              ds3g_mmgsdi_isim_info[non_prov_index].session_id);
              }

              ds3g_mmgsdi_isim_info[non_prov_index].is_operational = FALSE;
              memset(&ds3g_mmgsdi_isim_info[non_prov_index].app_info,
                     0,
                     sizeof(mmgsdi_aid_type));
              ds3g_mmgsdi_isim_info[non_prov_index].channel_id = 0;
              ds3g_mmgsdi_isim_info[non_prov_index].session_id
                                             = DS3G_MMGSDI_INVALID_SESSION_ID;
              ds3g_mmgsdi_isim_info[non_prov_index].slot_id
                                             = MMGSDI_MAX_SLOT_ID_ENUM;

              subs_id= ds3g_mmgsdi_get_sub_id_from_slot(mmgsdi_event_info_ptr->slot);
              if (ds3gsubsmgr_is_subs_id_valid(subs_id))
              {
                ds3g_mmgsdi_reset_impi_info(subs_id);
              }
            }
            break;

          case MMGSDI_1X_PROV_PRI_SESSION:
          case MMGSDI_1X_PROV_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM
          case MMGSDI_1X_PROV_TER_SESSION:
#endif /* FEATURE_TRIPLE_SIM */
#if defined(FEATURE_MMGSDI_SESSION_LIB) && defined (FEATURE_DATA_IS707)
#if 0
          ds707_pkt_mgr_mmgsdi_reset();
          /*-----------------------------------------------------------------
           With the FR 21260, DS will no longer trigger PPP graceful
           termination on receiving session changed event from MMGSDI.
           Rather CM sends the card error event via registered DS
           callback function and then DS will start graceful PPP
           termination. This is required so that IMS can send de-registration
           OTA as part of card removed event.
          ------------------------------------------------------------------*/
#endif
            ds707_invalidate_jcdma_card_info();
#endif /* FEATURE_MMGSDI_SESSION_LIB && FEATURE_DATA_IS707 */
            break;

          default:
            DATA_MSG1(MSG_LEGACY_MED,
                      "Session changed for an invalid session type [%d]",
                      session_type);
            break;
        }
      }
    }
  }
  return;
} /* ds3g_mmgsdi_session_changed_event_process */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_SRV_ABAIL_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for the MMGSDI SRV AVAIL events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_srv_avail_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_service_enum_type      client_ref = 0;
  mmgsdi_session_id_type        session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MDM_SYMBOL_NOT_USED(client_ref);
  DATA_MDM_SYMBOL_NOT_USED(session_id);

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  client_ref = (mmgsdi_service_enum_type)
                 mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info.client_ref;

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Received service info = [%d], session id = [%d]",
                  client_ref, session_id);
  return;
}/* ds3g_mmgsdi_srv_avail_event_process */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_TYPE_IDX

DESCRIPTION
  This function returns session info index based on session type

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Get the index of the table successfully
  FALSE: Failed to get the index of the table

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_session_type_idx
(
  mmgsdi_session_type_enum_type   session_type,
  uint8                          *index
)
{
  uint8    i;
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
         == session_type )
    {
      *index = i;
      ret_val =  TRUE;
      break;
    }
  }

  if( !ret_val )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "index not found in ds3g_mmgsdi_info for session_type:%d",
                    session_type);
  }

  return ret_val;
}/* ds3g_mmgsdi_get_session_type_idx */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_ID_IDX

DESCRIPTION
  This function returns session info index based on session id

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Get the index of the table successfully
  FALSE: Failed to get the index of the table

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_session_id_idx
(
  mmgsdi_session_id_type   session_id,
  uint8                   *index
)
{
  uint8   i;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id
         == session_id )
    {
      *index = i;
      ret_val = TRUE;
      break;
    }
  }

  if( !ret_val )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "index not found in ds3g_mmgsdi_info for session_id:%d",
                    session_id);
  }

  return ret_val;
}/* ds3g_mmgsdi_get_session_id_idx */

/*===========================================================================
FUNCTION DS3G_MMGSDI_PROCESS_CMD

DESCRIPTION
  A command processing function is called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any MMGSDI command is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_process_cmd
(
  ds_cmd_type  *cmd_ptr
)
{

  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( cmd_ptr != NULL );
  ASSERT( cmd_ptr->cmd_payload_ptr != NULL );

  mmgsdi_event_info_ptr = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;
  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "Processing cmd = [%d] event = [%d] session id = [%d]",
                  cmd_ptr->hdr.cmd_id,
                  mmgsdi_event_info_ptr->event_type,
                  mmgsdi_event_info_ptr->session_id );

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on ds3g
    mmgsdi events.
  -------------------------------------------------------------------------*/
  switch( mmgsdi_event_info_ptr->event_type )
  {
    case DS3G_MMGSDI_CLIENT_ID_EVENT:
      ds3g_mmgsdi_client_id_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT:
      ds3g_mmgsdi_subscription_ready_event_process( cmd_ptr );
      ds3g_mmgsdi_reset_cc_sp_throttle_if_required( cmd_ptr );
      break;

    case DS3G_MMGSDI_REFRESH_VOTING_EVENT:
      ds3g_mmgsdi_refresh_voting_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_OK_REFRESH_EVENT:
      ds3g_mmgsdi_ok_refresh_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_REFRESH_COMPLETE_EVENT:
      ds3g_mmgsdi_refresh_fcn_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_SIM_READ_EVENT:
      ds3g_mmgsdi_sim_read_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_SIM_WRITE_EVENT:
      ds3g_mmgsdi_sim_write_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_SRV_AVAIL_EVENT:
      ds3g_mmgsdi_srv_avail_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_IMSI_INFO_EVENT:
      ds3g_mmgsdi_imsi_info_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_MNC_INFO_EVENT:
      ds3g_mmgsdi_mnc_info_event_process( cmd_ptr );
      break;

#ifdef FEATURE_EHRPD
    case DS3G_MMGSDI_ACL_INFO_EVENT:
      ds3g_mmgsdi_acl_info_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_ACL_ENABLED_EVENT:
      ds3g_mmgsdi_acl_enabled_event_process( cmd_ptr );
      break;
#endif /* FEATURE_EHRPD */

    case DS3G_MMGSDI_CARD_ERROR_EVENT:
      ds3g_mmgsdi_card_error_event_process ( cmd_ptr );
      ds3g_mmgsdi_reset_cc_sp_throttle_if_required( cmd_ptr );
      if(ds_3gppi_is_rel_11_supported())
      {
        subs_id = ds3g_mmgsdi_get_sub_id_from_session_id
                  (mmgsdi_event_info_ptr->session_id);
        ds_3gpp_pdn_set_max_simul_active_pdp_context
          ( DS_3GPP_MAX_BEARER_CONTEXT, subs_id );
        ds_3gpp_pdn_throt_reset_pdn_throttle_if_required( subs_id );
      }
      break;

    case DS3G_MMGSDI_REFRESH_START_EVENT:
      ds3g_mmgsdi_card_refresh_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_SESSION_CHANGED_EVENT:
      ds3g_mmgsdi_session_changed_event_process( cmd_ptr);
      break;

    case DS3G_MMGSDI_ISIM_OK_REFRESH_EVENT:
      ds3g_mmgsdi_isim_ok_refresh_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_ISIM_REFRESH_START_EVENT:
      ds3g_mmgsdi_isim_refresh_start_processing( cmd_ptr );
      break;

    case DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_EVENT:
      ds3g_mmgsdi_isim_refresh_complete_processing( cmd_ptr );
      break;


    case DS3G_MMGSDI_EFACC_READ_CNF_EVENT:
      ds3g_mmgsdi_efacc_update_cache( cmd_ptr );
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Unrecognized event type [%d] rcvd",
                      mmgsdi_event_info_ptr->event_type);
      break;
  }
  return;
} /* ds3g_mmgsdi_process_cmd */

/*===========================================================================
FUNCTION:  DS3G_MMGSDI_READ_TRANSPARENT_CB

DESCRIPTION:
  This is the callback function provided when calling MMGSDI interface
  mmgsdi_session_read_transparent. This is a special callback function
  provided when read CDMA service table from CSIM/RUIM

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void ds3g_mmgsdi_read_transparent_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type                  *cmd_buf = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
  ds_at_mmgsdi_sim_info_type   *cmd_rd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cnf_ptr != NULL );

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
        cnf_ptr->read_cnf.response_header.client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_read_transparent_cb(): "
                    "Invalid client Id [%d] ",
                    cnf_ptr->read_cnf.response_header.client_id);
    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
       ( MMGSDI_READ_CNF == cnf_ptr->read_cnf.response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS == cnf_ptr->read_cnf.response_header.mmgsdi_status ) )
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                      "ds3g_mmgsdi_read_transparent_cb() with "
                      "client ref [%d] status [%d]", cnf, status);

      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;

      cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);

      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_SIM_READ_EVENT;
      // Session id
      mmgsdi_event_info_ptr->session_id
        = cnf_ptr->read_cnf.response_header.session_id;
      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;

      cmd_rd_ptr->cmd_status = (mmgsdi_return_enum_type)status;
      // Client data
      cmd_rd_ptr->client_ref = cnf_ptr->read_cnf.response_header.client_data;
      // Copy read_data
      cmd_rd_ptr->data_len = cnf_ptr->read_cnf.read_data.data_len;
      memscpy (cmd_rd_ptr->data, MAX_MMGSDI_BUFFER_LEN,
               cnf_ptr->read_cnf.read_data.data_ptr,
               (uint32)cmd_rd_ptr->data_len);
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "ds3g_mmgsdi_read_transparent_cb() with status error:%d",
                      status);
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "ds3g_mmgsdi_read_transparent_cb() with invalid CNF [%d]",
                    cnf);
  }
  return;
} /* ds3g_mmgsdi_read_transparent_cb */

/*===========================================================================
FUNCTION:  DS3G_MMGSDI_WRITE_TRANSPARENT_CB

DESCRIPTION:
  This is the callback function provided when calling MMGSDI interface
  mmgsdi_session_write_transparent. This is a special callback function
  provided when write 3GPD Operational Capabilities through MMGSDI

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void ds3g_mmgsdi_write_transparent_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type                  *cmd_buf = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
  ds_at_mmgsdi_sim_info_type   *cmd_rd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cnf_ptr != NULL );

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
        cnf_ptr->write_cnf.response_header.client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_write_transparent_cb(): Invalid client Id %d",
                    cnf_ptr->write_cnf.response_header.client_id);
    return;
  }

  if ( ( MMGSDI_WRITE_CNF == cnf ) &&
       ( MMGSDI_WRITE_CNF == cnf_ptr->write_cnf.response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS == cnf_ptr->write_cnf.response_header.mmgsdi_status ) )
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                      "ds3g_mmgsdi_write_transparent_cb client ref:%d status:%d",
                      cnf, status);

      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;

      cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);

      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_SIM_WRITE_EVENT;
      // Session Id
      mmgsdi_event_info_ptr->session_id
        =  cnf_ptr->write_cnf.response_header.session_id;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      cmd_rd_ptr->cmd_status = (mmgsdi_return_enum_type)status;
      // Client Data
      cmd_rd_ptr->client_ref = cnf_ptr->write_cnf.response_header.client_data;

      // Write data length
      cmd_rd_ptr->data_len = cnf_ptr->write_cnf.written_data_len;
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "ds3g_mmgsdi_write_transparent_cb() status error:%d",
                      status);
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "ds3g_mmgsdi_write_transparent_cb() with invalid CNF [%d]",
                    cnf);
  }
  return;
} /* ds3g_mmgsdi_write_transparent_cb */

/*===========================================================================
FUNCTION:  DS3G_MMGSDI_SERVICE_AVAILABLE_CB

DESCRIPTION:
  This is the callback function provided when calling MMGSDI interface
  mmgsdi_session_is_service_available. This is a special callback function
  provided when check available services through MMGSDI

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void ds3g_mmgsdi_service_available_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type                  *cmd_buf = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
  ds_at_mmgsdi_sim_info_type   *cmd_rd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cnf_ptr != NULL );

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
        cnf_ptr->srv_available_cnf.response_header.client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_service_available_cb(): Invalid client Id %d",
                    cnf_ptr->write_cnf.response_header.client_id);
    return;
  }

  if ( ( MMGSDI_WRITE_CNF == cnf ) &&
       ( MMGSDI_WRITE_CNF ==
         cnf_ptr->srv_available_cnf.response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS ==
           cnf_ptr->srv_available_cnf.response_header.mmgsdi_status ) )
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                      "ds3g_mmgsdi_service_available_cb client ref:%d status:%d",
                      cnf, status);

      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;

      cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_sim_info);

      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_SRV_AVAIL_EVENT;
      // Session Id
      mmgsdi_event_info_ptr->session_id
        = cnf_ptr->srv_available_cnf.response_header.session_id;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;

      cmd_rd_ptr->cmd_status = (mmgsdi_return_enum_type)status;

      // Client data
      cmd_rd_ptr->client_ref
        = cnf_ptr->srv_available_cnf.response_header.client_data;

      // Srv_available
      cmd_rd_ptr->data[0] = cnf_ptr->srv_available_cnf.srv_available;

      // Set data len
      cmd_rd_ptr->data_len = 1;
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "ds3g_mmgsdi_service_available_cb() status error:%d",
                      status);
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "ds3g_mmgsdi_service_available_cb() with invalid CNF [%d]",
                    cnf);
  }
  return;
} /* ds3g_mmgsdi_service_available_cb */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_TRANSPARENT

DESCRIPTION
  This functions provides read access to any transparent files using MMGSDI
  interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_read_transparent
(
  mmgsdi_access_type              access,
  mmgsdi_offset_type              offset,
  mmgsdi_len_type                 req_len,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_session_type_enum_type   session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type   session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DS3G MMGSDI Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }

  /* check session id */
  if ( ds3g_mmgsdi_get_session_id( session_type, &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session id using Session Type [%d] ",
                    session_type);
    return mmgsdi_status;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED, "Sending Read Transparent on Session "
                  "Type [%d]for Session Id [%d] ", session_type, session_id);

  if ( response_cb_ptr == NULL )
  {
    /* if callback function is not provided, we use the default call back */
    response_cb_ptr = ds3g_mmgsdi_client_rsp_cb;
  }

  mmgsdi_status = mmgsdi_session_read_transparent( session_id,
                                                   access,
                                                   offset,
                                                   req_len,
                                                   response_cb_ptr,
                                                   ( mmgsdi_client_data_type ) 0);

  /* Check to see if REQ was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Failed to call MMGSDI Read "
                    "Transparent [%d]", mmgsdi_status);
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_read_transparent */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_WRITE_TRANSPARENT

DESCRIPTION
  This functions provides write access to any transparent files using MMGSDI
  interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_write_transparent
(
  mmgsdi_access_type             file_access,
  mmgsdi_write_data_type         write_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DS3G MMGSDI Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }

  /* check session id */
  if ( ds3g_mmgsdi_get_session_id( session_type,
                                   &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session id using Session Type [%d] ",
                    session_type);
    return mmgsdi_status;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "Sending Write Transparent on "
                  "Session Type [%d]", session_type);

  if ( response_cb_ptr == NULL )
  {
     response_cb_ptr = ds3g_mmgsdi_client_rsp_cb;
  }

  mmgsdi_status = mmgsdi_session_write_transparent( session_id,
                                                    file_access,
                                                    0,
                                                    write_data,
                                                    response_cb_ptr,
                                                    ( mmgsdi_client_data_type ) 0 );

  /* Check to see if REQ was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DS3G MMGSDI Failed to call MMGSDI "
                    "Write Transparent [%d]", mmgsdi_status);
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_write_transparent */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_SERVICE_AVAILABLE

DESCRIPTION
  This functions provides service availble functionality using MMGSDI
  interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_service_available
(
  mmgsdi_service_enum_type       srvc_type,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type   session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_service_available(): Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }

  /* check session id */
  if ( ds3g_mmgsdi_get_session_id( session_type, &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session id using Session Type [%d] ",
                    session_type);
    return mmgsdi_status;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "ds3g_mmgsdi_service_available():Sending Session Available "
                  "on Session Type [%d]",
                  session_type);

  if ( response_cb_ptr == NULL )
  {
     response_cb_ptr = ds3g_mmgsdi_client_rsp_cb;
  }

  mmgsdi_status = mmgsdi_session_is_service_available( session_id,
                                                       srvc_type,
                                                       response_cb_ptr,
                                                       ( mmgsdi_client_data_type )
                                                       srvc_type );

  /* Check to see if REQ was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "DS3G MMGSDI Failed to call MMGSDI Write Transparent [%d]",
                    mmgsdi_status);
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_service_available */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_SESSION_GET_APP_CAPABILITIES

DESCRIPTION
  This functions requests app capabilities using MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_session_get_app_capabilities
(
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type   session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }

  /* check session id */
  if ( ds3g_mmgsdi_get_session_id( session_type, &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session id using Session Type [%d] ",
                    session_type);
    return mmgsdi_status;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "Requesting App Capabilities on Session Type [%d]",
                  session_type);

  if ( response_cb_ptr == NULL )
  {
     response_cb_ptr = ds3g_mmgsdi_client_rsp_cb;
  }

  mmgsdi_status = mmgsdi_session_get_app_capabilities( session_id,
                                                       response_cb_ptr,
                                                       client_ref );

  /* Check to see if Request was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to call mmgsdi_session_get_app_capabilitiest [%d]",
                    mmgsdi_status);
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_session_get_app_capabilities */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers file change notification using MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_register_for_refresh
(
  mmgsdi_refresh_file_list_type  refresh_files,
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type   session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DS3G MMGSDI Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }

  /* check session id */
  if ( ds3g_mmgsdi_get_session_id( session_type, &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session id using Session Type [%d] ",
                    session_type);
    return mmgsdi_status;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "Registering for file change notification Session Type [%d]",
                  session_type);

  mmgsdi_status = mmgsdi_session_register_for_refresh( session_id,
                                                       refresh_files,
                                                       TRUE,
                                                       ds3g_mmgsdi_client_rsp_cb,
                                                       ( mmgsdi_client_data_type ) 0 );

  /* Check to see if REQ for Registration was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to register for file change notification [%d]",
                    mmgsdi_status);
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_register_for_refresh */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_REFRESH_COMPLETE

DESCRIPTION
  This function informs MMGSDI when it has finished all the files that were
  refreshed

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_refresh_complete
(
  boolean                        pass_fail,
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type   session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DS3G MMGSDI Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }

  /* check session id */
  if ( ds3g_mmgsdi_get_session_id( session_type, &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session id using Session Type [%d] ",
                    session_type);
    return mmgsdi_status;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Sending MMGSDI Refresh complete pass_fail [%d] on "
                  "Session Type [%d]",
                  pass_fail, session_type);


  mmgsdi_status = mmgsdi_session_refresh_complete( session_id,
                                                   pass_fail,
                                                   ds3g_mmgsdi_client_rsp_cb,
                                                   (mmgsdi_client_data_type)0 );

  /* Check to see if REQ for Registration was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to call MMGSDI Refresh Complete [%d]",
                    mmgsdi_status);
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_refresh_complete */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_OK_TO_REFRESH

DESCRIPTION
  This functions sends OK to refresh to MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

==============================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_ok_to_refresh
(
  boolean                        ok_to_refresh,
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type   session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DS3G MMGSDI Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }

  /* check session id */
  if ( ds3g_mmgsdi_get_session_id( session_type, &session_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session id using Session Type [%d] ",
                    session_type);
    return mmgsdi_status;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Sending OK to Refresh [%d] on Session Type [%d]",
                  ok_to_refresh, session_type);

  mmgsdi_status = mmgsdi_session_ok_to_refresh( session_id,
                                                ok_to_refresh,
                                                ds3g_mmgsdi_client_rsp_cb,
                                                ( mmgsdi_client_data_type ) 0 );

  /* Check to see if REQ was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Failed to call MMGSDI OK to Refresh "
                    "[%d]", mmgsdi_status);
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_ok_to_refresh */

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION DS3G_MMGSDI_SESSION_READ_RECORD()

DESCRIPTION
  This function internally calls mmgsdi_session_read_record.

PARAMETERS
  session_id      : Session ID.
  file_access     : Info about the file need to be read.
  record_number   : Number of the record need to read.
  request_length  : Number of bytes need to be read.
  response_cb_ptr : Callback called after read is complete.
  client_ref      : Client reference.

RETURN VALUE
  MMGSDI_SUCCESS - If mmgsdi_session_read_record returns success.
  MMGSDI_ERROR   - If any Error Scenario occured.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_session_read_record
(
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_rec_num_type      record_number,
  mmgsdi_len_type          request_length,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  uint8                    index         = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_session_read_record");
  if( ds3g_mmgsdi_get_session_id_idx( session_id, &index ) == TRUE )
  {
    mmgsdi_status = mmgsdi_session_read_record(
                           session_id,
                           file_access,
                           record_number,
                           request_length,
                           response_cb_ptr,
                           client_ref);
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_LOW, "session_id not matched");
  }

  return mmgsdi_status;
} /* ds3g_mmgsdi_session_read_record */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SESSION_GET_FILE_ATTR

DESCRIPTION
  This function internally calls mmgsdi_session_get_file_attr.

PARAMETERS
  session_id      : Session ID.
  file_name       : File name
  response_cb_ptr : Callback called after read is complete.
  client_ref      : Client reference.

RETURN VALUE
  MMGSDI_SUCCESS - If mmgsdi_session_read_record returns success.
  MMGSDI_ERROR   - If any Error Scenario occured.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_session_get_file_attr
(
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_name,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  uint8                   index         = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ds3g_mmgsdi_get_session_id_idx( session_id, &index ) == TRUE )
  {
    mmgsdi_status = mmgsdi_session_get_file_attr(
                           session_id,
                           file_name,
                           response_cb_ptr,
                           client_ref);
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_LOW,"session_id not matched");
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_session_get_file_attr */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SESSION_JCDMA_GET_CARD_INFO

DESCRIPTION
  This function internally calls mmgsdi_session_jcdma_get_card_info.

PARAMETERS
  session_id      : Session ID.
  response_cb_ptr : Callback called after read is complete.
  client_ref      : Client reference.

RETURN VALUE
  MMGSDI_SUCCESS - If mmgsdi_session_read_record returns success.
  MMGSDI_ERROR   - If any Error Scenario occured.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_session_jcdma_get_card_info
(
  mmgsdi_session_id_type   session_id,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  uint8                    index         = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ds3g_mmgsdi_get_session_id_idx( session_id, &index ) == TRUE )
  {
    mmgsdi_status = mmgsdi_session_jcdma_get_card_info(
                           session_id,
                           response_cb_ptr,
                           client_ref);
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_LOW, "session_id not matched");
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_session_jcdma_get_card_info */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_ID_FROM_SESSION_TYPE

DESCRIPTION
  This function returns the session_id based on session_type.

PARAMETERS
  session_type : mmgsdi session type.

RETURN VALUE
  session_id : session_id corresponding to the given session_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_session_id_type ds3g_mmgsdi_get_session_id_from_session_type
(
  mmgsdi_session_type_enum_type  session_type
)
{
  uint8  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
          == session_type )
    {
      return ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id;
    }
  }
  return DS3G_MMGSDI_INVALID_CLIENT_ID;
} /* ds3g_mmgsdi_get_session_id_from_session_type */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SUB_ID_FROM_SESSION_TYPE

DESCRIPTION
  This function returns the session_id based on session_type.

PARAMETERS
  session_type : mmgsdi session type.

RETURN VALUE
  sub_id : sub_id corresponding to the given session_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_mmgsdi_get_sub_id_from_session_type
(
  mmgsdi_session_type_enum_type  session_type
)
{
  uint8  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
          == session_type )
    {
      return ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].sub_id;
    }
  }
  return SYS_MODEM_AS_ID_NONE;
} /* ds3g_mmgsdi_get_sub_id_from_session_type */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_TYPE_FROM_SESSION_ID

DESCRIPTION
  This function returns the session_type based on session_id.

PARAMETERS
  session_id : mmgsdi session id.

RETURN VALUE
  session_type : session_type corresponding to the given session_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_session_type_enum_type ds3g_mmgsdi_get_session_type_from_session_id
(
  mmgsdi_session_id_type session_id
)
{
  uint8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id == session_id )
    {
      return ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type;
    }
  }
  return MMGSDI_MAX_SESSION_TYPE_ENUM;
} /* ds3g_mmgsdi_get_session_type_from_session_id */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_APP_TYPE_FROM_SESSION_ID

DESCRIPTION
  This function returns the app_type based on session_id.

PARAMETERS
  session_id : mmgsdi session id.

RETURN VALUE
  app_type : app_type corresponding to the given session_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_app_enum_type ds3g_mmgsdi_get_app_type_from_session_id
(
  mmgsdi_session_id_type  session_id
)
{
  uint8  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id
          == session_id )
    {
      return ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].app_type;
    }
  }
  return MMGSDI_APP_NONE;
} /* ds3g_mmgsdi_get_app_type_from_session_id */
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*==============================================================================
FUNCTION DS3G_MMGSDI_VOTING_FOR_REFRESH

DESCRIPTION
  This function executes data refresh voting and sends refresh command with
  the voting result using MMGSDI interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_isim_voting_for_refresh
(
  mmgsdi_session_id_type         session_id,
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  ds3g_hdlr_func_tbl_type *hdlr_tbl_ptr; /* Ptr to mode-sp. hdlr table */
  boolean                  vote_result = FALSE;
  boolean                  vote_finished  = FALSE;
  sys_sys_mode_e_type      mode = SYS_SYS_MODE_NONE;
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subs_id = ds3g_mmgsdi_get_sub_id_from_session_id(session_id);
  mode = ds3g_get_current_network_mode_ex(subs_id);

  if( (( subs_id == SYS_MODEM_AS_ID_NONE ) ||
      ( subs_id != ds3g_get_ps_subs_id())) &&
      ( ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE))
  {
    /* for Non PS susbscription always vote "YES" */
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Vote for YES on Subs Id [%d]",
                    subs_id);
    vote_result   = TRUE;
    vote_finished = TRUE;
  }
  else
  {
    if ( mode == SYS_SYS_MODE_NONE )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Invalid System Mode %d",
                      mode);
      return;
    }
    else if ( mode == SYS_SYS_MODE_CDMA )
    {
 #ifdef FEATURE_DATA_IS707
      /* In C2K mode, vote YES to USIM refresh if the data call is not eHRPD */
      if ((TRUE != ds707_pkt_mgr_is_data_on_ehrpd()) &&
          (( session_type == MMGSDI_GW_PROV_PRI_SESSION ) ||
           ( session_type == MMGSDI_GW_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
           || ( session_type == MMGSDI_GW_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
          ))
      {
        vote_result = TRUE;
        vote_finished = TRUE;
        DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                       "Vote for YES on Session Type [%d], mode [%d]",
                        session_type,
                        mode);
      }
 #endif
    }
    else
    {
      /* In GSM/WCDMA/LTE mode, vote YES to CSIM refresh */
      if (( session_type == MMGSDI_1X_PROV_PRI_SESSION )||
          ( session_type == MMGSDI_1X_PROV_SEC_SESSION )
#ifdef FEATURE_TRIPLE_SIM
          || ( session_type == MMGSDI_1X_PROV_TER_SESSION )
#endif /* FEATURE_TRIPLE_SIM */
         )
      {
        vote_result = TRUE;
        vote_finished = TRUE;
        DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                        "Vote for YES on Session Type [%d], mode [%d]",
                        session_type,
                        mode);
      }
    }
  }
  /* Vote is not done  */
  if ( !vote_finished )
  {
    /* get the mode handler for current system */
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[(int8)mode][DS_CALL_TYPE_PKT];
    if (( hdlr_tbl_ptr != NULL ) &&
        ( hdlr_tbl_ptr->isim_refresh_voting_handler != NULL ))
    {
      // call the mode specific handler for current system
      vote_result = hdlr_tbl_ptr->isim_refresh_voting_handler(subs_id);
    }
    else
    {
      /* if the current system did not register a handler, vote yes */
      vote_result = TRUE;
    }
  }
  /* vote for refresh according to mode handler result */
  mmgsdi_status = ds3g_mmgsdi_ok_to_refresh( vote_result,
                                             session_type );
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Refresh voting failed with return status [%d], "
                    "session_type [%d]", mmgsdi_status, session_type);
  }
} /* ds3g_mmgsdi_isim_voting_for_refresh */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_ISIM_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register DS Manager for ISIM refresh voting.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void ds3g_mmgsdi_isim_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type refresh_files = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "Sending voting registration with session type ",
                  session_type);

  mmgsdi_status = ds3g_mmgsdi_register_for_refresh( refresh_files,
                                                    session_type );

  /* Check to see if REQ for Registration was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to Request Refresh Voting Registration with "
                    "status [%d]",
                    mmgsdi_status);
  }
  return;
} /* ds3g_mmgsdi_register_for_refresh */

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION DS3G_MMGSDI_UIM_OPERATIONAL_CAPABILITY

DESCRIPTION
  This function initializes the operational capability elementary file
  on an IS-820 Release A card to indicate 3GPD capabilities supported based
  on the features defined.
  It creates and issues a MMGSDI ACCESS command to write to the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_uim_operational_capability
(
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_write_data_type   write_data;
  byte                     operational_capabilities = 0;
  mmgsdi_access_type       file_access;
  mmgsdi_app_enum_type     app_type = MMGSDI_APP_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  operational_capabilities |= DS3G_MMGSDI_SUPPORT_SIMPLE_IP_MASK;
#ifdef FEATURE_DS_MOBILE_IP
  operational_capabilities |= DS3G_MMGSDI_SUPPORT_FALLBACK_MASK;
  operational_capabilities |= DS3G_MMGSDI_SUPPORT_MOBILE_IP_MASK;
#endif /* FEATURE_DS_MOBILE_IP */

  write_data.data_len = 1;  /* only the operational capabilities which is of 1 byte*/
  write_data.data_ptr = &operational_capabilities;

  /* check application type */
  if ( ds3g_mmgsdi_get_app_type( session_type, &app_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an activated Session type = [%d]",
                    session_type);
    return;
  }
  /* Check application type */
  if ( app_type == MMGSDI_APP_CSIM )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Writing 3GPD Operational Capabilities to CSIM");
    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_CSIM_3GPD_ME3GPDOPC;
  }
  else if ( app_type == MMGSDI_APP_RUIM )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Writing 3GPD Operational Capabilities to RUIM");
    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_CDMA_3GPD_OP_CAP;
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Could not write 3GPD Operational Capabilities to [%d]",
                    app_type);
    return;
  }

  /* Send the write request of the operational capabilites
     to 3GPD using MMGSDI API*/

  mmgsdi_status = ds3g_mmgsdi_write_transparent( file_access,
                                                 write_data,
                                                 (mmgsdi_callback_type)
                                                 ds3g_mmgsdi_write_transparent_cb,
                                                 MMGSDI_1X_PROV_PRI_SESSION );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Rejected setting of 3GPD Operational Capabilities status"
                    " [%d] on app type [%d]",
                    mmgsdi_status, app_type);
  }
  return;
} /* ds3g_mmgsdi_uim_operational_capability */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_1X_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register 1X CDMA for File Change Notifications for EF's.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds3g_mmgsdi_1x_register_for_refresh
(
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type  refresh_files = {0};
  mmgsdi_file_enum_type          file_list[DS3G_MMGSDI_MAX_NUM_REFRESH_FILES];
  mmgsdi_app_enum_type           app_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get application type */
  if ( ds3g_mmgsdi_get_app_type( session_type, &app_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an activated Session Type = [%d]",
                    session_type);
    return;
  }
  /* Issue a read command to read the SVC TABLE through MMGSDI API */
  if ( app_type == MMGSDI_APP_CSIM )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Registers CDMA file change from CSIM");
    file_list[0] = MMGSDI_CSIM_3GPD_3GPDOPM; /* EF for NV_DS_QCMIP_I */
    file_list[1] = MMGSDI_CSIM_3GPD_MIPSP;    /* EF for NV_DS_MIP_ACTIVE_PROF_I */
    refresh_files.num_files = 2;
  }
  else if ( app_type == MMGSDI_APP_RUIM )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Registers CDMA file change from RUIM");
    file_list[0] = MMGSDI_CDMA_3GPD_3GPDOPM; /* EF for NV_DS_QCMIP_I */
    file_list[1] = MMGSDI_CDMA_3GPD_MIPSP;    /* EF for NV_DS_MIP_ACTIVE_PROF_I */
    refresh_files.num_files = 2;
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Could not register CDMA file change from app type [%d]",
                    app_type);
    return;
  }
  refresh_files.file_list_ptr = &file_list[0];


  DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                  "Sending request for file change notification");

  /* Register with MMGSDI for File Change Notification using the file list
    in the array above   */
  mmgsdi_status = ds3g_mmgsdi_register_for_refresh( refresh_files,
                                                    session_type );

  /* Check to see if REQ for Registration was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Failed to Request Refresh FCN Registration");
  }

  return;
} /* ds3g_mmgsdi_1x_register_for_refresh */

/*==============================================================================
FUNCTION DS3G_MMGSDI_SESSION_COMPUTE_IP_AUTH

DESCRIPTION
  This function sends compute IP authentication to MMGSDI interface.
  This function is called by dsmip_auth and it is a synchronized call

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type ds3g_mmgsdi_session_compute_ip_auth
(
  mmgsdi_compute_ip_data_type  compute_ip_data
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type session_id = DS3G_MMGSDI_INVALID_SESSION_ID;

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "DS3G MMGSDI Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return mmgsdi_status;
  }
  /* check session id */

  /* We use primary session first */
  if ( ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_PRI_SESSION, &session_id )
         == TRUE )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Find a session id [%d] using session type [%d]",
                    session_id, MMGSDI_1X_PROV_PRI_SESSION);
  }
  /* We use secondary session */
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find a session id using session type [%d]",
                    MMGSDI_1X_PROV_PRI_SESSION);

    if ( ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_SEC_SESSION, &session_id )
           != TRUE )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Failed to find a session id using session type [%d]",
                      MMGSDI_1X_PROV_SEC_SESSION);
#ifdef FEATURE_TRIPLE_SIM
      /* We use tertiary session */
      if( ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_TER_SESSION, &session_id )
            != TRUE )
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Failed to find a session id using session type [%d]",
                        MMGSDI_1X_PROV_TER_SESSION);
        return mmgsdi_status;
      }
      else
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                        "Found a session id [%d] using session type [%d]",
                        session_id, MMGSDI_1X_PROV_TER_SESSION);
      }
#else
      return mmgsdi_status;
#endif /* FEATURE_TRIPLE_SIM */
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "Found a session id [%d] using session type [%d]",
                      session_id, MMGSDI_1X_PROV_SEC_SESSION);
    }
  }

  // This is running under PS context
  PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                  "DS3G MMGSDI Sending Request for Computing Ip Auth");

  /* Register with MMGSDI for computing IP authentication   */
  mmgsdi_status = mmgsdi_session_compute_ip_auth( session_id,
                                                  compute_ip_data,
                                                  ds3g_mmgsdi_mip_resp_cb,
                                                  ( mmgsdi_client_data_type ) 0 );

  /* Check to see if REQ for Registration was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "DS3G MMGSDI Failed to Request Compute Ip Auth [%d]",
                    mmgsdi_status);
    return mmgsdi_status;
  }

  // wait for callback
  (void)psi_wait((rex_sigs_type) 1 << (int)PS_UIM_CMD_SIGNAL);

  if ( ds3g_mmgsdi_mip_info.status == TRUE )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "DS3G MMGSDI Read Compute Ip Data Successfully");
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                    "DS3G MMGSDI Failed to read Compute Ip Data");
  }
  return mmgsdi_status;
} /* ds3g_mmgsdi_session_compute_ip_auth */

/*===========================================================================
FUNCTION DS3G_MMGSDI_MIP_RESP_CB

DESCRIPTION
  Retrieves the MN_HA/RRQ/MN_AAA Response from the mmgsdi response,
  and copies into variables local to the file.  This is a callback
  function which is called from the mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This is a synchronized call. It uses set signal to unblock DS.
===========================================================================*/
void ds3g_mmgsdi_mip_resp_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_compute_ip_auth_cnf_type  *compute_ip_cnf_ptr = NULL;
  rex_tcb_type                     *ps_tcb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* set status to FALSE */
  ds3g_mmgsdi_mip_info.status = FALSE;
  /*------------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
  We get the ps tcb pointer which is used to set rex signal
  ----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ps_tcb_ptr = rcinit_lookup_rextask("ps");
  if ( RCINIT_NULL == ps_tcb_ptr)
  {
    DATA_ERR_FATAL("RCINIT Lookup for ps tcb returned NULL");
  }
#else
  ps_tcb_ptr = &ps_tcb;
#endif

  if( NULL == cnf_ptr )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_mmgsdi_mip_resp_cb: cnf_ptr NULL");

    (void) rex_set_sigs( ps_tcb_ptr, 1 << (int)PS_UIM_CMD_SIGNAL );
    return;
  }

  compute_ip_cnf_ptr =
    (mmgsdi_compute_ip_auth_cnf_type *) &(cnf_ptr->compute_ip_cnf);

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
    compute_ip_cnf_ptr->response_header.client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                    "ds3g_mmgsdi_mip_resp_cb: Invalid client Id [%d] ",
                    compute_ip_cnf_ptr->response_header.client_id);

    (void) rex_set_sigs( ps_tcb_ptr, 1 << (int)PS_UIM_CMD_SIGNAL );

    return;
  }
  if ( ( MMGSDI_COMPUTE_IP_AUTH_CNF == cnf ) &&
       ( MMGSDI_COMPUTE_IP_AUTH_CNF ==
         compute_ip_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS ==
           compute_ip_cnf_ptr->response_header.mmgsdi_status )
       )
    {
      /* Initialize the MIP info table */
      memset( ds3g_mmgsdi_mip_info.mip_response,
              0,
              UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH );
    /*-----------------------------------------------------------------------
      The compute_ip was successful.  This copies the data from
      the mmgsdi response
    -----------------------------------------------------------------------*/
      if ( compute_ip_cnf_ptr->cmpt_ip_response_data.data_len <=
           UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH )
      {
        memscpy( ds3g_mmgsdi_mip_info.mip_response,
                 UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH,
                 compute_ip_cnf_ptr->cmpt_ip_response_data.data_ptr,
                 (unsigned int)compute_ip_cnf_ptr->cmpt_ip_response_data.data_len );
      }
      else
      {
        memscpy( ds3g_mmgsdi_mip_info.mip_response,
                 UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH,
                 compute_ip_cnf_ptr->cmpt_ip_response_data.data_ptr,
                 UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH );
      }

      ds3g_mmgsdi_mip_info.status = TRUE;
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
           "Command failure status [%d] and mmgsdi_status [%d] ",
           status, compute_ip_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
         " Improper command confirmation cnf [%d] response_type [%d]",
         cnf, compute_ip_cnf_ptr->response_header.response_type);
  }

  /*-------------------------------------------------------------------------
    Set the signal to indicate to the PS task that the
    uim command has completed
  -------------------------------------------------------------------------*/
  (void) rex_set_sigs( ps_tcb_ptr, 1 << (int)PS_UIM_CMD_SIGNAL );

  return;
} /* ds3g_mmgsdi_mip_resp_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_MIP_DATA_RETRIEVE

DESCRIPTION
  Retrieves the MN-AAA/RRQ/MN-HA Response from the mmgsdi response saved into
  mip table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_mip_data_retrieve
(
  void  *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the pointer is not NULL */
  if ( data_ptr )
  {
    /* copy info from storage to external data pointer */
    memscpy( data_ptr,UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH,
             ds3g_mmgsdi_mip_info.mip_response,
             UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH );
  }
  return;
} /* ds3g_mmgsdi_mip_resp_cb */
#endif /* FEATURE_CDMA */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_VALIDATE_CLIENT_ID

DESCRIPTION
  Validate provided client id with client id getting from MMGSDI interface.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Valid client id
  FALSE: Invalid client id

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3g_mmgsdi_validate_client_id
(
  mmgsdi_client_id_type  client_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (( client_id == ds3g_mmgsdi_info.client_id ) &&
      ( client_id != DS3G_MMGSDI_INVALID_CLIENT_ID ))
  {
    return TRUE;
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid client Id:%d", client_id);
    return FALSE;
  }
} /* ds3g_mmgsdi_validate_client_id */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SESSION_ID

DESCRIPTION
  Get CDMA/UMTS session id getting from MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  session id

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3g_mmgsdi_get_session_id
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_session_id_type         *session_id_ptr
)
{
  uint8    i;
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if (( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
          == session_type ) &&
        ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].is_operational ))
    {
      *session_id_ptr = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id;
      ret_val = TRUE;
      break;
    }
  }

  if(!ret_val)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "session_type:%d not found",
                    session_type);
  }

  return ret_val;
} /* ds3g_mmgsdi_get_session_id */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_APP_TYPE

DESCRIPTION
  Get session app type from ds3g_mmgsdi_info based on session_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully getting app type from ds3g_mmgsdi_info
  FALSE: Failed to get app type from ds3g_mmgsdi_info

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3g_mmgsdi_get_app_type
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_app_enum_type           *app_type_ptr
)
{
  uint8    i;
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if (( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
          == session_type ) &&
        ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].is_operational ))
    {
      *app_type_ptr = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].app_type;
      ret_val = TRUE;
      break;
    }
  }

  if(!ret_val)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "session_type:%d not found",
                    session_type);
  }

  return ret_val;
} /* ds3g_mmgsdi_get_app_type */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SLOT

DESCRIPTION
  Get slot info from ds3g_mmgsdi_info based on session_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully getting slot info from ds3g_mmgsdi_info
  FALSE: Failed to get slot info from ds3g_mmgsdi_info

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3g_mmgsdi_get_slot
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_slot_id_enum_type       *slot_ptr
)
{
  uint8  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if (( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
          == session_type ) &&
        ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].is_operational ))
    {
      *slot_ptr = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].slot;
      return TRUE;
    }
  }
  return FALSE;
} /* ds3g_mmgsdi_get_slot */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SUB_ID_FROM_SLOT

DESCRIPTION
  This function returns the subs id based on slot.

PARAMETERS
  slot : mmgsdi session slot.

RETURN VALUE
  sub_id : sub_id corresponding to the given session_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_mmgsdi_get_sub_id_from_slot
(
  mmgsdi_slot_id_enum_type  slot
)
{
  uint8  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].slot == slot &&
         ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].is_operational == TRUE)
    {
      return ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].sub_id;
    }
  }
  return SYS_MODEM_AS_ID_NONE;
} /* ds3g_mmgsdi_get_sub_id_from_slot */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SESSION_TYPE

DESCRIPTION
  Get CDMA/UMTS session type based on session id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Getting session type successfully
  FALSE: Failed to get session type

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_get_session_type
(
  mmgsdi_session_id_type          session_id,
  mmgsdi_session_type_enum_type  *session_type_ptr
)
{
  uint8   i;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( session_type_ptr == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed");
    return ret_val;
  }

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if (( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id
          == session_id ) &&
        ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].is_operational ))
    {
      *session_type_ptr
        = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type;
      ret_val = TRUE;
      break;
    }
  }

  if(!ret_val)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "session_id:%d not found", session_id);
  }

  return ret_val;
} /* ds3g_mmgsdi_get_session_type */

#ifdef FEATURE_EHRPD
/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_ACL

DESCRIPTION
  Get APN Control list afer receiving Subscription Ready Events from GW sessions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_read_acl
(
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_access_type       acl_access;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return FALSE;
  }

  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Sending Request for Reading ACL");

  /* Get ACL from USIM */
  acl_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  acl_access.file.file_enum = MMGSDI_USIM_ACL;

  /* Read the data */
  mmgsdi_status = ds3g_mmgsdi_read_transparent( acl_access,
                                                0, // Read from start of the file.
                                                0, // 0 for entire file.
                                                ds3g_mmgsdi_acl_resp_cb,
                                                session_type );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Failed to read ACL using session type [%d], status [%d]",
                    session_type, mmgsdi_status);

    /*------------------------------------------------------------------
      We can't proceed for ACL read, invoke ACL processing finish
      handler.
    --------------------------------------------------------------------*/
    ds3g_mmgsdi_acl_process_done();

    return FALSE;
  }

  return TRUE;
}/* ds3g_mmgsdi_read_acl */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_IF_ACL_ENABLED_READ_ACL

DESCRIPTION
  Checks if ACL service is enabeld in UST/EST on card, before reading ACL

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_if_acl_enabled_read_acl
(
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check client id */
  if ( ds3g_mmgsdi_info.client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Client ID [%d]",
                    ds3g_mmgsdi_info.client_id);
    return FALSE;
  }

  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "Sending Request for App Capabilities");

  mmgsdi_status = ds3g_mmgsdi_session_get_app_capabilities( session_type,
                                          ds3g_mmgsdi_if_acl_enabled_read_acl_cb,
                                          (mmgsdi_client_data_type) 0 );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Failed to request app capabilities using "
                    "session type [%d], status [%d]",
                    session_type, mmgsdi_status);

    /*------------------------------------------------------------------
      We can't proceed for ACL read, invoke ACL processing finish
      handler.
    --------------------------------------------------------------------*/
    ds3g_mmgsdi_acl_process_done();

    return FALSE;
  }

  return TRUE;
}/* ds3g_mmgsdi_if_acl_enabled_read_acl */

/*===========================================================================
FUNCTION DS3G_MMGSDI_ACL_RESP_CB

DESCRIPTION
  Gets ACL data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_acl_resp_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_read_cnf_type              *read_cnf_ptr = NULL;
  ds_cmd_type                       *cmd_buf;
  ds707_data_session_acl_info_type  *ds707_acl_info = NULL;
  ds3g_mmgsdi_event_info_type       *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == cnf_ptr )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_mmgsdi_get_acl_cb: cnf_ptr NULL");
    return;
  }

  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
        read_cnf_ptr->response_header.client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_get_acl_cb: Invalid client Id [%d] ",
                    read_cnf_ptr->response_header.client_id);
    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
       ( MMGSDI_READ_CNF ==
         read_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS == read_cnf_ptr->response_header.mmgsdi_status ) )
    {
      /*-------------------------------------------------------------------------
        If ACL is already populated and read ACL is called again, for REFRESH event,
        then clear & Invalidate previous ACL information.
        TODO:: This will change in future. If the refresh event occurs during
        data call then while updating ACL, make sure that
        data session profile module doesn't read inconsistent data.
      -------------------------------------------------------------------------*/
      if ( ds3g_mmgsdi_acl_info.acl_response != NULL )
      {
        modem_mem_free( ds3g_mmgsdi_acl_info.acl_response,
                        MODEM_MEM_CLIENT_DATA_CRIT );
        ds3g_mmgsdi_acl_info.acl_response_len = 0;
        ds707_acl_info = ds707_data_session_get_acl_info_ptr();
        ds707_acl_info->num_apn = -1;
      }

      /* Allocate & Initialize the ACL response array */
      ds3g_mmgsdi_acl_info.acl_response =
        modem_mem_alloc( read_cnf_ptr->read_data.data_len,
                         MODEM_MEM_CLIENT_DATA_CRIT );

      if ( ds3g_mmgsdi_acl_info.acl_response == NULL )
      {
        DATA_MSG2(MSG_LEGACY_FATAL,
                  "Heap Memory Allocation failed for client: %d size: %d",
                  MODEM_MEM_CLIENT_DATA, read_cnf_ptr->read_data.data_len);
        return;
      }

      memset( ds3g_mmgsdi_acl_info.acl_response,
              0,
              read_cnf_ptr->read_data.data_len );
    /*-----------------------------------------------------------------------
      The reading from ACL efs was successful.  This copies the data from
      the mmgsdi response
    -----------------------------------------------------------------------*/
     memscpy( ds3g_mmgsdi_acl_info.acl_response,
              read_cnf_ptr->read_data.data_len,
              read_cnf_ptr->read_data.data_ptr,
              read_cnf_ptr->read_data.data_len );

     /*-----------------------------------------------------------------------
       Store response length in ds3g_mmgsdi_acl_info
     -----------------------------------------------------------------------*/
      ds3g_mmgsdi_acl_info.acl_response_len =
         read_cnf_ptr->read_data.data_len;

      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;

      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_ACL_INFO_EVENT;
      // Session Id
      mmgsdi_event_info_ptr->session_id
        = cnf_ptr->read_cnf.response_header.session_id;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Command failure status [%d] and mmgsdi_status [%d] ",
                      status, read_cnf_ptr->response_header.mmgsdi_status);

      /*------------------------------------------------------------------
        ACL read failed, Possible reason - ACL not configured/enabled on
        card. Invoke ACL finish processing handler.
      --------------------------------------------------------------------*/
      ds3g_mmgsdi_acl_process_done();
    }
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                    "Improper command confirmation cnf [%d] response_type [%d]",
                    cnf, read_cnf_ptr->response_header.response_type);
  }
  return;
} /* ds3g_mmgsdi_acl_resp_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_IF_ACL_ENABLED_READ_ACL_CB

DESCRIPTION
  Gets ACL service info from the mmgsdi response, and copies into global var
  This is a callback function which is called from the mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_if_acl_enabled_read_acl_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_session_app_capabilities_cnf_type  *app_capabilities_cnf_ptr = NULL;
  mmgsdi_session_type_enum_type              session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_session_id_type                     session_id;
  ds_cmd_type                               *cmd_buf = NULL;
  ds3g_mmgsdi_event_info_type               *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == cnf_ptr )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_get_app_capabilities_cb: cnf_ptr NULL");
    return;
  }

  app_capabilities_cnf_ptr = (mmgsdi_session_app_capabilities_cnf_type *)
                              &(cnf_ptr->session_app_capabilities_cnf);

  /*-------------------------------------------------------------------------
    Check client id
  -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
        app_capabilities_cnf_ptr->response_header.client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_get_app_capabilities_cb: Invalid client Id:%d",
                    app_capabilities_cnf_ptr->response_header.client_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Get session type
  -------------------------------------------------------------------------*/
  session_id = app_capabilities_cnf_ptr->response_header.session_id;
  if ( ds3g_mmgsdi_get_session_type( session_id,&session_type )
         != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an Activated Session Id:%d from the table",
                    session_id);
    return;
  }

  if ( ( MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF == cnf ) &&
       ( MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF ==
         app_capabilities_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
         ( MMGSDI_SUCCESS ==
           app_capabilities_cnf_ptr->response_header.mmgsdi_status ) )
    {
      /* Set ACL enable/disable flag */
      ds3g_mmgsdi_acl_info.acl_is_enabled =
                  app_capabilities_cnf_ptr->acl_enabled;

      /* Switch from MMGSDI context to DS context for processing */
      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if ( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;

      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_ACL_ENABLED_EVENT;
      mmgsdi_event_info_ptr->session_id
        = app_capabilities_cnf_ptr->response_header.session_id;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Command failure status [%d] and mmgsdi_status [%d]",
                      status,
                      app_capabilities_cnf_ptr->response_header.mmgsdi_status);

      /*------------------------------------------------------------------
        Get app capabilities failed, ACL is not read
      --------------------------------------------------------------------*/
      ds3g_mmgsdi_acl_process_done();
    }
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                    "Improper command confirmation cnf [%d] response_type [%d]",
                    cnf,
                    app_capabilities_cnf_ptr->response_header.response_type);
  }
  return;
} /* ds3g_mmgsdi_if_acl_enabled_read_acl_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_PARSE_ACL

DESCRIPTION
  Based on content getting from MMGSDI_USIM_ACl Parse the APN control list
  and store it in to an internal data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_parse_acl
(
  void
)
{
  uint8                              num_apn = 0;
  uint8                              apn_counter = 0;
  char                              *apn_name = NULL;
  char                              *encoded_apn_name = NULL;
  uint8                              apn_len;
  int8                               decoded_apn_len;
  uint16                             offset = 0;
  ds707_data_session_acl_info_type  *ds707_acl_info = NULL;
  boolean                            parse_error = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_acl_info = ds707_data_session_get_acl_info_ptr();

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Parsing APN Control List");

  if( ds3g_mmgsdi_acl_info.acl_response == NULL )
  {
    DATA_MSG0(MSG_LEGACY_FATAL, "ds3g_mmgsdi_parse_acl(): "
               "Unexpected ds3g_mmgsdi_acl_info.acl_response is NULL ");

    return;
  }

  /*-------------------------------------------------------------------------
     If ACL is already populated, then clear the old values, and reparse
     new information. This may happen during refresh case.
  -------------------------------------------------------------------------*/
  if( ds707_acl_info->num_apn > 0 )
  {
    num_apn = ds707_acl_info->num_apn;
    for( apn_counter = 0;
         apn_counter < num_apn && apn_counter < DATA_SESSION_MAX_APN_NUM;
         apn_counter++ )
    {
      if( NULL != ds707_acl_info->profile_apn_list[apn_counter].apn_name )
      {
        modem_mem_free(
          ds707_acl_info->profile_apn_list[apn_counter].apn_name,
          MODEM_MEM_CLIENT_DATA_CRIT );

        ds707_acl_info->profile_apn_list[apn_counter].apn_name = NULL;
        ds707_acl_info->profile_apn_list[apn_counter].apn_len = 0;
      }
    }
    ds707_acl_info->num_apn = 0;
  }
  /*-------------------------------------------------------------------------
    Format of ACL (APN Control List):

    +-----------+
    |Num of APNs|
    |  8 bits   |
    +-----------+
    +-----------+------------+---------------+---------------+---------------
    | Type="DD" | APN length | Label1 length |    Label1     | Label2, etc..
    |  8 Bits   |   8 Bits   |     8 Bits    | variable size |
    +-----------+------------+---------------+---------------+---------------
    +-----------+------------+---------------+---------------+---------------
    | Type="DD" | APN length | Label1 length |    Label1     | Label2, etc..
    |  8 Bits   |   8 Bits   |     8 Bits    | variable size |
    +-----------+------------+---------------+---------------+---------------
  ---------------------------------------------------------------------------*/
/*lint -save -e661 suppressing error 661*/
  if( ds3g_mmgsdi_acl_info.acl_response_len > 0 )
  {
    /* Extract number of APNs on ACL */
    num_apn = ds3g_mmgsdi_acl_info.acl_response[offset];
    offset += sizeof(uint8);

    ds707_acl_info->num_apn = num_apn;

    for( apn_counter = 0; apn_counter < num_apn; apn_counter++ )
    {
      if( apn_counter >= DATA_SESSION_MAX_APN_NUM )
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                        "Not enough space to store all APNs, Storing first %d "
                        "APNs only",
                        DATA_SESSION_MAX_APN_NUM);

        ds707_acl_info->num_apn = DATA_SESSION_MAX_APN_NUM;

        break;
      }

      /* Extract type field */
      if( ds3g_mmgsdi_acl_info.acl_response[offset] != 0xDD )
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                        "APN TLV has invalid Tag/Type Value: 0x%x, should be "
                        "0xDD to indicate start of new APN",
                        ds3g_mmgsdi_acl_info.acl_response[offset]);
        parse_error = TRUE;
        break;
      }
      offset += sizeof(uint8);

      /* Extract length field */
      apn_len = ds3g_mmgsdi_acl_info.acl_response[offset];
      offset += sizeof(uint8);

      if ( 0 == apn_len )
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "(%d) Network Provided APN",
                        apn_counter);

        apn_name = modem_mem_alloc( 1 , MODEM_MEM_CLIENT_DATA_CRIT );
        if ( NULL != apn_name )
        {
          apn_name[0] = 0;

          ds707_acl_info->profile_apn_list[apn_counter].apn_len = 0;
          ds707_acl_info->profile_apn_list[apn_counter].apn_name = apn_name;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                          "Memory could not be allocated for apn_name");
          parse_error = TRUE;
          break;
        }
        continue;
      }
      else if( (apn_len + offset) > ds3g_mmgsdi_acl_info.acl_response_len )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                        "Length of APN name exceeds acl_response buffer");
        parse_error = TRUE;
        break;
      }

      encoded_apn_name =
              modem_mem_alloc( apn_len , MODEM_MEM_CLIENT_DATA_CRIT );
      apn_name =
              modem_mem_alloc( apn_len , MODEM_MEM_CLIENT_DATA_CRIT );

      if( encoded_apn_name == NULL || apn_name == NULL )
      {
        DATA_MSG0(MSG_LEGACY_FATAL,
                  "memory allocation failure for APN");

        modem_mem_free( encoded_apn_name, MODEM_MEM_CLIENT_DATA_CRIT );
        modem_mem_free( apn_name, MODEM_MEM_CLIENT_DATA_CRIT );

        return;
      }

      memscpy( encoded_apn_name,apn_len,
               &ds3g_mmgsdi_acl_info.acl_response[offset],
               apn_len );

      decoded_apn_len = ds3g_mmgsdi_decode_APN(encoded_apn_name, apn_name, apn_len);
      modem_mem_free( encoded_apn_name, MODEM_MEM_CLIENT_DATA_CRIT );
      encoded_apn_name = NULL;

      if ( -1 == decoded_apn_len )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                        "Incorrect format of encoded APN from ACL");
        parse_error = TRUE;

        modem_mem_free( apn_name, MODEM_MEM_CLIENT_DATA_CRIT );
        apn_name = NULL;

        break;
      }
      offset += decoded_apn_len;

      /* Stored apn_len should NOT take '/0' into consideration */
      ds707_acl_info->profile_apn_list[apn_counter].apn_len = decoded_apn_len-1;
      ds707_acl_info->profile_apn_list[apn_counter].apn_name = apn_name;
      DATA_3GMGR_MSG_SPRINTF_2(MSG_LEGACY_MED, " (%d) APN Name: %s ",
                               apn_counter, apn_name);

    } /* end for */

    /* Free any memory allocated for apn_name if there was a parse error */
    if( parse_error )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "ds3g_mmgsdi_parse_acl(): "
                      "APN control list parse error, freeing all stored APN "
                      "names. No calls allowed.");
      if( ds707_acl_info->num_apn > 0 )
      {
        num_apn = ds707_acl_info->num_apn;
        for( apn_counter = 0;
             apn_counter < num_apn && apn_counter < DATA_SESSION_MAX_APN_NUM;
             apn_counter++ )
        {
          if( NULL != ds707_acl_info->profile_apn_list[apn_counter].apn_name )
          {
            modem_mem_free(
              ds707_acl_info->profile_apn_list[apn_counter].apn_name,
              MODEM_MEM_CLIENT_DATA_CRIT );

            ds707_acl_info->profile_apn_list[apn_counter].apn_name = NULL;
            ds707_acl_info->profile_apn_list[apn_counter].apn_len = 0;
          }
        }
        ds707_acl_info->num_apn = 0;
      }
    }
  } /* end if */
/*lint -restore Restore lint error 661*/

  ds3g_mmgsdi_acl_process_done();

  return;
}/* ds3g_mmgsdi_parse_acl */

/*===========================================================================
FUNCTION DS3G_MMGSDI_DECODE_APN

DESCRIPTION
  Decodes ACL file that is returned from the card, and formats APN name to
  what is expected.

  Encoded Format: <label1_len><label1><label2_len><label2><etc...>
  Decoded Format: <label1>.<label2>.<etc...>

  Example:
    Encoded Format (in hex) -
      08 69 70 76 34 74 65 73 74 07 71 75 61 6C 6E 65 74 03 63 6F 6D
    Decoded Format (in ascii) -
      ipv4test.qualnet.com

DEPENDENCIES
  None

RETURN VALUE
  Length of APN string
  -1 on error

SIDE EFFECTS
  None
===========================================================================*/
int ds3g_mmgsdi_decode_APN
(
  char   *encoded,
  char   *decoded,
  uint8   apn_len
)
{
  int label_len, src, dst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  src=0;
  dst=0;

  if( 0 == apn_len )
  {
    return -1;
  }

  while(src < apn_len)
  {
    label_len = encoded[src];
    src++;
    if( label_len + src > apn_len )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                      "Incorrect provisioning of APN, next label_len exceeds "
                      "total apn_len");
      return -1;
    }
    memscpy(&decoded[dst], apn_len, &encoded[src], label_len);
    src += label_len;
    dst += label_len;

    decoded[dst++] = '.';
  }

  decoded[dst-1] = '\0';
  return dst;
}/* ds3g_mmgsdi_decode_APN */

/*===========================================================================
FUNCTION DS3G_MMGSDI_ACL_PROCESS_DONE

DESCRIPTION
  Take appropirate action after ACL processing is over.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_acl_process_done
(
  void
)
{
  ds707_data_session_acl_info_type  *ds707_acl_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_acl_info = ds707_data_session_get_acl_info_ptr();
  /*--------------------------------------------------------------------------
    Inidicate that mmgsdi processing for ACL is finished by setting
    ds3g_mmgsdi_init flag.
  ----------------------------------------------------------------------------*/
  ds707_acl_info->ds3g_mmgsdi_init = TRUE;

  /*--------------------------------------------------------------------------
    If both data session profile & ds3g mmgsdi initialization is complete,
    then call MIN APN check in data session profile.
  ----------------------------------------------------------------------------*/
  #ifdef FEATURE_EHRPD
  #ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  if (ds3g_get_cm_client_init_status())
  #endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
  {
  if ( ds707_acl_info->data_session_profile_init == TRUE &&
       ds707_acl_info->ds3g_mmgsdi_init == TRUE )
  {
    /*---------------------------------------------------------------------
      Check if mandatory APNs are enabled in profile and uicc card.
    ---------------------------------------------------------------------*/
    ds707_data_session_check_mandatory_apns();
  }
  }
  #endif /*FEATURE_EHRPD*/

  return;
}/* ds3g_mmgsdi_acl_process_done */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_ACL_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_ACL_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_acl_info_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if ( ds3g_mmgsdi_get_session_type( session_id, &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                    "Failed to find an Activated Session Id:%d from the table",
                    session_id);
    return;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Parse ACL info using Session Type [%d], Session ID [%d]",
                  session_type, session_id);


  /* Convert ACL content from EFS format and store it in internal data structure */
  ds3g_mmgsdi_parse_acl();
  return;
}/* ds3g_mmgsdi_acl_info_event_process */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_ACL_ENABLED_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_ACL_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_acl_enabled_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_session_id_type         session_id   = DS3G_MMGSDI_INVALID_SESSION_ID;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  /* check session type */
  session_id = mmgsdi_event_info_ptr->session_id;
  if ( ds3g_mmgsdi_get_session_type( session_id,
                                     &session_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an Activated Session Id:%d from the table",
                    session_id);
    return;
  }

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "Check ACL enabled flag, invoke ACL read if ACL enabled");

  /* Invoke ACL read, if ACL service is enabled */
  if( ds3g_mmgsdi_acl_info.acl_is_enabled == TRUE )
  {
    /* Read ACL from USIM */
    ( void )ds3g_mmgsdi_read_acl( session_type );
  }
  else
  {
    ds3g_mmgsdi_acl_process_done();
  }
  return;
}/* ds3g_mmgsdi_acl_enabled_event_process */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_ACL_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register GW UMTS for File Change Notifications for ACL's.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void ds3g_mmgsdi_acl_register_for_refresh
(
  mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type  refresh_files = {0};
  mmgsdi_file_enum_type          file_list[DS3G_MMGSDI_MAX_NUM_REFRESH_FILES];
  mmgsdi_app_enum_type           app_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get application type */
  if ( ds3g_mmgsdi_get_app_type( session_type, &app_type ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to find an activated Session Type = [%d]",
                    session_type);
    return;
  }
  /* Issue a read command to read the SVC TABLE through MMGSDI API */
  if ( app_type == MMGSDI_APP_USIM )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Registers UMTS file change from USIM");
    file_list[0] = MMGSDI_USIM_ACL; /* EF for Access Point Name Control List */
    refresh_files.num_files = 1;
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Could not register UMTS file change from app type [%d]",
                    app_type);
    return;
  }
  refresh_files.file_list_ptr = &file_list[0];


  DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                  "Sending request for file change notification");

  /* Register with MMGSDI for File Change Notification using the file list
    in the array above   */
  mmgsdi_status = ds3g_mmgsdi_register_for_refresh( refresh_files,
                                                    session_type );

  /* Check to see if REQ for Registration was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Failed to Request Refresh FCN Registration");
  }
} /* ds3g_mmgsdi_acl_register_for_refresh */
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION DS3G_MMGSDI_UPDATE_SUB_ID

DESCRIPTION
  Update subscription id into mmgsdi table

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to update subscription id
  TRUE : Update subscription id successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_update_sub_id
(
  mmgsdi_session_type_enum_type  session_type,
  sys_modem_as_id_e_type         sub_id
)
{
  uint8                   i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
          == session_type )
    {
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].sub_id = sub_id;
      DATA_3GMGR_MSG3( MSG_LEGACY_LOW,
                      "Subscription ID stored. Sub_id = %d, "
                      "Session_id = %d, session_type = %d",
                      sub_id,
                      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id,
                      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_type
                     );

        if(ds3g_mmgsdi_set_and_notify_subscription_ready_status(
             DS3G_MMGSDI_SUBS_ID_READY_MASK,session_type)
           == FALSE)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                          "cannot set DS3G_MMGSDI_SUBS_ID_READY_MASK");
        }

      if(ds3g_mmgsdi_subs_ready_to_process_usim(
         DS3G_MMGSDI_USIM_SUBS_ID_READY_MASK, session_type)
         == FALSE)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "subs not ready to process usim session");
      }
      return TRUE;
    }
  }
  return FALSE;
}
/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_ID_FROM_SUB_ID

DESCRIPTION
  Get session id from mmgsdi table provided subscription id

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to get session id
  TRUE: Get session id successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_session_id_from_sub_id
(
  sys_modem_as_id_e_type   sub_id,
  mmgsdi_session_id_type  *session_id_ptr
)
{
  uint8    i;
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if (( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].sub_id == sub_id ) &&
        ( ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].is_operational ))
    {
      *session_id_ptr = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id;
      ret_val = TRUE;
      break;
    }
  }

  if( !ret_val )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                "ds3g_mmgsdi_get_session_id_from_sub_id, sub_id:%d not found",
                sub_id);
  }

  return ret_val;
} /* ds3g_mmgsdi_get_session_id_from_sub_id */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SUB_ID_FROM_SESSION_ID

DESCRIPTION
  Get subs id from mmgsdi table provided session type

DEPENDENCIES
  None

RETURN VALUE
  SUBS ID

SIDE EFFECTS
  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_mmgsdi_get_sub_id_from_session_id
(
  mmgsdi_session_id_type  session_id
)
{
  uint8                   i;
  sys_modem_as_id_e_type  result = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( session_id ==
         ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].session_id )
    {
      result = ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].sub_id;
    }
  }
  return result;
}/* ds3g_mmgsdi_get_sub_id_from_session_id */

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DS3G_MMGSDI_REGISTER_FOR_SUBSCRIPTION_READY

DESCRIPTION
  This function notifies the callback for when subscription is ready

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_register_for_subscription_ready
(
  ds3g_mmgsdi_subscription_ready_cb  *callback_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_mmgsdi_on_subscription_ready = callback_ptr;
} /* ds3g_mmgsdi_register_for_subscription_ready */
#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_GET_IMPI_INFO_FROM_SLOT

DESCRIPTION
  This function fetches the IMPI info based on Slot Id

PARAMETERS
  Slot

DEPENDENCIES
  None.

RETURN VALUE
  IMPI Info Ptr

SIDE EFFECTS
  None.
===========================================================================*/
ds3g_mmgsdi_impi_info_type* ds3g_mmgsdi_get_impi_info_from_slot
(
  mmgsdi_slot_id_enum_type  slot_id
)
{
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subs_id = ds3g_mmgsdi_get_sub_id_from_slot(slot_id);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return NULL;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_LOW,"Subs Id: %d,Slot Id: %d,IMPI Info: %0xx",
                  subs_id, slot_id, &ds3g_mmgsdi_impi_info[subs_id]);

  return &ds3g_mmgsdi_impi_info[subs_id];
} /* ds3g_mmgsdi_get_impi_info_from_slot */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_GET_IMSI_INFO_FROM_SESSION_ID

DESCRIPTION
  This function fetches the IMSI info based on Session Id

PARAMETERS
  Session Id

DEPENDENCIES
  None.

RETURN VALUE
  IMSI Info Ptr

SIDE EFFECTS
  None.
===========================================================================*/
ds3g_mmgsdi_imsi_info_type* ds3g_mmgsdi_get_imsi_info_from_session_id
(
  mmgsdi_session_id_type  session_id
)
{
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subs_id = ds3g_mmgsdi_get_sub_id_from_session_id(session_id);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return NULL;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_MED,"Subs Id: %d,Session Id: %d,IMSI Info: %0xx",
                  subs_id, session_id, &ds3g_mmgsdi_imsi_info[subs_id]);

  return &ds3g_mmgsdi_imsi_info[subs_id];
} /* ds3g_mmgsdi_get_imsi_info_from_session_id */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_GET_IMSI_INFO_FROM_SESSION_ID

DESCRIPTION
  This function fetches the IMSI info based on Session Type

PARAMETERS
  Session Type

DEPENDENCIES
  None.

RETURN VALUE
  IMSI Info Ptr

SIDE EFFECTS
  None.
===========================================================================*/
ds3g_mmgsdi_imsi_info_type* ds3g_mmgsdi_get_imsi_info_from_session_type
(
  mmgsdi_session_type_enum_type  session_type
)
{
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return NULL;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                  "Subs Id: %d,Session_type: %d,IMSI Info: 0x%x",
                  subs_id, session_type, &ds3g_mmgsdi_imsi_info[subs_id]);

  return &ds3g_mmgsdi_imsi_info[subs_id];
} /* ds3g_mmgsdi_get_imsi_info_from_session_type */

/*===========================================================================
FUNCTION DS3G_MMGSDI_READ_IMSI_FROM_SESSION_TYPE

DESCRIPTION
  This function reads the IMSI info from SIM only when both subscription
  ready and CM subs_id from phone event are received.

PARAMETERS
  Session Type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_read_imsi_from_session_type
(
  mmgsdi_session_type_enum_type session_type
)
{
  uint8                  index = 0;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3g_mmgsdi_get_session_type_idx(session_type,&index) == FALSE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session info index for session type [%d], "
                    "skipping imsi read",
                    session_type);

    return;
  }

  subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);

  if((ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].is_operational == TRUE &&
     ds3gsubsmgr_is_subs_id_valid(subs_id) == TRUE) &&
     (session_type == MMGSDI_GW_PROV_PRI_SESSION ||
      session_type == MMGSDI_GW_PROV_SEC_SESSION
#ifdef FEATURE_TRIPLE_SIM
      || session_type == MMGSDI_GW_PROV_TER_SESSION
#endif /* FEATURE_TRIPLE_SIM */
     ))
  {
    if(ds3g_mmgsdi_read_imsi(session_type) == FALSE)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Failed to read imsi info for session type [%d]",
                      session_type);
    }
  }

  return;
} /* ds3g_mmgsdi_read_imsi_from_session_type() */

/*===========================================================================
FUNCTION DS3G_MMGSDI_READ_IMSI_FROM_SESSION_TYPE

DESCRIPTION
  This function reads the IMSI info from SIM only when both subscription
  ready and CM subs_id from phone event are received.

PARAMETERS
  Session Type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_read_impi_from_non_prov_session_type
(
  mmgsdi_session_type_enum_type  non_prov_session_type
)
{
  uint8                          non_prov_idx = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3g_mmgsdi_get_non_prov_isim_session_type_idx(
                                        non_prov_session_type,
                                        &non_prov_idx) == FALSE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session info index for session type [%d], "
                    "skipping impi read",
                    non_prov_session_type);

    return;
  }

  if(ds3g_mmgsdi_isim_info[non_prov_idx].is_operational == TRUE &&
     (non_prov_session_type == MMGSDI_NON_PROV_SESSION_SLOT_1 ||
      non_prov_session_type == MMGSDI_NON_PROV_SESSION_SLOT_2
#ifdef FEATURE_TRIPLE_SIM
      || non_prov_session_type == MMGSDI_NON_PROV_SESSION_SLOT_3
#endif /* FEATURE_TRIPLE_SIM */
     ))
  {
    if(ds3g_mmgsdi_read_impi(non_prov_session_type) == FALSE)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Failed to read impi info for session type [%d]",
                      non_prov_session_type);
    }
  }

  return;
} /* ds3g_mmgsdi_read_impi_from_non_prov_session_type() */

/*===========================================================================
FUNCTION ds3gpp_mmgsdi_is_reset_cc_sp_throttle_required

DESCRIPTION
  A command processing function is called based on the type of command received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_reset_cc_sp_throttle_if_required
(
  ds_cmd_type  *cmd_ptr
)
{
  uint8                         index;
  boolean                       reset = FALSE ;
  ds_cmd_type                   *post_cmd_ptr = NULL;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  sys_modem_as_id_e_type        *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
     ASSERT(0);
     return;
  }

  mmgsdi_event_info_ptr
    = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "ds3gpp_mmgsdi_is_reset_cc_sp_throttle_required %d",
                  mmgsdi_event_info_ptr->event_type);

  for ( index = 0; index < MAX_DS3G_MMGSDI_SESSIONS; index++ )
  {
    if( mmgsdi_event_info_ptr->slot ==
        ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].slot )
    {
      if ( ds3g_get_ps_subs_id() ==
           ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].sub_id )
      {
        reset = TRUE;
        break;
      }
    }
  }

  if(reset == TRUE)
  {
    post_cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
    if( (post_cmd_ptr == NULL) || (post_cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }

    post_cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_THROT_RESET;
    data_ptr = (sys_modem_as_id_e_type*)post_cmd_ptr->cmd_payload_ptr;

    *data_ptr = ds3g_get_ps_subs_id();

    ds_put_cmd(post_cmd_ptr);
  }
  return;
} /* ds3g_mmgsdi_reset_cc_sp_throttle_if_required() */


/*===========================================================================
FUNCTION DS3G_MMGSDI_ISIM_OK_REFRESH_EVENT_PROCESS

DESCRIPTION
This function processes event when receiving MMGSDI_REFRESH_EVT with
MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN from MMGSDI interface.
This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_isim_ok_refresh_event_process
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  uint8                         i = 0;
  ds3g_mmgsdi_event_info_type  *ds_mmgsdi_event_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_isim_ok_refresh_event_process");

  if(cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  ds_mmgsdi_event_info = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  for ( i = 0; i < MAX_DS3G_MMGSDI_NON_PROV_SESSIONS; i++ )
  {
    if ((ds_mmgsdi_event_info->session_id ==
          ds3g_mmgsdi_isim_info[i].session_id) &&
        (ds_mmgsdi_event_info->slot ==
          ds3g_mmgsdi_isim_info[i].slot_id)
       )
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "ISIM OK to refresh with session_id [%d], session type [%d]",
                  ds3g_mmgsdi_isim_info[i].session_id,
                  ds3g_mmgsdi_isim_info[i].session_type);

      mmgsdi_status = mmgsdi_session_ok_to_refresh( 
                                          ds3g_mmgsdi_isim_info[i].session_id,
                                          TRUE,
                                          ds3g_mmgsdi_client_rsp_cb,
                                          ( mmgsdi_client_data_type ) 0 );

      /* Check to see if REQ was OK.  */
      if ( mmgsdi_status != MMGSDI_SUCCESS)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Failed to call MMGSDI OK to Refresh "
                        "[%d]", mmgsdi_status);
      }
    }
  }

  return;
} /* ds3g_mmgsdi_isim_ok_refresh_event_process */

/*===========================================================================
FUNCTION DS3G_MMGSDI_ISIM_REFRESH_START_PROCESSING

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with
  MMGSDI_REFRESH_STAGE_START from MMGSDI interface.
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_isim_refresh_start_processing
(
  ds_cmd_type  *cmd_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  uint8                         i = 0;
  ds3g_mmgsdi_event_info_type  *ds_mmgsdi_event_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_isim_refresh_start_processing");

  if(cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  ds_mmgsdi_event_info = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  for ( i = 0; i < MAX_DS3G_MMGSDI_NON_PROV_SESSIONS; i++ )
  {
    if ((ds_mmgsdi_event_info->session_id ==
          ds3g_mmgsdi_isim_info[i].session_id) &&
        (ds_mmgsdi_event_info->slot ==
          ds3g_mmgsdi_isim_info[i].slot_id)
       )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                      "Sending refresh complete for ISIM session_id %d",
                      ds3g_mmgsdi_isim_info[i].session_id);

      mmgsdi_status = mmgsdi_session_refresh_complete(
                                          ds3g_mmgsdi_isim_info[i].session_id,
                                          TRUE,
                                          ds3g_mmgsdi_client_rsp_cb,
                                          (mmgsdi_client_data_type)0);


      /* Check to see if REQ was OK.  */
      if ( mmgsdi_status != MMGSDI_SUCCESS)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Failed to call MMGSDI OK to Refresh "
                        "[%d]", mmgsdi_status);
      }
    }
  }

  return;
} /* ds3g_mmgsdi_isim_refresh_start_processing */

/*===========================================================================
FUNCTION DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_PROCESSING

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with
  MMGSDI_REFRESH_STAGE_END_SUCCESS from MMGSDI interface.
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_isim_refresh_complete_processing
(
  ds_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_DATA_LTE
  ds_cmd_type                  *post_cmd_ptr = NULL;
  sys_modem_as_id_e_type       *sub_id_ptr = NULL;
#endif /* FEATURE_DATA_LTE */
  uint8                         i = 0;
  ds3g_mmgsdi_event_info_type  *ds_mmgsdi_event_info = NULL;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "ds3g_mmgsdi_isim_refresh_complete_processing");

  if(cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  ds_mmgsdi_event_info = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  for ( i = 0; i < MAX_DS3G_MMGSDI_NON_PROV_SESSIONS; i++ )
  {
    if ((ds_mmgsdi_event_info->session_id ==
          ds3g_mmgsdi_isim_info[i].session_id) &&
        (ds_mmgsdi_event_info->slot ==
          ds3g_mmgsdi_isim_info[i].slot_id)
       )
    {
      ds3g_mmgsdi_read_impi_from_non_prov_session_type( 
                               ds3g_mmgsdi_isim_info[i].session_type );

      if (ds_mmgsdi_event_info->refresh_mode == MMGSDI_REFRESH_RESET ||
          ds_mmgsdi_event_info->refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET)
      {
        subs_id
          = ds3g_mmgsdi_get_sub_id_from_slot(ds3g_mmgsdi_isim_info[i].slot_id);

        if (ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE)
        {
          return;
        }

#ifdef FEATURE_DATA_LTE
        post_cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));

        if ( (post_cmd_ptr == NULL) || (post_cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }

        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Posting DS_3GPP_MMGSDI_UICC_3G_RESET_CMD");
        post_cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_MMGSDI_UICC_3G_RESET;
        sub_id_ptr = (sys_modem_as_id_e_type*)(post_cmd_ptr->cmd_payload_ptr);
        *sub_id_ptr = subs_id;

        ds_put_cmd(post_cmd_ptr);
#endif /* FEATURE_DATA_LTE */
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "MMGSDI refresh mode is not UICC reset of 3G reset. "
                        "Not posting DS_3GPP_MMGSDI_UICC_3G_RESET_CMD");
      }
    }
  }

  return;
} /* ds3g_mmgsdi_isim_refresh_complete_processing */

/*===========================================================================
FUNCTION DS3G_MMGSDI_OPEN_NON_PROV_ISIM_SESSION

DESCRIPTION
  Function to open non-provisioned session for ISIM after subscription is
  ready

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_open_non_prov_isim_session
(
  mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_available_apps_type    *mmgsdi_available_apps_ptr = NULL;
  mmgsdi_session_type_enum_type  non_provisioned_session
                                               = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_app_info_type          *isim_app_info = NULL;
  mmgsdi_non_prov_app_info_type  non_prov_app_reg_info = {{0}};
  mmgsdi_return_enum_type        status = MMGSDI_ERROR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "ds3g_mmgsdi_open_non_prov_isim_session for slot %d",
                  slot_id);

  mmgsdi_available_apps_ptr =
    modem_mem_alloc(sizeof(mmgsdi_available_apps_type),
                    MODEM_MEM_CLIENT_DATA_CRIT);

  if (mmgsdi_available_apps_ptr == NULL)
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                     "Could not allocate memory for mmgsdi_available_apps_ptr");
     return;
  }

  memset(mmgsdi_available_apps_ptr,0,sizeof(mmgsdi_available_apps_type));

  if(mmgsdi_get_all_available_apps_sync(ds3g_mmgsdi_info.client_id,
                                        mmgsdi_available_apps_ptr)
     != MMGSDI_SUCCESS)
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                     "Could not get avaialble apps from MMGSDI");
     modem_mem_free(mmgsdi_available_apps_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
     return;
  }

  isim_app_info = ds3g_mmgsdi_get_isim_app_ptr_from_available_apps(slot_id,
                                                     mmgsdi_available_apps_ptr);

  if(isim_app_info == NULL)
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                     "No ISIM App found");
     modem_mem_free(mmgsdi_available_apps_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
     return;
  }

  non_prov_app_reg_info.exclusive_channel = FALSE;
  non_prov_app_reg_info.app_id_data.data_len
    = isim_app_info->app_data.aid.data_len;
  non_prov_app_reg_info.app_id_data.data_ptr
    = isim_app_info->app_data.aid.data_ptr;

  non_provisioned_session =
    ds3g_mmgsdi_slot_to_non_prov_session_type(isim_app_info->slot);

  /* register event call back. Register for refresh event to receive it in this callback */
  status = mmgsdi_session_open(ds3g_mmgsdi_info.client_id,
                               non_provisioned_session,
                               &non_prov_app_reg_info,
                               ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb,
                               FALSE,
                               ds3g_mmgsdi_non_prov_isim_session_reg_cb,
                               (mmgsdi_client_data_type)0);

  if ( status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_open_non_prov_isim_session(): "
                    "Session Open Registration Failed [%d]",
                    status);
  }

  modem_mem_free(mmgsdi_available_apps_ptr, MODEM_MEM_CLIENT_DATA_CRIT);

  return;
} /* ds3g_mmgsdi_open_non_provisioned_isim_session */

/*===========================================================================
FUNCTION DS3G_MMGSDI_NON_PROV_ISIM_SESSION_REG_CB

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback for the
  non provisioned isim session. It will assign the global Session ID value
  which will be used in mmgsdi function calls

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_non_prov_isim_session_reg_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  uint8                          index = 0;
  mmgsdi_session_type_enum_type  non_prov_session = MMGSDI_MAX_SESSION_TYPE_ENUM;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cnf_ptr)
  {
    DATA_ERR_FATAL("cnf_ptr is NULL");
    return;
  }

  if (MMGSDI_SUCCESS == status)
  {
    if (MMGSDI_SESSION_OPEN_CNF == cnf)
    {
      non_prov_session =
        ds3g_mmgsdi_slot_to_non_prov_session_type(cnf_ptr->
                                                    session_open_cnf.slot_id);

      if(ds3g_mmgsdi_get_non_prov_isim_session_type_idx(non_prov_session,&index)
          == FALSE)
      {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "Could not find index in ds3g_mmgsdi_isim_info "
                       "for session type %d",
                       non_prov_session);
        return;
      }

      ds3g_mmgsdi_isim_info[index].is_operational = TRUE;
      ds3g_mmgsdi_isim_info[index].app_info =
                                         cnf_ptr->session_open_cnf.app_info;
      ds3g_mmgsdi_isim_info[index].channel_id =
                                         cnf_ptr->session_open_cnf.channel_id;
      ds3g_mmgsdi_isim_info[index].session_id =
                                         cnf_ptr->session_open_cnf.session_id;
      ds3g_mmgsdi_isim_info[index].slot_id =
                                         cnf_ptr->session_open_cnf.slot_id;

      DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                      "Opened ISIM session for session_type %d, "
                      "session_id %d, slot_id %d",
                      ds3g_mmgsdi_isim_info[index].session_type,
                      ds3g_mmgsdi_isim_info[index].session_id,
                      ds3g_mmgsdi_isim_info[index].slot_id);

      ds3g_mmgsdi_non_prov_isim_session_register_for_refresh(index);

      ds3g_mmgsdi_read_impi_from_non_prov_session_type( 
                                   ds3g_mmgsdi_isim_info[index].session_type );
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Received Invalid CNF [%d] from mmgsdi", cnf);
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Session ID Registration Failed for ISIM [%d]", status);
  }
  return;
} /* ds3g_mmgsdi_non_prov_isim_session_reg_cb */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_NON_PROV_ISIM_SESSION_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register DS Manager for ISIM refresh.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void ds3g_mmgsdi_non_prov_isim_session_register_for_refresh
(
  uint8  index
)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type  refresh_files = {0};
  mmgsdi_file_enum_type          file_list[DS3G_MMGSDI_MAX_NUM_REFRESH_FILES];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "Registering for refresh event for ISIM session type %d, "
                  "session id: %d",
                  ds3g_mmgsdi_isim_info[index].session_type,
                  ds3g_mmgsdi_isim_info[index].session_id);

  /* Issue a read command to read the IMPI info through MMGSDI API */
  if ( ds3g_mmgsdi_is_isim_app(ds3g_mmgsdi_isim_info[index].app_info) == TRUE )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Registers IMPI file change from ISIM");
    file_list[0] = MMGSDI_ISIM_IMPI; /* EF for IMPI file */
    refresh_files.num_files = 1;
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "Could not register IMPI file change from ISIM");
    return;
  }
  refresh_files.file_list_ptr = &file_list[0];

  mmgsdi_status = mmgsdi_session_register_for_refresh(
                                     ds3g_mmgsdi_isim_info[index].session_id,
                                     refresh_files,
                                     FALSE,
                                     ds3g_mmgsdi_client_rsp_cb,
                                     (mmgsdi_client_data_type)0);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Failed to register for ISIM refresh notification [%d]",
                    mmgsdi_status);
  }

  return;
} /* ds3g_mmgsdi_non_prov_isim_session_register_for_refresh */

/*===========================================================================
FUNCTION DS3G_MMGSDI_NON_PROV_ISIM_SESSION_EVT_REG_CB

DESCRIPTION
  This function handles the MMGSDI client event registration callback for
  the non provisioned isim app.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb
(
  const mmgsdi_event_data_type  *event
)
{
  ds_cmd_type                  *cmd_ptr = NULL;
  ds3g_mmgsdi_event_info_type  *ds_mmgsdi_event_info = NULL;
  boolean                       event_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( event == NULL )
  {
    /* if not event info, we return right away */
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL event ptr");
    return;
  }

  /*-------------------------------------------------------------------------
    check client id
  -------------------------------------------------------------------------*/
  if ( ds3g_mmgsdi_validate_client_id( event->client_id ) != TRUE )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb(): "
                    "Invalid client Id [%d] ",
                    event->client_id);
    return;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  ds_mmgsdi_event_info
    = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  ds_mmgsdi_event_info->session_id = DS3G_MMGSDI_INVALID_SESSION_ID;

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb(): "
                  "Session ID [%d] Event [%d] stage [%d]",
                  event->session_id, event->evt, event->data.refresh.stage);

  switch( event->evt )
  {
    case MMGSDI_REFRESH_EVT:
      ds_mmgsdi_event_info->session_id = event->session_id;
      ds_mmgsdi_event_info->slot = event->data.refresh.slot;
      ds_mmgsdi_event_info->refresh_mode = event->data.refresh.mode;

      if (event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
      {
        ds_mmgsdi_event_info->event_type =
          DS3G_MMGSDI_ISIM_REFRESH_START_EVENT;
      }
      else if(event->data.refresh.stage == MMGSDI_REFRESH_STAGE_END_SUCCESS)
      {
        ds_mmgsdi_event_info->event_type =
          DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_EVENT;
      }
      else if(event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Received wait for OK");
        ds_mmgsdi_event_info->event_type = DS3G_MMGSDI_ISIM_OK_REFRESH_EVENT;
        ds_mmgsdi_event_info->session_id = event->session_id;
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Ignoring MMGSDI REFRESH stage %d",
                        event->data.refresh.stage);
        /* Free DS cmd buffer, this being a error case */
        ds_release_cmd_buf(&cmd_ptr);
        return;
      }
      break;

    default:
      event_valid = FALSE;
      // all other events are not of interest to DS manager
      break;
  }

  if (event_valid == TRUE)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb(): "
                    "posting command. cmd_id [%d]",
                     cmd_ptr->hdr.cmd_id);
    ds_put_cmd( cmd_ptr );
  }
  else
  {
   ds_release_cmd_buf(&cmd_ptr);
   return;
  }

  return;
} /* ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_ISIM_APP_PTR_FROM_AVAILABLE_APPS

DESCRIPTION
  Function to get the ISIM app ptr from

DEPENDENCIES
  None

RETURN VALUE
  slot_id - the slot associated with the ISIM app

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_app_info_type* ds3g_mmgsdi_get_isim_app_ptr_from_available_apps
(
   mmgsdi_slot_id_enum_type       slot_id,
   mmgsdi_available_apps_type    *mmgsdi_available_apps_ptr
)
{
  uint8  app_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (mmgsdi_available_apps_ptr->card1_num_app != 0)
  {
    for (app_index = 0;
         app_index < mmgsdi_available_apps_ptr->card1_num_app;
         app_index++)
    {
      if(mmgsdi_available_apps_ptr->card1_app_info[app_index].slot != slot_id)
      {
        break;
      }

      if(ds3g_mmgsdi_is_isim_app(mmgsdi_available_apps_ptr->
                                    card1_app_info[app_index].app_data) == TRUE)
      {
        return &mmgsdi_available_apps_ptr->card1_app_info[app_index];
      }
    }
  }

  if (mmgsdi_available_apps_ptr->card2_num_app != 0)
  {
    for (app_index = 0;
         app_index < mmgsdi_available_apps_ptr->card2_num_app;
         app_index++)
    {
      if(mmgsdi_available_apps_ptr->card2_app_info[app_index].slot != slot_id)
      {
        break;
      }

      if(ds3g_mmgsdi_is_isim_app(mmgsdi_available_apps_ptr->
                                    card2_app_info[app_index].app_data) == TRUE)
      {
        return &mmgsdi_available_apps_ptr->card2_app_info[app_index];
      }
    }
  }

  if (mmgsdi_available_apps_ptr->card3_num_app != 0)
  {
    for (app_index = 0;
         app_index < mmgsdi_available_apps_ptr->card3_num_app;
         app_index++)
    {
      if(mmgsdi_available_apps_ptr->card3_app_info[app_index].slot != slot_id)
      {
        break;
      }

      if(ds3g_mmgsdi_is_isim_app(mmgsdi_available_apps_ptr->
                                    card3_app_info[app_index].app_data) == TRUE)
      {
        return &mmgsdi_available_apps_ptr->card3_app_info[app_index];
      }
    }
  }

  return NULL;
} /* ds3g_mmgsdi_get_isim_app_ptr_from_available_apps */

/*===========================================================================
FUNCTION DS3G_MMGSDI_IS_ISIM_APP

DESCRIPTION
  Function to determine if the passed app is an ISIM app

DEPENDENCIES
  None

RETURN VALUE
  TRUE - ISIM app
  FALSE - Not ISIM app

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_is_isim_app
(
  mmgsdi_aid_type  app_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* "app_data.aid.data_ptr" is an array, no need for NULL ptr check */
  if ((app_data.aid.data_len >= DS3G_MMGSDI_ISIM_AID_CMP_LENGTH)
      && (memcmp(app_data.aid.data_ptr,
                 ds3g_mmgsdi_isim_aid_value,
                 DS3G_MMGSDI_ISIM_AID_CMP_LENGTH) == 0)
      )
  {
    return TRUE;
  }

  return FALSE;
} /* ds3g_mmgsdi_is_isim_app */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SLOT_TO_NON_PROV_SESSION_TYPE

DESCRIPTION
  Function to get the non provisioned session type from the slot

DEPENDENCIES
  None

RETURN VALUE
  non provisioned session type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_session_type_enum_type ds3g_mmgsdi_slot_to_non_prov_session_type
(
  mmgsdi_slot_id_enum_type  slot
)
{
  mmgsdi_session_type_enum_type  non_prov_session = MMGSDI_MAX_SESSION_TYPE_ENUM;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (slot == MMGSDI_SLOT_1)
  {
    non_prov_session = MMGSDI_NON_PROV_SESSION_SLOT_1;
  }
  else if (slot == MMGSDI_SLOT_2)
  {
    non_prov_session = MMGSDI_NON_PROV_SESSION_SLOT_2;
  }
  else
  {
    non_prov_session = MMGSDI_NON_PROV_SESSION_SLOT_3;
  }

  return non_prov_session;
} /* ds3g_mmgsdi_slot_to_non_prov_session_type */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_SESSION_TYPE_IDX

DESCRIPTION
  This function returns the non prov isim session info index based
  on session type

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Get the index of the table successfully
  FALSE: Failed to get the index of the table

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_non_prov_isim_session_type_idx
(
  mmgsdi_session_type_enum_type  session_type,
  uint8                         *index
)
{
  uint8    i;
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_NON_PROV_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_isim_info[i].session_type == session_type )
    {
      *index = i;
      ret_val = TRUE;
      break;
    }
  }

  if( !ret_val )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Index not found in isim_info for session type %d",
                    session_type);
  }

  return ret_val;
}/* ds3g_mmgsdi_get_non_prov_isim_session_type_idx */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_NON_PROV_ISIM_SESSION_ID

DESCRIPTION
  This function returns the non prov isim session id based on session type

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Get the session id of the non prov isim session successfully
  FALSE: Failed to get the session id of non prov isim session

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_non_prov_isim_session_id
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_session_id_type         *session_id_ptr
)
{
  uint8    i;
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_NON_PROV_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_isim_info[i].session_type == session_type )
    {
      *session_id_ptr = ds3g_mmgsdi_isim_info[i].session_id;
      ret_val = TRUE;
      break;
    }
  }

  if( !ret_val )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Session id not found for session type %d",
                    session_type);
  }

  return ret_val;

}/* ds3g_mmgsdi_get_non_prov_isim_session_id */

/*===========================================================================
FUNCTION ds3g_mmgsdi_get_subscription_ready_status_idx

DESCRIPTION
  Function to get the subscription ready status array index for the passed
  session type

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Get the index of the table successfully
  FALSE: Failed to get the index of the table

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_subscription_ready_status_idx
(
  mmgsdi_session_type_enum_type  session_type,
  uint8*                         index
)
{
  uint8  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if ( ds3g_mmgsdi_subscription_ready_status[i].session_type
         == session_type )
    {
      *index = i;
      return TRUE;
    }
  }
  return FALSE;
} /* ds3g_mmgsdi_get_subscription_ready_status_idx */

/*===========================================================================
FUNCTION ds3g_mmgsdi_set_and_notify_subscription_ready_status

DESCRIPTION
  Function to determine if both subscription ready event and subs id info
  has been received.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE - sucess
            FALSE - failure

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_set_and_notify_subscription_ready_status
(
  uint8                          mask,
  mmgsdi_session_type_enum_type  session_type
)
{
  uint8 index = 0;
  uint8                          ready_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((mask != DS3G_MMGSDI_SUBSCRIPTION_READY_MASK) &&
      (mask != DS3G_MMGSDI_SUBS_ID_READY_MASK))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Invalid ds3g mmgsdi status mask %d ",
                    mask);
    return FALSE;
  }

  if (ds3g_mmgsdi_get_subscription_ready_status_idx(session_type,&index)
        == FALSE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get subscription ready status index from "
                    "session type %d ",
                    session_type);
    return FALSE;
  }

  ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status |= mask;

  ready_mask = DS3G_MMGSDI_SUBSCRIPTION_READY_MASK |
               DS3G_MMGSDI_SUBS_ID_READY_MASK;

  if ((ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status &
      ready_mask) == ready_mask)
  {
    ds3g_mmgsdi_subscription_ready_status[index].subscription_ready_status &=
                   (~ready_mask);

    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                    "Notifying subscription ready for session type %d",
                    session_type);

    ds3g_mmgsdi_notify_event(DS3GEVENTMGR_MMGSDI_SUBSCRIPTION_READY_EV,
                             session_type,
                             NULL);
  }

  return TRUE;
} /* ds3g_mmgsdi_set_and_notify_subscription_ready_status */

/*===========================================================================
FUNCTION ds3g_mmgsdi_clear_subs_id_ready_mask

DESCRIPTION
  Function to clear subs_id_ready_mask.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_clear_subs_id_ready_mask
(
  sys_modem_as_id_e_type         subs_id
)
{
  uint8                          i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  for ( i = 0; i < MAX_DS3G_MMGSDI_SESSIONS; i++ )
  {
    if (ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[i].sub_id == subs_id) 
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                "Clear subs id ready mask, %d",
                 ds3g_mmgsdi_subscription_ready_status[i].subscription_ready_status);

      ds3g_mmgsdi_subscription_ready_status[i].subscription_ready_status &=
          (~(DS3G_MMGSDI_SUBS_ID_READY_MASK | DS3G_MMGSDI_USIM_SUBS_ID_READY_MASK));
    }
  }
} /* ds3g_mmgsdi_clear_subs_id_ready_mask */

/*===========================================================================
FUNCTION ds3g_mmgsdi_notify_event

DESCRIPTION
  Function to notify clients of ds3g mmgsdi events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_notify_event
(
  ds3geventmgr_event_type                event_id,
  mmgsdi_session_type_enum_type          session_type,
  mmgsdi_session_changed_evt_info_type  *session_changed_info
)
{
  ds3geventmgr_event_info_type  event_info;
  uint8                         index = 0;
  ds3geventmgr_filter_type      filter_info;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(event_id <= DS3GEVENTMGR_INVALID_EV || event_id >= DS3GEVENTMGR_MAX_EV)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid event_id");
    return;
  }

  if (ds3g_mmgsdi_get_session_type_idx(session_type,&index) == TRUE)
  {

    if ( event_id == DS3GEVENTMGR_SUB_AVAIL_EV )
    {
      subs_id = ds3g_mmgsdi_get_sub_id_from_session_type(session_type);
      if ((ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].is_operational
              == FALSE ) || 
             (ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE))
      {
        return;
      }
    }

    /* Notify subscription ready ev to registered clients */
    memset(&event_info,0,sizeof(ds3geventmgr_event_info_type));

    memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
        ds3gsubsmgr_subs_id_cm_to_ds(ds3g_mmgsdi_info.
                                     ds3g_mmgsdi_session_info[index].sub_id);
    filter_info.tech = PS_SYS_TECH_ALL;

    event_info.mmgsdi_info.session_id =
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_id;
    event_info.mmgsdi_info.session_type =
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].session_type;
    event_info.mmgsdi_info.app_type =
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].app_type;
    event_info.mmgsdi_info.slot =
      ds3g_mmgsdi_info.ds3g_mmgsdi_session_info[index].slot;

    if (session_changed_info != NULL)
    {
      event_info.mmgsdi_info.session_changed_info = *session_changed_info;
    }

    if(ds3geventmgr_set_event_info(event_id,&event_info,&filter_info) == FALSE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Unable to provide MMGSDI event info to regist clients");
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Failed to get session info index for session type [%d] "
                    "Not notifying event %d",
                    event_id);
  }
} /* ds3g_mmgsdi_notify_event */

/*===========================================================================
FUNCTION      DS3GMMGSDI_NV_REFRESH_IND

DESCRIPTION   Notify DS3G module about NV refresh indication.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gmmgsdi_nv_refresh_ind
(
  sys_modem_as_id_e_type subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_EHRPD
  /*---------------------------------------------------
    Reading nv item 06862
  ---------------------------------------------------*/
  ds3g_mmgsdi_acl_info.acl_read_from_usim = ds3g_get_ehrpd_auth_in_usim();

  /*---------------------------------------------------
    If acl read mode is S/W then don't read ACL from
    card and mark the initialization flag to TRUE.
  ---------------------------------------------------*/
  if( ds3g_mmgsdi_acl_info.acl_read_from_usim == FALSE )
  {
    ds3g_mmgsdi_acl_process_done();
  }
#endif /* FEATURE_EHRPD */

  return;
} /* ds3gmmgsdi_nv_refresh_ind() */

/*===========================================================================
FUNCTION DS3G_MMGSDI_NOTIFY_IMSI_INFO_EV

DESCRIPTION
  This function checks if valid IMSI info is availaible and notify
  the registered clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_notify_imsi_info_ev
(
  sys_modem_as_id_e_type       subs_id,
  mmgsdi_app_enum_type         app_type,
  ds3g_mmgsdi_imsi_info_type  *imsi_info_ptr
)
{
  ds3geventmgr_event_info_type	event_info;
  ds3geventmgr_filter_type      filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( imsi_info_ptr == NULL )
  {
    return;
  }

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
  
  filter_info.tech       = PS_SYS_TECH_ALL;
  filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                              ds3gsubsmgr_subs_id_cm_to_ds(subs_id);

  memset(&event_info,0,sizeof(ds3geventmgr_event_info_type));

  if ( imsi_info_ptr->ready_status )
  {
    event_info.imsi_info.is_valid = TRUE;
    memscpy(&event_info.imsi_info.imsi_data, sizeof(nv_ehrpd_imsi_type),
            &imsi_info_ptr->imsi_info, sizeof(nv_ehrpd_imsi_type));
    event_info.imsi_info.app_type = MMGSDI_APP_USIM;
  }
  else
  {
    event_info.imsi_info.is_valid = FALSE;
  }
  
  if(ds3geventmgr_set_event_info(DS3GEVENTMGR_MMGSDI_IMSI_INFO_EV,
                                  (void *)&event_info,&filter_info) == FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unable to provide IMSI info to registered clients");
  } 
  return;
} /* ds3g_mmgsdi_notify_imsi_info_ev */

/*===========================================================================

FUNCTION DS3G_MMGSDI_NOTIFY_IMPI_INFO_EV

DESCRIPTION
  This function notifes the IMPI info to the registered clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_notify_impi_info_ev
(
  sys_modem_as_id_e_type       subs_id,
  ds3g_mmgsdi_impi_info_type  *impi_info_ptr
)
{
  ds3geventmgr_event_info_type  event_info;
  ds3geventmgr_filter_type      filter_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( impi_info_ptr == NULL )
  {
    return;
  }

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
  
  filter_info.tech       = PS_SYS_TECH_ALL;
  filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                              ds3gsubsmgr_subs_id_cm_to_ds(subs_id);

  memset(&event_info,0,sizeof(ds3geventmgr_event_info_type));

  if ( impi_info_ptr->ready_status )
  {
    event_info.impi_info.is_valid = TRUE;
    memscpy(&event_info.impi_info.impi_data, sizeof(nv_ehrpd_imsi_type),
            &impi_info_ptr->imsi_info, sizeof(nv_ehrpd_imsi_type));
  }
  else
  {
    event_info.impi_info.is_valid = FALSE;
  }
  
  if(ds3geventmgr_set_event_info(DS3GEVENTMGR_MMGSDI_IMPI_INFO_EV,
                                  (void *)&event_info,&filter_info) == FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unable to provide IMSI info to registered clients");
  } 
  return;
}/* ds3g_mmgsdi_notify_impi_info_ev */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_LOW_PRIO_SIGNALING

DESCRIPTION
  This function returns the low priority signaling bit read from the SIM.
 
  This function returns FALSE if 
    1. If the file cannot be read
    2. If the Low priority signaling TAG cannot be found
 
   It returns TRUE otherwise
 
   If the function returns TRUE, low priority signaling bit parameter will
   be populated
 
DEPENDENCIES
  None

RETURN VALUE
  Boolean
 
SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_low_prio_signaling
(
  sys_modem_as_id_e_type         subs_id,
  boolean                       *low_prio_sig_ptr
)
{
  boolean                      ret_val          = FALSE;
  mmgsdi_return_enum_type      mmgsdi_ret_val   = MMGSDI_MAX_RETURN_ENUM;
  mmgsdi_session_id_type       session_id;
  mmgsdi_data_type             read_data;
  mmgsdi_len_type              cache_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (low_prio_sig_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("DS3GMMGSDI low_prio_sig is NULL");
    return ret_val;
  }

  *low_prio_sig_ptr = FALSE;

  if(FALSE == ds3g_mmgsdi_get_session_id_from_sub_id(subs_id,&session_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Error: Invalid session id: 0x%x", session_id);
    return ret_val;
  }

  /*---------------------------------------------------------------------- 
    1. Get the cache file size
    2. Read the value from the cache
 ----------------------------------------------------------------------*/
  mmgsdi_ret_val = mmgsdi_session_read_cache_file_size(session_id,
                                                       MMGSDI_USIM_NASCONFIG,
                                                       &cache_len);
                                                       
  if (mmgsdi_ret_val != MMGSDI_SUCCESS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "DS3GMMGSDI RET VAL ERR %d",mmgsdi_ret_val);
    return FALSE;
  }

  read_data.data_len = cache_len;
  read_data.data_ptr = (uint8 *)modem_mem_alloc(cache_len,
                                                MODEM_MEM_CLIENT_DATA_CRIT);


  mmgsdi_ret_val = mmgsdi_session_read_cache(session_id,
                                             MMGSDI_USIM_NASCONFIG,
                                             read_data);

  if (mmgsdi_ret_val != MMGSDI_SUCCESS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "DS3GMMGSDI RET VAL ERR %d",mmgsdi_ret_val);
    goto clean_up;
  }

  /*----------------------------------------------------------------- 
    Get the low priority bit from the utility function
    If this call returns TRUE, it means LOW PRIO SIGNALING
    is set in the FILE and the value will be populated in low_prio_sig
    ----------------------------------------------------------------*/
  if(ds_3gpp_read_low_prio_bit(read_data.data_ptr,
                            read_data.data_len,
                            low_prio_sig_ptr))
  {
    DS_3GPP_MSG0_HIGH("DS3GMMGSDI LOW_PRIO_BIT configured in the USIM file");
    ret_val = TRUE;
  }
  
  clean_up:
    modem_mem_free(read_data.data_ptr,MODEM_MEM_CLIENT_DATA_CRIT);

  return ret_val;
}/*ds3g_mmgsdi_get_low_prio_signaling*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================

FUNCTION DS3G_MMGSDI_RESET_IMSI_INFO

DESCRIPTION
  This function resets the IMSI info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_mmgsdi_reset_imsi_info
(
  sys_modem_as_id_e_type       subs_id,
  mmgsdi_app_enum_type         app_type,
  ds3g_mmgsdi_imsi_info_type  *imsi_info_ptr
)
{
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  " REset IMSI data");
    /* Reset the IMSI info table */
  memset( imsi_info_ptr->imsi_response,
          0,
          DS3G_MMGSDI_IMSI_LEN );
  imsi_info_ptr->mnc_response[0] = 0;

  memset( (void*)&imsi_info_ptr->imsi_info,
          0, sizeof(nv_ehrpd_imsi_type) );

  imsi_info_ptr->ready_status = FALSE;

  ds3g_mmgsdi_notify_imsi_info_ev(subs_id,
                                  app_type,
                                  imsi_info_ptr);
} /* ds3g_mmgsdi_reset_imsi_info */

/*===========================================================================

FUNCTION DS3G_MMGSDI_RESET_IMPI_INFO

DESCRIPTION
  This function resets the IMPI info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_mmgsdi_reset_impi_info
(
  sys_modem_as_id_e_type       subs_id
)
{
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  " REset IMPI data");
  if ( ds3g_mmgsdi_impi_info[subs_id].impi_info.data_ptr != NULL )
  {
    modem_mem_free( ds3g_mmgsdi_impi_info[subs_id].impi_info.data_ptr,
                    MODEM_MEM_CLIENT_DATA_CRIT );
  }

  memset( (void*)&ds3g_mmgsdi_impi_info[subs_id].impi_info, 
         0,
         sizeof(mmgsdi_data_type) );

  memset( (void*)&ds3g_mmgsdi_impi_info[subs_id].imsi_info,
         0,
         sizeof(nv_ehrpd_imsi_type) );

  ds3g_mmgsdi_impi_info[subs_id].ready_status = FALSE;

  ds3g_mmgsdi_notify_impi_info_ev(subs_id,
                                  &ds3g_mmgsdi_impi_info[subs_id]);
} /* ds3g_mmgsdi_reset_impi_info */

#endif /* FEATURE_MMGSDI */

