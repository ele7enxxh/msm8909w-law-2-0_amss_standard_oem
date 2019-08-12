#ifndef DS_3GPP_PDN_CONTEXT_H
#define DS_3GPP_PDN_CONTEXT_H
/*===========================================================================

            E P S   S P E C I F I C   M O D E   H A N D L E R

===========================================================================*/
/*!
  @file
  ds_3gpp_pdn_context.h

  @brief
  This header file defines the externalized function that interface DSEPS 
  and ESM.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_pdn_context.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/10/12   ss      Added support for IPV4 link MTU IE for Rel10.
08/08/12   nd      Release 10 or above ignore static IP address filling.
02/22/12   rr      Generate IFACE_EXTENDED_IP_CONFIG_EV after receiving PCSCF address update
12/08/11   rr      Added support to update p-cscf address from NW bearer modification 
                   indiacation
9/09/11    rr      Added support for fallback from Dual IP to single IP when nw rejects 
                   with specific cause codes in UMTS mode
08/09/11   bd      Added handler for ioctl to retrieve pdp context id
05/04/11   ttv     Added changes needed for SESSION_PARAMS IOCTL and NSAPI api.
03/17/11   sa      Added support for bearer tech change event indication.
02/22/11   ttv     Merged the changes of adding wrapper function to get 
                   cumulative iface stats.
10/1/10    ars     Enabled processing auth parameters from info   
05/17/10   hs      Added APN disable and PDN inactivity feature support.
05/12/10   sa      Added support for retrieving dynamic pdn state info.
04/02/10   vrk     UMTS interRAT support
02/08/10   sa      Added function ds_pdn_cntxt_get_pdn_context_pdp_addr.
01/23/10   vk      Added helper macros for virtual function dispatch
12/12/09   sa      Added Function ds_pdn_cntxt_get_pdn_context_state.
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ds_3gpp_hdlr.h"
#include "ps_svc.h"
#include "ps_iface.h"
#include "ps_aclrules.h"
#include "ps_route.h"
#include "dsumtspdpreg.h"
#include "ps_ppp_defs.h"
#include "dsumtspspco.h"
#include "sys.h"
#include "ps_ip_netconfig.h"
#include "sio.h"
#include "lte.h"
#include "sys_cnst.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "dsgen_iface6_hdlr.h"
#include "dstask_v.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ps_iface_ioctl.h"
#include "ps_flow_ioctl.h"
#include "ps_phys_link_ioctl.h"
#include "ps_iface_ipfltr.h"
#include "ps_phys_link.h"
#include "dsumts_rmsm.h"
#include "dhcp_client.h"

#include "ps_iface_logging.h"
#include "ps_logging_defs.h"

#ifdef FEATURE_STA_UMTSHDLR
#include "sta_umtshdlr.h"
#include "ps_iface_rx_qos_fltr.h"
#endif /* FEATURE_STA_UMTSHDLR */

#ifdef FEATURE_DATA_LTE
#include "lte_nas_esm_message_description.h"
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_DATA_LTE */
#include "ds_3gpp_rmsm_ati.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "dhcp6_client.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ps_qos_defs.h"
#include "ps_sys_conf.h"
#include "ds_3gppi_utils.h"

#include "cm.h"
#include "ds3gmgr.h"
#include "ds3geventmgr.h"
#include "ds_3gpp_epc_if.h"
#include "ds_sys.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_PDN_CTXT_VF_FPN(func) DS_3GPPI_VF_FPN(func)

#define DS_PDN_CTXT_VF_REG(mode, func, fp) \
          DS_3GPPI_VF_REG(ds_pdn_context_ftbl, mode, func, fp)

#define DS_PDN_CTXT_VOBJ_REG(ds_pdn_ctxt_p, mode, objp) \
          DS_3GPPI_VOBJ_REG((ds_pdn_ctxt_p)->ds_pdn_context_dyn_p->\
          vobj_data_ptr_tbl, mode, (void *)objp)

#define DS_PDN_CTXT_VOBJ(ds_pdn_ctxt_p, mode) \
          DS_3GPPI_VOBJ((ds_pdn_ctxt_p)->ds_pdn_context_dyn_p->\
          vobj_data_ptr_tbl, mode)

#define DS_PDN_CTXT_VFR_CALL(rval, func, subs_id, ...)  \
            DS_3GPPI_VFR_CALL(ds_pdn_context_ftbl, rval, func, subs_id, \
                              __VA_ARGS__)

#define DS_PDN_CTXT_VF_CALL(func, subs_id, ...)  \
            DS_3GPPI_VF_CALL(ds_pdn_context_ftbl, func, subs_id, __VA_ARGS__)

#define DS_PDN_CTXT_VF_CALL_NO_ARG(func, subs_id)  \
            DS_3GPPI_VF_CALL_NO_ARG(ds_pdn_context_ftbl, func, subs_id)

#define DS_PDN_CTXT_VFR_CALL_NO_ARG(rval, func, subs_id)  \
            DS_3GPPI_VFR_CALL_NO_ARG(ds_pdn_context_ftbl, rval, func, subs_id)     

#ifdef FEATURE_DATA_PS_IPV6
/*---------------------------------------------------------------------------
  Maximum Number of IPV6 filters that can be filled in PDP context activate
  message
---------------------------------------------------------------------------*/
#define DS3GPP_IPV6_IID_OFFSET 8

/*---------------------------------------------------------------------------
  Config protocol field of Protocol options field (PDP ctx req msg)
  should be 0x80 : ref: TS24.008 sec 10.5.6.3
  MSB should be 1 (ext field) protocol field b0-b2 should be 0x0
  define this set but stream value
---------------------------------------------------------------------------*/
#define PROT_CFG_IE_OCT3_VAL  0x80

/*---------------------------------------------------------------------------
  IPV6 DNS Container Identifier field of Protocol options field (PDP ctx req
  msg)
  should be 0x0003 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV6_DNS_CONTAIN_ID 0x0003
#endif /* FEATURE_DATA_PS_IPV6 */

/*-------------------------------------------------------------------------
Cookie to ensure that dynamic memory allocated is valid
---------------------------------------------------------------------------*/
#define PDN_CNTXT_VALID_COOKIE  0x9d9d9d9d

#define UMTS_PDN_CNTXT_VALID_COOKIE  0x9e9e9e9e

/*---------------------------------------------------------------------------
  IPV4 DNS Container Identifier field of Protocol options field (PDP ctx req
  msg)
  should be 0x000D : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV4_DNS_CONTAIN_ID 0x000D

/*---------------------------------------------------------------------------
  Bearer Control Mode Container Identifier field of Protocol options
  MS Support of Network Requested Bearer Control indicator for NW to NW and
  Selected Bearer Control Mode for NW to MS
  should be 0x0005 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define BEARER_CONTROL_MODE_CONTAINER_ID 0x0005

/*---------------------------------------------------------------------------
  IPv4 Link MTU Request Container Identifier field of Protocol options
  should be 0x0010 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV4_LINK_MTU_CONTAINER_ID 0x0010

/*---------------------------------------------------------------------------
  IPv4 Link MTU container length (NW to MS)
  should be 2 : ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IPV4_LINK_MTU_CONTAINER_LEN  2

/*---------------------------------------------------------------------------
  Rel11: Spec CR_2088 Local address Support in TFT
  should be 0x0011: ref CR_2088 Doc: C1_121662
---------------------------------------------------------------------------*/
#define LOCAL_ADDRESS_SUPPORT_IN_TFT_CONTAINER_ID 0x0011

/*---------------------------------------------------------------------------
  This is the interval between successive polls to the lower layers to find
  out if there has been any data transfer in either direction

  Recommendation: The polling interval should be set such that the PDN 
  inactivity timer value is an integral multiple of the polling interval. 
  This is just to avoid the polling timer from expiring after the PDN 
  inactivity timer expires.
---------------------------------------------------------------------------*/
#define DS_3GPP_POLLING_INTERVAL 30000

/*-------------------------------------------------------------------------
Set the apn param change timer to 5 secs
When the timer expires, MH will take care of cleaning up the pdn context.
---------------------------------------------------------------------------*/
#define DS_3GPP_APN_PARAM_CHG_INTERVAL 5000

#define DS_3GPP_OP_IDENTIFIER_LABEL_COUNT 0x3


/* --------------------------------------------------------------------------- 
  Bit mask used while decoding Protocol Config Options
---------------------------------------------------------------------------*/
#define DS_3GPP_PCO_DECODE_FAILED_MASK            (0x1)
#define DS_3GPP_IPCP_PKT_DECODED_MASK             (0x2)
#define DS_3GPP_IPV4_PKT_DECODED_MASK             (0x4)
#define DS_3GPP_IPV6_PKT_DECODED_MASK             (0x8)
#define DS_3GPP_PCSCF_V4_PKT_DECODED_MASK         (0x10)
#define DS_3GPP_PCSCF_V6_PKT_DECODED_MASK         (0x20)
#define DS_3GPP_BEARER_CONTROL_MODE_DECODED_MASK  (0x40)
#define DS_3GPP_OPER_DECODED_MASK                 (0x100)
#define DS_3GPP_IPV4V6_LINK_MTU_DECODED_MASK      (0x80)
#define DS_3GPP_IPV4V6_LINK_MTU_INVALID           (0x200)

#define DS_3GPP_PCO_DECODE_FAILED(bit_vector)  (bit_vector &                \
                                                DS_3GPP_PCO_DECODE_FAILED_MASK)

#define DS_3GPP_PCSCF_V4_ADDR_DECODED(bit_vector) (bit_vector & DS_3GPP_PCSCF_V4_PKT_DECODED_MASK)

#define DS_3GPP_PCSCF_V6_ADDR_DECODED(bit_vector) (bit_vector & DS_3GPP_PCSCF_V6_PKT_DECODED_MASK)      
 
#define DS_3GPP_OPER_PCO_DECODED(bit_vector) (bit_vector & DS_3GPP_OPER_DECODED_MASK)      



#if defined(FEATURE_EPC_HANDOFF)
/*---------------------------------------------------------------------------
  Variable to check if a call is allowed with LTE MH
---------------------------------------------------------------------------*/
extern ds_3gpp_if_call_allowed_hdlr_ex_f_type 
  ds_3gpp_pdn_cntxt_call_allowed_hdlr_ex_f;

/*---------------------------------------------------------------------------
  Variable to check the attach type - Initial or handover.
---------------------------------------------------------------------------*/
extern ds_3gpp_if_call_attach_type_hdlr_ex_f_type 
  ds_3gpp_pdn_cntxt_is_handover_attach_ex_f;

#endif /* FEATURE_EPC_HANDOFF */

/* --------------------------------------------------------------------------- 
 
Utility Macros for interfacing with the APM module. 
 
------------------------------------------------------------------------------*/

#define DS_3GPP_APM_SET_V4_IPADDR(payload, value)    \
   payload.ind_field_mask |= DS_DSD_IND_IP_ADDRESS_FIELD_MASK; \
   payload.ip_address.field_mask |= DS_DSD_APM_IP_V4_FIELD_MASK; \
   payload.ip_address.v4_address = value;

#define DS_3GPP_APM_SET_INVALID_V4_IPADDR(payload,reason) \
      DS_3GPP_APM_SET_V4_IPADDR(payload,FALSE) \
      payload.ip_address.v4_reason = reason;

#define DS_3GPP_APM_SET_VALID_V4_IPADDR(payload)   DS_3GPP_APM_SET_V4_IPADDR(payload,TRUE) 

#define DS_3GPP_APM_SET_V6_IPADDR(payload, value)    \
   payload.ind_field_mask |= DS_DSD_IND_IP_ADDRESS_FIELD_MASK; \
   payload.ip_address.field_mask |= DS_DSD_APM_IP_V6_FIELD_MASK; \
   payload.ip_address.v6_address = value;

#define DS_3GPP_APM_SET_INVALID_V6_IPADDR(payload,reason) \
      DS_3GPP_APM_SET_V6_IPADDR(payload,FALSE) \
      payload.ip_address.v6_reason = reason;

#define DS_3GPP_APM_SET_VALID_V6_IPADDR(payload)   DS_3GPP_APM_SET_V6_IPADDR(payload,TRUE) 

#define DS_3GPP_APM_SET_V4_PCSCFADDR(payload, value)    \
   payload.ind_field_mask |= DS_DSD_IND_PCSCF_FIELD_MASK; \
   payload.pcscf_address.field_mask |= DS_DSD_APM_IP_V4_FIELD_MASK; \
   payload.pcscf_address.v4_address = value;

#define DS_3GPP_APM_SET_INVALID_V4_PCSCFADDR(payload) DS_3GPP_APM_SET_V4_PCSCFADDR(payload,FALSE) 
#define DS_3GPP_APM_SET_VALID_V4_PCSCFADDR(payload)   DS_3GPP_APM_SET_V4_PCSCFADDR(payload,TRUE) 

#define DS_3GPP_APM_SET_V4_DNSADDR(payload, value, subs_id)    \
   payload.ind_field_mask |= DS_DSD_IND_DNS_ADDR_FIELD_MASK; \
   payload.dns_address.field_mask |= DS_DSD_APM_IP_V4_FIELD_MASK; \
   payload.dns_address.v4_address = value; \
   payload.subs_id = subs_id;

#define DS_3GPP_APM_SET_V6_PCSCFADDR(payload, value)    \
   payload.ind_field_mask |= DS_DSD_IND_PCSCF_FIELD_MASK; \
   payload.pcscf_address.field_mask |= DS_DSD_APM_IP_V6_FIELD_MASK; \
   payload.pcscf_address.v6_address = value;

#define DS_3GPP_APM_SET_INVALID_V6_PCSCFADDR(payload) DS_3GPP_APM_SET_V6_PCSCFADDR(payload,FALSE) 
#define DS_3GPP_APM_SET_VALID_V6_PCSCFADDR(payload)   DS_3GPP_APM_SET_V6_PCSCFADDR(payload,TRUE) 

#define DS_3GPP_APM_SET_V6_DNSADDR(payload, value, subs_id)    \
   payload.ind_field_mask |= DS_DSD_IND_DNS_ADDR_FIELD_MASK; \
   payload.dns_address.field_mask |= DS_DSD_APM_IP_V6_FIELD_MASK; \
   payload.dns_address.v6_address = value; \
   payload.subs_id = subs_id;

#define DS_3GPP_APM_SET_PROFILE_ID(payload, value)    \
   payload.ind_field_mask |= DS_DSD_IND_PROFILE_ID_FIELD_MASK; \
   payload.profile_id = value;

#define DS_3GPP_APM_SET_CAUSE_CODE(payload, value)    \
   payload.ind_field_mask |= DS_DSD_IND_CAUSE_CODE_FIELD_MASK; \
   payload.cause_code = value;

#define DS_3GPP_APM_SET_SWITCH_APN_FLAG(payload, value, subs_id) \
  payload.ind_field_mask |= DS_DSD_IND_APN_SWITCH_MASK; \
  payload.switch_apn = value; \
  payload.subs_id = subs_id;

#ifdef FEATURE_EMERGENCY_PDN
#define DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn)  \
   ( pdn->ds_pdn_context_dyn_p->pdp_profile.emergency_calls_are_supported == TRUE)
#else 
  #define DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn)  \
    0
#endif /* FEATURE_EMERGENCY_PDN */

#define DS_3GPP_IS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC(pdn_p, subs_id)  \
   (pdn_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC) && \
   (pdn_p->ds_pdn_context_dyn_p->subs_id == subs_id)

#define DS_3GPP_IS_PDN_CONTEXT_STATE_UP(pdn_p)  \
        (pdn_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)

#define DS_3GPP_PDP_TYPE_IPV4_NUM    0x21
#define DS_3GPP_PDP_TYPE_IPV6_NUM    0x57
#define DS_3GPP_PDP_TYPE_IPV4V6_NUM  0x8D

/*--------------------------------------------------------------------------
  Enum to indicate whether Attach Profile is Available or Not Available.
-------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL = 0,
  DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL,
  DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN
} ds_pdn_cntxt_attach_profile_status_type;


typedef enum
{
  DS_3GPP_IFACE_NOT_IN_USE_STATE      = 0,
  DS_3GPP_IFACE_IN_USE_STATE          = 1,
  DS_3GPP_IFACE_INVALID_STATE
} ds_3gpp_iface_state_e;

/*----------------------------------------------------------------------- 
  This type is used to distinguish between various redial scenarios.
  Extend as required
--------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_REDIAL_UMTS_LTE_IRAT,
  DS_3GPP_REDIAL_DUAL_IP_FALLBACK,
  DS_3GPP_REDIAL_AUTH_FALLBACK,
  DS_3GPP_REDIAL_OP_PCO_CHG,
  DS_3GPP_NO_REDIAL
}ds_3gpp_redial_e_type;

/*---------------------------------------------------------------------------
  3GPP release version for filling Container ID:s
    1.DNS server IPv4 address request. 
    2.P-CSCF IPv4 address request 
    3.IP address allocation via NAS signaling
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_REL99 = 0,
  DS_3GPP_REL5  = 1,
  DS_3GPP_REL6  = 2,
  DS_3GPP_REL7  = 3,
  DS_3GPP_REL8  = 4,
  DS_3GPP_REL9  = 5,
  DS_3GPP_REL10 = 6, 
  DS_3GPP_REL11 = 7,
  DS_3GPP_REL_MAX = 0xFF
}ds_3gpp_release_e;

/*---------------------------------------------------------------------------
  Enum to hold SIO WM flush state 
  Used during switching the data path to A2 while guaranteeing in-order 
  delivery
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_IFACE_SIO_WM_STATE_INVALID,
  DS_3GPP_IFACE_SIO_WM_STATE_FLUSHED,
  DS_3GPP_IFACE_SIO_WM_STATE_UNFLUSHED
}ds_3gpp_iface_sio_wm_flush_state_e;

typedef struct
{
  ps_iface_type   *this_iface_ptr;
  ps_iface_type   *target_iface_ptr;
  void            *client_data;
}ds_3gpp_pdn_setup_bridge_info_type;

typedef struct 
{
  ds_umts_pdp_type_enum_type  ds_pdp_type;  /* Incoming PDP type            */
  cm_call_id_type        call_id;           /* incoming call Id             */
  apn_T                  apn_name;          /* Incoming APN                 */
  pdp_address_T          pdp_addr;          /* Incoming PDP addr            */
} ds_3gpp_pdn_incom_call_info_type;

typedef struct
{
  ps_iface_type       *iface_ptr;           /* PS Iface pointer      */
  uint32              pdn_instance;         /* PDN instance          */
  boolean             is_companion_iface_in_use; /*Companion iface's status*/
  boolean             ota_teardown;              /*OTA teardown status*/
  void                *info_ptr;            /* void pointer */
}ds_3gpp_pdn_iface_info_type;

/*---------------------------------------------------------------------------
  Structure is used when calling function in 3G DSMGR.  These structures
  are then passed right back to this interface.  Contains info that is
  important in configuring the response to orig or end a call.
  Specific to UMTS.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                 call_instance;     /* call instance to work on    */
  void                   *info_ptr;         /* Info pointer                */
  boolean                mt_flag;           /*indicates that the call orig
                                              is due to a MT request       */
  boolean                dormant_reorig;    /* Phys link dormant reorig or
                                            call bring up*/
  void                   *user_data;        /* This is used to hold any other
                                               info that needs to be passed.*/
  ps_iface_event_info_u_type  event_info;      /* Event Info passed by PS*/
  struct
  {
    int rm_dev_instance;
    int sm_instance;
  } rmsm_instance;

  boolean               low_priority_signaling; /*Set to indicate 
                                                    low priority signaling*/

} ds_3gpp_pdn_call_info_type;

typedef struct
{
  uint64                                allowed_ul_rate;
  /** UL Rate (in Kbps) that the lower layers can accept. (i.e)
   *  In other words, Given the current RF conditions, this
   *  converts to the max UL grants that the the device can
   *  send on UL */
  ps_sys_throughput_quality_enum_type   ul_throughput_quality;
  /** Given the current UL allowed rate, can the lower layers
   *  accept more packets to be sent on the UL? */
}ds_3gpp_throughput_quality_s;

/*! @brief Structure to hold 3GPPIface, IPv4 and IPv6
*/  
typedef struct 
{
  ds_3gpp_iface_state_e state;             /*!< keeps track of in_use and 
                                               not in use states          */
  ps_iface_type         ps_iface;          /*!< Memory for iface contents. 
                                                One for each iface group   */

  ps_iface_type         *rm_iface_ptr;    /*!< This is used to cache the Rm
                                               Iface pointer */

  void                  *client_data_ptr; /*!< This is used to associate
                                               the ds_3gpp_iface with the
                                               pdn_context */
  acl_type              acl;               /*!< ACL Info of the iface.     
                                                One for each iface group   */
  void                  *dhcp_handle;     /*!< Stores the handle allocated 
                                               to the DHCP client.         */
#ifdef FEATURE_DATA_PS_IPV6
  dssnet6_sm_cb_type    dssnet6_sm_cb;  /*!< DSSNET6 state machine      */
  ip6_sm_type           ip6_sm;      /*!< V6 state machine           */
#endif /* FEATURE_DATA_PS_IPV6 */
  boolean               in_use; /* Boolean flag indicating use / reservation*/

  boolean               post_proc_done_flag;
                                        /*!< Boolean indicating if post proc has
                                             already been done on this iface  */
  ps_iface_net_down_reason_type  ue_init_net_down_reason; /* In case UE initiates
                                                            call teardown store the
                                                            reason here so that
                                                            it can be propogated to the
                                                            apps. */
}ds_3gpp_iface_s ;

/*--------------------------------------------------------------------------------------
Enum to hold different A2 bridging information
----------------------------------------------------------------------------------------*/

typedef enum
{
  IS_A2_BRIDGED = 0x01,
  IS_A2_BRIDGING_DISALLOWED = 0x02,
  IFACE_SET_HW_PATH_PROCESSING_CMD = 0x04,
  IFACE_SET_SW_PATH_PROCESSING_CMD = 0x08,
  PROCESSING_GW_L_IRAT = 0x10
}ds_3gpp_a2_bridging_enum_type;

typedef enum
{
  DS_PDN_CONTEXT_STATE_DOWN         = 0,
  DS_PDN_CONTEXT_STATE_COMING_UP    = 1,
  DS_PDN_CONTEXT_STATE_PEND_REDIAL  = 2,
  DS_PDN_CONTEXT_STATE_UP           = 3,
  DS_PDN_CONTEXT_STATE_UP_DETACHED  = 4,
  DS_PDN_CONTEXT_STATE_ANSWER       = 5,  
  DS_PDN_CONTEXT_STATE_GOING_DOWN   = 6,
  DS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC      = 7,
  DS_PDN_CONTEXT_STATE_MAX
} ds_pdn_context_state_e;

/*! @brief Structure to hold pdn context  
*/

typedef struct 
{  
  uint32                    cookie;              /* Cookie to validate the dyn mem*/ 
  ds_pdn_context_state_e    state;                    /*!< PDN context state */
  ds_3gpp_iface_s           *ds_3gpp_iface_tbl[DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT]; 
  sys_modem_as_id_e_type    subs_id;     /* Subscription ID on which PDN was brought up */

  ds_umts_pdp_profile_type  pdp_profile;
  uint32                    pdp_profile_num;
  uint32                    pdp_group_id;                   /*!< CM assigned */  
  uint32                    num_active_bearer_context;
  void*                     bearer_context_ptr_tbl\
                            [DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT];
                                           /*!< Array: ptrs to BearerContext */
  void*                     def_bearer_context_ptr;

  void*                     vobj_data_ptr_tbl[DS_3GPPI_SYS_MODE_MAX];
                           /*!< Array of pointers to mode specific data blob */
  boolean                   handover_attach_flag; 
                            /*!< used to indicate HO attch*/
  boolean                   allow_iface_bring_up;

  rex_timer_type            pdn_inactivity_timer;
                            /*!< To tear down PDN connection on inactivity */

  rex_timer_type            polling_timer;
                            /*!< Use to poll lower layers for data activity*/

  rex_timer_type            apn_param_chg_timer;  /*! < 
                            Timer that would be started when Clients are indicated 
                            and given a chance to
                            teardown  -- Will run for 5 secs */

  boolean                   local_disconnect_sent;/* This will be used to keep
                                                     track of whether we sent
                                                     local disconnect after
                                                     HO to eHRPD           */
  boolean                   tear_down_in_progress;
                            /*!< To indicate that bearer down ind is received
                            but the pdn context is not torn down yet */

  network_params_info_type  info;
                            /*! Used to save dsumtsps_call_info_type.info_ptr 
                             Required for redialing */
  boolean                   info_null;
                            /*! Used to indicate if 
                              dsumtsps_call_info_type.info_ptr is NULL  */
  boolean                   is_default; /* Set if this is the default PDN */

  int                       index;      /* Index where the memory is allocated*/

  boolean                   ue_initiated_tear_down;
                                       /* Flag to indicate whether the Network
                                          or UE initiated the Tear Down */
  boolean                   keep_alive_pdn;
                                       /*Flag that indicates that this is 
                                       a keep alive PDN, not to be torn 
                                       down when an app using this PDN
                                       disconnects the data call*/

  uint8                     rab_reestab_call_type  \
                                   [DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT];
  /*!<Used during LTE RAB-ReEstablishment.Specifies Re-Establishment Call type 
     Used only during VoLTE Call*/

   uint8                     redial_reason;
                            /*State to know why a redial on the PDN occured*/
	 
  boolean                   apn_param_chg_teardown;
                                       /* Flag indicating that the pdn will be 
                                       put to pending redial state and a reconnection
                                       request will be sent out*/
  ps_iface_net_down_reason_type  net_down_reason;
                                       /* Used to cache the down reason received from
                                          the network*/
#ifdef FEATURE_EMERGENCY_PDN
  boolean                  pending_teardown;
#endif /*FEATURE_EMERGENCY_PDN*/
  boolean                  profile_is_valid;

  boolean                  force_ota_teardown;
                                       /* This can be set if OTA teardown needs 
                                       to be forced*/
 boolean                  ims_sip_reg_failure_flag; 
    /* flag is set by IMS Client using IOCTL when SIP signnalling fails */   
  boolean                  low_priority_signaling;
   /* Flag to indicate low priority signaling */

  ds_sys_system_status_info_type   bearer_tech;
                             /* Stores the Bearer Tech information 
                                to be passed on to EPC when PDN 
                                goes down */
}ds_pdn_context_dyn_s;

typedef struct
{
  ds_pdn_context_dyn_s *ds_pdn_context_dyn_p;
}ds_pdn_context_s;

/*---------------------------------------------------------------------------
  Table of handler functions for pdnContext. Each mode-specific handler 
  maintains such a table, and registers this table with pdnContext
---------------------------------------------------------------------------*/
/*! @brief handler function table for pdn context  
*/  
typedef struct 
{
  /*-------------------------------------------------------------------------
   Extracts IFACE configuration information from passed in cm_call_mode_info,
   validates it and configures IFACE with it. Returns success or failure 
   depending on whether passed in configuration info is valid or not. If not,
   cause code is populated in cm_end_params. Note that this is a virtual 
   function and calls the mode-specific function. 
  -------------------------------------------------------------------------*/
  boolean (*DS_PDN_CTXT_VF_FPN(activateInd))( 
                           ds_pdn_context_s                 *pdn_cntx_p,
                           const cm_call_mode_info_s_type   *mode_info_p,
                           cm_end_params_s_type             *end_params
                         );
    
  boolean (*DS_PDN_CTXT_VF_FPN(getOrigParams))(
                          ds_pdn_context_s                  *pdn_cntx_p,
                          ds_3gpp_orig_params_s             *mm_orig_params_p,                          
                          const ds_3gpp_pdn_call_info_type  *call_info
                       );
  /*--------------------------------------------------------------------------
    called to indicate bearer activation request indication.
    NULL if we are in non-LTE and pre W REL 8.
  ---------------------------------------------------------------------------*/
  boolean(*DS_PDN_CTXT_VF_FPN(bearerActivateInd))(
                         cm_call_id_type                  call_id,
                         const cm_call_mode_info_s_type   *mode_info_ptr_i,
                         cm_act_bearer_rsp_s_type         *cm_act_res_p,
                         sys_modem_as_id_e_type           subs_id
                       );


  void (*DS_PDN_CTXT_VF_FPN(pdnUpInd))(
                         ds_pdn_context_s                 *pdn_cntx_p
                       );

  void (*DS_PDN_CTXT_VF_FPN(resume_ind))(sys_modem_as_id_e_type subs_id);

  /*-------------------------------------------------------------------------
     Called to see if QOS is supported for this technology.
  ---------------------------------------------------------------------------*/
  boolean (*DS_PDN_CTXT_VF_FPN(qos_is_supported)) (void);


      /*---------------------------------------------------------------------------
    This function is invoked to allocate the umts pdn context
  ---------------------------------------------------------------------------*/
  boolean (*DS_PDN_CTXT_VF_FPN(allocate_umts_pdn_context))(
                             int index); 

 /*---------------------------------------------------------------------------
    This function is invoked to free the umts pdn context
  ---------------------------------------------------------------------------*/
  boolean (*DS_PDN_CTXT_VF_FPN(free_umts_pdn_context))(
                             int index); 


  /*-------------------------------------------------------------------------
    This function is invoked to get the throughput quality for the device.
    Throughput quality is calculated for the whole device as such and 
    not per bearer
    -------------------------------------------------------------------------*/
  boolean (*DS_PDN_CTXT_VF_FPN(report_ul_throughput_quality))
                      (sys_modem_as_id_e_type           cm_subs_id,
                       ds_3gpp_throughput_quality_s    *ul_throughput_quality);
  /*-------------------------------------------------------------------------
    This function is invoked when the client requests for 
    -------------------------------------------------------------------------*/
  int16 (*DS_PDN_CTXT_VF_FPN(cost_of_modem_power))
                       (void    *cost_modem_pwr,
                        int16   *ps_errno);

}ds_pdn_context_vftbl_s;

typedef enum
{
  DS_3GPP_EPS_VOBJ      = 0,
  DS_3GPP_UMTS_VOBJ     = 1,
  DS_3GPP_MAX_VOBJ        
} ds_3gpp_vobj_e;

typedef enum
{
  DS_3GPP_BCM_INVALID   = 0,
  DS_3GPP_BCM_UE_ONLY   = 1,
  DS_3GPP_BCM_UE_NW     = 2
} ds_3gpp_bcm_info_e;
typedef struct 
{
  uint8 version ; 		 /* version of the NV item*/
  boolean l_to_w_qos_cont_enabled; /* NV item value */
  uint8 reserve_bytes[2];    /* reserve bytes for future use*/
}ds_3gpp_l_to_w_qos_cont_s;


/*! @brief Structure to hold UMTS pdn context  
*/  

typedef struct 
{  
  uint32                    cookie;              /* Cookie to validate the dyn mem*/ 
  uint32                    index;
  pdp_address_T       offered_mt_address;
}ds_umts_pdn_context_dyn_s;

typedef struct 
{  
  ds_umts_pdn_context_dyn_s *ds_umts_pdn_context_dyn_p;
}ds_umts_pdn_context_s ;

/*! @brief Structure to hold EPS pdn context  
*/  

typedef struct 
{  
  void *dummy;
}ds_eps_pdn_context_s ;

/*---------------------------------------------------------------------------
  Enum to decide type of call to be originated based on Netpolicy table.
---------------------------------------------------------------------------*/
/* VRK:TODO Change name to DS3GPP and get rid of featurization */
typedef enum
{
  DS_3GPP_PDP_TYPE_FAIL              = -1,
  DS_3GPP_PDP_TYPE_IPV4              = 0,      /* V4 Call               */
  DS_3GPP_PDP_TYPE_PPP               = 1,      /* PPP Call              */
  DS_3GPP_PDP_TYPE_IPV6              = 2,      /* V6 Call               */
  DS_3GPP_PDP_TYPE_IP_ANY            = 3,      /* V4/V6 Call            */ 
  DS_3GPP_PDP_TYPE_MAX               = 0xFF     /* MAX                  */ 
                                          /* enum is defined as a byte    */
} ds_pdn_context_type_e;

/*---------------------------------------------------------------------------
  Below structure defines a single item in the Blocked PLMN List
---------------------------------------------------------------------------*/
typedef struct
{
  list_link_type     link;     /* Pointer to the next item in list */
  sys_plmn_id_s_type plmn_id;  /* PLMN Id */
} ds_3gpp_pdn_cntxt_blocked_plmn_type;

extern ds_pdn_context_vftbl_s ds_pdn_context_ftbl[DS_3GPPI_SYS_MODE_MAX];

#define PDN_ADDRESS_IPV4V6_V4_OFFSET 8
#define DS_MAX_APN_NUM DS_3GPP_MAX_PDN_CONTEXT

typedef enum 
{
     DS_IP_TYPE_INVALID = 0,
     DS_IPV4_ONLY,
     DS_IPV6_ONLY,
     DS_IPV4_SINGLE_BEARER,
     DS_IPV6_SINGLE_BEARER,
     DS_SINGLE_BEARER,
     DS_IPV4V6
} ds_apn_ip_support_type_e;

/*-------------------------------------------------- 
  @brief:
  enum containing the possible return values
  of applying Bearer mgmt algorithm during
  iface bring up
  --------------------------------------------------*/
typedef enum
{
    DS_3GPP_PDN_CNTXT_BEARER_MGMT_FAIL   = 0,
    DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS,
    DS_3GPP_PDN_CNTXT_BEARER_MGMT_NOT_REQD

}ds_3gpp_pdn_cntxt_bearer_mgmt_status_e;

typedef enum
{
  DS_3GPP_PDN_CNTXT_CM_NOTIFY_CMD_MIN       = 0,
  DS_3GPP_PDN_CNTXT_PWROFF_LPM_MODE         = 1,
  DS_3GPP_PDN_CNTXT_SUB_NOT_AVAIL_NOTIFY    = 2,
  DS_3GPP_PDN_CNTXT_CM_NOTIFY_CMD_MAX
}ds_3gpp_pdn_cntxt_cm_notify_cmd_enum_type;

typedef struct
{
  sys_modem_as_id_e_type                    subs_id;
  ds_3gpp_pdn_cntxt_cm_notify_cmd_enum_type cm_notify_type;
}ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type;

typedef enum
{
  DS_3GPP_INIT_DATA_STATS,
  DS_3GPP_CHECK_DATA_ACTIVITY
}ds_3gpp_operation_type_e;

typedef struct
{
  uint32  rxbyte_cnt;
  uint32  txbyte_cnt;
} ds_3gpp_dormancy_delay_stats_s;

/* Structure to hold delay dormancy related parameters */
typedef struct
{
  ds_3gpp_dormancy_delay_stats_s stats[DS_3GPP_MAX_PDN_CONTEXT];
  rex_timer_type                 *timer_p;
}ds_3gpp_delay_dormancy_s;

#define DSPDNCNTX_GET_PDN_FROM_IFACE(iface_ptr) \
        (iface_ptr->client_data_ptr != NULL)? \
        ((ds_3gpp_iface_s*)(iface_ptr->client_data_ptr))->client_data_ptr: 0

/*===========================================================================
MACRO DSPDNCNTXT_IS_DS_IFACE_VALID

DESCRIPTION   
  Given the pdn cntx ptr and DS Iface index, returns if the DS iface is non NULL

DEPENDENCIES  
  None.

RETURN VALUE  
  See Descritpion.
  
SIDE EFFECTS  
  None.
===========================================================================*/
#define DSPDNCNTXT_IS_DS_IFACE_VALID(pdn_cntx_p, iface_index)\
  ((((ds_pdn_context_s *)pdn_cntx_p)\
   ->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[iface_index]) != NULL? \
                                                                TRUE:FALSE)
/*===========================================================================
MACRO DSPDNCNTXT_GET_V4_PS_IFACE_PTR

DESCRIPTION   
  Given the pdn cntx ptr, returns pointer to associated IPV4 DS IFACE

DEPENDENCIES  
  None.

RETURN VALUE  
  See Descritpion.
  
SIDE EFFECTS  
  None.
===========================================================================*/
#define DSPDNCNTXT_GET_V4_DS_IFACE_PTR(pdn_cntx_p) \
  ( DSPDNCNTXT_IS_DS_IFACE_VALID(pdn_cntx_p,DS_3GPP_PDN_CONTEXT_IPV4_IFACE)?\
   (((ds_pdn_context_s *)pdn_cntx_p)->ds_pdn_context_dyn_p\
 ->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]) : NULL ) 

/*===========================================================================
MACRO DSPDNCNTXT_GET_V6_PS_IFACE_PTR

DESCRIPTION   
  Given the pdn cntx ptr, returns pointer to associated IPV6 DS IFACE

DEPENDENCIES  
  None.

RETURN VALUE  
  See Descritpion.
  
SIDE EFFECTS  
  None.
===========================================================================*/
#define DSPDNCNTXT_GET_V6_DS_IFACE_PTR(pdn_cntx_p) \
  ( DSPDNCNTXT_IS_DS_IFACE_VALID(pdn_cntx_p,DS_3GPP_PDN_CONTEXT_IPV6_IFACE)?\
   (((ds_pdn_context_s *)pdn_cntx_p)->ds_pdn_context_dyn_p\
 ->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]) : NULL ) 

/*===========================================================================
MACRO DSPDNCNTXT_GET_V4_PS_IFACE_PTR

DESCRIPTION   
  Given the pdn cntx ptr, returns pointer to associated IPV4 PS IFACE

DEPENDENCIES  
  None.

RETURN VALUE  
  See Descritpion.
  
SIDE EFFECTS  
  None.
===========================================================================*/
#define DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p) \
  ( DSPDNCNTXT_IS_DS_IFACE_VALID(pdn_cntx_p,DS_3GPP_PDN_CONTEXT_IPV4_IFACE)?\
  &( (((ds_pdn_context_s *)pdn_cntx_p)\
 ->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE])->\
 ps_iface): NULL )
 


/*===========================================================================
MACRO DSPDNCNTXT_GET_V6_PS_IFACE_PTR

DESCRIPTION   
  Given the pdn cntx ptr, returns pointer to associated IPV6 PS IFACE

DEPENDENCIES  
  None.

RETURN VALUE  
  See Descritpion.
  
SIDE EFFECTS  
  None.
===========================================================================*/
#define DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p) \
  ( DSPDNCNTXT_IS_DS_IFACE_VALID(pdn_cntx_p,DS_3GPP_PDN_CONTEXT_IPV6_IFACE)?\
  &( (((ds_pdn_context_s *)pdn_cntx_p)\
 ->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE])->\
 ps_iface ): 0 )

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes DS_3GPP_PDN_CONTEXT's per subscription info.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_context_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_INFO_INIT

DESCRIPTION
  This function initializes all the globals in DS_3GPP_PDN_CONTEXT.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_context_info_init
(
  void
);

/*===========================================================================
FUNCTION DS3GPP_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each pdn context and initializes the
  parameters for each pdn context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask containing the REX signals that the handlers want to wait on.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cntxt_3gpp_init( void );

/*===========================================================================
FUNCTION dsPdnContext_getPdnCntxByInst

DESCRIPTION
  This function returns pdn context ptr.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to associated pdn context

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_context_s *ds_pdn_cntxt_get_pdn_cntx_by_inst
( 
  uint32           pdn_cntx_inst
);

/*===========================================================================
FUNCTION dsPdnContext_getProfilePtr

DESCRIPTION
  This function allocates unused pdn context.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context

SIDE EFFECTS
  None.

===========================================================================*/
ds_umts_pdp_profile_type *ds_pdn_context_get_profile_ptr
( 
   ds_pdn_context_s*           pdn_cntx_p
);

/*===========================================================================
FUNCTION DSPDNCNTX_ALLOCATE_NEW_PDN_CONTEXT

DESCRIPTION
  This function allocates unused pdn context. The allocation of the PDN cntx
  is protected within a critical section since this function could be called
  as a part of route acl post processing. Since the route acl post processing
  happens from the application's context, we need this critical section.

PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context or NULL if there is none left.

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_context_s* ds_pdn_cntxt_allocate_new_pdn_context
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_allocBearerCntx

DESCRIPTION
  This function allocates an unused bearer cntx associated with the specified
  pdn cntx   

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated bearer context

SIDE EFFECTS
  None.

===========================================================================*/

void *ds_pdn_cntxt_alloc_bearer_cntx
( 
  ds_pdn_context_s *pdn_cntx_p,
  cm_call_id_type   call_id
  
);

/*===========================================================================
FUNCTION DSUMTSPS_FLOW_IOCTL_HDLR

DESCRIPTION
  Flow ioctl Callback called from application context for secondary PDP
  context activation. Central function for embedded applications deactivate a 
  secondary context,resume a dormant secondary context or modify qos 
  parameters of a secondary context.

PARAMETERS
  *flow_ptr         - Ptr to the flow to operate on (validated by calling fn)
  ioctl_name        - Type of request
  *argval_ptr        - QoS specified by the application (validated by calling fn)
  *ps_errno         - Ptr to error code (if any)


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
LOCAL int dsumtsps_flow_ioctl_hdlr
(
  ps_flow_type            *flow_ptr,
  ps_flow_ioctl_type      ioctl_name,
  void                    *argval_ptr,
  int16                   *ps_errno  
);

/*Function header*/
int ds_pdn_cntxt_flow_ioctl_hdlr
(
  ps_flow_type            *flow_ptr,
  ps_flow_ioctl_type      ioctl_name,
  void                    *argval_ptr,
  int16                   *ps_errno
  /*lint -esym(715,argval_ptr) */
);

/*===========================================================================

FUNCTION DSUMTSPS_CVT_NAS_PDP_TYPE_TO_DS_FMT

DESCRIPTION
  Converts NAS PDP TYPE value to DS PDP TYPE.

PARAMETERS
  nas_pdp_type - This is NAS PDP TYPE.

DEPENDENCIES
  None.

RETURN VALUE
  ds_umts_pdp_type_enum_type - PDP type in DS format.

SIDE EFFECTS
  None.

===========================================================================*/
ds_umts_pdp_type_enum_type ds_pdn_cntxt_convert_nas_pdp_type_to_ds_fmt
(
    uint8 nas_pdp_type
);

/*Function header*/
void ds_pdn_cntxt_fill_origination_apn
(
    ds_umts_pdp_profile_type     *pdp_profile_parms,
    cm_gw_ps_orig_params_s_type  *orig_params_ptr
);

/*Function header*/
sys_pdp_header_comp_e_type ds_pdn_cntxt_conv_header_comp_from_pdp_t_to_sys_t
(
  const ds_umts_pdp_header_comp_e_type *pdp_h_comp
);

/*Function header*/
sys_pdp_data_comp_e_type ds_pdn_cntxt_conv_data_comp_from_pdp_t_to_sys_t
(
  const ds_umts__pdp_data_comp_e_type *pdp_d_comp
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_setupIPv4PSIface

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  pdn_cntx_p          - PDN cntx ptr
  ds_3gpp_pdp_addr_p  - Addr to configure V4 iface with
  end_params          - Fill in end_params in case of error

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
boolean ds_pdn_cntxt_setup_ipv4_ps_iface
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,
  cm_end_params_s_type           *end_params 
);

#ifdef  FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION ds_pdn_cntxt_setupIPv6PSIface

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  pdn_cntx_p          - PDN cntx ptr
  ds_3gpp_pdp_addr_p  - Addr to configure V6 iface with
  end_params          - Fill in end_params in case of error

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
boolean ds_pdn_cntxt_setup_ipV6_ps_iface
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,
  cm_end_params_s_type           *end_params 
);
#endif //FEATURE_DATA_PS_IPV6

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DSPDNCNTX_GEN_PCSCF_ADDRESS_REQUEST_PACKET

DESCRIPTION
  This function generates a PCSCF request packet to request the 
  pcscf address.

PARAMETERS   
  protocol_cfg_options_T *protocol_config_options_p
    
DEPENDENCIES 
  None.

RETURN VALUE  
  DS3G_SUCCESS - packet generated succesfully
  DS3G_FAILURE - pkt not generated
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_pdn_cntxt_gen_pcscf_address_request_packet
(
  protocol_cfg_options_T         *protocol_config_options_p,
  const ds_umts_pdp_profile_type *profile_p, /* profile parameters */
  sys_modem_as_id_e_type          subs_id
);

/*===========================================================================
FUNCTION DSPDNCNTX_GEN_IM_CN_SUBSYSTEM_REQUEST_PACKET

DESCRIPTION
  This function generates a IM CN request packet to request the 
  signaling only context.

PARAMETERS   
  protocol_cfg_options_T *protocol_config_options_p 
    
DEPENDENCIES 
  None.

RETURN VALUE  
  DS3G_SUCCESS - packet generated succesfully
  DS3G_FAILURE - pkt not generated
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_pdn_cntxt_gen_im_cn_subsystem_request_packet
(
  protocol_cfg_options_T *protocol_config_options_p 
);
#endif //FEATURE_DATA_IMS

/*===========================================================================
FUNCTION DS_PDN_CNTXT_PROCESS_PDP_PROFILE_INFO

DESCRIPTION
  This function Sets the PDP profile to be used for the call instance
  specified. It also gets the profile parameters for the call and stores
  it in the umtsps iface table.

  This function is called by the routing layer before to set this information.
  Subsequent to this, the call origination happens.

PARAMETERS
  pdn_cntx_p     - ptr to PDN context
  profile_number - profile number
  roaming_status - UE roaming status
  ip_support_p   - ptr to ip support. Out parameter

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cntxt_process_pdp_profile_info
(
  ds_pdn_context_s          *pdn_cntx_p,
  uint32                     profile_number, /* PDP context config info*/
  ds_apn_ip_support_type_e  *ip_support_p,
  ds_umts_pdp_profile_type  *prof_context
);

/*===========================================================================

FUNCTION DSUMTSPS_PROCESS_PS_CMD

DESCRIPTION

  This function processes the PS commands received by the DS task.
  The  commands that can be received by the mode specific handler
  are as follows:

  DSUMTSPS_IFACE_UP_CMD: Command to bring up the UMTS Interface  
  DSUMTSPS_IFACE_DOWN_CMD: Command to tear down the UMTS Interface
  DSUMTSPS_SEND_MT_REJECT_CMD: Command to send MT reject when the MT timer
                               expires

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_process_ps_cmd
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
);

/*===========================================================================

FUNCTION ds_pdn_cntxt_storeEncodedApn

DESCRIPTION
  This function encodes the apn name contained in apn_src
  and stores it in apn_dest.


PARAMETERS

  apn_dest:   Destination to store the encoded apn.

  apn_src:    Uncoded APN name.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the encoded APN

SIDE EFFECTS

===========================================================================*/
uint8 ds_pdn_cntxt_store_encoded_apn
(
  byte *apn_dest,
  byte *apn_src
);

/*===========================================================================
FUNCTION DSPDNCNTX_USE_DEFAULT_DIAL_STRING

DESCRIPTION
  This function is used by bearer context to figure out if it has to use the
  default dial string to bring up the call.

PARAMETERS
  IN
    ps_ip_type - This is the IP type of the PS Iface

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE  - if default dial string is to be used
  FALSE - if otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cntxt_use_default_dial_string
(
  ds_pdn_context_s  *pdn_cntx_p,
  void               *info_ptr
);
/*===========================================================================
FUNCTION DSPDNCNTX_FILL_PAP_ORIG_PARMS

DESCRIPTION
  This function fills the PAP specific protocol information in the PDP context
  req. message

PARAMETERS   :
    protocol_cfg_options_T *protocol_config_options_p,
    *username            - Ptr to User id
    user_id_len          - Length of User id
    *password            - Ptr to password
    password_len         - Length of password

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error

SIDE EFFECTS
  Updates protocol_config_options_p contents :
  protocol_config_options_p->num_protocol_ids will be updated
  to reflect the no. of PPP packets included by this function

===========================================================================*/
boolean ds_pdn_cntxt_fill_pap_orig_parms
(
  protocol_cfg_options_T       *protocol_config_options_p,
  const byte                   *username,
  byte                         user_id_len,
  const byte                   *password,
  byte                         password_len
);

/*===========================================================================
FUNCTION DSPDNCNTX_GEN_AND_FILL_CHAP_ORIG_PARMS

DESCRIPTION
  This function generates and fills the CHAP specific protocol information in
  the PDP context req. message. The chap challenge and response are generated
  internally.

PARAMETERS   :
    protocol_cfg_options_T       *protocol_config_options_p
    *username            - Ptr to User id
    user_id_len          - Length of User id
    *password            - Ptr to password
    password_len         - Length of password

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error

SIDE EFFECTS
  Updates Orig_params contents :
  orig_params_ptr->config_options.num_protocol_ids will be updated
  to reflect the no. of PPP packets included by this function

===========================================================================*/
boolean ds_pdn_cntxt_gen_and_fill_chap_orig_parms
(
  protocol_cfg_options_T       *protocol_config_options_p,
  const byte                   *username,
  byte                         user_id_len,
  const byte                   *password,
  byte                        password_len
);

/*===========================================================================
FUNCTION DSPDNCNTX_DHCP_STATUS_CB()

DESCRIPTION
  This function processes the DHCP client indications for a DHCPv4 address
  allocation.

PARAMETERS
  callback_data     -   pointer to store the call instance
  dhcp_status       -   enum to indicate dhcp status
  
RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_cntxt_dhcp_status_cb
(
  void                    *callback_data,
  dhcp_client_status_enum  dhcp_status
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_setupPdpIPprotoStack

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  pdn_cntx_p                - PDN cntx ptr
  ds_3gpp_pdp_addr_p        - Addr to configure V4V6 iface with
  protocol_config_options_p - PCO ptr
  end_params                - Fill in end_params in case of error

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
boolean ds_pdn_cntxt_setup_pdp_ip_proto_stack
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,  
  const protocol_cfg_options_T   *protocol_config_options_p,
  cm_end_params_s_type           *end_params 
);

/*===========================================================================
FUNCTION DSPDNCNTX_IS_DEFAULT

DESCRIPTION
  This function checks whether the pdn context pointer points to the 
  default pdn context

PARAMETERS   :
    ds_pdn_context_s * - pdn context pointer
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - This pdn context pointer points to the default pdn context
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_default(ds_pdn_context_s* pdn_cntx_p);

/*===========================================================================
FUNCTION DSPDNCNTX_IS_DEFAULT_IFACE

DESCRIPTION
  This function checks whether if the iface pointer points to one of the 
  default ifaces

PARAMETERS   :
    ps_iface_type * - iface pointer
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - This iface pointer points to a default iface
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_default_iface(ps_iface_type *iface_p);


/*===========================================================================
FUNCTION      DSPDNCNTX_SET_DEFAULT_BEARER

DESCRIPTION
  This function is used to set the default bearer field of a PDN context with
  the pointer to the bearer context provided.

PARAMETERS
  IN
    pdn_context_p    - This is the PDN context inside which we want to store
                       the default bearer info
    bearer_context_p - This is the pointer to the default bearer context

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_cntxt_set_default_bearer
(
  ds_pdn_context_s    *pdn_context_p,
  void *bearer_context_p
);

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CONTEXT_V6_SM_UPDATE

DESCRIPTION
  This function updates the v6 sm with iface info

PARAMETERS
  IN
  *ds3gpp_iface_p - pointer to iface
  subs_id - subs id associated with iface

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_context_v6_sm_update
(
  ds_3gpp_iface_s  *ds3gpp_iface_p,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DSPDNCNTX_STORE_BEARER_CONTEXT

DESCRIPTION
  This function is used to store the bearer context information in the PDN
  context.

PARAMETERS
  IN
    pdn_context_p    - This is the PDN context inside which we want to store
                       the bearer
    bearer_context_p - This is the pointer to the bearer context
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - on successfully storing the bearer info
  DS3G_FAILURE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_store_bearer_context
(
  ds_pdn_context_s    *pdn_context_p,
  void  *bearer_p
);

/*===========================================================================
FUNCTION      DSPDNCNTX_RELEASE_BEARER_CONTEXT

DESCRIPTION
  This function is used to release the bearer context information in the PDN
  context.

PARAMETERS
  IN
    pdn_context_p    - This is the PDN context inside which we want to release
                       the bearer
    bearer_context_p - This is the pointer to the bearer context
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - on successfully storing the bearer info
  DS3G_FAILURE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_release_bearer_context
(
  ds_pdn_context_s    *pdn_context_p,
  void  *bearer_p
);


/*
  This function is used to check the Iface address family agains the type of
  the call to be originated. If the types are the same, this function returns
  TRUE. Otherwise it returns FALSE.
*/
boolean ds_pdn_cntxt_check_iface_type_matches(ps_iface_type*, ds_pdn_context_type_e);


/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPCP_ORIG_PARAMS

DESCRIPTION
  This function fills the IPCP (Primary and Secondary DNS only) specific 
  protocol information in the PDP context request message.
  
PARAMETERS 
    *protocol_config_options_p     - Ptr to protocol config options 
    req_spec_dns_flg     - IS dns address requested
    primary_dns          - primary dns address requested
    secondary_dns        - secondary dns address requested
    req_spec_nbns_flg    - IS nbns address requested
    primary_nbns         - primary WINS address
    secondary_nbns       - secondary WINS address
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPCP packet generated succesfully and included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     protocol_config_options_p->num_protocol_ids will be updated
     to reflect the no. of PPP packets included by this function
     
===========================================================================*/
boolean ds_pdn_cntxt_fill_ipcp_orig_params
(
  protocol_cfg_options_T       *protocol_config_options_p,
  boolean                       req_spec_dns_flg,/* Should spec DNS add req*/
  uint32                        primary_dns,    /* primary DNS             */
  uint32                        secondary_dns,  /* secondary DNS           */
  boolean                       req_spec_nbns_flg,/*Should spec NBNS add req*/
  uint32                        primary_nbns,   /* primary WINS            */
  uint32                        secondary_nbns, /* secondary WINS          */
  net_cfg_params_mask_e_type    valid_fields    /* IPCP fileds to request */
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_PDP_PROFILE_NUM

DESCRIPTION
  Function to retrieve the profile params related to a call from the
  iface table and call info table.

PARAMETERS
  iface_ptr;   - Ptr to ps iface inst    
  argval_ptr   - Pointer to store pdp profile number
  ps_errno     - Error information

DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
       DS_EFAULT   if NULL pointers passed.
   
SIDE EFFECTS
  None
===========================================================================*/

int ds_3gpp_pdn_cntxt_get_pdp_profile_num
(
  ps_iface_type     *iface_ptr,
  void              *argval_ptr,
  int16             *ps_errno
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FILL_IPV4_DNS_CONTAINER

DESCRIPTION
  This function appends DNS server Ipv4 address request container to the 
  PCO buffer. 
  
PARAMETERS   
  protocol_cfg_options_T       *protocol_config_options_p
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV4 DNS packet container appended succesfully
  FALSE- Error

SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_fill_ipv4_dns_container
(
  protocol_cfg_options_T       *protocol_config_options_p
);

#ifdef FEATURE_DATA_PS_IPV6

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPV6_DNS_PARAMS

DESCRIPTION
  This function fills the IPV6 (DNS only) specific protocol information 
  in the PDP context request message.
  
PARAMETERS 
    *protocol_config_options_p     - Ptr to protocol config options PCO 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV6 DNS packet generated succesfully and included in PDP context 
  request message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     protocol_config_options_p->num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
boolean ds_pdn_cntxt_fill_ipv6_dns_params
(
  protocol_cfg_options_T       *protocol_config_options_p
);
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_ADDRESS_ALLOC_PARAM

DESCRIPTION
  This function fills the IPV4 address allocation container in PCO
  
PARAMETERS 
    *protocol_config_options_p - Ptr to protocol config options
    *profile_p           - Ptr to ds profile
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Successfully filled IPv4 Address allocation in PCO
  FALSE- Error
  
SIDE EFFECTS 
  Updates pco contents : 
     protocol_config_options_p->num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_address_alloc_param
(
  protocol_cfg_options_T    *protocol_config_options_p, /* Ptr to PCO */
  ds_umts_pdp_profile_type  *profile_p
);

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_BCM_REQUEST

DESCRIPTION
  This function fills the BCM container in PCO
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  none
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_bcm_indication
(
  protocol_cfg_options_T    *protocol_config_options_p
);

/*===========================================================================
FUNCTION DSPDNCNTX_UPDATE_APN_TABLE

DESCRIPTION
  This function updates the APN table with the supplied info

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_update_apn_table
(
  byte                      *apn,
  ds_apn_ip_support_type_e  ip_support,
  ds_pdn_context_s          *pdn_ptr
);

/*===========================================================================
FUNCTION DSPDNCNTX_RECOMPUTE_IP_SUPPORT_FOR_APN

DESCRIPTION
  This function recomputes the IP support for a given APN. Recomputation is
  done based on two aspects:
  1. apriori knowledge about the IP support of an APN. This is usually 
     assumed based on the PDP type
  2. New data from the network - The new data that this function uses is
     from the PDN reject indication. If the call is accepted, there is no
     need to recompute as the assumed IP support is correct.

  This function gets the recomputed IP support and updates the APN
  table so that the correct info is present in the table.

PARAMETERS
  IN
    rejected_pdn_cntx_p - PDN context that has to be removed from the table
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_cntxt_recompute_ip_support_for_apn
(
  ds_pdn_context_s  *rejected_pdn_cntx_p
);


/*===========================================================================
FUNCTION DSPDNCNTX_GET_PDN_CONTEXT_INSTANCE

DESCRIPTION
  This function returns the instance of the pdn context passed into the fn.
  The instance is simply the index of the pdn context in the table of
  PDN objects

PARAMETERS   
  pdn_cntx_p - Pointer to the PDN context whose index is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  Int - Instance is positive if found, -1 otherwise

SIDE EFFECTS
  None

===========================================================================*/
int ds_pdn_cntxt_get_pdn_context_instance
(
  ds_pdn_context_s*           pdn_cntx_p
);

/*===========================================================================
FUNCTION      DS_3GPP_GET_NUM_ACTIVE_DS_IFACES_ON_SUBS_ID

DESCRIPTION 
    This function returns the total number of ds3gpp ifaces that 
    are currently in use on the incoming subscription ID

PARAMETERS 
  sys_modem_as_id_e_type  -   subscription Id to check number of ifaces
                              active on that subscription
 
RETURN VALUE 
  uint8                   -   Number of ds3gpp DS ifaces in use
 
DEPENDENCIES 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
uint8 ds3gpp_get_num_active_ds_ifaces_on_subs_id
(
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_PDN_FROM_PDP_GROUP_ID

DESCRIPTION
  This function is used to find the pdn context, given pdp group id

PARAMETERS
  IN
    pdp_group_id - pdp group id provided by CM

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the pdn context if there is a pdp group id match
  NULL if there is no match

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_context_s* ds_pdn_cntxt_get_pdn_from_pdp_group_id(byte pdp_group_id);

/*===========================================================================
FUNCTION DSPDNCNTX_GET_PDN_CONTEXT_STATE

DESCRIPTION
  This function returns the PDN state of the Profile number passed into the fn.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  PDN state - if no valid instance found, 
              return DS_PDN_CONTEXT_STATE_DOWN state

SIDE EFFECTS
  None

===========================================================================*/
ds_pdn_context_state_e ds_pdn_cntxt_get_pdn_context_state
(
  uint32                    profile_num,
  sys_modem_as_id_e_type    subs_id 
);

/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_PDN_CONTEXT_STATE_FOR_DDS

DESCRIPTION
  This function returns the PDN state of the Profile number passed into the fn.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  PDN state - if no valid instance found, 
              return DS_PDN_CONTEXT_STATE_DOWN state

SIDE EFFECTS
  None

===========================================================================*/
ds_pdn_context_state_e ds_pdn_cntxt_get_pdn_context_state_for_dds
(
  uint32                    profile_num
);

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ATI_DYNAMIC_INFO

DESCRIPTION
  This function returns the PDN state info of the Profile number 
  passed into the function. It wil return v4,v6 iface pointer,
  eps_bearer_id and APN.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if no valid instance found, 
  TRUE - if valid pdn_state info is present

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_ati_dynamic_info
(
  uint32                             p_cid,
  sys_modem_as_id_e_type             subs_id,
  ds_3gpp_ati_pdn_dynamic_info_type *pdn_state_info_ptr
);



/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_ATI_DYNAMIC_INFO_FOR_DDS

DESCRIPTION
  This function returns the PDN state info of the Profile number 
  passed into the function. It wil return v4,v6 iface pointer,
  eps_bearer_id and APN.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if no valid instance found, 
  TRUE - if valid pdn_state info is present

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_ati_dynamic_info_for_dds
(
  uint32                             p_cid,
  ds_3gpp_ati_pdn_dynamic_info_type *pdn_state_info_ptr
);

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ALL_PDN_CONTEXT_PROFILES

DESCRIPTION
  This function returns the list of all primary Profile number that are 
  currently active.
  
PARAMETERS   
  profile_list_ptr  - list of profile numbers which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntx_get_all_pdn_context_profiles
(
  sys_modem_as_id_e_type                 subs_id,
  ds_3gpp_atif_prim_profile_list_type*   profile_list_ptr
);

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ALL_PDN_CONTEXT_PROFILES_FOR_DDS

DESCRIPTION
  This function returns the list of all primary Profile number that are 
  currently active.
  
PARAMETERS   
  profile_list_ptr  - list of profile numbers which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntx_get_all_pdn_context_profiles_for_dds
(
  ds_3gpp_atif_prim_profile_list_type*   profile_list_ptr
);

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ALL_IFACE_STATS

DESCRIPTION
  This function returns the cumulative rx and tx byte statistics for ifaces
  associated with a PDN context.

PARAMETERS   
  cm_call_id[IN]     - The call_id.
  txcntp[OUT]        - Pointer to Tx byte count.
  rxcntp[OUT]        - Pointer to Rx byte count.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - On successfully returning iface byte count.
  DS3G_FAILURE - On failure to return iface byte count.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_cntx_get_iface_stats
(
  cm_call_id_type cm_call_id,
  uint32          *rxcntp,
  uint32          *txcntp
);

/*===========================================================================
FUNCTION  DS_3GPP_REPORT_THROUGHPUT_INFO_FOR_IFACE

DESCRIPTION 
 For the given iface, this utility function reports the iface
 throughput stats

PARAMETERS 
  
  ps_iface_type*                - Pointer to the PS iface type
 
  ps_sys_throughput_info_type*  - Pointer to fill in the iface throughput
                                  stats
 
DEPENDENCIES 
  This utility function will be used only when
  * Iface is active
        &
  * Throughput Info Indication timer is running

RETURN VALUE
  TRUE   - If the throughput stats for the incoming iface is reported
           successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds_3gpp_report_throughput_info_for_iface
(
  ps_iface_type                 *ps_iface_p,
  ps_sys_throughput_info_type   *iface_info_list_p
);
/*===========================================================================
FUNCTION DSPDNCNTX_GET_PDN_CONTEXT_PDP_ADDR

DESCRIPTION
  This function returns the PDP addr of the PDN connection for which 
  the Profile number passed.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  : if the call succeds
  FALSE : if it fails

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_pdn_context_pdp_addr
(
  uint32                    profile_num,
  sys_modem_as_id_e_type    subs_id,
  ds_umts_pdp_addr_type     *pdp_addr_ptr
);

/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_PDN_CONTEXT_PDP_ADDR_FOR_DDS

DESCRIPTION
  This function returns the PDP addr of the PDN connection for which 
  the Profile number passed.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  : if the call succeds
  FALSE : if it fails

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_pdn_context_pdp_addr_for_dds
(
  uint32                    profile_num,
  ds_umts_pdp_addr_type     *pdp_addr_ptr
);

/*===========================================================================
FUNCTION DSPDNCNTX_ALLOCATE_DS_IFACE

DESCRIPTION
  This function is used to allocate a DS Iface primarily for Dual IP bearer
  scenarios.

  It loops through all the DS Ifaces and allocates one that is not marked as
  being in use. Since the allocation happens in Route ACL post processing, it
  could happen in a different task context. As a result, allocation and
  deallocation should be protected under critical sections.

PARAMETERS   
  None.
    
DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_iface_s* - Pointer to the DS iface allocated.

SIDE EFFECTS
  None

===========================================================================*/
ds_3gpp_iface_s* ds_pdn_cntxt_allocate_ds_iface(void);

/*===========================================================================
FUNCTION DSPDNCNTX_IS_UE_ATTACHED

DESCRIPTION
  This function is used to find out if the UE is already attached to any PDN
  It traverses through the list of PDNs and finds out if any of the PDN 
  contexts is in UP state

PARAMETERS   
  subs_id: Subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if the UE is connected to atleast one PDN
  DS3G_FAILURE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_ue_attached
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DSPDNCNTX_HANDLE_CNTXT_TRANSFER_IND

DESCRIPTION
  This function is used to invoke any target-rat specific operations on
  receiving context-transfer indication.

PARAMETERS   
  target rat
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_handle_cntxt_transfer_ind
(
  sys_sys_mode_e_type       target_rat,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DSPDNCNTX_UNBIND_DEFAULT_FLOWS

DESCRIPTION
  This function is used to unbind the phys link from all the default flows
  associated with the Ifaces of a given PDN context

PARAMETERS   
  void* - pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_unbind_default_flows(void* input_param_p);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_APN_THROTTLED

DESCRIPTION
  This function checks if an apn is throttled or not
 
PARAMETERS 
  pdn_cnt_p      - Pointer to the PDN context
  iface_p        - Pointer to the iface which is to be examined
    
DEPENDENCIES

RETURN VALUE
  True -  if iface is throttled
  False - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean 
ds_3gpp_pdn_cntxt_is_apn_throttled
(
  ds_pdn_context_s *pdn_cntx_p,
  ps_iface_type       *iface_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SETUP_PDN_WITH_DS_IFACE

DESCRIPTION
  This function associates a valid DS IFACE to a newly allocated pdn context
  Updated the pdp type in the profile and the APN table as required.
 
PARAMETERS 
  new_pdn_cnt_p      - Pointer to the newly allocated PDN context
  profile_num        - Profile from which pdn is to be updated
  ds_iface_p         - Pointer to a previously valid DS iface that is being
                       associated with the newly allocated PDN
  ds_iface_index     - Index into which ds_iface_p needs to be placed within
                       the DS iface table of the new pdn context
  old_apn            - Pointer to the apn string
  old_pdn_cntx_p    - Pointer to the old PDN context
      
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Unbinds the phys link from the flow of the DS iface passed.
===========================================================================*/
void ds_3gpp_pdn_cntxt_setup_pdn_with_ds_iface 
(
ds_pdn_context_s          *new_pdn_cntx_p, 
ds_umts_pdp_profile_type  *old_pdp_profile_p,
ds_3gpp_iface_s           *ds_iface_p, 
int                       ds_iface_index,
ds_pdn_context_s          *old_pdn_cntx_p
);

/*===========================================================================
FUNCTION DSPDNCNTX_DISSOCIATE_IFACE

DESCRIPTION
  This function dissociates the PDN context from the Iface that was not
  granted in Activate Bearer Indication

PARAMETERS   
  pdn_cnt_p          - Pointer to the PDN context
  ip_support_granted - The IP support that was returned by the Network
    
DEPENDENCIES
  Should always be called from within a critical section 
  ds3gpp_global_crit_sect

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_dissociate_iface
(
  ds_pdn_context_s         *pdn_cntx_p, 
  ds_apn_ip_support_type_e ip_support_granted
);

/*===========================================================================
FUNCTION DSPDNCNTX_SET_HANDOVER_ATTACH_FLAG

DESCRIPTION
  This function sets the handover attach flag maintained in the PDN context
  This flag will be used to findout whether we have to send initial request
  or handover attach while in the EPS mode of operation

PARAMETERS   
  handover_attach_flag - 
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_set_handover_attach_flag
(
  ds_pdn_context_s *pdn_cntx_p,
  boolean          handover_attach_flag
);

/*===========================================================================
FUNCTION DSPDNCNTX_ACTIVATEIND

DESCRIPTION
  Extracts IFACE configuration information from passed in cm_call_mode_info,
  validates it and configures IFACE with it. Returns success or failure 
  depending on whether passed in configuration info is valid or not. If not,
  cause code is populated in cm_end_params. Note that this is a virtual 
  function and calls the mode-specific function. 

PARAMETERS   
  pdn_cntx_p   - ptr to pdn context
  mode_info_p  - mode info ptr
  end_params   - call end params 
  subs_id      - Subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_activate_ind
(
  ds_pdn_context_s                  *pdn_cntx_p,
  const cm_call_mode_info_s_type    *mode_info_p,
  cm_end_params_s_type              *end_params,
  sys_modem_as_id_e_type             subs_id
);

/*===========================================================================
FUNCTION DSPDNCNTX_GETORIGPARAMS

DESCRIPTION
  This function returns the call origination parameters.

PARAMETERS   
  pdn_cntx_p        - ptr to pdn context
  mm_orig_params_p  - mm orig params
  call_info         - call info
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_orig_params
(
  ds_pdn_context_s                     *pdn_cntx_p,
  ds_3gpp_orig_params_s                *mm_orig_params_p,                          
  const ds_3gpp_pdn_call_info_type     *call_info
);

/*===========================================================================
FUNCTION DSPDNCNTX_BEARERACTIVATEIND

DESCRIPTION
  Called to indicate berarer activation request indication.

PARAMETERS   
  call_id           - call id
  mode_info_ptr_i   - mode info ptr
  subs_id           - subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_bearer_activate_ind
(
  cm_call_id_type                   call_id,
  const cm_call_mode_info_s_type   *mode_info_ptr_i,
  cm_act_bearer_rsp_s_type         *cm_act_res_p, 
  sys_modem_as_id_e_type            subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_GET_BRIDGED_IFACE

DESCRIPTION
  This function returns the cached Rm Iface pointer.

PARAMETERS   
  ds_iface_ptr - The DS iface whose bridged iface is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  RM Iface pointer if the TE call is up
  NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
ps_iface_type* ds_3gpp_pdn_context_get_bridged_iface
(
  ds_3gpp_iface_s  *ds_iface_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_GET_BRIDGED_SIO_PORT

DESCRIPTION
  This function returns the cached sio port info.

PARAMETERS   
  ds_iface_ptr - The DS iface whose bridged iface is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  sio port number if the TE call is up
  SIO_PORT_NULL  otherwise

SIDE EFFECTS
  None

===========================================================================*/
sio_port_id_type ds_3gpp_pdn_context_get_bridged_sio_port
(
  ds_3gpp_iface_s  *ds_iface_ptr
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_decodeProtocolParams

DESCRIPTION
  This function decodes the protocol parameters received for a PDP-IP call.

PARAMETERS
    call_id        - Call id of the call in progress
   *mode_info_ptr  - Pointer to the mode-specific call information provided
                     by Call Manager
DEPENDENCIES
  None.

RETURN VALUE
  Integer bitmask representing the decoded packets 

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_pdn_cntxt_decodeProtocolParams
(
  ds_3gpp_iface_s                *ds3gpp_iface_v4_p,
                                      /* Index into array of call states  */
  ds_3gpp_iface_s                *ds3gpp_iface_v6_p,
  const protocol_cfg_options_T   *protocol_config_options_p  
);

/*===========================================================================
FUNCTION dsPdnContext_bringUpIPv4PSIface

DESCRIPTION
  This function completes IPv4 PS iface configuration  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  

===========================================================================*/
void dsPdnContext_bringUpIPv4PSIface
(   
  ds_3gpp_iface_s  *ds3gpp_iface_p  
);



#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_AUTH_PARAMS_FROM_INFO

DESCRIPTION
  This function fills the authentication information obtained from the info_ptr
  in the iface up cmd hdlr.
  
PARAMETERS 
  *protocol_config_options_p    - config option that has to be filled
  *profile_p,                   - PDP profile
  *ppp_info_p                   - info pointer
  subs_id                       - Subscription Id

                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Auth info not included in PDP context request.
  
SIDE EFFECTS 
     
===========================================================================*/
boolean ds_pdn_cntxt_fill_auth_params_from_info
(
  protocol_cfg_options_T           *protocol_config_options_p,
  ds_umts_pdp_profile_type         *profile_p, 
  const network_params_info_type   *ppp_info_p,
  sys_modem_as_id_e_type            subs_id
);

/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_OPER_PCO_INFO

DESCRIPTION
  This function fills the operator information obtained.
  
  
PARAMETERS 
  *protocol_config_options_p    - config option that has to be filled
  *profile_p,    -   PDP profile

                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Successfuly updated the operator related info.
  FALSE- Error
  
SIDE EFFECTS 
     
===========================================================================*/

boolean ds_pdn_cntxt_fill_oper_pco_info_from_profile
(
  protocol_cfg_options_T       *protocol_config_options_p,
  ds_umts_pdp_profile_type     *profile_p
);


/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_OPER_PCO_INFO_FROM_APP

DESCRIPTION
  This function fills the operator specific protocol information in 
  the PDP context req. message. 

PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    profile_p - Prt to the profile info    
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_pdn_cntxt_fill_oper_pco_info_from_app
(
  protocol_cfg_options_T       *protocol_config_options_p,
  void                         *user_data_to_query_pco
);

/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_AUTH_PARAMS_FROM_PROFILE

DESCRIPTION
  This function fills the authentication information obtained from profile
  in the PDP context request message.
  
PARAMETERS 
  *protocol_config_options_p    - Config option that has to be filled
  *profile_p                    - PDP profile
  subs_id                       - Susbcription Id

                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
    
===========================================================================*/
boolean ds_pdn_cntxt_fill_auth_params_from_profile
(
  protocol_cfg_options_T           *protocol_config_options_p,
  const ds_umts_pdp_profile_type   *profile_p,
  sys_modem_as_id_e_type            subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_SET_CURRENT_PLMN

DESCRIPTION
  This function sets the current PLMN that we are operating on.
  This function can be called from any mode specific handler. It fills up
  the container we maintain in PDN context to hold the current PLMN

  This function does not check if the incoming PLMN ID is 0. It is the caller's
  responsibility to check this.

PARAMETERS
  plmn_id_ptr - pointer to PLMN id that has to be copied to the DS container
  subs_id     - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_set_current_plmn_ex
(
  sys_plmn_id_s_type      *plmn_id_ptr,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_CURRENT_PLMN

DESCRIPTION
  This function fetches the current PLMN Id corresponding to the given Subs Id

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned
  subs_id     - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_get_current_plmn_ex
(
  sys_plmn_id_s_type*     plmn_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_SET_RPM_PLMN_BLOCKED

DESCRIPTION
  This function sets the RPM PLMN blocked flag.
 
PARAMETERS
  blocked - RPM PLMN blocke or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_set_rpm_plmn_blocked
(
  boolean                  blocked
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_RPM_PLMN_BLOCKED

DESCRIPTION
  This function gets the RPM PLMN blocked flag.

PARAMETERS
  None.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  RPM PLMN blocked
  FALSE: RPM PLMN not blocked

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntx_get_rpm_plmn_blocked
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_BLOCKED_PLMN_LIST

DESCRIPTION
  This function gets the Blocked PLMN List.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  Blocked PLMN List

SIDE EFFECTS
  None
===========================================================================*/
list_type* ds_3gpp_pdn_cntx_get_blocked_plmn_list
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_SET_LOCAL_DISCON_FLAG

DESCRIPTION
  This function is used to set the local disconnect flag for a given PDN
  context

PARAMETERS
  pdn_cntx_ptr - PDN context where this has to be set

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_set_local_discon_sent_flag
(
  ds_pdn_context_s* pdn_cntx_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_BEARER_FOR_PDN_IN_GOING_DOWN

DESCRIPTION
  This function is used to determine if there are any PDN contexts that are
  in going Down state for which local disconnect has not been sent yet. 
  If yes, this function returns the pointer to the default bearer context
  of that PDN

  Note: This is called only during local deactivation of contexts after IRAT
  This should not be called under any other circumstances

PARAMETERS
  subs_id      - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  Void* - pointer to the def bearer of the PDN of interest

SIDE EFFECTS
  None
===========================================================================*/
void* ds_3gpp_pdn_cntx_get_bearer_for_pdn_in_going_down
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_ANY_PDN_PENDING_REDIAL

DESCRIPTION
  This function returns whether there is a pdn pending redial

PARAMETERS
  

DEPENDENCIES
  None

RETURN VALUE
  boolean - whether there is a pdn context pending redial

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_any_pdn_pending_redial
(
  sys_modem_as_id_e_type    subs_id 
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_ANY_PDN_AWAITING_BEARER_ALLOCATION

DESCRIPTION
  This function returns the status, if  there is a pdn waiting for bearer
  resource allocation

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If there is atleast one PDN in AWAITING_BEARER_ALLOC state
  FALSE - otherwise
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_any_pdn_awaiting_bearer_allocation
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_GET_PDN_CNTXT_AWAITING_BEARER_ALLOCATION

DESCRIPTION
  This function returns the PDN context waiting for bearer resource
  allocation

PARAMETERS
  

DEPENDENCIES
  None

RETURN VALUE
  ds_pdn_context_s - If there is a PDN waiting for bearer allocation
  NULL             - otherwise

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_context_s* ds_3gpp_get_pdn_cntxt_awaiting_bearer_allocation
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_BEARER_TECH_TYPE

DESCRIPTION 
  This function translates CM mode to one of the IFACE tech
  type enum values.
  
PARAMETERS
  mode        - mode defined in CM
  bearer_tech - structure to be populated
  subs_id     - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_get_bearer_tech_type
(
  sys_sys_mode_e_type               mode,
  ps_iface_bearer_technology_type  *bearer_tech,
  sys_modem_as_id_e_type            subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_POST_IFACE_GENERIC_IND

DESCRIPTION
  This function posts IFACE generic indication to all valid
  Iface pointers.

PARAMETERS
  event -- IFACE Event that needs to be posted.
  event_data_ptr -- data associated with this event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_post_iface_generic_ind
(
  ps_iface_event_enum_type             event,
  void                                *event_data_ptr,
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_POST_IFACE_GENERIC_IND_FOR_PDN

DESCRIPTION
  This function posts IFACE generic indication to all valid
  Iface pointers for a single PDN context.

PARAMETERS
  event -- IFACE Event that needs to be posted.
  event_data_ptr -- data associated with this event.
  pdn_cntxt_p  -- PDN context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_post_iface_generic_ind_for_pdn
(
  ds_pdn_context_s                   * pdn_cntxt_p,
  ps_iface_event_enum_type             event,
  void                                *event_data_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_RESUME_IND_HDLR

DESCRIPTION
  This function is used to perform any action at the PDN context level when a
  resume indication is received.

  This function in turn invokes the mode specific resume indication handler.

PARAMETERS
  subs_id      - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_resume_ind_hdlr
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_GET_SESSION_PARAMS

DESCRIPTION
  Function to retrieve the profile params related to a call from the
  iface table and call info table.

PARAMETERS
  iface_ptr   - Ptr to ps iface inst    
  session_ptr - Pointer to stiore the session params
  ps_errno    - Error information

DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
       DS_EFAULT   if NULL pointers passed.
   
SIDE EFFECTS
  None
===========================================================================*/
int ds_3gpp_pdn_cntxt_get_session_params
(
  ps_iface_type                                 *iface_ptr,
                                                  /* Ptr to ps iface inst    */
  ps_iface_ioctl_3gpp_session_params_info_type  *session_ptr,
                                            /* Pointer to store session data */
  sint15                                        *ps_errno
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_GET_DEF_PDN

DESCRIPTION
  This function is used to get to the default PDN context.

PARAMETERS
  subs_id      - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  ds_pdn_context_s* - Pointer to the default PDN context irrespective of the
                      state of the PDN. Never returns NULL
   
SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_context_s* ds_3gpp_pdn_cntxt_get_def_pdn
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_DECODE_APN_NAME_NON_STRICT

DESCRIPTION
  This function is used to decode the APN name returned by the network. This
  function leaves out the operator identified. It picks up only the labels
  that reflect the network identifier. This does not check if the APN string
  has more than DS_3GPP_OP_IDENTIFIER_LABEL_COUNT labels.

PARAMETERS
  IN
  encoded_apn - Pointer to the APN returned by the network. This is encoded
  encoded_apn_length - length of the encoded string

  OUT
  decoded_apn - Pointer to the byte array in which the decoded APN is returned

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if decoding is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_context_decode_apn_name_non_strict
(
  byte*  encoded_apn,
  uint32 encoded_apn_length,
  byte*  decoded_apn
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntx_update_pco

DESCRIPTION
  This function is used to update PCO. This inturn invokes
  ds_pdn_cntxt_decodeProtocolParams to perform the actual update.
PARAMETERS
    call_id        - Call id of the call in progress
   *mode_info_ptr  - Pointer to the mode-specific call information provided
                     by Call Manager
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
ds_3gpp_pdn_cntx_update_pco
(
  cm_call_mode_info_s_type *mode_info_ptr, 
  cm_call_id_type          call_id
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_tearDownDSIface

DESCRIPTION
  This function tears down the DS iface. This function also tears down the
  corresponding PS iface and disassociates the DS iface from the PDN cntxt.

PARAMETERS
  ds_ifaced_p - pointer to the DS iface to be torn down
  down_reason - reason for tearing down the iface - will be passed to the
                PS iface

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS 
Disassociation of DS Iface and PDN context. 
===========================================================================*/
void ds_3gpp_pdn_cntxt_tearDownDSIface
(
  ds_3gpp_iface_s *ds_iface_p,
  ps_iface_net_down_reason_type down_reason
);

/*===========================================================================
FUNCTION dsPdnContext_is_any_other_pdn_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) in UP state. Whether the PDN passed in is 
  UP or not, does not matter.
 
  This function does not take emergency PDNs into consideration.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_other_normal_pdn_active(ds_pdn_context_s *pdn_cntx_p);

/*===========================================================================
FUNCTION dsPdnContext_is_any_other_normal_pdn_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) in UP state. Whether the PDN passed in is 
  UP or not, does not matter.
 
  This function does not take emergency PDNs into consideration.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN in UP state, other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_other_normal_pdn_up
(
  ds_pdn_context_s *pdn_cntx_p,
  sys_modem_as_id_e_type  subs_id
);
/*===========================================================================
FUNCTION dsPdnContext_is_any_normal_pdn_coming_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN are in coming UP state.
  This function does not take emergency PDNs into consideration.
 
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS   
===========================================================================*/
boolean dsPdnContext_is_any_normal_pdn_coming_up(void );

/*===========================================================================
FUNCTION dsPdnContext_is_any_normal_pdn_coming_up_ex

DESCRIPTION
  This function loops over all PDN contexts for the passed subs_id to see if there is atleast one more 
  PDN are in coming UP state.
  This function does not take emergency PDNs into consideration.
 
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_normal_pdn_coming_up_ex
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_freeInstance

DESCRIPTION
  This function frees the pdn context.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cntxt_freeInstance
(
  ds_pdn_context_s    *pdn_cntx_p          /* Unique PDN context instance  */  
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_context_validate_PCSCF_address

DESCRIPTION
  This function validates if the network returned PCSCF address is consistent
  with the rules specified in the APM.
 
  If the rules defined in APM are not met by the Ifaces in this PDN context;
  a command is queued to tear down the PDN connection.

PARAMETERS 
 
  pdn_context_p (IN) :  PDN context object


DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if PCSCF addreess is valid according to the rules
            FALSE if  PCSCF adderess is not valid according to the rules
   
SIDE EFFECTS
  None
===========================================================================*/

boolean ds_3gpp_pdn_context_validate_PCSCF_address
(
  ds_pdn_context_s          *pdn_context_p,
  boolean                   *v4_or_v6_pcscf_is_present
);

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPV4V6_LINK_MTU_CONTAINER

DESCRIPTION
  This function fills the IPV4V6 mtu PCO container
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
    *profile_p                 Ptr to ds profile
    subs_id                    Subscription Id
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_ipv4v6_link_mtu_container
(
  protocol_cfg_options_T    *protocol_config_options_p,
  ds_umts_pdp_profile_type  *profile_p,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPV4V6_LOCAL_ADDRESS_SUPPORT_CONTAINER

DESCRIPTION
  This function fills the local address support in TFT indicator
  
PARAMETERS 
    protocol_cfg_options_T - *protocol_config_options_p 
    *profile_p             - Ptr to ds profile
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_ipv4v6_local_address_support_container
(
  protocol_cfg_options_T    *protocol_config_options_p,
  ds_umts_pdp_profile_type  *profile_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_VALIDATE_PDN_CONTEXT_INTERNAL

DESCRIPTION
  This function checks if the pdn context tbl has been allocated dynamic memory

PARAMETERS
  pdn_context_p       - pdn_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_cntx_validate_pdn_context_internal
(
  ds_pdn_context_s *pdn_context_p
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntx_validate_pdn_context

DESCRIPTION
  This function checks if the pdn context tbl has been allocated dynamic memory

PARAMETERS
  pdn_context_p       - pdn_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_cntx_validate_pdn_context
(
  ds_pdn_context_s *pdn_context_p
);

#ifdef FEATURE_DATA_LTE


/*===========================================================================
FUNCTION      DS_PDN_CNTX_IS_APM_REJECT_SM_THROTTLED

DESCRIPTION
  This function checks to see if apn_reject_sm for the particular PDN
  is blocked.
  However if it is a HO attach scenario, then no throttling will be performed.
 
PARAMETERS

DEPENDENCIES

RETURN VALUE
  True -  If the apn_reject_sm is blocked
  False - Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_is_apn_reject_sm_throttled
(
  ds_pdn_context_s     *pdn_cntx_p  
);
#endif

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntx_free_dyn_mem

DESCRIPTION
  This function frees dyn memory.
 
PARAMETERS 
  ds_pdn_context_s* pdn_context_p 
  subs_id           Subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  Void 
 
SIDE EFFECTS
  None

=============================================================================*/
void ds_3gpp_pdn_cntx_free_dyn_mem
(
   ds_pdn_context_s*      pdn_context_p,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_pdn_cntxt_get_companion_iface_for_emergency_calls

DESCRIPTION
  1 v4 Iface and 1 v6 Iface are always reserved for making emergency calls.
  This function returns the companion Iface 

PARAMETERS   
  ds_iface_s - The passed in Iface whose companion will be returned.
    
DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_iface_s* - Pointer to the companion Iface.
  NULL - If this is the passed in Iface is not the Iface reserved to make
         emergency calls.

SIDE EFFECTS
  None

===========================================================================*/
ds_3gpp_iface_s* ds_pdn_cntxt_get_companion_iface_for_emergency_calls
(
  ds_3gpp_iface_s *ds_iface_s
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_HANDLE_EMERGENCY_ATTACH_STATUS

DESCRIPTION
  This function send event to ps whether emergency pdn is only pdn active 
  or not
 
PARAMETERS
  pdn_cntx_p - ds_pdn_context_s 

DEPENDENCIES
  None.

RETURN VALUE  
  none

SIDE EFFECTS  
===========================================================================*/

void ds_3gpp_pdn_cntxt_handle_emergency_attach_status
(
  ds_pdn_context_s    *pdn_cntx_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_ONLY_EMERGENCY_PDN_IS_UP

DESCRIPTION
  This function loops over all PDN contexts to see if a data call to the emergency
  APN is active.
 
PARAMETERS
  subs_id 

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, If a only Emergency APN is up.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean ds_3gpp_pdn_context_only_emergency_pdn_is_up
(
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_context_emergency_pdn_is_up

DESCRIPTION
  This function loops over all PDN contexts to see if a data call to the 
  emergency APN is active.
 
PARAMETERS
  boolean      - coming_up, to check whether emrgency PDN is in 
                 coming_up state
  subs_id      - Subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, If a call to the Emergency APN is up or coming_up 
        (based on coming_up flag).
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean ds_3gpp_pdn_context_emergency_pdn_is_up
(
  boolean                coming_up,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_TEAR_DOWN

DESCRIPTION
  This function when invoked tears down all pdn connections which does not
  have active clients

PARAMETERS
  subs_id      - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_cntxt_tear_down
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_REMOVE_UNUSED_PDN

DESCRIPTION
  This function when invoked tears down all pdn connections which does not
  have active clients ,this function is invoked during bringup of another pdn

PARAMETERS
  subs_id      - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_remove_unused_pdn
(
  ds_pdn_context_s *pdn_cntx_p
);

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DETACH_AND_BLOCK_PLMN_IF_ATTACH_PROFILE_UNAVAILABLE
DESCRIPTION
  This function disables lte if there are no active bearers and all the attach apn's
  are blocked.we check both case where attach profile list is configured and not configured.
PARAMETERS  
  None.
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_detach_and_block_plmn_if_attach_profile_unavailable
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DISABLE_LTE_IF_ATTACH_PROFILE_UNAVAILABLE
DESCRIPTION
  This function disables lte if there are no active bearers and all the attach apn's
  are blocked.we check both case where attach profile list is configured and not configured.
PARAMETERS  
  None.
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable
(
   sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DETACH_AND_BLOCK_PLMN
DESCRIPTION
  This function function performs LTE detach and blocks the current PLMN.
PARAMETERS  
  Subscription Id
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_detach_and_block_plmn
(
  sys_modem_as_id_e_type    subs_id,
  boolean                   detach
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_BLOCK_PLMN
DESCRIPTION
  This function function blocks the current PLMN.
PARAMETERS  
  Subscription Id
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_block_plmn
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_GET_ENABLE_LTE_IF_ATTACH_PROFILE_AVAILABLE

DESCRIPTION
  This function enabled lte if attach apn's are available.
  we check both case where attach profile list is configured
  and not configured (default socket profile).
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
   
SIDE EFFECTS 
  None.
  
===========================================================================*/

void  ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_CHECK_IF_ATTACH_PROFILE_AVAILABLE
DESCRIPTION
  This function checks if Attach Profile is Available. 
  We check both case where attach profile list is configured and not configured.
PARAMETERS  
  Subscription Id
DEPENDENCIES 
  None.  
RETURN VALUE 
  DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN - If Attach profile status is unknown.
  DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL - If Attach profile is available
  DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL - If Attach profile is not available
SIDE EFFECTS 
  None.
===========================================================================*/
ds_pdn_cntxt_attach_profile_status_type
  ds_3gpp_pdn_cntxt_check_if_attach_profile_available
(
  sys_modem_as_id_e_type    subs_id
);
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FLUSH_PLMN_FROM_LIST

DESCRIPTION
  This function frees the PLMN specified in the blocking List.
PARAMETERS 
 sys_plmn_id_s_type unblock_plmn_id  - UNBLOCK PLMN ID
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.
SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_flush_plmn_from_list
(
  sys_plmn_id_s_type unblock_plmn_id,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_ENABLE_LTE_EACH_PLMNID

DESCRIPTION
  This function enabled lte on each blocked plmn
  if atleast one attach apn are available. on the blocked plmn list

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  ds_3gpp_pdn_cntxt_enable_lte_each_plmnid
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_IF_ATTACH_PROFILE_AVAILABLE_PLMN_ID

DESCRIPTION
  This function checks if the attach profile is available on the passed plmnid and attach list combination

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_pdn_cntxt_check_if_attach_profile_available_plmn_id
(
  sys_modem_as_id_e_type                       subs_id,
  sys_plmn_id_s_type                           plmn_id,
  ps_sys_3gpp_lte_attach_pdn_list_type  attach_list_ptr
);

#endif

/*===========================================================================
  FUNCTION DS_3GPP_IS_CONTEXT_PREF_PKT_MODE
  
  DESCRIPTION
    Checks if the current passed system mode is the preferred packet mode for
    the given pdn context
    
  PARAMETERS  
    pdn_context_p     :           Current pdn context
    mode              :           Mode to be checked for preference
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    TRUE  :  When the preferred mode is same as the mode passed in
    FALSE :  WHen the preferred mode does not match the passed in mode
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/

boolean ds_3gpp_is_context_pref_pkt_mode
(
  ds_pdn_context_s              * pdn_context_p,
  sys_sys_mode_e_type             mode
);

/*===========================================================================
  FUNCTION DS_3GPP_GET_APN_CURRENT_MODE
  
  DESCRIPTION
    This function extracts the APN from current PDN context and queries
    DSD/MAPCON to get the current mode for the APN
    
  PARAMETERS  
    pdn_context_p     :           Current pdn context
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE
  Current(preferred) sys sys mode
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
sys_sys_mode_e_type ds_3gpp_get_apn_current_mode
(
  ds_pdn_context_s       * pdn_context_p
);


/*===========================================================================
FUNCTION      DS_3GPP_GET_APN_FROM_PDN_CONTEXT

DESCRIPTION   Fills the apn details in the pointer passed for a given PDN 
              Context
 
PARAMETERS    pdn_context_p 
              apn_name  
 
DEPENDENCIES  None.

RETURN VALUE  TRUE: if APN was successfully extracted from PDN context 
              FALSE: if PDN context or its contents were not valid 

SIDE EFFECTS  None.
===========================================================================*/

boolean ds_3gpp_get_apn_from_pdn_context
(
  ds_pdn_context_s            * pdn_context_p,
  ds3g_apn_name_type          * apn_name
);

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CONTEXT_GET_COMPANION_IFACE

DESCRIPTION
  Fetches the other Iface from the current Iface pointer.

PARAMETERS
  iface_p       - Pointer to the current iface.  

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  companion_iface_p - Pointer to the other iface.  

SIDE EFFECTS
  None.
===========================================================================*/
ps_iface_type* ds_3gpp_pdn_get_companion_iface
(
  ps_iface_type *iface_p
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_active_pdns

DESCRIPTION
  This function checks if any pdn context is active and UP for the apn passed

PARAMETERS
  char *apn,
  ds_pdn_context_s  **pdn_cntxt_ptrs  
 
DEPENDENCIES
  None.

RETURN VALUE 
 .void

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_get_active_pdns
(
  char *apn,
  ds_pdn_context_s  **pdn_cntxt_ptrs
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_PREPARE_FOR_REDIAL

DESCRIPTION
  This function tears down the bearer and puts the bearer and PDN context
  in an appropriate state to redial

PARAMETERS 
  bearer_cntxt_p - Bearer on which redial is needed.
  redial_cause - Why a redial needs to happen
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_prepare_for_redial
(
   void *bearer_context_p,
   ds_3gpp_redial_e_type redial_cause
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_ROAMING_STATUS_CHG_CMD_HDLR

DESCRIPTION
  This function handles roaming status chg

PARAMETERS 
  roaming status
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_roaming_status_chg_cmd_hdlr
(
    sys_roam_status_e_type roam_status,
    sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal

DESCRIPTION
  This function gets the Subscription id from PDN pointer

PARAMETERS
  pdn_ptr- PDN pointer
  Subs_id_ptr - pointer to subs_id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE-- if successful
  FALSE-- inavalid PDN pointer/invalid SUBS_ID

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal
(
  ds_pdn_context_s* pdn_cntxt_ptr,
  sys_modem_as_id_e_type* subs_id_ptr
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_subs_id_from_pdn

DESCRIPTION
  This function gets the Subscription id from PDN pointer

PARAMETERS
  pdn_ptr- PDN pointer
  Subs_id_ptr - pointer to subs_id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE-- if successful
  FALSE-- inavalid PDN pointer/invalid SUBS_ID

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_subs_id_from_pdn
(
  ds_pdn_context_s* pdn_cntxt_ptr,
  sys_modem_as_id_e_type* subs_id_ptr
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_set_sub_id

DESCRIPTION
  This function sets the Subscription ID for the PDN which remain constant
  until the PDN is torn down

PARAMETERS
  ds_pdn_context_s           pdn_cntx_p, // PDN context
  sys_modem_as_id_e_type     subs_id     // Subscription ID

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE  = SUCCESS
  FALSE = FAILURE

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_set_sub_id
(
   ds_pdn_context_s          *pdn_cntx_p,
   sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_local_teardown

DESCRIPTION
  This function is used to teardown pending calls when DDS switch occurs
 
PARAMETERS
  void
 
DEPENDENCIES
  void

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_local_teardown
(
  boolean                cleanup_all_calls,
  sys_modem_as_id_e_type subs_id
);

  /*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_dds_switch_cb

DESCRIPTION
  This function is used as Callback when DDS switch occurs
 
PARAMETERS
  void
 
DEPENDENCIES
  void

RETURN VALUE  
  boolean TRUE  : Valid
          FALSE : In Valid

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_dds_switch_cb
(
  ds3gsubsmgr_event_type       event_id,
  ds3gsubsmgr_event_info_type *event_info_ptr,
  void                        *data_ptr
);
/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_handle_lpm_mode

DESCRIPTION
  This function is register as call back to DDS switch and is used to clean up 
  calls
 
PARAMETERS
  void
 
DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_handle_lpm_mode
(
  ds_3gpp_event_info_u_type event_info,
  uint32                    user_data
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_subs_id_from_pdn

DESCRIPTION
  This function gets the Subscription id from PDN pointer

PARAMETERS
  pdn_cntxt_ptr - PDN ptr
  Subs_id--susscription ID
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE- if successfull
  FALSE- invalid PDN ptr/invalid SUBS_ID

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_subsmgr_event_cb
(
  ds3gsubsmgr_event_type       event_id,
  ds3gsubsmgr_event_info_type *event_info_ptr,
  void                        *data_ptr
);
/*===========================================================================
FUNCTION  DS_3GPP_REPORT_ALL_IFACE_THROUGHPUT_INFO

DESCRIPTION 
 This Utility function reports the the throughput Information for all
 active DS 3GPP ifaces

PARAMETERS 
  sys_modem_as_id_e_type            - subscription ID to report per iface
                                      bearer stats
  ps_sys_throughput_info_list_type  - Pointer to the iface information
                                      list to fill in iface throughput
                                      related stats
  uint16                            - maximum number if ds ifaces that
                                      will be reported in the list

DEPENDENCIES 
  This utility function will be used only when
  * there is atleast one active DS 3GPP Iface
        &
  * Throughput Info Indication timer is running

RETURN VALUE
  TRUE   - If the throughput Info for the iface is reported successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds_3gpp_report_all_iface_throughput_info
(
  sys_modem_as_id_e_type              cm_subs_id,
  ps_sys_throughput_info_list_type   *iface_info_list_p,
  uint16                              num_max_ifaces
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_DISABLE_LTE_PER_PLMN_SELECTION_EX

DESCRIPTION
  This function disables LTE per PLMN for the given PLMN Id. It also adds
  the PLMN to blocked PLMN List.

PARAMETERS
  PLMN Id
  Timer Value for which PLMN will be blocked
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PLMN Disabled successfully
  FALSE, if PLMN Disable failed or PLMN already disabled

SIDE EFFECTS
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_disable_lte_per_plmn_selection_ex
(
   sys_plmn_id_s_type      plmn_id,
   uint32                  timer_val,
   sys_modem_as_id_e_type  subs_id,
   boolean                 detach
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_LTE_UNBLOCK_ALL_PLMNS

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns
(
  sys_modem_as_id_e_type  subs_id
);
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_PLMN_BLOCKED

DESCRIPTION
  This function is used to check if the given PLMN is blocked

PARAMETERS
  PLMN Id
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PLMN is blocked
  FALSE, otherwise

SIDE EFFECTS
None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_plmn_blocked
(
  sys_plmn_id_s_type	 plmn_id,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FLUSH_PLMN_FROM_LIST

DESCRIPTION
  This function frees the PLMN specified in the blocking List.

PARAMETERS 
 sys_plmn_id_s_type unblock_plmn_id  - UNBLOCK PLMN ID
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_flush_plmn_from_list
(
  sys_plmn_id_s_type unblock_plmn_id,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FLUSH_BLOCKED_PLMN_LIST

DESCRIPTION
  This function frees the Blocked PLMN List.
  If unblock_plmn is set, It will communicate the same to NAS

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_flush_blocked_plmn_list
(
    sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_TEARDOWN_PDN_PER_PROFILE

DESCRIPTION
  This function tears down the PDN Connections
  using the passed profile number.

PARAMETERS
 Profile Number
 DOWN Reason to be given to Client Applications

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN was torn down successfully
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntx_teardown_pdn_per_profile
( 
  uint8                         profile_num,
  ps_iface_net_down_reason_type tear_down_reason
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_TEARDOWN_APN

DESCRIPTION
  This function tears down the PDN Connections
  using the given APN Name

PARAMETERS
  APN Name
  Subs Id
  DOWN Reason to be given to Client Applications

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN was torn down successfully
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntx_teardown_apn
( 
  char                          *apn,
  sys_modem_as_id_e_type         subs_id,
  ps_iface_net_down_reason_type  tear_down_reason
);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_read_redial_no_cause_code_from_nv

DESCRIPTION
  This function gets the ds 3gpp redial for no cause from NV item
  NV item : 72563
 
PARAMETERS
  None : None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_read_redial_no_cause_code_from_nv(void);

/*===========================================================================
FUNCTION DS_3GPP_APN_PARAM_CHANGE_CMD_HDLR

DESCRIPTION
  This function detects if any of following profile params has changed or not
  1.  APN disabled flag
  2.  APN Name
  3.  Maximum PDN connections per block
  4.  Maximum PDN connection wait timer
  5.  PDN request wait timer
  6.  APN bearer
  7.  APN class
  8.  PDP Type
 
  If any of those above params have changed then it calls another which handles
  PDN disconnect processing if any PDN is up with the changed profile
 
PARAMETERS 
  prof_num:                 Profile whose contents have changed
  subs_id                   Profile Subscription ID
  prf_data:                 Profile specific data (mask, new values ....)
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_apn_param_change_cmd_hdlr
(
  uint16                           prof_num,
  sys_modem_as_id_e_type           subs_id,
  void                            *prf_data
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_PREPARE_IFACE_STATE_AND_TIMERS_FOR_REDIAL

DESCRIPTION
  This function puts the ifaces in configuring state and posts an event
  to stop IP6 state machine.
  It also stops the MCS timer maintained per bearer

PARAMETERS 
  bearer_cntxt_p
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_prepare_iface_state_and_timers_for_redial
(
   void  *bearer_context_ptr
);


/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_compare_pdp_address

DESCRIPTION
  This function compares the pdp_address passed by network with the address
  set on the PDN during call origination.
 
PARAMETERS
  identifier: for W/G/TD its nsapi, for LTE its eps_bid
  sys_mode: current sys_mode.
  subs_id:  subscription id of the call.
  pdp_address * : pointer to the pdp_address struct passed by nas

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE if address is same.
  FALSE if address is different.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_compare_pdp_address
(
  uint8 identifier,
  sys_sys_mode_e_type sys_mode,
  sys_modem_as_id_e_type subs_id,
  pdp_address_T *pdp_address
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_PDN_CONTEXT_FROM_CID

DESCRIPTION
  Retrieve the PDN context from context id.

PARAMETERS
  cid              - context id.
.
DEPENDENCIES
  None.

RETURN VALUE
  NULL          - Returns NULL if no valid pdn context pointers
                else valid pdn_context_pointer.
  
SIDE EFFECTS
  None.

===========================================================================*/

ds_pdn_context_s* ds_pdn_cntxt_get_pdn_context_from_cid
( 
  uint32                   cid,
  sys_modem_as_id_e_type   subs_id 
);

/*===========================================================================
FUNCTION DS_PDN_CNTXT_VALIDATE_PDN_SUBSCRIPTION_INTERNAL

DESCRIPTION
  This function validates whether the PDN Context belong to the
  specified subscription id. This is for checking valid, no F3 if invalid.

PARAMETERS
  pdn_context_p: PDN Context Pointer
  subs_id:       Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN Context belong to the specified subscription id.
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_validate_pdn_subscription_internal
(
  ds_pdn_context_s        *pdn_context_p,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DS_PDN_CNTXT_VALIDATE_PDN_SUBSCRIPTION

DESCRIPTION
  This function validates whether the PDN Context belong to the
  specified subscription id.

PARAMETERS
  pdn_context_p: PDN Context Pointer
  subs_id:       Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN Context belong to the specified subscription id.
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_validate_pdn_subscription
(
  ds_pdn_context_s        *pdn_context_p,
  sys_modem_as_id_e_type   subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_SET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp contexts for the current 
  pdn connection.

PARAMETERS
  max_active_pdp_context
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_set_max_simul_active_pdp_context
(
  uint8 max_active_pdp_context,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PS_IFACE_V4_PARAMS 
 
DESCRIPTION
  This function updates IPV4 param in PS IFace structure with Primary DNS,
  secondary DNS and Gateway Address

PARAMETERS 
  Iface_ptr       : PS IFACE ptr
  decode_ps_param : IPCP option parameters
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  return TRUE:  Successfully updated PS IFACE structure
  return FALSE: UnSuccessful in updating PS IFACE structure
 
SIDE EFFECTS 
  None 
===========================================================================*/
boolean ds_3gpp_update_ps_iface_v4_params
(
  ps_iface_type           *iface_ptr,
  dsumtsps_ipcp4_options   decode_ps_param
);

/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PS_IFACE_V6_PARAMS 
 
DESCRIPTION
  This function updates IPV6 param in PS IFace structure with Primary DNS,
  secondary DNS and Gateway Address

PARAMETERS 
  Iface_ptr       : PS IFACE ptr
  decode_ps_param : IPCP option parameters
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  return TRUE:  Successfully updated PS IFACE structure
  return FALSE: UnSuccessful in updating PS IFACE structure
 
SIDE EFFECTS 
  None 
===========================================================================*/
boolean ds_3gpp_update_ps_iface_v6_params
(
  ps_iface_type           *iface_ptr,
  dsumtsps_ipv6cp_options  decode_ps_param
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_GET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function returns the number of active pdp contexts for the current pdn connection..

PARAMETERS
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  Number of active pdp contexts.

SIDE EFFECTS
  None

===========================================================================*/
uint8 ds_3gpp_pdn_get_max_simul_active_pdp_context
(
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_GET_CURRENT_ACTIVE_CONTEXT

DESCRIPTION
  This function returns the number of active bearer contexts. 

PARAMETERS
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  Number of active bearer contexts.

SIDE EFFECTS
 None.
===========================================================================*/
uint8 ds_3gpp_pdn_get_current_active_context
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_GET_IPV6_PREFIX_DELEGATION_MODE
 
DESCRIPTION
  This function goes through the Profile cache to check if the changed apn is
  present in any of the other profiles.
  Return true if it is present in any other profiles
  Return false if it is not present in any other profiles.

PARAMETERS 
  profile_num
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_3gpp_get_ipv6_prefix_delegation_mode
(
   ps_iface_type * ps_iface_p
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION  DSPDNCNTX_IS_LAST_PREVAILING_PDN_IN_LTE

DESCRIPTION
  Indicates if the passed PDN context is the only prevailing one
  in LTE mode. 
  
PARAMETERS
  pdn_cntx_ptr

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  True - If it is the last prevailing PDN
  False - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntx_is_last_prevailing_pdn_in_lte
(
  ds_pdn_context_s *pdn_cntx_p 
);
#endif
/*===========================================================================
  FUNCTION DS_3GPP_PDN_CNTX_PDP_TYPE_CHG_CMD_HDLR

  DESCRIPTION
  This function handles the pdp type change, It extracts the apn name
  for the profile number passed and clears the network override handler
  
  PARAMETERS
  1. Cmd Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_cntx_pdp_type_chg_cmd_hdlr
(
  const ds_cmd_type      *cmd_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_GET_EMERG_PDN_CNTXT

DESCRIPTION 
  This function loops through the PDN context table and
  returns the emergency PDN context
 
PARAMETERS
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  ds_pdn_context_s* - Pointer to the emergency PDN context
  NULL              - If no Emergency PDN context found 
   
SIDE EFFECTS 
  None.
===========================================================================*/
ds_pdn_context_s* ds_3gpp_get_emerg_pdn_cntxt(void);

/*===========================================================================
FUNCTION DS_PDN_CNTXT_CHECK_OP_PCO_COLLISION

DESCRIPTION
  This function checks if the op_res_pco id is configured to be the
  same in profiles and from the app.
    
PARAMETERS   :
    profile_p - Ptr to the profile info
    void        user_data_to_query_pco 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_pdn_cntxt_check_op_pco_collision
(
  ds_umts_pdp_profile_type     *profile_p,
  void                         *user_data_to_query_pco
);

#ifdef FEATURE_EMERGENCY_PDN
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SET_PENDING_TEARDOWN_FLAG 
 
DESCRIPTION
  This function set boolean pending teardown flag in pdn_cntxt for all
  non-emergency COMING UP call , UP call and Pending Redial calls

PARAMETERS 
  None
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  
===========================================================================*/
void ds_3gpp_pdn_cntxt_set_pending_teardown_flag(void);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_CHECK_AND_TEARDOWN_NON_EMERGENCY_PDNS
 
DESCRIPTION
  This function checks if there are any calls in coming up state
  If YES: Do not perform any action
  If NO :
        If Auth fail occured :
              Teardown all calls in UP state which have emerg_auth_fail
              flag set in the PDN
  This Function is used to teardown all active calls after Emergency call was
  made with authentication failure.DS allows all the active UP and COMING UP
  calls to be brought up completely and then be torn down
   
PARAMETERS 
  None
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
 None
 
SIDE EFFECTS 
  
===========================================================================*/
void ds_3gpp_pdn_cntxt_check_and_teardown_non_emergency_pdns(void);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_CHECK_NON_EMERGENCY_PDN_TEARDOWN
 
DESCRIPTION
  This function checks if there are any calls in coming up state which have
  pending_teardown_pdn flag set

PARAMETERS 
  None
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean : TRUE : Teardown non emergency PDN 
          : FALSE: No action
 
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_check_non_emergency_pdn_teardown(void);

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_CHECK_IF_ATTACH_PROFILE_AVAILABLE
DESCRIPTION
  This function checks if Attach Profile is Available. 
  We check both case where attach profile list is configured and not configured.
PARAMETERS  
  Subscription Id
DEPENDENCIES 
  None.  
RETURN VALUE 
  DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN - If Attach profile status is unknown.
  DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL - If Attach profile is available
  DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL - If Attach profile is not available
SIDE EFFECTS 
  None.
===========================================================================*/
  ds_pdn_cntxt_attach_profile_status_type
  ds_3gpp_pdn_cntxt_check_if_attach_profile_available
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_PENDING_TEARDOWN_FLAG
 
DESCRIPTION
  This function gets the pending teardown flag from PDN being passed
   
PARAMETERS 
  pdn_cntxt_p : PDN context pointer
             
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean : TRUE : Pending Teardown flag is set  
          : FALSE: Pending Teardown flag is not set
 
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_pending_teardown_flag
(
  ds_pdn_context_s    *pdn_cntxt_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_EMERGENCY_AUTH_FAIL

DESCRIPTION
  This function gets the variable ds_3gpp_emergency_auth_fail
 
PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  Boolean : ds_3gpp_emergency_auth_fail : TRUE: Authentication failure on Emerg call
                                          FALSE: No Authentication Ffailure
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_emergency_auth_fail(void);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SET_EMERGENCY_AUTH_FAIL

DESCRIPTION
  This function sets the variable ds_3gpp_emergency_auth_fail to variable 
  being passed
 
PARAMETERS 
  Boolean Value: ds_3gpp_emergency_auth_fail gets updated to value being passed
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_set_emergency_auth_fail
(
  boolean value
);

#endif /*FEATURE_EMERGENCY_PDN*/

#ifdef FEATURE_UE_INITIATED_DORMANCY 
/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_alloc_memory_for_delay_dormancy_stats

DESCRIPTION
  This function allocated dynamic memory required for for delay 
   dormancy algorithm

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to dynamic memoryE

SIDE EFFECTS

===========================================================================*/
ds_3gpp_delay_dormancy_s* ds_3gpp_pdn_cntxt_alloc_memory_for_delay_dormancy_stats(void);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_free_memory_for_delay_dormancy_stats

DESCRIPTION
  This function frees the allocated dynamic memory for for delay 
   dormancy algorithm

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_free_memory_for_delay_dormancy_stats(void);

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr

DESCRIPTION
  This function gets the dynamic pointer allocated for stats

PARAMETERS
  adress of variable
  
DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to dynamic memory

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr
(
  ds_3gpp_delay_dormancy_s** info_p
);


/*===========================================================================
 DS_3GPP_BEARER_CNTXT_INIT_OR_CHECK_CURRENT_DATA_STATISTICS
  
 DESCRIPTION
  This function caches present IFACE statistics or checks for data activity 
  based on input operation type given.
  
 PARAMETERS
  operation_type: to Cache/Check data activity
 DEPENDENCIES
  None.
  
 RETURN VALUE
TRUE - for Cache operation
TRUE/FALSE - for Data activity check
  
 SIDE EFFECTS
  None.
  
 ===========================================================================*/
boolean ds_3gpp_bearer_cntxt_init_or_check_data_statistics
(
  ds_3gpp_operation_type_e op_type
);
#endif /* FEATURE_UE_INITIATED_DORMANCY */
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_ANY_PDN_GOING_DOWN

DESCRIPTION
  This function returns whether there is a pdn in going down

PARAMETERS
  

DEPENDENCIES
  None

RETURN VALUE
  boolean - whether there is a pdn context in going_down state

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_any_pdn_going_down
( 
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_GET_TH_MIT_PDN_STATE

DESCRIPTION
  This function returns the 3GPP thermal mitigation pdn state

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
   ds_3gpp_th_mit_state - 3GPP thermal mitigation pdn state

SIDE EFFECTS
  None.

===========================================================================*/
ds3geventmgr_th_mit_action_type ds_3gpp_pdn_cntxt_get_th_mit_pdn_state(void);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_SET_TH_MIT_PDN_STATE

DESCRIPTION
  This function sets the 3GPP thermal mitigation pdn state

PARAMETERS
  th_mit_action - thermal mitigation action to be set            
 
DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_set_th_mit_pdn_state
(
  ds3geventmgr_th_mit_action_type th_mit_state
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_IS_PDN_THIRD_PARTY_IMS

DESCRIPTION
  This function checks to see if the PDN is a 3rd party IMS PDN by matching
  the APN name with the APN in the device config for IMS

PARAMETERS
  pdn_cntxt_p - PDN context pointer
  subs_id - subscription for the PDN
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE - PDN is 3rd party IMS PDN
  FALSE - PDN is NOT 3rd part IMS PDN

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_pdn_third_party_ims
(
  ds_pdn_context_s       *pdn_cntxt_p,
  sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_APN_PARAM_CHG_TIMER_CB

DESCRIPTION
  This function is called when the APN_PARAM_CHGtimer expires. This call
  back function simply posts a command to DS. DS then processes the command
  and takes suitable action

PARAMETERS
  unsigned long - Call back data, points to the PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_apn_param_chg_timer_cb(unsigned long callback_data);
/*===========================================================================
FUNCTION dsPdnContext_is_any_other_normal_pdn_diff_profile_num_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) and not the same profile number in UP state.
  Whether the PDN passed in is UP or not, does not matter.
 
  This function does not take emergency PDNs into consideration.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_other_normal_pdn_diff_profile_num_active
(
  ds_pdn_context_s *pdn_cntx_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_DETERMINE_PDP_TYPE
DESCRIPTION
  This functions returns the PDP type to be used based on the current mode
  and roaming conditions per subscription.

PARAMETERS
  subs_id        - subscription id
  context_info_p - ptr to PDP context
  roaming_status - UE roaming status

DEPENDENCIES
  None.

RETURN VALUE
  ds_umts_pdp_type_enum_type - V4, V6 or V4V6

SIDE EFFECTS 
  None.
===========================================================================*/
ds_umts_pdp_type_enum_type ds_3gpp_pdn_context_determine_pdp_type
(
  sys_modem_as_id_e_type           subs_id,
  const ds_umts_pdp_context_type  *context_info_p,
  boolean                          roaming_status
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_CNTX_APN_NAME_CHG_CMD_HDLR

  DESCRIPTION
  This function handles the apn name change, It extracts the apn name
  for the profile number passed and sends both the old and new apn as
  an event to ds3gmgr
  
  PARAMETERS
  1. Cmd Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_cntx_apn_name_chg_cmd_hdlr
(
  const ds_cmd_type      *cmd_ptr
);

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTXT_VALIDATE_AUTH_PARAMS_AND_RESET_T3396_THROT

DESCRIPTION
  Check for username/password match - Call originated username/passwd is
  compared with pdn throttling config info, If they are not the same.
  throttling info is cleared.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.    
  info_ptr      - Call param info
  subs_id
 
DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  void

SIDE EFFECT 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_validate_auth_params_and_reset_t3396_throt
(
  ds_umts_pdp_profile_type        *pdp_profile,
  void                            *info_ptr,
  sys_modem_as_id_e_type           subs_id   
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_CURRENT_LTE_PLMN

DESCRIPTION
  This function fetches the current PLMN Id corresponding to the given Subs Id

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned
  subs_id     - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_get_current_lte_plmn
(
  sys_plmn_id_s_type*     plmn_ptr,
  sys_modem_as_id_e_type  subs_id
);

#ifdef FEATURE_EPC_HANDOFF
/*=========================================================================== 
FUNCTION DS_EPS_PDN_CNTXT_CALL_ALLOWED_ATTACH_EX_HDLR

DESCRIPTION
  This function is used to register the call allowed handler with the LTE
  Mode Handler. This API is called by EPC Mode Handler. 

PARAMETERS
  call_allowed_hdlr_f - Pointer to the call allowed handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_reg_call_allowed_ex_hdlr
(
  ds_3gpp_if_call_allowed_hdlr_ex_f_type call_allowed_hdlr_f
);

/*===========================================================================
FUNCTION ds_eps_pdn_cntxt_reg_attach_type_ex_hdlr

DESCRIPTION
  This function is used to register the Attach Type handler with the LTE
  Mode Handler. This API is called by EPC Mode Handler.
 
  LTE MH calls the registered function during the Attach process when it needs
  to find out the Attach Type (Initial Attach or Handover Attach)

PARAMETERS
  attach_type_hdlr_f - Pointer to the Attach Type handler.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_reg_attach_type_ex_hdlr
(
  ds_3gpp_if_call_allowed_hdlr_ex_f_type attach_type_hdlr_f
);

#endif /* FEATURE_EPC_HANDOFF */

#endif /* DS_3GPP_PDN_CONTEXT_H */

