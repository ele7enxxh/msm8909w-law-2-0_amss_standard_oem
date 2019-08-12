/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C A L L   M A N A G E R   M M G S D I   M O D U L E

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
  cmmmgsdi_cmals_mmgsdi_async_sim_write
  cmmmgsdi_cmals_mmgsdi_async_sim_read
  cm_mmgsdi_in_traffic_channel_w_mode
  cmmmgsdi_card_status_cb
  cmmmgsdi_client_id_reg_cb
  cmmmgsdi_init


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cm_init_after_task_start() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2008 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmmgsdi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/14   jvo     Fix compilation warning in cmmmgsdi_refresh_init()
12/04/13   jvo     Mainline FEATURE_MMODE_RPM
10/07/13   jvo     Replace FEATURE_UICC_RAT_INDICATOR_SUPPORT with
                   is_rat_bal flag
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
01/25/13   gm      FR 3059 - ONLINE optimization - skip duplicate subscription
09/20/12   sk      Manage the ok to proceed with refresh flags effectively for
                   different sessions
09/19/12   sg      Remove MMGSDI_RAT_MODE_PREF_TDS_ONLY
06/22/12   xs      Hold subscription unavailable and ok_to_proceed to GSDI until get DS
                   session termination confirmation
03/28/12   jh      Integrated RPM feature
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/12/12   jh      Modified MMGSDI Error fatal
02/13/12   qf      Map ef_rat tri-mode to CM_MODE_PREF_GWL
01/06/12   gm      Data synchronization changes for freefloating platform
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/13/11   rk      Fix lint issue
05/13/11   rn      Use Increase MMGSDI API for ACM files
04/21/11   sv      Correct Lint Errors
04/18/11   mp      Optimized cmmmgsdi_process_session_open_rpt to use heap
04/13/11   rn      Made ACM read a cyclic record read
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/11/11   rm      GSDI clean-up
02/28/11   rm      Fixing featurisation issue for LTE RAT-B
02/27/11   jqi     Fix the link error for 8K FUSION TYPE 2 ES2 release.
02/21/11   sv      Use the correct session_type in cmmmgsdi_read for MAIN
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/14/11   rm      LTE RAT-B changes
01/19/10   am      Removing gsdi_get_card_mode()
12/23/10   sv      Integrate DSDS feature
12/02/10   gm      Fix to compiler warnings- place a function under a feature.
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/08/10   sv      Queue SESSION_OPEN_RPT internally after session is opened
10/04/10   xs      Superlint critical/high error fixed
10/01/10   ak      Merged the Fusion code from Dev branch.
09/28/10   sv      Added handling for MMGSDI_ILLEGAL_SUBSCRIPTION_EVT
09/23/10   am      Adding check for app type before providing session info
09/08/10   np      Added LTE for in traffic channel mode
08/30/10   fj      Add support for LTE call control.
08/23/10   rm      Return false in case of wrong file access mode
08/16/10   sv      Added cm_mmgsdi_ssn_tbl_get_app_type_for_type
07/14/10   mh      Fixed compile warnings
07/11/10   am      Merging call control cb function registration APIs changes
06/15/10   sv      Updating handling for MMGSDI_CARD-INSERTED_EVT
05/24/10   sv      Updated callback functions to queue a rpt to CM
06/22/10   rm      Handle EF_RAT chnage when Subs available comes before ONLINE
                   during power-on
04/01/10   mj      Propagate refresh event session ID
03/24/10   mj      CSIM - set cmph_set_received_card_init_completed_evt
02/27/10   mj      CSIM - fix to access right MMGSDI event structure
01/11/10   mj      CSIM - GW uses Session based MMGSDI interfaces
08/13/09   mj      CSIM - Use session based MMGSDI interfaces
10/14/09   rm      Consider EF_RAT mode between 0x03 to 0x0F as automatic
09/23/09   sv      KW Fixes
09/14/09   am      Including check for cm_ph_aoc_no_funds()
09/09/09   am      Upating aoc_ready only as per the status of ACM and
                   ACM_MAX files read status.
08/20/09   rm      Read EF_RAT file during power-on only if ENS flag in On.
08/07/09   rn/rm   Centralised call control changes
08/07/09   am      Adding cmmmgsdi_get_all_pin_status_cb and handling of
                   card events and PIN events for subscription handlig in CM
19/23/09   am      Correcting MMGSDI_USIM_ACM_MAX with MMGSDI_GSM_ACM_MAX
06/19/09   rm      CM should vote PASS to refresh_complte for GSM_ACM
                   and GSM_ACM_MAX files while RATB feature is on
06/10/09   rm      CM should not error fatal for non-EF_RAT refresh.
                   Also CM should handle MMGSDI_REFRESH_NAA_INIT_FCN
05/26/09   rm      Changing signature of cmmmgsdi_rat_mode_init
04/07/09   am      fixing memset error.
03/31/09   rm      Updating correct number of files to read in
                   cmmmgsdi_get_files_used_incall
03/24/09   sv      updated to cmtaski.h
02/25/09   sv      Correct Lint errors
02/20/09   rm      Lint clean post CMI changes
02/13/09   rm      Putting cmmmgsdi_is_refresh_list_used_by_cm under
                   FEATURE_GSTK
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
01/08/09   rm      Reading EF_RAT contents in 'transparent' mode
12/17/08   jd      Fixed Lint Errors
12/08/08   am      Fixing Compilation Warning
11/25/08   am      Adding HIGH messages to trace the MMGSDI Refresh and
                   Card Status events.
11/25/08   am      Main lining, feature for 1X voting from MMGSDI
11/25/08   am      Fixing compilation warnings
11/21/08   am      Putting file_info and no_option under feature and
                   defining cmmmgsdi_dummy() to get rid of compiler warnings
11/21/08   am      Fixing compilation error for Running off-target tests
11/18/08   am      fixing RAT balancing - Honor EF_RAT file changes while
                   power up on MMGSDI side.
11/17/08   am      Using MMGSDI API in CM.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "cmidbg.h"
#include "cmph.h"
/*lint -esym(766,cmtaski.h)*/
#include "cmtaski.h"
#include "cmmmgsdi.h"
#include "cmclnup.h"
#include "cmdbg.h"
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cmxll.h"
#endif /*  if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
#include "cmwaoc.h"
#endif  /* #if (defined CM_GW_SUPPORTED || (defined FEATURE_CM_LTE)*/

#include "cmcc.h"
#include "cm_v.h"
#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib_common.h"
#include "mmgsdisessionlib_v.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#include "cmrpm.h"
#include "cmcall.h"
#ifdef CM_DEBUG
#error code not present
#endif

#ifdef FEATURE_MMODE_RPM_TDSCDMA
#include "cmrpm_tds.h"
#endif

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
**      Static data
**---------------------------------------------------------------------------
*/
/* in_CS for All tasks */
static mmgsdi_client_id_type   cm_mmgsdi_client_id = 0;


mmgsdi_data_type  cm_mmgsdi_read_cnf_ptr;

#ifdef CM_GW_SUPPORTED
static boolean           cm_mmgsdi_write_cnf;
static boolean           cm_mmgsdi_increase_cnf;
#endif /* defined CM_GW_SUPPORTED) */

#ifdef FEATURE_MMGSDI_SESSION_LIB

/* in_CS for All tasks: Array and objects */
static cm_mmgsdi_session_id_table_entry_s_type cm_mmgsdi_session_id_table[] =
{ {MMGSDI_GW_PROV_PRI_SESSION}, /*lint !e785 */
  {MMGSDI_1X_PROV_PRI_SESSION}, /*lint !e785 */
  {MMGSDI_GW_PROV_SEC_SESSION}, /*lint !e785 */
  {MMGSDI_1X_PROV_SEC_SESSION}, /*lint !e785 */
  {MMGSDI_GW_PROV_TER_SESSION}, /*lint !e785 */
  {MMGSDI_1X_PROV_TER_SESSION}, /*lint !e785 */
};

static cm_mmgsdi_card_inserted_info_s_type cm_mmgsdi_iccid_table[MAX_AS_IDS];

#ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
/* Size of this array always be same as number of elements of cm_mmgsdi_session_id_table
    Tracks the waiting to proceed with refresh status of the mmgsdi sessions*/
static boolean
cmmmgsdi_waiting_to_proceed_with_refresh_arr[ARR_SIZE(cm_mmgsdi_session_id_table)]={0};
static void cm_mmgsdi_set_waiting_to_proceed_with_refresh(boolean is_refresh_required,int index);
#endif

/* in_CS for all tasks. */
static cm_mmgsdi_jcdma_card_info_s_type cm_mmgsdi_jcdma_info = {
  FALSE,
  MMGSDI_JCDMA_NON_JCDMA_CARD
};

#endif /*  FEATURE_MMGSDI_SESSION_LIB */
static word                          cm_auto_reg_imsi_mcc = 0;
static byte                          cm_auto_reg_imsi_mnc = 0;
static boolean                       cm_received_card_pwr_down_cnf = FALSE;

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*=========================================================================

FUNCTION
  cmmmgsdi_crit_sect_ptr

DESCRIPTION

  Critical section to protect shared global data related to CM and MMGSDI
  interaction.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static rex_crit_sect_type* cmmmgsdi_crit_sect_ptr(void)
{
  /* In future if there is a need to have separate lock for different
  ** global data of CM, then we just need to change this API with MMGSDI
  ** specific lock
  */
  return cmtask_crit_sect_ptr();
}

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
mmgsdi_client_id_type  cm_mmgsdi_get_client_id( void )
{
  mmgsdi_client_id_type   client_id;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_client_id (8bytes) */
  client_id = cm_mmgsdi_client_id;
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return client_id;
}  /* cm_mmgsdi_client_id_ptr() */


/*===========================================================================

FUNCTION NAME cmmmgsdi_read_cdma_imsi

DESCRIPTION

  Function that reads the IMSI from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_read_cdma_imsi
(
  void
)
{

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_error_e_type        cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_session_type_enum_type cm_mmgsdi_session_type =
                                                MMGSDI_1X_PROV_PRI_SESSION;
  mmgsdi_len_type               file_length_mmgsdi = 0;
  mmgsdi_session_id_type        cm_mmgsdi_session_id=0;
  mmgsdi_app_enum_type          app_type=MMGSDI_APP_NONE;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  byte                   cdma_imsi[CM_AUTO_REG_CT_CDMA_IMSI_LEN];
  boolean                status = FALSE;
  int32                  file_size = sizeof( cdma_imsi );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  #ifdef FEATURE_MMGSDI_SESSION_LIB

  /*
  ** Read the attributes for the Preferred PLMN With Access Technology List.
  */
  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(
                                     cm_mmgsdi_session_type,
                                     &cm_mmgsdi_session_id);

  app_type = cm_mmgsdi_ssn_tbl_get_app_type_for_type(cm_mmgsdi_session_type);

  CM_MSG_HIGH_2("cm_mmgsdi_status:%d,app_type:%d", cm_mmgsdi_status, app_type);

  if( (CM_MMGSDI_SUCCESS == cm_mmgsdi_status) && (app_type != MMGSDI_APP_NONE) )
  {
    if ( app_type == MMGSDI_APP_CSIM )
    {
      if(mmgsdi_session_read_cache_file_size(
                             cm_mmgsdi_session_id,
                             MMGSDI_CSIM_IMSI_M,
                             &file_length_mmgsdi) != MMGSDI_SUCCESS)
      {
        CM_ERR_1("Error in reading MMGSDI_CSIM_IMSI_M len: %d", file_length_mmgsdi);
        return FALSE;
      }
    }

    else if ( app_type == MMGSDI_APP_RUIM )
  {
    if(mmgsdi_session_read_cache_file_size(
                             cm_mmgsdi_session_id,
                             MMGSDI_CDMA_IMSI_M,
                             &file_length_mmgsdi) != MMGSDI_SUCCESS)
    {
      CM_ERR_1("Error in reading MMGSDI_CDMA_IMSI_M len: %d", file_length_mmgsdi );
      return FALSE;
    }
  }

    else
    {
      CM_ERR_1("Unsupported App Type for IMSI_M EF: %d", app_type);
      return FALSE;
    }
  }
  else
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
            cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */
    return FALSE;
  }
  /*
  ** Set the maximum file size for the Preferred PLMN With Access Technology
  ** List.
  */
  file_size = MIN(file_length_mmgsdi, file_size);
  #endif /* FEATURE_MMGSDI_SESSION_LIB */


  /*
  ** Read the Preferred PLMN List.
  */
  if ( app_type == MMGSDI_APP_RUIM )
  {
  status = cmmmgsdi_read( MMGSDI_1X_PROV_PRI_SESSION,
                          MMGSDI_CDMA_IMSI_M,
                          0,
                          file_size,
                          (uint8*)cdma_imsi,
                          sizeof(cdma_imsi),
                          transparent);
  }

  else if ( app_type == MMGSDI_APP_CSIM )
  {
    status = cmmmgsdi_read( MMGSDI_1X_PROV_PRI_SESSION,
                          MMGSDI_CSIM_IMSI_M,
                          0,
                          file_size,
                          (uint8*)cdma_imsi,
                          sizeof(cdma_imsi),
                          transparent);
  }

  else
  {
    CM_ERR_1("Unsupported App Type for IMSI_M EF: %d", app_type);
    return FALSE;
  }

  if(!status)
  {
    CM_ERR_0("cmmmgsdi_read() of IMSI_M EF failed ");
    return FALSE;
  }

  CM_MSG_HIGH_1("MMGSDI_CDMA_IMSI_M read - file_size:%d ", file_size);

  /* Decode MCC from IMSI */
  if( file_size == CM_AUTO_REG_CT_CDMA_IMSI_LEN )
  {
    /* IMSI Strucuture */
    /*
    H G F E  D C B A  octet
       Digit 2   Digit 1    1
       Digit 4   Digit 3    2
       Digit 6   Digit 5    3
       Digit 8   Digit 7    4
       Digit 10 Digit 9    5
       Digit 12 Digit 11  6
       Digit 14 Digit 13  7
       filler      Digit 15  8
       */

    /* Dump CDMA IMSI */

    {
      int i=0;
      for (i=0;i<CM_AUTO_REG_CT_CDMA_IMSI_LEN;i++)
      {
        CM_MSG_HIGH_2("MMGSDI_CDMA_IMSI_M read - IMSI[%d]:%d ", i, cdma_imsi[i]);
      }
    }

    /* Decode MCC - First 3 digits of IMSI */
    /*
    cm_auto_reg_imsi_mcc  = 100 * (cdma_imsi[0] & 0x0F);
    cm_auto_reg_imsi_mcc += 10 * ((cdma_imsi[0] & 0xF0) >> 4 );
    cm_auto_reg_imsi_mcc += (cdma_imsi[1] & 0x0F);
    */
    cm_auto_reg_imsi_mcc =  (cdma_imsi[CM_AUTO_REG_CT_CDMA_IMSI_LEN - 1] & 0x3);
    cm_auto_reg_imsi_mcc = (cm_auto_reg_imsi_mcc<< 8);
    cm_auto_reg_imsi_mcc |=  cdma_imsi[CM_AUTO_REG_CT_CDMA_IMSI_LEN - 2];
    cm_auto_reg_imsi_mnc =  cdma_imsi[CM_AUTO_REG_CT_CDMA_IMSI_LEN - 4] ;

    CM_MSG_HIGH_2("MMGSDI_CDMA_IMSI_M read - MCC %d MNC:%d ", cm_auto_reg_imsi_mcc, cm_auto_reg_imsi_mnc);

  }
  else
  {
    CM_ERR_1("Error in reading MMGSDI_CDMA_IMSI_M len: %d", file_size);
  }


  return status;
}  /* cmmmgsdi_read_pplmn_list() */


/*===========================================================================

FUNCTION NAME cmmmgsdi_get_imsi_mcc

DESCRIPTION

  Function that reads the IMSI from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
 word cmmmgsdi_get_imsi_mcc
(
  void
)
{
  return cm_auto_reg_imsi_mcc;
}

/*===========================================================================

FUNCTION NAME cmmmgsdi_get_imsi_mnc

DESCRIPTION

  Function that returns the mnc read from the SIM

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
 byte cmmmgsdi_get_imsi_mnc
(
  void
)
{
  return cm_auto_reg_imsi_mnc;
}

#ifdef FEATURE_MMGSDI_SESSION_LIB

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
cm_mmgsdi_session_id_table_entry_s_type*
                                       cm_mmgsdi_ssn_tbl_get_entry_by_type
(
  mmgsdi_session_type_enum_type session_type
)
{
  int index;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    if(cm_mmgsdi_session_id_table[index].session_type == session_type)
    {
      ssn_entry = &cm_mmgsdi_session_id_table[index];
      break;
    }
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return ssn_entry;
}

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_entry_by_id

DESCRIPTION

  Get session table entry matching given session_id.
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
cm_mmgsdi_session_id_table_entry_s_type*
                                           cm_mmgsdi_ssn_tbl_get_entry_by_id
(
  mmgsdi_session_id_type session_id
)
{
  int index;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */
  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    if(cm_mmgsdi_session_id_table[index].session_id == session_id)
    {
      ssn_entry = &cm_mmgsdi_session_id_table[index];
      break;
    }
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return ssn_entry;
}

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_entry_by_app_info_slot

DESCRIPTION

  Get session table entry matching given app_info_slot.
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
static cm_mmgsdi_session_id_table_entry_s_type*
                       cm_mmgsdi_ssn_tbl_get_entry_by_app_info_slot
(
  mmgsdi_slot_id_enum_type app_info_slot
)
{
  int index;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */
  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    if(cm_mmgsdi_session_id_table[index].app_info.slot == app_info_slot)
    {
      ssn_entry = &cm_mmgsdi_session_id_table[index];
      break;
    }
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return ssn_entry;
}

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_update_id_for_type

DESCRIPTION

  Set session ID for the specified session type.
  At most one session ID is associated with each session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_update_id_for_type
(
   mmgsdi_session_type_enum_type session_type,
   mmgsdi_session_id_type        session_id
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_type(session_type);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    ssn_entry->session_id = session_id;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

} /* cm_mmgsdi_ssn_tbl_update_id_for_type */

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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(session_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    ssn_entry->asubs_id = asubs_id;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;
}

/*====================================================================
FUNCTION cm_mmgsdi_ssn_tbl_update_app_info_for_id

DESCRIPTION

  Update App info for session entry matching given session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_update_app_info_for_id
(
  mmgsdi_session_id_type ssn_id,
    /* Session id for which session info needs to be updated
    */

  const mmgsdi_app_info_type *app_info
    /* Pointer to app info */
)
{

  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(ssn_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    memscpy( &ssn_entry->app_info, sizeof(ssn_entry->app_info), app_info, sizeof(mmgsdi_app_info_type) );
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;
}


/*====================================================================
FUNCTION cm_mmgsdi_ssn_tbl_update_act_status_for_id

DESCRIPTION

  Update App info for session entry matching given session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_update_act_status_for_id
(
  mmgsdi_session_id_type ssn_id,
    /* Session id for which session info needs to be updated
    */

  const mmgsdi_app_act_status_enum_type activation_status
    /* Pointer to app info */
)
{

  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */
  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(ssn_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    ssn_entry->act_status = activation_status;
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;
}


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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_type(session_type);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    ssn_entry->app_info.app_data.app_type = sess_app_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;
}

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_id_for_type

DESCRIPTION

  This function returns a session ID for the specified
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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_type(session_type);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    *session_id_ptr = ssn_entry->session_id;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

} /* cm_mmgsdi_ssn_tbl_get_id_for_type */

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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(session_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    ssn_info_ptr->session_id = ssn_entry->session_id;
    ssn_info_ptr->session_type = ssn_entry->session_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;
}

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_1x

DESCRIPTION

  Get session table entry matching given asubs_id for CSIM/RUIM app type.
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
static cm_mmgsdi_session_id_table_entry_s_type*
                       cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_1x
(
  sys_modem_as_id_e_type asubs_id

)
{
  int index;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */
  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    if((cm_mmgsdi_session_id_table[index].asubs_id == asubs_id) &&
       (cm_mmgsdi_session_id_table[index].app_info.app_data.app_type
                == MMGSDI_APP_RUIM ||
        cm_mmgsdi_session_id_table[index].app_info.app_data.app_type
                == MMGSDI_APP_CSIM))
    {
      ssn_entry = &cm_mmgsdi_session_id_table[index];
      break;
    }
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return ssn_entry;
}

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_csim

DESCRIPTION

  Get session table entry matching given asubs_id for CSIM app type.
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
static cm_mmgsdi_session_id_table_entry_s_type*
                       cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_csim
(
  sys_modem_as_id_e_type asubs_id

)
{
  int index;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */
  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    if((cm_mmgsdi_session_id_table[index].asubs_id == asubs_id) &&
       (cm_mmgsdi_session_id_table[index].app_info.app_data.app_type
                == MMGSDI_APP_CSIM))
    {
      ssn_entry = &cm_mmgsdi_session_id_table[index];
      break;
    }
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return ssn_entry;
}


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
boolean cmmmgsdi_is_switch_allowed_per_csim_self_activation
(
  sys_modem_as_id_e_type asubs_id
)
{
  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;
  boolean ret_val = TRUE;
  
  if(ph_sub_config_ptr->self_activation_support == TRUE)
  {    
    ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_csim(asubs_id);
    if(ssn_entry  != NULL)
    {
      switch(ssn_entry->act_status)
      {
        case MMGSDI_ACT_STATUS_NOT_ACTIVATED:
        ret_val = FALSE;
        break;
        default:
        break;
      }
      CM_MSG_HIGH_2("act_status %d for CSIM APP, ret_val %d",ssn_entry->act_status, ret_val);
    }    
  }  
  
  return ret_val;
}



/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_gw

DESCRIPTION

  Get session table entry matching given asubs_id for USIM/SIM app type.
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
static cm_mmgsdi_session_id_table_entry_s_type*
                       cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_gw
(
  sys_modem_as_id_e_type asubs_id

)
{
  int index;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */
  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    if((cm_mmgsdi_session_id_table[index].asubs_id == asubs_id) &&
       (cm_mmgsdi_session_id_table[index].app_info.app_data.app_type
                == MMGSDI_APP_SIM||
        cm_mmgsdi_session_id_table[index].app_info.app_data.app_type
                == MMGSDI_APP_USIM))
    {
      ssn_entry = &cm_mmgsdi_session_id_table[index];
      break;
    }
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return ssn_entry;
}

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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_1x(asubs_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;

    subs_info_ptr->app_id_1x.data_len = memscpy( subs_info_ptr->app_id_1x.data_ptr,
                                                 sizeof(subs_info_ptr->app_id_1x.data_ptr),
                                                 ssn_entry->app_info.app_data.aid.data_ptr,
                                                 ssn_entry->app_info.app_data.aid.data_len );


    memscpy( subs_info_ptr->uicc_id,
             sizeof(subs_info_ptr->uicc_id),
             ssn_entry->uicc_id,
             sizeof(subs_info_ptr->uicc_id));

    subs_info_ptr->session_type_1x  = ssn_entry->session_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

}

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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_gw(asubs_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    subs_info_ptr->app_id_gw.data_len = (int32)MIN(
                                  ARR_SIZE(subs_info_ptr->app_id_gw.data_ptr),
                                  ssn_entry->app_info.app_data.aid.data_len);

    subs_info_ptr->app_id_gw.data_len = memscpy( subs_info_ptr->app_id_gw.data_ptr,
                                                 sizeof(subs_info_ptr->app_id_gw.data_ptr),
                                                 ssn_entry->app_info.app_data.aid.data_ptr,
                                                 (uint8)ssn_entry->app_info.app_data.aid.data_len );

    memscpy( subs_info_ptr->uicc_id,
             sizeof(subs_info_ptr->uicc_id),
             ssn_entry->uicc_id,
             sizeof(subs_info_ptr->uicc_id));

    subs_info_ptr->session_type_gw  = ssn_entry->session_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

}

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_1x

DESCRIPTION

  This function returns a session type for specified
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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_1x(asubs_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    *session_type_ptr = ssn_entry->session_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

} /* cm_mmgsdi_ssn_tbl_get_type_for_asubs_id */

/*====================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw

DESCRIPTION

  This function returns a session type for specified
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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_asubs_id_gw(asubs_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    *session_type_ptr = ssn_entry->session_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

} /* cm_mmgsdi_ssn_tbl_get_type_for_asubs_id */

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
)
{
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;
  mmgsdi_app_enum_type                     app_type = MMGSDI_APP_NONE;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_type(session_type);
  if(ssn_entry != NULL)
  {
    app_type = ssn_entry->app_info.app_data.app_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return app_type;
}

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
)
{
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;
  mmgsdi_slot_id_enum_type                 slot_id = MMGSDI_MAX_SLOT_ID_ENUM;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_type(session_type);
  if(ssn_entry != NULL)
  {
    slot_id = ssn_entry->app_info.slot;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return slot_id;

} /* cm_mmgsdi_ssn_tbl_get_slot_for_type */

/*===========================================================================

FUNCTION cm_mmgsdi_ssn_tbl_get_ssn_info_for_slot

DESCRIPTION

  This function returns session info for the specified
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
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_app_info_slot(slot_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    sess_info_ptr->session_id = ssn_entry->session_id;
    sess_info_ptr->session_type = ssn_entry->session_type;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

} /* cm_mmgsdi_ssn_tbl_get_ssn_info_for_slot */

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

)
{
   int index;
   cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;

   rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
   /* Serialize data access cm_mmgsdi_session_id_table[] */
   for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
   {
      if ( (slot_id == cm_mmgsdi_session_id_table[index].app_info.slot) &&
           (cm_mmgsdi_session_id_table[index].app_info.app_data.app_type==MMGSDI_APP_SIM ||
            cm_mmgsdi_session_id_table[index].app_info.app_data.app_type==MMGSDI_APP_USIM))
      {
        sess_info_ptr->session_id =
                                cm_mmgsdi_session_id_table[index].session_id;
        sess_info_ptr->session_type
                            = cm_mmgsdi_session_id_table[index].session_type;
        result = CM_MMGSDI_SUCCESS;
        break;
      }
   }
   rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

   return result;

} /* cm_mmgsdi_get_session_info_for_gw */

/*===========================================================================

FUNCTION cm_mmgsdi_get_session_info_for_cdma

DESCRIPTION

  This function returns a session info for the specified
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
)
{
   unsigned int index;
   cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;

   rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
   /* Serialize data access cm_mmgsdi_session_id_table[] */
   for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
   {
      if ( (slot_id == cm_mmgsdi_session_id_table[index].app_info.slot) &&
            (cm_mmgsdi_session_id_table[index].app_info.app_data.app_type==MMGSDI_APP_CSIM ||
            cm_mmgsdi_session_id_table[index].app_info.app_data.app_type==MMGSDI_APP_RUIM))
      {
        sess_info_ptr->session_id =
                                cm_mmgsdi_session_id_table[index].session_id;
        sess_info_ptr->session_type =
                                cm_mmgsdi_session_id_table[index].session_type;

        result = CM_MMGSDI_SUCCESS;
        break;
      }
   }
   rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

   return result;

} /* cm_mmgsdi_ssn_tbl_get_ssn_info_for_cdma_slot */

#ifdef FEATURE_MMODE_DUAL_SIM
/*====================================================================
FUNCTION cm_mmgsdi_ssn_tbl_update_uicc_for_id

DESCRIPTION

  Update UICC for session entry matching given session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_update_uicc_for_id
(
  mmgsdi_session_id_type    session_id,
  const uint8               *uicc_id_ptr,
  uint32                    uicc_id_len
)
{
  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(session_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    memscpy( ssn_entry->uicc_id, sizeof(ssn_entry->uicc_id), uicc_id_ptr, uicc_id_len );
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;

} /* cm_mmgsdi_ssn_tbl_update_uicc_for_id */
#endif /* FEATURE_MMODE_DUAL_SIM */


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
)
{
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(session_id);
  if(ssn_entry != NULL)
  {
    subs_id = ssn_entry->asubs_id;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return subs_id;

} /* cm_mmgsdi_ssn_tbl_get_subs_id_for_id */


#endif /* FEATURE_MMGSDI_SESSION_LIB */

#ifdef FEATURE_GSTK
/*==========================================================================

FUNCTION CMMMGSDI_GET_FILES_USED_INCALL

DESCRIPTION
  Returns the list of SIM files used by CM in a call

RETURN VALUE
  Pointer to list of SIM files.
  Length of SIM file list.

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(528, cmmmgsdi_get_files_used_incall) */

static  void  cmmmgsdi_get_files_used_incall
(
        mmgsdi_refresh_file_list_type      *file_info_ptr
          /* File info */

)
{
  static mmgsdi_file_enum_type file_list[CM_MMGSDI_NUM_REGISTRED_FILES] = {
              MMGSDI_GSM_ACM,
              MMGSDI_GSM_ACM_MAX,
              MMGSDI_GSM_PUCT,
              MMGSDI_USIM_ACM,
              MMGSDI_USIM_ACM_MAX,
              MMGSDI_USIM_PUCT,
              MMGSDI_USIM_7F66_PROP1_RAT,
              MMGSDI_USIM_7F66_PROP1_RPM_EF,
              MMGSDI_USIM_7F66_PROP1_RPM_PARAM,
              MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR,
              MMGSDI_USIM_7F66_PROP1_RPM_CNTRS
              };

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  CM_ASSERT (file_info_ptr != NULL);

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  file_info_ptr->num_files = ARR_SIZE (file_list);

  file_info_ptr->file_list_ptr = file_list;

  return;

}  /* cmmmgsdi_get_files_used_incall () */

/*==========================================================================

FUNCTION CMMMGSDI_GET_FILES_FOR_REFRESH

DESCRIPTION
  Returns the list of SIM files that are registerd by CM
  for refresh notification.

RETURN VALUE
  Pointer to list of SIM files.
  Length of SIM file list.

DEPENDENCIES
  None
===========================================================================*/
static void cmmmgsdi_get_files_for_refresh
(
  mmgsdi_refresh_file_list_type      *file_info_ptr
    /* File info */

)
{
  static mmgsdi_file_enum_type file_list[CM_MMGSDI_NUM_REFRESH_FILES] = {
              MMGSDI_GSM_ACM,
              MMGSDI_GSM_ACM_MAX,
              MMGSDI_GSM_PUCT,
              MMGSDI_USIM_ACM,
              MMGSDI_USIM_ACM_MAX,
              MMGSDI_USIM_PUCT };

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  CM_ASSERT (file_info_ptr != NULL);

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  file_info_ptr->num_files = ARR_SIZE (file_list);

  file_info_ptr->file_list_ptr = file_list;

  return;

}  /* cmmmgsdi_get_files_for_refresh () */

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
void cmmmgsdi_generic_cb(
    mmgsdi_return_enum_type       status,
    mmgsdi_cnf_enum_type          cnf,
    const mmgsdi_cnf_type         *cnf_ptr
)
{
  cmph_s_type              *ph_ptr = cmph_ptr();
  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (status == MMGSDI_SUCCESS)
  {
    switch(cnf)
    {
      case MMGSDI_CARD_PUP_CNF:
        CM_MSG_HIGH_0("MMGSDI succesfull power-up");
        break;
      case MMGSDI_CARD_PDOWN_CNF:
        CM_MSG_HIGH_0("MMGSDI succesfull power-down");
        /* If OTASP commit is pending */
        /* If subscription is from card, reset the card */
        if ( ph_ptr->is_otasp_commit_pending && cmph_get_cdma_subs_from_card_sent())
        {
          cm_received_card_pwr_down_cnf = TRUE;
        }

        break;
      case MMGSDI_REFRESH_CNF:
        switch (cnf_ptr->refresh_cnf.orig_refresh_req)
        {
          case MMGSDI_REFRESH_ORIG_REG_REQ:
            CM_MSG_HIGH_2("MMGSDI suceed reg CM refsh evnt.Status %d, Confirmation %d",
                   status, cnf);
            break;

          case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
            CM_MSG_HIGH_2("Ok to refsh Ack succeed %d %d", cnf, status);
            break;

          default:
            CM_ERR_2("MMGSDI ret unexpected cnf.orig_refresh_req %d, Confirmation %d",
                   cnf_ptr->refresh_cnf.orig_refresh_req, cnf);
            break;
        }
        break;

      default:
        CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
               status, cnf);
        break;
    }
  }
  else /* status != MMGSDI_SUCCESS */
  {
    switch(cnf)
    {
      case MMGSDI_CARD_PUP_CNF:
        CM_ERR_0("MMGSDI power-up fail");
        break;
      case MMGSDI_CARD_PDOWN_CNF:
        CM_ERR_0("MMGSDI power-down fail");
        /* If OTASP commit is pending */
        /* If subscription is from card, reset the card */
        if ( ph_ptr->is_otasp_commit_pending && cmph_get_cdma_subs_from_card_sent())
        {
          cm_received_card_pwr_down_cnf = FALSE;
        }
        break;
      case MMGSDI_REFRESH_CNF:
        switch (cnf_ptr->refresh_cnf.orig_refresh_req)
        {
          case MMGSDI_REFRESH_ORIG_REG_REQ:
            CM_MSG_HIGH_2("MMGSDI failed reg CM refsh evnt.Status %d, Confirmation %d",
                   status, cnf);
            break;

          case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
            CM_MSG_HIGH_2("Ok to refresh Ack fail %d %d", cnf, status);
            break;

          default:
            CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
                   status, cnf);
            break;
        }
        break;

      default:
        CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
               status, cnf);
        break;
    }
  }  /* status != MMGSDI_SUCCESS */
}  /* cmmmgsdi_generic_cb() */
/*lint +esym(715,cnf)*/

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
  mmgsdi_session_id_type         session_id,
    /* session ID for the refresh event */
  #endif

  boolean                        ok_to_refresh_to_mmgsdi
)
{

  #ifndef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_option_type             no_option = {0};
  #else
  int index = 0;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  CM_MSG_HIGH_2("Session id %d, slot id %d", session_id, slot);
  
  if(!session_id)
  {
    rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
    /* Serialize data access cm_mmgsdi_session_id_table[] */
    for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
    {
      if ( slot == cm_mmgsdi_session_id_table[index].app_info.slot )
      {
        session_id = cm_mmgsdi_session_id_table[index].session_id;
        break;
      }
    }
    rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());
  }
  else /* else part will take care of proper value for index to be used later*/
  {
    rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
    /* Serialize data access cm_mmgsdi_session_id_table[] */
    for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
    {
      if ( session_id == cm_mmgsdi_session_id_table[index].session_id )
      {
        break;
      }
    }
    rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());
  }

  /* If index is invalid, then the rest of information extracted from
  ** this index is invalid. Hence, we should return here.
  */
  if(index >= ARR_SIZE(cm_mmgsdi_session_id_table))
  {
    CM_ERR_0("SIM_REF: Index for mmsgdi_session_id_table is out of bound");
    return;
  }

  if (!session_id)
  {
    CM_ERR_1("SIM_REF: Unable to get session id for slot %d",slot);
    return;
  }

  if(mmgsdi_session_ok_to_refresh( session_id,
                                   ok_to_refresh_to_mmgsdi,
                                   cmmmgsdi_generic_cb,
                                   0
                                 ) != MMGSDI_SUCCESS
    )
  #else
  if(mmgsdi_ok_to_refresh( cm_mmgsdi_get_client_id(),
                           slot,
                           ok_to_refresh_to_mmgsdi,
                           no_option,
                           cmmmgsdi_generic_cb,
                           0
                         ) != MMGSDI_SUCCESS
    )
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  {
    CM_ERR_0("SIM_REF: Fail put vote for ok to refresh in MMGSDI q");
  }

  CM_MSG_HIGH_1("SIM_REF: Voting %d for refresh index", ok_to_refresh_to_mmgsdi);

  #ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
  /* If sending ok to refresh, mark session id into waiting for proceed with refresh */
  if(ok_to_refresh_to_mmgsdi)
  {
    cm_mmgsdi_set_waiting_to_proceed_with_refresh(TRUE,index);
  }
  #endif

  SYS_ARG_NOT_USED(slot);

}  /* cmmmgsdi_ok_to_refresh() */

#endif /* #ifdef FEATURE_GSTK */

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
)
{
  int index = 0;
  cm_subscription_status_e_type cdma_subs_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type gw_subs_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type hybr_gw_subs_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type hybr_3_gw_subs_status = CM_SUBSCRIPTION_STATUS_NO_CHANGE;

  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    CM_MSG_HIGH_2("refresh_deact :cmmmgsdi_waiting_to_proceed_with_refresh_arr[%d] = %d",index,
                                                  cmmmgsdi_waiting_to_proceed_with_refresh_arr[index]);

    if ( session_id == cm_mmgsdi_session_id_table[index].session_id )
    {
      cm_mmgsdi_set_waiting_to_proceed_with_refresh(TRUE,index);
      break;
    }
  }

  if(index < ARR_SIZE(cm_mmgsdi_session_id_table))
  {

    CM_MSG_HIGH_2("refresh_deact: session_id=%d, session_type=%d",
                   session_id,cm_mmgsdi_session_id_table[index].session_type);

    /* MMGSDI informed to proceed with deactivation, send subs not available now */
    /* For 1x immediately inform proceed with refresh */
    if(cm_mmgsdi_session_id_table[index].session_type == MMGSDI_GW_PROV_PRI_SESSION)
    {
      gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
      cmph_set_gw_subs_from_card_sent( FALSE );
      cm_mmgsdi_set_waiting_to_proceed_with_refresh(TRUE,index);
    }
    else if(cm_mmgsdi_session_id_table[index].session_type == MMGSDI_1X_PROV_PRI_SESSION)
    {
      cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
      cmph_set_cdma_subs_from_card_sent( FALSE );
      /* Don't send ok_to_proceed. Wait for Ds confirmation of VSNCP teardown then send */
      cm_mmgsdi_set_waiting_to_proceed_with_refresh(TRUE,index);
    }
    else if(cm_mmgsdi_session_id_table[index].session_type == MMGSDI_GW_PROV_SEC_SESSION)
    {
      #if defined(FEATURE_MMODE_SXLTE_G)
      if (cmph_is_sxlte())
      {
        hybr_3_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmph_set_hybr_3_gw_subs_from_card_sent( FALSE );
      }
      else
      #endif
      {
        hybr_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        cmph_set_hybr_gw_subs_from_card_sent(FALSE);
      }
      cm_mmgsdi_set_waiting_to_proceed_with_refresh(TRUE,index);
    }
    else if(cm_mmgsdi_session_id_table[index].session_type == MMGSDI_GW_PROV_TER_SESSION)
    {
      hybr_3_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
      cm_mmgsdi_set_waiting_to_proceed_with_refresh(TRUE,index);
    }

    else
    {
      CM_ERR_1("Incorrect session id %x", session_id);
    }

    CM_MSG_HIGH_1("OK to REFRESH DEACTIVATE, send subscription not available. sess_type: %d",cm_mmgsdi_session_id_table[index].session_type);
  }

  cmph_send_subscription_not_available_internal( cdma_subs_status,
                                                 gw_subs_status,
                                                 hybr_gw_subs_status,
                                                 hybr_3_gw_subs_status,
                                                 CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_OTHERS);
}

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
void cmmmgsdi_proceed_with_refresh_subs(
         sys_modem_as_id_e_type asubs_id
)
{
  int index=0;
  const int tbl_size = sizeof(cm_mmgsdi_session_id_table)/sizeof(cm_mmgsdi_session_id_table_entry_s_type);

  if( cmclnup_is_ongoing_subsc(BM(asubs_id)) )
  {
    CM_MSG_HIGH_1("CLNUP: Proceed with refresh after cleanup on subs_id %d",asubs_id);
    return;
  }

  CM_MSG_HIGH_1("proceed_refresh, subs =%d",asubs_id);

  for(index = 0; index < tbl_size ; index++)
  {

    if(cm_mmgsdi_session_id_table[index].asubs_id == asubs_id &&
       cmmmgsdi_waiting_to_proceed_with_refresh_arr[index] == TRUE)
    {
      /* Send proceed with refresh to MMGSDI
      */
      if(mmgsdi_session_ok_to_proceed_with_refresh (
              cm_mmgsdi_session_id_table[index].session_id,
              TRUE,
              cmmmgsdi_generic_cb,
              0) !=  MMGSDI_SUCCESS)
      {
        CM_ERR_0("Fail put vote for ok to proceed in MMGSDI q");
      }

       CM_MSG_HIGH_1("OK_TO_PROCEED sent on index = %d",index);

      cm_mmgsdi_set_waiting_to_proceed_with_refresh(FALSE,index);
    }
  }
}


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
void cmmmgsdi_proceed_with_refresh(
         cm_ss_e_type ss
)
{
  int index=0;
  const int tbl_size = sizeof(cm_mmgsdi_session_id_table)/sizeof(cm_mmgsdi_session_id_table_entry_s_type);

  if( cmclnup_is_ongoing_subsc(BM(cmph_map_cm_ss_to_as_id(ss))))
  {
    CM_MSG_HIGH_1("CLNUP: Proceed with refresh after cleanup on ss %d",ss);
    return;
  }

  for(index = 0; index < tbl_size; index++)
  {
    mmgsdi_session_type_enum_type session_type = cm_mmgsdi_session_id_table[index].session_type;

    if ( (( (ss == CM_SS_MAIN ||( ss == CM_SS_HYBR_2  && cmph_is_sxlte()))&&
            (session_type == MMGSDI_1X_PROV_PRI_SESSION ||
             session_type ==MMGSDI_GW_PROV_PRI_SESSION) ) ||
          (ss == CM_SS_HYBR_2 && !cmph_is_sxlte() &&
           (session_type ==MMGSDI_GW_PROV_SEC_SESSION)) ||
          (ss == CM_SS_HYBR_3 &&
           (session_type ==MMGSDI_GW_PROV_TER_SESSION)) ||
          (ss == CM_SS_HYBR_3 && cmph_is_sxlte() &&
           (session_type ==MMGSDI_GW_PROV_SEC_SESSION))) &&
         cmmmgsdi_waiting_to_proceed_with_refresh_arr[index] == TRUE)
    {
      /* Send proceed with refresh to MMGSDI
      */
      if(mmgsdi_session_ok_to_proceed_with_refresh (
                  cm_mmgsdi_session_id_table[index].session_id,
                  TRUE,
                  cmmmgsdi_generic_cb,
                  0) !=  MMGSDI_SUCCESS)
      {
        CM_ERR_0("Fail put vote for ok to proceed in MMGSDI q");
      }
      CM_MSG_HIGH_1("OK_TO_PROCEED sent on index = %d",index);

      cm_mmgsdi_set_waiting_to_proceed_with_refresh(FALSE,index);
    }
  }
}

/*===========================================================================

FUNCTION cm_mmgsdi_is_waiting_to_proceed_with_refresh

DESCRIPTION

  Return .

DEPENDENCIES
  None

RETURN VALUE
  TRUE if cmmmgsdi_is_proceed_with_refresh_flag is TRUE else FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_mmgsdi_is_waiting_to_proceed_with_refresh(mmgsdi_session_type_enum_type session)
{
  int index = 0;

  for(index = 0; index < sizeof(cm_mmgsdi_session_id_table)/sizeof(cm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( session == cm_mmgsdi_session_id_table[index].session_type )
    {
      return cmmmgsdi_waiting_to_proceed_with_refresh_arr[index];
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cm_mmgsdi_set_proceed_with_refresh_flag

DESCRIPTION

  Sets the value for cmmmgsdi_waiting_to_proceed_with_refresh_arr as per
  input parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cm_mmgsdi_set_waiting_to_proceed_with_refresh(boolean is_refresh_required,int index)
{
  CM_MSG_HIGH_2( "cm_mmgsdi_set_waiting_to_proceed_with_refresh index =%d refresh_required=%d",
                 index,is_refresh_required );

  cmmmgsdi_waiting_to_proceed_with_refresh_arr[index] = is_refresh_required;  /*lint !e661 */
}

#endif


#ifdef FEATURE_MMGSDI_SESSION_LIB

/*===========================================================================

FUNCTION NAME cmmmgsdi_get_jcdma_card_info_complete_cb

DESCRIPTION
  Callback when GET JCDMA CARD INFO is complete

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
static void cmmmgsdi_wait_get_jcdma_card_info_complete_cb(
    mmgsdi_return_enum_type         status,
    mmgsdi_cnf_enum_type            cnf,
    const mmgsdi_cnf_type           *cnf_ptr
)
{
  boolean is_valid;
  mmgsdi_jcdma_card_info_enum_type jcdma_card_info;

  if (cnf_ptr == NULL || status != MMGSDI_SUCCESS || cnf != MMGSDI_JCDMA_GET_CARD_INFO_CNF)
  {
    is_valid = FALSE;
    jcdma_card_info = MMGSDI_JCDMA_NON_JCDMA_CARD;
    CM_MSG_HIGH_3("JCDMA info get failed: cnf_ptr %x status %d cnf %d", cnf_ptr, status, cnf);

  }
  else
  {
    CM_MSG_HIGH_1("JCDMA info: %d", cnf_ptr->get_jcdma_card_info_cnf.card_info);

    is_valid = TRUE;
    jcdma_card_info = cnf_ptr->get_jcdma_card_info_cnf.card_info;
  }

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_jcdma_info */
  cm_mmgsdi_jcdma_info.is_valid = is_valid;
  cm_mmgsdi_jcdma_info.jcdma_card_info = jcdma_card_info;
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  (void) rex_set_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


}  /* cmmmgsdi_get_jcdma_card_info_complete_cb() */

#endif

/*===========================================================================

FUNCTION NAME cmmmgsdi_wait_for_read_complete_cb

DESCRIPTION
  Function is called by MMGSDI when finishing read to SIM.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
static void cmmmgsdi_wait_for_read_complete_cb(
    mmgsdi_return_enum_type    status,
     /* Status of 'read' sent by MMGSDI */
    mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
    const mmgsdi_cnf_type      *cnf_ptr
     /* conf pointer returned by MMGSDI */
)
{
  CM_MSG_HIGH_2 ("cmmmgsdi_wait_for_read_complete_cb: cnf_ptr - %d status - %d ", \
               cnf_ptr, status );

  if(cnf_ptr == NULL || status != MMGSDI_SUCCESS)
  {
    cm_mmgsdi_read_cnf_ptr.data_len = 0;
    cm_mmgsdi_read_cnf_ptr.data_ptr = NULL;
  }
  else
  {
   
    CM_MSG_LOW_3 ("Values read from MMGSDI-> data_len: %x, data_ptr: %x,file_enum: %x",
                   cnf_ptr->read_cnf.read_data.data_len,
                   cnf_ptr->read_cnf.read_data.data_ptr,
                   cnf_ptr->read_cnf.access.file.file_enum);

    cm_mmgsdi_read_cnf_ptr.data_len = 0;

    CM_MSG_HIGH_2("cmmmgsdi_wait_for_read_complete_cb: cnf- %d,data_len - %d",
                    cnf, cnf_ptr->read_cnf.read_data.data_len);

    if((MMGSDI_READ_CNF == cnf) &&
       (cnf_ptr->read_cnf.read_data.data_len != 0))
    {
      /* malloc the pointer for mmgsdi cnf type and copy the info over. */
      cm_mmgsdi_read_cnf_ptr.data_len = cnf_ptr->read_cnf.read_data.data_len;
      cm_mmgsdi_read_cnf_ptr.data_ptr = /*lint !e737 */
      (uint8*)cm_mem_malloc( sizeof(uint8) * (cm_mmgsdi_read_cnf_ptr.data_len));

      memscpy( cm_mmgsdi_read_cnf_ptr.data_ptr, /*lint !e737 */
               sizeof(uint8) * (cm_mmgsdi_read_cnf_ptr.data_len),
               cnf_ptr->read_cnf.read_data.data_ptr, /*lint !e737 */
               sizeof(uint8) * cnf_ptr->read_cnf.read_data.data_len ); /*lint !e737 */

    }  /* if */
  }

  (void) rex_set_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


}  /* cmmmgsdi_wait_for_read_complete_cb() */


/*===========================================================================

FUNCTION NAME cmmmgsdi_wait_for_read_acl_complete_cb

DESCRIPTION
  Function is called by MMGSDI when finishing EF_ACL read to SIM.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(765, cmmmgsdi_wait_for_read_acl_complete_cb)
** Can't be static, unit test uses it
*/
void cmmmgsdi_wait_for_read_acl_complete_cb
(
    mmgsdi_return_enum_type    status,
     /* Status of 'read' sent by MMGSDI */
    mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
    const mmgsdi_cnf_type      *cnf_ptr
     /* conf pointer returned by MMGSDI */
)
{
  boolean acl_success = FALSE;
  mmgsdi_data_type data_buffer;
  cm_call_id_type  call_id;
  data_buffer.data_len = 0;
  data_buffer.data_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2 ("cmmmgsdi_wait_for_read_acl_complete_cb:cnf_ptr %x status %d ",cnf_ptr, status);

  if(cnf_ptr != NULL)
  {
    call_id = (cm_call_id_type)cnf_ptr->response_header.client_data;

    CM_MSG_HIGH_4( "ACL Values read - mmgsdi_cnf = %x, data_len- %x data_ptr-%x call_id-%d",
                   cnf,
                   cnf_ptr->read_cnf.read_data.data_len,
                   cnf_ptr->read_cnf.read_data.data_ptr,
                   call_id );
  }
  else
  {
    CM_ERR_0("NULL ptr from MMGSDI");
    return;
  }

  if(call_id > CMCALL_MAX_CALL_OBJ)
  {
    call_id = CMCALL_MAX_CALL_OBJ;
  }

  //if already timer expired we dont need process again
  if(cmmmgsdi_validate_sim_read_req(call_id))
  {
  if(status != MMGSDI_SUCCESS || cnf != MMGSDI_READ_CNF)
  {
    acl_success = FALSE;
  }
  else
  {
    if(cnf_ptr->read_cnf.read_data.data_len != 0)
    {
      /* malloc the pointer for mmgsdi cnf type and copy the info over. */
      data_buffer.data_ptr =
      (uint8*)cm_mem_malloc( (sizeof(uint8)) * (uint32)(cnf_ptr->read_cnf.read_data.data_len));

        acl_success = TRUE;

      data_buffer.data_len = memscpy( data_buffer.data_ptr,
                                      ( (sizeof(uint8)) * (uint32)(cnf_ptr->read_cnf.read_data.data_len)),
                                      cnf_ptr->read_cnf.read_data.data_ptr,
                                      sizeof(uint8) * (uint32)cnf_ptr->read_cnf.read_data.data_len );
    }/* if */
    else
    {
      acl_success = FALSE;
    }
  }
  cmcc_acl_read_complete((uint32)data_buffer.data_len,
                         data_buffer.data_ptr,
                         acl_success,
                           cmmmgsdi_ptr()->mmgsdi_read_req_uptime[call_id]->cc_object_ptr);

    cmmmgsdi_deallocate_sim_read_req(call_id);
  }
}  /* cmmmgsdi_wait_for_read_complete_cb() */


#ifdef FEATURE_MMGSDI_SESSION_LIB
/*==========================================================================

FUNCTION CMMMGSDI_GET_JCDMA_CARD_INFO

DESCRIPTION
  Get JCDMA card info

RETURN VALUE
  NONE

DEPENDENCIES
  None
==========================================================================*/

static boolean cmmmgsdi_get_jcdma_card_info(
                           mmgsdi_session_type_enum_type session_type,
                           mmgsdi_jcdma_card_info_enum_type *jcdma_card_info_ptr)
{
  mmgsdi_return_enum_type  status = MMGSDI_ERROR;
    /* To store the calling success status for MMGSDI API */

  mmgsdi_session_id_type   cm_mmgsdi_session_id;

  boolean is_jcdma_info_available = FALSE;

  if (cm_mmgsdi_ssn_tbl_get_id_for_type(session_type, &cm_mmgsdi_session_id)
      != CM_MMGSDI_SUCCESS)
  {
    CM_MSG_HIGH_0("Unable to get session id");
    return FALSE;
  }

  status = mmgsdi_session_jcdma_get_card_info( cm_mmgsdi_session_id,
                                    cmmmgsdi_wait_get_jcdma_card_info_complete_cb,
                                    0
                                    );
  if( status != MMGSDI_SUCCESS )
  {
    CM_MSG_HIGH_0("mmgsdi_session_jcdma_get_card_info() failed to be queued");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_wait( CM_MMGSDI_REPORT_SIG );  /* Wait for MMGSDI response */
  (void) rex_clr_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_jcdma_info */
  if(cm_mmgsdi_jcdma_info.is_valid)
  {
    *jcdma_card_info_ptr = cm_mmgsdi_jcdma_info.jcdma_card_info;
    is_jcdma_info_available = TRUE;
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return is_jcdma_info_available;

} /* cmmmgsdi_get_jcdma_card_info() */


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

boolean cmmmgsdi_is_jcdma_card_white_equiv(void)
{
  mmgsdi_jcdma_card_info_enum_type jcdma_card_info= MMGSDI_JCDMA_UNKNOWN_CARD;

  if(cmmmgsdi_get_jcdma_card_info(MMGSDI_1X_PROV_PRI_SESSION, &jcdma_card_info))
  {
    if(jcdma_card_info == MMGSDI_JCDMA_WHITE_CARD ||
       jcdma_card_info == MMGSDI_JCDMA_WHITE_BLACK_CARD ||
       jcdma_card_info == MMGSDI_JCDMA_BLACK_WHITE_CARD ||
       jcdma_card_info == MMGSDI_JCDMA_GRAY_WHITE_CARD)
    {
      CM_MSG_HIGH_0("JCDCMA card White or Wite equiv");
      return TRUE;
    }
  }
  return FALSE;
}

#endif
/*===========================================================================

FUNCTION NAME cmmmgsdi_allocate_sim_read_req

DESCRIPTION
  Function that allocates memory for storing MMGSDI sim read request.


RETURN VALUE
 Address of allocated buffer

DEPENDENCIES
  None
===========================================================================*/
cm_mmgsdi_read_uptime_s_type* cmmmgsdi_allocate_sim_read_req(cm_call_id_type call_id)
{
  cmmmgsdi_s_type           *mmgsdi_ptr = cmmmgsdi_ptr();

  cm_mmgsdi_read_uptime_s_type* mmgsdi_acl_read_req =
                     (cm_mmgsdi_read_uptime_s_type *)cm_mem_malloc(sizeof(cm_mmgsdi_read_uptime_s_type));

  /*Last object is reserved for supplementary services*/
  if(call_id > CMCALL_MAX_CALL_OBJ)
  {
    call_id = CMCALL_MAX_CALL_OBJ;
  }

  CM_MSG_HIGH_1("Allocating sim read req for call id:%d",call_id);

  mmgsdi_ptr->mmgsdi_read_req_uptime[call_id] = mmgsdi_acl_read_req;

  return mmgsdi_acl_read_req;
}

/*===========================================================================

FUNCTION NAME cmmmgsdi_deallocate_sim_read_req

DESCRIPTION
  Function that deallocates memory for storing MMGSDI sim read request.


RETURN VALUE
 None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_deallocate_sim_read_req(cm_call_id_type call_id)
{
  cmmmgsdi_s_type           *mmgsdi_ptr = cmmmgsdi_ptr();

  /*Last object is reserved for supplementary services*/
  if(call_id > CMCALL_MAX_CALL_OBJ)
  {
    call_id = CMCALL_MAX_CALL_OBJ;
  }

  CM_MSG_HIGH_1("Deallocating sim read req for call id:%d",call_id);

  if(mmgsdi_ptr->mmgsdi_read_req_uptime[call_id] != NULL)
  {
    cm_mem_free(mmgsdi_ptr->mmgsdi_read_req_uptime[call_id]);
    mmgsdi_ptr->mmgsdi_read_req_uptime[call_id] = NULL;
  }
}

/*===========================================================================

FUNCTION NAME cmmmgsdi_validate_sim_read_req

DESCRIPTION
  Function that validates if sim read req is still pending for the response


RETURN VALUE
 TRUE if yes

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_validate_sim_read_req(cm_call_id_type call_id)
{
  cmmmgsdi_s_type           *mmgsdi_ptr = cmmmgsdi_ptr();

  /*Last object is reserved for supplementary services*/
  if(call_id > CMCALL_MAX_CALL_OBJ)
  {
    call_id = CMCALL_MAX_CALL_OBJ;
  }

  if(mmgsdi_ptr->mmgsdi_read_req_uptime[call_id] == NULL)
  {
    CM_MSG_HIGH_1("No MMGSDI read req pending at index:%d",call_id);
    return FALSE;
  }
  else
  {
    return TRUE;
  }

}


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
boolean cm_wait_for_mmgsdi(void)
{
  rex_sigs_type sigs;
  cmmmgsdi_s_type           *mmgsdi_ptr = cmmmgsdi_ptr();

  (void)rex_set_timer(&mmgsdi_ptr->mmgsdi_abort_timer,CM_MMGSDI_ABORT_TIMER_VALUE_MILLI_SECS);
   /* Wait for MMGSDI response */
   sigs = cm_wait( CM_MMGSDI_ABORT_TIMER_SIG|CM_MMGSDI_REPORT_SIG);
   if( sigs & CM_MMGSDI_ABORT_TIMER_SIG && !(sigs & CM_MMGSDI_REPORT_SIG))
   {
       /* we didnt get response ,
       ** but the wait timer expired , hence bail out */
       CM_MSG_HIGH_1("SIMBUSY: MMGSDI wait failed %d",cmmmgsdi_ptr()->sim_busy_status.is_sim_busy);
       return FALSE;
     }
   else
   {
     (void) rex_clr_timer(&mmgsdi_ptr->mmgsdi_abort_timer);
   }

  (void) rex_clr_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );
   return TRUE;
}


/*===========================================================================

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
===========================================================================*/
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


)
{
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;
  mmgsdi_data_type mmgsdi_data;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type          cm_mmgsdi_session_id;
  cm_mmgsdi_error_e_type          cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_access_type              file_access;

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_name;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );

  #ifdef FEATURE_MMGSDI_SESSION_LIB

  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(session_type,
                                                  &cm_mmgsdi_session_id);

  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
            session_type, cm_mmgsdi_status); /*lint !e641 */

    return FALSE;
  }
  #else
  SYS_ARG_NOT_USED(session_type);
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  switch (file_structure)
  {
    case cyclic:
      #ifdef FEATURE_MMGSDI_SESSION_LIB
      status = mmgsdi_session_read_record(  cm_mmgsdi_session_id,
                                            file_access,
                                            record_number,
                                            request_length,
                                            cmmmgsdi_wait_for_read_complete_cb,
                                            0
                                            );
      #endif /* FEATURE_MMGSDI_SESSION_LIB */

      if (status != MMGSDI_SUCCESS)
      {
        return FALSE;
      }
      break;

    case transparent:
      #ifdef FEATURE_MMGSDI_SESSION_LIB
      if(file_name == MMGSDI_GSM_PLMN ||
        file_name == MMGSDI_GSM_PLMNWACT ||
        file_name == MMGSDI_USIM_PLMNWACT||
        file_name == MMGSDI_GSM_ACM_MAX ||
          file_name == MMGSDI_USIM_ACM_MAX
        )
    {

      cm_mmgsdi_read_cnf_ptr.data_len = request_length;
      cm_mmgsdi_read_cnf_ptr.data_ptr = data_buffer_p;
      status = mmgsdi_session_read_cache(cm_mmgsdi_session_id,
                                               file_name,
                                               cm_mmgsdi_read_cnf_ptr);

     CM_MSG_HIGH_1 ("mmgsdi_read_cache returned : %d ", status);
        if (status != MMGSDI_SUCCESS)
        {
          cm_mmgsdi_read_cnf_ptr.data_len = 0;
          cm_mmgsdi_read_cnf_ptr.data_ptr = NULL;
          return FALSE;
        }
     else
     {
          cm_mmgsdi_read_cnf_ptr.data_len = 0;
          cm_mmgsdi_read_cnf_ptr.data_ptr = NULL;
       return TRUE;
     }

    }
    else
    #endif
    {
      mmgsdi_data.data_ptr = data_buffer_p;
      if(mmgsdi_session_read_cache ( cm_mmgsdi_session_id,
                                                  file_access.file.file_enum,
                                                  mmgsdi_data
                                                  )  != MMGSDI_SUCCESS)
      {
          status = mmgsdi_session_read_transparent( cm_mmgsdi_session_id,
                                                file_access,
                                                0,
                                                request_length,
                                                cmmmgsdi_wait_for_read_complete_cb,
                                                0
                                                );
      }
      else
      {
        return TRUE;
      }

      CM_MSG_HIGH_1 ("mmgsdi_read_transparent returned : %d ", status);

      if (status != MMGSDI_SUCCESS)
      {
        return FALSE;
      }
      }
      break;

    default:
      CM_ERR_0("CMMMGSDI : Read req file is neither cyclic nor transparent");
      return FALSE;

  }  /* switch() */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cm_wait_for_mmgsdi() == FALSE)
  {
    return FALSE;
  }

  if (cm_mmgsdi_read_cnf_ptr.data_len == 0 &&
      cm_mmgsdi_read_cnf_ptr.data_ptr == NULL)
  {
    CM_MSG_HIGH_0 ("data_len and data_ptr are null, returning FALSE");
    return FALSE;
  }

  CM_ASSERT(cm_mmgsdi_read_cnf_ptr.data_len <= request_length);

  memscpy( data_buffer_p, size_data_buf,
           cm_mmgsdi_read_cnf_ptr.data_ptr, /*lint !e737 */
           sizeof(uint8) * cm_mmgsdi_read_cnf_ptr.data_len ); /*lint !e737 */

  cm_mem_free(cm_mmgsdi_read_cnf_ptr.data_ptr);
  cm_mmgsdi_read_cnf_ptr.data_ptr = NULL;
  cm_mmgsdi_read_cnf_ptr.data_len = 0;

  return TRUE;

}  /* cmmmgsdi_read() */


/*===========================================================================

FUNCTION NAME CMMMGSDI_READ_ACL_FILE

DESCRIPTION
  Function that reads Elementary File ACL (EF-ACL) from SIM.

RETURN VALUE
  boolean  Flag indicating whether the specified EF was read from the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_read_acl_file
(
    void*                             client_data
)
{
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;
  mmgsdi_data_type mmgsdi_data;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type           cm_mmgsdi_session_id;
  mmgsdi_session_type_enum_type    cm_mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
  cm_mmgsdi_error_e_type           cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_access_type               file_access;
  cmcc_object_s_type               *cc_object_ptr = (cmcc_object_s_type*)client_data;
  #ifdef FEATURE_MMODE_DUAL_SIM
    cmph_s_type                      *ph_ptr = cmph_ptr();
  #endif /* FEATURE_MMODE_DUAL_SIM */

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = MMGSDI_USIM_ACL;


  #ifdef FEATURE_MMODE_DUAL_SIM

    if(ph_ptr->standby_pref == SYS_MODEM_DS_PREF_SINGLE_STANDBY)
    {
      #ifdef FEATURE_MMODE_TRIPLE_SIM
      if (ph_ptr->active_subs & SYS_MODEM_AS_ID_3_MASK)
      {
        cm_mmgsdi_session_type = MMGSDI_GW_PROV_TER_SESSION;
      }
      else
      #endif /* FEATURE_MMODE_TRIPLE_SIM */
      if (ph_ptr->active_subs & SYS_MODEM_AS_ID_2_MASK)
      {
        cm_mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
      }
      CM_MSG_HIGH_2("Updated Session type: %d for active_sub: %d", cm_mmgsdi_session_type, cmph_ptr()->active_subs);
    }




    else
    {
       #ifdef FEATURE_MMODE_TRIPLE_SIM
       if (ph_ptr->default_data_subs == SYS_MODEM_AS_ID_3)
       {
         cm_mmgsdi_session_type = MMGSDI_GW_PROV_TER_SESSION;
       }
       else
       #endif /* FEATURE_MMODE_TRIPLE_SIM */
       if (ph_ptr->default_data_subs == SYS_MODEM_AS_ID_2)
       {
         cm_mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
       }
       CM_MSG_HIGH_2("Updated Session type: %d for default_data_sub: %d", cm_mmgsdi_session_type, cmph_ptr()->default_data_subs);
    }
  #endif /* FEATURE_MMODE_DUAL_SIM */
  #endif /* FEATURE_MMGSDI_SESSION_LIB */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(cm_mmgsdi_session_type,
                                                  &cm_mmgsdi_session_id);

  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
           cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */
  }

  mmgsdi_data.data_len = 0;
  mmgsdi_data.data_ptr = (uint8*)cm_mem_malloc(MMGSDI_MAX_DATA_BLOCK_LEN);

  if((status = mmgsdi_session_read_cache ( cm_mmgsdi_session_id,
                                              file_access.file.file_enum,
                                              mmgsdi_data
                                              ))  == MMGSDI_SUCCESS)
  {
    cmcc_acl_read_complete((uint32)mmgsdi_data.data_len,
                           mmgsdi_data.data_ptr,
                           TRUE,
                           client_data);

  }
  if(status != MMGSDI_SUCCESS)
  {
    cm_mmgsdi_read_uptime_s_type* mmgsdi_acl_read_req =
                            cmmmgsdi_allocate_sim_read_req(cc_object_ptr->call_id);
    if(mmgsdi_acl_read_req != NULL)
    {
    mmgsdi_acl_read_req->uptime = time_get_uptime_secs() + CM_MMGSDI_ABORT_TIMER_VALUE_SECS;
      mmgsdi_acl_read_req->cc_object_ptr = (void*)client_data;
    mmgsdi_acl_read_req->response_cb_ptr = (mmgsdi_callback_type)cmmmgsdi_wait_for_read_acl_complete_cb;
    }

  status = mmgsdi_session_read_transparent( cm_mmgsdi_session_id,
                                            file_access,
                                            0,
                                            0, /* 0 for entire file */
                                            cmmmgsdi_wait_for_read_acl_complete_cb,
                                            (uint32)cc_object_ptr->call_id
                                          );
  }


  CM_MSG_HIGH_1 ("mmgsdi_read_transparent acl returned : %d ", status);

  if (status != MMGSDI_SUCCESS)
  {
    cmmmgsdi_deallocate_sim_read_req(cc_object_ptr->call_id);
    return FALSE;
  }

  return TRUE;

}  /* cmmmgsdi_read_acl_file() */

#ifdef CM_GW_SUPPORTED
/*===========================================================================

FUNCTION pass_fail_ptr

DESCRIPTION

  Return a pointer to pass_fail, which tells whether CM MMGSDI refresh
  is completed successfully.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to pass_fail.

SIDE EFFECTS
  none

===========================================================================*/
static boolean  *pass_fail_ptr( void )
{
    static boolean pass_fail;

    /* - - - - - - - - - - - - - - - - - - - - - */

    return &pass_fail;
}

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

)
/*lint -esym(715,cm_client_id) */
{
  #ifdef FEATURE_GSTK
  boolean                              rat_mode_result = TRUE;
  cmmmgsdi_rat_mode_preference_e_type  rat_bal_new_mode_pref =
                                         MMGSDI_RAT_MODE_PREF_NONE;
  boolean                              rat_bal_is_lte_capable  = FALSE;
  #endif /* FEATURE_GSTK */

  uint32                         file_num_count = 0;
  cmph_s_type                    *ph_ptr = cmph_ptr();
    /* Get the phone object. */

  boolean                        *pass_fail_status;
  boolean                        is_refresh_complete = TRUE;

  cm_ph_charge_data_type         *phone_charge       = NULL;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_type_enum_type   session_type;
  #endif
  mmgsdi_app_enum_type app_type = MMGSDI_APP_NONE;

  boolean is_rpm_file_updated = FALSE;

  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    #if defined(FEATURE_MMODE_SXLTE_G)
    if (cmph_is_sxlte())
    {
      
      phone_charge = &ph_ptr->hybr_3_stack_info.phone_charge;
    #ifdef FEATURE_MMGSDI_SESSION_LIB
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
    #endif
  }
  else
    #endif
    {
      phone_charge = &ph_ptr->hybr_2_stack_info.phone_charge;
      #ifdef FEATURE_MMGSDI_SESSION_LIB
      session_type = MMGSDI_GW_PROV_SEC_SESSION;
      #endif
    }
  }
  else
    #if defined(FEATURE_MMODE_TRIPLE_SIM)
    if(asubs_id == SYS_MODEM_AS_ID_3)
    {
      phone_charge = &ph_ptr->hybr_3_stack_info.phone_charge;
      #ifdef FEATURE_MMGSDI_SESSION_LIB
        session_type = MMGSDI_GW_PROV_TER_SESSION;
      #endif
    }

  else
  #endif
  {
    phone_charge = &ph_ptr->main_stack_info.phone_charge;
    #ifdef FEATURE_MMGSDI_SESSION_LIB
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
    #endif
  }

  CM_MSG_HIGH_1("=dbg: phone charge %d",phone_charge->aoc_ready);

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  app_type = (mmgsdi_app_enum_type)
                cm_mmgsdi_ssn_tbl_get_app_type_for_type(session_type);

  if (app_type != MMGSDI_APP_SIM &&
      app_type != MMGSDI_APP_USIM )

  {
    CM_MSG_HIGH_1("Invalid or unknown app type : %x", app_type);
  }

  #ifdef FEATURE_GSTK

  if(ph_ptr->is_rat_bal == TRUE && app_type != MMGSDI_APP_USIM)
  {
    /* Currently not supported for RAT balancing feature */
    CM_MSG_HIGH_1("Invalid or unknown app type for MMGSDI_USIM_7F66_PROP1_RAT : %x",
                  app_type);
    /* Make rat_bal_new_mode_pref to default value */
    rat_bal_new_mode_pref = MMGSDI_RAT_MODE_PREF_NONE;
    rat_mode_result = FALSE;
  }
  #endif /* FEATURE_GSTK */

  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (file_num_count=0; file_num_count < refresh_file_list->num_files;
       file_num_count++)
  {
    switch (refresh_file_list->file_list_ptr[file_num_count])
    {
      case MMGSDI_USIM_ACM:
        if(app_type == MMGSDI_APP_USIM)
        {
          uint8 temp_value[CM_AOC_ACM_SIZE];
          if (cmmmgsdi_read(
                            session_type,
                             MMGSDI_USIM_ACM,
                             1,
                             CM_AOC_ACM_SIZE,
                             temp_value,
                             sizeof(temp_value),
                             cyclic)
             )
          {
            /* The ACM is stored in 3 bytes, with the first being the MSB */
            phone_charge->ACM = temp_value[0] << 16 | temp_value[1] << 8 |
                                temp_value[2] << 0;
            /* Set acm_read to TRUE */
            phone_charge->is_acm_read_success = TRUE;
            CM_MSG_HIGH_1("USIM_ACM read success. Val: %d",phone_charge->ACM);
          }
          else
          {
            /* Set acm_read to FALSE */
            phone_charge->is_acm_read_success = FALSE;
            CM_MSG_HIGH_0("MMGSDI_USIM_ACM read fail");
          }
          is_refresh_complete = is_refresh_complete & phone_charge->is_acm_read_success;
        }
        break;

      case MMGSDI_USIM_ACM_MAX:
        if(app_type== MMGSDI_APP_USIM)
        {
          uint8 temp_value[CM_AOC_ACM_SIZE];
          if (cmmmgsdi_read(
                             session_type,
                             MMGSDI_USIM_ACM_MAX,
                             0,
                             CM_AOC_ACM_SIZE,
                             temp_value,
                             sizeof(temp_value),
                             transparent
                             )
              )
          {
            phone_charge->ACMmax = temp_value[0] << 16 | temp_value[1] << 8 |
                                   temp_value[2] << 0;

            /* Set acm_max_read to TRUE */
            phone_charge->is_acm_max_read_success = TRUE;
            CM_MSG_HIGH_1("USIM_ACM_MAX read success. Val: %d",phone_charge->ACMmax);
          }
          else
          {
            /* Set acm_max_read to FALSE */
            phone_charge->is_acm_max_read_success = FALSE;
            CM_MSG_HIGH_0("MMGSDI_USIM_ACM_MAX read fail");
          }
          is_refresh_complete = is_refresh_complete & phone_charge->is_acm_max_read_success;
        }
        break;

      case MMGSDI_GSM_ACM:
        if(app_type== MMGSDI_APP_SIM)
        {
          uint8 temp_value[CM_AOC_ACM_SIZE];
          if (cmmmgsdi_read(
                            session_type,
                             MMGSDI_GSM_ACM,
                             1,
                             CM_AOC_ACM_SIZE,
                             temp_value,
                             sizeof(temp_value),
                             cyclic
                             )
             )
          {
            /* The ACM is stored in 3 bytes, with the first being the MSB */
            phone_charge->ACM = temp_value[0] << 16 | temp_value[1] << 8 |
                                temp_value[2] << 0;
            /* Set acm_read to TRUE */
            phone_charge->is_acm_read_success = TRUE;
            CM_MSG_HIGH_1("GSM_ACM read success. Val: %d",phone_charge->ACM);
          }
          else
          {
            /* Set acm_read to FALSE */
            phone_charge->is_acm_read_success = FALSE;
            CM_MSG_HIGH_0("MMGSDI_GSM_ACM read fail");
          }
          is_refresh_complete = is_refresh_complete & phone_charge->is_acm_read_success;
        }
        break;

      case MMGSDI_GSM_ACM_MAX:
        if(app_type== MMGSDI_APP_SIM)
        {
          uint8 temp_value[CM_AOC_ACM_SIZE];
          if( cmmmgsdi_read(
                            session_type,
                            MMGSDI_GSM_ACM_MAX,
                            0,
                            CM_AOC_ACM_SIZE,
                            temp_value,
                            sizeof(temp_value),
                            transparent
                            )
             )
          {
            phone_charge->ACMmax = temp_value[0] << 16 | temp_value[1] << 8 |
                                   temp_value[2] << 0;
            /* Set acm_max_read to TRUE */
            phone_charge->is_acm_max_read_success = TRUE;
            CM_MSG_HIGH_1("GSM_ACM_MAX read success. Val: %d",phone_charge->ACMmax);
          }
          else
          {
            /* Set acm_max_read to FALSE */
            phone_charge->is_acm_max_read_success = FALSE;
            CM_MSG_HIGH_0("MMGSDI_GSM_ACM_MAX read fail");
          }
          is_refresh_complete = is_refresh_complete & phone_charge->is_acm_max_read_success;
        }
        break;

      #ifdef FEATURE_GSTK
      case MMGSDI_USIM_7F66_PROP1_RAT:

        if (ph_ptr->is_rat_bal == TRUE && rat_mode_result == TRUE)
        {
          if (!cmmmgsdi_read(
                             session_type,
                              MMGSDI_USIM_7F66_PROP1_RAT,
                              0,
                              1,
                              (uint8*)&rat_bal_new_mode_pref,
                              sizeof(rat_bal_new_mode_pref),
                              transparent
                              )
             )
          {
            CM_MSG_HIGH_0("Unable to read MMGSDI_USIM_7F66_PROP1_RAT");

            /* Make new_mode_pref to default value */
            rat_bal_new_mode_pref = MMGSDI_RAT_MODE_PREF_NONE;
            rat_mode_result = FALSE;
          }
          else
          {
            /* Store the information in ph_ptr, that UIM_USIM_RAT file
            ** is present.
            */
            ph_ptr->is_uim_usim_rat_present = TRUE;

            /* Update mode_pref of the phone as present in EF_RAT */
            rat_bal_new_mode_pref = (cmmmgsdi_rat_mode_preference_e_type)
                            ((int)rat_bal_new_mode_pref & 0xff);

            CM_MSG_HIGH_1(" mode_pref in MMGSDI_USIM_7F66_PROP1_RAT: %d",
                          rat_bal_new_mode_pref);

            /* If EF_RAT mode_pref is not AUTO, GSM, WCDMA or LTE
            ** (between 0x05 to 0xFF),make it Dual or AUTOMATIC based
            ** on LTE capability of the phone.
            */

            /* Check if the UE supports LTE */
            #ifdef FEATURE_CM_LTE

            if ( !SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&ph_ptr->lte_band_capability))
            {
              rat_bal_is_lte_capable = TRUE;
              CM_MSG_HIGH_0 ("lte capable");
            }
            #endif

            if (rat_bal_is_lte_capable == TRUE)
            {
            if ( !BETWEEN (rat_bal_new_mode_pref, MMGSDI_RAT_MODE_PREF_NONE,
                           MMGSDI_RAT_MODE_PREF_MAX))
              {
                CM_MSG_HIGH_1("EF_RAT mode_pref changed to AUTOMATIC from %d",
                            rat_bal_new_mode_pref);
                rat_bal_new_mode_pref = MMGSDI_RAT_MODE_PREF_AUTOMATIC;
              }
            }
            else
            {
              if ( !BETWEEN (rat_bal_new_mode_pref, MMGSDI_RAT_MODE_PREF_NONE,
                             MMGSDI_RAT_MODE_PREF_AUTOMATIC))
              {
                 CM_MSG_HIGH_1("EF_RAT mode_pref changed to GSM_WCDMA from %d",
                          rat_bal_new_mode_pref);
                 rat_bal_new_mode_pref = MMGSDI_RAT_MODE_PREF_GSM_WCDMA;
              }
            }

            if(ph_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE)
            {
              cmmmgsdi_update_rat_mode(cm_client_id, rat_bal_new_mode_pref);
            }
          }
          is_refresh_complete = is_refresh_complete & rat_mode_result;
        } /* if (ph_ptr->is_rat_bal == TRUE && rat_mode_result == TRUE) */

        break;

      case MMGSDI_USIM_7F66_PROP1_RPM_PARAM:
      case MMGSDI_USIM_7F66_PROP1_RPM_EF:
      case MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR:
      case MMGSDI_USIM_7F66_PROP1_RPM_CNTRS:
      case MMGSDI_USIM_7F66_PROP1_RPM_VI:
        is_rpm_file_updated = TRUE;
        break;

      #endif /* FEATURE_GSTK */

      default:
        CM_ERR_1("MMGSDI File: %x not refsh CM",
                    refresh_file_list->file_list_ptr[file_num_count]);
        break;

    }  /* switch (refresh_file_list->... */
  }  /* for (file_num_count=0.. */

  /* If sim_card_mode is neither USIM nor GSM,
  ** is_acm_read_success and is_acm_max_read_success
  ** will not be get updated even though refresh and AOC is failed.
  ** Taking this scnerio into consideration seperately.
  */
  if (!(app_type == (mmgsdi_app_enum_type)MMGSDI_APP_SIM ||
        app_type == (mmgsdi_app_enum_type)MMGSDI_APP_USIM))
  {
    phone_charge->is_acm_max_read_success = FALSE;
    phone_charge->is_acm_read_success = FALSE;
    is_refresh_complete = FALSE;
  }

  /* Decide the value of ph_ptr->aoc_ready.
  ** AOC_READY's value is based on both ACM and ACM_MAX files.
  */
  if (phone_charge->is_acm_max_read_success == TRUE &&
      phone_charge->is_acm_read_success == TRUE)
  {
    phone_charge->aoc_ready = TRUE;
  }
  else
  {
    phone_charge->aoc_ready = FALSE;
  }

  CM_MSG_HIGH_2( "is_acm_read_success: %d is_acm_max_read_success: %d",
                 phone_charge->is_acm_read_success,
                 phone_charge->is_acm_max_read_success );

  CM_MSG_HIGH_2( "AOC ready- %d is_refresh_complete %d",
                 phone_charge->aoc_ready,
                 is_refresh_complete );

  /* Updating the static variable pass_fail
  ** This is returned as the status of refresh success to MMGSDI.
  */
  pass_fail_status = pass_fail_ptr();
  *pass_fail_status = is_refresh_complete;

  /* Check for no_funds */
  if ( phone_charge->aoc_ready &&
       phone_charge->ACMmax > 0 && (phone_charge->ACM >= phone_charge->ACMmax)
     )
  {
    cmph_aoc_no_funds(asubs_id);
  }

  if(is_rpm_file_updated)
  {
    cmrpm_process_gsdi_refresh(refresh_file_list);
  }
  SYS_ARG_NOT_USED(ss);

}  /* cmmmgsdi_refresh_files_read_proc ()*/
/*lint +esym(715,cm_client_id) */


/*===========================================================================

FUNCTION NAME cmmmgsdi_wait_for_write_complete_cb

DESCRIPTION
  Function is called when finishing write to SIM.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
static void cmmmgsdi_wait_for_write_complete_cb(
    mmgsdi_return_enum_type         status,
    mmgsdi_cnf_enum_type            cnf,
    const mmgsdi_cnf_type           *cnf_ptr
)
{
  if (cnf_ptr == NULL || status != MMGSDI_SUCCESS || cnf != MMGSDI_WRITE_CNF)
  {
    cm_mmgsdi_write_cnf = FALSE;
  }
  else
  {
    cm_mmgsdi_write_cnf = TRUE;
  }

  (void) rex_set_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


}  /* cmmmgsdi_wait_for_write_complete_cb() */


/*===========================================================================

FUNCTION NAME cmmmgsdi_wait_for_increase_complete_cb

DESCRIPTION
  Function is called when INCREASE update to SIM is completed.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
static void cmmmgsdi_wait_for_increase_complete_cb(
    mmgsdi_return_enum_type         status,
    mmgsdi_cnf_enum_type            cnf,
    const mmgsdi_cnf_type           *cnf_ptr
)
{
  if (cnf_ptr == NULL ||
      status != MMGSDI_SUCCESS ||
      cnf != MMGSDI_INCREASE_CNF
     )
  {
    cm_mmgsdi_increase_cnf = FALSE;
  }
  else
  {
    int i = 0;
    cm_mmgsdi_increase_cnf = TRUE;
    CM_MSG_HIGH_1("Increased data len %d", cnf_ptr->increase_cnf.increased_data.data_len);
    for(i=0; i < cnf_ptr->increase_cnf.increased_data.data_len; i++)
    {
      CM_MSG_HIGH_1("Increased data = %d", cnf_ptr->increase_cnf.increased_data.data_ptr[i]);
    }
  }
  (void) rex_set_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


}  /* cmmmgsdi_wait_for_increase_complete_cb() */

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
)
{
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type           session_id;
  mmgsdi_session_type_enum_type    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  cm_mmgsdi_error_e_type           mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_access_type               file_access;
  mmgsdi_callback_type             mmgsdi_callback = NULL;

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_name;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(cmmmgsdi_ptr()->sim_busy_status.is_sim_busy == FALSE)
  {
    mmgsdi_callback = cmmmgsdi_wait_for_write_complete_cb;
  }
  (void) rex_clr_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


  #ifdef FEATURE_MMGSDI_SESSION_LIB

  if (subs_type == CM_MMGSDI_SUBS_TYPE_1X)
  {
    mmgsdi_status = cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_1x(
                                            asubs_id,
                                            &session_type
                                          );
  }
  else if (subs_type == CM_MMGSDI_SUBS_TYPE_GW)
  {
    mmgsdi_status = cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw(
                                            asubs_id,
                                            &session_type
                                          );
  }
  else
  {
    CM_ERR_1("Invalid subs_type %d", subs_type);
  }

  if (CM_MMGSDI_SUCCESS != mmgsdi_status)
  {

    /* If RPM enabled SIM and write is for USIM RPM counters use the default
        ** session type MMGSDI_GW_PROV_PRI_SESSION.
        */
    if ((cmrpm_is_rpm_enabled() == TRUE) && (file_name == MMGSDI_USIM_7F66_PROP1_RPM_CNTRS))
    {
      session_type = MMGSDI_GW_PROV_PRI_SESSION;
    }
    else
    {

      CM_ERR_2("Unable to get session info for as-id: %d",
           session_type, mmgsdi_status); /*lint !e641 */

      return FALSE;
    }
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(session_type,
                                                       &session_id);
  if (CM_MMGSDI_SUCCESS != mmgsdi_status)
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
           session_type, mmgsdi_status); /*lint !e641 */
    return FALSE;
  }

  if(file_name == MMGSDI_GSM_ACM_MAX   ||
     file_name == MMGSDI_USIM_ACM_MAX  ||
     file_name == MMGSDI_GSM_ACM       ||
     file_name == MMGSDI_USIM_ACM )
  {
     /* Use cyclic file type */
     status = mmgsdi_session_write_record(session_id,
                                          file_access,
                                          MMGSDI_CYCLIC_FILE,
                                          1,
                                          write_data,
                                          mmgsdi_callback,
                                          NULL
                                          );
  }
  else if(file_name == MMGSDI_USIM_CFIS)
  {
    /* Use Linear fixed type for CFIS */
    status = mmgsdi_session_write_record(session_id,
                                         file_access,
                                         MMGSDI_LINEAR_FIXED_FILE,
                                         1,
                                         write_data,
                                         cmmmgsdi_wait_for_write_complete_cb,
                                         NULL
                                         );

  }
  else
  {

     status = mmgsdi_session_write_transparent(
                               session_id,
                               file_access,
                               0,
                               write_data,
                               mmgsdi_callback,
                               NULL
                               );
  }
  #else

  if(file_name == MMGSDI_GSM_ACM_MAX   ||
     file_name == MMGSDI_USIM_ACM_MAX  ||
     file_name == MMGSDI_GSM_ACM       ||
     file_name == MMGSDI_USIM_ACM      ||
     file_name == MMGSDI_USIM_CFIS)
  {
     /* Use cyclic file type */
     status = mmgsdi_write_record(cm_mmgsdi_get_client_id(),
                                  MMGSDI_SLOT_1,
                                  file_name,
                                  1,
                                  write_data,
                                  mmgsdi_callback,
                                  NULL
                                  );
  }
  else
  {
     status = mmgsdi_write_transparent(
                               cm_mmgsdi_get_client_id(),
                               MMGSDI_SLOT_1,
                               file_name,
                               0,
                               write_data,
                               mmgsdi_callback,
                               NULL
                               );
  }

  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  if (status != MMGSDI_SUCCESS)
  {
    return FALSE;
  }

  if(cmmmgsdi_ptr()->sim_busy_status.is_sim_busy == FALSE)
  {
    cm_wait( CM_MMGSDI_REPORT_SIG );  /* Wait for MMGSDI response */
    (void) rex_clr_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );
  }





  return cm_mmgsdi_write_cnf;

}  /* cmmmgsdi_write()*/


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
)
{
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type           cm_mmgsdi_session_id;
  mmgsdi_session_type_enum_type    cm_mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
  cm_mmgsdi_error_e_type           cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_access_type               file_access;

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_name;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  (void) rex_clr_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


  #ifdef FEATURE_MMGSDI_SESSION_LIB

  if (subs_type == CM_MMGSDI_SUBS_TYPE_1X)
  {
    cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_1x (
                                            asubs_id,
                                            &cm_mmgsdi_session_type
                                          );
  }
  else if (subs_type == CM_MMGSDI_SUBS_TYPE_GW)
  {
    cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw (
                                            asubs_id,
                                            &cm_mmgsdi_session_type
                                          );
  }
  else
  {
    CM_ERR_1("Invalid subs_type %d",subs_type);
    return FALSE;
  }

  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_2("Unable to get session info for as-id: %d",
           cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */
    return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(cm_mmgsdi_session_type,
                                                  &cm_mmgsdi_session_id);
  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
           cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */
    return FALSE;
  }

  status = mmgsdi_session_increase(cm_mmgsdi_session_id,
                                   file_access,
                                   increase_data,
                                   cmmmgsdi_wait_for_increase_complete_cb,
                                   0
                                   );
  #else

  CM_ERR_0("MMGSDI Non-Session based support not available!");

  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  if (status != MMGSDI_SUCCESS)
  {
    return FALSE;
  }

  if (cm_wait_for_mmgsdi() == FALSE)
  {
    return FALSE;
  }

  return cm_mmgsdi_increase_cnf;

}  /* cmmmgsdi_increase()*/


/*===========================================================================

FUNCTION NAME CMMMGSDI_READ_PPLMN_W_ACT_LIST

DESCRIPTION
  Function that reads User Preferred PLMN with Access Technology List from
  the SIM.

RETURN VALUE

  boolean  Flag indicating whether the User Preferred PLMN with Access
           Technology List was read from the SIM successfully (TRUE) or
           unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmmmgsdi_read_pplmn_w_act_list
(
  cm_ss_e_type                       ss,
  mmgsdi_app_enum_type               app_type,
  sys_user_pref_plmn_list_s_type     *user_pref_plmn_list_ptr,
  boolean                            is_packed_req
)
{
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_error_e_type        cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_session_type_enum_type cm_mmgsdi_session_type =
                                             MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_session_id_type        cm_mmgsdi_session_id;
  mmgsdi_len_type               file_length_mmgsdi = 0;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  cm_mmgsdi_plmn_w_act_s_type  pplmn_w_act_list[SYS_USER_PPLMN_LIST_MAX_LENGTH];
  int32                        file_size = 0;
  uim_items_enum_type          sim_filename;
  boolean                      status = FALSE;
  int32                        num_entries;
  int32                        i;
  mmgsdi_file_enum_type        sim_file_name_mmgsdi;

  mmgsdi_session_type_enum_type   session_type;
  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_pref_plmn_list_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    session_type = MMGSDI_GW_PROV_TER_SESSION;
  }
  else if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  file_size = sizeof( pplmn_w_act_list );

  /*
  ** Initialize local variables based upon the requested list category.
  */
  if ( app_type == MMGSDI_APP_SIM )
  {
    sim_filename = UIM_GSM_PLMNWACT;
  }
  else
  {
    sim_filename = UIM_USIM_PLMNWACT;
  }

  if ( sim_filename == UIM_GSM_PLMNWACT )
  {
    sim_file_name_mmgsdi = MMGSDI_GSM_PLMNWACT;
  }
  else
  {
    sim_file_name_mmgsdi = MMGSDI_USIM_PLMNWACT;
  }

  user_pref_plmn_list_ptr->length = 0;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  /*
  ** Read the attributes for the Preferred PLMN With Access Technology List.
  */
  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(
                                       cm_mmgsdi_session_type,
                                       &cm_mmgsdi_session_id);

  if(CM_MMGSDI_SUCCESS == cm_mmgsdi_status)
  {
    if(MMGSDI_SUCCESS != mmgsdi_session_read_cache_file_size(
                                                   cm_mmgsdi_session_id,
                                                   sim_file_name_mmgsdi,
                                                   &file_length_mmgsdi))
    {
      CM_ERR_1("Error in reading file len file: %d", sim_file_name_mmgsdi);
      return FALSE;
    }
  }
  else
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
            cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */
    return FALSE;
  }
  /*
  ** Set the maximum file size for the Preferred PLMN With Access Technology
  ** List.
  */
  file_size = MIN(file_length_mmgsdi, file_size);

  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /*
  ** Read the Preferred PLMN With Access Technology List.
  */
  status = cmmmgsdi_read(
                          session_type,
                          sim_file_name_mmgsdi,
                          0,
                          file_size,
                          (uint8*)pplmn_w_act_list,
                          sizeof(pplmn_w_act_list),
                          transparent);
  if(!status)
  {
    return FALSE;
  }

  /*
  ** Loop over the Preferred PLMN With Access Technology List and
  ** append each PLMN to the output Preferred PLMN List.A PLMN
  ** that supports both GSM and UMTS access technologies may be
  ** added to the list twice depending upon the mode preference.
  */
  num_entries = file_size / (int32) sizeof( cm_mmgsdi_plmn_w_act_s_type );

  for ( i = 0; i < num_entries; i++ )
  {
    /*
    ** Ignore any undefined PLMNs that might be included in the
    ** Preferred PLMN With Access Technology List and user has requested
    ** packed version of the file.
    */

    if ( !sys_plmn_id_is_undefined(pplmn_w_act_list[i].plmn) ||
         !is_packed_req )
    {
      user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech = 0;
      user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].plmn =
                        pplmn_w_act_list[i].plmn;

      if ( pplmn_w_act_list[i].act[1] & 0x80 )
      {
        user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech =
              user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech |
              SYS_ACT_GSM_MASK;
      }

      if ( pplmn_w_act_list[i].act[1] & 0x40 )
      {
        user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech =
              user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech |
              SYS_ACT_GSM_COMPACT_MASK;
      }

      if ( pplmn_w_act_list[i].act[0] & 0x80 )
      {
        user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech =
              user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech |
              SYS_ACT_UMTS_MASK;
      }
      user_pref_plmn_list_ptr->length++;
    }  /* if !sys_plmn_id_is_undefined() */
  }  /* for */

  return status;
}  /* cmmmgsdi_read_pplmn_w_act_list() */

/*===========================================================================

FUNCTION NAME CM_MMGSI_READ_PPLMN_LIST

DESCRIPTION

  Function that reads the Preferred PLMN List from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmmmgsdi_read_pplmn_list
(
  cm_ss_e_type                    ss,
  sys_user_pref_plmn_list_s_type  *user_pref_plmn_list_ptr,
  boolean                         is_packed_req
)
{
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_error_e_type        cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_session_type_enum_type cm_mmgsdi_session_type =
                                                MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_len_type               file_length_mmgsdi = 0;
  mmgsdi_session_id_type        cm_mmgsdi_session_id;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  sys_plmn_id_s_type     pplmn_list[SYS_USER_PPLMN_LIST_MAX_LENGTH];
  boolean                status = FALSE;
  int32                  file_size = sizeof( pplmn_list );
  int32                  i;
  int32                  num_entries;

  mmgsdi_session_type_enum_type   session_type;
  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_pref_plmn_list_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    session_type = MMGSDI_GW_PROV_TER_SESSION;
  }
  else if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  user_pref_plmn_list_ptr->length = 0;

  #ifdef FEATURE_MMGSDI_SESSION_LIB

  /*
  ** Read the attributes for the Preferred PLMN With Access Technology List.
  */
  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(
                                     cm_mmgsdi_session_type,
                                     &cm_mmgsdi_session_id);

  if(CM_MMGSDI_SUCCESS == cm_mmgsdi_status)
  {
    if(mmgsdi_session_read_cache_file_size(
                             cm_mmgsdi_session_id,
                             MMGSDI_GSM_PLMN,
                             &file_length_mmgsdi) != MMGSDI_SUCCESS)
    {
      CM_ERR_0("Error in reading MMGSDI_GSM_PLMN len: %d");
      return FALSE;
    }
  }
  else
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
            cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */
    return FALSE;
   }
  /*
  ** Set the maximum file size for the Preferred PLMN With Access Technology
  ** List.
  */
  file_size = MIN(file_length_mmgsdi, file_size);
  #endif /* FEATURE_MMGSDI_SESSION_LIB */


  /*
  ** Read the Preferred PLMN List.
  */
  status = cmmmgsdi_read(
                          session_type,
                          MMGSDI_GSM_PLMN,
                          0,
                          file_size,
                          (uint8*)pplmn_list,
                          sizeof(pplmn_list),
                          transparent
                          );

  if(!status)
  {
    return FALSE;
  }

  /*
  ** Loop over the Preferred PLMN List and append each PLMN to the
  ** output Preferred PLMN List. GSM is given priority over UMTS
  ** as specified in TS 23.122 Section 4.4.3.1.1.c.
  */
  num_entries = file_size / (int32) sizeof( sys_plmn_id_s_type );

  for ( i = 0; i < num_entries; i++ )
  {
    /*
    ** Include the PLMN only if it is not undefined and onlypacked
    ** records are not requested.
    */
    if ( !sys_plmn_id_is_undefined(pplmn_list[i]) || !is_packed_req )
    {
      user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].plmn =
            pplmn_list[i];

      user_pref_plmn_list_ptr->info[user_pref_plmn_list_ptr->length].access_tech = 0;

      user_pref_plmn_list_ptr->length++;

    }  /* if !sys_plmn_id_is_undefined() */
  }  /* for */

  SYS_ARG_NOT_USED(ss);

  return status;
}  /* cmmmgsdi_read_pplmn_list() */

/*==========================================================================

FUNCTION NAME CMMMGSDI_READ_PREFERRED_PLMN_LIST

DESCRIPTION
  Function that reads User Preferred PLMN List from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN With Access Technologty
           List was read from the SIM successfully (TRUE) or unsuccessfully
          (FALSE).

DEPENDENCIES
  None
===========================================================================*/
boolean cmmmgsdi_read_preferred_plmn_list
(
  cm_ss_e_type                    ss,

  sys_user_pref_plmn_list_s_type  *user_pref_plmn_list_ptr,

  boolean                         is_packed
    /* if is_packed is TRUE, we would remove holes.
    */
)
{
  boolean                       status    = FALSE;
  mmgsdi_session_type_enum_type  session_type;
  mmgsdi_app_enum_type           app_type;
  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
    #ifdef FEATURE_MMODE_TRIPLE_SIM
      if(asubs_id == SYS_MODEM_AS_ID_3)
      {
        session_type = MMGSDI_GW_PROV_TER_SESSION;
      }
      else
    #endif
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }

  /*lint -e{718}*/
  app_type = (mmgsdi_app_enum_type)
                        cm_mmgsdi_ssn_tbl_get_app_type_for_type(session_type);

  if (app_type != (mmgsdi_app_enum_type)MMGSDI_APP_SIM &&
      app_type != (mmgsdi_app_enum_type)MMGSDI_APP_USIM )
  {
    CM_ERR_1( "Invalid app type %d, read not performed",
             app_type);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Try to read the User Preferred PLMN with Access Technology List
  */
  if( cmmmgsdi_read_pplmn_w_act_list( ss, app_type, user_pref_plmn_list_ptr,
                                      is_packed))
  {
    return TRUE;
  }
  else
  {
    /*
    ** Try to read the User Preferred PLMN List
    */
    status = cmmmgsdi_read_pplmn_list(ss, user_pref_plmn_list_ptr,is_packed );

    if ( !status )
    {
      CM_ERR_0( "Cud not read user pref network list");
    }
  }

  return status;

} /* cmmmgsdi_read_preferred_plmn_list() */

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
)
{
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_error_e_type        cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_session_type_enum_type cm_mmgsdi_session_type =
                                                  MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_session_id_type        cm_mmgsdi_session_id;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  mmgsdi_len_type               file_length = 0;
  cm_mmgsdi_plmn_w_act_s_type  pplmn_w_act_list[SYS_USER_PPLMN_LIST_MAX_LENGTH];
  sys_plmn_id_s_type           pplmn_list[SYS_USER_PPLMN_LIST_MAX_LENGTH];
  boolean                      status = FALSE;
  int32                        i;
  int32                        num_entries;
  mmgsdi_write_data_type       write_data;
  mmgsdi_file_enum_type        MMGSDI_sim_filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_session_type_enum_type  session_type;
  mmgsdi_app_enum_type           app_type;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_pref_plmn_list_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
    #ifdef FEATURE_MMODE_TRIPLE_SIM
      if(asubs_id == SYS_MODEM_AS_ID_3)
      {
        session_type = MMGSDI_GW_PROV_TER_SESSION;
      }
      else
    #endif
    {
      session_type = MMGSDI_GW_PROV_PRI_SESSION;
    }

  app_type = (mmgsdi_app_enum_type)
                  cm_mmgsdi_ssn_tbl_get_app_type_for_type(session_type);

  if (app_type != (mmgsdi_app_enum_type)MMGSDI_APP_SIM &&
      app_type != (mmgsdi_app_enum_type)MMGSDI_APP_USIM )
  {
    CM_ERR_1( "Invalid app type %d", app_type);
    return FALSE;
  }

  /*
  ** Verify the SIM card mode before attempting to read data.
  */
  if ( app_type == MMGSDI_APP_SIM )
  {
    MMGSDI_sim_filename = MMGSDI_GSM_PLMNWACT;
  }
  else if ( app_type == MMGSDI_APP_USIM)
  {
    MMGSDI_sim_filename = MMGSDI_USIM_PLMNWACT;
  }

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(
                                    cm_mmgsdi_session_type,
                                    &cm_mmgsdi_session_id);
  if(CM_MMGSDI_SUCCESS == cm_mmgsdi_status)
  {
    /*
    ** Read the file length for the Preferred PLMNwACT List.
    */
    mmgsdi_status = mmgsdi_session_read_cache_file_size(
                                              cm_mmgsdi_session_id,
                                              MMGSDI_sim_filename,
                                              &file_length);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* It means file should be MMGSDI_GSM_PLMN.
      */
      MMGSDI_sim_filename = MMGSDI_GSM_PLMN;
      mmgsdi_status = mmgsdi_session_read_cache_file_size(
                                                cm_mmgsdi_session_id,
                                                MMGSDI_sim_filename,
                                                &file_length);
    }

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return FALSE;
    }
  }
  else
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
            cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */
    return FALSE;
  }
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* Calculate how many entries can be written into the file.
  */
  if(MMGSDI_sim_filename == MMGSDI_GSM_PLMN)
  {
    num_entries = file_length/ (int32) sizeof( sys_plmn_id_s_type );
  }
  else
  {
    num_entries = file_length/ (int32) sizeof( cm_mmgsdi_plmn_w_act_s_type );
  }

  /* If file size is smaller than what user wants to store,
  ** we will only store what the file can accomodate.
  */
  if(num_entries > user_pref_plmn_list_ptr->length)
  {
    num_entries = user_pref_plmn_list_ptr->length;
  }

  if ( MMGSDI_sim_filename == MMGSDI_GSM_PLMN )
  {
    (void) memset( pplmn_list, 0xFF, sizeof( pplmn_list ) );

    for ( i = 0; i < num_entries; i++ )
    {
      pplmn_list[i] = user_pref_plmn_list_ptr->info[i].plmn;
    }
    write_data.data_len = file_length;
    write_data.data_ptr = ( uint8 * ) pplmn_list;

    status = cmmmgsdi_write( asubs_id, MMGSDI_sim_filename,
                             CM_MMGSDI_SUBS_TYPE_GW,
                             write_data
                           );
    if (! status )
    {
      CM_ERR_0( "Could not write user preferred network list");
    }

    return status;
  }  /* if ( !status ) */

  /*
  ** Convert the user preferred PLMN list to the EFPLMNwAcT and EFPLMNsel
  ** elementary file file format.
  */

  /* Clearing pplmn_w_act_list entries */
  for (i = 0; i < (int32)ARR_SIZE(pplmn_w_act_list) ; ++i)
  {
    /* Make the PLMN's undefined
    */
    sys_plmn_undefine_plmn_id (&pplmn_w_act_list[i].plmn);

    /* Access technology for undefined PLMN's should be set to 0
    ** Sect 4.2.5 3GPP TS 31.102
    ** Sect 3.4 ETSI TS 102.221
    */
    pplmn_w_act_list[i].act[0] = 0;
    pplmn_w_act_list[i].act[1] = 0;
  }

  for ( i = 0; i < num_entries; i++ )
  {
    /*
    ** Ignore any undefined PLMNs that might be included in the
    ** Preferred PLMN With Access Technology List.
    */
    if ( !sys_plmn_id_is_undefined(user_pref_plmn_list_ptr->info[i].plmn) )
    {
      pplmn_w_act_list[i].plmn = user_pref_plmn_list_ptr->info[i].plmn;
      pplmn_w_act_list[i].act[0] = 0;
      pplmn_w_act_list[i].act[1] = 0;

      if ( user_pref_plmn_list_ptr->info[i].access_tech & SYS_ACT_GSM_MASK )
      {
        pplmn_w_act_list[i].act[1] = 0x80;
      }

      if ( user_pref_plmn_list_ptr->info[i].access_tech & SYS_ACT_GSM_COMPACT_MASK )
      {
        pplmn_w_act_list[i].act[1] = pplmn_w_act_list[i].act[1] | 0x40;
      }

      if ( user_pref_plmn_list_ptr->info[i].access_tech & SYS_ACT_UMTS_MASK )
      {
        pplmn_w_act_list[i].act[0] = 0x80;
      }
    }  /* if !sys_plmn_id_is_undefined() */
  }  /* for */

  write_data.data_len = file_length;
  write_data.data_ptr = ( uint8 * ) pplmn_w_act_list;
  status = cmmmgsdi_write( asubs_id, MMGSDI_sim_filename,
                           CM_MMGSDI_SUBS_TYPE_GW,
                           write_data
                         );
  if (! status )
  {
    CM_ERR_0( "Could not write user preferred network list");
  }

  return status;
}  /* cmmmgsdi_write_preferred_plmn_list */

#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION CMMMGSDI_REFRESH_COMPLETE_CB

DESCRIPTION
  Callback function called by MMGSDI when refresh is completed.

RETURN VALUE
  None.

DEPENDENCIES
  None.
===========================================================================*/
/*lint -esym(715,cnf) */
/*lint -esym(715,cnf_ptr) */
static void cmmmgsdi_refresh_complete_cb
(
  mmgsdi_return_enum_type       status,
  mmgsdi_cnf_enum_type          cnf,
  const mmgsdi_cnf_type         *cnf_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(status != MMGSDI_SUCCESS)
  {
    CM_ERR_0("MMGSDI refresh ended with failure");
  }
}  /* cmmmgsdi_refresh_complete_cb() */
/*lint +esym(715,cnf) */
/*lint +esym(715,cnf_ptr) */

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
void cmmmgsdi_refresh_init(

  const mmgsdi_refresh_file_list_type      *refresh_file_list,
    /* File list to read */

  cm_ss_e_type                              ss
    /* SS on which the refresh is initiated */
)
{
  #ifndef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_option_type        no_option = {0};

  #else
  mmgsdi_session_id_type        cm_mmgsdi_session_id   = 0;
  mmgsdi_session_type_enum_type cm_mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
  cm_mmgsdi_error_e_type        cm_mmgsdi_status       = CM_MMGSDI_NOTFOUND;
  sys_modem_as_id_e_type        asubs_id               = cmph_map_cm_ss_to_subs(ss);

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    cm_mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    cm_mmgsdi_session_type = MMGSDI_GW_PROV_TER_SESSION;
  }
  #endif

  CM_MSG_HIGH_3( "SIM_REF: ss %d, asubs_id %d, cm_mmgsdi_session_type %d",
                 ss, asubs_id, cm_mmgsdi_session_type );

  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(cm_mmgsdi_session_type,
                                                  &cm_mmgsdi_session_id);
  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_1("SIM_REF: Unable to get session id; err: %d",cm_mmgsdi_status); /*lint !e641 */
  }
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  cmmmgsdi_refresh_files_read_proc( ss,
                                    refresh_file_list,
                                    CM_CLIENT_ID_ANONYMOUS
                                  );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  if(mmgsdi_session_refresh_complete( cm_mmgsdi_session_id,
                                      *pass_fail_ptr(),
                                      cmmmgsdi_refresh_complete_cb,
                                      0
                                    ) != MMGSDI_SUCCESS
     )
  #else
  if(mmgsdi_refresh_complete( cm_mmgsdi_get_client_id(),
                              MMGSDI_SLOT_1,
                              *pass_fail_ptr(),
                              no_option,
                              cmmmgsdi_refresh_complete_cb,
                              0
                            ) != MMGSDI_SUCCESS
     )
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  {
    CM_ERR_0("SIM_REF: mmgsdi_refresh_complete() fail to q");
  }
}  /* cmmmgsdi_refresh_init() */

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

  const mmgsdi_file_enum_type      *file_list_ptr,
    /* List of files */

  uint32                           num_files
    /* Length of the file list */

)
{
  mmgsdi_refresh_file_list_type    cm_file_info;
    /* Structure holds information about SIM files CM interacts with */

  uint32                           file_index = 0;
    /* counter for file list */

  uint32                           cm_file_index = 0;
    /* counter for cm files */

  uint32                           file_match = 0;
    /* counter */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear file info */
  memset(&cm_file_info, 0, sizeof(cm_file_info));

  /* Get the SIM files that CM works on
  */
  cmmmgsdi_get_files_used_incall(&cm_file_info);

  for(file_index=0; file_index < num_files; ++file_index)
  {
    for(cm_file_index=0; cm_file_index < cm_file_info.num_files; ++cm_file_index)
    {
      /* Match detected between given list and CM's list, given file is
      ** being used by CM.incall
      */
      if(file_list_ptr[file_index] == cm_file_info.file_list_ptr[cm_file_index])
      {
        CM_MSG_HIGH_1( "SIM_REF: File %d refreshed",file_list_ptr[file_index] );
        file_match++;
        break;
      }
    }  /* for (cm_file_index= ... */
  }  /* for (file_index= ... */

  CM_MSG_HIGH_2( "SIM_REF: No.of Files ref %d CM match %d",num_files,file_match );

  return(file_match == num_files);

}  /* cmmmgsdi_validate_received_file_list() */

/*===========================================================================
FUNCTION CMMMGSDI_UPDATE_RAT_MODE

DESCRIPTION
  This function updates the current mode of the phone
  with the new mode that's been reeived from MMGSDI.

  If there's any change, the current mode will be replaced with
  the one given by MMGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_update_rat_mode(
  cm_client_id_type                       client_id,
  cmmmgsdi_rat_mode_preference_e_type     rat_mode
)
{
  cmph_s_type               *ph_ptr = cmph_ptr(); /* Get the phone object. */
  cm_mode_pref_e_type       new_mode_pref = CM_MODE_PREF_NONE;
  cm_mode_pref_e_type       fav_mode_pref       = CM_MODE_PREF_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialise RAT mode only if NV_ENS_ENABLED_I is set to 1
  ** and RAT balancing enabled.
  */
  
    switch (rat_mode)
    {
      case MMGSDI_RAT_MODE_PREF_GSM_WCDMA:
        new_mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY;
        break;

      case MMGSDI_RAT_MODE_PREF_GSM_ONLY:
        new_mode_pref = CM_MODE_PREF_GSM_ONLY;
        break;

      case MMGSDI_RAT_MODE_PREF_WCDMA_ONLY:
        new_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
        break;

      case MMGSDI_RAT_MODE_PREF_AUTOMATIC:
        new_mode_pref = CM_MODE_PREF_GWL;
        break;

      case MMGSDI_RAT_MODE_PREF_LTE_ONLY:
        new_mode_pref = CM_MODE_PREF_LTE_ONLY;
        break;

      default:
        /* RAT mode can't be other than above three.
        ** Some issue in RAT file read. Exit.
        */
        CM_ERR_1("Can't access RAT mode from MMGSDI_USIM_7F66_PROP1_RAT:"
             " Unknown card mode: %x", rat_mode);
    }

  if(ph_ptr->is_rat_bal)
    {
    CM_MSG_HIGH_3("cmmmgsdi_update_rat_mode after mode_pref=%d, user_mode_pref=%d,rat_mode=%d", 
                   ph_ptr->main_stack_info.pref_info.mode_pref,
                   ph_ptr->main_stack_info.pref_info.user_mode_pref,
                   rat_mode );
 
      if(!cm_ph_cmd_sys_sel_pref_new(
                    NULL,
                    NULL,
                    client_id,
                    ph_ptr->main_stack_info.pref_info.user_mode_pref,
                    CM_PREF_TERM_PWR_CYCLE,
                    0,
                    CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                    CM_BAND_PREF_NO_CHANGE,
                    CM_PRL_PREF_NO_CHANGE,
                    CM_ROAM_PREF_NO_CHANGE,
                    CM_HYBR_PREF_NO_CHANGE,
                    CM_SRV_DOMAIN_PREF_NO_CHANGE,
                    CM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                    NULL)
        )
      {
      CM_ERR_2("Can't change phone's mode preference from ef_rat:%x to user_mode_pre:%x for "
               "RAT balancing",
        new_mode_pref, ph_ptr->main_stack_info.pref_info.user_mode_pref);
      }
      else
       {
          /* Changing to EF_RAT specified mode_pref is succesful */
          CM_MSG_HIGH_2 (" Queued req to CM to change phone's mode_pref as"
                      " indicated by MMGSDI_USIM_7F66_PROP1_RAT, from "
                      "%d to %d",
                 ph_ptr->main_stack_info.pref_info.user_mode_pref,new_mode_pref);
          /* Set the flag */
          cmph_ptr()->is_mode_change_due_to_ef_rat = TRUE;
       }

  }
  else /* if (ph_ptr->is_rat_bal) */
  {
    CM_ERR_0("Can't update RAT mode. NV_ENS_ENABLED_I flag is not enabled.");
  }

}  /* cmmmgsdi_update_rat_mode() */
#endif  /*  FEATURE_GSTK */
#endif  /*  defined CM_GW_SUPPORTED ) */

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
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

/*lint -esym(818, cmals_mmgsdi_async_cb)
** cmals_mmgsdi_async_cb cannot be constant, because it is typecasted
*/
boolean cmmmgsdi_cmals_mmgsdi_async_sim_write(
    mmgsdi_file_enum_type        file_name,
    mmgsdi_rec_num_type          record_number,
    mmgsdi_len_type              request_length,
    uint8*                       data_buffer_p,
    int                          file_structure,
    void*                        cmals_mmgsdi_async_cb
)
{
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;
  mmgsdi_write_data_type       write_data;
  void*                        temp_cmals_mmgsdi_async_cb = NULL;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type           cm_mmgsdi_session_id;
  mmgsdi_session_type_enum_type    cm_mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
  cm_mmgsdi_error_e_type           cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_access_type               file_access;

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_name;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  if(cmmmgsdi_ptr()->sim_busy_status.is_sim_busy == TRUE)
  {

    temp_cmals_mmgsdi_async_cb = cmals_mmgsdi_async_cb ;
    cmals_mmgsdi_async_cb = NULL;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  write_data.data_len = request_length;
  write_data.data_ptr = data_buffer_p;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(cm_mmgsdi_session_type,
     &cm_mmgsdi_session_id);
  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
            cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */

    return FALSE;
  }
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  switch (file_structure)
  {
    case cyclic:
      /*lint -e{611} Suspicious cast, MMGSDI function expects 6th arg as
      ** mmgsdi_callback_type */
      #ifdef FEATURE_MMGSDI_SESSION_LIB
      status = mmgsdi_session_write_record(
                    cm_mmgsdi_session_id,
                    file_access,
                    MMGSDI_CYCLIC_FILE,
                    record_number,
                    write_data,
                    (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
                    0
                    );
      #else
      status = mmgsdi_write_record(
                    cm_mmgsdi_get_client_id(),
                    MMGSDI_SLOT_1,  /* MMGSDI ativates MMGSDI_SLOT_1 only */
                    file_name,
                    record_number,
                    write_data,
                    (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
                    0
                    );
      #endif /* FEATURE_MMGSDI_SESSION_LIB */
      /*lint -restore */

      if (status != MMGSDI_SUCCESS) {
        return FALSE;
      }
      break;

    case transparent:
      /*lint -e{611} Suspicious cast, MMGSDI function expects 6th arg as
      ** mmgsdi_callback_type */
      #ifdef FEATURE_MMGSDI_SESSION_LIB
      status = mmgsdi_session_write_transparent(
                            cm_mmgsdi_session_id,
                            file_access,
                            0,
                            write_data,
                            (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
                            0
                            );
      #else
      status = mmgsdi_write_transparent(
                  cm_mmgsdi_get_client_id(),
                  MMGSDI_SLOT_1,  /* MMGSDI supports MMGSDI_SLOT_1 only */
                  file_name,
                  0,
                  write_data,
                  (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
                  0
                  );
      #endif /* FEATURE_MMGSDI_SESSION_LIB */
      /*lint -restore */

      if (status != MMGSDI_SUCCESS) {
        return FALSE;
      }
      break;

    default:
      CM_ERR_0("CMMMGSDI : Read req file is neither cyclic nor transparent");
      break;

  }

  if(cmmmgsdi_ptr()->sim_busy_status.is_sim_busy == TRUE)
  {
    mmgsdi_cnf_type     cnf;
    cnf.write_cnf.access.file.file_enum = file_access.file.file_enum;
    cnf.write_cnf.response_header.mmgsdi_status = MMGSDI_SUCCESS;

    cmals_mmgsdi_async_cb = temp_cmals_mmgsdi_async_cb ;

    if(cmals_mmgsdi_async_cb != NULL)
    {
    ((mmgsdi_callback_type)cmals_mmgsdi_async_cb)(MMGSDI_SUCCESS,MMGSDI_WRITE_CNF,&cnf);
  }
  }

  /* Command queued successfully
  */
  return TRUE;

}  /* cmmmgsdi_cmals_mmgsdi_async_sim_write() */
/*lint +esym(818, cmals_mmgsdi_async_cb) */

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

/*lint -esym(818, cmals_mmgsdi_async_cb)
** cmals_mmgsdi_async_cb cannot be constant, because it is typecasted
*/
boolean cmmmgsdi_cmals_mmgsdi_async_sim_read(
    mmgsdi_file_enum_type             file_name,
    mmgsdi_rec_num_type               record_number,
    mmgsdi_len_type                   request_length,
    int                               file_structure,
    mmgsdi_callback_type              cmals_mmgsdi_async_cb,
    mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type           cm_mmgsdi_session_id;
  mmgsdi_session_type_enum_type    cm_mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
  cm_mmgsdi_error_e_type           cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_access_type               file_access;
  cmmmgsdi_s_type           *mmgsdi_ptr = cmmmgsdi_ptr();

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_name;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_MMGSDI_SESSION_LIB

  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(cm_mmgsdi_session_type,
                                                  &cm_mmgsdi_session_id);

  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d",
           cm_mmgsdi_session_type, cm_mmgsdi_status); /*lint !e641 */

    return FALSE;
  }
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  mmgsdi_ptr->read_als_uptime.uptime = time_get_uptime_secs() + CM_MMGSDI_ABORT_TIMER_VALUE_SECS;
  mmgsdi_ptr->read_als_uptime.client_data = (uint32)client_ref;
  mmgsdi_ptr->read_als_uptime.response_cb_ptr = cmals_mmgsdi_async_cb;

  switch (file_structure)
  {
    case cyclic:
       #ifdef FEATURE_MMGSDI_SESSION_LIB
      /*lint -e{611} Suspicious cast, MMGSDI function expects 6th arg as
      ** mmgsdi_callback_type */
       status = mmgsdi_session_read_record(
          cm_mmgsdi_session_id,
          file_access,
          record_number,
          request_length,
          (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
          client_ref
          );
       #else
      /*lint -e{611} Suspicious cast, MMGSDI function expects 6th arg as
      ** mmgsdi_callback_type */
       status = mmgsdi_read_record(
          cm_mmgsdi_get_client_id(),
          MMGSDI_SLOT_1, /* MMGSDI ativates MMGSDI_SLOT_1 only */
          file_name,
          record_number,
          request_length,
          (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
          client_ref
          );
      /*lint -restore */
       #endif /* FEATURE_MMGSDI_SESSION_LIB */

      if (status != MMGSDI_SUCCESS)
      {
       cmmmgsdi_ptr()->read_als_uptime.uptime = 0;
       return FALSE;
      }
      break;

    case transparent:
       /*lint -e{611} Suspicious cast, MMGSDI function expects 6th arg as
       ** mmgsdi_callback_type */
       #ifdef FEATURE_MMGSDI_SESSION_LIB
       status = mmgsdi_session_read_transparent(
          cm_mmgsdi_session_id,
          file_access,
          0,
          request_length,
          (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
          client_ref
          );
       #else
       status = mmgsdi_read_transparent(
          cm_mmgsdi_get_client_id(),
          MMGSDI_SLOT_1, /* MMGSDI ativates MMGSDI_SLOT_1 only */
          file_name,
          0,
          request_length,
          (mmgsdi_callback_type)cmals_mmgsdi_async_cb,
          client_ref
          );
       /*lint -restore */
       #endif /* FEATURE_MMGSDI_SESSION_LIB */

      if (status != MMGSDI_SUCCESS) {
       cmmmgsdi_ptr()->read_als_uptime.uptime = 0;
       return FALSE;
      }
      break;

    default:
      CM_ERR_0("CMMMGSDI : Read req file is neither cyclic nor transparent");
      break;

  }


  /* Command queued successfully
  */
  return TRUE;

}  /* cmals_gsdi_async_sim_read() */
/*lint +esym(818, cmals_mmgsdi_async_cb) */
#endif  /* (defined FEATURE_ALS) && (defined FEATURE_GSM) */

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
      /* Status of cmmmgsdi_session_store_esn() call */

    mmgsdi_cnf_enum_type            store_esn_cnf,
      /* Indicates which API call results in cb function call */

    const mmgsdi_cnf_type           *cnf_ptr
      /* Results returned in cb function call */
)
{

  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( status != MMGSDI_SUCCESS )
  {
    CM_ERR_2("cmmmgsdi_get_all_pin_status() failed. Stat %d, Confir. %d",
             status, store_esn_cnf);
    return;
  }
}

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
)
/*lint -esym(715,get_all_pin_status_cnf)*/
{
  cm_subscription_status_e_type    gw_subs_status =
                                      CM_SUBSCRIPTION_STATUS_NO_CHANGE;
    /* Used for GW subscription availability parameter to API */

  cm_subscription_status_e_type    hybr_gw_subs_status =
                                      CM_SUBSCRIPTION_STATUS_NO_CHANGE;
    /* Used for Hybr GW subscription availability parameter to API */

  cm_subscription_status_e_type    hybr_3_gw_subs_status =
                                      CM_SUBSCRIPTION_STATUS_NO_CHANGE;
    /* Used for Hybr GW subscription availability parameter to API */

  cm_subscription_status_e_type    cdma_subs_status =
                                      CM_SUBSCRIPTION_STATUS_NO_CHANGE;
    /* Used for CDMA subscription availability parameter to API */

  sys_modem_as_id_e_type           asubs_id;

  mmgsdi_pin_info_type             universal_pin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

  if(cnf_ptr == NULL)/*lint !e774 */
  {
    CM_MSG_HIGH_0("NULL poiter sent in cb");
    return;
  }

  if( status != MMGSDI_SUCCESS )
  {
    CM_ERR_2("cmmmgsdi_get_all_pin_status() failed. Stat %d, Confir. %d",
                  status, get_all_pin_status_cnf);
    return;
  }

  if(cnf_ptr->get_all_pin_status_cnf.universal_pin.pin_replacement !=
                                          MMGSDI_PIN_REPLACED_BY_UNIVERSAL)
  {
    CM_MSG_HIGH_0("UPIN not replaced");
    return;
  }

  universal_pin = cnf_ptr->get_all_pin_status_cnf.universal_pin;

  /* If PIN1 is replaced by Universal PIN then only honor universal PIN
  ** event, otherwise discard the universal PIN event
  */
  if( universal_pin.pin_replacement == MMGSDI_PIN_REPLACED_BY_UNIVERSAL )
  {
      asubs_id =
        cm_mmgsdi_ssn_tbl_get_subs_id_for_id(cnf_ptr->get_all_pin_status_cnf.response_header.session_id);

    CM_MSG_HIGH_0("MMGSDI_PIN_REPLACED_BY_UNIVERSAL");

    /* If any of the PIN blocked or PIN permanently blocked, is received,
    ** send subscription not available to the technology for which
    ** subscription from card is already available
    */
    if( universal_pin.status == MMGSDI_PIN_BLOCKED ||
        universal_pin.status == MMGSDI_PIN_PERM_BLOCKED
      )
    {
      CM_MSG_HIGH_1("UPIN status %d", universal_pin.status);

      /* If CDMA subscription from card is already sent, made CDMA
      ** subscription not available
      */
      if (cmph_is_msim())
      {
        if( cmph_get_cdma_subs_from_card_sent() == TRUE &&
            asubs_id == cmph_ptr()->main_stack_info.asubs_id)
        {
          cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
        if( cmph_get_gw_subs_from_card_sent() == TRUE &&
            asubs_id == cmph_ptr()->main_stack_info.asubs_id)
        {
          gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      }
      else
      {
        if( cmph_get_cdma_subs_from_card_sent() == TRUE)
        {
          cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
        if( cmph_get_gw_subs_from_card_sent() == TRUE)
        {
          gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      }

      /* If Hybr GW subscription from card is already sent, made GW
      ** subscription not available
      */
      if (cmph_is_msim())
      {
        if (cmph_get_hybr_gw_subs_from_card_sent() == TRUE &&
            asubs_id == cmph_ptr()->hybr_2_stack_info.asubs_id)
        {
          hybr_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* If Hybr GW3 subscription from card is already sent, made GW
      ** subscription not available
      */
      if( cmph_get_hybr_3_gw_subs_from_card_sent() == TRUE &&
          asubs_id == cmph_ptr()->hybr_3_stack_info.asubs_id
        )
      {
        hybr_3_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
      }
      #endif
      }
      else
      {
        if (cmph_get_hybr_gw_subs_from_card_sent() == TRUE)
        {
          hybr_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      }

      /* If subscription from card for any of the cellular technology
      ** (CDMA or GW) is to be made unavailable, then only queue the
      ** subscription not available command to CM
      */
      if( gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
          hybr_gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
          #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
          hybr_3_gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
          #endif
          cdma_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE
        )
      {
        cmph_send_subscription_not_available_internal( cdma_subs_status,
                                                       gw_subs_status,
                                                       hybr_gw_subs_status,
                                                       hybr_3_gw_subs_status,
                                                       CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_OTHERS);
      } /* if( gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE.. */
    } /* if( cnf_ptr->get_all_pin_status_cnf... */
    else if( universal_pin.status == MMGSDI_PIN_UNBLOCKED )
    {
      /* Pin Unblock request always comes after Pin block only.
      ** If subs was avaialble before PIN BLOCK has happend,
      ** CM has to send subs available for 1x and GW protocols.
      */
      if (cmph_is_msim())
      {
        if( cmph_get_cdma_subs_from_card_sent() == TRUE &&
            asubs_id == cmph_ptr()->main_stack_info.asubs_id)
        {
          cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      }
      else
      {
        if( cmph_get_cdma_subs_from_card_sent() == TRUE)
        {
          cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      }


      if (cmph_is_msim())
      {
        if( cmph_get_gw_subs_from_card_sent() == TRUE &&
            asubs_id == cmph_ptr()->main_stack_info.asubs_id)
        {
          gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      }
      else
      {
        if( cmph_get_gw_subs_from_card_sent() == TRUE)
        {
          gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
        }
      }


      if( cmph_get_hybr_gw_subs_from_card_sent() == TRUE &&
          asubs_id == cmph_ptr()->hybr_2_stack_info.asubs_id
        )
      {
        /* Mark GW subs changed */
        hybr_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
      }

      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      if( cmph_get_hybr_3_gw_subs_from_card_sent() == TRUE &&
          asubs_id == cmph_ptr()->hybr_3_stack_info.asubs_id
        )
      {
        /* Mark GW subs changed */
        hybr_3_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
      }
      #endif

      /* Send subs available command */
      if ( gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE  ||
           cdma_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
           hybr_gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE
           #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
           || hybr_3_gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE
           #endif
         )/*lint !e774 */
      {
        if( !cm_ph_cmd_subscription_available3(
                                            NULL,
                                            NULL,
                                            CM_CLIENT_ID_ANONYMOUS,
                                            cdma_subs_status,
                                            gw_subs_status,
                                            hybr_gw_subs_status,
                                            hybr_3_gw_subs_status))
        {
          CM_ERR_3("pin unblock: subs status CDMA %d, GW %d, Hybr GW %d",
                        cdma_subs_status,
                        gw_subs_status, hybr_gw_subs_status);
          CM_ERR_1("pin unblock: subs na Hybr3 GW %d",
                       hybr_3_gw_subs_status);

        }
        else
        {
          CM_MSG_HIGH_4("pin unblock: subs status CDMA %d, GW %d, Hybr GW %d Hybr3 GW %d",
                          cdma_subs_status,
                          gw_subs_status, hybr_gw_subs_status, hybr_3_gw_subs_status);
        }
      }
    } /* else if( pin_info.status == MMGSDI_PIN_UNBLOCKED ) */
    /* if PIN event points to subscription availability
    */
    else
    {
      /* if card subscription for any cellular subscription is available,
      ** that means card is valid and no need to check for valid card and
      ** available apps becuase the present subscription status
      ** already represents that
      */
      CM_MSG_HIGH_1("UPIN status %d ", universal_pin.status);
    } /* if( cnf_ptr->get_all_pin_status_cnf.universal_pin.status == */
  } /*if( cnf_ptr->get_all_pin_status_cnf.universal_pin.. */
} /* cmmmgsdi_get_all_pin_status_cb() */
/*lint +esym(715,get_all_pin_status_cnf)*/

/*==========================================================================

FUNCTION CMMMGSDI_GET_ALL_PIN_STATUS

DESCRIPTION
  Put CM command in MMGSI to know the present PIN status

RETURN VALUE
  NONE

DEPENDENCIES
  None
==========================================================================*/

void cmmmgsdi_get_all_pin_status( mmgsdi_session_id_type session_id)
{
  mmgsdi_return_enum_type  status = MMGSDI_ERROR;
    /* To store the calling success status for MMGSDI API */

  status = mmgsdi_session_get_all_pin_status(
                                      session_id,
                                      cmmmgsdi_get_all_pin_status_cb,
                                      0
                                    );
  if( status != MMGSDI_SUCCESS )
  {
    CM_ERR_0("mmgsdi_session_get_all_pin_status() failed to be queued");
  }
  else
  {
    CM_MSG_HIGH_0("mmgsdi_session_get_all_pin_status() is queued successfully");
  }
} /* cmmmgsdi_get_all_pin_status() */

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
  void                  cm_mmgsdi_in_traffic_channel_w_mode(

  boolean               in_traffic,

  sys_sys_mode_e_type   mode,

  cm_ss_e_type          ss

)
/*lint -esym(715,in_traffic)*/
/*lint -esym(715,ss)*/
{

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  int index;
  mmgsdi_in_tc_mode_enum_type gsdi_mode = MMGSDI_NOT_IN_TC;
  #endif

  uint64 active_call_mask = 0;
  uint64 mmgsdi_call_mask = 0;

  switch ( mode )
  {
    case SYS_SYS_MODE_AMPS:
    case SYS_SYS_MODE_CDMA:
      #ifdef FEATURE_MMGSDI_SESSION_LIB
      gsdi_mode = MMGSDI_IN_TC_1X;
      #endif
      break;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_LTE:
    case SYS_SYS_MODE_TDS:
      #ifdef FEATURE_MMGSDI_SESSION_LIB
      gsdi_mode = MMGSDI_IN_TC_GW;
      #endif
      break;

    default:
      CM_MSG_HIGH_1 ("Unhandled Mode: %x",mode);
      return;
  }

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  if(ss == CM_SS_HYBR_2 && cmph_is_msim() && !cmph_is_sxlte())
  {
    index = (int)MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(ss == CM_SS_HYBR_3)
  {
    index = (int)MMGSDI_GW_PROV_TER_SESSION;
  }
  else
  #endif
  #ifdef FEATURE_MMODE_SXLTE_G
  if(ss == CM_SS_HYBR_3)
  {
    index = (int)MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  #endif
  {
    if(gsdi_mode == MMGSDI_IN_TC_GW)
    {
      index = (int)MMGSDI_GW_PROV_PRI_SESSION;
    }
    else
    {
      index = (int)MMGSDI_1X_PROV_PRI_SESSION;
    }
  }

  active_call_mask = cmcall_is_there_a_call_activity_per_sub(
                      cmph_map_cm_ss_to_subs(ss),
                      (CM_CALL_TYPE_VOICE_MASK
                       |CM_CALL_TYPE_EMERG_MASK
                       |CM_CALL_TYPE_CS_DATA_MASK
                       |CM_CALL_TYPE_VT_MASK
                       |CM_CALL_TYPE_PS_DATA_MASK
                       |CM_CALL_TYPE_VS_MASK)
                       );
  
  CM_MSG_HIGH_3 ("Informing traffic state to MMGSDI, traffic:%d, gsdi_mode:%d, active_call_mask:%d",
                                in_traffic,gsdi_mode,active_call_mask);

  if(in_traffic == TRUE)
  {
    if(active_call_mask & CM_CALL_TYPE_PS_DATA_MASK)
    {
      mmgsdi_call_mask = MMGSDI_MASK_PS_CALL_ONGOING;
    }

    if(active_call_mask != CM_CALL_TYPE_PS_DATA_MASK)
    {
      mmgsdi_call_mask |= MMGSDI_MASK_NON_PS_CALL_ONGOING;
    }
  }
  if( mmgsdi_session_toggle_uim_power_control_sync(
                      cm_mmgsdi_session_id_table[index].session_id,
                      in_traffic, gsdi_mode,mmgsdi_call_mask
                     ) != MMGSDI_SUCCESS
     )
  {
    CM_ERR_0("Unable to toggle UIM power control sync");
  }
  #endif
}
/*lint +esym(715,in_traffic)*/
/*lint +esym(715,ss)*/


#ifdef FEATURE_MMODE_DUAL_SIM

/*====================================================================
FUNCTION cm_mmgsdi_ssn_tbl_get_app_info_for_id

DESCRIPTION

  Get App info for session entry matching given session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_get_app_info_for_id
(
  mmgsdi_session_id_type ssn_id,
    /* Session id for which session info needs to be updated
    */

  mmgsdi_app_info_type *app_info
    /* Pointer to app info */
)
{

  cm_mmgsdi_error_e_type result = CM_MMGSDI_NOTFOUND;
  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(ssn_id);
  if(ssn_entry != NULL)
  {
    result = CM_MMGSDI_SUCCESS;
    *app_info = *(&(ssn_entry->app_info));
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  return result;
}


#endif /* FEATURE_MMODE_DUAL_SIM */


/*===========================================================================
FUNCTION CMMMGSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function for MMGSDI generated event.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_card_status_cb
(
  const mmgsdi_event_data_type *event
)
{
  cm_generic_rpt_s_type *cm_generic_rpt_ptr;
  cmph_s_type              *ph_ptr = cmph_ptr();
   /* Needed to handle card events and PIN events */

  mmgsdi_event_data_type          *mmgsdi_rpt_ptr;
  int idx = SYS_MODEM_AS_ID_NONE;
  uint8                         iccid_buf[CM_MMGSDI_ICCID_LEN];
  int i = 0;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_session_info_s_type   ssn_info = {MMGSDI_1X_PROV_PRI_SESSION, 0};
  #endif

  #if (defined CM_GW_SUPPORTED || defined (FEATURE_CM_LTE))
  cm_rpt_type         *cmd_ptr;
  #else /* !defined CM_GW_SUPPORTED || defined (FEATURE_CM_LTE)) */
  #ifdef FEATURE_GSTK
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cm_mc_rpt_type     *mc_rpt_ptr;
  #endif /*  (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
  #endif /* ifdef FEATURE_GSTK */
  #endif /* !defined CM_GW_SUPPORTED || defined (FEATURE_CM_LTE)) */
  CM_MSG_HIGH_1("<<CM mmgsdievt %d", event->evt);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( event->evt )
  {
    case MMGSDI_CARD_INSERTED_EVT:
    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
    case MMGSDI_SAP_CONNECT_EVT:
    case MMGSDI_PIN1_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
    #ifndef FEATURE_MMGSDI_SESSION_LIB
    /* Init complete event not to be handled if session based API is used.
    ** Instead session ready event will be sent
    */
    case MMGSDI_CARD_INIT_COMPLETED_EVT:
    /* Card level Illegal card event is ignored when session based API is
    ** enabled.
    */
    //case MMGSDI_ILLEGAL_CARD_EVT:
    #else
    case MMGSDI_SESSION_CHANGED_EVT:
    case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
    #endif
      /* Card and PIN events from MMGSDI is put in Queue
      */
      cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();
        mmgsdi_rpt_ptr = &(cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt);

        cm_generic_rpt_ptr->hdr.cmd     = CM_MMGSDI_CARD_RPT;
        mmgsdi_rpt_ptr->evt             = event->evt;
        mmgsdi_rpt_ptr->client_id       = event->client_id;

        #ifdef FEATURE_MMGSDI_SESSION_LIB
        mmgsdi_rpt_ptr->session_id      = event->session_id;
        #endif

        switch ( event->evt )
        {
          case MMGSDI_CARD_INSERTED_EVT:
            *(&(mmgsdi_rpt_ptr->data.card_inserted)) = *(&(event->data.card_inserted));
             if ( cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CT)
            {
              /*Check for change in ICCID*/
              memset(&iccid_buf[0], 0,sizeof(iccid_buf));
              memscpy(&iccid_buf[0],sizeof(iccid_buf),
                      &(event->data.card_inserted.iccid),
                      event->data.card_inserted.iccid_len);
              idx = (int)( event->data.card_inserted.slot - 1);
              idx = ( (idx<MAX_AS_IDS) && (idx>SYS_MODEM_AS_ID_NONE))?idx:SYS_MODEM_AS_ID_NONE;

              if( (idx != SYS_MODEM_AS_ID_NONE)&& (memcmp(iccid_buf, cm_mmgsdi_iccid_table[idx].iccid, event->data.card_inserted.iccid_len) != 0))
              {
                CM_MSG_HIGH_0( "cmmmgsdi_card_insert_evt - ICCID Changed");
                //Clear Avoidance List
                if(! cm_ph_cmd_avoid_sys( NULL,
                                          NULL,
                                          CM_CLIENT_TYPE_NONE,
                                          CM_SS_CLR_AVOID_SYS_LIST,
                                          0 /* avoid time is obsolete */) )
                {
                  CM_ERR_0( "cm_ph_cmd_avoid_sys returned failure");
                }
                else
                {
                  CM_MSG_HIGH_0( "cm_ph_cmd_avoid_sys returned true");
                }

                /*Store ICCID*/

                cm_mmgsdi_iccid_table[idx].slot = event->data.card_inserted.slot;
                memset(&cm_mmgsdi_iccid_table[idx].iccid, 0,sizeof(cm_mmgsdi_iccid_table[idx].iccid));
                memscpy(&cm_mmgsdi_iccid_table[idx].iccid,CM_MMGSDI_ICCID_LEN,
                        &(event->data.card_inserted.iccid),
                        event->data.card_inserted.iccid_len);

                for(i=0;i<CM_MMGSDI_ICCID_LEN;i++)
                {
                  CM_MSG_HIGH_3( "card_insert_evt-slot:%d,ICCID[%d]:%x", cm_mmgsdi_iccid_table[idx].slot,i, cm_mmgsdi_iccid_table[idx].iccid[i]);
                }

              }
              else
              {
                CM_MSG_HIGH_1( "cmmmgsdi_card_insert_evt-ICCID Not Changed in,idx:%d", idx);
              }
            }


            /*Call CMRMP_TDS*/
            #ifdef FEATURE_MMODE_RPM_TDSCDMA
            CM_MSG_HIGH_0("MMGSDI_CARD_INSERTED_EVT send to CMRPM_TDS");
            cmrpm_tds_sim_evt_proc(CMRPM_SIM_CARD_INSERTED_EVT);
            #endif

            break;

          case MMGSDI_CARD_ERROR_EVT:
            *(&(mmgsdi_rpt_ptr->data.card_error)) = *(&(event->data.card_error));

            /* If OTASP commit is pending */
            /* If subscription is from card, reset the card */
            if ( ph_ptr->is_otasp_commit_pending && cmph_get_cdma_subs_from_card_sent()&&
                 (cm_received_card_pwr_down_cnf == TRUE) )
            {
              /* Send card power-up cmd to MMGSDI upon receiving
                 MMGSDI_CARD_PDOWN_CNF and MMGSDI_CARD_ERROR_EVT
              */
              if(mmgsdi_card_pup(cm_mmgsdi_get_client_id(),
                         MMGSDI_SLOT_1,
                         cmmmgsdi_generic_cb,
                         MMGSDI_CARD_POWER_UP_INITIAL_PUP,
                         0 ) != MMGSDI_SUCCESS
                )
                {
                  CM_ERR_0("Unable to power-up card");
                }
                ph_ptr->is_otasp_commit_pending = FALSE;
                cm_received_card_pwr_down_cnf = FALSE;
             }

            break;

          case MMGSDI_CARD_REMOVED_EVT:
            *(&(mmgsdi_rpt_ptr->data.card_removed)) = *(&(event->data.card_removed));

            break;

          #ifndef FEATURE_MMGSDI_SESSION_LIB
          case MMGSDI_CARD_INIT_COMPLETED_EVT:
            cmph_set_received_card_init_completed_evt( TRUE );
            *(&(mmgsdi_rpt_ptr->data.card_init_completed)) = *(&(event->data.card_init_completed));

            break;
          #endif /* FEATURE_MMGSDI_SESSION_LIB */

          case MMGSDI_SAP_CONNECT_EVT:
            *(&(mmgsdi_rpt_ptr->data.sap_connect)) = *(&(event->data.sap_connect));

            break;

          case MMGSDI_PIN1_EVT:
            *(&(mmgsdi_rpt_ptr->data.pin)) = *(&(event->data.pin));

            break;

          case MMGSDI_UNIVERSAL_PIN_EVT:
            *(&(mmgsdi_rpt_ptr->data.pin)) = *(&(event->data.pin));

            break;

          #ifdef FEATURE_MMGSDI_SESSION_LIB
          case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
          {
            CM_MSG_HIGH_1("MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT, session-id %d",
                        event->data.illegal_session.session_id);

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            /* Send the illegal subscription event to CM
            ** - Set the fields of the generic report
            ** - Queue the report to CM
            */
            *(&(mmgsdi_rpt_ptr->data.illegal_session)) = *(&(event->data.illegal_session));

            /*Call CMRMP_TDS*/
            #ifdef FEATURE_MMODE_RPM_TDSCDMA
            CM_MSG_HIGH_0("MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT send to CMRPM_TDS");
            cmrpm_tds_sim_evt_proc(CMRPM_SIM_ILLEGAL_SUBSCRIPTION);
            #endif

          }
                  break;

          case MMGSDI_SESSION_CHANGED_EVT:
            CM_MSG_HIGH_3("MMGSDI_SESSION_CHANGED_EVT, slot %d app type %d activated %d",
               event->data.session_changed.app_info.slot,
               event->data.session_changed.app_info.app_data.app_type, event->data.session_changed.activated);

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            /* When subscription is available subscription_ready event is always
            ** recieved after a session_changed evt. No need to change any
            ** app_info for this event.
            */

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            /* Send the session changed event to CM
            ** - Set the fields of the generic report
            ** - Queue the report to CM
            */
            *(&(mmgsdi_rpt_ptr->data.session_changed)) = *(&(event->data.session_changed));

            break;
          #endif /* FEATURE_MMGSDI_SESSION_LIB */

          default:
            CM_ERR_1("Invalid MMGSDI evt %d", event->evt);
            break;
        }

        cm_generic_rpt(cm_generic_rpt_ptr);

      break;

    case MMGSDI_PERSO_EVT:
      switch(event->data.perso.status)
      {
        case MMGSDI_PERSO_STATUS_LOCKED:
        case MMGSDI_PERSO_STATUS_BLOCKED:
        case MMGSDI_PERSO_STATUS_GENERIC_FAILURE:
        case MMGSDI_PERSO_STATUS_WAIT_FOR_EXT_PERSO:
        case MMGSDI_PERSO_STATUS_SANITY_ERROR:

         {
           /* GW subscirpiton status */
          cm_subscription_status_e_type   gw_subs_status =
                                                CM_SUBSCRIPTION_STATUS_NO_CHANGE;
          /* Hybr GW subscirpiton status */
          cm_subscription_status_e_type   hybr_gw_subs_status =
                                                CM_SUBSCRIPTION_STATUS_NO_CHANGE;
          /* Hybr GW3 subscirpiton status (for SXLTE) */
          cm_subscription_status_e_type   hybr_3_gw_subs_status =
                                                CM_SUBSCRIPTION_STATUS_NO_CHANGE;
          /* CDMA subscirpiton status */
          cm_subscription_status_e_type   cdma_subs_status =
                                                CM_SUBSCRIPTION_STATUS_NO_CHANGE;

          /* session info for mmgsdi reports recieved */
          cm_mmgsdi_session_info_s_type   sess_info =
                                                {MMGSDI_MAX_SESSION_TYPE_ENUM, 0 };

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          /* Get the session type corresponding to this event */
          if(cm_mmgsdi_ssn_tbl_get_ssn_info_for_id(event->session_id , &sess_info) !=
                                                                    CM_MMGSDI_SUCCESS)
          {
            CM_ERR_1(" Wrong session_id %d", event->session_id);
            return;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          /* Map the sess_info.session_type to the subscriptions */
          switch(sess_info.session_type)
          {
            case MMGSDI_1X_PROV_PRI_SESSION:
              /* Perso feature is not needed for 1X subscription*/
              //cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
              break;

            case MMGSDI_GW_PROV_PRI_SESSION:
              #ifdef CM_GW_SUPPORTED
              /* When GW app is provisioned, set the GW subscription status*/
              gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
              #endif /* defined(WCDMA) || defined(GSM) */
              break;

            #ifdef FEATURE_MMODE_DUAL_SIM
            case MMGSDI_GW_PROV_SEC_SESSION:
              #if defined(FEATURE_MMODE_SXLTE_G)
              if (cmph_is_sxlte())
              {
                hybr_3_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
              }
              else
              #endif
              {
                hybr_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
              }
              break;
            #endif

            #ifdef FEATURE_MMODE_TRIPLE_SIM
              case MMGSDI_GW_PROV_TER_SESSION:
              #ifdef CM_GW_SUPPORTED
              /* When GW app is provisioned, set the GW subscription status*/
              hybr_3_gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;
              #endif /* defined(WCDMA) || defined(GSM) */
              break;
            #endif

            default:
              CM_ERR_1("Unhandled sess_info.session_type %d", sess_info.session_type);
              break;
          }

          /* If any of the cellular subscription (CDMA or GW) need to be
          ** made available, then only call subscription available API
          */
          if( gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
              hybr_gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
              hybr_3_gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
              cdma_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE
             )/*lint !e774 */
          {
            if( !cm_ph_cmd_perso_info_available( NULL,
                                                    NULL,
                                                    CM_CLIENT_ID_ANONYMOUS,
                                                    cdma_subs_status,
                                                    gw_subs_status,
                                                    hybr_gw_subs_status,
                                                    hybr_3_gw_subs_status
                                                  )
               )
            {
              CM_ERR_3("perso subs a CDMA %d, GW %d, Hybr GW %d", cdma_subs_status,
                             gw_subs_status, hybr_gw_subs_status);
              #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
              CM_ERR_1("perso subs a Hybr GW3 %d", hybr_3_gw_subs_status);
              #endif
            }
            else
            {
              CM_MSG_HIGH_3("perso subs a CDMA %d, GW %d, Hybr GW %d", cdma_subs_status,
                                  gw_subs_status, hybr_gw_subs_status);
              #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
              CM_MSG_HIGH_1("perso subs a Hybr GW3 %d", hybr_3_gw_subs_status);
              #endif
            }

          }
        }
        break;

      default:
        CM_MSG_HIGH_1("perso status %d", event->data.perso.status);
        break;
      }

      break;

    #ifdef FEATURE_MMGSDI_SESSION_LIB
    case MMGSDI_SUBSCRIPTION_READY_EVT:
    {
      cm_mmgsdi_error_e_type   cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;

      CM_MSG_HIGH_3("MMGSDI_SUBSCRIPTION_READY_EVT, slot %d app type %d, act_status %d",
                  event->data.subscription_ready.app_info.slot,
                  event->data.subscription_ready.app_info.app_data.app_type,
                  event->data.subscription_ready.activation_status);
      
      cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_update_app_info_for_id(
                        event->session_id,
                        &event->data.subscription_ready.app_info); 
      if( CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
      {
        CM_MSG_HIGH_1("Session ID not found: %d",event->session_id);
        break;
      }      
      cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_update_act_status_for_id(
                        event->session_id,
                        event->data.subscription_ready.activation_status);
      if( CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
      {
        CM_MSG_HIGH_1("Session ID not found: %d",event->session_id);
        break;
      }
      cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_ssn_info_for_id(event->session_id,
                                                            &ssn_info);
      if( CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
      {
        CM_MSG_HIGH_1("Session ID not found: %d",event->session_id);
        break;
      }

      cmph_set_received_card_init_completed_evt( TRUE );

      #ifdef FEATURE_MMODE_DUAL_SIM
      {
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

       /* Since we have all the info for the subscription
       ** Queue a generic command to CMPH to process the subscription Info
       */
       cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();
        /* Set the fields of the generic report to CM
        */
       cm_generic_rpt_ptr->hdr.cmd = CM_MMGSDI_CARD_RPT;
       cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt.evt =
                                                                                     MMGSDI_SUBSCRIPTION_READY_EVT;
       cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt.client_id =
                                                   event->client_id;
       cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt.session_id =
                                                     event->session_id;

       cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_app_info_for_id(
        event->session_id,
        &(cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt.data.subscription_ready.app_info));
       
       cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt.data.subscription_ready.activation_status = 
        event->data.subscription_ready.activation_status;

       if( CM_MMGSDI_SUCCESS == cm_mmgsdi_status )
       {
        cm_generic_rpt(cm_generic_rpt_ptr);
        CM_MSG_HIGH_0("cm_generic_rpt sent!!!!");
       }
       else
       {
        CM_ERR_1("MMGSDI app_info read fail session id %d cm_generic_rpt not sent",
                           event->session_id);/*lint !e641 */
       }

      }
      #else
      cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();
        mmgsdi_rpt_ptr = &(cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt);

        cm_generic_rpt_ptr->hdr.cmd     = CM_MMGSDI_CARD_RPT;
        mmgsdi_rpt_ptr->evt             = event->evt;
        mmgsdi_rpt_ptr->client_id       = event->client_id;
        mmgsdi_rpt_ptr->session_id      = event->session_id;

        /* Copy the subscription info */
        *(&(mmgsdi_rpt_ptr->data.subscription_ready)) = *(&(event->data.subscription_ready));

        cm_generic_rpt(cm_generic_rpt_ptr);
      #endif /* FEATURE_MMODE_DUAL_SIM */

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_MMODE_RPM_TDSCDMA
      CM_MSG_HIGH_0("MMGSDI_SUBSCRIPTION_READY_EVT send to CMRPM_TDS");
      cmrpm_tds_sim_evt_proc(CMRPM_SIM_SUBSCRIPTION_READY);
      #endif
    }
    break;
    case MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT:
    {

      CM_MSG_HIGH_1("MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT, session id: %d",
                  event->data.illegal_session.session_id);

      if( (cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal()) != NULL)
      {
        mmgsdi_rpt_ptr = &(cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt);

        cm_generic_rpt_ptr->hdr.cmd     = CM_MMGSDI_CARD_RPT;
        mmgsdi_rpt_ptr->evt             = event->evt;
        mmgsdi_rpt_ptr->client_id       = event->client_id;
        mmgsdi_rpt_ptr->session_id      = event->session_id;

        /* Copy the subscription info */
        *(&(mmgsdi_rpt_ptr->data.legal_session)) = *(&(event->data.legal_session));

        cm_generic_rpt(cm_generic_rpt_ptr);
      }

    }
    break;
    case MMGSDI_SIM_BUSY_EVT:
    {

      CM_MSG_HIGH_2("SIMBUSY: MMGSDI_SIM_BUSY_EVT, slot: %d, sim_busy:%d",
                  event->data.sim_busy.slot,event->data.sim_busy.sim_busy);

      if( (cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal()) != NULL)
      {
        mmgsdi_rpt_ptr = &(cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt);

        cm_generic_rpt_ptr->hdr.cmd     = CM_MMGSDI_CARD_RPT;
        mmgsdi_rpt_ptr->evt             = event->evt;
        mmgsdi_rpt_ptr->client_id       = event->client_id;

        mmgsdi_rpt_ptr->data.sim_busy.sim_busy = event->data.sim_busy.sim_busy;
        mmgsdi_rpt_ptr->data.sim_busy.slot     = event->data.sim_busy.slot;

        cm_generic_rpt(cm_generic_rpt_ptr);
      }

    }
    break;
    #endif /* FEATURE_MMGSDI_SESSION_LIB */

    #ifdef FEATURE_GSTK
    case MMGSDI_REFRESH_EVT:
      {
        CM_MSG_HIGH_3("SIM_REF: MMGSDI_REFRESH_EVT, mode %d stage %d, slot %d",
                         event->data.refresh.mode, event->data.refresh.stage, event->data.refresh.slot);

        if ( event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT ||
             event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN  ||
             event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START
           )
        {
          #if ((defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE))

          cmd_ptr = cm_get_cmd_buf();
          if (cmd_ptr == NULL)
          {
            sys_err_fatal_null_ptr_exception();
          }

          /* Fill in command type */
          cmd_ptr->hdr.cmd                           = CM_MMGSDI_REFRESH_FCN_IND;
          cmd_ptr->cmd.card_status_mmgsdi.evt        = event->evt;
          cmd_ptr->cmd.card_status_mmgsdi.client_id  = event->client_id;
          #ifdef FEATURE_MMGSDI_SESSION_LIB
          cmd_ptr->cmd.card_status_mmgsdi.session_id = event->session_id;
          #endif /* FEATURE_MMGSDI_SESSION_LIB */

          /* Copy data to command buffer */
          *(&(cmd_ptr->cmd.card_status_mmgsdi.data.refresh)) = *(&(event->data.refresh));

          if ((event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START ||
               event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
               &&
              (event->data.refresh.mode  == MMGSDI_REFRESH_NAA_FCN ||
               event->data.refresh.mode  == MMGSDI_REFRESH_NAA_INIT_FCN ||
               event->data.refresh.mode  == MMGSDI_REFRESH_3G_SESSION_RESET )
             )
          {
            if (event->data.refresh.refresh_files.num_files > 0)
            {
              /* Allocate memory to file list ptr */
              cmd_ptr->cmd.card_status_mmgsdi.data.refresh.refresh_files.file_list_ptr =
                (mmgsdi_file_enum_type *)cm_mem_malloc
                   (event->data.refresh.refresh_files.num_files * sizeof(mmgsdi_file_enum_type));

              memscpy( cmd_ptr->cmd.card_status_mmgsdi.data.refresh.refresh_files.file_list_ptr,
                       (event->data.refresh.refresh_files.num_files * sizeof(mmgsdi_file_enum_type)),
                       event->data.refresh.refresh_files.file_list_ptr,
                       event->data.refresh.refresh_files.num_files * sizeof(mmgsdi_file_enum_type));
            }
          }

          /* Put message in destination task queue */
          cm_put_cmd(cmd_ptr);

          #elif (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
          {
            mc_rpt_ptr = cm_mc_rpt_get_buf_else_err_fatal();

            mc_rpt_ptr->voting_mmgsdi.hdr.cmd = CM_MMGSDI_REFRESH_FCN_IND_F;

            *(&(mc_rpt_ptr->voting_mmgsdi.refresh)) = *(&(event->data.refresh));

            #ifdef FEATURE_MMGSDI_SESSION_LIB
            mc_rpt_ptr->voting_mmgsdi.session_id = event->session_id;
            #endif /* FEATURE_MMGSDI_SESSION_LIB */

            /* Queue the report to CM */
            cm_mc_rpt(mc_rpt_ptr);
          }
          #endif  /* !defined CM_GW_SUPPORTED || defined (FEATURE_CM_LTE)). */
        }
        #if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH)
        else if(event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH)
        {
          cmmmgsdi_proceed_with_refresh_stage_deactivate(event->session_id);
        }
        #endif
        else
        {
          CM_MSG_HIGH_0("SIM_REF: Unhandled mode stage combination for refresh");
        }
      }
      break;
    #endif  /* FEATURE_GSTK */

    default:
      CM_MSG_HIGH_1("unsupported event: %d", event->evt);
      break;
  } // switch ( event->evt )

}  /* cmmmgsdi_card_status_cb() */

#ifdef FEATURE_MMGSDI_SESSION_LIB
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
void  cm_mmgsdi_session_open_cb(

      mmgsdi_return_enum_type status,
        /* session open status */

      mmgsdi_cnf_enum_type    cnf,
        /* callback confirmation */

      const mmgsdi_cnf_type   *cnf_ptr
        /* callback */
)
{
  cm_generic_rpt_s_type *cm_generic_rpt_ptr;
    /* Needed to handle card events and PIN events */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(cnf_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (MMGSDI_SUCCESS != status || MMGSDI_SESSION_OPEN_CNF !=  cnf)
  {
    CM_ERR_2("MMGSDI fail session open. Status %d, Confirmation %d",
           status, cnf);/*lint !e641 */
  }
  else
  {
    cm_mmgsdi_error_e_type          cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check if the session_id is available in CM local DB */
    cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_update_id_for_type(
                       (mmgsdi_session_type_enum_type)
                         cnf_ptr->session_open_cnf.response_header.client_data,
                         cnf_ptr->session_open_cnf.response_header.session_id);
    if( CM_MMGSDI_SUCCESS == cm_mmgsdi_status)
    {
      CM_MSG_HIGH_2("MMGSDI session opened: session type %d, session id %d",
                  cnf_ptr->session_open_cnf.response_header.client_data,
                  cnf_ptr->session_open_cnf.response_header.session_id);

      cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();
        /* Create the report */
        cm_generic_rpt_ptr->hdr.cmd = CM_MMGSDI_SESSION_OPEN_RPT;
        cm_generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt.session_id =
                          cnf_ptr->session_open_cnf.response_header.session_id;

        /* Queue the report internally */
        cm_generic_rpt(cm_generic_rpt_ptr);
      }
      else
      {
      CM_ERR_2("MMGSDI fail session open unknown session type %d, session id %d",
                    cnf_ptr->session_open_cnf.response_header.client_data,
                    cnf_ptr->session_open_cnf.response_header.session_id);/*lint !e641 */

    }
  } /* MMGSDI_SUCCESS || MMGSDI_SESSION_OPEN_CNF*/

} /* cm_mmgsdi_session_open_cb */


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

)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;

  mmgsdi_status = mmgsdi_session_open(cm_mmgsdi_get_client_id(),
                                       cm_mmgsdi_session_type,
                                       0,
                                       cmmmgsdi_card_status_cb,
                                       FALSE,
                                       cm_mmgsdi_session_open_cb,
                                       (mmgsdi_client_data_type) cm_mmgsdi_session_type);

  /* This should not happen, raise an error */
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    CM_ERR_2( "MMGSDI CM unable to open session: Type %d STATUS %d",
               cm_mmgsdi_session_type, mmgsdi_status);
  }
} /* cm_mmgsdi_open_session */


#if 0
/*===========================================================================
FUNCTION CMMMGSDI_EVENT_REG_CB

DESCRIPTION
  Callback function from MMGSDI to acknowledge the CM registration for events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -esym(715,cnf)*/
/*lint -esym(715,cnf_ptr)*/
static void cmmmgsdi_event_reg_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type     *cnf_ptr
  )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (status != MMGSDI_SUCCESS)
  {
    CM_ERR_2("MMGSDI fail reg CM evnt handler. Status %d, Confirmation %d",
                  status, cnf);/*lint !e641 */
  }
}  /* cmmmgsdi_event_reg_cb() */
/*lint +esym(715,cnf)*/
/*lint +esym(715,cnf_ptr)*/

#endif
/*===========================================================================
FUNCTION CM_MMGSDI_IS_REG_SUCCESSFUL

DESCRIPTION
  Internal helper function to check status of registration returned via callback.

DEPENDENCIES
  mmgsdi_client_id_reg() or mmgsdi_client_id_and_evt_reg() called to initiate CB.

RETURN VALUE
  TRUE if registration was successful
  FALSE otherwise

SIDE EFFECTS
  Error fatal if registration failed
===========================================================================*/
static boolean cm_mmgsdi_is_reg_successful( mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr )
{
   if (status != MMGSDI_SUCCESS ||

#ifdef FEATURE_MMGSDI_SESSION_LIB
      cnf != MMGSDI_CLIENT_ID_AND_EVT_REG_CNF ||
#else
      cnf != MMGSDI_CLIENT_ID_REG_CNF ||
#endif /* FEATURE_MMGSDI_SESSION_LIB */

      cnf_ptr == NULL)
   {
      CM_ERR_FATAL_2("MMGSDI failed registering CM. Status %d, Confirmation %d",
         (uint8) status, (int)cnf);
   }

  return TRUE;

} /* cm_mmgsdi_is_reg_successful */


/*===========================================================================
FUNCTION CM_MMGSDI_STORE_CLIENT_ID

DESCRIPTION
  Internal helper function that stores the client ID

DEPENDENCIES
  none

SIDE EFFECTS
  Updates stored client ID
===========================================================================*/
static void   cm_mmgsdi_store_client_id(

       const  mmgsdi_cnf_type *cnf_ptr

)
{
  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_jcdma_info */
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_client_id = *(&(cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id));
  #else
  cm_mmgsdi_client_id = *(&(cnf_ptr->client_id_reg_cnf.response_header.client_id));
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());

  CM_MSG_HIGH_1("MMGSDI reg. CM with Client id : cm_mmgsdi_client_id %d",
                 cm_mmgsdi_get_client_id());

} /* cm_mmgsdi_store_client_id() */

/*===========================================================================
FUNCTION CM_MMGSDI_SESSION_CLIENT_REG_CB

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
)
{
  if (cm_mmgsdi_is_reg_successful(status, cnf, cnf_ptr) )
  {
    cm_mmgsdi_store_client_id(cnf_ptr);
  }
} /* cm_mmgsdi_session_client_id_reg_cb */
#endif /*  FEATURE_MMGSDI_SESSION_LIB */


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
cmmmgsdi_s_type  *cmmmgsdi_ptr( void )
{

  static cmmmgsdi_s_type cmmmgsdi_local;
    /* The one and only phone object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmmmgsdi_local;

} /* cmph_ptr() */


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
void cmmmgsdi_timer_proc(void)
{
  cmmmgsdi_s_type           *mmgsdi_ptr = cmmmgsdi_ptr();
      /* Point at MMGSDI object */
  dword                  uptime = time_get_uptime_secs();
      /* Get the current uptime */
  mmgsdi_return_enum_type    status;
  mmgsdi_cnf_type*           gsdi_cnf = (mmgsdi_cnf_type*)cm_mem_malloc(sizeof(mmgsdi_cnf_type));
  uint8 index =0 ;
  for(index=0;index<CMCALL_MAX_CALL_OBJ+1;index++)
  {
    if(mmgsdi_ptr->mmgsdi_read_req_uptime[index] == NULL)
    {
      continue;
  }

    if(mmgsdi_ptr->mmgsdi_read_req_uptime[index]->uptime <= uptime)
  {

      /*since MMGSDI  didnt respond in stipulated time ,
          **fake the response as an error , so that CM would continue */
      if(mmgsdi_ptr->sim_busy_status.is_sim_busy == TRUE)
    {
      status = MMGSDI_SIM_BUSY;
    }
    else
    {
      status = MMGSDI_ERROR;
    }
      gsdi_cnf->response_header.response_type       = MMGSDI_READ_CNF;
      gsdi_cnf->response_header.client_data = (uint32)index;
      gsdi_cnf->read_cnf.response_header.mmgsdi_status = status;

      mmgsdi_ptr->mmgsdi_read_req_uptime[index]->response_cb_ptr(status,
                             MMGSDI_READ_CNF,
                             gsdi_cnf);
  }
  }

  if(mmgsdi_ptr->read_als_uptime.uptime
      && mmgsdi_ptr->read_als_uptime.uptime <= uptime)
  {
    /*since MMGSDI  didnt respond in stipulated time ,
    **fake the response as an error , so that CM would continue */
    if(cmmmgsdi_ptr()->sim_busy_status.is_sim_busy == TRUE)
    {
      status = MMGSDI_SIM_BUSY;
    }
    else
    {
      status = MMGSDI_ERROR;
    }
    gsdi_cnf->response_header.response_type       = MMGSDI_READ_CNF;
    gsdi_cnf->read_cnf.access.file.file_enum      = mmgsdi_ptr->read_als_uptime.client_data;
    gsdi_cnf->response_header.client_data       = mmgsdi_ptr->read_als_uptime.client_data;
    gsdi_cnf->read_cnf.response_header.mmgsdi_status = status;

    mmgsdi_ptr->read_als_uptime.response_cb_ptr(status,
                           MMGSDI_READ_CNF,
                           gsdi_cnf);
  }

  cm_mem_free(gsdi_cnf);
}

/*===========================================================================
FUNCTION CMMMGSDI_INIT

DESCRIPTION
  Initilize CM MGSDI interface.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_init(void)
{
  mmgsdi_return_enum_type                     mmgsdi_status = MMGSDI_SUCCESS;
  int                                         index;
  cmmmgsdi_s_type                             *mmgsdi_ptr=cmmmgsdi_ptr();


  memset(mmgsdi_ptr,0,sizeof(*mmgsdi_ptr));

  /* timer for MMGSDI operations*/
  rex_def_timer(&mmgsdi_ptr->mmgsdi_abort_timer, get_cm_tcb(), CM_MMGSDI_ABORT_TIMER_SIG);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Register the MMGSDI event callback function */
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_status = mmgsdi_client_id_and_evt_reg(cmmmgsdi_card_status_cb,
                                               cm_mmgsdi_session_client_id_reg_cb,
                                               0 );
  #endif /*  FEATURE_MMGSDI_SESSION_LIB */

  /* This should not happen, raise an error */
   //lint -e{774} : suppress Boolean within 'if' always evaluates to False
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    CM_ERR_1( "MMGSDI CM reg cmd failed : STATUS %d",
                mmgsdi_status);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize the Session-Id table
  */
  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {
    cm_mmgsdi_session_id_table[index].asubs_id = SYS_MODEM_AS_ID_NONE;
    cm_mmgsdi_session_id_table[index].session_id = 0;
    memset(&(cm_mmgsdi_session_id_table[index].app_info), 0,
           sizeof(cm_mmgsdi_session_id_table[index].app_info));
    memset(cm_mmgsdi_session_id_table[index].uicc_id, 0,
           CM_MMGSDI_MAX_UICC_LEN);
    cm_mmgsdi_session_id_table[index].current_state = CMMMGSDI_STATE_NONE;
    cm_mmgsdi_session_id_table[index].act_status = MMGSDI_ACT_STATUS_UNKNOWN;
  }


  /* Initialize ICCID Table */
  memset(&cm_mmgsdi_iccid_table[0], 0,sizeof(cm_mmgsdi_iccid_table));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}  /* cmmmgsdi_init() */

#ifdef FEATURE_GSTK
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

)
{
  mmgsdi_refresh_file_list_type    file_info;
    /* Structure holds information about SIM files CM interacts with */

  int                  file_list       = 0;
    /* counter */

  uint32               cm_file_list    = 0;
    /* counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear file info */
  memset (&file_info, 0, sizeof(file_info));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the SIM files that CM works on
  */
  cmmmgsdi_get_files_for_refresh (&file_info);

  for (file_list=0; file_list< file_list_len ; ++file_list)
  {

    for (cm_file_list=0; cm_file_list< file_info.num_files; ++cm_file_list)
    {
      /* Match detected between given list and CM's list, given file is
      ** being used by CM
      */
      if (file_list_p[file_list] == file_info.file_list_ptr[cm_file_list])
      {
        CM_MSG_HIGH_1( "SIM_REF: File %d refreshed",file_list_p[file_list] );
        return TRUE;
      }

    } /* for (cm_file_list= ... */

  } /* for (file_list=0 ... */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* No match detected */
  return FALSE;

} /* cm_gsdi_is_refresh_list_used_by_cm () */

#endif /* #ifdef FEATURE_GSTK */

#ifdef CM_GW_SUPPORTED
/*===========================================================================
FUNCTION CMMMGSDI_RAT_MODE_INIT

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
)
{

  cmmmgsdi_rat_mode_preference_e_type new_mode_pref =
                                             MMGSDI_RAT_MODE_PREF_NONE;
  boolean                             is_lte_capable = FALSE;
  mmgsdi_app_enum_type                app_type;

  cmph_s_type                         *ph_ptr = cmph_ptr();

  if (ph_ptr->is_rat_bal == FALSE)
  {
    /* Return from here because rat balancing is not active */
    *mode_pref = CM_MODE_PREF_NONE;
  }

  /* Read SIM card mode.
   ** It should be USIM for EF_RAT.
   */
  app_type = cm_mmgsdi_ssn_tbl_get_app_type_for_type(
                                                  MMGSDI_GW_PROV_PRI_SESSION);

  if ( app_type != MMGSDI_APP_USIM )
  {
    CM_MSG_HIGH_1( "Invalid app type %d, rat mode init falis",
                   app_type );

    /* Return from here. App type is not USIM */
    *mode_pref = CM_MODE_PREF_NONE;
    return;
  }

  if (!cmmmgsdi_read(MMGSDI_GW_PROV_PRI_SESSION,
                     MMGSDI_USIM_7F66_PROP1_RAT,
                     0,
                     1,
                     (uint8*)&new_mode_pref,
                     sizeof(new_mode_pref),
                     transparent
                     ))
  {
    CM_MSG_HIGH_0("Unable to read MMGSDI_USIM_7F66_PROP1_RAT");

    /* Make new_mode_pref to default value */
    new_mode_pref = MMGSDI_RAT_MODE_PREF_NONE;
  }
  else
  {
    /* Reading of EF_RAT file is success */
    new_mode_pref = (cmmmgsdi_rat_mode_preference_e_type)
                          (((byte)new_mode_pref) & 0xff);
    CM_MSG_HIGH_1(" mode_pref in MMGSDI_USIM_7F66_PROP1_RAT: %d",
                  new_mode_pref);

    /* If EF_RAT mode_pref is not AUTO, GSM, WCDMA or LTE
    ** (between 0x05 to 0xFF),make it Dual or AUTOMATIC based
    ** on LTE capability of the phone.
    */

    #ifdef FEATURE_CM_LTE
    /* Check if the UE supports LTE */
    if ( !SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&ph_ptr->lte_band_capability))
    {
      is_lte_capable = TRUE;
      CM_MSG_HIGH_0 ("lte capable");
    }
    #endif

    if (is_lte_capable == TRUE)  /*lint !e774 */
    {
      if ( !BETWEEN (new_mode_pref, MMGSDI_RAT_MODE_PREF_NONE,
                   MMGSDI_RAT_MODE_PREF_MAX))
      {
        CM_MSG_HIGH_1("EF_RAT mode_pref changed to AUTOMATIC from %d",
                      new_mode_pref);
        new_mode_pref = MMGSDI_RAT_MODE_PREF_AUTOMATIC;
      }
    }
    else
    {
      if ( !BETWEEN (new_mode_pref, MMGSDI_RAT_MODE_PREF_NONE,
                     MMGSDI_RAT_MODE_PREF_AUTOMATIC))
      {
        CM_MSG_HIGH_1("EF_RAT mode_pref changed to GSM_WCDMA from %d",
                    new_mode_pref);
        new_mode_pref = MMGSDI_RAT_MODE_PREF_GSM_WCDMA;
      }
    }
  }

  /* Map the EF_RAT mode to CM mode_pref */
  switch(new_mode_pref)
  {
    case MMGSDI_RAT_MODE_PREF_GSM_WCDMA:
      *mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY;
      break;

    case MMGSDI_RAT_MODE_PREF_GSM_ONLY:
      *mode_pref = CM_MODE_PREF_GSM_ONLY;
      break;

    case MMGSDI_RAT_MODE_PREF_WCDMA_ONLY:
      *mode_pref = CM_MODE_PREF_WCDMA_ONLY;
      break;

    case MMGSDI_RAT_MODE_PREF_AUTOMATIC:
      *mode_pref = CM_MODE_PREF_GWL;
      break;

    case MMGSDI_RAT_MODE_PREF_LTE_ONLY:
      *mode_pref = CM_MODE_PREF_LTE_ONLY;
      break;

    default:
      /* Invalid mode_pref */
      *mode_pref = CM_MODE_PREF_NONE;

  }
}
#endif /* CM_GW_SUPPORTED */


#if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_GSTK)
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
)
{
  mmgsdi_refresh_file_list_type  file_info;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;

  /* Clear file info */
  memset (&file_info, 0, sizeof(file_info));

  /* Get the SIM files that CM works on
  */
  cmmmgsdi_get_files_used_incall(&file_info);

  #ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
      mmgsdi_status = mmgsdi_session_register_for_refresh_ok_to_proceed_voting(
                                  mmgsdi_card_rpt->session_id,
                                  file_info,
                                  TRUE,
                                  TRUE,
                                  cmmmgsdi_generic_cb,
                                  0
                                  );
  #else
  mmgsdi_status = mmgsdi_session_register_for_refresh(
                              mmgsdi_card_rpt->session_id,
                              file_info,
                              TRUE,
                              cmmmgsdi_generic_cb,
                              0
                              );
  #endif

  /* This should not happen, raise an error */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* MMGSDI can return MMGSDI_ERROR during power down state.
       We log ERR here instead of ERR_FATAL and let MMGSDI handle
       all other critical error internally
    */
    CM_ERR_1( "MMGSDI CM reg for refsh evnt cmd failed : STATUS %d",
    mmgsdi_status);
  }
}


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
void  cmmmgsdi_get_card_info(mmgsdi_available_apps_type *card_info)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;

  CM_ASSERT(card_info != NULL);

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_status =     mmgsdi_get_all_available_apps_sync(
      cm_mmgsdi_get_client_id(),
      card_info);


    /* This should not happen, raise an error */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* MMGSDI can return MMGSDI_ERROR during power down state.
       We log ERR here instead of ERR_FATAL and let MMGSDI handle
       all other critical error internally
    */
    CM_ERR_1( "MMGSDI mmgsdi_get_all_available_apps_sync cmd failed : STATUS %d",
    mmgsdi_status);
  }
  #else
  SYS_ARG_NOT_USED(mmgsdi_status);
  #endif

}

#endif /* FEATURE_MMGSDI_SESSION_LIB && FEATURE_GSTK */

#if defined(FEATURE_MMGSDI_SESSION_LIB)
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
void cmmmgsdi_update_session_state(
    mmgsdi_session_id_type   session,
    cmmmgsdi_session_state_type  state
)
{

  cm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;
  CM_MSG_HIGH_2("oprt_mode_online: update state->%d, session->%d",state,session);

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */

  ssn_entry = cm_mmgsdi_ssn_tbl_get_entry_by_id(session);
  if(ssn_entry != NULL)
  {
    CM_MSG_HIGH_0("oprt_mode_online: updating state");
    ssn_entry->current_state = state;
  }

  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());
}

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
uint8 cmmmgsdi_count_given_state(cmmmgsdi_session_state_type  state)
{

  int           index = 0;
  uint8         stateCount = 0;

  rex_enter_crit_sect(cmmmgsdi_crit_sect_ptr());
  /* Serialize data access cm_mmgsdi_session_id_table[] */
  for(index = 0; index < ARR_SIZE(cm_mmgsdi_session_id_table); index++)
  {

    if (cm_mmgsdi_session_id_table[index].current_state == state)
    {
      stateCount++;
    }
  }
  rex_leave_crit_sect(cmmmgsdi_crit_sect_ptr());
  CM_MSG_HIGH_1("oprt_mode_online: cmmmgsdi_count_given_state, count %d",stateCount);
  return stateCount;
}
#endif

