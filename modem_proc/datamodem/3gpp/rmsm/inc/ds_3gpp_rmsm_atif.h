#ifndef DS_3GPP_RMSM_ATIF_H
#define DS_3GPP_RMSM_ATIF_H
/*===========================================================================

                          D S _ 3 G P P _ R M S M  _A T I F  H

DESCRIPTION
  This file contains the definitions for RM state machine for ATCoP 
  interactions for PS related commands. The functions in this files are
  used by ATCoP for +CGACT, +CGPADDR ,$QCDGEN commands.
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/ds_3gpp_rmsm_atif.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/25/12    ss    3GPP MH LTE-Off compilation fixes
09/13/10    sa    Fixed QCDGEN handler to send more data.
05/12/10    sa    Added support for dynamic parameter related commands.
03/15/10    sa    Added support for +CGCMOD command.
01/15/10    sa    Added support for $QCDGEN and +CGPADDR API.
12/11/09    sa    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#ifdef FEATURE_DATA

#ifdef FEATURE_DATA_3GPP
#include "dsat_v.h"
#include "ps_iface_defs.h"
#include "dsumtspdpregint.h"
#include "dsumtspdpreg.h"
#include "dsumtspspco.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_qos.h"


/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

typedef int ds_3gpp_rmsm_at_instance_type;


/*---------------------------------------------------------------------------
  Type for +CGACT modes of operation
---------------------------------------------------------------------------*/
typedef enum ds_3gpp_atif_cgact_act_mode
{
  DS_3GPP_ATIF_DEACTIVATION    = 0,         /* +CGACT activation     */
  DS_3GPP_ATIF_ACTIVATION      = 1,         /* +CGACT deactivation   */
  DS_3GPP_ATIF_MAX             = 0xFF       /* Internal use only !   */
} ds_3gpp_atif_cgact_act_mode_e_type;


/*---------------------------------------------------------------------------
  Type for PDP connection status
---------------------------------------------------------------------------*/
typedef enum ds_3gpp_atif_pdp_connect_state
{
  DS_3GPP_ATIF_PDP_DEACTIVATING  = 0,
  DS_3GPP_ATIF_PDP_DEACTIVATED   = 1,
  DS_3GPP_ATIF_PDP_ACTIVATING    = 3,
  DS_3GPP_ATIF_PDP_ACTIVATED     = 4,
  DS_3GPP_ATIF_PDP_MAX           = 0xFF      /* Internal use only ! */
} ds_3gpp_atif_pdp_connect_state_e_type;


/*---------------------------------------------------------------------------
  Type for Profile status and validity related information
---------------------------------------------------------------------------*/
typedef struct
{
  ds_3gpp_atif_pdp_connect_state_e_type   act_status;
  uint32                                 profile_num;
}ds_3gpp_atif_profile_status_type;


/*---------------------------------------------------------------------------
  Type for Profile status and validity for all contexts
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 profile_count;
  ds_3gpp_atif_profile_status_type  list[DS_UMTS_MAX_PDP_PROFILE_NUM];
}ds_3gpp_atif_profile_status_info_type;

/*---------------------------------------------------------------------------
  Structure to hold the dynamic params    
---------------------------------------------------------------------------*/

typedef struct
{
  uint32   cid;
  uint32   p_cid;
  byte          bearer_id;
}ds_3gpp_atif_secondary_dynamic_info_type;

typedef enum
{
  DS_3GPP_ATIF_PRIM_INFO = 0x0,
  DS_3GPP_ATIF_SEC_INFO,
  DS_3GPP_ATIF_QOS_INFO,
  DS_3GPP_ATIF_TFT_INFO,
  DS_3GPP_ATIF_DYNAMIC_MAX =0xFF
} ds_3gpp_atif_dynamic_e_type;

#define DS_3GPP_ATIF_MAX_PCSCF_SERVERS (2)
/* no of rx filters max + number of tx filters max */

#define DS_3GPP_MAX_FILTERS (2 *DS_TFT_MAX_FILTERS)

//#define DS_3GPP_MAX_FILTERS  16
typedef struct
{
  byte                           bearer_id;
  byte                           apn[DS_UMTS_MAX_APN_STRING_LEN+1];
  ds_umts_pdp_addr_type          ip_addr;
  ds3gpp_ipcp_options_s          v4_ipcp;
  ds3gpp_ipcp_options_s          v6_ipcp;
  ds_umts_pdp_addr_type          prim_pcscf_addr;
  ds_umts_pdp_addr_type          sec_pcscf_addr;
}ds_3gpp_atif_primary_dynamic_info_type;

typedef enum
{
  DS_3GPP_ATIF_PRE_REL7,
  DS_3GPP_ATIF_UPLINK,
  DS_3GPP_ATIF_DOWNLINK,
  DS_3GPP_ATIF_BIDIRECTIONAL
}ds_3gpp_atif_tft_direction_e_type;

typedef struct
{
  ds_tft_pkt_filter_direction_enum_type  direction;
  boolean                            nw_flow;
  ds_umts_tft_params_type            tft_params;
}ds_3gpp_atif_tft_info_type;


typedef struct {
  uint8    data_len;
  uint32    data_ptr[DS_3GPP_MAX_FLOW_CONTEXT];
}ds_3gpp_atif_flow_cid_list_type;

typedef struct {
  uint8    data_len;
  uint32    data_ptr[DS_3GPP_MAX_PDN_CONTEXT];
}ds_3gpp_atif_prim_profile_list_type;


typedef struct
{
  ds_3gpp_atif_dynamic_e_type info_type;
  union
  {
    ds_3gpp_atif_primary_dynamic_info_type   prim_dynamic_info;
    ds_3gpp_atif_secondary_dynamic_info_type  sec_dynamic_info;
    ds_3gpp_lte_qos_params_type   dynamic_qos;
    ds_3gpp_atif_tft_info_type   dynamic_tft[DS_3GPP_MAX_FILTERS];
  }u;
}ds_3gpp_atif_dynamic_params_info_type;



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGACT_QUERY
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the Status of PDP
    profiles during +CGACT command execution. The function returns the
    activation status of all the valid profiles. 
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: in case of SUCCESS
  
  SIDE EFFECTS
    None
===========================================================================*/

dsat_result_enum_type ds_3gpp_rmsm_atif_cgact_query
(
  ds_3gpp_atif_profile_status_info_type * profile_status_info_ptr,
  sys_modem_as_id_e_type                  subs_id 
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGACT_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for activation or deactivation of profiles
    requested by +CGACT Command. This function returns DSAT_OK if the profile 
    is already activated by other applications or by +CGACT. Only the profile 
    activated by +CGACT context can only be de-activated from activated state.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: if the activation state and current state of the profile is same.
    DSAT_ASYNC_CMD: Activation or de-activation is in process.
  
  SIDE EFFECTS
    None
===========================================================================*/

dsat_result_enum_type ds_3gpp_rmsm_atif_cgact_handler
(
  uint32                                  profile_number,
  ds_3gpp_atif_cgact_act_mode_e_type      act_state,
  void *                                  user_info_ptr,
  sys_modem_as_id_e_type                  subs_id
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_QCDGEN_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for sending uplink packets of data len as 
    requested by $QCDGEN Command. Only the PDP-IP profile activated by +CGACT 
    context can be used for data transfer. DSAT_NO_RSP will returned if
    data transfer is initiated. Once the data transfer is complete, 
    result code is send using dsat_send_result_code().
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK/DSAT_ASYNC_CMD: in case of Success

  SIDE EFFECTS
    None
===========================================================================*/

dsat_result_enum_type ds_3gpp_rmsm_atif_qcdgen_handler
(
  uint32                                  profile_number,
  sys_modem_as_id_e_type                  subs_id,
  uint32                                  total_data_len,
  void *                                  user_info_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_PDP_ADDR
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the PDP address
    of the activated profile. 
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: in case of SUCCESS
  
  SIDE EFFECTS
    None
===========================================================================*/

dsat_result_enum_type ds_3gpp_rmsm_atif_get_pdp_addr
(
  uint32                                  profile_number,
  sys_modem_as_id_e_type                  subs_id,
  ds_umts_pdp_addr_type                  *pdp_addr_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGCMOD_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for modification of profiles
    requested by +CGCMOD Command. This function returns DSAT_ERROR if the 
    profile is not activated by +CGACT. Only the profile.

  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_ASYNC_CMD: Modification is in process.
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_cgcmod_handler
(
  uint32                                profile_number,
  sys_modem_as_id_e_type                subs_id,
  void *                                user_info_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_READ_DYNAMIC_PARAMS
  
  DESCRIPTION
    This Function is called by ATCoP for reading the dynamic parameters 
    associated with the primary /secondary activated profiles .
    This function returns FALSE if the profile is not activated .

  DEPENDENCIES
    None.
  
  RETURN VALUE
    FALSE: in case of Failure
    TRUE: If retrieval of dynamic data is success.
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_atif_read_dynamic_params
(
  uint32                                      profile_number,
  sys_modem_as_id_e_type                      subs_id,
  ds_3gpp_atif_dynamic_e_type                 info_type,
  ds_3gpp_atif_dynamic_params_info_type      *dynamic_param_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_ALL_ACTIVE_PRIM_PROFILES
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the list of all 
    active primary profiles.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_atif_get_all_active_prim_profiles
(
  sys_modem_as_id_e_type                subs_id,
  ds_3gpp_atif_prim_profile_list_type * profile_list_info_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_ALL_ACTIVE_FLOW_PROFILES
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the list of all 
    active flow profiles.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_atif_get_all_active_flow_profiles
(
  sys_modem_as_id_e_type            subs_id,
  ds_3gpp_atif_flow_cid_list_type * cid_list_info_ptr
);

#endif /* FEATURE_DATA_3GPP */
#endif /* FEATURE_DATA */
#endif /* DS_3GPP_RMSM_ATIF_H */
