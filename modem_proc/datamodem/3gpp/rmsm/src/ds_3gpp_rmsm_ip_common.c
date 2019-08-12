/*===========================================================================
                       D S _ 3 G P P _ R M S M _ I P _ C O M M O N. C

DESCRIPTION
  The Data Services UMTS RM State Machine for pdp-ip calls source file. 
  This state machine manages the Um and Rm interfaces for pdp-ip type 
  pkt data calls initiated from laptop or conneted terminals .
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/ds_3gpp_rmsm_ip_common.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/07/12    nd     SIO port not freed after DUN is ended in entitlement check.
06/19/12    nd     MT PDP pkt call.
02/21/12    vs     Code reviews comments for Ipv6 over PPP + Some cleanup
12/11/11    vs     Initial file.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "memory.h"
#include "err.h"
#include "msg.h"

#include "dstask.h"
#include "dsati.h"
#include "ps_iface.h"
#include "ds3gsiolib.h"
#include "ds_flow_control.h"
#include "dstaski.h"
#include "ps_iface.h"
#include "ps_in.h"
#include "ps_ppp_netmodel.h" 
#include "ps_ppp.h"
#include "ps_ppp_snoop.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_ppp_ipv6cp.h"
#include "ps_meta_info.h"
#include "ps_byte.h"
#include "ps_sys_event.h"
#include "ds_sys.h"
#include "ds_sys_event.h"

#ifdef FEATURE_GSM_DTM
#include "rr_ds_if.h"
#endif /* FEATURE_GSM_DTM */
#include "ds_3gppi_utils.h"
#include "dsumtspdpreg.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)
#include "dsumts_rmsm.h"
#include "dsumts_rmsmi.h"
#include "dsumts_rmsm_ip.h"
#include "dsumtspsqos.h"

#ifdef FEATURE_DATA_TE_MT_PDP
#include "dsatetsictab.h"
#include "dsumtspsmthdlr.h"
#endif /*FEATURE_DATA_TE_MT_PDP*/

#include "ds_3gpp_rmsm_ip_common.h"
#include "ds_3gpp_nv_manager.h"
#include "dsumts_rmsmi.h"
#include "ps_iface_addr_v6.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_rmsm_autoconfig_sm.h"
#include "ps_pppi.h"
#include "ds3gsiolib_ex.h"

#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_apn_table.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL DATA STRUCTURE DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

static boolean ds_3gpp_rmsm_ip_common_ipcp_creq_was_received 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
)
{
  boolean rval = FALSE;

  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  if (info_ptr->v4_info.ipcp_creq_recd == TRUE)
    rval = TRUE;

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  return rval;
}

#define IPCP_CREQ_RECD(info_ptr) \
  ds_3gpp_rmsm_ip_common_ipcp_creq_was_received(info_ptr)

static boolean ds_3gpp_rmsm_ip_common_ipv6cp_creq_was_received 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
)
{
  boolean rval = FALSE;

  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  if (info_ptr->v6_info.ipv6cp_creq_recd == TRUE)
    rval = TRUE;

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  return rval;
}

#define IPV6CP_CREQ_RECD(info_ptr) \
  ds_3gpp_rmsm_ip_common_ipv6cp_creq_was_received(info_ptr)

static void ds_3gpp_rmsm_ip_common_set_ipcp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  info_ptr->v4_info.ipcp_creq_recd = TRUE;

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 
}

static void ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  info_ptr->v4_info.ipcp_creq_recd = FALSE;

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 
}

static void ds_3gpp_rmsm_ip_common_set_ipv6cp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  info_ptr->v6_info.ipv6cp_creq_recd = TRUE;

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 
}

static void ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  info_ptr->v6_info.ipv6cp_creq_recd = FALSE;

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 
}

#ifdef FEATURE_DATA_TE_MT_PDP
LOCAL ds3g_siolib_func_tbl_type ds_3gpp_rmsm_ip_common_siolib_func_tbl =
{
   // at_escape_sequence_handler
   NULL,
   
   // dtr_changed_sig_handler 
   NULL,
   
   // dtr_changed_sig_handler_ampd0 
   NULL,
   
   // at_return_to_online_data_handler 
   NULL,
   
   // at_return_to_online_data_complete_handler 
   NULL,
   
   // return_to_online_cmd_mode_complete_handler                                        
   NULL,
   
   // auto_answer_handler
   dsumts_rmsm_mt_auto_ans_cb
};

#endif /*FEATURE_DATA_TE_MT_PDP*/

/*---------------------------------------------------------------------------
  IPCP response Timer  value: If we need to generate IPCP responses,
  this timeout value specifies how much delay should be introduced to cut
  down on the IPCP packets exchanged between TE & Mobile.
  Time is in Milli seconds : A value of 0 will result in the response being
  generated immediately (timer fires off immly)
  one second delay
---------------------------------------------------------------------------*/
#define DS_3GPP_RMSM_IP_COMMON_IPCP_RES_DELAY_TIME  1000   

#define DS_3GPP_RMSM_IP_COMMON_IPV6CP_RES_DELAY_TIME  1000 

/*---------------------------------------------------------------------------
  DNS addresses advertised in IPCP NAKs generated by the SM if 
  send_ipcp_naks_flg flag is set in the state machine. 
---------------------------------------------------------------------------*/
static uint32 bogus_ip_pdns=0x0a0b0c0d;
static uint32 bogus_ip_sdns=0x0a0b0c0e;


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DECLARATIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_TRANSITION_STATE

 DESCRIPTION
   
   This function is used to maintain the state of RmSm IP Common.

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

LOCAL void ds_3gpp_rmsm_ip_common_transition_state
(
  dsumts_rmsm_rm_dev_instance_type    instance,
  ds_3gpp_rmsm_ip_common_state_type    new_state
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPCP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

LOCAL void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb
( 
  unsigned long  callback_data
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPV6CP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

LOCAL void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cb
( 
  unsigned long  callback_data
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will generate a IPCP NAK with internal DNS addresses ONLY.
   This function is called in DS context when the rex timer 
   ipcp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPV6CP_RSP_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will generate a IPV6CP NAK with the Interface identifier
   (IID) to be used by the laptop.
 
   This function is called in DS context when the rex timer 
   ipv6cp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PROCESS_ENTITLEMENT_CHECK_FAILED_CMD

 DESCRIPTION
   
   This function will process the entitlement check failed cmd.
 
   This function is called when there is an error in posting entitlement check
   event to PS.

 DEPENDENCIES
   None

 RETURN VALUE
   None
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_entitlement_check_failed_cmd
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_HTONCNF

 DESCRIPTION
   
   This function will add the IPCP header fields to the IPCP packet in 
   network order
   
 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

/*lint -e818*/
LOCAL dsm_item_type  *ds_3gpp_rmsm_ip_common_ipcp_htoncnf 
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_GET_AUTH_INFO_FROM_RM_PPP

 DESCRIPTION
   
   This function will retrieve the Authentication information from PPP
   device running on the Rm device 

 DEPENDENCIES
   If Auth information is sucessfully retrieved, it updates the Auth info
   in the RM control block to point to the valid auth info structure, else
   it is set to NULL

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp
(
  dsumts_rmsmi_info_type       *rmsmi_info_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPCP_CFG_REQ

 DESCRIPTION
   
   This function will go through all of the options in a IPCP C-Req and store
   them Internally. 

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPCP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
LOCAL sint15 ds_3gpp_rmsm_ip_common_parse_ipcp_cfg_req
(
  dsumts_rmsmi_info_type       *rmsmi_info_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPV6CP_CFG_REQ

 DESCRIPTION
   
   This function goes through the header of the IPv6CP packet. It will retrieve
   the IPv6CP Config Request ID from the header. This ID will be used during
   generation of a NAK / ACK.

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPv6CP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
LOCAL sint15 ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req
(
  dsumts_rmsmi_info_type       *rmsmi_info_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPCP_CFG_REQ_CB

DESCRIPTION
  This callback function is called when a IPCP config request packet is 
  received on the Rm interface

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -e818*/
LOCAL snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPV6CP_CFG_REQ_CB

DESCRIPTION
  This callback function is called when a IPv6CP config request packet is 
  received on the Rm interface

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipv6cp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_CONFIGURE_PPP_CB

DESCRIPTION
  This callback function is called when RmSm IP(v4 or v6 instance) wants to
  configure PPP with IP Address / IID information.
 
  This function will inject the previously received IPCP / IPv6CP packet,
  which would generate a NAK with the newly configured IP Address / IID
  information.

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean ds_3gpp_rmsm_ip_common_configure_ppp_cb 
(
  void *this_ptr,
  ppp_protocol_e_type protocol
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_WANTS_MT_PKT_CALL_EV

  DESCRIPTION
    This function processes UM_WANTS_MT_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_um_wants_mt_pkt_call_ev
(
  dsumts_rmsm_instance_type *inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_REJECT_MT_PKT_CALL_EV

  DESCRIPTION
    This function processes RM_REJECT_MT_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_reject_mt_pkt_call_ev
(
  dsumts_rmsm_instance_type *inst_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_COMMON_PROCESS_RM_UM_LOCKED_EV

  DESCRIPTION
    This function processes RM_UM_LOCKED_EV event. If MT call is somehow
    answered by embedded application we will clear the MT call request
    at tethered port. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ip_common_process_rm_um_locked_ev
(
  dsumts_rmsm_instance_type *inst_ptr
);


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_NO_REDIALS_REMAINING_EV()
  
  DESCRIPTION
  This function processes UMTS_RMSM_NO_REDIALS_REMAINING_EV event. 
  
  PARAMETERS
  instance ptr
  
  RETURN VALUE
  None
  
  DEPENDENCIES
  none
  
  SIDE EFFECTS
  None
 ===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_no_redials_remaining_ev
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_HANDLE_RM_BRING_UP_UM_IFACE_EV
  
  DESCRIPTION
  This function processes RM_BRING_UP_UM_IFACE_EV event. 
  
  PARAMETERS
  instance ptr
  
  RETURN VALUE
  None
  
  DEPENDENCIES
  none
  
  SIDE EFFECTS
  None
 ===========================================================================*/

LOCAL void ds_3gpp_rmsm_ip_common_process_rm_bring_up_um_iface_ev 
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_WANTS_PKT_CALL_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/

LOCAL void ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_CONFIGURED_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_IFACE_DOWN_EV

  DESCRIPTION
    This function processes UM_IFACE_DOWN_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_um_iface_down_ev
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_UP_EV

  DESCRIPTION
    This function processes UM_IFACE_UP_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_um_iface_up_ev
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_DOWN_EV

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_UP_EV

  DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev
(
  dsumts_rmsm_instance_type* inst_ptr
);

LOCAL boolean ds_3gpp_rmsm_ip_common_is_call_tearable
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
);

LOCAL boolean ds_3gpp_rmsm_ip_common_allow_companion_iface_bringup
(
  dsumts_rmsm_instance_type* inst_ptr
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/



/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_CONFIGURE_PPP_CB

DESCRIPTION
  This callback function is called when RmSm IP(v4 or v6 instance) wants to
  configure PPP with IP Address / IID information.
 
  This function will inject the previously received IPCP / IPv6CP packet,
  which would generate a NAK with the newly configured IP Address / IID
  information.

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean ds_3gpp_rmsm_ip_common_configure_ppp_cb 
(
  void *this_ptr,
  ppp_protocol_e_type protocol
)
{
  dsumts_rmsmi_sm_info_type       *sm_info_ptr = NULL;
  dsumts_rmsmi_info_type*         rmsmi_info_ptr = NULL;
  dsumts_rmsm_sm_instance_type    sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
  rex_crit_sect_type               *crit_sec_ptr = NULL;
  uint16                           proto = 0;
  dsm_item_type                    **ppp_pkt_ptr = NULL;
  uint32                           reg_tag = 0;
  uint32                           ip_addr = 0;
  uint64                           te_iid = 0;
  uint64                           rm_iid = 0;
  dsumts_rmsm_instance_type       inst;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ppp_dev_enum_type                ppp_dev;
  /*-----------------------------------------------------------------*/

  DS_3GPP_MSG1_MED("Configure PPP callback invoked, this_ptr: %x", 
                    this_ptr);

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)this_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    return FALSE;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_instance; 

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);

  if (sm_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_info_ptr = NULL");
    return FALSE;
  }
 
  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return FALSE;
  }

  if (protocol == PPP_PROTOCOL_IPCP)
  {

    /*---------------------------------------------------------------------
       Assign the IP address,the DNS addresses and the NBNS addresses 
       obtained by the Um from the network to the peer on the Rm interface. 
       If there is a gateway address obtained by Um from the network, 
       advertixe that as local IP 
    ---------------------------------------------------------------------*/
    ppp_set_v4_server_opts
    ( 
      rmsmi_info_ptr->rm_ppp_config_ptr, 
      sm_info_ptr->um_iface_ptr->v4_net_info.gateway.ps_s_addr,
      PS_IFACE_IP_V4_ADDR(sm_info_ptr->um_iface_ptr),
      sm_info_ptr->um_iface_ptr->v4_net_info.primary_dns.ps_s_addr,
      sm_info_ptr->um_iface_ptr->v4_net_info.secondary_dns.ps_s_addr,
      sm_info_ptr->um_iface_ptr->v4_net_info.primary_nbns.ps_s_addr,
      sm_info_ptr->um_iface_ptr->v4_net_info.secondary_nbns.ps_s_addr
    );

    /*---------------------------------------------------------------------
      Disable VJ, Set the new IPCP options in PPP
    ---------------------------------------------------------------------*/
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.will_mask[FSM_LOCAL] 
      &= ~IPCP_N_COMPRESS;
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.will_mask[FSM_REMOTE] 
      &=~IPCP_N_COMPRESS;
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.want_mask[FSM_LOCAL] 
      &= ~IPCP_N_COMPRESS;
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.want_mask[FSM_REMOTE] 
      &=~IPCP_N_COMPRESS;

    ip_addr = PS_IFACE_IP_V4_ADDR(sm_info_ptr->um_iface_ptr);      
    DS_3GPP_MSG1_HIGH("IP addr::%lx", ps_ntohl(ip_addr)); 

    DS_3GPP_MSG2_HIGH("P DNS:%lx S DNS:%lx",
      ps_ntohl(sm_info_ptr->um_iface_ptr->v4_net_info.primary_dns.ps_s_addr),
      ps_ntohl(sm_info_ptr->um_iface_ptr->v4_net_info.secondary_dns.ps_s_addr));


    crit_sec_ptr = &(info_ptr->v4_info.ipi_sm_info_crit_sec);

    proto = PPP_IPCP_PROTOCOL;
    DSUMTS_ENTER_CRIT_SECTION( crit_sec_ptr ); 
    ppp_pkt_ptr = &info_ptr->v4_info.ipcp_cfg_pkt_ptr;
    reg_tag = info_ptr->v4_info.ipcp_snoop_tag;
    DSUMTS_LEAVE_CRIT_SECTION( crit_sec_ptr );  

  }

  else if (protocol == PPP_PROTOCOL_IPV6CP)
  {

    rm_iid = sm_info_ptr->pdp_specific_info.pdp_ip.v6_sm_info.gateway_iid;
    te_iid = sm_info_ptr->pdp_specific_info.pdp_ip.v6_sm_info.te_iid;

    ppp_set_v6_server_opts(rmsmi_info_ptr->rm_ppp_config_ptr, 
                           rm_iid, te_iid);

    DS_3GPP_MSG2_HIGH("Local IID::%lx, Global IID::%lx", 
                      ps_ntohll(rm_iid),ps_ntohll(te_iid)); 


    crit_sec_ptr = &(info_ptr->v6_info.ipi_sm_info_crit_sec);

    proto = PPP_IPV6CP_PROTOCOL;
    DSUMTS_ENTER_CRIT_SECTION( crit_sec_ptr ); 
    ppp_pkt_ptr = &info_ptr->v6_info.ipv6cp_cfg_pkt_ptr;
    reg_tag = info_ptr->v6_info.ipv6cp_snoop_tag;
    DSUMTS_LEAVE_CRIT_SECTION( crit_sec_ptr );  

  } 

  ppp_update_proto_config(ppp_dev, protocol,
                          rmsmi_info_ptr->rm_ppp_config_ptr);


  if (IPCP_CREQ_RECD(info_ptr) && IPV6CP_CREQ_RECD(info_ptr) )
  {
    DS_3GPP_MSG0_MED("Both IPCP and Ipv6Cp were recd. ");
    ppp_set_cp_mode(ppp_dev, PPP_PROTOCOL_IPCP, PPP_CP_MODE_PASSIVE); 
    ppp_set_cp_mode(ppp_dev, PPP_PROTOCOL_IPV6CP, PPP_CP_MODE_PASSIVE);    
  }

  else if (IPCP_CREQ_RECD(info_ptr))
  {
    if (protocol == PPP_PROTOCOL_IPCP &&
        ppp_get_mode(ppp_dev) != PPP_INTERNAL_MODE )
    {
      DS_3GPP_MSG0_MED("Setting IPV6CP to INVALID");
      ppp_set_cp_mode(ppp_dev, PPP_PROTOCOL_IPV6CP, PPP_CP_MODE_INVALID); 
    }

    else if (protocol == PPP_PROTOCOL_IPV6CP)
    {
      DS_3GPP_MSG0_MED("Setting IPV6CP to PASSIVE");
      ppp_set_cp_mode(ppp_dev, PPP_PROTOCOL_IPV6CP, PPP_CP_MODE_PASSIVE); 
    }
  }

  else if (IPV6CP_CREQ_RECD(info_ptr))
  {
    if (protocol == PPP_PROTOCOL_IPV6CP &&
        ppp_get_mode(ppp_dev) != PPP_INTERNAL_MODE )
    {
      DS_3GPP_MSG3_MED("Setting IPCP to INVALID",0,0,0);
      ppp_set_cp_mode(ppp_dev, PPP_PROTOCOL_IPCP, PPP_CP_MODE_INVALID); 
    }

    else if (protocol == PPP_PROTOCOL_IPCP)
    {
      DS_3GPP_MSG3_MED("Setting IPCP to PASSIVE",0,0,0);
      ppp_set_cp_mode(ppp_dev, PPP_PROTOCOL_IPCP, PPP_CP_MODE_PASSIVE); 
    }
  }

  else
  {
    DS_3GPP_MSG1_HIGH("Invalid PDP Type: %d",  
                        DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr)); 
    ASSERT(0);
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Set the PPP to INTERNAL mode so that packets gets routed up the stack
  ---------------------------------------------------------------------*/
    if (((IPCP_CREQ_RECD(info_ptr) && protocol == PPP_PROTOCOL_IPCP) ||
        (IPV6CP_CREQ_RECD(info_ptr) && protocol == PPP_PROTOCOL_IPV6CP)) &&
        (ppp_get_mode (ppp_dev) != PPP_INTERNAL_MODE)
     )
  {
    ppp_set_mode( ppp_dev, PPP_INTERNAL_MODE );
   
    /*---------------------------------------------------------------------
      Play the buffered IPCP packet 
    ---------------------------------------------------------------------*/
    DSUMTS_ENTER_CRIT_SECTION( crit_sec_ptr ); 
   
    snoop_unreg_event(reg_tag);
   
  if( !ppp_inject_packet(ppp_dev, proto, *ppp_pkt_ptr) )
    {
      *ppp_pkt_ptr= NULL;
       DSUMTS_LEAVE_CRIT_SECTION( crit_sec_ptr );     
       DS_3GPP_MSG0_HIGH("ppp pkt injected ");
       dsumts_rmsm_post_event( &inst , UMTS_RMSM_RM_PPP_CONFIGURED_EV );
       return TRUE;
    }
    else
    {
      DSUMTS_LEAVE_CRIT_SECTION( crit_sec_ptr );
      DS_3GPP_MSG0_HIGH("ppp pkt error ");
      return FALSE;
    }
  }
  
  else
  {
    DS_3GPP_MSG3_HIGH("Not injecting PPP pkt. PPP mode: %d, ipcp creq recd: %d, "
                      "ipv6cp creq recd: %d", 
                      ppp_get_mode (ppp_dev) , 
                      IPCP_CREQ_RECD(info_ptr),
                      IPV6CP_CREQ_RECD(info_ptr));

    return TRUE;
  }
  
} /*ds_3gpp_rmsm_ip_common_configure_ppp_cb*/

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPCP_CFG_REQ_CB

  DESCRIPTION
  This callback function is called when a IPCP config request packet is 
  received on the Rm interface

  DEPENDENCIES
  none.

  RETURN VALUE
  None

  SIDE EFFECTS
  None
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
)
{
  /* sm_instance will be needed to post UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV.
     It can be determined based on protocol. For now assume it to be 0. */

  dsumts_rmsm_sm_instance_type sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE; 

  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;
  dsat_num_item_type tmp_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG3_MED("Received  ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb, device: %d"
                     "protocol: %d", device, protocol, 0 ); 

  /*lint -e730 -esym(613, item_head_ptr)*/
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_IPCP_PROTOCOL );

  inst.rm_dev_instance = dsumts_rmsmi_get_rm_dev_from_ppp_dev(device);
  inst.sm_instance = sm_instance;

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527 */
    return SNOOP_CB_IGNORE_PKT;
    /*lint +e527 */
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);
  
  /*-------------------------------------------------------------------------
    Store the packet in the internal SM info. Discard the earlier packet
    we might have. 
  -------------------------------------------------------------------------*/
  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

  if (DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV4 &&
      DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV4V6 )
  {                                
    DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
    DS_3GPP_MSG1_ERROR("PDP Type %d is not V4, dicarding the IPCP packet",
                       DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }
  /*-------------------------------------------------------------------------
    If we have already injected the packet to PPP and moved to 
    DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE state,
    ignore the config packet
  -------------------------------------------------------------------------*/
  if(info_ptr->state == DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE) 
  {                                
    DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
    DS_3GPP_MSG1_HIGH("IPCP packet received in state %d, freeing it",
                       DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  if( info_ptr->v4_info.ipcp_cfg_pkt_ptr != NULL )
  {
    DS_3GPP_MSG0_HIGH("Freeing previous IPCP cfg req pkt");
    dsm_free_packet(&info_ptr->v4_info.ipcp_cfg_pkt_ptr);
  }

  info_ptr->v4_info.ipcp_cfg_pkt_ptr =  *item_head_ptr;
  
  /*---------------------------------------------------------------------
    Get PPP auth parameters from PPP on RM
    Parse IPCP config req packet on the Rm & bring up Um interface
  ---------------------------------------------------------------------*/
  if( ds_3gpp_rmsm_ip_common_parse_ipcp_cfg_req(rmsmi_info_ptr) == -1)
  {
    DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
    DS_3GPP_MSG0_ERROR("Error Parsing IPCP Config Req Packet, freeing it");
    dsm_free_packet(item_head_ptr);

    dsumts_rmsm_post_event(&inst, UMTS_RMSM_RM_PPP_DOWN_EV);
    return SNOOP_CB_IGNORE_PKT;
  } 

  /*-----------------------------------------------------------------
    See if we should respond to the IPCP packets with NAK's while the
    UM interface is coming UP.
   -----------------------------------------------------------------*/
  tmp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCPDPLT_IDX,
                                                 0,0,NUM_TYPE);
  info_ptr->v4_info.send_ipcp_naks_flg = (uint8)tmp_val;
      
  ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp(rmsmi_info_ptr);

    /*---------------------------------------------------------------------
    If we are set to generate IPCP naks, kick start a timer to delay
    the IPCP nak generation
  ---------------------------------------------------------------------*/
  if(info_ptr->v4_info.send_ipcp_naks_flg )
  {
    /*-----------------------------------------------------------------
      When the timer fires off& if  UM is not up, generate the NAK packet
    -----------------------------------------------------------------*/
    (void)rex_set_timer(&(info_ptr->v4_info.ipcp_res_del_timer),
            (rex_timer_cnt_type)DS_3GPP_RMSM_IP_COMMON_IPCP_RES_DELAY_TIME);

    DS_3GPP_MSG0_HIGH(" Initiating IPCP NAK delay timeout");
  }

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

  ds_3gpp_rmsm_ip_common_set_ipcp_creq_recd(info_ptr);


  DS_3GPP_MSG1_HIGH("Rx IPCP CReq: posting UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV, "
                    "RmSM IP Common State: %d ",
                     info_ptr->state);

  dsumts_rmsm_post_event( &inst , UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV );


  return SNOOP_CB_IGNORE_PKT;

}

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPCP_CFG_REQ_CB

DESCRIPTION
  This callback function is called when a IPCP config request packet is 
  received on the Rm interface

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -e818*/
LOCAL snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipv6cp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
)
{
  dsumts_rmsm_sm_instance_type sm_instance = DSUMTS_RMSM_V6_SM_INSTANCE; 
  
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;
  dsat_num_item_type tmp_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG2_HIGH("Received  dsumts_rmsmi_rm_ipv6cp_cfg_req_cb, device: %d"
                    "protocol: %d", device, protocol); 

  ASSERT(device < PPP_MAX_DEV);
  /*lint -e730 -esym(613, item_head_ptr)*/
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(protocol == PPP_IPV6CP_PROTOCOL );

  inst.rm_dev_instance = dsumts_rmsmi_get_rm_dev_from_ppp_dev(device);
  inst.sm_instance = sm_instance;


  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);
  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527 */
    return SNOOP_CB_IGNORE_PKT;
    /*lint +e527 */
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;

  ASSERT(info_ptr != NULL);
  
  /*-------------------------------------------------------------------------
    Store the packet in the internal SM info. Discard the earlier packet
    we might have. 
  -------------------------------------------------------------------------*/
  DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

  if (DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV6 &&
      DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV4V6 )
  {                                
    DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
    DS_3GPP_MSG1_ERROR("PDP Type %d is not V6, dicarding the IPV6CP packet",
                       DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    If we have already injected the packet to PPP and moved to 
    DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE state,
    ignore the config packet
  -------------------------------------------------------------------------*/
  if(rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state == 
       DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE)
  {                                
    DSUMTS_BRANCH_LEAVE_CRIT_SECTION(&(info_ptr->v6_info.ipi_sm_info_crit_sec) );
    DS_3GPP_MSG1_HIGH("IPV6CP packet received in state %d, freeing it",
                        DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  if( info_ptr->v6_info.ipv6cp_cfg_pkt_ptr != NULL )
  {
    DS_3GPP_MSG0_HIGH("Freeing previous IPV6CP cfg req pkt");

    dsm_free_packet(&info_ptr->v6_info.ipv6cp_cfg_pkt_ptr);
  }

  info_ptr->v6_info.ipv6cp_cfg_pkt_ptr =  *item_head_ptr;
  
  DS_3GPP_MSG0_HIGH("Rx IPCP CReq: posting UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV  ");

  /*---------------------------------------------------------------------
    Get PPP auth parameters from PPP on RM
    Parse IPCP config req packet on the Rm & bring up Um interface
  ---------------------------------------------------------------------*/
  if( ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req(rmsmi_info_ptr) == -1)
  {
    DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
    DS_3GPP_MSG0_ERROR("Error Parsing IPV6CP Config Req Packet, freeing it");
    dsm_free_packet(item_head_ptr);
    dsumts_rmsm_post_event(&inst, UMTS_RMSM_RM_PPP_DOWN_EV);

    return SNOOP_CB_IGNORE_PKT;
  } 

  /*-----------------------------------------------------------------
    See if we should respond to the IPV6CP packets with NAK's while the
    UM interface is coming UP.
   -----------------------------------------------------------------*/
  tmp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCPDPLT_IDX,
                                                 0,0,NUM_TYPE); 
  info_ptr->v6_info.send_ipv6cp_naks_flg = (uint8)tmp_val;
      
  ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp(rmsmi_info_ptr);

    /*---------------------------------------------------------------------
    If we are set to generate IPv6CP naks, kick start a timer to delay
    the IPCP nak generation
  ---------------------------------------------------------------------*/
  if(info_ptr->v6_info.send_ipv6cp_naks_flg )
  {
    /*-----------------------------------------------------------------
      When the timer fires off& if  UM is not up, generate the NAK packet
    -----------------------------------------------------------------*/
    (void)rex_set_timer(&(info_ptr->v6_info.ipv6cp_res_del_timer),
        (rex_timer_cnt_type)DS_3GPP_RMSM_IP_COMMON_IPV6CP_RES_DELAY_TIME);
    DS_3GPP_MSG0_HIGH(" Initiating IPV6CP NAK delay timeout");
  }

  DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );


  ds_3gpp_rmsm_ip_common_set_ipv6cp_creq_recd(info_ptr);


    DS_3GPP_MSG3_HIGH("Rx IPV6CP CReq: posting UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV, "
                      "RmSM IP Common State: %d ",
                       info_ptr->state,
                       0,
                       0);

  dsumts_rmsm_post_event( &inst , UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV );


  return SNOOP_CB_IGNORE_PKT;

}

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPCP_CFG_REQ

 DESCRIPTION
   
   This function will go through all of the options in a IPCP C-Req and store
   them Internally. 

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPCP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
LOCAL sint15 ds_3gpp_rmsm_ip_common_parse_ipcp_cfg_req
(
  dsumts_rmsmi_info_type       *rmsmi_info_ptr
)
{
  /*lint -e655*/
   dsm_item_type* item_ptr = NULL;       /* Working ptr to PPP packet */
   struct option_hdr opt_hdr;            /* option header */
   struct config_hdr cfg_hdr;            /* Config hdr: has Code, ID, len */
   sint31 pkt_len = 0;                   /* Length of the entire packet  */
   uint16 offset = 0;                    /* offset from begining of current item */
   uint16 copy_len = CONFIG_HDR_LEN;     /* Number of bytes copied into buffer */
   byte opt_val[PPP_OPT_MAX_LEN];        /* array to copy option value into */

   ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;

   /*-----------------------------------------------------------------------*/

   memset(opt_val, 0, PPP_OPT_MAX_LEN);
   memset(&opt_hdr, 0, sizeof(opt_hdr));
   memset(&cfg_hdr, 0, sizeof(cfg_hdr));

   DS_3GPP_MSG1_MED("Parsing IPCP Config Request Packet, rmsmi_info_ptr: %x",
                   rmsmi_info_ptr);

   if (rmsmi_info_ptr == NULL) 
   {
     DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
     /*lint -e527 */
     return -1;
     /*lint +e527 */
   }
   
   info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
   ASSERT(info_ptr != NULL);

   DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

   item_ptr = info_ptr->v4_info.ipcp_cfg_pkt_ptr;   
      
   ASSERT(item_ptr != NULL);
   
   /*-------------------------------------------------------------------------
     Reset the options seen in the packet
   -------------------------------------------------------------------------*/
   memset(&info_ptr->ppp_info.ipcp_info,0x0, sizeof(ipcp_info_type));

  /*-------------------------------------------------------------------------
     Extract the code, id and length fields (4 bytes) into cfg_hdr. Convert
     the length from network to host.
   -------------------------------------------------------------------------*/

   if(dsm_seek_extract(&item_ptr, &offset, &cfg_hdr, &copy_len) == FALSE)
   {
     DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
     DS_3GPP_MSG0_ERROR("seek_extract() failed - ignoring pkt");
     return -1;
   }

   info_ptr->v4_info.ipcp_creq_id = cfg_hdr.identifier;
   pkt_len =  ps_ntohs(cfg_hdr.length);

   DS_3GPP_MSG1_HIGH("ipcp id:%d",cfg_hdr.identifier);

   pkt_len -= CONFIG_HDR_LEN;
   offset  += CONFIG_HDR_LEN;

   /*-------------------------------------------------------------------------
     Go through the entire IPCP packet, one option at a time.
   -------------------------------------------------------------------------*/
   while ( pkt_len > 0)
   {
     /*-----------------------------------------------------------------------
       Get the option header, and update the packet length to reflect that the
       header has been grabbed.
     -----------------------------------------------------------------------*/
     memset(&opt_hdr, 0, sizeof(opt_hdr));
     copy_len = OPTION_HDR_LEN;
     if(dsm_seek_extract(&item_ptr, &offset, &opt_hdr, &copy_len) == FALSE)
     {
       DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
       DS_3GPP_MSG0_ERROR( "seek_extract() failed - ignoring pkt");
       return -1;
     }
     pkt_len -= opt_hdr.length;

     /*-----------------------------------------------------------------------
       Get the options value IFF it has one.
     -----------------------------------------------------------------------*/
     offset += OPTION_HDR_LEN;
     copy_len =  opt_hdr.length - OPTION_HDR_LEN;
     if ( copy_len > 0)
     {
       if(dsm_seek_extract(&item_ptr, &offset, opt_val, &copy_len) == FALSE)
       {
         DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
         DS_3GPP_MSG0_ERROR( "seek_extract() failed - ignoring pkt");
         return -1;
        }
       offset += copy_len;
     }
     /*-----------------------------------------------------------------------
       Save the options field
     -----------------------------------------------------------------------*/
    switch(opt_hdr.type)
    {
      /*-----------------------------------------------------------------------
        VJ compression
      -----------------------------------------------------------------------*/
      case IPCP_COMPRESS:
        DS_3GPP_MSG0_HIGH("Ignoring VJ");
        break;
  
      /*-----------------------------------------------------------------------
        IP address
      -----------------------------------------------------------------------*/
      case IPCP_1ADDRESS:
        info_ptr->ppp_info.ipcp_info.ip_address = 
          ps_ntohl((*((uint32*)opt_val)));
        info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_IP_ADDR_MASK;
        DS_3GPP_MSG1_HIGH("IP address in opt:%lx", 
                           info_ptr->ppp_info.ipcp_info.ip_address);
        break;
  
      /*-----------------------------------------------------------------------
        primary dns server address
      -----------------------------------------------------------------------*/
      case IPCP_PRIMARY_DNS:
        info_ptr->ppp_info.ipcp_info.primary_dns = 
          ps_ntohl((*((uint32*)opt_val)));
        info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_PRI_DNS_MASK;
        DS_3GPP_MSG1_HIGH("primary DNS address in opt:%lx", 
                           info_ptr->ppp_info.ipcp_info.primary_dns);
        break;
  
      /*-----------------------------------------------------------------------
        secondary dns server address
      -----------------------------------------------------------------------*/
      case IPCP_SECONDARY_DNS:
        info_ptr->ppp_info.ipcp_info.secondary_dns = 
          ps_ntohl((*((uint32*)opt_val)));
        info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_SEC_DNS_MASK;
        DS_3GPP_MSG1_HIGH("secondary DNS address in opt:%lx", 
                           info_ptr->ppp_info.ipcp_info.primary_dns);
        break;
       
      /*-----------------------------------------------------------------------
        Process NBNS options only if NBNS NV item is enabled.  
      -----------------------------------------------------------------------*/
      /*-----------------------------------------------------------------------
        primary nbns server address
      -----------------------------------------------------------------------*/
      case IPCP_PRIMARY_NBNS:
        if( ds_3gpp_nv_manager_get_nbns_enabled(rmsmi_info_ptr->subs_id))  
        {
          info_ptr->ppp_info.ipcp_info.primary_nbns =
            ps_ntohl((*((uint32*)opt_val)));
          info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_PRI_NBNS_MASK;
          DS_3GPP_MSG1_HIGH("primary NBNS address in opt:%lx",
                             info_ptr->ppp_info.ipcp_info.primary_nbns);
        }
      break;

      /*-----------------------------------------------------------------------
        secondary nbns server address
      -----------------------------------------------------------------------*/
      case IPCP_SECONDARY_NBNS:
        if( ds_3gpp_nv_manager_get_nbns_enabled(rmsmi_info_ptr->subs_id))  
        {
          info_ptr->ppp_info.ipcp_info.secondary_nbns = 
            ps_ntohl((*((uint32*)opt_val)));
          info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_SEC_NBNS_MASK;
          DS_3GPP_MSG1_HIGH("secondary NBNS address in opt:%lx",
                             info_ptr->ppp_info.ipcp_info.secondary_nbns);        
        }
      break;

      /*-----------------------------------------------------------------------
        default
      -----------------------------------------------------------------------*/
      default:
        DS_3GPP_MSG1_HIGH("Got unknown option %d - ignoring", opt_hdr.type);
  
    } /* switch(opt_hdr.type) */

   } /* while(more options) */
   

   DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

   return 0;
   /*lint +e655*/
} /*ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req() */


/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPV6CP_CFG_REQ

 DESCRIPTION
   
   This function goes through the header of the IPv6CP packet. It will retrieve
   the IPv6CP Config Request ID from the header. This ID will be used during
   generation of a NAK / ACK.

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPv6CP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
LOCAL sint15 ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req
(
  dsumts_rmsmi_info_type       *rmsmi_info_ptr
)
{
   dsm_item_type* item_ptr = NULL;       /* Working ptr to PPP packet */
   struct option_hdr opt_hdr;            /* option header */
   struct config_hdr cfg_hdr;            /* Config hdr: has Code, ID, len */
   uint16 offset = 0;                    /* offset from begining of 
                                            current item */
   uint16 copy_len = CONFIG_HDR_LEN;     /* Number of bytes copied into buffer */
   byte opt_val[PPP_OPT_MAX_LEN];        /* array to copy option value into */ 

   ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
   memset(opt_val, 0, PPP_OPT_MAX_LEN);
   memset(&opt_hdr, 0, sizeof(opt_hdr));
   memset(&cfg_hdr, 0, sizeof(cfg_hdr));
   
   DS_3GPP_MSG1_MED("Parsing IPV6CP Config Request Packet, rmsmi_info_ptr: %x", 
                    rmsmi_info_ptr);

   if (rmsmi_info_ptr == NULL) 
   {
	   DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	   /*lint -e527 */
	   return -1;
	   /*lint +e527 */
   }
   
   info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
   ASSERT(info_ptr != NULL);

   DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

   item_ptr = info_ptr->v6_info.ipv6cp_cfg_pkt_ptr;   
      
   ASSERT(item_ptr != NULL);
   
   if(dsm_seek_extract(&item_ptr, &offset, &cfg_hdr, &copy_len) == FALSE)
   {
     DSUMTS_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
     DS_3GPP_MSG0_ERROR("seek_extract() failed - ignoring pkt");
     return -1;
   }
   info_ptr->v6_info.ipv6cp_creq_id = cfg_hdr.identifier;

   DS_3GPP_MSG1_HIGH("ipv6cp id:%d",cfg_hdr.identifier);

   DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

   return 0;
   /*lint +e655*/
} /*ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req() */


/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_GET_AUTH_INFO_FROM_RM_PPP

 DESCRIPTION
   
   This function will retrieve the Authentitcation information from PPP
   device running on the Rm device 

 DEPENDENCIES
   If Auth information is sucessfully retrieved, it updates the Auth info
   in the RM state machine to point to the valid auth info structure, else
   it is set to NULL

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
)
{
  /*lint -e655*/
  int result = -1;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ppp_type *ppp_cb_array = NULL;
  ppp_dev_enum_type ppp_dev;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_3GPP_MSG1_MED("Getting Auth info from Rm PPP, rmsmi_info_ptr: %x", 
                   rmsmi_info_ptr);

  if (rmsmi_info_ptr == NULL)
  {
   	DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	  /*lint -e527 */
	  return;
	  /*lint +e527 */
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  result = ppp_get_auth_info(ppp_dev,
                             &info_ptr->ppp_info.auth_info.params,
                             &info_ptr->ppp_info.auth_info.auth_id );

  /*lint -e641 */
  if( result != 0 )
  {
    /*-----------------------------------------------------------------------
      Auth was never done on Rm. Update Mode
    -----------------------------------------------------------------------*/
    info_ptr->ppp_info.auth_info.auth_type = DS_UMTS_PDP_AUTH_NONE;
  }
  else
  {
    info_ptr->ppp_info.valid_fields |= NET_CFG_PRM_AUTH_PREF_MASK |
                                       NET_CFG_PRM_AUTH_USERNAME_MASK | 
                                       NET_CFG_PRM_AUTH_PASSWORD_MASK;

   /* ppp_cb_array used by PPP_AUTH_WAS_CHAP macro */
   ppp_cb_array = fsm_get_ppp_cb_array();

   /*-----------------------------------------------------------------------
      Find out if it was PAP/CHAP
    -----------------------------------------------------------------------*/
    if( PPP_AUTH_WAS_CHAP(ppp_dev) )
    {
      info_ptr->ppp_info.auth_info.auth_type =DS_UMTS_PDP_AUTH_CHAP;
      info_ptr->ppp_info.valid_fields |= NET_CFG_PRM_CHAP_CHAL_INFO_MASK  ;
    }
    else
    {
      info_ptr->ppp_info.auth_info.auth_type = DS_UMTS_PDP_AUTH_PAP;
    }
  }
  /*lint +e641 */
  /*lint +e655*/
} /*ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_HTONCNF

 DESCRIPTION
   
   This function will add the IPCP header fields to the IPCP packet in 
   network order
   
 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

/*lint -e818*/
LOCAL dsm_item_type  *ds_3gpp_rmsm_ip_common_ipcp_htoncnf 
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
)
{
register uint8 *cp = NULL;           
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  DS_3GPP_MSG2_MED("Entered ds_3gpp_rmsm_ip_common_ipcp_htoncnf, config_hdr: %x"
                    "reply_bp: %x", cnf, bp);
 
  if (cnf == NULL) 
  {
    DATA_ERR_FATAL("cnf = NULL");
	  /*lint -e527 */
	  return NULL;
	  /*lint +e527 */
  }
  if (dsm_pushdown(&bp, NULL, CONFIG_HDR_LEN, 
           DSM_DS_SMALL_ITEM_POOL) == CONFIG_HDR_LEN)
  {
    cp = bp->data_ptr;
    *cp++ = cnf->code;
    *cp++ = cnf->identifier;
    (void)put16(cp, cnf->length);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Out of Memory");
    dsm_free_packet(&bp);
  }
  
  return bp;
} /* dsumts_ip_common_ipcp_htoncnf() */
/*lint +e818*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPCP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb
( 
  unsigned long  callback_data
)
{
  ds_cmd_type                *cmd_ptr;  /* DS Task cmd buf        */
  dsumts_rmsm_instance_type  *data_ptr = NULL;
  dsumts_rmsmi_info_type     *rmsmi_info_ptr = NULL;

  /*-----------------------------------------------------------------------
    Get the pointer to the state machine. We need sm_ptr to 
  -----------------------------------------------------------------------*/
  rmsmi_info_ptr = (dsumts_rmsmi_info_type*) callback_data;

  if (rmsmi_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(dsumts_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_IPCP_RSP_TIMER;
    data_ptr = (dsumts_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->rm_dev_instance = rmsmi_info_ptr->instance;
    data_ptr->sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
    ds_put_cmd(cmd_ptr);
  }
} /*ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will generate a IPCP NAK with internal DNS addresses ONLY.
   This function is called in DS context when the rex timer 
   ipcp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr
( 
  dsumts_rmsm_instance_type* inst_ptr
)
{

  dsumts_rmsmi_info_type  *rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type  *sm_info_ptr = NULL;
  dsm_item_type           *reply_bp = NULL;              /* reply packet   */
  struct config_hdr       hdr;

  register uint8          *cp = NULL;
  uint16                  opt_len = 0;
  uint8                   *ipcp_buf = NULL;
  uint16                  protocol = 0 ;
  ppp_dev_enum_type       ppp_dev;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&hdr, 0, sizeof(hdr));

  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  DS_3GPP_MSG1_MED("Entered IPCP Response Timer Callback handling,"
                   " Rm Dev instance: %d", 
                   inst_ptr->rm_dev_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("sm_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  if( DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV4 &&
      DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV4V6)
  {
    DS_3GPP_MSG1_HIGH("Wrong PDP Type: %d, Expected V4. Returning",
                       DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    return;
  }

  /*-----------------------------------------------------------------------
    Generate the response packet ONLY if we get the timer in 
    DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_UP_STATE state
  -----------------------------------------------------------------------*/
  if( dsumts_rmsm_ip_get_state(sm_info_ptr) != DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE  )
  {
    DS_3GPP_MSG1_HIGH("IPCP rsp gen timer fired in wrong state:%d ignoring",
             rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
    return;
  }

  /*-----------------------------------------------------------------------
    Allocate DSM items with space reserved for IPCP config HDR. Reserve the
    space only if the Small DSM item has enough space to store config HDR. 
    Otherwise dsumts_ip_ipcp_htoncnf will allocate a new DSM item for HDR.
  -----------------------------------------------------------------------*/
  if( sizeof(struct config_hdr) <= DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))
  {
    reply_bp = dsm_offset_new_buffer(
                         DSM_DS_SMALL_ITEM_POOL,
                         sizeof(ppp_config_hdr_type));  
    if( reply_bp == NULL) 
    {
    /*-----------------------------------------------------------------------
        Assert since we ran out of small DSM items. We should not reach here 
        since after we hit low threshold on Small DSM items, RLC will release 
        all the DSM items in its queues.
    -----------------------------------------------------------------------*/
        DATA_ERR_FATAL("failed to DSM item for IPCP config HDR");
        return;
    }
  }

  opt_len = IPCP_1ADDRESS_SIZ;
  protocol = PPP_IPCP_PROTOCOL;
  ipcp_buf = (uint8*)DSUMTS_RMSMI_MEM_ALLOC(opt_len*sizeof(uint8),
                                            MODEM_MEM_CLIENT_DATA);
  if(ipcp_buf == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-----------------------------------------------------------------------
    primary dns server address
  -----------------------------------------------------------------------*/
  cp    = ipcp_buf;
  *cp++ = IPCP_PRIMARY_DNS;
  *cp++ = IPCP_1ADDRESS_SIZ;
  cp    = put32(cp, bogus_ip_pdns);
  if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
  {
    dsm_free_packet(&reply_bp);
    DSUMTS_RMSMI_MEM_FREE(ipcp_buf,MODEM_MEM_CLIENT_DATA);
    DATA_ERR_FATAL("failed to push Pri DNS option to DSM item");
    return;
  }


  /*-----------------------------------------------------------------------
    secondary dns server address
  -----------------------------------------------------------------------*/
  cp    = ipcp_buf;
  *cp++ = IPCP_SECONDARY_DNS;
  *cp++ = IPCP_1ADDRESS_SIZ;
  cp    = put32(cp, bogus_ip_sdns);
  if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
  {
    dsm_free_packet(&reply_bp);
    DSUMTS_RMSMI_MEM_FREE(ipcp_buf,MODEM_MEM_CLIENT_DATA);
    DATA_ERR_FATAL("failed to push Sec DNS option to DSM item");
    return;
  }
  
  /*-----------------------------------------------------------------------
    Process NBNS options only if NBNS NV item is enabled.  
  -----------------------------------------------------------------------*/
  if(ds_3gpp_nv_manager_get_nbns_enabled(rmsmi_info_ptr->subs_id))
  {
    /*---------------------------------------------------------------------
      primary nbns server address
  -----------------------------------------------------------------------*/
    cp    = ipcp_buf;
    *cp++ = IPCP_PRIMARY_NBNS;
    *cp++ = IPCP_1ADDRESS_SIZ;
    cp    = put32(cp, bogus_ip_pdns);
    if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
    {
      dsm_free_packet(&reply_bp);
      DSUMTS_RMSMI_MEM_FREE(ipcp_buf,MODEM_MEM_CLIENT_DATA);
      DATA_ERR_FATAL("failed to push Pri NBNS option to DSM item");
      return;
    }


    /*-----------------------------------------------------------------------
      secondary nbns server address
    -----------------------------------------------------------------------*/
    cp    = ipcp_buf;
    *cp++ = IPCP_SECONDARY_NBNS;
    *cp++ = IPCP_1ADDRESS_SIZ;
    cp    = put32(cp, bogus_ip_sdns);
    if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                                opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
    {
      dsm_free_packet(&reply_bp);
      DSUMTS_RMSMI_MEM_FREE(ipcp_buf,MODEM_MEM_CLIENT_DATA);
      DATA_ERR_FATAL("failed to push Sec NBNS option to DSM item");
      return;
    }
  }

  DS_3GPP_MSG2_HIGH("Bogus IP PDNs :%lx Bogus IP SDNs:%lx",
                    bogus_ip_pdns,bogus_ip_sdns);

  hdr.code = CONFIG_NAK;
  hdr.identifier =
    rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v4_info.ipcp_creq_id; 
  hdr.length = (uint16)(dsm_length_packet(reply_bp) + CONFIG_HDR_LEN);

  /*-------------------------------------------------------------------------
    Prepend header to packet
  -------------------------------------------------------------------------*/
  reply_bp = ds_3gpp_rmsm_ip_common_ipcp_htoncnf(&hdr,reply_bp);
  if (reply_bp == NULL)
  {
    DS_3GPP_MSG0_ERROR("htoncf failed : IPCP NAK not generated");
    dsm_free_packet(&reply_bp);
    DSUMTS_RMSMI_MEM_FREE(ipcp_buf,MODEM_MEM_CLIENT_DATA);
    return ;
  }

  if(pppi_outgoing(ppp_dev, protocol, &reply_bp, NULL) == -1)
  {
   /*---------------------------------------------------------------------
     pppi_outgoing() frees the packet in case of an error
   ---------------------------------------------------------------------*/
   DS_3GPP_MSG0_HIGH( "pppi_outgoing() Error");
  }
  DSUMTS_RMSMI_MEM_FREE(ipcp_buf,MODEM_MEM_CLIENT_DATA);

  DS_3GPP_MSG0_HIGH("IPCP delayed NAK sent");

} /*ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPV6CP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPV6CP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cb
( 
  unsigned long  callback_data
)
{
  ds_cmd_type                *cmd_ptr; /* DS Task cmd buf        */
  dsumts_rmsm_instance_type  *data_ptr = NULL;
  dsumts_rmsmi_info_type     *rmsmi_info_ptr = NULL;

  /*-----------------------------------------------------------------------
    Get the pointer to the state machine. We need sm_ptr to 
  -----------------------------------------------------------------------*/
  rmsmi_info_ptr = (dsumts_rmsmi_info_type*) callback_data;

  if (rmsmi_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(dsumts_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_IPV6CP_RSP_TIMER;
    data_ptr = (dsumts_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->rm_dev_instance = rmsmi_info_ptr->instance;
    data_ptr->sm_instance = DSUMTS_RMSM_V6_SM_INSTANCE;
    ds_put_cmd(cmd_ptr);
  }
} /* dsumts_rmsm_ip_ipv6cp_rsp_timer_cb */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPV6CP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will generate a IPCP NAK with internal DNS addresses ONLY.
   This function is called in DS context when the rex timer 
   ipcp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr
( 
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_info_type  *rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
  dsm_item_type           *reply_bp = NULL;              /* reply packet   */
  struct config_hdr       hdr;

  register uint8          *cp = NULL;
  uint16                  opt_len = 0;
  uint8                   *ipv6cp_buf = NULL;
  uint64                  iid = 0;
  uint16                  protocol = 0;
  int16                   ps_errno = 0;
  ppp_dev_enum_type       ppp_dev;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&hdr, 0, sizeof(hdr));

  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  DS_3GPP_MSG1_MED("Entered IPV6CP Response Timer Callback handling, "
                   "Rm Dev instance: %d", 
                   inst_ptr->rm_dev_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  sm_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  if( DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV6 &&
      DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_UMTS_PDP_IPV4V6)
  {
    DS_3GPP_MSG1_HIGH("Wrong PDP Type: %d, Expected V6. Returning",
                       DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    return;
  }

  /*-----------------------------------------------------------------------
    Generate the response packet ONLY if we get the timer in 
    DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_UP_STATE state
  -----------------------------------------------------------------------*/
  if( dsumts_rmsm_ip_get_state(sm_ptr) != DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE  )
  {
    DS_3GPP_MSG1_HIGH("IPv6CP rsp gen timer fired in wrong state:%d ignoring",
             rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
    return;
  }

  /*-----------------------------------------------------------------------
    Allocate DSM items with space reserved for IPCP config HDR. Reserve the
    space only if the Small DSM item has enough space to store config HDR. 
    Otherwise dsumts_ip_ipcp_htoncnf will allocate a new DSM item for HDR.
  -----------------------------------------------------------------------*/
  if( sizeof(struct config_hdr) <= DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))
  {
    reply_bp = dsm_offset_new_buffer(
                         DSM_DS_SMALL_ITEM_POOL,
                         sizeof(ppp_config_hdr_type));  
    if( reply_bp == NULL) 
    {
    /*-----------------------------------------------------------------------
        Assert since we ran out of small DSM items. We should not reach here 
        since after we hit low threshold on Small DSM items, RLC will release 
        all the DSM items in its queues.
    -----------------------------------------------------------------------*/
        DATA_ERR_FATAL("failed to DSM item for IPCP config HDR");
        return;
    }
  }

  opt_len = IPV6CP_IFACE_ID_OPTION_SIZE;
  protocol = PPP_IPV6CP_PROTOCOL;
  ipv6cp_buf = (uint8*)DSUMTS_RMSMI_MEM_ALLOC(opt_len*sizeof(uint8), 
                                              MODEM_MEM_CLIENT_DATA);
  if( ipv6cp_buf == NULL)
  {
    ASSERT(0);
    return;
  }

  if (-1 == ps_iface_generate_ipv6_iid(&sm_ptr->sio_iface, &iid, &ps_errno))
  {
    DSUMTS_RMSMI_MEM_FREE(ipv6cp_buf,MODEM_MEM_CLIENT_DATA);
    DATA_ERR_FATAL("Failed to generate a random IPv6 IID");
    return;
  }

  /*-----------------------------------------------------------------------
    Interface ID
  -----------------------------------------------------------------------*/
  cp    = ipv6cp_buf;
  *cp++ = IPV6CP_IFACE_ID_OPTION;
  *cp++ = IPV6CP_IFACE_ID_OPTION_SIZE;
  cp    = put64(cp, iid);
  if( dsm_pushdown_tail( &reply_bp, ipv6cp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
  {
    dsm_free_packet(&reply_bp);
    DSUMTS_RMSMI_MEM_FREE(ipv6cp_buf,MODEM_MEM_CLIENT_DATA);
    DATA_ERR_FATAL("failed to push Pri DNS option to DSM item");
    return;
  }

  DS_3GPP_MSG1_HIGH("IID :%lx",iid);
  
  hdr.code = CONFIG_NAK;
  hdr.identifier = 
    rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v4_info.ipcp_creq_id; 
  hdr.length = (uint16)(dsm_length_packet(reply_bp) + CONFIG_HDR_LEN);

  /*-------------------------------------------------------------------------
    Prepend header to packet
  -------------------------------------------------------------------------*/
  reply_bp = ds_3gpp_rmsm_ip_common_ipcp_htoncnf(&hdr,reply_bp);
  if (reply_bp == NULL)
  {
    DS_3GPP_MSG0_ERROR("htoncf failed : IPCP NAK not generated");
    dsm_free_packet(&reply_bp);
    DSUMTS_RMSMI_MEM_FREE(ipv6cp_buf,MODEM_MEM_CLIENT_DATA);
    return ;
  }

  if(pppi_outgoing(ppp_dev, protocol, &reply_bp, NULL) == -1)
  {
   /*---------------------------------------------------------------------
     pppi_outgoing() frees the packet in case of an error
   ---------------------------------------------------------------------*/
   DS_3GPP_MSG0_HIGH( "pppi_outgoing() Error");
  }

  DSUMTS_RMSMI_MEM_FREE(ipv6cp_buf,MODEM_MEM_CLIENT_DATA);
  DS_3GPP_MSG0_HIGH("IPv6CP delayed NAK sent");
} /* ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr */


/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_CLEAR_NAK_TIMER

 DESCRIPTION
   
   This function is used to clear the NAK timer.

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
static void ds_3gpp_rmsm_ip_common_clear_nak_timer
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
)
{
  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
    return;
  }
  
  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);


  if (rmsmi_info_ptr->instance == DSUMTS_RMSM_V4_SM_INSTANCE)
  {  
     DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
   
    /*---------------------------------------------------------------------
      Stop the IPCP response generation timer, if we had started it
    ---------------------------------------------------------------------*/            
    if(info_ptr->v4_info.send_ipcp_naks_flg )
    {
      /*-----------------------------------------------------------------
         Stop the timer to delay the IPCP response packet generation.
      -----------------------------------------------------------------*/
     (void)rex_clr_timer(&(info_ptr->v4_info.ipcp_res_del_timer));
    }

    DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
  }
  
  else if (rmsmi_info_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
  {
    DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
  
   /*---------------------------------------------------------------------
     Stop the IPV6CP response generation timer, if we had started it
    ---------------------------------------------------------------------*/            
    if(info_ptr->v6_info.send_ipv6cp_naks_flg )
    {
      /*-----------------------------------------------------------------
         Stop the timer to delay the IPv6CP response packet generation.
      -----------------------------------------------------------------*/
     (void)rex_clr_timer(&(info_ptr->v6_info.ipv6cp_res_del_timer));

    }

    DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
  }
  
  else
  {
    DS_3GPP_MSG1_ERROR("PDP Type is: %d, Expected V4 or V6", 
                        DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));          
    ASSERT(0);
  }
} /* ds_3gpp_rmsm_ip_common_clear_nak_timer */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PROCESS_ENTITLEMENT_CHECK_FAILED_CMD

 DESCRIPTION
   
   This function will process the entitlement check failed cmd.
 
   This function is called when there is an error in posting entitlement check
   event to PS.

 DEPENDENCIES
   None

 RETURN VALUE
   None
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_entitlement_check_failed_cmd
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_info_type  *rmsmi_info_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    return;
  }

  DS_3GPP_MSG1_MED("Entered Process Entitlement Check Failed CMd handling, "
                   "Rm Dev instance: %d", inst_ptr->rm_dev_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
     DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
     return;
  }

  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:

      rmsmi_info_ptr->entitlement_check_req = FALSE;
      /*-------------------------------------------------------------------
        Transition state to Waiting for IPCP config req state
      -------------------------------------------------------------------*/
      ds_3gpp_rmsm_ip_common_transition_state(inst_ptr->rm_dev_instance, 
                              DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE);
      break;

    default:
      DS_3GPP_MSG1_ERROR("Ignore entitlement failed Ev in %d state ", 
                       rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state); 
      break;
  }

} /* ds_3gpp_rmsm_ip_common_process_entitlement_check_failed_cmd */



/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_TRANSITION_STATE

 DESCRIPTION
   
   This function is used to maintain the state of RmSm IP Common.

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_transition_state
(
  dsumts_rmsm_rm_dev_instance_type    rm_dev_instance,
  ds_3gpp_rmsm_ip_common_state_type    new_state
)
{
  dsat_connect_rate_s_type    connect_rate; /* Report with next CONNECT */ 
  ds_umts_pdp_profile_type    *profile = NULL; /* Primary PDP profile */
#ifdef FEATURE_DATA_A2
  sint15                      ps_errno = -1; /* PS err number - needed for 
                                                IOCTL */
  sio_port_id_type            sio_port_id = -1;  
#endif /*FEATURE_DATA_A2*/
  ds_umts_pdp_auth_type       pdata;
  dsumts_rmsmi_info_type*    rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* v4_sm_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* v6_sm_info_ptr = NULL;
  int retval = -1;
#ifdef FEATURE_DATA_TE_MT_PDP
  dsat_pdp_answer_data_type   ans_cb_data; /* Holds cb data from ATCOp*/
  dsat_result_enum_type       result = DSAT_OK;
#endif /* FEATURE_DATA_TE_MT_PDP */
  dsumts_rmsm_instance_type  inst;
  dsumts_rmsm_instance_type  temp_inst;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ppp_dev_enum_type          ppp_dev;
  ds_umts_pdp_type_enum_type pdp_type;

  /*----------------------------------------------------------------------*/

  inst.rm_dev_instance = temp_inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DSUMTS_RMSM_INVALID_SM_INSTANCE; 

  memset( (void*)&connect_rate, 0, sizeof(dsat_connect_rate_s_type) );
  memset( (void*)&pdata, 0, sizeof(ds_umts_pdp_auth_type) );

  DS_3GPP_MSG1_MED("Entered dsumts_rmsmi_transition_state,"
                   " rm_dev_instance: %d", rm_dev_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  ASSERT(rmsmi_info_ptr != NULL);
  ASSERT(rmsmi_info_ptr->sm_type == DSUMTS_RMSM_SM_TYPE_PDP_IP);

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);


  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if (pdp_type == DS_UMTS_PDP_IPV4 || 
      pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    temp_inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
    v4_sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&temp_inst);
    ASSERT(v4_sm_info_ptr);
  }

  if (pdp_type == DS_UMTS_PDP_IPV6 ||
      pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    temp_inst.sm_instance = DSUMTS_RMSM_V6_SM_INSTANCE;
    v6_sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&temp_inst);
    ASSERT(v6_sm_info_ptr);
  }

#ifdef FEATURE_DATA_TE_MT_PDP
  /*------------------------------------------------------------------------- 
    Bug trap: 
    (1) check whether RMSM is locked
    RMSM gets locked when it exists NULL state
  -------------------------------------------------------------------------*/
    ASSERT( rmsmi_info_ptr->is_locked );
#endif /*FEATURE_DATA_TE_MT_PDP*/


  DS_3GPP_MSG3_HIGH("Changing the State of RmSm IP common from %d to %d", 
                     rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state,
                     new_state,
                     rmsmi_info_ptr->port_id);  

  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state = new_state;

  DS_3GPP_MSG1_MED("Entering %d state",new_state);

  switch (new_state)
  {
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    {
      rmsmi_info_ptr->subs_id =   SYS_MODEM_AS_ID_1;

      /*------------------------------------------------------------------
      If we got to NULL state as a result of a failure from ent check,
      we need no further clean up here as pp would not have been started and
      no event registration would have happened
      ------------------------------------------------------------------*/
      if(rmsmi_info_ptr->entitlement_check_req == TRUE)
      {
        DS_3GPP_MSG0_MED("No further cleanup action needed. "
                         "Reset ent_chk_reg flag");
        rmsmi_info_ptr->entitlement_check_req = FALSE;
        /* Client can also end the call while entitlement check is in progress */
        dsumts_rmsmi_sio_cleanup(rmsmi_info_ptr->instance);
        return;
      }  
#ifdef FEATURE_DATA_WCDMA_PS
      //Deregister with FC. We need not monitor CPU usage anymore
      DATA_MSG0_HIGH( "Deregistering with FC for CPU stats");
      dsumts_rmsmi_dl_fc_deregister();
#endif /*  FEATURE_DATA_WCDMA_PS */

      /*---------------------------------------------------------------------
        Pkt call is terminating, Perform all the necessary cleanup
      ---------------------------------------------------------------------*/
      ds3g_siolib_ex_set_inbound_flow( DS_FLOW_UMTS_RMSM_MASK, DS_FLOW_DISABLE,
                                       rmsmi_info_ptr->port_id );
      if (ds3g_siolib_ex_set_cd_state
           (DS3G_SIOLIB_REMOTE_CARRIER_OFF,rmsmi_info_ptr->port_id ) == 
            DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_3GPP_MSG0_ERROR("Error setting CD State");  
      }

      if (ds3g_siolib_ex_change_mode
            ( SIO_DS_AUTODETECT_MODE, NULL, NULL, NULL, rmsmi_info_ptr->port_id)
           == DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_3GPP_MSG0_ERROR("Error changing mode to autodetect");
      }
      
      if (rmsmi_info_ptr->user_cancel == TRUE)
      {
        /*-------------------------------------------------------------------
          User pressed cancel before the call came up. Send DSAT_OK result
          code to TE.
        -------------------------------------------------------------------*/
        dsat_send_result_code_ex( DSAT_OK,rmsmi_info_ptr->port_id  );
        rmsmi_info_ptr->user_cancel = FALSE;
      }
      else
      {
        dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
      }

      /*---------------------------------------------------------------------
        Reset the PPP state and deregister the callbacks
      ---------------------------------------------------------------------*/
      if (pdp_type == DS_UMTS_PDP_IPV4 || pdp_type == DS_UMTS_PDP_IPV4V6  )
      {
         retval = ppp_event_deregister
                 (
                   rmsmi_info_ptr->ppp_handle,
                   PPP_PROTOCOL_IPCP,
                   (ppp_event_mask_e_type) (PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK)
                 );

         if (retval == -1)
         {
           DS_3GPP_MSG0_ERROR("Unable to deregister IPCP events");
         }

        /*---------------------------------------------------------------------
        Free any IPCP packets we have stored & set sm specific info
        dsm_free_packet will set the pointer to NULL
        ---------------------------------------------------------------------*/
        DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

        if(info_ptr->v4_info.ipcp_cfg_pkt_ptr != NULL )
        {
          dsm_free_packet(&info_ptr->v4_info.ipcp_cfg_pkt_ptr);
        }

        DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

        ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);

      }

      if (pdp_type == DS_UMTS_PDP_IPV6 || pdp_type == DS_UMTS_PDP_IPV4V6)
      {
        retval = ppp_event_deregister
                (
                  rmsmi_info_ptr->ppp_handle,
                  PPP_PROTOCOL_IPV6CP,
                  (ppp_event_mask_e_type) (PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK)
                );

        if (retval == -1)
        {
          DS_3GPP_MSG0_ERROR("Unable to deregister IPV6CP events");
        }

        /*---------------------------------------------------------------------
        Free any IPV6CP packets we have stored & set sm specific info
        dsm_free_packet will set the pointer to NULL
        ---------------------------------------------------------------------*/
        DSUMTS_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

        if(info_ptr->v6_info.ipv6cp_cfg_pkt_ptr != NULL )
        {
          dsm_free_packet(&info_ptr->v6_info.ipv6cp_cfg_pkt_ptr);
        }

        DSUMTS_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

        ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);

      }

      retval = ppp_event_deregister( rmsmi_info_ptr->ppp_handle,
                                     PPP_PROTOCOL_LCP,
                                     PPP_DOWN_EV_MASK );

      if (retval == -1)
      {
        DS_3GPP_MSG0_ERROR("Unable to deregister LCP events");
      }
      
      retval = ppp_abort( ppp_dev );

      if (retval == -1)
      {
        DS_3GPP_MSG0_ERROR("PPP Abort failed");
      }

      DSUMTS_RMSMI_MEM_FREE(rmsmi_info_ptr->rm_ppp_config_ptr, 
                            MODEM_MEM_CLIENT_DATA);

      rmsmi_info_ptr->rm_ppp_config_ptr = NULL;


      netmdl_set_meta_state( PPP_OOC_META_STATE );

      /*---------------------------------------------------------------------
        Cleanup local watermarks.
      ---------------------------------------------------------------------*/

      dsm_empty_queue(&rmsmi_info_ptr->sio_tx_wmk);
      dsm_empty_queue(&rmsmi_info_ptr->sio_rx_wmk);

      /*---------------------------------------------------------------------
        Indicate to siolib that we are not in call anymore, deregister
        all the event handlers and enable the flow to accept the next call
      ---------------------------------------------------------------------*/
      ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,FALSE );
      dsumts_rmsmi_siolib_deregister_callbacks(rmsmi_info_ptr->port_id);
      ds3g_siolib_ex_release_port ( rmsmi_info_ptr->port_id );


#ifdef FEATURE_DATA_TE_MT_PDP
      /*-----------------------------------------------------------------------
        As one instance of RMSM can support either IP_SM or PPP_SM, on entry into 
        IP_NULL or PPP_NULL state, release lock on RMSM instance
      -----------------------------------------------------------------------*/

     DSUMTS_CONDITIONAL_BLOCK_START( (rmsmi_info_ptr->mt_index != DSUMTSPS_MT_INVALID_TBL_INDEX), 
                                     "RMSM_IP_TRANS_STATE: failed at ring,"
                                     "minimal cleanup" );
     rmsmi_info_ptr->mt_te_ring = FALSE;
     
      if ( !dsumts_rmsm_reset_mt( rmsmi_info_ptr->mt_index ) )
      {
       DS_3GPP_MSG0_ERROR("RMSM_IP_TRANS_STATE: unable to "
                          "reset MT flag");
      }
     DSUMTS_CONDITIONAL_BLOCK_END();
     /*-------------------------------------------------------------------------
     Clear the lock in case of DUN also as we are locking in 
     ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev in case of DUN 
     -------------------------------------------------------------------------*/
      rmsmi_info_ptr->is_locked = FALSE;
#endif /*FEATURE_DATA_TE_MT_PDP*/

      ds3g_siolib_ex_set_inbound_flow( DS_FLOW_UMTS_RMSM_MASK|DS_FLOW_PKT_WM_MASK,
                                       DS_FLOW_ENABLE,
                                       rmsmi_info_ptr->port_id );

      dsumts_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance,DS3G_SIOLIB_PORT_NONE);

      break;
    }

#ifdef FEATURE_DATA_TE_MT_PDP
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:
    {
      
      /*-------------------------------------------------------------------------
        Perform following Entry actions:
         Set mt_te_ring
         Perform auto-answer if set
         Register handlers for ANS and HANGUP
         Register handlers for ATSO Auto-answer with SIOLIB 
         RING TE
      -------------------------------------------------------------------------*/

      /* This flag is set to indicate a different cleanup mech on
         reaching NULL state */
      rmsmi_info_ptr->mt_te_ring = TRUE;

      /* This will make sure that default profile is used */
      memset( &ans_cb_data, 0, sizeof( dsat_pdp_answer_data_type ) );
      ans_cb_data.number_of_profiles = 0;

      if ( dsat_cgauto_val == 1 ) 
      {
        DS_3GPP_MSG0_HIGH("RMSM_IP_TRANS_STATE: Rm ready for auto-ans! "
                          "RING TE and ANS immediately");
        /*-------------------------------------------------------------------------
          Send RING to indicate to TE of MT call and disable ATS0 auto-answer
          then call RMSM_MT_ANS routine
        -------------------------------------------------------------------------*/
        ds3g_siolib_alert_user( DSAT_RING, 
                                FALSE );
        dsumts_rmsm_mt_ans_cb( &ans_cb_data );
        break;
      }
      else
      {
        DS_3GPP_MSG0_HIGH("RMSM_IP_TRANS_STATE: Rm is not set "
                          "for auto-ans! reg ANS+HUP cb");
      }

      /* Register handler with ATCOP and SIOLIB. ATCOP callback functions 
       * must be registered before SIOLIB callback functions are registered 
       * to avoid a race condition in ATCOP.
       *
       * DSAT_OK: if registered successfully
       * DSAT_ERROR: if trying to register when +CGAUTO=1
       * DSAT_BUSY: if registered unsuccessfully
       */

      result = dsat_register_pdp_handlers( 
                 dsumts_rmsm_mt_ans_cb,
                 dsumts_rmsm_mt_hangup_cb );

      if ( result == DSAT_BUSY ) 
      {
        DS_3GPP_MSG0_HIGH("RMSM_IP_TRANS_STATE: cannot reg "
                          "ANS+HUP CBs, reject MT call!");

        /* Post RM_REJECT_MT_PKT_CALL_EV to itself */
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV);

        return;
      }
      else if ( result == DSAT_ERROR ) 
      {
        /* dsat_cgauto_val has been checked and found not set, trap this */
        DATA_ERR_FATAL("RMSM_IP_TRANS_STATE: err! trying to register when +CGAUTO=1");
        return;
      }
      else if ( result == DSAT_OK ) 
      {
        DS_3GPP_MSG0_HIGH("RMSM_IP_TRANS_STATE: ANS and HUP registered"
                          "successfully!");
      }
      else
      {
        DATA_ERR_FATAL("RMSM_IP_TRANS_STATE: err! unknown ret code");
        return;
      }

      ds3g_siolib_set_answer_callbacks
        ( &ds_3gpp_rmsm_ip_common_siolib_func_tbl );

      DS_3GPP_MSG0_HIGH("RMSM_IP_TRANS_STATE: RING TE! "
                        "with SIOLIB auto-ans set");
      ds3g_siolib_alert_user( DSAT_RING, 
                              TRUE ); /*Perform ATS0 auto-answer*/
      break; 
    }
#endif /*FEATURE_DATA_TE_MT_PDP*/
    
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    {
    
      /*---------------------------------------------------------------------
        Send CONNECT result code immediately & Kick start the PPP on the 
        Rm interface & do not bring the call on the Um interface yet. 
        Start PPP in  Passive & INTERNAL_SNOOP mode. Register with PPP to 
        snoop for IPCP config request packet.
        
        Note: If we know that we do not have PS service, we can send a 
        "NO CARRIER" result code immediately as an optimization. 
      ---------------------------------------------------------------------*/
  
      /*---------------------------------------------------------------------
        Return the extended result code with CONNECT.
        \V0 and &E0: (CONNECT?<radio zone speed>)
        \V1 and &E0 (CONNECT?<radio zone speed> <communication protocol> [<text>])
        \V1 and &E1 (CONNECT?<TAF-TE speed> <communication protocol> [<text>])
        <text>:APN/MaxBitRateUplink/MaxBitRateDownlink
      ---------------------------------------------------------------------*/
      /* Always return PACKET for PS */
      connect_rate.call_type = DSAT_CALL_TYPE_PS;
      connect_rate.comm_prot = DSAT_COMM_PROT_PACKET;
      /* Return APN from the sm_ptr profile details */
      memscpy( (void*)connect_rate.rate.ps_call.apn,
               (DS_UMTS_MAX_APN_STRING_LEN+1),
               (void*)rmsmi_info_ptr->profile_context_info.apn,
               (DS_UMTS_MAX_APN_STRING_LEN+1) );      
  
      profile = (ds_umts_pdp_profile_type *)modem_mem_alloc(
                    sizeof(ds_umts_pdp_profile_type),
                    MODEM_MEM_CLIENT_DATA_CRIT );

      if (profile == NULL ) 
      {
        DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for ds_3gpp_rmsm_ip_common_transition_state");
        return;
      }

      memset( (void*)profile, 0, sizeof(ds_umts_pdp_profile_type) );
  
      /*---------------------------------------------------------------------
        We do not have the NW allocated data rates yet, so return data rates 
        from requested QOS or default if requested QOS is set to subscribed
      ---------------------------------------------------------------------*/
      (void)ds_umts_get_pdp_profile_all_data_per_subs((uint16)rmsmi_info_ptr->profile_id,
                                             dsumts_subs_mgr_get_subs_id(
                                                       rmsmi_info_ptr->subs_id),
                                             profile);
      /*Report data rate in bps */
  #ifdef FEATURE_DATA_LTE
      if(profile->qos_request_lte.valid_flg == TRUE)
      {
        connect_rate.rate.ps_call.dl_rate =
          profile->qos_request_lte.max_dl_bit_rate*1000;
        connect_rate.rate.ps_call.ul_rate =
          profile->qos_request_lte.max_ul_bit_rate*1000;
      }
      else 
  #endif /* FEATURE_DATA_LTE */
      if(profile->qos_request_umts.valid_flg)
      {
        connect_rate.rate.ps_call.dl_rate = 
            profile->qos_request_umts.max_dl_bitrate*1000;
        connect_rate.rate.ps_call.ul_rate = 
            profile->qos_request_umts.max_ul_bitrate*1000;
      }
      else if(profile->qos_request_gprs.valid_flg) 
      {
        /*-----------------------------------------------------------------
          Max Bit rate:
          8    kbps : peak throughput class = 1  ( 1k bytes )
          16   kbps : peak throughput class = 2  ( 2k bytes )
          32   kbps : peak throughput class = 3  ( 4k bytes )
          64   kbps : peak throughput class = 4  (8 k bytes )
          128  kbps : peak throughput class = 5  (16 k bytes )
          256  kbps : peak throughput class = 6  (32 k bytes )
          512  kbps : peak throughput class = 7  (64 k bytes )
          1024 kbps : peak throughput class = 8  (128 k bytes )
          2048 kbps : peak throughput class = 9  (256 k bytes )
        -----------------------------------------------------------------*/
        switch (profile->qos_request_gprs.peak)
        {
          case QOS_PEAK_TPUT_1KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_8KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_8KBPS;
            break;
          case QOS_PEAK_TPUT_2KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_16KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_16KBPS;
            break;
          case QOS_PEAK_TPUT_4KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_32KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_32KBPS;
            break;
          case QOS_PEAK_TPUT_8KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_64KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_64KBPS;
            break;
          case QOS_PEAK_TPUT_16KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_128KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_128KBPS;
            break;
          case QOS_PEAK_TPUT_32BYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_256KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_256KBPS;
            break;
          case QOS_PEAK_TPUT_64KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_512KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_512KBPS;
            break;
          case QOS_PEAK_TPUT_128KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_1024KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_1024KBPS;
            break;
          case QOS_PEAK_TPUT_256KBYTES:
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_2048KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_2048KBPS;
            break;
          default: /* return 64 kbps as default value for GPRS */
            connect_rate.rate.ps_call.ul_rate  = MAX_BIT_RATE_64KBPS;
            connect_rate.rate.ps_call.dl_rate  = MAX_BIT_RATE_64KBPS;
        }
      }
  #ifdef FEATURE_DATA_LTE
      if ( connect_rate.rate.ps_call.ul_rate == 0 )
      { /* return default value */
        connect_rate.rate.ps_call.dl_rate = MAX_BIT_RATE_150000KBPS;
        connect_rate.rate.ps_call.ul_rate = MAX_BIT_RATE_50000KBPS;
      }
  #else
      if ( connect_rate.rate.ps_call.ul_rate == 0 )
      { /* return default value */
        connect_rate.rate.ps_call.dl_rate = MAX_BIT_RATE_DEFAULT;
        connect_rate.rate.ps_call.ul_rate = MAX_BIT_RATE_DEFAULT;
      }
  #endif /* FEATURE_DATA_LTE */
  
      rmsmi_info_ptr->rm_ppp_config_ptr = 
        (ppp_dev_opts_type*) DSUMTS_RMSMI_MEM_ALLOC(sizeof(ppp_dev_opts_type), 
                                                     MODEM_MEM_CLIENT_DATA);
      if(NULL == rmsmi_info_ptr->rm_ppp_config_ptr)
      {
        ASSERT(0);
      }

      memset(rmsmi_info_ptr->rm_ppp_config_ptr, 0, sizeof(ppp_dev_opts_type));

      ppp_get_default_opts(rmsmi_info_ptr->rm_ppp_config_ptr);
  
      if (pdp_type == DS_UMTS_PDP_IPV4 || pdp_type == DS_UMTS_PDP_IPV4V6)
      {
         /*---------------------------------------------------------------------
          We enable WINS in IPCP configuration explicitly to ensure that 1x 
          is not affected by WINS. 
         ---------------------------------------------------------------------*/
        if( ds_3gpp_nv_manager_get_nbns_enabled(rmsmi_info_ptr->subs_id))
        {
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_REMOTE] |= IPCP_N_PRIMARY_NBNS ;
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_LOCAL]  |= IPCP_N_PRIMARY_NBNS ; 
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            want_mask[FSM_LOCAL]  |= IPCP_N_PRIMARY_NBNS ;
       
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_REMOTE] |= IPCP_N_SECONDARY_NBNS ;
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_LOCAL]  |= IPCP_N_SECONDARY_NBNS;
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            want_mask[FSM_LOCAL]  |= IPCP_N_SECONDARY_NBNS;
        }
  
        /*---------------------------------------------------------------------
        Disable VJ compression on the Rm link for both Rx and Tx
        ---------------------------------------------------------------------*/
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          will_mask[FSM_LOCAL]  &= ~IPCP_N_COMPRESS;
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          will_mask[FSM_REMOTE] &= ~IPCP_N_COMPRESS;
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          want_mask[FSM_LOCAL]  &= ~IPCP_N_COMPRESS;
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          want_mask[FSM_REMOTE] &= ~IPCP_N_COMPRESS;
  
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          rx_iface_ptr = &v4_sm_info_ptr->sio_iface;  
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          mode = PPP_CP_PASSIVE;

        ps_iface_enable_ind( &v4_sm_info_ptr->sio_iface );
        ps_iface_disable_flow( &v4_sm_info_ptr->sio_iface, 
                               DS_FLOW_UMTS_RMSM_MASK ); 
      }

      if (pdp_type == DS_UMTS_PDP_IPV6 || pdp_type == DS_UMTS_PDP_IPV4V6)
      {
        //VJ Compression is not supported for IPv6
        rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.rx_iface_ptr = 
          &v6_sm_info_ptr->sio_iface;  
        rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.mode = 
          PPP_CP_PASSIVE;
        ps_iface_enable_ind( &v6_sm_info_ptr->sio_iface );
        ps_iface_disable_flow( &v6_sm_info_ptr->sio_iface, 
                               DS_FLOW_UMTS_RMSM_MASK ); 
      }

      dsumts_rmsmi_setup_sio_watermarks (rm_dev_instance,
                                         &rmsmi_info_ptr->sio_tx_wmk,
                                         &rmsmi_info_ptr->sio_rx_wmk,
                                         &rmsmi_info_ptr->sio_tx_wmk_q,
                                         &rmsmi_info_ptr->sio_rx_wmk_q);
  
     rmsmi_info_ptr->rm_ppp_config_ptr->rx_f_ptr       
       = dsumts_rmsmi_sio_rx_ppp_data;
     rmsmi_info_ptr->rm_ppp_config_ptr->rx_user_data  
       = (void *)rmsmi_info_ptr;
     rmsmi_info_ptr->rm_ppp_config_ptr->tx_f_ptr       
       = dsumts_rmsmi_sio_tx_ppp_data;
     rmsmi_info_ptr->rm_ppp_config_ptr->tx_user_data   
       = (void *)rmsmi_info_ptr;
     
     rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.mode = PPP_CP_PASSIVE;
     rmsmi_info_ptr->rm_ppp_config_ptr->hdlc_mode     = HDLC_HW_PREF;
  
  #ifdef FEATURE_DATA_A2
     if ( (v4_sm_info_ptr != NULL) && 
          (&v4_sm_info_ptr->sio_iface != NULL) )
     {
       if(dsumts_rmsm_iface_ioctl_hdlr( &v4_sm_info_ptr->sio_iface,
                                     PS_IFACE_IOCTL_RM_GET_SIO_PORT,
                                     &sio_port_id,
                                     &ps_errno) == 0)
       {
         if(sio_port_id == SIO_PORT_A2_MDM)
         {
           rmsmi_info_ptr->rm_ppp_config_ptr->hdlc_mode = HDLC_A2_AUTONOMOUS;
         }
       }
     }
     else if ( (v6_sm_info_ptr != NULL) &&
               (&v6_sm_info_ptr->sio_iface != NULL) )
     {
       if(dsumts_rmsm_iface_ioctl_hdlr( &v6_sm_info_ptr->sio_iface,
                                        PS_IFACE_IOCTL_RM_GET_SIO_PORT,
                                        &sio_port_id,
                                        &ps_errno) == 0 )
       {
         if(sio_port_id == SIO_PORT_A2_MDM)
         {
           rmsmi_info_ptr->rm_ppp_config_ptr->hdlc_mode = HDLC_A2_AUTONOMOUS;
         }
       }
     }
     else
     {
     	 ASSERT(0);
     }
  #endif /* FEATURE_DATA_A2 */
     rmsmi_info_ptr->rm_ppp_config_ptr->bridge_dev     = PPP_UM_SN_DEV;
     
  
     netmdl_set_meta_state( PPP_OOC_META_STATE ); 


#ifdef FEATURE_DATA_TE_MT_PDP
     if( !rmsmi_info_ptr->is_mt )
#endif /* FEATURE_DATA_TE_MT_PDP */
     {
       DS_3GPP_MSG0_HIGH ("Not an MT call run PPP server");

     /*---------------------------------------------------------------------
       Turn ON the authentication on the Rm link. Attempt Chap, If peer
       rejects it attempt PAP or fallback to NO auth
     ---------------------------------------------------------------------*/
     rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
       will_mask[FSM_REMOTE] &= ~LCP_N_AP;
     rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
       want_mask[FSM_REMOTE] &= ~LCP_N_AP;
     (void)ppp_set_auth_server_opts(rmsmi_info_ptr->rm_ppp_config_ptr,
                                    "UMTS_CHAP_SRVR");
     }
#ifdef FEATURE_DATA_TE_MT_PDP
     /* For MT call PC acts as server and UE acts as client */
     else /* MT call: UE will act as client */
     {
       DS_3GPP_MSG0_HIGH ("MT call no PPP server");
       /* Make LCP active for MT call */
       rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.mode  = PPP_CP_ACTIVE;
       rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.mode = PPP_CP_ACTIVE;
  
       /*---------------------------------------------------------------------
         Turn on auth bits for remote.want and remote.will. Turn others off.
       ---------------------------------------------------------------------*/
      rmsmi_info_ptr->rm_ppp_config_ptr->
                       lcp_info.will_mask[FSM_REMOTE] |= LCP_N_AP;
      rmsmi_info_ptr->rm_ppp_config_ptr->
                       lcp_info.want_mask[FSM_REMOTE] |= LCP_N_AP;
      rmsmi_info_ptr->rm_ppp_config_ptr->
                       lcp_info.will_mask[FSM_LOCAL]  &= ~LCP_N_AP;
      rmsmi_info_ptr->rm_ppp_config_ptr->
                       lcp_info.want_mask[FSM_LOCAL]  &= ~LCP_N_AP; 
      rmsmi_info_ptr->rm_ppp_config_ptr->auths_peer = FALSE;
       /* Read authentication info from profile. This info will be used for 
          authentication negotiation with PC */
       if (ds_umts_get_pdp_profile_auth_info_per_subs((uint16)rmsmi_info_ptr->profile_id,
                            dsumts_subs_mgr_get_subs_id(rmsmi_info_ptr->subs_id),
                                             &pdata) != DS_UMTS_PDP_SUCCESS )
  
       {
         DS_3GPP_MSG1_HIGH ("Error PDP Auth profile read: %d,using CHAP/PAP"
                            " with default settings",
                             rmsmi_info_ptr->profile_id,0,0);
         rmsmi_info_ptr->rm_ppp_config_ptr->
          lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
              (ppp_auth_proto_e_type) ( (uint32)PPP_AUTH_PAP | (uint32)PPP_AUTH_CHAP );
       }
       else
       {
         DS_3GPP_MSG0_HIGH("Setting CHAP/PAP based on profile auth");
         rmsmi_info_ptr->rm_ppp_config_ptr->auth_info.user_id_len = 
                           strlen((char*)pdata.username);
         rmsmi_info_ptr->rm_ppp_config_ptr->auth_info.passwd_len = 
                           strlen((char*)pdata.password);
         strlcpy((char*)rmsmi_info_ptr->rm_ppp_config_ptr->
                 auth_info.user_id_info,(char*)pdata.username,
                 PPP_MAX_USER_ID_LEN);
         strlcpy((char*)rmsmi_info_ptr->rm_ppp_config_ptr->
                 auth_info.passwd_info,(char*)pdata.password,
                 PPP_MAX_PASSWD_LEN);
         if (pdata.auth_type == DS_UMTS_PDP_AUTH_PAP)
         {
           DS_3GPP_MSG0_HIGH("Using PAP protocol");
           rmsmi_info_ptr->rm_ppp_config_ptr.lcp_info.
                 want_vals[FSM_REMOTE].authentication = PPP_PAP_PROTOCOL;
    
         }
         else
         {
           DS_3GPP_MSG0_HIGH("Using CHAP protocol");
           rmsmi_info_ptr->rm_ppp_config_ptr.
                           lcp_info.want_vals[FSM_REMOTE].auth_proto_mask = 
                           (uint16)PPP_AUTH_CHAP;
         }
       }
     }
  
     if( !rmsmi_info_ptr->is_mt )
#endif /* FEATURE_DATA_TE_MT_PDP */
     {
  #ifdef FEATURE_GSM_DTM  
       if (rr_dtm_get_max_LAPDm_octets() != 0) {
         rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
           work_vals[FSM_LOCAL].authentication = PPP_PAP_PROTOCOL;
         rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
           want_vals[FSM_LOCAL].authentication = PPP_PAP_PROTOCOL;
         DS_3GPP_MSG0_HIGH("Using PAP protocol");
       }
       else
#endif /* FEATURE_GSM_DTM */
       {
         if (ds_umts_get_pdp_profile_auth_info_per_subs((uint16)rmsmi_info_ptr->profile_id,
                             dsumts_subs_mgr_get_subs_id(rmsmi_info_ptr->subs_id),
                                                &pdata) != DS_UMTS_PDP_SUCCESS )
         {
           DS_3GPP_MSG3_HIGH ("Error PDP Auth profile read: %d,using CHAP",
                               rmsmi_info_ptr->profile_id,0,0);
         }
         else
         {
           DS_3GPP_MSG0_HIGH("Setting CHAP/PAP based on profile auth");
           if (pdata.auth_type == DS_UMTS_PDP_AUTH_PAP)
           {
             rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
               work_vals[FSM_LOCAL].authentication =  PPP_PAP_PROTOCOL;
             rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
               want_vals[FSM_LOCAL].authentication = PPP_PAP_PROTOCOL;
             DS_3GPP_MSG0_HIGH("Using PAP protocol");
           }
           else
           {
             DS_3GPP_MSG0_HIGH("Using CHAP protocol");
           }
         }
       }
     }
      if ( ppp_start( ppp_dev, rmsmi_info_ptr->rm_ppp_config_ptr ) < 0 )
      {
        DS_3GPP_MSG0_ERROR("RM PPP config failed, posting RM_PPP_DOWN");
        dsumts_rmsm_post_event( &inst , UMTS_RMSM_RM_PPP_DOWN_EV );
        break;
      }
  
      DS_3GPP_MSG0_HIGH("Changing PPP to internal SNOOP mode" ); 
      /*---------------------------------------------------------------------
        Change the PPP mode on Rm to internal SNOOP Mode 
      ---------------------------------------------------------------------*/
      ppp_set_mode( ppp_dev, PPP_INTERNAL_SNOOP_MODE );
  
      /*---------------------------------------------------------------------
        Register for PPP events (Up and Down events )
      ---------------------------------------------------------------------*/
      retval = ppp_event_register( rmsmi_info_ptr->ppp_handle,
                                   PPP_PROTOCOL_LCP, 
                                   PPP_DOWN_EV_MASK  );

      if (retval == -1)
      {
        DS_3GPP_MSG0_ERROR("Unable to register for LCP events"); 
        dsumts_rmsm_post_event( &inst , UMTS_RMSM_RM_PPP_DOWN_EV );
        break;
      }
  
      if (pdp_type == DS_UMTS_PDP_IPV4 ||
          pdp_type == DS_UMTS_PDP_IPV4V6)
      {
        retval = ppp_event_register
                 (
                    rmsmi_info_ptr->ppp_handle,
                    PPP_PROTOCOL_IPCP, 
                    (ppp_event_mask_e_type)(PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK)
                  );

        if (retval == -1)
        {
          DS_3GPP_MSG0_ERROR("Unable to register for IPCP events"); 
          dsumts_rmsm_post_event( &inst , UMTS_RMSM_RM_PPP_DOWN_EV );
          break;
        }
        
        /*---------------------------------------------------------------------
          Snoop for IPCP packets: Register with ps snoop registry
        ---------------------------------------------------------------------*/
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v4_info.ipcp_snoop_tag
          = snoop_reg_event(ppp_dev, PPP_IPCP_PROTOCOL,
                            CONFIG_REQ, ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb);
      }

      if (pdp_type == DS_UMTS_PDP_IPV6 ||
          pdp_type == DS_UMTS_PDP_IPV4V6)
      {
        retval = ppp_event_register
                 (
                    rmsmi_info_ptr->ppp_handle,
                    PPP_PROTOCOL_IPV6CP, 
                   (ppp_event_mask_e_type) (PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK)
                  );

        if (retval == -1)
        {
          DS_3GPP_MSG0_ERROR("Unable to register for IPV6CP events"); 
          dsumts_rmsm_post_event( &inst , UMTS_RMSM_RM_PPP_DOWN_EV );
          break;
        }
        
        /*---------------------------------------------------------------------
           Snoop for IPV6CP packets: Register with ps snoop registry
        ---------------------------------------------------------------------*/
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v6_info.ipv6cp_snoop_tag 
          = snoop_reg_event(ppp_dev, PPP_IPV6CP_PROTOCOL,
                            CONFIG_REQ, ds_3gpp_rmsm_ip_common_rm_ipv6cp_cfg_req_cb);
      }

      /*--------------------------------------------------------------------- 
        Send connect response now and immediately change to packet mode; on
        receiving connect TE can start sending PPP frames and we need to be
        ready to receive them
      ---------------------------------------------------------------------*/
      dsat_report_rate_on_connect( connect_rate );
  
      dsat_send_result_code_ex( DSAT_CONNECT,rmsmi_info_ptr->port_id );
      (void)ds3g_siolib_ex_set_cd_state
             (DS3G_SIOLIB_REMOTE_CARRIER_ON,rmsmi_info_ptr->port_id);
  
      /*---------------------------------------------------------------------
        Rm configured : Set Rm to PKT mode & send result code to kick start
        PPP from the TE 
      ---------------------------------------------------------------------*/
      (void)ds3g_siolib_ex_change_mode( SIO_DS_PKT_MODE,
                                        &rmsmi_info_ptr->sio_rx_wmk,
                                        &rmsmi_info_ptr->sio_tx_wmk,
                                        NULL,
                                        rmsmi_info_ptr->port_id  );
  #ifdef FEATURE_QXDM_WM_TRACING
        //Register RMSM client with FC
        dsumts_rmsmi_register_with_fc(rmsmi_info_ptr->instance);
  #endif /*FEATURE_QXDM_WM_TRACING*/
  
      SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
  
        break;
    }

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
      SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
      break;

    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
      break;

    default:
      DS_3GPP_MSG1_ERROR("Cannot recognize the state %d", 
                         rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      ASSERT(0);
      break;
  }
  modem_mem_free((void *)profile, MODEM_MEM_CLIENT_DATA_CRIT );
} /* ds_3gpp_rmsm_ip_common_transition_state */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          GLOBAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_INIT

  DESCRIPTION
    This function initializes the RmSm IP Common Data Structure
    
  PARAMETERS
    Rm Device Instance
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ip_common_init
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance
)
{
  dsumts_rmsmi_info_type           *rmsmi_info_ptr = NULL;

  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  DS_3GPP_MSG1_MED("Initializing RmSm IP Common, "
                   "rm_dev_instance: %d", rm_dev_instance);  

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return FALSE;
  }

  info_ptr = (ds_3gpp_rmsm_ip_common_info_type*) 
             DSUMTS_RMSMI_MEM_ALLOC(sizeof(ds_3gpp_rmsm_ip_common_info_type), 
                                     MODEM_MEM_CLIENT_DATA );

  if(info_ptr == NULL)
  {
    ERR_FATAL("MEM ALLOC Failure: RmSm ip common Info ptr is NULL during init",0,0,0);
    return FALSE;
  }

  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr = info_ptr;

  /*-------------------------------------------------------------------------
    COMMON Initialization
  -------------------------------------------------------------------------*/

  info_ptr->state = DS_3GPP_RMSM_IP_COMMON_NULL_STATE;

  memset(&info_ptr->ppp_info, 0x0, sizeof(info_ptr->ppp_info));

  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->configure_ppp_f_ptr = 
    ds_3gpp_rmsm_ip_common_configure_ppp_cb;

  /*-------------------------------------------------------------------------
    V4 Initialization
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Initialize the rex critical section for ip info sm structure.
  -------------------------------------------------------------------------*/
  memset(&(info_ptr->v4_info.ipi_sm_info_crit_sec),
           0,
           sizeof(rex_crit_sect_type));
  DSUMTS_INIT_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );


  info_ptr->v4_info.ipcp_cfg_pkt_ptr = NULL;
  info_ptr->v4_info.ipcp_snoop_tag   = 0;
  info_ptr->v4_info.send_ipcp_naks_flg = FALSE;
  rex_def_timer_ex(&(info_ptr->v4_info.ipcp_res_del_timer),
                     ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb , 
                     (unsigned long )rmsmi_info_ptr );
  ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);

  /*-------------------------------------------------------------------------
    V6 Initialization
  -------------------------------------------------------------------------*/
  memset(&(info_ptr->v6_info.ipi_sm_info_crit_sec),
           0,
           sizeof(rex_crit_sect_type));
  DSUMTS_INIT_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );


  info_ptr->v6_info.ipv6cp_cfg_pkt_ptr = NULL;
  info_ptr->v6_info.ipv6cp_snoop_tag   = 0;
  info_ptr->v6_info.send_ipv6cp_naks_flg = FALSE;
  rex_def_timer_ex(&(info_ptr->v6_info.ipv6cp_res_del_timer),
                     ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cb , 
                     (unsigned long )rmsmi_info_ptr );
  ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);

 
  return TRUE;
} /* ds_3gpp_rmsm_ip_common_init */

#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_PROCESS_UM_WANTS_MT_PKT_CALL_EV

  DESCRIPTION
    This function processes UM_WANTS_MT_PKT_CALL_EV event. 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_um_wants_mt_pkt_call_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{

  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;

  DS_3GPP_MSG0_MED("RMSM_PROCESS_MT: UM wants MT PDP-IP pkt call");

  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("Inst ptr is NULL !");
	  return;
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("Sm Info ptr is NULL !");
    return;
  }

  /*-------------------------------------------------------------------------
    This event is valid only in DS_3GPP_RMSM_IP_NULL_STATE
    Perform following Exit Actions:
    (1).Mark RMSM for MT PDP then,
    (2).transition to DS_3GPP_RMSM_RINGING_TE
  -------------------------------------------------------------------------*/
  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
      /* Not required now as we are keeping it locked. 
         Locking rmsm as soon as we received incoming call */
      rmsmi_info_ptr->is_locked = TRUE;

      if ( !dsumts_rmsm_set_mt( rmsmi_info_ptr->mt_index ) )
      {
        DATA_ERR_FATAL("RMSM_IP_MT_PKT_CALL: unable to set MT flag");
      }
      DS_3GPP_MSG0_HIGH("RMSM_IP_PKT_CALL_EV: transition to RMSM_IP_RINGING_TE");
      ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance, 
                                      DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE );
    break;

    default:
      DS_3GPP_MSG1_ERROR("RMSM_PROCESS_MT: UM_WANTS_MT_PKT_CALL_EV is invalid in state:%d:",
                rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
  }
}/*dsumts_rmsm_ip_process_um_wants_mt_pkt_call_ev*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_PROCESS_RM_REJECT_MT_PKT_CALL_EV()

  DESCRIPTION
   

  PARAMETERS
    sm_ptr: State Machine info pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_reject_mt_pkt_call_ev
(
  dsumts_rmsm_instance_type *inst_ptr
)
{
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance = DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;

  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  rm_dev_instance = inst_ptr->rm_dev_instance;
  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  DS_3GPP_MSG0_MED("RMSM_IP_PROC_RM_REJ: Rejects MT PDP-IP pkt call");

  /*------------------------------------------------------------------------- 
    Bug trap: 
    (1) check that RMSM is marked for MT
  -------------------------------------------------------------------------*/
  ASSERT( rmsmi_info_ptr ||rmsmi_info_ptr->is_mt );

  /*-------------------------------------------------------------------------
    Exit actions:
    (1) Deregister ANS+HANGUP callbacks
    (2) Deregister SIOLIB auto-ans callbacks
    (3) Post REJECT_EV to MT-UMSM
    (then, Transition over to new state DS_3GPP_RMSM_IP_NULL_STATE
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG1_MED("RMSM_IP_PROC_RM_REJ: mt_index [%d], stop RING, dereg DSAT+SIOLIB hdlrs",
          rmsmi_info_ptr->mt_index);

  ds3g_siolib_stop_alerting();

  dsat_deregister_pdp_handlers();
  
  ds3g_siolib_set_answer_callbacks( NULL );
    
  dsumtsps_mt_umsm_post_ev( rmsmi_info_ptr->mt_index, 
                            MT_UMSM_CALL_REJECT_EV, NULL );

  DS_3GPP_MSG1_MED("RMSM_IP_PROC_RM_REJ: mt_index [%d], transition to RMSM_IP_NULL_STATE",
          rmsmi_info_ptr->mt_index);
 
  ds_3gpp_rmsm_ip_common_transition_state( rm_dev_instance,
                                    DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
}/* dsumts_rmsm_ip_process_rm_reject_mt_pkt_call_ev() */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_RM_UM_LOCKED_EV()

  DESCRIPTION
   If MT call is somehow answered by embedded application we will clear the 
   MT call request at tethered port.

  PARAMETERS
    sm_ptr: State Machine info pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ip_common_process_rm_um_locked_ev
(
  dsumts_rmsm_instance_type *inst_ptr
)
{
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance = DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;

  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  rm_dev_instance = inst_ptr->rm_dev_instance;
  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  DS_3GPP_MSG0_MED("RMSM_IP_PROC_UM_LOCKED: Um locked event");

  /*------------------------------------------------------------------------- 
    Bug trap: 
    (1) check that RMSM is marked for MT
  -------------------------------------------------------------------------*/
  ASSERT( rmsmi_info_ptr ||rmsmi_info_ptr->is_mt );
  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return;
  }

  if (inst_ptr->sm_instance >= DSUMTS_RMSM_MAX_SM_INSTANCES)
  { 
    DATA_ERR_FATAL("Sm Instance Invalid !");
    return;
  }
  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:

      /*-------------------------------------------------------------------------
      Exit actions:
      (1) Deregister ANS+HANGUP callbacks
      (2) Deregister SIOLIB auto-ans callbacks
      (then, Transition over to new state DS_3GPP_RMSM_IP_NULL_STATE
      -------------------------------------------------------------------------*/
      DS_3GPP_MSG1_MED("RMSM_IP_PROC_UM_LOCKED: mt_index [%d], stop RING, dereg DSAT+SIOLIB hdlrs",
              rmsmi_info_ptr->mt_index);
     
      ds3g_siolib_stop_alerting();
     
      dsat_deregister_pdp_handlers();
     
      ds3g_siolib_set_answer_callbacks( NULL );
     
      ds_3gpp_rmsm_ip_common_transition_state( rm_dev_instance,
                                        DS_3GPP_RMSM_IP_COMMON_NULL_STATE );

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
     ds_3gpp_rmsm_ip_common_transition_state( rm_dev_instance,
                                       DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
     break;
    default:
      DS_3GPP_MSG1_ERROR("Um locked received in incorrect state: %d ",rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;
  }
  
}/* dsumts_rmsm_ip_process_rm_reject_mt_pkt_call_ev() */

#endif /*FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_WANTS_PKT_CALL_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance ptr

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  ds_sys_event_dun_ctrl_entitlement_check_info_type ent_chk_info;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  int16 ps_errno;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inst_ptr == NULL)
  {
    DATA_ERR_FATAL("Inst ptr is NULL");
    return;
  }

  DS_3GPP_MSG2_MED("Handle pkt call, rm_dev_instance: %d, sm_instance: %d ", 
                   inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
    return;
  }

DS_3GPP_MSG3_MED("Handle Rm Wants pkt call EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   inst_ptr->rm_dev_instance, 
                   inst_ptr->sm_instance, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
#ifdef FEATURE_DATA_TE_MT_PDP
      rmsmi_info_ptr->is_locked = TRUE;
      /*------------------------------------------------------------------------- 
        Bug trap: 
        (1) check that RMSM is NOT marked for MT as this event is not allowed
            in NULL state for MT marked RMSM
      -------------------------------------------------------------------------*/
      ASSERT( !rmsmi_info_ptr->is_mt );
#endif /*FEATURE_DATA_TE_MT_PDP*/

      /*---------------------------------------------------------------------
        Send indication to DS_SYS_IF for ent_chk
      ---------------------------------------------------------------------*/
      memset(&ent_chk_info, 0, 
             sizeof(ds_sys_event_dun_ctrl_entitlement_check_info_type));

      ent_chk_info.tech_type = DS_SYS_TECH_3GPP;
      ent_chk_info.cb_data   = (unsigned char)(inst_ptr->rm_dev_instance);

      rmsmi_info_ptr->entitlement_check_req = TRUE;
      ds_3gpp_rmsm_ip_common_transition_state(inst_ptr->rm_dev_instance, 
                   DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE );

      if (ps_sys_event_ind_ex(PS_SYS_TECH_3GPP,
                              PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                              (ps_sys_subscription_enum_type)
                              rmsmi_info_ptr->subs_id,
                              &ent_chk_info,
                              &ps_errno) == -1)
      {
        DS_3GPP_MSG1_ERROR("Error in sending sys_event ent_chk. ps_errno: %d",
                            ps_errno);

        dsumts_rmsm_post_event( inst_ptr , UMTS_RMSM_ENTITLEMENT_CHECK_FAILED_EV );
      }

      break;

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
      /*-----------------------------------------------------------------
        Ent check is done. reset Flag
      ------------------------------------------------------------------*/
      rmsmi_info_ptr->entitlement_check_req = FALSE;
      ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance, 
                             DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE );
      break;
			 
#ifdef FEATURE_DATA_TE_MT_PDP
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:
      /*------------------------------------------------------------------------- 
        Perform exit actions from DS_3GPP_RMSM_IP_RINGING_TE state
      -------------------------------------------------------------------------*/

      /*------------------------------------------------------------------------- 
        Bug trap: 
        (1) check that RMSM is locked
        (2) check that RMSM is marked for MT
      -------------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("RMSM_IP_PKT_CALL_EV: check RMSM is locked, is_locked [%d]", 
              rmsmi_info_ptr->is_locked);
      ASSERT( rmsmi_info_ptr->is_locked );

      DS_3GPP_MSG1_HIGH("RMSM_IP_PKT_CALL_EV: check RMSM is for MT, is_mt [%d]", 
              rmsmi_info_ptr->is_mt);
      ASSERT( rmsmi_info_ptr->is_mt );

      /*-------------------------------------------------------------------
        Transition state to Waiting for IPCP config req state
      -------------------------------------------------------------------*/
      ds_3gpp_rmsm_ip_common_transition_state(inst_ptr->rm_dev_instance, 
                              DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE);
      break;

#endif /* FEATURE_DATA_TE_MT_PDP */
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
      /*---------------------------------------------------------------------
        It's error to get this event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Unexpected RM_WANTS_PKT_CALL_EV state:%d", 
           rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RM_WANTS_PKT_CALL_EV state:%d", 
           rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      ASSERT( 0 );
      break;

  }
} /* ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_CONFIGURED_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance ptr

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{

  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inst_ptr == NULL)
  {
    DATA_ERR_FATAL("Inst ptr is NULL");
    return;
  }

  DS_3GPP_MSG2_MED("Handle pkt call, rm_dev_instance: %d, sm_instance: %d ", 
                   inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi Info ptr is NULL");
    return;
  }

  DS_3GPP_MSG3_MED("Handle Rm PPP Configured EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   inst_ptr->rm_dev_instance, 
                   inst_ptr->sm_instance, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
      ds_3gpp_rmsm_ip_common_transition_state
      ( 
        inst_ptr->rm_dev_instance, 
        DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE 
      );
      break;

    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
      /*---------------------------------------------------------------------
        It's error to get this event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Unexpected RM_WANTS_PKT_CALL_EV state:%d", 
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RM_WANTS_PKT_CALL_EV state:%d", 
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      ASSERT( 0 );
      break;

  } 

} /*ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_IFACE_DOWN_EV

  DESCRIPTION
    This function processes UM_IFACE_DOWN_EV event. 
    
  PARAMETERS
    Instance ptr

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_um_iface_down_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ps_iface_type           *um_iface_ptr = NULL;

  ds_sys_event_dun_ctrl_silent_redial_check_info_type redial_chk_info;
  int16 ps_errno;
  ppp_dev_enum_type                ppp_dev;
  ppp_protocol_e_type protocol = PPP_PROTOCOL_MAX;
  /*----------------------------------------------*/

  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;
  /*----------------------------------------------*/

  DS_3GPP_MSG2_MED("Handle Um Iface Down, rm_dev_instance: %d, sm_instance: %d ",
                    inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);

  if (inst_ptr->sm_instance >= DSUMTS_RMSM_MAX_SM_INSTANCES)
  { 
    DATA_ERR_FATAL("Sm Instance Invalid !");
    return;
  }

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("Sm Info ptr is NULL !");
    return;
  }

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

 DS_3GPP_MSG3_MED("Handle Um Iface Down EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   inst_ptr->rm_dev_instance, 
                   inst_ptr->sm_instance, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    {
      /*---------------------------------------------------------------------
       Send indication to DS_SYS_IF for redial_chk
      ---------------------------------------------------------------------*/
      memset(&redial_chk_info, 0, sizeof(redial_chk_info));

      redial_chk_info.tech_type = DS_SYS_TECH_3GPP;
      redial_chk_info.cb_data   = (unsigned char)(inst_ptr->rm_dev_instance);
        
      /* Get the iface down reason */
      um_iface_ptr = sm_info_ptr->um_iface_ptr;
        
      if (PS_IFACE_IS_VALID(um_iface_ptr))
      {
        redial_chk_info.net_down_reason
          = um_iface_ptr->event_info_cache.iface_down_info.netdown_reason;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Invalid um_iface_ptr, filling net_down_reason unspecified");
        redial_chk_info.net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
      }
        
      if (ds_3gpp_rmsm_ip_common_is_call_tearable(rmsmi_info_ptr))
      {
        if (ps_sys_event_ind_ex(PS_SYS_TECH_3GPP,
                              PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                              (ps_sys_subscription_enum_type)
                              rmsmi_info_ptr->subs_id,
                              &redial_chk_info,
                              &ps_errno) == -1)
        {
          DS_3GPP_MSG1_ERROR("Error in sending sys_event redial_chk. ps_errno: %d ",
                            ps_errno);
        }
        else
        {
          dsumts_rmsm_ip_process_um_iface_down_ev(sm_info_ptr);
          ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance, 
                      DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE );
          break;
        }
      }
    }
     // Fall through
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    {
#ifdef FEATURE_DATA_TE_MT_PDP
      DSUMTS_CONDITIONAL_BLOCK_START( rmsmi_info_ptr->is_mt, 
                                      "RMSM_IP_UM_IFACE_DOWN: is_mt set!" );

      if ( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state == 
             DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE )
      {
        /* Call is not UP yet, send REJECT to MT-UMSM */
        dsumtsps_mt_umsm_post_ev( rmsmi_info_ptr->mt_index, 
                                  MT_UMSM_CALL_REJECT_EV , NULL );
      }
      else if ( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state == 
                      DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE )
      {
        DS_3GPP_MSG0_HIGH("Nothing to do here UMSM is cleared already in UM iface up.");
      }
      else
      {
        DS_3GPP_MSG1_ERROR("IP_PROCESS_UM_IFACE_DOWN: wrong state [%d]",
                  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
        return;
      }

      DSUMTS_CONDITIONAL_BLOCK_END( );
#endif /*FEATURE_DATA_TE_MT_PDP*/

      if (V4_SM_INSTANCE(inst_ptr->sm_instance))
      {
        if (IPCP_CREQ_RECD(info_ptr) && IPV6CP_CREQ_RECD(info_ptr) )
        {
          protocol = PPP_PROTOCOL_IPCP;
          ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
        }

        else if (IPCP_CREQ_RECD(info_ptr))
        {
          protocol = PPP_PROTOCOL_LCP;
        }
      }

      else if (V6_SM_INSTANCE(inst_ptr->sm_instance))
      {
        if (IPCP_CREQ_RECD(info_ptr) && IPV6CP_CREQ_RECD(info_ptr) )
        {
          protocol = PPP_PROTOCOL_IPV6CP;
          ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);
        }

        else if (IPV6CP_CREQ_RECD(info_ptr))
        {
          protocol = PPP_PROTOCOL_LCP;
        }
      }

      if (protocol == PPP_PROTOCOL_LCP)
      {
        if( !ppp_stop(ppp_dev, protocol) )
        {
          DS_3GPP_MSG0_HIGH("Stopping PPP ");
          ds_3gpp_rmsm_ip_common_transition_state(inst_ptr->rm_dev_instance, 
                            DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE);
        }
 
        else
        {
          DS_3GPP_MSG0_HIGH("PPP stop failed. Transition to NULL state");
          ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance,
                                           DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
        }
      }
      
      dsumts_rmsm_ip_process_um_iface_down_ev(sm_info_ptr); 
    
      break;
      
    }

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
      dsumts_rmsm_ip_process_um_iface_down_ev(sm_info_ptr);
      
      if (ds_3gpp_rmsm_ip_common_is_call_tearable(rmsmi_info_ptr))
      {
      ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance, 
                                           DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
      }

      break;

    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring UM_IFACE_DOWN_EV RmSm Instance :%d state:%d",
                         rmsmi_info_ptr->instance,
                         rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;

    default:
      DS_3GPP_MSG0_ERROR("Um down received in incorrect state: %d ");
       break;
  }
} /* ds_3gpp_rmsm_ip_common_process_um_iface_down_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_UP_EV

  DESCRIPTION
    This function processes UM_IFACE_UP_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_um_iface_up_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;

  /*-----------------------------------------*/

  DS_3GPP_MSG2_MED("Handle Um Iface Up, rm_dev_instance: %d, sm_instance: %d ",
                    inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return;
  }

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("Sm Info ptr is NULL !");
    return;
  }
  DS_3GPP_MSG3_MED("Handle Um Iface Up EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   inst_ptr->rm_dev_instance, 
                   inst_ptr->sm_instance, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:

      ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
    
      dsumts_rmsm_ip_process_um_iface_up_ev(sm_info_ptr); 

      break;
  
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
       /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring UM_IFACE_UP_EV RmSm instance: %d state:%d",
                          rmsmi_info_ptr->instance, 
                          rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

    default:
       DS_3GPP_MSG0_ERROR("Um Up received in incorrect state: %d ");
       break;
  
   }
} /* ds_3gpp_rmsm_ip_common_process_um_iface_up_ev */


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_DOWN_EV

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;
  dsumts_rmsm_sm_instance_type sm_inst;
  dsumts_rmsm_rm_dev_instance_type rm_dev_inst;
  dsumts_rmsm_instance_type        v4_inst;
  dsumts_rmsm_instance_type        v6_inst;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ds_umts_pdp_type_enum_type       pdp_type;
  /*----------------------------------------*/

  DS_3GPP_MSG2_MED("Handle PPP Down, rm_dev_instance: %d, sm_instance: %d ",
                    inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr);

  sm_inst = inst_ptr->sm_instance;
  rm_dev_inst = inst_ptr->rm_dev_instance;

  DS_3GPP_MSG3_MED("Handle Rm PPP Down EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   rm_dev_inst, sm_inst, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  if ((V4_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
  { 
    ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
  }

  if ((V6_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
  {
    ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);
  }

  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
#ifdef FEATURE_DATA_TE_MT_PDP
      /* Call is not UP yet, send REJECT to MT-UMSM */
      dsumtsps_mt_umsm_post_ev( rmsmi_info_ptr->mt_index, MT_UMSM_CALL_REJECT_EV, NULL );
#endif /*FEATURE_DATA_TE_MT_PDP*/
     /*Fall through*/

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
      ds_3gpp_rmsm_ip_common_transition_state(rmsmi_info_ptr->instance, 
                                             DS_3GPP_RMSM_IP_COMMON_NULL_STATE);
      break;

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
      ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
      //Fallthrough

    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    {
      if ((V4_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
      {
        v4_inst.rm_dev_instance = inst_ptr->rm_dev_instance;
        v4_inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

        ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
        sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&v4_inst);
      dsumts_rmsm_ip_process_rm_ppp_down_ev(sm_info_ptr);
      }
	  
      if ((V6_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
      {
        v6_inst.rm_dev_instance = inst_ptr->rm_dev_instance;
        v6_inst.sm_instance = DSUMTS_RMSM_V6_SM_INSTANCE;

        ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
        sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&v6_inst);
      dsumts_rmsm_ip_process_rm_ppp_down_ev(sm_info_ptr);
      }


      if ( !IPCP_CREQ_RECD(info_ptr) && !IPV6CP_CREQ_RECD(info_ptr) )
      {
      ds_3gpp_rmsm_ip_common_transition_state(rmsmi_info_ptr->instance,
                             DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE);
      }

      break;
    }
    default:
     DS_3GPP_MSG1_ERROR( "PPP Down received in invalid state %d", 
                         rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
     break;
  }
} /* ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_UP_EV

  DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;

  /*----------------------------------------*/

  DS_3GPP_MSG2_MED("Handle PPP Up, rm_dev_instance: %d, sm_instance: %d ",
                    inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
  }

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("Sm Info ptr is NULL !");
  }

  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    { 
      dsumts_rmsm_ip_process_rm_ppp_up_ev(sm_info_ptr);
      break;
    }

    default:
      DS_3GPP_MSG1_ERROR( "PPP UP received in invalid state %d", 
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;

  }
} /* ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_PROCESS_NO_REDIALS_REMAINING_EV()
  
  DESCRIPTION
  This function processes UMTS_RMSM_NO_REDIALS_REMAINING_EV event. 
  
  PARAMETERS
  instance ptr
  
  RETURN VALUE
  None
  
  DEPENDENCIES
  none
  
  SIDE EFFECTS
  None
 ===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_no_redials_remaining_ev
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
  ppp_dev_enum_type                ppp_dev;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return;
  }

  sm_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("SM ptr is NULL !");
    return;
  }
  
  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);
  
  DS_3GPP_MSG2_HIGH( "Recvd NO_REDIALS_REMAINING_EV, Rm instance: %d state:%d", 
                     rmsmi_info_ptr->instance,
                     rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
  
  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
      
      /*---------------------------------------------------------------------
        All entitlement redials have been exhausted. Redial callback returned
        failure. Clean up the Rm.
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Terminate Rm PPP. Normally this happens as part of Um iface down
        processing, but because there is an extra state introduced for redial
        it must be done here.
       ---------------------------------------------------------------------*/
      if( !ppp_stop(ppp_dev, PPP_PROTOCOL_LCP) )
      {
        DS_3GPP_MSG0_HIGH( "Stopping PPP ");
        ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance,
                       DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE );
      }
      else
      {
        DS_3GPP_MSG0_HIGH( "PPP stop failed. Move to NULL State ");
        ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance,
                                        DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
        
      }     

      break;
      
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
       /*---------------------------------------------------------------------
        Ignore event in all other states.
        ---------------------------------------------------------------------*/
       DS_3GPP_MSG2_HIGH( "Ignoring NO_REDIALS_REMAINING_EV "
                          "Rm Instance :%d state:%d", rmsmi_info_ptr->instance, 
                           rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
       break;
       
    default:
      
      /*---------------------------------------------------------------------
       Invalid state
       ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
          rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      ASSERT( 0 );
      break;
      
  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state */
  
} /* dsumts_rmsm_ip_process_no_redials_remaining_ev() */


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_HANDLE_RM_BRING_UP_UM_IFACE_EV
  
  DESCRIPTION
  This function processes RM_BRING_UP_UM_IFACE_EV event. 
  
  PARAMETERS
  instance ptr
  
  RETURN VALUE
  None
  
  DEPENDENCIES
  none
  
  SIDE EFFECTS
  None
 ===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_bring_up_um_iface_ev 
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
  ds_umts_pdp_type_enum_type          pdp_type;
  ds_3gpp_rmsm_ip_common_info_type*   info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr);

  sm_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);
  ASSERT(sm_ptr); 

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("SM ptr is NULL !");
    return;
  }

  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  DS_3GPP_MSG3_HIGH( "Recvd Rm Bring Up Um Iface EV, "
                     "Rm instance: %d, SM instance: %d, state:%d", 
                     rmsmi_info_ptr->instance,
                     sm_ptr->instance,
                     rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  DS_3GPP_MSG2_HIGH( "Recvd Rm Bring UP Um iface EV, Rm instance: %d state:%d", 
                     rmsmi_info_ptr->instance,
                     rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE:
      DS_3GPP_MSG3_HIGH( "In entitlement, WAITING_FOR_REDIAL_CHECK_STATE, "
                         "transition to WAITING_FOR_UM_UP",
                          0, 0, 0 );
      //fallthrough

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    {
      dsumts_rmsm_ip_process_bring_up_um_iface_ev(sm_ptr);
      ds_3gpp_rmsm_ip_common_transition_state 
      ( 
        rmsmi_info_ptr->instance, 
        DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE
      );
      break;
    }

    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    {
      dsumts_rmsm_ip_process_bring_up_um_iface_ev(sm_ptr);
      break;
    }

    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE:
       /*---------------------------------------------------------------------
        Ignore event in all other states.
        ---------------------------------------------------------------------*/
       DS_3GPP_MSG2_HIGH( "Ignoring Rm Bring Up Um Iface Ev Rm Instance :%d state:%d",
                rmsmi_info_ptr->instance, rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
       break;

    default:

      /*---------------------------------------------------------------------
       Invalid state
       ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
          rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state */
}

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to PDP-IP type RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
 	  inst_ptr: Pointer to the Rm Dev Instance and SM Instance
    event: The event that is being posted to the state machine

  RETURN VALUE
    None

  DEPENDENCIES
    dsumts_rmsm_ip_sm_init() should have been called first.
    The top level state machine post function checks if we the state machine
    is initialized in the first place

  SIDE EFFECTS
    None
===========================================================================*/
/*lint -e818*/
void ds_3gpp_rmsm_ip_common_post_event
(
  dsumts_rmsm_instance_type* inst_ptr,
  dsumts_rmsm_event_type     event
)
{
  ds_cmd_type                *cmd_ptr = NULL;  /* DS Task cmd buf        */
  ds_cmd_enum_type            cmd_id;          /* Identifies the command */
  dsumts_rmsm_instance_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently
    in.
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case UMTS_RMSM_RM_WANTS_PKT_CALL_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL;
      break;

    case UMTS_RMSM_RM_PPP_CONFIGURED_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED;
      break;

    case UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_BRING_UP_UM_IFACE;
      break;

    case UMTS_RMSM_RM_PPP_UP_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_PPP_UP;
      break;

    case UMTS_RMSM_RM_PPP_DOWN_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_PPP_DOWN;
      break;

    case UMTS_RMSM_UM_IFACE_UP_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_IFACE_UP;
      break;

    case UMTS_RMSM_UM_IFACE_DOWN_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_IFACE_DOWN;
      break;

#ifdef FEATURE_DATA_TE_MT_PDP
    case UMTS_RMSM_UM_WANTS_MT_PKT_CALL_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_WANTS_MT_PKT_CALL;
      break;

    case UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV:
      cmd_id = DS_CMD_3GPP_RMSM_RM_REJECT_MT_PKT_CALL;
      break;

    case UMTS_RMSM_UM_LOCKED_EV :
      cmd_id = DS_CMD_3GPP_RMSM_UM_LOCKED;
      break;

#endif /*FEATURE_DATA_TE_MT_PDP*/

    case UMTS_RMSM_UM_IFACE_DOWN_ARB_EV:
      cmd_id = DS_CMD_3GPP_RMSM_UM_IFACE_DOWN_ARB;
      break;

    case UMTS_RMSM_NO_REDIALS_REMAINING_EV:
      cmd_id = DS_CMD_3GPP_RMSM_NO_REDIALS_REMAINING;
      break;

    case UMTS_RMSM_ADDR_CFG_COMPLETE_EV:
      cmd_id = DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE;
      break;

    case UMTS_RMSM_ADDR_CFG_END_EV:
      cmd_id = DS_CMD_3GPP_RMSM_ADDR_CFG_END;
      break;

    case UMTS_RMSM_ADDR_CFG_FAILURE_EV:
      cmd_id = DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL;
      break;

    case UMTS_RMSM_ADDR_CFG_CHANGED_EV:
      cmd_id = DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED;
      break;

    case UMTS_RMSM_ENTITLEMENT_CHECK_FAILED_EV:
      cmd_id = DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_FAIL;
      break;

    default:
      DS_3GPP_MSG3_ERROR( "Invalid UMTS RMSM SM instance:%d, "
                          "Rm Dev instance:%d, event %d, ignoring", 
                          inst_ptr->sm_instance,
                          inst_ptr->rm_dev_instance,
                          event);

      /*lint -e718 -e746 */
      ASSERT(0);
      /*lint +e718 +e746 */
      return;
  } /* switch(event) */

  cmd_ptr = ds_allocate_cmd_buf(sizeof(dsumts_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  cmd_ptr->hdr.cmd_id = cmd_id;
  data_ptr = (dsumts_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;

  data_ptr->rm_dev_instance = (int)inst_ptr->rm_dev_instance;
  data_ptr->sm_instance = (int)inst_ptr->sm_instance;

  ds_put_cmd( cmd_ptr );
} /* ds_3gpp_rmsm_ip_common_post_event() */
/*lint +e818 */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_3GPP_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it

  PARAMETERS
    inst_ptr: Pointer to the Rm Dev Instance and SM Instance
    cmd id to be processed

  RETURN VALUE
    none

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
/*lint -e818*/
void ds_3gpp_rmsm_ip_common_process_cmd
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_cmd_enum_type  cmd
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (inst_ptr == NULL)
  {
    DATA_ERR_FATAL("Inst ptr is NULL");
  	/*lint -e527*/
	  return;
	  /*lint +e527*/
  }
    
  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently
    in.
  -------------------------------------------------------------------------*/
  switch( cmd )
  {
    case DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL:
      ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED:
      ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_IFACE_UP:
      ds_3gpp_rmsm_ip_common_process_um_iface_up_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_IFACE_DOWN:
      ds_3gpp_rmsm_ip_common_process_um_iface_down_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_PPP_UP:
      ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_PPP_DOWN:
      ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_NO_REDIALS_REMAINING:
      ds_3gpp_rmsm_ip_common_process_no_redials_remaining_ev (inst_ptr);
        break;

    case DS_CMD_3GPP_RMSM_RM_BRING_UP_UM_IFACE:
      ds_3gpp_rmsm_ip_common_process_rm_bring_up_um_iface_ev (inst_ptr);
      break;


#ifdef FEATURE_DATA_TE_MT_PDP
    case DS_CMD_3GPP_RMSM_UM_WANTS_MT_PKT_CALL:
      ds_3gpp_rmsm_ip_common_process_um_wants_mt_pkt_call_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_REJECT_MT_PKT_CALL:
      ds_3gpp_rmsm_ip_common_process_rm_reject_mt_pkt_call_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_LOCKED:
      dsumts_rmsm_ip_common_process_rm_um_locked_ev(inst_ptr);
      break;
#endif /*FEATURE_DATA_TE_MT_PDP*/

    case DS_CMD_3GPP_RMSM_IPCP_RSP_TIMER:
      ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_IPV6CP_RSP_TIMER:
      ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_IPCP_CREQ_RECD:
      ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_IPV6CP_CREQ_RECD:
      ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_FAIL:
      ds_3gpp_rmsm_ip_common_process_entitlement_check_failed_cmd(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_IFACE_DOWN_ARB:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_END:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED:
      dsumts_rmsm_ip_process_cmd(inst_ptr, cmd);
        break;

    default:
      DS_3GPP_MSG3_ERROR("Invalid UMTS RMSM sm instance: %d, "
                         "rm dev instance: %d, cmd %d, ignoring", 
                          inst_ptr->sm_instance, 
                          inst_ptr->rm_dev_instance, cmd);
      ASSERT(0);
  } /* switch( cmd ) */
  return;
} /* ds_3gpp_rmsm_ip_common_process_cmd() */
/*lint +e818*/

LOCAL boolean ds_3gpp_rmsm_ip_common_is_call_tearable
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
)
{
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
  ds_umts_pdp_type_enum_type pdp_type;
  dsumts_rmsm_instance_type inst;
  boolean ret1 = TRUE;
  boolean ret2 = TRUE;

  if (rmsmi_info_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Invalid RmSm info ptr, returning false",0,0,0);
    return FALSE;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  
  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if (pdp_type == DS_UMTS_PDP_IPV4 ||
      pdp_type == DS_UMTS_PDP_IPV4V6)
  {  
    inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
    sm_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);
    ASSERT(sm_ptr);

    if (dsumts_rmsm_ip_get_state(sm_ptr) == DSUMTS_RMSM_IP_NULL_STATE)
    {
      ret1 = TRUE;
    }
    else
    {
      ret1 = FALSE;
    }
  }

  if (pdp_type == DS_UMTS_PDP_IPV6 ||
      pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    inst.sm_instance = DSUMTS_RMSM_V6_SM_INSTANCE;
    sm_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);
    ASSERT(sm_ptr);

    if (dsumts_rmsm_ip_get_state(sm_ptr) == DSUMTS_RMSM_IP_NULL_STATE)
    {
      ret2 = TRUE;
    }
    else
    {
      ret2 = FALSE;
    }
  }

  return (ret1 && ret2);

}

boolean ds_3gpp_rmsm_ip_common_allow_companion_iface_bringup
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  ds_pdn_context_s           *pdn_cntx_p = NULL;
  ds_3gpp_iface_s            *ds3gpp_iface_p = NULL;
  ds_apn_ip_support_type_e   ip_support;
  dsumts_rmsmi_sm_info_type  *sm_ptr = NULL;
  ps_iface_type              *iface_ptr = NULL;
  dsumts_rmsmi_info_type       *rmsmi_info_ptr = NULL;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ds_umts_pdp_type_enum_type     pdp_type;

  /*-----------------------------------------*/

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  ASSERT(rmsmi_info_ptr != NULL);

  sm_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  ASSERT(sm_ptr != NULL);
  ASSERT(sm_ptr->um_iface_ptr != NULL);

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr);

  iface_ptr = sm_ptr->um_iface_ptr;

/*-------------------------------------------------------------------------
    Get the DS Iface pointer from the PS Iface. Once this is done, get the
    PDN context pointer.
-------------------------------------------------------------------------*/
  ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_ptr->client_data_ptr);
  ASSERT (ds3gpp_iface_p != NULL);

  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);

  if (pdn_cntx_p == NULL)
  {
    DS_3GPP_MSG3_ERROR("PDN Context is NULL",0,0,0);
    return FALSE;
  }
  
  if ( (pdp_type == DS_UMTS_PDP_IPV4V6) &&
       (ds_pdn_cntxt_get_ip_support_info_for_pdn(pdn_cntx_p, &ip_support) == TRUE) &&
       (ip_support == DS_IPV4V6) )
  {
     return TRUE;
  }

  return FALSE;
}


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_GET_STATE()

  DESCRIPTION
    This function retrieves the RMSM IP common state.

  PARAMETERS
    inst: Rm Dev Instance

  RETURN VALUE
    ds_3gpp_rmsm_ip_common_state_type

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
ds_3gpp_rmsm_ip_common_state_type ds_3gpp_rmsm_ip_common_get_state
(
  dsumts_rmsm_rm_dev_instance_type  inst
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr;
  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
    return 0;
  }

  return (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
} /* ds_3gpp_rmsm_ip_common_get_state */

#endif /*#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)*/
