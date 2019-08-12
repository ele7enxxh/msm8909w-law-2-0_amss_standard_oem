#ifndef DS_3GPP_RMSM_ATI_H
#define DS_3GPP_RMSM_ATI_H
/*===========================================================================

                          D S _ 3 G P P _ R M S M _A T I  H

DESCRIPTION
  This file contains the definitions for RM state machine for ATCoP 
  interactions for PS related commands.
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/ds_3gpp_rmsm_ati.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---   ----------------------------------------------------------
09/25/12    ss    3GPP MH LTE-Off compilation fixes
09/13/10    sa    Fixed QCDGEN handler to send more data.
05/19/10    sa    Fixed crash during phys_link_ptr de-registration.
05/12/10    sa    Added support for dynamic parameter related commands.
03/15/10    sa    Added +CGACT support for secondary PDP context,
                  +CGACT support for IPv4v6 profile, +CGCMOD command.
01/15/10    sa    Added support for $QCDGEN and +CGPADDR API.
11/11/09    sa    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#ifdef FEATURE_DATA

#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_rmsm_atif.h"
#include "ds_3gpp_hdlr.h"
#include "ps_iface_defs.h"
#if defined (FEATURE_DATA_WCDMA_PS) ||  defined (FEATURE_GSM_GPRS)
#include "dsumtspsqos.h"
#endif /* (FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) */
#include "ds_sys.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
    The number of instances of the state machine. The total instances
    that can be supported is equal to the MAX calls.
---------------------------------------------------------------------------*/

#define DS_3GPP_RMSM_AT_UM_MAX_INSTANCES  SYS_MAX_EPS_BEARERS_CONTEXTS

typedef enum
{
  DS_3GPP_RMSM_ATIF_BOTH_RM_UM_IFACE_DOWN_STATE =0,     /* Both UM & RM iface down */
  DS_3GPP_RMSM_ATIF_RM_IFACE_UP_ONLY_STATE = 1,         /* Rm i/f is up            */
  DS_3GPP_RMSM_ATIF_RM_IFACE_DOWN_ONLY_STATE = 2,       /* Rm i/f is down          */
  DS_3GPP_RMSM_ATIF_UM_IFACE_UP_ONLY_STATE = 3,         /* Um i/f is up            */
  DS_3GPP_RMSM_ATIF_UM_IFACE_DOWN_ONLY_STATE = 4,       /* Um i/f is down          */
  DS_3GPP_RMSM_ATIF_BOTH_RM_UM_IFACE_UP_STATE = 5,      /* Both UM & RM iface  up  */
  DS_3GPP_RMSM_ATIF_MAX_STATE = 0xFF
}ds_3gpp_rmsm_ati_iface_state_e_type;


/*---------------------------------------------------------------------------
  The enum type for TFT operation code  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_RMSM_TFT_OPCODE_SPARE      = 0x0,  /* Spare                           */
  DS_3GPP_RMSM_TFT_OPCODE_CREATE     = 0x1,  /* Create new TFT                  */
  DS_3GPP_RMSM_TFT_OPCODE_DEL_TFT    = 0x2,  /* Delete existing TFT             */
  DS_3GPP_RMSM_TFT_OPCODE_ADD        = 0x3,  /* Add pkt ftr to exiting TFT      */
  DS_3GPP_RMSM_TFT_OPCODE_REPLACE    = 0x4,  /* Replace pkt ftr in existing TFT */
  DS_3GPP_RMSM_TFT_OPCODE_DEL_FILTER = 0x5,  /* Delete pkt ftr in existing TFT  */
  DS_3GPP_RMSM_TFT_OPCODE_NO_OP      = 0x6,  /* No TFT Operation                */
  DS_3GPP_RMSM_TFT_OPCODE_RESERVED   = 0xFF  /* Reserved                        */
} ds_3gpp_rmsm_tft_opcode_enum_type;

typedef struct 
{
  uint32                                profile_id;
                /* profile: context def   */
  ds_umts_pdp_context_type              profile_context_info; 
  ds_3gpp_atif_pdp_connect_state_e_type connect_state;
  ds_3gpp_rmsm_ati_iface_state_e_type   iface_state;
  boolean                               modify_pending;
                /* pending data for transfer */
  uint32                                pending_data;
  boolean                               um_flow_enabled;
               /* Subs_id on which Instance was UP */
  sys_modem_as_id_e_type                subs_id;
               /* V4 Iface Pointer to associated UM iface   */
  ps_iface_type*                        v4_um_iface_ptr;
               /* V6 Iface Pointer to associated UM iface   */
  ps_iface_type*                        v6_um_iface_ptr;

  ps_flow_type*                         flow_ptr;
                                /* Phys_link_pointer for V4 for data transfer */
  ps_phys_link_type*                    v4_phys_link_ptr;
                /* um up event registration buffer                            */
  void*                                 um_iface_up_buf_ptr;
                /* um down event registration buffer                          */
  void*                                 um_iface_down_buf_ptr; 
                /* um flow on event registration buffer                       */
  void*                                 um_iface_flow_enable_buf_ptr;
                /* um flow off event registration buffer                      */
  void*                                 um_iface_flow_disable_buf_ptr;
                /* um phys link flow on event registration buffer             */
  void*                                 um_phys_link_flow_enable_buf_ptr;
                /* um phys link flow off event registration buffer            */
  void*                                 um_phys_link_flow_disable_buf_ptr;
                /* um phys link gone event registration buffer            */
  void*                                 um_phys_link_gone_buf_ptr;
                /* um flow ACTIVATED event registration buffer      */
  void*                                 um_flow_activated_buf_ptr;
                /* um flow NULL event registration buffer           */
  void*                                 um_flow_null_buf_ptr;
                /* um  flow Modify accept event registration buffer  */
  void*                                 um_flow_modify_accepted_buf_ptr;
                /* um flow modify reject event registration buffer  */
  void*                                 um_flow_modify_rejected_buf_ptr;

  void*                               user_info_ptr;        /*user data     */

} ds_3gpp_ati_pdp_state_type_dyn_s;


typedef struct 
{
                /* flag to denote whether it is in use */
  boolean                               in_use;
  ds_3gpp_ati_pdp_state_type_dyn_s*  ds_3gpp_ati_pdp_state_type_dyn_p;
} ds_3gpp_ati_pdp_state_type;



typedef struct
{
  ps_iface_type *    v4_iface_ptr;
  ps_iface_type *    v6_iface_ptr;
  byte               apn[DS_UMTS_MAX_APN_STRING_LEN+1];
  byte               bearer_id;   /* EPS bearer id 
                                     or NSAPI for UMTS */
  nsapi_T            nsapi;
}ds_3gpp_ati_pdn_dynamic_info_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        FUNCTION DECLARATIONS
  These are to be used by DS_3GPP_RMSM_AT internally and should not be used by 
  the clients of DS_3GPP_RMSM_AT

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_V4_IFACE_PTR
  
  DESCRIPTION   
    Given the call instance, returns the v4 ps iface_ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    See Descritpion.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ps_iface_type* ds_3gpp_rmsm_get_v4_iface_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_V6_IFACE_PTR
  
  DESCRIPTION   
    Given the call instance, returns the v6 ps iface_ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    See Descritpion.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ps_iface_type* ds_3gpp_rmsm_get_v6_iface_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_ACTIVATION

  DESCRIPTION
    This function activates the Profile, if not already activated .
    If the profile is already activated return SUCCESS immediately.

  RETURN VALUE
    DSAT_OK -- Profile is already activated
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- Profile is being activated.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_cgact_activation
(
  uint32                         profile_number,
  sys_modem_as_id_e_type  subs_id,
  void *                  user_info_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_DEACTIVATION

  DESCRIPTION
    This function de-activates the Profile activated by this module,
    if not already de-activated .
    If the profile is already de-activated return SUCCESS immediately.

  RETURN VALUE
    DSAT_OK -- Profile is already deactivated
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- Profile is being deactivated.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_cgact_deactivation
(
  uint32                profile_number,
  sys_modem_as_id_e_type  subs_id,
  void *                user_info_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_INIT_ALL_INSTANCES

DESCRIPTION
  This function initializes all RMSM AT instances 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_3GPP_AT MODULES ONLY


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_rmsm_ati_init_all_instances( void );

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_QUERY

  DESCRIPTION
    This function returns teh profile status of all PDP contexts. It 
    retrieves data from DS_3GPP_RMSM_ST interface and Mode handler for
    valid PDP profiles.

  RETURN VALUE
    DSAT_OK -- if SUCCESS
    DSAT_ERROR -- if FAILURE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_cgact_query
(
  ds_3gpp_atif_profile_status_info_type * profile_status_info_ptr,
  sys_modem_as_id_e_type                  subs_id 
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_V4_UM_IFACE_STATE

DESCRIPTION
  This function returns the RMSM AT instance from  V4 Um Iface pointer if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_AT MODULES ONLY

RETURN VALUE
  PS IFACE STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_state_enum_type ds_3gpp_rmsm_ati_get_v4_um_iface_state
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_V6_UM_IFACE_STATE

DESCRIPTION
  This function returns the RMSM AT instance from  V6 Um Iface pointer if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_AT MODULES ONLY

RETURN VALUE
  PS IFACE STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_state_enum_type ds_3gpp_rmsm_ati_get_v6_um_iface_state
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_INSTANCE_NUM_FROM_PROFILE

DESCRIPTION
  This function returns the RMSM AT instance from profile number if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_AT MODULES ONLY

RETURN VALUE
  The instance number:
     valid DS_3GPP_RMSM instance  if Success
     DS_3GPP_RMSM_AT_UM_MAX_INSTANCES if failure to allocate an instance

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/

ds_3gpp_rmsm_at_instance_type ds_3gpp_rmsm_ati_get_inst_num_from_profile
(
  uint32 profile_number,
  sys_modem_as_id_e_type   subs_id 
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_GET_INFO_PTR_FROM_INSTANCE
  
  DESCRIPTION
    Returns the pointer to 3GPP RMSM_AT instance for the specified instance 
    
  NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
  DSUMTS_RMSM MODULES ONLY
  
  RETURN VALUE
    Pointer to the RMSM AT info structure if a valid instance is passed, else
    NULL
  
  
  DEPENDENCIES
    None
  
  SIDE EFFECTS
    None
  ===========================================================================*/
ds_3gpp_ati_pdp_state_type *  ds_3gpp_rmsm_get_info_ptr_from_instance
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_DEALLOCATE_INSTANCE

DESCRIPTION
  This function de-allocates RMSM AT instance if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_3GPP_AT MODULES ONLY

RETURN VALUE
  None

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_rmsm_ati_deallocate_instance
(
  ds_3gpp_rmsm_at_instance_type  instance
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_PROCESS_QCDGEN

  DESCRIPTION
    This function process data to a context activated
    using CGACT PDP IP profile.

  RETURN VALUE
    DSAT_ERROR -- In case of any FAILURE
    DSAT_NO_RSP -- data transfer is halted
                   due to flow control/ pending data 
    DSAT_OK   -- complete data is transferred

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

dsat_result_enum_type ds_3gpp_rmsm_ati_process_qcdgen
(
  uint32                                  profile_number,
  sys_modem_as_id_e_type                  subs_id,
  uint32                                  total_data_len,
  void *                                  user_info_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_GET_PDP_ADDR

  DESCRIPTION
  
  This function returns the PDP addr of the PDN connection for which 
  the Profile number is passed.

  RETURN VALUE
    DSAT_OK -- if SUCCESS
    DSAT_ERROR -- if FAILURE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_get_pdp_addr
(
  uint32                    profile_num,
  sys_modem_as_id_e_type    subs_id,
  ds_umts_pdp_addr_type    *pdp_addr_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATI_CGCMOD_HANDLER
  
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
dsat_result_enum_type ds_3gpp_rmsm_ati_cgcmod_handler
(
  uint32                                profile_number,
  sys_modem_as_id_e_type                subs_id,
  void *                                user_info_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATI_CONV_TO_LTE_APP_QOS_FROM_ID
  
  DESCRIPTION
    This Function is called for converting the QOS present 
    in the profile context info to LTE APP QOS. 

  DEPENDENCIES
    None.
  
  RETURN VALUE
    FALSE: in case of Failure
    TRUE : in case of Success
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ati_conv_flow_to_lte_app_qos_from_id
(
  ds_umts_pdp_profile_type           *pdp_ptr,
  qos_spec_type                      *app_flow
);

/*===========================================================================
FUNCTION DS_3GPP_RMSM_ATI_CONV_TFT_TO_APP_TFT_FROM_ID

DESCRIPTION
  This function fills in the Flow TFT parameters based on the operation code,
  requested profile TFT and filter offset and quantity information. 
  
PARAMETERS
  opcode     - Operation code.
  *req_tft   - Requested TFT to copy filter info from.
  filter_ofs - Offset at which to start pulling filter info from: invalid 
               packet filter definitions are skipped when counting the offset 
               and copying the number of filters given, so this is offet into
               an imaginary array filled only with valid packet filters.
  filter_qty - Number of filters to copy.
  *app_flow   - Apps Flow TFT to copy into.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE: if conversion is Success
  FALSE : if conversion is Failure
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_rmsm_ati_conv_tft_to_app_tft_from_id
(
  ds_3gpp_rmsm_tft_opcode_enum_type  opcode,
  ds_umts_tft_params_type      *req_tft,
  uint8                         filter_ofs,
  uint8                         filter_qty,
  qos_spec_type                *app_flow
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_GET_FLOW_PTR
  
  DESCRIPTION   
    Given the call instance, returns the flow_ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    See Description.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ps_flow_type* ds_3gpp_rmsm_ati_get_flow_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATI_READ_DYNAMIC_PARAMS
  
  DESCRIPTION
    This Function is called by ATCoP for reading the dynamic parameters 
    associated with the primary /secondary activated profiles .
    This function returns DSAT_ERROR if the profile is not activated .

  DEPENDENCIES
    None.
  
  RETURN VALUE
    FALSE: in case of Failure
    TRUE: If retrieval of dynamic data is success.
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ati_read_dynamic_params
(
  uint32                                      profile_number,
  sys_modem_as_id_e_type                      subs_id,
  ds_3gpp_atif_dynamic_e_type                 info_type,
  ds_3gpp_atif_dynamic_params_info_type      *dynamic_param_ptr
);

#endif /*FEATURE_DATA_3GPP */
#endif /* FEATURE_DATA */

#endif /* DS_3GPP_RMSM_ATI_H */
