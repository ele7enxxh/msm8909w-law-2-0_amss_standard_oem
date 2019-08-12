/******************************************************************************
  @file    ds_profile_3gpp2_mgr.c
  @brief

  DESCRIPTION
  Tech specific implementation of 3GPP2 Profile Management

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsprofile/src/ds_profile_3gpp2_mgr.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/28/14   hr      Sim swap feature
04/14/14   hr      Add support for common PDN and uniform callback operations
03/19/14   hr      Fix APN enabled flag default value 
12/18/13   hr      Created the module. First version of the file.
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "ds_profile_db_internal.h"
#include "ds_profile_db.h"
#include "ds_profile_3gpp2_mgr.h"
#include "ds707_data_session_profile.h"
#include "ds_profile_3gpp2_utils.h"
#include "data_msg.h"
#include "ds_profile_3gpp2i.h"
#include "ds_profile.h"
#include "dstask_v.h"
#include "ds_profile_db_handler.h"
#include "ds_profile_tech_common.h"
//#include "ds3gmgr.h"

/*===========================================================================
                          GLOBAL VARIALBES DEFINTIONS
===========================================================================*/
/*------------------------------------------------------------------------ 
  Keep track of the callback ID in case deregister ev callback is needed
 -------------------------------------------------------------------------*/
static ds_profile_db_callback_id_type ds_profile_3gpp2_mgr_callback_id = 0;

/*===========================================================================
                         INTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_POWER_UP_EV_HDLR

DESCRIPTION   Handles power up event from profile DB.

PARAMETERS    profiles       : array of profile DB managed profile ptrs
              profiles_count : size of the array

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_power_up_ev_hdlr
(
  ds_profile_db_profile_managed_type   **profiles,
  uint16                                 profiles_count,
  ds_profile_db_subs_type                subs_id
)
{
  uint16                                 index = 0;
  boolean                                is_def_prf_passed = FALSE;
  boolean                                is_mand_apn_chk = FALSE;
  ds_profile_db_gen_params_type          gen_params;
  ds_profile_db_3gpp2_tech_params_type   tech_params;
  ds_profile_db_common_tech_params_type *common_params = NULL;
  ds707_data_session_profile_info_type  *data_sess_profile = NULL;
  ds707_data_session_acl_info_type      *ds707_acl_info = NULL;
#ifdef FEATURE_EHRPD
  ds_cmd_type                           *cmd_ptr = NULL;             
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( profiles != NULL ) 
  {
    for ( index=0; index < profiles_count; index++ )
    {
      /*-------------------------------------------------------------- 
        Retrieve all required profile info in order to copy
       --------------------------------------------------------------*/
      if( FALSE == 
            ds_profile_3gpp2_util_get_params(profiles[index],
                                             &gen_params,
                                             &tech_params ) )
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "Fail to get required params or changed idents "
                         "from DB, skip %d th profile in array",
                         index );
        continue;
      }

      if (gen_params.subscription_id != subs_id)
      {
        DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                         "Profile Subs ID %d and event Subs ID %d mismatch",
                         gen_params.subscription_id,
                         subs_id );
        continue;
      }

      /*-------------------------------------------------------------- 
        Get profile entry based on the profile type. For eHRPD profile,
        new entry has to be allocated during power up.
       --------------------------------------------------------------*/
      if( gen_params.profile_number == DATA_SESSION_DEFAULT_PROFILE )
      {
        if( subs_id != DB_PROFILE_ACTIVE_SUBSCRIPTION_1 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_MED,
                           "Ignore default profile on non-default subs" );

          continue;
        }

        is_def_prf_passed = TRUE;
        
        if(gen_params.persistent == FALSE )
        {
          DATA_IS707_MSG0( MSG_LEGACY_MED,
                           "Default profile is not modified, skip copying" );

          /* Default profile is 3GPP2 only, no need to free common params */

          continue;
        }

        if( TRUE != ds707_data_session_get_profile(
                      DATA_SESSION_DEFAULT_PROFILE, 
                      (ds_profile_subs_id_enum_type)gen_params.subscription_id,
                      &data_sess_profile ) )
        {
          ASSERT(0);
          continue;
        }
      }
      /*---------------------------------------------------------------------
        Remove check eHRPD, make sure profile DB table and cache are in sync
       --------------------------------------------------------------------*/
      else
      {             
        (void)ds707_data_session_alloc_cache_entry(
                &data_sess_profile,
                (ds_profile_subs_id_enum_type)gen_params.subscription_id);
      }

      /*----------------------------------------------------------- 
       Populate cache values with EFS profile values provided by DB
       -----------------------------------------------------------*/
      ds_profile_3gpp2_util_copy_profile_and_set_cust_mask(profiles[index],
                                                           &is_mand_apn_chk,
                                                           &gen_params,
                                                           &tech_params,
                                                           &common_params,
                                                           data_sess_profile);

      DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                       "ds_profile_3gpp2_mgr_power_up_ev_hdlr: "
                       "Copied profile %d from DB EFS on subs %d",
                       gen_params.profile_number, gen_params.subscription_id );

    } /*for all profiles passed by DB from EFS*/
  } /*if profiles not NULL*/
  
  if( ( subs_id == DB_PROFILE_ACTIVE_SUBSCRIPTION_1 ) &&
      (( profiles == NULL ) || ( is_def_prf_passed == FALSE )) )
  {
    /*----------------------------------------------------------------- 
      Profile DB should always pass default profile back to MH even if
      it might not exist in EFS. Assert here.
     -----------------------------------------------------------------*/
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------- 
    Initialize ds707_acl_info
   --------------------------------------------------------------------*/
  ds707_acl_info = ds707_data_session_get_acl_info_ptr();
  ds707_acl_info->data_session_profile_init = TRUE;

#ifdef FEATURE_EHRPD
  if( ds707_acl_info->ds3g_mmgsdi_init == TRUE )
  {
    /*-------------------------------------------------------------------------
      Send command to DS task to check if there is any change to mandatory APNs
    -------------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
    if(NULL == cmd_ptr)
    {
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_MANDATORY_APN_CHECK;
    ds_put_cmd( cmd_ptr );
  }
#endif /* FEATURE_EHRPD */

}/* ds_profile_3gpp2_mgr_power_up_ev_hdlr */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_CREATE_PROFILE_EV_HDLR

PARAMETERS    profiles       : array of profile DB managed profile ptrs
              profiles_count : size of the array

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_create_profile_ev_hdlr
(
  ds_profile_db_profile_managed_type   **profiles,
  uint16                                 profiles_count,
  ds_profile_db_subs_type                subs_id
)
{
  uint16                                 index = 0;
  boolean                                is_mand_apn_chk = FALSE;
  ds_profile_db_gen_params_type          gen_params;
  ds_profile_db_3gpp2_tech_params_type   tech_params;
  ds_profile_db_common_tech_params_type *common_params = NULL;
  ds707_data_session_profile_info_type  *data_sess_profile = NULL;
#ifdef FEATURE_EHRPD
  ds_cmd_type                           *cmd_ptr = NULL;             
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(profiles == NULL)
  {
    return;
  }

  for (index = 0; index < profiles_count; index++)
  {
    /*-------------------------------------------------------------- 
      Retrieve all required profile info in order to copy
     --------------------------------------------------------------*/
    if( FALSE == 
          ds_profile_3gpp2_util_get_params(profiles[index],
                                           &gen_params,
                                           &tech_params ) )
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "Fail to get required params or changed idents "
                       "from DB, skip %d th profile in array",
                       index );
      continue;
    }

    if (gen_params.subscription_id != subs_id)
    {

      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Profile Subs ID %d and event Subs ID %d mismatch",
                       gen_params.subscription_id,
                       subs_id );
      continue;
    }

    /*---------------------------------------------------------------------
      Remove check eHRPD, make sure profile DB table and cache are in sync
      Create a new entry
     --------------------------------------------------------------------*/
    (void)ds707_data_session_alloc_cache_entry(
            &data_sess_profile,
            (ds_profile_subs_id_enum_type)gen_params.subscription_id);

    /*----------------------------------------------------------- 
     Populate Cache values with values provided by DB
     -----------------------------------------------------------*/
    ds_profile_3gpp2_util_copy_profile_and_set_cust_mask(profiles[index],
                                                         &is_mand_apn_chk,
                                                         &gen_params,
                                                         &tech_params,
                                                         &common_params,
                                                         data_sess_profile);

    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "ds_profile_3gpp2_mgr_create_profile_ev_hdlr: "
                     "Profile %d created on subs %d",
                     gen_params.profile_number, gen_params.subscription_id );

  } /*for all profiles passed by DB*/

#ifdef FEATURE_EHRPD
  if( is_mand_apn_chk == TRUE )
  {
    /*---------------------------------------------------------------------
      Send command to DS task to check if there is any change to mandatory APNs
    ---------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
    if(NULL == cmd_ptr)
    {
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_MANDATORY_APN_CHECK;
    ds_put_cmd( cmd_ptr );
  }
#endif

}/* ds_profile_3gpp2_mgr_create_profile_ev_hdlr */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_DELETE_PROFILE_EV_HDLR

DESCRIPTION   Handles create profile event from profile DB.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_delete_profile_ev_hdlr
(
  ds_profile_db_profile_managed_type **profiles,
  uint16                               profiles_count,
  ds_profile_db_subs_type              subs_id
)
{
  uint16 index = 0;
  ds_profile_db_gen_params_type        gen_params;
  ds_profile_db_result_type            ret_val = DB_RESULT_FAIL;
#ifdef FEATURE_EHRPD
  ds_cmd_type                         *cmd_ptr = NULL;
#endif      
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void *)&gen_params, 0, sizeof(ds_profile_db_gen_params_type));

  if(profiles == NULL)
  {
    return;
  }

  for (index=0; index < profiles_count; index++)
  {
    if( profiles[index] == NULL )
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "ds_profile_3gpp2_mgr_delete_profile_ev_hdlr: "
                       "NULL pointer %d in profiles array",
                       index );
      continue;
    }

    /*------------------------------------------------------------------- 
      Get C type profile structure from DB
     -------------------------------------------------------------------*/
    ret_val = ds_profile_db_get_tech_params( profiles[index],
                                             DB_TECH_3GPP2,
                                             NULL,
                                             &gen_params );
    if(ret_val != DB_RESULT_SUCCESS) 
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "ds_profile_3gpp2_mgr_delete_profile_ev_hdlr: "
                       "Unable to get gen_params" );
      continue;
    }

    if (gen_params.subscription_id != subs_id)
    {

      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Profile Subs ID %d and event Subs ID %d mismatch",
                       gen_params.subscription_id,
                       subs_id );
      continue;
    }

    /*---------------------------------------------------------------------
      Remove check eHRPD, make sure profile DB table and cache are in sync
      Delete the cache entry here
     --------------------------------------------------------------------*/
    if( FALSE == ds707_data_session_delete_cache_entry(
                   (ds707_data_session_profile_id)gen_params.profile_number,
                   (ds_profile_subs_id_enum_type)gen_params.subscription_id) ) 
    {
      continue;
    }

    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "ds_profile_3gpp2_mgr_delete_profile_ev_hdlr: "
                     "Profile %d deleted on subs %d ",
                     gen_params.profile_number, gen_params.subscription_id);

  }/*for all profiles passed by DB*/

#ifdef FEATURE_EHRPD
  if( profiles_count !=0 )
  {
    /*---------------------------------------------------------------------
      Send command to DS task to check if there is any change to mandatory APNs
    ---------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
    if(NULL == cmd_ptr)
    {
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_MANDATORY_APN_CHECK;
    ds_put_cmd( cmd_ptr );
  }
#endif

}/* ds_profile_3gpp2_mgr_delete_profile_ev_hdlr */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_MODIFY_PROFILE_EV_HDLR

DESCRIPTION   Handles modify profile event from profile DB.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_modify_profile_ev_hdlr
(
  ds_profile_db_profile_managed_type   **profiles,
  uint16                                 profiles_count,
  ds_profile_db_subs_type                subs_id
)
{
  uint16                                 index = 0;
  boolean                                is_mand_apn_chk = FALSE;
  ds_profile_db_gen_params_type          gen_params;
  ds_profile_db_3gpp2_tech_params_type   tech_params;
  ds_profile_db_common_tech_params_type *common_params = NULL;
  ds707_data_session_profile_info_type  *data_sess_profile = NULL;
#ifdef FEATURE_EHRPD
  ds_cmd_type                           *cmd_ptr = NULL;             
  ds707_pkt_min_apn_update_cmd_type        *min_apn_update_ptr=NULL;
  char new_apn[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN]= {0};
  uint8 new_apn_len=0;
  char old_apn[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN] = {0};
  uint8 old_apn_len=0;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(profiles == NULL)
  {
    return;
  }

  for (index=0; index < profiles_count; index++) 
  {
    /*-------------------------------------------------------------- 
      Retrieve all required profile info in order to copy
     --------------------------------------------------------------*/
    if( FALSE == 
          ds_profile_3gpp2_util_get_params(profiles[index],
                                           &gen_params,
                                           &tech_params ) )
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "Fail to get required params or changed idents "
                       "from DB, skip %d th profile in array",
                       index );
      continue;
    }

    if (gen_params.subscription_id != subs_id)
    {

      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Profile Subs ID %d and event Subs ID %d mismatch",
                       gen_params.subscription_id,
                       subs_id );
      continue;
    }

    if ( FALSE == ds707_data_session_get_profile(
                    (ds707_data_session_profile_id)gen_params.profile_number,
                    (ds_profile_subs_id_enum_type)gen_params.subscription_id,
                    &data_sess_profile) )
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Profile %d does not exist on subs %d",
                       gen_params.profile_number, gen_params.subscription_id );
      continue;
    }

    /*----------------------------------------------------------------- 
      The reason not to check the profile type is because potentially we 
      are allowing modifications on KDDI profiles thru DB. Modification
      on OMH profiles however should already fail at validation stage.
     -----------------------------------------------------------------*/
    /*----------------------------------------------------------- 
     Populate Cache values with values provided by DB
     -----------------------------------------------------------*/
#ifdef FEATURE_EHRPD
    new_apn_len = strlen(gen_params.apn_name);
    old_apn_len = strlen(data_sess_profile->apn_string);

    if ( new_apn_len != 0 )
    {
      memscpy ( new_apn,
                    DS707_PKT_MGR_APN_MAX_VAL_LEN,
                    gen_params.apn_name,
                    new_apn_len+1);
    }

    if( old_apn_len != 0)
    {
      memscpy ( old_apn,
                    DS707_PKT_MGR_APN_MAX_VAL_LEN,
                    data_sess_profile->apn_string,
                    old_apn_len+1);
    }
#endif /* FEATURE_EHRPD */

    ds_profile_3gpp2_util_copy_profile_and_set_cust_mask(profiles[index],
                                                         &is_mand_apn_chk,
                                                         &gen_params,
                                                         &tech_params,
                                                         &common_params,
                                                         data_sess_profile);

    /*-------------------------------------------------------------- 
      For KDDI profiles, if it is persistent write to legacy folder.
     ------------------------------------------------------------*/
    if( (data_sess_profile->profile_type & DB_3GPP2_PROFILE_KDDI) &&
        (data_sess_profile->is_persistent == TRUE) )
    {
      if(!ds707_write_profile_to_efs(data_sess_profile->data_session_profile_id,
                                     data_sess_profile) )
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "Fail to write KDDI profile %d to legacy EFS", 
                         data_sess_profile->data_session_profile_id);
      }
    }

    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "ds_profile_3gpp2_mgr_modify_profile_ev_hdlr: "
                     "Profile %d modified on subs %d",
                     gen_params.profile_number, gen_params.subscription_id);

  } /*for all profiles passed by DB*/

#ifdef FEATURE_EHRPD
    /*---------------------------------------------------------------------
      check if there was a change to APN enabled flag, if yes run the
      mandatory APN enabled/disabled check
     ---------------------------------------------------------------------*/
    if( is_mand_apn_chk == TRUE )
    {

      DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "ds_profile_3gpp2_mgr_modify_profile_ev_hdlr: "
                     "old aPn length %d new apn length %d",
                     old_apn_len,new_apn_len);

      if( strncmp( (char *)new_apn, (char *)old_apn, new_apn_len) == 0)
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "No chagne in APN name , dont update MIN APN list");
      }
      else
      {
        /*---------------------------------------------------------------------
          Send command to DS task to check if there is any change to mandatory APNs
        ---------------------------------------------------------------------*/
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_pkt_min_apn_update_cmd_type));
 
        if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
        {
          ASSERT(0);
          return;
        }

        min_apn_update_ptr  = (ds707_pkt_min_apn_update_cmd_type *)(cmd_ptr->cmd_payload_ptr);
  
        if ( new_apn_len != 0 )
        {
          memscpy ( min_apn_update_ptr->new_apn,
                      DS707_PKT_MGR_APN_MAX_VAL_LEN,
                      new_apn,
                      new_apn_len+1);
        }
        if ( old_apn_len != 0 )
        {
          memscpy ( min_apn_update_ptr->old_apn,
                    DS707_PKT_MGR_APN_MAX_VAL_LEN,
                    old_apn,
                    old_apn_len+1);
        } 

        min_apn_update_ptr->is_modify = TRUE;

        cmd_ptr->hdr.cmd_id = DS_CMD_707_MANDATORY_APN_UPDATE;
        ds_put_cmd( cmd_ptr );
      }
      
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "ds_profile_3gpp2_mgr_modify_profile_ev_hdlr: "
                       "APN enabled flag changed, check mandatory APNs" );
      cmd_ptr = NULL;
      /*---------------------------------------------------------------------
        Send command to DS task to check if there is any change to mandatory APNs
      ---------------------------------------------------------------------*/
      cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
      if(NULL == cmd_ptr)
      {
        ASSERT(0);
        return;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_707_MANDATORY_APN_CHECK;
      ds_put_cmd( cmd_ptr );
    }
#endif /* FEATURE_EHRPD */

}/* ds_profile_3gpp2_mgr_modify_profile_ev_hdlr */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_RESET_PROFILE_EV_HDLR

DESCRIPTION   Handles reset profile event from profile DB.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_reset_profile_ev_hdlr
(
  ds_profile_db_profile_managed_type   **profiles,
  uint16                                 profiles_count,
  ds_profile_db_subs_type                subs_id
)
{
  uint16                                 index = 0;
  boolean                                is_mand_apn_chk = FALSE;
  ds_profile_db_gen_params_type          gen_params;
  ds_profile_db_3gpp2_tech_params_type   tech_params;
  ds_profile_db_common_tech_params_type *common_params = NULL;
  ds707_data_session_profile_info_type  *data_sess_profile = NULL;
#ifdef FEATURE_EHRPD
  ds_cmd_type                            *cmd_ptr = NULL;             
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(profiles == NULL)
  {
    return;
  }

  for (index=0; index < profiles_count; index++)
  {

    /*-------------------------------------------------------------- 
      Retrieve all required profile info in order to copy
     --------------------------------------------------------------*/
    if( FALSE == 
          ds_profile_3gpp2_util_get_params(profiles[index],
                                           &gen_params,
                                           &tech_params ) )
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "Fail to get required params or changed idents "
                       "from DB, skip %d th profile in array",
                       index );
      continue;
    }

    if (gen_params.subscription_id != subs_id)
    {

      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Profile Subs ID %d and event Subs ID %d mismatch",
                       gen_params.subscription_id,
                       subs_id );
      continue;
    }

    if ( FALSE == ds707_data_session_get_profile(
                    (ds707_data_session_profile_id)gen_params.profile_number,
                    (ds_profile_subs_id_enum_type)gen_params.subscription_id,
                    &data_sess_profile) )
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                       "Profile %d does not exist on subs %d",
                       gen_params.profile_number, gen_params.subscription_id );
      continue;
    }

    memset((void*)&data_sess_profile->custom_val_mask, 0, sizeof(uint64));

    /*----------------------------------------------------------- 
     Populate Cache values with values provided by DB
     -----------------------------------------------------------*/
    ds_profile_3gpp2_util_copy_profile_and_set_cust_mask(profiles[index],
                                                         &is_mand_apn_chk,
                                                         &gen_params,
                                                         &tech_params,
                                                         &common_params,
                                                         data_sess_profile);

    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "ds_profile_3gpp2_mgr_reset_profile_ev_hdlr: "
                     "Profile %d reset to default on subs %d",
                     gen_params.profile_number, gen_params.subscription_id);

  }/*for all profiles passed by DB*/

#ifdef FEATURE_EHRPD
  if( profiles_count != 0 )
  {
    /*---------------------------------------------------------------------
      Send command to DS task to check if there is any change to mandatory APNs
    ---------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
    if(NULL == cmd_ptr)
    {
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_MANDATORY_APN_CHECK;
    ds_put_cmd( cmd_ptr );
  }
#endif /* FEATURE_EHRPD */

} /* ds_profile_3gpp2_mgr_reset_profile_ev_hdlr */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_EVENT_CB

DESCRIPTION   3GPP2 MH will register this callback function for all events 
              from profile DB, and depending on the event, will call the
              appropriate event handler function

PARAMETERS    event 
              **profiles  Profile DB managed profile
              profiles_count

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_event_cb
(
  ds_profile_db_event_type             event,
  ds_profile_db_profile_managed_type **profiles,
  uint16                               profiles_count,
  void                                *user_data,
  ds_profile_db_subs_type              subs
)
{
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "ds_profile_3gpp2_mgr_event_cb: "
                   "Processing event %d",
                   event );

  switch (event)
  {

  case DB_CREATE_PROFILE_EVENT:
    ds_profile_3gpp2_mgr_create_profile_ev_hdlr(profiles, profiles_count, subs);
    break;

  case DB_DELETE_PROFILE_EVENT:
    ds_profile_3gpp2_mgr_delete_profile_ev_hdlr(profiles, profiles_count, subs);
    break;

  case DB_MODIFY_PROFILE_EVENT:
    ds_profile_3gpp2_mgr_modify_profile_ev_hdlr(profiles, profiles_count, subs);
    break;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  case DB_REFRESH_PROFILE_EVENT:
    ds707_data_session_profile_free_non_primary_subs_profile(
      (ds_profile_subs_id_enum_type)subs);
#endif/* FEATURE_MODEM_CONFIG_REFRESH */
  /* fall through intentionally */

  case DB_POWER_UP_EVENT:
    ds_profile_3gpp2_mgr_power_up_ev_hdlr(profiles, profiles_count, subs);
    break;

  case DB_RESET_PROFILE_EVENT:
    ds_profile_3gpp2_mgr_reset_profile_ev_hdlr(profiles, profiles_count, subs);
    break;

  default:
    /* Unknown event, ignore */
    break;
  }

  /*----------------------------------------------------
   Free managed profiles structure before return
  ----------------------------------------------------*/
  ds_profile_db_free_profiles( profiles, profiles_count );

  return;
}/*ds_profile_3gpp2_mgr_event_cb*/

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_VALIDATION_FUNC

DESCRIPTION   Validate the profile parameters when creating a profile.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_validation_func
(
  ds_profile_db_ident_type        ident, 
  const ds_profile_db_info_type*  info,
  const void*                     profile_params,
  ...
)
{
  uint32                      valid_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------- 
    check if identifier is valid for this profile.
   --------------------------------------------------*/
  valid_mask = get_valid_mask_from_ident( (ds_profile_identifier_type)ident );
  if( valid_mask != DS_PROFILE_3GPP2_PROFILE_INVALID) 
  {
    if(!(((ds_profile_db_3gpp2_tech_params_type *)profile_params)->profile_type & valid_mask))
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "ds_profile_3gpp2_mgr_validation_func: "
                       "Invalid param %d",
                       ident);

      return DB_RESULT_ERR_3GPP2_INVALID_IDENT_FOR_PROFILE;
    }
    else
    {
      return dsi_profile_3gpp2_util_ident_validation( ident, info );
    }
  }
  else
  {
    /*----------------------------------------------------------------------- 
      To support idents that are unknown to 3GPP2 MH.
     -----------------------------------------------------------------------*/
    return DB_RESULT_SUCCESS;
  }
} /*ds_profile_3gpp2_mgr_validation_func*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_PROFILE_VALIDATION_GENERAL

DESCRIPTION   General validations related to internal (not public) params, 
              invoked during once during profile creation

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_profile_validation_general
(
  const void *profile
)
{
  /* Validate profile DB internal params */

  return dsi_profile_3gpp2_util_gen_ident_validation( 
           (ds_profile_db_3gpp2_tech_params_type*)profile );
} /*ds_profile_3gpp2_mgr_profile_validation_general*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_DEFAULT_PROFILE

DESCRIPTION   Get default parameters.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_get_default_profile
(
  void *profile
)
{
  data_sess_profile_ppp_params_type *default_ppp_opts;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------- 
   Read NV for default_ppp_opts
   --------------------------------------------------------------*/
  ds707_data_sess_profile_get_stored_ppp_opts(&default_ppp_opts);

  /*-------------------------------------------------------------- 
   Copy default_ppp_opts and hardcoded values.
   --------------------------------------------------------------*/
  ds_profile_3gpp2_util_get_default_params(
    (ds_profile_db_3gpp2_tech_params_type*)profile, 
    default_ppp_opts );

  return;
} /* ds_profile_3gpp2_mgr_get_default_profile */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_DEFAULT_PROFILE_NUM

DESCRIPTION   Get default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_default_profile_num
(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_family_type      family
)
{
  if(num == NULL)
  {
    return DB_RESULT_FAIL;
  }
  else
  {
    /*-------------------------------------------------------------------- 
      TODO: Double check because when in JCDMA mode we seem to use return
      different default profile ID for ds profile. Make sure this is the
      expected behavior moving forward.
     --------------------------------------------------------------------*/
    *num = (ds_profile_db_profile_num_type)
                  ds707_data_session_get_default_profile();

    DATA_IS707_MSG1( MSG_LEGACY_MED,
                     "ds_profile_3gpp2_mgr_get_default_profile_num: %d", 
                     *num);

    return DB_RESULT_SUCCESS;
  }
} /*ds_profile_3gpp2_mgr_get_default_profile_num*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION   Get default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_default_profile_num_per_subs
(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_family_type      family,
  ds_profile_db_subs_type        subs
)
{

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "ds_profile_3gpp2_mgr_get_default_profile_num_per_subs: "
                   "profile num ptr %d  subs id %d", num, subs );
  if(num == NULL)
  {
    return DB_RESULT_FAIL;
  }
  
#ifdef FEATURE_DUAL_SIM
  if( subs == DB_PROFILE_ACTIVE_SUBSCRIPTION_1)
  {
    *num = DATA_SESSION_DEFAULT_PROFILE;
    return DB_RESULT_SUCCESS;
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds_profile_3gpp2_mgr_get_default_profile_num_per_subs:"
                     "invalid subs id %d", 
                     subs );
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }
#else
  return DB_RESULT_FAIL;

#endif /* FEATURE_DUAL_SIM */

} /*ds_profile_3gpp2_mgr_get_default_profile_num_per_subs*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_SET_DEFAULT_PROFILE_NUM

DESCRIPTION   Set default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_set_default_profile_num
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type      family
)
{
  return DB_RESULT_ERR_INVAL_OP;
} /*ds_profile_3gpp2_mgr_set_default_profile_num*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_SET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION   Set default profile number.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_set_default_profile_num_per_subs
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type      family,
  ds_profile_db_subs_type        subs
)
{
  return DB_RESULT_ERR_INVAL_OP;
} /*ds_profile_3gpp2_mgr_set_default_profile_num_per_subs*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_PROFILE

DESCRIPTION   Get the parameters of a specific profile.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_profile
(
  ds_profile_db_profile_num_type          num, 
  void*                                   tech_params,
  ds_profile_db_gen_params_type*          gen_params,
  ds_profile_db_subs_type                 subs
)
{
  ds707_data_session_profile_info_type*   data_sess_profile = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "ds_profile_3gpp2_mgr_get_profile: number %d",
                   num );

  if( ds707_data_session_get_profile(num, 
                                     (ds_profile_subs_id_enum_type)subs,
                                     &data_sess_profile) == TRUE)
  {
    ds_profile_3gpp2_util_cache_to_db_type_convert( gen_params,
                                                    (ds_profile_db_3gpp2_tech_params_type*)tech_params,
                                                    data_sess_profile);
    return DB_RESULT_SUCCESS;
  }
  else
  {
    if ( (num >= DATA_SESSION_NVRUIM_PROFILE_MIN) && 
         (num <= DATA_SESSION_NVRUIM_PROFILE_MAX) )
    {
      return DB_RESULT_ERR_3GPP2_OMH_PROFILE_NOT_VALID;
    }
    else
    {
      return DB_RESULT_ERR_INVAL_PROFILE_NUM;
    }
  }

} /*ds_profile_3gpp2_mgr_get_profile*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_PROFILES_RANGE

DESCRIPTION   Get the range of a specific profile subtype.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_profiles_range
(
  ds_profile_db_3gpp2_profile_subtype  sub_tech,
  ds_profile_db_profile_num_type*      min_num,
  ds_profile_db_profile_num_type*      max_num
)
{
   if( (min_num == NULL)||(max_num == NULL) )
   {
     return DB_RESULT_FAIL;
   }
   else
   {
      switch (sub_tech)
      {
      case DB_3GPP2_PROFILE_COMMON:
         *min_num = DATA_SESSION_PROFILE_ID_MIN;
         *max_num = DATA_SESSION_MIP_PROFILE_MAX;
         break;

      case DB_3GPP2_PROFILE_KDDI:
         if( ds707_roaming_is_curr_mode_jcdma() ||
             ds707_roaming_is_simplified_jcdma_mode() )
         {
           *min_num = DATA_SESSION_JCDMA_PROFILE_MIN;
           *max_num = DATA_SESSION_JCDMA_PROFILE_MAX;
         }
         else
         {
           *min_num = DATA_SESSION_DEFAULT_PROFILE;
           *max_num = DATA_SESSION_DEFAULT_PROFILE;
           DATA_IS707_MSG0( MSG_LEGACY_LOW,
                            "ds_profile_3gpp2_mgr_get_profile_range: "
                            "KDDI disabled, replace with default" );
           return DB_RESULT_FAIL;
         }
         break;
         
      case DB_3GPP2_PROFILE_OMH:
         *min_num = DATA_SESSION_NVRUIM_PROFILE_MIN;
         *max_num = DATA_SESSION_NVRUIM_PROFILE_MAX;
         break;

      default:
         *min_num = DATA_SESSION_DEFAULT_PROFILE;
         *max_num = DATA_SESSION_DEFAULT_PROFILE;
         DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                          "ds_profile_3gpp2_mgr_get_profile_range: "
                          "Invalid subtype, replace with default" );
         return DB_RESULT_FAIL;
      }

      DATA_IS707_MSG3( MSG_LEGACY_LOW,
                       "ds_profile_3gpp2_mgr_get_profile_range: "
                       "Subtype: %d, min %d, max %d", 
                       sub_tech,
                       *min_num,
                       *max_num );

      return DB_RESULT_SUCCESS;
   }
}/*ds_profile_3gpp2_mgr_get_profiles_range*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_NUM_VALID_OMH_PROFILES

DESCRIPTION   Returns number of valid OMH profiles

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  number of valid OMH profiles

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_profile_3gpp2_mgr_get_num_valid_omh_profiles
(
  void
)
{
  return ds707_data_session_get_num_valid_profiles_in_nvruim();
}/* ds_profile_3gpp2_mgr_get_num_valid_omh_profiles */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_GET_IS_TECH_PARAM_SET

DESCRIPTION   Get if the tech specific parameter is set in the EPC profile

PARAMETERS    num                - ds_profile_db_profile_num_type 
              ident              - ds_profile_db_ident_type
              *is_tech_param_set - boolean
DEPENDENCIES  None

RETURN VALUE  ds_profile_db_result_type

SIDE EFFECTS  None
===========================================================================*/
ds_profile_db_result_type ds_profile_3gpp2_mgr_get_is_tech_param_set
(
  ds_profile_db_profile_num_type          num,
  ds_profile_db_subs_type                 subs,
  ds_profile_db_ident_type                ident,
  boolean*                                is_tech_param_set
)
{
  ds707_data_session_profile_info_type*   data_sess_profile = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "ds_profile_3gpp2_mgr_get_param_mask: number %d",
                   num );

  //TODO hrong: Pass subs id in this func?
  if( ds707_data_session_get_profile(num, 
                                     ACTIVE_SUBSCRIPTION_1,
                                     &data_sess_profile) == TRUE)
  {
    ds_profile_3gpp2_util_cache_to_db_get_is_tech_param_set(ident, data_sess_profile, is_tech_param_set);
    return DB_RESULT_SUCCESS;
  }
  else
  {
    if ( (num >= DATA_SESSION_NVRUIM_PROFILE_MIN) && 
         (num <= DATA_SESSION_NVRUIM_PROFILE_MAX) )
    {
      return DB_RESULT_ERR_3GPP2_OMH_PROFILE_NOT_VALID;
    }
    else
    {
      return DB_RESULT_ERR_INVAL_PROFILE_NUM;
    }
  }
} /*ds_profile_3gpp2_mgr_get_profile*/

/*===========================================================================
                   EXTERNAL - POWER-UP INIT FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_FUNC_TABLE_INIT

DESCRIPTION   Register function table with profile DB during MH cache init 
 
PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_func_table_init
(
  void
)
{  
  tech_func_table_type ds_profile_3gpp2_mgr_func_tbl = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_profile_3gpp2_mgr_func_tbl.get_is_tech_param_set            = &ds_profile_3gpp2_mgr_get_is_tech_param_set;
  ds_profile_3gpp2_mgr_func_tbl.get_profile                      = &ds_profile_3gpp2_mgr_get_profile;
  ds_profile_3gpp2_mgr_func_tbl.get_default_profile_num          = &ds_profile_3gpp2_mgr_get_default_profile_num;
  ds_profile_3gpp2_mgr_func_tbl.get_default_profile_num_per_subs = &ds_profile_3gpp2_mgr_get_default_profile_num_per_subs;
  ds_profile_3gpp2_mgr_func_tbl.get_profiles_range               = &ds_profile_3gpp2_mgr_get_profiles_range;
  ds_profile_3gpp2_mgr_func_tbl.set_default_profile_num_per_subs = &ds_profile_3gpp2_mgr_set_default_profile_num_per_subs;
  ds_profile_3gpp2_mgr_func_tbl.set_default_profile_num          = &ds_profile_3gpp2_mgr_set_default_profile_num;
  ds_profile_3gpp2_mgr_func_tbl.validation_function              = &ds_profile_3gpp2_mgr_validation_func;
  ds_profile_3gpp2_mgr_func_tbl.validate_general                 = &ds_profile_3gpp2_mgr_profile_validation_general;
  ds_profile_3gpp2_mgr_func_tbl.get_default_profile              = &ds_profile_3gpp2_mgr_get_default_profile;

  if ( DB_RESULT_SUCCESS != ds_profile_db_register_function_table_MH(
                              &ds_profile_3gpp2_mgr_func_tbl, 
                              DB_TECH_TO_TECHMASK(DB_TECH_3GPP2)) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "ds_profile_3gpp2_mgr_func_tbl_init: "
                     "Unable to register function table" );
    ASSERT(0);
  }

  return;
}/*ds_profile_3gpp2_mgr_func_tbl_init*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_MGR_REGISTER_CB_EVENT

DESCRIPTION   3GPP2 MH will register this callback function for all events 
              from profile DB, and depending on the event, will call the
              appropriate event handler function

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_mgr_register_cb_event
(
  void
)
{
  ds_profile_3gpp2_mgr_callback_id =
    ds_profile_db_register_for_notifications_MH(
        DB_UNSPECIFIED_PROFILE_NUM, 
        DB_TECH_TO_TECHMASK(DB_TECH_3GPP2)|DB_TECH_TO_TECHMASK(DB_TECH_COMMON),
        TRUE,
        ds_profile_3gpp2_mgr_event_cb,
        NULL);

  if ( ds_profile_3gpp2_mgr_callback_id == 0 )
  {
    DATA_ERR_FATAL("ds_profile_3gpp2_mgr_register_event_cb: Unable to register event callback");
  }

  return;
}/*ds_profile_3gpp2_mgr_register_cb_event*/

/*===========================================================================
FUNCTION DS_PROFILE_3GPP2_MODEM_INIT

DESCRIPTION
  This function is called on the library init. It initializes the function
  pointers to valid functions for 3gpp2

PARAMETERS
  fntbl : pointer to table of function pointers

DEPENDENCIES

RETURN VALUE
  returns the mask for 3gpp2. (Used later as valid mask which is ORed value
  returned from all techs.
SIDE EFFECTS

===========================================================================*/
uint8 ds_profile_3gpp2_modem_init( tech_fntbl_type *fntbl )
{
  /* Init function pointers */
  fntbl->create                       = ds_profile_db_handler_create;
  fntbl->del                          = ds_profile_db_handler_delete;
  fntbl->profile_read                 = ds_profile_db_handler_profile_read;
  fntbl->profile_write                = ds_profile_db_handler_profile_write;
  fntbl->validate_profile_num         = ds_profile_db_handler_validate_profile_num;
  fntbl->get_list                     = ds_profile_db_handler_get_list;
  fntbl->get_list_node                = ds_profile_db_handler_get_list_node;
  fntbl->reset_param                  = ds_profile_db_handler_reset_param;
  fntbl->reset_profile_to_default     = ds_profile_db_handler_reset_profile_to_default;
  fntbl->set_default_profile          = ds_profile_db_handler_set_default_profile;
  fntbl->get_default_profile          = ds_profile_db_handler_get_default_profile;
  fntbl->set_default_profile_per_subs = ds_profile_db_handler_set_default_profile_per_subs;
  fntbl->get_default_profile_per_subs = ds_profile_db_handler_get_default_profile_per_subs;
  fntbl->get_num_range                = ds_profile_db_handler_get_num_range;

  return (0x01 << DS_PROFILE_TECH_3GPP2);
}

/*===========================================================================
FUNCTION DS_PROFILE_3GPP2_INIT

DESCRIPTION
  This function is called on the library init. It initializes the function
  pointers to valid functions for 3gpp2

PARAMETERS
  fntbl : pointer to table of function pointers

DEPENDENCIES

RETURN VALUE
  returns the mask for 3gpp2. (Used later as valid mask which is ORed value
  returned from all techs.
SIDE EFFECTS

===========================================================================*/

uint8 ds_profile_3gpp2_init ( tech_fntbl_type *fntbl )
{
  /* Init function pointers */
  fntbl->dealloc   = ds_profile_db_handler_dealloc;
  fntbl->set_param = ds_profile_db_handler_set_param;
  fntbl->get_param = ds_profile_db_handler_get_param;

  return (0x01 << DS_PROFILE_TECH_3GPP2);
}

