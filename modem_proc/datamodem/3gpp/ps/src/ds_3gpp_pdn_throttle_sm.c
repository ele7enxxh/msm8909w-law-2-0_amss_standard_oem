/*!
  @file
  ds_3gpp_pdn_throttle_sm.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_pdn_throttle_sm.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
09/05/11     ss      Changes to support RPM.
10/01/10     az      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "ds_3gppi_utils.h"
#include "modem_mem.h"
#include "ds_system_heap.h"
#include "dsutil.h"
#include "ran.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include <stringl/stringl.h>
#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */
#include "ds3gmgr.h"
#include "ds3gcmif.h"
#include "cm.h"
#include "ds_dsd_apm_ext_i.h"
#include "mcfg_fs.h"
#include "ds3gmmgsdiif.h"
#include "ds3gmshif.h"
#include "ds_3gpp_bearer_context.h"
#include "dsumtspdpregint.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_3gpp_throttle_sm.h"
#ifdef FEATURE_DATA_LTE
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_eps_pdn_context.h"
#include "ds_eps_pdn_throttle_sm.h"
#include "ds_3gpp_apn_table.h"
#endif /*FEATURE_DATA_LTE*/

/*==========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
                   GENERIC PDN THROTTLING INFO START
---------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
  This is the structure that will be used to perform Throttling per PLMN
  This structure holds the PLMN on which the throttling SMs for different APNs
  are valid
  Note that throttling can be effective for an APN on two different PLMNs
------------------------------------------------------------------------------*/
typedef struct
{
  byte                                 apn[DS_UMTS_MAX_APN_STRING_LEN+1]; 
  ds_3gpp_pdn_throttle_sm_info_s      *ds_3gpp_pdn_throttle_tbl
                                          [DS_3GPP_MAX_PLMN_SUPPORTED];
  boolean                              globally_blocked; 
  boolean                              valid;
  boolean                              blocked_due_to_roaming;
  boolean                              apn_disabled;
  boolean                              ui_data_disabled;
} ds_3gpp_pdn_throttle_sm_apn_based_dyn_s;

/*------------------------------------------------------------------------------
  This is the table that maintains PDN Throttling Info for all APNs
------------------------------------------------------------------------------*/
typedef struct
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s 
                   *apn_based_dyn_tbl[DS_MAX_APN_NUM];
} ds_3gpp_pdn_throttle_sm_apn_based_s;

/*------------------------------------------------------------------------------
  This is the table that maintains PDN Throttling Info for all Subscriptions
------------------------------------------------------------------------------*/
static ds_3gpp_pdn_throttle_sm_apn_based_s*
         ds_3gpp_pdn_throttle_sm_apn_based_tbl_ptr[DS3GSUBSMGR_SUBS_ID_MAX] = {NULL};

static const sys_plmn_id_s_type global_plmn_id = {{0xAA, 0xAA, 0xAA}};

#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

ds_3gpp_pdn_throt_log_list_type pdn_throt_log_list
                                      [DS_PDN_THROT_LOG_LIST_TYPE_MAX] = {{NULL}};

/*---------------------------------------------------------------------------
                   GENERIC PDN THROTTLING INFO END
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------- 
  APN reject state machine -- This maintains a state to determine
  what apn must be throttled
  ---------------------------------------------------------------------------*/
ds_3gpp_pdn_throt_apn_reject_sm_s 
  ds_3gpp_pdn_throt_apn_reject_sm[SYS_MODEM_AS_ID_MAX] = {{NULL}};

/*---------------------------------------------------------------------------
                   RELEASE 10 INFO START
---------------------------------------------------------------------------*/

#define DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME        "apn_throttle"

#define DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME_LEN    12

/*---------------------------------------------------------------------------
  TWO additional spaces are req
  one for '/' and the other for the string terminator
---------------------------------------------------------------------------*/
static char ds_3gpp_apn_throt_dir_name[\
               DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME_LEN + \
               1 + 1] = {NULL};


#define DS_3GPP_PDN_THROT_LONGER_T3396_TIMER    36000000 /* 10 hours timer */
#define DS_3GPP_PDN_THROT_LONGER_SAMPLING_TIMER  3600000 /*1 hour timer */
#define DS_3GPP_PDN_THROT_EFS_FLUSH_TIME           90000 /*90 seconds*/


#define DS_3GPP_APN_THROTTLE_PARAM_FILE_NAME     "apn_throttle_params"
#define DS_3GPP_APN_THROTTLE_PARAM_FILE_NAME_LEN  19

typedef struct
{
  void   *data_ptr;
  uint32  data_len;
} ds_3gpp_pdn_throt_efs_data_buf_type;

typedef struct
{
  boolean valid;
  uint32  cookie;
  byte apn_name[DS_UMTS_MAX_APN_STRING_LEN+1]; /*APN name*/
  uint32 timer_val;                            /*t3396 timer value*/
  sys_plmn_id_s_type  plmn_id;                /*Plmn id which was set*/
} throt_info_t3396_timer_type_dyn_s;


typedef struct
{
 throt_info_t3396_timer_type_dyn_s  *throt_info_t3396_timer_type_dyn_p;
} throt_info_t3396_timer_type;

#define APN_THROT_T3396_VALID_COOKIE 0xaeabeabe
#define DS_MAX_APN_PLMN_SUPPORTED DS_MAX_APN_NUM*DS_3GPP_MAX_PLMN_SUPPORTED


/*------------------------------------------------------------------------------
  This is the structure that holds all the release 10 specific information
------------------------------------------------------------------------------*/
typedef struct
{
  /* Structure to store APN throttle t3396 timer config values              */ 
  throt_info_t3396_timer_type   
    t3396_timer_tbl[DS_MAX_APN_PLMN_SUPPORTED];                                                                            
             
  rex_timer_type    sampling_timer;              /* Sampling timer           */
  uint32            sampling_timer_val;          /* Sampling timer value     */
  nv_ehrpd_imsi_type imsi_data;                  /* IMSI data                */

  boolean imsi_data_avail;                       /* IMSI data available flag */

  /* EFS file name for the throttle file "apn_throttle_params"               */
  /* Allocate an 
     additional character for the string terminator                          */
  char
    ds_3gpp_apn_throt_file_path[DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME_LEN + \
                          1 + \
                          DS_3GPP_APN_THROTTLE_PARAM_FILE_NAME_LEN + 
                          1];

} ds_3gpp_pdn_throt_rel10_info_type;

static ds_3gpp_pdn_throt_rel10_info_type*
                  ds_3gpp_pdn_throt_rel10_info_ptr[DS3GSUBSMGR_SUBS_ID_MAX];

/*---------------------------------------------------------------------------
                   RELEASE 10 INFO END
---------------------------------------------------------------------------*/

/*===========================================================================

                         FORWARD DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SAMPLING_TIMER_CB
 
DESCRIPTION
  This callback function is invoked when the sampling timer expires. 
  
PARAMETERS 
  callback_data

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_pdn_throt_sampling_timer_cb
(
  unsigned long callback_data
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_START_SAMPLING_TIMER
 
DESCRIPTION
  This function starts the sampling timer which is a fraction of the shortest
  running T3396 timers. This timer helps keep track of elapsed timer in the
  EFS.
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_pdn_throt_start_sampling_timer
(
  boolean                calc_sampling_timer,
  sys_modem_as_id_e_type subs_id
);

#ifdef FEATURE_DATA_REL10
/*===========================================================================
FUNCTION DS_3GPP_APN_THROT_GET_T3396_TIMER_CB
 
DESCRIPTION
  This function fetches the T3396 timer value.
  
PARAMETERS
  1. Core Throttle SM Ptr
  2. Callback Data (Subs Id)

DEPENDENCIES
  None.

RETURN VALUE
  T3396 timer value
 
SIDE EFFECTS
  None.

===========================================================================*/
static uint32 ds_3gpp_apn_throt_get_t3396_timer_cb
(
  void* throt_sm_void_ptr,
  void* cb_data
);
#endif /*FEATURE_DATA_REL10*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ALLOCATE_REL10_INFO
 
DESCRIPTION
  This function allocates Rel10 Info Tbl for a particular subscription.
  
PARAMETERS 
  1. Subs Id
  2. Ret_val: FALSE if Allocation failed or entry already allocated.
              TRUE, otherwise

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the newly allocated Rel10 Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throt_rel10_info_type*
         ds_3gpp_pdn_throt_allocate_rel10_info
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_REL10_INFO
 
DESCRIPTION
  This function fetches Rel10 Info Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
 
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the Rel10 Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throt_rel10_info_type* 
         ds_3gpp_pdn_throt_get_rel10_info
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_REL10_INFO
 
DESCRIPTION
  This function sets Rel10 Info Tbl for a particular subscription.
  
PARAMETERS
  1. Subs Id
  2. Rel10 Info ptr
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if set was done successfully
  FALSE, otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_pdn_throt_set_rel10_info
(
  sys_modem_as_id_e_type             subs_id,
  ds_3gpp_pdn_throt_rel10_info_type* rel10_info_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ALLOCATE_APN_BASED_TBL
 
DESCRIPTION
  This function allocates APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  2. Ret_val: FALSE if Allocation failed or entry already allocated.
              TRUE, otherwise

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the newly allocated APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throttle_sm_apn_based_s*
         ds_3gpp_pdn_throt_allocate_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_APN_BASED_TBL
 
DESCRIPTION
  This function fetches the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throttle_sm_apn_based_s* 
         ds_3gpp_pdn_throt_get_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_APN_BASED_TBL
 
DESCRIPTION
  This function sets the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  2. APN Based Tbl Ptr
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if set successfully
  FALSE, otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_pdn_throt_set_apn_based_tbl
(
  sys_modem_as_id_e_type               subs_id,
  ds_3gpp_pdn_throttle_sm_apn_based_s* apn_based_tbl_ptr
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_T3402_TIMER_EXPIRY

DESCRIPTION
  This function posts a cmd to reset the apn reject sm on timer expiry

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

static void ds_3gpp_pdn_throt_apn_reject_t3402_timer_expiry
(
     unsigned long callback_data /*unused*/
);
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_IS_THROTTLE_APN_ALLOWED_FOR_ATTACH

DESCRIPTION
  This function checks if throttled APN is allowed for attach
  we allow throttle APN for attach is UE is not attached and apn is not
   globally blocked

PARAMETERS   :
   subs id        - Subscription Id
   sm_type    - Internal or External Client
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_pdn_throt_sm_is_throttled_apn_allowed_for_attach
(
  sys_modem_as_id_e_type           subs_id,
  ds_3gpp_pdn_throttle_sm_type     sm_type
);
/*===========================================================================

                                FUNCTIONS

===========================================================================*/

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
)
{
  int                                       plmn_loop_index;
  int                                       apn_loop_index;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *per_apn_throt_sm_ptr;
  ds_3gpp_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  apn_based_tbl_ptr = 
    ds_3gpp_pdn_throt_allocate_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_MED("apn_based_tbl_ptr is NULL");
    return;
  }

  for(apn_loop_index = 0; 
      apn_loop_index < DS_MAX_APN_NUM;
      apn_loop_index++) 
  {
    per_apn_throt_sm_ptr = 
     (ds_3gpp_pdn_throttle_sm_apn_based_dyn_s*) 
        modem_mem_alloc(sizeof(ds_3gpp_pdn_throttle_sm_apn_based_dyn_s),
                         MODEM_MEM_CLIENT_DATA);

    if(per_apn_throt_sm_ptr != NULL)
    {
      apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index] = 
        per_apn_throt_sm_ptr;

      /*------------------------------------------------------------------------
        Initialize APN name to NULL
      ------------------------------------------------------------------------*/
      memset( (void*)&(per_apn_throt_sm_ptr->apn), 
               0, 
              (DS_UMTS_MAX_APN_STRING_LEN+1) );
      per_apn_throt_sm_ptr->valid = FALSE;
      per_apn_throt_sm_ptr->globally_blocked = FALSE;
      per_apn_throt_sm_ptr->blocked_due_to_roaming = FALSE;
      per_apn_throt_sm_ptr->apn_disabled = FALSE;
      per_apn_throt_sm_ptr->ui_data_disabled = FALSE;

      /*------------------------------------------------------------------------
        Loop over all the PDN throt SMs (per APN) and set the TCM 
      ------------------------------------------------------------------------*/
      for(plmn_loop_index = 0; 
          plmn_loop_index < DS_3GPP_MAX_PLMN_SUPPORTED;
          plmn_loop_index++) 
      {
        per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl[plmn_loop_index] = NULL;
      } //loop over all PDN throt sm pointers for a given PLMN
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Could not allocate Per APN throt SM. No PDN throt");
    }
  } //loop over all APNs

} /* ds_3gpp_pdn_throt_sm_per_apn_power_up_init */

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_INIT

DESCRIPTION
  This function initialize a pdn throttling sm

PARAMETERS   :
   *pdn_throt_sm_ptr      - TCM ptr
   *pdn_throt_sm_info_ptr - Parent of TCM Ptr
   *throt_ident_info_p    - Information used to identify the type of throttling

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_sm_init
(
  ds_3gpp_pdn_throttle_sm_s                *pdn_throt_sm_ptr,
  void                                     *pdn_throt_sm_info_ptr,
  ds_3gpp_pdn_throt_ident_info_type        *throt_ident_info_p
) 
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pdn_throt_sm_ptr == NULL)
  {
    DATA_ERR_FATAL("pdn_throt_sm_ptr is NULL");
    return;
  }

  if(pdn_throt_sm_info_ptr == NULL)
  {
    DATA_ERR_FATAL("pdn_throt_sm_info_ptr is NULL");
    return;
  }

  if(throt_ident_info_p == NULL)
  {
    DATA_ERR_FATAL("throt_ident_info_p is NULL");
    return;
  }

  pdn_throt_sm_ptr->init = TRUE;

  pdn_throt_sm_ptr->pdn_throt_sm_info_p = pdn_throt_sm_info_ptr;

  pdn_throt_sm_ptr->sm_type = throt_ident_info_p->sm_type;

  if (throt_ident_info_p->sm_type == DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT || 
      throt_ident_info_p->sm_type == DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME)
  {
    pdn_throt_sm_ptr->v4_throttle_sm = 
      ds_3gppi_throt_sm_get_core_throttle_sm(throt_ident_info_p->plmn_id, 
                                             throt_ident_info_p->subs_id);

    if(pdn_throt_sm_ptr->v4_throttle_sm == NULL)
    {
      DS_3GPP_MSG0_ERROR("Could not allocate mem for core throttling SM");
    }
#ifdef FEATURE_DATA_RPM
    else
    {
      /* Copy the APN into the throttling SM */
      strlcpy( (char*)pdn_throt_sm_ptr->v4_throttle_sm->apn, 
               (char*)throt_ident_info_p->apn,
               (DS_UMTS_MAX_APN_STRING_LEN+1) );
    }
#endif /* FEATURE_DATA_RPM */
  }
  else
  {
    pdn_throt_sm_ptr->v4_throttle_sm = 
      ds_3gppi_throt_sm_get_core_throttle_sm(throt_ident_info_p->plmn_id, 
                                             throt_ident_info_p->subs_id);
    pdn_throt_sm_ptr->v6_throttle_sm = 
      ds_3gppi_throt_sm_get_core_throttle_sm(throt_ident_info_p->plmn_id, 
                                             throt_ident_info_p->subs_id);

    if(pdn_throt_sm_ptr->v4_throttle_sm == NULL || 
       pdn_throt_sm_ptr->v6_throttle_sm == NULL)
    {
      DS_3GPP_MSG0_ERROR("Could not allocate mem for core throttling SM ");
    }
#ifdef FEATURE_DATA_RPM
    else
    {
      /* Copy the APN into the throttling SM */
      strlcpy( (char*)pdn_throt_sm_ptr->v4_throttle_sm->apn, 
               (char*)throt_ident_info_p->apn,
               (DS_UMTS_MAX_APN_STRING_LEN+1) );
 
      strlcpy( (char*)pdn_throt_sm_ptr->v6_throttle_sm->apn, 
               (char*)throt_ident_info_p->apn,
               (DS_UMTS_MAX_APN_STRING_LEN+1) );
    }
#endif /* FEATURE_DATA_RPM */

  }

  /*-------------------------------------------------------------------------- 
    Reset ds_3gpp_pdn_throt_profile_num_list_type
    -------------------------------------------------------------------------*/
  memset(pdn_throt_log_list,0,sizeof(pdn_throt_log_list));

}

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_GET_PROFILE_PARAM_INFO

DESCRIPTION   This function gets the profile param info from profile cache.

DEPENDENCIES  None

RETURN VALUE  TRUE: If read is successful 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gpp_pdn_throt_get_profile_param_info
(
  uint16                            profile_id,
  ds_profile_identifier_type        profile_param_id,
  ds_profile_info_type             *profile_param_info_ptr
)
{
  ds_profile_status_etype           result;
  ds_profile_hndl_type              profile_handle = NULL;
  boolean                           ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( profile_param_info_ptr == NULL )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"NULL profile_param_info ptr");
    return FALSE;
  }

  do
  {
    result = ds_profile_begin_transaction( DS_PROFILE_TRN_READ,
                                           DS_PROFILE_TECH_3GPP,
                                           (ds_profile_num_type)profile_id,
                                           &profile_handle );

    if( DS_PROFILE_REG_RESULT_SUCCESS != result ||
        profile_handle == NULL )
    {
      DATA_MSG2(MSG_LEGACY_ERROR,"Begin transaction failed %d %x", 
                result,profile_handle);
      break;
    }

    result = ds_profile_get_param( profile_handle,
                                   profile_param_id,
                                   profile_param_info_ptr);

    if( DS_PROFILE_REG_RESULT_SUCCESS != result )
    {
      DATA_MSG0(MSG_LEGACY_ERROR,"Read profile failed");
      break;
    }

    ret_val = TRUE;
  }while (0);

  if( NULL != profile_handle )
  {
    result = ds_profile_end_transaction (profile_handle,
                                         DS_PROFILE_ACTION_COMMIT);
  }

  return ret_val;
}/* ds_3gpp_pdn_throt_get_profile_param_info */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_DECIDE_THROTTLED_IP_TYPE

DESCRIPTION   This function decides what IP types have to be throttled depending 
              on Profiles PDP Type
 
              If only one of IP Type is throttled and Profile's PDP type is
              v4v6 then APM should not informed to block this Profile.

DEPENDENCIES  None

RETURN VALUE  TRUE: If read is successful 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gpp_pdn_throt_decide_throttled_ip_type
(
  byte                                *apn,
  ds_3gpp_pdn_throt_temp_status_type  *temp_throt_status,
  sys_modem_as_id_e_type               subs_id
)
{
  boolean                              v4_profile = FALSE;
  boolean                              v6_profile = FALSE;
  boolean                              v4v6_profile = FALSE;
  ds_profile_info_type                 prof_pdp_type;
  ds_profile_3gpp_pdp_type_enum_type   pdp_type = DS_PROFILE_3GPP_PDP_MAX;
  ds_profile_info_type                 prof_apn;
  byte                                 apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
  uint8                                profile_nums_arr[DS_UMTS_MAX_PDP_PROFILE_NUM];
  uint8                                profile_count = 0;
  uint8                                i = 0;
  boolean                              retVal = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(apn == NULL || temp_throt_status == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid Input Params, Exiting !!!!");
    return retVal;
  }


  if(
     (temp_throt_status->v4_throttle_status == TRUE && 
     temp_throt_status->v6_throttle_status == FALSE) ||
     (temp_throt_status->v4_throttle_status == FALSE && 
     temp_throt_status->v6_throttle_status == TRUE)
     )
  {
    profile_count = ds_3gpp_profile_cache_get_valid_profile_numbers(
                                              dsumts_subs_mgr_get_subs_id(subs_id),
                                                   profile_nums_arr, 
                                                   DS_UMTS_MAX_PDP_PROFILE_NUM);

    for (i=0; i < profile_count; i++)
    {

      memset(&prof_apn, 0, sizeof(ds_profile_info_type));
      memset(apn_name, 0, DS_UMTS_MAX_APN_STRING_LEN+1);

      prof_apn.buf = apn_name;
      prof_apn.len = DS_UMTS_MAX_APN_STRING_LEN+1;

      if(ds_3gpp_pdn_throt_get_profile_param_info(
                             profile_nums_arr[i],
                             DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                             &prof_apn) == FALSE)
      {
        DS_3GPP_MSG1_ERROR("Unable to Extract the APN Name of Profile: %d",
                           profile_nums_arr[i]);
      }
      else
      {
        if(strncasecmp((const char *)apn, (const char *)apn_name,
                        DS_UMTS_MAX_APN_STRING_LEN+1) == 0)
        {
          DS_3GPP_MSG1_MED("APN Match found at Index: %d", i);
          memset(&prof_pdp_type, 0, sizeof(ds_profile_info_type));
          prof_pdp_type.buf = &pdp_type;
          prof_pdp_type.len = sizeof(ds_profile_3gpp_pdp_type_enum_type);

          if(ds_3gpp_pdn_throt_get_profile_param_info(
                                 profile_nums_arr[i],
                                 DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
                                 &prof_pdp_type) == FALSE)
          {
            DS_3GPP_MSG1_ERROR("Unable to Extract the PDP type of Profile: %d",
                               profile_nums_arr[i]);
            pdp_type = DS_PROFILE_3GPP_PDP_MAX; 
          }
          else
          {
            DS_3GPP_MSG1_MED("PDP Type extraced is: %d", pdp_type);

            if(pdp_type == DS_PROFILE_3GPP_PDP_IP)
            { 
              v4_profile = TRUE;
            }
            else if(pdp_type == DS_PROFILE_3GPP_PDP_IPV6)
            {
              v6_profile = TRUE;
            }
            else if(pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6)
            {
              v4v6_profile = TRUE;
            }
            else
            {
              DS_3GPP_MSG1_HIGH("Un-expected PDP Type: %d", pdp_type);
            }
          }
        }
      }
    }

    if(((temp_throt_status->v4_throttle_status == TRUE && v6_profile == FALSE) ||
       (temp_throt_status->v6_throttle_status == TRUE && v4_profile == FALSE)) && 
       (v4v6_profile == FALSE))
    {
      retVal = TRUE;
    }
    else if(v4v6_profile == TRUE)
    {
      DS_3GPP_MSG0_HIGH("Only one IP Type was throttled but PDP Type is v4v6, "
                        "no external throttling info broadcasted");
      retVal = FALSE;
    }
  }
  else if(temp_throt_status->v4_throttle_status == TRUE &&
          temp_throt_status->v6_throttle_status == TRUE)
  {
    DS_3GPP_MSG0_MED("Both the IP types are throttled, inform other clients");
    retVal = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_MED("None of the IP types are throttled, not informing "
                     "other clients");
    retVal = FALSE;
  }
  
  return retVal;
} /* ds_3gpp_pdn_throt_decide_throttled_ip_type */
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_IS_THROTTLE_APN_ALLOWED_FOR_ATTACH
DESCRIPTION
  This function checks if throttled APN is allowed for attach
  we allow throttle APN for attach is UE is not attached and apn is not
   globally blocked

PARAMETERS   :
   subs id        - Subscription Id
   sm_type    - Internal or External Client
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_pdn_throt_sm_is_throttled_apn_allowed_for_attach
(
  sys_modem_as_id_e_type           subs_id,
  ds_3gpp_pdn_throttle_sm_type     sm_type
)
{
  if((!ds_3gpp_nv_manager_get_reg_pdn_throttling(subs_id)) &&
     (sm_type != DS_3GPP_THROTTLE_TYPE_T3396) &&
     (sm_type != DS_3GPP_THROTTLE_TYPE_T3396_ABSENT)
#ifdef FEATURE_DATA_LTE
     && (!ds_eps_attach_sm_is_ue_attached(subs_id))
#endif /* FEATURE_DATA_LTE */
     )
  {
    return TRUE;
  }
  return FALSE;
}
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
)
{
  uint8                                    apn_index = 0;
  uint8                                    plmn_index = 0;
  boolean                                  ipv4_throttled = FALSE;            
  boolean                                  ipv6_throttled = FALSE;  
  boolean                                  globally_throttled = FALSE;          
  uint32                                   remaining_ipv4_throttled_time = 0;
  uint32                                   remaining_ipv6_throttled_time = 0;
  uint32                                   remaining_global_throttled_time = 0;
  ds_3gpp_pdn_throttle_sm_s                *pdn_throttle_sm_ptr;
  ds_3gpp_pdn_throttle_sm_info_s           *pdn_throttle_sm_info_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_based_throttle_sm_ptr = NULL;
  ps_sys_ioctl_pdn_ctxt_throttle_info_type *apn_throttle_info;
  ds_3gpp_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr = NULL;
  ds_3gpp_pdn_throt_temp_status_type       temp_throt_status;
  boolean                                  inform_other_clients = FALSE;
#ifdef FEATURE_DATA_LTE
  uint8                                    index = 0;
  uint32                                   current_pdn_conn_tmr_val = 0;
  uint32                                   time_remaining_in_current_blk = 0;
#endif /*FEATURE_DATA_LTE*/
/*------------------------------------------------------------------------*/

  DS_3GPP_MSG2_HIGH("Fetching PDN Throt Info for subs_id: %d, "
                    "RAT index: %d", subs_id, rat_index);

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }
        
  apn_throttle_info = throttle_info->pdn_throttle_info;  
  
  ds3gpp_enter_global_crit_section();

  do
  {
#ifdef FEATURE_DATA_LTE
    if (ds_3gpp_pdn_throt_get_apn_reject_pdn_blocked_status(subs_id) == TRUE)
    {
      apn_throttle_info->is_ipv4_throttled = TRUE;
      apn_throttle_info->is_ipv6_throttled = TRUE;

      apn_throttle_info->remaining_ipv4_throttled_time = 
        ds_3gpp_pdn_throt_apn_reject_get_t3402_timer_val(subs_id);
      apn_throttle_info->remaining_ipv6_throttled_time = 
        apn_throttle_info->remaining_ipv4_throttled_time;

      ds_3gpp_pdn_throt_apn_reject_get_apn_name (apn_throttle_info->apn_string,subs_id);
      apn_throttle_info->apn_string_len = strlen (apn_throttle_info->apn_string);

      throttle_info->num_apn++;
      break;
    }
#endif /*FEATURE_DATA_LTE*/
    for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
    {
      apn_based_throttle_sm_ptr = 
                         apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

      if( (apn_based_throttle_sm_ptr != NULL) &&
          (apn_based_throttle_sm_ptr->valid == TRUE) )
      {
        if (apn_based_throttle_sm_ptr->blocked_due_to_roaming ||
            apn_based_throttle_sm_ptr->apn_disabled ||
            apn_based_throttle_sm_ptr->ui_data_disabled)
        {
          globally_throttled = TRUE;
          remaining_global_throttled_time = DS_3GPP_PDN_THROT_INFINITE_THROT;
        }

        else
        {
          for (plmn_index = 0; plmn_index<DS_3GPP_MAX_PLMN_SUPPORTED; plmn_index++)
          {
            pdn_throttle_sm_info_ptr 
                   = apn_based_throttle_sm_ptr->ds_3gpp_pdn_throttle_tbl[plmn_index];

            if (pdn_throttle_sm_info_ptr == NULL)
            {
              continue;
            }

            /*----------------------------------------------------------------------
              Here we try to find out whether the APN is globally throttled.
              If it is globally throttled, we store the remaining global throttling
              time.
             
              We also try to find out whether the APN is throttled on the current PLMN.
              If it is, we store the remaining throttling time.
             
              Next we try to find out if APN is throttled because Max PDN Connections
              per Block has been reached. If it is, we store the time remaining for
              the next block to kick in.
             
              We will return the maximum throttling time to the application.
             
            ------------------------------------------------------------------------*/
            if ( memcmp(&(pdn_throttle_sm_info_ptr->plmn_id), 
                          &global_plmn_id, sizeof(sys_plmn_id_s_type)) == 0)
            {
              pdn_throttle_sm_ptr = 
                &(pdn_throttle_sm_info_ptr->per_rat[DS_3GPP_THROT_RAT_GLOBAL]);

              if((pdn_throttle_sm_ptr != NULL) && 
                 (pdn_throttle_sm_ptr->v4_throttle_sm != NULL))
              {
                if(!ds_3gpp_pdn_throt_sm_is_throttled_apn_allowed_for_attach(
                                       subs_id,pdn_throttle_sm_ptr->sm_type))
                {
                  globally_throttled = ds_3gppi_throt_sm_get_is_throttled_flag(
                                          pdn_throttle_sm_ptr->v4_throttle_sm);
                  remaining_global_throttled_time = 
                    ds_3gppi_throt_sm_get_remaining_throttle_time( 
                      pdn_throttle_sm_ptr->v4_throttle_sm);     
                } /*globally_throttled*/
              } /* pdn_throttle_sm_ptr->v4_throttle_sm != NULL */
            }

            else if ( memcmp(&(pdn_throttle_sm_info_ptr->plmn_id), 
                          plmn_ptr, sizeof(sys_plmn_id_s_type)) == 0)
            {
              if (rat_index > DS_3GPP_THROT_RAT_MIN &&
                  rat_index < DS_3GPP_THROT_RAT_MAX)
              {
                pdn_throttle_sm_ptr = &(pdn_throttle_sm_info_ptr->per_rat[rat_index]);
              }
              else
              {
                pdn_throttle_sm_ptr = NULL;
              }

              if (pdn_throttle_sm_ptr != NULL)
              {
                if((pdn_throttle_sm_ptr->v4_throttle_sm != NULL)&&
                   (ds_3gppi_throt_sm_get_is_throttled_flag(
                                           pdn_throttle_sm_ptr->v4_throttle_sm)))
                    {
                  if(!ds_3gpp_pdn_throt_sm_is_throttled_apn_allowed_for_attach(
                                                 subs_id, pdn_throttle_sm_ptr->sm_type))
                    {
                     ipv4_throttled = TRUE;                 
                      remaining_ipv4_throttled_time = 
                        ds_3gppi_throt_sm_get_remaining_throttle_time( 
                          pdn_throttle_sm_ptr->v4_throttle_sm);
                  } /* ipv4_throttled */
                } /* pdn_throttle_sm_ptr->v4_throttle_sm != NULL */
              
                if((pdn_throttle_sm_ptr->v6_throttle_sm != NULL)&&
                   (ds_3gppi_throt_sm_get_is_throttled_flag(
                                               pdn_throttle_sm_ptr->v6_throttle_sm)))
                {
                  if(!ds_3gpp_pdn_throt_sm_is_throttled_apn_allowed_for_attach(
                                               subs_id, pdn_throttle_sm_ptr->sm_type)
                    )
                  {
                     ipv6_throttled = TRUE;                                    
                      remaining_ipv6_throttled_time = 
                        ds_3gppi_throt_sm_get_remaining_throttle_time( 
                          pdn_throttle_sm_ptr->v6_throttle_sm);
                  } /* ipv4_throttled */
                } /* pdn_throttle_sm_ptr->v6_throttle_sm != NULL */
              } /* if (pdn_throttle_sm_ptr != NULL) */
            }       
          } /* end for(plmn_index)*/

    #ifdef FEATURE_DATA_LTE
          if (ds_3gpp_pdn_limit_is_enabled(subs_id))
          {
            if ((index = ds_3gpp_pdn_limit_tbl_find_entry
                          (apn_based_throttle_sm_ptr->apn, subs_id)) != 
                            DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY  )
            {
              current_pdn_conn_tmr_val = 
                ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val(index, subs_id);

              if(!ds_eps_attach_sm_is_ue_attached(subs_id))
              {
                 current_pdn_conn_tmr_val = 0;
              }

              if (current_pdn_conn_tmr_val != 0)
              {
                if (ds_3gpp_pdn_limit_get_pdn_conn_cntr(index, subs_id) >=  
                      ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time
                       (index, subs_id, current_pdn_conn_tmr_val))
                {
                  time_remaining_in_current_blk = 
                    ds_3gpp_pdn_limit_tbl_get_time_remaining_in_current_blk
                      (index, subs_id, current_pdn_conn_tmr_val);

                  remaining_global_throttled_time = 
                    MAX(remaining_global_throttled_time, time_remaining_in_current_blk);
                  globally_throttled = TRUE;
                }
              }
            }
          }
    #endif /*FEATURE_DATA_LTE*/
        } /* apn_based_throttle_sm_ptr->blocked_due_to_roaming */

        memset(&temp_throt_status, 0, 
               sizeof(ds_3gpp_pdn_throt_temp_status_type));

        temp_throt_status.v4_throttle_status = ipv4_throttled;
        temp_throt_status.v6_throttle_status = ipv6_throttled;

        inform_other_clients = ds_3gpp_pdn_throt_decide_throttled_ip_type(
                                  apn_based_throttle_sm_ptr->apn,
                                  &temp_throt_status,
                                  subs_id);

        if(ipv4_throttled == TRUE && ipv6_throttled == TRUE)
        {
          DS_3GPP_MSG0_MED("Both IP Types are throttled, no additional "
                            "processing needed");
        }
        else
        {
          if(ipv4_throttled == TRUE &&
             inform_other_clients == TRUE)
          {
            DS_3GPP_MSG0_MED("Both IP types need throttling, "
                             "only v4 was throttled");
            ipv6_throttled = TRUE;
            remaining_ipv6_throttled_time = DS_3GPP_PDN_THROT_INFINITE_THROT;
          }
          else if(ipv6_throttled == TRUE &&
                  inform_other_clients == TRUE)
          {
            DS_3GPP_MSG0_MED("Both IP types need throttling, "
                             "only v6 was throttled");
            ipv4_throttled = TRUE;
            remaining_ipv4_throttled_time = DS_3GPP_PDN_THROT_INFINITE_THROT;
          }
        }

        if(inform_other_clients || globally_throttled)
        {
          apn_throttle_info->is_ipv4_throttled = 
            ipv4_throttled || globally_throttled;
          apn_throttle_info->is_ipv6_throttled = 
            ipv6_throttled || globally_throttled;
          apn_throttle_info->remaining_ipv4_throttled_time = 
            MAX(remaining_ipv4_throttled_time,remaining_global_throttled_time);
          apn_throttle_info->remaining_ipv6_throttled_time = 
            MAX(remaining_ipv6_throttled_time,remaining_global_throttled_time);
          strlcpy((char *)apn_throttle_info->apn_string, 
                  (char *)apn_based_throttle_sm_ptr->apn, 
                  (DS_UMTS_MAX_APN_STRING_LEN+1));
          apn_throttle_info->apn_string_len = strlen((char *)
                                                apn_throttle_info->apn_string);

          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                                  "Returning throt timer value for APN %s",
                                  apn_throttle_info->apn_string);

          DS_3GPP_MSG1_HIGH("Remaining Ipv4 throttling time: %d", 
                            apn_throttle_info->remaining_ipv4_throttled_time);

          DS_3GPP_MSG1_HIGH("Remaining Ipv6 throttling time: %d", 
                            apn_throttle_info->remaining_ipv6_throttled_time);
          apn_throttle_info ++;
          throttle_info->num_apn ++;      
          if(throttle_info->num_apn >= PS_SYS_MAX_APNS)
          {
            break;
          }
        }

        //Clear throttling info
        globally_throttled = FALSE;
        ipv4_throttled = FALSE;
        ipv6_throttled = FALSE;
        remaining_global_throttled_time = 0;
        remaining_ipv6_throttled_time = 0;
        remaining_ipv4_throttled_time = 0;
        inform_other_clients = FALSE;   

      } /*if apn index valid*/
    } /* end for(apn_index)*/
  }while (0);

  ds3gpp_leave_global_crit_section();

} /*ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn() */
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_APN_BASED_TBL_INDEX

DESCRIPTION
  This function retrieves the APN based table index for a specific APN.

PARAMETERS   :
   apn    - the apn for which the APN table index is to be retrieved.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ds_3gpp_pdn_throttle_sm_apn_based_dyn_s* 
ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index
(
  byte                          *apn,
  sys_modem_as_id_e_type         subs_id
)
{
  uint8                                     apn_index  = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("Passed APN param is NULL");
    return NULL;
  }

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return NULL;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

    if ( (NULL != apn_index_ptr) &&
         (TRUE == apn_index_ptr->valid) )
    {
      if ( strcasecmp((char*)apn_index_ptr->apn,(char *)apn) == 0 )
      {
        DS_3GPP_MSG1_HIGH("APN match found at index %d", apn_index);
        return apn_index_ptr;
      }
    }
  }
  return NULL;
}/*ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM_INFO_FROM_PLMN

DESCRIPTION
  This function retrieves the pdn throttle_sm_info pointer
  for a specific plmn_id and apn.

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
)
{
  uint8                                     plmn_index = 0;
  uint8                                     apn_index  = 0;
  sys_plmn_id_s_type                        zero_plmn;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr = NULL;
  boolean                                   found_match = FALSE;
  ds_3gpp_pdn_throttle_sm_info_s           *pdn_throt_sm_info_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG1_MED("Get PDN throttling SM for Subs Id: %d", subs_id);

  memset(&zero_plmn, 0, sizeof(sys_plmn_id_s_type));

  if ( memcmp(&plmn_id, &zero_plmn, sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG0_ERROR("plmn_id passed is zero");
    return NULL;
  }

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return NULL;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

    if ( (NULL != apn_index_ptr) &&
         (TRUE == apn_index_ptr->valid) )
    {
      if ( (strcasecmp((char*)apn_index_ptr->apn, (char *)apn) == 0) )
      {
        DS_3GPP_MSG1_HIGH("APN match found at index %d", apn_index);
        for (plmn_index = 0; plmn_index < DS_3GPP_MAX_PLMN_SUPPORTED; plmn_index++)
        {
          pdn_throt_sm_info_p 
                = apn_index_ptr->ds_3gpp_pdn_throttle_tbl[plmn_index];
          if (NULL != pdn_throt_sm_info_p)
          {
            if ( memcmp(&(pdn_throt_sm_info_p->plmn_id),
                        &plmn_id,
                        sizeof(sys_plmn_id_s_type)) == 0)
            {
              DS_3GPP_MSG1_HIGH("PLMN match found at index %d", plmn_index);
              found_match = TRUE;
              break;
            }/*if(plmn id match)*/
          }
        }/*for(plmn_index)*/
      }/*if(apn match)*/
    }
  }/*apn_index*/
 
  if (found_match)
  {
    return (void*)pdn_throt_sm_info_p;
  }
  else
  {
    return NULL;
  }
} 

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM_FROM_PLMN

DESCRIPTION
  This function retrieves the pdn throttle_sm for a specific plmn_id and apn
  for the global rat mask.

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
)
{
  sys_plmn_id_s_type                        zero_plmn;
  ds_3gpp_pdn_throttle_sm_s                *pdn_throttle_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s           *pdn_throt_sm_info_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG1_MED("Get PDN throttling SM for Subs Id: %d", subs_id);

  memset(&zero_plmn, 0, sizeof(sys_plmn_id_s_type));

  if ( memcmp(&plmn_id, &zero_plmn, sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG0_ERROR("plmn_id passed is zero");
    return NULL;
  }

  pdn_throt_sm_info_p =(ds_3gpp_pdn_throttle_sm_info_s*) 
                         ds_3gpp_pdn_throt_sm_get_throttle_sm_info_from_plmn
                                                    (plmn_id,
                                                     apn,
                                                     subs_id);

  if(pdn_throt_sm_info_p != NULL)
  {
    pdn_throttle_sm_ptr = 
      &(pdn_throt_sm_info_p->per_rat[DS_3GPP_THROT_RAT_GLOBAL]);

    return (void *)pdn_throttle_sm_ptr;
  }
  else
  {
    return NULL;
  }
} /* ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn */

void* ds_3gpp_pdn_throt_sm_get_apn_index_ptr
(
  byte                          *apn,
  boolean                        allow_create,
  sys_modem_as_id_e_type         subs_id
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  int                                      apn_index; 
  int                                      first_free_apn_index = 
                                             DS_MAX_APN_NUM;
  /*----------------------------------------------------------------*/

  do
  {
    apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

    if (apn_based_tbl_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
      break;
    }

    for(apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++) 
    {
      apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

      if (apn_index_ptr != NULL)
      {
          if( (apn_index_ptr->valid == TRUE)&& 
            (strcasecmp((char*)apn_index_ptr->apn, (char*)apn) == 0) )
          {
            /*------------------------------------------------------------------------
              APN match. Break from the loop. We have found the entry in the table
             for the current APN
            ------------------------------------------------------------------------*/
            DS_3GPP_MSG1_HIGH("APN matched at index: %d. Break", apn_index);
            break;
          }
          else if(apn_index_ptr->valid == FALSE)
          {
            if(first_free_apn_index == DS_MAX_APN_NUM)
            {
              /*----------------------------------------------------------------------
                This is the first free APN index. Cache this info
              ----------------------------------------------------------------------*/
              first_free_apn_index = apn_index;
            }
          }
          else
          { 
            /* Apn mismatch*/ 
            DS_3GPP_MSG1_LOW("APN did not match at index: %d", apn_index);
          }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Unusable NULL APN index %d", apn_index);        
      } //apn match or NULL apn check
      
    } //loop over all entries of ds_3gpp_pdn_throttle_per_apn_tbl

    if(apn_index == DS_MAX_APN_NUM)
    {
      if( (first_free_apn_index == DS_MAX_APN_NUM) ||   
           (allow_create == FALSE) )
      {
        /*------------------------------------------------------------------------
          This case will be reached under the following conditions
           - APN match did not occur, No free APN index exists
           - APN match did not occur, No need to create throt SM for PLMN & APN
        ------------------------------------------------------------------------*/
        DS_3GPP_MSG1_HIGH("APN no match. First free index: %d, Allow create: %d",
                          first_free_apn_index);
        break;
      }
      else if ( (first_free_apn_index >= 0) && 
                (first_free_apn_index < DS_MAX_APN_NUM) )
      {
        apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[first_free_apn_index];

        /*------------------------------------------------------------------------
          Copy the current APN into the index we have chosen to work with.
          This is so that a match can happen next time we try to get the index for
          this APN
        ------------------------------------------------------------------------*/
        apn_index_ptr->valid = TRUE;
        strlcpy( (char*)apn_index_ptr->apn, 
                 (char*)apn, 
                 (DS_UMTS_MAX_APN_STRING_LEN+1) );
      }
    }
    else
    {
      /*--------------------------------------------------------------------------
        APN match occured
        Ensure that apn_index is in acceptable range 
      --------------------------------------------------------------------------*/
      if((apn_index >= 0) && (apn_index < DS_MAX_APN_NUM))
      {
        apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];
      }
    }
  } while(0);

  return (void*)apn_index_ptr;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM_PER_RAT

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
)
{
  sys_plmn_id_s_type                  current_plmn;
  /*------------------------------------------------------------------------*/

  if (throt_ident_info_p == NULL)
  {
    DATA_ERR_FATAL("throt_ident_info_p is NULL");
    return NULL;
  }

  memset((void*)&(current_plmn),0,sizeof(sys_plmn_id_s_type));
  
  if (throt_ident_info_p->globally_blocked == TRUE)
  {
    memscpy((void*)(&(throt_ident_info_p->plmn_id)),
            sizeof(sys_plmn_id_s_type),
            (void*)(&global_plmn_id),
            sizeof(sys_plmn_id_s_type));
  }
  else
  {
    ds_3gpp_pdn_cntx_get_current_plmn_ex(&(throt_ident_info_p->plmn_id),
                                          throt_ident_info_p->subs_id);
  }

  return ds_3gpp_pdn_throt_sm_get_throttle_sm_plmn_based(throt_ident_info_p,
                                                         allow_create);
  
} /* ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM

DESCRIPTION
  This function gets a Throttling state machine instance ptr for a apn on the 
  current plmn. If the pointer does not exist ;an entry will be allocated

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
)
{
  ds_3gpp_pdn_throt_ident_info_type throt_ident_info;
  /*-----------------------------------------------------------------------*/

  ds_3gpp_pdn_throt_fill_ident_info
  (
    &throt_ident_info, 
    apn, 
    DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
    subs_id,
    globally_blocked,
    DS_3GPP_THROT_RAT_GLOBAL
   );

  return ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, 
                                                      allow_create);
  
} /* ds_3gpp_pdn_throt_sm_get_throttle_sm */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_GET_THROTTLE_SM_PLMN_BASED

DESCRIPTION
  This function gets a Throttling state machine instance ptr for a apn, plmn,
  subscription id pair. If the pointer does not exist ;an entry will be allocated

PARAMETERS   :
  *throt_ident_info_p    - Information used to identify the type of throttling
  allow_create           - Flag indicating whether creation is allowed
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
  boolean                            allow_create
)
{
  int                                      existing_index = -1;
  int                                      first_empty_index = -1;
  int                                      loop = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  sys_plmn_id_s_type                       zero_plmn;
  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s          *pdn_throt_sm_info_ptr = NULL;
  ds_3gpp_throt_rat_e_type                 rat_type = DS_3GPP_THROT_RAT_ILLEGAL;
  /*----------------------------------------------------------------------------*/

  if (throt_ident_info_p == NULL)
  {
    DATA_ERR_FATAL("throt_ident_info_p is NULL");
    return NULL;
  }

  DS_3GPP_MSG1_LOW("Get PDN throttling SM for Subs Id: %d",
                   throt_ident_info_p->subs_id);

  memset((void*)&(zero_plmn),0,sizeof(sys_plmn_id_s_type));

  if(memcmp((void*)(&(throt_ident_info_p->plmn_id)),
            (void*)(&zero_plmn),
            sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG0_ERROR("Zero PLMN returned as current PLMN. Can't get throt SM");
    return NULL;
  }

  rat_type = throt_ident_info_p->rat_type; 

  if (rat_type <=DS_3GPP_THROT_RAT_MIN || 
      rat_type >= DS_3GPP_THROT_RAT_MAX)
  {
    ASSERT(0);
    return NULL;
  }

  /*---------------------------------------------------------------------------
    Logic would be as follows: Get the current PLMN first. If current PLMN ret 
    is zero return NULL

    1. Get the APN index in ds_3gpp_pdn_throttle_per_apn_tbl based on passed APN
    2. If none exists allocate one index from the table mentioned above
    3. Look for the PLMN in this index
    4. If present return it
    5. If not present, see if it has to be created based on incoming params
    6. Return created PDN throt sm. If it needed not be created ret NULL
  ---------------------------------------------------------------------------*/


  apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_index_ptr
                    (throt_ident_info_p->apn, allow_create, 
                     throt_ident_info_p->subs_id);
  
  if (apn_index_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("apn_index_ptr is NULL", 0,0,0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Now that we have a ds_3gpp_pdn_throttle_tbl to work with for a specific
    APN, we have to search if a throttling machine for the PLMN already exists

    Search through the pdn throttling table to see if this PLMN has an 
    existing entry, if so it is returned, if not, it is allocated
  -------------------------------------------------------------------------*/
  for( loop = 0; loop < DS_3GPP_MAX_PLMN_SUPPORTED; loop++)
  {
    if (apn_index_ptr->ds_3gpp_pdn_throttle_tbl[loop] == NULL)
    {
      if (first_empty_index < 0)
      {
        first_empty_index = loop;
      }      
    }
    /*--------------------------------------------------------------------------
      Compare current plmn with the PLMN in the index being searched inside
      ds_3gpp_pdn_throttle_per_apn_tbl
    --------------------------------------------------------------------------*/
    else if(memcmp((void*)(&(throt_ident_info_p->plmn_id)),
           (void*)&(apn_index_ptr->ds_3gpp_pdn_throttle_tbl[loop]->plmn_id),
            sizeof(sys_plmn_id_s_type)) == 0)
    {
      /*------------------------------------------------------------------------
        PLMN match. Break from the loop. We have found the entry in the table
        for the current PLMN
      ------------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("PLMN matched at index: %d. Break", loop);
      existing_index = loop;
      break;
    }
  }
  /*-------------------------------------------------------------------------
    If there is an existing entry with current PLMN id, return it
  -------------------------------------------------------------------------*/
  if ((existing_index >= 0) && (existing_index < DS_MAX_APN_NUM))
  {
    pdn_throt_sm_info_ptr = apn_index_ptr->ds_3gpp_pdn_throttle_tbl[existing_index];

    if (pdn_throt_sm_info_ptr != NULL)
    {
      pdn_throt_sm_ptr = &(pdn_throt_sm_info_ptr->per_rat[rat_type]);   

     /*-------------------------------------------------------------------------
      If there is an existing entry with given RAT, return it. 
      Otherwise if allow_create is set to TRUE, 
      intialize a new entry for that RAT and return the new entry.
     -------------------------------------------------------------------------*/
      
      if (pdn_throt_sm_ptr->init != TRUE)
      {
        if (allow_create)
        {
          /*-------------------------------------------------------------------------
            Increment the number of active RATs to keep track of RATs on 
            which we are throttling or maintaining a throttle counter
          -------------------------------------------------------------------------*/

          pdn_throt_sm_info_ptr->num_active_rats++;

          ds_3gpp_pdn_throt_sm_init
          (
            pdn_throt_sm_ptr, pdn_throt_sm_info_ptr, throt_ident_info_p
          );
        }
        else
        {
          pdn_throt_sm_ptr = NULL;
        }
      }
    }
    else
    {
      ASSERT(0);
    }

    return (void*) pdn_throt_sm_ptr;
  }
  else if (first_empty_index >= 0 && first_empty_index < DS_3GPP_MAX_PLMN_SUPPORTED)
  {
    if (allow_create)
    {
      /*-----------------------------------------------------------------------
         Allocate TCM from heap
      -----------------------------------------------------------------------*/
      pdn_throt_sm_info_ptr = 
        apn_index_ptr->ds_3gpp_pdn_throttle_tbl[first_empty_index] = 
          (ds_3gpp_pdn_throttle_sm_info_s *)modem_mem_alloc(
            sizeof(ds_3gpp_pdn_throttle_sm_info_s), MODEM_MEM_CLIENT_DATA);

      if(pdn_throt_sm_info_ptr == NULL)
      {
        DS_3GPP_MSG2_ERROR("Heap Mem Allocation failed for client: %d size: %d",
                   MODEM_MEM_CLIENT_DATA,(sizeof(ds_3gpp_pdn_throttle_sm_s)));
      }
      else
      {
        memset(pdn_throt_sm_info_ptr, 0, sizeof(ds_3gpp_pdn_throttle_sm_info_s));

        /*------------------------------------------------------------------------
          Copy the current PLMN into the PDN throt SM
        ------------------------------------------------------------------------*/
        memscpy((void*)&(pdn_throt_sm_info_ptr->plmn_id),
                sizeof(sys_plmn_id_s_type),
                (void*)&(throt_ident_info_p->plmn_id),
                sizeof(sys_plmn_id_s_type));

        /*----------------------------------------------------------------------
          Set the backward pointer from the PDN throt SM to the Per PLMN throt
          table
        ----------------------------------------------------------------------*/
        pdn_throt_sm_info_ptr->apn_throt_sm_ptr = (void*)apn_index_ptr;

        /*-------------------------------------------------------------------------
          Increment the number of active RATs to keep track of RATs on 
          which we are throttling or maintaining a throttle counter
        -------------------------------------------------------------------------*/
        pdn_throt_sm_info_ptr->num_active_rats++;

        pdn_throt_sm_ptr = &(pdn_throt_sm_info_ptr->per_rat[rat_type]);

        if (pdn_throt_sm_ptr != NULL)
        {
          ds_3gpp_pdn_throt_sm_init
          (
            pdn_throt_sm_ptr,
            pdn_throt_sm_info_ptr, 
            throt_ident_info_p
          );
        }

      }      
      return ((void*)pdn_throt_sm_ptr);
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Returning NULL as allow_create was set to FALSE");
      return NULL;
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Cannot locate an empty entry in pdn throttle table!");
    return NULL;
  } 
} /* ds_3gpp_pdn_throt_sm_get_throttle_sm_plmn_based */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLEANUP_APN_INDEX_ENTRY_IF_NEEDED

DESCRIPTION
  This function cleans up APN Index Entry if no throttling is active for
  that APN.

PARAMETERS   
   APN index ptr
   Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed
(
  void                    *apn_index_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  int                                       loop = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_tbl_ptr = 
    (ds_3gpp_pdn_throttle_sm_apn_based_dyn_s*)apn_index_ptr;

  if(apn_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid arguments. Returning");
    return;
  }

  if (apn_tbl_ptr->blocked_due_to_roaming == FALSE && 
      apn_tbl_ptr->apn_disabled == FALSE &&
      apn_tbl_ptr->ui_data_disabled == FALSE)
  {
    for( loop = 0; loop < DS_3GPP_MAX_PLMN_SUPPORTED; loop++)
    {
      if (apn_tbl_ptr->ds_3gpp_pdn_throttle_tbl[loop] != NULL)
      {
       DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                               "Not cleaning APN index entry for apn: %s ", 
                               (char*)apn_tbl_ptr->apn);
        break;
      }
    }
  }
  else
  {
     DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                             "Not cleaning APN index entry for apn: %s ", 
                             (char*)apn_tbl_ptr->apn);
  }

  if (loop == DS_3GPP_MAX_PLMN_SUPPORTED)
  {
     DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                             "Cleaning APN index entry for apn: %s ", 
                             (char*)apn_tbl_ptr->apn);
     /*--------------------------------------------------------------------------
      There is no more PDN throt SM for the given APN
      NULL out the APN. This will make this index available for throt req
      for a new APN
    --------------------------------------------------------------------------*/
    memset((void*)&(apn_tbl_ptr->apn), 0, (DS_UMTS_MAX_APN_STRING_LEN+1));
    apn_tbl_ptr->valid = FALSE;

    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn (subs_id);
  }
} /* ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_DEALLOC_THROT_SM

DESCRIPTION
  This function deallocates the TCM for a pdn context point in the APN table

PARAMETERS   :
   plmn_index_ptr - Pointer to let us know which PLMN index in the PLMN tbl
                    we are working with currently
   byte* - pointer to the APN

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - This iface pointer points to a default iface
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_dealloc_throt_sm
(
  void                   *apn_index_ptr, 
  sys_plmn_id_s_type     *plmn_ptr,
  sys_modem_as_id_e_type  subs_id
) 
{
  int                                       loop = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_tbl_ptr = 
    (ds_3gpp_pdn_throttle_sm_apn_based_dyn_s*)apn_index_ptr;
  ds_3gpp_throt_rat_e_type                  rat_index = 0;
  ds_3gpp_pdn_throttle_sm_s                *pdn_throt_sm_ptr = NULL;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(apn_tbl_ptr == NULL || plmn_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid arguments. Returning");
    return;
  }
  
  for( loop = 0; loop < DS_3GPP_MAX_PLMN_SUPPORTED; loop++)
  {
    if (apn_tbl_ptr->ds_3gpp_pdn_throttle_tbl[loop] != NULL)
    {
      if(memcmp((void*)plmn_ptr,
                (void*)&(apn_tbl_ptr->ds_3gpp_pdn_throttle_tbl[loop]->plmn_id),
                 sizeof(sys_plmn_id_s_type)) == 0)
      {
        /*---------------------------------------------------------------------
         PLMN match. We have found the entry in the table for the current PLMN.
         Free the state machine memory.
        ---------------------------------------------------------------------*/
        DS_3GPP_MSG1_MED("PLMN matched at index: %d", loop);

        for (rat_index = 0; rat_index < DS_3GPP_THROT_RAT_ILLEGAL; rat_index++)
        {
          pdn_throt_sm_ptr = 
            &(apn_tbl_ptr->ds_3gpp_pdn_throttle_tbl[loop]->per_rat[rat_index]);

          if (pdn_throt_sm_ptr->init == TRUE)
          {
            pdn_throt_sm_ptr->init = FALSE;

            if (pdn_throt_sm_ptr->v4_throttle_sm)
            {
              (void)rex_clr_timer(&(pdn_throt_sm_ptr->
                                    v4_throttle_sm->throttle_timer));

              ds_3gppi_throt_sm_free_core_state_machine(
                pdn_throt_sm_ptr->v4_throttle_sm);

              pdn_throt_sm_ptr->v4_throttle_sm = NULL;
            }

            if (pdn_throt_sm_ptr->v6_throttle_sm)
            {
              (void)rex_clr_timer(&(pdn_throt_sm_ptr->
                                    v6_throttle_sm->throttle_timer));

              ds_3gppi_throt_sm_free_core_state_machine(
                pdn_throt_sm_ptr->v6_throttle_sm);

              pdn_throt_sm_ptr->v6_throttle_sm = NULL;
            }
          } /* (pdn_throt_sm_ptr->init) */
        } /* for (rat_index = 0; rat_index < DS_3GPP_THROT_RAT_ILLEGAL; rat_index++) */

        modem_mem_free(apn_tbl_ptr->ds_3gpp_pdn_throttle_tbl[loop], 
                       MODEM_MEM_CLIENT_DATA);

        apn_tbl_ptr->ds_3gpp_pdn_throttle_tbl[loop] = NULL;
      } // PLMN comparison
    } // if PDN throt SM exists in this index
  } //loop over all PDN throt SM for a given PLMN

  ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed(apn_index_ptr, subs_id);

  DS_3GPP_MSG0_ERROR("No entry in pdn throt sm tbl to dealloc!");
  return;
} /* ds_3gpp_pdn_throt_dealloc_throt_sm */

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
) 
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
  For globally blocked PLMN's unblock all plmn's 
  else 
  unblock only the individual plmn
  -------------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn (subs_id);
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_validate_and_unblock_plmn");
#ifdef FEATURE_DATA_LTE
  if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available(subs_id) 
        == DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL)
  {
    if(!ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id))
    { 
      DS_3GPP_MSG0_HIGH("Global PLMN unblock");
      ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns(subs_id);
      ds_3gpp_pdn_cntxt_flush_blocked_plmn_list(subs_id);
    }
    else if (ds_3gpp_pdn_cntxt_is_plmn_blocked(plmn_id,subs_id))
    {
      DS_3GPP_MSG0_HIGH("Per plmn throttling unblock");
      ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(
                      plmn_id, 
                      CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY, 
                      subs_id);
      ds_3gpp_pdn_cntxt_flush_plmn_from_list(
                      plmn_id,
                      subs_id); 
    }
  }
#endif /* FEATURE_DATA_LTE */
  return;
}/*ds_3gpp_pdn_throt_validate_and_unblock_plmn*/


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLEAR_THROTTLE_INFO

DESCRIPTION
  This function clears throttling info for a pdn context and a given ip
  type.

PARAMETERS   :
   ds_pdn_context_s * - pdn context ptr
   pdn_pdp_type       - PDP Type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_throt_clear_throttle_info
(
  ds_3gpp_pdn_throttle_sm_s  *pdn_throt_sm_ptr,
  ds_umts_pdp_type_enum_type  pdn_pdp_type,
  sys_modem_as_id_e_type      subs_id
) 
{
  ds_3gpp_pdn_throttle_sm_info_s *pdn_throt_sm_info_p = NULL;
  uint8                           v4_failure_count = 0;
  uint8                           v6_failure_count = 0;
  /*--------------------------------------------------------------------------------*/

  /* In case of global release 10 throttling we deallocate the v4 throttle
     state machine and v6 global throttle state machine*/
  if (pdn_throt_sm_ptr != NULL)
  {
    if ((pdn_pdp_type == DS_UMTS_PDP_IPV4) ||
        (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
    {

      if (pdn_throt_sm_ptr->v4_throttle_sm)
      {
        /*----------------------------------------------------------------------- 
           Stop the timer and reset count cause code and is_throttled flag
         ------------------------------------------------------------------------*/
        (void) rex_clr_timer(&(pdn_throt_sm_ptr->v4_throttle_sm->throttle_timer));
        ds_3gppi_throt_sm_set_is_throttled_flag(pdn_throt_sm_ptr->
                                              v4_throttle_sm,FALSE);

        pdn_throt_sm_ptr->v4_throttle_sm->user_data.cause_code = 0;

        ds_3gppi_throt_sm_set_failure_count(pdn_throt_sm_ptr->v4_throttle_sm, 0);
 
        ds_3gppi_throt_sm_set_throttle_timer_val
          (pdn_throt_sm_ptr->v4_throttle_sm, 0);

        ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_ptr->v4_throttle_sm,
                                         pdn_throt_sm_ptr->v4_throttle_sm->apn,
                                         DS_UMTS_PDP_IPV4);

        ds_3gpp_pdn_throt_clr_log_entry(pdn_throt_sm_ptr->v4_throttle_sm);

        /*--------------------------------------------------------------------- 
          Check if the apn is a part of attach PDN list, If it is store the
          PLMN ID to unblock LTE
        -----------------------------------------------------------------------*/
        if(ds_dsd_apm_is_apn_in_attach_pdn_list(
           subs_id, (char*)pdn_throt_sm_ptr->v4_throttle_sm->apn))
        {
          ds_3gpp_pdn_throt_validate_and_unblock_plmn(pdn_throt_sm_ptr->
                                                      v4_throttle_sm->plmn_id,
                                                      subs_id);
        }
      }

    }
    if ((pdn_pdp_type == DS_UMTS_PDP_IPV6) ||
        (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
    {
      if (pdn_throt_sm_ptr->v6_throttle_sm)
      {
        /*------------------------------------------------------------------------ 
          Stop the timer and reset count,cause code and is_throttled flag
         ------------------------------------------------------------------------*/
        (void) rex_clr_timer(&(pdn_throt_sm_ptr->v6_throttle_sm->throttle_timer));

        ds_3gppi_throt_sm_set_is_throttled_flag(pdn_throt_sm_ptr->
                                              v6_throttle_sm,FALSE);

        pdn_throt_sm_ptr->v6_throttle_sm->user_data.cause_code = 0;

        ds_3gppi_throt_sm_set_failure_count(pdn_throt_sm_ptr->v6_throttle_sm, 0);

        ds_3gppi_throt_sm_set_throttle_timer_val
          (pdn_throt_sm_ptr->v6_throttle_sm, 0);

        ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_ptr->v6_throttle_sm,
                                         pdn_throt_sm_ptr->v6_throttle_sm->apn,
                                         DS_UMTS_PDP_IPV6);

        ds_3gpp_pdn_throt_clr_log_entry(pdn_throt_sm_ptr->v6_throttle_sm);
      /*--------------------------------------------------------------------- 
          Check if the apn is a part of attach PDN list, If it is store the
          PLMN ID to unblock LTE
        -----------------------------------------------------------------------*/
        if(ds_dsd_apm_is_apn_in_attach_pdn_list(
           subs_id, (char*)pdn_throt_sm_ptr->v6_throttle_sm->apn))
        {
          ds_3gpp_pdn_throt_validate_and_unblock_plmn(pdn_throt_sm_ptr->  
                                                      v6_throttle_sm->plmn_id,
                                                      subs_id);
        }
      }
    }

    ds_3gppi_throt_sm_get_failure_count(pdn_throt_sm_ptr->v4_throttle_sm,
                                        &v4_failure_count);

    ds_3gppi_throt_sm_get_failure_count(pdn_throt_sm_ptr->v6_throttle_sm,
                                        &v6_failure_count);

    /*---------------------------------------------------------------------------- 
      If v4 failure count is 0 and v4 is unthrottled,
      If v6 failure count is 0 and v6 is unthrottled only then clean up.
    ----------------------------------------------------------------------------*/
    if((v4_failure_count == 0 && (pdn_throt_sm_ptr->v4_throttle_sm) &&
        !(pdn_throt_sm_ptr->v4_throttle_sm->is_throttled)) &&
       (v6_failure_count == 0 && (pdn_throt_sm_ptr->v6_throttle_sm) &&
        !(pdn_throt_sm_ptr->v6_throttle_sm->is_throttled)))
    {
      pdn_throt_sm_info_p = pdn_throt_sm_ptr->pdn_throt_sm_info_p;

      if (pdn_throt_sm_info_p != NULL)
      {
        /*-------------------------------------------------------------------------
          Decrement the number of active RATs
        -------------------------------------------------------------------------*/
        pdn_throt_sm_info_p->num_active_rats--;

        if (pdn_throt_sm_info_p->num_active_rats == 0)
        {
          ds_3gpp_pdn_throt_dealloc_throt_sm
            (pdn_throt_sm_info_p->apn_throt_sm_ptr, 
             &(pdn_throt_sm_info_p->plmn_id), subs_id);
        }
      }
      else 
      {
        DS_3GPP_MSG0_ERROR("pdn_throt_sm_info_p is NULL ! This looks wrong");
      }
    }
    else
    {
      if(pdn_pdp_type == DS_UMTS_PDP_IPV4 || 
         pdn_pdp_type == DS_UMTS_PDP_IPV6 || 
         pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
      {
        ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn (subs_id);
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Invalid PDP type, Exiting");
      }
    }
  }
} /* ds_3gpp_pdn_throt_clear_throttle_info */


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNTHROTTLE_AND_CLEAR_USER_DATA

DESCRIPTION
  This function clears throttling info for a pdn context and a given ip
  type and cleans up the user data 

PARAMETERS   :
   ds_3gpp_pdn_throttle_sm_s  *pdn_throt_sm_ptr
   pdn_pdp_type                PDP Type
   throt_user_data_p           Throttle user data
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
) 
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if((pdn_throt_sm_ptr == NULL) || (throt_user_data_p == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input ptr is NULL:pdn_throt_sm_ptr:0x%x,"
                       "throt_user_Data_p:0x%x",pdn_throt_sm_ptr,
                       throt_user_data_p);
    return;
  }

  pdn_throt_sm_ptr->sm_type = 
     DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED;


  ds_3gpp_pdn_throt_clear_throttle_info(pdn_throt_sm_ptr,
                                        pdn_pdp_type,
                                        subs_id);

  return;

}/* ds_3gpp_pdn_throt_unthrottle_and_clear_user_data*/


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
) 
{
  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  boolean                                  apn_globally_blocked = FALSE;
  ds_3gpp_pdn_throt_ident_info_type        throt_ident_info;
  ds_3gpp_throt_rat_e_type                 rat_type = DS_3GPP_THROT_RAT_ILLEGAL;
  byte                                     throt_apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
                                           /* RAT on which throttling is
                                              to be unblocked */
  /*--------------------------------------------------------------*/

  if (apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid arguments. Returning");
    return;
  }
  else
  {
    memset(throt_apn_name, 0, DS_UMTS_MAX_APN_STRING_LEN+1);
    strlcpy((char *)throt_apn_name, 
                  (char *)apn, 
                  (DS_UMTS_MAX_APN_STRING_LEN+1));

  }

  ds3gpp_enter_global_crit_section();

  apn_globally_blocked = ds_3gpp_pdn_throt_is_apn_globally_throttled
                           (throt_apn_name, subs_id);

  if (apn_globally_blocked == TRUE)
  {
    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                        (throt_apn_name, TRUE, FALSE, DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
                         subs_id);

   /*--------------------------------------------------------------------
     Clear globally_blocked flag associated with APN
    --------------------------------------------------------------------*/
    apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index(throt_apn_name, subs_id);

    if (apn_index_ptr != NULL)
    {
      apn_index_ptr->globally_blocked = FALSE;
    }
    /*----------------------------------------------------------------------- 
      Overwrite the pdn_pdp_type to IPv4v6 if APN was globally blocked,
      since global blocking involves both ipv4 and ipv6.
    ------------------------------------------------------------------------*/
    pdn_pdp_type = DS_UMTS_PDP_IPV4V6;

    ds_3gpp_pdn_throt_clear_throttle_info(pdn_throt_sm_ptr,pdn_pdp_type,subs_id);
  }

  /*----------------------------------------------------------------------- 
    Extract all RATs on which throttling needs to be unblocked and 
    unblock throttling on those RATs one by one.
  ------------------------------------------------------------------------*/
  while ( rat_mask != DS_3GPP_THROT_RAT_MASK_NONE)
  {
    rat_type = ds_3gpp_pdn_throt_get_next_rat_type(&rat_mask);

    ds_3gpp_pdn_throt_fill_ident_info
      (&throt_ident_info, 
       throt_apn_name, 
       DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
       subs_id,
       FALSE,
       rat_type
       );

    pdn_throt_sm_ptr = 
      ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, FALSE);

    ds_3gpp_pdn_throt_clear_throttle_info(pdn_throt_sm_ptr,pdn_pdp_type,subs_id);

  } /* while ( rat_mask != DS_3GPP_THROT_RAT_MASK_NONE) */

  ds3gpp_leave_global_crit_section();
  return;
} /* ds_3gpp_pdn_throt_clear_throttle_state */

/*===========================================================================
FUNCTION DS_3GPP_THROT_HANDLE_CONN_FAILURE

DESCRIPTION
  This function handles connection failure for a PDN context.

PARAMETERS   :
   *throt_ident_info_p        - Information used to identify the type
                                of throttling
   ds_pdn_context_s            *pdn_cntxt_p
   void*                        cause_code
   ds_umts_pdp_type_enum_type - pdp type
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void ds_3gpp_pdn_throt_handle_conn_failure
(
  ds_3gpp_pdn_throt_ident_info_type *throt_ident_info_p,
  ds_umts_pdp_type_enum_type         pdn_pdp_type,
  void*                              cause_code,
  ds_pdn_context_s                  *pdn_cntxt_p
)
{
  ds_3gpp_pdn_throttle_sm_s*   pdn_throt_sm_ptr = NULL;

  /*------------------------------------------------------------------------*/

  ASSERT(throt_ident_info_p != NULL);

  ds3gpp_enter_global_crit_section();

  /* get the pdn throt sm for this pdn, if it does not exist, it will be created */
  pdn_throt_sm_ptr = 
    ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(throt_ident_info_p, TRUE);

  if (pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN throt allocation failed,pdn  throttling not applied");
    ds3gpp_leave_global_crit_section();
    return;
  }
  /* check if throttling is needed */
  if ((pdn_pdp_type == DS_UMTS_PDP_IPV4) ||
      (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
  {
    ds_3gppi_throt_sm_feed_failure_to_core_sm
                                            (pdn_throt_sm_ptr->v4_throttle_sm, 
                                             cause_code, 
                                             throt_ident_info_p->subs_id,
                                             pdn_cntxt_p);
 /*-------------------------------------------------------------------------- 
      Chect is_throttled flag and if the throttled flag is set, log the
      information
      -------------------------------------------------------------------------*/
    if ( pdn_throt_sm_ptr->v4_throttle_sm != NULL &&
         pdn_throt_sm_ptr->v4_throttle_sm->is_throttled)
    {

      /*----------------------------------------------------------------------- 
       Update pdn_throt_profile_num_list with the throt_sm_ptr
      -----------------------------------------------------------------------*/
      ds_3gpp_pdn_throt_update_log_list(pdn_throt_sm_ptr->
                                               v4_throttle_sm,DS_UMTS_PDP_IPV4);
      ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_ptr->v4_throttle_sm,
                                       pdn_throt_sm_ptr->v4_throttle_sm->apn,
                                       DS_UMTS_PDP_IPV4);

    }
    else
    {
      DS_3GPP_MSG1_ERROR("V4 throttle ptr is %d",pdn_throt_sm_ptr->v4_throttle_sm);
    }
  }
  if ((pdn_pdp_type == DS_UMTS_PDP_IPV6) ||
      (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
  {
    ds_3gppi_throt_sm_feed_failure_to_core_sm(pdn_throt_sm_ptr->v6_throttle_sm, 
                                             cause_code, 
                                             throt_ident_info_p->subs_id,
                                             pdn_cntxt_p);

    /*-------------------------------------------------------------------------- 
    Chect is_throttled flag and if the throttled flag is set, log the
    information
    -------------------------------------------------------------------------*/
    if( pdn_throt_sm_ptr->v6_throttle_sm != NULL &&
        pdn_throt_sm_ptr->v6_throttle_sm->is_throttled)
    {

      /*----------------------------------------------------------------------- 
       Update pdn_throt_profile_num_list with the throt_sm_ptr
      -----------------------------------------------------------------------*/
      ds_3gpp_pdn_throt_update_log_list(pdn_throt_sm_ptr->
                                               v6_throttle_sm,DS_UMTS_PDP_IPV6);

      ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_ptr->v6_throttle_sm,
                                       pdn_throt_sm_ptr->v6_throttle_sm->apn,
                                       DS_UMTS_PDP_IPV6);

    }
    else
    {
      DS_3GPP_MSG1_ERROR("V6 throttle ptr is %d",pdn_throt_sm_ptr->v6_throttle_sm);
    }
  }
  ds3gpp_leave_global_crit_section();
  return;
} /* ds_3gpp_pdn_throt_handle_conn_failure */

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
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s* apn_index_ptr;
  /*-------------------------------------------------------------------------*/

  /* find the index for given apn in apn based throt table */
  apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index(apn, subs_id);

  if (apn_index_ptr != NULL)
  {
    return(apn_index_ptr->globally_blocked);
  }
  else
  {
    return FALSE;
  }
}/*ds_3gpp_pdn_throt_is_apn_globally_throttled*/

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s* apn_index_ptr;
  /*-------------------------------------------------------------------------*/

  /* find the index for given apn in apn based throt table */
  apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index(apn, subs_id);

  if (apn_index_ptr != NULL)
  {
    return(apn_index_ptr->blocked_due_to_roaming);
  }
  else
  {
    return FALSE;
  }
} /* ds_3gpp_pdn_throt_is_apn_roaming_throttled */

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s* apn_index_ptr;
  /*-------------------------------------------------------------------------*/

  /* find the index for given apn in apn based throt table */
  apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index(apn, subs_id);

  if (apn_index_ptr != NULL)
  {
    return(apn_index_ptr->apn_disabled);
  }
  else
  {
    return FALSE;
  }
} /* ds_3gpp_pdn_throt_is_apn_disabled */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_APN_BLOCKED_ALLPLMNS_OR_ROAMING

DESCRIPTION
  This function checks if given APN is blocked on all the plmns or blocked due
  to roaming

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s* apn_index_ptr;
  /*-------------------------------------------------------------------------*/

  /* find the index for given apn in apn based throt table */
  apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index(apn, subs_id);

  if (apn_index_ptr != NULL)
  {
    return(apn_index_ptr->apn_disabled || apn_index_ptr->globally_blocked 
     || apn_index_ptr->ui_data_disabled || apn_index_ptr->blocked_due_to_roaming);
  }
  else
  {
    return FALSE;
  }
} /* ds_3gpp_pdn_throt_is_apn_disabled */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_IS_UI_DATA_DISABLED

DESCRIPTION
  This function checks if UI Data is disabled.

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
boolean ds_3gpp_pdn_throt_is_ui_data_disabled
(
  byte                   *apn,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s* apn_index_ptr;
  /*-------------------------------------------------------------------------*/

  /* find the index for given apn in apn based throt table */
  apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index(apn, subs_id);

  if (apn_index_ptr != NULL)
  {
    return(apn_index_ptr->ui_data_disabled);
  }
  else
  {
    return FALSE;
  }
} /* ds_3gpp_pdn_throt_is_ui_data_disabled */

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
)
{
  boolean                            result = FALSE;
  ds_3gpp_pdn_throttle_sm_s*         pdn_throt_sm_ptr = NULL;
  sys_sys_mode_e_type                current_mode = SYS_SYS_MODE_NONE;
  uint32                             rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                     /* Mask indicating RATs 
                                        on which we need to check whether APN 
                                        is throttled */
  ds_3gpp_throt_rat_e_type           rat_type = DS_3GPP_THROT_RAT_ILLEGAL;
                                     /* RAT Type extracted from the Mask */
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
  /*-------------------------------------------------------------------------*/
  current_mode = ds3gpp_get_current_network_mode(subs_id);

  DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                          "Checking throt state for APN %s",(char*)apn);

  ds3gpp_enter_global_crit_section();

  /* check if it is globally blocked on all PLMNs and all pdp_type's */
  if(ds_3gpp_pdn_throt_is_apn_globally_throttled(apn, subs_id))
  {
    DS_3GPP_MSG0_HIGH("APN is globally blocked");
    ds3gpp_leave_global_crit_section();
    return TRUE;
  }

  if(ds_3gpp_pdn_throt_is_apn_roaming_throttled(apn, subs_id))
  {
    DS_3GPP_MSG0_HIGH("APN is throttled due to roaming");
    ds3gpp_leave_global_crit_section();
    return TRUE;
  }

  if(ds_3gpp_pdn_throt_is_apn_disabled(apn, subs_id))
  {
    DS_3GPP_MSG0_HIGH("APN is disabled");
    ds3gpp_leave_global_crit_section();
    return TRUE;
  }

  if(ds_3gpp_pdn_throt_is_ui_data_disabled(apn, subs_id))
  {
    DS_3GPP_MSG0_HIGH("APN is disabled from UI");
    ds3gpp_leave_global_crit_section();
    return TRUE;
  }

  /*------------------------------------------------------------------
    We run a loop to check whether APN is throttled on any RATs
    indicated by the RAT Mask  
  -------------------------------------------------------------------*/
  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    rat_mask |= ds_3gpp_map_cm_sys_mode_to_rat_mask(current_mode);
  }

  while ( rat_mask != DS_3GPP_THROT_RAT_MASK_NONE)
  {
    rat_type = ds_3gpp_pdn_throt_get_next_rat_type(&rat_mask);

    ds_3gpp_pdn_throt_fill_ident_info
      (&throt_ident_info, 
       apn, 
       DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
       subs_id,
       FALSE,
       rat_type
       );

    /* get the TCM pointer for this pdn */
    pdn_throt_sm_ptr = 
      ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, FALSE);

    /*--------------------------------------------------------------------------
      If RPM is enabled then if any of the throttle SM is throttled then return
      TRUE for that APN as throttling is per APN based in RPM
    --------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_RPM
    if( FALSE == ds_3gpp_is_rpm_enabled(subs_id) )
    {
#endif /* FEATURE_DATA_RPM */
      if (pdn_throt_sm_ptr != NULL)
      {
        switch(call_type)
        {
          case DS_3GPP_PDP_TYPE_IPV4:
            if (ds_3gppi_throt_sm_get_is_throttled_flag
                         (pdn_throt_sm_ptr->v4_throttle_sm))
            {
              if(ds_3gppi_is_rel_11_supported())
              {
                if((pdn_throt_sm_ptr->v4_throttle_sm->throttle_sys_mask & current_mode))
                {
                  result = TRUE;
                }
              }
              else
              {
                result = TRUE;
              }
            }
            break;
          case DS_3GPP_PDP_TYPE_IPV6:
            if (ds_3gppi_throt_sm_get_is_throttled_flag
                         (pdn_throt_sm_ptr->v6_throttle_sm))
            {
              if(ds_3gppi_is_rel_11_supported())
              {
                if((pdn_throt_sm_ptr->v6_throttle_sm->throttle_sys_mask & current_mode))
                {
                  result = TRUE;
                }
              }
              else
              {
                result = TRUE;
              }
            }
            break;
          case DS_3GPP_PDP_TYPE_IP_ANY:
            if (ds_3gppi_throt_sm_get_is_throttled_flag
                         (pdn_throt_sm_ptr->v4_throttle_sm) &&
                ds_3gppi_throt_sm_get_is_throttled_flag
                         (pdn_throt_sm_ptr->v6_throttle_sm))
            {
              if(ds_3gppi_is_rel_11_supported())
              {
                if((pdn_throt_sm_ptr->v4_throttle_sm->throttle_sys_mask & current_mode) &&
                   (pdn_throt_sm_ptr->v6_throttle_sm->throttle_sys_mask & current_mode))
                {
                  result = TRUE;
                }
              }
              else
              {
                result = TRUE;
              }
            }
          default:
          	DS_3GPP_MSG0_HIGH("Invalid PDP type.");
            break;
        }
        DS_3GPP_MSG1_HIGH("Found throt SM for APN,Throt state: %d",result);
      }
    
#ifdef FEATURE_DATA_RPM
    }
    else
    {
      if (pdn_throt_sm_ptr != NULL)
      {
        if (ds_3gppi_throt_sm_get_is_throttled_flag
                                            (pdn_throt_sm_ptr->v4_throttle_sm)||
            ds_3gppi_throt_sm_get_is_throttled_flag
                                            (pdn_throt_sm_ptr->v6_throttle_sm))
        {
          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                                  "APN: %s is throttled",apn);
          result = TRUE;
        }
        else
        {
          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                                  "APN: %s is not throttled",apn);
        }
      }
    }
#endif /* FEATURE_DATA_RPM */

#ifdef FEATURE_DATA_LTE
    if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id))&&  	 
  	 (ds3g_get_current_network_mode_ex(subs_id) == SYS_SYS_MODE_LTE)&&
  	 (ds_eps_pdn_throt_is_apn_throttled_on_curr_plmn(apn, subs_id)))
    {
      DS_3GPP_MSG0_ERROR("APN is blocked on the current PLMN, call not allowed");      
      result =  TRUE;
    }
#endif /* FEATURE_DATA_LTE */

    if (result == TRUE)
    {
      break;
    }

  }


  ds3gpp_leave_global_crit_section();
  return result;
}

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
)
{
  sys_modem_as_id_e_type             subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
  boolean                            apn_globally_blocked = FALSE;
  /*-------------------------------------------------------------------------*/

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
  {
    return;
  }
#ifdef FEATURE_DATA_RPM

  if (TRUE == ds_3gpp_is_rpm_enabled(subs_id))
  {
    if (FALSE == conn_status)
    {
      /* Reject case */
      if ( TRUE == ds_3gpp_rpm_handle_pdp_act_reject(
                                        pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
                                        down_reason,
                                        subs_id))
      {

        apn_globally_blocked = 
          ds_3gpp_pdn_throt_is_apn_globally_throttled
            (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
             subs_id);

        ds_3gpp_pdn_throt_fill_ident_info
          (&throt_ident_info, 
           pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
           DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
           subs_id,
           apn_globally_blocked,
           DS_3GPP_THROT_RAT_GLOBAL);

        ds_3gpp_pdn_throt_handle_conn_failure
          (&throt_ident_info,  
           pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type,
           NULL,
           pdn_context_p);
  
        if (TRUE == ds_3gpp_is_rpm_enabled(subs_id))
        {
          ds_3gpp_rpm_start_hourly_timer(pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
                                         SEC_TO_MS(SEC_PER_HOUR), subs_id);
        }
  
        if (FALSE == 
             ds_3gpp_rpm_write_throttle_params(
                (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn), subs_id))
        {
          DS_3GPP_MSG1_ERROR("Could not backup RPM params to EFS for apn: %s",
                              pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn);
        }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("ds_3gpp_rpm_handle_pdp_act_reject returned FALSE "
                           "not throttling apn: %s", 
                           pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn);
      }
    }
    else
    {
      /* Success case */
      ds_3gpp_rpm_handle_pdp_act_accept((pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn),
                                        subs_id);
    }


  }
#endif /* FEATURE_DATA_RPM */

#ifdef FEATURE_DATA_LTE
  if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE) &&
     ( conn_status == TRUE))
  {
    if(ds_eps_pdn_throt_is_any_lte_plmn_blocked(subs_id) == TRUE)
    {
      ds_eps_pdn_throt_start_lte_unblock_plmn_timer(
					 DS_EPS_PDN_THROT_2_HOURS_TIMER, 
					 subs_id);
    }
  }
  else
#endif /*  FEATURE_DATA_LTE */
  {    
    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
  }

} /* ds_3gpp_pdn_throt_handle_conn_response */
#ifdef FEATURE_DATA_RPM

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
)
{
  uint8 apn_index  = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  sys_plmn_id_s_type                       zero_plmn;
  uint8                                    plmn_index = 0;
  ds_3gpp_pdn_throttle_sm_s               *pdn_throttle_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_s   	          *current_pdn_throttle_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s          *pdn_throttle_sm_info_ptr = NULL;
  boolean                                  ipv4_throttled = FALSE, 
                                           ipv6_throttled = FALSE; 
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  boolean                                  blocked_due_to_roaming = 
                                             FALSE;
  boolean                                  apn_disabled = FALSE;
  boolean                                  ui_data_disabled = FALSE;
  ds_3gpp_pdn_throt_temp_status_type       temp_throt_status;
  boolean                                  inform_other_clients = FALSE;
  ds_3gpp_throt_rat_e_type                 rat_index = DS_3GPP_THROT_RAT_GLOBAL;
  boolean                                  current_plmn_valid = FALSE;
/*---------------------------------------------------------------------------*/

  if((blocked_apn_list == NULL) ||(num_blocked_apn == NULL))
  {
    DS_3GPP_MSG0_ERROR("Passed argument is NULL");
    return FALSE;
  }

  *num_blocked_apn = 0;
  memset((void*)blocked_apn_list, 0, 
         DS_MAX_APN_NUM*sizeof(ds_3gpp_pdn_throt_apn_list_type));

  memset((void*)&(zero_plmn),0,sizeof(sys_plmn_id_s_type));


  if(memcmp((void*)(&plmn_id),
            (void*)(&zero_plmn),
            sizeof(sys_plmn_id_s_type)) != 0)
  {
    current_plmn_valid = TRUE;
  }

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return FALSE;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

    if( (apn_index_ptr != NULL) && 
        (apn_index_ptr->valid == TRUE) )
    {
      ipv4_throttled = FALSE;
      ipv6_throttled = FALSE;
      pdn_throttle_sm_ptr = NULL;
      current_pdn_throttle_sm_ptr = NULL;
      pdn_throttle_sm_info_ptr = NULL;
      
      blocked_due_to_roaming = apn_index_ptr->blocked_due_to_roaming;
      apn_disabled = apn_index_ptr->apn_disabled;
      ui_data_disabled = apn_index_ptr->ui_data_disabled;

      if (blocked_due_to_roaming == FALSE && 
          apn_disabled == FALSE && 
          ui_data_disabled == FALSE)
      {
        for (plmn_index = 0; plmn_index<DS_3GPP_MAX_PLMN_SUPPORTED; plmn_index++)
        {
          pdn_throttle_sm_info_ptr 
                 = apn_index_ptr->ds_3gpp_pdn_throttle_tbl[plmn_index];

          if (pdn_throttle_sm_info_ptr == NULL)
          {
            continue;
          }

          if (memcmp(&(pdn_throttle_sm_info_ptr->plmn_id), 
                      &(global_plmn_id), sizeof(sys_plmn_id_s_type)) == 0)
          {
            rat_index = DS_3GPP_THROT_RAT_GLOBAL;
          }
          else if (TRUE == current_plmn_valid &&
                   memcmp(&(pdn_throttle_sm_info_ptr->plmn_id), 
                      &(plmn_id), sizeof(sys_plmn_id_s_type)) == 0)
          {
            /* If per RAT Throttling is enabled,
               we need to indicate only those APNs to APM that are throttled on 
               LTE */
            if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
            {
              rat_index = DS_3GPP_THROT_RAT_LTE;
            }
            else
            {
              rat_index = DS_3GPP_THROT_RAT_GLOBAL;
            }

          }
          else
          {
            continue;
          }
          
          if (rat_index > DS_3GPP_THROT_RAT_MIN &&
              rat_index < DS_3GPP_THROT_RAT_MAX)
          {
            pdn_throttle_sm_ptr = &(pdn_throttle_sm_info_ptr->per_rat[rat_index]);
          }
          else
          {
            pdn_throttle_sm_ptr = NULL;
          }

          if (pdn_throttle_sm_ptr != NULL)
          {
            if(pdn_throttle_sm_ptr->v4_throttle_sm != NULL)
            {
              ipv4_throttled = ds_3gppi_throt_sm_get_is_throttled_flag(
                                           pdn_throttle_sm_ptr->v4_throttle_sm);
              if(ipv4_throttled)
              {
                current_pdn_throttle_sm_ptr = pdn_throttle_sm_ptr;      
              } 
            }
          
            if(pdn_throttle_sm_ptr->v6_throttle_sm != NULL)
            {
              ipv6_throttled = ds_3gppi_throt_sm_get_is_throttled_flag(
                                           pdn_throttle_sm_ptr->v6_throttle_sm);
            }
          } /* if (pdn_throttle_sm_ptr != NULL) */
        } /* end for(plmn_index)*/

        memset(&temp_throt_status, 0, 
               sizeof(ds_3gpp_pdn_throt_temp_status_type));
        
        temp_throt_status.v4_throttle_status = ipv4_throttled;
        temp_throt_status.v6_throttle_status = ipv6_throttled;

        inform_other_clients = ds_3gpp_pdn_throt_decide_throttled_ip_type (
                                                    apn_index_ptr->apn,
                                                    &temp_throt_status, 
                                                    subs_id);
         /*If cc_sp_throttling is enabled advertize only globally blocked apns*/
        if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE )&&
           (inform_other_clients)&&
           (ds_3gpp_pdn_throt_is_apn_globally_throttled(apn_index_ptr->apn, subs_id) == FALSE))
        {
          inform_other_clients = FALSE;
        } 
      }
      else
      {
        DS_3GPP_MSG3_HIGH("APN Disabled: %d, Roaming Disabled: %d," 
                          "Data UI Disabled: %d",
                          apn_disabled, blocked_due_to_roaming, 
                          ui_data_disabled);
        inform_other_clients = TRUE;
      }
      
      if(inform_other_clients)
      {
        DS_3GPP_MSG1_MED("Found a blocked APN at index %d", apn_index);
        strlcpy((char*)blocked_apn_list->apn, 
                (char*)apn_index_ptr->apn,
                (DS_UMTS_MAX_APN_STRING_LEN+1));
        blocked_apn_list->len = strlen((const char*)blocked_apn_list->apn);
        if(!ds_3gpp_nv_manager_get_reg_pdn_throttling(subs_id))
        {
          blocked_apn_list->attach_allowed = TRUE;
          if(blocked_due_to_roaming | apn_disabled | ui_data_disabled)
          {
            blocked_apn_list->attach_allowed = FALSE;
          }
          else
          {
            if((current_pdn_throttle_sm_ptr!= NULL) &&
               ((current_pdn_throttle_sm_ptr->sm_type == DS_3GPP_THROTTLE_TYPE_T3396)
               ||(current_pdn_throttle_sm_ptr->sm_type == 
                  DS_3GPP_THROTTLE_TYPE_T3396_ABSENT)))
            {
              blocked_apn_list->attach_allowed = FALSE;
            }
          }
        }
        else
        {
          blocked_apn_list->attach_allowed = FALSE;
        }
        blocked_apn_list++;
        (*num_blocked_apn)++;
      }
    } /* apn_index is valid*/
  } /* end for(apn_index)*/
  return TRUE;
}/*ds_3gpp_pdn_throt_get_blocked_apn_list_on_cur_plmn()*/


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
 
  pdn_throt_sm_ptr         -  PDN throttle state machine pointer to fetch the 
                       cause code from the core throttle state machine
  v4_cause_code_ptr        -  Failure cause code ptr to populate V4 causecode
  v6_cause_code_ptr        -  Failure cause code ptr to populate V6 causecode 

DEPENDENCIES
  None.

RETURN VALUE
  boolean -  TRUE indicating SUCCESS.
		         FALSE indicating pdn_throt sm ptr has not been allocated.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_throt_get_prev_cause_code
(
  void                               *pdn_throt_sm_ptr,
  ps_iface_net_down_reason_type      *v4_cause_code_ptr,
  ps_iface_net_down_reason_type      *v6_cause_code_ptr
)
{
  boolean                            ret_val          =   TRUE;
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_p   =   NULL;
  ds_3gpp_throt_user_data           *throt_user_data_p =  NULL;
  /*-------------------------------------------------------------------------*/
  pdn_throt_sm_p = (ds_3gpp_pdn_throttle_sm_s *)pdn_throt_sm_ptr;

  if (pdn_throt_sm_p == NULL)
  {  
    DS_3GPP_MSG0_ERROR("PDN throttle SM ptr not allocated");
    ret_val = FALSE;
  }
    
  if (ret_val != FALSE)
  {
    /*----------------------------------------------------------------------------- 
     Get the prev failure cause code from V4 pdn throttle SM 
     ------------------------------------------------------------------------------*/
    if(v4_cause_code_ptr != NULL)
    {
      throt_user_data_p = ds_3gppi_throt_sm_get_user_data(pdn_throt_sm_p->v4_throttle_sm);

      if(throt_user_data_p != NULL)
      {
        *v4_cause_code_ptr = throt_user_data_p->cause_code;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Throt user data ptr is NULL");
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("v4 cause code ptr is NULL");
    }

    /*------------------------------------------------------------------------------ 
     Get the prev failure cause code from V6 pdn throttle SM 
    -------------------------------------------------------------------------------*/
    
    if(v6_cause_code_ptr != NULL)
    {
      throt_user_data_p = ds_3gppi_throt_sm_get_user_data(pdn_throt_sm_p->v6_throttle_sm);

      if(throt_user_data_p != NULL)
      {
        *v6_cause_code_ptr = throt_user_data_p->cause_code;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Throt user data ptr is NULL");
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("v6 casue code ptr is NULL");
    }
  }

  return ret_val;
}

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
)
{
  boolean                            ret_val          = TRUE;
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_p   = NULL;
  /*------------------------------------------------------------------------*/

   /* get the TCM pointer for this pdn */
  pdn_throt_sm_p = ds_3gpp_pdn_throt_sm_get_throttle_sm
                     (apn, globally_blocked, FALSE, 
                        DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, subs_id);

  if (pdn_throt_sm_p == NULL)
  {  
    DS_3GPP_MSG0_MED("PDN throttle SM ptr not allocated");
    ret_val = FALSE;
  }
  else
  {
    *throt_sm_type_p = pdn_throt_sm_p->sm_type;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_FAILURE_CAUSE_CODE

DESCRIPTION
  This function sets the failure cause based on the PDP type
 
PARAMETERS   : 
 
  pdn_throt_sm_ptr					 -   PDN throttle state machine pointer to
 									               set the cause code in the core
									               throttle state machine
  cause_code			           -   Failure cause code to be set
  pdn_pdp_type                       -   PDP type
									  

DEPENDENCIES
  None.

RETURN VALUE
  boolean -  TRUE indicating SUCCESS
             FALSE indicating pdn_throt sm ptr has not been allocated.

SIDE EFFECTS
  None.
============================================================================*/
boolean ds_3gpp_pdn_throt_set_failure_cause_code
(
  void			        *pdn_throt_sm_ptr,
  void           	        *cause_code,
  ds_umts_pdp_type_enum_type     pdn_pdp_type
)
{
  boolean                            ret_val          = TRUE;
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_p   = NULL;
  /*-------------------------------------------------------------------------*/
  pdn_throt_sm_p = (ds_3gpp_pdn_throttle_sm_s *)pdn_throt_sm_ptr;

  if (pdn_throt_sm_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN throttle SM ptr not allocated");
    ret_val = FALSE;
  }
  
  if (ret_val != FALSE)
  {
    /*---------------------------------------------------------------------------- 
      Set the IPV4 core throttling SM cause code 
     ----------------------------------------------------------------------------*/

    if ((pdn_pdp_type == DS_UMTS_PDP_IPV4) || (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
    {
      pdn_throt_sm_p->v4_throttle_sm->user_data.cause_code = 
                                      (ps_iface_net_down_reason_type)cause_code;
    }

   /*---------------------------------------------------------------------------- 
      Set the IPV6 core throttling SM cause code 
    ----------------------------------------------------------------------------*/

    if ((pdn_pdp_type == DS_UMTS_PDP_IPV6) || (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
    {
      pdn_throt_sm_p->v6_throttle_sm->user_data.cause_code = 
                                      (ps_iface_net_down_reason_type)cause_code;
    }
  }

  return ret_val;
}

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
  None

===========================================================================*/

void ds_3gpp_pdn_throt_handle_perm_conn_failure
(
  void                        *pdn_throt_sm_ptr,
  void                        *cause_code,
  ds_umts_pdp_type_enum_type  pdn_pdp_type,
  sys_modem_as_id_e_type      subs_id,
  ds_pdn_context_s             *pdn_context_p
)
{
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_p   = NULL;
  /*-------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_handle_perm_conn_failure");
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p) &&
     DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_context_p) == TRUE )
  {
    DS_3GPP_MSG0_ERROR("Emergency pdn is passed. No throttling needed");
    return;
  }

  pdn_throt_sm_p = (ds_3gpp_pdn_throttle_sm_s *)pdn_throt_sm_ptr;

  if (pdn_throt_sm_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN throttle SM ptr not allocated");
  }
  else 
  {
    if ((pdn_pdp_type == DS_UMTS_PDP_IPV4) || (pdn_pdp_type ==
                                                DS_UMTS_PDP_IPV4V6))
    {
      DS_3GPP_MSG0_HIGH("Setting Infinite Throttling for V4 SM");
      ds_3gppi_throt_sm_set_is_throttled_flag(pdn_throt_sm_p->v4_throttle_sm,
                                              TRUE);
      ds_3gppi_throt_sm_set_throttle_timer_val(pdn_throt_sm_p->v4_throttle_sm,
                                             DS_3GPP_PDN_THROT_INFINITE_THROT);

      ds_3gpp_pdn_throt_update_log_list(pdn_throt_sm_p->
                                             v4_throttle_sm,DS_UMTS_PDP_IPV4);

      ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_p->v4_throttle_sm,
                                       pdn_throt_sm_p->v4_throttle_sm->apn,
                                       DS_UMTS_PDP_IPV4);


    }

    if ((pdn_pdp_type == DS_UMTS_PDP_IPV6) || (pdn_pdp_type ==
                                                DS_UMTS_PDP_IPV4V6))
    {
      DS_3GPP_MSG0_HIGH("Setting Infinite Throttling for V6 SM");
      ds_3gppi_throt_sm_set_is_throttled_flag(pdn_throt_sm_p->v6_throttle_sm,
                                              TRUE);
      ds_3gppi_throt_sm_set_throttle_timer_val(pdn_throt_sm_p->v6_throttle_sm,
                                              DS_3GPP_PDN_THROT_INFINITE_THROT);
     
      ds_3gpp_pdn_throt_update_log_list(pdn_throt_sm_p->
                                           v6_throttle_sm,DS_UMTS_PDP_IPV6);

      ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_p->v6_throttle_sm,
                                       pdn_throt_sm_p->v6_throttle_sm->apn,
                                       DS_UMTS_PDP_IPV6);
  
    }

    ds_3gpp_pdn_throt_set_failure_cause_code(pdn_throt_sm_ptr,
	                                     cause_code,
                                             DS_UMTS_PDP_IPV4V6);
  }
}

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
)
{

  ps_sys_pdn_throttle_info_type    blocked_apn_info;
  sys_plmn_id_s_type               current_plmn_id;
  int                              i = 0;
  uint32                           rem_timer =0;
  uint32                           min_timer = 0;
  ds_3gpp_throt_rat_e_type         rat_index = 0;
  /*---------------------------------------------------------------------------*/

  DS_3GPP_MSG0_MED("ds_3gpp_pdn_throt_lte_disable_throttle_timer_valu Entered");

  memset(&blocked_apn_info,0,sizeof(blocked_apn_info));

  /*--------------------------------------------------------------------- 
    Passing the throttling information to MPPM based on the current PLMN
    --------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn_id, subs_id);

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    rat_index = DS_3GPP_THROT_RAT_LTE;
  } 
  else
  {
    rat_index = DS_3GPP_THROT_RAT_GLOBAL;
  }

  /*------------------------------------------------------------------------
    For current plmn, get throttle info for all apns
   -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
    (&current_plmn_id, &blocked_apn_info, rat_index, subs_id);

  for(i = 0; i < blocked_apn_info.num_apn; i++)
  {
    if (!ds_dsd_apm_is_apn_in_attach_pdn_list
          (subs_id, blocked_apn_info.pdn_throttle_info[i].apn_string))
    {
      continue;
    }

    if (blocked_apn_info.pdn_throttle_info[i].is_ipv4_throttled || 
                  blocked_apn_info.pdn_throttle_info[i].is_ipv6_throttled )
    {
      if (blocked_apn_info.pdn_throttle_info[i].remaining_ipv4_throttled_time < 
          blocked_apn_info.pdn_throttle_info[i].remaining_ipv6_throttled_time)
      {
        rem_timer = 
          blocked_apn_info.pdn_throttle_info[i].remaining_ipv4_throttled_time;
      }
      else
      {
        rem_timer = 
          blocked_apn_info.pdn_throttle_info[i].remaining_ipv6_throttled_time;
      }
    }

    if ((rem_timer <= min_timer) || (min_timer ==0))
    {
      min_timer = rem_timer;
    }
  }
  return min_timer;

} /* ds_3gpp_pdn_throt_lte_disable_throttle_timer_value */

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
  void	                         *pdn_throt_sm_ptr,
  ds_pdn_context_s               *pdn_cntxt_ptr,
  void                           *cause_code,
  ds_umts_pdp_type_enum_type 	  pdn_pdp_type,
  ds_3gpp_throt_rat_e_type        rat_type

)
{
  boolean                           result_val = FALSE;
  sys_modem_as_id_e_type            subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_pdn_throt_ident_info_type throt_ident_info;
  boolean                           apn_globally_blocked = FALSE;
  /*------------------------------------------------------------------------*/
  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntxt_ptr, &subs_id))
  {
    return;
  }

  if (DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402 == 
  	                  ds_3gpp_throt_get_srv_req_throttle_behavior(subs_id))
  {
    DS_3GPP_MSG0_HIGH("Regular throttling config not active, returning");
    return;
  }  

  /*
  For emergency we are throttling 50/51 cause code. For all other cause
  code we will not throttle the pdn
  */
  if(DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntxt_ptr) && 
     (PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED != 
      (ps_iface_net_down_reason_type)cause_code) &&
     (PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED != 
      (ps_iface_net_down_reason_type)cause_code))
  {
    DS_3GPP_MSG1_HIGH("Emergency PDN Request rejected with ps_net_down_reason %d"
                     ", no throttling needed",cause_code);
    return;
  }

  apn_globally_blocked = 
    ds_3gpp_pdn_throt_is_apn_globally_throttled
      (pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
       subs_id);

  ds_3gpp_pdn_throt_fill_ident_info
    (&throt_ident_info, 
     pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
     DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
     subs_id,
     apn_globally_blocked,
     rat_type);

  ds_3gpp_pdn_throt_handle_conn_failure(&throt_ident_info,
                                        pdn_pdp_type, 
                                        cause_code,
                                        pdn_cntxt_ptr);
  
  /*--------------------------------------------------------------------
  Now set the cause code, Making a call to conn_failure would always
  populate the pdn_throttle_sm_ptr if the pdn_throttle_sm_ptr was NULL.
  Get the pdn_throttle_sm_ptr again and set the cause code.
 --------------------------------------------------------------------*/
  if(pdn_throt_sm_ptr == NULL)
  {
    pdn_throt_sm_ptr = 
      ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, FALSE);
  }

  result_val = ds_3gpp_pdn_throt_set_failure_cause_code(
    pdn_throt_sm_ptr, (void *)cause_code, pdn_pdp_type);
  
  if (result_val != TRUE)
  {
    DS_3GPP_MSG0_ERROR(" Failure setting SM cause code");
  }
} /* ds_3gpp_pdn_throt_perform_reg_throttling */

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
)
{
  ds_3gpp_pdn_throt_apn_list_type  *temp_apn_list = NULL;
  ds_dsd_apm_block_list_type        forbidden_apn_list;
  ds_dsd_apm_apn_block_info_type   *temp_pdn_block_info = NULL;
  uint8 i = 0;
/*------------------------------------------------------------------------*/

  memset((void*)&(forbidden_apn_list),0,sizeof(ds_dsd_apm_block_list_type));

  if (num_blocked_apns == 0)
  {
    DS_3GPP_MSG0_HIGH("num_blocked_apns is 0, advertising empty list to APM");
    ds_dsd_apm_forbidden_apn_list_ind (subs_id, &forbidden_apn_list);

    return;
  }

  /*allocate forbidden apn list memory for APM*/
  if( NULL != (forbidden_apn_list.pdn_block_info = 
                (ds_dsd_apm_apn_block_info_type*)modem_mem_alloc(
                 num_blocked_apns*sizeof(ds_dsd_apm_apn_block_info_type),
                 MODEM_MEM_CLIENT_DATA)) )
  {
    memset((void*)forbidden_apn_list.pdn_block_info, 0, 
            sizeof(ds_dsd_apm_apn_block_info_type));
    temp_apn_list = globally_blocked_apn_list;
    temp_pdn_block_info = forbidden_apn_list.pdn_block_info;

    for (i=0; i<num_blocked_apns; i++)
    {
      strlcpy(temp_pdn_block_info->apn_name, 
              (char *)temp_apn_list->apn, 
              DS_UMTS_MAX_APN_STRING_LEN+1);
      temp_pdn_block_info->apn_len = temp_apn_list->len;
      temp_pdn_block_info->ip_type_blocked = 
        DS_DSD_APM_IP_V4_BLOCKED_MASK|DS_DSD_APM_IP_V6_BLOCKED_MASK;
      temp_pdn_block_info->attach_allowed = 
        temp_apn_list->attach_allowed;
      temp_pdn_block_info++;
      temp_apn_list++;        
    }
    forbidden_apn_list.num_apn = num_blocked_apns;
    /*------------------------------------------------------------------
     call the APM API to inform it currently blocked list of APNs.
    ------------------------------------------------------------------*/
    ds_dsd_apm_forbidden_apn_list_ind (subs_id, &forbidden_apn_list);
    modem_mem_free(forbidden_apn_list.pdn_block_info, MODEM_MEM_CLIENT_DATA);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Could not alloc APM forbidden APN list, not sending"
                        "forbidden apn list to APM");
  }
  return;
} /*ds_3gpp_pdn_throt_advertise_globally_blocked_apns_to_apm*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ADVERTISE_GLOBALLY_BLOCKED_APNS_to_CM

DESCRIPTION
  This function sends the currently blocked list of of APNs to CM
  
PARAMETERS   : 
  globally_blocked_apn_list
  num_blocked_apns
 
DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_pdn_throt_advertise_globally_blocked_apns_to_cm 
(
  ds_3gpp_pdn_throt_apn_list_type *globally_blocked_apn_list,
  uint8                            num_blocked_apns,
  sys_modem_as_id_e_type           subs_id
)
{
  cm_blocked_apn_list_s_type       *cm_apn_list = NULL;
  ds_3gpp_pdn_throt_apn_list_type  *temp_apn_list = NULL;
  uint8 i = 0;
/*------------------------------------------------------------------------*/
  /*allocate the CM apn list memory*/
  if( NULL != (cm_apn_list = (cm_blocked_apn_list_s_type*)modem_mem_alloc(
                       sizeof(cm_blocked_apn_list_s_type), 
                       MODEM_MEM_CLIENT_DATA)) )
  {
    memset((void*)cm_apn_list, 0, sizeof(cm_blocked_apn_list_s_type));
    temp_apn_list = globally_blocked_apn_list;
    i = 0;
    while (i < num_blocked_apns)
    {
      cm_apn_list->apn_name[i].valid = TRUE;
      cm_apn_list->apn_name[i].apn_addr_len = temp_apn_list->len;
      strlcpy((char *)cm_apn_list->apn_name[i].address, 
              (char *)temp_apn_list->apn, 
              DS_UMTS_MAX_APN_STRING_LEN+1);
      temp_apn_list++;
      i++;
    }
    cm_apn_list->length = num_blocked_apns;
    cm_apn_list->as_id = subs_id;
    /*------------------------------------------------------------------
     call the CM API to inform it currently blocked list of APNs.
    ------------------------------------------------------------------*/
    cm_call_cmd_blocked_apn_list(NULL, 
                                 NULL, 
                                 ds3g_get_cm_client_id(), 
                                 cm_apn_list);
    DS_3GPP_MSG1_HIGH("Advertised %d blocked APNs to CM", 
                       num_blocked_apns);
    modem_mem_free(cm_apn_list, MODEM_MEM_CLIENT_DATA);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Could not alloc CM blocked APN list, not sending"
                       "blocked apn list to CM");
  }
  return;
} /*ds_3gpp_pdn_throt_advertise_globally_blocked_apns_to_cm*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ADVERTISE_BLOCKED_APNS_CURRENT_PLMN

DESCRIPTION
  This function sends the currently blocked list of of APNs to CM and APM
  
PARAMETERS   : 
  void

DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn
(
  sys_modem_as_id_e_type    subs_id
)
{
  sys_plmn_id_s_type                       current_plmn;
  memset((void*)&(current_plmn),0,sizeof(sys_plmn_id_s_type));
  /*---------------------------------------------------------------------------*/

  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);
  ds_3gpp_pdn_throt_advertise_blocked_apns_with_plmn (subs_id, current_plmn);

  return;
} /* ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ADVERTISE_BLOCKED_APNS_WITH_PLMN

DESCRIPTION
  This function sends the currently blocked list of of APNs to CM and APM
  
PARAMETERS   : 
  void

DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_advertise_blocked_apns_with_plmn
(
  sys_modem_as_id_e_type    subs_id,
  sys_plmn_id_s_type        plmn_id
)
{
  ds_3gpp_pdn_throt_apn_list_type *blocked_apn_list = NULL;
  uint8                            num_blocked_apn = 0;
  /*---------------------------------------------------------------------------*/

  blocked_apn_list = (ds_3gpp_pdn_throt_apn_list_type*)modem_mem_alloc(
                      DS_MAX_APN_NUM*sizeof(ds_3gpp_pdn_throt_apn_list_type),
                      MODEM_MEM_CLIENT_DATA);
             
  if(blocked_apn_list != NULL)
  {
    if(ds_3gpp_pdn_throt_get_blocked_apn_list_on_plmn_id(
       blocked_apn_list,
       &num_blocked_apn, 
       subs_id,
       plmn_id))
    {
      DS_3GPP_MSG1_HIGH("Found %d blocked APNs on current plmn",
                         num_blocked_apn);

      ds_3gpp_pdn_throt_advertise_globally_blocked_apns_to_cm(
        blocked_apn_list,
        num_blocked_apn,
        subs_id);
      /* Advertise globally blocked APNs to APM*/
      ds_3gpp_pdn_throt_advertise_globally_blocked_apns_to_apm(
        blocked_apn_list,
        num_blocked_apn,
        subs_id);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Could not get blocked APNs from APN throt tbl");
    }
    modem_mem_free(blocked_apn_list, MODEM_MEM_CLIENT_DATA);
  }

  ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm(subs_id);

  return;
} /* ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn */

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
)
{
  if(pdn_throttle_sm_ptr != NULL)
  {
    pdn_throttle_sm_ptr->sm_type = sm_type;
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SM_RESET_REL_10_PERM_PLMN_SM

DESCRIPTION
  This function resets the pdn throttling state machine for a release 10
  based throttling sm. 
  
PARAMETERS   : 
  pdn_throttle_sm_ptr - pdn throttling pointer

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
)
{
  if(per_plmn_throt_sm_ptr != NULL)
  {
    ds3gpp_enter_global_crit_section();
    ds_3gpp_pdn_throt_sm_set_sm_type(per_plmn_throt_sm_ptr,
                                     DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED);
    ds_3gpp_throt_sm_reset_rel_10_per_plmn_sm(per_plmn_throt_sm_ptr->v4_throttle_sm);
    ds_3gpp_throt_sm_reset_rel_10_per_plmn_sm(per_plmn_throt_sm_ptr->v6_throttle_sm);
    ds3gpp_leave_global_crit_section();
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CONVERT_RAT_TO_RAT_MASK

DESCRIPTION
    The function converts the rat mask of enum type and returns uint32 ratmask
 
PARAMETERS: 
 
DEPENDENCIES
  None.

RETURN VALUE
  boolean.
 
SIDE EFFECTS

===========================================================================*/

static uint32 ds_3gpp_pdn_throt_convert_rat_to_rat_mask
(
   ds_3gpp_throt_rat_e_type temp_rat_mask
)
{
  uint32 rat_mask = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(temp_rat_mask)
  {
    case DS_3GPP_THROT_RAT_GLOBAL:
      rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
      break;

    case DS_3GPP_THROT_RAT_LTE:
      rat_mask = DS_3GPP_THROT_RAT_MASK_LTE;
      break;

    case DS_3GPP_THROT_RAT_UMTS:
      rat_mask = DS_3GPP_THROT_RAT_MASK_UMTS;
      break;

    default:
       rat_mask = DS_3GPP_THROT_RAT_MASK_NONE;
  }

  DS_3GPP_MSG1_HIGH("Rat_mask is %d",rat_mask);
  return rat_mask;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_NO_RESET_ON_LPM_MODE

DESCRIPTION
    The function checks if throttling should not be reset
    Throttling should not be reset on lpm for certain cause codes of REL10
 
PARAMETERS: 
 
DEPENDENCIES
  None.

RETURN VALUE
  boolean.
 
SIDE EFFECTS

===========================================================================*/

static boolean ds_3gpp_pdn_throt_no_reset_on_lpm_mode
(
   ds_3gpp_pdn_throttle_sm_s *per_plmn_throt_sm_ptr,
   ds_3gppi_throttle_sm_s    *throt_sm_ptr
)
{
  boolean ret_val = FALSE;
  ds_3gpp_throt_user_data *user_data = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(per_plmn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per plmn throt sm ptr is NULL");
    return ret_val;
  }
  /*------------------------------------------------------------------ 
   If clear_t3396_throttling is set, check if the throt SM state is
   set to T3396. get the cause code, If the cause code is set to 26
   dont reset the throttling
             
   Also if SM type is set to PDN_CONN_LIMIT or PDN_REQ_WAIT_TIME
   Do not clean up the throttle sm
  ----------------------------------------------------------------*/

  if((per_plmn_throt_sm_ptr ->sm_type  == DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT) ||
     (per_plmn_throt_sm_ptr->sm_type == DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME))
  {
    ret_val = TRUE;
  }
  else if((per_plmn_throt_sm_ptr->sm_type == DS_3GPP_THROTTLE_TYPE_T3396) &&
          (throt_sm_ptr != NULL))
  {
    user_data = ds_3gppi_throt_sm_get_user_data(throt_sm_ptr);

    if((user_data != NULL) && 
       (user_data->cause_code == PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES))
    {
      ret_val = TRUE;
    }
  }

  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_throt_no_reset_on_lpm_mode returns %d",ret_val);
  return ret_val;
}/*ds_3gpp_pdn_throt_no_reset_on_lpm_mode*/



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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  int                                      plmn_loop_index = 0;
  int                                      apn_loop_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_s               *per_plmn_per_rat_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s          *plmn_throt_sm_info_p = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  ds_3gpp_throt_rat_e_type                 temp_rat_mask = DS_3GPP_THROT_RAT_MIN;
  boolean                                  plmn_globally_blocked = FALSE;
  uint32                                   rat_mask = 0;
  /*---------------------------------------------------------------------------*/
  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }

  for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {
    per_apn_throt_sm_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];

    if(per_apn_throt_sm_ptr != NULL)
    {
      plmn_globally_blocked = per_apn_throt_sm_ptr->globally_blocked;
      /*------------------------------------------------------------------------
        Loop over all the PDN throt SMs (per PLMN), If Throttling
        is set ---> Clear the is_throttled flag, throttled timer value and
        also reset the failure cause code
      --------------------------------------------------------------------------*/
      for(plmn_loop_index = 0; plmn_loop_index < DS_3GPP_MAX_PLMN_SUPPORTED; 
          plmn_loop_index++) 
      {
        if(per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl[plmn_loop_index] != NULL)
        {
          for(temp_rat_mask = DS_3GPP_THROT_RAT_GLOBAL;
              temp_rat_mask < DS_3GPP_THROT_RAT_ILLEGAL;
               temp_rat_mask++)
          {
            plmn_throt_sm_info_p = 
              per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl[plmn_loop_index];

            DS_3GPP_MSG3_MED("plmn_throt_sm_info_p: 0x%x, plmn loop index %d"
                             " table_p: 0x%x",
                            per_apn_throt_sm_ptr, plmn_loop_index,
                            per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl[plmn_loop_index]);

            if (plmn_throt_sm_info_p == NULL) 
            {
              DS_3GPP_MSG2_HIGH("plmn_throt_sm_info_p is NULL for plmn_loop_index: %d, "
                                "temp_rat_mask: %d", plmn_loop_index, temp_rat_mask);
              break;
            }

            per_plmn_per_rat_throt_sm_ptr = 
               &(plmn_throt_sm_info_p->per_rat[temp_rat_mask]);

            if(per_plmn_per_rat_throt_sm_ptr->init == TRUE)
            {
              if(per_plmn_per_rat_throt_sm_ptr->v4_throttle_sm != NULL)
              {
                /*---------------------------------------------------------------------
                  Call clear_throttle_state_function to clear throttling_flag,
                  timer_val, cause_code and failure_count.
               ----------------------------------------------------------------------*/
                DS_3GPP_MSG3_HIGH("Resetting throttling for V4 throttle SM, "
                                  "APN index:%d, PLMN index:%d", apn_loop_index,
                                   plmn_loop_index,0);

                if(ds_3gpp_pdn_throt_no_reset_on_lpm_mode(per_plmn_per_rat_throt_sm_ptr,
                                                           per_plmn_per_rat_throt_sm_ptr->
                                                           v4_throttle_sm))
                {
                  continue;
                }
                /* -----------------------------------------------------------------------
                   Resets t3396 timer entry from local cache and EFS file
                   Its sufficient to clear t3396 only for V4 since for REL10
                   No need to check if rel10 NV is set 
                ---------------------------------------------------------------------*/
                if(per_plmn_per_rat_throt_sm_ptr->sm_type == DS_3GPP_THROTTLE_TYPE_T3396)
                {
                  ds_3gpp_apn_throt_clear_t3396_tmr_entry(
                                            per_plmn_per_rat_throt_sm_ptr->v4_throttle_sm->apn,
                                            per_plmn_per_rat_throt_sm_ptr->v4_throttle_sm->plmn_id,
                                            subs_id);    
                }
              }

              if(per_plmn_per_rat_throt_sm_ptr->v6_throttle_sm != NULL)
              {
              /*---------------------------------------------------------------------
                Call clear_throttle_state_function to clear throttling_flag,
                timer_val, cause_code and failure_count.
              ----------------------------------------------------------------------*/
                DS_3GPP_MSG3_HIGH("Resetting throttling for V6 throttle SM, "
                                  "APN index:%d, PLMN index:%d", apn_loop_index,
                                   plmn_loop_index,0);

                if(ds_3gpp_pdn_throt_no_reset_on_lpm_mode(per_plmn_per_rat_throt_sm_ptr,
                                                          per_plmn_per_rat_throt_sm_ptr->
                                                          v6_throttle_sm))
                {
                  continue;
                }
              }

              rat_mask = ds_3gpp_pdn_throt_convert_rat_to_rat_mask(temp_rat_mask);

              ds_3gpp_pdn_throt_clear_throttle_state(
                        per_apn_throt_sm_ptr->apn,
                        DS_UMTS_PDP_IPV4V6, 
                        subs_id,
                        rat_mask);  
            }
          }//per rat
        } //plmn ptr check
      } //loop over all PDN throt sm pointers for a given PLMN for all APN's
    }// per plmn throt sm ptr check
  } //loop over all PLMNs

  /*-------------------------------------------------------------------------------- 
    Clean up apn reject pdn blocked, The timer will need to be cleared for  permanent
    and temp throttling
    --------------------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_set_apn_reject_pdn_blocked(FALSE,subs_id);
  ds_3gpp_pdn_throt_apn_reject_clr_timer(subs_id);

  /*Clear CC SP throttling*/
  if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE)
  {
    DS_3GPP_MSG0_HIGH(" throttling Reset on SYS_OPRT_MODE_LPM | SYS_OPRT_MODE_OFFLINE");
#ifdef FEATURE_DATA_LTE
    ds_eps_pdn_throt_reset_throttling(subs_id);
    ds_eps_pdn_throt_sm_clear_plmn_blocking(subs_id);
#endif /* FEATURE_DATA_LTE */
    ds_3gpp_pdn_throt_reset_pdn_throttle(subs_id);
  }
}/*ds_3gpp_pdn_throt_clear_throttling_on_lpm*/

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
)
{
  int                                      plmn_loop_index = 0;
  int                                      apn_loop_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_s               *per_plmn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s          *plmn_throt_sm_info_p = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  boolean                                  is_attach_apn = FALSE;
  ds_3gpp_throt_user_data                 *user_data = NULL;
  /*---------------------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }

  for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {
    per_apn_throt_sm_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];

    if(per_apn_throt_sm_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Loop over all the PDN throt SMs (per PLMN), If Infinite Throttling
        is set ---> Clear the is_throttled flag, throttled timer value and
        also reset the failure cause code
      --------------------------------------------------------------------------*/
      for(plmn_loop_index = 0; plmn_loop_index < DS_3GPP_MAX_PLMN_SUPPORTED; 
          plmn_loop_index++) 
      {
        plmn_throt_sm_info_p = per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl
          [plmn_loop_index];

        if (plmn_throt_sm_info_p != NULL)
        {
          per_plmn_throt_sm_ptr = 
            &(plmn_throt_sm_info_p->per_rat[DS_3GPP_THROT_RAT_GLOBAL]);
        }
        else
        {
          per_plmn_throt_sm_ptr = NULL;
        }

        if(per_plmn_throt_sm_ptr != NULL)
        {
          if(per_plmn_throt_sm_ptr->v4_throttle_sm != NULL)
          { 
            /*---------------------------------------------------------------------
              Check if V4 throttling timer is set to infinite throttling
              If set, Call clear_throttle_state_function to clear throttling_flag,
              timer_val, cause_code and failure_count.
              For Release 10; we always clear throttling state for both V4V6
            ----------------------------------------------------------------------*/
              if(ds_3gppi_throt_sm_get_throttle_timer_val(per_plmn_throt_sm_ptr->
                  v4_throttle_sm) == DS_3GPP_PDN_THROT_INFINITE_THROT)
              {
                  DS_3GPP_MSG3_HIGH("Resetting Infite throttling for V4V6 throttle SM, "
                                  "APN index:%d, PLMN index:%d", apn_loop_index,
                                   plmn_loop_index,0);

                /*------------------------------------------------------------------ 
                  If clear_t3396_throttling is set, check if the throt SM state is
                  set to T3396 absent. If not then Infinite throttling need not be
                  cleared
                -----------------------------------------------------------------*/
                user_data = ds_3gppi_throt_sm_get_user_data
                                (per_plmn_throt_sm_ptr->v4_throttle_sm);

                if((clear_t3396_throttling) && 
                   ((per_plmn_throt_sm_ptr->sm_type != DS_3GPP_THROTTLE_TYPE_T3396) || 
                   ((user_data != NULL)  && 
                    (user_data->cause_code != PS_NET_DOWN_REASON_UNKNOWN_APN ))))
                {
                  DS_3GPP_MSG0_MED("Clear t3396 throttling set but dont "
                                   "clear infinite throttling ");
                  continue;
                }

                /*---------------------------------------------------------------- 
                  If NV is set to Global throttling and throttle SM is set as
                  THROTTLE_TYPE_T3396_ABSENT, dont clean up the infinite throttling
                -----------------------------------------------------------------*/
                if((ds_3gpp_nv_manager_get_global_throttling(subs_id)) &&
                   (per_plmn_throt_sm_ptr->sm_type != DS_3GPP_THROTTLE_TYPE_T3396_ABSENT))
                {
                  DS_3GPP_MSG0_HIGH("T3396 absent and global throttling set, "
                                    "Dont clear infintie throttling");
                  continue;
                }		

                if (ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id))
                {
                  ds_3gpp_pdn_throt_sm_reset_rel_10_per_plmn_sm(per_plmn_throt_sm_ptr,
                                                                subs_id);
#ifdef FEATURE_DATA_LTE
                  if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id,
                        (char*)per_apn_throt_sm_ptr->apn))
                  {
                    ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(
                            per_plmn_throt_sm_ptr->v4_throttle_sm->plmn_id, 
                            CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY, 
                            subs_id);
                    ds_3gpp_pdn_cntxt_flush_plmn_from_list(
                       per_plmn_throt_sm_ptr->v4_throttle_sm->plmn_id,
                       subs_id);   
                  }
#endif /* FEATURE_DATA_LTE */
                }
                else
                {
                  if (ds_dsd_apm_is_apn_in_attach_pdn_list
                      (subs_id, (char*)per_apn_throt_sm_ptr->apn) && (is_attach_apn == FALSE))
                  {
                     is_attach_apn = TRUE;
                  }

                  ds_3gpp_pdn_throt_clear_throttle_state(
                    per_apn_throt_sm_ptr->apn,
                    DS_UMTS_PDP_IPV4V6, 
                    subs_id,
                    DS_3GPP_THROT_RAT_MASK_GLOBAL);                                        
                }
              }

          }
        }  // per apn throttle sm ptr
      } //loop over all PDN throt sm pointers for a given PLMN for all APN's
    }// per plmn throt sm ptr check
  } //loop over all PLMNs

  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
#ifdef FEATURE_DATA_LTE  
 if (is_attach_apn &&
     (!ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id)))
 {  
  ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id); 
 }
#endif  

}


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
  byte                      *apn,
  sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  /*------------------------------------------------------------------------*/

  apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_based_tbl_index(apn, subs_id);

  if (apn_index_ptr != NULL)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                        "Setting globally blocked flag for apn %s",
                        (char*)apn);
    apn_index_ptr->globally_blocked = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("apn_index_ptr NULL, not able to set glob throt flag");
  }
  return;
}/*ds_3gpp_pdn_throt_set_global_throttle_flag*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_RESET_PDN_THROTTLE

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

void ds_3gpp_pdn_throt_reset_pdn_throttle
(
  sys_modem_as_id_e_type subs_id
)
{
  int                                     plmn_loop_index = 0;
  int                                     apn_loop_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s          *pdn_throt_sm_info_p = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr;
  uint32                                   rat_mask = 
                                             DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                           /* RATs on which throttling is to be
                                              cleared */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    rat_mask = DS_3GPP_THROT_RAT_MASK_ALL;
  }

  DS_3GPP_MSG0_HIGH("in ds_3gpp_pdn_throt_reset_pdn_throttle");

  for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {

    per_apn_throt_sm_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];

    if((per_apn_throt_sm_ptr != NULL) && (per_apn_throt_sm_ptr->globally_blocked == FALSE))
    {
      /*------------------------------------------------------------------------
        Loop over all the PDN throt SMs (per PLMN), If  Throttling
        is set ---> Clear the is_throttled flag, throttled timer value and
        also reset the failure cause code
      --------------------------------------------------------------------------*/
      for(plmn_loop_index = 0; plmn_loop_index < DS_3GPP_MAX_PLMN_SUPPORTED; 
          plmn_loop_index++) 
      {
        pdn_throt_sm_info_p = per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl
          [plmn_loop_index];

        if(pdn_throt_sm_info_p != NULL)
        {
          /*---------------------------------------------------------------------
            Check if V4 throttling timer is set to infinite throttling
            If set, Call clear_throttle_state_function to clear throttling_flag,
            timer_val, cause_code and failure_count.
            For Release 10; we always clear throttling state for both V4V6
          ----------------------------------------------------------------------*/          
          DS_3GPP_MSG3_HIGH("Resetting Infite throttling for V4V6 throttle SM, "
                            "APN index:%d, PLMN index:%d", apn_loop_index,
                            plmn_loop_index,0);
          ds_3gpp_pdn_throt_clear_throttle_state(per_apn_throt_sm_ptr->apn,
                                                 DS_UMTS_PDP_IPV4V6,
                                                 subs_id, rat_mask);
        }  // per apn throttle sm ptr
      } //loop over all PDN throt sm pointers for a given PLMN for all APN's
    }// per plmn throt sm ptr check
  } //loop over all PLMNs
}

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
  boolean flag
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  /*------------------------------------------------------------------------*/

  apn_index_ptr = (ds_3gpp_pdn_throttle_sm_apn_based_dyn_s*)index_ptr;

  if (apn_index_ptr != NULL)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                            "Setting roaming blocked flag to %d for apn %s",
                            flag,
                            (char*)apn_index_ptr->apn);
    apn_index_ptr->blocked_due_to_roaming = flag;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("apn_index_ptr NULL, not able to set roam throt flag");
  }
  return;
}
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
)
{
  int                                     plmn_loop_index = 0;
  int                                     apn_loop_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_s               *per_plmn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_info_s          *pdn_throt_sm_info_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("in ds_3gpp_pdn_throt_is_any_apn_throttled_for_max_failure");

  for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {
   
    per_apn_throt_sm_ptr  = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];
    
    if(per_apn_throt_sm_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Loop over all the PDN throt SMs (per PLMN), If  Throttling
        is set ---> Clear the is_throttled flag, throttled timer value and
        also reset the failure cause code
      --------------------------------------------------------------------------*/
      for(plmn_loop_index = 0; plmn_loop_index < DS_3GPP_MAX_PLMN_SUPPORTED; 
          plmn_loop_index++) 
      {
        pdn_throt_sm_info_p = per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl
          [plmn_loop_index];
		
        if (pdn_throt_sm_info_p != NULL)
        {
          per_plmn_throt_sm_ptr = 
            &(pdn_throt_sm_info_p->per_rat[DS_3GPP_THROT_RAT_GLOBAL]);
        }
        else
        {
          per_plmn_throt_sm_ptr = NULL;
        }
		
        if(per_plmn_throt_sm_ptr != NULL)
        {
          if((per_plmn_throt_sm_ptr->v4_throttle_sm != NULL)&&
		  	 (per_plmn_throt_sm_ptr->v4_throttle_sm->is_throttled == TRUE)&&
		  	 (per_plmn_throt_sm_ptr->v4_throttle_sm->failure_count == 
		  	                          DS_EPS_PDN_THROT_MAX_FAIL_COUNT))
          { 
            return TRUE;
          }	  
        }  // per apn throttle sm ptr
      } //loop over all PDN throt sm pointers for a given PLMN for all APN's
    }// per plmn throt sm ptr check
  } //loop over all PLMNs
  return FALSE;
}
#endif /* FEATURE_DATA_LTE */

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  /*------------------------------------------------------------------------*/

  apn_index_ptr = (ds_3gpp_pdn_throttle_sm_apn_based_dyn_s*)index_ptr;

  if (apn_index_ptr != NULL)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                            "Setting APN Disabled flag to %d for apn %s",
                            flag,
                            (char*)apn_index_ptr->apn);
    apn_index_ptr->apn_disabled = flag;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("apn_index_ptr NULL, not able to set apn disabled flag");
  }
  return;
} /* ds_3gpp_pdn_throt_set_apn_disabled_flag */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_UI_DATA_DISABLED_FLAG

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  boolean                                   ret_val = FALSE;
  /*------------------------------------------------------------------------*/

  apn_index_ptr = (ds_3gpp_pdn_throttle_sm_apn_based_dyn_s*)index_ptr;

  if (apn_index_ptr != NULL)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                            "Setting UI Data Disabled flag to %d for apn %s",
                            flag,
                            (char*)apn_index_ptr->apn);
    apn_index_ptr->ui_data_disabled = flag;
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("apn_index_ptr NULL, not able to set UI Data Disabled flag");
  }
  return ret_val;
} /* ds_3gpp_pdn_throt_set_ui_data_disabled_flag */

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  /*------------------------------------------------------------------------*/

  apn_index_ptr = (ds_3gpp_pdn_throttle_sm_apn_based_dyn_s*)index_ptr;

  if (apn_index_ptr != NULL)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                            "Getting roaming blocked flag: %d for apn %s",
                            apn_index_ptr->blocked_due_to_roaming,
                            (char*)apn_index_ptr->apn);

    return apn_index_ptr->blocked_due_to_roaming;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("apn_index_ptr NULL, not able to set roam throt flag");
  }
  return FALSE;
} /* ds_3gpp_pdn_throt_get_roaming_throttle_flag */


/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_APN_ON_TIMER_EXP

DESCRIPTION
  This function posts cmd to unblock the APN on expiry of the throttling
  timer
  
PARAMETERS   : 
  throt_sm_ptr      - pointer to core Throt state machine
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/

static void ds_3gpp_pdn_throt_unblock_apn_on_timer_exp
(
  void * throt_sm_p,
  void * cb_data
)
{
  ds_cmd_type                    *cmd_buf = NULL;
  ds_3gpp_pdn_throt_sm_info_type *throt_sm_info_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_buf = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_throt_sm_info_type));
  if( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_buf->hdr.cmd_id = DS_CMD_3GPP_PDN_THROT_UNBLOCK_APN_TIMER_EXP;
  throt_sm_info_ptr = (ds_3gpp_pdn_throt_sm_info_type*)cmd_buf->cmd_payload_ptr;

  throt_sm_info_ptr->subs_id = (int32)cb_data;
  throt_sm_info_ptr->throt_sm_ptr = throt_sm_p;
  ds_put_cmd(cmd_buf);
}/*ds_3gpp_pdn_throt_unblock_apn_on_timer_exp*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR

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
static void ds_3gpp_pdn_throt_timestamp_apn_throt_tmr
(
  void *throt_sm_p,
  void *cb_data
)
{
  ds_cmd_type                    *cmd_buf = NULL;
  ds_3gpp_pdn_throt_sm_info_type *throt_sm_info_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_buf = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_throt_sm_info_type));
  if( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_buf->hdr.cmd_id = DS_CMD_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR;
  throt_sm_info_ptr = (ds_3gpp_pdn_throt_sm_info_type*)cmd_buf->cmd_payload_ptr;

  throt_sm_info_ptr->subs_id = (int)cb_data;
  throt_sm_info_ptr->throt_sm_ptr = throt_sm_p;

  ds_put_cmd(cmd_buf);
}/*ds_3gpp_pdn_throt_timestamp_apn_throt_tmr*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_APN_THROT_T3396_VALIDATE

DESCRIPTION
  This function validates the t3396 throttle state info
  
PARAMETERS   : 
  apn_t3396_ptr      - pointer to t3396 throttle info
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_pdn_throt_apn_throt_t3396_validate
(
  throt_info_t3396_timer_type *apn_t3396_ptr
)
{
  boolean ret_val = FALSE;
  throt_info_t3396_timer_type_dyn_s *apn_t3396_ptr_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(apn_t3396_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Input parameter passed is INVALID",0,0,0);
    return ret_val;
  }

  apn_t3396_ptr_dyn_p = apn_t3396_ptr->throt_info_t3396_timer_type_dyn_p;

  if ((apn_t3396_ptr_dyn_p != NULL) && 
       apn_t3396_ptr_dyn_p->cookie == APN_THROT_T3396_VALID_COOKIE)

  {
    DS_3GPP_MSG3_LOW("Apn T3396 has dyn memory allocated",0,0,0);
    ret_val = TRUE;
  }

  return ret_val;
}


/*===========================================================================
FUNCTION      DS_3GPP_APN_THROTTLE_GET_T3396_TIMER

DESCRIPTION 
  This function returns the throttling failure timer value

PARAMETERS 
  apn_name     - apn name passed by the client
  plmn_id      - plmn id passed by the client
  subs_id      - subscription passed by the client  

RETURN VALUE 
  PDN throttle Failure Timer value in msec

SIDE EFFECTS 
  None
===========================================================================*/

uint32 ds_3gpp_apn_throt_get_t3396_timer 
(
  byte                     *apn_name,
  sys_plmn_id_s_type        plmn_id,
  uint8                    *index,
  sys_modem_as_id_e_type    subs_id
)
{
  uint8                              i = 0;
  uint32                             timer_val = 0;
  ds_3gpp_pdn_throt_rel10_info_type *rel10_info_ptr = NULL;
/*---------------------------------------------------------------------------*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return 0;
  }

  for(i= 0; i<DS_MAX_APN_PLMN_SUPPORTED; i++ )
  {
    if((ds_3gpp_pdn_throt_apn_throt_t3396_validate(
       &(rel10_info_ptr->t3396_timer_tbl[i]))) &&
       (strcasecmp((char*)(apn_name), 
                   (char*)(rel10_info_ptr->t3396_timer_tbl[i].
                           throt_info_t3396_timer_type_dyn_p->apn_name)) == 0) &&
       (memcmp((void*)(&plmn_id),
               (void *)(&(rel10_info_ptr->t3396_timer_tbl[i].
                 throt_info_t3396_timer_type_dyn_p->plmn_id)),
               sizeof(sys_plmn_id_s_type)) == 0))
    {
      timer_val = rel10_info_ptr->t3396_timer_tbl[i].
                    throt_info_t3396_timer_type_dyn_p->timer_val;
      break;
    }
  }
  *index = i;
  return timer_val;
} /* ds_3gpp_apn_throt_get_t3396_timer() */

/*===========================================================================
FUNCTION DS_3GPP_APN_THROT_WRITE_TO_EFS

DESCRIPTION
  This function writes the data passes to it at the specified offset in the
  APN THROTTLE efs throttle param file.

PARAMETERS
  *write_data      - write data buffer
  write_offset     - offset in the file where the data is to be written.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_apn_throt_write_to_efs
(
  ds_3gpp_pdn_throt_efs_data_buf_type  *write_data,
  uint32                                write_offset,
  sys_modem_as_id_e_type                subs_id
)
{
  int32                               fs_handle;
  boolean                             ret_val = TRUE;
  ds_3gpp_pdn_throt_rel10_info_type  *rel10_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------
    Open the file for write 
  -------------------------------------------------------------------*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DATA_ERR_FATAL("rel10_info_ptr is null");
    return FALSE;
  }

  DS_3GPP_MSG0_HIGH("In ds_3gpp_apn_throt_write_to_efs");
  fs_handle = mcfg_fopen( rel10_info_ptr->ds_3gpp_apn_throt_file_path,
                          MCFG_FS_O_RDWR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          (mcfg_fs_sub_id_e_type)subs_id );
  if ( fs_handle == -1)
  {
    DS_3GPP_MSG2_ERROR("efs_open failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  do
  {
    if( mcfg_lseek(fs_handle,
                  write_offset,
                   SEEK_SET,
                   MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG2_ERROR( "efs_lseek failed for %d errno=%d",
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      ret_val = FALSE;
      break;
    }

    if( mcfg_fwrite(fs_handle,
                    write_data->data_ptr,
                    write_data->data_len,
                    MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG2_ERROR("efs_write failed for %d errno = %d",
                          fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      ret_val = FALSE;
      break;
    }
  } while (0); 

  /*-------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------*/
  if(mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1)
  {
    DS_3GPP_MSG2_ERROR("efs_close failed for %d errno = %d", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    ret_val = FALSE;
  }

  return ret_val;
} /* ds_3gpp_apn_throt_write_to_efs */

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
)
{
  ds_3gpp_pdn_throt_efs_data_buf_type efs_write_data;
  uint8                               efs_write_index = 0;
  ds_3gpp_pdn_throt_rel10_info_type  *rel10_info_ptr = NULL;
  ds_3gppi_throttle_sm_s*             throt_sm_ptr = 
                                       (ds_3gppi_throttle_sm_s*)throt_sm_p;
  ds_3gpp_throt_user_data            *throt_user_data_p = NULL;
  /*------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_timestamp_apn_throt_tmr_cmd_hdlr");

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }

  if(throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("throt_sm_ptr is null");
    return; 
  }
  (void)ds_3gpp_apn_throt_get_t3396_timer(throt_sm_ptr->apn, 
                                          throt_sm_ptr->plmn_id, 
                                          &efs_write_index,
                                          subs_id);

  if (efs_write_index >= DS_MAX_APN_PLMN_SUPPORTED)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Entry not found for APN: %s",
                            (char*)throt_sm_ptr->apn);
    return;
  }


  if(!ds_3gpp_pdn_throt_apn_throt_t3396_validate(
     &(rel10_info_ptr->t3396_timer_tbl[efs_write_index])))
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Invalid Apn Table Entry: %s",
                           (char*)throt_sm_ptr->apn);
    return;
  }


  efs_write_data.data_ptr = rel10_info_ptr->t3396_timer_tbl[efs_write_index].
                              throt_info_t3396_timer_type_dyn_p;
  efs_write_data.data_len = sizeof(throt_info_t3396_timer_type_dyn_s);


  /*------------------------------------------------------------------------------ 
   Get the prev failure cause code
  -------------------------------------------------------------------------------*/
  throt_user_data_p = ds_3gppi_throt_sm_get_user_data(throt_sm_ptr);

  if((throt_user_data_p != NULL) && 
     (throt_user_data_p->cause_code == PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES))
  {
    (void)ds_3gpp_apn_throt_write_to_efs(
       &efs_write_data,
            sizeof(throt_info_t3396_timer_type_dyn_s)*efs_write_index +
            sizeof(nv_ehrpd_imsi_type), 
            subs_id);
    /* Calculate sampling timer for valid t3396 running timers and run it in
    background */
    ds_3gpp_pdn_throt_start_sampling_timer(TRUE, subs_id);
  }

}/*ds_3gpp_pdn_throt_timestamp_apn_throt_tmr_cmd_hdlr*/

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
  pdn_context_ptr               pdn_cntxt_ptr
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_perform_apn_throttling
(
  void                                *pdn_throt_sm_p,
  byte                                *apn,
  void                                *cause_code,
  ds_umts_pdp_type_enum_type           pdn_pdp_type,
  sys_modem_as_id_e_type               subs_id,
  boolean                              is_global,
  ds_pdn_context_s                    *pdn_cntxt_ptr
)
{
  boolean                            result_val = FALSE;
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
  /*------------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_perform_apn_throttling");
  if (pdn_throt_sm_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("pdn_throt_sm_p is NULL");
    return;
  }

  pdn_throt_sm_ptr = (ds_3gpp_pdn_throttle_sm_s*)pdn_throt_sm_p;

  strlcpy((char *)pdn_throt_sm_ptr->v4_throttle_sm->apn, 
          (char *)apn, 
          DS_UMTS_MAX_APN_STRING_LEN + 1);

  if (is_global)
  {
    /*Set the globally_blocked flag to TRUE for the APN*/
     ds_3gpp_pdn_throt_set_global_throttle_flag(apn, subs_id);
  }

  ds_3gpp_pdn_throt_fill_ident_info
  (
     &throt_ident_info, 
     apn, 
     DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
     subs_id,
     is_global,
     DS_3GPP_THROT_RAT_GLOBAL
  );

  ds_3gpp_pdn_throt_handle_conn_failure
  (
    &throt_ident_info,
    pdn_pdp_type, 
    cause_code,
    pdn_cntxt_ptr
  );
 
  /*--------------------------------------------------------------------
   Now set the cause code, Making a call to conn_failure would always
   populate the pdn_throttle_sm_ptr if the pdn_throttle_sm_ptr was NULL.
   Get the pdn_throttle_sm_ptr again and set the cause code.
  --------------------------------------------------------------------*/
  result_val = ds_3gpp_pdn_throt_set_failure_cause_code(
       pdn_throt_sm_ptr, (void*)cause_code, pdn_pdp_type);
  
  if (result_val != TRUE)
  {
    DS_3GPP_MSG0_ERROR(" Failure setting SM cause code");
  }

  return;
}/*ds_3gpp_pdn_throt_perform_apn_throttling*/

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  
  /*------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_perform_roaming_throttling");

  apn_index_ptr = 
    ds_3gpp_pdn_throt_sm_get_apn_index_ptr(apn, TRUE, subs_id);

  /*Set the roaming_blocked flag to TRUE for the APN*/
  ds_3gpp_pdn_throt_set_roaming_throttle_flag(apn_index_ptr, TRUE);

  return;
} /* ds_3gpp_pdn_throt_perform_roaming_throttling */

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  
  /*------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_perform_apn_disabled_throttling");

  apn_index_ptr = 
    ds_3gpp_pdn_throt_sm_get_apn_index_ptr(apn, TRUE, subs_id);

  /*Set the apn_disabled flag to TRUE*/
  ds_3gpp_pdn_throt_set_apn_disabled_flag(apn_index_ptr, TRUE);

  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
#ifdef FEATURE_DATA_LTE
  ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable(subs_id);
#endif /* FEATURE_DATA_LTE */
  return;
} /* ds_3gpp_pdn_throt_perform_apn_disabled_throttling */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_APN_DISABLE_POWERUP_THROT

DESCRIPTION
  This function performs apn disable specifc throttling during power up

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
)
{ 
 ds_profile_info_type                  prof_info;
 byte	                               apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
 uint8	                               profile_nums_arr[DS_UMTS_MAX_PDP_PROFILE_NUM];
 uint8	                               profile_count = 0;  
 uint8	                               profile_index = 0;
 boolean	                       apn_disable_flag = FALSE;
  /*------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_apn_disable_powerup_throttle");

  profile_count = ds_3gpp_profile_cache_get_valid_profile_numbers(
                                            dsumts_subs_mgr_get_subs_id(subs_id),
                                            profile_nums_arr, 
                                            DS_UMTS_MAX_PDP_PROFILE_NUM);
  for(profile_index = 0; profile_index<profile_count; profile_index++)
  {    
    memset(&prof_info, 0, sizeof(ds_profile_info_type));
    memset(apn_name, 0, DS_UMTS_MAX_APN_STRING_LEN+1);
    apn_disable_flag = FALSE;
    prof_info.buf = &apn_disable_flag;
    prof_info.len = sizeof(apn_disable_flag);

    if(ds_3gpp_pdn_throt_get_profile_param_info(
                             profile_nums_arr[profile_index],
                             DS_PROFILE_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG,
                             &prof_info) == FALSE)
    {
      DS_3GPP_MSG1_ERROR("Unable to Extract the APN diable flag or flag set to FALSE for Profile: %d",
                           profile_nums_arr[profile_index]);
      continue;
    }
    else if(apn_disable_flag)
    {    
      memset(&prof_info, 0, sizeof(ds_profile_info_type));
      prof_info.buf = apn_name;
      prof_info.len = DS_UMTS_MAX_APN_STRING_LEN+1;

      if(ds_3gpp_pdn_throt_get_profile_param_info(
                             profile_nums_arr[profile_index],
                             DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                             &prof_info) == FALSE)
      {
        DS_3GPP_MSG1_ERROR("Unable to Extract the APN Name of Profile: %d",
                           profile_nums_arr[profile_index]);
      }
      else
      {
        ds_3gpp_pdn_throt_perform_apn_disabled_throttling(apn_name, subs_id);
      }
    }	
  }
  return;
} /* ds_3gpp_pdn_throt_apn_disable_powerup_throttle */

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
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  boolean                                  data_disabled = FALSE;
  /*------------------------------------------------------------------------*/
  DS_3GPP_MSG0_MED("In ds_3gpp_pdn_throt_perform_ui_data_disabled_throttling");

  apn_index_ptr = 
    ds_3gpp_pdn_throt_sm_get_apn_index_ptr(apn, TRUE, subs_id);

  /*Set the ui_data_disabled flag to TRUE*/
  data_disabled = 
    ds_3gpp_pdn_throt_set_ui_data_disabled_flag(apn_index_ptr, TRUE);

  return data_disabled;
    
} /* ds_3gpp_pdn_throt_perform_ui_data_disabled_throttling */


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
)
{
  uint8                                    apn_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr;
  /*-------------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }

  if (is_attach_apn_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("is_attach_apn_ptr is NULL !");
    return;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

    if (unblock_all_apns == FALSE)
    {
      if (apn != NULL)
      {
        if (strcasecmp((char*)apn_index_ptr->apn, (char*)apn) != 0)
        {
          continue;
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("APN ptr is NULL !");
        return;
      }
    }

    if( (NULL != apn_index_ptr) && 
        (TRUE == apn_index_ptr->valid) && 
        (TRUE == apn_index_ptr->blocked_due_to_roaming) )
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Clearing Roaming Throttling"
                              " for APN: %s", (char*)apn_index_ptr->apn);

      if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)apn_index_ptr->apn) &&
          (*is_attach_apn_ptr == FALSE))
      {
        *is_attach_apn_ptr = TRUE;
      }

      /* Clear the PDN throttle information for this APN */
      ds_3gpp_pdn_throt_set_roaming_throttle_flag(apn_index_ptr, FALSE);

      ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed
        (apn_index_ptr, subs_id);
    }
  }
  return;
} /* ds_3gpp_pdn_throt_unblock_roaming_throttling */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UNBLOCK_APN_DISABLED_THROTTLING

DESCRIPTION
  This function unblocks PDN throttling because APN was re-enabled.
  
PARAMETERS
  1. APN name
  2. Subscription Id
  3. Boolean Pointer set to True if the attach apn is unblocked
 
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
)
{
  uint8                                    apn_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr;
  boolean                                  is_attach_apn = FALSE;
  /*-------------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
  }

  else
  {
    for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
    {
      apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

      if (apn != NULL)
      {
        if (strcasecmp((char*)apn_index_ptr->apn, (char*)apn) != 0)
        {
          continue;
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("APN ptr is NULL !");
        break;
      }

      if( (NULL != apn_index_ptr) && 
          (TRUE == apn_index_ptr->valid) && 
          (TRUE == apn_index_ptr->apn_disabled) )
      {
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Clearing APN Disabled flag"
                                " for APN: %s", (char*)apn_index_ptr->apn);

        if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)apn) && 
            (is_attach_apn == FALSE))
        {
          is_attach_apn = TRUE;
        }

        /* Clear the PDN throttle information for this APN */
        ds_3gpp_pdn_throt_set_apn_disabled_flag(apn_index_ptr, FALSE);

        ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed
          (apn_index_ptr, subs_id);
      }
    }
  }

  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
#ifdef FEATURE_DATA_LTE 
  if(is_attach_apn)
  {
  ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
  }
#endif /* FEATURE_DATA_LTE */
  return;
} /* ds_3gpp_pdn_throt_unblock_apn_disabled_throttling */

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
)
{
  uint8                                    apn_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr;
  /*-------------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
  }

  else if (apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("APN ptr is NULL !");
  }
  else
  {
    for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
    {
      apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

      if (strcasecmp((char*)apn_index_ptr->apn, (char*)apn) != 0)
      {
        continue;
      }

      if( (NULL != apn_index_ptr) && 
          (TRUE == apn_index_ptr->valid) && 
          (TRUE == apn_index_ptr->ui_data_disabled) )
      {
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Clearing APN Disabled flag"
                                " for APN: %s", (char*)apn_index_ptr->apn);

        /* Clear the PDN throttle information for this APN */
        if (ds_3gpp_pdn_throt_set_ui_data_disabled_flag(apn_index_ptr, FALSE) 
              == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Failed to clear UI Data disabled flag");
        }

        ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed
          (apn_index_ptr, subs_id);
      }
    }
  }

  return;
} /* ds_3gpp_pdn_throt_unblock_ui_data_disabled_throttling */


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
)
{
  ds_3gppi_throttle_sm_s     *throt_sm_ptr = 
                                (ds_3gppi_throttle_sm_s*)throt_sm_p;

  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr;
  boolean                    is_attach_apn = FALSE;   
  sys_plmn_id_s_type         unblock_plmn_id;
  /*-------------------------------------------------------------------*/

  if(throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Throt sm ptr is NULL");
    return;
  }

  memset(&unblock_plmn_id,0,sizeof(sys_plmn_id_s_type));

  if (throt_sm_ptr != NULL  &&
      ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)throt_sm_ptr->apn))
  {
    is_attach_apn = TRUE;
  }

  /* Resets t3396 timer entry from local cache and EFS file*/
  ds_3gpp_apn_throt_clear_t3396_tmr_entry(throt_sm_ptr->apn,
                                           throt_sm_ptr->plmn_id
                                           ,subs_id);

  /* Also clear the t3396 timer because timer could be running
     if this was called because of USIM removal*/
  rex_clr_timer(&(throt_sm_ptr->throttle_timer));

  /* Clear the PDN throttle information for this APN */
  if (ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id))
  {
    memscpy(&unblock_plmn_id,
                sizeof(sys_plmn_id_s_type),
                &(throt_sm_ptr->plmn_id),
                sizeof(sys_plmn_id_s_type));

    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn(throt_sm_ptr->plmn_id,
                                                                       throt_sm_ptr->apn,
                                                                       subs_id); 
    if (pdn_throt_sm_ptr != NULL)
    {
      ds_3gpp_pdn_throt_sm_reset_rel_10_per_plmn_sm(pdn_throt_sm_ptr,subs_id);
    }
  /* Send updated blocked APN list to CM and APM */
    ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm(subs_id);
    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);

#ifdef FEATURE_DATA_LTE
    if ((ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)throt_sm_ptr->apn)) &&
        (ds_3gpp_pdn_cntxt_is_plmn_blocked(throt_sm_ptr->plmn_id,
                                           subs_id)) 
        && (ds_3gpp_pdn_cntxt_check_if_attach_profile_available(subs_id) 
                                    == DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL)
       )
    {
        ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(
                       throt_sm_ptr->plmn_id, CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY, 
                       subs_id);
        ds_3gpp_pdn_cntxt_flush_plmn_from_list(
                       throt_sm_ptr->plmn_id,
                       subs_id); 
    }
#endif /* FEATURE_DATA_LTE */   
  }
  else
  {
    ds_3gpp_pdn_throt_clear_throttle_state(throt_sm_ptr->apn,
                                           DS_UMTS_PDP_IPV4V6,
                                          subs_id,
                                          DS_3GPP_THROT_RAT_MASK_GLOBAL);  
  /* Send updated blocked APN list to CM and APM */
    ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm(subs_id);
    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
  }

#ifdef FEATURE_DATA_LTE
#ifdef FEATURE_DATA_REL10
   if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id) && is_attach_apn)
   {
     if(ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id))
     {
       ds_3gpp_pdn_cntxt_flush_plmn_from_list(unblock_plmn_id,subs_id);
     }
     else
     {
       ds_3gpp_pdn_cntxt_flush_blocked_plmn_list(subs_id);
     }
   }
#endif /*FEATURE_DATA_REL10*/

   /*---------------------------------------------------------------
    Unblock all PLMN if any PLMN was blocked due to cc_sp_throttlling
   -----------------------------------------------------------------*/
   if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE)&&
       (ds_eps_pdn_throt_is_any_lte_plmn_blocked(subs_id)== TRUE))
   {   	
     ds_eps_pdn_throt_sm_clear_plmn_blocking(subs_id);	  
   }
#endif /* FEATURE_DATA_LTE */

  return;
}/*ds_3gpp_pdn_throt_unblock_apn_timer*/

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
  void                       *throt_sm_p,
  sys_modem_as_id_e_type      subs_id
)
{
  ds_3gpp_pdn_throt_unblock_apn_timer(throt_sm_p,subs_id);
  return;
}/*ds_3gpp_pdn_throt_unblock_apn_timer_exp_cmd_hdlr*/

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
)
{
  uint8                                    apn_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr;
  sys_plmn_id_s_type                       current_plmn;


  memset((void*)&(current_plmn),0,sizeof(sys_plmn_id_s_type));

  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);


  /*-------------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

    if( (NULL != apn_index_ptr) && 
        (TRUE == apn_index_ptr->valid))
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Clearing Throttle"
                              " state for APN: %s",
                              (char*)apn_index_ptr->apn);
      /* Resets t3396 timer entry from local cache and EFS file*/
      if (apn_index_ptr->globally_blocked)
      {
        ds_3gpp_apn_throt_clear_t3396_tmr_entry(apn_index_ptr->apn,
                                                global_plmn_id,
                                                subs_id);
      }
      else
      {
        ds_3gpp_apn_throt_clear_t3396_tmr_entry(apn_index_ptr->apn,
                                                current_plmn,
                                                subs_id);
      }
      /* Clear the PDN throttle information for this APN */
       ds_3gpp_pdn_throt_clear_throttle_state(apn_index_ptr->apn,
                                              DS_UMTS_PDP_IPV4V6,
                                              subs_id,
                                              DS_3GPP_THROT_RAT_MASK_GLOBAL); 
    }
  }
  return;
}
/*===========================================================================
FUNCTION DS_3GPP_APN_THROT_RESET_APN_THROT_EFS

DESCRIPTION
  This function resets all the params in the apn throt efs.

PARAMETERS
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_apn_throt_reset_apn_throt_efs
(
  sys_modem_as_id_e_type subs_id
)
{
  int32                               fs_handle;
  boolean                             ret_val = FALSE;
  nv_ehrpd_imsi_type                  temp_imsi_data;
  ds_3gpp_pdn_throt_rel10_info_type  *rel10_info_ptr = NULL;
  throt_info_t3396_timer_type        *temp_t3396_timer_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DATA_ERR_FATAL("rel10_info_ptr is null");
    return FALSE;
  }
 
  /*-------------------------------------------------------------------
    Open the file for write 
  -------------------------------------------------------------------*/
  fs_handle = mcfg_fopen( rel10_info_ptr->ds_3gpp_apn_throt_file_path,
                          MCFG_FS_O_RDWR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          (mcfg_fs_sub_id_e_type)subs_id );
  if ( fs_handle == -1)
  {
    DS_3GPP_MSG2_ERROR("efs_open failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  if( mcfg_lseek(fs_handle,
                0,
                 SEEK_SET,
                 MCFG_FS_TYPE_EFS) == -1 )
  {
    DATA_MSG2_ERROR( "efs_lseek failed for %d errno=%d",
                     fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS) );
    return FALSE;
  }

  /* Reset IMSI Data in EFS */
  memset((void*)&temp_imsi_data, 0, 
         sizeof(nv_ehrpd_imsi_type));
 
  if( mcfg_fwrite(fs_handle,
                 (void*)&temp_imsi_data, 
                  sizeof(nv_ehrpd_imsi_type),
                  MCFG_FS_TYPE_EFS) == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_write failed for %d errno = %d",
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    ret_val = FALSE;
  } /* File pos updated with sizeof(nv_ehrpd_imsi_type) */

  /* Reset APN block in EFS */
  temp_t3396_timer_info = modem_mem_alloc((sizeof(throt_info_t3396_timer_type)*
                                           DS_MAX_APN_PLMN_SUPPORTED),
  	                                   MODEM_MEM_CLIENT_DATA);
  if (NULL != temp_t3396_timer_info)
  {
    memset(temp_t3396_timer_info, 0, 
           (DS_MAX_APN_PLMN_SUPPORTED*sizeof(throt_info_t3396_timer_type)));
 
    if ( mcfg_fwrite(fs_handle, 
                     (void*)temp_t3396_timer_info, 
                     (DS_MAX_APN_PLMN_SUPPORTED*sizeof(throt_info_t3396_timer_type)),
                      MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG2_ERROR("efs_write failed for %d errno = %d",
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      ret_val = FALSE;
    }
    
    modem_mem_free(temp_t3396_timer_info, MODEM_MEM_CLIENT_DATA);
  }

  if( mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_close failed for %d errno = %d", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    ret_val = FALSE;
  }
  
  return ret_val;
}

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
  byte                     *apn,
  sys_plmn_id_s_type        plmn_id,
  sys_modem_as_id_e_type    subs_id
)
{
  uint8     index = DS_MAX_APN_PLMN_SUPPORTED;
  ds_3gpp_pdn_throt_efs_data_buf_type  efs_write_data;
  ds_3gpp_pdn_throt_rel10_info_type   *rel10_info_ptr = NULL;
/*---------------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("In ds_3gpp_apn_throt_clear_t3396_tmr_entry");

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }

  (void)ds_3gpp_apn_throt_get_t3396_timer (apn,plmn_id, &index, subs_id);

  if (index >= DS_MAX_APN_PLMN_SUPPORTED)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Entry not found for APN: %s",
                            (char*)apn);
    return;
  }

   if(!ds_3gpp_pdn_throt_apn_throt_t3396_validate(
              &(rel10_info_ptr->t3396_timer_tbl[index])))
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Invalid Apn Table Entry: %s",
                            (char*)apn);
    return;
  }

  memset((void*)(rel10_info_ptr->t3396_timer_tbl[index].
         throt_info_t3396_timer_type_dyn_p),
         0,
         sizeof(throt_info_t3396_timer_type_dyn_s));

  efs_write_data.data_ptr = rel10_info_ptr->t3396_timer_tbl[index].
                                throt_info_t3396_timer_type_dyn_p;
  efs_write_data.data_len = 
           sizeof(throt_info_t3396_timer_type_dyn_s);

  (void)ds_3gpp_apn_throt_write_to_efs(
     &efs_write_data,
     sizeof(throt_info_t3396_timer_type_dyn_s)*index +
     sizeof(nv_ehrpd_imsi_type),
     subs_id);


  if(rel10_info_ptr->t3396_timer_tbl[index].
     throt_info_t3396_timer_type_dyn_p != NULL)
  {
    DS_3GPP_MSG3_MED("Freeing dyn allocated apn throttle t3396 timer",0,0,0);
    modem_mem_free(rel10_info_ptr->t3396_timer_tbl[index].
      throt_info_t3396_timer_type_dyn_p,MODEM_MEM_CLIENT_DATA);
    rel10_info_ptr->t3396_timer_tbl[index].throt_info_t3396_timer_type_dyn_p = NULL;
  }

  return;
}/*ds_3gpp_apn_throt_clear_t3396_tmr_entry*/

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
)
{
  uint8                                 i = 0;
  uint8  first_free_index = DS_MAX_APN_PLMN_SUPPORTED;
  uint8                                 efs_write_index = 0;
  boolean                               ret_val = FALSE;
  boolean t3396_validation = FALSE;
  ds_3gpp_pdn_throt_efs_data_buf_type   efs_write_data;
  ds_3gpp_pdn_throt_rel10_info_type    *rel10_info_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_s            *pdn_throt_sm_ptr = NULL;

/*---------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_apn_throt_set_t3396_timer");

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return FALSE;
  }

  for(i= 0; i<DS_MAX_APN_PLMN_SUPPORTED; i++)
  {
    t3396_validation = ds_3gpp_pdn_throt_apn_throt_t3396_validate(
                         &rel10_info_ptr->t3396_timer_tbl[i]);
    if((!t3396_validation) && 
        (first_free_index == DS_MAX_APN_PLMN_SUPPORTED) )
    {
      first_free_index = i;
    }
    else if( t3396_validation &&
             (strcasecmp
                ((char*)(apn),
                 (char*)rel10_info_ptr->t3396_timer_tbl[i].
                 throt_info_t3396_timer_type_dyn_p->apn_name) == 0) &&
              (memcmp
                 ((void*)(&plmn_id),
                 (void *)(&(rel10_info_ptr->t3396_timer_tbl[i].
                 throt_info_t3396_timer_type_dyn_p->plmn_id)),
                 sizeof(sys_plmn_id_s_type)) == 0))
    {
      DS_3GPP_MSG3_HIGH("Found match at index %d, saving timer_val 0x%x",
                         i, timer_val, 0);
      rel10_info_ptr->t3396_timer_tbl[i].
        throt_info_t3396_timer_type_dyn_p->valid = TRUE;
      rel10_info_ptr->t3396_timer_tbl[i].
        throt_info_t3396_timer_type_dyn_p->timer_val = timer_val;
      efs_write_index = i;
      ret_val = TRUE;
      goto EFS_WRITE;
    }
  }
  if(first_free_index == DS_MAX_APN_PLMN_SUPPORTED) 
  {
    ret_val = FALSE;
    DATA_ERR_FATAL("No free space in t3396 tbl for APN"); 
  }
  else
  {

        /*Allocate Memory */
    if (rel10_info_ptr->t3396_timer_tbl[first_free_index].
        throt_info_t3396_timer_type_dyn_p == NULL)
    {
      rel10_info_ptr->t3396_timer_tbl[first_free_index].
        throt_info_t3396_timer_type_dyn_p = 
        (throt_info_t3396_timer_type_dyn_s *)modem_mem_alloc
        (sizeof(throt_info_t3396_timer_type_dyn_s),MODEM_MEM_CLIENT_DATA);
    }
    rel10_info_ptr->t3396_timer_tbl[first_free_index].
      throt_info_t3396_timer_type_dyn_p->cookie = APN_THROT_T3396_VALID_COOKIE;
    strlcpy((char*)rel10_info_ptr->t3396_timer_tbl[first_free_index].
            throt_info_t3396_timer_type_dyn_p->apn_name, 
            (char*)apn, DS_UMTS_MAX_APN_STRING_LEN+1);
    memscpy((void*)(&(rel10_info_ptr->t3396_timer_tbl[first_free_index].
            throt_info_t3396_timer_type_dyn_p->plmn_id)),
            sizeof(sys_plmn_id_s_type),
            (void*)(&plmn_id),
            sizeof(sys_plmn_id_s_type));

    rel10_info_ptr->t3396_timer_tbl[first_free_index].
      throt_info_t3396_timer_type_dyn_p->valid = TRUE;
    rel10_info_ptr->t3396_timer_tbl[first_free_index].
      throt_info_t3396_timer_type_dyn_p->timer_val = timer_val;
    efs_write_index = first_free_index;
    ret_val = TRUE;
  }

EFS_WRITE:

  pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn(plmn_id,
                                                                    apn,
                                                                    subs_id); 

  if( pdn_throt_sm_ptr != NULL)
  {
    ds_3gpp_pdn_throt_sm_set_sm_type(pdn_throt_sm_ptr,
                                     DS_3GPP_THROTTLE_TYPE_T3396);
  }

  if(ret_val == TRUE &&
     (down_reason == PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES))
  {
    efs_write_data.data_ptr = 
      rel10_info_ptr->t3396_timer_tbl[efs_write_index].
        throt_info_t3396_timer_type_dyn_p;
    efs_write_data.data_len = 
      sizeof(throt_info_t3396_timer_type_dyn_s);

    (void)ds_3gpp_apn_throt_write_to_efs(
       &efs_write_data,
       sizeof(throt_info_t3396_timer_type_dyn_s) * efs_write_index +
       sizeof(nv_ehrpd_imsi_type), subs_id);
  }
  return ret_val;
}/*ds_3gpp_apn_throt_set_t3396_timer()*/

/*===========================================================================
FUNCTION      DS_3GPP_APN_THROT_T3396_TIMER_TBL_INIT

DESCRIPTION   This function initiatizes the PDN throttling t3396 timer table

PARAMETERS 
void
 
RETURN VALUE 
void 

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_apn_throt_tbl_init
(
  int subs_id
)
{
  uint8                                 i = 0;
  ds_3gpp_pdn_throt_rel10_info_type    *rel10_info_ptr = NULL;
/*-----------------------------------------------------------------------*/
  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null !");
    return;
  }

  for( i= 0; i<DS_MAX_APN_PLMN_SUPPORTED; i++ )
  {
     rel10_info_ptr->t3396_timer_tbl[i].
       throt_info_t3396_timer_type_dyn_p = NULL;
  }
  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_VALIDATE_MIN_FAILURE_COUNT

DESCRIPTION
  This function ensures that the throttle failure count is atleast
  equal to the minimum throttle failure count passed
  

PARAMETERS   : 
  ds_pdn_context_s           -  Rejected PDN cntxt pointer
  pdn_pdp_type               -  PDP type
  min_throt_failure_cnt      -  The min failure count of V4/V6 core
                                throttle SM
 
 
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
)
{
  uint8                              fail_cnt = 0;
  ds_3gpp_pdn_throttle_sm_s*         pdn_throt_sm_ptr = NULL;
  boolean                            apn_globally_blocked = FALSE;
  sys_modem_as_id_e_type             subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
/*------------------------------------------------------------------------*/

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntxt_ptr, &subs_id))
  {
    return;
  }

  apn_globally_blocked = ds_3gpp_pdn_throt_is_apn_globally_throttled
  (pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile.context.apn, subs_id);

  ds_3gpp_pdn_throt_fill_ident_info
  (
    &throt_ident_info, 
    pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
    DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
    subs_id,
    apn_globally_blocked,
    rat_type
   );

  ds3gpp_enter_global_crit_section();
  /* get the pdn throt sm for this pdn, if it does not exist, it will be created */
  pdn_throt_sm_ptr = 
    ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, TRUE);

  if (pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN throt allocation failed");
    ds3gpp_leave_global_crit_section();
    return;
  }
  
  /*----------------------------------------------------------------------------- 
    For a given PDP type get the throttle failure count
    If throttle failure count < min_throt_failure_cnt
    set throttle failure count = min_throt_failure_cnt
   ------------------------------------------------------------------------------*/
  if ((pdn_pdp_type == DS_UMTS_PDP_IPV4) || (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
  {
    ds_3gppi_throt_sm_get_failure_count(pdn_throt_sm_ptr->v4_throttle_sm,
                                        &fail_cnt);

    if (fail_cnt < min_throt_failure_cnt)
    {
      ds_3gppi_throt_sm_set_failure_count(pdn_throt_sm_ptr->v4_throttle_sm,
                                          min_throt_failure_cnt);
      DS_3GPP_MSG1_MED("Setting IPV4 throttle failure count to %d",
                       min_throt_failure_cnt);
    }

  }
  
  if ((pdn_pdp_type == DS_UMTS_PDP_IPV6) || (pdn_pdp_type == DS_UMTS_PDP_IPV4V6))
  {
    ds_3gppi_throt_sm_get_failure_count(pdn_throt_sm_ptr->v6_throttle_sm,
                                        &fail_cnt);

    if (fail_cnt < min_throt_failure_cnt)
    {
      ds_3gppi_throt_sm_set_failure_count(pdn_throt_sm_ptr->v6_throttle_sm,
                                          min_throt_failure_cnt);
      DS_3GPP_MSG1_MED("Setting IPV6 throttle failure count to %d",
                       min_throt_failure_cnt);
    }

  }

  ds3gpp_leave_global_crit_section();
  return;
}
/*===========================================================================
FUNCTION DS_3GPP_APN_THROT_READ_FROM_EFS

DESCRIPTION
  This function reads all the APN throttle params from EFS and populates the
  local cache (apn_throt_t3396_timer_tbl and imsi info).

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on success.
  FALSE - on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_apn_throt_read_from_efs
(
  sys_modem_as_id_e_type subs_id
)
{
  int32                                 fs_handle;
  uint8                                 apn_index = 0;
  ds_3gpp_pdn_throt_rel10_info_type    *rel10_info_ptr = NULL;
  boolean                               ret = TRUE; 
  throt_info_t3396_timer_type_dyn_s temp_t3396_timer_info;

/*-----------------------------------------------------------------------*/

  /*-------------------------------------------------------------------
    Open the file for read 
  ------------------------------------------------------------------*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return FALSE;
  }

  fs_handle = mcfg_fopen( rel10_info_ptr->ds_3gpp_apn_throt_file_path,
                          MCFG_FS_O_RDONLY,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          (mcfg_fs_sub_id_e_type)subs_id);
  if ( fs_handle == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_open failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  do
  {
    /*-------------------------------------------------------------------
      Read the contents
    ------------------------------------------------------------------*/
    if( mcfg_fread(fs_handle,
                 &rel10_info_ptr->imsi_data, 
                   sizeof(nv_ehrpd_imsi_type),
                   MCFG_FS_TYPE_EFS) <= 0 )
    {
      DS_3GPP_MSG2_ERROR("efs_read failed for %d errno=%d.", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      ret = FALSE;
      break;
    }

    for (apn_index = 0; apn_index < DS_MAX_APN_PLMN_SUPPORTED; apn_index++)
    {
      memset(&temp_t3396_timer_info, 0, 
           sizeof(throt_info_t3396_timer_type_dyn_s));

      if ( mcfg_fread(fs_handle,
                (void*)&(temp_t3396_timer_info),
                      sizeof(throt_info_t3396_timer_type_dyn_s),
                      MCFG_FS_TYPE_EFS) <= 0 )
      {
        DS_3GPP_MSG2_ERROR( "efs_read failed for %d errno=%d", 
                            fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
        ret = FALSE;
        break;
      }
      else
      {
        if (temp_t3396_timer_info.valid)
        {
          /*Allocate Memory while reading the efs file for all valid entries*/
          rel10_info_ptr->t3396_timer_tbl[apn_index].
            throt_info_t3396_timer_type_dyn_p = (throt_info_t3396_timer_type_dyn_s *)
            modem_mem_alloc(sizeof(throt_info_t3396_timer_type_dyn_s),
            MODEM_MEM_CLIENT_DATA);
           
          memscpy((void*)(rel10_info_ptr->t3396_timer_tbl[apn_index].
                  throt_info_t3396_timer_type_dyn_p),
                  sizeof(throt_info_t3396_timer_type_dyn_s),
                  (void *)&temp_t3396_timer_info,
                  sizeof(throt_info_t3396_timer_type_dyn_s));
        }
      }
    }

  } while (0); 
  /*-------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------*/
  if( mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_close failed for %d errno = %d", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  return ret;
} /* ds_3gpp_apn_throt_read_from_efs */

/*===========================================================================
FUNCTION DS_3GPP_APN_THROT_CREATE_EFS_THROTTLE_PARAM_FILE

DESCRIPTION
  This function creates EFS file for storing APN throttle parameters.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_apn_throt_create_efs_throttle_param_file
(
  sys_modem_as_id_e_type subs_id
)
{
  int32                              fs_handle;
  ds_3gpp_pdn_throt_rel10_info_type *rel10_info_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return FALSE;
  }

  fs_handle = mcfg_fopen( rel10_info_ptr->ds_3gpp_apn_throt_file_path,
                          MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                          MCFG_FS_DEFFILEMODE,
                          MCFG_FS_TYPE_EFS,
                          (mcfg_fs_sub_id_e_type)subs_id);
                        
  if ( fs_handle == -1)
  {
    DATA_MSG2_ERROR("efs_open failed for fd=%d errno=%d.",
                    fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------*/
  if( mcfg_fclose(fs_handle, MCFG_FS_TYPE_EFS) == -1 )
  {
    DS_3GPP_MSG2_ERROR("efs_close failed for %d errno = %d", 
                        fs_handle, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPP_APN_THROT_POWERUP_THROT_HDLR

DESCRIPTION
  This function does the powerup throttling of APNs which were globally
  blocked before last UE power down.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_apn_throt_powerup_throt_hdlr
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8                              index = 0;
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_ptr;
  ps_iface_net_down_reason_type      ps_down_reason = 
                                     PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;

  ds_3gpp_pdn_throt_rel10_info_type *rel10_info_ptr = NULL;
  boolean                            global_blocking = FALSE;
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
/*-----------------------------------------------------------------------*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_LOW("rel10_info_ptr is null");
    return;
  }

  for (index = 0; index < DS_MAX_APN_PLMN_SUPPORTED; index++)
  {
    if(!ds_3gpp_pdn_throt_apn_throt_t3396_validate(
        &(rel10_info_ptr->t3396_timer_tbl[index])))
    {
      continue;
    }

    if (rel10_info_ptr->t3396_timer_tbl[index].
        throt_info_t3396_timer_type_dyn_p->valid == TRUE)
    {
      if((memcmp((void*)(&global_plmn_id),
                 (void *)(&(rel10_info_ptr->t3396_timer_tbl[index].
                  throt_info_t3396_timer_type_dyn_p->plmn_id)),
                  sizeof(sys_plmn_id_s_type))== 0))
      {
        global_blocking = TRUE;
      }
      else
      {
        global_blocking = FALSE;
      }

      if (0 != (long int)rel10_info_ptr->t3396_timer_tbl[index].
             throt_info_t3396_timer_type_dyn_p->timer_val)
      {
          /*If timer not expired*/
         DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Powerup throttling APN %s",
                                 (char*)rel10_info_ptr->t3396_timer_tbl[index].
                                  throt_info_t3396_timer_type_dyn_p->apn_name);

         DS_3GPP_MSG1_HIGH("timer_val is %d milli-secs",
                            rel10_info_ptr->t3396_timer_tbl[index].
                            throt_info_t3396_timer_type_dyn_p->timer_val
                            );

         ds_3gpp_pdn_throt_fill_ident_info
         (
           &throt_ident_info, 
           rel10_info_ptr->t3396_timer_tbl[index].
             throt_info_t3396_timer_type_dyn_p->apn_name, 
           DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED,
           subs_id,
           FALSE,
           DS_3GPP_THROT_RAT_GLOBAL
        );

        throt_ident_info.plmn_id =  
          rel10_info_ptr->t3396_timer_tbl[index].
            throt_info_t3396_timer_type_dyn_p->plmn_id;

          /*---------------------------------------------------------------- 
          Perform global APN throttling. Create a PDN throt SM exclusively 
          for globally throttling the given APN across all PLMNs.
          ------------------------------------------------------------------*/
        pdn_throt_sm_ptr = 
          ds_3gpp_pdn_throt_sm_get_throttle_sm_plmn_based(&throt_ident_info, 
                                                           TRUE);

         if (pdn_throt_sm_ptr == NULL)
         {
           DS_3GPP_MSG0_ERROR("pdn_throt_sm_ptr returned NULL");
           DATA_3GPP_MSG_SPRINTF_1
             (MSG_LEGACY_HIGH, "Not able to powerup throttle APN: %s,"
                "hence clearing the t3396 timer tbl entry",
              (char*)rel10_info_ptr->t3396_timer_tbl[index].
               throt_info_t3396_timer_type_dyn_p->apn_name);

           ds_3gpp_apn_throt_clear_t3396_tmr_entry(
             rel10_info_ptr->t3396_timer_tbl[index].
             throt_info_t3396_timer_type_dyn_p->apn_name,
             rel10_info_ptr->t3396_timer_tbl[index].
             throt_info_t3396_timer_type_dyn_p->plmn_id,
             subs_id);

           continue;
         }
              
         if( FALSE == ds_3gpp_apn_throt_set_t3396_timer(
                        rel10_info_ptr->t3396_timer_tbl[index].
                        throt_info_t3396_timer_type_dyn_p->apn_name,
                        rel10_info_ptr->t3396_timer_tbl[index].
                         throt_info_t3396_timer_type_dyn_p->timer_val,
                        rel10_info_ptr->t3396_timer_tbl[index].
                        throt_info_t3396_timer_type_dyn_p->plmn_id,
                        subs_id,
                        ps_down_reason) 
         )
         {
           DS_3GPP_MSG0_ERROR("Failure to store t3396 timer in EFS");
           continue;
         }
        
         ds_3gppi_throt_sm_register_timer_start_ev_cb(
           pdn_throt_sm_ptr->v4_throttle_sm,
           ds_3gpp_pdn_throt_timestamp_apn_throt_tmr,
           (void*)subs_id);
         
         ds_3gppi_throt_sm_register_timer_expiry_ev_cb(
           pdn_throt_sm_ptr->v4_throttle_sm,
           ds_3gpp_pdn_throt_unblock_apn_on_timer_exp,
           (void*)subs_id);

#ifdef FEATURE_DATA_REL10
         if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
         {
           if ((ps_down_reason== PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) || 
               (ps_down_reason == PS_NET_DOWN_REASON_UNKNOWN_APN) )
           {
    
            ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
              pdn_throt_sm_ptr->v4_throttle_sm,
              ds_3gpp_apn_throt_get_t3396_timer_cb,
              (void*)subs_id);

            ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
              pdn_throt_sm_ptr->v6_throttle_sm,
              ds_3gpp_apn_throt_get_t3396_timer_cb,
              (void*)subs_id);

           }
         }
#endif /*FEATURE_DATA_REL10*/

          /*Set the globally_blocked flag to TRUE for the APN*/
         if (global_blocking)
         {
          ds_3gpp_pdn_throt_set_global_throttle_flag(
             rel10_info_ptr->t3396_timer_tbl[index].
             throt_info_t3396_timer_type_dyn_p->apn_name, subs_id);
         }
       
          ds_3gppi_throt_sm_feed_failure_to_core_sm(
                                       pdn_throt_sm_ptr->v4_throttle_sm,
                                       (void*)ps_down_reason, subs_id,NULL);
       /*-------------------------------------------------------------------------- 
          Check is_throttled flag and if the throttled flag is set, log the
          information
        -------------------------------------------------------------------------*/
          if (pdn_throt_sm_ptr->v4_throttle_sm->is_throttled)
          {

            ds_3gpp_pdn_throt_update_log_list(pdn_throt_sm_ptr->
                                             v4_throttle_sm,DS_UMTS_PDP_IPV4);


            ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_ptr->v4_throttle_sm,
                                             pdn_throt_sm_ptr->v4_throttle_sm->apn,
                                             DS_UMTS_PDP_IPV4);

 
          }
          ds_3gppi_throt_sm_feed_failure_to_core_sm(
                                       pdn_throt_sm_ptr->v6_throttle_sm,
                                       (void*)ps_down_reason, subs_id,NULL);
          if (pdn_throt_sm_ptr->v6_throttle_sm->is_throttled)
          {

            ds_3gpp_pdn_throt_update_log_list(pdn_throt_sm_ptr->
                                               v6_throttle_sm,DS_UMTS_PDP_IPV6);


            ds_3gpp_pdn_throt_log_throt_info(pdn_throt_sm_ptr->v6_throttle_sm,
                                             pdn_throt_sm_ptr->v6_throttle_sm->apn,
                                             DS_UMTS_PDP_IPV6);

          }
        }
        else
        {
          /*If timer has expired*/
          DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Clearing the t3396 timer tbl"
            "entry for APN %s after powerup",
             (char*)rel10_info_ptr->t3396_timer_tbl[index].
              throt_info_t3396_timer_type_dyn_p->apn_name);

          ds_3gpp_apn_throt_clear_t3396_tmr_entry(
            rel10_info_ptr->t3396_timer_tbl[index].
              throt_info_t3396_timer_type_dyn_p->apn_name,
            rel10_info_ptr->t3396_timer_tbl[index].
              throt_info_t3396_timer_type_dyn_p->plmn_id,
            subs_id);
      }
    } /*if: apn_throt_t3396_timer_tbl[index].valid*/
  } /*for: apn index*/

  /* Advertise globally blocked APNs to APM and CM*/
  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);

#ifdef FEATURE_DATA_LTE
   ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable(subs_id);
#endif /*FEATURE_DATA_LTE*/

  return;
}/*ds_3gpp_apn_throt_powerup_throt_hdlr*/

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
)
{
  nv_ehrpd_imsi_type                   temp_imsi_data;
  ds_3gpp_pdn_throt_efs_data_buf_type  write_data;
  ds_3gpp_pdn_throt_rel10_info_type    *rel10_info_ptr = NULL;
/*-----------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_imsi_info_avail_cmd_hdlr");
  memset(&temp_imsi_data, 0, sizeof(nv_ehrpd_imsi_type));

#ifdef FEATURE_DATA_RPM
  /* Invoke RPM SIM read	*/
  ds_3gpp_rpm_read_sim_info(subs_id);
#endif /* FEATURE_DATA_RPM */

  if (ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
  {
    rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

    if (rel10_info_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
      return;
    }

    if (rel10_info_ptr->imsi_data_avail == FALSE)
    {
      (void)ds3g_mmgsdi_get_imsi_data_per_subs_id(&temp_imsi_data, subs_id);

      if ( memcmp((void*)&(rel10_info_ptr->imsi_data), 
                  (void*)&(temp_imsi_data), 
                   sizeof(nv_ehrpd_imsi_type)) != 0 )
      {
        DS_3GPP_MSG0_HIGH("Sim changed, wiping out the apn throt EFS data");

        if(FALSE == ds_3gpp_apn_throt_reset_apn_throt_efs(subs_id))
        {
          DS_3GPP_MSG0_ERROR("Error resetting apn throt efs file");
        }
        /*write the current IMSI info in EFS*/
        write_data.data_ptr = &temp_imsi_data;
        write_data.data_len = sizeof(nv_ehrpd_imsi_type);
        (void)ds_3gpp_apn_throt_write_to_efs(&write_data, 0, subs_id);

        /* Update local cache with the new IMSI value */
        memscpy(&rel10_info_ptr->imsi_data, sizeof(nv_ehrpd_imsi_type),
                &temp_imsi_data, sizeof(nv_ehrpd_imsi_type));

        /* Clear T3396 throttling upon SIM removal */
        ds_3gpp_pdn_throt_clear_t3396_on_usim_removal(subs_id, TRUE);
      }
      else
      {
        DS_3GPP_MSG0_HIGH("IMSI didn't change, same card re-inserted, not clearing"
                          " T3396 throttling");
        /*perform powerup throttling of APNs which were globally
        blocked before last UE power down*/
        ds_3gpp_apn_throt_powerup_throt_hdlr(subs_id);
      }

      rel10_info_ptr->imsi_data_avail = TRUE;
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Powerup apn throttling already done");
    }
  }
  return;
}/*ds_3gpp_pdn_throt_imsi_info_avail_cmd_hdlr*/

/*===========================================================================
FUNCTION DS_3GPP_APN_THROTTLE_CREATE_AND_INIT_THROT_PARAM_FILE

DESCRIPTION
  This function creates and initializes the APN Throttle Param EFS File.
 
PARAMETERS
  Subs Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_apn_throttle_create_and_init_throt_param_file
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_throt_rel10_info_type    *rel10_info_ptr = NULL;
  ds_3gpp_pdn_throt_efs_data_buf_type   write_data;
  /*-----------------------------------------------------------------------*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }

  /* APN throttle param file not present. Create it */
  if (FALSE == ds_3gpp_apn_throt_create_efs_throttle_param_file(subs_id))
  {
    DS_3GPP_MSG0_ERROR("Failed to create efs throttle param file");
  }
  else
  {
    if(FALSE == ds_3gpp_apn_throt_reset_apn_throt_efs(subs_id))
    {
      DS_3GPP_MSG0_ERROR("Error resetting apn throt efs file");
    }

    if( TRUE == 
        (rel10_info_ptr->imsi_data_avail = 
           ds3g_mmgsdi_get_imsi_data_per_subs_id
             (&rel10_info_ptr->imsi_data, subs_id)) )
    {
      /*write the current IMSI info in EFS*/
      write_data.data_ptr = &rel10_info_ptr->imsi_data;
      write_data.data_len = sizeof(nv_ehrpd_imsi_type);
      (void)ds_3gpp_apn_throt_write_to_efs(&write_data, 0, subs_id);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Could not get IMSI info from MMGDSI");
    }
  }
}

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
)
{
  ds_3gpp_pdn_throt_efs_data_buf_type  write_data;
  nv_ehrpd_imsi_type                   temp_imsi_data
                                         [DS3GSUBSMGR_SUBS_ID_MAX];
  boolean                              temp_imsi_data_avail
                                         [DS3GSUBSMGR_SUBS_ID_MAX];
  struct fs_stat                       sbuf;
  uint8                                i = 0;
  ds_3gpp_pdn_throt_rel10_info_type    *rel10_info_ptr = NULL;
  boolean                              efs_marked_for_clear = FALSE;
/*-----------------------------------------------------------------------*/
  DS_3GPP_MSG1_HIGH("In ds_3gpp_apn_throt_init, subs_id: %d",subs_id);

  rel10_info_ptr = ds_3gpp_pdn_throt_allocate_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_MED("rel10_info_ptr is null");
    return;
  }

  strlcpy(ds_3gpp_apn_throt_dir_name, 
              DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME,
              sizeof(ds_3gpp_apn_throt_dir_name));

  ds_3gpp_apn_throt_dir_name[DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME_LEN] = '/';

  ds_3gpp_apn_throt_dir_name[DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME_LEN + 1] 
    = '\0';

  /* Check if the APN throttle param file is present */
  strlcpy(rel10_info_ptr->ds_3gpp_apn_throt_file_path, 
            ds_3gpp_apn_throt_dir_name, 
            sizeof(rel10_info_ptr->ds_3gpp_apn_throt_file_path));

  strlcat(rel10_info_ptr->ds_3gpp_apn_throt_file_path,
            DS_3GPP_APN_THROTTLE_PARAM_FILE_NAME,
            sizeof(rel10_info_ptr->ds_3gpp_apn_throt_file_path) );

  rel10_info_ptr->ds_3gpp_apn_throt_file_path
    [DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME_LEN + \
      1 + DS_3GPP_APN_THROTTLE_PARAM_FILE_NAME_LEN] = '\0';

  /*Initialize the apn throttle t3396 timer table */
  ds_3gpp_pdn_throt_apn_throt_tbl_init(subs_id);

  memset((void*)&rel10_info_ptr->imsi_data, 0, 
          sizeof(nv_ehrpd_imsi_type));

  rel10_info_ptr->imsi_data_avail = FALSE;

  rel10_info_ptr->sampling_timer_val = 0xffffffff;
  
  rex_def_timer_ex(&rel10_info_ptr->sampling_timer,
                  (rex_timer_cb_type)ds_3gpp_pdn_throt_sampling_timer_cb,
                  (unsigned long)subs_id);

  if( mcfg_fs_stat((char*) ds_3gpp_apn_throt_dir_name,
                   &sbuf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)subs_id) == MCFG_FS_STATUS_OK )
  {
    if( mcfg_fs_stat((char*) rel10_info_ptr->ds_3gpp_apn_throt_file_path,
                     &sbuf,
                     MCFG_FS_TYPE_EFS,
                     (mcfg_fs_sub_id_e_type)subs_id) != MCFG_FS_STATUS_OK )
    {
      /* APN throttle param file not present. Create it */
      ds_3gpp_apn_throttle_create_and_init_throt_param_file(subs_id);
    }

    else 
    { 
      /*File present in EFS, read and populate the local cache*/
      ds_3gpp_apn_throt_read_from_efs(subs_id);
    }
  }
  else
  { 
    /* apn throt directory not present, create it */
    if( mcfg_mkdir( DS_3GPP_APN_THROTTLE_PARAM_DIR_NAME,
                    S_IREAD|S_IWRITE|S_IEXEC,
                    MCFG_FS_TYPE_EFS) == -1 )
    {
      DS_3GPP_MSG1_ERROR( "apn throt: fs_mkdir failed errno = %d", 
                           mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return;
    }

    ds_3gpp_apn_throttle_create_and_init_throt_param_file(subs_id);
  }/* apn throt directory not present */
 
  for (i=0; i<DS3GSUBSMGR_SUBS_ID_MAX; i++ )
  {
    temp_imsi_data_avail[i] = 
      ds3g_mmgsdi_get_imsi_data_per_subs_id(&temp_imsi_data[i], i);

    if( TRUE == temp_imsi_data_avail[i] )
    {
      if ( memcmp((void*)&(rel10_info_ptr->imsi_data), 
                  (void*)&(temp_imsi_data[i]), 
                   sizeof(nv_ehrpd_imsi_type)) != 0 )
      {
        efs_marked_for_clear = TRUE;        
      }
      else
      {
        efs_marked_for_clear = FALSE;
        /*perform powerup throttling of APNs which were globally
        blocked before last UE power down*/
        ds_3gpp_apn_throt_powerup_throt_hdlr(subs_id);
        break;
      }
    }
    else
    {
      efs_marked_for_clear = FALSE;
      DS_3GPP_MSG0_ERROR("Could not get IMSI info from MMGDSI");
    }
  }

  if (efs_marked_for_clear == TRUE)
  {
    if(FALSE == ds_3gpp_apn_throt_reset_apn_throt_efs(subs_id))
    {
      DS_3GPP_MSG0_ERROR("Error resetting apn throt efs file");
    }
    /*write the current IMSI info in EFS*/
    write_data.data_ptr = &temp_imsi_data;
    write_data.data_len = sizeof(nv_ehrpd_imsi_type);
    (void)ds_3gpp_apn_throt_write_to_efs(&write_data, 0, subs_id);
  }

}/*ds_3gpp_apn_throt_init*/



/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_HANDLE_T3396_ABSENT

DESCRIPTION
  This function performs infinite throttling and is called when no T3396
  timer value is obatined in PDN rej/ PDP deactivation
  

PARAMETERS   : 
  
 pdn_throt_sm_ptr               --- Pdn throttle_ptr
 apn                            ---  apn
 down_reason                    ---  net_down_reason
 subs_id                        ---  subs_id
 pdn_context_p                  ---  pdn_cntxt_ptr
 global_throttling              ---  global throttling flag
)
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_pdn_throt_handle_t3396_absent
(
  ds_3gpp_pdn_throttle_sm_s     *pdn_throt_sm_ptr,
  byte                          *apn,
  ps_iface_net_down_reason_type  down_reason,
  sys_modem_as_id_e_type         subs_id,
  ds_pdn_context_s              *pdn_context_p,
  boolean                        global_throttling
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ((pdn_throt_sm_ptr == NULL) || (apn == NULL))
  {
    DS_3GPP_MSG2_ERROR("pdn throt sm ptr:0x%x,"
                       "apn:0x%x",
                       pdn_throt_sm_ptr,
                       apn);
    return;
  }

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }


  ds_3gpp_pdn_throt_handle_perm_conn_failure(pdn_throt_sm_ptr,
                                            (void*)down_reason,
                                             DS_UMTS_PDP_IPV4V6, 
                                             subs_id,
                                             pdn_context_p);

  ds_3gpp_pdn_throt_sm_set_sm_type(pdn_throt_sm_ptr,
                                   DS_3GPP_THROTTLE_TYPE_T3396_ABSENT);


  /*------------------------------------------------------------------------- 
    Only if global throttling is enabled proceed to store the auth info
    If not, return and also set SM type to T3396 as this would be the case
    where network returned inactive T3396 timer resulting in permanent
    throttling
    -------------------------------------------------------------------------*/
  if(!global_throttling)
  {
    DS_3GPP_MSG0_HIGH("Return from ds_3gpp_pdn_throt_handle_t3396_absent");
    ds_3gpp_pdn_throt_sm_set_sm_type(pdn_throt_sm_ptr,
                                     DS_3GPP_THROTTLE_TYPE_T3396);
    return;
  }

  /*------------------------------------------------------------------------- 
    Store the auth info from pdn context
   -------------------------------------------------------------------------*/
  if((pdn_throt_sm_ptr->v4_throttle_sm != NULL) && 
     (pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p == NULL))
  {
    pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p =
      (ds_umts_pdp_auth_type*)modem_mem_alloc(sizeof(ds_umts_pdp_auth_type),
                                              MODEM_MEM_CLIENT_DATA);

    if(pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("Heap allocation failure when allocating auth_data");
    }
    else
    {
      /*------------------------------------------------------------------- 
        If pdn info_null is FALSE, then copy from pdn context's info_ptr
        -------------------------------------------------------------------*/
      if((pdn_context_p->ds_pdn_context_dyn_p->info_null == FALSE) &&
         ((pdn_context_p->ds_pdn_context_dyn_p->info.valid_fields & NET_CFG_PRM_AUTH_PREF_MASK) ||
         (pdn_context_p->ds_pdn_context_dyn_p->info.valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK) ||
         (pdn_context_p->ds_pdn_context_dyn_p->info.valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK)))
      {
          pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p->auth_type = 
                pdn_context_p->ds_pdn_context_dyn_p->info.auth_info.auth_type;

          memscpy((void *)(pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p->username),
                   sizeof(pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p->username),
                  (void *)(pdn_context_p->ds_pdn_context_dyn_p->info.auth_info.params.user_id_info),
                  PPP_MAX_USER_ID_LEN);

          memscpy((void *)(pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p->password),
                  sizeof(pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p->password),
                  (void *)(pdn_context_p->ds_pdn_context_dyn_p->info.auth_info.params.passwd_info),
                  PPP_MAX_PASSWD_LEN);

      }
      else
      {
        memscpy((void *)(pdn_throt_sm_ptr->v4_throttle_sm->user_data.auth_info_p),
                 sizeof(ds_umts_pdp_auth_type),
                (void*)(&pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.auth),
                sizeof(ds_umts_pdp_auth_type));
      }

      DS_3GPP_MSG0_HIGH("Auth info copied to internal throttle sm");
    }
  }

  if((pdn_throt_sm_ptr->v6_throttle_sm != NULL) && 
     (pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p == NULL))
  {
    pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p =
      (ds_umts_pdp_auth_type*)modem_mem_alloc(sizeof(ds_umts_pdp_auth_type),
                                              MODEM_MEM_CLIENT_DATA);

    if(pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("Heap allocation failure when allocating auth_data");
    }
    else
    {
      if((pdn_context_p->ds_pdn_context_dyn_p->info_null == FALSE) &&
         ((pdn_context_p->ds_pdn_context_dyn_p->info.valid_fields & NET_CFG_PRM_AUTH_PREF_MASK) ||
         (pdn_context_p->ds_pdn_context_dyn_p->info.valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK) ||
         (pdn_context_p->ds_pdn_context_dyn_p->info.valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK)))
      {
        pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p->auth_type = 
                pdn_context_p->ds_pdn_context_dyn_p->info.auth_info.auth_type;

        memscpy((void *)(pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p->username),
                sizeof(pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p->username),
                (void *)(pdn_context_p->ds_pdn_context_dyn_p->info.auth_info.params.user_id_info),
                PPP_MAX_USER_ID_LEN);

        memscpy((void *)(pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p->password),
                 sizeof(pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p->password),
                (void *)(pdn_context_p->ds_pdn_context_dyn_p->info.auth_info.params.passwd_info),
                PPP_MAX_PASSWD_LEN);
      }
      else
      {
        memscpy((void *)(pdn_throt_sm_ptr->v6_throttle_sm->user_data.auth_info_p),
                 sizeof(ds_umts_pdp_auth_type),
                (void*)(&pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.auth),
                sizeof(ds_umts_pdp_auth_type));
      }

      DS_3GPP_MSG0_HIGH("Auth info copied to internal throttle sm");
    }
  }


}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_UMTS_HANLDE_REJECT_WITH_T3396_IE

DESCRIPTION
  This function handles the reject message received with a t3396 IE
  based on Release 10 requirements
  

PARAMETERS   : 
  gw_ps_call_info 					 -  PDN throttle state machine pointer to
  apn                        -  APN name on the PDP context
 
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
)
{
  ds_3gpp_pdn_throttle_sm_s      *pdn_throt_sm_ptr = NULL;
  sys_plmn_id_s_type              current_plmn;
  ds_3gpp_pdn_throttle_sm_info_s *pdn_throt_sm_info_ptr = NULL;
  boolean                         global_throttling_nv = FALSE;
/*-----------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_umts_handle_reject_with_t3396_ie");
  pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                       (apn, TRUE, TRUE, DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, 
                          subs_id);
  if (pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Couldnot get throttle SM, not able to throttle");
    return;
  }

  global_throttling_nv = ds_3gpp_nv_manager_get_global_throttling(subs_id);

  if(ds_3gppi_is_rel_11_supported())
  {
    /*---------------------------------------------------------------------
      Stop T3396 timer for v4 and v6, if it is already running and 
      clear the global throttle flag for the current APN
    ----------------------------------------------------------------------*/
    memset((void*)&(current_plmn),0,sizeof(sys_plmn_id_s_type));
    ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);
    ds_3gpp_pdn_throt_clear_is_throttle_flag_and_timer(
                                                       pdn_throt_sm_ptr,
                                                       current_plmn,
                                                       subs_id,
                                                       apn);
  } 

  /* Perform throttling based on t3396 IE */   
  if (gw_ps_call_info->backoff_timer.active)
  {
    DS_3GPP_MSG1_HIGH("backoff timer is active, timer_count %d",
                       gw_ps_call_info->backoff_timer.timer_count);
    if (gw_ps_call_info->backoff_timer.timer_count != 0)
    {
      pdn_throt_sm_info_ptr = (ds_3gpp_pdn_throttle_sm_info_s*)
                                pdn_throt_sm_ptr->pdn_throt_sm_info_p;

      ASSERT(pdn_throt_sm_info_ptr != NULL);

      if( FALSE == ds_3gpp_apn_throt_set_t3396_timer(
                           apn,
                           gw_ps_call_info->backoff_timer.timer_count,
                           pdn_throt_sm_info_ptr->plmn_id,
                           subs_id,
                           down_reason) )
      {
        DATA_ERR_FATAL("Failure to store t3396 timer in EFS");
      }

      ds_3gppi_throt_sm_register_timer_start_ev_cb(
        pdn_throt_sm_ptr->v4_throttle_sm,
        ds_3gpp_pdn_throt_timestamp_apn_throt_tmr,
        (void*)subs_id);

      ds_3gppi_throt_sm_register_timer_expiry_ev_cb(
        pdn_throt_sm_ptr->v4_throttle_sm,
        ds_3gpp_pdn_throt_unblock_apn_on_timer_exp,
        (void*)subs_id);

#ifdef FEATURE_DATA_REL10
      if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
      {
        if ((down_reason== PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) || 
            (down_reason == PS_NET_DOWN_REASON_UNKNOWN_APN) )
        {
          ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
            pdn_throt_sm_ptr->v4_throttle_sm,
            ds_3gpp_apn_throt_get_t3396_timer_cb,
            (void*)subs_id);

          ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
              pdn_throt_sm_ptr->v6_throttle_sm,
              ds_3gpp_apn_throt_get_t3396_timer_cb,
              (void*)subs_id);
        }
      }
#endif /*FEATURE_DATA_REL10*/
      ds_3gpp_pdn_throt_perform_apn_throttling
      (
        pdn_throt_sm_ptr,
        apn,
        (void*)down_reason,
        DS_UMTS_PDP_IPV4V6,
        subs_id,
        TRUE,
        pdn_context_p
      );
    }

    /*------------------------------------------------------------------------ 
      If cause_code is 26 or 27 and global throttling is enabled perform
      infinite throttling.
      This infinite throttling will be reset only when PAP/CHAP change happens
      or when the sim is reset
    --------------------------------------------------------------------------*/
    else if((gw_ps_call_info->backoff_timer.timer_count == 0) &&
            (global_throttling_nv) &&
            ((down_reason== PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) || 
             (down_reason == PS_NET_DOWN_REASON_UNKNOWN_APN)))
    {
      DS_3GPP_MSG0_HIGH("Performing permanent throttling since t3396 is zero");

      ds_3gpp_pdn_throt_set_global_throttle_flag(apn,subs_id);

      ds_3gpp_pdn_throt_handle_t3396_absent(pdn_throt_sm_ptr,
                                            apn,
                                            down_reason,
                                            subs_id,
                                            pdn_context_p,
                                            TRUE);
    }
    else
    {
       DS_3GPP_MSG0_HIGH("No throttling since t3396 is absent");
       return;

    }
  }
  else
  {
    /*Set the globally_blocked flag to TRUE for the APN*/
    ds_3gpp_pdn_throt_set_global_throttle_flag(apn, subs_id);

    ds_3gpp_pdn_throt_handle_t3396_absent(pdn_throt_sm_ptr,
                                          apn,
                                          down_reason,
                                          subs_id,
                                          pdn_context_p,
                                          global_throttling_nv);
  }
  /* Advertise current blocked APNs to APM and CM*/


   ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
#ifdef FEATURE_DATA_LTE
   ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable(subs_id);
#endif /* FEATURE_DATA_LTE */

return;
} /*ds_3gpp_pdn_throt_umts_handle_reject_with_t3396_ie*/

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_LTE_HANLDE_REJECT_WITH_T3396_IE

DESCRIPTION
  This function handles the reject message received with a t3396 IE
  based on Release 10 requirements
  

PARAMETERS   : 
  lte_call_info 				  	 -  LTE call info received in reject from CM
  apn                        -  APN name on the PDN connection
  down_reason
  all_lte_atch_apn_blked
 
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
)
{
  ds_3gpp_pdn_throttle_sm_s       *pdn_throt_sm_ptr = NULL;
  boolean                          per_plmn_blocking = FALSE;
  ps_iface_net_down_reason_type    v4_prev_failure_cause_code = 
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ps_iface_net_down_reason_type    v6_prev_failure_cause_code = 
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED;
  sys_plmn_id_s_type               current_plmn;
  ds_3gpp_pdn_throttle_sm_info_s   *pdn_throt_sm_info_ptr = NULL;
  ds_3gpp_throt_rat_e_type         rat_type = DS_3GPP_THROT_RAT_GLOBAL;
  boolean                          global_throttling_nv = FALSE;
/*-----------------------------------------------------------------------*/

  per_plmn_blocking =  ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id);
  global_throttling_nv = ds_3gpp_nv_manager_get_global_throttling(subs_id);

  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_lte_handle_reject_with_t3396_ie");

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p) &&
     DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_context_p) == TRUE )
  {
    DS_3GPP_MSG0_ERROR("Emergency pdn is passed. No throttling needed");
    return;
  }

  pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm(
                       apn, 
                       !per_plmn_blocking,
                       TRUE,
                       DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, 
                       subs_id);

  if (pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Couldnot get throttle SM, not throttling APN");
    return;
  }

  if( ds_3gppi_is_rel_11_supported())
  {
    /*---------------------------------------------------------------------
      Stop T3396 timer for v4 and v6, if it is already running and 
      clear the global throttle flag for the current APN
    ----------------------------------------------------------------------*/
    memset((void*)&(current_plmn),0,sizeof(sys_plmn_id_s_type));
    ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);
    ds_3gpp_pdn_throt_clear_is_throttle_flag_and_timer(
                                                       pdn_throt_sm_ptr,
                                                       current_plmn,
                                                       subs_id,
                                                       apn);
  }

  
  /* Perform throttling based on t3396 IE */   
  if (lte_call_info->backoff_timer.active)
  {
    DS_3GPP_MSG1_HIGH("backoff timer is active, timer_count %d",
                       lte_call_info->backoff_timer.timer_count);

    if(lte_call_info->backoff_timer.timer_count == 0)
    {
      if (per_plmn_blocking)
      {
        /* Requireement: Two consecutive Pdn rejects with cause code:
        27: missing or unknown APN when the T3396 IE is absent and T3396 is set to 0*/
        ds_3gpp_pdn_throt_get_prev_cause_code(
                         pdn_throt_sm_ptr,
                        &v4_prev_failure_cause_code,
                        &v6_prev_failure_cause_code);

        if ( (down_reason ==PS_NET_DOWN_REASON_UNKNOWN_APN) && 
             ((down_reason == v4_prev_failure_cause_code) || 
             (down_reason == v6_prev_failure_cause_code)) )
        {
          if(ds_eps_attach_sm_is_ue_attached(subs_id))
          {
            ds_3gpp_pdn_throt_handle_perm_conn_failure(
                                        pdn_throt_sm_ptr,       
                                       (void*)down_reason,
                                        DS_UMTS_PDP_IPV4V6,
                                        subs_id,
                                        pdn_context_p);
          }
          else
          {
             DS_3GPP_MSG0_HIGH("Not blocking APN since t3396 is "
                               "zero and Attach Apn Throttling not allowed");
          }   
        }
        else
        {
          /*--------------------------------------------------------------------------------- 
            If UE is attached perform regular throttling
          ---------------------------------------------------------------------------------*/
          if(ds_eps_attach_sm_is_ue_attached(subs_id))
          {
            if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
            {
              if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
              {
                rat_type = DS_3GPP_THROT_RAT_LTE;
              }

              ds_3gpp_pdn_throt_perform_reg_throttling(pdn_throt_sm_ptr,
                                                       pdn_context_p,
                                                       (void*)down_reason,
                                                       DS_UMTS_PDP_IPV4V6,
                                                       rat_type);   
            }
          }
          else
          {
             DS_3GPP_MSG0_HIGH("Not blocking APN since t3396 is zero and "
                               "Attach Apn Throttling not allowed");		  
          }
        }	
      }
      else if(global_throttling_nv)
      {
        /*----------------------------------------------------------------------- 
          When global_blocking/global_throttling is enabled and T3396 timer value
          is received as 0, perform infinite throttling.
          This permanent throttling will be blocked only when USER PAP/CHAP change
          occurs or on sim removal
        -------------------------------------------------------------------------*/
        DS_3GPP_MSG3_HIGH("T3396 is 0, Enforce permanent throttling ",0,0,0);
 
        ds_3gpp_pdn_throt_set_global_throttle_flag(apn, subs_id);

        ds_3gpp_pdn_throt_handle_t3396_absent(pdn_throt_sm_ptr,
                                              apn,
                                              down_reason,
                                              subs_id,
                                              pdn_context_p,
                                              TRUE);
      }
      else
      {
        /*---------------------------------------------------------------------- 
        If per_plmn_blocking is disabled and if global_throttling_nv is disabled
        and T3396 timer is 0 return
        ----------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("No throttling because T3396 timer is 0");
      }
    } /*(lte_call_info->backoff_timer.timer_count != 0)*/ 
    else
    {
      pdn_throt_sm_info_ptr = (ds_3gpp_pdn_throttle_sm_info_s*)
                                pdn_throt_sm_ptr->pdn_throt_sm_info_p;

      ASSERT(pdn_throt_sm_info_ptr != NULL);
      if( FALSE == ds_3gpp_apn_throt_set_t3396_timer(
                     apn,
                     lte_call_info->backoff_timer.timer_count,
                     pdn_throt_sm_info_ptr->plmn_id,
                     subs_id,down_reason) 
      )
      {
        DATA_ERR_FATAL("Failure to store t3396 timer in EFS");
      }

      ds_3gppi_throt_sm_register_timer_start_ev_cb(
        pdn_throt_sm_ptr->v4_throttle_sm,
        ds_3gpp_pdn_throt_timestamp_apn_throt_tmr,
        (void*)subs_id);

      ds_3gppi_throt_sm_register_timer_expiry_ev_cb(
        pdn_throt_sm_ptr->v4_throttle_sm,
        ds_3gpp_pdn_throt_unblock_apn_on_timer_exp,
        (void*)subs_id);

#ifdef FEATURE_DATA_REL10
      if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
      {
        if ((down_reason== PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) || 
            (down_reason == PS_NET_DOWN_REASON_UNKNOWN_APN) )
        {
          ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
            pdn_throt_sm_ptr->v4_throttle_sm,
            ds_3gpp_apn_throt_get_t3396_timer_cb,
            (void*)subs_id);

          ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
            pdn_throt_sm_ptr->v6_throttle_sm,
            ds_3gpp_apn_throt_get_t3396_timer_cb,
            (void*)subs_id);

        }
      }
#endif /*FEATURE_DATA_REL10*/

      ds_3gpp_pdn_throt_perform_apn_throttling(
        pdn_throt_sm_ptr,
        apn,
        (void*)down_reason,
        DS_UMTS_PDP_IPV4V6,
        subs_id,
        !per_plmn_blocking,
        pdn_context_p
      );
    }
  }
  else /* T3396  IE is absent*/ 
  {
    /*Set the globally_blocked flag to TRUE for the APN*/
    if (!per_plmn_blocking)
    {
      ds_3gpp_pdn_throt_set_global_throttle_flag(apn, subs_id);
    } 

    ds_3gpp_pdn_throt_handle_t3396_absent(pdn_throt_sm_ptr,
                                          apn,
                                          down_reason,
                                          subs_id,
                                          pdn_context_p,
                                          global_throttling_nv);
  }

  return;
}/*ds_3gpp_pdn_throt_lte_handle_reject_with_t3396_ie*/
#endif /* FEATURE_DATA_LTE */
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_HANLDE_NW_INITED_UNBLOCK_APN 
 
DESCRIPTION
  This function handles the network initiated to unblock a globally blocked 
  APN based on Release 10 requirements
  

PARAMETERS   : 
  apn                        -  APN name on the PDN connection
  subs_id                    -  Subscription Id
  rat_mask                   -  Mask Indicating the RATs to be unblocked

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn
(
  byte                      *apn,
  sys_modem_as_id_e_type     subs_id,
  uint32                     rat_mask
)
{
  sys_plmn_id_s_type                       current_plmn, zero_plmn;
  ds_3gpp_pdn_throttle_sm_type         throt_sm_type_p = 
                                           DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED;
/*---------------------------------------------------------------------------*/
  memset((void*)&(zero_plmn),0,sizeof(sys_plmn_id_s_type));
  memset((void*)&(current_plmn),0,sizeof(sys_plmn_id_s_type));

  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);

  if(memcmp((void*)(&current_plmn),
           (void*)(&zero_plmn),
            sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_3GPP_MSG0_ERROR("Zero PLMN returned as current PLMN. Can't get throt SM");
    return;
  }

    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                          "APN %s was throt'ed, unthrottling it",
                            (char*)apn);

    /* Resets t3396 timer entry from local cache and EFS file*/
  ds_3gpp_apn_throt_clear_t3396_tmr_entry(apn,global_plmn_id, subs_id);
  
  ds_3gpp_apn_throt_clear_t3396_tmr_entry(apn,current_plmn, subs_id);

  /*---------------------------------------------------------------------------
   Clear the PDN throttle information for this APN on network bearer activate
   only if it is blocked due to release 10(t3396)
  ---------------------------------------------------------------------------*/
  if((ds_3gpp_pdn_throt_get_sm_type(apn, subs_id, TRUE, &throt_sm_type_p) == TRUE)&& 
     (throt_sm_type_p == DS_3GPP_THROTTLE_TYPE_T3396))
  {
    ds_3gpp_pdn_throt_clear_throttle_state(apn, DS_UMTS_PDP_IPV4V6, subs_id, 
                                         rat_mask ); 
  }
  /* Send updated blocked APN list to CM and APM */
  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
  
  return;
} /*ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SAMPLING_TIMER_CB
 
DESCRIPTION
  This callback function is invoked when the sampling timer expires. 
  
PARAMETERS 
  callback_data

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_pdn_throt_sampling_timer_cb
(
  unsigned long callback_data
)
{
  ds_cmd_type   *cmd_buf = NULL;
  unsigned long *data_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  cmd_buf = ds_allocate_cmd_buf(sizeof(unsigned long));
  if( (cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_buf->hdr.cmd_id = DS_CMD_3GPP_PDN_THROT_SAMPLING_TIMER_EXP;
  data_ptr = (unsigned long*)cmd_buf->cmd_payload_ptr;

  *data_ptr = callback_data;

  ds_put_cmd(cmd_buf);
} /*ds_3gpp_pdn_throt_sampling_timer_cb*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_START_SAMPLING_TIMER
 
DESCRIPTION
  This function starts the sampling timer which is a fraction of the shortedt
  running T3396 timers. This timer helps keep track of elapsed timer in the
  EFS.
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_pdn_throt_start_sampling_timer
(
  boolean                calc_sampling_timer,
  sys_modem_as_id_e_type subs_id
)
{
  uint8                                     apn_index = 0;
  uint32                                    timer_val = 0;
  uint32                                    shortest_timer_val = 0xffffffff;
  ds_3gpp_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr;   
  ds_3gpp_pdn_throt_rel10_info_type        *rel10_info_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_start_sampling_timer");

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }

  if (calc_sampling_timer)
  {
    for (apn_index =0; apn_index < DS_MAX_APN_PLMN_SUPPORTED;
         apn_index++)
    {
      if(!ds_3gpp_pdn_throt_apn_throt_t3396_validate(
           &(rel10_info_ptr->t3396_timer_tbl[apn_index])))
        {
        DS_3GPP_MSG3_LOW("Invalid apn table entry", 0,0,0);
        continue;
      }

      if (rel10_info_ptr->t3396_timer_tbl[apn_index].
          throt_info_t3396_timer_type_dyn_p->valid)
          {
        if( (rel10_info_ptr->t3396_timer_tbl[apn_index].
             throt_info_t3396_timer_type_dyn_p->timer_val != 0) && 
             (rel10_info_ptr->t3396_timer_tbl[apn_index].
              throt_info_t3396_timer_type_dyn_p->timer_val <  
              shortest_timer_val) )
        {
          shortest_timer_val = rel10_info_ptr->t3396_timer_tbl[apn_index].
            throt_info_t3396_timer_type_dyn_p->timer_val;
        }
      }
    }

    if(shortest_timer_val != 0xffffffff)
    {
      if(shortest_timer_val < DS_3GPP_PDN_THROT_LONGER_T3396_TIMER)
      {
        /* If shortest running timer is less than 10 hours, use 10% of
        its value as sampling timer value */
        timer_val = shortest_timer_val/10;
        /* If timer value calculated is less than 90 seconds, it is
        increased to 90 seconds since File System flushes the changes 
        into EFS every 90 seconds */
        if (timer_val < DS_3GPP_PDN_THROT_EFS_FLUSH_TIME)
        {
          timer_val = DS_3GPP_PDN_THROT_EFS_FLUSH_TIME;
        }
      }
      else
      {
        /* If shortest running timer is greater than 10 hours, use 1 hour
        as sampling timer value */
        timer_val = DS_3GPP_PDN_THROT_LONGER_SAMPLING_TIMER;
      }

      rel10_info_ptr->sampling_timer_val = timer_val;
      DS_3GPP_MSG1_HIGH("Calculated Sampling timer %d ms", 
                         timer_val);
    }
  }

  DS_3GPP_MSG1_HIGH("Starting sampling timer for %d ms", 
                     rel10_info_ptr->sampling_timer_val);
  (void)rex_set_timer(&(rel10_info_ptr->sampling_timer), 
                      rel10_info_ptr->sampling_timer_val);

  return;
} /*ds_3gpp_pdn_throt_start_sampling_timer*/

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
)
{
  ds_3gpp_pdn_throt_efs_data_buf_type efs_write_data;
  uint8                               i;
  ds_3gpp_pdn_throt_rel10_info_type  *rel10_info_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_s           *pdn_throt_sm_ptr = NULL;
  ds_3gpp_throt_user_data            *throt_user_data_p = NULL;
  /*------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_sampling_timer_exp_cmd_hdlr");

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }


  for (i=0; i<DS_MAX_APN_PLMN_SUPPORTED; i++)
  {
    if(!ds_3gpp_pdn_throt_apn_throt_t3396_validate(
       &(rel10_info_ptr->t3396_timer_tbl[i])))
    {
      DS_3GPP_MSG3_LOW("Invalid apn table entry", 0,0,0);
      continue;
    } 
    if(rel10_info_ptr->t3396_timer_tbl[i].
                          throt_info_t3396_timer_type_dyn_p == NULL)
      {
      DS_3GPP_MSG1_HIGH("throt_info_t3396_timer_type_dyn_p is NULL for index %d", i);
      continue;
    }

        pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn(
                              rel10_info_ptr->t3396_timer_tbl[i].
                                  throt_info_t3396_timer_type_dyn_p->plmn_id,
                              rel10_info_ptr->t3396_timer_tbl[i].
                                  throt_info_t3396_timer_type_dyn_p->apn_name,
                              subs_id); 

    if (pdn_throt_sm_ptr == NULL) 
    {
      DS_3GPP_MSG0_HIGH("pdn throttle sm is NULL");
      continue;
    }

    if (pdn_throt_sm_ptr->v4_throttle_sm == NULL) 
      {
      DS_3GPP_MSG0_HIGH("pdn throttle sm's v4 sm is NULL");
      continue;
    }
    
    DS_3GPP_MSG2_MED("old timer_val %d at index %d", 
                      rel10_info_ptr->t3396_timer_tbl[i].
                        throt_info_t3396_timer_type_dyn_p->timer_val,i);
    rel10_info_ptr->t3396_timer_tbl[i].throt_info_t3396_timer_type_dyn_p->timer_val = 
                ds_3gppi_throt_sm_get_remaining_throttle_time
                  (pdn_throt_sm_ptr->v4_throttle_sm);

    DS_3GPP_MSG2_MED("new timer_val %d at index %d", 
      rel10_info_ptr->t3396_timer_tbl[i].
                      throt_info_t3396_timer_type_dyn_p->timer_val,i);

      efs_write_data.data_ptr = rel10_info_ptr->t3396_timer_tbl[i].
                                  throt_info_t3396_timer_type_dyn_p;
      efs_write_data.data_len = sizeof(throt_info_t3396_timer_type_dyn_s);

    if(pdn_throt_sm_ptr != NULL)
    {
      throt_user_data_p = 
        ds_3gppi_throt_sm_get_user_data(pdn_throt_sm_ptr->v4_throttle_sm);

      if((throt_user_data_p != NULL) && 
         (throt_user_data_p->cause_code == 
           PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES))
      {
        (void)ds_3gpp_apn_throt_write_to_efs(
                        &efs_write_data,
                        sizeof(throt_info_t3396_timer_type_dyn_s)*i +
                        sizeof(nv_ehrpd_imsi_type),
                        subs_id);
      }
    }

  }

  /* Start the sampling timer in background for valid t3396 running timers */
  ds_3gpp_pdn_throt_start_sampling_timer(FALSE, subs_id);

  return;
} /*ds_3gpp_pdn_throt_sampling_timer_exp_cmd_hdlr*/

#ifdef FEATURE_DATA_REL10
/*===========================================================================
FUNCTION DS_3GPP_APN_THROT_GET_T3396_TIMER_CB
 
DESCRIPTION
  This function fetches the T3396 timer value.
  
PARAMETERS   : 
  1. Core Throttle SM Ptr
  2. Callback Data (Subs Id)

DEPENDENCIES
  None.

RETURN VALUE
  T3396 timer value
 
SIDE EFFECTS
  None.

===========================================================================*/
static uint32 ds_3gpp_apn_throt_get_t3396_timer_cb
(
  void *throt_sm_void_ptr,
  void* cb_data
)
{
  ds_3gppi_throttle_sm_s* throt_sm_ptr;
  sys_modem_as_id_e_type  subs_id = (sys_modem_as_id_e_type)cb_data;
  uint8                   index = 0;
  uint32                  throt_tmr_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  throt_sm_ptr = (ds_3gppi_throttle_sm_s*) throt_sm_void_ptr;

  if (throt_sm_ptr != NULL)
  {
    throt_tmr_val = ds_3gpp_apn_throt_get_t3396_timer(throt_sm_ptr->apn,
                                                      throt_sm_ptr->plmn_id,   
                                                      &index,
                                                      subs_id );
  }

  return throt_tmr_val;
}
#endif /*FEATURE_DATA_REL10*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ALLOCATE_REL10_INFO
 
DESCRIPTION
  This function allocates Rel10 Info Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  2. Ret_val: FALSE if Allocation failed or entry already allocated.
              TRUE, otherwise

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the newly allocated Rel10 Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throt_rel10_info_type*
         ds_3gpp_pdn_throt_allocate_rel10_info
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_throt_rel10_info_type* rel10_info_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (ds_3gpp_pdn_throt_get_rel10_info(subs_id) == NULL)
  {
    rel10_info_ptr =
      (ds_3gpp_pdn_throt_rel10_info_type*) 
         modem_mem_alloc(sizeof(ds_3gpp_pdn_throt_rel10_info_type),
                          MODEM_MEM_CLIENT_DATA);
 
    if (rel10_info_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Could not allocate new Rel10 info entry");
    }
    else
    {
      memset(rel10_info_ptr, 0, sizeof(ds_3gpp_pdn_throt_rel10_info_type));
      if (ds_3gpp_pdn_throt_set_rel10_info(subs_id, rel10_info_ptr))
      {
        DS_3GPP_MSG0_MED("Allocated new Rel10 info entry");
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Error in allocating new Rel10 info entry");
        modem_mem_free((void*)rel10_info_ptr, MODEM_MEM_CLIENT_DATA);
        rel10_info_ptr = NULL;
      }
    }
  }
  else
  {
    DS_3GPP_MSG0_MED("Rel10 Info entry already allocated");
  }
 
  DS_3GPP_MSG2_MED("Rel10 Info Ptr: 0x%x for Subs Id: %d",
                   rel10_info_ptr, subs_id);


  return rel10_info_ptr;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_REL10_INFO
 
DESCRIPTION
  This function fetches Rel10 Info Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
 
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the Rel10 Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throt_rel10_info_type* 
         ds_3gpp_pdn_throt_get_rel10_info
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_throt_rel10_info_type *rel10_info_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return rel10_info_ptr;
  }

  rel10_info_ptr = ds_3gpp_pdn_throt_rel10_info_ptr[subs_id];

  return rel10_info_ptr;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_REL10_INFO
 
DESCRIPTION
  This function sets Rel10 Info Tbl for a particular subscription.
  
PARAMETERS
  1. Subs Id
  2. Rel10 Info ptr
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if set was done successfully
  FALSE, otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_pdn_throt_set_rel10_info
(
  sys_modem_as_id_e_type             subs_id,
  ds_3gpp_pdn_throt_rel10_info_type* rel10_info_ptr
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  ds_3gpp_pdn_throt_rel10_info_ptr[subs_id] = rel10_info_ptr;

  return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_ALLOCATE_APN_BASED_TBL
 
DESCRIPTION
  This function allocates APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  2. Ret_val: FALSE if Allocation failed or entry already allocated.
              TRUE, otherwise

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the newly allocated APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throttle_sm_apn_based_s*
         ds_3gpp_pdn_throt_allocate_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_s* apn_based_tbl_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id)== NULL)
  {
    apn_based_tbl_ptr =
      (ds_3gpp_pdn_throttle_sm_apn_based_s*) 
         modem_mem_alloc(sizeof(ds_3gpp_pdn_throttle_sm_apn_based_s),
                          MODEM_MEM_CLIENT_DATA);

    if (apn_based_tbl_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Memory allocation for APN Based Tbl Entry failed");
    }
    else
    {
      if (ds_3gpp_pdn_throt_set_apn_based_tbl(subs_id, apn_based_tbl_ptr))
      {
        DS_3GPP_MSG0_MED("Allocated new APN Based Tbl entry");
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Failure in setting APN Based TBl Ptr");
        modem_mem_free((void*)apn_based_tbl_ptr, MODEM_MEM_CLIENT_DATA);
        apn_based_tbl_ptr = NULL;
      }
    }
  }
  else
  {
    DS_3GPP_MSG0_MED("APN Based Tbl entry already allocated");
  }

  DS_3GPP_MSG2_MED("APN Based TBL Ptr: 0x%x for Subs Id: %d", 
                    apn_based_tbl_ptr, subs_id);


  return apn_based_tbl_ptr;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_GET_APN_BASED_TBL
 
DESCRIPTION
  This function fetches the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the APN based Tbl Info 
 
SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_throttle_sm_apn_based_s* 
         ds_3gpp_pdn_throt_get_apn_based_tbl
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return apn_based_tbl_ptr;
  }

  apn_based_tbl_ptr = ds_3gpp_pdn_throttle_sm_apn_based_tbl_ptr[subs_id];

  DS_3GPP_MSG2_MED("APN Based TBL Ptr: 0x%x for Subs Id: %d", 
                   apn_based_tbl_ptr, subs_id);

  return apn_based_tbl_ptr;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_SET_APN_BASED_TBL
 
DESCRIPTION
  This function sets the APN Based Tbl for a particular subscription.
  
PARAMETERS   : 
  1. Subs Id
  2. APN Based Tbl Ptr
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if set successfully
  FALSE, otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_pdn_throt_set_apn_based_tbl
(
  sys_modem_as_id_e_type               subs_id,
  ds_3gpp_pdn_throttle_sm_apn_based_s* apn_based_tbl_ptr
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  ds_3gpp_pdn_throttle_sm_apn_based_tbl_ptr[subs_id] = apn_based_tbl_ptr;

  return TRUE;
}
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
)
{
  uint8                      profile_num = 0;
  sys_modem_as_id_e_type     subs_id = SYS_MODEM_AS_ID_NONE;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Throt sm ptr is NULL");
    return;
  }

  subs_id = ds3g_get_ps_subs_id();
  /*----------------------------------------------------------------------- 
    In order to log the packet, extract the pdn context corresponding
    to the apn and get the profile number
  -----------------------------------------------------------------------*/

  DS_3GPP_MSG1_HIGH("Logging PDN throttling info:%d",throt_sm_ptr->throt_tmr_val);

  profile_num = ds_3gpp_pdn_throt_get_profile_number(throt_sm_ptr);

  ds_3gpp_pdn_throt_hdl_gen_event(throt_sm_ptr->failure_count,
                                  throt_sm_ptr->throt_tmr_val,
                                  throt_sm_ptr->is_throttled,subs_id,
                                  &throt_sm_ptr->plmn_id,pdn_pdp_type,
                                  (char *)apn,profile_num);
}

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
)
{
  uint8 loop_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (loop_index = 0;loop_index < DS_PDN_THROT_LOG_LIST_TYPE_MAX;
       loop_index++)
  {    
    if(pdn_throt_log_list[loop_index].throttle_sm_ptr == 
                                                          throt_sm_ptr)
    {
      pdn_throt_log_list[loop_index].profile_num = 0;
      pdn_throt_log_list[loop_index].throttle_sm_ptr = NULL;
      break;
    }
  }

  if (loop_index == DS_PDN_THROT_LOG_LIST_TYPE_MAX )
  {
    DS_3GPP_MSG0_ERROR("Cannot get profile number");
  }

  return;
}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_UPDATE_LOG_LIST

DESCRIPTION
  This function would update the log list from apn and ip type

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
)
{
  uint16            profile_num = 0;
  ds_pdn_context_s *pdn_context_ptr = NULL;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*----------------------------------------------------------------------------*/
  subs_id = ds3g_get_ps_subs_id();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Throt ptr is NULL");
    return;
  }

  if (pdn_pdp_type == DS_UMTS_PDP_IPV4)
  {
    if((ds_pdn_cntxt_get_v4_pdn_context(throt_sm_ptr->apn,&pdn_context_ptr,
                                        subs_id)) &&
      (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr)))
    {
      profile_num = (uint8)pdn_context_ptr->
                    ds_pdn_context_dyn_p->pdp_profile_num;
    }
  }
  else if(pdn_pdp_type == DS_UMTS_PDP_IPV6)
  {
    if((ds_pdn_cntxt_get_v6_pdn_context(throt_sm_ptr->apn,&pdn_context_ptr,
                                        subs_id)) &&
      (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr)))
    {
      profile_num = (uint8)pdn_context_ptr->
                    ds_pdn_context_dyn_p->pdp_profile_num;
    }
  }

  if (profile_num != 0)
  {
    DS_3GPP_MSG1_HIGH("Updating profile_num:%d to profile_num_list_type",
                      profile_num);
    ds_3gpp_pdn_throt_add_log_entry(throt_sm_ptr,profile_num);
  }

  return;

}


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
)
{
  uint8 loop_index = 0;
  uint16 profile_num = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (loop_index = 0;loop_index < DS_PDN_THROT_LOG_LIST_TYPE_MAX;
       loop_index++)
  {    
    if(pdn_throt_log_list[loop_index].throttle_sm_ptr == 
                                                          throt_sm_ptr)
    {
      profile_num = pdn_throt_log_list[loop_index].profile_num;
      break;
    }
  }

  if (loop_index == DS_PDN_THROT_LOG_LIST_TYPE_MAX )
  {
    DS_3GPP_MSG0_ERROR("Cannot get profile number");
  }

  return profile_num;
}

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
)
{
  ds_3gpp_pdn_throttle_event_type  *pdn_throt_ev_ptr = NULL; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------ 
    Perform necessary action for PDN THROTTLING EV LOGGING
    After generating the event, free the memory allocated from heap
  --------------------------------------------------------------------------*/
  pdn_throt_ev_ptr = ds_3gpp_pdn_throt_ev_alloc_mem();

  if (pdn_throt_ev_ptr != NULL)
  {
    memset((void *)pdn_throt_ev_ptr,0,sizeof(ds_3gpp_pdn_throttle_event_type));
    pdn_throt_ev_ptr->counter = failure_count;

    if (timer_val == DS_3GPP_PDN_THROT_INFINITE_THROT)
    {
      DS_3GPP_MSG0_HIGH("Setting log throt time to 0xFFFF");
      pdn_throt_ev_ptr->duration = 0xFFFF;
    }
    else
    {
      pdn_throt_ev_ptr->duration = (uint16)(timer_val/1000);
    }

    pdn_throt_ev_ptr->is_throttled = (is_throttled & 0x0F);
    pdn_throt_ev_ptr->subs_id = (uint8)subs_id;

   /*------------------------------------------------------------------------ 
     Check if apn is NULL, If not NULL copy only
     6 bytes even if length is greater than 6
    -----------------------------------------------------------------------*/
    if (apn == NULL)
    {
      DS_3GPP_MSG0_ERROR("APN param passed is NULL");
    }
    else
    {
      strlcpy(pdn_throt_ev_ptr->apn,apn,DS_PDN_THROT_MAX_APN_LOG_LENGTH);
    }

    ds_3gpp_extract_mcc_mnc_from_plmn_id(plmn_id,&pdn_throt_ev_ptr->mcc,
                                        &pdn_throt_ev_ptr->mnc);

    pdn_throt_ev_ptr->profile_id = (uint8)profile_id;
    pdn_throt_ev_ptr->throttle_ip_type = (throttle_ip_type & 0x0F);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Failure to allocate PDN throt ptr from heap");
    return;
  }

  ds_3gpp_pdn_throt_gen_event(pdn_throt_ev_ptr);

  modem_mem_free((void *)pdn_throt_ev_ptr, MODEM_MEM_CLIENT_DATA);

  pdn_throt_ev_ptr = NULL;
}

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
)
{
  uint8 loop_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (loop_index = 0;loop_index < DS_PDN_THROT_LOG_LIST_TYPE_MAX;
       loop_index++)
  {
    if (pdn_throt_log_list[loop_index].throttle_sm_ptr == 
        throt_sm_ptr)
    {
      DS_3GPP_MSG0_ERROR("No profile num addition, Entry already found");
      break;
    }
        
    if(pdn_throt_log_list[loop_index].throttle_sm_ptr == NULL)
    {
      pdn_throt_log_list[loop_index].throttle_sm_ptr =
        throt_sm_ptr;
      pdn_throt_log_list[loop_index].profile_num = profile_num;
      break;
    }
  }

  if (loop_index == DS_PDN_THROT_LOG_LIST_TYPE_MAX )
  {
    DS_3GPP_MSG0_ERROR("No free index to add profile_number to"
                       " pdn_throt_profile_num_list");
  }

  return;
}

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
)
{
  ds_3gpp_pdn_throttle_event_type *pdn_throt_ev_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  pdn_throt_ev_ptr = (ds_3gpp_pdn_throttle_event_type *)modem_mem_alloc
                     (sizeof(ds_3gpp_pdn_throttle_event_type),
                      MODEM_MEM_CLIENT_DATA);

  if (pdn_throt_ev_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Throt ev ptr mem cant be allocated from heap");
  }

  return pdn_throt_ev_ptr;
}

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (pdn_throt_ev_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("pdn_throt_ev_ptr passed is unallocated");
    return;
  }

  DS_3GPP_MSG0_MED("Generating PDN THROT EV MSG");

  event_report_payload(EVENT_DS_3GPP_PDN_THROTTLE,
                      sizeof(ds_3gpp_pdn_throttle_event_type),
                      (void*)pdn_throt_ev_ptr);
}

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
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_MED("Clearing apn reject throttle timer");

  (void)rex_clr_timer(&(ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn_reject_sm_timer));

}

#ifdef FEATURE_DATA_LTE

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
)
{
  
  sys_modem_as_id_e_type  subs_id =  ds3g_get_ps_subs_id();;

  ds_3gpp_pdn_throt_apn_rej_sm_init_per_subs_id(subs_id);


}

/*===========================================================================
  FUNCTION ds_3gpp_pdn_throt_sm_fill_rej_sm_apn_info

  DESCRIPTION
  This function initializes the apn reject throttling information per
  subscription once its extracted from EFS.
  
  PARAMETERS
  efs_info_q_ptr - Queue containing extracted EFS info
  subs_id - subscription_id 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_throt_sm_fill_rej_sm_apn_info
(
  q_type                 *efs_info_q_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gppi_utils_extracted_info_type       *extracted_info_block_ptr = NULL;
  ds_3gppi_utils_extracted_info_type       *prev_info_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id) ||
     NULL == efs_info_q_ptr)
  {
    DS_3GPP_MSG2_ERROR("Invalid Input Params, subs: %d EFS queue: 0x%x",
                       subs_id, efs_info_q_ptr);
    return;
  }

  extracted_info_block_ptr = q_check(efs_info_q_ptr);

  while(NULL != extracted_info_block_ptr)
  {
    if(NULL == ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn)
    {
      ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn = (byte *)ds_system_heap_mem_alloc
        (extracted_info_block_ptr->extracted_data_length+1);

      if(NULL != ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn)
      {
        memset(ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn, 
               0, 
               extracted_info_block_ptr->extracted_data_length);

        strlcpy((char *)ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn, 
                extracted_info_block_ptr->extracted_data,
                extracted_info_block_ptr->extracted_data_length+1);
      }
      else
      {
        DATA_ERR_FATAL("Heap Exhaustion, Memory allocation failed !!!");
        return;
      }
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Memory already allocated, Not re-writing APN name");
    }

    prev_info_block_ptr = extracted_info_block_ptr;
    extracted_info_block_ptr = q_next(efs_info_q_ptr, 
                                      &(extracted_info_block_ptr->link));
    q_delete(efs_info_q_ptr, &(prev_info_block_ptr->link));

    modem_mem_free(prev_info_block_ptr->extracted_data, MODEM_MEM_CLIENT_DATA);
    modem_mem_free(prev_info_block_ptr, MODEM_MEM_CLIENT_DATA);
    prev_info_block_ptr = NULL;
  }
}
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
)
{
  boolean result = FALSE;
  char efs_file_name[] 
          = "/nv/item_files/modem/data/3gpp/ps/apn_reject/apn_reject_name.txt";
  q_type  efs_info_q;
  char compare_string[] = "reject_apn";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void)ds3gsubsmgr_is_subs_id_valid(subs_id);

  if (ds_3gpp_pdn_throt_apn_reject_sm[subs_id].is_initialized)
  {
    DS_3GPP_MSG1_HIGH("ds_3gppi_utils_read_reject_apn_from_efs "
                      "already initialized for  subs ID %d",subs_id);
   	return;
  }

  memset(&efs_info_q, 0, sizeof(q_type));
  q_init(&efs_info_q);
  memset((void *)&ds_3gpp_pdn_throt_apn_reject_sm[subs_id],0,
         sizeof(ds_3gpp_pdn_throt_apn_reject_sm_s));

 /*----------------------------------------------------------------------- 
    Read from EFS to populate the cache, Memory for apn will also be
    allocated in ds_read_reject_apn_from_efs if result returned is true.
 ----------------------------------------------------------------------*/
  result = ds_3gppi_utils_read_apn_info_from_efs((char *)compare_string,
                                                 (char *)efs_file_name,
                                                  &efs_info_q,
                                                  subs_id);

  DS_3GPP_MSG1_HIGH("ds_3gppi_utils_read_reject_apn_from_efs returned %d",
                    result);

  /*------------------------------------------------------------------------ 
  Define the apn_reject_sm timer
  ------------------------------------------------------------------------*/
  if (result)
  {
    DS_3GPP_MSG0_MED("Defining the apn_reject_sm timer");

    rex_def_timer_ex(
       &(ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn_reject_sm_timer),
       (rex_timer_cb_type)ds_3gpp_pdn_throt_apn_reject_t3402_timer_expiry,
       (unsigned long)subs_id
                    );

    ds_3gpp_pdn_throt_sm_fill_rej_sm_apn_info(&efs_info_q, subs_id);
  }

  ds_3gpp_pdn_throt_apn_reject_sm[subs_id].is_initialized = TRUE; 
  q_destroy(&efs_info_q);

  return;

}
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
)
{
  (void)ds3gsubsmgr_is_subs_id_valid(subs_id);

  if( ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn != NULL)
  {
    ds_system_heap_mem_free(ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn);
  }
  rex_clr_timer(&ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn_reject_sm_timer);

  ds_3gpp_pdn_throt_apn_reject_sm[subs_id].is_initialized = FALSE;

}

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
)
{
  uint32 ds_t3402_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((ds_t3402_val = ds_eps_pdn_cntxt_get_t3402_timer_val()) > 0)
  {
    DS_3GPP_MSG1_HIGH("ds_t3402 timer val is %d",ds_t3402_val);

    /*------------------------------------------------------------------------ 
      Start the T3402 timer 
    -------------------------------------------------------------------------*/
    (void)rex_set_timer(&(ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn_reject_sm_timer), 
                          ds_t3402_val);
  }

  return;

}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_APN_REJECT_T3402_TIMER_EXPIRY

DESCRIPTION
  This function posts a cmd to reset the apn reject sm on timer expiry

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_pdn_throt_apn_reject_t3402_timer_expiry
(
  unsigned long callback_data /*unused*/
)
{
  ds_cmd_type *cmd_buf = NULL;
  unsigned long *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_apn_reject_t3402_timer_expiry()");

  cmd_buf = ds_allocate_cmd_buf(sizeof(unsigned long));
  if(cmd_buf == NULL )
  {
    ASSERT(0);
    return;
  }

  cmd_buf->hdr.cmd_id = DS_CMD_3GPP_PDN_THROT_CLEAR_APN_REJ_T3402_TMR;
  data_ptr = (unsigned long*)cmd_buf->cmd_payload_ptr;
  *data_ptr = callback_data;


  ds_put_cmd(cmd_buf);

  return;
}

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
)
{
  boolean result = FALSE;
  sys_plmn_id_s_type      current_plmn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_apn_reject_t3402_timer_expiry_cmd_hdlr()");

  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn, subs_id);


  result = ds_3gpp_pdn_throt_set_apn_reject_pdn_blocked(FALSE,subs_id);
  
  if(result == FALSE)
  {
    DS_3GPP_MSG0_ERROR ("Unable to reset Throttle SM after Timer Expiry");
  }

  ds_3gpp_pdn_throt_hdl_gen_event(DS_PDN_THROT_LOG_PARAM_NA,
                                  0,
                                  FALSE,subs_id,&current_plmn,
                                  DS_PDN_THROT_LOG_PARAM_NA,
                                 (char *)ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn,
                                  ds_3gpp_pdn_throt_apn_reject_sm[subs_id].profile_num);

  return;
}

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
)
{
  DS_3GPP_MSG0_MED ("Trying to get Status for Blocked PDN flag in Reject SM");
  return ds_3gpp_pdn_throt_apn_reject_sm[subs_id].block_pdn;
}

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
  sys_modem_as_id_e_type    subs_id
)
{
  uint32                            remaining_timer_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - */

  remaining_timer_val = (uint32)rex_get_timer(&(ds_3gpp_pdn_throt_apn_reject_sm[subs_id].
                                       apn_reject_sm_timer));
  DS_3GPP_MSG1_MED("Remaining Timer value for PDN throttle is %d",
                    remaining_timer_val);
  return remaining_timer_val;
}

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
)
{
   if (ds_3gpp_pdn_throt_validate_apn_reject_name(subs_id) == TRUE)
   {
     strlcpy (apn_name, (char *)ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn,
              DS_SYS_MAX_APN_LEN);
     DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_MED, "APN: %s is throttled",apn_name);
   }
   else
   {
     DS_3GPP_MSG0_ERROR ("ds_3gpp_pdn_throt_apn_reject_get_apn_name: APN Name"
                         " Invalid");
   }
}

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
        1. Globally blocked (due to T3396) or
        3. Blocked due to APN being disabled 
  FALSE, otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_throt_check_if_block_plmn_list_needs_update
(
  sys_modem_as_id_e_type           subs_id
)
{
  uint8                                    apn_index  = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  boolean                                  attach_apn_found = FALSE;
  boolean                                  block_plmn_list_needs_update = TRUE; 
  ds_3gpp_pdn_throttle_sm_type             sm_type = 
                                            DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED;
  /*----------------------------------------------------------------*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);

  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return FALSE;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

    if ((apn_index_ptr != NULL) && (apn_index_ptr->valid == TRUE))
    {
      if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)apn_index_ptr->apn))
      {
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                                  "APN: %s is attach APN",apn_index_ptr->apn);

        attach_apn_found = TRUE;

        if(apn_index_ptr->globally_blocked == FALSE &&
           apn_index_ptr->apn_disabled == FALSE && 
           apn_index_ptr->blocked_due_to_roaming == FALSE &&
           apn_index_ptr->ui_data_disabled == FALSE)
        {
          DS_3GPP_MSG0_HIGH("APN is not blocked globally / "
                            "Not disabled / Not blocked due to roaming");
          block_plmn_list_needs_update = FALSE;
          break;
        }
        else
        {
          if (ds_3gpp_pdn_throt_get_sm_type 
                (apn_index_ptr->apn, subs_id, TRUE, &sm_type))
          {
            if ((sm_type == DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT || 
                 sm_type == DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME))
            {
              DS_3GPP_MSG0_HIGH("Global throttling not due to t3396");
              block_plmn_list_needs_update = FALSE;
            }
          } 
        }
      }
      else
      {
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                               "APN: %s is NOT attach APN",apn_index_ptr->apn);
      }
    }
  }

  DS_3GPP_MSG1_HIGH("Block PLMN List needs an update: %d", 
                    (attach_apn_found && block_plmn_list_needs_update) );

  return (attach_apn_found && block_plmn_list_needs_update);

}

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
   uint8                     profile_number,
   sys_modem_as_id_e_type    subs_id
)
{
  boolean result = FALSE;
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  result = ds_3gpp_pdn_throt_validate_apn_reject_name(subs_id);

  if (!result)
  {
    DS_3GPP_MSG0_HIGH("APN reject EFS unset");
    return ret_val;
  }

  ds_3gpp_pdn_throt_apn_reject_sm[subs_id].profile_num = profile_number;

  DS_3GPP_MSG1_HIGH("APN reject SM profile number is set to %d",
                    ds_3gpp_pdn_throt_apn_reject_sm[subs_id].profile_num);

  ret_val = TRUE;
  return ret_val;
}

#endif /* FEATURE_DATA_LTE */


/*===========================================================================
FUNCTION  DS_3GP_PDN_THROT_VALIDATE_APN_REJECT_EFS

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
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if((ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn == NULL) ||
    (strlen((char*)ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn) == 0))
  {
    DS_3GPP_MSG0_HIGH("APN reject APN is not set in EFS");
    return ret_val;
  }

  ret_val = TRUE;
  return ret_val;

}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_SET_APN_REJECT_PDN_BLOCKED

DESCRIPTION
  This function sets PDN _BLOCKED in the apn_reject sm

PARAMETERS
   boolean set_val            
 
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
)
{
  boolean ret_val = FALSE;
  boolean result = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (set_val)
  {
    result = ds_3gpp_pdn_throt_validate_apn_reject_name(subs_id);

    if (!result)
    {
      DS_3GPP_MSG0_HIGH("APN reject EFS unset");
      return ret_val;
    }
  }

  ds_3gpp_pdn_throt_apn_reject_sm[subs_id].block_pdn = set_val;

  DS_3GPP_MSG1_HIGH("APN reject SM is set to %d",
                    ds_3gpp_pdn_throt_apn_reject_sm[subs_id].block_pdn);

  ret_val = TRUE;

  ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm(subs_id);

  return ret_val;
}

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
)
{
  uint8                                     plmn_loop_index = 0;
  uint8                                     apn_loop_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s   *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_s                 *per_plmn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s       *apn_based_tbl_ptr;
  ps_iface_net_down_reason_type             v4_failure_cause_code = 
                                               PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ps_iface_net_down_reason_type             v6_failure_cause_code = 
                                               PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds_3gpp_pdn_throttle_sm_info_s            *pdn_throt_sm_info_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);
  
  if (apn_based_tbl_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
    return;
  }
  DS_3GPP_MSG0_HIGH("In ds_3gpp_pdn_throt_reset_pdn_throttle_if_required");
  
  for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {
    per_apn_throt_sm_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];
    if(per_apn_throt_sm_ptr != NULL)
    {
      for(plmn_loop_index = 0; plmn_loop_index < DS_3GPP_MAX_PLMN_SUPPORTED; 
          plmn_loop_index++)
      {
        pdn_throt_sm_info_p = per_apn_throt_sm_ptr->ds_3gpp_pdn_throttle_tbl
        [plmn_loop_index];

        if (pdn_throt_sm_info_p != NULL)
        {
          per_plmn_throt_sm_ptr = 
            &(pdn_throt_sm_info_p->per_rat[DS_3GPP_THROT_RAT_GLOBAL]);
        }
        else
        {
          per_plmn_throt_sm_ptr = NULL;
        }

        if(per_plmn_throt_sm_ptr != NULL &&
          TRUE ==  ds_3gpp_pdn_throt_get_prev_cause_code(per_plmn_throt_sm_ptr,
                                                        &v4_failure_cause_code,
                                                        &v6_failure_cause_code))
        {
    
          if(v4_failure_cause_code == 
                             PS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN)
          { 
            /*---------------------------------------------------------------------
               Check if V4 throttling timer is set to infinite throttling and userdata was set to 
               PS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN
               If set, Call clear_throttle_state_function to clear throttling_flag,
               timer_val, cause_code and failure_count.
               For Release 11; we always clear throttling state for both V4V6
             ----------------------------------------------------------------------*/          
            DS_3GPP_MSG3_HIGH("Resetting Infite throttling for V4V6 throttle SM, "
                              "APN index:%d, PLMN index:%d", apn_loop_index,
                              plmn_loop_index,0);
            ds_3gpp_pdn_throt_clear_throttle_state(per_apn_throt_sm_ptr->apn,
                                                   DS_UMTS_PDP_IPV4V6,
                                                   subs_id,
                                                   DS_3GPP_THROT_RAT_MASK_GLOBAL);                                                  

          }
        }  // per apn throttle sm ptr
      }
   
    }
  }

}/* ds_3gpp_pdn_throt_reset_pdn_throttle_if_required */

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_CURRENT_SYS_MODE

DESCRIPTION
  This Function sets the throttle_sys_mask to the mode which is passed.

PARAMETERS: 
  ds_3gpp_pdn_throttle_sm_s               *pdn_throt_sm_ptr,
  ds_umts_pdp_type_enum_type               pdn_pdp_type,
  sys_sys_mode_e_type                      sys_mode
 
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
)
{
  if(pdn_pdp_type == DS_UMTS_PDP_IPV4V6 && pdn_throt_sm_ptr != NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
    pdn_throt_sm_ptr->v4_throttle_sm->throttle_sys_mask |= sys_mode;
    pdn_throt_sm_ptr->v6_throttle_sm->throttle_sys_mask |= sys_mode;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("NULL argument passed in ds_3gppi_throt_sm_set_current_sys_mode"
                       "Ignore");
  }
} /* ds_3gppi_throt_sm_set_current_sys_mode */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_LTE_HANDLE_NAS_REJECT_WITH_T3396_IE 
 
DESCRIPTION
  This function handles the reject message received from NAS with a t3396 IE
  based on Release 11 requirements

PARAMETERS   : 
  cm_lte_call_info_s_type              *lte_call_info,
  ds_pdn_context_s                     *pdn_context_p,
  ps_iface_net_down_reason_type        down_reason

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
)
{
  byte                    decoded_apn[DS_UMTS_MAX_APN_STRING_LEN];
  sys_modem_as_id_e_type  subs_id  = SYS_MODEM_AS_ID_NONE;
  byte                    *apn = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return;
  }

  if ((lte_call_info->backoff_timer.active == TRUE) &&
      (lte_call_info->backoff_timer.timer_count != 0) )
  {
     DS_LTE_MSG0_HIGH("call_end cause code is release 11 specific,reset"
                       "throt_req_flag");
     /*---------------------------------------------------------------- 
       Perform global APN throttling for these cause codes. Create a 
       PDN throt SM exclusively for globally throttling the given APN
       across all PLMNs.
     ------------------------------------------------------------------*/
    if(lte_call_info->pdn_reject_ind.access_point_name.valid)
    {
      apn = lte_call_info->pdn_reject_ind.access_point_name.address;
      memset(decoded_apn,0x0,DS_UMTS_MAX_APN_STRING_LEN);

      if( (apn != NULL) && 
         (TRUE == ds_3gpp_pdn_context_decode_apn_name_non_strict(apn,
                      (uint32)lte_call_info->pdn_reject_ind.\
                              access_point_name.apn_addr_len,
                       decoded_apn) ))
      {
        apn = decoded_apn;
      }
      else
      {
        DS_LTE_MSG0_HIGH("Unable to decode APN or NULL APN, use profile apn");
        apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
      }
    }
    else
    {
      apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
      DS_LTE_MSG0_HIGH("APN name is not valid in reject ind, use Profie APN");          
    }
       
    if( apn != NULL)
    {
      DATA_LTE_MSG_SPRINTF_1(
        MSG_LEGACY_HIGH,"APN being throttled: %s",(char*)apn);
    }
    else
    {
      DS_LTE_MSG0_HIGH("NULL APN !!");
    }
    ds_3gpp_pdn_throt_lte_handle_reject_with_t3396_ie(lte_call_info,
                                                      apn,
                                                      down_reason,
                                                      subs_id,
                                                      pdn_context_p);
  }
}/* ds_3gpp_pdn_throt_lte_handle_nas_reject_with_t3396_ie */
#endif /* FEATURE DATA LTE*/

/*===========================================================================
FUNCTION  DS_3GPP_PDN_THROT_CLEAR_IS_THROTTLE_FLAG_AND_TIMER

DESCRIPTION
  Stop T3396 timer for v4 and v6, if it is already running and reset 
  the throttling flag to false.

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
)
{
  ds_3gpp_pdn_throt_rel10_info_type   *rel11_info_ptr = NULL;
  uint8                                index = DS_MAX_APN_PLMN_SUPPORTED;
/*----------------------------------------------------------------*/
  rel11_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel11_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }

  (void)ds_3gpp_apn_throt_get_t3396_timer (apn, current_plmn, &index, subs_id);

  if (index >= DS_MAX_APN_PLMN_SUPPORTED)
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Entry not found for APN: %s",
                            (char*)apn);
    return;
  }

  if(!ds_3gpp_pdn_throt_apn_throt_t3396_validate(
              &(rel11_info_ptr->t3396_timer_tbl[index])))
  {
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Invalid Apn Table Entry: %s",
                            (char*)apn);
    return;
  }
     /*------------------------------------------------------------------
       Clear throttle timer if t3396 is already running.
     ------------------------------------------------------------------*/
    if( (TRUE == 
      ds_3gppi_throt_sm_get_is_throttled_flag(pdn_throt_sm_ptr->v4_throttle_sm)) && 
      (ds_3gppi_throt_sm_get_remaining_throttle_time(pdn_throt_sm_ptr->v4_throttle_sm) > 0) )
    {
      DS_3GPP_MSG0_HIGH("Since Back-off timer already running for the APN"
                       "Clearing the timer value");
      ds_3gppi_throt_sm_clear_throttle_timer(pdn_throt_sm_ptr->v4_throttle_sm, 
                                               subs_id);
    }
    if( (TRUE == 
      ds_3gppi_throt_sm_get_is_throttled_flag(pdn_throt_sm_ptr->v6_throttle_sm)) &&
      (ds_3gppi_throt_sm_get_remaining_throttle_time(pdn_throt_sm_ptr->v6_throttle_sm) > 0) )
    {
      DS_3GPP_MSG0_HIGH("Since Back-off timer already running for the APN"
                           "Clearing the timer value");
      ds_3gppi_throt_sm_clear_throttle_timer(pdn_throt_sm_ptr->v6_throttle_sm, 
                                              subs_id);
    }

    ds_3gpp_pdn_throt_sm_set_sm_type(pdn_throt_sm_ptr,
                                     DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED);

}/* ds_3gpp_pdn_throt_clear_is_throttle_flag_and_timer */

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
)
{
  uint8                              index = 0;
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throt_rel10_info_type *rel10_info_ptr  = NULL;
 /*-----------------------------------------------------------------------*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }

  for(index = 0;index < DS_MAX_APN_PLMN_SUPPORTED; index++)
  {
    if(!ds_3gpp_pdn_throt_apn_throt_t3396_validate(&(rel10_info_ptr->t3396_timer_tbl[index])))
    {
      DS_3GPP_MSG0_MED("Invalid apn table entry");
      continue;
    }

    /*------------------------------------------------------------------------- 
      Get the pdn throt sm ptr for the stored PLMN ID
      -------------------------------------------------------------------------*/
    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn(
                                  rel10_info_ptr->t3396_timer_tbl[index]. 
                                  throt_info_t3396_timer_type_dyn_p->plmn_id, 
                                  rel10_info_ptr->t3396_timer_tbl[index].
                                  throt_info_t3396_timer_type_dyn_p->apn_name,
                                  subs_id); 

    /*-------------------------------------------------------------------------- 
      Only care about V4 throttling sm because for rel 10 throttling V4 and V6
      will both be populated. 
     --------------------------------------------------------------------------*/

    if((pdn_throt_sm_ptr == NULL) || (pdn_throt_sm_ptr->v4_throttle_sm == NULL))
    {
      continue;
    }

    DS_3GPP_MSG1_HIGH("Clearing T3396 timer for index:%d",index);
    ds_3gpp_pdn_throt_unblock_apn_timer_exp_cmd_hdlr
                                  ((void *)pdn_throt_sm_ptr->v4_throttle_sm,
                                    subs_id);
  }

  return;
}/* ds_3gpp_pdn_throt_clear_t3396_entry*/

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
)
{
  ASSERT(throt_ident_info_p != NULL);

  throt_ident_info_p->apn = apn;
  throt_ident_info_p->sm_type = sm_type;
  throt_ident_info_p->subs_id = subs_id;
  throt_ident_info_p->globally_blocked = globally_blocked;
  throt_ident_info_p->rat_type = rat_type;
}

ds_3gpp_throt_rat_e_type ds_3gpp_pdn_throt_get_next_rat_type
(
  uint32  *rat_mask_p
)
{
  ds_3gpp_throt_rat_e_type rat_value = DS_3GPP_THROT_RAT_ILLEGAL;
  /*-------------------------------------------------------------------*/

  if (rat_mask_p == NULL)
  {
    ASSERT(0);
    return rat_value;
  }

 
  if (*rat_mask_p & DS_3GPP_THROT_RAT_MASK_GLOBAL)
  {
    rat_value = DS_3GPP_THROT_RAT_GLOBAL;
    *rat_mask_p &= ~DS_3GPP_THROT_RAT_MASK_GLOBAL;
  }
  else if (*rat_mask_p & DS_3GPP_THROT_RAT_MASK_LTE )
  {
    rat_value = DS_3GPP_THROT_RAT_LTE;
    *rat_mask_p &= ~DS_3GPP_THROT_RAT_MASK_LTE;
  }
  else if (*rat_mask_p & DS_3GPP_THROT_RAT_MASK_UMTS)
  {
    rat_value = DS_3GPP_THROT_RAT_UMTS;
    *rat_mask_p &= ~DS_3GPP_THROT_RAT_MASK_UMTS;
  }

  return rat_value;
  
} /* ds_3gpp_pdn_throt_get_next_rat_type */

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
)
{
  sys_sys_mode_e_type mode = SYS_SYS_MODE_NONE;
  /*--------------------------------------------------------------------*/

  /*--------------------------------------------------------------------- 
    If the current mode is LTE return it (regardless of whether it is
    full service or not)
    Otherwise return the last 3GPP mode in full service
  ---------------------------------------------------------------------*/

  mode = ds3gpp_get_current_network_mode(subs_id);

  if (mode != SYS_SYS_MODE_LTE)
  {
    mode = ds3gpp_get_last_full_srv_mode(subs_id);
  }

  return mode;
} /* ds_3gpp_pdn_throt_determine_mode */

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_CLEAR_T3396_ON_USIM_REMOVAL

DESCRIPTION
  This function clears imsi_info_avail flag so that when card is inserted 
  again, IMSI can be compared and t3396 throttling can be cleared if needed.
  if clear throttling flag is set, T3396 timers for corresponding APNs are
  cleared. Permanent throttling for T3396 is also cleared.

PARAMETERS
  subs_id
  clear_throttling

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
)
{
  uint8                              index = 0;
  ds_3gpp_pdn_throttle_sm_s         *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throt_rel10_info_type *rel10_info_ptr  = NULL;
  uint8                                    apn_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr;
 /*-----------------------------------------------------------------------*/

  rel10_info_ptr = ds_3gpp_pdn_throt_get_rel10_info(subs_id);

  if (rel10_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rel10_info_ptr is null");
    return;
  }

  /*----------------------------------------------------------------------
    Only clear the imsi_info_avail
  -----------------------------------------------------------------------*/
  rel10_info_ptr->imsi_data_avail = FALSE;

  if (TRUE == clear_throttling)
  {
    for(index = 0;index < DS_MAX_APN_PLMN_SUPPORTED; index++)
    {
      if( !ds_3gpp_pdn_throt_apn_throt_t3396_validate(
                                      &(rel10_info_ptr->t3396_timer_tbl[index])) )   
      {
        continue;
      }
  
      /*------------------------------------------------------------------------- 
        Get the pdn throt sm ptr for the stored PLMN ID
      -------------------------------------------------------------------------*/
      pdn_throt_sm_ptr = 
          ds_3gpp_pdn_throt_sm_get_throttle_sm_from_plmn(
                                  rel10_info_ptr->t3396_timer_tbl[index].
                                  throt_info_t3396_timer_type_dyn_p->plmn_id, 
                                  rel10_info_ptr->t3396_timer_tbl[index].
                                  throt_info_t3396_timer_type_dyn_p->apn_name,
                                  subs_id); 
  
      /*-------------------------------------------------------------------------- 
        Only care about V4 throttling sm because for rel 10 throttling V4 and V6
        will both be populated. For non-T3396 SM types we don't want to clear 
        throttling here.
      --------------------------------------------------------------------------*/ 
      if((pdn_throt_sm_ptr == NULL) || 
         (pdn_throt_sm_ptr->v4_throttle_sm == NULL) ||
         (DS_3GPP_THROTTLE_TYPE_T3396 != pdn_throt_sm_ptr->sm_type))
      {
        continue;
      }
  
      DS_3GPP_MSG1_HIGH("Clearing T3396 timer for index:%d",index);
      ds_3gpp_pdn_throt_unblock_apn_timer((void *)pdn_throt_sm_ptr->v4_throttle_sm,
                                          subs_id);
    }

     /*------------------------------------------------------------------------ 
       Clear throttling for the case where APN is blocked permanently because
       T3396 timer was inactive
     ------------------------------------------------------------------------*/
     apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl(subs_id);
	
     if (apn_based_tbl_ptr == NULL)
     {
       DS_3GPP_MSG0_ERROR("apn_based_tbl_ptr is NULL");
       return;
     }

     for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
     {
       apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];

       if( (NULL != apn_index_ptr) && 
           (TRUE == apn_index_ptr->valid))
       {
         if (apn_index_ptr->globally_blocked)
         {
           pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm(
                       apn_index_ptr->apn, 
                       TRUE, /* globally blocked */
                       FALSE, /* don't create */
                       DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, 
                       subs_id);

           if ((NULL != pdn_throt_sm_ptr) &&
               (pdn_throt_sm_ptr->sm_type == DS_3GPP_THROTTLE_TYPE_T3396))
           {
             DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Clearing Global Throttle"
                                     " state for APN: %s",
                                    (char*)apn_index_ptr->apn);
             ds_3gpp_apn_throt_clear_t3396_tmr_entry(apn_index_ptr->apn,
                                                     global_plmn_id,
                                                     subs_id);

             /* Clear the PDN throttle information for this APN */
             ds_3gpp_pdn_throt_clear_throttle_state(apn_index_ptr->apn,
                                                    DS_UMTS_PDP_IPV4V6,
                                                    subs_id,
                                                    DS_3GPP_THROT_RAT_MASK_GLOBAL);
           }
         }
       }
     }
  }
  return;
}/* ds_3gpp_pdn_throt_clear_t3396_on_usim_removal */
