/*!
  @file
  ds_3gpp_flow_manager.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_flow_manager.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/10   vd      Changed the function naming format.
01/27/10   vd      Changed the function naming format.
01/23/10   vk      Added helper macros for virtual function dispatch
01/20/10   vd      Changes to support UE Qos modification.
07/17/09   vd      Initial version.
===========================================================================*/

#ifndef DS3GPPFLOWMANAGER_H
#define DS3GPPFLOWMANAGER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#if defined(FEATURE_DATA_3GPP) 
#include "list.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds3gmgr.h"
#include "ds_3gpp_tft.h"
#include "ds_3gpp_flow_context.h"
#include "ps_qos_defs.h"
#include "ps_ipfltr_defs.h"
#include "rex.h"
#include "ds_3gpp_qos.h"

// See if we can avoid this
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gppi_utils.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_FLOW_MGR_VF_FPN(func) DS_3GPPI_VF_FPN(func)

#define DS_FLOW_MGR_VF_REG(mode, func, fp) \
          DS_3GPPI_VF_REG(ds_flow_manager_ftbl, mode, func, fp)

#define DS_FLOW_MGR_VOBJ_REG(ds_flow_manager_p, mode, objp) \
          DS_3GPPI_VOBJ_REG((ds_flow_manager_p)->vobj_data_ptr_tbl, mode, (void*)objp)

#define DS_FLOW_MGR_VOBJ(ds_flow_manager_p, mode) \
          DS_3GPPI_VOBJ((ds_flow_manager_p)->vobj_data_ptr_tbl, mode)


#define DS_FLOW_MGR_VFR_CALL(rval, func, subs_id, ...)  \
            DS_3GPPI_VFR_CALL(ds_flow_manager_ftbl, rval, func, subs_id, \
                              __VA_ARGS__)

#define DS_FLOW_MGR_VF_CALL(func, subs_id, ...)  \
            DS_3GPPI_VF_CALL(ds_flow_manager_ftbl, func, subs_id, __VA_ARGS__)
                                                                             

#define DS_FLOW_E_BIT_DISABLE    0
#define DS_FLOW_E_BIT_ENABLE     1
#define DS_FLOW_SDF_ID           0x5400
#define DS_FI_PRECED_ARR_SIZE    8
#define DS_DEF_FILTER_ID         256

/*-----------------------------------------------------------------------------
  Global critical section
-----------------------------------------------------------------------------*/
extern rex_crit_sect_type ds_3gpp_flow_crit_sect; 

/*-----------------------------------------------------------------------------
  Each PDN will have one flow manager.
-----------------------------------------------------------------------------*/
typedef struct
{  
  uint32		          cookie; 		/* Cookie to ensure dynamic allocation is successful*/
  ds_pdn_context_s *  pdn_ptr;/* PDN associated with the flow manager */
  uint32              fi_preced[DS_FI_PRECED_ARR_SIZE];/*bitmask of the Filter precedence in use*/
  uint32              req_fi_preced[DS_FI_PRECED_ARR_SIZE];/*bitmask of the requested Filter prece*/
  list_type           bearer_flow_mgr;/*bearers associated with this PDN*/
  void *              vobj_data_ptr_tbl[DS_3GPPI_SYS_MODE_MAX];/* Mode specific data*/
  list_type           pending_flow_cntx;/*Pending UE initiated requests*/
  ds_3gpp_bcm_info_e  bcm_info;   /* bearer control mode info */
  int                 index;
} ds_flow_manager_dyn_s;

typedef struct
{
  ds_flow_manager_dyn_s *ds_flow_manager_dyn_p;
} ds_flow_manager_s;

/*-----------------------------------------------------------------------------
  Flow Manager table exposed to other files through header
-----------------------------------------------------------------------------*/
extern ds_flow_manager_s ds_flow_manager_tbl[DS_3GPP_MAX_PDN_CONTEXT];

#define FLOW_MANAGER_VALID_COOKIE  0xf1f1f1f1

/*---------------------------------------------------------------------------
  The enum type for TFT operation code  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_TFT_OPCODE_SPARE      = 0x0,  /* Spare                           */
  DS_3GPP_TFT_OPCODE_CREATE     = 0x1,  /* Create new TFT                  */
  DS_3GPP_TFT_OPCODE_DEL_TFT    = 0x2,  /* Delete existing TFT             */
  DS_3GPP_TFT_OPCODE_ADD        = 0x3,  /* Add pkt ftr to exiting TFT      */
  DS_3GPP_TFT_OPCODE_REPLACE    = 0x4,  /* Replace pkt ftr in existing TFT */
  DS_3GPP_TFT_OPCODE_DEL_FILTER = 0x5,  /* Delete pkt ftr in existing TFT  */
  DS_3GPP_TFT_OPCODE_NO_OP      = 0x6,  /* No TFT Operation                */
  DS_3GPP_TFT_OPCODE_RESERVED   = 0xFF  /* Reserved                        */
} ds_tft_opcode_enum_type;

/*---------------------------------------------------------------------------
  The enum type for TFT operation code  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_TFT_AUTHORIZATION_TOKEN      = 0x1,  /* Authorizationt token     */
  DS_3GPP_TFT_FLOW_IDENTIFIER          = 0x2,  /* Flow identifier          */
  DS_3GPP_TFT_PACKET_FILTER_IDENTIFIER = 0x3,  /* Filter identifier        */
  DS_3GPP_TFT_PARAM_RESERVED           = 0xFF  /* Reserved                 */
} ds_tft_param_enum_type;

/*---------------------------------------------------------------------------
  The enum type for TFT packet filter component type identifier  
  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_TFT_EMPTY_TYPE          = 0x00,   /* Empty type                            */
  DS_3GPP_TFT_IPV4_ADDR           = 0x10,   /* IPV4 Address type                     */
  DS_3GPP_TFT_IPV4_LOCAL_ADDR     = 0x11,   /* IPV4 local Address type               */
  DS_3GPP_TFT_IPV6_ADDR           = 0x20,   /* IPV6 Address type                     */
  DS_3GPP_TFT_IPV6_REM_PRE_ADDR   = 0x21,   /* IPV6 remote address/prefix lengthtype */
  DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR = 0x23,   /* IPV6 local address/prefix lengthtype  */
  DS_3GPP_TFT_NEXT_HDR_TYPE       = 0x30,   /* Protocol identifier                   */
  DS_3GPP_TFT_SINGLE_LOCAL_PORT   = 0x40,   /* Single local port                     */
  DS_3GPP_TFT_LOCAL_PORT_RANGE    = 0x41,   /* local port range                      */
  DS_3GPP_TFT_SINGLE_REMOTE_PORT  = 0x50,   /* Single remote port                    */
  DS_3GPP_TFT_REMOTE_PORT_RANGE   = 0x51,   /* remote port range                     */
  DS_3GPP_TFT_SEC_PARAM_IDX       = 0x60,   /* Security parameter index              */
  DS_3GPP_TFT_TYPE_OF_SRVC        = 0x70,   /* Type of service                       */
  DS_3GPP_TFT_IP6_TRAFFIC_CLASS   = DS_3GPP_TFT_TYPE_OF_SRVC, 
                                            /* IPV6 traffic class                    */
  DS_3GPP_TFT_FLOW_LABEL          = 0x80    /* Flow Label                            */
} ds_tft_comp_type_id_enum_type;


/*---------------------------------------------------------------------------
  enum type to help verify that the Network did not send wrong params.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_FILTER_NONE                  = 0x0,
  DS_3GPP_FILTER_IP_ADDR_TYPE          = 0x1,
  DS_3GPP_FILTER_NXT_HDR_TYPE          = 0x2,
  DS_3GPP_FILTER_LOCAL_PORT_TYPE       = 0x4,
  DS_3GPP_FILTER_REMOTE_PORT_TYPE      = 0x8,
  DS_3GPP_FILTER_SEC_TYPE              = 0x10,
  DS_3GPP_FILTER_TOS_TYPE              = 0x20,
  DS_3GPP_FILTER_FLOW_LABEL_TYPE       = 0x40,
  DS_3GPP_FILTER_IP_LOCAL_ADDR_TYPE    = 0x80,
  DS_3GPP_FILTER_IP_REMOTE_PREFIX_TYPE = 0x100,
  DS_3GPP_FILTER_IP_LOCAL_PREFIX_TYPE  = 0x200,
  DS_3GPP_FILTER_ALL                   = 0x3FF
} ds_3gpp_filter_mask_enum_type;

typedef enum
{
  DS_3GPP_FLOW_BEARER_ALLOC = 0,
  DS_3GPP_FLOW_BEARER_ALLOC_ABORT = 1,
  DS_3GPP_FLOW_BEARER_MODIFY = 2
} ds_3gpp_flow_qos_enum_type;

typedef struct
{
  void   * flow_context_p;
  void   * bearer_context_p;
  uint32   fltr_id_conflict;
  uint32   preced_conflict[8];
  boolean  prec_conf_on_same_bearer_modify;
}ds_flow_manager_cmd_info_type;

typedef struct
{
  qos_spec_type               app_qos;
  tft_type_T                  nw_tft;
  void                        *flow_context_p;
  void                        *pdn_context_p;
  int                         flow_state;
  ds_3gpp_flow_qos_enum_type  event_type;
}ds_flow_manager_qos_cmd_info_type;


// macros for filter id bit mask
#define DS_SET_FI_BIT(mask, fi_id)  (mask |= (0x1 << fi_id))
#define DS_CLEAR_FI_BIT(mask, fi_id)  (mask &= (~(0x1 << fi_id)))
#define DS_IS_FI_BIT_SET(mask, fi_id)  ((mask & (0x1 << fi_id)) != 0)

/*---------------------------------------------------------------------------
  Table of handler functions for flow manager. Each mode-specific handler 
  maintains such a table, and registers this table with the flow manager.

  Based on the current mode the function belonging to the mode is invoked.
  In some cases these function pointers could be NULL which means that the
  mode does not need such functionality.

  The same functions can be registered in different modes if the procedures
  are the same across them.
---------------------------------------------------------------------------*/
typedef struct 
{
  /*---------------------------------------------------------------------------
    This function is invoked to verify the Network passed params are valid.
    Returns failure if the params are not valid.
    if default_bearer is TRUE, only data rate is expected
    if bearer_alloc is  FALSE, this is a new bearer allocation request.   
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(checkNetworkInput))(
                                  cm_call_mode_info_s_type *mode_info_ptr,
                                  boolean default_bearer,
                                  boolean bearer_alloc
                                ); 

  /*---------------------------------------------------------------------------
    This function is invoked to convert generic cause codes to mode specific
    cause codes.
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(convertCauseCodes))(
                             const cm_call_mode_info_s_type *mode_info_ptr,
                   ds_3gpp_internal_cause_code_enum_type  *ds_internal_cause
                                   ); 

  /*---------------------------------------------------------------------------
    This function is invoked to handle secondary bearer tear down
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(TearDownSecBearer))(
                            ds_flow_context_s   *flow_context_p
                          );

  /*---------------------------------------------------------------------------
    This function is invoked to handle bearer allocation rejection by the 
    network.
  ---------------------------------------------------------------------------*/
  void (*DS_FLOW_MGR_VF_FPN(NwBearerAllocRej))(
                            cm_call_mode_info_s_type* mode_info_ptr
                          );
                                 
  /*---------------------------------------------------------------------------
    This function is invoked to handle bearer allocation failure by the 
    network.
  ---------------------------------------------------------------------------*/
  void (*DS_FLOW_MGR_VF_FPN(NwBearerAllocFail))(
                             cm_call_mode_info_s_type* mode_info_ptr
                           );

  /*---------------------------------------------------------------------------
    This function is invoked to handle bearer modification rejection by the
    network.
  ---------------------------------------------------------------------------*/
  void (*DS_FLOW_MGR_VF_FPN(NwBearerModifyRej))(
                             cm_call_mode_info_s_type* mode_info_ptr
                           ); 
                                 
  /*---------------------------------------------------------------------------
    This function is invoked to handle Network bearer modification indication
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(NwBearerModifyInd))(
                                cm_call_mode_info_s_type* mode_info_ptr,
                                cm_call_id_type           call_id
                              ); 
  /*---------------------------------------------------------------------------
    This function is invoked to handle Network context modification confirmation
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(NwContextModifyCnf))(
                                cm_call_id_type           call_id,
                                cm_call_mode_info_s_type* mode_info_ptr
                              ); 
  /*---------------------------------------------------------------------------
    This function is invoked to handle Network context modification rejection
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(NwContextModifyRej))(
                                cm_call_id_type           call_id,
                                cm_call_mode_info_s_type* mode_info_ptr
                              ); 
  /*---------------------------------------------------------------------------
    This function is invoked to handle Network context modification rejection
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(NwContextModifyInd))(
                                cm_call_id_type           call_id,
                                cm_call_mode_info_s_type* mode_info_ptr
                              ); 
  /*---------------------------------------------------------------------------
    This function is invoked to handle Network context modification rejection
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(NwContextModifyReq))(
                                cm_call_id_type                 call_id,
                                ds_cmd_type                    *cmd_ptr,
                                cm_pdp_act_res_params_s_type   *act_res_params_ptr
                              ); 
  /*---------------------------------------------------------------------------
    This function is invoked to handle Network bearer allocation indication
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(NwBearerAllocationInd))(
                                ds_pdn_context_s         *pdn_context_p,
                                ds_bearer_context_s      *bearer_context_p,
                                const cm_call_mode_info_s_type *mode_info_ptr
                              ); 
  /*---------------------------------------------------------------------------
    This function is invoked to hanlde NW requested bearer activation
  ---------------------------------------------------------------------------*/
  boolean (*DS_FLOW_MGR_VF_FPN(NwBearerActivationReq))(
                                ds_pdn_context_s*         pdn_context_p,
                                ds_bearer_context_s*      bearer_context_p,
                                const cm_call_mode_info_s_type* mode_info_ptr,
                                cm_end_params_s_type*     end_params,
                                sys_modem_as_id_e_type    subs_id
                              );

  /*---------------------------------------------------------------------------
    This function is invoked to verify if UE has any pending requests matching
    the SDF ID passed in the NW call tear down inidication.
  ---------------------------------------------------------------------------*/
  void (*DS_FLOW_MGR_VF_FPN(checkSdfId))(
                        cm_call_mode_info_s_type* mode_info_ptr
                      ); 

  /*---------------------------------------------------------------------------
    This function is invoked to handle UE request to modify an existing bearer
    bearer. Should move bearer stuff to bearer flow manager. 
    But qos checks are in flow manager.
    Make 2nd param Void * to remove the dependency
  ---------------------------------------------------------------------------*/
  int (*DS_FLOW_MGR_VF_FPN(UeBearerModifyReq))(
                           ds_flow_context_s  *flow_context_p,
                           tft_type_T         *nw_tft_p,
                           qos_spec_type      *app_qos,
                           int                flow_state
                          );

  /*---------------------------------------------------------------------------
    This function is invoked to handle UE request to allocate dedicated bearer.
  ---------------------------------------------------------------------------*/
  int (*DS_FLOW_MGR_VF_FPN(UeBearerAllocReq))(
                            ds_pdn_context_s   *pdn_context_p,
                            ds_flow_context_s  *flow_context_p,
                            qos_spec_type      *app_qos,
                            tft_type_T         *nw_tft_p
                          ); 

  /*---------------------------------------------------------------------------
    This function is invoked to hanlde UE request to abort a pending dedicated
    bearer request (allocation/modification).
  ---------------------------------------------------------------------------*/
  int (*DS_FLOW_MGR_VF_FPN(UeBearerAllocAbortReq))(
                                ds_flow_context_s  *flow_context_p
                              ); 

  boolean (*DS_FLOW_MGR_VF_FPN(ProcessCreateTFT))(
                                   ds_pdn_context_s         *pdn_context_p,
                                   ds_bearer_context_s      *bearer_context_p,
                                   const tft_type_T*         nw_tft_p,
                                   const cm_call_mode_info_s_type *mode_info_p,
                                   ds_3gpp_internal_cause_code_enum_type *cause_code_p
                               );

  boolean (*DS_FLOW_MGR_VF_FPN(ProcessDeleteTFT))(
                                   ds_pdn_context_s         *pdn_context_p,
                                   ds_bearer_context_s      *bearer_context_p,
                                   const tft_type_T*         nw_tft_p,
                                   const cm_call_mode_info_s_type *mode_info_p,
                                   ds_3gpp_internal_cause_code_enum_type *cause_code_p
                               );


  boolean (*DS_FLOW_MGR_VF_FPN(allocate_eps_flow_manager))(
                                   int index
                               );


  boolean (*DS_FLOW_MGR_VF_FPN(free_eps_flow_manager))(
                                   int index
                               );

} ds_3gpp_flow_manager_ftbl_s;

extern ds_3gpp_flow_manager_ftbl_s ds_flow_manager_ftbl[DS_3GPPI_SYS_MODE_MAX];



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_3GPP_INIT

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
void ds_flow_manager_3gpp_init( void );


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CREATE_FLOW_CONTEXT

DESCRIPTION
  Create a new flow context.

PARAMETERS
  pdn_p         -  PDN information
  bearer_p      -  Bearer information
  mode_info_p   -  mode specific call info
  app_info_p    -  application passed QOS info

DEPENDENCIES
  None.

RETURN VALUE
 boolean - return DS3G_FAILURE or DS3G_SUCCESS

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_create_flow_context
(
 ds_pdn_context_s                *pdn_context_p,
 ds_bearer_context_s             *bearer_context_p,
 const cm_call_mode_info_s_type  *mode_info_p,
 tft_type_T                             *nw_tft_p,
 ds_3gpp_internal_cause_code_enum_type  *icause_code
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ALLOCATE_NEW_FLOW_MANAGER

DESCRIPTION
  Allocate a new flow manager for this PDN.

PARAMETERS
 pdn_context_p     - Pointer to PDN context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_manager_s * ds_flow_manager_allocate_new_flow_manager
( 
  ds_pdn_context_s *pdn_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_RELEASE_FLOW_MANAGER

DESCRIPTION
  Return flow context instance to the flow context array.

PARAMETERS
  pdn_context_p  - Pointer to the PDN context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_release_flow_manager
(
 ds_pdn_context_s *pdn_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_FLOW_MANAGER

DESCRIPTION
  Find the flow manager for this PDN.

PARAMETERS
 pdn_context_p     - Pointer to PDN context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_manager_s * ds_flow_manager_find_flow_manager
( 
  ds_pdn_context_s *pdn_context_p
);


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_BEARER_CONTEXT_TO_FLOW_MANAGER

DESCRIPTION
  Add a bearer context to the flow manager list.

PARAMETERS
 pdn_context_s        - Pointer to PDN Context.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_add_bearer_context_to_flow_manager
( 
  ds_pdn_context_s   *pdn_context_p,
  ds_bearer_context_s *bearer_context_p
  
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REMOVE_BEARER_CONTEXT_FROM_FLOW_MANAGER

DESCRIPTION
  Remove a bearer context from the flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_remove_bearer_context_from_flow_manager
( 
    ds_pdn_context_s   *pdn_context_p,
    ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_PENDING_FLOW_CONTEXT

DESCRIPTION
  Add a bearer context to the flow manager list.

PARAMETERS
 pdn_context_s        - Pointer to PDN Context.
 flow_context_p       - Pointer to Flow Context.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_add_pending_flow_context
( 
  ds_pdn_context_s   *pdn_context_p,
  ds_flow_context_s  *flow_context_p 
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REMOVE_PENDING_FLOW_CONTEXT

DESCRIPTION
  Remove a pending flow context from the flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Context.
 flow_context_p     - Pointer to flow context.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_remove_pending_flow_context
( 
    ds_pdn_context_s   *pdn_context_p,
    ds_flow_context_s  *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_PRECEDENCE_BIT_MASK

DESCRIPTION
  Update precednce bit mask for the given flow context.

PARAMETERS
 pdn_context_p       - Pointer to PDN Context.
 flow_context_p      - Pointer to flow context.
 req_modify          - boolean. If true, request from modify filter set.
 set_precedence      - boolean. If true, set the bit else clear the bit.
 req_precedence      - boolean. If true, update requested precedence else
                       update precedence bit mask.  

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_precedence_bit_mask
( 
    ds_pdn_context_s   *pdn_context_p,
    ds_flow_context_s  *flow_context_p,
    boolean            req_modify,
    boolean            set_precedence,
    boolean            req_precedence  
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_VERIFY_APP_FILTER_PRECEDENCE

DESCRIPTION
  Verify if the app passed valid precedence value.
  This function will also assign the precedence value.

PARAMETERS
 pdn_context_s       - Pointer to PDN Context.
 input_qos           - Pointer to App passed Qos.
 check_default       - boolean indicating to verify preced is default.
 filter_op           - enum indicating what filters to check.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_verify_app_filter_precedence
( 
  ds_pdn_context_s   *pdn_context_p,
  qos_spec_type      *input_qos,
  boolean             check_default,
  ds_3gpp_app_filter_op filter_op
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_VALIDATE_APP_QOS_FILTER

DESCRIPTION
  Verify the app did not pass invalid src/dest addr info

PARAMETERS
 input_filter           - Pointer to filter.
 flow_type              - Mask indicating the flow type

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_validate_app_qos_filter
(
  ip_filter_type                  *inp_filter,
  qos_spec_field_mask_enum_type    flow_type
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_APP_TFT_TO_NW_TFT

DESCRIPTION
  Converts the APP passed TFT spec to NW filter spec.

PARAMETERS
 nw_tft            -  network TFT to be converted
 app_filter        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_convert_app_tft_to_nw_tft
(
  tft_type_T      *nw_tft,
  qos_spec_type   *app_filter_spec
);


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_PS_CMD

DESCRIPTION
  This function processes flow specific PS commands received by the DS task

PARAMETERS
  IN
    const ds_cmd_type* - Pointer to the command buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_process_ps_cmd
(
 const ds_cmd_type *cmd_ptr 
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FLOW_IOCTL_HDLR

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
int ds_flow_manager_flow_ioctl_hdlr
(
  ps_flow_type            *flow_ptr,
  ps_flow_ioctl_type      ioctl_name,
  void                    *argval_ptr,
  int16                   *ps_errno
  /*lint -esym(715,argval_ptr) */
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_3GPP_PROCESS_SEC_CTX

DESCRIPTION
  Handles the APP request to create a secondary flow. Creates the new PS flow
  and invoke mode specific handler to send the request to NW.  
  Called from application context.

PARAMETERS
  *pdn_ptr          - Ptr to PDN
  *ds_iface_ptr     - Ptr to DS iface
  *iface_ptr        - Ptr to PS iface.
  *ioctl_info       - Ptr to App passed ioctl information

DEPENDENCIES
  None.

RETURN VALUE
  int               - "0" if the sec ctx is brought up.
SIDE EFFECTS
  The function access data structures from APP context.

===========================================================================*/
int ds_flow_manager_3gpp_process_sec_ctx
(
  ds_pdn_context_s                *pdn_ptr,
  ds_3gpp_iface_s                 *ds_iface_ptr,
  ps_iface_type                   *iface_ptr,
  ps_iface_ioctl_qos_request_type *ioctl_info
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_TEARDOWN_SEC_CTX

DESCRIPTION
  Function to process the secondary ctx tear down request from App. Invokes 
  mode specific function to process the teardown.


PARAMETERS
  *flow_context_p   - Ptr to the flow context
  *ps_errno         - Ptr to error code (if any)


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_flow_manager_teardown_sec_ctx
(
  ds_flow_context_s   *flow_context_p ,
  int16                   *ps_errno
  /*lint -esym(715,argval_ptr) */
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_TEARDOWN_BEARER

DESCRIPTION
  Function to process the secondary bearer tear down. Invokes 
  mode specific function to process the bearer teardown.


PARAMETERS
  *bearer_context_p      - Ptr to bearer context
  *preced_conflict       - Ptr to array of precedence conflict values


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_flow_manager_teardown_bearer
(
  ds_bearer_context_s  *bearer_context_p,
  uint32               *preced_coflict,
  boolean              local_deactivate
  /*lint -esym(715,argval_ptr) */
);



/*===========================================================================
FUNCTION DS_FLOW_MANAGER_MODIFY_SEC_CTX

DESCRIPTION
  Function to process the secondary ctx modify request from App. Invokes 
  mode specific function to process the teardown.


PARAMETERS
  *flow_context_p   - Ptr to the flow to context
  *ps_errno         - Ptr to error code (if any)
   subs_id          - Subscription Id


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_flow_manager_modify_sec_ctx
(
  ds_flow_context_s                *flow_context_p,
  ps_flow_ioctl_qos_modify_type    *modify_info,
  int16                            *ps_errno,
  sys_modem_as_id_e_type            subs_id
  /*lint -esym(715,argval_ptr) */
);


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_PENDING_FLOW_CONTEXT_FOR_PDN

DESCRIPTION
  Return a pending flow context based on SDF_ID.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 sdf_id              -  SDF ID to find.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_context_s * - Return valid pointer if flow context exists. Else NULL.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_manager_find_pending_flow_context_for_pdn
( 
  ds_pdn_context_s   *pdn_context_p,
  uint16             sdf_id
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_PENDING_FLOW_CONTEXT

DESCRIPTION
  Return a pending flow context based on SDF_ID.

PARAMETERS
 pdn_cntx_p          -  Output parameter. The pointer is updated, if flow
                        context is found.
 sdf_id              -  SDF ID to find.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_context_s * - Return valid pointer if flow context exists. Else NULL.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_manager_find_pending_flow_context
( 
  ds_pdn_context_s   **pdn_cntx_p,
  uint16             sdf_id
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_APP_FILTERS_TO_DELETE

DESCRIPTION
  Update the NW TFT with the filters to be deleted.

PARAMETERS
 nw_tft            -  network TFT to be converted
 flow_context_p    -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns TRUE if update is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_app_filter_to_delete
(
  tft_type_T          *nw_tft,
  ds_flow_context_s   *flow_context_p
);
 
/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_FILTER_TO_TFT

DESCRIPTION
  Add filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context to add filters.
  nw_tft_p              -  TFT information
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_add_filter_to_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft_p,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p,
  boolean                                allow_flow_create
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REMOVE_FILTER_FROM_TFT

DESCRIPTION
  remove filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context to remove filters.
  nw_tft_p              -  TFT information.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_remove_filter_from_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft_p,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REPLACE_FILTER_IN_TFT

DESCRIPTION
  replace filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context to replace filters.
  nw_tft_p              -  TFT information.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_replace_filter_in_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  const  cm_call_mode_info_s_type       *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION DSFLOWMANAGER_UPDATEDATARATEINFLOW

DESCRIPTION
  Update the data rate in the flow.

PARAMETERS
  bearer_context_p      -  Bearer information
  mode_info_p           -  Mode information

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_data_rate_in_flow
(
 ds_bearer_context_s                   *bearer_context_p,
 const cm_call_mode_info_s_type        *mode_info_p,
 ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_FLOW_IN_TFT

DESCRIPTION
  update the flow values in a TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  nw_tft_p              -  TFT information
  cause_code_p          -  Cause code NW TFT is not accurate
  check_tft             -  Indicates if the TFT needs to checked.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_flow_in_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p, 
  tft_type_T                            *nw_tft,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p,
  boolean                                check_tft
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CREATE_FLOW_CONTEXT_PER_IFACE

DESCRIPTION
  Create a new flow context.

PARAMETERS
  pdn_context_p    -  PDN information
  bearer_context_p -  Bearer information
  mode_info_p      -  mode specific call info.try to reuse for all modes.
  nw_tft_p         -  Pointer to TFT in network format. 
  ds_iface_p       -  Information about the 3GPP IFace.
  ip_type          -  Indicate if it is a V4 or V6 iface.
  icause_code      -  internal cause code to be updated in case of failure.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_create_flow_context_per_iface
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  tft_type_T                      *nw_tft_p,
  ds_3gpp_iface_s                 *ds_iface,
  ip_version_enum_type             ip_type,
  ds_3gpp_internal_cause_code_enum_type *icause_code,
  boolean                          is_bearer_activated
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_TFT_FOR_NW_BEARER_MOD

DESCRIPTION
  Verify parameters in the NW bearer modification request.
  Check if the filter identifier, filter precedence are unique. Update the TFT
  appropriately.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  nw_tft_p              -  TFT information
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_process_tft_for_nw_bearer_mod
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  tft_type_T                            *nw_tft_p,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p,
  boolean                                allow_flow_create
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_FILTER_ID_PRECEDENCE

DESCRIPTION
  Update the filter ID and precedence for the filters.

PARAMETERS
  pdn_context_p    - PDN information
  bearer_context_p - Bearer information
  flow_context_p   - Flow Context information.
  filter_id        - Filter to be removed.
  app_qos_spec     - Original filter spec.
  clear_bit        -  If True will clear the bit else will set.
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_filter_id_precedence
( 
 ds_pdn_context_s     *pdn_context_p,
 ds_bearer_context_s  *bearer_context_p,
 ds_flow_context_s    *flow_context_p, 
 uint32                filter_id,
 qos_spec_type        *app_qos_spec,
 boolean               clear_bit
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_VERIFY_NW_RESPONSE

DESCRIPTION
  Verify parameters in the NW response. Check if the filter identifier
  filter precedence are unique. Check UE requested filters and NW
  responded filters.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow information.
  mode_info_p           -  mode specific call info

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_verify_nw_response
(
 ds_pdn_context_s                *pdn_context_p,
 ds_bearer_context_s             *bearer_context_p,
 ds_flow_context_s               *flow_context_p,
 const cm_call_mode_info_s_type  *mode_info_p,
 tft_type_T                      *nw_tft,
 ds_3gpp_internal_cause_code_enum_type *icause_code
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_COMPARE_MODIFY_QOS

DESCRIPTION
  Compare the App requested modify Qos to current Qos

PARAMETERS
  modiy_qos                -  App requested modify Qos.
  current_qos              -  current App Qos.
  flow_context_p           -  Flow context being modified.
  app_bk                   -  App book keeping info.
  tft                      -  TFT in network format.

DEPENDENCIES
  None.

RETURN VALUE
  boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_manager_compare_modify_qos
(
  qos_spec_type               *modify_qos,
  qos_spec_type               *current_qos,
  ds_flow_context_s           *flow_context_p,
  ds_3gpp_app_modify_qos_bk_s *app_bk,
  tft_type_T                  *nw_tft
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_PS_WITH_NETWORK_RESPONSE

DESCRIPTION
  Update App with network response to UE bearer flow modification request.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  bearer_context_p         -  Pointer to Bearer context.
  flow_context_p           -  Pointer to Flow context.
  is_accepted              -  Indicates if the request was accepted.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_manager_update_ps_with_network_response
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  ds_flow_context_s        *flow_context_p,
  boolean                  is_accepted
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_CAUSE_CODES

DESCRIPTION
  This function is invoked to convert generic cause codes to mode specific
  cause codes.

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  ds_internal_cause -  internal cause code.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_convert_cause_codes
(
  const cm_call_mode_info_s_type         *mode_info_ptr,
  ds_3gpp_internal_cause_code_enum_type  *ds_internal_cause,
  sys_modem_as_id_e_type                  subs_id
); 

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_TEAR_DOWN_SEC_BEARER

DESCRIPTION
  This function is invoked to handle secondary bearer tear down
PARAMETERS
  mode_info_ptr     -  mode into ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_tear_down_sec_bearer
(
  ds_flow_context_s   *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_ALLOC_REJ

DESCRIPTION
  This function is invoked to handle bearer allocation rejection by the 
  network.

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_nw_bearer_alloc_rej
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
);
                         
/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_ALLOC_FAIL

DESCRIPTION
  This function is invoked to handle bearer allocation failure by the 
  network.

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_nw_bearer_alloc_fail
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_MODIFY_REJ

DESCRIPTION
  This function is invoked to handle bearer modification rejection by the
  network.

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_nw_bearer_modify_rej
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
); 
                               
/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_MODIFY_IND

DESCRIPTION
  This function is invoked to handle Network bearer modification indication

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_bearer_modify_ind
(
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id,
  sys_modem_as_id_e_type    subs_id
); 

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_CNF

DESCRIPTION
  This function is invoked to handle Network bearer modification confirmation

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_cnf
(
  cm_call_id_type                 call_id,
  cm_call_mode_info_s_type*       mode_info_ptr,
  sys_modem_as_id_e_type          subs_id
); 

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_REJ

DESCRIPTION
  This function is invoked to handle Network context modification rejection

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_rej
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_IND

DESCRIPTION
  This function is invoked to handle Network context modification indication

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_ind
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_REQ

DESCRIPTION
  This function is invoked to handle Network context modification request

PARAMETERS
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_req
(
  cm_call_id_type                 call_id,
  ds_cmd_type                    *cmd_ptr,
  cm_pdp_act_res_params_s_type   *act_res_params_ptr,
  sys_modem_as_id_e_type          subs_id
);

/*===========================================================================
FUNCTION DSFLOWMANAGER_NWBEARERALLOCATIONIND

DESCRIPTION
  This function is invoked to handle Network bearer allocation indication

PARAMETERS
  pdn_context_p     -  ptr to pdn context
  bearer_context_p  -  ptr to bearer context
  mode_info_ptr     -  mode info ptr

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_bearer_allocation_ind
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const cm_call_mode_info_s_type *mode_info_ptr
); 

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_ACTIVATION_REQ

DESCRIPTION
  This function is invoked to handle Network bearer activation indication

PARAMETERS
  pdn_context_p     -  ptr to pdn context
  bearer_context_p  -  ptr to bearer context
  mode_info_ptr     -  mode info ptr

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_bearer_activation_req
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const cm_call_mode_info_s_type *mode_info_ptr,
  cm_end_params_s_type     *end_params
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CHECK_SDF_ID

DESCRIPTION
  This function is invoked to verify if UE has any pending requests matching
  the SDF ID passed in the NW call tear down inidication.

PARAMETERS
  mode_info_ptr     -  mode info ptr
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_check_sdf_id
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
); 

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UE_BEARER_MODIFY_REQ

DESCRIPTION
  This function is invoked to handle UE request to modify an existing bearer
  bearer. 

PARAMETERS
  flow_context_p    -  ptr to flow context
  nw_tft_p          -  tft ptr
  app_qos           -  qos in app format
  flow_state        -  flow state when the command was posted

DEPENDENCIES
  None.

RETURN VALUE
  int  - Returns 0 if success, -1 otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_manager_ue_bearer_modify_req
(
  ds_flow_context_s  *flow_context_p,
  tft_type_T         *nw_tft_p,
  qos_spec_type      *app_qos,
  int                flow_state
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UE_BEARER_ALLOC_REQ

DESCRIPTION
  This function is invoked to handle UE request to allocate dedicated bearer.

PARAMETERS
  pdn_context_p     -  ptr to pdn context
  flow_context_p    -  ptr to flow context
  app_qos           -  qos in app format
  nw_tft            -  tft

DEPENDENCIES
  None.

RETURN VALUE
  int  - Returns 0 if success, -1 otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_manager_ue_bearer_alloc_req
(
  ds_pdn_context_s  *pdn_context_p,
  ds_flow_context_s *flow_context_p,
  qos_spec_type  *app_qos,
  tft_type_T     *nw_tft
); 

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UE_BEARER_ALLOC_ABORT_REQ

DESCRIPTION
  This function is invoked to hanlde UE request to abort a pending dedicated
  bearer request (allocation/modification).

PARAMETERS
  flow_context_p    -  ptr to flow context

DEPENDENCIES
  None.

RETURN VALUE
  int  - Returns 0 if success, -1 otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_manager_ue_bearer_alloc_abort_req
(
  ds_flow_context_s  *flow_context_p
);                      

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_IFACE_OUT_OF_USE

DESCRIPTION
  The iface has gone out of use. clean up any pending flow contexts and
  update the TFT information to inform network about filter cleanup.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  ds_iface_p               -  Pointer to DS Iface context.
  ip_type                  -  Iface IP type.
  delete_nw_flow           -  boolean. IF true NW flow needs to be deleted.
DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
void ds_flow_manager_iface_out_of_use
(
  ds_pdn_context_s         *pdn_context_p,
  ds_3gpp_iface_s          *ds_iface_p,
  ds_pdn_context_type_e    ip_type,
  boolean                  delete_nw_flow
);


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_NW_FILTER_TO_APP_FILTER

DESCRIPTION
  Converts the NW passed filters spec to App filter spec.

PARAMETERS
 nw_filter            -  network filter to be converted
 ip_filter            -  App filter to store the converted info.
 filter_id            -  filter_id passed by NW
 direction            -  direction in which the filter should be applied.
 flow_context_p       -  Pointer to the flow context

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_manager_convert_nw_filter_to_app_filter
(
  packet_filter_type_T                    *nw_filter,
  ip_filter_type                          *ip_filter,
  byte                                    filter_id,
  ds_tft_pkt_filter_direction_enum_type   direction,
  ip_version_enum_type                    ip_type,
  ds_flow_context_s                       *flow_context_p,
  ds_3gpp_internal_cause_code_enum_type    *cause_code_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONFIGURE_DEFAULT_FLOW

DESCRIPTION
  This function is invoked to configure the default flow for a given IFACE.
  Here we bind flow to the default bearer phys_link and configure the
  flow_ioctl_hdlr for the default flow.

PARAMETERS
  iface_p            -  Pointer to IFACE.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_configure_default_flow
(  
  ds_pdn_context_s          *pdn_context_p,
  ds_bearer_context_s       *bearer_context_p,
  ps_iface_addr_family_type  iface_type
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ACCEPT_MODIFY_ENABLE_FILTER

DESCRIPTION


PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_accept_modify_enable_filter
(
  ds_bearer_context_s                   *bearer_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_CREATE_TFT

DESCRIPTION
  This function is invoked to handle the Create TFT operation.

PARAMETERS
  pdn_context_p     -  PDN context info.
  bearer_context_p  -  Bearer context info
  nw_tft_p          -  TFT information.
  mode_info_ptr     -  Extended information about this operation.
  cause_code_p      -  Cause code in case of failure.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_process_create_tft
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const tft_type_T*         nw_tft_p,
  const cm_call_mode_info_s_type *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_DELETE_TFT

DESCRIPTION
  This function is invoked to handle the Delete Existing TFT operation.

PARAMETERS
  pdn_context_p     -  PDN context info.
  bearer_context_p  -  Bearer context info
  nw_tft_p          -  TFT information.
  mode_info_ptr     -  Extended information about this operation.
  cause_code_p      -  Cause code in case of failure.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_process_delete_tft
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const tft_type_T*         nw_tft_p,
  const cm_call_mode_info_s_type *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
);

/*===========================================================================
FUNCTION ds_flow_manager_update_uplink_flow_capability

DESCRIPTION
  This function is used to enable or disable uplink flow based on 
  flow direction (This is based on GBR values granted by NW) 

PARAMETERS
 bearer_context_p: Bearer for which data path is to be setup
 boolean:  to Enable or Disable flow
 
DEPENDENCIES
  None.

RETURN VALUE
None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_update_uplink_flow_capability
(
  ds_bearer_context_s *bearer_context_p,
  boolean              ul_flow_enable
);

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FREE_NW_FLOW_CONTEXT

DESCRIPTION
  Utility function used to free the nw flow contexts associated with a given
  bearer.

PARAMETERS
  bearer_context_p     (in) -  Bearer context info. 
 
DEPENDENCIES
  None.

RETURN VALUE
 TRUE - Success
 FALSE - Failure.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_free_nw_flow_context(ds_bearer_context_s *bearer_context_p);

/*===========================================================================
FUNCTION ds_flow_manager_validate_flow_manager

DESCRIPTION
  This function checks if the flow manager tbl has been allocated dynamic memory

PARAMETERS
  flow_manager_p       - flow_manager to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
inline boolean ds_flow_manager_validate_flow_manager
(
  ds_flow_manager_s *flow_manager_p
);

/*===========================================================================
FUNCTION ds_flow_manager_free_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
 pdn_context_p 
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
void ds_flow_manager_free_dyn_mem
(
  ds_flow_manager_s *flow_manager_p
);

#endif /* FEATURE_DATA_3GPP */
#endif /* DS3GPPFLOWMANAGER_H */

