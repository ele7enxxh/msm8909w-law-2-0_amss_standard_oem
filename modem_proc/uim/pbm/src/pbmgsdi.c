/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmgsdi.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/16    nr     Send cache update event to clear phonebook global in QMI-PBM
07/01/16    nr     NAL China emergency requirement
06/09/16    ar     Send FDN event source along with FDN event
05/25/16    sp     F3 improvements
05/24/16    sp     Logging improvements
04/18/16    sp     ECC only support in PBM
05/11/16    sp     Logging improvements
05/04/16    lm     Update ECC cache and send ready after NV Refresh
11/07/15    nr     Refresh FCN should not be processed for a session if it is
                   already present in pbm initialization queue
10/26/15    nr     Disable MMGSDI PB cache for PBM module
04/03/15   stv     Migrate some efs access to uim_common_efs API
08/05/15    nr     Operator specifc NV ECC numbers configuration
07/20/15    nr     Enter & leave critical section inside pbm_ecc_set_hardcoded_eccs()
07/02/15    nr     Validate BCD number and EXT data length
06/09/15   stv     Fix to address multiple initialization in parallel on card session
03/10/15    nr     Clearing slot id in session de-activation handling
11/19/14   stv     Fix to build ECC hardcode cache between card power down and power ups
11/14/14   stv     Need not send refresh start and refresh end indication to client during session init
10/13/14    NR     Added NULL check while accessing EXT cache and fixed the memory leak.
10/09/14    NR     Fix to eliminate redundant ECC indications for session
                   change deactivation followed by CARD_ERROR event
09/22/14    NR     GRP-ID should be validated before reading and writing to SIM
08/25/14   stv     Feature to maintain sim absent hardcoded ecc list till pin is verified
09/10/14    NR     Fix to query for WMS ready in case of REFRESH while provisioning 
                   session is ongoing
07/29/14    am     Correctly direct/redirect atomic_ops APIs across MOBs 
07/25/14    am     Added stubs for RFM APIs to fix linker errors
07/07/14    NR     Use v2.0 diag macros
05/30/14    NR     Resetting the globals to avoid sending SIM_INIT_DONE more 
                   than once per slot in REFRESH Scenarios.
05/16/14    NR     Enabling or Disabling ADN based on NV configurations
05/01/14    NR     FR 19033: Reduce Code Duplication pbm
03/06/14    stv    CM SS event handling changes for SGLTE 
02/16/14    NR     PBM/QMI_PBM KW Critical warnings
02/08/14    NR     1X session INIT stopped in ICC mode
1/23/14    stv     FDN number match successful for any dialed number if FDN list has a record with BCD length 
                   as non-0xFF and actual BCD number is all set to 0xFF 
01/21/14   NR      PBM/QMI_PBM valid_pb_mask is not resetting after REFRESH EVENT
01/09/14   NR      potential memory leak issues in PBM funcions
01/03/14   NR      PBM doesn't need to wait for WMS ready in case of 1x_secondary 
                   and 1x_teritary sessions to proceed with the intilzation.
12/19/13   NR      PBM: LLVM compiler warnings
10/28/13   stv     Fix for PBM init halted for invalid group /index record length 
10/10/13   stv     Fix for NULL check klockwork errors
10/04/13   stv     F3 message reduction 
10/03/13  nalavala memcpy banned api replacement with safer version memscpy
09/20/13  nalavala The Txid should be cleard for slot on which we got card error event
09/16/10  krishnac Refresh by path changes
04/30/10  krishnac Read from root adn when app_type is 2G(241978)
04/30/10  krishnac Do not close the sessions during card errors
                     and reuse them for future valid sessions
04/30/10  krishnac initialize the caches with proper pb_id
03/18/10   krishnac Fixed crash CR 231076 by populating the proper slot
02/02/10   krishnac CR 225159 Call to number in EF-ECC is initiated as Normal Voice call
11/05/09   krishnac CR 211973  MO live Call fails if FDN enabled RUIM is used
                     (Fixed issue populating file access method)
10/14/09  krishnac Support for feature DSDS
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
04/01/09   kcp     Fixed CR 169091 - Terminal does not read PB files during power up.
16/03/09   kcp     resolved Lint errors
02/02/09   kcp     165174 - After deleting a contact,
                   2nd byte of EF-ANR is 00 instead of FF
09/04/08   sg      Fix the SEEK/SEARCH pattern for GRP files in pbm_get_seek_pattern()
09/03/08   sg      Add proper featurization and multiple phonebook support in
                   pbm_get_seek_pattern()
09/03/08   sg      Return a SEARCH/SEEK pattern of 0x00 for UID and PBC
08/28/08   sg      Featurize MMGSDI seek/search calls based on FEATURE_MMGSDI_ENHANCED_SEARCH
                   and read records sequentially if the feature is not defined
07/07/08   sg      Use mmgsdi_seek() to skip over unused records to improve
                   SIM initialization time
06/13/08   cvs     New GCF field trial docs move ECC number changes to SIM inserted
04/07/08   clm     Handle MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN stage while
                   in MMGSDI_REFRESH_NAA_INIT_FCN mode for MMGSDI refresh events.
03/13/08   clm     Add notification of PBM_EVENT_PB_REFRESH_START/DONE for
                   both local and global USIM files.
03/13/08   clm     Add restart of SIM read init for enable/disable FDN
                   Free pbm_buffer from read cb func if restart is needed.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/31/07   cvs     Fix issue with fcn during init
05/18/07   cvs     Make pbm_handle_sim_init_completed() public
03/12/07   cvs     Featurize 4 PB set support
02/23/07   cvs     Handle PIN enable/disable only as needed.
02/22/07   cvs     Keep track of filetype for read + write ops
02/09/07   cvs     Support for 4 phonebook sets
12/16/06   cvs     Only respond to PIN events for the provisioning app
12/15/06   cvs     Register for refresh on all files, in one call to fix refresh
11/01/06   cvs     multi pb set fixes
10/24/06   cvs     Register for refresh on all files, to fix FDN refresh issue
10/10/06   cvs     Move messages to PBM's diag msg ID
08/17/06   cvs     Fix race condition with GSDI refresh before done initializing.
07/25/06   slw     Added error reporting for pbm_create_sim_record
07/24/06   cvs     Fixes for unequal size multi pb set support
06/14/06   aps     support for ANRC and ANRC1 type records
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
02/23/06   cvs     rework memory allocation macros
02/09/06   cvs     When can't encode all characters on SIM, return errors
02/07/06   cvs     better log messages
01/27/06   cvs     don't add UIM_NO_SUCH_ITEMs to refresh list,
                   make refresh list decisions based on USIM local PB
01/09/06   cvs     Lint fixes
12/13/05   cvs     Implement partial PB refresh
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/16/05   cvs     Lint Fixes
09/30/06   cvs     Fix error message
09/14/05   cvs     Properly handle empty email entries.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/12/05   cvs     When extension records go unused, write out blank record
07/19/05   AT      Fix issue where PIN becomes blocked and unblocked
                   after SIM_INIT_COMPLETED.  A bit of code cleanup.
07/11/05   cvs     Better error messages
06/24/05   cvs     Fix dual IMSI FDN support
06/14/05   cvs     Fix GCF failures related to emergency numbers
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications.
02/17/05   PA      Made alphabet translation table constant.
02/14/05   PA      Do not stop initialization if extension record type is
                   not recognized.
01/18/05   PA      Convert 7-bit SMS default alphabet to PBM 8-bit ascii and
                   fix compilation errors for non-MMGSDI builds
01/14/05   PA      Make PBM GSTK unaware
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds, code review comments
09/29/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm_common_types.h"
#include "pbm.h"
#include "time_jul.h"
#include "string.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmtask_i.h"
#include "pbm_conversion.h"
#include "pbmuim.h"
#include "atomic_ops.h"
#include "pbmefs.h"
#include "stringl/stringl.h"
#include "wms.h"
#include "pbmlib.h"
#include "cmutil.h"
#include "uim_msg.h"
#include "uim_pbm.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
/* Global array to store the card status 
 * pbm_card_status set --- In card inserted event 
 * pbm_card_status Reset -- In card error event
 */
boolean pbm_card_status[PBM_MAX_SLOTS];
mmgsdi_client_id_type pbm_mmgsdi_client_id = 0;

extern PBM_NV_ECC_LIST_PER_SUB  *pbm_slot_specific_ecc_list_ptr[PBM_MAX_SLOTS][PBM_NV_EMERGENCY_NUMBERS_PER_SUB];


uint32 pbm_hidden_key_verified = 0;
atomic_word_t pbm_pin1_verified; /* to know if PIN1 is verified for particular session */
atomic_word_t pbm_pin2_verified; /* to know if PIN2 is verified for particular session */

extern uint32 ecc_has_entries;

static void pbm_mmgsdi_card_status_cb(const mmgsdi_event_data_type *mmgsdi_event);

static void pbm_mmgsdi_refresh_cb(mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    cnf,
                                  const mmgsdi_cnf_type  *cnf_ptr);

static void pbm_mmgsdi_ok_to_refresh_cb(mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf,
                                        const mmgsdi_cnf_type  *cnf_ptr);
static boolean pbm_mmgsdi_process_mdn_data(pbm_session_enum_type session_type,
                                           pbm_file_type file_id,
                                           int index,
                                           byte *local_pbm_data_buffer,
                                           pbm_num_text_record_s_type *return_data);


static boolean pbm_mmgsdi_process_sim_data(pbm_session_enum_type session_id,
                                           pbm_file_type file_id,
                                           int index,
                                           byte *local_pbm_data_buffer,
                                           pbm_num_text_record_s_type *return_data,
                                           int *extension_index);

static void pbm_clear_refresh_caches(pbm_session_enum_type session_type);

static boolean pbm_update_refresh_globals(
                                         pbm_refresh_file_list_type  *refresh_file_list_p,
                                         pbm_session_enum_type       session);

pbm_async_gsdi_buffer pbm_buffers[PBM_ASYNC_BUFFERS];

/* For storing SEEK data */
pbm_seek_data_type pbm_seek_data[MAX_UIM_APPS];

/*===========================================================================
FUNCTION pbm_wms_ready_event_cb

DESCRIPTION
  This function is registered with WMS. A PBM command is added to the
  gsdi_resp queue when we receive this event.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_wms_ready_event_cb(wms_cfg_efs_read_evt_s_type *wms_cfg_efs_read_evt)
{
  pbm_wms_resp_s_type *wms_resp_ptr = NULL;

  wms_resp_ptr = (pbm_wms_resp_s_type *)pbm_cmd_alloc(sizeof(pbm_wms_resp_s_type));
  if (!wms_resp_ptr)
  {
    UIM_MSG_ERR_0("Unable to allocate PBM command");
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return;
  }

  wms_resp_ptr->cmd_type              = PBM_WMS_STATUS_CB;
  wms_resp_ptr->info_ptr.session_type = wms_cfg_efs_read_evt->session_type;
  wms_resp_ptr->info_ptr.t_id         = wms_cfg_efs_read_evt->t_id;
  UIM_MSG_HIGH_2("PBM pbm_wms_ready_event_cb WMS TxId %d session_type %d",
	           wms_cfg_efs_read_evt->t_id,wms_cfg_efs_read_evt->session_type);

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(wms_resp_ptr));
}

/*===========================================================================
FUNCTION pbm_alloc_txid

DESCRIPTION
  This function is will return the first availble free TxId to include in the
  pbm ready event mesage to WMS .

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint8 pbm_alloc_txid(
pbm_session_enum_type pbm_session
)
{
  pbm_session_info[pbm_session].last_txid += 1;
  if(pbm_session_info[pbm_session].last_txid == 0)
  {
    pbm_session_info[pbm_session].last_txid += 1;
  }
  
  return pbm_session_info[pbm_session].last_txid;
}/* pbm_alloc_txid */

/*===========================================================================
FUNCTION pbm_free_txid

DESCRIPTION
  This function is will free the TxId once PBM receieved the CARD error event
  from WMS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_free_txid(
void
)
{
   uint8 session_loop_itrator;

   /* Intilizing the global last updated TxID to zero */
   for(session_loop_itrator = 0;session_loop_itrator < PBM_SESSION_MAX;session_loop_itrator++)
   {
     pbm_session_info[session_loop_itrator].last_txid = 0;
   }
}/* pbm_free_txid */

/*===========================================================================
FUNCTION pbm_session_to_mmgsdi_session_mapping

DESCRIPTION
  This function is will map the PBM sessions to MMGSDi session to send to WMS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
mmgsdi_session_type_enum_type pbm_session_to_mmgsdi_session_mapping(
pbm_session_enum_type pbm_session
)
{
  mmgsdi_session_type_enum_type mmgsdi_session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;

  switch (pbm_session)
  {
    case PBM_SESSION_GPB_1:
      mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_1;
      break;
    case PBM_SESSION_GPB_2:
      mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_2;
      break;
    case PBM_SESSION_LPB_GW:
      mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
      break;
    case PBM_SESSION_LPB_1X:
      mmgsdi_session_type = MMGSDI_1X_PROV_PRI_SESSION;
      break;
#ifdef FEATURE_DUAL_SIM
    case PBM_SESSION_LPB_GW_SECONDARY:
      mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
      break;
    case PBM_SESSION_LPB_1X_SECONDARY:
      mmgsdi_session_type = MMGSDI_1X_PROV_SEC_SESSION;
      break;
#endif
#ifdef FEATURE_TRIPLE_SIM
    case PBM_SESSION_GPB_3:
      mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_3;
      break;
    case PBM_SESSION_LPB_GW_TERTIARY:
      mmgsdi_session_type = MMGSDI_GW_PROV_TER_SESSION;
      break;
    case PBM_SESSION_LPB_1X_TERTIARY:
      mmgsdi_session_type = MMGSDI_1X_PROV_TER_SESSION;
      break;
#endif
    default:
      break;
  }
	
  return mmgsdi_session_type;
}/* pbm_session_to_mmgsdi_session_mapping */

/*===========================================================================
FUNCTION pbm_send_ready_event_to_wms

DESCRIPTION
  This function is will fill the session info,TxID and the call back for PBM 
  ready message and call the WMS exposed API.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_send_ready_event_to_wms(
pbm_session_enum_type pbm_session_type
)
{
    boolean                      ret_val = TRUE;
    wms_cfg_efs_read_evt_s_type  wms_cfg_efs_read_evt;		
    wms_status_e_type            wms_status;

    if( pbm_session_type_to_pb_category(pbm_session_type) != PBM_LPB )
    {
      return ret_val;
    }

    /* Check if WMS init is set already or not,IT was already set in case of REFRESH FCN */
    if(pbm_wms_init_finished & PBM_SESSIONID_TO_SESSION_MASK(pbm_session_type))
    {
      return ret_val;
    }

    /* Don't query WMS ready in case of 1x_secondary and 1x_teritary and proceed
     * with initlization as WMS don't open sessions on 1x_secondary and 1x_teritary
     */
    if(pbm_session_type == PBM_SESSION_LPB_1X_SECONDARY || 
   	   pbm_session_type == PBM_SESSION_LPB_1X_TERTIARY)
    {
      /* Setting pbm_wms_init_finished as if like wms ready receieved from WMS 
       * in case of 1x_secondary and 1x_teritary 
       */
      PBM_SESSION_SET(pbm_wms_init_finished,pbm_session_type);
      return ret_val;
    }

    wms_cfg_efs_read_evt.session_type = pbm_session_to_mmgsdi_session_mapping( pbm_session_type );
    wms_cfg_efs_read_evt.t_id         = pbm_alloc_txid(pbm_session_type);
    wms_cfg_efs_read_evt.cfg_event_cb = pbm_wms_ready_event_cb;
    wms_cfg_efs_read_evt.user_data    = NULL; /* User_data should not be used */

    UIM_MSG_HIGH_2("PBM pbm_send_ready_event_to_wms TxId %d MMGSDI session %d",
                    wms_cfg_efs_read_evt.t_id,wms_cfg_efs_read_evt.session_type);

    if(wms_cfg_efs_read_evt.t_id == 0 || wms_cfg_efs_read_evt.session_type == MMGSDI_MAX_SESSION_TYPE_ENUM)
    {
        ret_val = FALSE;
    }
    else
    {
        wms_status = wms_cfg_send_sim_efs_read_event_to_pbm ( wms_cfg_efs_read_evt );
        if(wms_status != WMS_OK_S)
        {
            UIM_MSG_ERR_1("PBM wms_cfg_send_sim_efs_read_event_to_pbm fails wms_status %d",wms_status);
            ret_val = FALSE;
        }
    }

    if(ret_val == TRUE)
    {
      UIM_MSG_HIGH_1("PBM pbm_send_ready_event_to_wms () Success %d ",pbm_session_type);
    }

    return ret_val;
}

/*===========================================================================
FUNCTION PBM_ALLOC_RECORD

DESCRIPTION
  During async calls, we need to store the memory location in which the
  final answer will be stored.  This call will allocate a buffer and
  return the index.  This index is passed to and from GSDI to know where
  to place the buffers.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_alloc_record(void)
{
   int i;
   int retval = -1;
   rex_enter_crit_sect(&pbm_crit_sect);
   for (i = 0; i < PBM_ASYNC_BUFFERS; i++)
   {
      if (!pbm_buffers[i].in_use)
      {
         memset(&pbm_buffers[i], 0, sizeof(pbm_buffers[i]));
         pbm_buffers[i].in_use = TRUE;
         retval = i;
         break;
      }
   }
   rex_leave_crit_sect(&pbm_crit_sect);

   return retval;
}


/*===========================================================================
FUNCTION PBM_FREE_RECORD

DESCRIPTION
  Mark the pbm_buffer as available for later use.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_free_record(int record)
{
   pbm_buffers[record].in_use = FALSE;
}



/*===========================================================================
FUNCTION PBM_MMGSDI_REG_STATUS_CB

DESCRIPTION
  Callback function from MMGSDI to log errors

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_mmgsdi_reg_status_cb (mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    data_type,
                                  const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;

   PBM_CHECK_PTR_RET(data_ptr, VOID);


   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_REG_STATUS_CB;

   mmgsdi_resp_ptr->data_type = data_type;
   mmgsdi_resp_ptr->status = status;

   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

   //unblock PBM task
   (void) rex_set_sigs(PBM_TCB, PBM_MMGSDI_WAIT_SIG);

   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}


/**===========================================================================
FUNCTION PBM_MMGSDI_INIT

DESCRIPTION
  Initialize MMGSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================**/
pbm_return_type pbm_mmgsdi_init()
{
   mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;


   UIM_MSG_HIGH_0("Begin PBM MMGSDI init");

   /* Initialize pbm_session_info structure */
   memset(&pbm_session_info[0], 0, sizeof(pbm_session_info));
   memset(&pbm_fileinfo_requests[0], 0, sizeof(pbm_fileinfo_requests));
   memset(pbm_buffers, 0, sizeof(pbm_buffers));

   (void) rex_clr_sigs(PBM_TCB, PBM_MMGSDI_WAIT_SIG);

   mmgsdi_status = mmgsdi_client_id_and_evt_reg_ext (pbm_mmgsdi_card_status_cb,
                                                     pbm_mmgsdi_reg_status_cb,
                                                     0,
                                                     PBM_CLIENT_NAME,
                                                     sizeof(PBM_CLIENT_NAME));
   if (mmgsdi_status != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_client_id_and_evt_reg failed %d", mmgsdi_status);
      return PBM_ERROR;
   }


   //block waiting for callback
   (void) pbm_wait (PBM_MMGSDI_WAIT_SIG);
   (void) rex_clr_sigs(PBM_TCB, PBM_MMGSDI_WAIT_SIG);

   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION pbm_mmgsdi_register_fcn

DESCRIPTION
  Initialize MMGSDI file change notifications

DEPENDENCIES
  sim init complete and card apps available

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_register_fcn(pbm_session_enum_type session_type)
{
   mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
   pbm_return_type pbm_ret = PBM_ERROR;
   mmgsdi_refresh_file_list_type mmgsdi_refresh_file_list;
   int i = 0;
   uint8 path_index = 0;
   uint32 fileset = 0;
   pbm_file_type pbm_file = PBM_FILE_NONE;


   memset(&mmgsdi_refresh_file_list, 0, sizeof(mmgsdi_refresh_file_list));
   //register with files only applicable for this session
   fileset = pbm_util_return_fileset_for_session(session_type);

   //wee need to allocate the buffer for 8 bytes for every file
   PBM_MEM_ALLOC(mmgsdi_refresh_file_list.file_path_ptr,(int)PBM_FILE_MAX_REQUESTS*4*sizeof(uint16));
   PBM_CHECK_PTR_RET(mmgsdi_refresh_file_list.file_path_ptr, VOID);
   /* register for FCN on all UIM files in SIM */
   for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
   {
      if (pbm_file_to_uim_file[i].pbm_session_filename[fileset] != PBM_FILE_NONE)
      {
        mmgsdi_access_type file_struct;
        pbm_file = pbm_file_to_uim_file[i].pbm_session_filename[fileset];
        pbm_ret = pbm_populate_file_access(session_type,&file_struct, pbm_file);
        if(pbm_ret == PBM_SUCCESS)
        {
        if(file_struct.access_method == MMGSDI_BY_PATH_ACCESS)
        {
          //below loop copies paths in uint16 array to byte array
          //below logic will takes care of both little endian and big endian
          for (path_index= 0; path_index< file_struct.file.path_type.path_len; path_index++)
          {
            mmgsdi_refresh_file_list.file_path_ptr[mmgsdi_refresh_file_list.file_path_len++] =
              (uint8)(file_struct.file.path_type.path_buf[path_index] >> 8);
            mmgsdi_refresh_file_list.file_path_ptr[mmgsdi_refresh_file_list.file_path_len++] =
              (uint8)(file_struct.file.path_type.path_buf[path_index] & 0x00FF);
          }
        }
              }
        }
      }
   mmgsdi_status = mmgsdi_session_register_for_refresh (
                                                       pbm_session_info[session_type].session_id,       /* sending mmgsdi session_id */
                                                       mmgsdi_refresh_file_list,
                                                       FALSE,
                                                       pbm_mmgsdi_refresh_cb,
                                                       0);
   /* Check to see if REQ for Registration was ok */
   if (mmgsdi_status != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("Error %d in Request Refresh FCN Registration",mmgsdi_status);
   }
   PBM_MEM_FREEIF(mmgsdi_refresh_file_list.file_path_ptr);
   return;
}


/*===========================================================================*
 *              PHONE BOOK MANAGER TO GSDI INTERFACE FUNCTIONS.              *
 *              These function are only used internally by PBM               *
 *              and provide the interface to GSDI.                           *
 *===========================================================================*/


/*===========================================================================
FUNCTION PBM_MMGSDI_SIM_READ

DESCRIPTION
  This function is the interface to the gsdi_sim_read function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_sim_read(
                                   pbm_session_enum_type         pbm_session_type,
                                   pbm_file_type sim_filename,
                                   mmgsdi_file_structure_enum_type sim_filetype,
                                   int                   rec_num,
                                   int                   required_data_len,
                                   uint32                client_ref,
                                   mmgsdi_callback_type  async_cb
                                   )
{
   mmgsdi_return_enum_type mmgsdi_retval;
   pbm_return_type pbm_ret = PBM_SUCCESS;
   mmgsdi_session_id_type session_id;
   mmgsdi_access_type file_struct;

   if ( PBM_FILE_FDN == sim_filename )
   {
     pbm_ret = pbm_populate_enum_file_access(pbm_session_type,&file_struct, sim_filename);
   }
   else
   {
   pbm_ret = pbm_populate_file_access(pbm_session_type,&file_struct, sim_filename);
   }

   if(pbm_ret != PBM_SUCCESS)
   {
      UIM_MSG_HIGH_3("PBM: in pbm_mmgsdi_sim_read pbm_populate_file_access returned error %d, no file path found for file %d, session %d",pbm_ret,sim_filename,pbm_session_type);
      return PBM_ERROR;
   }
   session_id = pbm_session_info[pbm_session_type].session_id;
   switch (sim_filetype)
   {
      case MMGSDI_LINEAR_FIXED_FILE:
      case MMGSDI_CYCLIC_FILE:
         mmgsdi_retval = mmgsdi_session_read_record (session_id,
                                                     file_struct,
                                                     rec_num,
                                                     required_data_len,
                                                     async_cb,
                                                     client_ref);
         break;
      case MMGSDI_TRANSPARENT_FILE:
         mmgsdi_retval = mmgsdi_session_read_transparent (session_id,
                                                          file_struct,
                                                          0,
                                                          required_data_len,
                                                          async_cb,
                                                          client_ref);
         break;
      default:
         UIM_MSG_ERR_1("pbm_mmgsdi_sim_read got invalid file type %d", sim_filetype);
         return PBM_ERROR;
   }

   if (mmgsdi_retval != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_read_record returned %d", mmgsdi_retval);
      return PBM_ERROR;
   }
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_MMGSDI_UIM_WRITE

DESCRIPTION
  This function is the interface to the mmgsdi_sim_write function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_uim_write (
   pbm_session_enum_type            session_type,
   pbm_file_type                    sim_filename,
   mmgsdi_file_structure_enum_type  sim_filetype,
   int                              rec_num,
   byte                            *data_p,
   int                              data_len,
   uint32                           client_ref,
   mmgsdi_callback_type             async_cb
)
{
   mmgsdi_return_enum_type mmgsdi_retval;
   pbm_return_type pbm_ret = PBM_SUCCESS;
   mmgsdi_session_id_type mmgsdi_session_id;
   mmgsdi_write_data_type write_data;
   mmgsdi_access_type file_struct;

   UIM_MSG_HIGH_9("PBM: Writing to SIM file: 0x%x, record 0x%x, len 0x%x sim data: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                sim_filename,
                rec_num,
                data_len,
                ((data_p != NULL) && (data_len > 0)) ? data_p[0] : 0,
                ((data_p != NULL) && (data_len > 1)) ? data_p[1] : 0,
                ((data_p != NULL) && (data_len > 2)) ? data_p[2] : 0,
                ((data_p != NULL) && (data_len > 3)) ? data_p[3] : 0,
                ((data_p != NULL) && (data_len > 4)) ? data_p[4] : 0,
                ((data_p != NULL) && (data_len > 5)) ? data_p[5] : 0);

   write_data.data_len = data_len;
   write_data.data_ptr = data_p;
   mmgsdi_session_id = pbm_session_info[session_type].session_id;
   if( PBM_FILE_FDN == sim_filename )
   {
      pbm_ret = pbm_populate_enum_file_access(session_type,&file_struct, sim_filename);
   }
   else
   {
   pbm_ret = pbm_populate_file_access(session_type,&file_struct, sim_filename);
   }
   if(pbm_ret != PBM_SUCCESS)
   {
      UIM_MSG_HIGH_3("error %d session_type %d sim_filename %d",pbm_ret,session_type,sim_filename);
      return PBM_ERROR;
   }
   switch (sim_filetype)
   {
      case MMGSDI_LINEAR_FIXED_FILE:
      case MMGSDI_CYCLIC_FILE:
         {
            mmgsdi_retval = mmgsdi_session_write_record (mmgsdi_session_id,
                                                         file_struct,
                                                         sim_filetype,
                                                         rec_num,
                                                         write_data,
                                                         async_cb,
                                                         client_ref);

         }
         break;
      case MMGSDI_TRANSPARENT_FILE:
         mmgsdi_retval =  mmgsdi_session_write_transparent (mmgsdi_session_id,
                                                            file_struct,
                                                            0,
                                                            write_data,
                                                            async_cb,
                                                            client_ref);
         break;
      default:
         return PBM_ERROR;
   }
   if (mmgsdi_retval != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_write_record returned %d", mmgsdi_retval);
      return PBM_ERROR;
   }
   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_MMGSDI_SESSION_GET_FILE_ATTRIBUTES

DESCRIPTION
  This function is the interface to the gsdi_get_file_attributes function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_get_file_attributes (
                                               pbm_session_enum_type          session_type,
                                               pbm_file_type  sim_filename,
                                               uint32                 client_ref,
                                               mmgsdi_callback_type   async_cb
                                               )
{
   mmgsdi_return_enum_type mmgsdi_retval;
   pbm_return_type pbm_ret = PBM_SUCCESS;
   mmgsdi_access_type file_access_info;
   boolean            skip_uicc_arr = TRUE;

   memset(&file_access_info, 0, sizeof(file_access_info));

   if ( PBM_FILE_FDN == sim_filename )
   {
      pbm_ret = pbm_populate_enum_file_access(session_type,&file_access_info, sim_filename);
   }
   else
   {
   pbm_ret = pbm_populate_file_access(session_type,&file_access_info, sim_filename);
   }

   if(pbm_ret != PBM_SUCCESS)
   {
      UIM_MSG_HIGH_3("error %d session_type %d sim_filename %d",pbm_ret,session_type,sim_filename);
      return PBM_ERROR;
   }
   if(pbm_file_id_to_cache_type(sim_filename) == PBM_CACHE_TYPE_PB)
   {
     //For only phonebook EFs we need the security attributes .
     //For the rest of EFs such as EXT1 , EXT2 , EMAIL , ANR etc ..
     //will inherit the attributes from the Phonebook
     //which they belong to
     //Hence we doesnt need to read
     //security attributes for EFs
     skip_uicc_arr = FALSE;
   }
   mmgsdi_retval =  mmgsdi_session_get_file_attr_ext (
                                                 pbm_session_info[session_type].session_id,
                                                 file_access_info,
                                                 async_cb,
                                                 client_ref,
                                                 skip_uicc_arr);
   if (mmgsdi_retval != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_session_get_file_attr_ext returned %d", mmgsdi_retval);
      return PBM_ERROR;
   }
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_MMGSDI_SESSION_GET_FILE_ATTRIBUTES_PBR

DESCRIPTION
  This function is the interface to the gsdi_get_file_attributes function.
  It should only be used for the files which are to be accessed via enum.
  PBR is the only file as per now.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_get_file_attributes_pbr (
                                               pbm_session_enum_type          session_type,
                                               mmgsdi_file_enum_type  sim_filename,
                                               uint32                 client_ref,
                                               mmgsdi_callback_type   async_cb
                                               )
{
   mmgsdi_return_enum_type mmgsdi_retval;
   mmgsdi_access_type file_access_info;

   memset(&file_access_info, 0, sizeof(file_access_info));
   file_access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
   file_access_info.file.file_enum = sim_filename;
   mmgsdi_retval =  mmgsdi_session_get_file_attr_ext (
                                                 pbm_session_info[session_type].session_id,
                                                 file_access_info,
                                                 async_cb,
                                                 client_ref,
                                                 TRUE);
   if (mmgsdi_retval != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_session_get_file_attr_ext returned %d", mmgsdi_retval);
      return PBM_ERROR;
   }
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_GSDI_ASYNC_CB

DESCRIPTION
  This function is registered with GSDI. A PBM command is added to the
  gsdi_resp queue when we receive this event.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_async_cb(mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   uint32 client_ref;
   uint16 read_cnf_data_len = 0;

   PBM_CHECK_PTR_RET(data_ptr, VOID);

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   if (!mmgsdi_resp_ptr)
   {
      UIM_MSG_ERR_0("Unable to allocate PBM command");
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   client_ref = data_ptr->response_header.client_data;
   if ((client_ref >= PBM_ASYNC_BUFFERS) || (!pbm_buffers[client_ref].in_use))
   {
      UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
      pbm_cmd_free(mmgsdi_resp_ptr);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   mmgsdi_resp_ptr->cmd_type = pbm_buffers[client_ref].gsdi_resp_type;

   mmgsdi_resp_ptr->status = status;
   mmgsdi_resp_ptr->data_type = data_type;
   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));




   if (PBM_MMGSDI_READ_CB == mmgsdi_resp_ptr->cmd_type
       || PBM_DEL_LINKS_READ_CB  == mmgsdi_resp_ptr->cmd_type)
   {
      read_cnf_data_len = data_ptr->read_cnf.read_data.data_len;
      if (data_ptr->read_cnf.read_data.data_len > 0)
      {
        PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,
                                                       read_cnf_data_len);
        PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,VOID);

         memscpy((void *)mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr, read_cnf_data_len,
                (void *)data_ptr->read_cnf.read_data.data_ptr,
                read_cnf_data_len); //deep copy
      } else
      {
         mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr = NULL;
      }
   } else if (PBM_MMGSDI_WRITE_CB != mmgsdi_resp_ptr->cmd_type)
   {
      //print error
      UIM_MSG_ERR_1("Unexpected cmd_type %d", mmgsdi_resp_ptr->cmd_type);
   }

   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));

} /*pbm_mmgsdi_async_cb...*/


/*===========================================================================
FUNCTION   pbm_cm_ss_event_cb

DESCRIPTION
   This function is called when we get an SS event.

   This function is enhanced to listen the CM SS event which sends the emergency list 
   per sub and per stack 

DEPENDENCIES
   None

RETURNS
   None

SIDE EFFECTS
===========================================================================*/
void pbm_cm_ss_event_cb (
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type  *ii_info_ptr
)
{
  pbm_cm_ss_evt_type   *cm_ss_evt_ptr              = NULL;
  boolean               srv_or_roam_status_changed = FALSE;
  uint8                 stack_idx                  = 0; 

  if(ii_info_ptr == NULL)
  {
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return;
  }

  if(ss_event == CM_SS_EVENT_SRV_CHANGED)
  {
    for (stack_idx = 0; stack_idx < ii_info_ptr->number_of_stacks && stack_idx < CM_NO_STACKS; stack_idx++)
    {
      UIM_MSG_HIGH_7("PBM Received CM SS Event, stack:0x%x is operational 0x%x, ASUBS ID:0x%x, Service Status: 0x%x Sys Mode: 0x%x Changed fields: 0x%x roam_status : 0x%x",
                      stack_idx,
                      ii_info_ptr->stack_info[stack_idx].is_operational,
                      ii_info_ptr->asubs_id,
                      ii_info_ptr->stack_info[stack_idx].srv_status,
                      ii_info_ptr->stack_info[stack_idx].sys_mode,
                      ii_info_ptr->stack_info[stack_idx].changed_fields,
                      ii_info_ptr->stack_info[stack_idx].roam_status);

      switch(ii_info_ptr->asubs_id)
      {
        case SYS_MODEM_AS_ID_1:
           if (PBM_SS_INFO_CHANGED(ii_info_ptr->stack_info[0].changed_fields) ||
               (stack_idx == 1 &&
                ((ii_info_ptr->stack_info[1].is_operational &&
                  PBM_SS_INFO_CHANGED(ii_info_ptr->stack_info[1].changed_fields)) ||
                 (!ii_info_ptr->stack_info[1].is_operational &&
                  ii_info_ptr->stack_info[0].srv_status != SYS_SRV_STATUS_SRV))))
          {
            srv_or_roam_status_changed = TRUE;
          }
          break;

        case SYS_MODEM_AS_ID_2:
        case SYS_MODEM_AS_ID_3:
          if (PBM_SS_INFO_CHANGED(ii_info_ptr->stack_info[0].changed_fields))
          {
            srv_or_roam_status_changed = TRUE;
          }
          break;

        default:
	  break;
      }
    }
  }

  UIM_MSG_HIGH_2("pbm_cm_ss_event_cb, ss_event:0x%x srv_status_changed 0x%x",
                  ss_event,
                  srv_or_roam_status_changed);

  if (!srv_or_roam_status_changed &&
       ss_event == CM_SS_EVENT_SRV_CHANGED)
  {
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return;
  }

  cm_ss_evt_ptr = (pbm_cm_ss_evt_type *)
                    pbm_cmd_alloc(sizeof(pbm_cm_ss_evt_type));
  if (!cm_ss_evt_ptr)
  {
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return;
  }
  PBM_MEM_ALLOC(cm_ss_evt_ptr->ii_info_ptr,sizeof(cm_mm_msim_ss_info_s_type));
  PBM_CHECK_PTR_RET(cm_ss_evt_ptr->ii_info_ptr,VOID);
  cm_ss_evt_ptr->cmd_type = PBM_CM_SS_CB;
  cm_ss_evt_ptr->ss_event = ss_event;

  memscpy(cm_ss_evt_ptr->ii_info_ptr, sizeof(cm_mm_msim_ss_info_s_type), ii_info_ptr,sizeof(cm_mm_msim_ss_info_s_type));

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(cm_ss_evt_ptr));
} /* pbm_cm_ss_event_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_FDN_EXT_READ_CB

DESCRIPTION
  This function is registered with GSDI. A PBM command is added to the
  gsdi_resp queue when we receive this event.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_fdn_ext_read_cb(mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   uint16 read_cnf_data_len = 0;
   PBM_CHECK_PTR_RET(data_ptr, VOID);

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   if (!mmgsdi_resp_ptr)
   {
      UIM_MSG_ERR_0("Unable to allocate PBM command");
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }
   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_FDN_EXT_READ_CB;
   mmgsdi_resp_ptr->status = status;
   mmgsdi_resp_ptr->data_type = data_type;
   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));
   if (data_ptr->read_cnf.read_data.data_len > 0)
   {
     read_cnf_data_len = data_ptr->read_cnf.read_data.data_len;
     PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,
                   read_cnf_data_len);
     PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,VOID);
      memscpy((void *)mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr, read_cnf_data_len,
             (void *)data_ptr->read_cnf.read_data.data_ptr,
             read_cnf_data_len); //deep copy
   } else
   {
      mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr = NULL;
   }
   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
} /*pbm_mmgsdi_async_cb...*/

/*===========================================================================
FUNCTION PBM_MMGSDI_UIM_WRITE_CB

DESCRIPTION
  After MMGSDI has written any SIM/USIM/CSIM record, this function is
  called. The appropriate callback function is called based on the
  file type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_uim_write_cb( mmgsdi_return_enum_type status,
                              mmgsdi_cnf_enum_type    data_type,
                              const mmgsdi_cnf_type  *data_ptr)
{
   uint32 client_ref = 0;

   if (!data_ptr || data_type != MMGSDI_WRITE_CNF ||
       ((client_ref = data_ptr->response_header.client_data) >= PBM_ASYNC_BUFFERS) || 
       (!pbm_buffers[client_ref].in_use))
   {
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   UIM_MSG_HIGH_1("pbm_mmgsdi_uim_write_cb file_id 0x%x",
                   pbm_buffers[client_ref].record.file_id);

   switch (pbm_file_id_to_cache_type(pbm_buffers[client_ref].record.file_id))
   {
      case PBM_CACHE_TYPE_PB:
         pbm_sim_write_cb(status, data_ptr);
         break;

      case PBM_CACHE_TYPE_USIM:
         pbm_usim_write_cb(status, data_ptr);
         break;

      case PBM_CACHE_TYPE_SYNC1:
      case PBM_CACHE_TYPE_SYNC2:
         pbm_sync_write_cb(status, data_ptr);
         break;

      case PBM_CACHE_TYPE_INDEX:
         pbm_index_write_cb(status, data_ptr);
         break;
      case PBM_CACHE_TYPE_EXT:
         pbm_ext_write_cb(status, data_ptr);
         break;

      default:
         break;
   }
}


/*===========================================================================
FUNCTION PBM_MMGSDI_READ_CB

DESCRIPTION
  After MMGSDI has read any SIM/CSIM/USIM record this function is called.
  This function makes sure that the data sent by GSDI is sane.

  The appropriate pbm read callback is called based on the file type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_read_cb( mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)
{
   uint32 client_ref;
   pbm_session_enum_type session_type;

   PBM_CHECK_PTR_RET(data_ptr, VOID);

   client_ref = data_ptr->response_header.client_data;

   if (data_type != MMGSDI_READ_CNF)
   {
      UIM_MSG_ERR_1("Invalid response type to a GSDI read. %d",
                  data_type);
      return;
   }

   if ((client_ref >= PBM_ASYNC_BUFFERS) ||
       (pbm_buffers[client_ref].in_use == FALSE))
   {
      UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
      return;
   }

   session_type = pbm_mmgsdi_session_to_pbm_session(
                                                   data_ptr->response_header.session_id);

   if (pbm_buffers[client_ref].record.session_type != session_type)
   {
      UIM_MSG_ERR_3("Wrong callback called for file %d and session %d, %d",
                  pbm_buffers[client_ref].record.file_id,
                  pbm_buffers[client_ref].record.session_type, session_type);
      return;
   }

   // Restart initialization if required
   if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
      UIM_MSG_HIGH_1("PBM restarting init of SIM %d", session_type);
      pbm_restart_fileinfo_internal(session_type);
      pbm_free_record((int) client_ref);
      return;
   }

   // Stop initialization if required
   if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type)))
   {
      UIM_MSG_HIGH_1("PBM stopping init of SIM %d", session_type);
      pbm_free_record((int) client_ref);
      return;
   }

   /* GSDI data looks fine */
   switch (pbm_file_id_to_cache_type(pbm_buffers[client_ref].record.file_id))
   {
      case PBM_CACHE_TYPE_PB:
         pbm_mmgsdi_sim_read_cb(status, data_ptr);
         break;

      case PBM_CACHE_TYPE_EXT:
         pbm_mmgsdi_ext_read_cb(status, data_ptr);
         break;

      case PBM_CACHE_TYPE_USIM:
         pbm_mmgsdi_usim_read_cb(status, data_ptr);
         break;

      case PBM_CACHE_TYPE_SYNC1:
      case PBM_CACHE_TYPE_SYNC2:
         pbm_mmgsdi_sync_read_cb(status, data_ptr);
         break;

      case PBM_CACHE_TYPE_INDEX:
         pbm_mmgsdi_index_read_cb(status, data_ptr);
         break;

      default:
         UIM_MSG_HIGH_1("Unknown file id %d", pbm_buffers[client_ref].record.file_id);
         pbm_free_record((int) client_ref);
         break;
   }

   return;

} /* pbm_mmgsdi_read_cb...*/


/*===========================================================================
FUNCTION PBM_MMGSDI_EXT_READ_CB

DESCRIPTION
  After GSDI has retreived the extension record from EXT1 or EXT5,
  PBM_EXT_READ_CB is called to copy the data in to a pbm_buffer
  for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_ext_read_cb(
  mmgsdi_return_enum_type        status,
  const mmgsdi_cnf_type         *data_ptr)
{
  uint32                      client_ref    = PBM_ASYNC_BUFFERS;
  byte                       *bcd_number    = NULL;
  pbm_field_id_e_type         field_id      = PBM_FIELD_NONE;
  pbm_return_type             ret_val       = PBM_ERROR;
  pbm_session_enum_type       session_type  = PBM_SESSION_DEFAULT;
  pbm_ext_cache_s_type       *cache         = NULL;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if((client_ref >= PBM_ASYNC_BUFFERS) ||
     (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }

  session_type = pbm_mmgsdi_session_to_pbm_session(
                                    data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  cache = (pbm_ext_cache_s_type *)pbm_file_id_to_cache(session_type,
                                                       pbm_buffers[client_ref].record.file_id);
  if(cache == NULL)
  {
    pbm_free_record((int) client_ref);
    return;
  }

  pbm_buffers[client_ref].record.file_id = pbm_buffers[client_ref].m_file_id;
  if (status == MMGSDI_SUCCESS &&
      cache->ext_rec_length == data_ptr->read_cnf.read_data.data_len)
  {
    field_id = pbm_file_id_to_field_id(pbm_buffers[client_ref].m_file_id);
    if (field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
    {
      bcd_number = (byte *)&pbm_buffers[client_ref].record.data.anr.anr_data[1];
      if(data_ptr->read_cnf.read_data.data_ptr[1] != 0xFF &&
         ((bcd_number[BCD_LEN] + data_ptr->read_cnf.read_data.data_ptr[1]) > 
           sizeof(pbm_buffers[client_ref].record.data.anr.anr_data)))
      {
        /* saving truncated addtional number if EF-ANR length + extension number length
           saved in EF-EXT exceeds size of destination buffer */
        data_ptr->read_cnf.read_data.data_ptr[1] = 
             sizeof(pbm_buffers[client_ref].record.data.anr.anr_data) - bcd_number[BCD_LEN];
      }
    }
    else
    {
      bcd_number = (byte *)&pbm_buffers[client_ref].record.data.num_text.bcd_number;
      if(data_ptr->read_cnf.read_data.data_ptr[1] != 0xFF &&
         ((bcd_number[BCD_LEN] + data_ptr->read_cnf.read_data.data_ptr[1]) > 
           sizeof(pbm_buffers[client_ref].record.data.anr.anr_data)))
      {
        /* saving truncated BCD number */
        data_ptr->read_cnf.read_data.data_ptr[1] = 
              sizeof(pbm_buffers[client_ref].record.data.num_text.bcd_number) - bcd_number[BCD_LEN];
      }
    }

    (void)pbm_mmgsdi_process_ext_data(data_ptr->read_cnf.read_data.data_ptr, bcd_number);
    ret_val = PBM_SUCCESS;
  }

  /* Read data processing */
  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func)(ret_val,
                                         &pbm_buffers[client_ref].record);
  }
  pbm_free_record((int) client_ref);
  return;
} /* pbm_mmgsdi_ext_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_INDEX_READ_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_index_read_cb(
  mmgsdi_return_enum_type   status,
  const mmgsdi_cnf_type    *data_ptr)
{
  uint32                      client_ref    = PBM_ASYNC_BUFFERS;
  pbm_session_enum_type       session_type  = PBM_SESSION_DEFAULT;
  PB_MASTER_USIM_CACHE       *iap_cache     = NULL;
  pbm_return_type             ret_val       = PBM_ERROR;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if((client_ref >= PBM_ASYNC_BUFFERS) ||
     (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }

  session_type = pbm_mmgsdi_session_to_pbm_session(
                                    data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  iap_cache = (PB_MASTER_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                           pbm_buffers[client_ref].record.file_id);
  if(iap_cache == NULL)
  {
    pbm_free_record((int) client_ref);
    return;
  }

  if (status == MMGSDI_SUCCESS &&
      iap_cache->iap_rec_len == data_ptr->read_cnf.read_data.data_len)
  {
    if (pbm_mmgsdi_process_index_data(pbm_buffers[client_ref].record.session_type,
                                      pbm_buffers[client_ref].record.file_id,
                                      data_ptr->read_cnf.accessed_rec_num,
                                      data_ptr->read_cnf.read_data.data_ptr,
                                      &pbm_buffers[client_ref].record.data.iap) == PBM_SUCCESS)
    {
      ret_val = PBM_SUCCESS;
    }
  }

  /* Read failed */
  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func) (ret_val,
                                          &pbm_buffers[client_ref].record);
  }

  pbm_free_record((int) client_ref);
  return;
} /* pbm_mmgsdi_index_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_SYNC_READ_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_sync_read_cb(
  mmgsdi_return_enum_type        status,
  const mmgsdi_cnf_type         *data_ptr)
{
  uint32                   client_ref    = PBM_ASYNC_BUFFERS;
  pbm_return_type          ret           = PBM_SUCCESS;
  byte                    *data          = NULL;
  void                    *cache         = NULL;
  pbm_session_enum_type    session_type  = PBM_SESSION_DEFAULT;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref   = data_ptr->response_header.client_data;
  if((client_ref >= PBM_ASYNC_BUFFERS) ||
     (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }

  session_type = pbm_mmgsdi_session_to_pbm_session(
                                    data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  cache = (void *)pbm_file_id_to_cache(session_type,
                               pbm_buffers[client_ref].record.file_id);
  if(cache == NULL)
  {
    pbm_free_record((int) client_ref);
    return;
  }

  if (status == MMGSDI_SUCCESS)
  {
    data = data_ptr->read_cnf.read_data.data_ptr;
    switch (pbm_buffers[client_ref].record.file_id)
    {
      case PBM_FILE_PSC:
      {
        PB_SYNC1_CACHE *sync1_cache = (PB_SYNC1_CACHE *)cache;

        /* As per TS 31.102 4.4.2.12.2 EF-PSC (Phone book Synchronisation Counter) 
           length should be 4 bytes */
        if(sync1_cache->record_length >= 4)
        {
          pbm_buffers[client_ref].record.data.dword = data[0] << 24 |
                                                      data[1] << 16 |
                                                      data[2] << 8  |
                                                      data[3];
        }
        else
        {
          /* returning error for non-spec complaint cards */
          ret = PBM_ERROR;
        }
      }
        break;

      case PBM_FILE_PUID:
      case PBM_FILE_CC:
      {
        PB_SYNC1_CACHE *sync1_cache = (PB_SYNC1_CACHE *)cache;

        /* As per TS 31.102 4.4.2.12.3 EF-CC and 4.4.2.12.4 EF-PUID
           lengths should be 2 bytes */
        if(sync1_cache->record_length >= 2)
        {
          pbm_buffers[client_ref].record.data.word = data[0] << 8 |
                                                     data[1];
        }
        else
        {
          /* returning error for non-spec complaint cards */
          ret = PBM_ERROR;
        }
      }
      break;
      case PBM_FILE_PBC:
      case PBM_FILE_PBC1:
      case PBM_FILE_PBC2:
      case PBM_FILE_PBC3:
      case PBM_FILE_UID:
      case PBM_FILE_UID1:
      case PBM_FILE_UID2:
      case PBM_FILE_UID3:
      {
        PB_SYNC2_CACHE *sync2_cache = (PB_SYNC2_CACHE *)cache;

        /* As per TS 31.102 4.4.2.5 EF-PBC and 4.4.2.12.1 EF-UID
           lengths should be 2 bytes */
        if(sync2_cache->record_length >= 2)
        {
          pbm_buffers[client_ref].record.data.word = data[0] << 8 |
                                                     data[1];
        }
        else
        {
          /* returning error for non-spec complaint cards */
          ret = PBM_ERROR;
        }
      }
      break;

      default:
        UIM_MSG_ERR_1("Unexpected file_id %d",
                       pbm_buffers[client_ref].record.file_id);
        ret = PBM_ERROR;
        break;
    }
  }

  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func)( ret,
                                          &pbm_buffers[client_ref].record);
  }

  pbm_free_record((int) client_ref);
  return;
} /* pbm_mmgsdi_sync_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_SIM_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the SIM, PBM_READ_CB is
  called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_sim_read_cb(
  mmgsdi_return_enum_type     status,
  const mmgsdi_cnf_type      *data_ptr)
{
  int                           extension_rec_num  = 0;
  /* this file id points to the file from buffer, i.e. for which read_cb has been called */
  pbm_file_type                 file_id            = PBM_FILE_NONE;
  uint32                        client_ref         = PBM_ASYNC_BUFFERS;
  PB_CACHE                     *cache              = NULL;
  pbm_session_enum_type         session_type       = PBM_SESSION_DEFAULT;
  boolean                       sim_data_processed = FALSE;
  pbm_pb_set_type               pb_set             = PBM_USIM_PB_INVALID;
  pbm_return_type               ret_val            = PBM_ERROR;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if ((client_ref >= PBM_ASYNC_BUFFERS) ||
      (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }
  file_id = pbm_buffers[client_ref].record.file_id;

  pb_set = pbm_file_id_to_pb_set(file_id);
  if(file_id != PBM_FILE_ADN && file_id != PBM_FILE_ADN1 && 
     file_id != PBM_FILE_ADN2 && file_id != PBM_FILE_ADN3)
  {
    pb_set = PBM_USIM_PB_ONE;
  }

  if(!pbm_validate_pb_set(pb_set))
  {
    pbm_free_record((int) client_ref);
    return;
  }
  session_type = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);

  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  cache = (PB_CACHE *) pbm_file_id_to_cache (session_type, file_id);
  if (!cache)
  {
    pbm_free_record((int) client_ref);
    UIM_MSG_ERR_2("No Cache found for session_type %d, file_id %d\n",
                  session_type, file_id);
    return;
  }

  /* If the read was successful, process the extension if needed */
  if (status == MMGSDI_SUCCESS &&
      cache->record_length[pb_set] == data_ptr->read_cnf.read_data.data_len)
  {
    if(file_id == PBM_FILE_GAS || file_id == PBM_FILE_AAS)
    {
      memscpy(pbm_buffers[client_ref].record.data.num_text.sim_text,
              sizeof(pbm_buffers[client_ref].record.data.num_text.sim_text),
              data_ptr->read_cnf.read_data.data_ptr,cache->text_length[pb_set]);
      /* Processing was complete. Life is good. */
      ret_val = PBM_SUCCESS;
    } 
    /* MDN in RUIM & CSIM case is read as PBM_FILE_MSISDN and it need a 
       special processing , since the rec structure is different to MSISDN in GW */
    else if(file_id == PBM_FILE_MSISDN &&
             (USE_CSIM_LOCAL_SUPPORT(session_type) || USE_CDMA_SUPPORT(session_type)))
    {
      if( pbm_mmgsdi_process_mdn_data(session_type,
                                      file_id,
                                      pbm_buffers[client_ref].record.index,
                                      data_ptr->read_cnf.read_data.data_ptr,
                                      &pbm_buffers[client_ref].record.data.num_text))
      {
        sim_data_processed = TRUE;
      }
    }
    else if ( pbm_mmgsdi_process_sim_data(session_type,
                                          file_id,
                                          pbm_buffers[client_ref].record.index,
                                          data_ptr->read_cnf.read_data.data_ptr,
                                          &pbm_buffers[client_ref].record.data.num_text,
                                          &extension_rec_num))
    {
      sim_data_processed = TRUE;
    }
    if(sim_data_processed == TRUE)
    {
      /* Processing was complete. Life is good. */
      ret_val = PBM_SUCCESS;
    }
    else
    {
      if (extension_rec_num && cache->extension_cache &&
          (cache->extension_cache->file_id != PBM_FILE_NONE))
      {
        pbm_buffers[client_ref].m_file_id      = file_id;
        pbm_buffers[client_ref].record.file_id =
                   cache->extension_cache->file_id;
        pbm_buffers[client_ref].gsdi_resp_type = PBM_MMGSDI_READ_CB;
        if (pbm_send_read_request(pbm_buffers[client_ref].record.session_type,
                                  cache->extension_cache->file_id,
                                  extension_rec_num, (int) client_ref,
                                  pbm_mmgsdi_async_cb))
        {
          return;
        }
        UIM_MSG_ERR_0("Could not issue read request");
      }
      else
      {
        UIM_MSG_ERR_2("PBM could not find a valid extension record for %d from cache 0x%x",
                      file_id, cache);
      }
    }
  }
  else
  {
    UIM_MSG_ERR_3("status 0x%x session_type 0x%x file_id 0x%x",
                   status, session_type, file_id);
  }

  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func) ( ret_val,
                                           &pbm_buffers[client_ref].record);
  }
  pbm_free_record((int) client_ref);
}/* pbm_mmgsdi_sim_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_USIM_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the USIM, pbm_mmgsdi_usim_read_cb is
  called to copy the data in. At this point we process the data from USIM into
  a generic SIM record type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_usim_read_cb(
  mmgsdi_return_enum_type       status,
  const mmgsdi_cnf_type        *data_ptr)
{
  pbm_return_type                  ret               = PBM_SUCCESS;
  int                              extension_rec_num = 0;
  pbm_ext_cache_s_type            *ext_cache         = NULL;
  uint32                           client_ref        = PBM_ASYNC_BUFFERS;
  PB_USIM_CACHE                   *usim_cache        = NULL;
  pbm_file_type                    file_id           = PBM_FILE_NONE;
  pbm_session_enum_type            session_type      = PBM_SESSION_DEFAULT;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  session_type = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  client_ref = data_ptr->response_header.client_data;
  if ((client_ref >= PBM_ASYNC_BUFFERS) ||
      (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }
  file_id = pbm_buffers[client_ref].record.file_id;

  usim_cache = (PB_USIM_CACHE *) pbm_file_id_to_cache(session_type, file_id);
  if (!usim_cache)
  {
    pbm_free_record((int) client_ref);
    UIM_MSG_ERR_2("No cache found for session_type %d, file_id %d\n",
                  session_type, file_id);
    return;
 }

  do
  {
    if (status != MMGSDI_SUCCESS ||
        (usim_cache->record_length != data_ptr->read_cnf.read_data.data_len))
    {
      UIM_MSG_ERR_2("PBM Error %d from GSDI for file %d",
                     status, file_id);
      ret = PBM_ERROR;
      break;
    }

      switch (file_id)
      {
         case PBM_FILE_EMAIL:
         case PBM_FILE_EMAIL1:
         case PBM_FILE_SNE:
         case PBM_FILE_SNE1:
         case PBM_FILE_EMAIL2:
         case PBM_FILE_EMAIL3:
         case PBM_FILE_SNE2:
         case PBM_FILE_SNE3:
            ret = pbm_mmgsdi_process_text_data(session_type,
                                               file_id,
                                               data_ptr->read_cnf.accessed_rec_num,
                                               data_ptr->read_cnf.read_data.data_ptr,
                                               &pbm_buffers[client_ref].record.data.text);
            break;

         case PBM_FILE_GRP:
         case PBM_FILE_GRP1:
         case PBM_FILE_GRP2:
         case PBM_FILE_GRP3:
            ret = pbm_mmgsdi_process_grp_data(session_type,
                                              file_id,
                                              data_ptr->read_cnf.accessed_rec_num,
                                              data_ptr->read_cnf.read_data.data_ptr,
                                              &pbm_buffers[client_ref].record.data.grp);
            break;

         case PBM_FILE_ANR:
         case PBM_FILE_ANRA:
         case PBM_FILE_ANRB:
         case PBM_FILE_ANRC:
         case PBM_FILE_ANR1:
         case PBM_FILE_ANRA1:
         case PBM_FILE_ANRB1:
         case PBM_FILE_ANRC1:
         case PBM_FILE_ANR2:
         case PBM_FILE_ANRA2:
         case PBM_FILE_ANRB2:
         case PBM_FILE_ANRC2:
         case PBM_FILE_ANR3:
         case PBM_FILE_ANRA3:
         case PBM_FILE_ANRB3:
         case PBM_FILE_ANRC3:
            ret = pbm_mmgsdi_process_anr_data(session_type,
                                              file_id,
                                              data_ptr->read_cnf.accessed_rec_num,
                                              data_ptr->read_cnf.read_data.data_ptr,
                                              &pbm_buffers[client_ref].record.data.anr,
                                              &extension_rec_num);
            break;

      default:
        ret = PBM_ERROR;
        break;
    }

      if (ret != PBM_SUCCESS)
      {
         UIM_MSG_ERR_2("Error %d in processing data from file %d", ret, file_id);
         break;
      }

      /* If there are no extension records, we are done here */
      if (!extension_rec_num)
         break;

      if (pbm_file_id_to_field_id(file_id) != PBM_FIELD_ADDITIONAL_NUMBER_EXT)
      {
         UIM_MSG_ERR_2("File id %d is not supposed to have extension record %d",
                     file_id, extension_rec_num);
         ret = PBM_ERROR;
         break;
      }

      ext_cache = &ext1_cache[session_type];
      if (ext_cache->file_id == PBM_FILE_NONE)
      {
         UIM_MSG_ERR_1("PBM could not find a valid extension record for %d",
                     file_id);
         ret = PBM_ERROR;
         break;
      }

      pbm_buffers[client_ref].m_file_id = file_id;
      pbm_buffers[client_ref].record.file_id = ext_cache->file_id;
      pbm_buffers[client_ref].gsdi_resp_type = PBM_MMGSDI_READ_CB;
      if (!pbm_send_read_request(pbm_buffers[client_ref].record.session_type,
                                 ext_cache->file_id,
                                 extension_rec_num, (int) client_ref,
                                 pbm_mmgsdi_async_cb))
      {
         UIM_MSG_ERR_2("Could not issue read request for %d index %d",
                     ext_cache->file_id, extension_rec_num);
         ret = PBM_ERROR;
         break;
      }

      return; /* We have sent an extension request, buffer is not freed yet */

   } while (FALSE); /*lint !e717 */

   if (pbm_buffers[client_ref].proc_func != NULL)
   {
      (*pbm_buffers[client_ref].proc_func)( ret,
                                            &pbm_buffers[client_ref].record);
   }

   pbm_free_record((int) client_ref);
   return;
}/* pbm_mmgsdi_usim_read_cb */


/*===========================================================================
FUNCTION PBM_CREATE_SIM_RECORD

DESCRIPTION
  Does the writes of the non-extension record.
  If the phone number is longer than the max for the file in question
  (e.g. >ADN_MAX_PHONE_NUMBER), the remainder of the phone number was
  stored in the extension file after it was converted into BCD.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_sim_record(pbm_session_enum_type       session_type,
                                      pbm_file_type      file_id,
                                      pbm_num_text_record_s_type *record,
                                      int                         buffer_index )
{
   pbm_return_type                        pbm_retval;
   mmgsdi_file_structure_enum_type        filetype;
   byte                                   local_pbm_data_buffer[ PBM_MAX_DATA_BLOCK_LEN ];
   pbm_fixed_sim_rec_s_type               *fixed_sim_record;
   time_julian_type                       julian_time;
   PB_CACHE                               *cache;
   int                                    index;
   int                                    cache_rec_len = 0;
   int                                    cache_text_len = 0;
   pbm_pb_set_type                        pb_set = pbm_file_id_to_pb_set(file_id);

   PBM_CHECK_PTR_RET(record,PBM_ERROR);
   cache = pbm_file_id_to_cache(session_type, file_id);
   if (cache == NULL)
   {
      UIM_MSG_ERR_2("No cache session_type = %d, file_id = %d", session_type, file_id);
      return PBM_ERROR;
   }
   
   if(pb_set != PBM_USIM_PB_INVALID)
   {
     cache_rec_len  = cache->record_length[pb_set];
     cache_text_len = cache->text_length[pb_set];
   }
   else
   {
     cache_rec_len  = cache->record_length[0];
     cache_text_len = cache->text_length[0];
   }

   index = pbm_pb_record_index_to_fileloc(cache, record->index);
   filetype = cache->file_type;

   if (file_id == PBM_FILE_NONE || index < 0)
   {
      UIM_MSG_ERR_3("Invalid cache/index %d, %d, %d", file_id, record->index,
                  cache->num_files);
      return PBM_ERROR_INDEX_INVALID;
   }


   /* Set local buffer to FF's. Therefore, all unwritten fields will be 0xFF. */
   memset ( (byte *)local_pbm_data_buffer, 0xFF,
            sizeof (local_pbm_data_buffer));
   if (((uint16*)record->sim_text)[0] != 0xFFFF)
   {
     memscpy(local_pbm_data_buffer,sizeof(local_pbm_data_buffer),
              record->sim_text,cache_text_len);
   }
   if(pbm_buffers[buffer_index].record.file_id != PBM_FILE_GAS
       && pbm_buffers[buffer_index].record.file_id != PBM_FILE_AAS)
   {
      //lint -e{826} struct has oci/ici, even though we may not access it
      fixed_sim_record = (pbm_fixed_sim_rec_s_type *) (local_pbm_data_buffer +
                                                    cache_text_len);

      /* The phone number length is stored in buffer[0].  Check length
            * and copy the local buffer. */
      if (record->bcd_number[0] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
      {
         memscpy((byte *)&fixed_sim_record->ton_npi, 1, &record->bcd_number[1], 1);
         memscpy((byte *)fixed_sim_record->number, PBM_SIM_DIALING_NUMBER_LEN, &record->bcd_number[2],
                PBM_SIM_DIALING_NUMBER_LEN);
         fixed_sim_record->bcd_len = (PBM_SIM_DIALING_NUMBER_LEN + 1);
      } 
      else
      {
         if (record->bcd_number[0])
            memscpy((byte *)fixed_sim_record, record->bcd_number[0]+1, record->bcd_number, record->bcd_number[0]+1);
      }
	  
      fixed_sim_record->ext_id = pbm_buffers[buffer_index].extension;
      if ((cache_rec_len - cache_text_len) >= OCI_KNOWN_REC_LEN)
      {
         time_jul_from_secs(record->time, &julian_time);
         fixed_sim_record->date_time[0] = pbm_bin_to_bcd( (byte) (julian_time.year % 100) );
         fixed_sim_record->date_time[1] = pbm_bin_to_bcd( (byte) (julian_time.month) );
         fixed_sim_record->date_time[2] = pbm_bin_to_bcd( (byte) (julian_time.day) );
         fixed_sim_record->date_time[3] = pbm_bin_to_bcd( (byte) (julian_time.hour) );
         fixed_sim_record->date_time[4] = pbm_bin_to_bcd( (byte) (julian_time.minute) );
         fixed_sim_record->date_time[5] = pbm_bin_to_bcd( (byte) (julian_time.second) );
         fixed_sim_record->date_time[6] = 0xFF;  /* time zone not supported */

         fixed_sim_record->duration[0] = (byte) ((record->duration & 0xFF0000) >> 16);
         fixed_sim_record->duration[1] = (byte) ((record->duration & 0x00FF00) >> 8);
         fixed_sim_record->duration[2] = (byte) ( record->duration & 0x0000FF);

         if ((cache_rec_len - cache_text_len) >= ICI_KNOWN_REC_LEN)
         {
            /* ICI status LSB = 0 if call answered, 1 if call not answered */
            fixed_sim_record->ici_oci_ending.ici_end.ici_status =
            ( record->is_answered ? 0x00 : 0x01 );
         }
      }
   }
   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                     file_id,
                                     filetype,
                                     index,
                                     local_pbm_data_buffer,
                                     cache_rec_len,
                                     (uint32) buffer_index,
                                     pbm_mmgsdi_async_cb);

  if (pbm_retval != PBM_SUCCESS)
  {
    return PBM_ERROR;
  }

   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_CREATE_EXT_RECORD

DESCRIPTION
  This creates an extension record, and writes it to GSDI.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_ext_record(uint32 buffer_index,
                                      pbm_ext_cache_s_type *ext_cache,
                                      const byte *num_buffer,
                                      uint16 m_index,
                                      pbm_file_type m_filename)
{
   byte *gsdi_buffer=NULL;
   pbm_session_enum_type session_type = pbm_buffers[buffer_index].record.session_type;
   int extension_record;
   uint32 extension_len;
   pbm_return_type pbm_retval;

   PBM_CHECK_PTR2_RET(ext_cache, num_buffer, PBM_ERROR);

   PBM_MEM_ALLOC(gsdi_buffer, (uint32)ext_cache->ext_rec_length);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
   memset(gsdi_buffer, 0xFF, (uint32)ext_cache->ext_rec_length);

   /* buffer contains the bcd in the format    */
   /* byte  0      1      2 thru buffer[0]     */
   /*     <len><ton/npi><bcd packed number>    */
   /* Note that length is the length including */
   /* the TON/NPI field.                       */

   extension_record = pbm_find_extension_record(ext_cache, m_index,
                                                m_filename);
   if (!extension_record)
   {
      UIM_MSG_ERR_3("pbm_find_extension_record() failed %d %d %d", ext_cache, m_index, m_filename);
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR_MEM_FULL;
   }


   pbm_buffers[buffer_index].extension = (byte) extension_record;
   extension_len = MIN((num_buffer[0] - 1) - PBM_SIM_DIALING_NUMBER_LEN,
                       PBM_SIM_DIALING_NUMBER_LEN);

   gsdi_buffer[0] = (byte) EXT_REC_TYPE_ADDITIONAL_DATA;
   gsdi_buffer[1] = (byte) extension_len;

   memscpy(&gsdi_buffer[2], extension_len, &num_buffer[PBM_SIM_DIALING_NUMBER_LEN + 2],
          extension_len);

   pbm_buffers[buffer_index].record.file_id = ext_cache->file_id;
   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write ( session_type,
                                       ext_cache->file_id,
                                       ext_cache->file_type,
                                       extension_record,
                                       gsdi_buffer,
                                       EXT_KNOWN_REC_LEN,
                                       (uint32) buffer_index,
                                       pbm_mmgsdi_async_cb);

   PBM_MEM_FREEIF(gsdi_buffer);
   return pbm_retval;
}


/*===========================================================================
FUNCTION PBM_CLEAR_EXT_RECORD

DESCRIPTION
  This finds the extension record associated with a record, and
  writes a blank record in its place to GSDI.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_clear_ext_record(uint32 buffer_index,
                                     pbm_ext_cache_s_type *ext_cache,
                                     uint16 m_index,
                                     pbm_file_type m_filename)
{
   byte *gsdi_buffer=NULL;
   pbm_session_enum_type session_type = pbm_buffers[buffer_index].record.session_type;
   int extension_record, i=0;
   pbm_return_type pbm_retval;
   pbm_g_other_field_info_s_type *field_info;

   PBM_CHECK_PTR_RET(ext_cache, PBM_ERROR_MEM_FULL);

   PBM_MEM_ALLOC(gsdi_buffer, (uint32)ext_cache->ext_rec_length);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
   memset(gsdi_buffer, 0xFF, (uint32)ext_cache->ext_rec_length);

   /* buffer contains the bcd in the format    */
   /* byte  0      1      2 thru buffer[0]     */
   /*     <len><ton/npi><bcd packed number>    */
   /* Note that length is the length including */
   /* the TON/NPI field.                       */

   extension_record = pbm_free_extension_record(ext_cache,
                                                m_index,
                                                m_filename);

   if (!extension_record)
   {
      UIM_MSG_ERR_3("pbm_free_extension_record() failed %d %d %d", ext_cache, m_index, m_filename);
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
   }
   /* Since the one of the extension record has been free
   PBM can make the number length as 42 for ADN/FDN field info */
   field_info = &g_sim_field_info[session_type][0]; /* ADN info */
   for ( i = 0; i < field_info->num_fields; i++)
   {
     if (field_info->info[i].field_type == PBM_FT_PHONE)
     {
       //increment by 2 to accomodate '+' sign & NULL char
       field_info->info[i].max_length = 2 + PBM_MAX_NUM_LENGTH;
     }
   }
   field_info = &g_sim_field_info[session_type][1]; /* FDN info*/
   for ( i = 0; i < field_info->num_fields; i++)
   {
     if (field_info->info[i].field_type == PBM_FT_PHONE)
     {
       //increment by 2 to accomodate '+' sign & NULL char
       field_info->info[i].max_length = 2 + PBM_MAX_NUM_LENGTH;
     }
   }
   pbm_buffers[buffer_index].extension = (byte) 0xff;
   gsdi_buffer[0] = (byte) EXT_REC_TYPE_FREE;
   pbm_buffers[buffer_index].record.file_id = ext_cache->file_id;
   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write ( session_type,
                                       ext_cache->file_id,
                                       ext_cache->file_type,
                                       extension_record,
                                       gsdi_buffer,
                                       EXT_KNOWN_REC_LEN,
                                       (uint32) buffer_index,
                                       pbm_mmgsdi_async_cb);
   PBM_MEM_FREEIF(gsdi_buffer);
   return pbm_retval;
}

/* ==========================================================================
FUNCTION PBM_UTIL_CONVERT_TO_REFRESH_FILE_PATHS

DESCRIPTION
  This function shall accept byte buffer that has list of file paths.
  Each file path begins from MF, hence byte value 0x 3F shall be used
  as a delimiter. The individual file paths shall be stored in the
  array of mmgsdi_path_type.
  Input buffer: 0x3F 0x00 0x2F 0x02 0x3F 0x00 0x7F 0x10 0x6F 0xD2
                 |
                First byte is number of files
  Should give output -
  *file_path_pptr[0].path_buf={0x3F 0x00 0x2F 0x02 0x00 0x00 0x00 0x00 0x00 0x00}
  *file_path_pptr[0].path_len = 2
  *file_path_pptr[1].path_buf={0x3F 0x00 0x7F 0x10 0x6F 0xD2 0x00 0x00 0x00 0x00}
  *file_path_pptr[1].path_len = 3
  num_files = 2

DEPENDENCIES
  NONE

LIMITATIONS


RETURN VALUE
  mmgsdi_return_enum_type
  PBM_SUCCESS          - Command processed,
  PBM_ERROR            - Other unknown failure

SIDE EFFECTS
  Calling function should free memory allocated to file_path_pptr
----------------------------------------------------------------------------*/
pbm_return_type  pbm_util_convert_to_refresh_file_paths(
  uint32                  data_len,
  const uint8            *data_ptr,
  mmgsdi_path_type       *file_path_pptr,
  uint8                  num_files
)
{
  uint8                   file_num                    = 0;
  byte                    index                       = 0;
  byte                    path_index                  = 0;
  word                    path[MMGSDI_MAX_PATH_LEN];

  /* Check for input params */
  PBM_CHECK_PTR2_RET(data_ptr, file_path_pptr,PBM_ERROR);

  if(data_len == 0)
  {
    UIM_MSG_ERR_1("Bad Input Params data_len=0x%x",data_len);
    return PBM_ERROR;
  }

  memset(path, 0x00,(sizeof(uint16)*(MMGSDI_MAX_PATH_LEN)));

  while (( index < data_len ) && (file_num < num_files))
  {
    path_index = 0;

    /* Parse for each file path */
    do
    {
      path[path_index] = data_ptr[index++] << 8;
      path[path_index] = path[path_index] | data_ptr[index++];
      UIM_MSG_HIGH_1("File path info - 0x%x", path[path_index]);
      path_index++;
    }while ((index < data_len) && (data_ptr[index] != 0x3F) && (path_index<ARR_SIZE(path)));

    /* Path index was incremented by 1 in the last iteration of do-while loop,
       hence we have path_len = path index here */
    file_path_pptr[file_num].path_len = path_index;

    /* Copy path buffer */
    memscpy(file_path_pptr[file_num].path_buf, sizeof(file_path_pptr[file_num].path_buf),
           path, (path_index) * sizeof(uint16));
    file_num++;
  } /* while (index < data_len ) */
  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_CREATE_ANR_RECORD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_anr_record(pbm_session_enum_type          session_type,
                                      pbm_file_type file_id,
                                      pbm_anr_record_s_type *anr_record,
                                      uint32                 buffer_index )
{
   byte                                            *gsdi_buffer=NULL;
   PB_USIM_CACHE                                   *usim_cache;
   pbm_async_gsdi_buffer                           *buffer = &pbm_buffers[buffer_index];
   uint16                                          index;
   mmgsdi_file_structure_enum_type                 filetype;
   pbm_return_type                                 pbm_retval;
   byte                                            *num_buffer;
   pbm_fixed_sim_rec_s_type                        *sim_rec;

   PBM_CHECK_PTR_RET(anr_record, PBM_ERROR);
   usim_cache = pbm_file_id_to_cache(session_type, file_id);
   PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

   PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
   memset(gsdi_buffer, 0xFF, (uint32) usim_cache->record_length);
   filetype = usim_cache->file_type;
   index = (uint16) buffer->record.index;
   sim_rec = (pbm_fixed_sim_rec_s_type *) &gsdi_buffer[1];
   if (anr_record->anr_data[1] != 0)
   {
      /* Updating the ANR record identifier with the following values
       * - '00' – no additional number description;
       * - 'xx' – record number in EFAAS describing the type of number (e.g. "FAX");
       * - 'FF' – free record.
       */
      gsdi_buffer[0] = anr_record->anr_data[0];
      num_buffer = &anr_record->anr_data[1];

      if (num_buffer[0] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
      {
         memscpy((byte *)&sim_rec->ton_npi, 1, (byte *)&num_buffer[1],1);
         memscpy((byte *)sim_rec->number, PBM_SIM_DIALING_NUMBER_LEN, (byte *)&num_buffer[2],
                PBM_SIM_DIALING_NUMBER_LEN);
         sim_rec->bcd_len = (PBM_SIM_DIALING_NUMBER_LEN + 1);
      } else
      {
         memscpy((byte *)sim_rec, num_buffer[0]+1, num_buffer, num_buffer[0]+1);
      }
   }

   sim_rec->ext_id = pbm_buffers[buffer_index].extension;
   if (usim_cache->mapping_type == PBM_MAPPING_TYPE_2 &&
       pbm_buffers[buffer_index].state == PBM_WRITE_CURR_USIM)
   {
      pbm_retval = pbm_fill_adn_sfi(anr_record->adn_record_id,
                           &gsdi_buffer[usim_cache->record_length-2],session_type);
      if (PBM_SUCCESS != pbm_retval)
      {
         UIM_MSG_ERR_1("Error %d in filling ADN SFI", pbm_retval);
         PBM_MEM_FREEIF(gsdi_buffer);
         return PBM_ERROR;
      }
   }

   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                     file_id,
                                     filetype,
                                     index,
                                     gsdi_buffer,
                                     usim_cache->record_length,
                                     buffer_index,
                                     pbm_mmgsdi_async_cb);

   PBM_MEM_FREEIF(gsdi_buffer);
   return pbm_retval;
}


/*===========================================================================
FUNCTION PBM_MMGSDI_REFRESH_COMPLETE_CB

DESCRIPTION
  This function is called in response to gdi_refresh_complete by GSDI

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} suppress could be const */
static void pbm_mmgsdi_refresh_complete_cb(mmgsdi_return_enum_type status,
                                           mmgsdi_cnf_enum_type    cnf,
                                           const mmgsdi_cnf_type  *cnf_ptr)
{
   /* Do Nothing */
   if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
   {
      UIM_MSG_ERR_2("Refresh Complete Ack failed %d %d",
                  status, cnf);
   }
}


/*==============================================================================

FUNCTION NAME pbm_refresh_file_change_notification_cb

DESCRIPTION
  Function that gets called by GSDI when a REFRESH Request is made and a
  determination must be made whether to proceed or not.

  Currently we refresh all SIM phonebooks irrespective of which files were
  changed. This should actually be based on the file list that GSDI sends us.

RETURN VALUE

  void

DEPENDENCIES
  None
==============================================================================*/
void pbm_refresh_file_change_notification_cb
(
mmgsdi_refresh_file_list_type  *refresh_file_list_p, /*list of changed files*/
mmgsdi_session_id_type mmgsdi_session_id
)
{
   boolean                globals_updated = FALSE;
   pbm_session_enum_type session_type;
   pbm_phonebook_type     pb_id = {PBM_GPB};
   uint32                    i, j;
   uint8                  local = 0;     /* Local vs Global phonebook */
   pbm_file_type  pbm_file_iterator;
   pbm_file_type pbm_file;
   mmgsdi_path_type       *l_file_path_ptr=NULL;
   uint8 l_num_path_files = 0;
   pbm_refresh_file_list_type pbm_refresh_file_list;
   memset(&pbm_refresh_file_list,0,sizeof(pbm_refresh_file_list));

   PBM_CHECK_PTR_RET(refresh_file_list_p, VOID);
   session_type = pbm_mmgsdi_session_to_pbm_session(mmgsdi_session_id);
   for (i = 0; i < (refresh_file_list_p->file_path_len); i+=2)
   {
     if ((refresh_file_list_p->file_path_ptr[i] == 0x3F) &&
         (refresh_file_list_p->file_path_ptr[i+1] == 0x00))
     {
       l_num_path_files++;
     }
   }
   /* allocate memory to file_path_array */
   PBM_MEM_ALLOC(l_file_path_ptr,l_num_path_files * sizeof(mmgsdi_path_type));
   PBM_CHECK_PTR_RET(l_file_path_ptr,VOID);
   (void)pbm_util_convert_to_refresh_file_paths(refresh_file_list_p->file_path_len,
                                          refresh_file_list_p->file_path_ptr,
                                          l_file_path_ptr,
                                          l_num_path_files);

   PBM_MEM_ALLOC(pbm_refresh_file_list.file_list_ptr,
                (sizeof(pbm_file_type) * (l_num_path_files + refresh_file_list_p->num_files)));
   /* why we have l_num_path_files + refresh_file_list_p->num_files, can there be path and enum notifications from GSDI
   if true then why do we have a below mentioned for loop for l_num_path_files only */
   if(pbm_refresh_file_list.file_list_ptr == NULL)
   {
     PBM_MEM_FREEIF(l_file_path_ptr);   
     return;
   }
   
   for(i=0;i<l_num_path_files;i++)
   {
     if(PBM_SUCCESS == pbm_convert_path_to_file_enum(
                                            session_type,
                                            l_file_path_ptr[i],
                                            &pbm_file_iterator))
     {
       pbm_refresh_file_list.file_list_ptr[refresh_file_list_p->num_files] = pbm_file_iterator;
       refresh_file_list_p->num_files++;
     }
   }
   pbm_refresh_file_list.num_files = refresh_file_list_p->num_files;
   /* since pbm_refresh_file_list_p would be used in updating refresh globals below*/
   /* Populate Static Globals for PBM */
   globals_updated = pbm_update_refresh_globals(&pbm_refresh_file_list,
                                                session_type);
   if ( globals_updated == TRUE)
   {
      local = pbm_util_return_fileset_for_session(session_type);

      /* Clear out the PBM Caches affected */
      pbm_clear_refresh_caches(session_type);
      if (pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type))
      {
         /* Session is already in progress and the session init done will be sent to clients eventually 
            So no need of sending per phonebook refresh to client now*/
         UIM_MSG_HIGH_1("Already session init - No need for refresh event to clients now %d",session_type);
         PBM_SESSION_SET(pbm_session_restart, session_type);
         PBM_MEM_FREEIF(pbm_refresh_file_list.file_list_ptr);
         PBM_MEM_FREEIF(l_file_path_ptr);
         return;
     }
     else
     {
        UIM_MSG_HIGH_1("Set session init and continuing for FCN refresh processing %d",session_type);
        PBM_SESSION_SET(pbm_session_initializing, session_type);
     }
	   
      /* Send notification about refresh start for each phonebook that is
       * changed */
      /* For each SESSION phonebook file */
      for (i = 0; i < (uint32)ARR_SIZE(pbm_file_to_uim_file); i++)
      {
         pbm_file_iterator = pbm_file_to_uim_file[i].pbm_filename;

         /* Check if the phonebook was changed */
         for (j = 0; j < refresh_file_list_p->num_files; j++)
         {
           /*Check both the local and global versions*/
           pbm_file = pbm_file_to_uim_file[i].pbm_session_filename[local];
           /* since pbm and uim files name are same now, this check would double sure that if files is not valid for some session
           and still refresh has come, pb refresh should not be sent */
           if (pbm_file == pbm_refresh_file_list.file_list_ptr[j])
           {
              pb_id = pbm_file_id_to_pb_id(session_type, pbm_file_iterator);
              if (pb_id.device_type != PBM_DEFAULT)
              {
                pbm_session_event_data_u_type evt_data = {0};
                evt_data.pb_id = pb_id;
                pbm_add_notify(PBM_EVENT_PB_REFRESH_START, evt_data);
              }
           }
         }
      }

     PBM_MEM_FREEIF(pbm_refresh_file_list.file_list_ptr);
     PBM_MEM_FREEIF(l_file_path_ptr);

      /* Reinitialize the PBM Cache */
      pbm_init_fileinfo_internal(session_type);
   }
} /* pbm_refresh_file_change_notification_cb */


/*===========================================================================
FUNCTION pbm_mmgsdi_refresh_cb()

DESCRIPTION
  register for refresh mmgsdi cb function.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pbm_mmgsdi_refresh_cb(mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    cnf,
                                  const mmgsdi_cnf_type  *cnf_ptr)
{
   /* DO Nothing */
   if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
   {
      UIM_MSG_ERR_2("Refresh Complete Ack failed %d %d",
                  cnf, status);
   }
} /* pbm_mmgsdi_refresh_cb */


/*===========================================================================
FUNCTION pbm_mmgsdi_ok_to_refresh_cb()

DESCRIPTION
  Init Complete mmgsdi cb function.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pbm_mmgsdi_ok_to_refresh_cb(mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf,
                                        const mmgsdi_cnf_type  *cnf_ptr)
{
   /* DO Nothing */
   if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
   {
      UIM_MSG_ERR_2("Ok to refresh Ack failed %d %d",
                  cnf, status);
   }
} /* pbm_mmgsdi_ok_to_refresh_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void pbm_mmgsdi_card_status_cb(
                                     const mmgsdi_event_data_type   *mmgsdi_event
                                     )
{
   pbm_mmgsdi_card_status_s_type *card_status;

   PBM_CHECK_PTR_RET(mmgsdi_event, VOID);

   card_status = (pbm_mmgsdi_card_status_s_type *)
                 pbm_cmd_alloc(sizeof(pbm_mmgsdi_card_status_s_type));
   PBM_CHECK_PTR_RET(card_status, VOID);

   card_status->cmd_type = PBM_MMGSDI_CARD_STATUS_CB;

   card_status->mmgsdi_event = *mmgsdi_event;

   if (mmgsdi_event->evt == MMGSDI_REFRESH_EVT)
   {
      //Init this in case number of files is zero
      card_status->mmgsdi_event.data.refresh.refresh_files.file_list_ptr = NULL;

      if ((mmgsdi_event->data.refresh.refresh_files.file_path_len > 0) &&
          (mmgsdi_event->data.refresh.refresh_files.file_path_ptr != NULL))
      {
        uint32 list_size = mmgsdi_event->data.refresh.refresh_files.file_path_len;

        //Need to copy FCN refresh path list seperately.
        //Need to copy only paths, since we always get only paths
        PBM_MEM_ALLOC(card_status->mmgsdi_event.data.refresh.refresh_files.file_path_ptr,
                      list_size);
        PBM_CHECK_PTR_RET(card_status->mmgsdi_event.data.refresh.refresh_files.file_path_ptr, VOID);
        memscpy(card_status->mmgsdi_event.data.refresh.refresh_files.file_path_ptr, list_size,
               mmgsdi_event->data.refresh.refresh_files.file_path_ptr,list_size);
      }
   }
   if (mmgsdi_event->evt == MMGSDI_FDN_EVT)
   {
     if(mmgsdi_event->data.fdn.rec_num!= 0)
     {
       card_status->mmgsdi_event.data.fdn.rec_data.data_ptr = NULL;

       if (mmgsdi_event->data.fdn.rec_data.data_len> 0)
       {
          uint32 rec_len = mmgsdi_event->data.fdn.rec_data.data_len;
          PBM_MEM_ALLOC(card_status->mmgsdi_event.data.fdn.rec_data.data_ptr,rec_len);
          PBM_CHECK_PTR_RET(card_status->mmgsdi_event.data.fdn.rec_data.data_ptr, VOID);
          memscpy(card_status->mmgsdi_event.data.fdn.rec_data.data_ptr, rec_len,
                 mmgsdi_event->data.fdn.rec_data.data_ptr,rec_len);
       }
     }
   }
   if (mmgsdi_event->evt == MMGSDI_ECC_EVT)
   {
     if(mmgsdi_event->data.ecc.ecc_count != 0)
     {
       uint16 i,j;
       mmgsdi_ecc_evt_info_type *temp_src_ecc_evt_info = (mmgsdi_ecc_evt_info_type *)&mmgsdi_event->data.ecc;
       card_status->mmgsdi_event.data.ecc.ecc_data = NULL;

       PBM_MEM_ALLOC(card_status->mmgsdi_event.data.ecc.ecc_data,
                       sizeof(mmgsdi_ecc_data_type) *
					             mmgsdi_event->data.ecc.ecc_count);
       PBM_CHECK_PTR_RET(card_status->mmgsdi_event.data.ecc.ecc_data,VOID);
       for(i = 0; i<temp_src_ecc_evt_info->ecc_count; i++)
       {
         PBM_MEM_ALLOC(card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_ptr,
                         temp_src_ecc_evt_info->ecc_data[i].data.data_len);
         if(card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_ptr == NULL)
         {
           for(j = 0; j <= i; j++)
           {
             PBM_MEM_FREEIF(card_status->mmgsdi_event.data.ecc.ecc_data[j].data.data_ptr);
           }
           PBM_MEM_FREEIF(card_status->mmgsdi_event.data.ecc.ecc_data);
           return;
         }
         card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_len =
                             temp_src_ecc_evt_info->ecc_data[i].data.data_len;
         memscpy(card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_ptr,
		 	     temp_src_ecc_evt_info->ecc_data[i].data.data_len,
                 temp_src_ecc_evt_info->ecc_data[i].data.data_ptr,
                 temp_src_ecc_evt_info->ecc_data[i].data.data_len);
       }
     }
   }
   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(card_status));
}


/*===========================================================================
FUNCTION pbm_handle_app_init_completed

DESCRIPTION
  Function called after getting SUBSCRIPTION READY event from MMGSDI, EVT is
  received for provisioning 1X, GW apps for which we'll do a cache build
  here check slot id for successful built session n prepare GPB cache for that
  if support is available

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_handle_app_init_completed(mmgsdi_session_id_type mmgsdi_session_id,
                                     mmgsdi_slot_id_enum_type mmgsdi_slot,
                                     mmgsdi_app_enum_type app_type)
{
   pbm_session_enum_type    pbm_session_type;
   pbm_slot_type            slot_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_slot);
   int                      q_id = 0;

   /* Convert mmgsdi session to pbm session index */
   pbm_session_type = pbm_mmgsdi_session_to_pbm_session(mmgsdi_session_id);
   UIM_MSG_HIGH_4("pbm_handle_app_init_completed session 0x%x restart 0x%x activated 0x%x session_init 0x%x",
              pbm_session_type, pbm_session_restart, pbm_session_activated, pbm_session_initializing);

   if (pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(pbm_session_type))
   {
     PBM_SESSION_SET(pbm_session_restart, pbm_session_type);
     return;
   }

   /* Queuing the session to serialize the phonebook initialization  */
   pbm_queue_algorithm(pbm_session_type);

   /* Start cache initialization needs to be done for possible phonebooks
    * GPB + LPB. Check conditions for app_type first -
    * if RUIM/ SIM - LPB absent + prepare default GPB (slot)
    * if USIM/ CSIM - check for LPB support + check and prepare GPB 
    */
   if ( FALSE == pbm_session_info[pbm_session_type].isActive )
   {
     /* LPB would be present for this . phonebooks like ECC would be read */
     pbm_session_info[pbm_session_type].isActive = TRUE;
     rex_enter_crit_sect(&pbm_crit_sect); /* for app_type and in_use_adn_pb */
     pbm_session_info[pbm_session_type].app_type = app_type;
     if (USE_GSM_SUPPORT(pbm_session_type) || (USE_CDMA_SUPPORT(pbm_session_type)))
     {
       /* LPB would be present for this .. phonebooks like ECC would be read */
       pbm_session_info[pbm_session_type].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
     }
     rex_leave_crit_sect(&pbm_crit_sect);
   }

   /* get session_index for GPB for same (slot) first */
   pbm_session_type = pbm_pb_type_and_slot_to_session_type(slot_id, PBM_GPB);
   if (pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(pbm_session_type))
   {
     UIM_MSG_HIGH_1("PBM already session init %d",pbm_session_type);
     PBM_SESSION_SET(pbm_session_restart, pbm_session_type);
     return;
   }

   /* avoid duplicacy of cache building when prov apps are selected on
    * same slot - you'll be here twice in that case 
    */
   if ( FALSE == pbm_session_info[pbm_session_type].isActive )
   {
     /* set data for the GPB fields
      * app type would be unknown for CARD_SLOT session 
      */
     pbm_session_info[pbm_session_type].isActive = TRUE;
     /* if the corresponding prov app type is 2G 
      * then we don't need to consider the PBR file
      */
     if(!(USE_USIM_SUPPORT(pbm_session_type)||USE_CSIM_SUPPORT(pbm_session_type)))
     {
       /* sine this is an atomic update, critical section protection not required */
       pbm_session_info[pbm_session_type].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
     }
   }

   q_id = pbm_session_info[pbm_session_type].slot_id - 1 ;
   if( q_id >= 0 && q_id < PBM_MAX_SLOTS )
   {
      UIM_MSG_HIGH_3("PBM pbm_handle_app_init_completed q_id %d pbm_session_type %d curr_q[q_id] %d",
              q_id,pbm_session_type,curr_q[q_id]);
     if( curr_q[q_id] == PBM_SESSION_MAX )
     {
       curr_q[q_id] = pbm_q_dequeue(q_id);
       pbm_session_phonebook_init( curr_q[q_id] );
     }
   }
}/* pbm_handle_app_init_completed */

/*===========================================================================
FUNCTION PBM_OPEN_SESSIONS

DESCRIPTION
  Function to init SIM variables, and do processing that can be done
  without access restrictions (eg. ECC).

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_open_sessions ()
{
   mmgsdi_return_enum_type result;
   uint32        session_type_mask = (MMGSDI_GW_PROV_PRI_SESSION_MASK | \
                                               MMGSDI_1X_PROV_PRI_SESSION_MASK | \
                                               MMGSDI_CARD_SESSION_SLOT_1_MASK );
#ifdef FEATURE_DUAL_SIM
           session_type_mask |= (MMGSDI_GW_PROV_SEC_SESSION_MASK | \
                                               MMGSDI_1X_PROV_SEC_SESSION_MASK | \
                                               MMGSDI_CARD_SESSION_SLOT_2_MASK );
#ifdef FEATURE_TRIPLE_SIM
           session_type_mask |= (MMGSDI_GW_PROV_TER_SESSION_MASK | \
                                               MMGSDI_1X_PROV_TER_SESSION_MASK | \
                                               MMGSDI_CARD_SESSION_SLOT_3_MASK);
#endif
#endif

   result = mmgsdi_session_open_ext(pbm_mmgsdi_client_id,
                                session_type_mask,
                                   pbm_mmgsdi_card_status_cb,
                                   FALSE,
                                   pbm_mmgsdi_open_session_cb,
                                   (mmgsdi_client_data_type)session_type_mask);

   if ( MMGSDI_SUCCESS != result )
   {
         /* log that error and return */
      UIM_MSG_ERR_1("PBM session open failed with error %d",result);
   }

}


/*===========================================================================
FUNCTION pbm_build_slot_specific_ecc_cache

DESCRIPTION
Builds the records in ecc_sim_cache as per the data configured in NV 73755

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_build_slot_specific_ecc_cache(
  pbm_session_enum_type        session_type,
  pbm_ecc_mode_type            emergency_mode,
  uint8                        nv_ecc_per_sub
)
{
  uint8                 j            = 0;
  uint16                i            = 0;
  PB_CACHE             *cache        = NULL;
  pbm_return_type       pbm_ret      = PBM_SUCCESS;
  pbm_slot_type         slot_id      = PBM_MAX_SLOTS;
  uint8                 slot_index   = 0;
  boolean               found_in_sim = FALSE;

  PBM_SESSION_SET(ecc_has_entries, session_type);
  if(emergency_mode < EMERGENCY_GW || emergency_mode > EMERGENCY_GW_1X)
  {
    UIM_MSG_ERR_1("Incorrect emergency_mode 0x%x", emergency_mode);
    return;
  }

  PBM_VALIDATE_SESSION_RET(session_type, VOID);
  slot_id = pbm_session_info[session_type].slot_id;
  if(pbm_util_get_slot_index(slot_id, &slot_index) != PBM_SUCCESS)
  {
    return;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  cache = (PB_CACHE *) pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
  if (!cache)
  {
    UIM_MSG_ERR_1("No Cache found for session 0x%x, PBM_FILE_ECC", session_type);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  for(j = 0; j < nv_ecc_per_sub; j++)
  {
    found_in_sim = FALSE;
    if(pbm_slot_specific_ecc_list_ptr[slot_index][j] == NULL)
    {
      continue;
    }

    for (i = 1; i <= cache->num_of_records; i++)
    {
      if (cache->pb_cache_array != NULL && cache->pb_cache_array[i])
      {
        if (match_found(PBM_CAT_NONE, PBM_FIELD_SIM_ECC, NULL, 0, 0,
                        cache->pb_cache_array[i]))
        {
          /* Avoid adding from NV ECC per sub if same ECC number
             found SIM ECC cache.*/
          if ((pbm_slot_specific_ecc_list_ptr[slot_index][j]->string_length > 0) &&
               (match_found(PBM_CAT_NONE, PBM_FIELD_SIM_ECC,
                            pbm_slot_specific_ecc_list_ptr[slot_index][j]->dial_string,
                            (uint16) pbm_slot_specific_ecc_list_ptr[slot_index][j]->string_length,
                           PBM_SEARCHTYPE_EXACT,
                            cache->pb_cache_array[i])))
          {
            found_in_sim = TRUE;
            break;
          }
        }
      }
    }

    /* Avoid adding from NV ECC per sub if same ECC number
       found SIM ECC cache.*/
    if(found_in_sim)
    {
      continue;
    }

    /* Adding PBM_FIELD_SIM_ECC EFS records to SIM cache */
    pbm_ret = pbm_build_ecc_record(PBM_FIELD_SIM_ECC,
                                   pbm_slot_specific_ecc_list_ptr[slot_index][j]->string_length,
                                   pbm_slot_specific_ecc_list_ptr[slot_index][j]->dial_string,
                                   0, NULL,
                                   ((pbm_slot_specific_ecc_list_ptr[slot_index][j]->service_cat_val) ? TRUE : FALSE),
                                   pbm_slot_specific_ecc_list_ptr[slot_index][j]->service_cat_val,
                                   session_type,
                                   emergency_mode,
                                   pbm_slot_specific_ecc_list_ptr[slot_index][j]->srv_status_flag);

    if (PBM_SUCCESS != pbm_ret)
    {
      UIM_MSG_ERR_1("Error 0x%x in building NV ECC per sub record.", pbm_ret);
    }
  }
  cache->status = PBM_STATUS_READY;

  rex_leave_crit_sect(&pbm_crit_sect);
}/* pbm_build_slot_specific_ecc_cache */


/*===========================================================================
FUNCTION pbm_update_sim_ecc_cache

DESCRIPTION
Builds the records in ecc_sim_cache as per the data received in MMGSDI_ECC_EVT 

In case of USIM and SIM cards , copies the OTA numbers pointers to the newly
allocated cache array pointer so that the numbers wont be lost


RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void pbm_update_sim_ecc_cache(
  pbm_session_enum_type        session_type,
  mmgsdi_ecc_evt_info_type    *ecc_evt_info
)
{
  PB_CACHE                         *cache                                      = NULL;
  pbm_field_id_e_type               field_id                                   = PBM_FIELD_NONE;
  int                               data_length                                = 0;   /* Length of ASCII version of ECC # */
  uint32                            name_length                                = 0;
  uint16                            ascii_emergency_alpha[PBM_TEXT_SIZE_CHARS] = {0}; /* Emergency number Alpha Identifier. */
  byte                              emergency_category                         = 0;   /* Emergency service category octet. */
  int                               ecc_offset                                 = 0;          /* Where in the ecc_list are we */
  int                               i                                          = 0;
  uint8                             j                                          = 0;
  pbm_return_type                   pbm_ret                                    = PBM_SUCCESS;
  pbm_ecc_mode_type                 emergency_mode                             = NOT_EMERGENCY;
  uint16                            number_of_records                          = 0;
  int                               record_length                              = 0;
  mmgsdi_file_structure_enum_type   file_type                                  = MMGSDI_LINEAR_FIXED_FILE;
  /* + and null for string */
  char                              number[PBM_NUM_SIZE]                       = {0};
  uint8                             nv_ecc_per_sub                             = PBM_NV_EMERGENCY_NUMBERS_PER_SUB;

  rex_enter_crit_sect(&pbm_crit_sect);

  cache = (PB_CACHE *) pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
  if (!cache)
  {
    UIM_MSG_ERR_1("No Cache found for session 0x%x", session_type);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  pbm_util_get_emergency_mode(session_type, &emergency_mode);

  /* clear previous SIM ecc no's if any */
  pbm_ecc_clear_all_id(session_type, PBM_FIELD_SIM_ECC);

  if( ecc_evt_info != NULL &&
      ecc_evt_info->ecc_count != 0 && ecc_evt_info->ecc_data != NULL )
  {
    /* for any GSM sim no of ecc recs will be always 1
        * and max len would be GSDI_MAX_ECC_LEN */
    if (USE_USIM_SUPPORT(session_type))
    {
      /* QMI-PBM interface supports MAX of 255 records. Hence we are limiting
           number of records to be included in card emergency TLV */
      if((ecc_evt_info->ecc_count) + PBM_NV_EMERGENCY_NUMBERS_PER_SUB >= 255)
      {
        nv_ecc_per_sub = 255 - (ecc_evt_info->ecc_count) -1;
      }

      record_length = PBM_MAX_NUM_LENGTH;
      number_of_records = ecc_evt_info->ecc_count + CM_MAX_EMERGENCY_NUM_COUNT + nv_ecc_per_sub;
      file_type = MMGSDI_LINEAR_FIXED_FILE;
    }
    else
    {
      record_length     = PBM_MAX_ECC_LEN;
      file_type         = MMGSDI_TRANSPARENT_FILE;

      /* each non-USIM ecc number is of maximum of 3 bytes encoded in BCD on SIM card */
      number_of_records = (ecc_evt_info->ecc_data->data.data_len/3);

      /* QMI-PBM interface supports MAX of 255 records. Hence we are limiting
          number of records to be included in card emergency TLV */
      if(number_of_records + PBM_NV_EMERGENCY_NUMBERS_PER_SUB >= 255)
      {
        nv_ecc_per_sub = 255 - number_of_records -1;
      }
      number_of_records += nv_ecc_per_sub;

      if(USE_GSM_SUPPORT(session_type))
      {
        number_of_records += CM_MAX_EMERGENCY_NUM_COUNT;
      }
    }

    if((USE_USIM_SUPPORT(session_type) || USE_GSM_SUPPORT(session_type)) && 
        cache->num_of_records < number_of_records)
    {
      /* This means the current EF ECC records are more than the previous
          instance , Hence we need to increase the sizeof cache->pb_cache_array */
      pbm_addr_cache_s_type **pb_cache_array_temp = NULL;

      PBM_MEM_ALLOC(pb_cache_array_temp, ((uint32) number_of_records + 1) *
                                  sizeof(pbm_addr_cache_s_type *));
      if( NULL == pb_cache_array_temp ) 
      {
        rex_leave_crit_sect(&pbm_crit_sect);
        return ; 
      }
      memset(pb_cache_array_temp,0,
             ((uint32) number_of_records + 1) *
                                  sizeof(pbm_addr_cache_s_type *));
      memscpy(pb_cache_array_temp, ((uint32) number_of_records + 1) *
            sizeof(pbm_addr_cache_s_type *), cache->pb_cache_array, ((uint32) cache->num_of_records + 1) *
                       sizeof(pbm_addr_cache_s_type *));

      PBM_MEM_FREEIF(cache->pb_cache_array);
      cache->pb_cache_array = pb_cache_array_temp;
    }
    else if (  USE_CDMA_SUPPORT(session_type) || USE_CSIM_SUPPORT(session_type) ) 
    {
      PBM_MEM_FREEIF(cache->pb_cache_array);

      /*CSIM and RUIM ECC SIM cache is not allocated till now */
      PBM_MEM_ALLOC(cache->pb_cache_array,
                    ((uint32) number_of_records + 1) * sizeof(pbm_addr_cache_s_type *));
      if( NULL == cache->pb_cache_array ) 
      {
        rex_leave_crit_sect(&pbm_crit_sect);
        return ; 
      }
    }

    pbm_fill_pb_cache_info(ecc_sim_cache,
                           session_type,
                           PBM_USIM_PB_ONE,
                           record_length,
                           PBM_ECC_MIN_LEN,
                           PBM_FILE_ECC,
                           file_type,
                           FALSE,
                           number_of_records,
                           NULL);

    field_id = pbm_file_id_to_field_id(PBM_FILE_ECC);

    for(i = 0; i<ecc_evt_info->ecc_count; i++)
    {
      mmgsdi_data_type    *temp_ecc_data = &ecc_evt_info->ecc_data[i].data;

      if(temp_ecc_data == NULL || temp_ecc_data->data_ptr == NULL)
      {
        continue;
      }

      /* we would need different processing for an USIM and the rest
          since in USIM EC-ECC will be linear fixed and in normal SIM 
           it will be a Transparent file */
      if (USE_USIM_SUPPORT(session_type))
      {
        data_length = pbm_ecc_bcd_to_ascii(temp_ecc_data->data_ptr,
                                           number);
        if (data_length <= 0)
        {
         UIM_MSG_ERR_1("SESSION ECC record 0x%x had 0 length", ecc_offset);
        }

        memset(ascii_emergency_alpha, 0, sizeof(ascii_emergency_alpha));

        /* since the last byte denotes Service Category we should not consider that */
        name_length = pbmconvert_sim_to_string(temp_ecc_data->data_ptr + 3,
                                               (temp_ecc_data->data_len - 4),
                                                ascii_emergency_alpha,
                                                ARR_SIZE(ascii_emergency_alpha));
        if (name_length > PBM_TEXT_SIZE_CHARS) /*  KW error */
        {
          name_length=PBM_TEXT_SIZE_CHARS;
        }

        /* The last octet is the Service Category */
        emergency_category = temp_ecc_data->data_ptr[temp_ecc_data->data_len - 1];

        if (PBM_SUCCESS != pbm_build_ecc_record(field_id,
                                                (data_length>0?(uint32)data_length + 1:0),
                                                number,
                                                sizeof(uint16) * (name_length?name_length + 1:0),
                                                ascii_emergency_alpha,
                                                TRUE,
                                                emergency_category,
                                                session_type,
                                                emergency_mode, 0))
        {
          UIM_MSG_ERR_0("Could not build PBM record");
        }
      }
      else
      {
        /* we need the below special processing since GSM and RUIM EF-ECCs are transparent files,
              and all the ecc codes are appended together in one record
              there exists only one EF ECC record in this case */
        for (j=0; j<cache->num_of_records; j++)
        {
          if (j < (temp_ecc_data->data_len/3))
          {
            data_length = pbm_ecc_bcd_to_ascii((temp_ecc_data->data_ptr + 3*j), number);
            if (data_length <= 0)
            {
              UIM_MSG_ERR_1("SESSION ECC record 0x%x had 0 length", ecc_offset);
            }
            if ((pbm_ret = pbm_build_ecc_record(field_id,
                                                data_length>0?(uint32)data_length + 1:0,
                                                number,0,
                                                NULL, FALSE, 0,
                                                session_type, emergency_mode, 0)) !=PBM_SUCCESS)
            {
              UIM_MSG_ERR_0("Could not build PBM record.");
            }
          }//if(i < (data_ptr->read_cnf.read_data.data_len/3))
        }
      }
      PBM_MEM_FREEIF(ecc_evt_info->ecc_data[i].data.data_ptr);
    }
    PBM_MEM_FREEIF(ecc_evt_info->ecc_data);
  }
  rex_leave_crit_sect(&pbm_crit_sect);

  /* Appending NV slot based numbers as well */
  pbm_build_slot_specific_ecc_cache(session_type, emergency_mode, nv_ecc_per_sub);
  pbm_ecc_set_hardcoded_eccs(TRUE);
  PBM_SET_PB_CACHE_READY(cache);
} /* pbm_update_sim_ecc_cache */


/*===========================================================================
FUNCTION       pbm_find_provioning_sesssion_in_queue

DESCRIPTION
This function handles session deactivation event from MMGSDI.

RETURN VALUE
  pbm_session_enum_type

DEPENDENCIES

===========================================================================*/
pbm_session_enum_type pbm_find_provioning_sesssion_in_queue (pbm_session_enum_type deactivate_session)
{
  uint16 loop_session,slot_id;
  
  slot_id = pbm_session_info[deactivate_session].slot_id;
  for(loop_session = 0;loop_session< PBM_SESSION_MAX;loop_session++)
  {
    if((pbm_session_type_to_pb_category(loop_session) == PBM_LPB) && 
       (slot_id == pbm_session_info[loop_session].slot_id) && 
        (loop_session != deactivate_session))
    {
      UIM_MSG_HIGH_3("PBM match_found for deactivate_session %d slot_id %d deact_session_complement %d",
                      deactivate_session,slot_id,loop_session);
      break;
    }
  }

  return (pbm_session_enum_type)loop_session;
}


/*===========================================================================
FUNCTION pbm_session_deactivate_handling

DESCRIPTION
This function handles session deactivation event from MMGSDI.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_session_deactivate_handling(
  pbm_session_enum_type deact_session_type,
  uint16                slot_id
)
{
  pbm_session_enum_type  deact_session_complement = PBM_SESSION_DEFAULT;
  pbm_session_enum_type  card_session             = PBM_SESSION_DEFAULT;
  int                    q_id                     = 0;
  PB_CACHE               cache;

  memset(&cache,0,sizeof(PB_CACHE));

  q_id = slot_id - 1;
  if(q_id < 0 || q_id >= PBM_MAX_SLOTS)
  {
    UIM_MSG_ERR_1("session_deactivate_handling invalid q_id 0x%x", q_id);
    return;
  }
 
  /* get session_index for GPB for same (slot) first */
  card_session = pbm_pb_type_and_slot_to_session_type(
                                          slot_id,
                                          PBM_GPB);

  UIM_MSG_HIGH_3("session_deactivate_handling curr_q[q_id] 0x%x card_session 0x%x q_id 0x%x",
                 curr_q[q_id], card_session, q_id);
  
  pbm_clear_and_set_session_caches(deact_session_type,PBM_STATUS_NOT_INIT,FALSE);

  cache.pb_id.slot_id     = slot_id;
  cache.pb_id.prov_type   = pbm_session_type_to_provision_type(deact_session_type);
  cache.pb_id.pb_category = pbm_session_type_to_pb_category(deact_session_type);
  cache.pb_id.device_type = PBM_DEVICE_TYPE_ALL;

  /* Notifying cache status to QMI PBM to clear the global data */
  pbm_notify_pb_cache_status_update(&cache);
  pbm_stop_fileinfo_internal(deact_session_type);
  
  if(curr_q[q_id] == deact_session_type) 
  {
    curr_q[q_id] = PBM_SESSION_MAX;
  }

  if(pbm_if_queue_has_sess(deact_session_type,q_id))
  {
    UIM_MSG_HIGH_1("session_deactivate_handling removing session from queue deact_session_type 0x%x",
                   deact_session_type);

    /* Remove deactivate_session from queue */
    pbm_remove_deactivate_sess_from_queue(deact_session_type,q_id);
  }

  deact_session_complement = pbm_find_provioning_sesssion_in_queue(deact_session_type);

  UIM_MSG_HIGH_3("session_deactivate_handling deact_session_complement 0x%x init status 0x%x card_session 0x%x",
                 deact_session_complement,
                 (deact_session_complement == PBM_SESSION_MAX)?0:pbm_init_info.pbm_session_init_done[deact_session_complement],
                 card_session);

  /* Initlize ECC NV per card session only if no provisiong is activated in a slot */
  /* Skip ECC ready indication if an card error received before session de-activation */
  if(deact_session_complement == PBM_SESSION_MAX &&
     pbm_card_status[slot_id - 1])
  {
    pbm_build_slot_specific_ecc_cache(card_session, EMERGENCY_GW_1X, 
                                      PBM_NV_EMERGENCY_NUMBERS_PER_SUB);
  }
  
  /* Clearing slot info as well */
  pbm_session_info[deact_session_type].slot_id  = 0 ;
  if(deact_session_complement == PBM_SESSION_MAX ||
     !(pbm_if_queue_has_sess(deact_session_complement,q_id) || 
       curr_q[q_id] == deact_session_complement || 
      (pbm_init_info.pbm_session_init_done[deact_session_complement] == TRUE)))
  {
    pbm_clear_and_set_session_caches(card_session,PBM_STATUS_NOT_INIT,FALSE);

    cache.pb_id.slot_id     = slot_id;
    cache.pb_id.prov_type   = pbm_session_type_to_provision_type(card_session);
    cache.pb_id.pb_category = pbm_session_type_to_pb_category(card_session);
    cache.pb_id.device_type = PBM_DEVICE_TYPE_ALL;

    /* Notifying cache status to QMI PBM to clear the global data */
    pbm_notify_pb_cache_status_update(&cache);
    pbm_stop_fileinfo_internal(card_session);

    /* Remove card_session from queue */
    pbm_remove_deactivate_sess_from_queue(card_session,q_id);
    if(curr_q[q_id] == card_session)
    {
      curr_q[q_id] = PBM_SESSION_MAX;
    }
  }
  else
  {
    if(pbm_is_queue_empty(q_id))
    {
      pbm_session_enum_type temp_session = curr_q[q_id];

      UIM_MSG_HIGH_1("PBM session_deactivate_handling queue empty :temp_session_type 0x%x", temp_session);
      pbm_session_q_init(slot_id);
      curr_q[q_id] = temp_session;
      return;
    }
    else
    {
      if(curr_q[q_id] == deact_session_complement)
      {
        UIM_MSG_HIGH_2("PBM curr_session 0x%x deact_session_complement 0x%x",
                       curr_q[q_id],deact_session_complement);
      }
      else
      {
        curr_q[q_id] = pbm_q_dequeue(q_id);

        UIM_MSG_HIGH_2("PBM session deactivate handling q_id 0x%x curr_q[q_id] 0x%x",
                        q_id, curr_q[q_id]);
        if( curr_q[q_id] != PBM_SESSION_MAX )
        {
          if(pbm_session_type_to_pb_category(curr_q[q_id]) == PBM_GPB && 
             !pbm_session_info[curr_q[q_id]].isActive)
          {
            /* Setting back the isActive flag to TRUE If the dequeued session is a card session and
             * if it is not set already in case of multiapp card in single slot 
             */
            pbm_session_info[curr_q[q_id]].isActive = TRUE;
          }
          pbm_session_phonebook_init( curr_q[q_id] );
        }
      }
    }
  }
}/* pbm_session_deactivate_handling */


/*===========================================================================
FUNCTION PBM_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.
  Note:  MMGSDI returns the most recent event at registration.  So, our first
  event may be MMGSDI_CARD_INSERTED_EVT, or it may be MMGSDI_CARD_INIT_COMPLETED_EVT

RETURN VALUE
  void.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the PBM task.
===========================================================================*/
void pbm_card_status_cb(
  mmgsdi_event_data_type   mmgsdi_event
)
{
  pbm_slot_type                   slot         = 0;
  mmgsdi_return_enum_type         ret          = MMGSDI_SUCCESS;
  int                             index        = 0;
  pbm_session_enum_type           session_type = PBM_SESSION_DEFAULT;
  boolean                         new_init     = FALSE;

  session_type = pbm_mmgsdi_session_to_pbm_session(mmgsdi_event.session_id);
      
  if(MMGSDI_CARD_INSERTED_EVT == mmgsdi_event.evt ||
     MMGSDI_SESSION_CHANGED_EVT == mmgsdi_event.evt ||
     MMGSDI_SUBSCRIPTION_READY_EVT == mmgsdi_event.evt)
  {
    new_init = TRUE;
  }

  //except for MMGSDI_REFRESH_NAA_FCN
  //all refresh modes will have MMGSDI_SUBSCRIPTION_READY_EVT followed   
  if(MMGSDI_REFRESH_EVT == mmgsdi_event.evt &&
     (mmgsdi_event.data.refresh.mode != MMGSDI_REFRESH_NAA_FCN && 
      mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_START))
  {
    new_init = TRUE;
  }

  if(new_init == TRUE)
  {
    PBM_SESSION_RESET(pbm_session_activated, session_type);
    PBM_SESSION_RESET(pbm_wms_init_finished,session_type);
  }

  switch (mmgsdi_event.evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
    {
      pbm_session_enum_type card_sess = PBM_SESSION_DEFAULT ;

      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_inserted.slot);
      UIM_MSG_HIGH_1("PBM received MMGSDI_CARD_INSERTED_EVT for slot 0x%x", slot);
      if(slot <= 0 || slot > PBM_MAX_SLOTS)
      {
        break;
      }

      pbm_card_status[slot - 1] = TRUE;

      /* Initilizing the queue globals per slot */
      pbm_session_q_init(slot);
      card_sess = pbm_pb_type_and_slot_to_session_type(slot,PBM_GPB);

      rex_enter_crit_sect(&pbm_crit_sect);

      pbm_session_info[card_sess].slot_id = (uint16)slot ; 
      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        /* clear caches in all sessions present on this slot */
        if ((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
              !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          pbm_ecc_clear_all_id((pbm_session_enum_type)index,
                                 PBM_FIELD_SIM_ECC);
          pbm_ecc_clear_all_id((pbm_session_enum_type)index,
                                  PBM_FIELD_NETWORK_ECC);
        }
      }

      rex_leave_crit_sect(&pbm_crit_sect);

      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
            !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          /* Initlize ECC NV per SUB */
          pbm_build_slot_specific_ecc_cache(index, EMERGENCY_GW_1X, 
                                            PBM_NV_EMERGENCY_NUMBERS_PER_SUB);

          /* need to set hardcoded eccs as if card is present, and LPM mode is not ON */
          pbm_ecc_set_hardcoded_eccs(TRUE);
          break;
        }
      }

      pbm_set_session_caches_status(pbm_pb_type_and_slot_to_session_type(slot,PBM_GPB),
                                    PBM_STATUS_NOT_READY );
      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if (pbm_session_type_to_pb_category( (pbm_session_enum_type)index ) == PBM_LPB &&
             !(pbm_session_activated & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          pbm_set_session_caches_status(index,PBM_STATUS_NOT_READY);
        }
      }

      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
            !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          PBM_SET_PB_CACHE_READY(&ecc_other_cache); /* cache ready for hardcoded #s */
          break;
        }
      }
    }
    break;  //MMGSDI_CARD_INSERTED_EVT
    case MMGSDI_ECC_EVT:
    {
      pbm_session_enum_type          pbm_card_session = PBM_SESSION_GPB_1;

      UIM_MSG_HIGH_3("PBM received MMGSDI_ECC_EVT slot %d for session %d, num ecc %d.",
                      mmgsdi_event.data.ecc.slot,
                      session_type,
                      mmgsdi_event.data.ecc.ecc_count);

      rex_enter_crit_sect(&pbm_crit_sect);

      /* Get session_index for GPB for same (slot) first */
      pbm_card_session = pbm_pb_type_and_slot_to_session_type(
                                   pbm_session_info[session_type].slot_id,
                                   PBM_GPB);

      /* clear NV ecc per sub no's if any */
      pbm_ecc_clear_all_id(pbm_card_session, PBM_FIELD_SIM_ECC);
      rex_leave_crit_sect(&pbm_crit_sect);

      /* Make sure we free the memory which was allocated in deep copy in the below function.*/
      pbm_update_sim_ecc_cache(session_type,&mmgsdi_event.data.ecc);
    }
    break;
    case MMGSDI_PIN1_EVT:
    {
            boolean found_another = FALSE;
            uint8 l_i;
            pbm_session_enum_type card_session = PBM_SESSION_GPB_1;
            atomic_word_t   pin1_prev_status; 

            pin1_prev_status.value  = pbm_pin1_verified.value ; 

            UIM_MSG_HIGH_3("PBM received MMGSDI_PIN1_EVT with status  %d for session %d, for pin %d.",
                         mmgsdi_event.data.pin.pin_info.status,
                         session_type,
                         mmgsdi_event.data.pin.pin_info.pin_id);
            if( pbm_session_type_to_pb_category(session_type) != PBM_GPB  )
            {
              UIM_MSG_HIGH_2("PIN app_type %d for slot %d",
                           mmgsdi_event.data.pin.aid_type[0].app_type,
                           mmgsdi_event.data.pin.slot);
              pbm_session_info[session_type].app_type = mmgsdi_event.data.pin.aid_type[0].app_type;
	            pbm_session_info[session_type].slot_id = mmgsdi_event.data.pin.slot;
            }

      PBM_VALIDATE_SESSION_RET(session_type, VOID);

            //since PIN events would come only for PROV sessions
            //we need to take the appropriate action on
            //the card session that belonged to this prov session
            switch( mmgsdi_event.data.pin.slot )
            {
               case MMGSDI_SLOT_1 :
                 card_session = PBM_SESSION_GPB_1;
               break;

               case MMGSDI_SLOT_2 :
                 card_session = PBM_SESSION_GPB_2;
               break;

#ifdef FEATURE_TRIPLE_SIM
               case MMGSDI_SLOT_3 :
                 card_session = PBM_SESSION_GPB_3;
               break;
#endif
               default :
               break;
            }

            for(l_i = (uint8)PBM_SESSION_GPB_1;l_i < (uint8)PBM_SESSION_MAX; l_i++)
            {
              //check if there exists 2nd provisioning session on the same slot
              //if not we should clear the appropriate card session as well
              if(pbm_session_type_to_pb_category((pbm_session_enum_type)l_i) == PBM_LPB && l_i != (uint8)session_type)
              {
                if(pbm_session_info[l_i].isActive == TRUE &&
                   pbm_session_info[l_i].slot_id == (uint16) PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.pin.slot))
                  found_another = TRUE;
              }
            }

            switch ( mmgsdi_event.data.pin.pin_info.status )
            {
               case  MMGSDI_PIN_PERM_BLOCKED :
               {
                  if(TRUE == pbm_session_info[session_type].isActive)
                  {
                    pbm_stop_fileinfo_internal(session_type);
                  }
                  //need to clear caches except ECC
                  pbm_clear_and_set_session_caches(session_type, PBM_STATUS_SIM_FAIL, FALSE);
                  PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
                  if(!found_another)
                  {
                    pbm_stop_fileinfo_internal(card_session);
                    pbm_clear_and_set_session_caches(card_session, PBM_STATUS_SIM_FAIL, FALSE);
                    PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
                  }
               }
               break;

               case MMGSDI_PIN_ENABLED_NOT_VERIFIED :
               {
                   //no need to restart if only ECC being read
                   if(TRUE == pbm_session_info[session_type].isActive)
                   {
                      pbm_stop_fileinfo_internal(session_type);
                   }
                   //need to clear caches except ECC
                   pbm_clear_and_set_session_caches(session_type, PBM_STATUS_PIN_REQ, FALSE);
                   PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
                   if(!found_another)
                   {
                     pbm_stop_fileinfo_internal(card_session);
                     pbm_clear_and_set_session_caches(card_session, PBM_STATUS_PIN_REQ, FALSE);
                     PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
                   }
               }
               break;

               //there can be 2 possibilities for event MMGSDI_PIN_ENABLED_VERIFIED
               // 1 - Earlier PIN is enabled and not verified , now it is verified
               // 2 - SIM doesnt need PIN verification and now PIN is
                   //enabled to prompt for verification in the next powerup
               //for case 2 we dont need to take any action since not applicable for PBM
               case MMGSDI_PIN_ENABLED_VERIFIED :
               {
                  //here we should just mark the caches as not ready.
                  //However subscription ready follows this
                  //and PBM init would be triggred from that
                  if(pbm_session_info[session_type].isActive == FALSE)
                  {
                     pbm_clear_and_set_session_caches(session_type, PBM_STATUS_NOT_READY, FALSE);
                     if(!found_another)
                     {
                        pbm_clear_and_set_session_caches(card_session, PBM_STATUS_NOT_READY, FALSE);
                     }
                  }
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                  /* in case of enabled verified no need to check other  app's status for pin1_verified on same slot */
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
               }
               break;

               case MMGSDI_PIN_BLOCKED :
               {
                  //isActive flag tells us whether we have recieved this event during powerup (OR)
                  //this PIN is blocked after MMGSDI_SUBSCRIPTION_READY_EVT
                  if((TRUE == pbm_init_info.pbm_session_init_done[session_type]) &&
                                         (TRUE == pbm_session_info[session_type].isActive))
                  {
                     //in this case the session is already initialized ,
                     //dont flush the caches , instead just mark the status as PBM_STATUS_READY_PUC_REQ
                     //when we recieve MMGSDI_PIN_UNBLOCKED event we can just mark them back to PBM_STATUS_READY
                     pbm_set_session_caches_status(session_type, PBM_STATUS_READY_PUC_REQ);
                  }
                  else
                  {
                     //session is in initialization process
                     //since anyway it is not completed we will flush all the caches
                     //and restart the initialization after we get MMGSDI_PIN_UNBLOCKED event
                     if(TRUE == pbm_session_info[session_type].isActive)
                     {
                        pbm_stop_fileinfo_internal(session_type);
                     }
                     pbm_clear_and_set_session_caches(session_type, PBM_STATUS_PUC_REQ, FALSE);
                  }
                  PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
                  if(!found_another)
                  {
                     if((TRUE == pbm_init_info.pbm_session_init_done[card_session]) &&
                                        (TRUE == pbm_session_info[card_session].isActive))
                     {
                        pbm_set_session_caches_status(card_session, PBM_STATUS_READY_PUC_REQ);
                     }
                     else
                     {
                        if(TRUE == pbm_session_info[card_session].isActive)
                        {
                           pbm_stop_fileinfo_internal(card_session);
                        }
                        pbm_clear_and_set_session_caches(card_session, PBM_STATUS_PUC_REQ, FALSE);
                     }
                     PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
                  }
               }
               break;

               case MMGSDI_PIN_UNBLOCKED :
               {
                  if((TRUE == pbm_init_info.pbm_session_init_done[session_type]) &&
                                      (TRUE == pbm_session_info[session_type].isActive))
                  {
                      //in this case the session is already initialized ,
                      //we can just mark them back to STATUS_READY
                      pbm_set_session_caches_status(session_type, PBM_STATUS_READY);
                  }
                  else if(TRUE == pbm_session_info[session_type].isActive)
                  {
                     //session is in initialization process
                     //restart the initialization
                     pbm_session_info[session_type].isActive = FALSE;
                     pbm_handle_app_init_completed(pbm_session_info[session_type].session_id,
                                                   mmgsdi_event.data.pin.slot,
                                                   pbm_session_info[session_type].app_type);
                  }
                  if(!found_another)
                  {
                     if((TRUE == pbm_init_info.pbm_session_init_done[card_session]) &&
                                        (TRUE == pbm_session_info[card_session].isActive))
                    {
                       pbm_set_session_caches_status(card_session, PBM_STATUS_READY);
                    }
                    else if(TRUE == pbm_session_info[card_session].isActive)
                    {
                      if ( pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(card_session))
                      {
                        UIM_MSG_HIGH_1("Already session init %d",card_session);
                        PBM_SESSION_SET(pbm_session_restart, card_session);
                      }
                      else
                      {
                         PBM_SESSION_SET(pbm_session_initializing, card_session);
                         pbm_session_phonebook_init(card_session);
                      }
                    }
                  }
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                  /* in case of pin unblocked no need to check other  app's status for pin1_verified on same slot */
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
              }
              break;

              case MMGSDI_PIN_DISABLED :
              {
                PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
              }
              break;

              case MMGSDI_PIN_CHANGED :
        {
          /* changed is equivalent to verified case */
                 PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                 PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
              }
              break;

              default :
                 break;
           }

           if( pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status) ) 
           {
             /* if hardcoded list is updated send ready */
             PBM_SET_PB_CACHE_READY(&ecc_other_cache);
           }
         }
         break;

      case MMGSDI_PIN2_EVT:
         {
            boolean found_another = FALSE;
            uint8 l_i;
            pbm_session_enum_type card_session = PBM_SESSION_GPB_1;

            UIM_MSG_HIGH_3("PBM received MMGSDI_PIN2_EVT with status  %d for session %d, for pin %d.",
                         mmgsdi_event.data.pin.pin_info.status,
                         session_type,
                         mmgsdi_event.data.pin.pin_info.pin_id);
            PBM_VALIDATE_SESSION_RET(session_type, VOID);
            //since PIN events would come only for PROV sessions
            //we need to take the appropriate action on
            //the card session that belonged to this prov session
            if(mmgsdi_event.data.pin.slot == MMGSDI_SLOT_1)
            {
              card_session = PBM_SESSION_GPB_1;
            }
            else if(mmgsdi_event.data.pin.slot == MMGSDI_SLOT_2)
            {
              card_session = PBM_SESSION_GPB_2;
            }
#ifdef FEATURE_TRIPLE_SIM
            else if(mmgsdi_event.data.pin.slot == MMGSDI_SLOT_3)
            {
              card_session = PBM_SESSION_GPB_3;
            }
#endif
            for(l_i = (uint8)PBM_SESSION_GPB_1;l_i < (uint8)PBM_SESSION_MAX; l_i++)
            {
              //check if there exists 2nd provisioning session on the same slot
              if(pbm_session_type_to_pb_category((pbm_session_enum_type)l_i) == PBM_LPB && l_i != (uint8)session_type)
              {
                if(pbm_session_info[l_i].isActive == TRUE &&
                   pbm_session_info[l_i].slot_id == (uint16) PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.pin.slot))
                  found_another = TRUE;
                break; /* break to have the valid value for l_i */
              }
            }
            switch(mmgsdi_event.data.pin.pin_info.status)
            {
            case MMGSDI_PIN_PERM_BLOCKED:
            case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
            case MMGSDI_PIN_BLOCKED:
              {
                PBM_SESSION_ATOMIC_RESET(pbm_pin2_verified, session_type);
                if(!found_another)
                {
                  PBM_SESSION_ATOMIC_RESET(pbm_pin2_verified, card_session);
                }
              }
              break;
            case MMGSDI_PIN_ENABLED_VERIFIED:
            case MMGSDI_PIN_UNBLOCKED:
            case MMGSDI_PIN_DISABLED:
              {
                PBM_SESSION_ATOMIC_SET(pbm_pin2_verified, session_type);
                /* in case of enabled verified no need to check other  app's status for pin2_verified on same slot */
                PBM_SESSION_ATOMIC_SET(pbm_pin2_verified, card_session);
              }
              break;
            default:
              break;
            }
         }
         break;

      case MMGSDI_CARD_ERROR_EVT:
      case MMGSDI_CARD_REMOVED_EVT:
         {
            uint8                           ecc_sim_airplane         = 0;
            boolean                         card_present_on_any_slot = FALSE;
            uim_common_efs_status_enum_type efs_status               = UIM_COMMON_EFS_ERROR;
            atomic_word_t              pin1_prev_status;
            pin1_prev_status.value   = pbm_pin1_verified.value ; 

            efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_SIM_ECC_AIRPLANE,
                                             UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                             UIM_COMMON_EFS_DEVICE,
                                             &ecc_sim_airplane,
                                            (uint32)sizeof(ecc_sim_airplane));
            
            if(efs_status != UIM_COMMON_EFS_SUCCESS)
            {
              UIM_MSG_MED_0("Failed reading pbm_sim_ecc_airplane - pick default");
            }

            slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_error.slot);
            PBM_VALIDATE_SLOT_RET(slot, VOID);

            pbm_card_status[slot - 1] = FALSE;
            UIM_MSG_HIGH_2("PBM received MMGSDI_CARD_ERROR_EVT for slot %d, pbm_card_status %d",
                            slot, pbm_card_status[slot - 1]);
            UIM_MSG_HIGH_2("PBM ecc_sim_airplane nv %d and event %d",ecc_sim_airplane,mmgsdi_event.evt);

           /* Initilizing the queue globals per slot */
            pbm_session_q_init(slot);

            for (index = 0; index < (int) MAX_UIM_APPS; index++ )
            {
               /* clear caches in all sessions present on this slot */
               if ( slot == (pbm_slot_type)pbm_session_info[index].slot_id )
               {
                  pbm_stop_fileinfo_internal((pbm_session_enum_type)index);
                  pbm_clear_and_set_session_caches((pbm_session_enum_type)index, PBM_STATUS_NO_SIM, FALSE);
                  rex_enter_crit_sect(&pbm_crit_sect);
                  pbm_session_info[index].isActive = FALSE;
                  if ( ecc_sim_airplane && (mmgsdi_event.evt == MMGSDI_CARD_ERROR_EVT) && 
                      (mmgsdi_event.data.card_error.info == MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY ))
                  {
                      PBM_SESSION_SET(pbm_session_lpm_status, index);
                  }
                  else
                  {
                      PBM_SESSION_RESET(pbm_session_lpm_status, index);
                      pbm_ecc_clear_all_id((pbm_session_enum_type)index, PBM_FIELD_SIM_ECC);
                      pbm_ecc_clear_all_id((pbm_session_enum_type)index, PBM_FIELD_NETWORK_ECC);
                  }

                  pbm_session_info[index].slot_id  = 0 ;
                  rex_leave_crit_sect(&pbm_crit_sect);
               }
            }

            if(pbm_card_status[PBM_SLOT_1 - 1] == TRUE || 
               pbm_card_status[PBM_SLOT_2 - 1] == TRUE || pbm_card_status[PBM_SLOT_3 - 1] == TRUE)
            {
              card_present_on_any_slot = TRUE;
            }


            if ( !card_present_on_any_slot && 
		         (!ecc_sim_airplane || (mmgsdi_event.evt != MMGSDI_CARD_ERROR_EVT) || 
                 (mmgsdi_event.data.card_error.info != MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY) ))
            {
              pbm_ecc_set_hardcoded_eccs(FALSE);
              PBM_SET_PB_CACHE_READY(&ecc_other_cache);
            }
            else if ( card_present_on_any_slot )
            {
              if ( pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status) ) 
              {
                 PBM_SET_PB_CACHE_READY(&ecc_other_cache);
              }
            }
         }
         break;


      case MMGSDI_FDN_EVT:
         {
           pbm_session_enum_type pbm_card_session;

          if (mmgsdi_event.data.fdn.evt_source == MMGSDI_FDN_SOURCE_CARD_INIT ||
              mmgsdi_event.data.fdn.evt_source == MMGSDI_FDN_SOURCE_REFRESH)
          {
            break;
          }
           /* Resetting the session_init_done for the provisioning and card session to
              avoid sending PBM_EVENT_SESSION_INIT_DONE twice */
           pbm_init_info.pbm_session_init_done[session_type] = FALSE;
           /* get session_index for GPB for same (slot) first */
           pbm_card_session = pbm_pb_type_and_slot_to_session_type(pbm_session_info[session_type].slot_id, PBM_GPB);
           pbm_init_info.pbm_session_init_done[pbm_card_session] = FALSE;
           UIM_MSG_HIGH_3("PBM received MMGSDI_FDN_EVT status %d  for session %d, rec num %d",
                         mmgsdi_event.data.fdn.enabled, session_type, mmgsdi_event.data.fdn.rec_num);
           PBM_VALIDATE_SESSION_RET(session_type, VOID);

            if(mmgsdi_event.data.fdn.rec_num != 0)
            {
              int buffer_index;
              PB_CACHE *cache = NULL;
              int extension_rec_num, iterator =0;
			  boolean pbm_waiting_write_cnf = FALSE;
			  pbm_phonebook_type pb_id = {PBM_GPB};
			  pbm_record_id_type rec_id;
              //if the card type is ICC though PBM gets FDN rec changed event on prov session
              //PBM need to update the corresponding card session
              if(USE_GSM_SUPPORT(session_type) || USE_CDMA_SUPPORT(session_type))
              {
                session_type = pbm_card_session;
              }

			  pb_id = pbm_file_id_to_pb_id(session_type, PBM_FILE_FDN);
			  rec_id = pbm_session_location_to_record_id(pb_id,mmgsdi_event.data.fdn.rec_num);
              /* if PBM recieves this event before the write cnf
			  has come from gsdi for FDN write originated via PBM.
			  In this case PBM dont need to do anything for this event.
			  PBM will update its commited list and caches when write
			  cnf event would come from GSDI */
              for(iterator = 0; iterator < PBM_ASYNC_BUFFERS; iterator++)
			  {
				  /* check all pbm buffers to see if the for rec_id write rsp cnf is awaited */
				  if( (pbm_buffers[iterator].in_use) && (pbm_buffers[iterator].state == PBM_WRITE_DONE)
				  	&& (pbm_compare_rec_ids(pbm_buffers[iterator].write->rec_id,rec_id) ))
				  {
                                    pbm_waiting_write_cnf = TRUE;
                                    UIM_MSG_HIGH_3("PBM waiting for write cnf for record %d  session %d, buffer index %d",
                                                    rec_id, session_type, iterator);
				  }
			  }
			  if(!pbm_waiting_write_cnf) /* PBM not waiting for write confirmation */
			  {
              cache = (PB_CACHE *) pbm_file_id_to_cache (session_type, PBM_FILE_FDN);
              //if the particular FDN cache is not yet ready,
              //we cannot update the cache,otherwise we need to restart the initialization of this session
              if (cache->status != PBM_STATUS_READY)
              {
                if (pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                {
                  UIM_MSG_HIGH_1("PBM Already init session %d.  Continue later.\n", session_type);
                  PBM_SESSION_SET(pbm_session_restart, session_type);
                  break;
                }
              }
              else
              {
                buffer_index = pbm_alloc_record();
                if(buffer_index < 0)
                {
                  UIM_MSG_ERR_0("Unable to alloc PBM buffer");
                  break;
                }
                pbm_buffers[buffer_index].record.index = mmgsdi_event.data.fdn.rec_num;
                if ( pbm_mmgsdi_process_sim_data(session_type,
                                                 PBM_FILE_FDN,
                                                 mmgsdi_event.data.fdn.rec_num,
                                                 mmgsdi_event.data.fdn.rec_data.data_ptr,
                                                 &pbm_buffers[buffer_index].record.data.num_text,
                                                 &extension_rec_num))
                {
                  pbm_addr_cache_s_type *prev_rec=NULL,*curr_rec=NULL;
                  boolean record_added = FALSE;
                  pbm_notify_event_e_type event = PBM_EVENT_REC_ADD;
                  pbm_session_event_data_u_type evt_data = {0};
                  /* Processing was complete. No ext record */
                  pbm_phonebook_type pb_id = {PBM_GPB};
                  pb_id = pbm_file_id_to_pb_id(session_type, PBM_FILE_FDN);
                  rex_enter_crit_sect(&pbm_crit_sect);
                  /* for fdn cache since fdn cache could be ready for this back door event, and some thread might be updating the fdn cache via record write */
                  prev_rec = cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num];
                  if (cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num] != NULL)
                  {
                     if ( pbm_cache_delete(cache, cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num]->record_id) != PBM_SUCCESS )
                     {
                        rex_leave_crit_sect(&pbm_crit_sect);
                        break;
                     }
                  }
                  record_added = pbm_cache_record_add(pb_id,
                       &pbm_buffers[buffer_index].record.data.num_text,
                    mmgsdi_event.data.fdn.rec_num);
                  if (record_added)
                  {
                    curr_rec = cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num];
                  }
                  rex_leave_crit_sect(&pbm_crit_sect);
                  if(record_added)
                  {
                    evt_data.rec_id = pbm_session_location_to_record_id(pb_id,mmgsdi_event.data.fdn.rec_num);
                    if(!prev_rec && curr_rec)
                    {
                      event = PBM_EVENT_REC_ADD;
                    }
                    else if(prev_rec && curr_rec)
                    {
                      event = PBM_EVENT_REC_UPDATE;
                    }
                    else if(prev_rec && !curr_rec)
                    {
                      event = PBM_EVENT_REC_DELETE;
                    }
                    pbm_add_notify(event, evt_data);
                  }
                  else
                  {
                    UIM_MSG_ERR_2("Unable to add FDN rec to cache, index %d session %d",
                                   mmgsdi_event.data.fdn.rec_num, session_type);
                  }
                  pbm_free_record((int) buffer_index);
                  break;
                }
                else
                {
                   if (extension_rec_num && cache->extension_cache &&
                       (cache->extension_cache->file_id != PBM_FILE_NONE))
                   {
                      if (pbm_send_read_request(session_type,
                                                cache->extension_cache->file_id,
                                                extension_rec_num,
                                                (int) buffer_index,
                                                pbm_mmgsdi_fdn_ext_read_cb))
                      {
                         break;
                      }
                      UIM_MSG_ERR_1("Could not issue read request %d", extension_rec_num);
                   } else
                   {
                      UIM_MSG_ERR_1("PBM could not find a valid extension record from cache 0x%x",
                                  cache);
                   }
                }
              }
            }
            //Clean up deep copy of record data
            PBM_MEM_FREEIF(mmgsdi_event.data.fdn.rec_data.data_ptr);
			}
            else if (mmgsdi_event.data.fdn.enabled)
            {
               pbm_fdn_enable(session_type);
            } else
            {
               pbm_fdn_disable(session_type);
            }
         }
         break;

      case MMGSDI_REFRESH_EVT:
         {
            UIM_MSG_HIGH_3("PBM received MMGSDI_REFRESH_EVT for session %d, mode %d, stage %d",
                         session_type,
                         mmgsdi_event.data.refresh.mode,
                         mmgsdi_event.data.refresh.stage);

            if (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_FCN &&
                mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
            {
              /* Send refresh complete immediately if the session is already in PBM intilization
                 queue for which REFRESH FCN is received here */
              if(pbm_if_queue_has_sess(session_type, pbm_session_info[session_type].slot_id - 1))
              {
                /* This session Initialization is due to refresh - Send refresh complete to MMGSDI */
                (void) mmgsdi_session_refresh_complete(pbm_session_info[session_type].session_id,
                                                       TRUE,
                                                       pbm_mmgsdi_refresh_complete_cb,
                                                       0);
              }
              else
              {
                //Kick off pbm app fcn refresh
                pbm_refresh_file_change_notification_cb(&(mmgsdi_event.data.refresh.refresh_files),
                                                        mmgsdi_event.session_id);
              }
            } else if (((mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN) ||
		        (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_3G_SESSION_RESET) ||
                        (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_FCN) )&&
                       (mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN))
            {
               //Notify MMGSDI it's okay to proceed with refresh for the above modes of refresh
               ret = mmgsdi_session_ok_to_refresh(mmgsdi_event.session_id,
                                            TRUE,
                                            pbm_mmgsdi_ok_to_refresh_cb,
                                            0);
            } else if ( ( (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_APP_RESET)
              || (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_RESET) )
              && (mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_START))
            {
              /* reset the LPM status variable, since in case of reset refresh, we need to read ecc again */
              PBM_SESSION_RESET(pbm_session_lpm_status, session_type);
            }

            if ( ret != MMGSDI_SUCCESS )
            {
               UIM_MSG_HIGH_1(" PBM: mmgsdi_session_ok_to_refresh returned error  %d",ret);
            }

            //Clean up deep copy of file list
            PBM_MEM_FREEIF(mmgsdi_event.data.refresh.refresh_files.file_list_ptr);
         }
         break;

      case MMGSDI_SESSION_CHANGED_EVT:
      {
        mmgsdi_app_info_type     app_info          = mmgsdi_event.data.session_changed.app_info;
        PB_CACHE                *ecc_cache         = NULL;
        atomic_word_t            pin1_prev_status;

        pin1_prev_status.value = pbm_pin1_verified.value ; 

        UIM_MSG_HIGH_3("PBM received MMGSDI_SESSION_CHANGED_EVT event on session %d app_type %d activated Status %d",
                        session_type, app_info.app_data.app_type,mmgsdi_event.data.session_changed.activated);
        if( pbm_session_type_to_pb_category(session_type) != PBM_LPB )
        {
          break;
        }

        if(mmgsdi_event.data.session_changed.activated == TRUE)
        {
          if( pbm_session_type_to_pb_category(session_type) != PBM_GPB )
          {
            pbm_session_info[session_type].app_type = app_info.app_data.app_type;
          }

          pbm_session_info[session_type].slot_id = (uint16) mmgsdi_event.data.session_changed.app_info.slot;
        }
        else
        {
          rex_enter_crit_sect(&pbm_crit_sect);

          /* clearing the card ECC and OTA numbers */
          pbm_ecc_clear_all_id(session_type, PBM_FIELD_SIM_ECC);
          pbm_ecc_clear_all_id(session_type, PBM_FIELD_NETWORK_ECC);

          rex_leave_crit_sect(&pbm_crit_sect);

          ecc_cache = pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
          UIM_MSG_HIGH_1("PBM received MMGSDI_SESSION_CHANGED_EVT event on slot %d",
                          (uint16) mmgsdi_event.data.session_changed.app_info.slot);

          /* PBM session deactivate handling */
          pbm_session_deactivate_handling(session_type,
                                          (uint16) mmgsdi_event.data.session_changed.app_info.slot);

          /* pin1 cleared in the above de-activate handling and hence cache building is to be after that */
         (void)pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status);

          /* Skip ECC ready indication if an card error received before session de-activation */
          if(pbm_card_status[PBM_SLOT_1 - 1] == TRUE || 
             pbm_card_status[PBM_SLOT_2 - 1] == TRUE || pbm_card_status[PBM_SLOT_3 - 1] == TRUE)
          {
            /* sending cache ready to clients - not checking for pin1_status_change has updated hardcoded list because this ready has to go 
                      irrespective of its update.Because network and card ecc are cleared out*/
            PBM_SET_PB_CACHE_READY(ecc_cache);
          }
        }
        break;
      }

      case MMGSDI_SUBSCRIPTION_READY_EVT:
         {
            PB_CACHE                cache;
            pbm_session_enum_type   pbm_card_session;
            pbm_slot_type           slot_id;
            mmgsdi_session_id_type  mmgsdi_session_id = mmgsdi_event.session_id;
            mmgsdi_app_info_type    app_info          = mmgsdi_event.data.subscription_ready.app_info;

            memset(&cache,0,sizeof(PB_CACHE));
            if( pbm_session_type_to_pb_category(session_type) != PBM_LPB )
            {
              UIM_MSG_HIGH_1("Subs ready on incorrect session %d",session_type);
              break;
            }

      pbm_session_info[session_type].in_use_adn_pb = PBM_PB_NOT_KNOWN;
      /* Resetting the session_init_done for the provisioning and card session to
        avoid sending PBM_EVENT_SESSION_INIT_DONE twice */
      pbm_init_info.pbm_session_init_done[session_type] = FALSE;
      PBM_SESSION_SET(pbm_session_subs_ready,session_type);

            slot_id     = pbm_session_info[session_type].slot_id -1;
      UIM_MSG_HIGH_5("PBM received SUBSCRIPTION READY event on session 0x%x slot_id 0x%x ADN gpb_status 0x%x lpb_1x_status 0x%x lpb_gw_status 0x%x",
	  	      session_type,
                      slot_id,
                      pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].gpb_caching_status,
                      pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].lpb_1x_caching_status,
                      pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].lpb_gw_caching_status);

            rex_enter_crit_sect(&pbm_crit_sect); /* for slot id, app type and in_use_adn_pb */
            if(pbm_session_type_to_pb_category(session_type) != PBM_GPB )
            {
                pbm_session_info[session_type].app_type = app_info.app_data.app_type;
            }
            pbm_session_info[session_type].slot_id = (uint16) mmgsdi_event.data.subscription_ready.app_info.slot;
            /* Go for app init completed */
            //reset the Active flag since it should be re-read
            pbm_session_info[session_type].isActive = FALSE;
            //reset the corresponding GPB's Active flag since it also should be re-read
            if(mmgsdi_event.data.subscription_ready.app_info.slot == MMGSDI_SLOT_1)
            {
                 pbm_session_info[PBM_SESSION_GPB_1].slot_id = (uint16) PBM_SLOT_1;
                 /* assigning the value directly as constant boolean warnign comes for this  */
                 /*PBM_MMGSDI_SLOT_TO_PBM_SLOT(MMGSDI_SLOT_1);*/
                 pbm_session_info[PBM_SESSION_GPB_1].isActive = FALSE;
                 pbm_init_info.pbm_session_init_done[PBM_SESSION_GPB_1] = FALSE;
            }
            else if(mmgsdi_event.data.subscription_ready.app_info.slot == MMGSDI_SLOT_2)
            {
                 pbm_session_info[PBM_SESSION_GPB_2].slot_id = (uint16) PBM_SLOT_2;
                 /* assigning the value directly as constant boolean warnign comes for this  */
                 /*PBM_MMGSDI_SLOT_TO_PBM_SLOT(MMGSDI_SLOT_2);*/
                 pbm_session_info[PBM_SESSION_GPB_2].isActive = FALSE;
                 pbm_init_info.pbm_session_init_done[PBM_SESSION_GPB_2] = FALSE;
            }
#ifdef FEATURE_TRIPLE_SIM
            else if(mmgsdi_event.data.subscription_ready.app_info.slot == MMGSDI_SLOT_3)
            {
                 pbm_session_info[PBM_SESSION_GPB_3].slot_id = (uint16) PBM_SLOT_3;
                 /* assigning the value directly as constant boolean warnign comes for this  */
                 /*PBM_MMGSDI_SLOT_TO_PBM_SLOT(MMGSDI_SLOT_3);*/
                 pbm_session_info[PBM_SESSION_GPB_3].isActive = FALSE;
                 pbm_init_info.pbm_session_init_done[PBM_SESSION_GPB_3] = FALSE;
            }
#endif
            else
            {
                 UIM_MSG_ERR_1("invalid slot from MMGSDI %d", mmgsdi_event.data.subscription_ready.app_info.slot);
            }
  	    rex_leave_crit_sect(&pbm_crit_sect);

            /* This event is to update the QMI PBM cache variables in case of 
             * non-FCN refresh/Hot-swap cases followed by subscription ready event from MMGSDI
             */
            cache.pb_id.slot_id     = pbm_session_info[session_type].slot_id;
            cache.pb_id.prov_type   = pbm_session_type_to_provision_type(session_type);
            cache.pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
            cache.pb_id.device_type = PBM_DEVICE_TYPE_ALL;
            /* Notifying cache status to QMI PBM to clear the global data */
            pbm_notify_pb_cache_status_update(&cache);

            slot_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT(app_info.slot);
            /* get session_index for GPB for same (slot) first */
            pbm_card_session = pbm_pb_type_and_slot_to_session_type(slot_id, PBM_GPB);

            cache.pb_id.slot_id     = pbm_session_info[pbm_card_session].slot_id;
            cache.pb_id.prov_type   = pbm_session_type_to_provision_type(pbm_card_session);
            cache.pb_id.pb_category = pbm_session_type_to_pb_category(pbm_card_session);
            cache.pb_id.device_type = PBM_DEVICE_TYPE_ALL;
            /* Notifying cache status to QMI PBM to clear the global data */
            pbm_notify_pb_cache_status_update(&cache);

            pbm_handle_app_init_completed(mmgsdi_session_id, app_info.slot,app_info.app_data.app_type);
         }
         break;

         //  case MMGSDI_CARD_INIT_COMPLETED_EVT:  /* Depreciated */
         //  case MMGSDI_SESSION_CLOSE_EVT:        /* Depreciated */
         //  case MMGSDI_SELECT_AID_EVT:        /* Depreciated */
        case MMGSDI_HIDDENKEY_EVT:
        {
            PB_CACHE *cache = NULL;
            /* check if the event has come after the ADN cache was ready we need to start the ADN read again */
            /*check to see if the cache is ready*/
            UIM_MSG_HIGH_3("PBM received MMGSDI_HIDDENKEY_EVT event on session %d pin status %d with dir_index %d",
                          session_type, mmgsdi_event.data.pin.pin_info.status, mmgsdi_event.data.pin.dir_index);
            rex_enter_crit_sect(&pbm_crit_sect);
            switch(mmgsdi_event.data.pin.pin_info.status)
            {
                case MMGSDI_PIN_ENABLED_VERIFIED:
                case MMGSDI_PIN_DISABLED:
                    //since disabled is also equalent to enabled-verified
                    pbm_session_info[session_type].dir_index = mmgsdi_event.data.pin.dir_index;
                    PBM_SESSION_SET(pbm_hidden_key_verified, session_type);
                    break;
                case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
                    pbm_session_info[session_type].dir_index = mmgsdi_event.data.pin.dir_index;
                    PBM_SESSION_RESET(pbm_hidden_key_verified, session_type);
                    break;
                case MMGSDI_PIN_NOT_FOUND:
                default:
                    pbm_session_info[session_type].dir_index = 0;
                    PBM_SESSION_SET(pbm_hidden_key_verified, session_type);
                    break;
            }
            cache = pbm_file_id_to_cache(session_type, PBM_FILE_ADN);
            //no need to do anything when hiddenkey is MMGSDI_PIN_NOT_FOUND
            if (cache->status == PBM_STATUS_READY && pbm_session_info[session_type].dir_index != 0)
            {
                /*pbm_init_fileinfo_requests(session_type, PBM_FILE_ADN);*/ /* merge_hidden_contacts functionality this funcion will also be called from the init flow if verify event comes in between the init flow*/
                if(pbm_hidden_key_verified & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                {
                  if(pbm_merge_hidden_contacts(session_type) == PBM_SUCCESS) /* this api NEEDS TO KNOW THE INDICES where the hidden cache has to be added to adn cache */
                  {
                     /* to send event to clients that the cache has been updated */
                     pbm_hidden_key_notify(session_type,TRUE);
                  }
                  else
                  {
                      UIM_MSG_ERR_0("PBM Hidden contacts merging encountered error");
                  }
                }
                else
                {
                  if(pbm_delete_hidden_contacts_from_adn_cache(session_type) == PBM_SUCCESS) /* this api NEEDS TO KNOW THE INDICES where the hidden cache has to be added to adn cache */
                  {
                    /* to send event to clients that the cache has been updated */
                    pbm_hidden_key_notify(session_type,FALSE);
                  }
                  else
                  {
                      UIM_MSG_ERR_0("PBM Hidden contacts un-merging encountered error");
                  }
                }
            }
			rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;
      case MMGSDI_UNIVERSAL_PIN_EVT:
      default:
         break;
   }
}


/*===========================================================================
FUNCTION PBM_SEND_READ_REQUEST

DESCRIPTION
  Populate the msg struct for GSDI including the callback function the
  call gsdi to process the read request.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_send_read_request( pbm_session_enum_type session_type,
                               pbm_file_type file_id,
                               int rec_num,
                               int pbm_buffer_index,
                               mmgsdi_callback_type callback)
{
   int len = 0;
   boolean ret_val = FALSE;
   PB_CACHE *cache;
   pbm_ext_cache_s_type *ext_cache;
   mmgsdi_file_structure_enum_type sim_filetype = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
   PB_USIM_CACHE *usim_cache;
   PB_MASTER_USIM_CACHE *m_usim_cache;
   PB_SYNC1_CACHE *sync1_cache;
   PB_SYNC2_CACHE *sync2_cache;

   switch (pbm_file_id_to_cache_type(file_id))
   {
      case PBM_CACHE_TYPE_PB:
         if ((cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
         {
           pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
           if(pb_set != PBM_USIM_PB_INVALID)
           {
             len = cache->record_length[pb_set];
           }
           else
           {
             len = cache->record_length[0];
           }
		   
           sim_filetype = cache->file_type;
         }
         break;

      case PBM_CACHE_TYPE_USIM:
         if ((usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
         {
            len = usim_cache->record_length;
            sim_filetype = usim_cache->file_type;
         }
         break;

      case PBM_CACHE_TYPE_INDEX:
         if ((m_usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
         {
            len = m_usim_cache->iap_rec_len;
            sim_filetype = m_usim_cache->file_type;
         }
         break;

      case PBM_CACHE_TYPE_SYNC1:
         if ((sync1_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
         {
            len = sync1_cache->record_length;
            sim_filetype = sync1_cache->file_type;
         }
         break;

      case PBM_CACHE_TYPE_SYNC2:
         if ((sync2_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
         {
            len = sync2_cache->record_length;
            sim_filetype = sync2_cache->file_type;
         }
         break;

      case PBM_CACHE_TYPE_EXT:
         if ((ext_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
         {
            len = ext_cache->ext_rec_length;
            sim_filetype = ext_cache->file_type;
         }
         break;

      default:
         UIM_MSG_ERR_1("Unknown file id %d", file_id);
         break;
   }

   if (len == 0)
   {
      UIM_MSG_ERR_2("Could not issue read request for file_id 0x%x on Session 0x%x",
                  file_id, session_type);
   } else
   {
      pbm_return_type pbm_retval;

      /* Now issue the asynchronous command to GSDI. */
      pbm_retval = pbm_mmgsdi_sim_read(session_type,            /* Session */
                                       file_id,      /* file id */
                                       sim_filetype,      /* filetype */
                                       rec_num,           /* Record number */
                                       len,               /* Required data length */
                                       (uint32) pbm_buffer_index,  /* Client Ref. */
                                       callback);         /* Asynchronous callback. */
      if (pbm_retval != PBM_SUCCESS)
      {
         UIM_MSG_ERR_3("PBM pbm_mmgsdi_sim_read returned an error 0x%x for file_id 0x%x on session 0x%x",
                     pbm_retval, file_id, session_type);
      } else
      {
         ret_val = TRUE;
      }
   }

   return ret_val;
}


/*===========================================================================
FUNCTION PBM_UPDATE_REFRESH_GLOBALS

DESCRIPTION
  This function is called to update the REFRESH Related Global Variables
  contained in PBM.

DEPENDENCIES
  None.

SIDE EFFECTS
  refresh_num_files and refresh_file_list
===========================================================================*/
static boolean pbm_update_refresh_globals(
                                         pbm_refresh_file_list_type  *pbm_refresh_file_list_p,
                                         pbm_session_enum_type session_type)
{
   uint8 num_file_devices;
   int refresh_file;
   int file_group_index;
   int file_device_index;
   int i;

   pbm_file_type pbm_file_group;

   pbm_file_type affected_groups[PBM_FILE_MAX_REQUESTS];
   int num_affected_groups;
   uint32 file_set = 0;


   if ( pbm_refresh_file_list_p->num_files > PBM_MAX_FILE_CHANGE_NOTIFICATIONS )
   {
      UIM_MSG_ERR_1("PBM pbm_update_refresh_globals refresh too many files %d",
                  pbm_refresh_file_list_p->num_files);
      return FALSE;
   }

   /* Copy Number of Files */
   pbm_session_info[session_type].pbm_refresh_num_files = pbm_refresh_file_list_p->num_files;

   if ( pbm_refresh_file_list_p->file_list_ptr == NULL )
   {
      return FALSE;
   }

   /* Copy File List */
   memscpy(pbm_session_info[session_type].pbm_refresh_file_list,
          sizeof(pbm_session_info[session_type].pbm_refresh_file_list),
          pbm_refresh_file_list_p->file_list_ptr,
          sizeof(pbm_file_type) * pbm_refresh_file_list_p->num_files);
   if(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
      pbm_session_info[session_type].pbm_refresh_init = PBM_REFRESH_DURING_INIT;
      /* refresh has come when session was still initializing */
   }
   else
   {
      pbm_session_info[session_type].pbm_refresh_init = PBM_REFRESH_ONLY;
      /* refresh event came after session got initialized */
   }

   /* figure out which groups of files were affected */
   pbm_session_info[session_type].pbm_refresh_num_file_devices = 0;
   num_file_devices = 0;
   num_affected_groups = 0;

   memset(pbm_session_info[session_type].pbm_refresh_pbm_file_devices,
          0,
          sizeof(pbm_session_info[session_type].pbm_refresh_pbm_file_devices));
   memset(affected_groups,0,sizeof(affected_groups));

   for (refresh_file=0; refresh_file < pbm_session_info[session_type].pbm_refresh_num_files; refresh_file++)
   {
      pbm_file_group = pbm_filename_to_refresh_group(
                         pbm_session_info[session_type].pbm_refresh_file_list[refresh_file],
                                                                              session_type);
      /* add it to the list, if not already there */
      if (pbm_file_group != PBM_FILE_NONE)
      {
         PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_file_group,
                                     affected_groups,
                                     num_affected_groups,
                                     file_group_index);
      }
   }

   /* now translate the affected groups into the list of files */

   /* go through the list of file groups */
   /* possible optimization: if PBM_FILE_ALL is found, or array becomes full, stop looking */
   file_set = pbm_util_return_fileset_for_session(session_type);
   for (file_group_index=0; file_group_index < num_affected_groups; file_group_index++)
   {
      /* find the files associated */
      for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
      {
         if ((affected_groups[file_group_index] ==
              pbm_file_to_uim_file[i].pbm_file_group[file_set]) ||
             (affected_groups[file_group_index] == PBM_FILE_ALL))
         {
            /* This one is affected.  add it to the list if not already there */
            PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_file_to_uim_file[i].pbm_filename,
                                        pbm_session_info[session_type].pbm_refresh_pbm_file_devices,
                                        num_file_devices,
                                        file_device_index);
         }
      }
   } /* end loop over affected file groups */

   if (session_type < PBM_SESSION_MAX) //lint check for session_type crossing the boundary of MAX_UIM_APPS
   {
      //Add in any files that haven't yet been read the first time
      for (i=pbm_initialize_index[session_type]; i<(int)ARR_SIZE(pbm_initialize_list); i++)
      {
         PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_initialize_list[i],
                                     pbm_session_info[session_type].pbm_refresh_pbm_file_devices,
                                     num_file_devices,
                                     file_device_index);
      }
      pbm_session_info[session_type].pbm_refresh_num_file_devices = num_file_devices;
   }



   return TRUE;
}


/*===========================================================================
FUNCTION PBM_MMGSDI_REFRESH_COMPLETE

DESCRIPTION
  This function sends refresh complete to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_refresh_complete(pbm_session_enum_type session_type)
{
   if ( (pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY) ||
        (pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_DURING_INIT) )
   {
      /* This ession Initialization is due to refresh - Send refresh complete to MMGSDI */
      (void) mmgsdi_session_refresh_complete(pbm_session_info[session_type].session_id,
                                             TRUE,
                                             pbm_mmgsdi_refresh_complete_cb,
                                             0);

      if( pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY )
      {
          /*Send Refresh End events on respective phonebooks only if the Refresh is not during PBM Init
            During PBM init there is no need of sending this event as All PB init done or SIM init done are not yet received to the client 
            and anyways will eventually be sent */
          pbm_file_type          pbm_file_iterator;
          pbm_file_type          pbm_file;
          pbm_phonebook_type     pb_id = {PBM_GPB};
          int                    i = 0, j = 0 ;
          uint8                  local = 0;

          local = pbm_util_return_fileset_for_session(session_type);
          for (i = 0; i < (int) ARR_SIZE(pbm_file_to_uim_file); i++)
          {
             pbm_file = pbm_file_to_uim_file[i].pbm_filename;

             /* Check if the phonebook was changed */
             for (j = 0; j < pbm_session_info[session_type].pbm_refresh_num_files; j++)
             {
                /*Check both the local and global versions*/
                pbm_file_iterator = pbm_file_to_uim_file[i].pbm_session_filename[local];
                if (pbm_file_iterator == pbm_session_info[session_type].pbm_refresh_file_list[j])
                {
                   pb_id = pbm_file_id_to_pb_id(session_type, pbm_file);
                   if (pb_id.device_type != PBM_DEFAULT)
                   {
                       pbm_session_event_data_u_type evt_data = {0};

                       evt_data.pb_id = pb_id;
                       pbm_add_notify(PBM_EVENT_PB_REFRESH_DONE, evt_data);
                   }
                }
             }
          }
      }

      pbm_session_info[session_type].pbm_refresh_init = PBM_REFRESH_NONE;
   }
}



/*===========================================================================
FUNCTION PBM_FREE_EXTENSION_RECORD

DESCRIPTION
  When we will overwrite a record, this routine will find the
  extension record associated with that record and free that in
  our cache structures. It returns the record it freed, or 0.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_free_extension_record(pbm_ext_cache_s_type *ext_cache, int index,
                              pbm_file_type file_id)
{
   int i;
   int ext_num_of_rec;
   pbm_ext_rec_in_use_s_type *ext_rec_in_use;

   PBM_CHECK_PTR_RET(ext_cache, 0);

   ext_num_of_rec = ext_cache->num_of_rec;
   ext_rec_in_use = ext_cache->in_use_array;

   /* Now search through the extension records to see if there is
    * any available.
    */
   for (i = ext_num_of_rec; i > 0; i--)
   {
      /* Is the current record already using an extension record? */
      if ((ext_rec_in_use[i].uim_device == file_id) &&
          (ext_rec_in_use[i].index == index))
      {
         /* erase */
         ext_rec_in_use[i].uim_device = PBM_FILE_NONE;
         ext_rec_in_use[i].index = 0;

         return i;
      }
   }
   /* if we got here, we didn't find it */
   return 0;
}


/*===========================================================================
FUNCTION PBM_FIND_EXTENSION_RECORD

DESCRIPTION
  When its time to pick an extension record, this routine will find one.
  It returns the record it found, or 0.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_find_extension_record(pbm_ext_cache_s_type *ext_cache, int index,
                              pbm_file_type file_id)
{
   int i;        /* Control variable for loop */
   int ext_num_of_rec;
   pbm_ext_rec_in_use_s_type *ext_rec_in_use;
   int extension_record = 0;

   PBM_CHECK_PTR_RET(ext_cache, 0);

   ext_num_of_rec = ext_cache->num_of_rec;
   ext_rec_in_use = ext_cache->in_use_array;

   /* Now search through the extension records to see if there is
    * any available.
    */
   for (i = ext_num_of_rec; i > 0; i--)
   {
      /* Is the current record already using an extension record? */
      if ((ext_rec_in_use[i].uim_device == file_id) &&
          (ext_rec_in_use[i].index == index))
      {
         extension_record = i;
         break;
      } else /* pick a new extension record */
      {
         if (ext_rec_in_use[i].index == 0)
            extension_record = i;
      }
   }

   if (extension_record==0)
   {
      UIM_MSG_ERR_2("No more Extension records available %d %d", ext_num_of_rec, file_id);
   }

   return extension_record;
}


/*===========================================================================
FUNCTION PBM_CHECK_EXTENSION_RECORD

DESCRIPTION
  This routine will return TRUE if a record is currently using an extension
  record, or FALSE if not.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_check_extension_record(pbm_ext_cache_s_type *ext_cache, int index,
                                   pbm_file_type file_id)
{
   int i;
   int ext_num_of_rec;
   pbm_ext_rec_in_use_s_type *ext_rec_in_use;

   PBM_CHECK_PTR_RET(ext_cache, FALSE);

   ext_num_of_rec = ext_cache->num_of_rec;
   ext_rec_in_use = ext_cache->in_use_array;

   /* Now search through the extension records */
   for (i = ext_num_of_rec; i > 0; i--)
   {
      /* Is the current record the right one? */
      if ((ext_rec_in_use[i].uim_device == file_id) &&
          (ext_rec_in_use[i].index == index))
      {
         return i;
      }
   }
   /* if we got here, we didn't find it */
   return 0;
}


/*===========================================================================
FUNCTION pbm_mmgsdi_process_text_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_text_data( pbm_session_enum_type session_type,
                                              pbm_file_type file_id,
                                              int index,
                                              byte *data_buffer,
                                              pbm_text_record_s_type *return_data)
{
   uint16 text_length;
   PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                                     file_id);
   boolean convert = TRUE;

   pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return PBM_ERROR; 
   }

   PBM_CHECK_PTR3_RET(data_buffer, return_data, usim_cache, PBM_ERROR);

   memset(return_data->sim_text, 0xFF, sizeof(return_data->sim_text));
   return_data->adn_record_id = 0;

   text_length = (uint16) usim_cache->record_length;
   if ((text_length > 2) && (usim_cache->mapping_type == PBM_MAPPING_TYPE_2))
   {
     /* This is OK if the record is empty, otherwise not OK */
     if (data_buffer[0] == 0xff)
     {
         convert = FALSE;
	 }
     text_length -= 2;
   }

   if (convert)
   {
       memscpy(return_data->sim_text,sizeof(return_data->sim_text),data_buffer,text_length);  
   }
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION pbm_mmgsdi_process_anr_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the anr record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_anr_data(
  pbm_session_enum_type     session_type,
  pbm_file_type             file_id,
  int                       index,
  byte                     *data_buffer,
  pbm_anr_record_s_type    *return_data,
  int                      *extension_index)
{
  pbm_adn_record_id_type       adn_rec_id  = 0;
  PB_USIM_CACHE               *usim_cache  = NULL;
  pbm_ext_cache_s_type        *ext_cache   = NULL;
  pbm_pb_set_type              pb_set      = PBM_USIM_PB_INVALID;

  usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type, file_id);
  pb_set     = pbm_file_id_to_pb_set(file_id);
  if (!pbm_validate_pb_set(pb_set) )
  {
    return PBM_ERROR;
  }

  PBM_CHECK_PTR4_RET(data_buffer, return_data, usim_cache, extension_index, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_anr_record_s_type));

  /* Check if the number is not blank and number length is configured
     as per spec TS 31.102 4.4.2.9 EF-ANR (Additional Number) or not */
  switch(usim_cache->mapping_type)
  {
    case PBM_MAPPING_TYPE_1:
    {
      adn_rec_id = pbm_location_to_adn_rec_id(session_type, pb_set, (uint8)index);
      if(usim_cache->record_length != 15)
      {
        return PBM_ERROR;
      }
    }
    break;

    case PBM_MAPPING_TYPE_2:
    {
      if(usim_cache->record_length != 17)
      {
        return PBM_ERROR;
      }
    }
    break;

	case PBM_MAPPING_TYPE_3:
    default:
      return PBM_ERROR;
  }

  if(data_buffer[1] != 0xFF)
  {
    /* As per spec TS 31.102 4.4.2.9 EF-ANR value of Length of BCD number/SSC
       contents  should be not more than 11 bytes */
    if(data_buffer[1] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
    {
      data_buffer[1] = PBM_SIM_DIALING_NUMBER_LEN + 1;
    }

    /* one byte for AAS Id,
       one byte for BCD len,
       one byte for TON/NPI,
       ten bytes for BCD number */
    memscpy(&return_data->anr_data, sizeof(return_data->anr_data),
            data_buffer, 3 + PBM_SIM_DIALING_NUMBER_LEN);
  }

  return_data->adn_record_id = adn_rec_id;

  if (data_buffer[14] == 0xFF)
  {
    *extension_index = 0;
  } 
  else
  {
    ext_cache = &ext1_cache[session_type];
    if (ext_cache == NULL || data_buffer[14] < 1 ||
        data_buffer[14] > (byte)ext_cache->num_of_rec)
    {
      UIM_MSG_ERR_3("USIM record index %d file %d wants extension record %d",
                     index, file_id, (int) data_buffer[14]);
    } 
    else
    {
      *extension_index = (int)data_buffer[14];
      ext_cache->in_use_array[*extension_index].uim_device =
      usim_cache->file_id;
      ext_cache->in_use_array[*extension_index].index = (uint8)index;
    }
  }

  return PBM_SUCCESS;
}/* pbm_mmgsdi_process_anr_data */


/*===========================================================================
FUNCTION pbm_mmgsdi_process_grp_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_grp_data( pbm_session_enum_type session_type,
                                             pbm_file_type file_id,
                                             int index,
                                             byte *data_buffer,
                                             pbm_grp_record_s_type *return_data)
{
   int i;
   PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                                     file_id);
   PBM_CHECK_PTR3_RET(data_buffer, return_data, usim_cache, PBM_ERROR);

   memset(return_data, 0, sizeof(pbm_grp_record_s_type));

   for (i = 0; i < MIN(usim_cache->record_length,PBM_MAX_NUM_GROUPS); i++)
   {
	 /* Coding:TS 31.1.02 4.4.2.6 EFGRP (Grouping file)
	  * - '00' – no group indicated;
	  *   'XX' – record number in EFGAS containing the alpha string naming 
          *          the group of which the phone book entry is a member.
	  */
      if (data_buffer[i] != 0xFF && 
          data_buffer[i] < grpname_cache[session_type].num_of_records)
      {
         return_data->group_id[i] = (uint16) data_buffer[i];
      }
   }

   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION pbm_mmgsdi_process_index_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_index_data( pbm_session_enum_type session_type,
                                               pbm_file_type file_id,
                                               int index,
                                               byte *data_buffer,
                                               pbm_iap_record_s_type *return_data)
{
   int i;
   PB_MASTER_USIM_CACHE *m_usim_cache = (PB_MASTER_USIM_CACHE *)
                                        pbm_file_id_to_cache(session_type, file_id);

   PBM_CHECK_PTR3_RET(data_buffer, return_data, m_usim_cache, PBM_ERROR);

   memset(return_data, 0, sizeof(pbm_iap_record_s_type));

   for (i = 0; i < MIN(m_usim_cache->iap_rec_len,PBM_MAX_NUM_TYPE2_FILES); i++)
      if (data_buffer[i] != 0xFF)
         return_data->index[i] = data_buffer[i];

   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_MMGSDI_PROCESS_SIM_DATA

DESCRIPTION
  Parse the data buffer read by pbm_gsdi_sim_read then populate the record
  stucture passed in by the caller.  The return value from this function
  is whether the buffer contained a complete record (TRUE), or if an
  extension record needs to be read (FALSE).  If the function returns
  FALSE, it also populates extension_index to contain the record to be
  read.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean pbm_mmgsdi_process_sim_data(
  pbm_session_enum_type         session_type,
  pbm_file_type                 file_id,
  int                           index,
  byte                         *local_pbm_data_buffer,
  pbm_num_text_record_s_type   *return_data,
  int                          *extension_index)
{
  pbm_fixed_sim_rec_s_type        *fixed_sim_record = NULL;
  pbm_pb_set_type                  pb_set           = PBM_MAX_NUM_PB_SETS;
  int                              cache_text_len   = 0;
  int                              cache_rec_len    = 0;
  time_julian_type                 julian_time;
  PB_CACHE                        *cache            = NULL;

  memset(&julian_time, 0, sizeof(julian_time));
  cache  = (PB_CACHE *)pbm_file_id_to_cache(session_type, file_id);
  PBM_CHECK_PTR4_RET(return_data, extension_index, local_pbm_data_buffer, cache, FALSE);

  /* Initialize return record to 0's */
  memset(return_data, 0, sizeof(pbm_num_text_record_s_type));
  memset(return_data->sim_text, 0xFF, sizeof(return_data->sim_text));

  /* Store the index as this index. */
  return_data->index = index;

  pb_set = pbm_file_id_to_pb_set(file_id);
  if(pb_set != PBM_USIM_PB_INVALID)
  {
    cache_text_len = cache->text_length[pb_set];
    cache_rec_len  = cache->record_length[pb_set];
  }
  else
  {
    cache_text_len = cache->text_length[0];
    cache_rec_len  = cache->record_length[0];
  }

  /* Point to the SIM record starting at the number. */
  //lint -e{826} struct has oci/ici, even though we may not access it
  fixed_sim_record = (pbm_fixed_sim_rec_s_type *) (local_pbm_data_buffer +
                                                   cache_text_len);

  if(local_pbm_data_buffer[0] != 0xFF)
  {
    memscpy(return_data->sim_text,sizeof(return_data->sim_text),
            local_pbm_data_buffer,cache_text_len);
  }

  /* If the number is not blank and number length is configured as per spec 
     TS 31.102 4.4.2.3 EF-ADN (Abbreviated dialling numbers),EF-FDN,etc..*/
  if (fixed_sim_record->bcd_len != 0xFF && fixed_sim_record->number[0] != 0xFF &&
      (cache_rec_len - cache_text_len) >= (2 + PBM_SIM_DIALING_NUMBER_LEN))
  {
    /* one byte for BCD len,
       one byte for Type,
       ten bytes for BCD number */
    memscpy(&return_data->bcd_number, sizeof(return_data->bcd_number),
             fixed_sim_record, 2 + PBM_SIM_DIALING_NUMBER_LEN);

    /* as per spec TS 31.102 4.4.2.9 EF-ANR value of Length of BCD number/SSC
       contents  should be not more than 11 bytes */
    if(return_data->bcd_number[BCD_LEN] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
    {
      return_data->bcd_number[BCD_LEN] = PBM_SIM_DIALING_NUMBER_LEN + 1;
    }
  }

  /* These guys have the timestamp. */
  if ((file_id == PBM_FILE_ICI) || (file_id == PBM_FILE_OCI))
  {
    /* Convert timestamp */
    julian_time.year   = pbm_bcd_to_bin(fixed_sim_record->date_time[0]);

    if (julian_time.year >= 80)  /* 80 becomes 1980, 79 becomes 2079 */
      julian_time.year += 1900;
    else
      julian_time.year += 2000;

    julian_time.month  = pbm_bcd_to_bin(fixed_sim_record->date_time[1]);
    julian_time.day    = pbm_bcd_to_bin(fixed_sim_record->date_time[2]);
    julian_time.hour   = pbm_bcd_to_bin(fixed_sim_record->date_time[3]);
    julian_time.minute = pbm_bcd_to_bin(fixed_sim_record->date_time[4]);
    julian_time.second = pbm_bcd_to_bin(fixed_sim_record->date_time[5]);
    return_data->time  = time_jul_to_secs(&julian_time);

    return_data->duration = fixed_sim_record->duration[0] << 16 |
                            fixed_sim_record->duration[1] << 8  |
                            fixed_sim_record->duration[2];

    if ( return_data->duration == 0x00FFFFFF )
    {
      return_data->duration = 0;
    }
  }

  /* Figure out if there is an extension record.  If not, we are done
   * return TRUE.  If there is, say that there is and return FALSE that
   * the processing is not complete.
   */
  if (fixed_sim_record->ext_id != 0xFF)
  {
    if (!cache->extension_cache)
    {
      UIM_MSG_ERR_2("SIM ERROR, record %d no extension %d",
                     index,  (int) fixed_sim_record->ext_id);
    } 
    else if (((int) fixed_sim_record->ext_id < 1) ||
             ((int) fixed_sim_record->ext_id > cache->extension_cache->num_of_rec))
    {
      UIM_MSG_ERR_3("SIM ERROR record %d wants extension record %d of %d",
                     index, (int) fixed_sim_record->ext_id,
                     cache->extension_cache->num_of_rec);
    }
    else
    {
      *extension_index = (int) fixed_sim_record->ext_id;
      cache->extension_cache->in_use_array[*extension_index].uim_device =
                      pbm_pb_record_index_to_filename(cache, return_data->index);
      cache->extension_cache->in_use_array[*extension_index].index =
                     (byte)pbm_pb_record_index_to_fileloc(cache, return_data->index);
      return FALSE;     /* Processing is not complete. */
    }
  }

  return TRUE;          /* Processing of this buffer is correct. */
}/* pbm_mmgsdi_process_sim_data */


typedef PACKED struct
{
   byte num_digits;
   byte number[8];      /* BCD coding */
   byte ton_npi;
   byte pi_si;   /* 0xFF if not used */
} pbm_fixed_mdn_rec_s_type;

/*===========================================================================
FUNCTION pbm_mmgsdi_process_mdn_data

DESCRIPTION
 This is the special processing for the CSIM MSISDN file data 
 The file is EF MDN for CSIM 
 Base bytes 11 

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

static boolean pbm_mmgsdi_process_mdn_data(pbm_session_enum_type session_type,
                                           pbm_file_type file_id,
                                           int index,
                                           byte *local_pbm_data_buffer,
                                           pbm_num_text_record_s_type *return_data)
{
   pbm_fixed_mdn_rec_s_type *fixed_mdn_record;
   char    number[PBM_NUM_SIZE]; /* + and null for string */

   PB_CACHE *cache = (PB_CACHE *)pbm_file_id_to_cache(session_type, file_id);

   PBM_CHECK_PTR3_RET(return_data, local_pbm_data_buffer, cache, FALSE);

   /* Initialize return record to 0's */
   memset(return_data, 0, sizeof(pbm_num_text_record_s_type));

   /* Store the index as this index. */
   return_data->index = index;

   /* Point to the SIM record starting at the number. */
   //lint -e{826} struct has oci/ici, even though we may not access it
   fixed_mdn_record = (pbm_fixed_mdn_rec_s_type *) local_pbm_data_buffer;
   //interchange nibbles of TON NPI ,to be compatible with GW order
   fixed_mdn_record->ton_npi = (fixed_mdn_record->ton_npi & 0x07) << 4 |
                                (fixed_mdn_record->ton_npi & 0x78) >> 3;
   fixed_mdn_record->num_digits = fixed_mdn_record->num_digits & 0x0F;
   /* If the number is not blank */
   if (fixed_mdn_record->num_digits != 0)
   {
      /* Convert the number to ASCII. */
      if (!pbm_mdn_to_ascii(fixed_mdn_record->number,
                            (fixed_mdn_record->num_digits+1)/2,
                            fixed_mdn_record->ton_npi,
                            (byte *) number ))
      {
         UIM_MSG_ERR_1("SIM ERROR record %d could not be converted to ascii", index);
      }
      if (!pbm_ascii_to_bcd((byte *)number, fixed_mdn_record->ton_npi, return_data->bcd_number))
         return FALSE;
      
   }
   return TRUE;          /* Processing of this buffer is correct. */
}


/*===========================================================================
FUNCTION PBM_MMGSDI_PROCESS_EXT_DATA

DESCRIPTION
  If the phone number is longer than ADN_MAX_PHONE_NUMBER, the
  remainder of the phone number was stored in the extension record
  in the EXT file.  After the extension data is read from GSDI, this
  function is called with the return_data which was filled in by
  pbm_mmgsdi_process_sim_data and this finishes where that function
  left off.

DEPENDENCIES
  return_data should be filled in from the main gsdi record first.

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_process_ext_data(
  byte         *sim_data,
  byte         *bcd_data)
{
  if(sim_data == NULL || bcd_data == NULL)
  {
    return;
  }

  if (sim_data[0] == EXT_REC_TYPE_ADDITIONAL_DATA &&
      sim_data[1] <= PBM_SIM_DIALING_NUMBER_LEN)
  {
    bcd_data[BCD_LEN] += sim_data[1];
    memscpy(&bcd_data[12],sim_data[1],&sim_data[2],sim_data[1]);
  }
  else
  {
    UIM_MSG_ERR_2("error in Extension data : 0x%x 0x%x",
                   sim_data[0],
                   sim_data[1]);
  }
  return;
}/* pbm_mmgsdi_process_ext_data */


/*===========================================================================
FUNCTION PBM_MMGSDI_FILE_INFO_CB

DESCRIPTION
  This function is called by GSDI when it has the results of our last
  get_file_attributes request.  It puts a command into PBM GSDI response qeuue
  and the actual handling is done in pbm_fileinfo_cb.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_mmgsdi_fileinfo_cb (mmgsdi_return_enum_type status,
                             mmgsdi_cnf_enum_type    data_type,
                             const mmgsdi_cnf_type  *data_ptr)
{
   mmgsdi_client_data_type client_ref;
   mmgsdi_file_structure_enum_type file_type;
   int32 num_protection_pin;
   uint32 pin_data_size;
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;

   PBM_CHECK_PTR_RET(data_ptr, VOID);

   client_ref = data_ptr->response_header.client_data;
   if (client_ref >= ARR_SIZE(pbm_fileinfo_requests))
   {
      UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
      return;
   }

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_FILE_INFO_CB;

   mmgsdi_resp_ptr->data_type = data_type;
   mmgsdi_resp_ptr->status = status;
   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));
   file_type = mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_type;
   /* deep copy the protection_pin_ptr since they are to be used in pbm_fileinfo_cb */
   switch (file_type)
   {
	 case MMGSDI_LINEAR_FIXED_FILE:
	    num_protection_pin = data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.num_protection_pin;
		if(num_protection_pin > 0)
		{
		   pin_data_size = (num_protection_pin) * (sizeof(mmgsdi_pin_enum_type));
		   PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,
			  pin_data_size);
                   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,VOID);
                   memscpy((void *)mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr, pin_data_size,
	              (void *)data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,
	               pin_data_size); //deep copy
		}
	    break;

	 case MMGSDI_CYCLIC_FILE:
	    num_protection_pin = data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.num_protection_pin;
		if(num_protection_pin > 0)
		{
		   pin_data_size = (num_protection_pin) * (sizeof(mmgsdi_pin_enum_type));
		   PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr,
			  pin_data_size);
                   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr,VOID);
           memscpy((void *)mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr, pin_data_size,
	              (void *)data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr, pin_data_size); //deep copy
		}
	    break;

	 case MMGSDI_TRANSPARENT_FILE :
		 num_protection_pin = data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.num_protection_pin;
		if(num_protection_pin > 0)
		{
		   pin_data_size = (num_protection_pin) * (sizeof(mmgsdi_pin_enum_type));
		   PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr,
			  pin_data_size);
                   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr,VOID);
           memscpy((void *)mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr, pin_data_size,
	              (void *)data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr, pin_data_size); //deep copy
		}
		break;


	 default:
	    break;
    }


   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}


/*===========================================================================
FUNCTION PBM_MMGSDI_READ_PBR_ATTR_CB

DESCRIPTION
  This function is called by GSDI with the interpreted results of the PBR table.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
===========================================================================*/

void pbm_mmgsdi_read_pbr_attr_cb (mmgsdi_return_enum_type status,
								 mmgsdi_cnf_enum_type	 data_type,
								 const mmgsdi_cnf_type	*data_ptr)
{

   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;

   PBM_CHECK_PTR_RET(data_ptr, VOID);


   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_READ_PBR_ATTR_CB;

   mmgsdi_resp_ptr->data_type = data_type;
   mmgsdi_resp_ptr->status = status;

   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}

/*===========================================================================
FUNCTION PBM_MMGSDI_READ_PBR_REC_CB

DESCRIPTION
  This function is called by GSDI with the interpreted results of the PBR table.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_read_pbr_rec_cb(mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   uint16 read_cnf_data_len = 0;

   PBM_CHECK_PTR_RET(data_ptr, VOID);

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   if (!mmgsdi_resp_ptr)
   {
      UIM_MSG_ERR_0("Unable to allocate PBM command");
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_READ_PBR_REC_CB;

   mmgsdi_resp_ptr->status = status;
   mmgsdi_resp_ptr->data_type = data_type;
   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));


      if (data_ptr->read_cnf.read_data.data_len > 0)
      {
        read_cnf_data_len = data_ptr->read_cnf.read_data.data_len;
        PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,
                                  data_ptr->read_cnf.read_data.data_len);
        PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,VOID);
        memscpy((void *)mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr, data_ptr->read_cnf.read_data.data_len,
                (void *)data_ptr->read_cnf.read_data.data_ptr, read_cnf_data_len); //deep copy
      } else
      {
         mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr = NULL;
      }

   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));

} /*pbm_mmgsdi_async_cb...*/




/*===========================================================================
FUNCTION PBM_CLEAR_REFRESH_CACHES

DESCRIPTION
  This function will clear the caches associated with the global refresh file
  list.

PARAMETERS

SIDE EFFECTS
===========================================================================*/
static void pbm_clear_refresh_caches(pbm_session_enum_type session_type)
{
   int                      i;
   pbm_file_type            file_id;
   PB_CACHE                 *cache;
   pbm_ext_cache_s_type     *ext_cache;
   PB_USIM_CACHE            *usim_cache;
   PB_MASTER_USIM_CACHE     *m_usim_cache;
   PB_SYNC1_CACHE           *sync1_cache;
   PB_SYNC2_CACHE           *sync2_cache;

   /* go through the list of files that you need to clear out */
   /* find the files associated */
   for (i=0; i<pbm_session_info[session_type].pbm_refresh_num_file_devices; i++)
   {
      /* clear the cache */
      file_id = pbm_session_info[session_type].pbm_refresh_pbm_file_devices[i];
       /* if we're doing the ADN group, we need to clear the USIM caches, too. */
      if (file_id == PBM_FILE_ADN)
      {
      pbm_clear_hidden_cache(session_type);
      pbm_clear_usim_caches(session_type);
      }
      switch (pbm_file_id_to_cache_type(file_id))
      {
         case PBM_CACHE_TYPE_PB:
            if ((cache = pbm_file_id_to_cache(session_type, file_id)) != NULL) //lint !e826 area too small
            {
               /* Notifying cache cache status to QMI PBM to clear the PB bitmask for the affected */
               pbm_notify_pb_cache_status_update(cache);
               rex_enter_crit_sect(&pbm_crit_sect);
               pbm_clear_cache(cache);
               rex_leave_crit_sect(&pbm_crit_sect);
            }
            break;

         case PBM_CACHE_TYPE_EXT:
            if ((ext_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
            {
               rex_enter_crit_sect(&pbm_crit_sect);
               /*for ext caches, you can just clear the in use array */
               if (ext_cache->num_of_rec && ext_cache->in_use_array != NULL)
               {
                  memset(ext_cache->in_use_array, 0, (uint32)(ext_cache->num_of_rec + 1) *
                         sizeof(pbm_ext_rec_in_use_s_type));
               }
               rex_leave_crit_sect(&pbm_crit_sect);
            }
            break;

         case PBM_CACHE_TYPE_USIM:
            if ((usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL) //lint !e826 area too small
            {
               rex_enter_crit_sect(&pbm_crit_sect);
               pbm_clear_usim_cache(session_type, usim_cache);
               rex_leave_crit_sect(&pbm_crit_sect);
            }
            break;

         case PBM_CACHE_TYPE_INDEX:
            if ((m_usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL) //lint !e826 area too small
            {
               rex_enter_crit_sect(&pbm_crit_sect);
               pbm_clear_index_cache(m_usim_cache);
               rex_leave_crit_sect(&pbm_crit_sect);
            }
            break;

         case PBM_CACHE_TYPE_SYNC1:
            if ((sync1_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL) //lint !e826 area too small
            {
               rex_enter_crit_sect(&pbm_crit_sect);
               pbm_clear_sync1_cache(sync1_cache);
               rex_leave_crit_sect(&pbm_crit_sect);
            }
            break;

         case PBM_CACHE_TYPE_SYNC2:
            if ((sync2_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL) //lint !e826 area too small
            {
               rex_enter_crit_sect(&pbm_crit_sect);
               pbm_clear_sync2_cache(sync2_cache);
               rex_leave_crit_sect(&pbm_crit_sect);
            }
            break;

         default:
            UIM_MSG_ERR_1("Unknown file id %d", file_id);
            break;
      }

   }
}


/*===========================================================================
FUNCTION PBM_INIT_SEEK_DATA

DESCRIPTION
   Initialize the members of pbm_seek_data for the session_type

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_init_seek_data(pbm_session_enum_type session_type)
{
   int i;

   pbm_seek_data[session_type].status = PBM_ERROR;
   pbm_free_seek_data(session_type); /* it is better to free the data here, since in case of card error there is memory leak */
   pbm_seek_data[session_type].in_use_records = NULL;
   pbm_seek_data[session_type].num_of_records = 0;
   pbm_seek_data[session_type].record_length = 0;
   pbm_seek_data[session_type].num_files = 0;
   pbm_seek_data[session_type].cur_file = 0;

   for (i = 0; i < (int) PBM_MAX_NUM_PB_SETS; ++i)
   {
      pbm_seek_data[session_type].records_in_pb_set[i] = 0;
      pbm_seek_data[session_type].file_ids[i] = PBM_FILE_NONE;
   }
}


/*===========================================================================
FUNCTION PBM_ALLOC_SEEK_DATA

DESCRIPTION
   Allocate memory for in_use_records and set all the records to USED for
   the corresponding session_type

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_alloc_seek_data(pbm_session_enum_type session_type, int num_of_records)
{
   if (num_of_records > 0)
   {
      PBM_MEM_ALLOC(pbm_seek_data[session_type].in_use_records, \
                    (pbm_seek_data[session_type].num_of_records+1)*sizeof(uint8));

      /* Mark all records as used */
      if (pbm_seek_data[session_type].in_use_records != NULL)
      {
         memset(pbm_seek_data[session_type].in_use_records, TRUE,
                (uint32) (pbm_seek_data[session_type].num_of_records+1)*sizeof(uint8));
      } else
      {
         UIM_MSG_ERR_0("Could not allocate memory for SEEK data");
      }
   }
}


/*===========================================================================
FUNCTION PBM_FREE_SEEK_DATA

DESCRIPTION
   Free memory for in_use_records in pbm_session_seek_data corresponding to
   session_type

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_free_seek_data(pbm_session_enum_type session_type)
{
   PBM_MEM_FREEIF(pbm_seek_data[session_type].in_use_records);
}


/*===========================================================================
FUNCTION PBM_MMGSDI_SEEK_CB

DESCRIPTION
   Call back for the mmgsdi_seek(). Enqueues a PBM_GSDI_SEEK_CB command for
   PBM task to process the SEEKed data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_mmgsdi_seek_cb(mmgsdi_return_enum_type status,
                               mmgsdi_cnf_enum_type    data_type,
                               const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   uint16 read_cnf_data_len = 0;


   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_SEEK_CB;

   mmgsdi_resp_ptr->data_type = data_type;
   mmgsdi_resp_ptr->status = status;

   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));


   read_cnf_data_len  = (uint16)data_ptr->search_cnf.searched_record_nums.data_len;
   if (read_cnf_data_len > 0)
   {
     PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr,
                                          (uint32)read_cnf_data_len);
     PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr,VOID);
     memscpy((void *)mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr, (uint32)read_cnf_data_len,
             (void *)data_ptr->search_cnf.searched_record_nums.data_ptr, read_cnf_data_len);  //deep copy
   } else
   {
      mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr = NULL;
   }


   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}


/*===========================================================================
FUNCTION PBM_GET_SEEK_PATTERN

DESCRIPTION
   This function returns the SEEK pattern to use for different files

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint8 pbm_get_seek_pattern(uint32 client_ref)
{
   pbm_file_type file_id;
   uint8 pattern;

   file_id = (pbm_file_type)((int) client_ref % (int) PBM_FILE_MAX_REQUESTS);

   switch (file_id)
   {
      case PBM_FILE_UID:
      case PBM_FILE_PBC:
      case PBM_FILE_GRP:
      case PBM_FILE_UID1:
      case PBM_FILE_PBC1:
      case PBM_FILE_GRP1:
      case PBM_FILE_UID2:
      case PBM_FILE_PBC2:
      case PBM_FILE_GRP2:
      case PBM_FILE_UID3:
      case PBM_FILE_PBC3:
      case PBM_FILE_GRP3:
         pattern = 0x00;
         break;
      default:
         pattern = 0xFFU;
         break;
   }

   return pattern;
}


/*===========================================================================
FUNCTION PBM_SEND_SEEK_REQUEST

DESCRIPTION
   This function sends the SEEK request to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_send_seek_request(pbm_session_enum_type session_type,
                                      uint32 client_ref)
{
   pbm_return_type         ret_val = PBM_ERROR;
   pbm_pb_set_type         pb_set;
   pbm_file_type           file_id = pbm_seek_data[session_type].file_ids[pbm_seek_data[session_type].cur_file];
   mmgsdi_option_type      dummy_option;
   mmgsdi_access_type      file_access;
   mmgsdi_seek_data_type   seek_pattern;
   mmgsdi_return_enum_type seek_ret;
   pbm_file_type           pbm_file = PBM_FILE_NONE;

   memset(&seek_pattern, 0, sizeof(seek_pattern));
   memset(&dummy_option, 0, sizeof(dummy_option));
   pb_set = pbm_file_id_to_pb_set(file_id);
   if(((file_id == PBM_FILE_ADN)    
       || (file_id == PBM_FILE_ADN1)
       || (file_id == PBM_FILE_ADN2)
       || (file_id == PBM_FILE_ADN3)
      ))
   {
     PB_CACHE *cache = (PB_CACHE *) pbm_file_id_to_cache(session_type,file_id);
     PBM_CHECK_PTR_RET(cache, PBM_ERROR);
     seek_pattern.data_len = cache->record_length[pb_set];
   }
   else
   {
     seek_pattern.data_len = pbm_seek_data[session_type].record_length;
   }

   PBM_MEM_ALLOC(seek_pattern.data_ptr, seek_pattern.data_len);
   PBM_CHECK_PTR_RET(seek_pattern.data_ptr, PBM_ERROR_MEM_FULL);
   memset(seek_pattern.data_ptr, pbm_get_seek_pattern(client_ref),
                                                seek_pattern.data_len);

   pbm_file = pbm_seek_data[session_type].file_ids[pbm_seek_data[session_type].cur_file];

   if ( PBM_FILE_FDN == pbm_file )
   {
     ret_val = pbm_populate_enum_file_access(session_type,&file_access, pbm_file);
   }
   else
   {
     ret_val = pbm_populate_file_access(session_type,&file_access, pbm_file);
   }

   if(ret_val != PBM_SUCCESS)
   {
      UIM_MSG_HIGH_3("PBM: in pbm_send_seek_request pbm_populate_file_access returned error %d session %d file %d",
                      ret_val,session_type,pbm_file);
      /* Free the memory allocated for the SEEK pattern */
      PBM_MEM_FREEIF(seek_pattern.data_ptr);
      return ret_val;
   }

   if ( USE_USIM_SUPPORT(session_type) || USE_CSIM_SUPPORT(session_type) )
   {
      mmgsdi_search_offset_data_type dummy_offset;
      memset(&dummy_offset, 0, sizeof(dummy_offset));

      if ((seek_ret = mmgsdi_session_search(
                                          pbm_session_info[session_type].session_id,
                                          file_access,
                                          MMGSDI_UICC_SIMPLE_SEARCH, 1, MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,
                                          dummy_offset, seek_pattern,
                                          pbm_mmgsdi_seek_cb, client_ref)
         ) == MMGSDI_SUCCESS)
      {
         ret_val = PBM_SUCCESS;
      } else
      {
         UIM_MSG_ERR_1("mmgsdi_search returned %d", seek_ret);
         ret_val = PBM_ERROR;
      }
   } else if ((seek_ret = mmgsdi_session_seek(
                                            pbm_session_info[session_type].session_id,
                                            file_access,
                                            MMGSDI_SEEK_BEGINNING_FORWARDS, seek_pattern,
                                            pbm_mmgsdi_seek_cb, client_ref)
             ) == MMGSDI_SUCCESS)
   {
      ret_val = PBM_SUCCESS;
   } else
   {
      UIM_MSG_ERR_1("mmgsdi_seek returned %d", seek_ret);
      ret_val = PBM_ERROR;
   }


   /* Free the memory allocated for the SEEK pattern */
   PBM_MEM_FREEIF(seek_pattern.data_ptr);

   return ret_val;
}


/*===========================================================================
FUNCTION PBM_SEEK_CB

DESCRIPTION
   This function is called in the context of the PBM task when PBM_GSDI_SEEK_CB
   command is put in the GSDI queue. This function updates the in_use_records[]
   with the list of unused records SEEKed and starts reading the USED records.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_seek_cb(mmgsdi_return_enum_type status,
                 mmgsdi_cnf_enum_type    data_type,
                 const mmgsdi_cnf_type  *data_ptr)
{
   pbm_session_enum_type session_type;
   pbm_file_type file_id;
   uint32 client_ref;
   PBM_CHECK_PTR_RET(data_ptr,VOID);

   client_ref = data_ptr->response_header.client_data;

   session_type  = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);
   file_id = (pbm_file_type)((int) client_ref % (int) PBM_FILE_MAX_REQUESTS);

   // Restart initialization if required
   if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
      UIM_MSG_HIGH_1("PBM restarting init of session_type %d", session_type);
      pbm_restart_fileinfo_internal(session_type);
      return;
   }

   // Stop initialization if required
   if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type)))
   {
      UIM_MSG_HIGH_1("PBM stopping init of SIM %d", session_type);
      return;
   }

   if (status == MMGSDI_SUCCESS)
   {
      if ((data_type == MMGSDI_SEARCH_CNF) &&
          (pbm_seek_data[session_type].in_use_records != NULL))
      {
         const mmgsdi_seek_data_type *seek_data;
         int i;
         int start;  /* start index for this pb set in in_use_records[] */
         int file_index = pbm_seek_data[session_type].cur_file;

         /* Calculate the start index in in_use_records[] of this file */
         for (i = 0, start = 0; i < file_index; ++i)
         {
            start += pbm_seek_data[session_type].records_in_pb_set[i];
         }

         seek_data = &data_ptr->search_cnf.searched_record_nums;

         for (i = 0; i < seek_data->data_len; ++i)
         {
            /* Check that the record index is in range */
            if ((seek_data->data_ptr[i] > 0) &&
                (seek_data->data_ptr[i] <= pbm_seek_data[session_type].records_in_pb_set[file_index]) &&
                (start + seek_data->data_ptr[i] <= pbm_seek_data[session_type].num_of_records))
            {
               pbm_seek_data[session_type].in_use_records[start + seek_data->data_ptr[i]] = FALSE;
            } else
            {
               UIM_MSG_ERR_0("Invalid record number returned in pbm_gsdi_seek_cb");
            }
         }

         /* See if we need to SEEK unused records for the next file in the pb set */
         if (++pbm_seek_data[session_type].cur_file < pbm_seek_data[session_type].num_files)
         {
            if (pbm_send_seek_request(session_type, client_ref) == PBM_SUCCESS)
               return;

            UIM_MSG_ERR_2("PBM SEEK failed of PBM file: sim %d, client ref %d", session_type, client_ref);
         }

         /* The SEEK data is valid */
         pbm_seek_data[session_type].status = PBM_SUCCESS;
      }
   } else
   {
      UIM_MSG_ERR_3("PBM SEEK callback returned %d, sim %d, file id %d", status, session_type, file_id);
   }

   /* Start reading the records for this file. Read atleast one record (even for empty files)
      so that the cache could be set to ready in the read callback */
   if (PBM_SUCCESS == pbm_start_record_read(session_type, file_id, 1))
   {
      return;
   }

   /* Move to next file regardless of why we got here. */
   pbm_initialize_index[session_type]++;

   pbm_init_entries(session_type);
}


/*===========================================================================
FUNCTION PBM_GET_NEXT_USED_RECORD

DESCRIPTION
   Returns the next used record number starting after prev_index. If the
   memory allocation for SEEK data failed or the SEEK was unsuccessful,
   returns the next record in the sequence starting from prev_index.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
int pbm_get_next_used_record(pbm_session_enum_type session_type, int prev_index)
{
   int num_records = pbm_seek_data[session_type].num_of_records;
   int next_index = prev_index + 1;

   /* If memory allocation failed or if SEEK was unsuccessful, read all
      the records in sequence */
   if (pbm_seek_data[session_type].in_use_records != NULL &&
       pbm_seek_data[session_type].status == PBM_SUCCESS)
   {
      for (; next_index <= num_records; ++next_index)
      {
         if (pbm_seek_data[session_type].in_use_records[next_index] == TRUE)
         {
            break;
         }
      }
   }

   return(next_index <= num_records) ? next_index : num_records+1;
}

/*===========================================================================
FUNCTION pbm_open_session_update_session_info

DESCRIPTION
  PBM internal function to update the session related information.
  protected under critical section.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_open_session_update_session_info(pbm_session_enum_type pbm_session,
                                  mmgsdi_session_id_type session_id,
                                  mmgsdi_app_enum_type app_type,
                                  pbm_slot_type pbm_slot_id)
{
    pbm_session_info[pbm_session].session_id = session_id;
    rex_enter_crit_sect(&pbm_crit_sect); /* for pbm_session_info slot id and app type */
    pbm_session_info[pbm_session].slot_id = (uint16) pbm_slot_id;
    pbm_session_info[pbm_session].app_type = app_type;
    rex_leave_crit_sect(&pbm_crit_sect);/* for pbm_session_info slot id and app type*/
}


/*===========================================================================
FUNCTION pbm_mmgsdi_open_session_cb

DESCRIPTION
  Callback function from MMGSDI in response to OPEN SESSION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_open_session_cb (mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    data_type,
                                  const mmgsdi_cnf_type  *data_ptr)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr = NULL;
  uint32                  len = 0;
  boolean                 sent_success = FALSE;
  mmgsdi_session_open_info_type *session_open_info = NULL; 

  if ( data_ptr && data_ptr->session_open_ext_cnf.session_info_ptr )
  {
    mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *) pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));

    if ( mmgsdi_resp_ptr )
    {
      len = sizeof(mmgsdi_session_open_info_type)* MIN(data_ptr->session_open_ext_cnf.num_sessions,PBM_SESSION_MAX );

      PBM_MEM_ALLOC(session_open_info, len);
      PBM_CHECK_PTR_RET(session_open_info, VOID);

      if ( session_open_info )
      {
        mmgsdi_resp_ptr->cmd_type  = PBM_MMGSDI_OPEN_SESSION_CB;
        mmgsdi_resp_ptr->status    = status;
        mmgsdi_resp_ptr->data_type = data_type;

        memscpy(&mmgsdi_resp_ptr->data,sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

        if ( data_ptr->session_open_ext_cnf.num_sessions > PBM_SESSION_MAX )
        {
          UIM_MSG_ERR_0("Error :Number of sessions are greater than PBM_SESSION_MAX ");
        }

        memscpy(session_open_info,len, data_ptr->session_open_ext_cnf.session_info_ptr, len);
        mmgsdi_resp_ptr->data.session_open_ext_cnf.session_info_ptr = session_open_info;

        pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));

        sent_success = TRUE;
      }
      else
      {
        UIM_MSG_ERR_0( "Unable to allocate session_info_ptr");
        PBM_MEM_FREEIF(mmgsdi_resp_ptr);
      }
    }
    else
    {
      UIM_MSG_ERR_0( "Unable to allocate mmgsdi_resp_ptr");
    }
  }
  else
  {
    UIM_MSG_ERR_0( "NULL ptr");
  }

  if ( !sent_success )
  {
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
  }
}

/*===========================================================================
FUNCTION pbm_open_session_cb

DESCRIPTION
  PBM internal function in response to OPEN SESSION callback from MMGSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_open_session_cb (mmgsdi_return_enum_type status,
                          mmgsdi_cnf_enum_type    data_type,
                          mmgsdi_cnf_type  *data_ptr)
{
    pbm_slot_type slot_id;
    mmgsdi_session_id_type session_id;
    mmgsdi_app_enum_type app_type;
    mmgsdi_session_type_enum_type mmgsdi_session_type;
    uint16 i = 0;

    /* data_ptr is NULL for ICC card where open_session returns success */
    PBM_CHECK_PTR_RET(data_ptr, VOID);

    if ( status == MMGSDI_SUCCESS )
    {
        if (data_type == MMGSDI_SESSION_OPEN_EXT_CNF)
        {
          for (i = 0; i < data_ptr->session_open_ext_cnf.num_sessions; i++)
          {
            slot_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT
               (data_ptr->session_open_ext_cnf.session_info_ptr[i].slot_id);
            session_id = data_ptr->session_open_ext_cnf.session_info_ptr[i].session_id;;
            app_type = data_ptr->session_open_ext_cnf.session_info_ptr[i].app_info.app_type;
            mmgsdi_session_type = data_ptr->session_open_ext_cnf.session_info_ptr[i].session_type;
            switch (mmgsdi_session_type)
            {
                case MMGSDI_CARD_SESSION_SLOT_1:
                                    //app_type would be none for card session
                pbm_open_session_update_session_info(PBM_SESSION_GPB_1, session_id, MMGSDI_APP_NONE, slot_id);
                break;
                case MMGSDI_GW_PROV_PRI_SESSION:
                pbm_open_session_update_session_info(PBM_SESSION_LPB_GW, session_id, app_type, slot_id);
                break;
                case MMGSDI_1X_PROV_PRI_SESSION:
                pbm_open_session_update_session_info(PBM_SESSION_LPB_1X, session_id, app_type, slot_id);
                break;
#ifdef FEATURE_DUAL_SIM
                case MMGSDI_CARD_SESSION_SLOT_2:
                                    //app_type would be none for card session
                pbm_open_session_update_session_info(PBM_SESSION_GPB_2, session_id, MMGSDI_APP_NONE, slot_id);
                break;
                case MMGSDI_GW_PROV_SEC_SESSION:
                pbm_open_session_update_session_info(PBM_SESSION_LPB_GW_SECONDARY, session_id, app_type, slot_id);
                break;
                case MMGSDI_1X_PROV_SEC_SESSION:
                pbm_open_session_update_session_info(PBM_SESSION_LPB_1X_SECONDARY, session_id, app_type, slot_id);
                break;
#ifdef FEATURE_TRIPLE_SIM
               case MMGSDI_CARD_SESSION_SLOT_3:
               //app_type would be none for card session
               pbm_open_session_update_session_info(PBM_SESSION_GPB_3, session_id, MMGSDI_APP_NONE, slot_id);
               break;
               case MMGSDI_GW_PROV_TER_SESSION:
               pbm_open_session_update_session_info(PBM_SESSION_LPB_GW_TERTIARY, session_id, app_type, slot_id);
               break;
               case MMGSDI_1X_PROV_TER_SESSION:
               pbm_open_session_update_session_info(PBM_SESSION_LPB_1X_TERTIARY, session_id, app_type, slot_id);
               break;
#endif
#endif
                default:
                UIM_MSG_ERR_0("MMGSDI returned invalid session info");
                break;
            }
          }
          PBM_MEM_FREEIF(data_ptr->session_open_ext_cnf.session_info_ptr);
        }
    }
}


/*===========================================================================
FUNCTION pbm_session_phonebook_init

DESCRIPTION
  Starts cache building for a particular session

DEPENDENCIES
  app init complete

SIDE EFFECTS
===========================================================================*/
void pbm_session_phonebook_init(pbm_session_enum_type session_type)
{
   /* Sending PBM ready event to WMS with unique TxId */
   pbm_send_ready_event_to_wms(session_type);

   UIM_MSG_HIGH_4("pbm_session_phonebook_init session 0x%x restart 0x%x activated 0x%x isActive 0x%x",
                  session_type, pbm_session_restart, pbm_session_activated, pbm_session_info[session_type].isActive);
   PBM_VALIDATE_SESSION_RET(session_type,VOID);

   if ( FALSE == pbm_session_info[session_type].isActive )
   {
      /* Shouldn't be here - Failure */
      return;
   }

   /* though we clear the caches here we would need to
    * call pbm_clear_and_set_session_caches again at the end of this API
    * to set the status to PBM_STATUS_NOT_READY since it would overwritten by PBM_CACHE_INIT
    */
   pbm_clear_and_set_session_caches(session_type, PBM_STATUS_NOT_READY, FALSE);
   PBM_SESSION_SET(pbm_session_initializing, session_type);
   PBM_SESSION_SET(pbm_session_activated, session_type);
   rex_enter_crit_sect(&pbm_crit_sect);
   /* initialize the pbm usim path look up table by default for the session_type 
    * though we do it in pbm_usim_read_pbr ,
    * we might need do this here also since this will handle hotswap case with a 2G card
    */
   memscpy(pbm_usim_path_lookup_table[session_type],
           sizeof(pbm_usim_path_lookup_table[0][0])*PBM_NUM_FILES_IN_LOOKUP_TABLE,
           pbm_usim_path_lookup_table_default[0],
           sizeof(pbm_usim_path_lookup_table[0][0])*PBM_NUM_FILES_IN_LOOKUP_TABLE);
   PBM_CACHE_INIT(adn_cache,PBM_FILE_ADN,session_type);
   PBM_CACHE_INIT(sdn_cache,PBM_FILE_SDN,session_type);
   PBM_CACHE_INIT(msisdn_cache,PBM_FILE_MSISDN,session_type);
   PBM_CACHE_INIT(mbn_cache,PBM_FILE_MBN,session_type);
   PBM_CACHE_INIT(fdn_cache,PBM_FILE_FDN,session_type);
   PBM_CACHE_INIT(lnd_cache,PBM_FILE_LND,session_type);
   PBM_CACHE_INIT(grpname_cache,PBM_FILE_GAS,session_type);
   PBM_CACHE_INIT(aas_cache,PBM_FILE_AAS,session_type);
   PBM_CACHE_INIT(mbdn_cache,PBM_FILE_MBDN,session_type);
   PBM_CACHE_INIT(ici_cache,PBM_FILE_ICI,session_type);
   PBM_CACHE_INIT(oci_cache,PBM_FILE_OCI,session_type);

   rex_leave_crit_sect(&pbm_crit_sect);
   /* though we clear the caches above we would need to
    * call pbm_clear_and_set_session_caches again here
    * to set the status to PBM_STATUS_NOT_READY since it had overwritten by PBM_CACHE_INIT
    */
   pbm_clear_and_set_session_caches(session_type, PBM_STATUS_NOT_READY, FALSE);

   /* Register for FCN changes */
   pbm_mmgsdi_register_fcn(session_type);

   {
     mmgsdi_return_enum_type result = MMGSDI_ERROR;
     /* USIM and CSIM cases check for LPB support, 
      * if yes start cache initialization for LPB
      */
     if (USE_USIM_LOCAL_SUPPORT(session_type))
     {
       result = mmgsdi_session_is_service_available(pbm_session_info[session_type].session_id,
                                                      MMGSDI_USIM_SRV_LOCAL_PHONEBOOK,
                                                      pbm_mmgsdi_local_pb_srv_cb,
                                                      0);
     }
     else if (USE_CSIM_LOCAL_SUPPORT(session_type))
     {
       result = mmgsdi_session_is_service_available(pbm_session_info[session_type].session_id,
                                                    MMGSDI_CSIM_SRV_LOCAL_PHONEBOOK,
                                                    pbm_mmgsdi_local_pb_srv_cb,
                                                    0);
     }

     UIM_MSG_HIGH_1("PBM queried for local phonebook %d",result);
     if( result == MMGSDI_SUCCESS )
     {
       return ;
     }
   }
   /* Start Session initialization */
   pbm_init_fileinfo_internal(session_type);
}


/*===========================================================================
FUNCTION pbm_local_phonebook_check_cb

DESCRIPTION
  Checks the local phonebook support available on card or not

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_local_phonebook_check_cb(mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    data_type,
                                  const mmgsdi_cnf_type  *data_ptr)
{
   mmgsdi_session_id_type mmgsdi_session_id;
   pbm_session_enum_type pbm_session_id;

   UIM_MSG_HIGH_3("Local phonebook resp from MGMSDI: Status 0x%x srv_available 0x%x data_type 0x%x",
                   status,
                   data_ptr->srv_available_cnf.srv_available,
                   data_type);

   if ( MMGSDI_SRV_AVAILABLE_CNF != data_type )
   {
      return;
   }

   mmgsdi_session_id = data_ptr->srv_available_cnf.response_header.session_id;
   pbm_session_id = pbm_mmgsdi_session_to_pbm_session(mmgsdi_session_id);

   /* Check if the 1st bit in UST(denoted by data_ptr->srv_available_cnf.srv_available) is set*/
   if (!data_ptr->srv_available_cnf.srv_available)
   {
     /* local phonebook not supported */
     pbm_session_info[pbm_session_id].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
   }

   /* even 1st bit in UST is not we would still required to read like FDN,SDN etc ..
    * prepare cache for local phonebook available 
    */
   pbm_init_fileinfo_internal(pbm_session_id);

   return;
}


/*===========================================================================
FUNCTION PBM_MMGSDI_SERVICE_AVAILABLE_CB

DESCRIPTION
  This function is called by MMGSDI if service requested is available on card

PARAMETERS

SIDE EFFECTS

===========================================================================*/
void pbm_mmgsdi_service_available_cb (mmgsdi_return_enum_type status,
                                      mmgsdi_cnf_enum_type    data_type,
                                      const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   PBM_CHECK_PTR_RET(data_ptr, VOID);

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_SERVICE_INFO_CB;

   mmgsdi_resp_ptr->data_type = data_type;
   mmgsdi_resp_ptr->status = status;

   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}

/*===========================================================================
FUNCTION PBM_MMGSDI_LOCAL_PB_SRV_CB

DESCRIPTION
  This function is called by MMGSDI to check if service requested is available on card

PARAMETERS

SIDE EFFECTS

===========================================================================*/
void pbm_mmgsdi_local_pb_srv_cb (mmgsdi_return_enum_type status,
                                      mmgsdi_cnf_enum_type    data_type,
                                      const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   PBM_CHECK_PTR_RET(data_ptr, VOID);

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_LOCAL_PB_SRV_CB;

   mmgsdi_resp_ptr->data_type = data_type;
   mmgsdi_resp_ptr->status = status;

   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}

