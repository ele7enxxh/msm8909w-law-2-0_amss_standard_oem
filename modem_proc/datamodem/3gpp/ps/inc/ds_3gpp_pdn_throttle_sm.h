/*!
  @file
  ds_3gpp_throttle_sm.h

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_pdn_throttle_sm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/11   ss      Changes to support RPM.
08/16/10   hs      Initial version

===========================================================================*/

#ifndef DS_3GPP_PDN_THROTTLE_SM_H
#define DS_3GPP_PDN_THROTTLE_SM_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "rex.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_3gpp_pdn_context.h"
#include "ps_iface_defs.h"
#include "ps_sys_ioctl.h"
#include "cm.h"
#include "ds_3gpp_apn_table.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_3GPP_PDN_THROT_INFINITE_THROT 0xFFFFFFFF
#define DS_PDN_THROT_LOG_PARAM_NA   0x0F
#define DS_PDN_THROT_MAX_APN_LOG_LENGTH  6
#define DS_PDN_THROT_LOG_LIST_TYPE_MAX (DS_MAX_APN_NUM * 2)

typedef enum 
{
     DS_3GPP_UNTHROTTLED = 0,
     DS_3GPP_THROTTLED_V4,
     DS_3GPP_THROTTLED_V6,
     DS_3GPP_THROTTLED_V4V6
} ds_3gpp_pdn_throttle_state_e;

/*------------------------------------------------------------------------
  Structure to hold information about pdn throttling for logging
-------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8         subs_id;
  unsigned int  is_throttled:4;
  uint8         throttle_ip_type:4;
  uint8         profile_id;
  uint8         counter;  
  uint16        duration;
  uint16        mcc;
  uint16        mnc;
  char          apn[6];
  char          reserved[4];
} ds_3gpp_pdn_throttle_event_type;

/*------------------------------------------------------------------------------
  This enumeration will indicate the kind of Throttling being applied. 
------------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED = 0,
                           /*Throttling type was not specified*/
  DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT,
                           /*Throttle type because Max limit of successful 
                             PDN Connections was reached*/
  DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME,
                           /*Throttle type because UE initiated PDN Disconnection*/
  DS_3GPP_THROTTLE_TYPE_T3396,
                         /*Throttle type because of T3396 Timer*/
  DS_3GPP_THROTTLE_TYPE_T3396_ABSENT
                         /*Throttle type set when T3396 Timer value is received as 0*/
} ds_3gpp_pdn_throttle_sm_type;

/*------------------------------------------------------------------------------
  This structure stores the PDN Throttling information for a given RAT
------------------------------------------------------------------------------*/
typedef struct
{
  ds_3gppi_throttle_sm_s           *v4_throttle_sm;
                                      /* V4 Throttle SM */
  ds_3gppi_throttle_sm_s           *v6_throttle_sm;
                                      /* V6 Throttle SM */
  ds_3gpp_pdn_throttle_sm_type     sm_type;
                                      /* type of PDN Throttling */
  void                             *pdn_throt_sm_info_p;
                                      /* back pointer to parent */
  boolean                          init;
                                      /* indicates whether throttling 
                                         info is valid */

}ds_3gpp_pdn_throttle_sm_s;

/*------------------------------------------------------------------------------
  This structure stores the PDN Throttling information on a particular PLMN
  for all RATs
------------------------------------------------------------------------------*/
typedef struct
{
  ds_3gpp_pdn_throttle_sm_s  per_rat[DS_3GPP_THROT_RAT_MAX];
                                       /* Holds PDN Throttling Information
                                          for all RATs */
  sys_plmn_id_s_type         plmn_id;
                                       /* PLMN Id */
  uint8                      num_active_rats;
                                       /* Number of RATs whose throttling 
                                          is valid */
  void                       *apn_throt_sm_ptr;
                                       /* Back Pointer to parent */
} ds_3gpp_pdn_throttle_sm_info_s;

typedef struct 
{
  byte       apn[DS_UMTS_MAX_APN_STRING_LEN+1];
  uint8      len;
  boolean    attach_allowed; 
}ds_3gpp_pdn_throt_apn_list_type;

/*------------------------------------------------------------------------------
  This structure stores the identification information used to fetch the
  handle to the correct PDN Throttle SM.
------------------------------------------------------------------------------*/
typedef struct
{
  byte                          *apn;
  ds_3gpp_pdn_throttle_sm_type   sm_type;
  sys_modem_as_id_e_type         subs_id;
  sys_plmn_id_s_type             plmn_id;
  boolean                        globally_blocked;
  ds_3gpp_throt_rat_e_type       rat_type;
} ds_3gpp_pdn_throt_ident_info_type;

/*------------------------------------------------------------------------------ 
  This structure stores the profile number only if a particule apn is PDN
  throttled. This is needed to log the throttle information when timer expires
  We will not have access to the throttled profile information otherwise
  ------------------------------------------------------------------------------*/
typedef struct
{
  ds_3gppi_throttle_sm_s *throttle_sm_ptr;
  uint16                  profile_num;
}ds_3gpp_pdn_throt_log_list_type;

typedef struct
{
  boolean                 v4_throttle_status;
  boolean                 v6_throttle_status;
}ds_3gpp_pdn_throt_temp_status_type;

/*---------------------------------------------------------------------------- 
  This structure is used to update the apn from EFS and on receiving a
  PDN connectivity Reject, a decision is made to set block_pdn to TRUE based
  on the cause code received.
  ----------------------------------------------------------------------------*/
typedef struct
{
  boolean block_pdn;
  byte *apn;
  rex_timer_type  apn_reject_sm_timer;
  uint16  profile_num;
  boolean is_initialized;
}ds_3gpp_pdn_throt_apn_reject_sm_s;

extern ds_3gpp_pdn_throt_apn_reject_sm_s ds_3gpp_pdn_throt_apn_reject_sm[SYS_MODEM_AS_ID_MAX];

typedef struct
{
  int   subs_id;
  void  *throt_sm_ptr;
}ds_3gpp_pdn_throt_sm_info_type;

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_THROTTLE_TIMER_FOR_PDN_THROT_SM

DESCRIPTION
  This function returns the throttle timer value for given PDN throt SM

PARAMETERS   :
  pdn_throt_sm   - PDN throt SM for which throttle timer value is retrieved.

DEPENDENCIES
  None.

RETURN VALUE
  uint32.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_3gpp_pdn_throt_get_throttle_timer_for_pdn_throt_sm
(
  ds_3gpp_pdn_throttle_sm_s* pdn_throt_sm_ptr
);
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLEAR_THROTTLE_STATE

DESCRIPTION
  This function clears throttling state for a pdn context and a given ip
  type.

PARAMETERS   :
   ds_pdn_context_s *         - pdn context ptr
   ds_umts_pdp_type_enum_type - pdp type
   rat_mask                   - Mask Indicating the RAT Types for which
                                throttling needs to be cleared
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_clear_throttle_state
(
  byte                       *apn,
  ds_umts_pdp_type_enum_type  pdn_pdp_type,
  sys_modem_as_id_e_type      subs_id,
  uint32                      rat_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_PER_APN_POWER_UP_INIT

DESCRIPTION
  This function initializes the data structure needed to perform PDN throt
  on a per APN basis.

  This initializes the APN names to NULL and initializes the pointer array
  of PDN throt SM per APN to NULL

PARAMETERS
   None

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_sm_per_apn_power_up_init
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_INFO_FOR_PLMN

DESCRIPTION
  This function retrieves the pdn throttle SM info for a specific
  plmn_id. It is used by IOCTL handler
  DS_3GPP_PDN_CNTX_GET_THR0TTLE_INFO

PARAMETERS   :
   plmn_id        - plmn_id
   *throttle_info - retrieved throttle info is put here
   subs id        - Subscription Id
   RAT Index      - Indicates the RAT for which throttling info is 
                    needed
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
(
  sys_plmn_id_s_type                   *plmn_ptr,
  ps_sys_ioctl_pdn_throttle_info_type  *throttle_info,
  ds_3gpp_throt_rat_e_type              rat_index,
  sys_modem_as_id_e_type                subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM_FROM_PLMN

DESCRIPTION
  This function retrieves the pdn throttle_sm for a specific plmn_id and apn
  for the global rat mask

PARAMETERS   :
   plmn_id - plmn_id
   *apn    - the apn for which the pdn throttle_sm is to be retrieved.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void *ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn
(
  sys_plmn_id_s_type     plmn_id,
  byte                   *apn,
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM_FROM_PLMN

DESCRIPTION
  This function retrieves the pdn throttle_sm_info pointer for a specific 
  plmn_id and apn.

PARAMETERS   :
   plmn_id - plmn_id
   *apn    - the apn for which the pdn throttle_sm is to be retrieved.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/


void *ds_3gpp_pdn_throt_sm_get_throttle_sm_info_from_plmn
(
  sys_plmn_id_s_type     plmn_id,
  byte                   *apn,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM

DESCRIPTION
  This function gets a Throttling state machine instance ptr for a apn on the 
  current plmn. If the pointer does not exist ;an entry will be allocated.
  NOTE: Use this function if you want to allocate/fetch a PDN Throttling 
  Pointer for a particular RAT

PARAMETERS   :
  throt_ident_info_p - Information to identify/create the correct PDN Throttle 
                       SM
  allow_create       - Flag indicating whether creation is allowed

DEPENDENCIES
  None.

RETURN VALUE
 Pointer to the Throttling state machine

SIDE EFFECTS
  None

===========================================================================*/
void* ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat
(
  ds_3gpp_pdn_throt_ident_info_type *throt_ident_info_p,
  boolean                            allow_create
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM

DESCRIPTION
  This function gets a Throttling state machine instance ptr for a apn on the 
  current plmn. If the pointer does not exist ;an entry will be allocated.
  NOTE: Use this function if you do not need to allocate/fetch a PDN Throttling 
  Pointer for any particular RAT

PARAMETERS   :
  apn              - 
  globally_blocked - Flag indicating if globally blocked throttling state machine
                     is needed
  allow_create     - Flag indicating whether creation is allowed
  sm_type          - Throttling state machine type requested
  subs_id          - Subscription id of the client
DEPENDENCIES
  None.

RETURN VALUE
 Pointer to the Throttling state machine

SIDE EFFECTS
  None

===========================================================================*/
void* ds_3gpp_pdn_throt_sm_get_throttle_sm
(
  byte                          *apn,
  boolean                        globally_blocked,
  boolean                        allow_create,
  ds_3gpp_pdn_throttle_sm_type   sm_type,
  sys_modem_as_id_e_type         subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM_PLMN_BASED

DESCRIPTION
  This function gets a Throttling state machine instance ptr for a apn, plmn,
  subscription id pair. If the pointer does not exist ;an entry will be allocated

PARAMETERS   :
  throt_ident_info_p - Information to identify the correct PDN Throttle SM
  allow_create       - Flag indicating whether creation is allowed

DEPENDENCIES
  None.

RETURN VALUE
 Pointer to the Throttling state machine

SIDE EFFECTS
  None

===========================================================================*/

void* ds_3gpp_pdn_throt_sm_get_throttle_sm_plmn_based
(
  ds_3gpp_pdn_throt_ident_info_type *throt_ident_info_p,
  boolean                           allow_create
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_PDN_THROTTLED

DESCRIPTION
  This function clears throttling state for a pdn context and a given ip
  type.

PARAMETERS   :
   byte* - apn name ptr
   ds_umts_pdp_type_enum_type - pdp type
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_throt_is_apn_throttled
(
  byte                       *apn,
  ds_pdn_context_type_e       call_type,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_HANDLE_CONN_RESPONSE

DESCRIPTION
  This function handles the PDP/PDN connection response. Delegates the
  response to the RPM handler function for appropriate action.

PARAMETERS   :
  conn_status    - whether network accepted or rejected.
  *pdn_context_p - pdn_context_ptr.
  down_reason    - the ps_iface_cause code for reject.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_handle_conn_response
(
  boolean                       conn_status,
  ds_pdn_context_s*             pdn_context_p,
  ps_iface_net_down_reason_type down_reason
);

#ifdef FEATURE_DATA_RPM
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* FEATURE_DATA_RPM */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_PREV_CAUSE_CODE

DESCRIPTION
  This function returns the previous IPV4 and IPV6 cause codes stored in
  the core throttle state machine
 
PARAMETERS   : 
 
  pdn_throt_sm_ptr  -  PDN throttle state machine pointer to fetch the cause
 					   code from the core throttle state machine
  v4_cause_code_ptr -  Failure cause code ptr to populate V4 causecode
  v6_cause_code_ptr -  Failure cause code ptr to populate V6 causecode 

DEPENDENCIES
  None.

RETURN VALUE
  boolean -  TRUE indicating SUCCESS
		     FALSE indicating pdn_throt sm ptr has not been allocated

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_throt_get_prev_cause_code
(
  void 							    *pdn_throt_sm_ptr,
  ps_iface_net_down_reason_type                             *v4_cause_code_ptr,
  ps_iface_net_down_reason_type		                    *v6_cause_code_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_SM_TYPE

DESCRIPTION
  This function gets the PDN Throttle SM Type.
 
PARAMETERS   : 
  APN Name
  Whether the APN is globally blocked
  SM Type to be returned
DEPENDENCIES
  None.

RETURN VALUE
  boolean -  TRUE indicating SUCCESS
             FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_throt_get_sm_type
(
  byte                          *apn,
  sys_modem_as_id_e_type         subs_id,
  boolean                        globally_blocked,
  ds_3gpp_pdn_throttle_sm_type  *throt_sm_type_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_FAILURE_CAUSE_CODE

DESCRIPTION
  This function sets the failure cause based on the PDP type
 
PARAMETERS   : 
 
  pdn_throt_sm_ptr					 -   PDN throttle state machine pointer to
 									               set the cause code in the core
									               throttle state machine
  cause_code			        	 -   Failure cause code to be set
  pdn_pdp_type                       -   PDP type
									  

DEPENDENCIES
  None.

RETURN VALUE
  boolean -  TRUE indicating SUCCESS
             FALSE indicating pdn_throt sm ptr has not been allocated

SIDE EFFECTS
  None.
============================================================================*/
boolean ds_3gpp_pdn_throt_set_failure_cause_code
(
  void                          *pdn_throt_sm_ptr,
  void                          *cause_code,
  ds_umts_pdp_type_enum_type    pdn_pdp_type
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_HANDLE_PERM_CONN_FAILURE

DESCRIPTION
  This function sets INFINITE THROTTLING for a PDN context.
  Set the is_throttled flag to TRUE, Set the Timer value to 0xFFFFFFFF
  

PARAMETERS   : 
  pdn_throt_sm_ptr					 -   PDN throttle state machine pointer to
 									               set the cause code in the core
									               throttle state machine
  pdn_pdp_type                       -   PDP type
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_handle_perm_conn_failure
(
  void                        *pdn_throt_sm_ptr,
  void                        *cause_code,
  ds_umts_pdp_type_enum_type   pdn_pdp_type,
  sys_modem_as_id_e_type       subs_id,
  ds_pdn_context_s             *pdn_context_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLEAR_INFINITE_THROTTLING

DESCRIPTION
  This function clears INFINITE THROTTLING for a PDN context.
  The is_throttled flag is set to FALSE and the Timer value is cleared.
  

PARAMETERS: 
 subs_id
 clear_t3396_throttling
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_clear_infinite_throttling
(
  sys_modem_as_id_e_type subs_id,
  boolean                clear_t3396_throttling
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_ANY_APN_THROTTLED_FOR_MAX_FAILURE

DESCRIPTION
  This function is used to check of any APN is throttled for max count 
  

PARAMETERS: 
  subs_id.
 
DEPENDENCIES
  None.

RETURN VALUE
  true - if any APN is throttled and fail count reached maximum
   flase - otherwise.
 
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_pdn_throt_is_any_apn_throttled_for_max_failure
(
  sys_modem_as_id_e_type subs_id
);
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_SET_SM_TYPE

DESCRIPTION
  This function sets the pdn throttling state machine type
  
PARAMETERS   : 
  pdn_throttle_sm_ptr - pdn throttling pointer
  sm_type             - state machine type

DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_sm_set_sm_type
(
  ds_3gpp_pdn_throttle_sm_s *pdn_throttle_sm_ptr,
  ds_3gpp_pdn_throttle_sm_type     sm_type
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_RESET_PDN_THROTTLING

DESCRIPTION
  This function clears cause code specific THROTTLING for a PDN context.
  The is_throttled flag is set to FALSE and the Timer value is cleared.
  

PARAMETERS: 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_reset_pdn_throttle(sys_modem_as_id_e_type);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_PERFORM_REG_THROTTLING

DESCRIPTION
  This function performs regular throttling and sets failure cause code
  

PARAMETERS   : 
  pdn_throt_sm_ptr      -  PDN throttle state machine pointer to
                           Set the cause code in the core
                           throttle state machine
  ds_pdn_context_s      -  Rejected PDN cntxt pointer
  cause_code            -  ESM cause code (void *)
  pdn_pdp_type          -  PDP type
  rat_type              -  RAT on which throttling needs to be performed
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_perform_reg_throttling
(
  void	                      *pdn_throt_sm_ptr,
  ds_pdn_context_s            *pdn_cntxt_ptr,
  void                        *cause_code,
  ds_umts_pdp_type_enum_type   pdn_pdp_type,
  ds_3gpp_throt_rat_e_type     rat_type
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_VALIDATE_MIN_FAILURE_COUNT

DESCRIPTION
  This function ensures that the throttle failure count is atleast
  equal to the minimum throttle failure count passed
  

PARAMETERS   : 
  ds_pdn_context_s           -  Rejected PDN cntxt pointer
  min_throt_failure_cnt      -  The min failure count of V4/V6 core
                                throttle SM
  pdn_pdp_type               -  PDP type
  rat_type                   -  RAT on which throttling needs to be performed
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_validate_min_failure_count
(
  ds_pdn_context_s               *pdn_cntxt_ptr,
  ds_umts_pdp_type_enum_type      pdn_pdp_type,
  uint8                           min_throt_failure_cnt,
  ds_3gpp_throt_rat_e_type        rat_type
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_PERFORM_APN_THROTTLING

DESCRIPTION
  This function performs PDN throttling based on Release 10 requirements
  

PARAMETERS   : 
  pdn_throt_sm          -  PDN throttle state machine pointer to
 									              set the cause code in the core throttle
                                state machine
  ds_pdn_context_s           -  Rejected PDN cntxt pointer
  cause_code                 -  ESM cause code (void *)
  pdn_pdp_type               -  PDP type
  is_global                  -  Flag indicating if throttling is global
                                or per plmn
  ds_pdn_context_s           -  pdn_cntxt_ptr
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_perform_apn_throttling
(
  void                                *pdn_throt_sm_ptr,
  byte                                *apn,
  void                                *cause_code,
  ds_umts_pdp_type_enum_type  	       pdn_pdp_type,
  sys_modem_as_id_e_type               subs_id,
  boolean                              is_global,
  ds_pdn_context_s                    *pdn_cntxt_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_PERFORM_ROAMING_THROTTLING

DESCRIPTION
  This function performs PDN throttling based on Roaming Requirement.
  

PARAMETERS
  APN name to be throttled
  Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_perform_roaming_throttling
(
  byte                                *apn,
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_APN_DISABLE_POWERUP_THROT

DESCRIPTION
  This function performs apn disable specifc throttling at power up

PARAMETERS
  Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_apn_disable_powerup_throttle
(
  sys_modem_as_id_e_type               subs_id
);
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_PERFORM_APN_DISABLED_THROTTLING

DESCRIPTION
  This function performs PDN throttling because APN was disabled.
  

PARAMETERS
  APN name to be throttled
  Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_perform_apn_disabled_throttling
(
  byte                                *apn,
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_PERFORM_APN_DISABLED_THROTTLING

DESCRIPTION
  This function performs PDN throttling because UI Data was disabled.
  

PARAMETERS
  APN name to be throttled
  Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_throt_perform_ui_data_disabled_throttling
(
  byte                                *apn,
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_ROAMING_THROTTLING

DESCRIPTION
  This function unblocks roaming throttling for given APN or for all
  APNs blocked due to roaming.
  
PARAMETERS
  1. APN name to be throttled
  2. Subscription Id
  3. Flag to indicate whether only the given APN should be unblocked or
     all APNs blocked due to roaming should be unblocked.
  4. Boolean Pointer set to True if the attach apn is unblocked
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_unblock_roaming_throttling
(
  byte*                                apn,
  sys_modem_as_id_e_type               subs_id,
  boolean                              unblock_all_apns,
  boolean*                             is_attach_apn_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_APN_DISABLED_THROTTLING

DESCRIPTION
  This function unblocks PDN throttling because APN was re-enabled.
  
PARAMETERS
  1. APN name
  2. Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_unblock_apn_disabled_throttling
(
  byte*                                apn,
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_APN_DISABLED_THROTTLING

DESCRIPTION
  This function unblocks PDN throttling because UI Data was re-enabled.
  
PARAMETERS
  1. APN name
  2. Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_unblock_ui_data_disabled_throttling
(
  byte*                                apn,
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_APN_THROT_SET_T3396_TIMER

DESCRIPTION   This function saves the PDN throttling t3396 timer value 
              corresponding to the APN for which it was received.

PARAMETERS 
apn - apn name for which it was received 
timer_val - t3396 timer value 
plmn_id   - plmn passed by the client
subs_id   - subscription id passed by the client

RETURN VALUE 
void 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gpp_apn_throt_set_t3396_timer 
(
  byte                   *apn,
  uint32                 timer_val,
  sys_plmn_id_s_type     plmn_id,
  sys_modem_as_id_e_type subs_id,
  ps_iface_net_down_reason_type down_reason 
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_RESET_REL_10_PERM_PLMN_SM

DESCRIPTION
  This function resets the pdn throttling state machine for a release 10
  based throttling sm. 
  
PARAMETERS   : 
  pdn_throttle_sm_ptr - pdn throttling pointer
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_sm_reset_rel_10_per_plmn_sm
(
  ds_3gpp_pdn_throttle_sm_s *per_plmn_throt_sm_ptr,
  sys_modem_as_id_e_type     subs_id
);
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_GLOBAL_THROTTLE_FLAG

DESCRIPTION
  This function sets the global throttle flag for the given APN

PARAMETERS   : 
  apn - APN name
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_set_global_throttle_flag
(
  byte                     *apn,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_ROAMING_THROTTLE_FLAG

DESCRIPTION
  This function sets the Roaming throttle flag for the given index

PARAMETERS
  Index ptr - Index to the APN based throttle SM.
  Flag      - Indicates throttle or no throttle
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_set_roaming_throttle_flag
(
  void    *index_ptr,
  boolean  flag
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_APN_DISABLED_FLAG

DESCRIPTION
  This function sets the APN Disabled flag for the given index.

PARAMETERS
  Index ptr - Index to the APN based throttle SM.
  Flag      - Indicates throttle or no throttle
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_set_apn_disabled_flag
(
  void    *index_ptr,
  boolean  flag
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_APN_DISABLED_FLAG

DESCRIPTION
  This function sets the UI Data Disabled flag for the given index.

PARAMETERS
  Index ptr - Index to the APN based throttle SM.
  Flag      - Indicates throttle or no throttle
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_set_ui_data_disabled_flag
(
  void    *index_ptr,
  boolean  flag
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_ROAMING_THROTTLE_FLAG

DESCRIPTION
  This function gets the Roaming throttle flag for the given index

PARAMETERS
  Index ptr - Index to the APN based throttle SM.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if throttled
  FALSE, otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_get_roaming_throttle_flag
(
  void    *index_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_BLOCKED_APN_LIST_ON_PLMN_ID

DESCRIPTION
  This function gets the blocked list of of APNs from the PDN throt
  SM on the passed PLMN ID
  
PARAMETERS   : 
 
  list - list of blocked APNs (output parameter)

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_get_blocked_apn_list_on_plmn_id
(
  ds_3gpp_pdn_throt_apn_list_type *blocked_apn_list,
  uint8                           *num_blocked_apn,
  sys_modem_as_id_e_type           subs_id,
  sys_plmn_id_s_type               plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ADVERTISE_GLOBALLY_BLOCKED_APNS_to_APM

DESCRIPTION
  This function sends the currently blocked list of of APNs to APM
  
PARAMETERS   : 
  globally_blocked_apn_list
  num_blocked_apns
 
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_advertise_globally_blocked_apns_to_apm
(
  ds_3gpp_pdn_throt_apn_list_type *globally_blocked_apn_list,
  uint8                            num_blocked_apns,
  sys_modem_as_id_e_type           subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_APN_TIMER_EXP_CMD_HDLR

DESCRIPTION
  This function processes cmd to unblock the APN on expiry of the throttling
  timer
  
PARAMETERS   : 
  apn
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_unblock_apn_timer_exp_cmd_hdlr
(
  void                  *throt_sm_p,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_ALL_APN_GLOBALLY_BLKED_APN

DESCRIPTION
  This function processes cmd to unblock all the globally blocked APN
  
PARAMETERS   : 
  void
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_unblock_all_globally_blked_apn
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR_CMD_HDLR

DESCRIPTION
  This function timestamps the APN throttle timer event in EFS
  
PARAMETERS   : 
  throt_sm_p      - pointer to core Throt state machine
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_timestamp_apn_throt_tmr_cmd_hdlr
(
  void*                  throt_sm_p,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IMSI_INFO_AVAIL_CMD_HDLR

DESCRIPTION
  This function checks to see if the IMSI info matches the one stored in EFS.
  If yes, powerup global APN throttling is performed. Else, it has already
  been done, so nothing is done in this function.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_imsi_info_avail_cmd_hdlr
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_APN_GLOBALLY_THROTTLED

DESCRIPTION
  This function checks if given APN is globally throttled across all PLMNs.

PARAMETERS   :
   byte* - apn name ptr
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_throt_is_apn_globally_throttled
(
  byte                   *apn,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_APN_ROAMING_THROTTLED

DESCRIPTION
  This function checks if given APN is throttled due to roaming.

PARAMETERS   :
   byte* - apn name ptr
   Subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if throttled
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_throt_is_apn_roaming_throttled
(
  byte                   *apn,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_APN_DISABLED

DESCRIPTION
  This function checks if given APN is disabled.

PARAMETERS   :
   byte* - apn name ptr
   Subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if throttled
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_throt_is_apn_disabled
(
  byte                   *apn,
  sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_APN_BLOCKED_ON_ALL_PLMNS_ROAMING

DESCRIPTION
  This function checks if given APN is blocked on all the plmns or blocked
  due to roaming

PARAMETERS   :
   byte* - apn name ptr
   Subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if throttled
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_throt_is_apn_blocked_allplmns_or_roaming
(
  byte                   *apn,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_UI_DATA_DISABLED

DESCRIPTION
  This function checks if UI Data is disabled.

PARAMETERS   :
   byte*   - apn name ptr
   subs_id - Subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if throttled
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_throt_is_ui_data_disabled
(
  byte                   *apn,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_APN_THROTTLE_INIT

DESCRIPTION
  This function does the powerup initialization for the APN throttle
  functionality.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_apn_throt_init
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_APN_THROT_CLEAR_T3396_TMR_ENTRY

DESCRIPTION   This function resets the t3396 timer entry from local cache and 
              also the EFS file.
 
PARAMETERS 
apn - apn name for which it was received 
plmn_id   - plmn passed by the client
subs_id   - subscription id passed by the client

RETURN VALUE 
void 

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_apn_throt_clear_t3396_tmr_entry 
(
  byte                      *apn,
  sys_plmn_id_s_type         plmn_id,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UMTS_HANLDE_REJECT_WITH_T3396_IE

DESCRIPTION
  This function handles the reject message received with a t3396 IE
  based on Release 10 requirements
  

PARAMETERS   : 
  gw_ps_call_info 					 -  GW PS call info received in reject from CM
  apn                        -  APN name on the PDP context
  down_reason                
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_umts_handle_reject_with_t3396_ie
(
  cm_gw_ps_call_info_s_type    *gw_ps_call_info,
  byte                         *apn,
  ps_iface_net_down_reason_type down_reason,
  sys_modem_as_id_e_type        subs_id,
  ds_pdn_context_s             *pdn_context_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_LTE_HANLDE_REJECT_WITH_T3396_IE

DESCRIPTION
  This function handles the reject message received with a t3396 IE
  based on Release 10 requirements
  

PARAMETERS   : 
  lte_call_info 				  	 -  LTE call info received in reject from CM
  apn                        -  APN name on the PDN connection
  down_reason
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_lte_handle_reject_with_t3396_ie
(
  cm_lte_call_info_s_type      *lte_call_info,
  byte                         *apn,
  ps_iface_net_down_reason_type down_reason,
  sys_modem_as_id_e_type        subs_id,
  ds_pdn_context_s             *pdn_context_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_HANLDE_NW_INITED_UNBLOCK_APN 
 
DESCRIPTION
  This function handles the network initiated to unblock a globally blocked 
  APN based on Release 10 requirements
  

PARAMETERS   : 
  apn                        -  APN name on the PDN connection
  subs_id                    -  Subscription Id
  rat_mask                   -  Mask Indicating the RAT Types for which
                                throttling needs to be unblocked

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn
(
  byte                     *apn,
  sys_modem_as_id_e_type    subs_id,
  uint32                    rat_mask
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SAMPLING_TIMER_EXP_CMD_HDLR 
 
DESCRIPTION
  This function handles the sampling timer expiration command.
  
PARAMETERS   : 
  void

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_sampling_timer_exp_cmd_hdlr
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_GET_APN_NAME

DESCRIPTION
  This function obtains the list of APNs throttled on
  current PLMN and advertises this list to APM.

PARAMETERS
  Subscription Id            
 
DEPENDENCIES
  None.

RETURN VALUE 
  None 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION  FUNCTION DS_3GPP_PDN_THROT_ADVERTISE_BLOCKED_APNS_WITH_PLMN

DESCRIPTION
  This function obtains the list of APNs throttled on
  PLMN passed as argument and advertises this list to APM.

PARAMETERS
  Subscription Id            
  PLMN
 
DEPENDENCIES
  None.

RETURN VALUE 
  None 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_advertise_blocked_apns_with_plmn
(
  sys_modem_as_id_e_type    subs_id,
  sys_plmn_id_s_type        plmn_id
);


/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_HDL_GEN_EVENT

DESCRIPTION
  This function would populate the structure and generate an event
  when pdn throttling happens

PARAMETERS
   uint8                            failure_count
   uint16                           timer_val
   boolean                          is_throttled
   sys_modem_as_id_e_type           subs_id
   sys_plmn_id_s_type               *plmn_id
   uint8                            throttle_ip_type
   char                             *apn
   uint8                            profile_id
 
DEPENDENCIES

RETURN VALUE 
  void
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_hdl_gen_event
(
   uint8                            failure_count,
   uint32                           timer_val,
   boolean                          is_throttled,
   sys_modem_as_id_e_type           subs_id,
   sys_plmn_id_s_type               *plmn_id,
   uint8                            throttle_ip_type,
   char                             *apn,
   uint8                            profile_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_ADD_LOG_ENTRY

DESCRIPTION
  This function would add profile number to pdn_throt_profile_num_list

PARAMETERS
   ds_3gppi_throttle_sm_s*     throt_sm_ptr,
   uint16                      profile_num
 
DEPENDENCIES
  None

RETURN VALUE 
  void
 
SIDE EFFECTS

===========================================================================*/

void ds_3gpp_pdn_throt_add_log_entry
(
   ds_3gppi_throttle_sm_s     *throt_sm_ptr,
   uint16                      profile_num  
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_EV_ALLOC_MEM

DESCRIPTION
  This function would allocate memory for pdn_throt_ev_ptr from heap

PARAMETERS
  Void
 
DEPENDENCIES
  None.

RETURN VALUE 
  ds_3gpp_pdn_throttle_event_type ptr
 
SIDE EFFECTS
  None.

===========================================================================*/

ds_3gpp_pdn_throttle_event_type *ds_3gpp_pdn_throt_ev_alloc_mem
(
   void 
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_GEN_EVENT

DESCRIPTION
  This function would generate an event when pdn throttling
  happens

PARAMETERS
  ds_3gpp_pdn_throttle_event_type ptr
 
DEPENDENCIES
  The ptr must be populated with appropriate values

RETURN VALUE 
  void
 
SIDE EFFECTS
  None.

===========================================================================*/

void  ds_3gpp_pdn_throt_gen_event
(
   ds_3gpp_pdn_throttle_event_type *pdn_throt_ev_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_RESET_PDN_THROTTLE_IF_REQUIRED

DESCRIPTION
  This function clears cause code specific THROTTLING for a PDN context.
  The is_throttled flag is set to FALSE and the Timer value is cleared.

PARAMETERS: 
  sys_modem_as_id_e_type subs_id.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_reset_pdn_throttle_if_required
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_UPDATE_LOG_LIST

DESCRIPTION
  This function would update the logging list from apn and ip type

PARAMETERS
   ds_3gppi_throttle_sm_s*     throt_sm_ptr,
  ds_umts_pdp_type_enum_type  pdn_pdp_type   
 
DEPENDENCIES
  None

RETURN VALUE 
  void
 
SIDE EFFECTS/DEPENDENCIES 
PDP type must be IPV4 or IPV6 
 

===========================================================================*/

void ds_3gpp_pdn_throt_update_log_list
(
   ds_3gppi_throttle_sm_s     *throt_sm_ptr,
   ds_umts_pdp_type_enum_type  pdn_pdp_type 
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_SET_APN_REJECT_PDN_BLOCKED

DESCRIPTION
  This function sets PDN _BLOCKED in the apn_reject sm

PARAMETERS
   void              
 
DEPENDENCIES
  None.

RETURN VALUE
   True - If SET is successful
   False - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_set_apn_reject_pdn_blocked
(      
   boolean set_val,
   sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_APN_REJ_SM_INIT

DESCRIPTION
  This function initialize the apn reject sm

PARAMETERS   :

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_apn_rej_sm_init
(
   void
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_CLR_TIMER

DESCRIPTION
  This function clears the t3402 timer 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_apn_reject_clr_timer
(
   sys_modem_as_id_e_type  subs_id
);
/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_T3402_TIMER_START

DESCRIPTION
  This function starts the t3402 timer receiving the start timer cb

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_apn_reject_t3402_timer_start
(
   sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_T3402_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function resets the sm on timer expiry

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_apn_reject_t3402_timer_expiry_cmd_hdlr
(
   sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GP_PDN_THROT_VALIDATE_APN_REJECT_NAME

DESCRIPTION
  This function checks the EFS item to see if enable APN reject has been set.
  It it has not been set the apn_name will be NULL

PARAMETERS
                     
 
DEPENDENCIES
  None.

RETURN VALUE
   True - If the apn has been set in EFS
   False - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_validate_apn_reject_name
(
  sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_APN_REJ_SM_INIT

DESCRIPTION
  This function initialize the apn reject sm

PARAMETERS   :

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_apn_rej_sm_init
(
   void
);
/*===========================================================================
  FUNCTION ds_3gpp_pdn_throt_apn_rej_sm_init_per_subs_id

DESCRIPTION
  This function initializes the apn reject throttling information per
  subscription.

PARAMETERS
  subs_id -subscription_id 
 
DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_apn_rej_sm_init_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION ds_3gpp_pdn_throt_apn_rej_sm_refresh_per_subs_id

DESCRIPTION
  This function resets the apn reject throttling information per
  subscription.

PARAMETERS
  subs_id -subscription_id 
 
DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_apn_rej_sm_refresh_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_CLR_TIMER

DESCRIPTION
  This function clears the t3402 timer 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_apn_reject_clr_timer
(
   sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_GET_APN_REJECT_PDN_BLOCKED_STATUS

DESCRIPTION
  This function gets the PDN blocked flag from Reject SM

PARAMETERS
  void           
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If the flag is set
  FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_get_apn_reject_pdn_blocked_status
(      
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_GET_T3402_TIMER_VAL

DESCRIPTION
  This function gets the remaining Timer value of the throttle SM

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  timer_val: Remaining Timer value 

SIDE EFFECTS
  None.

===========================================================================*/

uint32 ds_3gpp_pdn_throt_apn_reject_get_t3402_timer_val
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_GET_APN_NAME

DESCRIPTION
  This function stores the Reject APN in the pointer passed

PARAMETERS
  apn_name: Memory block in which APN name has to be copied            
 
DEPENDENCIES
  None.

RETURN VALUE 
  None 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_apn_reject_get_apn_name
(
   char                            *apn_name,
   sys_modem_as_id_e_type           subs_id
);


/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_LTE_DISABLE_THROTTLE_TIMER_VALUE

DESCRIPTION
  This function calculates the amount time PLMN needs to be blocked.
  
PARAMETERS
  Subscription Id            
 
DEPENDENCIES
  None.

RETURN VALUE 
  Timer value in seconds

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_3gpp_pdn_throt_lte_disable_throttle_timer_value
(
   sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_CHECK_IF_BLOCK_PLMN_LIST_NEEDS_UPDATE

DESCRIPTION
  This function checks if the blocked PLMN List can be updated.
 
PARAMETERS
  Subscription Id          
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE, if all Attach APNs are  
        1. Globally blocked or
        3. Blocked due to APN being disabled 
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_check_if_block_plmn_list_needs_update
(
  sys_modem_as_id_e_type           subs_id
);
/*===========================================================================

FUNCTION  DS_3GPP_PDN_THROT_SET_APN_REJECT_PRF_NUM      

DESCRIPTION
  This function sets the profile number in the apn_reject sm

PARAMETERS
  uint8 profile_number
 
DEPENDENCIES
  None.

RETURN VALUE
   True - If NV is set
   False - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_set_apn_reject_prf_num
(      
   uint8 profile_number,
   sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_LOG_THROT_INFO

DESCRIPTION
  This function would check if the pdn throttling information would needed
  to be logged based on the is_throttled flag

PARAMETERS
   ds_3gppi_throttle_sm_s*     throt_sm_ptr,
   ds_umts_pdp_type_enum_type  pdn_pdp_type
 
DEPENDENCIES
  None

RETURN VALUE 
  void
 
SIDE EFFECTS
  PDN PDP type can only be IPV4 or IPV6, It cannot be V4V6 here.

===========================================================================*/

void ds_3gpp_pdn_throt_log_throt_info
(
   ds_3gppi_throttle_sm_s     *throt_sm_ptr,
   byte                       *apn,
   ds_umts_pdp_type_enum_type  pdn_pdp_type
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_CLR_LOG_ENTRY

DESCRIPTION
  This function would delete profile number from pdn_throt_profile_num_list

PARAMETERS
   ds_3gppi_throttle_sm_s*     throt_sm_ptr,
 
DEPENDENCIES
  None

RETURN VALUE 
  void 
 
SIDE EFFECTS

===========================================================================*/

void ds_3gpp_pdn_throt_clr_log_entry
(
   ds_3gppi_throttle_sm_s     *throt_sm_ptr
);


/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_GET_PROFILE_NUMBER

DESCRIPTION
  This function would get profile number to pdn_throt_profile_num_list

PARAMETERS
   ds_3gppi_throttle_sm_s*     throt_sm_ptr,
 
DEPENDENCIES
  None

RETURN VALUE 
  uint16                       profile_number
 
SIDE EFFECTS

===========================================================================*/

uint16 ds_3gpp_pdn_throt_get_profile_number
(
   ds_3gppi_throttle_sm_s     *throt_sm_ptr
);


/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_CURRENT_SYS_MODE

DESCRIPTION
  This Function sets the throttle_sys_mask to the old mode and sets 
  the throttled flag to false

PARAMETERS: 
  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr,
  ds_umts_pdp_type_enum_type              pdn_pdp_type,
  sys_sys_mode_e_type                     old_mode
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gppi_throt_sm_set_current_sys_mode
(
  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr,
  ds_umts_pdp_type_enum_type              pdn_pdp_type,
  sys_sys_mode_e_type                     sys_mode
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_LTE_HANDLE_NAS_REJECT_WITH_T3396_IE 
 
DESCRIPTION
  This function handles the reject message received from NAS with a t3396 IE
  based on Release 11 requirements

PARAMETERS   : 
  cm_lte_call_info_s_type               *lte_call_info,
  ds_pdn_context_s                      *pdn_context_p,
  ps_iface_net_down_reason_type         down_reason

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_lte_handle_nas_reject_with_t3396_ie
(
  cm_lte_call_info_s_type      *lte_call_info,
  ds_pdn_context_s             *pdn_context_p,
  ps_iface_net_down_reason_type down_reason
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_CLEAR_GLOBAL_THROTTLE_FLAG_AND_TIMER

DESCRIPTION
  Stop T3396 timer for v4 and v6, if it is already running and reset the 
  global throttling flag to false.

PARAMETERS
  ds_3gpp_pdn_throttle_sm_s    *pdn_throt_sm_ptr
  sys_plmn_id_s_type            current_plmn
  sys_modem_as_id_e_type        subs_id
  byte                          *apn
 
DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_clear_is_throttle_flag_and_timer
(
  ds_3gpp_pdn_throttle_sm_s    *pdn_throt_sm_ptr,
  sys_plmn_id_s_type            current_plmn,
  sys_modem_as_id_e_type        subs_id,
  byte                         *apn
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_FILL_IDENT_INFO

DESCRIPTION
  This function fills the information used to identify the correct 
  PDN Throttle SM that needs to be allocated / fetched

PARAMETERS
  *throt_ident_info_p : Structure to be filled.
  *apn                : APN Name
  sm_type             : PDN Throttle SM Type
  subs_id             : Subscription Id
  globally_blocked    : Globally Blocked or Not
  rat_type            : RAT to be throttled
  
 
DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_throt_fill_ident_info
(
  ds_3gpp_pdn_throt_ident_info_type *throt_ident_info_p,
  byte                              *apn,
  ds_3gpp_pdn_throttle_sm_type       sm_type,
  sys_modem_as_id_e_type             subs_id,
  boolean                            globally_blocked,
  ds_3gpp_throt_rat_e_type           rat_type
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_GET_NEXT_RAT_TYPE

DESCRIPTION
  This function decodes the RAT Mask and returns a RAT Type. The RAT Mask
  might have multiple RAT Type bits set. 
  The function will return the first RAT Type it decodes and will clear the bit
  correponding to that RAT Type. 
  To fetch all the RAT Types that are set in the mask, the caller should 
  invoke this function in a loop, until RAT Mask becomes
  DS_3GPP_THROT_RAT_MASK_NONE.

PARAMETERS
  *rat_mask_p : RAT Mask to be decoded

DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  The bit corresponding to the RAT Type being returned will be 
  cleared from the RAT Mask
===========================================================================*/
ds_3gpp_throt_rat_e_type ds_3gpp_pdn_throt_get_next_rat_type
(
  uint32  *rat_mask_p
);
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_APN_TIMER

DESCRIPTION
  This function unblocks apn timer 
 
PARAMETERS   : 
  throt_sm_p
  subs_id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_unblock_apn_timer
(
  void                       *throt_sm_p,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNTHROTTLE_AND_CLEAR_USER_DATA

DESCRIPTION
  This function clears throttling info for a pdn context and a given ip
  type and cleans up the user data 

PARAMETERS   :
   ds_3gpp_pdn_throttle_sm_s  *pdn_throt_sm_ptr
   throt_user_data_p           Throttle user data
   pdn_pdp_type                PDP Type
   sys_modem_as_id_e_type      subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_unthrottle_and_clear_user_data
(
  ds_3gpp_pdn_throttle_sm_s  *pdn_throt_sm_ptr,
  ds_3gpp_throt_user_data    *throt_user_data_p,
  ds_umts_pdp_type_enum_type  pdn_pdp_type,
  sys_modem_as_id_e_type      subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CHECK_AND_CLEAR_T3396_ENTRY

DESCRIPTION
  This function clears all T3396 entries

PARAMETERS
  subs_id

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_throt_clear_t3396_entry
(
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLEAR_THROTTLING_ON_LPM

DESCRIPTION
  This function clears THROTTLING for a particular APN/PDN.
  The is_throttled flag is set to FALSE and the Timer value is cleared.

PARAMETERS: 
  subs_id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  This should not be called to clear T3396 timer throttling

===========================================================================*/

void ds_3gpp_pdn_throt_clear_throttling_on_lpm
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLEAR_T3396_ON_USIM_REMOVAL

DESCRIPTION
  This function clears imsi_info_avail flag so that when card is inserted 
  again, IMSI can be compared and t3396 throttling can be cleared if needed.
  if clear throttling flag is set, T3396 timers for corresponding APNs are
  cleared. Permanent throttling for T3396 is also cleared.

PARAMETERS
  subs_id

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_throt_clear_t3396_on_usim_removal
(
  sys_modem_as_id_e_type subs_id,
  boolean                clear_throttling
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_VALIDATE_AND_UNBLOCK_PLMN

DESCRIPTION
  This function checks if the passed plmn id is global or not and unblocks
  the plmn accordingly. For global plmn id all blocked plmn's are unblocked

PARAMETERS   : 
  PLMN ID 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_validate_and_unblock_plmn
(
   sys_plmn_id_s_type   plmn_id,
   sys_modem_as_id_e_type subs_id  
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_DETERMINE_MODE

DESCRIPTION
  This function determines the mode for which throttling information
  needs to be published.

PARAMETERS
   subs_id: Subscription Id            
 
DEPENDENCIES
  None.

RETURN VALUE 
  Network Mode 

SIDE EFFECTS
  None.

===========================================================================*/
sys_sys_mode_e_type ds_3gpp_pdn_throt_determine_mode
(
  sys_modem_as_id_e_type subs_id
);

#endif /* DS_3GPP_PDN_THROTTLE_SM_H */
