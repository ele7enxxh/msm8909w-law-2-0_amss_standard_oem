#ifndef PDCP_H
#define PDCP_H
/*===========================================================================

                                   P D C P

                  E X T E R N A L   H E A D E R   F I L E


  DESCRIPTION
  This file contains external data declarations and function prototypes.
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/pdcp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/09   sch     Changed lc_info_ptr (pointer) to lc_info (struct) in 
                   pdcp_reg_dereg_wm_data_type
10/28/09   vs      Removed the inclusion of customer.h
10/15/09   vs      SU Level Api Defeaturization
08/31/09   ss      New header file for SU level apis.
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "ps_rohc.h"
#include "rrcdata.h"
#include "ps_iphc.h"
#include "ds_flow_control.h"
#include "dsumtspdpreg.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define MAX_COMP_ENG_TYPES 2                   /* IPHC and ROHC for now, 
                                                  increase this if more
                                                  comp eng types are added */

/*---------------------------------------------------------------------------
  Currently PDCP only supports 2 types of compression schemes: ROHC (RFC 3095)
  and IPHC (RFC 2507).
---------------------------------------------------------------------------*/
typedef enum
{
  PDCP_NO_COMP = 0x00,
  PDCP_ROHC = 0x01,
  PDCP_IPHC = 0x02,
  PDCP_INVALID_COMP
}pdcp_comp_eng_e_type;

/*---------------------------------------------------------------------------
  Enum used to modify respective flow masks.
---------------------------------------------------------------------------*/
typedef enum
{
  PDCP_TO_PS = 0,
  PDCP_TO_RLC = 1
}pdcp_flow_dir_e_type;

/*---------------------------------------------------------------------------
  maximum pdcp SN window size configured by UTRAN
---------------------------------------------------------------------------*/
typedef enum
{
  SN_255 = 0,
  SN_65535 = 1
}max_pdcp_sn_win_size_e_type;

/*---------------------------------------------------------------------------
  Whether PDCP PDU Header is present or not, default is 'present'
---------------------------------------------------------------------------*/
typedef enum
{
  ABSENT = 0,
  PRESENT = 1
}pdcp_pdu_header_e_type;

/*---------------------------------------------------------------------------
  whether algo shall reorder pdcp sdus, default is not expected
---------------------------------------------------------------------------*/
typedef enum
{
  REORDERING_NOT_EXPECTED = 0,
  REORDERING_EXPECTED = 1
} expect_reorder_e_type; 
  
/*---------------------------------------------------------------------------
  On UL/DL, whether ROHC CID values will be in ROHC_PACKET_FORMAT
---------------------------------------------------------------------------*/
typedef enum
{
  ROHC_PACKET_FORMAT = 0,
  ROHC_INVALID_CID_INC
}rohc_cid_inc_info_e_type;

/*---------------------------------------------------------------------------
  2507 compression engine specific information
  Derived from TS25.331 10.3.4.2
  iphc_f_max_period - largest num of comp non tcp headers that may be 
  sent without sending a full header, default is 255
  iphc_f_max_time - comp headers may not be sent more than this many sec 
  after sending last full header, default is 5
  iphc_max_header - largest header size in octets that may be comp, default 168
  iphc_tcp_space - max cid for tcp connections, default is 15 
  iphc_non_tcp_space - max cid for non-tcp connections, default is 15
  expect_reorder - whether algo shall reorder pdcp sdus, default 'not expected'
---------------------------------------------------------------------------*/
typedef struct
{
  pdcp_comp_eng_e_type        comp_eng_type;
  uint16                      iphc_f_max_period; 
  uint16                      iphc_f_max_time; 
  uint16                      iphc_max_header;  
  uint16                      iphc_tcp_space; 
  uint16                      iphc_non_tcp_space; 
  expect_reorder_e_type       expect_reorder;
}iphc_info_type;

/*---------------------------------------------------------------------------
  ROHC compression engine specific information
  Derived from TS25.331 10.3.4.2  
  rohc_ul_info_present - ul_info is optional hence this is required
  rohc_dl_info_present - dl_info is optional hence this is required
  rohc_profiles - profiles 1 - 3
  rohc_ul_cid_inc_info - which method to carry ROHC CID, 
  PDCP Header/ROHC pkt format
  rohc_ul_max_cid - highest cid num to be used by UE comp, default is 15
  rohc_ul_pkt_sizes_allowed - list of pkt sizes allowed by UE comp
  rohc_dl_cid_inc_info - which method to carry ROHC CID, 
  PDCP Header/ROHC pkt format
  rohc_dl_max_cid - default value is 15
  rohc_dl_rev_comp_depth - 0-65535, 0 means rev comp not used
---------------------------------------------------------------------------*/
typedef struct
{
  pdcp_comp_eng_e_type      comp_eng_type;
  boolean                   rohc_ul_info_present;
  boolean                   rohc_dl_info_present;
  rohc_profiles_e_type      rohc_profiles[ROHC_PROFILE_MAX];
  rohc_cid_inc_info_e_type  rohc_ul_cid_inc_info;
  uint16                    rohc_ul_max_cid; 
  uint32                    rohc_num_allowed_pkt_sizes;
  uint16                    rohc_ul_pkt_sizes_allowed[ROHC_MAX_NUM_ALLOWED_PKT_SIZES]; 
  rohc_cid_inc_info_e_type  rohc_dl_cid_inc_info;
  uint16                    rohc_dl_max_cid;
  uint32                    rohc_dl_rev_decomp_depth; 
}rohc_info_type;


/*---------------------------------------------------------------------------
  pdcp_info can have one or more comp eng info, hence this union is used.
---------------------------------------------------------------------------*/
typedef union
{
  iphc_info_type iphc_info;
  rohc_info_type rohc_info;
}comp_eng_u_type;

/*---------------------------------------------------------------------------
  Overall structure for pdcp_info used in cfg req between RRC and PDCP
  Derived from TS25.331 10.3.4.2
  pdcp_info_valid - pdcp_info is optional in rb_setup/reconfig, hence this parameter 
  is required
  support_for_lossless - whether lossless feature is supported
  pdcp_sn_win_size - configured by UTRAN 255 or 65535, 
  valid only if lossless support is true
  pdcp_pdu_hdr - whether pdcp pdu header is present or not, default is present
  comp_eng_info - pdcp_info can have one or more comp eng entries in this array
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                     pdcp_info_valid;
  boolean                     support_for_lossless; 
  max_pdcp_sn_win_size_e_type pdcp_sn_win_size;
  pdcp_pdu_header_e_type      pdcp_pdu_hdr;
  comp_eng_u_type             comp_eng_info[MAX_COMP_ENG_TYPES];
} pdcp_info_type;

/*---------------------------------------------------------------------------
  RRC can send one of the following config actions to PDCP
---------------------------------------------------------------------------*/
typedef enum
{
  SETUP_PDCP = 0,
  RECONFIG_PDCP = 1,
  RELEASE_PDCP = 2,
  RELEASE_ALL_PDCP = 3,
  RESET_COMP_ENG_PDCP = 4,
  SET_ROHC_TARGET_MODE_PDCP = 5,
  MAX_ACTION_PDCP
} pdcp_action_e_type;

/*---------------------------------------------------------------------------
  This flag is used when just reset comp eng is to be performed i.e. in case
  of ROHC, context reinitialization.  Used in the case of a HO.
---------------------------------------------------------------------------*/
typedef enum
{
  PDCP_NO_RESET_COMP_ENG = 0,
  PDCP_RESET_COMP_ENG = 1
}pdcp_flag_e_type;

/*---------------------------------------------------------------------------
  RRC sends following user data to PDCP
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                  rb_id;
  rlc_lc_id_type          rlc_dl_id;
  rlc_lc_id_type          rlc_ul_id;
  pdcp_info_type          pdcp_info;
  pdcp_action_e_type      action;
  pdcp_flag_e_type        srns_reloc_flag;
  uint32                  rab_dl_rate;
  uint32                  rab_ul_rate;
  rohc_mode_e_type        rohc_target_mode;
  boolean                 is_mcast_call;
  sys_modem_as_id_e_type  subs_id;  
} pdcp_cfg_req_data_type;                 /* data passed in by RRC to PDCP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Register WM command data type used by mode handler to register tx and rx wm.
  lc_info_ptr - pdcp_inst passed as part of this
  tx_wm_ptr - PS to PDCP WM
  rx_wm_ptr - PDCP to PS WM
  post_rx_func_ptr - function to be invoked when data arrives on PDCP to PS WM
  rx_func_para - parameter passed in when rx_func_ptr is invoked.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  uint8                    dl_lc_id;
  uint8                    ul_lc_id;
  dsm_watermark_type      *ps_to_pdcp_wm_ptr;
  dsm_watermark_type      *pdcp_to_ps_wm_ptr;
  void                    (*pdcp_to_ps_post_rx_func_ptr)(byte, byte, void *);
  void                    *pdcp_to_ps_rx_func_para;
  ds_umts_qos_tclass_enum_type traf_class;
} pdcp_reg_dereg_wm_data_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Data passed in as part of set_flow_mask cmd to set/reset rx_flow_mask,
  downlink data from PDCP to PS.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  uint8                pdcp_inst;
  pdcp_flow_dir_e_type flow_dir;
  ds3g_flow_e_type     flow_mask;
} pdcp_set_flow_mask_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      PDCP_GET_TRAFFIC_CLASS

DESCRIPTION   
  This function returns Traffic Class for given context.

PARAMETERS   
  rlc_dl_id - DL LC id for context .
  
DEPENDENCIES  
  None.

RETURN VALUE  
  ds_umts_qos_tclass_enum_type -- DL Traffic Class for the context.

SIDE EFFECTS  
  None.
===========================================================================*/
ds_umts_qos_tclass_enum_type  pdcp_get_traffic_class(rlc_lc_id_type rlc_dl_id);

/*===========================================================================
FUNCTION pdcp_VALIDATE_PARAMS

DESCRIPTION
  This primitive is used by RRC to validate pdcp_info parameters 
  sent by UTRAN for a given pdcp_instance.

PARAMETERS
  pdcp_info - the parameters to be validated

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS If the validation succeeded.
  E_FAILURE If the validation failed.

SIDE EFFECTS
  None

===========================================================================*/
boolean pdcp_validate_params
(
  pdcp_info_type pdcp_info,
  boolean        is_mcast_call
);

/*===========================================================================
FUNCTION      PDCP_GET_TRAFFIC_CLASS_SUB

DESCRIPTION   
  This function returns Traffic Class for given context.

PARAMETERS  
  subs_id   - CM subs id  
  rlc_dl_id - DL LC id for context .
  
DEPENDENCIES  
  None.

RETURN VALUE  
  ds_umts_qos_tclass_enum_type -- DL Traffic Class for the context.

SIDE EFFECTS  
  None.
===========================================================================*/
ds_umts_qos_tclass_enum_type  pdcp_get_traffic_class_sub
(
  sys_modem_as_id_e_type  subs_id,
  rlc_lc_id_type          rlc_dl_id
);

/*===========================================================================
FUNCTION pdcp_VALIDATE_PARAMS_SUB

DESCRIPTION
  This primitive is used by RRC to validate pdcp_info parameters 
  sent by UTRAN for a given pdcp_instance.

PARAMETERS
  subs_id -  Subscription ID
  pdcp_info - the parameters to be validated
  is_mcast_call - boolean mcast call


DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS If the validation succeeded.
  E_FAILURE If the validation failed.

SIDE EFFECTS
  None

===========================================================================*/
boolean pdcp_validate_params_sub
(
  sys_modem_as_id_e_type subs_id,
  pdcp_info_type         pdcp_info,
  boolean                is_mcast_call
);

#endif /* PDCP_H */
