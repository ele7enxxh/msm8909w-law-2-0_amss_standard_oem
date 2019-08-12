/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M O B I L E  E Q U I P M E N T  C O M M A N D S )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common, 
  non-ETSI Mobile Equipment commands.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatme_cmd_status_handler_ex
    This function is the async event handler for async functions such as
    pbm_file_info, pbm_read, pbm_write, and pbm_find_name.

  dsatme_ld_connected
    Change ld_rec state to be connected.

  dsatme_ld_save
    Store ld_rec to the last dial ph book.

  dsatme_pbm_cb_cmd_handler
    Processes DS_AT_PBM_CB_CMD cmd infos.

  dsatme_reset_pb
    Reset ph book to default values.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatme_ex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/16   skc     Fixed issue related to duplicate call of csp, spn and imsi
                   read handler.
06/15/16   skc     Fixed issue while saving last dialed number with ATD.
09/16/15   skc     Fixed compilation issue with WTGL Flavour
05/22/15   sc      Fixed +CLCK personalization pin issue.
06/27/14   tk/pg   Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DS_OMH
#include "mobile.h"
#endif /* FEATURE_DS_OMH */

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#include "dsatme.h"
#include "dsatctab.h"

#ifdef FEATURE_MMGSDI
#include "err.h"
#include "amssassert.h"
#include "dsatcmif.h"
#include "dsatparm.h"

/* Needed for MMGSDI support for SIM card power control functionality,
  +CPLS and +CPOL commands */
#include "mmgsdilib.h"
#include "mmgsdilib_v.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#include "sm_common.h"
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_DATA_ETSI_PIN
#include "dsatetsime.h"
#include "dsatetsictab.h"
#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicmif.h"
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_IS707

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip.h"
#include "dsat707mip.h"
#endif /* FEATURE_DS_MOBILE_IP */

#include "ds707_data_session_profile.h"
#include "dsat707util.h"
#include "dsat707ext.h"
#include "dsat707vend.h"
#endif /* FEATURE_DATA_IS707 */

#include "nvruimi.h"
#include "dstaski.h"
#include "ps_svc.h"
#include "uim_v.h"
#endif /* FEATURE_MMGSDI */
#include "sys_v.h"
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#include "err.h"
#include "time_svc.h"
#include "time_tod.h"
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#include "sys_m_reset.h"
#include "dsatdl.h"
#include "mcfg_fs.h"

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define DEBUG( x )

#ifdef FEATURE_MMGSDI
boolean dsat_pcasp_cmd_pending = FALSE;

#ifdef FEATURE_MMGSDI
/* Send MMGSDI refresh complete set to TRUE/FALSE for pending 
   FCN notification ( (num_files > 1), denotes pending FCN)
   Num of files == 1 indicates thats the last file processed so send refresh complete */

#define SEND_PASS_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id) \
     {\
        dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL; \
        me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE); \
        if (NULL == me_ms_val) \
        { \
          return DSAT_ASYNC_EVENT; \
        } \
        if(  (DSAT_MMGSDI_REFRESH_MASK_CB & refresh_mask ) && ( me_ms_val->dsat_refresh_fcn_ind.num_files == 1 )) \
        {\
          me_mmgsdi_refresh_complete(TRUE,apps_id);\
          memset((void*)&me_ms_val->dsat_refresh_fcn_ind,0x0,sizeof(dsat_fcn_ind_type));\
        }\
        else if ( me_ms_val->dsat_refresh_fcn_ind.num_files > 1) \
        {\
          me_ms_val->dsat_refresh_fcn_ind.num_files--;\
          DS_AT_MSG1_MED("Pending files to be read %d",\
              me_ms_val->dsat_refresh_fcn_ind.num_files );\
        }\
        }
#define SEND_FAIL_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id) \
     {\
        dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL; \
        me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE); \
        if (NULL == me_ms_val) \
        { \
          return DSAT_ASYNC_EVENT; \
        } \
        if ( (DSAT_MMGSDI_REFRESH_MASK_CB & refresh_mask ) && \
                 ( me_ms_val->dsat_refresh_fcn_ind.num_files != 0 )) \
        {\
          me_mmgsdi_refresh_complete(FALSE,apps_id);\
          memset((void*)&me_ms_val->dsat_refresh_fcn_ind,0x0,sizeof(dsat_fcn_ind_type));\
        }\
        }
/*Can be used in functions that return void*/
#define SEND_PASS_MMGSDI_REFRESH_COMPLETE_EXT(refresh_mask,apps_id) \
     {\
        dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL; \
        me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE); \
        if (NULL == me_ms_val) \
        { \
          return ; \
        } \
        if(  (DSAT_MMGSDI_REFRESH_MASK_CB & refresh_mask ) && ( me_ms_val->dsat_refresh_fcn_ind.num_files == 1 )) \
        {\
          me_mmgsdi_refresh_complete(TRUE,apps_id);\
          memset((void*)&me_ms_val->dsat_refresh_fcn_ind,0x0,sizeof(dsat_fcn_ind_type));\
        }\
        else if ( me_ms_val->dsat_refresh_fcn_ind.num_files > 1) \
        {\
          me_ms_val->dsat_refresh_fcn_ind.num_files--;\
          DS_AT_MSG1_MED("Pending files to be read %d",\
              me_ms_val->dsat_refresh_fcn_ind.num_files );\
        }\
     }
#define SEND_FAIL_MMGSDI_REFRESH_COMPLETE_EXT(refresh_mask,apps_id) \
     {\
        dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL; \
        me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE); \
        if (NULL == me_ms_val) \
        { \
          return ; \
        } \
        if ( (DSAT_MMGSDI_REFRESH_MASK_CB & refresh_mask ) && \
                 ( me_ms_val->dsat_refresh_fcn_ind.num_files != 0 )) \
        {\
          me_mmgsdi_refresh_complete(FALSE,apps_id);\
          memset((void*)&me_ms_val->dsat_refresh_fcn_ind,0x0,sizeof(dsat_fcn_ind_type));\
        }\
      }
#endif /* FEATURE_MMGSDI */

LOCAL void me_mmgsdi_client_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);

LOCAL void dsatme_mmgsdi_refresh_fcn_read
(
  ds_at_mmgsdi_event_type *mmgsdi_event_info /* MMGSDI event info pointer */
);

LOCAL void me_mmgsdi_refresh_complete
(
  boolean               pass_fail,
  dsat_apps_id_e_type   apps_id
);

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
#endif /* FEATURE_MMGSDI */

/*-------------------------------------------------------------------------
  Local variables
-------------------------------------------------------------------------*/
cb_cmd_e_type cb_expected = CB_NONE;
#ifdef FEATURE_DATA_ETSI_PIN

LOCAL dsat_result_enum_type cpin_process_pin_status_ex (ds_at_cmd_status_type *);

#endif /* FEATURE_DATA_ETSI_PIN */

LOCAL void dsatme_init_thin_ui_cfg(void);

#ifdef FEATURE_DATA_ETSI_PIN
const status_handler_table_entry_type status_handler_table_ex[] =
{
  {CPIN_MMGSDI_PIN1_STATUS,  cpin_process_pin_status_ex},
  {CPIN_MMGSDI_PIN2_STATUS,  cpin_process_pin_status_ex},
  {CB_NONE,                NULL}
};
#endif /* FEATURE_DATA_ETSI_PIN */

LOCAL boolean is_thin_ui = FALSE;

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)


/* Variable to hold Card Application specific info 
   Default set to access - Global Phone book 
                         - Provision NONE
                         - PBM's First slot
  The fourth parameter is updated with the appropriate pbm_device type.
*/
pbm_phonebook_type dsat_pb_id = 
{
  PBM_GPB, 
  PBM_PROVISION_NONE,
  (uint16)PBM_FIRST_SLOT,
  PBM_DEFAULT
};
/* hold infos of current ph book storage 
   Initialize to be the default (SM) in the dsat_cpbs_list*/
me_pb_entry_type me_cpbs_entry =
{
  PBM_DS_SM,
  DEFAULT_CPB_STORAGE,
  -1,
  -1,
  PBM_ERROR
};

/* save the infos of the default ph book storage,
   we will restore me_cpbs_entry to the default storage after
   ATZ or AT&F
   DEFAULT_CPB_STORAGE is ME, we use macro here to synchronize it with
   dsat_cpbs_storage_valstr */
LOCAL const me_pb_entry_type me_cpbs_default =
{
  PBM_DS_SM,
  DEFAULT_CPB_STORAGE,
  -1,
  -1
};

LOCAL const me_pb_entry_type me_nvonly_cpbs_default =
{
  PBM_EFS,
  DEFAULT_NV_CPB_STORAGE,
  -1,
  -1
};


LOCAL void cpb_pbm_status_cb(boolean);

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
const pbm_cb_handler_table_type pbm_cb_handler_table_ex[] =
{
  { CPB_PBM_CB_READY,           cpb_pbm_cb_ready_handler           },
  { CPB_PBM_CB_READY_FILE_INFO, cpb_pbm_cb_ready_file_info_handler },
  { CB_NONE,                    NULL                               }
};
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

LOCAL void me_pbm_file_info_cb
(
  pbm_return_type                status,
  pbm_device_type                pbm_device,
  pbm_extended_fileinfo_s_type * pbm_extended_fileinfo
);

LOCAL void me_pbm_cb_submit_ds_cmd
(
  cb_cmd_e_type     cmd,
  pbm_return_type    status,
  ds_at_pbm_file_info_type * info_ptr,
  pbm_record_s_type * record_ptr
);

#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */


#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/*===========================================================================

FUNCTION  DSATME_INIT_PBM

DESCRIPTION
  PBM initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to PBM.
  - Registers the client call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_init_pbm ( void )
{
  
  (void) pbm_reg_event_cb(cpb_pbm_status_cb);

  /* Set verbose mode only during powerup (not ATZ/&F reset) 
   * for RESTRICTED command state case. */
  DSATUTIL_SET_VAL(DSAT_EXT_CMEE_IDX,0,0,0,2,NUM_TYPE);
  dsatme_reset_pb();
  
  return;
}/* dsatme_init_pbm */
/*===========================================================================
FUNCTION DSATME_INIT_PBM_MODE_FROM_NV

DESCRIPTION

    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_init_pbm_mode_from_nv
(
  void
)
{
  nv_stat_enum_type    nv_status = NV_FAIL_S;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, TRUE);
  if (NULL == pbm_ss_val)
  {
    return;
  }

  nv_status = dsatutil_get_nv_item( NV_RTRE_CONFIG_I, &ds_nv_item );
  if ( nv_status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
      In NV only PBM does not send ready indication, set the same.
     -----------------------------------------------------------------------*/
    if(ds_nv_item.rtre_config == NV_RTRE_CONFIG_NV_ONLY)
    {
#ifdef FEATURE_PBM_USE_EFS_PB
      pbm_ss_val->dsatme_nv_only = TRUE;
#endif /*FEATURE_PBM_USE_EFS_PB*/
    }
  }
}


/*===========================================================================

FUNCTION DSATME_RESET_PB

DESCRIPTION
  Reset ph book to default values.  AT cmd processor call this function
  at init time or when atz, at&f are executed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  me_cpbs_entry will be set to default if dsat_cpbs_val is 0
  and me_cpbs_entry.device != PBM_DS_SM

===========================================================================*/
void dsatme_reset_pb(void)
{

dsat_num_item_type  enum_val;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, TRUE);
  if (NULL == pbm_ss_val)
  {
    return ;
  }
  enum_val = (dsat_num_item_type) DSATETSI_EXT_ACT_CPBS_ETSI_IDX;

  if((dsat_num_item_type)dsatutil_get_val(enum_val,0,0,NUM_TYPE) == 0 && 
     me_cpbs_entry.device != me_cpbs_default.device)
  {
    me_cpbs_entry = me_cpbs_default;
  }
  if(pbm_ss_val->dsatme_nv_only== TRUE)
  {
    DSATUTIL_SET_VAL(enum_val,0,0,0,5,NUM_TYPE)
    me_cpbs_entry = me_nvonly_cpbs_default;
  }
  DS_AT_MSG0_HIGH("dsatme_reset_pb");
} /* dsatme_reset_pb */





/*===========================================================================

FUNCTION DSATME_LD_CONNECTED

DESCRIPTION
  Change ld_rec state to be connected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_connected
(
  sys_modem_as_id_e_type subs_id
)
{
  time_julian_type          julian_time;
  dsatme_pbm_state_ms_info *pbm_ms_val = NULL;

  pbm_ms_val = dsat_get_base_addr_per_subs(DSAT_PBM_MS_VALS, subs_id, TRUE);
  if ((NULL == pbm_ms_val) || (NULL == pbm_ms_val->ld_rec_ptr))
  {
    return ;
  }

  if (pbm_ms_val->ld_rec_ptr->state == LD_DIALED)
  {
    pbm_ms_val->ld_rec_ptr->state = LD_CONNECTED;

    pbm_ms_val->ld_rec_ptr->rec.is_answered = TRUE;

    /* get the current time at the connction */
    memset(&julian_time, 0, sizeof(time_julian_type));

    time_get_julian(&julian_time);

    pbm_ms_val->ld_rec_ptr->conn_time = time_jul_to_secs(&julian_time);
  }

} /* dsatme_ld_connected */

/*===========================================================================

FUNCTION DSATME_LD_SAVE

DESCRIPTION
  Store ld_rec to the last dial ph book.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_save
(
  sys_modem_as_id_e_type subs_id
)
{
  uint32 data_sizes[DSAT_CPBW_MAX_FIELDS];
  uint8 *fields_ptr=NULL;
  uint32 fields_ptr_size;
  pbm_field_s_type *ptyped_fields_ptr = NULL;
  uint32 num_fields = 0;
  uint32 field_index = 0;
  uint16 name[PBM_TEXT_SIZE_CHARS];
  dsat_pbm_rec_id_type rec_id;
  pbm_phonebook_type pb_id_local;
  dsatme_pbm_state_ms_info  *pbm_ms_val = NULL;

  pbm_ms_val = dsat_get_base_addr_per_subs(DSAT_PBM_MS_VALS, subs_id, TRUE);
  if ((NULL == pbm_ms_val) || (NULL == pbm_ms_val->ld_rec_ptr))
  {
    return ;
  }
  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));
  if(pbm_ms_val->ld_rec_ptr->state != LD_NONE)
  {
    if(pbm_ms_val->ld_rec_ptr->state == LD_CONNECTED)
    {
      /* get the duration of the call */
      time_julian_type     julian_time;
      dword               cur_time;
      
      ASSERT(pbm_ms_val->ld_rec_ptr->rec.is_answered);

      /* get current time */
      memset(&julian_time, 0, sizeof(time_julian_type));

      time_get_julian(&julian_time);

      cur_time = time_jul_to_secs(&julian_time);
      pbm_ms_val->ld_rec_ptr->rec.duration = cur_time - pbm_ms_val->ld_rec_ptr->conn_time;
    }

    if(pbm_ms_val->ld_rec_ptr->rec.text[0] != '\0' )
    {
      (void) dsatutil_memscpy((void*)name,
            PBM_TEXT_SIZE_CHARS,(void*)pbm_ms_val->ld_rec_ptr->rec.text,PBM_TEXT_SIZE_BYTES);
      data_sizes[num_fields++] = (pbmutils_wstrlen(name)+ 1) * sizeof(uint16);	  
    }

    data_sizes[num_fields++] =  strlen(pbm_ms_val->ld_rec_ptr->rec.number)+ 1;
    data_sizes[num_fields++] =  sizeof(uint8);

    if (FALSE == pbm_allocate_fields (&fields_ptr,
                                      &fields_ptr_size,
                                      num_fields,
                                      data_sizes))
    {
      DS_AT_MSG0_ERROR("Error: Could not allocate size");
      return;
    }

    ptyped_fields_ptr = (pbm_field_s_type *) fields_ptr;

    if(pbm_ms_val->ld_rec_ptr->rec.text[0] != '\0' )
    {
      pbm_fill_field(&ptyped_fields_ptr[field_index],PBM_FIELD_NAME,PBM_FT_UCS2,
                  (byte *)name, (uint16) data_sizes[field_index]);
      field_index++;
    }

    pbm_fill_field(&ptyped_fields_ptr[field_index],PBM_FIELD_PHONE_GENERIC,
                   PBM_FT_PHONE,(byte *)pbm_ms_val->ld_rec_ptr->rec.number, 
                   (uint16) data_sizes[field_index]);

    field_index++;

    pbm_fill_field(&ptyped_fields_ptr[field_index],PBM_FIELD_NUMBER_TYPE,
                   PBM_FT_BYTE, (byte *)&pbm_ms_val->ld_rec_ptr->rec.type, 
                   (uint16) data_sizes[field_index]);

    rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;

    dsat_get_phonebook_id(&pb_id_local);
    pb_id_local.device_type = PBM_DIALED;
    (void) pbm_session_record_write( pb_id_local, 
                                     &rec_id,
                                     (uint16)PBM_CAT_OUTGOING,
                                     (int)num_fields, 
                                     fields_ptr, 
                                     fields_ptr_size,
                                     me_pbm_write_cb, 
                                     NULL);

    /* Now its safe to free the fields_ptr */
    pbm_free_fields(&fields_ptr,num_fields);

    /* reset the state */
    pbm_ms_val->ld_rec_ptr->state = LD_NONE;

    dsatutil_free_memory(pbm_ms_val->ld_rec_ptr);
    pbm_ms_val->ld_rec_ptr = NULL;
  }
} /* dsatme_ld_save */



/*===========================================================================

FUNCTION ME_PBM_CB_SUBMIT_DS_CMD

DESCRIPTION
  Submit a ds cmd to process pbm cb result in ds_task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void me_pbm_cb_submit_ds_cmd
(
  cb_cmd_e_type   cmd,                   /* cmd id                   */
  pbm_return_type status,                /*  command status          */

                                         /* for CPB_PBM_CB_FILE_INFO */
  ds_at_pbm_file_info_type * info_ptr,   /* file info                */


  pbm_record_s_type * record_ptr         /* for CPB_PBM_CB_READ,
                                            CPB_PBM_CB_FIND          */
)
{
  ds_cmd_type          *cmd_buf;
  ds_at_pbm_info_type *pbm_info = NULL;

  if(cmd == CPB_PBM_CB_READY_FILE_INFO ||
     cmd == CPB_PBM_CB_FILE_INFO ||
     cmd == DD_PBM_FILE_INFO)
  {
    if (status == PBM_SUCCESS && info_ptr == NULL)
    {
      DS_AT_MSG0_ERROR("NULL info_ptr");
      return;
    }
  }
  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_pbm_info_type), FALSE);
  pbm_info = cmd_buf->cmd_payload_ptr;

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id      = DS_CMD_ATCOP_PBM_CB_CMD;
  pbm_info->cmd        = (int8) cmd;
  pbm_info->status     = status;

  /* record_ptr or  info_ptr contain good information only when 
     status is PBM_SUCCESS */
  if(status == PBM_SUCCESS)
  {
    if(cmd == CPB_PBM_CB_READY_FILE_INFO ||
       cmd == CPB_PBM_CB_FILE_INFO ||
       cmd == DD_PBM_FILE_INFO)
    {
      pbm_info->cmd_info.file_info = *info_ptr;
    }
  }

  ds_put_cmd(cmd_buf);
}

/*===========================================================================

FUNCTION ME_PBM_WRITE_CB

DESCRIPTION
  This function is called by pbm to return the results of pbm_session_record_write
  function call.

  This function is passed to pbm with pbm_session_record_write function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void me_pbm_write_cb
(
  pbm_writecb_data_s_type *cb_data   /* pbm_session_record_write callback structure */
)
{
  DS_AT_MSG1_HIGH("me_pbm_write_cb status %d",cb_data->ret);
  
  me_pbm_cb_submit_ds_cmd(cb_expected, cb_data->ret, NULL, NULL);
} /* me_pbm_write_cb */


/*==========================================================================
FUNCTION DSAT_GET_PHONEBOOK_ID

DESCRIPTION
  This function takes the pointer variable of type pbm_phonebook_type 
and fills with the globle data.
        
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
=============================================================================*/
void dsat_get_phonebook_id(pbm_phonebook_type *pb_id_local)
{
  if (NULL != pb_id_local)
  {
    (*pb_id_local).pb_category = dsat_pb_id.pb_category;
    (*pb_id_local).prov_type = dsat_pb_id.prov_type;
    (*pb_id_local).slot_id = dsat_pb_id.slot_id;
    (*pb_id_local).device_type = dsat_pb_id.device_type;
  }
} /*dsat_get_phonebook_id*/

#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/*--------                pbm call back functions                --------*/

/*===========================================================================

FUNCTION CPB_PBM_STATUS_CB

DESCRIPTION
  Pbm status report call back function.  Pbm calls this function
  to inform its status, ready or not ready.

  DEPENDENCIES
  PBM is checking only for the availability of ADN phone books
  and the status is sent solely based on that. So, neglecting
  status sent by PBM. From here on for Phone book access commands,
  the status sent by the PBM at the time of invocation is final. 

RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cpb_pbm_status_cb(boolean status)
{
  me_pbm_cb_submit_ds_cmd(CPB_PBM_CB_READY, (pbm_return_type) TRUE,
                          NULL, NULL);
}

/*===========================================================================

FUNCTION DSATME_PBM_CB_CMD_HANDLER

DESCRIPTION
  Processes DS_AT_PBM_CB_CMD cmd infos.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ASYNC_CMD   : waiting for next cb.
    DSAT_ERROR       : if there is error.
    DSAT_CMD_ERR_RSP : if there is cme error.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type dsatme_pbm_cb_cmd_handler
(
  ds_cmd_type *         cmd_ptr
)
{
  int index;
  ds_at_pbm_info_type *pbm_info = dsat_get_cmd_payload_ptr(cmd_ptr);

  /* reset the cb_expect variable */
  cb_expected = CB_NONE;

  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.pbm_cb_handler_table_ptr);

  for(index = 0; dsatdl_vtable.pbm_cb_handler_table_ptr[index].cmd_id != CB_NONE; index++)
  {
    if((int8)dsatdl_vtable.pbm_cb_handler_table_ptr[index].cmd_id == pbm_info->cmd)
    {
      return dsatdl_vtable.pbm_cb_handler_table_ptr[index].handler(pbm_info);
    }
  }

  /* the default action is to ignore the event */
  DS_AT_MSG1_ERROR("wrong cmd %d",pbm_info->cmd);
  return DSAT_ASYNC_EVENT;
} /* dsatme_pbm_cb_cmd_handler */


/*===========================================================================

FUNCTION CPB_PBM_CB_READY_HANDLER

DESCRIPTION
  Process CPB_PBM_CB_READY pbm cb cmd.

  It set dsatme_pbm_ready according to the cmd status.
  If the status is TRUE, it calls pbm_file_info to get the ph book
  info of the default storage type.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT : unsolicited events.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_ready_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
)
{
  if(pbm_info->status)
  {
    /* call pbm_get_file_info to get the info about the default storage */
    cb_expected = CPB_PBM_CB_READY_FILE_INFO;
    /* error cases are also handled in the call-back */
    dsat_pb_id.device_type = me_cpbs_entry.device;
    (void) pbm_session_extended_file_info_async(dsat_pb_id,
                                     me_session_pbm_file_info_cb);
  }

  return DSAT_ASYNC_EVENT;
} /* cpb_pbm_cb_ready_handler */

/*===========================================================================

FUNCTION ME_PBM_FILE_INFO_CB

DESCRIPTION
  This function is called by pbm to return the results of pbm_file_info
  function call.

  This function is passed to pbm with pbm_file_info function call.

  If the pbm_file_info command succeeds, update the content of 
  val_ptr (dsat_cpbs_val) of cpbs command and save the storage info in 
  current cpbs entry, me_cpbs_entry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void me_pbm_file_info_cb
(
  pbm_return_type    status,           /* command pbm_file_info status    */
  pbm_device_type pbm_device,       /* the storage type be selected    */
  pbm_extended_fileinfo_s_type * pbm_extended_fileinfo /*Response*/
)
{
  ds_at_pbm_file_info_type  file_info;

  DS_AT_MSG3_HIGH("pbm_device %d, used %d, total, %d",
                 pbm_device, pbm_extended_fileinfo->records_used,
                 pbm_extended_fileinfo->number_of_records);

  file_info.pbm_device = pbm_device;
  file_info.used = pbm_extended_fileinfo->records_used;
  file_info.total = pbm_extended_fileinfo->number_of_records;
  file_info.text_len = pbm_extended_fileinfo->text_len;
  me_pbm_cb_submit_ds_cmd(cb_expected, status, &file_info, NULL);
} /* me_pbm_file_info_cb */

/*===========================================================================
FUNCTION ME_SESSION_PBM_FILE_INFO_CB

DESCRIPTION
  This is a wrapper call back API for PBM API. 
Note: This is called in the same context as in DS as PBM is not 
processing its command in PBM task. this needs to be optmized in later
versions. 


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void me_session_pbm_file_info_cb
(
  pbm_return_type                status,
  pbm_phonebook_type             pb_id,
  pbm_extended_fileinfo_s_type * pbm_extended_fileinfo
)
{
  /* Ignoring Session Details as we dont deal with them */
  me_pbm_file_info_cb( status,
                       pb_id.device_type,
                       pbm_extended_fileinfo );

} /* me_session_pbm_file_info_cb */

/*===========================================================================

FUNCTION CPB_PBM_CB_READY_FILE_INFO_HANDLER

DESCRIPTION
  This function processes the file_info returned from pbm just after
  pbm gets ready.

  If the pbm_file_info command succeeds, update the ph book info of
  the default ph number storage type.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_ready_file_info_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
)
{
  ds_at_pbm_file_info_type   *info_ptr    = NULL;
  pbm_return_type             status;

  /* command pbm_file_info status */
  status = pbm_info->status;
  
  /* file info */
  info_ptr = &pbm_info->cmd_info.file_info;

  me_cpbs_entry.pbm_status = status;

  /*-------- the command succeeds --------*/
  if(status == PBM_SUCCESS)
  {
    /* save the file info to the default storage */
    ASSERT(me_cpbs_entry.device == info_ptr->pbm_device);
    me_cpbs_entry.total = (int16) info_ptr->total;
    me_cpbs_entry.text_len = (int16) MIN(info_ptr->text_len,PBM_MAX_TEXT_LENGTH);
  } /* if(status == PBM_SUCCESS) */
  /* the command fails. the storage did not get set correctly */
  else
  {
    DS_AT_MSG1_ERROR("pbm_file_info cb with status: %d",status);
  }

  return DSAT_ASYNC_EVENT;
} /* cpb_pbm_cb_ready_file_info_handler */

#ifdef FEATURE_MMGSDI

/*-------------------------------------------------------------------------
            Function Definitions: Internal functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION  DS3G_MMGSDI_CLIENT_INIT

DESCRIPTION
  This function updates the session id's to ATCoP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_mmgsdi_client_init
(
  mmgsdi_session_open_info_type *session_info_ptr,
  mmgsdi_client_id_type          client_id
)
{
  ds_cmd_type            *cmd_buf  = NULL;
  ds_at_mmgsdi_init_info *mmgsdi_init_info = NULL;

  if(FALSE == dsat_is_atcop_allowed())
  {
    DS_AT_MSG0_ERROR(" ATcop is not initialized");
    return ;
  }

  if (NULL == session_info_ptr)
  {
     DS_AT_MSG0_ERROR("session_info_ptr is NULL");
     return;
  }

  DS_AT_MSG3_HIGH("Session_type = %d, Session_id = %d, App type = %d",
                  session_info_ptr->session_type, session_info_ptr->session_id,
                  session_info_ptr->app_info.app_type);

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_init_info), FALSE);
  mmgsdi_init_info = cmd_buf->cmd_payload_ptr;

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_MMGSDI_INIT_INFO;
  mmgsdi_init_info->client_id    = client_id;
  mmgsdi_init_info->session_type = session_info_ptr->session_type;
  mmgsdi_init_info->session_id   = session_info_ptr->session_id;
  mmgsdi_init_info->app_type     = session_info_ptr->app_info.app_type;
  ds_put_cmd(cmd_buf);
} /* dsat_mmgsdi_client_init */
/*===========================================================================
FUNCTION DSAT_MMGSDI_INIT_HANDLER

DESCRIPTION
  This function is handler function for MMGSDI INIT event.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type dsat_mmgsdi_init_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  ds_at_mmgsdi_init_info *mmgsdi_init_info = dsat_get_cmd_payload_ptr(cmd_ptr);

  DS_AT_MSG3_HIGH("Session_type = %d, Session_id = %d, App type = %d",
    mmgsdi_init_info->session_type,
    mmgsdi_init_info->session_id,
    mmgsdi_init_info->app_type);

    me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, TRUE);
    if (NULL == me_ss_val)
    {
      return DSAT_ASYNC_EVENT;
    }

  me_ss_val->dsat_mmgsdi_client_id = mmgsdi_init_info->client_id;

  switch (mmgsdi_init_info->session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      dsat_qcsimapp_info[DSAT_APPS_ID_GW_PRIMARY].session_id =
        mmgsdi_init_info->session_id;
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      dsat_qcsimapp_info[DSAT_APPS_ID_GW_SECONDARY].session_id =
        mmgsdi_init_info->session_id;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      dsat_qcsimapp_info[DSAT_APPS_ID_1X_PRIMARY].session_id =
        mmgsdi_init_info->session_id;
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      dsat_qcsimapp_info[DSAT_APPS_ID_GW_TERTIARY].session_id =
        mmgsdi_init_info->session_id;
      break;

    default:
      DS_AT_MSG0_HIGH("Ignoring this Session details");
      break;
  }

  return DSAT_ASYNC_EVENT;

}/*dsat_mmgsdi_init_handler*/


/*==============================================================================

FUNCTION DSAT_MMGSDI_EVENT_CB

DESCRIPTION
  This function handles the MMGSDI client event response callback.  


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsat_mmgsdi_event_cb
(
  const mmgsdi_event_data_type *event
)
{

  ds_cmd_type             *cmd_ptr    = NULL;
  ds_at_mmgsdi_event_type *cmd_rd_ptr = NULL;

  if(event == NULL)
  {
    DATA_ERR_FATAL("Event pointer is null  ");
  }

  if(FALSE == dsat_is_atcop_allowed())
  {
      DS_AT_MSG0_ERROR(" ATcop is not initialized");
      return ;
    }

  DS_AT_MSG2_HIGH("Event = %d, Session id = %d",event->evt,event->session_id);

  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_event_type), FALSE);
  cmd_rd_ptr = cmd_ptr->cmd_payload_ptr;
  /* Fill in command type */
  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_MMGSDI_INFO_CMD;
  cmd_rd_ptr->mmgsdi_event = event->evt;
  cmd_rd_ptr->session_id = event->session_id;

  /* apps_id Calculation move to handler */ 
  cmd_rd_ptr->cmd_status =(int16) MMGSDI_SUCCESS;
  switch( event->evt )
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    case MMGSDI_CARD_INSERTED_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
    case MMGSDI_CARD_ERROR_EVT:

      cmd_rd_ptr->slot_id = event->data.card_inserted.slot;
      cmd_rd_ptr->card_error.info= event->data.card_error.info;
      break;
        
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      cmd_rd_ptr->app_type = event->data.subscription_ready.app_info.app_data.app_type;
      cmd_rd_ptr->slot_id = event->data.subscription_ready.app_info.slot;
      
      (void) dsatutil_memscpy((void*)&cmd_rd_ptr->subscription_ready,
               sizeof(mmgsdi_subscription_ready_evt_info_type),
              (void*)&event->data.subscription_ready,
        sizeof(mmgsdi_subscription_ready_evt_info_type));
      break;
    case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
    case MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT:
      break;

    case MMGSDI_PIN1_EVT:
    case MMGSDI_PIN2_EVT:
      (void) dsatutil_memscpy((void*)&cmd_rd_ptr->pin,
                                      sizeof(mmgsdi_pin_evt_info_type),
                             (void*)&event->data.pin,
                                      sizeof(mmgsdi_pin_evt_info_type));
      break;
      
    case MMGSDI_PERSO_EVT:
      (void) dsatutil_memscpy((void*)&cmd_rd_ptr->perso_info,
                                      sizeof(mmgsdi_perso_evt_info_type),
                             (void*)&event->data.perso,
                                      sizeof(mmgsdi_perso_evt_info_type));
      break;
      
    case MMGSDI_REFRESH_EVT:
        /* Fill in command type */
      DS_AT_MSG2_HIGH("Refresh stage %d, File change notification %d",
              event->data.refresh.stage, 
              event->data.refresh.refresh_files.num_files);
      cmd_rd_ptr->refresh_stage = event->data.refresh.stage;
      cmd_rd_ptr->cmd_status =(int16) MMGSDI_SUCCESS;
      if(event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START &&
         event->data.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN)
      {
        DS_AT_MSG2_HIGH( "Refresh stage %d, Refresh mode %d",
              event->data.refresh.stage, 
              event->data.refresh.mode);
        ds_release_cmd_buf(&cmd_ptr);
        return;
      }
      if((event->data.refresh.refresh_files.file_list_ptr == NULL)||
         (event->data.refresh.refresh_files.num_files == 0))
      {
        DS_AT_MSG0_ERROR("null ptr: me_mmgsdi_refresh_file_change_notification_cb");
        cmd_rd_ptr->u.fcn_ind.num_files = 0;
        memset(cmd_rd_ptr->u.fcn_ind.file_list,0x0,DSAT_MAX_NUM_REFRESH_FILES * sizeof(mmgsdi_file_enum_type));
      }
      else
      {
      /* Copy data to command buffer */
      cmd_rd_ptr->u.fcn_ind.num_files = 
      event->data.refresh.refresh_files.num_files;
      (void) dsatutil_memscpy((void*)cmd_rd_ptr->u.fcn_ind.file_list,
                 sizeof(mmgsdi_file_enum_type)*DSAT_MAX_NUM_REFRESH_FILES,
                 (void*)event->data.refresh.refresh_files.file_list_ptr,
                 cmd_rd_ptr->u.fcn_ind.num_files*sizeof(mmgsdi_file_enum_type));
      }
      break;
#endif /* FEATURE_DSAT_ETSI_MODE */

    case MMGSDI_SESSION_CHANGED_EVT:
      cmd_rd_ptr->activated = event->data.session_changed.activated;
      cmd_rd_ptr->slot_id   = event->data.session_changed.app_info.slot;
      cmd_rd_ptr->app_type = event->data.session_changed.app_info.app_data.app_type;
	  
      DS_AT_MSG3_HIGH(" App type = %d Session changed is activated %d  slot %d",
	  	                 cmd_rd_ptr->app_type,cmd_rd_ptr->activated,cmd_rd_ptr->slot_id);

      break;
    default:
      DS_AT_MSG1_HIGH("Not a valid event %d",(int)event->evt);
      ds_release_cmd_buf(&cmd_ptr);

      return;
  }
  DS_AT_MSG0_MED("Post event to DS task");
  ds_put_cmd(cmd_ptr);


}/* dsat_mmgsdi_event_cb */

#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION  DSATME_INIT_THIN_UI_CFG

DESCRIPTION
  This function initializes the variable is_thin_ui based on
  the EFS file enable_thin_ui_cfg value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void dsatme_init_thin_ui_cfg(void)
{
  int fd;
  struct fs_stat temp_buf;
  #define ENABLE_THIN_UI_CFG_F_NAME (char*)"/nv/item_files/Thin_UI/enable_thin_ui_cfg"

  mcfg_fs_status_e_type fs_status = MCFG_FS_STATUS_OK;


  fs_status = mcfg_fs_stat(ENABLE_THIN_UI_CFG_F_NAME,
                           &temp_buf,
                           MCFG_FS_TYPE_EFS,
                           MCFG_FS_SUBID_NONE);

  /* EFS SUCCESS == MCFG_FS_STATUS_OK */
  if(fs_status != MCFG_FS_STATUS_OK)
  {
    DS_AT_MSG1_ERROR("enable_thin_ui_cfg EFS stat errcode = %d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));
  }
  else
  {
    fd = mcfg_fopen(ENABLE_THIN_UI_CFG_F_NAME, 
                    MCFG_FS_O_RDONLY,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    MCFG_FS_SUBID_NONE);
    if (fd < 0)
    {
      DS_AT_MSG1_ERROR("enable_thin_ui_cfg EFS open errcode = %d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    }
    else
    {
      if(-1 == mcfg_fread(fd, &is_thin_ui, sizeof(is_thin_ui), MCFG_FS_TYPE_EFS))
      {
        DS_AT_MSG0_ERROR("enable_thin_ui_cfg EFS read failed");
      }
      else
      {
        DS_AT_MSG1_HIGH("is_thin_ui = %d", is_thin_ui);
      }
      (void)mcfg_fclose(fd,MCFG_FS_TYPE_EFS);
    }
  }
} /* dsatme_init_thin_ui_cfg */

/*===========================================================================

FUNCTION  DSATME_IS_THIN_UI

DESCRIPTION
  This fucntion tells whether thin UI is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If thin UI is enabled.
  FALSE: If thin UI is disabled (default).

SIDE EFFECTS
  None

===========================================================================*/

boolean dsatme_is_thin_ui(void)
{
  return is_thin_ui;
} /* dsatme_is_thin_ui */

/*===========================================================================

FUNCTION DSATME_INITIALIZE_TO_DEFAULT

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_initialize_to_default(dsat_mem_type mem_type,void *ptr)
{
  if(NULL == ptr)
  {
    DS_AT_MSG0_ERROR("NULL pointer");
    return;
  }
  switch(mem_type)
  {
    case DSAT_MMGSDI_SS_VALS:
    {
      dsatme_mmgsdi_state_ss_info  *ss = (dsatme_mmgsdi_state_ss_info  *)ptr;
      ss->dsat_mmgsdi_client_id = (mmgsdi_client_id_type)MMGSDI_INVALID_SESSION_ID;
#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)
      ss->dsat_apps_provisioned =     DSAT_MMGSDI_INACTIVE;
#endif /*SGLTE || DSDA FUSIOn*/
#ifdef FEATURE_DATA_PS_EAP
      ss->dsat_eap_info.eap_method      =  0;
      ss->dsat_eap_info.eap_state       =   DSAT_EAP_STATE_NONE;
      ss->dsat_eap_info.eap_handle      =  EAP_INVALID_HANDLE;
      ss->dsat_eap_info.eap_session_id  = 0;
      ss->dsat_eap_info.eap_key         = NULL;
      ss->dsat_eap_info.is_initialized  = FALSE;
#endif /* FEATURE_DATA_PS_EAP*/      
    }
    break;
    case DSAT_MMGSDI_MS_VALS:
    {
      dsatme_mmgsdi_state_ms_info  *ms = (dsatme_mmgsdi_state_ms_info  *)ptr;
      ms->dsatme_plmn_mode_sel_bit    = CSP_PLMN_MODE_BIT_UNSET;
      ms->dsat_pin_required           = DSAT_CPIN_NONE_REQUIRED;
      ms->dsat_mask_pin2_state        = TRUE;
      ms->dsat_sim_init_comp          = FALSE;
      ms->dsat_pin_error = DSAT_CME_SIM_NOT_INSERTED;
      /* Initialize the plmn selctor */
      ms->dsat_sim_filename_plmn_selector = MMGSDI_NO_FILE_ENUM;
    }
    break;
    case DSAT_MMGSDI_MS_SLOT_VALS:
    {
      dsatme_mmgsdi_state_ms_slot_info  *ms_slot = (dsatme_mmgsdi_state_ms_slot_info  *)ptr;
      ms_slot->dsat_qcsimstat_curr         = DSAT_QCSIMSTAT_UNKNOWN;
      ms_slot->is_qcsimstat_updated        = FALSE;
      ms_slot->dsat_pin_error = DSAT_CME_SIM_NOT_INSERTED;
    }
    break;
    case DSAT_PBM_SS_VALS:
    {
      dsatme_pbm_state_ss_info  *ss = (dsatme_pbm_state_ss_info  *)ptr;
      ss->dsat_pbm_handler = 0;
      ss->dsatme_nv_only = FALSE;
      
    }
    break;
    case DSAT_PBM_MS_VALS:
    {
      dsatme_pbm_state_ms_info *ms = (dsatme_pbm_state_ms_info  *)ptr;
      ms->ld_rec_ptr = NULL;
    }
    break;
    case DSAT_COMMON_SS_VALS:
    {
      dsat_common_state_ss_info *ss = (dsat_common_state_ss_info  *)ptr;
      ss->dsat_qcsimapp_val_updated   = DSAT_QCSIMAPP_VAL_INVALID;
      ss->dsat_qcsimapp_val_user_flag = FALSE;
    }
    break;
    case DSAT_MS_MD_VALS:
    {
      dsatetsipkt_gprs_msg_ms_info  *ms = (dsatetsipkt_gprs_msg_ms_info  *)ptr;
      ms->flush_gprs_buffer = FALSE;
#ifdef FEATURE_DSAT_ETSI_DATA
      /* Initialize the GPRS events message queue for +CGEREP command */
      (void) q_init(&ms->etsipkt_gprs_msg_q );
#endif /* FEATURE_DSAT_ETSI_DATA */
    }
    break;
    default:
     break;
  }
}


/*===========================================================================
FUNCTION:      DSATME_UPDATE_APPSID

DESCRIPTION:
  Returns apps id based on the session id passed

DEPENDENCIES:
  None

RETURN VALUE:
  uint8 - apps id

SIDE EFFECTS:
  None

===========================================================================*/
dsat_apps_id_e_type dsatme_update_appsid
(
  mmgsdi_session_id_type  session_id
)
{
  dsat_apps_id_e_type apps_id = DSAT_APPS_ID_GW_PRIMARY;

  while (apps_id < DSAT_APPS_ID_MAX)
  {
    if (dsat_qcsimapp_info[apps_id].session_id == session_id)
    {
      return apps_id;
    }

    apps_id++;
  }

  return DSAT_APPS_ID_MAX;
}

/*===========================================================================
FUNCTION: DSATME_FLEX_MAPPING_HANDLER

DESCRIPTION:
  This function handles flex mapping between slots and sessions.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
LOCAL void dsatme_flex_mapping_handler
(
  dsat_slot_idx_e_type mmgsdi_slot_idx,
  dsat_apps_id_e_type  mmgsdi_apps_id
)
{
  dsat_slot_idx_e_type slot_idx;
  dsat_apps_id_e_type  apps_id;
  boolean              apps_id_mapped;

  /* Determine the slot index in which MMGSDI apps ID is internally mapped.*/
  for (slot_idx = DSAT_SLOT_IDX_1; slot_idx < DSAT_SLOT_IDX_MAX; slot_idx++)
  {
    if (TRUE == dsat_qcsimapp_table[slot_idx].apps_id_mapped[mmgsdi_apps_id])
    {
      break;
    }
  }

  DS_AT_MSG3_HIGH("Flex mapping handler: mmgsdi_slot_idx = %d, mmgsdi_apps_id = %d, slot_idx = %d",
                  mmgsdi_slot_idx, mmgsdi_apps_id, slot_idx);

  /* Each session must be internally mapped to a slot.*/
  ASSERT(slot_idx < DSAT_SLOT_IDX_MAX);

  /* Loop over all sessions to swap the internally mapped sessions between MMGSDI slot index and
  earlier slot index of MMGSDI apps ID */
  for (apps_id = DSAT_APPS_ID_GW_PRIMARY; apps_id < DSAT_APPS_ID_MAX; apps_id++)
  {
    /* If a session is mapped to a valid MMGSDI slot, then it shouldn't be swapped */
    if (!IS_VALID_SLOT_ID(dsat_qcsimapp_info[apps_id].slot_id))
    {
      apps_id_mapped = dsat_qcsimapp_table[slot_idx].apps_id_mapped[apps_id];

      dsat_qcsimapp_table[slot_idx].apps_id_mapped[apps_id] =
        dsat_qcsimapp_table[mmgsdi_slot_idx].apps_id_mapped[apps_id];

      dsat_qcsimapp_table[mmgsdi_slot_idx].apps_id_mapped[apps_id] = apps_id_mapped;
    }
  }

  dsat_qcsimapp_info[mmgsdi_apps_id].slot_id = mmgsdi_slot_idx + MMGSDI_SLOT_1;

  /* Determine the subs ID for MMGSDI slot index.*/
  dsat_update_qcsimapp_table(mmgsdi_slot_idx);

  /* Determine the subs ID for earlier slot index of MMGSDI apps ID.*/
  dsat_update_qcsimapp_table(slot_idx);

  for (slot_idx = DSAT_SLOT_IDX_1; slot_idx < DSAT_SLOT_IDX_MAX; slot_idx++)
  {
    DS_AT_MSG2_HIGH("slot_idx = %d, subs_id = %d",
                    slot_idx, dsat_qcsimapp_table[slot_idx].subs_id);

    for (apps_id = DSAT_APPS_ID_GW_PRIMARY; apps_id < DSAT_APPS_ID_MAX; apps_id++)
    {
      DS_AT_MSG2_HIGH("apps_id = %d, apps_id_mapped = %d",
                      apps_id, dsat_qcsimapp_table[slot_idx].apps_id_mapped[apps_id]);
    }
  }

  return;
} /* dsatme_flex_mapping_handler */

/*--------     AT mobile equipment cmd handler functions         --------*/
/*===========================================================================

FUNCTION  DSATME_MMGSDI_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_mmgsdi_event_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type    result = DSAT_ASYNC_EVENT;
  dsat_apps_id_e_type      apps_id;
  mmgsdi_slot_id_enum_type slot_id;
  dsat_slot_idx_e_type     slot_idx;
  mmgsdi_events_enum_type mmgsdi_event;
  
#ifdef FEATURE_DSAT_ETSI_MODE
mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  mmgsdi_perso_feature_enum_type perso_event;
  boolean respond = FALSE;
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#endif /*FEATURE_DSAT_ETSI_MODE*/
  static boolean nv_read_done = FALSE;
  dsatme_mmgsdi_state_ms_info       *me_ms_val = NULL;
  dsatme_mmgsdi_state_ms_slot_info  *me_ms_slot_val = NULL;
  ds_at_mmgsdi_event_type *mmgsdi_event_info = dsat_get_cmd_payload_ptr(cmd_ptr);
  uint32                   pin_bitmask = 0;

  mmgsdi_event_info->apps_id = 
    dsatme_update_appsid(mmgsdi_event_info->session_id);

  apps_id = mmgsdi_event_info->apps_id;
  slot_id = mmgsdi_event_info->slot_id;
  slot_idx = DSAT_SLOT_INDEX(slot_id);
  mmgsdi_event = mmgsdi_event_info->mmgsdi_event;


  DS_AT_MSG4_HIGH("dsatme_mmgsdi_event_handler for event: %d, Session id: %d Apps ID: %d, slot_id %d",
                               mmgsdi_event,
                               mmgsdi_event_info->session_id,
                               apps_id, slot_id);
  /* If apps_id == DSAT_APPS_ID_MAX (0xFF) allow session independent events. */
  if((!IS_VALID_APPS_ID(apps_id))&&( ( mmgsdi_event != MMGSDI_CARD_ERROR_EVT )&&
                                      ( mmgsdi_event != MMGSDI_CARD_INSERTED_EVT)&&
                                      ( mmgsdi_event != MMGSDI_CARD_REMOVED_EVT)))

  {
    DS_AT_MSG1_HIGH("Apps ID %d ignoring",apps_id);
    return DSAT_ASYNC_EVENT;
  }

#ifdef FEATURE_DATA_ETSI_PIN
  if ( IS_VALID_GW_APPS_ID(apps_id) || 
       (( mmgsdi_event == MMGSDI_CARD_ERROR_EVT )||
        ( mmgsdi_event == MMGSDI_CARD_INSERTED_EVT)||
        ( mmgsdi_event == MMGSDI_CARD_REMOVED_EVT)))
  {
    result = dsat_pin_mmgsdi_event_handler_ex(mmgsdi_event_info);
  }
#endif /* FEATURE_DATA_ETSI_PIN */
  switch (mmgsdi_event)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      me_ms_slot_val = dsat_get_base_addr_per_slot(slot_id, TRUE);
      if (NULL == me_ms_slot_val)
      {
        return DSAT_ASYNC_EVENT;
      }
      me_ms_slot_val->dsat_pin_error      = DSAT_CME_SIM_BUSY;
      break;

    case MMGSDI_CARD_ERROR_EVT:
      me_ms_slot_val = dsat_get_base_addr_per_slot(slot_id, TRUE);
      if (NULL == me_ms_slot_val)
      {
        return DSAT_ASYNC_EVENT;
      }

      if ( MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT == mmgsdi_event_info->card_error.info || 
           MMGSDI_CARD_ERR_CARD_REMOVED == mmgsdi_event_info->card_error.info )
      {
        me_ms_slot_val->dsat_pin_error = DSAT_CME_SIM_NOT_INSERTED;
      }
      else
      {
        me_ms_slot_val->dsat_pin_error = DSAT_CME_SIM_FAILURE;
      }
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      if (TRUE == mmgsdi_event_info->activated)
      {
        /* MMGSDI mapping between slot and session can be different from internal mapping 
        in flex mapping scenarios. This must be handled before rest of the handling. */
        if (FALSE == dsat_qcsimapp_table[slot_idx].apps_id_mapped[apps_id])
        {
          /* Flex mapping handler uses slot ID in all indices of dsat_qcsimapp_info array */
          dsatme_flex_mapping_handler(slot_idx, apps_id);
        }
        else
        {
          dsat_qcsimapp_info[apps_id].slot_id  = slot_id;
        }

        dsat_qcsimapp_info[apps_id].app_type = mmgsdi_event_info->app_type;
      }
      else
      {
        dsat_sysinfo_cmd.sim_valid[apps_id]  = DSAT_SIM_VALID_NOT_PRESENT;
        dsat_qcsimapp_info[apps_id].app_type = MMGSDI_APP_NONE;
        dsat_qcsimapp_info[apps_id].slot_id  = MMGSDI_MAX_SLOT_ID_ENUM;

        me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE); 
        if (NULL != me_ms_val)
        {
          memset((void *)&me_ms_val->dsat_spn_info,0x00,sizeof(dsat_mmgsdi_spn_info_s_type));
          me_ms_val->dsatme_plmn_mode_sel_bit = CSP_PLMN_MODE_BIT_UNSET;
          /* Update the variable as this is equivalent to SIM restart */
          me_ms_val->dsat_sim_init_comp = FALSE;
          /* Update the CPIN status to be SIM FAILURE */
          /*Clear PUK status*/
          me_ms_val->dsat_pin_blocked  = 0;
          /*Set PIN to PIN NONE*/
          me_ms_val->dsat_pin_required = DSAT_CPIN_NONE_REQUIRED;
          me_ms_val->dsat_pin_error = DSAT_CME_SIM_BUSY;
        } 
      }

      /* Read NV values on receiving either first session changed event or
         first subscription ready event. */
      if (FALSE == nv_read_done)
      {
#ifndef FEATURE_DSAT_DYNAMIC_LOADING
        dsatme_init_config_from_efs();
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */
        dsatme_init_thin_ui_cfg();
        nv_read_done = TRUE;
      }
      break;
    case MMGSDI_SUBSCRIPTION_READY_EVT:
     /* Overwrite/Save apps_type if session changed event handled/missed */         
      if (!IS_VALID_APPS_ID(apps_id))
      {
         return DSAT_ERROR;
      }

      /* MMGSDI mapping between slot and session can be different from internal mapping 
      in flex mapping scenarios. This must be handled before rest of the handling. */
      if (FALSE == dsat_qcsimapp_table[slot_idx].apps_id_mapped[apps_id])
      {
        /* Flex mapping handler uses slot ID in all indices of dsat_qcsimapp_info array */
        dsatme_flex_mapping_handler(slot_idx, apps_id);
      }
      else
      {
        dsat_qcsimapp_info[apps_id].slot_id  = slot_id;
      }

      dsat_qcsimapp_info[apps_id].app_type = mmgsdi_event_info->app_type;
      dsat_qcsimapp_info[apps_id].active = TRUE;
      dsat_sysinfo_cmd.sim_valid[apps_id] = DSAT_SIM_VALID_PRESENT;

      if (IS_VALID_GW_APPS_ID(apps_id))
      {
        /* Read all the EONS files */
        dsatme_read_uim_files( apps_id );
        /* Register with MMGSDI for refresh handler */
        dsatme_mmgsdi_register_for_refresh( apps_id );
      }
      /* Read NV values on receiving either first session changed event or
         first subscription ready event. */
      if (FALSE == nv_read_done)
      {
#ifndef FEATURE_DSAT_DYNAMIC_LOADING
        dsatme_init_config_from_efs();
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */
        dsatme_init_thin_ui_cfg();
        nv_read_done = TRUE;
      }
      break;

    case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
      dsat_qcsimapp_info[apps_id].active = FALSE;
      break;
   
    case MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT:
      dsat_qcsimapp_info[apps_id].active = TRUE;
      break;

#ifdef FEATURE_DSAT_ETSI_MODE
    case MMGSDI_REFRESH_EVT:
     if (IS_VALID_GW_APPS_ID(apps_id))
     {
      if(mmgsdi_event_info->refresh_stage == 
             MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
      {
      /* Invoke OK to refresh */
        mmgsdi_status = mmgsdi_session_ok_to_refresh(
                         dsat_qcsimapp_info[apps_id].session_id,
                                   TRUE,
                                   me_mmgsdi_client_rsp_cb,
                                   0);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          DS_AT_MSG1_ERROR("mmgsdi_ok to refresh failed with return status %d",
                    mmgsdi_status);
        }
      }
      else if(mmgsdi_event_info->refresh_stage == 
                MMGSDI_REFRESH_STAGE_START)
      {
        dsatme_mmgsdi_refresh_fcn_read(mmgsdi_event_info);
      }
     }
      break;

#endif /* FEATURE_DSAT_ETSI_MODE */
#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_MMGSDI_PERSONALIZATION
        case MMGSDI_PERSO_EVT:
        if (IS_VALID_GW_APPS_ID(apps_id))
        {
          DS_AT_MSG2_HIGH("Perso feature = %d status = %d",
          mmgsdi_event_info->perso_info.feature,
          mmgsdi_event_info->perso_info.status);
          me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE); 
          if (NULL == me_ms_val) 
          {
            return DSAT_ASYNC_EVENT; 
          }

          perso_event = mmgsdi_event_info->perso_info.feature;
          switch (perso_event)
          {
            case MMGSDI_PERSO_NW:      /* NET Personalization PIN required */
              if(mmgsdi_event_info->perso_info.status == MMGSDI_PERSO_STATUS_LOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_NET_PIN_REQUIRED;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_UNLOCKED)
              {
                me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_NET_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_NET_PIN_REQUIRED;
                /* Check for disabling restricted command mode */
                respond = TRUE;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_BLOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_NET_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_NET_PIN_REQUIRED;
              }
              break;
      
            case MMGSDI_PERSO_NS:      /* NETSUB Personalization PIN required */
              if(mmgsdi_event_info->perso_info.status == 
                                                       MMGSDI_PERSO_STATUS_LOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_UNLOCKED)
              {
                me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED;
                /* Check for disabling restricted command mode */
                respond = TRUE;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_BLOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED;
              }
              break;
      
            case MMGSDI_PERSO_SP:      /* SP Personalization PIN required */
              if(mmgsdi_event_info->perso_info.status == 
                                                  MMGSDI_PERSO_STATUS_LOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_SP_PIN_REQUIRED;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_UNLOCKED)
              {
                me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_SP_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_SP_PIN_REQUIRED;
                /* Check for disabling restricted command mode */
                respond = TRUE;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_BLOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_SP_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_SP_PIN_REQUIRED;
              }
              break;
      
            case MMGSDI_PERSO_CP:      /* CORP Personalization PIN required */
              if(mmgsdi_event_info->perso_info.status == 
                                                 MMGSDI_PERSO_STATUS_LOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_CORP_PIN_REQUIRED;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_UNLOCKED)
              {
                me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_CORP_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_CORP_PIN_REQUIRED;
                /* Check for disabling restricted command mode */
                respond = TRUE;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_BLOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_CORP_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_CORP_PIN_REQUIRED;
              }
              break;
      
            case MMGSDI_PERSO_SIM:      /* SIM Personalization PIN required */
              if(mmgsdi_event_info->perso_info.status ==
                                                    MMGSDI_PERSO_STATUS_LOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_UNLOCKED)
              {
                me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED;
                /* Check for disabling restricted command mode */
                respond = TRUE;
              }
              else if(mmgsdi_event_info->perso_info.status == 
                   MMGSDI_PERSO_STATUS_BLOCKED)
              {
                me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED;
                me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED;
              }
              break;
              case MMGSDI_MAX_PERSO_FEATURE_ENUM:
              {
                if (mmgsdi_event_info->perso_info.status == MMGSDI_PERSO_STATUS_DONE)
                {
                  pin_bitmask = (uint32)(DSAT_CPIN_NET_PIN_REQUIRED |
                                         DSAT_CPIN_NETSUB_PIN_REQUIRED | 
                                         DSAT_CPIN_SP_PIN_REQUIRED |
                                         DSAT_CPIN_CORP_PIN_REQUIRED |
                                         DSAT_CPIN_PH_FSIM_PIN_REQUIRED);
                  me_ms_val->dsat_pin_required &= ~pin_bitmask;
                  me_ms_val->dsat_pin_blocked  &= ~pin_bitmask;
                  respond = TRUE;
                }
              }
              break;
            
            default:
              DS_AT_MSG1_MED("Unsupported MMGSDI personalization event: %d",
                      perso_event);
              break;
          }
#ifdef FEATURE_DATA_ETSI_PIN
            dsatcmdp_set_restricted_mode( me_is_restricting_pin_puk_set(apps_id),apps_id); 
#endif /* FEATURE_DATA_ETSI_PIN */
    
            if ( dsatme_pending_mmgsdi_command[apps_id] )
            {
              dsatme_pending_mmgsdi_command[apps_id] = FALSE;
          
              if ( respond )
              {
                result = DSAT_OK;
              }
            }
          }
          break;
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#endif /* FEATURE_DSAT_ETSI_MODE */

    default:
      /* appease lint */
      break;
  }
  return result;
} /* dsatme_mmgsdi_event_handler */


/*==============================================================================
FUNCTION  DSATME_MMGSDI_CSP_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for MMGSDI_USIM_CSP or MMGSDI_GSM_CSP.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_OK/DSAT_ERROR : if PACSP command is pending.
  DSAT_ASYNC_EVENT: if PACSP command is not pending
                    and reponse for FCN refresh read.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_csp_read_handler
(
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr /* MMGSDI SIM info pointer */
)
{
/* MACRO to set the result */
#define SEND_RESULT(result) \
        if ( dsat_pcasp_cmd_pending )\
        { \
          dsat_pcasp_cmd_pending = FALSE;\
          return result;\
        }\
        else \
        {\
          return DSAT_ASYNC_EVENT;\
        }
/*Each CSP entry consists of two bytes in CPHS:
  Byte A	- service group code 
  Byte B	- services byte 
*/
#define PLMN_MODE_BIT      0x80 /* MSB bit of Byte B of VAS is PLMN mode bit */
#define CPHS_VAS_OFFSET    0xC0 /* Value Added Service (VAS)group code*/

  dsat_result_enum_type result = DSAT_OK;
  uint16 index =0;
  me_plmn_mode_bit_e_type prev_csp_val = CSP_PLMN_MODE_BIT_UNSET;
  cb_cmd_e_type           refresh_mask = CB_NONE;
  dsat_apps_id_e_type apps_id;
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  dsatme_mmgsdi_state_ms_info   *me_ms_val    = NULL;

  DS_AT_MSG0_HIGH("In dstme_mmgsdi_csp_read_handler");
  apps_id    = cmd_rd_ptr->apps_id;
 
  if( (cmd_rd_ptr->client_ref & DSAT_MMGSDI_REFRESH_MASK_CB)== 
       DSAT_MMGSDI_REFRESH_MASK_CB )
   {
     DS_AT_MSG0_HIGH("dsatme_mmgsdi_opl_read_handler for Refresh file");
     refresh_mask = DSAT_MMGSDI_REFRESH_MASK_CB;
   }

  if (!IS_VALID_GW_APPS_ID(apps_id))
  {
    DS_AT_MSG2_ERROR("Invalid APPS id = %d, refresh_mask = %d",apps_id,refresh_mask);
    return DSAT_ASYNC_EVENT;
  }

  if( MMGSDI_SUCCESS != cmd_rd_ptr->cmd_status )
  {
    DS_AT_MSG0_ERROR("Reading of CSP file FAILED");
    SEND_FAIL_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id);
    SEND_RESULT(DSAT_ERROR)
  }
   ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,dsat_qcsimapp_info[apps_id].subs_id, TRUE);
   me_ms_val    = dsat_get_base_addr_per_session(apps_id, TRUE);
   if ((NULL == ph_ss_ms_val) || (NULL == me_ms_val))
   {
     return DSAT_ASYNC_EVENT;
   }

  while((index < MAX_MMGSDI_BUFFER_LEN )&&( index < cmd_rd_ptr->data_len ))
  {
    if(cmd_rd_ptr->data[index] == CPHS_VAS_OFFSET)
    {
      DS_AT_MSG1_HIGH("Value Added Service is present at index %d",index);
      break;
    }
    /*Look at every alternate byte for Service group code*/
    index = index + 2; 
  }

  /* Save the previous value to trigger the COPS to Automatic mode if required */
  prev_csp_val = me_ms_val->dsatme_plmn_mode_sel_bit;
  
  /* index < total length, then VAS group code is present */
  if( ( index < MAX_MMGSDI_BUFFER_LEN-1 ) && 
      ( index < cmd_rd_ptr->data_len) )
  {
    /* Check the next byte for PLMN Mode Bit */
    if(cmd_rd_ptr->data[index+1] & PLMN_MODE_BIT)
    {
      /* Manual PLMN Selection is selected */
      me_ms_val->dsatme_plmn_mode_sel_bit = CSP_PLMN_MODE_BIT_ON;
    }
    else
    {
      /* Manual PLMN Selection is not selected */
      me_ms_val->dsatme_plmn_mode_sel_bit = CSP_PLMN_MODE_BIT_OFF;
    }
    /* send update to the terminal */
    if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatme_report_pacsp_result_fp))
    {
      dsatdl_vtable.dsatme_report_pacsp_result_fp(apps_id);
    }
  }
  else
  { 
    /* No VAS record present in the SIM */
    DS_AT_MSG0_ERROR("Value Added Service is NOT present");
    SEND_PASS_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id);
    SEND_RESULT(DSAT_ERROR)
  }

  DS_AT_MSG3_HIGH("CSP prev val = %d current val = %d COPS mode = %d",prev_csp_val,
                          me_ms_val->dsatme_plmn_mode_sel_bit, (dsat_num_item_type)
                          dsatutil_get_val(DSATETSI_EXT_ACT_COPS_ETSI_IDX,apps_id,0,MIX_NUM_TYPE));
  /*
     When PLMN mode bit changes from 1 to 0 and the current mode is manual,
     it must be changed to Automatic mode 
  */
  
  if( ( me_ms_val->dsatme_plmn_mode_sel_bit == CSP_PLMN_MODE_BIT_OFF ) &&
      ( prev_csp_val == CSP_PLMN_MODE_BIT_ON ) &&
      ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_COPS_ETSI_IDX,
          apps_id,0,MIX_NUM_TYPE) == (dsat_num_item_type)DSAT_COPS_MODE_MANUAL))
  {
    /* Change to auto registration.  Set COPS mode accordingly */
    ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, dsat_qcsimapp_info[apps_id].subs_id, TRUE);
    if (NULL == ph_ss_ms_val)
    {
      return DSAT_ASYNC_EVENT;
    }
    
    DS_AT_MSG0_HIGH("PLMN mode bit changed from 1 to 0 change COPS mode to Auto ");
    ph_ss_ms_val->dsatetsicall_network_list.requested_pref.mode = DSAT_COPS_MODE_AUTO;
    ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_PREF;
    (void)dsatetsicmif_change_network_registration( DSAT_COPS_MODE_AUTO,
                                                    NULL,
                                                    DSAT_COPS_ACT_AUTO ,
                                                    (sys_modem_as_id_e_type)apps_id);
    DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,apps_id,0,0,
                                               DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
  }
  /* Send MMGSDI refresh complete set to TRUE for pending
     FCN notification,if any */
  SEND_PASS_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id);
  SEND_RESULT(result)
}/* dsatme_mmgsdi_csp_read_handler */



/*==============================================================================
FUNCTION  DSATME_EXTRACT_OPERATOR_NAME

DESCRIPTION
  Extracts the SPN name from EF-SPN, and decodes any UCS2 encoded information.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_OK:     If SPN files is succesfully read.
  DSAT_ERROR : if reading SPN failed.
  
SIDE EFFECTS
  None
==============================================================================*/
uint8 dsatme_extract_operator_name(char *spn_in, 
                                     uint16 spn_in_len,
                                     char *spn_out, 
                                     uint8 spn_out_len,
                                     dsati_chset_type *spn_type)
{
#define SPN_ENCODING_UCS2_TYPE_80  0x80
#define SPN_ENCODING_UCS2_TYPE_81  0x81
#define SPN_ENCODING_UCS2_TYPE_82  0x82

  uint8 i = 0, j=0;
  char *in_p = spn_in;
  char *out_p = spn_out;
  uint16 ucs2_base = 0;
  uint16 ucs2_out = 0;
  uint8 ucs2_len = 0;
  uint8 spn_len = 0;

  if ((spn_in == NULL) || (spn_out == NULL)) {
    return spn_len;
  }
  in_p++;
  /* The first octet, if 0x80, 0x81, 0x81 denotes the UCS2 encoding, else
   * the GSM encoding is used.
   */
  DS_AT_MSG1_HIGH("In dsatme_extract_operator_name : EF-SPN encoding used [ 0x%02x ]", spn_in[0]);
  switch (spn_in[0]) 
  {
    case SPN_ENCODING_UCS2_TYPE_80:
      for (i=0; (i<(spn_out_len-1)) && (j < (spn_in_len-1)); i+=2, j+=2, in_p+=2, out_p+=2) 
      {
        if (*in_p == 0xFF)
          break;
        else 
        {
          *(out_p+1) = *(in_p);
          *out_p = *(in_p+1);
        }
      }
      if (i > 0) {
        spn_len = i;
        *spn_type = ALPHA_UCS2;
      }
      break;
    case SPN_ENCODING_UCS2_TYPE_81:
      /* the second-octet is length of UCS2 chars in the EF-SPN */
      ucs2_len = *(in_p++);
      if ((ucs2_len > 0) && (ucs2_len < DSAT_MAX_SPN_NAME_LEN))
      {
        /* the third octet contains an 8-bit number which defines bits
         * 15 - 8 of a 16-base pointer, where bit 16 is set to 0, and bits 7 - 1 
         * are also set to zero. These 16 bits consitute a base pointer to a 
         * half-page in the UCS2 code space, to be used with some or all of the 
         * remaining octets in the string. 
         */
        ucs2_base = *(in_p++);
        ucs2_base <<= 7;
        
        for (i=0, j=0; (i < ucs2_len) && (i < spn_in_len) && (j < spn_out_len); i++, j+=2, in_p++, out_p+=2) 
        {
          /*                                        UNICODE              :  GSM-7bit */
          ucs2_out = (*(in_p) & 0x80) ? ( ucs2_base + (*(in_p) & 0x7F) ) : *(in_p);
          *out_p = (ucs2_out & 0xFF);
          *(out_p+1) = (ucs2_out >> 8);
        }
        spn_len = i*2;
        *spn_type = ALPHA_UCS2;
      }
      break;
    case SPN_ENCODING_UCS2_TYPE_82:
      ucs2_len = *(in_p++);
      if ((ucs2_len > 0) && (ucs2_len < DSAT_MAX_SPN_NAME_LEN)) 
      {
        /* the third and fourth octets contain a 16-bit number which defines the complete
         * 16-bit base pointer to a 'half-page' in the UCS2 code space, for use with some
         * or all of the remaining octets in the string. 
         */
        ucs2_base = *(in_p++);
        ucs2_base <<= 8;
        ucs2_base |= *(in_p++);

        for (i=0, j=0; (i < ucs2_len) && (j < spn_out_len); i++, j+=2, in_p++, out_p+=2) 
        {
          /*                                            UNICODE          :  GSM-7bit */
          ucs2_out = (*(in_p) & 0x80) ? ( ucs2_base + (*(in_p) & 0x7F) ) : *(in_p);
          *out_p = (ucs2_out & 0xFF);
          *(out_p+1) = (ucs2_out >> 8);
        }
        spn_len  = i*2;
        *spn_type = ALPHA_UCS2;
      }
      break;
    default:
      /* default encoding is GSM 7-bit  */
      (void)strlcpy((char*)spn_out, spn_in, DSAT_MAX_SPN_NAME_LEN );
      for (spn_len = 0; (spn_len < DSAT_MAX_SPN_NAME_LEN) && (spn_out[spn_len] != 0xFF) ; spn_len++);
      *spn_type = ALPHA_GSM;
  }

  return spn_len;
} /* dsatme_extract_operator_name */


/*==============================================================================
FUNCTION  DSATME_MMGSDI_SPN_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_SPN or UIM_GSM_SPN.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_OK:     If SPN files is succesfully read.
  DSAT_ERROR : if reading SPN failed.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_spn_read_handler
(
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr /* MMGSDI SIM info pointer */
)
{
#define DISPLAY_COND_INDEX  0
#define OPERATOR_NAME_INDEX 1
  cb_cmd_e_type              refresh_mask = CB_NONE;
  dsat_apps_id_e_type apps_id;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  DS_AT_MSG0_HIGH("In dstme_mmgsdi_spn_read_handler");
  apps_id = cmd_rd_ptr->apps_id;
 
  if( (cmd_rd_ptr->client_ref & DSAT_MMGSDI_REFRESH_MASK_CB)== 
       DSAT_MMGSDI_REFRESH_MASK_CB )
   {
     DS_AT_MSG0_HIGH("dsatme_mmgsdi_opl_read_handler for Refresh file");
     refresh_mask = DSAT_MMGSDI_REFRESH_MASK_CB;
   }

  if (!IS_VALID_GW_APPS_ID(apps_id))
  {
    DS_AT_MSG2_ERROR("Invalid APPS id = %d, refresh_mask = %d",apps_id,refresh_mask);
    return DSAT_ASYNC_EVENT;
  }

  me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE);
  if (NULL == me_ms_val)
  {
    return DSAT_ASYNC_EVENT;
  }
 
  if( MMGSDI_SUCCESS != cmd_rd_ptr->cmd_status )
  {
    DS_AT_MSG0_ERROR("Reading of SPN file FAILED");
    SEND_FAIL_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id)
    return DSAT_ASYNC_EVENT;
  }
  memset((void *)&me_ms_val->dsat_spn_info,0x00,sizeof(dsat_mmgsdi_spn_info_s_type));
  me_ms_val->dsat_spn_info.operator_name_len = 0;
  /* Byte 0 contains the display condition */
  me_ms_val->dsat_spn_info.display_cond = cmd_rd_ptr->data[DISPLAY_COND_INDEX];

  /* From Byte 1 to length -1, the operator name is present */
  if ( cmd_rd_ptr->data[OPERATOR_NAME_INDEX] != 0xFF )
  {

    me_ms_val->dsat_spn_info.operator_name_len = 
      dsatme_extract_operator_name((char*)&cmd_rd_ptr->data[OPERATOR_NAME_INDEX], 
                                   (MAX_MMGSDI_BUFFER_LEN - 1),
                                   (char*)me_ms_val->dsat_spn_info.operator_name,
                                   DSAT_MAX_SPN_NAME_LEN, 
                                   &me_ms_val->dsat_spn_info.encoding_type);
    me_ms_val->dsat_spn_info.spn_info_valid = TRUE;
  }
  else
  {
    DS_AT_MSG1_ERROR("SPN Display Info is invalid %x",
                    cmd_rd_ptr->data[OPERATOR_NAME_INDEX]);
    me_ms_val->dsat_spn_info.spn_info_valid = FALSE;
  }
  SEND_PASS_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id);

  return DSAT_ASYNC_EVENT;
}/* dsatme_mmgsdi_spn_read_handler */

/*==============================================================================
FUNCTION  DSATME_MMGSDI_IMSI_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_IMSI or UIM_GSM_IMSI.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ASYNC_EVENT: Always. 
                  But, if failed, SEND_FAIL_MMGSDI_REFRESH_COMPLETE else
                  SEND_PASS_MMGSDI_REFRESH_COMPLETE() is sent. 
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_imsi_read_handler
(
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr /* MMGSDI SIM info pointer */
)
{
  cb_cmd_e_type              refresh_mask = CB_NONE;
  dsat_apps_id_e_type apps_id;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  apps_id = cmd_rd_ptr->apps_id;

  DS_AT_MSG1_HIGH("File IMSI read handler,read record len %d",cmd_rd_ptr->data_len);
  me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE);
  if (NULL == me_ms_val)
  {
    return DSAT_ASYNC_EVENT;
  }

  if( (cmd_rd_ptr->client_ref & DSAT_MMGSDI_REFRESH_MASK_CB)== 
       DSAT_MMGSDI_REFRESH_MASK_CB )
   {
     DS_AT_MSG0_HIGH("dsatme_mmgsdi_IMSI_read_handler for Refresh file");
     refresh_mask = DSAT_MMGSDI_REFRESH_MASK_CB;
   }
  if (!IS_VALID_GW_APPS_ID(apps_id))
  {
    DS_AT_MSG2_ERROR("Invalid APPS id = %d, refresh_mask = %d",apps_id,refresh_mask);
    return DSAT_ASYNC_EVENT;
  }
  
  if( MMGSDI_SUCCESS != cmd_rd_ptr->cmd_status )
  {
    DS_AT_MSG0_ERROR("Reading of IMSI file FAILED");
    SEND_FAIL_MMGSDI_REFRESH_COMPLETE(refresh_mask,cmd_rd_ptr->apps_id);
    return DSAT_ASYNC_EVENT;
  }

  memset((void *)&me_ms_val->dsat_imsi_info,0x00,sizeof(dsat_mmgsdi_imsi_info_s_type));
   /* Copy the entire data */

  (void) dsatutil_memscpy((void*)&me_ms_val->dsat_imsi_info.data[0],
          MMGSDI_IMSI_LEN,(void*)&cmd_rd_ptr->data[0],
           MIN(cmd_rd_ptr->data_len, MAX_MMGSDI_BUFFER_LEN) ); 
  me_ms_val->dsat_imsi_info.len =(uint16) cmd_rd_ptr->data_len;

  SEND_PASS_MMGSDI_REFRESH_COMPLETE(refresh_mask,apps_id);
  return DSAT_ASYNC_EVENT;
}/* dsatme_mmgsdi_imsi_read_handler */


/*===========================================================================

FUNCTION  DSATME_MMGSDI_SIM_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_mmgsdi_sim_event_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  cb_cmd_e_type client_ref; 
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  ds_at_mmgsdi_sim_info_type *sim_info = dsat_get_cmd_payload_ptr(cmd_ptr);

  sim_info->apps_id = dsatme_update_appsid(sim_info->session_id);
  if (!IS_VALID_APPS_ID(sim_info->apps_id))
  {
    DS_AT_MSG1_ERROR("Invalid APPS id = %d", sim_info->apps_id);
    return DSAT_ASYNC_EVENT;
  }

  client_ref = (cb_cmd_e_type)sim_info->client_ref;
  DS_AT_MSG1_HIGH("ATCoP MMGSDI SIM event handler with client_ref = %d",
                                                      client_ref);

  if ( ( client_ref & DSAT_MMGSDI_REFRESH_MASK_CB ) == 
                                           DSAT_MMGSDI_REFRESH_MASK_CB )
  {
    DS_AT_MSG0_HIGH("dsatme_mmgsdi_sim_event_handler_ex for Refresh file");
    client_ref = (cb_cmd_e_type)((int32)client_ref & (int32)(~DSAT_MMGSDI_REFRESH_MASK_CB));
  }
  switch( client_ref )
  {
#ifdef FEATURE_MMGSDI
    case DSAT_MMGSDI_CSP_READ:
      (void)dsatme_mmgsdi_csp_read_handler( sim_info );
      break;
    case DSAT_MMGSDI_SPN_READ:
      (void)dsatme_mmgsdi_spn_read_handler( sim_info );
      break;
    case DSAT_MMGSDI_IMSI_READ:
      (void)dsatme_mmgsdi_imsi_read_handler ( sim_info ); 
       break;
#endif /* FEATURE MMGSDI */
    default:
      DS_AT_MSG1_ERROR("Unsupported MMGSDI SIM read event: %d",
                           sim_info->client_ref);
  }
  
  if(client_ref == DSAT_MMGSDI_CSP_READ ||
     client_ref == DSAT_MMGSDI_SPN_READ ||
     client_ref == DSAT_MMGSDI_IMSI_READ )
  {
    return result;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatme_mmgsdi_sim_event_handler_fp))
  {
    result = dsatdl_vtable.dsatme_mmgsdi_sim_event_handler_fp(sim_info);
  }

  return result;
} /* dsatme_mmgsdi_sim_event_handler */

/*===========================================================================

FUNCTION DSATME_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void dsatme_mmgsdi_req_cb
(
  mmgsdi_return_enum_type status, /* MMGSDI return status */
  mmgsdi_cnf_enum_type    cnf,    /* Type of command confirmation */
  const mmgsdi_cnf_type *cnf_ptr  /* Pointer to confirmation data */
)
{
  byte data = 0;
  byte data1 = 0;
  mmgsdi_session_id_type application_id = 0;
  cb_cmd_e_type client_data = (cb_cmd_e_type)cnf_ptr->response_header.client_data;
  
  DEBUG ( ASSERT(cb_expected == (cb_cmd_e_type)cnf_ptr->response_header.client_data) ;);

  DS_AT_MSG3_HIGH("Status = %d, Response type = %d, Client data = %d",
    status,
    cnf_ptr->response_header.response_type,
    cnf_ptr->response_header.client_data);

  application_id = cnf_ptr->response_header.session_id;

  switch (cnf_ptr->response_header.response_type)
  {
    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
       switch (cnf_ptr->response_header.client_data) 
       {
         case CLCK_MMGSDI_PIN1_STATUS:
         case CPIN_MMGSDI_PIN1_STATUS:
           data = (byte)cnf_ptr->get_all_pin_status_cnf.pin1.status;
           break;
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
         case CPBS_MMGSDI_PIN2_STATUS:
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
         case CPIN_MMGSDI_PIN2_STATUS:
           data = (byte)cnf_ptr->get_all_pin_status_cnf.pin2.status;
           break;
         case CPIN_MMGSDI_ALL_PIN_STATUS:
           /* Submit pin1 and pin2 status as seperate cmds for handling */
           client_data = CPIN_MMGSDI_PIN1_STATUS;
           data = (byte)cnf_ptr->get_all_pin_status_cnf.pin1.status;
           data1 = (byte)cnf_ptr->get_all_pin_status_cnf.pin2.status;
           break;
         default:
             /* Nothing to do here; to appease lint */
           break;
        }
      break;

    case MMGSDI_PIN_OPERATION_CNF:
      DS_AT_MSG2_HIGH("pin retries = %d, puk retries = %d",
              cnf_ptr->pin_operation_cnf.pin_info.num_retries,
              cnf_ptr->pin_operation_cnf.pin_info.num_unblock_retries);
      data = (byte) cnf_ptr->pin_operation_cnf.pin_info.num_retries;

      if (data == 0) /* PIN retries are over, so copy PUK data */
      {
        data = (byte) cnf_ptr->pin_operation_cnf.pin_info.num_unblock_retries;
      }
      break;

    default:
      DS_AT_MSG1_ERROR("Not a valid mmgsdi cb rsp: %d", 
           cnf_ptr->response_header.response_type);
      break;
  }

  submit_cmd_status( client_data, 
                     status,
                     application_id,
                     data );

  if(data1 != 0)
  {
    /* Submit pin2 status */
    submit_cmd_status( CPIN_MMGSDI_PIN2_STATUS, 
                       status,
                       application_id,
                       data1 );
  }

  cb_expected = CB_NONE;
} /* dsatme_mmgsdi_req_cb */


/*===========================================================================

FUNCTION SUBMIT_CMD_STATUS

DESCRIPTION
  This function send DSTASK a status cmd which will be processed by ATCOP.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
void submit_cmd_status 
(
  cb_cmd_e_type cmd_id,         /* who submitted this status */
  mmgsdi_return_enum_type status,         /* cmd status */
  mmgsdi_session_id_type         session_id,        /* Which apps_id  */
  byte          data            /* cmd data */
)
{
  ds_cmd_type            *cmd_ptr;
  ds_at_cmd_status_type  *status_cmd_ptr = NULL;

  /* tell atcop that the async return is done */
  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_cmd_status_type), FALSE);
  status_cmd_ptr = cmd_ptr->cmd_payload_ptr;

  cmd_ptr->hdr.cmd_id                = DS_CMD_ATCOP_STATUS_CMD;
  status_cmd_ptr->cmd_id                 = (uint16) cmd_id;
  status_cmd_ptr->cmd_status             = status;
  status_cmd_ptr->cmd_data               = data;
  status_cmd_ptr->session_id             = session_id;
  status_cmd_ptr->is_status_cmd_handling = TRUE;
  ds_put_cmd(cmd_ptr);
} /* submit_cmd_status */

/*===========================================================================

FUNCTION DSATME_CMD_STATUS_HANDLER_EX

DESCRIPTION
  This function is the async event handler for async functions such as
  pbm_file_info, pbm_read, pbm_write, pbm_find_name and gsdi pin requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_cmd_status_handler_ex
(
  ds_cmd_type * cmd_ptr
)
{
  ds_at_cmd_status_type *status_cmd_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) || defined FEATURE_DSAT_ETSI_MODE
  int index;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) || FEATURE_DSAT_ETSI_MODE */
  dsat_result_enum_type result;

  /*Initialize it to default*/
  status_cmd_ptr->apps_id = 0;
  if(status_cmd_ptr->is_status_cmd_handling == TRUE)
  {
    status_cmd_ptr->apps_id = dsatme_update_appsid(status_cmd_ptr->session_id);
  }
  DS_AT_MSG3_HIGH("Command status handler = %d Apps _id %d Session _id %d",
                  status_cmd_ptr->cmd_id,
                  status_cmd_ptr->apps_id,
                  status_cmd_ptr->session_id);
  if (!IS_VALID_APPS_ID(status_cmd_ptr->apps_id))
  {
    return DSAT_ASYNC_EVENT;
  }
  /* Default action is to return the incoming cmd status */
  result = (dsat_result_enum_type)status_cmd_ptr->cmd_status;

  if (!IS_VALID_GW_APPS_ID(status_cmd_ptr->apps_id)
#ifdef FEATURE_DSAT_CDMA_PIN
      ||((status_cmd_ptr->cmd_id >= DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS) &&
         (status_cmd_ptr->cmd_id <= DSAT_CDMA_CPWD_MMGSDI_PIN2_CHANGE))
#endif /* FEATURE_DSAT_CDMA_PIN */
     )
  {
#ifdef FEATURE_DSAT_CDMA_PIN
    result = dsat707_status_event_handler_ex(status_cmd_ptr);
#endif /* FEATURE_DSAT_CDMA_PIN */
  }
  else
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    /* Check for cmd status handler */
    for(index = 0; status_handler_table_ex[index].cmd_id != CB_NONE; index++)
    {
      if( status_handler_table_ex[index].cmd_id ==
          (cb_cmd_e_type)status_cmd_ptr->cmd_id )
      {
        result = 
          status_handler_table_ex[index].status_handler (status_cmd_ptr);
      }
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatme_cmd_status_handler_fp))
  {
    result = dsatdl_vtable.dsatme_cmd_status_handler_fp(status_cmd_ptr);
  }

  return result;
} /* dsatme_cmd_status_handler_ex */


/* ----------------------------------------------------------------------------
   FUNCTION:      DSATME_MMGSDI_READ_TRANSPARENT_CB

   DESCRIPTION:
     This function provides internal callback function pointer to internal
     processing that MMGSDI does not care for the callback information returned

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
/* ARGSUSED */
void dsatme_mmgsdi_read_transparent_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type                *cmd_buf;
  ds_at_mmgsdi_sim_info_type *sim_info = NULL;

  uint32        temp_client_ref;
  dsat_result_enum_type  result = DSAT_OK;
  ASSERT( cnf_ptr != NULL );

  temp_client_ref = cnf_ptr->read_cnf.response_header.client_data;
  DS_AT_MSG2_HIGH(" dsatme_mmgsdi_read_transparent_cb with client ref %d status %d",
                              temp_client_ref,status);

  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_event_type), FALSE);
  sim_info = cmd_buf->cmd_payload_ptr;

  if ( ( temp_client_ref & DSAT_MMGSDI_REFRESH_MASK_CB ) == 
                                        DSAT_MMGSDI_REFRESH_MASK_CB )
  {
    DS_AT_MSG0_HIGH(" dsatme_mmgsdi_read_transparent_cb for Refresh file");
  }
  
  temp_client_ref = temp_client_ref & ~DSAT_MMGSDI_REFRESH_MASK_CB;
  
  switch ( temp_client_ref )
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    case CPOL_MMGSDI_SIM_READ:
    {
      if( status == MMGSDI_SUCCESS )
      {
        sim_info->cmd_status = MMGSDI_SUCCESS; 
        sim_info->client_ref = 
                           cnf_ptr->read_cnf.response_header.client_data;
      }
      else
      {
        sim_info->cmd_status =  MMGSDI_ERROR;
        if (status == MMGSDI_ACCESS_DENIED )
        {
          sim_info->client_ref = 
                                     (uint32)CPOL_MMGSDI_SIM_ACCESS_DENIED;
        }    
        else
        {
          sim_info->client_ref = (uint32)CPOL_MMGSDI_SIM_ERROR;
        }
      }
    }/* case: CPOL_MMGSDI_SIM_READ */
    break;
#endif /* FEATURE_DSAT_ETSI_MODE */
    case DSAT_MMGSDI_CSP_READ:
    case DSAT_MMGSDI_SPN_READ:
    case DSAT_MMGSDI_IMSI_READ:
      sim_info->cmd_status = status; 
      sim_info->client_ref = 
                           cnf_ptr->read_cnf.response_header.client_data;
      break;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    case ICCID_MMGSDI_SIM_READ:
      sim_info->cmd_status = status; 
      sim_info->client_ref = 
                           cnf_ptr->read_cnf.response_header.client_data;
      break;
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
    default:
      DS_AT_MSG1_ERROR(" Invalid Client ref %d in read transparent cb clearing cmd buf",
                              temp_client_ref);
      ds_release_cmd_buf(&cmd_buf);
      result = DSAT_ERROR;

  }/* switch ( temp_client_ref ) */
  if(DSAT_OK == result )
  {
  /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id                = DS_CMD_ATCOP_MMGSDI_SIM_INFO;
    sim_info->data_len = 
                    cnf_ptr->read_cnf.read_data.data_len;
    sim_info->session_id = cnf_ptr->response_header.session_id;
  if(NULL != cnf_ptr->read_cnf.read_data.data_ptr)
  {
  /* Do not overrun the buffers so copy based on the MIN size */
  (void) dsatutil_memscpy((void*)sim_info->data,
          MAX_MMGSDI_BUFFER_LEN,(void*)cnf_ptr->read_cnf.read_data.data_ptr,
          sim_info->data_len);
  }
  else
  {
    sim_info->data_len = 0;
  }
    ds_put_cmd(cmd_buf);
  }
} /* dsatme_mmgsdi_read_transparent_cb */

/*===========================================================================
FUNCTION:      DSATME_MMGSDI_READ_RECORD_CB

   DESCRIPTION:
     This function provides internal callback function pointer to internal
  processing that MMGSDI does not care for the callback information returned.

   DEPENDENCIES:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=========================================================================== */
/* ARGSUSED */
void dsatme_mmgsdi_read_record_cb 
(
  mmgsdi_return_enum_type           status, 
  mmgsdi_cnf_enum_type              cnf, 
  const mmgsdi_cnf_type            *cnf_ptr
)
{
  uint32                  client_ref;
  uint32                  max_data_size = 0;
  ds_cmd_type            *cmd_ptr;
  ds_at_mmgsdi_sim_info_type *sim_info  = NULL;

  ASSERT( cnf_ptr != NULL );

  /* allocate command buffer */
  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_sim_info_type), FALSE);
  sim_info = cmd_ptr->cmd_payload_ptr;

  client_ref = cnf_ptr->read_cnf.response_header.client_data;
  if ( cnf_ptr->read_cnf.response_header.client_data & DSAT_MMGSDI_REFRESH_MASK_CB )
  {
    DS_AT_MSG0_HIGH("dsatme_mmgsdi_read_record_cb for Refresh file");
  }
  client_ref = (cnf_ptr->read_cnf.response_header.client_data & 
               ~DSAT_MMGSDI_REFRESH_MASK_CB);
  max_data_size = MIN(MAX_MMGSDI_BUFFER_LEN,(uint32)cnf_ptr->read_cnf.read_data.data_len );
  
  DS_AT_MSG2_HIGH("ATCoP MMGSDI Read record cb with status %d data len %d",
                           status,cnf_ptr->read_cnf.read_data.data_len);
  switch ( client_ref )
  {
    case DSAT_MMGSDI_CSP_READ:
    case DSAT_MMGSDI_SPN_READ:
    case DSAT_MMGSDI_IMSI_READ:
    case CUAD_MMGSDI_SIM_READ:
      cmd_ptr->hdr.cmd_id                    = DS_CMD_ATCOP_MMGSDI_SIM_INFO;
      sim_info->cmd_status= status;
      sim_info->client_ref = 
                cnf_ptr->read_cnf.response_header.client_data;
      sim_info->data_len  = 
                                         max_data_size;
      sim_info->session_id = cnf_ptr->response_header.session_id;
      
      sim_info->accessed_rec_num = 
                             cnf_ptr->read_cnf.accessed_rec_num;
      sim_info->accessed_offset = 
                             cnf_ptr->read_cnf.accessed_offset;     
      if(NULL != cnf_ptr->read_cnf.read_data.data_ptr)
      {
         (void) dsatutil_memscpy((void*)&sim_info->data[0],
                MAX_MMGSDI_BUFFER_LEN,(void*)cnf_ptr->read_cnf.read_data.data_ptr,
                 max_data_size);
      }
      else
      {
        sim_info->data_len = 0;
      }
      if(client_ref == CUAD_MMGSDI_SIM_READ)
      {
        sim_info->accessed_rec_num = cnf_ptr->read_cnf.accessed_rec_num;
      }
      break;
    default:
       DS_AT_MSG1_ERROR("Currently  client ref %d is not supported",client_ref);
       ASSERT(0); /* for testing purpose */
  }

  ds_put_cmd(cmd_ptr);

}/* dsatme_mmgsdi_read_cb */

/*===========================================================================

FUNCTION DSATME_REQ_MMGSDI_SIM_READ

DESCRIPTION
  This function sends MMGSDI a message to read the EF from the card selected
  by $QCSLOT value. The input parameter maps to a specific EF.
                  

DEPENDENCIES
  None

RETURN VALUE
  returns boolean:
    FALSE: if dsat_qcslot_val is not GSM operational slot
    TRUE:  executing the MMGSDI async call and waiting for the callback function
           to be called.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatme_req_mmgsdi_sim_read
(
  cb_cmd_e_type                   cb_cmd,
  mmgsdi_file_structure_enum_type file_type,
  uint32                          rec_num,
  dsat_apps_id_e_type             apps_id
)
{
  mmgsdi_access_type  file_access;
  file_access.file.file_enum = MMGSDI_NO_FILE_ENUM;
  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;

  /* Convert the call back command type to the UIM file name */
#ifdef FEATURE_DSAT_ETSI_MODE
  etsime_map_cmd_to_file( &file_access.file.file_enum, cb_cmd, apps_id);
#endif /* FEATURE_DSAT_ETSI_MODE */

  if ( MMGSDI_TRANSPARENT_FILE == file_type )
  {
    DS_AT_MSG3_HIGH("ATCoP MMGSDI Read Transparent for file name %d type %d rec num %d",
                                                file_access.file.file_enum, file_type, rec_num);
    if ( MMGSDI_SUCCESS == mmgsdi_session_read_transparent(
                dsat_qcsimapp_info[apps_id].session_id,
                       file_access,
                       0,
                       0,
                      dsatme_mmgsdi_read_transparent_cb,
                      (mmgsdi_client_data_type)cb_cmd ))
    {
      return TRUE;
    }
    else
    {
      DS_AT_MSG0_ERROR(" ATCoP MMGSDI Read Transparent failed");
      return FALSE;
    }
  }
  else if ( ( MMGSDI_LINEAR_FIXED_FILE == file_type ) ||
            ( MMGSDI_CYCLIC_FILE  == file_type ))
  {  
    DS_AT_MSG3_HIGH("ATCoP MMGSDI Read Record for file name %d type %d rec num %d",
                                                file_access.file.file_enum, file_type, rec_num);
    /* Call MMGSDI API to read record */
    if (MMGSDI_SUCCESS == mmgsdi_session_read_record(
                         dsat_qcsimapp_info[apps_id].session_id,
                            file_access,
                            (mmgsdi_rec_num_type)rec_num,
                            0,
                            (mmgsdi_callback_type)dsatme_mmgsdi_read_record_cb,
                            (mmgsdi_client_data_type)cb_cmd))
    {
      return TRUE;
    }
    else 
    {
      DS_AT_MSG0_ERROR(" ATCoP MMGSDI Read Record failed");
      return FALSE;
    }
  }
  else
  {
    DS_AT_MSG0_ERROR(" Invalid file type for MMGSDI read");
    return FALSE;
  }
} /* dsatme_req_mmgsdi_sim_read*/


/*===========================================================================
FUNCTION DSATME_READ_UIM_FILES

DESCRIPTION
  This is an internal function, which reads all the MMGSDI files at initalization.. 
  
DEPENDENCIES
  Whenever a new file is being used in ATCOP, this function needs to be checked
  for addition.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_read_uim_files ( dsat_apps_id_e_type apps_id )
{

  cb_cmd_e_type cb_cmd_list[] = {
                                 DSAT_MMGSDI_CSP_READ ,
                                 DSAT_MMGSDI_IMSI_READ,
                                 DSAT_MMGSDI_SPN_READ ,
                                 CB_NONE
                                      };
  uint16 i;
  mmgsdi_file_structure_enum_type file_type = MMGSDI_TRANSPARENT_FILE;

  DS_AT_MSG0_HIGH("ATCoP read all CSP , SPN , IMSI files");
  for( i=0; (i < ARR_SIZE(cb_cmd_list)) && (cb_cmd_list[i] != CB_NONE) ; ++i )
  {
    if ( FALSE == dsatme_req_mmgsdi_sim_read( cb_cmd_list[i],file_type,0, apps_id ) )
    {
      DS_AT_MSG1_ERROR("Initial read failed for file %d",cb_cmd_list[i]);
    }
  }
}/*dsatme_read_uim_files*/

/*===========================================================================
FUNCTION ME_MMGSDI_CLIENT_RSP_CB

DESCRIPTION
  This function handles the MMGSDI client event response callback.  


DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void me_mmgsdi_client_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
  DS_AT_MSG2_HIGH("MMGSDI response call back for cnf=%d with status=%d",
     cnf,status);
}/* me_mmgsdi_client_rsp_cb */

/*===========================================================================
FUNCTION ME_MMGSDI_REFRESH_COMPLETE

DESCRIPTION
  Called within ATCoP to indicate that it has completed reading the files it
  was notified to read.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void me_mmgsdi_refresh_complete
(
  boolean               pass_fail,
  dsat_apps_id_e_type   apps_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  DS_AT_MSG1_HIGH("Sending MMGSDI Refresh complete pass_fail(%d)",(int)pass_fail);
  mmgsdi_status = mmgsdi_session_refresh_complete(
                                         dsat_qcsimapp_info[apps_id].session_id,
                                         pass_fail,
                                         me_mmgsdi_client_rsp_cb,
                                         0);
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DS_AT_MSG0_ERROR("ATCoP failed to post REFRESH COMPLETE to MMGSDI");
  }

} /* me_mmgsdi_refresh_complete */

/*==============================================================================
FUNCTION  DSATME_MMGSDI_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register ATCoP for File Change Notifications for EF's.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void dsatme_mmgsdi_register_for_refresh
(
  dsat_apps_id_e_type apps_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type refresh_files = {0};
  mmgsdi_file_enum_type file_list[] = {
                      MMGSDI_GSM_CSP,
                      MMGSDI_GSM_IMSI,
                      MMGSDI_GSM_SPN,
                      MMGSDI_USIM_CSP,
                      MMGSDI_USIM_SPN,
                      MMGSDI_USIM_IMSI
                      }; 

  refresh_files.num_files = ARR_SIZE(file_list);
  refresh_files.file_list_ptr = &file_list[0];

  DS_AT_MSG0_HIGH("Sending request for file change notification");
  /* Register with MMGSDI for File Change Notification using the file list
    in the array above, Voting set to TRUE so as not to override DS3G registration */
  mmgsdi_status = mmgsdi_session_register_for_refresh(
             dsat_qcsimapp_info[apps_id].session_id,
             refresh_files,
             TRUE,
             me_mmgsdi_client_rsp_cb,
             0);

  /* Check to see if REQ for Registration was OK.  */
  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
    DS_AT_MSG0_ERROR("Failed to Request Refresh FCN Registration");
  }
} /* dsatme_mmgsdi_register_for_refresh */

/*===========================================================================
FUNCTION DSATME_MMGSDI_REFRESH_FCN_READ

DESCRIPTION
  Handles the asynchronous response of FCN refesh.
  It reads the EF's present in FCN indication.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatme_mmgsdi_refresh_fcn_read
(
  ds_at_mmgsdi_event_type *mmgsdi_event_info /* MMGSDI event info pointer */
)
{
  boolean pass_fail = TRUE;
  uint8 index =0;
  cb_cmd_e_type cb_cmd = CB_NONE;
  cb_cmd_e_type refresh_mask = CB_NONE;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  dsat_apps_id_e_type apps_id;
  mmgsdi_file_structure_enum_type file_type = MMGSDI_TRANSPARENT_FILE;

  apps_id = mmgsdi_event_info->apps_id;  
  me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE);
  if (NULL == me_ms_val)
  {
    return;
  }
  me_ms_val->dsat_refresh_fcn_ind.num_files = 0;
  
  (void) dsatutil_memscpy((void*)&me_ms_val->dsat_refresh_fcn_ind,
          sizeof(dsat_fcn_ind_type),
          (void*)&mmgsdi_event_info->u.fcn_ind,
  	      sizeof(dsat_fcn_ind_type));

  if( (0 == me_ms_val->dsat_refresh_fcn_ind.num_files)||
      (me_ms_val->dsat_refresh_fcn_ind.num_files > DSAT_MAX_NUM_REFRESH_FILES))
  {
    DS_AT_MSG1_HIGH("MMGSDI refresh complete has %d files pass_fail = FALSE",
                                               me_ms_val->dsat_refresh_fcn_ind.num_files);
    pass_fail = FALSE;
    refresh_mask = DSAT_MMGSDI_REFRESH_MASK_CB;
  }
  else
  {
    DS_AT_MSG1_HIGH("MMGSDI refresh complete has %d number of files",
                                               me_ms_val->dsat_refresh_fcn_ind.num_files);
    while((index < me_ms_val->dsat_refresh_fcn_ind.num_files)&& (FALSE != pass_fail))
    {
      switch(me_ms_val->dsat_refresh_fcn_ind.file_list[index])
      {
        case MMGSDI_GSM_CSP:
        case MMGSDI_USIM_CSP:
          cb_cmd = (cb_cmd_e_type)( (int32) DSAT_MMGSDI_CSP_READ  | 
                     (int32) DSAT_MMGSDI_REFRESH_MASK_CB);
          break;
        case MMGSDI_GSM_SPN:
        case MMGSDI_USIM_SPN:
          cb_cmd = (cb_cmd_e_type)( (int32)DSAT_MMGSDI_SPN_READ  |
                     (int32)DSAT_MMGSDI_REFRESH_MASK_CB );
          break;
        case MMGSDI_GSM_IMSI:
        case MMGSDI_USIM_IMSI:
            cb_cmd = (cb_cmd_e_type)( (int32)DSAT_MMGSDI_IMSI_READ | 
                         (int32)DSAT_MMGSDI_REFRESH_MASK_CB );
          break;
        default:
          DS_AT_MSG0_ERROR("Unknown file in file list ,pass_fail = FALSE");
          refresh_mask = DSAT_MMGSDI_REFRESH_MASK_CB;
          SEND_PASS_MMGSDI_REFRESH_COMPLETE_EXT(refresh_mask,apps_id);
          break;
      }/* Switch end */
      if ( CB_NONE != cb_cmd )
      {
        if ( FALSE == dsatme_req_mmgsdi_sim_read( cb_cmd,file_type,0 ,apps_id) )
        {
          DS_AT_MSG1_ERROR("Initial read failed for file %d",cb_cmd);
          
        }
      }
      /* Update the variables for next loop */
      cb_cmd = CB_NONE;
      index++;
    }/* While end */
  }
  if ( FALSE == pass_fail )
  {
    SEND_FAIL_MMGSDI_REFRESH_COMPLETE_EXT(refresh_mask,apps_id);
  }
} /* dsatme_mmgsdi_refresh_fcn_read */

/*===========================================================================

FUNCTION CPIN_PROCESS_PIN_STATUS_EX

DESCRIPTION
  This function is called after the MMGSDI fulfilled the request to
  get the pin status.  The MMGSDI reply is in cmd_status.

  If status shows the ME is pin locked, continue to verify the password.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD if the request is sent to mmgsdi to verify pin
                 or sent to pbm to set the storage type.
  DSAT_CMD_ERR_RSP if a me error happens when pbm executes the 
                   set storage request.  A cme error is returned
                   to TE.
  DSAT_ERROR if the set storage request is refused by PBM.
             It should not happen.  A system ERR is submitted.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpin_process_pin_status_ex
(
   ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  mmgsdi_pin_status_enum_type pin_status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
  boolean pin_locked = FALSE;
  mmgsdi_pin_enum_type  pin_id = MMGSDI_PIN1;
  mmgsdi_events_enum_type event = MMGSDI_MAX_EVT_ENUM;

  cb_cmd_e_type handler = CB_NONE;
  DS_AT_MSG1_HIGH("Command id = %d",cmd_info_ptr->cmd_id);

  /* Ensure MMGSDI call was successful */
  if ( MMGSDI_SUCCESS != cmd_info_ptr->cmd_status )
  {
    DS_AT_MSG0_HIGH("MMGSDI call not successful");
    result = DSAT_ERROR;
  }
  else
  {
    pin_status = (mmgsdi_pin_status_enum_type)cmd_info_ptr->cmd_data;
    DS_AT_MSG2_HIGH("command id = %d apps_id = %d",cmd_info_ptr->cmd_id,cmd_info_ptr->apps_id);
    /* Process the PIN state */
    switch (cmd_info_ptr->cmd_id)
    {
    case CPIN_MMGSDI_PIN1_STATUS:
      pin_id = MMGSDI_PIN1;
      handler = CPIN_MMGSDI_PIN1_VERIFY;
      event = MMGSDI_PIN1_EVT;
      pin_locked = (cmd_info_ptr->cmd_data != (int16)MMGSDI_PIN_DISABLED);
      break;
      
    case CPIN_MMGSDI_PIN2_STATUS:
      pin_id = MMGSDI_PIN2;
      event = MMGSDI_PIN2_EVT;
      handler = CPIN_MMGSDI_PIN2_VERIFY;
      pin_locked = (cmd_info_ptr->cmd_data != (int16)MMGSDI_PIN_DISABLED);
      break;

    default:
      DS_AT_MSG1_MED("Invalid PIN Status cmd_id: %d",cmd_info_ptr->cmd_id);
      result = DSAT_ERROR;
      break;
    }
  }
  
  if (DSAT_ASYNC_CMD == result)
  {
    /* Update PIN state tracking */
    if (DSAT_OK != cpin_track_pin_status (event, pin_status,cmd_info_ptr->apps_id))
    {
      DS_AT_MSG0_ERROR("Problem updating PIN status");
      result = DSAT_ERROR;
    }
    else
    {
      mmgsdi_data_type pin_data;

      pin_data.data_ptr = (uint8 *)dsatutil_get_val(
               DSATETSI_EXT_ACT_CPIN_ETSI_IDX,cmd_info_ptr->apps_id,0,MIX_STR_TYPE);

#ifdef FEATURE_DSAT_CUST
     #error code not present
#else
     pin_data.data_len = (mmgsdi_len_type)strlen((char*) dsatutil_get_val(
               DSATETSI_EXT_ACT_CPIN_ETSI_IDX,cmd_info_ptr->apps_id,0,MIX_STR_TYPE));
#endif /* FEATURE_DSAT_CUST */

      /* if PIN locked, verify the password (if provided) */
      if( pin_locked && (0 != pin_data.data_ptr[0]) && (0xFF != pin_data.data_ptr[0]) )
      {
        mmgsdi_return_enum_type mmgsdi_status;

        /* verify the pin, this will unlock the pin lock if the password
           is correct */
        VERIFY_PIN( dsat_qcsimapp_info[cmd_info_ptr->apps_id].session_id,
                    pin_id, pin_data, handler );

        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          result = DSAT_ERROR;
        }

        /* clear the password, we are not allowed to maintain a copy
           of the pin code */
        //dsatutil_reset_password(pin, PIN_MAX_LEN+1);
      }
    }
  }

  return result;
} /* cpin_process_pin_status_ex */

