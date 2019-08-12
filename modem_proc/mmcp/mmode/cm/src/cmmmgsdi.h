#ifndef CMMMGSDI_H
#define CMMMGSDI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 C A L L   M A N A G E R   MMGSDI   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager MMGSDI Interface block, which groups
  all the functionality that is a associated with CM<->MMGSDI interfacing.




EXTERNALIZED FUNCTIONS
  cmmmgsdi_ok_to_refresh
  cmmmgsdi_refresh_files_read_proc
  cmmmgsdi_read
  cmmmgsdi_read_preferred_plmn_list
  cmmmgsdi_write_preferred_plmn_list
  cmmmgsdi_refresh_init
  cmmmgsdi_validate_received_file_list
  cmmmgsdi_update_rat_mode
  cmmmgsdi_cmals_mmgsdi_async_sim_read
  cmmmgsdi_cmals_mmgsdi_async_sim_write
  cm_mmgsdi_in_traffic_channel_w_mode
  cmmmgsdi_card_status_cb
  cmmmgsdi_client_id_reg_cb
  cm_mmgsdi_ssn_tbl_get_app_type_for_type
  cmmmgsdi_ini



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cm_init_after_task_start() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2008, 2009, 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmmgsdi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/13   jvo     Mainline FEATURE_MMODE_RPM
09/20/12   sk      Manage the ok to proceed with refresh flags effectively for
                   different sessions
09/19/12   sg      Remove MMGSDI_RAT_MODE_PREF_TDS_ONLY
06/22/12   xs      Hold subscription unavailable and ok_to_proceed to GSDI until get DS
                   session termination confirmation
03/28/12   jh      Integrated RPM feature
01/06/12   gm      Data synchronization changes for freefloating platform
07/13/11   rk      Fix lint issue
05/13/11   rn      Use Increase MMGSDI API for ACM files
04/18/11   mp      Optimized cmmmgsdi_process_session_open_rpt to use heap
03/11/11   rm      GSDI clean-up
02/14/11   rm      LTE RAT-B changes
01/19/10   am      Adding prototypes for removing gsdi_get_card_mode()
12/23/10   sv      Integrate DSDS feature
10/01/10   ak      Merged the Fusion code from Dev branch.
09/23/10   am      Adding cm_mmgsdi_get_session_info_for_gw() and
                   cm_mmgsdi_get_session_info_cdma() prototypes
08/16/10   sv      Added cmmmgsdi_session_get_app_type
07/11/10   am      Merging call control cb function registration APIs changes
05/24/10   sv      Updated callback functions to queue a rpt to CM
04/01/10   mj      Propagate refresh event session ID
01/11/10   mj      CSIM - GW uses Session based MMGSDI interfaces
11/13/09   fj      Add MMGSDI_RAT_MODE_PREF for GSM_WCDMA and LTE_ONLY.
02/18/09   fj      Enable some APIs for LTE.
08/21/09   rm      Including gsdi_exp_v.h
08/07/09   rn/rm   Centralised call control changes
08/07/09   am      Adding cmmmgsdi_get_all_pin_status()
05/26/09   rm      Changing signature of cmmmgsdi_rat_mode_init
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
11/25/08   am      Fixing compilation warnings
11/21/08   am      Fixing compilation error for Running off-target tests
11/17/08   am      Using MMGSDI API in CM.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"

#include "comdef.h"
#include "mmgsdilib.h"
#include "cm.h"
#include "cmi.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdisessionlib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_GSTK
#define CM_MMGSDI_NUM_REFRESH_FILES       6
#define CM_MMGSDI_NUM_REGISTRED_FILES    11
#endif /* #ifdef FEATURE_GSTK */

#define cyclic 1
#define transparent 2
#define CM_AUTO_REG_CT_CDMA_IMSI_LEN 10
/** ICCID LEN */
#define CM_MMGSDI_ICCID_LEN                              10

#if ( defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE))

/*
** This data structure is the elementary file file format for EFPLMNwAcT
** (User controlled PLMN selector with Access Technology).  Refer to
** TS 11.11.
*/
#define CM_MMGSDI_ACT_LENGTH        (2)

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMMMGSDI_SESSION_PREF( ss ) \
                     ((ss==CM_SS_HYBR_3 && cmph_is_sxlte())?(MMGSDI_GW_PROV_SEC_SESSION):\
  /*lint -save -e506 */(ss==CM_SS_HYBR_3\
                      ?MMGSDI_GW_PROV_TER_SESSION\
                      :(ss==CM_SS_HYBR_2?MMGSDI_GW_PROV_SEC_SESSION:MMGSDI_GW_PROV_PRI_SESSION))) /*lint -restore */
#else
#define CMMMGSDI_SESSION_PREF( ss ) \
  /*lint -save -e506 */(ss==CM_SS_HYBR_2\
                      ?MMGSDI_GW_PROV_SEC_SESSION\
                      :MMGSDI_GW_PROV_PRI_SESSION) /*lint -restore */

#endif


/**-----------------------------------------------------------------------
**      Typedefs
**------------------------------------------------------------------------
*/
typedef struct cm_mmgsdi_plmn_w_act_s
{
  sys_plmn_id_s_type plmn;
  uint8              act[CM_MMGSDI_ACT_LENGTH];
} cm_mmgsdi_plmn_w_act_s_type;

/* The enum to differentiate the byte values that is
** received from MMGSDI.
*/
typedef enum
{
  MMGSDI_RAT_MODE_PREF_NONE = -1,
  MMGSDI_RAT_MODE_PREF_GSM_WCDMA,  /* Dual Mode */
  MMGSDI_RAT_MODE_PREF_GSM_ONLY,
  MMGSDI_RAT_MODE_PREF_WCDMA_ONLY,
  MMGSDI_RAT_MODE_PREF_AUTOMATIC,
  MMGSDI_RAT_MODE_PREF_LTE_ONLY,
  MMGSDI_RAT_MODE_PREF_MAX
} cmmmgsdi_rat_mode_preference_e_type;
#endif /*  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_CM_LTE)) */

typedef enum
{
   CM_MMGSDI_SUCCESS            = 0,
   CM_MMGSDI_UNSUPPORTED        = 1,
   CM_MMGSDI_NOTFOUND           = 2
}cm_mmgsdi_error_e_type;

typedef enum {
  CMMMGSDI_STATE_NONE               = 0,
  CMMMGSDI_STATE_SUBSCRIPTION_READY = 1,
  CMMMGSDI_STATE_SESSION_CHANGED    = 2,
  CMMMGSDI_STATE_MAX                = 3
}cmmmgsdi_session_state_type;


/* below will be used to store MMGSDI request details
    ** when a request is sent to MMGSDI uptime will be set to
    **current_time + CM_MMGSDI_ABORT_TIMER_VALUE_SECS
    **if the response is not rcvd within the above time ,
    **cmmmgsdi_timer_proc will call the cb stored in response_cb_ptr
    **The uptime need to be reset right after the read confirmation,
    **and make sure we wont overwrite if an other req request is queued */
typedef struct{
  dword             uptime;
  /* timer value
      **The uptime need to be reset right after the read confirmation,
      **and make sure we wont overwrite if an other req request is queued */
  void*              cc_object_ptr;
  /* client data */
  mmgsdi_callback_type response_cb_ptr;
  /* CM callback */
} cm_mmgsdi_read_uptime_s_type;

/* below will be used to store MMGSDI request details
    ** when a request is sent to MMGSDI uptime will be set to
    **current_time + CM_MMGSDI_ABORT_TIMER_VALUE_SECS
    **if the response is not rcvd within the above time ,
    **cmmmgsdi_timer_proc will call the cb stored in response_cb_ptr
    **The uptime need to be reset right after the read confirmation,
    **and make sure we wont overwrite if an other req request is queued */
typedef struct{
  dword             uptime;
  /* timer value
      **The uptime need to be reset right after the read confirmation,
      **and make sure we wont overwrite if an other req request is queued */
  uint32              client_data;
  /* client data */
  mmgsdi_callback_type response_cb_ptr;
  /* CM callback */
} cm_mmgsdi_als_read_uptime_s_type;

/* below will be used to store MMGSDI request details
** is_sim_busy will be set to TRUE is SIM is busy and
    **session-id will updated with that session */
typedef struct{
  boolean is_sim_busy;
  /* is SIM busy currenlty */
} cm_mmgsdi_sim_busy_s_type;

typedef struct cmmmgsdi_s  {
  rex_timer_type mmgsdi_abort_timer;/* timer for MMGSDI operations*/

  cm_mmgsdi_read_uptime_s_type*  mmgsdi_read_req_uptime[CMCALL_MAX_CALL_OBJ+1];
  /* when a ACL read request/APP capabalities request is sent to MMGSDI uptime 
      **will be set to current_time + CM_MMGSDI_ABORT_TIMER_VALUE_SECS
      **if the response is not rcvd within the above time ,
      **cmmmgsdi_timer_proc will call the cb stored in
      **cm_mmgsdi_read_uptime_s_type::response_cb_ptr
      **The uptime need to be reset right after the read confirmation,
      **and make sure we wont overwrite if an other req request is queued */

  cm_mmgsdi_als_read_uptime_s_type  read_als_uptime;
  /* ALS read request */
  /* when a read ALS equest is sent to MMGSDI uptime will be set to
      **current_time + CM_MMGSDI_ABORT_TIMER_VALUE_SECS
      **if the response is not rcvd within the above time ,
      **cmmmgsdi_timer_proc will call the cb stored in
      **cm_mmgsdi_read_uptime_s_type::response_cb_ptr
      **The uptime need to be reset right after the read confirmation,
      **and make sure we wont overwrite if an other req request is queued */

  cm_mmgsdi_sim_busy_s_type  sim_busy_status;
  /* SIM busy status information */

  boolean  subsc_failed_dueTo_simBusy;
  /* MMOC SUBSC change failed due to sim busy */

}cmmmgsdi_s_type;



typedef struct cm_mmgsdi_session_id_table_entry_s
{
  mmgsdi_session_type_enum_type session_type;
    /* MMGSDI session type GW/1X and PRI/SEC */

  mmgsdi_session_id_type        session_id;
    /* Session-ID assigned by MMGSDI */

  sys_modem_as_id_e_type        asubs_id;
    /* Active Subscription ID assigned by to CM
    ** If not available, set to NONE
    */

  mmgsdi_app_info_type          app_info;
    /* Info regarding the provisioning App */

  uint8                         uicc_id[CM_MMGSDI_MAX_UICC_LEN];
    /* UICC id of the app */

  cmmmgsdi_session_state_type       current_state;

  mmgsdi_app_act_status_enum_type act_status;
  /* activation status of the APP */

} cm_mmgsdi_session_id_table_entry_s_type;

typedef struct cm_mmgsdi_card_inserted_info_s
{
  mmgsdi_slot_id_enum_type      slot;
  uint8                         iccid[CM_MMGSDI_ICCID_LEN];
} cm_mmgsdi_card_inserted_info_s_type;

typedef enum
{
  CM_MMGSDI_SUBS_TYPE_NONE,
  CM_MMGSDI_SUBS_TYPE_1X,
  CM_MMGSDI_SUBS_TYPE_GW,
  CM_MMGSDI_SUBS_TYPE_MAX
}cm_mmgsdi_subs_type_e_type;

 /* stores the cient ID provided by MMGSDI to CM */

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

 /*===========================================================================
 
 FUNCTION NAME cmmmgsdi_allocate_sim_read_req
 
 DESCRIPTION
   Function that allocates memory for storing MMGSDI sim read request.
 
 
 RETURN VALUE
  Address of allocated buffer
 
 DEPENDENCIES
   None
 ===========================================================================*/
 cm_mmgsdi_read_uptime_s_type* cmmmgsdi_allocate_sim_read_req(cm_call_id_type call_id);

/*===========================================================================

FUNCTION NAME cmmmgsdi_deallocate_sim_read_req

DESCRIPTION
  Function that deallocates memory for storing MMGSDI sim read request.


RETURN VALUE
 None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_deallocate_sim_read_req(cm_call_id_type call_id);

/*===========================================================================

FUNCTION NAME cmmmgsdi_validate_sim_read_req

DESCRIPTION
  Function that validates if sim read req is still pending for the response


RETURN VALUE
 TRUE if yes

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_validate_sim_read_req(cm_call_id_type call_id);

/*===========================================================================

FUNCTION NAME cm_wait_for_mmgsdi

DESCRIPTION
 Function that waits mmgsdi report or abort timer .
 if abort timer expires that means MMGSDI didnt respond in stipulated time

RETURN VALUE
 boolean  Flag indicating whether the specified MMGSDI operation is successful

DEPENDENCIES
 None
===========================================================================*/
boolean cm_wait_for_mmgsdi(void);

/*===========================================================================

FUNCTION cmmmgsdi_ptr

DESCRIPTION
  Return a pointer to the CM MMGSDI object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmmmgsdi_s_type  *cmmmgsdi_ptr( void );

/*===========================================================================

FUNCTION cmmmgsdi_timer_proc

DESCRIPTION
  Process timer events.

DEPENDENCIES
  Phone object must have already been initialized with
  cmmmgsdi_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmmmgsdi_timer_proc(void);

#ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
/*===========================================================================

 FUNCTION cm_mmgsdi_is_proceed_with_refresh_required

 DESCRIPTION

   Return the value of static variable cmmmgsdi_proceed_with_refresh_required.

 DEPENDENCIES
   None

 RETURN VALUE
   TRUE if cmmmgsdi_proceed_with_refresh_required is TRUE else FALSE.

 SIDE EFFECTS
   None
 ===========================================================================*/
boolean cm_mmgsdi_is_proceed_with_refresh_required(void);

/*===========================================================================

FUNCTION cm_mmgsdi_set_proceed_with_refresh_required

DESCRIPTION

  Sets the value for variable cmmmgsdi_proceed_with_refresh_required as per
  input parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cm_mmgsdi_set_proceed_with_refresh_required(boolean is_refresh_required);

#endif //#ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH


/*===========================================================================

FUNCTION cm_mmgsdi_get_client_id

DESCRIPTION

  Return a a copy of global data of cm_mmgsdi_client_id

DEPENDENCIES
  none

RETURN VALUE
  A copy of cm_mmgsdi_client_id.

SIDE EFFECTS
  none

===========================================================================*/
mmgsdi_client_id_type  cm_mmgsdi_get_client_id( void );


#ifdef FEATURE_GSTK
/*===========================================================================

FUNCTION CMMMGSDI_OK_TO_REFRESH

DESCRIPTION
  This function called when Refresh Voting is needed. This function
  sends Refresh vote to MMGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_ok_to_refresh
(
  mmgsdi_slot_id_enum_type       slot,

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type         session_id,  /* session ID for the refresh event */
  #endif

  boolean                        ok_to_refresh_to_mmgsdi
);

/*===========================================================================

FUNCTION cmmmgsdi_process_session_open_rpt

DESCRIPTION
  This function processes the MMGSDI session_open report from MMGSDI.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmmmgsdi_process_session_open_rpt(

      const mmgsdi_event_data_type          *mmgsdi_card_rpt
);


#endif /* FEATURE_GSTK */

#if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH)
/*===========================================================================

FUNCTION cmmmgsdi_proceed_with_refresh_stage_deactivate

DESCRIPTION
  This function called when Refresh Voting is needed. This function
  sends Refresh vote to MMGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_proceed_with_refresh_stage_deactivate(
         mmgsdi_session_id_type         session_id
);

/*===========================================================================

FUNCTION CMMMGSDI_OK_TO_REFRESH

DESCRIPTION
  This function called when Refresh Voting is needed. This function
  sends Refresh vote to MMGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
extern void cmmmgsdi_proceed_with_refresh(
         cm_ss_e_type ss
);

/*===========================================================================

FUNCTION CMMMGSDI_PROCEED_WITH_REFRESH_SUBS

DESCRIPTION
  This function called when Refresh Voting is needed. This function
  sends Refresh vote to MMGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
extern void cmmmgsdi_proceed_with_refresh_subs(
         sys_modem_as_id_e_type asubs_id
);

/*===========================================================================

FUNCTION cm_mmgsdi_is_waiting_to_proceed_with_refresh

DESCRIPTION

  Return if mmgsdi is waiting for cm confirmation to proceed with refresh

DEPENDENCIES
  None

RETURN VALUE
  TRUE if cmmmgsdi_waiting_to_proceed_with_refresh_arr is TRUE else FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_mmgsdi_is_waiting_to_proceed_with_refresh(mmgsdi_session_type_enum_type session);

#endif

#if ( defined CM_GW_SUPPORTED || defined(FEATURE_CM_LTE))


/*===========================================================================
FUNCTION CMMMGSDI_REFRESH_FILES_READ_PROC

DESCRIPTION
  Read the EFs as suggested by MMGSDI.

  If MMGSDI_USIM_7F66_PROP1_RAT EF is read, the RAT mode is also
  updated by this function.

RETURN VALUE
  None.

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_refresh_files_read_proc
(
  cm_ss_e_type                           ss,
    /* stack/subscription whose files to read */

  const mmgsdi_refresh_file_list_type    *refresh_file_list,

  cm_client_id_type                      cm_client_id

);

/*==============================================================================

FUNCTION NAME CMMMGSDI_READ_PREFERRED_PLMN_LIST

DESCRIPTION
  Function that reads User Preferred PLMN List from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN With Access Technologty
           List was read from the SIM successfully (TRUE) or unsuccessfully
           (FALSE).

DEPENDENCIES
  None
==============================================================================*/
boolean cmmmgsdi_read_preferred_plmn_list
(
  cm_ss_e_type                    ss,
    /* stack/subscription whose files to read */

  sys_user_pref_plmn_list_s_type  *user_pref_plmn_list_ptr,

  boolean                         is_packed
  /* if is_packed is TRUE, we would remove holes.
  */
);

/*===========================================================================

FUNCTION NAME cmmmgsdi_write_preferred_plmn_list

DESCRIPTION
  Function that writes the User Preferred PLMN list to the SIM.

RETURN VALUE

  boolean  Flag indicating whether the User Preferred PLMN with Access
           Technology List was written to the SIM successfully (TRUE) or
           unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_write_preferred_plmn_list
(
  sys_modem_as_id_e_type     asubs_id,
    /* sub on which to write */

  const sys_user_pref_plmn_list_s_type  *user_pref_plmn_list_ptr
);


/*===========================================================================

FUNCTION NAME cmmmgsdi_write

DESCRIPTION

  Function that writes in a specified Elementary File (EF) to the SIM.

RETURN VALUE

  boolean  Flag indicating whether the specified EF was written to the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_write
(
  sys_modem_as_id_e_type     asubs_id,
    /* sub on which to write */

  mmgsdi_file_enum_type        file_name,
    /* File name that should be updated */

  cm_mmgsdi_subs_type_e_type   subs_type,
    /* 1x or gw subs */

  mmgsdi_write_data_type       write_data
    /* data to write to the file */
);

/*===========================================================================

FUNCTION NAME cmmmgsdi_increase

DESCRIPTION

  Function that increments data in a specified Elementary File(EF) on the SIM

RETURN VALUE

  boolean  Flag indicating whether the specified EF was incremented on the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_increase
(
  sys_modem_as_id_e_type     asubs_id,
    /* sub on which to write */

  mmgsdi_file_enum_type        file_name,
    /* File name that should be updated */

  cm_mmgsdi_subs_type_e_type   subs_type,
    /* 1x or gw subs */

  mmgsdi_increase_data_type       increase_data
    /* increment value */
);

#ifdef FEATURE_GSTK
/*===========================================================================

FUNCTION cmmmgsdi_refresh_init

DESCRIPTION
  This function reads all the EFs, whose list is provided as parameter. Also
  after completion of reading, the read completion message is sent to MMGSDI.

DEPENDENCIES
  The initialization of the SIM must be complete, with the PIN verified.

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void  cmmmgsdi_refresh_init(
      const mmgsdi_refresh_file_list_type       *refresh_file_list,
      cm_ss_e_type                              ss
);


/*===========================================================================

FUNCTION CMMMGSDI_VALIDATE_RECEIVED_FILE_LIST

DESCRIPTION
  Checks whether the file list only contains in call files.

RETURN VALUE
  TRUE   - Given list only contains in call files
  FALSE  - Given list is not confined to in call files only.

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_validate_received_file_list
(

  const mmgsdi_file_enum_type   *file_list_ptr,
    /* List of files */

  uint32                  num_files
    /* Length of the file list */

);


/*===========================================================================
FUNCTION CMMMGSDI_UPDATE_RAT_MODE

DESCRIPTION
  This function updates the current mode of the phone
  with the new mode that's been reeived from MMGSDI.

  If there's any change, the current mode will be replaced with
  the one given by MMGSDI.

RETURN VALUE

  TRUE  - Command successfully queued.
  FALSE - Command was not queued.

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_update_rat_mode(
  cm_client_id_type                   client_id,
  cmmmgsdi_rat_mode_preference_e_type rat_mode
);


#endif /* FEATURE_GSTK */

#endif /* #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_CM_LTE)) */

/*=========================================================================

FUNCTION NAME CMMMGSDI_READ

DESCRIPTION
  Function that reads a specified Elementary File (EF) from the SIM.
  After posting the read request CM waits for CM_MMGSDI_ABORT_TIMER_VALUE_SECS secs ,
  and bear out from waiting for MMGSDI response , since it is not recieved in stiuplated time .


RETURN VALUE
  boolean  Flag indicating whether the specified EF was read from the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
==========================================================================*/
boolean cmmmgsdi_read
(
  mmgsdi_session_type_enum_type     session_type,
    /* session whose files to read */

  mmgsdi_file_enum_type             file_name,
   /* Name of the file to read */

  mmgsdi_rec_num_type               record_number,
   /* Record number to begin with, in the file */

  mmgsdi_len_type                   request_length,
   /* Total bytes to be read */

  uint8*                            data_buffer_p,
   /* Pointer to hold data reurned by MMGSDI */

  size_t                            size_data_buf,
   /* Size of the data return buffer */

  int                               file_structure
        /* file structure - cyclic/transperant */

);

/*========================================================================

FUNCTION NAME cmmmgsdi_read_acl_file

DESCRIPTION
  Function that reads a specified Elementary File (EF) from the SIM.

RETURN VALUE
  boolean  Flag indicating whether the specified EF was read from the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
========================================================================*/
boolean cmmmgsdi_read_acl_file
(
  void*                             client_data
);

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
/*===========================================================================
FUNCTION CMMMGSDI_CMALS_MMGSDI_ASYNC_SIM_READ

DESCRIPTION

  Queue ALS EF read command to MMGSDI.
  This function will provide an immediate response to the caller.
  Read results will be returned in the callback provided.

RETURN VALUE

  TRUE  - Command successfully queued.
  FALSE - Command was not queued.

DEPENDENCIES

  This routine is not reentrant.
  Shouldn't be a problem, as it doesn't exit till we're done, and
  it's only called from the CM task.

===========================================================================*/
boolean cmmmgsdi_cmals_mmgsdi_async_sim_read(
    mmgsdi_file_enum_type             file_name,
    mmgsdi_rec_num_type               record_number,
    mmgsdi_len_type                   request_length,
    int                               file_structure,
    mmgsdi_callback_type              cmals_mmgsdi_async_cb,
    mmgsdi_client_data_type           client_ref
);

/*===========================================================================
FUNCTION CMMMGSDI_CMALS_MMGSDI_ASYNC_SIM_WRITE

  Queue ALS EF write command to MMGSDI.
  This function will provide an immediate response to the caller.
  Write results will be returned in the callback provided.

  INPUT:
  mmgsdi_file_enum_type file_name,          EF To Access
  mmgsdi_rec_num_type   record_number,      Record Number
  mmgsdi_len_type       request_length,     Amount of data (in bytes)
  uint8*                data_buffer_p,      Buffer with data to be written
  int                   file_structure,     transparent/cyclic
  (void *)              cm_mmgsdi_async_cb  Callback to call when write done

  The callback provided must have a prototype of
  void (*mmgsdi_callback_type) (mmgsdi_return_enum_type status,
                                mmgsdi_cnf_enum_type    cnf,
                                const mmgsdi_cnf_type  *cnf_ptr);

RETURN VALUE

  TRUE  - Command successfully queued.
  FALSE - Command was not queued.

DEPENDENCIES

  This routine is not reentrant.
  Shouldn't be a problem, as it doesn't exit till we're done, and
  it's only called from the CM task.

===========================================================================*/
boolean cmmmgsdi_cmals_mmgsdi_async_sim_write(
    mmgsdi_file_enum_type        file_name,
    mmgsdi_rec_num_type          record_number,
    mmgsdi_len_type              request_length,
    uint8*                       data_buffer_p,
    int                          file_structure,
    void*                        cmals_mmgsdi_async_cb
);
#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

/*==========================================================================

FUNCTION CMMMGSDI_GET_ALL_PIN_STATUS

DESCRIPTION
  Put CM command in MMGSI to know the present PIN status

RETURN VALUE
  NONE

DEPENDENCIES
  None
==========================================================================*/

void cmmmgsdi_get_all_pin_status( mmgsdi_session_id_type );

/*===========================================================================
FUNCTION CM_MMGSDI_IN_TRAFFIC_CHANNEL_W_MODE

DESCRIPTION
  Inform GSDI that call is connected/disconnected, so GSDI can start/stop
  in call polling.

  in_traffic is a boolean which indicates whether we are in traffic or not.
  mode is a indicates whether the call is GSM / UMTS or 1X.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  None
===========================================================================*/
void  cm_mmgsdi_in_traffic_channel_w_mode(

      boolean               in_traffic,

      sys_sys_mode_e_type   mode,

      cm_ss_e_type          ss
);

/*===========================================================================
FUNCTION CMMMGSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function for MMGSDI generated event.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_card_status_cb(
  const mmgsdi_event_data_type *event
);

/*===========================================================================
FUNCTION CMMMGSDI_INIT

DESCRIPTION
  Initilize MGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_init(void);

/*==============================================================================

FUNCTION NAME CMGSDI_IS_REFRESH_LIST_USED_BY_CM

DESCRIPTION
  Returns the list of SIM files

RETURN VALUE

  TRUE   - Given list contains files that CM uses during a call
  FALSE  - Given list does NOT CONTAIN files that CM uses during a call.

DEPENDENCIES
  None
==============================================================================*/
boolean cmmmgsdi_is_refresh_list_used_by_cm
(

  const mmgsdi_file_enum_type  *file_list_p,
    /* List of files */

  uint8                       file_list_len
    /* Length of the file list */

);


#ifdef CM_GW_SUPPORTED
/*===========================================================================
FUNCTION CMMMGSDI_INIT

DESCRIPTION
  Initilize CM MGSDI interface.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_rat_mode_init(

    cm_mode_pref_e_type  *mode_pref
    /* Pointer to the mode_pref to be read from EF_RAT */
);
#endif

/*===========================================================================
FUNCTION CM_MMGSDI_SESSION_CLIENT_ID_REG_CB

DESCRIPTION
  Callback function for session based MMGSDI interface.  Called by MMGSDI
  to acknowledge CM client registration and event registration.  Client ID
  provided in this callback.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

SIDE EFFECTS
  Error Fatal if registration unsuccessful
  Client ID is stored
===========================================================================*/
void cm_mmgsdi_session_client_id_reg_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);


/*==========================================================================
FUNCTION CMMMGSDI_GET_ALL_PIN_STATUS_CB

DESCRIPTION
 Provide as callback during mmgsdi_get_all_pin_status() call back Called
 by MMGSDI to tell CM about the present PIN status.

RETURN VALUE
 NONE

DEPENDENCIES
 None
==========================================================================*/

void cmmmgsdi_get_all_pin_status_cb(

    mmgsdi_return_enum_type         status,
      /* Status of cmmmgsdi_get_all_pin_status() call */

    mmgsdi_cnf_enum_type            get_all_pin_status_cnf,
      /* Indicates which API call results in cb function call */

    const mmgsdi_cnf_type           *cnf_ptr
      /* Results returned in cb function call */
);

/*==========================================================================
FUNCTION cmmmgsdi_session_store_esn_cb

DESCRIPTION
 Provide as callback during mmgsdi_session_store_esn() call back Called
 by MMGSDI to tell CM about the store ESN process status.

RETURN VALUE
 NONE

DEPENDENCIES
 None
==========================================================================*/

void cmmmgsdi_session_store_esn_cb(

    mmgsdi_return_enum_type         status,
      /* Status of cmmmgsdi_get_all_pin_status() call */

    mmgsdi_cnf_enum_type            store_esn_cnf,
      /* Indicates which API call results in cb function call */

    const mmgsdi_cnf_type           *cnf_ptr
      /* Results returned in cb function call */
);

#ifdef FEATURE_MMGSDI_SESSION_LIB

typedef struct cm_mmgsdi_jcdma_card_info_s
{
  boolean is_valid;
  mmgsdi_jcdma_card_info_enum_type jcdma_card_info;

}cm_mmgsdi_jcdma_card_info_s_type;

typedef enum cm_mmgsdi_acl_avail_status_e
{
  CM_MMGSDI_ACL_AVAIL_INVALID,
    /* Initialized values, USIM is not contacted yet */

  CM_MMGSDI_ACL_AVAIL_ENABLED,
    /* USIM is contacted and ACL is available */

  CM_MMGSDI_ACL_AVAIL_DISABLED,
    /* USIM is contacted and ACL is not supported */

  CM_MMGSDI_ACL_AVAIL_MAX

}cm_mmgsdi_acl_avail_status_e_type;

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_update_asubs_for_id

DESCRIPTION

  This function sets active subscription id for session specified by
  session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/

cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_update_asubs_for_id
(
  mmgsdi_session_id_type session_id,

  sys_modem_as_id_e_type asubs_id
);

/*====================================================================

FUNCTION cmmmgsdi_ssn_tbl_update_app_type_for_type

DESCRIPTION

  This function sets app type for session_type in the session table.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
cm_mmgsdi_error_e_type cmmmgsdi_ssn_tbl_update_app_type_for_type
(
  mmgsdi_session_type_enum_type    session_type,
  mmgsdi_app_enum_type             sess_app_type
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_id_for_type

DESCRIPTION

  This function returns a pointer to the session ID for the specified
  session type.  At most one session ID is associated with each session
  type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_id_for_type
(
   mmgsdi_session_type_enum_type session_type,
   mmgsdi_session_id_type        *session_id_ptr
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_ssn_info_for_id

DESCRIPTION

  This function returns a session info for the specified session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/

cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_ssn_info_for_id
(
  mmgsdi_session_id_type session_id,

  cm_mmgsdi_session_info_s_type *ssn_info_ptr
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_subs_info_for_asubs_id_1x

DESCRIPTION

  This function returns subscription info associated with subs id for CSIM/RUIM app type
  in session table.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_subs_info_for_asubs_id_1x
(
  sys_modem_as_id_e_type                     asubs_id,
  cm_subs_info_s_type                        *subs_info_ptr
);


/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_subs_info_for_asubs_id_gw

DESCRIPTION

  This function returns subscription info associated with subs id for USIM/SIM app type
  in session table.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_subs_info_for_asubs_id_gw
(
  sys_modem_as_id_e_type                     asubs_id,
  cm_subs_info_s_type                        *subs_info_ptr
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_1x

DESCRIPTION

  This function returns a pointer to session type for specified
  asubs id for CSIM/RUIM app type.  At most one app_info structure is associated with each
  session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_1x
(
  sys_modem_as_id_e_type         asubs_id,
  mmgsdi_session_type_enum_type  *session_type_ptr
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw

DESCRIPTION

  This function returns a pointer to session type for specified
  asubs id for USIM/SIM app type.  At most one app_info structure is associated with each
  session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw
(
  sys_modem_as_id_e_type         asubs_id,
  mmgsdi_session_type_enum_type  *session_type_ptr
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_app_type_for_type

DESCRIPTION

  This function returns app type for session_type in the session table.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
mmgsdi_app_enum_type cm_mmgsdi_ssn_tbl_get_app_type_for_type
(
  mmgsdi_session_type_enum_type    session_type
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_slot_for_type

DESCRIPTION

  This function returns the app info slot for the specified
  session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE

SIDE EFFECTS
  none

=====================================================================*/
mmgsdi_slot_id_enum_type cm_mmgsdi_ssn_tbl_get_slot_for_type
(
   mmgsdi_session_type_enum_type session_type
);

/*===========================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_ssn_info_for_slot

DESCRIPTION

  This function returns a pointer to the session info for the specified
  slot.  First pp_info structure associated with that slot will be
  returned.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

===========================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_ssn_info_for_slot
(
  mmgsdi_slot_id_enum_type      slot_id,
  cm_mmgsdi_session_info_s_type *sess_info_ptr
);

/*===========================================================================

FUNCTION cm_mmgsdi_get_session_info_for_gw

DESCRIPTION

  This function returns session info pointer for the specified
  slot, if the app type is SIM or USIM.

  Session ID table is searched for the entry for which slot is
  same as the input papramater as well as app type is SIM or USIM.
  Once this entry found corresponding session info is returned.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

===========================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_ssn_info_for_gw_slot
(
  mmgsdi_slot_id_enum_type                   slot_id,

  cm_mmgsdi_session_info_s_type              *sess_info_ptr

);

/*===========================================================================

FUNCTION cm_mmgsdi_get_session_info_for_cdma

DESCRIPTION

  This function returns a pointer to session info for for the specified
  slot, if the app type is RUIM or CSIM.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

  Session ID table is searched for the entry for which slot is
  same as the input papramater as well as app type is RUIM or CSIM.
  Once this entry found corresponding session info is returned.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

===========================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_ssn_info_for_cdma_slot
(
  mmgsdi_slot_id_enum_type                   slot_id,
  cm_mmgsdi_session_info_s_type              *sess_info_ptr
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_subs_id_for_id

DESCRIPTION

  Searches through the session table for given session id, if entry is found
  then returns active subscription id associated with the entry.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
sys_modem_as_id_e_type cm_mmgsdi_ssn_tbl_get_subs_id_for_id
(
  mmgsdi_session_id_type session_id
);

/*====================================================================

FUNCTION cmmmgsdi_open_session

DESCRIPTION
  Internal function which calls mmgsdi_session_open for the specified
  session type, and generates an error if unsuccessful.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  Call after client registration with MMGSDI is complete, and CM
  has a valid client ID.

RETURN VALUE
  none

SIDE EFFECTS
  Request to open a session enqueued with MMGSDI.
  ERROR if unsuccessful.

=====================================================================*/
void cmmmgsdi_open_session(
     mmgsdi_session_type_enum_type cm_mmgsdi_session_type
);


/*==========================================================================

FUNCTION CMMMGSDI_IS_JCDMA_CARD_WHITE_EQUIV

DESCRIPTION
  To know if JCDMA card is White or equivalent to White

RETURN VALUE
  TRUE if White or Equivalent JCDMA card
  FALSE otherwise

DEPENDENCIES
  None
==========================================================================*/

extern boolean cmmmgsdi_is_jcdma_card_white_equiv(void);

#endif /* FEATURE_MMGSDI_SESSION_LIB */


/*====================================================================

FUNCTION cm_mmgsdi_session_evt_cb

DESCRIPTION

  Callback function for session based events from MMGSDI interface.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================*/
void cm_mmgsdi_session_evt_cb(
  const mmgsdi_event_data_type *event
);

/*====================================================================

FUNCTION cm_mmgsdi_session_open_cb

DESCRIPTION

  Callback function for session based MMGSDI interface.  Called by MMGSDI
  in response to call to mmgsdi_open_session() function.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================*/
void cm_mmgsdi_session_open_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================

FUNCTION CMMMGSDI_GENERIC_CB

DESCRIPTION
  Called by MMGSDI when it has completed servicing CM's REEQUEST. This is a
  generic handler for callbacks used for MMGSDI APIs

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(715,cnf)*/
extern void cmmmgsdi_generic_cb(
    mmgsdi_return_enum_type       status,
    mmgsdi_cnf_enum_type          cnf,
    const mmgsdi_cnf_type         *cnf_ptr
);


/*===========================================================================

FUNCTION cmmmgsdi_get_card_info

DESCRIPTION
  This function return number of applications supported by card

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmmmgsdi_get_card_info(mmgsdi_available_apps_type *card_info);

/*===========================================================================

FUNCTION cmmmgsdi_update_session_state

DESCRIPTION
  This function updates state of session

DEPENDENCIES
  mmsgdi table must have been initialized with proper value.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmmmgsdi_update_session_state(
    mmgsdi_session_id_type   session_id,
    cmmmgsdi_session_state_type  state
);

/*===========================================================================

FUNCTION cmmmgsdi_count_given_state

DESCRIPTION
  This function counts number of given state on all sessions

DEPENDENCIES
  mmsgdi table must have been initialized with proper value.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern uint8 cmmmgsdi_count_given_state(cmmmgsdi_session_state_type  state);

cm_mmgsdi_session_id_table_entry_s_type*
                                           cm_mmgsdi_ssn_tbl_get_entry_by_id
(
  mmgsdi_session_id_type session_id
);

/*====================================================================

FUNCTION cmmmgsdi_is_switch_allowed_per_csim_self_activation

DESCRIPTION

  This Function finds the CSIM app for given subs id and tells if that CSIM APP is activated or not..

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  TRUE : Switch is allowed, activation status is any value other than STATUS_NOT_ACTIVATED
  FALSE : CSIM APP is not activated.

SIDE EFFECTS
  none

=====================================================================*/
extern boolean cmmmgsdi_is_switch_allowed_per_csim_self_activation
(
  sys_modem_as_id_e_type asubs_id
);

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_entry_by_type

DESCRIPTION

  Get session table entry matching given session_type.
  Assumption: At most one entry associated for given criteria, if not first
  entry is returned.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  Invalid: NULL, no match found
  Valid : non-NULL, first match for the criterion

SIDE EFFECTS
  none

=====================================================================*/
extern cm_mmgsdi_session_id_table_entry_s_type*
                                       cm_mmgsdi_ssn_tbl_get_entry_by_type
(
  mmgsdi_session_type_enum_type session_type
);

#endif   /* CMMMGSDI_H */

