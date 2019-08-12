/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  BEARER INDEPENDENT PROTOCOL


GENERAL DESCRIPTION : ENHANCED STK layer BIP support

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2010 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_bip.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/30/16   gs      F3 logging improvements
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/06/16   gs      F3 logging improvements
05/04/16   gm      Fix to use first index of DS rat array as preferred
05/04/16   dd      Allow SCWS send data even if destination address is not valid
11/26/15   lm      Fix KW errors
10/01/15   lm      Fix compilation issues on linux mob
09/07/15   gs      Read Default APN NV using GSTK NV API
06/26/15   gs      Update the Channel index while populating SCWS Channel Table
06/15/15   vr      Remove TR delay logic for CLOSE CHANNEL
03/17/15   shr     While processing SEND DATA, along with Link Drop envelope
                   (due to OOS/DS subsystem error), send error TR
03/12/15   gm      Added Channel Index validity check
03/10/15   gm      Fixed incorrect usage of snprintf
02/26/15   gm      Fix channel reuse not working for DNS query open channel
01/22/15   gs      Send the right TR cause when DS Service is not available
01/19/15   kr      Fixed Compilation warnings
01/19/15   vr      estk_cm_cs_call_connected changed as counter
01/08/15   gs      Send respective TR instead of Link Drop event
12/29/14   gs      Fix for rejecting the Open Channel with with Busy on Call
12/17/14   gs      Dereg the IP Filter after properly closing the socket
12/09/14   lj/bd   Deliver profile 254 to DS if null APN provisioned in EHRPD
12/08/14   dy      Re checking in backed out CR 743973
11/25/14   dy      Fix BIP err TR not sent in open ch DNS query fail case
11/24/14   shr     Use gcf_gstk EFS item instead of NV_GPRS_ANITE_GCF_I
11/11/14   shr     Fixed compilation warnings
11/07/14   dy      Backing out fix for CR 743973 temporarily
11/07/14   bd      Ignore keep_ch_open flag in UICC server mode case
11/03/14   dy      Fix channel reuse variable incorrectly reset
                   Fix not attempting to get IPv6 handle for IPv4v6 profile
10/29/14   dy      Fix open ch DNS query followed by legacy close ch clean up error
10/29/14   dy      Fix keep_ch_open timer expiry not stopping PDN connection
10/29/14   dy      Fix possible crash when prev_open_ch cleaned up
10/29/14   sw      Check availability of 3GPP2 system in all indexes of DS avail_sys
10/21/14   shr     Close channel kept open if next proactive cmd is unexpected
10/16/14   dy      Fix incorrect usage of GSTK_IO_GET_DS_SYSTEM_RAT_INFO
10/14/14   bd      Support for configuring NV73530 with lesser than 100 bytes
10/12/14   sw      Check DS service availability for OPEN CH
10/07/14   shr     Memory optmization to the GSTK NV logic
10/01/14   bd      Removed feature GSTK_CFG_FEATURE_ME_ESTK_ICON
09/29/14   dy      Fix dsnet_stop() being called twice for same nethandle
09/25/14   dy      Fix memory leak
09/24/14   dy      Use ephermeral ports for BIP sessions
09/15/14   dy      Do not delete BIP profile if it is being used by other channels
09/15/14   sw      rex_timer_undef() keep_ch_open_timer_p to deallocate timer memory
09/09/14   vr      Reduce stack usage
09/09/14   dy      Fix logic involving IPV6 iface bring up in DNS query
09/09/14   bd      Features clean up
08/22/14   shr     Disable continous read into BIP Rx buffer by default
08/13/14   dy      Fix querying default profile not checking NV item
08/11/14   bd      Support for using default APN from NV 73530 for OPEN CH (default bearer)
08/01/14   gs      Send No specific cause can be given when slot mismatches
07/21/14   sw      Display alpha related changes
07/11/14   shr     Add Support for NV Refresh
07/11/14   gs      Reject the Open Channel incase of GSM, when the CS Call is connected
06/17/14   dy      Fix ch id not being copied to TR for on demand open ch
06/13/14   shr     Enable continous read into BIP Rx buffer based on NV config
06/10/14   bd      Fix 3GPP2 APN, user name and pwd local buffer size
06/03/14   hn      Fix prev_open_ch memory leak
06/03/14   hn      Pack Channel ID in TR for OPEN CH DNS query
05/29/14   shr     Cycle through source ports for consecutive BIP sessions
05/27/14   bd      Changes to hold null alpha and no alpha indications
05/12/14   shr     Do not send Alpha to clients if SEND DATA processing fails
05/12/14   shr     Do not send Link Drop envelope after Close Channel
05/07/14   shr     Fix 3GPP APN local buffer size
05/06/14   gs      Removed the ESTK featurization
05/05/14   gm      Update protocol identifier for IPv6
04/30/14   hn      Close existing socket for legacy OPEN CH reuse
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   hn      Fix crash in channel reuse logic
03/11/14   dy      Fixed wrong TR sent when no data available for recv data req
03/04/14   shr     Made BIP Session in progress API slot specific
03/04/13   shr     Fixed on-demand link establishment implementation
03/03/14   shr     ESTK BIP clean-up and enhancements
02/27/14   dy      Add default_apn, bip_apn_mapping_tbl, rat_priority NV item
02/21/14   dy      Fixed GSTK EFS stub fct bug and added default_apn nv
02/21/14   vr      Check default profile can be used for default bearer
02/21/14   vr      Remove hack in estk_set_net_policy_profile()
02/21/14   gs      Dereg the filter only after socket close is successful
02/11/14   gm      Fix KW critical warning
02/11/14   gs      Establish BIP channel even if we are not able to set a DS profile
                   for a non-preferred network
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Cleanup global variables while handling STOP signal
01/09/14   dy      Fix array reading error for user reject open ch req case
01/03/14   shr     BIP throughput optimizations
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
10/10/13   gm      Marked cmd_ref_id to get command table index
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
09/02/13   sw      Removed returning error for 3GPP2 profile creation in 3GPP case
07/29/13   gm      Sending response to client when data available envelope fail
07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
07/24/13   hn      Fix merging error leading to BIP username not set (3GPP)
07/23/13   sw      Support for DS callback to obtain RAT information.
07/15/13   gs      Remove NV 5770 dependency for STK related tests except for GSM
06/28/13   sw      Call to estk_bip_set_default_apn_from_efs is removed for
                   GSTK_BEARER_DEFAULT bearer type
06/11/13   hh      Fix Toolkit Compiling Errors
05/24/13   hn      Does APN mapping based on NV item
05/23/13   hn      Reset auth params after 3GPP create profile
                   Reset profile buf len for 3GPP
                   Revert previous change
05/23/13   gm      compilation error in MSG_ERROR macro call
05/23/13   gm      handling duplicate user confirmation
05/13/13   vr      FIX for BIP error returned after pressing no during
                   the user confirmation phase
05/07/13   vr      Send data buffer clean-up, if timer expires for send data
04/11/13   hh      Log "BIP session in progress" only when BIP is active
04/09/13   vr      Use safer versions of memcpy() and memmove()
03/19/13   bd      Migration to transient DS profiles
02/19/13   bd      Do not copy user ID and User pwd while checking Auth params
02/11/13   bd      Support for (e)HRPD access tech
02/07/13   hn      Update apn_class_info table
12/10/12   js      Fixed compiler warnings
10/19/12   hk      Memory Optimization in ESTK through F3 removal
10/08/12   sg      Support class 2/3 APN modification
09/19/12   sg      Store and reuse iface_id for phys link down
09/10/12   sg      Handle SCWS alpha
08/31/12   sg      Read auth parameter to set it in profile for 3gpp
06/25/12   dd      Read CS_PS NV item at init
06/08/12   dd      Fix SCWS Open Ch TR if call is rejected
05/28/12   sg      DS Sys API changes
05/14/12   sg      APN comparison for 3gpp2 shall not be case sensitive
04/10/12   sg      Create-reuse profile for 3gpp2
04/03/12   av      Replace feature flags with NVITEM
04/03/12   xz      Don't free APN, username and passwd until CH ID is freed
02/27/12   sg      When no alpha but QMI is present send to QMI to handle
02/18/12   av      Cleaned up remaining ALWAYS ON/OFF features
01/16/12   sg      Clear User CNF timer
01/03/12   sg      Avoid race condition by not using dropped channel
11/02/11   sg      Remove always on FEATURE_ESTK_BIP_USE_DATA_FILTER
10/20/11   dd      Fix BIP Client Data Available
10/06/11   sg      Remove AEESTD APIs
10/05/11   dd      Fix stale data in SCWS overflow buffer
10/03/11   sg      Send correct error code when send/rcv on closed channel
09/28/11   dd      Clear data buffer in send data or listen state
09/27/11   sg      Set QOS Parameters for TR based on what DS returns
09/23/11   sg      LTE QOS parameters for PDN connection
09/20/11   bd      Fixes for compilation errors
08/24/11   xz      Fix compiler warnings and LINT/KW errors
08/23/11   sg      Clean up after no_sim state
08/22/11   sg      Use ESTK Client id in open channel alpha handling
08/18/11   sg      Remove bearer type versus RAT checks
07/25/11   dd      Remove the SCWS Send Data log to improve performance
07/21/11   bd      Fixed copying issue when dealing with overlapping blocks
07/13/11   dd      SCWS Close Ch Fix
07/13/11   dd      Clear the SCWS Tx/Rx buffers
07/13/11   dd      Fix modified reason code for Open Ch TR
07/13/11   dd      Fix the scws overflow buffer copy
06/24/11   js      Update DS iface policy for dss_open_netlib2()
06/13/11   sg      Move FEATURE_ESTK_IGNORE_ALPHA to NV
05/20/11   sg      Fix crash when clearing null timer for phys link down
05/05/11   dd      Cache overflow SCWS data in ESTK
05/03/11   xz      Fix CMI compile issue
04/29/11   sg      Fix BIP alpha
04/28/11   nk      Fixed warning to include gstk.h need for gstk_get_nv() and
                   fixed warning for return value for non-void function
04/24/11   dd      SCWS support
04/22/11   yk      Fix for a card issue leading to card error during STATUS
                   command
04/20/11   sg      Remove reference to depricated fs.h
04/19/11   nb      Support to check call in progress for OPEN CHANNEL
04/12/11   sg      Fix estk_bip_set_default_apn_from_efs
03/30/11   sg      Use correct global for features_enabled
03/21/11   sg      Change PDN type if NV is set to default
03/09/11   sg      Clean up network handle when socket descriptor is invalid
02/28/11   sg      Add new state for CLOSE IN PROGRESS
02/14/11   dd      Add user confirmation with icon for BIP
02/14/11   sg      Fix close channel for on-demand link establishment
01/26/11   sg      Remove terminating null from APN, username and password
01/26/11   sg      Move registration for DS events outside DS context
12/09/10   js      Support PDN type IP_V4_V6 for BIP
01/03/11   sg      1) Call dsnet_start everytime for BIP call
                   2) Add API for open channel cleanup on gstk timer expiry
                   3) Organize cleanup
12/28/10   sg      Cleanup Open channel proactive command
12/21/10   sg      When setting profiles for Inter-RAT fails ignore error
12/08/10   sg      Port BIP fixes
11/22/10   js      1) Fix to not send duplicate channel status evt for link
                   dropped.
                   2) Try reconnection when you get DS_EINPROGRESS for socket.
11/09/10   bd      Fixed dual standby issues for network related commands
11/10/10   js      Set correct APN profile when EHRPD is defined
10/20/10   xz      Use default profile when APN is null
10/14/10   dd      Set routeable to 1 if filtering APIs are used
10/07/10   dd      Change opt_len size to 4
10/05/10   xz      1) KW Fixes
                   2) Use DS naming convention
10/04/10   js      Use profile 107 only when EHRPD feature is available
10/01/10   dd      Add filtering APIs
10/01/10   dd      Fix for Open Channel User Confirmation
09/23/10   dd      Add support of raw commands to BIP Alpha
08/30/10   xz      1) Fix issue of send error TR
                   2) Don't send CHANNEL STATUS if OPEN CH is pending
08/19/10   xz      Support default APN config
08/18/10   dd      Fix KW error
08/07/10   js      Fixed compilation error
08/06/10   sg      1)Fix crash when received data is equal to max buffer size
                   2)Release handle when establish link fails
                   3)Use Profile 107 always for 3GPP2
08/05/10   sg      Fix compilation error due to merge
07/30/10   dd      Send the Open Ch TR in case of on demand data
07/27/10   sg      Register for Physical Iface Down event
07/23/10   sg      Fix memory overwrite in receive data
07/22/10   sg      Fix compilation warnings on 6610
07/20/10   xz      Initialize extran param for TR
07/06/10   sg      1) Fix Lint error
                   2) Release network handle when no network for open channel
                   3) Reuse profiles on EFS when possible
07/01/10   xz      Fix the issue of send error TR
06/18/10   sg      Fix compilation error on 6610 target
06/10/10   sg      Save profile number and clean it up on close channel
06/09/10   sg      Correct profile creation
04/26/10   sg      Fixes memory leak
04/23/10   yt      Fixed KW errors
04/23/10   sg      Temporary hack for close channel race condition
04/09/10   sg      When no network for open channel send error TR
03/31/10   sg      Fix TCP handling and close channel PDN teardown
03/30/10   sg      Fix memory allocation for APN, User Login and User Password
03/23/10   xz      1) Fix the issue of second OPEN CH not working
                   2) copy alpha only when length is not zero
03/19/10   xz      Ignore access tech check as we don't have info from E-MM
03/15/10   xz      Add support of LTE and setting bearer parameters in profile
03/17/10   sg      UDP multihoming support and TCP bug fixes
03/12/10   sg      BIP bug fixes
02/21/10   xz      Fix link error
02/15/10   nb      Update for Dual Slot Support
02/05/10   sg      Fix malloc issue for APN
01/14/10   sg      Fixed multiple bugs for several commands
01/12/10   sg      Added featurization FEATURE_ESTK_BIP_INTERFACE
11/03/09   sg      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>
#include "uim_msg.h"
#ifdef FEATURE_ESTK
#include "gstk_nv_p.h"
#include "gstk_exp.h"
#include "fs_sys_types.h"
#include "fs_lib.h"
#include "fs_public.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#include "estk_bip.h"
#include "nv.h"
#include "dssinternal.h"

#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_3gpp2.h"
#include "ds_sys.h"
#include "ds_sys_event.h"
#include "ds_sys_conf.h"

static char estk_bip_profile_name[] = "BIP_PROFILE";

//static boolean estk_data_avail_evt_flag = FALSE;
//static boolean estk_ch_status_evt_flag = FALSE;
static gstk_access_technology_type  estk_curr_access_tech = GSTK_ACCESS_NONE;

static estk_result_enum_type estk_bip_process_send_data_error(
  uint32 ch_idx);

static estk_result_enum_type estk_bip_process_sock_close_evt(
  sint15 sock_fd,
  uint32 cmd_ref_id);

static estk_result_enum_type estk_bip_process_sock_close_error(
  int                                 index,
  uint32                              cmd_ref_id,
  gstk_general_result_enum_type       general_result,
  gstk_additional_info_ptr_type      *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type *other_info_ptr
);

static void estk_bip_reg_ds_iface_event(
  dss_iface_ioctl_event_enum_type     ds_iface_event,
  sint15                              dss_nethandle,
  boolean                             yes_or_no,
  int                                 index
);

static estk_result_enum_type estk_bip_get_prio_network(
  estk_network_type               *nw_type_ptr,
  uint8                           *addi_res_ptr,
  gstk_slot_id_enum_type           slot,
  gstk_bearer_code_enum_type       bearer_type,
  gstk_pkt_data_address_enum_type  address_type
);

static estk_result_enum_type estk_populate_open_ch_table(
  int32             index,
  estk_network_type nw_type
);

static estk_result_enum_type estk_bip_send_ch_status_event(
  int32                                    channel_id,
  gstk_ch_status_additional_info_enum_type ch_status_add_info
);

static estk_result_enum_type estk_bip_check_and_start_fallback(
  int32 index
);

static void estk_save_prev_open_ch(
  const gstk_cmd_from_card_type *gstk_req_ptr
);

static estk_result_enum_type estk_set_net_policy_profile(
  gstk_access_technology_type       access_tech,
  int                               ch_idx,
  dss_net_policy_info_type         *net_policy
);

static estk_result_enum_type estk_bip_set_3gpp_default_profile_apn(
  uint32              index,
  ds_profile_num_type profile_num
);

static boolean estk_bip_check_existing_gsm_cs_call(void);

static estk_result_enum_type estk_pre_process_3gpp_profile(
  int                         ch_idx,
  ds_profile_num_type         *profile_num_ptr,
  boolean                     *has_apn_ptr
);

/*===========================================================================

FUNCTION: estk_bip_channel_table_init

DESCRIPTION :
  This function sets bip_channel_table fields with non-zero initial values

PARAMETERS:
  instance: the estk_instances_ptr instance to be initialized

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_channel_table_init(uint32 instance)
{
  uint32 i = 0;

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[instance]);

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    estk_instances_ptr[instance]->bip_channel_table[i].sock_fd           = DSS_ERROR;
    estk_instances_ptr[instance]->bip_channel_table[i].sock_nethandle    = DSS_ERROR;
    estk_instances_ptr[instance]->bip_channel_table[i].sock_nethandle_v4 = DSS_ERROR;
    estk_instances_ptr[instance]->bip_channel_table[i].sock_nethandle_v6 = DSS_ERROR;
    estk_instances_ptr[instance]->bip_channel_table[i].profile_3gpp      = -1;
    estk_instances_ptr[instance]->bip_channel_table[i].profile_3gpp2     = -1;
    estk_instances_ptr[instance]->bip_channel_table[i].channel_id        = -1;
  }
}/* estk_bip_channel_table_init() */

/*===========================================================================

FUNCTION: estk_bip_cellular_oos_timer_cb

DESCRIPTION :
  The callback funciton supplied to Cellular OOS timer

PARAMETERS:
  slot_index: Index of the slot corresponding to the timer

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_cellular_oos_timer_cb(uint32 slot_index)
{
  estk_cmd_type            *cmd_ptr = NULL;
  gstk_slot_id_enum_type  slot_id = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_1("OOS timer expiry on slot: 0x%x", slot_index);

  slot_id = (gstk_slot_id_enum_type)slot_index;

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }

  cmd_ptr = estk_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd = ESTK_BIP_CELLULAR_OOS_TIMER_EXP_CMD;
    cmd_ptr->hdr.sim_slot_id = slot_id;
    estk_put_cmd(cmd_ptr);
  }
} /* estk_bip_cellular_oos_timer_cb */

/*===========================================================================

FUNCTION: estk_bip_keep_ch_open_timer_cb

DESCRIPTION :
  The callback funciton supplied to keep ch open timer

PARAMETERS:
  mask: mask containing the index and slot of the channel where the
        link is dropped

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_keep_ch_open_timer_cb(uint32 mask)
{
  estk_cmd_type          *cmd_ptr = NULL;
  uint32                  index   = 0;
  gstk_slot_id_enum_type  slot_id = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_1("estk_bip_keep_ch_open_timer_cb, mask: 0x%x",mask);

  ESTK_BIP_GET_INDEX_FROM_MASK(mask, index);

  if (index >= ESTK_MAX_BIP_CH)
  {
    return;
  }

  ESTK_BIP_GET_SLOT_FROM_MASK(mask, slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }

  cmd_ptr = estk_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd           = ESTK_BIP_KEEP_CH_OPEN_TIMER_EXP_CMD;
    cmd_ptr->hdr.cli_user_data = index;
    cmd_ptr->hdr.sim_slot_id   = slot_id;
    estk_put_cmd(cmd_ptr);
  }
} /* estk_bip_keep_ch_open_timer_cb */

/*===========================================================================

FUNCTION: estk_bip_set_default_apn_from_efs

DESCRIPTION :
  This function sets the default APN from EFS

PARAMETERS:
  index: the index in bip_channel_table

DEPENDENCIES:
  None

RETURN VALUE:
  The length of the apn (apn > 0), -1 if error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
int32 estk_bip_set_default_apn_from_efs(uint32 index)
{
  uint32                   i           = 0;
  size_t                   apn_len     = 0;
  int32                    ret         = -1;
  gstk_status_enum_type    gstk_status = GSTK_SUCCESS;
  gstk_nv_items_data_type  nv_data;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ret);

  if(index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ret;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].apn_len == 0)
  {
    memset(&nv_data, 0x00, sizeof(nv_data));

    /* Allocate memory in order to read the defualt APN from NV */
    nv_data.default_apn_ptr =
      (gstk_default_apn_data_type*)GSTK_CALLOC(sizeof(gstk_default_apn_data_type));

    if (nv_data.default_apn_ptr == NULL)
    {
      return ESTK_ERROR;
    }

    /* Read the Default APN */
    gstk_status = gstk_nv_access_read(GSTK_NV_SLOT_DEFAULT_APN,
                                      estk_curr_inst_ptr->slot_id,
                                      &nv_data);

    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Unable to get the default APN from efs");
      gstk_free(nv_data.default_apn_ptr);
      return ret;
    }

    apn_len = strnlen((char *)nv_data.default_apn_ptr->apn, (ESTK_APN_MAX_LENGTH + 1));
    if ((apn_len > 0) && (apn_len <= ESTK_APN_MAX_LENGTH))
    {
      if (estk_curr_inst_ptr->bip_channel_table[index].apn)
      {
        UIM_MSG_HIGH_0("Previous APN not deallocated, freeing memory now");
        gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
      }

      /*
        For 3GPP, DS uses strlen() to get length of APN (not apn_len)
        Hence, memory allocation size is apn_length + 1 in order to add
        a NULL character at the end of the string
      */
      estk_curr_inst_ptr->bip_channel_table[index].apn = (uint8 *)GSTK_CALLOC(apn_len + 1);

      if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
      {
        if(gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[index].apn,
                       nv_data.default_apn_ptr->apn,
                       apn_len,
                       apn_len,
                       sizeof(nv_data.default_apn_ptr->apn)) < apn_len)
        {
          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
          estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
          gstk_free(nv_data.default_apn_ptr);
          return -1;
        }
        estk_curr_inst_ptr->bip_channel_table[index].apn_len = (uint32)apn_len;
        /* Add NULL character at end of APN */
        estk_curr_inst_ptr->bip_channel_table[index].apn[apn_len] = (uint8)0;

        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++i)
        {
          UIM_MSG_HIGH_3("DEFAULT APN[%d] = 0x%x %c", i,
                          estk_curr_inst_ptr->bip_channel_table[index].apn[i],
                          estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
        }
        ret = (int32)apn_len;
      }
    } /* if ((apn_len > 0) && (apn_len <= ESTK_APN_MAX_LENGTH)) */
    else
    {
      UIM_MSG_ERR_1("Incorrect APN configuration, apn_len: 0x%x", apn_len);
    }
    gstk_free(nv_data.default_apn_ptr);
  } /* if(bip_channel_table[index].apn_len == 0) */
  return ret;
} /*estk_bip_set_default_apn_from_efs*/

/*===========================================================================

FUNCTION: estk_find_ch_socket_index

DESCRIPTION :
  This function is used to get the index of the first available channel
  corresponding to the channel status specified as argument

PARAMETERS:
  index : [Input] Value result argument to return available channel
  ch_status : [Input] The status of channel that is of interest

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_find_ch_socket_index(
  int32                     *index,
  estk_channel_status_type   ch_status
)
{
  uint8 i = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index == NULL)
  {
    UIM_MSG_ERR_0("index ERR:NULL");
    return ESTK_ERROR;
  }

  if (estk_curr_inst_ptr->keep_ch_open)
  {
    *index = estk_curr_inst_ptr->ch_open_id;
    return ESTK_SUCCESS;
  }

  for (i=0; i< ESTK_MAX_BIP_CH; i++)
  {
    if (estk_curr_inst_ptr->bip_channel_table[i].status == ch_status)
    {
      *index = i;
      estk_curr_inst_ptr->bip_channel_table[i].index = i;
      return ESTK_SUCCESS;
    }
  }
  return ESTK_ERROR;
} /* estk_find_ch_socket_index */

/*===========================================================================

FUNCTION: estk_bringdown_iface

DESCRIPTION :
  Bring down iface

PARAMETERS:
  dss_nethandle : handle to iface to be brought down

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bringdown_iface(int index, sint15 dss_nethandle)
{
  sint31 dss_status = DSS_ERROR;
  sint15 error_num  = DSS_ERROR;

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return;
  }

  /* cleanup for bip_channel_table[index].sock_nethandle
   will be handled automatically by estk_bip_process_sock_close_evt() */
  if (dss_nethandle != DSS_ERROR &&
      dss_nethandle !=
      estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle)
  {
    dss_status = dsnet_stop(dss_nethandle,
                            &error_num );
    UIM_MSG_HIGH_2("dsnet_stop returns %d with error_num %d",
                   dss_status, error_num);

    dss_status = dsnet_release_handle(dss_nethandle,
                                     &error_num );
    UIM_MSG_HIGH_2("dsnet_release_handle returns %d with error_num %d",
                   dss_status, error_num);
  }
}

/*===========================================================================

FUNCTION: estk_bip_clean_up_reuse_ch

DESCRIPTION :
  Free memory allocated for the BIP channel that was meant to be reused

PARAMETERS:
  index : [Input] Index of the channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_clean_up_reuse_ch(int index)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid Channel Table Index: 0x%x",
                  index);
    return;
  }

  UIM_MSG_HIGH_3("estk_bip_clean_up_reuse_ch: index %d, net_handle_v4 %d, net_handle_v6 %d",
                 index,
                 estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4,
                 estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6);

  estk_curr_inst_ptr->keep_ch_open = FALSE;
  estk_curr_inst_ptr->ch_open_id = -1;

  if (estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet)
  {
    gstk_free(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet);
    estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet = NULL;
    estk_curr_inst_ptr->prev_open_ch.access_point_name.length = 0;
  }
  if (estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address)
  {
    gstk_free(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address);
    estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address = NULL;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p != NULL)
  {
    UIM_MSG_HIGH_0("Clean up keep_ch_open_timer_p");
    (void)rex_clr_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
    rex_undef_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
    estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p = NULL;
  }

  estk_bringdown_iface(index, estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4);
  estk_bringdown_iface(index, estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6);

  estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 = DSS_ERROR;
  estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 = DSS_ERROR;
  estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 = DS_ENETNONET;
  estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v6 = DS_ENETNONET;
} /*estk_bip_clean_up_reuse_ch*/

/*===========================================================================

FUNCTION: estk_bip_clean_up_ch_table

DESCRIPTION :
  Free memory allocated for the BIP channel

PARAMETERS:
  index : [Input] Index of the channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_clean_up_ch_table(int index)
{
  estk_result_enum_type     estk_result        = ESTK_SUCCESS;
  sint31                    dss_status         = DSS_ERROR;
  sint15                    error_num          = DSS_ERROR;
  int                       i                  = 0;
  boolean                   delete_profile     = TRUE;

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return;
  }

  (void)estk_bip_clean_up_reuse_ch(index);

  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].send_buffer);
    estk_curr_inst_ptr->bip_channel_table[index].send_buffer = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer);
    estk_curr_inst_ptr->bip_channel_table[index].recv_buffer = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].src_addr != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].src_addr);
    estk_curr_inst_ptr->bip_channel_table[index].src_addr = NULL;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].dest_addr != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].dest_addr);
    estk_curr_inst_ptr->bip_channel_table[index].dest_addr = NULL;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
    estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].username_len > 0)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].username);
    estk_curr_inst_ptr->bip_channel_table[index].username = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].username_len = 0;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].passwd_len > 0)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].passwd);
    estk_curr_inst_ptr->bip_channel_table[index].passwd = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].passwd_len = 0;
  }

  memset(&(estk_curr_inst_ptr->bip_channel_table[index].policy_info), 0,
         sizeof(estk_curr_inst_ptr->bip_channel_table[index].policy_info));

  /*
    Delete 3gpp profile only if it is not being used by other ESTK BIP channels
  */
  if(estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp >= 0)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
    {
      delete_profile = TRUE;
      for (i = 0; i < ESTK_MAX_BIP_CH; i++)
      {
        if ((i != index) &&
            (estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_CLOSED) &&
            (estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp ==
             estk_curr_inst_ptr->bip_channel_table[i].profile_3gpp))
        {
          UIM_MSG_HIGH_1("3GPP profile %d being used by other channel(s)- do not delete",
                         estk_curr_inst_ptr->bip_channel_table[i].profile_3gpp);
          delete_profile = FALSE;
          break;
        }
      }

      if (delete_profile)
      {
        UIM_MSG_HIGH_1("ESTK_BIP Deleting profile number %d",
                       estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp);
        (void)ds_profile_delete(DS_PROFILE_TECH_3GPP,
                                estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp);
      }
    }
  }

  /*
    Delete 3gpp2 profile only if it was created by ESTK and is not being
    used by other ESTK BIP channels
  */
  if(estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp2 >= 0)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
    {
      delete_profile = TRUE;
      for (i = 0; i < ESTK_MAX_BIP_CH; i++)
      {
        if ((i != index) &&
            (estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_CLOSED) &&
            (estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp2 ==
             estk_curr_inst_ptr->bip_channel_table[i].profile_3gpp2))
        {
          UIM_MSG_HIGH_1("3GPP2 profile %d used by other ESTK BIP channel(s)- do not delete",
                         estk_curr_inst_ptr->bip_channel_table[i].profile_3gpp2);
          delete_profile = FALSE;
          break;
        }
      }

      if (delete_profile)
      {
        UIM_MSG_HIGH_1("ESTK_BIP Deleting profile number %d created by ESTK",
                       estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp2);
        (void)ds_profile_delete(DS_PROFILE_TECH_3GPP2,
                                estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp2);
      }
    }
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN)
  {
    UIM_MSG_HIGH_0("De-register for WLAN IP Address change");
    if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
    {
      estk_bip_reg_ds_iface_event(DSS_IFACE_IOCTL_ADDR_CHANGED_EV,
                                  estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                                  FALSE,
                                  index);
    }
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd != DSS_ERROR)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_CLOSED)
    {
      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;

      if(estk_curr_inst_ptr == NULL)
      {
        UIM_MSG_ERR_0("estk_curr_inst_ptr is NULL");
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSED;
        return;
      }
      if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
      {
        estk_result = estk_bip_process_sock_close_evt(
                        estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                        estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference);
        UIM_MSG_HIGH_1("BIP sock close returns %d", estk_result);
      }
    }
  }
  else
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle != DSS_ERROR)
    {
      if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
      {
        dss_status = dsnet_stop(
                       estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                       &error_num);
        UIM_MSG_HIGH_2("dsnet_stop returns %d with error_num %d",
                       dss_status, error_num);
        dss_status = dsnet_release_handle(
                       estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                       &error_num);
        UIM_MSG_HIGH_2("dsnet_release_handle returns %d with error_num %d",
                       dss_status, error_num);
      }
    }
  }

  estk_curr_inst_ptr->bip_channel_table[index].channel_id            = -1;
  estk_curr_inst_ptr->bip_channel_table[index].sock_fd               = DSS_ERROR;
  estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle        = DSS_ERROR;
  estk_curr_inst_ptr->bip_channel_table[index].iface_id              = DSS_IFACE_INVALID_ID;
  estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp2         = -1;
  estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp          = -1;
  estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params  = FALSE;
  estk_curr_inst_ptr->bip_channel_table[index].first_dss_sendto_done = FALSE;
  estk_curr_inst_ptr->bip_channel_table[index].receive_data          = FALSE;
  estk_scws_clear_send_receive_buf(index+1); /* ch_id = index+1 */
  estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed  = TRUE;
  estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch            = FALSE;
  estk_curr_inst_ptr->bip_channel_table[index].me_port_num           = 0;

  /* If Channel Table clean-up is not due to a fallback, then clean-up the
     cached Open Channel command, Channel status and Network Type */
  if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_FALLBACK_IN_PROGRESS)
  {
    estk_curr_inst_ptr->bip_channel_table[index].nw_type = ESTK_NETWORK_NONE;
    estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSED;
    estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_CMD_ENUM_NOT_USE;
    estk_open_ch_cleanup(&estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd);
  }
} /*estk_bip_clean_up_ch_table*/

/*===========================================================================

FUNCTION: estk_convert_to_ip_format

DESCRIPTION :
  Converts IP address into string format

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_convert_to_ip_format(
  gstk_pkt_data_address_type         *p_addr,
  char                               **p_s_addr)
{
  char    buff[64];
  uint16  bytes_copied = 0;
  uint16  buff_size    = sizeof(buff);
  uint16  i            = 0;
  int16   ret          = 0;

  UIM_MSG_HIGH_2("estk_convert_to_ip_format: p_addr=0x%x, p_s_addr=0x%x",
                 p_addr, p_s_addr);

  if (p_addr == NULL || p_s_addr == NULL)
  {
    return ESTK_ERROR;
  }

  memset(buff, 0x00, buff_size);
  *p_s_addr = NULL;

  UIM_MSG_HIGH_1("estk_convert_to_ip_format: Address type:",
                 p_addr->address_type);
  switch(p_addr->address_type)
  {
  case GSTK_PKT_DATA_IPV4_ADDRESS:
    if (p_addr->pkt_address.length != 4)
    {
      UIM_MSG_ERR_1("invalid addr len 0x%x!", p_addr->pkt_address.length);
      return ESTK_ERROR;
    }
#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#else
    ret = snprintf(
            buff,
            buff_size,
            "%d.%d.%d.%d",
            p_addr->pkt_address.address[0],
            p_addr->pkt_address.address[1],
            p_addr->pkt_address.address[2],
            p_addr->pkt_address.address[3]);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
    if (ret >= buff_size || ret < 0)
    {
      return ESTK_ERROR;      
    }
    break;
  case GSTK_PKT_DATA_IPV6_ADDRESS:
    if (p_addr->pkt_address.length != 16)
    {
      UIM_MSG_ERR_1("invalid addr len 0x%x!", p_addr->pkt_address.length);
      return ESTK_ERROR;
    }

    for (i = 1; i < p_addr->pkt_address.length; i = i + 2)
    {
      if (i != (p_addr->pkt_address.length - 1))
      {
        ret = snprintf(
                (buff + bytes_copied),
                buff_size,
                "%02x%02x:",
                p_addr->pkt_address.address[i - 1],
                p_addr->pkt_address.address[i]);
      }
      else
      {
#ifdef FEATURE_UIM_TEST_FRAMEWORK
        #error code not present
#else
        ret = snprintf(
                (buff + bytes_copied),
                buff_size,
                "%02x%02x",
                p_addr->pkt_address.address[i - 1],
                p_addr->pkt_address.address[i]);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      }
      if (ret >= buff_size || ret < 0)
      {
        return ESTK_ERROR; 
      }
      bytes_copied = bytes_copied + ret;
      buff_size = buff_size - ret;
    }
    break;
  default:
    UIM_MSG_ERR_1("invalid address_type 0x%x!", p_addr->address_type);
    return ESTK_ERROR;
  }
  for (i = 0; i < strlen(buff); ++i)
  {
    UIM_MSG_HIGH_3("IP Addr[%d] = 0x%x(%c)", i, buff[i], buff[i]);
  }
  i = strlen(buff) + 1;
  *p_s_addr = (char *)GSTK_CALLOC(i);
  if (*p_s_addr == NULL)
  {
    return ESTK_ERROR;
  }
  (void)strlcpy(*p_s_addr, buff, i);
  return ESTK_SUCCESS;
} /* estk_convert_to_ip_format */

/*===========================================================================

FUNCTION: estk_bip_send_ch_status_event

DESCRIPTION :
  This function sends the Channel Status Event Download envelope to GSTK

PARAMETERS:
  channel_id : [Input] Channel ID

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_send_ch_status_event(
  int32                                    channel_id,
  gstk_ch_status_additional_info_enum_type ch_status_add_info
)
{
  gstk_evt_dl_ind_type ch_status;

  UIM_MSG_HIGH_2("In estk_bip_send_ch_status_event, channel id: 0x%x, add info: 0x%x",
                 channel_id, ch_status_add_info);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&ch_status, 0x00, sizeof(ch_status));

  ch_status.evt_type = GSTK_CH_STATUS_EVT_IND;
  ch_status.evt_ind.ch_status_evt_ind.ch_id = channel_id;
  ch_status.evt_ind.ch_status_evt_ind.ch_status_additional_info =
    ch_status_add_info;
  ch_status.evt_ind.ch_status_evt_ind.is_link_est = FALSE;

  if(gstk_send_envelope_evt_download_command(
       estk_curr_inst_ptr->gstk_client_id,
       0, &ch_status) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Sending Channel Status envelope failed");
    return ESTK_ERROR;
  }
  return ESTK_SUCCESS;
} /* estk_bip_send_ch_status_event */

/*===========================================================================

FUNCTION: estk_bip_process_sock_read_evt

DESCRIPTION :
  This function processes a read socket event from DS

PARAMETERS:
  sock_fd : [Input] Socket descriptor

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_sock_read_evt(
  sint15 sock_fd)
{
  int8                   index              = 0;
  sint15                 num_bytes_read     = 0;
  int32                  prev_recv_buf_len  = 0;
  int32                  recv_buf_len_avail = 0;
  sint15                 error_num          = DSS_ERROR;
  sint31                 dss_status         = DSS_ERROR;
  estk_result_enum_type  estk_status        = ESTK_ERROR;
  char                  *read_buffer        = NULL;
  gstk_evt_dl_ind_type   data_avail;
  gstk_evt_dl_ind_type   ch_status;

  UIM_MSG_HIGH_1("estk_bip_process_sock_read_evt: sock_fd=0x%x", sock_fd);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);
  ESTK_BIP_CHECK_SOCK_FD(sock_fd);

  /* Find the Channel index corresponding to the Socket */
  for(index = 0; index < ESTK_MAX_BIP_CH; index++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == sock_fd)
    {
      break;
    }
  }

  if(index == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid sock_fd");
    return ESTK_ERROR;
  }

  memset(&data_avail, 0x00, sizeof(gstk_evt_dl_ind_type));
  memset(&ch_status, 0x00, sizeof(gstk_evt_dl_ind_type));

  /* Only process Read Event when Channel is active */
  if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE)
  {
    UIM_MSG_HIGH_1("Cannot process DS_READ_EVENT in Channel State 0x%x",
                   estk_curr_inst_ptr->bip_channel_table[index].status);
    return ESTK_ERROR;
  }

  switch(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol)
  {
    case GSTK_PKT_DATA_TCP:
      /* In case of TCP, when continous read feature is disabled,
         do not read new data if existing data in the receive buffer
         has not yet been sent to the card */
      if((gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ,
            estk_curr_inst_ptr->slot_id) == FALSE) &&
         (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len != 0))
      {
        UIM_MSG_HIGH_1("TCP: Cannot append to Receive Buffer when its not empty: %d and continous read feature is disabled",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        return ESTK_ERROR;
      }

      /* If current length of buffer is non-zero but less than 0xFF, that
         implies that the previous DATA AVAILABLE or RECEIVE DATA TR
         indicated a Channel length of < 0xFF to the card.
         Appending new data to the receive buffer means that there would
         be a mismatch in the Data Length card assumes available and the
         actual data available. Hence, do not read new Data till receive
         buffer is empty */
      if((estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len > 0) &&
         (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len < 0xFF))
      {
        UIM_MSG_HIGH_1("TCP: Cannot append to Receive Buffer when its not empty and buffer len: %d is < 255",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        return ESTK_ERROR;
      }
      break;
    /* In case of UDP, do not read new data if existing data in the
     receive buffer has not yet been sent to the card */
    case GSTK_PKT_DATA_UDP:
      if (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len != 0)
      {
        /* ETSI 102 223: 6.4.29
           In the case of packet/datagram transmission, the terminal shall put
           in the Rx buffer a complete packet SDU and only one at one time. For
           example, if UDP datagrams are received by the terminal, the latter shall
           insert only the SDU of each UDP packet received in the Rx buffer. After
           one SDU has been downloaded by the UICC (using one or several RECEIVE
           DATA commands), the terminal shall insert the next SDU
           of UDP datagram, and so on */
        UIM_MSG_HIGH_1("UDP: Cannot append to Receive Buffer when its not empty, buffer length: %d",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        return ESTK_ERROR;
      }
      break;
    default:
      UIM_MSG_ERR_1("Invalid protocol: 0x%x, Never reach here",
                    estk_curr_inst_ptr->bip_channel_table[index].transport_protocol);
      return ESTK_ERROR;
  }

  recv_buf_len_avail = estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
                         estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

  if (recv_buf_len_avail <= 0)
  {
    UIM_MSG_HIGH_0("No space in receive buffer to Read new data from DS");
    return ESTK_ERROR;
  }

  /* Allocate memory to read from DATA */
  read_buffer = (char *)GSTK_CALLOC(recv_buf_len_avail * sizeof(char));
  if(read_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_3("Current Recv buffer len: %d, Len available: %d, Max. buffer size: %d",
                 estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len,
                 recv_buf_len_avail,
                 estk_curr_inst_ptr->bip_channel_table[index].buffer_size);

  if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
     GSTK_PKT_DATA_TCP)
  {
    num_bytes_read = dss_read(sock_fd,
                              read_buffer,
                              int32touint16(recv_buf_len_avail),
                              &error_num);

    UIM_MSG_HIGH_2("dss_read returned %d bytes with error_num %d",
                   num_bytes_read, error_num);
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
          GSTK_PKT_DATA_UDP)
  {
    num_bytes_read = dss_recvfrom(sock_fd,
                                  read_buffer,
                                  int32touint16(recv_buf_len_avail),
                                  0,
                                  NULL,
                                  NULL,
                                  &error_num);

    UIM_MSG_HIGH_2("dss_recvfrom returned %d bytes with error_num %d",
                   num_bytes_read, error_num);
  }

  do
  {
    /* Code must be added to check boundary conditions */
    if(num_bytes_read > 0 && num_bytes_read <= recv_buf_len_avail)
    {
      (void)memscpy((void *)(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer +
              estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len),
              recv_buf_len_avail,
              (void *)read_buffer,
              num_bytes_read);

      /* Cache the Receive buffer length for later use */
      prev_recv_buf_len = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

      estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len += (int32)num_bytes_read;

      /* If protocol is TCP, attempt continuous read from DATA (ETSI 102 223:
         6.4.29 is not applicable to TCP since its a streaming protocol) if there
         is space available in the receive buffer and continuous read feature
         is enabled */
      if((gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ,
            estk_curr_inst_ptr->slot_id) == TRUE) &&
         (estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP) &&
         ((estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
           estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len) > 0))
      {
        /* Request for more Data only if card has been indicated in previous DATA
           AVAILABLE/RECEIVE DATA TR that 0xFF data is available OR
           if receive buffer is empty and more than 0xFE data has been read.
           This is to ensure that there would not be a mismatch in the Data
           Length card assumes available and the actual data available */
        if((prev_recv_buf_len >= 0xFF) ||
           ((prev_recv_buf_len == 0) && (num_bytes_read >= 0xFF)))
        {
          dss_status = dss_async_select(sock_fd, DS_READ_EVENT, &error_num);

          if(dss_status != DSS_ERROR)
          {
            UIM_MSG_HIGH_0("dss_async_select succeeded");
          }
          else
          {
            UIM_MSG_ERR_0("dss_async_select failed");
          }
        }
      }

      /* Send DATA available envelope to card only if Receive Buffer was empty
         prior to read from DS */
      if(prev_recv_buf_len == 0)
      {
         /* Send event download envelope with data available event */
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_id
          = estk_curr_inst_ptr->bip_channel_table[index].channel_id;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_status_additional_info
          = GSTK_CH_STATUS_NO_FURTHER_INFO;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.is_link_est = TRUE;
        data_avail.evt_type = GSTK_DATA_AVAIL_EVT_IND;
        data_avail.evt_ind.data_avail_evt_ind.ch_len
          = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

        UIM_MSG_HIGH_1("ESTK sending data available event, Channel Len: %d",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

        if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
                                                   0, &data_avail) != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Sending data available envelope failed");
          estk_status = ESTK_ERROR;
          break;
        }
      }
      estk_status = ESTK_SUCCESS;
    }
    else if(num_bytes_read == 0 && error_num == DS_EEOF)
    {
      UIM_MSG_HIGH_0("EOF!");
      estk_status = ESTK_SUCCESS;
    }
    else if(num_bytes_read == DSS_ERROR)
    {
      if(error_num == DS_EWOULDBLOCK)
      {
        UIM_MSG_HIGH_0("dss_read returned EWOULDBLOCK");

        dss_status = dss_async_select(sock_fd, DS_READ_EVENT, &error_num);

        if(dss_status != DSS_ERROR)
        {
          UIM_MSG_HIGH_0("dss_async_select succeeded");
          estk_status = ESTK_SUCCESS;
        }
        else
        {
          UIM_MSG_ERR_0("dss_async_select failed");
          estk_status = ESTK_ERROR;
        }
      }
      else /*if error_num != DS_EWOULDBLOCK*/
      {
        UIM_MSG_HIGH_0("Sending channel dropped event");
        if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_DROPPED)
        {
          estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;

          /* Send Channel status Event Download envelope command */
          estk_status = estk_bip_send_ch_status_event(
                          estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                          GSTK_CH_STATUS_LINK_DROPPED);

          if(estk_status != ESTK_SUCCESS)
          {
            UIM_MSG_ERR_0("Sending Channel Status envelope failed");
            estk_status = ESTK_ERROR;
            break;
          }
        }
        else
        {
          UIM_MSG_HIGH_0("Link Drop already sent");
        }
        estk_status = ESTK_SUCCESS;
      }
    }
  }while(0);

  gstk_free(read_buffer);
  read_buffer = NULL;
  return estk_status;
} /* estk_bip_process_sock_read_evt */

/*===========================================================================
FUNCTION: estk_bip_write_sock

DESCRIPTION:


PARAMETERS:

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_write_sock (
  int32               index,
  uint32              cmd_ref_id
)
{
  sint31                              num_bytes       = 0;
  sint15                              error_num       = DSS_ERROR;
  sint31                              dss_status      = DSS_ERROR;
  estk_result_enum_type               estk_status     = ESTK_ERROR;
  uint8                               addi_res        = 0;
  gstk_additional_info_ptr_type       additional_info;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  uint16                              i;
  struct ps_sockaddr_in              *p_dest_addr_v4  = NULL;
  uint16                              addr_size;

  if ((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("estk_bip_write_sock: invalid index=0x%x",
                  index);
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;

  UIM_MSG_HIGH_3("estk_bip_write_sock, index=0x%x, cmd_ref_id=0x%x, p_dest_addr_v4=0x%x",
                 index, cmd_ref_id, p_dest_addr_v4);

  if (p_dest_addr_v4 == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;

  for(i = 0; i<estk_curr_inst_ptr->bip_channel_table[index].send_buf_len; i++)
  {
    UIM_MSG_HIGH_2("Send Data buf %d is 0x%x",
                   i, estk_curr_inst_ptr->bip_channel_table[index].send_buffer[i]);
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
     GSTK_PKT_DATA_TCP)
  {
    num_bytes = dss_write(
                  estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                  (void *)estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                  &error_num);
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
          GSTK_PKT_DATA_UDP)
  {
    p_dest_addr_v4->ps_sin_port =
      dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
    addr_size =
      (p_dest_addr_v4->ps_sin_family == DSS_AF_INET) ?
        sizeof(struct ps_sockaddr_in) : sizeof(struct ps_sockaddr_in6);

    num_bytes = dss_sendto(
                  estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                  0, /*flags not supported*/
                  (struct ps_sockaddr*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr,
                  addr_size,
                  &error_num);
  }

  UIM_MSG_HIGH_3("Wrote %d bytes with error_num %d, Buffer Len: %d",
                 num_bytes,
                 error_num,
                 estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);

  tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
  tr_extra_param.present = TRUE;

  if(num_bytes == (sint15)estk_curr_inst_ptr->bip_channel_table[index].send_buf_len)
  {
    /* dss_sendto is successful. Cache this info */
    if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
       GSTK_PKT_DATA_UDP)
    {
      estk_curr_inst_ptr->bip_channel_table[index].first_dss_sendto_done = TRUE;
    }
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
     estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    /* Send Success terminal response */
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      estk_curr_inst_ptr->extra_param = tr_extra_param;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      return ESTK_SUCCESS;
    }
    else
    {
      estk_status = estk_bip_send_terminal_response(
                      cmd_ref_id,
                      GSTK_SEND_DATA_CNF,
                      GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                      NULL,
                      &tr_extra_param,
                      index);

      if(estk_status == ESTK_ERROR)
      {
       UIM_MSG_ERR_0("Error in sending terminal response");
       return estk_status;
      }
      else
      {
       /* Reset the TX buffer */
       estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
       return ESTK_SUCCESS;
      }
    }
  }
  else if(num_bytes == DSS_ERROR)
  {
    if(error_num == DS_EWOULDBLOCK)
    {
      dss_status = dss_async_select(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                                    DS_WRITE_EVENT,
                                    &error_num);
      if(dss_status == DSS_SUCCESS)
      {
        return ESTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_1("dss_async_select failed with error_num %d" , error_num);
        return ESTK_ERROR;
      }
    }
    else
    {
      UIM_MSG_ERR_1("dss_write returns unhandled error_num %d", error_num);

        /* UDP: Write to socket failed. Check if bearer fallback needs to be
           attempted (WLAN to Cellular or vice-versa) and start fallback
           if needed */
      if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
         GSTK_PKT_DATA_UDP)
      {
        estk_status = estk_bip_check_and_start_fallback(index);

        if(estk_status == ESTK_UNSUPPORTED)
        {
          UIM_MSG_HIGH_1("Fallback Could not be started for index: 0x%x",
                         index);
        }
        else
        {
          return ESTK_SUCCESS;
        }
      }

      tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
       estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
       estk_curr_inst_ptr->bip_channel_table[index].send_buf_len;
      addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving SEND DATA TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
        estk_cache_additional_info(&additional_info);
        estk_curr_inst_ptr->extra_param = tr_extra_param;
        estk_status = ESTK_SUCCESS;
      }
      else
      {
        /* Send error terminal response */
        estk_status = estk_bip_send_terminal_response(
                        cmd_ref_id,
                        GSTK_SEND_DATA_CNF,
                        GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                        &additional_info,
                        &tr_extra_param,
                        index);
      }
      if(error_num != DS_EMSGSIZE)
      {
       UIM_MSG_ERR_0("DS_EMSGSIZE error -- update channel status ");
       if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_DROPPED)
       {
         /* Send Event Download envelope command */
         estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;

         (void)estk_bip_send_ch_status_event(
                 estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                 GSTK_CH_STATUS_LINK_DROPPED);
       }
       else
       {
         UIM_MSG_HIGH_0("Link Drop already sent");
       }
       return ESTK_ERROR;
      }
      else
      {
       return estk_status;
      }
    }
  }
  else /* num of bytes is a positive number */
  {
    UIM_MSG_HIGH_0("dss_write did not write all the bytes of send data");
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len -= (uint16)num_bytes;
    memsmove(estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
             (size_t)estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
             estk_curr_inst_ptr->bip_channel_table[index].send_buffer+num_bytes,
             (size_t)estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);
    estk_status = estk_bip_write_sock(index, cmd_ref_id);
    return estk_status;
  }
} /* estk_bip_write_sock */

/*===========================================================================
FUNCTION: estk_scws_send_data

DESCRIPTION:
   This function sends the SCWS Send Data req to the Agent

PARAMETERS:
  index     - Index into the bip ch table
  cmd_ref_id - Cmd ref id for terminal response
DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data(
  int32               index,
  uint32              cmd_ref_id
)
{
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type    call_cli_evt_cb;
  uint32                                   user_data   = 0;
  gstk_cmd_from_card_type                  gstk_scws_cmd;
  uint8                                    addi_res    = 0;
  gstk_additional_info_ptr_type            additional_info;
  gstk_terminal_rsp_extra_param_type       tr_extra_param;
  gstk_client_id_type                      client_id       = 0;
  gstk_status_enum_type                    gstk_status     = GSTK_SUCCESS;

  UIM_MSG_HIGH_2("estk_scws_send_data: index=0x%x, cmd_ref_id=0x%x",
                 index, cmd_ref_id);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index >= ESTK_MAX_BIP_CH || index < 0)
  {
    UIM_MSG_ERR_1("invalid ch idx 0x%x!", index);
    return ESTK_ERROR;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL)
  {
    UIM_MSG_ERR_0("Send buffer is null");
    return ESTK_ERROR;
  }

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));
  memset(&additional_info, 0x00, sizeof(gstk_additional_info_ptr_type));
  memset(&tr_extra_param, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));
  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;

  tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
  tr_extra_param.present = TRUE;

  find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
  find_cli_reg.cmd_type = GSTK_SCWS_SEND_DATA_REQ;
  find_cli_reg.format_type = GSTK_GSTK_FORMAT;
  gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_0("Did not find any clients registered for GSTK_SCWS_SEND_DATA_REQ");
    return ESTK_ERROR;
  }
  else
  {
    call_cli_evt_cb.client_id = client_id;
    call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
    gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_SEND_DATA_REQ;
    gstk_scws_cmd.hdr_cmd.cmd_detail_reference = cmd_ref_id;
    gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
    gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
    gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

    gstk_scws_cmd.cmd.scws_send_data_cmd_req.ch_id = index+1;
    gstk_scws_cmd.cmd.scws_send_data_cmd_req.ch_data.data_len =
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len;
    gstk_scws_cmd.cmd.scws_send_data_cmd_req.ch_data.data =
      estk_curr_inst_ptr->bip_channel_table[index].send_buffer;

    if (gstk_io_ctrl(
            GSTK_IO_CALL_CLIENT_EVT_CB,
            (void *)&call_cli_evt_cb,
            (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("can't call cli evt cb!");
      addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving SEND DATA TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
        estk_cache_additional_info(&additional_info);
        estk_curr_inst_ptr->extra_param = tr_extra_param;
      }
      else
      {
      /* Send error terminal response */
      (void)estk_bip_send_terminal_response(
              cmd_ref_id,
              GSTK_SEND_DATA_CNF,
              GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
              &additional_info,
              &tr_extra_param,
              index);
      }
    }
  }
  return ESTK_SUCCESS;
} /* estk_scws_send_data  */
/*===========================================================================

FUNCTION: estk_bip_process_sock_write_evt

DESCRIPTION :
  This function processes a write socket event from DS

PARAMETERS:
  sock_fd : [Input] Socket descriptor

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_sock_write_evt(
  sint15 sock_fd,
  uint32 cmd_ref_id)
{
  int8                  index          = 0;
  estk_result_enum_type estk_status    = ESTK_SUCCESS;

  UIM_MSG_HIGH_2("estk_bip_process_sock_write_evt: sock_fd=0x%x, cmd_ref_id=0x%x",
                 sock_fd, cmd_ref_id);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  for(index = 0; index < ESTK_MAX_BIP_CH; index++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == sock_fd)
    {
      break;
    }
  }

  if(index == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid sock_fd");
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_bip_process_sock_write_evt: Index: 0x%x, Current command is 0x%x",
                 index,
                estk_curr_inst_ptr->bip_channel_table[index].current_command_id);

  if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ)
  {
    return estk_perform_net_connect();
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_SEND_DATA_REQ)
  {
    if((estk_curr_inst_ptr->bip_channel_table[index].on_demand_link == TRUE) &&
       (estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE))
    {
      estk_status = estk_perform_net_connect();
      UIM_MSG_HIGH_1("Returning from estk_perform_net_connect with status 0x%x",
                     estk_status);
    }

    if((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_ACTIVE) &&
       (estk_status == ESTK_SUCCESS))
    {
      return estk_bip_write_sock(index, cmd_ref_id);
    }
    else
    {
      UIM_MSG_ERR_0("Channel not active for writing data on socket");
      return ESTK_ERROR;
    }
  }
  return ESTK_ERROR;
} /* estk_bip_process_sock_write_evt */

/*===========================================================================
FUNCTION: estk_bip_process_send_data_error

DESCRIPTION:
  The function:
  - Sends error TR for the SEND DATA being processed if
    not waiting for alpha CNF
  - Sets up error TR info in ESTK globals if SEND DATA processing
    is waiting for alpha CNF
  - Sends link Drop Channel Status Envelope to the card

PARAMETERS:
  ch_idx : [Input] Index of the channel on which SEND DATA is being processed

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_process_send_data_error(
  uint32 ch_idx)
{
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res    = 0;
  estk_result_enum_type               estk_status = ESTK_ERROR;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(ch_idx >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", ch_idx);
    return ESTK_ERROR;
  }

  memset((void *)&additional_info, 0, sizeof(gstk_additional_info_ptr_type));

  addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  additional_info.additional_info_ptr = &addi_res;
  additional_info.length = 1;

  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    UIM_MSG_HIGH_0("Saving SEND DATA TR");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
    estk_cache_additional_info(&additional_info);
    estk_status = ESTK_SUCCESS;
  }
  else
  {
    estk_status = estk_bip_send_terminal_response(
                    estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                    GSTK_SEND_DATA_CNF,
                    GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                    &additional_info,
                    NULL,
                    ch_idx);

    if(estk_status != ESTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Sending TR for send data failed");
    }
  }
  if(estk_curr_inst_ptr->bip_channel_table[ch_idx].status != ESTK_CH_DROPPED)
  {
    estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;

    (void)estk_bip_send_ch_status_event(
            estk_curr_inst_ptr->bip_channel_table[ch_idx].channel_id,
            GSTK_CH_STATUS_LINK_DROPPED);
  }
  return estk_status;
} /* estk_bip_process_send_data_error */

/*===========================================================================

FUNCTION: estk_bip_process_sock_close_evt

DESCRIPTION :
  This function processes a close socket event from DS

PARAMETERS:
  sock_fd : [Input] Socket descriptor

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_process_sock_close_evt(
  sint15 sock_fd,
  uint32 cmd_ref_id)
{
  sint15                              error_num      = DSS_ERROR;
  sint31                              dss_status     = DSS_ERROR;
  estk_result_enum_type               estk_status    = ESTK_ERROR;
  int32                               index          = 0;
  uint8                               addi_res       = 0;
  uint32                              opt_len        = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  dss_so_linger_type                  sock_linger;
  gstk_general_result_enum_type       general_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
  estk_network_type                   nw_type        = ESTK_NETWORK_NONE;

  UIM_MSG_HIGH_2("estk_bip_process_sock_close_evt: sock_fd=0x%x, cmd_ref_id=0x%x",
                 sock_fd, cmd_ref_id);

  for(index = 0; index< ESTK_MAX_BIP_CH; index++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == sock_fd)
    {
      break;
    }
  }
  if(index == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid sock_fd");
    return ESTK_ERROR;
  }

  memset((void *)&sock_linger, 0x00, sizeof(dss_so_linger_type));
  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));

  /* Check the pending command */
  if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_CLOSE_CH_REQ ||
      estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_CLOSE_IN_PROGRESS ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_FALLBACK_IN_PROGRESS)
  {
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    additional_info.additional_info_ptr = &addi_res;
    additional_info.length = 1;

    /* only OPEN CH TR will use those */
    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
      FALSE;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_status_additional_info =
      GSTK_CH_STATUS_LINK_DROPPED;
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
           sizeof(gstk_bearer_description_type),
           &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
           sizeof(gstk_bearer_description_type));
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size;

    sock_linger.l_onoff = 1;
    sock_linger.l_linger = 10000; /* Linger time of 10 seconds */
    opt_len = sizeof(sock_linger);

    /* Set socket option Linger time */
    dss_status = dss_setsockopt(sock_fd,
                                DSS_SOCK,
                                DSS_SO_LINGER,
                                (void *)&sock_linger,
                                &opt_len,
                                &error_num);

    UIM_MSG_HIGH_2("Setting Linger time returned %d and error_num %d",
                   dss_status, error_num);

    dss_status = dss_close(sock_fd, &error_num);

    if(dss_status == DSS_SUCCESS)
    {
      /* IP filter is not register for WLAN. Hence, no need to de-register */
      if(estk_curr_inst_ptr->bip_channel_table[index].nw_type != ESTK_NETWORK_WLAN)
      {
        dss_status = dss_dereg_ip_filter(
                       sock_fd,
                       estk_curr_inst_ptr->bip_channel_table[index].iface_id,
                       &error_num);
        if(dss_status != 0)
        {
          UIM_MSG_MED_2("dss_dereg_ip_filter failed with status 0x%x error number 0x%x",
                      dss_status, error_num);
        }
      }

      estk_curr_inst_ptr->bip_channel_table[index].sock_fd = DSS_ERROR;

      if (estk_curr_inst_ptr->keep_ch_open &&
          estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
          GSTK_CLOSE_CH_REQ)
      {
        UIM_MSG_HIGH_0("Successfully closed existing socket. PDN connection kept open.");

        /* Reset ch status to ESTK_CH_PENDING so new socket can be created for next OPEN CH*/
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;

        return estk_bip_send_terminal_response(
                 cmd_ref_id,
                 GSTK_CLOSE_CH_CNF,
                 GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                 NULL,
                 NULL,
                 index);
      }

      dss_status = dsnet_stop(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                              &error_num );
      if(dss_status == DSS_ERROR)
      {
        UIM_MSG_HIGH_1("dsnet_stop returns DSS_ERROR with error_num %d",
                       error_num);
        switch(error_num)
        {
        case DS_EWOULDBLOCK:
        case DS_ENETCLOSEINPROGRESS:
          return ESTK_SUCCESS;
        default:
          if(error_num == DS_EBADAPP)
          {
            UIM_MSG_ERR_0("Invalid network handle");
          }

          /* Handle the Socket Close error */
          return estk_bip_process_sock_close_error(index,
                                                   cmd_ref_id,
                                                   general_result,
                                                   &additional_info,
                                                   &tr_extra_param);
        }
      }
      else if(dss_status == DSS_SUCCESS)
      {
        UIM_MSG_HIGH_0("dsnet_stop succeeded");
      }
      else
      {
        UIM_MSG_ERR_0("Unhandled return status for close channel");

        /* Handle the Socket Close error */
        return estk_bip_process_sock_close_error(index,
                                                 cmd_ref_id,
                                                 general_result,
                                                 &additional_info,
                                                 &tr_extra_param);
      }
      dss_status = dsnet_release_handle(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                               &error_num );

      if (estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle ==
          estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4)
      {
        estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 = DSS_ERROR;
      }
      if (estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle ==
          estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6)
      {
        estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 = DSS_ERROR;
      }

      estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle = DSS_ERROR;
      UIM_MSG_HIGH_2("dsnet_release_handle returns %d with error_num %d",
                     dss_status, error_num);

      /* If fallback is in progress, start setup of the fallback bearer */
      if(estk_curr_inst_ptr->bip_channel_table[index].status ==
         ESTK_CH_FALLBACK_IN_PROGRESS)
      {
        /* Fallback to Cellular if current network is WLAN and vice-versa */
        nw_type = (estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN) ?
                    ESTK_NETWORK_CELLULAR : ESTK_NETWORK_WLAN;
        estk_status = estk_populate_open_ch_table(index, nw_type);
      }
      else
      {
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;
        estk_status = ESTK_ERROR;
      }

      if(estk_status == ESTK_SUCCESS)
      {
        return estk_status;
      }

      /* If the proactive command is Close Channel, then success TR to be sent,
         as Socket is successfully closed */
      if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
         GSTK_CLOSE_CH_REQ)
      {
        general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

        /* No additional info to be sent in success case */
        additional_info.length = 0;
        additional_info.additional_info_ptr = NULL;
      }

      return estk_bip_process_sock_close_error(index,
                                               cmd_ref_id,
                                               general_result,
                                               &additional_info,
                                               &tr_extra_param);
    }
    else if(dss_status == DSS_ERROR)
    {
      UIM_MSG_HIGH_1("dss_close returns DSS_ERROR with error_num %d",
                     error_num);

      if(error_num == DS_EWOULDBLOCK)
      {
        dss_status = dss_async_select(sock_fd, DS_CLOSE_EVENT, &error_num);
        if(dss_status != DSS_ERROR)
        {
          return ESTK_SUCCESS;
        }
        /* Corner case scenario - not expected to happen */
        else
        {
          UIM_MSG_ERR_0("dss_async_select failed");
        }
      }
      else
      {
        /* IP filter is not register for WLAN. Hence, no need to de-register */
        if(estk_curr_inst_ptr->bip_channel_table[index].nw_type != ESTK_NETWORK_WLAN)
        {
          dss_status = dss_dereg_ip_filter(
                       sock_fd,
                       estk_curr_inst_ptr->bip_channel_table[index].iface_id,
                       &error_num);
          if(dss_status != 0)
          {
            UIM_MSG_MED_2("dss_dereg_ip_filter failed with status 0x%x error number 0x%x",
                      dss_status, error_num);
          }
        }
      }

      /* Force sock_fd to DSS_ERROR, so that Channel table index can be freed
         up and error TR sent to card */
      estk_curr_inst_ptr->bip_channel_table[index].sock_fd = DSS_ERROR;

      if (estk_curr_inst_ptr->keep_ch_open &&
          estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
          GSTK_CLOSE_CH_REQ)
      {
        UIM_MSG_HIGH_0("Failed to close existing socket. PDN connection kept open.");

        /* Reset ch status to ESTK_CH_PENDING so new socket can be created for next OPEN CH*/
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;

        if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
        {
          UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
          estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
          estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
          estk_cache_additional_info(&additional_info);
          return ESTK_SUCCESS;
        }
        else
        {
          return estk_bip_send_terminal_response(
                   cmd_ref_id,
                   GSTK_CLOSE_CH_CNF,
                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                   &additional_info,
                   NULL,
                   index);
        }
      }

      /* Handle the Socket Close error */
      return estk_bip_process_sock_close_error(index,
                                               cmd_ref_id,
                                               general_result,
                                               &additional_info,
                                               &tr_extra_param);
    }
  }
  else
  {
    /* If Open Channel has already been processed */
    if ((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_ACTIVE) ||
        ((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING) &&
         (estk_curr_inst_ptr->bip_channel_table[index].on_demand_link == TRUE)))
    {
      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;
      return estk_bip_send_ch_status_event(
               estk_curr_inst_ptr->bip_channel_table[index].channel_id,
               GSTK_CH_STATUS_LINK_DROPPED);
    }
    else
    {
      UIM_MSG_HIGH_0("Link Drop already sent");
      return ESTK_SUCCESS;
    }
  }
  return ESTK_ERROR;
} /* estk_bip_process_sock_close_evt */

/*===========================================================================

FUNCTION: estk_bip_process_sock_close_error

DESCRIPTION :
  This function processes a close socket error and sends down
  TR to GSTK based on the proactive command in progress

PARAMETERS:
  index :                 [Input] Index of the channel on which error occured
  cmd_detail_ref_id:      [Input] Specifies the reference ID for command details of
                                  the proactive command that the client is sending
                                  a terminal response to. The reference id value
                                  is sent to the client earlier in the structure
                                  of the proactive command request, inside the
                                  gstk_exp_hdr_type
  general_result:         [Input] Specifies the result of the proactive command
                                  operation
  additional_result_ptr:  [Input] Pointer specifies the optional additional result
                                  for the proactive command, the length field in
                                  the gstk_additonal_info_type specifies number of
                                  additional_info in the structure
  other_info_ptr:         [Input] A union pointer of various extra parameters for the
                                  various proactive command response, please refer to
                                  gstk_terminal_rsp_param_type for details

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_process_sock_close_error(
  int                                 index,
  uint32                              cmd_ref_id,
  gstk_general_result_enum_type       general_result,
  gstk_additional_info_ptr_type      *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type *other_info_ptr
)
{
  estk_result_enum_type estk_status = ESTK_ERROR;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  /* Fallback has to be stopped due to Socket Close Error */
  if(estk_curr_inst_ptr->bip_channel_table[index].status ==
     ESTK_CH_FALLBACK_IN_PROGRESS)
  {
    estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_CLOSE_CH_REQ)
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = general_result;
      estk_cache_additional_info(additional_result_ptr);
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      return ESTK_SUCCESS;
    }
    else
    {
      (void)estk_bip_clean_up_ch_table(index);

      return estk_bip_send_terminal_response(
               cmd_ref_id,
               GSTK_CLOSE_CH_CNF,
               general_result,
               additional_result_ptr,
               NULL,
               index);
    }
  }
  else if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ)
  {
    (void)estk_bip_clean_up_ch_table(index);

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             general_result,
             additional_result_ptr,
             other_info_ptr,
             index);
  }
  else if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_SEND_DATA_REQ)
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = general_result;
      estk_cache_additional_info(additional_result_ptr);
      estk_status = ESTK_SUCCESS;
    }
    else
    {
      estk_status = estk_bip_send_terminal_response(
                      cmd_ref_id,
                      GSTK_SEND_DATA_CNF,
                      general_result,
                      additional_result_ptr,
                      NULL,
                      index);
    }

    (void)estk_bip_send_ch_status_event(
            estk_curr_inst_ptr->bip_channel_table[index].channel_id,
            GSTK_CH_STATUS_LINK_DROPPED);

    return estk_status;
  }
  else
  {
    return ESTK_SUCCESS;
  }
} /* estk_bip_process_sock_close_error */

/*===========================================================================

FUNCTION: estk_bip_process_sock_evt

DESCRIPTION :
  This function processes a socket event from DS

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_sock_evt(
  const estk_cmd_type *cmd_ptr
)
{
  estk_result_enum_type       estk_status  = ESTK_ERROR;

  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_3("estk_bip_process_sock_evt: evt_type=0x%x, sock_fd=0x%x, "
                 "cmd_ref_id=0x%x",
                 cmd_ptr->cmd_data.sock_evt.evt_type,
                 cmd_ptr->cmd_data.sock_evt.sock_fd,
                 cmd_ptr->hdr.cmd_ref_id);

  if(cmd_ptr->cmd_data.sock_evt.evt_type & ESTK_DS_CLOSE_EVENT)
  {
    estk_status = estk_bip_process_sock_close_evt(
                    cmd_ptr->cmd_data.sock_evt.sock_fd,
                    cmd_ptr->hdr.cmd_ref_id);
  }
  /* Read and Write events can come in together */
  else
  {
    if(cmd_ptr->cmd_data.sock_evt.evt_type & (uint32)ESTK_DS_READ_EVENT)
    {
      estk_status = estk_bip_process_sock_read_evt(
                    cmd_ptr->cmd_data.sock_evt.sock_fd);
      UIM_MSG_HIGH_1("estk_bip_process_sock_read_evt() rc=0x%x", estk_status);
    }
    if(cmd_ptr->cmd_data.sock_evt.evt_type & (uint32)ESTK_DS_WRITE_EVENT)
    {
    estk_status = estk_bip_process_sock_write_evt(
                    cmd_ptr->cmd_data.sock_evt.sock_fd,
                    cmd_ptr->hdr.cmd_ref_id);
    }
  }
  UIM_MSG_HIGH_1("estk_bip_process_sock_evt: rc=0x%x", estk_status);
  return estk_status;
} /* estk_bip_process_sock_evt */

/*===========================================================================
FUNCTION estk_bip_ds_iface_event_cb

DESCRIPTION
  Callback function used for the DS Iface events

PARAMETERS
  event: [Input] Event notified using callback by Data Services
  event_info: [Input] Event Related information
  user_data: [Input] User Data
  app_id: [Input] Application Id
  iface_id: [Input] Interface Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void estk_bip_ds_iface_event_cb (
    dss_iface_ioctl_event_enum_type        event,
    dss_iface_ioctl_event_info_union_type  event_info,
    void                                  *user_data,
    sint15                                 app_id,
    dss_iface_id_type                      iface_id
)
{
  int                     index   = -1;
  estk_cmd_type          *cmd_ptr = NULL;
  gstk_slot_id_enum_type  slot_id = GSTK_SLOT_ID_MAX;

  (void)event_info;
  (void)iface_id;

  UIM_MSG_HIGH_3("estk_bip_ds_iface_event_cb: event=0x%x, app_id=0x%x, user_data=0x%x",
                 event, app_id, user_data);

  if(user_data == NULL)
  {
    return;
  }

  slot_id = (*(gstk_slot_id_enum_type *)user_data);

  UIM_MSG_HIGH_1("estk_bip_ds_iface_event_cb: slot_id=0x%x",
                 slot_id);

  if(!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",slot_id);
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  for(index = 0;index < ESTK_MAX_BIP_CH; index++)
  {
    if(estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[index].sock_nethandle == app_id)
    {
      break;
    }
  }
  UIM_MSG_HIGH_2("estk_bip_ds_iface_event_cb for event %d index %d", event, index);

  if(index == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Network handle not found for WLAN IP address change event");
    return;
  }

  switch (event)
  {
  case DSS_IFACE_IOCTL_ADDR_CHANGED_EV:
    cmd_ptr = estk_get_cmd_buf();
    if (cmd_ptr != NULL)
    {
      cmd_ptr->hdr.cmd = ESTK_BIP_WLAN_IP_ADDR_CHANGE_EVT_CMD;
      cmd_ptr->hdr.sim_slot_id = slot_id;
      estk_put_cmd(cmd_ptr);
    }
    else
    {
      UIM_MSG_ERR_0("failed to get cmd buf!");
    }
    break;
  default:
    UIM_MSG_HIGH_0("Unhandled event by ESTK at this time");
    break;
  }
} /*estk_bip_ds_iface_event_cb*/

/*===========================================================================

FUNCTION: estk_bip_reg_ds_iface_event

DESCRIPTION :
  This function creates registers an iface event with DS

PARAMETERS:
  ds_iface_event: [Input] The iface event needed to register with DS
  dss_nethandle : [Input] Network handle of the active channel
  yes_or_no: [Input] Boolean to decide to register or deregister
  index: [Input] Index of current open channel session in internal table

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_bip_reg_ds_iface_event(
  dss_iface_ioctl_event_enum_type     ds_iface_event,
  sint15                              dss_nethandle,
  boolean                             yes_or_no,
  int                                 index
)
{
  dss_iface_ioctl_ev_cb_type dss_iface_ev_cb;
  sint15                     error_num = DSS_ERROR;

  UIM_MSG_HIGH_3("estk_bip_reg_ds_iface_event %d for event %d and handle %d",
                 yes_or_no, ds_iface_event, dss_nethandle);

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(ds_iface_event != DSS_IFACE_IOCTL_ADDR_CHANGED_EV)
  {
    UIM_MSG_ERR_0("DS iface event registration unsupported at this time");
    return;
  }

  if(index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid channel");
    return;
  }

  memset((void *)&dss_iface_ev_cb, 0, sizeof(dss_iface_ioctl_ev_cb_type));

  dss_iface_ev_cb.event_cb =
     (dss_iface_ioctl_event_cb)estk_bip_ds_iface_event_cb;
  dss_iface_ev_cb.event         = ds_iface_event;
  dss_iface_ev_cb.user_data_ptr = (void *)&(estk_curr_inst_ptr->slot_id);
  dss_iface_ev_cb.app_id        = dss_nethandle;

  if(estk_curr_inst_ptr->bip_channel_table[index].iface_id == DSS_IFACE_INVALID_ID)
  {
    UIM_MSG_ERR_0("dss_get_iface_id failed");
    return;
  }

  if(yes_or_no)
  {
    if(dss_iface_ioctl(
         (dss_iface_id_type)estk_curr_inst_ptr->bip_channel_table[index].iface_id,
         DSS_IFACE_IOCTL_REG_EVENT_CB,
         (void *)&dss_iface_ev_cb,
         &error_num) != 0)
    {
      UIM_MSG_ERR_2(" Register for %d event returned error 0x%x",
                    ds_iface_event, error_num);
      return;
    }
  }
  else
  {
    if(dss_iface_ioctl(
         (dss_iface_id_type)estk_curr_inst_ptr->bip_channel_table[index].iface_id,
         DSS_IFACE_IOCTL_DEREG_EVENT_CB,
         (void *)&dss_iface_ev_cb,
         &error_num) != 0)
    {
      UIM_MSG_ERR_2(" De-Register for %d event returned error 0x%x",
                    ds_iface_event, error_num);
      return;
    }
  }

} /*estk_bip_reg_ds_iface_event*/

/*===========================================================================

FUNCTION: estk_bip_network_cb

DESCRIPTION :
  This function serves as cb for the network related events

PARAMETERS:
  dss_nethandle    : [Input] Application id
  iface_id         : [Input] Interface id
  errno            : [Input] type of network error, ENETISCONN, ENETNONET
  net_cb_user_data : {Input] User data specfied during registration

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_network_cb(
  sint15             dss_nethandle,
  dss_iface_id_type  iface_id,
  sint15             error_num,
  void              *net_cb_user_data)
{
  estk_cmd_type          *task_cmd_ptr = NULL;
  gstk_slot_id_enum_type  slot_id      = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_3("estk_bip_network_cb: dss_nethandle=0x%x, error_num=0x%x, net_cb_user_data=0x%x",
                 dss_nethandle,
                 error_num,
                 net_cb_user_data);

  if(net_cb_user_data == NULL)
  {
    return;
  }

  slot_id = (*(gstk_slot_id_enum_type *)net_cb_user_data);

  UIM_MSG_HIGH_1("estk_bip_network_cb: slot_id=0x%x", slot_id);

  if(!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot_id);
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr != NULL)
  {
    task_cmd_ptr->hdr.cmd_ref_id =
      estk_instances_ptr[(uint32)slot_id - 1]->curr_cmd.hdr_cmd.cmd_detail_reference;
    task_cmd_ptr->hdr.cmd                            = ESTK_BIP_PROCESS_NET_CB_CMD;
    task_cmd_ptr->hdr.sim_slot_id                    = slot_id;
    task_cmd_ptr->cmd_data.net_cb_info.dss_nethandle = dss_nethandle;
    task_cmd_ptr->cmd_data.net_cb_info.iface_id      = iface_id;
    task_cmd_ptr->cmd_data.net_cb_info.error_num     = error_num;
    estk_put_cmd(task_cmd_ptr);
  }
}


/*===========================================================================

FUNCTION: estk_bip_socket_cb

DESCRIPTION :
  This function serves as cb for the socket related events

PARAMETERS:
  dss_nethandle     : [Input] Application id
  sock_fd           : [Input] Socket descriptor
  event_mask        : [Input] Event occured
  sock_cb_user_data : [Input] User data specfied during registration

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_socket_cb (
  sint15  dss_nethandle,
  sint15  sock_fd,
  uint32  event_mask,
  void   *sock_cb_user_data)
{
  estk_cmd_type          *task_cmd_ptr = NULL;
  uint8                   i            = 0;
  gstk_slot_id_enum_type  slot_id      = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_3("estk_bip_socket_cb: dss_nethandle=0x%x, sock_fd=0x%x, event_mask=0x%x",
                 dss_nethandle,
                 sock_fd,
                 event_mask);

  if(sock_cb_user_data == NULL)
  {
    UIM_MSG_ERR_0("sock_cb_user_data is NULL");
    return;
  }

  slot_id = (*(gstk_slot_id_enum_type *)sock_cb_user_data);

  UIM_MSG_HIGH_1("estk_bip_socket_cb: slot_id=0x%x",
                 slot_id);

  if(!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",slot_id);
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  for(i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if(estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].sock_nethandle_v4 == dss_nethandle ||
       estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].sock_nethandle_v6 == dss_nethandle ||
       estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].sock_nethandle == dss_nethandle)
    {
      break;
    }
  }

  if(i == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_HIGH_0("Invalid network event notication from DS");
    return;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf();

  if(task_cmd_ptr != NULL)
  {
    task_cmd_ptr->hdr.cmd_ref_id =
      estk_instances_ptr[(uint32)slot_id - 1]->curr_cmd.hdr_cmd.cmd_detail_reference;
    task_cmd_ptr->hdr.sim_slot_id            = slot_id;
    task_cmd_ptr->hdr.cmd                    = ESTK_BIP_SOCK_EVT_CMD;
    task_cmd_ptr->cmd_data.sock_evt.evt_type = event_mask;
    task_cmd_ptr->cmd_data.sock_evt.sock_fd  = sock_fd;
    task_cmd_ptr->result                     = ESTK_SUCCESS;
    estk_put_cmd(task_cmd_ptr);
  }
}/* estk_bip_socket_cb */

/*===========================================================================

FUNCTION: estk_perform_net_disconnect

DESCRIPTION :
  This function tears down the link and takes care of releasing resources

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_perform_net_disconnect(
  sint15 dss_nethandle)
{
  uint32                              i           = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res   = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset((void *)&additional_info, 0, sizeof(gstk_additional_info_ptr_type));

  for(i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[i].sock_nethandle == dss_nethandle)
    {
      /* Matching network handle */
      switch(estk_curr_inst_ptr->bip_channel_table[i].current_command_id)
      {
        case GSTK_OPEN_CH_REQ:
          (void)estk_bip_clean_up_ch_table(i);
          /* Send error TR for open channel */
          tr_extra_param.present = TRUE;
          tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
            FALSE;
  
          (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                 sizeof(gstk_bearer_description_type),
                 &estk_curr_inst_ptr->bip_channel_table[i].bearer_desc,
                 sizeof(gstk_bearer_description_type));
  
          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[i].buffer_size;
  
          addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          additional_info.additional_info_ptr = &addi_res;
          additional_info.length = 1;

          if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
          {
            UIM_MSG_HIGH_0("Saving Open Channel Error TR");
            estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
            estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
            estk_cache_additional_info(&additional_info);
            estk_curr_inst_ptr->extra_param = tr_extra_param;
          }
          else
          {
            if(estk_bip_send_terminal_response(
                estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                GSTK_OPEN_CH_CNF,
                GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                &additional_info,
                &tr_extra_param,
                i) != ESTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Sending TR for open channel failed");
              return ESTK_ERROR;
            }
          }
          return ESTK_SUCCESS;
        case GSTK_SEND_DATA_REQ:
          return estk_bip_process_send_data_error(i);
        case GSTK_CLOSE_CH_REQ:
        (void)estk_bip_process_sock_close_evt(
                estk_curr_inst_ptr->bip_channel_table[i].sock_fd,
                estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference);
          return ESTK_SUCCESS;
        default:
          UIM_MSG_HIGH_2("Current command is %d and ch status 0x%x",
                         estk_curr_inst_ptr->bip_channel_table[i].current_command_id,
                         estk_curr_inst_ptr->bip_channel_table[i].status);
          
          /* If Open Channel has already been processed */
          if ((estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_ACTIVE) ||
            ((estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_PENDING) &&
             (estk_curr_inst_ptr->bip_channel_table[i].on_demand_link == TRUE)))
          {
            estk_curr_inst_ptr->bip_channel_table[i].status = ESTK_CH_DROPPED;
            /* Send Channel status Event Download envelope command */
            (void)estk_bip_send_ch_status_event(
                  estk_curr_inst_ptr->bip_channel_table[i].channel_id,
                  GSTK_CH_STATUS_LINK_DROPPED);
          }
          return ESTK_SUCCESS;        
      } /* switch */
    }
  }
  return ESTK_ERROR;
} /*estk_perform_net_disconnect*/

/*===========================================================================

FUNCTION: estk_process_net_cb_cmd

DESCRIPTION :
  This function processes the net callback result in ESTK context

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_net_cb_cmd(
  const estk_cmd_type *cmd_ptr)
{
  gstk_open_ch_req_type              *open_ch_cmd   = NULL;
  boolean                            is_v6_cb = FALSE;
  boolean                            send_tr = FALSE;
  sint15                             dss_nethandle = cmd_ptr->cmd_data.net_cb_info.dss_nethandle;
  sint15                             error_num = cmd_ptr->cmd_data.net_cb_info.error_num;
  estk_result_enum_type              result = ESTK_SUCCESS;
  uint8                              i = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[i].sock_nethandle_v4 == dss_nethandle ||
       estk_curr_inst_ptr->bip_channel_table[i].sock_nethandle_v6 == dss_nethandle)
    {
      break;
    }
  }

  if (i == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid network event notication from DS");
    return ESTK_ERROR;
  }

  if (estk_curr_inst_ptr->bip_channel_table[i].sock_nethandle_v6 == dss_nethandle)
  {
    is_v6_cb = TRUE;
    estk_curr_inst_ptr->bip_channel_table[i].ds_netcb_status_v6 = error_num;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[i].ds_netcb_status_v4 = error_num;
  }

  UIM_MSG_HIGH_2("estk_process_net_cb_cmd: is_v6_cb = %d, error_num = %d",
                 is_v6_cb, error_num);

  if (open_ch_cmd != NULL && open_ch_cmd->dns_requested)
  {
    if(gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
         estk_curr_inst_ptr->slot_id) == TRUE)
    {
      /* v4 only, ready to send TR */
      send_tr = TRUE;
    }
    else
    {
      /* v4 and v6 */
      if (is_v6_cb)
      {
        /* got v6 callback, ready to send TR */
        send_tr = TRUE;
      }
      else if (estk_curr_inst_ptr->bip_channel_table[i].sock_nethandle_v6 == DSS_ERROR)
      {
        /* got v4 callback, bring up v6 iface */
        UIM_MSG_HIGH_0("estk_process_net_cb_cmd: Bring up IPv6 iface");
        result = estk_bringup_iface(i, DSS_AF_INET6);
        /*
          If ESTK cannot bring up IPV6 iface, then it needs to always send DNS TR because:

          1) If IPV4 bring up succeeded - send the IPV4 DNS addresses to card
          2) If IPV4 bring up failed - send an error TR. This is guaranteed
             because both v4 and v6 nethandle is DSS_ERROR
        */
        if (result == ESTK_ERROR)
        {
          send_tr = TRUE;
        }
      }
    }

    if (send_tr)
    {
      UIM_MSG_HIGH_0("estk_process_net_cb_cmd: Send OPEN CH TR for DNS addresses");
      result = estk_send_dns_tr(i);
    }
  }
  else
  {
    UIM_MSG_HIGH_0("estk_process_net_cb_cmd: No DNS requested by card");
    switch(error_num)
    {
    case DS_ENETISCONN:
      result = estk_perform_net_connect();
      break;
    case DS_ENETNONET:
      result = estk_perform_net_disconnect(dss_nethandle);
      break;
    default:
      UIM_MSG_HIGH_1("Network CB returns Network not connected %d", error_num);
      result = ESTK_SUCCESS;
      break;
    }
  }

  return result;
} /* estk_process_net_cb_cmd */

/*===========================================================================

FUNCTION: estk_send_dns_tr

DESCRIPTION :
  This function sends the DNS addresses to card in a Terminal Response

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_dns_tr(
  uint32 index)
{
  estk_result_enum_type              estk_status  = ESTK_ERROR;
  int16                              dsErrno = 0;
  int16                              result = 0;
  dss_iface_id_type                  iface_id = DSS_IFACE_INVALID_ID;
  dss_iface_ioctl_get_all_dns_addrs_type argVal;
  uint32                             i = 0;
  gstk_terminal_rsp_extra_param_type tr_extra_param;
  gstk_additional_info_ptr_type      additional_info;
  uint8                              addi_res;
  gstk_general_result_enum_type      gen_result = GSTK_GENERAL_RESULT_MAX;
  uint32                             cmd_ref_id;

  UIM_MSG_HIGH_1("estk_send_dns_tr, index %d", index);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));
  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;

  if (index >= ESTK_MAX_BIP_CH ||
      (estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 == DSS_ERROR &&
       estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 == DSS_ERROR) ||
      (estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 != DS_ENETISCONN &&
       estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v6 != DS_ENETISCONN))
  {
    /* Send error TR */
    UIM_MSG_ERR_1("estk_send_dns_tr, no valid nethandle or index %d out of bound",
                  index);
    gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
    addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    additional_info.length = 1;
    additional_info.additional_info_ptr = &addi_res;

    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
      FALSE;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_status_additional_info =
      GSTK_CH_STATUS_NO_FURTHER_INFO;
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
           sizeof(gstk_bearer_description_type),
           &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
           sizeof(gstk_bearer_description_type));

    goto ESTK_DNS_ADDRS_SEND_TR;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 != DSS_ERROR)
  {
    iface_id = dss_get_iface_id(
                 estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4);
  }

  /* Reset number of DNS addresses saved */
  estk_curr_inst_ptr->dns_addrs_len = 0;
  if (DSS_IFACE_INVALID_ID != iface_id)
  {
    argVal.dns_addrs_ptr = estk_curr_inst_ptr->dns_addrs;
    argVal.num_dns_addrs = PS_IFACE_NUM_DNS_ADDRS;
    result = dss_iface_ioctl(
      iface_id,
      DSS_IFACE_IOCTL_GET_ALL_DNS_ADDRS,
      &argVal,
      &dsErrno);

    if (DSS_SUCCESS == result)
    {
      UIM_MSG_HIGH_0("Retrieved v4 iface DNS addresses");

      /* Update number of DNS addresses saved so far */
      estk_curr_inst_ptr->dns_addrs_len += argVal.num_dns_addrs;
      for (i = 0; i < estk_curr_inst_ptr->dns_addrs_len &&
           i < (sizeof(estk_curr_inst_ptr->dns_addrs) / sizeof(ip_addr_type)); ++ i)
      {
        if (estk_curr_inst_ptr->dns_addrs[i].type == IPV4_ADDR)
        {
          UIM_MSG_HIGH_2("DNS[%d]: 0x%x",
                         i, estk_curr_inst_ptr->dns_addrs[i].addr.v4);
        }
      }
    }
  }

  if(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
       estk_curr_inst_ptr->slot_id) == FALSE)
  {
    iface_id = DSS_IFACE_INVALID_ID;
    if (estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 != DSS_ERROR)
    {
      iface_id = dss_get_iface_id(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6);
    }
    if (DSS_IFACE_INVALID_ID != iface_id &&
        estk_curr_inst_ptr->dns_addrs_len <= PS_IFACE_NUM_DNS_ADDRS)
    {
      argVal.dns_addrs_ptr = &estk_curr_inst_ptr->dns_addrs[estk_curr_inst_ptr->dns_addrs_len];
      argVal.num_dns_addrs = PS_IFACE_NUM_DNS_ADDRS;
      result = dss_iface_ioctl(
        iface_id,
        DSS_IFACE_IOCTL_GET_ALL_DNS_ADDRS,
        &argVal,
        &dsErrno);

      if (DSS_SUCCESS == result)
      {
        /* Update number of DNS addresses */
        estk_curr_inst_ptr->dns_addrs_len += argVal.num_dns_addrs;
        UIM_MSG_HIGH_0("Retrieved v6 iface DNS addresses");
        for (i = 0; i < estk_curr_inst_ptr->dns_addrs_len &&
             i < (sizeof(estk_curr_inst_ptr->dns_addrs) / sizeof(ip_addr_type)); ++ i)
        {
          if (estk_curr_inst_ptr->dns_addrs[i].type == IPV6_ADDR)
          {
            UIM_MSG_HIGH_3("DNS[%d]: 0x%lx 0x%lx",
                           i,
                           estk_curr_inst_ptr->dns_addrs[i].addr.v6[0],
                           estk_curr_inst_ptr->dns_addrs[i].addr.v6[1]);
          }
        }
      }
    }
  }

  UIM_MSG_HIGH_1("Number of DNS addresses: %d",
                 estk_curr_inst_ptr->dns_addrs_len);

  if (estk_curr_inst_ptr->dns_addrs_len > 0)
  {
    tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses =
      estk_curr_inst_ptr->dns_addrs_len;
  }
  else
  {
    UIM_MSG_HIGH_0("No addresses retrieved, pack dummy address");
    tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses = 1;
  }

  if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses > 0)
  {
    tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses =
      GSTK_CALLOC(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses * sizeof(gstk_data_address_type));
    if (!tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses)
    {
      /* Send error TR */
      gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      additional_info.length = 1;
      additional_info.additional_info_ptr = &addi_res;
      goto ESTK_DNS_ADDRS_SEND_TR;
    }
    memset(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses,
           0x00,
           tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses * sizeof(gstk_data_address_type));
  }

  gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  additional_info.length = 0;
  additional_info.additional_info_ptr = NULL;

  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
  (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                sizeof(gstk_bearer_description_type),
                &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
                sizeof(gstk_bearer_description_type));
  tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
  tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
    estk_curr_inst_ptr->bip_channel_table[index].channel_id;
  tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
    TRUE;

  UIM_MSG_HIGH_0("Convert ip_addr_type to gstk_data_address_type");
  for (i = 0; i < estk_curr_inst_ptr->dns_addrs_len; ++ i)
  {
    if (estk_curr_inst_ptr->dns_addrs[i].type == IPV4_ADDR)
    {
      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address =
        GSTK_CALLOC(GSTK_IPV4_ADDRESS_LEN);
      if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address)
      {
        tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].length = 4;
        (void)memscpy(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].length,
                      &estk_curr_inst_ptr->dns_addrs[i].addr.v4,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].length);
      }
    }
    else if (estk_curr_inst_ptr->dns_addrs[i].type == IPV6_ADDR)
    {
      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address =
        GSTK_CALLOC(GSTK_IPV6_ADDRESS_LEN);
      if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address)
      {
        tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].length = 16;
        (void)memscpy(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].length,
                      estk_curr_inst_ptr->dns_addrs[i].addr.v6,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].length);
      }
    }
    else
    {
      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].length = 0;
      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address = NULL;
    }
  }

ESTK_DNS_ADDRS_SEND_TR:
  if (gen_result != GSTK_COMMAND_PERFORMED_SUCCESSFULLY && index != -1)
  {
    estk_bip_clean_up_ch_table(index);
  }

  estk_status = estk_bip_send_terminal_response(
                  cmd_ref_id,
                  GSTK_OPEN_CH_CNF,
                  gen_result,
                  &additional_info,
                  &tr_extra_param,
                  index);

  if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses)
  {
    for (i = 0; i < tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses; ++i)
    {
      if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address)
      {
        gstk_free(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address);
        tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address = NULL;
      }
    }
    gstk_free(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses);
    tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses = NULL;
  }

  return estk_status;
} /* estk_send_dns_tr */

/*===========================================================================

FUNCTION: estk_bip_setup_data_filter

DESCRIPTION :
  This function sets up a data filter

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_setup_data_filter(int32 index)
{
  struct ps_sockaddr_storage  local_addr;
  struct ps_sockaddr_in      *p_inaddr_v4    = NULL;
  struct ps_sockaddr_in6     *p_inaddr_v6    = NULL;
  struct ps_sockaddr_in      *p_dest_addr_v4 = NULL;
  struct ps_sockaddr_in6     *p_dest_addr_v6 = NULL;
  sint31                      dss_status     = DSS_ERROR;
  sint15                      error_num      = DSS_ERROR;
  uint16                      sock_addr_len  = 0;
  int                         ret            = DSS_ERROR;
  uint32                      opt_len        = 4;         /* DSS requires 4 */
  boolean                     sys_sock_value = FALSE;
  ip_filter_type              filters[1];

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_bip_setup_data_filter: index=0x%x, dest_addr=0x%x",
                 index, estk_curr_inst_ptr->bip_channel_table[index].dest_addr);

  if (estk_curr_inst_ptr->bip_channel_table[index].dest_addr == NULL)
  {
    return ESTK_ERROR;
  }

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
  p_dest_addr_v6 =
    (struct ps_sockaddr_in6*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;

  sys_sock_value = TRUE;

  dss_status = dss_setsockopt(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                              DSS_SOCK,
                              DSS_SO_SYS_SOCK,
                              (void *)&sys_sock_value,
                              &opt_len,
                              &error_num);

  UIM_MSG_HIGH_2("Setting SO_SYS_SOCK option, returned %d and error_num %d",
                 dss_status, error_num);

  (void)gstk_memcpy(&local_addr,
                 estk_curr_inst_ptr->bip_channel_table[index].dest_addr,
                 sizeof(struct ps_sockaddr_storage),
                 sizeof(struct ps_sockaddr_storage),
                 sizeof(struct ps_sockaddr_storage));

  p_inaddr_v4 = (struct ps_sockaddr_in*)&local_addr;
  p_inaddr_v6 = (struct ps_sockaddr_in6*)&local_addr;

  switch(p_inaddr_v4->ps_sin_family)
  {
  case DSS_AF_INET:
    p_inaddr_v4->ps_sin_addr.ps_s_addr = dss_htonl(PS_INADDR_ANY);
    sock_addr_len = sizeof(struct ps_sockaddr_in);
    break;
  case DSS_AF_INET6:
    p_inaddr_v6->ps_sin6_addr = ps_in6addr_any;
    sock_addr_len = sizeof(struct ps_sockaddr_in6);
    break;
  default:
    UIM_MSG_ERR_1("invalid ps_sin_family 0x%x!", p_inaddr_v4->ps_sin_family);
    return ESTK_ERROR;
  }

  /*
    Use ephemeral port - when port number 0 is passed,
    DS assigns a random port number from the port range
    reserved for modem
  */
  p_inaddr_v4->ps_sin_port = 0;

  /* Bind port to the dss_socket */
  dss_status = dss_bind(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                        (struct ps_sockaddr *)&local_addr,
                        sock_addr_len,
                        &error_num);

  if(dss_status != DSS_SUCCESS)
  {
    UIM_MSG_ERR_1("dss_bind failed with error_num %d", error_num);
    return ESTK_ERROR;
  }

  /* Get dss_socket2's port. Used to populate IP filter */
  ret = dss_getsockname(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                        (struct ps_sockaddr *)&local_addr,
                        &sock_addr_len,
                        &error_num);

  if(dss_status != DSS_SUCCESS)
  {
    UIM_MSG_ERR_1("dss_getsockname failed with error_num %d", error_num);
    return ESTK_ERROR;
  }
  else
  {
    UIM_MSG_HIGH_3("Local addr family=%d, port=%d, addr=0x%x",
                   p_inaddr_v4->ps_sin_family,
                   p_inaddr_v4->ps_sin_port,
                   p_inaddr_v4->ps_sin_addr.ps_s_addr);
  }

  /* Cache ephemeral port number from DS in host format */
  estk_curr_inst_ptr->bip_channel_table[index].me_port_num =
    dss_ntohs(p_inaddr_v4->ps_sin_port);

  /* IP Filter setup is not required for WLAN, hence return */
  if(estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN)
  {
   return ESTK_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Populate filter using destination machine's IP address as source address,
    destination socket's port as source port, and dss_socket2's port as destination
    port
  -------------------------------------------------------------------------*/
  switch(p_inaddr_v4->ps_sin_family)
  {
    case DSS_AF_INET:
      filters[0].ip_vsn = IP_V4;
      filters[0].ip_hdr.v4.field_mask = (IPFLTR_MASK_IP4_SRC_ADDR |
                                          IPFLTR_MASK_IP4_NEXT_HDR_PROT);
      filters[0].ip_hdr.v4.src.addr.ps_s_addr =
        p_dest_addr_v4->ps_sin_addr.ps_s_addr;
      filters[0].ip_hdr.v4.src.subnet_mask.ps_s_addr = dss_htonl(0xFFFFFFFF);
      switch(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol)
      {
        case GSTK_PKT_DATA_TCP:
          filters[0].ip_hdr.v4.next_hdr_prot = PS_IPPROTO_TCP;
          break;
        case GSTK_PKT_DATA_UDP:
          filters[0].ip_hdr.v4.next_hdr_prot = PS_IPPROTO_UDP;
          break;
        default:
          filters[0].ip_hdr.v4.next_hdr_prot = PS_IPPROTO_TCP_UDP;
          break;
      }
      break;
    case DSS_AF_INET6:
      filters[0].ip_vsn = IP_V6;
      filters[0].ip_hdr.v6.field_mask =
        IPFLTR_MASK_IP6_SRC_ADDR | IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      filters[0].ip_hdr.v6.src.addr = p_dest_addr_v6->ps_sin6_addr;
      filters[0].ip_hdr.v6.src.prefix_len = 128;
      switch(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol)
      {
        case GSTK_PKT_DATA_TCP:
          filters[0].ip_hdr.v6.next_hdr_prot = PS_IPPROTO_TCP;
          break;
        case GSTK_PKT_DATA_UDP:
          filters[0].ip_hdr.v6.next_hdr_prot = PS_IPPROTO_UDP;
          break;
        default:
          filters[0].ip_hdr.v6.next_hdr_prot = PS_IPPROTO_TCP_UDP;
          break;
      }
      break;
    default:
      UIM_MSG_ERR_1("invalid ps_sin_family 0x%x!", p_inaddr_v4->ps_sin_family);
      return ESTK_ERROR;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP)
  {
    filters[0].next_prot_hdr.tcp.field_mask = IPFLTR_MASK_TCP_DST_PORT;
    filters[0].next_prot_hdr.tcp.src.port =
      dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
    filters[0].next_prot_hdr.tcp.src.range = 0;
    filters[0].next_prot_hdr.tcp.dst.port = p_inaddr_v4->ps_sin_port;
    filters[0].next_prot_hdr.tcp.dst.range = 0;
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_UDP)
  {
    filters[0].next_prot_hdr.udp.field_mask = IPFLTR_MASK_UDP_DST_PORT;
    filters[0].next_prot_hdr.udp.src.port =
      dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
    filters[0].next_prot_hdr.udp.src.range = 0;
    filters[0].next_prot_hdr.udp.dst.port = p_inaddr_v4->ps_sin_port;
    filters[0].next_prot_hdr.udp.dst.range = 0;
  }

  /*Register socket's based filter*/
  ret = dss_reg_ip_filter(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                          estk_curr_inst_ptr->bip_channel_table[index].iface_id,
                          IPFLTR_DEFAULT_TYPE,
                          1,
                          filters,
                          &error_num);

  if(ret != DSS_SUCCESS)
  {
    UIM_MSG_ERR_1("dss_reg_ip_filter failed with error_num %d", error_num);
    return ESTK_ERROR;
  }

  return ESTK_SUCCESS;
} /*estk_bip_setup_data_filter*/

/*===========================================================================

FUNCTION: estk_bip_copy_pdn_qos_parameters

DESCRIPTION :
  This function copies the updated PDN QOS parameters from Data Services

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_copy_pdn_qos_parameters(
  dss_iface_ioctl_primary_qos_get_granted_flow_spec_type *ds_pdn_qos,
  uint32                                                  index)
{
  UIM_MSG_HIGH_2("estk_bip_copy_pdn_qos_parameters: ds_pdn_qos=0x%x, index=0x%x",
                 ds_pdn_qos, index);

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(ds_pdn_qos == NULL)
  {
    return;
  }

  if (index >= ESTK_MAX_BIP_CH)
  {
    /*invalid ch idx 0x%x!*/
    return;
  }

  switch(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_type)
  {
  case GSTK_EUTRAN_EXT_PARAM:
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.traffic_class
      =  (uint8)ds_pdn_qos->rx_ip_flow.trf_class;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.max_bitrate_ul
      =  (uint32)ds_pdn_qos->tx_ip_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.max_bitrate_dl
      =  (uint32)ds_pdn_qos->rx_ip_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.guaranteed_bitrate_ul
      =  (uint32)ds_pdn_qos->tx_ip_flow.data_rate.format.min_max.guaranteed_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.guaranteed_bitrate_dl
      =  (uint32)ds_pdn_qos->rx_ip_flow.data_rate.format.min_max.guaranteed_rate;
    break;
  case GSTK_EUTRAN_MAPPED_UTRAN_PS:
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.qci
      = (uint8)ds_pdn_qos->rx_ip_flow.lte_params.lte_qci;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_ul
      = (uint8)ds_pdn_qos->tx_ip_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_dl
      = (uint8)ds_pdn_qos->rx_ip_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_ul
      =  (uint8)ds_pdn_qos->tx_ip_flow.data_rate.format.min_max.guaranteed_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_dl
      =  (uint8)ds_pdn_qos->rx_ip_flow.data_rate.format.min_max.guaranteed_rate;
    break;
  default:
    break;
  }
} /* estk_bip_copy_pdn_qos_parameters */

/*===========================================================================

FUNCTION: estk_perform_net_connect

DESCRIPTION :
  This function established the link

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_perform_net_connect(void)
{
  struct ps_sockaddr_in                                  *p_dest_addr_v4 = NULL;
  struct ps_sockaddr_in6                                 *p_dest_addr_v6 = NULL;
  uint16                                                 addr_size;
  sint31                                                 dss_status      = DSS_ERROR;
  sint15                                                 error_num       = DSS_ERROR;
  int32                                                  index           = -1;
  sint15                                                 sock_fd         = DSS_ERROR;
  gstk_general_result_enum_type                          gen_result      = GSTK_GENERAL_RESULT_MAX;
  gstk_terminal_rsp_extra_param_type                     tr_extra_param;
  gstk_additional_info_ptr_type                          additional_info;
  uint8                                                  addi_res;
  dss_iface_ioctl_primary_qos_get_granted_flow_spec_type ds_pdn_qos;
  uint32                                                 cmd_ref_id      = 0;
  estk_result_enum_type                                  estk_status     = ESTK_ERROR;

/* Call dss_netstatus to find the current network status */

  UIM_MSG_HIGH_0("estk_perform_net_connect");

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));
  memset(&ds_pdn_qos, 0x00, sizeof(ds_pdn_qos));

  for(index = 0; index < ESTK_MAX_BIP_CH; index++)
  {
    p_dest_addr_v4 =
      (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
    p_dest_addr_v6 =
      (struct ps_sockaddr_in6*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;

    if(estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING)
    {
      UIM_MSG_HIGH_1("Found channel %d in ESTK_CH_PENDING", index);

      /* save iface_id and set scope_id for IPv6 */
      if (p_dest_addr_v6 == NULL)
      {
        UIM_MSG_ERR_0("invalid dest addr!");
        /* Continue looping through the channel table, since OPEN CH DNS query
           could have empty destination address. Theoretically this scenario
           should not occur because card is expected to send CLOSE CH for OPEN
           CH DNS query before sending new OPEN CHs. */
        continue;
      }
      if(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle == DSS_ERROR)
      {
        UIM_MSG_ERR_1("Invalid socket handle for connect, on demand link: 0x%x",
                      estk_curr_inst_ptr->bip_channel_table[index].on_demand_link);
        continue;
      }

      estk_curr_inst_ptr->bip_channel_table[index].iface_id =
        dss_get_iface_id(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle);
      if(estk_curr_inst_ptr->bip_channel_table[index].iface_id == DSS_IFACE_INVALID_ID)
      {
        UIM_MSG_ERR_0("dss_get_iface_id_by_policy failed");
        return ESTK_ERROR;
      }

      if(estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN)
      {
        UIM_MSG_HIGH_1("Register for WLAN IP Address change, index: 0x%x",index);

        estk_bip_reg_ds_iface_event(DSS_IFACE_IOCTL_ADDR_CHANGED_EV,
                                    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                                    TRUE,
                                    index);
      }
      /* QOS is not supported for WLAN */
      else
      {
        /*Get updated QOS parameters from DS for PDN%*/
        if(dss_iface_ioctl(estk_curr_inst_ptr->bip_channel_table[index].iface_id,
                           DSS_IFACE_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC,
                           &ds_pdn_qos,
                           &error_num))
        {
          UIM_MSG_HIGH_1("IOCTL to get QOS failed with error %d", error_num);
        }
        else
        {
          estk_bip_copy_pdn_qos_parameters(&ds_pdn_qos, index);
        }
      }
      if (p_dest_addr_v6->ps_sin6_family == DSS_AF_INET6)
      {
        p_dest_addr_v6->ps_sin6_scope_id = dss_get_scope_id_by_iface_id(
                                             estk_curr_inst_ptr->bip_channel_table[index].iface_id,
                                             &error_num);
        if (p_dest_addr_v6->ps_sin6_scope_id == DSS_INVALID_SCOPE_ID)
        {
          UIM_MSG_ERR_0("Failed to get scope id!");
          return ESTK_ERROR;
        }
      }

      UIM_MSG_HIGH_3("net_handle: 0x%x, iface id: 0x%x, scope id: 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                     estk_curr_inst_ptr->bip_channel_table[index].iface_id,
                     p_dest_addr_v6->ps_sin6_scope_id);

      if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP)
      {
        sock_fd = dss_socket(
                    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                    uint16touint8(p_dest_addr_v6->ps_sin6_family),
                    DSS_SOCK_STREAM,
                    (byte)PS_IPPROTO_TCP,
                    &error_num);
      }
      else
      {
        sock_fd = dss_socket(
                    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle,
                    uint16touint8(p_dest_addr_v6->ps_sin6_family),
                    DSS_SOCK_DGRAM,
                    (byte)PS_IPPROTO_UDP,
                    &error_num);
      }

      /* If socket cannot be opened, send error TR to card */
      if(sock_fd == DSS_ERROR)
      {
        UIM_MSG_ERR_0("dss_socket, Invalid sock_fd");

        addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        additional_info.additional_info_ptr = &addi_res;
        additional_info.length = 1;
        tr_extra_param.present = TRUE;
        tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
          FALSE;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_status_additional_info =
          GSTK_CH_STATUS_LINK_DROPPED;
        (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
               sizeof(gstk_bearer_description_type),
               &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
               sizeof(gstk_bearer_description_type));

        tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
          estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
        cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;

        if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
            GSTK_OPEN_CH_REQ)
        {
          (void)estk_bip_clean_up_ch_table(index);

          return estk_bip_send_terminal_response(
                   cmd_ref_id,
                   GSTK_OPEN_CH_CNF,
                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                   &additional_info,
                   &tr_extra_param,
                   index);
        }
        else if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
                 GSTK_SEND_DATA_REQ)
        {
          if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
          {
            UIM_MSG_HIGH_0("Saving SEND DATA TR");
            estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
            estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
            estk_cache_additional_info(&additional_info);
            estk_status = ESTK_SUCCESS;
          }
          else
          {
            estk_status = estk_bip_send_terminal_response(
                            cmd_ref_id,
                            GSTK_SEND_DATA_CNF,
                            GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                            &additional_info,
                            NULL,
                            index);
           }
          estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;

          (void)estk_bip_send_ch_status_event(
                  estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                  GSTK_CH_STATUS_LINK_DROPPED);

          return estk_status;
        }
      }

      UIM_MSG_HIGH_1("dss_socket, sock_fd = %d", sock_fd);
      estk_curr_inst_ptr->bip_channel_table[index].sock_fd = sock_fd;

      if(estk_bip_setup_data_filter(index) == ESTK_ERROR)
      {
        UIM_MSG_ERR_0("estk_bip_setup_data_filter failed");
      }

      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_OPEN_WAIT;
    }
    if(estk_curr_inst_ptr->bip_channel_table[index].status ==
       ESTK_CH_OPEN_WAIT)
    {
      UIM_MSG_HIGH_1("Found channel %d in ESTK_CH_OPEN_WAIT", index);

      if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
         GSTK_PKT_DATA_TCP)
      {
        if (p_dest_addr_v4 == NULL)
        {
          UIM_MSG_ERR_0("invalid dest addr!");
          return ESTK_ERROR;
        }

        p_dest_addr_v4->ps_sin_port =
          dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
        addr_size =
          (p_dest_addr_v4->ps_sin_family == DSS_AF_INET) ?
            sizeof(struct ps_sockaddr_in) : sizeof(struct ps_sockaddr_in6);

        ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);
        dss_status = dss_connect(
                       estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                       (struct ps_sockaddr *)estk_curr_inst_ptr->bip_channel_table[index].dest_addr,
                       addr_size,
                       &error_num);

        UIM_MSG_HIGH_2("dss_connect returns dss_status %d with error_num %d",
                       dss_status, error_num);

        if(dss_status == DSS_ERROR && error_num == DS_EISCONN)
        {
          UIM_MSG_HIGH_0("dss_connect succeeded for TCP");
          dss_status = DSS_SUCCESS;
        }
      }
      else
      {
        UIM_MSG_HIGH_0("UDP transport layer, omit dss_connect" );
        dss_status = DSS_SUCCESS;
      }

      switch(dss_status)
      {
      case DSS_ERROR:
        if((error_num == DS_EWOULDBLOCK) || (error_num == DS_EINPROGRESS))
        {
          ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);
          dss_status = dss_async_select(
                         estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                         DS_WRITE_EVENT | DS_CLOSE_EVENT,
                         &error_num);
          if(dss_status != DSS_ERROR)
          {
            return ESTK_SUCCESS;
          }
          else
          {
            UIM_MSG_ERR_1("dss_async_select call failed - error_num %d",
                          error_num);
            return ESTK_ERROR;
          }
        }
        else
        {
          UIM_MSG_ERR_0("dss_connect failed");

          if(estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN)
          {
            /* Since dss_connect failed, remote server most likely not
               reachable.Its possible that this happens due to the WLAN
               bearer being used. Hence, do not use WLAN for BIP until
               there is a change in WLAN status */
            UIM_MSG_HIGH_0("Marking WLAN as unusable: dss_connect failed");
            estk_shared_info.wlan_unusable = TRUE;
          }

          /* dss_connect failed. Check if bearer fallback needs to be attempted
             (WLAN to Cellular or vice-versa) and start fallback if needed */
          estk_status = estk_bip_check_and_start_fallback(index);

          if(estk_status == ESTK_UNSUPPORTED)
          {
             UIM_MSG_HIGH_1("Fallback Could not be started for index: 0x%x",
                            index);

            /* Clean up allocated resources */
            (void)estk_bip_clean_up_ch_table(index);
          }
          return ESTK_SUCCESS;
        }
      case DSS_SUCCESS:
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_ACTIVE;
        ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);
        dss_status = dss_async_select(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                                      DS_READ_EVENT | DS_WRITE_EVENT | DS_CLOSE_EVENT,
                                      &error_num);

        if(dss_status == DSS_ERROR)
        {
          UIM_MSG_HIGH_0("dss_async_select failed");
          return ESTK_ERROR;
        }

        if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
           GSTK_OPEN_CH_REQ)
        {
          /*Send success terminal response*/
          gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
          if(estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified)
          {
            gen_result = GSTK_COMMAND_PERFORMED_WITH_MODIFICATION;
          }

          tr_extra_param.present = TRUE;
          tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
            estk_curr_inst_ptr->bip_channel_table[index].channel_id;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
            TRUE;
          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
          (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                 sizeof(gstk_bearer_description_type),
                 &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
                 sizeof(gstk_bearer_description_type));

          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;

          return estk_bip_send_terminal_response(
                   estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                   GSTK_OPEN_CH_CNF,
                   gen_result,
                   NULL,
                   &tr_extra_param,
                   index);

        }
        else if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
                GSTK_SEND_DATA_REQ)
        {
          UIM_MSG_HIGH_0("Returning success because in Send Data command");
          return ESTK_SUCCESS;
        }
        break;
      default:
        UIM_MSG_ERR_1("Unhandled return value for dss_connect %d", dss_status);
        break;
      } /* end of switch*/
    }
  }
  UIM_MSG_HIGH_0("estk_perform_net_connect as no channels in OPEN_WAIT");
  return ESTK_ERROR;
} /* estk_perform_net_connect */

/*===========================================================================

FUNCTION: estk_get_profile_auth_param

DESCRIPTION :
  This function gets auth param in a matching profile with same APN as open
  channel command.

PARAMETERS:
  ch_idx: [Input] Index of open channel in table
  tech_type : [Input] Tech type 3GPP
  auth_num: [Output] Authentication parameter to set in 3gpp profile created

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_get_profile_auth_param(
  int                            ch_idx,
  ds_profile_tech_etype          tech_type,
  ds_profile_3gpp_auth_pref_type *auth_type
)
{
  ds_profile_status_etype             api_ret_status;
  ds_profile_list_type                profile_list;
  ds_profile_itr_type                 profile_itr;
  ds_profile_list_info_type           profile_list_info;
  ds_profile_info_type                profile_list_name;
  ds_profile_hndl_type                profile_hndl;
  ds_profile_info_type                profile_info;
  int32                               param_len = -1;
  char                                profile_apn[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1];
  char                                profile_name[DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN];
  ds_profile_3gpp_auth_pref_type      profile_auth_type = DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED;
  int                                 i = 0;
  char                                profile_user_id[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];
  char                                profile_user_pwd[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];

  UIM_MSG_HIGH_3("estk_get_profile_auth_param: ch_idx=0x%x, tech_type=0x%x, "
                 "auth_type=0x%x", ch_idx, tech_type, auth_type);

  if(tech_type != DS_PROFILE_TECH_3GPP)
  {
    UIM_MSG_ERR_1("Unsupported API for the access tech 0x%x", tech_type);
    return ESTK_ERROR;
  }

  if(ch_idx < 0 || ch_idx >= ESTK_MAX_BIP_CH || auth_type == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  memset((void *)&profile_list, 0, sizeof(ds_profile_list_type));
  memset((void *)&profile_itr, 0, sizeof(ds_profile_itr_type));
  memset((void *)&profile_list_info, 0, sizeof(ds_profile_list_info_type));
  memset((void *)&profile_info, 0, sizeof(ds_profile_info_type));
  memset((void *)profile_user_id, 0, sizeof(profile_user_id));
  memset((void *)profile_user_pwd, 0, sizeof(profile_user_pwd));

  profile_list.dfn = DS_PROFILE_LIST_ALL_PROFILES;
  profile_list_info.name = &(profile_list_name);
  profile_list_info.name->len = DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN;
  profile_list_info.name->buf = profile_name;

  /* Get an iterator first */
  api_ret_status = ds_profile_get_list_itr(tech_type,
                                           &profile_list,
                                           &profile_itr);

  if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_get_list_itr failed %d", api_ret_status);
    return ESTK_ERROR;
  }

  do
  {
    /* Reset profile name buffer length between iteration */
    profile_list_info.name->len = DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN;

    /* Get the profile number for current profile using iterator */
    api_ret_status = ds_profile_get_info_by_itr(profile_itr,
                                                &profile_list_info);

    ESTK_BIP_DESTROY_ITR_IF_FAIL(api_ret_status, profile_itr);

    UIM_MSG_HIGH_1("Checking profile number %d", profile_list_info.num);

    /* Get handle on the profile */
    api_ret_status = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                                  tech_type,
                                                  profile_list_info.num,
                                                  &profile_hndl);

    ESTK_BIP_DESTROY_ITR_IF_FAIL(api_ret_status, profile_itr);

    /* Get the profile APN parameter to check if profile can be resused */
    profile_info.buf = profile_apn;
    profile_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1;

    ESTK_BIP_GET_PROFILE_PARAM(api_ret_status, profile_hndl, profile_itr, profile_info,
       (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN);

    UIM_MSG_HIGH_2("Profile Info APN Length = %d , Open Channel APN length = %d",
                   profile_info.len,
                   estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len);

    for(i = 0; i < profile_info.len; i++)
    {
      UIM_MSG_HIGH_3("Profile %d APN[%d]=0x%x",
                     profile_list_info.num, i, profile_apn[i]);
    }

    /* Check if APN is terminated by NULL */
    param_len = estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len;
    if((param_len > 0) &&
       (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn[param_len - 1] == 0))
    {
      UIM_MSG_HIGH_0("Open Channel APN terminated by NULL");
      param_len = param_len - 1;
    }

    /*Check if the APN matches*/
    if((profile_info.len == param_len) &&
       (strncasecmp(profile_info.buf,
                    (char *)estk_curr_inst_ptr->bip_channel_table[ch_idx].apn,
                    profile_info.len) == 0))
    {
       UIM_MSG_HIGH_1("Found matching profile number for apn%d",
                      profile_list_info.num);
    }
    else  /* Iterate on the iterator */
    {
      ESTK_BIP_ITERATOR_ITERATE(api_ret_status, profile_itr, profile_hndl);
      continue;
    }

    profile_info.buf = &profile_auth_type;
    profile_info.len = sizeof(profile_auth_type);

    ESTK_BIP_GET_PROFILE_PARAM(api_ret_status, profile_hndl, profile_itr, profile_info,
       (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE);

    *auth_type = profile_auth_type;

    UIM_MSG_HIGH_1("Profile auth type is 0x%x", *auth_type);

    if(estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len == 0)
    {
      profile_info.buf = profile_user_id;
      profile_info.len = sizeof(profile_user_id);

      ESTK_BIP_GET_PROFILE_PARAM(api_ret_status, profile_hndl, profile_itr, profile_info,
        (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME);

      UIM_MSG_HIGH_3("Profile Info User ID Length = %d , Open Channel User ID length = %d, profile_num = %d",
                     profile_info.len,
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len,
                     profile_list_info.num);

      estk_curr_inst_ptr->bip_channel_table[ch_idx].username = (char *)GSTK_CALLOC(profile_info.len);

      if(estk_curr_inst_ptr->bip_channel_table[ch_idx].username != NULL)
      {
        estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len = profile_info.len;
        (void)gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[ch_idx].username,
                       profile_info.buf,
                       profile_info.len,
                       profile_info.len,
                       profile_info.len);
        for(i=0; i < profile_info.len; i++)
        {
          UIM_MSG_HIGH_3("Profile User ID[%d]=0x%x BIP User ID 0x%x",
                         i,
                         profile_user_id[i],
                         estk_curr_inst_ptr->bip_channel_table[ch_idx].username[i]);
        }
      }
    }

    if(estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len == 0)
    {
      profile_info.buf = profile_user_pwd;
      profile_info.len = sizeof(profile_user_pwd);

      ESTK_BIP_GET_PROFILE_PARAM(api_ret_status, profile_hndl, profile_itr, profile_info,
        (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD);

      UIM_MSG_HIGH_3("Profile Info Pwd Length = %d , Open Channel Pwd length = %d, profile_num = %d",
                     profile_info.len,
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len,
                     profile_list_info.num);

      estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd =
        (char *)GSTK_CALLOC(profile_info.len);

      if(estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd != NULL)
      {
        estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len = profile_info.len;
        (void)gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd,
                       profile_info.buf,
                       profile_info.len,
                       profile_info.len,
                       profile_info.len);
        for(i=0; i < profile_info.len; i++)
        {
          UIM_MSG_HIGH_3("Profile User ID[%d]=0x%x BIP User ID 0x%x",
                         i,
                         profile_user_pwd[i],
                         estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd[i]);
        }
      }
    }

    /* Release handle */
    if (ds_profile_end_transaction(
      profile_hndl,
      DS_PROFILE_ACTION_CANCEL) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to commit profile!");
      (void)ds_profile_itr_destroy(profile_itr);
      return ESTK_ERROR;
    }
    (void)ds_profile_itr_destroy(profile_itr);
    return ESTK_SUCCESS;
  } while(api_ret_status != DS_PROFILE_REG_RESULT_LIST_END);

  UIM_MSG_HIGH_0("Did not find auth param matching APN");
  (void)ds_profile_itr_destroy(profile_itr);
  return ESTK_ERROR;
} /* estk_get_profile_auth_param */

/*===========================================================================

FUNCTION: estk_get_3gpp2_profile_to_use

DESCRIPTION :
  This function iterates throught the list of profiles and tries to reuse a
  profile number if possible for 3gpp2

PARAMETERS:
  tech_type : [Input] Tech type 3GPP2
  profile_num: [Output] the profile num to be reused

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_get_3gpp2_profile_to_use(
  int                          ch_idx,
  ds_profile_tech_etype        tech_type,
  ds_profile_num_type          *profile_num
)
{
  ds_profile_status_etype             api_ret_status;
  ds_profile_list_type                profile_list;
  ds_profile_itr_type                 profile_itr;
  ds_profile_list_info_type           profile_list_info;
  ds_profile_info_type                profile_list_name;
  ds_profile_hndl_type                profile_hndl;
  ds_profile_info_type                profile_info;
  char                                profile_apn[DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1];
  char                                profile_user_id[DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN + 1];
  char                                profile_user_pwd[DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN + 1];
  int32                               param_len = -1;
  int                                 i = 0;

  UIM_MSG_HIGH_3("estk_get_3gpp2_profile_to_use: ch_idx=0x%x, tech_type=0x%x, "
                 "profile_num=0x%x", ch_idx, tech_type, profile_num);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if((ch_idx < 0) || (ch_idx >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", ch_idx);
    return ESTK_ERROR;
  }

  if((profile_num == NULL)||(tech_type != DS_PROFILE_TECH_3GPP2))
  {
    UIM_MSG_ERR_0("Wrong parameter!");
    return ESTK_ERROR;
  }

  memset((void *)&profile_list, 0, sizeof(ds_profile_list_type));
  memset((void *)&profile_itr, 0, sizeof(ds_profile_itr_type));
  memset((void *)&profile_list_info, 0, sizeof(ds_profile_list_info_type));
  memset((void *)&profile_info, 0, sizeof(ds_profile_info_type));
  memset((void *)profile_apn, 0, sizeof(profile_apn));
  memset((void *)profile_user_id, 0, sizeof(profile_user_id));
  memset((void *)profile_user_pwd, 0, sizeof(profile_user_pwd));

  profile_list.dfn = DS_PROFILE_LIST_ALL_PROFILES;
  profile_list_info.name = &(profile_list_name);
  profile_list_info.name->len = DS_PROFILE_3GPP2_MAX_PROFILE_NAME_LEN;
  profile_list_info.name->buf = NULL;

  /* Get an iterator first */
  api_ret_status = ds_profile_get_list_itr(tech_type,
                                           &profile_list,
                                           &profile_itr);

  if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_get_list_itr failed %d", api_ret_status);
    return ESTK_ERROR;
  }

  do
  {
    /* Get the profile number for current profile using iterator */
    api_ret_status = ds_profile_get_info_by_itr(profile_itr,
                                                &profile_list_info);

    ESTK_BIP_DESTROY_ITR_IF_FAIL(api_ret_status, profile_itr);

    /* Get handle on the profile */
    api_ret_status = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                                  tech_type,
                                                  profile_list_info.num,
                                                  &profile_hndl);

    ESTK_BIP_DESTROY_ITR_IF_FAIL(api_ret_status, profile_itr);

    /* Get the profile APN parameter to check if profile can be resused */
    profile_info.buf = profile_apn;
    profile_info.len = DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1;

    ESTK_BIP_GET_PROFILE_PARAM(api_ret_status, profile_hndl, profile_itr, profile_info,
       (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING);

    UIM_MSG_HIGH_3("profile number %d, Profile Info APN Length = %d , Open Channel APN length = %d",
                    profile_list_info.num,
                    profile_info.len,
                    estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len);

    for(i=0; i < profile_info.len; i++)
    {
      UIM_MSG_HIGH_3("Profile %d APN[%d]=0x%x",
                     profile_list_info.num, i, profile_apn[i]);
    }

    /* Check if APN is terminated by NULL */
    param_len = estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len;
    if((param_len > 0) &&
       (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn[param_len - 1] == 0))
    {
      UIM_MSG_HIGH_0("Open Channel APN terminated by NULL");
      param_len = param_len - 1;
    }

    /*Check if the APN matches*/
    if((profile_info.len == param_len) &&
       (profile_info.len == 0 ||
       strncasecmp(profile_info.buf,
                   (char *)estk_curr_inst_ptr->bip_channel_table[ch_idx].apn,
                   profile_info.len) == 0))
    {
       UIM_MSG_HIGH_1("Found matching profile number for apn%d",
                      profile_list_info.num);
       *profile_num = profile_list_info.num;
    }
    else  /* Iterate on the iterator */
    {
      ESTK_BIP_ITERATOR_ITERATE(api_ret_status, profile_itr, profile_hndl);
      continue;
    }

    /* Check if User ID is terminated by NULL */
    param_len = estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len;
    if((param_len > 0) &&
       (estk_curr_inst_ptr->bip_channel_table[ch_idx].username[param_len - 1] == 0))
    {
      UIM_MSG_HIGH_0("Open Channel User ID terminated by NULL");
      param_len = param_len - 1;
    }
    if(param_len > 0)
    {
      /* Get the profile User ID parameter to check if profile can be resused */
      profile_info.buf = profile_user_id;
      profile_info.len = DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN + 1;

      ESTK_BIP_GET_PROFILE_PARAM(api_ret_status, profile_hndl, profile_itr, profile_info,
         (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_USER_ID);

      UIM_MSG_HIGH_2("Profile Info User ID Length = %d , Open Channel User ID length = %d",
                     profile_info.len,
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len);

      for(i=0; i < profile_info.len; i++)
      {
        UIM_MSG_HIGH_3("Profile %d User ID[%d]=0x%x",
                       profile_list_info.num, i, profile_user_id[i]);
      }

      /*Check if the User ID matches*/
      if((profile_info.len == param_len) &&
        (memcmp(profile_info.buf, estk_curr_inst_ptr->bip_channel_table[ch_idx].username, profile_info.len) == 0))
      {
        UIM_MSG_HIGH_1("Found matching profile number for user ID %d",
                       profile_list_info.num);
        *profile_num = profile_list_info.num;
      }
      else  /* Iterate on the iterator */
      {
        ESTK_BIP_ITERATOR_ITERATE(api_ret_status, profile_itr, profile_hndl);
        continue;
      }
    }

    /* Check if Password is terminated by NULL */
    param_len = estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len;
    if((param_len > 0) &&
       (estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd[param_len - 1] == 0))
    {
      UIM_MSG_HIGH_0("Open Channel Password terminated by NULL");
      param_len = param_len - 1;
    }
    if(param_len > 0)
    {
      /* Get the profile User Password parameter to check if profile can be resused */
      profile_info.buf = profile_user_pwd;
      profile_info.len = DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN + 1;

      ESTK_BIP_GET_PROFILE_PARAM(api_ret_status, profile_hndl, profile_itr, profile_info,
         (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD);

      UIM_MSG_HIGH_2("Profile Info Pwd Length = %d , Open Channel Pwd length = %d",
                profile_info.len,
                estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len);

      for(i=0; i < profile_info.len; i++)
      {
        UIM_MSG_HIGH_3("Profile %d Pwd[%d]=0x%x",
                       profile_list_info.num, i, profile_user_pwd[i]);
      }

      /*Check if the Password matches*/
      if((profile_info.len == param_len) &&
         (memcmp(profile_info.buf,
                 estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd,
                 profile_info.len) == 0))
      {
        UIM_MSG_HIGH_1("Found matching profile number for Passwd %d",
                       profile_list_info.num);
        *profile_num = profile_list_info.num;
      }
      else  /* Iterate on the iterator */
      {
        ESTK_BIP_ITERATOR_ITERATE(api_ret_status, profile_itr, profile_hndl);
        continue;
      }
    }

    /* Release handle */
    if (ds_profile_end_transaction(
      profile_hndl,
      DS_PROFILE_ACTION_CANCEL) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to commit profile!");
      (void)ds_profile_itr_destroy(profile_itr);
      return ESTK_ERROR;
    }
    (void)ds_profile_itr_destroy(profile_itr);
    return ESTK_SUCCESS;
  } while(api_ret_status != DS_PROFILE_REG_RESULT_LIST_END);

  (void)ds_profile_itr_destroy(profile_itr);
  UIM_MSG_HIGH_0("Did not find a re-usable profile");
  return ESTK_ERROR;
} /* estk_get_3gpp2_profile_to_use */

/*===========================================================================

FUNCTION: estk_set_net_policy_3gpp2_profile

DESCRIPTION :
  This function creates profiles with APN and 3GPP2 bearer parameters

PARAMETERS:
  ch_idx : [Input] Index of the active channel
  net_policy: [Iutput] the net policy where the profile to be set
  profile_num: [Output] the profile num created

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_set_net_policy_3gpp2_profile(
  int                          ch_idx,
  ds_profile_num_type         *profile_num
)
{
  ds_profile_hndl_type                profile_handler;
  ds_profile_info_type                profile_info;
  ds_profile_3gpp2_pdn_type_enum_type pdn_type        = DS_PROFILE_3GPP2_PDN_TYPE_V4;
  ds_profile_action_etype             commit_act      = DS_PROFILE_ACTION_COMMIT;
  ds_profile_3gpp2_rat_type_enum_type rat_type        = DS_PROFILE_3GPP2_RAT_TYPE_HRPD_EHRPD;
  struct ps_sockaddr_in               *p_dest_addr_v4 = NULL;
  ds_profile_config_type              profile_cfg     = {DS_PROFILE_CONFIG_MASK_PERSISTENCE, TRUE};
  gstk_open_ch_req_type              *open_ch_cmd     = NULL;
  int                                 i               = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  if(ch_idx < 0 ||
     ch_idx >= ESTK_MAX_BIP_CH ||
     profile_num == NULL)
  {
    UIM_MSG_ERR_2("invalid params: 0x%x 0x%x 0x%x", ch_idx, profile_num);
    return ESTK_BAD_INPUT_PARAM;
  }

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[ch_idx].dest_addr;
  if (open_ch_cmd != NULL && !open_ch_cmd->dns_requested && p_dest_addr_v4 == NULL)
  {
    UIM_MSG_ERR_0("invalid dest addr!");
    return ESTK_ERROR;
  }

  /* Check if profile_num can be reused */
  if(estk_get_3gpp2_profile_to_use(ch_idx, DS_PROFILE_TECH_3GPP2, profile_num) != ESTK_SUCCESS)
  {
    profile_cfg.config_mask = DS_PROFILE_CONFIG_MASK_PERSISTENCE;
    profile_cfg.is_persistent = FALSE;
    /* Get profile to use */
    if (ds_profile_create_ex(
          DS_PROFILE_TECH_3GPP2,
          &profile_cfg,
          profile_num) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to create profile!");
      return ESTK_ERROR;
    }
    else
    {
      /* Save profile number to delete on close channel */
      UIM_MSG_HIGH_1("ESTK_BIP: Profile number created %d", *profile_num);
      estk_curr_inst_ptr->bip_channel_table[ch_idx].profile_3gpp2 = uint16toint16(*profile_num);
    }
  }
  else
  {
    UIM_MSG_HIGH_0("ESTK_BIP: 3GPP2 Profile Reuse - no parameters to set");
    /*
      Check if we are re-using a profile created by ESTK. If not created
      by ESTK we do not cache the profile number to avoid deleting the profile
    */
    for (i = 0; i < ESTK_MAX_BIP_CH; i++)
    {
      if (estk_curr_inst_ptr->bip_channel_table[i].profile_3gpp2 == uint16toint16(*profile_num) &&
          estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_CLOSED)
      {
        estk_curr_inst_ptr->bip_channel_table[ch_idx].profile_3gpp2 = uint16toint16(*profile_num);
        UIM_MSG_HIGH_1("ESTK_BIP: Reusing profile %d created by ESTK",
                       estk_curr_inst_ptr->bip_channel_table[ch_idx].profile_3gpp2);
      }
    }
    return ESTK_SUCCESS;
  }

  if (ds_profile_begin_transaction(
        DS_PROFILE_TRN_RW,
        DS_PROFILE_TECH_3GPP2,
        *profile_num,
        &profile_handler) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to start modifying profile!");
    return ESTK_ERROR;
  }

  if (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len != 0)
  {
    profile_info.len = int32touint16(estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len);
    profile_info.buf = estk_curr_inst_ptr->bip_channel_table[ch_idx].apn;
    if (ds_profile_set_param(
          profile_handler,
          (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set APN!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP2_PROFILE_RETURN;
    }
  }

  /* set login name */
  if (estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len > 0)
  {
    profile_info.len =
      uint32touint16(estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len);
    profile_info.buf = estk_curr_inst_ptr->bip_channel_table[ch_idx].username;
    if (ds_profile_set_param(
          profile_handler,
          (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_USER_ID,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set USERNAME!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP2_PROFILE_RETURN;
    }
  }

  /* set password */
  if (estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len > 0)
  {
    profile_info.len =
      uint32touint16(estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len);
    profile_info.buf = estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd;
    if (ds_profile_set_param(
          profile_handler,
          (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set PASSWORD!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP2_PROFILE_RETURN;
    }
  }

  /* set profile to IPV4 or IPV6 accordingly */
  if (open_ch_cmd != NULL && open_ch_cmd->dns_requested)
  {
    pdn_type = DS_PROFILE_3GPP2_PDN_TYPE_V4_V6;
  }
  else
  {
    switch (p_dest_addr_v4->ps_sin_family)
    {
    case DSS_AF_INET:
      pdn_type = DS_PROFILE_3GPP2_PDN_TYPE_V4_V6;
      if(gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
           estk_curr_inst_ptr->slot_id) == TRUE)
      {
        UIM_MSG_HIGH_0("GSTK_CFG_FEATURE_ESTK_BIP_IP_DEFAULT NV set - set PDN type v4");
        pdn_type = DS_PROFILE_3GPP2_PDN_TYPE_V4;
      }
      break;
    case DSS_AF_INET6:
      pdn_type = DS_PROFILE_3GPP2_PDN_TYPE_V4_V6;
      break;
    default:
      UIM_MSG_ERR_1("invalid ps_sin_family 0x%x!",
                    p_dest_addr_v4->ps_sin_family);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP2_PROFILE_RETURN;
    }
  }
  profile_info.len = sizeof(pdn_type);
  profile_info.buf = &pdn_type;
  if (ds_profile_set_param(
          profile_handler,
          DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to set PDP type!");
    commit_act = DS_PROFILE_ACTION_CANCEL;
    goto ESTK_SET_NET_POLICY_3GPP2_PROFILE_RETURN;
  }

  profile_info.len = sizeof(rat_type);
  profile_info.buf = &rat_type;
  if (ds_profile_set_param(
          profile_handler,
          DS_PROFILE_3GPP2_PROFILE_PARAM_RAT_TYPE,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to set RAT type!");
    commit_act = DS_PROFILE_ACTION_CANCEL;
    goto ESTK_SET_NET_POLICY_3GPP2_PROFILE_RETURN;
  }

ESTK_SET_NET_POLICY_3GPP2_PROFILE_RETURN:
  if (ds_profile_end_transaction(
        profile_handler,
        commit_act) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to commit profile!");
    return ESTK_ERROR;
  }

  if (commit_act == DS_PROFILE_ACTION_COMMIT)
  {
    return ESTK_SUCCESS;
  }
  else
  {
    return ESTK_ERROR;
  }
}

/*===========================================================================

FUNCTION: estk_get_profile_to_use

DESCRIPTION :
  This function iterates throught the list of profiles and tries to reuse a
  profile number if possible.

PARAMETERS:
  tech_type : [Input] Tech type 3GPP or 3GPP2
  profile_num: [Output] the profile num to be reused

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_get_profile_to_use(
        int                          ch_idx,
        ds_profile_tech_etype        tech_type,
        ds_profile_num_type          *profile_num
)
{
  ds_profile_list_type            profile_list;
  ds_profile_itr_type             profile_itr;
  ds_profile_status_etype         api_ret_status;
  ds_profile_list_info_type       profile_list_info;
  ds_profile_hndl_type            profile_hndl;
  ds_profile_info_type            profile_info;
  ds_profile_info_type            profile_list_name;
  char                            profile_name[DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN];
  char                            profile_apn[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1];
  int32                           open_ch_apn_len = -1;

  UIM_MSG_HIGH_3("estk_get_profile_to_use: ch_idx=0x%x, tech_type=0x%x, "
                 "profile_num=0x%x", ch_idx, tech_type, profile_num);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if((ch_idx < 0) || (ch_idx >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", ch_idx);
    return ESTK_ERROR;
  }

  if((profile_num == NULL)||(tech_type != DS_PROFILE_TECH_3GPP))
  {
    UIM_MSG_ERR_0("Wrong parameter");
    return ESTK_ERROR;
  }

  memset((void *)&profile_list, 0, sizeof(ds_profile_list_type));
  memset((void *)&profile_itr, 0, sizeof(ds_profile_itr_type));
  memset((void *)&profile_list_info, 0, sizeof(ds_profile_list_info_type));
  memset((void *)&profile_info, 0, sizeof(ds_profile_info_type));

  profile_list.dfn = DS_PROFILE_LIST_ALL_PROFILES;
  profile_list_info.name = &(profile_list_name);
  profile_list_info.name->len = DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN;
  profile_list_info.name->buf = profile_name;
  profile_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1;
  profile_info.buf = profile_apn;

  /* Get an iterator first */
  api_ret_status = ds_profile_get_list_itr(tech_type,
                                           &profile_list,
                                           &profile_itr);

  if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_get_list_itr failed %d", api_ret_status);
    return ESTK_ERROR;
  }

  do
  {
    memset((void *)profile_apn, 0x00, sizeof(char)*DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1);
    memset((void *)profile_name, 0x00, sizeof(char)*DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN);
    ESTK_RETURN_ERROR_IF_NULL_PTR(profile_list_info.name, ESTK_ERROR);
    profile_list_info.name->len = DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN;
    profile_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1;
    /* Get the value and iterate */
    api_ret_status = ds_profile_get_info_by_itr(profile_itr,
                                                &profile_list_info);
    if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_1("ds_profile_get_info_by_itr failed %d", api_ret_status);
      (void)ds_profile_itr_destroy(profile_itr);
      return ESTK_ERROR;
    }

    if(profile_list_info.name != NULL)
    {
      if((profile_list_info.name->len == (sizeof(estk_bip_profile_name) - 1)) &&
          memcmp(profile_list_info.name->buf, estk_bip_profile_name,
                 profile_list_info.name->len) == 0)
      {
        UIM_MSG_HIGH_1("Found BIP profile %d", profile_list_info.num);

        /* Get handle on the profile */
        api_ret_status = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                                      tech_type,
                                                      profile_list_info.num,
                                                      &profile_hndl);
        if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          UIM_MSG_ERR_1("ds_profile_begin_transaction failed %d",
                        api_ret_status);
          /* Release Iterator */
          (void)ds_profile_itr_destroy(profile_itr);
          return ESTK_ERROR;
        }

        /* Get the profile APN parameter to check if profile can be resused */
        api_ret_status = ds_profile_get_param(profile_hndl,
                           (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                           &profile_info);
        if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          UIM_MSG_ERR_1("ds_profile_get_param failed for APN %d",
                        api_ret_status);
          /* Release Iterator */
          (void)ds_profile_itr_destroy(profile_itr);
          /* Release Handle */
          (void)ds_profile_end_transaction(profile_hndl,
                                           DS_PROFILE_ACTION_CANCEL);
          return ESTK_ERROR;
        }

        /* Check if APN is terminated by NULL */
        open_ch_apn_len = estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len;
        if((open_ch_apn_len > 0) &&
           (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn[open_ch_apn_len - 1] == 0))
        {
          UIM_MSG_HIGH_0("Open Channel terminated by NULL");
          open_ch_apn_len = open_ch_apn_len - 1;
        }

        /*Check if the APN matches*/
        if(profile_info.len == open_ch_apn_len)
        {
          if(profile_info.len == 0 ||
             strncasecmp(profile_info.buf,
                         (char *)estk_curr_inst_ptr->bip_channel_table[ch_idx].apn,
                         profile_info.len) == 0)
          {
            UIM_MSG_HIGH_1("Found matching profile number %d",
                           profile_list_info.num);
            *profile_num = profile_list_info.num;
          }
        }

        /* Release handle */
        if (ds_profile_end_transaction(
          profile_hndl,
          DS_PROFILE_ACTION_CANCEL) != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          UIM_MSG_ERR_0("failed to commit profile!");
          (void)ds_profile_itr_destroy(profile_itr);
          return ESTK_ERROR;
        }

        if(*profile_num == profile_list_info.num)
        {
          (void)ds_profile_itr_destroy(profile_itr);
          return ESTK_SUCCESS;
        }
      }
      else
      {
        UIM_MSG_HIGH_1("Found non UIM profile %d", profile_list_info.num);
      }
    }
    /* Iterate on the iterator */
    api_ret_status = ds_profile_itr_next(profile_itr);
    if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS &&
       api_ret_status != DS_PROFILE_REG_RESULT_LIST_END)
    {
      UIM_MSG_ERR_1("ds_profile_itr_next returns %d", api_ret_status);
      /* Release Iterator */
      (void)ds_profile_itr_destroy(profile_itr);
      return ESTK_ERROR;
    }
  }while(api_ret_status != DS_PROFILE_REG_RESULT_LIST_END);

  (void)ds_profile_itr_destroy(profile_itr);
  UIM_MSG_HIGH_0("Did not find a re-usable profile");
  return ESTK_ERROR;
} /* estk_get_profile_to_use */

/*===========================================================================

FUNCTION: estk_set_net_policy_3gpp_profile

DESCRIPTION :
  This function creates profiles with APN and 3GPP bearer parameters

PARAMETERS:
  ch_idx : [Input] Index of the active channel
  net_policy: [Iutput] the net policy where the profile to be set
  profile_num: [Output] the profile num created

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_set_net_policy_3gpp_profile(
  int                          ch_idx,
  ds_profile_num_type         *profile_num
)
{
  gstk_gprs_bearer_param_type            *gprs_bearer     = NULL;
  gstk_eutran_ext_param_type             *umts_bearer     = NULL;
  gstk_eutran_mapped_utran_ps_param_type *eps_bearer      = NULL;
  ds_profile_hndl_type                    profile_handler;
  ds_profile_info_type                    profile_info;
  ds_profile_3gpp_gprs_qos_params_type    gprs_qos;
  ds_profile_3gpp_3gpp_qos_params_type    umts_qos;
  ds_profile_3gpp_lte_qos_params_type     lte_qos;
  ds_profile_3gpp_pdp_type_enum_type      pdp_type;
  ds_profile_identifier_type              prof_item;
  ds_profile_action_etype                 commit_act        = DS_PROFILE_ACTION_COMMIT;
  struct ps_sockaddr_in                   *p_dest_addr_v4   = NULL;
  ds_profile_3gpp_auth_pref_type          profile_auth_type = DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED;
  ds_profile_config_type                  profile_cfg       = {DS_PROFILE_CONFIG_MASK_PERSISTENCE, TRUE};
  gstk_open_ch_req_type                   *open_ch_cmd      = NULL;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  UIM_MSG_HIGH_2("estk_set_net_policy_3gpp_profile: ch_idx=0x%x, profile_num=0x%x",
                 ch_idx, profile_num);

  if(ch_idx < 0 ||
     ch_idx >= ESTK_MAX_BIP_CH ||
     profile_num == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[ch_idx].dest_addr;
  if (open_ch_cmd != NULL && !open_ch_cmd->dns_requested && p_dest_addr_v4 == NULL)
  {
    UIM_MSG_ERR_0("invalid dest addr!");
    return ESTK_ERROR;
  }

  prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_INVALID;

  /* Check if profile_num can be reused */
  if(estk_get_profile_to_use(ch_idx, DS_PROFILE_TECH_3GPP, profile_num) != ESTK_SUCCESS)
  {
    profile_cfg.config_mask = DS_PROFILE_CONFIG_MASK_PERSISTENCE;
    profile_cfg.is_persistent = FALSE;
    /* Get profile to use */
    if (ds_profile_create_ex(
          DS_PROFILE_TECH_3GPP,
          &profile_cfg,
          profile_num) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to create profile!");
      return ESTK_ERROR;
    }
  }

  /* Save profile number to delete on close channel */
  UIM_MSG_HIGH_1("ESTK_BIP: Profile number created %d", *profile_num);

  estk_curr_inst_ptr->bip_channel_table[ch_idx].profile_3gpp =
    uint16toint16(*profile_num);

  if (ds_profile_begin_transaction(
        DS_PROFILE_TRN_RW,
        DS_PROFILE_TECH_3GPP,
        *profile_num,
        &profile_handler) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to start modifying profile!");
    (void)ds_profile_delete(DS_PROFILE_TECH_3GPP, *profile_num);
    return ESTK_ERROR;
  }

  /* reset profile to default */
  if (ds_profile_reset_profile_to_default(
        DS_PROFILE_TECH_3GPP,
        *profile_num) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to reset profile!");
  }

  /* set profile name */
  profile_info.len = sizeof(estk_bip_profile_name);
  profile_info.buf = estk_bip_profile_name;
  if (ds_profile_set_param(
        profile_handler,
        (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME,
        &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to set Profile Name!");
    commit_act = DS_PROFILE_ACTION_CANCEL;
    goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
  }

  /* set apn */
  if (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len > 0)
  {
    profile_info.len = int32touint16(estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len);
    profile_info.buf = estk_curr_inst_ptr->bip_channel_table[ch_idx].apn;
    if (ds_profile_set_param(
          profile_handler,
          (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set APN!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
  }

  /* set auth param */
  if(estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len > 0 &&
     estk_get_profile_auth_param(ch_idx, DS_PROFILE_TECH_3GPP, &profile_auth_type) == ESTK_SUCCESS)
  {
    profile_info.len = sizeof(profile_auth_type);
    profile_info.buf = &profile_auth_type;
    if (ds_profile_set_param(
          profile_handler,
          (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("Failed to set Auth Type!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
  }

  /* set login name */
  if (estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len > 0)
  {
    profile_info.len =
      uint32touint16(estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len);
    profile_info.buf = estk_curr_inst_ptr->bip_channel_table[ch_idx].username;
    if (ds_profile_set_param(
          profile_handler,
          (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set APN!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
  }

  /* set password */
  if (estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len > 0)
  {
    profile_info.len =
      uint32touint16(estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len);
    profile_info.buf = estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd;
    if (ds_profile_set_param(
          profile_handler,
          (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set APN!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
  }

  /* Reset auth params in OPEN CHANNEL table if card did not have auth params
     in the OPEN CHANNEL command. This is done because these same cache variables
     are used later for 3GPP2 lookup and profile creation. 3GPP2 auth params can
     only come from the card or existing 3GPP2 profile in DS */
  if (!estk_curr_inst_ptr->bip_channel_table[ch_idx].card_has_auth_params)
  {
    UIM_MSG_HIGH_1("ESTK_BIP: Reset auth params in OPEN CHANNEL table for channel %d", ch_idx);
    if (estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len > 0)
    {
      gstk_free(estk_curr_inst_ptr->bip_channel_table[ch_idx].username);
      estk_curr_inst_ptr->bip_channel_table[ch_idx].username = NULL;
      estk_curr_inst_ptr->bip_channel_table[ch_idx].username_len = 0;
    }

    if (estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len > 0)
    {
      gstk_free(estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd);
      estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd = NULL;
      estk_curr_inst_ptr->bip_channel_table[ch_idx].passwd_len = 0;
    }
  }

  /* set bearer params */
  prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_INVALID;
  switch (estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_type)
  {
  case GSTK_GPRS:
    gprs_bearer =
      &estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_params.gprs_bearer;
    gprs_qos.precedence = int32touint32(gprs_bearer->precedence_cls);
    gprs_qos.delay = int32touint32(gprs_bearer->delay_cls);
    gprs_qos.reliability = int32touint32(gprs_bearer->reliability_cls);
    gprs_qos.peak = int32touint32(gprs_bearer->peak_throughput);
    gprs_qos.mean = int32touint32(gprs_bearer->mean_throughput);
    profile_info.len = sizeof(gprs_qos);
    profile_info.buf = &gprs_qos;
    prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS;
    break;
  case GSTK_EUTRAN_EXT_PARAM:
    umts_bearer =
      &estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_params.eutran_ext_bearer;
    switch (umts_bearer->traffic_class)
    {
    case 0:
      umts_qos.traffic_class = DS_PROFILE_3GPP_TC_CONVERSATIONAL;
      break;
    case 1:
      umts_qos.traffic_class = DS_PROFILE_3GPP_TC_STREAMING;
      break;
    case 2:
      umts_qos.traffic_class = DS_PROFILE_3GPP_TC_INTERACTIVE;
      break;
    case 3:
      umts_qos.traffic_class = DS_PROFILE_3GPP_TC_BACKGROUND;
      break;
    case 4:
      umts_qos.traffic_class =DS_PROFILE_3GPP_TC_SUBSCRIBED;
      break;
    default:
      UIM_MSG_ERR_1("invalid traffic class 0x%x!",
                    umts_bearer->traffic_class);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    umts_qos.max_ul_bitrate = umts_bearer->max_bitrate_ul;
    umts_qos.max_dl_bitrate = umts_bearer->max_bitrate_dl;
    umts_qos.gtd_ul_bitrate = umts_bearer->guaranteed_bitrate_ul;
    umts_qos.gtd_dl_bitrate = umts_bearer->guaranteed_bitrate_dl;
    switch (umts_bearer->delivery_order)
    {
    case 0:
      umts_qos.dlvry_order = DS_PROFILE_3GPP_DO_OFF;
      break;
    case 1:
      umts_qos.dlvry_order = DS_PROFILE_3GPP_DO_ON;
      break;
    case 2:
      umts_qos.dlvry_order = DS_PROFILE_3GPP_DO_SUBSCRIBED;
      break;
    default:
      UIM_MSG_ERR_1("invalid delivery order 0x%x",
                    umts_bearer->delivery_order);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    umts_qos.max_sdu_size = umts_bearer->max_sdu_size;
    if (umts_bearer->sdu_error_ratio <= 7)
    {
      umts_qos.sdu_err =
        (ds_profile_3gpp_sdu_err_ratio_type)umts_bearer->sdu_error_ratio;
    }
    else
    {
      UIM_MSG_ERR_1("invalid sdu error ratio 0x%x!",
                    umts_bearer->sdu_error_ratio);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    if (umts_bearer->residual_bit_error_ratio <= 9)
    {
      umts_qos.res_biterr =
        (ds_profile_3gpp_residual_ber_ratio_type)umts_bearer->residual_bit_error_ratio;
    }
    else
    {
      UIM_MSG_ERR_1("invalid residual bit error ration 0x%x!",
                    umts_bearer->residual_bit_error_ratio);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    if (umts_bearer->delivery_of_err_sdu <= 3)
    {
      umts_qos.dlvr_err_sdu =
        (ds_profile_3gpp_deliver_err_sdu_type)umts_bearer->delivery_of_err_sdu;
    }
    else
    {
      UIM_MSG_ERR_1("invalid delivery of err sdu 0x%x!",
                    umts_bearer->delivery_of_err_sdu);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    umts_qos.trans_delay = umts_bearer->transfer_delay;
    umts_qos.thandle_prio = umts_bearer->traffic_handling_pri;
    umts_qos.sig_ind = FALSE;
    profile_info.len = sizeof(umts_qos);
    profile_info.buf = &umts_qos;
    prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS;
    break;
  case GSTK_EUTRAN_MAPPED_UTRAN_PS:
    eps_bearer =
      &estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer;
    memset(&lte_qos, 0 , sizeof(lte_qos));
    switch (eps_bearer->qci)
    {
    case 0:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_0;
      break;
    case 1:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_1;
      break;
    case 2:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_2;
      break;
    case 3:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_3;
      break;
    case 4:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_4;
      break;
    case 5:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_5;
      break;
    case 6:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_6;
      break;
    case 7:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_7;
      break;
    case 8:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_8;
      break;
    case 9:
      lte_qos.qci = DS_PROFILE_3GPP_LTE_QCI_9;
      break;
    default:
      UIM_MSG_ERR_1("invalid qci 0x%x!", eps_bearer->qci);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    lte_qos.g_dl_bit_rate = eps_bearer->guaranteed_bitrate_dl;
    lte_qos.max_dl_bit_rate = eps_bearer->guaranteed_bitrate_dl_ext;
    lte_qos.g_ul_bit_rate = eps_bearer->guaranteed_bitrate_ul;
    lte_qos.max_ul_bit_rate = eps_bearer->guaranteed_bitrate_ul_ext;
    profile_info.len = sizeof(lte_qos);
    profile_info.buf = &lte_qos;
    prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS;
    break;
  case GSTK_BEARER_DEFAULT:
    /* nothing to do */
    break;
  default:
    UIM_MSG_ERR_1("invalid bearer type 0x%x",
                  estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_type);
    commit_act = DS_PROFILE_ACTION_CANCEL;
    goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
  }

  if (prof_item != (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_INVALID)
  {
    if (ds_profile_set_param(
          profile_handler,
          prof_item,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set bearer params!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
  }

  /* set pdp type */
  prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_INVALID;
  switch (estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_type)
  {
  case GSTK_GPRS:
    gprs_bearer =
      &estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_params.gprs_bearer;
    if (gprs_bearer->pkt_data_protocol == GSTK_PKT_DATA_IP)
    {
      pdp_type = DS_PROFILE_3GPP_PDP_IP;
      profile_info.len = sizeof(pdp_type);
      profile_info.buf = &pdp_type;
      prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE;
    }
    else
    {
      UIM_MSG_ERR_1("invalid PDP type 0x%x",
                    gprs_bearer->pkt_data_protocol);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    break;
  case GSTK_EUTRAN_EXT_PARAM:
    umts_bearer =
      &estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_params.eutran_ext_bearer;
    if (umts_bearer->pdp_type == GSTK_PKT_DATA_IP)
    {
      pdp_type = DS_PROFILE_3GPP_PDP_IP;
      profile_info.len = sizeof(pdp_type);
      profile_info.buf = &pdp_type;
      prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE;
    }
    else
    {
      UIM_MSG_ERR_1("invalid PDP type 0x%x",
                    umts_bearer->pdp_type);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    break;
  case GSTK_EUTRAN_MAPPED_UTRAN_PS:
    eps_bearer =
      &estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer;
    if (eps_bearer->pdp_type == GSTK_PKT_DATA_IP)
    {
      pdp_type = DS_PROFILE_3GPP_PDP_IP;
      profile_info.len = sizeof(pdp_type);
      profile_info.buf = &pdp_type;
      prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE;
    }
    else
    {
      UIM_MSG_ERR_1("invalid PDP type 0x%x",
                    eps_bearer->pdp_type);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
    break;
  case GSTK_BEARER_DEFAULT:
    /* nothing to do */
    break;
  default:
    UIM_MSG_ERR_1("invalid bearer type 0x%x",
                  estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_type);
    commit_act = DS_PROFILE_ACTION_CANCEL;
    goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
  }

  if (prof_item != (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_INVALID)
  {
    if (ds_profile_set_param(
          profile_handler,
          prof_item,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      UIM_MSG_ERR_0("failed to set PDP type!");
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
  }
  if (open_ch_cmd != NULL && open_ch_cmd->dns_requested)
  {
    pdp_type = DS_PROFILE_3GPP_PDP_IPV4V6;
    if(gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
         estk_curr_inst_ptr->slot_id) == TRUE)
    {
      UIM_MSG_HIGH_0("GSTK_CFG_FEATURE_ESTK_BIP_IP_DEFAULT NV set - PDP type IP");
      pdp_type = DS_PROFILE_3GPP_PDP_IP;
    }
  }
  else
  {
    UIM_MSG_HIGH_1("ps_sin_family = %d", p_dest_addr_v4->ps_sin_family);
    /* set profile to IPV4 or IPV6 accordingly */
    switch (p_dest_addr_v4->ps_sin_family)
    {
    case DSS_AF_INET6:
      pdp_type = DS_PROFILE_3GPP_PDP_IPV4V6;
      break;
    case DSS_AF_INET:
      pdp_type = DS_PROFILE_3GPP_PDP_IPV4V6;
      if(gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
           estk_curr_inst_ptr->slot_id) == TRUE)
      {
        UIM_MSG_HIGH_0("GSTK_CFG_FEATURE_ESTK_BIP_IP_DEFAULT NV set - PDP type IP");
        pdp_type = DS_PROFILE_3GPP_PDP_IP;
      }
      break;
    default:
      UIM_MSG_ERR_1("invalid ps_sin_family 0x%x!",
                    p_dest_addr_v4->ps_sin_family);
      commit_act = DS_PROFILE_ACTION_CANCEL;
      goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
    }
  }
  profile_info.len = sizeof(pdp_type);
  profile_info.buf = &pdp_type;
  prof_item = (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE;
  if (ds_profile_set_param(
          profile_handler,
          prof_item,
          &profile_info) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to set PDP type!");
    commit_act = DS_PROFILE_ACTION_CANCEL;
    goto ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN;
  }

ESTK_SET_NET_POLICY_3GPP_PROFILE_RETURN:
  if (ds_profile_end_transaction(
        profile_handler,
        commit_act) != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to commit profile!");
    return ESTK_ERROR;
  }

  if (commit_act == DS_PROFILE_ACTION_COMMIT)
  {
    return ESTK_SUCCESS;
  }
  else
  {
    return ESTK_ERROR;
  }
} /* estk_set_net_policy_3gpp_profile */

/*=====================================================================================

FUNCTION: estk_check_3gpp_profile_usability

DESCRIPTION :
 This function checks for usability of profile_num by checking the compatibility between
 the profile's pdp_type with address_type

PARAMETERS:
  profile_num  : 3GPP DS Profile number
  address_type : Type of address

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS - profile_num is compatible with address_type
    ESTK_ERROR   - otherwise (profile_num can't be used)

COMMENTS:
  None

SIDE EFFECTS:
  None

=====================================================================================*/
static estk_result_enum_type estk_check_3gpp_profile_usability(
  ds_profile_num_type               profile_num,
  gstk_pkt_data_address_enum_type   address_type
)
{
  ds_profile_status_etype             api_ret_status   = DS_PROFILE_REG_RESULT_MAX;
  ds_profile_hndl_type                profile_hndl     = NULL;
  ds_profile_info_type                profile_info;
  ds_profile_3gpp_pdp_type_enum_type  profile_pdp_type = DS_PROFILE_3GPP_PDP_MAX;
  estk_result_enum_type               estk_status      = ESTK_ERROR;

  UIM_MSG_HIGH_2("In estk_check_3gpp_profile_usability profile_num: %d, address_type : %d",
                  profile_num, address_type);

  memset((void *)&profile_info, 0, sizeof(ds_profile_info_type));

  profile_info.len = 1;
  profile_info.buf = &profile_pdp_type;

  /* Get handle on the profile */
  api_ret_status = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                                DS_PROFILE_TECH_3GPP,
                                                profile_num,
                                                &profile_hndl);
  if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_begin_transaction failed %d", api_ret_status);
    return ESTK_ERROR;
  }

  /* Get the profile APN parameter to check if profile can be resused */
  api_ret_status = ds_profile_get_param(profile_hndl,
                    (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
                    &profile_info);
  if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_get_param failed for APN %d", api_ret_status);
    /* Release Handle */
    (void)ds_profile_end_transaction(profile_hndl,
                                    DS_PROFILE_ACTION_CANCEL);
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_1("Profile PDP type = %d", profile_pdp_type);

  /*Check if the PDP type matches*/
  switch(address_type)
  {
    case GSTK_PKT_DATA_IPV4_ADDRESS:
      if(profile_pdp_type == DS_PROFILE_3GPP_PDP_IP ||
         profile_pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6)
      {
        estk_status = ESTK_SUCCESS;
        /*
          If NW only accepts PDP type as IPV4 but default profile is IPV4V6,
          then we return failure in order to force ESTK to create a new profile
          with PDP type as IPV4
        */
        if (profile_pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6 &&
            (gstk_nv_get_feature_status(
              GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
              estk_curr_inst_ptr->slot_id) == TRUE))
        {
          estk_status = ESTK_ERROR;
        }
      }
      break;
    case GSTK_PKT_DATA_IPV6_ADDRESS:
      if(profile_pdp_type == DS_PROFILE_3GPP_PDP_IPV6 ||
         profile_pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6)
      {
        estk_status = ESTK_SUCCESS;
      }
      break;
    default:
      estk_status = ESTK_SUCCESS;
      break;
  }
   /* Release Handle */
  (void)ds_profile_end_transaction(profile_hndl, DS_PROFILE_ACTION_CANCEL);

  return estk_status;
} /* estk_check_3gpp_profile_usability */

/*===========================================================================

FUNCTION: estk_bip_set_3gpp_default_profile_apn

DESCRIPTION :
  This function sets the default profile APN

PARAMETERS:
  index        : the index in the BIP channel table
  profile_num  : the default profile num

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS - Success in case default profile APN copied successfully
    ESTK_ERROR - failed to copy default profile APN

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_set_3gpp_default_profile_apn(
  uint32              index,
  ds_profile_num_type profile_num
)
{
  ds_profile_status_etype             api_ret_status                                      = DS_PROFILE_REG_RESULT_MAX;
  ds_profile_hndl_type                profile_hndl                                        = NULL;
  ds_profile_info_type                profile_info;
  char                                profile_apn[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1] = {0};
  int                                 i                                                   = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  memset((void *)&profile_info, 0, sizeof(ds_profile_info_type));

  profile_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1;
  profile_info.buf = profile_apn;

  /* Get handle on the profile */
  api_ret_status = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                                DS_PROFILE_TECH_3GPP,
                                                profile_num,
                                                &profile_hndl);
  if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_begin_transaction failed %d", api_ret_status);
    return ESTK_ERROR;
  }

  /* Get the profile APN parameter to check if profile can be resused */
  api_ret_status = ds_profile_get_param(profile_hndl,
                    (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                    &profile_info);
  if(api_ret_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_get_param failed for APN %d", api_ret_status);
    /* Release Handle */
    (void)ds_profile_end_transaction(profile_hndl,
                                     DS_PROFILE_ACTION_CANCEL);
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
    estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
  }
  estk_curr_inst_ptr->bip_channel_table[index].apn = (uint8 *)GSTK_CALLOC(profile_info.len);
  if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
  {
    if(gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[index].apn,
                   profile_apn,
                   profile_info.len,
                   profile_info.len,
                   DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1) <
       profile_info.len)
    {
      /* Release Handle */
      (void)ds_profile_end_transaction(profile_hndl,
                                       DS_PROFILE_ACTION_CANCEL);
      gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
      estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
      return ESTK_ERROR;
    }
    estk_curr_inst_ptr->bip_channel_table[index].apn_len = profile_info.len;
    for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++i)
    {
      UIM_MSG_HIGH_2("DEFAULT profile APN[%d] = 0x%x",
                     i, estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
    }
    /* Release Handle */
    (void)ds_profile_end_transaction(profile_hndl,
                                    DS_PROFILE_ACTION_CANCEL);
    return ESTK_SUCCESS;
  }
  else
  {
    /* Release Handle */
    (void)ds_profile_end_transaction(profile_hndl,
                                    DS_PROFILE_ACTION_CANCEL);
    return ESTK_ERROR;
  }
} /* estk_bip_set_3gpp_default_profile_apn */

/*===========================================================================

FUNCTION: estk_pre_process_3gpp_profile

DESCRIPTION :
  This function reads APN from NV or call DS api to get default 3GPP profile number

PARAMETERS:
  ch_idx : [Input] Index of the active channel
  profile_num: [Output] The default profile returned by DS api
  has_apn: [Output] have apn from card/nv or not

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR
    ESTK_BAD_INPUT_PARAM

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type  estk_pre_process_3gpp_profile(
  int                         ch_idx,
  ds_profile_num_type         *profile_num_ptr,
  boolean                     *has_apn_ptr
)
{
  gstk_nv_items_data_type       nv_data  = {0};

  if((ch_idx < 0) || (ch_idx >= ESTK_MAX_BIP_CH) ||
     (profile_num_ptr == NULL) || (has_apn_ptr == NULL))
  {
    UIM_MSG_ERR_3("invalid params: ch_idx=0x%x profile_num_ptr=0x%x has_apn_ptr=0x%x",
                  ch_idx, profile_num_ptr, has_apn_ptr);
    return ESTK_BAD_INPUT_PARAM;
  }

  *has_apn_ptr = TRUE;
  
  (void)gstk_nv_access_read(GSTK_NV_EXT_ME_GCF_GSTK,
                            estk_curr_inst_ptr->slot_id,
                            &nv_data);
  /* Do not read the APN from NV if it's a GCF */
  if((nv_data.gcf_gstk) || (estk_bip_set_default_apn_from_efs(ch_idx) < 0))
  {
    *has_apn_ptr = FALSE;
    /* Either it's GCF or the NV is not configured. Read the DS default profile */
    if(ds_profile_get_default_profile_num(DS_PROFILE_TECH_3GPP,
                                          DS_PROFILE_3GPP_SOCKETS_PROFILE_FAMILY,
                                          profile_num_ptr) == DS_PROFILE_REG_RESULT_SUCCESS)
    {
      if(estk_check_3gpp_profile_usability(*profile_num_ptr,
                                           estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.data_dest_address.address_type)
                                           != ESTK_SUCCESS)
      {
        /* Copy APN from DS default profile in to bip_channel_table[].apn */
        if(estk_bip_set_3gpp_default_profile_apn(ch_idx, *profile_num_ptr) != ESTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Unable to read APN from default DS profile");
          return ESTK_ERROR;
        }
      }
    }
    else
    {
      UIM_MSG_ERR_0("Failed to get default profile!");
      return ESTK_ERROR;
    }
  }
  
  return ESTK_SUCCESS;
} /* estk_pre_process_3gpp_profile */

/*===========================================================================

FUNCTION: estk_set_net_policy_profile

DESCRIPTION :
  This function creates profiles with APN and bearer parameters and set the
  profile to net policy

PARAMETERS:
  ch_idx : [Input] Index of the active channel
  net_policy: [Output] the net policy where the profile to be set

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_set_net_policy_profile(
  gstk_access_technology_type       access_tech,
  int                               ch_idx,
  dss_net_policy_info_type         *net_policy
)
{
  ds_profile_num_type                  profile_num     = DS_PROFILE_UNSPECIFIED_NUM;
  const ds_sys_system_status_ex_type   *sys_status     = NULL;
  struct ps_sockaddr_in                *p_dest_addr_v4 = NULL;
  sint15                               ds_errno        = DS_EINVAL;
  boolean                              profile_3gpp    = TRUE;
  boolean                              profile_3gpp2   = TRUE;
  gstk_open_ch_req_type                *open_ch_cmd    = NULL;
  boolean                              has_apn         = TRUE; /* suppose we have an APN from NV or card */

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  UIM_MSG_HIGH_3("estk_set_net_policy_profile: access_tech=0x%x, "
                 "ch_idx=0x%x, "
                 "net_policy=0x%x", access_tech, ch_idx, net_policy);

  if(ch_idx < 0 || ch_idx >= ESTK_MAX_BIP_CH || net_policy == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[ch_idx].dest_addr;
  if (open_ch_cmd != NULL && !open_ch_cmd->dns_requested && p_dest_addr_v4 == NULL)
  {
    UIM_MSG_ERR_0("invalid dest addr!");
    return ESTK_ERROR;
  }

  (void)ds_profile_init_lib();

  if (open_ch_cmd != NULL && !open_ch_cmd->dns_requested)
  {
    switch (p_dest_addr_v4->ps_sin_family)
    {
      case DSS_AF_INET:
        net_policy->family = DSS_AF_INET;
        break;
      case DSS_AF_INET6:
        net_policy->family = DSS_AF_INET6;
        break;
      default:
        UIM_MSG_ERR_1("invalid ps_sin_family 0x%x!",
                      p_dest_addr_v4->ps_sin_family);
        return ESTK_ERROR;
    }
  }

  /* Profiles are not required to be set for WLAN */
  if(estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type == ESTK_NETWORK_WLAN)
  {
    return ESTK_SUCCESS;
  }

  if(gstk_io_ctrl(GSTK_IO_GET_DS_SYSTEM_RAT_INFO,
                  (void *)&estk_curr_inst_ptr->slot_id,
                  (void *)&sys_status)!= GSTK_SUCCESS)
  {
    sys_status = NULL;
    UIM_MSG_ERR_1("ds_sys_conf_get failed error code = 0x%x", ds_errno);
  }

  switch (estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_type)
  {
    case GSTK_BEARER_DEFAULT:
      profile_num = 0;
      /* We need set both 3gpp profile and 3gpp2 profile to support inter-RAT handover */
      if(estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len == 0)
      {
        if(estk_pre_process_3gpp_profile(ch_idx, &profile_num, &has_apn) != ESTK_SUCCESS)
        {
          profile_3gpp = FALSE;
          UIM_MSG_ERR_0("failed to pre process 3gpp profile!");
        }
      } /* if (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len == 0) */

      /* bip_channel_table[].apn might have got populated before start of the function
         due to OPEN CH having APN TLV in itself or by estk_pre_process_3gpp_profile() above */
      if(estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len != 0)
      {
        if(estk_set_net_policy_3gpp_profile(ch_idx, &profile_num) != ESTK_SUCCESS)
        {
          profile_3gpp = FALSE;
          UIM_MSG_ERR_0("Failed to set 3gpp profile!");
        }
      }
      UIM_MSG_HIGH_1("Use profile 0x%x for 3GPP", profile_num);
      net_policy->umts.pdp_profile_num = profile_num;
      
      profile_num = DS_PROFILE_UNSPECIFIED_NUM;
      
      /* If valid RATs are available */
      if(sys_status != NULL && sys_status->num_avail_sys > 0)
      {
        if((sys_status->avail_sys[0].rat_value == DS_SYS_RAT_EX_3GPP2_1X)   ||
           (sys_status->avail_sys[0].rat_value == DS_SYS_RAT_EX_3GPP2_HRPD) || 
           (!has_apn))
        {
          profile_num = DS_PROFILE_ALWAYS_SHARE_PROFILE_NUM;
        }
        if(profile_num == DS_PROFILE_UNSPECIFIED_NUM &&
           (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len != 0))
        {
          profile_num = 107;
          if (estk_set_net_policy_3gpp2_profile(ch_idx, &profile_num) != ESTK_SUCCESS)
          {
            UIM_MSG_ERR_0("failed to set 3GPP2 profile!");
            profile_3gpp2 = FALSE;
          }
        }
        else
        {
          /* This is a case of DS currently using one of the 3GPP RATs as most preffered
             RAT and BIP has choosen default 3GPP profile earlier. In this case, for
             3GPP2, let's chose Profile 254 because there is no APN available */
          profile_num = DS_PROFILE_ALWAYS_SHARE_PROFILE_NUM;
        }
        UIM_MSG_HIGH_1("Use profile 0x%x for 3GPP2", profile_num);
      } /* if(sys_status->num_avail_sys > 0) */
      net_policy->cdma.data_session_profile_id = profile_num;
      if(!profile_3gpp && !profile_3gpp2)
      {
        UIM_MSG_ERR_0("Failed to set both 3GPP and 3GPP2 profiles!");
        return ESTK_ERROR;
      }
      break;
    case GSTK_GPRS:
    case GSTK_EUTRAN_EXT_PARAM:
    case GSTK_EUTRAN_MAPPED_UTRAN_PS:
      switch (access_tech)
      {
        case GSTK_ACCESS_TECH_GSM:
        case GSTK_ACCESS_TECH_UTRAN:
        case GSTK_ACCESS_TECH_LTE:
          profile_num = 0;
          if(estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len == 0)
          {
            if(estk_pre_process_3gpp_profile(ch_idx, &profile_num, &has_apn) != ESTK_SUCCESS)
            {
              UIM_MSG_ERR_0("failed to pre process 3gpp profile!");
              return ESTK_ERROR;
            }
          }
          /* bip_channel_table[].apn might have got populated before start of the function
             due to OPEN CH having APN TLV in itself or by estk_pre_process_3gpp_profile() above */
          if(estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len != 0)
          {
            if(estk_set_net_policy_3gpp_profile(ch_idx, &profile_num) != ESTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Failed to set 3gpp profile!");
              return ESTK_ERROR;
            }
          }
          UIM_MSG_HIGH_1("Use profile 0x%x for 3GPP", profile_num);
          net_policy->umts.pdp_profile_num = profile_num;
          break;
        default:
          UIM_MSG_ERR_1("invalid rat 0x%x for 3GPP bearer types!", access_tech);
          return ESTK_ERROR;
      } /* switch (access_tech) */

      profile_num = DS_PROFILE_UNSPECIFIED_NUM;
      /* If valid RATs are available */
      if(sys_status != NULL && sys_status->num_avail_sys > 0)
      {
        if((sys_status->avail_sys[0].rat_value == DS_SYS_RAT_EX_3GPP2_1X)   ||
           (sys_status->avail_sys[0].rat_value == DS_SYS_RAT_EX_3GPP2_HRPD) || 
           (!has_apn))
        {
          profile_num = DS_PROFILE_ALWAYS_SHARE_PROFILE_NUM;
        }
        if(profile_num == DS_PROFILE_UNSPECIFIED_NUM &&
           (estk_curr_inst_ptr->bip_channel_table[ch_idx].apn_len != 0))
        {
          profile_num = 107;
          if (estk_set_net_policy_3gpp2_profile(ch_idx, &profile_num) != ESTK_SUCCESS)
          {
            UIM_MSG_ERR_0("failed to set 3gpp2 profile!");
          }
        }
        else
        {
          /* This is a case of DS currently using one of the 3GPP RATs as most preffered
             RAT and BIP has choosen default 3GPP profile earlier. In this case, for
             3GPP2, let's chose Profile 254 because there is no APN available */
          profile_num = DS_PROFILE_ALWAYS_SHARE_PROFILE_NUM;
        }
        UIM_MSG_HIGH_1("Use profile 0x%x for 3gpp2", profile_num);
      } /* if(sys_status->num_avail_sys > 0) */
      net_policy->cdma.data_session_profile_id = profile_num;
      break;
    default:
      UIM_MSG_ERR_1("bearer type 0x%x not supported!",
                    estk_curr_inst_ptr->bip_channel_table[ch_idx].bearer_desc.bearer_type);
      return ESTK_ERROR;
  }

  UIM_MSG_HIGH_1("set profile num 0x%x to net policy!", profile_num);
  return ESTK_SUCCESS;
} /* estk_set_net_policy_profile */

/*===========================================================================

FUNCTION: estk_bip_check_and_start_fallback

DESCRIPTION :
  This function checks if a bearer fallback (from Cellular to WLAN or
  vice-versa) can be attempted, and starts the fallback as needed

PARAMETERS:
  index : [Input] Index of the channel for which fallback should be attempted

DEPENDENCIES:
  None

RETURN VALUE:
  boolean
    ESTK_SUCCESS: Fallback started
    ESTK_ERROR: Fallback started but errored out
    ESTK_UNSUPPORTED: Fallback not attempted

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_check_and_start_fallback(int32 index)
{
  boolean                              cellular_data_avail = FALSE;
  boolean                              wlan_avail          = FALSE;
  uint32                               i                   = 0;
  gstk_slot_id_enum_type               active_slot         = GSTK_SLOT_ID_MAX;
  gstk_status_enum_type                gstk_status         = GSTK_SUCCESS;
  const ds_sys_system_status_ex_type  *sys_status          = NULL;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("In estk_bip_check_and_start_fallback, invalid ch idx: 0x%x", index);
    return ESTK_UNSUPPORTED;
  }

  UIM_MSG_HIGH_2("In estk_bip_check_and_start_fallback, index: 0x%x, nw_type: 0x%x",
                 index,
                 estk_curr_inst_ptr->bip_channel_table[index].nw_type);

  if (estk_curr_inst_ptr->keep_ch_open &&
      estk_curr_inst_ptr->ch_open_id != -1 &&
      index == estk_curr_inst_ptr->ch_open_id)
  {
    UIM_MSG_HIGH_1("CH %d being reused- no need to fallback", index);
    return ESTK_UNSUPPORTED;
  }

  /* For UDP, the BIP Destination Server is first contacted when a SEND DATA
     is received. If the first SEND DATA was a success (i.e. Session with
     server is established), then there is no need to attempt a fallback */
  if((estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_UDP) &&
     (estk_curr_inst_ptr->bip_channel_table[index].first_dss_sendto_done == TRUE))
  {
    UIM_MSG_HIGH_0("UDP: first Send to was successful, no need to fallback");
    return ESTK_UNSUPPORTED;
  }

  /* If current bearer is WLAN, and WLAN has higher priority over Cellular,
     it means a fallback to Cellular can be attempted */
  if((estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN) &&
     (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_ESTK_BIP_WLAN_PRIO,
        estk_curr_inst_ptr->slot_id) == TRUE))
  {
    if(gstk_io_ctrl(GSTK_IO_GET_DS_SYSTEM_RAT_INFO,
                    (void *)&estk_curr_inst_ptr->slot_id,
                    (void *)&sys_status) == GSTK_SUCCESS)
    {
      /* Check if any Cellular RATs are available */
      for(i = 0;
          i < sys_status->num_avail_sys;
          i++)
      {
        if((sys_status->avail_sys[i].technology ==
            DS_SYS_NETWORK_3GPP) ||
           (sys_status->avail_sys[i].technology ==
            DS_SYS_NETWORK_3GPP2))
        {
          if(sys_status->avail_sys[i].rat_value !=
             DS_SYS_RAT_EX_NULL_BEARER)
          {
            cellular_data_avail = TRUE;
            break;
          }
        }
      }
    }

    UIM_MSG_HIGH_1("cellular_data_avail: 0x%x", cellular_data_avail);
    /* Fallback to Cellular */
    if(cellular_data_avail == TRUE)
    {
      gstk_status = gstk_io_ctrl(GSTK_IO_GET_ACTIVE_DATA_SUB_SLOT_ID,
                                 (void *)&estk_curr_inst_ptr->slot_id,
                                 (void *)&active_slot);

      /* If BIP transaction is requested on the slot with the active DATA
         subscription, BIP over Cellular can be allowed */
      if((gstk_status == GSTK_SUCCESS) &&
         (active_slot == estk_curr_inst_ptr->slot_id))
      {
        if(!estk_bip_check_existing_gsm_cs_call())
        {
          estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_FALLBACK_IN_PROGRESS;

          /* Close existing WLAN socket and start fallback */
          (void)estk_bip_process_sock_close_evt(
                  estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                  estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference);

          /* If channel status is not Fallback in Progress (implying existing
             socket close is ongoing) or Pending (implying setup of fallback bearer
             has started), means fallback has failed */
          if((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_FALLBACK_IN_PROGRESS) ||
             (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING))
          {
            UIM_MSG_HIGH_0("Fallback to Cellular started");
            return ESTK_SUCCESS;
          }
          else
          {
            UIM_MSG_HIGH_0("Fallback attempt to Cellular failed");
            return ESTK_ERROR;
          }
        }
        else
        {
           return ESTK_UNSUPPORTED;
        }
      }
    }
  }
  /* If current bearer is Cellular, and BIP over WLAN support is present,
     Cellular has higher priority over WLAN, WLAN is usable,
     Bearer is default type and IP address is IPv4, it means a fallback
     to WLAN can be attempted */
  else if((estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_CELLULAR) &&
          (gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_ESTK_BIP_OVER_WLAN,
             estk_curr_inst_ptr->slot_id) == TRUE) &&
          (gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_ESTK_BIP_WLAN_PRIO,
             estk_curr_inst_ptr->slot_id) == FALSE) &&
          (estk_shared_info.wlan_unusable == FALSE) &&
          (estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd.bearer.bearer_type ==
           GSTK_BEARER_DEFAULT) &&
          (estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd.data_dest_address.address_type !=
           GSTK_PKT_DATA_IPV6_ADDRESS))
  {
    gstk_status = gstk_io_ctrl(GSTK_IO_GET_WLAN_AVAILABILITY,
                               (void *)&estk_curr_inst_ptr->slot_id,
                               (void *)&wlan_avail);

    UIM_MSG_HIGH_1("WLAN Available: 0x%x", wlan_avail);

    /* Fallback to WLAN */
    if((gstk_status == GSTK_SUCCESS)&& (wlan_avail == TRUE))
    {
      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_FALLBACK_IN_PROGRESS;

      /* Close existing Cellular socket and start fallback */
      (void)estk_bip_process_sock_close_evt(
              estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
              estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference);

      /* If channel status is not Fallback in Progress (implying existing
         socket close is ongoing) or Pending (implying setup of fallback bearer
         has started), means fallback has failed */
      if((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_FALLBACK_IN_PROGRESS) ||
         (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING))
      {
        UIM_MSG_HIGH_0("Fallback to WLAN started");
        return ESTK_SUCCESS;
      }
      else
      {
        UIM_MSG_HIGH_0("Fallback attempt to WLAN failed");
        return ESTK_ERROR;
      }
    }
  }
  return ESTK_UNSUPPORTED;
} /* estk_bip_check_and_start_fallback */

/*===========================================================================

FUNCTION: estk_bringup_iface

DESCRIPTION :
  This function brings up iface for a PDN connection

PARAMETERS:
  index : Channel index
  address_family: DSS_AF_INET or DSS_AF_INET6

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bringup_iface(
  uint32 index,
  uint16 address_family)
{
  sint15                    error_num        = DSS_ERROR;
  dss_sock_cb_fcn           sock_cb          = estk_bip_socket_cb;
  dss_net_cb_fcn            net_cb           = estk_bip_network_cb;
  gstk_open_ch_req_type     *open_ch_cmd       = NULL;
  dss_net_policy_info_type  policy_info;

  UIM_MSG_HIGH_2("estk_bringup_iface: index=0%d, address_family=%d",
                 index, address_family);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  }

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  memset(&policy_info, 0x00, sizeof(dss_net_policy_info_type));
  dss_init_net_policy_info(&policy_info);
  policy_info.iface.kind = DSS_IFACE_NAME;
  policy_info.is_routeable = TRUE;
  policy_info.family = address_family;

  if(estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN)
  {
    policy_info.iface.info.name = DSS_IFACE_WLAN;
  }
  else
  {
    policy_info.iface.info.name = DSS_IFACE_EPC_ANY;

#ifdef FEATURE_EHRPD
    policy_info.cdma.data_session_profile_id = 107;
#else
    policy_info.cdma.data_session_profile_id = 0;
#endif
  }

  if(estk_set_net_policy_profile(estk_curr_access_tech,
                                 index,
                                 &policy_info) != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to set net policy profile!");
    return ESTK_ERROR;
  }

  /* Open DS network library and get network handle*/
  estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle =
    dss_open_netlib2(net_cb,
                     (void *)&(estk_curr_inst_ptr->slot_id),
                     sock_cb,
                     (void *)&(estk_curr_inst_ptr->slot_id),
                     &policy_info,
                     &error_num);

  if (estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle == DSS_ERROR)
  {
    UIM_MSG_ERR_0("Could not open network library");

    if (open_ch_cmd != NULL && open_ch_cmd->dns_requested && address_family == DSS_AF_INET &&
        (gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
           estk_curr_inst_ptr->slot_id) == FALSE))
    {
      UIM_MSG_ERR_0("DNS query: iface v4 bringup fails, attempt iface v6...");
      return estk_bringup_iface(index, DSS_AF_INET6);
    }
    else
    {
      return ESTK_ERROR;
    }
  }

  if (address_family == DSS_AF_INET)
  {
    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 =
      estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle;
  }
  else if (address_family == DSS_AF_INET6)
  {
    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 =
      estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle;
  }

   /* Call dsnet_start to start network subsystem */
  if(dsnet_start(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle, &error_num) != DSS_SUCCESS)
  {
    UIM_MSG_HIGH_1("dsnet_Start returned %d", error_num);
    switch(error_num)
    {
    case DS_EWOULDBLOCK:
      return ESTK_SUCCESS;
    default:
      UIM_MSG_HIGH_1("dsnet_start return unhandled return status %d",
                     error_num);
      if (open_ch_cmd != NULL && open_ch_cmd->dns_requested && address_family == DSS_AF_INET &&
          (gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
             estk_curr_inst_ptr->slot_id) == FALSE))
      {
        UIM_MSG_ERR_0("DNS query: iface v4 bringup fails, attempt iface v6...");
        return estk_bringup_iface(index, DSS_AF_INET6);
      }
      return ESTK_ERROR;
    }
  }
  else
  {
    return ESTK_SUCCESS;
  }
} /* estk_bringup_iface */

/*===========================================================================

FUNCTION: estk_bip_establish_link

DESCRIPTION :
  This function creates the socket and establishes link

PARAMETERS:
  index : [Input] Index of the active channel

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_establish_link(int32 index, uint16 address_family)
{
  boolean                            perform_net_connect = FALSE;
  gstk_general_result_enum_type      gen_result      = GSTK_GENERAL_RESULT_MAX;
  gstk_terminal_rsp_extra_param_type tr_extra_param;

  UIM_MSG_HIGH_2("estk_bip_establish_link: index=%d,address_family=%d",
                 index, address_family);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  }

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  if (estk_curr_inst_ptr->keep_ch_open &&
      estk_curr_inst_ptr->ch_open_id != -1 &&
      index == estk_curr_inst_ptr->ch_open_id)
  {
    UIM_MSG_HIGH_0("estk_bip_establish_link: CHANNEL reuse");
    /* Reusing previous CHANNEL */
    if (address_family == DSS_AF_INET &&
        estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 != DSS_ERROR)
    {
      if (estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 != DS_ENETISCONN)
      {
        UIM_MSG_ERR_0("estk_bip_establish_link: IPv4 interface status != DS_ENETISCONN");
        return ESTK_ERROR;
      }

      estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle =
        estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4;
      perform_net_connect = TRUE;
    }
    else if (address_family == DSS_AF_INET6 &&
             estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 != DSS_ERROR)
    {
      if (estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v6 != DS_ENETISCONN)
      {
        UIM_MSG_ERR_0("estk_bip_establish_link: IPv6 interface status != DS_ENETISCONN");
        return ESTK_ERROR;
      }

      estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle = estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6;
      perform_net_connect = TRUE;
    }
    else
    {
      /*
        If profile was set to IPV4 only, and we don't have a valid v4 nethandle, then
        we must return error

        If profile was NOT set to IPV4 only, and we don't have a valid v4 nethandle, then
        we must attempt to bring up v4 nethandle

        If profile was NOT set as IPV4 only, and we don't have a valid v6 nethandle, then
        we must attempt to bring up v6 nethandle
      */
      if(gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT,
           estk_curr_inst_ptr->slot_id) == FALSE)
      {
        if (address_family == DSS_AF_INET &&
            estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 != DSS_ERROR)
        {
          UIM_MSG_HIGH_0("Missing IPV4 net handle");
        }
        else if (address_family == DSS_AF_INET6 &&
            estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 != DSS_ERROR)
        {
          UIM_MSG_HIGH_0("Missing IPV6 net handle");
        }
        else
        {
          UIM_MSG_ERR_2("invalid sock_nethandles %d, %d",
                        estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4,
                        estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6);
          return ESTK_ERROR;
        }
        UIM_MSG_HIGH_1("Attempting to bring up address_family = 0x%x nethandle", address_family);
        return estk_bringup_iface(index, address_family);
      }
      UIM_MSG_ERR_2("estk_bip_establish_link: invalid sock_nethandle! %d, %d",
                    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4,
                    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6);
      return ESTK_ERROR;
    }
  }

  if (perform_net_connect)
  {
    estk_curr_inst_ptr->bip_channel_table[index].iface_id =
      dss_get_iface_id(estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle);
    if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE)
    {
      return estk_perform_net_connect();
    }
    else
    {
      if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ)
      {
        /*Send success terminal response*/
        gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        if(estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified)
        {
          gen_result = GSTK_COMMAND_PERFORMED_WITH_MODIFICATION;
        }

        tr_extra_param.present = TRUE;
        tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
          estk_curr_inst_ptr->bip_channel_table[index].channel_id;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
          TRUE;
        tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
          estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
        UIM_MSG_HIGH_0("Copying bearer param for terminal response");
        (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
               sizeof(gstk_bearer_description_type),
               &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
               sizeof(gstk_bearer_description_type));

        tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
          estk_curr_inst_ptr->bip_channel_table[index].buffer_size;

        return estk_bip_send_terminal_response(
                 estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                 GSTK_OPEN_CH_CNF,
                 gen_result,
                 NULL,
                 &tr_extra_param,
                 index);
      }
      else if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_SEND_DATA_REQ)
      {
        UIM_MSG_HIGH_0("Returning success because in Send Data command");
        return ESTK_SUCCESS;
      }
    }
  }

  /* Not reusing previous CHANNEL */
  return estk_bringup_iface(index, address_family);
} /* estk_bip_establish_link */

/*===========================================================================

FUNCTION: estk_populate_address_in_open_ch_table

DESCRIPTION :
  This function populates local or destination address into the Open channel
  Table.

PARAMETERS:
  index : [Input] Index of the channel to be populated
  address_type : [Input] Local or destination address

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_populate_address_in_open_ch_table(
  int32                       index,
  estk_address_type           address_type,
  gstk_pkt_data_address_type  *pkt_data_addr_ptr)
{
  char                       *IP_address   = NULL;
  sint31                     dss_status    = DSS_ERROR;
  int16                      dss_errno     = 0;
  estk_result_enum_type      estk_status   = ESTK_ERROR;
  struct ps_sockaddr_storage **p_addr_ptr  = NULL;
  struct ps_sockaddr_in      *p_addr_v4    = NULL;
  struct ps_sockaddr_in6     *p_addr_v6    = NULL;

  UIM_MSG_HIGH_3("estk_populate_address_in_open_ch_table: index=0x%x "
                 "address_type=0x%x, "
                 "pkt_data_addr_ptr=0x%x",
                 index, address_type, pkt_data_addr_ptr);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(pkt_data_addr_ptr == NULL)
  {
    return ESTK_SUCCESS;
  }

  if(index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  }

  if(address_type > ESTK_DESTINATION_ADDRESS)
  {
    UIM_MSG_ERR_0("Invalid address type to populate open channel table");
    return ESTK_ERROR;
  }

  if(pkt_data_addr_ptr->pkt_address.length > 0)
  {
    switch(address_type)
    {
    case ESTK_LOCAL_ADDRESS:
      p_addr_ptr = &estk_curr_inst_ptr->bip_channel_table[index].src_addr;
      break;
    case ESTK_DESTINATION_ADDRESS:
      p_addr_ptr = &estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
      break;
    default:
      UIM_MSG_ERR_1("Invalid addr type 0x%x!", address_type);
      return ESTK_ERROR;
    }

    /* Free existing address as the address will be re-alloc-ed for channel reuse */
    if (p_addr_ptr != NULL && *p_addr_ptr != NULL)
    {
      gstk_free(*p_addr_ptr);
      *p_addr_ptr = NULL;
    }

    *p_addr_ptr =
        (struct ps_sockaddr_storage*)GSTK_CALLOC(sizeof(struct ps_sockaddr_storage));
    if (*p_addr_ptr == NULL)
    {
      return ESTK_ERROR;
    }
    memset(*p_addr_ptr, 0x00, sizeof(struct ps_sockaddr_storage));
    p_addr_v4 = (struct ps_sockaddr_in*)(*p_addr_ptr);
    p_addr_v6 = (struct ps_sockaddr_in6*)(*p_addr_ptr);

    if(estk_convert_to_ip_format(pkt_data_addr_ptr,
                                 &IP_address) != ESTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Local IP address conversion to IPV4 format failed");
    }
    else
    {
      switch(pkt_data_addr_ptr->address_type)
      {
      case GSTK_PKT_DATA_IPV4_ADDRESS:
        p_addr_v4->ps_sin_family = DSS_AF_INET;
        dss_status = dss_inet_pton(
                       IP_address,
                       DSS_AF_INET,
                       &p_addr_v4->ps_sin_addr,
                       sizeof(struct ps_in_addr),
                       &dss_errno);
        if (dss_status != DSS_SUCCESS)
        {
          UIM_MSG_ERR_2("failed inet_pton()! 0x%x, 0x%x",
                        dss_status, dss_errno);
          estk_status = ESTK_ERROR;
        }
        else
        {
          estk_status = ESTK_SUCCESS;
        }
        break;
      case GSTK_PKT_DATA_IPV6_ADDRESS:
        p_addr_v6->ps_sin6_family = DSS_AF_INET6;
        dss_status = dss_inet_pton(
                       IP_address,
                       DSS_AF_INET6,
                       &p_addr_v6->ps_sin6_addr,
                       sizeof(struct ps_in6_addr),
                       &dss_errno);
        if (dss_status != DSS_SUCCESS)
        {
          UIM_MSG_ERR_2("failed inet_pton()! 0x%x, 0x%x",
                        dss_status, dss_errno);
          estk_status = ESTK_ERROR;
        }
        else
        {
          estk_status = ESTK_SUCCESS;
        }
        break;
      default:
        UIM_MSG_ERR_1("invalid address_type 0x%x!",
                      pkt_data_addr_ptr->address_type);
        estk_status = ESTK_ERROR;
      }

      gstk_free(IP_address);
      IP_address = NULL;
    }
  }
  else
  {
    estk_status = ESTK_SUCCESS;
  }
  return estk_status;
} /* estk_populate_address_in_open_ch_table */

/*===========================================================================

FUNCTION: estk_modify_apn_based_on_mapping_table

DESCRIPTION :
  This function modifies APN based on pre-configured APN mapping table

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_modify_apn_based_on_mapping_table(int32 index)
{
  int32                    idx         = 0;
  uint32                   i           = 0;
  uint8                   *new_apn     = NULL;
  uint32                   new_apn_len = 0;
  gstk_status_enum_type    gstk_status = GSTK_ERROR;
  gstk_nv_items_data_type  nv_data;

  UIM_MSG_HIGH_1("estk_modify_apn_based_on_mapping_table: index=0x%x", index);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  memset(&nv_data, 0x00, sizeof(nv_data));

  nv_data.apn_mapping_table_ptr =
    (gstk_apn_mapping_table_data_type*)GSTK_CALLOC(sizeof(gstk_apn_mapping_table_data_type));

  if (nv_data.apn_mapping_table_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  /* Read the BIP APN mapping table */
  gstk_status = gstk_nv_access_read(GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE,
                                    estk_curr_inst_ptr->slot_id,
                                    &nv_data);

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("estk_read_apn_mapping_table() failed, skip mapping");
    gstk_free(nv_data.apn_mapping_table_ptr);
    return ESTK_SUCCESS;
  }

  /*Start comparison and do APN mapping if necessary...*/
  UIM_MSG_HIGH_2("estk_curr_inst_ptr->bip_channel_table[%x].apn_len = %x ",
                 index, estk_curr_inst_ptr->bip_channel_table[index].apn_len);

  for (idx = 0; idx < ESTK_APN_MAPPING_MAX; ++ idx)
  {
    UIM_MSG_HIGH_1("APN table idx: %x ", idx);

    /* exact mapping */
    if (nv_data.apn_mapping_table_ptr->value[idx].input_apn &&
        nv_data.apn_mapping_table_ptr->value[idx].input_apn[0] != '\0' &&
        estk_curr_inst_ptr->bip_channel_table[index].apn_len ==
        strlen(nv_data.apn_mapping_table_ptr->value[idx].input_apn) &&
        strncasecmp((char *)estk_curr_inst_ptr->bip_channel_table[index].apn,
                    nv_data.apn_mapping_table_ptr->value[idx].input_apn,
                    estk_curr_inst_ptr->bip_channel_table[index].apn_len) == 0)
    {
      new_apn_len = strlen(nv_data.apn_mapping_table_ptr->value[idx].output_apn);
      new_apn = (uint8 *)GSTK_CALLOC(new_apn_len + 1);
      if (new_apn == NULL)
      {
        gstk_free(nv_data.apn_mapping_table_ptr);
        return ESTK_ERROR;
      }
      memset(new_apn,0,new_apn_len + 1);

      if (estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
      {
        gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
        estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
      }

      (void)gstk_memcpy(new_apn,
                     nv_data.apn_mapping_table_ptr->value[idx].output_apn,
                     new_apn_len,
                     new_apn_len,
                     new_apn_len);

      estk_curr_inst_ptr->bip_channel_table[index].apn = new_apn;
      estk_curr_inst_ptr->bip_channel_table[index].apn_len = new_apn_len;
      UIM_MSG_HIGH_0("*** New APN ***");
      for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++ i)
      {
        UIM_MSG_HIGH_2("APN[%d]: %c",
                       i, estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
      }
      gstk_free(nv_data.apn_mapping_table_ptr);
      return ESTK_SUCCESS;
    }
  }

  gstk_free(nv_data.apn_mapping_table_ptr);
  return ESTK_SUCCESS;
} /* estk_modify_apn_based_on_mapping_table */

/*===========================================================================

FUNCTION: estk_modify_apn_based_on_class_and_rat

DESCRIPTION :
  This function modifies APN to pre-configured APN based on profile class type

PARAMETERS:
  index : [Input] Index of the channel to be populated\
  rat : [Input] The type of radio access technology
  class_type : [Input] The class type of profile to be looked for

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_modify_apn_based_on_class_and_rat(
  int32 index,
  ds_profile_tech_etype rat,
  ds_profile_3gpp2_apn_class class_type)
{
  ds_profile_status_etype             itr_status       = DS_PROFILE_REG_RESULT_SUCCESS;
  ds_profile_status_etype             api_ret_status   = DS_PROFILE_REG_RESULT_SUCCESS;
  ds_profile_list_type                profile_list;
  ds_profile_itr_type                 profile_itr;
  ds_profile_list_info_type           profile_list_info;
  ds_profile_info_type                profile_list_name;
  ds_profile_info_type                profile_info;
  ds_profile_hndl_type                profile_hndl;
  ds_profile_3gpp2_apn_class          apn_class        = 0;
  char                                profile_3gpp2_apn[DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1];
  char                                profile_3gpp_apn[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1];
  char                                profile_name[DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN];
  estk_result_enum_type               estk_status      = ESTK_SUCCESS;
  uint8                               *new_apn         = NULL;
  uint32                              i                = 0;

  UIM_MSG_HIGH_3("estk_modify_apn_based_on_class_and_rat(): index=0x%x, "
                 "rat=0x%x, "
                 "class_type=0x%x", index, rat, class_type);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  memset((void *)&profile_list, 0, sizeof(ds_profile_list_type));
  memset((void *)&profile_itr, 0, sizeof(ds_profile_itr_type));
  memset((void *)&profile_list_info, 0, sizeof(ds_profile_list_info_type));
  memset((void *)&profile_info, 0, sizeof(ds_profile_info_type));
  memset((void *)profile_3gpp_apn, 0, sizeof(profile_3gpp_apn));
  memset((void *)profile_3gpp2_apn, 0, sizeof(profile_3gpp2_apn));
  memset((void *)profile_name, 0, sizeof(profile_name));

  profile_list.dfn = DS_PROFILE_LIST_ALL_PROFILES;
  profile_list_info.name = &(profile_list_name);
  profile_list_info.name->len = DS_PROFILE_3GPP2_MAX_PROFILE_NAME_LEN;
  profile_list_info.name->buf = NULL;
  if (rat == DS_PROFILE_TECH_3GPP)
  {
    profile_list_info.name->len = DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN;
    profile_list_info.name->buf = profile_name;
    profile_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1;
    profile_info.buf = profile_3gpp_apn;
  }

  /* Get an iterator first */
  itr_status = ds_profile_get_list_itr(rat, &profile_list, &profile_itr);

  if(itr_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    UIM_MSG_ERR_1("ds_profile_get_list_itr failed %d", api_ret_status);
    return ESTK_ERROR;
  }

  while (itr_status == DS_PROFILE_REG_RESULT_SUCCESS)
  {
    profile_list_info.name->len = (rat == DS_PROFILE_TECH_3GPP) ?
                                    DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN :
                                    DS_PROFILE_3GPP2_MAX_PROFILE_NAME_LEN;

    /* Get the profile number for current profile using iterator */
    api_ret_status = ds_profile_get_info_by_itr(profile_itr,
                                                &profile_list_info);

    ESTK_BIP_DESTROY_ITR_IF_FAIL(api_ret_status, profile_itr);

    /* Get handle on the profile */
    api_ret_status = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                                  rat,
                                                  profile_list_info.num,
                                                  &profile_hndl);

    ESTK_BIP_DESTROY_ITR_IF_FAIL(api_ret_status, profile_itr);

    /* Get the profile class type */
    profile_info.buf = &apn_class;
    profile_info.len = sizeof(apn_class);
    ESTK_BIP_GET_PROFILE_PARAM(
      api_ret_status,
      profile_hndl,
      profile_itr,
      profile_info,
      (rat == DS_PROFILE_TECH_3GPP) ?
        (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS :
        (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_APN_CLASS);

    UIM_MSG_HIGH_2("profile num: 0x%x, class type: 0x%x",
                   profile_list_info.num, apn_class);

    if (apn_class == class_type)
    {
      /* Get profile APN */
      profile_info.buf = (rat == DS_PROFILE_TECH_3GPP) ?
                           profile_3gpp_apn : profile_3gpp2_apn;
      profile_info.len = (rat == DS_PROFILE_TECH_3GPP) ?
                           (DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1) :
                           (DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1);

      ESTK_BIP_GET_PROFILE_PARAM(
        api_ret_status,
        profile_hndl,
        profile_itr,
        profile_info,
        (rat == DS_PROFILE_TECH_3GPP) ?
          (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN :
          (ds_profile_identifier_type)DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING);

      new_apn = (uint8 *)GSTK_CALLOC(profile_info.len);
      if (new_apn == NULL)
      {
        estk_status = ESTK_ERROR;
      }
      else
      {
        if (estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
        {
          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
          estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
        }

        estk_curr_inst_ptr->bip_channel_table[index].apn = new_apn;
        (void)gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[index].apn,
                       profile_info.buf,
                       profile_info.len,
                       profile_info.len,
                       profile_info.len);

        if (estk_curr_inst_ptr->bip_channel_table[index].apn)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn_len = profile_info.len;
          /**** New APN ****/
          for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++ i)
          {
            UIM_MSG_HIGH_2("APN[%d]: %c",
                           i, estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
          }
          estk_status = ESTK_SUCCESS;
        }
      }
      (void)ds_profile_end_transaction(profile_hndl,
                                       DS_PROFILE_ACTION_CANCEL);
      (void)ds_profile_itr_destroy(profile_itr);
      return estk_status;
    }

    (void)ds_profile_end_transaction(profile_hndl,
                                     DS_PROFILE_ACTION_CANCEL);
    itr_status = ds_profile_itr_next(profile_itr);
    UIM_MSG_HIGH_1("profile iterator status: 0x%x", itr_status);
  }
  (void)ds_profile_itr_destroy(profile_itr);
  UIM_MSG_ERR_1("Can't find class %d profile!", class_type);
  return ESTK_ERROR;
} /*estk_modify_apn_based_on_class_and_rat*/

/*===========================================================================

FUNCTION: estk_modify_apn_based_on_class

DESCRIPTION :
  This function modifies APN to pre-configured APN based on profile class type

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_modify_apn_based_on_class(int32 index)
{
  typedef struct
  {
    char *apn;
    ds_profile_3gpp2_apn_class apn_class;
  } estk_apn_class_info_type;

  estk_apn_class_info_type apn_class_info[] =
    {
      {"vzwinternet", 3},
      {"vzwadmin", 2},
      {"pcsadmin", 2}
    };
  int32  idx = 0;
  int32  info_size = sizeof(apn_class_info) / sizeof(estk_apn_class_info_type);
  uint32 cmp_len = 0;

  UIM_MSG_HIGH_1("estk_modify_apn_based_on_class: index=0x%x", index);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  }

  for (idx = 0; idx < info_size; ++ idx)
  {
    cmp_len = strlen(apn_class_info[idx].apn);
    cmp_len = (estk_curr_inst_ptr->bip_channel_table[index].apn_len < cmp_len) ?
                estk_curr_inst_ptr->bip_channel_table[index].apn_len:
                cmp_len;

    if (strncasecmp((char *)estk_curr_inst_ptr->bip_channel_table[index].apn,
                    apn_class_info[idx].apn,
                    cmp_len) == 0)
    {
      break;
    }
  }
  if (idx == info_size)
  {
    /* no need to modify */
    UIM_MSG_HIGH_0("No need to modify APN.");
    return ESTK_SUCCESS;
  }

  switch (estk_curr_access_tech)
  {
    case GSTK_ACCESS_TECH_CDMA:
    case GSTK_ACCESS_TECH_HRPD:
    case GSTK_ACCESS_TECH_EHRPD:
      /* do look-up in 3GPP2 profiles */
      return estk_modify_apn_based_on_class_and_rat(
               index,
               DS_PROFILE_TECH_3GPP2,
               apn_class_info[idx].apn_class);
    default:
      /* do look-up in 3GPP profiles */
      return estk_modify_apn_based_on_class_and_rat(
               index,
               DS_PROFILE_TECH_3GPP,
               apn_class_info[idx].apn_class);
  }
} /* estk_modify_apn_based_on_class */

/*===========================================================================

FUNCTION: estk_bip_get_prio_network

DESCRIPTION :
  This function returns the network on which the BIP session should be
  attempted: Cellular or WLAN. The decision is made based on:
  - Bearer type (Default or Packet Data)
  - Destination IP address (IPv4 or IPv6)
  - If BIP over WLAN is supported (GSTK_CFG_FEATURE_ESTK_BIP_OVER_WLAN)
  - If BIP over WLAN has higher priority than Cellular
    (GSTK_CFG_FEATURE_ESTK_BIP_WLAN_PRIO)
  - Which networks are available (WLAN/Cellular)

PARAMETERS:
  nw_type_ptr : [Output] Contains the Priority network
  addi_res_ptr : [Output] Contains Additional Result for an Error TR

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_get_prio_network(
  estk_network_type               *nw_type_ptr,
  uint8                           *addi_res_ptr,
  gstk_slot_id_enum_type           slot,
  gstk_bearer_code_enum_type       bearer_type,
  gstk_pkt_data_address_enum_type  address_type
)
{
  boolean                               wlan_avail           = FALSE;
  boolean                               wlan_allowed         = FALSE;
  boolean                               cellular_allowed     = FALSE;
  boolean                               cellular_data_avail  = FALSE;
  uint32                                index                = 0;
  gstk_status_enum_type                 gstk_status          = GSTK_SUCCESS;
  gstk_slot_id_enum_type                active_slot          = GSTK_SLOT_ID_MAX;
  gstk_open_ch_req_type                *open_ch_cmd          = NULL;
  const ds_sys_system_status_ex_type   *sys_status           = NULL;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  UIM_MSG_HIGH_3("In estk_bip_get_prio_network, slot: 0x%x, bearer_type: 0x%x, address type: 0x%x",
                 slot, bearer_type, address_type);

  if(nw_type_ptr == NULL || addi_res_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null pointer for Network type", nw_type_ptr, addi_res_ptr);
    return ESTK_ERROR;
  }

  if(!ESTK_IS_VALID_SLOT_ID(slot))
  {
    return ESTK_ERROR;
  }

  *nw_type_ptr = ESTK_NETWORK_NONE;
  *addi_res_ptr = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

  (void)gstk_io_ctrl(GSTK_IO_GET_WLAN_AVAILABILITY,
                     (void *)&slot,
                     (void *)&wlan_avail);

  if(gstk_io_ctrl(GSTK_IO_GET_DS_SYSTEM_RAT_INFO,
                  (void *)&estk_curr_inst_ptr->slot_id,
                  (void *)&sys_status) == GSTK_SUCCESS)
  {
    /* Check if any Cellular RATs are available */
    for(index = 0;
        index < sys_status->num_avail_sys;
        index++)
    {
      if((sys_status->avail_sys[index].technology ==
          DS_SYS_NETWORK_3GPP) ||
         (sys_status->avail_sys[index].technology ==
          DS_SYS_NETWORK_3GPP2))
      {
        if(sys_status->avail_sys[index].rat_value !=
           DS_SYS_RAT_EX_NULL_BEARER)
        {
          cellular_data_avail = TRUE;
          break;
        }
      }
    }
  }

  *addi_res_ptr = (uint8)GSTK_NO_SERVICE_AVAILABLE;
  gstk_status = gstk_io_ctrl(GSTK_IO_GET_ACTIVE_DATA_SUB_SLOT_ID,
                             (void *)&slot,
                             (void *)&active_slot);

  if(gstk_status == GSTK_SUCCESS)
  {
    /* If the BIP transaction is requested on the slot 
       with the active DATA subscription, BIP over
       Cellular can be allowed */
    if(slot == active_slot)
    {
       if(estk_bip_check_existing_gsm_cs_call())
       {
          *addi_res_ptr = (uint8)GSTK_ME_CURRENTLY_BUSY_ON_CALL;
       }
       else
       {
          if(cellular_data_avail == TRUE)
          {
            cellular_allowed = TRUE;
          }
       }
    }
    else
    {
       *addi_res_ptr = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    }
  }

  UIM_MSG_HIGH_3("WLAN available: 0x%x, Cellular data available: 0x%x, WLAN unusable: 0x%x",
                 wlan_avail, cellular_data_avail, estk_shared_info.wlan_unusable);

  if (open_ch_cmd != NULL && open_ch_cmd->dns_requested)
  {
    if (cellular_allowed == TRUE)
    {
      *nw_type_ptr = ESTK_NETWORK_CELLULAR;
      return ESTK_SUCCESS;
    }
    else
    {
      return ESTK_ERROR;
    }
  }

  /* Check if WLAN can be used for the BIP Session */
  if((wlan_avail == TRUE) &&
     (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_ESTK_BIP_OVER_WLAN,
        estk_curr_inst_ptr->slot_id) == TRUE) &&
     (bearer_type == GSTK_BEARER_DEFAULT) &&
     (address_type != GSTK_PKT_DATA_IPV6_ADDRESS))
  {
    wlan_allowed = TRUE;
  }

  UIM_MSG_HIGH_2("WLAN allowed for BIP: 0x%x, Cellular allowed for BIP: 0x%x",
                 wlan_allowed, cellular_allowed);

  /* If WLAN has higher priority over Cellular, try to use WLAN */
  if(gstk_nv_get_feature_status(
       GSTK_CFG_FEATURE_SLOT_ESTK_BIP_WLAN_PRIO,
       estk_curr_inst_ptr->slot_id) == TRUE)
  {
    /* If WLAN is allowed for this OPEN CHANNEL request and WLAN is marked
       usable, use WLAN */
    if((wlan_allowed == TRUE) && (estk_shared_info.wlan_unusable == FALSE))
    {
      *nw_type_ptr = ESTK_NETWORK_WLAN;
    }
    /* If Cellular is in normal service and OPEN CHANNEL is requested on active
       DATA slot, use Cellular */
    else if(cellular_allowed == TRUE)
    {
      *nw_type_ptr = ESTK_NETWORK_CELLULAR;
    }
    /* Ignore WLAN unusable status if WLAN is the only bearer available */
    else if((wlan_allowed == TRUE) && (estk_shared_info.wlan_unusable == TRUE))
    {
      *nw_type_ptr = ESTK_NETWORK_WLAN;
    }
    else
    {
      return ESTK_ERROR;
    }
  }
  /* If Cellular has higher priority over WLAN, use it if in Normal Service */
  else
  {
    if(cellular_allowed == TRUE)
    {
      *nw_type_ptr = ESTK_NETWORK_CELLULAR;
    }
    else if(wlan_allowed == TRUE)
    {
      *nw_type_ptr = ESTK_NETWORK_WLAN;
    }
    else
    {
      return ESTK_ERROR;
    }
  }

  UIM_MSG_HIGH_1("Chosen network for BIP: 0x%x", *nw_type_ptr);

  return ESTK_SUCCESS;
} /* estk_bip_get_prio_network */

/*===========================================================================

FUNCTION: estk_save_prev_open_ch

DESCRIPTION :
  This function saves the current OPEN CH cmd so the channel might be reused
  by the card if needed.

PARAMETERS:
  gstk_req_ptr :   [Input] Original OPEN CH cmd

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_save_prev_open_ch(const gstk_cmd_from_card_type *gstk_req_ptr)
{
  if (gstk_req_ptr == NULL)
  {
    return;
  }

  /* Save current OPEN CH for future comparison */
  /*--------------------------------------------------------------------------*/

  /* Bearer TLV */
 (void)memscpy(&(estk_curr_inst_ptr->prev_open_ch.bearer),
               sizeof(gstk_bearer_description_type),
               &(gstk_req_ptr->cmd.open_ch_pro_cmd_req.bearer),
               sizeof(gstk_bearer_description_type));

  /* APN TLV */

  if (estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet)
  {
    gstk_free(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet);
    estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet = NULL;
    estk_curr_inst_ptr->prev_open_ch.access_point_name.length = 0;
  }

  estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet_given
    = gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given;

  estk_curr_inst_ptr->prev_open_ch.access_point_name.length
    = gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given &&
     gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length != 0)
  {
    estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet
      = (uint16 *)GSTK_CALLOC(((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length) * sizeof(uint16));
    if(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet == NULL)
    {
      return;
    }
    else
    {
      (void)memscpy(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16),
                    gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16));
    }
  }

  /* Transport Level TLV */
  (void)memscpy(&(estk_curr_inst_ptr->prev_open_ch.transport_level),
                sizeof(gstk_pkt_data_transport_level_type),
                &(gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level),
                sizeof(gstk_pkt_data_transport_level_type));

  /* Data Destination Address TLV */
  estk_curr_inst_ptr->prev_open_ch.data_dest_address.address_type =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.address_type;

  estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length > 0)
  {
    if (estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address)
    {
      gstk_free(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address);
      estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address = NULL;
    }
    estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address
      = (uint8 *)GSTK_CALLOC((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length);
    if(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address == NULL)
    {
      if(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet)
      {
        gstk_free(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet);
        estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet = NULL;
      }
      return;
    }
    else
    {
      (void)memscpy(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length,
                    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length * sizeof(uint8));
    }
  }
} /* estk_save_prev_open_ch */

/*===========================================================================

FUNCTION: estk_populate_open_ch_table

DESCRIPTION :
  This function populates information into the Open channel Table.

PARAMETERS:
  index :   [Input] Index of the channel to be populated
  nw_type : [Input] Contains the network on which Channel is being opened

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_populate_open_ch_table(
  int32             index,
  estk_network_type nw_type)
{
  gstk_open_ch_req_type   *open_ch_cmd        = NULL;
  uint32                   i                  = 0;
  boolean                  in_fallback        = FALSE;
  uint16                   address_family     = DSS_AF_UNSPEC;
  struct ps_sockaddr_in    *p_dest_addr_v4    = NULL;
  boolean                  ch_reuse           = FALSE;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_2("estk_populate_open_ch_table: index=0x%x, slot=0x%x",
                 index, estk_curr_inst_ptr->slot_id);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_OPEN_CH_REQ &&
     estk_curr_inst_ptr->keep_ch_open &&
     estk_curr_inst_ptr->ch_open_id != -1 &&
     index == estk_curr_inst_ptr->ch_open_id)
  {
    UIM_MSG_HIGH_0("estk_populate_open_ch_table: Attempting CHANNEL reuse");
    if (estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.access_point_name.length == 0 ||
        (estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet == NULL) ||
        (estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.access_point_name.length !=
         estk_curr_inst_ptr->prev_open_ch.access_point_name.length) ||
        (memcmp((uint8*)estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.access_point_name.alphabet,
         (uint8*)estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet,
         estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16)) != 0) ||
        (memcmp(&estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
         &estk_curr_inst_ptr->prev_open_ch.bearer,
         sizeof(gstk_bearer_description_type)) != 0))
    {
      UIM_MSG_HIGH_0("estk_populate_open_ch_table: CHANNEL reuse failed - APN/bearer is different");
      (void)estk_bip_clean_up_ch_table(index);
    }
    else
    {
      if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p != NULL)
      {
        UIM_MSG_HIGH_0("estk_populate_open_ch_table: CHANNEL reuse - clear timer");
        (void)rex_clr_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
        ch_reuse = TRUE;
      }
    }
  }

  /* Clean-up channel table if table is not being populated as part of
     Channel Reuse */
  if(!ch_reuse)
  {
    estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp = -1;
    estk_curr_inst_ptr->bip_channel_table[index].profile_3gpp2 = -1;
    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle = DSS_ERROR;
    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v4 = DSS_ERROR;
    estk_curr_inst_ptr->bip_channel_table[index].sock_nethandle_v6 = DSS_ERROR;
    estk_curr_inst_ptr->bip_channel_table[index].sock_fd = DSS_ERROR;
    estk_curr_inst_ptr->bip_channel_table[index].iface_id = DSS_IFACE_INVALID_ID;
    estk_bip_clean_up_ch_table(index);
  }

  /* If fallback is in progress, retrieve the cached Open Channel command */
  if(estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_FALLBACK_IN_PROGRESS)
  {
    UIM_MSG_HIGH_0("estk_populate_open_ch_table: ESTK_CH_FALLBACK_IN_PROGRESS");
    open_ch_cmd = &estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd;
    in_fallback = TRUE;
  }
  else
  {
    open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

    estk_open_ch_cleanup(&estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd);
    /* Save the Open Channel command for later use (in fallback) */
    (void)estk_save_open_channel_cmd_details(
      &estk_curr_inst_ptr->curr_cmd,
      &estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd);

    /* Save the Open Channel command for possible channel reuse */
    (void)estk_save_prev_open_ch(&estk_curr_inst_ptr->curr_cmd);
  }

  if(!ch_reuse)
  {
    UIM_MSG_HIGH_0("estk_populate_open_ch_table: Initialize CHANNEL data...");

    estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;
    estk_curr_inst_ptr->bip_channel_table[index].nw_type = nw_type;
    estk_curr_inst_ptr->curr_bip_cmd_index = (int8)index;
    estk_curr_inst_ptr->bip_channel_table[index].channel_id = (index + 1);
  }

  /* Cache the currently ongoing proactive command - only done for OPEN CHANNEL,
     as for rest of BIP Proactive commands, the chaching happens elsewhere */
  if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_OPEN_CH_REQ)
  {
    estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_OPEN_CH_REQ;
  }

  /* Initialize or update Channel parameters */
  estk_curr_inst_ptr->bip_channel_table[index].transport_protocol =
    open_ch_cmd->transport_level.transport_protocol;
  estk_curr_inst_ptr->bip_channel_table[index].port_number =
    uint32touint16(open_ch_cmd->transport_level.port_number);
  estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params = FALSE;

  memset(&(estk_curr_inst_ptr->bip_channel_table[index].policy_info), 0,
         sizeof(estk_curr_inst_ptr->bip_channel_table[index].policy_info));

  /* Handle buffer size and if the command parameters are modified */
  if(open_ch_cmd->buffer_size <= (int32)ESTK_BIP_MAX_BUFFER_SIZE)
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = open_ch_cmd->buffer_size;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = FALSE;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = ESTK_BIP_MAX_BUFFER_SIZE;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = TRUE;
  }

  /* Handle On demand Link Establishment*/
  estk_curr_inst_ptr->bip_channel_table[index].on_demand_link = open_ch_cmd->on_demand_link_est;

  /* Handle Bearer type and copy parameters */
  (void)gstk_memcpy(&estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
                 &open_ch_cmd->bearer,
                 sizeof(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc),
                 sizeof(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc),
                 sizeof(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc));

  /* Handle memory allocation for the receive buffer */
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].recv_buffer = (uint8 *)GSTK_CALLOC(
    int32touint32(estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].send_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buffer = (uint8 *)GSTK_CALLOC(
    int32touint32(estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;

  /* Local Address */
  if(estk_populate_address_in_open_ch_table(index, ESTK_LOCAL_ADDRESS,
       &(open_ch_cmd->local_address)) != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Saving Local IP address info failed");
    return ESTK_ERROR;
  }

  /* Cellular network access related info need not be parsed for WLAN */
  if(nw_type != ESTK_NETWORK_WLAN)
  {
    /* APN setting */
    if(open_ch_cmd->access_point_name.alphabet_given == TRUE &&
       open_ch_cmd->access_point_name.length != 0)
    {
      UIM_MSG_HIGH_0("APN present in open channel command");
      if(open_ch_cmd->access_point_name.length > 0)
      {
        estk_curr_inst_ptr->bip_channel_table[index].apn_len =
          open_ch_cmd->access_point_name.length;
        /* If NULL terminated remove */
        if(open_ch_cmd->access_point_name.alphabet[open_ch_cmd->access_point_name.length - 1] == 0)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn_len--;
        }
        if (estk_curr_inst_ptr->bip_channel_table[index].apn)
        {
          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
          estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
        }
        estk_curr_inst_ptr->bip_channel_table[index].apn = (uint8 *)GSTK_CALLOC(
          (estk_curr_inst_ptr->bip_channel_table[index].apn_len) * sizeof(uint8));
        if(estk_curr_inst_ptr->bip_channel_table[index].apn == NULL)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
          return ESTK_ERROR;
        }
        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++i)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn[i] =
            uint16touint8(open_ch_cmd->access_point_name.alphabet[i]);
          UIM_MSG_HIGH_2("APN[%d] = 0x%x",
                         i, estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
        }

        if (estk_modify_apn_based_on_class(index) != ESTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Failed to modify APN!");
        }
        if (estk_modify_apn_based_on_mapping_table(index) != ESTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Failed to modify APN based on mapping table!");
        }
      }
    }

    /* login name */
    if(open_ch_cmd->text_string_user_login.alphabet_given &&
       open_ch_cmd->text_string_user_login.length != 0)
    {
      UIM_MSG_HIGH_0("Save login name");
      estk_curr_inst_ptr->bip_channel_table[index].username_len =
          int32touint32(open_ch_cmd->text_string_user_login.length);
      estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params = TRUE;

      /* If NULL terminated remove */
      if(open_ch_cmd->text_string_user_login.alphabet[open_ch_cmd->text_string_user_login.length - 1] == 0)
      {
        estk_curr_inst_ptr->bip_channel_table[index].username_len--;
      }
      estk_curr_inst_ptr->bip_channel_table[index].username =
        (char *)GSTK_CALLOC(estk_curr_inst_ptr->bip_channel_table[index].username_len);
      if (estk_curr_inst_ptr->bip_channel_table[index].username)
      {
        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].username_len; ++i)
        {
          estk_curr_inst_ptr->bip_channel_table[index].username[i] =
            uint16touint8(open_ch_cmd->text_string_user_login.alphabet[i]);
          UIM_MSG_HIGH_2("UserLogin[%d] = 0x%x",
                         i, estk_curr_inst_ptr->bip_channel_table[index].username[i]);
        }
      }
      else
      {
        return ESTK_ERROR;
      }
    }

    /* password */
    if(open_ch_cmd->text_string_user_passwd.alphabet_given &&
       open_ch_cmd->text_string_user_passwd.length != 0)
    {
      UIM_MSG_HIGH_0("Save password");
      estk_curr_inst_ptr->bip_channel_table[index].passwd_len =
          int32touint32(open_ch_cmd->text_string_user_passwd.length);
      estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params = TRUE;

      if(open_ch_cmd->text_string_user_passwd.alphabet[open_ch_cmd->text_string_user_passwd.length - 1] == 0)
      {
        estk_curr_inst_ptr->bip_channel_table[index].passwd_len--;
      }
      estk_curr_inst_ptr->bip_channel_table[index].passwd =
        (char *)GSTK_CALLOC(estk_curr_inst_ptr->bip_channel_table[index].passwd_len);
      if (estk_curr_inst_ptr->bip_channel_table[index].passwd)
      {
        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].passwd_len; ++ i)
        {
          estk_curr_inst_ptr->bip_channel_table[index].passwd[i] =
            uint16touint8(open_ch_cmd->text_string_user_passwd.alphabet[i]);
        }
      }
      else
      {
        return ESTK_ERROR;
      }
    }
  }

  /* Data Destination address */
  if(estk_populate_address_in_open_ch_table(index, ESTK_DESTINATION_ADDRESS,
       &(open_ch_cmd->data_dest_address)) != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Saving Destination IP address info failed");
    return ESTK_ERROR;
  }

  /* Attempt immediate link establishment if card has requested it or fallback
     is being attempted */
  if((open_ch_cmd->on_demand_link_est == TRUE) &&
     (in_fallback == FALSE))
  {
    UIM_MSG_HIGH_0("On demand link establishment");
    return ESTK_SUCCESS;
  }
  else
  {
    p_dest_addr_v4 =
      (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
    if (estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.dns_requested)
    {
      address_family = DSS_AF_INET;
    }
    else if (p_dest_addr_v4 != NULL)
    {
      address_family = p_dest_addr_v4->ps_sin_family;
    }

    if(estk_bip_establish_link(index, address_family) == ESTK_ERROR)
    {
      UIM_MSG_HIGH_0("Establish link failed");
      estk_bip_clean_up_ch_table(index);
      return ESTK_ERROR;
    }
    else
    {
      UIM_MSG_HIGH_0("BIP link successfully established");
      return ESTK_SUCCESS;
    }
  }
} /* estk_populate_open_ch_table */

/*===========================================================================

FUNCTION: estk_bip_check_existing_gsm_cs_call

DESCRIPTION :
  This function checks for the CS call existence in case of GSM RAT.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE: Voice call is already established connected
  FALSE: No CS Call.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static boolean estk_bip_check_existing_gsm_cs_call(void)
{
  gstk_slot_id_enum_type       slot             = GSTK_SLOT_ID_MAX;
  gstk_status_enum_type        gstk_status      = GSTK_SUCCESS;
  boolean                      cs_ps_parallel   = FALSE;
  boolean                      dtm_feat_support = FALSE;
  gstk_nv_items_data_type      nv_data;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, FALSE); 
  slot = estk_curr_inst_ptr->slot_id;

  if(!ESTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("Invalid slot %d", slot);
    return FALSE;
  }
  (void)gstk_nv_access_read(GSTK_NV_SLOT_TOOLKIT_CS_PS_PARALLEL_I,
                            slot,
                            &nv_data);

  cs_ps_parallel = nv_data.cs_ps_parallel;

  (void)gstk_nv_access_read(GSTK_NV_EXT_SUB_DTM_FEATURE_SUPPORT_I,
                            slot,
                            &nv_data);

  dtm_feat_support = nv_data.dtm_feat_support;

  gstk_status = gstk_io_ctrl(GSTK_IO_GET_CURR_ACCESS_TECH,
                             (void *)&slot,
                             (void *)&estk_curr_access_tech);
  if(gstk_status == GSTK_SUCCESS)
  {
    /* Only in case of GSM Access Technology estk checks for the CS and PS Parllel Call Support
       Where as in case of UMTS and LTE by default the Parllel Calls are possible,
       therefore !cs_ps_parallel_call should be checked only in case of GSM Technology */
    if((estk_curr_access_tech == GSTK_ACCESS_TECH_GSM) &&
       (!cs_ps_parallel || !dtm_feat_support)
        /*If there is any existing CS Call, then send TR with unable to Process the Command */
       && (estk_instances_ptr[(uint8)slot - 1]->estk_cm_cs_call_connected != 0))
    {
      UIM_MSG_HIGH_0("GSM CS Call is in progress");
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION: estk_scws_populate_open_ch_table

DESCRIPTION :
  This function populates information into the Open channel Table.

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_populate_open_ch_table(int32 index)
{
  gstk_open_ch_req_type                    *open_ch_cmd = NULL;
  uint32                                   user_data   = 0;
  gstk_cmd_from_card_type                  gstk_scws_cmd;
  gstk_client_id_type                      client_id   = 0;
  gstk_status_enum_type                    gstk_status = GSTK_SUCCESS;
  int                                      i           = 0;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type call_cli_evt_cb;

  UIM_MSG_HIGH_1("estk_scws_populate_open_ch_table: index=0x%x", index);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));
  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_OPEN_CH_REQ;
  estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;
  estk_curr_inst_ptr->bip_channel_table[index].channel_id = (index + 1);
  estk_curr_inst_ptr->bip_channel_table[index].transport_protocol =
    open_ch_cmd->transport_level.transport_protocol;
  estk_curr_inst_ptr->bip_channel_table[index].port_number =
    uint32touint16(open_ch_cmd->transport_level.port_number);
  if(open_ch_cmd->transport_level.transport_protocol
     == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
  {
     estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch = TRUE;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch = FALSE;
  }
  /* Handle buffer size and if the command parameters are modified */
  if(open_ch_cmd->buffer_size <= (int32)ESTK_BIP_MAX_BUFFER_SIZE)
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = open_ch_cmd->buffer_size;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = FALSE;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = ESTK_BIP_MAX_BUFFER_SIZE;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = TRUE;
  }

  /* Handle memory allocation for the receive buffer */
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].recv_buffer = (uint8 *)GSTK_CALLOC(
    int32touint32(estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].send_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buffer = (uint8 *)GSTK_CALLOC(
    int32touint32(estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;

  estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed = TRUE;

  find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
  find_cli_reg.cmd_type = GSTK_SCWS_OPEN_CH_REQ;
  find_cli_reg.format_type = GSTK_GSTK_FORMAT;
  gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    return ESTK_ERROR;
  }
  else
  {
    call_cli_evt_cb.client_id = client_id;
    call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
    gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_OPEN_CH_REQ;
    gstk_scws_cmd.hdr_cmd.cmd_detail_reference = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
    gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
    gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
    gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.ch_id       = (uint32)(index+1);
    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.buffer_size = estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.port_number = estk_curr_inst_ptr->bip_channel_table[index].port_number;
    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet_given = open_ch_cmd->alpha.alphabet_given;
    estk_curr_inst_ptr->curr_bip_cmd_index = (int8)index;

    if(open_ch_cmd->alpha.length > 0)
    {
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length = int32touint8(open_ch_cmd->alpha.length);
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet =
        (uint16 *)GSTK_CALLOC(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length * 2 );
      if(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet == NULL)
      {
        gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length = 0;
      }
      else
      {
        for(i=0; i < gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length; i++)
        {
          gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet[i] = uint16touint8(open_ch_cmd->alpha.alphabet[i]);
          UIM_MSG_HIGH_2("BIP alpha 8 bit = 0x%x , 16 bit = 0x%x",
                         gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet[i],
                         open_ch_cmd->alpha.alphabet[i]);
        }
      }
    }

    if (gstk_io_ctrl(
            GSTK_IO_CALL_CLIENT_EVT_CB,
            (void *)&call_cli_evt_cb,
            (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("can't call cli evt cb!");
      return ESTK_ERROR;
    }
    if(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet != NULL)
    {
      gstk_free(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet);
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet = NULL;
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length = 0;
    }
  }
  return ESTK_SUCCESS;
} /* estk_scws_populate_open_ch_table */

/*===========================================================================

FUNCTION: estk_process_bip_user_cnf_rsp

DESCRIPTION :
  This function will be called by ESTK to process the user confirmation response
  received from the UI client.If the user confirmed for an affirmative action,
  the call will be set up, otherwise an appropriate Terminal Response will be
  sent to the card saying "The user rejected the call setup".
  If the user fails to respond within a certain time, a Terminal response of
  "No response from the user will be sent to the card.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_bip_user_cnf_rsp(
  const estk_cmd_type *cmd_ptr
)
{
  uint32                              cmd_ref_id       = 0;
  uint32                              type_of_command  = 0;
  boolean                             send_tr          = FALSE;
  gstk_general_result_enum_type       gen_result       = GSTK_GENERAL_RESULT_MAX;
  int32                               index            = -1;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res         = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  gstk_open_ch_req_type              *open_ch_cmd      = NULL;
  gstk_slot_id_enum_type              slot             = GSTK_SLOT_ID_MAX;
  estk_network_type                   nw_type          = ESTK_NETWORK_MAX;
  estk_result_enum_type               estk_status      = ESTK_SUCCESS;

  if(cmd_ptr == NULL)
  {
    /*Either cmd_ptr is NULL or Ignoring Second User confirmation*/
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  if(rex_get_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer)) > 0)
  {
    UIM_MSG_HIGH_0("Clear user_or_disp_conf_timer on alpha response");
    (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));
  }

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

   /* Step 1: Check input parameters */
  cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;
  if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND", cmd_ref_id);
    return ESTK_ERROR;
  }

  type_of_command =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id)].command_details.type_of_command;
  if(type_of_command != GSTK_CMD_STK_OPEN_CHANNEL)
  {
    UIM_MSG_ERR_1("Incorrect command type 0x%x", type_of_command);
    return ESTK_ERROR;
  }

  if(cmd_ptr->hdr.cmd != ESTK_GENERAL_USER_CNF_CMD)
  {
    UIM_MSG_ERR_1("Incorrect command id 0x%x", cmd_ptr->hdr.cmd);
    return ESTK_ERROR;
  }

  additional_info.length = 0;

  UIM_MSG_HIGH_1("Icon disp is 0x%x", cmd_ptr->cmd_data.user_rsp.icon_disp);
  estk_curr_inst_ptr->icon_disp = cmd_ptr->cmd_data.user_rsp.icon_disp;

  /* Check user response. If User said "No", send TR and do not originate call */
  if(cmd_ptr->cmd_data.user_rsp.yes_no_rsp == 0)
  {
    UIM_MSG_HIGH_0("User did not accept open channel request");
    gen_result = GSTK_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST;
    send_tr    = TRUE;
  }
  else /* If user responded with a yes */
  {
    additional_info.length = 1;
    if(estk_find_ch_socket_index(&index, ESTK_CH_CLOSED) != ESTK_SUCCESS)
    {
      /* no more room */
      UIM_MSG_ERR_0("No more Room for Open Ch");
      gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      send_tr    = TRUE;
      addi_res   = (uint8)GSTK_CH_UNAVAILABLE;
      additional_info.additional_info_ptr = &addi_res;
    }
    else
    {
      UIM_MSG_HIGH_1("Index chosen for open channel %d", index );
      if(open_ch_cmd->transport_level.transport_protocol
        == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
      {
        if (estk_scws_populate_open_ch_table(index) != ESTK_SUCCESS)
        {
          UIM_MSG_ERR_0("estk_scws_populate_open_ch_table failed");
          gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
          send_tr    = TRUE;
          addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          additional_info.additional_info_ptr = &addi_res;
        }
        else
        {
          send_tr = FALSE;
        }
      }
      else
      {
        /* If BIP over WLAN feature is not enabled or it does not have higher
           higher priority over Cellular, execute logic specific to Cellular networks */
        slot = estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;

        if(!ESTK_IS_VALID_SLOT_ID(slot))
        {
          UIM_MSG_HIGH_1("Invalid slot %d", slot );
          return ESTK_ERROR;
        }

        estk_status = estk_bip_get_prio_network(
                        &nw_type,
                        &addi_res,
                        slot,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer.bearer_type,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.data_dest_address.address_type);

        if(estk_status != ESTK_SUCCESS)
        {
          gen_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          send_tr    = TRUE;
          additional_info.additional_info_ptr = &addi_res;
        }

        /* Continue processing the Open Channel command if no error has yet
           been flagged */
        if(send_tr == FALSE)
        {
          if(estk_populate_open_ch_table(index, nw_type) != ESTK_SUCCESS)
          {
            UIM_MSG_ERR_0("estk_populate_open_ch_table failed");
            gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
            send_tr    = TRUE;
            addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            additional_info.additional_info_ptr = &addi_res;
          }
          else
          {
            if(open_ch_cmd->on_demand_link_est)
            {
              UIM_MSG_HIGH_1("Sending TR because on demand link is 0x%x",
                             open_ch_cmd->on_demand_link_est);
              send_tr = TRUE;
              gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
              additional_info.length = 0;
              additional_info.additional_info_ptr = NULL;
              tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
                estk_curr_inst_ptr->bip_channel_table[index].channel_id;
            }
            else
            {
              UIM_MSG_HIGH_0("Populating open channel table succeeded");
              return ESTK_SUCCESS;
            }
          }
        }
      }
    }
  }

  if(send_tr == TRUE)
  {
    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
           sizeof(gstk_bearer_description_type),
           &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
           sizeof(gstk_bearer_description_type));

    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      open_ch_cmd->buffer_size;

    /* If the Open Ch is SCWS type, set the is_scws_ch to TRUE so that the correct TR is sent */
    if(open_ch_cmd->transport_level.transport_protocol
       == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
    {
      tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
    }

    if (gen_result != GSTK_COMMAND_PERFORMED_SUCCESSFULLY && index != -1)
    {
      estk_bip_clean_up_ch_table(index);
    }

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             gen_result,
             &additional_info,
             &tr_extra_param,
             index);
  }

  return ESTK_ERROR;
} /* estk_process_bip_user_cnf_rsp */

/*===========================================================================
FUNCTION: estk_process_open_ch_req

DESCRIPTION:
  Process the OPEN CHANNEL Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk OPEN CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_open_ch_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  gstk_client_id_type                 client_id            = 0;
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  uint32                              cmd_ref_id           = 0;
  gstk_additional_info_ptr_type       additional_info      = {0};
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_slot_id_enum_type              slot                 = GSTK_SLOT_ID_MAX;
  estk_network_type                   nw_type              = ESTK_NETWORK_MAX;
  uint8                               addi_res             = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  estk_result_enum_type               estk_status          = ESTK_SUCCESS;
  gstk_general_result_enum_type       gen_result           = GSTK_GENERAL_RESULT_MAX;

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_process_open_ch_req, slot: 0x%x, Transport Protocol: 0x%x",
                 gstk_req_ptr->hdr_cmd.sim_slot_id,
                 gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol);

  slot = gstk_req_ptr->hdr_cmd.sim_slot_id;
  if (!ESTK_IS_VALID_SLOT_ID(slot))
  {
    return ESTK_ERROR;
  }

  memset((void*)&tr_extra_param, 0x00, sizeof(tr_extra_param));
  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  additional_info.length = 0;
  additional_info.additional_info_ptr = NULL;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
  {
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                  sizeof(gstk_bearer_description_type),
                  &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
                  sizeof(gstk_bearer_description_type));

    estk_status = estk_bip_get_prio_network(
                        &nw_type,
                        &addi_res,
                        slot,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer.bearer_type,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.data_dest_address.address_type);

    if(estk_status != ESTK_SUCCESS)
    {
      gen_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      additional_info.length = 1;
      additional_info.additional_info_ptr = &addi_res;
      return estk_send_terminal_response(cmd_ref_id,
                                  GSTK_OPEN_CH_CNF,
                                  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                  &additional_info,
                                  &tr_extra_param);
    }
  }

  /* Check transport layer protocol is TCP or UDP.
     If DNS requested, protocol would be missing */
  if(((gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_UDP) &&
      (gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_TCP)
      && (gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
      ) && !gstk_req_ptr->cmd.open_ch_pro_cmd_req.dns_requested)
  {
    UIM_MSG_ERR_0("Transport protocol Not Supported");

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             &tr_extra_param,
             ESTK_MAX_BIP_CH);
  }
  else
  {
    estk_curr_inst_ptr->state = ESTK_WAIT_FOR_USER_CNF_ST;
    client_id = estk_curr_inst_ptr->gstk_client_id;
    if ((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == TRUE)
       || (gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
       == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
       )
    {
      gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(client_id, cmd_ref_id, TRUE, 0);
      if(gstk_status == GSTK_SUCCESS)
      {
        return ESTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_0("gstk_send_user_cnf_alpha_and_icon_disp_rsp failed");
        return ESTK_ERROR;
      }
    }
    /* Check if confirmation alpha is present*/
    else if(((gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length) > 0 &&
            (gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.alphabet)) ||
            (gstk_nv_get_feature_status(
               GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
               GSTK_SLOT_ID_MAX) == TRUE))
    {
      UIM_MSG_HIGH_1("ALPHA TLV length: 0x%x",
                      gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length);
      return estk_send_alpha_to_display_client(gstk_req_ptr, FALSE);
    }
    else /* no alpha or null alpha */
    {
      gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(client_id, cmd_ref_id, TRUE, 0);
      if(gstk_status == GSTK_SUCCESS)
      {
        return ESTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_0("gstk_send_user_cnf_alpha_and_icon_disp_rsp failed");
        return ESTK_ERROR;
      }
    }
  }
} /* estk_process_open_ch_req */

/*===========================================================================
FUNCTION: estk_convert_ch_id_to_index

DESCRIPTION:
  Find out index in bip_channel_table based on channel id

PARAMETERS:
  ch_id :   [Input] Channel id whose index has to found
  index :   [Input] Result that contains the mapped index

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_convert_ch_id_to_index (
  int32                   ch_id,
  int32                  *index
)
{
  uint32                i       = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(index == NULL)
  {
    return ESTK_ERROR;
  }
  for(i=0; i<ESTK_MAX_BIP_CH; i++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[i].channel_id == ch_id)
    {
      *index = estk_curr_inst_ptr->bip_channel_table[i].index;
      return ESTK_SUCCESS;
    }
  }
  return ESTK_ERROR;
} /* estk_convert_ch_id_to_index */

/*===========================================================================
FUNCTION: estk_process_send_data_req

DESCRIPTION:
  Process the Send Data Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk SEND DATA cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_data_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  int32                               index;
  int32                               ch_id;
  estk_result_enum_type               estk_status     = ESTK_SUCCESS;
  uint32                              cmd_ref_id      = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res;
  gstk_raw_cmd_from_card_type         raw_cmd;

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&raw_cmd, 0x00, sizeof(gstk_raw_cmd_from_card_type));
  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;
  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  ch_id = gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_id;

  UIM_MSG_HIGH_2("estk_process_send_data_req: cmd_detail_reference: 0x%x, ch_id: 0x%x",
                 cmd_ref_id, ch_id);

  estk_status = estk_convert_ch_id_to_index(ch_id, &index);

  if(estk_status != ESTK_SUCCESS)
  {
    addi_res = (uint8)GSTK_CH_ID_INVALID;
    UIM_MSG_ERR_0("estk_convert_ch_id_to_index failed");
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             ESTK_MAX_BIP_CH);
  }

  /* Cache the currently ongoing proactive command */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_SEND_DATA_REQ;

  /* Check channel status if PENDING or ACTIVE */
  if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_PENDING
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_OPEN_WAIT
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_SCWS_LISTEN
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_SCWS_ESTABLISHED
  )
  {
    UIM_MSG_ERR_1("Channel status for send data is wrong for send data %d",
                  estk_curr_inst_ptr->bip_channel_table[index].status);

    addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  /* Send data timer will expire when socket write fails or Link
     Establishment (on demand link) fails */
  if(estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired)
  {
    UIM_MSG_HIGH_2("Send Data timer has expired previously, Send Buffer len: %d, Send Data Command data len: %d",
                   estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                   gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);

    /* Card is resending only the failed SEND DATA request */
    if((gstk_req_ptr->cmd.send_data_pro_cmd_req.send_data_immediately) &&
       (estk_curr_inst_ptr->bip_channel_table[index].send_buf_len >=
        uint32touint16(gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len)))
    {
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len
        -= uint32touint16(gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);
    }
    /* Card is resending SEND DATA from first */
    else
    {
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    }
    estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;
  }

  /* Check if Send Buffer is valid and there is space in buffer */
  if((estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL) ||
     ((estk_curr_inst_ptr->bip_channel_table[index].send_buf_len
       + gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len) >
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size))
  {
    UIM_MSG_ERR_3("Send Buffer 0x%x, Send buffer len %d, Send Data buffer len %d",
                  estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                  gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);

    addi_res = (uint8)GSTK_BUFFER_SIZE_UNAVAILABLE;
    additional_info.additional_info_ptr = &addi_res;

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  if(gstk_memcpy((void *)(estk_curr_inst_ptr->bip_channel_table[index].send_buffer +
                          estk_curr_inst_ptr->bip_channel_table[index].send_buf_len),
                 (void *)gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data,
                 gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len,
                 estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
                 estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                 gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len) <
     (size_t)gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len)
  {
    UIM_MSG_ERR_0("estk_process_send_data_req(): gstk_memcpy failed");
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(cmd_ref_id,
                                           GSTK_SEND_DATA_CNF,
                                           GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                           &additional_info,
                                           NULL,
                                           index);
  }

  /* Check if display alpha is present*/
  if(((gstk_req_ptr->cmd.send_data_pro_cmd_req.alpha.length) > 0 &&
      (gstk_req_ptr->cmd.send_data_pro_cmd_req.alpha.alphabet)) ||
     (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
        GSTK_SLOT_ID_MAX) == TRUE))
  {
    UIM_MSG_HIGH_0("ALPHA TLV present in Send Data command"  );
    if(gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA,
         GSTK_SLOT_ID_MAX) == FALSE)
    {
      estk_status = estk_send_alpha_to_display_client(gstk_req_ptr, TRUE);
      if(estk_status != ESTK_SUCCESS)
      {
        return ESTK_ERROR;
      }
    }
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len
    += uint32touint16(gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);

  if(estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING)
  {
    struct ps_sockaddr_in               *p_dest_addr_v4 = NULL;
    
    UIM_MSG_HIGH_0("Send Data Channel status is pending");

    p_dest_addr_v4 =
      (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
    
    if (p_dest_addr_v4 == NULL)
    {
      UIM_MSG_ERR_0("estk_process_send_data_req: invalid dest addr!");
      addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      return estk_bip_send_terminal_response(
               cmd_ref_id,
               GSTK_SEND_DATA_CNF,
               GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
               &additional_info,
               NULL,
               index);
    }
    
    estk_status = estk_bip_establish_link(index, p_dest_addr_v4->ps_sin_family);
    if(estk_status != ESTK_SUCCESS)
    {
      addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving SEND DATA TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
        estk_cache_additional_info(&additional_info);
        estk_status = ESTK_SUCCESS;
      }
      else
      {
      estk_status = estk_bip_send_terminal_response(
                      cmd_ref_id,
                      GSTK_SEND_DATA_CNF,
                      GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                      &additional_info,
                      NULL,
                      index);
      }
      if(estk_status == ESTK_SUCCESS)
      {
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSED;
        /* Send Event Download envelope command */
        return estk_bip_send_ch_status_event(
                 estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                 GSTK_CH_STATUS_LINK_DROPPED);
      }
      else
      {
        UIM_MSG_HIGH_0("Failed on sending terminal response for send data");
        return ESTK_ERROR;
      }
    }
  }
  else if((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_ACTIVE)
         || (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_SCWS_ESTABLISHED)
         || (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_SCWS_LISTEN))
  {
    UIM_MSG_HIGH_1("Send Data channel status active: send_data_immediately=%x",
                   gstk_req_ptr->cmd.send_data_pro_cmd_req.send_data_immediately);

    if(gstk_req_ptr->cmd.send_data_pro_cmd_req.send_data_immediately)
    {
      if (estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch)
      {
        return estk_scws_send_data(index, cmd_ref_id);
      }
      else
      {
         return estk_bip_write_sock(index, cmd_ref_id);
      }
    }
    else
    {
      /* Success terminal response with num bytes left */
      tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
      tr_extra_param.present = TRUE;
      tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
        estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
        estk_curr_inst_ptr->bip_channel_table[index].send_buf_len;
      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
          UIM_MSG_HIGH_0("Saving SEND DATA TR");
          estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
          estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
          estk_curr_inst_ptr->extra_param = tr_extra_param;
          estk_status = ESTK_SUCCESS;
      }
      else
      {
        estk_status = estk_bip_send_terminal_response(
                        cmd_ref_id,
                        GSTK_SEND_DATA_CNF,
                        GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                        NULL,
                        &tr_extra_param,
                        index);
      }
    }
  }
  else
  {
    UIM_MSG_ERR_0("Send Data Channel status neither Pending nor Active");
    if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL)
    {
      UIM_MSG_HIGH_0("Freeing the SCWS overflow buffer");
      gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = 0;
    }
    if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
    {
      UIM_MSG_HIGH_0("Setting the send buffer to 0");
      memset(estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
             0x00,
             estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    }
    return ESTK_ERROR;
  }
  return estk_status;
} /* estk_process_send_data_req */

/*===========================================================================
FUNCTION: estk_util_min_3num

DESCRIPTION:
  Find the minimum of 3 numbers

PARAMETERS:
  a : [Input] First number
  b : [Input] Second number
  c : [Input] Third number

DEPENDENCIES:
  None

RETURN VALUE:
  int32 - minimum of 3 numbers

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
int32 estk_util_min_3num(int32 a, int32 b, int32 c)
{
  if (a<b)
  {
    if (a<c)
    {
      return a;
    }
    else
    {
      return c;
    }
  }
  else
  {
    if (b<c)
    {
      return b;
    }
    else
    {
      return c;
    }
  }
} /* estk_util_min_3num */

/*===========================================================================
FUNCTION: estk_process_receive_data_req

DESCRIPTION:
  Process the Receive Data Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk RECEIVE DATA cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_receive_data_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  int32                                 index;
  int32                                 ch_id;
  int32                                 copy_len;
  uint32                                cmd_ref_id;
  estk_result_enum_type                 estk_status = ESTK_SUCCESS;
  gstk_receive_data_req_type            *recv_data_req;
  gstk_additional_info_ptr_type         additional_info;
  uint8                                 addi_res;
  gstk_raw_cmd_from_card_type           raw_cmd;

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&raw_cmd, 0x00, sizeof(gstk_raw_cmd_from_card_type));
  memset(&additional_info, 0x00, sizeof(additional_info));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;
  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  recv_data_req = &(gstk_req_ptr->cmd.receive_data_pro_cmd_req);
  ch_id = recv_data_req->ch_id;

  estk_status = estk_convert_ch_id_to_index(ch_id, &index);

  if(estk_status != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("estk_convert_ch_id_to_index failed");
    return ESTK_ERROR;
  }

  /* Cache the currently ongoing proactive command */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_RECEIVE_DATA_REQ;

  /* If channel is already closed and no data is available send error TR
     if channel is closed, but data is available to send, send the available
     data to the card */
  if((estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0) &&
     (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_CLOSED ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_CLOSE_IN_PROGRESS ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_DROPPED))
  {
    UIM_MSG_ERR_1("Channel with status 0x%x not active to receive data",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_RECEIVE_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  /* Check if there is available data */
  if (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0)
  {
    UIM_MSG_ERR_1("Channel with status 0x%x has no available data",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_RECEIVE_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  /* Cache the info that RECEIVE DATA has occurred. This shall be used later
     when processing Channel Close to see if the BIP Session was successful or
     not */
  estk_curr_inst_ptr->bip_channel_table[index].receive_data = TRUE;

  if((estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == TRUE) &&
     (estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed == FALSE))
  {
    UIM_MSG_ERR_0("receive_data_allowed is FALSE, returning error");
    return ESTK_ERROR;
  }

  /* Populate terminal response with data from the receive data */
  estk_curr_inst_ptr->extra_param.present = TRUE;
  estk_curr_inst_ptr->extra_param.rsp_type = GSTK_RECEIVE_DATA_CNF;

  copy_len = estk_util_min_3num((int32)ESTK_BIP_MAX_CH_DATA_SIZE,
                                recv_data_req->ch_data_length,
                                estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

  UIM_MSG_HIGH_3("estk_util_min_3num returns %d, request length %d , buf len %d",
                 copy_len,
                 recv_data_req->ch_data_length,
                 estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

  estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data_len = copy_len;

  estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data
    = (uint8 *)GSTK_CALLOC(copy_len);

  if(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data == NULL)
  {
    return ESTK_ERROR;
  }

  (void)gstk_memcpy((void *)estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data,
                 (void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
                 copy_len,
                 copy_len,
                 copy_len);

  estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data_remaining_len =
     estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len - copy_len;

  UIM_MSG_HIGH_1("Receive Data sends terminal response with remaining len: %d",
                 estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data_remaining_len);

  /* Check if display alpha is present*/
  if(((recv_data_req->alpha.length) > 0 && (recv_data_req->alpha.alphabet)) ||
     (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
        GSTK_SLOT_ID_MAX) == TRUE))

  {
    UIM_MSG_HIGH_0("ALPHA TLV present in Receive Data command"  );
    if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
    {
      estk_status = estk_send_alpha_to_display_client(gstk_req_ptr, TRUE);
      if(estk_status != ESTK_SUCCESS)
      {
        return ESTK_ERROR;
      }
    }
  }

  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    UIM_MSG_HIGH_0("Saving RECEIVE DATA TR"  );
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }
  else
  {
    /* Send terminal response with channel data copied from Rx buffer */
    estk_status = estk_send_receive_data_terminal_response(cmd_ref_id);
  }

  return estk_status;
} /* estk_process_receive_data_req */

/*===========================================================================
FUNCTION: estk_send_receive_data_terminal_response

DESCRIPTION:
  Send TR for RECEIVE DATA Toolkit Request from the card

PARAMETERS:
  cmd_ref_id : [Input] ref_id of the Receive Data request

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_receive_data_terminal_response(uint32 cmd_ref_id)
{
  int32                                 ch_id;
  int32                                 index;
  int32                                 copy_len;
  gstk_additional_info_ptr_type         additional_info;
  uint8                                 addi_res;
  sint31                                dss_status  = DSS_ERROR;
  estk_result_enum_type                 estk_status = ESTK_SUCCESS;
  sint15                                error_num   = DSS_ERROR;
  gstk_evt_dl_ind_type                  data_avail;

  memset(&additional_info, 0x00, sizeof(additional_info));
  memset(&data_avail, 0x00, sizeof(gstk_evt_dl_ind_type));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;

  ch_id = estk_curr_inst_ptr->curr_cmd.cmd.receive_data_pro_cmd_req.ch_id;
  UIM_MSG_HIGH_1("estk_send_receive_data_terminal_response ch_id = 0x%x", ch_id);

  estk_status = estk_convert_ch_id_to_index(ch_id, &index);
  if(estk_status != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("estk_convert_ch_id_to_index failed");
    return ESTK_ERROR;
  }

  copy_len = estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data_len;

  /* Send terminal response with channel data copied from Rx buffer */
  if(estk_bip_send_terminal_response(
       cmd_ref_id,
       GSTK_RECEIVE_DATA_CNF,
       GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
       NULL,
       &estk_curr_inst_ptr->extra_param,
       index) == ESTK_SUCCESS)
  {
    if(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data != NULL)
    {
      gstk_free(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data);
      estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data = NULL;
    }
    /* Adjust Receive buffer length*/
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len -= copy_len;

    if(estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0)
    {
      UIM_MSG_HIGH_0("Receive buffer is empty after receive data command" );
      if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == TRUE)
      {
        UIM_MSG_HIGH_3("scws_overflow_buf_len is %d, buffer size is %d scws_overflow_buf is %d",
                       estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                       estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                       estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
        if((estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len <=
            (uint32)estk_curr_inst_ptr->bip_channel_table[index].buffer_size) &&
           (estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL))
        {
          /* Copy the data from the overflow buffer if present */
          if(gstk_memcpy((void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
                         (void *)(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf),
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
             estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len)
          {
            addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            return estk_bip_send_terminal_response(cmd_ref_id,
                                                   GSTK_SEND_DATA_CNF,
                                                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                   &additional_info,
                                                   NULL,
                                                   index);
          }

          estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len =
            estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len;

          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
          estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
          estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = 0;
          UIM_MSG_HIGH_1("recv_buf_len is %d, scws_overflow_buf is deleted",
                         estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        }
        else if((estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len >
                 (uint32)estk_curr_inst_ptr->bip_channel_table[index].buffer_size) &&
                (estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL))
        {
          if(gstk_memcpy((void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
                         (void *)(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf),
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
             (size_t)estk_curr_inst_ptr->bip_channel_table[index].buffer_size)
          {
            addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            return estk_bip_send_terminal_response(cmd_ref_id,
                                                   GSTK_SEND_DATA_CNF,
                                                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                   &additional_info,
                                                   NULL,
                                                   index);
          }

          estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;


          UIM_MSG_HIGH_0("Copy overflow scws to receive buffer");
          if(gstk_memcpy(
               (void *)estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf,
               (void *)(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf + estk_curr_inst_ptr->bip_channel_table[index].buffer_size),
               estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len - estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
               estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
               estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
             estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len - estk_curr_inst_ptr->bip_channel_table[index].buffer_size)
          {
            addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            return estk_bip_send_terminal_response(cmd_ref_id,
                                                   GSTK_SEND_DATA_CNF,
                                                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                   &additional_info,
                                                   NULL,
                                                   index);
          }

          estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len -=
            (uint16)estk_curr_inst_ptr->bip_channel_table[index].buffer_size;

          UIM_MSG_HIGH_2("scws_overflow_buf_len= %d, recv_buf_len = %d",
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size);
        }
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_id
          = estk_curr_inst_ptr->bip_channel_table[index].channel_id;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.is_scws_ch = TRUE;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.scws_ch_status =
        (gstk_scws_ch_state_enum_type)((uint32)estk_curr_inst_ptr->bip_channel_table[index].status - (uint32)ESTK_CH_SCWS_OFFSET);
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_status_additional_info
          = GSTK_CH_STATUS_NO_FURTHER_INFO;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.is_link_est = TRUE;
        data_avail.evt_type = GSTK_DATA_AVAIL_EVT_IND;
        data_avail.evt_ind.data_avail_evt_ind.ch_len
          = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

        UIM_MSG_HIGH_1("ESTK sending data available event with size %d",
                       data_avail.evt_ind.data_avail_evt_ind.ch_len);
        if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
                                                   0, &data_avail) != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Sending data available envelope failed");
          return ESTK_ERROR;
        }
      }
      else
      {
        dss_status = dss_async_select(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                                      DS_READ_EVENT,
                                      &error_num);
        if(dss_status == DSS_ERROR)
        {
          UIM_MSG_ERR_0("dss_async_select failed");
          return ESTK_ERROR;
        }
        else
        {
          UIM_MSG_HIGH_0("dss_async_select succeeded");
          return ESTK_SUCCESS;
        }
      }
    }
    else
    {
      /* If protocol is TCP, attempt continuous read from DATA (ETSI 102 223:
         6.4.29 is not applicable to TCP since its a streaming protocol) if there
         is space available in the receive buffer and buffer size is greater
         than 0xFE and continuous read feature is enabled */
      if((gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ,
            estk_curr_inst_ptr->slot_id) == TRUE) &&
         (estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP) &&
         ((estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
           estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len) > 0) &&
         (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len >= 0xFF))
      {
        dss_status = dss_async_select(
                       estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                       DS_READ_EVENT,
                       &error_num);

        if(dss_status != DSS_ERROR)
        {
          UIM_MSG_HIGH_0("dss_async_select succeeded");
        }
        else
        {
          UIM_MSG_ERR_0("dss_async_select failed");
        }
      }

      UIM_MSG_HIGH_0("Adjust receive buffer for leftover data");
      memsmove((void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
               (size_t)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len,
               (void *)(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer+copy_len),
               (size_t)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
    }
  }
  else
  {
    if(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data != NULL)
    {
      UIM_MSG_HIGH_0("Receive data mem leak");
      gstk_free(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data);
      estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data = NULL;
    }
    UIM_MSG_ERR_0("estk_send_terminal_response failed" );
    return ESTK_ERROR;
  }

  return ESTK_SUCCESS;

}

/*===========================================================================
FUNCTION: estk_process_get_ch_status_req

DESCRIPTION:
  Process the Get Channel Status Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk GET CHANNEL STATUS cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_get_ch_status_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  uint8                                 i           = 0;
  gstk_terminal_rsp_extra_param_type    tr_extra_param;

  if(gstk_req_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_GET_CH_STATUS_CNF;

  for(i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_id
      = i + 1;
    if(estk_curr_inst_ptr->bip_channel_table[i].is_scws_ch == TRUE)
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].is_scws_ch = TRUE;

      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_status_additional_info
        = GSTK_CH_STATUS_NO_FURTHER_INFO;
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].scws_ch_status =
        (gstk_scws_ch_state_enum_type)((uint32)estk_curr_inst_ptr->bip_channel_table[i].status - (uint32)ESTK_CH_SCWS_OFFSET);
    }
    if(estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_ACTIVE)
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].is_link_est
        = TRUE;
    }
    else
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].is_link_est
        = FALSE;
    }

    if(estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_DROPPED)
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_status_additional_info
        = GSTK_CH_STATUS_LINK_DROPPED;
    }
    else
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_status_additional_info
        = GSTK_CH_STATUS_NO_FURTHER_INFO;
    }

    tr_extra_param.extra_param.get_ch_status_extram_param.cnt = (i+1);
  }
  return estk_bip_send_terminal_response(
           gstk_req_ptr->hdr_cmd.cmd_detail_reference,
           GSTK_GET_CH_STATUS_CNF,
           GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
           NULL,
           &tr_extra_param,
           ESTK_MAX_BIP_CH);
} /* estk_process_get_ch_status_req */

/*===========================================================================
FUNCTION: estk_process_setup_evt_list_req

DESCRIPTION:
  Process the Set Up Event List Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk OPEN CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_evt_list_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }
  UIM_MSG_HIGH_1("estk_process_setup_evt_list_req: command_id=0x%x",
                 gstk_req_ptr->hdr_cmd.command_id);

  if(gstk_req_ptr->hdr_cmd.command_id == GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY)
  {
    /*Setting Data Available Event Flag*/
 //   estk_data_avail_evt_flag = TRUE;
  }
  else if(gstk_req_ptr->hdr_cmd.command_id == GSTK_SETUP_EVT_CH_STATUS_NOTIFY)
  {
    /*Setting Channel Status Notify Flag*/
 //   estk_ch_status_evt_flag = TRUE;
  }
  else
  {
    //Should not get here
    UIM_MSG_ERR_1("Unhandled command id %d", gstk_req_ptr->hdr_cmd.command_id);
    return ESTK_ERROR;
  }

  return ESTK_SUCCESS;
} /* estk_process_setup_evt_list_req */

/*===========================================================================
FUNCTION: estk_process_close_ch_req

DESCRIPTION:
  Process the Close Channel Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk CLOSE CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_close_ch_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  int32                               close_ch_id;
  int32                               i;
  int32                               index = -1;
  uint32                              mask  = 0;
  gstk_status_enum_type               gstk_status;
  gstk_client_id_type                 client_id;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res;
  gstk_raw_cmd_from_card_type         raw_cmd;
  uint32                              user_data   = 0;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type call_cli_evt_cb;
  gstk_cmd_from_card_type               gstk_scws_cmd;
  boolean                               keep_ch_open = FALSE;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&raw_cmd, 0x00, sizeof(gstk_raw_cmd_from_card_type));
  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));
  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_process_close_ch_req: cmd_detail_reference: 0x%x, ch_id: 0x%x",
                 gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                 gstk_req_ptr->cmd.close_ch_pro_cmd_req.ch_id);

  close_ch_id = gstk_req_ptr->cmd.close_ch_pro_cmd_req.ch_id;

  for(i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[i].channel_id == close_ch_id)
    {
      if(estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_ACTIVE
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_PENDING
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_OPEN_WAIT
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_DROPPED
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_CLOSE_IN_PROGRESS
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_SCWS_LISTEN
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_SCWS_ESTABLISHED
      )
      {
        UIM_MSG_ERR_1("Invalid channel id 0x%x to close", close_ch_id);
        break;
      }
      else
      {
        index = i;
        UIM_MSG_HIGH_2("Found channel 0x%x to close, index=0x%x",
                       close_ch_id, index);
        break;
      }
    }
  }

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_HIGH_0("Send error terminal response for close channel");
    additional_info.length = 1;
    addi_res = (uint8)GSTK_CH_ID_INVALID;
    additional_info.additional_info_ptr = &addi_res;
    return estk_bip_send_terminal_response(
              gstk_req_ptr->hdr_cmd.cmd_detail_reference,
              GSTK_CLOSE_CH_CNF,
              GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
              &additional_info,
              NULL,
              ESTK_MAX_BIP_CH);
  }

  /* Cache the currently ongoing proactive command */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_CLOSE_CH_REQ;

  /* Have to handle if display alpha is present */
  if(((gstk_req_ptr->cmd.close_ch_pro_cmd_req.alpha.length) > 0 &&
      (gstk_req_ptr->cmd.close_ch_pro_cmd_req.alpha.alphabet)) ||
     (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND,
        GSTK_SLOT_ID_MAX) == TRUE))
  {
    UIM_MSG_HIGH_0("ALPHA TLV present in CLOSE Channel command"  );
    if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
    {
      if(estk_send_alpha_to_display_client(gstk_req_ptr, TRUE) != ESTK_SUCCESS)
      {
        return ESTK_ERROR;
      }
    }
  }

  /* 
    If bit1 of qualifier in CLOSE CHANNEL for packet data service is 1, next CAT
    command will be OPEN CHANNEL using same Network Access Name as channel to be closed
    
    This is assuming the close channel is NOT for a UICC server mode channel
  */
  if ((estk_curr_inst_ptr->bip_channel_table[index].transport_protocol != GSTK_PKT_DATA_TCP_UICC_SERVER_MODE) &&
      (gstk_req_ptr->cmd.close_ch_pro_cmd_req.qualifier & 0x01))
  {
    keep_ch_open = TRUE;
  }

  if (keep_ch_open)
  {
    /* keep_ch_open_timer */
    if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p == NULL)
    {
      boolean mask_set_status = FALSE;

      ESTK_BIP_SET_SLOT_INDEX_MASK(
        estk_curr_inst_ptr->slot_id, index, mask, mask_set_status);

      if(mask_set_status)
      {
        estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p =
          (rex_timer_type *)GSTK_CALLOC(sizeof(rex_timer_type));

        UIM_MSG_HIGH_1("keep_ch_open_timer_p malloc-ed, ptr = 0x%x",
                       estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
        if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p != NULL)
        {
          rex_def_timer_ex(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p,
                           estk_bip_keep_ch_open_timer_cb,
                           mask);
        }
        else
        {
          return ESTK_ERROR;
        }
      }
    }
    if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p != NULL)
    {
      UIM_MSG_HIGH_1("estk_process_close_ch_req: Keep CH %d open, set timer",
                     index);
      (void)rex_clr_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
      (void)rex_set_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p,
                          ESTK_BIP_KEEP_CH_OPEN_DURATION);

      estk_curr_inst_ptr->keep_ch_open = TRUE;
      estk_curr_inst_ptr->ch_open_id = index;

      /* If sock_fd is valid, close the socket because it might not be reused */
      if (estk_curr_inst_ptr->bip_channel_table[index].sock_fd != DSS_ERROR)
      {
        UIM_MSG_HIGH_0("estk_process_close_ch_req: close existing socket");

        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;

        if(estk_bip_process_sock_close_evt(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
           gstk_req_ptr->hdr_cmd.cmd_detail_reference) == ESTK_SUCCESS)
        {
          return ESTK_SUCCESS;
        }
        else
        {
          UIM_MSG_ERR_0("estk_bip_process_sock_close_evt failed");
          return ESTK_ERROR;
        }
      }

      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR.");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        return ESTK_SUCCESS;
      }
      else
      {
        return estk_bip_send_terminal_response(
                 gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                 GSTK_CLOSE_CH_CNF,
                 GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                 NULL,
                 NULL,
                 index);
      }
    }
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
  {
    if((estk_curr_inst_ptr->bip_channel_table[index].on_demand_link == TRUE)
       && (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING))
    {
      UIM_MSG_HIGH_0("Close channel for on-demand in pending state");

      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving CLOSE CHANNEL  TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        return ESTK_SUCCESS;
      }
      else
      {
        estk_bip_clean_up_ch_table(index);
        return estk_bip_send_terminal_response(
                 gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                 GSTK_CLOSE_CH_CNF,
                 GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                 NULL,
                 NULL,
                 index);
      }
    }

    /* If during the BIP session on WLAN over UDP, only SEND DATA was processed,
       but NO RECEIVE DATA, this means that the remote server was most likely
       not reachable. Hence, mark WLAN as unusable, so that the next BIP session
       is attempted over Cellular.
       Note that this is a workaround since Datamodem does not support method
       to determine if remote server is reachable over UDP.
       Also, this workaround is applied only to WLAN since the possiblity of
       remote server being not reachable over public internet is highly likely */
    if((estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN) &&
       (estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_UDP))
    {
      if((estk_curr_inst_ptr->bip_channel_table[index].first_dss_sendto_done == TRUE) &&
          (estk_curr_inst_ptr->bip_channel_table[index].receive_data == FALSE))
      {
        UIM_MSG_HIGH_0("Marking WLAN as unusable: No Receive Data");
        estk_shared_info.wlan_unusable = TRUE;
      }
    }

    estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;

    /*
      sock_fd is only valid for legacy open channel. For DNS query open channel
      we do not perform net_connect() so we do not need to close any sockets
    */
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == DSS_ERROR)
    {
      /* No additional info to be sent in success case */
      additional_info.length = 0;
      additional_info.additional_info_ptr = NULL;

      return estk_bip_process_sock_close_error(index,
                                               gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                                               GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                                               &additional_info,
                                               NULL);
    }
    else if(estk_bip_process_sock_close_evt(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
         gstk_req_ptr->hdr_cmd.cmd_detail_reference) == ESTK_SUCCESS)
    {
      return ESTK_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_0("estk_bip_process_sock_close_evt failed");
      return ESTK_ERROR;
    }
  }
  else
  {
    find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
    find_cli_reg.cmd_type = GSTK_SCWS_CLOSE_CH_REQ;
    find_cli_reg.format_type = GSTK_GSTK_FORMAT;
    gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_HIGH_0("Did not find any clients registered for GSTK_SCWS_CLOSE_CH_REQ");
      return ESTK_ERROR;
    }
    else
    {
      call_cli_evt_cb.client_id = client_id;
      call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
      gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_CLOSE_CH_REQ;
      gstk_scws_cmd.hdr_cmd.cmd_detail_reference = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
      gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
      gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
      gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

      gstk_scws_cmd.cmd.scws_close_ch_cmd_req.ch_id         = close_ch_id;
      gstk_scws_cmd.cmd.scws_close_ch_cmd_req.channel_state =
        (gstk_scws_ch_state_enum_type)(estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference & 0x1);

      if (gstk_io_ctrl(
            GSTK_IO_CALL_CLIENT_EVT_CB,
            (void *)&call_cli_evt_cb,
            (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0("can't call cli evt cb!");
        return ESTK_ERROR;
      }
    }
  }
  return ESTK_SUCCESS;
} /* estk_process_close_ch_req */

/*===========================================================================
FUNCTION: estk_process_keep_ch_open_expired_evt

DESCRIPTION:
  Process the keep ch open expired event

PARAMETERS:
  ch_idx : [Input] The channel index of which the link is to be torn down

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_keep_ch_open_expired_evt(
  uint32 ch_idx
)
{
  if (ch_idx >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("invalid ch idx 0x%x!", ch_idx);
    return ESTK_BAD_INPUT_PARAM;
  }

  (void)estk_bip_clean_up_ch_table(ch_idx);

  return ESTK_SUCCESS;
}

/*===========================================================================
FUNCTION: estk_process_cellular_oos_evt

DESCRIPTION:
  Processes the Cellular OOS event received from Data Services (via GSTK).
  If Location Status (from MM IDLE/CM SS event) is Normal Service,
  function starts a 10 sec timer to wait for Location status to change,
  else sends Link Dropped Envelope as required.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cellular_oos_evt(void)
{
  uint32                         ch_idx      = 0;
  gstk_slot_id_enum_type         slot       = GSTK_SLOT_ID_MAX;
  gstk_location_status_enum_type loc_status = GSTK_NO_SERVICE;

  UIM_MSG_HIGH_0("In estk_process_cellular_oos_evt");

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  slot = estk_curr_inst_ptr->slot_id;

  if (gstk_io_ctrl(GSTK_IO_GET_CURR_LOC_STATUS,
                   (void *)&slot,
                   (void *)&loc_status) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to get loc status");
    return ESTK_ERROR;
  }

  /* If Locaton Status is still Normal Service, start 10 sec timer to wait for
     location status to change */
  if (loc_status == GSTK_NORMAL_SERVICE)
  {
    UIM_MSG_HIGH_1("Still in service, start link drop timer, duration: 0x%x",
                   ESTK_OOS_TIMER_DURATION);

    (void)rex_clr_timer(&(estk_curr_inst_ptr->cellular_oos_timer));
    (void)rex_set_timer(&(estk_curr_inst_ptr->cellular_oos_timer),
                        ESTK_OOS_TIMER_DURATION);
  }
  else
  {
    for(ch_idx = 0; ch_idx < ESTK_MAX_BIP_CH; ch_idx++)
    {
      UIM_MSG_HIGH_3("Channel Table Index: 0x%x, Status: 0x%x, NW type: 0x%x",
                      ch_idx,
                      estk_curr_inst_ptr->bip_channel_table[ch_idx].status,
                      estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type);

      UIM_MSG_HIGH_2("NW type: 0x%x, Slot 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type,
                     estk_curr_inst_ptr->slot_id);

      /* Process Cellular OOS event only for Channels open over Cellular */
      if (estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type != ESTK_NETWORK_CELLULAR)
      {
        continue;
      }

      UIM_MSG_HIGH_1("Current Command: 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].current_command_id);

      switch (estk_curr_inst_ptr->bip_channel_table[ch_idx].current_command_id)
      {
        case GSTK_OPEN_CH_REQ:
        case GSTK_CLOSE_CH_REQ:
          /* OPEN/CLOSE CHANNEL is in progress */
          if (estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_ACTIVE)
          {
            estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;
          }
          break;
        case GSTK_SEND_DATA_REQ:
        return estk_bip_process_send_data_error(ch_idx);
        default:
          if ((estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_ACTIVE) ||
              ((estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_PENDING) &&
               (estk_curr_inst_ptr->bip_channel_table[ch_idx].on_demand_link == TRUE)))
          {
            UIM_MSG_HIGH_0("send link drop envelope cmd");
            estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;

            return estk_bip_send_ch_status_event(
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].channel_id,
                     GSTK_CH_STATUS_LINK_DROPPED);
          }
      }
    }
  }

  return ESTK_SUCCESS;
} /* estk_process_cellular_oos_evt */

/*===========================================================================
FUNCTION: estk_process_cellular_status_change_evt

DESCRIPTION :
  This function processes a Cellular Status Change notification from GSTK

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cellular_status_change_evt(
  const estk_cmd_type *cmd_ptr
)
{
  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_2("In estk_process_cellular_status_change_evt, Cellular status: 0x%x, Slot: 0x%x",
                 cmd_ptr->cmd_data.cellular_status_evt.cellular_service_available,
                 estk_curr_inst_ptr->slot_id);

  /* If DS indicates that Cellular Service has become available and Location
     status has always been Normal Service, stop the OOS timer */
  if(cmd_ptr->cmd_data.cellular_status_evt.cellular_service_available)
  {
    if(rex_get_timer(&(estk_curr_inst_ptr->cellular_oos_timer)) > 0)
    {
      UIM_MSG_HIGH_0("Clear running OOS timer");
      (void) rex_clr_timer(&(estk_curr_inst_ptr->cellular_oos_timer));
    }
  }
  else
  {
    estk_process_cellular_oos_evt();
  }

  return ESTK_SUCCESS;
} /* estk_process_cellular_status_change_evt */

/*===========================================================================
FUNCTION: estk_process_wlan_status_change_evt

DESCRIPTION :
  This function processes a WLAN status change event from GSTK

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_wlan_status_change_evt(
  const estk_cmd_type *cmd_ptr
)
{
  uint32 ch_idx = 0;

  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }


  UIM_MSG_HIGH_1("In estk_process_wlan_status_change_evt, WLAN status: 0x%x",
                 cmd_ptr->cmd_data.wlan_status_evt.wlan_available);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr->cmd_data.wlan_status_evt.wlan_available)
  {
    /* If WLAN status has changed, then we can re-attempt BIP over WLAN,
       so mark WLAN as unable again (if currently marked unusable) */
    estk_reset_wlan_unusable_status();
    return ESTK_SUCCESS;
  }

  for(ch_idx = 0; ch_idx < ESTK_MAX_BIP_CH; ch_idx++)
  {
    UIM_MSG_HIGH_3("Channel Table Index: 0x%x, Status: 0x%x, NW type: 0x%x",
                   ch_idx,
                   estk_curr_inst_ptr->bip_channel_table[ch_idx].status,
                   estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type);

    UIM_MSG_HIGH_2("NW type: 0x%x, Slot 0x%x",
                   estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type,
                   estk_curr_inst_ptr->slot_id);

    /* Process WLAN starus change event only for Channels open over WLAN */
    if (estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type != ESTK_NETWORK_WLAN)
    {
      continue;
    }

    UIM_MSG_HIGH_1("Current Command: 0x%x",
                   estk_curr_inst_ptr->bip_channel_table[ch_idx].current_command_id);

    switch (estk_curr_inst_ptr->bip_channel_table[ch_idx].current_command_id)
    {
      case GSTK_OPEN_CH_REQ:
      case GSTK_CLOSE_CH_REQ:
        /* OPEN/CLOSE CHANNEL is in progress */
        if (estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_ACTIVE)
        {
          estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;
        }
        break;
      case GSTK_SEND_DATA_REQ:
        return estk_bip_process_send_data_error(ch_idx);
      default:
        if ((estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_ACTIVE) ||
            ((estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_PENDING) &&
             (estk_curr_inst_ptr->bip_channel_table[ch_idx].on_demand_link == TRUE)))
        {
          UIM_MSG_HIGH_0("send link drop envelope cmd");
          estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;

          return estk_bip_send_ch_status_event(
                   estk_curr_inst_ptr->bip_channel_table[ch_idx].channel_id,
                   GSTK_CH_STATUS_LINK_DROPPED);
        }
    }
  }

  return ESTK_SUCCESS;
} /* estk_process_wlan_status_change_evt */

/*===========================================================================
FUNCTION: estk_process_wlan_ip_addr_change_evt

DESCRIPTION:
  Process the WLAN IP address change event

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_wlan_ip_addr_change_evt(void)
{
  UIM_MSG_HIGH_0("In estk_process_wlan_ip_addr_change_evt()");

  /* If WLAN status has changed (indicated by IP address change here), then we
     can re-attempt BIP over WLAN, so mark WLAN as unable
     again (if currently marked unusable) */
  estk_reset_wlan_unusable_status();

  return ESTK_SUCCESS;
} /*estk_process_wlan_ip_addr_change_evt */

/*===========================================================================
FUNCTION:   estk_open_ch_cleanup

DESCRIPTION:
  This ESTK function clears the open channel related private data.

PARAMETERS:
  None

DEPENDENCIES:
  open_ch_ptr : [Input] Open Channel pointer to clean-up

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_open_ch_cleanup(
  gstk_open_ch_req_type *open_ch_ptr
)
{
  if(open_ch_ptr == NULL)
  {
    UIM_MSG_ERR_0("estk_open_ch_ptr is NULL");
    return;
  }

  if(open_ch_ptr->alpha.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->alpha.alphabet);
    open_ch_ptr->alpha.alphabet = NULL;
  }

  if(open_ch_ptr->access_point_name.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->access_point_name.alphabet);
    open_ch_ptr->access_point_name.alphabet = NULL;
  }

  if(open_ch_ptr->local_address.pkt_address.address != NULL)
  {
    gstk_free(open_ch_ptr->local_address.pkt_address.address);
    open_ch_ptr->local_address.pkt_address.address = NULL;
  }

  if(open_ch_ptr->data_dest_address.pkt_address.address != NULL)
  {
    gstk_free(open_ch_ptr->data_dest_address.pkt_address.address);
    open_ch_ptr->data_dest_address.pkt_address.address = NULL;
  }

  if(open_ch_ptr->text_string_user_login.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->text_string_user_login.alphabet);
    open_ch_ptr->text_string_user_login.alphabet = NULL;
  }

  if(open_ch_ptr->text_string_user_passwd.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->text_string_user_passwd.alphabet);
    open_ch_ptr->text_string_user_passwd.alphabet = NULL;
  }
} /*estk_open_ch_cleanup*/

/*===========================================================================
FUNCTION:   estk_clean_up_bip_open_ch_timer_exp

DESCRIPTION:
  This ESTK function cleans up ESTK globals on open channel timer expiry

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_clean_up_bip_open_ch_timer_exp(void)
{
  if(estk_curr_inst_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  if((estk_curr_inst_ptr->curr_bip_cmd_index < 0)
     || (estk_curr_inst_ptr->curr_bip_cmd_index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_0("Invalid index to clean up");
    return ESTK_ERROR;
  }
  estk_bip_clean_up_ch_table(estk_curr_inst_ptr->curr_bip_cmd_index);
  return ESTK_SUCCESS;
}
/*estk_clean_up_bip_open_ch_timer_exp*/

/*===========================================================================
FUNCTION:   estk_clean_up_bip_send_data_timer_exp

DESCRIPTION:
  This ESTK function cleans up send data buffer on send data timer expiry

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_clean_up_bip_send_data_timer_exp(void)
{
  int8 index = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  index = estk_curr_inst_ptr->curr_bip_cmd_index;

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    return ESTK_ERROR;
  }

  /* Reset current command as SEND DATA is no longer being processed */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_CMD_ENUM_NOT_USE;

  if(estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.send_data_immediately)
  {
    estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = TRUE;
  }
  return ESTK_SUCCESS;
}
/*estk_clean_up_bip_send_data_timer_exp*/

/*===========================================================================
FUNCTION:   estk_bip_full_clean_up

DESCRIPTION:
  This ESTK function cleans up entire BIP table

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_full_clean_up(void)
{
  uint8                                     i = 0;

  for (i=0; i< ESTK_MAX_BIP_CH; i++)
  {
    estk_bip_clean_up_ch_table(i);
  }
}
/*estk_bip_full_clean_up*/

/*===========================================================================
FUNCTION:   estk_bip_full_clean_up_slot

DESCRIPTION :
  Free memory allocated for the BIP channel

PARAMETERS:
  slot_id : [Input] slot on which the BIP channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_full_clean_up_slot(gstk_slot_id_enum_type slot_id)
{
  uint8 i = 0;

  UIM_MSG_HIGH_1("estk_bip_full_clean_up_slot: slot_id=0x%x", slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    estk_bip_clean_up_ch_table(i);
  }
  estk_instances_ptr[(uint32)slot_id - 1]->curr_bip_cmd_index = -1;
}/* estk_bip_full_clean_up_slot */

/*===========================================================================
FUNCTION: estk_uim_is_bip_session_in_progress

DESCRIPTION:
  Return status indicating any BIP session is in progress on the slot requested

PARAMETERS:
  slot_id : [Input] slot on which the BIP session status is requested

DEPENDENCIES:
  None

RETURN VALUE:
  Return
  TRUE:  BIP session in progress
  FALSE: No BIP session in progress

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean estk_uim_is_bip_session_in_progress(
  gstk_slot_id_enum_type slot_id
)
{
  boolean estk_bip_session_in_progress = FALSE;
  uint16 i = 0;

  UIM_MSG_HIGH_1("estk_uim_is_bip_session_in_progress: slot_id: 0x%x",
                 slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return FALSE;
  }

  if(estk_instances_ptr[(uint32)slot_id - 1] == NULL)
  {
    UIM_MSG_ERR_1("ESTK instance: 0x%x is NULL",
                  ((uint32)slot_id - 1));
    return FALSE;
  }

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if ((estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].status !=
         ESTK_CH_CLOSED) &&
        (estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].status !=
         ESTK_CH_DROPPED))
    {
      /* Even if one channel is not CLOSED or DROPPED
         implies a BIP session is active. So, STATUS command
         cannot return with FCP or DF */
      estk_bip_session_in_progress = TRUE;
      UIM_MSG_HIGH_1("BIP session in progress is %d",
                     estk_bip_session_in_progress);
      break;
    }
  }
  return estk_bip_session_in_progress;
}

/*===========================================================================
FUNCTION: estk_reset_wlan_unusable_status

DESCRIPTION:
  Clears the WLAN unusable status in ESTK (w.r.t. BIP transactions)

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_reset_wlan_unusable_status(void)
{
  if(estk_shared_info.wlan_unusable == TRUE)
  {
    UIM_MSG_HIGH_0("Reset WLAN unusable status");
    estk_shared_info.wlan_unusable = FALSE;
  }
}

/*===========================================================================
FUNCTION: estk_bip_reset_curr_cmd_id_for_channel

DESCRIPTION:
  Clears the current command ID in the BIP channel table

PARAMETERS:
  index:      [Input] Index of BIP channel table for which the Current
                      command ID needs to be cleared. Clear only if index
                      is in valid range

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_reset_curr_cmd_id_for_channel(uint32 index)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(index < ESTK_MAX_BIP_CH)
  {
    estk_curr_inst_ptr->bip_channel_table[index].current_command_id =
      GSTK_CMD_ENUM_NOT_USE;
  }
} /* estk_bip_reset_curr_cmd_id_for_channel */

/*===========================================================================

FUNCTION estk_bip_send_terminal_response

DESCRIPTION:
  This function is served as a utility function to the estk client, so that
  the client can call this function in sending a terminal response to
  a BIP proactive command.

  The function resets the current command ID stored in the BIP channel table
  index (only if a valid index is passed in).

PARAMETERS:
  cmd_detail_ref_id:      [Input] Specifies the reference ID for command details of
                                  the proactive command that the client is sending
                                  a terminal response to.  The reference id value
                                  is sent to the client earlier in the structure
                                  of the proactive command request, inside the
                                  gstk_exp_hdr_type
  terminal_response_type: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result:         [Input] Specifies the result of the proactive command
                                  operation
  additional_result_ptr:  [Input] Pointer specifies the optional additional result
                                  for the proactive command, the length field in
                                  the gstk_additonal_info_type specifies number of
                                  additional_info in the structure
  other_info_ptr:         [Input] A union pointer of various extra parameters for the
                                  various proactive command response, please refer to
                                  gstk_terminal_rsp_param_type for details
  bip_channel_index:      [Input] Index of BIP channel table for which the Current
                                  command ID needs to be cleared. Clear only if index
                                  is in valid range

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  This function should only be called after estk_client_init has been called

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_send_terminal_response (
  uint32                                     cmd_detail_ref_id,
  gstk_cmd_enum_type                         terminal_response_type,
  gstk_general_result_enum_type              general_result,
  gstk_additional_info_ptr_type              *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type         *other_info_ptr,
  uint32                                     bip_channel_index)
{
  UIM_MSG_HIGH_1("estk_bip_send_terminal_response, index: 0x%x",
                 bip_channel_index);

  /* Clear current command ID in the BIP channel table if a valid index is
     provided - since TR is being sent, command is no longer is progress */
  estk_bip_reset_curr_cmd_id_for_channel(bip_channel_index);

  /* Send the Terminal Response */
  return estk_send_terminal_response(
           cmd_detail_ref_id,
           terminal_response_type,
           general_result,
           additional_result_ptr,
           other_info_ptr);
} /* estk_bip_send_terminal_response */

/*===========================================================================

FUNCTION: estk_scws_send_data_avail_rsp

DESCRIPTION :
  This function sends the data available response

PARAMETERS:
  index : [Input] Index to the estk channel table

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data_avail_rsp (
  uint32 index, boolean result)
{

  gstk_status_enum_type                    gstk_status = GSTK_SUCCESS;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type call_cli_evt_cb;
  uint32                                   user_data   = 0;
  gstk_cmd_from_card_type                  gstk_scws_cmd;
  gstk_client_id_type                      client_id   = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Index is out or range 0x%x", index);
    return ESTK_ERROR;
  }

  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));
  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));

  find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
  find_cli_reg.cmd_type = GSTK_SCWS_DATA_AVAIL_RSP_REQ;
  find_cli_reg.format_type = GSTK_GSTK_FORMAT;
  gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    return ESTK_ERROR;
  }
  else
  {
    call_cli_evt_cb.client_id = client_id;
    call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
    gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_DATA_AVAIL_RSP_REQ;
    gstk_scws_cmd.hdr_cmd.cmd_detail_reference = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
    gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->slot_id;
    gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
    gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

    gstk_scws_cmd.cmd.scws_data_avail_rsp_cmd_req.ch_id    = index+1;
    gstk_scws_cmd.cmd.scws_data_avail_rsp_cmd_req.success  = result;

    if (gstk_io_ctrl(
          GSTK_IO_CALL_CLIENT_EVT_CB,
          (void *)&call_cli_evt_cb,
          (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("can't call cli evt cb!");
      return ESTK_ERROR;
    }
  }
  return ESTK_SUCCESS;
} /* estk_scws_send_data_avail_rsp */

/*===========================================================================

FUNCTION: estk_process_scws_ch_status_req

DESCRIPTION :
  This function processes Ch Status request from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_ch_status_req(
  const estk_cmd_type *cmd_ptr)
{

  gstk_evt_dl_ind_type        ch_status;
  uint32                      index = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_ch_status_req");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_ch_status_req.ch_id - 1;

  UIM_MSG_HIGH_3("estk_process_scws_ch_status_req: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_ch_status_req.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH) ||
      (cmd_ptr->cmd_data.scws_ch_status_req.ch_id !=
       estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&ch_status, 0x00, sizeof(gstk_evt_dl_ind_type));

  estk_curr_inst_ptr->bip_channel_table[index].status =
     (estk_channel_status_type)((uint32)cmd_ptr->cmd_data.scws_ch_status_req.ch_status + (uint32)ESTK_CH_SCWS_OFFSET);
  /* Send Channel status Event Download envelope command */
  ch_status.evt_type = GSTK_CH_STATUS_EVT_IND;
  ch_status.evt_ind.ch_status_evt_ind.ch_id =
         estk_curr_inst_ptr->bip_channel_table[index].channel_id;
  ch_status.evt_ind.ch_status_evt_ind.is_scws_ch = TRUE;
  ch_status.evt_ind.ch_status_evt_ind.scws_ch_status = cmd_ptr->cmd_data.scws_ch_status_req.ch_status;
  ch_status.evt_ind.ch_status_evt_ind.ch_status_additional_info =
         GSTK_CH_STATUS_NO_FURTHER_INFO;
  if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
             0, &ch_status) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Sending Channel Status envelope failed");
    return ESTK_ERROR;
  }
  if((cmd_ptr->cmd_data.scws_ch_status_req.ch_status == GSTK_SCWS_TCP_IN_LISTEN_STATE) ||
     (cmd_ptr->cmd_data.scws_ch_status_req.ch_status == GSTK_SCWS_TCP_IN_CLOSED_STATE))
  {
     estk_scws_clear_send_receive_buf((uint8)ch_status.evt_ind.ch_status_evt_ind.ch_id);
  }
  if(cmd_ptr->cmd_data.scws_ch_status_req.ch_status == GSTK_SCWS_TCP_IN_LISTEN_STATE)
  {
    UIM_MSG_HIGH_0("Setting receive_data_allowed to FALSE");
    estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed = FALSE;
  }
  return ESTK_SUCCESS;
} /* estk_process_scws_ch_status_req */

/*===========================================================================

FUNCTION: estk_process_scws_data_avail_req

DESCRIPTION :
  This function processes Data Avail req from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_data_avail_req(
  estk_cmd_type *cmd_ptr)
{

   gstk_evt_dl_ind_type        data_avail;
   uint32                      index = 0;
   boolean                     data_avail_required = FALSE;
   uint8                       *tmp_ptr = NULL;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_data_avail_req");
    estk_scws_send_data_avail_rsp((uint32)index, FALSE);
    return ESTK_ERROR;
  }

  if(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr == NULL)
  {
    UIM_MSG_ERR_0("No data was sent, return error");
    estk_scws_send_data_avail_rsp((uint32)index, FALSE);
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_data_avail_req.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_data_avail_req: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_data_avail_req.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH)||
      (cmd_ptr->cmd_data.scws_data_avail_req.ch_id !=
       estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
    cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
    estk_scws_send_data_avail_rsp((uint32)index, FALSE);
    return ESTK_ERROR;
  }

  memset(&data_avail, 0x00, sizeof(gstk_evt_dl_ind_type));

  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0)
  {
    /* Since the receive data is zero, Data Availale envelope is required */
    data_avail_required = TRUE;
  }

  UIM_MSG_HIGH_3("Data received is 0x%x, buffer size is 0x%x receive buffer len is 0x%x",
                 cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                 estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                 estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

  if((estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL) &&
     ((uint32)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len <=
      ((uint32)estk_curr_inst_ptr->bip_channel_table[index].buffer_size - (uint32)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len))
      && (estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf == NULL))
  {
    if(gstk_memcpy((void *)(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer +
                   estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len),
                   (void *)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                   cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                   estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                   cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len) <
       cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len)
    {
      gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
      cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
      estk_scws_send_data_avail_rsp((uint32)index, FALSE);
      return ESTK_ERROR;
    }
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len += (int32)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len;
  }
  else
  {
    UIM_MSG_HIGH_0("Have to use the SCWS overflow buffer");
    /* Copy the data to the overflow buffer */
    if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf == NULL)
    {
      UIM_MSG_HIGH_0("scws_overflow_buf is Null");
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf =
            (uint8 *)GSTK_CALLOC(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len * sizeof(uint8));
      if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf == NULL)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }

      (void)gstk_memcpy((void *)estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf,
                        (void *)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len);
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len;
      UIM_MSG_HIGH_1("scws_overflow_buf_len is 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
    }
    else
    {
      /* The buffer is not empty, extend it with new data */
      UIM_MSG_HIGH_1("SCWS buffer already exists, allocating a buffer of size 0x%x",
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len +
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
      tmp_ptr =
         (uint8 *)GSTK_CALLOC(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len +
           estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
      if(tmp_ptr == NULL)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }
      /* Copy the existing overflow buffer */
      if(gstk_memcpy((void *)tmp_ptr,
                     (void *)estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf,
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len + estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        gstk_free(tmp_ptr);
        tmp_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }
      /* Copy new data */
      if(gstk_memcpy((void *)(tmp_ptr + estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len),
                     (void *)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len + cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len) <
         cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        gstk_free(tmp_ptr);
        tmp_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len +=
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len;
      UIM_MSG_HIGH_1("New scws overflow buf is 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
      /* Release old buffer and use the new one */
      gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = tmp_ptr;
    }
  }

   /* Send event download envelope with data available event */
  if (data_avail_required)
  {
    data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_id
      = estk_curr_inst_ptr->bip_channel_table[index].channel_id;
    data_avail.evt_ind.data_avail_evt_ind.ch_status.is_scws_ch = TRUE;
    data_avail.evt_ind.data_avail_evt_ind.ch_status.scws_ch_status =
        (gstk_scws_ch_state_enum_type)((uint32)estk_curr_inst_ptr->bip_channel_table[index].status - (uint32)ESTK_CH_SCWS_OFFSET);
    data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_status_additional_info
      = GSTK_CH_STATUS_NO_FURTHER_INFO;
    data_avail.evt_ind.data_avail_evt_ind.ch_status.is_link_est = TRUE;
    data_avail.evt_type = GSTK_DATA_AVAIL_EVT_IND;
    data_avail.evt_ind.data_avail_evt_ind.ch_len
       = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

    UIM_MSG_HIGH_1("ESTK sending data available event with data length 0x%x",
                   data_avail.evt_ind.data_avail_evt_ind.ch_len);
    if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
                                               0, &data_avail) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Sending data available envelope failed");
      gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
      cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
      estk_scws_send_data_avail_rsp((uint32)index, FALSE);
      return ESTK_ERROR;
    }
  }

  gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
  cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;

   /* Reply to the data available message */
  return estk_scws_send_data_avail_rsp((uint32)index, TRUE);
} /* estk_process_scws_data_avail_req */
/*===========================================================================

FUNCTION: estk_process_scws_open_ch_rsp

DESCRIPTION :
  This function processes the Open Ch response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_open_ch_rsp(
  const estk_cmd_type *cmd_ptr)
{
  estk_result_enum_type               estk_status  = ESTK_ERROR;
  uint32                              index        = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  uint32                              cmd_ref_id = 0;
  gstk_general_result_enum_type       gen_result       = GSTK_GENERAL_RESULT_MAX;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_open_ch_rsp");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_open_ch_rsp: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH) ||
      (cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id !=
       estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;

  estk_curr_inst_ptr->bip_channel_table[index].status =
      (estk_channel_status_type)((uint32)cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status + (uint32)ESTK_CH_SCWS_OFFSET);
  if(cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status != GSTK_SCWS_TCP_IN_LISTEN_STATE)
  {
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
    UIM_MSG_ERR_1("The channel is not in 0x%x state",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             NULL,
             &tr_extra_param,
             index);
  }
  else
  {
    gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    if(estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified)
    {
      gen_result = GSTK_COMMAND_PERFORMED_WITH_MODIFICATION;
    }
    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
          estk_curr_inst_ptr->bip_channel_table[index].channel_id;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
  tr_extra_param.extra_param.open_ch_extra_param.ch_status.scws_ch_status = cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status;
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
          estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    estk_status = estk_bip_send_terminal_response(
                    cmd_ref_id,
                    GSTK_OPEN_CH_CNF,
                    gen_result,
                    NULL,
                    &tr_extra_param,
                    index);
  }
  return estk_status;
} /* estk_process_scws_open_ch_rsp */
/*===========================================================================

FUNCTION: estk_process_scws_close_ch_rsp

DESCRIPTION :
  This function processes the Close Ch response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_close_ch_rsp(
  const estk_cmd_type *cmd_ptr)
{
  uint32                              index = 0;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res   = 0;
  uint32                              cmd_ref_id = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_close_ch_rsp");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_close_ch_rsp: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH) ||
      (cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id !=
       estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&additional_info, 0x00, sizeof(additional_info));
  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
  addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  additional_info.additional_info_ptr = &addi_res;
  additional_info.length = 1;

  estk_curr_inst_ptr->bip_channel_table[index].status =
     (estk_channel_status_type)((uint32)cmd_ptr->cmd_data.scws_close_ch_rsp.ch_status + (uint32)ESTK_CH_SCWS_OFFSET);
  if(cmd_ptr->cmd_data.scws_close_ch_rsp.ch_status !=
    estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_state)
  {
    UIM_MSG_ERR_1("The channel is not in 0x%x state",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      estk_cache_additional_info(&additional_info);
      return ESTK_SUCCESS;
    }
    else
    {
      estk_bip_clean_up_ch_table(index);
      return estk_bip_send_terminal_response(cmd_ref_id,
               GSTK_CLOSE_CH_CNF,
               GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
               &additional_info,
               NULL,
               index);
    }
  }
  else
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      return ESTK_SUCCESS;
    }
    else
    {
      estk_bip_clean_up_ch_table(index);
      return estk_bip_send_terminal_response(cmd_ref_id,
               GSTK_CLOSE_CH_CNF,
               GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
               NULL,
               NULL,
               index);
    }
  }
} /* estk_process_scws_close_ch_rsp */

/*===========================================================================

FUNCTION: estk_process_scws_send_data_rsp

DESCRIPTION :
  This function processes the Send Data Response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_send_data_rsp(
  const estk_cmd_type *cmd_ptr)
{

  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res   = 0;
  uint32                              cmd_ref_id = 0;
  uint32                              index      = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_send_data_rsp");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_send_data_rsp.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_send_data_rsp: estk_curr_inst_ptr->bip_channel_table idx=0x%x, "
                 "channel_id=0x%x, "
                 "status=0x%x",
                 index,
                 estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                 estk_curr_inst_ptr->bip_channel_table[index].status);
  if ((ESTK_CH_CLOSED == estk_curr_inst_ptr->bip_channel_table[index].status) ||
      (cmd_ptr->cmd_data.scws_send_data_rsp.ch_id !=
       estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));

  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
  addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  additional_info.additional_info_ptr = &addi_res;
  additional_info.length = 1;
  tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
  tr_extra_param.present = TRUE;

  if(!cmd_ptr->cmd_data.scws_send_data_rsp.is_success)
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      estk_cache_additional_info(&additional_info);
      estk_curr_inst_ptr->extra_param = tr_extra_param;
    }
    else
    {
    /* Send error terminal response */
    (void)estk_bip_send_terminal_response(cmd_ref_id,
            GSTK_SEND_DATA_CNF,
            GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
            &additional_info,
            &tr_extra_param,
            index);
    }
    return ESTK_ERROR;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
       estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      estk_curr_inst_ptr->extra_param = tr_extra_param;
    }
    else
    {
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
             NULL,
             &tr_extra_param,
             index);
    }
    return ESTK_SUCCESS;
  }
} /* estk_process_scws_send_data_rsp */

/*===========================================================================

FUNCTION: estk_scws_clear_send_receive_buf

DESCRIPTION :
  This function clears the Tx/Rx buffers

PARAMETERS:
   ch_id : Channel id

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_send_receive_buf(
   uint8 ch_id)
{

  uint8 index;

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  index = ch_id - 1;

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Index is out or range 0x%x", index);
    return;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    /*Setting the receive buffer to 0*/
    memset(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
           0x00,
           (uint32)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL)
  {
    /*Freeing the SCWS overflow buffer*/
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
    estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = 0;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    /*Setting the send buffer to 0*/
    memset(estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
           0x00,
           estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
  }
} /* estk_scws_clear_send_receive_buf */

/*===========================================================================

FUNCTION: estk_scws_clear_receive_data_allowed

DESCRIPTION :
  This function sets receive data allowed to true

PARAMETERS:
  ch_id   [Output]: Channel ID
  slot_id [Output]: Slot ID

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_receive_data_allowed(
   uint8                  ch_id,
   gstk_slot_id_enum_type slot_id)
{

  uint8 index;

  UIM_MSG_HIGH_2("estk_scws_clear_receive_data_allowed, ch_id=0x%x, slot_id=0x%x",
                 ch_id, slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",slot_id);
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  index = ch_id - 1;

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Index is out or range 0x%x", index);
    return;
  }

  estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[index].receive_data_allowed = TRUE;
} /* estk_scws_clear_receive_data_allowed */

/*===========================================================================

FUNCTION: estk_notify_proactive_command

DESCRIPTION :
  This function allows GSTK to notify ESTK whenever a proactive command is
  received from the card

PARAMETERS:
  type_of_command : [Input] Type of proactive command
  slot_id         : [Input] Slot ID on which the proactive command is received

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void estk_notify_proactive_command(
  uint8                  type_of_command,
  gstk_slot_id_enum_type slot_id
)
{
  int32   index           = -1;
  uint32  mask            = 0;
  boolean mask_set_status = FALSE;

  UIM_MSG_HIGH_2("estk_notify_proactive_command, command=0x%x, slot_id=0x%x",
                 type_of_command, slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",slot_id);
    return;
  }

  if(estk_instances_ptr[(uint32)slot_id - 1] == NULL)
  {
    UIM_MSG_ERR_1("ESTK global instance for slot: 0x%x is invalid",slot_id);
    return;
  }

  /* If currently a BIP channel is kept open, but the subsequent CAT command
     is not an OPEN CHANNEL, the channel needs to be closed */
  if((type_of_command != GSTK_CMD_STK_END_OF_PROACTIVE_SES) &&
     (type_of_command != GSTK_CMD_STK_OPEN_CHANNEL))
  {
    index = estk_instances_ptr[(uint32)slot_id - 1]->ch_open_id;

    /* Check if a BIP channel is kept open */
    if (index != -1 &&
        estk_instances_ptr[(uint32)slot_id - 1]->keep_ch_open &&
        estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[index].keep_ch_open_timer_p != NULL &&
        (rex_get_timer(estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[index].keep_ch_open_timer_p) > 0))
    {
      UIM_MSG_HIGH_1("Channel index: 0x%x kept open, trigger channel clean-up",
                     index);

      ESTK_BIP_SET_SLOT_INDEX_MASK(
        estk_instances_ptr[(uint32)slot_id - 1]->slot_id, index, mask, mask_set_status);

      if(mask_set_status)
      {
        /* Clear the Keep channel open timer */
        (void)rex_clr_timer(
                estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[index].keep_ch_open_timer_p);

        /* Simulate a timer expiry callback in order to trigger a channel clean-up */
        estk_bip_keep_ch_open_timer_cb(mask);
      }
    }
  }
} /* estk_notify_proactive_command */

#endif /* FEATURE_ESTK */

