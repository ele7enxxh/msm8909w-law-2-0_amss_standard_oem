/******************************************************************************
  @file    ds_profile_3gppi.h
  @brief   Definitions associated with profile related functions for 3GPP. This
           is for internal use only

  DESCRIPTION
  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/api/ds_profile_3gppi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/11   ttv     Changes to add src_stat_desc as profile parameter.
05/31/11   rr      Added support for transient profiles
02/28/11   hs      Added changes to support APN class in profile
01/25/11   ss      SU API defeaturization and removal of customer.h.
01/17/11   ss      Added support for Iface linger.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
10/15/09   vk      Added support for LTE params
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/
#ifndef DS_PROFILE_3GPPI_H
#define DS_PROFILE_3GPPI_H

#include "datamodem_variation.h"
#include "list.h"
#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_tech_common.h"

#define DS_PROFILE_3GPP_PROFILE_NUM_MIN 1

/* Macro to check if identifier is valid */
#define DS_PROFILE_3GPP_IDENT_IS_VALID( ident ) \
  ( ( ident >= DS_PROFILE_3GPP_PROFILE_PARAM_MIN ) && \
    ( ident <= DS_PROFILE_3GPP_PROFILE_PARAM_MAX ) )

/* Internal list node */
typedef struct
{
  ds_profile_num_type   prf_num;
  char                  prf_name[DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN+1];
} ds_profile_3gpp_list_info_type;

typedef uint64 dsi_profile_3gpp_mask_type;

typedef PACKED struct PACKED_POST
{
  ds_profile_3gpp_pdp_context_number_type pdp_context_number; 
                                                     /* same as profile number */
  ds_profile_3gpp_pdp_type_enum_type      pdp_type;  /* PDP type (IP/PPP)      */
  ds_profile_3gpp_pdp_header_comp_e_type  h_comp;    /* PDP Header Comp support*/
  ds_profile_3gpp_pdp_data_comp_e_type    d_comp;    /* Data Comp       support*/
  ds_profile_3gpp_pdp_addr_type           pdp_addr;  /* PDP address            */ 
  byte                                    apn[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1];          
                                                    /* APN string              */
  ds_profile_3gpp_pdp_context_secondary_flag_type  secondary_flag;                         
                                                    /* this is a sec profile   */
  ds_profile_3gpp_pdp_context_primary_id_type      primary_id;
                                                    /* link to primary profile */ 
  ds_profile_3gpp_pdp_access_control_e_type	access_ctrl_flag;
                                                    /* TE MT PDP access cntrl  */
  ds_profile_3gpp_pdp_ipv4_addr_alloc_e_type ipv4_addr_alloc; 
                                                    /* IPv4 addr alloc mech.   */
}ds_profile_3gpp_pdp_context_type;

typedef ds_profile_linger_params_type ds_profile_3gpp_linger_params_type;

/*---------------------------------------------------------------------------
  PDP Profile complete structure definition
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  byte                                 version;          /* Version identifier */
  boolean                              read_only_flag;   /* Is context read-only*/
  char                                 profile_name[DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN+1];
                                                         /* Profile name in    */
  boolean ds_profile_3gpp_context_valid_flg;
  ds_profile_3gpp_ip_version_enum_type pdp_addr_ip_vsn;
  ds_profile_3gpp_pdp_context_type     context;          /* Context definition */
  
  ds_profile_3gpp_pdp_auth_type        auth;             /* Authentication info*/

  boolean ds_profile_3gpp_qos_req_3gpp_valid_flg;
  boolean ds_profile_3gpp_qos_min_3gpp_valid_flg;
  boolean ds_profile_3gpp_qos_req_gprs_valid_flg;   
  boolean ds_profile_3gpp_qos_min_gprs_valid_flg;
  boolean ds_profile_3gpp_qos_req_lte_valid_flg;
  ds_profile_3gpp_3gpp_qos_params_type qos_request_3gpp; /* 3GPP QOS params:req*/
  ds_profile_3gpp_3gpp_qos_params_type qos_minimum_3gpp; /* 3GPP QOS params:min*/
  ds_profile_3gpp_gprs_qos_params_type qos_request_gprs; /* GPRS QOS Params:req*/
  ds_profile_3gpp_gprs_qos_params_type qos_minimum_gprs; /* GPRS QOS params:min*/
  ds_profile_3gpp_lte_qos_params_type  qos_request_lte;  /* LTE QOS params:req */

  ds_profile_3gpp_ip_version_enum_type dns_addr_primary_ip_vsn;
  ds_profile_3gpp_ip_version_enum_type dns_addr_secondary_ip_vsn;
  ds_profile_3gpp_dns_addresses_type   dns_addr;         /* DNS addr.-user 
                                                            specfied           */
  boolean ds_profile_3gpp_tft_valid_flg[DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS]; 
  ds_profile_3gpp_tft_params_type      tft[DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS];
                                                         /* Traffic Flow 
                                                            Template           */
  boolean ds_profile_3gpp_linger_params_valid_flag;
  ds_profile_3gpp_linger_params_type   iface_linger_params;
                                                    /* Iface linger parameters */
  boolean                        otap_enabled_flag; /* Is context OTA      */
                                                    /* provisionable       */
  byte                           otap_napid[DS_PROFILE_3GPP_MAX_OTAP_NAPID_LEN+1];
                                        /* Network Access Point Identifier */
                                        /* in UTF8 format with variable    */
                                        /* length encoding                 */
  boolean                              request_pcscf_address_flag; 
                                               /* Flag to indicate if pcscf */
                                               /* address should be         */
                                               /* requested in PCO for      */
                                               /* this profile.             */
  boolean                              request_pcscf_address_using_dhcp_flag;
                                               /* Flag to indicate if pcscf */
                                               /* address should be         */
                                               /* requested using DHCP for  */
                                               /* this profile.             */
  boolean                              request_dns_address_using_dhcp_flag;
                                               /* Flag to indicate if dns */
                                               /* address should be         */
                                               /* requested using DHCP for  */
                                               /* this profile.             */
  boolean                              im_cn_flag; 
                                               /* Flag to indicate if im_cn */
                                               /* flag should be requested  */
                                               /* for this profile.         */
  ds_profile_3gpp_apn_disable_flag_type          apn_disable_flag;
                                                 /* APN disable flag        */
  ds_profile_3gpp_inactivity_timer_val_type      inactivity_timer_val;
                                                 /* Value of inactvity timer*/
  ds_profile_3gpp_apn_class_type                 apn_class;
                                                 /* APN class               */
   boolean                                       transience_flag;
                                         /* Flag to indicate if the profile */
                                         /* is written to EFS or not        */
  uint32                                         src_stat_desc_req;
                                           /* Source Statistics Descriptor for
                                              requested QoS */
  uint32                                         src_stat_desc_min;
                                           /* Source Statistics Descriptor for
                                              minimum QoS */
  uint8                                          apn_bearer;
                                              /* Indicates 3GPP RAT type for
                                                 profile                    */
  boolean                                  emergency_calls_are_supported;
                                           /* Flag to indicate if IMS emergency
                                              calls can be made using this profile */
  uint16                                        operator_reserved_pco;
                                                /* Indicates Operator Reserved Pco id to be used
                                                     in the Pco */
  uint16                                        mcc;
                                                /* Indicates Mcc to be used
                                                    in the Pco */
  ds_profile_3gpp_mnc_type                      mnc; 
                                                /* Indicates Mnc to be used
                                                     in the Pco */

  uint16                                max_pdn_conn_per_blk;
                                        /* Indicates max PDN Connections 
                                           that can be made per Time Blk */
                                       
  uint16                                pdn_req_wait_time;
                                        /* Specifies the minimum time 
                                           interval (secondss) between
                                           the new PDN connection request
                                           and the last successful UE
                                           initiated PDN disconnection. */
                                       
  uint16                                max_pdn_conn_time;   
                                        /* Specifies the time duration 
                                           in seconds during which the UE 
                                           counts the PDN connections
                                           already made. */      

}ds_profile_3gpp_profile_info_type;

/* Used internally to exchange 3GPP parameters */
typedef struct 
{
  dsi_profile_3gpp_mask_type         mask;
  ds_profile_3gpp_profile_info_type *prf;
  void                              *self;
}dsi_profile_3gpp_type;

/*---------------------------------------------------------------------------
  Table of callback functions for handling ds profile events. Each client
  of DS Profile can maintain such a table and take actions based on events.
---------------------------------------------------------------------------*/
typedef struct
{
  list_link_type link;     /* Pointer to the next item in list */
  /*-------------------------------------------------------------------------
    This handler is invoked when apn_disable_flag is changed
  -------------------------------------------------------------------------*/
  void (*apn_disable_flag_change_handler)
  (
    uint16   prof_num,
    sys_modem_as_id_e_type subs_id,
    boolean  apn_disable_flag
  ); 
  /*-------------------------------------------------------------------------
    This handler is invoked when apn name changes on the ds_profile
  -------------------------------------------------------------------------*/
  void (*apn_name_change_handler)
   (
    uint16 prof_num,
    sys_modem_as_id_e_type subs_id,
    void                  *data_ptr
   );

   void (*max_pdn_conn_per_blk_chg_hdlr)
   (
     uint16 prof_num,
     sys_modem_as_id_e_type subs_id,
     uint16 max_pdn_conn_per_blk
   );

   void (*pdn_req_wait_time_chg_hdlr)
   (
     uint16 prof_num,
     sys_modem_as_id_e_type subs_id,
     uint16 pdn_req_wait_time
   );

   void (*max_pdn_conn_time_chg_hdlr)
   (
     uint16 prof_num,
     sys_modem_as_id_e_type subs_id,
     uint16 max_pdn_conn_time
   );

   void (*roaming_disallowed_flag_chg_hdlr)
   (
     uint16  profile_num,
     sys_modem_as_id_e_type subs_id,
     boolean roaming_disallowed_flag
   );

   void (*pdp_type_chg_hdlr)
   (
     uint16                      profile_num,
     sys_modem_as_id_e_type subs_id,
     ds_umts_pdp_type_enum_type  new_pdp_type
   );

} ds_profile_3gpp_cb_func_tbl_type;

/*===========================================================================
FUNCTION DS_PROFILE_3GPP_GET_CALLBACK_TABLE_LIST
 
DESCRIPTION
  This function is used to get callback table list.

PARAMETERS
  void

DEPENDENCIES 
  
RETURN VALUE 
  Pointer to Callback Table List
 
SIDE EFFECTS 
 
===========================================================================*/
list_type* ds_profile_3gpp_get_callback_table_list
(  
  void
);

/*===========================================================================

FUNCTION DS_PROFILE_3GPP_REGISTER_CALLBACK_TABLE

DESCRIPTION
  This function registers a callback function table of a client
  with DS profile.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void ds_profile_3gpp_register_callback_table
(
  ds_profile_3gpp_cb_func_tbl_type  *cb_tbl_ptr /* Callback function table */
);

/*===========================================================================
FUNCTION DS_PROFILE_3GPP_DEREGISTER_CALLBACK_TABLE

DESCRIPTION
  This function deregisters a callback function table of a client
  with DS profile.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void ds_profile_3gpp_deregister_callback_table
(
  ds_profile_3gpp_cb_func_tbl_type  *cb_tbl_ptr /* Callback function table */
);

/*===========================================================================
FUNCTION DS_PROFILE_3GPP_INIT

DESCRIPTION
  This function is called on the library init. It initializes the function
  table pointers to valid functions for 3gpp

PARAMETERS 
  fntbl : pointer to function table

DEPENDENCIES 
  
RETURN VALUE 
  returns the mask for 3gpp. (Used later as valid mask which is ORed value
  returned from all techs.
SIDE EFFECTS 
  
===========================================================================*/
uint8 ds_profile_3gpp_init( 
  tech_fntbl_type *fntbl 
);

extern void dsi_profile_get_profile_num_range (
  ds_profile_tech_etype tech,
  uint16               *min_num,
  uint16               *max_num
);

#endif /* DS_PROFILE_3GPPI_H */
