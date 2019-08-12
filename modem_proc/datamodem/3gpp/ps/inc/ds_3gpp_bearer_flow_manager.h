/*!
  @file
  ds_3gpp_bearer_flow_manager.h

  @brief
  Manages Flow contexts and provides interface to PDN and bearer

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_bearer_flow_manager.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   ss      3GPP MH LTE-Off compilation fixes
10/19/11   nd      Bearer rate querry uses NW granted bit rates.
02/04/10   vd      Changed function naming format.
01/23/10   vk      Added helper macros for virtual function dispatch
01/20/10   vd      Changes to support UE Qos modification.
11/07/09   vd      Initial version.
===========================================================================*/

#ifndef DS3GPPBEARERFLOWMANAGER_H
#define DS3GPPBEARERFLOWMANAGER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "list.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds3gmgr.h"
#include "ds_3gpp_flow_context.h"
#include "ps_qos_defs.h"
#include "ps_ipfltr_defs.h"
#include "ds_3gppi_utils.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_FLOW_E_BIT_DISABLE    0
#define DS_FLOW_E_BIT_ENABLE     1
#define DS_MAX_FI_ID             16
#define DS_MAX_FI_PRECED         255
#define DS_MIN_FI_PRECED         1
#define DS_FI_PRECED_ARR_SIZE    8
#define DS_DEF_FILTER_PRECD      256
#define DS_DEF_FILTER_ID         256
#define BEARER_FLOW_MANAGER_VALID_COOKIE  0xbefbefbe

#define DS_BEARER_FLOW_MGR_VF_FPN(func) DS_3GPPI_VF_FPN(func)

#define DS_BEARER_FLOW_MGR_VF_REG(mode, func, fp) \
          DS_3GPPI_VF_REG(ds_bearer_flow_manager_ftbl, mode, func, fp)

#define DS_BEARER_FLOW_MGR_VOBJ_REG(ds_bearer_flow_manager_p, mode, objp) \
          DS_3GPPI_VOBJ_REG((ds_bearer_flow_manager_p)->ds_bearer_flow_manager_dyn_p->\
          vobj_data_ptr_tbl, mode, (void*)objp)

#define DS_BEARER_FLOW_MGR_VOBJ(ds_bearer_flow_manager_p, mode) \
          DS_3GPPI_VOBJ((ds_bearer_flow_manager_p)->ds_bearer_flow_manager_dyn_p->\
          vobj_data_ptr_tbl, mode)


#define DS_BEARER_FLOW_MGR_VFR_CALL(rval, func, subs_id, ...)  \
            DS_3GPPI_VFR_CALL(ds_bearer_flow_manager_ftbl, rval, func, subs_id,\
                              __VA_ARGS__)

#define DS_BEARER_FLOW_MGR_VF_CALL(func, subs_id, ...)  \
            DS_3GPPI_VF_CALL(ds_bearer_flow_manager_ftbl, func, subs_id,\
                             __VA_ARGS__)

/*---------------------------------------------------------------------------
  enum type to identify the cause code to be sent to network.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_INTERNAL_CC_NONE                  = 0x0,
  DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES   = 0x1,
  DS_3GPP_INTERNAL_FEATURE_NOT_SUPPORTED      = 0x2,
  DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED           = 0x3,
  DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT        = 0x4,
  DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT     = 0x5,
  DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER     = 0x6,
  DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER  = 0x7,
  DS_3GPP_INTERNAL_BEARER_WITH_NO_TFT_ALREADY_PRESENT = 0x8,
  DS_3GPP_INTERNAL_INVALID_MANDATORY_INFO     = 0x9,
  DS_3GPP_INTERNAL_ACTIVATION_REJECTED        = 0xA,
  DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED     = 0xB,
  DS_3GPP_INTERNAL_BCM_VIOLATION              = 0xC,
  DS_3GPP_INTERNAL_CC_INVALID                 = 0xFF
} ds_3gpp_internal_cause_code_enum_type;

/*===========================================================================
MACRO DS_3GPP_QOS_IS_NETW_PRE_REL8_QOS_VALID

DESCRIPTION   
  Checks validilty of Cm provided Netw Allocated QoS 

PARAMETERS
  qos_T *netw_qos_ptr;

DEPENDENCIES  
  None.

RETURN VALUE  
  boolean
  
LIMITATIONS

===========================================================================*/
#define DS_3GPP_QOS_IS_NETW_PRE_REL8_QOS_VALID( netw_pre_rel8_qos_ptr ) \
                  ( ( (netw_pre_rel8_qos_ptr) == NULL ) \
                    ?FALSE \
                    :(netw_pre_rel8_qos_ptr)->valid \
                   )

/*===========================================================================
MACRO DS_3GPP_QOS_IS_NETW_EPS_QOS_VALID

DESCRIPTION   
  Checks validilty of Cm provided Netw Allocated QoS 

PARAMETERS
  qos_T *netw_qos_ptr;

DEPENDENCIES  
  None.

RETURN VALUE  
  boolean
  
LIMITATIONS

===========================================================================*/
#define DS_3GPP_QOS_IS_NETW_EPS_QOS_VALID( netw_eps_qos_ptr ) \
                  ( ( (netw_eps_qos_ptr) == NULL ) \
                    ?FALSE \
                    :(netw_eps_qos_ptr)->valid \
                   )

/*-----------------------------------------------------------------------------
  Each bearer will have one bearer flow manager.
-----------------------------------------------------------------------------*/
typedef struct
{  
  uint32		            cookie;      /* Cookie to ensure dynamic allocation is successful*/
  ds_bearer_context_s  *bearer_ptr;/* bearer associated with flow manager*/
  ds_pdn_context_s     *pdn_ptr;/* PDN associated with the bearer*/
  uint32                fi_identifier; /*bitmask of filter identifiers in use*/
  list_type             flow_context;/*Flow contexts associated with bearer*/
  qos_T                 neg_qos; /* QOS in WCDMA format */
  void *                vobj_data_ptr_tbl[DS_3GPPI_SYS_MODE_MAX];/* Mode specific data*/
  ip_flow_type          rx_flow;  /* NW RX Flow in App format */
  ip_flow_type          tx_flow;  /* NW TX Flow in App Format */
  int                   index;    /* Index where the memory is allocated*/
} ds_bearer_flow_manager_dyn_s;

typedef struct
{
  list_link_type        link;       /* Ptr to next node */
  ds_bearer_flow_manager_dyn_s *ds_bearer_flow_manager_dyn_p;
} ds_bearer_flow_manager_s;

/*-----------------------------------------------------------------------------
  Flow Manager table exposed to other files through header
-----------------------------------------------------------------------------*/
extern ds_bearer_flow_manager_s 
                      ds_bearer_flow_manager_tbl[DS_3GPP_MAX_BEARER_CONTEXT];


// macros for filter id bit mask
#define DS_SET_FI_BIT(mask, fi_id)  (mask |= (0x1 << fi_id))
#define DS_CLEAR_FI_BIT(mask, fi_id)  (mask &= (~(0x1 << fi_id)))
#define DS_IS_FI_BIT_SET(mask, fi_id)  ((mask & (0x1 << fi_id)) != 0)

/*---------------------------------------------------------------------------
  Table of handler functions for bearer flow manager. Each mode-specific handler 
  maintains such a table, and registers this table with the bearer flow manager.

  Based on the current mode the function belonging to the mode is invoked.
  In some cases these function pointers could be NULL which means that the
  mode does not need such functionality.

  The same functions can be registered in different modes if the procedures
  are the same across them.
---------------------------------------------------------------------------*/

typedef struct 
{
  /*---------------------------------------------------------------------------
    This function is invoked to convert data rate from the network specific
    format to the application format.
    Returns failure if the network passes invalid parameters.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(nwDataRateToAppDataRate))(
                                       ds_bearer_flow_manager_s *bflow_mgr_p,
                                       void           *nw_qos,
                                       qos_spec_type  *app_qos,
                                       ds_3gpp_internal_cause_code_enum_type *cause_code_p
                                      ); 

  /*---------------------------------------------------------------------------
    This function is invoked to convert data rate from the application specific
    format to network format.
    Returns failure if the application passes invalid parameters.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(appDataRateToNwDataRate))(
                                       qos_spec_type  *app_qos,
                                       void           *nw_qos
                                      ); 

  /*---------------------------------------------------------------------------
    This function is invoked to retrieve bearer data rate.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(getBearerRate))(
                             ds_bearer_flow_manager_s *bearer_flow_manager,
                             void  *pdn_aggregate_rate
                            ); 
  /*---------------------------------------------------------------------------
    This function is invoked to store the bearer QOS details. The Qos is stored
    in NW format.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(setBearerQos))(
                             ds_pdn_context_s *pdn_ptr,
                             void  *pdn_aggregate_rate
                           ); 
  /*---------------------------------------------------------------------------
    This function is invoked to validate flow parameters provided by the app.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(flowValidation))(
                        qos_spec_type      *input_qos_p);
  

    /*---------------------------------------------------------------------------
    This function is invoked to allocate the umts bearer flow manager
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(allocate_umts_bearer_flow_manager))(
                             int index); 
  /*---------------------------------------------------------------------------
     This function is invoked to free the umts bearer flow manager
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(free_umts_bearer_flow_manager))(
                        int  index);

  /*---------------------------------------------------------------------------
    This function is invoked to allocate the eps bearer flow manager
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(allocate_eps_bearer_flow_manager))(
                             int index); 

 /*---------------------------------------------------------------------------
    This function is invoked to free the eps bearer flow manager
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_FLOW_MGR_VF_FPN(free_eps_bearer_flow_manager))(
                             int index); 
 
} ds_3gpp_bearer_flow_manager_ftbl_s;

extern ds_3gpp_bearer_flow_manager_ftbl_s 
                                   ds_bearer_flow_manager_ftbl[DS_3GPPI_SYS_MODE_MAX];





/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DSBEARERFLOWMANAGER_3GPPINIT

DESCRIPTION
  Initialize the flow manager module.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_3gpp_init( void );


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update filter identifier bit mask for all the filters in the flow context.

PARAMETERS
 bearer_context_p    - Pointer to bearer Context.
 flow_context_p      - Pointer to flow context.
 set_identifier      - boolean. If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_update_filter_identifier_bit_mask
( 
  ds_bearer_context_s  *bearer_context_p,
  ds_flow_context_s    *flow_context_p,
  boolean              set_identifier
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_VERIFY_APP_FILTER_IDENTIFIER

DESCRIPTION
  Verify if the app passed valid filter identifier value.

PARAMETERS
 input_qos           - Pointer to App passed Qos.
 filter_op           - enum indicating what filters to check.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_verify_app_filter_identifier
( 
  qos_spec_type      *input_qos,
  ds_3gpp_app_filter_op filter_op
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ALLOCATE_FLOW_MANAGER

DESCRIPTION
  Allocate a new flow manager for this bearer.

PARAMETERS
 bearer_context_p     - Pointer to bearer context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_bearer_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_flow_manager_s * ds_bearer_flow_manager_allocate_flow_manager
( 
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DSBEARERFLOWMANAGER_RELEASEFLOWMANAGER

DESCRIPTION
  Release the flow manager associated with the bearer.

PARAMETERS
  bearer_context_p  - Pointer to the bearer context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_release_flow_manager
(
  ds_bearer_context_s *bearer_context_p
);


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_FLOW_MANAGER

DESCRIPTION
  Find the flow manager for this bearer.

PARAMETERS
 bearer_context_p     - Pointer to PDN context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_bearer_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_flow_manager_s *ds_bearer_flow_manager_find_flow_manager
( 
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ADD_FLOW_CONTEXT_TO_FLOW_MANAGER

DESCRIPTION
  Add a flow context to the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_add_flow_context_to_flow_manager
( 
  ds_bearer_context_s *bearer_context_p,
  ds_flow_context_s   *flow_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_REMOVE_FLOW_CONTEXT_FROM_FLOW_MANAGER

DESCRIPTION
  Remove a flow context from the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_remove_flow_context_from_flow_manager
( 
  ds_bearer_context_s *bearer_context_p,
  ds_flow_context_s   *flow_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_DELETE_ALL_FLOW_CONTEXT

DESCRIPTION
  Delete all the flow contexts associated with the bearer context.

PARAMETERS
  pdn_context_p           -  PDN information
  bearer_context_p        -  Bearer information

DEPENDENCIES
  None.

RETURN VALUE
  boolean         - Return FALSE when delete fails.
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_delete_all_flow_context
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_BIND_FLOW

DESCRIPTION
  Bind the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_bind_flow
( 
  ds_bearer_context_s *bearer_context_p
);


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ACTIVATE_FLOW

DESCRIPTION
  Activate the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_activate_flow
( 
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_SUSPEND_FLOW

DESCRIPTION
  Suspend the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_suspend_flow
( 
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GO_NULL_FLOW

DESCRIPTION
  Post go Null on the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_go_null_flow
( 
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_NW_FLOW_CONTEXT

DESCRIPTION
  Find the Flow context associated with the NW initiated Qos.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 flow_context_p       - Pointer to Bearer information.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s * ds_bearer_flow_manager_find_nw_flow_context
( 
  ds_bearer_context_s *bearer_context_p,
  uint8                iface_type
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_FLOW_CONTEXT

DESCRIPTION
  Find the Flow context associated with the Qos.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.
 iface_type           - Indicates V4 or V6 flow.
 is_nw_flow           - flag to indicate whether QOS is nw init

DEPENDENCIES
  None.

RETURN VALUE
 flow_context_p       - Pointer to flow_context.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s * ds_bearer_flow_manager_find_flow_context
( 
  ds_bearer_context_s *bearer_context_p,
  uint8                iface_type,
  boolean              is_nw_flow
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_DATA_RATE

DESCRIPTION
  Convert the NW FLOW to APP Flow. Invoke the mode specific handlers to 
  convert the value.

PARAMETERS
  bearer_context_p -  Bearer information
  mode_info_p      -  mode specific call info. 
  app_qos_p        -  Pointer to App Qos structure.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_convert_data_rate
(
  ds_bearer_context_s             *bearer_context_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  qos_spec_type                   *app_qos_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_APP_QOS_TO_NW_QOS

DESCRIPTION
  Converts the APP passed Flow spec to NW Qos format.

PARAMETERS
 app_flow_spec     -  App passed Qos
 nw_qos            -  network Qos to be filled
 subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_convert_app_qos_to_nw_qos
(
   qos_spec_type           *app_flow_spec,
   void                    *nw_qos,
   sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_FLOW_MANAGER_CONVERT_NW_DATA_RATE_TO_APP
DESCRIPTION
  Converts the UMTS NW passed data rate to App data rate.

PARAMETERS
nw_qos          -  EPS network qos to be converted
app_qos         -  Application qos structure
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_flow_manager_convert_nw_data_rate_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void           *nw_qos,
  qos_spec_type  *app_qos
);

/*===========================================================================
FUNCTION  DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_SDF_QOS_TO_APP
DESCRIPTION
  Converts the SDF QOS NW passed to App data rate.

PARAMETERS
nw_qos          -  SDF qos to be converted
app_qos         -  Application qos structure
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_flow_manager_convert_nw_sdf_qos_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void           *nw_qos,
  qos_spec_type  *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_NW_DATA_RATE_TO_APP_DATA_RATE

DESCRIPTION
  This function is invoked to convert data rate from the network specific
  format to the application format.

PARAMETERS
  bflow_mgr_p   -  'this' ptr
  nw_qos        -  network Qos 
  app_flow_spec -  App Qos

DEPENDENCIES
  None.

RETURN VALUE
  boolean      - returns DS3G_SUCCESS on success, DS3G_FAILURE otherwise
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_nw_data_rate_to_app_data_rate
(
  ds_bearer_flow_manager_s *bflow_mgr_p,
  void           *nw_qos,
  qos_spec_type  *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
); 

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_APP_DATA_RATE_TO_NW_DATA_RATE

DESCRIPTION
  This function is invoked to convert data rate from the application specific
  format to network format.

PARAMETERS
  app_flow_spec -  App Qos
  nw_qos        -  network Qos
  subs_id       -  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean      - returns DS3G_SUCCESS on success, DS3G_FAILURE otherwise
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_app_data_rate_to_nw_data_rate
(
  qos_spec_type          *app_qos,
  void                   *nw_qos,
  sys_modem_as_id_e_type  subs_id
); 

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_BEARER_QOS

DESCRIPTION
  This function is invoked to retrieve bearer QOS details.Qos is stored in 
  NW format. The conversion to Application format has to be invoked
  before passing it to Application.

PARAMETERS
  bearer_flow_manager     -  Bearer flow manager ptr
  pdn_aggregate_rate      -  aggregate data rate
  sys_modem_as_id_e_type  -  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  boolean      - returns DS3G_SUCCESS on success, DS3G_FAILURE otherwise
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_get_bearer_qos
(
  ds_bearer_flow_manager_s *bearer_flow_manager,
  void                     *pdn_aggregate_rate,
  sys_modem_as_id_e_type    subs_id

);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_QOS_FROM_BEARER_ID

DESCRIPTION
  Return the sdfqos ie stored in the bearer flow manager for a specific
  bearer

PARAMETERS
  eps_bearer_id           -  EPS bearer id for the bearer
  sys_modem_as_id_e_type  -  subs_id

DEPENDENCIES
  None.

RETURN VALUE
 void*             - pointer to the mode specific qos structure
  
SIDE EFFECTS
  None.

===========================================================================*/
void* ds_bearer_flow_manager_get_qos_from_bearer_id
(
  byte                    eps_bearer_id,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FLOW_VALIDATION

DESCRIPTION
  This function is invoked to validate flow parameters provided by the app.

PARAMETERS
  input_qos_p - pointer to flow/qos provided by the app.
  subs_id     - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean - success/failure
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_flow_validation
(
  qos_spec_type           *input_qos_p,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_IFACE_OUT_OF_USE

DESCRIPTION
  The iface has gone out of use. Check for an UE initiated bearers and
  clean them up. Update the TFT information that needs to be sent out to 
  the network.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  bearer_context_p         -  Pointer to Bearer context.
  flow_type                -  IP Type.
  delete_nw_flow           -  boolean. IF true NW flow needs to be deleted.

DEPENDENCIES
  None.

RETURN VALUE
  flow_context_p          -    Returns pointer to flow context to modify.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
ds_flow_context_s *ds_bearer_flow_manager_iface_out_of_use
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  uint8                            flow_type,
  boolean                          delete_nw_flow
);

/*===========================================================================
FUNCTION DS_3GPP_COVERT_TO_EPS_QOS

DESCRIPTION
3gpp spec ref: 23.401 Annex E  

PARAMETERS
 pre_rel8_qos_p           - input
 eps_qos_p                - output

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_convert_pre_rel8_to_eps_qos
(
  qos_T         *pre_rel8_qos_p,
  sdf_qos_T     *eps_qos_p
);

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_FLOW_MANAGER_CONVERT_NW_QOS_TO_APP

DESCRIPTION
  Converts the UMTS NW passed data rate to App data rate.

PARAMETERS
nw_qos          -  EPS network qos to be converted
app_qos         -  Application qos structure

DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_flow_manager_convert_nw_qos_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void           *nw_qos,
  qos_spec_type  *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);


/*===========================================================================
FUNCTION  DS_3GPP_BEARER_FLOW_MANAGER_GET_APP_DATA_RATE

DESCRIPTION
  Gives the App data rate.

PARAMETERS
  bearer_flow_manager     -  Bearer flow manager ptr
  pdn_aggregate_rate      -  aggregate data rate 

DEPENDENCIES
  None.

RETURN VALUE
  boolean                 - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_flow_manager_get_app_data_rate
(
  ds_bearer_flow_manager_s *bearer_flow_manager,
  void  *pdn_aggregate_rate
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_AND_SET_FILTER_ID

DESCRIPTION
  Returns the first available filter id value for flow manager. 

PARAMETERS
  flow_mgr_ptr     - Pointer to the flow manager context.
  lower_limit      - Assigned precedence should be >=  lower limit.
  upper_limit      - Assigned precedence should be <=  upper limit.
DEPENDENCIES
  None.

RETURN VALUE
  int           - Returns -1 if the precedence cannot be assigned.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_bearer_flow_manager_find_and_set_filter_id
(
  uint32    *fi_id_p,
  uint16    lower_limit,
  uint16    upper_limit
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_DELETE_FLOW_CONTEXT

DESCRIPTION
  Delete a flow context from a bearer context.

PARAMETERS
  pdn_context_p           -  PDN information
  bearer_context_p        -  Bearer information
  flow_context_p          -  Flow context information.
  

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_delete_flow_context
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  ds_flow_context_s               *flow_context_p
);

/*===========================================================================
FUNCTION ds_bearer_flow_manager_validate_flow_manager

DESCRIPTION
  This function checks if the bearer flow manager tbl
  has been allocated dynamic memory

PARAMETERS
  flow_manager_p       - bearer_flow_manager to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_flow_manager_validate_flow_manager
(
  ds_bearer_flow_manager_s *bearer_flow_manager_p
);

/*===========================================================================
FUNCTION ds_bearer_flow_manager_free_dyn_mem

DESCRIPTION
  This function frees dyn memory.
 
PARAMETERS
   NULL
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/

void ds_bearer_flow_manager_free_dyn_mem
(
   ds_bearer_flow_manager_s *bflow_manager_p
); 
 
/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_ALL_CIDS

DESCRIPTION
  This function returns the list of all flow cid's that are 
  currently active.
  
PARAMETERS   
  cid_list_ptr  - list of cid's which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_flow_manager_get_all_cids
(
  sys_modem_as_id_e_type            subs_id,
  ds_3gpp_atif_flow_cid_list_type * cid_list_ptr
); 


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_ALL_CIDS_FOR_DDS

DESCRIPTION
  This function returns the list of all flow cid's that are 
  currently active.
  
PARAMETERS   
  cid_list_ptr  - list of cid's which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_flow_manager_get_all_cids_for_dds
(
  ds_3gpp_atif_flow_cid_list_type * cid_list_ptr
);

/*===========================================================================
FUNCTION ds_bearer_flow_manager_convert_nw_r9798_qos_to_app_r99_qos

DESCRIPTION
  This function converts Release 97/98 into Release 99 specific QOS parameters.
  Octet 3 to Octet 5 of the QOS parameters in PDP context req are
  R97/98 qos parameters.

PARAMETERS
  *nw_alloc_qos - Ptr to NW QoS parameters for the current call
  *tx_flow - Ptr to tx flow Qos
  *rx_flow - Ptr to rx flow Qos


DEPENDENCIES
  For origination, assumes that the R99 parameters have been filled in 
  already in the orig_params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_convert_nw_r9798_qos_to_app_r99_qos
(
  qos_T         *nw_alloc_qos,
  ip_flow_type  *tx_flow,
  ip_flow_type  *rx_flow
);

#endif /* FEATURE_DATA_3GPP */
#endif /* DS3GPPBEARERFLOWMANAGER_H */

