/*===========================================================================   

DESCRIPTION
  
Copyright (c) 2010-2016 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pdn_context.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:03 $
                     
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/15/15   sd      Block both Ifaces if APN is rejected by the network.
02/19/15   sd      Fix to avoid PPP resync for VSNCP cause code 9 or 11 if the
                   last active PDN is not in UP state.
12/15/14   sc      Added condition to check the throttle count for the iface along
                   with the iface ptr in configure_vsncp before assert
12/09/14   hr      To support ALWAYS_SHARE_PROFILE
09/05/14   jc      Modified posting of v4 and v6 iface down logic.
07/28/14   hr      Sim swap feature
07/18/14   hr      To support default APN
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
06/30/14   ms      Reset the throttle v4/v6 count to MAX value when it is 
                   increemented past MAX THROTTLE TIMERS COUNT.
06/23/14   sc      Posts iface_go_null for second iface in dual ip only if 
                   phys link is not in NULL state
05/26/14   vm      Fixed compilation errors
05/26/14   vm      Perform PPP resync for VSNCP cause code 9 and 11
04/05/14   ms      Klocwork fix.
02/04/14   ms      If v6 is Disabled,  post iface down indication for V6 iface directly.
11/26/13   sd      Re-added the check to verify if PDN is being cleaned up
                   properly.
11/14/13   vm      Fixed KW errors
10/28/13   sd      LLVM Compiler Warnings clean-up - 3GPP2
06/26/13   sc      Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
12/20/12   hr      KW errors cleanup
12/02/12   jz      Support for Operator PCO  
08/13/12   ss      Moving efs write operation to dsutil performed during 
                   bootup.
07/31/12   jee     To cleanup dual IP calls correctly
05/02/12   jz      Set ip_ver_capability according to profile specifies
05/06/12   jee     Fix to avoid tearing down of v4 & v6 calls together in HRPD
04/20/12   op      Added support to determine DNS v4 address request type
04/12/12   jee     To propagate correct call end reason from MH
02/27/12   op      Fix to handle invalid PDN level authentication parameters
02/24/12   jee     To handle subscription limited events correctly
02/17/12   ssh     Added ds_3gpp2_mpit_timer_stop()
02/10/12   jz      Support for ignoring the last PDN inactivity timer   
02/13/12   jz      Clear PDN failure count for PDN fallback
11/11/11   jee     To fix Arbitration issue with v4/v6 calls
10/28/11   ls      PDN throttle info notification through SYS IOCTL
10/27/11   op      Added support for PDN level authentication
09/28/11   var     Fix to start inactivity timer only for valid profile id
09/20/11   jee     Fix to handle new call when device manager is cleaning up
08/08/11   ash     Reset MPIT timeout upon LCP resync
08/11/11   ms      Made KW error fix in function ds707_pdncntx_get_iface_ptr_from_iface_tbl_index
07/21/11   vpk     Fix for closing other interfaces during DUN call bringup
06/06/11   ash     Added support for PPP Partial Context
05/05/11   op      Fix to start inactiviy timer for default eHRPD profile
03/31/11   op      Fix to use stored APN string length instead of
                   calling strlen function
03/15/11   mg      Global variable cleanup
03/14/11   mga     Merged changes to set the MTU accordingly
02/02/11   mga     Made changes to request DNS address accordingly
10/22/10   ssh     Start inactivity timer only for eHRPD
10/20/10   op      Added support for PDN throttling timers in profiles 
10/14/10   ss      Adding debug messages before ASSERT.
10/13/10   op      Migrated to MSG 2.0 macros
07/18/10   mg      PDN throttling feature for eHRPD
07/08/10   ssh     Posting command to DS on PDN Inactivity timer expiry
05/13/10   ssh     Added support for PDN Inactivity timer
03/12/10   sc      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include <stringl/stringl.h>

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#include "ds707_pdn_context.h"
#include "ds_ehrpd_pdn_mgr.h"
#include "ds707_pkt_mgr.h"
#include "ds3gcfgmgr.h"
#include "modem_mem.h"
#include "ds707_data_session_profile.h"
#include "ps_acl.h"
#include "data_err_suppr.h"
#include "ps_ifacei.h"
#include "ds707_roaming.h"
#include "ds707_devmgr.h"
#include "ds3gtimer.h"
#include "ds3gutil.h"
#ifdef FEATURE_HDR_QOS
#include "ds707_qosdefs.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "ds707_sec_pkt_mgr.h"
#include "ds707_pkt_mgri.h"
#include "ps_iface_defs.h"
#endif /* FEATURE_HDR_QOS */
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
#include "secapi.h"
#include "dsutil.h"
#include "ps_aclrules.h"
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */

#ifdef FEATURE_EHRPD
#include "ds707_sys_chg_hdlr.h"
#include "ps_sys_conf.h"
#include "ds_sys.h"

#ifdef FEATURE_DATA_PS_IPV6 
#include "ps_ifacei_utils.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /*FEATURE_EHRPD*/

#ifdef FEATURE_DATA_3GPP2_VS
#include "ds_3gpp2_vendor.h"
#endif /* FEATURE_DATA_3GPP2_VS */

#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

#include "ps_sys.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"

#include "ps_ppp_vsncp_3gpp2.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ip6_sm.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "nv_items.h"
#include "ds3gmmgsdiif.h"
#ifdef FEATURE_NIQ_EHRPD
#include "ds707_nw_init_qos_mgr.h"
#endif /* FEATURE_NIQ_EHRPD */

#ifdef FEATURE_DATA_IS707
#include "dsdctm.h"
#endif /* FEATURE_DATA_IS707 */


/*===========================================================================
                          VARIABLES AND MACRO DEFINITIONS
===========================================================================*/
#define DS707_PDNCNTXT_IFACE_STATE_IS_NOT_DOWN( state )          \
       (state == IFACE_COMING_UP   ||  \
       state == IFACE_CONFIGURING   || \
       state == IFACE_ROUTEABLE ||     \
       state == IFACE_UP ||            \
       state == IFACE_GOING_DOWN ||    \
       state == IFACE_LINGERING)

#ifdef FEATURE_DS_MTU_PCO_VSNCP
#define VSNCP_MTU_PCO_EFS_PATH "/nv/item_files/data/3gpp2/request_mtu_pco_in_vsncp"
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
/*---------------------------------------------------------------------------
  Packet call states.  Each instance is associated with a particular Iface
  and encapsulates multiple traffic channels, each associated with a 
  particular RLP.
---------------------------------------------------------------------------*/
static ds707_pdn_context_cb_ptr_type ds707_pdn_context_tbl[DS707_MAX_PDN_INSTANCES];

static ds707_iface_table_type       ds707_iface_tbl[DS707_MAX_IFACES];

/*---------------------------------------------------------------------------
  If ds707_pdncntx_disable_pdn_type_override(NV 67346) is set to TRUE, ip type     
  in VSNCP config_req will be set according to what is specified in the     
  requested profile, otherwise, always send v4v6 in VSNCP config req. 
---------------------------------------------------------------------------*/
static boolean ds707_pdncntx_disable_pdn_type_override = FALSE;
#ifdef FEATURE_EHRPD

static void ds707_pdnctx_pdn_down_ind
(
  void
);

#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
/*---------------------------------------------------------------------------
  Array of throttling info structure, to store throttling info for each PDN
---------------------------------------------------------------------------*/
static ds707_pdn_throttle_info_ptr_type ds707_pdn_throttle_info_tbl[DS707_PDNCNTX_MAX_EHRPD_APN];
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */

#ifdef FEATURE_EHRPD
/*---------------------------------------------------------------------------
  To store the default PDN information
---------------------------------------------------------------------------*/
typedef struct 
{  
  ds707_pdn_context_cb_type *default_pdn_cb_ptr;
  char                       default_apn[VSNCP_3GPP2_APN_MAX_VAL_LEN];
  uint8                      default_apn_len;
}ds707_pdncntx_default_apn_info_type;

static ds707_pdncntx_default_apn_info_type 
         *ds707_pdncntx_default_apn_info_ptr = NULL;
#endif /* FEATURE_EHRPD */

/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/
void ds707_pdncntx_reset_default_pdn(void);

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
/*===========================================================================
FUNCTION      DS707_IFACE_IN_USE_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface in use 
              command on the 707 iface.

PARAMETERS    ps_iface_type* : pointer to iface

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_pdncntx_iface_in_use_cmd_hdlr
(
  ps_iface_type*  iface_ptr
)
{
  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    DATA_ERR_FATAL("PS Iface ptr is invalid");
  }

  DATA_IS707_MSG3(MSG_LEGACY_HIGH,
                  "ds707_pdncntx_iface_in_use_cmd_hdlr for iface 0x%x:%d, "
                  "iface state: %d",
                  iface_ptr->name, iface_ptr->instance, 
                  ps_iface_state(iface_ptr));
  /*-------------------------------------------------------------------------
    If iface state is in UP state, return iface_up_ind
  -------------------------------------------------------------------------*/
  if(ps_iface_state(iface_ptr) == IFACE_UP)
  {
    ps_iface_up_ind(iface_ptr);
#ifdef FEATURE_DATA_IS707    
    ds707_pkt_mgr_check_coex_data_status(iface_ptr);
#endif /* FEATURE_DATA_IS707 */
  }
  return;
}

/*===========================================================================
FUNCTION      DS707_IFACE_OUT_USE_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface out use 
              command on the 707 iface.

PARAMETERS    ps_iface_type* : pointer to iface

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_pdncntx_iface_out_use_cmd_hdlr
(
  ds707_tear_down_cmd_info_type* tear_down_cmd_info
)
{
  ps_iface_type*             iface4_ptr = NULL;
  ps_iface_type*             iface6_ptr = NULL;
  boolean                    is_any_iface_in_use = FALSE;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
  int                        ret_val;
  int16                      ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (tear_down_cmd_info == NULL)
  {
    DATA_ERR_FATAL("tear_down_cmd_info ptr is invalid");
  }
  
  if (!PS_IFACE_IS_VALID(tear_down_cmd_info->iface_ptr))
  {
    DATA_ERR_FATAL("PS Iface ptr is invalid");
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "ds707_pdncntx_iface_out_use_cmd_hdlr for iface 0x%x:%d",
                  tear_down_cmd_info->iface_ptr->name, 
                  tear_down_cmd_info->iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Verify the iface state is not in DOWN state.
  -------------------------------------------------------------------------*/
  if(ps_iface_state(tear_down_cmd_info->iface_ptr)
     == IFACE_DOWN )
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "Invalid iface state %d",
                    ps_iface_state(tear_down_cmd_info->iface_ptr));
    return;
  }

  /*get the pdn context cb from iface*/
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      tear_down_cmd_info->iface_ptr);
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_ERR_FATAL("707 PDN context cb ptr is invalid");
  }

  /*-------------------------------------------------------------------------
    Check to see if all other ifaces in this pdn context are either down or
    out of use, if so, we need to do additional processing. 
  -------------------------------------------------------------------------*/
  /*SID TODO: SHOULD THIS BE IN A LOOP FOR ALL IFACES FOR THIS PDN???*/
  iface4_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
  if (PS_IFACE_IS_IN_USE(iface4_ptr))
  {
    is_any_iface_in_use = TRUE;
  }

#ifdef FEATURE_DATA_PS_IPV6
  iface6_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
  if (PS_IFACE_IS_IN_USE(iface6_ptr))
  {
    is_any_iface_in_use = TRUE;
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  if (!is_any_iface_in_use)
  {
    if (PS_IFACE_IS_VALID(iface4_ptr))
    {
      ret_val = ps_iface_active_tear_down_cmd(iface4_ptr,
                    &ps_errno, 
                    ((void *) &tear_down_cmd_info->user_data.tear_down_info));

      if(ret_val == 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Iface in DOWN state already");
      }
      else if(ps_errno != DS_EWOULDBLOCK)
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                        "Error while tearing down the iface. Err Code: %d",
                        ps_errno);
      }
    }

    if (PS_IFACE_IS_VALID(iface6_ptr))
    {
      ret_val = ps_iface_active_tear_down_cmd(iface6_ptr,
                     &ps_errno, 
                     ((void *) &tear_down_cmd_info->user_data.tear_down_info));

      if(ret_val == 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Iface in DOWN state already");
      }
      else if(ps_errno != DS_EWOULDBLOCK)
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                        "Error while tearing down the iface. Err Code: %d",
                        ps_errno);
      }
    }
  }/*NO IFACES IN USE*/
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_FREE_V6_IFACE

DESCRIPTION   This function frees the V6 iface of the pdn context cb. This 
              is needed if a dual IP call is attempted before session
              negotiation and it goes on hrpd system, then we need to free
              the V6 iface, if app request is V4.
              
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_free_v6_iface(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  ps_iface_type*          iface4_ptr = NULL;
  ps_iface_type*          iface6_ptr = NULL;  
  ds707_iface_table_type* iface_tbl_ptr = NULL;

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "ds707_pdncntx_free_v6_iface: pdn_context_cb_ptr 0x%x",
                   pdncntx_cb_ptr );

  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    iface4_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
    iface6_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

    if(PS_IFACE_IS_VALID(iface4_ptr))
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ds707_pdncntx_free_v6_iface: pdn_context_cb_ptr 0x%x "
                       "set V4 iface to passive", 
                       pdncntx_cb_ptr );

      PS_IFACE_SET_IS_ACTIVE(iface4_ptr, FALSE);
    }

    if(PS_IFACE_IS_VALID(iface6_ptr))
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ds707_pdncntx_free_v6_iface: pdn_context_cb_ptr 0x%x "
                       "set V6 iface to passive, free V6 iface", 
                       pdncntx_cb_ptr );

      PS_IFACE_SET_IS_ACTIVE(iface6_ptr, FALSE);
      iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface6_ptr);
      if (DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr))
      {
        iface_tbl_ptr->pdn_cntx_ptr = NULL;
      }
      ds707_pdncntx_set_v6_iface(pdncntx_cb_ptr, NULL);
    }
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_FREE_V4_IFACE

DESCRIPTION   This function frees the V4 iface of the pdn context cb. This 
              is needed if a dual IP call is attempted before session
              negotiation and it goes on hrpd system, then we need to free
              the V4 iface, if app request is V6.
              
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_free_v4_iface(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  ps_iface_type*          iface4_ptr = NULL;
  ps_iface_type*          iface6_ptr = NULL;  
  ds707_iface_table_type* iface_tbl_ptr = NULL;

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "ds707_pdncntx_free_v4_iface: pdn_context_cb_ptr 0x%x",
                   pdncntx_cb_ptr );

  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    iface4_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
    iface6_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

    if(PS_IFACE_IS_VALID(iface6_ptr))
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ds707_pdncntx_free_v4_iface: pdn_context_cb_ptr 0x%x "
                       "set V6 iface to passive", 
                       pdncntx_cb_ptr );

      PS_IFACE_SET_IS_ACTIVE(iface6_ptr, FALSE);
    }

    if(PS_IFACE_IS_VALID(iface4_ptr))
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ds707_pdncntx_free_v4_iface: pdn_context_cb_ptr 0x%x "
                       "set V4 iface to passive, free V4 iface", 
                       pdncntx_cb_ptr );

      PS_IFACE_SET_IS_ACTIVE(iface4_ptr, FALSE);
      iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface4_ptr);
      if (DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr))
      {
        iface_tbl_ptr->pdn_cntx_ptr = NULL;
      }
      ds707_pdncntx_set_v4_iface(pdncntx_cb_ptr, NULL);
    }
  }
}
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */

/*===========================================================================

FUNCTION DSSNETI_STORE_ENCODED_APN

DESCRIPTION
  This function encodes the apn name contained in apn_src
  and stores it in apn_dest.


PARAMETERS

  apn_dest:   Destination to store the encoded apn.

  apn_src:    Uncoded APN name.
 
  pdp_addr_str_len: Length of APN name.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the encoded APN

SIDE EFFECTS

===========================================================================*/
uint8 dssneti_store_encoded_apn
(
  byte  *apn_dest,
  byte  *apn_src,
  uint32 pdp_addr_str_len
)
{
  uint8  apn_encoded_length,label_length;
  byte  *label_ptr,*label_len_ptr,*pdp_addr_str_ptr;
  uint32 apn_len_parsed;

  if(apn_dest == NULL)
  {
    DATA_ERR_FATAL("APN dest ptr is NULL");
    return 0;
  }

  /*-------------------------------------------------------------------------
    If the apn is a null apn, set the first byte of the destination apn to 0
  -------------------------------------------------------------------------*/
  if(pdp_addr_str_len == 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Null APN");
    apn_dest[0] = 0;
    return 0;
  }

  if(( pdp_addr_str_len + 1 ) > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN  )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "APN:larger than space allowed:not including");
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, "APN len:%d,MAX space to encode:%d",
                    pdp_addr_str_len, DS_VSNCP_3GPP2_APN_MAX_VAL_LEN);
    apn_dest[0] = 0;
    return 0;
  }

  /*-------------------------------------------------------------------------
    We have a valid APN.
    Check if we have room to encode the APN first. Since APN is specified as
    label1.label2.label3 (e.g www.ya-hoo.com ), and encoded as
    <len1><label1><len2><label2><len3L<label3> (3www6ya-hoo3com)
    the total length required to encode will be one more than the
    original length of the APN string
  -------------------------------------------------------------------------*/

#define LABEL_DELIMITOR 0x2e

  /*-------------------------------------------------------------------------
    Encode APN as per encoding rules defined in TS23.003 section 9.1
    The APN consists of one or more labels. Each label is coded as one octet
    length field followed by that number of octets coded as 8 bit ASCII chars

    label_len_ptr is used to point to the byte where the length of the
    current label must be stored. It is initialized to the start of the apn
    storage area.It is set to be one byte before the label starts.
    label_ptr is used to encode each label. It is initialized to the start of
    the apn storage area + 1.
    apn_encoded_length is used to track the total number of bytes encoded
    It initilized to '1' to account for the label length of the first label
  -------------------------------------------------------------------------*/
  label_len_ptr = &apn_dest[0];
  label_ptr = &apn_dest[1];
  label_length  = 0;
  apn_encoded_length  = 1;

  /*-------------------------------------------------------------------------
    Get the total length of the APN string and initiliaze the bytes parsed to
    to zero.
    pdp_addr_str_ptr tracks the current byte which is being parsed in the
    APN. Initilize it to the start of the APN string
  -------------------------------------------------------------------------*/
  apn_len_parsed = 0;
  pdp_addr_str_ptr = (byte*)&apn_src[0];

  /*-------------------------------------------------------------------------
    Now do the encoding while parsing the APN string. If we detect an error,
    we abort encoding and return as though NO apn is included.
  -------------------------------------------------------------------------*/
  for(apn_len_parsed = 0;
      apn_len_parsed < pdp_addr_str_len;
      apn_len_parsed++,pdp_addr_str_ptr++)
  {
    if( *pdp_addr_str_ptr == LABEL_DELIMITOR )
    {
      /*---------------------------------------------------------------------
        At the end of the current label. Encode the label length and reset
        the fields for the next label
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Encoding APN: label len:%d",
                      label_length);
      *label_len_ptr = label_length;
      /*---------------------------------------------------------------------
        The length of the next label starts at the end of the current label.
        and label_ptr starts at one byte off the label_len_ptr
        Increment apn_encoded_length by 1 to account for the storage reqd for
        the legnth of the next label
      ---------------------------------------------------------------------*/
      label_len_ptr = label_ptr;
      label_ptr++;
      apn_encoded_length++;
      label_length  = 0;
    }
    else
    {
      /*---------------------------------------------------------------------
        Encode the character and keep track of the length encoded so far
        increment the current label's ptr
        increment the length encoded so far
        increment the total space used in encoding all labels till now
      ---------------------------------------------------------------------*/
      *label_ptr = *pdp_addr_str_ptr ;
      label_ptr++;
      label_length++;
      apn_encoded_length++;
    }

  }
  if(apn_encoded_length > 1 )
  {
    /*-----------------------------------------------------------------------
      Tie of the length of the last label encoded if we had any
    -----------------------------------------------------------------------*/
    *label_len_ptr = label_length;
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Encoding APN: label len:%d",label_length);


  }
  return apn_encoded_length;
}

#ifdef FEATURE_EHRPD
/*===========================================================================

FUNCTION DS707_PDNCNTX_DECODE_APN

DESCRIPTION
  This function decodes the apn name contained in apn_src
  and stores it in apn_dest.


PARAMETERS

  apn_dest:   Destination to store the decoded apn.

  apn_src:    Encoded APN name.
 
  pdp_addr_str_len: Length of APN name.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the decoded APN

SIDE EFFECTS

===========================================================================*/
uint8 ds707_pdncntx_decode_apn
(
  char  *apn_src,
  uint32 apn_len_encoded,
  char  *apn_dest
)
{
  char  *label_ptr, *pdp_addr_str_ptr;
  uint8 label_len, apn_len_parsed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if(apn_dest == NULL)
  {
    DATA_ERR_FATAL("APN dest ptr is NULL");
    return 0;
  }

  /*-------------------------------------------------------------------------
    If the apn is a null apn, set the first byte of the destination apn to 0
  -------------------------------------------------------------------------*/
  if(apn_len_encoded == 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Null APN");
    apn_dest[0] = 0;
    return 0;
  }

  /*-------------------------------------------------------------------------
    Decode APN as per encoding rules defined in TS23.003 section 9.1
    The APN consists of one or more labels. Each label is coded as one octet
    length field followed by that number of octets coded as 8 bit ASCII chars
  -------------------------------------------------------------------------*/
  label_len = apn_src[0];
  label_ptr = &apn_src[1];
  pdp_addr_str_ptr = apn_dest;

  /*-------------------------------------------------------------------------
    Now do the decoding while parsing the APN string.
  -------------------------------------------------------------------------*/
  for ( apn_len_parsed = 0;
        apn_len_parsed < apn_len_encoded;
        apn_len_parsed++, pdp_addr_str_ptr++, label_ptr++ )
  {
    if ( label_len == 0 )
    {
      if ( *label_ptr == 0)
      {
        *pdp_addr_str_ptr = 0x0;
        break;
      }
      else
      {
        label_len = *label_ptr;
        *pdp_addr_str_ptr = LABEL_DELIMITOR;
      }
    }
    else
    {
      *pdp_addr_str_ptr = *label_ptr;
      label_len--;
    }
  }

  if ( (apn_len_parsed != apn_len_encoded - 1) ||
       (label_len !=0) )
  {
    /* parse error reset */
    apn_dest[0] = 0x0;
    apn_len_parsed = 0;
  }

  return apn_len_parsed;
} /* ds707_pdncntx_decode_apn */

/*===========================================================================
FUNCTION DS707_PDNCNTX_REPORT_DEFAULT_APN_CHANGE

DESCRIPTION
  This function reports the default APN change to PS framework
  
PARAMETERS  
  
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_pdncntx_report_default_apn_change
(
   void
)
{
  int16 dss_errno = 0;
  ps_sys_null_resolved_apn_info_type resolved_apn_info;
  ds3geventmgr_filter_type           eventmgr_input;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resolved_apn_info, 0, sizeof(ps_sys_null_resolved_apn_info_type));

  if ( ds707_pdncntx_default_apn_info_ptr != NULL )
  {
    memscpy(resolved_apn_info.apn_name,
            DS_SYS_MAX_APN_LEN,
            ds707_pdncntx_default_apn_info_ptr->default_apn,
            ds707_pdncntx_default_apn_info_ptr->default_apn_len);

    resolved_apn_info.apn_length =
      ds707_pdncntx_default_apn_info_ptr->default_apn_len;
  }

  if (0 > ps_sys_conf_set_ex(PS_SYS_TECH_3GPP2,
                             PS_SYS_CONF_NULL_RESOLVED_APN,
                             PS_SYS_DEFAULT_SUBS,
                             (void*)&resolved_apn_info,
                             &dss_errno))
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sending resolved APN info failed");
  }
  memset( (void*)&eventmgr_input, 
          0, 
          sizeof(ds3geventmgr_filter_type ));
  eventmgr_input.tech = PS_SYS_TECH_3GPP2;
  eventmgr_input.ps_subs_id = PS_SYS_PRIMARY_SUBS;
  if( TRUE != ds3geventmgr_set_event_info(DS3GEVENTMGR_NULL_RESOLVED_APN_EV,
                              (void *)&resolved_apn_info,
                              &eventmgr_input))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Failed to generate DS3GMGR NULL RESOLVED APN");  
  }
  return;
} /* ds707_pdncntx_report_default_apn_change */
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
/*===========================================================================
FUNCTION     DS707_GET_THROTTLE_INFO_TYPE_FROM_INSTANCE

DESCRIPTION   Given the throttle info table index, returns that instance 
              pointer 

PARAMETERS    Index to pdn cb table

RETURN VALUE  ds707_pdn_throttle_info_type *
===========================================================================*/
static ds707_pdn_throttle_info_type* ds707_get_throttle_info_type_from_instance(
  uint8 x
)
{
  return ((x < DS707_PDNCNTX_MAX_EHRPD_APN) ? (ds707_pdn_throttle_info_tbl[x].ds707_pdn_throttle_info_ptr): NULL);
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CLEAR_THROTTLE_INFO

DESCRIPTION   This function clears all the fields of the throttle info ptr 
              passed to it. This is called when both the ipv4 count and ipv6
              count is cleared.

PARAMETERS    ds707_pdn_throttle_info_type* : pointer to throttle info type

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
static void ds707_pdncntx_free_pdn_throttle_tbl_entry(
  ds707_pdn_throttle_info_type* throttle_info_ptr
)
{
  uint8   index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-----------------------------------------------------------------------
    Check if throttle_info_ptr is valid
  -----------------------------------------------------------------------*/
  if(!DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "throttle info ptr passed is NULL" );
    return; 
  }  

  /*-------------------------------------------------------------------------
    Find the index of the PDN throttle info pointer
  -------------------------------------------------------------------------*/
  for (index = 0; index < DS707_PDNCNTX_MAX_EHRPD_APN; index++)
  {
    if(ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr == throttle_info_ptr)
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, 
                      "free PDN throttle memory of ptr at %d index", index);
      break;
    }
  }

  if ( index == DS707_PDNCNTX_MAX_EHRPD_APN )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Cannot find PDN throttle ptr in pointer table");
  }
  else
  {
    /*-------------------------------------------------------------------------
      Free the throttle timers for this PDN
    -------------------------------------------------------------------------*/
    rex_undef_timer(&(ds707_pdn_throttle_info_tbl[index].\
                    ds707_pdn_throttle_info_ptr->ipv4_throttle_info.throttle_timer));
    
    rex_undef_timer(&(ds707_pdn_throttle_info_tbl[index].\
                    ds707_pdn_throttle_info_ptr->ipv6_throttle_info.throttle_timer));
    
    /*-------------------------------------------------------------------------
      Free PDN throttle info memory.
    -------------------------------------------------------------------------*/
    modem_mem_free((void*)(ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr),
                   MODEM_MEM_CLIENT_DATA_CRIT);
    ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr = NULL;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_ALLOCATE_PDN_THROTTLE_TBL_ENTRY

DESCRIPTION   This function allocates a free entry from the pdn throttle 
              table. 

PARAMETERS    ds707_data_session_profile_id: profile id for which throttle 
                                             tbl entry is allocated 

RETURN VALUE  ds707_pdn_throttle_info_type* : free entry from the table, 
                                              NULL otherwise 

SIDE EFFECTS  None
===========================================================================*/
static ds707_pdn_throttle_info_type* ds707_pdncntx_allocate_pdn_throttle_tbl_entry(
  ds707_data_session_profile_info_type* data_sess_profile_info_ptr
)
{
  uint8 index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    Sanity Check - should not ever happen
  -----------------------------------------------------------------------*/
  if (data_sess_profile_info_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Invalid profile passed for throttle tbl allocation");
    return NULL;
  }

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "In function ds707_pdncntx_allocate_pdn_throttle_tbl_entry for profile id %d",
                   data_sess_profile_info_ptr->data_session_profile_id );

  /*-----------------------------------------------------------------------
    Find free entry in the table
  -----------------------------------------------------------------------*/
  for (index = 0; index < DS707_PDNCNTX_MAX_EHRPD_APN; index++)
  {
    if( !ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr )
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, 
                      "allocate throttling info block to ptr at %d index", 
                       index );
      break;
    }
  }

  if( index == DS707_PDNCNTX_MAX_EHRPD_APN )
  {
    /*-------------------------------------------------------------------------
      If no entry in the table is free, return NULL
    -------------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "Max PDN throttled - returning NULL");
    return NULL;
  }
  else
  {
    /*-------------------------------------------------------------------------
      Allocate memory to the free pointer.
    -------------------------------------------------------------------------*/
    ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr = 
       (ds707_pdn_throttle_info_type *)modem_mem_alloc(sizeof(ds707_pdn_throttle_info_type), 
                                                       MODEM_MEM_CLIENT_DATA_CRIT);
    if(NULL == ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr )
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "memory allocation failed");
      return NULL;
  }

    /*-------------------------------------------------------------------------
      Initialize the PDN throttle info block
    -------------------------------------------------------------------------*/
    memset((void *)(ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr),
           0, sizeof(ds707_pdn_throttle_info_type));

    ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr->apn_string_len = 
      data_sess_profile_info_ptr->apn_string_len;
    memscpy( (void *)(ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr->apn_string), 
             DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
             (void *)(data_sess_profile_info_ptr->apn_string), 
             data_sess_profile_info_ptr->apn_string_len );
    ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr->this_throttle_info_ptr = 
      ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr;

    rex_def_timer_ex( &(ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr->ipv4_throttle_info.throttle_timer), 
                      ds707_pdncntx_v4_throttling_timer_cb, 
                      (unsigned long) (ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr));

    rex_def_timer_ex( &(ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr->ipv6_throttle_info.throttle_timer), 
                      ds707_pdncntx_v6_throttling_timer_cb, 
                      (unsigned long) (ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr));

    DATA_IS707_MSG1(MSG_LEGACY_MED, "return throttling info ptr 0x%x",
                    ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr);

    return (ds707_pdn_throttle_info_tbl[index].ds707_pdn_throttle_info_ptr);
  }
}

#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_INIT

DESCRIPTION   This function inits a PDN context cb 

PARAMETERS    pointer PDN cntx 

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_pdncntx_init(ds707_pdn_context_cb_type* pdncntx_cb_ptr)
{
  ds707_data_session_info_type *data_session_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    Initialize the PPP resync flag for the iface and initialize the
    resync completion call back function to NULL
  -----------------------------------------------------------------------*/
  pdncntx_cb_ptr->ppp_resync_lock = FALSE;
    
  /*-----------------------------------------------------------------------
    Get the pointer to data_session_info structure 
  -----------------------------------------------------------------------*/
  data_session_info_ptr = &(pdncntx_cb_ptr->data_profile);
    
  /*---------------------------------------------------------------------
    Initialize the IFACE_CLIENT_DATA_INFO
  ---------------------------------------------------------------------*/
  if ( ds707_data_session_info_init( data_session_info_ptr)< 0 ) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_FATAL,
                     "Could not initialize data session info" );
  }

#ifdef FEATURE_EHRPD
  pdncntx_cb_ptr->ncp_handle = NULL;
  pdncntx_cb_ptr->ncp_started = FALSE;
  pdncntx_cb_ptr->ncp_resync_started = FALSE;
  pdncntx_cb_ptr->call_bringup_in_tunnel_mode = FALSE;
  rex_def_timer_ex( &(pdncntx_cb_ptr->inactivity_timer),
                    ds707_pdncntx_inactivity_timeout_cb, 
                    (unsigned long)pdncntx_cb_ptr);
#endif    
    
  /*-------------------------------------------------------------------------
    Mark the iface tbl entries to NULL
  -------------------------------------------------------------------------*/
  pdncntx_cb_ptr->ds707_iface_tbl[DS707_PDN_CONTEXT_IPV4_IFACE] = NULL;
  pdncntx_cb_ptr->ds707_iface_tbl[DS707_PDN_CONTEXT_IPV6_IFACE] = NULL;

#ifdef FEATURE_EPC_HANDOFF
  /*-------------------------------------------------------------------------
    Initialize handover information
  -------------------------------------------------------------------------*/
  pdncntx_cb_ptr->handover_ip_config_handle = NULL;
  pdncntx_cb_ptr->handover_attach = FALSE;  
#endif
#ifdef FEATURE_CALL_THROTTLE
  /*-------------------------------------------------------------------------
    Set the dctm indication posted flag to FALSE. 
  -------------------------------------------------------------------------*/
  pdncntx_cb_ptr->dctm_ind_posted = FALSE; 
#endif
#ifdef FEATURE_DATA_FALLBACK_LIST
  ds707_pdncntx_set_pdn_fb_cnt_added(pdncntx_cb_ptr,FALSE);
#endif /*FEATURE_DATA_FALLBACK_LIST*/
}

/*===========================================================================
FUNTION       DS707_PDNCNTX_FREE_PDNCNTX_CB

DESCRIPTION   This function frees the PDN context control block memory.       
              Called from PDN context cleanup function. 

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdncntx_cb in question.	
              boolean	

RETURN VALUE  None 
 
SIDE EFFECTS  None 
===========================================================================*/
static void ds707_pdncntx_free_pdncntx_cb(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  uint8   index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(!pdncntx_cb_ptr)
  {
	DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
					"ds707_pdncntx_free_pdncntx_cb invalid pdn cb ptr");
	return;
  }

  /*-------------------------------------------------------------------------
    Find the index of the PDN context cb pointer
  -------------------------------------------------------------------------*/
  for (index = 0; index < DS707_MAX_PDN_INSTANCES; index++)
  {
    if(ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr == pdncntx_cb_ptr)
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED, "free PDN cb 0x%x at %d index",
                      pdncntx_cb_ptr, index);
      break;
    }
  }

  if ( index == DS707_MAX_PDN_INSTANCES )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "Cannot find PDN cb 0x%x in pointer table",
                    pdncntx_cb_ptr);
  }
  else
  {
    /*-------------------------------------------------------------------------
      Free PDN context cb memory.
    -------------------------------------------------------------------------*/
    modem_mem_free((void*)(ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr),
                   MODEM_MEM_CLIENT_DATA_CRIT);
    ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr = NULL;
  }
}

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
#ifdef FEATURE_DATA_FALLBACK_LIST
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_PDN_FB_CNT_ADDED

DESCRIPTION   Set the pdn_fb_cnt_added flag, to indicated whether a pdn fallback 
              count has been increased during this PDN session. 
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb 
              boolean: to set the pdn_fb_cnt_added flag 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_pdn_fb_cnt_added(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  boolean                    pdn_fb_cnt_added
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED,
                     "ds707_pdncntx_set_pdn_fb_cnt_added to %d", 
                     pdn_fb_cnt_added );
    pdncntx_cb_ptr->pdn_fb_cnt_added = pdn_fb_cnt_added;
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_set_pdn_fb_cnt_added: Invalid pdncntx_cb_ptr: 0x%x",
                     pdncntx_cb_ptr );
  }
}
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PDN_FB_CNT_ADDED

DESCRIPTION   Set the pdn_fb_cnt_added flag, to indicated whether a pdn fallback 
              count has been increased during this PDN session. 
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb 

RETURN VALUE  TRUE: fallback count has been added.   
              FALSE: otherwise 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_pdn_fb_cnt_added(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED,
                     "ds707_pdncntx_get_pdn_fb_cnt_added: return %d",
                     pdncntx_cb_ptr->pdn_fb_cnt_added );
    return (pdncntx_cb_ptr->pdn_fb_cnt_added);
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_get_pdn_fb_cnt_added: "
                     "Invalid pdncntx_cb_ptr: 0x%x",
                     pdncntx_cb_ptr );
    return FALSE;
  }
}
#endif /*FEATURE_DATA_FALLBACK_LIST*/
/*===========================================================================
FUNCTION      DS707_GET_PDNCNTX_CB_FROM_INSTANCE 

DESCRIPTION   Given the pdn cb table index, returns the pdn cb tbl pointer 

PARAMETERS    Index to pdn context control block table

RETURN VALUE  pdn_context_cb ptr

SIDE EFFECTS  None
===========================================================================*/
ds707_pdn_context_cb_type* ds707_get_pdncntx_cb_from_instance(
  uint8 x
)
{
  return ((x < DS707_MAX_PDN_INSTANCES) ? (ds707_pdn_context_tbl[x].ds707_pdn_context_cb_ptr): NULL);
}

/*===========================================================================
FUNCTION      DS707_GET_IFACE_TBL_FROM_INSTANCE

DESCRIPTION   Given the iface tbl index, returns the iface tbl pointer. 

PARAMETERS    Index to iface table

RETURN VALUE  Iface_tbl_ptr

SIDE EFFECTS  None
===========================================================================*/
ds707_iface_table_type* ds707_get_iface_tbl_from_instance(
  uint8 x
)
{
  return ((x < DS707_MAX_IFACES) ? (&(ds707_iface_tbl[x])): NULL);
}

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_THROTTLE_INFO_PTR

DESCRIPTION   This function checks if the APN passed through the profile has 
              valid throttle info table entry. If it is, the throttle table
              entry ptr is returned.

PARAMETERS    ds707_data_session_profile_id: profile id.

RETURN VALUE  ds707_pdn_throttle_info_type* : if pdn is throttled then the 
                                              ptr to the table entry,
                                              NULL otherwise 

SIDE EFFECTS  None
===========================================================================*/
ds707_pdn_throttle_info_type* ds707_pdncntx_get_throttle_info_ptr( 
  ds707_data_session_profile_info_type  *data_sess_profile_info_ptr
)
{
  uint8 index = 0;
  ds707_pdn_throttle_info_type*         throttle_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (data_sess_profile_info_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Invalid profile passed for throttle check" );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check if a throttle table entry is in use, if yes, compare the APN info
  -------------------------------------------------------------------------*/
  for (index = 0; index < DS707_PDNCNTX_MAX_EHRPD_APN; index++)
  {
    throttle_info_ptr = ds707_get_throttle_info_type_from_instance(index);
    if( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
    {
      if ( throttle_info_ptr->apn_string_len ==
           data_sess_profile_info_ptr->apn_string_len ) 
      {
        if (0 == strncmp( throttle_info_ptr->apn_string,
                          data_sess_profile_info_ptr->apn_string,
                          throttle_info_ptr->apn_string_len ) )
        {
          return throttle_info_ptr;
        }        
      }
    }
  }

  //index == DS707_PDNCNTX_MAX_EHRPD_APN
  /*-------------------------------------------------------------------------
    If no entry found in the table with this APN info, return NULL
  -------------------------------------------------------------------------*/
  return NULL;
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V4_THROTTLING_TIMER_CB

DESCRIPTION   This function handles expiry of v4 throttling timer for a PDN
               
PARAMETERS    user_data ptr : pointer to throttle info type

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_v4_throttling_timer_cb(
  unsigned long user_data
)
{
  ds707_pdn_throttle_info_type* throttle_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Get throttle_info_ptr from user data, and check is its valid. Clear
    the timer entry
  -------------------------------------------------------------------------*/  
  throttle_info_ptr = (ds707_pdn_throttle_info_type *)user_data; 

  if(!DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "v4_throttling_timer_cb: throttle info ptr passed is NULL" );
    return; //ASSERT ?
  }
  
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "v4 timer expired for throttle info ptr 0x%x",
                   throttle_info_ptr );

  (void)rex_clr_timer( &(throttle_info_ptr->ipv4_throttle_info.throttle_timer) );
  /* 
   * generate system pdn throttle info since throttle info is updated 
   */
  ds707_pdncntx_sys_pdn_throttle_info_notification();
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V6_THROTTLE_TIMER_CB

DESCRIPTION   This function handles expiry of v6 throttling timer for a PDN

PARAMETERS    user_data ptr : pointer to throttle info type

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_v6_throttling_timer_cb(
  unsigned long user_data
)
{
  ds707_pdn_throttle_info_type* throttle_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Get throttle_info_ptr from user data, and check is its valid. Clear
    the timer entry
  -------------------------------------------------------------------------*/  
  throttle_info_ptr = (ds707_pdn_throttle_info_type *)user_data; 

  if(!DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "v6_throttling_timer_cb: throttle info ptr passed is NULL" );
    return; //ASSERT ?
  }
  
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "v6 timer expired for throttle info ptr 0x%x",
                   throttle_info_ptr );

  (void)rex_clr_timer( &(throttle_info_ptr->ipv6_throttle_info.throttle_timer) );
  /* 
   * generate system pdn throttle info since throttle info is updated 
   */
  ds707_pdncntx_sys_pdn_throttle_info_notification();
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CLEAR_THROTTLE_COUNT

DESCRIPTION   This function clears the count of that iptype to 0. This is 
              called when there is a successful data connection on that 
              iptype

PARAMETERS    ds707_pdn_throttle_info_type* : pointer to throttle info type
              ds707_pdncntx_iptype_enum_type: ip type (v4/v6)

RETURN VALUE  None 
 
SIDE EFFECTS  None 
===========================================================================*/
void ds707_pdncntx_clear_throttle_count( 
  ds707_pdn_throttle_info_type*  throttle_info_ptr,
  ds707_pdncntx_iptype_enum_type ip_type
)
{
  /*-------------------------------------------------------------------------
    Check if throttle_info_ptr is valid
  -------------------------------------------------------------------------*/  
  if(!DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_clear_throttle_count: "
                     "throttle info ptr passed is NULL" );
    return; 
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "ds707_pdncntx_clear_throttle_count for IP type %d, "
                   "throttle_info_ptr: 0x%x ", 
                   ip_type, throttle_info_ptr );
  switch ( ip_type )
  {
    case DS707_PDNCNTX_IP_TYPE_V4:
    {
      /*-----------------------------------------------------------------------
        set ipv4_throttle_info.failure_count to 0 and throttle reason
        to PS_NET_DOWN_REASON_NOT_SPECIFIED. 
      -----------------------------------------------------------------------*/  
      throttle_info_ptr->ipv4_throttle_info.failure_count = 0;
      (void)rex_clr_timer( &(throttle_info_ptr->ipv4_throttle_info.throttle_timer) );

      throttle_info_ptr->ipv4_throttle_info.throttle_reason 
                                                  = PS_NET_DOWN_REASON_NOT_SPECIFIED;
      throttle_info_ptr->ipv4_throttle_info.throttled_in_tunnel_mode = FALSE;

      break;
    }
 
    case DS707_PDNCNTX_IP_TYPE_V6:
    {
      /*-----------------------------------------------------------------------
        set ipv6_throttle_info.failure_count to 0 and throttle reason
        to PS_NET_DOWN_REASON_NOT_SPECIFIED. 
      -----------------------------------------------------------------------*/ 
      throttle_info_ptr->ipv6_throttle_info.failure_count = 0;
      (void)rex_clr_timer( &(throttle_info_ptr->ipv6_throttle_info.throttle_timer) );

      throttle_info_ptr->ipv6_throttle_info.throttle_reason 
                                          = PS_NET_DOWN_REASON_NOT_SPECIFIED;
      throttle_info_ptr->ipv6_throttle_info.throttled_in_tunnel_mode = FALSE;

      break;
    }

    default:

      ASSERT(0);
  } /* switch (ip_type) */ 

  /*-----------------------------------------------------------------------     
     If IPv4 and IPv6 failure count are 0, then free the throttle info ptr
  -----------------------------------------------------------------------*/
  if( (throttle_info_ptr->ipv4_throttle_info.failure_count == 0) && 
      (throttle_info_ptr->ipv6_throttle_info.failure_count == 0) )
  {
    ds707_pdncntx_free_pdn_throttle_tbl_entry(throttle_info_ptr);
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_THROTTLE_PDN_IPTYPE

DESCRIPTION   This function increments the failure count for that ip type, 
              stores the fail reason, gets the timer value depending
              on the throttle_type, and starts the throttling timer.

PARAMETERS    ds707_data_session_profile_id: profile id 
              ds707_pdncntx_iptype_enum_type: ip_type
              ds707_pdncntx_throttle_enum_type:  throttle type (disallow/failure)
              ps_iface_net_down_reason_type: fail_reason (reason for throttling)

RETURN VALUE  None 
 
SIDE EFFECTS  None 
===========================================================================*/
void ds707_pdncntx_throttle_pdn_iptype( 
  ds707_data_session_profile_id  data_sess_profile_id,
  ds707_pdncntx_iptype_enum_type ip_type,
  ds707_pdncntx_throttle_enum_type throttle_type,
  ps_iface_net_down_reason_type fail_reason 
) 
{
  ds707_pdn_throttle_info_type*  throttle_info_ptr = NULL;
  uint32              throttling_timer_value = 0;
  uint8               random_value = 0;
  secerrno_enum_type  sec_errno = E_FAILURE;
  uint8               ip_count = 0;
  boolean             in_tunnel_mode = FALSE;
  boolean             call_bringup_in_tunnel_mode = FALSE;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
  ds707_data_session_profile_info_type *profile_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( FALSE == ds707_data_session_get_profile(data_sess_profile_id,
                                               ACTIVE_SUBSCRIPTION_1,
                                               &profile_info_ptr) )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,  
                     "Invalid profile ID %d passed in "
                     "ds707_pdncntx_throttle_pdn_iptype",
                     data_sess_profile_id );
    return;
  }

  DATA_IS707_MSG3( MSG_LEGACY_HIGH,  
                   "In function ds707_pdncntx_throttle_pdn_iptype: profile_id %d"
                   "ip_type %d throttle_type %d",
                   data_sess_profile_id, ip_type, throttle_type );

  /*-----------------------------------------------------------------------
    Retrieve the associated PDN cb for this requesting profile ID.
  -----------------------------------------------------------------------*/
  pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_req_profile_apn(profile_info_ptr);

  if(DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    call_bringup_in_tunnel_mode = pdncntx_cb_ptr->call_bringup_in_tunnel_mode;
  }

#ifdef FEATURE_DATA_OPTHO
  /*----------------------------------------------------------------------- 
  Check the current S101 State
  -----------------------------------------------------------------------*/
  if (DS707_S101_SM_NON_TUNNEL != ds707_s101_query_sm_state())
  {
    in_tunnel_mode = TRUE;
  }
#endif /* FEATURE_DATA_OPTHO */

  if (TRUE == call_bringup_in_tunnel_mode &&
      FALSE == in_tunnel_mode)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,  
                     "Call bringup was attempted in tunnel mode, however "
                     "system is no longer in tunnel mode. PDN not throttled");
    return;

  }

  /*-----------------------------------------------------------------------
    Check if this PDN is throttled, if not allocate a new throttle table
    entry for this PDN.
  -----------------------------------------------------------------------*/
  throttle_info_ptr = ds707_pdncntx_get_throttle_info_ptr( profile_info_ptr );
  if ( !DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
  {
    throttle_info_ptr = ds707_pdncntx_allocate_pdn_throttle_tbl_entry( 
                                                         profile_info_ptr );
    if (!DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)) 
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,  
                       "could not allocate throttle table entry - PDN cannot be throttled" );
      return;
    }
  }

  /*-----------------------------------------------------------------------
    For the specified IP Type:
   
    1. Check the ip failure count, if its less than the max timers (6 values 
    of timers are defined for 6 failures), increment the count for this 
    failure and Get the throttle timer value for this count depending
    on the throttle_type.
   
    2. Set the throttle reason to the fail reason passed in by the caller.
   
    3. Check if the PDN is throttled while attempting a tunnel call
       (Optho case) and set the throttled_in_tunnel_mode flag accordingly.
  -----------------------------------------------------------------------*/
  if( ip_type == DS707_PDNCNTX_IP_TYPE_V4 )
  {
    if( !(throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE &&
       	  profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6 &&
          throttle_info_ptr->ipv4_throttle_info.throttle_type == 
                                          DS707_PDNCNTX_THROTTLE_TYPE_INFINITE)
      )
    {
      throttle_info_ptr->ipv4_throttle_info.failure_count++;
      if( throttle_info_ptr->ipv4_throttle_info.failure_count <= 
                                                      PDN_THROTTLE_MAX_TIMERS )
      {
        ip_count = throttle_info_ptr->ipv4_throttle_info.failure_count;
      }
      else
      {
        ip_count = PDN_THROTTLE_MAX_TIMERS;
        throttle_info_ptr->ipv4_throttle_info.failure_count = ip_count;
      }
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, "v4 throttle count %d",
                       throttle_info_ptr->ipv4_throttle_info.failure_count );
  
      throttle_info_ptr->ipv4_throttle_info.throttle_type = throttle_type;
  
      /* Also store the failure reason in the throttle table and if
         throttling while in tunnel Mode */
      throttle_info_ptr->ipv4_throttle_info.throttle_reason = fail_reason;

      throttle_info_ptr->ipv4_throttle_info.throttled_in_tunnel_mode 
                                                = call_bringup_in_tunnel_mode;
    }

    if( throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE &&
    	profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6 &&
        throttle_info_ptr->ipv6_throttle_info.throttle_type != throttle_type )
    {
      throttle_info_ptr->ipv6_throttle_info.failure_count++;
      if( throttle_info_ptr->ipv6_throttle_info.failure_count <= 
                                                PDN_THROTTLE_MAX_TIMERS )
      {
        ip_count = throttle_info_ptr->ipv6_throttle_info.failure_count;
      }
      else
      {
        ip_count = PDN_THROTTLE_MAX_TIMERS;
        throttle_info_ptr->ipv6_throttle_info.failure_count = ip_count;
      }
   
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "Throttling v6 as well. v6 throttle count %d",
                       throttle_info_ptr->ipv6_throttle_info.failure_count );

      throttle_info_ptr->ipv6_throttle_info.throttle_type = throttle_type;

      /* Also store the failure reason in the throttle table and if
         throttling while in tunnel Mode */
      throttle_info_ptr->ipv6_throttle_info.throttle_reason = fail_reason;
  
      throttle_info_ptr->ipv6_throttle_info.throttled_in_tunnel_mode 
                                                = call_bringup_in_tunnel_mode;
    }
  }
  else if( ip_type == DS707_PDNCNTX_IP_TYPE_V6 )
  {
    if( !(throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE &&
       	  profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6 &&
          throttle_info_ptr->ipv6_throttle_info.throttle_type == 
                                          DS707_PDNCNTX_THROTTLE_TYPE_INFINITE)
      )
    {
      throttle_info_ptr->ipv6_throttle_info.failure_count++;
      if( throttle_info_ptr->ipv6_throttle_info.failure_count <= 
                                                      PDN_THROTTLE_MAX_TIMERS )
      {
        ip_count = throttle_info_ptr->ipv6_throttle_info.failure_count;
      }
      else
      {
        ip_count = PDN_THROTTLE_MAX_TIMERS;
        throttle_info_ptr->ipv6_throttle_info.failure_count = ip_count;
      }
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, "v6 throttle count %d",
                       throttle_info_ptr->ipv6_throttle_info.failure_count );

      throttle_info_ptr->ipv6_throttle_info.throttle_type = throttle_type;

      /* Also store the failure reason in the throttle table and if
         throttling while in tunnel Mode*/
      throttle_info_ptr->ipv6_throttle_info.throttle_reason = fail_reason;

      throttle_info_ptr->ipv6_throttle_info.throttled_in_tunnel_mode 
                                                  = call_bringup_in_tunnel_mode;
    }

    if( throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE &&
    	profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6 &&
        throttle_info_ptr->ipv4_throttle_info.throttle_type != throttle_type )
    {
      throttle_info_ptr->ipv4_throttle_info.failure_count++;
      if( throttle_info_ptr->ipv4_throttle_info.failure_count <= 
                                             PDN_THROTTLE_MAX_TIMERS )
      {
        ip_count = throttle_info_ptr->ipv4_throttle_info.failure_count;
      }
      else
      {
        ip_count = PDN_THROTTLE_MAX_TIMERS;
        throttle_info_ptr->ipv4_throttle_info.failure_count = ip_count;
      }

      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "Throttling v4 as well. v4 throttle count %d",
                       throttle_info_ptr->ipv4_throttle_info.failure_count );

      throttle_info_ptr->ipv4_throttle_info.throttle_type = throttle_type;
  
      /* Also store the failure reason in the throttle table and if
         throttling while in tunnel Mode */
      throttle_info_ptr->ipv4_throttle_info.throttle_reason = fail_reason;
  
      throttle_info_ptr->ipv4_throttle_info.throttled_in_tunnel_mode 
                                              = call_bringup_in_tunnel_mode;
    }
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_throttle_pdn_iptype: Invalid ip type %d",
                     ip_type );
    ASSERT(0);
  }

  if( throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW )
  {
    throttling_timer_value = ds707_get_pdn_throttle_disallow_timer( 
                                                    profile_info_ptr, 
                                                    ip_count );
  }
  else if( throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE )
  {
    throttling_timer_value = ds707_get_pdn_throttle_failure_timer(
                                                    profile_info_ptr,
                                                    ip_count );
  }
  else if (throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,  
                     "Start infinite throttling" );
    throttling_timer_value = 0;
    ds707_pdncntx_sys_pdn_throttle_info_notification();
    return;
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,  
                     "Invalid throttle type, throttle_timer = 0" );
    throttling_timer_value = 0;
  }

  /*-----------------------------------------------------------------------
    According to the algorithm, for the 3rd failure, we need to add a 
    random value (0-15 secs) to the throttling timer value
  -----------------------------------------------------------------------*/
  if ( (ip_count == 3) &&
       (throttling_timer_value != 0) )
  {
    sec_errno = secapi_get_random( SECAPI_SECURE_RANDOM, 
                                   (uint8 *)&random_value, 
                                   sizeof(uint8) );  
    if(sec_errno != E_SUCCESS)
    {
      random_value = 0;
    }
    random_value = random_value % 15;
    DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                     "3rd failure for ip type %d: adding random_value %d*1000msec to "
                     "throttle_timer_value %dmsec", 
                     ip_type, random_value, throttling_timer_value );
    throttling_timer_value = throttling_timer_value + (((uint32)random_value)*1000);
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "ip_type %d throttle timer %d msec", 
                   ip_type, throttling_timer_value );

  /*-------------------------------------------------------------------------
    If the throttle_timer is not 0, start the throttling for v4/v6 for this PDN.
  -------------------------------------------------------------------------*/
  if ( throttling_timer_value != 0 )
  {
    if (ip_type == DS707_PDNCNTX_IP_TYPE_V4 )
    {
      if (rex_get_timer(&(throttle_info_ptr->ipv4_throttle_info.throttle_timer)) != 0)
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "v4 timer running, clearing timer before starting timer with new value" );
        (void)rex_clr_timer( &(throttle_info_ptr->ipv4_throttle_info.throttle_timer) );
      }

      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "starting v4 throttling for timer %d msec", 
                       throttling_timer_value );
  
      (void)rex_set_timer( &(throttle_info_ptr->ipv4_throttle_info.throttle_timer), 
                           throttling_timer_value );
    }
    else
    {
      if (rex_get_timer(&(throttle_info_ptr->ipv6_throttle_info.throttle_timer)) != 0)
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "v6 timer running, clearing timer before starting timer with new value" );
        (void)rex_clr_timer( &(throttle_info_ptr->ipv6_throttle_info.throttle_timer) );
      }

      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "starting v6 throttling for timer %d msec", 
                       throttling_timer_value );
      
      (void)rex_set_timer( &(throttle_info_ptr->ipv6_throttle_info.throttle_timer), 
                           throttling_timer_value );
    }
    /* 
     * Since PDN throttling timer is running, 
     * we need to generate system PDN throttle info to clients
     */
    ds707_pdncntx_sys_pdn_throttle_info_notification();
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_THROTTLING_SET_ACL_DENY_REASON

DESCRIPTION   Set acl deny reason for v4 or V6 IFACE when currrent iface is 
              throttled. 

DEPENDENCIES  None

RETURN VALUE  ACL deny reason is returned.  
 
SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_throttling_set_acl_deny_reason
(
  ds707_data_session_profile_info_type * data_sess_prof_ptr,
  ps_iface_addr_family_type              ip_family             
)
{
  ds707_pdn_throttle_info_type    * throttle_info_ptr = NULL;
  ds707_pdncntx_throttle_enum_type throttle_type;
  int                              deny_reason = 
                              ACL_DENY_REASON_ERR_PDN_IPV4_CALL_THROTTLED;
/*-----------------------------------------------------------------------*/
 throttle_info_ptr = 
  ds707_pdncntx_get_throttle_info_ptr(data_sess_prof_ptr);

  /*-------------------------------------------------------------------------
    throttle info_ptr and iface ptr has to be valid
  -------------------------------------------------------------------------*/
  if ( !DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
  {
    DATA_ERR_FATAL("invalid throttle_info_ptr.return default deny reason");
    return deny_reason;
  }

  if ( IPV4_ADDR == ip_family ) /*if V4 iface*/
  {
    throttle_type = throttle_info_ptr->ipv4_throttle_info.throttle_type;
  }
  else if( IPV6_ADDR == ip_family ) /*if V6 iface*/
  {
    throttle_type = throttle_info_ptr->ipv6_throttle_info.throttle_type;
  }
  else /*invalid iface type*/
  {
    DATA_ERR_FATAL("invalid iface type; return default deny reason");
    return deny_reason;
  }

  switch(throttle_type)
  {
    case DS707_PDNCNTX_THROTTLE_TYPE_INFINITE:
      deny_reason = ACL_DENY_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED;
      break;

    case DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW:
      if ( IPV4_ADDR == ip_family )
      {
        deny_reason = ACL_DENY_REASON_ERR_PDN_IPV4_CALL_DISALLOWED;
      }
      else 
      {
        deny_reason = ACL_DENY_REASON_ERR_PDN_IPV6_CALL_DISALLOWED;
      }
      break;

    case DS707_PDNCNTX_THROTTLE_TYPE_FAILURE:
      if ( IPV4_ADDR == ip_family )
      {
        deny_reason = ACL_DENY_REASON_ERR_PDN_IPV4_CALL_THROTTLED;
      }
      else
      {
        deny_reason = ACL_DENY_REASON_ERR_PDN_IPV6_CALL_THROTTLED;
      }
      break;

    default:/*INVLID THROTTLE TYPE*/
     break;

  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                  "Set ACL deny reason for throttling: %d",
                  deny_reason);

  return deny_reason;

}/*ds707_pdncntx_throttling_set_acl_deny_reason*/

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V4_THROTTLING_SET_PS_CALL_END_REASON

DESCRIPTION   Set netdown reason for v4 IFACE when PDN throttling happens

DEPENDENCIES  None

RETURN VALUE  None 
 
SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_v4_throttling_set_ps_call_end_reason
(
  ds707_pdn_throttle_info_type    * throttle_info_ptr,
  ds707_iface_table_type          * iface_tbl_ptr              
)
{
  dssnet4_sm_cb_type   *instance_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if ( throttle_info_ptr == NULL || iface_tbl_ptr == NULL ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_v4_throttle_set_ps_call_end_reason: invalid "
                    "pointer, return.");
    return;
  }

  instance_ptr = ds707_pdncntx_get_dssnet4_sm_ptr(iface_tbl_ptr);
  if ( instance_ptr == NULL ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "NULL instance ptr.");
    return;
  }

  if (throttle_info_ptr->ipv4_throttle_info.throttle_type 
                  == DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW)
  {
    dssnet4_set_ps_call_end_reason( instance_ptr, 
         PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED );
  }

  else if(throttle_info_ptr->ipv4_throttle_info.throttle_type 
                     == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE )
  {
    dssnet4_set_ps_call_end_reason( instance_ptr, 
         PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED );
  }

  else if(throttle_info_ptr->ipv4_throttle_info.throttle_type 
        == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE )
  {
    dssnet4_set_ps_call_end_reason( instance_ptr, 
        PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED );
  }
   
  return;
} /*ds707_pdncntx_v4_throttling_set_ps_call_end_reason*/

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V6_THROTTLING_SET_PS_CALL_END_REASON

DESCRIPTION   Set netdown reason for v6 IFACE when PDN throttling happens

DEPENDENCIES  None

RETURN VALUE  None 
 
SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_v6_throttling_set_ps_call_end_reason
(
  ds707_pdn_throttle_info_type    * throttle_info_ptr,
  ds707_iface_table_type          * iface_tbl_ptr              
)
{
  dssnet6_sm_cb_type   *instance_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  if (throttle_info_ptr == NULL || iface_tbl_ptr == NULL ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_v6_throttle_set_ps_call_end_reason: invalid "
                    "pointer, return.");
    return;
  }

  instance_ptr = ds707_pdncntx_get_dssnet6_sm_ptr(iface_tbl_ptr);
  if ( instance_ptr == NULL ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "NULL instance ptr.");
    return;
  }

  if (throttle_info_ptr->ipv6_throttle_info.throttle_type 
        == DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW)
  {
    dssnet6_set_ps_call_end_reason( instance_ptr, 
           PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED );
  }

  else if(throttle_info_ptr->ipv6_throttle_info.throttle_type 
        == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE )
  {
    dssnet6_set_ps_call_end_reason( instance_ptr, 
           PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED );
  }

  else if(throttle_info_ptr->ipv6_throttle_info.throttle_type 
        == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE )
  {
    dssnet6_set_ps_call_end_reason( instance_ptr, 
           PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED );
  }

  return; 
} /*ds707_pdncntx_v6_throttling_set_ps_call_end_reason*/

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IS_IFACE_THROTTLED

DESCRIPTION   This function checks if the APN passed through the profile is 
              being throttled or not. Throttle will be judged per iface.
              For dualIP, if v4 iface is throttled, companion v6 iface may not
              be throttled.

PARAMETERS    data_sess_profile_info_ptr 
              IP family type: v4/v6 

RETURN VALUE  TRUE: if current iface is throttled 
              FALSE: otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdncntx_is_iface_throttled(
  ds707_data_session_profile_info_type  *data_sess_profile_info_ptr,
  ps_iface_addr_family_type              ip_family
)
{
  ds707_pdn_throttle_info_type *throttle_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == data_sess_profile_info_ptr ) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "invalid data_sess_profile_info_ptr. Return false.");
    return FALSE;
  }

  throttle_info_ptr = 
  ds707_pdncntx_get_throttle_info_ptr(data_sess_profile_info_ptr);


  /*-------------------------------------------------------------------------
   If throttle info ptr is valid and throttle timer is running, it means
   this PDN is throttled. 
  -------------------------------------------------------------------------*/
  if ( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) ) 
  {
    if ( IPV4_ADDR == ip_family)  /*if v4 iface*/
    {
      if ( (rex_get_timer(
                 &(throttle_info_ptr->ipv4_throttle_info.throttle_timer)) != 0)
          || (DS707_PDNCNTX_THROTTLE_TYPE_INFINITE ==
                          throttle_info_ptr->ipv4_throttle_info.throttle_type) ) 
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,"V4 iface is throttled. return TRUE.");
        return TRUE;
      }      
    }
    else if ( IPV6_ADDR == ip_family ) /*if v6 iface*/
    {
      if ( (rex_get_timer(
                  &(throttle_info_ptr->ipv6_throttle_info.throttle_timer)) != 0)
          || (DS707_PDNCNTX_THROTTLE_TYPE_INFINITE == 
                          throttle_info_ptr->ipv6_throttle_info.throttle_type) ) 
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,"V6 iface is throttled.return TRUE.");
        return TRUE;
      }  
    }
    else  /*if not v4 or v6 iface*/
    {
      ASSERT(0);
      return FALSE;
    }
  }

  return FALSE;

}
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS707_GET_NUM_DS_IFACES_IN_USE

DESCRIPTION   This function returns the total number of ds707 ifaces that 
              are currently being used

PARAMETERS    None

RETURN VALUE  uint8 - NUmber of ds707 ifaces in use

SIDE EFFECTS  None
===========================================================================*/
uint8 ds707_get_num_active_ds_ifaces(void)
{
  ps_iface_type    *iface_ptr    = NULL;
  uint8                      num_ifaces   = 0;
  uint8                      index        = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index = 0; index < DS707_MAX_IFACES;index++) 
  {
   iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(index);

   if ( (ds707_pkt_is_um_iface_up(iface_ptr) == TRUE))
   {
      num_ifaces++;
   }
  }/*End for*/

  if (num_ifaces >= DS707_MAX_IFACES)
  {
    num_ifaces = DS707_MAX_IFACES;
  }
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,"Num DS Ifaces in use: %d",num_ifaces);
  
  return num_ifaces;
}/*ds707_get_num_active_ds_ifaces*/
/*===========================================================================
FUNCTION      DS707_PDNCNTX_ALLOCATE_PDNCNTX_CB

DESCRIPTION   This function allocates a PDN context cb 

PARAMETERS    None

RETURN VALUE  pointer PDN cntx 

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdn_context_cb_type* ds707_pdncntx_allocate_pdncntx_cb(
  void
)
{
  uint8   index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*SID TODO: DO WE NEED A CRITICAL SECTION HERE AS ALLOCATION HAPPENS FROM 
    APPLICATION CONTEXT AND DE-ALLOCATION HAPPENS FROM DS CONTEXT?*/

  /*-------------------------------------------------------------------------
    Find free pointer in the PDN context pointer table.
  -------------------------------------------------------------------------*/
  for (index = 0; index < DS707_MAX_PDN_INSTANCES; index++)
  {
    if (!ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr)
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "allocate PDN cb to ptr at %d index", index);
      break;
    }
  }

  /*-------------------------------------------------------------------------
    If all pointers are in use, return NULL
  -------------------------------------------------------------------------*/
  if ( index == DS707_MAX_PDN_INSTANCES )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Cannot allocate PDN - max PDNs used! - returning NULL");
    return NULL;
  }
  else
  {
    /*-------------------------------------------------------------------------
      Allocate memory to the free pointer.
    -------------------------------------------------------------------------*/
    ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr = 
      (ds707_pdn_context_cb_type *)modem_mem_alloc(sizeof(ds707_pdn_context_cb_type), 
                                                   MODEM_MEM_CLIENT_DATA_CRIT);
    if(NULL == ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr )
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "memory allocation failed");
      return NULL;
    }
    
    /*-------------------------------------------------------------------------
      Initialize the PDN context control block
    -------------------------------------------------------------------------*/
    memset((void *)ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr, 
           0, sizeof(ds707_pdn_context_cb_type));
    ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr->this_pdn = 
      ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr;
    ds707_pdncntx_init(ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr);

    DATA_IS707_MSG1(MSG_LEGACY_MED, "PDN cb ptr 0x%x",
                    ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr);

    return(ds707_pdn_context_tbl[index].ds707_pdn_context_cb_ptr);
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PDNCNTX_CB_PTR_FROM_IFACE_PTR

DESCRIPTION   Given an iface ptr it returns a ptr to pdn context it is 
              attached to. If it doesnt belong to any pdn then returns NULL 

PARAMETERS    ps_iface_type * : pointer to iface

RETURN VALUE  pointer PDN cntx if valid 
              NULL if invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdn_context_cb_type* ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
  ps_iface_type* iface_ptr
)
{
  ds707_iface_table_type* iface_tbl_ptr = NULL;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "Invalid Iface ptr");
    return NULL;
  }

  DATA_IS707_MSG2(MSG_LEGACY_LOW, 
                  "ds707_pdncntx_get_pdncntx_cb_ptr from iface: 0x%x::%d",
                  iface_ptr->name, iface_ptr->instance );

  iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);

  if (iface_tbl_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "Iface ptr doesn't belong to any ds707_iface_tbl entry");
    return NULL;
  }

  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr(iface_tbl_ptr);

  return (pdncntx_cb_ptr);
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_V4_IFACE

DESCRIPTION   Given a PDN context cb ptr get the V4 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  iface ptr 

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_v4_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  ds707_iface_table_type*    iface_tbl_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid pdn_cntx_ptr");
    return NULL;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "ds707_pdncntx_get_v4_iface from pdn context: 0x%x",
                  pdncntx_cb_ptr);

  iface_tbl_ptr = pdncntx_cb_ptr->ds707_iface_tbl[DS707_PDN_CONTEXT_IPV4_IFACE];

  if (!DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "pdn_cntx doesn't have a V4 iface associated with it");
    return NULL;
  }

  return (&(iface_tbl_ptr->ps_iface)); 
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_CURRENT_PROFILE

DESCRIPTION   Given a PDN context cb ptr get the current profile

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  TRUE  if requesting profile ID is valid 
              FALSE if PDN CB or profile ID is invalid 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_current_profile(
  ds707_pdn_context_cb_type*              pdncntx_cb_ptr,
  ds707_data_session_profile_info_type**  data_sess_profile_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    *data_sess_profile_ptr = NULL;
    return FALSE;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "ds707_pdncntx_get_current_profile_id for PDN cb 0x%x: %d",
                  pdncntx_cb_ptr,
                  pdncntx_cb_ptr->data_profile.iface_profile_info
                        .current_data_session_profile.data_session_profile_id);

  if (DATA_SESSION_PROFILE_ID_INVALID == 
        pdncntx_cb_ptr->data_profile.iface_profile_info
                       .current_data_session_profile.data_session_profile_id )
  {
    *data_sess_profile_ptr = NULL;
    return FALSE;
  }
  else
  {
    *data_sess_profile_ptr = 
                    &pdncntx_cb_ptr->data_profile.iface_profile_info
                                    .current_data_session_profile;
    return TRUE;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_REQUESTING_PROFILE

DESCRIPTION   Given a PDN context cb ptr get the requesting profile

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  TRUE  if requesting profile ID is valid 
              FALSE if PDN CB or profile ID is invalid 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_requesting_profile(
  ds707_pdn_context_cb_type*              pdncntx_cb_ptr,
  ds707_data_session_profile_info_type**  data_sess_profile_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "Invalid pdn_cntx_ptr");
    *data_sess_profile_ptr = NULL;
    return FALSE;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED,
                  "requesting_profile_id for PDN cb 0x%x: %d",
                  pdncntx_cb_ptr, 
                  pdncntx_cb_ptr->data_profile.iface_profile_info
                    .requesting_data_session_profile.data_session_profile_id);

  if (DATA_SESSION_PROFILE_ID_INVALID == 
        pdncntx_cb_ptr->data_profile.iface_profile_info
                       .requesting_data_session_profile.data_session_profile_id)
  {
    *data_sess_profile_ptr = NULL;
    return FALSE;
  }
  else
  {
    *data_sess_profile_ptr = &pdncntx_cb_ptr->data_profile.iface_profile_info
                                .requesting_data_session_profile;
    return TRUE;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PROFILE

DESCRIPTION   Given a PDN context cb ptr get the profile ptr (either current 
              or requesting profile whichever is valid). Currently used only
              with MPPM.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  profile id 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_get_profile
(
  ds707_pdn_context_cb_type             *pdncntx_cb_ptr,
  ds707_data_session_profile_info_type **data_sess_prof_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid pdn_cntx_ptr");
    *data_sess_prof_ptr = NULL;
    return;
  }

  *data_sess_prof_ptr = &pdncntx_cb_ptr->data_profile.iface_profile_info
                                         .requesting_data_session_profile;

  if ((*data_sess_prof_ptr)->data_session_profile_id ==
          DATA_SESSION_PROFILE_ID_INVALID)
  {
    *data_sess_prof_ptr = &pdncntx_cb_ptr->data_profile.iface_profile_info
                                           .current_data_session_profile;
  }
} /* ds707_pdncntx_get_profile() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_V4_IFACE

DESCRIPTION   Given a PDN context cb ptr set the V4 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
              ds707_iface_table_type *    : pointer to iface tbl entry

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_v4_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  ds707_iface_table_type*    iface_tbl_ptr
)
{
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "Invalid pdn_cntx_ptr");
    return;
  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                  "ds707_pdncntx_set_v4_iface for pdn context: 0x%x",
                  pdncntx_cb_ptr);

  pdncntx_cb_ptr->ds707_iface_tbl[DS707_PDN_CONTEXT_IPV4_IFACE] = iface_tbl_ptr;
}

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_V6_IFACE

DESCRIPTION   Given a PDN context cb ptr get the V6 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  iface ptr 

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_v6_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  ds707_iface_table_type*    iface_tbl_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid pdn_cntx_ptr");
    return NULL;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "ds707_pdncntx_get_v6_iface from pdn context: 0x%x",
                  pdncntx_cb_ptr);

  iface_tbl_ptr = pdncntx_cb_ptr->ds707_iface_tbl[DS707_PDN_CONTEXT_IPV6_IFACE];

  if (!DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "pdn_cntx doesn't have a V6 iface associated with it");
    return NULL;
  }

  return (&(iface_tbl_ptr->ps_iface)); 
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_V6_IFACE

DESCRIPTION   Given a PDN context cb ptr set the V6 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
              ds707_iface_table_type *    : pointer to iface tbl entry

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_v6_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  ds707_iface_table_type*    iface_tbl_ptr
)
{
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "Invalid pdn_cntx_ptr");
    return;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "ds707_pdncntx_set_v6_iface for pdn context: 0x%x",
                  pdncntx_cb_ptr);

  pdncntx_cb_ptr->ds707_iface_tbl[DS707_PDN_CONTEXT_IPV6_IFACE] = iface_tbl_ptr;
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_DOWN_FLAG

DESCRIPTION   Returns the boolean which indicates which of the ifaces are 
              ready to be brought down
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb

RETURN VALUE  ds707_pdn_context_iface_down_mask: first_iface_down
===========================================================================*/
ds707_pdn_context_iface_down_mask ds707_pdncntx_get_iface_down_flag
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED,
                     "ds707_pdncntx_get_iface_down_flag: return %d",
                     pdncntx_cb_ptr->iface_down_flag );
    return (pdncntx_cb_ptr->iface_down_flag);
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_get_iface_down_flag: Invalid pdncntx_cb_ptr: 0x%x",
                     pdncntx_cb_ptr );
    return DS707_PDNCNTX_IFACE_DOWN_NONE;
  }
} /* ds707_pdncntx_get_iface_down_flag() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_IFACE_DOWN_FLAG

DESCRIPTION   Given a PDN context cb ptr set the iface down flag

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
              ds707_pdn_context_iface_down_mask : Value to which 
                                                  iface_down_flag is set

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_iface_down_flag
(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  ds707_pdn_context_iface_down_mask  iface_down_flag
)
{
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid pdn_cntx_ptr");
    return;
  }

  pdncntx_cb_ptr->iface_down_flag |= iface_down_flag;

DATA_IS707_MSG3(MSG_LEGACY_MED, 
                  "ds707_pdncntx_set_iface_down_flag for pdn context: 0x%x,"
                  "iface_down_flag %d , new iface down flag %d",
                  pdncntx_cb_ptr, iface_down_flag, pdncntx_cb_ptr->iface_down_flag);
} /* ds707_pdncntx_set_iface_down_flag() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IP6_SM_PTR

DESCRIPTION   Given the iface tbl pointer, returns the IP6 pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  ip6_sm_type *

SIDE EFFECTS  None.
===========================================================================*/
ip6_sm_type* ds707_pdncntx_get_ip6_sm_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  if (!(DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr)))
  {
    return NULL;
  }
  return (&(ds707_iface_tbl_ptr->ip6_sm));
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DSSNET6_SM_PTR

DESCRIPTION   Given the iface tbl pointer, returns the DSSNET6 pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  dssnet6_sm_cb_type *

SIDE EFFECTS  None.
===========================================================================*/
dssnet6_sm_cb_type* ds707_pdncntx_get_dssnet6_sm_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  if (!(DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr)))
  {
    return NULL;
  }
//  if (!(ps_iface_addr_family_is_v6(&(ds707_iface_tbl_ptr->ps_iface))))
//  {
//    return NULL;
//  }
  return (&(ds707_iface_tbl_ptr->dssnet6_sm_cb));
}
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_TBL_INDEX_FROM_IFACE

DESCRIPTION   Given an iface ptr it returns an index of iface tbl entry it 
              belongs to. 

PARAMETERS    ps_iface_type*

RETURN VALUE  int - positive value if success 
              -1  - if error 

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_get_iface_tbl_index_from_iface(
  ps_iface_type* iface_ptr
)
{
  uint8                    iface_index   = 0;
  ps_iface_type*           if_ptr        = NULL;
  ds707_iface_table_type*  iface_tbl_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
      "Invalid Iface ptr passed to ds707_pdncntx_get_iface_tbl_index_from_iface");
    return -1;
  }

  for (iface_index = 0;
       iface_index < DS707_MAX_IFACES;
       iface_index++)
  {
    iface_tbl_ptr = ds707_get_iface_tbl_from_instance(iface_index);
    if_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl(iface_tbl_ptr);
    if (if_ptr == iface_ptr)
    {
      break;
    }
  }

  if (iface_index < DS707_MAX_IFACES)
  {
    DATA_IS707_MSG3( MSG_LEGACY_MED, 
                     "ds707_pdncntx_get_iface_tbl_index_from_iface"
                     "iface 0x%x:%d found at index %d",
                     iface_ptr->name, iface_ptr->instance, iface_index);
    return iface_index;
  }
  else
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_get_iface_tbl_index_from_iface"
                     "iface 0x%x:%d not found at any index",
                     iface_ptr->name, iface_ptr->instance );
    return -1;
  }
}
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_TBL_ENTRY_FROM_IFACE

DESCRIPTION   Given an iface ptr it returns a ptr to iface tbl entry it 
              belongs to. This is set to iface_ptr->client_data_ptr at
              powerup

PARAMETERS    ps_iface_ptr * : pointer to iface

RETURN VALUE  pointer iface_tbl if valid 
              NULL if invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds707_iface_table_type* ds707_pdncntx_get_iface_tbl_entry_from_iface(
  ps_iface_type* iface_ptr
)
{
  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "Invalid Iface ptr");
    return NULL;
  }

  DATA_IS707_MSG2(MSG_LEGACY_LOW, 
                  "ds707_pdncntx_get_iface_tbl_entry for iface 0x%x::%d", 
                  iface_ptr->name, iface_ptr->instance );
  /*SID TODO: SHOULD WE PRINT THE RETURN VALUE?*/
  return(iface_ptr->client_data_ptr); 
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_PTR_FROM_IFACE_TBL_INDEX

DESCRIPTION   Given the iface tbl index, returns the ps_iface pointer.

PARAMETERS    uint8 : iface table index

RETURN VALUE  ps_iface_type* : pointer to iface

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(
  uint8 iface_tbl_index
)
{
  ds707_iface_table_type* iface_tbl_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "ds707_pdncntx_get_iface_ptr_from_iface_tbl_index %d",
                  iface_tbl_index);

  /*lint -save -e685 -e568 suppressing error*/
  if (iface_tbl_index < DS707_MAX_IFACES)
  /*lint -restore*/
  {
    iface_tbl_ptr = ds707_get_iface_tbl_from_instance(iface_tbl_index);
    return (ds707_pdncntx_get_iface_ptr_from_iface_tbl(iface_tbl_ptr));
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid Iface table index %d",
                    iface_tbl_index);
    return NULL;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_PTR_FROM_IFACE_TBL

DESCRIPTION   Given the iface tbl ptr, returns the ps_iface pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  ps_iface_type* : pointer to iface

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_iface_ptr_from_iface_tbl(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  if (!(DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr)))
  {
    return NULL;
  }
  return (&(ds707_iface_tbl_ptr->ps_iface));
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PDNCNTX_CB_PTR

DESCRIPTION   Given an iface table ptr it returns a ptr to pdn context it is 
              attached to. If it doesnt belong to any pdn then returns NULL 

PARAMETERS    ds707_iface_table_type* : pointer to iface tbl entry

RETURN VALUE  pointer PDN cntx if valid 
              NULL if invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdn_context_cb_type* ds707_pdncntx_get_pdncntx_cb_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  if (!(DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr)))
  {
    return NULL;
  }
  return (ds707_iface_tbl_ptr->pdn_cntx_ptr);
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DSSNET4_SM_PTR

DESCRIPTION   Given the iface tble pointer, returns the DSSNET4 SM 
              instance Ptr.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  dssnet4_sm_cb_type* : pointer to dssnet4 

SIDE EFFECTS  None.
===========================================================================*/
dssnet4_sm_cb_type* ds707_pdncntx_get_dssnet4_sm_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  if (!(DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr)))
  {
    return NULL;
  }
  if (!(ps_iface_addr_family_is_v4(&(ds707_iface_tbl_ptr->ps_iface))))
  {
    return NULL;
  }
  return (&(ds707_iface_tbl_ptr->dssnet4_sm_cb));
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ACL_PTR

DESCRIPTION   Given the iface tbl pointer, returns the ACL pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  pointer to acl_type

SIDE EFFECTS  None.
===========================================================================*/
acl_type* ds707_pdncntx_get_acl_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  if (!(DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr)))
  {
    return NULL;
  }
  return (&(ds707_iface_tbl_ptr->acl));
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_CONN_DELAY_TIMER_PTR

DESCRIPTION   Given the iface tbl pointer, returns the connection delay timer 
              pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  rex_timer_type *

SIDE EFFECTS  None.
===========================================================================*/
rex_timer_type* ds707_pdncntx_get_conn_delay_timer_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  if (!(DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr)))
  {
    return NULL;
  }
  return (&(ds707_iface_tbl_ptr->conn_delay_timer));
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DATA_SESSION_INFO_PTR

DESCRIPTION   Given the pdncntx pointer, returns the data sesssion info
              pointer.

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  ds707_data_session_info_type *

SIDE EFFECTS  None.
===========================================================================*/
ds707_data_session_info_type* ds707_pdncntx_get_data_session_info_ptr(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  if (!(DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr)))
  {
    return NULL;
  }
  return (&(pdncntx_cb_ptr->data_profile));
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_PPP_RESYNC_LOCK

DESCRIPTION   Sets ppp_resync_lock for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb
              boolean                     : ppp_resync_lock

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ppp_resync_lock(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  boolean                    ppp_resync_lock
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED,
                    "ds707_pdncntx_set_ppp_resync_lock to %d for pdn 0x%x",
                    ppp_resync_lock, pdncntx_cb_ptr);
    pdncntx_cb_ptr->ppp_resync_lock = ppp_resync_lock;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PPP_RESYNC_LOCK

DESCRIPTION   Gets ppp_resync_lock for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb
              
RETURN VALUE  boolean 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_ppp_resync_lock(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    return(pdncntx_cb_ptr->ppp_resync_lock);
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
    return FALSE;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_PDN_CB_PTR

DESCRIPTION   Set the iface table entry's pdn context control block pointer. 

PARAMETERS    ds707_iface_table_type* : pointer to iface table entry for which 
                                        pdn cb pointer needs to be set
              ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_pdn_cb_ptr( 
  ds707_iface_table_type      *iface_tbl_ptr, 
  ds707_pdn_context_cb_type   *pdncntx_cb_ptr 
)
{
  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "ds707_pdncntx_set_pdn_cb_ptr 0x%x for iface_tbl_ptr 0x%x",
                  pdncntx_cb_ptr, iface_tbl_ptr);

  if ( (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr)) ||
     (!DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr)) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Invalid pdn_cntx_ptr or iface table ptr" );
    return;
  } 

  iface_tbl_ptr->pdn_cntx_ptr = pdncntx_cb_ptr;
}

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMEOUT_CB

DESCRIPTION   Posts a command to DS Task on PDN inactivity timer expiry

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timeout_cb(
  uint32 pdn_context_cb
)
{
  ds_cmd_type * cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( pdn_context_cb != 0 )
  {
    if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
    {
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_PDN_INACTIVITY_TIMER_CMD;
    cmd_ptr->cmd_payload_ptr = (void *)pdn_context_cb;
    ds_put_cmd( cmd_ptr ); 
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Invalid pdn context in PDN timeout cb. Ignoring");
  }
} /* ds707_pdncntx_inactivity_timeout_cb() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMER_PROCESS_CMD

DESCRIPTION   Handles expiry of inactivity timer for a PDN connection

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timer_process_cmd(
  ds_cmd_type * cmd_ptr
)
{
  int ret_val;
  int16 ps_errno;
  ds707_pdn_context_cb_type *pdn_context_cb = NULL;
  ps_iface_type             *v4_iface_ptr   = NULL;
  ps_iface_type             *v6_iface_ptr   = NULL;
#ifdef FEATURE_HDR_QOS
  ds707_flow_type     *ds_flow_ptr_arr[DS707_MAX_DS_FLOWS];
  ps_flow_type        *ps_flow_ptr_arr[DS707_MAX_DS_FLOWS];
  uint8                num_ds_flows = 0;
  uint8                i;
#endif /* FEATURE_HDR_QOS */
  ps_iface_tear_down_client_data_info_type teardown_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&teardown_info, 0, sizeof(teardown_info));
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);
  if( cmd_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid ds cmd ptr!");
    return;
  }

  if((cmd_ptr->hdr.cmd_id != DS_CMD_707_PDN_INACTIVITY_TIMER_CMD ) &&
     (cmd_ptr->cmd_payload_ptr == NULL))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid command, Ignoring!");
    return;
  }
  
  pdn_context_cb = (ds707_pdn_context_cb_type *)cmd_ptr->cmd_payload_ptr;
  if( !DS707_PDNCNTX_CB_IS_VALID( pdn_context_cb ) )
  {
    DATA_ERR_FATAL("Bad parameters passed to ds707_pdn_inactivity_timer_process_cmd");
    return;   
  }

  /*------------------------------------------------------------------------- 
    If this is the last pdn and NV ignore_last_pdn_ia_timer is set to TRUE,
    then ignore the inactivity timer, restart it without any PDN termination
    execution.
  -------------------------------------------------------------------------*/
  if(ds707_pkt_mgr_ignore_last_pdn_ia_timer()&&
     ds707_pdncntx_is_last_active_pdn(pdn_context_cb))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "The last PDN inactivity timer has expired,"
                     "restart the timer and return, no pdn termination executed");
    ds707_pdncntx_inactivity_timer_restart(pdn_context_cb);
    return;
  }
  /*------------------------------------------------------------------------- 
    Clear the Inactivity timer
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "PDN CB 0x%x, inactivity timer expired",
                   pdn_context_cb);
                   
  (void)rex_clr_timer( &pdn_context_cb->inactivity_timer );

  /*------------------------------------------------------------------------- 
    Perform IPv4 iface related cleanup  
  -------------------------------------------------------------------------*/
  v4_iface_ptr = ds707_pdncntx_get_v4_iface( pdn_context_cb );
  if( PS_IFACE_IS_VALID( v4_iface_ptr ) )
  {
#ifdef FEATURE_HDR_QOS
    /*-----------------------------------------------------------------------
      Cleanup QoS
    -----------------------------------------------------------------------*/
    num_ds_flows = 0;
    ds707_sec_pkt_get_all_active_secondary_flows( v4_iface_ptr, 
                                                  ds_flow_ptr_arr, 
                                                  &num_ds_flows );

    /* Check the # of ds flows is less than the maxium # allowed */
    if ( num_ds_flows > DS707_MAX_DS_FLOWS)
    {
       num_ds_flows = DS707_MAX_DS_FLOWS;
    }

    for(i = 0; i < num_ds_flows; i++)
    {
      ps_flow_ptr_arr[i] = ds_flow_ptr_arr[i]->ps_flow_ptr;
    }
    
    ds707_sec_pkt_mgr_release_qos_resources
    ( ps_flow_ptr_arr,
      num_ds_flows,
      (uint8)DS707_SEC_PKT_MGR_AQOS_NS,
      (uint8)DS707_SEC_PKT_ATTEMPT_TO_CLEAN_DS_FLOW,
      PS_EIC_NOT_SPECIFIED 
    );
#endif /* FEATURE_HDR_QOS */
    teardown_info.validity_mask |= PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
    teardown_info.data_info.call_end_reason = 
                  PS_NET_DOWN_REASON_INTERNAL_PDN_INACTIVITY_TIMER_EXPIRED;
    teardown_info.validity_mask |= PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK; 
    teardown_info.data_info.local_abort = FALSE;    
    /*-----------------------------------------------------------------------
      Tear down the v4 iface
    -----------------------------------------------------------------------*/
    ret_val = ps_iface_active_tear_down_cmd( v4_iface_ptr,
                                             &ps_errno, 
                                             &teardown_info );
    if(ret_val == 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Iface in DOWN state already");
    }
    else if(ps_errno != DS_EWOULDBLOCK)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "Error while tearing down the iface. Err Code: %d",
                      ps_errno);
    }
  } 

  /*------------------------------------------------------------------------- 
    Perform IPv6 iface related cleanup  
  -------------------------------------------------------------------------*/
  v6_iface_ptr = ds707_pdncntx_get_v6_iface( pdn_context_cb );
  if( PS_IFACE_IS_VALID( v6_iface_ptr ) )
  {
#ifdef FEATURE_HDR_QOS
    /*-----------------------------------------------------------------------
      Cleanup QoS
    -----------------------------------------------------------------------*/
    num_ds_flows = 0;
    ds707_sec_pkt_get_all_active_secondary_flows( v6_iface_ptr, 
                                                  ds_flow_ptr_arr, 
                                                  &num_ds_flows );

    /* Check the # of ds flows is less than the maxium # allowed */
    if ( num_ds_flows > DS707_MAX_DS_FLOWS)
    {
       num_ds_flows = DS707_MAX_DS_FLOWS;
    }

    for(i = 0; i < num_ds_flows; i++)
    {
      ps_flow_ptr_arr[i] = ds_flow_ptr_arr[i]->ps_flow_ptr;
    }
    
    ds707_sec_pkt_mgr_release_qos_resources
    ( ps_flow_ptr_arr,
      num_ds_flows,
      (uint8)DS707_SEC_PKT_MGR_AQOS_NS,
      (uint8)DS707_SEC_PKT_ATTEMPT_TO_CLEAN_DS_FLOW,
      PS_EIC_NOT_SPECIFIED 
    );
#endif /* FEATURE_HDR_QOS */

    /*-----------------------------------------------------------------------
      Tear down the v6 iface
    -----------------------------------------------------------------------*/
    ret_val = ps_iface_active_tear_down_cmd( v6_iface_ptr,
                                             &ps_errno, 
                                             &teardown_info );

    if(ret_val == 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Iface in DOWN state already");
    }
    else if(ps_errno != DS_EWOULDBLOCK)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "Error while tearing down the iface. Err Code: %d",
                      ps_errno);
    }
  }
} /* ds707_pdncntx_inactivity_timer_process_cmd() */


/*===========================================================================
FUNCTION      DS707_PDNCNTX_TEARDOWN_IFACE

DESCRIPTION   This triggers teardown of given PDN.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_teardown_iface
(
  ps_iface_type                 *iface_ptr,
  ps_iface_net_down_reason_type  end_reason
)
{
#ifdef FEATURE_HDR_QOS
  ds707_flow_type     *ds_flow_ptr_arr[DS707_MAX_DS_FLOWS];
  ps_flow_type        *ps_flow_ptr_arr[DS707_MAX_DS_FLOWS];
  uint8                num_ds_flows = 0;
  uint8                i;
#endif /* FEATURE_HDR_QOS */
  int ret_val;
  int16 ps_errno;
  ds707_iface_table_type *iface_tbl_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( PS_IFACE_IS_VALID( iface_ptr ) )
  {
    /*-----------------------------------------------------------------------
      Store the call end reason in the dssnets
    -----------------------------------------------------------------------*/
    iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);
    if (ps_iface_addr_family_is_v4(iface_ptr))
    {
      dssnet4_set_ps_call_end_reason(
                                ds707_pdncntx_get_dssnet4_sm_ptr(iface_tbl_ptr), 
                                end_reason);
    }
    
    else if (ps_iface_addr_family_is_v6(iface_ptr))
    {
      dssnet6_set_ps_call_end_reason(
                                ds707_pdncntx_get_dssnet6_sm_ptr(iface_tbl_ptr), 
                                end_reason);
    }

#ifdef FEATURE_HDR_QOS
    /*---------------------------------------------------------------------
      Cleanup QoS
    ---------------------------------------------------------------------*/
    num_ds_flows = 0;
    ds707_sec_pkt_get_all_active_secondary_flows( iface_ptr, 
                                                  ds_flow_ptr_arr, 
                                                  &num_ds_flows );

    /* Check the # of ds flows is less than the maxium # allowed */
    if ( num_ds_flows > DS707_MAX_DS_FLOWS)
    {
       num_ds_flows = DS707_MAX_DS_FLOWS;
    }

    for(i = 0; i < num_ds_flows; i++)
    {
      ps_flow_ptr_arr[i] = ds_flow_ptr_arr[i]->ps_flow_ptr;
    }

    ds707_sec_pkt_mgr_release_qos_resources
    ( ps_flow_ptr_arr,
      num_ds_flows,
      (uint8)DS707_SEC_PKT_MGR_AQOS_NS,
      (uint8)DS707_SEC_PKT_ATTEMPT_TO_CLEAN_DS_FLOW,
      PS_EIC_NOT_SPECIFIED 
    );
#endif /* FEATURE_HDR_QOS */

    /*---------------------------------------------------------------------
      Tear down the v4 iface
    ---------------------------------------------------------------------*/
    ret_val = ps_iface_active_tear_down_cmd( iface_ptr,
                                             &ps_errno, 
                                             NULL );
    if(ret_val == 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Iface in DOWN state already");
    }
    else if(ps_errno != DS_EWOULDBLOCK)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "Error while tearing down the iface. Err Code: %d",
                      ps_errno);
    }
  } 
} /* ds707_pdncntx_teardown_iface() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_TEARDOWN_PDN

DESCRIPTION   This triggers teardown of given PDN.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_teardown_pdn
(
  ds707_pdn_context_cb_type     *pdn_context_cb,
  ps_iface_net_down_reason_type  end_reason
)
{
  ps_iface_type * v4_iface_ptr = NULL;
  ps_iface_type * v6_iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !DS707_PDNCNTX_CB_IS_VALID( pdn_context_cb ) )
  {
    DATA_ERR_FATAL("Bad parameters passed to ds707_pdncntx_mpit_timer_process_cmd");
    return;
  }

  /*----------------------------------------------------------------------- 
    Perform IPv4 iface related cleanup  
  -----------------------------------------------------------------------*/
  v4_iface_ptr = ds707_pdncntx_get_v4_iface( pdn_context_cb );
  ds707_pdncntx_teardown_iface(v4_iface_ptr, end_reason);

  /*----------------------------------------------------------------------- 
    Perform IPv6 iface related cleanup  
  -----------------------------------------------------------------------*/
  v6_iface_ptr = ds707_pdncntx_get_v6_iface( pdn_context_cb );
  ds707_pdncntx_teardown_iface(v6_iface_ptr, end_reason);

} /* ds707_pdncntx_teardown_pdn() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_TEARDOWN_ALL_PDNS

DESCRIPTION   This function triggers teardown of all PDN connections.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_teardown_all_pdns
(
  ps_iface_net_down_reason_type end_reason
)
{
  ds707_pdn_context_cb_type * pdn_context_cb;
  uint8                       idx;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "teardown all PDNs called, end reason = %d", end_reason);

  for (idx = 0; idx <= DS707_MAX_PDN_INSTANCES; idx++)
  {
    pdn_context_cb = ds707_get_pdncntx_cb_from_instance(idx);
   /*----------------------------------------------------------------------- 
     Perform PDN teardown.
    -----------------------------------------------------------------------*/
    if (DS707_PDNCNTX_CB_IS_VALID(pdn_context_cb))
    {
      ds707_pdncntx_teardown_pdn(pdn_context_cb, end_reason);
    }
  } /* for loop */
} /* ds707_pdncntx_terminate_all_pdns() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMER_RESTART

DESCRIPTION   Restarts the inactivity timer for a PDN connection

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timer_restart(
  ds707_pdn_context_cb_type * pdn_context_cb
)
{
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "ds707_pdncntx_inactivity_timer_restart");
  
  ds707_pdncntx_inactivity_timer_start(pdn_context_cb);
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMER_START

DESCRIPTION   Starts the inactivity timer for a PDN connection

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timer_start(
  ds707_pdn_context_cb_type * pdn_context_cb
)
{
  ds707_data_session_profile_info_type* data_sess_profile_info_ptr = NULL;
  uint32 inactivity_timeout;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !DS707_PDNCNTX_CB_IS_VALID( pdn_context_cb ) )
  {
    DATA_ERR_FATAL("Bad parameters passed to ds707_pdncntx_inactivity_timer_start");
  }

  /*------------------------------------------------------------------------- 
    Start the PDN Inactivity Timer
  -------------------------------------------------------------------------*/
  /* Get current profile id from the pdn context. 
     If current profile id is invalid, get the requesting profile id
     If requesting profile id is also invalid, return & do not start inactivity timer
  */
  if(FALSE == ds707_pdncntx_get_current_profile(pdn_context_cb,
                                                &data_sess_profile_info_ptr))
  {
    if(FALSE == ds707_pdncntx_get_requesting_profile(pdn_context_cb,
                                                     &data_sess_profile_info_ptr))
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid profile id in PDN context");
      return;
    }
  }

  if( (data_sess_profile_info_ptr->profile_type & PROFILE_EHRPD)  &&
      ds707_pkt_is_ehrpd_mode_of_operation() )
  {
    inactivity_timeout = 
      data_sess_profile_info_ptr->pdn_inactivity_timeout;
    if( inactivity_timeout != 0 
#ifdef FEATURE_DATA_OPTHO
        && DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state()
#endif /* FEATURE_DATA_OPTHO */
      )
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "Profile %d, starting inactivity timer %d ms",
                       data_sess_profile_info_ptr->data_session_profile_id, 
                       inactivity_timeout);
      (void)rex_set_timer( &pdn_context_cb->inactivity_timer, 
                           inactivity_timeout );
    }
    else
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "Inactivity timer not started for Profile %d",
                       data_sess_profile_info_ptr->data_session_profile_id);
    }
  }
} /* ds707_pdncntx_inactivity_timer_start() */
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_EVENT_CB

DESCRIPTION   This function is called when iface_down_ev event is generated 
              for registerd ifaces.
              -Mark this iface table entry as NULL.
              -If both iface tbl entries for this pdn are NULL, then
              mark the PDN out of use.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_iface_event_cb
(
  ps_iface_type                *this_iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_data_ptr
)
{
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
  ds707_iface_table_type    *iface_tbl_ptr  = NULL;
  ds707_data_session_info_type *session_info_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "ds707_pdncntx_iface_event_cb: processing event: %d",
                   event);

  if( !PS_IFACE_IS_VALID(this_iface_ptr) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_iface_event_cb: this_iface_ptr invalid" );
    return;
  }

  /*--------------------------------------------------------------------
    Get iface tbl ptr for this iface
  ---------------------------------------------------------------------*/
  iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(this_iface_ptr);
  if( !DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_iface_event_cb: pdn_cb_ptr invalid" );
    return;
  }

  /*--------------------------------------------------------------------
    Get PDN cb ptr from iface_tbl_ptr
  ---------------------------------------------------------------------*/
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr(iface_tbl_ptr);
  if( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_iface_event_cb: pdn_cb_ptr invalid" );
    return;
  }

  session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(pdncntx_cb_ptr);
  if (!(DATA_SESSION_IS_VALID(session_info_ptr))) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Invalid data sess info ptr passed to data sess ev cb");
    return;
  }
  

  switch ( event )
  {
    case IFACE_UP_EV:
    case IFACE_ROUTEABLE_EV:
    {
#ifdef FEATURE_DATA_FALLBACK_LIST
      ds707_pkt_mgr_clear_pdn_fb_count();
#endif /*FEATURE_DATA_FALLBACK_LIST*/
     /*--------------------------------------------------------------------
        If the current id is INVALID (no call up yet) only then update it 
        depending on the value of requesting id. 
        This check will prevent from updating the current id in case of a 
        PPP resync.
      --------------------------------------------------------------------*/
      /* need to init other fields in the data_session_info_type ?? */
      if( session_info_ptr->iface_profile_info.current_data_session_profile.data_session_profile_id
          == DATA_SESSION_PROFILE_ID_INVALID )
      {
        /*--------------------------------------------------------------------
          If requesting id is not INVALID only then update current id to 
          requesting id and make requesting id as INVALID.
        --------------------------------------------------------------------*/
        if( session_info_ptr->iface_profile_info.requesting_data_session_profile.data_session_profile_id
            != DATA_SESSION_PROFILE_ID_INVALID ) 
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "Moving iface requesting ID to current ID");
          session_info_ptr->iface_profile_info.current_data_session_profile =
            session_info_ptr->iface_profile_info.requesting_data_session_profile;

          ds707_data_session_set_iface_requesting_profile(
              this_iface_ptr,
              DATA_SESSION_PROFILE_ID_INVALID,
              ACTIVE_SUBSCRIPTION_1 );
        } /* if requesting id is valid */
      } /* if current id is invalid */
	  
      /*-------------------------------------------------------------------
        Set the iface linger status to FALSE
      -------------------------------------------------------------------*/
      ds707_data_session_set_iface_linger_status(this_iface_ptr, FALSE);
      
#ifdef FEATURE_EHRPD
      /*-------------------------------------------------------------------
        Start the PDN inactivity timer
      -------------------------------------------------------------------*/
      ds707_pdncntx_inactivity_timer_start( pdncntx_cb_ptr );

#ifdef FEATURE_DATA_3GPP2_VS
      /*-------------------------------------------------------------------
        Start the Max PPP inactivity timer.
        ds_3gpp2_mpit_timer_start() function will start the timer only 
        if the timer is not running.
      -------------------------------------------------------------------*/
      ds_3gpp2_mpit_timer_start();
#endif /* FEATURE_DATA_3GPP2_VS */

#ifdef FEATURE_DATA_OPTHO
      /** When iface is up, it implies VSNCP is up. Notify S101
       *  SM that VSNCP is up */
      ds707_s101_ppp_vsncp_ev(TRUE);
#endif /* FEATURE_DATA_OPTHO */

#endif /* FEATURE_EHRPD */
      break;
    }

    default:
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_iface_event_cb: %d event not handled",
                     event );
    break;
  }
} /* ds707_pdncntx_iface_event_cb() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_ARE_ALL_PDNS_INVALID

DESCRIPTION   This function checks if all PDN contexts are invalid

DEPENDENCIES  None

RETURN VALUE  TRUE: If all PDNs are invalid
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdnctx_are_all_pdns_invalid
(
  void
)
{
  uint8 pdn_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Check the PDN context table to see if all the PDNs are
   *  invalid */
  for (pdn_idx = 0; pdn_idx < DS707_MAX_PDN_INSTANCES; pdn_idx++)
  {
    if( DS707_PDNCNTX_CB_IS_VALID(
          ds707_pdn_context_tbl[pdn_idx].ds707_pdn_context_cb_ptr) )
    {
      return FALSE;
    }
  }

  return TRUE;
} /* ds707_pdnctx_are_all_pdns_invalid */

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_PDNCNTX_ARE_ALL_PDNS_DOWN

DESCRIPTION   This function checks if all PDN contexts are down

DEPENDENCIES  None

RETURN VALUE  TRUE: If all PDNs are down 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdnctx_are_all_pdns_down
(
  void
)
{
  uint8 pdn_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Check the PDN context table to see if all the PDNs are
   *  DOWN */
  for (pdn_idx = 0; pdn_idx < DS707_MAX_PDN_INSTANCES; pdn_idx++)
  {
    if(ds707_pdn_context_tbl[pdn_idx].ds707_pdn_context_cb_ptr != NULL)
    {
      if (NULL != 
          ds707_pdn_context_tbl[pdn_idx].ds707_pdn_context_cb_ptr->ncp_handle)
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                        "ds707_pdnctx_are_all_pdns_down(): "
                        "Some PDN active at %d index", pdn_idx);
        return FALSE;
      }
    }
  }

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "ds707_pdnctx_are_all_pdns_down(): All PDNs Down");
  return TRUE;
} /* ds707_pdnctx_are_all_pdns_down */


/*===========================================================================
FUNCTION      DS707_PDNCNTX_PDN_DOWN_IND

DESCRIPTION   Checks if all the PDNs are down & notify device manager

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds707_pdnctx_pdn_down_ind
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "ds707_pdn_down_indication() called");

  if (!ds707_pdnctx_are_all_pdns_down())
  {
    return;  
  }

  /* -------------------------------------------------------
   First remove the COEX MASK if it is flow controlled/exists  to allow
   LCP tear down gracefully from devmgr
   -------------------------------------------------------*/
  ds707_phys_link_enable_flow( DS_FLOW_COEX_MASK );
  
  /** If all PDNs are DOWN stop MPIT */
  ds_3gpp2_mpit_timer_stop();

  /** If all PDNs are DOWN notify device manager */
  ds707_devmgr_all_pdn_down_ind();

#ifdef FEATURE_DATA_OPTHO
  /** Notify S101 SM when all PDNs are down */
  ds707_s101_ppp_vsncp_ev(FALSE);
#endif /* FEATURE_DATA_OPTHO */

  /** If all PDNs are DOWN notify sys chg sm
   *  
   *  Keep this call at the very end of this function
   *  Note:  Adding the ind here will work only if the pdn cb is
   *  allocated dynamically. On targets that have a static pdn cb
   *  array, the all_pdn_down function checks for
   *  ncp_handle==NULL. So for dual ip non epc calls the
   *  ncp_handle will always be null. So as soon as the first IP
   *  type goes down, we would prematurely think all PDNs are down
   *  - Revisit this if code is being back ported */
  ds707_sys_chg_all_pdn_down_ind();

} /* ds707_pdnctx_pdn_down_indication() */
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_PROCESS_PS_CMD

DESCRIPTION   This function processes the PS commands received by the DS task.
              The  commands that can be received by the mode specific handler
              are as follows:

	          DS707_IFACE_IN_USE_CMD: Command to notify active iface use by app(s)
              DS707_IFACE_OUTOF_USE_CMD: Command to notify 
	 
PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_process_cmd(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
)
{

  ds707_tear_down_cmd_info_type  *tear_down_cmd_info_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(NULL == cmd_ptr)
  {
    DATA_ERR_FATAL("cmd_ptr is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch(cmd_ptr->hdr.cmd_id)
  {
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
    case DS_CMD_707_IFACE_IN_USE:
      /*---------------------------------------------------------------------
        Command to notify iface when it is being actively used by an APP
      ---------------------------------------------------------------------*/
      if(!PS_IFACE_IS_VALID((ps_iface_type*)(cmd_ptr->cmd_payload_ptr)))
      {
        DATA_ERR_FATAL("PS Iface ptr is invalid");
        return;
      }

      ds707_pdncntx_iface_in_use_cmd_hdlr(
                  (ps_iface_type*)(cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_707_IFACE_OUT_USE:
      /*---------------------------------------------------------------------
        Command to notify iface when it is not actively being used by an APP
      ---------------------------------------------------------------------*/
      tear_down_cmd_info_ptr 
                = (ds707_tear_down_cmd_info_type*)(cmd_ptr->cmd_payload_ptr);
      if(tear_down_cmd_info_ptr == NULL)
      {
        DATA_ERR_FATAL("Payload is invalid");
        return;
      }
	  
      if(!PS_IFACE_IS_VALID(
          (ps_iface_type*)(tear_down_cmd_info_ptr->iface_ptr)))
      {
        DATA_ERR_FATAL("PS Iface ptr is invalid");
      }
      ds707_pdncntx_iface_out_use_cmd_hdlr(tear_down_cmd_info_ptr);
      break;

#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */

    default:
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "invalid cmd %d passed to ds707_pdncntx_process_cmd",
                       cmd_ptr->hdr.cmd_id );
      break;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_REGISTER_IFACE_EV

DESCRIPTION   Register for iface up, down and routable events for this iface

PARAMETERS    ps_iface_type * : iface_ptr

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_register_iface_ev(
  ds707_iface_table_type* ds707_iface_tbl_ptr
)
{
  ps_iface_type *iface_ptr = NULL;

  if( !DS707_IFACE_TBL_IS_VALID(ds707_iface_tbl_ptr) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_register_iface_ev: invalid iface tbl ptr" );
    return;
  }

  iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl(ds707_iface_tbl_ptr);
  if( !PS_IFACE_IS_VALID(iface_ptr) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_register_iface_ev: invalid iface ptr" );
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "register for IFACE events for iface 0x%x::%d", 
                   iface_ptr->name, iface_ptr->instance );

  /*---------------------------------------------------------------------
   register for IFACE_UP event
  ---------------------------------------------------------------------*/
  ds707_iface_tbl_ptr->iface_up_buf_ptr = 
    ps_iface_alloc_event_cback_buf(ds707_pdncntx_iface_event_cb, NULL);
  
  if (ds707_iface_tbl_ptr->iface_up_buf_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Event buf allocation failed");
    ASSERT(0);
  }
  
  if (ps_iface_event_cback_reg( iface_ptr, 
                                IFACE_UP_EV,
                                ds707_iface_tbl_ptr->iface_up_buf_ptr ) == -1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "IFACE_UP_EV reg failed");
    ASSERT(0);
  }

  /*---------------------------------------------------------------------
   register for IFACE_ROUTEABLE_EV event
  ---------------------------------------------------------------------*/
  ds707_iface_tbl_ptr->iface_routable_buf_ptr = 
    ps_iface_alloc_event_cback_buf(ds707_pdncntx_iface_event_cb, NULL);
  
  if (ds707_iface_tbl_ptr->iface_routable_buf_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Event buf allocation failed");
    ASSERT(0);
  }
  
  if (ps_iface_event_cback_reg( iface_ptr, 
                                IFACE_ROUTEABLE_EV,
                                ds707_iface_tbl_ptr->iface_routable_buf_ptr ) == -1)
  {
    DATA_ERR_FATAL("IFACE_ROUTEABLE_EV reg failed");
    return;
  }
}

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_NCP_HANDLE

DESCRIPTION   Sets ncp_handle for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb
              ppp_ncp_handle_type*       : pointer to ncp handle

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ncp_handle(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  ppp_ncp_handle_type        ncp_handle
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                    "set NCP handle for pdn cb 0x%x to 0x%x",
                    pdncntx_cb_ptr, ncp_handle);
    pdncntx_cb_ptr->ncp_handle = ncp_handle;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_NCP_HANDLE

DESCRIPTION   Gets ncp_handle for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb
              
RETURN VALUE  ppp_ncp_handle_type        : ncp handle

SIDE EFFECTS  None.
===========================================================================*/
ppp_ncp_handle_type ds707_pdncntx_get_ncp_handle(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    return (pdncntx_cb_ptr->ncp_handle);
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
    return NULL;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_GRACEFUL_PPP_CLOSE

DESCRIPTION   Sets graceful ppp close flag for all the ifaces attached to 
              this PDN CB.

PARAMETERS    ds707_pdn_context_cb_type* : pdn context cb ptr
              boolean                    : graceful_ppp_close flag

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_graceful_ppp_close(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  boolean                    graceful_ppp_close
)
{
  ps_iface_type *iface4_ptr = NULL;
  ps_iface_type *iface6_ptr = NULL;

  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "setting graceful_ppp_close to %d for pdncntx_cb_ptr 0x%x",
                    graceful_ppp_close, pdncntx_cb_ptr);

    iface4_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
    iface6_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

    if(PS_IFACE_IS_VALID(iface4_ptr))
    {
      DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                       "V4 IFACE 0x%x:%d, setting graceful ppp close to %d",
                       iface4_ptr->name, 
                       iface4_ptr->instance, 
                       graceful_ppp_close );

      PS_IFACE_SET_GRACEFUL_CLOSE(iface4_ptr, graceful_ppp_close);
    }

    if(PS_IFACE_IS_VALID(iface6_ptr))
    {
      DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                       "V6 IFACE 0x%x:%d, setting graceful ppp close to %d",
                       iface6_ptr->name, 
                       iface6_ptr->instance, 
                       graceful_ppp_close );

      PS_IFACE_SET_GRACEFUL_CLOSE(iface6_ptr, graceful_ppp_close);
    }
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "INVALID pdn_cb_ptr 0x%x", pdncntx_cb_ptr );
  }
}
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_NCP_STARTED

DESCRIPTION   Sets ncp_started boolean for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pdn context cb ptr
              boolean                    : ncp_started flag

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ncp_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  boolean                    ncp_started
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "setting ncp started to %d for pdncntx_cb_ptr 0x%x",
                    ncp_started, pdncntx_cb_ptr);
    pdncntx_cb_ptr->ncp_started = ncp_started;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_NCP_STARTED

DESCRIPTION   Gets ncp_started boolean for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_ncp_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    return (pdncntx_cb_ptr->ncp_started);
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
    return FALSE;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_NCP_RESYNC_STARTED

DESCRIPTION   When NCP goes into resync this flag is set. When the ifaces 
              come up/go down after resync, the flag is unset.

PARAMETERS    ds707_pdn_context_cb_type* : pdn context cb ptr
              boolean                    : ncp_resync_started

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ncp_resync_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  boolean                    ncp_resync_started
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "setting ncp resync started to %d for pdncntx_cb_ptr 0x%x",
                    ncp_resync_started, pdncntx_cb_ptr);
    pdncntx_cb_ptr->ncp_resync_started = ncp_resync_started;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "set_ncp_resync_started: "
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
  }
} /* ds707_pdncntx_set_ncp_resync_started */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_NCP_RESYNC_STARTED

DESCRIPTION   Gets ncp_resync_started boolean for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_ncp_resync_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED,
                    "get_ncp_resync_started: in_resync: %d,pdncntx_cb_ptr 0x%x",
                    pdncntx_cb_ptr->ncp_resync_started, pdncntx_cb_ptr);
    return (pdncntx_cb_ptr->ncp_resync_started);
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "get_ncp_resync_started: "
                    "Invalid pdncntx_cb_ptr: 0x%x",pdncntx_cb_ptr);
    return FALSE;
  }
} /* ds707_pdncntx_get_ncp_resync_started */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_PROCESS_VSNCP_STARTED

DESCRIPTION   When VSNCP is started, in dual ip case, call active_bringup 
              on the other iface for that PDN 

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_process_vsncp_started(
  ds707_pdn_context_cb_type  *pdncntx_cb_ptr
)
{
  ps_iface_type *iface_ptr = NULL;
  int16          ps_errno; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                   "Invalid pdn_cntx_ptr");
    return;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "ds707_pdncntx_process_vsncp_started: pdncntx_cb_ptr 0x%x",
                  pdncntx_cb_ptr);

  /*----------------------------------------------------------------------- 
    This function should only be called once, depending on the ncp_started
    flag. So ncp_started = TRUE is ASSERT
  -----------------------------------------------------------------------*/
  if( ds707_pdncntx_get_ncp_started(pdncntx_cb_ptr) == TRUE )
  {
    DATA_ERR_FATAL("ncp_started flag is TRUE");
  }

  /*----------------------------------------------------------------------- 
    Set the ncp_started flag to TRUE for this PDN
  -----------------------------------------------------------------------*/
  ds707_pdncntx_set_ncp_started(pdncntx_cb_ptr, TRUE);
   
  /*----------------------------------------------------------------------- 
    For dual IP case, active bring up needs to be called on the other
    iface of the PDN.
    For this PDN if there is an iface which is allocated but not in use,
    then call active bringup on it.
  -----------------------------------------------------------------------*/

  iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
  if( (PS_IFACE_IS_VALID(iface_ptr)) && 
      (!PS_IFACE_IS_IN_USE(iface_ptr)) )
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                     "Calling active bringup for iface 0x%x::%d", 
                     iface_ptr->name, iface_ptr->instance );
    if( (ps_iface_active_bring_up_cmd( iface_ptr, &ps_errno, NULL) < 0) &&
        (ps_errno != DS_EWOULDBLOCK) )
    {
      DATA_IS707_MSG3( MSG_LEGACY_ERROR, 
                       "active bringup for iface 0x%x::%d failed", 
                       iface_ptr, iface_ptr->name, iface_ptr->instance );
      /*-------------------------------------------------------------------- 
        TODO: If active iface bring up fails
        -- we should handle it so that this PDN can work like single IP PDN?
        -- ASSERT condition?
        For now not asserting in this case
      ---------------------------------------------------------------------*/
      return;
    }  
  }
  else
  {
#ifdef FEATURE_DATA_PS_IPV6
    iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
    if( (PS_IFACE_IS_VALID(iface_ptr)) && 
        (!PS_IFACE_IS_IN_USE(iface_ptr)) )
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "Calling active bringup for iface 0x%x::%d", 
                       iface_ptr->name, iface_ptr->instance );
      if( (ps_iface_active_bring_up_cmd( iface_ptr, &ps_errno, NULL) < 0) && 
          (ps_errno != DS_EWOULDBLOCK) )
      {
        DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                         "active bringup for iface 0x%x::%d failed", 
                         iface_ptr->name, iface_ptr->instance );
        return;
      }
    }
    else
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "ds707_pdncntx_process_vsncp_started: "
                       "both ifaces for PDN 0x%x already in use", 
                       pdncntx_cb_ptr );
    }
#endif /* FEATURE_DATA_PS_IPV6 */
  }
}
/*===========================================================================
FUNCTION      DS707_PDNCNTX_READ_PDN_OVERRIDE_NV

DESCRIPTION   Read the NV record for disable pdn type override.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_read_pdn_override_nv(void)
{
  char item_file_path[] = 
          "/nv/item_files/data/3gpp2/disable_eHRPD_PDN_type_override";

  nv_stat_enum_type  status = NV_DONE_S;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  status = ds3gcfgmgr_read_efs_nv_ex(item_file_path, 
                          &ds707_pdncntx_disable_pdn_type_override, 
                          sizeof(ds707_pdncntx_disable_pdn_type_override), ds707_curr_subs_id()); 
  if (0 != status)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "EFS item not configured for ds707_pdncntx_disable_pdn_type_override, "
        "Initializing defaults.");
    ds707_pdncntx_disable_pdn_type_override = FALSE;
  }
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
      "ds707_pdncntx_read_pdn_override_nv: %d",
      ds707_pdncntx_disable_pdn_type_override);
}
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_IP_VER_CAPABILITY

DESCRIPTION   Configures the IP version capability VSNCP Parameter, if pdn type 
              override is disabled, set the capability according to what profile
              specified, otherwise, always override the capability to V4V6.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_set_ip_ver_capability(
                              vsncp_3gpp2_pdn_type *ip_ver_cap_ptr,
                              ds707_data_sess_pdn_type_enum_type pdn_type
                                    )
{
  if(NULL == ip_ver_cap_ptr)
  {
    return;
  }

  /*--------------------------------------------------------------------------- 
    If PDN type override is disabled, set the VSNCP ip ver capability according
    to the pdn type specified in the requested profile.
  --------------------------------------------------------------------------- */

  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "ds707_pdncntx_set_ip_ver_capability, pdn_type: %d "
                   "PDN overide diabled? %d",
                   pdn_type, ds707_pdncntx_disable_pdn_type_override ); 

  if(ds707_pdncntx_disable_pdn_type_override)
  {
    switch(pdn_type)
    {
      case DS707_DATA_SESS_PDN_TYPE_V4:
        *ip_ver_cap_ptr = VSNCP_3GPP2_PDN_TYPE_IPV4;
        break;
      case DS707_DATA_SESS_PDN_TYPE_V6:
        *ip_ver_cap_ptr = VSNCP_3GPP2_PDN_TYPE_IPV6;
        break;
      case DS707_DATA_SESS_PDN_TYPE_V4_V6:
        if (ds3g_is_ipv6_enabled())
        {
          *ip_ver_cap_ptr = VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6;
        }
        else
        {
          *ip_ver_cap_ptr = VSNCP_3GPP2_PDN_TYPE_IPV4;
        }
        break;
      default:
        *ip_ver_cap_ptr = VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6;
        break;
    }
  }
  /*--------------------------------------------------------------------------- 
   Else, always override to v4v6.
  --------------------------------------------------------------------------- */
  else
  {
    *ip_ver_cap_ptr = VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6;
  }
}
/*===========================================================================
FUNCTION      DS707_PDNCNTX_CONFIGURE_OP_PCO

DESCRIPTION   to configure operator PCO

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds707_pdncntx_configure_op_pco(  
                                  ppp_vsncp_3gpp2_config_type          *ncp_config_ptr,
                                  ds707_data_session_profile_info_type *data_sess_profile_info_ptr 
                                  )
{
  nv_ehrpd_imsi_type ehrpd_imsi;
  uint8              mcc[MAX_MCC_DIGITS] = {0};
  uint8              mnc[MAX_MNC_DIGITS] = {0};
  char               temp_buf[MAX_MCC_DIGITS+1] = {0};
  uint8              mnc_len = 0;

  if(NULL == ncp_config_ptr ||
     NULL == data_sess_profile_info_ptr)
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_configure_op_pco, invalid input"
                     "%x, %x",ncp_config_ptr, data_sess_profile_info_ptr); 
    return;

  }

  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "ds707_pdncntx_configure_op_pco, op_pco_id=%x ",data_sess_profile_info_ptr->op_pco_id); 
  if(data_sess_profile_info_ptr->op_pco_id >= PPP_OP_PCO_ID_START)
  {
    memset(&(ncp_config_ptr->option_vals.pco.op_pco), 0, sizeof(vsncp_3gpp2_op_pco_type));
    /*---------------------------------------------------------------------------
    If there is MCC&MNC in profile, get MCC MNC from profile
    ---------------------------------------------------------------------------*/ 
    if(data_sess_profile_info_ptr->mcc <= DS707_DATA_SESS_MAX_MCCMNC_VALUE &&
         data_sess_profile_info_ptr->mnc.mnc <= DS707_DATA_SESS_MAX_MCCMNC_VALUE )
    {
      snprintf(temp_buf,MAX_MCC_DIGITS+1,"%d",data_sess_profile_info_ptr->mcc);
      mcc[0] = temp_buf[0] - '0';
      mcc[1] = temp_buf[1] - '0';
      mcc[2] = temp_buf[2] - '0';

      memset(temp_buf, 0, MAX_MCC_DIGITS+1);
      if(data_sess_profile_info_ptr->mnc.mnc_includes_pcs_digit)
      {
        snprintf(temp_buf,MAX_MCC_DIGITS+1,"%03d",data_sess_profile_info_ptr->mnc.mnc);
        mnc_len = PPP_MNC_LEN;
        mnc[0] = temp_buf[0] - '0';
        mnc[1] = temp_buf[1] - '0';
        mnc[2] = temp_buf[2] - '0';
      }
      else
      {
        snprintf(temp_buf,MAX_MCC_DIGITS,"%d",data_sess_profile_info_ptr->mnc.mnc);
        mnc_len = PPP_MNC_LEN - 1;
        mnc[0] = temp_buf[0] - '0';
        mnc[1] = temp_buf[1] - '0';
      }
          
    }
    /*---------------------------------------------------------------------------
    MCC&MNC not configured in profile, get MCC MNC from IMSI
    ---------------------------------------------------------------------------*/ 
    else
    { 
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "MCC&MNC not configured in profile, get MCC MNC from IMSI");       
      if(!ds3g_mmgsdi_get_imsi_data (&ehrpd_imsi))
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "ds707_pdncntx_configure_op_pco, getting IMSI data fails! "); 
        return;
      }
      if(ehrpd_imsi.num_of_mcc_digits != PPP_MCC_LEN ||
         (ehrpd_imsi.num_of_mnc_digits != PPP_MNC_LEN &&
          ehrpd_imsi.num_of_mnc_digits != PPP_MNC_LEN-1)
         )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "ds707_pdncntx_configure_op_pco: get from imsi,"
                         "wrong MCC MNC length"); 
        return;
      }
      memscpy(mcc, MAX_MCC_DIGITS, ehrpd_imsi.ehrpd_mcc, ehrpd_imsi.num_of_mcc_digits);
      memscpy(mnc, MAX_MNC_DIGITS, ehrpd_imsi.ehrpd_mnc, ehrpd_imsi.num_of_mnc_digits);
      mnc_len = ehrpd_imsi.num_of_mnc_digits;

    }

    DATA_IS707_MSG7( MSG_LEGACY_MED,
                     "ds707_pdncntx_configure_op_pco, mnc :%x %x %x,"
                     " mcc :%x %x %x,len:%d",
                     mnc[0],mnc[1],mnc[2],
                     mcc[0],mcc[1],mcc[2],mnc_len);

    ncp_config_ptr->option_vals.pco.op_pco.container_id = (uint16)data_sess_profile_info_ptr->op_pco_id;
    memscpy(ncp_config_ptr->option_vals.pco.op_pco.mcc,
            PPP_MCC_LEN,
            mcc,
            PPP_MCC_LEN);
    ncp_config_ptr->option_vals.pco.op_pco.mcc_len = PPP_MCC_LEN;
    memscpy(ncp_config_ptr->option_vals.pco.op_pco.mnc,
            PPP_MNC_LEN,
            mnc,
            PPP_MNC_LEN);
    ncp_config_ptr->option_vals.pco.op_pco.mnc_len = mnc_len;

  }
}

#ifdef FEATURE_DS_MTU_PCO_VSNCP
/*===========================================================================
FUNCTION      DS707_PDNCNTX_CONFIGURE_VSNCP_MTU_PCO

DESCRIPTION   Configure VSNCP MTU PCO

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds707_pdncntx_configure_vsncp_mtu_pco
(  
   ppp_vsncp_3gpp2_config_type          *ncp_config_ptr
)
{
  boolean request_mtu_pco = FALSE;
  nv_stat_enum_type  status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == ncp_config_ptr)
  {
    DATA_ERR_FATAL("ncp_config_ptr is NULL");
    return;
  }

  status = ds3gcfgmgr_read_efs_nv_ex(VSNCP_MTU_PCO_EFS_PATH,
                          &request_mtu_pco,
                          sizeof(request_mtu_pco), ds707_curr_subs_id());
  if (0 != status)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                "EFS item not configured for VSNCP MTU PCO, setting to FALSE");
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED,
                   "request_mtu_pco value read from NV:%d ", request_mtu_pco); 
  }

  ncp_config_ptr->option_vals.pco.mtu_is_requested = request_mtu_pco;
} 
#endif /* FEATURE_DS_MTU_PCO_VSNCP */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CONFIGURE_VSNCP

DESCRIPTION   Configures the VSNCP Parameters for the given PDN Context Block

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdncntx_configure_vsncp(
  ds707_pdn_context_cb_type* pdn_context_cb,
  ppp_vsncp_3gpp2_config_type* ncp_config_ptr,
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr 
)
{
  uint8                   pdn_id  = 0;  
  boolean                 ret_val = FALSE;
  uint8                   encoded_apn_len  = 0;  
  ps_iface_type*          v4_iface_ptr     = NULL;
  ps_iface_type*          v6_iface_ptr     = NULL;
  dssnet4_sm_cb_type*     dssnet4_sm_ptr   = NULL;
  dssnet6_sm_cb_type*     dssnet6_sm_ptr   = NULL;
  ds707_iface_table_type* v4_iface_tbl_ptr = NULL;
  ds707_iface_table_type* v6_iface_tbl_ptr = NULL;
  ps_ip_addr_type         prim_dns;
  ps_ip_addr_type         sec_dns;
  uint16 mtu;
#ifdef FEATURE_EHRPD
  ds707_data_session_profile_info_type *pdn_cb_requesting_profile_ptr = NULL;
  uint32 inactivity_timeout;
#ifdef FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH
  uint8 pdn_level_auth_protocol;
  char* pdn_level_user_id;
  uint8 pdn_level_user_id_len;
  char* pdn_level_auth_password;
  uint8 pdn_level_auth_password_len;
#endif /* FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH */
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_EPC_HANDOFF
  acl_handoff_ctxt_type*  handover_ip_config_handle = NULL;
#endif
  ds707_pdn_throttle_info_type* throttle_info_ptr = NULL;
  uint8                         v4_throttle_count = 0;
  uint8                         v6_throttle_count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( ( !DS707_PDNCNTX_CB_IS_VALID( pdn_context_cb ) ) ||
       ( data_sess_profile_info_ptr == NULL ) || 
       ( ncp_config_ptr == NULL ) )
  {
    DATA_ERR_FATAL("Bad parameters passed to ds707_pdncntx_configure_vsncp");
  }

  memset( ncp_config_ptr, 0, sizeof(ppp_vsncp_3gpp2_config_type));
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Configuring VSNCP for PDN context blk: 0x%x ",
                   pdn_context_cb ); 

  v4_iface_ptr = ds707_pdncntx_get_v4_iface( pdn_context_cb );
#ifdef FEATURE_DATA_PS_IPV6
  v6_iface_ptr = ds707_pdncntx_get_v6_iface( pdn_context_cb );
#endif

  /*--------------------------------------------------------------------------- 
    In case of Dual IP check if the attach type for both the ifaces
    are the same. else ASSERT(0)
  --------------------------------------------------------------------------- */
  if( PS_IFACE_IS_VALID(v4_iface_ptr) )
  {
    v4_iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(v4_iface_ptr);
    dssnet4_sm_ptr = ds707_pdncntx_get_dssnet4_sm_ptr(v4_iface_tbl_ptr);
    ASSERT(dssnet4_sm_ptr != NULL);
  }
  
#ifdef FEATURE_DATA_PS_IPV6    
  if( PS_IFACE_IS_VALID(v6_iface_ptr) )
  {
    v6_iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(v6_iface_ptr);
    dssnet6_sm_ptr = ds707_pdncntx_get_dssnet6_sm_ptr(v6_iface_tbl_ptr);
    ASSERT(dssnet6_sm_ptr != NULL);
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  if( data_sess_profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6 )  
  {
    //Fetch the throttle count value
    throttle_info_ptr = 
          ds707_pdncntx_get_throttle_info_ptr(data_sess_profile_info_ptr);
    if(throttle_info_ptr != NULL)
    {
      v4_throttle_count = throttle_info_ptr->ipv4_throttle_info.failure_count;
      v6_throttle_count = throttle_info_ptr->ipv6_throttle_info.failure_count;
    }
    if( ((dssnet4_sm_ptr == NULL) && (v4_throttle_count == 0)) ||
        ((dssnet6_sm_ptr == NULL) && (v6_throttle_count == 0))   )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "dssnet sm ptr not valid");
      if (ds3g_is_ipv6_enabled() && 
          (dssnet6_sm_ptr == NULL))
      {
        ASSERT(0);
      }
    }
/*
#ifdef FEATURE_EPC_HANDOFF
    if( ( (dssnet4_sm_ptr->handover_attach) != (dssnet6_sm_ptr->handover_attach) ) )
    {
      DATA_ERR_FATAL("Attach types dont match");
    }
#endif
*/
  }

  /*----------------------------------------------------------------------- 
    Call the function to read the MTU from NV.
    If the NV is inactive then the function will return the default value.
  -----------------------------------------------------------------------*/
  mtu = ds707_pkt_mgr_get_mtu_size_from_nv () ; 
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,"mtu value : %d",mtu );

  if ( ds_ehrpd_pdn_mgr_allocate_pdn_id ( pdn_context_cb, &pdn_id ) != FALSE )
  { 
    ncp_config_ptr->IPV4_IFACE_PTR = v4_iface_ptr;
#ifdef FEATURE_DATA_PS_IPV6
    ncp_config_ptr->IPV6_IFACE_PTR = v6_iface_ptr;
#endif 
    ncp_config_ptr->option_vals.pdn_id = pdn_id;

    encoded_apn_len = 
      dssneti_store_encoded_apn(ncp_config_ptr->option_vals.apn,
                            (byte*)data_sess_profile_info_ptr->apn_string,
                            (uint32)data_sess_profile_info_ptr->apn_string_len);
    ncp_config_ptr->option_vals.apn_len = encoded_apn_len;

    ds707_pdncntx_set_ip_ver_capability(
      &(ncp_config_ptr->option_vals.ip_ver_capability),
      data_sess_profile_info_ptr->pdn_type
      );
    if (data_sess_profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4)
    {
      ncp_config_ptr->option_vals.req_addr_type = VSNCP_3GPP2_PDN_TYPE_IPV4;
    }
    else if (data_sess_profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V6)
    {
      ncp_config_ptr->option_vals.req_addr_type = VSNCP_3GPP2_PDN_TYPE_IPV6;
    }
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
   else if (data_sess_profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6)
    {
      if (ds3g_is_ipv6_enabled()) 
      {
        ncp_config_ptr->option_vals.req_addr_type = VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6;
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,"IPV6 NV is disabled, setting VSNCP config "
                                          "to V4 only");
        ncp_config_ptr->option_vals.req_addr_type =  
                                    VSNCP_3GPP2_PDN_TYPE_IPV4;  
      }
    }
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
    else
    {
       /* Invalid Option in the Profile */
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Invalid PDN type specified in profile: %d", 
                       data_sess_profile_info_ptr->pdn_type);
      ASSERT(0);
    }

#ifdef FEATURE_NIQ_EHRPD
    /*-----------------------------------------------------------------------
      Get is_bcm_supported value from the nw_init_qos_mgr
    -----------------------------------------------------------------------*/
    ncp_config_ptr->option_vals.pco.bcm_is_supported 
                                  = ds707_nw_init_qos_mgr_is_bcm_supported();
#else
    ncp_config_ptr->option_vals.pco.bcm_is_supported = FALSE;
#endif /* FEATURE_NIQ_EHRPD */
    DATA_IS707_MSG1(MSG_LEGACY_MED,"is_bcm_supported = %d", 
                    ncp_config_ptr->option_vals.pco.bcm_is_supported);

    /*-----------------------------------------------------------------------
      Configure VSNCP options for Initial attach. If there is some IP
      context to transfer then overwrite these options with the context
      info.
    -----------------------------------------------------------------------*/
    ncp_config_ptr->option_vals.attach_type = VSNCP_3GPP2_ATTACH_TYPE_INITIAL;
    ncp_config_ptr->option_vals.ip4.ps_s_addr = 0;
    ncp_config_ptr->option_vals.iid = 0;

#ifdef FEATURE_EPC_HANDOFF
    /*-----------------------------------------------------------------------
      If this is a handover attach then configure VSNCP with the context
      info that needs to be transferred
    -----------------------------------------------------------------------*/
    if ( pdn_context_cb->handover_attach == TRUE )
    {
      ncp_config_ptr->option_vals.attach_type = VSNCP_3GPP2_ATTACH_TYPE_HANDOVER;
      handover_ip_config_handle = 
        (acl_handoff_ctxt_type*)(pdn_context_cb->handover_ip_config_handle);

      if (handover_ip_config_handle != NULL)
      {
        /*-----------------------------------------------------------------------
          ASSERT if neither V4 nor V6 containers are valid
        -----------------------------------------------------------------------*/
        if (handover_ip_config_handle->
             pdn_ip_config.iface_ipv4_config.is_ip_cfg_valid == FALSE &&
            handover_ip_config_handle->
             pdn_ip_config.iface_ipv6_config.is_ip_cfg_valid == FALSE
           )
        {
            DATA_ERR_FATAL("Neither V4 nor V6 IP containers in handover_ip_config_handle are valid");
        }
  
        /*-----------------------------------------------------------------------
        Get the V4 IP address stored in the handover_ip_config_handle and 
        configure VSNCP with that.
        -----------------------------------------------------------------------*/
        if (handover_ip_config_handle->
             pdn_ip_config.iface_ipv4_config.is_ip_cfg_valid == TRUE)
        {
          ncp_config_ptr->option_vals.ip4.ps_s_addr = 
                handover_ip_config_handle->
                     pdn_ip_config.iface_ipv4_config.ip_addr.addr.v4.ps_s_addr;
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "Configuring VSNCP for Handover attach, with IPv4 addr:");
          IPV4_ADDR_MSG(ncp_config_ptr->option_vals.ip4.ps_s_addr);
        }
  
        if (handover_ip_config_handle->
              pdn_ip_config.iface_ipv6_config.is_ip_cfg_valid == TRUE
            )
        {
          ncp_config_ptr->option_vals.iid = 
            handover_ip_config_handle->
              pdn_ip_config.iface_ipv6_config.ip_addr.addr.v6.ps_s6_addr64[1];
          DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                           "Configuring VSNCP for Handover attach, with IPv6 iid: 0x%llx",
                           ncp_config_ptr->option_vals.iid );
        }
  
        if (handover_ip_config_handle->bearer_ip_type == PS_IFACE_BEARER_IP_V4_ONLY ||
            handover_ip_config_handle->bearer_ip_type == PS_IFACE_BEARER_IP_V6_ONLY)
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "Transfering only one IP addr during handover attach");
        }
        else if (handover_ip_config_handle->bearer_ip_type == PS_IFACE_BEARER_IP_DUAL_BEARER)
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "Transfering both V4, V6 IP addrs during handover attach");
        }
        else
        {
          /*-----------------------------------------------------------------------
           Note: Currently we dont support SADB-2B. So any other option including
                 SINGLE_BEARER is treated as a V4/V6 Only bearer. If in future
                 eHRPD starts supporting SADB-2B, we have to add a condition
                 for SINGLE_BEARER and handover_ip_config_handle->bearer_ip_type
                 contains the bearer ip info.
   
           Note: remove assert. LTE will fill the PDN type: single bearer
                 when the first IPv4 (IPv6) call on a bearer is made, since 
                 potentially this bearer can later serve a IPv6 (IPv4) call.
                 For now, just treat the bearer type as IPv4 only or IPv6 only.
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
             "Trans Only one IP addr during handover attach on single bearer");
          /*ASSERT(0);*/
        }
      }
      else
      {
        DATA_ERR_FATAL("Configuring VSNCP for Handover attach, with IPv4 addr: 0.0.0.0 ");
      }  

      /*-----------------------------------------------------------------------
      Free up the memory blob 
      -----------------------------------------------------------------------*/

      if (handover_ip_config_handle != NULL)
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "Freeing up dynamic memory allocated for IP context");
        modem_mem_free( pdn_context_cb->handover_ip_config_handle, 
                        MODEM_MEM_CLIENT_DATA_CRIT );
        pdn_context_cb->handover_ip_config_handle = NULL;
      }
      /*-----------------------------------------------------------------------
      Reset the handover_attach flag
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Setting the handover attach flag to FALSE" );
      pdn_context_cb->handover_attach = FALSE;
    }
    else
#endif
    {
      /*-----------------------------------------------------------------------
      This is already done at the start of the function
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Configuring VSNCP for Initial attach" );
    }
    /*-----------------------------------------------------------------------
      Read from Profile, and if Primary DNS is provided then set Primary and
      Secondary DNS addresses in the IFACE and set PCO option for requesting
      DNS addresses from the network to FALSE 
      If Primary DNS is not provided in the profile then request for DNS 
      addresses from the network. Determine the request type by using the
      dns_v4_addr_req NV item.
    -----------------------------------------------------------------------*/
    /*lint -save -e831 -e774*/
    if( PS_IFACE_IS_VALID(v4_iface_ptr) )
    {
      if ( ps_iface_addr_family_is_v4( v4_iface_ptr ) )
      {
        PS_IFACE_SET_V4_DNS_SERVER_ADDR( 
          v4_iface_ptr,
          data_sess_profile_info_ptr->v4_dns_addr[PRIMARY_DNS_ADDR].ps_s_addr,
          data_sess_profile_info_ptr->v4_dns_addr[SECONDARY_DNS_ADDR].ps_s_addr);
  
        if ( v4_iface_ptr->v4_net_info.primary_dns.ps_s_addr == 0 )
        {
          ncp_config_ptr->option_vals.pco.dns_is_requested |= IPV4_DNS_REQUEST ;
          ncp_config_ptr->option_vals.pco.dns_v4_addr_req = 
                                      ds707_pkt_mgr_get_dns_v4_addr_req_type();
        }

        DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                         "V4 iface dns_is_requested = %d, "
                         "Setting v4 iface MTU: %d",
                         ncp_config_ptr->option_vals.pco.dns_is_requested, mtu);

        PS_IFACE_SET_MTU( v4_iface_ptr, mtu );

      }/*V4 Type addr*/
    } /* v4_iface is valid */
    /*lint -restore*/
    else
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "V4 iface not allocated for PDN context cb 0x%x, "
                       "not configuring vsncp for v4",
                       pdn_context_cb );
    }

#ifdef FEATURE_DATA_PS_IPV6
    if( PS_IFACE_IS_VALID(v6_iface_ptr) )
    {
      if( ps_iface_addr_family_is_v6 (v6_iface_ptr) )
      {
        /*memset local pri and sec dns structs*/
        memset(&prim_dns.addr.v6, 0, sizeof(struct ps_in6_addr));
        memset(&sec_dns.addr.v6, 0, sizeof(struct ps_in6_addr));
        prim_dns.type = IPV6_ADDR;
        sec_dns.type = IPV6_ADDR;
        /* struct copy */
        prim_dns.addr.v6 = 
          data_sess_profile_info_ptr->v6_dns_addr[PRIMARY_DNS_ADDR]; 
        sec_dns.addr.v6 = 
          data_sess_profile_info_ptr->v6_dns_addr[SECONDARY_DNS_ADDR];
        if( ps_iface_set_v6_dns_addrs(v6_iface_ptr,&prim_dns,&sec_dns) < 0)
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                           "ps_iface_set_v6_dns_addrs failed" );
        }
  
        if (PS_IN6_IS_ADDR_UNSPECIFIED(&(v6_iface_ptr->v6_net_info.primary_dns)))
        {
          ncp_config_ptr->option_vals.pco.dns_is_requested |= IPV6_DNS_REQUEST ;
        }

        DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                         "V6 iface requesting DNS dns_is_requested = %d, "
                         "Setting v6 iface MTU: %d",
                         ncp_config_ptr->option_vals.pco.dns_is_requested, mtu);

        PS_IFACE_SET_MTU( v6_iface_ptr, mtu );

      }/*V6 Type addr*/
    } /* v6_iface is valid */
    else
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "V6 iface not allocated for PDN context cb 0x%x, "
                       "not configuring vsncp for v6",
                       pdn_context_cb );
    }
#endif

    ncp_config_ptr->option_vals.pco.p_cscf_is_requested = 
      data_sess_profile_info_ptr->is_pcscf_addr_needed;
    ncp_config_ptr->option_vals.pco.ip_addr_via_nas = TRUE;
    ncp_config_ptr->option_vals.default_rtr_ip4.ps_s_addr = 0;
  
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH

    if ( data_sess_profile_info_ptr->pdn_level_auth_protocol == 
           DATA_SESS_AUTH_PROTOCOL_PAP_CHAP ) 
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "Profile PDN level auth has PAP_CHAP,"
                       "read NV configuration" );
      if(DS3G_PDN_AUTH_CHAP == ds3g_get_config_pdn_level_auth(SYS_MODEM_AS_ID_1))
      {
         pdn_level_auth_protocol = DATA_SESS_AUTH_PROTOCOL_CHAP;
      }
      else if(DS3G_PDN_AUTH_PAP == ds3g_get_config_pdn_level_auth(SYS_MODEM_AS_ID_1))
      {
         pdn_level_auth_protocol = DATA_SESS_AUTH_PROTOCOL_PAP;
      }
      else
      {      
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "NV invalid, defaulting to CHAP" );

        pdn_level_auth_protocol = DATA_SESS_AUTH_PROTOCOL_CHAP;
      }
    }
    else
    {
      pdn_level_auth_protocol = data_sess_profile_info_ptr->pdn_level_auth_protocol;
    }

    if ( DATA_SESS_AUTH_PROTOCOL_PAP == pdn_level_auth_protocol ||
         DATA_SESS_AUTH_PROTOCOL_CHAP == pdn_level_auth_protocol )
    {
      DATA_IS707_MSG1( MSG_LEGACY_MED, "PDN Level Authentication will occur. "
                       "Auth protocol = %d", pdn_level_auth_protocol);
      pdn_level_user_id = data_sess_profile_info_ptr->pdn_level_user_id;
      pdn_level_user_id_len = data_sess_profile_info_ptr->pdn_level_user_id_len;
      pdn_level_auth_password = data_sess_profile_info_ptr->pdn_level_auth_password;
      pdn_level_auth_password_len = data_sess_profile_info_ptr->pdn_level_auth_password_len;

      if( ( NULL != pdn_level_user_id ) &&
          ( pdn_level_user_id_len < PPP_MAX_USER_ID_LEN ) &&
          ( pdn_level_user_id_len != 0 ) )
      {
        memscpy(ncp_config_ptr->option_vals.pco.pdn_level_auth.user_id,
                PPP_MAX_USER_ID_LEN,
                pdn_level_user_id,
                pdn_level_user_id_len);
        ncp_config_ptr->option_vals.pco.pdn_level_auth.user_id_len = pdn_level_user_id_len;
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid PDN level auth user id, "
                         "Call failed!" );
        return FALSE;
      }

      if ( pdn_level_auth_password_len > PPP_MAX_PASSWD_LEN )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid PDN level auth password, "
                         "Call failed!" );
        return FALSE;
      }

      /* Only check for NULL password if protocol is CHAP,
         NULL passwords are valid for PAP */
      if ( DATA_SESS_AUTH_PROTOCOL_CHAP == pdn_level_auth_protocol ) 
      {
        if( ( NULL != pdn_level_auth_password ) &&
            ( pdn_level_auth_password_len != 0 ) )
        {
          memscpy(ncp_config_ptr->option_vals.pco.pdn_level_auth.password,
                  PPP_MAX_PASSWD_LEN,
                  pdn_level_auth_password,
                  pdn_level_auth_password_len);
          ncp_config_ptr->option_vals.pco.pdn_level_auth.password_len = pdn_level_auth_password_len;
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid PDN level auth password for CHAP, "
                           "Call failed!" );
          return FALSE;
        }
      }
      else
      { /* PDN Level Auth Protocol is PAP, password can be NULL for PAP */
        if( ( NULL != pdn_level_auth_password ) &&
            ( pdn_level_auth_password_len != 0 ) )
        {
          memscpy(ncp_config_ptr->option_vals.pco.pdn_level_auth.password,
                  PPP_MAX_PASSWD_LEN,
                  pdn_level_auth_password,
                  pdn_level_auth_password_len);
          ncp_config_ptr->option_vals.pco.pdn_level_auth.password_len = pdn_level_auth_password_len;
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Using a NULL password for PDN level"
                           " PAP authentication" );
          ncp_config_ptr->option_vals.pco.pdn_level_auth.password_len = 0;
        }
      }

      ncp_config_ptr->option_vals.pco.pdn_level_auth.protocol =
        (ppp_auth_proto_e_type)pdn_level_auth_protocol;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "PDN Level Authentication not requested.");
    }
#endif /* FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH */

    /*-----------------------------------------------------------------------
      If a non-zero value for inactivity timeout was supplied in the profile,
      allocate a timer and register the activity callback with PPP
    -----------------------------------------------------------------------*/
    if( ds707_pdncntx_get_requesting_profile(pdn_context_cb,
                                             &pdn_cb_requesting_profile_ptr) )
    {
      if (pdn_cb_requesting_profile_ptr != NULL)
      {
        inactivity_timeout = 
          pdn_cb_requesting_profile_ptr->pdn_inactivity_timeout;
        if( inactivity_timeout != 0 )
        {
          if(rex_get_timer(&(pdn_context_cb->inactivity_timer)) != 0)
          {
            DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "inactivity timer running, clearing timer before starting timer with new value" );
            (void)rex_clr_timer( &(pdn_context_cb->inactivity_timer) );
          }

          ncp_config_ptr->activity_cb.func = 
            (vsncp_3gpp2_pdn_activity_cb_f_ptr_type)ds707_pdncntx_inactivity_timer_start;
          ncp_config_ptr->activity_cb.data = pdn_context_cb;
        }
      }
    }

    ds707_pdncntx_configure_op_pco(ncp_config_ptr, data_sess_profile_info_ptr);

#ifdef FEATURE_DS_MTU_PCO_VSNCP
    ds707_pdncntx_configure_vsncp_mtu_pco(ncp_config_ptr);
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
#endif /* FEATURE_EHRPD */
    ret_val = TRUE;
  }

  return ret_val;
} /* ds707_pdncntx_configure_vsncp() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CHECK_COMPATIBILITY

DESCRIPTION   Checks if the pdn context cb ptr is of the same config that is 
              passed in the policy. 

PARAMERTERS   ds707_pdn_context_cb_type* : pointer to pdn context cb 
              acl_policy_info_type*      : pointer to policy info 

RETURN VALUE  boolean: TRUE if compatible. 
                       FALSE otherwise 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_check_compatibility( 
  ds707_pdn_context_cb_type *pdncntx_cb_ptr, 
  acl_policy_info_type      *policy_info_ptr 
)
{
  ds707_data_session_profile_info_type  *prof_info_ptr = NULL;
  ds707_data_session_info_type   *data_session_info_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr)) || 
      (policy_info_ptr == NULL) )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "Invalid pdncntx_cb_ptr 0x%x or policy_info_ptr 0x%x",
                    pdncntx_cb_ptr, policy_info_ptr);
    return FALSE;
  }

  //TODO: Check if the incoming subs_id  in policy info ptr is same as existing PDN's subsid
  if( TRUE != ds707_data_session_get_profile( 
                policy_info_ptr->data_session_profile_id,
                (ds_profile_subs_id_enum_type)policy_info_ptr->subs_id,
                &prof_info_ptr ) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Invalid pending_id");
    return FALSE;
  }

  data_session_info_ptr = ds707_pdncntx_get_data_session_info_ptr( pdncntx_cb_ptr );
  if ( !DATA_SESSION_IS_VALID(data_session_info_ptr) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Invalid data_session_info_ptr");
    return FALSE;
  }

  if ( data_session_info_ptr->iface_profile_info.current_data_session_profile.data_session_profile_id
         != DATA_SESSION_PROFILE_ID_INVALID )
  {
    if( (ds707_data_session_ehrpd_friend_profiles(
         &(data_session_info_ptr->iface_profile_info.current_data_session_profile), 
         prof_info_ptr,
         policy_info_ptr)) ||
        ( ( TRUE == ds707_pdncntx_is_default_pdn(pdncntx_cb_ptr) ) &&
          ( ds707_pdncntx_is_default_apn(prof_info_ptr) == TRUE ) )
      )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    if ( data_session_info_ptr->iface_profile_info.requesting_data_session_profile.data_session_profile_id
           != DATA_SESSION_PROFILE_ID_INVALID )
    {
      if( (ds707_data_session_ehrpd_friend_profiles(
           &(data_session_info_ptr->iface_profile_info.requesting_data_session_profile), 
           prof_info_ptr,
           policy_info_ptr)) ||
          ( ( TRUE == ds707_pdncntx_is_default_pdn(pdncntx_cb_ptr) ) &&
            ( ds707_pdncntx_is_default_apn(prof_info_ptr) == TRUE ) )
        )
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
  }
  return FALSE;
}

#ifdef FEATURE_DATA_EHRPD_DUAL_IP
/*===========================================================================
FUNCTION      DS707_PDNCNTX_ALLOCATE_IFACE_TBL_ENTRY

DESCRIPTION   This function allocates a iface table entry

PARAMETERS    None

RETURN VALUE  pointer to iface table entry 

SIDE EFFECTS  None.
===========================================================================*/
ds707_iface_table_type* ds707_pdncntx_allocate_iface_tbl_entry(
  void
)
{
  ds707_iface_table_type* iface_tbl_ptr = NULL;
  uint8                   iface_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*SID TODO: DO WE NEED A CRITICAL SECTION HERE AS ALLOCATION HAPPENS FROM 
    APPLICATION CONTEXT AND DE-ALLOCATION HAPPENS FROM DS CONTEXT?*/
  for (iface_index = 0; 
       iface_index < DS707_MAX_IFACES; 
       iface_index++)
  {
    iface_tbl_ptr = ds707_get_iface_tbl_from_instance(iface_index);
    if( iface_tbl_ptr->pdn_cntx_ptr == NULL )
    {
      break;
    }
  }

  if ( iface_index == DS707_MAX_IFACES )
  {
    iface_tbl_ptr = NULL;
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "Cannot allocate iface tbl entry - max ifaces used! - returning NULL");
  }
  return(iface_tbl_ptr);
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_IN_USE_CMD

DESCRIPTION   Post a command to indicate the active use of the iface 
              by application.

PARAMETERS    ps_iface_type * : Iface_ptr 

DEPENDENCIES  None.

RETURN VALUE  int 0 - success

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_iface_in_use_cmd
(
  ps_iface_type    *iface_ptr,
  void             *not_used
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    This function can only be called for an active iface
  ------------------------------------------------------------------------*/
  if ( !(PS_IFACE_GET_IS_ACTIVE(iface_ptr)) ) 
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"iface 0x%x is inactive", iface_ptr);
    return(0);
  }

  /*-------------------------------------------------------------------------
    Iface has already been brought up. Now this function is called when the 
	first app starts to use the iface.
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "DS_CMD_707_IFACE_IN_USE iface 0x%x:%d",
                  iface_ptr->name,iface_ptr->instance);


  if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO))== NULL)
  {
    ASSERT(0);
    return -1;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_707_IFACE_IN_USE;
    cmd_ptr->cmd_payload_ptr = (void *)iface_ptr;
    ds_put_cmd_ext(cmd_ptr);
  }

  return(0);
} 

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_OUT_OF_USE_CMD

DESCRIPTION   Post a command to indicate the iface is not in use by 
              any application.

PARAMETERS    ps_iface_type * : iface_ptr
              void * : client_ptr of type ps_iface_tear_down_client_data_info_type

DEPENDENCIES  None.

RETURN VALUE  int 0 - Success

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_iface_out_use_cmd
(
  ps_iface_type    *iface_ptr,
  void             *client_ptr
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
  ds707_tear_down_cmd_info_type *tear_down_cmd_info_ptr = NULL;
  boolean                        local_abort = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    This function can only be called for an active iface
  ------------------------------------------------------------------------*/
  if ( !(PS_IFACE_GET_IS_ACTIVE(iface_ptr)) ) 
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"iface 0x%x is inactive", iface_ptr);
    ASSERT(0);
    return -1;
  }

  DATA_IS707_MSG3(MSG_LEGACY_HIGH,
               "DS707_IFACE_OUT_USE_CMD iface 0x%x:%d client ptr is 0x%x",
               iface_ptr->name, iface_ptr->instance, client_ptr);

  if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_tear_down_cmd_info_type))) == NULL )
  {
    ASSERT(0);
    return -1;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_707_IFACE_OUT_USE;

    tear_down_cmd_info_ptr 
              = (ds707_tear_down_cmd_info_type*)(cmd_ptr->cmd_payload_ptr);
    if(tear_down_cmd_info_ptr == NULL)
    {
      ASSERT(0);
      return -1;
    }
	
    local_abort =  PS_IFACE_GET_PDN_TEARDOWN_LOCAL_ABORT_CONFIG(iface_ptr);

    if(client_ptr != NULL)
    {    
      tear_down_cmd_info_ptr->is_user_data_set = TRUE;
      memscpy(&(tear_down_cmd_info_ptr->user_data.tear_down_info),
              sizeof(ps_iface_tear_down_client_data_info_type),	  	
              (ps_iface_tear_down_client_data_info_type *) client_ptr,
              sizeof(ps_iface_tear_down_client_data_info_type));
    }

    if (local_abort)
    {
      tear_down_cmd_info_ptr->is_user_data_set = TRUE;
      tear_down_cmd_info_ptr->user_data.tear_down_info.validity_mask =
        PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK;
      tear_down_cmd_info_ptr->user_data.tear_down_info.data_info.local_abort =
        TRUE;
    }
    	
    tear_down_cmd_info_ptr->iface_ptr = iface_ptr;
    tear_down_cmd_info_ptr->lpm_mode  = FALSE;

    ds_put_cmd_ext(cmd_ptr);
  }
  return(0);
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_HANDLE_PPP_EV

DESCRIPTION   From dssnet this function is called to resolve what action 
              should be taken for ppp_event for this iface. We need to
              resolve 2 PPP events - SUBSCRIPTION_LIMITED_TO_V4 and
              SUBSCRIPTION_LIMITED_TO_V6 depending on the address family of
              the iface for dual ip. This function should be called only
              in case of the above 2 events. This function implements the
              following table
 
------------------------------------------------------------------------------ 
 App request/ | PDNcb allocated | PPP event  |  Action / return code  
 failover NV  |  ifaces         |            |
--------------|-----------------|------------|-------------------------------- 
    V4        |   V4            |  SUB_V4    | LINK_UP_WITH_SIP
--------------|-----------------|------------|--------------------------------
              |                 |  SUB_V6    | PPP_CLOSE_EV
--------------|-----------------|------------|-------------------------------- 
              |   V4_V6         |  SUB_V4    | For V4 iface: LINK_UP_WITH_SIP
              |                 |            | For V6 iface: LINK_DOWN
--------------|-----------------|------------|--------------------------------
              |                 |  SUB_V6    | PPP_CLOSE
--------------|-----------------|------------|-------------------------------- 
    V6        |   V6            |  SUB_V4    | PPP_CLOSE
--------------|-----------------|------------|-------------------------------- 
              |                 |  SUB_V6    | LINK_UP_WITH_SIP
--------------|-----------------|------------|-------------------------------- 
              |   V4_V6         |  SUB_V4    | PPP_CLOSE
--------------|-----------------|------------|-------------------------------- 
              |                 |  SUB_V6    | For V4 iface: LINK_DOWN
              |                 |            | For V6 iface: LINK_UP_WITH_SIP
--------------|-----------------|------------|-------------------------------- 
 

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb 
              ps_iface_type*             : pointer to iface
              event                      : ppp event

RETURN VALUE  ds707_pdncntx_ppp_event_action.

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdncntx_ppp_event_action ds707_pdncntx_handle_ppp_ev(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr, 
  ps_iface_type             *iface_ptr, 
  ppp_event_e_type           ppp_event
)
{
  ps_iface_type                  *pdn_v4_iface_ptr     = NULL;
  ps_iface_type                  *pdn_v6_iface_ptr     = NULL;
  boolean                        v4_iface_is_allocated = FALSE;
  boolean                        v6_iface_is_allocated = FALSE;
  boolean                        v4_iface_in_use       = FALSE;
  boolean                        v6_iface_in_use       = FALSE;
  ds707_pdncntx_ppp_event_action ret_val               = 
                                                  DS707_PDNCNTX_INVALID_ACTION;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_handle_ppp_ev:Invalid pdn_cntx_ptr" );
    return DS707_PDNCNTX_INVALID_ACTION; 
  }

  if(!PS_IFACE_IS_VALID(iface_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "ds707_pdncntx_handle_ppp_ev:Invalid iface_ptr" );
    return DS707_PDNCNTX_INVALID_ACTION;
  }

  /*----------------------------------------------------------------------- 
    Get the address family requested by the app from the iface which is
    valid and in use of this PDN.
    Set flags to remember whether v4, v6 or both ifaces are allocated to
    this PDN
  ------------------------------------------------------------------------*/
  pdn_v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
  if(PS_IFACE_IS_VALID(pdn_v4_iface_ptr))
  {
    v4_iface_is_allocated = TRUE;
    if(PS_IFACE_IS_IN_USE(pdn_v4_iface_ptr))
    {
      v4_iface_in_use = TRUE;
    }
  }

#ifdef FEATURE_DATA_PS_IPV6
  pdn_v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
  if(PS_IFACE_IS_VALID(pdn_v6_iface_ptr))
  {
    v6_iface_is_allocated = TRUE;
    if(PS_IFACE_IS_IN_USE(pdn_v6_iface_ptr))
    {
      v6_iface_in_use = TRUE;
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  DATA_IS707_MSG4( MSG_LEGACY_MED, 
                   "ds707_pdncntx_handle_ppp_ev: "
                   "v4 iface: allocated: %d, in use: %d, "
                   "v6 iface: allocated: %d, in use: %d",
                   v4_iface_is_allocated, v4_iface_in_use,
                   v6_iface_is_allocated, v6_iface_in_use );

  if( v4_iface_is_allocated &&
      v6_iface_is_allocated )
  {
    PS_IFACE_SET_IS_ACTIVE(pdn_v4_iface_ptr, FALSE); 
    PS_IFACE_SET_IS_ACTIVE(pdn_v6_iface_ptr, FALSE); 
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "ds707_pdncntx_handle_ppp_ev: "
                     "setting v4 and v6 iface as passive" );

  }

  switch( ppp_event )
  {
    case PPP_SUBS_LIMITED_TO_V4_EV:
    {
      if( ps_iface_addr_family_is_v4(iface_ptr) )
      {
        if( v4_iface_in_use == TRUE )
        {
          ret_val = DS707_PDNCNTX_LINK_UP;
        }
        else
        {
          ret_val = DS707_PDNCNTX_PPP_CLOSE;
        }
      }

      if( ps_iface_addr_family_is_v6(iface_ptr) )
      {
        if( v6_iface_in_use == TRUE )
        {
          if( v4_iface_in_use != TRUE )
          {
            ret_val = DS707_PDNCNTX_PPP_CLOSE;
          }          
        }
        ret_val = DS707_PDNCNTX_LINK_DOWN;
      }
    
      break;
    } /* case PPP_SUBS_LIMITED_TO_V4_EV */

    case PPP_SUBS_LIMITED_TO_V6_EV:
    {
      if( ps_iface_addr_family_is_v6(iface_ptr) )
      {
        if( v6_iface_in_use == TRUE )
        {
          ret_val = DS707_PDNCNTX_LINK_UP;         
        }
        else
        {
          ret_val = DS707_PDNCNTX_PPP_CLOSE;
        }
      }

      if( ps_iface_addr_family_is_v4(iface_ptr) )
      {
        if( v4_iface_in_use == TRUE )
        {
          if( v6_iface_in_use != TRUE )
          {
            ret_val = DS707_PDNCNTX_PPP_CLOSE;
          }          
        }
        ret_val = DS707_PDNCNTX_LINK_DOWN;
      }

      break;
    } /* case PPP_SUBS_LIMITED_TO_V6_EV */

    default:
      ret_val = DS707_PDNCNTX_INVALID_ACTION;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "ds707_pdncntx_handle_ppp_ev: received PPP event: %d, "
                   "return_code %d",
                   ppp_event, ret_val );

  return ret_val;
}
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_EPC_HANDOFF_CTXT

DESCRIPTION   Handles the IOCTL to get epc handoff ctxt. It returns the IP
              config details of the passed IFACE and companion IFACE (in 
              case of dual IP connection) and bearer type (single IP or dual
              IP)
    
PARAMETERS    argval_ptr: Pointer to acl_handoff_ctxt_type that contains IP
              config of this IFACE, companion IFACE (in case of dual IP) and
              bearer type (single IP or dual IP bearer)

DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_get_epc_handoff_ctxt(
                                      ps_iface_type             *iface_ptr,
                                      void                      *argval_ptr
                                      )
{
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
  acl_handoff_ctxt_type     *handoff_cntx_ptr = NULL;
  ps_iface_type             *v4_iface_ptr = NULL;
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type             *v6_iface_ptr = NULL;
#endif
  ps_iface_type             *cmpn_iface_ptr = NULL;
  ps_iface_addr_family_type current_iface_iptype = IFACE_INVALID_ADDR_FAMILY;
  ps_iface_addr_family_type companion_iface_iptype = IFACE_INVALID_ADDR_FAMILY;
  ps_iface_ip_config_type   *current_iface_ip_cfg_ptr = NULL;
  ps_iface_ip_config_type   *companion_iface_ip_cfg_ptr = NULL;

/*- - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG2(
                 MSG_LEGACY_HIGH,
                 "ds707_pdncntx_get_epc_handoff_ctxt, iface: 0x%x, argval_ptr: 0x%x",
                 iface_ptr, argval_ptr);  

  if ( (!PS_IFACE_IS_VALID(iface_ptr)) ||
       (argval_ptr == NULL) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,"invalid IFACE ptr or argval ptr" );
    return -1;
  }

  // get the pdn cntx cb from iface.
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
  if(!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr)) 
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "given iface 0x%x:%d not associated with any pdn, failing IOCTL",
                     iface_ptr->name,iface_ptr->instance );
    return -1;
  }

  handoff_cntx_ptr = (acl_handoff_ctxt_type*)argval_ptr;

  v4_iface_ptr = ds707_pdncntx_get_v4_iface( pdncntx_cb_ptr );
#ifdef FEATURE_DATA_PS_IPV6
  v6_iface_ptr = ds707_pdncntx_get_v6_iface( pdncntx_cb_ptr );
#endif
  if (v4_iface_ptr == iface_ptr)
  {
    handoff_cntx_ptr->bearer_ip_type = PS_IFACE_BEARER_IP_V4_ONLY;

    current_iface_ip_cfg_ptr = 
      &(handoff_cntx_ptr->pdn_ip_config.iface_ipv4_config);
    current_iface_iptype = IFACE_IPV4_ADDR_FAMILY;
    
//sid: print
#ifdef FEATURE_DATA_PS_IPV6
    cmpn_iface_ptr = v6_iface_ptr;
    companion_iface_ip_cfg_ptr = 
      &(handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config);
    companion_iface_iptype = IFACE_IPV6_ADDR_FAMILY;
#endif
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if (v6_iface_ptr == iface_ptr)
  {
    handoff_cntx_ptr->bearer_ip_type = PS_IFACE_BEARER_IP_V6_ONLY;
    cmpn_iface_ptr = v4_iface_ptr;
    current_iface_iptype = IFACE_IPV6_ADDR_FAMILY; 
    companion_iface_iptype = IFACE_IPV4_ADDR_FAMILY;
    current_iface_ip_cfg_ptr = 
      &(handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config);
    companion_iface_ip_cfg_ptr = 
      &(handoff_cntx_ptr->pdn_ip_config.iface_ipv4_config);  
  }
#endif
  else
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "given iface: 0x%x:%d not associated with any pdn, failing IOCTL",
                     iface_ptr->name,iface_ptr->instance);
    ASSERT(0);
  }

  /*------------------------------------------------------------------------
    Init the IP cfg container to be invalid for both IP types
  ------------------------------------------------------------------------*/
  current_iface_ip_cfg_ptr->is_ip_cfg_valid = FALSE;
  companion_iface_ip_cfg_ptr->is_ip_cfg_valid = FALSE;

  /*------------------------------------------------------------------
    Get the IP cfg of the iface passed in
  ------------------------------------------------------------------*/
  if (-1 == ps_iface_get_ip_config
                   (iface_ptr, current_iface_ip_cfg_ptr)
     )
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "can't get IP cfg for iface 0x%x:%d; failing IOCTL",
                     iface_ptr->name,iface_ptr->instance );
    return -1;
  }

#ifdef FEATURE_DATA_PS_IPV6          
  /*------------------------------------------------------------------
    Fill up router life time if iface IP type is IPV6
  ------------------------------------------------------------------*/
  if (IFACE_IPV6_ADDR_FAMILY == current_iface_iptype)
  {
    if (-1 == ds707_pdncntx_get_v6_rem_rtr_lifetime
              (iface_ptr,
               &(current_iface_ip_cfg_ptr->router_life_time))
       )
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "can't get V6 rem rtr lifetime "
                     "for iface 0x%x:%d",
                     iface_ptr->name,iface_ptr->instance );
      current_iface_ip_cfg_ptr->router_life_time = 0;
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  current_iface_ip_cfg_ptr->is_ip_cfg_valid = TRUE;

  if (PS_IFACE_IS_VALID(cmpn_iface_ptr))
  {
    /*------------------------------------------------------------------
      Get the IP Config of the cmpn iface
    ------------------------------------------------------------------*/
    if (-1 == ps_iface_get_ip_config
                    (cmpn_iface_ptr, companion_iface_ip_cfg_ptr)
       )
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "can't get IP cfg for cmpn iface 0x%x:%d",
                       cmpn_iface_ptr->name,cmpn_iface_ptr->instance );  
    }
    else
    {
#ifdef FEATURE_DATA_PS_IPV6
      /*------------------------------------------------------------------
        Fill up router life time if iface IP type is IPV6
      ------------------------------------------------------------------*/
      if ((IFACE_IPV6_ADDR_FAMILY == companion_iface_iptype)
           && 
           (-1 == ds707_pdncntx_get_v6_rem_rtr_lifetime
                  (cmpn_iface_ptr, &(companion_iface_ip_cfg_ptr->router_life_time)))
         )
      {
        DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                   "can't get V6 rem rtr lifetime "
                   "for iface 0x%x:%d; failing IOCTL",
                   cmpn_iface_ptr->name,cmpn_iface_ptr->instance );
        companion_iface_ip_cfg_ptr->router_life_time = 0;
      }
#endif /* FEATURE_DATA_PS_IPV6 */
      
      handoff_cntx_ptr->bearer_ip_type = PS_IFACE_BEARER_IP_DUAL_BEARER;
      companion_iface_ip_cfg_ptr->is_ip_cfg_valid = TRUE;
    }
  }
  
  return 0;
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_EPC_HANDOFF_CTXT

DESCRIPTION   Handles the IOCTL to set epc handoff ctxt. It updates the IP
              config for the passed IFACE and companion IFACE (in case of
              dual IP connection)
    
PARAMETERS    argval_ptr: Pointer to acl_handoff_ctxt_type that contains IP
              config for this IFACE and companion IFACE (in case of dual IP)

DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_set_epc_handoff_ctxt
(
  ps_iface_type             *iface_ptr,
  void                      *argval_ptr
)
{
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
  acl_handoff_ctxt_type     *handoff_cntx_ptr = NULL;
  ps_iface_type             *v4_iface_ptr = NULL;
  ds707_iface_table_type    *iface4_tbl_ptr    = NULL;
  dssnet4_sm_cb_type        *dssnet4_instance = NULL;
  dssnet4i_state_type       dssnet4_sm_state = DSSNET4I_CLOSED_STATE;
  boolean                   is_apply_v4_success = FALSE;
  boolean                   v4_in_ppp_resync = FALSE;
  boolean                   retval = FALSE;
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type             *v6_iface_ptr = NULL;
  ds707_iface_table_type    *iface6_tbl_ptr    = NULL;
  dssnet6_sm_cb_type        *dssnet6_instance = NULL;
  dssnet6i_state_type       dssnet6_sm_state = DSSNET6I_CLOSED_STATE;
  boolean                   is_apply_v6_success = FALSE;
  boolean                   v6_in_ppp_resync = FALSE;
#endif/* FEATURE_DATA_PS_IPV6 */
#ifdef FEATURE_DATA_OPTHO
  ds707_s101_sm_e S101_State = DS707_S101_SM_NON_TUNNEL;
#endif /* FEATURE_DATA_OPTHO */

/*- - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG2(
                 MSG_LEGACY_HIGH,
                 "ds707_pdncntx_set_epc_handoff_ctxt, iface: 0x%x, argval_ptr: 0x%x",
                 iface_ptr, argval_ptr);  

  if ( (!PS_IFACE_IS_VALID(iface_ptr)) ||
       (argval_ptr == NULL) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,"invalid IFACE ptr or argval ptr" );
    return -1;
  }

  // get the pdn cntx cb from iface.
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
  if(!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "given iface 0x%x:%d not associated with any pdn, failing IOCTL",
                     iface_ptr->name,iface_ptr->instance );
    return -1;
  }

  handoff_cntx_ptr = (acl_handoff_ctxt_type*)argval_ptr;

#ifdef FEATURE_DATA_OPTHO
  S101_State = ds707_s101_query_sm_state();
#endif /* FEATURE_DATA_OPTHO */

  v4_iface_ptr = ds707_pdncntx_get_v4_iface( pdncntx_cb_ptr );
#ifdef FEATURE_DATA_PS_IPV6
  v6_iface_ptr = ds707_pdncntx_get_v6_iface( pdncntx_cb_ptr );
#endif /* FEATURE_DATA_PS_IPV6 */

  /*--------------------------------------------------------------------------- 
                        Applying IP config to V4 Iface
  ---------------------------------------------------------------------------*/
  if (NULL != v4_iface_ptr && 
      TRUE == handoff_cntx_ptr->pdn_ip_config.iface_ipv4_config.is_ip_cfg_valid
     )
  {
    iface4_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(v4_iface_ptr);
    dssnet4_instance = ds707_pdncntx_get_dssnet4_sm_ptr(iface4_tbl_ptr);
    dssnet4_sm_state = dssnet4_get_instance_state(dssnet4_instance);

    DATA_IS707_MSG3( MSG_LEGACY_HIGH,
                     "DSSNET4 state:%d for V4 Iface:0x%x:%d",
                     dssnet4_sm_state, v4_iface_ptr->name,
                     v4_iface_ptr->instance);

    /* Apply the IP cfg to iface only if DSSNET SM is in up state */
    if (NULL == dssnet4_instance || 
        DSSNET4I_NET_UP_STATE != dssnet4_sm_state)
    {
      if (DSSNET4I_LINK_RESYNC_STATE == dssnet4_sm_state)
      {
        v4_in_ppp_resync = TRUE;
      }
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "Ignore Ip cfg update in current DSSNET4 state %d",
                       dssnet4_sm_state );
    }
    else
    {
      if (0 == ps_iface_set_ip_config
                     (v4_iface_ptr, &(handoff_cntx_ptr->pdn_ip_config.iface_ipv4_config)))
      {
        is_apply_v4_success = TRUE;
      }
      DATA_IS707_MSG3( MSG_LEGACY_HIGH,
                       "Updating IP cfg for V4 Iface:0x%x:%d succeeded? %d",
                       v4_iface_ptr->name,v4_iface_ptr->instance, 
                       is_apply_v4_success );
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "PDN has no V4 Iface or IP container doesn't contain "
                    "V4 Information");
  }
 
#ifdef FEATURE_DATA_PS_IPV6
  /*--------------------------------------------------------------------------- 
                        Applying IP config to V6 Iface
  ---------------------------------------------------------------------------*/
  if (NULL != v6_iface_ptr && 
      TRUE == handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.is_ip_cfg_valid
     )
  {
#ifdef FEATURE_DATA_OPTHO
    if (DS707_S101_SM_NON_TUNNEL != S101_State)
    {
      /* We override the V6 timer values to be infinite while operating
         in tunnel mode. This ensures that the RS doesn't get triggered
         while operating in tunnel */
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,"Override: Set 0xFFFFFFFFUL to V6 "
                                       "preferred lifetime, valid lifetime & "
                                       "router lifetime");
      handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.prefix_pref_life_time 
        = 0xFFFFFFFFUL;
      handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.prefix_valid_life_time
        = 0xFFFFFFFFUL;
      handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.router_life_time
        = 0xFFFF;                                      
    }
#endif /* FEATURE_DATA_OPTHO */

    iface6_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(v6_iface_ptr);
    dssnet6_instance = ds707_pdncntx_get_dssnet6_sm_ptr(iface6_tbl_ptr);
    dssnet6_sm_state = dssnet6_get_instance_state(dssnet6_instance);
    DATA_IS707_MSG3( MSG_LEGACY_HIGH,
                     "DSSNET6 state:%d for V6 Iface:0x%x:%d",
                     dssnet6_sm_state, v6_iface_ptr->name,
                     v6_iface_ptr->instance);

    /* Apply the IP cfg to iface only if DSSNET SM is in up or waiting for
       net up state */
    if (NULL == dssnet6_instance || 
        (DSSNET6I_NET_UP_STATE != dssnet6_sm_state &&
         DSSNET6I_WAITING_FOR_NET_UP_STATE != dssnet6_sm_state)
       )
    {
      if (DSSNET6I_LINK_RESYNC_STATE == dssnet6_sm_state)
      {
        v6_in_ppp_resync = TRUE;
      }
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "Ignore Ip cfg update in current DSSNET6 state %d",
                       dssnet6_sm_state );
      /* We just ignore the set ioctl. No need to return -1 here */
    }
    else
    {
      DATA_IS707_MSG3(MSG_LEGACY_HIGH,
           "Life time values in V6 IP container: "
           "Preferred: %u, Valid: %u, Router: %d",
           handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.prefix_pref_life_time,
           handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.prefix_valid_life_time,
           handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.router_life_time);
      /* Set IP config only if prefix and router lifetime are non-zero */
      if ((0 != 
            handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.ip_addr.addr.v6.ps_s6_addr64[0]) &&
          (0 != 
            handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config.router_life_time) &&
          (0 == ps_iface_set_ip_config
                     (v6_iface_ptr, &(handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config)))
         )
      {
        DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                         "Updating IP cfg for V6 Iface:0x%x:%d Succeeded",
                         v6_iface_ptr->name,v6_iface_ptr->instance);
        is_apply_v6_success = TRUE;
      }
      else
      {
        DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                         "IPV6 Prefix/Router lifetime is 0 or Updating IP cfg for V6 "
                         "Iface:0x%x:%d failed", v6_iface_ptr->name,
                         v6_iface_ptr->instance);
      }
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "PDN has no V6 Iface or IP container doesn't contain "
                    "V6 Information");
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /****************************************************************************
    Now we decide whether to 
    1. Trigger NCP resync 
    2. Return failure/success to the ioctl 
   
    Logic:
      Tunnel Mode:
       1. requested iface == V4
          If apply IP succeeded or if the iface is in resync, return
          success. We don't care about the V6 iface. If apply failed
          for V6 and if there is no EPC client, we'll recover it when
          we go to direct mode
       2. requested iface == V6
          If apply IP succeeded, return success. We don't return success
          if the iface is in resync 'cos even after resync, the iface will
          remain in config state and relies on EPC to transfer the
          prefix, lifetime details from LTE. On moving to direct mode, EPC
          will abort this prereg and trigger a normal handoff
          Limitation:
          If the V6 iface undergoes resync when EPC tries to
          apply IP cfg, then we cannot preregister that iface. Revisit this
          if neccessary
   
       Non-Tunnel/Direct Mode:
       1. If all valid ifaces for the PDN succeed in applying IP Cfg or
          in resync, return success.
       2. Else trigger NCP resync to recover the PDN and send success back
          to EPC
  ****************************************************************************/
#ifdef FEATURE_DATA_OPTHO
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "ds707_pdncntx_set_epc_handoff_ctxt: In %d Mode",
                   S101_State);

  if (DS707_S101_SM_NON_TUNNEL != S101_State)
  {
    if(iface_ptr == v4_iface_ptr)
    {
      if ( is_apply_v4_success || v4_in_ppp_resync )
      {
        DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "Apply IP cfg succeeded/iface in PPP resync for the "
                        "requested V4 iface: 0x%x:%d. Return Success", 
                        iface_ptr->name, iface_ptr->instance);
        retval = TRUE;
      }
      else
      {
        DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                        "Apply IP cfg failed for the requested V4 iface: "
                        "0x%x:%d. Return Failure", iface_ptr->name, 
                        iface_ptr->instance);
        retval = FALSE;
      }
    }
#ifdef FEATURE_DATA_PS_IPV6
    else if (iface_ptr == v6_iface_ptr)
    {
      /* We don't return true if V6 went into PPP resync when EPC tried to
         apply IP cfg to move the iface to UP state. Because in tunnel
         even after resync, iface will remain in config state and cannot
         move to UP state until EPC reapplies the IP cfg. If we return TRUE
         here for resync, then EPC will think the apply succeeded and will wait
         for the iface to goto UP state and we'll be stuck in that state*/
      if (is_apply_v6_success)
      {
        DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "Apply IP cfg succeeded for the requested V6 iface: "
                        "0x%x:%d. Return Success", iface_ptr->name, 
                        iface_ptr->instance);
        retval = TRUE;
      }
      else
      {
        DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                        "Apply IP cfg failed for the requested V6 iface: "
                        "0x%x:%d. Return Failure", iface_ptr->name, 
                        iface_ptr->instance);
        retval = FALSE;
      }
    }
#endif /* FEATURE_DATA_PS_IPV6 */
    else
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Neither V4 nor V6 iface corresponds to the requested "
                       "iface: 0x%x:%d. Return Failure", 
                       iface_ptr->name, iface_ptr->instance);
      retval = FALSE;
    }
  }
  else
#endif /* FEATURE_DATA_OPTHO */
  {
    if (((NULL == v4_iface_ptr) || is_apply_v4_success || v4_in_ppp_resync) &&
        ((NULL == v6_iface_ptr) || is_apply_v6_success || v6_in_ppp_resync)
        )
    {
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                      "Apply IP cfg succeeded/iface in PPP resync for the "
                      "requested iface: 0x%x:%d. Return Success", 
                      iface_ptr->name, iface_ptr->instance);
      retval = TRUE;
    }
    else
    {
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                      "Apply IP cfg failed for the requested iface: 0x%x:%d "
                      "or the companion iface. Triggering NCP resync", 
                      iface_ptr->name, iface_ptr->instance);

      /* Trigger NCP resync to recover. This is applicable only in non
         tunnel mode. In tunnel mode, even if UE resyncs, it cannot get
         the ip cfg (DNS, SIP addr) from network. So ignore the failure
         in tunnel mode */
      if (0 == ds707_pdncntx_iface_reconfig(iface_ptr))
      {
        retval = TRUE;
      }
      else
      {
        retval = FALSE;
      }

      DATA_IS707_MSG3(MSG_LEGACY_HIGH,
                      "NCP resync triggered for iface: 0x%x:%d. "
                      "Return: %d", 
                      iface_ptr->name, iface_ptr->instance, retval);
    }
  } /* In Non-Tunnel Mode */

  if (retval)
    return 0;
  else
    return -1;

} /* ds707_pdncntx_set_epc_handoff_ctxt */
#endif /*FEATURE_EPC_HANDOFF*/

#ifdef FEATURE_CALL_THROTTLE
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DCTM_IND_POSTED

DESCRIPTION   Returns the boolean which indicates whether DCTM is 
              posted to CM  
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb

RETURN VALUE   boolean: dctm_ind_posted

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_dctm_ind_posted(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED,
                     "ds707_pdncntx_get_dctm_ind_posted: return %d",
                     pdncntx_cb_ptr->dctm_ind_posted );
    return (pdncntx_cb_ptr->dctm_ind_posted);
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_get_dctm_ind_posted: Invalid pdncntx_cb_ptr: 0x%x",
                     pdncntx_cb_ptr );
    return FALSE;
  }
}

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_DCTM_IND_POSTED

DESCRIPTION   Set the boolean, to the value passed. This flag indicates 
              whether DCTM is posted to CM
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb 
              boolean: to set the dctm_ind_posted flag 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_dctm_ind_posted(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  boolean                    dctm_posted
)
{
  if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED,
                     "ds707_pdncntx_set_dctm_ind_posted to %d", 
                     dctm_posted );
    pdncntx_cb_ptr->dctm_ind_posted = dctm_posted;
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds707_pdncntx_set_dctm_ind_posted: Invalid pdncntx_cb_ptr: 0x%x",
                     pdncntx_cb_ptr );
    return;
  }
}
#endif /* FEATURE_CALL_THROTTLE */


/*===========================================================================
FUNCTION      DS707_PDNCTX_POST_DSSNET6_EVENT

DESCRIPTION   Called to post events to dssnet6
    
PARAMETERS    event           - dssnet6 event
              ppp_status_ptr  - PPP status pointer

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_post_dssnet6_event 
(
  dssnet6_sm_event_type      event,
  ds_mip_ppp_status         *ppp_status_ptr
)
{
  ds707_pdn_context_cb_type *pdn_ctx_ptr   = NULL;
  ps_iface_type             *iface_ptr     = NULL;
  ds707_iface_table_type    *iface_tbl_ptr = NULL;
  uint8                      pdn_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ppp_status_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ppp_status_ptr is NULL");
    return;
  }

  /*------------------------------------------------------------------------
    Scan through all the PDNs & post events to each of the active Dssnet6's
  -------------------------------------------------------------------------*/
  for(pdn_idx = 0; pdn_idx < DS707_MAX_PDN_INSTANCES; pdn_idx++)
  {
    pdn_ctx_ptr = ds707_get_pdncntx_cb_from_instance(pdn_idx);

    /*------------------------------------------------------------------
      Check if the PDN context pointer is valid
    ------------------------------------------------------------------*/
    if (!DS707_PDNCNTX_CB_IS_VALID(pdn_ctx_ptr))
    {
      continue;
    }

    /*------------------------------------------------------------------
      Get v6 iface if valid
    ------------------------------------------------------------------*/
    if (NULL == (iface_ptr = ds707_pdncntx_get_v6_iface(pdn_ctx_ptr)))
    {
      continue;
    }

    /*------------------------------------------------------------------
      Post the event to the v6 iface if PPP event handle is valid
    ------------------------------------------------------------------*/
    if (NULL != (iface_tbl_ptr 
                 = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr)))
    {
      ppp_status_ptr->dssnet_instance = (void *)(&iface_tbl_ptr->dssnet6_sm_cb);
      if (NULL != iface_tbl_ptr->dssnet6_sm_cb.ppp_event_handle)
      {
        (void)dssnet6_sm_post_event(&iface_tbl_ptr->dssnet6_sm_cb, 
                                    event,
                                    (void *)ppp_status_ptr);
      }
    }
  }

  return;
} /* ds707_pdncntx_post_dssnet6_event() */

/*===========================================================================
FUNCTION      DS707_PDNCTX_POST_DSSNET4_EVENT

DESCRIPTION   Called to post events to dssnet4
    
PARAMETERS    event           - dssnet4 event
              ppp_status_ptr  - PPP status pointer

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_post_dssnet4_event 
(
  dssnet4_sm_event_type      event,
  ds_mip_ppp_status         *ppp_status_ptr
)
{
  ds707_pdn_context_cb_type *pdn_ctx_ptr   = NULL;
  ps_iface_type             *iface_ptr     = NULL;
  ds707_iface_table_type    *iface_tbl_ptr = NULL;
  uint8                      pdn_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ppp_status_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ppp_status_ptr is NULL");
    return;
  }

  /*------------------------------------------------------------------------
    Scan through all the PDNs & post events to each of the active Dssnet4's
  -------------------------------------------------------------------------*/
  for(pdn_idx = 0; pdn_idx < DS707_MAX_PDN_INSTANCES; pdn_idx++)
  {
    pdn_ctx_ptr = ds707_get_pdncntx_cb_from_instance(pdn_idx);

    /*------------------------------------------------------------------
      Check if the PDN context pointer is valid
    ------------------------------------------------------------------*/
    if (!DS707_PDNCNTX_CB_IS_VALID(pdn_ctx_ptr))
    {
      continue;
    }

    /*------------------------------------------------------------------
      Get v4 iface if valid
    ------------------------------------------------------------------*/
    if (NULL == (iface_ptr = ds707_pdncntx_get_v4_iface(pdn_ctx_ptr)))
    {
      continue;
    }

    /*------------------------------------------------------------------
      Post the event to the v4 iface if PPP event handle is valid
    ------------------------------------------------------------------*/
    if (NULL != (iface_tbl_ptr 
                 = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr)))
    {
      ppp_status_ptr->dssnet_instance = (void *)(&iface_tbl_ptr->dssnet4_sm_cb);
      if (NULL != iface_tbl_ptr->dssnet4_sm_cb.ppp_event_handle)
      {
        (void)dssnet4_sm_post_event(&iface_tbl_ptr->dssnet4_sm_cb, 
                                    event,
                                    (void *)ppp_status_ptr);
      }
    }
  }

  return;
} /* ds707_pdncntx_post_dssnet4_event() */

/*===========================================================================
FUNCTION      DS707_PDNCTX_PPP_EVENT_HDL

DESCRIPTION   Called by device manager to indicate about LCP/AUTH events.
    
PARAMETERS    protocol        - Protocol type
              ppp_event       - PPP event
              ppp_fail_reason - Failure reason

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_ppp_event_hdl
(
  ppp_protocol_e_type    protocol,
  ppp_event_e_type       ppp_event,
  ppp_fail_reason_e_type ppp_fail_reason
)
{
  ds_mip_ppp_status          ppp_status;
  boolean                    ignore_ppp_ev = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG3(MSG_LEGACY_HIGH, 
                  "Rcvd protocol %d event %d ppp_fail_reason %d",
                  protocol, ppp_event, ppp_fail_reason);

  ppp_status.ppp_protocol = protocol;
  ppp_status.ppp_fail_reason = ppp_fail_reason;

  /*--------------------------------------------------------------------------
    Currently only AUTH_STARTING_EVENT & LCP_DOWN events are posted to 
    the dssnets. AUTH_STARTING_EVENT is posted only for dssnet4's
  --------------------------------------------------------------------------*/
  switch (ppp_event)
  {
    case PPP_DOWN_EV:
      if (PPP_PROTOCOL_LCP == protocol)
      {
        ds707_pdncntx_post_dssnet4_event(DSSNET4_LINK_DOWN_EV, &ppp_status);
        ds707_pdncntx_post_dssnet6_event(DSSNET6_LINK_DOWN_EV, &ppp_status);
      }
      else
      {
        ignore_ppp_ev = TRUE;
      }
      break;

    case PPP_STARTING_EV:
      if (PPP_PROTOCOL_AUTH == protocol)
      {
        ds707_pdncntx_post_dssnet4_event(DSSNET4_PPP_AUTH_STARTED_EV, &ppp_status);
      }
      else
      {
        ignore_ppp_ev = TRUE;
      }
      break;

    default:
      ignore_ppp_ev = TRUE;
      break;
  }

  if (ignore_ppp_ev)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Ignoring ppp event %d", ppp_event);
  }

  return;
} /* ds707_pdncntx_ppp_event_hdl() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_CURRENT_PROFILE_ID

DESCRIPTION   This function sets the current data session profile id in 
              data_session_info in the pdn cb

PARAMETERS    pdn_cb_ptr *: pointer to PDN context cb
              ds707_data_session_profile_id : 
                    Current data session profile id for the iface.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_set_current_profile_id
(
  ds707_pdn_context_cb_type     *pdn_cb_ptr,
  ds707_data_session_profile_id  current_profile_id,
  ds_profile_subs_id_enum_type   subs_id
)
{
  ds707_data_session_info_type *session_info_ptr = NULL;
  ds707_data_session_profile_info_type *data_sess_profile_ptr = NULL;
/*----------------------------------------------------------------------*/
  if ( DS707_PDNCNTX_CB_IS_VALID(pdn_cb_ptr) )
  {
    session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(pdn_cb_ptr);
    if (DATA_SESSION_IS_VALID(session_info_ptr))
    {
      /*------------------------------------------------------- 
       In case of clean up
       -------------------------------------------------------*/
      if (current_profile_id == DATA_SESSION_PROFILE_ID_INVALID)
      {
        memset((void*)&session_info_ptr->iface_profile_info.current_data_session_profile,
               0,
               sizeof(ds707_data_session_profile_info_type));
        session_info_ptr->iface_profile_info.current_data_session_profile.data_session_profile_id = 
           DATA_SESSION_PROFILE_ID_INVALID;
      }
      else
      {
        if ( TRUE == ds707_data_session_get_profile( current_profile_id,
                                                     subs_id,
                                                     &data_sess_profile_ptr ) )
        {
          session_info_ptr->iface_profile_info.current_data_session_profile = 
            *data_sess_profile_ptr;
  
          if (PROFILE_ID_IS_MIP(current_profile_id))
          {
            /*------------------------------------------------------------------ 
              If it's a MIP profile, default profile is copied. Overwrite
              profile ID here.
             ------------------------------------------------------------------*/
            session_info_ptr->iface_profile_info.current_data_session_profile.data_session_profile_id = 
               current_profile_id;
          }
        }
      }

      DATA_IS707_MSG2( MSG_LEGACY_MED, 
                       "Setting current profile id to %d for PDN cb 0x%x",
                       session_info_ptr->iface_profile_info.current_data_session_profile.data_session_profile_id,
                       pdn_cb_ptr );
    }
  }
} /* ds707_pdncntx_set_current_profile_id() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_REQUESTING_PROFILE_ID

DESCRIPTION   This function sets the requesting data session profile id in 
              data_session_info in the pdn cb

PARAMETERS    pdn_cb_ptr *: pointer to PDN context cb
              ds707_data_session_profile_id : 
                   Requesting data session profile id for the iface.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_set_requesting_profile_id
(
  ds707_pdn_context_cb_type     *pdn_cb_ptr,
  ds707_data_session_profile_id  requesting_profile_id,
  ds_profile_subs_id_enum_type   subs_id
)
{
  ds707_data_session_info_type * session_info_ptr = NULL;
  ds707_data_session_profile_info_type *data_sess_profile_ptr = NULL;
/*----------------------------------------------------------------------*/
  if ( DS707_PDNCNTX_CB_IS_VALID(pdn_cb_ptr) )
  {
    session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(pdn_cb_ptr);
    if (DATA_SESSION_IS_VALID(session_info_ptr))
    {
      /*------------------------------------------------------- 
       In case of clean up
       -------------------------------------------------------*/
      if (requesting_profile_id == DATA_SESSION_PROFILE_ID_INVALID)
      {
        memset((void*)&session_info_ptr->iface_profile_info.requesting_data_session_profile,
               0,
               sizeof(ds707_data_session_profile_info_type));
        session_info_ptr->iface_profile_info.requesting_data_session_profile.data_session_profile_id = 
           DATA_SESSION_PROFILE_ID_INVALID;
      }
      else
      {
        if ( TRUE == ds707_data_session_get_profile( requesting_profile_id,
                                                     subs_id,
                                                     &data_sess_profile_ptr ) )
        {
          session_info_ptr->iface_profile_info.requesting_data_session_profile = 
            *data_sess_profile_ptr;
  
          if (PROFILE_ID_IS_MIP(requesting_profile_id))
          {
            /*------------------------------------------------------------------ 
              If it's a MIP profile, default profile is copied. Overwrite
              profile ID here.
             ------------------------------------------------------------------*/
            session_info_ptr->iface_profile_info.requesting_data_session_profile.data_session_profile_id = 
               requesting_profile_id;
          }
        }
      }
    }
    DATA_IS707_MSG2( MSG_LEGACY_MED, 
                     "Setting requesting profile id to %d for PDN cb 0x%x",
                     session_info_ptr->iface_profile_info.requesting_data_session_profile.data_session_profile_id, 
                     pdn_cb_ptr );
  }
} /* ds707_pdncntx_set_requesting_profile_id() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_IFACE_LINGER_STATUS

DESCRIPTION   This function sets the linger status in the data_session_info 
              in pdn cb

PARAMETERS    pdn_cb_ptr *: pointer to PDN context cb
              boolean        :  TRUE - IFACE lingering else FALSE
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_set_iface_linger_status
(
  ds707_pdn_context_cb_type  *pdn_cb_ptr,
  boolean                     is_iface_lingering
)
{
  ds707_data_session_info_type * session_info_ptr = NULL;
/*----------------------------------------------------------------------*/
  if ( DS707_PDNCNTX_CB_IS_VALID(pdn_cb_ptr) )
  {
    session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(pdn_cb_ptr);
    if (DATA_SESSION_IS_VALID(session_info_ptr))
    {
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "Setting iface linger status to %d",
                       is_iface_lingering );
      session_info_ptr->is_iface_lingering = is_iface_lingering;
    }
  }
} /* ds707_pdncntx_set_iface_linger_status() */

/*===========================================================================
FUNCTION DS707_PDNCTX_PDN_CLEANUP

DESCRIPTION
  This function cleans up the PDN associated with the IFACE

PARAMETERS
  pdncntx_cb_ptr - PDN context control block pointer

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS  
  None
===========================================================================*/
void ds707_pdncntx_pdn_cleanup
(
  ds707_pdn_context_cb_type    *pdncntx_cb_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "pdncntx_cb_ptr is invalid");
    return;
  }

#ifdef FEATURE_EHRPD
  /*--------------------------------------------------------------------
    Clear default PDN if it is this PDN
  --------------------------------------------------------------------*/
  if( ds707_pdncntx_is_default_pdn(pdncntx_cb_ptr) )
  {
    ds707_pdncntx_reset_default_pdn();
  }
#endif /* FEATURE_EHRPD */

  /*--------------------------------------------------------------------
    update current id to INVALID.
  --------------------------------------------------------------------*/
  ds707_pdncntx_set_current_profile_id(pdncntx_cb_ptr,
                                       DATA_SESSION_PROFILE_ID_INVALID,
                                       ACTIVE_SUBSCRIPTION_1);

  /*-------------------------------------------------------------------
    Set the iface linger status to FALSE
  -------------------------------------------------------------------*/  
  ds707_pdncntx_set_iface_linger_status(pdncntx_cb_ptr, FALSE);

  /*--------------------------------------------------------------------
   Free the pdn and set req id to invalid
  ---------------------------------------------------------------------*/
  ds707_pdncntx_set_requesting_profile_id(pdncntx_cb_ptr, 
                                          DATA_SESSION_PROFILE_ID_INVALID,
                                          ACTIVE_SUBSCRIPTION_1);

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "All iface tbl entries for this PDN are NULL, free this PDN");
#ifdef FEATURE_EHRPD
  if (ds_ehrpd_pdn_mgr_free_pdn_id((void*)pdncntx_cb_ptr) == FALSE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "Cannot free PDN Instance:0x%x", 
                       pdncntx_cb_ptr);
  }
  ds707_pdncntx_set_ncp_started(pdncntx_cb_ptr, FALSE);
  ds707_pdncntx_set_ncp_handle(pdncntx_cb_ptr, NULL);

  ds707_pdncntx_reset_call_bringup_in_tunnel_mode(pdncntx_cb_ptr);

  ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, FALSE);

  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Iface down, freeing inactivity timer, pdn 0x%x",
                    pdncntx_cb_ptr); 
  rex_undef_timer(&(pdncntx_cb_ptr->inactivity_timer)); 

#endif /* FEATURE_EHRPD */
  ds707_pdncntx_set_ppp_resync_lock(pdncntx_cb_ptr, FALSE);

#ifdef FEATURE_EPC_HANDOFF
  /*---------------------------------------------------------------
     reset the EPC handoff context information
  ---------------------------------------------------------------*/
  if (pdncntx_cb_ptr->handover_ip_config_handle != NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "Freeing up dynamic memory allocated for IP context");
    modem_mem_free( pdncntx_cb_ptr->handover_ip_config_handle,
                    MODEM_MEM_CLIENT_DATA_CRIT);
    pdncntx_cb_ptr->handover_ip_config_handle = NULL;
  }
  /*-----------------------------------------------------------------------
    Reset the handover_attach flag
  -----------------------------------------------------------------------*/
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Setting the handover attach flag to FALSE");
  pdncntx_cb_ptr->handover_attach = FALSE;
#endif
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, FALSE);
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_DATA_FALLBACK_LIST
  /*-----------------------------------------------------------------------
    clear the pdn_fb_cnt_added flag
  -----------------------------------------------------------------------*/
  ds707_pdncntx_set_pdn_fb_cnt_added(pdncntx_cb_ptr, FALSE);
#endif
  /*--------------------------------------------------------------------
    Reset PDN control block flags
  --------------------------------------------------------------------*/
  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Reset PDN control block flags");

  pdncntx_cb_ptr->iface_down_flag = DS707_PDNCNTX_IFACE_DOWN_NONE;

  ds707_pdncntx_free_pdncntx_cb(pdncntx_cb_ptr);

#ifdef FEATURE_EHRPD
  /*-------------------------------------------------------------------
    Notify PDN context manager about PDN DOWN event
  -------------------------------------------------------------------*/
  ds707_pdnctx_pdn_down_ind();
#endif /* FEATURE_EHRPD */

} /* ds707_pdncntx_pdn_cleanup() */

/*===========================================================================
FUNCTION DS707_PDNCTX_SET_OLDEST_ACTIVE_IFACE

DESCRIPTION
  This function sets the oldest active IFACE

PARAMETERS
  this_iface_ptr - IFACE pointer

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS  
  None
===========================================================================*/
void ds707_pdncntx_set_oldest_active_iface
(
  ps_iface_type   *this_iface_ptr
)
{
  uint8           curr_iface_index = 0;
  ps_iface_type  *curr_iface_ptr   = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*--------------------------------------------------------------------
    If the oldest_active_iface is going down, then find another iface 
    that is up, and update oldest_active_iface. This is used to return
    the profile_id of the oldest_active_iface, for an upcomign call.


    NOTE: This logic works as we have only two ifaces, when there are 
          more than two ifaces this needs to be revisited.
  --------------------------------------------------------------------*/
  if ( ds707_data_session_get_oldest_active_iface_ptr() == this_iface_ptr )
  {
    for ( curr_iface_index = 0;
          curr_iface_index < DS707_MAX_IFACES;
          curr_iface_index ++ )
    {
      curr_iface_ptr =  
        ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
      if ( PS_IFACE_IS_VALID( curr_iface_ptr ) &&
           PS_IFACE_IS_UP( curr_iface_ptr ) )
      {     
        if ( this_iface_ptr != curr_iface_ptr )
        {
          ds707_data_session_set_oldest_active_iface_ptr(curr_iface_ptr);
          break;
        }
      } /* if the current iface is up */
    } /* for each iface */
  } /* if the oldest active iface is going down */  
  
  if ( curr_iface_index == DS707_MAX_IFACES )
  {
    ds707_data_session_set_oldest_active_iface_ptr(NULL);
  }
} /* ds707_pdncntx_set_oldest_active_iface() */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION DS707_PDNCTX_DSSNET6_GET_CALL_END_REASON

DESCRIPTION
  To get call end reason for a dssnet6 instance

PARAMETERS  pdncntx_cb_ptr   - PDN context cb pointer

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS  
  None
===========================================================================*/
ps_iface_net_down_reason_type ds707_pdnctx_dssnet6_get_call_end_reason
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  ps_iface_net_down_reason_type dssnet6_call_end_reason 
                                            = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  dssnet6_sm_cb_type        *dssnet6_instance = NULL;
  ps_iface_type             *iface6_ptr       = NULL;
  ds707_iface_table_type    *iface6_tbl_ptr   = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "PDN context cb ptr is invalid");
    return dssnet6_call_end_reason;
  }

  iface6_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
  if (PS_IFACE_IS_VALID(iface6_ptr))
  {
    iface6_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface6_ptr);

    dssnet6_instance = ds707_pdncntx_get_dssnet6_sm_ptr(iface6_tbl_ptr);

    dssnet6_call_end_reason 
                  = dssnet6_get_instance_call_end_reason(dssnet6_instance);
  }

  return dssnet6_call_end_reason;
} /* ds707_pdnctx_dssnet6_get_call_end_reason() */
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION DS707_PDNCTX_POST_IFACE_DOWN

DESCRIPTION
  This function does additional handling before v4/v6 iface goes down. It 
  sees that both the v4/v6 ifaces go down together to avoid race conditions

PARAMETERS  iface_ptr   - IFACE pointer
            down_reason - IFACE down reason

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS  
  None
===========================================================================*/
void ds707_pdncntx_post_iface_down
(
  ps_iface_type                 *iface_ptr,
  ps_iface_net_down_reason_type  down_reason
)
{
  ps_iface_type             *iface4_ptr = NULL;
  ps_phys_link_type         *phys_link_ptr = NULL;
  ds707_pdn_context_cb_type *pdncntx_cb_ptr   = NULL;
  ds707_iface_table_type    *iface4_tbl_ptr    = NULL;
  dssnet4_sm_cb_type        *dssnet4_instance = NULL;
  ps_iface_net_down_reason_type dssnet4_call_end_reason 
                                            = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  boolean post_v4_iface_down = FALSE;   
  ps_iface_down_mh_event_info_type      iface_down_ind_info;
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type             *iface6_ptr = NULL;
  ds707_iface_table_type    *iface6_tbl_ptr    = NULL;
  dssnet6_sm_cb_type        *dssnet6_instance = NULL;
  ps_iface_net_down_reason_type dssnet6_call_end_reason 
                                            = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  boolean post_v6_iface_down = FALSE;
#endif /* FEATURE_DATA_PS_IPV6 */
  boolean cleanup_pdn        = FALSE;
  boolean v4_iface_down = TRUE;
  boolean v6_iface_down = TRUE;
  int16 ps_errno;
  ds707_pdn_context_iface_down_mask iface_down_flag,old_iface_down_flag;
  boolean resync_ppp_session = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == iface_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "iface_ptr is NULL");
    return;
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "ds707_pdncntx_handle_iface_down for iface 0x%x:%d",
                  iface_ptr->name, iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Verify the iface state is not in DOWN state.
  -------------------------------------------------------------------------*/
  if(IFACE_DOWN == ps_iface_state(iface_ptr))
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "Invalid iface state %d",ps_iface_state(iface_ptr));
  }

  memset(&iface_down_ind_info, 0, sizeof(iface_down_ind_info));
  /*---------------------------------------------------------------------
    Set PPP termination policy.
  ---------------------------------------------------------------------*/
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, FALSE);
  }
  else
  {
    PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, TRUE);
  }

  /*-------------------------------------------------------------------------
    Get the pdn context cb from iface
  -------------------------------------------------------------------------*/
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,"707 PDN context cb ptr is invalid");
    /*----------------------------------------------------------------------- 
     Chances are that routing is done in the middle of cleanup, and when
     bringup cmd is processed, the PDN CB ptr has been cleaned up. Post iface
     down here, otherwise it's possible that the iface gets stuck in going
     down state.
    -----------------------------------------------------------------------*/
    ds707_pkt_mgr_get_bearer_tech_info_ex( &iface_down_ind_info.bearer_tech);
    iface_down_ind_info.down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED; 

    ps_iface_down_ind(iface_ptr, &iface_down_ind_info);

    ds707_pdncntx_set_oldest_active_iface(iface_ptr);
    return;
  }

  /*---------------------------------------------------------------------------
    Check if both v4 & v6 ifaces are valid.If one of them is not valid continue 
    posting DOWN event to the one that is valid.
    If they are valid then check if the iface_down_flag is set for both ifaces.  
    If yes, then we post DOWN events to both the ifaces together. 
    If call end reason is SUBS limited(V4 & v6) or PREFIX unavilable/IPv6 
    diabled on HRPD(only V6) then DOWN event has to be posted to the particular
    iface but PDN cleanup won't happen.
  ---------------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------
    Get DSSNET4 info if V4 iface is valid
  ---------------------------------------------------------------------------*/
  iface4_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
  if (PS_IFACE_IS_VALID(iface4_ptr))
  {
    iface4_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface4_ptr);

    dssnet4_instance = ds707_pdncntx_get_dssnet4_sm_ptr(iface4_tbl_ptr);
    if (NULL == dssnet4_instance)
    {
      DATA_ERR_FATAL("Invalid dssnet4 instance");
      return;
    }

    dssnet4_call_end_reason 
                      = dssnet4_get_instance_call_end_reason(dssnet4_instance);

    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
		    "post_iface_Down ps_iface_state(iface4_ptr) = %d",
                    ps_iface_state(iface4_ptr));
    if ((IFACE_DOWN != ps_iface_state(iface4_ptr))
       && (IFACE_STATE_INVALID != ps_iface_state(iface4_ptr))
       && (IFACE_DISABLED != ps_iface_state(iface4_ptr)))
    {
      v4_iface_down = FALSE;
    }
   }

#ifdef FEATURE_DATA_PS_IPV6
  /*---------------------------------------------------------------------------
    Get DSSNET6 info if V6 iface is valid
  ---------------------------------------------------------------------------*/
  iface6_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
  if (PS_IFACE_IS_VALID(iface6_ptr))
  {
    iface6_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface6_ptr);

    dssnet6_instance = ds707_pdncntx_get_dssnet6_sm_ptr(iface6_tbl_ptr);
    if (NULL == dssnet6_instance)
    {
      DATA_ERR_FATAL("Invalid dssnet6 instance");
      return;
    }
    dssnet6_call_end_reason 
                      = dssnet6_get_instance_call_end_reason(dssnet6_instance);

    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
		    "post_iface_Down ps_iface_state(iface6_ptr) = %d",
                    ps_iface_state(iface6_ptr));
    if ((IFACE_DOWN != ps_iface_state(iface6_ptr))
       && (IFACE_STATE_INVALID != ps_iface_state(iface6_ptr))
       && (IFACE_DISABLED != ps_iface_state(iface6_ptr)))
    {
      v6_iface_down = FALSE;
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  iface_down_flag = ds707_pdncntx_get_iface_down_flag(pdncntx_cb_ptr);
  old_iface_down_flag = iface_down_flag;
  /* Update the iface_down_flag with the new mask as well */
  if((iface_ptr == iface4_ptr) &&
     (iface_down_flag != DS707_PDNCNTX_IFACE_DOWN_V4))
  {
    iface_down_flag |= DS707_PDNCNTX_IFACE_DOWN_V4;
  }
  else if((iface_ptr == iface6_ptr)&&
          (iface_down_flag != DS707_PDNCNTX_IFACE_DOWN_V6))
  {
    iface_down_flag |= DS707_PDNCNTX_IFACE_DOWN_V6;
  }
  else
  {
    /* Re entered for the same iface hence ignore this callback*/
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
		    "post_iface_Down Re entered for the same iface"
                    " hence ignore this callback");
    return;
  }

  /*-------------------------------------------------------------------------
    Decide if iface down needs to be posted
  -------------------------------------------------------------------------*/
  if ((PS_IFACE_IS_VALID(iface4_ptr))
#ifdef FEATURE_DATA_PS_IPV6 
      && ((TRUE == v6_iface_down) 
          || (PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V6 == dssnet4_call_end_reason)
          || (PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED == dssnet4_call_end_reason)
          || (PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED  == dssnet4_call_end_reason)
          || (DS707_PDNCNTX_IFACE_DOWN_NONE != old_iface_down_flag ))
#endif /* FEATURE_DATA_PS_IPV6 */
     )
  {
    post_v4_iface_down = TRUE;
  }
  
#ifdef FEATURE_DATA_PS_IPV6
  if ((PS_IFACE_IS_VALID(iface6_ptr)) 
      && ((TRUE == v4_iface_down)
          || (PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V4 == dssnet6_call_end_reason)
          || (PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE == dssnet6_call_end_reason)
          || (PS_NET_DOWN_REASON_IPV6_ERR_HRPD_IPV6_DISABLED == dssnet6_call_end_reason)
          || (PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED == dssnet6_call_end_reason)
          || (PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED  == dssnet6_call_end_reason)
          || (PS_NET_DOWN_REASON_IPV6_DISABLED                == dssnet6_call_end_reason)
          || (DS707_PDNCNTX_IFACE_DOWN_NONE != old_iface_down_flag ))
     )
    {
      post_v6_iface_down = TRUE;
    }

  DATA_IS707_MSG4(MSG_LEGACY_HIGH,
                  "v4_iface_down %d v6_iface_down %d"
                  "post_v6_iface_down %d post_v4_iface_down %d",
                  v4_iface_down, v6_iface_down, 
                  post_v6_iface_down, post_v4_iface_down);
  /*-------------------------------------------------------------------------
    Set post_v4_iface_down to TRUE if post_v6_iface_down is TRUE & this
    function was called for a V4 iface. This will avoid going ahead with
    posting iface down ind to v6 iface while iface4 & PDN CB are not cleaned
    up properly (& vice versa).
  -------------------------------------------------------------------------*/
  if (((iface_ptr == iface4_ptr) && (TRUE == post_v6_iface_down)) 
      || ((iface_ptr == iface6_ptr) && (TRUE == post_v4_iface_down)))
  {
    post_v4_iface_down = TRUE;
    post_v6_iface_down = TRUE;
  }

  /*-------------------------------------------------------------------------
    If post iface down is true for both IP types, but get_first_iface_down
    flag is false it implies that this is the first iface going down for a
    dual IP PDN. So just set the first iface down flag to true and return.
    When second iface comes here it'll post down for both ifaces and
    cleanup the PDN as well
   
    E.g: Scenario: both V4 and V6 ip types are throttled and a new call is made
    for the same APN. Both dssnets will call this function back to back.
    When the first dssnet(say v4) calls this function the end reason might
    already be set for dssnet6 and so the post_v4/6_iface_down flag will
    be true for both. In this case, we need to wait for the second dssnet to
    also go down and only then we should post iface down for both IP types
    and also cleanup the PDN
  -------------------------------------------------------------------------*/
  if ( (((iface_ptr == iface6_ptr)&& (IFACE_DOWN != ps_iface_state(iface4_ptr))) ||
        ((iface_ptr == iface4_ptr)&& (IFACE_DOWN != ps_iface_state(iface6_ptr)))
       )&&
       ((TRUE == post_v4_iface_down) && (TRUE == post_v6_iface_down) &&
        (DS707_PDNCNTX_IFACE_DOWN_NONE == old_iface_down_flag)
        )
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "Both post_v4/6_iface_down flags are set but "
                    "get_iface_down_flag is not set for both ifaces"
                    "then return ");
    /* Wait for the next iface down cb to come in*/
    ds707_pdncntx_set_iface_down_flag(pdncntx_cb_ptr, iface_down_flag);
    return;
  }

  /*-------------------------------------------------------------------------
    Decide PDN cleanup needs to be done
  -------------------------------------------------------------------------*/
  if(((TRUE == v6_iface_down) && (TRUE == post_v4_iface_down))
     || ((TRUE == v4_iface_down) && (TRUE == post_v6_iface_down))
     || ( DS707_PDNCNTX_IFACE_DOWN_NONE != old_iface_down_flag )
    )
  {
      cleanup_pdn = TRUE;
    }
#else
    cleanup_pdn = TRUE;
#endif /* FEATURE_DATA_PS_IPV6 */

  /*---------------------------------------------------------------------------
    Post DOWN event to v4 iface post_v4_iface_down flag is set
  ---------------------------------------------------------------------------*/
  if (TRUE == post_v4_iface_down)
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                    "MH posting iface down ind on 0x%x (V4 Iface), "
                    "reason: 0x%x",
                    iface4_ptr, dssnet4_call_end_reason);

    ds707_pkt_mgr_get_bearer_tech_info_ex( &iface_down_ind_info.bearer_tech);
    iface_down_ind_info.down_reason = dssnet4_call_end_reason;  
    ps_iface_down_ind(iface4_ptr, &iface_down_ind_info );

    /*-----------------------------------------------------------------------
      Set the oldest active IFACE pointer
    -----------------------------------------------------------------------*/
    ds707_pdncntx_set_oldest_active_iface(iface4_ptr);

    dssnet4_reset_call_end_reason(dssnet4_instance);
    dssnet4_reset_local_abort(dssnet4_instance);
  }

#ifdef FEATURE_DATA_PS_IPV6
  /*---------------------------------------------------------------------------
    Post DOWN event to v6 iface post_v6_iface_down flag is set
  ---------------------------------------------------------------------------*/
  if (TRUE == post_v6_iface_down)
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                    "MH posting iface down ind on 0x%x (V6 Iface), reason: 0x%x",
                    iface6_ptr, dssnet6_call_end_reason);

    memset(&iface_down_ind_info, 0, sizeof(iface_down_ind_info));
    ds707_pkt_mgr_get_bearer_tech_info_ex( &iface_down_ind_info.bearer_tech);
    iface_down_ind_info.down_reason = dssnet6_call_end_reason;  
    ps_iface_down_ind(iface6_ptr, &iface_down_ind_info);

    /*-----------------------------------------------------------------------
      Set the oldest active IFACE pointer
    -----------------------------------------------------------------------*/
    ds707_pdncntx_set_oldest_active_iface(iface6_ptr);

    dssnet6_reset_call_end_reason(dssnet6_instance);
    dssnet6_reset_local_abort(dssnet6_instance);
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
    Cleanup all the associations. This is required to handle dual ip cleanup
    cases where the call fails before phys link comes up.
  -------------------------------------------------------------------------*/
  if ((TRUE == post_v4_iface_down) || (TRUE == cleanup_pdn))
  {
    /*-----------------------------------------------------------------------
      Set pdn_cntx_ptr to NULL
    -----------------------------------------------------------------------*/
    if (DS707_IFACE_TBL_IS_VALID(iface4_tbl_ptr))
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                      "Setting pdn cntx ptr to NULL for iface tbl 0x%x",
                      iface4_tbl_ptr);
      iface4_tbl_ptr->pdn_cntx_ptr = NULL;
    }

    /*-----------------------------------------------------------------------
      Set V4 iface associated with pdn context to NULL
    -----------------------------------------------------------------------*/
    ds707_pdncntx_set_v4_iface(pdncntx_cb_ptr, NULL);  

    /*-----------------------------------------------------------------------
      Set is_active flag to FALSE
    -----------------------------------------------------------------------*/
    PS_IFACE_SET_IS_ACTIVE(iface4_ptr, FALSE);
  }

#ifdef FEATURE_DATA_PS_IPV6
  if ((TRUE == post_v6_iface_down) || (TRUE == cleanup_pdn))
  {
    /*-----------------------------------------------------------------------
      Set pdn_cntx_ptr to NULL
    -----------------------------------------------------------------------*/
    if (DS707_IFACE_TBL_IS_VALID(iface6_tbl_ptr))
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                      "Setting pdn cntx ptr to NULL for iface tbl 0x%x",
                      iface6_tbl_ptr);
      iface6_tbl_ptr->pdn_cntx_ptr = NULL;
    }

    /*-----------------------------------------------------------------------
      Set V6 iface associated with pdn context to NULL
    -----------------------------------------------------------------------*/
    ds707_pdncntx_set_v6_iface(pdncntx_cb_ptr, NULL);

    /*-----------------------------------------------------------------------
      Set is_active flag to FALSE
    -----------------------------------------------------------------------*/
    PS_IFACE_SET_IS_ACTIVE(iface6_ptr, FALSE);
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  ds707_pdncntx_set_iface_down_flag(pdncntx_cb_ptr, iface_down_flag);
  /*--------------------------------------------------------------------
    Cleanup the PDN
  -------------------------------------------------------------------*/
  if (TRUE == cleanup_pdn)
  {
    /*--------------------------------------------------------------------
      Set PPP resync flag for cause codes 9 and 11, if there are other 
      active PDNs.
    -------------------------------------------------------------------*/
    if(((dssnet4_call_end_reason == PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_ID_IN_USE) ||
        (dssnet4_call_end_reason == PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN) ||
        (dssnet6_call_end_reason == PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_ID_IN_USE) ||
        (dssnet6_call_end_reason == PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN))&&
        (!ds707_pdncntx_is_last_active_pdn(pdncntx_cb_ptr)))
    {
      resync_ppp_session = TRUE;
    }

    ds707_pdncntx_pdn_cleanup(pdncntx_cb_ptr);

    if(resync_ppp_session == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "PDN DOWN with cause code 9 or 11, resync PPP!");
      if (ppp_resync( PPP_UM_SN_DEV, PPP_PROTOCOL_LCP, NULL) == -1)
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, "UE Init Resync Failed"); 
      }
    }
  }

  /*--------------------------------------------------------------------
    This is to handle the case where the first iface got a NCP down from
    PPP even before the second iface got a chance to register for the
    NCP events. In this case, we will eternally wait for the second
    iface to go down to post iface down on both ifaces and cleanup the
    PDN.
    This is needed only in dual ip scenario. Hence featurizing under
    IPV6
  -------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if (FALSE == cleanup_pdn)
  {
    /* At this moment only one iface is ready to go down and waiting for the
       other iface to go down */
    phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(iface_ptr);
    if ((FALSE == post_v4_iface_down) && (FALSE == post_v6_iface_down) &&
        (PS_PHYS_LINK_GET_STATE(phys_link_ptr) != PHYS_LINK_NULL))
    {
      if ((iface_ptr == iface4_ptr) && 
         (IFACE_COMING_UP == ps_iface_state(iface6_ptr)))
      {
        /* It's possible the V6 iface will also go down automatically or
           it may get stuck. In any case blindly call gonull on v6 iface
           Set the end reason in DSSNET6 based on dssnet4. This is needed
           if the iface go null cmd is processed first, it'll end up
           reporting client_end as the end reason
        */
        if (PS_NET_DOWN_REASON_NOT_SPECIFIED != dssnet4_call_end_reason &&
            NULL != dssnet6_instance)
        {
          dssnet6_set_ps_call_end_reason(dssnet6_instance, 
                                         dssnet4_call_end_reason);
        }
        if( !ds3g_is_ipv6_enabled() )
        {
          DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                          "V6 Iface cleanup 0x%x: %d IPV6 disabled",
                          iface6_ptr->name,iface6_ptr->instance);
          ds707_post_flow_gonull_iface_down_ind(iface6_ptr, 
                               PS_NET_DOWN_REASON_IPV6_DISABLED);
        }
        else
        {
          DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                    "Posting iface_go_null_cmd for V6 iface 0x%x:%d",
                    iface6_ptr->name, iface6_ptr->instance);
          (void) ps_iface_go_null_cmd(iface6_ptr,&ps_errno,NULL);
        }
      }
      else if ((iface_ptr == iface6_ptr) && 
         (IFACE_COMING_UP == ps_iface_state(iface4_ptr)))
      {
        /* It's possible the V4 iface will also go down automatically or
           it may get stuck. In any case blindly call gonull on v4 iface
           Set the end reason in DSSNET4 based on DSSNET6. This is needed
           if the iface go null cmd is processed first, it'll end up
           reporting client_end as the end reason
        */
        if (PS_NET_DOWN_REASON_NOT_SPECIFIED != dssnet6_call_end_reason &&
            NULL != dssnet4_instance)
        {
          dssnet4_set_ps_call_end_reason(dssnet4_instance, 
                                         dssnet6_call_end_reason);
        }
        DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "Posting iface_go_null_cmd for V4 iface 0x%x:%d",
                  iface4_ptr->name, iface4_ptr->instance);
        (void) ps_iface_go_null_cmd(iface4_ptr,&ps_errno,NULL);
      }
    }/* Not posting down on any iface */
  } /* Not cleaning up PDN */

#endif /* FEATURE_DATA_PS_IPV6 */

} /* ds707_pdncntx_post_iface_down() */

/*===========================================================================
FUNCTION DS707_PDNCTX_DUN_CLEANUP

DESCRIPTION
  This function handles cleanup for dun calls

PARAMETERS  this_iface_ptr - IFACE pointer

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS  
  None
===========================================================================*/
void ds707_pdncntx_dun_cleanup
(
  ps_iface_type   *this_iface_ptr
)
{
  ds707_pdn_context_cb_type   *pdncntx_cb_ptr   = NULL;
  ds707_iface_table_type      *iface_tbl_ptr    = NULL;
  uint8                        curr_iface_index = 0;
  boolean                      linger_status;
  boolean                      last_iface_down = FALSE;
  ps_iface_type               *temp_iface_ptr  = NULL;
#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_EHRPD_DUAL_IP)
  ps_iface_type                *v6_iface_ptr   = NULL;
#endif /* FEATURE_EHRPD && FEATURE_DATA_EHRPD_DUAL_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!PS_IFACE_IS_VALID(this_iface_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ds707_pdncntx_dun_cleanup: this_iface_ptr invalid");
    return;
  }

  /*--------------------------------------------------------------------
    Get iface tbl ptr for this iface
  ---------------------------------------------------------------------*/
  iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(this_iface_ptr);
  if(!DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ds707_pdncntx_dun_cleanup: pdn_cb_ptr invalid");
    return;
  }

  /*--------------------------------------------------------------------
    Get PDN cb ptr from iface_tbl_ptr
  ---------------------------------------------------------------------*/
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr(iface_tbl_ptr);
  if(!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ds707_pdncntx_dun_cleanup: pdn_cb_ptr invalid");
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                  "re-setting graceful_ppp_close for IFACE: 0x%x:%d",
                  this_iface_ptr->name,
                  this_iface_ptr->instance);

  /*---------------------------------------------------------------------
    Set PPP termination policy.
  ---------------------------------------------------------------------*/
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    PS_IFACE_SET_GRACEFUL_CLOSE(this_iface_ptr, FALSE);
  }
  else
  {
    PS_IFACE_SET_GRACEFUL_CLOSE(this_iface_ptr, TRUE);
  }
  
  for (curr_iface_index = 0;
       curr_iface_index < DS707_MAX_IFACE_PER_PDN_CONTEXT;
       curr_iface_index++)
  {
    if (pdncntx_cb_ptr->ds707_iface_tbl[curr_iface_index] != NULL)
    {
      temp_iface_ptr = &(pdncntx_cb_ptr->ds707_iface_tbl[curr_iface_index]->ps_iface);
      if ( (temp_iface_ptr != this_iface_ptr) &&
           ( (ps_iface_state(temp_iface_ptr) != IFACE_DOWN) &&
             (ps_iface_state(temp_iface_ptr) != IFACE_STATE_INVALID) ) )
      {
        /*----------------------------------------------------------------
          If there is any other iface associated with this PDN which is
          not in down state, then this_iface is not the last iface
          going down for the PDN. This condition is for the scenario
          when there is a failure in bring up, even before bringup on
          the second iface starts. In this case, iface down event will be
          received only for one iface, but still both ifaces need to be
          cleaned and the pdn needs to be freed.
        ----------------------------------------------------------------*/
        break;
      }
    }
  }
  if (curr_iface_index == DS707_MAX_IFACE_PER_PDN_CONTEXT)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "ds707_pdncntx_dun_cleanup: "
                     "All iface tbl entries for this PDN are NULL" );
    last_iface_down = TRUE;
  }
  
  if (last_iface_down == TRUE)
  {
    /*--------------------------------------------------------------------
      Set the current id to INVALID.
    --------------------------------------------------------------------*/
    ds707_data_session_set_iface_current_profile(
      this_iface_ptr,
      DATA_SESSION_PROFILE_ID_INVALID,
      ACTIVE_SUBSCRIPTION_1 );
  
    /*-------------------------------------------------------------------
      Set the iface linger status to FALSE
    -------------------------------------------------------------------*/  
    ds707_data_session_set_iface_linger_status(this_iface_ptr, FALSE);
  }
  
  /*-----------------------------------------------------------------------
    Check if the call that went down was a lingering call or an active or
    coming up call. 
    If the call was a lingering call then do not clear the requesting id 
    because it will be needed for the next call bring up. Also do not
    disassociate iface table from pdn.
    If the call was an active call then clear the requesting id.
  -----------------------------------------------------------------------*/
  if (ds707_data_session_get_iface_linger_status
        (this_iface_ptr, &linger_status) < 0)
  {
    DATA_ERR_FATAL("Cannot update the requesting profile id");
    return;
  }

  if (linger_status == FALSE)
  {
    /*--------------------------------------------------------------------
      For this PDN, set the iface_tbl_entry for this pdn as NULL. Also
      set the pdn_cb_ptr for this iface_tbl_entry as NULL.
    ---------------------------------------------------------------------*/
    if( ps_iface_addr_family_is_v4(this_iface_ptr) )
    {
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "ds707_pdncntx_dun_cleanup: "
                       "Setting PDN 0x%x v4 iface tbl entry to NULL",
                       pdncntx_cb_ptr );
      ds707_pdncntx_set_v4_iface(pdncntx_cb_ptr, NULL);        
    }

    /*--------------------------------------------------------------------
     To make sure v6 iface is free'd correctly during some DUN bringup
     error scenarios
    ---------------------------------------------------------------------*/
#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_EHRPD_DUAL_IP)
    v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

    if( (!PS_IFACE_IS_IN_USE(v6_iface_ptr)) &&
        (ps_iface_state(v6_iface_ptr) == IFACE_DOWN) )
    {
      /* Free V6 iface */
      DATA_IS707_MSG2(MSG_LEGACY_HIGH, "DUN call, free V6 iface 0x%x::%d", 
                      v6_iface_ptr->name, v6_iface_ptr->instance);
      ds707_pdncntx_free_v6_iface(pdncntx_cb_ptr);
    }
#endif /* FEATURE_EHRPD && FEATURE_DATA_EHRPD_DUAL_IP */

    /*--------------------------------------------------------------------
     if all ifaces are down free the pdn and set req id to invalid
    ---------------------------------------------------------------------*/
    if (last_iface_down == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "ds707_pdncntx_dun_cleanup:last IFACE for this pdn down, "
                       "clearing the requesting profile id and free this PDN");

      ds707_data_session_set_iface_requesting_profile(
        this_iface_ptr, 
        DATA_SESSION_PROFILE_ID_INVALID,
        ACTIVE_SUBSCRIPTION_1);

      ds707_pdncntx_set_ppp_resync_lock(pdncntx_cb_ptr, FALSE);

#ifdef FEATURE_CALL_THROTTLE
      ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, FALSE);
#endif /* FEATURE_CALL_THROTTLE */
      ds707_pdncntx_free_pdncntx_cb(pdncntx_cb_ptr);
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "all IFACEs for this pdn not down, do not free this PDN");
    }
    iface_tbl_ptr->pdn_cntx_ptr = NULL;
    PS_IFACE_SET_IS_ACTIVE(this_iface_ptr, FALSE);
  }

  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Lingering IFACE down, maintaining requesting profile id");
  }

  /*-----------------------------------------------------------------------
    Set the oldest active IFACE pointer
  -----------------------------------------------------------------------*/
  ds707_pdncntx_set_oldest_active_iface(this_iface_ptr);

} /* ds707_pdncntx_dun_cleanup() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ACTIVE_IFACE

DESCRIPTION   Returns an active iface
    
PARAMETERS    None.

RETURN VALUE  ps_iface_type *

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type *ds707_pdncntx_get_active_iface
(
  void
)
{
  ps_iface_type *active_iface_ptr = NULL;
  ps_iface_type *curr_iface_ptr = NULL;
  int curr_iface_index;

  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index ++ )
  {
    curr_iface_ptr =  
        ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    
    if ( PS_IFACE_IS_VALID(curr_iface_ptr ) &&
         DS707_PDNCNTXT_IFACE_STATE_IS_NOT_DOWN(ps_iface_state(curr_iface_ptr )))
    {     
      active_iface_ptr = curr_iface_ptr; 
      break;
    } /* if the current iface is up */
  } /* for each iface */
  
  return active_iface_ptr;
} /* ds707_pdncntx_get_active_iface */
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ACTIVE_PDN_COUNT

DESCRIPTION   Returns current active PDN count
    
PARAMETERS    None.

RETURN VALUE  uint8: active pdn count

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds707_pdncntx_get_active_pdn_count(void)
{
  uint8                      i = 0;
  uint8                      active_pdn_count = 0;
  ds707_pdn_context_cb_type  *pdncntx_cb_ptr = NULL;

  for (i=0; i<DS707_MAX_PDN_INSTANCES; i++)
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(i);
    if(NULL != ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr))
    {
      active_pdn_count++;
      DATA_IS707_MSG2( MSG_LEGACY_MED, "ds707_pdncntx_get_active_pdn_count:"
                       "pdn_id %d is active, accumulated active count %d", 
                       pdncntx_cb_ptr->data_profile.pdn_id, active_pdn_count);
    }
  }
  return active_pdn_count;
} /* ds707_pdncntx_get_active_pdn_count */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IS_LAST_ACTIVE_PDN

DESCRIPTION   Returns if the input pdn is the last active pdn
    
PARAMETERS    None.

RETURN VALUE  TRUE: input pdn is the last PDN 
              FALSE: input pdn is not the last PDN 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_is_last_active_pdn(
                        ds707_pdn_context_cb_type *curr_pdn_cntx_ptr
                        )
{
  uint8                      i = 0;
  boolean                    is_the_last_active_pdn = TRUE;
  ds707_pdn_context_cb_type  *pdncntx_cb_ptr = NULL;
  ps_iface_type              *v4_iface_ptr = NULL;
  ps_iface_type              *v6_iface_ptr = NULL;

  if (NULL == curr_pdn_cntx_ptr)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "ds707_pdncntx_is_last_active_pdn:"
          "Input pdn contex null!");
    return FALSE;
  }
  for (i=0; i<DS707_MAX_PDN_INSTANCES; i++)
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(i);
    if (pdncntx_cb_ptr != curr_pdn_cntx_ptr)
    {
      v4_iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
      if (NULL != v4_iface_ptr)
      {
        if (IFACE_UP == ps_iface_state(v4_iface_ptr))
        {
          DATA_IS707_MSG1( MSG_LEGACY_MED, "ds707_pdncntx_is_last_active_pdn:"
                "pdn instance %d is still active for v4 iface", i);
          is_the_last_active_pdn = FALSE;
          break;
        }
      }
      v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
      if (NULL != v6_iface_ptr)
      {
        if (IFACE_UP == ps_iface_state(v6_iface_ptr))
        {
          DATA_IS707_MSG1( MSG_LEGACY_MED, "ds707_pdncntx_is_last_active_pdn:"
                "pdn instance %d is still active for v6 iface", i);
          is_the_last_active_pdn = FALSE;
          break;
        }
      }      
    }
  }
  DATA_IS707_MSG1( 
                   MSG_LEGACY_MED, 
                   "ds707_pdncntx_is_last_active_pdn:%d", 
                   is_the_last_active_pdn
                   );
  return is_the_last_active_pdn;
}
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SYS_IOCTL_EHRPD_PDN_THROTTLE_INFO

DESCRIPTION   This function will be called by sys ioctl to get PDN throttle 
              info
    
PARAMETERS    argval_ptr: this pointer is pointing to the caller for getting
                          PDN throttle info

RETURN VALUE   0 on success
              -1 on Failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds707_pdncntx_sys_ioctl_ehrpd_pdn_throttle_info
(
  void            *argval_ptr,
  int16           *ps_errno
)
{
  uint8 index = 0;
  ds707_pdn_throttle_info_type*         throttle_info_ptr = NULL;
  ps_sys_ioctl_pdn_throttle_info_type* pdn_throttle_info_ptr = NULL;
  ps_sys_ioctl_pdn_ctxt_throttle_info_type* pdn_ctxt_throttle_info_ptr = NULL;
  uint32 throttled_remaining = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (ps_errno == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno ptr, fail ioctl");
    return -1;
  }

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_pdncntx_sys_ioctl_ehrpd_pdn_throttle_info(): "
                       "argval_ptr passed in is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

    /* Allocate memory for PDN throttle info*/
  pdn_throttle_info_ptr = 
    (ps_sys_ioctl_pdn_throttle_info_type*)
    modem_mem_alloc((sizeof(ps_sys_ioctl_pdn_throttle_info_type)),
        MODEM_MEM_CLIENT_DATA_CRIT);

  if ( pdn_throttle_info_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Heap Memory Allocation failed for PDN throttle info");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /* Initialize throttle inf */
  memset( (void*)pdn_throttle_info_ptr, 
          0x0, 
          sizeof(ps_sys_ioctl_pdn_throttle_info_type));

  for (index = 0; index < DS707_PDNCNTX_MAX_EHRPD_APN; index++)
  {
    throttle_info_ptr = ds707_get_throttle_info_type_from_instance(index);
     
    /*  We pass throttle info only if the is throttled because of failure*/
    if (( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) ) &&
        (((rex_get_timer(&(throttle_info_ptr->ipv4_throttle_info.throttle_timer)) != 0) && 
          (throttle_info_ptr->ipv4_throttle_info.throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE) ) 
         ||
         ((rex_get_timer(&(throttle_info_ptr->ipv6_throttle_info.throttle_timer)) != 0)&& 
          (throttle_info_ptr->ipv6_throttle_info.throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE) )))
    {
      pdn_ctxt_throttle_info_ptr = &(pdn_throttle_info_ptr->pdn_throttle_info
                       [pdn_throttle_info_ptr->num_apn]);
      /* fill in apn string len */
      pdn_ctxt_throttle_info_ptr->apn_string_len = 
              throttle_info_ptr->apn_string_len;
      /* copy apn string - without NULL termination, 
         hence dst_size=PS_SYS_MAX_APN_LEN-1 */
      memscpy( (void *)(pdn_ctxt_throttle_info_ptr->apn_string), 
               PS_SYS_MAX_APN_LEN - 1,
               (void *)(throttle_info_ptr->apn_string), 
               throttle_info_ptr->apn_string_len );

      /*  Fill in IPV4 throttle info only if IPV4 throttle is due to failure*/
      throttled_remaining = rex_get_timer(&(throttle_info_ptr->ipv4_throttle_info.throttle_timer));
      if ( (throttled_remaining != 0)&& 
          (throttle_info_ptr->ipv4_throttle_info.throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE) )
      {
        pdn_ctxt_throttle_info_ptr->is_ipv4_throttled = TRUE;
        pdn_ctxt_throttle_info_ptr->remaining_ipv4_throttled_time = throttled_remaining;
      }
      /*Fill in IPV6 throttle info only if IPV6 throttle is due to failure*/
      throttled_remaining = rex_get_timer(&(throttle_info_ptr->ipv6_throttle_info.throttle_timer));
      if ((throttled_remaining != 0) && 
          (throttle_info_ptr->ipv6_throttle_info.throttle_type == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE) )
      {
        pdn_ctxt_throttle_info_ptr->is_ipv6_throttled = TRUE;
        pdn_ctxt_throttle_info_ptr->remaining_ipv6_throttled_time = throttled_remaining;
      }

      DATA_IS707_MSG_SPRINTF_2( MSG_LEGACY_HIGH, 
                                "Sys PDN Throttled APN:'%s', len = %d ",
                                pdn_ctxt_throttle_info_ptr->apn_string,
                                pdn_ctxt_throttle_info_ptr->apn_string_len);

      DATA_IS707_MSG5( MSG_LEGACY_HIGH, 
                       "Sys PDN Throttled IPV4 - %d, IPV6 - %d, index[%d]. "
                       "IPV4 remaining - %d, IPV6 Remaining - %d",
                       pdn_ctxt_throttle_info_ptr->is_ipv4_throttled,
                       pdn_ctxt_throttle_info_ptr->is_ipv6_throttled,
                       pdn_throttle_info_ptr->num_apn,
                       pdn_ctxt_throttle_info_ptr->remaining_ipv4_throttled_time,
                       pdn_ctxt_throttle_info_ptr->remaining_ipv6_throttled_time);

      /* Increase valid elements */
      pdn_throttle_info_ptr->num_apn++;
    }
  } /* for */
  /* copy pdn throttle info */
  memscpy( (void *)(argval_ptr), 
           sizeof(ps_sys_ioctl_pdn_throttle_info_type),
           (void *)(pdn_throttle_info_ptr), 
           sizeof(ps_sys_ioctl_pdn_throttle_info_type) );

  /* Free the memory allocated to PDN throttle info */
  modem_mem_free( pdn_throttle_info_ptr,MODEM_MEM_CLIENT_DATA_CRIT);

  return 0;

} /* ds707_pdncntx_sys_ioctl_ehrpd_pdn_throttle_info */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SYS_PDN_THROTTLE_INFO_NOTIFICATION

DESCRIPTION   Generate system PDN throttle events whenever PDN throttling
              and unthrottling happens
    
PARAMETERS    None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_sys_pdn_throttle_info_notification
(
  void 
)
{
  uint8 index = 0;
  ds707_pdn_throttle_info_type*         throttle_info_ptr = NULL;
  ps_sys_pdn_throttle_info_type* ehrpd_pdn_ctxt_throttle_ev_info_ptr = NULL;
  ps_sys_pdn_ctxt_throttle_info_type* pdn_ctxt_throttle_info_ptr = NULL;
  int16  ps_error = 0;
  int16  rval = 0;
  ds3geventmgr_filter_type           eventmgr_input;
  ds3gevent_mgr_throttle_info_type  ds3g_throttle_info;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Allocate memory for PDN throttle info*/
  ehrpd_pdn_ctxt_throttle_ev_info_ptr = 
    (ps_sys_pdn_throttle_info_type*)
    modem_mem_alloc((sizeof(ps_sys_pdn_throttle_info_type)),
        MODEM_MEM_CLIENT_DATA_CRIT);

 if ( ehrpd_pdn_ctxt_throttle_ev_info_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Heap Memory Allocation failed for PDN throttle info");
    return;
  }

  /* Initialize throttle inf */
  memset( (void*)ehrpd_pdn_ctxt_throttle_ev_info_ptr, 
          0x0, 
          sizeof(ps_sys_pdn_throttle_info_type));

  for (index = 0; index < DS707_PDNCNTX_MAX_EHRPD_APN; index++)
  {
    throttle_info_ptr = ds707_get_throttle_info_type_from_instance(index);
    if (( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) ) &&
        ((rex_get_timer(&(throttle_info_ptr->ipv4_throttle_info.throttle_timer)) != 0)||
         (rex_get_timer(&(throttle_info_ptr->ipv6_throttle_info.throttle_timer)) != 0)||
         (DS707_PDNCNTX_THROTTLE_TYPE_INFINITE ==throttle_info_ptr->ipv4_throttle_info.throttle_type) ||
         (DS707_PDNCNTX_THROTTLE_TYPE_INFINITE == throttle_info_ptr->ipv6_throttle_info.throttle_type)))
    {
      pdn_ctxt_throttle_info_ptr = &(ehrpd_pdn_ctxt_throttle_ev_info_ptr->pdn_throttle_info
                       [ehrpd_pdn_ctxt_throttle_ev_info_ptr->num_apn]);
      /* fill in apn string len */
      pdn_ctxt_throttle_info_ptr->apn_string_len = 
              throttle_info_ptr->apn_string_len;
      /* copy apn string - without NULL termination, 
         hence dst_size=PS_SYS_MAX_APN_LEN-1 */
      memscpy( (void *)(pdn_ctxt_throttle_info_ptr->apn_string), 
               PS_SYS_MAX_APN_LEN - 1,
               (void *)(throttle_info_ptr->apn_string), 
               throttle_info_ptr->apn_string_len );
      
      if (rex_get_timer(&(throttle_info_ptr->ipv4_throttle_info.throttle_timer)) != 0 ||
          DS707_PDNCNTX_THROTTLE_TYPE_INFINITE ==throttle_info_ptr->ipv4_throttle_info.throttle_type)
      {
        pdn_ctxt_throttle_info_ptr->is_ipv4_throttled = TRUE;
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      }

      if (rex_get_timer(&(throttle_info_ptr->ipv6_throttle_info.throttle_timer)) != 0 ||
          DS707_PDNCNTX_THROTTLE_TYPE_INFINITE ==throttle_info_ptr->ipv6_throttle_info.throttle_type)
      {
        pdn_ctxt_throttle_info_ptr->is_ipv6_throttled = TRUE;
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }

      DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                       "Sys PDN Throttled IPV4 - %d, IPV6 - %d, index[%d] ",
                       pdn_ctxt_throttle_info_ptr->is_ipv4_throttled,
                       pdn_ctxt_throttle_info_ptr->is_ipv6_throttled,
                       ehrpd_pdn_ctxt_throttle_ev_info_ptr->num_apn);

      DATA_IS707_MSG_SPRINTF_2( MSG_LEGACY_HIGH, 
                                "Sys PDN Throttled APN:'%s', len = %d ",
                                pdn_ctxt_throttle_info_ptr->apn_string,
                                pdn_ctxt_throttle_info_ptr->apn_string_len);

      ehrpd_pdn_ctxt_throttle_ev_info_ptr->num_apn++;

    }
  } /* for */

  /* Generate system PDN thorottle events */
  rval = ps_sys_event_ind( PS_SYS_TECH_3GPP2,
                           PS_SYS_EVENT_PDN_THROTTLE_INFO,
                           (void *)ehrpd_pdn_ctxt_throttle_ev_info_ptr,
                           &ps_error);
  if (( rval != DSS_SUCCESS ) || ps_error )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                    "Failed to generate System PDN throttle event %d with ps_error: %d", 
                    rval,
                    ps_error);
  }

  memset( (void*)&eventmgr_input, 
          0, 
          sizeof(ds3geventmgr_filter_type ));

  
  memset( (void*)&ds3g_throttle_info, 
          0, 
          sizeof(ds3gevent_mgr_throttle_info_type));
  
  memscpy((void*)&ds3g_throttle_info.pdn_throttle_info, 
              sizeof(ps_sys_pdn_throttle_info_type),
              (void*)(ehrpd_pdn_ctxt_throttle_ev_info_ptr),
              sizeof(ps_sys_pdn_throttle_info_type));

  eventmgr_input.tech = PS_SYS_TECH_3GPP2;
  eventmgr_input.ps_subs_id = PS_SYS_PRIMARY_SUBS;
  if( TRUE != ds3geventmgr_set_event_info(DS3GEVENTMGR_PDN_THROTTLE_INFO_EV,
                              (void *)&ds3g_throttle_info,
                              &eventmgr_input))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Failed to generate DS3GMGR PDN throttle event");  
  }

  /* Free the memory allocated to PDN throttle info */
  modem_mem_free( ehrpd_pdn_ctxt_throttle_ev_info_ptr, MODEM_MEM_CLIENT_DATA_CRIT );

} /* ds707_pdncntx_sys_pdn_throttle_info_notification */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CLEAR_PDN_THROTTLED_IN_TUNNEL_MODE

DESCRIPTION   Function to clear PDNs that were throttled due to non-explicit 
              Network Reject in Tunnel Mode
              The Following Fail reasons will be cleared from the throttle
              table
              1. PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_TIMEOUT
              2. PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE
    
PARAMETERS    None

RETURN VALUE  None 
 
SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_clear_pdn_throttled_in_tunnel_mode
(
  void
)
{
  uint8 index = 0;
  ds707_pdn_throttle_info_type*         throttle_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_pdncntx_clear_pdn_throttle_info: Clear "
                   "throttling for PDNs throttled due to non-explicit "
                   "network reject in Tunnel Mode");

  for (index = 0; index < DS707_PDNCNTX_MAX_EHRPD_APN; index++)
  {
    throttle_info_ptr = ds707_get_throttle_info_type_from_instance(index);
    if( (DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)) &&
        ( (throttle_info_ptr->ipv4_throttle_info.throttled_in_tunnel_mode == TRUE) && 
          (throttle_info_ptr->ipv4_throttle_info.throttle_reason 
           == PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_TIMEOUT) ) )
    {
      /* Note: Sometimes, when a PDN failure happens for the first time,
         we might just increment the throttle count but won't start the
         throttle timer. So don't check if the throttling timer is running
         here 
      */
        ds707_pdncntx_clear_throttle_count(throttle_info_ptr, 
                                           DS707_PDNCNTX_IP_TYPE_V4); 
    }
        
    throttle_info_ptr = ds707_get_throttle_info_type_from_instance(index);
    if( (DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)) &&
        ( (throttle_info_ptr->ipv6_throttle_info.throttled_in_tunnel_mode == TRUE) &&
          ( (throttle_info_ptr->ipv6_throttle_info.throttle_reason 
             == PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_TIMEOUT) || 
          (throttle_info_ptr->ipv6_throttle_info.throttle_reason 
             == PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE) ) ) )
      {
        ds707_pdncntx_clear_throttle_count(throttle_info_ptr, 
                                           DS707_PDNCNTX_IP_TYPE_V6); 
      }
    }
} /* ds707_pdncntx_clear_pdn_throttle_info */

/*===========================================================================
FUNCTION      DS707_GET_PDNCNTX_CB_FROM_REQ_PROFILE_APN

DESCRIPTION   Given the requesting profile, returns the pdn cb tbl pointer 
              associated to it 

PARAMETERS    ds707_data_session_profile_id : 
                    Requesting data session profile id.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_pdn_context_cb_type* ds707_get_pdncntx_cb_from_req_profile_apn(
  ds707_data_session_profile_info_type *data_sess_profile_ptr
)
{
  ds707_data_session_profile_info_type *requesting_profile_ptr = NULL;
  uint8 i = 0;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_sess_profile_ptr == NULL)
  {
    return NULL;
  }

  for(i = 0; i < DS707_MAX_PDN_INSTANCES; i++)
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(i);
  
    if (ds707_pdncntx_get_requesting_profile(pdncntx_cb_ptr,
                                             &requesting_profile_ptr))
    {
      if( data_sess_profile_ptr->apn_string_len == 
          requesting_profile_ptr->apn_string_len )
      {
        if( 0  == strncmp(data_sess_profile_ptr->apn_string,
                          requesting_profile_ptr->apn_string,
                          data_sess_profile_ptr->apn_string_len) )
        {
          break;
        }
      }
    }
  }

  if(i == DS707_MAX_PDN_INSTANCES)
  {
    return NULL;
  }
  else
  {
    return pdncntx_cb_ptr;
  }
} /* ds707_get_pdncntx_cb_from_req_profile_id */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_RESET_CALL_BRINGUP_IN_TUNNEL_MODE

DESCRIPTION   For a given PDN context cb ptr resets the 
              bringup_in_tunnel_mode flag to FALSE

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_reset_call_bringup_in_tunnel_mode
(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_ERR_FATAL("PDN context cb ptr is invalid");
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "Reset call_bringup_in_tunnel_mode flag in PDN CB");

  pdncntx_cb_ptr->call_bringup_in_tunnel_mode = FALSE;

} /* ds707_pdncntx_set_call_bringup_in_tunnel_mode */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_CALL_BRINGUP_MODE

DESCRIPTION   For a given PDN context cb ptr set the bringup flag to 
              indicate call bring up in tunnel mode depending on the S101
              state

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_call_bringup_mode
(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
)
{
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_ERR_FATAL("PDN context cb ptr is invalid");
  }

#ifdef FEATURE_DATA_OPTHO
  if (ds707_s101_query_sm_state() != DS707_S101_SM_NON_TUNNEL)
  {
    pdncntx_cb_ptr->call_bringup_in_tunnel_mode =  TRUE;
  }
  else
#endif /* FEATURE_DATA_OPTHO */
  {
    pdncntx_cb_ptr->call_bringup_in_tunnel_mode = FALSE;
  }
  
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "PDN Call bringup in eHRPD, Tunnel Mode: %d",
                   pdncntx_cb_ptr->call_bringup_in_tunnel_mode );
  
} /* ds707_pdncntx_set_call_bringup_mode */

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_RECONFIG

DESCRIPTION   Function to reconfigure (resync) NCP connection
    
PARAMETERS    iface_ptr: iface to be reconfigured
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_iface_reconfig
(
  ps_iface_type            *iface_ptr
)
{
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL; 
  ppp_ncp_handle_type        ncp_handle      = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(iface_ptr)) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "ds707_pdncntx_iface_reconfig:"
                                       "Invalid IFACE ptr");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get the NCP handle stored in the PDN CB for this IFACE. Call PPP / NCP
    resync on this NCP handle.
  -------------------------------------------------------------------------*/
  pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                     iface_ptr);
  if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "Cannot get PDN CB for IFACE 0x%x:%d, fail ioctl",
                     iface_ptr->name, iface_ptr->instance);
    return -1; 
  }

  ncp_handle = ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr);
  if (ncp_handle == NULL)
  {
    DATA_IS707_MSG3( MSG_LEGACY_ERROR,
                     "NULL NCP handle for IFACE 0x%x:%d, PDN CB 0x%x, "
                     "fail ioctl",
                     iface_ptr->name,iface_ptr->instance,pdncntx_cb_ptr);
    return -1;
  }

  if (ds707_pdncntx_get_ncp_started(pdncntx_cb_ptr) == FALSE)
  {
    DATA_IS707_MSG3( MSG_LEGACY_ERROR,
                     "NCP not started for IFACE 0x%x:%d, PDN CB 0x%x, "
                     "fail ioctl",
                     iface_ptr->name,iface_ptr->instance,pdncntx_cb_ptr);
    return -1;
  }

  if (ds707_pdncntx_get_ncp_resync_started(pdncntx_cb_ptr) == TRUE)
  {
    DATA_IS707_MSG3( MSG_LEGACY_ERROR,
                     "NCP resync already in progress for "
                     "IFACE 0x%x:%d, PDN CB 0x%x, "
                     "passing ioctl without triggering a new ppp_ncp_resync",
                     iface_ptr->name,iface_ptr->instance,pdncntx_cb_ptr);
    return 0;
  }

  /*------------------------------------------------------------------------- 
     Notify sys_chg_sm before sending resync. If Sys chg SM is in
     EPC_TO_NONEPC state it'll transition to NULL state. This should be done
     before issuing ncp_resync
  -------------------------------------------------------------------------*/
  ds707_sys_chg_iface_reconfig_ind();

  if (ppp_ncp_resync(ncp_handle) < 0)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "Can't resync NCP, fail ioctl");
    return -1;
  }

  /* Set the NCP resync started flag here so if EPC calls the same ioctl again
     for the companion iface, we don't end up resyncing again.
     The flag will get unset when either the iface comes up/goes down due to
     resync */
  ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, TRUE);

  return 0;
} /* ds707_pdncntx_iface_reconfig */
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_V6_REM_RTR_LIFETIME

DESCRIPTION   Function to retrieve remaining IPv6 router lifetime.
    
PARAMETERS    iface_ptr: iface associated IPv6 PDN 
              argnal_ptr: void pointer to return value
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_get_v6_rem_rtr_lifetime
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr
)
{
  ds707_iface_table_type*                  v6_iface_tbl_ptr = NULL;
  ip6_sm_type*                             ip6_sm_ptr = NULL;
  ps_iface_ioctl_v6_router_lifetime_type*  ipv6_rem_rtr_lifetime_ptr;
  int32                                    ipv6_rtr_lifetime;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Get remaining IPv6 router lifetime from iface->dssnet6->ip6_sm.
  -------------------------------------------------------------------------*/
  ipv6_rem_rtr_lifetime_ptr = 
                         (ps_iface_ioctl_v6_router_lifetime_type *)argval_ptr;

  v6_iface_tbl_ptr = 
                ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);
  if (NULL == v6_iface_tbl_ptr)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Invalid v6_iface_tbl_ptr, fail ioctl");
    return -1;
  }

  ip6_sm_ptr = ds707_pdncntx_get_ip6_sm_ptr(v6_iface_tbl_ptr);
  if (NULL == ip6_sm_ptr)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Invalid ip6_sm_ptr, fail ioctl");
    return -1;
  }

  if ((ipv6_rtr_lifetime = ip6_sm_get_remaining_router_lifetime(ip6_sm_ptr)) < 0)
  {
    *ipv6_rem_rtr_lifetime_ptr = 0;
    return -1;
  }

  *ipv6_rem_rtr_lifetime_ptr = 
                         (ps_iface_ioctl_v6_router_lifetime_type)ipv6_rtr_lifetime;

  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "IPv6 remaining router lifetime is %d", 
                   *ipv6_rem_rtr_lifetime_ptr);    

  return 0;
}
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_ORIG_SYS

DESCRIPTION   Given a PDN context cb ptr set the origination system

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
              
RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_orig_sys
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  ds707_sys_info_enum_type   last_good_sys
)
{
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_ERR_FATAL("PDN context cb ptr is invalid");
    return;
  }

  pdncntx_cb_ptr->orig_sys = last_good_sys;
  DATA_IS707_MSG1(MSG_LEGACY_MED, "PDN CB set orig_sys = %d", 
                  pdncntx_cb_ptr->orig_sys);
} /* ds707_pdncntx_set_orig_sys() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ORIG_SYS

DESCRIPTION   Given a PDN context cb ptr get the origination system

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
ds707_sys_info_enum_type ds707_pdncntx_get_orig_sys
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{
  if (!DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
  {
    DATA_ERR_FATAL("PDN context cb ptr is invalid");
    return DS707_SYS_INFO_NO_SYSTEM;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, "PDN CB get orig_sys = %d", 
                  pdncntx_cb_ptr->orig_sys);
  return (pdncntx_cb_ptr->orig_sys);
} /* ds707_pdncntx_get_orig_sys() */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IS_CALL_ALLOWED

DESCRIPTION   Utility function to see if the current serving system is 
              compatible with the system stored in PDN CB during call orig
              
DEPENDENCIES  None

RETURN VALUE  TRUE  - if the current serving system & stored orig system 
                      PDN CB are compatible
              FALSE - otherwise

SIDE EFFECTS  None 
  
===========================================================================*/
boolean ds707_pdncntx_is_call_allowed 
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  ps_iface_net_down_reason_type *call_end_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Sanity check */
  if (NULL == call_end_reason)
  {
    DATA_ERR_FATAL("call_end_reason is NULL");
    return FALSE;
  }

  /* Check if both current serving sytem & the system stored in PDN CB is 
     compatible.
     Note: we can't use dsd to check for ehrpd becuase when the call is up
     there can be a delay when dsd does not have the correct pref sys info */
  if (((TRUE == ds707_pkt_is_ehrpd_mode_of_operation()) && 
       (DS707_SYS_INFO_EPC_SYSTEM == ds707_pdncntx_get_orig_sys(pdncntx_cb_ptr))) ||
      ((FALSE == ds707_pkt_is_ehrpd_mode_of_operation()) && 
       (DS707_SYS_INFO_NON_EPC_SYSTEM == ds707_pdncntx_get_orig_sys(pdncntx_cb_ptr))))
  {
    return TRUE;
  }

  *call_end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
  DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Call landed on incompatible RAT, don't allow");
  return FALSE;
} /* ds707_pdncntx_is_call_allowed() */

/*===========================================================================
FUNCTION      DS707_PDNCTX_RESET_THROTTLE_ON_SIM_SWAP

DESCRIPTION   Clear throttling info on hotswap
              
DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None 
  
===========================================================================*/
void ds707_pdnctx_reset_throttle_on_sim_swap()
{
   uint8 index = 0;
   ds707_pdn_throttle_info_type*         throttle_info_ptr = NULL;
    
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "ds707_pdncntx_clear_pdn_throttle_info as part of NV"
                   "refresh");
    
  for (index = 0; index < DS707_PDNCNTX_MAX_EHRPD_APN; index++)
  {
    throttle_info_ptr = ds707_get_throttle_info_type_from_instance(index);
    if( (DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) ) )
    {
       ds707_pdncntx_clear_throttle_count(throttle_info_ptr, 
                                           DS707_PDNCNTX_IP_TYPE_V4); 
       ds707_pdncntx_clear_throttle_count(throttle_info_ptr, 
                                           DS707_PDNCNTX_IP_TYPE_V6); 
    }
  }
  return;
}/* ds707_pdnctx_reset_throttle_on_sim_swap() */

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION DS707_PDNCNTX_CB_IS_DEFAULT_PDN

DESCRIPTION
  This function returns if the PDN CB is default PDN
  
PARAMETERS  
  pointer to the PDN in question
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE:  if this PDN is default PDN
  FALSE: otherwise.
==========================================================================*/
boolean ds707_pdncntx_is_default_pdn
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
)
{

  if ( (ds707_pdncntx_default_apn_info_ptr != NULL) &&
       (pdncntx_cb_ptr == 
          ds707_pdncntx_default_apn_info_ptr->default_pdn_cb_ptr) )
  {
    return TRUE;
  }

  return FALSE;
} /* ds707_pdncntx_is_default */

/*===========================================================================
FUNCTION DS707_PDNCNTX_CB_IS_DEFAULT_APN

DESCRIPTION
  This function returns if the APN in profile is default APN
  
PARAMETERS  
  pointer to the profile in question
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE:  if this APN is default APN
  FALSE: otherwise.
==========================================================================*/
boolean ds707_pdncntx_is_default_apn
(
   ds707_data_session_profile_info_type *data_sess_prof_ptr
)
{

  if ( (data_sess_prof_ptr == NULL) || 
       (ds707_pdncntx_default_apn_info_ptr == NULL) )
  {
    return FALSE;
  }

  if ( (data_sess_prof_ptr->apn_string_len == 0) &&
       (data_sess_prof_ptr->apn_string[0] == '\0') )
  {
    return TRUE;
  }

  if ( (data_sess_prof_ptr->apn_string_len == 
          ds707_pdncntx_default_apn_info_ptr->default_apn_len) &&
       (0 == strncasecmp(data_sess_prof_ptr->apn_string,
                         ds707_pdncntx_default_apn_info_ptr->default_apn,
                         data_sess_prof_ptr->apn_string_len)) )
  {
    return TRUE;
  }

  return FALSE;
} /* ds707_pdncntx_is_default */

/*===========================================================================
FUNCTION DS707_PDNCNTX_SET_DEFAULT_PDN_AND_APN

DESCRIPTION
  This function stores the default PDN and APN inicated by HSGW
  
PARAMETERS  
  pointer to the APN
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_pdncntx_set_default_pdn_and_apn
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  char                      *apn,
  uint8                      apn_len
)
{
  ds707_data_session_profile_info_type *pdn_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( pdncntx_cb_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid pdn_cntx_ptr");
    return;
  }

  /*------------------------------------------------------------------------- 
    Allocate memory for default PDN info if it hasn't been allocated yet
  -------------------------------------------------------------------------*/
  ds707_pdncntx_default_apn_info_ptr = 
    (ds707_pdncntx_default_apn_info_type*)modem_mem_alloc( 
                                 sizeof(ds707_pdncntx_default_apn_info_type),
                                 MODEM_MEM_CLIENT_DATA );

  if (ds707_pdncntx_default_apn_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Memory allocation failed for default APN info structure");
    return;
  }

  memset((void*)ds707_pdncntx_default_apn_info_ptr, 
         0, sizeof(ds707_pdncntx_default_apn_info_type));

  /*------------------------------------------------------------------------- 
    Decode and store default APN
  -------------------------------------------------------------------------*/
  ds707_pdncntx_default_apn_info_ptr->default_apn_len = 
    ds707_pdncntx_decode_apn(apn, apn_len, 
                             ds707_pdncntx_default_apn_info_ptr->default_apn);

  if ( ds707_pdncntx_default_apn_info_ptr->default_apn_len == 0 )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid decoded APN");

    modem_mem_free((void*)ds707_pdncntx_default_apn_info_ptr, 
                   MODEM_MEM_CLIENT_DATA);
    ds707_pdncntx_default_apn_info_ptr = NULL;

    return;
  }

  /*------------------------------------------------------------------------- 
    Store the default APN in PDN as well to prevent mismatch of
  -------------------------------------------------------------------------*/
  if( TRUE == ds707_pdncntx_get_requesting_profile(pdncntx_cb_ptr, 
                                                   &pdn_profile_ptr) )
  {
    pdn_profile_ptr->apn_string_len = 
      ds707_pdncntx_default_apn_info_ptr->default_apn_len;

    memscpy(pdn_profile_ptr->apn_string,
            DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
            ds707_pdncntx_default_apn_info_ptr->default_apn,
            ds707_pdncntx_default_apn_info_ptr->default_apn_len);
  }

  /*------------------------------------------------------------------------- 
    Store default PDN
  -------------------------------------------------------------------------*/
  ds707_pdncntx_default_apn_info_ptr->default_pdn_cb_ptr = pdncntx_cb_ptr;

  DATA_IS707_MSG_SPRINTF_3(MSG_LEGACY_HIGH,
                           "Stored default APN as %s, len %d "
                           "on default PDN CB 0x%x",
                           ds707_pdncntx_default_apn_info_ptr->default_apn, 
                           ds707_pdncntx_default_apn_info_ptr->default_apn_len, 
                           pdncntx_cb_ptr);

  ds707_pdncntx_report_default_apn_change();

  return;
} /* ds707_pdncntx_set_default_pdn_and_apn */

/*===========================================================================
FUNCTION DS707_PDNCNTX_RESET_DEFAULT_PDN

DESCRIPTION
  This function clears the default PDN context in UE
  
PARAMETERS  
  None.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_pdncntx_reset_default_pdn
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Clear default APN and PDN info");

  modem_mem_free((void*)ds707_pdncntx_default_apn_info_ptr, 
                 MODEM_MEM_CLIENT_DATA);
  ds707_pdncntx_default_apn_info_ptr = NULL;

  ds707_pdncntx_report_default_apn_change();

  return;
} /* ds707_pdncntx_reset_default_pdn */
#endif /* FEATURE_EHRPD */